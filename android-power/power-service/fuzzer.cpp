/*==============================================================================
*  Copyright (c) 2023 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/

#include <fuzzbinder/libbinder_ndk_driver.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <fuzzbinder/libbinder_ndk_driver.h>
#include <android-base/logging.h>
#include <android/binder_interface_utils.h>
#include "PowerModule.h"

using android::fuzzService;
using ndk::SharedRefBase;
using aidl::vendor::qti::hardware::power::powermodule::PowerModule;

using ::ndk::ScopedAStatus;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {

   if( size < 12 ) return 0;
   auto binder = ::ndk::SharedRefBase::make<PowerModule>();
   fuzzService(binder->asBinder().get(), FuzzedDataProvider(data, size));
   return 0;
}
