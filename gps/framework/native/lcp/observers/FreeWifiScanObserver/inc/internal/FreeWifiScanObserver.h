/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IIPCMessagingResponse

  Copyright (c) 2015 - 2016, 2020, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_FREEWIFISCANOBSERVER_H__
#define __IZAT_MANAGER_FREEWIFISCANOBSERVER_H__

#include <list>
#include <string>
#include <lowi_request.h>
#include <mq_client/IPCMessagingProxy.h>
#include <MsgTask.h>
#include <IWifiScanInitiator.h>
#include <IFreeWifiScanSubscription.h>
#include <LocTimer.h>

using namespace loc_util;

namespace qc_loc_fw {
class InPostcard;
class LOWIAsyncDiscoveryScanResultRequest;
}

namespace izat_manager
{

using namespace std;
using namespace qc_loc_fw;

class IFreeWifiScanObserver;

class FreeWifiScanObserver :
public IWifiScanInitiator,
public IFreeWifiScanSubscription,
public IIPCMessagingResponse {

public:
    // Singleton methods
    static FreeWifiScanObserver * getInstance (IIPCMessagingRequest * ipcIface,
                                               MsgTask * msgTask);
    static void destroyInstance ();

    // IWifiScanInitiator overrides
    void initiateWifiScan
    (
        unsigned long int measurementAgeFilterInSecs,
        unsigned long int fallbackToleranceSec,
        long long int timeoutTimestamp,
        bool mIsEmergencyRequest
    );

    // query WiFi HW capabilities
    void queryWiFiHwCapabilities();
    // IFreeWifiScanSubscription, overrides
    void subscribe (const IFreeWifiScanObserver * observer);
    void unsubscribe (const IFreeWifiScanObserver * observer);

    // IIPCMessagingResponse overrides
    void handleMsg (InPostcard * const inCard);


private:
    FreeWifiScanObserver (IIPCMessagingRequest * ipcIface, MsgTask * msgTask);
    inline ~FreeWifiScanObserver () {}

    // helpers
    void sendFreeWifiScanRequest
    (
        unsigned long int validTime = ASYNC_DISCOVERY_REQ_VALIDITY_PERIOD_SEC_MAX
    );
    void refreshFreeWifiScanRequest ();
    void cancelFreeWifiScanRequest ();
    int registerOSAgentUpdate ();

    // Loc messages
    struct LocMsgBase : public LocMsg {
        LocMsgBase (FreeWifiScanObserver * parent);
        virtual ~LocMsgBase ();
        // Data memebers
        FreeWifiScanObserver * mParent;
    };

    struct InitiateWifiScanLocMsg : public LocMsgBase {

        InitiateWifiScanLocMsg (FreeWifiScanObserver * parent,
                                unsigned long int measurementAgeFilterInSecs,
                                unsigned long int fallbackToleranceSec,
                                long long int timeoutTimestamp,
                                bool isEmergencyRequest);
        virtual ~InitiateWifiScanLocMsg ();
        void proc () const;
        // Data members
        const unsigned long int mMeasurementAgeFilterInSecs;
        const unsigned long int mFallbackToleranceSec;
        const int64 mTimeoutTimestamp;
        bool mIsEmergencyRequest;
    };

    struct SubscribeLocMsg : public LocMsgBase {
        SubscribeLocMsg (FreeWifiScanObserver * parent,
                         const IFreeWifiScanObserver * observer);
        virtual ~SubscribeLocMsg ();
        void proc () const;
        // Data members
        const IFreeWifiScanObserver * mObserver;
    };

    struct UnsubscribeLocMsg : public LocMsgBase {
        UnsubscribeLocMsg (FreeWifiScanObserver * parent,
                           const IFreeWifiScanObserver * observer);
        virtual ~UnsubscribeLocMsg ();
        void proc () const;
        // Data members
        const IFreeWifiScanObserver * mObserver;
    };

    struct NotifyLocMsg : public LocMsgBase {
        NotifyLocMsg (FreeWifiScanObserver * parent,
                      const LOWIResponse * response);
        virtual ~NotifyLocMsg ();
        void proc () const;
        // Data members
        const LOWIResponse * mResponse;
    };

    struct ReqExpiryMsg : public LocMsgBase {
        ReqExpiryMsg (FreeWifiScanObserver * parent);
        virtual ~ReqExpiryMsg ();
        void proc () const;
        // Data members
    };

    struct OsSettingRegExpiryMsg : public LocMsgBase {
        OsSettingRegExpiryMsg (FreeWifiScanObserver * parent);
        virtual ~OsSettingRegExpiryMsg ();
        void proc () const;
        // Data members
    };

    struct ReqWiFiCapabilitiesMsg : public LocMsgBase {
        ReqWiFiCapabilitiesMsg (FreeWifiScanObserver * parent) :
            LocMsgBase(parent) {}
        virtual ~ReqWiFiCapabilitiesMsg () {}
        void proc () const;
    };

    struct LocSettingChangeMsg : public LocMsgBase {
        LocSettingChangeMsg (FreeWifiScanObserver * parent,
                             bool locSettingEnabled);
        virtual ~LocSettingChangeMsg ();
        void proc () const;
        // Data members
        bool mLocSettingEnabled;
    };

    class Timer : public LocTimer {
        FreeWifiScanObserver* mClient;
    public:
        inline Timer(FreeWifiScanObserver* client) :
            LocTimer(), mClient(client) {}
        void timeOutCallback() override;
    };

    class OsSettingRegTimer : public LocTimer {
        FreeWifiScanObserver* mClient;
    public:
        inline OsSettingRegTimer(FreeWifiScanObserver* client) :
            LocTimer(), mClient(client) {}
        void timeOutCallback() override;
    };

    // Data members
    static FreeWifiScanObserver         * mInstance;
    IIPCMessagingRequest                * mIpcIface;
    MsgTask                             * mMsgTask;
    string                                mAddress;
    bool                                  mRegisteredForFreeWifiScan;
    bool                                  mOsRegisterd;     // registered with OS agent
    bool                                  mLocationEnabled; // location enabled on the device
    unsigned long int                     mRequestId;
    Timer                                 mTimer;
    OsSettingRegTimer                     mOsRegTimer;
    list <const IFreeWifiScanObserver *>  mClients;
    friend class Timer;
    friend class OsSettingRegTimer;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_FREEWIFISCANOBSERVER_H__

