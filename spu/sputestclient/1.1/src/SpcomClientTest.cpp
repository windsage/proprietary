/*!
 *
 * Copyright (c) 2019-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <vendor/qti/spu/1.1/ISPUManager.h>
#include <qti-utils.h>
#include <hidl/HidlSupport.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <chrono>
#include <cstring>
#include <thread>
#include <qti-utils.h>

#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"
#include "SpcomClientTest.h"

using namespace std;

using ::android::hardware::hidl_vec;
using ::android::hidl::allocator::V1_0::IAllocator;

SpcomClientTest::SpcomClientTest(const char* channelName) :
    mSpcomClient(nullptr),
    mSharedBuffer(nullptr),
    mHidlSharedMemory(nullptr),
    mIsValid(false) {

    if(channelName == nullptr) {
        ALOGE("Invalid channel Name for spcom client");
        return;
    }

    uint32_t chNameLen = strnlen(channelName, sMaxSpcomMessageLength);
    if(chNameLen == sMaxSpcomMessageLength) {
        ALOGE("ChannelName is too long");
        return;
    }

    ALOGD("Init spcom client [%s]", channelName);

    memset(mChannelName, 0x00, sizeof(mChannelName));
    memscpy(mChannelName, sizeof(mChannelName), channelName, chNameLen);

    sp<ISPUManager> spuManager = SPUTestFw::getSPUManagerService();
    if(!spuManager) {
        ALOGE("Framework is not initialized");
        return;
    }

    ALOGD("Register ch name: %s", channelName);

    mSpcomClient = spuManager->getSpcomClient(channelName);
    if(!mSpcomClient) {
        ALOGE("Failed to get SpcomClient obj");
        return;
    }

    mIsValid = true;
}

SpcomClientTest::~SpcomClientTest() {

    ALOGD("Spcom client [%s] cleanup", mChannelName);

    mIsValid = false;

    mSpcomClient.clear();
    if(mSpcomClient) {
        ALOGE("Failed to release spcom client");
    }

    memset(mChannelName, 0x00, sMaxSpcomChannelNameLength);
}

int32_t SpcomClientTest::registerUnregisterClientStress(size_t cycles) {

    for(size_t i = 0; i < cycles; ++i) {

        // Register spcom channel
        int32_t err = mSpcomClient->registerClient(false);
        if(err != SUCCESS) {
            ALOGE("Failed to register spcom channel[%s], cycle[%zu], err[%d]",
                sSpcomServerAppChannel, i, err);
            return CHANNEL_ERROR;
        }

        // Unregister spcom channel
        err = mSpcomClient->unregisterClient();
        if(err != SUCCESS) {
            ALOGE("Failed to unregister spcom channel[%s], cycle[%zu], err[%d]",
                sSpcomServerAppChannel, i, err);
            return CHANNEL_ERROR;
        }
    }

    return SUCCESS;
}

int32_t SpcomClientTest::registerClient() {

    ALOGD("Register client was called [%s]", mChannelName);

    int32_t ret = mSpcomClient->registerClient(false);
    if(ret != 0) {
        ALOGD("Failed to register client, err[%d]", ret);
        return CHANNEL_ERROR;
    }

    ALOGD("Registered client successfully, channel [%s]", mChannelName);

    int connectRetries = sConnectionRetries;
    bool isConnected = mSpcomClient->isConnected();

    while (isConnected == false) {

        ALOGD("Wait for channel connectivity [%s] - Retries left [%d]",
                mChannelName, connectRetries);

        this_thread::sleep_for(chrono::seconds(1));

        if (--connectRetries == 0) {
            ALOGE("No connection on channel [%s]", mChannelName);
            return RESOURCE_BUSY;
        }

        isConnected = mSpcomClient->isConnected();
    }

    ALOGD("Client is now connected to client [%s]", mChannelName);

    return SUCCESS;
}

int32_t SpcomClientTest::unregisterClient() {

    ALOGD("Unregister client [%s] was called", mChannelName);

    uint32_t ret = mSpcomClient->unregisterClient();
    if (ret != 0) {
        ALOGE("Failed to unregister client, err[%u]", ret);
        return CHANNEL_ERROR;
    }

    return SUCCESS;
}

int32_t SpcomClientTest::sendMessage(const hidl_vec<uint8_t>& request,
    uint8_t* expectedResponse, uint32_t timeoutMs) {

    ALOGD("Spcom client send message");

    int32_t ret = SUCCESS;
    mSpcomClient->sendMessage(request, timeoutMs,
        [&](int32_t rxBytes, const hidl_vec<uint8_t> &response) {

        if (rxBytes <= 0 || response.data() == nullptr) {
            ALOGE("Failed to get response, err: %d", rxBytes);
            ret = MESSAGING_ERROR;
            return;
        }

        ALOGD("Response: %.10s, response size: %zu, rxBytes: %d",
            response.data(), response.size(), rxBytes);

        if(strncmp((const char*)response.data(),
            (const char*)expectedResponse, rxBytes) != 0) {
            ALOGE("Unexpected response: %.10s, expected: %.10s",
                response.data(), expectedResponse);
            ret = RESPONSE_ERROR;
        }
    });

    return ret;
}

int32_t SpcomClientTest::sendSharedBufferMessage(
    const hidl_vec<uint8_t>& request, uint32_t offset,
    uint8_t* expectedResponse, uint32_t timeoutMs) {

    ALOGD("Send shared buffer request: offset[%u], timeout[%u]",
        offset, timeoutMs);

    if(!mSharedBuffer) {
        ALOGE("Invalid shared buffer");
        return SHARED_BUFFER_ERROR;
    }

    int32_t ret = SUCCESS;
    mSpcomClient->sendSharedBufferMessage(request, mSharedBuffer, offset,
        timeoutMs,
        [&](int32_t rxBytes, const hidl_vec<uint8_t> &response) {

        if (rxBytes <= 0 || response.data() == nullptr) {
            ALOGE("Failed to get response, err: %d", rxBytes);
            ret = MESSAGING_ERROR;
            return;
        }

        ALOGD("Response: %.10s, response size: %zu, rxBytes: %d",
                response.data(), response.size(), rxBytes);

        if (strncmp((const char*)response.data(),
            (const char*)expectedResponse, rxBytes) != 0) {
            ALOGE("Unexpected response: %.10s, expected: %.10s",
                response.data(), expectedResponse);
            ret = RESPONSE_ERROR;
        }
    });

    return ret;
}

int32_t SpcomClientTest::sendInvalidSharedBufferMessage() {

    ALOGD("Send invalid shared buffer request");

    if(!mSharedBuffer) {
        ALOGE("Invalid shared buffer");
        return SHARED_BUFFER_ERROR;
    }

    const hidl_vec<uint8_t> request(0);
    uint32_t offset = sizeof(void*);

    int32_t ret = SUCCESS;
    mSpcomClient->sendSharedBufferMessage(request, mSharedBuffer, offset,
        sDefaultTimeotMs,
        [&](int32_t rxBytes, const hidl_vec<uint8_t> &response) {
        if (rxBytes > 0 || response.size() > 0) {
            ALOGE("Invalid send shared buffer didn't fail, rx[%d], size[%ld]",
                rxBytes, response.size());
            ret = MESSAGING_ERROR;
        }
    });

    return ret;
}

int32_t SpcomClientTest::getSpcomSharedBuffer(uint32_t nBytes) {

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
        mSharedBuffer = mSpcomClient->getSpcomSharedBuffer(mem);
        if (mSharedBuffer == nullptr) {
            ALOGE("Failed to get shared buffer");
            ret = SPCOM_MEMORY_ERROR;
        }
    });

    return ret;;
}

int32_t SpcomClientTest::sendCmdMessage(SpcomClientCmdId cmdId, uint8_t* expectedResponse) {

    ALOGD("Send cmd: [%d]", cmdId);

    if (cmdId >= SpcomClientCmdId::LAST) {
        ALOGE("Invalid cmd ID [%d]", cmdId);
        return INVALID_PARAM;
    }

    // Copy message to hidl vector
    hidl_vec<uint8_t> msg(sizeof(SpcomTestRequest));
    SpcomTestRequest* msgData = (SpcomTestRequest*)msg.data();

    msgData->cmdId = cmdId;

    int32_t ret = sendMessage(msg, expectedResponse, sDefaultTimeotMs);
    if(ret != 0) {
        ALOGE("Failed sending client cmd[%u], err[%u]", cmdId, ret);
        return ret;
    }

    return SUCCESS;
}

int32_t SpcomClientTest::sendPingWaitForPong() {

    ALOGD("PING-PONG test");

    uint8_t expectedResp[] = { 'P', 'O', 'N', 'G', '\0' };
    if (sendCmdMessage(SpcomClientCmdId::PING, expectedResp) != SUCCESS) {
        ALOGE("PING-PONG test failed");
        return GENERAL_ERROR;
    }

    ALOGD("PING-PONG test done successfully");

    return SUCCESS;
}

int32_t SpcomClientTest::sendInitSharedBuffCmd() {

    SpcomClientCmdId cmdId = SpcomClientCmdId::SHARED_BUFFER_INIT;

    ALOGD("Send cmd: [%d]", cmdId);

    int32_t ret = getSpcomSharedBuffer(sPgSz);
    if(ret != SUCCESS) {
        ALOGE("Failed to allocate spcom shared memory");
        return ret;
    }

    hidl_vec<uint8_t> msg(sizeof(SpcomTestRequest));
    SpcomTestRequest* msgData = (SpcomTestRequest*)msg.data();
    msgData->cmdId = cmdId;
    msgData->sharedBufferAddr = 0;
    msgData->sharedBufferSz = sPgSz;

    // Calculate the offset of the buffer address to be shared with SPU side
    size_t offset = (size_t) &msgData->sharedBufferAddr - (size_t) &msgData->cmdId;

    const char* expectedRespStr = "SHARED BUFFER INIT DONE";
    uint8_t expectedResp[sMaxSpcomMessageLength] = {0};
    size_t expectedResponseStrLength = strnlen(expectedRespStr,
        sMaxSpcomMessageLength);
    memscpy(expectedResp, sizeof(expectedResp), expectedRespStr,
        expectedResponseStrLength);

    if(sendSharedBufferMessage(msg, offset, expectedResp, sDefaultTimeotMs) != SUCCESS) {
        ALOGE("Failed to send shared buffer request");
        return MESSAGING_ERROR;
    }

    return SUCCESS;
}

// Ask SPU app to read from shared buffer and share the content
// Verify the response and also the shared buffer content
int32_t SpcomClientTest::sendReadSharedBuffCmd(char expectedByte) {

    // Create expected response buffer
    uint8_t expectedResp[sMaxSpcomMessageLength] = { 0 };
    memset(expectedResp, expectedByte, sMaxSpcomMessageLength - 1);

    // Send shared buffer read command to SPU side
    if (sendCmdMessage(SHARED_BUFFER_READ, expectedResp) != SUCCESS) {
        ALOGE("SPU read [%c]s from shared buffer test failed", expectedByte);
        return GENERAL_ERROR;
    }

    mHidlSharedMemory->read();

    // Read 4 first byres from HIDL shared memory
    void *data = mHidlSharedMemory->getPointer();
    ALOGD("Read first 4 bytes directly from shared memory: %c%c%c%c",
        ((uint8_t*)data)[0], ((uint8_t*)data)[1], ((uint8_t*)data)[2],
        ((uint8_t*)data)[3]);

    mHidlSharedMemory->commit();

    return SUCCESS;
}

// Ask SPU app to write to the shared buffer and verify the response
int32_t SpcomClientTest::sendWriteSharedBuffCmd(char expectedByte) {

    // Created expected response buffer
    uint8_t expectedResp[sMaxSpcomMessageLength] = { 0 };
    memset(expectedResp, expectedByte, sMaxSpcomMessageLength - 1);
    expectedResp[sMaxSpcomMessageLength - 1] = 0x00;

    // Send shared buffer write command to SPU side
    if (sendCmdMessage(SpcomClientCmdId::SHARED_BUFFER_WRITE, expectedResp) != SUCCESS) {
        ALOGE("SPU write [%c]s to shared buffer test failed", expectedByte);
        return GENERAL_ERROR;
    }

    // Sync shared buffer - will copy from ION buffer to hidl shared buffer
    // After sync the data in the ION buffer will be accessible to HIDL client
    mSharedBuffer->copyFromSpu();

    mHidlSharedMemory->read();

    // Read 4 first bytes from hidl shared memory - for debug
    void *data = mHidlSharedMemory->getPointer();
    ALOGD("Read first 4 bytes directly from shared memory: %c%c%c%c",
            ((uint8_t* )data)[0], ((uint8_t* )data)[1], ((uint8_t* )data)[2],
            ((uint8_t* )data)[3]);

    mHidlSharedMemory->commit();

    return SUCCESS;
}

int32_t SpcomClientTest::writeToSharedBuff(const char *str, uint64_t nBytes) {

    // Get pointer of HIDL shared memory
    void *data = mHidlSharedMemory->getPointer();
    if (data == nullptr) {
        ALOGE("Failed to obtain hidl shared memory data");
        return GENERAL_ERROR;
    }

    // Check in HIDL shared buffer for edit
    mHidlSharedMemory->update();

    // Write directly to HIDL shared buffer
    uint64_t ret = memscpy(data, sPgSz, str, nBytes + 1);
    if (ret != nBytes + 1) { // + 1 is for null terminator
        ALOGE("Failed to write all message to hidl shared memory, ret [%zu]",
                ret);
    }

    // Commit changes to HIDL shared buffer
    // This is still not obtained in SPU app side
    mHidlSharedMemory->commit();

    // Flush will make the HIDL server copy the changes from hidl shared memory to
    // ION buffer and will be available is SPU app side
    mSharedBuffer->copyToSpu();

    ALOGD("Wrote %.10s to shared buffer", str);

    return SUCCESS;
}

int32_t SpcomClientTest::readFromSharedBuff(uint64_t nBytes, char expectedByte) {

    // Sync HIDL shared memory with ION buffer content (sync the double buffer)
    mSharedBuffer->copyFromSpu();

    // Get pointer of HIDL shared memory
    void *data = mHidlSharedMemory->getPointer();
    if (data == nullptr) {
        ALOGE("Failed to obtain HIDL shared memory data");
        return GENERAL_ERROR;
    }

    // Copy nBytes from HIDL shared buffer to read buffer and print to log
    uint8_t readBuff[sMaxSpcomMessageLength] = {0};
    memscpy(readBuff, sMaxSpcomMessageLength - 1, data, nBytes);
    ALOGD("Read directly from shared buffer [%.10s]", readBuff);

    // Compare response to expected
    uint8_t expectedResp[sMaxSpcomMessageLength] = {0};
    memset(expectedResp, expectedByte, sMaxSpcomMessageLength - 1);

    if(strncmp((const char*)readBuff, (const char*)expectedResp, nBytes)
        != 0) {
        ALOGE("Unexpected response: %.10s, expected: %.10s", readBuff,
            expectedResp);
        return GENERAL_ERROR;
    }

    return SUCCESS;
}

int32_t SpcomClientTest::sendReleaseSharedBuffCmd() {

    const char* expectedRespStr = "SHARED BUFFER FREE DONE";
    uint8_t expectedResp[sMaxSpcomMessageLength] = {0};
    size_t expectedResponseStrLength = strnlen(expectedRespStr,
        sMaxSpcomMessageLength);
    memscpy(expectedResp, sizeof(expectedResp), expectedRespStr,
        expectedResponseStrLength);

    // Release buffer from SPU side
    if (sendCmdMessage(SpcomClientCmdId::SHARED_BUFFER_RELEASE, expectedResp) != 0) {
        ALOGE("SPU app failed to release buffer");
        return GENERAL_ERROR;
    }

    mHidlSharedMemory.clear();
    mSharedBuffer.clear();

    return SUCCESS;
}


int32_t SpcomClientTest::sendSharedBufferMessageFlow() {

    // Init shared buffer and send it to SPU app
    // SPU app will open extmem window
    ALOGD("***********INIT-SHARED-BUFFER-TEST************");

    int32_t ret = sendInitSharedBuffCmd();
    if (ret != 0) {
        ALOGE("Failed to init shared buffer, err[%d]", ret);
        return ret;
    }

    // Invalid shared buffer size should fail
    ALOGD("**********SEND-INVALID-SHARED-BUFFER***********");
    ret = sendInvalidSharedBufferMessage();
    if (ret != 0) {
        ALOGE("Succeeded to send invalid shared buffer, should fail");
        return ret;
    }

    // Write 'A's to shared buffer
    ALOGD("*********WRITE-A-TO-SHARED-BUFFER*************");

    char msg[sMaxSpcomMessageLength];
    memset(msg, 'A', sMaxSpcomMessageLength);
    ret = writeToSharedBuff(msg, sMaxSpcomMessageLength);
    if (ret != 0) {
        ALOGE("Failed to write to shared buffer, err[%d]", ret);
        return ret;
    }

    // Send SPU app a command to read from shared buffer and expect all 'A's
    ALOGD("****SPU-READ-FROM-SHARED-BUFFER-EXPECT-A*****");

    ret = sendReadSharedBuffCmd('A');
    if (ret != 0) {
        ALOGE("Failed to send read buffer cmd, err[%d]", ret);
        return ret;
    }

    // Send SPU app a command to write 'B's to shared buffer
    ALOGD("*******SPU-WRITE-B-TO-SHARED-BUFFER**********");

    ret = sendWriteSharedBuffCmd('B');
    if (ret != 0) {
        ALOGE("Failed to send write buffer cmd");
        return ret;
    }

    // Send SPU app a command to read from shared buffer and expect all 'B's
    ALOGD("****SPU-READ-FROM-SHARED-BUFFER-EXPECT-B*****");

    ret = sendReadSharedBuffCmd('B');
    if (ret != 0) {
        ALOGE("Failed to send read buffer cmd");
        return ret;
    }

    // Read from shared buffer and expect all 'B's
    ALOGD("***READ-FROM-SHARED-BUFFER-EXPECT-B*********");

    uint64_t bytesToRead = sMaxSpcomMessageLength;
    ret = readFromSharedBuff(bytesToRead, 'B');
    if (ret != 0) {
        ALOGE("Failed to read from shared buffer");
        return ret;
    }

    ret = sendReleaseSharedBuffCmd();
    if(ret != 0) {
        ALOGE("Failed to release shared buff from spu side");
        return ret;
    }

    return SUCCESS;
}

int32_t SpcomClientTest::handleSSR() {

    int32_t ret = SUCCESS;

    if(mSharedBuffer) {

        int32_t err = mSharedBuffer->copyFromSpu();
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

    // Unregister client
    if(mSpcomClient) {
        ALOGD("Unregister client");
        ret = unregisterClient();
        if(ret != SUCCESS) {
            ALOGE("Unregister client [%s] failed", mChannelName);
            ret = GENERAL_ERROR;
        }
    }

    return ret;
}
