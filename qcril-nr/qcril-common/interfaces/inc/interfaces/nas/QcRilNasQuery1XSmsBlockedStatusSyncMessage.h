/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"

/*===========================================================================*/
/*!
    @brief
    Query if 1x sms is blocked

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasQuery1XSmsBlockedStatusSyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilNasQuery1XSmsBlockedStatusSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.query_1x_sms_blocked_status_message";
  ~QcRilNasQuery1XSmsBlockedStatusSyncMessage() = default;

  QcRilNasQuery1XSmsBlockedStatusSyncMessage()
      : SolicitedSyncMessage<bool>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
