/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef REGISTERFORSECUREMODECBMESSAGE
#define REGISTERFORSECUREMODECBMESSAGE

#include <framework/UnSolicitedMessage.h>
#include <framework/Dispatcher.h>
#include <framework/add_message_id.h>

namespace rildata {

class RegisterForSecureModeCbMessage: public UnSolicitedMessage, public add_message_id<RegisterForSecureModeCbMessage>
{
public:
    static constexpr const char *MESSAGE_NAME = "RegisterForSecureModeCbMessage";
    inline RegisterForSecureModeCbMessage():
        UnSolicitedMessage(get_class_message_id())
    {
        mName = MESSAGE_NAME;
    }
     ~RegisterForSecureModeCbMessage() { }
    virtual string dump() {
        return MESSAGE_NAME;
    }
    std::shared_ptr<UnSolicitedMessage> clone() {
        return std::make_shared<RegisterForSecureModeCbMessage>();
    }
};

}
#endif