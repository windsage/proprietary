/******************************************************************************
#  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#ifndef __MODULE_H__
#define __MODULE_H__

#include <memory>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>
#include <iostream>

#include "framework/Looper.h"
#include "framework/Message.h"
#include "framework/MessageQueue.h"

#define DEFINE_MSG_ID_INFO(name) \
    message_id_ref get_id_##name() { \
        static message_id_info id_##name{#name}; \
        static message_id_ref local_ref = REG_MSG_N( #name, id_##name); \
        return local_ref; }

#define DECLARE_MSG_ID_INFO(name) message_id_ref get_id_##name()

#define HANDLER(MSG_TYPE, FUNC) {REG_MSG_N(MSG_TYPE::MESSAGE_NAME, MSG_TYPE::get_class_message_id()), ([this](auto msg) { Module::handler<MSG_TYPE>(std::bind(&FUNC, this, std::placeholders::_1), msg); })}
#define HANDLER_MULTI(MSG_TYPE, name, FUNC) {REG_MSG_N(#name, get_id_##name()), ([this](auto msg) { Module::handler<MSG_TYPE>(std::bind(&FUNC, this, std::placeholders::_1), msg); })}
#define LEGACY_HANDLER(ANDROID_REQ_TYPE, FUNC) {ril_request_info_##ANDROID_REQ_TYPE.msg_id, ([this](auto msg) { Module::handler<RilRequestMessage>(std::bind(&FUNC, this, std::placeholders::_1), msg); })}

using MessageHandler = std::unordered_map<std::reference_wrapper<message_id_info>,std::function<void(std::shared_ptr<Message>)>>;

class Module: public std::enable_shared_from_this<Module> {
 protected:
  std::string mName;
  std::unique_ptr<Looper> mLooper;
  MessageHandler mMessageHandler;

 public:
  enum class MessageHandlingPolicy {
    NONE,
    SYSTEM_SHARED_THREAD,
    MODULE_SHARED_THREAD,
  };

  Module();
  virtual ~Module();
  virtual void init();
  void dispatch(std::shared_ptr<Message> msg);
  void dispatchSync(std::shared_ptr<Message> msg);
  virtual void handleMessage(std::shared_ptr<Message> msg);
  virtual void handleMessageSync(std::shared_ptr<Message> msg);

  /* Below method is invoked by Dispather module only. */
  void onMessageArrival(std::shared_ptr<Message> msg);

  string to_string() const;
  std::thread *getLooperThread();
  size_t getUnProcessedMessageCount();
  bool hasValidLooper();

  virtual MessageHandlingPolicy getMessageHandlingPolicy(std::shared_ptr<Message> /*msg*/) const {
    return MessageHandlingPolicy::SYSTEM_SHARED_THREAD;
  }


  template<class M>
  inline void handler(std::function<void(std::shared_ptr<M>)> f, std::shared_ptr<Message> msg) {
    auto event = std::static_pointer_cast<M> (msg);
    f(event);
  }

  /* Only for simulation */
  void waitForLooperToConsumeAllMsgs();
  void killLooper();

  qtimutex::QtiRecursiveMutex mModuleHandlerExecutionMutex;
};

inline Module::Module() = default;

template <typename M>
class load_module
{
    public:
        load_module() {
            get_module().init();
        }
        M &get_module() {
            static M module{};
            return module;
        }
};

template <typename M>
class load_sharedptr_module
{
    public:
        load_sharedptr_module() {
            std::shared_ptr<M> module = get_module();
            if(module != nullptr) {
                module->init(); // TODO: Indicate success
            }
            // TODO: Indicate the failure to client somehow as this will fail silently.
        }
        std::shared_ptr<M> get_module() {
            static std::shared_ptr<M> module = std::make_shared<M>();
            return module;
        }
};

#endif
