/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Altitude Receiver Proxy

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef ALTITUDE_RECEIVER_H
#define ALTITUDE_RECEIVER_H

#include "LocTimer.h"
#include "LocationProvider.h"
#include <list>
#include <memory>
#include "LocationDataTypes.h"

using namespace std;
using namespace izat_manager;

namespace izat_zaxis {
class AltitudeReqTimer;

class FusedLocationReport : public LocationReport {
public:
    uint16_t mSpoofMask;
    uint64_t mElapsedRealTimeUnc;
    bool mIsValid;
    inline FusedLocationReport() : mSpoofMask(0), mElapsedRealTimeUnc(0),
            mIsValid(false) {}
    FusedLocationReport& operator=(const FusedLocationReport& loc) {
        LocationReport::operator=(loc);
        mSpoofMask = loc.mSpoofMask;
        mElapsedRealTimeUnc = loc.mElapsedRealTimeUnc;
        mIsValid = true;
        return *this;
    }

    void reset() {
        LocationReport::reset();
        mSpoofMask = 0;
        mElapsedRealTimeUnc = 0;
        mIsValid = false;
    }
};

enum AltitudeQueryStatus {
    WAIT_FOR_FIRST_FIX,
    QUERY_PENDING,
    QUERY_ALLOWED,
};

class AltitudeReceiverProxy : public LocationProvider {
public:
    AltitudeReceiverProxy(const struct s_IzatContext* izatContext);
    void getAltitudeFromLocation(const FusedLocationReport& location, bool isEmergency);
    void setAltitudeListener(const ILocationResponse* listener, bool enable);

    // IIPCMessagingResponse overrides
    virtual void handleMsg(InPostcard * const /*in_card*/) override;
private:
    void sendAltitudeLookUpRequest(const FusedLocationReport& location, bool isEmergency);
    void reportAltitude(const FusedLocationReport& location, bool status);

    list<pair<FusedLocationReport, shared_ptr<AltitudeReqTimer>>> mLocationQueue;
    FusedLocationReport mLatestLocation;
    FusedLocationReport mLocationUsedInQuery;
    AltitudeQueryStatus mQueryStatus;
    const std::string mIPCTag;

    struct altitudeReqMsg : public LocMsg {
        AltitudeReceiverProxy& mAltReceiver;
        FusedLocationReport mLocation;
        bool mIsEmergency;

        inline altitudeReqMsg(AltitudeReceiverProxy& altReceiver,
                const FusedLocationReport& loc, bool isEmergency) :
            mAltReceiver(altReceiver), mLocation(loc),
            mIsEmergency(isEmergency) {
            //Set altitude flag to false before requesting zaxis from 3rd party Z-Provider
            mLocation.mHasAltitudeWrtEllipsoid = false;
            mLocation.mAltitudeWrtEllipsoid = 0;
        }
        virtual ~altitudeReqMsg() {}
        void proc() const;
    };

    struct reportAltitudeMsg : public LocMsg {
        AltitudeReceiverProxy& mAltReceiver;
        FusedLocationReport mLocation;

        inline reportAltitudeMsg(AltitudeReceiverProxy& altReceiver,
                FusedLocationReport& loc) :
                mAltReceiver(altReceiver), mLocation(loc) {}
        virtual ~reportAltitudeMsg() {}
        void proc() const;
    };

    struct altitudeReqTimerExpireMsg : public LocMsg {
        AltitudeReceiverProxy& mAltReceiver;
        FusedLocationReport mLocation;

        inline altitudeReqTimerExpireMsg(AltitudeReceiverProxy& altReceiver,
                const FusedLocationReport& loc) :
                mAltReceiver(altReceiver), mLocation(loc) {}
        virtual ~altitudeReqTimerExpireMsg() {}
        void proc() const;
    };

    struct setAltitudeQueryAllowedMsg : public LocMsg {
        AltitudeReceiverProxy& mAltReceiver;

        inline setAltitudeQueryAllowedMsg(AltitudeReceiverProxy& altReceiver) :
                mAltReceiver(altReceiver) {}
        virtual ~setAltitudeQueryAllowedMsg() {}
        void proc() const;
    };

    struct setListenerMsg : public LocMsg {
        AltitudeReceiverProxy& mAltReceiver;
        bool mIsEnabled;

        inline setListenerMsg(AltitudeReceiverProxy& altReceiver, bool isEnabled) :
                mAltReceiver(altReceiver), mIsEnabled(isEnabled) {}
        virtual ~setListenerMsg() {}
        void proc() const;
    };

};
}
#endif
