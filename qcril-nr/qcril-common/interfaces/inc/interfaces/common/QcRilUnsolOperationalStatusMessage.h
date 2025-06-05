/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "framework/legacy.h"
#include <string>

class QcRilUnsolOperationalStatusMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilUnsolOperationalStatusMessage> {
 private:
  qmi_ril_gen_operational_status_type mState;

 public:
  static constexpr const char* MESSAGE_NAME = "com.qualcomm.qti.qcril.operational_state";
  ~QcRilUnsolOperationalStatusMessage() {
  }

  explicit inline QcRilUnsolOperationalStatusMessage(qmi_ril_gen_operational_status_type state)
      : UnSolicitedMessage(get_class_message_id()), mState(state) {
    mName = MESSAGE_NAME;
  }

  qmi_ril_gen_operational_status_type getOperationalState() {
    return mState;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QcRilUnsolOperationalStatusMessage>(mState);
  }
  string dump() {
    return mName + ", state = " + std::to_string((int)mState);
  }
};
