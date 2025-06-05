/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

class VoiceImsWaveRadioStateMessage : public UnSolicitedMessage,
                                      public add_message_id<VoiceImsWaveRadioStateMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "VoiceImsWaveRadioStateMessage";

  VoiceImsWaveRadioStateMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  ~VoiceImsWaveRadioStateMessage() = default;

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<VoiceImsWaveRadioStateMessage>());
  }

  std::string dump() {
    std::string os = mName;
    os += "{";
    os += "}";
    return os;
  }
};
