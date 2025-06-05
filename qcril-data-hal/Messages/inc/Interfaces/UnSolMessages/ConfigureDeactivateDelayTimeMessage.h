/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef CONFIGUREDEACTIVATEDELAYTIMEMESSAGE
#define CONFIGUREDEACTIVATEDELAYTIMEMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"


namespace rildata {

/********************** Class Definitions *************************/
class ConfigureDeactivateDelayTimeMessage: public UnSolicitedMessage,
                           public add_message_id<ConfigureDeactivateDelayTimeMessage> {
private:
  uint64_t delayTimeMilliSecs;

public:
  static constexpr const char *MESSAGE_NAME = "ConfigureDeactivateDelayTimeMessage";

  ConfigureDeactivateDelayTimeMessage() = delete;
  ConfigureDeactivateDelayTimeMessage(uint64_t millisecs):
    UnSolicitedMessage(get_class_message_id()), delayTimeMilliSecs(millisecs) {}
  ~ConfigureDeactivateDelayTimeMessage() {}

  uint64_t getDelayTimeMilliSecs() { return delayTimeMilliSecs; }

std::shared_ptr<UnSolicitedMessage> clone()
{
    return std::make_shared<ConfigureDeactivateDelayTimeMessage>(delayTimeMilliSecs);
}

std::string dump()
{
    std::string msg = std::string(MESSAGE_NAME) + " " + std::to_string(delayTimeMilliSecs);
    return msg;
}

};

} //namespace
#endif