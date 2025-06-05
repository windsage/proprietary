/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __TUI_TA_VM_INTERFACE_H__
#define __TUI_TA_VM_INTERFACE_H__

/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/
#include <stdint.h>
#include <string>
#include <semaphore.h>
#include <vendor/qti/hardware/trustedui/1.0/ITrustedUICallback.h>
#include <android/hidl/memory/1.0/IMemory.h>
using android::sp;
using vendor::qti::hardware::trustedui::V1_0::ITrustedUICallback;
using ::android::hardware::hidl_memory;
#define TRUSTED_VM_OEM_ERROR 0x7FFF

/** Event indication from Trusted VM /Android SystemService to HAL */
typedef enum {
    TRUSTED_VM_EVENT_SESSION_COMPLETE = 0, /** Session completed */
    TRUSTED_VM_EVENT_SESSION_CANCELLED = 1, /** Session cancelled by user  */
    TRUSTED_VM_EVENT_SESSSION_ERROR = 2,   /** Session error */
    TRUSTED_VM_EVENT_ABORT = 3,   /** Abort request */
    TRUSTED_VM_EVENT_OEM_EVENT = 4,   /** OEM Event */
    TRUSTED_VM_EVENT_MAX = TRUSTED_VM_EVENT_OEM_EVENT,
    TRUSTED_VM_EVENT_INVALID = (int32_t)-1
} trusted_vm_event_t;

struct vmSessionConfig {
    bool useSecureIndicator;
    std::string layoutName;
    bool enableFrameAuth;
    uint32_t inputController;
};

static enum TAVMIntf { TUI_INTF_MINKINVOKE };

class TAVMInterface
{
public:
    virtual ~TAVMInterface(){};

    static TAVMInterface *getInterface(uint8_t intfType);

    virtual int32_t createSession(std::string appName, int32_t dpyIdx,
            uint32_t inputType, sem_t *sem, trusted_vm_event_t* vmStatus,
            uint32_t *sessionId, const uint32_t uid, const hidl_memory& appBin, bool isTUIAppinOEMVM);

    virtual int32_t startSession(uint32_t sessionId, vmSessionConfig cfg);

    virtual int32_t stopSession(uint32_t sessionId);

    virtual int32_t deleteSession(uint32_t sessionId);

    virtual int32_t sendCmd(uint32_t sessionId, uint32_t commandId,
                            const std::vector<uint8_t> &commandData,
                            std::vector<uint8_t> &responseData);


};

#endif
