/*!
 *
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#pragma once

#include <android/hidl/memory/1.0/IMemory.h>
#include <vendor/qti/spu/1.0/ISpcomClient.h>
#include <stdint.h>
#include <utils/StrongPointer.h>

#include "log.h"

using namespace std;

using android::sp;
using vendor::qti::spu::V1_0::ISpcomClient;
using vendor::qti::spu::V1_0::ISpcomSharedBuffer;
using ::android::hardware::hidl_vec;
using ::android::hidl::memory::V1_0::IMemory;

struct SpcomClientTest {

public:

    explicit SpcomClientTest(const char* channelName);
    virtual ~SpcomClientTest();

    int32_t registerClient();
    int32_t unregisterClient();
    int32_t registerUnregisterClientStress(size_t cycles =
        sSpcomStressTestCycles);
    int32_t sendPingWaitForPong();
    int32_t sendSharedBufferMessageFlow();

    int32_t handleSSR();

    inline bool valid() { return mIsValid; }

private:

    enum SpcomClientCmdId : uint32_t {
        PING                  = 1,
        SHARED_BUFFER_INIT    = 2,
        SHARED_BUFFER_READ    = 3,
        SHARED_BUFFER_WRITE   = 4,
        SHARED_BUFFER_RELEASE = 5,
        LAST                  = 6,
    };

    struct SpcomTestRequest {
        uint32_t cmdId;
        uint64_t sharedBufferAddr;
        uint32_t sharedBufferSz;
    };

    char mChannelName[sMaxSpcomChannelNameLength];
    sp<ISpcomClient> mSpcomClient;
    sp<ISpcomSharedBuffer> mSharedBuffer;
    sp<IMemory> mHidlSharedMemory;
    bool mIsValid;

    int32_t sendMessage(const hidl_vec<uint8_t>& request,
         uint8_t* expectedResponse, uint32_t timeoutMs);
    int32_t sendSharedBufferMessage(const hidl_vec<uint8_t>& request,
        uint32_t offset, uint8_t* expectedResponse, uint32_t timeoutMs);
    int32_t sendInvalidSharedBufferMessage();
    int32_t getSpcomSharedBuffer(uint32_t nBytes);

    int32_t sendCmdMessage(SpcomClientCmdId cmdId,
        uint8_t* expectedResponse);
    int32_t sendInitSharedBuffCmd();
    int32_t sendReadSharedBuffCmd(char expectedByte);
    int32_t sendWriteSharedBuffCmd(char expectedByte);
    int32_t writeToSharedBuff(const char *str, uint64_t nBytes);
    int32_t readFromSharedBuff(uint64_t nBytes, char expectedByte);
    int32_t sendReleaseSharedBuffCmd();
};
