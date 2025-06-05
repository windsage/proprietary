/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
 * Unsol message to notify modem restart
 */
class RilUnsolModemRestartMessage : public UnSolicitedMessage,
                                    public add_message_id<RilUnsolModemRestartMessage> {
 private:
   std::string mReason;

 public:
  static constexpr const char* MESSAGE_NAME = "RIL_UNSOL_MODEM_RESTART";
  ~RilUnsolModemRestartMessage() {
  }

  explicit inline RilUnsolModemRestartMessage(std::string reason)
      : UnSolicitedMessage(get_class_message_id()), mReason(reason) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolModemRestartMessage>(mReason);
  }

  std::string getReason() {
    return mReason;
  }

  std::string dump() {
    return RilUnsolModemRestartMessage::MESSAGE_NAME;
  }
};
