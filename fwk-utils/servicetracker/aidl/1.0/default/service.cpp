/******************************************************************************
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <log/log.h>

#include "Servicetracker.h"
using ::aidl::vendor::qti::hardware::servicetrackeraidl::implementation::Servicetracker;


int main() {
    ALOGE("Servicetracker AIDL starting up");
    if (!ABinderProcess_setThreadPoolMaxThreadCount(16)) {
        LOG(INFO) << "failed to set thread pool max thread count";
        return 1;
    }
    std::shared_ptr<Servicetracker> st_service = ndk::SharedRefBase::make<Servicetracker>();
    const std::string instance = std::string() + Servicetracker::descriptor + "/default";
    binder_status_t status =
    AServiceManager_addService(st_service->asBinder().get(), instance.c_str());
    CHECK(status == STATUS_OK);
    ABinderProcess_joinThreadPool();
    return 0;
}
