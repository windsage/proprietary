/*
Copyright (c) 2017-2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Not a contribution.
*/
/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "GARDEN_GfwAidlApi"
#define GNSS_VENDOR_SERVICE_INSTANCE "gnss_vendor"

#include "GardenUtil.h"
#include <loc_cfg.h>
#include "GfwAidlApi.h"
#include <mutex>
#include <algorithm>
#include <memory>
#include "loc_ril.h"

#include <android/binder_auto_utils.h>
#include <android/binder_ibinder_platform.h>
#include <android/binder_libbinder.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#define GEOFENCE_BREACH_ENTERED            (1<<0L)
#define GEOFENCE_BREACH_EXITED             (1<<1L)
#define GEOFENCE_BREACH_UNCERTAIN          (1<<2L)

#define GNSS_DEBUG_UNKNOWN_HORIZONTAL_ACCURACY_METERS (20000000)
#define GNSS_DEBUG_UNKNOWN_VERTICAL_ACCURACY_METERS   (20000)
#define GNSS_DEBUG_UNKNOWN_SPEED_ACCURACY_PER_SEC     (500)
#define GNSS_DEBUG_UNKNOWN_BEARING_ACCURACY_DEG       (180)

#define GNSS_DEBUG_UNKNOWN_UTC_TIME            (1483228800000LL) // 1/1/2017 00:00 GMT
#define GNSS_DEBUG_UNKNOWN_UTC_TIME_UNC_MIN    (999) // 999 ns
#define GNSS_DEBUG_UNKNOWN_UTC_TIME_UNC_MAX    (1.57783680E17) // 5 years in ns
#define GNSS_DEBUG_UNKNOWN_FREQ_UNC_NS_PER_SEC (2.0e5)  // ppm

#include <aidl/android/hardware/gnss/BnGnssMeasurementInterface.h>
#include <aidl/android/hardware/gnss/BnGnssMeasurementCallback.h>
#include <aidl/android/hardware/gnss/visibility_control/BnGnssVisibilityControl.h>
#include <aidl/android/hardware/gnss/visibility_control/BnGnssVisibilityControlCallback.h>
#include <aidl/android/hardware/gnss/BnGnss.h>
#include <aidl/android/hardware/gnss/BnGnssCallback.h>
#include <aidl/android/hardware/gnss/BnAGnssCallback.h>
#include <aidl/android/hardware/gnss/BnAGnssRilCallback.h>
#include <aidl/android/hardware/gnss/BnGnssBatchingCallback.h>
#include <aidl/android/hardware/gnss/BnGnssGeofenceCallback.h>
#include <aidl/android/hardware/gnss/BnGnssConfiguration.h>
using ::aidl::android::hardware::gnss::GnssLocation;
using ::aidl::android::hardware::gnss::GnssMeasurement;
using ::aidl::android::hardware::gnss::GnssData;
using ::aidl::android::hardware::gnss::ElapsedRealtime;
using ::aidl::android::hardware::gnss::BlocklistedSource;
using ::aidl::android::hardware::gnss::GnssConstellationType;
using ::aidl::android::hardware::gnss::GnssClock;
using ::aidl::android::hardware::gnss::BnGnssMeasurementCallback;
using ::aidl::android::hardware::gnss::BnGnss;
using ::aidl::android::hardware::gnss::IGnss;
using ::aidl::android::hardware::gnss::IGnssCallback;
using ::aidl::android::hardware::gnss::BnGnssCallback;
using ::aidl::android::hardware::gnss::BnGnssBatchingCallback;
using ::aidl::android::hardware::gnss::BnGnssGeofenceCallback;
using ::aidl::android::hardware::gnss::IGnssMeasurementInterface;
using ::aidl::android::hardware::gnss::SatellitePvt;
using ::std::shared_ptr;
using ::aidl::android::hardware::gnss::IGnssConfiguration;
using IGnssAidl = ::aidl::android::hardware::gnss::IGnss;
using aidl::android::hardware::gnss::IGnssGeofenceCallback;
using aidl::android::hardware::gnss::IAGnssRil;
using aidl::android::hardware::gnss::IAGnssRilCallback;
using aidl::android::hardware::gnss::IAGnss;
using aidl::android::hardware::gnss::IAGnssCallback;
using ::aidl::android::hardware::gnss::BnAGnssCallback;
using ::aidl::android::hardware::gnss::BnAGnssRilCallback;
using aidl::android::hardware::gnss::IGnssBatching;
using aidl::android::hardware::gnss::IGnssGeofence;
using aidl::android::hardware::gnss::IGnssDebug;
using ::aidl::android::hardware::gnss::measurement_corrections::IMeasurementCorrectionsInterface;
using ::aidl::android::hardware::gnss::visibility_control::IGnssVisibilityControl;
using ::aidl::android::hardware::gnss::visibility_control::BnGnssVisibilityControlCallback;
using ::aidl::android::hardware::gnss::visibility_control::IGnssVisibilityControlCallback;
using ::aidl::android::hardware::gnss::GnssSignalType;
std::shared_ptr<IGnssAidl> gnssHal = nullptr;

//Fix the re definition comile error
#undef LOC_DEFAULT_SUB
#undef LOC_PRIMARY_SUB
#undef LOC_SECONDARY_SUB
#undef LOC_TERTIARY_SUB
#include <aidl/vendor/qti/gnss/ILocAidlGnss.h>
#include <aidl/vendor/qti/gnss/BnLocAidlAGnssCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlFlpServiceCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlGeofenceServiceCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlIzatConfigCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlIzatSubscriptionCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlGnssNiCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlIzatProviderCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlGnssConfigServiceCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlWiFiDBProviderCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlWiFiDBReceiverCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlWWANDBProviderCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlWWANDBReceiverCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlEsStatusCallback.h>
using ::ndk::SharedRefBase;
using ::aidl::vendor::qti::gnss::ILocAidlGnss;
using ::aidl::vendor::qti::gnss::ILocAidlIzatConfig;
using ::aidl::vendor::qti::gnss::ILocAidlFlpService;
using ::aidl::vendor::qti::gnss::ILocAidlGeofenceService;
using ::aidl::vendor::qti::gnss::ILocAidlGnssNi;
using ::aidl::vendor::qti::gnss::ILocAidlFlpServiceCallback;
using ::aidl::vendor::qti::gnss::ILocAidlGeofenceServiceCallback;
using ::aidl::vendor::qti::gnss::ILocAidlGnssNiCallback;
using ::aidl::vendor::qti::gnss::LocAidlLocation;
using ::aidl::vendor::qti::gnss::LocAidlIzatLocation;
using ::aidl::vendor::qti::gnss::LocAidlBatchOptions;
using ::aidl::vendor::qti::gnss::LocAidlBatchStatusInfo;
using ::aidl::vendor::qti::gnss::ILocAidlIzatSubscription;
using ::aidl::vendor::qti::gnss::ILocAidlIzatSubscriptionCallback;
using ::aidl::vendor::qti::gnss::LocAidlSubscriptionDataItemId;
using ::aidl::vendor::qti::gnss::ILocAidlRilInfoMonitor;
using ::aidl::vendor::qti::gnss::ILocAidlIzatProvider;
using ::aidl::vendor::qti::gnss::ILocAidlIzatProviderCallback;
using ::aidl::vendor::qti::gnss::LocAidlNetworkPositionSourceType;
using ::aidl::vendor::qti::gnss::LocAidlIzatProviderStatus;
using ::aidl::vendor::qti::gnss::LocAidlIzatHorizontalAccuracy;
using ::aidl::vendor::qti::gnss::LocAidlIzatRequest;
using ::aidl::vendor::qti::gnss::LocAidlIzatStreamType;
using ::aidl::vendor::qti::gnss::ILocAidlIzatConfig;
using ::aidl::vendor::qti::gnss::ILocAidlIzatConfigCallback;
using ::aidl::vendor::qti::gnss::ILocAidlGnssConfigService;
using ::aidl::vendor::qti::gnss::ILocAidlGnssConfigServiceCallback;
using ::aidl::vendor::qti::gnss::ILocAidlAGnss;
using ::aidl::vendor::qti::gnss::ILocAidlAGnssCallback;
using ::aidl::vendor::qti::gnss::ILocAidlDebugReportService;
using ::aidl::vendor::qti::gnss::ILocAidlWiFiDBReceiver;
using ::aidl::vendor::qti::gnss::ILocAidlWiFiDBReceiverCallback;
using ::aidl::vendor::qti::gnss::ILocAidlWWANDBReceiver;
using ::aidl::vendor::qti::gnss::ILocAidlWWANDBReceiverCallback;
using ::aidl::vendor::qti::gnss::ILocAidlWWANDBProvider;
using ::aidl::vendor::qti::gnss::ILocAidlWWANDBProviderCallback;
using ::aidl::vendor::qti::gnss::ILocAidlWiFiDBProvider;
using ::aidl::vendor::qti::gnss::ILocAidlWiFiDBProviderCallback;
using ::aidl::vendor::qti::gnss::ILocAidlEsStatusReceiver;
using ::aidl::vendor::qti::gnss::ILocAidlEsStatusCallback;
using ::aidl::vendor::qti::gnss::LocAidlUlpLocation;
using ::aidl::vendor::qti::gnss::LocAidlWifiDBListStatus;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusReports;
using ::aidl::vendor::qti::gnss::LocAidlGnssNiNotification;
using ::aidl::vendor::qti::gnss::LocAidlRobustLocationInfo;
using ::aidl::vendor::qti::gnss::LocAidlGnssConstellationType;
using ::aidl::vendor::qti::gnss::LocAidlAGnssStatusIpV4;
using ::aidl::vendor::qti::gnss::LocAidlAGnssStatusIpV6;
using ::aidl::vendor::qti::gnss::LocAidlApInfo;
using ::aidl::vendor::qti::gnss::LocAidlWifiDBListStatus;
using ::aidl::vendor::qti::gnss::LocAidlBsInfo;
using ::aidl::vendor::qti::gnss::LocAidlApObsData;
using ::aidl::vendor::qti::gnss::LocAidlBSObsData;
using ::aidl::vendor::qti::gnss::LocAidlBoolDataItem;
using ::aidl::vendor::qti::gnss::LocAidlStringDataItem;
using ::aidl::vendor::qti::gnss::LocAidlNetworkInfoDataItem;
using ::aidl::vendor::qti::gnss::LocAidlRilServiceInfoDataItem;
using ::aidl::vendor::qti::gnss::LocAidlCellLteDataItem;
using ::aidl::vendor::qti::gnss::LocAidlCellGwDataItem;
using ::aidl::vendor::qti::gnss::LocAidlCellCdmaDataItem;
using ::aidl::vendor::qti::gnss::LocAidlCellOooDataItem;
using ::aidl::vendor::qti::gnss::LocAidlServiceStateDataItem;
using ::aidl::vendor::qti::gnss::LocAidlScreenStatusDataItem;
using ::aidl::vendor::qti::gnss::LocAidlPowerConnectStatusDataItem;
using ::aidl::vendor::qti::gnss::LocAidlTimeZoneChangeDataItem;
using ::aidl::vendor::qti::gnss::LocAidlTimeChangeDataItem;
using ::aidl::vendor::qti::gnss::LocAidlWifiSupplicantStatusDataItem;
using ::aidl::vendor::qti::gnss::LocAidlApLocationData;
using ::aidl::vendor::qti::gnss::LocAidlApSpecialInfo;
using ::aidl::vendor::qti::gnss::LocAidlBsLocationData;
using ::aidl::vendor::qti::gnss::LocAidlBsSpecialInfo;
using ::aidl::vendor::qti::gnss::LocAidlApnType;
using ::aidl::vendor::qti::gnss::LocAidlAGnssType;


using ::aidl::vendor::qti::gnss::BnLocAidlAGnssCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlFlpServiceCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlGeofenceServiceCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlIzatConfigCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlIzatSubscriptionCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlGnssNiCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlIzatProviderCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlGnssConfigServiceCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlWiFiDBProviderCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlWiFiDBReceiverCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlWWANDBProviderCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlWWANDBReceiverCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlEsStatusCallback;
using android::OK;
using android::sp;
using android::wp;

using std::mutex;
using std::min;
using namespace loc_core;

namespace garden {
std::shared_ptr<IGnss> gnssHal = nullptr;
std::shared_ptr<IAGnssRil> agnssRilIface = nullptr;
std::shared_ptr<IGnssGeofence> gnssGeofencingIface = nullptr;
std::shared_ptr<IAGnss> agnssIface = nullptr;
std::shared_ptr<IGnssBatching> gnssBatchingIface = nullptr;
std::shared_ptr<IGnssDebug> gnssDebugIface = nullptr;
std::shared_ptr<IMeasurementCorrectionsInterface> measurementCorrIface = nullptr;
std::shared_ptr<IGnssVisibilityControl> visibCtrlIface = nullptr;


std::shared_ptr<IGnssConfiguration> gnssConfigIface = nullptr;
std::shared_ptr<IGnssMeasurementInterface> gnssMeasurementIface = nullptr;
std::shared_ptr<ILocAidlGnss> locAidlHal = nullptr;
std::shared_ptr<ILocAidlFlpService> locAidlFlpIface = nullptr;
std::shared_ptr<ILocAidlGeofenceService> locAidlGeofenceIface = nullptr;
std::shared_ptr<ILocAidlGnssNi> gnssAidlNiIface = nullptr;
std::shared_ptr<ILocAidlIzatSubscription> gnssSubscriptionIface = nullptr;
std::shared_ptr<ILocAidlRilInfoMonitor> gnssRilInfoIface = nullptr;
std::shared_ptr<ILocAidlIzatProvider> izatProviderIface = nullptr;
std::shared_ptr<ILocAidlGnssConfigService> lochidlgnssConfigIface = nullptr;
std::shared_ptr<ILocAidlIzatConfig> izatConfigIface = nullptr;
std::shared_ptr<ILocAidlAGnss> agnssExtIface = nullptr;
std::shared_ptr<ILocAidlDebugReportService> debugReportIface = nullptr;
std::shared_ptr<ILocAidlWiFiDBReceiver> wifiDBReceiverIface = nullptr;
std::shared_ptr<ILocAidlWiFiDBProvider> wifiDBProviderIface = nullptr;
std::shared_ptr<ILocAidlWWANDBReceiver> wwanDBReceiverIface = nullptr;
std::shared_ptr<ILocAidlWWANDBProvider> wwanDBProviderIface = nullptr;
std::shared_ptr<ILocAidlEsStatusReceiver> esStatusReceiverIface = nullptr;
static GnssCbBase* sGnssCbs = nullptr;
static GnssCbBase* sFlpCbs = nullptr;
static GnssCbBase* sGeofenceCbs = nullptr;
static GnssCbBase* sAGnssCbs = nullptr;
static GnssCbBase* sAGnssRilCbs = nullptr;
static GnssCbBase* sGnssMeasurementCbs = nullptr;
static GnssCbBase* sVisibCtrlCbs = nullptr;
static mutex sGnssLock;
static mutex sFlpLock;
static mutex sGeofenceLock;
static mutex sAGnssLock;
static mutex sAGnssRilLock;
static mutex sGnssMeasurementLock;
static mutex sVisibCtrlLock;

static GnssCbBase* sDataItemCbs = nullptr;
static GnssCbBase* sIzatProviderCbs = nullptr;
static GnssCbBase* sGnssConfigCbs = nullptr;
static GnssCbBase* sIzatConfigCbs = nullptr;
static GnssCbBase* sAgnssExtCbs = nullptr;
static GnssCbBase* sWifiDBReceiverCbs = nullptr;
static GnssCbBase* sWifiDBProviderCbs = nullptr;
static GnssCbBase* sWwanDBReceiverCbs = nullptr;
static GnssCbBase* sWwanDBProviderCbs = nullptr;
static GnssCbBase* sEsStatusReceiverCbs = nullptr;
static mutex sDataItemLock;
static mutex sIzatProviderLock;
static mutex sOsNpLock;
static mutex sGnssConfigLock;
static mutex sIzatConfigLock;
static mutex sAgnssExtLock;
static mutex sWiFiDBReceiverLock;
static mutex sWiFiDBProviderLock;
static mutex sWwanDBReceiverLock;
static mutex sWwanDBProviderLock;
static mutex sEsStatusReceiverLock;

static void translateLocation(Location& location, const GnssLocation& gnssLocation) {
    location.size = sizeof(Location);
    location.flags = static_cast<uint16_t>(gnssLocation.gnssLocationFlags);
    location.timestamp = gnssLocation.timestampMillis;
    location.latitude = gnssLocation.latitudeDegrees;
    location.longitude = gnssLocation.longitudeDegrees;
    location.altitude = gnssLocation.altitudeMeters;
    location.speed = gnssLocation.speedMetersPerSec;
    location.bearing = gnssLocation.bearingDegrees;
    location.accuracy = gnssLocation.horizontalAccuracyMeters;
    location.verticalAccuracy = gnssLocation.horizontalAccuracyMeters;
    location.speedAccuracy = gnssLocation.speedAccuracyMetersPerSecond;
    location.bearingAccuracy = gnssLocation.bearingAccuracyDegrees;
}

static void translateToHidlLocation(GnssLocation& gnssLocation, const Location& location) {
    gnssLocation.gnssLocationFlags = location.flags ;
    gnssLocation.timestampMillis = location.timestamp;
    gnssLocation.latitudeDegrees = location.latitude;
    gnssLocation.longitudeDegrees = location.longitude;
    gnssLocation.altitudeMeters = location.altitude;
    gnssLocation.speedMetersPerSec = location.speed;
    gnssLocation.bearingDegrees = location.bearing;
    gnssLocation.horizontalAccuracyMeters = location.accuracy;
    gnssLocation.horizontalAccuracyMeters = location.verticalAccuracy;
    gnssLocation.speedAccuracyMetersPerSecond = location.speedAccuracy;
    gnssLocation.bearingAccuracyDegrees = location.bearingAccuracy;
}
static void translateElapsedRealtimeNanos(
        const ElapsedRealtime& elapsedRealtime, GnssMeasurementsNotification& out) {
    if (elapsedRealtime.flags & ElapsedRealtime::HAS_TIMESTAMP_NS) {
        out.clock.flags |= GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_BIT;
        out.clock.elapsedRealTime = elapsedRealtime.timestampNs;
    }
    if (elapsedRealtime.flags & ElapsedRealtime::HAS_TIME_UNCERTAINTY_NS) {
        out.clock.elapsedRealTimeUnc = elapsedRealtime.timeUncertaintyNs;
    }
}

static void translateGnssSatellitePvt(const GnssMeasurement& gnssMeas, GnssMeasurementsData& out) {
    // satPosEcef
    out.satellitePvt.satPosEcef.posXMeters = gnssMeas.satellitePvt.satPosEcef.posXMeters;
    out.satellitePvt.satPosEcef.posYMeters = gnssMeas.satellitePvt.satPosEcef.posYMeters;
    out.satellitePvt.satPosEcef.posZMeters = gnssMeas.satellitePvt.satPosEcef.posZMeters;
    out.satellitePvt.satPosEcef.ureMeters = gnssMeas.satellitePvt.satPosEcef.ureMeters;
    // satVelEcef
    out.satellitePvt.satVelEcef.velXMps = gnssMeas.satellitePvt.satVelEcef.velXMps;
    out.satellitePvt.satVelEcef.velYMps = gnssMeas.satellitePvt.satVelEcef.velYMps;
    out.satellitePvt.satVelEcef.velZMps = gnssMeas.satellitePvt.satVelEcef.velZMps;
    out.satellitePvt.satVelEcef.ureRateMps = gnssMeas.satellitePvt.satVelEcef.ureRateMps;
    // satClockInfo
    out.satellitePvt.satClockInfo.satHardwareCodeBiasMeters =
            gnssMeas.satellitePvt.satClockInfo.satHardwareCodeBiasMeters;
    out.satellitePvt.satClockInfo.satTimeCorrectionMeters =
            gnssMeas.satellitePvt.satClockInfo.satTimeCorrectionMeters;
    out.satellitePvt.satClockInfo.satClkDriftMps =
            gnssMeas.satellitePvt.satClockInfo.satClkDriftMps;
    // ionoDelayMeters
    out.satellitePvt.ionoDelayMeters = gnssMeas.satellitePvt.ionoDelayMeters;
    // tropoDelayMeters
    out.satellitePvt.tropoDelayMeters = gnssMeas.satellitePvt.tropoDelayMeters;
    // timeOfClockSeconds
    out.satellitePvt.TOC = gnssMeas.satellitePvt.timeOfClockSeconds;
    // issueOfDataClock
    out.satellitePvt.IODC = gnssMeas.satellitePvt.issueOfDataClock;
    // timeOfEphemerisSeconds
    out.satellitePvt.TOE = gnssMeas.satellitePvt.timeOfEphemerisSeconds;
    // issueOfDataEphemeris
    out.satellitePvt.IODE = gnssMeas.satellitePvt.issueOfDataEphemeris;
    // ephemerisSource
    switch (gnssMeas.satellitePvt.ephemerisSource) {
    case SatellitePvt::SatelliteEphemerisSource::DEMODULATED:
        out.satellitePvt.ephemerisSource = GNSS_EPHEMERIS_SOURCE_EXT_DEMODULATED;
        break;
    case SatellitePvt::SatelliteEphemerisSource::SERVER_NORMAL:
        out.satellitePvt.ephemerisSource = GNSS_EPHEMERIS_SOURCE_EXT_SERVER_NORMAL;
        break;
    case SatellitePvt::SatelliteEphemerisSource::SERVER_LONG_TERM:
        out.satellitePvt.ephemerisSource = GNSS_EPHEMERIS_SOURCE_EXT_SERVER_LONG_TERM;
        break;
    case SatellitePvt::SatelliteEphemerisSource::OTHER:
        out.satellitePvt.ephemerisSource = GNSS_EPHEMERIS_SOURCE_EXT_OTHER;
        break;
    default:
        out.satellitePvt.ephemerisSource = GNSS_EPHEMERIS_SOURCE_EXT_INVALID;
        break;
    }
}


static void translateLocation(Location& location, const LocAidlLocation& aidlLocation) {
    location.size = sizeof(Location);
    location.flags = aidlLocation.locationFlagsMask;
    location.timestamp = aidlLocation.timestamp;
    location.latitude = aidlLocation.latitude;
    location.longitude = aidlLocation.longitude;
    location.altitude = aidlLocation.altitude;
    location.speed = aidlLocation.speed;
    location.bearing = aidlLocation.bearing;
    location.accuracy = aidlLocation.accuracy;
    location.verticalAccuracy = aidlLocation.verticalAccuracy;
    location.speedAccuracy = aidlLocation.speedAccuracy;
    location.bearingAccuracy = aidlLocation.bearingAccuracy;
    location.techMask = aidlLocation.locationTechnologyMask;
}

static void translateHidlUlpLocation(Location& location, const LocAidlUlpLocation& ulpLocation) {
    translateLocation(location, ulpLocation.gpsLocation);
}

static void translateHidlUlpLocationToUlpLocation(UlpLocation& location,
        const LocAidlUlpLocation& ulpLocation) {
    location.position_source = ulpLocation.position_source;
    location.tech_mask = ulpLocation.tech_mask;
    location.gpsLocation.flags = ulpLocation.gpsLocation.locationFlagsMask;
    location.gpsLocation.latitude = ulpLocation.gpsLocation.latitude;
    location.gpsLocation.longitude = ulpLocation.gpsLocation.longitude;
    location.gpsLocation.altitude = ulpLocation.gpsLocation.altitude;
    location.gpsLocation.speed = ulpLocation.gpsLocation.speed;
    location.gpsLocation.bearing = ulpLocation.gpsLocation.bearing;
    location.gpsLocation.accuracy = ulpLocation.gpsLocation.accuracy;
    location.gpsLocation.vertUncertainity = ulpLocation.gpsLocation.verticalAccuracy;
    location.gpsLocation.timestamp = ulpLocation.gpsLocation.timestamp;
}

static void translateIzatLocation(Location& location, const LocAidlIzatLocation& izatLocation) {
    location.size = sizeof(Location);
    location.timestamp = izatLocation.utcTimestampInMsec;
    if (izatLocation.hasLatitude && izatLocation.hasLongitude) {
        location.latitude = izatLocation.latitude;
        location.longitude = izatLocation.longitude;
        location.flags |= LOCATION_HAS_LAT_LONG_BIT;
    }
    if (izatLocation.hasAltitudeWrtEllipsoid) {
        location.altitude = izatLocation.altitudeWrtEllipsoid;
        location.flags |= LOCATION_HAS_ALTITUDE_BIT;
    }
    if (izatLocation.hasSpeed) {
        location.speed = izatLocation.speed;
        location.flags |= LOCATION_HAS_SPEED_BIT;
    }
    if (izatLocation.hasBearing) {
        location.bearing = izatLocation.bearing;
        location.flags |= LOCATION_HAS_BEARING_BIT;
    }
    if (izatLocation.hasHorizontalAccuracy) {
        location.accuracy = izatLocation.horizontalAccuracy;
        location.flags |= LOCATION_HAS_ACCURACY_BIT;
    }
    if (izatLocation.hasVertUnc) {
        location.verticalAccuracy = izatLocation.vertUnc;
        location.flags |= LOCATION_HAS_VERTICAL_ACCURACY_BIT;
    }
    if (izatLocation.hasSpeedUnc) {
        location.speedAccuracy = izatLocation.speedUnc;
        location.flags |= LOCATION_HAS_SPEED_ACCURACY_BIT;
    }
    if (izatLocation.hasBearingUnc) {
        location.bearingAccuracy = izatLocation.bearingUnc;
        location.flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
    }
    if (izatLocation.hasNetworkPositionSource) {
        if (izatLocation.networkPositionSource == LocAidlNetworkPositionSourceType::WIFI) {
            location.techMask |= LOCATION_TECHNOLOGY_WIFI_BIT;
        } else if (izatLocation.networkPositionSource == LocAidlNetworkPositionSourceType::CELL) {
            location.techMask |= LOCATION_TECHNOLOGY_CELL_BIT;
        }
    }
}

static LocAidlGnssConstellationType intToGnssConstellation(uint8_t svTypeInt) {
    switch (svTypeInt) {
        case 1:
            return LocAidlGnssConstellationType::GPS;
        case 2:
            return LocAidlGnssConstellationType::SBAS;
        case 3:
            return LocAidlGnssConstellationType::GLONASS;
        case 4:
            return LocAidlGnssConstellationType::QZSS;
        case 5:
            return LocAidlGnssConstellationType::BEIDOU;
        case 6:
            return LocAidlGnssConstellationType::GALILEO;
        case 7:
            return LocAidlGnssConstellationType::IRNSS;
        default:
            return LocAidlGnssConstellationType::UNKNOWN;
    }
}

//Debug report service related translation
static void populateTimeAndClockReport(
        LocAidlSystemStatusReports& aidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mTimeAndClockVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusTimeAndClock status;

        status.mUtcTime.tv_sec = aidlReports.mTimeAndClockVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mTimeAndClockVec[i].mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = aidlReports.mTimeAndClockVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = aidlReports.mTimeAndClockVec[i].mUtcReported.tv_nsec;

        status.mGpsWeek = aidlReports.mTimeAndClockVec[i].mGpsWeek;
        status.mGpsTowMs = aidlReports.mTimeAndClockVec[i].mGpsTowMs;
        status.mTimeValid = aidlReports.mTimeAndClockVec[i].mTimeValid;
        status.mTimeSource = aidlReports.mTimeAndClockVec[i].mTimeSource;
        status.mTimeUnc = aidlReports.mTimeAndClockVec[i].mTimeUnc;
        status.mClockFreqBias =  aidlReports.mTimeAndClockVec[i].mClockFreqBias;
        status.mClockFreqBiasUnc = aidlReports.mTimeAndClockVec[i].mClockFreqBiasUnc;
        systemReports.mTimeAndClock.push_back(status);
    }
}

static void populateXoStateReport(
        LocAidlSystemStatusReports& aidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mXoStateVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusXoState status;

        status.mUtcTime.tv_sec = aidlReports.mXoStateVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mXoStateVec[i].mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = aidlReports.mXoStateVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = aidlReports.mXoStateVec[i].mUtcReported.tv_nsec;

        status.mXoState = aidlReports.mXoStateVec[i].mXoState;
        systemReports.mXoState.push_back(status);
    }
}

static void populateRfParamsReport(
        LocAidlSystemStatusReports& aidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mRfAndParamsVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusRfAndParams status;

        status.mUtcTime.tv_sec = aidlReports.mRfAndParamsVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mRfAndParamsVec[i].mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec =
                aidlReports.mRfAndParamsVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec =
                aidlReports.mRfAndParamsVec[i].mUtcReported.tv_nsec;

        status.mPgaGain = aidlReports.mRfAndParamsVec[i].mPgaGain;
        status.mAdcI = aidlReports.mRfAndParamsVec[i].mAdcI;
        status.mAdcQ = aidlReports.mRfAndParamsVec[i].mAdcQ;
        status.mJammerGps = aidlReports.mRfAndParamsVec[i].mJammerGps;
        status.mJammerGlo = aidlReports.mRfAndParamsVec[i].mJammerGlo;
        status.mJammerBds = aidlReports.mRfAndParamsVec[i].mJammerBds;
        status.mJammerGal = aidlReports.mRfAndParamsVec[i].mJammerGal;
        status.mGpsBpAmpI = aidlReports.mRfAndParamsVec[i].mGpsBpAmpI;
        status.mGpsBpAmpQ = aidlReports.mRfAndParamsVec[i].mGpsBpAmpQ;
        status.mGloBpAmpI = aidlReports.mRfAndParamsVec[i].mGloBpAmpI;
        status.mGloBpAmpQ = aidlReports.mRfAndParamsVec[i].mGloBpAmpQ;
        status.mBdsBpAmpI = aidlReports.mRfAndParamsVec[i].mBdsBpAmpI;
        status.mBdsBpAmpQ = aidlReports.mRfAndParamsVec[i].mBdsBpAmpQ;
        status.mGalBpAmpI = aidlReports.mRfAndParamsVec[i].mGalBpAmpI;
        status.mGalBpAmpQ = aidlReports.mRfAndParamsVec[i].mGalBpAmpQ;
        status.mJammedSignalsMask = aidlReports.mRfAndParamsVec[i].mJammedSignalsMask;
        status.mJammerInd.assign(aidlReports.mRfAndParamsVec[i].mJammerInd->begin(),
                aidlReports.mRfAndParamsVec[i].mJammerInd->end());
        LOC_LOGv("mJammerData size: %d", status.mJammerInd.size());
        systemReports.mRfAndParams.push_back(status);

    }
}

static void populateErrRecoveryReport(
        LocAidlSystemStatusReports& aidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mErrRecoveryVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusErrRecovery status;

        status.mUtcTime.tv_sec = aidlReports.mErrRecoveryVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mErrRecoveryVec[i].mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = aidlReports.mErrRecoveryVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = aidlReports.mErrRecoveryVec[i].mUtcReported.tv_nsec;
        systemReports.mErrRecovery.push_back(status);
    }
}

static void populateInjectedPositionReport(
        LocAidlSystemStatusReports& aidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mInjectedPositionVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusInjectedPosition status;

        status.mUtcTime.tv_sec = aidlReports.mInjectedPositionVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mInjectedPositionVec[i].mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec =
                aidlReports.mInjectedPositionVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec =
                aidlReports.mInjectedPositionVec[i].mUtcReported.tv_nsec;

        status.mEpiValidity = aidlReports.mInjectedPositionVec[i].mEpiValidity;
        status.mEpiLat = aidlReports.mInjectedPositionVec[i].mEpiLat;
        status.mEpiLon = aidlReports.mInjectedPositionVec[i].mEpiLon;
        status.mEpiHepe = aidlReports.mInjectedPositionVec[i].mEpiHepe;
        status.mEpiAlt = aidlReports.mInjectedPositionVec[i].mEpiAlt;
        status.mEpiAltUnc = aidlReports.mInjectedPositionVec[i].mEpiAltUnc;
        status.mEpiSrc = aidlReports.mInjectedPositionVec[i].mEpiSrc;
        systemReports.mInjectedPosition.push_back(status);
    }
}

static void populateBestPositionReport(
        LocAidlSystemStatusReports& aidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mBestPositionVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusBestPosition status;

        status.mUtcTime.tv_sec = aidlReports.mBestPositionVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mBestPositionVec[i].mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = aidlReports.mBestPositionVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = aidlReports.mBestPositionVec[i].mUtcReported.tv_nsec;

        status.mBestLat = aidlReports.mBestPositionVec[i].mBestLat;
        status.mBestLon = aidlReports.mBestPositionVec[i].mBestLon;
        status.mBestHepe = aidlReports.mBestPositionVec[i].mBestHepe;
        status.mBestAlt = aidlReports.mBestPositionVec[i].mBestAlt;
        status.mBestAltUnc = aidlReports.mBestPositionVec[i].mBestAltUnc;
        systemReports.mBestPosition.push_back(status);
    }
}

static void populateXtraReport(
        LocAidlSystemStatusReports& aidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mXtraVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusXtra status;

        status.mUtcTime.tv_sec = aidlReports.mXtraVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mXtraVec[i].mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = aidlReports.mXtraVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = aidlReports.mXtraVec[i].mUtcReported.tv_nsec;

        status.mXtraValidMask = aidlReports.mXtraVec[i].mXtraValidMask;
        status.mGpsXtraAge = aidlReports.mXtraVec[i].mGpsXtraAge;
        status.mGloXtraAge = aidlReports.mXtraVec[i].mGloXtraAge;
        status.mBdsXtraAge = aidlReports.mXtraVec[i].mBdsXtraAge;
        status.mGalXtraAge = aidlReports.mXtraVec[i].mGalXtraAge;
        status.mQzssXtraAge = aidlReports.mXtraVec[i].mQzssXtraAge;
        status.mGpsXtraValid = aidlReports.mXtraVec[i].mGpsXtraValid;
        status.mGloXtraValid = aidlReports.mXtraVec[i].mGloXtraValid;
        status.mBdsXtraValid = aidlReports.mXtraVec[i].mBdsXtraValid;
        status.mGalXtraValid = aidlReports.mXtraVec[i].mGalXtraValid;
        status.mQzssXtraValid = aidlReports.mXtraVec[i].mQzssXtraValid;
        systemReports.mXtra.push_back(status);
    }
}

static void populateEphemerisReport(
        LocAidlSystemStatusReports& aidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mEphemerisVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusEphemeris status;

        status.mUtcTime.tv_sec = aidlReports.mEphemerisVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mEphemerisVec[i].mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = aidlReports.mEphemerisVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = aidlReports.mEphemerisVec[i].mUtcReported.tv_nsec;

        status.mGpsEpheValid = aidlReports.mEphemerisVec[i].mGpsEpheValid;
        status.mGloEpheValid = aidlReports.mEphemerisVec[i].mGloEpheValid;
        status.mBdsEpheValid = aidlReports.mEphemerisVec[i].mBdsEpheValid;
        status.mGalEpheValid = aidlReports.mEphemerisVec[i].mGalEpheValid;
        status.mQzssEpheValid = aidlReports.mEphemerisVec[i].mQzssEpheValid;
        systemReports.mEphemeris.push_back(status);
    }
}

static void populateSvHealthReport(
        LocAidlSystemStatusReports& aidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mSvHealthVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusSvHealth status;

        status.mUtcTime.tv_sec = aidlReports.mSvHealthVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mSvHealthVec[i].mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = aidlReports.mSvHealthVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = aidlReports.mSvHealthVec[i].mUtcReported.tv_nsec;

        status.mGpsGoodMask = aidlReports.mSvHealthVec[i].mGpsGoodMask;
        status.mGloGoodMask = aidlReports.mSvHealthVec[i].mGloGoodMask;
        status.mBdsGoodMask = aidlReports.mSvHealthVec[i].mBdsGoodMask;
        status.mGalGoodMask = aidlReports.mSvHealthVec[i].mGalGoodMask;
        status.mQzssGoodMask = aidlReports.mSvHealthVec[i].mQzssGoodMask;

        status.mGpsBadMask = aidlReports.mSvHealthVec[i].mGpsBadMask;
        status.mGloBadMask = aidlReports.mSvHealthVec[i].mGloBadMask;
        status.mBdsBadMask = aidlReports.mSvHealthVec[i].mBdsBadMask;
        status.mGalBadMask = aidlReports.mSvHealthVec[i].mGalBadMask;
        status.mQzssBadMask = aidlReports.mSvHealthVec[i].mQzssBadMask;

        status.mGpsUnknownMask = aidlReports.mSvHealthVec[i].mGpsUnknownMask;
        status.mGloUnknownMask = aidlReports.mSvHealthVec[i].mGloUnknownMask;
        status.mBdsUnknownMask = aidlReports.mSvHealthVec[i].mBdsUnknownMask;
        status.mGalUnknownMask = aidlReports.mSvHealthVec[i].mGalUnknownMask;
        status.mQzssUnknownMask = aidlReports.mSvHealthVec[i].mQzssUnknownMask;
        systemReports.mSvHealth.push_back(status);
    }
}

static void populatePdrReport(
        LocAidlSystemStatusReports& aidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mPdrVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusPdr status;

        status.mUtcTime.tv_sec = aidlReports.mPdrVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mPdrVec[i].mUtcTime.tv_nsec;
        status.mUtcReported.tv_sec = aidlReports.mPdrVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = aidlReports.mPdrVec[i].mUtcReported.tv_nsec;
        status.mFixInfoMask = aidlReports.mPdrVec[i].mFixInfoMask;
        systemReports.mPdr.push_back(status);
    }
}

static void populateNavDataReport(
        LocAidlSystemStatusReports& /*aidlReports*/,
        SystemStatusReports& /*systemReports*/, int32_t /*maxReports*/) {

    /* Not yet supported */
}

static void populatePositionFailureReport(
        LocAidlSystemStatusReports& aidlReports,
        loc_core::SystemStatusReports& systemReports, int32_t maxReports) {

    int count = aidlReports.mPositionFailureVec.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }

    for (int i=0; i < count && i < maxReports; i++) {

        SystemStatusPositionFailure status;

        status.mUtcTime.tv_sec = aidlReports.mPositionFailureVec[i].mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = aidlReports.mPositionFailureVec[i].mUtcTime.tv_nsec;
        status.mUtcReported.tv_sec =
                aidlReports.mPositionFailureVec[i].mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec =
                aidlReports.mPositionFailureVec[i].mUtcReported.tv_nsec;
        status.mFixInfoMask = aidlReports.mPositionFailureVec[i].mFixInfoMask;
        status.mHepeLimit = aidlReports.mPositionFailureVec[i].mHepeLimit;
        systemReports.mPositionFailure.push_back(status);
    }
}

static void translateStatus(LocationError& error, int& status) {
    switch (status) {
        case IGnssGeofenceCallback::OPERATION_SUCCESS:
            error = LOCATION_ERROR_SUCCESS;
            break;
        case IGnssGeofenceCallback::ERROR_TOO_MANY_GEOFENCES:
            error = LOCATION_ERROR_GEOFENCES_AT_MAX;
            break;
        case IGnssGeofenceCallback::ERROR_ID_EXISTS:
            error = LOCATION_ERROR_ID_EXISTS;
            break;
        case IGnssGeofenceCallback::ERROR_ID_UNKNOWN:
            error = LOCATION_ERROR_ID_UNKNOWN;
            break;
        case IGnssGeofenceCallback::ERROR_INVALID_TRANSITION:
            error = LOCATION_ERROR_INVALID_PARAMETER;
            break;
        default:
            error = LOCATION_ERROR_GENERAL_FAILURE;
    }
}
static void translateGnssConstellationType(GnssSvType& out,
        const GnssConstellationType& in) {
    switch (in) {
        case GnssConstellationType::GPS:
            out = GNSS_SV_TYPE_GPS;
                break;
        case GnssConstellationType::SBAS:
            out = GNSS_SV_TYPE_SBAS;
                break;
        case GnssConstellationType::GLONASS:
            out = GNSS_SV_TYPE_GLONASS;
                break;
        case GnssConstellationType::QZSS:
            out = GNSS_SV_TYPE_QZSS;
                break;
        case GnssConstellationType::BEIDOU:
            out = GNSS_SV_TYPE_BEIDOU;
                break;
        case GnssConstellationType::GALILEO:
            out = GNSS_SV_TYPE_GALILEO;
                break;
        case GnssConstellationType::IRNSS:
            out = GNSS_SV_TYPE_NAVIC;
                break;
        default:
            out = GNSS_SV_TYPE_UNKNOWN;
                break;
    }
}

static void translateGnssMeasurementsCodeType(GnssMeasurementsCodeType& out,
        const std::string& in) {
    LOC_LOGd("(): in: %s in.size(): %u", in.c_str(), in.size());
    if (in.size() != 1) { //GNSS_MEASUREMENTS_CODE_TYPE_UNKNOWN
        out = GNSS_MEASUREMENTS_CODE_TYPE_OTHER;
        return;
    }
    std::string str(in.c_str());
        LOC_LOGd("(): %c, in.size(): %u", str[0], in.size());
    switch (str[0]) {
        // TODO  'D'
        case 'A':
            out = GNSS_MEASUREMENTS_CODE_TYPE_A;
            break;
        case 'B':
            out = GNSS_MEASUREMENTS_CODE_TYPE_B;
            break;
        case 'C':
            out = GNSS_MEASUREMENTS_CODE_TYPE_C;
            break;
        case 'I':
            out = GNSS_MEASUREMENTS_CODE_TYPE_I;
            break;
        case 'L':
            out = GNSS_MEASUREMENTS_CODE_TYPE_L;
            break;
        case 'M':
            out = GNSS_MEASUREMENTS_CODE_TYPE_M;
            break;
        case 'P':
            out = GNSS_MEASUREMENTS_CODE_TYPE_P;
            break;
        case 'Q':
            out = GNSS_MEASUREMENTS_CODE_TYPE_Q;
            break;
        case 'S':
            out = GNSS_MEASUREMENTS_CODE_TYPE_S;
            break;
        case 'W':
            out = GNSS_MEASUREMENTS_CODE_TYPE_W;
            break;
        case 'X':
            out = GNSS_MEASUREMENTS_CODE_TYPE_X;
            break;
        case 'Y':
            out = GNSS_MEASUREMENTS_CODE_TYPE_Y;
            break;
        case 'Z':
            out = GNSS_MEASUREMENTS_CODE_TYPE_Z;
            break;
        case 'N':
            out = GNSS_MEASUREMENTS_CODE_TYPE_N;
            break;
        default:
            out = GNSS_MEASUREMENTS_CODE_TYPE_OTHER;
    }
}

static void translateGnssMeasurementsState(GnssMeasurementsStateMask& out, int state) {
    memset(&out, 0, sizeof(out));
    if (state & GnssMeasurement::STATE_CODE_LOCK)
        out |= GNSS_MEASUREMENTS_STATE_CODE_LOCK_BIT;
    if (state & GnssMeasurement::STATE_BIT_SYNC)
        out |= GNSS_MEASUREMENTS_STATE_BIT_SYNC_BIT;
    if (state & GnssMeasurement::STATE_SUBFRAME_SYNC)
        out |= GNSS_MEASUREMENTS_STATE_SUBFRAME_SYNC_BIT;
    if (state & GnssMeasurement::STATE_TOW_DECODED)
        out |= GNSS_MEASUREMENTS_STATE_TOW_DECODED_BIT;
    if (state & GnssMeasurement::STATE_MSEC_AMBIGUOUS)
        out |= GNSS_MEASUREMENTS_STATE_MSEC_AMBIGUOUS_BIT;
    if (state & GnssMeasurement::STATE_SYMBOL_SYNC)
        out |= GNSS_MEASUREMENTS_STATE_SYMBOL_SYNC_BIT;
    if (state & GnssMeasurement::STATE_GLO_STRING_SYNC)
        out |= GNSS_MEASUREMENTS_STATE_GLO_STRING_SYNC_BIT;
    if (state & GnssMeasurement::STATE_GLO_TOD_DECODED)
        out |= GNSS_MEASUREMENTS_STATE_GLO_TOD_DECODED_BIT;
    if (state & GnssMeasurement::STATE_BDS_D2_BIT_SYNC)
        out |= GNSS_MEASUREMENTS_STATE_BDS_D2_BIT_SYNC_BIT;
    if (state & GnssMeasurement::STATE_BDS_D2_SUBFRAME_SYNC)
        out |= GNSS_MEASUREMENTS_STATE_BDS_D2_SUBFRAME_SYNC_BIT;
    if (state & GnssMeasurement::STATE_GAL_E1BC_CODE_LOCK)
        out |= GNSS_MEASUREMENTS_STATE_GAL_E1BC_CODE_LOCK_BIT;
    if (state & GnssMeasurement::STATE_GAL_E1C_2ND_CODE_LOCK)
        out |= GNSS_MEASUREMENTS_STATE_GAL_E1C_2ND_CODE_LOCK_BIT;
    if (state & GnssMeasurement::STATE_GAL_E1B_PAGE_SYNC)
        out |= GNSS_MEASUREMENTS_STATE_GAL_E1B_PAGE_SYNC_BIT;
    if (state & GnssMeasurement::STATE_SBAS_SYNC)
        out |= GNSS_MEASUREMENTS_STATE_SBAS_SYNC_BIT;
    if (state & GnssMeasurement::STATE_TOW_KNOWN)
        out |= GNSS_MEASUREMENTS_STATE_TOW_KNOWN_BIT;
    if (state & GnssMeasurement::STATE_GLO_TOD_KNOWN)
        out |= GNSS_MEASUREMENTS_STATE_GLO_TOD_KNOWN_BIT;
    if (state & GnssMeasurement::STATE_2ND_CODE_LOCK)
        out |= GNSS_MEASUREMENTS_STATE_2ND_CODE_LOCK_BIT;
}

static void translateGnssClock(GnssMeasurementsClock& out,
        const ::aidl::android::hardware::gnss::GnssClock& in) {
    memset(&out, 0, sizeof(out));
    if (in.gnssClockFlags & GnssClock::HAS_LEAP_SECOND) {
        out.flags |= GNSS_MEASUREMENTS_CLOCK_FLAGS_LEAP_SECOND_BIT;
    }
    if (in.gnssClockFlags & GnssClock::HAS_TIME_UNCERTAINTY) {
        out.flags |= GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_UNCERTAINTY_BIT;
    }
    if (in.gnssClockFlags & GnssClock::HAS_FULL_BIAS) {
        out.flags |= GNSS_MEASUREMENTS_CLOCK_FLAGS_FULL_BIAS_BIT;
    }
    if (in.gnssClockFlags & GnssClock::HAS_BIAS) {
        out.flags |= GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_BIT;
    }
    if (in.gnssClockFlags & GnssClock::HAS_BIAS_UNCERTAINTY) {
        out.flags |= GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_UNCERTAINTY_BIT;
    }
    if (in.gnssClockFlags & GnssClock::HAS_DRIFT) {
        out.flags |= GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_BIT;
    }
    if (in.gnssClockFlags & GnssClock::HAS_DRIFT_UNCERTAINTY) {
        out.flags |= GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_UNCERTAINTY_BIT;
    }
    out.leapSecond = in.leapSecond;
    out.timeNs = in.timeNs;
    out.timeUncertaintyNs = in.timeUncertaintyNs;
    out.fullBiasNs = in.fullBiasNs;
    out.biasNs = in.biasNs;
    out.biasUncertaintyNs = in.biasUncertaintyNs;
    out.driftNsps = in.driftNsps;
    out.driftUncertaintyNsps = in.driftUncertaintyNsps;
    out.hwClockDiscontinuityCount = in.hwClockDiscontinuityCount;

    translateGnssConstellationType(out.referenceSignalTypeForIsb.svType,
            in.referenceSignalTypeForIsb.constellation);
    out.referenceSignalTypeForIsb.carrierFrequencyHz =
            in.referenceSignalTypeForIsb.carrierFrequencyHz;
    translateGnssMeasurementsCodeType(out.referenceSignalTypeForIsb.codeType,
            in.referenceSignalTypeForIsb.codeType);
}

static void translateGnssMeasurement(GnssMeasurementsData& out,
        const GnssMeasurement& in) {
    memset(&out, 0, sizeof(out));
    out.size = sizeof(out);
    if (in.flags & GnssMeasurement::HAS_SNR) {
        out.flags |= GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_CARRIER_FREQUENCY) {
        out.flags |= GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_CARRIER_CYCLES) {
        out.flags |= GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_CARRIER_PHASE) {
        out.flags |= GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_CARRIER_PHASE_UNCERTAINTY) {
        out.flags |= GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_AUTOMATIC_GAIN_CONTROL) {
        out.flags |= GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_FULL_ISB) {
        out.fullInterSignalBiasNs = in.fullInterSignalBiasNs;
        out.flags |= GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_FULL_ISB_UNCERTAINTY) {
        out.fullInterSignalBiasUncertaintyNs = in.fullInterSignalBiasUncertaintyNs;
        out.flags |= GNSS_MEASUREMENTS_DATA_FULL_ISB_UNCERTAINTY_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_SATELLITE_ISB) {
        out.satelliteInterSignalBiasNs = in.satelliteInterSignalBiasNs;
        out.flags |= GNSS_MEASUREMENTS_DATA_SATELLITE_ISB_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_SATELLITE_ISB_UNCERTAINTY) {
        out.satelliteInterSignalBiasUncertaintyNs = in.satelliteInterSignalBiasUncertaintyNs;
        out.flags |= GNSS_MEASUREMENTS_DATA_SATELLITE_ISB_UNCERTAINTY_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_SATELLITE_PVT) {
        translateGnssSatellitePvt(in, out);
        out.flags |= GNSS_MEASUREMENTS_DATA_SATELLITE_PVT_BIT;
    }
    if (in.flags & GnssMeasurement::HAS_CORRELATION_VECTOR) {
        out.flags |= GNSS_MEASUREMENTS_DATA_CORRELATION_VECTOR_BIT;
    }
    out.svId = in.svid;
    translateGnssConstellationType(out.svType, in.signalType.constellation);
    translateGnssMeasurementsCodeType(out.codeType, in.signalType.codeType);
    translateGnssMeasurementsState(out.stateMask, in.state);
    out.timeOffsetNs = in.timeOffsetNs;
    if (in.accumulatedDeltaRangeState & GnssMeasurement::ADR_STATE_VALID) {
        out.adrStateMask |= GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_VALID_BIT;
    }
    if (in.accumulatedDeltaRangeState & GnssMeasurement::ADR_STATE_RESET) {
        out.adrStateMask |= GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_RESET_BIT;
    }
    if (in.accumulatedDeltaRangeState & GnssMeasurement::ADR_STATE_CYCLE_SLIP) {
        out.adrStateMask |= GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_CYCLE_SLIP_BIT;
    }
    if (in.accumulatedDeltaRangeState & GnssMeasurement::ADR_STATE_HALF_CYCLE_RESOLVED) {
        out.adrStateMask |= GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_HALF_CYCLE_RESOLVED_BIT;
    }
    out.basebandCarrierToNoiseDbHz = in.basebandCN0DbHz;
    out.receivedSvTimeNs = in.receivedSvTimeInNs;
    out.receivedSvTimeUncertaintyNs = in.receivedSvTimeUncertaintyInNs;
    out.carrierToNoiseDbHz = in.antennaCN0DbHz;
    out.pseudorangeRateMps = in.pseudorangeRateMps;
    out.pseudorangeRateUncertaintyMps = in.pseudorangeRateUncertaintyMps;
    out.adrMeters = in.accumulatedDeltaRangeM;
    out.adrUncertaintyMeters = in.accumulatedDeltaRangeUncertaintyM;
    out.carrierFrequencyHz = in.signalType.carrierFrequencyHz;
    out.carrierCycles = in.carrierCycles;
    out.carrierPhase = in.carrierPhase;
    out.carrierPhaseUncertainty = in.carrierPhaseUncertainty;
    out.multipathIndicator = static_cast<GnssMeasurementsMultipathIndicator>(in.multipathIndicator);
    out.signalToNoiseRatioDb = in.snrDb;
    out.agcLevelDb = in.agcLevelDb;
}

static void translateGnssAgc(GnssMeasurementsAgc& out, const GnssData& in, size_t i) {
    memset(&out, 0, sizeof(out));

    translateGnssConstellationType(out.svType, in.gnssAgcs[i].constellation);
    out.carrierFrequencyHz = in.gnssAgcs[i].carrierFrequencyHz;
    out.agcLevelDb = in.gnssAgcs[i].agcLevelDb;
}

static void translateGnssData(GnssMeasurementsNotification& out, const GnssData& in) {
    out.size = sizeof(out);
    out.count = in.measurements.size();
    LOC_LOGd("(): %u, in.measurements.size(): %u", out.count, in.measurements.size());
    translateGnssClock(out.clock, in.clock);
    translateElapsedRealtimeNanos(in.elapsedRealtime, out);
    for (size_t i = 0; i < out.count; i++) {
        translateGnssMeasurement(out.measurements[i], in.measurements[i]);
    }
    out.agcCount = in.gnssAgcs.size();
    for (size_t i = 0; i < out.agcCount; i++) {
        translateGnssAgc(out.gnssAgc[i], in, i);
    }
}

static void translateGnssEphemerisType(GnssEphemerisType& out,
        const IGnssDebug::SatelliteEphemerisType& in) {
    switch (in) {
        case IGnssDebug::SatelliteEphemerisType::EPHEMERIS:
            out = GNSS_EPH_TYPE_EPHEMERIS;
            break;
        case IGnssDebug::SatelliteEphemerisType::ALMANAC_ONLY:
            out = GNSS_EPH_TYPE_ALMANAC;
            break;
        default:
            out = GNSS_EPH_TYPE_UNKNOWN;
            break;
    }
}

static void translateGnssEphemerisSource(GnssEphemerisSource& out,
        const SatellitePvt::SatelliteEphemerisSource& in) {
    switch (in) {
        case SatellitePvt::SatelliteEphemerisSource::DEMODULATED:
            out = GNSS_EPH_SOURCE_DEMODULATED;
            break;
        case SatellitePvt::SatelliteEphemerisSource::SERVER_NORMAL:
        case SatellitePvt::SatelliteEphemerisSource::SERVER_LONG_TERM:
            out = GNSS_EPH_SOURCE_SUPL_PROVIDED;
            break;
        case SatellitePvt::SatelliteEphemerisSource::OTHER:
        default:
            out = GNSS_EPH_SOURCE_UNKNOWN;
            break;
    }
}

static void translateGnssEphemerisHealth(GnssEphemerisHealth& out,
        const IGnssDebug::SatelliteEphemerisHealth& in) {
    switch (in) {
        case IGnssDebug::SatelliteEphemerisHealth::GOOD:
            out = GNSS_EPH_HEALTH_GOOD;
            break;
        case IGnssDebug::SatelliteEphemerisHealth::BAD:
            out = GNSS_EPH_HEALTH_BAD;
            break;
        default:
            out = GNSS_EPH_HEALTH_UNKNOWN;
            break;
    }
}

static void translateGnssDebugReport(GnssDebugReport& reports,
        const IGnssDebug::DebugData& data) {
    if (data.position.valid) {
        reports.mLocation.mValid = true;
        reports.mLocation.mLocation.latitude = data.position.latitudeDegrees;
        reports.mLocation.mLocation.longitude = data.position.longitudeDegrees;
        reports.mLocation.mLocation.altitude = data.position.altitudeMeters;

        reports.mLocation.mLocation.speed = data.position.speedMetersPerSec;
        reports.mLocation.mLocation.bearing = data.position.bearingDegrees;
        reports.mLocation.mLocation.accuracy = data.position.horizontalAccuracyMeters;
        reports.mLocation.verticalAccuracyMeters = data.position.verticalAccuracyMeters;
        reports.mLocation.speedAccuracyMetersPerSecond = data.position.speedAccuracyMetersPerSecond;
        reports.mLocation.bearingAccuracyDegrees = data.position.bearingAccuracyDegrees;

        reports.mTime.size = sizeof(reports.mTime);
        reports.mTime.mValid = true;
        reports.mTime.timeEstimate = data.time.timeEstimateMs;
        reports.mTime.timeUncertaintyNs = data.time.timeUncertaintyNs;
        reports.mTime.frequencyUncertaintyNsPerSec = data.time.frequencyUncertaintyNsPerSec;

    }
    else {
        reports.mLocation.mValid = false;
        reports.mTime.mValid = false;
    }

    if (reports.mLocation.verticalAccuracyMeters <= 0 ||
        reports.mLocation.verticalAccuracyMeters > GNSS_DEBUG_UNKNOWN_VERTICAL_ACCURACY_METERS) {
        reports.mLocation.verticalAccuracyMeters = GNSS_DEBUG_UNKNOWN_VERTICAL_ACCURACY_METERS;
    }
    if (reports.mLocation.speedAccuracyMetersPerSecond <= 0 ||
        reports.mLocation.speedAccuracyMetersPerSecond > GNSS_DEBUG_UNKNOWN_SPEED_ACCURACY_PER_SEC){
        reports.mLocation.speedAccuracyMetersPerSecond = GNSS_DEBUG_UNKNOWN_SPEED_ACCURACY_PER_SEC;
    }
    if (reports.mLocation.bearingAccuracyDegrees <= 0 ||
        reports.mLocation.bearingAccuracyDegrees > GNSS_DEBUG_UNKNOWN_BEARING_ACCURACY_DEG) {
        reports.mLocation.bearingAccuracyDegrees = GNSS_DEBUG_UNKNOWN_BEARING_ACCURACY_DEG;
    }


    if (reports.mTime.timeEstimate < GNSS_DEBUG_UNKNOWN_UTC_TIME) {
        reports.mTime.timeEstimate = GNSS_DEBUG_UNKNOWN_UTC_TIME;
    }
    if (reports.mTime.timeUncertaintyNs <= 0) {
        reports.mTime.timeUncertaintyNs = (float)GNSS_DEBUG_UNKNOWN_UTC_TIME_UNC_MIN;
    }
    else if (reports.mTime.timeUncertaintyNs > GNSS_DEBUG_UNKNOWN_UTC_TIME_UNC_MAX) {
        reports.mTime.timeUncertaintyNs = (float)GNSS_DEBUG_UNKNOWN_UTC_TIME_UNC_MAX;
    }
    if (reports.mTime.frequencyUncertaintyNsPerSec <= 0 ||
        reports.mTime.frequencyUncertaintyNsPerSec > (float)GNSS_DEBUG_UNKNOWN_FREQ_UNC_NS_PER_SEC){
        reports.mTime.frequencyUncertaintyNsPerSec = (float)GNSS_DEBUG_UNKNOWN_FREQ_UNC_NS_PER_SEC;
    }

    // satellite data block
    GnssDebugSatelliteInfo s = { };
    std::vector<IGnssDebug::SatelliteData> s_array;

    for (uint32_t i=0; i<data.satelliteDataArray.size(); i++) {
        s.svid = data.satelliteDataArray[i].svid;
        translateGnssConstellationType(s.constellation,
            data.satelliteDataArray[i].constellation);
        translateGnssEphemerisType(
            s.mEphemerisType, data.satelliteDataArray[i].ephemerisType);
        translateGnssEphemerisSource(
            s.mEphemerisSource, data.satelliteDataArray[i].ephemerisSource);
        translateGnssEphemerisHealth(
            s.mEphemerisHealth, data.satelliteDataArray[i].ephemerisHealth);

        s.ephemerisAgeSeconds =
            data.satelliteDataArray[i].ephemerisAgeSeconds;
        s.serverPredictionIsAvailable =
            data.satelliteDataArray[i].serverPredictionIsAvailable;
        s.serverPredictionAgeSeconds =
            data.satelliteDataArray[i].serverPredictionAgeSeconds;

        reports.mSatelliteInfo.push_back(s);
    }
}

static void translateGnssNfwNotification(GnssNfwNotification& out,
    const IGnssVisibilityControlCallback::NfwNotification& in) {
    memset(&out, 0, sizeof(IGnssVisibilityControlCallback::NfwNotification));
    strlcpy(out.proxyAppPackageName, in.proxyAppPackageName.c_str(),
            in.proxyAppPackageName.size()<GNSS_MAX_NFW_APP_STRING_LEN?
            in.proxyAppPackageName.size():GNSS_MAX_NFW_APP_STRING_LEN);
    out.protocolStack = (GnssNfwProtocolStack)in.protocolStack;
    strlcpy(out.otherProtocolStackName, in.otherProtocolStackName.c_str(),
            in.otherProtocolStackName.size()<GNSS_MAX_NFW_STRING_LEN?
            in.otherProtocolStackName.size():GNSS_MAX_NFW_STRING_LEN);

    out.requestor = (GnssNfwRequestor)in.requestor;
    strlcpy(out.requestorId, in.requestorId.c_str(), in.requestorId.size()<GNSS_MAX_NFW_STRING_LEN?
            in.requestorId.size():GNSS_MAX_NFW_STRING_LEN);
    out.responseType = (GnssNfwResponseType)in.responseType;
    out.inEmergencyMode = in.inEmergencyMode;
    out.isCachedLocation = in.isCachedLocation;
}
/*
 * GnssCallback class implements the callback methods for IGnss interface.
 */
struct GnssCallback : public BnGnssCallback {
  ::ndk::ScopedAStatus gnssSetCapabilitiesCb(int32_t capabilities) override;
  ::ndk::ScopedAStatus gnssStatusCb(IGnssCallback::GnssStatusValue status) override;
  ::ndk::ScopedAStatus gnssSvStatusCb(
          const std::vector<IGnssCallback::GnssSvInfo>& svInfoList) override;
  ::ndk::ScopedAStatus gnssLocationCb(const GnssLocation& location) override;
  ::ndk::ScopedAStatus gnssNmeaCb(int64_t timestamp, const std::string& nmea) override;
  ::ndk::ScopedAStatus gnssAcquireWakelockCb() { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssReleaseWakelockCb() { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssSetSystemInfoCb(
          const IGnssCallback::GnssSystemInfo& info) { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssRequestTimeCb() override;
  ::ndk::ScopedAStatus gnssRequestLocationCb(
          bool independentFromGnss, bool isUserEmergency) override;
  ::ndk::ScopedAStatus gnssSetSignalTypeCapabilitiesCb(
          const std::vector<GnssSignalType>& in_gnssSignalTypes) override;
};

/*
 * GnssBatchingCallback interface implements the callback methods
 * required by the IGnssBatching interface.
 */
struct GnssBatchingCallback : public BnGnssBatchingCallback {
    virtual ::ndk::ScopedAStatus gnssLocationBatchCb(
            const std::vector<GnssLocation>& locations) override;
};

struct GnssGeofenceCallback : public BnGnssGeofenceCallback {
  virtual ::ndk::ScopedAStatus gnssGeofenceTransitionCb(int32_t geofenceId,
          const GnssLocation& location, int32_t transition, int64_t timestampMillis) override;
  virtual ::ndk::ScopedAStatus gnssGeofenceStatusCb(
          int32_t availability, const GnssLocation& lastLocation) override;
  virtual ::ndk::ScopedAStatus gnssGeofenceAddCb(int32_t geofenceId, int32_t status) override;
  virtual ::ndk::ScopedAStatus gnssGeofenceRemoveCb(int32_t geofenceId, int32_t status) override;
  virtual ::ndk::ScopedAStatus gnssGeofencePauseCb(int32_t geofenceId, int32_t status) override;
  virtual ::ndk::ScopedAStatus gnssGeofenceResumeCb(int32_t geofenceId, int32_t status) override;
};

::ndk::ScopedAStatus GnssCallback::gnssLocationCb(const GnssLocation& gnssLocation) {
    Location location;
    memset(&location, 0, sizeof(Location));
    translateLocation(location, gnssLocation);
    LOC_LOGD("%s]: (flags: %02x)", __FUNCTION__, location.flags);

    lock_guard<std::mutex> guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssLocationCb(location);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssCallback::gnssStatusCb(IGnssCallback::GnssStatusValue status) {
    lock_guard<std::mutex> guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssStatusCb(static_cast<uint32_t>(status));
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssCallback::gnssSvStatusCb(
        const std::vector<IGnssCallback::GnssSvInfo>& svInfoList) {
    GnssSvNotification gnssSvNotification;
    memset(&gnssSvNotification, 0, sizeof(GnssSvNotification));
    gnssSvNotification.size = sizeof(GnssSvNotification);
    uint32_t numSvs = std::min((int)svInfoList.size(), (GNSS_SV_MAX));
    if (numSvs > 0) {
        gnssSvNotification.count = numSvs;
        for (int i=0; i< numSvs; ++i) {
            gnssSvNotification.gnssSvs[i].size = sizeof(GnssSv);
            gnssSvNotification.gnssSvs[i].svId = static_cast<uint16_t>(svInfoList[i].svid);
            gnssSvNotification.gnssSvs[i].type =
                static_cast<GnssSvType>(svInfoList[i].constellation);
            gnssSvNotification.gnssSvs[i].cN0Dbhz = svInfoList[i].cN0Dbhz;
            gnssSvNotification.gnssSvs[i].elevation = svInfoList[i].elevationDegrees;
            gnssSvNotification.gnssSvs[i].azimuth = svInfoList[i].azimuthDegrees;
            gnssSvNotification.gnssSvs[i].gnssSvOptionsMask =
                static_cast<uint16_t>(svInfoList[i].svFlag);
        }
        LOC_LOGd("(count: %zu)", gnssSvNotification.count);
        lock_guard<std::mutex> guard(sGnssLock);
        if (sGnssCbs != nullptr) {
            sGnssCbs->gnssSvStatusCb(gnssSvNotification);
        }
    } else {
        LOC_LOGe("Invalid Sv count");
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssCallback::gnssNmeaCb(int64_t timestamp, const std::string& nmea) {
    lock_guard<std::mutex> guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssNmeaCb(timestamp, nmea.c_str(), nmea.size());
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssCallback::gnssSetCapabilitiesCb(int32_t capabilities) {
    LOC_LOGD("%s]: (%02x)", __FUNCTION__, capabilities);
    lock_guard<std::mutex> guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssSetCapabilitiesCb(capabilities);
    }
    return ndk::ScopedAStatus::ok();
}

  ::ndk::ScopedAStatus GnssCallback::gnssRequestTimeCb() {
    lock_guard<std::mutex> guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssRequestUtcTimeCb();
    }
    return ndk::ScopedAStatus::ok();
}
  ::ndk::ScopedAStatus GnssCallback::gnssRequestLocationCb(
          bool independentFromGnss, bool isUserEmergency) {
    lock_guard<std::mutex> guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssRequestLocationCb(independentFromGnss, false);
    }
    return ndk::ScopedAStatus::ok();
}

  ::ndk::ScopedAStatus GnssCallback::gnssSetSignalTypeCapabilitiesCb(
      const std::vector<::aidl::android::hardware::gnss::GnssSignalType>& in_gnssSignalTypes) {
      return ndk::ScopedAStatus::ok();
  }

::ndk::ScopedAStatus gnssLocationBatchCb(const std::vector<GnssLocation>& locations) {
    size_t length = locations.size();
    LOC_LOGd("(count: %zu)", length);
    lock_guard<std::mutex> guard(sFlpLock);
    Location *location = new Location[length];
    if (nullptr == location) {
        LOC_LOGe("fail to alloc memory");
        return ndk::ScopedAStatus::ok();
    }
    if(sFlpCbs != nullptr) {
        for (int i=0; i<length; ++i) {
            memset(&location[i], 0, sizeof(Location));
            translateLocation(location[i], locations[i]);
        }
        sFlpCbs->batchingCb(length, location);
    }
    delete[] location;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus gnssGeofenceTransitionCb(int32_t geofenceId, const GnssLocation& gnssLocation,
        int32_t transition, int64_t timestampMillis) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    Location location;
    memset(&location, 0, sizeof(Location));
    translateLocation(location, gnssLocation);
    GeofenceBreachType geoBreach = GEOFENCE_BREACH_UNKNOWN;
    if (IGnssGeofenceCallback::ENTERED == transition) {
        geoBreach = GEOFENCE_BREACH_ENTER;
    } else if (IGnssGeofenceCallback::EXITED == transition) {
        geoBreach = GEOFENCE_BREACH_EXIT;
    }
    lock_guard<std::mutex> guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->geofenceBreachCb(geofenceId, location, geoBreach,
                static_cast<uint64_t>(timestampMillis));
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssGeofenceCallback::gnssGeofenceStatusCb(int status,
        const GnssLocation& location) {
    LOC_LOGD("%s]: (%d)", __func__, status);
    GeofenceStatusAvailable available = GEOFENCE_STATUS_AVAILABILE_NO;
    if (IGnssGeofenceCallback::AVAILABLE == status) {
        available = GEOFENCE_STATUS_AVAILABILE_YES;
    }
    lock_guard<std::mutex> guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->geofenceStatusCb(available);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssGeofenceCallback::gnssGeofenceAddCb(int32_t geofenceId, int status) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    LocationError error;
    translateStatus(error, status);
    lock_guard<std::mutex> guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->addGeofencesCb(geofenceId, error);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssGeofenceCallback::gnssGeofenceRemoveCb(int32_t geofenceId,
        int status) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    LocationError error;
    translateStatus(error, status);
    lock_guard<std::mutex> guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->removeGeofencesCb(geofenceId, error);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssGeofenceCallback::gnssGeofencePauseCb(int32_t geofenceId,
        int status) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    LocationError error;
    translateStatus(error, status);
    lock_guard<std::mutex> guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->pauseGeofencesCb(geofenceId, error);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssGeofenceCallback::gnssGeofenceResumeCb(int32_t geofenceId,
        int status) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    LocationError error;
    translateStatus(error, status);
    lock_guard<std::mutex> guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->resumeGeofencesCb(geofenceId, error);
    }
    return ndk::ScopedAStatus::ok();
}
/* ===========================================================
 *   AIDL Callbacks : ILocAidlFlpServiceCallback.hal
 * ===========================================================*/
struct GnssFlpServiceCallback : public BnLocAidlFlpServiceCallback {

    ::ndk::ScopedAStatus gnssLocationTrackingCb(
      const LocAidlLocation& gnssLocation) override;

    ::ndk::ScopedAStatus gnssLocationBatchingCb(
      const LocAidlBatchOptions &batchOptions,
      const std::vector<LocAidlLocation>& gnssLocations) override;

    ::ndk::ScopedAStatus gnssBatchingStatusCb(
            const LocAidlBatchStatusInfo &batchStatus,
            const std::vector<int32_t>& listOfCompletedTrips) override;

    ::ndk::ScopedAStatus gnssMaxPowerAllocatedCb(int32_t powerInMW) override;
};

::ndk::ScopedAStatus GnssFlpServiceCallback::gnssLocationTrackingCb(
        const LocAidlLocation& gnssLocation) {
    LOC_LOGD("%s]: (flags: %02x)", __FUNCTION__, gnssLocation.locationFlagsMask);
    Location location;
    memset(&location, 0, sizeof(Location));
    translateLocation(location, gnssLocation);
    location.conformityIndex = gnssLocation.conformityIndex;
    lock_guard<std::mutex> guard(sFlpLock);
    if (sFlpCbs != nullptr) {
        sFlpCbs->gnssLocationCb(location);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssFlpServiceCallback::gnssLocationBatchingCb(
        const LocAidlBatchOptions &batchOptions,
        const std::vector<LocAidlLocation>& gnssLocations) {
    size_t length = gnssLocations.size();
    LOC_LOGd("(count: %zu)", length);
    lock_guard<std::mutex> guard(sFlpLock);
    Location* location = new Location[length];
    if (nullptr == location) {
        LOC_LOGe("fail to alloc memory");
        return ndk::ScopedAStatus::ok();
    }
    if(sFlpCbs != nullptr) {
        for (int i=0; i<length; ++i) {
            memset(&location[i], 0, sizeof(Location));
            translateLocation(location[i], gnssLocations[i]);
        }
        sFlpCbs->batchingCb(length, location);
    }
    delete[] location;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssFlpServiceCallback::gnssBatchingStatusCb(
        const LocAidlBatchStatusInfo &batchStatusInfo,
        const std::vector<int32_t>& listOfCompletedTrips) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssFlpServiceCallback::gnssMaxPowerAllocatedCb(
        int32_t powerInMW) {
    return ndk::ScopedAStatus::ok();
}


/* =================================================
 *   AIDL Callbacks : ILocAidlGnssNiCallback.hal
 * =================================================*/
struct GnssNiVendorCallback : public BnLocAidlGnssNiCallback {

    ::ndk::ScopedAStatus niNotifyCbExt(
            const LocAidlGnssNiNotification& notification) override;

    ::ndk::ScopedAStatus gnssCapabilitiesCb(int32_t capabilitiesMask) override;
};

::ndk::ScopedAStatus GnssNiVendorCallback::niNotifyCbExt(
        const LocAidlGnssNiNotification& gnssNiNotification) {
    LOC_LOGD("%s]: (%d)", __func__, gnssNiNotification.notificationId);
    ::GnssNiNotification gnssNiNotif;
    memset(&gnssNiNotif, 0, sizeof(gnssNiNotif));
    gnssNiNotif.size = sizeof(gnssNiNotif);
    if (static_cast<uint8_t>(gnssNiNotification.niType) != 0) {
        gnssNiNotif.type = static_cast<::GnssNiType>((uint8_t)gnssNiNotification.niType-1);
    }
    gnssNiNotif.options = static_cast<uint16_t>(gnssNiNotification.notifyFlags);
    gnssNiNotif.timeout = gnssNiNotification.timeoutSec;
    gnssNiNotif.timeoutResponse = static_cast<GnssNiResponse>(gnssNiNotification.defaultResponse);
    memcpy(&gnssNiNotif.requestor, gnssNiNotification.requestorId.c_str(),
            gnssNiNotification.requestorId.size()<GNSS_NI_REQUESTOR_MAX?
            gnssNiNotification.requestorId.size():GNSS_NI_REQUESTOR_MAX);
    gnssNiNotif.requestorEncoding =
            static_cast<::GnssNiEncodingType>(gnssNiNotification.requestorIdEncoding);
    memcpy(&gnssNiNotif.message, gnssNiNotification.notificationMessage.c_str(),
            gnssNiNotification.notificationMessage.size()<GNSS_NI_MESSAGE_ID_MAX?
            gnssNiNotification.notificationMessage.size():GNSS_NI_MESSAGE_ID_MAX);
    gnssNiNotif.messageEncoding =
            static_cast<::GnssNiEncodingType>(gnssNiNotification.notificationIdEncoding);

    lock_guard<std::mutex> guard(sGnssLock);
    if (sGnssCbs == nullptr) {
        gardenPrint("%s]: mGnssNiCbIface is nullptr", __FUNCTION__);
        return ndk::ScopedAStatus::ok();
    }
    sGnssCbs->gnssNiNotifyCb(gnssNiNotification.notificationId, gnssNiNotif);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssNiVendorCallback::gnssCapabilitiesCb(
        int32_t capabilitiesMask) {
    ALOGE("%s] Not supported !", __FUNCTION__);
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : ILocAidlGeofenceServiceCallback.hal
 * ===========================================================*/
struct GnssGeofenceServiceProviderCallback :
        public BnLocAidlGeofenceServiceCallback {

    ::ndk::ScopedAStatus gnssGeofenceBreachCb(
            int32_t count, const std::vector<int32_t>& idList,
            const LocAidlLocation& aidlLocation, int32_t breachType,
            int64_t timestamp) override;
    ::ndk::ScopedAStatus gnssGeofenceStatusCb(
            int32_t statusAvailable, int32_t locTechType) override;
    ::ndk::ScopedAStatus gnssAddGeofencesCb(
            int32_t count, const std::vector<int32_t>& locationErrorList,
            const std::vector<int32_t>& idList) override;
    ::ndk::ScopedAStatus gnssRemoveGeofencesCb(
            int32_t count, const std::vector<int32_t>& locationErrorList,
            const std::vector<int32_t>& idList) override;
    ::ndk::ScopedAStatus gnssPauseGeofencesCb(
            int32_t count, const std::vector<int32_t>& locationErrorList,
            const std::vector<int32_t>& idList) override;
    ::ndk::ScopedAStatus gnssResumeGeofencesCb(
            int32_t count, const std::vector<int32_t>& locationErrorList,
            const std::vector<int32_t>& idList) override;
};

::ndk::ScopedAStatus GnssGeofenceServiceProviderCallback::gnssGeofenceBreachCb(
        int32_t count, const std::vector<int32_t>& idList,
        const LocAidlLocation& aidlLocation, int32_t breachType,
        int64_t timestamp) {
    LOC_LOGD("%s]: (%zu)", __func__, count);
    Location location;
    translateLocation(location, aidlLocation);
    lock_guard<std::mutex> guard(sGeofenceLock);
    if (sGeofenceCbs) {
        for (size_t i = 0; i < count; i++) {
            sGeofenceCbs->geofenceBreachCb(idList[i], location,
                    static_cast<GeofenceBreachType>(breachType), timestamp);
        }
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssGeofenceServiceProviderCallback::gnssGeofenceStatusCb(
        int32_t statusAvailable, int32_t /*locTechType*/) {
    LOC_LOGD("%s]: (%d)", __func__, statusAvailable);
    lock_guard<std::mutex> guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->geofenceStatusCb(static_cast<GeofenceStatusAvailable>(statusAvailable));
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssGeofenceServiceProviderCallback::gnssAddGeofencesCb(
        int32_t count, const std::vector<int32_t>& locationErrorList,
        const std::vector<int32_t>& idList) {
    LOC_LOGD("%s]: (%d)", __func__, count);
    lock_guard<std::mutex> guard(sGeofenceLock);
    for (size_t i = 0; i < count; i++) {
        if (sGeofenceCbs) {
            sGeofenceCbs->addGeofencesCb(idList[i],
                    static_cast<LocationError>(locationErrorList[i]));
        }
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssGeofenceServiceProviderCallback::gnssRemoveGeofencesCb(
        int32_t count, const std::vector<int32_t>& locationErrorList,
        const std::vector<int32_t>& idList) {
    LOC_LOGD("%s]: (%d)", __func__, count);
    lock_guard<std::mutex> guard(sGeofenceLock);
    for (size_t i = 0; i < count; i++) {
        if (sGeofenceCbs) {
            sGeofenceCbs->removeGeofencesCb(idList[i],
                    static_cast<LocationError>(locationErrorList[i]));
        }
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssGeofenceServiceProviderCallback::gnssPauseGeofencesCb(
        int32_t count, const std::vector<int32_t>& locationErrorList,
        const std::vector<int32_t>& idList) {
    LOC_LOGD("%s]: (%d)", __func__, count);
    lock_guard<std::mutex> guard(sGeofenceLock);
    for (size_t i = 0; i < count; i++) {
        if (sGeofenceCbs) {
            sGeofenceCbs->pauseGeofencesCb(idList[i],
                    static_cast<LocationError>(locationErrorList[i]));
        }
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssGeofenceServiceProviderCallback::gnssResumeGeofencesCb(
        int32_t count, const std::vector<int32_t>& locationErrorList,
        const std::vector<int32_t>& idList) {
    LOC_LOGD("%s]: (%d)", __func__, count);
    lock_guard<std::mutex> guard(sGeofenceLock);
    for (size_t i = 0; i < count; i++) {
        if (sGeofenceCbs) {
            sGeofenceCbs->resumeGeofencesCb(idList[i],
                    static_cast<LocationError>(locationErrorList[i]));
        }
    }
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : ILocAidlIzatSubscriptionCallback.hal
 * ===========================================================*/
struct IzatSubscriptionCallback : public BnLocAidlIzatSubscriptionCallback {
    ::ndk::ScopedAStatus requestData(
            const std::vector<LocAidlSubscriptionDataItemId>& list) override;
    ::ndk::ScopedAStatus updateSubscribe(const std::vector<LocAidlSubscriptionDataItemId>& list,
                                 bool subscribe) override;
    ::ndk::ScopedAStatus unsubscribeAll() override;
    ::ndk::ScopedAStatus turnOnModule(LocAidlSubscriptionDataItemId di, int32_t timeout) override;
    ::ndk::ScopedAStatus turnOffModule(LocAidlSubscriptionDataItemId di) override;
    ::ndk::ScopedAStatus boolDataItemUpdate(const std::vector<LocAidlBoolDataItem>& diVec)
            override {
        return ndk::ScopedAStatus::ok();
    }
};

::ndk::ScopedAStatus IzatSubscriptionCallback::requestData(
        const std::vector<LocAidlSubscriptionDataItemId>& list) {
    std::vector<int> dataItemArray;

    for (int i = 0; i < list.size(); i++){
        dataItemArray.push_back((int)list[i]);
    }

    lock_guard<std::mutex> guard(sDataItemLock);
    if (sDataItemCbs) {
        sDataItemCbs->requestDataCb(dataItemArray);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IzatSubscriptionCallback::updateSubscribe(
        const std::vector<LocAidlSubscriptionDataItemId>& list, bool subscribe) {
    std::vector<int> dataItemArray;

    for (int i = 0; i < list.size(); i++){
        dataItemArray.push_back((int)list[i]);
    }

    lock_guard<std::mutex> guard(sDataItemLock);
    if (sDataItemCbs) {
        sDataItemCbs->updateSubscribeCb(dataItemArray, subscribe);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IzatSubscriptionCallback::unsubscribeAll() {
    lock_guard<std::mutex> guard(sDataItemLock);
    if (sDataItemCbs) {
        sDataItemCbs->unsbscribeAllCb();
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IzatSubscriptionCallback::turnOnModule(
        LocAidlSubscriptionDataItemId di, int32_t timeout) {
    lock_guard<std::mutex> guard(sDataItemLock);
    if (sDataItemCbs) {
        sDataItemCbs->turnOnCb((int32_t)di, timeout);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IzatSubscriptionCallback::turnOffModule(LocAidlSubscriptionDataItemId di) {
    lock_guard<std::mutex> guard(sDataItemLock);
    if (sDataItemCbs) {
        sDataItemCbs->turnOffCb((int32_t)di);
    }
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : ILocAidlIzatProviderCallback.hal
 * ===========================================================*/
struct IzatProviderCallback : public BnLocAidlIzatProviderCallback {
    ::ndk::ScopedAStatus onLocationChanged(const LocAidlIzatLocation& izatlocation) override;
    ::ndk::ScopedAStatus onStatusChanged(LocAidlIzatProviderStatus status) override;
};

::ndk::ScopedAStatus IzatProviderCallback::onLocationChanged(
        const LocAidlIzatLocation& izatlocation) {
    Location location;
    memset(&location, 0, sizeof(Location));
    translateIzatLocation(location, izatlocation);
    lock_guard<std::mutex> guard(sIzatProviderLock);
    if (sIzatProviderCbs) {
        sIzatProviderCbs->izatProviderLocationCb(location);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IzatProviderCallback::onStatusChanged(LocAidlIzatProviderStatus status) {
    lock_guard<std::mutex> guard(sIzatProviderLock);
    if (sIzatProviderCbs) {
        sIzatProviderCbs->onStatusChanged((uint8_t)status);
    }
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : ILocAidlOsNpGlueCallback.hal
 * ===========================================================*/
struct IzatConfigCallback : public BnLocAidlIzatConfigCallback {
    ::ndk::ScopedAStatus izatConfigCallback(const string& izatConfigContent) override;
};

::ndk::ScopedAStatus IzatConfigCallback::izatConfigCallback(const string& izatConfigContent) {
    lock_guard<std::mutex> guard(sIzatConfigLock);
    if (sIzatConfigCbs) {
        sIzatConfigCbs->izatConfigcallback(izatConfigContent.c_str());
    }
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : ILocAidlGnssConfigServiceCallback.hal
 * ===========================================================*/
struct GnssConfigServiceCallback : public BnLocAidlGnssConfigServiceCallback {
    ::ndk::ScopedAStatus getGnssSvTypeConfigCb(
            const std::vector<LocAidlGnssConstellationType>& disabledSvTypeList) override;
    ::ndk::ScopedAStatus getRobustLocationConfigCb(
            const LocAidlRobustLocationInfo& info) override;
};

::ndk::ScopedAStatus GnssConfigServiceCallback::getGnssSvTypeConfigCb(
        const std::vector<LocAidlGnssConstellationType>& disabledSvTypeList) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssConfigServiceCallback::getRobustLocationConfigCb(
        const LocAidlRobustLocationInfo& info) {
    if (sGnssConfigCbs) {
        sGnssConfigCbs->getRobustLocationConfigCb(info.validMask, info.enable, info.enableForE911,
                info.major, info.minor);
    }
    return ndk::ScopedAStatus::ok();
}
/* =================================================
 *    AIDL Callbacks : ILocAidlAGnssCallback.hal
 * =================================================*/
struct AgnssExtCallback : public BnLocAidlAGnssCallback {
    ::ndk::ScopedAStatus locAidlAgnssStatusIpV6Cb(
            const LocAidlAGnssStatusIpV6& agps_status) override;
    ::ndk::ScopedAStatus locAidlAgnssStatusIpV4Cb(
            const LocAidlAGnssStatusIpV4& agps_status) override;
};

::ndk::ScopedAStatus AgnssExtCallback::locAidlAgnssStatusIpV6Cb(
        const LocAidlAGnssStatusIpV6& agps_status) {
    LOC_LOGD("%s]: Not supported", __func__);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus AgnssExtCallback::locAidlAgnssStatusIpV4Cb(
        const LocAidlAGnssStatusIpV4& agps_status) {
    lock_guard<std::mutex> guard(sAgnssExtLock);
    if (sAgnssExtCbs) {
        sAgnssExtCbs->agnssStatusIpV4Cb((int)agps_status.type,
                                        (uint32_t)agps_status.apnTypeMask,
                                        (int)agps_status.status);
    }
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : ILocAidlWiFiDBReceiverCallback.hal
 * ===========================================================*/

struct WiFiDBReceiverCallback : public BnLocAidlWiFiDBReceiverCallback {
    ::ndk::ScopedAStatus apListUpdateCallback(
        const std::vector<LocAidlApInfo>& apInfoList,
        int32_t apListSize, LocAidlWifiDBListStatus apListStatus,
        const LocAidlUlpLocation& ulpLocation, bool ulpLocValid) override;
    ::ndk::ScopedAStatus statusUpdateCallback(
        bool status, const string& reason) override;
    ::ndk::ScopedAStatus serviceRequestCallback() override;
    ::ndk::ScopedAStatus attachVmOnCallback() override;
};

::ndk::ScopedAStatus WiFiDBReceiverCallback::apListUpdateCallback(
    const std::vector<LocAidlApInfo>& apInfoList,
    int32_t apListSize, LocAidlWifiDBListStatus apListStatus,
    const LocAidlUlpLocation& ulpLocation, bool ulpLocValid) {
    lock_guard<std::mutex> guard(sWiFiDBReceiverLock);
    if (sWifiDBReceiverCbs) {
        std::vector<izat_remote_api::WiFiDBUpdater::APInfo> apList;
        Location location;

        for (int i = 0; i < apListSize; i++) {
            izat_remote_api::WiFiDBUpdater::APInfo apInfo;
            apInfo.mac_R48b = apInfoList[i].mac_R48b;
            apInfo.cellType = apInfoList[i].cell_type;
            apInfo.cellRegionID1 = apInfoList[i].cell_id1;
            apInfo.cellRegionID2 = apInfoList[i].cell_id2;
            apInfo.cellRegionID3 = apInfoList[i].cell_id3;
            apInfo.cellRegionID4 = apInfoList[i].cell_id4;
            memcpy(apInfo.ssid, apInfoList[i].ssid.c_str(),
                    sizeof(apInfo.ssid));
            apInfo.ssid_valid_byte_count = apInfoList[i].ssid_valid_byte_count;
            apInfo.utcTime = apInfoList[i].utc_time;
            apInfo.fdal_status = apInfoList[i].fdal_status;
            apList.push_back(apInfo);
        }
        if (ulpLocValid) {
            translateHidlUlpLocation(location, ulpLocation);
        }
        sWifiDBReceiverCbs->apListUpdateCallback(apList, apList.size(), location, ulpLocValid);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus WiFiDBReceiverCallback::statusUpdateCallback(
        bool status, const string& reason) {
    lock_guard<std::mutex> guard(sWiFiDBReceiverLock);
    if (sWifiDBReceiverCbs) {
        sWifiDBReceiverCbs->wifiDBReceiverStatusUpdateCallback(status, reason.c_str());
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus WiFiDBReceiverCallback::serviceRequestCallback() {
    lock_guard<std::mutex> guard(sWiFiDBReceiverLock);
    if (sWifiDBReceiverCbs) {
        sWifiDBReceiverCbs->wifiDBReceiverServiceRequestCallback();
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus WiFiDBReceiverCallback::attachVmOnCallback() {
    lock_guard<std::mutex> guard(sWiFiDBReceiverLock);
    if (sWifiDBReceiverCbs) {
        sWifiDBReceiverCbs->wifiDBReceiverAttachVmOnCallback();
    }
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : ILocAidlWWANDBReceiverCallback.hal
 * ===========================================================*/
struct WWANDBReceiverCallback : public BnLocAidlWWANDBReceiverCallback {

    ::ndk::ScopedAStatus bsListUpdateCallback(
            const std::vector<LocAidlBsInfo>& bsInfoList,
            int32_t bsListSize, int8_t status, const LocAidlUlpLocation& ulpLocation) override;
    ::ndk::ScopedAStatus statusUpdateCallback(
            bool status, const string& reason) override;
    ::ndk::ScopedAStatus serviceRequestCallback() override;
    ::ndk::ScopedAStatus attachVmOnCallback() override;
};

::ndk::ScopedAStatus WWANDBReceiverCallback::bsListUpdateCallback(
        const std::vector<LocAidlBsInfo>& bsInfoList,
        int32_t bsListSize, int8_t status, const LocAidlUlpLocation& ulpLocation) {
    lock_guard<std::mutex> guard(sWwanDBReceiverLock);
    if (sWwanDBReceiverCbs) {
        std::vector<BSInfo_s> bsList;
        Location location;

        for (int i = 0; i < bsListSize; i++) {
            BSInfo_s bsInfo;
            bsInfo.cell_type = bsInfoList[i].cell_type;
            bsInfo.cell_id1 = bsInfoList[i].cell_id1;
            bsInfo.cell_id2 = bsInfoList[i].cell_id2;
            bsInfo.cell_id3 = bsInfoList[i].cell_id3;
            bsInfo.cell_id4 = bsInfoList[i].cell_id4;
            bsInfo.cell_timestamp = bsInfoList[i].timestamp;
            bsList.push_back(bsInfo);
        }
        translateHidlUlpLocation(location, ulpLocation);

        sWwanDBReceiverCbs->bsListUpdateCallback(bsList, status, location);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus WWANDBReceiverCallback::statusUpdateCallback(
        bool status, const string& reason) {
    lock_guard<std::mutex> guard(sWwanDBReceiverLock);
    if (sWwanDBReceiverCbs) {
        sWwanDBReceiverCbs->wwanDBReceiverStatusUpdateCallback(status, reason.c_str());
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus WWANDBReceiverCallback::serviceRequestCallback() {
    lock_guard<std::mutex> guard(sWwanDBReceiverLock);
    if (sWwanDBReceiverCbs) {
        sWwanDBReceiverCbs->wwanDBReceiverServiceRequestCallback();
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus WWANDBReceiverCallback::attachVmOnCallback() {
    lock_guard<std::mutex> guard(sWwanDBReceiverLock);
    if (sWwanDBReceiverCbs) {
        sWwanDBReceiverCbs->wwanDBReceiverAttachVmOnCallback();
    }
    return ndk::ScopedAStatus::ok();
}


/* ===========================================================
 *   AIDL Callbacks : ILocAidlWiFiDBProviderCallback.hal
 * ===========================================================*/

struct WiFiDBProviderCallback : public BnLocAidlWiFiDBProviderCallback {
    ::ndk::ScopedAStatus apObsLocDataUpdateCallback(
            const std::vector<LocAidlApObsData>& apObsLocDataList,
            int32_t apObsLocDataListSize, LocAidlWifiDBListStatus apListStatus) override;
    ::ndk::ScopedAStatus serviceRequestCallback() override;
    ::ndk::ScopedAStatus attachVmOnCallback() override;
};

::ndk::ScopedAStatus WiFiDBProviderCallback::apObsLocDataUpdateCallback(
        const std::vector<LocAidlApObsData>& apObsLocDataList,
        int32_t apObsLocDataListSize, LocAidlWifiDBListStatus apListStatus) {
    lock_guard<std::mutex> guard(sWiFiDBProviderLock);
    if (sWifiDBProviderCbs) {
        std::vector<izat_remote_api::WiFiDBUploader::APObsData> apObsList;
        uint8_t status = (uint8_t) apListStatus;
        for (int i = 0; i < apObsLocDataListSize; i++) {
            izat_remote_api::WiFiDBUploader::APObsData apObsData;
            apObsData.scanTimestamp_ms = apObsLocDataList[i].scanTimestamp_ms;
            apObsData.bUlpLocValid = apObsLocDataList[i].bUlpLocValid;
            apObsData.cellInfo.cellType = apObsLocDataList[i].cellInfo.cell_type;
            apObsData.cellInfo.cellRegionID1 = apObsLocDataList[i].cellInfo.cell_id1;
            apObsData.cellInfo.cellRegionID2 = apObsLocDataList[i].cellInfo.cell_id2;
            apObsData.cellInfo.cellRegionID3 = apObsLocDataList[i].cellInfo.cell_id3;
            apObsData.cellInfo.cellRegionID4 = apObsLocDataList[i].cellInfo.cell_id4;
            translateHidlUlpLocationToUlpLocation(apObsData.gpsLoc,
                    apObsLocDataList[i].gpsLoc);
            for (int j = 0; j < apObsLocDataList[i].ap_scan_info_size; j++) {
                izat_remote_api::WiFiDBUploader::APScanInfo apScanInfo;
                apScanInfo.mac_R48b = apObsLocDataList[i].ap_scan_info[j].mac_R48b;
                apScanInfo.rssi = apObsLocDataList[i].ap_scan_info[j].rssi;
                apScanInfo.age_usec = apObsLocDataList[i].ap_scan_info[j].age_usec;
                apScanInfo.channel_id = apObsLocDataList[i].ap_scan_info[j].channel_id;
                memcpy(apScanInfo.ssid, apObsLocDataList[i].ap_scan_info[j].ssid.c_str(),
                        sizeof(apScanInfo.ssid));
                apScanInfo.ssid_valid_byte_count =
                        apObsLocDataList[i].ap_scan_info[j].ssid_valid_byte_count;
                apObsData.ap_scan_info.push_back(apScanInfo);
            }
            apObsList.push_back(apObsData);
        }
        sWifiDBProviderCbs->apObsLocDataUpdateCallback(apObsList, status);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus WiFiDBProviderCallback::serviceRequestCallback() {
    lock_guard<std::mutex> guard(sWiFiDBProviderLock);
    if (sWifiDBProviderCbs) {
        sWifiDBProviderCbs->wifiDBProviderServiceRequestCallback();
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus WiFiDBProviderCallback::attachVmOnCallback() {
    lock_guard<std::mutex> guard(sWiFiDBProviderLock);
    if (sWifiDBProviderCbs) {
        sWifiDBProviderCbs->wifiDBProviderAttachVmOnCallback();
    }
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : ILocAidlWWANDBProviderCallback.hal
 * ===========================================================*/

struct WWANDBProviderCallback : public BnLocAidlWWANDBProviderCallback {
    ::ndk::ScopedAStatus bsObsLocDataUpdateCallback(
            const std::vector<LocAidlBSObsData>& bsObsLocDataList,
            int32_t bsObsLocDataListSize, int8_t bsListStatus) override;
    ::ndk::ScopedAStatus serviceRequestCallback() override;
    ::ndk::ScopedAStatus attachVmOnCallback() override;
};

::ndk::ScopedAStatus WWANDBProviderCallback::bsObsLocDataUpdateCallback(
        const std::vector<LocAidlBSObsData>& bsObsLocDataList,
        int32_t bsObsLocDataListSize, int8_t bsListStatus) {
    lock_guard<std::mutex> guard(sWwanDBProviderLock);
    if (sWwanDBProviderCbs) {
        std::vector<BSObsData_s> bsObsList;

        for (int i = 0; i < bsObsLocDataListSize; i++) {
            BSObsData_s bsObsData;
            bsObsData.scanTimestamp_ms = bsObsLocDataList[i].scanTimestamp_ms;
            bsObsData.bUlpLocValid = bsObsLocDataList[i].bUlpLocValid;
            if (bsObsData.bUlpLocValid) {
                translateHidlUlpLocationToUlpLocation(bsObsData.gpsLoc, bsObsLocDataList[i].gpsLoc);
            }
            bsObsData.cellInfo.cell_type = bsObsLocDataList[i].cellInfo.cell_type;
            bsObsData.cellInfo.cell_id1 = bsObsLocDataList[i].cellInfo.cell_id1;
            bsObsData.cellInfo.cell_id2 = bsObsLocDataList[i].cellInfo.cell_id2;
            bsObsData.cellInfo.cell_id3 = bsObsLocDataList[i].cellInfo.cell_id3;
            bsObsData.cellInfo.cell_id4 = bsObsLocDataList[i].cellInfo.cell_id4;
            bsObsList.push_back(bsObsData);
        }
        sWwanDBProviderCbs->bsObsLocDataUpdateCallback(bsObsList, bsListStatus);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus WWANDBProviderCallback::serviceRequestCallback() {
    lock_guard<std::mutex> guard(sWwanDBProviderLock);
    if (sWwanDBProviderCbs) {
        sWwanDBProviderCbs->wwanDBProviderServiceRequestCallback();
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus WWANDBProviderCallback::attachVmOnCallback() {
    lock_guard<std::mutex> guard(sWwanDBProviderLock);
    if (sWwanDBProviderCbs) {
        sWwanDBProviderCbs->wwanDBProviderAttachVmOnCallback();
    }
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : ILocAidlEsStatusReceiverCallback.hal
 * ===========================================================*/

struct EsStatusReceiverCallback : public BnLocAidlEsStatusCallback {
    ::ndk::ScopedAStatus onEsStatusChanged(bool isEmergencyMode) override;
};

::ndk::ScopedAStatus EsStatusReceiverCallback::onEsStatusChanged(bool isEmergencyMode) {
    lock_guard<std::mutex> guard(sEsStatusReceiverLock);
    LOC_LOGD("%s]", __func__);
    LOC_LOGD("isEmergencyMode:%d", isEmergencyMode);
    if (sEsStatusReceiverCbs) {
        sEsStatusReceiverCbs->onEsStatusChanged(isEmergencyMode);
    }
    return ndk::ScopedAStatus::ok();
}

/* ===================================================================
 *   AIDL Interface
 * ===================================================================*/

std::shared_ptr<IGnssBatching>& getBatchingIface() {
    if (gnssBatchingIface == nullptr) {
        if (gnssHal != nullptr) {
            auto ret = gnssHal->getExtensionGnssBatching(&gnssBatchingIface);
            if (!ret.isOk()) {
                ALOGE("Unable to get a handle to IGnssBatching extension!");
            }
        }
    }
    return gnssBatchingIface;
}

std::shared_ptr<IGnssGeofence>& getGeofenceIface() {
    if (gnssGeofencingIface == nullptr) {
        if (gnssHal != nullptr) {
            auto ret = gnssHal->getExtensionGnssGeofence(&gnssGeofencingIface);
            if (!ret.isOk()) {
                ALOGE("Unable to get a handle to IGnssGeofence extension!");
            }
        }
    }
    return gnssGeofencingIface;
}

std::shared_ptr<IAGnss> Gnss_getExtensionAGnss() {
    if (agnssIface == nullptr) {
        if (gnssHal != nullptr) {
            auto status = gnssHal->getExtensionAGnss(&agnssIface);
            if (!status.isOk()) {
                ALOGE("Unable to get a handle to IAGnss extension!");
            }
        }
    }
    return agnssIface;
}

std::shared_ptr<IAGnssRil> Gnss_getExtensionAGnssRil() {
    if (agnssRilIface == nullptr) {
        if (gnssHal != nullptr) {
            auto status = gnssHal->getExtensionAGnssRil(&agnssRilIface);
            if (!status.isOk()) {
                ALOGE("Unable to get a handle to IAGnssRil extension!");
            }
        }
    }
    return agnssRilIface;
}

std::shared_ptr<IGnssConfiguration> getExtensionGnssConfigurationAidl() {
    if (gnssConfigIface == nullptr) {
        if (gnssHal != nullptr) {
            auto ret = gnssHal->getExtensionGnssConfiguration(&gnssConfigIface);
            if (!ret.isOk()) {
                ALOGE("Unable to get a handle to IGnssConfiguration extension!");
            }
        }
    }
    return gnssConfigIface;
}

std::shared_ptr<IGnssDebug> Gnss_getExtensionGnssDebug() {
    if (gnssDebugIface == nullptr) {
        if (gnssHal != nullptr) {
            auto ret = gnssHal->getExtensionGnssDebug(&gnssDebugIface);
            if (!ret.isOk()) {
                ALOGE("Unable to get a handle to IGnssDebug extension!");
            }
        }
    }
    return gnssDebugIface;
}

std::shared_ptr<IGnssMeasurementInterface> getExtensionGnssMeasurementAidl() {
    if (gnssMeasurementIface == nullptr) {
        if (gnssHal != nullptr) {
            auto gnssMeasurementHal = gnssHal->getExtensionGnssMeasurement(&gnssMeasurementIface);
            if (!gnssMeasurementHal.isOk()) {
                ALOGE("Unable to get a handle to IGnssMeasurement extension!");
            }
        }
    }
    return gnssMeasurementIface;
}

std::shared_ptr<IMeasurementCorrectionsInterface> Gnss_getExtensionMeasurementCorrections() {
    if (measurementCorrIface == nullptr) {
        if (gnssHal != nullptr) {
            auto measurementCorrHal = gnssHal->getExtensionMeasurementCorrections(
                    &measurementCorrIface);
            if (!measurementCorrHal.isOk()) {
                ALOGE("Unable to get a handle to IAGnss extension!");
            }
        }
    }
    return measurementCorrIface;
}

std::shared_ptr<IGnssVisibilityControl> Gnss_getExtensionVisibilityControl() {
    if (visibCtrlIface == nullptr) {
        if (gnssHal != nullptr) {
            auto visibCtrlHal = gnssHal->getExtensionGnssVisibilityControl(&visibCtrlIface);
            if (!visibCtrlHal.isOk()) {
                ALOGE("Unable to get a handle to IGnssVisibilityControl extension!");
            }
        }
    }
    return visibCtrlIface;
}

std::shared_ptr<ILocAidlFlpService>& getLocAidlFlpIface() {
    if (locAidlFlpIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status = locAidlHal->getExtensionLocAidlFlpService(&locAidlFlpIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (locAidlFlpIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlFlpService extension!");
        }
    }
    return locAidlFlpIface;
}

std::shared_ptr<ILocAidlGeofenceService>& getLocAidlGeofenceIface() {
    if (locAidlGeofenceIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status = locAidlHal->getExtensionLocAidlGeofenceService(&locAidlGeofenceIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (locAidlGeofenceIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlGeofenceService extension!");
        }
    }
    return locAidlGeofenceIface;
}

std::shared_ptr<ILocAidlIzatSubscription>& getLocAidlIzatSubscriptionIface() {
    if (gnssSubscriptionIface == nullptr) {
        if (locAidlHal != nullptr) {
             auto status= locAidlHal->getExtensionLocAidlIzatSubscription(&gnssSubscriptionIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (gnssSubscriptionIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlIzatSubscription extension!");
        }
    }
    return gnssSubscriptionIface;

}

std::shared_ptr<ILocAidlRilInfoMonitor>& getLocAidlRilInfoMonitorIface() {
    if (gnssRilInfoIface == nullptr) {
        if (locAidlHal != nullptr) {
             auto status= locAidlHal->getExtensionLocAidlRilInfoMonitor(&gnssRilInfoIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (gnssRilInfoIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlIzatSubscription extension!");
        }
    }
    return gnssRilInfoIface;
}

std::shared_ptr<ILocAidlIzatProvider>& getLocAidlIzatProviderIface() {
    if (izatProviderIface == nullptr) {
        if (locAidlHal != nullptr) {
             auto status= locAidlHal->getExtensionLocAidlIzatNetworkProvider(&izatProviderIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (izatProviderIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlIzatProvider extension!");
        }
    }
    return izatProviderIface;
}

std::shared_ptr<ILocAidlIzatConfig>& getLocAidlIzatConfigIface() {
    if (izatConfigIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status= locAidlHal->getExtensionLocAidlIzatConfig(&izatConfigIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (izatConfigIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlIzatConfig extension!");
        }
    }
    return izatConfigIface;
}

std::shared_ptr<ILocAidlGnssConfigService>& getLocAidlGnssConfigIface() {
    if (lochidlgnssConfigIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status = locAidlHal->getExtensionLocAidlGnssConfigService(&lochidlgnssConfigIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (lochidlgnssConfigIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlGnssConfig extension!");
        }
    }
    return lochidlgnssConfigIface;
}

std::shared_ptr<ILocAidlAGnss>& getLocAidlAgnssIface() {
    if (agnssExtIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status = locAidlHal->getExtensionLocAidlAGnss(&agnssExtIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (agnssExtIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlAGnss extension!");
        }
    }
    return agnssExtIface;
}

std::shared_ptr<ILocAidlDebugReportService>& getLocAidlDebugReportServiceIface() {
    if (debugReportIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status = locAidlHal->getExtensionLocAidlDebugReportService(&debugReportIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (debugReportIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlDebugReportService extension!");
        }
    }
    return debugReportIface;
}

std::shared_ptr<ILocAidlWiFiDBReceiver>& getLocAidlWiFiDBReceiverIface() {
    if (wifiDBReceiverIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status = locAidlHal->getExtensionLocAidlWiFiDBReceiver(&wifiDBReceiverIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (wifiDBReceiverIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlWiFiDBReceiver extension!");
        }
    }
    return wifiDBReceiverIface;
}

std::shared_ptr<ILocAidlWiFiDBProvider>& getLocAidlWiFiDBProviderIface() {
    if (wifiDBProviderIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status = locAidlHal->getExtensionLocAidlWiFiDBProvider(&wifiDBProviderIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (wifiDBProviderIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlWiFiDBProvider extension!");
        }
    }
    return wifiDBProviderIface;
}

std::shared_ptr<ILocAidlWWANDBReceiver>& getLocAidlWwanDBReceiverIface() {
    if (wwanDBReceiverIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status = locAidlHal->getExtensionLocAidlWWANDBReceiver(&wwanDBReceiverIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (wwanDBReceiverIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlWWANDBReceiver extension!");
        }
    }
    return wwanDBReceiverIface;
}

std::shared_ptr<ILocAidlWWANDBProvider>& getLocAidlWwanDBProviderIface() {
    if (wwanDBProviderIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status = locAidlHal->getExtensionLocAidlWWANDBProvider(&wwanDBProviderIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (wwanDBProviderIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlWWANDBProvider extension!");
        }
    }
    return wwanDBProviderIface;
}

std::shared_ptr<ILocAidlEsStatusReceiver>& getLocAidlEsStatusReceiverIface() {
    if (esStatusReceiverIface == nullptr) {
        if (locAidlHal != nullptr) {
            auto status = locAidlHal->getExtensionLocAidlEsStatusReceiver(&esStatusReceiverIface);
            if (!status.isOk()) {
                ALOGE("get aidl interface failed!");
            }
        }
        if (esStatusReceiverIface == nullptr) {
            ALOGE("Unable to get a handle to ILocAidlEsStatusReceiver extension!");
        }
    }
    return esStatusReceiverIface;
}

void GfwAidlApi::setGnssCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sGnssLock);
        sGnssCbs = callbacks;
    }
    std::shared_ptr<GnssCallback> gnssCbIface = SharedRefBase::make<GnssCallback>();
    /*
     * Fail if the main interface fails to initialize
     */
    if (gnssHal == nullptr) {
        ALOGE("Unable to Initialize GNSS HAL\n");
    }

    auto result = gnssHal->setCallback(gnssCbIface);
    if (!result.isOk()) {
        ALOGE("SetCallback for Gnss Interface fails\n");
    }

/*    auto gnssNi = gnssHal->getExtensionGnssNi();
    if (!gnssNi.isOk()) {
        ALOGD("Unable to get a handle to GnssNi");
    } else {
        gnssNiIface = gnssNi;
    }
    std::shared_ptr<IGnssNiCallback> gnssNiCbIface = new GnssNiCallback();
    if (gnssNiIface != nullptr) {
        gnssNiIface->setCallback(gnssNiCbIface);
    } else {
        ALOGE("Unable to initialize GNSS NI interface");
    } */
}

void GfwAidlApi::setFlpCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sFlpLock);
        sFlpCbs = callbacks;
    }
    gardenPrint("entry of setFlpCbs.");
    auto flpHidlIface = getLocAidlFlpIface();
    if (flpHidlIface == nullptr) {
        ALOGE("ILocAidlFlpService Iface NULL!");
        return;
    }
    std::shared_ptr<GnssFlpServiceCallback> gnssFlpServiceCb =
        SharedRefBase::make<GnssFlpServiceCallback>();
    std::shared_ptr<ILocAidlFlpService> flpVendorIface = flpHidlIface;
    bool ret;
    flpVendorIface->init(gnssFlpServiceCb, &ret);
#if 0
    std::shared_ptr<IGnssBatchingCallback> gnssBatchingCbIface = new GnssBatchingCallback();
    auto iface = getBatchingIface();
    if (iface == nullptr) {
        ALOGE("IGnssBatching Iface NULL!");
        return;
    }
    iface->init(gnssBatchingCbIface);
#endif
}

void GfwAidlApi::setGeofenceCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sGeofenceLock);
        sGeofenceCbs = callbacks;
    }
    gardenPrint("entry of setGeofenceCbs.");
    auto geofenceVendor = getLocAidlGeofenceIface();
    std::shared_ptr<GnssGeofenceServiceProviderCallback> gnssGeofenceServiceProviderCb =
        SharedRefBase::make<GnssGeofenceServiceProviderCallback>();
    std::shared_ptr<ILocAidlGeofenceService> geofenceVendorIface = geofenceVendor;
    bool ret;
    if (geofenceVendorIface) {
        auto init = geofenceVendorIface->init(gnssGeofenceServiceProviderCb, &ret);
    }
#if 0
    std::shared_ptr<IGnssGeofenceCallback> gnssGeofencingCbIface = new GnssGeofenceCallback();
    auto iface = getGeofenceIface();
    if (iface == nullptr) {
        ALOGE("IGnssGeofence Iface NULL!");
        return;
    }
    iface->setCallback(gnssGeofencingCbIface);
#endif
}

void GfwAidlApi::setIzatSubscriptionCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sGeofenceLock);
        sDataItemCbs = callbacks;
    }
    gardenPrint("entry of setIzatSubscriptionCbs.");
    auto iface = getLocAidlIzatSubscriptionIface();
    if (iface == nullptr) {
        ALOGE("gnssSubscriptionIface Iface NULL!");
        return;
    }
    std::shared_ptr<IzatSubscriptionCallback> subscriptionCb =
        SharedRefBase::make<IzatSubscriptionCallback>();
    bool ret;
    iface->init(subscriptionCb, &ret);
}

void GfwAidlApi::setIzatProviderCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sIzatProviderLock);
        sIzatProviderCbs = callbacks;
    }
    gardenPrint("entry of setIzatProviderCbs.");
    auto iface = getLocAidlIzatProviderIface();
    if (iface == nullptr) {
        ALOGE("gnssIzatProviderIface Iface NULL!");
        return;
    }
    std::shared_ptr<IzatProviderCallback> izatProviderCb =
        SharedRefBase::make<IzatProviderCallback>();
    bool ret;
    iface->init(izatProviderCb, &ret);
}

void GfwAidlApi::setOsNpCbs(GnssCbBase* callbacks) {
    ALOGE("API deprecated!");
}

void GfwAidlApi::setIzatConfigCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sIzatConfigLock);
        sIzatConfigCbs = callbacks;
    }
    gardenPrint("entry of setIzatConfigCbs.");
    auto iface = getLocAidlIzatConfigIface();
    if (iface == nullptr) {
        ALOGE("gnssIzatConfig Iface NULL!");
        return;
    }
    std::shared_ptr<IzatConfigCallback> izatConfigCb = SharedRefBase::make<IzatConfigCallback>();
    bool ret;
    iface->init(izatConfigCb, &ret);
}

void GfwAidlApi::setGnssConfigCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sGnssConfigLock);
        sGnssConfigCbs = callbacks;
    }
    gardenPrint("entry of setGnssConfigCbs.");
    auto iface = getLocAidlGnssConfigIface();
    if (iface == nullptr) {
        ALOGE("gnssGnssConfig Iface NULL!");
        return;
    }
    std::shared_ptr<GnssConfigServiceCallback> gnssConfigCb =
        SharedRefBase::make<GnssConfigServiceCallback>();
    bool ret;
    iface->init(gnssConfigCb, &ret);
}

void GfwAidlApi::setAGnssExtCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sAgnssExtLock);
        sAgnssExtCbs = callbacks;
    }
    gardenPrint("entry of setAgnssExtCbs.");
    auto iface = getLocAidlAgnssIface();
    if (iface == nullptr) {
        ALOGE("gnssAgnss Iface NULL!");
        return;
    }
    std::shared_ptr<AgnssExtCallback> agnssExtCb = SharedRefBase::make<AgnssExtCallback>();
    iface->setCallbackExt(agnssExtCb);
}

void GfwAidlApi::setWiFiDBReceiverCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sWiFiDBReceiverLock);
        sWifiDBReceiverCbs = callbacks;
    }
    gardenPrint("entry of setWiFiDBReceiverCbs.");
    auto iface = getLocAidlWiFiDBReceiverIface();
    if (iface == nullptr) {
        ALOGE("wifidb receiver Iface NULL!");
        return;
    }
    std::shared_ptr<WiFiDBReceiverCallback> wifiDBReceiverCb =
        SharedRefBase::make<WiFiDBReceiverCallback>();
    bool ret;
    iface->init(wifiDBReceiverCb, &ret);
    iface->registerWiFiDBUpdater(wifiDBReceiverCb);
}

void GfwAidlApi::setWWanDBReceiverCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sWwanDBReceiverLock);
        sWwanDBReceiverCbs = callbacks;
    }
    gardenPrint("entry of setWwanDBReceiverCbs.");
    auto iface = getLocAidlWwanDBReceiverIface();
    if (iface == nullptr) {
        ALOGE("wwandb receiver Iface NULL!");
        return;
    }
    std::shared_ptr<WWANDBReceiverCallback> wwanDBReceiverCb =
        SharedRefBase::make<WWANDBReceiverCallback>();
    bool ret;
    iface->init(wwanDBReceiverCb, &ret);
    iface->registerWWANDBUpdater(wwanDBReceiverCb);
}

void GfwAidlApi::setWiFiDBProviderCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sWiFiDBProviderLock);
        sWifiDBProviderCbs = callbacks;
    }
    gardenPrint("entry of setWiFiDBProviderCbs.");
    auto iface = getLocAidlWiFiDBProviderIface();
    if (iface == nullptr) {
        ALOGE("wifidb provider Iface NULL!");
        return;
    }
    std::shared_ptr<WiFiDBProviderCallback> wifiDBProviderCb =
        SharedRefBase::make<WiFiDBProviderCallback>();
    bool ret;
    iface->init(wifiDBProviderCb, &ret);
    iface->registerWiFiDBProvider(wifiDBProviderCb);
}

void GfwAidlApi::setWWanDBProviderCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sWwanDBProviderLock);
        sWwanDBProviderCbs = callbacks;
    }
    gardenPrint("entry of setWWanDBProviderCbs.");
    auto iface = getLocAidlWwanDBProviderIface();
    if (iface == nullptr) {
        ALOGE("wwandb provider Iface NULL!");
        return;
    }
    std::shared_ptr<WWANDBProviderCallback> wwanDBProviderCb =
        SharedRefBase::make<WWANDBProviderCallback>();
    bool ret;
    iface->init(wwanDBProviderCb, &ret);
    iface->registerWWANDBProvider(wwanDBProviderCb);
}

void GfwAidlApi::setEsStatusReceiverCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sEsStatusReceiverLock);
        sEsStatusReceiverCbs = callbacks;
    }
    gardenPrint("entry of setEsStatusReceiverCbs.");
    auto iface = getLocAidlEsStatusReceiverIface();
    if (iface == nullptr) {
        ALOGE("emergency receiver Iface NULL!");
        return;
    }
    std::shared_ptr<EsStatusReceiverCallback> esStatusReceiverCb
        = SharedRefBase::make<EsStatusReceiverCallback>();
    bool ret;
    iface->setCallback(esStatusReceiverCb, &ret);
}

GfwAidlApi::GfwAidlApi() {
    LOC_LOGd("GfwAidlApi");
    ndk::SpAIBinder gnssBinder(AServiceManager_getService("android.hardware.gnss.IGnss/default"));
    gnssHal = IGnssAidl::fromBinder(gnssBinder);
    ABinderProcess_startThreadPool();

    ndk::SpAIBinder binder(AServiceManager_getService("vendor.qti.gnss.ILocAidlGnss/default"));
    locAidlHal = ILocAidlGnss::fromBinder(binder);;
    if (locAidlHal != nullptr) {
        gardenPrint("get Lochidl api success.");
    } else {
        gardenPrint("get Lochidl api failed.");
    }
#if 0
    if (gnssHal != nullptr) {
        gardenPrint("get hidl api success.");
        gnssHalDeathRecipient = new GnssDeathRecipient();
        android::hardware::::ndk::ScopedAStatus linked = gnssHal->linkToDeath(
                gnssHalDeathRecipient, 0);
        if (!linked.isOk()) {
            ALOGE("Transaction error in linking to GnssHAL death: %s",
                    linked.description().c_str());
        } else if (!linked) {
            ALOGW("Unable to link to GnssHal death notifications");
        } else {
            ALOGD("Link to death notification successful");
        }
    } else {
        gardenPrint("get hidl api failed.");
    }
#endif
}

GfwAidlApi::~GfwAidlApi() {
    LOC_LOGd("~GfwAidlApi");
    sGnssCbs = nullptr;
    sFlpCbs = nullptr;
    sGeofenceCbs = nullptr;
    sAGnssCbs = nullptr;
    sAGnssRilCbs = nullptr;
    sGnssMeasurementCbs = nullptr;
    sVisibCtrlCbs = nullptr;
    sDataItemCbs = nullptr;
    sIzatProviderCbs = nullptr;
    sGnssConfigCbs = nullptr;
    sIzatConfigCbs = nullptr;
    sAgnssExtCbs = nullptr;
    sEsStatusReceiverCbs = nullptr;
    unregisterWiFiDBProvider();
    unregisterWWANDBProvider();
    unregisterWiFiDBUpdater();
    unregisterWWANDBUpdater();
}

int GfwAidlApi::gnssStart() {
    if (gnssHal != nullptr) {
        gnssHal->start();
    }
    return 0;
}

int GfwAidlApi::gnssStop() {
    if (gnssHal != nullptr) {
        gnssHal->stop();
    }
    return 0;
}

void GfwAidlApi::gnssCleanup() {
}

int GfwAidlApi::gnssInjectLocation(double latitude, double longitude, float accuracy) {
    if (gnssHal != nullptr) {
        GnssLocation location = {};
        location.latitudeDegrees = latitude;
        location.longitudeDegrees = longitude;
        location.horizontalAccuracyMeters = accuracy;
        gnssHal->injectLocation(location);
    }
    return 0;
}

void GfwAidlApi::gnssDeleteAidingData(LocGpsAidingData flags) {
    if (gnssHal != nullptr) {
        gnssHal->deleteAidingData(static_cast<IGnss::GnssAidingData>(flags));
    }
}

bool GfwAidlApi::gnssInjectTime(int64_t timeMs, int64_t timeReferenceMs, int32_t uncertaintyMs) {
    if (gnssHal != nullptr) {
        auto result = gnssHal->injectTime(timeMs, timeReferenceMs, uncertaintyMs);
        if (!result.isOk()) {
            return false;
        }
    }
    return true;
}

int GfwAidlApi::gnssSetPositionMode(LocGpsPositionMode mode,
        LocGpsPositionRecurrence recurrence, uint32_t minterval,
        uint32_t preferred_accuracy, uint32_t preferred_time, bool lowPowerMode) {
    if (gnssHal != nullptr) {
        IGnss::PositionModeOptions options;
        options.mode = static_cast<IGnss::GnssPositionMode>(mode);
        options.recurrence = static_cast<IGnss::GnssPositionRecurrence>(recurrence);
        options.minIntervalMs = minterval;
        options.preferredAccuracyMeters = preferred_accuracy;
        options.preferredTimeMs = preferred_time;
        options.lowPowerMode = lowPowerMode;
        gnssHal->setPositionMode(options);
    }
    return 0;
}

void GfwAidlApi::configurationUpdate(const char* config_data, int32_t length) {
    LOC_LOGD("%s]: (%s %d)", __func__, config_data, length);
    const int n = 11;
    uint8_t flags[n];
    memset(&flags, 0, sizeof(uint8_t) * n);
    GnssConfig data;
    memset(&data, 0, sizeof(GnssConfig));
    data.size = sizeof(GnssConfig);

    const loc_param_s_type gnssConfTable[] =
    {
        {"GPS_LOCK",                            &data.gpsLock,                      flags+0,  'n'},
        {"SUPL_VER",                            &data.suplVersion,                  flags+1,  'n'},
        {"LPP_PROFILE",                         &data.lppProfileMask,               flags+3,  'n'},
        {"LPPE_CP_TECHNOLOGY",                  &data.lppeControlPlaneMask,         flags+4,  'n'},
        {"LPPE_UP_TECHNOLOGY",                  &data.lppeUserPlaneMask,            flags+5,  'n'},
        {"A_GLONASS_POS_PROTOCOL_SELECT",       &data.aGlonassPositionProtocolMask, flags+6,  'n'},
        {"USE_EMERGENCY_PDN_FOR_EMERGENCY_SUPL",&data.emergencyPdnForEmergencySupl, flags+7,  'n'},
        {"SUPL_ES",                             &data.suplEmergencyServices,        flags+8,  'n'},
        {"SUPL_MODE",                           &data.suplModeMask,                 flags+9,  'n'},
        {"EMERGENCY_EXTENSION_SECONDS",         &data.emergencyExtensionSeconds,    flags+10, 'n'},
    };
    UTIL_UPDATE_CONF(config_data, length, gnssConfTable);

    for (int i = 0; i < n; i++) {
        if (flags[i] != 0)
            data.flags |= (0x1 << i);
    }

    std::shared_ptr<IGnssConfiguration> gnssConf;
    if (gnssHal != nullptr) {
        auto gnssConfig = gnssHal->getExtensionGnssConfiguration(&gnssConf);
        if (!gnssConfig.isOk()) {
            ALOGE("Unable to get a handle to IGnssConfiguration extension!");
        } else if (gnssConf != nullptr) {
            gnssConf->setSuplVersion(data.suplVersion);
            gnssConf->setSuplMode(data.suplModeMask);
            gnssConf->setLppProfile(data.lppProfileMask);
            gnssConf->setGlonassPositioningProtocol(data.aGlonassPositionProtocolMask);
            gnssConf->setEmergencySuplPdn(data.emergencyPdnForEmergencySupl);
            gnssConf->setEsExtensionSec(data.emergencyExtensionSeconds);
        }
    }
}

void GfwAidlApi::updateNetworkAvailability(uint64_t netHandle, bool isConnected,
                                           uint64_t caps, const char* apn) {
    if (gnssHal != nullptr) {
        auto agnssRilIface = Gnss_getExtensionAGnssRil();
        if (agnssRilIface == nullptr) {
            ALOGE("Unable to get a handle to IAGnssRil extension!");
        } else {
            IAGnssRil::NetworkAttributes attr;
            attr.networkHandle = netHandle;
            attr.isConnected = isConnected;
            attr.capabilities = caps;
            attr.apn = apn;
            agnssRilIface->updateNetworkState(attr);
        }
    }
}

int GfwAidlApi::gnssInjectBestLocation(Location location) {
    if (gnssHal != nullptr) {
        GnssLocation gnssLocation;
        memset(&gnssLocation, 0, sizeof(gnssLocation));
        translateToHidlLocation(gnssLocation, location);
        gnssHal->injectBestLocation(gnssLocation);
    }
    return 0;
}
void GfwAidlApi::configSvStatus(bool enable) {
    if (gnssHal != nullptr) {
        if (enable) {
            gnssHal->startSvStatus();
        } else {
            gnssHal->stopSvStatus();
        }
    }
}
void GfwAidlApi::configNmea(bool enable) {
    if (gnssHal != nullptr) {
        if (enable) {
            gnssHal->startNmea();
        } else {
            gnssHal->stopNmea();
        }
    }
}

int GfwAidlApi::flpStartSession(uint32_t mode, LocationOptions options, uint32_t id) {
    auto iface = getLocAidlFlpIface();
    if (iface  == nullptr) {
        ALOGE("ILocAidlFlpService Iface NULL!");
        return 1;
    }
    uint32_t flags = mode;
    int32_t ret;
    iface->startFlpSession(id, flags, (int64_t)options.minInterval*1000000, options.minDistance, 0,
            GNSS_POWER_MODE_M2, 0, &ret);
#if 0
    IGnssBatching::Options batchingOptions;
    memset(&batchingOptions, 0, sizeof(batchingOptions));
    batchingOptions.periodNanos = options.minInterval*1000000;
    if (SESSION_MODE_ON_FULL == mode)
        batchingOptions.flags = static_cast<uint8_t>(IGnssBatching::Flag::WAKEUP_ON_FIFO_FULL);
    auto iface = getBatchingIface();
    if (iface == nullptr) {
        ALOGE("IGnssBatching Iface NULL!");
        return 1;
    }
    iface->start(batchingOptions);
#endif
    return 0;
}

int GfwAidlApi::flpStopSession(uint32_t id) {
    auto iface = getLocAidlFlpIface();
    if (iface  == nullptr) {
        ALOGE("ILocAidlFlpService Iface NULL!");
        return 1;
    }
    int32_t ret;
    iface->stopFlpSession(id, &ret);
#if 0
    auto iface = getBatchingIface();
    if (iface == nullptr) {
        ALOGE("IGnssBatching Iface NULL!");
        return 1;
    }
    iface->stop();
#endif
    return 0;
}

int GfwAidlApi::flpGetBatchSize() {
    auto iface = getLocAidlFlpIface();
    ALOGE("%s] Not supported !", __FUNCTION__);
    return 0;
#if 0
    auto iface = getBatchingIface();
    if (iface == nullptr) {
        ALOGE("IGnssBatching Iface NULL!");
        return 1;
    }
    return iface->getBatchSize();
#endif
}

int GfwAidlApi::flpUpdateSession(uint32_t sessionMode, LocationOptions options,
        uint32_t id) {
    auto iface = getLocAidlFlpIface();
    if (iface  == nullptr) {
        ALOGE("ILocAidlFlpService Iface NULL!");
        return 1;
    }
    uint32_t flags = 0;
    if (SESSION_MODE_ON_FULL == sessionMode) {
        flags = static_cast<uint32_t>(IGnssBatching::WAKEUP_ON_FIFO_FULL);
    }
    int32_t ret;
    iface->updateFlpSession(id, flags, (int64_t)options.minInterval*1000000, options.minDistance,
            0, GNSS_POWER_MODE_M2, 0, &ret);
    return 0;
}

int GfwAidlApi::flpGetBatchedLocation(int /*lastNLocations*/, uint32_t id) {
    auto iface = getLocAidlFlpIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlFlpService Iface NULL!");
        return 1;
    }
    int32_t ret;
    iface->getAllBatchedLocations(id, &ret);
    return 0;
}

void GfwAidlApi::flpCleanUp() {
    auto iface = getLocAidlFlpIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlFlpService Iface NULL!");
        return;
    }
    bool ret;
    iface->init(nullptr, &ret);
#if 0
    auto iface = getBatchingIface();
    if (iface == nullptr) {
        ALOGE("IGnssBatching Iface NULL!");
        return;
    }
    iface->cleanup();
#endif
}

void GfwAidlApi::addGeofence(uint32_t geofenceId, double latitudeDegrees, double longitudeDegrees,
        double radiusMeters, GeofenceBreachTypeMask monitorTransitions,
        uint32_t notificationResponsivenessMs,
        uint32_t dwellTimeSeconds) {
    auto iface = getLocAidlGeofenceIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlGeofenceService Iface NULL!");
        return;
    }
    iface->addGeofence(geofenceId, latitudeDegrees, longitudeDegrees, radiusMeters,
           GEOFENCE_BREACH_ENTERED|GEOFENCE_BREACH_EXITED|GEOFENCE_BREACH_UNCERTAIN,
           notificationResponsivenessMs, 0/*confidence not used*/, 1,
           0/*dwell time mask not used*/);
#if 0
    auto iface = getGeofenceIface();
    if (iface == nullptr) {
        ALOGE("IGnssGeofence Iface NULL!");
        return;
    }
    iface->addGeofence(geofenceId, latitudeDegrees, longitudeDegrees, radiusMeters,
           static_cast<IGnssGeofenceCallback::GeofenceTransition>(0x100),
           GEOFENCE_BREACH_ENTERED|GEOFENCE_BREACH_EXITED|GEOFENCE_BREACH_UNCERTAIN,
           notificationResponsivenessMs, 100);
#endif
}
void GfwAidlApi::removeGeofence(uint32_t geofenceId) {
    auto iface = getLocAidlGeofenceIface();
#if 0
    auto iface = getGeofenceIface();
#endif
    if (iface == nullptr) {
        ALOGE("IGnssGeofence Iface NULL!");
        return;
    }
    iface->removeGeofence(geofenceId);
}
void GfwAidlApi::modifyGeofence(uint32_t geofenceId, GeofenceBreachTypeMask monitorTransitions,
        uint32_t notificationResponsivenessMs, uint32_t dwellTimeSeconds) {
    auto iface = getLocAidlGeofenceIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlGeofenceService Iface NULL!");
        return;
    }
    iface->updateGeofence(geofenceId, monitorTransitions, notificationResponsivenessMs);
}
void GfwAidlApi::pauseGeofence(uint32_t geofenceId) {
    auto iface = getLocAidlGeofenceIface();
#if 0
    auto iface = getGeofenceIface();
#endif
    if (iface == nullptr) {
        ALOGE("IGnssGeofence Iface NULL!");
        return;
    }
    iface->pauseGeofence(geofenceId);
}
void GfwAidlApi::resumeGeofence(uint32_t geofenceId, GeofenceBreachTypeMask monitorTransitions) {
    auto iface = getLocAidlGeofenceIface();
#if 0
    auto iface = getGeofenceIface();
#endif
    if (iface == nullptr) {
        ALOGE("IGnssGeofence Iface NULL!");
        return;
    }
    iface->resumeGeofence(geofenceId, monitorTransitions);
}
void GfwAidlApi::removeAllGeofences() {
}

/* ===========================================================
 *   AIDL Callbacks : IAGnssCallback.hal
 * ===========================================================*/
struct AGnssCallback: public BnAGnssCallback {
    ::ndk::ScopedAStatus agnssStatusCb(IAGnssCallback::AGnssType type,
            IAGnssCallback::AGnssStatusValue status);
};
::ndk::ScopedAStatus AGnssCallback::agnssStatusCb(IAGnssCallback::AGnssType type,
        IAGnssCallback::AGnssStatusValue status) {
    lock_guard<std::mutex> guard(sAGnssLock);
    if (sAGnssCbs) {
        sAGnssCbs->agnssStatusCb(static_cast<uint8_t>(type), static_cast<uint8_t>(status));
    }
    return ndk::ScopedAStatus::ok();

}

/* ===========================================================
 *   AIDL Callbacks : IAGnssRilCallback.hal
 * ===========================================================*/
struct AGnssRilCallback: public BnAGnssRilCallback {
    ::ndk::ScopedAStatus requestSetIdCb(int32_t setIdflag);
    ::ndk::ScopedAStatus requestRefLocCb();

};

::ndk::ScopedAStatus AGnssRilCallback::requestSetIdCb(int32_t setIdflag) {
    lock_guard<std::mutex> guard(sAGnssRilLock);
    if (sAGnssRilCbs) {
        sAGnssRilCbs->requestSetIdCb(setIdflag);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus AGnssRilCallback::requestRefLocCb() {
    lock_guard<std::mutex> guard(sAGnssRilLock);
    if (sAGnssRilCbs) {
        sAGnssRilCbs->requestRefLocCb();
    }
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : IGnssMeasurementCallback.hal
 * ===========================================================*/
struct GnssMeasurementCallback: public BnGnssMeasurementCallback {
    ::ndk::ScopedAStatus gnssMeasurementCb(
            const GnssData& data);
};
::ndk::ScopedAStatus GnssMeasurementCallback::gnssMeasurementCb(
        const GnssData& data) {
    LOC_LOGd("gnssmeasurementCb: data.measurements.size(): %zu", data.measurements.size());
    lock_guard<std::mutex> guard(sGnssMeasurementLock);
    GnssMeasurementsNotification measNotif;
    ALOGE("Calling translateGnssData");
    translateGnssData(measNotif, data);
    ALOGE("sGnssMeasurementCbs=%p", sGnssMeasurementCbs);
    if (sGnssMeasurementCbs) {
        sGnssMeasurementCbs->gnssMeasurementsCb(measNotif);
    }
    return ndk::ScopedAStatus::ok();
}

/* ===========================================================
 *   AIDL Callbacks : IGnssVisibilityControlCallback.hal
 * ===========================================================*/
struct GnssVisibilityControlCallback: public BnGnssVisibilityControlCallback {
    ::ndk::ScopedAStatus nfwNotifyCb(
            const IGnssVisibilityControlCallback::NfwNotification& notification);
    ::ndk::ScopedAStatus isInEmergencySession(bool* _aidl_return);
};
::ndk::ScopedAStatus GnssVisibilityControlCallback::nfwNotifyCb(
        const IGnssVisibilityControlCallback::NfwNotification& notification) {
    lock_guard<std::mutex> guard(sVisibCtrlLock);
    GnssNfwNotification gnssNfwNotif;
    translateGnssNfwNotification(gnssNfwNotif, notification);
    if (sVisibCtrlCbs) {
        sVisibCtrlCbs->nfwNotifyCb(gnssNfwNotif);
    }
    return ndk::ScopedAStatus::ok();

}
::ndk::ScopedAStatus GnssVisibilityControlCallback::isInEmergencySession(bool* _aidl_return) {
    lock_guard<std::mutex> guard(sVisibCtrlLock);
    if (sVisibCtrlCbs) {
        sVisibCtrlCbs->isInEmergencySession();
    }
    return ndk::ScopedAStatus::ok();
}

//=============================Set Gnss HAL Callback Utils=========================================
//=================================================================================================
void GfwAidlApi::setAGnssCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sAGnssLock);
        sAGnssCbs = callbacks;
    }
    auto agnssIface = Gnss_getExtensionAGnss();
    std::shared_ptr<IAGnssCallback> agnssCbIface = SharedRefBase::make<AGnssCallback>();
    if (agnssIface != nullptr) {
        agnssIface->setCallback(agnssCbIface);
    } else {
        ALOGE("Unable to initialize AGNSS interface");
    }
}

void GfwAidlApi::setAGnssRilCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sAGnssRilLock);
        sAGnssRilCbs = callbacks;
    }
    auto agnssRilIface = Gnss_getExtensionAGnssRil();
    std::shared_ptr<IAGnssRilCallback> agnssRilCbIface = SharedRefBase::make<AGnssRilCallback>();
    if (agnssRilIface != nullptr) {
        agnssRilIface->setCallback(agnssRilCbIface);
    } else {
        ALOGE("Unable to initialize AGnssRil interface");
    }
}

void GfwAidlApi::setGnssMeasurementCbs(GnssCbBase* callbacks, bool fullPower, int tbmInMs) {
    {
        lock_guard<std::mutex> guard(sGnssMeasurementLock);
        sGnssMeasurementCbs = callbacks;
    }
    auto gnssMeasurementIface = getExtensionGnssMeasurementAidl();
    std::shared_ptr<GnssMeasurementCallback> gnssMeasurementCbIface =
            SharedRefBase::make<GnssMeasurementCallback>();
    LOC_LOGd("setGnssMeasurementCbs, gnssMeasurementIface : %p", gnssMeasurementIface.get());
    if (gnssMeasurementIface != nullptr) {
        IGnssMeasurementInterface::Options options = {
            .enableFullTracking = fullPower,
            .enableCorrVecOutputs = true,
            .intervalMs = tbmInMs
        };
        gnssMeasurementIface->setCallbackWithOptions(gnssMeasurementCbIface, options);
    } else {
        ALOGE("Unable to initialize GnssMeasurement interface");
    }
}

void GfwAidlApi::setGnssVisibilityControlCbs(GnssCbBase* callbacks) {
    {
        lock_guard<std::mutex> guard(sVisibCtrlLock);
        sVisibCtrlCbs = callbacks;
    }
    auto visibCtrl = Gnss_getExtensionVisibilityControl();
    std::shared_ptr<IGnssVisibilityControlCallback> visibCtrlCbIface =
            SharedRefBase::make<GnssVisibilityControlCallback>();
    if (visibCtrl != nullptr) {
        visibCtrl->setCallback(visibCtrlCbIface);
    } else {
        ALOGE("Unable to initialize GnssXtra interface");
    }
}

//=============================================Gnss HAL APIs=======================================
//=================================================================================================
    //IAGnss
bool GfwAidlApi::agnssDataConnClosed() {
    if (agnssIface != nullptr) {
        auto result = agnssIface->dataConnClosed();
        if (result.isOk()) {
            ALOGV("agnssIface->dataConnClosed() success.");
            return true;
        } else {
            ALOGE("agnssIface->dataConnClosed() failed.");
        }
    } else {
        ALOGE("agnssIface null");
    }
    return false;
}
bool GfwAidlApi::agnssDataConnFailed() {
    if (agnssIface != nullptr) {
        auto result = agnssIface->dataConnFailed();
        if (!result.isOk()) {
            ALOGE("%s: Failed to notify unavailability of AGnss data connection", __func__);
            return false;

        }
        return true;
    } else {
        ALOGE("agnssIface null");
    }
    return false;
}

bool GfwAidlApi::agnssSetServer(int32_t type, const std::string& hostname, int32_t port) {
    if (agnssIface != nullptr) {
        auto result = agnssIface->setServer((IAGnssCallback::AGnssType)type, hostname, port);

        if (result.isOk()) {
            ALOGV("agnssIface->setServer() success.");
            return true;

        } else {
            ALOGE("agnssIface->setServer() failed.");
        }
    } else {
        ALOGE("agnssIface null");
    }
    return false;
}
bool GfwAidlApi::agnssDataConnOpen(uint64_t networkHandle, const std::string& apn,
        int32_t apnIpType) {
    if (agnssIface != nullptr) {
        auto result = agnssIface->dataConnOpen(networkHandle, apn,
                (IAGnss::ApnIpType)apnIpType);

        if (result.isOk()) {
            ALOGV("agnssIface->dataConnOpen() success.");
            return true;
        } else {
            ALOGE("agnssIface->dataConnOpen() failed.");
        }
    } else {
        ALOGE("agnssIface null");
    }
    return false;
}

    //IAGnssRil
void GfwAidlApi::agnssRilSetRefLocation(int type, int mcc, int mnc, int lac, int cid) {
    if (agnssRilIface != nullptr) {
        IAGnssRil::AGnssRefLocation agnssReflocation;
        switch (static_cast<IAGnssRil::AGnssRefLocationType>(type)) {
            case IAGnssRil::AGnssRefLocationType::GSM_CELLID:
            case IAGnssRil::AGnssRefLocationType::UMTS_CELLID:
                agnssReflocation.type = static_cast<IAGnssRil::AGnssRefLocationType>(type);
                agnssReflocation.cellID.mcc = mcc;
                agnssReflocation.cellID.mnc = mnc;
                agnssReflocation.cellID.lac = lac;
                agnssReflocation.cellID.cid = cid;
                break;
            default:
                ALOGE("Neither a GSM nor a UMTS cellid (%s:%d).", __FUNCTION__, __LINE__);
                return;
        }
        auto result = agnssRilIface->setRefLocation(agnssReflocation);
        if (!result.isOk()) {
            ALOGE("agnssRilIface->setRefLocation failed.");
        } else {
            ALOGV("agnssRilIface->setRefLocation success.");
        }
    } else {
        ALOGE("agnssRilIface is null");
    }
}

bool GfwAidlApi::agnssRilSetSetId(int type, const std::string& setid) {
    if (agnssRilIface != nullptr) {
        auto result = agnssRilIface->setSetId((IAGnssRil::SetIdType)type, setid);
        if (!result.isOk()) {
            ALOGE("agnssRilIface->setSetId failed.");
        } else {
            ALOGV("agnssRilIface->setSetId success.");
            return true;
        }
    } else {
        ALOGE("agnssRilIface is null");
    }
    return false;
}

    //IGnssConfiguration
bool GfwAidlApi::gnssConfigSetBlacklist(
        const std::vector<int>& constellations, const std::vector<int>& svIDs) {
    if (gnssConfigIface != nullptr) {
        int length = constellations.size();
        std::vector<BlocklistedSource> blacklist;
        blacklist.resize(length);

        for (int i = 0; i < length; i++) {
            blacklist[i].constellation = static_cast<GnssConstellationType>(constellations[i]);
            blacklist[i].svid = svIDs[i];
        }
        auto result = gnssConfigIface->setBlocklist(blacklist);
        if (!result.isOk()) {
            ALOGE("gnssConfigIface->setBlocklist failed.");
        } else {
            ALOGV("gnssConfigIface->setBlocklist success.");
            return true;
        }
    } else {
        ALOGE("gnssConfigIface is null");
    }
    return false;
}

    //IGnssDebug
void  GfwAidlApi::gnssDebugGetDebugData(getDebugDataCb _hidl_cb) {
    IGnssDebug::DebugData data;

    auto gnssDebugIface = Gnss_getExtensionGnssDebug();
    if (gnssDebugIface != nullptr) {
        auto result = gnssDebugIface->getDebugData(&data);
        if (!result.isOk()) {
            ALOGE("gnssDebugIface->getDebugData failed.");
        } else {
            ALOGV("gnssDebugIface->getDebugData success.");
        }
    }
    GnssDebugReport report;

    translateGnssDebugReport(report, data);

    _hidl_cb(report);
}

    //IGnssMeasurement
void GfwAidlApi::gnssMeasurementClose() {
    if (gnssMeasurementIface != nullptr) {
        auto result = gnssMeasurementIface->close();
        if (result.isOk()) {
            ALOGV("gnssMeasurementIface->close() success.");
        } else {
            ALOGE("gnssMeasurementIface->close() failed.");
        }
    } else {
        ALOGE("gnssMeasurementIface null");
    }
}

    //IGnssVisibilityControl
bool GfwAidlApi::gnssVisibilityCtrlEnableNfwLocationAccess(
        const std::vector<std::string>& proxyApps) {
    if (visibCtrlIface != nullptr) {
        int length = proxyApps.size();
        std::vector<std::string> hidlProxyApps(length);
        for (int i=0; i<length; ++i) {
            hidlProxyApps[i] = proxyApps[i];
        }
        auto result = visibCtrlIface->enableNfwLocationAccess(hidlProxyApps);
        if (result.isOk()) {
            ALOGV("visibCtrlIface->enableNfwLocationAccess() success.");
            return true;
        } else {
            ALOGE("visibCtrlIface->enableNfwLocationAccess() failed.");
        }
    } else {
        ALOGE("visibCtrlIface null");
    }
    return false;
}

void GfwAidlApi::dataItemUpdate(IDataItemCore* dataItem) {
    if (dataItem == NULL) {
        return;
    }

    auto iface = getLocAidlIzatSubscriptionIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlIzatSubscription interface NULL!");
        return;
    }
    DataItemId id = dataItem->getId();
    switch (id) {
        case MODEL_DATA_ITEM_ID:
        {
            LocAidlStringDataItem di;
            di.id = id;
            di.str = (static_cast<ModelDataItem*>(dataItem))->mModel;
            iface->stringDataItemUpdate(di);
            break;
        }
        case MANUFACTURER_DATA_ITEM_ID:
        {
            LocAidlStringDataItem di;
            di.id = id;
            di.str = (static_cast<ManufacturerDataItem*>(dataItem))->mManufacturer;
            iface->stringDataItemUpdate(di);
            break;
        }
        case TAC_DATA_ITEM_ID:
        {
            LocAidlStringDataItem di;
            di.id = id;
            di.str = (static_cast<TacDataItem*>(dataItem))->mValue;
            iface->stringDataItemUpdate(di);
            break;
        }
        case MCCMNC_DATA_ITEM_ID:
        {
            LocAidlStringDataItem di;
            di.id = id;
            di.str = (static_cast<MccmncDataItem*>(dataItem))->mValue;
            iface->stringDataItemUpdate(di);
            break;
        }
        case NETWORKINFO_DATA_ITEM_ID:
        {
            LocAidlNetworkInfoDataItem di;
            di.type = (static_cast<NetworkInfoDataItem*>(dataItem))->mType;
            di.available = (static_cast<NetworkInfoDataItem*>(dataItem))->mAvailable;
            di.connected = (static_cast<NetworkInfoDataItem*>(dataItem))->mConnected;
            di.roaming = (static_cast<NetworkInfoDataItem*>(dataItem))->mRoaming;
            di.typeName = (static_cast<NetworkInfoDataItem*>(dataItem))->mTypeName;
            di.subTypeName = (static_cast<NetworkInfoDataItem*>(dataItem))->mSubTypeName;
            iface->networkinfoUpdate(di);
            break;
        }
        case RILSERVICEINFO_DATA_ITEM_ID:
        {
            LocAidlRilServiceInfoDataItem di;
            di.type = (static_cast<izat_manager::RilServiceInfoDataItemExt*>(dataItem))
                    ->mServiceInfo->carrierAirIf_type;
            di.mcc = (static_cast<izat_manager::RilServiceInfoDataItemExt*>(dataItem))
                    ->mServiceInfo->carrierMcc;
            di.mnc = (static_cast<izat_manager::RilServiceInfoDataItemExt*>(dataItem))
                    ->mServiceInfo->carrierMnc;
            di.name = (static_cast<izat_manager::RilServiceInfoDataItemExt*>(dataItem))
                    ->mServiceInfo->carrierName;
            iface->serviceinfoUpdate(di);
            break;
        }
        case RILCELLINFO_DATA_ITEM_ID:
        {
            LOC_RilCellInfo *cellInfo =
                    (static_cast<izat_manager::RilCellInfoDataItemExt*>(dataItem))->mCellInfo;
            if (cellInfo->rtType == LOC_RIL_TECH_LTE) {
                LocAidlCellLteDataItem di;
                di.status = cellInfo->nwStatus;
                di.type = cellInfo->rtType;
                di.mcc = cellInfo->u.lteCinfo.mcc;
                di.mnc = cellInfo->u.lteCinfo.mnc;
                di.pci = cellInfo->u.lteCinfo.pci;
                di.tac = cellInfo->u.lteCinfo.tac;
                di.cid = cellInfo->u.lteCinfo.cid;
                di.mask = cellInfo->valid_mask;
                iface->cellLteUpdate(di);
            } else if (cellInfo->rtType == LOC_RIL_TECH_WCDMA ||
                    cellInfo->rtType == LOC_RIL_TECH_GSM) {
                LocAidlCellGwDataItem di;
                di.status = cellInfo->nwStatus;
                di.type = cellInfo->rtType;
                if (cellInfo->rtType == LOC_RILAIRIF_GSM) {
                    di.mcc = cellInfo->u.gsmCinfo.mcc;
                    di.mnc = cellInfo->u.gsmCinfo.mnc;
                    di.lac = cellInfo->u.gsmCinfo.lac;
                    di.cid = cellInfo->u.gsmCinfo.cid;
                } else if (cellInfo->rtType == LOC_RILAIRIF_WCDMA) {
                    di.mcc = cellInfo->u.wcdmaCinfo.mcc;
                    di.mnc = cellInfo->u.wcdmaCinfo.mnc;
                    di.lac = cellInfo->u.wcdmaCinfo.lac;
                    di.cid = cellInfo->u.wcdmaCinfo.cid;
                }
                di.mask = cellInfo->valid_mask;
                iface->cellGwUpdate(di);
            } else if (cellInfo->rtType == LOC_RIL_TECH_CDMA){
                LocAidlCellCdmaDataItem di;
                di.status = cellInfo->nwStatus;
                di.type = cellInfo->rtType;
                di.sid = cellInfo->u.cdmaCinfo.sid;
                di.nid = cellInfo->u.cdmaCinfo.nid;
                di.bsid = cellInfo->u.cdmaCinfo.bsid;
                di.bslat = cellInfo->u.cdmaCinfo.bslat;
                di.bslong = cellInfo->u.cdmaCinfo.bslon;
                di.timeOffset = cellInfo->u.cdmaCinfo.local_timezone_offset_from_utc;
                di.inDST = cellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings;
                di.mask = cellInfo->valid_mask;
                iface->cellCdmaUpdate(di);
            } else if (cellInfo->nwStatus == LOC_NW_OOO){
                LocAidlCellOooDataItem di;
                di.status = cellInfo->nwStatus;
                di.mask = cellInfo->valid_mask;
                iface->cellOooUpdate(di);
            }
            break;
        }
        case SERVICESTATUS_DATA_ITEM_ID:
        {
            LocAidlServiceStateDataItem di;
            di.status = (static_cast<ServiceStatusDataItem*>(dataItem))->mServiceState;
            iface->serviceStateUpdate(di);
            break;
        }
        case SCREEN_STATE_DATA_ITEM_ID:
        {
            LocAidlScreenStatusDataItem di;
            di.status = (static_cast<ScreenStateDataItem*>(dataItem))->mState;
            iface->screenStatusUpdate(di);
            break;
        }
        case TIMEZONE_CHANGE_DATA_ITEM_ID:
        {
            LocAidlTimeZoneChangeDataItem di;
            di.curTimeMillis = (static_cast<TimeZoneChangeDataItem*>(dataItem))->mCurrTimeMillis;
            di.rawOffset = (static_cast<TimeZoneChangeDataItem*>(dataItem))->mRawOffsetTZ;
            di.dstOffset = (static_cast<TimeZoneChangeDataItem*>(dataItem))->mDstOffsetTZ;
            iface->timezoneChangeUpdate(di);
            break;
        }
        case TIME_CHANGE_DATA_ITEM_ID:
        {
            LocAidlTimeChangeDataItem di;
            di.curTimeMillis = (static_cast<TimeChangeDataItem*>(dataItem))->mCurrTimeMillis;
            di.rawOffset = (static_cast<TimeChangeDataItem*>(dataItem))->mRawOffsetTZ;
            di.dstOffset = (static_cast<TimeChangeDataItem*>(dataItem))->mDstOffsetTZ;
            iface->timeChangeUpdate(di);
            break;
        }
        case SHUTDOWN_STATE_DATA_ITEM_ID:
        {
            iface->shutdownUpdate();
            break;
        }
        case WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID:
        {
            LocAidlWifiSupplicantStatusDataItem di;
            WifiSupplicantStatusDataItem* wifiDi =
                    static_cast<WifiSupplicantStatusDataItem*>(dataItem);
            di.state = wifiDi->mState;
            di.apMacAddressValid = wifiDi->mApMacAddressValid;
            di.apSsidValid = wifiDi->mWifiApSsidValid;
            di.apSsid = wifiDi->mWifiApSsid;
            std::vector<uint8_t> vec;
            for (int i = 0; i < MAC_ADDRESS_LENGTH; i++) {
                vec.push_back(wifiDi->mApMacAddress[i]);
            }
            di.apMacAddress = vec;
            iface->wifiSupplicantStatusUpdate(di);
            break;
        }
        default: break;
    }
}

void GfwAidlApi::dataItemsUpdate(std::unordered_set<IDataItemCore *> & dset) {
    std::vector<LocAidlBoolDataItem> dataItemArray;
    auto iface = getLocAidlIzatSubscriptionIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlIzatSubscription interface NULL!");
        return;
    }
    for (auto it = dset.begin(); it != dset.end(); it++) {
        LocAidlBoolDataItem di;
        IDataItemCore* dataItem = *it;
        DataItemId id = dataItem->getId();
        di.id = id;
        di.enabled = false;
        switch (id) {
            case ENH_DATA_ITEM_ID:
            {
                di.enabled = (static_cast<ENHDataItem*>(dataItem))->isEnabled();
                break;
            }
            case AIRPLANEMODE_DATA_ITEM_ID:
            {
                di.enabled = (static_cast<AirplaneModeDataItem*>(dataItem))->mMode;
                break;
            }
            case GPSSTATE_DATA_ITEM_ID:
            {
                di.enabled = (static_cast<GPSStateDataItem*>(dataItem))->mEnabled;
                break;
            }
            case NLPSTATUS_DATA_ITEM_ID:
            {
                di.enabled = (static_cast<NLPStatusDataItem*>(dataItem))->mEnabled;
                break;
            }
            case WIFIHARDWARESTATE_DATA_ITEM_ID:
            {
                di.enabled = (static_cast<WifiHardwareStateDataItem*>(dataItem))->mEnabled;
                break;
            }
            case ASSISTED_GPS_DATA_ITEM_ID:
            {
                di.enabled = (static_cast<AssistedGpsDataItem*>(dataItem))->mEnabled;
                break;
            }
            default: break;
        }
        dataItemArray.push_back(di);
    }
    if (dataItemArray.size() != 0) {
        iface->boolDataItemUpdate(dataItemArray);
    }
}

void GfwAidlApi::izatSubscriptionCleanUp() {
    auto iface = getLocAidlIzatSubscriptionIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlIzatSubscription interface NULL!");
        return;
    }
    iface->deinit();
}

void GfwAidlApi::rilInfoMonitorInit() {
    auto iface = getLocAidlRilInfoMonitorIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlRilInfoMonitor interface NULL!");
        return;
    }
    iface->init();
}

void GfwAidlApi::cinfoInject(int32_t cid, int32_t lac, int32_t mnc, int32_t mcc, bool roaming) {
    auto iface = getLocAidlRilInfoMonitorIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlRilInfoMonitor interface NULL!");
        return;
    }
    iface->cinfoInject(cid, lac, mnc, mcc, roaming);
}

void GfwAidlApi::oosInform() {
    auto iface = getLocAidlRilInfoMonitorIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlRilInfoMonitor interface NULL!");
        return;
    }
    iface->oosInform();
}

void GfwAidlApi::niSuplInit(std::string& str) {
    auto iface = getLocAidlRilInfoMonitorIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlRilInfoMonitor interface NULL!");
        return;
    }
    iface->niSuplInit(str.c_str());
}

void GfwAidlApi::chargerStatusInject(int32_t status) {
    auto iface = getLocAidlRilInfoMonitorIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlRilInfoMonitor interface NULL!");
        return;
    }
    iface->chargerStatusInject(status);
}

bool GfwAidlApi::onEnable() {
    auto iface = getLocAidlIzatProviderIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlIzatProvider interface NULL!");
        return false;
    }
    bool ret;
    iface->onEnable(&ret);
    return true;
}

bool GfwAidlApi::onDisable() {
    auto iface = getLocAidlIzatProviderIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlIzatProvider interface NULL!");
        return false;
    }
    bool ret;
    iface->onDisable(&ret);
    return true;
}

bool GfwAidlApi::onAddRequest(int providerType,
        int32_t numFixes, int32_t tbf, float displacement, int accuracy) {
    auto iface = getLocAidlIzatProviderIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlIzatProvider interface NULL!");
        return false;
    }
    LocAidlIzatRequest request;
    request.provider = LocAidlIzatStreamType::NETWORK;
    request.numUpdates = numFixes;
    request.timeIntervalBetweenFixes = tbf;
    request.smallestDistanceBetweenFixes = displacement;
    request.suggestedHorizontalAccuracy =
            static_cast<LocAidlIzatHorizontalAccuracy>(accuracy);
    bool ret;
    iface->onAddRequest(request, &ret);
    return true;
}
bool GfwAidlApi::onRemoveRequest(int providerType,
        int32_t numFixes, int32_t tbf, float displacement, int accuracy) {
    auto iface = getLocAidlIzatProviderIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlIzatProvider interface NULL!");
        return false;
    }
    LocAidlIzatRequest request;
    request.provider = LocAidlIzatStreamType::NETWORK;
    request.numUpdates = numFixes;
    request.timeIntervalBetweenFixes = tbf;
    request.smallestDistanceBetweenFixes = displacement;
    request.suggestedHorizontalAccuracy =
            static_cast<LocAidlIzatHorizontalAccuracy>(accuracy);
    bool ret;
    iface->onRemoveRequest(request, &ret);

    return true;
}

void GfwAidlApi::OsNpLocationChanged(Location& location) {
    ALOGE("API deprecated!");
}

void GfwAidlApi::xtInit() {
    ALOGE("API deprecated!");
}

void GfwAidlApi::setUserPref(bool userPref) {
    ALOGE("API deprecated!");
}

void GfwAidlApi::getGnssSvTypeConfig() {
    auto iface = getLocAidlGnssConfigIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlGnssConfig interface NULL!");
        return;
    }
    iface->getGnssSvTypeConfig();
}

void GfwAidlApi::setGnssSvTypeConfig(std::vector<uint8_t> svTypeVec) {
    auto iface = getLocAidlGnssConfigIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlGnssConfig interface NULL!");
        return;
    }
    std::vector<LocAidlGnssConstellationType> disabledSvTypeVec;
    for (int i = 0; i < svTypeVec.size(); i++) {
        disabledSvTypeVec.push_back(intToGnssConstellation(svTypeVec[i]));
    }
    iface->setGnssSvTypeConfig(disabledSvTypeVec);
}

void GfwAidlApi::resetGnssSvTypeConfig() {
    auto iface = getLocAidlGnssConfigIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlGnssConfig interface NULL!");
        return;
    }
    iface->resetGnssSvTypeConfig();
}

void GfwAidlApi::getRobustLocationConfig() {
    auto iface = getLocAidlGnssConfigIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlGnssConfig interface NULL!");
        return;
    }
    iface->getRobustLocationConfig();
}

void GfwAidlApi::setRobustLocationConfig(bool enable, bool enableForE911) {
    auto iface = getLocAidlGnssConfigIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlGnssConfig interface NULL!");
        return;
    }
    iface->setRobustLocationConfig(enable, enableForE911);
}

bool GfwAidlApi::readIzatConfig() {
    auto iface = getLocAidlIzatConfigIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlIzatConfig interface NULL!");
        return false;
    }
    bool ret;
    iface->readConfig(&ret);
    return true;
}

void GfwAidlApi::dataConnOpenExt(uint8_t agpsType, std::string apn, int bearerType) {
    auto iface = getLocAidlAgnssIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlAGnss interface NULL!");
        return;
    }
    LocAidlApnType type;
    if (bearerType == 0) {
        type = LocAidlApnType::IPV4;
    } else if (bearerType == 1) {
        type = LocAidlApnType::IPV4V6;
    } else {
        type = LocAidlApnType::INVALID;
    }
    bool ret;
    iface->dataConnOpenExt(apn, type, static_cast<LocAidlAGnssType>(agpsType), &ret);
}

void GfwAidlApi::dataConnClosedExt(uint8_t agpsType) {
    auto iface = getLocAidlAgnssIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlAGnss interface NULL!");
        return;
    }
    bool ret;
    iface->dataConnClosedExt(static_cast<LocAidlAGnssType>(agpsType), &ret);
}

void GfwAidlApi::dataConnFailedExt(uint8_t agpsType) {
    auto iface = getLocAidlAgnssIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlAGnss interface NULL!");
        return;
    }
    bool ret;
    iface->dataConnFailedExt(static_cast<LocAidlAGnssType>(agpsType), &ret);
}

void GfwAidlApi::getDebugReport(int maxReports, loc_core::SystemStatusReports& systemReports) {
    auto iface = getLocAidlDebugReportServiceIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlDebugReportService interface NULL!");
        return;
    }
    bool status;
    iface->init(&status);
    if (!status) {
        ALOGE("Failed to init ILocAidlDebugReportService interface!!!");
        return;
    }
    LocAidlSystemStatusReports aidlReports = {};
    iface->getReport(maxReports, &aidlReports);

    populateTimeAndClockReport(aidlReports, systemReports, maxReports);
    populateXoStateReport(aidlReports, systemReports, maxReports);
    populateRfParamsReport(aidlReports, systemReports, maxReports);
    populateErrRecoveryReport(aidlReports, systemReports, maxReports);
    populateInjectedPositionReport(aidlReports, systemReports, maxReports);
    populateBestPositionReport(aidlReports, systemReports, maxReports);
    populateXtraReport(aidlReports, systemReports, maxReports);
    populateEphemerisReport(aidlReports, systemReports, maxReports);
    populateSvHealthReport(aidlReports, systemReports, maxReports);
    populatePdrReport(aidlReports, systemReports, maxReports);
    populateNavDataReport(aidlReports, systemReports, maxReports);
    populatePositionFailureReport(aidlReports, systemReports, maxReports);
}

void GfwAidlApi::unregisterWiFiDBUpdater() {
    {
        lock_guard<std::mutex> guard(sWiFiDBReceiverLock);
        sWifiDBReceiverCbs = nullptr;
    }
    auto iface = getLocAidlWiFiDBReceiverIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlWiFiDBReceiver interface NULL!");
        return;
    }
    iface->unregisterWiFiDBUpdater();
}

void GfwAidlApi::sendAPListRequest(int32_t expireInDays) {
    auto iface = getLocAidlWiFiDBReceiverIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlWiFiDBReceiver interface NULL!");
        return;
    }
    iface->sendAPListRequest(expireInDays);
}

void GfwAidlApi::pushWiFiDB(
        std::vector<izat_remote_api::WiFiDBUpdater::APLocationData> apLocationData,
        std::vector<izat_remote_api::WiFiDBUpdater::APSpecialInfo> apSpecialInfo,
        int32_t daysValid, bool isLookup) {
    auto iface = getLocAidlWiFiDBReceiverIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlWiFiDBReceiver interface NULL!");
        return;
    }
    std::vector<LocAidlApLocationData> apLocList;
    std::vector<LocAidlApSpecialInfo> splList;
    int16_t apLocationDataListSize = apLocationData.size();
    int16_t apSpecialInfoListSize = apSpecialInfo.size();

    for (int i = 0; i < apLocationDataListSize; i++) {
        LocAidlApLocationData apLocData;
        apLocData.mac_R48b = apLocationData[i].mac_R48b;
        apLocData.latitude = apLocationData[i].latitude;
        apLocData.longitude = apLocationData[i].longitude;
        apLocData.max_antenna_range = apLocationData[i].max_antena_range;
        apLocData.horizontal_error = apLocationData[i].horizontal_error;
        apLocData.reliability = apLocationData[i].reliability;
        apLocData.valid_bits = (int8_t)apLocationData[i].valid_mask;
        apLocList.push_back(apLocData);
    }

    for (int i = 0; i < apSpecialInfoListSize; i++) {
        LocAidlApSpecialInfo apInfo;
        apInfo.mac_R48b = apSpecialInfo[i].mac_R48b;
        apInfo.info = apSpecialInfo[i].info;
        splList.push_back(apInfo);
    }

    iface->pushAPWiFiDB(apLocList, apLocationDataListSize, splList, apSpecialInfoListSize,
            daysValid, isLookup);
}

void GfwAidlApi::unregisterWWANDBUpdater() {
    {
        lock_guard<std::mutex> guard(sWwanDBReceiverLock);
        sWwanDBReceiverCbs = nullptr;
    }
    auto iface = getLocAidlWwanDBReceiverIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlWwanDBReceiver interface NULL!");
        return;
    }
    iface->unregisterWWANDBUpdater();
}

void GfwAidlApi::sendBSListRequest(int32_t expireInDays) {
    auto iface = getLocAidlWwanDBReceiverIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlWwanDBReceiver interface NULL!");
        return;
    }
    iface->sendBSListRequest(expireInDays);
}

void GfwAidlApi::pushBSWWANDB(
        std::vector<BSLocationData_s> bsLocationDataList,
        std::vector<BSSpecialInfo_s> bsSpecialInfoList,
        int32_t daysValid) {
    auto iface = getLocAidlWwanDBReceiverIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlWwanDBReceiver interface NULL!");
        return;
    }
    std::vector<LocAidlBsLocationData> bsLocList;
    std::vector<LocAidlBsSpecialInfo> splList;
    int16_t bsLocationDataListSize = bsLocationDataList.size();
    int16_t bsSpecialInfoListSize = bsSpecialInfoList.size();

    for (int i = 0; i < bsLocationDataListSize; i++) {
        LocAidlBsLocationData bsLocData;
        bsLocData.cellType = bsLocationDataList[i].cellType;
        bsLocData.cellRegionID1 = bsLocationDataList[i].cellRegionID1;
        bsLocData.cellRegionID2 = bsLocationDataList[i].cellRegionID2;
        bsLocData.cellRegionID3 = bsLocationDataList[i].cellRegionID3;
        bsLocData.cellRegionID4 = bsLocationDataList[i].cellRegionID4;
        bsLocData.latitude = bsLocationDataList[i].latitude;
        bsLocData.longitude = bsLocationDataList[i].longitude;
        bsLocData.valid_bits = bsLocationDataList[i].valid_bits;
        bsLocData.horizontal_coverage_radius = bsLocationDataList[i].horizontal_coverage_radius;
        bsLocData.horizontal_confidence = bsLocationDataList[i].horizontal_confidence;
        bsLocData.horizontal_reliability = bsLocationDataList[i].horizontal_reliability;
        bsLocData.altitude = bsLocationDataList[i].altitude;
        bsLocData.altitude_uncertainty = bsLocationDataList[i].altitude_uncertainty;
        bsLocData.altitude_confidence = bsLocationDataList[i].altitude_confidence;
        bsLocData.altitude_reliability = bsLocationDataList[i].altitude_reliability;
        bsLocList.push_back(bsLocData);
    }

    for (int i = 0; i < bsSpecialInfoListSize; i++) {
        LocAidlBsSpecialInfo bsInfo;
        bsInfo.cellType = bsSpecialInfoList[i].cellType;
        bsInfo.cellRegionID1 = bsSpecialInfoList[i].cellRegionID1;
        bsInfo.cellRegionID2 = bsSpecialInfoList[i].cellRegionID2;
        bsInfo.cellRegionID3 = bsSpecialInfoList[i].cellRegionID3;
        bsInfo.cellRegionID4 = bsSpecialInfoList[i].cellRegionID4;
        bsInfo.info = bsSpecialInfoList[i].info;
        splList.push_back(bsInfo);
    }

    iface->pushBSWWANDB(bsLocList, bsLocationDataListSize, splList, bsSpecialInfoListSize,
            daysValid);
}

void GfwAidlApi::unregisterWiFiDBProvider() {
    {
        lock_guard<std::mutex> guard(sWiFiDBProviderLock);
        sWifiDBProviderCbs = nullptr;
    }
    auto iface = getLocAidlWiFiDBProviderIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlWiFiDBProvider interface NULL!");
        return;
    }
    iface->unregisterWiFiDBProvider();
}

void GfwAidlApi::sendAPObsLocDataRequest() {
    auto iface = getLocAidlWiFiDBProviderIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlWiFiDBProvider interface NULL!");
        return;
    }
    iface->sendAPObsLocDataRequest();
}

void GfwAidlApi::unregisterWWANDBProvider() {
    {
        lock_guard<std::mutex> guard(sWwanDBProviderLock);
        sWwanDBProviderCbs = nullptr;
    }
    auto iface = getLocAidlWwanDBProviderIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlWwanDBProvider interface NULL!");
        return;
    }
    iface->unregisterWWANDBProvider();
}

void GfwAidlApi::sendBSObsLocDataRequest() {
    auto iface = getLocAidlWwanDBProviderIface();
    if (iface == nullptr) {
        ALOGE("ILocAidlWwanDBProvider interface NULL!");
        return;
    }
    iface->sendBSObsLocDataRequest();
}

} // namespace garden
