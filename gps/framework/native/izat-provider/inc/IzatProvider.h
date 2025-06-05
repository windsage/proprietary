/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat Provider

  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IZAT_PROVIDER_H
#define IZAT_PROVIDER_H
#include<log_util.h>
#include<ILocationAPI.h>
#include<location_interface.h>
#include<gps_extended_c.h>
#include<IzatManager.h>
#include<LBSProxy.h>
#include<mutex>
#include<AltitudeReceiverProxy.h>
#include<IOSListener.h>

using namespace izat_manager;
using namespace std;
using namespace izat_zaxis;
class IzatProvider;

class ZlocationResponseHandler : public ILocationResponse {
    public:
        inline ZlocationResponseHandler(IzatProvider* in) : mIzatProvider(in) {}
        ~ZlocationResponseHandler() = default;
        virtual void reportLocation(const LocationReport*, int locRptCnt, const ILocationProvider*);
        virtual void reportError(const IZatLocationError*, const ILocationProvider*);
        void convertToLocation(const izat_zaxis::FusedLocationReport* in, Location&);
        void convertToFusedLocationReport(izat_zaxis::FusedLocationReport&, Location&);
    private:
        IzatProvider* mIzatProvider;
};

class NLPResponseListener : public IOSListener {
    public:
        inline NLPResponseListener(IzatProvider* in): mIzatProvider(in) {}
        inline virtual IzatListenerMask listensTo() const override {
            return IZAT_STREAM_NETWORK_NOMINAL;
        }
        virtual void onLocationChanged (const IzatLocation *location, int locCnt,
            const IzatLocationStatus status) override;
        virtual void onStatusChanged (const IzatProviderStatus status) override {}
        virtual void onNmeaChanged (const IzatNmea * nmea) override {}
    private:
        IzatProvider* mIzatProvider;
};

class IzatProvider: public IzatProviderBase {
    public:
        inline ~IzatProvider() {
            if (NULL == mZaxisProvider) {
                delete mZaxisProvider;
            }
        }
        static IzatProviderBase* getInstance();
        virtual void setIzatContext(void* izatManager, void* context) override;
        virtual void connectToZProvider() override;
        virtual void registerIzatOdcpiCallback() override;
        virtual void getLocationWithZ(Location location, trackingCallback broadcaster) override;
        void broadCastLocationWithZ(Location);
        void injectNLPFix(double latitude, double longitude, float accuracy);
        void setNlpRequestStatus(bool isReqActive) override;
    private:
        IzatProvider();
        static IzatProviderBase* sIzatProviderHandle;
        static std::mutex mIzatProviderLock;
        ILocationAPI* mLocationApi;
        LocationCallbacks mLocationCbs;
        trackingCallback mBroadcaster;
        const GnssInterface* mGnssInterface;
        AltitudeReceiverProxy* mZaxisProvider;
        const struct s_IzatContext* mIzatContext;
        IzatManager* mIzatManager;
        ZlocationResponseHandler mZlocationResponseHandler;
        NLPResponseListener mNLPResponseListener;
        bool mEmeregency;
        bool mIsOdcpiSessionInProgress;
        bool mZaxisListernerAdded;
        uint32_t mSessionId;
        bool mFlpSessionStarted;
        bool mNlpReqActive;
        LocationCapabilitiesMask mCapabilitiesMask;
        void registerLocationCbs();
        void startFLPSession();
        void stopFLPSession();
        void odcpiInject(Location location);
        void odcpiRequestCb(const OdcpiRequestInfo& odcpiRequest);
        // Mandatory LocationAPI callbacks
        void onCollectiveResponseCb(size_t, LocationError*, uint32_t*){ return; }
        void onResponseCb(LocationError locErrCode, uint32_t sessionId);
        void onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask);
        void onTrackingCb(const Location&){ return; }
};
#endif
