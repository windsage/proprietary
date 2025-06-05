/*===========================================================================

  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef SETLOCALURSPCONFIGURATIONREQUESTMESSAGE
#define SETLOCALURSPCONFIGURATIONREQUESTMESSAGE
#include "MessageCommon.h"

using namespace std;

namespace rildata {

class SetLocalUrspConfigurationRequestMessage : public SolicitedMessage<RIL_Errno>,
                          public add_message_id<SetLocalUrspConfigurationRequestMessage> {
private:
  std::vector<UrspRule_t> mUrspRules;

public:
  static constexpr const char *MESSAGE_NAME = "SetLocalUrspConfigurationRequestMessage";
  SetLocalUrspConfigurationRequestMessage() = delete;
  SetLocalUrspConfigurationRequestMessage(std::vector<UrspRule_t> rules)
      :SolicitedMessage<RIL_Errno>(get_class_message_id()), mUrspRules(rules) {
    mName = MESSAGE_NAME;
  }
  ~SetLocalUrspConfigurationRequestMessage(){};

  string dump(){return mName;}
  std::vector<UrspRule_t> getUrspRules() {return mUrspRules;}
};

}

#endif