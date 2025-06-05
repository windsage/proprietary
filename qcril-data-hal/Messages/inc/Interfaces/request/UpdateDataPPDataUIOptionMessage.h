/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#ifndef UPDATEDATAPPDATAUIOPTIONMESSAGE
#define UPDATEDATAPPDATAUIOPTIONMESSAGE
#include "framework/Message.h"
#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include <modules/android/RilRequestMessage.h>
#include "telephony/ril.h"
#include "MessageCommon.h"

using namespace std;

namespace rildata {

/********************** Class Definitions *************************/
class UpdateDataPPDataUIOptionMessage : public SolicitedMessage<RIL_Errno>,
                              public add_message_id<UpdateDataPPDataUIOptionMessage> {
private:
  bool mDataPPData;

public:
  static constexpr const char* MESSAGE_NAME = "UpdateDataPPDataUIOptionMessage";
  UpdateDataPPDataUIOptionMessage(const bool dataUI):SolicitedMessage<RIL_Errno>(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mDataPPData = dataUI;
  }
  ~UpdateDataPPDataUIOptionMessage() = default;

  bool getDataPPDataUIOption() {return mDataPPData;}

  string dump() {
    stringstream ss;
    ss << mName << " CIWlan=" << (getDataPPDataUIOption()?"true":"false");
    return ss.str();
  }
};

}

#endif