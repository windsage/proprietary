/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"

enum class NasSmsStatus {
  STATUS_UNKNOWN = 0,
  STATUS_NO_SMS = 1,
  STATUS_IMS = 2,
  STATUS_1X = 3,
  STATUS_3GPP = 4,
};

/*===========================================================================*/
/*!
    @brief

    Messages to get the LTE SMS status

    Returns an enum indicating the LTE SMS status
*/
/*==========================================================================*/
class QcRilGetLteSmsStatusSyncMessage : public SolicitedSyncMessage<NasSmsStatus>,
                                        public add_message_id<QcRilGetLteSmsStatusSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.get_lte_sms_status_sync_message";
  ~QcRilGetLteSmsStatusSyncMessage() = default;

  QcRilGetLteSmsStatusSyncMessage() : SolicitedSyncMessage<NasSmsStatus>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
