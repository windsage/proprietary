/******************************************************************************
  @file    HintExtHandler.cpp
  @brief   Implementation of hint extensions

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG    "ANDR-PERF-HINTEXT"

#include "HintExtHandler.h"
#include <pthread.h>
#include "client.h"
#include "OptsData.h"
#include "PerfThreadPool.h"
#include "OptsHandlerExtn.h"
#include "PerfLog.h"

using namespace std;

HintExtHandler::HintExtHandler():mPerfDataStore(PerfConfigDataStore::getPerfDataStore()) {
    Reset();
    char propVal[PROP_VAL_LENGTH] = {0};
    char * retVal = mPerfDataStore.GetProperty("ro.vendor.perf.lal", propVal, sizeof(propVal));
    if (retVal != NULL) {
        mLALStatus = !strncmp(propVal,"true", PROP_VAL_LENGTH);
    }
    memset(propVal,0,sizeof(propVal));
    retVal = mPerfDataStore.GetProperty("ro.vendor.perf.lgl", propVal, sizeof(propVal));
    if (retVal != NULL) {
        mLGLStatus = !strncmp(propVal,"true", PROP_VAL_LENGTH);
    }
    memset(propVal,0,sizeof(propVal));
    retVal = mPerfDataStore.GetProperty("ro.vendor.perf.lplh", propVal, sizeof(propVal));
    if (retVal != NULL) {
        mLPLHStatus = !strncmp(propVal,"true", PROP_VAL_LENGTH);
    }

    memset(propVal,0,sizeof(propVal));
    retVal = mPerfDataStore.GetProperty("ro.vendor.perf.splh", propVal, sizeof(propVal));
    if (retVal != NULL) {
        mSPLHStatus = !strncmp(propVal,"true", PROP_VAL_LENGTH) ||
                      !strncmp(propVal,"hw", PROP_VAL_LENGTH);
    }

    memset(propVal,0,sizeof(propVal));
    retVal = mPerfDataStore.GetProperty("ro.vendor.perf.ss", propVal, sizeof(propVal));
    if (retVal != NULL) {
        mSSStatus = !strncmp(propVal,"true", PROP_VAL_LENGTH);
    }

    memset(propVal,0,sizeof(propVal));
    retVal = mPerfDataStore.GetProperty("ro.vendor.perf.ssv2", propVal, sizeof(propVal));
    if (retVal != NULL) {
        mSSV2Status = !strncmp(propVal,"true", PROP_VAL_LENGTH);
    }

    memset(propVal,0,sizeof(propVal));
    retVal = mPerfDataStore.GetProperty("ro.vendor.perf.dplh", propVal, sizeof(propVal));
    if (retVal != NULL) {
        mDPLHStatus = !strncmp(propVal,"true", PROP_VAL_LENGTH);
    }

    memset(propVal,0,sizeof(propVal));
    retVal = mPerfDataStore.GetProperty("ro.vendor.perf.attach.hint.ll", propVal, sizeof(propVal));
    if (retVal != NULL) {
        mAttachHintLLStatus = !strncmp(propVal,"true", PROP_VAL_LENGTH);
    }
}

HintExtHandler::~HintExtHandler() {
    Reset();
}

bool HintExtHandler::Register(uint32_t hintId, HintExtAction preAction, HintExtAction postAction,
                    HintExtAction hintExcluder) {
    lock_guard<mutex> lock(mMutex);
    if (mNumHandlers >= MAX_HANDLERS) {
        //no more registrations
        return false;
    }

    mExtHandlers[hintId] = {hintId, preAction, postAction, hintExcluder};
    mNumHandlers++;
    return true;
}

int32_t HintExtHandler::FetchConfigPreAction(mpctl_msg_t *pMsg) {
    int32_t retVal = 0;
    if (NULL == pMsg) {
        return retVal;
    }
    try {
        lock_guard<mutex> lock(mMutex);
        auto it = mExtHandlers.find(pMsg->hint_id);
        if(it != mExtHandlers.end() && (it->second).mPreAction) {
            retVal = (it->second).mPreAction(pMsg);
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return retVal;
}

int32_t HintExtHandler::FetchConfigPostAction(mpctl_msg_t *pMsg) {
    if (NULL == pMsg) {
        return FAILED;
    }
    try {
        lock_guard<mutex> lock(mMutex);
        auto it = mExtHandlers.find(pMsg->hint_id);
        if(it != mExtHandlers.end() && (it->second).mPostAction) {
            (it->second).mPostAction(pMsg);
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return SUCCESS;
}
int32_t HintExtHandler::FetchHintExcluder(mpctl_msg_t *pMsg) {
    int8_t retval = FAILED;
    if (!pMsg) {
        return SUCCESS;
    }
    try {
        lock_guard<mutex> lock(mMutex);
        auto it = mExtHandlers.find(pMsg->hint_id);
        if(it != mExtHandlers.end() && (it->second).mHintExcluder) {
            retval = (it->second).mHintExcluder(pMsg);
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return retval;
}

/*
* Add/Remove a Hint to/from mIgnoreHints based on ignore flag
* mIgnoreHints is a map of <HintID, counter>
* The counter indicates that how many active Hints are setting this HintID to ignore
*/
void HintExtHandler::setIgnoreHint(int32_t HintID, bool ignore) {
    if (ignore) { //Increment the ignore counter for HintID
        mIgnoreHints[HintID]++;
    } else { // Decrement the ignore counter for HintID
        if (mIgnoreHints.find(HintID) != mIgnoreHints.end()) {
            mIgnoreHints[HintID]--;
        }
    }
    QLOGL(LOG_TAG, QLOG_L2, "IgnoreHint HintId:%x ignore count:%" PRIu32 " ignore flag:%s",
                                        HintID, mIgnoreHints[HintID], ignore ? "true": "false");
}

/* Check if the hint is to be ignored or not*/
bool HintExtHandler::checkToIgnore(int32_t HintID) {
    return ((mIgnoreHints.find(HintID) != mIgnoreHints.end()) && (mIgnoreHints[HintID] >= 1));
}

void HintExtHandler::Reset() {
    lock_guard<mutex> lock(mMutex);
    mNumHandlers = 0;

    mExtHandlers.clear();
    return;
}

//hint extension actions by modules
/**taskboost's fecth config post action
 * since perfhint attach_application used the param slot which is designed to pass timeout to pass process id
 * so for this perfhint, we also use the timeout defined in xml
 */
int32_t TaskBoostAction::TaskBoostPostAction(mpctl_msg_t *pMsg) {
    if (NULL == pMsg) {
        return FAILED;
    }

    uint16_t size = pMsg->data;
    uint16_t taskboostCount = 0;

    if (size == 0){
        return FAILED;
    }

    if (size > MAX_ARGS_PER_REQUEST)
        size = MAX_ARGS_PER_REQUEST;

    if (pMsg->hint_id == VENDOR_HINT_BOOST_RENDERTHREAD && pMsg->pl_time > 0) {
        renderThreadTidOfTopApp = pMsg->pl_time;
        for (uint16_t i = 0; i < size-1; i = i + 2) {
            if(pMsg->pl_args[i] == MPCTLV3_SCHED_ENABLE_TASK_BOOST_RENDERTHREAD)
            {
                QLOGL(LOG_TAG, QLOG_L2, "renderThreadTidOfTopApp:%" PRId32 ", currentFPS:%" PRId32,
                       renderThreadTidOfTopApp, FpsUpdateAction::getInstance().GetFps());
                if(FpsUpdateAction::getInstance().GetFps() < FpsUpdateAction::getInstance().FPS144)
                {
                    pMsg->pl_args[i] = MPCTLV3_SCHED_DISABLE_TASK_BOOST_RENDERTHREAD;
                }
                pMsg->pl_args[i+1] = pMsg->pl_time;
                break;
            }
        }
        pMsg->pl_time = -1;
    }

    if (pMsg->hint_id == VENDOR_HINT_SCROLL_BOOST || pMsg->hint_id == VENDOR_HINT_DRAG_BOOST) {
        for (uint16_t i = 0; i < size - 1; i = i + 2) {
           if (pMsg->pl_args[i] == MPCTLV3_SCHED_TASK_BOOST) {
                OptsData &d = OptsData::getInstance();
                int16_t req = pMsg->pl_args[i+1];
                if (taskboostCount == 0) {
                    pMsg->pl_args[i+1] = req << 16 | d.appPid;
                } else if (taskboostCount > 0) {
                    pMsg->pl_args[i+1] = req << 16 | d.renderTid;
                }
                taskboostCount++;
           }
        }
        return SUCCESS;
    }

    if(pMsg->hint_type == LAUNCH_TYPE_ATTACH_APPLICATION) {
        for (uint16_t i = 0; i < size - 1; i = i + 2) {
           if (pMsg->pl_args[i] == MPCTLV3_SCHED_TASK_BOOST) {
                pMsg->pl_args[i+1] = pMsg->pl_time;
                break;
           }
        }
        pMsg->pl_time = -1;
    }

    if (pMsg->hint_type == LAUNCH_LL_DEFAULT || pMsg->hint_type == LAUNCH_ACTIVITY ||
        pMsg->hint_type == LAUNCH_BASE_BOOST_ATTACH_APP_LL) {
        for (uint16_t i = 0; i < size - 1; i = i + 2) {
            if (pMsg->pl_args[i] == MPCTLV3_SCHED_TASK_BOOST) {
                pMsg->pl_args[i+1] = pMsg->app_pid;
                break;
            }
        }
    }

    return SUCCESS;
}

int32_t LaunchBoostAction::LaunchBoostPostAction(mpctl_msg_t *pMsg) {
    if (!pMsg) {
        return FAILED;
    }
    uint16_t size = pMsg->data;
    if ((size == 0) || (size > MAX_ARGS_PER_REQUEST) || (size & 1))
        return FAILED;
    auto &extHandler = HintExtHandler::getInstance();
    //setting cpu freq value to minimum in case lPLH is enabled - only for games
    if (extHandler.getLPLHStatus() &&
        (pMsg->hint_type == LAUNCH_BOOST_V1 ||
        pMsg->hint_type == LAUNCH_RESERVED_2 ||
        pMsg->hint_type == LAUNCH_LL_DEFAULT ||
        pMsg->hint_type == LAUNCH_LAL ||
        pMsg->hint_type == LAUNCH_LGL)) {
        for (uint16_t i = 0; i < size-1; i = i + 2) {
            if (pMsg->pl_args[i] == MPCTLV3_MIN_FREQ_CLUSTER_BIG_CORE_0 ||
                pMsg->pl_args[i] == MPCTLV3_MIN_FREQ_CLUSTER_PRIME_CORE_0 ||
                pMsg->pl_args[i] == MPCTLV3_MIN_FREQ_CLUSTER_LITTLE_CORE_0) {
                pMsg->pl_args[i+1] = 0;
            }
        }
    }
    return TaskBoostAction::TaskBoostPostAction(pMsg);
}

int32_t LaunchBoostAction::LaunchBoostHintExcluder(mpctl_msg_t *pMsg) {
    int32_t retval = FAILED;
    if (!pMsg ) {
        return SUCCESS;
    }
    if (pMsg->hint_type == LAUNCH_LL_DEFAULT || pMsg->hint_type == LAUNCH_ACTIVITY) {
        return retval;
    }
    auto &extHandler = HintExtHandler::getInstance();
    bool isAttachHintLLEnabled = extHandler.getAttachHintLLStatus();
    if(pMsg->hint_type == LAUNCH_BASE_BOOST_ATTACH_APP_LL) {
        return isAttachHintLLEnabled ? FAILED : SUCCESS;
    } else if (pMsg->hint_type == LAUNCH_TYPE_ATTACH_APPLICATION) {
        return isAttachHintLLEnabled ? SUCCESS : FAILED;
    }
    if (extHandler.getLGLStatus() && extHandler.getLALStatus()) {
        if (pMsg->hint_type != LAUNCH_LGL && pMsg->hint_type != LAUNCH_LAL) {
            retval = SUCCESS;
        }
    }
    else if (pMsg->app_workload_type == GAME && extHandler.getLGLStatus() &&
             pMsg->hint_type != LAUNCH_LGL) {
        retval = SUCCESS;
    }
    else if (pMsg->app_workload_type != GAME && extHandler.getLALStatus() &&
             pMsg->hint_type != LAUNCH_LAL) {
        retval = SUCCESS;
    }
    else if (pMsg->hint_type == LAUNCH_TYPE_ATTACH_APPLICATION) {
        retval = SUCCESS;
    }

    return retval;
}

int32_t DisplayEarlyWakeupAction::DisplayEarlyWakeupPreAction(mpctl_msg_t *pMsg) {
    if (NULL == pMsg) {
        return FAILED;
    }

    int32_t hintType = pMsg->hint_type;
    if (hintType == 0xFFFF) {
        hintType = 0xFFFFFFFF;
    }
    OptsData &d = OptsData::getInstance();
    d.setEarlyWakeupDispId(hintType);

    // Just set the hint type to 0
    pMsg->hint_type = 0;

    QLOGL(LOG_TAG, QLOG_L2, "drmIOCTL DisplayEarlyWakeupPreAction hintid: %" PRIu32 ", hint_type: %" PRId32 ", \
            displayId: %" PRIu32, pMsg->hint_id, pMsg->hint_type, d.getEarlyWakeupDispId());
    return SUCCESS;
}

int DisplayAction::DisplayPostAction(mpctl_msg_t *pMsg){
    if(NULL == pMsg){
        return FAILED;
    }
    pMsg->pl_time = 0;
    return SUCCESS;
}

FpsUpdateAction::FpsUpdateAction() {
    mCurFps = 0;
    mHandle = -1;
    mFpsHandleToRelease = -1;
    mTimer = 0;
    mFpsTimerCreated = false;
    mFpsValAfterTimerExpires = 0;

    char fpsSwitchHystTimeProperty[PROPERTY_VALUE_MAX];
    strlcpy(fpsSwitchHystTimeProperty,
            perf_get_prop("vendor.perf.fps_switch_hyst_time_secs","0").value,PROPERTY_VALUE_MAX);
    fpsSwitchHystTimeProperty[PROPERTY_VALUE_MAX-1]='\0';
    mFpsSwitchHystTime = strtod(fpsSwitchHystTimeProperty, NULL);
    QLOGL(LOG_TAG, QLOG_L2, "fpsSwitchHystTime: %.2f secs", mFpsSwitchHystTime);
    if(mFpsSwitchHystTime < FPS_SWITCH_MIN_HYST_TIME_SECS) {
        QLOGE(LOG_TAG, "FPS Switch Hysteresis time < %" PRId8 " secs, ignore it", FPS_SWITCH_MIN_HYST_TIME_SECS);
    }
}

FpsUpdateAction::~FpsUpdateAction() {
    mCurFps = 0;
    mHandle = -1;
    mFpsHandleToRelease = -1;
    mFpsSwitchHystTime = 0;
}

float FpsUpdateAction::getFpsSwitchHystTimeSecs() {
    char fpsSwitchHystTimeProperty[PROPERTY_VALUE_MAX];
    float hystTimeSec = 0.0f;

    OptsData &d = OptsData::getInstance();
    float dFpsHystTime = d.get_fps_hyst_time();
    if(dFpsHystTime != -1.0f) {
        hystTimeSec = dFpsHystTime;
    }
    else {
        hystTimeSec = mFpsSwitchHystTime;
    }
    return hystTimeSec;
}

//If fps value sent by AOSP is not standard, then standardize it as below
void FpsUpdateAction::StandardizeFps(int32_t &fps) {
    if (fps <= FPS30_MAX_RANGE) {
        fps = FPS30;
    }
    else if (fps <= FPS45_MAX_RANGE) {
        fps = FPS45;
    }
    else if (fps <= FPS60_MAX_RANGE) {
        fps = FPS60;
    }
    else if (fps <= FPS75_MAX_RANGE) {
        fps = FPS75;
    }
    else if (fps <= FPS90_MAX_RANGE) {
        fps = FPS90;
    }
    else if (fps <= FPS120_MAX_RANGE) {
        fps = FPS120;
    }
    else if (fps <= FPS144_MAX_RANGE) {
        fps = FPS144;
    }
    else if (fps <= FPS180_MAX_RANGE) {
        fps = FPS180;
    }
    else if (fps <= FPS_MAX_LIMIT) {
        fps = FPS240;
    }
}

int32_t FpsUpdateAction::releaseAction(int32_t handle) {
    int32_t rc = FAILED;
    bool resized = false;
    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool();
    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();
    do {
        try {
            rc = ptp.placeTask([=]() {
                    QLOGL(LOG_TAG, QLOG_L3, "Offloading CallHintReset from %s", __func__);
                    perf_lock_rel(handle);
                    });
        } catch (std::exception &e) {
            QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE(LOG_TAG, "Exception caught in %s", __func__);
        }
        if (rc == FAILED && resized == false) {
            uint32_t new_size = ptp.resize(1);
            QLOGL(LOG_TAG, QLOG_WARNING, "Failed to Offload CallHintReset from %s\n ThreadPool resized to : %" PRIu32,__func__ , new_size);
            resized = true;
        } else {
            break;
        }
    } while(true);
    if (rc == SUCCESS) {
        pFpsUpdateObj.mHandle = -1;
        QLOGL(LOG_TAG, QLOG_L3, "Offloaded new thread from %s", __func__);
    } else {
        QLOGE(LOG_TAG, "FAILED to offload from %s", __func__);
        QLOGE(LOG_TAG, "%" PRId32 " FPS release Failed for Handle %" PRId32, pFpsUpdateObj.mCurFps, handle);
    }
    return rc;
}

void FpsUpdateAction::switchToNewFps(sigval_t) {
    uint32_t locFps = 0;
    int32_t handle = -1;
    int32_t rc = FAILED;
    bool resized = false;
    int32_t tmpHandle = -1;
    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();
    QLOGL(LOG_TAG, QLOG_WARNING, "Timer expired, switch to new fps: %" PRIu32, pFpsUpdateObj.mFpsValAfterTimerExpires);
    {
        if (pFpsUpdateObj.timeGoHigh > pFpsUpdateObj.timeGoLow) {
            QLOGL(LOG_TAG, QLOG_L1, "There is a new higher fps to come, so ignore the old lower fps hint.");
            pFpsUpdateObj.DeleteTimer();
            return;
        }

        lock_guard<mutex> lock(pFpsUpdateObj.mMutex);
        locFps = static_cast<float>(pFpsUpdateObj.mFpsValAfterTimerExpires);
        pFpsUpdateObj.mCurFps = pFpsUpdateObj.mFpsValAfterTimerExpires;
        if (pFpsUpdateObj.mHandle > 0) {
            rc = pFpsUpdateObj.releaseAction(pFpsUpdateObj.mHandle);
        }
    }
    set_fps_file(locFps);
    if (rc == SUCCESS) {
        handle = perf_hint(VENDOR_HINT_FPS_IMMEDIATE_UPDATE, nullptr, 0, (uint32_t)locFps);
        if (pFpsUpdateObj.mHandle > 0) {
            pFpsUpdateObj.releaseAction(pFpsUpdateObj.mHandle);
            locFps = static_cast<float> (pFpsUpdateObj.mCurFps);
            set_fps_file(locFps);
        }
        {
            lock_guard<mutex> lock(pFpsUpdateObj.mMutex);
            pFpsUpdateObj.mHandle = handle;
        }
    }
    QLOGL(LOG_TAG, QLOG_L1, "Switched to %u fps successfully, saved fps hint hyst handle : %" PRId32, locFps, handle);
    pFpsUpdateObj.DeleteTimer();
}

int32_t FpsUpdateAction::CreateTimer(int32_t req_handle) {
    int32_t rc = FAILED;
    struct sigevent sigEvent;
    sigEvent.sigev_notify = SIGEV_THREAD;
    sigEvent.sigev_notify_function = &FpsUpdateAction::switchToNewFps;
    sigEvent.sigev_notify_attributes = NULL;
    sigEvent.sigev_value.sival_int = req_handle;
    rc = timer_create(CLOCK_MONOTONIC, &sigEvent, &mTimer);
    if (rc != 0) {
        QLOGE(LOG_TAG, "Failed to create timer");
        return rc;
    }
    QLOGL(LOG_TAG, QLOG_L2, "Created fps switch timer");
    mFpsTimerCreated = true;
    return rc;
}

int32_t FpsUpdateAction::SetTimer() {
    int32_t rc = FAILED;
    struct itimerspec mTimeSpec;
    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();
    float hystTimeSec = pFpsUpdateObj.getFpsSwitchHystTimeSecs();
    mTimeSpec.it_value.tv_sec = hystTimeSec;
    mTimeSpec.it_value.tv_nsec = 0;
    mTimeSpec.it_interval.tv_sec = 0;
    mTimeSpec.it_interval.tv_nsec = 0;
    if (mFpsTimerCreated) {
        rc = timer_settime(mTimer, 0, &mTimeSpec, NULL);
        if (rc != 0) {
            QLOGE(LOG_TAG, "Failed to set timer, rc:%" PRId32, rc);
        }
        else {
            QLOGL(LOG_TAG, QLOG_L1, "Fps Switch Timer %0.2f secs set", hystTimeSec);
        }
    }
    return rc;
}

int32_t FpsUpdateAction::DeleteTimer() {
    int32_t rc = FAILED;
    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();
    pFpsUpdateObj.mFpsValAfterTimerExpires = 0;
    if (mFpsTimerCreated) {
        mFpsTimerCreated = false;
        rc = timer_delete(mTimer);
        if (rc != 0) {
            QLOGE(LOG_TAG, "Failed to delete timer, rc:%" PRId32, rc);
        }
    }
    else {
        QLOGL(LOG_TAG, QLOG_L3, "mFpsTimerCreated is false, so deleteTimer is NA");
    }
    return rc;
}

int64_t getCurrentTime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    int64_t timestamp = (int64_t)ts.tv_sec * 1000000000LL + (int64_t)ts.tv_nsec;
    return timestamp;
}

int32_t FpsUpdateAction::createFpsSwitchTimer() {
    int32_t rc = FAILED;
    int32_t req_handle = 1;
    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();
    pFpsUpdateObj.timeGoLow = getCurrentTime();
    if (mFpsTimerCreated == false) {
        rc = pFpsUpdateObj.CreateTimer(req_handle);
    }
    if(mFpsTimerCreated == true) {
        rc = pFpsUpdateObj.SetTimer();
        if(rc != 0) {
            QLOGE(LOG_TAG, "setTimer unsuccessful, hence deleting timer");
            pFpsUpdateObj.DeleteTimer();
        }
    }
    return rc;
}

int32_t FpsUpdateAction::FpsUpdatePreAction(mpctl_msg_t *pMsg) {
    float locFps = 0;
    int32_t rc = FAILED;
    bool resized = false;
    int32_t tmpHandle = -1;

    if (NULL == pMsg) {
        QLOGE(LOG_TAG, "FpsUpdatePreAction: pMsg is NULL");
        return PRE_ACTION_NO_FPS_UPDATE;
    }
    pMsg->offloadFlag = false;

    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();

    if (pMsg->hint_type < FPS_MIN_LIMIT || pMsg->hint_type > FPS_MAX_LIMIT) {
        QLOGE(LOG_TAG, "FPS Update for values < %" PRId8 " & > %" PRId8 "  are unsupported",
                FPS_MIN_LIMIT, FPS_MAX_LIMIT);
        return PRE_ACTION_NO_FPS_UPDATE;
    }
    QLOGL(LOG_TAG, QLOG_L2, "ORIGINAL hintid: 0x%" PRIu32 ", new fps: %" PRId32, pMsg->hint_id, pMsg->hint_type);
    TargetConfig &tc = TargetConfig::getTargetConfig();
    int32_t fps = tc.getMinFpsTuning();
    if (fps > pMsg->hint_type)
        pMsg->hint_type = fps;
    pFpsUpdateObj.StandardizeFps(pMsg->hint_type);
    QLOGL(LOG_TAG, QLOG_WARNING, "STANDARDIZED hintid: 0x%" PRIu32 " , curr fps: %" PRId32 ", new fps: %" PRId32,
           pMsg->hint_id, pFpsUpdateObj.mCurFps,pMsg->hint_type);
    float hystTimeSec = pFpsUpdateObj.getFpsSwitchHystTimeSecs();
    if (hystTimeSec >= FPS_SWITCH_MIN_HYST_TIME_SECS) {
        pFpsUpdateObj.DeleteTimer();
        if (pMsg->hint_type < pFpsUpdateObj.GetFps()) {
            pFpsUpdateObj.mFpsValAfterTimerExpires = pMsg->hint_type;
            rc = pFpsUpdateObj.createFpsSwitchTimer();
            if(rc == 0) {
                QLOGL(LOG_TAG, QLOG_L2, "Created Fps Switch Timer. Do not act on this lower FPS notfn from SF right away.");
                return PRE_ACTION_NO_FPS_UPDATE;
            }
        }
    }
    if (pMsg->hint_type == pFpsUpdateObj.mCurFps) {
        QLOGL(LOG_TAG, QLOG_L2, "New fps = curr fps = %" PRId32 ", don't do anything", pFpsUpdateObj.mCurFps);
        return PRE_ACTION_NO_FPS_UPDATE;
    }

    QLOGL(LOG_TAG, QLOG_L1, "New fps rcvd from SF, %" PRId32 " will be updated now.", pMsg->hint_type);
    {
        lock_guard<mutex> lock(pFpsUpdateObj.mMutex);
        pFpsUpdateObj.timeGoHigh = getCurrentTime();
        if (pFpsUpdateObj.mHandle > 0) {
            pFpsUpdateObj.releaseAction(pFpsUpdateObj.mHandle);
        }
        pFpsUpdateObj.mCurFps = pMsg->hint_type;
        locFps = static_cast<float> (pFpsUpdateObj.mCurFps);
    }
    set_fps_file(locFps);
    return SUCCESS;
}

int32_t FpsUpdateAction::FpsUpdatePostAction(mpctl_msg_t *pMsg) {
    if (NULL == pMsg) {
        return FAILED;
    }
    uint16_t size = pMsg->data;

    if (size == 0){
        return FAILED;
    }

    if (size > MAX_ARGS_PER_REQUEST)
        size = MAX_ARGS_PER_REQUEST;

    if(pMsg->hint_type >= FPS144) {
        for (uint16_t i = 0; i < size-1; i = i + 2) {
            if (pMsg->pl_args[i] == MPCTLV3_SCHED_ENABLE_TASK_BOOST_RENDERTHREAD) {
                QLOGL(LOG_TAG, QLOG_L2, "FPS from FW:%" PRId32 ", apply task boost on top-app renderThreadTid:%" PRId32,
                       pMsg->hint_type, renderThreadTidOfTopApp);
                pMsg->pl_args[i+1] = renderThreadTidOfTopApp;
            }
        }
    }
    else if(pMsg->hint_type < FPS144) {
        for (uint16_t i = 0; i < size-1; i = i + 2) {
            if (pMsg->pl_args[i] == MPCTLV3_SCHED_DISABLE_TASK_BOOST_RENDERTHREAD) {
                QLOGL(LOG_TAG, QLOG_L2, "FPS from FW:%" PRId32 ", disable task boost on top-app renderThreadTid:%" PRId32,
                       pMsg->hint_type, renderThreadTidOfTopApp);
                pMsg->pl_args[i+1] = renderThreadTidOfTopApp;
            }
        }
    }
    pMsg->pl_time = INT_MAX;
    return SUCCESS;
}

void FpsUpdateAction::SetFps() {
    int32_t fps = 0, handle = -1;
    fps = static_cast<int32_t> (get_fps_file());
    handle = perf_hint(VENDOR_HINT_FPS_UPDATE, nullptr, 0, fps);
}

void FpsUpdateAction::saveFpsHintHandle(int32_t handle, uint32_t hint_id) {
    if(hint_id == VENDOR_HINT_FPS_UPDATE) {
        {
            lock_guard<mutex> lock(mMutex);
            if (mHandle > 0) {
                FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();
                pFpsUpdateObj.releaseAction(pFpsUpdateObj.mHandle);
                float locFps = static_cast<float> (pFpsUpdateObj.mCurFps);
                set_fps_file(locFps);
            }
            mHandle = handle;
        }
        QLOGL(LOG_TAG, QLOG_L3, "Saved new fps update handle = %" PRId32, handle);
    }
}

//CPUFreqPostAction updates the freq val in the hint
//from perfboostconfig to a specified value
int32_t CPUFreqAction::CPUFreqPostAction(mpctl_msg_t *pMsg) {
    TaskBoostAction::TaskBoostPostAction(pMsg);

    auto &extHandler = HintExtHandler::getInstance();
    bool sPLHStatus = false, dPLHStatus = false;

    sPLHStatus = extHandler.getSPLHStatus();
    dPLHStatus = extHandler.getDPLHStatus();
    if (!sPLHStatus && !dPLHStatus) {
        return FAILED;
    }
    if (NULL == pMsg) {
        return FAILED;
    }
    uint16_t size = pMsg->data;
    if ((size == 0) || (size > MAX_ARGS_PER_REQUEST) || (size & 1))
        return FAILED;

    //interested in only vertical, horizontal scroll and drag
    bool scrollExcluder = (sPLHStatus && (pMsg->hint_id == VENDOR_HINT_SCROLL_BOOST) &&
                          ((pMsg->hint_type == 1) || (pMsg->hint_type == 2) ||
                           (pMsg->hint_type == 5) || (pMsg->hint_type == 6) ||
                           (pMsg->hint_type == 7)));
    bool dragExcluder = (dPLHStatus && (pMsg->hint_id == VENDOR_HINT_DRAG_BOOST) &&
                         (pMsg->hint_type == 1));

    if (scrollExcluder || dragExcluder) {
        for (uint16_t i = 0; i < size-1; i = i + 2) {
            if (pMsg->pl_args[i] == MPCTLV3_MIN_FREQ_CLUSTER_BIG_CORE_0 ||
                pMsg->pl_args[i] == MPCTLV3_MIN_FREQ_CLUSTER_PRIME_CORE_0) {
                //setting freq value to minimum in case feature is enabled
                pMsg->pl_args[i+1] = 0;
            }
        }
        return SUCCESS;
    }
    return FAILED;
}

//ScrollBoostHintExcluder Returns Exclusion status as SUCCESS for traditional boosts if SS enabled
int32_t ScrollBoostAction::ScrollBoostHintExcluder(mpctl_msg_t *pMsg) {
    auto &extHandler = HintExtHandler::getInstance();
    bool ssEnabled = extHandler.getSSStatus();
    bool ssv2Enabled = extHandler.getSSV2Status();

    bool excludeScrolls = (ssEnabled || ssv2Enabled);

    if (NULL == pMsg) {
        return SUCCESS;
    }

    // Exclude vertical and horizontal scroll boosts when ss or ssv2 is enabled
    if (excludeScrolls && pMsg->hint_id == VENDOR_HINT_SCROLL_BOOST && ((pMsg->hint_type == 1) ||
       (pMsg->hint_type == 2))) {
        return SUCCESS;
    }

    // Exclude pre-fling boost when ssv2 is enabled
    if (ssv2Enabled && pMsg->hint_id == VENDOR_HINT_SCROLL_BOOST && pMsg->hint_type == 4) {
        return SUCCESS;
    }

    // Exclude drag boost when ssv2 is enabled
    if (ssv2Enabled && pMsg->hint_id == VENDOR_HINT_DRAG_BOOST && pMsg->hint_type == 1) {
        return SUCCESS;
    }

    return FAILED;
}

int32_t LargeComp::LargeCompPreAction(mpctl_msg_t *pMsg) {
    OptsData &d = OptsData::getInstance();
    FILE *pFile = NULL;
    char buf[NODE_MAX]= "";
    if (NULL == pMsg) {
        return FAILED;
    }

    pMsg->offloadFlag = false;

    if(pMsg->pl_time <= 0) {
        QLOGL(LOG_TAG, QLOG_L2, "LargeComp: HWC TID: %" PRId32 ", no change", d.hwcPid);
        //<=0 means that there is no change in HWC TID from display HAL
        return SUCCESS;
    }

    d.hwcPid = pMsg->pl_time;
    pMsg->pl_time = 0;

    pFile = fopen(HWC_PID_FILE, "w");
    if (pFile == NULL) {
        QLOGE(LOG_TAG, "Cannot open/create HWC pid file");
        return FAILED;
    }
    snprintf(buf, NODE_MAX, "%" PRId32, d.hwcPid);

    fwrite(buf, sizeof(char), strlen(buf), pFile);
    fclose(pFile);
    QLOGL(LOG_TAG, QLOG_L2, "HWC pid %" PRId32, d.hwcPid);
    return SUCCESS;
}

//Storing pid for SurfaceFlinger, RenderEngine & HWC from the hint
int32_t StorePID::StorePIDPreAction(mpctl_msg_t *pMsg) {
    OptsData &d = OptsData::getInstance();
    FILE *pFile = NULL;
    FILE *rtSoftAffinityFile = NULL;
    char buf[NODE_MAX]= "";
    char rtSoftAffinityBuf[NODE_MAX]= "";
    if (NULL == pMsg) {
        return FAILED;
    }
    QLOGL(LOG_TAG, QLOG_L3, "StorePIDPreAction hint_type=%" PRId32 ", pid=%" PRId32, pMsg->hint_type, pMsg->pl_time);
    if(pMsg->pl_time <= 0) {
        QLOGE(LOG_TAG, "Will not store PID=%" PRId32, pMsg->pl_time);
        return FAILED;
    }
    if(pMsg->hint_type == HINT_TYPE_FOR_SF_PID) {
        d.sfPid = pMsg->pl_time;
        pFile = fopen(SF_PID_FILE, "w");
        if (pFile == NULL) {
            QLOGE(LOG_TAG, "Cannot open/create SF pid file");
            return FAILED;
        }
        snprintf(buf, NODE_MAX, "%" PRId32, d.sfPid);
        fwrite(buf, sizeof(char), strlen(buf), pFile);
        fclose(pFile);
        QLOGL(LOG_TAG, QLOG_L3, "SF pid %" PRId32, d.sfPid);
        Target &t = Target::getCurTarget();
        int8_t numSilvers = t.getNumLittleCores();
        //soft affinity for SF only for CPU topology with 1-2 silvers
        //Below deterministic placement is NA for 0 silvers or > 2 silvers
        if(numSilvers < 3 && numSilvers > 0) {
            rtSoftAffinityFile = fopen(RT_SOFT_AFFINITY_FILE, "w");
            if (rtSoftAffinityFile == NULL) {
                QLOGE(LOG_TAG, "Cannot open/create softAffinity file");
                return FAILED;
            }
            else {
                uint16_t sfSoftAffinityMask = 0;
                int8_t cpu = -1, startCpu = -1, endCpu  = -1, cluster = -1;
                cpu = t.getFirstCoreOfClusterWithSpilloverRT();
                if (cpu < 0) {
                    QLOGE(LOG_TAG, "getFirstCoreOfClusterWithSpilloverRT failed");
                    fclose(rtSoftAffinityFile);
                    return FAILED;
                }
                cluster = t.getClusterForCpu(cpu, startCpu, endCpu);
                if ((startCpu < 0) || (startCpu >= MAX_CORES) || (endCpu < 0) || (endCpu >= MAX_CORES) || (cluster < 0)) {
                    QLOGE(LOG_TAG, "Could not find a cluster corresponding to the core %" PRId8, cpu);
                    fclose(rtSoftAffinityFile);
                    return FAILED;
                }
                QLOGL(LOG_TAG, QLOG_L3, "Prefer CPUs(%d-%d) for SF", startCpu, endCpu);
                for(int i=startCpu; i<=endCpu; i++) {
                    sfSoftAffinityMask += pow(2,i);
                }
                QLOGL(LOG_TAG, QLOG_L3, "sfSoftAffinityMask %" PRIu16, sfSoftAffinityMask);
                snprintf(rtSoftAffinityBuf, NODE_MAX, "%" PRIu32 " %" PRIu16, d.sfPid, sfSoftAffinityMask);
                fwrite(rtSoftAffinityBuf, sizeof(char), strlen(rtSoftAffinityBuf), rtSoftAffinityFile);
                fclose(rtSoftAffinityFile);
            }
        }
    }
    if(pMsg->hint_type == HINT_TYPE_FOR_RE_TID) {
        d.reTid = pMsg->pl_time;
        pFile = fopen(RE_TID_FILE, "w");
        if (pFile == NULL) {
            QLOGE(LOG_TAG, "Cannot open/create RE tid file");
            return FAILED;
        }
        snprintf(buf, NODE_MAX, "%" PRId32, d.reTid);
        fwrite(buf, sizeof(char), strlen(buf), pFile);
        fclose(pFile);
        QLOGL(LOG_TAG, QLOG_L2, "RE tid %" PRId32, d.reTid);
        Target &t = Target::getCurTarget();
        int8_t numSilvers = t.getNumLittleCores();
        //soft affinity for RE only for CPU topology with 1-2 silvers
        //Below deterministic placement is NA for 0 silvers or > 2 silvers
        if(numSilvers < 3 && numSilvers > 0) {
            rtSoftAffinityFile = fopen(RT_SOFT_AFFINITY_FILE, "w");
            if (rtSoftAffinityFile == NULL) {
                QLOGE(LOG_TAG, "Cannot open/create softAffinity file");
                return FAILED;
            }
            else {
                uint16_t reSoftAffinityMask = 0;
                int8_t cpu = -1, startCpu = -1, endCpu  = -1, cluster = -1;
                cpu = t.getFirstCoreOfClusterWithSpilloverRT();
                if (cpu < 0) {
                    QLOGE(LOG_TAG, "getFirstCoreOfClusterWithSpilloverRT failed");
                    fclose(rtSoftAffinityFile);
                    return FAILED;
                }
                cluster = t.getClusterForCpu(cpu, startCpu, endCpu);
                if ((startCpu < 0) || (startCpu >= MAX_CORES) || (endCpu < 0) || (endCpu >= MAX_CORES) || (cluster < 0)) {
                    QLOGE(LOG_TAG, "Could not find a cluster corresponding to the core %" PRId8, cpu);
                    fclose(rtSoftAffinityFile);
                    return FAILED;
                }
                QLOGL(LOG_TAG, QLOG_L3, "Prefer CPUs(%d-%d) for RE", startCpu, endCpu);
                for(int i=startCpu; i<=endCpu; i++) {
                    reSoftAffinityMask += pow(2,i);
                }
                QLOGL(LOG_TAG, QLOG_L3, "reSoftAffinityMask %" PRIu16, reSoftAffinityMask);
                snprintf(rtSoftAffinityBuf, NODE_MAX, "%" PRIu32 " %" PRIu16, d.reTid, reSoftAffinityMask);
                fwrite(rtSoftAffinityBuf, sizeof(char), strlen(rtSoftAffinityBuf), rtSoftAffinityFile);
                fclose(rtSoftAffinityFile);
            }
        }
    }
    if(pMsg->hint_type == HINT_TYPE_FOR_HWC_TID) {
        d.hwcPid = pMsg->pl_time;
        pFile = fopen(HWC_PID_FILE, "w");
        if (pFile == NULL) {
            QLOGE(LOG_TAG, "Cannot open/create HWC pid file");
            return FAILED;
        }
        snprintf(buf, NODE_MAX, "%" PRId32, d.hwcPid);

        fwrite(buf, sizeof(char), strlen(buf), pFile);
        fclose(pFile);
        QLOGL(LOG_TAG, QLOG_L3, "HWC pid %" PRId32, d.hwcPid);
    } else if (pMsg->hint_type == HINT_TYPE_FOR_APP_PID) {
        d.appPid = pMsg->pl_time;
    } else if (pMsg->hint_type == HINT_TYPE_FOR_RENDER_TID) {
        d.renderTid = pMsg->pl_time;
    }
    return PRE_ACTION_SF_RE_VAL;
}
