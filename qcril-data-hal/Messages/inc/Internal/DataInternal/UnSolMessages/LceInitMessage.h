/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef LCEINITMESSAGE
#define LCEINITMESSAGE

#include <framework/UnSolicitedMessage.h>
#include <framework/Dispatcher.h>
#include <framework/add_message_id.h>

namespace rildata {

class LceInitMessage: public UnSolicitedMessage, public add_message_id<LceInitMessage>
{
public:
    static constexpr const char *MESSAGE_NAME = "LceInitMessage";
    inline LceInitMessage():
        UnSolicitedMessage(get_class_message_id())
    {
        mName = MESSAGE_NAME;
    }
     ~LceInitMessage() { }
    virtual string dump() {
        return MESSAGE_NAME;
    }
    std::shared_ptr<UnSolicitedMessage> clone() {
        return std::make_shared<LceInitMessage>();
    }
};

}
#endif