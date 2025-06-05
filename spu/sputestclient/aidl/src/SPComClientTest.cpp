/*!
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/hardware/spu/BnSPUManager.h>
#include <qti-utils.h>
#include <chrono>
#include <cstring>
#include <thread>
#include <cutils/ashmem.h>
#include "SPComClientTest.h"
#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>


using namespace std;
using chrono::seconds;
using this_thread::sleep_for;
using ::ndk::ScopedAStatus;
using aidl::vendor::qti::hardware::spu::ISPComClient;
using aidl::android::hardware::common::Ashmem;

// import android.os.ParcelFileDescriptor;


SPComClientTest::SPComClientTest(const char* name) : chName(name)
{
    ScopedAStatus status = ScopedAStatus::ok();
    ALOGD("SPCom client create");
    chNameStr = chName.c_str();

    std::shared_ptr<ISPUManager> spuManager = SPUTestFw::getSPUManagerService();
    if (!spuManager)
        return;

    status = spuManager->getSPComClient(chNameStr, &client);
    if (!client)
        return;

    isValid = true;
}

SPComClientTest::~SPComClientTest()
{
    ALOGD("SPCom client release");

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

    client.reset();
    if (client)
        ALOGE("Failed to release SPCom client");
}

int32_t SPComClientTest::openCloseChStress(size_t cycles)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t err = SUCCESS;

    for (size_t i = 0; i < cycles; ++i) {

        // Register spcom channel
        status = client->registerClient(&err);
        if (err || !status.isOk()) {
            ALOGE("Failed to register SPCom ch[%s], cycle[%zu], err[%d]", spuSrvAppName, i, err);
            return CHANNEL_ERROR;
        }

        // Unregister spcom channel
        status = client->unregisterClient(&err);
        if (err || !status.isOk()) {
            ALOGE("Failed to unregister SPCom ch[%s], cycle[%zu], err[%d]", spuSrvAppName, i, err);
            return CHANNEL_ERROR;
        }
    }

    return SUCCESS;
}

int32_t SPComClientTest::registerClient()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    bool isConnected;

    status = client->registerClient(&ret);
    if (ret || !status.isOk()) {
        ALOGD("Failed to register client, err[%d]", ret);
        return CHANNEL_ERROR;
    }

    int connectRetries = 100;
    status = client->isConnected(&isConnected);
    if (!status.isOk()) {
        ALOGD("Failed to check connectivity");
        return CHANNEL_ERROR;
    }

    while (!isConnected && status.isOk()) {
        sleep_for(seconds(1));
        status = client->isConnected(&isConnected);
        --connectRetries;
        if (!connectRetries) {
            ALOGE("Client channel is not connected to remote edge");
            return RESOURCE_BUSY;
        }
        if (!status.isOk()){
            ALOGD("Failed to check connectivity");
            return CHANNEL_ERROR;
        }
    }

    return SUCCESS;
}

int32_t SPComClientTest::unregisterClient()
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    status = client->unregisterClient(&ret);
    if (!ret && status.isOk())
        return SUCCESS;

    ALOGE("Failed to unregister client, err[%u]", ret);
    return CHANNEL_ERROR;
}

int32_t SPComClientTest::sendMessage(const std::vector<uint8_t> &request, uint8_t *expectedResponse,
    uint32_t timeoutMs)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    ISPComClient::data_st response;

    response.size = 0;

    status = client->sendMessage(request, timeoutMs,&response, &ret);
    if (!status.isOk() || SUCCESS != ret || 0 == response.size) {
        ALOGE("Failed to get response, err [%d]", ret);
        ret = MESSAGING_ERROR;
        return ret;
    }

    ALOGD("Response [%.10s], size [%zu], rxBytes [%d]",
            response.data.data(), response.data.size(), response.size);

    if(memcmp(response.data.data(), expectedResponse, response.size)) {
        ALOGE("Unexpected response [%.10s], expected [%.10s]",
              response.data.data(), expectedResponse);
        ret = RESPONSE_ERROR;
        return ret;
    }

    return ret;
}

int32_t SPComClientTest::sendModifiedMessage(const std::vector<uint8_t>& request, uint32_t offset,
    uint8_t* expectedResponse, uint32_t timeoutMs)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    ISPComClient::data_st response;

    response.size = 0;

    ALOGD("Send shared buffer request: offset[%u], timeout[%u]", offset, timeoutMs);

    if (!sharedBuffer) {
        ALOGE("Invalid shared buffer");
        return SHARED_BUFFER_ERROR;
    }

    status = client->sendModifiedMessage(request, sharedBuffer, offset, timeoutMs, &response, &ret);
    if (!status.isOk() || SUCCESS != ret || response.size == 0) {
        ALOGE("Failed to get response, err: %d", ret);
        return MESSAGING_ERROR;
    }

    ALOGD("Response: %.10s, response size: %zu, rxBytes: %d",
           response.data.data(), response.data.size(), response.size);

    if (strncmp((const char*)response.data.data(), (const char*)expectedResponse, response.size)) {
        ALOGE("Unexpected resp: %.10s, expected: %.10s", response.data.data(), expectedResponse);
        ret = RESPONSE_ERROR;
    }

    return ret;
}

int32_t SPComClientTest::sendInvalidModifiedMessage()
{
    const std::vector<uint8_t> request(0);
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    ISPComClient::data_st response;
    uint32_t offset = sizeof(void *);

    response.size = 0;
    status = client->sendModifiedMessage(request, sharedBuffer, offset, defaultTimeotMs, &response, &ret);
    if (SUCCESS == ret || response.size > 0) {
        ALOGE("Sending invalid modified message succeeded, status [%d]", ret);
        return MESSAGING_ERROR;
    }

    if (status.isOk() && ret == SUCCESS){
        ALOGE("status %d %d. failed sending invalid message", status.isOk(), ret);
        return FRAMEWORK_ERROR;
    }

    return SUCCESS;
}

int32_t SPComClientTest::getSPComSharedBuffer(uint32_t size)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    int32_t fd = -1;

    ALOGD("Get shared buffer with size of [%u] bytes", size);
    mem.fd.set(ashmem_create_region(nullptr, size));

    fd = mem.fd.get();
    if (fd == -1) {
        ALOGE("failed to allocate memory");
        return HAL_MEMORY_ERROR;
    }

    mem.size = size;

    // Map HAL memory
    sharedMemory = mmap(nullptr, mem.size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == nullptr || sharedMemory == (void*) -1) {
        ALOGE("Failed to map HAL memory");
        ret = GENERAL_ERROR;
    }

    // Create SpcomSharedBuffer
    status = client->getSPComSharedBuffer(mem, &sharedBuffer);
    if (!status.isOk() || sharedBuffer == nullptr) {
        ALOGE("Failed to get shared buffer");
        ret = SPCOM_MEMORY_ERROR;
    }

    return ret;

}

int32_t SPComClientTest::releaseSPComSharedBuffer()
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

int32_t SPComClientTest::sendCmdMessage(SPComClientCmdId cmdId, uint8_t *expectedResponse)
{
    ALOGD("Send cmd: [%d]", cmdId);

    if (cmdId >= SPComClientCmdId::LAST) {
        ALOGE("Invalid cmd ID [%d]", cmdId);
        return INVALID_PARAM;
    }

    // Copy message to hidl vector
    std::vector<uint8_t> msg(sizeof(SPComTestRequest));
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

    std::vector<uint8_t> msg(sizeof(SPComTestRequest));
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

    // Read 4 first byres from HIDL shared memory
    void *data = sharedMemory;
    ALOGD("Read first 4 bytes directly from shared memory: %c%c%c%c",
        ((uint8_t*)data)[0], ((uint8_t*)data)[1], ((uint8_t*)data)[2], ((uint8_t*)data)[3]);

    return SUCCESS;
}

int32_t SPComClientTest::sendWriteExtmemCmd(char expectedByte)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

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
    status = sharedBuffer->copyFromSpu(0, pgSz, &ret);
    if (!status.isOk()) {
        ALOGE("Failed to copy from SPU");
        return SHARED_BUFFER_ERROR;
    }

    // Read 4 first bytes from hidl shared memory - for debug
    void *data = sharedMemory;
    ALOGD("Read first 4 bytes directly from shared memory: %c%c%c%c",
        ((uint8_t* )data)[0], ((uint8_t* )data)[1], ((uint8_t* )data)[2], ((uint8_t* )data)[3]);

    return SUCCESS;
}

int32_t SPComClientTest::writeToSharedBuff(const char *str, uint64_t nBytes)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t err = SUCCESS;

    // Get shared memory data pointer
    void *data = sharedMemory;
    if (data == nullptr) {
        ALOGE("Failed to obtain hidl shared memory data");
        return GENERAL_ERROR;
    }

    // Write directly to HAL shared buffer
    uint64_t ret = memscpy(data, pgSz, str, nBytes + 1);
    if (ret != nBytes + 1) // + 1 is for null terminator
        ALOGE("Failed to write all message to hidl shared memory, ret [%zu]", ret);

    // Copy the changes from HIDL shared memory to DMA buffer so it will be available in SPU side
    status = sharedBuffer->copyToSpu(0, pgSz, &err);
    if (!status.isOk() || err) {
        ALOGE("Failed to copy to SPU");
        return SHARED_BUFFER_ERROR;
    }

    ALOGD("Wrote [%.10s] to shared buffer", str);

    return SUCCESS;
}

int32_t SPComClientTest::readFromSharedBuff(uint64_t nBytes, char expectedByte)
{
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;

    // Sync HAL shared memory with DMA buffer content
    status = sharedBuffer->copyFromSpu(0, pgSz, &ret);
    if (!status.isOk()) {
        ALOGE("Failed to from SPU");
        return SHARED_BUFFER_ERROR;
    }

    // Get a pointer of HIDL shared memory
    void *data = sharedMemory;
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


    if (releaseSPComSharedBuffer()){
        return RESOURCE_BUSY;
    };

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
    ScopedAStatus status = ScopedAStatus::ok();
    int32_t ret = SUCCESS;
    int32_t err = SUCCESS;

    if (sharedBuffer) {
        status = sharedBuffer->copyFromSpu(0, pgSz, &err);
        if (err || !status.isOk()) {
            ALOGE("Copy from SPU fails after SSR, err[%d]", err);
            ret = MESSAGING_ERROR;
        }

        status = sharedBuffer->copyToSpu(0, pgSz, &err);
        if(err || !status.isOk()) {
            ALOGE("Copy to SPU fails after SSR, err[%d]", err);
            ret = MESSAGING_ERROR;
        }

        sharedBuffer.reset();
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
