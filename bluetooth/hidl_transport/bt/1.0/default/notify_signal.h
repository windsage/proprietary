/*==========================================================================
Description
 It has implementation for Signaling Power driver class

# Copyright (c) 2024 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once

#include <cutils/properties.h>
#include "data_handler.h"
#include <hidl/HidlSupport.h>
#include <hci_uart_transport.h>
#include <logger.h>
#include "power_manager.h"
#include "patch_dl_manager.h"
#include "data_handler.h"

#include <stdint.h>

#define REGISTER_BT_PID     5
#define SSR_ON_BT           1
#define BT_SSR_COMPLETED    2

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {
// Sub-System SSR status

typedef enum {
  WAITING_FOR_SOC_ACCESS = 0,
  ACCESS_GRANTED = 1,
  SSR_ON_OTHER_CLIENT = 2,
  SSR_ON_OTHER_CLINET_COMPLETED = 3,
} SoCAccessWaitState;

typedef enum {
  UWB_SSR_STATE_IDLE = 0,
  SSR_ON_UWB,
  UWB_SSR_COMPLETED,
} ssr_states;

typedef enum {
  SOC_ACCESS_GRANTED = 0,
  SOC_ACCESS_DENIED = 1,
  SOC_ACCESS_RELEASED = 2,
  SOC_ACCESS_DISALLOWED = -1,
} SoCAccessState;

class NotifySignal {
  public:
    NotifySignal();
    ~NotifySignal();
    static NotifySignal *Get(void);
    void RegSigIOCallBack(void);
    static void SigIOSignalHandler(int signum, siginfo_t *info, void *unused);
    static void PowerDriverSignalHandler(int uwb_ssr_state);
    static const int notify_signal_cmd_ = 0xbfe2;
    static const int soc_access_cmd = 0xbfe4;
    bool RegisterService(void);
    int GetSubSystemSsrStatus(void);
    bool NotifyDriver(int SsrState);
    bool notifySubsystem;
    SoCAccessState RequestSoCAccess(void);
    SoCAccessState ReleaseSoCAccess(void);
    bool WaitForSoCAccess();
 protected:
 private:
   static NotifySignal *instance_;
   static void SoCAccessSigHandler(SoCAccessWaitState);
   static void UpdateSoCAccessState(SoCAccessWaitState);
};


} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
