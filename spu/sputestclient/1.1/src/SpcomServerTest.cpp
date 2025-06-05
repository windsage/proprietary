/*!
 *
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <vendor/qti/spu/1.1/ISPUManager.h>
#include <qti-utils.h>
#include <string.h>
#include <hidl/HidlSupport.h>
#include <android/hidl/memory/1.0/IMemory.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <chrono>
#include <thread>

#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"
#include "SpcomServerTest.h"

using namespace vendor::qti::spu;
using namespace std;

using vendor::qti::spu::V1_1::ISPUManager;
using vendor::qti::spu::V1_1::ISpcomServer;
using vendor::qti::spu::V1_0::ISpcomSharedBuffer;
using android::sp;
using ::android::hardware::hidl_vec;

SpcomServerTest::SpcomServerTest(const char* channelName) :
    mSpcomServer(nullptr),
    mSharedBuffer(nullptr),
    mHidlSharedMemory(nullptr),
    mIsValid(false) {

    if(channelName == nullptr) {
        ALOGE("Invalid channel Name for spcom server");
        return;
    }

    uint32_t chNameLen = strnlen(channelName, sMaxSpcomMessageLength);
    if(chNameLen == sMaxSpcomMessageLength) {
        ALOGE("ChannelName is too long");
        return;
    }

    ALOGD("Init spcom server [%s]", channelName);

    memset(mChannelName, 0x00, sizeof(mChannelName));
    memscpy(mChannelName, sizeof(mChannelName), channelName, chNameLen);

    sp<ISPUManager> spuManager = SPUTestFw::getSPUManagerService();
    if(!spuManager) {
        ALOGE("Framework is not initialized");
        return;
    }

    mSpcomServer = spuManager->getSpcomServer(channelName);
    if(!mSpcomServer) {
        ALOGE("Failed to get SpcomServer obj");
        return;
    }

    mIsValid = true;
}

SpcomServerTest::~SpcomServerTest() {

    ALOGD("Spcom server [%s] cleanup", mChannelName);

    mIsValid = false;

    mSpcomServer.clear();
    if(mSpcomServer) {
        ALOGE("Failed to release spcom server");
    }

    memset(mChannelName, 0x00, sMaxSpcomChannelNameLength);
}

int32_t SpcomServerTest::registerUnregisterServerStress(size_t cycles) {

    for(size_t i = 0; i < cycles; ++i) {
        // Register spcom channel
        int32_t err = mSpcomServer->registerServer();
        if(err != SUCCESS) {
            ALOGE("Failed to register spcom channel[%s], cycle[%zu], err[%d]",
                sSpcomClientAppChannel, i, err);
            return CHANNEL_ERROR;
        }

        // Unregister spcom channel
        err = mSpcomServer->unregisterServer();
        if(err != SUCCESS) {
            ALOGE("Failed to unregister spcom channel[%s], cycle[%zu], err[%d]",
                sSpcomClientAppChannel, i, err);
            return CHANNEL_ERROR;
        }
    }

    return SUCCESS;
}

int32_t SpcomServerTest::registerServer() {

    ALOGD("Register server [%s] was called", mChannelName);

    if(mSpcomServer->registerServer() != 0) {
        ALOGD("Failed to register server [%s]", mChannelName);
        return CHANNEL_ERROR;
    }

    return SUCCESS;
}

int32_t SpcomServerTest::unregisterServer() {

    ALOGD("Unregister server [%s] was called", mChannelName);

    if(mSpcomServer->unregisterServer() != 0) {
        ALOGE("Failed to unregister server [%s]", mChannelName);
        return CHANNEL_ERROR;
    }

    return SUCCESS;
}

int32_t SpcomServerTest::waitForPingSendPong() {

    if(waitForNextRequest(PING) != SUCCESS) {
        return REQUEST_ERROR;
    }

    const char* respCStr = "PONG";
    size_t respCStrLen = strnlen(respCStr, sMaxSpcomMessageLength);
    hidl_vec<uint8_t> response(respCStrLen + 1);
    memscpy(response.data(), response.size(), respCStr, respCStrLen);

    if(sendResponse(response) != SUCCESS) {
        return RESPONSE_ERROR;
    }

    return SUCCESS;
}

int32_t SpcomServerTest::sendSharedBufferResponseFlow() {

    // Init shared buffer test

    if(waitForNextRequest(SHARED_BUFFER_REQUEST) != SUCCESS) {
        return REQUEST_ERROR;
    }

    if(getSpcomSharedBuffer(sPgSz) != SUCCESS) {
        return SHARED_BUFFER_ERROR;
    }

    if(sendInvalidSharedBufferResponse() != SUCCESS) {
        ALOGE("Send invalid shared buffer response size should fail");
        return SHARED_BUFFER_ERROR;
    }

    void* data = mHidlSharedMemory->getPointer();

    mHidlSharedMemory->update();
    memset(data, '1', sPgSz);
    mHidlSharedMemory->commit();

    mSharedBuffer->copyToSpu();

    hidl_vec<uint8_t> response(sizeof(SharedBufferResponseStruct));
    ((SharedBufferResponseStruct*)(response.data()))->sharedBufferSz = sPgSz;
    if(sendSharedBufferResponse(response, 0) != SUCCESS) {
        return RESPONSE_ERROR;
    }

    // Read shared buffer test

    if(waitForNextRequest(SHARED_BUFFER_READ) != SUCCESS) {
        return REQUEST_ERROR;
    }

    int32_t ret = mSharedBuffer->copyFromSpu();
    if(ret != SUCCESS) {
        ALOGE("Failed to copy from SPU");
        return SHARED_BUFFER_ERROR;
    }

    uint8_t expectedByte = '2';

    mHidlSharedMemory->read();

    for(uint32_t i = 0; i < sPgSz; ++i) {
        if(((uint8_t*)data)[i] != expectedByte) {
            ALOGE("Read shared buffer expected all bytes [%c], found [%c], index[%d]",
                expectedByte, ((uint8_t*)data)[i], i);
            return UNEXPECTED_ERROR;
        }
    }

    mHidlSharedMemory->commit();

    const char* respCStr = "PASS";
    size_t respCStrLen = strnlen(respCStr, sMaxSpcomMessageLength);
    hidl_vec<uint8_t> passResp(respCStrLen + 1);
    memscpy(passResp.data(), passResp.size(), respCStr, respCStrLen);

    if(sendResponse(passResp) != SUCCESS) {
        return RESPONSE_ERROR;
    }

    // Write shared buffer test

    if(waitForNextRequest(SHARED_BUFFER_WRITE) != SUCCESS) {
        return REQUEST_ERROR;
    }

    expectedByte = '3';

    mHidlSharedMemory->update();
    memset(data, expectedByte, sPgSz);
    mHidlSharedMemory->commit();

    ret = mSharedBuffer->copyToSpu();
    if(ret != SUCCESS) {
        ALOGE("Failed to copy to SPU");
        return SHARED_BUFFER_ERROR;
    }

    if(sendResponse(passResp) != SUCCESS) {
        return RESPONSE_ERROR;
    }

    // Release shared buffer

    if(waitForNextRequest(SHARED_BUFFER_RELEASE) != SUCCESS) {
        return REQUEST_ERROR;
    }

    // Do nothing, will be released on SSR

    if(sendResponse(passResp) != SUCCESS) {
        return RESPONSE_ERROR;
    }

    return SUCCESS;
}

int32_t SpcomServerTest::waitForNextRequest(
    SpcomServerRequestId expectedRequest) {

    ALOGD("Spcom server [%s] is waiting for a new requests...", mChannelName);

    int32_t ret = SUCCESS;

    mSpcomServer->waitForRequest([&](hidl_vec<uint8_t> request, int32_t err) {

        if(err != SUCCESS) {
            ALOGE("Wait for request failed, err [%d]", err);
            ret = MESSAGING_ERROR;
            return;
        }

        size_t reqSz = request.size();

        if(reqSz < sizeof(SpcomServerRequestId)) {
            ALOGE("Invalid request size [%ld]", reqSz);
            ret = REQUEST_ERROR;
            return;
        }

        SpcomServerRequestId reqType = *(SpcomServerRequestId*)request.data();

        if(reqType != expectedRequest) {
            ALOGE("Invalid request type [%u], expected [%u]", reqType,
                expectedRequest);
            ret = REQUEST_ERROR;
            return;
        }

        ALOGI("Got a new request of type [%u], request size [%ld]", reqType,
            reqSz);
    });

    return ret;
}

int32_t SpcomServerTest::sendResponse(hidl_vec<uint8_t> response) {

    ALOGD("Send response was called");

    size_t respSz = response.size();
    if(respSz == 0) {
        ALOGE("Invalid response size");
        return INVALID_PARAM;
    }

    int32_t ret = mSpcomServer->sendResponse(response);
    if(ret != SUCCESS) {
        ALOGE("Failed to send response");
        return MESSAGING_ERROR;
    }

    return SUCCESS;
}

int32_t SpcomServerTest::getSpcomSharedBuffer(uint32_t nBytes) {

    ALOGD("Get shared buffer with size of [%u] bytes", nBytes);

    sp<IAllocator> ashmemAllocator = SPUTestFw::getIAllocatorService();
    if(ashmemAllocator == nullptr) {
        ALOGE("Failed to allocate HIDL shared memory");
        return FRAMEWORK_ERROR;
    }

    int ret = SUCCESS;
    ashmemAllocator->allocate(nBytes,
        [&](bool success, const hidl_memory &mem) {

        // Check allocation status
        if (success == false || mem.valid() == false) {
            ALOGE("Failed to allocate shared memory");
            ret = HIDL_MEMORY_ERROR;
        }

        // Map HIDL memory buffer
        mHidlSharedMemory = mapMemory(mem);
        if (mHidlSharedMemory == nullptr) {
            ALOGE("Failed to map HIDL memory");
            ret = GENERAL_ERROR;
        }

       // Create SpcomSharedBuffer - double buffer
        mSharedBuffer = mSpcomServer->getSpcomSharedBuffer(mem);
        if (mSharedBuffer == nullptr) {
            ALOGE("Failed to get shared buffer");
            ret = SPCOM_MEMORY_ERROR;
        }
    });

    return ret;
}

int32_t SpcomServerTest::sendInvalidSharedBufferResponse() {

    ALOGD("Send invalid shared buffer response");

    hidl_vec<uint8_t> response(0);
    uint32_t offset = sizeof(void*);

    if(!mSharedBuffer) {
        ALOGE("Invalid shared buffer");
        return INVALID_PARAM;
    }

    int32_t ret = mSpcomServer->sendSharedBufferResponse(response,
        mSharedBuffer, offset);
    if(ret == SUCCESS) {
        ALOGE("Send shared buffer response with zero size should fail");
        return MESSAGING_ERROR;
    }

    return SUCCESS;
}

int32_t SpcomServerTest::sendSharedBufferResponse(hidl_vec<uint8_t> response,
    uint32_t offset) {

    ALOGD("Send shared buffer response");

    size_t respSz = response.size();
    if(respSz == 0) {
        ALOGE("Invalid response size");
        return INVALID_PARAM;
    }

    if(!mSharedBuffer) {
        ALOGE("Invalid shared buffer");
        return INVALID_PARAM;
    }

    if(offset > respSz - sizeof(void*)) {
        ALOGE("Invalid offset [%u], response size [%zu]", offset, respSz);
        return INVALID_PARAM;
    }

    // Send the message and the shared buffer address and size to SPU app
    int32_t ret = mSpcomServer->sendSharedBufferResponse(response,
        mSharedBuffer, offset);
    if(ret != SUCCESS) {
        return MESSAGING_ERROR;
    }

    return SUCCESS;
}

int32_t SpcomServerTest::handleSSR() {

    int32_t ret = SUCCESS;
    if(mSharedBuffer) {

        int32_t err = mSharedBuffer->copyFromSpu();
        int32_t ret = SUCCESS;
        if(err != 0) {
            ALOGE("Copy from SPU fails after SSR, err[%d]", err);
            ret = MESSAGING_ERROR;
        }

        err = mSharedBuffer->copyToSpu();
        if(err != 0) {
            ALOGE("Copy to SPU fails after SSR, err[%d]", err);
            ret = MESSAGING_ERROR;
        }

        // Release shared buffer
        ALOGD("Clear SPU shared buffer");
        mSharedBuffer.clear();
        if(mSharedBuffer) {
            ALOGE("Shared buffer wasn't released");
            ret = GENERAL_ERROR;
        }
    }

    // Unregister server
    if(mSpcomServer) {
        ALOGD("Unregister server");
        ret = unregisterServer();
        if(ret != SUCCESS) {
            ALOGE("Unregister server [%s] failed", mChannelName);
            ret = GENERAL_ERROR;
        }
    }

   return ret;
}
