/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef AUTODDSSWITCHCONTROLINDMESSAGE
#define AUTODDSSWITCHCONTROLINDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

enum class AutoDdsSwitchControl_t : uint8_t {
  AUTO_DDS_SWITCH_CAPABILITY_CHANGED  = 0,
  AUTO_DDS_SWITCH_ENABLED             = 1,
  AUTO_DDS_SWITCH_DISABLED            = 2,
};

/********************** Class Definitions *************************/
class AutoDdsSwitchControlIndMessage : public UnSolicitedMessage,
                           public add_message_id<AutoDdsSwitchControlIndMessage> {

private:
    AutoDdsSwitchControl_t mControl;

public:
    static constexpr const char *MESSAGE_NAME = "AutoDdsSwitchControlIndMessage";

    AutoDdsSwitchControlIndMessage() = delete;
   ~AutoDdsSwitchControlIndMessage() = default;
    AutoDdsSwitchControlIndMessage(AutoDdsSwitchControl_t control):
    UnSolicitedMessage(get_class_message_id()), mControl(control)
    {
      mName = MESSAGE_NAME;
    }

    AutoDdsSwitchControl_t getAutoDdsSwitchControl() {return mControl;}

    std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::make_shared<AutoDdsSwitchControlIndMessage>(mControl);
    }

    string dump()
    {
      std::stringstream ss;
      ss << MESSAGE_NAME << " " << (int)getAutoDdsSwitchControl();
      return ss.str();
    }

};

} //namespace

#endif