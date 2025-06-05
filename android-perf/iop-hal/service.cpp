/******************************************************************************
  @file    service.cpp
  @brief   Android IOP HAL service

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "vendor.qti.hardware.iop@2.0-service"

#include <vendor/qti/hardware/iop/2.0/IIop.h>
#include <hidl/LegacySupport.h>
#include "Iop.h"

#include "PerfLog.h"
#include <client.h>

using vendor::qti::hardware::iop::V2_0::implementation::Iop;
using vendor::qti::hardware::iop::V2_0::IIop;
using android::hardware::defaultPassthroughServiceImplementation;

using android::sp;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::status_t;
using android::OK;

int main() {
    QLOGE(LOG_TAG, "IIop HAL service main");

    Iop *iopObj = new(std::nothrow) Iop();

    if (iopObj != NULL) {
        android::sp<IIop> service = iopObj;

        configureRpcThreadpool(1, true /*callerWillJoin*/);

        if (service->registerAsService() != OK) {
            QLOGE(LOG_TAG, "Cannot register IIop HAL service");
            return 1;
        }

        QLOGE(LOG_TAG, "Registered IIop HAL service success!");
        joinRpcThreadpool();
    }
    return 0;
}
