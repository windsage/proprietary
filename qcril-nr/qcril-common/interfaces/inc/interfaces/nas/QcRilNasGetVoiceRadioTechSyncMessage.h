/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "interfaces/nas/nas_types.h"

/*===========================================================================*/
/*!
    @brief
    NAS exported API: get voice radio tech

    Returns current voice radio tech and reported voice radio tech
*/
/*==========================================================================*/

struct VoiceRadioTechType {
  qmi_ril_nw_reg_rte_type currentVoiceRadioTech;
  RIL_RadioTechnology reportedVoiceRadioTech;
};

class QcRilNasGetVoiceRadioTechSyncMessage
    : public SolicitedSyncMessage<VoiceRadioTechType>,
      public add_message_id<QcRilNasGetVoiceRadioTechSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.get_voice_radio_tech_sync_message";
  ~QcRilNasGetVoiceRadioTechSyncMessage() = default;

  QcRilNasGetVoiceRadioTechSyncMessage()
      : SolicitedSyncMessage<VoiceRadioTechType>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  inline string dump() {
    return mName;
  }
};
