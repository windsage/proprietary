/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/Util.h"
#include "framework/add_message_id.h"
#include "framework/Dispatcher.h"
#include "modules/sms/qcril_qmi_sms.h"

class QcRilSyncQuerySmsFormatMessage : public SolicitedSyncMessage<qmi_ril_sms_format_type>,
                                    public add_message_id<QcRilSyncQuerySmsFormatMessage>
{
 public:
  static constexpr const char *MESSAGE_NAME = "QUERY_SMS_FORMAT";
  inline QcRilSyncQuerySmsFormatMessage() :
    SolicitedSyncMessage<qmi_ril_sms_format_type>(get_class_message_id())
  {
    mName = MESSAGE_NAME;
  }
  ~QcRilSyncQuerySmsFormatMessage() {}

  string dump() {
    return MESSAGE_NAME;
  }
};
