/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to get sub prov status message
   @Receiver: NasModule
   */

class RilRequestGetSubProvisioningPrefMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestGetSubProvisioningPrefMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_HOOK_GET_UICC_PROVISION_PREFERENCE";

    RilRequestGetSubProvisioningPrefMessage() = delete;
    ~RilRequestGetSubProvisioningPrefMessage() {}

    inline RilRequestGetSubProvisioningPrefMessage(std::shared_ptr<MessageContext> context)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }
};
