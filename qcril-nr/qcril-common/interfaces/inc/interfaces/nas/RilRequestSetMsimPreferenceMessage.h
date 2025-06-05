/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to set Msim preference message
   @Receiver: NasModule
*/

class RilRequestSetMsimPreferenceMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestSetMsimPreferenceMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_SET_MSIM_PREFERENCE";

    RilRequestSetMsimPreferenceMessage() = delete;
    ~RilRequestSetMsimPreferenceMessage() {}

    inline RilRequestSetMsimPreferenceMessage(std::shared_ptr<MessageContext> context,
        Msim_Preference preference) : QcRilRequestMessage(get_class_message_id(), context),
        mPreference(preference) {
      mName = MESSAGE_NAME;
    }
    inline Msim_Preference getPreference() { return mPreference; }
private:
    Msim_Preference mPreference;
};