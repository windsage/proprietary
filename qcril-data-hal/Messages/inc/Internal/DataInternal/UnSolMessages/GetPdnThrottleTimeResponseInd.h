/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef GETPDNTHROTTLETIMERESPONSEIND
#define GETPDNTHROTTLETIMERESPONSEIND
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

namespace rildata {

/********************** Class Definitions *************************/
class GetPdnThrottleTimeResponseInd: public UnSolicitedMessage,
                                     public add_message_id<GetPdnThrottleTimeResponseInd> {
private:
  int mCid;
  int64_t mThrottleTime;

public:
  static constexpr const char *MESSAGE_NAME = "GetPdnThrottleTimeResponseInd";

  GetPdnThrottleTimeResponseInd() = delete;
  GetPdnThrottleTimeResponseInd(int cid, int64_t throttleTime)
  : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mCid = cid;
    mThrottleTime = throttleTime;
  }
  ~GetPdnThrottleTimeResponseInd(){};

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<GetPdnThrottleTimeResponseInd>(mCid, mThrottleTime);
  }

  int getCid() {
    return mCid;
  }

  int64_t getThrottleTime() {
    return mThrottleTime;
  }

  string dump() {
    return std::string(MESSAGE_NAME) + " " + std::to_string(mCid) + " " + std::to_string(mThrottleTime);
  }
};

} //namespace

#endif