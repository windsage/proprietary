/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include <stdint.h>

/*===========================================================================*/
/*!
    @brief
    NAS exported API: query the simultaneous voice data capability

    Returns mask with uint64_t type
*/
/*==========================================================================*/
class QcRilNasQuerySimulVoiceDataCapabilitySyncMessage
    : public SolicitedSyncMessage<uint64_t>,
      public add_message_id<QcRilNasQuerySimulVoiceDataCapabilitySyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.query_simul_voice_data_capability_sync_message";
  ~QcRilNasQuerySimulVoiceDataCapabilitySyncMessage() = default;

  QcRilNasQuerySimulVoiceDataCapabilitySyncMessage()
      : SolicitedSyncMessage<uint64_t>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
