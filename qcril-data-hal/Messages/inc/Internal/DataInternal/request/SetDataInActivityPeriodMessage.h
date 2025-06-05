/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef DATA_ACTIVITY_MESSAGE
#define DATA_ACTIVITY_MESSAGE

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

namespace rildata {

class SetDataInActivityPeriodMessage: public SolicitedMessage<ResponseError_t>,
                           public add_message_id<SetDataInActivityPeriodMessage>
{
private:
  uint32_t interval;
  rildata::ApnTypes_t apnType;
  //int requestId;
public:
  static constexpr const char *MESSAGE_NAME = "DATA_ACTIVITY_MESSAGE";

  SetDataInActivityPeriodMessage(rildata::ApnTypes_t type, uint32_t period):
          SolicitedMessage<ResponseError_t>(get_class_message_id()) {
    mName = MESSAGE_NAME;
    interval = period;
    apnType = type;
  }
  ~SetDataInActivityPeriodMessage() {};

  uint32_t getInterval() {
    return interval;
  }
  rildata::ApnTypes_t getApnType() {
    return apnType;
  }
  string dump() {
    return std::string(MESSAGE_NAME);
  }
};

} //namespace rildata

#endif //DATA_ACTIVITY_MESSAGE