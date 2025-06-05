/*
 ** Copyright (c) 2024 Qualcomm Technologies, Inc.
 ** All Rights Reserved.
 ** Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/
#include <fuzzbinder/libbinder_ndk_driver.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <android/binder_interface_utils.h>
#include "qtiwifi.h"

using aidl::vendor::qti::hardware::wifi::qtiwifi::QtiWifi;
std::shared_ptr<QtiWifi> service;

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv) {
    qtiwifi_nl80211_init();
    service = ndk::SharedRefBase::make<QtiWifi>(global);
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (service == nullptr) {
        return -1;
    }

    android::fuzzService(service->asBinder().get(), FuzzedDataProvider(data, size));
    return 0;
}
