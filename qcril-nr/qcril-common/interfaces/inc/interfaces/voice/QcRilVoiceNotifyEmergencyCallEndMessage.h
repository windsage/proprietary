/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

struct EmerCallEndType {
  bool is_eme_ip_call;
  bool is_eme_call_connected;
};

class QcRilVoiceNotifyEmergencyCallEndMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilVoiceNotifyEmergencyCallEndMessage> {
 private:
  EmerCallEndType mEmeCallInfo;

 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.voice.notify_emergency_call_end";
  QcRilVoiceNotifyEmergencyCallEndMessage() = delete;
  ~QcRilVoiceNotifyEmergencyCallEndMessage() {
  }

  explicit inline QcRilVoiceNotifyEmergencyCallEndMessage(EmerCallEndType call)
      : UnSolicitedMessage(get_class_message_id()), mEmeCallInfo(call) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QcRilVoiceNotifyEmergencyCallEndMessage>(mEmeCallInfo);
  }

  string dump() {
    return mName + " is emergency ip call: " + (mEmeCallInfo.is_eme_ip_call ? "true" : "false") +
           " is emergency call connected: " +
           (mEmeCallInfo.is_eme_call_connected ? "true" : "false");
  }

  EmerCallEndType getEmergencyCallInfo() {
    return mEmeCallInfo;
  }
};
