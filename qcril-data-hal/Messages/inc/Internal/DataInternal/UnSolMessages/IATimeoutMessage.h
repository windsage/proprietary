/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef IATIMEOUTMESSAGE
#define IATIMEOUTMESSAGE

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

namespace rildata {

/********************** Class Definitions *************************/
class IATimeoutMessage: public UnSolicitedMessage,
                           public add_message_id<IATimeoutMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "IATimeoutMessage";
  IATimeoutMessage():UnSolicitedMessage(get_class_message_id()){}
  ~IATimeoutMessage() {}

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::make_shared<IATimeoutMessage>();
  }
  string dump()
  {
    return MESSAGE_NAME;
  }
};
}
#endif
