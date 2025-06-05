/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "voice_service_v02.h"

class QcRilVoiceNotifyCallInProcessMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilVoiceNotifyCallInProcessMessage> {
 private:
  call_mode_enum_v02 mCallMode;
  call_state_enum_v02 mCallState;
  bool mIsAlertingCsCall;

 public:
  static constexpr const char* MESSAGE_NAME = "com.qualcomm.qti.qcril.voice.notify_call_in_process";

  static std::string toString(call_mode_enum_v02 callMode) {
    switch (callMode) {
      case CALL_MODE_NO_SRV_V02:
        return "No service";
      case CALL_MODE_CDMA_V02:
        return "CDMA";
      case CALL_MODE_GSM_V02:
        return "GSM";
      case CALL_MODE_UMTS_V02:
        return "UMTS";
      case CALL_MODE_LTE_V02:
        return "LTE";
      case CALL_MODE_TDS_V02:
        return "TD-SCDMA";
      case CALL_MODE_UNKNOWN_V02:
        return "Unknown";
      case CALL_MODE_WLAN_V02:
      case CALL_MODE_C_IWLAN_V02:
        return "WLAN";
      case CALL_MODE_NR5G_V02:
        return "NR5G";
      default:
        return "Unknown";
    }
  }

  static std::string toString(call_state_enum_v02 callState) {
    switch (callState) {
      case CALL_STATE_ORIGINATING_V02:
        return "Origination";
      case CALL_STATE_INCOMING_V02:
        return "Incoming";
      case CALL_STATE_CONVERSATION_V02:
        return "Conversation";
      case CALL_STATE_CC_IN_PROGRESS_V02:
        return "Call Control in progress";
      case CALL_STATE_ALERTING_V02:
        return "Alerting";
      case CALL_STATE_HOLD_V02:
        return "Hold";
      case CALL_STATE_WAITING_V02:
        return "Waiting";
      case CALL_STATE_DISCONNECTING_V02:
        return "Disconnecting";
      case CALL_STATE_END_V02:
        return "End";
      case CALL_STATE_SETUP_V02:
        return "Setup";
      case CALL_STATE_PRE_ALERTING_V02:
        return "Pre alerting";
      default:
        return "Unknown";
    }
  }

  ~QcRilVoiceNotifyCallInProcessMessage() = default;

  explicit inline QcRilVoiceNotifyCallInProcessMessage(call_mode_enum_v02 callMode,
                                                       call_state_enum_v02 callState)
      : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mCallMode = callMode;
    mCallState = callState;
    mIsAlertingCsCall = false;
  }

  explicit inline QcRilVoiceNotifyCallInProcessMessage()
      : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mCallMode = CALL_MODE_UNKNOWN_V02;
    mCallState = CALL_STATE_ENUM_MIN_ENUM_VAL_V02;
    mIsAlertingCsCall = false;
  }

  void setAlertingCsCall(bool value) {
    mIsAlertingCsCall = value;
  }

  bool getAlertingCsCall() {
    return mIsAlertingCsCall;
  }

  call_mode_enum_v02 getCallMode() {
    return mCallMode;
  }

  call_state_enum_v02 getCallState() {
    return mCallState;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<QcRilVoiceNotifyCallInProcessMessage>(mCallMode, mCallState);
    assert(msg != nullptr);
    msg->setAlertingCsCall(mIsAlertingCsCall);
    return msg;
  }

  string dump() {
    return mName + " call mode: " + toString(mCallMode) + " call state: " + toString(mCallState);
  }
};
