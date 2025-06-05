/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/ims/ims.h>

/*
 * Unsol message to notify HLOS to whether to disable C_IWLAN or not.
 * This Unsol is sent when C_IWLAN registration fails and modem is configured
 * for C_IWLAN Only mode so that user is aware of device registration failure
 * due to enabling C_IWLAN feature and disabling C_IWLAN may help register.
 *
 */
class QcRilUnsolImsCIwlanNotification : public UnSolicitedMessage,
                                         public add_message_id<QcRilUnsolImsCIwlanNotification> {
 private:
  qcril::interfaces::CiWlanNotificationInfo mNotification;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilUnsolImsCIwlanNotification";
  ~QcRilUnsolImsCIwlanNotification() {
  }

  QcRilUnsolImsCIwlanNotification(
    const qcril::interfaces::CiWlanNotificationInfo& notification)
      : UnSolicitedMessage(get_class_message_id()), mNotification(notification) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<QcRilUnsolImsCIwlanNotification>(mNotification);
    return msg;
  }

  const qcril::interfaces::CiWlanNotificationInfo& getNotificationStatus() {
    return mNotification;
  }

  virtual string dump() {
    std::string os;
    os += QcRilUnsolImsCIwlanNotification::MESSAGE_NAME;
    os += "{";
    os += qcril::interfaces::toString(mNotification);
    os += "}";
    return os;
  }
};
