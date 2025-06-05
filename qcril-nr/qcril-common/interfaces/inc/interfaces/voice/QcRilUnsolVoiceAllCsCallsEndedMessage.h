/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class QcRilUnsolVoiceAllCsCallsEndedMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilUnsolVoiceAllCsCallsEndedMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.voice.notify_all_cs_calls_ended";
  ~QcRilUnsolVoiceAllCsCallsEndedMessage() {
  }

  explicit inline QcRilUnsolVoiceAllCsCallsEndedMessage()
      : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QcRilUnsolVoiceAllCsCallsEndedMessage>();
  }

  string dump() {
    return mName;
  }
};
