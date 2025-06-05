/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IZAT_API_BASE_H
#define IZAT_API_BASE_H

#include <WiperData.h>
#include <LocApiBase.h>
#include <ContextBase.h>
#include <LBSApiBase.h>
#include <LocApiProxy.h>

using namespace loc_core;
using namespace lbs_core;

namespace izat_core {

enum IzatApiReturnCode {
    IZAT_SUCCESS,
    IZAT_FAIL,
    IZAT_INVALID_ARGUMENTS,
    IZAT_GPS_LOCKED,
    IZAT_FAIL_UNSUPPORTED
};

enum XtraTimeSource {
    XTRA_TIME_SOURCE_UNKNOWN = 0,
    XTRA_TIME_SOURCE_AP,
    XTRA_TIME_SOURCE_NTP,
    XTRA_TIME_SOURCE_NTS
};

#define XTRA_MP_CONFIG_DISABLE_AUDO_DOWNLOD_TIMER (0x00000001)
#define XTRA_MP_CONFIG_NAVIC_EPH_ASSIST (0x00000002)
 /**<  Precise Xtra Enabled support from Client */
#define XTRA_MP_CONFIG_PRECISE_XTRA_ENABLED (0x00000004)

 /**<  How many hours that the current XTRA information is valid  */
#define XTRA_INFO_MASK_ABS_AGE (0x00000001)
 /**<  Last XTRA data download time  */
#define XTRA_INFO_MASK_REL_AGE (0x00000002)
  /**<  XTRA server URLs  */
#define XTRA_INFO_MASK_XTRA_SERVER (0x00000004)
  /**<  Network Time Protocol(NTP) server URLs  */
#define XTRA_INFO_MASK_NTP_SERVER (0x00000008)
  /**<  Network Time Protocol(NTP) server URLs  */
#define XTRA_INFO_MASK_TIME_REQUEST (0x00000010)
  /**<  Network Time Protocol(NTP) server URLs  */
#define XTRA_INFO_MASK_PREF_VALID_AGE (0x00000020)
  /**<  NAVIC eph assist supported  */
#define XTRA_INFO_MASK_NAVIC_EPH_ASSIST (0x00000040)
  /**<  XTRA disablement  */
#define XTRA_INFO_MASK_XTRA_DISABLED   (0x00000080)
  /**< Precise Xtra support enable status from Modem */
#define XTRA_INFO_MASK_PRECISE_XTRA_ENABLED (0x00000100)

  /**<  Request for injection of predicted orbits */
#define XTRA_REQ_MASK_INJECT_REQUEST (0x00000001)
  /**<  Update server list */
#define XTRA_REQ_MASK_SERVER_UPDATE (0x00000002)
  /**<  Refresh periodicity of injection  */
#define XTRA_REQ_MASK_REFRESH_UPDATE_RATE (0x00000004)

  /**<  Injected orbit rate update */
#define XTRA_RATE_UPDATE_INJECTED_ORBIT (1)
  /**<  Integrity rate update  */
#define XTRA_RATE_UPDATE_INTEGRITY (2)

#define XTRA_FILE_TYPE_DATA (1) /**<  XTRA file */
#define XTRA_FILE_TYPE_NAVIC (2) /**<  NavIC eph file */

 /*XC4.0 Bit 0 NAVIC Used for setting Usable/forced Enable bit Mask*/
#define XTRA_GNSS_CONSTELLATION_NAVIC    (1 << 0)
 /*XC4.0 Bit 1 QZSS  Used for setting Usable/forced Enable bit Mask*/
#define XTRA_GNSS_CONSTELLATION_QZSS     (1 << 1)
 /*XC4.0 Bit 2 GAL   Used for setting Usable/forced Enable bit Mask*/
#define XTRA_GNSS_CONSTELLATION_GAL      (1 << 2)
 /*XC4.0 Bit 3 BDS3  Used for setting Usable/forced Enable bit Mask*/
#define XTRA_GNSS_CONSTELLATION_BDS3     (1 << 3)
 /*XC4.0 Bit 4 GLO   Used for setting Usable/forced Enable bit Mask*/
#define XTRA_GNSS_CONSTELLATION_GLO      (1 << 4)
 /*XC4.0 Bit 5 GPS   Used for setting Usable/forced Enable bit Mask*/
#define XTRA_GNSS_CONSTELLATION_GPS      (1 << 5)

class IzatAdapterBase;

class IzatApiBase {
    IzatAdapterBase* mIzatAdapters[MAX_ADAPTERS];
    static bool mIsXtraInitialized;
protected:
    LocApiProxyBase *mLocApiProxy;
    std::unordered_map<int, LocFeatureStatus> mQwesStatusMap;
public:

    IzatApiBase(LocApiProxyBase *locApiProxy);
    virtual ~IzatApiBase();

    void addAdapter(IzatAdapterBase* adapter);
    void removeAdapter(IzatAdapterBase* adapter);

    // set user preference
    virtual void setUserPref(bool user_pref);
    virtual void sendBestAvailablePosReq(LocApiResponseData<LocGpsLocation>* adapterResponseData);
    virtual void sendBestAvailablePosReq(LocPosTechMask &tech_mask,
                                         LocApiResponseData<LocGpsLocation>* adapterResponseData);

    // for TDP
    virtual void sendGtpEnhancedCellConfigBlocking(e_premium_mode mode,
                                                   LocApiResponse* adapterResponse);
    virtual void sendGtpCellConfigurationsBlocking(const char* data, uint32_t len,
                                                   LocApiResponse* adapterResponse);

    virtual void sendGdtUploadBeginResponse(int32_t service, int32_t session, int32_t status,
                                            LocApiResponse* adapterResponse);
    virtual void sendGdtUploadEndResponse(int32_t service, int32_t session, int32_t status,
                                          LocApiResponse* adapterResponse);

    virtual void sendGtpApStatus(int32_t gtpApDbStatus, uint64_t gtpApPcid64,
                                 int8_t gtpApPcid64_valid,
                                 const char* oemId, int8_t oemId_valid,
                                 const char* modelId, int8_t modelId_valid,
                                 LocApiResponse* adapterResponse);
    virtual void sendGdtDownloadBeginResponse(int32_t service, uint32_t session, int32_t status,
                                              const uint8_t* respLocInfo, int8_t respLocInfo_valid,
                                              uint32_t respLocInfo_len,
                                              uint32_t remainingThrottleTime,
                                              int8_t remainingThrottleTime_valid,
                                              LocApiResponse* adapterResponse);
    virtual void sendGdtDownloadReadyStatus(int32_t service, uint32_t session, int32_t status,
                                            const char* filePath,
                                            LocApiResponse* adapterResponse);
    virtual void sendGdtReceiveDoneResponse(int32_t service, uint32_t session, int32_t status,
                                            LocApiResponse* adapterResponse);
    virtual void sendGdtDownloadEndResponse(int32_t service, uint32_t session, int32_t status,
                                            LocApiResponse* adapterResponse);

    // XTRA Client
    virtual void injectIzatPcid(uint64_t pcid);
    virtual void injectXtraData_legacy(const char* data, uint32_t len);
    virtual void injectXtraData(const char* data, uint32_t len, bool legacyRetry,
                                LocApiResponse* adapterResponse, uint32_t type = 0,
                                uint32_t interval = 0);
    virtual void injectNtpTime(const uint64_t time, const uint64_t reference,
                               const uint64_t uncertainty, enum XtraTimeSource  timeSource);
    virtual void requestXtraServers(const uint32_t reqSource);
    virtual void requestXtraConfigInfo(const uint32_t configMask, const uint32_t configReqSource,
                                       const uint8_t xcMajorVersion, const uint8_t xcMinorVersion);

protected:

    void handleGdtUploadBeginEvent(int32_t service,
                                   int32_t session,
                                   const char* filePath,
                                   uint32_t filePath_len);
    void handleGdtUploadEndEvent(int32_t service, int32_t session, int32_t status);
    void handleGtpApStatusResp(int32_t status, uint16_t clientSwVer,
                               uint8_t asnMajVer, uint8_t asnMinVer, uint8_t asnPtVer);
    void handleGdtDownloadBeginEvent(int32_t service, uint32_t session, uint32_t respTimeoutInterval,
                                     const uint8_t* clientInfo, uint32_t clientInfo_len,
                                     const uint8_t* mobileStatusData, uint32_t mobileStatusData_len,
                                     const char* filePath, uint32_t filePath_len,
                                     uint32_t powerBudgetInfo, int8_t powerBudgetInfo_valid,
                                     uint32_t powerBudgetAllowance, int8_t powerBudgetAllowance_valid,
                                     uint32_t downloadRequestMask, int8_t downloadRequestMask_valid);
    void handleGdtReceiveDoneEvent(int32_t service, uint32_t session, int32_t status);
    void handleGdtDownloadEndEvent(int32_t service, uint32_t session, int32_t status);

    // XTRA Client
    void handleReceiveXtraServers(const char* server1, const char* server2, const char* server3,
                                  const uint32_t reqSource, uint8_t serverUpdateMask_valid = 0,
                                  uint64_t serverUpdateMask = 0, uint8_t updateType_valid = 0,
                                  uint32_t updateType = 0, uint8_t updateRate_valid = 0,
                                  uint32_t updateRate = 0, uint8_t fileInfo_valid = 0,
                                  uint32_t fileType = 0, uint32_t downloadInterval = 0);
    void handleRequestNtpTime();
    void handleReceiveXtraConfigInfo(uint32_t status, uint32 configReqSource,
                                     uint32_t report_mask, uint16_t absAge,
                                     uint64_t relAge, uint8_t timereqValid,
                                     uint16_t prefValidAgeHrs,
                                     const char* ntpServer1, const char* ntpServer2,
                                     const char* ntpServer3, const char* xtraServer1,
                                     const char* xtraServer2, const char* xtraServer3,
                                     uint8_t mpMajorVersion, uint8_t mpMinorVersion,
                                     uint32_t useableConstellations,
                                     uint32_t forcedConstellations,
                                     uint8_t mbEnabled, uint8_t xtraFileMajorVersion,
                                     uint8_t xtraFileMinorVersion,
                                     uint16_t primaryMcc, uint16_t secondaryMcc,
                                     uint32_t xtraRemValidity);

    void handleQueryXtraInfoReqEvent(uint32_t useableConstellations,
                                         uint32_t forcedConstellations,
                                         uint8_t mbEnabled, uint8_t xtraFileMajorVersion,
                                         uint8_t xtraFileMinorVersion,
                                         uint16_t primaryMcc, uint16_t secondaryMcc,
                                         uint32_t xtraRemValidity, uint8_t preciseXtraStatus);
    void handleReceiveXtraInjectionStatus(const int32_t status,
                                          const uint64_t constellationMask);
    uint32_t getCarrierCapabilities();
};

}  // namespace izat_core

#endif //IZAT_API_BASE_H
