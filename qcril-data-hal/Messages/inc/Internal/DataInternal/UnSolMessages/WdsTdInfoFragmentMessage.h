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
class WdsTdInfoFragmentMessage: public UnSolicitedMessage,
                           public add_message_id<WdsTdInfoFragmentMessage> {

private:
  TrafficDescriptorFragment_t mTdFrag;

public:
  static constexpr const char *MESSAGE_NAME = "WdsTdInfoFragmentMessage";

  WdsTdInfoFragmentMessage() = delete;
  WdsTdInfoFragmentMessage(TrafficDescriptorFragment_t tdfrags)
  : UnSolicitedMessage(get_class_message_id()), mTdFrag(tdfrags) {
    mName = MESSAGE_NAME;
  }
  ~WdsTdInfoFragmentMessage(){};

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<WdsTdInfoFragmentMessage>(mTdFrag);
  }

  TrafficDescriptorFragment_t getParams() {
    return mTdFrag;
  }

  std::string dump() {
    std::stringstream ss;
    ss << mName << " ";
    mTdFrag.dump("", ss);
    return ss.str();
  }
};

} //namespace
