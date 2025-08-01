/******************************************************************************
  @file    service.cpp
  @brief   Android performance HAL service

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "vendor.qti.hardware.perf2-service"
#include <cutils/properties.h>

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

// add for cloud control by chao.xu5 at Jul 31th, 2025 start.
#include "LibPerfCloud.h"
// add for cloud control by chao.xu5 at Jul 31th, 2025 end.
#include "Perf.h"
#include "PerfLog.h"

//TODO: Check if reuired with AIDL based service
/*
#ifdef ENABLE_BINDER_BUFFER_TUNING_FOR_32_BIT
#include <hwbinder/ProcessState.h>
using android::hardware::ProcessState;
        #define PERF_HAL_BINDER_BUFFER_SIZE 384*1024
#endif
*/

using aidl::vendor::qti::hardware::perf2::Perf;
//TODO: check its usage
//using android::hardware::defaultPassthroughServiceImplementation;

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(0);
    std::shared_ptr<Perf> ser = ndk::SharedRefBase::make<Perf>();
    binder_status_t status = STATUS_OK;
    // add for cloud control by chao.xu5 at Jul 31th, 2025 start.
    RegistCloudctlListener();
    // add for cloud control by chao.xu5 at Jul 31th, 2025 end.

    const std::string instance = std::string() + Perf::descriptor + "/default";
    if (ser != NULL) {
        status = AServiceManager_addService(ser->asBinder().get(), instance.c_str());
    } else {
        QLOGE(LOG_TAG, "Couldn't get perf service object");
        return EXIT_FAILURE;
    }

    CHECK_EQ(status, STATUS_OK);

    ABinderProcess_startThreadPool();
    QLOGE(LOG_TAG, "Registered IPerf HAL service success!");
    ABinderProcess_joinThreadPool();

    // add for cloud control by chao.xu5 at Jul 31th, 2025 start.
    UnregistCloudctlListener();
    // add for cloud control by chao.xu5 at Jul 31th, 2025 end.
    return EXIT_FAILURE;
}
