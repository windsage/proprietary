/******************************************************************************
  @file    service.cpp
  @brief   Power service

  DESCRIPTION

  ===========================================================================
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================
******************************************************************************/
#define LOG_TAG "poweropt-service"

#include "PowerCallback.h"
#include "PowerModule.h"
#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <dlfcn.h>

using aidl::vendor::qti::hardware::power::powermodule::PowerModule;
using aidl::vendor::qti::hardware::perf2::PowerCallback;
using IPerfAidl = ::aidl::vendor::qti::hardware::perf2::IPerf;
using ::ndk::SpAIBinder;
using namespace android;
using IPerfCallbackAidl = ::aidl::vendor::qti::hardware::perf2::IPerfCallback;

void* perf_handle = nullptr;
int (*perflock_callbackRegister)(const std::shared_ptr<IPerfCallbackAidl>& , int32_t) = nullptr;

static int perfCallbackRegister(const std::shared_ptr<IPerfCallbackAidl>& callback, int32_t clientId){
    int rc = 0;
    if (callback != nullptr) {
        perf_handle = dlopen("libqti-perfd-client.so", RTLD_NOW);
        if(!perf_handle){
            ALOGE("Unable to open perf  lib: %s", dlerror());
            return rc;
        }
        perflock_callbackRegister = (int (*)(const std::shared_ptr<IPerfCallbackAidl>& , int32_t))dlsym(perf_handle, "_Z22perf_callback_registerRKNSt3__110shared_ptrIN4aidl6vendor3qti8hardware5perf213IPerfCallbackEEEi");
        if(!perflock_callbackRegister){
            ALOGE("Unable to  get perflock_callbackRegister handle:%s", dlerror());
        }else{
            rc = perflock_callbackRegister(callback, clientId);
        }
    }

    return rc;
}

int main() {
    static int32_t clientId = 1;
    ALOGE("PowerService is starting");
    ABinderProcess_setThreadPoolMaxThreadCount(1);
    std::shared_ptr<PowerModule> mPowerModule = ndk::SharedRefBase::make<PowerModule>();
    const std::string instance = std::string() + PowerModule::descriptor + "/default";

    if(mPowerModule != nullptr && mPowerModule->asBinder() != nullptr){
        if(mPowerModule->asBinder().get()) {
            ALOGE("Starting Powermodule HAL");
            binder_status_t status = AServiceManager_addService(mPowerModule->asBinder().get(), instance.c_str());
            CHECK_EQ(status, STATUS_OK);

            std::shared_ptr<PowerCallback> mPowerCallback = ndk::SharedRefBase::make<PowerCallback>();
            if (mPowerCallback != nullptr) {
                /*Client Impl*/
                const std::string instance1 = std::string() + IPerfAidl::descriptor + "/default";
                auto perfBinder = ::ndk::SpAIBinder(AServiceManager_waitForService(instance1.c_str()));
                if (perfBinder.get() != nullptr) {
                    int rc = perfCallbackRegister(mPowerCallback, clientId);
                    if (!rc){
                        ALOGE("PerfCallback register failed.");
                    }
                }
            }else {
                ALOGE("PowerCallback is nullptr.");
            }
        }else{
            ALOGE("asBinder failed");
        }
    }

    ABinderProcess_startThreadPool();
    ABinderProcess_joinThreadPool();
    return 0;
}
