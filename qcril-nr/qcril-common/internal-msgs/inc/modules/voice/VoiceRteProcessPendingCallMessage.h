/******************************************************************************
#  Copyright (c) 2017,2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

class VoiceRteProcessPendingCallMessage : public UnSolicitedMessage,
                                          public add_message_id<VoiceRteProcessPendingCallMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "VoiceRteProcessPendingCallMessage";

  VoiceRteProcessPendingCallMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  ~VoiceRteProcessPendingCallMessage() = default;

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<VoiceRteProcessPendingCallMessage>());
  }

  std::string dump() {
    std::string os = mName;
    os += "{";
    os += "}";
    return os;
  }
};
