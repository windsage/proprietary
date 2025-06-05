/**
* Copyright (c) 2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef GETSLICINGCONFIGINTERNALMESSAGE
#define GETSLICINGCONFIGINTERNALMESSAGE
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

namespace rildata {

/********************** Class Definitions *************************/
class GetSlicingConfigInternalMessage: public UnSolicitedMessage,
                                     public add_message_id<GetSlicingConfigInternalMessage> {
private:
  std::shared_ptr<Message> msg;

public:
  static constexpr const char *MESSAGE_NAME = "GetSlicingConfigInternalMessage";

  GetSlicingConfigInternalMessage() = delete;
  GetSlicingConfigInternalMessage(std::shared_ptr<Message> m)
  : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    msg = m;
  }
  ~GetSlicingConfigInternalMessage(){};

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<GetSlicingConfigInternalMessage>(msg);
  }

  std::shared_ptr<Message> getMessage() {
    return msg;
  }

  string dump() {
    return std::string(MESSAGE_NAME) + " ";
  }
};

} //namespace

#endif
