/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "telephony/ril.h"

/*===========================================================================*/
/*!
    @brief
    DMS exported API: query radio state

    Returns current radio state
*/
/*==========================================================================*/
class QcRilDmsGetRadioStateSyncMessage : public SolicitedSyncMessage<RIL_RadioState>,
                                         public add_message_id<QcRilDmsGetRadioStateSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.dms.get_radio_state_sync_message";
  ~QcRilDmsGetRadioStateSyncMessage() = default;

  QcRilDmsGetRadioStateSyncMessage()
      : SolicitedSyncMessage<RIL_RadioState>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
