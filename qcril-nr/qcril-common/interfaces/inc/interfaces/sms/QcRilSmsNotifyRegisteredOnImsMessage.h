/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class QcRilSmsNotifyRegisteredOnImsMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilSmsNotifyRegisteredOnImsMessage> {
 private:
  bool mRegistered;

 public:
  static constexpr const char* MESSAGE_NAME = "com.qualcomm.qti.qcril.sms.notify_registered_on_ims";
  QcRilSmsNotifyRegisteredOnImsMessage() = delete;
  ~QcRilSmsNotifyRegisteredOnImsMessage() {
  }

  explicit inline QcRilSmsNotifyRegisteredOnImsMessage(bool registered)
      : UnSolicitedMessage(get_class_message_id()), mRegistered(registered) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QcRilSmsNotifyRegisteredOnImsMessage>(mRegistered);
  }

  string dump() {
    return mName + " is registered: " + (mRegistered ? "true" : "false");
  }

  int32_t isRegistered() {
    return mRegistered;
  }
};
