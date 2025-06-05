/**
* Copyright (c) 2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

/********************** Class Definitions *************************/
class ProcessInitialAttachRequestMessage: public UnSolicitedMessage,
                           public add_message_id<ProcessInitialAttachRequestMessage> {
private:
  std::shared_ptr<Message> mMessage;
public:
  static constexpr const char *MESSAGE_NAME = "ProcessInitialAttachRequestMessage";

  ProcessInitialAttachRequestMessage(std::shared_ptr<Message> msg):UnSolicitedMessage(get_class_message_id())
  {
    mMessage = msg;
  }
  ~ProcessInitialAttachRequestMessage() {}

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::make_shared<ProcessInitialAttachRequestMessage>(mMessage);
  }

  string dump()
  {
    return MESSAGE_NAME;
  }

  std::shared_ptr<Message> getMessage()
  {
    return mMessage;
  }
};
