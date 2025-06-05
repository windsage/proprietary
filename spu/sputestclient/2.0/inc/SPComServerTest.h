/*!
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <vendor/qti/spu/2.0/ISPComServer.h>
#include <utils/StrongPointer.h>
#include <stdint.h>
#include "log.h"
#include "SPUTestDefinitions.h"

using namespace std;
using vendor::qti::spu::V2_0::ISPComServer;
using vendor::qti::spu::V2_0::ISPComSharedBuffer;
using ::android::hardware::hidl_vec;
using android::sp;

class SPComServerTest {

public:

    explicit SPComServerTest(const char* name);
    virtual ~SPComServerTest();

    int32_t openCloseChStress(size_t cycles = testCycles);
    int32_t registerServer();
    int32_t unregisterServer();
    int32_t waitForPingSendPong();
    int32_t sendModifiedResponseFlow();
    int32_t sendInvalidModifiedResponse();
    int32_t handleSSR();
    inline bool valid() { return isValid; }

private:

    enum SPComServerRequestId : uint32_t {
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

    string chName;
    const char *chNameStr = nullptr;
    sp<ISPComServer> server = nullptr;
    sp<ISPComSharedBuffer> sharedBuffer = nullptr;
    sp<IMemory> sharedMemory = nullptr;
    bool isValid = false;

    int32_t waitForNextRequest(SPComServerRequestId expectedRequest);
    int32_t sendResponse(hidl_vec<uint8_t> response);
    int32_t sendModifiedResponse(hidl_vec<uint8_t> &response, uint32_t offset);
    int32_t getSPComSharedBuffer(uint32_t nBytes);
};
