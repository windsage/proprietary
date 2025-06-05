/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
******************************************************************************/
#ifndef AIDL_DEATH_MESSAGE
#define AIDL_DEATH_MESSAGE

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

namespace rildata {

class AidlDeathMessage: public UnSolicitedMessage, public add_message_id<AidlDeathMessage>
{

public:
  static constexpr const char *MESSAGE_NAME = "AIDL_DEATH_MESSAGE";
  typedef enum Type {
    DATA_ACTIVITY,
    KEEP_ALIVE,
  };

  AidlDeathMessage(Type t, void* p):
          UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mType = t;
    mCookie = p;
  }
  ~AidlDeathMessage() {};

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<AidlDeathMessage>(
        getType(), getCookie());
    return msg;
  }
  string dump() {
    return std::string(MESSAGE_NAME);
  }
  Type getType() {
    return mType;
  }
  void* getCookie() {
    return mCookie;
  }

private:
  Type mType;
  void* mCookie;
};

} //namespace rildata

#endif //AIDL_DEATH_MESSAGE
