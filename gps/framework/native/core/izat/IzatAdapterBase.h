/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef IZAT_ADAPTER_BASE_H
#define IZAT_ADAPTER_BASE_H

#include <stdbool.h>
#include <string.h>
#include <LocAdapterBase.h>
#include <LocAdapterProxyBase.h>

#ifdef __cplusplus
extern "C"
{
#endif

using namespace loc_core;

namespace izat_core {

class IzatApiBase;

class IzatAdapterBase : public LocAdapterProxyBase {
protected:
    IzatApiBase* mIzatApi;
    IzatAdapterBase(const LOC_API_ADAPTER_EVENT_MASK_T mask,
                    ContextBase* context, bool isMaster = false);
    virtual ~IzatAdapterBase();
    const MsgTask* mMsgTask;
public:

    inline void sendMsg(const LocMsg* msg) const {
        mMsgTask->sendMsg(msg);
    }
    inline void sendMsg(const LocMsg* msg) {
        mMsgTask->sendMsg(msg);
    }

    virtual void handleEngineUpEvent();
    virtual void handleEngineDownEvent();


    // for GDT AP
    virtual bool onGdtUploadBeginEvent(int32_t service,
                                       int32_t session,
                                       const char* filePath,
                                       uint32_t filePath_len);
    virtual bool onGdtUploadEndEvent(int32_t service, int32_t session, int32_t status);
    virtual bool handleGtpApStatusResp(int32_t status, uint16_t clientSwVer,
                                       uint8_t asnMajVer, uint8_t asnMinVer, uint8_t asnPtVer);
    virtual bool onGdtDownloadBeginEvent(int32_t service, uint32_t session,
                                         uint32_t respTimeoutInterval,
                                         const uint8_t* clientInfo, uint32_t clientInfo_len,
                                         const uint8_t* mobileStatusData, uint32_t mobileStatusData_len,
                                         const char* filePath, uint32_t filePath_len,
                                         uint32_t powerBudgetInfo, int8_t powerBudgetInfo_valid,
                                         uint32_t powerBudgetAllowance, int8_t powerBudgetAllowance_valid,
                                         uint32_t downloadRequestMask, int8_t downloadRequestMask_valid);
    virtual bool onGdtReceiveDoneEvent(int32_t service, uint32_t session, int32_t status);
    virtual bool onGdtDownloadEndEvent(int32_t service, uint32_t session, int32_t status);

    // XTRA Client 2.0
    virtual bool onReceiveXtraServers(const char* server1, const char* server2,
                                      const char* server3, const uint32_t reqSource,
                                      const uint8_t serverUpdateMaskValid,
                                      const uint64_t serverUpdateMask,
                                      const uint8_t updateTypeValid, const uint32_t updateType,
                                      const uint8_t updateRateValid, const uint32_t updateRate,
                                      const uint8_t fileInfo_valid, const uint32_t fileType,
                                      const uint32_t downloadInterval);
    virtual bool onRequestXtraData(const uint32_t reqSource);
    virtual bool onRequestNtpTime();
    virtual bool onReceiveXtraConfig(uint32_t status, uint32_t configReqSource,
                                     uint32_t report_mask,
                                     uint16_t absAge, uint64_t relAge,
                                     uint8_t timereqValid, uint16_t prefValidAgeHrs,
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


    virtual bool onQueryXtraInfoReqEvent(uint32_t useableConstellations,
                                          uint32_t forcedConstellations,
                                          uint8_t mbEnabled, uint8_t xtraFileMajorVersion,
                                          uint8_t xtraFileMinorVersion,
                                          uint16_t primaryMcc, uint16_t secondaryMcc,
                                          uint32_t xtraRemValidity, uint8_t preciseXtraStatus);

    virtual bool onXtraInjectionStatusInfo(const int32_t status,
                                          const uint64_t constellationMask);
};

}  // namespace izat_core

#ifdef __cplusplus
}
#endif

#endif /* IZAT_ADAPTER_BASE_H */
