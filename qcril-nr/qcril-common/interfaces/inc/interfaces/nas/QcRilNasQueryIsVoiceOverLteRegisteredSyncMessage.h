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
    NAS exported API: query if voice over lte is registered

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasQueryIsVoiceOverLteRegisteredSyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilNasQueryIsVoiceOverLteRegisteredSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.query_is_voice_lte_registered_sync_message";
  ~QcRilNasQueryIsVoiceOverLteRegisteredSyncMessage() = default;

  QcRilNasQueryIsVoiceOverLteRegisteredSyncMessage()
      : SolicitedSyncMessage<bool>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
