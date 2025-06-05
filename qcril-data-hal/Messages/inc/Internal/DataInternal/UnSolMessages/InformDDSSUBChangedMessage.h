/**
* Copyright (c) 2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef INFORMDDSSUBCHANGEDMESSAGE
#define INFORMDDSSUBCHANGEDMESSAGE

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "DataCommon.h"

namespace rildata {

class InformDDSSUBChangedMessage : public UnSolicitedMessage,
                                        public add_message_id<InformDDSSUBChangedMessage> {
  public:
    static constexpr const char *MESSAGE_NAME = "InformDDSSubChangedMessage";

    InformDDSSUBChangedMessage():UnSolicitedMessage(get_class_message_id()) {
        mName = MESSAGE_NAME;
    }
    ~InformDDSSUBChangedMessage() {}

    string dump() {
      return MESSAGE_NAME;
    }

    std::shared_ptr<UnSolicitedMessage> clone() {
      return std::make_shared<InformDDSSUBChangedMessage>();
    }
};

}
#endif
