/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "interfaces/audio/audio_types.h"

/*===========================================================================*/
/*!
    @brief
    Provides the Audio Parameters set with AM Service.

    mVsid - Vsid for which Audio Parameters are requested

    Returns Audio Paramters
*/
/*==========================================================================*/


class QcRilQueryAudioParamMessage : public SolicitedMessage<qcril::interfaces::QcRilGetAudioParams>, public add_message_id<QcRilQueryAudioParamMessage> {

private:
    string mVsid;
    string mQueryString;

public:
    static constexpr const char* MESSAGE_NAME =
        "com.qualcomm.qti.qcril.audio.query_audio_param_request";
    QcRilQueryAudioParamMessage() = delete;
    ~QcRilQueryAudioParamMessage() = default;

    QcRilQueryAudioParamMessage(uint32_t vsid, string queryString)
        : SolicitedMessage<qcril::interfaces::QcRilGetAudioParams>(get_class_message_id()) {
        mVsid = std::to_string(vsid);
        mQueryString = queryString;
        mName = MESSAGE_NAME;
    }

    inline string dump() {
        return mName + "{ Call Vsid :" + mVsid + " Query String : " + mQueryString + "}";
    }

    string getVsid() {
        return mVsid;
    }

    string getQueryString() {
        return mQueryString;
    }
};

