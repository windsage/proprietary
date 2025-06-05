/*!
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <aidl/vendor/qti/hardware/spu/BnSPComServer.h>
#include <stdint.h>
#include "log.h"
#include "SPUTestDefinitions.h"

using namespace std;
using aidl::vendor::qti::hardware::spu::ISPComServer;
using aidl::vendor::qti::hardware::spu::ISPComSharedBuffer;
using ::android::sp;

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
    std::shared_ptr<ISPComServer> server = nullptr;
    std::shared_ptr<ISPComSharedBuffer> sharedBuffer = nullptr;
    void * sharedMemory = nullptr;
    Ashmem mem;
    bool isValid = false;

    int32_t waitForNextRequest(SPComServerRequestId expectedRequest);
    int32_t sendResponse(std::vector<uint8_t> response);
    int32_t sendModifiedResponse(std::vector<uint8_t> &response, uint32_t offset);
    int32_t getSPComSharedBuffer(uint32_t nBytes);
    int32_t releaseSPComSharedBuffer();
};
