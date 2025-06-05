/******************************************************************************
  @file    Perf.cpp
  @brief   Android performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define LOG_TAG "ANDR-PERF-SERVICE"
#include <string>
#include <dlfcn.h>
#include <pthread.h>
#include <cutils/properties.h>

#include "Perf.h"
#include "PerfController.h"
#include "TargetConfig.h"
#include "PerfThreadPool.h"
#include "MpctlUtils.h"

#include "PerfLog.h"
#include <stdio.h>
#include <stdlib.h>
enum INDEX_REFERENCE {INDEX_START = -1,
                      INDEX_CLIENT_TID = 0,
                      INDEX_CLIENT_PID = 1,
                      INDEX_REQUEST_OFFLOAD_FLAG = 2,
                      INDEX_REQUEST_TYPE = 3,
                      INDEX_REQUEST_DURATION = 4,
                      INDEX_END
                      };
/*
   INTERNAL_PRIVATE_EXTN_ARGS
   INDEX_CLIENT_TID,
   INDEX_CLIENT_PID,
   INDEX_REQUEST_OFFLOAD_FLAG
 */
#define PERF_HAL_DISABLE_PROP "vendor.disable.perf.hal"
#define INIT_NOT_COMPLETED (-3)

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace perf2 {

ScopedAStatus Perf::perfLockAcquire(int32_t in_pl_handle, int32_t in_duration, const std::vector<int32_t>& in_boostsList, const std::vector<int32_t>& in_reserved, int32_t* _aidl_return) {
    *_aidl_return = -1;
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();
    }
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKACQ;
    for (uint32_t i = 0; i < INDEX_END && i < in_reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = in_reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = in_reserved[i];
                    break;
            default: QLOGL(LOG_TAG, QLOG_WARNING,  "Unknown params");
                    break;
        }
    }
    uint32_t size  = in_boostsList.size();
    if (size > MAX_ARGS_PER_REQUEST) {
       QLOGE(LOG_TAG, "Maximum number of arguments alowed exceeded");
       return ndk::ScopedAStatus::ok();;
    }
    std::copy(in_boostsList.begin(), in_boostsList.end(), pMsg.pl_args);
    pMsg.data = size;
    pMsg.pl_time = in_duration;
    pMsg.pl_handle = in_pl_handle;
    pMsg.version = MSG_VERSION;
    pMsg.size = sizeof(mpctl_msg_t);
    if (in_pl_handle > 0) {
        pMsg.renewFlag = true;
    }

    *_aidl_return = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfLockRelease(int32_t in_pl_handle, const std::vector<int32_t>& in_reserved) {
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();;
    }
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKREL;
    for (uint32_t i = 0; i <= INDEX_CLIENT_PID && i < in_reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = in_reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = in_reserved[i];
                    break;
            default: QLOGL(LOG_TAG, QLOG_WARNING,  "Unknown params");
                    break;
        }
    }
    pMsg.pl_handle = in_pl_handle;
    pMsg.version = MSG_VERSION;
    pMsg.size = sizeof(mpctl_msg_t);

    mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfHint(int32_t hint, const std::string& userDataStr, int32_t userData1, int32_t userData2, const std::vector<int32_t>& reserved, int32_t* _aidl_return) {
    *_aidl_return = -1;
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();;
    }
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKHINTACQ;
    for (uint32_t i = 0; i < INDEX_END && i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                    break;
            case INDEX_REQUEST_OFFLOAD_FLAG: pMsg.offloadFlag = reserved[i];
                    break;
            default: QLOGL(LOG_TAG, QLOG_WARNING,  "Unknown params");
                    break;
        }
    }
    pMsg.hint_id = hint;
    pMsg.pl_time = userData1;
    pMsg.hint_type = userData2;
    pMsg.version = MSG_VERSION;
    pMsg.size = sizeof(mpctl_msg_t);
    strlcpy(pMsg.usrdata_str, userDataStr.c_str(), MAX_MSG_APP_NAME_LEN);
    pMsg.usrdata_str[MAX_MSG_APP_NAME_LEN - 1] = '\0';
    *_aidl_return = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    if (*_aidl_return != INIT_NOT_COMPLETED) {
        sendPerfCallbackOffload(hint, userDataStr, userData1, userData2, reserved);
    }
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfProfile(int32_t, int32_t , int32_t, int32_t* _aidl_return) {
    QLOGE(LOG_TAG, "%s API is deprecated from perf-hal 2.3", __func__);
    *_aidl_return = -1;
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfLockCmd(int32_t cmd, int32_t reserved) {
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();
    }
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));
    uint32_t perf_hal_pid = getpid();

    pMsg.req_type = cmd;
    pMsg.client_pid = AIBinder_getCallingPid();
    pMsg.client_tid = reserved;
    pMsg.version = MSG_VERSION;
    pMsg.size = sizeof(mpctl_msg_t);
    if (perf_hal_pid != (uint32_t)pMsg.client_pid) {
        QLOGE(LOG_TAG, "%s API is deprecated from perf-hal 2.3", __func__);
        return ndk::ScopedAStatus::ok();
    }

    mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfGetProp(const std::string& propName, const std::string& defaultVal, std::string *_aidl_return) {

    if (!checkPerfStatus(__func__)) {
        *_aidl_return = defaultVal;
        return ndk::ScopedAStatus::ok();
    }
    char prop_val[PROP_VAL_LENGTH], prop_name[PROP_VAL_LENGTH];
    char trace_buf[TRACE_BUF_SZ];
    char *retVal;
    strlcpy(prop_name, propName.c_str(), PROP_VAL_LENGTH);
    retVal = mPerfDataStore.GetProperty(prop_name, prop_val, sizeof(prop_val));
    if (retVal != NULL) {
         *_aidl_return = prop_val;
         if (perf_debug_output) {
             snprintf(trace_buf, TRACE_BUF_SZ, "perfGetProp: Return Val from Perf.cpp is %s", _aidl_return->c_str());
             QLOGL(LOG_TAG, QLOG_L3, "%s", trace_buf);
         }
    } else {
         *_aidl_return = defaultVal;
    }
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfSetProp(const std::string& /*propName*/, const std::string& /*value*/, int32_t * /*_aidl_return*/) {
    // TODO implement
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfAsyncRequest(int32_t /*cmd*/, const std::string& /*userDataStr*/, const std::vector<int32_t>& /*params*/, int32_t * /*_aidl_return*/) {
    // TODO implement
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfSyncRequest(int32_t cmd, const std::string& /*userDataStr*/,
                                   const std::vector<int32_t>& /*params*/,
                                   std::string *_aidl_return) {
    if (!checkPerfStatus(__func__)) {
        *_aidl_return = "";
        return ndk::ScopedAStatus::ok();
    }
    *_aidl_return = mImpl.PerfGlueLayerSyncRequest(cmd);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfCallbackRegister(const std::shared_ptr<::aidl::vendor::qti::hardware::perf2::IPerfCallback>& callback, int32_t clientId, int32_t* _aidl_return){
    *_aidl_return = 0;
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();
    }
    if (callback == nullptr) {
        QLOGE(LOG_TAG, "Invalid callback pointer PerfHAL ");
        return ndk::ScopedAStatus::ok();
    } else {
        *_aidl_return = 1;
    }

    std::lock_guard<std::mutex> _lock(perf_callback_mutex);
    if (std::any_of( callbacks.begin(), callbacks.end(), [&](const Perfcallback& c){
            return (c.clientId == clientId);
        })) {
        *_aidl_return = 0;
        QLOGE(LOG_TAG, "Same callback interface registered already");
    } else {
        callbacks.emplace_back(callback, clientId);
        QLOGE(LOG_TAG, "A callback has been registered to PerfHAL ");
    }
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfCallbackDeregister(const std::shared_ptr<::aidl::vendor::qti::hardware::perf2::IPerfCallback>& callback, int32_t clientId, int32_t *_aidl_return){
    *_aidl_return = 0;
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();
    }
    if (callback == nullptr) {
        QLOGE(LOG_TAG, "Invalid callback pointer PerfHAL ");
        return ndk::ScopedAStatus::ok();
    } else {
        *_aidl_return = 1;
    }
    bool removed = false;
    std::lock_guard<std::mutex> _lock(perf_callback_mutex);
    callbacks.erase(
        std::remove_if(callbacks.begin(), callbacks.end(), [&](const Perfcallback& c) {
            if (c.clientId == clientId) {
                QLOGL(LOG_TAG, QLOG_WARNING, "a callback has been unregistered to PerfHAL");
                removed = true;
                //These return are for inside function remove_if
                return true;
            }
            return false;
        }), callbacks.end());
    if (!removed) {
        QLOGE(LOG_TAG, "The callback was not registered before");
    }
    return ndk::ScopedAStatus::ok();
}

void Perf::sendPerfCallback(uint32_t hint, const std::string& userDataStr, int32_t userData1, int32_t userData2, const std::vector<int32_t>& reserved) {
    std::unique_lock<std::mutex> _lock(perf_callback_mutex, std::try_to_lock);
    if (_lock.owns_lock() == false) {
        return;
    }
    for(const auto& c : callbacks){
        if(c.callback == nullptr){
            QLOGE(LOG_TAG, "Invalid callback pointer");
        } else{
            ndk::ScopedAStatus ret = c.callback->notifyCallback(hint, userDataStr, userData1, userData2, reserved);
            if(!ret.isOk()){
                QLOGE(LOG_TAG, "NotifyCallback is not called");
            }
        }
    }
}

void Perf::sendPerfCallbackOffload(uint32_t hint, const std::string& userDataStr, int32_t userData1, int32_t userData2, const std::vector<int32_t>& reserved) {

    int32_t rc = FAILED;
    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool();
    try {
        rc = ptp.placeTask([=]() {
                sendPerfCallback(hint, userDataStr, userData1, userData2, reserved);
                });
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Failed to Offload %s task", __func__);
    }
}

ScopedAStatus Perf::perfLockAcqAndRelease(int32_t pl_handle, int32_t duration, const std::vector<int32_t>& boostsList, const std::vector<int32_t>& reserved,  int32_t *_aidl_return) {
    *_aidl_return = -1;
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();
    }
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKREL;
    for (uint32_t i = 0; i <= INDEX_CLIENT_PID && i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                                   break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                                   break;
            default: QLOGL(LOG_TAG, QLOG_WARNING,  "Unknown params");
                     break;
        }
    }
    pMsg.pl_handle = pl_handle;
    pMsg.version = MSG_VERSION;
    pMsg.size = sizeof(mpctl_msg_t);

    mImpl.PerfGlueLayerSubmitRequest(&pMsg);

    memset(&pMsg, 0, sizeof(mpctl_msg_t));
    pMsg.req_type = MPCTL_CMD_PERFLOCKACQ;
    for (uint32_t i = 0; i < INDEX_END && i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                                   break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                                   break;
            default: QLOGL(LOG_TAG, QLOG_WARNING,  "Unknown params");
                     break;
        }
    }
    uint32_t size  = boostsList.size();
    if (size > MAX_ARGS_PER_REQUEST) {
        QLOGE(LOG_TAG, "Maximum number of arguments alowed exceeded");
        return ndk::ScopedAStatus::ok();
    }
    std::copy(boostsList.begin(), boostsList.end(), pMsg.pl_args);
    pMsg.data = size;
    pMsg.pl_time = duration;
    pMsg.version = MSG_VERSION;
    pMsg.size = sizeof(mpctl_msg_t);
    *_aidl_return = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfEvent(int32_t eventId, const std::string& userDataStr, const std::vector<int32_t>& reserved) {
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();
    }
    mpctl_msg_t pMsg;
    int32_t retVal = -1;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));
    pMsg.hint_id = eventId;
    pMsg.pl_time = -1;
    pMsg.hint_type = -1;
    pMsg.version = MSG_VERSION;
    pMsg.size = sizeof(mpctl_msg_t);
    int32_t argsProcessed = halPreAction(&pMsg, reserved);
    pMsg.req_type = MPCTL_CMD_PERFEVENT;
    for (uint32_t i = 0; i < INDEX_END && i < (reserved.size() - argsProcessed); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                    break;
            case INDEX_REQUEST_OFFLOAD_FLAG: pMsg.offloadFlag = reserved[i];
                    break;
            case INDEX_REQUEST_TYPE: pMsg.hint_type = reserved[i];
                    break;
            case INDEX_REQUEST_DURATION: pMsg.pl_time = reserved[i];
                    break;
            default:
                    break;
        }
    }
    strlcpy(pMsg.usrdata_str, userDataStr.c_str(), MAX_MSG_APP_NAME_LEN);
    pMsg.usrdata_str[MAX_MSG_APP_NAME_LEN - 1] = '\0';
    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    if (retVal != INIT_NOT_COMPLETED) {
        sendPerfCallbackOffload(pMsg.hint_id, userDataStr, pMsg.pl_time, pMsg.hint_type, reserved);
    }
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfGetFeedback(int32_t featureId, const std::string& userDataStr, const std::vector<int32_t>& reserved, int32_t *_aidl_return) {
    *_aidl_return = -1;
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();
    }
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFGETFEEDBACK;
    for (uint32_t i = 0; i < INDEX_END  && i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                    break;
            case INDEX_REQUEST_OFFLOAD_FLAG: pMsg.offloadFlag = reserved[i];
                    break;
            case INDEX_REQUEST_TYPE: pMsg.hint_type = reserved[i];
                    break;
            case INDEX_REQUEST_DURATION: pMsg.pl_time = reserved[i];
                    break;
            default: QLOGL(LOG_TAG, QLOG_WARNING,  "Unknown params");
                    break;
        }
    }
    pMsg.hint_id = featureId;
    pMsg.version = MSG_VERSION;
    pMsg.size = sizeof(mpctl_msg_t);
    strlcpy(pMsg.usrdata_str, userDataStr.c_str(), MAX_MSG_APP_NAME_LEN);
    pMsg.usrdata_str[MAX_MSG_APP_NAME_LEN - 1] = '\0';
    *_aidl_return = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfHintAcqRel(int32_t pl_handle, int32_t hint, const std::string& pkg_name, int32_t duration,
        int32_t hint_type, const std::vector<int32_t>& reserved, int32_t *_aidl_return) {
    *_aidl_return = -1;
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();
    }
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));
    if (pl_handle > 0) {
        pMsg.req_type = MPCTL_CMD_PERFLOCKREL;
        for (uint32_t i = 0; i <= INDEX_CLIENT_PID && i < reserved.size(); i++) {
            switch (i) {
                case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                        break;
                case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                        break;
                case INDEX_REQUEST_OFFLOAD_FLAG:
                        break;
                default: QLOGL(LOG_TAG, QLOG_WARNING,  "Unknown params");
                        break;
            }
        }
        pMsg.pl_handle = pl_handle;
        pMsg.version = MSG_VERSION;
        pMsg.size = sizeof(mpctl_msg_t);

        mImpl.PerfGlueLayerSubmitRequest(&pMsg);

        memset(&pMsg, 0, sizeof(mpctl_msg_t));
    }

    pMsg.req_type = MPCTL_CMD_PERFLOCKHINTACQ;
    for (uint32_t i = 0;i < INDEX_END && i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                    break;
            case INDEX_REQUEST_OFFLOAD_FLAG: pMsg.offloadFlag = reserved[i];
                    break;
            case INDEX_REQUEST_TYPE: pMsg.app_workload_type = reserved[i];
                    break;
            case INDEX_REQUEST_DURATION: pMsg.app_pid = reserved[i];
                    break;
            default: QLOGL(LOG_TAG, QLOG_WARNING,  "Unknown params");
                    break;
        }
    }
    pMsg.hint_id = hint;
    pMsg.pl_time = duration;
    pMsg.hint_type = hint_type;
    pMsg.version = MSG_VERSION;
    pMsg.size = sizeof(mpctl_msg_t);
    strlcpy(pMsg.usrdata_str, pkg_name.c_str(), MAX_MSG_APP_NAME_LEN);
    pMsg.usrdata_str[MAX_MSG_APP_NAME_LEN - 1] = '\0';
    *_aidl_return = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    if (*_aidl_return != INIT_NOT_COMPLETED) {
        sendPerfCallback(hint, pkg_name, duration, hint_type, reserved);
    }
    return ndk::ScopedAStatus::ok();
}


ScopedAStatus Perf::perfHintRenew(int32_t pl_handle, int32_t hint, const std::string& pkg_name, int32_t duration,
        int32_t hint_type, const std::vector<int32_t>& reserved, int32_t *_aidl_return) {
    *_aidl_return = -1;
    if (!checkPerfStatus(__func__)) {
        return ndk::ScopedAStatus::ok();
    }
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKHINTACQ;
    for (uint32_t i = 0; i < INDEX_END && i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                    break;
            default: QLOGL(LOG_TAG, QLOG_WARNING, "Unknown params");
                    break;
        }
    }
    pMsg.pl_handle = pl_handle;
    pMsg.hint_id = hint;
    pMsg.pl_time = duration;
    pMsg.hint_type = hint_type;
    pMsg.version = MSG_VERSION;
    pMsg.size = sizeof(mpctl_msg_t);
    if (pl_handle > 0) {
        pMsg.renewFlag = true;
    }
    strlcpy(pMsg.usrdata_str, pkg_name.c_str(), MAX_MSG_APP_NAME_LEN);
    pMsg.usrdata_str[MAX_MSG_APP_NAME_LEN - 1] = '\0';
    *_aidl_return = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    if (*_aidl_return != INIT_NOT_COMPLETED) {
        sendPerfCallback(hint, pkg_name, duration, hint_type, reserved);
    }
    return ndk::ScopedAStatus::ok();
}

int32_t Perf::halPreAction(mpctl_msg_t *pMsg,  const std::vector<int32_t>& reserved) {
    if (!pMsg || pMsg->hint_id != VENDOR_HINT_FD_COUNT) {
        return 0;
    }

    uint32_t size  = reserved.size();
    if (size > MAX_RESERVE_ARGS_PER_REQUEST || size <= INDEX_REQUEST_TYPE) {
        return 0;
    }

    pMsg->data = size - INDEX_REQUEST_TYPE;
    pMsg->hint_type = 0; //Keeps Meter Alive
    try {
    std::copy(reserved.begin() + INDEX_REQUEST_TYPE,
                reserved.end(), pMsg->pl_args);
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught on copy: %s in %s", e.what(), __func__);
        pMsg->data = 0;
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
        pMsg->data = 0;
    }
    return pMsg->data;
}

Perf::Perf() {
    char prop_read[PROPERTY_VALUE_MAX] = {0};
    mPerfEnabled = true;
    ALOGE("PERF2: inside Perf()");
    if (property_get(PERF_HAL_DISABLE_PROP, prop_read,"0")) {
        mPerfEnabled = !std::stoi(prop_read,NULL,0);
    }

    if (mPerfEnabled) {
        Init();
    }
    QLOGL(LOG_TAG, QLOG_L1, "Perf Hal Enabled %" PRId8, mPerfEnabled);
}

Perf::~Perf() {
    mImpl.PerfGlueLayerExit();
}

void Perf::Init() {
    char trace_prop[PROPERTY_VALUE_MAX];

    QLOGV(LOG_TAG, "PERF2: inside Perf::Init()");
    if (PerfLogInit() < 0) {
        QLOGE(LOG_TAG, "PerfLogInit failed");
    }

    //init TargetConfig
    mTargetConfig.InitializeTargetConfig();
    QLOGL(LOG_TAG, QLOG_L1, "TargetConfig Init Complete");
    //init PerfConfigStore
    mPerfDataStore.ConfigStoreInit();

    //register mp-ctl
    mImpl.LoadPerfLib("libqti-perfd.so");
    string prop = "";
    perfGetProp("vendor.debug.enable.lm", "false", &prop);

    bool enableLM = (!prop.compare("false"))? false : true;
    if(enableLM) {
        QLOGL(LOG_TAG, QLOG_L1, "LM enabled : Loading liblearningmodule.so");
        mImpl.LoadPerfLib("liblearningmodule.so");
    }

    perfGetProp("vendor.debug.enable.memperfd", "false", &prop);
    bool enableMemperfd = (!prop.compare("false"))? false : true;
    if(enableLM && enableMemperfd) {
        perfGetProp("vendor.enable.memperfd_MIN_RAM_in_KB", "1048576", &prop);
        uint32_t minRAMKB = stoi(prop.c_str(), NULL);
        perfGetProp("vendor.enable.memperfd_MAX_RAM_in_KB", "20971520", &prop);
        uint32_t maxRAMKB = stoi(prop.c_str(), NULL);
        uint32_t memTotal = mTargetConfig.getRamInKB();//memtotal is in KB. So accept thresholds in KB.
        if (memTotal > minRAMKB  && memTotal <= maxRAMKB) {
            mImpl.LoadPerfLib("libmemperfd.so");
        } else {
            QLOGL(LOG_TAG, QLOG_WARNING,"Unable to load Memperfd as RAM configuration is not satisfied");
        }
    }

    /* Enable systraces by adding vendor.debug.trace.perf=1 into build.prop */
    if (property_get(PROP_NAME, trace_prop, NULL) > 0) {
        if (trace_prop[0] == '1') {
            perf_debug_output = PERF_SYSTRACE = atoi(trace_prop);
        }
    }

    //init all modules
    mImpl.PerfGlueLayerInit();
}

bool Perf::checkPerfStatus(const char* func) {
    if (!mPerfEnabled) {
        QLOGL(LOG_TAG, QLOG_WARNING, "Perf Hal is Disabled cannot execute %s", func);
        (void) func;
    }
    return mPerfEnabled;
}

}  // namespace perf2
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
