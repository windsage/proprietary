/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef THROTTLEDAPNTIMEREXPIRATIONMESSAGE
#define THROTTLEDAPNTIMEREXPIRATIONMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class ThrottledApnTimerExpirationMessage: public UnSolicitedMessage,
                           public add_message_id<ThrottledApnTimerExpirationMessage> {
private:
  RequestSource_t src;
  rildata::DataProfileInfo_t profileInfo;

public:
  static constexpr const char *MESSAGE_NAME = "ThrottledApnTimerExpirationMessage";

  ThrottledApnTimerExpirationMessage() = delete;
  ThrottledApnTimerExpirationMessage(DataProfileInfo_t info, RequestSource_t src):
    UnSolicitedMessage(get_class_message_id()), src(src), profileInfo(info) {
        profileInfo.apn = profileInfo.originalapn;
  }
  ~ThrottledApnTimerExpirationMessage() {}

  std::string getApn() { return profileInfo.apn; }
  RequestSource_t getSrc() { return src; }


std::shared_ptr<UnSolicitedMessage> clone()
{
    return std::make_shared<ThrottledApnTimerExpirationMessage>(profileInfo, src);
}

std::string dump()
{
    std::string msg = std::string(MESSAGE_NAME) + " " + profileInfo.apn + " " + std::to_string(static_cast<int>(src));
    return msg;
}

rildata::DataProfileInfo_t getProfileInfo()
{
    return profileInfo;
}

};

} //namespace
#endif
