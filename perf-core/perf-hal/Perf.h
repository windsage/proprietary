/******************************************************************************
  @file    Perf.h
  @brief   Android performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef VENDOR_QTI_HARDWARE_PERF2_V1_H
#define VENDOR_QTI_HARDWARE_PERF2_V1_H

#include <aidl/vendor/qti/hardware/perf2/BnPerf.h>

#include "PerfGlueLayer.h"
#include "PerfConfig.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace perf2 {

using ::ndk::ScopedAStatus;

struct Perfcallback {
    Perfcallback(std::shared_ptr<::aidl::vendor::qti::hardware::perf2::IPerfCallback> callback, int32_t clientId):
        callback(callback), clientId(clientId){}
    std::shared_ptr<::aidl::vendor::qti::hardware::perf2::IPerfCallback> callback;
    int32_t clientId;
};

class Perf : public BnPerf {
private:
    PerfGlueLayer mImpl;
    PerfConfigDataStore &mPerfDataStore = PerfConfigDataStore::getPerfDataStore();
    void Init();
    bool checkPerfStatus(const char*);
    std::vector< Perfcallback > callbacks;
    std::mutex perf_callback_mutex ;
    bool mPerfEnabled;
    TargetConfig &mTargetConfig = TargetConfig::getTargetConfig();
    int32_t halPreAction(mpctl_msg_t *pMsg,  const std::vector<int32_t>& reserved);
public:
    Perf();
    ~Perf();

public:
    ScopedAStatus perfLockAcquire(int32_t in_pl_handle, int32_t in_duration, const std::vector<int32_t>& in_boostsList, const std::vector<int32_t>& in_reserved, int32_t* _aidl_return) override;
    ScopedAStatus perfLockRelease(int32_t in_pl_handle, const std::vector<int32_t>& in_reserved) override;

    ScopedAStatus perfHint(int32_t in_hint, const std::string& in_userDataStr, int32_t in_userData1, int32_t in_userData2, const std::vector<int32_t>& in_reserved, int32_t* _aidl_return) override;
    ScopedAStatus perfProfile(int32_t in_pl_handle, int32_t in_profile, int32_t in_reserved, int32_t* _aidl_return) override;
    ScopedAStatus perfLockCmd(int32_t in_cmd, int32_t in_reserved) override;
    ScopedAStatus perfGetProp(const std::string& in_propName, const std::string& in_defaultVal, std::string* _aidl_return) override;
    ScopedAStatus perfSetProp(const std::string& in_propName, const std::string& in_value, int32_t* _aidl_return) override;
    ScopedAStatus perfAsyncRequest(int32_t in_cmd, const std::string& in_userDataStr, const std::vector<int32_t>& in_params, int32_t* _aidl_return) override;
    ScopedAStatus perfSyncRequest(int32_t in_cmd, const std::string& in_userDataStr, const std::vector<int32_t>& in_params, std::string* _aidl_return) override;
    ScopedAStatus perfCallbackDeregister(const std::shared_ptr<::aidl::vendor::qti::hardware::perf2::IPerfCallback>& in_callback, int32_t in_clientId, int32_t* _aidl_return) override;
    ScopedAStatus perfCallbackRegister(const std::shared_ptr<::aidl::vendor::qti::hardware::perf2::IPerfCallback>& in_callback, int32_t in_clientId, int32_t* _aidl_return) override;
    ScopedAStatus perfLockAcqAndRelease(int32_t in_pl_handle, int32_t in_duration, const std::vector<int32_t>& in_boostsList, const std::vector<int32_t>& in_reserved, int32_t* _aidl_return) override;
    ScopedAStatus perfEvent(int32_t in_eventId, const std::string& in_userDataStr, const std::vector<int32_t>& in_reserved) override;
    ScopedAStatus perfGetFeedback(int32_t in_featureId, const std::string& in_userDataStr, const std::vector<int32_t>& in_reserved, int32_t* _aidl_return) override;
    ScopedAStatus perfHintAcqRel(int32_t in_pl_handle, int32_t in_hint, const std::string& in_pkg_name, int32_t in_duration, int32_t in_hint_type, const std::vector<int32_t>& in_reserved, int32_t* _aidl_return) override;
    ScopedAStatus perfHintRenew(int32_t in_pl_handle, int32_t in_hint, const std::string& in_pkg_name, int32_t in_duration, int32_t in_hint_type, const std::vector<int32_t>& in_reserved, int32_t* _aidl_return) override;

    void sendPerfCallback(uint32_t hint, const std::string& userDataStr, int32_t userData1, int32_t userData2, const std::vector<int32_t>& reserved);
    void sendPerfCallbackOffload(uint32_t hint, const std::string& userDataStr, int32_t userData1, int32_t userData2, const std::vector<int32_t>& reserved);
};


}  // namespace perf2
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // aidl

#endif  // VENDOR_QTI_HARDWARE_PERF2_V1_H
