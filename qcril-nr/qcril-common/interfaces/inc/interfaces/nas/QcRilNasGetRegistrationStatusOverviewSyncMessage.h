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
    NAS exported API: get the overview of registration status

*/
/*==========================================================================*/

class QcRilNasGetRegistrationStatusOverviewSyncMessage
    : public SolicitedSyncMessage<uint32_t>,
      public add_message_id<QcRilNasGetRegistrationStatusOverviewSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.get_reg_status_overview_sync_message";
  ~QcRilNasGetRegistrationStatusOverviewSyncMessage() = default;

  QcRilNasGetRegistrationStatusOverviewSyncMessage()
      : SolicitedSyncMessage<uint32_t>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  inline string dump() {
    return mName;
  }
};
