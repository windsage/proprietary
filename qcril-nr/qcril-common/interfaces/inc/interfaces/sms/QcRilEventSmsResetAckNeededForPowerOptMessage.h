/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/message_id.h"
#include "framework/add_message_id.h"

class QcRilEventSmsResetAckNeededForPowerOptMessage : public UnSolicitedMessage,
                                 public add_message_id<QcRilEventSmsResetAckNeededForPowerOptMessage>
{
 public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.sms.reset_ack_needed_for_poweropt";
  ~QcRilEventSmsResetAckNeededForPowerOptMessage() {}

  QcRilEventSmsResetAckNeededForPowerOptMessage() :
        UnSolicitedMessage(get_class_message_id()) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<QcRilEventSmsResetAckNeededForPowerOptMessage>());
  }

  string dump() {
    return MESSAGE_NAME;
  }
};
