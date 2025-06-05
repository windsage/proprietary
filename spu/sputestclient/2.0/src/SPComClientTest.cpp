/*!
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <vendor/qti/spu/2.0/ISPUManager.h>
#include <qti-utils.h>
#include <hidl/HidlSupport.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <chrono>
#include <cstring>
#include <thread>
#include <qti-utils.h>
#include "SPComClientTest.h"
#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"

using namespace std;
using chrono::seconds;
using this_thread::sleep_for;
using ::android::hardware::hidl_vec;
using ::android::hidl::allocator::V1_0::IAllocator;

SPComClientTest::SPComClientTest(const char* name) : chName(name)
{
    ALOGD("SPCom client create");
    chNameStr = chName.c_str();

    sp<ISPUManager> spuManager = SPUTestFw::getSPUManagerService();
    if (!spuManager)
        return;

    client = spuManager->getSPComClient(chNameStr);
    if (!client)
        return;

    isValid = true;
}

SPComClientTest::~SPComClientTest()
{
    ALOGD("SPCom client release");

    isValid = false;

    client.clear();
    if (client)
        ALOGE("Failed to release SPCom client");
}

int32_t SPComClientTest::openCloseChStress(size_t cycles)
{
    for (size_t i = 0; i < cycles; ++i) {

        // Register spcom channel
        int32_t err = client->registerClient();
        if (err) {
            ALOGE("Failed to register SPCom ch[%s], cycle[%zu], err[%d]", spuSrvAppName, i, err);
            return CHANNEL_ERROR;
        }

        // Unregister spcom channel
        err = client->unregisterClient();
        if (err) {
            ALOGE("Failed to unregister SPCom ch[%s], cycle[%zu], err[%d]", spuSrvAppName, i, err);
            return CHANNEL_ERROR;
        }
    }

    return SUCCESS;
}

int32_t SPComClientTest::registerClient()
{
    int32_t ret = client->registerClient();
    if (ret) {
        ALOGD("Failed to register client, err[%d]", ret);
        return CHANNEL_ERROR;
    }

    int connectRetries = 100;
    bool isConnected = client->isConnected();

    while (!isConnected) {
        sleep_for(seconds(1));
        isConnected = client->isConnected();
        --connectRetries;
        if (!connectRetries) {
            ALOGE("Client channel is not connected to remote edge");
            return RESOURCE_BUSY;
        }
    }

    return SUCCESS;
}

int32_t SPComClientTest::unregisterClient()
{
    uint32_t ret = client->unregisterClient();
    if (!ret)
        return SUCCESS;

    ALOGE("Failed to unregister client, err[%u]", ret);
    return CHANNEL_ERROR;
}

int32_t SPComClientTest::sendMessage(const hidl_vec<uint8_t> &request, uint8_t *expectedResponse,
    uint32_t timeoutMs)
{
    int32_t ret = SUCCESS;
    client->sendMessage(request, timeoutMs,
        [&](int32_t status, const hidl_vec<uint8_t> &response) {

        if (status <= 0 || !response.data()) {
            ALOGE("Failed to get response, err [%d]", status);
            ret = MESSAGING_ERROR;
            return;
        }

        ALOGD("Response [%.10s], size [%zu], rxBytes [%d]", response.data(), response.size(), status);

        if(memcmp(response.data(), expectedResponse, status)) {
            ALOGE("Unexpected response [%.10s], expected [%.10s]",
                response.data(), expectedResponse);
            ret = RESPONSE_ERROR;
            return;
        }
    });

    return ret;
}

int32_t SPComClientTest::sendModifiedMessage(const hidl_vec<uint8_t>& request, uint32_t offset,
    uint8_t* expectedResponse, uint32_t timeoutMs)
{
    ALOGD("Send shared buffer request: offset[%u], timeout[%u]", offset, timeoutMs);

    if (!sharedBuffer) {
        ALOGE("Invalid shared buffer");
        return SHARED_BUFFER_ERROR;
    }

    int32_t ret = SUCCESS;
    client->sendModifiedMessage(request, sharedBuffer, offset, timeoutMs,
        [&](int32_t status, const hidl_vec<uint8_t> &response) {

        if (status <= 0 || !response.data()) {
            ALOGE("Failed to get response, err: %d", status);
            ret = MESSAGING_ERROR;
            return;
        }

        ALOGD("Response: %.10s, response size: %zu, rxBytes: %d",
                response.data(), response.size(), status);

        if (strncmp((const char*)response.data(), (const char*)expectedResponse, status)) {
            ALOGE("Unexpected resp: %.10s, expected: %.10s", response.data(), expectedResponse);
            ret = RESPONSE_ERROR;
        }
    });

    return ret;
}

int32_t SPComClientTest::sendInvalidModifiedMessage()
{
    const hidl_vec<uint8_t> request(0);
    uint32_t offset = sizeof(void *);

    int32_t ret = SUCCESS;
    client->sendModifiedMessage(request, sharedBuffer, offset, defaultTimeotMs,
        [&](int32_t status, const hidl_vec<uint8_t> &response) {
        if (status > 0 || response.size() > 0) {
            ALOGE("Sending invalid modified message succeeded, status [%d]", status);
            ret = MESSAGING_ERROR;
        }
    });

    return ret;
}

int32_t SPComClientTest::getSPComSharedBuffer(uint32_t size)
{
    sp<IAllocator> ashmemAllocator = SPUTestFw::getIAllocatorService();
    if(!ashmemAllocator)
        return FRAMEWORK_ERROR;

    int ret = SUCCESS;
    ashmemAllocator->allocate(size, [&](bool success, const hidl_memory &mem) {

        // Check allocation status
        if (!success || !mem.valid()) {
            ALOGE("Failed to allocate shared memory");
            ret = HIDL_MEMORY_ERROR;
            return;
        }

        // Map shared memory
        sharedMemory = mapMemory(mem);
        if (!sharedMemory) {
            ALOGE("Failed to map shared memory");
            ret = GENERAL_ERROR;
            return;
        }

        // Create SpcomSharedBuffer
        sharedBuffer = client->getSPComSharedBuffer(mem);
        if (!sharedBuffer) {
            ALOGE("Failed to get shared buffer");
            ret = SPCOM_MEMORY_ERROR;
        }
    });

    return ret;;
}

int32_t SPComClientTest::sendCmdMessage(SPComClientCmdId cmdId, uint8_t *expectedResponse)
{
    ALOGD("Send cmd: [%d]", cmdId);

    if (cmdId >= SPComClientCmdId::LAST) {
        ALOGE("Invalid cmd ID [%d]", cmdId);
        return INVALID_PARAM;
    }

    // Copy message to hidl vector
    hidl_vec<uint8_t> msg(sizeof(SPComTestRequest));
    SPComTestRequest *msgData = (SPComTestRequest*)msg.data();

    msgData->cmdId = cmdId;

    int32_t ret = sendMessage(msg, expectedResponse, defaultTimeotMs);
    if(ret) {
        ALOGE("Failed sending client cmd[%u], err[%u]", cmdId, ret);
        return ret;
    }

    return SUCCESS;
}

int32_t SPComClientTest::sendPingWaitForPong()
{
    ALOGD("PING-PONG test");

    uint8_t expectedResp[] = { 'P', 'O', 'N', 'G', '\0' };
    if (sendCmdMessage(SPComClientCmdId::PING, expectedResp)) {
        ALOGE("PING-PONG test failed");
        return GENERAL_ERROR;
    }

    ALOGD("PING-PONG test done successfully");

    return SUCCESS;
}

int32_t SPComClientTest::sendInitExtmemCmd()
{
    SPComClientCmdId cmdId = SPComClientCmdId::SHARED_BUFFER_INIT;

    ALOGD("Send cmd: [%d]", cmdId);

    int32_t ret = getSPComSharedBuffer(pgSz);
    if (ret) {
        ALOGE("Failed to allocate spcom shared memory");
        return ret;
    }

    hidl_vec<uint8_t> msg(sizeof(SPComTestRequest));
    SPComTestRequest *msgData = (SPComTestRequest*)msg.data();
    msgData->cmdId = cmdId;
    msgData->sharedBufferAddr = 0;
    msgData->sharedBufferSz = pgSz;

    // Calculate the offset of the buffer address to be shared with SPU side
    size_t offset = (size_t)&msgData->sharedBufferAddr - (size_t)&msgData->cmdId;

    const char* expectedRespStr = "SHARED BUFFER INIT DONE";
    uint8_t expectedResp[maxSpcomMessageLength] = {0};
    size_t expectedResponseStrLength = strnlen(expectedRespStr, maxSpcomMessageLength);
    memscpy(expectedResp, sizeof(expectedResp), expectedRespStr, expectedResponseStrLength);

    if (sendModifiedMessage(msg, offset, expectedResp, defaultTimeotMs)) {
        ALOGE("Failed to send shared buffer request");
        return MESSAGING_ERROR;
    }

    return SUCCESS;
}

int32_t SPComClientTest::sendReadExtmemCmd(char expectedByte)
{
    // Create expected response buffer
    uint8_t expectedResp[maxSpcomMessageLength] = {0};
    memset(expectedResp, expectedByte, maxSpcomMessageLength - 1);

    // Send shared buffer read command to SPU side
    if (sendCmdMessage(SHARED_BUFFER_READ, expectedResp)) {
        ALOGE("SPU read [%c]s from shared buffer test failed", expectedByte);
        return GENERAL_ERROR;
    }

    sharedMemory->read();

    // Read 4 first byres from HIDL shared memory
    void *data = sharedMemory->getPointer();
    ALOGD("Read first 4 bytes directly from shared memory: %c%c%c%c",
        ((uint8_t*)data)[0], ((uint8_t*)data)[1], ((uint8_t*)data)[2], ((uint8_t*)data)[3]);

    sharedMemory->commit();

    return SUCCESS;
}

int32_t SPComClientTest::sendWriteExtmemCmd(char expectedByte)
{
    // Created expected response buffer
    uint8_t expectedResp[maxSpcomMessageLength] = {0};
    memset(expectedResp, expectedByte, maxSpcomMessageLength - 1);
    expectedResp[maxSpcomMessageLength - 1] = 0;

    // Send shared buffer write command to SPU side
    if (sendCmdMessage(SPComClientCmdId::SHARED_BUFFER_WRITE, expectedResp)) {
        ALOGE("SPU write [%c]s to shared buffer test failed", expectedByte);
        return GENERAL_ERROR;
    }

    // Sync shared buffer - will copy from dma buffer to hidl shared buffer
    // After sync the data in the ION buffer will be accessible to HIDL client
    sharedBuffer->copyFromSpu(0, pgSz);

    sharedMemory->read();

    // Read 4 first bytes from hidl shared memory - for debug
    void *data = sharedMemory->getPointer();
    ALOGD("Read first 4 bytes directly from shared memory: %c%c%c%c",
        ((uint8_t* )data)[0], ((uint8_t* )data)[1], ((uint8_t* )data)[2], ((uint8_t* )data)[3]);

    sharedMemory->commit();

    return SUCCESS;
}

int32_t SPComClientTest::writeToSharedBuff(const char *str, uint64_t nBytes)
{
    // Get shared memory data pointer
    void *data = sharedMemory->getPointer();
    if (data == nullptr) {
        ALOGE("Failed to obtain hidl shared memory data");
        return GENERAL_ERROR;
    }

    // Check in HIDL shared buffer for edit
    sharedMemory->update();

    // Write directly to HIDL shared buffer
    uint64_t ret = memscpy(data, pgSz, str, nBytes + 1);
    if (ret != nBytes + 1) // + 1 is for null terminator
        ALOGE("Failed to write all message to hidl shared memory, ret [%zu]", ret);

    // Commit HIDL shared buffer changes
    sharedMemory->commit();

    // Copy the changes from HIDL shared memory to DMA buffer so it will be available in SPU side
    sharedBuffer->copyToSpu(0, pgSz);

    ALOGD("Wrote [%.10s] to shared buffer", str);

    return SUCCESS;
}

int32_t SPComClientTest::readFromSharedBuff(uint64_t nBytes, char expectedByte)
{
    // Sync HIDL shared memory with DMA buffer content
    sharedBuffer->copyFromSpu(0, pgSz);

    // Get a pointer of HIDL shared memory
    void *data = sharedMemory->getPointer();
    if (!data)
        return GENERAL_ERROR;

    // Copy nBytes from HIDL shared buffer to read buffer and print to log
    uint8_t readBuff[maxSpcomMessageLength] = {0};
    memscpy(readBuff, maxSpcomMessageLength, data, nBytes);

    ALOGD("Read directly from shared buffer [%.10s]", readBuff);

    // Compare response to expected
    uint8_t expectedResp[maxSpcomMessageLength] = {0};
    memset(expectedResp, expectedByte, nBytes);

    if(memcmp((const char*)readBuff, (const char*)expectedResp, nBytes)) {
        ALOGE("Unexpected response: [%.10s], expected: [%.10s]", readBuff, expectedResp);
        return GENERAL_ERROR;
    }

    return SUCCESS;
}

int32_t SPComClientTest::sendReleaseExtmemCmd()
{
    const char* expectedRespStr = "SHARED BUFFER FREE DONE";
    uint8_t expectedResp[maxSpcomMessageLength] = {0};
    size_t expectedResponseStrLength = strnlen(expectedRespStr, maxSpcomMessageLength);
    memscpy(expectedResp, sizeof(expectedResp), expectedRespStr, expectedResponseStrLength);

    // Release buffer from SPU side
    if (sendCmdMessage(SPComClientCmdId::SHARED_BUFFER_RELEASE, expectedResp)) {
        ALOGE("SPU app failed to release buffer");
        return GENERAL_ERROR;
    }

    sharedMemory.clear();
    sharedBuffer.clear();

    return SUCCESS;
}


int32_t SPComClientTest::sendModifiedMessageFlow()
{
    // Init shared buffer and send it to SPU app, SPU app will open an extmem window
    ALOGD("***********INIT-SHARED-BUFFER-TEST************");
    int32_t ret = sendInitExtmemCmd();
    if (ret) {
        ALOGE("Failed to init shared buffer, err[%d]", ret);
        return ret;
    }

    // Invalid shared buffer size should fail
    ALOGD("**********SEND-INVALID-SHARED-BUFFER***********");
    ret = sendInvalidModifiedMessage();
    if (ret) {
        ALOGE("Succeeded to send invalid shared buffer, should fail");
        return ret;
    }

    // Write 'A's to shared buffer
    ALOGD("*********WRITE-A-TO-SHARED-BUFFER*************");
    char msg[maxSpcomMessageLength];
    memset(msg, 'A', maxSpcomMessageLength);
    ret = writeToSharedBuff(msg, maxSpcomMessageLength);
    if (ret) {
        ALOGE("Failed to write to shared buffer, err[%d]", ret);
        return ret;
    }

    // Send SPU app a command to read from shared buffer and expect all 'A's
    ALOGD("****SPU-READ-FROM-SHARED-BUFFER-EXPECT-A*****");
    ret = sendReadExtmemCmd('A');
    if (ret) {
        ALOGE("Failed to send read buffer cmd, err[%d]", ret);
        return ret;
    }

    // Send SPU app a command to write 'B's to shared buffer
    ALOGD("*******SPU-WRITE-B-TO-SHARED-BUFFER**********");
    ret = sendWriteExtmemCmd('B');
    if (ret != 0) {
        ALOGE("Failed to send write buffer cmd");
        return ret;
    }

    // Send SPU app a command to read from shared buffer and expect all 'B's
    ALOGD("****SPU-READ-FROM-SHARED-BUFFER-EXPECT-B*****");
    ret = sendReadExtmemCmd('B');
    if (ret) {
        ALOGE("Failed to send read buffer cmd");
        return ret;
    }

    // Read from shared buffer and expect all 'B's
    ALOGD("***READ-FROM-SHARED-BUFFER-EXPECT-B*********");
    uint64_t bytesToRead = maxSpcomMessageLength;
    ret = readFromSharedBuff(bytesToRead, 'B');
    if (ret) {
        ALOGE("Failed to read from shared buffer");
        return ret;
    }

    ret = sendReleaseExtmemCmd();
    if (ret) {
        ALOGE("Failed to release shared buff from spu side");
        return ret;
    }

    return SUCCESS;
}

int32_t SPComClientTest::handleSSR()
{
    int32_t ret = SUCCESS;

    if (sharedBuffer) {
        int32_t err = sharedBuffer->copyFromSpu(0, pgSz);
        if (err) {
            ALOGE("Copy from SPU fails after SSR, err[%d]", err);
            ret = MESSAGING_ERROR;
        }

        err = sharedBuffer->copyToSpu(0, pgSz);
        if(err) {
            ALOGE("Copy to SPU fails after SSR, err[%d]", err);
            ret = MESSAGING_ERROR;
        }

        sharedBuffer.clear();
        if (sharedBuffer) {
            ALOGE("Shared buffer wasn't released");
            ret = GENERAL_ERROR;
        }
    }

    if(client) {
        ret = unregisterClient();
        if (ret) {
            ALOGE("Unregister client [%s] failed", chNameStr);
            ret = GENERAL_ERROR;
        }
    }

    return ret;
}
