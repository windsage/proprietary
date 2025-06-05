/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to set NR disable mode message
   @Receiver: NasModule
*/

class RilRequestSetNrDisableModeMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestSetNrDisableModeMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_SET_NR_DISABLE_MODE";

    RilRequestSetNrDisableModeMessage() = delete;
    ~RilRequestSetNrDisableModeMessage() {}

    inline RilRequestSetNrDisableModeMessage(std::shared_ptr<MessageContext> context,
        RIL_NR_DISABLE_MODE config) : QcRilRequestMessage(get_class_message_id(), context), mConfig(config) {
      mName = MESSAGE_NAME;
    }
    inline RIL_NR_DISABLE_MODE getConfig() { return mConfig; }
private:
    RIL_NR_DISABLE_MODE mConfig;
};
