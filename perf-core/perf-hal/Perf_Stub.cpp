/******************************************************************************
  @file    Perf_Stub.cpp
  @brief   Android performance HAL stub module
           These functions called when perf opts are disabled during target bringup

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017,2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define LOG_TAG "ANDR-PERF-STUB"

#include "Perf.h"
#include "PerfLog.h"

enum INDEX_REFERENCE {INDEX_CLIENT_TID=0, INDEX_CLIENT_PID =1};

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace perf2 {

ScopedAStatus Perf::perfLockAcquire(int32_t in_pl_handle, int32_t in_duration, const std::vector<int32_t>& in_boostsList, const std::vector<int32_t>& in_reserved, int32_t* _aidl_return) {
    *_aidl_return = -1;

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfLockRelease(int32_t in_pl_handle, const std::vector<int32_t>& in_reserved) {

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfHint(int32_t hint, const std::string& userDataStr, int32_t userData1, int32_t userData2, const std::vector<int32_t>& reserved, int32_t* _aidl_return) {
    *_aidl_return = -1;

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfProfile(int32_t, int32_t , int32_t, int32_t* _aidl_return) {
    *_aidl_return = -1;

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfLockCmd(int32_t cmd, int32_t reserved) {

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfGetProp(const std::string& propName, const std::string& defaultVal, std::string *_aidl_return) {
    *_aidl_return = defaultVal;

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfSetProp(const std::string& /*propName*/, const std::string& /*value*/, int32_t *_aidl_return) {

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfAsyncRequest(int32_t /*cmd*/, const std::string& /*userDataStr*/, const std::vector<int32_t>& /*params*/, int32_t *_aidl_return) {

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfSyncRequest(int32_t cmd, const std::string& /*userDataStr*/,
                                   const std::vector<int32_t>& /*params*/,
                                   std::string *_aidl_return) {
    *_aidl_return = "";

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

Perf::Perf() {
    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
}

Perf::~Perf() {
    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
}

ScopedAStatus Perf::perfCallbackRegister(const std::shared_ptr<::aidl::vendor::qti::hardware::perf2::IPerfCallback>& callback, int32_t clientId, int32_t* _aidl_return){
    *_aidl_return = 0;

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfCallbackDeregister(const std::shared_ptr<::aidl::vendor::qti::hardware::perf2::IPerfCallback>& callback, int32_t clientId, int32_t *_aidl_return){
    *_aidl_return = 0;

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

void Perf::sendPerfCallback(uint32_t hint, const std::string& userDataStr, int32_t userData1, int32_t userData2, const std::vector<int32_t>& reserved) {

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
}

ScopedAStatus Perf::perfLockAcqAndRelease(int32_t pl_handle, int32_t duration, const std::vector<int32_t>& boostsList, const std::vector<int32_t>& reserved,  int32_t *_aidl_return) {
    *_aidl_return = -1;

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfEvent(int32_t eventId, const std::string& userDataStr, const std::vector<int32_t>& reserved) {

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfGetFeedback(int32_t featureId, const std::string& userDataStr, const std::vector<int32_t>& reserved, int32_t *_aidl_return) {
    *_aidl_return = -1;

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfHintAcqRel(int32_t pl_handle, int32_t hint, const std::string& pkg_name, int32_t duration,
        int32_t hint_type, const std::vector<int32_t>& reserved, int32_t *_aidl_return) {
    *_aidl_return = -1;

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

ScopedAStatus Perf::perfHintRenew(int32_t pl_handle, int32_t hint, const std::string& pkg_name, int32_t duration,
        int32_t hint_type, const std::vector<int32_t>& reserved, int32_t *_aidl_return) {
    *_aidl_return = -1;

    QLOGL(LOG_TAG, QLOG_L3, "Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return ndk::ScopedAStatus::ok();

}

}  // namespace perf2
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
