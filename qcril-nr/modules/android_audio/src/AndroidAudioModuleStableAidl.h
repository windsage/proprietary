/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_AUDIO_STABLE_AIDL_H_
#define _QTI_AUDIO_STABLE_AIDL_H_

#include "framework/Module.h"
#include "framework/QcrilInitMessage.h"
#include "interfaces/audio/QcRilQueryAudioParamMessage.h"
#include "interfaces/audio/QcRilSetAudioParamMessage.h"
#include "stable_aidl_impl/qti_audio_stable_aidl_service.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>

namespace aidlimplimports {
    using namespace aidl::vendor::qti::hardware::radio::am::implementation;
}

class QcrilAudioStableAidlModule : public Module {
    public:
        QcrilAudioStableAidlModule();
        ~QcrilAudioStableAidlModule();
        void init();

    private:
        std::string mServiceName = "vendor.qti.hardware.radio.am.IQcRilAudio";
        void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
        void handleQcrilQueryAudioParameters(std::shared_ptr<QcRilQueryAudioParamMessage> msg);
        void handleQcrilSetAudioParameters(std::shared_ptr<QcRilSetAudioParamMessage> msg);
        std::shared_ptr<aidlimplimports::QcrilAudioSAidlService> mQcrilAudioSAidlService = nullptr;
};

#endif // _QTI_AUDIO_STABLE_AIDL_H_
