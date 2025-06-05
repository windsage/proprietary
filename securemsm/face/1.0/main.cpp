/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "Face.h"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

using aidl::android::hardware::biometrics::face::Face;

int main()
{
    LOG(INFO) << "Face HAL started";
    ABinderProcess_setThreadPoolMaxThreadCount(0);
    std::shared_ptr<Face> hal = ndk::SharedRefBase::make<Face>();
    if (!hal) {
        return EXIT_FAILURE;
    }

    const std::string instance = std::string(Face::descriptor) + "/default";
    binder_status_t status =
        AServiceManager_addService(hal->asBinder().get(), instance.c_str());
    CHECK(status == STATUS_OK);

    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;  // should not reach
}
