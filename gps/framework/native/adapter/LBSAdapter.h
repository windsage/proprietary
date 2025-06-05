/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef LBS_ADAPTER_H
#define LBS_ADAPTER_H

#include <WiperData.h>
#include <stdbool.h>
#include <string.h>
#include <loc_pla.h>
#include <LBSAdapterBase.h>
#include <LocContext.h>
#include <functional>
#include <vector>

#ifdef __cplusplus
extern "C"
{
#endif

using namespace loc_core;
using namespace lbs_core;
using namespace izat_manager;

enum zpp_fix_type {
    ZPP_FIX_WWAN,
    ZPP_FIX_BEST_AVAILABLE
};

struct WiperSsrInform : public LocMsg {
    inline WiperSsrInform() : LocMsg() {}
    virtual void proc() const;
};

struct WiperApDataRequest : public LocMsg {
    const WifiApDataRequest mRequest;
    inline WiperApDataRequest(const WifiApDataRequest &request) :
        LocMsg(), mRequest(request) {}
    virtual void proc() const;
};

#ifndef __ANDROID__
struct WwanFixMsg : public LocMsg {
    WwanFixMsg(LocGpsLocation *gpsLocation);
    virtual void proc() const;
private:
    LocGpsLocation mGpsLocation;
};
#endif

struct TimeZoneInfoRequest : public LocMsg {
    inline TimeZoneInfoRequest() : LocMsg() {}
    virtual void proc() const;
};

class LBSAdapter : public LBSAdapterBase {
    static LBSAdapter* mMe;

protected:
    inline LBSAdapter(const LOC_API_ADAPTER_EVENT_MASK_T mask) :
        LBSAdapterBase(mask,
                LocContext::getLocContext(LocContext::mLocationHalName),
                true),
        mFdclCallbacks(),
        mFdclClientData(nullptr) {}
    inline virtual ~LBSAdapter() {}
public:
    static LBSAdapter* get(const LOC_API_ADAPTER_EVENT_MASK_T mask);

    inline virtual bool requestWifiApData(const WifiApDataRequest &request) {
        sendMsg(new WiperApDataRequest(request));
        return true;
    }

    inline virtual void handleEngineUpEvent() {
        sendMsg(new WiperSsrInform());
    }

    virtual bool requestTimeZoneInfo();

    int cinfoInject(int cid, int lac, int mnc, int mcc, bool roaming);
    int oosInform();
    int niSuplInit(const char* supl_init, int length);
    int wifiEnabledStatusInject(int status);
    int wifiAttachmentStatusInject(WifiSupplicantInfo &wifiSupplicantInfo);
    int injectWifiApInfo(WifiApInfo &wifiApInfo);
    int injectWifiPosition(const Location* location);
    int setWifiWaitTimeoutValue(int timeout);
    int timeInfoInject(long curTimeMillis, int rawOffset, int dstOffset);

    FdclCallbacks mFdclCallbacks;
    void* mFdclClientData;
    void registerFdclCommand(FdclCallbacks&, void* fdclClientData);
    void unregisterFdclCommand();
    void injectFdclDataCommand(FdclData& fdclData);
    void requestFdclDataCommand(int32_t expireInDays);
    bool requestFdclDataEvent();
    bool reportFdclStatusEvent(bool success, const char* errorString, uint32_t errorLength);
    bool reportFdclInfoEvent(FdclInfo& fdclInfo, uint8_t status, UlpLocation& location);

    CellCSCallbacks mCellCSCallbacks;
    void* mCellCSClientData;
    void registerCellCSCommand(CellCSCallbacks&, void* cellCSClientData);
    void unregisterCellCSCommand();
    void requestCellCSDataCommand();
    bool requestCellCSDataEvent();
    bool reportCellCSInfoEvent(CellCSInfo& cellCSInfo, uint8_t status);

    // Zpp related
    bool getZppFixSync(LocGpsLocation &gpsLocation, LocPosTechMask &tech_mask);
    bool reportWwanZppFix(LocGpsLocation &zppLoc);

    int enableMask(LOC_API_ADAPTER_EVENT_MASK_T mask);
    void injectPositionCommand(const Location& location, bool onDemandCpi);

    // Sap Map related
    void sendMapData(const string maString);
};

#ifdef __cplusplus
}
#endif

#endif /* LBS_ADAPTER_H */
