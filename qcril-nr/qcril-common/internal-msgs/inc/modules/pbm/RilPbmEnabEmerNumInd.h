/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef RIL_ENAB_EMER_NUM_IND
#define RIL_ENAB_EMER_NUM_IND

#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/**
 * Message to enable emergency number indications.
 */

class RilPbmEnabEmerNumInd : public SolicitedSyncMessage<bool>,public add_message_id<RilPbmEnabEmerNumInd> {

    private:
        bool status{false};

    public:
        static constexpr const char *MESSAGE_NAME = "QCRIL_ENAB_EMER_NUM_IND";

        inline RilPbmEnabEmerNumInd():SolicitedSyncMessage<bool>(get_class_message_id()) {
            mName = MESSAGE_NAME;
        }

        ~RilPbmEnabEmerNumInd();

        void setStatus(bool val);
        bool getStatus();
        string dump();
};


#endif // RIL_ENAB_EMER_NUM_IND
