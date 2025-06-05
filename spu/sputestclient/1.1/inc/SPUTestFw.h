/*!
 *
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#pragma once

#include <android/hidl/allocator/1.0/IAllocator.h>
#include <android/hidl/memory/1.0/IMemory.h>
#include <hidlmemory/mapping.h>
#include <vendor/qti/spu/1.1/ISPUManager.h>
#include <utils/StrongPointer.h>
#include <stdint.h>
#include <thread>
#include <mutex>

#include "log.h"
#include "SPUTestDefinitions.h"
#include "SpcomSSREvent.h"
#include "SpcomClientTest.h"
#include "SpcomServerTest.h"

using namespace std;
using namespace vendor::qti::spu;

using android::sp;
using vendor::qti::spu::V1_1::ISPUManager;
using ::android::hidl::memory::V1_0::IMemory;
using ::android::hidl::allocator::V1_0::IAllocator;
using ::android::hardware::hidl_memory;

class SPUTestFw {

public:

    static int32_t init();
    static void release();

    static sp<ISPUManager> getSPUManagerService();
    static sp<IAllocator> getIAllocatorService();

    static SpcomServerTest* getSpcomServerTest();
    static SpcomClientTest* getSpcomClientTest();

    /*
     * Read health status and external params of SPU
     *
     * @Return nonzero on failure, zero on success
     */
    static int32_t readSpuInfo();
    static int32_t readAppVersion(const char* appName, uint32_t appId);

    /*
     * Load SPU app from path and channel name which were delivered from
     * command line
     *
     * @Return nonzero on failure, zero on success
     */
    static int32_t loadApp(const char* appFilePath, const char* appName);

    static int32_t spuReset();

    static sp<SpcomSSREvent> getSpcomSSREvent();

    static int32_t addSSRCb(sp<ISpcomSSREvent> eventCb);
    static int32_t rmSSRCb(sp<ISpcomSSREvent> eventCb);
    static int32_t startSSRHandling();
    static void stopSSRHandling();
    static SSRHandlingState getSSRHandlingState();
    static int32_t getSSRThreadError();

    static int32_t waitForSpuReady();
private:

    // Singleton pattern
    explicit SPUTestFw();
    ~SPUTestFw();

    static void handleSSRThreadActivity();
    static void setSSRThreadState(SSRHandlingState state, int32_t err = SUCCESS);

    static sp<ISPUManager> sSpuManager;
    static sp<IAllocator> sAshmemAllocator;
    static sp<SpcomSSREvent> sSpcomSSREvent;
    static SpcomClientTest* sSpcomClientTest;
    static SpcomServerTest* sSpcomServerTest;
    static thread* sSSRHandleThread;
    static volatile SSRHandlingState sSSRState;
    static bool sRegisteredForSSRNotifications;
    static int32_t sSSRThreadError;
    static mutex sMtx;
};
