/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef RIL_SEND_ECC_LIST_IND
#define RIL_SEND_ECC_LIST_IND

#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/**
 * Message to send ecc list indication
 */

class RilPbmSendEccListInd : public SolicitedSyncMessage<bool>,public add_message_id<RilPbmSendEccListInd> {

    public:
        static constexpr const char *MESSAGE_NAME = "QCRIL_SEND_ECC_LIST_IND";

        inline RilPbmSendEccListInd():SolicitedSyncMessage<bool>(get_class_message_id()) {
            mName = MESSAGE_NAME;
        }

        ~RilPbmSendEccListInd();

        string dump();
};


#endif // RIL_SEND_ECC_LIST_IND
