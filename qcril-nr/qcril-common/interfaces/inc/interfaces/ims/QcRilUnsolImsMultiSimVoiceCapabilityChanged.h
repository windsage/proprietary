/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/ims/ims.h>

/*
 * Unsol message to notify the IMS multi sim voice capability.
 */
class QcRilUnsolImsMultiSimVoiceCapabilityChanged
    : public UnSolicitedMessage,
      public add_message_id<QcRilUnsolImsMultiSimVoiceCapabilityChanged> {
 private:
  qcril::interfaces::MultiSimVoiceCapability mVoiceCapability;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilUnsolImsMultiSimVoiceCapabilityChanged";
  ~QcRilUnsolImsMultiSimVoiceCapabilityChanged() {
  }

  QcRilUnsolImsMultiSimVoiceCapabilityChanged(qcril::interfaces::MultiSimVoiceCapability val)
      : UnSolicitedMessage(get_class_message_id()), mVoiceCapability(val) {}

  std::shared_ptr<UnSolicitedMessage> clone() {
    std::shared_ptr<QcRilUnsolImsMultiSimVoiceCapabilityChanged> msg =
        std::make_shared<QcRilUnsolImsMultiSimVoiceCapabilityChanged>(mVoiceCapability);
    return msg;
  }

  qcril::interfaces::MultiSimVoiceCapability getMultiSimVoiceCapability() {
    return mVoiceCapability;
  }
  void setMultiSimVoiceCapability(qcril::interfaces::MultiSimVoiceCapability val) {
    mVoiceCapability = val;
  }

  virtual string dump() {
    std::string os;
    os += QcRilUnsolImsMultiSimVoiceCapabilityChanged::MESSAGE_NAME;
    os += "{";
    os += ".mVoiceCapability = ";
    os += qcril::interfaces::toString(mVoiceCapability);
    os += "}";
    return os;
  }
};
