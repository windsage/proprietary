/******************************************************************************
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  
  Copyright (c) 2019-2020 The Linux Foundation. All rights reserved
 *******************************************************************************/

#ifndef LOCATION_CLIENT_API_DIAG
#define LOCATION_CLIENT_API_DIAG

#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <inttypes.h>
#include <errno.h>
#include <comdef.h>
#include <log.h>
#include <log_codes.h>
#include <LocDiagIface.h>
#include <LocationClientApi.h>
#include <log_util.h>
#include <loc_misc_utils.h>
#include <LocLoggerBase.h>
#include <LocationDataTypes.h>

#define CLIENT_DIAG_GNSS_SV_MAX            (176)
#define CLIENT_DIAG_GNSS_MEASUREMENTS_MAX  (128)
#define CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ  (100)
#define CLIENT_DIAG_DGNSS_STATION_ID_MAX    (3)
#define CLIENT_DIAG_GEOFENCE_MAX           (200)
/**  Maximum number of satellites in an ephemeris report.  */
#define CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE (32)

#define LOG_CLIENT_LOCATION_DIAG_MSG_VERSION        (16)
#define LOG_CLIENT_SV_REPORT_DIAG_MSG_VERSION       (7)
#define LOG_CLIENT_NMEA_REPORT_DIAG_MSG_VERSION     (3)
#define LOG_CLIENT_MEASUREMENTS_DIAG_MSG_VERSION    (8)
#define LOG_CLIENT_LATENCY_DIAG_MSG_VERSION         (2)
#define LOG_CLIENT_DC_REPORT_DIAG_MSG_VERSION       (1)
#define LOG_CLIENT_GEOFENCE_DIAG_MSG_VERSION        (1)
#define LOG_CLIENT_EPH_DIAG_MSG_VERSION             (0)

using namespace loc_util;

namespace location_client {

/** Flags to indicate which values are valid in a Location */
typedef uint16_t clientDiagLocationFlagsMask;
typedef enum {
    /** location has valid latitude and longitude */
    CLIENT_DIAG_LOCATION_HAS_LAT_LONG_BIT          = (1<<0),
    /** location has valid altitude */
    CLIENT_DIAG_LOCATION_HAS_ALTITUDE_BIT          = (1<<1),
    /** location has valid speed */
    CLIENT_DIAG_LOCATION_HAS_SPEED_BIT             = (1<<2),
    /** location has valid bearing */
    CLIENT_DIAG_LOCATION_HAS_BEARING_BIT           = (1<<3),
    /** location has valid accuracy */
    CLIENT_DIAG_LOCATION_HAS_ACCURACY_BIT          = (1<<4),
    /** location has valid vertical accuracy */
    CLIENT_DIAG_LOCATION_HAS_VERTICAL_ACCURACY_BIT = (1<<5),
    /** location has valid speed accuracy */
    CLIENT_DIAG_LOCATION_HAS_SPEED_ACCURACY_BIT    = (1<<6),
    /** location has valid bearing accuracy */
    CLIENT_DIAG_LOCATION_HAS_BEARING_ACCURACY_BIT  = (1<<7),
    /** location has valid valid timestamp  */
    CLIENT_DIAG_LOCATION_HAS_TIMESTAMP_BIT         = (1<<8),
    /** Location has valid Location::elapsedRealTime. <br/>   */
    CLIENT_DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_BIT = (1<<9),
    /** Location has valid Location::elapsedRealTimeUnc. <br/>   */
    CLIENT_DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_UNC_BIT = (1<<10),
    /** Location has valid Location::timeUncMs. <br/>   */
    CLIENT_DIAG_LOCATION_HAS_TIME_UNC_BIT = (1<<11),
    /** GnssLocation has valid Location::elapsedgPTPTime. <br/> */
    CLIENT_DIAG_LOCATION_HAS_GPTP_TIME_BIT         = (1<<12),
    /** GnssLocation has valid Location::elapsedgPTPTimeUnc. <br/> */
    CLIENT_DIAG_LOCATION_HAS_GPTP_TIME_UNC_BIT     = (1<<13),
} clientDiagLocationFlagsBits;

typedef uint32_t clientDiagGnssLocationPosDataMask;
typedef enum {
    /** Navigation data has Forward Acceleration */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT  = (1<<0),
    /** Navigation data has Sideward Acceleration */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT   = (1<<1),
    /** Navigation data has Vertical Acceleration */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT  = (1<<2),
    /** Navigation data has Heading Rate */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_RATE_BIT    = (1<<3),
    /** Navigation data has Body pitch */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_BIT       = (1<<4),
    /** Navigation data has Forward Acceleration uncertainty */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT = (1<<5),
    /** Navigation data has Sideward Acceleration uncertainty */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT  = (1<<6),
    /** Navigation data has Vertical Acceleration uncertainty */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT = (1<<7),
    /** Navigation data has Heading Rate uncertainty */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT   = (1<<8),
    /** Navigation data has Body pitch uncertainty */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT      = (1<<9),
    /** Navigation data has pitch rate */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT     = (1<<10),
    /** Navigation data has body pitch rate uncertainty */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT = (1<<11),
    /** Navigation data has body roll */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_BIT           = (1<<12),
    /** Navigation data has body roll uncertainty */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT       = (1<<13),
    /** Navigation data has body rate roll */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT      = (1<<14),
    /** Navigation data has body roll rate uncertainty */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT  = (1<<15),
    /** Navigation data has body yaw */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_BIT            = (1<<16),
    /** Navigation data has body roll uncertainty */
    CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_UNC_BIT        = (1<<17)
} clientDiagGnssLocationPosDataBits;

typedef enum {
    /** GPS satellite. */
    CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GPS                    = 1,
    /** GALILEO satellite. */
    CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GALILEO                = 2,
    /** SBAS satellite. */
    CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_SBAS                   = 3,
    //Leave 4 blank for backward compatibility.
    /** GLONASS satellite. */
    CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GLONASS                = 5,
    /** BDS satellite. */
    CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_BDS                    = 6,
    /** QZSS satellite. */
    CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_QZSS                   = 7,
    /** NAVIC satellite. */
    CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_NAVIC                  = 8
} clientDiagGnss_LocSvSystemEnumType;

typedef uint32_t clientDiagGnssSystemTimeStructTypeFlags;
typedef enum {
    CLIENT_DIAG_GNSS_SYSTEM_TIME_WEEK_VALID             = (1 << 0),
    CLIENT_DIAG_GNSS_SYSTEM_TIME_WEEK_MS_VALID          = (1 << 1),
    CLIENT_DIAG_GNSS_SYSTEM_CLK_TIME_BIAS_VALID         = (1 << 2),
    CLIENT_DIAG_GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID     = (1 << 3),
    CLIENT_DIAG_GNSS_SYSTEM_REF_FCOUNT_VALID            = (1 << 4),
    CLIENT_DIAG_GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID      = (1 << 5)
} clientDiagGnssSystemTimeTypeBits;

typedef uint32_t clientDiagGnssGloTimeStructTypeFlags;
typedef enum {
    CLIENT_DIAG_GNSS_CLO_DAYS_VALID                     = (1 << 0),
    CLIENT_DIAG_GNSS_GLOS_MSEC_VALID                    = (1 << 1),
    CLIENT_DIAG_GNSS_GLO_CLK_TIME_BIAS_VALID            = (1 << 2),
    CLIENT_DIAG_GNSS_GLO_CLK_TIME_BIAS_UNC_VALID        = (1 << 3),
    CLIENT_DIAG_GNSS_GLO_REF_FCOUNT_VALID               = (1 << 4),
    CLIENT_DIAG_GNSS_GLO_NUM_CLOCK_RESETS_VALID         = (1 << 5),
    CLIENT_DIAG_GNSS_GLO_FOUR_YEAR_VALID                = (1 << 6)
} clientDiagGnssGloTimeTypeBits;

typedef PACKED struct PACKED_POST {
    /** Validity mask for below fields */
    clientDiagGnssSystemTimeStructTypeFlags validityMask;
    /** Extended week number at reference tick.
    Unit: Week.
    Set to 65535 if week number is unknown.
    For GPS:
      Calculated from midnight, Jan. 6, 1980.
      OTA decoded 10 bit GPS week is extended to map between:
      [NV6264 to (NV6264 + 1023)].
      NV6264: Minimum GPS week number configuration.
      Default value of NV6264: 1738
    For BDS:
      Calculated from 00:00:00 on January 1, 2006 of Coordinated Universal Time (UTC).
    For GAL:
      Calculated from 00:00 UT on Sunday August 22, 1999 (midnight between August 21 and August 22).
   */
    uint16_t systemWeek;
    /** Time in to the current week at reference tick.
       Unit: Millisecond. Range: 0 to 604799999.
       Check for systemClkTimeUncMs before use */
    uint32_t systemMsec;
    /** System clock time bias (sub-millisecond)
        Units: Millisecond
        Note: System time (TOW Millisecond) = systemMsec - systemClkTimeBias.
        Check for systemClkTimeUncMs before use. */
    float systemClkTimeBias;
    /** Single sided maximum time bias uncertainty
        Units: Millisecond */
    float systemClkTimeUncMs;
    /** FCount (free running HW timer) value. Don't use for relative time purpose
         due to possible discontinuities.
         Unit: Millisecond */
    uint32_t refFCount;
    /** Number of clock resets/discontinuities detected,
        affecting the local hardware counter value. */
    uint32_t numClockResets;
} clientDiagGnssSystemTimeStructType;

typedef PACKED struct PACKED_POST {
    /** GLONASS day number in four years. Refer to GLONASS ICD.
        Applicable only for GLONASS and shall be ignored for other constellations.
        If unknown shall be set to 65535 */
    uint16_t gloDays;
    /** Validity mask for below fields */
    clientDiagGnssGloTimeStructTypeFlags validityMask;
    /** GLONASS time of day in Millisecond. Refer to GLONASS ICD.
        Units: Millisecond
        Check for gloClkTimeUncMs before use */
    uint32_t gloMsec;
    /** GLONASS clock time bias (sub-millisecond)
        Units: Millisecond
        Note: GLO time (TOD Millisecond) = gloMsec - gloClkTimeBias.
        Check for gloClkTimeUncMs before use. */
    float gloClkTimeBias;
    /** Single sided maximum time bias uncertainty
        Units: Millisecond */
    float gloClkTimeUncMs;
    /** FCount (free running HW timer) value. Don't use for relative time purpose
        due to possible discontinuities.
        Unit: Millisecond */
    uint32_t  refFCount;
    /** Number of clock resets/discontinuities detected,
        affecting the local hardware counter value. */
    uint32_t numClockResets;
    /** GLONASS four year number from 1996. Refer to GLONASS ICD.
        Applicable only for GLONASS and shall be ignored for other constellations.
        If unknown shall be set to 255 */
    uint8_t gloFourYear;
} clientDiagGnssGloTimeStructType;

typedef PACKED union PACKED_POST {
    clientDiagGnssSystemTimeStructType gpsSystemTime;
    clientDiagGnssSystemTimeStructType galSystemTime;
    clientDiagGnssSystemTimeStructType bdsSystemTime;
    clientDiagGnssSystemTimeStructType qzssSystemTime;
    clientDiagGnssGloTimeStructType gloSystemTime;
    clientDiagGnssSystemTimeStructType navicSystemTime;
} clientDiagSystemTimeStructUnion;

typedef PACKED struct PACKED_POST {
    /** Contains Body frame LocPosDataMask bits. */
    clientDiagGnssLocationPosDataMask  bodyFrameDataMask;
    /** Forward Acceleration in body frame (m/s2)*/
    float           longAccel;
    /** Sideward Acceleration in body frame (m/s2)*/
    float           latAccel;
    /** Vertical Acceleration in body frame (m/s2)*/
    float           vertAccel;
    /** Heading Rate (Radians/second) */
    float           yawRate;
    /** Body pitch (Radians) */
    float           pitch;
    /** Uncertainty of Forward Acceleration in body frame */
    float           longAccelUnc;
    /** Uncertainty of Side-ward Acceleration in body frame */
    float           latAccelUnc;
    /** Uncertainty of Vertical Acceleration in body frame */
    float           vertAccelUnc;
    /** Uncertainty of Heading Rate */
    float           yawRateUnc;
    /** Uncertainty of Body pitch */
    float           pitchUnc;
    /** Body pitch rate (Radians/second) */
    float pitchRate;
    /** Uncertainty of pitch rate (Radians/second) */
    float pitchRateUnc;
    /** Roll of body frame. Clockwise positive. (Radian) */
    float roll;
    /** Uncertainty of Roll, 68% confidence level (radian) */
    float rollUnc;
    /** Roll rate of body frame. Clockwise positive. (radian/second)
    */
    float rollRate;
    /** Uncertainty of Roll rate, 68% confidence level (radian/second) */
    float rollRateUnc;
    /** Yaw of body frame. Clockwise positive (radian) */
    float yaw;
    /** Uncertainty of Yaw, 68% confidence level (radian) */
    float yawUnc;
} clientDiagGnssLocationPositionDynamics;

/** @struct
    Time applicability of PVT report
*/
typedef PACKED struct PACKED_POST {
    /** Specifies GNSS system time reported. Mandatory field */
    clientDiagGnss_LocSvSystemEnumType gnssSystemTimeSrc;
    /** Reporting of GPS system time is recommended.
      If GPS time is unknown & other satellite system time is known,
      it should be reported.
      Mandatory field
     */
    clientDiagSystemTimeStructUnion u;
} clientDiagGnssSystemTime;

/** GNSS Signal Type and RF Band */
typedef uint32_t clientDiagGnssSignalTypeMask;
typedef enum {
    /** GPS L1CA Signal */
    CLIENT_DIAG_GNSS_SIGNAL_GPS_L1CA            = (1<<0),
    /** GPS L1C Signal */
    CLIENT_DIAG_GNSS_SIGNAL_GPS_L1C             = (1<<1),
    /** GPS L2 RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_GPS_L2              = (1<<2),
    /** GPS L5 RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_GPS_L5              = (1<<3),
    /** GLONASS G1 (L1OF) RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_GLONASS_G1          = (1<<4),
    /** GLONASS G2 (L2OF) RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_GLONASS_G2          = (1<<5),
    /** GALILEO E1 RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E1          = (1<<6),
    /** GALILEO E5A RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E5A         = (1<<7),
    /** GALILEO E5B RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E5B         = (1<<8),
    /** BEIDOU B1 RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1           = (1<<9),
    /** BEIDOU B2 RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2           = (1<<10),
    /** QZSS L1CA RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_QZSS_L1CA           = (1<<11),
    /** QZSS L1S RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_QZSS_L1S            = (1<<12),
    /** QZSS L2 RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_QZSS_L2             = (1<<13),
    /** QZSS L5 RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_QZSS_L5             = (1<<14),
    /** SBAS L1 RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_SBAS_L1             = (1<<15),
    /** BEIDOU B1I RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1I          = (1<<16),
    /** BEIDOU B1C RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1C          = (1<<17),
    /** BEIDOU B2I RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2I          = (1<<18),
    /** BEIDOU B2AI RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2AI         = (1<<19),
    /** NAVIC L5 RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_NAVIC_L5            = (1<<20),
    /** BEIDOU B2AQ RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2AQ         = (1<<21),
    /** BEIDOU B2BI RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2BI         = (1<<22),
    /** BEIDOU B2BQ RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2BQ         = (1<<23),
    /** NAVIC L1 RF Band */
    CLIENT_DIAG_GNSS_SIGNAL_NAVIC_L1           = (1<<24),
} clientDiagGnssSignalTypeBits;

typedef PACKED struct PACKED_POST {
    /** GnssSignalType mask */
    clientDiagGnssSignalTypeMask gnssSignalType;
   /** Specifies GNSS Constellation Type */
    clientDiagGnss_LocSvSystemEnumType gnssConstellation;
    /**  GNSS SV ID.
     For GPS:      1 to 32
     For GLONASS:  65 to 96. When slot-number to SV ID mapping is unknown, set as 255.
     For SBAS:     120 to 151
     For QZSS-L1CA:193 to 197
     For BDS:      201 to 263
     For GAL:      301 to 336
     For NAVIC:    401 to 414 */
    uint16_t gnssSvId;
} clientDiagGnssMeasUsageInfo;

typedef enum {
    CLIENT_DIAG_GNSS_SV_TYPE_UNKNOWN = 0,
    CLIENT_DIAG_GNSS_SV_TYPE_GPS,
    CLIENT_DIAG_GNSS_SV_TYPE_SBAS,
    CLIENT_DIAG_GNSS_SV_TYPE_GLONASS,
    CLIENT_DIAG_GNSS_SV_TYPE_QZSS,
    CLIENT_DIAG_GNSS_SV_TYPE_BEIDOU,
    CLIENT_DIAG_GNSS_SV_TYPE_GALILEO,
    CLIENT_DIAG_GNSS_SV_TYPE_NAVIC
} clientDiagGnssSvType;

typedef uint16_t clientDiagGnssSvOptionsMask;
typedef enum {
    CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_EPHEMER_BIT           = (1<<0),
    CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_ALMANAC_BIT           = (1<<1),
    CLIENT_DIAG_GNSS_SV_OPTIONS_USED_IN_FIX_BIT           = (1<<2),
    CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT = (1<<3),
    CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_GNSS_SIGNAL_TYPE_BIT  = (1<<4),
    CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_BASEBAND_CARRIER_TO_NOISE_BIT = (1<<5),
    CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_ELEVATION_BIT         = (1<<6),
    CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_AZIMUTH_BIT           = (1<<7),
} clientDiagGnssSvOptionsBits;

typedef PACKED struct PACKED_POST {
    /** Unique Identifier */
    uint16_t svId;
    /** type of SV (GPS, SBAS, GLONASS, QZSS, BEIDOU, GALILEO, NAVIC) */
    clientDiagGnssSvType type;
    /** signal strength */
    float cN0Dbhz;
    /** elevation of SV (in degrees) */
    float elevation;
    /** azimuth of SV (in degrees) */
    float azimuth;
    /** Bitwise OR of GnssSvOptionsBits */
    clientDiagGnssSvOptionsMask gnssSvOptionsMask;
    /** carrier frequency of the signal tracked */
    float carrierFrequencyHz;
    /** Specifies GNSS signal type */
    clientDiagGnssSignalTypeMask gnssSignalTypeMask;
    /** baseband signal strength */
    double basebandCarrierToNoiseDbHz;
    /** Glonass frequency number */
    uint16_t gloFrequency;
} clientDiagGnssSv;

typedef uint16_t clientDiagLocationTechnologyMask;
typedef enum {
    /** location was calculated using GNSS */
    CLIENT_DIAG_LOCATION_TECHNOLOGY_GNSS_BIT     = (1<<0),
    /** location was calculated using Cell */
    CLIENT_DIAG_LOCATION_TECHNOLOGY_CELL_BIT     = (1<<1),
    /** location was calculated using WiFi */
    CLIENT_DIAG_LOCATION_TECHNOLOGY_WIFI_BIT     = (1<<2),
    /** location was calculated using Sensors */
    CLIENT_DIAG_LOCATION_TECHNOLOGY_SENSORS_BIT  = (1<<3),
    /** location was calculated using reference station */
    CLIENT_DIAG_LOCATION_REFERENCE_LOCATION_BIT  = (1<<4),
    /** location was calculated using CPI */
    CLIENT_DIAG_LOCATION_INJECTED_COARSE_POSITION_BIT  = (1<<5),
    /** location was calculated using AFLT */
    CLIENT_DIAG_LOCATION_AFLT_BIT               = (1<<6),
    /** location was calculated using HYBRID */
    CLIENT_DIAG_LOCATION_HYBRID_BIT             = (1<<7),
    /** location was calculated using PPE */
    CLIENT_DIAG_LOCATION_PPE_BIT                = (1<<8),
    /** location was calculated using VEH */
    CLIENT_DIAG_LOCATION_VEH_BIT                = (1<<9),
    /** location was calculated using VIS */
    CLIENT_DIAG_LOCATION_VIS_BIT                = (1<<10),
    /** location was calculated using cached measurements */
    CLIENT_DIAG_LOCATION_PROPAGATED_BIT         = (1<<11)
} clientDiagLocationTechnologyBits;

typedef uint32_t clientDiagDrCalibrationStatusMask;
typedef enum {
    /** Indicate that roll calibration is needed. Need to take more
     *  turns on level ground */
    CLIENT_DIAG_DR_ROLL_CALIBRATION_NEEDED  = (1<<0),
    /** Indicate that pitch calibration is needed. Need to take more
     *  turns on level ground */
    CLIENT_DIAG_DR_PITCH_CALIBRATION_NEEDED = (1<<1),
    /** Indicate that yaw calibration is needed. Need to accelerate
     *  in a straight line  */
    CLIENT_DIAG_DR_YAW_CALIBRATION_NEEDED   = (1<<2),
    /** Indicate that odo calibration is needed. Need to accelerate
     *  in a straight line  */
    CLIENT_DIAG_DR_ODO_CALIBRATION_NEEDED   = (1<<3),
    /** Indicate that gyro calibration is needed. Need to take more
     *  turns on level ground */
    CLIENT_DIAG_DR_GYRO_CALIBRATION_NEEDED  = (1<<4),
    /** Lot more turns on level ground needed */
    CLIENT_DIAG_DR_TURN_CALIBRATION_LOW  = (1<<5),
    /*8 Some more turns on level ground needed */
    CLIENT_DIAG_DR_TURN_CALIBRATION_MEDIUM  = (1<<6),
    /** Sufficient turns on level ground observed */
    CLIENT_DIAG_DR_TURN_CALIBRATION_HIGH  = (1<<7),
    /** Lot more accelerations in straight line needed */
    CLIENT_DIAG_DR_LINEAR_ACCEL_CALIBRATION_LOW  = (1<<8),
    /** Some more accelerations in straight line needed */
    CLIENT_DIAG_DR_LINEAR_ACCEL_CALIBRATION_MEDIUM  = (1<<9),
    /** Sufficient acceleration events in straight line observed */
    CLIENT_DIAG_DR_LINEAR_ACCEL_CALIBRATION_HIGH  = (1<<10),
    /** Lot more motion in straight line needed */
    CLIENT_DIAG_DR_LINEAR_MOTION_CALIBRATION_LOW  = (1<<11),
    /** Some more motion in straight line needed */
    CLIENT_DIAG_DR_LINEAR_MOTION_CALIBRATION_MEDIUM  = (1<<12),
    /** Sufficient motion events in straight line observed */
    CLIENT_DIAG_DR_LINEAR_MOTION_CALIBRATION_HIGH  = (1<<13),
    /** Lot more stationary events on level ground needed */
    CLIENT_DIAG_DR_STATIC_CALIBRATION_LOW  = (1<<14),
    /** Some more stationary events on level ground needed */
    CLIENT_DIAG_DR_STATIC_CALIBRATION_MEDIUM  = (1<<15),
    /** Sufficient stationary events on level ground observed */
    CLIENT_DIAG_DR_STATIC_CALIBRATION_HIGH  = (1<<16)
} clientDiagDrCalibrationStatusBits;

typedef uint64_t clientDiagGnssLocationInfoFlagMask;
typedef enum {
    /** valid altitude mean sea level */
    CLIENT_DIAG_GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT      = (1<<0),
    /** valid pdop, hdop, and vdop */
    CLIENT_DIAG_GNSS_LOCATION_INFO_DOP_BIT                          = (1<<1),
    /** valid magnetic deviation */
    CLIENT_DIAG_GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT           = (1<<2),
    /** valid horizontal reliability */
    CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT              = (1<<3),
    /** valid vertical reliability */
    CLIENT_DIAG_GNSS_LOCATION_INFO_VER_RELIABILITY_BIT              = (1<<4),
    /** valid elipsode semi major */
    CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT = (1<<5),
    /** valid elipsode semi minor */
    CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MINOR_BIT = (1<<6),
    /** valid accuracy elipsode azimuth */
    CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT    = (1<<7),
    /** valid gnss sv used in pos data */
    CLIENT_DIAG_GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT            = (1<<8),
    /** valid navSolutionMask */
    CLIENT_DIAG_GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT            = (1<<9),
    /** valid LocPosTechMask */
    CLIENT_DIAG_GNSS_LOCATION_INFO_POS_TECH_MASK_BIT                = (1<<10),
    /** valid LocSvInfoSource */
    CLIENT_DIAG_GNSS_LOCATION_INFO_SV_SOURCE_INFO_BIT               = (1<<11),
    /** valid position dynamics data */
    CLIENT_DIAG_GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT            = (1<<12),
    /** valid gdop, tdop */
    CLIENT_DIAG_GNSS_LOCATION_INFO_EXT_DOP_BIT                      = (1<<13),
    /** valid North standard deviation */
    CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_STD_DEV_BIT                = (1<<14),
    /** valid East standard deviation */
    CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_STD_DEV_BIT                 = (1<<15),
    /** valid North Velocity */
    CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_VEL_BIT                    = (1<<16),
    /** valid East Velocity */
    CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_VEL_BIT                     = (1<<17),
    /** valid Up Velocity */
    CLIENT_DIAG_GNSS_LOCATION_INFO_UP_VEL_BIT                       = (1<<18),
    /** valid North Velocity Uncertainty */
    CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_VEL_UNC_BIT                = (1<<19),
    /** valid East Velocity Uncertainty */
    CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_VEL_UNC_BIT                 = (1<<20),
    /** valid Up Velocity Uncertainty */
    CLIENT_DIAG_GNSS_LOCATION_INFO_UP_VEL_UNC_BIT                   = (1<<21),
    /** valid leap seconds */
    CLIENT_DIAG_GNSS_LOCATION_INFO_LEAP_SECONDS_BIT                 = (1<<22),
    /** valid time uncertainty */
    CLIENT_DIAG_GNSS_LOCATION_INFO_TIME_UNC_BIT                     = (1<<23),
    /** valid numSvUsedInPosition */
    CLIENT_DIAG_GNSS_LOCATION_INFO_NUM_SV_USED_IN_POSITION_BIT      = (1<<24),
    /** valid calibrationConfidencePercent */
    CLIENT_DIAG_GNSS_LOCATION_INFO_CALIBRATION_CONFIDENCE_PERCENT_BIT = (1<<25),
    /** valid calibrationStatus */
    CLIENT_DIAG_GNSS_LOCATION_INFO_CALIBRATION_STATUS_BIT           = (1<<26),
    /** valid output engine type */
    CLIENT_DIAG_GNSS_LOCATION_INFO_OUTPUT_ENG_TYPE_BIT              = (1<<27),
    /** valid output engine mask */
    CLIENT_DIAG_GNSS_LOCATION_INFO_OUTPUT_ENG_MASK_BIT              = (1<<28),
    /** valid output conformityIndex */
    CLIENT_DIAG_GNSS_LOCATION_INFO_CONFORMITY_INDEX_BIT             = (1<<29),
    /** valid llaVRPBased.  */
    CLIENT_DIAG_GNSS_LOCATION_INFO_LLA_VRP_BASED_BIT                = (1<<30),
    /** valid enuVelocityVRPBased. */
    CLIENT_DIAG_GNSS_LOCATION_INFO_ENU_VELOCITY_VRP_BASED_BIT       = (1ULL<<31),
    /** valid drSolutionStatusMask. */
    CLIENT_DIAG_GNSS_LOCATION_INFO_DR_SOLUTION_STATUS_MASK_BIT      = (1ULL<<32),
    /** valid altitudeAssumed  */
    CLIENT_DIAG_GNSS_LOCATION_INFO_ALTITUDE_ASSUMED_BIT             = (1ULL<<33),
    /** valid sessionStatus  */
    CLIENT_DIAG_GNSS_LOCATION_INFO_SESSION_STATUS_BIT               = (1ULL<<34),
    /** integrity risk used */
    CLIENT_DIAG_GNSS_LOCATION_INFO_INTEGRITY_RISK_USED_BIT          = (1ULL<<35),
    /** along-track protection level */
    CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_ALONG_TRACK_BIT          = (1ULL<<36),
    /** Cross-track protection level */
    CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_CROSS_TRACK_BIT          = (1ULL<<37),
    /** vertical component protection level */
    CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_VERTICAL_BIT             = (1ULL<<38),
    /** GnssLocation has valid GnssLocation::dgnssStationId */
    CLIENT_DIAG_GNSS_LOCATION_INFO_DGNSS_STATION_ID_BIT             = (1ULL<<39),
} clientDiagGnssLocationInfoFlagBits;

typedef enum {
    CLIENT_DIAG_LOCATION_RELIABILITY_NOT_SET = 0,
    CLIENT_DIAG_LOCATION_RELIABILITY_VERY_LOW,
    CLIENT_DIAG_LOCATION_RELIABILITY_LOW,
    CLIENT_DIAG_LOCATION_RELIABILITY_MEDIUM,
    CLIENT_DIAG_LOCATION_RELIABILITY_HIGH,
} clientDiagLocationReliability;

typedef PACKED struct PACKED_POST {
    uint64_t gpsSvUsedIdsMask;
    uint64_t gloSvUsedIdsMask;
    uint64_t galSvUsedIdsMask;
    uint64_t bdsSvUsedIdsMask;
    uint64_t qzssSvUsedIdsMask;
    uint64_t navicSvUsedIdsMask;
} clientDiagGnssLocationSvUsedInPosition;

typedef uint32_t clientDiagGnssLocationNavSolutionMask;
typedef enum {
    /** SBAS ionospheric correction is used*/
    CLIENT_DIAG_LOCATION_SBAS_CORRECTION_IONO_BIT  = (1<<0),
    /** SBAS fast correction is used*/
    CLIENT_DIAG_LOCATION_SBAS_CORRECTION_FAST_BIT  = (1<<1),
    /** SBAS long-tem correction is used*/
    CLIENT_DIAG_LOCATION_SBAS_CORRECTION_LONG_BIT  = (1<<2),
    /** SBAS integrity information is used*/
    CLIENT_DIAG_LOCATION_SBAS_INTEGRITY_BIT        = (1<<3),
    /** Position Report is DGNSS corrected*/
    CLIENT_DIAG_LOCATION_NAV_CORRECTION_DGNSS_BIT  = (1<<4),
    /** Position Report is RTK corrected*/
    CLIENT_DIAG_LOCATION_NAV_CORRECTION_RTK_BIT    = (1<<5),
    /** Position Report is PPP corrected*/
    CLIENT_DIAG_LOCATION_NAV_CORRECTION_PPP_BIT    = (1<<6),
    /** Posiiton Report is RTF fixed corrected */
    CLIENT_DIAG_LOCATION_NAV_CORRECTION_RTK_FIXED_BIT  = (1<<7),
    /** Position report is computed with only SBAS corrected SVs */
    CLIENT_DIAG_LOCATION_NAV_CORRECTION_ONLY_SBAS_CORRECTED_SV_USED_BIT = (1<<8)
} clientDiagGnssLocationNavSolutionBits;

typedef uint32_t clientDiagGnssLocationPosTechMask;
typedef enum {
    CLIENT_DIAG_LOCATION_POS_TECH_DEFAULT_BIT                  = 0,
    CLIENT_DIAG_LOCATION_POS_TECH_SATELLITE_BIT                = (1<<0),
    CLIENT_DIAG_LOCATION_POS_TECH_CELLID_BIT                   = (1<<1),
    CLIENT_DIAG_LOCATION_POS_TECH_WIFI_BIT                     = (1<<2),
    CLIENT_DIAG_LOCATION_POS_TECH_SENSORS_BIT                  = (1<<3),
    CLIENT_DIAG_LOCATION_POS_TECH_REFERENCE_LOCATION_BIT       = (1<<4),
    CLIENT_DIAG_LOCATION_POS_TECH_INJECTED_COARSE_POSITION_BIT = (1<<5),
    CLIENT_DIAG_LOCATION_POS_TECH_AFLT_BIT                     = (1<<6),
    CLIENT_DIAG_LOCATION_POS_TECH_HYBRID_BIT                   = (1<<7),
    CLIENT_DIAG_LOCATION_POS_TECH_PPE_BIT                      = (1<<8),
    CLIENT_DIAG_LOCATION_POS_TECH_VEH_BIT                      = (1<<9),
    CLIENT_DIAG_LOCATION_POS_TECH_VIS_BIT                      = (1<<10),
    CLIENT_DIAG_LOCATION_POS_TECH_PROPAGATED_BIT               = (1<<11)
} clientDiagGnssLocationPosTechBits;

typedef enum {
    /** This is the propagated/aggregated reports from all engines
        running on the system (e.g.: DR/SPE/PPE). */
    CLIENT_DIAG_LOC_OUTPUT_ENGINE_FUSED = 0,
    /** This fix is the unmodified fix from modem GNSS engine */
    CLIENT_DIAG_LOC_OUTPUT_ENGINE_SPE   = 1,
    /** This is the unmodified fix from PPP/RTK correction engine */
    CLIENT_DIAG_LOC_OUTPUT_ENGINE_PPE   = 2,
    /** This is unmodified fix from VP Positioning Engine */
    CLIENT_DIAG_LOC_OUTPUT_ENGINE_VPE   = 3,
    CLIENT_DIAG_LOC_OUTPUT_ENGINE_COUNT,
} clientDiagLocOutputEngineType;

typedef uint32_t clientDiagPositioningEngineMask;
typedef enum {
    CLIENT_DIAG_STANDARD_POSITIONING_ENGINE = (1 << 0),
    CLIENT_DIAG_DEAD_RECKONING_ENGINE       = (1 << 1),
    CLIENT_DIAG_PRECISE_POSITIONING_ENGINE  = (1 << 2),
    CLIENT_DIAG_VP_POSITIONING_ENGINE       = (1 << 3)
} clientDiagPositioningEngineBits;

/** Specify various status that contributes to the DR poisition
 *  engine. <br/> */
typedef uint64_t clientDiagDrSolutionStatusMask;
typedef enum {
    /** Vehicle sensor speed input was detected by the DR position
     *  engine. <br/> */
    CLIENT_DIAG_DR_SOLUTION_STATUS_VEHICLE_SENSOR_SPEED_INPUT_DETECTED = (1<<0),
    /** Vehicle sensor speed input was used by the DR position
     *  engine. <br/> */
    CLIENT_DIAG_DR_SOLUTION_STATUS_VEHICLE_SENSOR_SPEED_INPUT_USED     = (1<<1),
    CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_UNCALIBRATED                = (1<<2),
    CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_GNSS_QUALITY_INSUFFICIENT   = (1<<3),
    CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_FERRY_DETECTED              = (1<<4),
    CLIENT_DIAG_DR_SOLUTION_STATUS_ERROR_6DOF_SENSOR_UNAVAILABLE       = (1<<5),
    CLIENT_DIAG_DR_SOLUTION_STATUS_ERROR_VEHICLE_SPEED_UNAVAILABLE     = (1<<6),
    CLIENT_DIAG_DR_SOLUTION_STATUS_ERROR_GNSS_EPH_UNAVAILABLE          = (1<<7),
    CLIENT_DIAG_DR_SOLUTION_STATUS_ERROR_GNSS_MEAS_UNAVAILABLE         = (1<<8),
    CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_INIT_POSITION_INVALID       = (1<<9),
    CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_INIT_POSITION_UNRELIABLE    = (1<<10),
    CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_POSITON_UNRELIABLE          = (1<<11),
    CLIENT_DIAG_DR_SOLUTION_STATUS_ERROR_GENERIC                       = (1<<12),
    CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_SENSOR_TEMP_OUT_OF_RANGE    = (1<<13),
    CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_USER_DYNAMICS_INSUFFICIENT  = (1<<14),
    CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_FACTORY_DATA_INCONSISTENT   = (1<<15)
} clientDiagDrSolutionStatusBits;

typedef PACKED struct PACKED_POST {
    /**  Latitude, in unit of degrees, range [-90.0, 90.0]. */
    double latitude;

    /**  Longitude, in unit of degrees, range [-180.0, 180.0]. */
    double longitude;

    /** Altitude above the WGS 84 reference ellipsoid, in unit
    of meters.  */
    float altitude;
} clientDiagLLAInfo;

typedef uint64_t clientDiagLocationCapabilitiesMask;
typedef enum {
    /** LocationClientApi can support time-based tracking session
     *  via LocationClientApi::startPositionSession(uint32_t,
     *  LocReqEngineTypeMask, const EngineReportCbs&, ResponseCb)
     *  and LocationClientApi::startPositionSession(uint32_t, const
     *  GnssReportCbs&, ResponseCb) and
     *  LocationClientApi::startPositionSession(uint32_t, uint32_t,
     *  LocationCb, ResponseCb) with distanceInMeters set to 0.
     *  <br/> */
    CLIENT_DIAG_LOCATION_CAPS_TIME_BASED_TRACKING_BIT           = (1<<0),
    /** LocationClientApi can support time-based batching session
     *  via LocationClientApi::startRoutineBatchingSession() with
     *  minInterval specified. <br/> */
    CLIENT_DIAG_LOCATION_CAPS_TIME_BASED_BATCHING_BIT           = (1<<1),
    /** LocationClientApi can support distance based tracking
     *  session via
     *  LocationClientApi::startPositionSession(uint32_t, uint32_t,
     *  LocationCb, ResponseCb) with distanceInMeters specified.
     *  <br/> */
    CLIENT_DIAG_LOCATION_CAPS_DISTANCE_BASED_TRACKING_BIT       = (1<<2),
    /** LocationClientApi can support distance-based batching via
     *  LocationClientApi::startRoutineBatchingSession() with
     *  minDistance set to none-zero value. <br/> */
    CLIENT_DIAG_LOCATION_CAPS_DISTANCE_BASED_BATCHING_BIT       = (1<<3),
    /** LocationClientApi can support geo fence via
     *  LocationClientApi::addGeofences(). <br/>   */
    CLIENT_DIAG_LOCATION_CAPS_GEOFENCE_BIT                      = (1<<4),
    /** LocationClientApi can support trip batching via
     *  LocationClientApi::startTripBatchingSession(). <br/>   */
    CLIENT_DIAG_LOCATION_CAPS_OUTDOOR_TRIP_BATCHING_BIT         = (1<<5),
    /** LocationClientApi can support receiving GnssMeasurements
     *  data in GnssMeasurementsCb when LocationClientApi is in
     *  a positioning session.. <br/>   */
    CLIENT_DIAG_LOCATION_CAPS_GNSS_MEASUREMENTS_BIT             = (1<<6),
    /** LocationIntegrationApi can support configure constellations
     *  via LocationIntegrationApi::configConstellations.  <br/>   */
    CLIENT_DIAG_LOCATION_CAPS_CONSTELLATION_ENABLEMENT_BIT      = (1<<7),
    /** Modem supports Carrier Phase for Precise Positioning
     *  Measurement Engine (PPME).
     *  This is a Standalone Feature.  <br/>   */
    CLIENT_DIAG_LOCATION_CAPS_CARRIER_PHASE_BIT                 = (1<<8),
    /** Modem supports SV Polynomial for tightly coupled
     *  external DR support.
     *  This is a Standalone Feature.  <br/>   */
    CLIENT_DIAG_LOCATION_CAPS_SV_POLYNOMIAL_BIT                 = (1<<9),
    /** Modem supports GNSS Single Frequency feature.
     *  This is a Standalone Feature.  <br/>   */
    CLIENT_DIAG_LOCATION_CAPS_QWES_GNSS_SINGLE_FREQUENCY        = (1<<10),
    /** Modem supports GNSS Multi Frequency feature. Multi
     *  Frequency enables Single frequency also.  <br/>   */
    CLIENT_DIAG_LOCATION_CAPS_QWES_GNSS_MULTI_FREQUENCY         = (1<<11),
    /** This mask indicates VEPP license bundle is enabled.
     *  VEPP bundle include Carrier Phase and SV Polynomial
     *  features.  <br/>   */
    CLIENT_DIAG_LOCATION_CAPS_QWES_VPE                          = (1<<12),
    /** This mask indicates support for CV2X Location basic
     *  features. This bundle includes features for GTS Time
     *  & Freq, C-TUNC (Constrained Time uncertainity.
     *  LocationIntegrationApi can support setting of C-TUNC
     *  via configConstrainedTimeUncertainty. <br/> */
    CLIENT_DIAG_LOCATION_CAPS_QWES_CV2X_LOCATION_BASIC          = (1<<13),
    /** This mask indicates support for CV2X Location premium
     *  features. This bundle includes features for CV2X Location
     *  Basic features, QDR3 feature, and PACE. (Position
     *  Assisted Clock Estimator.
     *  LocationIntegrationApi can support setting of C-TUNC
     *  via configPositionAssistedClockEstimator. <br/> */
    CLIENT_DIAG_LOCATION_CAPS_QWES_CV2X_LOCATION_PREMIUM        = (1<<14),
    /** This mask indicates that PPE (Precise Positioning Engine)
     *  library is enabled or Precise Positioning Framework (PPF)
     *  is available. This bundle includes features for Carrier
     *  Phase and SV Ephermeris.  <br/>   */
    CLIENT_DIAG_LOCATION_CAPS_QWES_PPE                          = (1<<15),
    /** This mask indicates QDR2_C license bundle is enabled.
     *  This bundle includes features for SV Polynomial. <br/> */
    CLIENT_DIAG_LOCATION_CAPS_QWES_QDR2                         = (1<<16),
    /** This mask indicates QDR3_C license bundle is enabled.
     *  This bundle includes features for SV Polynomial. <br/> */
    CLIENT_DIAG_LOCATION_CAPS_QWES_QDR3                         = (1<<17),
    /** This mask indicates QWES license for ML inference of Pseudo Range meas is present <br/> */
    CLIENT_DIAG_LOCATION_CAPS_NLOS_ML20                         = (1<<18),

} clientDiagLocationCapabilitiesBits;

enum clientDiagLocSessionStatus {
    CLIENT_DIAG_LOC_SESS_SUCCESS      = 0,
    CLIENT_DIAG_LOC_SESS_INTERMEDIATE = 1,
    CLIENT_DIAG_LOC_SESS_FAILURE      = 2,
};

// Specify the LCA API type
// Used by LCA diag location report to indicate the
// API that is called to generate the location report
enum clientDiagLocReportTriggerType {
    CLIENT_DIAG_LOC_REPORT_TRIGGER_UNSPECIFIED               = 0,
    CLIENT_DIAG_LOC_REPORT_TRIGGER_SIMPLE_TRACKING_SESSION   = 1,
    CLIENT_DIAG_LOC_REPORT_TRIGGER_DETAILED_TRACKING_SESSION = 2,
    CLIENT_DIAG_LOC_REPORT_TRIGGER_ENGINE_TRACKING_SESSION   = 3,
    CLIENT_DIAG_LOC_REPORT_TRIGGER_SINGLE_TERRESTRIAL_FIX    = 4,
    CLIENT_DIAG_LOC_REPORT_TRIGGER_SINGLE_FIX                = 5,
    CLIENT_DIAG_LOC_REPORT_TRIGGER_TRIP_BATCHING_SESSION     = 6,
    CLIENT_DIAG_LOC_REPORT_TRIGGER_ROUTINE_BATCHING_SESSION  = 7,
    CLIENT_DIAG_LOC_REPORT_TRIGGER_GEOFENCE_SESSION          = 8,
};

typedef PACKED struct PACKED_POST {
    /** Used by Logging Module
      *  Mandatory field */
    log_hdr_type logHeader;
    /** clientDiag Message Version
     *  Mandatory field */
    uint8 version;
    /** bitwise OR of LocationFlagsBits to mark which params are valid */
    clientDiagLocationFlagsMask flags;
    /** UTC timestamp for location fix, milliseconds since January 1, 1970 */
    uint64_t timestamp;
    /** in degrees; range [-90.0, 90.0] */
    double latitude;
    /** in degrees; range [-180.0, 180.0] */
    double longitude;
    /** in meters above the WGS 84 reference ellipsoid */
    double altitude;
    /** in meters per second */
    float speed;
    /** in degrees; range [0, 360) */
    float bearing;
    /** in meters */
    float horizontalAccuracy;
    /** in meters */
    float verticalAccuracy;
    /** in meters/second */
    float speedAccuracy;
    /** in degrees (0 to 359.999) */
    float bearingAccuracy;
    clientDiagLocationTechnologyMask techMask;
    /** bitwise OR of GnssLocationInfoBits for param validity */
    clientDiagGnssLocationInfoFlagMask gnssInfoFlags;
    /** altitude wrt mean sea level */
    float altitudeMeanSeaLevel;
    /** position dilusion of precision */
    float pdop;
    /** horizontal dilusion of precision */
    float hdop;
    /** vertical dilusion of precision */
    float vdop;
    /** geometric  dilution of precision */
    float gdop;
    /** time dilution of precision */
    float tdop;
    /** magnetic deviation */
    float magneticDeviation;
    /** horizontal reliability */
    clientDiagLocationReliability horReliability;
    /** vertical reliability */
    clientDiagLocationReliability verReliability;
    /** horizontal elliptical accuracy semi-major axis */
    float horUncEllipseSemiMajor;
    /** horizontal elliptical accuracy semi-minor axis */
    float horUncEllipseSemiMinor;
    /** horizontal elliptical accuracy azimuth */
    float horUncEllipseOrientAzimuth;
    /** North standard deviation Unit: Meters */
    float northStdDeviation;
    /** East standard deviation. Unit: Meters */
    float eastStdDeviation;
    /** North Velocity.Unit: Meters/sec */
    float northVelocity;
    /** East Velocity  Unit: Meters/sec */
    float eastVelocity;
    /** Up Velocity  Unit: Meters/sec */
    float upVelocity;
    float northVelocityStdDeviation;
    float eastVelocityStdDeviation;
    float upVelocityStdDeviation;
    /** Gnss sv used in position data */
    clientDiagGnssLocationSvUsedInPosition svUsedInPosition;
    /** Nav solution mask to indicate sbas corrections */
    clientDiagGnssLocationNavSolutionMask  navSolutionMask;
    /** Position technology used in computing this fix */
    clientDiagGnssLocationPosTechMask      posTechMask;
    /** Body Frame Dynamics: 4wayAcceleration and pitch set with validity */
    clientDiagGnssLocationPositionDynamics bodyFrameData;
    /**  GNSS System Time */
    clientDiagGnssSystemTime               gnssSystemTime;
    /** Number of measurements received for use in fix */
    uint8_t                                numOfMeasReceived;
    /** Gnss measurement usage info  */
    clientDiagGnssMeasUsageInfo measUsageInfo[CLIENT_DIAG_GNSS_SV_MAX];
    /** Leap Seconds */
    uint8_t leapSeconds;
    /** Time uncertainty in milliseconds   */
    float timeUncMs;
    /** number of SV used in position report */
    uint16_t numSvUsedInPosition;
    /** Sensor calibration confidence percent, valid range [0,
     *  100] */
    uint8_t calibrationConfidencePercent;
    /** Sensor calibration status */
    clientDiagDrCalibrationStatusMask calibrationStatus;
    /** boot timestamp in nano-second when this diag log packet is
     *  created and filled with the info at location client api
     *  layer */
    uint64_t bootTimestampNs;
    /** location engine type. When the fix. when the type is set to
        LOC_ENGINE_SRC_FUSED, the fix is the propagated/aggregated
        reports from all engines running on the system (e.g.:
        DR/SPE/PPE). To check which location engine contributes to
        the fused output, check for locOutputEngMask. */
    clientDiagLocOutputEngineType locOutputEngType;
    /** when loc output eng type is set to fused, this field
        indicates the set of engines contribute to the fix. */
    clientDiagPositioningEngineMask locOutputEngMask;
    /** When robust location is enabled, this field
     * will indicate how well the various input data considered for
     * navigation solution conform to expectations.
     * Range: [0.0, 1.0], with 0.0 for least conforming and 1.0 for
     * most conforming. */
    float conformityIndex;
    /** VRR-based latitude/longitude/altitude. */
    clientDiagLLAInfo llaVRPBased;
    /** VRR-based east, north, and up velocity */
    float enuVelocityVRPBased[3];
    /** qtimer tick count when this diag log packet is
     *  created and filled with the info at location client api
     *  layer. This field is always valid. */
    uint64_t qtimerTickCnt;
    /** DR fix status mask */
    clientDiagDrSolutionStatusMask drSolutionStatusMask;
    char processName[32];
    /** When this field is valid, it will indicates whether altitude
     *  is assumed or calculated.
     *  false: Altitude is calculated.
     *  true:  Altitude is assumed; there may not be enough
     *         satellites to determine the precise altitude. */
    bool altitudeAssumed;
    clientDiagLocationCapabilitiesMask capabilitiesMask;
    /** session status */
    clientDiagLocSessionStatus sessionStatus;
    /** integrity risk used for protection level parameters. */
    uint32_t integrityRiskUsed;
    /** along-track protection level */
    float    protectAlongTrack;
    /** cross-track protection level */
    float    protectCrossTrack;
    /** vertical component protection level */
    float    protectVertical;
    /** Boot timestamp corresponding to the UTC timestamp for
     *  location fix. */
    uint64_t elapsedRealTimeNs;
    /** Uncertainty for the boot timestamp. */
    uint64_t elapsedRealTimeUncNs;
    /** session start boot timestamp in ns */
    uint64_t sessionStartBootTimestampNs;
    /** lca req api type */
    clientDiagLocReportTriggerType reportTriggerType;
    /** number of dgnss station id that is valid in dgnssStationId */
    uint16_t  numOfDgnssStationId;
    /** dgnss station id */
    uint16_t dgnssStationId[CLIENT_DIAG_DGNSS_STATION_ID_MAX];
    /** GPTP time field in ns */
    uint64_t elapsedgPTPTime;
    /** GPTP time Unc */
    uint64_t elapsedgPTPTimeUnc;
} clientDiagGnssLocationStructType;

typedef PACKED struct PACKED_POST {
    /** in degrees; range [-90.0, 90.0] */
    double latitude;
    /** in degrees; range [-180.0, 180.0] */
    double longitude;
    /** radius of the circular Geofence in meters */
    double radius;
    /** bitwise OR of LocationFlagsBits to specify the breach events */
    clientDiagLocationFlagsMask breachType;
    /** Specifies the rate of detection for a Geofence breach, range [1, 5] */
    uint32_t responsiveness;
    /** the time in seconds a user spends in the Geofence before a dwell event is sent */
    uint32_t dwellTime;
} clientDiagGeofenceData;

typedef uint32_t clientDiagGnssMeasurementsDataFlagsMask;
typedef enum {
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SV_ID_BIT = (1 << 0),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SV_TYPE_BIT = (1 << 1),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_STATE_BIT = (1 << 2),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_BIT = (1 << 3),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_UNCERTAINTY_BIT = (1 << 4),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_TO_NOISE_BIT = (1 << 5),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT = (1 << 6),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT = (1 << 7),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_STATE_BIT = (1 << 8),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_BIT = (1 << 9),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_UNCERTAINTY_BIT = (1 << 10),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT = (1 << 11),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT = (1 << 12),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT = (1 << 13),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT = (1 << 14),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_MULTIPATH_INDICATOR_BIT = (1 << 15),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT = (1 << 16),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT = (1 << 17),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_GNSS_SIGNAL_TYPE_BIT       = (1 << 18),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_BASEBAND_CARRIER_TO_NOISE_BIT = (1<<19),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT                  = (1 << 20),
    CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_FULL_ISB_UNCERTAINTY_BIT      = (1 << 21)
} clientDiagGnssMeasurementsDataFlagsBits;

typedef uint32_t clientDiagGnssMeasurementsStateMask;
typedef enum {
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_UNKNOWN_BIT = 0,
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_CODE_LOCK_BIT = (1 << 0),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_BIT_SYNC_BIT = (1 << 1),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_SUBFRAME_SYNC_BIT = (1 << 2),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_TOW_DECODED_BIT = (1 << 3),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_MSEC_AMBIGUOUS_BIT = (1 << 4),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_SYMBOL_SYNC_BIT = (1 << 5),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_GLO_STRING_SYNC_BIT = (1 << 6),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_GLO_TOD_DECODED_BIT = (1 << 7),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_BDS_D2_BIT_SYNC_BIT = (1 << 8),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_BDS_D2_SUBFRAME_SYNC_BIT = (1 << 9),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_GAL_E1BC_CODE_LOCK_BIT = (1 << 10),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_GAL_E1C_2ND_CODE_LOCK_BIT = (1 << 11),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_GAL_E1B_PAGE_SYNC_BIT = (1 << 12),
    CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_SBAS_SYNC_BIT = (1 << 13),
} clientDiagGnssMeasurementsStateBits;

typedef uint16_t clientDiagGnssMeasurementsAdrStateMask;
typedef enum {
    CLIENT_DIAG_GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_UNKNOWN = 0,
    CLIENT_DIAG_GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_VALID_BIT = (1 << 0),
    CLIENT_DIAG_GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_RESET_BIT = (1 << 1),
    CLIENT_DIAG_GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_CYCLE_SLIP_BIT = (1 << 2),
    CLIENT_DIAG_GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_HALF_CYCLE_RESOLVED_BIT = (1<<3),
} clientDiagGnssMeasurementsAdrStateBits;

typedef enum {
    CLIENT_DIAG_GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_UNKNOWN = 0,
    CLIENT_DIAG_GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_PRESENT,
    CLIENT_DIAG_GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_NOT_PRESENT,
} clientDiagGnssMeasurementsMultipathIndicator;

typedef enum {
    CLIENT_DIAG_AGC_STATUS_UNKNOWN                              = 0,
    CLIENT_DIAG_AGC_STATUS_NO_SATURATION                        = 1,
    CLIENT_DIAG_AGC_STATUS_FRONT_END_GAIN_MAXIMUM_SATURATION    = 2,
    CLIENT_DIAG_AGC_STATUS_FRONT_END_GAIN_MINIMUM_SATURATION    = 3,
} clientDiagAgcStatus;

typedef uint32_t clientDiagGnssMeasurementsClockFlagsMask;
typedef enum {
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_LEAP_SECOND_BIT = (1 << 0),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_BIT = (1 << 1),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_UNCERTAINTY_BIT = (1 << 2),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_FULL_BIAS_BIT = (1 << 3),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_BIT = (1 << 4),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_UNCERTAINTY_BIT = (1 << 5),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_BIT = (1 << 6),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_UNCERTAINTY_BIT = (1 << 7),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_HW_CLOCK_DISCONTINUITY_COUNT_BIT = (1 << 8),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_BIT = (1 << 9),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_UNC_BIT = (1 << 10),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_GPTP_TIME_BIT = (1 << 11),
    CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_GPTP_TIME_UNC_BIT = (1 << 12)
} clientDiagGnssMeasurementsClockFlagsBits;

typedef PACKED struct PACKED_POST {
    clientDiagGnssMeasurementsDataFlagsMask flags;  // bitwise OR of GnssMeasurementsDataFlagsBits
    int16_t svId;
    clientDiagGnssSvType svType;
    double timeOffsetNs;
    clientDiagGnssMeasurementsStateMask stateMask;  // bitwise OR of GnssMeasurementsStateBits
    int64_t receivedSvTimeNs;
    float receivedSvTimeSubNs;
    int64_t receivedSvTimeUncertaintyNs;
    double carrierToNoiseDbHz;
    double pseudorangeRateMps;
    double pseudorangeRateUncertaintyMps;
    clientDiagGnssMeasurementsAdrStateMask adrStateMask;
    double adrMeters;
    double adrUncertaintyMeters;
    float carrierFrequencyHz;
    int64_t carrierCycles;
    double carrierPhase;
    double carrierPhaseUncertainty;
    clientDiagGnssMeasurementsMultipathIndicator multipathIndicator;
    double signalToNoiseRatioDb;
    double agcLevelDb;
    double basebandCarrierToNoiseDbHz;
    clientDiagGnssSignalTypeMask gnssSignalType;
    double fullInterSignalBiasNs;
    double fullInterSignalBiasUncertaintyNs;
} clientDiagGnssMeasurementsData;

typedef PACKED struct PACKED_POST {
    clientDiagGnssMeasurementsClockFlagsMask flags;// bitwise OR of GnssMeasurementsClockFlagsBits
    int16_t leapSecond;
    int64_t timeNs;
    double timeUncertaintyNs;
    int64_t fullBiasNs;
    double biasNs;
    double biasUncertaintyNs;
    double driftNsps;
    double driftUncertaintyNsps;
    uint32_t hwClockDiscontinuityCount;
    uint64_t elapsedRealTime;
    uint64_t elapsedRealTimeUnc;
    uint64_t elapsedgPTPTime;
    uint64_t elapsedgPTPTimeUnc;
} clientDiagGnssMeasurementsClock;

/* This structure is too large to be sent in one shot,
   therefore we segment using maxSequence and sequenceNumber
*/
typedef PACKED struct PACKED_POST {
    /** Used by Logging Module
    *  Mandatory field */
    log_hdr_type logHeader;
    /** clientDiag Message Version
    *  Mandatory field */
    uint8 version;
    uint8 maxSequence;
    uint8 sequenceNumber;
    uint32_t count;        // number of items in GnssMeasurementsData array
    bool isNhz;            // NHz measurement indicator
    clientDiagGnssMeasurementsClock clock; // clock
    clientDiagGnssMeasurementsData measurements[CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ];
    clientDiagAgcStatus agcStatusL1; // RF Automatic gain control status for L1 band
    clientDiagAgcStatus agcStatusL2; // RF Automatic gain control status for L2 band
    clientDiagAgcStatus agcStatusL5; // RF Automatic gain control status for L5 band
} clientDiagGnssMeasurementsStructType;

typedef PACKED struct PACKED_POST {
    /** Used by Logging Module
      *  Mandatory field */
    log_hdr_type logHeader;
    /** clientDiag Message Version
     *  Mandatory field */
    uint8 version;
    /** number of SVs in the GnssSvs array */
    uint32_t count;
    clientDiagGnssSv gnssSvs[CLIENT_DIAG_GNSS_SV_MAX];
} clientDiagGnssSvStructType;

/** Flags to indicate which values are valid in a Location */
typedef uint16_t clientDiagGeofenceBreachTypeMask;
typedef enum {
    CLIENT_DIAG_GEOFENCE_BREACH_ENTER_BIT               = (1<<0),
    CLIENT_DIAG_GEOFENCE_BREACH_EXIT_BIT                = (1<<1),
    CLIENT_DIAG_GEOFENCE_BREACH_DWELL_IN_BIT            = (1<<2),
    CLIENT_DIAG_GEOFENCE_BREACH_DWELL_OUT_BIT           = (1<<3),
} clientDiagGeofenceBreachTypeBits;

typedef PACKED struct PACKED_POST {
    /** Used by Logging Module
      *  Mandatory field */
    log_hdr_type logHeader;
    /** clientDiag Message Version
     *  Mandatory field */
    uint32 version;
    /** number of SVs in the GnssSvs array */
    uint32_t count;
    clientDiagGeofenceData geofences[CLIENT_DIAG_GEOFENCE_MAX];
    clientDiagGnssLocationStructType location;
    clientDiagGeofenceBreachTypeMask type;
    uint64_t timestamp;
} clientDiagGeofenceBreachStructType;

typedef PACKED struct PACKED_POST {
    /** Used by Logging Module
      *  Mandatory field */
    log_hdr_type logHeader;
    /** clientDiag Message Version
     *  Mandatory field */
    uint8 version;
/** Timestamp of NMEA generation */
    uint64_t timestamp;
/** engine type */
    LocOutputEngineType engType;
/** Length of nmea data */
    uint32_t nmeaLength;
/** One or more nmea strings separated by \n charachter */
    uint8_t nmea[1];
} clientDiagGnssNmeaStructType;

typedef PACKED struct PACKED_POST {
    /** Used by Logging Module
    *  Mandatory field */
    log_hdr_type logHeader;
    /** clientDiag Message Version
    *  Mandatory field */
    uint32 version;

    uint64_t meQtimer1;             // in ticks
    uint64_t meQtimer2;             // in ticks
    uint64_t meQtimer3;             // in ticks
    uint64_t peQtimer1;             // in ticks
    uint64_t peQtimer2;             // in ticks
    uint64_t peQtimer3;             // in ticks
    uint64_t smQtimer1;             // in ticks
    uint64_t smQtimer2;             // in ticks
    uint64_t smQtimer3;             // in ticks
    uint64_t locMwQtimer;           // in ticks
    uint64_t hlosQtimer1;           // in ticks
    uint64_t hlosQtimer2;           // in ticks
    uint64_t hlosQtimer3;           // in ticks
    uint64_t hlosQtimer4;           // in ticks
    uint64_t hlosQtimer5;           // in ticks
    uint64_t overallLatency;        // in nanoseconds
    uint64_t meLatency1;            // in nanoseconds
    uint64_t meLatency2;            // in nanoseconds
    uint64_t meToPeLatency;         // in nanoseconds
    uint64_t peLatency1;            // in nanoseconds
    uint64_t peLatency2;            // in nanoseconds
    uint64_t peToSmLatency;         // in nanoseconds
    uint64_t smLatency1;            // in nanoseconds
    uint64_t smLatency2;            // in nanoseconds
    uint64_t smToLocMwLatency;      // in nanoseconds
    uint64_t locMwToHlosLatency;    // in nanoseconds
    uint64_t hlosLatency1;          // in nanoseconds
    uint64_t hlosLatency2;          // in nanoseconds
    uint64_t hlosLatency3;          // in nanoseconds
    uint64_t hlosLatency4;          // in nanoseconds
} clientDiagGnssLatencyStructType;

/** Disaster crisis report type from GNSS engine that are
 *  currently supported. <br/> */
enum clientDiagGnssDcReportType {
    /** Disaster Prevention information provided by Japan
     *  Meteolorogical Agency. <br/>  */
    CLIENT_DIAG_QZSS_JMA_DISASTER_PREVENTION_INFO = 43,
    /** Disaster Prevention information provided by other
     *  organizations. <br/> */
    CLIENT_DIAG_QZSS_NON_JMA_DISASTER_PREVENTION_INFO = 44,
    /** force enum type to 32 bit */
    CLIENT_DIAG_GNSS_DC_REPORT_TYPE_MAX = INT_MAX,
};

/** Specify the info contained in the disaster and crisis report
 *  received from GNSS engine. <br/>  */
typedef PACKED struct PACKED_POST {
    /** Used by Logging Module
      *  Mandatory field */
    log_hdr_type logHeader;
    /** clientDiag Message Version
     *  Mandatory field */
    uint32 version;
    /** dc report type, as defined in standard.  <br/> */
    clientDiagGnssDcReportType dcReportType;
    /** number of valid bits that client should make use in
    dcReportData. <br/>  */
    uint16_t                   numValidBits;
    /** number of valid bytes in dcReportData */
    uint8_t                    dcReportDataLen;
    /** dc report data payload */
    uint8_t                    dcReportData[64];
} clientDiagGnssDcReportStructType;

/** Specifies Source of Ephemeris data */
typedef enum {
    /** Source of ephemeris is unknown  */
    CLIENT_DIAG_GNSS_EPH_SRC_UNKNOWN     = 0,
    /** Source of ephemeris is OTA  */
    CLIENT_DIAG_GNSS_EPH_SRC_OTA         = 1,
    /** Max value for ephemeris Source. DO NOT USE  */
    CLIENT_DIAG_GNSS_EPH_SRC_MAX         = 999
} clientDiagGnssEphSource;

/** Specifies the action to be performed on receipt of the ephemeris (Update/Delete)
 *  Action shall be performed on GnssEphSource specified. */
typedef enum {
    /** Epehmeris Action Unknown  */
    CLIENT_DIAG_GNSS_EPH_ACTION_UNKNOWN = 0,
    /** Update ephemeris data */
    CLIENT_DIAG_GNSS_EPH_ACTION_UPDATE     = 1,
    /** delete ephemeris action. */
    CLIENT_DIAG_GNSS_EPH_ACTION_DELETE     = 2,
    /** Max value for  ephemeris action. DO NOT USE  */
    CLIENT_DIAG_GNSS_EPH_ACTION_MAX     = 999,
} clientDiagGnssEphAction;

/** Galileo Signal Source. */
typedef enum {
    /** GALILEO signal is unknown */
    CLIENT_DIAG_GAL_EPH_SIGNAL_SRC_UNKNOWN = 0,
    /** GALILEO signal is E1B  */
    CLIENT_DIAG_GAL_EPH_SIGNAL_SRC_E1B    = 1,
    /** GALILEO signal is E5A  */
    CLIENT_DIAG_GAL_EPH_SIGNAL_SRC_E5A    = 2,
    /** GALILEO signal is E5B  */
    CLIENT_DIAG_GAL_EPH_SIGNAL_SRC_E5B    = 3,
} clientDiagGalEphSignalSource;

/** Comman Ephemeris Data  */
typedef PACKED struct PACKED_POST {
    /** Specify satellite vehicle ID number.
     *  For SV id range of each supported constellations, refer to
     *  documentation in GnssSv::svId.
     */
    uint16_t gnssSvId;

    /** Specifies the source of ephemeris.
     *  - Type: int32 enum */
    clientDiagGnssEphSource ephSource;

    /** Specifies the delete action of ephemeris.
     *  - Type: int32 enum */
    clientDiagGnssEphAction ephAction;

    /** Issue of data ephemeris used (unit-less).
     *  GPS: IODE 8 bits.
     *  BDS: AODE 5 bits.
     *  GAL: SIS IOD 10 bits.
     *  - Type: uint16
     *  - Units: Unit-less */
    uint16_t IODE;

    /** Square root of semi-major axis.
     * - Type: double
     * - Units: Square Root of Meters */
    double aSqrt;

    /** Mean motion difference from computed value.
     * - Type: double
     * - Units: Radians/Second */
    double deltaN;

    /** Mean anomaly at reference time.
     * - Type: double
     * - Units: Radians */
    double m0;

    /** Eccentricity.
     * - Type: double
     * - Units: Unit-less */
    double eccentricity;

    /** Longitude of ascending node of orbital plane at the weekly epoch.
     * - Type: double
     * - Units: Radians */
    double omega0;

    /** Inclination angle at reference time.
     * - Type: double
     * - Units: Radians */
    double i0;

    /** Argument of Perigee.
     * - Type: double
     * - Units: Radians */
    double omega;

    /** Rate of change of right ascension.
     * - Type: double
     * - Units: Radians/Second */
    double omegaDot;

    /** Rate of change of inclination angle.
     * - Type: double
     * - Units: Radians/Second */
    double iDot;

    /** Amplitude of the cosine harmonic correction term to the argument of latitude.
     * - Type: double
     * - Units: Radians */
    double cUc;

    /** Amplitude of the sine harmonic correction term to the argument of latitude.
     * - Type: double
     * - Units: Radians */
    double cUs;

    /** Amplitude of the cosine harmonic correction term to the orbit radius.
     * - Type: double
     * - Units: Meters */
    double cRc;

    /**  Amplitude of the sine harmonic correction term to the orbit radius.
     * - Type: double
     * - Units: Meters */
    double cRs;

    /** Amplitude of the cosine harmonic correction term to the angle of inclination.
     * - Type: double
     * - Units: Radians */
    double cIc;

    /** Amplitude of the sine harmonic correction term to the angle of inclination.
     * - Type: double
     * - Units: Radians */
    double cIs;

    /** Reference time of ephemeris.
     * - Type: uint32
     * - Units: Seconds */
    uint32_t toe;

    /**  Clock data reference time of week.
     * - Type: uint32
     * - Units: Seconds */
    uint32_t toc;

    /** Clock bias correction coefficient.
     * - Type: double
     * - Units: Seconds */
    double af0;

    /** Clock drift coefficient.
     * - Type: double
     * - Units: Seconds/Second */
    double af1;

    /** Clock drift rate correction coefficient.
     * - Type: double
     * - Units: Seconds/Seconds^2 */
    double af2;
} clientDiagGnssEphCommonStruct;

/** GPS Navigation Model Info */
typedef PACKED struct PACKED_POST {
    /**   Common ephemeris data.   */
    clientDiagGnssEphCommonStruct commonEphemerisData;

    /**   Signal health.
     *   Bit 0 : L5 Signal Health.
     *   Bit 1 : L2 Signal Health.
     *   Bit 2 : L1 Signal Health.
     *   - Type: uint8
     *   - Values: 3 bit mask of signal health, where set bit indicates unhealthy signal */
    uint8_t signalHealth;

    /**   User Range Accuracy Index.
     *   - Type: uint8
     *   - Units: Unit-less */
    uint8_t URAI;

    /**   Indicates which codes are commanded ON for the L2 channel (2-bits).
     *   - Type: uint8
     *   Valid Values:
     *   - 00 : Reserved
     *   - 01 : P code ON
     *   - 10 : C/A code ON */
    uint8_t codeL2;

    /**   L2 P-code indication flag.
     *   - Type: uint8
     *   - Value 1 indicates that the Nav data stream was commanded OFF
     *     on the P-code of the L2 channel. */
    uint8_t dataFlagL2P;

    /**   Time of group delay.
     *   - Type: double
     *   - Units: Seconds */
    double tgd;

    /**  Indicates the curve-fit interval used by the CS.
     *   - Type: uint8
     *   Valid Values:
     *   - 0 : Four hours
     *   - 1 : Greater than four hours */
    uint8_t fitInterval;

    /**   Issue of Data, Clock.
     *   - Type: uint16
     *   - Units: Unit-less */
    uint16_t IODC;
} clientDiagGpsQzssEphemerisStruct;

/** GLONASS Navigation Model Info */
typedef PACKED struct PACKED_POST {

    /** Specify satellite vehicle ID number.
     *  For SV id range of each supported constellations, refer to
     *  documentation in GnssSv::svId.
     */
    uint16_t gnssSvId;

    /** Specifies the source of ephemeris.
     *  - Type: int32 enum */
    clientDiagGnssEphSource ephSource;

    /** Specifies the delete action of ephemeris.
     *  - Type: int32 enum */
    clientDiagGnssEphAction ephAction;

    /**  SV health flags.
     * - Type: uint8
     * Valid Values:
     * - 0 : Healthy
     * - 1 : Unhealthy */
    uint8_t bnHealth;

    /** Ln SV health flags.
     *  - Type: uint8
     *  Valid Values:
     *  - 0 : Healthy
     *  - 1 : Unhealthy */
    uint8_t lnHealth;

    /** Index of a time interval within current day according to UTC(SU) + 03 hours 00 min.
     * - Type: uint8
     * - Units: Unit-less */
    uint8_t tb;

    /** SV accuracy index.
     * - Type: uint8
     * - Units: Unit-less */
    uint8_t ft;

    /** GLONASS-M flag.
     * - Type: uint8
     * Valid Values:
     * - 0 : GLONASS
     * - 1 : GLONASS-M */
    uint8_t gloM;

    /** Characterizes "Age" of current information.
     * - Type: uint8
     * - Units: Days */
    uint8_t enAge;

    /** GLONASS frequency number + 8.
     * - Type: uint8
     * - Range: 1 to 14
     */
    uint8_t gloFrequency;

    /** Time interval between two adjacent values of tb parameter.
     * - Type: uint8
     * - Units: Minutes */
    uint8_t p1;

    /** Flag of oddness ("1") or evenness ("0") of the value of tb
     *  for intervals 30 or 60 minutes.
     *  - Type: uint8 */
    uint8_t p2;

    /** Time difference between navigation RF signal transmitted in L2 sub-band
     *  and aviation RF signal transmitted in L1 sub-band.
     *  - Type: floating point
     *  - Units: Seconds */
    float deltaTau;

    /** Satellite XYZ position.
     * - Type: array of doubles
     * - Units: Meters */
    double position[3];

    /** Satellite XYZ velocity.
     * - Type: array of doubles
     * - Units: Meters/Second */
    double velocity[3];

    /** Satellite XYZ sola-luni acceleration.
     * - Type: array of doubles
     * - Units: Meters/Second^2 */
    double acceleration[3];

    /** Satellite clock correction relative to GLONASS time.
     * - Type: floating point
     * - Units: Seconds */
    float tauN;

    /** Relative deviation of predicted carrier frequency value
     * from nominal value at the instant tb.
     * - Type: floating point
     * - Units: Unit-less */
    float gamma;

    /**<   Complete ephemeris time, including N4, NT and Tb.
       [(N4-1)*1461 + (NT-1)]*86400 + tb*900
       - Type: double
       - Units: Seconds */
    double toe;

    /** Current date, calendar number of day within four-year interval.
     *  Starting from the 1-st of January in a leap year.
     *  - Type: uint16
     *  - Units: Days */
    uint16_t nt;
} clientDiagGloEphemerisStruct;

/** BDS Navigation Model Info */
typedef PACKED struct PACKED_POST {

    /**  Common ephemeris data.   */
    clientDiagGnssEphCommonStruct commonEphemerisData;

    /**  Satellite health information applied to both B1 and B2 (SatH1).
     * - Type: uint8
     * Valid Values:
     * - 0 : Healthy
     * - 1 : Unhealthy */
    uint8_t svHealth;

    /**  Age of data clock.
     * - Type: uint8
     * - Units: Hours */
    uint8_t AODC;

    /** Equipment group delay differential on B1 signal.
     * - Type: double
     * - Units: Nano-Seconds */
    double tgd1;

    /** Equipment group delay differential on B2 signal.
     * - Type: double
     * - Units: Nano-Seconds */
    double tgd2;

    /** User range accuracy index (4-bits).
     * - Type: uint8
     * - Units: Unit-less */
    uint8_t URAI;
} clientDiagBdsEphemerisStruct;

/** GALILEO Navigation Model Info */
typedef PACKED struct PACKED_POST {

    /**  Common ephemeris data. */
    clientDiagGnssEphCommonStruct commonEphemerisData;

    /** Galileo Signal Source.
     *  Valid Values:
     * - GAL_EPH_SIGNAL_SRC_UNKNOWN (0) --  GALILEO signal is unknown
     * - GAL_EPH_SIGNAL_SRC_E1B (1) --  GALILEO signal is E1B
     * - GAL_EPH_SIGNAL_SRC_E5A (2) --  GALILEO signal is E5A
     * - GAL_EPH_SIGNAL_SRC_E5B (3) --  GALILEO signal is E5B  */
    clientDiagGalEphSignalSource dataSourceSignal;

    /**  Signal-in-space index for dual frequency E1-E5b/E5a depending on dataSignalSource.
     * - Type: uint8
     * - Units: Unit-less */
    uint8_t sisIndex;

    /** E1-E5a Broadcast group delay from F/Nav (E5A).
     * - Type: double
     * - Units: Seconds */
    double bgdE1E5a;

    /**  E1-E5b Broadcast group delay from I/Nav (E1B or E5B).
     * For E1B or E5B signal, both bgdE1E5a and bgdE1E5b are valid.
     * For E5A signal, only bgdE1E5a is valid.
     * Signal source identified using dataSignalSource.
     * - Type: double
     * - Units: Seconds */
    double bgdE1E5b;

    /** SV health status of signal identified by dataSourceSignal.
     * - Type: uint8
     * Valid Values:
     * - 0 : Healthy
     * - 1 : Unhealthy */
    uint8_t svHealth;

} clientDiagGalEphemerisStruct;

/** QZSS Navigation Model */
typedef PACKED struct PACKED_POST {

    /** GPS Ephemeris structure */
    clientDiagGpsQzssEphemerisStruct qzssEphData;

} clientDiagQzssEphemerisStruct;

/** NAVIC Navigation Model Info */
typedef PACKED struct PACKED_POST {

    /** Common ephemeris data. */
    clientDiagGnssEphCommonStruct commonEphemerisData;

    /** Week number since the NavIC system time start epoch (August 22, 1999) */
    uint32_t weekNum;
    /** Issue of Data, Clock
     *  Mandatory Field */
    uint32_t iodec;
    /** Health status of navigation data on L5 SPS signal.
     *  0=OK, 1=bad */
    uint8_t l5Health;
    /** Health status of navigation data on S SPS signal.
     *  0=OK, 1=bad */
    uint8_t sHealth;
    /** Inclination angle at reference time
     *  Unit: radian
     *  Mandatory Field */
    double inclinationAngleRad;
    /** User Range Accuracy Index(4bit)
        Mandatory Field */
    uint8_t urai;
    /** Time of Group delay
     *  Unit: second
     *  Mandatory Field */
    double  tgd;
} clientDiagNavicEphemerisStruct;

/** GNSS Ephemeris Information */
typedef PACKED struct PACKED_POST {
    /** Used by Logging Module
      *  Mandatory field */
    log_hdr_type logHeader;
    /** clientDiag Message Version
     *  Mandatory field */
    uint8 version;
    /** Indicates GNSS Constellation Type
     *  Mandatory field */
    clientDiagGnss_LocSvSystemEnumType gnssConstellation;

    /** Validity of GPS System Time of the ephemeris report */
    bool isSystemTimeValid;

    /** GPS System Time of the ephemeris report */
    clientDiagGnssSystemTimeStructType systemTime;

    /** Number of satellites in ephemeris Report */
    uint8_t numberOfEphemeris;

    union {
        /** Ephemeris Data for each GPS SV */
        clientDiagGpsQzssEphemerisStruct gpsEphemerisData[CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE];

        /** Ephemeris Data for each GLONASS SV */
        clientDiagGloEphemerisStruct gloEphemerisData[CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE];

        /** Ephemeris Data for each BDS SV */
        clientDiagBdsEphemerisStruct bdsEphemerisData[CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE];

        /** Ephemeris Data for each GAL SV */
        clientDiagGalEphemerisStruct galEphemerisData[CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE];

        /** Ephemeris Data for each QZSS SV */
        clientDiagQzssEphemerisStruct qzssEphemerisData[CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE];

        /** Ephemeris Data for each NavIC SV */
        clientDiagNavicEphemerisStruct navicEphemerisData[CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE];
    } ephInfo;
} clientDiagGnssEphReportStructType;


class LocationClientApiDiag {
protected:
    LocDiagIface* mDiagIface;

    LocationClientApiDiag();
    void translateDiagGnssLocationPositionDynamics(clientDiagGnssLocationPositionDynamics& out,
            const GnssLocationPositionDynamics& in);
    clientDiagGnssSystemTimeStructType parseDiagGnssTime(
            const GnssSystemTimeStructType &halGnssTime);
    clientDiagGnssGloTimeStructType parseDiagGloTime(const GnssGloTimeStructType &halGloTime);
    void translateDiagSystemTime(clientDiagGnssSystemTime& out, const GnssSystemTime& in);
    clientDiagGnssLocationSvUsedInPosition parseDiagLocationSvUsedInPosition(
            const GnssLocationSvUsedInPosition &halSv);
    void translateDiagGnssSignalType(clientDiagGnssSignalTypeMask& out, GnssSignalTypeMask in);
    clientDiagGnss_LocSvSystemEnumType parseDiagGnssConstellation(
            Gnss_LocSvSystemEnumType gnssConstellation);
    void translateDiagGnssMeasUsageInfo(clientDiagGnssMeasUsageInfo& out,
            const GnssMeasUsageInfo& in);
    void populateClientDiagLocation(clientDiagGnssLocationStructType* diagGnssLocPtr,
            const GnssLocation& gnssLocation, const DiagLocationInfoExt & diagLocationInfoExt);
    void populateClientDiagMeasurements(clientDiagGnssMeasurementsStructType* diagGnssMeasPtr,
            const GnssMeasurements& gnssMeasurements);
    void translateDiagGnssSv(clientDiagGnssSv& out, const GnssSv& in);
    void populateClientDiagGnssSv(clientDiagGnssSvStructType* diagGnssSvPtr,
            const std::vector<GnssSv>& gnssSvs);
    void populateClientDiagGeofenceBreach(clientDiagGeofenceBreachStructType* diagGeofenceBreachPtr,
            const GeofenceBreachNotification& gfBreachNotif,
            const std::vector<Geofence>& geofences);
    void parseLocation(const ::Location &halLocation, clientDiagGnssLocationStructType& location);
    clientDiagGeofenceBreachTypeMask parseGeofenceBreachType(
            ::GeofenceBreachTypeMask halBreachType);
    void translateDiagGeofenceData(clientDiagGeofenceData& out, const Geofence& in);

    // Utility functions to fill Ephemris Report
    void populateCommanGnssEph(clientDiagGnssEphCommonStruct& out,
            const GnssEphCommonInfo& in);
    void populateGpsEph(clientDiagGpsQzssEphemerisStruct& out,
        const GpsQzssEphemeris& in);
    void populateGalEph(clientDiagGalEphemerisStruct& out,
        const GalileoEphemeris& in);
    void populateGloEph(clientDiagGloEphemerisStruct& out,
        const GlonassEphemeris& in);
    void populateBdsEph(clientDiagBdsEphemerisStruct& out,
            const BdsEphemeris& in);
    void  populateQzssEph(clientDiagQzssEphemerisStruct& out,
            const QzssEphemeris& in);
    void populateNavicEph(clientDiagNavicEphemerisStruct& out,
        const NavicEphemeris& in);
    void populateEphemerisReport(clientDiagGnssEphReportStructType* diagEphReport,
            const GnssEphemeris& ephReport);
};

class GnssLocationReport : public LocLoggerBase, public LocationClientApiDiag {
public:
    GnssLocationReport(const GnssLocation& gnssLocation,
                       const DiagLocationInfoExt & diagLocationInfoExt):
        mGnssLocation(gnssLocation),
        mDiagLocationInfoExt(diagLocationInfoExt) {}
    virtual ~GnssLocationReport() = default;
    const GnssLocation& mGnssLocation;
    const DiagLocationInfoExt& mDiagLocationInfoExt;

    virtual void log() override;
};

class GnssSvReport : public LocLoggerBase, public LocationClientApiDiag {
public:
    GnssSvReport(const std::vector<GnssSv> &gnssSvsVector): mGnssSvsVector(gnssSvsVector),
            LocLoggerBase(),
            LocationClientApiDiag() {}
    virtual ~GnssSvReport() = default;
    const std::vector<GnssSv> & mGnssSvsVector;
    virtual void log() override;
};

class GeofenceBreachDiagReport : public LocLoggerBase, public LocationClientApiDiag {
public:
    GeofenceBreachDiagReport(const GeofenceBreachNotification& breachNotif,
            const std::vector<Geofence> &geofences):
            mGeofenceBreachNotif(breachNotif), mGfVector(geofences) {}
    virtual ~GeofenceBreachDiagReport() = default;
    const GeofenceBreachNotification& mGeofenceBreachNotif;
    const std::vector<Geofence> & mGfVector;

    virtual void log() override;
};

class GnssNmeaReport : public LocLoggerBase, public LocationClientApiDiag {
public:
    GnssNmeaReport(uint64_t timestamp, uint32_t length, const char* nmea,
                   LocOutputEngineType engType): mTimestamp(timestamp), mLength(length),
                   mNmea(nmea), mEngType(engType), LocLoggerBase(),
            LocationClientApiDiag() {}
    virtual ~GnssNmeaReport() = default;
    uint64_t mTimestamp;
    uint32_t mLength;
    const char* mNmea;
    LocOutputEngineType mEngType;
    virtual void log() override;
};

class GnssMeasReport : public LocLoggerBase, public LocationClientApiDiag {
public:
    GnssMeasReport(const GnssMeasurements& gnssMeasurements):
            mGnssMeasurements(gnssMeasurements),
            LocLoggerBase(),
            LocationClientApiDiag() {}
    virtual ~GnssMeasReport() = default;
    const GnssMeasurements& mGnssMeasurements;
    virtual void log() override;
};

class GnssLatencyReport : public LocLoggerBase, public LocationClientApiDiag {
public:
    GnssLatencyReport(const GnssLatencyInfo& gnssLatencyInfo):
        mGnssLatencyInfo(gnssLatencyInfo),
        LocLoggerBase(),
        LocationClientApiDiag() {}
    virtual ~GnssLatencyReport() = default;
    const GnssLatencyInfo& mGnssLatencyInfo;
    virtual void log() override;
};

class GnssDiagDcReport : public LocLoggerBase, public LocationClientApiDiag {
public:
    GnssDiagDcReport(const GnssDcReport& dcReport):
        mGnssDcReport(dcReport),
        LocLoggerBase(),
        LocationClientApiDiag() {}
    virtual ~GnssDiagDcReport() = default;
    const GnssDcReport& mGnssDcReport;
    virtual void log() override;
};

class GnssDiagEphReport : public LocLoggerBase, public LocationClientApiDiag {
public:
    GnssDiagEphReport(const GnssEphemeris& ephReport):
        mGnssEphReport(ephReport),
        LocLoggerBase(),
        LocationClientApiDiag() {}
    virtual ~GnssDiagEphReport() = default;
    const GnssEphemeris& mGnssEphReport;
    virtual void log() override;
};

}

#endif
