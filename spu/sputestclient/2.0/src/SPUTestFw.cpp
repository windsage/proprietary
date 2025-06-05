/*!
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <chrono>
#include <fstream>
#include <vendor/qti/spu/2.0/ISPUNotifier.h>
#include <spu_definitions.h>
#include "log.h"
#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"

using namespace std;
using namespace chrono;
using vendor::qti::spu::V2_0::ISPUNotifier;
using ::android::hardware::hidl_memory;

sp<ISPUManager> SPUTestFw::spuManager(nullptr);
sp<IAllocator> SPUTestFw::ashmemAllocator(nullptr);
sp<SPUNotifier> SPUTestFw::notifier(nullptr);
SPComClientTest *SPUTestFw::spcomClientTest(nullptr);
SPComServerTest *SPUTestFw::spcomServerTest(nullptr);
thread *SPUTestFw::ssrHandleThread(nullptr);
volatile SSRHandlingState SPUTestFw::ssrState(SSR_STATE_OFF);
bool SPUTestFw::registeredForSSRNotifications(false);
int32_t SPUTestFw::ssrThreadError(SUCCESS);
mutex SPUTestFw::mtx;

void SPUTestFw::setSSRThreadState(SSRHandlingState state, int32_t err)
{
    unique_lock<mutex> lk(mtx);
    ssrThreadError = err;
    ssrState = state;
}

void SPUTestFw::handleSSRThreadActivity()
{
    if(!spuManager || !notifier) {
        ALOGE("Framework isn't initialized, can't handle SSR events");
        setSSRThreadState(SSR_STATE_ERROR, FRAMEWORK_ERROR);
    }

    while(ssrState != SSR_STATE_SHUTDOWN && ssrState != SSR_STATE_ERROR) {
        setSSRThreadState(SSR_STATE_READY);
        notifier->waitForSSREvent();

        if(getSSRHandlingState() == SSR_STATE_SHUTDOWN) {
            ALOGD("SSR thread shut down detected");
            break;
        }

        setSSRThreadState(SSR_STATE_HANDLE);

        int32_t ret = spcomClientTest->handleSSR();
        if (ret) {
            ALOGE("Failed to handle SSR in SPCOM client");
            setSSRThreadState(SSR_STATE_ERROR, CLIENT_TEST_ERROR);
        }

        ret = spcomServerTest->handleSSR();
        if (ret) {
            ALOGE("Failed to handle SSR in SPCOM server");
            setSSRThreadState(SSR_STATE_ERROR, SERVER_TEST_ERROR);
        }

        notifier->notifySSRHandled();
    }

    while (ssrState != SSR_STATE_SHUTDOWN) {
        ALOGD("Waiting for thread shutdown");
        this_thread::sleep_for(chrono::seconds(2 /*seconds*/));
    }

    ALOGD("SSR handle thread shut-down in 2 seconds");
    this_thread::sleep_for(seconds(2 /*seconds*/));
    setSSRThreadState(SSR_STATE_OFF, SUCCESS);
}

int32_t SPUTestFw::init()
{
    unique_lock<mutex> lk(mtx);

    // Init SPU HAL service
    if (!spuManager) {
        ALOGD("get SpuManager service handle");
        spuManager = ISPUManager::tryGetService();
        if (!spuManager) {
            ALOGE("Failed to get service ISPUManager");
            return FRAMEWORK_ERROR;
        }
    }

    // Init HIDL shared memory allocator service
    if (!ashmemAllocator) {
        ALOGD("get AshmemAllocator server handle");
        ashmemAllocator = IAllocator::tryGetService("ashmem");
        if(!ashmemAllocator) {
            ALOGE("Failed to get shared memory allocator service");
            return FRAMEWORK_ERROR;
        }
    }

    // Init the spcomClientTest
    if (!spcomClientTest) {
        ALOGD("Create SpcomClientTest obj");
        spcomClientTest = new SPComClientTest(spuSrvAppName);
        if(!spcomClientTest) {
            ALOGE("Failed to create SpcomClientTest obj");
            return FRAMEWORK_ERROR;
        }
    }

    // Init the spcomServerTest
    if (!spcomServerTest) {
        ALOGD("Create SpcomServerTest obj");
        spcomServerTest = new SPComServerTest(spuSrvHLOSChannelName);
        if(!spcomServerTest) {
            ALOGE("Failed to create SpcomServerTest obj");
            return FRAMEWORK_ERROR;
        }
    }

    // Init SPU SSR event object
    if (!notifier) {
        ALOGD("Create SpcomSSREvent obj");
        notifier = new SPUNotifier();
        if (!notifier) {
            ALOGE("Notifier obj is not valid");
            return FRAMEWORK_ERROR;
        }
    }

    // Wait for SPU to be ready
    if (spuManager->waitForSpuReady(waitForSpuReadyTimeoutSec)) {
        ALOGE("SPU not ready");
        return RESOURCE_BUSY;
    }

    return SUCCESS;
}

void SPUTestFw::release()
{
    if (spcomClientTest) {
        ALOGD("Release SpcomClientTest obj");
        delete spcomClientTest;
        spcomClientTest = nullptr;
    }

    if (spcomServerTest) {
        ALOGD("Release SpcomServerTest obj");
        delete spcomServerTest;
        spcomServerTest = nullptr;
    }

    if (spuManager) {
        ALOGD("Release SPU HIDL server handle");
        spuManager.clear();
    }

    if (ashmemAllocator) {
        ALOGD("Release Ashmem allocator handle");
        ashmemAllocator.clear();
    }
}

/**
 * Load SPU application:
 *   > Allocate HIDL shared memory
 *   > Copy The application content to the shared memory
 *   > Call loadApp and share HIDL shared memory with server
 *   > Check app is loaded
 */
int32_t SPUTestFw::loadApp(const char* appFilePath, const char* appName)
{
    if (!spuManager || !ashmemAllocator)
        return FRAMEWORK_ERROR;

    if (!appFilePath || !appName) {
        ALOGE("Load app invalid params");
        return INVALID_PARAM;
    }

    if (spuManager->isAppLoaded(appName)) {
        ALOGD("App is already loaded [%s]", appName);
        return ALREADY_LOADED;
    }

    ALOGD("Going to load app [%s], channel [%s]", appFilePath, appName);

    // Open app image for read
    ifstream appBinFile(appFilePath, ifstream::in);
    if (!appBinFile) {
        ALOGE("Couldn't open image file");
        return GENERAL_ERROR;
    }

    // Read app size
    appBinFile.seekg(0, appBinFile.end);
    int fileLen = appBinFile.tellg();
    appBinFile.seekg(0, appBinFile.beg);

    if (fileLen <= 0) {
        ALOGE("Failed to read file [%s], size[%d]", appFilePath, fileLen);
        return GENERAL_ERROR;
    }

    ashmemAllocator->allocate(fileLen, [&](bool success, const hidl_memory &mem) {

         if (!success) {
            ALOGE("Failed to allocate shared memory");
            return GENERAL_ERROR;
        }

        sp<IMemory> memory;

        // Map HIDL shared memory to client
        memory = mapMemory(mem);
        if (!memory) {
            ALOGE("Failed to fetch IMemory obj to handle the hidl_mem");
            return GENERAL_ERROR;
        }

        // Copy SPU application content to the mapped shared memory
        memory->update();
        void *data = memory->getPointer();

        for (long int i = 0; i < fileLen; ++i) {
            *(((uint8_t*) data) + i) = appBinFile.get();
        }
        memory->commit();

        // Load application share with HIDL server over HIDL shared memory
        int32_t ret = spuManager->loadApp(appName, mem, fileLen);

        if (ret != 0) {
            ALOGE("Failed to load app[%s], res [%d]", appName, ret);
            return LOAD_APP_ERROR;
        }

        return SUCCESS;
    });

    // Check if app was loaded successfully
    if (!spuManager->isAppLoaded(appName)) {
        ALOGE("Failed to load app");
        return LOAD_APP_ERROR;
    }

    ALOGD("App [%s] was loaded successfully, channel [%s]", appFilePath,
        appName);

    return SUCCESS;
}

int32_t SPUTestFw::sysParamReadWrapper(uint32_t id, uint32_t arg1, uint32_t arg2, uint32_t *val)
{
    int32_t ret = 0;

    spuManager->sysParamRead(id, arg1, arg2,
        [&](int32_t err, uint32_t retval) {
            if (err) {
                ret = GENERAL_ERROR;
                return;
            }

            *val = retval;
        });

    return ret;
}

int32_t SPUTestFw::sysDataReadWrapper(uint32_t id, uint32_t arg1, uint32_t arg2, uint8_t *val, uint32_t size)
{
    int32_t ret = 0;

    spuManager->sysDataRead(id, arg1, arg2,
        [&](int32_t err, hidl_vec<int32_t> data) {
            if (err) {
                ret = GENERAL_ERROR;
            }
            else if(size > data.size() << 2) {
                ret = INVALID_PARAM;
            }
            else {
                memcpy(val, data.data(), size);
                ret = SUCCESS;
            }
        }
    );

    return ret;
}

// Test SPU health status API and prints SPU applications version
int32_t SPUTestFw::readSpuInfo()
{
    if (!spuManager)
        return FRAMEWORK_ERROR;

    uint32_t errCount = 0;
    uint32_t maxSpcomMessageSize = spuManager->getSPComMaxMessageSize();
    uint32_t maxChannelNameLength = spuManager->getSPComMaxChannelNameLength();
    uint32_t imageType = spuManager->getImageType();

    // Print health status
    spuManager->checkHealth([&](int32_t error, const hidl_vec<int32_t>& healthStatus) {
        if (error) {
            ++errCount;
            return;
        }

        sp_health_status_data *healthData = (sp_health_status_data *)healthStatus.data();

        ALOGI("\n*ARI*");
        ALOGI("Lifecycle state        [%u]", healthData->ari_lifecycle_state);
        ALOGI("Fuse gauge             [%u]", healthData->ari_fuse_gauge);

        ALOGI("\n*FIPS*");
        ALOGI("Fips enabled           [%u]", healthData->fips_enabled);
        ALOGI("Fips self test passed  [%u]", healthData->fips_self_test_passed);

        ALOGI("\n*Sendors*");
        ALOGI("Sensors calibrated     [%u]", healthData->sensors_calibrated);

        ALOGI("\n*POR indicator*");
        ALOGI("POR indicator          [%u]", healthData->por_indicator);

        ALOGI("\n*NVM result*");
        ALOGI("POR indicator          [%u]", healthData->nvm_result);

        ALOGI("\n*POR data*");
        ALOGI("POR counter            [%u]", healthData->por_data.por_counter);
        ALOGI("Normal boot count      [%u]", healthData->por_data.normal_boot_count_since_por);
        ALOGI("Commit count           [%u]", healthData->por_data.commit_count_since_por);

        ALOGI("\n*Aux Data*");
        ALOGI("AUX DATA 0             [0x%x]", healthData->aux_data0);
        ALOGI("AUX DATA 1             [0x%x]", healthData->aux_data1);
        ALOGI("AUX DATA 2             [0x%x]", healthData->aux_data2);
        ALOGI("AUX DATA 3             [0x%x]", healthData->aux_data3);

        ALOGI("\n*Data Mismatch*");
        ALOGI("Current boot mismatch          [%u]",
            healthData->mismatch_data.current_boot_mismatch);
        ALOGI("POR boot mismatch number       [%u]",
            healthData->mismatch_data.por_boot_mismatch_num);
        ALOGI("Normal boot mismatch number    [%u]",
            healthData->mismatch_data.normal_boot_mismatch_num);
        ALOGI("Fuse count left by granularity [%u]",
            healthData->mismatch_data.fuse_count_left_by_granularity);
    });

    uint32_t buildVer = 0;
    uint32_t buildHash = 0;
    uint32_t hwVer = 0;
    uint8_t  iskId[SP_DEVICE_ISK_ID_SIZE_BYTES] = {0};

    int32_t ret = sysParamReadWrapper(SP_SYSPARAM_ID_BUILD_VERSION, 0, 0, &buildVer);
    if (ret)
        ++errCount;

    ret = sysParamReadWrapper(SP_SYSPARAM_ID_BUILD_VERSION_HASH, 0, 0, &buildHash);
        if (ret)
            ++errCount;

    ret = sysParamReadWrapper(SP_SYSPARAM_ID_HW_VERSION, 0, 0, &hwVer);

    ret = sysDataReadWrapper(SP_SYSDATA_ID_GET_DEVICE_ISK_ID, 0, 0, iskId, sizeof(iskId));

    if (ret)
        ++errCount;

    ALOGI("SPU isk_id [%02x%02x%02x%02x%02x%02x%02x]", iskId[0], iskId[1], iskId[2],
        iskId[3], iskId[4], iskId[5], iskId[6]);

    uint32_t buildVerMajor = buildVer >> 16;
    uint32_t buildVerMinor = buildVer & 0x0000FFFF;

    // Print build info
    ALOGI("SPU build v%d.%d.%08X, HW v%08X", buildVerMajor, buildVerMinor, buildHash, hwVer);

    // Print apps info
    for (uint8_t i = 0; i < numOfBootApps; ++i) {

        uint32_t appVer = 0;
        uint32_t appHash = 0;

        ret = sysParamReadWrapper(SP_SYSPARAM_ID_APP_BUILD_VERSION, bootAppsDescriptors[i].uuid,
            0, &appVer);
        if (ret)
            ++errCount;
        ret = sysParamReadWrapper(SP_SYSPARAM_ID_APP_BUILD_VERSION_HASH,
            bootAppsDescriptors[i].uuid, 0, &appHash);
        if (ret)
            ++errCount;

        uint32_t appVerMajor = appVer >> 16;
        uint32_t appVerMinor = appVer & 0x0000FFFF;

        ALOGI("SPU app[%s] v%d.%d.%08X", bootAppsDescriptors[i].appName, appVerMajor, appVerMinor,
            appHash);
    }

    return errCount;
}

int32_t SPUTestFw::spuReset()
{
    ALOGD("Reset SPU");

    sp<ISPUManager> spuManager = getSPUManagerService();
    if (!spuManager)
       return FRAMEWORK_ERROR;

    // Reset SPU
    int ret = spuManager->resetSpu();
    if (ret) {
        ALOGE("Failed to reset SPU, err[%d]", ret);
        return SSR_ERROR;
    }

   return SUCCESS;
}

int32_t SPUTestFw::startSSRHandling()
{
    ALOGD("Start SSR handling");

    if (!spuManager || !notifier)
        return FRAMEWORK_ERROR;

    ssrState = SSR_STATE_STARTED;

    ssrHandleThread = new thread(handleSSRThreadActivity);
    if (!ssrHandleThread) {
        ALOGE("Failed to run SSR handle thread");
        stopSSRHandling();
        return FRAMEWORK_ERROR;
    }

    ALOGD("Wait for SSR handling to be in ready state");
    uint32_t retries = waitForSSRReadyRetries;
    while(ssrState != SSR_STATE_READY && --retries) {
         this_thread::sleep_for(chrono::seconds(1 /*second*/));
    }

    if(ssrState != SSR_STATE_READY) {
        ALOGE("Failed to start SSR handling test");
        stopSSRHandling();
        return FRAMEWORK_ERROR;
    }

    ALOGD("Set events notifier [%p]", notifier.get());
    int32_t ret = spuManager->setEventNotifier(notifier);
    if (ret) {
        ALOGE("Failed to set notifier");
        stopSSRHandling();
        return SSR_ERROR;
    }

    return SUCCESS;
}

void SPUTestFw::stopSSRHandling()
{
    ALOGD("Clear events notifer [%p]", notifier.get());
    if (notifier)
        spuManager->clearEventNotifier();

    ALOGD("Stop SSR handling");
    ssrState = SSR_STATE_SHUTDOWN;

    if (notifier) {
        ALOGD("Trigger SSR cb to wake up SSR handling thread");
        notifier->callback(SPU_HAL_EVENT_SSR);
    }

    if (ssrHandleThread) {
        ALOGD("Waiting for SSR thread to be joined");
        ssrHandleThread->join();

        delete ssrHandleThread;
        ssrHandleThread = nullptr;
    }

    ssrState = SSR_STATE_OFF;
    notifier.clear();
}

int32_t SPUTestFw::waitForSpuReady()
{
    if(!spuManager)
        return FRAMEWORK_ERROR;

    // Wait for SPU to be ready
    if(spuManager->waitForSpuReady(waitForSpuReadyTimeoutSec)) {
        ALOGE("SPU not ready");
        return RESOURCE_BUSY;
    }

    return SUCCESS;
}

int32_t SPUTestFw::setNotifier(sp<ISPUNotifier> notifier)
{
    if (!spuManager)
        return FRAMEWORK_ERROR;

    return spuManager->setEventNotifier(notifier);
}

int32_t SPUTestFw::clearNotifier()
{
    if (!spuManager)
        return FRAMEWORK_ERROR;

    return spuManager->clearEventNotifier();
}
