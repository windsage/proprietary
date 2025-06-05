/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _ANDROID_AUDIO_STABLE_AIDL_SERVICE_H_
#define _ANDROID_AUDIO_STABLE_AIDL_SERVICE_H_

#ifndef QMI_RIL_UTF
#include "aidl/vendor/qti/hardware/radio/am/BnQcRilAudio.h"
#include "aidl/vendor/qti/hardware/radio/am/BnQcRilAudioResponse.h"
#include "aidl/vendor/qti/hardware/radio/am/AudioError.h"
#include "aidl/vendor/qti/hardware/radio/am/IQcRilAudioResponse.h"
#include "aidl/vendor/qti/hardware/radio/am/IQcRilAudioRequest.h"

#endif //QMI_RIL_UTF

#include <vector>
#include "interfaces/audio/QcRilQueryAudioParamMessage.h"
#include "interfaces/audio/QcRilSetAudioParamMessage.h"
#include "QcRilAudioContext.h"
#include "framework/PendingMessageList.h"

#undef TAG
#define TAG "RILQ"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace am {
namespace implementation {

using AmCallState = qcril::interfaces::AudioCallState;

class QcrilAudioSAidlService : public BnQcRilAudio {

    class QcrilAudioResponseCallback : public BnQcRilAudioResponse {
        public:
            QcrilAudioResponseCallback() = delete;
            QcrilAudioResponseCallback(QcrilAudioSAidlService& service):
                mService(service){}
            virtual ~QcrilAudioResponseCallback(){}
            ndk::ScopedAStatus queryParametersResponse(const int32_t token,
                const std::string& params) override;
            ndk::ScopedAStatus setParametersResponse(const int32_t token,
                AudioError errorCode) override;
        private:
            AmCallState convertAudioNumToCallState(const uint32_t call_state);
            QcrilAudioSAidlService& mService;
    };


    private:
        qcril_instance_id_e_type mInstanceId;
        qtimutex::QtiSharedMutex mCallbackLock;
        AIBinder_DeathRecipient* mDeathRecipient{nullptr};
        PendingMessageList msgQueue;
        std::shared_ptr<IQcRilAudioRequest> mRequestCb;
        std::shared_ptr<IQcRilAudioResponse> mResponseCb;
        uint32_t convertCallStateToAudioNum(const AmCallState& call_state);
        std::string convertRilAudioParamtersToString(
            const qcril::interfaces::QcRilSetAudioParams& params);

    protected:
        std::shared_ptr<IQcRilAudioRequest> getRequestCallback();
        void clearCallbacks();
        void clearCallbacks_nolock();

    public:
        QcrilAudioSAidlService(): msgQueue("Audio Msg Queue"),
            mResponseCb(ndk::SharedRefBase::make<QcrilAudioResponseCallback>(*this)){}
        virtual ~QcrilAudioSAidlService(){}
        void deathNotifier(void* cookie);
        std::shared_ptr<QcRilAudioContext> getContext(uint32_t serial);
        void setInstanceId(qcril_instance_id_e_type instanceId);
        qcril_instance_id_e_type getInstanceId();

        void setRequestInterface_nolock(
        const std::shared_ptr<IQcRilAudioRequest>& in_requestCallback);
        void onQueryParameters(std::shared_ptr<QcRilQueryAudioParamMessage> msg);
        void onSetParameters(std::shared_ptr<QcRilSetAudioParamMessage> msg);


        //aidl request functions
        ndk::ScopedAStatus setRequestInterface(
            const std::shared_ptr<IQcRilAudioRequest>& in_requestCallback,
                std::shared_ptr<IQcRilAudioResponse>* _aidl_return) override;
        ndk::ScopedAStatus setError(AudioError errorCode) override;

};

}
}
}
}
}
}
}

#endif //_ANDROID_AUDIO_STABLE_AIDL_SERVICE_H_
