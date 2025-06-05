/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef RIL_PBM_PROCESS_DIALCALL_REQ
#define RIL_PBM_PROCESS_DIALCALL_REQ

#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/**
 * Message to :
 * 1) Check if number is alternate emergency call
 * 2) Get alternate emergency call category
 * 3) Check if number is non std
 */

class RilPbmProcessAltEmerCall : public SolicitedSyncMessage<bool>,public add_message_id<RilPbmProcessAltEmerCall> {
    private:
        char* number;
        uint32_t callCateg{0};
        bool is_non_std_otasp{false};
    public:
        static constexpr const char *MESSAGE_NAME = "QCRIL_PBM_PROCESS_DIALCALL_REQ";

        inline RilPbmProcessAltEmerCall():SolicitedSyncMessage<bool>(get_class_message_id()) {
            mName = MESSAGE_NAME;
        }

        ~RilPbmProcessAltEmerCall();
        void setNumber(char* number);
        char* getNumber();
        void setCallCategory(uint32_t categ);
        uint32_t getCallCategory();
        void setNonStd(bool status);
        bool getNonStd();
        void resetNonStd();
        string dump();
};


#endif // RIL_PBM_PROCESS_DIALCALL_REQ
