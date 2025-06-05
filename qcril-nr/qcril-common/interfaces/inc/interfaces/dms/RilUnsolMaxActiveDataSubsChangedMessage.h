/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
 * Unsol message to notify Maximum Active Data Subscriptions
 */
class RilUnsolMaxActiveDataSubsChangedMessage
    : public UnSolicitedMessage,
      public add_message_id<RilUnsolMaxActiveDataSubsChangedMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilUnsolMaxActiveDataSubsChangedMessage";

  RilUnsolMaxActiveDataSubsChangedMessage() = delete;
  RilUnsolMaxActiveDataSubsChangedMessage(uint8_t maxDataSubs)
      : UnSolicitedMessage(get_class_message_id()), mMaxActiveDataSubscriptions(maxDataSubs) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolMaxActiveDataSubsChangedMessage>(mMaxActiveDataSubscriptions);
  }

  uint8_t getMaxActiveDataSubscriptions() {
    return mMaxActiveDataSubscriptions;
  }

  string dump() {
    return RilUnsolMaxActiveDataSubsChangedMessage::MESSAGE_NAME;
  }

 private:
  uint8_t mMaxActiveDataSubscriptions;
};
