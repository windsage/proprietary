/**
* Copyright (c) 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#ifndef GETDATAPPDATACAPABILITYMESSAGE
#define GETDATAPPDATACAPABILITYMESSAGE

#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

class GetDataPPDataCapabilityMessage : public SolicitedMessage<bool>,
                           public add_message_id<GetDataPPDataCapabilityMessage> {

  public:
    static constexpr const char *MESSAGE_NAME = "GetDataPPDataCapabilityMessage";
    inline GetDataPPDataCapabilityMessage():
      SolicitedMessage<bool>(get_class_message_id())
    {
      mName = MESSAGE_NAME;
    }
    ~GetDataPPDataCapabilityMessage() = default;

    string dump(){return mName;}
};
}
#endif
