/******************************************************************************
#  Copyright (c) 2018-2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef SETINITIALATTACHAPNREQUESTMESSAGE
#define SETINITIALATTACHAPNREQUESTMESSAGE
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"
#include "telephony/ril.h"

using namespace std;

namespace rildata {
class SetInitialAttachApnRequestMessage : public SolicitedMessage<RIL_Errno>,
                              public add_message_id<SetInitialAttachApnRequestMessage>
{
private:
    int32_t mSerial;
    shared_ptr<function<void(uint16_t)>> mAcknowlegeRequestCb;
    std::optional<DataProfileInfo_t> mProfile;

    public:
    static constexpr const char* MESSAGE_NAME = "SetInitialAttachApnRequestMessage";
    SetInitialAttachApnRequestMessage() = delete;
    SetInitialAttachApnRequestMessage(
        const int32_t serial,
        const DataProfileInfo_t profile,
        const shared_ptr<function<void(uint16_t)>> ackCb
        ):SolicitedMessage<RIL_Errno>(get_class_message_id()) {

      mName = MESSAGE_NAME;
      mSerial = serial;
      mProfile = profile;
      mAcknowlegeRequestCb = ackCb;
    }
    SetInitialAttachApnRequestMessage(
        const int32_t serial,
        const shared_ptr<function<void(uint16_t)>> ackCb
        ):SolicitedMessage<RIL_Errno>(get_class_message_id()) {

      mName = MESSAGE_NAME;
      mSerial = serial;
      mAcknowlegeRequestCb = ackCb;
    }
    ~SetInitialAttachApnRequestMessage() = default;
    string dump() {
      std::stringstream ss;
      ss << mName << " Serial=[" << (int)getSerial() << "] ";
      if(mProfile.has_value())
      {
        mProfile.value().dump("", ss);
      }
      return ss.str();
    }
    DataProfileInfo_t getProfile() {return (mProfile.value());}
    int32_t getSerial() {return mSerial;}
    bool hasProfileInfo() {return mProfile.has_value();}
};
}


#endif
