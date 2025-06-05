/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#define TAG "RILLooper"
#include "framework/Looper.h"
#include "framework/Module.h"
#include "framework/Util.h"
#include "framework/Log.h"
#include "framework/Dispatcher.h"

using std::condition_variable_any;
using std::lock_guard;
using std::unique_lock;
using std::thread;
using std::runtime_error;

void myLoop(Looper *instance) {
  string NTAG = instance->to_string();
  setThreadName(NTAG.c_str());
  Log::getInstance().d("\t[" + NTAG + "]: Entry");
  while (!instance->mExit) {
    {
      std::unique_lock<qtimutex::QtiSharedMutex> mylock(instance->looperMutex);
      instance->looperCv.wait(mylock,
                              [=] { return instance->looperReady == true; });
    }
    while (instance->isEmptyQueue() == false) {
      Log::getInstance().d("\t[" + NTAG + "]: Queue size = " +
                           std::to_string(instance->getSize()));
      std::shared_ptr<Message> msg = instance->getFirstMessage();
      if(msg)
      {
        string msgName = msg->to_string();
        Log::getInstance().d("\t[" + NTAG + "]: Handle msg = " + msgName);

        instance->handleMessage(msg);

        Log::getInstance().d("\t[" + NTAG + "]: Done handling msg = " + msgName);
      }
    }
    {
      std::unique_lock<qtimutex::QtiSharedMutex> mylock(instance->looperMutex);
      if (instance->isEmptyQueue()) {
        instance->looperReady = false;
      }
    }
    instance->looperCv.notify_all();
    if (Looper::FEATURE_DYNAMIC_THREAD) {
      instance->releaseThread();
      break;
    }
  }
  Log::getInstance().d("\t[" + NTAG + "]: Exiting");
}

void Looper::dispatch(std::shared_ptr<Message> msg) {
  bool acquired = acquireThread();
  if (acquired && !mExit && mMessageQueue) {
    mMessageQueue->enqueue(msg);
  } else if(!acquired) {
    std::string msgdump = msg->dump();
    QCRIL_LOG_ERROR("Unable to create thread while dispatching message %s", msgdump.c_str());
    Dispatcher::invokeCallback(msg, Message::Callback::Status::FAILURE, nullptr);
  }

  {
    lock_guard<qtimutex::QtiSharedMutex> lock(looperMutex);
    looperReady = true;
  }
  looperCv.notify_all();
}

void Looper::dispatchSync(std::shared_ptr<Message> msg) {
  handleMessageSync(msg);
}

size_t Looper::getSize() {
  if (mMessageQueue) {
   return mMessageQueue->getSize();
  }
  return 0;
}

bool Looper::isEmptyQueue() {
  if (mMessageQueue) {
    return mMessageQueue->isEmpty();
  }
  return true;
}

void Looper::dumpMessageQueue() {
  if (mMessageQueue) {
    mMessageQueue->dumpMessageQueue();
  }
}

std::shared_ptr<Message> Looper::getFirstMessage() {
  if (mMessageQueue) {
    return mMessageQueue->pop();
  }
  return nullptr;
}

void Looper::init(Module *module) {
  mModule = module;
  mName = mModule->to_string() + "-Looper";
  if (mMessageQueue) {
    mMessageQueue->init(mModule->to_string());
  }
}

bool Looper::acquireThread() {
  bool ret = false;
  std::lock_guard<qtimutex::QtiSharedMutex> lock(looperMutex);
  if (mLooperThread == nullptr && mExit == false) {
    int attempt = 1;
    for (; mLooperThread == nullptr && mExit == false &&
            attempt <= mMaxThreadAcqAttempts; attempt++) {
      try {
          mLooperThread = new std::thread ( myLoop, this );
          ret = true;
          break;
      } catch(const std::system_error& e) {
          QCRIL_LOG_ERROR("Thread acquisition failed. Attempt: %d", attempt);
      } catch(...) {
          QCRIL_LOG_ERROR("Unexpected exception while starting thread. Module: %s. Attempt: %d", mName.c_str(), attempt);
      }
    }
    if(!ret && attempt > mMaxThreadAcqAttempts) {
        QCRIL_LOG_ERROR("Unable to acquire thread to handle message for looper %s", mName.c_str());
    }
  } else {
      ret = true;
  }
  return ret;
}

void Looper::releaseThread() {
  std::lock_guard<qtimutex::QtiSharedMutex> lock(looperMutex);
  if (Looper::FEATURE_DYNAMIC_THREAD) {
    if (mLooperThread != nullptr) {
      Log::getInstance().d("\t[" + to_string() + "]: ReleaseThread***************************************************");
      {
        std::lock_guard<qtimutex::QtiSharedMutex> lock(looperMutex);
        mReleaseThread = true;
        looperReady = true;
      }
      looperCv.notify_all();

      mLooperThread->join();
      delete mLooperThread;
      mLooperThread = nullptr;
    }
  }
}

void Looper::waitForLooperToConsumeAllMsgs() {
  while (true) {
    {
      std::unique_lock<qtimutex::QtiSharedMutex> mylock(looperMutex);
      looperCv.wait(mylock, [&] { return looperReady == false; });
    }
    if (isEmptyQueue()) {
      threadSleep(2);
      return;
    }
  }
}

void Looper::killLooper() {
  {
    std::lock_guard<qtimutex::QtiSharedMutex> lock(looperMutex);
    mExit = true;
    looperReady = true;
  }
  looperCv.notify_all();

}
