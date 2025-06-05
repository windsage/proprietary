/*
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __TUI_SAMPLECLIENT_H__
#define __TUI_SAMPLECLIENT_H__

#include "TAVMInterface.h"
#include "types.hpp"
#include <TUIUtils.h>
#include <TrustedUICallback.h>
#include <aidl/vendor/qti/hardware/display/config/DisplayType.h>
#include <aidl/vendor/qti/hardware/display/config/IDisplayConfig.h>
#include <android/binder_manager.h>
#include <gtest/gtest.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <stringl.h>
#include <thread>
#include "SecureIndicatorAPI.h"

using namespace android;
using namespace std;
using android::sp;
using android::hardware::Return;
using android::hardware::Void;
using ::aidl::vendor::qti::hardware::display::config::DisplayType;
using ::aidl::vendor::qti::hardware::display::config::IDisplayConfig;
using ::android::hardware::hidl_string;
using ::android::hidl::base::V1_0::IBase;
using ::android::hidl::memory::V1_0::IMemory;
using ::android::hardware::hidl_memory;
using ::android::hidl::allocator::V1_0::IAllocator;
using ::ndk::ScopedAStatus;

class TrustedUIAppTest : public ::testing::Test {
public:
  virtual ~TrustedUIAppTest() = default;

  enum Result : int32_t {
    SUCCESS = 0,
    FAIL = -1,
    SEC_UI_LISTENER_ERROR = -2,
    SEC_UI_ALREADY_RUNNING = -3,
    SEC_UI_IS_NOT_RUNNING = -4,
    SEC_UI_SERVICE_NOT_READY = -31,
  };

  Result sendCmd(uint32_t commandId, const std::vector<uint8_t> &commandData,
                 std::vector<uint8_t> &responseData);

  virtual void SetUp();
  virtual void TearDown();
  void setLogLevel(uint32_t level);
  int32_t _waitForVMNotification();
  void _waitForSessionComplete(TrustedUICallback *cb);
  void _stopSessionWaitThreadIfRunning();
  void _startSessionWaitThreadIfRunning();
  int32_t _readAppIntoBuffer(hidl_memory &appBin, const char *appPath);
  static void SignalHandler(int signalNum);

  static inline TAVMInterface *mTAIntf = nullptr;
  static inline TrustedUICallback *mClientCB = nullptr;
  int32_t tuiDispIdx = -1;
  trusted_vm_event_t mReceivedEvent = TRUSTED_VM_EVENT_INVALID;
  static inline const std::string memType = "ashmem";
  static inline uint32_t mSessionId = 0;
  vmSessionConfig sCfg;
  DisplayType mDisplayType;
  inline static const std::vector<hidl_string> displayTypes = {"primary",
                                                               "secondary"};
  bool mSetUpOk = false;
  hidl_string mDisplayTypeStr = "primary";
  static inline std::shared_ptr<IDisplayConfig> mDisplayConfigIntf = nullptr;
  std::shared_ptr<std::thread> mSessionWaitThread = nullptr;
  void TestBody() {}
};

#endif
