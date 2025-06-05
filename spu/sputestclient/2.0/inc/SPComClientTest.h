/*!
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <android/hidl/memory/1.0/IMemory.h>
#include <vendor/qti/spu/2.0/ISPComClient.h>
#include <stdint.h>
#include <utils/StrongPointer.h>
#include "SPUTestDefinitions.h"
#include "log.h"

using namespace std;
using android::sp;
using vendor::qti::spu::V2_0::ISPComClient;
using vendor::qti::spu::V2_0::ISPComSharedBuffer;
using ::android::hardware::hidl_vec;
using ::android::hidl::memory::V1_0::IMemory;

struct SPComClientTest {

public:

    explicit SPComClientTest(const char* channelName);
    virtual ~SPComClientTest();

    int32_t registerClient();
    int32_t unregisterClient();
    int32_t openCloseChStress(size_t cycles = testCycles);
    int32_t sendPingWaitForPong();
    int32_t sendModifiedMessageFlow();
    int32_t handleSSR();
    inline bool valid() { return isValid; }

private:
    enum SPComClientCmdId : uint32_t {
        PING                  = 1,
        SHARED_BUFFER_INIT    = 2,
        SHARED_BUFFER_READ    = 3,
        SHARED_BUFFER_WRITE   = 4,
        SHARED_BUFFER_RELEASE = 5,
        LAST                  = 6,
    };

    struct SPComTestRequest {
        uint32_t cmdId;
        uint64_t sharedBufferAddr;
        uint32_t sharedBufferSz;
    };

    string chName;
    const char *chNameStr = nullptr;
    sp<ISPComClient> client = nullptr;
    sp<ISPComSharedBuffer> sharedBuffer = nullptr;
    sp<IMemory> sharedMemory = nullptr;
    bool isValid = false;

    int32_t sendMessage(const hidl_vec<uint8_t> &request, uint8_t *expectedResponse, uint32_t timeoutMs);
    int32_t sendModifiedMessage(const hidl_vec<uint8_t> &request, uint32_t offset, uint8_t *expectedResponse, uint32_t timeoutMs);
    int32_t sendInvalidModifiedMessage();
    int32_t getSPComSharedBuffer(uint32_t nBytes);
    int32_t sendCmdMessage(SPComClientCmdId cmdId, uint8_t *expectedResponse);
    int32_t sendInitExtmemCmd();
    int32_t sendReadExtmemCmd(char expectedByte);
    int32_t sendWriteExtmemCmd(char expectedByte);
    int32_t writeToSharedBuff(const char *str, uint64_t nBytes);
    int32_t readFromSharedBuff(uint64_t nBytes, char expectedByte);
    int32_t sendReleaseExtmemCmd();
};
