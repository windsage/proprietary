/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "telephony/ril.h"

/* Request to set usage setting message
   @Receiver: NasModule
   */

class RilRequestSetUsageSettingMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestSetUsageSettingMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_SET_USAGE_SETTING";

    RilRequestSetUsageSettingMessage() = delete;
    ~RilRequestSetUsageSettingMessage() {}

    inline RilRequestSetUsageSettingMessage(std::shared_ptr<MessageContext> context,
        RIL_UsageSettingMode mode) : QcRilRequestMessage(get_class_message_id(), context), mUsageSettingMode(mode) {
      mName = MESSAGE_NAME;
    }
    inline int getUsageSettingMode() { return mUsageSettingMode; }
private:
    RIL_UsageSettingMode mUsageSettingMode;
};
