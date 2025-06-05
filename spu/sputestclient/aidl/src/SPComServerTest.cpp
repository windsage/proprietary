/*!
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/hardware/spu/BnSPUManager.h>
#include <aidl/android/hardware/common/Ashmem.h>
#include <qti-utils.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <sys/mman.h>
#include <cutils/ashmem.h>
#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"
#include "SPComServerTest.h"


using namespace ::aidl::vendor::qti::hardware::spu;
using namespace std;
using aidl::vendor::qti::hardware::spu::ISPUManager;
using aidl::vendor::qti::hardware::spu::ISPComServer;
using aidl::android::hardware::common::Ashmem;
using ::ndk::ScopedAStatus;

SPComServerTest::SPComServerTest(const char* name) : chName(name)
{
    ScopedAStatus status = ScopedAStatus::ok();
    chNameStr = chName.c_str();

    std::shared_ptr<ISPUManager> spuManager = SPUTestFw::getSPUManagerService();
    if (!spuManager)
        return;

    status = spuManager->getSPComServer(chName, &server);
    if (!server)
        return;

    isValid = true;
}

SPComServerTest::~SPComServerTest()
{
    isValid = false;

    if (mem.fd.get() > 0) {
        if (sharedMemory != nullptr){
            munmap(sharedMemory, mem.size);
            sharedMemory = nullptr;
        }

        if (sharedBuffer) {
            sharedBuffer.reset();
        }

        mem.fd.set(-1);
        mem.size = 0;
    }

    server.reset();
    if (server)
        ALOGE("Failed to release SPCom server");
}

int32_t SPComServerTest::openCloseChStress(size_t cycles)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t err = SUCCESS;

    for (size_t i = 0; i < cycles; ++i) {
        err = SUCCESS;
        // Register spcom channel
        status = server->registerServer(&err);
        if(err != SUCCESS || !status.isOk()) {
            ALOGE("Failed to register SPCom server, cycle[%zu], err[%d]", i, err);
            return CHANNEL_ERROR;
        }

        // Unregister spcom channel
        status = server->unregisterServer(&err);
        if((err != SUCCESS) || !status.isOk()) {
            ALOGE("Failed to unregister SPCom server, cycle[%zu], err[%d]", i, err);
            return CHANNEL_ERROR;
        }
    }

    return SUCCESS;
}

int32_t SPComServerTest::registerServer()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t err = SUCCESS;

    ALOGD("Register server [%s] was called", chNameStr);

    status = server->registerServer(&err);
    if((err != SUCCESS) || !status.isOk()) {
        ALOGD("Failed to register server [%s]", chNameStr);
        return CHANNEL_ERROR;
    }

    return SUCCESS;
}

int32_t SPComServerTest::unregisterServer()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t err = SUCCESS;

    ALOGD("Unregister server [%s] was called", chNameStr);

    status = server->unregisterServer(&err);
    if ((err != SUCCESS) || !status.isOk()) {
        ALOGE("Failed to unregister server [%s]", chNameStr);
        return CHANNEL_ERROR;
    }

    return SUCCESS;
}

int32_t SPComServerTest::waitForPingSendPong()
{
    if (waitForNextRequest(PING))
        return REQUEST_ERROR;

    const char* respStr = "PONG";
    size_t respStrLen = strnlen(respStr, maxSpcomMessageLength);
    std::vector<uint8_t> response(respStrLen + 1);
    memscpy(response.data(), response.size(), respStr, respStrLen);

    int32_t err = sendResponse(response);
    if (err != SUCCESS)
        return RESPONSE_ERROR;

    return SUCCESS;
}

int32_t SPComServerTest::sendModifiedResponseFlow()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    if (waitForNextRequest(SHARED_BUFFER_REQUEST))
        return REQUEST_ERROR;

    ret = getSPComSharedBuffer(pgSz);
    if (ret != SUCCESS) {
        return SHARED_BUFFER_ERROR;
    }

    if (sendInvalidModifiedResponse()) {
        ALOGE("Send invalid shared buffer response size should fail");
        return SHARED_BUFFER_ERROR;
    }

    memset(sharedMemory, '1', pgSz);

    status = sharedBuffer->copyToSpu(0, pgSz, &ret);
    if (!status.isOk() || ret != SUCCESS) {
        ALOGE("Failed to copy to SPU");
        return SHARED_BUFFER_ERROR;
    }

    std::vector<uint8_t> response(sizeof(SharedBufferResponseStruct));
    ((SharedBufferResponseStruct*)(response.data()))->sharedBufferSz = pgSz;

    if (sendModifiedResponse(response, 0))
        return RESPONSE_ERROR;

    if (waitForNextRequest(SHARED_BUFFER_READ))
        return REQUEST_ERROR;

    status = sharedBuffer->copyFromSpu(0, pgSz, &ret);
    if (!status.isOk() || ret != SUCCESS) {
        ALOGE("Failed to copy from SPU");
        return SHARED_BUFFER_ERROR;
    }

    uint8_t expectedByte = '2';

    for(uint32_t i = 0; i < pgSz; ++i) {
        if(((uint8_t*)sharedMemory)[i] != expectedByte) {
            ALOGE("Read shared buffer expected all bytes [%c], found [%c], index[%d]", expectedByte, ((uint8_t*)sharedMemory)[i], i);
            return UNEXPECTED_ERROR;
        }
    }

    const char* respStr = "PASS";
    size_t respStrLen = strnlen(respStr, maxSpcomMessageLength);
    std::vector<uint8_t> passResp(respStrLen + 1);
    memscpy(passResp.data(), passResp.size(), respStr, respStrLen);

    if (sendResponse(passResp))
        return RESPONSE_ERROR;

    // Write shared buffer test
    if (waitForNextRequest(SHARED_BUFFER_WRITE))
        return REQUEST_ERROR;

    expectedByte = '3';

    memset(sharedMemory, expectedByte, pgSz);

    status = sharedBuffer->copyToSpu(0, pgSz, &ret);
    if(ret || !status.isOk()) {
        ALOGE("Failed to copy to SPU");
        return SHARED_BUFFER_ERROR;
    }

    if (sendResponse(passResp))
        return RESPONSE_ERROR;

    // Release shared buffer
    if (waitForNextRequest(SHARED_BUFFER_RELEASE))
        return REQUEST_ERROR;

    // Do nothing, will be released on SSR
    if (sendResponse(passResp))
        return RESPONSE_ERROR;

    if (releaseSPComSharedBuffer()){
        return RESOURCE_BUSY;
    };

    return SUCCESS;
}

int32_t SPComServerTest::waitForNextRequest(SPComServerRequestId expectedRequest)
{
    ScopedAStatus status = ScopedAStatus::ok();
    ISPComServer::data_st waitReq;
    int32_t ret = SUCCESS;

    ALOGD("SPCom server [%s] is waiting for a new requests...", chNameStr);

    waitReq.size = 0;
    status = server->waitForRequest(&waitReq, &ret);
    if (!status.isOk() || SUCCESS != ret) {
        ALOGE("Wait for request failed, err [%d]", ret);
        ret = MESSAGING_ERROR;
        return ret;
    }

    size_t reqSz = waitReq.size;
    if (reqSz < sizeof(SPComServerRequestId)) {
        ALOGE("Invalid request size [%ld]", reqSz);
        return REQUEST_ERROR;
    }

    SPComServerRequestId reqType = *(SPComServerRequestId*)waitReq.data.data();
    if (reqType != expectedRequest) {
        ALOGE("Invalid request type [%u], expected [%u]", reqType, expectedRequest);
        return REQUEST_ERROR;
    }

    ALOGD("Got a new request of type [%u], request size [%ld]", reqType, reqSz);

    return ret;
}

int32_t SPComServerTest::sendResponse(std::vector<uint8_t> response)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    ALOGD("Send response was called");

    status = server->sendResponse(response, &ret);
    if (ret < 0 || !status.isOk()) {
        ALOGE("Failed to send response");
        return MESSAGING_ERROR;
    }

    return SUCCESS;
}

int32_t SPComServerTest::getSPComSharedBuffer(uint32_t nBytes)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int ret = SUCCESS;
    int32_t fd = -1;

    ALOGD("Get shared buffer with size of [%u] bytes", nBytes);
    mem.fd.set(ashmem_create_region(nullptr, nBytes));

    fd = mem.fd.get();
    if (fd == -1) {
        ALOGE("failed to allocate memory");
        return HAL_MEMORY_ERROR;
    }

    mem.size = nBytes;

    // Map HAL memory
    sharedMemory = mmap(nullptr, mem.size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (sharedMemory == nullptr || sharedMemory == (void*) -1) {
        ALOGE("Failed to map HAL memory");
        ret = GENERAL_ERROR;
    }

    // Create SpcomSharedBuffer
    status = server->getSPComSharedBuffer(mem, &sharedBuffer);
    if (!status.isOk() || sharedBuffer == nullptr) {
        ALOGE("Failed to get shared buffer");
        ret = SPCOM_MEMORY_ERROR;
    }

    return ret;
}

int32_t SPComServerTest::releaseSPComSharedBuffer()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int ret = SUCCESS;
    int32_t fd = -1;

    if (sharedBuffer) {
        sharedBuffer.reset();
        sharedBuffer = nullptr;
        ALOGD("Resetting sharedBuffer");
    }

    if (sharedMemory) {
        if (!(ret = munmap(sharedMemory, mem.size))) {
            sharedMemory = nullptr;
            ALOGD("un-mapping sharedMemory");
        } else {
            ALOGE("Failed to un-map shared buffer");
        }
    }

    mem.fd.set(-1);
    mem.size = 0;

    return ret;
}

int32_t SPComServerTest::sendInvalidModifiedResponse()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    ALOGD("Send invalid shared buffer response");

    std::vector<uint8_t> response(0);
    uint32_t offset = sizeof(void*);

    status = server->sendModifiedResponse(response, sharedBuffer, offset, &ret);
    if (!ret && status.isOk()) {
        ALOGE("Send shared buffer response with zero size should fail");
        return MESSAGING_ERROR;
    }

    return SUCCESS;
}

int32_t SPComServerTest::sendModifiedResponse(std::vector<uint8_t> &response, uint32_t offset)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    ALOGD("Send modified response");

    size_t respSz = response.size();
    if (!respSz || ! sharedBuffer)
        return INVALID_PARAM;

    status = server->sendModifiedResponse(response, sharedBuffer, offset, &ret);
    if (ret || !status.isOk()){
        return MESSAGING_ERROR;
    }

    return SUCCESS;
}

int32_t SPComServerTest::handleSSR()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    int32_t err = SUCCESS;

    if (sharedBuffer) {
        status  = sharedBuffer->copyFromSpu(0, pgSz, &err);
        if(err || !status.isOk()) {
            ALOGE("Copy from SPU fails after SSR, err[%d]", err);
            ret = MESSAGING_ERROR;
        }

        status = sharedBuffer->copyToSpu(0, pgSz, &err);
        if (err || !status.isOk()) {
            ALOGE("Copy to SPU fails after SSR, err[%d]", err);
            ret = MESSAGING_ERROR;
        }

        // Release shared buffer
        ALOGD("Clear SPU shared buffer");
        sharedBuffer.reset();
        if (sharedBuffer) {
            ALOGE("Shared buffer wasn't released");
            ret = GENERAL_ERROR;
        }
    }

    // Unregister server
    if (server) {
        ALOGD("Unregister server");
        ret = unregisterServer();
        if (ret) {
            ALOGE("Unregister server [%s] failed", chNameStr);
            ret = GENERAL_ERROR;
        }
    }

   return ret;
}
