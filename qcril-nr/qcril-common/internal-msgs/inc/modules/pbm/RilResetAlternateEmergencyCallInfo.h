/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef RIL_RESET_ALT_EMER_CALL_INFO
#define RIL_RESET_ALT_EMER_CALL_INFO

#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/**
 * Message to reset Alternate Emergency call info
 */

class RilResetAlternateEmergencyCallInfo : public SolicitedSyncMessage<bool>,public add_message_id<RilResetAlternateEmergencyCallInfo> {

    public:
        static constexpr const char *MESSAGE_NAME = "QCRIL_RESET_ALT_EMER_CALL_INFO";

        inline RilResetAlternateEmergencyCallInfo():SolicitedSyncMessage<bool>(get_class_message_id()) {
            mName = MESSAGE_NAME;
        }

        ~RilResetAlternateEmergencyCallInfo();

        string dump();
};


#endif // RIL_RESET_ALT_EMER_CALL_INFO
