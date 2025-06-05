/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include "interfaces/nas/nas_types.h"
/**
 * Unsol message to report sub prov status.
 */
class RilUnsolSubProvisioningStatusMessage : public UnSolicitedMessage,
                                    public add_message_id<RilUnsolSubProvisioningStatusMessage> {
 private:
  qcril::interfaces::RILSubProvStatus_t mStatus;

 public:
  static constexpr const char* MESSAGE_NAME = "QCRIL_REQ_HOOK_UNSOL_SUB_PROVISION_STATUS";
  ~RilUnsolSubProvisioningStatusMessage() {
  }

  RilUnsolSubProvisioningStatusMessage(qcril::interfaces::RILSubProvStatus_t status)
      : UnSolicitedMessage(get_class_message_id()), mStatus(status) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolSubProvisioningStatusMessage>(mStatus);
  }

  qcril::interfaces::RILSubProvStatus_t getStatus() {
    return mStatus;
  }
  string dump() {
    return RilUnsolSubProvisioningStatusMessage::MESSAGE_NAME;
  }
};
