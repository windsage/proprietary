/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
******************************************************************************/
#ifndef DATA_INACTIVITY_TIMER_EXP_MESSAGE
#define DATA_INACTIVITY_TIMER_EXP_MESSAGE

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

namespace rildata {

class DataInActivityTimerExpiredMessage :
                    public UnSolicitedMessage,
                    public add_message_id<DataInActivityTimerExpiredMessage>
{

public:
  static constexpr const char *MESSAGE_NAME = "DataInActivityTimerExpiredMessage";

  DataInActivityTimerExpiredMessage(int apnBitMask):
          UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mApnBitMask = apnBitMask;
  }
  ~DataInActivityTimerExpiredMessage() {};

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<DataInActivityTimerExpiredMessage>(
        getApnBitMask());
    return msg;
  }
  string dump() {
    return std::string(MESSAGE_NAME);
  }
  int getApnBitMask() {
    return mApnBitMask;
  }

private:
  int mApnBitMask;
};

} //namespace rildata

#endif //DATA_INACTIVITY_TIMER_EXP_MESSAGE