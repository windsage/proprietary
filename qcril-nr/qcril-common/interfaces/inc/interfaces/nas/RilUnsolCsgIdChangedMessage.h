/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
 * Unsol message to notify CSG ID change
 */
class RilUnsolCsgIdChangedMessage : public UnSolicitedMessage,
                                    public add_message_id<RilUnsolCsgIdChangedMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilUnsolCsgIdChangedMessage";

  RilUnsolCsgIdChangedMessage() = delete;
  RilUnsolCsgIdChangedMessage(uint32_t csgId)
      : UnSolicitedMessage(get_class_message_id()), mCsgId(csgId) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolCsgIdChangedMessage>(mCsgId);
  }

  uint32_t getCsgId() {
    return mCsgId;
  }

  string dump() {
    return RilUnsolCsgIdChangedMessage::MESSAGE_NAME;
  }

 private:
  uint32_t mCsgId;
};
