/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "interfaces/nas/nas_types.h"

using qcril::interfaces::RIL_EccMapType;

/*===========================================================================*/
/*!
    @brief
    NAS exported API: get the emergency number map table

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasGetRilEccMapSyncMessage : public SolicitedSyncMessage<RIL_EccMapType>,
                                        public add_message_id<QcRilNasGetRilEccMapSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.get_ril_ecc_map_sync_message";
  ~QcRilNasGetRilEccMapSyncMessage() = default;

  QcRilNasGetRilEccMapSyncMessage() : SolicitedSyncMessage<RIL_EccMapType>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  inline string dump() {
    return mName;
  }
};
