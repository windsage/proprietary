/**
* Copyright (c) 2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef SECUREMODECHANGEDCALLBACKMESSAGE
#define SECUREMODECHANGEDCALLBACKMESSAGE

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include <framework/add_message_id.h>

namespace rildata {

class SecureModeChangedCallBackMessage : public UnSolicitedMessage,
                             public add_message_id<SecureModeChangedCallBackMessage> {
private:
   uint8_t mState;

public:
  static constexpr const char *MESSAGE_NAME = "SecureModeChangedCallBackMessage";
  ~SecureModeChangedCallBackMessage() = default;

  inline SecureModeChangedCallBackMessage(uint8_t state)
        : UnSolicitedMessage(get_class_message_id())
  {
    mState = state;
    mName = MESSAGE_NAME;
  }

  inline std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::static_pointer_cast<UnSolicitedMessage>
          (std::make_shared<SecureModeChangedCallBackMessage>(mState));
  }

  inline uint8_t getState() {
    return mState;
  }

  inline string dump()
  {
    std::stringstream ss;
    ss << mName << " State =" << (int)mState;
    return ss.str();
  }
};
} //namespace

#endif