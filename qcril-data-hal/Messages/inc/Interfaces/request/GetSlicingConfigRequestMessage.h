/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef GETSLICINGCONFIGREQUESTMESSAGE
#define GETSLICINGCONFIGREQUESTMESSAGE

#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

struct GetSlicingConfigResp_t {
  SlicingConfig_t slicingConfig;
  ResponseError_t respErr;
};

class GetSlicingConfigRequestMessage : public SolicitedMessage<GetSlicingConfigResp_t>,
                          public add_message_id<GetSlicingConfigRequestMessage> {
  private:
    int32_t mSerial;
    std::shared_ptr<std::function<void(int32_t)>> mAcknowlegeRequestCb;

  public:
    static constexpr const char *MESSAGE_NAME = "GetSlicingConfigRequestMessage";
    GetSlicingConfigRequestMessage() = delete;
    GetSlicingConfigRequestMessage(const int32_t serial,
          const std::shared_ptr<std::function<void(int32_t)>> ackCb):
          SolicitedMessage<GetSlicingConfigResp_t>(get_class_message_id()) {
      mName = MESSAGE_NAME;
      mSerial = serial;
      mAcknowlegeRequestCb = ackCb;
    }
    ~GetSlicingConfigRequestMessage(){};

    string dump(){return mName;}

    int32_t getSerial() {return mSerial;}

    std::shared_ptr<std::function<void(int32_t)>> getAcknowlegeRequestCb() {
      return mAcknowlegeRequestCb;
    }
};

}

#endif
