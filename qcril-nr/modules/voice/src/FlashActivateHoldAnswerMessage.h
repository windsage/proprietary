/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"

class FlashActivateHoldAnswerMessage : public SolicitedMessage<void>,
                                       public add_message_id<FlashActivateHoldAnswerMessage> {
 private:
  uint8_t mCallId;

 public:
  static constexpr const char* MESSAGE_NAME = "FlashActivateHoldAnswerMessage";

  FlashActivateHoldAnswerMessage(uint8_t callId)
      : SolicitedMessage<void>(get_class_message_id()), mCallId(callId) {
    mName = MESSAGE_NAME;
  }

  inline ~FlashActivateHoldAnswerMessage() {
  }

  uint8_t getCallId() {
    return mCallId;
  }

  string dump() {
    std::string os = mName;
    os += "{";
    os += std::to_string(mCallId);
    os += "}";
    return os;
  }
};
