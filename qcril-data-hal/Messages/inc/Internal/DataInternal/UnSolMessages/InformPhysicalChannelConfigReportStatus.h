/**
* Copyright (c) 2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef INFORMPHYSICALCHANNELCONFIGREPORTSTATUS
#define INFORMPHYSICALCHANNELCONFIGREPORTSTATUS

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "DataCommon.h"

namespace rildata {

class InformPhysicalChannelConfigReportStatus : public UnSolicitedMessage,
                                        public add_message_id<InformPhysicalChannelConfigReportStatus> {
    public:
    boolean mEnable;
    static constexpr const char *MESSAGE_NAME = "InformPhysicalChannelConfigReportStatus";

    InformPhysicalChannelConfigReportStatus() = delete;

    inline InformPhysicalChannelConfigReportStatus( boolean enable ):
                     UnSolicitedMessage(get_class_message_id()) {
        mName = MESSAGE_NAME;
        mEnable = enable;
  }
  ~InformPhysicalChannelConfigReportStatus() {}

  string dump()
  {
    return MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::make_shared<InformPhysicalChannelConfigReportStatus>(mEnable);
  }
};
}

#endif
