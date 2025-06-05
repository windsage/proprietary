/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_GeofenceClient"
#define LOG_NDEBUG 0

#include "LocAidlGeofenceClient.h"
#include "LocAidlUtils.h"
#include <LocationAPI.h>
#include <log_util.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

LocAidlGeofenceClient * LocAidlGeofenceClient::sGeofenceClient = nullptr;

LocAidlGeofenceClient* LocAidlGeofenceClient::getInstance() {
    if (sGeofenceClient == nullptr) {
        sGeofenceClient = new LocAidlGeofenceClient();
    }
    return sGeofenceClient;
}

LocAidlGeofenceClient::LocAidlGeofenceClient() : LocationAPIClientBase() {}

void LocAidlGeofenceClient::setCallback(
        const std::shared_ptr<ILocAidlGeofenceServiceCallback>& callback) {

    ENTRY_LOG();
    mGnssCbIface = callback;
    LocationCallbacks locationCallbacks;
    memset(&locationCallbacks, 0, sizeof(LocationCallbacks));
    locationCallbacks.size = sizeof(LocationCallbacks);

    locationCallbacks.trackingCb = nullptr;
    locationCallbacks.batchingCb = nullptr;

    locationCallbacks.geofenceBreachCb =
        [this](const GeofenceBreachNotification& geofenceBreachNotification) {
            onGeofenceBreachCb(geofenceBreachNotification);
        };

    locationCallbacks.geofenceStatusCb =
        [this](const GeofenceStatusNotification& geofenceStatusNotification) {
            onGeofenceStatusCb(geofenceStatusNotification);
        };

    locationCallbacks.gnssLocationInfoCb = nullptr;
    locationCallbacks.gnssNiCb = nullptr;
    locationCallbacks.gnssSvCb = nullptr;
    locationCallbacks.gnssNmeaCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;

    locAPISetCallbacks(locationCallbacks);
}

void LocAidlGeofenceClient::onGeofenceBreachCb(
        const GeofenceBreachNotification& notification)
{
    ENTRY_LOG();

    // Sanity checks
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }
    int count = (int)notification.count;
    if (count <= 0) {
        LOC_LOGE("Invalid count %d", count);
        return;
    }

    std::vector<int32_t> idVec;
    idVec.resize(count);
    for (int i = 0; i < count; i++) {
        idVec[i] = notification.ids[i];
    }

    LocAidlLocation gnssLocation;
    LocAidlUtils::locationToLocAidlLocation(notification.location, gnssLocation);

    TO_AIDL_CLIENT();
    mGnssCbIface->gnssGeofenceBreachCb(
            count, idVec, gnssLocation,
            (int32_t)notification.type, notification.timestamp);
}

void LocAidlGeofenceClient::onGeofenceStatusCb(
        const GeofenceStatusNotification& notification)
{
    ENTRY_LOG();

    // Sanity checks
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }

    TO_AIDL_CLIENT();
    mGnssCbIface->gnssGeofenceStatusCb(
            (int32_t)notification.available, (int32_t)notification.techType);
}

void LocAidlGeofenceClient::onAddGeofencesCb(
        size_t count, LocationError* errors, uint32_t* ids)
{
    ENTRY_LOG();

    // Sanity checks
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }
    if ((int)count <= 0) {
        LOC_LOGE("Invalid count %zu", count);
        return;
    }

    std::vector<int32_t> errorVec;
    errorVec.resize(count);
    for (size_t i = 0; i < count; i++) {
        errorVec[i] = errors[i];
    }

    std::vector<int32_t> idVec;
    idVec.resize(count);
    for (size_t i = 0; i < count; i++) {
        idVec[i] = ids[i];
    }

    TO_AIDL_CLIENT();
    mGnssCbIface->gnssAddGeofencesCb(count, errorVec, idVec);
}

void LocAidlGeofenceClient::onRemoveGeofencesCb(
        size_t count, LocationError* errors, uint32_t* ids)
{
    ENTRY_LOG();

    // Sanity checks
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }
    if (count <= 0) {
        LOC_LOGE("Invalid count %zu", count);
        return;
    }

    std::vector<int32_t> errorVec;
    errorVec.resize(count);
    for (size_t i = 0; i < count; i++) {
        errorVec[i] = errors[i];
    }

    std::vector<int32_t> idVec;
    idVec.resize(count);
    for (size_t i = 0; i < count; i++) {
        idVec[i] = ids[i];
    }

    TO_AIDL_CLIENT();
    mGnssCbIface->gnssRemoveGeofencesCb(count, errorVec, idVec);
}

void LocAidlGeofenceClient::onPauseGeofencesCb(
        size_t count, LocationError* errors, uint32_t* ids)
{
    ENTRY_LOG();

    // Sanity checks
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }
    if (count <= 0) {
        LOC_LOGE("Invalid count %zu", count);
        return;
    }

    std::vector<int32_t> errorVec;
    errorVec.resize(count);
    for (size_t i = 0; i < count; i++) {
        errorVec[i] = errors[i];
    }

    std::vector<int32_t> idVec;
    idVec.resize(count);
    for (size_t i = 0; i < count; i++) {
        idVec[i] = ids[i];
    }

    TO_AIDL_CLIENT();
    mGnssCbIface->gnssPauseGeofencesCb(count, errorVec, idVec);
}

void LocAidlGeofenceClient::onResumeGeofencesCb(
        size_t count, LocationError* errors, uint32_t* ids)
{
    ENTRY_LOG();

    // Sanity checks
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }
    if (count <= 0) {
        LOC_LOGE("Invalid count %zu", count);
        return;
    }

    std::vector<int32_t> errorVec;
    errorVec.resize(count);
    for (size_t i = 0; i < count; i++) {
        errorVec[i] = errors[i];
    }

    std::vector<int32_t> idVec;
    idVec.resize(count);
    for (size_t i = 0; i < count; i++) {
        idVec[i] = ids[i];
    }

    TO_AIDL_CLIENT();
    mGnssCbIface->gnssResumeGeofencesCb(count, errorVec, idVec);
}

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
