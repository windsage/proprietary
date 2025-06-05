/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class QcRilUnsolAssessEccNumberListMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilUnsolAssessEccNumberListMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.assess_ecc_number_list_message";
  ~QcRilUnsolAssessEccNumberListMessage() {
  }

  explicit inline QcRilUnsolAssessEccNumberListMessage(bool freshCheck = false)
      : UnSolicitedMessage(get_class_message_id()), mFreshCheck(freshCheck) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QcRilUnsolAssessEccNumberListMessage>(mFreshCheck);
  }

  bool ensureFreshCheck() {
    return mFreshCheck;
  }

  string dump() {
    return mName;
  }

 private:
  bool mFreshCheck;
};
