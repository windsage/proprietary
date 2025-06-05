/******************************************************************************
  @file    HintExtHandler.h
  @brief   Declaration of hint extensions

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __HINT_EXT_HANDLER__H_
#define __HINT_EXT_HANDLER__H_

#include <mutex>
#include "PerfConfig.h"
#include "PerfController.h"
#include <unordered_map>
#include "MpctlUtils.h"
#include <config.h>
#include <time.h>

#define MAX_HANDLERS 16
#define FPS_SWITCH_MIN_HYST_TIME_SECS 1

enum hintTypeForPassingPid {
    HINT_TYPE_NONE = 0,
    HINT_TYPE_FOR_SF_PID = 1,
    HINT_TYPE_FOR_RE_TID = 2,
    HINT_TYPE_FOR_HWC_TID = 3,
    HINT_TYPE_FOR_APP_PID = 4,
    HINT_TYPE_FOR_RENDER_TID = 5,
    HINT_TYPE_MAX = 6,
};

typedef int (*HintExtAction)(mpctl_msg_t *);

typedef struct HintActionInfo {
    uint32_t mHintId;
    HintExtAction mPreAction;
    HintExtAction mPostAction;
    HintExtAction mHintExcluder;
} HintActionInfo;

class HintExtHandler {
    private:
        std::mutex mMutex;
        int mNumHandlers;
        bool mLALStatus = false;
        bool mLGLStatus = false;
        bool mLPLHStatus = false;
        bool mSPLHStatus = false;
        bool mDPLHStatus = false;
        bool mSSStatus = false; //silky scrolls
        bool mSSV2Status = false;
        bool mAttachHintLLStatus = false;
        std::unordered_map<uint32_t, HintActionInfo> mExtHandlers;
        std::unordered_map<int32_t, uint32_t> mIgnoreHints;
        PerfConfigDataStore &mPerfDataStore;

    private:
        HintExtHandler();
        HintExtHandler(HintExtHandler const&);
        HintExtHandler& operator=(HintExtHandler const&);

    public:
        ~HintExtHandler();
        static HintExtHandler &getInstance() {
            static HintExtHandler mHintExtHandler;
            return mHintExtHandler;
        }

        void Reset();
        bool Register(uint32_t hintId, HintExtAction preAction, HintExtAction postAction,
                        HintExtAction hintExcluder = nullptr);
        int32_t FetchConfigPreAction(mpctl_msg_t *pMsg);
        int32_t FetchConfigPostAction(mpctl_msg_t *pMsg);
        int32_t FetchHintExcluder(mpctl_msg_t *pMsg);
        bool getLGLStatus() { return mLGLStatus;};
        bool getLALStatus() { return mLALStatus;};
        bool getLPLHStatus() { return mLPLHStatus;};
        bool getSSStatus() { return mSSStatus;};
        bool getSSV2Status() { return mSSV2Status;};
        bool getSPLHStatus() { return mSPLHStatus;};
        bool getDPLHStatus() { return mDPLHStatus;};
        bool getAttachHintLLStatus() { return mAttachHintLLStatus;};
        int64_t getCurrentTime();
        void setIgnoreHint(int32_t HintID, bool ignore);
        bool checkToIgnore(int32_t HintID);
};

class DisplayEarlyWakeupAction {
    public:
        static int32_t DisplayEarlyWakeupPreAction(mpctl_msg_t *pMsg);
};

class DisplayAction {
    public:
        static int DisplayPostAction(mpctl_msg_t *pMsg);
};

//pre/post actions of modules
class TaskBoostAction  {
    public:
        static int32_t TaskBoostPostAction(mpctl_msg_t *pMsg);
};

class LaunchBoostAction  {
    public:
        static int32_t LaunchBoostPostAction(mpctl_msg_t *pMsg);
        static int32_t LaunchBoostHintExcluder(mpctl_msg_t *pMsg);
};

class ScrollBoostAction  {
    public:
        static int32_t ScrollBoostHintExcluder(mpctl_msg_t *pMsg);
};

#define CURRENT_FPS_FILE "/data/vendor/perfd/current_fps"
class FpsUpdateAction {
    private:
        int32_t mCurFps;
        int32_t mHandle;
        int32_t mFpsHandleToRelease;
        std::mutex mMutex;
        float mFpsSwitchHystTime;
        timer_t mTimer;
        bool mFpsTimerCreated;
        uint32_t mFpsValAfterTimerExpires;
        int64_t timeGoLow = 0;
        int64_t timeGoHigh = 0;

    private:
        FpsUpdateAction();
        FpsUpdateAction(FpsUpdateAction const&);
        FpsUpdateAction& operator=(FpsUpdateAction const&);
        ~FpsUpdateAction();

        void StandardizeFps(int32_t &fps);

    public:
        static FpsUpdateAction& getInstance() {
            static FpsUpdateAction mInstance;
            return mInstance;
        }
        static int32_t FpsUpdatePreAction(mpctl_msg_t *pMsg);
        static int32_t FpsUpdatePostAction(mpctl_msg_t *pMsg);
        float getFpsSwitchHystTimeSecs();
        void SetFps();
        int32_t GetFps() {
            return mCurFps;
        }
        int32_t createFpsSwitchTimer();
        int32_t releaseAction(int32_t handle);
        static void switchToNewFps(sigval_t pvData);
        int32_t CreateTimer(int32_t req_handle);
        int32_t SetTimer();
        int32_t DeleteTimer();
        void saveFpsHintHandle(int32_t handle, uint32_t hint_id);
        enum fps {
            FPS_MIN_LIMIT = 10,
            FPS30 = 30,
            FPS30_MAX_RANGE = 37,
            FPS45 = 45,
            FPS45_MAX_RANGE = 52,
            FPS60 = 60,
            FPS60_MAX_RANGE = 65,
            FPS75 = 75,
            FPS75_MAX_RANGE = 85,
            FPS90 = 90,
            FPS90_MAX_RANGE = 105,
            FPS120 = 120,
            FPS120_MAX_RANGE = 132,
            FPS144 = 144,
            FPS144_MAX_RANGE = 162,
            FPS180 = 180,
            FPS180_MAX_RANGE = 210,
            FPS240 = 240,
            FPS_MAX_LIMIT = 260
        };
};

class CPUFreqAction {
    public:
        static int32_t CPUFreqPostAction(mpctl_msg_t *pMsg);
};

#define HWC_PID_FILE "/data/vendor/perfd/hwc_pid"
#define SF_PID_FILE "/data/vendor/perfd/sf_pid"
#define RE_TID_FILE "/data/vendor/perfd/re_tid"
#define RT_SOFT_AFFINITY_FILE "/proc/sys/walt/task_reduce_affinity"
class LargeComp {
    public:
        static int32_t LargeCompPreAction(mpctl_msg_t *pMsg);
};
class StorePID {
    public:
        static int32_t StorePIDPreAction(mpctl_msg_t *pMsg);
};

class PerfInitAction {
    public:
        static int32_t PerfInitCheckHintExcluder(mpctl_msg_t *) {
            return SUCCESS;
        }
};
#endif /*__HINT_EXT_HANDLER__H_*/
