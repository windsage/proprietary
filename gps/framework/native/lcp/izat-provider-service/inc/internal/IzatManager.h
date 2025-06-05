/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat Manager

  Copyright (c) 2015-2016, 2018-2021, 2022-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef __IZAT_MANAGER_IZATMANAGER_H__
#define __IZAT_MANAGER_IZATMANAGER_H__

#include <set>
#include <string>
#include <comdef.h>
#include <unordered_set>
#include "IIzatManager.h"
#include <mq_client/IPCMessagingProxy.h>
#include "ILocationResponse.h"
#include "IzatLocation.h"
#include "IzatContext.h"
#include "IzatNmea.h"
#include "IzatSvInfo.h"
#include "IDataItemCore.h"
#include "DataItemConcreteTypes.h"
#include "ILocationAPI.h"
#include "location_interface.h"
#include "loc_misc_utils.h"
#include "ContextBase.h"
#include <LBSProxy.h>

namespace qc_loc_fw {
    class InPostcard;
}

using namespace loc_util;
using loc_core::IDataItemCore;
using loc_core::IDataItemObserver;
using loc_core::ENHDataItem;
using loc_core::InEmergencyCallDataItem;

using lbs_core::IzatProviderBase;

namespace izat_manager
{
class LocationReport;
class IZatLocationError;
class IzatManager :
    public IIzatManager,
    public IIPCMessagingResponse,
    public ILocationResponse {
public:
    static IzatManager* getInstance(IOSFramework* pOSFrameworkObj);
    virtual ~IzatManager();
    void init();

    // IIzatManager overrides used by framework
    virtual int32 subscribeListener(IOSListener* osLocationListener);
    virtual int32 unsubscribeListener(IOSListener* osLocationListener);
    virtual int32 addRequest(const IzatRequest* request);
    virtual int32 removeRequest(const IzatRequest* request);
    virtual int32 enableProvider(const IzatRequest::IzatProviderType provider);
    virtual int32 disableProvider(const IzatRequest::IzatProviderType provider);
    virtual int32 notifyNetworkStatus (const IzatNetworkStatus& status);

    // ILocationResponse overrides
    virtual void reportLocation(const LocationReport* locReport, int locCnt,
        const ILocationProvider* providerSrc = NULL);
    virtual void reportError(const IZatLocationError* error, const ILocationProvider* providerSrc = NULL);

    // IIPCMessagingResponse overrides
    virtual void handleMsg(InPostcard* const in_card);
    virtual inline MsgTask* getMsgTask() {return mIzatContext.mMsgTask;};

    virtual void sendPostcard(qc_loc_fw::OutPostcard* const card, const char* msgTo);

    // callback for LocationAPI, the first three are mandatory
    inline void onCapabilitiesCb(LocationCapabilitiesMask) {};
    inline void onResponseCb(LocationError, uint32_t) {};
    inline void onCollectiveResponseCb(size_t, LocationError*, uint32_t*) {};
    void onTrackingCb(const Location& locInfo);
    void registerIzatOdcpiCallback(OdcpiPrioritytype priority, OdcpiCallbackTypeMask typeMask);
    void deRegisterIzatOdcpiCbForNonEmergency();
    void registerEsStatusCallback();
    const GnssInterface* getGnssInterface();
    void odcpiRequestCb(const OdcpiRequestInfo& odcpiRequest, OdcpiPrioritytype priority);
    IzatRequest* createIzatReqest(const OdcpiRequestInfo& odcpiRequest,
            IzatRequest::IzatProviderType provider);
    static const std::string mConfFile;
    static const std::string mIzatConfFile;
    const GnssInterface* mGnssInterface = nullptr;
    typedef const GnssInterface* (getLocationInterface)();
    // Registers callback to get gnss enable/diablement signal
    void registerGnssStatusCallback();
    typedef void (registerGnssStatuscb)(std::function<void(bool)>, std::function<void(bool)>);
    // notify the subscribers by loading FW_glue
    typedef void (notifyOptinStatusToSubscriberExt)(std::unordered_set<IDataItemCore*> dataItemList);
    void setIzatFusedProviderOverride(bool izatFused);
    bool getIzatFusedProviderOverride();
    // report emergency status
    void reportEsStatus(bool isEmergencyMode);
    // report optin status
    void reportOptinStatus(bool optIn);
    void registerLocationAPICb();

private:
    // Internal Methods
    IzatManager(IOSFramework* pOSFrameworkObj);
    int deinit();

    // used to inform NLP position session status down to ULP for ZPP purpose
    static IzatLocation izatLocationFromUlpLocation(const UlpLocation *ulpLoc,
                                                    const GpsLocationExtended* locExtended);
    void handleNLPSettingChange(bool nlpSettingsEnabled);
    void printUlpLocation(UlpLocation * ulpLocation,
                          GpsLocationExtended * locationExtended,
                          enum loc_sess_status status);

    void registerWithQesdkProxy();

    // Data Types
    typedef set<IOSListener*> TListenerCol;

    // Class instance
    static IzatManager* mIzatManager;
    static std::mutex mMutex;

    // Data members
    const std::string mSelfName;
    const std::string mOSAgentIPCAddressName;

    loc_core::ContextBase* mContext;
    s_IzatContext mIzatContext;
    TListenerCol mListeners;
    bool mFLPProviderEnabled;
    bool mNLPProviderEnabled;
    bool mEmergencyStatus;
    bool mOptInStatus;
    bool mIsNonEsOdcpiCallbackValid;
    IDataItemObserver *mWiper;
    // location api client to be linked with both GNSS and FLP adapter stack
    ILocationAPI* mLocAPI;
    // callbacks registered with both GNSS adapter and FLP stack
    LocationCallbacks mLocAPICb;

    // Network provider stuff
    ILocationProvider* mWiFiLocationProvider;
    ILocationProvider* mWWanLocationProvider;

    typedef void (setXtAdapterUserPref)(bool userPref);
    class OptInObserver : public IDataItemObserver {
        setXtAdapterUserPref* mSetUserPrefFn;
        IOsObserver* mOsObserver;
        IzatManager* mIzatManager;
    public:
        inline OptInObserver(IzatManager* mgrObj, setXtAdapterUserPref fn, IOsObserver* osObsrvr) :
                mIzatManager(mgrObj), mSetUserPrefFn(fn), mOsObserver(osObsrvr) {
            const std::unordered_set<DataItemId> dl({ENH_DATA_ITEM_ID});
            mOsObserver->subscribe(dl, this);
        }
        inline ~OptInObserver() {
            const std::unordered_set<DataItemId> dl({ENH_DATA_ITEM_ID});
            mOsObserver->unsubscribe(dl, this);
        }
        inline virtual void getName(string& name) override { name = "OptInObserver"; }
        inline virtual void notify(const std::unordered_set<IDataItemCore*>& dlist) override {
            for (auto d : dlist) {
                if (d->getId() == ENH_DATA_ITEM_ID) {
                    mSetUserPrefFn(((ENHDataItem*)d)->isEnabled());
                    mIzatManager->reportOptinStatus(((ENHDataItem*)d)->isEnabled());
                }
            }
        }
    };
    OptInObserver* mOptInObserver;

    class EsStatusObserver : public IDataItemObserver {
        IzatManager* mIzatManager;
        IOsObserver* mOsObserver;
    public:
        inline EsStatusObserver(IzatManager* mgrObj, IOsObserver* osObsrvr) :
                mIzatManager(mgrObj), mOsObserver(osObsrvr) {
            const std::unordered_set<DataItemId> dl({IN_EMERGENCY_CALL_DATA_ITEM_ID});
            mOsObserver->subscribe(dl, this);
        }
        inline ~EsStatusObserver() {
            const std::unordered_set<DataItemId> dl({IN_EMERGENCY_CALL_DATA_ITEM_ID});
            mOsObserver->unsubscribe(dl, this);
        }
        inline virtual void getName(string& name) override { name = "EsStatusObserver"; }
        inline virtual void notify(const std::unordered_set<IDataItemCore*>& dlist) override {
            for (auto d : dlist) {
                if (d->getId() == IN_EMERGENCY_CALL_DATA_ITEM_ID) {
                    mIzatManager->reportEsStatus(((InEmergencyCallDataItem*)d)->mIsEmergency);
                }
            }
        }
    };
    EsStatusObserver* mEsStatusObserver;

    // IzatProvider
    IzatProviderBase* mIzatProvider;
    struct subscribeListenerMsg : public LocMsg {
        IzatManager* mIzatManager;
        IOSListener* mListener;

        inline subscribeListenerMsg(IzatManager *mgrObj,
            IOSListener* listener)
        :mIzatManager(mgrObj), mListener(listener) {}

        virtual void proc() const;
    };

    struct unsubscribeListenerMsg : public LocMsg {
        IzatManager* mIzatManager;
        IOSListener* mListener;

        inline unsubscribeListenerMsg(IzatManager *mgrObj,
            IOSListener* listener)
        :mIzatManager(mgrObj), mListener(listener) {}

        virtual void proc() const;
    };

    struct addRemoveRequestMsg : public LocMsg {
        IzatManager* mIzatManager;
        bool         mIsAddRequest; // true: add, false: remove
        IzatRequest  mRequest;    // only valid if addRequest is true

        inline addRemoveRequestMsg(IzatManager *mgrObj, bool isAddRequest,
                const IzatRequest* request) : mIzatManager(mgrObj),
                mIsAddRequest(isAddRequest) {
                    mRequest = *request;
                }

        virtual void proc() const;
    };

    struct handleNonEsOdcpiReqMsg : public LocMsg {
        IzatManager* mIzatManager;

        inline handleNonEsOdcpiReqMsg(IzatManager *mgrObj) : mIzatManager(mgrObj) {}
        virtual void proc() const;
    };

    struct enableProviderMsg : public LocMsg {
        IzatManager* mIzatManager;
        IzatRequest::IzatProviderType mProvider;

        inline enableProviderMsg(IzatManager *mgrObj, IzatRequest::IzatProviderType prov)
        :mIzatManager(mgrObj)
        ,mProvider(prov) {}

        virtual void proc() const;
    };

    struct disableProviderMsg : public LocMsg {
        IzatManager* mIzatManager;
        IzatRequest::IzatProviderType mProvider;

        inline disableProviderMsg(IzatManager *mgrObj, IzatRequest::IzatProviderType prov)
        :mIzatManager(mgrObj)
        ,mProvider(prov) {}

        virtual void proc() const;
    };

    struct startPassiveLocationListenerMsg : public LocMsg {
        IzatManager *mIzatManager;
        std::string mClientName;
        IzatListenerMask mListensTo;
        bool mIntermediateFixAccepted;

        inline startPassiveLocationListenerMsg(IzatManager *mgrObj,
                                               const char *clientName,
                                               IzatListenerMask listensTo,
                                               bool intermediateFixAccepted)
            : mIzatManager(mgrObj)
            , mClientName(clientName)
            , mListensTo(listensTo)
            , mIntermediateFixAccepted(intermediateFixAccepted)
            { }

        virtual void proc() const;
    };

    struct stopPassiveLocationListenerMsg : public LocMsg {
        IzatManager *mIzatManager;
        std::string mClientName;
        inline stopPassiveLocationListenerMsg(IzatManager *mgrObj,
                                              const char *clientName)
            : mIzatManager(mgrObj), mClientName(clientName) { }
        virtual void proc() const;
    };


    struct trackingCbMsg : public LocMsg {
        IzatManager *mIzatManager;
        Location mLocation;

        inline trackingCbMsg(IzatManager *mgrObj,
                             const Location &locInfo)
            : mIzatManager(mgrObj)
            , mLocation(locInfo)
            { }

        virtual void proc() const;
    };

    struct registerIzatOdcpiCallbackAsync : public LocMsg {
        IzatManager* mIzatManager;

        inline registerIzatOdcpiCallbackAsync(IzatManager *mgrObj)
        :mIzatManager(mgrObj) {}

        virtual void proc() const;
    };

    struct connectToZaxisProviderAsync : public LocMsg {
        IzatManager* mIzatManager;

        inline connectToZaxisProviderAsync(IzatManager *mgrObj)
            :mIzatManager(mgrObj) {}

        virtual void proc() const;
    };

    struct registerGnssStatusCallbackAsync : public LocMsg {
        IzatManager* mIzatManager;

        inline registerGnssStatusCallbackAsync(IzatManager *mgrObj)
        :mIzatManager(mgrObj) {}

        virtual void proc() const;
    };
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATMANAGER_H__
