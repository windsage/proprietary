/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to get usage setting message
   @Receiver: NasModule
   */

class RilRequestGetUsageSettingMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestGetUsageSettingMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_GET_USAGE_SETTING";

    RilRequestGetUsageSettingMessage() = delete;
    ~RilRequestGetUsageSettingMessage() {}

    inline RilRequestGetUsageSettingMessage(std::shared_ptr<MessageContext> context)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }
};
