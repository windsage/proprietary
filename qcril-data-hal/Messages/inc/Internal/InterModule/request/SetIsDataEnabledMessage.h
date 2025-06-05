/**
* Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef SETISDATAENABLEDMESSAGE
#define SETISDATAENABLEDMESSAGE

#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "DataCommon.h"

namespace rildata {

class SetIsDataEnabledMessage : public SolicitedMessage<RIL_Errno>,
                                public add_message_id<SetIsDataEnabledMessage> {
  private:
    bool mDataEnabled;

  public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_SET_IS_DATA_ENABLED";
    SetIsDataEnabledMessage()= delete;
    string dump(){return mName;}
    inline SetIsDataEnabledMessage( bool dataEnabled):
               SolicitedMessage<RIL_Errno>(get_class_message_id()),mDataEnabled(dataEnabled) {
      mName = MESSAGE_NAME;
    }
    ~SetIsDataEnabledMessage() {
      if (mCallback) {
        delete mCallback;
        mCallback = nullptr;
      }
    }
    boolean getDataState() {
          return mDataEnabled;
    }
};
}
#endif