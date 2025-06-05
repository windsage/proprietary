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
    NAS exported API: query if device is registered in cs domain

    Returns true or false
*/
/*==========================================================================*/
class QcRilNasQueryIsDeviceRegisteredCsDomainSyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilNasQueryIsDeviceRegisteredCsDomainSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.query_is_device_reg_cs_domain_sync_message";
  ~QcRilNasQueryIsDeviceRegisteredCsDomainSyncMessage() = default;

  QcRilNasQueryIsDeviceRegisteredCsDomainSyncMessage()
      : SolicitedSyncMessage<bool>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
