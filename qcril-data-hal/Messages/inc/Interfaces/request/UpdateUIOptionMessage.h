/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#ifndef UPDATEUIOPTIONMESSAGE
#define UPDATEUIOPTIONMESSAGE
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
class UpdateUIOptionMessage : public SolicitedMessage<RIL_Errno>,
                              public add_message_id<UpdateUIOptionMessage> {
private:
  bool mCIWlan;

public:
  static constexpr const char* MESSAGE_NAME = "UpdateUIOptionMessage";
  UpdateUIOptionMessage():SolicitedMessage<RIL_Errno>(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mCIWlan = false;
  }
  ~UpdateUIOptionMessage() = default;

  bool getCIWlanUIOption() {return mCIWlan;}
  void setCIWlanUIOption(bool selection) {mCIWlan = selection;}

  string dump() {
    stringstream ss;
    ss << mName << " CIWlan=" << (getCIWlanUIOption()?"true":"false");
    return ss.str();
  }
};

}

#endif