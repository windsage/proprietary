/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class WdsThrottleInfoIndMessage: public UnSolicitedMessage,
                           public add_message_id<WdsThrottleInfoIndMessage> {

private:
  std::list<WdsThrottleInfo> throttleInfo;

public:
  static constexpr const char *MESSAGE_NAME = "WdsThrottleInfoIndMessage";

  WdsThrottleInfoIndMessage() = delete;
  WdsThrottleInfoIndMessage(std::list<WdsThrottleInfo> throttleInfo)
  : UnSolicitedMessage(get_class_message_id()), throttleInfo(throttleInfo) {
    mName = MESSAGE_NAME;
  }
  ~WdsThrottleInfoIndMessage(){};

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<WdsThrottleInfoIndMessage>(throttleInfo);
  }

  std::list<WdsThrottleInfo> getThrottleInfo() {
    return throttleInfo;
  }

  std::string dump() {
    std::stringstream ss;
    ss << mName << " ";
    for (auto info : throttleInfo) {
      ss << info.dump() << ",";
    }
    return ss.str();
  }
};

} //namespace
