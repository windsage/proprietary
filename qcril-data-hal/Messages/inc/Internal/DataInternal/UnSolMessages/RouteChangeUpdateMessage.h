/**
* Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
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
class RouteChangeUpdateMessage: public UnSolicitedMessage,
                           public add_message_id<RouteChangeUpdateMessage> {

private:
  int mCid;
  vector<TrafficDescriptor_t> mTds;

public:
  static constexpr const char *MESSAGE_NAME = "RouteChangeUpdateMessage";

  RouteChangeUpdateMessage() = delete;
  RouteChangeUpdateMessage(int cid, vector<TrafficDescriptor_t> tdInfo)
  : UnSolicitedMessage(get_class_message_id()), mCid(cid), mTds(tdInfo) {
    mName = MESSAGE_NAME;
  }
  ~RouteChangeUpdateMessage(){};

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RouteChangeUpdateMessage>(mCid, mTds);
  }

  int getCid() {
    return mCid;
  }

  vector<TrafficDescriptor_t> getParams() {
    return mTds;
  }

  std::string dump() {
    std::stringstream ss;
    ss << mName << "{" << mCid << ":";
    for (auto td: mTds)
    {
      td.dump("td=", ss);
    }
     ss << " }";
    return ss.str();
  }
};

} //namespace
