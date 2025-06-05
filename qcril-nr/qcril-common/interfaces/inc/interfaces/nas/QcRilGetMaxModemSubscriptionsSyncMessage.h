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
    Provides the number of maximum modem subscriptions

    Returns number of modem subs
*/
/*==========================================================================*/
class QcRilGetMaxModemSubscriptionsSyncMessage : public SolicitedSyncMessage<int>,
                                      public add_message_id<QcRilGetMaxModemSubscriptionsSyncMessage> {

 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.get_max_modem_subs_sync_message";
  ~QcRilGetMaxModemSubscriptionsSyncMessage() = default;

  QcRilGetMaxModemSubscriptionsSyncMessage()
      : SolicitedSyncMessage<int>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
