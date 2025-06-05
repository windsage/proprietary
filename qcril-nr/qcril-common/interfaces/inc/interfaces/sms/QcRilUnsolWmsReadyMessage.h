/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class QcRilUnsolWmsReadyMessage : public UnSolicitedMessage,
                                  public add_message_id<QcRilUnsolWmsReadyMessage> {
 private:
  uint8_t mStatus;

 public:
  static constexpr const char* MESSAGE_NAME = "com.qualcomm.qti.qcril.wms.wms_ready";
  ~QcRilUnsolWmsReadyMessage(){};

  explicit inline QcRilUnsolWmsReadyMessage(uint8_t status)
      : UnSolicitedMessage(get_class_message_id()), mStatus(status) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QcRilUnsolWmsReadyMessage>(mStatus);
  }

  uint8_t getStatus() {
    return mStatus;
  }

  virtual string dump() {
    return mName;
  }
};
