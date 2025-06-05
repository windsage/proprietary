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
    NAS exported API: query UI status (related to boot up optimization)

    Returns true or false
*/
/*==========================================================================*/
class QcRilGetUIStatusSyncMessage : public SolicitedSyncMessage<uint8_t>,
                                    public add_message_id<QcRilGetUIStatusSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.get_ui_status_sync_message";
  ~QcRilGetUIStatusSyncMessage() = default;

  QcRilGetUIStatusSyncMessage() : SolicitedSyncMessage<uint8_t>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
