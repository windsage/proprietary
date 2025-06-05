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
    NAS exported API: query if device is in leaving LPM window (online operating mode
    request is sent, but device has not camped on full service)

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasQueryWhetherInApmLeaveWindowSyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilNasQueryWhetherInApmLeaveWindowSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.query_whether_in_apm_leave_window_sync_message";
  ~QcRilNasQueryWhetherInApmLeaveWindowSyncMessage() = default;

  QcRilNasQueryWhetherInApmLeaveWindowSyncMessage()
      : SolicitedSyncMessage<bool>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  inline string dump() {
    return mName;
  }
};
