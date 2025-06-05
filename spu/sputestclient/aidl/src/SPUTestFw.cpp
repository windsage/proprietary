/*!
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <chrono>
#include <fstream>
#include <aidl/vendor/qti/hardware/spu/BnSPUNotifier.h>
#include <aidl/android/hardware/common/Ashmem.h>
#include <aidl/vendor/qti/hardware/spu/BnSPUManager.h>
#include <spu_definitions.h>
#include "log.h"
#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"

#include <android/binder_manager.h>
#include <android/binder_process.h>


using namespace std;
using namespace chrono;
using ::ndk::SpAIBinder;
using aidl::vendor::qti::hardware::spu::ISPUNotifier;
using aidl::vendor::qti::hardware::spu::ISPUManager;
using aidl::android::hardware::common::Ashmem;

std::shared_ptr<ISPUManager> SPUTestFw::spuManager(nullptr);
std::shared_ptr<SPUNotifier> SPUTestFw::notifier(nullptr);
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

    setSSRThreadState(SSR_STATE_OFF, SUCCESS);
    ALOGD("SSR handle thread Exited");
}

int32_t SPUTestFw::init()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    unique_lock<mutex> lk(mtx);

    // Init SPU HAL service
    if (!spuManager) {
        ALOGD("get SpuManager service handle");
        // getting service
        const std::string instance = std::string() + ISPUManager::descriptor + "/default";
        spuManager = ISPUManager::fromBinder(SpAIBinder(AServiceManager_checkService(instance.c_str())));
        if (!spuManager) {
            ALOGE("Failed to check service ISPUManager");
            return FRAMEWORK_ERROR;
        }
    }

    ABinderProcess_startThreadPool();

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
        notifier = ndk::SharedRefBase::make<SPUNotifier>();
        if (!notifier) {
            ALOGE("Notifier obj is not valid");
            return FRAMEWORK_ERROR;
        }
    }

    // Wait for SPU to be ready
    status = spuManager->waitForSpuReady(waitForSpuReadyTimeoutSec, &ret);
    if (ret || !status.isOk()) {
        ALOGE("SPU not ready");
        return RESOURCE_BUSY;
    }

    return SUCCESS;
}

void SPUTestFw::release()
{
    if (spcomServerTest) {
        ALOGD("Release SpcomServerTest obj");
        delete spcomServerTest;
        spcomServerTest = nullptr;
    }

    if (spuManager) {
        ALOGD("Release spuManager server handle");
        spuManager.reset();
    }

    if (spcomClientTest) {
        ALOGD("Release SpcomClientTest obj");
        delete spcomClientTest;
        spcomClientTest = nullptr;
    }

}

/**
 * Load SPU application:
 *   > Allocate HIDL shared memory
 *   > Copy The application content to the shared memory
 *   > Call loadApp and share HIDL shared memory with server
 *   > Check app is loaded
 */
int32_t SPUTestFw::loadApp(const char* appFilePath, const char* appName, const uint32_t uuid)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    bool isLoaded = 0;
    void * sharedMemory = nullptr;
    static Ashmem shared;

    if (!spuManager)
        return FRAMEWORK_ERROR;

    if (!appFilePath || !appName) {
        ALOGE("Load app invalid params");
        return INVALID_PARAM;
    }
    status = spuManager->isAppLoaded(appName, &isLoaded);
    if (isLoaded || !status.isOk()) {
        ALOGD("App is already loaded [%s]", appName);
        return ALREADY_LOADED;
    }

    ALOGD("Going to load app [%s], channel [%s] uuid [0x%x]", appFilePath, appName, uuid);

    // Open app image for read
    ifstream appBinFile(appFilePath, ifstream::in);
    if (!appBinFile) {
        ALOGE("Couldn't open image file");
        return GENERAL_ERROR;
    }

    // Read app size
    appBinFile.seekg(0, appBinFile.end);
    int32_t fileLen = appBinFile.tellg();
    appBinFile.seekg(0, appBinFile.beg);

    if (fileLen <= 0) {
        ALOGE("Failed to read file [%s], size[%d]", appFilePath, fileLen);
        return GENERAL_ERROR;
    }

    int32_t fd = -1;

    ALOGD("Get shared buffer with size of [%u] bytes", fileLen);
    shared.fd.set(ashmem_create_region(nullptr, fileLen));

    fd = shared.fd.get();
    if (fd == -1) {
        ALOGE("failed to allocate memory");
        return HAL_MEMORY_ERROR;
    }

    shared.size = fileLen;

    // Map HAL memory
    sharedMemory = mmap(nullptr, fileLen, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == nullptr || sharedMemory == (void*) -1) {
        ALOGE("Failed to map HAL memory");
        ret = GENERAL_ERROR;
        goto cleanup;
    }

    for (long int i = 0; i < fileLen; ++i) {
        *(((uint8_t*) sharedMemory) + i) = appBinFile.get();
    }

    // Load application share with HAL server over HAL shared memory
    status = spuManager->loadApp(appName, shared, fileLen, &ret);
    if (ret != 0 || !status.isOk()) {
        ALOGE("Failed to load app[%s], res [%d]", appName, ret);
        ret = LOAD_APP_ERROR;
        goto cleanup;
    }

    // Check if app was loaded successfully
    status = spuManager->isAppLoaded(appName, &isLoaded);
    if (!isLoaded || !status.isOk()) {
        ALOGE("App [%s] is not loaded", appName);
        ret = LOAD_APP_ERROR;
        goto cleanup;
    }

    if (0 != uuid) {
        status = spuManager->isAppLoadedByUUID(uuid, &isLoaded);
        if (!isLoaded || !status.isOk()) {
            ALOGE("Failed to verify App [%s] loaded by uuid [%x]", appName, uuid);
            ret = LOAD_APP_ERROR;
            goto cleanup;
        }
        ALOGD("App [%s] verified by uuid [0x%x]", appFilePath,uuid);
    }

    ALOGD("App [%s] was loaded successfully, channel [%s]", appFilePath,
        appName);

cleanup:
    if (sharedMemory != nullptr) {
        if (!munmap(sharedMemory, fileLen)) {
            sharedMemory = nullptr;
            ALOGD("un-mapping sharedMemory");
        } else {
            ALOGE("Failed to un-map shared buffer");
        }
    }

    return ret;
}

int32_t SPUTestFw::sysParamReadWrapper(uint32_t id, uint32_t arg1, uint32_t arg2, uint32_t *val)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    ISPUManager::data_st readParam;

    readParam.size = 0;
    status = spuManager->sysParamRead(id, arg1, arg2, &readParam, &ret);
    if (ret || !status.isOk() || readParam.size == 0) {
        return GENERAL_ERROR;
    }

    *val = *(int32_t *)readParam.data.data();

    return ret;
}

int32_t SPUTestFw::sysDataReadWrapper(uint32_t id, uint32_t arg1, uint32_t arg2, uint8_t *val, uint32_t size)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    ISPUManager::data_st readData;

    readData.size = 0;

    status = spuManager->sysDataRead(id, arg1, arg2, &readData, &ret);
    if (ret || !status.isOk() || readData.size == 0) {
        ret = GENERAL_ERROR;
    }
    else if(size > readData.size) {
        ret = INVALID_PARAM;
    }
    else {
        memcpy(val, readData.data.data(), size);
        ret = SUCCESS;
    }

    return ret;
}

// Test SPU health status API and prints SPU applications version
int32_t SPUTestFw::readSpuInfo()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    ISPUManager::data_st info;
    int32_t maxSpcomMessageSize = 0;
    int32_t maxChannelNameLength = 0;
    int32_t imageType = 0;

    if (!spuManager)
        return FRAMEWORK_ERROR;

    uint32_t errCount = 0;
    status = spuManager->getSPComMaxMessageSize(&maxSpcomMessageSize);
    if (!status.isOk()){
        ALOGE("Failed to get maxSpcomMessageSize");
        return GENERAL_ERROR;
    }
    status = spuManager->getSPComMaxChannelNameLength(&maxChannelNameLength);
    if (!status.isOk()){
        ALOGE("Failed to get maxChannelNameLength");
        return GENERAL_ERROR;
    }

    status = spuManager->getImageType(&imageType);
    if (!status.isOk()){
        ALOGE("Failed to get imageType");
        return GENERAL_ERROR;
    }


    // Print health status
    info.size = 0;
    status = spuManager->checkHealth(&info, &ret);
    if (ret || !status.isOk() || info.size == 0) {
        ++errCount;
        return SPU_INFO_TEST_ERROR;
    }

    sp_health_status_data *healthData = (sp_health_status_data *)info.data.data();

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

    uint32_t buildVer = 0;
    uint32_t buildHash = 0;
    uint32_t hwVer = 0;
    uint8_t  iskId[SP_DEVICE_ISK_ID_SIZE_BYTES] = {0};

    ret = sysParamReadWrapper(SP_SYSPARAM_ID_BUILD_VERSION, 0, 0, &buildVer);
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
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    ALOGD("Reset SPU");

    std::shared_ptr<ISPUManager> spuManager = getSPUManagerService();
    if (!spuManager)
       return FRAMEWORK_ERROR;

    // Reset SPU
    status = spuManager->resetSpu(&ret);
    if (ret || !status.isOk()) {
        ALOGE("Failed to reset SPU, err[%d]", ret);
        return SSR_ERROR;
    }

   return SUCCESS;
}

int32_t SPUTestFw::startSSRHandling()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    ALOGD("Start SSR handling");

    if (!spuManager || !notifier){
        ALOGE("SSRHandling:Null input. %d %d", spuManager != nullptr, notifier != nullptr);
        return FRAMEWORK_ERROR;
    }

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
    status = spuManager->setEventNotifier(notifier, &ret);
    if (ret || !status.isOk()) {
        ALOGE("Failed to set notifier");
        stopSSRHandling();
        return SSR_ERROR;
    }

    return SUCCESS;
}

void SPUTestFw::stopSSRHandling()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    ALOGD("Clear events notifer [%p]", notifier.get());
    if (notifier){
        status = spuManager->clearEventNotifier(&ret);
        if (ret || !status.isOk()) {
            ALOGE("Failed to clear event notifier");
        }
    }

    ALOGD("Stop SSR handling");
    ssrState = SSR_STATE_SHUTDOWN;

    if (notifier) {
        ALOGD("Trigger SSR cb to wake up SSR handling thread");
        status = notifier->callback(SPU_HAL_EVENT_SSR);
        if (!status.isOk()) {
            ALOGE("Failed to callback notifier");
        }
    }

    if (ssrHandleThread) {
        ALOGD("Waiting for SSR thread to be joined");

        ssrHandleThread->join();

        ALOGD("SSR joined");

        delete ssrHandleThread;
        ssrHandleThread = nullptr;
    }

    ssrState = SSR_STATE_OFF;
    notifier.reset();
}

int32_t SPUTestFw::waitForSpuReady()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    if(!spuManager)
        return FRAMEWORK_ERROR;

    // Wait for SPU to be ready
    status = spuManager->waitForSpuReady(waitForSpuReadyTimeoutSec, &ret);
    if(!status.isOk() || ret) {
        ALOGE("SPU not ready");
        return RESOURCE_BUSY;
    }

    return SUCCESS;
}

int32_t SPUTestFw::setNotifier(std::shared_ptr<ISPUNotifier> notifier)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    if (!spuManager)
        return FRAMEWORK_ERROR;

    status = spuManager->setEventNotifier(notifier, &ret);
    if(!status.isOk()) {
        ALOGE("setEventNotifier failed");
        return FRAMEWORK_ERROR;
    }

    return ret;
}

int32_t SPUTestFw::clearNotifier()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    if (!spuManager)
        return FRAMEWORK_ERROR;

    status = spuManager->clearEventNotifier(&ret);
    if(!status.isOk()) {
        ALOGE("clearNotifier failed");
        return FRAMEWORK_ERROR;
    }

    return ret;
}


