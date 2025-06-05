/******************************************************************************
  @file    PowerCallback.cpp
  @brief   power call back HAL module

  DESCRIPTION

  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define LOG_TAG "PowerCallback"
#include <dlfcn.h>
#include <pthread.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include "PowerCallback.h"
#include "PowerCore.h"
#include <cutils/properties.h>
#include "PowerOptAsyncData.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace perf2 {


PowerCallback::PowerCallback() {

}

PowerCallback::~PowerCallback() {
}

ScopedAStatus PowerCallback::notifyCallback(int32_t hint, const std::string& userDataStr, int32_t userData1, int32_t userData2, const std::vector<int32_t>& reserved)  {
    if(mPowerCore != nullptr){
        mPowerCore->sendEvent(hint, userDataStr, userData1, userData2, reserved);
    }
    return ndk::ScopedAStatus::ok();
}



}  // namespace perf2
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
