/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include "telephony/ril.h"

/*
 * Unsol message to report emergency callback mode
 */

class RilUnsolEmergencyCallbackModeMessage : public UnSolicitedMessage,
            public add_message_id<RilUnsolEmergencyCallbackModeMessage> {
 public:
  enum class EmergencyCallbackMode {
    INVALID,
    ENTER,
    LEAVE
  };
  static constexpr const char *MESSAGE_NAME = "RIL_UNSOL_EMERGENCY_CALLBACK_MODE";
  ~RilUnsolEmergencyCallbackModeMessage() { }

  explicit RilUnsolEmergencyCallbackModeMessage(EmergencyCallbackMode mode, bool isEmergencyIp)
      : UnSolicitedMessage(get_class_message_id()), mMode(mode), mIsEmergencyIp(isEmergencyIp) { }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolEmergencyCallbackModeMessage>(mMode, mIsEmergencyIp);
  }

  EmergencyCallbackMode getEmergencyCallbackMode() { return mMode; }

  bool isEmergencyIp() { return mIsEmergencyIp; }

  string dump() {
    return RilUnsolEmergencyCallbackModeMessage::MESSAGE_NAME;
  }

 private:
  EmergencyCallbackMode mMode;
  bool mIsEmergencyIp;
};
