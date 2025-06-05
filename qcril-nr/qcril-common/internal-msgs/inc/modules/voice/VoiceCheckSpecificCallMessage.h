/******************************************************************************
#  Copyright (c) 2017,2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "voice_service_v02.h"

typedef struct {
  bool has_call;
  call_mode_enum_v02 call_mode;
} SpecificCallResult;

class VoiceCheckSpecificCallMessage : public SolicitedSyncMessage<SpecificCallResult>,
                                      public add_message_id<VoiceCheckSpecificCallMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "VoiceCheckSpecificCallMessage";
  enum class SpecificCallType { CALL_TO_ATEL, CALL_TYPE_MAX };

  VoiceCheckSpecificCallMessage() = delete;
  ~VoiceCheckSpecificCallMessage() = default;

  VoiceCheckSpecificCallMessage(SpecificCallType type)
      : SolicitedSyncMessage<SpecificCallResult>(get_class_message_id()), mType(type) {
    mName = MESSAGE_NAME;
  }

  SpecificCallType getType() {
    return mType;
  }

  std::string dump() {
    std::string os = mName;
    os += "{";
    os += std::string(".mType=") +
          (mType == SpecificCallType::CALL_TO_ATEL ? "CALL_TO_ATEL" : "NONE");
    os += "}";
    return os;
  }

 private:
  SpecificCallType mType;
};
