/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/


#ifndef IZAT_MANAGER_WIFIPROVIDER_H
#define IZAT_MANAGER_WIFIPROVIDER_H

#include "LocTimer.h"
#include "LocationProvider.h"
#include <mq_client/IPCMessagingProxy.h>
#include "LBSAdapter.h"

#define GTP_WIFI_PROVIDER_REQUEST_TIME_OUT_IN_MS 10000

namespace izat_manager
{

using namespace qc_loc_fw;

class ZppMeasurementReport : public LocationReport {
    public:
        bool  mHasSourceTechMask;
        LocPosTechMask mSourceTechMask;

        ZppMeasurementReport()
        {
            resetInternal();
        }

        ZppMeasurementReport(const ZppMeasurementReport & zppLocReport):
            LocationReport(zppLocReport)
        {
            makeCopy(zppLocReport);
        }

        ZppMeasurementReport & operator=(const ZppMeasurementReport & zppLocReport)
        {
            LocationReport::operator=(zppLocReport);
            makeCopy(zppLocReport);
            return *this;
        }
        void reset()
        {
            LocationReport::reset();
            resetInternal();
        }
        virtual ~ZppMeasurementReport()
        {
            resetInternal();
        }

    protected:

    private:
       void makeCopy(const ZppMeasurementReport & zppLocReport)
        {
            mHasSourceTechMask = zppLocReport.mHasSourceTechMask;
            mSourceTechMask = zppLocReport.mSourceTechMask;
        }

        void resetInternal()
        {
            mHasSourceTechMask = false;
        }
};

class WifiMeasurementReport : public LocationReport {
public:
    bool mIsRttFix;

    WifiMeasurementReport() {
        resetInternal();
    }

    WifiMeasurementReport(const WifiMeasurementReport & wifiLocReport):
            LocationReport(wifiLocReport) {
        makeCopy(wifiLocReport);
    }

    WifiMeasurementReport & operator=(const WifiMeasurementReport & wifiLocReport)
    {
        LocationReport::operator=(wifiLocReport);
        makeCopy(wifiLocReport);
        return *this;
    }
    void reset()
    {
        LocationReport::reset();
        resetInternal();
    }
    virtual ~WifiMeasurementReport()
    {
        resetInternal();
    }

protected:

private:
    void makeCopy(const WifiMeasurementReport & wWifiLocReport)
    {
        mIsRttFix = wWifiLocReport.mIsRttFix;
    }

    void resetInternal()
    {
        mIsRttFix = false;
    }
};

class WiFiLocationProvider : public LocationProvider {
public:
    static ILocationProvider* getInstance(const struct s_IzatContext* izatContext);

    // ILocationProvider overrides
    int setRequest(const LocationRequest *request);
    void enable() override {}
    void disable() override {}


    // IMQServerProxyResponse Overrides
    virtual void handleMsg(InPostcard *const in_card);

private:
    class Timer : public LocTimer {
        std::function<void ()> mProcImpl;
    public:
        inline Timer(std::function<void()> fun) :
            LocTimer(), mProcImpl(fun) {}
        virtual void timeOutCallback() override {mProcImpl();}
    };
    friend class Timer;

    typedef enum {
        MODE_DISABLED = 0,
        MODE_SDK,
    } ProviderMode;

    const std::string mProviderTag;
    static ILocationProvider* mWifiLocationProvider;

    static ProviderMode mProviderMode;
    LBSAdapter *mLBSAdapter;
    LocationRequest mConsolidatedReq;
    Timer mTBFTimer;
    Timer mRequestTimer;
    int mTxId;

    // Mac accuracy threshold to report network location fix
    static unsigned int mMaxAcuracyThresholdToFilterWiFiPosition;

    // Max allowed distance between zpp and wifi fix,
    // if wifi fix does not fall within the uncertainity circle of Zpp
    static unsigned int mMaxZppWifiConsistencyCheckInMeters;
    //Cache the last GTP WiFi position report
    WifiMeasurementReport mWifiLocReport;
    ZppMeasurementReport mZppLocReport;
    int64 mTimestampLastTrackingRequestInNs;

    WiFiLocationProvider(const struct s_IzatContext* izatContext);
    inline ~WiFiLocationProvider() {}
    static void readConfig();
    void resetTimer();
    void startSession(bool forceToUpdateStatus = false);
    void stopSession(bool forceToUpdateStatus = false);
    void resetSession();
    int sendRequest();
    void cacheFixAndReport(const WifiMeasurementReport& wifiRpt);
    void getZppFixSync();
    void consolidateRequest(const LocationRequest& req);
    void reportProviderError(IZatLocationError::LocationErrorType errorType);

    //Consistency check functions
    bool consistencyCheckWifiFix(LocationReport& locReportToCompareWith,
            WifiMeasurementReport* wifiLocReport);
    bool zppHasCPIContentOrWiFiContent();
    bool isLocReportedWithinTime(LocationReport& locReport, int timeInSeconds);
    float getPUNCIncrease(const LocationReport & locReport);
    void increasePUNC(LocationReport & locReport);
    double getGreatCircleDistanceMeter(const LocationReport *loc1, const LocationReport *loc2);

    // LocMsg classes
    struct RequestWifiLocationMsg : public LocMsg {
        LocationRequest mLocationRequest;
        WiFiLocationProvider* mWifiProvider;

        RequestWifiLocationMsg(WiFiLocationProvider *pGtpWifiProvider,
            const LocationRequest* locReq) :
                mWifiProvider(pGtpWifiProvider) {
            memcpy(&mLocationRequest, locReq, sizeof(LocationRequest));
        }
        inline ~RequestWifiLocationMsg() {}
        virtual void proc() const;
    };

    struct RequestTimerMsg : public LocMsg {
        WiFiLocationProvider* mWifiProvider;

        inline RequestTimerMsg(WiFiLocationProvider *pGtpWifiProvider) :
            mWifiProvider(pGtpWifiProvider) {
        }
        inline ~RequestTimerMsg() {}

        virtual void proc() const;
    };

    struct TBFTimerMsg : public LocMsg {
        WiFiLocationProvider* mWifiProvider;

        inline TBFTimerMsg(WiFiLocationProvider *pGtpWifiProvider) :
            mWifiProvider(pGtpWifiProvider) {
        }
        inline ~TBFTimerMsg() {}

        virtual void proc() const;
    };

    struct PositionMsgWifi : public LocMsg {
        InPostcard *mIncard;
        WiFiLocationProvider* mWifiProvider;

        inline PositionMsgWifi(WiFiLocationProvider *pGtpWifiProvider, InPostcard *in_card) :
             mIncard(in_card), mWifiProvider(pGtpWifiProvider) {
        }
        ~PositionMsgWifi();

        virtual void proc() const;
    };

    struct HandleGtpWifiSSRMsg : public LocMsg {
        WiFiLocationProvider* mWifiProvider;

        HandleGtpWifiSSRMsg(WiFiLocationProvider *pGtpWifiProvider) :
            mWifiProvider(pGtpWifiProvider) {}
        virtual void proc() const;
        inline ~HandleGtpWifiSSRMsg() {}
    };
};
}
#endif
