/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __TUI_MINKINVOKE_H__
#define __TUI_MINKINVOKE_H__

#include <string>
#include <stdint.h>
#include "object.h"
#include <semaphore.h>
#include <thread>
#include "impl_base.hpp"
#include "proxy_base.hpp"
#include <TAVMInterface.h>
#include <ITrustedUIApp.hpp>
#include <vendor/qti/hardware/trustedui/1.0/ITrustedUICallback.h>
using android::sp;
using vendor::qti::hardware::trustedui::V1_0::ITrustedUICallback;
using ::android::hardware::hidl_memory;

class TUIMINKInvoke : public TAVMInterface
{
public:
    ~TUIMINKInvoke(){};

    int32_t createSession(std::string appName, int32_t dpyIdx, uint32_t inputType,
                          sem_t *sem, trusted_vm_event_t* vmStatus, uint32_t *sessionId,
                          const uint32_t uid, const hidl_memory& appBin, bool isTUIAppinOEMVM);

    int32_t sendCmd(uint32_t sessionId, uint32_t commandId,
            const std::vector<uint8_t> &commandData, std::vector<uint8_t> &responseData);

    int32_t startSession(uint32_t sessionId, vmSessionConfig cfg);

    int32_t stopSession(uint32_t sessionId);

    int32_t deleteSession(uint32_t sessionId);
private:
    std::string mAppName;
    uint32_t mTEESessionId;

    //App Object/handle
    TrustedUIApp *mTUIAppHandle = nullptr;

    int32_t _loadApp(const uint32_t uid, std::string appName, const hidl_memory& appBin, bool isTUIAppinOEMVM);
    int32_t _unloadApp();
    std::shared_ptr<std::thread> mWaitThread = nullptr;
    sp<ITrustedUICallback> mClientCB;
    sem_t *mSessionNotification;
};

#endif
