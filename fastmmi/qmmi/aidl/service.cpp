/*
 * Copyright (c) 2017, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <android-base/logging.h>
#include <log/log.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include "Factory.h"

using ::aidl::vendor::qti::hardware::factory::Factory;

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(0);

    std::shared_ptr<Factory> ser = ndk::SharedRefBase::make<Factory>();
    ALOGI("start to register Factory HAL service");
    const std::string instance = std::string() + Factory::descriptor + "/default";
    binder_status_t status = AServiceManager_addService(ser->asBinder().get(), instance.c_str());

    CHECK_EQ(status, STATUS_OK);

    ABinderProcess_joinThreadPool();
    ALOGE("Can't register Factory HAL service");
    return EXIT_FAILURE;// should never get here
}
