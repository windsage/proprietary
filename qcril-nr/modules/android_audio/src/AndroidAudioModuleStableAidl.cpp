/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef QMI_RIL_UTF
#include <android/binder_manager.h>
#include <android/binder_process.h>
#endif

#include <cstring>
#include "AndroidAudioModuleStableAidl.h"
#include <framework/Log.h>
#include "stable_aidl_impl/qti_audio_stable_aidl_service.h"


static load_module<QcrilAudioStableAidlModule> sAndroidAudioModule;

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
QcrilAudioStableAidlModule::QcrilAudioStableAidlModule() {
    mName = "QcrilAudioSAidlModule";
    using std::placeholders::_1;
    mMessageHandler = {
        HANDLER(QcrilInitMessage, QcrilAudioStableAidlModule::handleQcrilInit),
        HANDLER(QcRilQueryAudioParamMessage, QcrilAudioStableAidlModule::handleQcrilQueryAudioParameters),
        HANDLER(QcRilSetAudioParamMessage, QcrilAudioStableAidlModule::handleQcrilSetAudioParameters)
    };
}


/* Follow RAII.
 */
QcrilAudioStableAidlModule::~QcrilAudioStableAidlModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void QcrilAudioStableAidlModule::init() {
    Module::init();
}

/*
 * Audio module initialization
 */
void QcrilAudioStableAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
    if (msg == nullptr) {
        Log::getInstance().d("[" + mName + "]: Handling invalid msg");
        return;
    }
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_instance_id_e_type instance_id = msg->get_instance_id();
    Log::getInstance().d("[" + mName + "]: get_instance_id = " + std::to_string(instance_id));
    if (mQcrilAudioSAidlService == nullptr) {
        mQcrilAudioSAidlService = ndk::SharedRefBase::make<aidlimplimports::QcrilAudioSAidlService>();
        std::string serviceInstance = mServiceName + "/slot" + std::to_string(instance_id + 1);
        binder_status_t status = AServiceManager_addService(mQcrilAudioSAidlService->asBinder().get(), serviceInstance.c_str());
        QCRIL_LOG_INFO("QcrilAudio Stable AIDL addService, status=%d", status);
        if (status != STATUS_OK) {
            mQcrilAudioSAidlService = nullptr;
            QCRIL_LOG_ERROR("Error registering service %s", serviceInstance.c_str());
        } else if (status == STATUS_OK) {
            mQcrilAudioSAidlService->setInstanceId(instance_id);
        }
    }
}

/*
 * Handling QueryAudioParameters
 */
void QcrilAudioStableAidlModule::handleQcrilQueryAudioParameters(std::shared_ptr<QcRilQueryAudioParamMessage> msg){
    if (msg == nullptr) {
        Log::getInstance().d("[" + mName + "]: QcrilQueryAudioParamMessage is null");
        return;
    }
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    if (mQcrilAudioSAidlService != nullptr) {
        mQcrilAudioSAidlService->onQueryParameters(msg);
    } else {
        QCRIL_LOG_INFO("QcrilAudio Stable AIDL service unavailable.");
        msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
    }
}

/*
 * Handling SetAudioParameters
 */
void QcrilAudioStableAidlModule::handleQcrilSetAudioParameters(std::shared_ptr<QcRilSetAudioParamMessage> msg){
    if (msg == nullptr) {
        Log::getInstance().d("[" + mName + "]: QcrilSetAudioParamMessage is Null");
        return;
    }

    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    if (mQcrilAudioSAidlService != nullptr) {
        msg->preProcessParams();
        mQcrilAudioSAidlService->onSetParameters(msg);
    } else {
        QCRIL_LOG_INFO("QcrilAudio Stable AIDL service unavailable.");
        msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
    }
}

