/*
 * Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_Utils"
#define LOG_NDEBUG 0
#include <log_util.h>
#include <inttypes.h>
#include "LocAidlUtils.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::LocAidlNetworkPositionSourceType;
using ::aidl::vendor::qti::gnss::LocAidlIzatHorizontalAccuracy;
using ::aidl::vendor::qti::gnss::LocAidlIzatOtherAccuracy;
using ::aidl::vendor::qti::gnss::LocAidlLocation;
using ::aidl::vendor::qti::gnss::LocAidlAddress;
using ::aidl::vendor::qti::gnss::LocAidlIzatLocation;
using ::aidl::vendor::qti::gnss::LocAidlIzatStreamType;
using ::aidl::vendor::qti::gnss::LocAidlIzatRequest;
using ::aidl::vendor::qti::gnss::LocAidlSubscriptionDataItemId;
using ::aidl::vendor::qti::gnss::LocAidlIzatProviderStatus;

void LocAidlDeathRecipient::registerToPeer(AIBinder* binder, void* cookie) {
    mPeerDied = true;
    if (nullptr == binder || nullptr == cookie) {
        LOC_LOGE("binder or cookie is null");
        return;
    }
    binder_status_t r = AIBinder_linkToDeath(binder, mDeathRecipient, cookie);
    if (STATUS_OK != r) {
        LOC_LOGE("Failed to register death recipient [%u]", r);
    } else {
        mPeerDied = false;
    }
}

void LocAidlDeathRecipient::unregisterFromPeer(AIBinder* binder, void* cookie) {
    mPeerDied = true;
    if (nullptr == binder || nullptr == cookie) {
        LOC_LOGE("binder or cookie is null");
        return;
    }
    binder_status_t r = AIBinder_unlinkToDeath(binder, mDeathRecipient, cookie);
    if (STATUS_OK != r) {
        LOC_LOGE("Failed to unregister death recipient [%u]", r);
    }
}
/**
void LocAidlDeathRecipient::serviceDied(uint64_t cookie,
        const ::android::wp<::android::hidl::base::IBase>& who) {
    mPeerDied = true;
    LOC_LOGE("%s] Peer died. cookie: %" PRIu64 ", who: %p",
            __FUNCTION__, cookie, &who);
    if (mFun != nullptr) {
        mFun();
    }
}
*/
void LocAidlUtils::locationToLocAidlLocation(
        const Location& loc, LocAidlLocation& gl) {

    gl.locationFlagsMask = loc.flags;
    gl.timestamp = loc.timestamp;
    gl.latitude = loc.latitude;
    gl.longitude = loc.longitude;
    gl.altitude = loc.altitude;
    gl.speed = loc.speed;
    gl.bearing = loc.bearing;
    gl.accuracy = loc.accuracy;
    gl.verticalAccuracy = loc.verticalAccuracy;
    gl.speedAccuracy = loc.speedAccuracy;
    gl.bearingAccuracy = loc.bearingAccuracy;
    gl.locationTechnologyMask = loc.techMask;
    gl.conformityIndex = loc.conformityIndex;
    gl.elapsedRealTime = loc.elapsedRealTime;
    gl.elapsedRealTimeUnc = loc.elapsedRealTimeUnc;
    switch (loc.qualityType) {
        case LOCATION_STANDALONE_QUALITY_TYPE:
            gl.qualityType = LocAidlLocationQualityType::STANDALONE_QUALITY_TYPE;
        break;
        case LOCATION_DGNSS_QUALITY_TYPE:
            gl.qualityType = LocAidlLocationQualityType::DGNSS_QUALITY_TYPE;
        break;
        case LOCATION_FLOAT_QUALITY_TYPE:
            gl.qualityType = LocAidlLocationQualityType::FLOAT_QUALITY_TYPE;
        break;
        case LOCATION_FIXED_QUALITY_TYPE:
            gl.qualityType = LocAidlLocationQualityType::FIXED_QUALITY_TYPE;
        break;
    }
}

void LocAidlUtils::LocAidlLocationToLocation(
        const LocAidlLocation& loc, Location& gl) {

    gl.flags = loc.locationFlagsMask;
    gl.timestamp = loc.timestamp;
    gl.latitude = loc.latitude;
    gl.longitude = loc.longitude;
    gl.altitude = loc.altitude;
    gl.speed = loc.speed;
    gl.bearing = loc.bearing;
    gl.accuracy = loc.accuracy;
    gl.verticalAccuracy = loc.verticalAccuracy;
    gl.speedAccuracy = loc.speedAccuracy;
    gl.bearingAccuracy = loc.bearingAccuracy;
    gl.techMask = loc.locationTechnologyMask;
    gl.conformityIndex = loc.conformityIndex;
    gl.elapsedRealTime = loc.elapsedRealTime;
    gl.elapsedRealTimeUnc = loc.elapsedRealTimeUnc;
    switch (loc.qualityType) {
        case LocAidlLocationQualityType::STANDALONE_QUALITY_TYPE:
            gl.qualityType = LOCATION_STANDALONE_QUALITY_TYPE;
        break;
        case LocAidlLocationQualityType::DGNSS_QUALITY_TYPE:
            gl.qualityType = LOCATION_DGNSS_QUALITY_TYPE;
        break;
        case LocAidlLocationQualityType::FLOAT_QUALITY_TYPE:
            gl.qualityType = LOCATION_FLOAT_QUALITY_TYPE;
        break;
        case LocAidlLocationQualityType::FIXED_QUALITY_TYPE:
            gl.qualityType = LOCATION_FIXED_QUALITY_TYPE;
        break;
    }
}

void LocAidlUtils::LocAidlAddressToCivicAddress(
        const LocAidlAddress& addr, GnssCivicAddress& gnssAddr) {
    gnssAddr.size = sizeof(GnssCivicAddress);

    gnssAddr.adminArea = addr.adminArea;
    gnssAddr.countryCode = addr.countryCode;
    gnssAddr.countryName = addr.countryName;
    gnssAddr.featureName = addr.featureName;
    gnssAddr.hasLatitude = addr.hasLatitude;
    gnssAddr.latitude = addr.latitude;
    gnssAddr.hasLongitude = addr.hasLongitude;
    gnssAddr.longitude = addr.longitude;
    gnssAddr.locale = addr.locale;
    gnssAddr.locality = addr.locality;
    gnssAddr.phone = addr.phone;
    gnssAddr.postalCode = addr.postalCode;
    gnssAddr.premises = addr.premises;
    gnssAddr.subAdminArea = addr.subAdminArea;
    gnssAddr.subLocality = addr.subLocality;
    gnssAddr.thoroughfare = addr.thoroughfare;
    gnssAddr.subThoroughfare = addr.subThoroughfare;
    gnssAddr.url = addr.url;
}

void LocAidlUtils::izatLocationToGnssIzatLocation(
        const IzatLocation& il, LocAidlIzatLocation& gil) {

    gil.hasUtcTimestampInMsec = il.mHasUtcTimestampInMsec;
    gil.utcTimestampInMsec = il.mUtcTimestampInMsec;

    gil.hasElapsedRealTimeInNanoSecs = il.mHasElapsedRealTimeInNanoSecs;
    gil.elapsedRealTimeInNanoSecs = il.mElapsedRealTimeInNanoSecs;

    gil.hasLatitude = il.mHasLatitude;
    gil.latitude = il.mLatitude;

    gil.hasLongitude = il.mHasLongitude;
    gil.longitude = il.mLongitude;

    gil.hasHorizontalAccuracy = il.mHasHorizontalAccuracy;
    gil.horizontalAccuracy = il.mHorizontalAccuracy;

    gil.hasAltitudeWrtEllipsoid = il.mHasAltitudeWrtEllipsoid;
    gil.altitudeWrtEllipsoid = il.mAltitudeWrtEllipsoid;

    gil.hasAltitudeWrtMeanSeaLevel = il.mHasAltitudeWrtMeanSeaLevel;
    gil.altitudeWrtMeanSeaLevel = il.mAltitudeWrtMeanSeaLevel;

    gil.hasBearing = il.mHasBearing;
    gil.bearing = il.mBearing;

    gil.hasSpeed = il.mHasSpeed;
    gil.speed = il.mSpeed;

    gil.position_source = il.mPositionSource;

    gil.hasAltitudeMeanSeaLevel = il.mHasAltitudeMeanSeaLevel;
    gil.altitudeMeanSeaLevel = il.mAltitudeMeanSeaLevel;

    gil.hasDop = il.mHasDop;
    gil.pDop = il.mPdop;
    gil.hDop = il.mHdop;
    gil.vDop = il.mVdop;

    gil.hasMagneticDeviation = il.mHasMagneticDeviation;
    gil.magneticDeviation = il.mMagneticDeviation;

    gil.hasVertUnc = il.mHasVertUnc;
    gil.vertUnc = il.mVertUnc;

    gil.hasSpeedUnc = il.mHasSpeedUnc;
    gil.speedUnc = il.mSpeedUnc;

    gil.hasBearingUnc = il.mHasBearingUnc;
    gil.bearingUnc = il.mBearingUnc;

    gil.hasHorizontalReliability = il.mHasHorizontalReliability;
    gil.horizontalReliability = (uint16_t)il.mHorizontalReliability;

    gil.hasVerticalReliability = il.mHasVerticalReliability;
    gil.verticalReliability = (uint16_t)il.mVerticalReliability;

    gil.hasHorUncEllipseSemiMajor = il.mHasHorUncEllipseSemiMajor;
    gil.horUncEllipseSemiMajor = il.mHorUncEllipseSemiMajor;

    gil.hasHorUncEllipseSemiMinor = il.mHasHorUncEllipseSemiMinor;
    gil.horUncEllipseSemiMinor = il.mHorUncEllipseSemiMinor;

    gil.hasHorUncEllipseOrientAzimuth = il.mHasHorUncEllipseOrientAzimuth;
    gil.horUncEllipseOrientAzimuth = il.mHorUncEllipseOrientAzimuth;

    gil.hasNetworkPositionSource = il.mHasNetworkPositionSource;
    gil.networkPositionSource =
            static_cast<LocAidlNetworkPositionSourceType>(il.mNetworkPositionSource);

    gil.hasAltitudeRefType = il.mHasAltitudeRefType;
    gil.altitudeRefType =
            static_cast<LocAidlAltitudeRefType>(il.mAltitudeRefType);
//    gil.hasNavSolutionMask = il.mHasNavSolutionMask;
//    gil.navSolutionMask = il.mNavSolutionMask;
//
//    gil.hasPositionTechMask = il.mHasPositionTechMask;
//    gil.positionTechMask = il.mPositionTechMask;
}

DataItemId LocAidlUtils::translateToDataItemId(LocAidlSubscriptionDataItemId id) {

    DataItemId ret = INVALID_DATA_ITEM_ID;
    switch (id) {
        case LocAidlSubscriptionDataItemId::AIRPLANEMODE_DATA_ITEM_ID :
            ret = AIRPLANEMODE_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::ENH_DATA_ITEM_ID :
            ret = ENH_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::NLPSTATUS_DATA_ITEM_ID :
            ret = NLPSTATUS_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::WIFIHARDWARESTATE_DATA_ITEM_ID :
            ret = WIFIHARDWARESTATE_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::NETWORKINFO_DATA_ITEM_ID :
            ret = NETWORKINFO_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::RILVERSION_DATA_ITEM_ID :
            ret = RILVERSION_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::RILSERVICEINFO_DATA_ITEM_ID :
            ret = RILSERVICEINFO_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::RILCELLINFO_DATA_ITEM_ID :
            ret = RILCELLINFO_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::SERVICESTATUS_DATA_ITEM_ID :
            ret = SERVICESTATUS_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::MODEL_DATA_ITEM_ID :
            ret = MODEL_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::MANUFACTURER_DATA_ITEM_ID :
            ret = MANUFACTURER_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::VOICECALL_DATA_ITEM :
            ret = VOICECALL_DATA_ITEM;
            break;
        case LocAidlSubscriptionDataItemId::ASSISTED_GPS_DATA_ITEM_ID :
            ret = ASSISTED_GPS_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::SCREEN_STATE_DATA_ITEM_ID :
            ret = SCREEN_STATE_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::TIMEZONE_CHANGE_DATA_ITEM_ID :
            ret = TIMEZONE_CHANGE_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::TIME_CHANGE_DATA_ITEM_ID :
            ret = TIME_CHANGE_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID :
            ret = WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::SHUTDOWN_STATE_DATA_ITEM_ID :
            ret = SHUTDOWN_STATE_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::TAC_DATA_ITEM_ID :
            ret = TAC_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::MCCMNC_DATA_ITEM_ID :
            ret = MCCMNC_DATA_ITEM_ID;
            break;
        case LocAidlSubscriptionDataItemId::MAX_DATA_ITEM_ID :
            ret = MAX_DATA_ITEM_ID;
            break;
        default:
            ret = INVALID_DATA_ITEM_ID;
            break;
    }
    return ret;
}

void LocAidlUtils::izatRequestToGnssIzatRequest(
        const IzatRequest& ir, LocAidlIzatRequest& gir) {

    gir.provider = static_cast<LocAidlIzatStreamType>(ir.getProvider());
    gir.numUpdates = ir.getNumUpdates();
    gir.suggestedResponseTimeForFirstFix = ir.getTimeForFirstFix();
    gir.timeIntervalBetweenFixes = ir.getInterval();
    gir.smallestDistanceBetweenFixes = ir.getDistance();
    gir.suggestedHorizontalAccuracy =
            static_cast<LocAidlIzatHorizontalAccuracy>(ir.getHorizontalAccuracy());
    gir.suggestedAltitudeAccuracy =
            static_cast<LocAidlIzatOtherAccuracy>(ir.getAltitudeAccuracy());
    gir.suggestedBearingAccuracy =
            static_cast<LocAidlIzatOtherAccuracy>(ir.getBearingAccuracy());
}

LocAidlSubscriptionDataItemId
LocAidlUtils::translateToSubscriptionDataItemId(DataItemId id) {

    LocAidlSubscriptionDataItemId ret =
            LocAidlSubscriptionDataItemId::INVALID_DATA_ITEM_ID;
    switch (id) {
        case AIRPLANEMODE_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::AIRPLANEMODE_DATA_ITEM_ID;
            break;
        case ENH_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::ENH_DATA_ITEM_ID;
            break;
        case NLPSTATUS_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::NLPSTATUS_DATA_ITEM_ID;
            break;
        case WIFIHARDWARESTATE_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::WIFIHARDWARESTATE_DATA_ITEM_ID;
            break;
        case NETWORKINFO_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::NETWORKINFO_DATA_ITEM_ID;
            break;
        case RILVERSION_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::RILVERSION_DATA_ITEM_ID;
            break;
        case RILSERVICEINFO_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::RILSERVICEINFO_DATA_ITEM_ID;
            break;
        case RILCELLINFO_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::RILCELLINFO_DATA_ITEM_ID;
            break;
        case SERVICESTATUS_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::SERVICESTATUS_DATA_ITEM_ID;
            break;
        case MODEL_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::MODEL_DATA_ITEM_ID;
            break;
        case MANUFACTURER_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::MANUFACTURER_DATA_ITEM_ID;
            break;
        case VOICECALL_DATA_ITEM :
            ret = LocAidlSubscriptionDataItemId::VOICECALL_DATA_ITEM;
            break;
        case ASSISTED_GPS_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::ASSISTED_GPS_DATA_ITEM_ID;
            break;
        case SCREEN_STATE_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::SCREEN_STATE_DATA_ITEM_ID;
            break;
        case TIMEZONE_CHANGE_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::TIMEZONE_CHANGE_DATA_ITEM_ID;
            break;
        case TIME_CHANGE_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::TIME_CHANGE_DATA_ITEM_ID;
            break;
        case WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID;
            break;
        case SHUTDOWN_STATE_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::SHUTDOWN_STATE_DATA_ITEM_ID;
            break;
        case TAC_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::TAC_DATA_ITEM_ID;
            break;
        case MCCMNC_DATA_ITEM_ID :
            ret = LocAidlSubscriptionDataItemId::MCCMNC_DATA_ITEM_ID;
            break;
        case MAX_DATA_ITEM_ID:
            ret = LocAidlSubscriptionDataItemId::MAX_DATA_ITEM_ID;
            break;
        default:
            break;
    }
    return ret;
}

IzatStreamType LocAidlUtils::gnssIzatStreamToIzatStreamType(LocAidlIzatStreamType gist) {
    IzatStreamType izatType = IZAT_STREAM_ALL;
    switch (gist) {
        case LocAidlIzatStreamType::FUSED:
        {
            izatType = IZAT_STREAM_FUSED;
            break;
        }
        case LocAidlIzatStreamType::NETWORK:
        {
            izatType = IZAT_STREAM_NETWORK;
            break;
        }
        case LocAidlIzatStreamType::GNSS:
        {
            izatType = IZAT_STREAM_GNSS;
            break;
        }
        case LocAidlIzatStreamType::NMEA:
        {
            izatType = IZAT_STREAM_NMEA;
            break;
        }
        case LocAidlIzatStreamType::DR:
        {
            izatType = IZAT_STREAM_DR;
            break;
        }
        case LocAidlIzatStreamType::GNSS_SVINFO:
        {
            izatType = IZAT_STREAM_GNSS_SVINFO;
            break;
        }
        case LocAidlIzatStreamType::DR_SVINFO:
        {
            izatType = IZAT_STREAM_DR_SVINFO;
            break;
        }
        case LocAidlIzatStreamType::ALL:
        {
            izatType = IZAT_STREAM_ALL;
            break;
        }
    }
    return izatType;
}

}  // namespace implementation
}  // namespace AIDL
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
