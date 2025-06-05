/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Z-location Response Handler

  Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include<IzatProvider.h>
#define LOG_TAG "Z_Location_Response_Handler"
using namespace izat_zaxis;

void ZlocationResponseHandler::reportLocation(const LocationReport* flpLocationReport, int rptCnt,
        const ILocationProvider* provider) {
    for (int i = 0; i < rptCnt; ++i) {
        Location locationReport = {};
        convertToLocation(static_cast<const FusedLocationReport*>(flpLocationReport + i),
                locationReport);
        mIzatProvider->broadCastLocationWithZ(locationReport);
    }
}

void ZlocationResponseHandler::reportError(const IZatLocationError* izatError,
        const ILocationProvider* provider) {
//TODO
}

void ZlocationResponseHandler::convertToLocation(const FusedLocationReport* in,
        Location& locationReport) {
    locationReport.size = sizeof(Location);
    if (in->mHasLatitude && in->mHasLongitude) {
        locationReport.flags |= LOCATION_HAS_LAT_LONG_BIT;
        locationReport.latitude = in->mLatitude;
        locationReport.longitude = in->mLongitude;
    }

    if (in->mHasAltitudeWrtEllipsoid) {
        locationReport.flags |= LOCATION_HAS_ALTITUDE_BIT;
        locationReport.altitude = in->mAltitudeWrtEllipsoid;
    }

    if (in->mHasSpeed) {
        locationReport.flags |= LOCATION_HAS_SPEED_BIT;
        locationReport.speed = in->mSpeed;
    }

    if (in->mHasBearing) {
        locationReport.flags |= LOCATION_HAS_BEARING_BIT;
        locationReport.bearing = in->mBearing;
    }

    if (in->mHasBearingUnc) {
        locationReport.flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
        locationReport.bearingAccuracy = in->mBearingUnc;
    }

    if (in->mHasHorizontalAccuracy) {
        locationReport.flags |= LOCATION_HAS_ACCURACY_BIT;
        locationReport.accuracy = in->mHorizontalAccuracy;
    }

    if (in->mHasVertUnc) {
        locationReport.flags |= LOCATION_HAS_VERTICAL_ACCURACY_BIT;
        locationReport.verticalAccuracy = in->mVertUnc;
    }

    if (in->mHasSpeedUnc) {
        locationReport.flags |= LOCATION_HAS_SPEED_ACCURACY_BIT;
        locationReport.speedAccuracy = in->mSpeedUnc;
    }
    if (in->mHasUtcTimestampInMsec) {
        locationReport.timestamp = in->mUtcTimestampInMsec;
    }

    if (in->mHasPositionTechMask) {
        locationReport.techMask = in->mPositionTechMask;
    }
    locationReport.spoofMask = in->mSpoofMask;
}

void  ZlocationResponseHandler::convertToFusedLocationReport(FusedLocationReport& out,
        Location& in) {
    if (in.flags & LOCATION_HAS_LAT_LONG_BIT) {
        out.mHasLatitude = true;
        out.mHasLongitude = true;
        out.mLatitude = in.latitude;
        out.mLongitude = in.longitude;
    }

    if (in.flags & LOCATION_HAS_ALTITUDE_BIT) {
        out.mHasAltitudeWrtEllipsoid = true;
        out.mHasAltitudeWrtEllipsoid = in.altitude ;
    }

    if (in.flags & LOCATION_HAS_SPEED_BIT) {
        out.mHasSpeed =true;
        out.mSpeed = in.speed;
    }

    if (in.flags & LOCATION_HAS_BEARING_BIT) {
        out.mHasBearing = true;
        out.mBearing = in.bearing;
    }

    if (in.flags & LOCATION_HAS_BEARING_ACCURACY_BIT) {
        out.mHasBearingUnc = true;
        out.mBearingUnc = in.bearingAccuracy;
    }

    if (in.flags & LOCATION_HAS_ACCURACY_BIT) {
        out.mHasHorizontalAccuracy = true;
        out.mHorizontalAccuracy = in.accuracy;
    }

    if (in.flags & LOCATION_HAS_VERTICAL_ACCURACY_BIT) {
        out.mHasVertUnc = true;
        out.mVertUnc = in.verticalAccuracy;
    }

    if (in.flags & LOCATION_HAS_SPEED_ACCURACY_BIT) {
        out.mHasSpeedUnc = true;
        out.mSpeedUnc = in.speedAccuracy;
    }

    struct timespec tp;
    clock_gettime(CLOCK_BOOTTIME, &tp);
    out.mHasElapsedRealTimeInNanoSecs = true;
    out.mElapsedRealTimeInNanoSecs =
        (int64_t)tp.tv_sec * 1000000000 + (int64_t)tp.tv_nsec;

    out.mHasUtcTimestampInMsec = true;
    out.mUtcTimestampInMsec = in.timestamp;

    out.mHasPositionTechMask = true;
    out.mPositionTechMask = in.techMask;
    out.mSpoofMask = in.spoofMask ;
    out.mIsValid = true;
}
