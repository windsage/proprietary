/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#ifndef GETSMARTTEMPDDSSWITCHCAPABILITYMESSAGE
#define GETSMARTTEMPDDSSWITCHCAPABILITYMESSAGE

#include "MessageCommon.h"

using namespace std;

namespace rildata {

enum class SmartDdsSwitchCapability_t : uint8_t {
  SMART_DDS_SWITCH_CAPABILITY_UNKNOWN        = 0,
  SMART_DDS_SWITCH_CAPABILITY_SUPPORTED      = 1,
  SMART_DDS_SWITCH_CAPABILITY_NOT_SUPPORTED  = 2,
};

class GetSmartTempDdsSwitchCapabilityMessage : public SolicitedMessage<SmartDdsSwitchCapability_t>,
                          public add_message_id<GetSmartTempDdsSwitchCapabilityMessage> {
  public:

    static constexpr const char *MESSAGE_NAME = "GetSmartTempDdsSwitchCapabilityMessage";

    inline GetSmartTempDdsSwitchCapabilityMessage():
      SolicitedMessage<SmartDdsSwitchCapability_t>(get_class_message_id())
    {
      mName = MESSAGE_NAME;
    }
    ~GetSmartTempDdsSwitchCapabilityMessage() = default;

    string dump(){return mName;}
};

}
#endif