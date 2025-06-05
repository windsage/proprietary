/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IZAT_API_V02_H
#define IZAT_API_V02_H

#include <LocApiV02.h>
#include <IzatApiBase.h>
#include <vector>
#include <LocTimer.h>

namespace lbs_core {
    class LocApiProxyV02;
};

using namespace loc_core;
using namespace lbs_core;

namespace izat_core {

class IzatApiV02 : public IzatApiBase {

public:
    IzatApiV02(LocApiProxyV02* locApiProxy);
    virtual ~IzatApiV02();

    void sendMsg(const LocMsg* msg);

    inline virtual void* getSibling() { return (IzatApiBase*)this; }

    /* event callback registered with the loc_api v02 interface */
    virtual int eventCb(locClientHandleType client_handle,
                         uint32_t loc_event_id,
                         locClientEventIndUnionType loc_event_payload);
    double getGloRfLoss(uint32_t left, uint32_t center,
                        uint32_t right, uint8_t gloFrequency);
    bool getSatellitePVT(GnssSvPolynomial& svPolynomial,
                         GnssSvMeasurementHeader& svMeasSetHeader,
                         GnssMeasurementsData& measurementData);
    float getGeoidalSeparation(double latitude, double longitude);

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

    virtual void sendGtpApStatus(int32_t gtpApDbStatus, uint64_t gtpApPcid64, int8_t gtpApPcid64_valid,
                                 const char* oemId, int8_t oemId_valid,
                                 const char* modelId, int8_t modelId_valid,
                                 LocApiResponse* adapterResponse);

    virtual void sendGdtDownloadBeginResponse(int32_t service, uint32_t session, int32_t status,
                                              const uint8_t* respLocInfo, int8_t respLocInfo_valid,
                                              uint32_t respLocInfo_len, uint32_t remainingThrottleTime,
                                              int8_t remainingThrottleTime_valid,
                                              LocApiResponse* adapterResponse);
    virtual void sendGdtDownloadReadyStatus(int32_t service, uint32_t session, int32_t status,
                                            const char* filePath,
                                            LocApiResponse* adapterResponse);
    virtual void sendGdtReceiveDoneResponse(int32_t service, uint32_t session, int32_t status,
                                            LocApiResponse* adapterResponse);
    virtual void sendGdtDownloadEndResponse(int32_t service, uint32_t session, int32_t status,
                                            LocApiResponse* adapterResponse);

    // XTRA Client 2.0
    virtual void injectIzatPcid(uint64_t pcid);
    virtual void injectXtraData_legacy(const char* data, uint32_t len);
    virtual void injectXtraData(const char* data, uint32_t len,
                                bool legacyRetry, LocApiResponse* adapterResponse,
                                uint32_t type, uint32_t interval);
    virtual void injectNtpTime(const uint64_t time, const uint64_t reference,
                               const uint64_t uncertainty,
                               enum XtraTimeSource timeSource);
    virtual void requestXtraServers(const uint32_t reqSource);
    virtual void requestXtraConfigInfo(const uint32_t configMask, const uint32_t configReqSource,
                                       const uint8_t xcMajorVersion, const uint8_t xcMinorVersion);

    //Feature status check
    bool checkFeatureStatus(int* fids, LocFeatureStatus* status, uint32_t idCount,
            bool directQwesCall = false);
private:

    void onGdtUploadBeginEvent(const qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02* pInfo);
    void onGdtUploadEndEvent(const qmiLocEventGdtUploadEndReqIndMsgT_v02* pInfo);
    void onGdtDownloadBeginEvent(const qmiLocEventGdtDownloadBeginReqIndMsgT_v02* pInfo);
    void onGdtReceiveDoneEvent(const qmiLocEventGdtReceiveDoneIndMsgT_v02* pInfo);
    void onGdtDownloadEndEvent(const qmiLocEventGdtDownloadEndReqIndMsgT_v02* pInfo);
    void onQueryXtraInfoReqEvent(const qmiLocEventQueryXtraInfoReqIndMsgT_v02* pInfo);

    locClientStatusEnumType locSyncSendReq(uint32_t req_id, locClientReqUnionType req_payload,
            uint32_t timeout_msec, uint32_t ind_id, void* ind_payload_ptr);
    locClientStatusEnumType locClientSendReq(uint32_t req_id, locClientReqUnionType req_payload);

    // Utils
    static qmiLocPowerModeEnumT_v02 convertPowerMode(GnssPowerMode powerMode);

    class IzatApiTimer : public LocTimer {
        IzatApiV02& mClient;
    public:
        inline IzatApiTimer(IzatApiV02& client) : LocTimer(), mClient(client) {}
        virtual void timeOutCallback();
    };

    IzatApiTimer mTimer;
};

}  // namespace izat_core

#endif //IZAT_API_V02_H
