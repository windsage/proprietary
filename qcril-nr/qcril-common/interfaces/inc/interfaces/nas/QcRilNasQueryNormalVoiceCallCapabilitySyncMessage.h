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
    NAS exported API: query if device has normal voice call capability

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasQueryNormalVoiceCallCapabilitySyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilNasQueryNormalVoiceCallCapabilitySyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.query_normal_voice_call_capability_sync_message";
  ~QcRilNasQueryNormalVoiceCallCapabilitySyncMessage() = default;

  QcRilNasQueryNormalVoiceCallCapabilitySyncMessage()
      : SolicitedSyncMessage<bool>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  inline string dump() {
    return mName;
  }
};
