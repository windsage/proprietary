/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  WiFi DB Provider Types

  Copyright (c) 2018, 2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_WIFIDBPROV_H__
#define __IZAT_MANAGER_WIFIDBPROV_H__

#include <vector>
#include <gps_extended.h>
#include "IzatRemoteApi.h"

#define AP_SSID_LENGTH            33

namespace izat_remote_api {

class WiFiDBUploader: public IzatNotifier {

    // apListUpdate() status
    static const int E_STD_CONT   = 0;  // more FDAL AP list to follow
    static const int E_STD_FINAL  = 1;  // final FDAL AP list
    static const int E_SCAN_FINAL = 2;  // final scan list

protected:
    inline WiFiDBUploader() : IzatNotifier(sName, nullptr) {}
    virtual inline ~WiFiDBUploader() = default;
public:
    static const char sName[];
    class APScanInfo;
    class APCellInfo;
    class APRangingMeasurement;
    class APRttScanInfo;
    class APObsData;
    virtual void handleMsg(qc_loc_fw::InPostcard *const in_card) final;
    void sendAPOBSLocDataReq();
    virtual void onAPObsLocDataAvailable(std::vector<APObsData>*,
                              const int apListStatus) = 0;
    virtual void serviceRequest() = 0;
    virtual void notifyCallbackEnv() = 0;
};

typedef enum {
    BANDWIDTH_20MHZ   = 0,
    BANDWIDTH_40MHZ   = 1,
    BANDWIDTH_80MHZ   = 2,
    BANDWIDTH_160MHZ  = 3,
    BANDWIDTH_320MHZ  = 4
} APRangingBandwidth;

typedef enum {
    AP_SERVICE_UNKNOWN = 0,
    AP_SERVING         = 1,
    AP_NOT_SERVING     = 2
} APServiceStatus;

class WiFiDBUploader::APScanInfo {
public:
    // Full 48 bits of the Mac Address in reverse order represented
    // in 64 bit format
    uint64_t mac_R48b;

    //Signal strength in 0.5dBm
    int16_t rssi;

    //Measurement age msec
    uint64_t age_usec;

    //channel id
    int channel_id;

    // SSID information. First 32 charcters of SSID
    char ssid[AP_SSID_LENGTH];
    uint8_t ssid_valid_byte_count;

    APServiceStatus serviceStatus;
    uint32_t frequency;
    APRangingBandwidth bandwidth;

    inline APScanInfo() : mac_R48b(0),
        rssi(0),
        age_usec(0),
        channel_id(0),
        ssid {0},
        ssid_valid_byte_count(0),
        serviceStatus(AP_SERVICE_UNKNOWN),
        frequency(0),
        bandwidth(BANDWIDTH_20MHZ) {
    }
};

class WiFiDBUploader::APRangingMeasurement {
public:
    uint32_t mDistanceInMm;
    int16_t mRssi;
    APRangingBandwidth mTxBandwidth;
    APRangingBandwidth mRxBandwidth;
    int16_t mChainNumber;

    inline APRangingMeasurement(uint32_t distance, int16_t rssi,
            APRangingBandwidth txBandwidth, APRangingBandwidth rxBandwidth, int16_t chainNumber) :
        mDistanceInMm(distance),
        mRssi(rssi),
        mTxBandwidth(txBandwidth),
        mRxBandwidth(rxBandwidth),
        mChainNumber(chainNumber) {}
};

class WiFiDBUploader::APRttScanInfo {
public:
    uint64_t mMacR48b;
    uint16_t mNumAttempted;
    uint64_t mAgeInUsec;
    std::vector<APRangingMeasurement> mRttMeas;

    inline APRttScanInfo(uint64_t macR48b, uint16_t numAttempted, uint64_t ageInUsec,
            const std::vector<APRangingMeasurement>& rttMeas):
        mMacR48b(macR48b),
        mNumAttempted(numAttempted),
        mAgeInUsec(ageInUsec),
        mRttMeas(rttMeas) {}
};

class WiFiDBUploader::APCellInfo {
public:
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
    uint32_t cellRegionID3;
    // Cell RegionID4 Cell ID(CID), For CDMA set to Base Station
    // ID(BSID)
    // Global Cell ID (64-bit for NR, and 32-bit for WCDMA and LTE)
    uint64_t cellRegionID4;
    // ARFCN, or UNAVAILABLE
    uint32_t frequency;
    // physical id: PSC (UMTS), PCI (LTE/NR), or UNAVAILABLE
    uint32_t physicalId;

    inline APCellInfo() : cellType(0)
        , cellRegionID1(0)
        , cellRegionID2(0)
        , cellRegionID3(UINT32_MAX)
        , cellRegionID4(0)
        , frequency(UINT32_MAX)
        , physicalId(UINT32_MAX) {}
};

class WiFiDBUploader::APObsData {
public:
    std::vector<APScanInfo> ap_scan_info;
    std::vector<APRttScanInfo> ap_rtt_scan_info;
    uint64_t scanTimestamp_ms;
    UlpLocation gpsLoc;
    bool bUlpLocValid;
    APCellInfo cellInfo;

    inline APObsData(): ap_scan_info{0}
        ,scanTimestamp_ms(0)
        ,gpsLoc()
        ,bUlpLocValid(0)
        ,cellInfo() {
    }
};

} //izat_remote_api

#endif // #ifndef __IZAT_MANAGER_WIFIDBPROV_H__
