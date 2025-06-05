/*
 * Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_FlpClient"
#define LOG_NDEBUG 0

#include "LocAidlUtils.h"
#include "LocAidlFlpClient.h"
#include <LocationAPI.h>
#include <log_util.h>
#include <dlfcn.h>
#include <cinttypes>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

typedef const GnssInterface* (getLocationInterface)();

LocAidlFlpClient * LocAidlFlpClient::sFlpClient = nullptr;

using ::aidl::vendor::qti::gnss::LocAidlBatchOptions;
using ::aidl::vendor::qti::gnss::LocAidlBatchMode;
using ::aidl::vendor::qti::gnss::LocAidlBatchStatusInfo;
using ::aidl::vendor::qti::gnss::LocAidlBatchStatus;
using ::aidl::vendor::qti::gnss::LocAidlLocation;
using ::aidl::vendor::qti::gnss::ILocAidlFlpServiceCallback;
using ILocAidlFlpServiceCallback = ::aidl::vendor::qti::gnss::ILocAidlFlpServiceCallback;
using LocAidlLocation = ::aidl::vendor::qti::gnss::LocAidlLocation;

LocAidlFlpClient* LocAidlFlpClient::getInstance() {
    if (sFlpClient == nullptr) {
        sFlpClient = new LocAidlFlpClient();
    }
    return sFlpClient;
}

LocAidlFlpClient::LocAidlFlpClient() :
        LocationAPIClientBase(), mCapabilitiesMask(-1), mGnssCbIface(nullptr) {}

void LocAidlFlpClient::setCallback(const std::shared_ptr<ILocAidlFlpServiceCallback>& callback) {
    ENTRY_LOG();

    mGnssCbIface = callback;
    mGnssInterface = nullptr;

    LocationCallbacks locationCallbacks;
    memset(&locationCallbacks, 0, sizeof(LocationCallbacks));
    locationCallbacks.size = sizeof(LocationCallbacks);

    locationCallbacks.batchingCb = [this](size_t count, Location* location,
            const BatchingOptions& batchOptions) {
        onBatchingCb(count, location, batchOptions);
    };

#ifdef QESDK_ENABLED
    // Register with engineLocationsInfoCb, drop aggreated PVTs(if received),
    // so this call back only report SPE no matter what the techonolgy is used;
    locationCallbacks.engineLocationsInfoCb = [this](uint32_t count,
            GnssLocationInfoNotification* engineLocationInfoNotification) {
        LOC_LOGv("engineLocationsInfoCb, count %d", count);
        onEngineLocationsInfoCb(count, engineLocationInfoNotification);
    };
#else
    locationCallbacks.trackingCb = [this](const Location& location) {
            onTrackingCb(location);
    };
#endif

    locationCallbacks.batchingStatusCb = [this](const BatchingStatusInfo& batchStatusInfo,
            std::list<uint32_t> &listOfCompletedTrips) {
        onBatchingStatusCb(batchStatusInfo, listOfCompletedTrips);
    };

    locationCallbacks.geofenceBreachCb = nullptr;
    locationCallbacks.geofenceStatusCb = nullptr;
    locationCallbacks.gnssLocationInfoCb = nullptr;
    locationCallbacks.gnssNiCb = nullptr;
    locationCallbacks.gnssSvCb = nullptr;
    locationCallbacks.gnssNmeaCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;

    locAPISetCallbacks(locationCallbacks);
}

const GnssInterface* LocAidlFlpClient::getGnssInterface() {
    static bool getGnssInterfaceFailed = false;
    if (nullptr == mGnssInterface && !getGnssInterfaceFailed) {
        LOC_LOGd("loading libgnss.so::getGnssInterface ...");
        getLocationInterface* getter = NULL;
        const char *error = NULL;
        dlerror();
        void *handle = dlopen("libgnss.so", RTLD_NOW);
        if (NULL == handle)  {
            LOC_LOGW("dlopen for libgnss.so failed");
        } else if (NULL != (error = dlerror()))  {
            LOC_LOGW("dlopen for libgnss.so failed, error = %s", error);
        } else {
            getter = (getLocationInterface*)dlsym(handle, "getGnssInterface");
            if ((error = dlerror()) != NULL)  {
                LOC_LOGW("dlsym for libgnss.so::getGnssInterface failed, error = %s", error);
                getter = NULL;
            }
        }

        if (NULL == getter) {
            getGnssInterfaceFailed = true;
        } else {
            mGnssInterface = (GnssInterface*)(*getter)();
        }
    }
    return mGnssInterface;
}

uint32_t LocAidlFlpClient::locAPIGnssDeleteAidingData(GnssAidingData& data)
{
    ENTRY_LOG();

    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    const GnssInterface* gnssInterface = getGnssInterface();
    if (gnssInterface != nullptr) {
        gnssInterface->gnssDeleteAidingData(data);
        retVal = LOCATION_ERROR_SUCCESS;
    }

    return retVal;
}

uint32_t LocAidlFlpClient::updateXtraThrottle(const bool enabled)
{
    ENTRY_LOG();

    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    const GnssInterface* gnssInterface = getGnssInterface();
    if (gnssInterface != nullptr) {
        gnssInterface->gnssUpdateXtraThrottle(enabled);
        retVal = LOCATION_ERROR_SUCCESS;
    }

    return retVal;
}

void LocAidlFlpClient::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) {
    LOC_LOGv("capabilities mask: 0x%" PRIx64 "", capabilitiesMask);
    mCapabilitiesMask = capabilitiesMask;
}

void LocAidlFlpClient::onEngineLocationsInfoCb(uint32_t count,
            GnssLocationInfoNotification* engineLocationInfoNotification) {
    if (nullptr == engineLocationInfoNotification) {
        LOC_LOGe("engineLocationInfoNotification is nullptr");
        return;
    }
    GnssLocationInfoNotification* locPtr = nullptr;
    bool foundSPE = false;

    for (int i = 0; i < count; i++) {
        locPtr = engineLocationInfoNotification + i;
        if (nullptr == locPtr) return;
        LOC_LOGv("count %d, type %d", i, locPtr->locOutputEngType);
        if (LOC_OUTPUT_ENGINE_SPE == locPtr->locOutputEngType) {
            foundSPE = true;
            break;
        }
    }
    if (foundSPE) {
        onTrackingCb(locPtr->location);
    }
}

void LocAidlFlpClient::onTrackingCb(const Location& location) {

    // Let's convert Location to LocAidlLocation
    LocAidlLocation gnssLocation;
    LocAidlUtils::locationToLocAidlLocation(location, gnssLocation);

    // Invoke the callback
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }
    TO_AIDL_CLIENT();
    mGnssCbIface->gnssLocationTrackingCb(gnssLocation);
}

void LocAidlFlpClient::onBatchingCb(size_t count, Location* locations,
        const BatchingOptions& batchOptions) {

    // Sanity checks
    if ((int)count <= 0) {
        LOC_LOGE("Invalid count %zu", count);
        return;
    }
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }

    // Create a vector with all locations
    vector<LocAidlLocation> gnssLocationVec;
    gnssLocationVec.resize(count);
    for (int i = 0; i < (int)count; i++) {
        LocAidlUtils::locationToLocAidlLocation(
                locations[i], gnssLocationVec[i]);
    }
    TO_AIDL_CLIENT();
    LocAidlBatchOptions hidlbatchOptions;
    hidlbatchOptions.batchMode = (LocAidlBatchMode)batchOptions.batchingMode;

    mGnssCbIface->gnssLocationBatchingCb(hidlbatchOptions, gnssLocationVec);
}

void LocAidlFlpClient::onBatchingStatusCb(const BatchingStatusInfo& batchStatusInfo,
        std::list<uint32_t> &listOfCompletedTrips)
{
    // Sanity checks
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }

    // Create a vector with all trip ids
    vector<int32_t> completedTripsVec;
    completedTripsVec.resize(listOfCompletedTrips.size());
    auto it = listOfCompletedTrips.begin();
    for (int index = 0; it != listOfCompletedTrips.end(); it++, index++) {
        int32_t tripClientId = *it;
        completedTripsVec[index] = tripClientId;
    }

    TO_AIDL_CLIENT();

    LocAidlBatchStatusInfo hidlBatchStatusInfo;
    hidlBatchStatusInfo.batchStatus = (LocAidlBatchStatus)batchStatusInfo.batchingStatus;

    mGnssCbIface->gnssBatchingStatusCb(hidlBatchStatusInfo, completedTripsVec);
}


}  // namespace implementation
}  // namespace AIDL
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
