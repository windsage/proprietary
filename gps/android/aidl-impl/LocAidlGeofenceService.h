/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/gnss/BnLocAidlGeofenceService.h>
#include <location_interface.h>
#include "LocAidlGeofenceClient.h"
#include "LocAidlUtils.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {


using ::aidl::vendor::qti::gnss::ILocAidlGeofenceService;
using ::aidl::vendor::qti::gnss::ILocAidlGeofenceServiceCallback;

struct LocAidlGeofenceService : public BnLocAidlGeofenceService {
    LocAidlGeofenceService();
    virtual ~LocAidlGeofenceService();
    // Methods from ::vendor::qti::gnss::ILocAidlGeofenceService follow.
    ::ndk::ScopedAStatus init(const std::shared_ptr<ILocAidlGeofenceServiceCallback>& callback,
            bool* _aidl_return) override;
    ::ndk::ScopedAStatus addGeofence(int32_t id, double latitude, double longitude, double radius,
            int32_t transitionTypes, int32_t responsiveness, int32_t confidence,
            int32_t dwellTime, int32_t dwellTimeMask) override;
    ::ndk::ScopedAStatus removeGeofence(int32_t id) override;
    ::ndk::ScopedAStatus updateGeofence(int32_t id, int32_t transitionTypes,
            int32_t responsiveness) override;
    ::ndk::ScopedAStatus pauseGeofence(int32_t id) override;
    ::ndk::ScopedAStatus resumeGeofence(int32_t id, int32_t transitionTypes) override;

    void handleAidlClientSsr();

private:
    LocAidlGeofenceClient* mIzatGeofenceApiClient;
    std::shared_ptr<ILocAidlGeofenceServiceCallback> mCallbackIface = nullptr;
    std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
};

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
