/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to exit SCBM Message
   @Receiver: ImsModule
   */

class QcRilRequestImsExitSmsCallbackModeMessage : public QcRilRequestMessage,
    public add_message_id<QcRilRequestImsExitSmsCallbackModeMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "QcRilRequestImsExitSmsCallbackModeMessage";

    QcRilRequestImsExitSmsCallbackModeMessage() = delete;
    ~QcRilRequestImsExitSmsCallbackModeMessage() {}

    inline QcRilRequestImsExitSmsCallbackModeMessage(std::shared_ptr<MessageContext> context)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }
};
