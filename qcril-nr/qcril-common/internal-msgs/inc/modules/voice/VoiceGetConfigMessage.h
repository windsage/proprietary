/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef __VOICEGETCONFIGMESSAGE_H_
#define __VOICEGETCONFIGMESSAGE_H_

#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "voice_service_v02.h"

class VoiceGetConfigMessage : public SolicitedSyncMessage<voice_get_config_resp_msg_v02>,
                              public add_message_id<VoiceGetConfigMessage> {
public:
    static constexpr const char* MESSAGE_NAME = "VoiceGetConfigMessage";
    VoiceGetConfigMessage() = delete;
    ~VoiceGetConfigMessage() = default;
    inline VoiceGetConfigMessage(voice_get_config_req_msg_v02& req)
        : SolicitedSyncMessage<voice_get_config_resp_msg_v02>(get_class_message_id()), mRequest(req) {
    mName = MESSAGE_NAME;
    }

    voice_get_config_req_msg_v02& getRequest() {
        return mRequest;
    }

    std::string dump(){
        std::string os = mName;
        os +="{";
        os +="}";
        return os;
    }
private:
    voice_get_config_req_msg_v02 mRequest;
};

#endif  // __VOICEGETCONFIGMESSAGE_H_
