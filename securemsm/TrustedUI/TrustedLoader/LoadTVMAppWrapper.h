/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef LOADTVMAPPWRAPPER_H
#define LOADTVMAPPWRAPPER_H

#include <android/hidl/memory/1.0/IMemory.h>
#include <string>
#include "ITrustedUIApp.hpp"

using ::android::hardware::hidl_memory;

int32_t _loadTUIApp(TrustedUIApp **appObj, const uint32_t uid, std::string appName, const hidl_memory& appBin, bool isTUIAppinOEMVM);

int32_t _unloadTUIApp(TrustedUIApp *appObj);

#endif
