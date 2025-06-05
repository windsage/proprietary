/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef LCEDEINITMESSAGE
#define LCEDEINITMESSAGE

#include <framework/UnSolicitedMessage.h>
#include <framework/Dispatcher.h>
#include <framework/add_message_id.h>

namespace rildata {

class LceDeInitMessage: public UnSolicitedMessage, public add_message_id<LceDeInitMessage>
{
public:
    static constexpr const char *MESSAGE_NAME = "LceDeInitMessage";
    inline LceDeInitMessage():
        UnSolicitedMessage(get_class_message_id())
    {
        mName = MESSAGE_NAME;
    }
     ~LceDeInitMessage() { }
    virtual string dump() {
        return MESSAGE_NAME;
    }
    std::shared_ptr<UnSolicitedMessage> clone() {
        return std::make_shared<LceDeInitMessage>();
    }
};

}
#endif