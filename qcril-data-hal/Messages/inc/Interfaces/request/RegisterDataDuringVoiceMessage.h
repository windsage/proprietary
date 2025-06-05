/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#ifndef REGISTERDATADURINGVOICEMESSAGE
#define REGISTERDATADURINGVOICEMESSAGE
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

enum class DdsSwitchUserSelection_t {
  DISABLE_DATA_DURING_VOICE = 0,
  ENABLE_DATA_DURING_VOICE  = 1,
};

/********************** Class Definitions *************************/
class RegisterDataDuringVoiceMessage : public SolicitedMessage<RIL_Errno>,
                              public add_message_id<RegisterDataDuringVoiceMessage> {
    private:
    SubscriptionId_t mSubId;
    DdsSwitchUserSelection_t mDdsSwitchUserSelection;

    public:
    static constexpr const char* MESSAGE_NAME = "RegisterDataDuringVoiceMessage";
    RegisterDataDuringVoiceMessage() = delete;
    RegisterDataDuringVoiceMessage(
        const SubscriptionId_t subId,
        const DdsSwitchUserSelection_t selection
        ):SolicitedMessage<RIL_Errno>(get_class_message_id()) {
      mName = MESSAGE_NAME;
      mSubId = subId;
      mDdsSwitchUserSelection = selection;
    }
    ~RegisterDataDuringVoiceMessage() = default;

    SubscriptionId_t getSubscriptionId() { return mSubId; }
    DdsSwitchUserSelection_t getUserSelection() { return mDdsSwitchUserSelection; }

    string dump() {
      stringstream ss;
      ss << mName << "SubId=[" << (int)getSubscriptionId() << "], ";
      ss << "Select=[" << (int)getUserSelection() << "],";
      return ss.str();
    }
};

}

#endif