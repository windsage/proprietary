/*!
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <aidl/vendor/qti/hardware/spu/BnSPUManager.h>
#include <aidl/android/hardware/common/Ashmem.h>
#include <cutils/ashmem.h>
#include <sys/mman.h>
#include <stdint.h>
#include <thread>
#include <mutex>

#include "log.h"
#include "SPComClientTest.h"
#include "SPComServerTest.h"
#include "SPUTestDefinitions.h"
#include "SPUNotifier.h"

using namespace std;
using namespace aidl::vendor::qti::hardware::spu;
using aidl::android::hardware::common::Ashmem;
using aidl::vendor::qti::hardware::spu::ISPUManager;
using aidl::vendor::qti::hardware::spu::ISPComSharedBuffer;

class SPUTestFw {

public:
    static int32_t init();
    static void release();

    // Getters
    static inline std::shared_ptr<ISPUManager> getSPUManagerService() { return spuManager; }
    static inline SPComClientTest *getSPComClientTest() { return spcomClientTest; }
    static inline SPComServerTest *getSPComServerTest() { return spcomServerTest; }

    // Read info
    static int32_t readSpuInfo();
    static int32_t sysParamReadWrapper(uint32_t id, uint32_t arg1, uint32_t arg2);

    // App loading
    static int32_t readAppVersion(const char* appName, uint32_t appId);
    static int32_t loadApp(const char* appFilePath, const char* appName, const uint32_t uuid);

    // Wait for system readiness
    static int32_t waitForSpuReady();

    // SSR
    static int32_t spuReset();
    static inline std::shared_ptr<SPUNotifier> getNotifier() { return notifier; }
    static int32_t setNotifier(std::shared_ptr<ISPUNotifier> eventCb);
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

    static std::shared_ptr<ISPUManager> spuManager;
    static std::shared_ptr<SPUNotifier> notifier;
    static SPComClientTest *spcomClientTest;
    static SPComServerTest *spcomServerTest;
    static thread *ssrHandleThread;
    static volatile SSRHandlingState ssrState;
    static bool registeredForSSRNotifications;
    static int32_t ssrThreadError;
    static mutex mtx;
};
