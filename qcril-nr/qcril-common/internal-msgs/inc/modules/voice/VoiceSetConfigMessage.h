/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef __VOICESETCONFIGMESSAGE_H_
#define __VOICESETCONFIGMESSAGE_H_

#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "voice_service_v02.h"

class VoiceSetConfigMessage : public SolicitedSyncMessage<voice_set_config_resp_msg_v02>,
                              public add_message_id<VoiceSetConfigMessage> {
public:
    static constexpr const char* MESSAGE_NAME = "VoiceSetConfigMessage";
    VoiceSetConfigMessage() = delete;
    ~VoiceSetConfigMessage() = default;
    inline VoiceSetConfigMessage(voice_set_config_req_msg_v02& req)
        : SolicitedSyncMessage<voice_set_config_resp_msg_v02>(get_class_message_id()), mRequest(req) {
    mName = MESSAGE_NAME;
    }

    voice_set_config_req_msg_v02& getRequest(){
        return mRequest;
    }

    std::string dump(){
        std::string os = mName;
        os +="{";
        os +="}";
        return os;
    }
private:
    voice_set_config_req_msg_v02 mRequest;
};

#endif  // __VOICESETCONFIGMESSAGE_H_
