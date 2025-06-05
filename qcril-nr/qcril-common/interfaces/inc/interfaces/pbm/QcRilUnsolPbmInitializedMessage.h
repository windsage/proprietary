/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class QcRilUnsolPbmInitializedMessage : public UnSolicitedMessage,
                                        public add_message_id<QcRilUnsolPbmInitializedMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "com.qualcomm.qti.qcril.pbm.initialized";
  ~QcRilUnsolPbmInitializedMessage() {
  }

  explicit inline QcRilUnsolPbmInitializedMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QcRilUnsolPbmInitializedMessage>();
  }
  string dump() {
    return mName;
  }
};
