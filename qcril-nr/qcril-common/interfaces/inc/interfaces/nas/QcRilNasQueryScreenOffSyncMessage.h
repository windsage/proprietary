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
    NAS exported API: query if screen is off

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasQueryScreenOffSyncMessage : public SolicitedSyncMessage<bool>,
                                          public add_message_id<QcRilNasQueryScreenOffSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.query_screen_off_sync_message";
  ~QcRilNasQueryScreenOffSyncMessage() = default;

  QcRilNasQueryScreenOffSyncMessage() : SolicitedSyncMessage<bool>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  inline string dump() {
    return mName;
  }
};
