/*!
 *
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#pragma once

#include <vendor/qti/spu/1.1/ISpcomServer.h>
#include <stdint.h>
#include <utils/StrongPointer.h>

#include "log.h"
#include "SPUTestDefinitions.h"

using namespace std;

using vendor::qti::spu::V1_1::ISpcomServer;
using vendor::qti::spu::V1_0::ISpcomSharedBuffer;
using android::sp;
using ::android::hardware::hidl_vec;

class SpcomServerTest {

public:

    explicit SpcomServerTest(const char* channelName);
    virtual ~SpcomServerTest();

    int32_t registerUnregisterServerStress(size_t cycles =
        sSpcomStressTestCycles);
    int32_t registerServer();
    int32_t unregisterServer();

    int32_t waitForPingSendPong();
    int32_t sendSharedBufferResponseFlow();
    int32_t sendInvalidSharedBufferResponse();

    int32_t handleSSR();

    inline bool valid() { return mIsValid; }

private:

    enum SpcomServerRequestId : uint32_t {
        PING                  = 1,
        SHARED_BUFFER_REQUEST = 2,
        SHARED_BUFFER_READ    = 3,
        SHARED_BUFFER_WRITE   = 4,
        SHARED_BUFFER_RELEASE = 5,
        LAST                  = 6,
    };

    struct SharedBufferResponseStruct {
        uint64_t sharedBufferAddr;
        uint32_t sharedBufferSz;
    };

    char mChannelName[sMaxSpcomChannelNameLength];
    sp<ISpcomServer> mSpcomServer;
    sp<ISpcomSharedBuffer> mSharedBuffer;
    sp<IMemory> mHidlSharedMemory;
    bool mIsValid;

    int32_t waitForNextRequest(SpcomServerRequestId expectedRequest);
    int32_t sendResponse(hidl_vec<uint8_t> response);
    int32_t sendSharedBufferResponse(hidl_vec<uint8_t> response,
        uint32_t offset);
    int32_t getSpcomSharedBuffer(uint32_t nBytes);
};
