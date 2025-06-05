/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef RIL_SET_ALT_EMER_CALL_INFO
#define RIL_SET_ALT_EMER_CALL_INFO

#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/**
 * Message to set alternate emergency call number and category.
 */

class RilSetAlternateEmergencyCallInfo : public SolicitedSyncMessage<bool>,public add_message_id<RilSetAlternateEmergencyCallInfo> {
    private:
        char* number{nullptr};
        uint32_t emerCateg{0};
    public:
        static constexpr const char *MESSAGE_NAME = "QCRIL_SET_ALT_EMER_CALL_INFO";

        inline RilSetAlternateEmergencyCallInfo():SolicitedSyncMessage<bool>(get_class_message_id())
        {
            mName = MESSAGE_NAME;
        }

        ~RilSetAlternateEmergencyCallInfo();

        char* getNumber();
        uint32_t getEmerCateg();
        void setNumber(char* num);
        void setEmerCateg(uint32_t emerCat);
        string dump();
};


#endif // RIL_SET_ALT_EMER_CALL_INFO
