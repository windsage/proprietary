/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat Types

  Copyright (c) 2015 - 2016, 2021-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IZATTYPES_H__
#define __IZAT_MANAGER_IZATTYPES_H__

#include <string>

namespace izat_manager {

/**
 * Bit Mask for location listeners.
 */
typedef unsigned short int IzatListenerMask;

/**
 * Enum of supported streams.
 * Each elements of enum represent a bit mask value.
 * Do not add values with multiple bits set.
 *
 * Note of caution:
 * If new value is being added to this enum (after 0x4),
 * update the logic in onSetRequestJNI() method to derive
 * providerNameLookUpTable array access index.
 */
enum IzatStreamType {

    /**
     * Fused location stream.
     */
    IZAT_STREAM_FUSED = 0x1,


    /**
     * GNSS location stream.
     */
    IZAT_STREAM_GNSS  = 0x4,

    /**
     * GNSS NMEA stream.
     */
    IZAT_STREAM_NMEA  = 0x8,

    /**
     * GNSS Raw location stream.
     */
    IZAT_STREAM_DR  = 0x10,

    /**
     * GNSS SV Info stream.
     */
    IZAT_STREAM_GNSS_SVINFO  = 0x20,

    /**
     * GNSS Raw SV Info stream.
     */
    IZAT_STREAM_DR_SVINFO  = 0x40,

    /**
     * EMERGENCY STATUS.
     */
    IZAT_STREAM_EMERGENCY_STATUS  = 0x80,

    /**
     * OPTIN STATUS
     */
    IZAT_STREAM_OPTIN_STATUS = 0x100,
    /**
     * GTP Wifi RTT Network location stream.
     */
    IZAT_STREAM_NETWORK_PRECISE = 0x200,
    /**
     * GTP Wifi RSSI/GTP Cell Network/ZPP location stream.
     */
    IZAT_STREAM_NETWORK_NOMINAL = 0x400,

    /**
     * WWAN positioning session need to request network stream.
     */
    IZAT_STREAM_REQUEST_NETWORK = 0x800,

    /**
     * All Network location stream, like GTP Wifi RTT/RSSI, GTP Cell
     */
    IZAT_STREAM_NETWORK = IZAT_STREAM_NETWORK_PRECISE | IZAT_STREAM_NETWORK_NOMINAL,
    IZAT_STREAM_ALL = (IZAT_STREAM_OPTIN_STATUS << 1) - 1

};

enum IzatLocationStatus {
    /**
     * Final location
     */
    IZAT_LOCATION_STATUS_FINAL = 0x0,

    /**
      * Intermediate location
      */
    IZAT_LOCATION_STATUS_INTERMEDIATE = 0x1
};

/**
 * Izat Provider status Enumeration
 */
enum IzatProviderStatus {

    /**
     * Out of service.
     */
    IZAT_PROVIDER_OUT_OF_SERVICE = 0,

    /**
     * Temporarily unavailable.
     */
    IZAT_PROVIDER_TEMPORARILY_UNAVAILABLE,

    /**
     * Available.
     */
    IZAT_PROVIDER_AVAILABLE,

    /**
     * GNSS status none
     */
    IZAT_PROVIDER_GNSS_STATUS_NONE,

    /**
     * GNSS Session BEGIN
     */
    IZAT_PROVIDER_GNSS_STATUS_SESSION_BEGIN,

    /**
     * GNSS Session END
     */
    IZAT_PROVIDER_GNSS_STATUS_SESSION_END,

    /**
     * GNSS Engine ON
     */
    IZAT_PROVIDER_GNSS_STATUS_ENGINE_ON,

    /**
     * GNSS Engine OFF
     */
    IZAT_PROVIDER_GNSS_STATUS_ENGINE_OFF,

    /**
     * Emergency mode
     */
    IZAT_PROVIDER_EMERGENCY_MODE,

    /**
     * None Emergency mode
     */
    IZAT_PROVIDER_NONE_EMERGENCY_MODE,

    /**
     * Optin enabled
     */
    IZAT_PROVIDER_OPTIN_ENABLED,

    /**
     * Optin disabled
     */
    IZAT_PROVIDER_OPTIN_DISABLED,
};

/**
 * Enumeration of Horizontal accuracy to be used for specifying accuracy of Latitude and Longitudes
 */
enum IzatHorizontalAccuracy {

    /**
     * High power consumption.
     */
    IZAT_HORIZONTAL_FINE = 0,

    /**
     * Medium power consumption.
     */
    IZAT_HORIZONTAL_BLOCK,

    /**
     * Low power consumption.
     */
    IZAT_HORIZONTAL_CITY,

    /**
     * Zero power consumption.
     */
    IZAT_HORIZONTAL_NONE

};

/**
 * Enumeration of accuracy to be used for specifying accuracy of Bearing and or Altitude.
 */
enum IzatOtherAccuracy {

    /**
     * High accuracy.
     */
    IZAT_OTHER_HIGH = 0,

    /**
     * Low accuracy.
     */
    IZAT_OTHER_LOW,

    /**
     * No Accuracy requirement.
     */
    IZAT_OTHER_NONE

};

enum IzatNetworkPositionSourceType {
    /**
     * Network position is from Cell-Tower
     */
    IZAT_NETWORK_POSITION_FROM_CELL = 0,

    /**
     *  Network position is from WIFI Access Points
     */
    IZAT_NETWORK_POSITION_FROM_WIFI_RSSI = 1,
    /**
     *  Network position is from GTP WIFI RTT with TPS_SERVER AP source
     */
    IZAT_NETWORK_POSITION_FROM_WIFI_RTT_SERVER = 2,
    /**
     *  Network position is from GTP WIFI RTT with FTM AP source
     */
    IZAT_NETWORK_POSITION_FROM_WIFI_RTT_FTM = 3
};

enum IzatAltitudeRefType {

    /**
     * Altitude type of the AP is unknown
     */
    ALT_UNKNOWN = 0,

    /**
     * Altitude type of the AP is of World Geodetic System (WGS84)
     */
    ALT_REF_WGS84,

    /**
     * Altitude type of the AP is in meters above mean sea level
     */
    ALT_REF_MSL,

    /**
     * Altitude type of the AP is in meters above ground level
     */
    ALT_REF_AGL,

    /**
     * Altitude type of the AP is in a floor level value
     */
    ALT_REF_FLOOR_LEVEL
};

enum IzatRequestType {
    IZAT_WIFI_BASIC,
    IZAT_WIFI_PREMIUM,
    IZAT_WWAN_BASIC,
    IZAT_WWAN_PREMIUM,
    IZAT_WWAN_LEGACY
};

enum IzatDataConnStatus {
    IZAT_DATA_CONN_OPEN,
    IZAT_DATA_CONN_CLOSE,
    IZAT_DATA_CONN_FAILED
};

enum IzatAGpsType {
    IZAT_AGPS_TYPE_INVALID,
    IZAT_AGPS_TYPE_ANY,
    IZAT_AGPS_TYPE_SUPL,
    IZAT_AGPS_TYPE_C2K,
    IZAT_AGPS_TYPE_WWAN_ANY,
    IZAT_AGPS_TYPE_WIFI,
    IZAT_AGPS_TYPE_SUPL_ES
};

enum IzatAGpsBearerType {
    IZAT_APN_BEARER_INVALID,
    IZAT_APN_BEARER_IPV4,
    IZAT_APN_BEARER_IPV6,
    IZAT_APN_BEARER_IPV4V6
};

typedef struct {
    IzatDataConnStatus status;
    IzatAGpsType type;
    IzatAGpsBearerType bearerType;
    std::string apn;
} IzatNetworkStatus;

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATTYPES_H__
