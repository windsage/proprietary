/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef EXITFROMCURRENTSTATEREQUESTMESSAGE
#define EXITFROMCURRENTSTATEREQUESTMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"


namespace rildata {

/********************** Class Definitions *************************/
class ExitFromCurrentStateRequestMessage: public UnSolicitedMessage,
                            public add_message_id<ExitFromCurrentStateRequestMessage> {

public:
  static constexpr const char *MESSAGE_NAME = "ExitFromCurrentStateRequestMessage";

  ExitFromCurrentStateRequestMessage() : UnSolicitedMessage(get_class_message_id()){}
  ~ExitFromCurrentStateRequestMessage() {};

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<ExitFromCurrentStateRequestMessage>();
  }

  string dump() {
    return MESSAGE_NAME;
  }
};

} //namespace

#endif