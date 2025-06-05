/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "modules/nas/qcril_qmi_nas.h"

/*===========================================================================*/
/*!
    @brief
    NAS exported API: get the overview of registration status

*/
/*==========================================================================*/

class QcRilNasGetRoamStatusOverviewSyncMessage
    : public SolicitedSyncMessage<uint32_t>,
      public add_message_id<QcRilNasGetRoamStatusOverviewSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.get_roam_status_overview_sync_message";
  ~QcRilNasGetRoamStatusOverviewSyncMessage() = default;

  QcRilNasGetRoamStatusOverviewSyncMessage()
      : SolicitedSyncMessage<uint32_t>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  inline string dump() {
    return mName;
  }
};
