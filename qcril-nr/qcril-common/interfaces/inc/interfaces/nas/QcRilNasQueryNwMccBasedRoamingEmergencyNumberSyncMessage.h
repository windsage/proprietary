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
    NAS exported API: check if call mode matches device config

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasQueryNwMccBasedRoamingEmergencyNumberSyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilNasQueryNwMccBasedRoamingEmergencyNumberSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.query_nw_mcc_based_roaming_emergency_number_sync_message";

  ~QcRilNasQueryNwMccBasedRoamingEmergencyNumberSyncMessage() = default;

  QcRilNasQueryNwMccBasedRoamingEmergencyNumberSyncMessage(std::string num)
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
