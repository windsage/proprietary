/******************************************************************************
#  Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/android/ril_message_factory.h"
#include "qtiril-loadable/QtiRilLoadable.h"
#include "VoiceModule.h"
#include "ims/ImsVoiceModule.h"

#if 0
#if 0
static load_module<VoiceModule> voiceModule;
#else
static load_module<ImsVoiceModule> voiceModule;
#endif

std::shared_ptr<VoiceModule> getVoiceModule(){
    return (voiceModule.get_module());
}
#endif

#ifndef QMI_RIL_UTF
static struct LoadableHeader *theHeader = nullptr;
static QtiSharedMutex theHeaderMutex;
extern "C"
struct LoadableHeader *loadable_init() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader == nullptr) {
        auto theModule = std::make_shared<ImsVoiceModule>();
        if (theModule) {
            theModule->init();
        }
        auto header = new LoadableHeader{"VoiceModule", std::static_pointer_cast<Module>(theModule)};
        theHeader = header;
    }
    return theHeader;
}

extern "C"
int loadable_deinit() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader != nullptr) {
        delete theHeader;
    }
    return 0;
}

std::shared_ptr<VoiceModule> getVoiceModule() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader != nullptr) {
        return std::static_pointer_cast<VoiceModule>(theHeader->theModule);
    } else {
        return nullptr;
    }
}
#else
static load_sharedptr_module<ImsVoiceModule> sVoiceModule;
std::shared_ptr<VoiceModule> getVoiceModule() {
    return (sVoiceModule.get_module());
}

#endif
