/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
 * Unsol message to notify number of CS calls has been changed
 */
class QcRilUnsolVoiceNumberOfCsCallChangedMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilUnsolVoiceNumberOfCsCallChangedMessage> {
 private:
  uint8_t mNumCsCalls;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilUnsolVoiceNumberOfCsCallChangedMessage";
  ~QcRilUnsolVoiceNumberOfCsCallChangedMessage() = default;

  QcRilUnsolVoiceNumberOfCsCallChangedMessage(uint8_t num)
      : UnSolicitedMessage(get_class_message_id()), mNumCsCalls(num) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QcRilUnsolVoiceNumberOfCsCallChangedMessage>(mNumCsCalls);
  }

  uint8_t getNumOfCsCalls() {
    return mNumCsCalls;
  }

  string dump() {
    return mName + " number of CS calls: " + std::to_string(mNumCsCalls);
  }
};
