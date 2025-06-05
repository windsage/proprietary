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
    Query if power optimization is enabled

    Returns true or false
*/
/*==========================================================================*/
class QcRilGetPowerOptEnabledSyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilGetPowerOptEnabledSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.get_power_opt_enabled_sync_message";
  ~QcRilGetPowerOptEnabledSyncMessage() = default;

  QcRilGetPowerOptEnabledSyncMessage() : SolicitedSyncMessage<bool>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
