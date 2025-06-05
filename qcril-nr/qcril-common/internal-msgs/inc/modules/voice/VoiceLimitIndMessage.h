/******************************************************************************
#  Copyright (c) 2017,2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/message_id.h"
#include "framework/add_message_id.h"

class VoiceLimitIndMessage : public UnSolicitedMessage, public add_message_id<VoiceLimitIndMessage> {
 private:
  bool mIsLimited;

 public:
  static constexpr const char* MESSAGE_NAME = "VoiceLimitIndMessage";

  VoiceLimitIndMessage() = delete;

  VoiceLimitIndMessage(bool isLimited = false)
      : UnSolicitedMessage(get_class_message_id()), mIsLimited(isLimited) {
    mName = MESSAGE_NAME;
  }

  ~VoiceLimitIndMessage() = default;

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<VoiceLimitIndMessage>(mIsLimited));
  }

  bool isIndLimited() {
    return mIsLimited;
  }

  std::string dump() {
    std::string os = mName;
    os += "{";
    os += ".mIsLimited=";
    os += (mIsLimited ? "true" : "false");
    os += "}";
    return os;
  }
};
