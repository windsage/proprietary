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
    NAS exported API: query if device is lte registered only

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasQueryIsLteOnlyRegisteredSyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilNasQueryIsLteOnlyRegisteredSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.query_is_lte_only_registered_sync_message";
  ~QcRilNasQueryIsLteOnlyRegisteredSyncMessage() = default;

  QcRilNasQueryIsLteOnlyRegisteredSyncMessage()
      : SolicitedSyncMessage<bool>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
