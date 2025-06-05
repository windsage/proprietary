/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  WiFi DB Receiver Types

  Copyright (c) 2017-2018, 2020-2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_WIFIDBRECV_H__
#define __IZAT_MANAGER_WIFIDBRECV_H__

#include <vector>
#include <string>
#include "IzatRemoteApi.h"
#include <queue>
#include <IzatTypes.h>

#define AP_SSID_PREFIX_LENGTH            8

namespace izat_remote_api {

class WiFiDBUpdater : public IzatNotifier {
    // apListUpdate() status
    static const int E_STD_CONT   = 0;  // more AP list to follow
    static const int E_STD_FINAL  = 1;  // final AP list
    static const int E_SCAN_FINAL = 2;  // final scan list
    static const int E_LOOKUP     = 3;  // lookup request

    // Fdal status
    static const int E_NOT_IN_FDAL = 0;  // AP not in FDAL cache
    static const int E_IN_FDAL     = 1;  // AP already in FDAL cache
    static const int E_BLACKLISTED = 2;  // AP is blacklisted

    //Destination queue of AP Look up result
    std::queue<std::string> mDesQueue;

protected:
    inline WiFiDBUpdater() : IzatNotifier(sName, nullptr) {}
    virtual inline ~WiFiDBUpdater() = default;
public:
    static const char sName[];
    class APInfo;
    class APLocationData;
    class APSpecialInfo;
    virtual void handleMsg(qc_loc_fw::InPostcard *const in_card) final;
    void pushWiFiDB(std::vector<APLocationData>*,
                    std::vector<APSpecialInfo>*,
                    int daysValid, bool is_lookup);
    void reportNlpApiConnected();
    virtual void statusUpdate(bool isSuccess, const char* errStr) = 0;
    virtual void apListUpdate(std::vector<APInfo>*,
                              const int apListStatus,
                              const UlpLocation& ulpLoc,
                              const bool bUlpLocValid) = 0;
    virtual void serviceRequest() = 0;
    virtual void notifyCallbackEnv() = 0;
};

class WiFiDBUpdater::APInfo {
public:
    // Full 48 bits of the Mac Address in reverse order represented
    // in 64 bit format
    uint64_t mac_R48b;

    // Type of cell in which this Mac Address is observerd atleast
    // once, if available
    // 0: unknown, 1: GSM, 2: WCDMA, 3: CDMA, 4: LTE, 5: NR
    uint8_t  cellType;

    // Cell ID if known
    // Cell RegionID1 Mobile Country Code(MCC), For CDMA Set to 0
    uint16_t cellRegionID1;
    // Cell RegionID2 Mobile Network Code(MNC), For CDMA set to
    // System ID(SID), For WCDMA set to 0 if not available
    uint16_t cellRegionID2;
    // Cell RegionID3 GSM: Local Area Code(LAC), WCDMA: Local Area
    // Code(LAC) set to 0 if not available, CDMA: Network ID(NID),
    // LTE: Tracking Area Code(TAC) set to 0 if not available
    uint16_t cellRegionID3;
    // Cell RegionID4 Cell ID(CID), For CDMA set to Base Station
    // ID(BSID)
    uint32_t cellRegionID4;

    // SSID information. First 8 charcters of SSID
    char ssid[AP_SSID_PREFIX_LENGTH];
    uint8_t ssid_valid_byte_count;

    // UTC time last observed (0 = invalid/unavailable)
    uint64_t utcTime;
    // Status in FDAL
    uint8_t fdal_status;

    inline APInfo() : mac_R48b(0)
        , cellType(0)
        , cellRegionID1(0)
        , cellRegionID2(0)
        , cellRegionID3(0)
        , cellRegionID4(0)
        , ssid { 0}
        , ssid_valid_byte_count(0)
        , utcTime(0)
        , fdal_status(0) {
    }
};

typedef enum {
    QUALITY_UNKNOWN = 0,
    VERY_LOW_ACCURACY,
    LOW_ACCURACY,
    HIGH_ACCURACY
} ApPositionQuality;

typedef enum {
    NOT_SUPPORTED = 0,
    TWO_SIDED
} WiFiRttCapability;

class WiFiDBUpdater::APLocationData {
public:
    // Full 48 bits of the Mac Address in reverse order represented
    // in 64 bit format
    uint64_t mac_R48b;

    float latitude;
    float longitude;
    float max_antena_range;
    float horizontal_error;
    uint8_t reliability;
    int valid_mask;

    float altitude;
    izat_manager::IzatAltitudeRefType altRefType;
    ApPositionQuality positionQuality;
    WiFiRttCapability rttCapability;
    float rttRangeBiasInMm;

    static const int WIFIDBUPDATER_APLOC_WITH_LAT_LON = 0x1;
    static const int WIFIDBUPDATER_APLOC_MAR_VALID = 0x2;
    static const int WIFIDBUPDATER_APLOC_HORIZONTAL_ERR_VALID = 0x4;
    static const int WIFIDBUPDATER_APLOC_RELIABILITY_VALID = 0x8;
    static const int WIFIDBUPDATER_APLOC_ALTITUDE_VALID = 0x10;
    static const int WIFIDBUPDATER_APLOC_POSITION_QUALITY_VALID = 0x20;
    static const int WIFIDBUPDATER_APLOC_RTT_CAPABILITY_VALID = 0x40;
    static const int WIFIDBUPDATER_APLOC_RTT_BIAS_VALID = 0x80;

    inline APLocationData(): mac_R48b(0),
        latitude(0),
        longitude(0),
        max_antena_range(0),
        horizontal_error(0),
        reliability(0),
        valid_mask(0),
        altitude(0),
        altRefType(izat_manager::ALT_UNKNOWN),
        positionQuality(QUALITY_UNKNOWN),
        rttCapability(NOT_SUPPORTED),
        rttRangeBiasInMm(0) {
    }
};
class WiFiDBUpdater::APSpecialInfo {
public:
    // Full 48 bits of the Mac Address in reverse order represented
    // in 64 bit format
    uint64_t mac_R48b;

    // Special Information on this AP
    // 0: AP information not available
    // 1: AP detected as a moving AP
    uint8_t info;

    inline APSpecialInfo(): mac_R48b(0), info(0) {
    }
};

} //izat_remote_api

#endif // #ifndef __IZAT_MANAGER_WIFIDBRECV_H__
