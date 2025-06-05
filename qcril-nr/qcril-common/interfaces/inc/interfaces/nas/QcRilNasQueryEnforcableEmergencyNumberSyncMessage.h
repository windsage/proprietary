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
    NAS exported API: check if the number needs to be enforced as emergency number

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasQueryEnforcableEmergencyNumberSyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilNasQueryEnforcableEmergencyNumberSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.check_enforcable_ecc_number_sync_message";

  ~QcRilNasQueryEnforcableEmergencyNumberSyncMessage() = default;

  QcRilNasQueryEnforcableEmergencyNumberSyncMessage(std::string num)
      : SolicitedSyncMessage<bool>(get_class_message_id()), mNum(num) {
    mName = MESSAGE_NAME;
  }

  std::string getNum() {
    return mNum;
  }

  inline string dump() {
    return mName + " checked number: " + mNum;
  }

 private:
  std::string mNum;
};
