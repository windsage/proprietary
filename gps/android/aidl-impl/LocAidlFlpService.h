/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_LOCAIDLFLPSERVICE_H
#define VENDOR_QTI_GNSS_LOCAIDLFLPSERVICE_H

#include "LocAidlUtils.h"
#include <memory>
#include <LocAidlFlpClient.h>
#include <aidl/vendor/qti/gnss/BnLocAidlFlpService.h>
#include <location_interface.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::ILocAidlFlpService;
using ::aidl::vendor::qti::gnss::ILocAidlFlpServiceCallback;
using ILocAidlFlpServiceCallback = ::aidl::vendor::qti::gnss::ILocAidlFlpServiceCallback;

struct LocAidlFlpService : public BnLocAidlFlpService {
    LocAidlFlpService();
    virtual ~LocAidlFlpService();
    // Methods from ::aidl::vendor::qti::gnss::ILocAidlFlpService follow.
    ::ndk::ScopedAStatus init(const std::shared_ptr<ILocAidlFlpServiceCallback>& callback,
            bool* _aidl_return) override;
    ::ndk::ScopedAStatus getAllSupportedFeatures(int32_t* _aidl_return) override;
    ::ndk::ScopedAStatus getAllBatchedLocations(int32_t sessionId, int32_t* _aidl_return) override;
    ::ndk::ScopedAStatus stopFlpSession(int32_t sessionId, int32_t* _aidl_return) override;
    ::ndk::ScopedAStatus deleteAidingData(int64_t flags) override;
    ::ndk::ScopedAStatus getMaxPowerAllocated() override;
    ::ndk::ScopedAStatus updateXtraThrottle(bool enabled) override;

    // Methods from ::aidl::vendor::qti::gnss::ILocAidlFlpService follow.
    ::ndk::ScopedAStatus startFlpSession(int32_t id, int32_t flags, int64_t minIntervalNanos,
            int32_t minDistanceMetres, int32_t tripDistanceMeters,
            int32_t power_mode, int32_t tbm_ms, int32_t* _aidl_return) override;
    ::ndk::ScopedAStatus updateFlpSession(int32_t id, int32_t flags, int64_t minIntervalNanos,
            int32_t minDistanceMetres, int32_t tripDistanceMeters,
            int32_t power_mode, int32_t tbm_ms, int32_t* _aidl_return) override;

    void handleAidlClientSsr();
private:
    const GnssInterface* getGnssInterface();

    int32_t startFlpSessionWithPowerMode(
            int32_t id, uint32_t flags, int64_t minIntervalNanos,
            int32_t minDistanceMetres, uint32_t tripDistanceMeters,
            uint32_t power_mode, uint32_t tbm_ms);
    int32_t updateFlpSessionWithPowerMode(
            int32_t id, uint32_t flags, int64_t minIntervalNanos,
            int32_t minDistanceMetres, uint32_t tripDistanceMeters,
            uint32_t power_mode, uint32_t tbm_ms);

private:
    LocAidlFlpClient* mIzatFlpApiClient;
    std::shared_ptr<ILocAidlFlpServiceCallback> mCallbackIface = nullptr;
    GnssInterface* mGnssInterface = nullptr;
    bool mGetGnssInterfaceFailed = false;
    std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
};

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif  // VENDOR_QTI_GNSS__LOCHIDLFLPSERVICE_H
