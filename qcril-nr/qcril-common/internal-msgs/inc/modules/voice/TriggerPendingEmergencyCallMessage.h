/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"

class TriggerPendingEmergencyCallMessage
    : public SolicitedMessage<void>,
      public add_message_id<TriggerPendingEmergencyCallMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "TriggerPendingEmergencyCallMessage";

  TriggerPendingEmergencyCallMessage() : SolicitedMessage<void>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  ~TriggerPendingEmergencyCallMessage() = default;

  string dump() {
    std::string os = mName;
    os += "{";
    os += "}";
    return os;
  }
};
