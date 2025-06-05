/**
* Copyright (c) 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#ifndef GETCIWLANCAPABILITYMESSAGE
#define GETCIWLANCAPABILITYMESSAGE

#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

struct CIWlanCapabilitySupport_t {
  bool cap;
  CIWlanCapabilityType type;
  void dump(std::string padding, std::ostream& os) const {
    os << padding << "cap=" << (int)cap;
    os << padding << "type=" << (int)type;
  }
};

class GetCIWlanCapabilityMessage : public SolicitedMessage<CIWlanCapabilitySupport_t>,
                           public add_message_id<GetCIWlanCapabilityMessage> {

  public:
    static constexpr const char *MESSAGE_NAME = "GetCIWlanCapabilityMessage";
    inline GetCIWlanCapabilityMessage():
      SolicitedMessage<CIWlanCapabilitySupport_t>(get_class_message_id())
    {
      mName = MESSAGE_NAME;
    }
    ~GetCIWlanCapabilityMessage() = default;

    string dump(){return mName;}
};
}
#endif
