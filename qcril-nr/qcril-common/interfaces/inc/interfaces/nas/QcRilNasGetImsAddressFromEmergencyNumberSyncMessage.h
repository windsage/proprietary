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
    NAS exported API: get ims address from emergency number

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasGetImsAddressFromEmergencyNumberSyncMessage
    : public SolicitedSyncMessage<std::string>,
      public add_message_id<QcRilNasGetImsAddressFromEmergencyNumberSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.get_ims_address_from_emergency_number_sync_message";

  ~QcRilNasGetImsAddressFromEmergencyNumberSyncMessage() = default;

  QcRilNasGetImsAddressFromEmergencyNumberSyncMessage(std::string num)
      : SolicitedSyncMessage<std::string>(get_class_message_id()), mNum(num) {
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
