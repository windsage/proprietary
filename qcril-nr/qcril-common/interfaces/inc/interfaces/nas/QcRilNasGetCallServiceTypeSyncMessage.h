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
    NAS exported API: get call service type based on current network

    Returns call service type
*/
/*==========================================================================*/
class QcRilNasGetCallServiceTypeSyncMessage
    : public SolicitedSyncMessage<voice_dial_call_service_type_enum_v02>,
      public add_message_id<QcRilNasGetCallServiceTypeSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.get_call_service_type_sync_message";
  ~QcRilNasGetCallServiceTypeSyncMessage() = default;

  QcRilNasGetCallServiceTypeSyncMessage(bool dialOnCs)
      : SolicitedSyncMessage<voice_dial_call_service_type_enum_v02>(get_class_message_id()),
        mDialOnCs(dialOnCs) {
    mName = MESSAGE_NAME;
  }
  bool getDialOnCs() {
    return mDialOnCs;
  }
  inline string dump() {
    return mName + " is dial on CS:" + (mDialOnCs ? "true" : "false");
  }

 private:
  bool mDialOnCs;
};
