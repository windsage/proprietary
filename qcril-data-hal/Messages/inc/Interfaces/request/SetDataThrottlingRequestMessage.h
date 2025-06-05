/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef SETDATATHROTTLINGREQUESTMESSAGE
#define SETDATATHROTTLINGREQUESTMESSAGE
#include "MessageCommon.h"

namespace rildata {

enum class DataThrottleAction_t : int32_t {
  NoDataThrottle              = 0,
  ThrottleSecondaryCarrier    = 1,
  ThrottleAnchorCarrier       = 2,
  Hold                        = 3,
};

class SetDataThrottlingRequestMessage : public SolicitedMessage<RIL_Errno>,
                                  public add_message_id<SetDataThrottlingRequestMessage> {
  private:
    DataThrottleAction_t mAction;
    int64_t mCompleteDurationMills;

  public:
    static constexpr const char* MESSAGE_NAME = "SetDataThrottlingRequestMessage";
    SetDataThrottlingRequestMessage() = delete;
    SetDataThrottlingRequestMessage(
      const DataThrottleAction_t action,
      const int64_t duration
      ):SolicitedMessage<RIL_Errno>(get_class_message_id()) {
      mName = MESSAGE_NAME;
      mAction = action;
      mCompleteDurationMills = duration;
    }
    ~SetDataThrottlingRequestMessage() = default;

    string dump(){
      std::stringstream ss;
      ss << "[" << mName << "] ";
      ss << "action=" << (int)mAction <<",";
      ss << "duration=" << mCompleteDurationMills <<",";
      return ss.str();
    }

    DataThrottleAction_t getDataThrottleAction() {return mAction;}
    int64_t getCompleteDurationMills() {return mCompleteDurationMills;}
};

}//namespace

#endif