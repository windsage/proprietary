/**
* Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef SETISDATAROAMINGENABLEDMESSAGE
#define SETISDATAROAMINGENABLEDMESSAGE

#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "DataCommon.h"

namespace rildata {

class SetIsDataRoamingEnabledMessage : public SolicitedMessage<RIL_Errno>,
                                       public add_message_id<SetIsDataRoamingEnabledMessage> {
  private:
    bool mDataRoamingEnabled;

  public:
    static constexpr const char *MESSAGE_NAME = "SetIsDataRoamingEnabledMessage";
    SetIsDataRoamingEnabledMessage() = delete;
    string dump(){return mName;}
    inline SetIsDataRoamingEnabledMessage( boolean isDataRoamingEnabled ):
              SolicitedMessage<RIL_Errno>(get_class_message_id()),mDataRoamingEnabled(isDataRoamingEnabled) {
        mName = MESSAGE_NAME;
    }
    ~SetIsDataRoamingEnabledMessage() {
      if (mCallback) {
        delete mCallback;
        mCallback = nullptr;
      }
    }
    bool getDataRoamingState() {
        return mDataRoamingEnabled;
    }
};
}

#endif