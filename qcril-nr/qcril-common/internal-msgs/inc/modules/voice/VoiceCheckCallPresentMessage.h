/******************************************************************************
#  Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "voice_service_v02.h"

class VoiceCheckCallPresentMessage : public SolicitedSyncMessage<bool>,
                                     public add_message_id<VoiceCheckCallPresentMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "VoiceCheckCallPresentMessage";

  enum class Type { ALL_CALL, VOICE_CALL, MAX };

  VoiceCheckCallPresentMessage() = delete;
  ~VoiceCheckCallPresentMessage() = default;

  inline VoiceCheckCallPresentMessage(Type type)
      : SolicitedSyncMessage<bool>(get_class_message_id()), mType(type) {
    mName = MESSAGE_NAME;
  }

  Type getCallType() {
    return mType;
  }
  string dump() {
    std::string os = mName;
    os += "{";
    os += std::string(".mType=") + (mType == Type::VOICE_CALL ? "VOICE_CALL" : "ALL_CALL");
    os += "}";
    return os;
  }

 private:
  Type mType;
};
