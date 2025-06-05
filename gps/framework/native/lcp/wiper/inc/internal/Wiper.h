/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2018, 2020, 2022-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_WIPER_H__
#define __IZAT_MANAGER_WIPER_H__

#include <MsgTask.h>
#include <ContextBase.h>
#include "IFreeWifiScanObserver.h"
#include "IDataItemObserver.h"
#include <mq_client/IPCMessagingProxy.h>
#include "LBSAdapter.h"
#include "FreeWifiScanObserver.h"
#include <base_util/vector.h>
#include "IzatContext.h"
#include "lowi_response.h"
#include "DataItemConcreteTypesExt.h"
#include "IDataItemCore.h"
#include <functional>
#include "IzatTypes.h"
#include <LocTimer.h>

namespace qc_loc_fw {
class InPostcard;
class LOWIScanMeasurement;
class LOWIResponse;
class LOWIDiscoveryScanResultResponse;
class LOWIAsyncDiscoveryScanResultResponse;
}

namespace izat_manager
{

using namespace std;
using namespace loc_core;
using namespace izat_manager;

class Wiper :
    public IFreeWifiScanObserver,
    public IDataItemObserver {

public:

    // Singleton methods
    static Wiper * getInstance(const struct s_IzatContext * izatContex);
    static void destroyInstance();

    // set feature registration bits and initialize/update LBSAdapter with event mask
    void setFeatureRegistrationBits();

    // IFreeWifiScanObserver overrides
    virtual void notify(const LOWIResponse * response) override;
    virtual void notifyLocSettingChange(bool locationEnabled) override;

    // IDataItemObserver overrides
    virtual void getName (string & name);
    virtual void notify(const std::unordered_set<IDataItemCore *> & dlist);

    static void handleWifiApDataRequest(const WifiApDataRequest &request);
    static void handleSsrInform();
    static void handleTimeZoneInfoRequest();

private:
    typedef struct WiperConfigData {
        int    mWifiWaitTimeoutSelect;
    } WiperConfigData;

    typedef enum WiperFeatureFlag {
        FEATURE_FREE_WIFI_SCAN_INJECT_MASK = 0x2000,
        FEATURE_SUPL_WIFI_MASK = 0x4000,
        FEATURE_WIFI_SUPPLICANT_INFO_MASK = 0x8000
    } WiperFeatureFlag;

    // Data members
    static Wiper                  * mInstance;
    const struct s_IzatContext    * mIzatContext;
    LBSAdapter                    * mLbsAdapter;
    FreeWifiScanObserver          * mWifiScanObserver;
    int                             mWiperFlag;
    WiperConfigData                 mWiperConfigData;
    bool                            mIsWifiScanInSession;
    int64                           mWifiScanRequestedUTCTimesInMs;
    WifiScanSet                     mWifiScanSet;
    WifiSupplicantStatusDataItem::WifiSupplicantState    mLatestSupplicantState;
    bool                            mSupportWIFIInject2V2;
    bool                            mLowiStatusValid;
    uint8_t                         mCapabilityQueryCnt;

    Wiper(const struct s_IzatContext * izatContext,
        unsigned int wiperFlag, WiperConfigData &wiperConfData);
    ~Wiper();

    static void stringify (WifiSupplicantInfo & wsi, string & valueStr);
    static void stringify (WifiApInfo & wai, string & valueStr);
    static void stringify (WifiApSsidInfo & ws, string & valueStr);

    struct wifiScanNotificationMsg : public LocMsg {
        Wiper *mWiperObj;
        qc_loc_fw::vector<LOWIScanMeasurement *> mLowiScanMeasurements;
        WifiScanType mWifiScanType;
        LOWIMacAddress mSelfMacAddr;

        wifiScanNotificationMsg(Wiper *wiperObj,
            qc_loc_fw::vector<LOWIScanMeasurement *> lowiScanMeasurements,
            WifiScanType scanType, LOWIMacAddress self_mac);

        ~wifiScanNotificationMsg();
        virtual void proc() const;
    };

     struct wifiScanNotificationErrorMsg : public LocMsg {
        Wiper *mWiperObj;
        WifiScanResponseType mErrType;
        WifiScanType mWifiScanType;

        wifiScanNotificationErrorMsg(Wiper *wiperObj, WifiScanResponseType errType,
            WifiScanType scanType) : mWiperObj(wiperObj), mErrType(errType), mWifiScanType(scanType) {}

        ~wifiScanNotificationErrorMsg() {}
        virtual void proc() const;
    };

    struct wifiScanRequestMsg : public LocMsg {
        Wiper *mWiperObj;
        const WifiApDataRequest mRequest;

        inline wifiScanRequestMsg(Wiper *wiperObj, const WifiApDataRequest &request) :
            mWiperObj(wiperObj), mRequest(request) {
        }

        inline ~wifiScanRequestMsg() {}
        virtual void proc() const;
    };

    struct handleOsObserverUpdateMsg : public LocMsg {
        Wiper *mWiperObj;
        std::list <IDataItemCore *> mDataItemList;

        handleOsObserverUpdateMsg(Wiper *wiperObj,
            const std::unordered_set<IDataItemCore *> & dataItemList);

        ~handleOsObserverUpdateMsg();
        virtual void proc() const;
    };

    struct handleSsrMsg : public LocMsg {
        Wiper *mWiperObj;

        inline handleSsrMsg(Wiper *wiperObj) :
            mWiperObj(wiperObj) {
        }

        inline ~handleSsrMsg() {}
        virtual void proc() const;
    };

    struct wiperSubscribeInit : public LocMsg {
        Wiper *mWiperObj;
        int mWiperFlag;

        wiperSubscribeInit(Wiper *wiperObj, int wiperFlag) :
            mWiperObj(wiperObj), mWiperFlag(wiperFlag) {
        }

        inline ~wiperSubscribeInit() {}
        virtual void proc() const;
    };

    class LBSShutDownAdapter;
    struct lbsShutDownMsg : public LocMsg {
        LBSShutDownAdapter* mAdapter;
        inline lbsShutDownMsg(LBSShutDownAdapter* adapter) :
            LocMsg(), mAdapter(adapter) {}
        inline ~lbsShutDownMsg() {
            ContextBase* ctx = mAdapter->getContext();
            delete mAdapter;
            delete (MsgTask*)ctx->getMsgTask();
            delete ctx;
        }
        void proc() const;
    };

    class LBSShutDownAdapter : public LBSAdapterBase {
    public:
        inline LBSShutDownAdapter(ContextBase* context) :
                LBSAdapterBase(0, context) {}
        inline virtual void handleEngineUpEvent() {
            sendMsg(new (nothrow) lbsShutDownMsg(this));
        }
    };

    struct handleTimeZoneInfoRequestMsg : public LocMsg {
        Wiper *mWiperObj;

        inline handleTimeZoneInfoRequestMsg(Wiper *wiperObj) :
            mWiperObj(wiperObj) {
        }

        inline ~handleTimeZoneInfoRequestMsg() {}
        virtual void proc() const;
    };

    class wiperTimer : public LocTimer {
        Wiper *mWiperObj;
    public:
        inline wiperTimer(Wiper *wiperObj): LocTimer(), mWiperObj(wiperObj) {}
        virtual void timeOutCallback() override;
    };

    struct wifiCapabilitiesNotificationMsg : public LocMsg {
        Wiper *mWiperObj;
        LOWICapabilities mCapabilities;
        bool mStatus;

        wifiCapabilitiesNotificationMsg(Wiper *wiperObj, LOWICapabilityResponse *resp) :
                mWiperObj(wiperObj) {
            mCapabilities = resp->getCapabilities();
            mStatus = resp->getStatus();
        }

        inline ~wifiCapabilitiesNotificationMsg() {}
        virtual void proc() const;
    };

};
} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_WIPER_H__

