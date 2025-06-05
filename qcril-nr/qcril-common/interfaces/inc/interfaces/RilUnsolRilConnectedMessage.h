/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/voice/voice.h>

/*
 * Notify unsol ril connected
 */
class RilUnsolRilConnectedMessage : public UnSolicitedMessage,
                                    public add_message_id<RilUnsolRilConnectedMessage> {
 private:
 public:
  static constexpr const char* MESSAGE_NAME = "RIL_UNSOL_RIL_CONNECTED";

  ~RilUnsolRilConnectedMessage() {
  }

  RilUnsolRilConnectedMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<RilUnsolRilConnectedMessage>();
    return msg;
  }

  string dump() {
    return mName + "{}";
  }
};
