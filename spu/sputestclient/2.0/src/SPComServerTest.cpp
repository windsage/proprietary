/*!
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <vendor/qti/spu/2.0/ISPUManager.h>
#include <qti-utils.h>
#include <string.h>
#include <hidl/HidlSupport.h>
#include <android/hidl/memory/1.0/IMemory.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <chrono>
#include <thread>
#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"
#include "SPComServerTest.h"

using namespace vendor::qti::spu;
using namespace std;
using vendor::qti::spu::V2_0::ISPUManager;
using ::android::hardware::hidl_vec;
using android::sp;

SPComServerTest::SPComServerTest(const char* name) : chName(name)
{
    chNameStr = chName.c_str();

    sp<ISPUManager> spuManager = SPUTestFw::getSPUManagerService();
    if (!spuManager)
        return;

    server = spuManager->getSPComServer(chName);
    if (!server)
        return;

    isValid = true;
}

SPComServerTest::~SPComServerTest()
{
    isValid = false;

    server.clear();
    if (server)
        ALOGE("Failed to release SPCom server");
}

int32_t SPComServerTest::openCloseChStress(size_t cycles)
{
    for (size_t i = 0; i < cycles; ++i) {
        // Register spcom channel
        int32_t err = server->registerServer();
        if(err) {
            ALOGE("Failed to register SPCom server, cycle[%zu], err[%d]", i, err);
            return CHANNEL_ERROR;
        }

        // Unregister spcom channel
        err = server->unregisterServer();
        if(err != SUCCESS) {
            ALOGE("Failed to unregister SPCom server, cycle[%zu], err[%d]", i, err);
            return CHANNEL_ERROR;
        }
    }

    return SUCCESS;
}

int32_t SPComServerTest::registerServer()
{
    ALOGD("Register server [%s] was called", chNameStr);

    if(server->registerServer()) {
        ALOGD("Failed to register server [%s]", chNameStr);
        return CHANNEL_ERROR;
    }

    return SUCCESS;
}

int32_t SPComServerTest::unregisterServer()
{
    ALOGD("Unregister server [%s] was called", chNameStr);

    if (server->unregisterServer()) {
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
    hidl_vec<uint8_t> response(respStrLen + 1);
    memscpy(response.data(), response.size(), respStr, respStrLen);

    if (sendResponse(response))
        return RESPONSE_ERROR;

    return SUCCESS;
}

int32_t SPComServerTest::sendModifiedResponseFlow()
{
    if (waitForNextRequest(SHARED_BUFFER_REQUEST))
        return REQUEST_ERROR;

    if (getSPComSharedBuffer(pgSz))
        return SHARED_BUFFER_ERROR;

    if (sendInvalidModifiedResponse()) {
        ALOGE("Send invalid shared buffer response size should fail");
        return SHARED_BUFFER_ERROR;
    }

    void* data = sharedMemory->getPointer();

    sharedMemory->update();
    memset(data, '1', pgSz);
    sharedMemory->commit();

    sharedBuffer->copyToSpu(0, pgSz);

    hidl_vec<uint8_t> response(sizeof(SharedBufferResponseStruct));
    ((SharedBufferResponseStruct*)(response.data()))->sharedBufferSz = pgSz;

    if (sendModifiedResponse(response, 0))
        return RESPONSE_ERROR;

    if (waitForNextRequest(SHARED_BUFFER_READ))
        return REQUEST_ERROR;

    int32_t ret = sharedBuffer->copyFromSpu(0, pgSz);
    if (ret) {
        ALOGE("Failed to copy from SPU");
        return SHARED_BUFFER_ERROR;
    }

    uint8_t expectedByte = '2';

    sharedMemory->read();

    for(uint32_t i = 0; i < pgSz; ++i) {
        if(((uint8_t*)data)[i] != expectedByte) {
            ALOGE("Read shared buffer expected all bytes [%c], found [%c], index[%d]", expectedByte, ((uint8_t*)data)[i], i);
            return UNEXPECTED_ERROR;
        }
    }

    sharedMemory->commit();

    const char* respStr = "PASS";
    size_t respStrLen = strnlen(respStr, maxSpcomMessageLength);
    hidl_vec<uint8_t> passResp(respStrLen + 1);
    memscpy(passResp.data(), passResp.size(), respStr, respStrLen);

    if (sendResponse(passResp))
        return RESPONSE_ERROR;

    // Write shared buffer test
    if (waitForNextRequest(SHARED_BUFFER_WRITE))
        return REQUEST_ERROR;

    expectedByte = '3';

    sharedMemory->update();
    memset(data, expectedByte, pgSz);
    sharedMemory->commit();

    ret = sharedBuffer->copyToSpu(0, pgSz);
    if(ret) {
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

    return SUCCESS;
}

int32_t SPComServerTest::waitForNextRequest(SPComServerRequestId expectedRequest)
{
    ALOGD("SPCom server [%s] is waiting for a new requests...", chNameStr);

    int32_t ret = SUCCESS;

    server->waitForRequest([&](int32_t status, hidl_vec<uint8_t> request) {
        if (status <= 0) {
            ALOGE("Wait for request failed, err [%d]", status);
            ret = MESSAGING_ERROR;
            return;
        }

        size_t reqSz = request.size();
        if (reqSz < sizeof(SPComServerRequestId)) {
            ALOGE("Invalid request size [%ld]", reqSz);
            ret = REQUEST_ERROR;
            return;
        }

        SPComServerRequestId reqType = *(SPComServerRequestId*)request.data();
        if (reqType != expectedRequest) {
            ALOGE("Invalid request type [%u], expected [%u]", reqType, expectedRequest);
            ret = REQUEST_ERROR;
            return;
        }

        ALOGD("Got a new request of type [%u], request size [%ld]", reqType, reqSz);
    });

    return ret;
}

int32_t SPComServerTest::sendResponse(hidl_vec<uint8_t> response)
{
    ALOGD("Send response was called");

    int32_t ret = server->sendResponse(response);
    if (ret < 0) {
        ALOGE("Failed to send response");
        return MESSAGING_ERROR;
    }

    return SUCCESS;
}

int32_t SPComServerTest::getSPComSharedBuffer(uint32_t nBytes)
{
    ALOGD("Get shared buffer with size of [%u] bytes", nBytes);

    sp<IAllocator> ashmemAllocator = SPUTestFw::getIAllocatorService();
    if(!ashmemAllocator)
        return FRAMEWORK_ERROR;

    int ret = SUCCESS;
    ashmemAllocator->allocate(nBytes, [&](bool success, const hidl_memory &mem) {
        // Check allocation status
        if (!success || !mem.valid()) {
            ALOGE("Failed to allocate shared memory");
            ret = HIDL_MEMORY_ERROR;
        }

        // Map HIDL memory
        sharedMemory = mapMemory(mem);
        if (!sharedMemory) {
            ALOGE("Failed to map HIDL memory");
            ret = GENERAL_ERROR;
        }

       // Create SpcomSharedBuffer
        sharedBuffer = server->getSPComSharedBuffer(mem);
        if (!sharedBuffer) {
            ALOGE("Failed to get shared buffer");
            ret = SPCOM_MEMORY_ERROR;
        }
    });

    return ret;
}

int32_t SPComServerTest::sendInvalidModifiedResponse()
{
    ALOGD("Send invalid shared buffer response");

    hidl_vec<uint8_t> response(0);
    uint32_t offset = sizeof(void*);

    int32_t ret = server->sendModifiedResponse(response, sharedBuffer, offset);
    if (!ret) {
        ALOGE("Send shared buffer response with zero size should fail");
        return MESSAGING_ERROR;
    }

    return SUCCESS;
}

int32_t SPComServerTest::sendModifiedResponse(hidl_vec<uint8_t> &response, uint32_t offset)
{
    ALOGD("Send modified response");

    size_t respSz = response.size();
    if (!respSz || ! sharedBuffer)
        return INVALID_PARAM;

    int32_t ret = server->sendModifiedResponse(response, sharedBuffer, offset);
    if (ret)
        return MESSAGING_ERROR;

    return SUCCESS;
}

int32_t SPComServerTest::handleSSR()
{
    int32_t ret = SUCCESS;
    if (sharedBuffer) {

        int32_t err = sharedBuffer->copyFromSpu(0, pgSz);
        int32_t ret = SUCCESS;
        if(err) {
            ALOGE("Copy from SPU fails after SSR, err[%d]", err);
            ret = MESSAGING_ERROR;
        }

        err = sharedBuffer->copyToSpu(0, pgSz);
        if (err) {
            ALOGE("Copy to SPU fails after SSR, err[%d]", err);
            ret = MESSAGING_ERROR;
        }

        // Release shared buffer
        ALOGD("Clear SPU shared buffer");
        sharedBuffer.clear();
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
