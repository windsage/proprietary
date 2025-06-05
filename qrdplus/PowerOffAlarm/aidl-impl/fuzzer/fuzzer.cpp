/*==============================================================================
*  Copyright (c) 2024 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
#include <fuzzbinder/libbinder_ndk_driver.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "Alarm.h"

using aidl::vendor::qti::hardware::alarm::Alarm;

std::shared_ptr<Alarm> service;

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv) {
    service = ndk::SharedRefBase::make<Alarm>();
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    android::fuzzService(service->asBinder().get(), FuzzedDataProvider(data, size));

    return 0;
}