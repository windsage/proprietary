/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "framework/legacy.h"

class RequestRadioStateMessage : public SolicitedSyncMessage<RIL_RadioState>,
                              public add_message_id<RequestRadioStateMessage> {
public:
    static constexpr const char* MESSAGE_NAME = "RequestRadioStateMessage";
    RequestRadioStateMessage() = delete;
    ~RequestRadioStateMessage() = default;
    inline RequestRadioStateMessage(qcril_instance_id_e_type rilInstanceId)
        : SolicitedSyncMessage<RIL_RadioState>(get_class_message_id()),
        rilInstanceId(rilInstanceId)
    {
        mName = MESSAGE_NAME;
    }

    qcril_instance_id_e_type forInstance() {
        return rilInstanceId;
    }

    std::string dump(){
        std::string os = mName;
        os +="{";
        os +="}";
        return os;
    }

private:
    qcril_instance_id_e_type rilInstanceId;
};
