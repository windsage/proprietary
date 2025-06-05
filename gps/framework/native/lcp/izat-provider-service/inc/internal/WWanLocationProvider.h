/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#ifndef IZAT_MANAGER_WWANPROVIDER_H
#define IZAT_MANAGER_WWANPROVIDER_H

#include "LocTimer.h"
#include "LocationProvider.h"
#include <mq_client/IPCMessagingProxy.h>
#include <functional>

#define WWAN_PROVIDER_REQUEST_TIME_OUT_IN_MS 40000
#define TIME_THRESHOLD_TO_RE_NOTIFY_NLP_STATUS_SEC 30

namespace izat_manager
{

using namespace qc_loc_fw;

typedef std::function<void(bool)> DataConnectionReqCb;

class WWanLocationProvider : public LocationProvider {
public:
    static ILocationProvider* getInstance(const struct s_IzatContext* izatContext);

    // ILocationProvider overrides
    int setRequest(const LocationRequest *request);
    void enable() override {}
    void disable() override {}

    inline void setDataConnectionCb(DataConnectionReqCb cb) {mDataConnectionReqFunc = cb;}
    void notifyNetworkStatus(const IzatNetworkStatus& status);
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

    const std::string mProviderTag;
    static ILocationProvider* mWWanLocationProvider;

    DataConnectionReqCb mDataConnectionReqFunc;

    LocationRequest mConsolidatedReq;
    Timer mTBFTimer;
    Timer mRequestTimer;
    int mTxId;

    // Mac accuracy threshold to report network location fix
    static unsigned int mMaxAcuracyThresholdToFilterNlpPosition;

    //Cache the last WWan position report
    LocationReport mWWanLocReport;

    int64 mTimestampLastTrackingRequestInNs;
    int64 mTimestampLastPositionReportInNs;
    int mCachePositionReportThresholdInSec;

    WWanLocationProvider(const struct s_IzatContext* izatContext);
    inline ~WWanLocationProvider() {}
    void readConfig();
    void resetTimer();
    void startSession(bool forceToUpdateStatus = false);
    void stopSession(bool forceToUpdateStatus = false);
    void resetSession();
    int sendWWanPostioningReq();
    int sendWWanConnResp(bool status, const char* resp_type);
    void consolidateRequest(const LocationRequest& req);
    void reportProviderError(IZatLocationError::LocationErrorType errorType);

    // LocMsg classes
    struct RequestWWanLocationMsg : public LocMsg {
        LocationRequest mLocationRequest;
        WWanLocationProvider* mProvider;

        RequestWWanLocationMsg(WWanLocationProvider *pWWanProvider, const LocationRequest* locReq) :
                mProvider(pWWanProvider) {
            memcpy(&mLocationRequest, locReq, sizeof(LocationRequest));
        }
        inline ~RequestWWanLocationMsg() {}
        virtual void proc() const;
    };

    struct RequestTimerMsg : public LocMsg {
        WWanLocationProvider* mProvider;

        inline RequestTimerMsg(WWanLocationProvider *pWWanProvider) :
            mProvider(pWWanProvider) {
        }
        inline ~RequestTimerMsg() {}

        virtual void proc() const;
    };

    struct TBFTimerMsg : public LocMsg {
        WWanLocationProvider* mProvider;

        inline TBFTimerMsg(WWanLocationProvider *pWWanProvider) :
            mProvider(pWWanProvider) {
        }
        inline ~TBFTimerMsg() {}

        virtual void proc() const;
    };

    struct PositionMsgWWan : public LocMsg {
        InPostcard *mIncard;
        WWanLocationProvider* mProvider;

        inline PositionMsgWWan(WWanLocationProvider *pWWanProvider, InPostcard *in_card) :
                mProvider(pWWanProvider) {
            if (nullptr != in_card) {
                mIncard = InPostcard::createInstance();
                copyInCard(in_card, mIncard);
            } else {
                LOC_LOGe("ERROR: in_card = nullptr");
                return;
            }
        }
        ~PositionMsgWWan();

        virtual void proc() const;
    };

    struct DataConnectionMsgWWan : public LocMsg {
        InPostcard *mIncard;
        WWanLocationProvider* mProvider;

        inline DataConnectionMsgWWan(WWanLocationProvider *pWWanProvider, InPostcard *in_card) :
                mProvider(pWWanProvider) {
            if (nullptr != in_card) {
                mIncard = InPostcard::createInstance();
                copyInCard(in_card, mIncard);
            } else {
                LOC_LOGe("ERROR: in_card = nullptr");
                return;
            }
        }
        virtual void proc() const;
        ~DataConnectionMsgWWan();
    };

    struct HandleGtpWWanSSRMsg : public LocMsg {
        WWanLocationProvider* mProvider;

        HandleGtpWWanSSRMsg(WWanLocationProvider *pWWanProvider) :
            mProvider(pWWanProvider) {}
        virtual void proc() const;
        inline ~HandleGtpWWanSSRMsg() {}
    };

    //Below are Test code, shall be removed from final PS
    void mockReportLocation();
    void mockDataConnection();

};
}
#endif
