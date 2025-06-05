/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>

#include "framework/Dispatcher.h"
#include "framework/Log.h"
#include "framework/MessageQueue.h"
#include "framework/Module.h"
#include "framework/PolicyManager.h"
#include "framework/message_id.h"
#include "framework/UnSolicitedMessage.h"

#define RIL_likely(x) (__builtin_expect(!!(x), true))
#define RIL_unlikely(x) (__builtin_expect(!!(x), false))

using std::lock_guard;
using Lock = qtimutex::QtiRecursiveMutex;
using ScopedLock = std::unique_lock<Lock>;

#define TAG "RILQ Dispatcher"

bool Dispatcher::is_handle_valid(message_id_ref id)
{
    bool ret = false;

    if (!id.m_name.empty() && id.idx > 0 && id.idx < mMessageHandlers.size()) {
        message_id_ref internal= mMessageHandlers[id.idx].first;
        if (internal == id ) {
            ret = true;
            return ret;
        }
    }

    QCRIL_LOG_DEBUG("%s: message_id: %p", __func__, &id);
    return ret;
}

void Dispatcher::dump_message_id(message_id_ref id) {
      std::string module_list_str;
      ModuleList& ml = mMessageHandlers[id.idx].second;
      for (Module* m : ml) {
          if (m) {
              module_list_str += m->to_string()+", ";
          }
      }
      QCRIL_LOG_DEBUG("%s: id->m_name %s. message_id: %p registered modules = %s", __func__, id.get_name().c_str(), &id, module_list_str.c_str());
}

void Dispatcher::forwardMsgToModule(std::shared_ptr<Message> msg) {
  ScopedLock lock(mMutex);

  if (is_handle_valid(msg->m_h)) {
     ModuleList& ml = mMessageHandlers[msg->m_h.idx].second;
    /* Allow broadcast only for UnSolicitedMessage.
       Messages having next_hop as a module other than Dispatcher are exception
       to this rule. Message would be forwarded to the next_hop irrespective of multiple
       registered recipients.  TODO: Clients should fix their code to have cleaner interface.*/
    Module* next_hop_module = msg->get_next_hop();
    if (!next_hop_module) {
      Log::getInstance().d("[DispatcherModule]: next_hop_module is nullptr for msg = " + msg->to_string() + "!!");
      next_hop_module = mModule;
    }
    if (ml.size() > 0) {
        if (ml.size() > 1) {
          if (next_hop_module == mModule
              && msg->getMessageType() != Message::MessageType::UnSolicitedMessage) {
            //Broadcast not allowed for non-broadcast type messages.
            Log::getInstance().d("[DispatcherModule]: Broadcast not allowed for non-broadcast type msg = " +
              msg->to_string());
            informMessageDispatchFailure(msg,
              Message::Callback::Status::BROADCAST_NOT_ALLOWED);
            return;
          }
        }
        std::vector<Module*> deliver_list;
        for (Module* m : ml) {
          bool attempt_delivery = false;

          if (next_hop_module == mModule) {
            /* next_hop is dispatcher module. Deliver the msg to all registered modules */
            attempt_delivery = true;
          } else if (next_hop_module == m) {
            /* next_hop is a pre-identified recepient. Deliver the msg to only that module */
            attempt_delivery = true;
          }

          if (attempt_delivery) {
            if (!m->hasValidLooper()) {
                deliver_list.push_back(m);
            }
          }
        }

        lock.unlock();
        for (Module* module : deliver_list) {
          if(nullptr != module)
          {
            Log::getInstance().d("[DispatcherModule]: Forwarding msg = " +
                                 msg->to_string() + " to module = " +
                                 module->to_string());
            module->onMessageArrival(msg);
          }
        }
    } else {
        Log::getInstance().d("[DispatcherModule]: No handler registered for msg = " +
                             msg->to_string());
        informMessageDispatchFailure(msg,
                                     Message::Callback::Status::NO_HANDLER_FOUND);

    }
  } else {
    Log::getInstance().d("[DispatcherModule]: No handler registered for msg = " +
                         msg->to_string());
    informMessageDispatchFailure(msg,
                                 Message::Callback::Status::NO_HANDLER_FOUND);
  }
}

/* this is called from Dispatcher context */
void Dispatcher::informMessageDispatchFailure(
    std::shared_ptr<Message> msg, Message::Callback::Status status) {
  msg->informDispatchFailure(msg, status);
  /* Clear out the restriction.*/
  std::shared_ptr<Restriction> restriction_sharedPtr =
      PolicyManager::getInstance().getMessageRestriction(msg);
  restriction_sharedPtr->onMessageCallbackCompletion(msg);
}

TimeKeeper::timer_id Dispatcher::setTimeoutForMessage(std::shared_ptr<Message> msg, TimeKeeper::millisec maxTimeout) {
  Log::getInstance().logTime("[DispatcherModule]: set timeout for " +
                             msg->to_string());
  TimeKeeper::timer_id tid = TimeKeeper::getInstance ().set_timer(
      [this, msg](void* user_data) {
        QCRIL_NOTUSED(user_data);
        if (!(msg->isCallbackExecuting() || msg->isCallbackExecuted())) {
          Log::getInstance().logTime("[DispatcherModule]: Timer expired for " +
                                     msg->to_string());
          Log::getInstance().d(
              "[Timer]: *************************TIME-OUT for msg = " +
              msg->to_string());
          msg->cancelling();
          this->informMessageDispatchFailure(
              msg, Message::Callback::Status::TIMEOUT);

          Log::getInstance().d("Finished");
          /* Clear out the restriction.*/
          std::shared_ptr<Restriction> restriction_sharedPtr =
              PolicyManager::getInstance().getMessageRestriction(msg);
          restriction_sharedPtr->onMessageCallbackCompletion(msg);
          msg->cancelled();
        }
      },
      nullptr,
      maxTimeout);

  Log::getInstance().logTime("[" + msg->to_string() + "]: timer_id = " + std::to_string(tid));
  msg->setTimerId(tid);
  return tid;
}

TimeKeeper::timer_id Dispatcher::setTimeoutForMessage(std::shared_ptr<Message> msg, TimeKeeper::millisec maxTimeout, TimeKeeper::handler_type functor) {
  Log::getInstance().logTime("[DispatcherModule]: Set Client specific timeout for " +
    msg->to_string());
  TimeKeeper::timer_id tid = TimeKeeper::getInstance ().set_timer(functor, nullptr, maxTimeout);
  msg->setTimerId(tid);
  return tid;
}

bool Dispatcher::clearTimeoutForMessage(std::shared_ptr<Message> msg) {
  return TimeKeeper::getInstance ().clear_timer(msg->getTimerId());
}

/* This is called from module context.*/
void Dispatcher::informMessageCallbackFinished(std::shared_ptr<Message> msg) {
    /* Message transaction finishes when callback is executed.*/
    msg->callbackExecuted();
    /* Callback just finished. Cancel the timer.*/
    auto tid = msg->getTimerId();
    TimeKeeper::getInstance ().clear_timer(tid);

    /* Clear out the restriction.*/
    std::shared_ptr<Restriction> restriction_sharedPtr =
      PolicyManager::getInstance().getMessageRestriction(msg);
    restriction_sharedPtr->onMessageCallbackCompletion(msg);
}

void Dispatcher::forwardMsgToModuleSync(std::shared_ptr<Message> msg) {
  ScopedLock lock(mMutex);

  message_id_ref id = msg->get_message_id();
  ModuleList& ml = mMessageHandlers[id.idx].second;
  string msgStr = msg->to_string();
  if (msg && is_handle_valid(id)) {

    if (ml.size() > 0) {
        if (ml.size() > 1) {
          if (msg->getMessageType() != Message::MessageType::UnSolicitedMessage) {
            //Broadcast not allowed for non-broadcast type messages.
            Log::getInstance().d("[DispatcherModule]: Broadcast not allowed for non-broadcast type msg = " +
              msg->to_string());
            informMessageDispatchFailure(msg,
              Message::Callback::Status::BROADCAST_NOT_ALLOWED);
            return;
          }
        }

        if ((msg->getMessageType() == Message::MessageType::UnSolicitedMessage)
                && checkSuppressMap(std::static_pointer_cast<UnSolicitedMessage>(msg)))
            return;

        std::vector<Module*> deliver_list;
        for (Module* m : ml) {
            deliver_list.push_back(m);
        }

        lock.unlock();
        for (Module* module : deliver_list) {
          if(nullptr != module)
          {
            Log::getInstance().d("[DispatcherModule]: Forwarding SYNC msg = " +
                                 msg->to_string() + " to module = " +
                                 module->to_string());
            module->dispatchSync(msg);
          }
        }
    } else {
        Log::getInstance().d("[DispatcherModule]: No handler register for msg = " +
                             msg->to_string());
        informMessageDispatchFailure(msg,
                                     Message::Callback::Status::NO_HANDLER_FOUND);
    }
  } else {
    Log::getInstance().d("[DispatcherModule]: No handler register for msg = " +
                         msg->to_string());
    informMessageDispatchFailure(msg,
                                 Message::Callback::Status::NO_HANDLER_FOUND);
  }
}

void Dispatcher::joinAllModulesLooper() {
  lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
  /*Join all known modules loopers*/
  for (const MhPair &elem : mMessageHandlers) {
    if (elem.first.get().get_idx() < 0) {
        continue;
    }
    for (Module* m : elem.second) {
      if (m) {
        std::thread *t = m->getLooperThread();
        if (t != nullptr) {
          Log::getInstance().d("[DispatcherModule]: Waiting for looper of " + m->to_string() + " to join.");
          t->join();
          Log::getInstance().d("[DispatcherModule]: looper of " + m->to_string() + " joined.");
        }
      }
    }
  }
}

std::thread::id Dispatcher::getDispatcherLooperThreadId() {
  auto looperThread = mModule->getLooperThread();
  std::thread::id ret;
  if(looperThread){
     ret = looperThread->get_id();
  }
  return ret;
}

void Dispatcher::joinDispatcherLooper() {
  /*Join qcrilMainLooper*/
  std::thread *t = mModule->getLooperThread();
  mModule->killLooper();
  if (t != nullptr) {
      t->join();
  }
}

void Dispatcher::dispatch(std::shared_ptr<Message> msg) {
  if (!msg) return;
  bool isNeededToEnqueue = false;
  std::shared_ptr<Restriction> restriction_sharedPtr =
    PolicyManager::getInstance().getMessageRestriction(msg);
  Message::Callback::Status status = Message::Callback::Status::SUCCESS;

  do {
    if (restriction_sharedPtr->isDispatchAllowed(msg)) {
      ScopedLock lock(mMutex);
      if (msg && is_handle_valid(msg->m_h)) {
        message_id_ref id = mMessageHandlers[msg->m_h.idx].first;
        ModuleList& ml = mMessageHandlers[id.idx].second;
        if (ml.size() > 0) {
          Module* next_hop_module = msg->get_next_hop();
          if (!next_hop_module) {
            Log::getInstance().d("[dispatch function]: next_hop_module is nullptr for msg = " + msg->to_string() + "!!");
            next_hop_module = mModule;
          }

          if((next_hop_module == mModule) && (ml.size() > 1) && (msg->getMessageType() !=
              Message::MessageType::UnSolicitedMessage)) {
            //Broadcast not allowed for non-broadcast type messages.
            status = Message::Callback::Status::BROADCAST_NOT_ALLOWED;
            break;
          }

          for (Module* m : ml) {
              bool attempt_delivery = false;
              if (next_hop_module == mModule) {
                /* next_hop is dispatcher module. Deliver the msg to all registered modules */
                attempt_delivery = true;
              } else if (next_hop_module == m) {
                /* next_hop is a pre-identified recepient. Deliver the msg to only that module */
                attempt_delivery = true;
              }
              if (attempt_delivery) {
                if(!m->hasValidLooper()) {
                  isNeededToEnqueue = true;
                } else {
                    // fwd msg to registered module which has a looper
                    m->onMessageArrival(msg);
                }
              }
          }
          lock.unlock();
        } else {
            Log::getInstance().d("[dispatch function]: No handler registered for msg = " +
                             msg->to_string());
            status = Message::Callback::Status::NO_HANDLER_FOUND;
            break;
        }
      } else {
          Log::getInstance().d("[dispatch function]: No handler registered for msg = " +
                              msg->to_string());
          status = Message::Callback::Status::NO_HANDLER_FOUND;
          break;
      }
    }
  }while(0);

  if(Message::Callback::Status::SUCCESS != status) {
    informMessageDispatchFailure(msg, status);
  }

  if(true == isNeededToEnqueue) {
    mModule->dispatch(msg);
  }
}

void Dispatcher::broadcast(std::shared_ptr<Message> msg) {
  mModule->dispatch(msg);
}

void Dispatcher::dispatchSync(std::shared_ptr<Message> msg) {
  /* SyncApiSession manages the timeout for sync calls.*/
  mModule->dispatchSync(msg);
}

Dispatcher &Dispatcher::getInstance() {
  static Dispatcher sInstance;
  // std::cout << "\nDispatcher = " << &sInstance;
  return sInstance;
}

size_t Dispatcher::getUnProcessedMessageCount() {
  return mModule->getUnProcessedMessageCount();
}

void Dispatcher::registerHandler(message_id_ref id, Module* module) {
  lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
  if (is_handle_valid(id) && module) {
    ModuleList& ml = mMessageHandlers[id.idx].second;
    auto found = std::find(ml.begin(), ml.end(), module);
    if (found == ml.end()) {
      ml.push_back(module);
    } else {
      Log::getInstance().d("module already present in the list at position: " +
                      std::to_string(found - ml.begin() + 1));
    }
  } else {
      if (module == nullptr) {
          Log::getInstance().d("Module is null");
      }
      Log::getInstance().d("Invalid handle passed");
  }
}

void Dispatcher::dump_registered_handler() {
    Log::getInstance().d("[Dispatcher]: Dump message handler: start.");
    for (auto kv = mMessageHandlers.begin(); kv != mMessageHandlers.end(); kv ++) {
        dump_message_id(kv->first);
    }
    Log::getInstance().d("[Dispatcher]: Dump message handler: finished.");
}

void Dispatcher::unRegisterHandler(Module *module) {
    //for (auto kv : mMessageHandlers) {
    lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);

    Log::getInstance().d("[Dispatcher]: unregister module = " + module->to_string());
    for (auto kv = mMessageHandlers.begin(); kv != mMessageHandlers.end(); kv ++) {
        ModuleList& registeredModuleSet = kv->second;
        registeredModuleSet.erase(std::remove_if(
                    registeredModuleSet.begin(),
                    registeredModuleSet.end(),
                    [&module] (auto &m) {
                        bool ret = module == m;
                        return ret; }
                    ),
                    registeredModuleSet.end());
    }
}

/* Unit test helper, dont use in production code*/
int Dispatcher::getRegisteredModulesCount() {
    lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
    int count = 0;
    for (const MhPair &kv : mMessageHandlers) {
        const ModuleList& registeredModuleSet = kv.second;
        count += registeredModuleSet.size();
    }
    return count;
}
/* Unit test helper, dont use in production code*/
int Dispatcher::getRegisteredMessagesCount() {
    lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
    return mMessageHandlers.size();
}
/* Unit test helper, dont use in production code*/
bool Dispatcher::isMessageRegistered(const std::string &msgName) {
    lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
    for (const MhPair &h : mMessageHandlers) {
        const std::string &name = h.first.get().get_name();
        if (name == msgName) {
            return true;
        }
    }
    return false;
}


/* Only for simulation*/
void Dispatcher::waitForLooperToConsumeAllMsgs() {
  mModule->waitForLooperToConsumeAllMsgs();
}

/*
 * This function is used to register a new message and/or return a reference to
 * an existing message_id_info. When a new message is to be registered - it uses
 * the passed in reference ID (static or dynamic) to create a new message ID and
 * sets its name and idx (position) in the message handler.
 * If the message is already registered - it simply returns a reference to its id.
 * If the message is already registered but a copy of the message ID is passed -
 * it assigns the name and idx to keep them identical.
 */

message_id_ref Dispatcher::registerMessage(const string &msg_name, message_id_ref id)
{
    if (RIL_unlikely(msg_name != id.get_name())) {
        QCRIL_LOG_DEBUG("msg_name: %s. id.get_name(): %s", msg_name.c_str(), id.get_name().c_str());
    }
    assert(msg_name == id.get_name());
    lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
    message_id_p h = nullptr;
    for (MhPair &mh : mMessageHandlers ) {
        const std::string &name = mh.first.get().get_name();
        if (name == msg_name) {
            h = &mh.first.get();
            id.idx = h->idx; // Always set the idx and name
            break;
        }
    }
    if (h == nullptr) {
        auto it = mMessageHandlers.insert(mMessageHandlers.end(), std::make_pair(std::ref(id), std::vector<Module*>()));
        h = &it->first.get();
        h->idx = mMessageHandlers.size() - 1;
    }
    QCRIL_LOG_DEBUG("%s: Registered message %s. message_id: %p (%zu)", __func__, msg_name.c_str(), h, h->idx);
    return *h;
}

/*
 * This function is used as a query to check if the message is registered with
 * the dispatcher and returns a reference to it. It returns a nullptr for
 * unregistered messages.
 */

message_id_p Dispatcher::registerMessage(const string &msg_name)
{
    lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
    message_id_p h = nullptr;
    for (MhPair &mh : mMessageHandlers ) {
        const std::string &name = mh.first.get().get_name();
        if (name == msg_name) {
            h = &mh.first.get();
            break;
        }
    }
    return h;
}


string Dispatcher::getMessageName(message_id_ref id)
{
    string ret = "";
    if (is_handle_valid(id)) {
        ret = id.get_name();
    }
    return ret;
}

bool Dispatcher::isAllMessageHandlerModulesWithLooper(std::shared_ptr<Message> msg) {
  bool allRegisteredModulesWithLooper = false;

  if (msg && is_handle_valid(msg->m_h)) {
    message_id_ref id = mMessageHandlers[msg->m_h.idx].first;
    ModuleList& ml = mMessageHandlers[id.idx].second;
    if (ml.size() > 0) {
      allRegisteredModulesWithLooper = true;
      for (Module* m : ml) {
        if (m) {
          if (!m->hasValidLooper()) {
            allRegisteredModulesWithLooper = false;
            break;
          }
        }
      }
    }
  }

  return allRegisteredModulesWithLooper;
}

void Dispatcher::suppressMessage(message_id_ref id, bool status) {
    ScopedLock lock(mMutex);

    if (status) {
        mSuppMap[id].SuppressCount++;
        lock.unlock();
    } else {
        mSuppMap[id].SuppressCount--;
        if (mSuppMap[id].SuppressCount <= 0) {
            if (mSuppMap[id].SuppressCount < 0) {
                QCRIL_LOG_WARN("unexpected: the number of unsuppressed reqs is more than that of suppressed ones");
            }
            auto list = std::move(mSuppMap[id].msgList);
            mSuppMap.erase(id);
            lock.unlock();

            for (auto msg: list) {
                forwardMsgToModuleSync(msg);
            }
        } else {
            lock.unlock();
        }
    }
}

bool Dispatcher::checkSuppressMap(std::shared_ptr<UnSolicitedMessage> msg) {
    lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);

    message_id_ref id = msg->get_message_id();
    if (mSuppMap.find(id) == mSuppMap.end())
        return false;
    if (mSuppMap[id].SuppressCount <= 0)
        return false;

    mSuppMap[id].msgList.push_back(msg);
    return true;
}
