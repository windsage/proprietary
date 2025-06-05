/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "interfaces/nas/nas_types.h"

/* Request to set sub prov state message
   @Receiver: NasModule
   */

class RilRequestSetSubProvisioningPrefMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestSetSubProvisioningPrefMessage> {
private:
    qcril::interfaces::RILSetSubProvState_t mState;
public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_HOOK_SET_UICC_PROVISION_PREFERENCE";

    RilRequestSetSubProvisioningPrefMessage() = delete;
    ~RilRequestSetSubProvisioningPrefMessage() {}

    inline RilRequestSetSubProvisioningPrefMessage(std::shared_ptr<MessageContext> context,
        qcril::interfaces::RILSetSubProvState_t state)
        : QcRilRequestMessage(get_class_message_id(), context), mState(state) {
      mName = MESSAGE_NAME;
    }
    qcril::interfaces::RILSetSubProvState_t& getState() {
        return mState;
    }
};
