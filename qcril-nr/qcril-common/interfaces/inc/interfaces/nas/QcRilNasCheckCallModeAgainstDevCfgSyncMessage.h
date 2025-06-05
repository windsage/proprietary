/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "voice_service_v02.h"

/*===========================================================================*/
/*!
    @brief
    NAS exported API: check if call mode matches device config

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasCheckCallModeAgainstDevCfgSyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilNasCheckCallModeAgainstDevCfgSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.check_call_mode_against_devcfg_sync_message";

  static std::string toString(call_mode_enum_v02 callMode) {
    switch (callMode) {
      case CALL_MODE_NO_SRV_V02:
        return "No service";
      case CALL_MODE_CDMA_V02:
        return "CDMA";
      case CALL_MODE_GSM_V02:
        return "GSM";
      case CALL_MODE_UMTS_V02:
        return "UMTS";
      case CALL_MODE_LTE_V02:
        return "LTE";
      case CALL_MODE_TDS_V02:
        return "TD-SCDMA";
      case CALL_MODE_UNKNOWN_V02:
        return "Unknown";
      case CALL_MODE_WLAN_V02:
      case CALL_MODE_C_IWLAN_V02:
        return "WLAN";
      case CALL_MODE_NR5G_V02:
        return "NR5G";
      default:
        return "Unknown";
    }
  }

  ~QcRilNasCheckCallModeAgainstDevCfgSyncMessage() = default;

  QcRilNasCheckCallModeAgainstDevCfgSyncMessage(call_mode_enum_v02 callMode)
      : SolicitedSyncMessage<bool>(get_class_message_id()), mCallMode(callMode) {
    mName = MESSAGE_NAME;
  }

  call_mode_enum_v02 getCallMode() {
    return mCallMode;
  }

  inline string dump() {
    return mName + " Call mode: " + toString(mCallMode);
  }

 private:
  call_mode_enum_v02 mCallMode;
};
