/*
 * Copyright (c) 2019-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <android-base/logging.h>
#include <cutils/properties.h>
#include <errno.h>
#include <gtest/gtest.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <poll.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <stringl.h>
#include <sys/stat.h>
#include <utils/Log.h>
#include <sys/mman.h>
#include <unistd.h>
#include <aidl/vendor/qti/hardware/display/config/IDisplayConfig.h>
#include <aidl/vendor/qti/hardware/trustedui/BnTrustedUI.h>
#include <aidl/vendor/qti/hardware/trustedui/BnTrustedInput.h>
#include <thread>
#include "TrustedUICallback.h"
#include "common_log.h"
#include "display_config.h"
#include "SecureIndicatorAPI.h"
#include <aidl/android/hardware/common/Ashmem.h>
#include <cutils/ashmem.h>
#include "memscpy.h"

// AIDL specific namespaces
using ::ndk::ScopedAStatus;
using ::ndk::ScopedFileDescriptor;
using ::aidl::android::hardware::common::Ashmem;
using ::aidl::vendor::qti::hardware::display::config::IDisplayConfig;
using ::aidl::vendor::qti::hardware::trustedui::ITrustedUI;
using ::aidl::vendor::qti::hardware::trustedui::ITrustedInput;
using ::aidl::vendor::qti::hardware::trustedui::ITrustedUICallback;
using ::aidl::vendor::qti::hardware::trustedui::TUICreateParams;
using ::aidl::vendor::qti::hardware::trustedui::TUIResponse;
using ::aidl::vendor::qti::hardware::trustedui::TUIConfig;
using ::aidl::vendor::qti::hardware::trustedui::TUIOutputID;
using ::aidl::vendor::qti::hardware::trustedui::implementation::TrustedUICallback;

GTEST_DEFINE_bool_(enable_frame_auth, false,
                   "Enable periodic (every vsync) frame auth");
GTEST_DEFINE_int32_(logLevel, 1, "Set logging level");
GTEST_DEFINE_string_(tuiTEE, "qtee-tz", "Set tee env for TUI: qtee-tz for TZ, qtee-vm for VM");

/** adb log */
#undef LOG_TAG
#define LOG_TAG "TUI_TEST_APP:"

#define LOGD_PRINT(...)      \
    do {                     \
        LOGD(__VA_ARGS__);   \
        printf(__VA_ARGS__); \
        printf("\n");        \
    } while (0)
//#define LOGD_PRINT(...) do {  } while(0)

#define LOGE_PRINT(...)      \
    do {                     \
        LOGE(__VA_ARGS__);   \
        printf(__VA_ARGS__); \
        printf("\n");        \
    } while (0)

#define FRAME_AUTH_START \
    if (GTEST_FLAG(enable_frame_auth)) startAuthenticateSecureDisplay();

#define FRAME_AUTH_STOP \
    if (GTEST_FLAG(enable_frame_auth)) stopAuthenticateSecureDisplay();

#define TUI_DEMO_LOGO_PATH "/data/vendor/tui/logo.png"
#define TUI_DEMO_IND_PATH "/data/vendor/tui/sec_ind.png"
#define TUI_SAMPLE_APP_NAME_32BIT "tuiapp32"
#define TUI_SAMPLE_APP_NAME_64BIT "tuiapp"

static const std::string TUITestTAFile = "/data/vendor/tui/tuiapp.mbn";

#define TUI_APP_MAX_NAME_LEN  25

// custom commands are 16bits
#define TUI_CMD_AUTHENTICATE_FRAME 5
#define TUI_CMD_SEND_MSG 18
#define TUI_CMD_SET_PROMPT_TEXT 19
#define TUI_CMD_GET_CONF_RESULT 20

//increased shared buffer size only for secureindicator
#define TA_SHARE_BUFFER_SIZE 420000

bool mUseSecureIndicator = false;
int  mDisplayId = 0, dispIdx = 0;
bool mInputHandlingError = false;
bool mHALkilled = false;
sem_t mSessionComplete;
bool mLoadTAFromBuffer = false;

// TODO: Implement app_buffer_basic_test
static char legacy_app_name[TUI_APP_MAX_NAME_LEN + 1] = TUI_SAMPLE_APP_NAME_64BIT;
static char *config_app_name = legacy_app_name;
static const uint32_t TUIAPP_UID = 292;
static uint32_t appUID = 0;
static uint32_t iterations = 1;

#ifdef ENABLE_TRUSTED_UI_VM_3_0
static int GetQDUtilsDisplayType(int32_t displayId) {
  switch(displayId) {
    case 0: /*Primary Display*/
      return qdutils::DISPLAY_PRIMARY;
    case 1: /*Secondary Display*/
      return qdutils::DISPLAY_BUILTIN_2;
    default:
      return -1;
  }
}

static int GetDisplayPortId(int32_t displayId, int32_t *portId) {

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
#endif

class TrustedUIAppTest : public ::testing::Test
{

    public:
    ~TrustedUIAppTest() {};
    uint32_t mSessionId = -1;
    std::shared_ptr<ITrustedUI> mTUIIntf = nullptr;
    std::shared_ptr<ITrustedUICallback> mSessionCallback = nullptr;

    protected:
    virtual void SetUp();
    virtual void TearDown();
    virtual int32_t checkPrerequisites();
    void *postVSyncLoop();
    int32_t startAuthenticateSecureDisplay();
    int32_t stopAuthenticateSecureDisplay();
    void setLogLevel(uint32_t level);
    void getTUIInterface(std::string destination);

    private:

    TUIOutputID outParam;
    size_t TASize = 0;
    int32_t mBufferFd = -1;
    ::ndk::SpAIBinder tuiBinder;
    ::ndk::SpAIBinder tuiInputBinder;
    ::ndk::ScopedAIBinder_DeathRecipient deathRecipient;
    bool mSetUpOk = false;
    bool gVSyncThreadDone = false;
    std::string mTrustedEE;
    std::shared_ptr<IDisplayConfig> mDisplayConfigIntf = nullptr;
    std::shared_ptr<std::thread> mAuthThread = nullptr;

    std::shared_ptr<ITrustedInput> mTrustedInputDevice = nullptr;
    int32_t _readAppIntoBuffer(const char* appPath);
    static void deathRecipientCallback(void* cookie);

};

void TrustedUIAppTest :: deathRecipientCallback(void* cookie) {
    ALOGI("TUIHAL just died");
    mHALkilled = true;
    sem_post(&mSessionComplete);
}

int32_t TrustedUIAppTest::_readAppIntoBuffer(const char* appPath)
{
    int fd = -1;
    struct stat f_info = {};
    void *pBuf = nullptr;
    void *appBuffer = nullptr;
    ssize_t writeLen = 0;
    int ret = 0;
    size_t copied = 0;

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
    pBuf = malloc(TASize);
    if (pBuf == NULL) {
        ALOGE("%s::%d malloc failed", __func__, __LINE__);
        goto end;
    }
    ret = read(fd, pBuf, TASize);
    if (ret != TASize) {
        ALOGE("%s::%d Failed to read buffer data", __func__, __LINE__);
        goto end;
    }

    // Logic to convert input app buffer data to Ashmem format
    mBufferFd = ashmem_create_region("ashem_TA_file", TASize);
    if (mBufferFd <= 0) {
        ALOGE("%s::%d Failed to create file for buffer read", __func__, __LINE__);
        goto end;
    }

    appBuffer = mmap(NULL, TASize, PROT_READ | PROT_WRITE, MAP_SHARED, mBufferFd, 0);
    if (appBuffer == MAP_FAILED) {
        ALOGE("%s: Unable to peform memory map for app buffer: %s", __func__, strerror(errno));
        goto end;
    }

    /* copying into appbuffer */
    copied =  memscpy(appBuffer, TASize, pBuf, TASize);
    if (copied != TASize) {
        ALOGE("%s: error while copying app buffer", __func__);
        goto end;
    }
    munmap(appBuffer, TASize);

    if (fd >= 0) {
      close(fd);
    }
    if (pBuf != NULL) {
        free(pBuf);
    }
    return 0;
end:
   if (fd >= 0) {
      close(fd);
   }
   if (mBufferFd >= 0) {
       close(mBufferFd);
   }
   if (pBuf != NULL) {
       free(pBuf);
   }
   return -1;
}

// TrustedUICallback methods
ndk::ScopedAStatus TrustedUICallback::onComplete()
{
    LOGD_PRINT("%s: TrustedUICallback:: onComplete Notification", __func__);
    sem_post(&mSessionComplete);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus TrustedUICallback::onError()
{
    LOGD_PRINT("%s: onError Notification", __func__);
    mInputHandlingError = true;
    sem_post(&mSessionComplete);
    return ndk::ScopedAStatus::ok();
}

// TrustedUIAppTest methods
void TrustedUIAppTest::getTUIInterface(std::string destination) {
    int32_t ret = -1;
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    if(mTUIIntf == nullptr && mTrustedInputDevice == nullptr) {
        {
            const std::string instance = std::string() + ITrustedUI::descriptor + "/" +destination;
            // If the client can be up before the service, and getService is sometimes returning null,
            // use AServiceManager_waitForService(instance.c_str()) instead to wait until the service is up
            tuiBinder = ::ndk::SpAIBinder(AServiceManager_waitForService(instance.c_str()));
        }

        if (tuiBinder.get() == nullptr) {
            ALOGE("TUI HAL service doesn't exist");
            goto end;
        }

        {
            const std::string instance = std::string() + ITrustedInput::descriptor + "/" +destination;
            // If the client can be up before the service, and getService is sometimes returning null,
            // use AServiceManager_waitForService(instance.c_str()) instead to wait until the service is up
            tuiInputBinder = ::ndk::SpAIBinder(AServiceManager_waitForService(instance.c_str()));
        }

        if (tuiInputBinder.get() == nullptr) {
            ALOGE("Trusted Input HAL service doesn't exist");
            goto end;
        }

        deathRecipient = ::ndk::ScopedAIBinder_DeathRecipient(AIBinder_DeathRecipient_new(&deathRecipientCallback));
        status = ::ndk::ScopedAStatus::fromStatus(AIBinder_linkToDeath(tuiBinder.get(), deathRecipient.get(), (void*) deathRecipientCallback));

        if (!status.isOk()) {
            ALOGE("linking TUI HAL service to death failed: %d: %s", status.getStatus(), status.getMessage());
            goto end;
        }

        mTUIIntf = ITrustedUI::fromBinder(tuiBinder);
        mTrustedInputDevice = ITrustedInput::fromBinder(tuiInputBinder);

        if (mTUIIntf == nullptr || mTrustedInputDevice == nullptr) {
            ALOGE("TUI HAL service doesn't exist");
            goto end;
        }

        ALOGD("TUI HAL service linked to death!!");
        ret = 0;
    }

end:
   if(ret == 0) {
      ALOGD("Successful in getting TUI HAL service and link to death");
   }
}

void TrustedUIAppTest::SetUp()
{
    const std::string instance = std::string() + ITrustedUI::descriptor +"/default";
    TUIResponse res;
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();

    if (!AServiceManager_isDeclared(instance.c_str())) {
        LOGD_PRINT("%s:%d AIDL service is not declared in VINTF manifest", __func__, __LINE__);
        return;
    }

    if (GTEST_FLAG(tuiTEE) == "qtee-vm") {
        LOGD_PRINT("====================  TUI :: VM ==========================");
        getTUIInterface("default");

#ifdef ENABLE_TRUSTED_UI_VM_3_0
        //Add dual display support in TVM-TUI
        int32_t portId = 0, rv = -1;
        rv = GetDisplayPortId(dispIdx, &portId);
        if (rv) {
            LOGE_PRINT("%s: Unable to get display Port information for display id :%d", __func__, dispIdx);
        }
        ASSERT_TRUE(rv == 0);

        //Java clients can call getPhysicalDisplayIds to pass displayId used.
        //Native clients can make use of qdutils to pass appropriate portId instead.
        mDisplayId = portId;
#endif
    } else {
        LOGD_PRINT("====================  TUI :: TZ ==========================");
        getTUIInterface("default");
    }

    ASSERT_TRUE(mTUIIntf != nullptr);
    ASSERT_TRUE(mTrustedInputDevice != nullptr);

    //Create Thread pool to receive callbacks
    if (!ABinderProcess_isThreadPoolStarted()) {
        ABinderProcess_startThreadPool();
    }

    LOGD("%s: TrustedUICallback ", __func__);
    mSessionCallback = ndk::SharedRefBase::make<TrustedUICallback>();

    ASSERT_TRUE(mSessionCallback != nullptr);
    ndk::SpAIBinder displayBinder = ::ndk::SpAIBinder((
         AServiceManager_checkService("vendor.qti.hardware.display.config.IDisplayConfig/default")));

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

    if(mTUIIntf != nullptr) {

        LOGD_PRINT("TrustedUIAppTest:: SetUp SUCCESS in connecting to AIDL TrustedUI HAL");
        setLogLevel(GTEST_FLAG(logLevel));

        if (mLoadTAFromBuffer) {
            LOGD_PRINT("%s:%d start to load TA from buff...", __func__, __LINE__);
            int32_t ret = _readAppIntoBuffer(TUITestTAFile.c_str());

            if((mBufferFd) == -1 || (ret == -1)) {
                LOGD_PRINT("%s: Failed to read App into buffer",__func__);
            }

            TUICreateParams inParams = { .trustedEE = config_app_name,
                                         .dpyIdx = mDisplayId,
                                         .minSharedMemSize = (int32_t)(mUseSecureIndicator ? TA_SHARE_BUFFER_SIZE : 0),
                                         .appBin.fd = ScopedFileDescriptor(mBufferFd),
                                         .appBin.size = (int) TASize,
                                         .TUI_UID = 0};
            status = mTUIIntf->createSession(inParams, mTrustedInputDevice, mSessionCallback, &outParam, &res);
            mSessionId = outParam.sessionId;
            LOGD_PRINT("%s:%d sessionId = %u", __func__, __LINE__, mSessionId);
        } else {
            // Create dummy file as ScopeFileDescriptor doesn't allow invalid fd
            mBufferFd = ashmem_create_region("TUIAppBuffer", 0);
            ASSERT_GT(mBufferFd, 0);
            TUICreateParams inParams = { .trustedEE = config_app_name,
                                         .dpyIdx = mDisplayId,
                                         .minSharedMemSize = (int32_t)(mUseSecureIndicator ? TA_SHARE_BUFFER_SIZE : 0),
                                         .appBin.fd = ScopedFileDescriptor(mBufferFd),
                                         .appBin.size = 0,
                                         .TUI_UID = 0 };
            status = mTUIIntf->createSession(inParams, mTrustedInputDevice, mSessionCallback, &outParam, &res);
            mSessionId = outParam.sessionId;
            LOGD_PRINT("%s:%d sessionId = %u", __func__, __LINE__, mSessionId);
            LOGD_PRINT("TrustedUIAppTest:: SetUp sharedMemSize: %d ",inParams.minSharedMemSize);
        }
        if (mBufferFd >= 0) {
            close(mBufferFd);
        }
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(res, TUIResponse::TUI_SUCCESS);
        LOGD_PRINT("TrustedUIAppTest:: SetUp  HAL State is now : LOADED ");
        sem_init(&mSessionComplete, 0, 0);

        mSetUpOk = true;
    }
}

void TrustedUIAppTest::TearDown()
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    TUIResponse ret;
    if (mSetUpOk == false) {
        LOGE_PRINT("TrustedUIAppTest:: Teardown Nothing to be done");
        return;
    }

    if (!mHALkilled) {
    /* Call TUI Stop in case the test is tearing down due to error
     * and did not get to Stop the TUI Session. If not called, it will
     * result in failure of all subsequent tests */
        LOGD_PRINT("TrustedUIAppTest:: Teardown Issuing STOP to TUI HAL");

        status = mTUIIntf->stopSession(mSessionId, &ret);
        status = mTUIIntf->deleteSession(mSessionId, &ret);

        if (status.isOk() && ret == TUIResponse::TUI_FAILURE) {
            LOGE_PRINT("TrustedUIAppTest::TearDown failed to delete the Session");
        }

        if(mTUIIntf != nullptr) {
            status = ScopedAStatus::fromStatus(AIBinder_unlinkToDeath(tuiBinder.get(), deathRecipient.get(),
                                               (void*) deathRecipientCallback));
            if (!status.isOk()) {
                LOGE_PRINT("Failed to unlinking from death recipient%d: %s", status.getStatus(), status.getMessage());
            }
        }
    }

    sem_destroy(&mSessionComplete);

    LOGD_PRINT("TrustedUIAppTest::TearDown complete!");
    mSetUpOk = false;

    LOGD_PRINT("%s: ENABLE MDP Idle Power Collapse", __func__);
    status = mDisplayConfigIntf->controlIdlePowerCollapse(true, false);
    if (!status.isOk()) {
        LOGE_PRINT(
            "%s: IDisplayConfig::controlIdlePowerCollapse failed to ENABLE "
            "MDP Idle PC=[%d]", __func__, status.getServiceSpecificError());
    }
    mDisplayConfigIntf = nullptr;
    ASSERT_TRUE(status.isOk());
}

int32_t TrustedUIAppTest::checkPrerequisites()
{
    struct stat st;
    if (GTEST_FLAG(tuiTEE) == "qtee-vm") {
        return 0;
    }

    if (stat(TUI_DEMO_LOGO_PATH, &st) != 0) {
        LOGE_PRINT("TUI Sample Client: logo image is missing");
        LOGD_PRINT(
            "TUI Sample Client: logo image should be a PNG located at: %s",
            TUI_DEMO_LOGO_PATH);
        return -1;
    }

    if (stat(TUI_DEMO_IND_PATH, &st) != 0) {
        LOGE_PRINT(
            "TUI Sample Client: indicator image is missing, will try to "
            "display the secure indicator instead");
        LOGD_PRINT(
            "TUI Sample Client: indicator image should be a PNG located at: %s",
            TUI_DEMO_IND_PATH);
    }
    return 0;
}

void TrustedUIAppTest::setLogLevel(uint32_t debugLevel)
{
    char *level = NULL;
    switch(debugLevel) {
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

void *TrustedUIAppTest::postVSyncLoop()
{
    int32_t fds[2] = {-1};
    TUIResponse ret;
    std::vector<uint8_t> _responseData;
    std::vector<uint8_t> cmdData;

    if (pipe(fds) == -1) {
        ALOGE("%s: pipe() failed !", __func__);
        return NULL;
    }
    struct pollfd poll_fd = {
        .fd = fds[0], .events = POLLIN,
    };

    while (!gVSyncThreadDone) {
        /* poll timeout time is kept random between 16 and 32 ms , so
         * as to make the "time of CRC check in TZ" random   and a max of 32 ms
         * as we want to check CRC every Vsync */
        int random_poll_timeout = 16 + (rand() % 16);
        int status = poll(&poll_fd, 1, random_poll_timeout /*ms*/);
        if (status == 0) { /*TIMEOUT*/
            ALOGD("%s: Sending cmd TUI_CMD_AUTHENTICATE_FRAME", __func__);

            mTUIIntf->sendCommand(mSessionId, TUI_CMD_AUTHENTICATE_FRAME, cmdData, &_responseData, &ret);
            if (ret != TUIResponse::TUI_SUCCESS) {
                ALOGE("%s: send command failed with ret : %d", __func__, ret);
                break;
            }

        } else {
            ALOGE("%s: poll failed !", __func__);
            break;
        }
    }

    close(fds[0]);
    close(fds[1]);
    return NULL;
}

int32_t TrustedUIAppTest::startAuthenticateSecureDisplay()
{
    int32_t ret = 0;

    /* Create the main thread of sending Secure UI authenticate cmd requests in
     * a while loop */
    mAuthThread =
        std::make_shared<std::thread>([&]() { this->postVSyncLoop(); });
    if (!mAuthThread->joinable()) {
        ALOGE(
            "Error: Creating thread for vsync-trigger in secure UI client "
            "failed!");
        ret = -1;
        goto end;
    }
end:
    return ret;
}

int32_t TrustedUIAppTest::stopAuthenticateSecureDisplay()
{
    int32_t ret = 0;

    if (gVSyncThreadDone || !mAuthThread->joinable()) {
        ALOGE(
            "Error: %s called while thread not joinable, gVSyncThreadDone:%d "
            "joinable:%d",
            gVSyncThreadDone, mAuthThread->joinable());
        ret = -1;
        goto end;
    }
    gVSyncThreadDone = true;
    mAuthThread->join();
end:
    return ret;
}

// TODO: Address this in current layout instead of doing it for next layout
#define CHECK_FOR_TEST_COMPLETION(ret)                         \
    if ((ret) > 0) {                                           \
        ret = 0;                                               \
        LOGD_PRINT("\nUser Pressed Cancel. Test Completed\n"); \
        goto end;                                              \
    }

TEST_F(TrustedUIAppTest, TestTUILayouts)
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    ASSERT_TRUE(mTUIIntf != nullptr);
    TUIResponse ret;
    int32_t rv = 0;
    mInputHandlingError = 0;
    ASSERT_EQ(checkPrerequisites(), 0);

    TUIConfig cfg;
    while (1) {
        LOGD_PRINT("TrustedUIAppTest:: Start PIN Screen..");

        /* get pin screen */
        cfg = {mUseSecureIndicator /*useSecureIndicator*/, "pin", false /*enableFrameAuth*/};
        status = mTUIIntf->startSession(mSessionId, cfg, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_GE(ret, TUIResponse::TUI_SUCCESS);
        rv = (int)ret;
        CHECK_FOR_TEST_COMPLETION(rv);
        LOGD_PRINT(
            "TrustedUIAppTest:: Launched the layout. Start user interaction "
            "..");
        LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

        FRAME_AUTH_START;
        sem_wait(&mSessionComplete);
        LOGD_PRINT("TrustedUIAppTest:: Session completed.");
        FRAME_AUTH_STOP;

        if (mHALkilled) break;

        LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP to TUI HAL");
        status = mTUIIntf->stopSession(mSessionId, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);

        if (mInputHandlingError) break;

        /* message screen for showing the received pin */
        LOGD_PRINT("TrustedUIAppTest:: Start MSG_PIN Screen..");
        cfg = {mUseSecureIndicator /*useSecureIndicator*/, "msg_pin", false /*enableFrameAuth*/};
        status = mTUIIntf->startSession(mSessionId, cfg, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_GE(ret, TUIResponse::TUI_SUCCESS);
        rv = (int)ret;
        CHECK_FOR_TEST_COMPLETION(rv);

        LOGD_PRINT(
            "TrustedUIAppTest:: Launched the layout. Start user interaction "
            "..");
        LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

        FRAME_AUTH_START;
        sem_wait(&mSessionComplete);
        LOGD_PRINT("TrustedUIAppTest:: Session completed.");
        FRAME_AUTH_STOP;

        if (mHALkilled) break;

        LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP to TUI HAL");
        status = mTUIIntf->stopSession(mSessionId, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);

        if (mInputHandlingError) break;

        /* login screen */
        LOGD_PRINT("TrustedUIAppTest:: Start LOGIN Screen..");
        cfg = {mUseSecureIndicator /*useSecureIndicator*/, "login", false /*enableFrameAuth*/};
        status = mTUIIntf->startSession(mSessionId, cfg, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_GE(ret, TUIResponse::TUI_SUCCESS);
        rv = (int)ret;
        CHECK_FOR_TEST_COMPLETION(rv);

        LOGD_PRINT(
            "TrustedUIAppTest:: Launched the layout. Start user interaction "
            "..");
        LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

        FRAME_AUTH_START;
        sem_wait(&mSessionComplete);
        LOGD_PRINT("TrustedUIAppTest:: Session completed.");
        FRAME_AUTH_STOP;

        if (mHALkilled) break;

        LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP to TUI HAL");
        status = mTUIIntf->stopSession(mSessionId, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);

        if (mInputHandlingError) break;

        /* message screen for showing the received username & password */
        LOGD_PRINT("TrustedUIAppTest:: Start MSG_LOGIN Screen..");
        cfg = {mUseSecureIndicator, "msg_login", false /*enableFrameAuth*/};
        status = mTUIIntf->startSession(mSessionId, cfg, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_GE(ret, TUIResponse::TUI_SUCCESS);
        rv = (int)ret;
        CHECK_FOR_TEST_COMPLETION(rv);

        LOGD_PRINT(
            "TrustedUIAppTest:: Launched the layout. Start user interaction "
            "..");
        LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

        FRAME_AUTH_START;
        sem_wait(&mSessionComplete);
        LOGD_PRINT("TrustedUIAppTest:: Session completed.");
        FRAME_AUTH_STOP;

        if (mHALkilled) break;

        LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP to TUI HAL");
        status = mTUIIntf->stopSession(mSessionId, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);
        if (mInputHandlingError) break;

    }
end:
    return;
}

TEST_F(TrustedUIAppTest, SecureTouch)
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    LOGD_PRINT("TrustedUIAppTest:: Basic_SecureTouch");
    ASSERT_TRUE(mTUIIntf != nullptr);

    TUIResponse ret;
    TUIConfig cfg = {mUseSecureIndicator /*useSecureIndicator*/, "draw_dot",
                     false /*enableFrameAuth*/};

    LOGD_PRINT("TrustedUIAppTest::Sending cmd START to TUI HAL");
    status = mTUIIntf->startSession(mSessionId, cfg, &ret);
    ASSERT_TRUE(status.isOk());
    ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);

    LOGD_PRINT(
        "TrustedUIAppTest:: Launched the layout. Start user interaction ..");
    LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

    FRAME_AUTH_START;
    sem_wait(&mSessionComplete);
    LOGD_PRINT("TrustedUIAppTest:: Session completed.");
    FRAME_AUTH_STOP;

    if (mHALkilled) return;

    LOGD_PRINT("TrustedUIAppTest::Sending cmd STOP to TUI HAL");
    status = mTUIIntf->stopSession(mSessionId, &ret);
    ASSERT_TRUE(status.isOk());
    ASSERT_EQ(ret,TUIResponse::TUI_SUCCESS);

}

// NOTE: display-only tests are currently not supported
TEST_F(TrustedUIAppTest, Basic_TUIStartStop)
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    TUIResponse ret;
    LOGD_PRINT("TrustedUIAppTest::Basic_TUIStartStop");
    ASSERT_TRUE(mTUIIntf != nullptr);

    TUIConfig cfg = {mUseSecureIndicator /*useSecureIndicator*/, "pin",
                     false /*enableFrameAuth*/};

    LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession to TUI HAL");
    status = mTUIIntf->startSession(mSessionId, cfg, &ret);
    ASSERT_TRUE(status.isOk());
    ASSERT_GE(ret, TUIResponse::TUI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest::Session started successfully ");
    LOGD_PRINT("TrustedUIAppTest::Wait for 2 seconds before exiting..");
    sleep(2);

    LOGD_PRINT("TrustedUIAppTest::Sending cmd STOP to TUI HAL");
    status = mTUIIntf->stopSession(mSessionId, &ret);
    ASSERT_TRUE(status.isOk());
    ASSERT_EQ(ret,TUIResponse::TUI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest:: Session completed.");

}

// This test runs multiple sessions and sendCmd during and between sessions
TEST_F(TrustedUIAppTest, TUI_Stability)
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    LOGD_PRINT("TrustedUIAppTest::Basic_TUIStartStop");
    ASSERT_TRUE(mTUIIntf != nullptr);

    std::vector<uint8_t> cmdData;
    std::vector<uint8_t> rspData;
    TUIConfig cfg;
    TUIResponse ret;
    size_t rspLastIndex;

    uint16_t cmdId = TUI_CMD_SEND_MSG;

    // fills buffer with around 500KB
    std::string msg = "this msg is from HLOS";
    for (int i = 0; i < 24000; i++)
    {
        for (char i : msg)
        {
            cmdData.push_back(i);
        }
    }
    cmdData.push_back('\0');

    for (int i = 0; i < iterations; i++)
    {
        cfg = {mUseSecureIndicator /*useSecureIndicator*/, "pin",
               false /*enableFrameAuth*/};

        LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession to TUI HAL");
        status = mTUIIntf->startSession(mSessionId, cfg, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);

        LOGD_PRINT("TrustedUIAppTest::Session started successfully ");
        LOGD_PRINT("TrustedUIAppTest::Wait for 1 second before exiting..");
        sleep(1);

        ASSERT_TRUE(mTUIIntf != nullptr);

        status = mTUIIntf->sendCommand(mSessionId, cmdId, cmdData, &rspData, &ret);
        ASSERT_TRUE(status.isOk());

        LOGD_PRINT("TrustedUIAppTest:: SendCmd returned: %d", ret);
        ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);
        LOGD_PRINT("cmd response: %d", *((uint32_t *)rspData.data()));

        rspLastIndex = rspData.size() - 1;

        LOGD_PRINT("%x", rspData[rspLastIndex]);
        ASSERT_EQ(rspData[rspLastIndex], rspLastIndex % 256);

        LOGD_PRINT("TrustedUIAppTest::Sending cmd STOP to TUI HAL");
        status = mTUIIntf->stopSession(mSessionId, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(ret,TUIResponse::TUI_SUCCESS);

        cfg = {mUseSecureIndicator /*useSecureIndicator*/, "login",
               false /*enableFrameAuth*/};

        LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession to TUI HAL");
        status = mTUIIntf->startSession(mSessionId, cfg, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);

        LOGD_PRINT("TrustedUIAppTest::Session started successfully ");
        LOGD_PRINT("TrustedUIAppTest::Wait for  second before exiting..");
        sleep(1);

        LOGD_PRINT("TrustedUIAppTest::Sending cmd STOP to TUI HAL");
        status = mTUIIntf->stopSession(mSessionId, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(ret,TUIResponse::TUI_SUCCESS);

        ASSERT_TRUE(mTUIIntf != nullptr);

        status = mTUIIntf->sendCommand(mSessionId, cmdId, cmdData, &rspData, &ret);
        ASSERT_TRUE(status.isOk());
        LOGD_PRINT("TrustedUIAppTest:: SendCmd returned: %d", ret);
        ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);
        LOGD_PRINT("cmd response: %d", *((uint32_t *)rspData.data()));

        rspLastIndex = rspData.size() - 1;

        LOGD_PRINT("%x", rspData[rspLastIndex]);
        ASSERT_EQ(rspData[rspLastIndex], rspLastIndex % 256);

        cfg = {mUseSecureIndicator /*useSecureIndicator*/, "msg_login",
               false /*enableFrameAuth*/};

        LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession to TUI HAL");
        status = mTUIIntf->startSession(mSessionId, cfg, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);

        LOGD_PRINT("TrustedUIAppTest::Session started successfully ");
        LOGD_PRINT("TrustedUIAppTest::Wait for 1 second before exiting..");
        sleep(1);

        LOGD_PRINT("TrustedUIAppTest::Sending cmd STOP to TUI HAL");
        status = mTUIIntf->stopSession(mSessionId, &ret);
        ASSERT_TRUE(status.isOk());
        ASSERT_EQ(ret,TUIResponse::TUI_SUCCESS);
        LOGD_PRINT("TrustedUIAppTest:: Session completed.");
    }
}

//The purpose of this test is to check proper session clean up
//in case of error in startSession api.
TEST_F(TrustedUIAppTest, Basic_NegativeTest_TUIStartError)
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    TUIResponse ret;
    LOGD_PRINT("TrustedUIAppTest::NegativeTest_TUIStartError");
    ASSERT_TRUE(mTUIIntf != nullptr);

    //"no_layout" is not supported layout, so startSession should return err
    TUIConfig cfg = {mUseSecureIndicator /*useSecureIndicator*/, "no_layout",
                     false /*enableFrameAuth*/};

    LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession to TUI HAL");
    status = mTUIIntf->startSession(mSessionId, cfg, &ret);
    ASSERT_TRUE(status.isOk());
    ASSERT_LT(ret, TUIResponse::TUI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest::Start Session failed as expected ret:%d", ret);

}

TEST_F(TrustedUIAppTest, NegativeTest_DeleteSessionWithoutStopSession)
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    TUIResponse ret;
    LOGD_PRINT(
        "TrustedUIAppTest::NegativeTest_DeleteSessionWithoutStopSession");
    ASSERT_TRUE(mTUIIntf != nullptr);

    TUIConfig cfg = {mUseSecureIndicator /*useSecureIndicator*/, "pin",
                     false /*enableFrameAuth*/};

    LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession to TUI HAL");
    status = mTUIIntf->startSession(mSessionId, cfg, &ret);
    ASSERT_TRUE(status.isOk());
    ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);

    LOGD_PRINT(
        "TrustedUIAppTest:: Launched the layout. Start user interaction ..");
    LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

    sem_wait(&mSessionComplete);
    LOGD_PRINT("TrustedUIAppTest:: Session completed.");

    if (mHALkilled) return;

    LOGD_PRINT("TrustedUIAppTest:: deleteSession ..");
    status = mTUIIntf->deleteSession(mSessionId, &ret);
    ASSERT_TRUE(status.isOk());
    ASSERT_EQ(ret, TUIResponse::TUI_FAILURE);

    LOGD_PRINT("TrustedUIAppTest::deleteSession failed as expected !");
}

TEST_F(TrustedUIAppTest, Test_SecureIndicatorProvision)
{
    boolean result = 0;
    uint32 maxWidthInPixels = 0, maxHeightInPixels = 0;

    LOGD_PRINT(
    "TrustedUIAppTest::NegativeTest_SecureIndicatorProvision");

    LOGD_PRINT("TrustedUIAppTest::Check if the sec_ind.png is present ...");
    ASSERT_EQ(checkPrerequisites(), 0);

    LOGD_PRINT("TrustedUIAppTest::Init the secure Indicator ...");
    ASSERT_EQ(SecureIndicator_Init(), TUI_SI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest::Check if a secure indicator has been provisioned ...");
    ASSERT_EQ(SecureIndicator_IsIndicatorProvisioned(&result), TUI_SI_SUCCESS);
    if (result) {
            LOGD_PRINT("TrustedUIAppTest::Found a secure indicator has been provisioned, removed it ...");
            ASSERT_EQ(SecureIndicator_RemoveIndicator(), TUI_SI_SUCCESS);
    }

    LOGD_PRINT("TrustedUIAppTest::get the demensions of secure indicator from secure indicator TA ...");
    ASSERT_EQ(SecureIndicator_GetSecureIndicatorDimensions(&maxWidthInPixels, &maxHeightInPixels), TUI_SI_SUCCESS);
    LOGD_PRINT("TrustedUIAppTest:: demensions of secure indicator are %d(width) and %d(hight)...", maxWidthInPixels, maxHeightInPixels);

    LOGD_PRINT("TrustedUIAppTest::Store a new image to secure indicator ...");
    ASSERT_EQ(SecureIndicator_StoreSecureIndicator(TUI_DEMO_IND_PATH), TUI_SI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest::Check if the secure indicator has been provisioned successfully...");
    ASSERT_EQ(SecureIndicator_IsIndicatorProvisioned(&result), TUI_SI_SUCCESS);
    ASSERT_EQ(result, 1);

    LOGD_PRINT("TrustedUIAppTest::the secure indicator has been provisioned successfully, remove it...");
    ASSERT_EQ(SecureIndicator_RemoveIndicator(), TUI_SI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest::Tear down the secure indicator ...");
    ASSERT_EQ(SecureIndicator_Teardown(), TUI_SI_SUCCESS);
}

TEST_F(TrustedUIAppTest, TestGetConfResultCmd)
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    uint16_t cmdId = TUI_CMD_SET_PROMPT_TEXT;
    std::vector<uint8_t> cmdData;
    std::vector<uint8_t> rspData;
    TUIResponse ret;

    LOGD_PRINT("TrustedUIAppTest:: TestGetConfResultCmd");
    ASSERT_TRUE(mTUIIntf != nullptr);

    // fill cmdData with promptText
    std::string promptText = "testing123 prompt text";
    for (char i : promptText) {
        cmdData.push_back(i);
    }
    cmdData.push_back('\0'); // null terminated

    // first we fill promptText for this test
    status = mTUIIntf->sendCommand(mSessionId, cmdId, cmdData, &rspData, &ret);
    ASSERT_TRUE(status.isOk());

    LOGD_PRINT("TrustedUIAppTest:: TUI_CMD_SET_PROMPT_TEXT returned: %d", ret);
    ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);
    rspData.clear();

    // now we test the get confirmation result (HMAC + prompt text)
    cmdId = TUI_CMD_GET_CONF_RESULT;
    // get the HMAC
    status = mTUIIntf->sendCommand(mSessionId, cmdId, cmdData, &rspData, &ret);
    ASSERT_TRUE(status.isOk());
    ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS); // we check to make sure the sendCommand returned successfully

    LOGD_PRINT("TrustedUIAppTest:: TestGetConfResultCmd returned: %d", ret);
    LOGD_PRINT("TrustedUIAppTest:: cmd response: %u", *((uint32_t *)rspData.data()));
}

TEST_F(TrustedUIAppTest, Basic_TestSendCmd)
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    uint16_t cmdId = TUI_CMD_SEND_MSG;
    TUIResponse ret;
    std::vector<uint8_t> cmdData;
    std::vector<uint8_t> rspData;

    LOGD_PRINT("TrustedUIAppTest:: TestSendCmd");
    ASSERT_TRUE(mTUIIntf != nullptr);

    std::string msg = "this msg is from HLOS";
    for (char i : msg) {
        cmdData.push_back(i);
    }
    cmdData.push_back('\0');

    status = mTUIIntf->sendCommand(mSessionId, cmdId, cmdData, &rspData, &ret);
    ASSERT_TRUE(status.isOk());

    LOGD_PRINT("TrustedUIAppTest:: SendCmd returned: %d", ret);
    ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);
    LOGD_PRINT("cmd response: %d", *((uint32_t *)rspData.data()));
}

TEST_F(TrustedUIAppTest, Basic_SendCmdTestMaxRespSize)
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    TUIResponse ret;
    uint16_t cmdId = TUI_CMD_SEND_MSG;
    std::vector<uint8_t> cmdData;
    std::vector<uint8_t> rspData;

    LOGD_PRINT("TrustedUIAppTest:: SendCmdTestMaxRespSize");
    ASSERT_TRUE(mTUIIntf != nullptr);

    //fills buffer with 10240 bytes
    std::string msg = "this msg is from HLOS";
    for (int i = 0; i < 512; i++) {
        for (char i : msg) {
            cmdData.push_back(i);
        }
    }
    cmdData.push_back('\0');

    status = mTUIIntf->sendCommand(mSessionId, cmdId, cmdData, &rspData, &ret);
    ASSERT_TRUE(status.isOk());

    LOGD_PRINT("TrustedUIAppTest:: SendCmd returned: %d", ret);
    ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);
    LOGD_PRINT("cmd response: %d", *((uint32_t *)rspData.data()));

    size_t rspLastIndex = rspData.size() - 1;

    /*rsp buffer populated with n%256 by TA
      checking last index populated*/
    LOGD_PRINT("%x", rspData[rspLastIndex]);
    ASSERT_EQ(rspData[rspLastIndex], rspLastIndex % 256);
}

TEST_F(TrustedUIAppTest, Basic_SendCmdTestMaxDataSize)
{
    ::ndk::ScopedAStatus status = ::ndk::ScopedAStatus::ok();
    uint16_t cmdId = TUI_CMD_SEND_MSG;
    TUIResponse ret;
    std::vector<uint8_t> cmdData;
    std::vector<uint8_t> rspData;

    LOGD_PRINT("TrustedUIAppTest:: SendCmdTestMaxDataSize");
    ASSERT_TRUE(mTUIIntf != nullptr);

    //fills buffer with around 500KB
    std::string msg = "this msg is from HLOS";
    for (int i = 0; i < 24000; i++) {
        for (char i : msg) {
            cmdData.push_back(i);
        }
    }
    cmdData.push_back('\0');

    status = mTUIIntf->sendCommand(mSessionId, cmdId, cmdData, &rspData, &ret);
    ASSERT_TRUE(status.isOk());

    LOGD_PRINT("TrustedUIAppTest:: SendCmd returned: %d", ret);
    ASSERT_EQ(ret, TUIResponse::TUI_SUCCESS);
    LOGD_PRINT("cmd response: %d", *((uint32_t *)rspData.data()));

    size_t rspLastIndex = rspData.size() - 1;

    /*rsp buffer populated with n%256 by TA
      checking last index populated*/
    LOGD_PRINT("%x", rspData[rspLastIndex]);
    ASSERT_EQ(rspData[rspLastIndex], rspLastIndex % 256);
}

TEST_F(TrustedUIAppTest, SecureIndicator2_StoreIndicator)
{
    LOGD_PRINT("TrustedUIAppTest:: SecureIndicator2_StoreIndicator");

    const std::string imagePath = "/data/vendor/tui/sec_ind.png";
    ASSERT_EQ(SecureIndicator2_StoreSecureIndicator(imagePath), TUI_SI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest:: SecureIndicator2 StoreIndicator call completed.");
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    if (argc == 1) goto run;

    for (int i = 1; i < argc; i++) {
        // Enable frame authentication
        if ((strcmp(argv[i], "-f") == 0) || (strcmp(argv[i], "--frameauth") == 0)) {
            GTEST_FLAG(enable_frame_auth) = true;
            continue;
        } // Enable Medium level logging
        else if ((strcmp(argv[i], "-d1") == 0) || (strcmp(argv[i], "--debug=medium") == 0)) {
            GTEST_FLAG(logLevel) = 1;
            continue;
        // Enable Low level logging
        } else if ((strcmp(argv[i], "-d2") == 0) || (strcmp(argv[i], "--debug=low") == 0)) {
            GTEST_FLAG(logLevel) = 2;
            continue;
        // Enable all logs
        } else if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--debug=all") == 0)) {
            GTEST_FLAG(logLevel) = 3;
            continue;
        } else if ((strcmp(argv[i], "-vm") == 0)) {
            LOGD_PRINT("Selecting VM implementation");
            GTEST_FLAG(tuiTEE) = "qtee-vm";
            continue;
        } else if ((strcmp(argv[i], "-tz") == 0)) {
            LOGD_PRINT("Selecting TZ implementation");
            GTEST_FLAG(tuiTEE) = "qtee-tz";
            continue;
        // Enable secure indicator
        } else if ((strcmp(argv[i], "-si") == 0) || (strcmp(argv[i], "--si") == 0)) {
            LOGD_PRINT("Enable secure indicator");
            mUseSecureIndicator = true;
            continue;
        } else if (strcmp(argv[i], "-loadtafrombuffer") == 0) {
            LOGD_PRINT("Enable Loading TA from buffer.");
            mLoadTAFromBuffer = true;
            continue;
        } else if (((strcmp(argv[i], "-UID") == 0) && (i + 1) < argc)||
                   ((strcmp(argv[i], "--appUID") == 0) && (i + 1) < argc)) {
            appUID = atoi(argv[i+1]);
            appUID = appUID >= 0 ? appUID : TUIAPP_UID;
            LOGD_PRINT("TUI App UID :%d", appUID);
            i++;
            continue;
        } else if (((strcmp(argv[i], "-id") == 0) && (i + 1) < argc)||
                   ((strcmp(argv[i], "--displayid") == 0) && (i + 1) < argc)) {
            mDisplayId = atoi(argv[i+1]);
            mDisplayId = mDisplayId >= 0 ? mDisplayId : 0;
            dispIdx = mDisplayId;
            i += 1;
            LOGD_PRINT("set display ID :%d", mDisplayId);
            continue;
        } else if (strcmp(argv[i], "-appname") == 0 && (i + 1) < argc) {
            config_app_name = argv[i+1];
            i++;
            continue;
        } else if (strncmp(argv[i], "--appname=", sizeof("--appname=") - 1 ) == 0) {
            config_app_name = argv[i] + sizeof("--appname=") - 1;
            continue;
        } else if (strncmp(argv[i], "-i", sizeof("-i") - 1 ) == 0) {
            iterations = atoi(argv[i+1]);
            i++;
            continue;
        } else {
            LOGD_PRINT("Use: TrustedUISampleTest --gtest_filter=*[test]* [-opts]");
            LOGD_PRINT("options:");
            LOGD_PRINT("--gtest_filter=*{test_name}* \t\t Run specific tests (TestTUILayouts, SecureTouch).");
            LOGD_PRINT("--appname={TUI TA name} \t\t Optional TUI TA name, default is tuiapp");
            LOGD_PRINT("--si \t\t\t\t Enable secure indicator. it is disabled by default.");
            LOGD_PRINT("--displayid {display id number}  Set display ID, it's 0 by defult.");
            LOGD_PRINT("-f \t\t\t\t Enable frame authentication");
            LOGD_PRINT("-vm \t\t\t\t Use VM implementation of TUI");
            LOGD_PRINT("-d1 \t\t\t\t Enable INFO logging. Error is enabled by default");
            LOGD_PRINT("-d2 \t\t\t\t Enable DEBUG logging. Error is enabled by default");
            LOGD_PRINT("-d \t\t\t\t Enable ALL logging. Error is enabled by default");
            LOGD_PRINT("-appname {TUI TA name} \t\t\t\t Optional TUI TA name, default is tuiapp");
            LOGD_PRINT("-si \t\t\t\t Enable secure indicator. it is disabled by default");
            LOGD_PRINT("-loadtafrombuffer \t Enable loading TA from buffer. TA image:%s", TUITestTAFile.c_str());
            LOGD_PRINT("-UID {TUI App UID} \t\t\t\t Pass App UID from client. Default is TUI App UID");
            LOGD_PRINT("-appUID {TUI App UID} \t\t\t\t Pass App UID from client. Default is TUI App UID");
            LOGD_PRINT("-id {display id number}\t Set display ID, it's 0 by defult.");
            return 0;
        }
    }
run:
    return RUN_ALL_TESTS();
}
