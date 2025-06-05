/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef RIL_NUM_IS_EMERGENCY
#define RIL_NUM_IS_EMERGENCY

#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/**
 * Message to check if the phone number is an emergency number and is part of the ecc list
 */

class RilNumberIsEmergency : public SolicitedSyncMessage<bool>,public add_message_id<RilNumberIsEmergency> {
    private:
        char* number;
    public:
        static constexpr const char *MESSAGE_NAME = "QCRIL_CHECK_NUMBER_IS_EMERGENCY";

        inline RilNumberIsEmergency():SolicitedSyncMessage<bool>(get_class_message_id()) {
            mName = MESSAGE_NAME;
        }

        ~RilNumberIsEmergency();
        void setNumber( char* number);
        char* getNumber();

        string dump();
};


#endif // RIL_NUM_IS_EMERGENCY
