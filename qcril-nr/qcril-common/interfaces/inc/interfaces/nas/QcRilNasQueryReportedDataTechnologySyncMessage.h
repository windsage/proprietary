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
    NAS exported API: query reported data technology

    Returns data technology (int type)
*/
/*==========================================================================*/
class QcRilNasQueryReportedDataTechnologySyncMessage
    : public SolicitedSyncMessage<int>,
      public add_message_id<QcRilNasQueryReportedDataTechnologySyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.nas.query_reported_data_technology_sync_message";
  ~QcRilNasQueryReportedDataTechnologySyncMessage() = default;

  QcRilNasQueryReportedDataTechnologySyncMessage()
      : SolicitedSyncMessage<int>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
