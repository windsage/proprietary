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
    NAS exported API: get escv type for a given emergency number

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasGetEscvTypeSyncMessage : public SolicitedSyncMessage<int>,
                                       public add_message_id<QcRilNasGetEscvTypeSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.get_escv_type_sync_message";

  ~QcRilNasGetEscvTypeSyncMessage() = default;

  QcRilNasGetEscvTypeSyncMessage(std::string num)
      : SolicitedSyncMessage<int>(get_class_message_id()), mNum(num) {
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
