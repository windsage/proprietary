/*!
 *
 * Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <chrono>
#include <fstream>
#include <vendor/qti/spu/1.1/ISPUInfo.h>

#include "log.h"
#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"


using namespace std;
using vendor::qti::spu::V1_1::SPUImageType;
using vendor::qti::spu::V1_1::ISPUInfo;

sp<ISPUManager> SPUTestFw::sSpuManager(nullptr);
sp<IAllocator> SPUTestFw::sAshmemAllocator(nullptr);
sp<SpcomSSREvent> SPUTestFw::sSpcomSSREvent(nullptr);
SpcomClientTest* SPUTestFw::sSpcomClientTest;
SpcomServerTest* SPUTestFw::sSpcomServerTest;
thread* SPUTestFw::sSSRHandleThread(nullptr);
volatile SSRHandlingState SPUTestFw::sSSRState(SSR_STATE_OFF);
bool SPUTestFw::sRegisteredForSSRNotifications(false);
int32_t SPUTestFw::sSSRThreadError(SUCCESS);
mutex SPUTestFw::sMtx;

/* POR indicator 32bit field was split into 2 16bit fields:
   1. POR indicator
   2. NVM result
   Use this helper struct to parse the SPU HAL v1.1 POR Indicator 32bit field
*/
struct por_indicator_helper_struct {
    uint16_t por_indicator;
    uint16_t nvm_result;
};

void SPUTestFw::setSSRThreadState(SSRHandlingState state, int32_t err) {

    unique_lock<mutex> lk(sMtx);

    sSSRThreadError = err;
    sSSRState = state;
}

void SPUTestFw::handleSSRThreadActivity() {

    if(!sSpuManager || !sSpcomSSREvent) {
        ALOGE("Framework isn't initialized, can't handle SSR events");
        setSSRThreadState(SSR_STATE_ERROR, FRAMEWORK_ERROR);
    }

    while(sSSRState != SSR_STATE_SHUTDOWN && sSSRState != SSR_STATE_ERROR) {

        setSSRThreadState(SSR_STATE_READY);
        sSpcomSSREvent->waitForSSREvent();

        if(getSSRHandlingState() == SSR_STATE_SHUTDOWN) {
            ALOGD("SSR thread shut down dtected");
            break;
        }

        setSSRThreadState(SSR_STATE_HANDLE);

        int32_t ret = sSpcomClientTest->handleSSR();
        if(ret != SUCCESS) {
            ALOGE("Failed to handle SSR in SPCOM client");
            setSSRThreadState(SSR_STATE_ERROR, CLIENT_TEST_ERROR);
        }

        ret = sSpcomServerTest->handleSSR();
        if(ret != SUCCESS) {
            ALOGE("Failed to handle SSR in SPCOM server");
            setSSRThreadState(SSR_STATE_ERROR, SERVER_TEST_ERROR);
        }

        sSpcomSSREvent->notifySSRHandled();
    }

    while(sSSRState != SSR_STATE_SHUTDOWN) {
        ALOGD("Waiting for thread shutdown");
        this_thread::sleep_for(chrono::seconds(2 /*seconds*/));
    }

    ALOGD("SSR handle thread shut-down in 2 seconds");
    this_thread::sleep_for(chrono::seconds(2 /*seconds*/));
    setSSRThreadState(SSR_STATE_OFF, SUCCESS);
}

int32_t SPUTestFw::init() {

    unique_lock<mutex> lk(sMtx);

    // Init SPU HAL service
    if(!sSpuManager) {
        ALOGD("get SpuManager service handle");
        sSpuManager = ISPUManager::tryGetService();
        if (!sSpuManager) {
            ALOGE("Failed to get service ISPUManager");
            return FRAMEWORK_ERROR;
        }
    }

    // Init HIDL shared memory allocator service
    if(!sAshmemAllocator) {
        ALOGD("get AshmemAllocator server handle");
        sAshmemAllocator = IAllocator::tryGetService("ashmem");
        if(!sAshmemAllocator) {
            ALOGE("Failed to get shared memory allocator service");
            return FRAMEWORK_ERROR;
        }
    }

    // Init the spcomClientTest
    if(!sSpcomClientTest) {
        ALOGD("Create SpcomClientTest obj");
        sSpcomClientTest = new SpcomClientTest(sSpcomServerAppChannel);
        if(!sSpcomClientTest) {
            ALOGE("Failed to create SpcomClientTest obj");
            return FRAMEWORK_ERROR;
        }
    }

    // Init the spcomServerTest
    if(!sSpcomServerTest) {
        ALOGD("Create SpcomServerTest obj");
        sSpcomServerTest = new SpcomServerTest(sSpcomServerHLOSChannelName);
        if(!sSpcomServerTest) {
            ALOGE("Failed to create SpcomServerTest obj");
            return FRAMEWORK_ERROR;
        }
    }

    // Init SPU SSR event object
    if(!sSpcomSSREvent) {
        ALOGD("Create SpcomSSREvent obj");
        sSpcomSSREvent = new SpcomSSREvent();
        if(!sSpcomSSREvent) {
            ALOGE("SpcomSSREvent obj is not valid");
            return FRAMEWORK_ERROR;
        }
    }

    // Wait for SPU to be ready
    if(sSpuManager->waitForSpuReady(sWaitForSpuReadyTimeoutSec) != 0) {
        ALOGE("SPU not ready");
        return RESOURCE_BUSY;
    }

    return SUCCESS;
}

void SPUTestFw::release() {

    if(sSpcomClientTest) {
        ALOGD("Release SpcomClientTest obj");
        delete sSpcomClientTest;
        sSpcomClientTest = nullptr;
    }

    if(sSpcomServerTest) {
        ALOGD("Release SpcomServerTest obj");
        delete sSpcomServerTest;
        sSpcomServerTest = nullptr;
    }

    if (sSpuManager) {
        ALOGD("Release SPU HIDL server handle");
        sSpuManager.clear();
    }

    if(sAshmemAllocator) {
        ALOGD("Release Ashmem allocator handle");
        sAshmemAllocator.clear();
    }
}

sp<ISPUManager> SPUTestFw::getSPUManagerService() {
    return sSpuManager;
}

sp<IAllocator> SPUTestFw::getIAllocatorService() {
    return sAshmemAllocator;
}

SpcomServerTest* SPUTestFw::getSpcomServerTest() {
    return sSpcomServerTest;
}

SpcomClientTest* SPUTestFw::getSpcomClientTest() {
    return sSpcomClientTest;
}

sp<SpcomSSREvent> SPUTestFw::getSpcomSSREvent() {
    return sSpcomSSREvent;
}

// Load SPU application:
//  Allocate HIDL shared memory
//  Copy The application content to the shared memory
//  Call loadApp and share HIDL shared memory with server
//  Check app is loaded
int32_t SPUTestFw::loadApp(const char* appFilePath, const char* appName) {

    if(appFilePath == nullptr || appName == nullptr) {
        ALOGE("Load app invalid params");
        return INVALID_PARAM;
    }

    if(sSpuManager == nullptr || sAshmemAllocator == nullptr) {
        ALOGE("Framework is not initialized");
        return FRAMEWORK_ERROR;
    }

    if(sSpuManager->isAppLoaded(appName) == true) {
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

    ALOGD("App file size [%d]", fileLen);

    if(fileLen <= 0) {
        ALOGE("Failed to read file [%s], size[%d]", appFilePath, fileLen);
        return GENERAL_ERROR;
    }

    sAshmemAllocator->allocate(fileLen,
       [&](bool success, const hidl_memory &mem) {

       if (!success) {
               ALOGE("Failed to allocate shared memory");
               return GENERAL_ERROR;
           }

           sp<IMemory> memory;

           // Map HIDL shared memory to client
           memory = mapMemory(mem);
           if (memory == nullptr) {
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

           // Load application share with HIDL server over HIDL shared
           // memory
           int32_t ret = sSpuManager->loadApp(appName, appName, mem, fileLen,
                   sMaxSwapSz);

           if (ret != 0) {
               ALOGE("Failed to load app[%s], res [%d]", appName, ret);
               return LOAD_APP_ERROR;
           }

           return SUCCESS;
       });

    // Check if app was loaded successfully
    if(sSpuManager->isAppLoaded(appName) != true) {
        ALOGE("Failed to load app");
        return LOAD_APP_ERROR;
    }

    ALOGD("App [%s] was loaded successfully, channel [%s]", appFilePath,
        appName);

    return SUCCESS;
}

// Test sysparam read API to read SPU application's version
int32_t SPUTestFw::readAppVersion(const char *appName, uint32_t uuid)
{
    if(!sSpuManager)
        return GENERAL_ERROR;

    int32_t ret = SUCCESS;
    sSpuManager->sysparamRead(ISPUManager::SysparamID::APP_BUILD_VERSION,
        uuid, 0, [&](int32_t err, uint32_t buildVer) {
        if (err != 0) {
            ALOGE("Failed to read app build version, err[%d]", err);
            ret = GENERAL_ERROR;
            return;
        }

        ALOGI("%s, v%d.%d", appName, ((uint32_t) - 1 == buildVer) ?
        0 : (buildVer >> 16), ((uint32_t) - 1 == buildVer) ?
        0 : (buildVer & 0xFFFF));
    });

    return SUCCESS;
}

// Test SPU health status API and prints SPU applications version
int32_t SPUTestFw::readSpuInfo() {

    ALOGD("Read SPU info");
    if(sSpuManager == nullptr) {
        ALOGE("Framework is not initialized");
        return FRAMEWORK_ERROR;
    }

    uint32_t errCount = 0;

    // Read from ISPUInfo (v1.1)
    sp<ISPUInfo> spuInfo = sSpuManager->getSPUInfo();
    if(!spuInfo) {
        ALOGE("Failed to get spu info obj handle");
        return FRAMEWORK_ERROR;
    }

    V1_1::SPUImageType imageType = spuInfo->getImageType();
    ALOGD("Image type [%hhu]", imageType);

    uint32_t spcomMsgSz = spuInfo->getSpcomMaxMessageSize();
    ALOGD("Spcom max message size [%d]", spcomMsgSz);

    uint32_t spcomChSz = spuInfo->getSpcomMaxChannelNameLength();
    ALOGD("Spcom max channel size [%d]", spcomChSz);

    spuInfo.clear();

    // Print health status
    sSpuManager->checkHealth(
            [&](int32_t err, const ISPUManager::HealthStatusData &status) {

            if (err != 0) {
                    ALOGE("Failed to read health status, err[%d]", err);
                    ++errCount;
                    return;
                }

                por_indicator_helper_struct* porIndicator =
                    (por_indicator_helper_struct*)&status.porIndicator;

                ALOGI("ARI mode [%u] - DEPRECATED DO NOT USE", status.ariMode);
                ALOGI("ARI fuse gauge [%u]", status.ariFuseGauge);
                ALOGI("Fips enables [%u]", status.fipsEnabled);
                ALOGI("Fips self test [%u]", status.fipsSelfTestPassed);
                ALOGI("Sensors calibrated [%u]", status.sensorsCalibrated);
                ALOGI("Ari on fuse [%u] - DEPRECATED DO NOT USE", status.ariOnFuse);
                ALOGI("POR Indicator [%u]", porIndicator->por_indicator);
                ALOGI("POR status [%u]", status.porCounter);
                ALOGI("AUX data[0] [0x%x]", status.auxData0);
                ALOGI("AUX data[1] [0x%x]", status.auxData1);
                ALOGI("AUX DATA[2] [0x%x]", status.auxData2);
                ALOGI("AUX DATA[3] [0x%x]", status.auxData3);
                ALOGI("ARI lifecycle state [%u]", status.additionalData[0]);
                ALOGI("Normal boot count since POR [%u]", status.additionalData[1]);
                ALOGI("Commit count since por [%u]", status.additionalData[2]);
                ALOGI("Current boot mismatch [%u]", status.additionalData[3]);
                ALOGI("POR boot mismatch num [%u]", status.additionalData[4]);
                ALOGI("Normal boot mismatch num [%u]", status.additionalData[5]);
                ALOGI("Fuse count left by granularity [%u]", status.additionalData[6]);
                ALOGI("NVM retval [%u]", status.additionalData[7]);
            });

    // Read build hash
    sSpuManager->sysparamRead(ISPUManager::SysparamID::BUILD_VERSION_HASH, 0,
        0, [&](int32_t err, uint32_t buildHash) {
            if (err != 0) {
                ALOGE("Failed to read app build version");
                ++errCount;
                return;
            }

            ALOGI("SPSS build hash: %08X", buildHash);
        });

    // Print applications version
    for(uint8_t i = 0; i < sNumOfBootApps; ++i) {

        int32_t err = readAppVersion(
            sBootAppsDescriptors[i].appName, sBootAppsDescriptors[i].uuid);

        if(err != SUCCESS) {
            ALOGE("Read app version failed for app[%s], err[%d]",
                    sBootAppsDescriptors[i].appName, err);
            ++errCount;
        }
    }

    if(errCount != 0) {
        ALOGE("Read SPU info number of errors [%d]", errCount);
        return GENERAL_ERROR;
    }

    return SUCCESS;
}

int32_t SPUTestFw::spuReset() {

    ALOGD("Reset SPU");

    sp<ISPUManager> spuManager = getSPUManagerService();
    if(!spuManager) {
       ALOGE("SPUManager is not initialized");
       return FRAMEWORK_ERROR;
    }

    // Reset SPU
    int32_t ret = sSpuManager->resetSpu();
    if(ret != 0) {
        ALOGE("Failed to reset SPU, err[%d]", ret);
        return SSR_ERROR;
    }

   return SUCCESS;
}

int32_t SPUTestFw::startSSRHandling() {

    ALOGD("Start SSR handling");

    if(!sSpuManager || !sSpcomSSREvent) {
        ALOGE("Framework is not initialized");
        return FRAMEWORK_ERROR;
    }

    sSSRState = SSR_STATE_STARTED;

    sSSRHandleThread = new thread(handleSSRThreadActivity);
    if(!sSSRHandleThread) {
        ALOGE("Failed to run SSR handle thread");
        stopSSRHandling();
        return FRAMEWORK_ERROR;
    }

    ALOGD("Wait for SSR handling to be in ready state");
    uint32_t retries = sWaitForSSRReadyRetries;
    while(sSSRState != SSR_STATE_READY && --retries) {
         this_thread::sleep_for(chrono::seconds(1 /*second*/));
    }

    if(sSSRState != SSR_STATE_READY) {
        ALOGE("Failed to start SSR handling test");
        stopSSRHandling();
        return FRAMEWORK_ERROR;
    }

    ALOGD("Add SSR cb to server [%p]", sSpcomSSREvent.get());
    int32_t ret = sSpuManager->addSSRCallback(sSpcomSSREvent);
    if(ret != SUCCESS) {
        ALOGE("Failed to add SSR callback");
        stopSSRHandling();
        return SSR_ERROR;
    }

    return SUCCESS;
}

void SPUTestFw::stopSSRHandling() {

    ALOGD("Remove SSR callback from server [%p]", sSpcomSSREvent.get());
    if(sSpcomSSREvent) {
        sSpuManager->rmSSRCallback(sSpcomSSREvent);
    }

    ALOGD("Stop SSR handling");
    sSSRState = SSR_STATE_SHUTDOWN;

    if(sSpcomSSREvent) {
        ALOGD("Trigger SSR cb to wake up SSR handling thread");
        sSpcomSSREvent->callback();
    }

    if(sSSRHandleThread) {
        ALOGD("Waiting for SSR thread to be joined");
        sSSRHandleThread->join();

        delete sSSRHandleThread;
        sSSRHandleThread = nullptr;
    }

    sSSRState = SSR_STATE_OFF;
    sSpcomSSREvent.clear();
}

SSRHandlingState SPUTestFw::getSSRHandlingState() {
    return sSSRState;
}

int32_t SPUTestFw::getSSRThreadError() {
    return sSSRThreadError;
}

int32_t SPUTestFw::waitForSpuReady() {

    if(sSpuManager == nullptr) {
        ALOGE("Framework is not initialized");
        return FRAMEWORK_ERROR;
    }

    // Wait for SPU to be ready
    if(sSpuManager->waitForSpuReady(sWaitForSpuReadyTimeoutSec) != 0) {
        ALOGE("SPU not ready");
        return RESOURCE_BUSY;
    }

    return SUCCESS;
}

int32_t SPUTestFw::addSSRCb(sp<ISpcomSSREvent> eventCb) {

    if(sSpuManager == nullptr) {
        ALOGE("Framework is not initialized");
        return FRAMEWORK_ERROR;
    }

    return sSpuManager->addSSRCallback(eventCb);
}

int32_t SPUTestFw::rmSSRCb(sp<ISpcomSSREvent> eventCb) {

    if(sSpuManager == nullptr) {
        ALOGE("Framework is not initialized");
        return FRAMEWORK_ERROR;
    }

    return sSpuManager->rmSSRCallback(eventCb);
}
