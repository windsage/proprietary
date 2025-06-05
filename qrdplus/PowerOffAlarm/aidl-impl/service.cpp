/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include "Alarm.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.hardware.alarm-service"


using aidl::vendor::qti::hardware::alarm::Alarm;


int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(0);
    ABinderProcess_startThreadPool();
    std::shared_ptr<Alarm> alarm = ndk::SharedRefBase::make<Alarm>();
    const std::string name_alarm = std::string() + Alarm::descriptor + "/default";

    binder_status_t status = AServiceManager_addService(alarm->asBinder().get(), name_alarm.c_str());
    CHECK_EQ(status, STATUS_OK);

    ALOGI("IAlarm service starts to join service pool");
    ABinderProcess_joinThreadPool();

    return EXIT_FAILURE;  // should not reached
}
