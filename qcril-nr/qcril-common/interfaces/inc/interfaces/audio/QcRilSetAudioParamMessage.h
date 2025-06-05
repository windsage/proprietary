/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "interfaces/audio/audio_types.h"
#include "voice_service_v02.h"
#include <vector>


class QcRilSetAudioParamMessage : public SolicitedMessage<RIL_Errno>,
                              public add_message_id<QcRilSetAudioParamMessage> {

    private:
        std::vector<qcril::interfaces::QcRilSetAudioParams> mAudioParams;
    public:
        static constexpr const char* MESSAGE_NAME =
            "com.qualcomm.qti.qcril.audio.saidl_set_audio_param_request";
        QcRilSetAudioParamMessage() = delete;
        ~QcRilSetAudioParamMessage() = default;

        QcRilSetAudioParamMessage(std::vector<qcril::interfaces::QcRilSetAudioParams> params)
            : SolicitedMessage<RIL_Errno>(get_class_message_id()), mAudioParams(params) {
            mName = MESSAGE_NAME;
        }

        inline string dump() {
            string dump_message = mName;
            for (auto& i : mAudioParams) {
                dump_message += "{Vsid:" + std::to_string(i.vsid) +
                                " CallState:" + qcril::interfaces::toString(i.call_state) +
                                " CallMode:" + std::to_string(i.call_mode) +
                                " IsCrs:" + std::string(i.crs_call ? "true" : "false") +"},";
            }
            return dump_message;
        }

        /* Preprocess the audio params to have all the nonActive requests
         * before the first active request
         */
        void preProcessParams() {
            std::vector<qcril::interfaces::QcRilSetAudioParams> audioParams;
            for (auto& p : mAudioParams) {
                if (p.call_state != qcril::interfaces::AudioCallState::ACTIVE) {
                    audioParams.push_back(p);
                }
            }

            for (auto& p : mAudioParams) {
                if (p.call_state == qcril::interfaces::AudioCallState::ACTIVE) {
                    audioParams.push_back(p);
                    break;
                }
            }
            mAudioParams = audioParams;
        }

        // Checks if audio params is empty
        bool isParamEmpty(){
            return mAudioParams.empty();
        }

        // Must be called only if isParamEmpty returns false
        qcril::interfaces::QcRilSetAudioParams& getFirstParam() {
            return mAudioParams.front();
        }

        // Remove top param after success response
        void removeFirstParam(){
            mAudioParams.erase(mAudioParams.begin());
        }
};
