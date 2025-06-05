/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to get sim iccid message
   @Receiver: NasModule
   */

class RilRequestGetSimIccidMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestGetSimIccidMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_REQ_HOOK_GET_SIM_ICCID_REQ";

    RilRequestGetSimIccidMessage() = delete;
    ~RilRequestGetSimIccidMessage() {}

    inline RilRequestGetSimIccidMessage(std::shared_ptr<MessageContext> context)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }
};
