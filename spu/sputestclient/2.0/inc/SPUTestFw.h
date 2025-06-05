/*!
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <android/hidl/allocator/1.0/IAllocator.h>
#include <android/hidl/memory/1.0/IMemory.h>
#include <hidlmemory/mapping.h>
#include <vendor/qti/spu/2.0/ISPUManager.h>
#include <utils/StrongPointer.h>
#include <stdint.h>
#include <thread>
#include <mutex>

#include "log.h"
#include "SPComClientTest.h"
#include "SPComServerTest.h"
#include "SPUTestDefinitions.h"
#include "SPUNotifier.h"

using namespace std;
using namespace vendor::qti::spu;

using android::sp;
using vendor::qti::spu::V2_0::ISPUManager;
using ::android::hidl::memory::V1_0::IMemory;
using ::android::hidl::allocator::V1_0::IAllocator;
using ::android::hardware::hidl_memory;

class SPUTestFw {

public:
    static int32_t init();
    static void release();

    // Getters
    static inline sp<ISPUManager> getSPUManagerService() { return spuManager; }
    static inline sp<IAllocator> getIAllocatorService() { return ashmemAllocator; }
    static inline SPComClientTest *getSPComClientTest() { return spcomClientTest; }
    static inline SPComServerTest *getSPComServerTest() { return spcomServerTest; }

    // Read info
    static int32_t readSpuInfo();
    static int32_t sysParamReadWrapper(uint32_t id, uint32_t arg1, uint32_t arg2);

    // App loading
    static int32_t readAppVersion(const char* appName, uint32_t appId);
    static int32_t loadApp(const char* appFilePath, const char* appName);

    // Wait for system readiness
    static int32_t waitForSpuReady();

    // SSR
    static int32_t spuReset();
    static inline sp<SPUNotifier> getNotifier() { return notifier; }
    static int32_t setNotifier(sp<ISPUNotifier> eventCb);
    static int32_t clearNotifier();
    static int32_t startSSRHandling();
    static void stopSSRHandling();
    static inline SSRHandlingState getSSRHandlingState() { return ssrState; }
    static inline int32_t getSSRThreadError() { return ssrThreadError; }

private:
    // Singleton pattern
    explicit SPUTestFw();
    ~SPUTestFw();

    static void handleSSRThreadActivity();
    static void setSSRThreadState(SSRHandlingState state, int32_t err = SUCCESS);
    static int32_t sysParamReadWrapper(uint32_t id, uint32_t arg1, uint32_t arg2, uint32_t *val);
    static int32_t sysDataReadWrapper(uint32_t id, uint32_t arg1, uint32_t arg2, uint8_t *val, uint32_t size);

    static sp<ISPUManager> spuManager;
    static sp<IAllocator> ashmemAllocator;
    static sp<SPUNotifier> notifier;
    static SPComClientTest *spcomClientTest;
    static SPComServerTest *spcomServerTest;
    static thread *ssrHandleThread;
    static volatile SSRHandlingState ssrState;
    static bool registeredForSSRNotifications;
    static int32_t ssrThreadError;
    static mutex mtx;
};
