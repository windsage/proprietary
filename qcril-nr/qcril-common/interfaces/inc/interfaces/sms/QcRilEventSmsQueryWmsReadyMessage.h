/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/message_id.h"
#include "framework/add_message_id.h"

class QcRilEventSmsQueryWmsReadyMessage : public UnSolicitedMessage,
                                 public add_message_id<QcRilEventSmsQueryWmsReadyMessage>
{
 public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.sms.query_wms_ready";
  ~QcRilEventSmsQueryWmsReadyMessage() {}

  QcRilEventSmsQueryWmsReadyMessage() :
        UnSolicitedMessage(get_class_message_id()) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<QcRilEventSmsQueryWmsReadyMessage>());
  }

  string dump() {
    return MESSAGE_NAME;
  }
};
