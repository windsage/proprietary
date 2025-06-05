/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include <interfaces/ims/ims.h>

/*===========================================================================*/
/*!
    @brief

    Messages to get C_IWLAN mode

    Returns an enum indicating the C_IWLAN mode
*/
/*==========================================================================*/
class QcRilRequestImsQueryCIWLANConfigSyncMessage : public SolicitedSyncMessage<qcril::interfaces::ImsCiWlanConfig>,
                                        public add_message_id<QcRilRequestImsQueryCIWLANConfigSyncMessage> {
 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.ims.get_c_iwlan_mode";
  ~QcRilRequestImsQueryCIWLANConfigSyncMessage() = default;

  QcRilRequestImsQueryCIWLANConfigSyncMessage() : SolicitedSyncMessage<qcril::interfaces::ImsCiWlanConfig>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName;
  }
};
