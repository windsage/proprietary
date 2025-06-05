/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class RilUnsolSimSmsStorageFullMessage : public UnSolicitedMessage,
                public add_message_id<RilUnsolSimSmsStorageFullMessage> {

 public:
  static constexpr const char *MESSAGE_NAME =
        "com.qualcomm.qti.qcril.sms.sim_sms_storage_full";
  ~RilUnsolSimSmsStorageFullMessage() {};

  explicit inline RilUnsolSimSmsStorageFullMessage()
      : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolSimSmsStorageFullMessage>();
  };
  string dump() {
    return mName;
  };
};
