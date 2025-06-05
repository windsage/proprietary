/**
* Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef INFORMHANDOVERFAILUREMESSAGE
#define INFORMHANDOVERFAILUREMESSAGE

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "DataCommon.h"

namespace rildata {

class InformHandoverFailureMessage : public UnSolicitedMessage,
                                        public add_message_id<InformHandoverFailureMessage> {
    private:
    ApnTypes_t  mApnType;
    string mApn;

    public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_INFORM_HANDOVER_FAILURE_MESSAGE";

    InformHandoverFailureMessage() = delete;

    inline InformHandoverFailureMessage( ApnTypes_t  type, string apn ):
                     UnSolicitedMessage(get_class_message_id()) {
        mName = MESSAGE_NAME;
        mApnType = type;
        mApn = apn;
  }
  ~InformHandoverFailureMessage() {}

  string dump()
  {
    return MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::make_shared<InformHandoverFailureMessage>(mApnType, mApn);
  }

  ApnTypes_t getApnType()
  {
    return mApnType;
  }

  string getApn()
  {
    return mApn;
  }
};
}

#endif
