/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <deque>
#include <set>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <functional>
#include "framework/DispatcherModule.h"
#include "framework/message_id.h"
#include "framework/Message.h"
#include "framework/MessageQueue.h"
#include "framework/Module.h"
#include "framework/TimeKeeper.h"
#include "framework/message_id.h"

using std::deque;
using std::lock_guard;

#define REG_MSG_N(name, id) Dispatcher::getInstance().registerMessage(name, id)
#define REG_MSG_Q(name) Dispatcher::getInstance().registerMessage(name)
#define REG_MSG(name) REG_MSG_N(#name, get_id_##name())

class UnSolicitedMessage;

struct SuppressInfo {
    SuppressInfo(): SuppressCount(0) {}
    int SuppressCount;
    std::vector<std::shared_ptr<UnSolicitedMessage>> msgList;
};
using SuppressMap = std::unordered_map<std::reference_wrapper<message_id_info>, SuppressInfo>;

using ModuleList = std::vector<Module*>;
using MhPair = std::pair<std::reference_wrapper<message_id_info>, ModuleList>;
using MessageHandlers = std::vector<MhPair>;

class Dispatcher {

 private:
  friend class Looper;
  Module *mModule;
  qtimutex::QtiRecursiveMutex mMutex;
  SuppressMap mSuppMap;

  /*Pair key = Message id references , Value = vector of Module pointers.*/
  MessageHandlers mMessageHandlers;
  bool is_handle_valid(message_id_ref);

  static constexpr int const &totalMsgs = 1050;

  inline static message_id_info id_Invalid{"Invalid"}; // id for invalid message at idx =0

  bool checkSuppressMap(std::shared_ptr<UnSolicitedMessage> msg);
  static void invokeCallback(std::shared_ptr<Message> msg, Message::Callback::Status status,
          std::shared_ptr<void> responseData) {
      msg->invokeCallback(msg, status, responseData);
  }

  Dispatcher();

 public:
  #ifdef HOST_EXECUTABLE_BUILD
    static constexpr int sIsHostExecutable = 1;
  #else
    static constexpr int sIsHostExecutable = 0;
  #endif
  static constexpr int MESSAGE_EXPIRY_TIMEOUT{1000 * 90};
  static constexpr int SYNC_MESSAGE_EXPIRY_TIMEOUT{ 1000 * 20 };
  ~Dispatcher();

  Dispatcher(Dispatcher const &) = delete;             // Copy construct
  Dispatcher(Dispatcher &&) = delete;                  // Move construct
  Dispatcher &operator=(Dispatcher const &) = delete;  // Copy assign
  Dispatcher &operator=(Dispatcher &&) = delete;       // Move assign

  void dispatch(std::shared_ptr<Message> msg);
  void dispatchSync(std::shared_ptr<Message> msg);
  void broadcast(std::shared_ptr<Message> msg);
  static Dispatcher &getInstance();
  void joinAllModulesLooper();
  void joinDispatcherLooper();
  void waitForLooperToConsumeAllMsgs();
  std::thread::id getDispatcherLooperThreadId();

  message_id_ref registerMessage(const string &msg_name, message_id_ref id);
  message_id_p registerMessage(const string &msg_name);
  void dump_message_id(message_id_ref id);
  void dump_registered_handler();
  string getMessageName(message_id_ref id);
  void registerHandler(message_id_ref id, Module* module);
  void unRegisterHandler(Module *module);
  void forwardMsgToModule(std::shared_ptr<Message> msg);
  void forwardMsgToModuleSync(std::shared_ptr<Message> msg);
  void informMessageCallbackFinished(std::shared_ptr<Message> msg);
  size_t getUnProcessedMessageCount();

  TimeKeeper::timer_id setTimeoutForMessage(std::shared_ptr<Message> msg, TimeKeeper::millisec maxTimeout);
  TimeKeeper::timer_id setTimeoutForMessage(std::shared_ptr<Message> msg, TimeKeeper::millisec maxTimeout, TimeKeeper::handler_type);
  bool clearTimeoutForMessage(std::shared_ptr<Message> msg);
  void informMessageDispatchFailure(std::shared_ptr<Message> msg,
                                    Message::Callback::Status status);

  /* Unit test helper, dont use in production code*/
  int getRegisteredModulesCount();
  int getRegisteredMessagesCount();
  bool isMessageRegistered(const std::string &msgName);

  inline void reset() {
      joinAllModulesLooper();
      if (mModule) {
          delete mModule;
          mModule = nullptr;
      }

      //TimeKeeper::getInstance ().wait_for_looper_to_join();
      TimeKeeper::getInstance ().kill();
      {
        lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
        mMessageHandlers.clear();
      }
  }
  bool isAllMessageHandlerModulesWithLooper(std::shared_ptr<Message> msg);
  void suppressMessage(message_id_ref id, bool status);
};

inline Dispatcher::Dispatcher() {
  mModule = new DispatcherModule;
  mModule->init();
  mMessageHandlers.reserve(totalMsgs); // reserving memory to avoid resize/reallocation
  // In order to have index 0 as an invalid marker
  registerMessage("Invalid", id_Invalid);
}

/* this is singleton. Destructor would never be called.*/
inline Dispatcher::~Dispatcher() {
}
