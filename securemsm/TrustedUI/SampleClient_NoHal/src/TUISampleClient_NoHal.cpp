/*
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <android-base/logging.h>
#include <cutils/properties.h>
#include <errno.h>
#include <hidl/HidlSupport.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <hidlmemory/mapping.h>
#include <hidl/Status.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <utils/Log.h>
#include "common_log.h"
#include "display_config.h"
#include <vector>
#include <fstream>
#include <dirent.h>
#include <fcntl.h>
#include "TUISampleClient_NoHal.h"

GTEST_DEFINE_bool_(enable_frame_auth, false,
                   "Enable periodic (every vsync) frame auth");
GTEST_DEFINE_int32_(logLevel, 1, "Set logging level");
GTEST_DEFINE_string_(tuiTEE, "qtee-vm-nohal", "Set tee env for TUI in VM: qtee-vm-nohal");

/** adb log */
#undef LOG_TAG
#define LOG_TAG "TUI_TEST_APP_NOHAL:"

#define LOGD_PRINT(...)                                                        \
  do {                                                                         \
    LOGD(__VA_ARGS__);                                                         \
    printf(__VA_ARGS__);                                                       \
    printf("\n");                                                              \
  } while (0)
//#define LOGD_PRINT(...) do {  } while(0)

#define LOGE_PRINT(...)                                                        \
  do {                                                                         \
    LOGE(__VA_ARGS__);                                                         \
    printf(__VA_ARGS__);                                                       \
    printf("\n");                                                              \
  } while (0)

#define TUI_DEMO_IND_PATH "/data/vendor/tui/sec_ind.png"
#define TUI_SAMPLE_APP_NAME_32BIT "tuiapp32"
#define TUI_SAMPLE_APP_NAME_64BIT "tuiapp"

static const std::string TUITestTAFile = "/data/vendor/tui/tuiapp.mbn";

#define MAX_NAME_LEN 25
#define MAX_SHARED_BUF_SIZE_IN_BYTES (63 * 1024)

// custom commands are 16bits
#define TUI_CMD_AUTHENTICATE_FRAME 5
#define TUI_CMD_SEND_MSG 18

#define TRUSTEDUIVM_CMD_OFFSET 0x03000000
#define TRUSTEDUIVM_CMD_PING (1 | TRUSTEDUIVM_CMD_OFFSET)

// increased shared buffer size only for secureindicator
#define TA_SHARE_BUFFER_SIZE 420000
#define INVALID_SESSION_ID (-1)

#define REGISTER_SIGNAL_HANDLER do { \
                               signal(SIGSEGV, TrustedUIAppTest::SignalHandler); \
                               signal(SIGTERM, TrustedUIAppTest::SignalHandler); \
                               signal(SIGABRT, TrustedUIAppTest::SignalHandler); \
                               signal(SIGINT, TrustedUIAppTest::SignalHandler);  \
                               } while(0);

bool mUseSecureIndicator = false;
bool mLoadTAFromBuffer = false;
bool mTUIAppinOEMVM = false;
int mDisplayId = 0, dispIdx = 0;
sem_t mSessionComplete;
sem_t mSessionWait;
bool mInputHandlingError = false;
static char legacy_app_name[MAX_NAME_LEN + 1] = TUI_SAMPLE_APP_NAME_64BIT;
static char *config_app_name = legacy_app_name;
static const uint32_t TUIAPP_UID = 1234;
static uint32_t appUID = 0;
static int32_t memSize = 0;

Return<void> TrustedUICallback::onComplete() {
  LOGD_PRINT("%s: TrustedUICallback:: onComplete Notification", __func__);
  sem_post(&mSessionComplete);
  return Void();
}

Return<void> TrustedUICallback::onError() {
  LOGD_PRINT("%s: TrustedUICallback:: onError Notification", __func__);
  mInputHandlingError = true;
  sem_post(&mSessionComplete);
  return Void();
}

int32_t GetQDUtilsDisplayType(int32_t displayId) {
  switch (displayId) {
  case 0: /*Primary Display*/
    return qdutils::DISPLAY_PRIMARY;
  case 1: /*Secondary Display*/
    return qdutils::DISPLAY_BUILTIN_2;
  default:
    return -1;
  }
}

int32_t GetDisplayPortId(int32_t displayId, int32_t *portId) {

  int qdutils_disp_type = GetQDUtilsDisplayType(displayId);
  if (qdutils_disp_type < 0) {
    ALOGE("Display Idx :%d is not supported", displayId);
    return -1;
  }
  int ret = qdutils::GetDisplayPortId(qdutils_disp_type, portId);
  if (ret != 0) {
    ALOGE("GetDisplayPortId failed with ret %d", ret);
    return -1;
  }
  ALOGI("display port id is %d for display %d", *portId, displayId);
  return 0;
}

TrustedUIAppTest::Result
TrustedUIAppTest::sendCmd(uint32_t commandId,
                          const std::vector<uint8_t> &commandData,
                          std::vector<uint8_t> &responseData) {
  Result ret = FAIL;
  uint32_t sessionId = 0;
  int32_t respSize = 0, cmdSize, minRespSize;

  cmdSize = commandData.size() + sizeof(uint32_t);
  if (cmdSize > UINT32_MAX) {
    ALOGE("%s: cmdSize overflow: %d", __func__, cmdSize);
    ret = Result::FAIL;
    goto end;
  }
  minRespSize = sizeof(uint32_t);

  if (memSize > MAX_SHARED_BUF_SIZE_IN_BYTES) {
    ALOGE(
        "%s: ERROR: shared memsize(%d) set by user is more than supported(%d)",
        __func__, memSize, MAX_SHARED_BUF_SIZE_IN_BYTES);
    ret = Result::FAIL;
    goto end;
  }

  if (memSize == 0) {
    ALOGI("%s: User has not set explicit memsize, setting memSize to max(%d)",
          __func__, MAX_SHARED_BUF_SIZE_IN_BYTES);
    memSize = MAX_SHARED_BUF_SIZE_IN_BYTES;
  }

  respSize = memSize - cmdSize;
  ALOGD("%s: Available respSize: %d", __func__, respSize);

  if (respSize < 0 || respSize < minRespSize) {
    ALOGE("sendCommand: Not sufficient respSize: %d", respSize);
    ret = Result::FAIL;
    goto end;
  }

  responseData.resize(respSize, 0);

  ret =
      (Result)mTAIntf->sendCmd(sessionId, commandId, commandData, responseData);

end:
  return ret;
}

int32_t TrustedUIAppTest::_readAppIntoBuffer(hidl_memory &appBin,
                                             const char *appPath) {
  int32_t ret = -1;
  int fd = -1;
  struct stat f_info = {};
  size_t TASize = 0;
  char *pBuf = nullptr;
  uint32_t getsize = 0;
  sp<IMemory> memory;
  sp<IAllocator> ashmemAllocator;

  fd = open(appPath, O_RDONLY);
  if (fd < 0) {
    ALOGE("%s::%d Could not open the Trusted App", __func__, __LINE__);
    goto end;
  }

  if (fstat(fd, &f_info) == -1) {
    ALOGE("%s::%d Could not get the Trusted App file info", __func__, __LINE__);
    goto end;
  }
  TASize = f_info.st_size;

  /* Get the Memory Allocator service from Memory HAL */
  ashmemAllocator = IAllocator::getService(memType);
  if (ashmemAllocator == nullptr) {
    ALOGE("%s::%d Could not get IAllocator service", __func__, __LINE__);
    goto end;
  }

  /* Allocate memory of size upto the TrustedApp size */
  ashmemAllocator->allocate(TASize, [&](bool success, const hidl_memory &mem) {
    if (!success) {
      ALOGE("%s::%d ashmem allocate failed!!", __func__, __LINE__);
      return;
    }
    /* Memory map hidl memory, so that the HAL memory can access it*/
    appBin = mem;
    memory = mapMemory(mem);
    if (memory == nullptr) {
      ALOGE("%s::%d Could not map HIDL memory to IMemory", __func__, __LINE__);
      return;
    }
    memory->update();
    pBuf = static_cast<char *>(static_cast<void *>(memory->getPointer()));
    if (pBuf == nullptr) {
      ALOGE("%s::%d Could not get the memory pointer", __func__, __LINE__);
      return;
    }
    getsize = (uint32_t)memory->getSize();
    LOGD_PRINT("%s:%d memory size is: %u", __func__, __LINE__, getsize);
    if (read(fd, pBuf, f_info.st_size) == -1) {
      ALOGE("%s::%d Error %d failed to read image.\n", __func__, __LINE__,
            errno);
      return;
    }
    memory->commit();
    ret = 0;
  });

end:
  if (fd >= 0) {
    close(fd);
  }
  return ret;
}

void TrustedUIAppTest::SetUp() {
  hidl_memory appBin, emptyAppBin;
  Result rv = FAIL;
  int32_t portId = 0;
  uint32_t sId = INVALID_SESSION_ID;
  uint32_t inputType = 1;
  int32_t ret = -1;
  ndk::SpAIBinder displayBinder;
  ScopedAStatus status = ndk::ScopedAStatus::ok();

  sem_init(&mSessionWait, 0, 0);
  sem_init(&mSessionComplete, 0, 0);

  if (GTEST_FLAG(tuiTEE) == "qtee-vm-nohal") {
    ALOGD("%s: VM Session init", __func__);

    // Add dual display support in TVM-TUI

    ret = GetDisplayPortId(dispIdx, &portId);
    if (ret) {
      LOGE_PRINT(
          "%s: Unable to get display Port information for display id :%d",
          __func__, dispIdx);
    }
    ASSERT_TRUE(ret == 0);

    // Java clients can call getPhysicalDisplayIds to pass displayId used.
    // Native clients can make use of qdutils to pass appropriate portId
    // instead.
    mDisplayId = portId;
    displayBinder = ::ndk::SpAIBinder((AServiceManager_checkService(
                      "vendor.qti.hardware.display.config.IDisplayConfig/default")));

    if (displayBinder.get() == NULL) {
      LOGE_PRINT("%s: DisplayConfig AIDL is not present", __func__);
    }
    ASSERT_TRUE(displayBinder.get() != NULL);

    mDisplayConfigIntf = IDisplayConfig::fromBinder(displayBinder);
    ASSERT_TRUE(mDisplayConfigIntf != nullptr);

    LOGD_PRINT("%s; DISABLE MDP Idle Power Collapse for Trusted UI Session",__func__);
    status = mDisplayConfigIntf->controlIdlePowerCollapse(false, false);
    if (!status.isOk()) {
      LOGE_PRINT("%s: IDisplayConfig::controlIdlePowerCollapse failed to DISABLE "
        "MDP Idle PC=[%d]", __func__, status.getServiceSpecificError());
      LOGE_PRINT("%s: Make sure that the display is ON", __func__);
    }
    ASSERT_TRUE(status.isOk());

    mClientCB = new TrustedUICallback();
    ASSERT_TRUE(mClientCB != nullptr);

    mDisplayType = DisplayType::INVALID;
    status = mDisplayConfigIntf->getDisplayType(mDisplayId, &mDisplayType);
    ASSERT_TRUE(status.isOk());
    ALOGD("%s: GetDisplayType[dpy:%d] returned mDisplayType: %d", __func__,
          mDisplayId, mDisplayType);

    switch (mDisplayType) {
    case DisplayType::PRIMARY:
      tuiDispIdx = TUI_PRIMARY_DISPLAY;
      mDisplayTypeStr = displayTypes[0];
      break;

    case DisplayType::BUILTIN2:
      tuiDispIdx = TUI_SECONDARY_DISPLAY;
      mDisplayTypeStr = displayTypes[1];
      break;

    default:
      ALOGE("%s: Invalid display type: %d",__func__, mDisplayType);
    }

    ALOGD("%s: Internal display Idx [dpy: %d] is %d", __func__, mDisplayId,
          tuiDispIdx);

    setLogLevel(GTEST_FLAG(logLevel));
    mTAIntf = TAVMInterface::getInterface(TUI_INTF_MINKINVOKE);
    TUI_CHECK_ERR(mTAIntf != nullptr, -1);

    if (mLoadTAFromBuffer) {
      LOGD_PRINT("%s:%d start to load TA from buff...", __func__, __LINE__);
      ret = _readAppIntoBuffer(appBin, TUITestTAFile.c_str());
      ASSERT_TRUE(appBin.valid() && ret == 0);
      rv = (Result)mTAIntf->createSession(
          config_app_name, tuiDispIdx, inputType, &mSessionWait,
          &mReceivedEvent, &mSessionId, 0, appBin, mTUIAppinOEMVM);
    }

    else {
      LOGD_PRINT("%s:%d start to load TA from file...", __func__, __LINE__);

      rv = (Result)mTAIntf->createSession(
          config_app_name, tuiDispIdx, inputType, &mSessionWait,
          &mReceivedEvent, &mSessionId, appUID, emptyAppBin, mTUIAppinOEMVM);
    }

    ASSERT_EQ(rv, Result::SUCCESS);

    mSetUpOk = true;
  }
end:
  return;
}

void TrustedUIAppTest::TearDown() {
  ScopedAStatus status = ndk::ScopedAStatus::ok();
  if (mSetUpOk == false) {
    LOGE_PRINT("TrustedUIAppTest:: Teardown Nothing to be done");
    return;
  }

  LOGD_PRINT("TrustedUIAppTest:: Teardown Issuing STOP");

  mTAIntf->stopSession(mSessionId);

  Result ret = (Result)mTAIntf->deleteSession(mSessionId);
  if (ret == Result::FAIL) {
    LOGE_PRINT("TrustedUIAppTest::TearDown failed to delete the Session");
  }

  LOGD_PRINT("%s: ENABLE MDP Idle Power Collapse", __func__);
  status = mDisplayConfigIntf->controlIdlePowerCollapse(true, false);
  if (!status.isOk()) {
    LOGE_PRINT(
      "%s: IDisplayConfig::controlIdlePowerCollapse failed to ENABLE "
      "MDP Idle PC=[%d]", __func__, status.getServiceSpecificError());
  }
  mDisplayConfigIntf = nullptr;
  ASSERT_TRUE(status.isOk());
  mClientCB = nullptr;
  sem_destroy(&mSessionComplete);

  LOGD_PRINT("TrustedUIAppTest::TearDown complete!");
  mSetUpOk = false;
}

void TrustedUIAppTest::SignalHandler(int signalNum) {
  ScopedAStatus status = ndk::ScopedAStatus::ok();
  ALOGE("%s: Error: TUI Sample Client of No HAL died RECEIVED SIGNAl:%d", __func__, signalNum);

  if (mTAIntf != nullptr) {
    mTAIntf->stopSession(mSessionId);
    Result ret = (Result)mTAIntf->deleteSession(mSessionId);
    if (ret == Result::FAIL) {
      ALOGE("TrustedUIAppTest::SignalHandler failed to delete the Session");
    }
  }

  if (mDisplayConfigIntf != nullptr) {
    LOGD_PRINT("%s: ENABLE MDP Idle Power Collapse", __func__);
    status = mDisplayConfigIntf->controlIdlePowerCollapse(true, false);
    if (status.isOk()) {
      LOGE_PRINT(
        "%s: IDisplayConfig::controlIdlePowerCollapse failed to ENABLE "
        "MDP Idle PC=[%d]", __func__, status.getServiceSpecificError());
    }
    mDisplayConfigIntf = nullptr;
  }

  mClientCB = nullptr;

  sem_destroy(&mSessionWait);
  sem_destroy(&mSessionComplete);
  exit(signalNum);
}

void TrustedUIAppTest::setLogLevel(uint32_t debugLevel) {
  char *level = NULL;
  switch (debugLevel) {
  case 0x1:
    level = "INFO";
    break;
  case 0x2:
    level = "DEBUG";
    break;
  case 0x3:
    level = "ALL";
    break;
  default:
    level = "ERROR";
  }
  if (0 == property_set("vendor.tui_debug_level", level)) {
    LOGD_PRINT("%s: set logging level:%s", __func__, level);
  }
}

int32_t TrustedUIAppTest::_waitForVMNotification() {
  std::vector<uint8_t> cmdData;
  std::vector<uint8_t> respData(4, 0);
  uint32_t cmdId = TRUSTEDUIVM_CMD_PING;
  Result ret;

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 1;

  errno = 0;
  while (sem_timedwait(&mSessionWait, &ts) != 0) {
    ALOGD("waitForVMNotification: sem err:%d %s", errno, strerror(errno));
    ret = sendCmd(cmdId, cmdData, respData);
    if (ret == Object_ERROR_UNAVAIL) {
      ALOGE("Connection to TUI App lost err: %d, terminating", ret);
      return -1;
    }

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;
    errno = 0;
  }
  return 0;
}

void TrustedUIAppTest::_waitForSessionComplete(TrustedUICallback *cb) {
  int32_t ret = 0;
  Return<void> retVal;

  ALOGD("%s::%d Waiting for TUI Session to complete ..", __func__, __LINE__);
  ret = _waitForVMNotification();
  {
    // VM has been disconnected, wait for VM to return resources
    if (ret != 0) {
      sleep(1);
    }
  }

  if (mReceivedEvent != TRUSTED_VM_EVENT_SESSION_COMPLETE) {
    ALOGE("%s: Session is aborted event:%d", __func__, mReceivedEvent);
    retVal = cb->onError();
  } else {
    ALOGE("%s: Session completed successfully %d", __func__, mReceivedEvent);
    retVal = cb->onComplete();
  }
  if (ret == -1 || (!retVal.isOk() && retVal.isDeadObject())) {
    ALOGE("%s::%d Callback to client Failed.Ending the current session now.",
          __func__, __LINE__);

    ASSERT_EQ(mTAIntf->stopSession(mSessionId), 0);
    ret = mTAIntf->deleteSession(mSessionId);
    if (ret == -1) {
      LOGE_PRINT("TrustedUIAppTest::TearDown failed to delete the Session");
    }
  }
  return;
}

void TrustedUIAppTest::_stopSessionWaitThreadIfRunning() {
  if (mSessionWaitThread != nullptr && mSessionWaitThread->joinable()) {
    ALOGE("%s: Waiting for session thread to join", __func__);
    mSessionWaitThread->join();
    mSessionWaitThread = nullptr;
    ALOGE("%s: Session thread is stopped", __func__);
  }
}

void TrustedUIAppTest::_startSessionWaitThreadIfRunning() {
  mSessionWaitThread = std::make_shared<std::thread>(
      [](TrustedUIAppTest *_app, TrustedUICallback *_cb) {
        _app->_waitForSessionComplete(_cb);
      },
      this, mClientCB);

  if (mSessionWaitThread == nullptr) {
    ALOGE("Failed to start session thread, abort");
    mTAIntf->stopSession(mSessionId);
    sem_destroy(&mSessionWait);
    goto end;
  }
end:
  return;
}

// TODO: Address this in current layout instead of doing it for next layout
#define CHECK_FOR_TEST_COMPLETION(ret)                                         \
  if ((ret) > 0) {                                                             \
    ret = 0;                                                                   \
    LOGD_PRINT("\nUser Pressed Cancel. Test Completed\n");                     \
    goto end;                                                                  \
  }

TEST_F(TrustedUIAppTest, TestTUILayouts) {
  ASSERT_TRUE(mTAIntf != nullptr);
  Result ret;
  int32_t rv = 0;
  mInputHandlingError = 0;

  while (1) {
    LOGD_PRINT("TrustedUIAppTest:: Start PIN Screen..");

    /* get pin screen */
    sCfg.layoutName = "pin";
    sCfg.useSecureIndicator = mUseSecureIndicator;
    sCfg.enableFrameAuth = false;

    LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession");

    ASSERT_GE(ret = (Result)mTAIntf->startSession(mSessionId, sCfg),
              Result::SUCCESS);
    rv = (int)ret;
    CHECK_FOR_TEST_COMPLETION(rv);

    LOGD_PRINT("TrustedUIAppTest:: Launched the layout. Start user interaction "
               "..");
    LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

    // Create thread for to wait async for session completion
    _startSessionWaitThreadIfRunning();

    sem_wait(&mSessionComplete);

    _stopSessionWaitThreadIfRunning();

    LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP");
    ASSERT_EQ(mTAIntf->stopSession(mSessionId), Result::SUCCESS);

    if (mInputHandlingError)
      break;

    /* message screen for showing the received pin */
    LOGD_PRINT("TrustedUIAppTest:: Start MSG_PIN Screen..");

    sCfg.layoutName = "msg_pin";
    sCfg.useSecureIndicator = mUseSecureIndicator;
    sCfg.enableFrameAuth = false;

    LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession");
    ASSERT_GE(ret = (Result)mTAIntf->startSession(mSessionId, sCfg),
              Result::SUCCESS);

    rv = (int)ret;
    CHECK_FOR_TEST_COMPLETION(rv);
    LOGD_PRINT("TrustedUIAppTest:: Launched the layout. Start user interaction "
               "..");
    LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

    // Create thread for to wait async for session completion
    _startSessionWaitThreadIfRunning();

    sem_wait(&mSessionComplete);

    _stopSessionWaitThreadIfRunning();

    LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP");
    ASSERT_EQ(mTAIntf->stopSession(mSessionId), Result::SUCCESS);

    if (mInputHandlingError)
      break;

    LOGD_PRINT("TrustedUIAppTest:: Start Login Screen..");

    sCfg.layoutName = "login";
    sCfg.useSecureIndicator = mUseSecureIndicator;
    sCfg.enableFrameAuth = false;

    LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession");
    ASSERT_GE(ret = (Result)mTAIntf->startSession(mSessionId, sCfg),
              Result::SUCCESS);

    rv = (int)ret;
    CHECK_FOR_TEST_COMPLETION(rv);
    LOGD_PRINT("TrustedUIAppTest:: Launched the layout. Start user interaction "
               "..");
    LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

    // Create thread for to wait async for session completion
    _startSessionWaitThreadIfRunning();

    sem_wait(&mSessionComplete);

    _stopSessionWaitThreadIfRunning();

    LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP");
    ASSERT_EQ(mTAIntf->stopSession(mSessionId), Result::SUCCESS);

    if (mInputHandlingError)
      break;

    LOGD_PRINT("TrustedUIAppTest:: Start LOGIN_MSG Screen..");

    sCfg.layoutName = "msg_login";
    sCfg.useSecureIndicator = mUseSecureIndicator;
    sCfg.enableFrameAuth = false;

    LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession");
    ASSERT_GE(ret = (Result)mTAIntf->startSession(mSessionId, sCfg),
              Result::SUCCESS);
    rv = (int)ret;
    CHECK_FOR_TEST_COMPLETION(rv);
    LOGD_PRINT("TrustedUIAppTest:: Launched the layout. Start user interaction "
               "..");
    LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

    // Create thread for to wait async for session completion
    _startSessionWaitThreadIfRunning();

    sem_wait(&mSessionComplete);

    _stopSessionWaitThreadIfRunning();

    LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP");
    ASSERT_EQ(mTAIntf->stopSession(mSessionId), Result::SUCCESS);

    if (mInputHandlingError)
      break;
  }
end:
  return;
}

TEST_F(TrustedUIAppTest, Basic_SecureTouch) {
  LOGD_PRINT("TrustedUIAppTest:: Basic_SecureTouch");
  ASSERT_TRUE(mTAIntf != nullptr);

  Result ret;
  sCfg.layoutName = "draw_dot";
  sCfg.useSecureIndicator = mUseSecureIndicator;
  sCfg.enableFrameAuth = false;

  LOGD_PRINT("TrustedUIAppTest::Sending cmd START");
  ASSERT_EQ(ret = (Result)mTAIntf->startSession(mSessionId, sCfg),
            Result::SUCCESS);

  LOGD_PRINT(
      "TrustedUIAppTest:: Launched the layout. Start user interaction ..");
  LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

  // Create thread for to wait async for session completion
  _startSessionWaitThreadIfRunning();

  sem_wait(&mSessionComplete);

  _stopSessionWaitThreadIfRunning();
  LOGD_PRINT("TrustedUIAppTest:: Session completed.");

  LOGD_PRINT("TrustedUIAppTest::Sending cmd STOP");
  ASSERT_EQ(mTAIntf->stopSession(mSessionId), Result::SUCCESS);
}

TEST_F(TrustedUIAppTest, Basic_TUIStartStop) {
  LOGD_PRINT("TrustedUIAppTest::Basic_TUIStartStop");
  ASSERT_TRUE(mTAIntf != nullptr);
  Result ret;

  sCfg.layoutName = "pin";
  sCfg.useSecureIndicator = false;
  sCfg.enableFrameAuth = false;

  LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession");
  ASSERT_GE(ret = (Result)mTAIntf->startSession(mSessionId, sCfg),
            Result::SUCCESS);

  LOGD_PRINT("TrustedUIAppTest::Session started successfully ");
  LOGD_PRINT("TrustedUIAppTest::Wait for 2 seconds before exiting..");
  sleep(2);

  LOGD_PRINT("TrustedUIAppTest::Sending cmd STOP");

  ASSERT_EQ(mTAIntf->stopSession(mSessionId), Result::SUCCESS);

  LOGD_PRINT("TrustedUIAppTest:: Session completed.");
}

// The purpose of this test is to check proper session clean up
// in case of error in startSession api.
TEST_F(TrustedUIAppTest, NegativeTest_TUIStartError) {
  LOGD_PRINT("TrustedUIAppTest::NegativeTest_TUIStartError");
  ASSERT_TRUE(mTAIntf != nullptr);

  //"no_layout" is not supported layout, so startSession should return err
  sCfg.layoutName = "no_layout";
  sCfg.useSecureIndicator = mUseSecureIndicator;
  sCfg.enableFrameAuth = false;

  Result ret;

  LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession");
  ASSERT_LT(ret = (Result)mTAIntf->startSession(mSessionId, sCfg),
            Result::SUCCESS);

  LOGD_PRINT("TrustedUIAppTest::Start Session failed as expected ret:%d", ret);
}

TEST_F(TrustedUIAppTest, NegativeTest_DeleteSessionWithoutStopSession) {
  LOGD_PRINT("TrustedUIAppTest::NegativeTest_DeleteSessionWithoutStopSession");
  ASSERT_TRUE(mTAIntf != nullptr);

  sCfg.layoutName = "pin";
  sCfg.useSecureIndicator = mUseSecureIndicator;
  sCfg.enableFrameAuth = false;

  Result ret;

  LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession");
  ASSERT_EQ(ret = (Result)mTAIntf->startSession(mSessionId, sCfg),
            Result::SUCCESS);

  LOGD_PRINT(
      "TrustedUIAppTest:: Launched the layout. Start user interaction ..");
  LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

  // Create thread for to wait async for session completion
  _startSessionWaitThreadIfRunning();

  sem_wait(&mSessionComplete);

  _stopSessionWaitThreadIfRunning();
  LOGD_PRINT("TrustedUIAppTest:: Session completed.");

  LOGD_PRINT("TrustedUIAppTest:: deleteSession ..");
  ASSERT_NE(ret = (Result)mTAIntf->deleteSession(mSessionId), Result::SUCCESS);
  LOGD_PRINT("TrustedUIAppTest::deleteSession failed as expected !");
}

TEST_F(TrustedUIAppTest, TestSendCmd) {
  LOGD_PRINT("TrustedUIAppTest:: TestSendCmd");
  ASSERT_TRUE(mTAIntf != nullptr);

  std::vector<uint8_t> cmdData;
  std::vector<uint8_t> respData;
  uint16_t cmdId = TUI_CMD_SEND_MSG;

  std::string msg = "this msg is from HLOS";
  for (char i : msg) {
    cmdData.push_back(i);
  }
  cmdData.push_back('\0');
  Result ret;

  ret = sendCmd(cmdId, cmdData, respData);
  if (ret == Object_ERROR_UNAVAIL) {
    ALOGE("Connection to TUI App lost err: %d, terminating", ret);
  }

  LOGD_PRINT("TrustedUIAppTest:: SendCmd returned: %d", ret);
  ASSERT_EQ(ret, Result::SUCCESS);
  LOGD_PRINT("cmd response: %d", *((uint32_t *)respData.data()));
}

TEST_F(TrustedUIAppTest, SendCmdTestMaxRespSize) {
  LOGD_PRINT("TrustedUIAppTest:: SendCmdTestMaxRespSize");
  ASSERT_TRUE(mTAIntf != nullptr);
  Result ret;

  std::vector<uint8_t> cmdData;
  std::vector<uint8_t> rspData;
  uint16_t cmdId = TUI_CMD_SEND_MSG;

  // fills buffer with 10240 bytes
  std::string msg = "this msg is from HLOS";
  for (int i = 0; i < 512; i++) {
    for (char i : msg) {
      cmdData.push_back(i);
    }
  }

  cmdData.push_back('\0');

  ret = sendCmd(cmdId, cmdData, rspData);
  if (ret == Object_ERROR_UNAVAIL) {
    ALOGE("Connection to TUI App lost err: %d, terminating", ret);
  }

  LOGD_PRINT("TrustedUIAppTest:: SendCmd returned: %d", ret);
  ASSERT_EQ(ret, Result::SUCCESS);
  LOGD_PRINT("cmd response: %d", *((uint32_t *)rspData.data()));

  // rsp buffer populated with n%256
  LOGD_PRINT("%x", rspData[1280]);
  ASSERT_EQ(rspData[1280], 1280 % 256);
}

TEST_F(TrustedUIAppTest, SecureIndicator2_StoreIndicator) {
  LOGD_PRINT("TrustedUIAppTest:: SecureIndicator2_StoreIndicator");

  const std::string imagePath = "/data/vendor/tui/sec_ind.png";
  ASSERT_EQ(SecureIndicator2_StoreSecureIndicator(imagePath), TUI_SI_SUCCESS);

  LOGD_PRINT(
      "TrustedUIAppTest:: SecureIndicator2 StoreIndicator call completed.");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  if (argc == 1)
    goto run;

  for (int i = 1; i < argc; i++) {
    // Enable Medium level logging
    if ((strcmp(argv[i], "-d1") == 0) ||
        (strcmp(argv[i], "--debug=medium") == 0)) {
      GTEST_FLAG(logLevel) = 1;
      continue;
      // Enable Low level logging
    } else if ((strcmp(argv[i], "-d2") == 0) ||
               (strcmp(argv[i], "--debug=low") == 0)) {
      GTEST_FLAG(logLevel) = 2;
      continue;
      // Enable all logs
    } else if ((strcmp(argv[i], "-d") == 0) ||
               (strcmp(argv[i], "--debug=all") == 0)) {
      GTEST_FLAG(logLevel) = 3;
      continue;
    } else if ((strcmp(argv[i], "-vm-nohal") == 0)) {
      LOGD_PRINT("Selecting VM implementation with no hal");
      GTEST_FLAG(tuiTEE) = "qtee-vm-nohal";
      continue;
      // Enable secure indicator
    } else if ((strcmp(argv[i], "-si") == 0) ||
               (strcmp(argv[i], "--si") == 0)) {
      LOGD_PRINT("Enable secure indicator");
      mUseSecureIndicator = true;
      memSize = TA_SHARE_BUFFER_SIZE;
      continue;
    } else if (strcmp(argv[i], "-loadtafrombuffer") == 0) {
      LOGD_PRINT("Enable Loading TA from buffer.");
      mLoadTAFromBuffer = true;
      continue;
    } else if (((strcmp(argv[i], "-UID") == 0) && (i + 1) < argc) ||
               ((strcmp(argv[i], "--appUID") == 0) && (i + 1) < argc)) {
      appUID = atoi(argv[i + 1]);
      appUID = appUID >= 0 ? appUID : TUIAPP_UID;
      LOGD_PRINT("TUI App UID :%d", appUID);
      i++;
      continue;
    } else if (((strcmp(argv[i], "-id") == 0) && (i + 1) < argc) ||
               ((strcmp(argv[i], "--displayid") == 0) && (i + 1) < argc)) {
      mDisplayId = atoi(argv[i + 1]);
      mDisplayId = mDisplayId >= 0 ? mDisplayId : 0;
      dispIdx = mDisplayId;
      i += 1;
      LOGD_PRINT("set display ID :%d", mDisplayId);
      continue;
    } else if (strcmp(argv[i], "-appname") == 0 && (i + 1) < argc) {
      config_app_name = argv[i + 1];
      i++;
      continue;
    } else if (strncmp(argv[i], "--appname=", sizeof("--appname=") - 1) == 0) {
      config_app_name = argv[i] + sizeof("--appname=") - 1;
      continue;
    } else if ((strcmp(argv[i], "-oem-vm") == 0)) {
      LOGD_PRINT("Selecting OEM VM to run TUI");
      mTUIAppinOEMVM = true;
      continue;
    } else {
      LOGD_PRINT("Use: TrustedUISampleTest --gtest_filter=*[test]* [-opts]");
      LOGD_PRINT("options:");
      LOGD_PRINT("--gtest_filter=*{test_name}* \t\t Run specific tests "
                 "(TestTUILayouts, SecureTouch).");
      LOGD_PRINT("--appname={TUI TA name} \t\t Optional TUI TA name, default "
                 "is tuiapp");
      LOGD_PRINT(
          "--si \t\t\t\t Enable secure indicator. it is disabled by default.");
      LOGD_PRINT(
          "--displayid {display id number}  Set display ID, it's 0 by defult.");
      LOGD_PRINT("-vm-nohal \t\t\t\t Use VM implementation of TUI-NoHal");
      LOGD_PRINT(
          "-d1 \t\t\t\t Enable INFO logging. Error is enabled by default");
      LOGD_PRINT(
          "-d2 \t\t\t\t Enable DEBUG logging. Error is enabled by default");
      LOGD_PRINT("-d \t\t\t\t Enable ALL logging. Error is enabled by default");
      LOGD_PRINT("-appname {TUI TA name} \t\t\t\t Optional TUI TA name, "
                 "default is tuiapp");
      LOGD_PRINT(
          "-si \t\t\t\t Enable secure indicator. it is disabled by default");
      LOGD_PRINT(
          "-loadtafrombuffer \t Enable loading TA from buffer. TA image:%s",
          TUITestTAFile.c_str());
      LOGD_PRINT("-UID {TUI App UID} \t\t\t\t Pass App UID from client. "
                 "Default is TUI App UID");
      LOGD_PRINT("-appUID {TUI App UID} \t\t\t\t Pass App UID from client. "
                 "Default is TUI App UID");
      LOGD_PRINT("-id {display id number}\t Set display ID, it's 0 by defult.");
      LOGD_PRINT("-oem-vm \t\t\t\t Run OEM VM TUI without HAL");
      return 0;
    }
  }
run:
  REGISTER_SIGNAL_HANDLER;
  return RUN_ALL_TESTS();
}
