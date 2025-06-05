/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef REGISTER_SECURE_MODE_INDICATION
#define REGISTER_SECURE_MODE_INDICATION

#include <framework/UnSolicitedMessage.h>
#include <framework/Dispatcher.h>
#include <framework/add_message_id.h>

class RegisterSecureModeIndicationMessage: public UnSolicitedMessage, public add_message_id<RegisterSecureModeIndicationMessage>
{
public:
    static constexpr const char *MESSAGE_NAME = "RegisterSecureModeIndicationMessage";
    inline RegisterSecureModeIndicationMessage():
        UnSolicitedMessage(get_class_message_id())
    {
        mName = MESSAGE_NAME;
    }
     ~RegisterSecureModeIndicationMessage() { }
    virtual string dump() {
        return MESSAGE_NAME;
    }
    std::shared_ptr<UnSolicitedMessage> clone() {
        return std::make_shared<RegisterSecureModeIndicationMessage>();
    }
};

#endif
