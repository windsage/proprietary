/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"

struct QcRilGetMccMncType {
  std::string mcc;
  std::string mnc;
  bool success;
};

/*===========================================================================*/
/*!
    @brief

    Messages to get the current MCC and MNC

    params:
    mIs3gpp  true to consider the 3gpp mcc/mnc
    mIs3gpp2 true to consider the 3gpp2 mcc/mnc
    Both can be set as true, since NAS can find what is in service currently

    Returns mcc and mnc
*/
/*==========================================================================*/
class QcRilGetCurrentMccMncSyncMessage : public SolicitedSyncMessage<QcRilGetMccMncType>,
                                         public add_message_id<QcRilGetCurrentMccMncSyncMessage> {
 private:
  bool mIs3gpp;
  bool mIs3gpp2;

 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.get_current_mcc_mnc_sync_message";
  ~QcRilGetCurrentMccMncSyncMessage() = default;

  QcRilGetCurrentMccMncSyncMessage(bool is3gpp, bool is3gpp2)
      : SolicitedSyncMessage<QcRilGetMccMncType>(get_class_message_id()),
        mIs3gpp(is3gpp),
        mIs3gpp2(is3gpp2) {
    mName = MESSAGE_NAME;
  }
  bool is3gpp() {
    return mIs3gpp;
  }
  bool is3gpp2() {
    return mIs3gpp2;
  }
  inline string dump() {
    return mName + " is_3gpp:" + (mIs3gpp ? "true" : "false") + " is_3gpp2:" +
           (mIs3gpp2 ? "true" : "false");
  }
};
