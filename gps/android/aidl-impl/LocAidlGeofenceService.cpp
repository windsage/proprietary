/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_GeofenceService"
#define LOG_NDEBUG 0

#include <log_util.h>
#include <gps_extended_c.h>
#include <LocationAPI.h>
#include "LocAidlGeofenceService.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

void GeofenceServiceDied(void* cookie) {
    LOC_LOGe("ILocAidlGeofenceServiceCallback died.");
    auto thiz = static_cast<LocAidlGeofenceService*>(cookie);
    thiz->handleAidlClientSsr();
}

LocAidlGeofenceService::LocAidlGeofenceService() :
        mIzatGeofenceApiClient(LocAidlGeofenceClient::getInstance()), mCallbackIface(nullptr) {
    ENTRY_LOG();
}

LocAidlGeofenceService::~LocAidlGeofenceService() {
    handleAidlClientSsr();
}

void LocAidlGeofenceService::handleAidlClientSsr() {
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    mDeathRecipient = nullptr;
    mCallbackIface = nullptr;
    if (mIzatGeofenceApiClient != nullptr) {
        mIzatGeofenceApiClient->locAPIRemoveAllGeofences();
    }
}

// Methods from ::vendor::qti::gnss::IGnssGeofenceServiceProvider follow.
::ndk::ScopedAStatus LocAidlGeofenceService::init(
        const std::shared_ptr<ILocAidlGeofenceServiceCallback>& callback, bool* _aidl_return) {
    FROM_AIDL_CLIENT();

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(GeofenceServiceDied);
    } else if (mCallbackIface != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    if (callback != nullptr) {
        mDeathRecipient->registerToPeer(callback->asBinder().get(), this);
    } else {
        LOC_LOGe("callback is nullptr!");
    }
    mCallbackIface = callback;

    if (mIzatGeofenceApiClient != nullptr) {
        mIzatGeofenceApiClient->setCallback(mCallbackIface);
    }
    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGeofenceService::addGeofence(
        int32_t id, double latitude, double longitude, double radius,
        int32_t transitionTypes, int32_t responsiveness, int32_t confidence,
        int32_t dwellTime, int32_t dwellTimeMask) {

    FROM_AIDL_CLIENT();
    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGe("mIzatGeofenceApiClient is nullptr");
        return ndk::ScopedAStatus::ok();
    }

    GeofenceOption options;
    memset(&options, 0, sizeof(GeofenceOption));
    options.size = sizeof(GeofenceOption);
    if (transitionTypes & LOC_GPS_GEOFENCE_ENTERED)
        options.breachTypeMask |= GEOFENCE_BREACH_ENTER_BIT;
    if (transitionTypes & LOC_GPS_GEOFENCE_EXITED)
        options.breachTypeMask |=  GEOFENCE_BREACH_EXIT_BIT;
    if (dwellTimeMask & LOC_GPS_GEOFENCE_DWELL_INSIDE)
        options.breachTypeMask |= GEOFENCE_BREACH_DWELL_IN_BIT;
    if (dwellTimeMask & LOC_GPS_GEOFENCE_DWELL_OUTSIDE)
        options.breachTypeMask |= GEOFENCE_BREACH_DWELL_OUT_BIT;
    options.responsiveness = responsiveness;
    options.dwellTime = dwellTime;

    switch (confidence) {
    case LOC_GPS_GEOFENCE_CONFIDENCE_LOW:
        options.confidence = GEOFENCE_CONFIDENCE_LOW;
        break;
    case LOC_GPS_GEOFENCE_CONFIDENCE_MEDIUM:
        options.confidence = GEOFENCE_CONFIDENCE_MEDIUM;
        break;
    case LOC_GPS_GEOFENCE_CONFIDENCE_HIGH:
        options.confidence = GEOFENCE_CONFIDENCE_HIGH;
        break;
    default: // default to HIGH
        options.confidence = GEOFENCE_CONFIDENCE_HIGH;
    }

    GeofenceInfo data;
    data.size = sizeof(GeofenceInfo);
    data.latitude = latitude;
    data.longitude = longitude;
    data.radius = radius;

    uint32_t gfId = id;
    mIzatGeofenceApiClient->locAPIAddGeofences(1, &gfId, &options, &data);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGeofenceService::removeGeofence(int32_t id) {
    FROM_AIDL_CLIENT();
    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGe("mIzatGeofenceApiClient is nullptr");
        return ndk::ScopedAStatus::ok();
    }

    uint32_t gfId = id;
    mIzatGeofenceApiClient->locAPIRemoveGeofences(1, &gfId);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGeofenceService::updateGeofence(
        int32_t id, int32_t transitionTypes, int32_t responsiveness) {

    FROM_AIDL_CLIENT();
    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGe("mIzatGeofenceApiClient is nullptr");
        return ndk::ScopedAStatus::ok();
    }

    GeofenceOption options;
    memset(&options, 0, sizeof(GeofenceOption));
    options.size = sizeof(GeofenceOption);
    if (transitionTypes & LOC_GPS_GEOFENCE_ENTERED)
        options.breachTypeMask |= GEOFENCE_BREACH_ENTER_BIT;
    if (transitionTypes & LOC_GPS_GEOFENCE_EXITED)
        options.breachTypeMask |=  GEOFENCE_BREACH_EXIT_BIT;
    options.responsiveness = responsiveness;

    uint32_t gfId = id;
    mIzatGeofenceApiClient->locAPIModifyGeofences(1, &gfId, &options);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGeofenceService::pauseGeofence(int32_t id) {
    FROM_AIDL_CLIENT();
    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGe("mIzatGeofenceApiClient is nullptr");
        return ndk::ScopedAStatus::ok();
    }

    uint32_t gfId = id;
    mIzatGeofenceApiClient->locAPIPauseGeofences(1, &gfId);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGeofenceService::resumeGeofence(
        int32_t id, int32_t transitionTypes) {

    FROM_AIDL_CLIENT();
    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGe("mIzatGeofenceApiClient is nullptr");
        return ndk::ScopedAStatus::ok();
    }

    GeofenceBreachTypeMask mask = 0;
    if (transitionTypes & LOC_GPS_GEOFENCE_ENTERED)
        mask |= GEOFENCE_BREACH_ENTER_BIT;
    if (transitionTypes & LOC_GPS_GEOFENCE_EXITED)
        mask |=  GEOFENCE_BREACH_EXIT_BIT;
    uint32_t gfId = id;
    mIzatGeofenceApiClient->locAPIResumeGeofences(1, &gfId, &mask);

    return ndk::ScopedAStatus::ok();
}

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
