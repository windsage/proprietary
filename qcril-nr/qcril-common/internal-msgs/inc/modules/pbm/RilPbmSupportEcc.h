/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef RIL_PBM_SUPPORTS_ECC
#define RIL_PBM_SUPPORTS_ECC

#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/**
 * Message to check if centralized ecc support is available
 */

class RilPbmSupportEcc : public SolicitedSyncMessage<bool>,public add_message_id<RilPbmSupportEcc> {
    public:
        static constexpr const char *MESSAGE_NAME = "QCRIL_PBM_SUPPORTS_ECC";

        inline RilPbmSupportEcc():SolicitedSyncMessage<bool>(get_class_message_id()) {
            mName = MESSAGE_NAME;
        }

        ~RilPbmSupportEcc();

        string dump();
};


#endif // RIL_PBM_SUPPORTS_ECC
