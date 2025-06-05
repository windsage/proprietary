/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_FlpService"
#define LOG_NDEBUG 0

#include "LocAidlFlpService.h"


#include <log_util.h>
#include <gps_extended_c.h>
#include <LocationAPI.h>
#include <dlfcn.h>
#include "loc_misc_utils.h"
#include <loc_cfg.h>
namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

typedef const GnssInterface* (getLocationInterface)();

#define FLP_SERVICE_PROVIDER_ERROR 1
#define FLP_SERVICE_PROVIDER_SUCCESS 0

#define FLP_EXT_BATCHING_ON_FULL                0x0000001
#define FLP_EXT_BATCHING_ON_FIX                 0x0000002
#define FLP_EXT_BATCHING_ON_TRIP_COMPLETED      0x0000003

#define FLP_TIME_BASED_BATCHING_IS_SUPPORTED      (1U<<0)
#define FLP_DISTANCE_BASED_TRACKING_IS_SUPPORTED  (1U<<1)
#define FLP_ADAPTIVE_BATCHING_IS_SUPPORTED        (1U<<2)
#define FLP_DISTANCE_BASED_BATCHING_IS_SUPPORTED  (1U<<3)
#define FLP_OUTDOOR_TRIP_BATCHING_IS_SUPPORTED    (1U<<4)
#define FLP_AGPM_IS_SUPPORTED                     (1U<<5)
#define CONSTELLATION_ENABLEMENT_IS_SUPPORTED     (1U<<6)
#define CONFORMITY_INDEX_IS_SUPPORTED             (1U<<7)
#define PRECISE_POSITION_IS_SUPPORTED             (1U<<8)
#define GTP_LOCATION_IS_SUPPORTED                 (1U<<9)
#define QESDK_GTP_LOCATION_IS_SUPPORTED           (1U<<10)
#define HW_RSSI_IS_SUPPORTED                      (1U<<11)
#define HW_RTT_IS_SUPPORTED                       (1U<<12)
#define QWES_GTP_RSSI_IS_SUPPORTED                (1U<<13)
#define QWES_GTP_RTT_IS_SUPPORTED                 (1U<<14)
#define QWES_WWAN_STANDARD_IS_SUPPORTED           (1U<<15)
#define QWES_WWAN_PREMIUM_IS_SUPPORTED            (1U<<16)

#define FLP_SESSION_POWER_LEVEL_HIGH           5000
#define FLP_SESSION_POWER_LEVEL_MIDDLE         2500
#define FLP_SESSION_POWER_LEVEL_LOW            1000

static int gtpEnabled = 0;
static loc_param_s_type locSdkConfParamTable[] = {
    {"LOCATION_SDK_GTP_ENABLED", &gtpEnabled, nullptr, 'n'}
};

void FlpServiceDied(void* cookie) {
    LOC_LOGe("ILocAidlFlpServiceCallback died.");
    auto thiz = static_cast<LocAidlFlpService*>(cookie);
    if (nullptr != thiz) {
        thiz->handleAidlClientSsr();
        thiz = nullptr;
    }

}

const GnssInterface* LocAidlFlpService::getGnssInterface() {

    ENTRY_LOG();

    if (nullptr == mGnssInterface && !mGetGnssInterfaceFailed) {
        LOC_LOGd("loading libgnss.so::getGnssInterface ...");
        void * handle = nullptr;
        getLocationInterface* getter =
                (getLocationInterface*)dlGetSymFromLib(handle, "libgnss.so", "getGnssInterface");

        if (NULL == getter) {
            mGetGnssInterfaceFailed = true;
        } else {
            mGnssInterface = (GnssInterface*)(*getter)();
        }
    }
    return mGnssInterface;
}

LocAidlFlpService::LocAidlFlpService() :
        mIzatFlpApiClient(LocAidlFlpClient::getInstance()), mCallbackIface(nullptr) {
    UTIL_READ_CONF(LOC_PATH_IZAT_CONF, locSdkConfParamTable);
    ENTRY_LOG();
}

LocAidlFlpService::~LocAidlFlpService() {
    ENTRY_LOG();
    handleAidlClientSsr();
}

void LocAidlFlpService::handleAidlClientSsr() {
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    mDeathRecipient = nullptr;
    mCallbackIface = nullptr;
    if (mIzatFlpApiClient != nullptr) {
        mIzatFlpApiClient->locAPIRemoveAllSessions();
    }
}

::ndk::ScopedAStatus LocAidlFlpService::init(
        const std::shared_ptr<ILocAidlFlpServiceCallback>& callback, bool* _aidl_return) {
    FROM_AIDL_CLIENT();

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(FlpServiceDied);
    } else if (mCallbackIface != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }

    mCallbackIface = callback;
    if (callback != nullptr) {
        mDeathRecipient->registerToPeer(callback->asBinder().get(), this);
    } else {
        LOC_LOGe("callback is nullptr!");
    }

    if (mIzatFlpApiClient != nullptr) {
        mIzatFlpApiClient->setCallback(mCallbackIface);
    }
    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlFlpService::getAllSupportedFeatures(int32_t* _aidl_return) {
    FROM_AIDL_CLIENT();

    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGe("mIzatFlpApiClient is nullptr!");
        return ndk::ScopedAStatus::ok();
    }

    uint32_t result = 0;
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT) {
        result |= FLP_TIME_BASED_BATCHING_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT) {
        result |= FLP_DISTANCE_BASED_BATCHING_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT) {
        result |= FLP_DISTANCE_BASED_TRACKING_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_OUTDOOR_TRIP_BATCHING_BIT) {
        result |= FLP_OUTDOOR_TRIP_BATCHING_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_AGPM_BIT) {
        result |= FLP_AGPM_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_CONSTELLATION_ENABLEMENT_BIT) {
        result |= CONSTELLATION_ENABLEMENT_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_CONFORMITY_INDEX_BIT) {
        result |= CONFORMITY_INDEX_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_EDGNSS_BIT ||
            mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_QWES_PPE) {
        result |= PRECISE_POSITION_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_WIFI_RSSI_POSITIONING) {
        result |= HW_RSSI_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_WIFI_RTT_POSITIONING) {
        result |= HW_RTT_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_QWES_WIFI_RSSI_POSITIONING) {
        result |= QWES_GTP_RSSI_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_QWES_WIFI_RTT_POSITIONING) {
        result |= QWES_GTP_RTT_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_QWES_WWAN_STANDARD_POSITIONING)
    {
        result |= QWES_WWAN_STANDARD_IS_SUPPORTED;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_QWES_WWAN_PREMIUM_POSITIONING)
    {
        result |= QWES_WWAN_PREMIUM_IS_SUPPORTED;
    }
    if (0 != result) {
        result |= FLP_ADAPTIVE_BATCHING_IS_SUPPORTED;
    }

    result |= GTP_LOCATION_IS_SUPPORTED;

#ifdef QESDK_ENABLED
    result |= QESDK_GTP_LOCATION_IS_SUPPORTED;
    if (gtpEnabled) {
        result &= ~QESDK_GTP_LOCATION_IS_SUPPORTED;
    }
#endif

    *_aidl_return = result;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlFlpService::getAllBatchedLocations(int32 sessionId,
        int32_t* _aidl_return) {

    FROM_AIDL_CLIENT();
    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGe("mIzatFlpApiClient is nullptr!");
        return ndk::ScopedAStatus::ok();
    }

    mIzatFlpApiClient->locAPIGetBatchedLocations(sessionId, SIZE_MAX);
    *_aidl_return = FLP_SERVICE_PROVIDER_SUCCESS;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlFlpService::stopFlpSession(int32_t sessionId, int32_t* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGe("mIzatFlpApiClient is nullptr!");
        return ndk::ScopedAStatus::ok();
    }

    int32_t result = mIzatFlpApiClient->locAPIStopSession(sessionId);
    if (result != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("locAPIStopSession() failed, ret %d", result);
        *_aidl_return = FLP_SERVICE_PROVIDER_ERROR;
    }

    *_aidl_return = FLP_SERVICE_PROVIDER_SUCCESS;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlFlpService::deleteAidingData(int64_t flags) {
    FROM_AIDL_CLIENT();
    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGe("mIzatFlpApiClient is nullptr!");
        return ndk::ScopedAStatus::ok();
    }

    GnssAidingData data;
    memset(&data, 0, sizeof (GnssAidingData));
    data.sv.svTypeMask = GNSS_AIDING_DATA_SV_TYPE_GPS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT |
        GNSS_AIDING_DATA_SV_TYPE_GALILEO_BIT;
    data.posEngineMask = STANDARD_POSITIONING_ENGINE;

    if (flags == LOC_GPS_DELETE_ALL) {
        data.deleteAll = true;
    }
    else {
        if (flags & LOC_GPS_DELETE_EPHEMERIS) {
            data.sv.svMask |= GNSS_AIDING_DATA_SV_EPHEMERIS_BIT;
        }
        if (flags & LOC_GPS_DELETE_ALMANAC) {
            data.sv.svMask |= GNSS_AIDING_DATA_SV_ALMANAC_BIT;
        }
        if (flags & LOC_GPS_DELETE_POSITION) {
            data.common.mask |= GNSS_AIDING_DATA_COMMON_POSITION_BIT;
        }
        if (flags & LOC_GPS_DELETE_TIME) {
            data.common.mask |= GNSS_AIDING_DATA_COMMON_TIME_BIT;
        }
        if (flags & LOC_GPS_DELETE_IONO) {
            data.sv.svMask |= GNSS_AIDING_DATA_SV_IONOSPHERE_BIT;
        }
        if (flags & LOC_GPS_DELETE_UTC) {
            data.common.mask |= GNSS_AIDING_DATA_COMMON_UTC_BIT;
        }
        if (flags & LOC_GPS_DELETE_HEALTH) {
            data.sv.svMask |= GNSS_AIDING_DATA_SV_HEALTH_BIT;
        }
        if (flags & LOC_GPS_DELETE_SVDIR) {
            data.sv.svMask |= GNSS_AIDING_DATA_SV_DIRECTION_BIT;
        }
        if (flags & LOC_GPS_DELETE_SVSTEER) {
            data.sv.svMask |= GNSS_AIDING_DATA_SV_STEER_BIT;
        }
        if (flags & LOC_GPS_DELETE_SADATA) {
            data.sv.svMask |= GNSS_AIDING_DATA_SV_SA_DATA_BIT;
        }
        if (flags & LOC_GPS_DELETE_RTI) {
            data.common.mask |= GNSS_AIDING_DATA_COMMON_RTI_BIT;
        }
        if (flags & LOC_GPS_DELETE_CELLDB_INFO) {
            data.common.mask |= GNSS_AIDING_DATA_COMMON_CELLDB_BIT;
        }
    }

    int32_t result = mIzatFlpApiClient->locAPIGnssDeleteAidingData(data);
    if (result != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("deleteAidingData() failed, ret %d", result);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlFlpService::getMaxPowerAllocated() {
    FROM_AIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    gnssInterface->getPowerStateChanges([cbIface = mCallbackIface](bool isOn) {
                int32_t powerMW = isOn ? FLP_SESSION_POWER_LEVEL_HIGH : 0;
                TO_AIDL_CLIENT();
                if (cbIface != nullptr) {
                    cbIface->gnssMaxPowerAllocatedCb(powerMW);
                }
            });

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlFlpService::updateXtraThrottle(bool enabled) {
    FROM_AIDL_CLIENT();
    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGe("mIzatFlpApiClient is nullptr!");
        return ndk::ScopedAStatus::ok();
    }

    uint32_t result = mIzatFlpApiClient->updateXtraThrottle(enabled);
    if (result != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("updateXtraThrottle() failed, ret %d", result);
    }

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlFlpService::startFlpSession(
        int32_t id, int32_t flags, int64_t minIntervalNanos, int32_t minDistanceMetres,
        int32_t tripDistanceMeters, int32_t power_mode, int32_t tbm_ms, int32_t* _aidl_return) {

    *_aidl_return = startFlpSessionWithPowerMode(id, flags, minIntervalNanos, minDistanceMetres,
                                        tripDistanceMeters, power_mode, tbm_ms);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlFlpService::updateFlpSession(
        int32_t id, int32_t flags, int64_t minIntervalNanos, int32_t minDistanceMetres,
        int32_t tripDistanceMeters, int32_t power_mode, int32_t tbm_ms, int32_t* _aidl_return) {

    *_aidl_return = updateFlpSessionWithPowerMode(id, flags, minIntervalNanos, minDistanceMetres,
                                         tripDistanceMeters, power_mode, tbm_ms);
    return ndk::ScopedAStatus::ok();
}

int32_t LocAidlFlpService::startFlpSessionWithPowerMode(
        int32_t id, uint32_t flags, int64_t minIntervalNanos, int32_t minDistanceMetres,
        uint32_t tripDistanceMeters, uint32_t power_mode, uint32_t tbm_ms) {

    FROM_AIDL_CLIENT();
    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGe("mIzatFlpApiClient is nullptr!");
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    LocationOptions locOptions;
    locOptions.size = sizeof(LocationOptions);
    locOptions.minInterval = (uint32_t)(minIntervalNanos / 1000000L);
    locOptions.minDistance = minDistanceMetres;
    locOptions.qualityLevelAccepted = QUALITY_ANY_VALID_FIX;
    if (flags == FLP_EXT_BATCHING_ON_TRIP_COMPLETED) {
        locOptions.minDistance = tripDistanceMeters;
    }

    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSB_BIT) {
        locOptions.mode = GNSS_SUPL_MODE_MSB;
    }

    TrackingOptions options(locOptions);

    uint32_t mode = 0;
    if (flags == FLP_EXT_BATCHING_ON_FIX) {
         mode = SESSION_MODE_ON_FIX;
    } else if (flags == FLP_EXT_BATCHING_ON_FULL) {
         mode = SESSION_MODE_ON_FULL;
    }  else if (flags == FLP_EXT_BATCHING_ON_TRIP_COMPLETED) {
         mode = SESSION_MODE_ON_TRIP_COMPLETED;
    } else {
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    if (power_mode >= GNSS_POWER_MODE_M1 && power_mode <= GNSS_POWER_MODE_M5) {
        options.powerMode = (GnssPowerMode)power_mode;
        options.tbm = tbm_ms;
    }

    int32_t result = mIzatFlpApiClient->locAPIStartSession(id, mode, std::move(options));
    if (result != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("startFlpSession() failed, ret %d", result);
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    return FLP_SERVICE_PROVIDER_SUCCESS;
}

int32_t LocAidlFlpService::updateFlpSessionWithPowerMode(
        int32_t id, uint32_t flags, int64_t minIntervalNanos, int32_t minDistanceMetres,
        uint32_t tripDistanceMeters, uint32_t power_mode, uint32_t tbm_ms) {

    FROM_AIDL_CLIENT();
    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGe("mIzatFlpApiClient is nullptr!");
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    LocationOptions locOptions;
    memset(&locOptions, 0, sizeof(LocationOptions));
    locOptions.size = sizeof(LocationOptions);
    locOptions.minInterval = (uint32_t)(minIntervalNanos / 1000000L);
    locOptions.minDistance = minDistanceMetres;
    locOptions.qualityLevelAccepted = QUALITY_ANY_VALID_FIX;
    if (flags == FLP_EXT_BATCHING_ON_TRIP_COMPLETED) {
        locOptions.minDistance = tripDistanceMeters;
    }
    locOptions.mode = GNSS_SUPL_MODE_STANDALONE;
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSB_BIT) {
        locOptions.mode = GNSS_SUPL_MODE_MSB;
    }

    TrackingOptions options(locOptions);

    uint32_t mode = 0;
    if (flags == FLP_EXT_BATCHING_ON_FIX) {
         mode = SESSION_MODE_ON_FIX;
    } else if (flags == FLP_EXT_BATCHING_ON_FULL) {
         mode = SESSION_MODE_ON_FULL;
    } else if (flags == FLP_EXT_BATCHING_ON_TRIP_COMPLETED) {
         mode = SESSION_MODE_ON_TRIP_COMPLETED;
    } else {
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    if (power_mode <= GNSS_POWER_MODE_M5) {
        options.powerMode = (GnssPowerMode)power_mode;
        options.tbm = tbm_ms;
    } else {
        LOC_LOGE("Invalid power mode %d", power_mode);
    }

    int32_t result = mIzatFlpApiClient->locAPIUpdateSessionOptions(id, mode, std::move(options));
    if (result != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("updateFlpSession() failed, ret %d", result);
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    return FLP_SERVICE_PROVIDER_SUCCESS;
}

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
