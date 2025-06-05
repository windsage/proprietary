/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to send a Get system selection channels message.
   @Receiver: NasModule
*/

class RilRequestGetSysSelChannelsMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestGetSysSelChannelsMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS";

    RilRequestGetSysSelChannelsMessage() = delete;
    ~RilRequestGetSysSelChannelsMessage() {}

    inline RilRequestGetSysSelChannelsMessage(std::shared_ptr<MessageContext> context) :
        QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }
};
