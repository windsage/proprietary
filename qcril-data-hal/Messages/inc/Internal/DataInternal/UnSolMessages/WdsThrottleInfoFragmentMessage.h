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
class WdsThrottleInfoFragmentMessage: public UnSolicitedMessage,
                           public add_message_id<WdsThrottleInfoFragmentMessage> {

private:
  std::list<WdsThrottleInfo> throttleInfo;

public:
  static constexpr const char *MESSAGE_NAME = "WdsThrottleInfoFragmentMessage";

  WdsThrottleInfoFragmentMessage() = delete;
  WdsThrottleInfoFragmentMessage(std::list<WdsThrottleInfo> throttleInfo)
  : UnSolicitedMessage(get_class_message_id()), throttleInfo(throttleInfo) {
    mName = MESSAGE_NAME;
  }
  ~WdsThrottleInfoFragmentMessage(){};

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<WdsThrottleInfoFragmentMessage>(throttleInfo);
  }

  std::list<WdsThrottleInfo> getParams() {
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
