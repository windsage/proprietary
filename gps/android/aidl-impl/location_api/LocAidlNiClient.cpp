/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_NiClient"
#define LOG_NDEBUG 0

#include <LocContext.h>
#include <LocationAPI.h>
#include <log_util.h>
#include <loc_gps.h>
#include "LocAidlNiClient.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

LocAidlNiClient* LocAidlNiClient::sNiClient = nullptr;

LocAidlNiClient* LocAidlNiClient::getInstance() {
    if (sNiClient == nullptr) {
        sNiClient = new LocAidlNiClient();
    }
    return sNiClient;
}

LocAidlNiClient::LocAidlNiClient() : LocationAPIClientBase() {}


void LocAidlNiClient::setCallback(const std::shared_ptr<ILocAidlGnssNiCallback>& callback) {

    ENTRY_LOG();
    mGnssCbIface = callback;
    LocationCallbacks locationCallbacks;
    memset(&locationCallbacks, 0, sizeof(LocationCallbacks));
    locationCallbacks.size = sizeof(LocationCallbacks);

    locationCallbacks.trackingCb = nullptr;
    locationCallbacks.batchingCb = nullptr;
    locationCallbacks.geofenceBreachCb = nullptr;
    locationCallbacks.geofenceStatusCb = nullptr;
    locationCallbacks.gnssLocationInfoCb = nullptr;

    locationCallbacks.gnssNiCb = [this](uint32_t id,
            const GnssNiNotification& gnssNiNotification) {
        onGnssNiCb(id, gnssNiNotification);
    };

    locationCallbacks.gnssSvCb = nullptr;
    locationCallbacks.gnssNmeaCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;

    locAPISetCallbacks(locationCallbacks);
}

void LocAidlNiClient::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) {
    LOC_LOGv("capabilities mask 0x%" PRIx64 "", capabilitiesMask);

    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }

    uint32_t capabilities = 0;
    if ((capabilitiesMask & LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT) ||
            (capabilitiesMask & LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT) ||
            (capabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT) ||
            (capabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT))
        capabilities |= LOC_GPS_CAPABILITY_SCHEDULING;
    if (capabilitiesMask & LOCATION_CAPABILITIES_GEOFENCE_BIT)
        capabilities |= LOC_GPS_CAPABILITY_GEOFENCING;
    if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MEASUREMENTS_BIT)
        capabilities |= LOC_GPS_CAPABILITY_MEASUREMENTS;
    if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSB_BIT)
        capabilities |= LOC_GPS_CAPABILITY_MSB;
    if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSA_BIT)
        capabilities |= LOC_GPS_CAPABILITY_MSA;

    TO_AIDL_CLIENT();
    mGnssCbIface->gnssCapabilitiesCb(capabilities);
}

void LocAidlNiClient::onGnssNiCb(uint32_t id, const GnssNiNotification& gnssNiNotification)
{
    ENTRY_LOG();

    LocAidlGnssNiNotification cbNotification;
    memset(&cbNotification, 0, sizeof(cbNotification));

    cbNotification.notificationId = id;

    if (gnssNiNotification.type == GNSS_NI_TYPE_VOICE)
        cbNotification.niType = LocAidlGnssNiType::VOICE;
    else if (gnssNiNotification.type == GNSS_NI_TYPE_SUPL)
        cbNotification.niType = LocAidlGnssNiType::UMTS_SUPL;
    else if (gnssNiNotification.type == GNSS_NI_TYPE_CONTROL_PLANE)
        cbNotification.niType = LocAidlGnssNiType::UMTS_CTRL_PLANE;
    else if (gnssNiNotification.type == GNSS_NI_TYPE_EMERGENCY_SUPL)
        cbNotification.niType = LocAidlGnssNiType::EMERGENCY_SUPL;

    if (gnssNiNotification.options & GNSS_NI_OPTIONS_NOTIFICATION_BIT)
        cbNotification.notifyFlags |= 0x0001;
    if (gnssNiNotification.options & GNSS_NI_OPTIONS_VERIFICATION_BIT)
        cbNotification.notifyFlags |= 0x0002;
    if (gnssNiNotification.options & GNSS_NI_OPTIONS_PRIVACY_OVERRIDE_BIT)
        cbNotification.notifyFlags |= 0x0004;

    cbNotification.timeoutSec = gnssNiNotification.timeout;

    if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_ACCEPT)
        cbNotification.defaultResponse = LocAidlGnssUserResponseType::RESPONSE_ACCEPT;
    else if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_DENY)
        cbNotification.defaultResponse = LocAidlGnssUserResponseType::RESPONSE_DENY;
    else if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_NO_RESPONSE ||
            gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_IGNORE)
        cbNotification.defaultResponse = LocAidlGnssUserResponseType::RESPONSE_NORESP;

    cbNotification.requestorId = gnssNiNotification.requestor;
    cbNotification.notificationMessage = gnssNiNotification.message;

    if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_NONE)
        cbNotification.requestorIdEncoding =
            LocAidlGnssNiEncodingType::ENC_NONE;
    else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_GSM_DEFAULT)
        cbNotification.requestorIdEncoding =
            LocAidlGnssNiEncodingType::ENC_SUPL_GSM_DEFAULT;
    else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_UTF8)
        cbNotification.requestorIdEncoding =
            LocAidlGnssNiEncodingType::ENC_SUPL_UTF8;
    else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_UCS2)
        cbNotification.requestorIdEncoding =
            LocAidlGnssNiEncodingType::ENC_SUPL_UCS2;

    if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_NONE)
        cbNotification.notificationIdEncoding =
            LocAidlGnssNiEncodingType::ENC_NONE;
    else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_GSM_DEFAULT)
        cbNotification.notificationIdEncoding =
            LocAidlGnssNiEncodingType::ENC_SUPL_GSM_DEFAULT;
    else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_UTF8)
        cbNotification.notificationIdEncoding =
            LocAidlGnssNiEncodingType::ENC_SUPL_UTF8;
    else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_UCS2)
        cbNotification.notificationIdEncoding =
            LocAidlGnssNiEncodingType::ENC_SUPL_UCS2;

    cbNotification.extras = gnssNiNotification.extras;
    cbNotification.esEnabled = false;

    TO_AIDL_CLIENT();
    mGnssCbIface->niNotifyCbExt(cbNotification);
}

void LocAidlNiClient::gnssNiRespond(int32_t notifId,
        LocAidlGnssUserResponseType userResponse) {

    LOC_LOGd("(%d %d)", notifId, static_cast<int>(userResponse));

    GnssNiResponse data = GNSS_NI_RESPONSE_IGNORE;
    if (userResponse == LocAidlGnssUserResponseType::RESPONSE_ACCEPT)
        data = GNSS_NI_RESPONSE_ACCEPT;
    else if (userResponse == LocAidlGnssUserResponseType::RESPONSE_DENY)
        data = GNSS_NI_RESPONSE_DENY;
    else if (userResponse == LocAidlGnssUserResponseType::RESPONSE_NORESP)
        data = GNSS_NI_RESPONSE_NO_RESPONSE;

    locAPIGnssNiResponse(notifId, data);
}

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
