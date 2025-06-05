/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Altitude Receiver Proxy

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_zaxis"

#include "AltitudeReceiverProxy.h"
#include "IzatAltitudeReceiver.h"
#include<log_util.h>
#include<functional>
#include <thread>
#include <unistd.h>
#include <inttypes.h>

#define TIMEOUT_IN_MS           (5000)

namespace izat_zaxis {

using NlpLocation = izat_remote_api::AltitudeReceiverUpdater::NlpLocation;

class AltitudeReqTimer : public LocTimer {
    function<void()> mTimeOutFunc;
public:
    inline AltitudeReqTimer(function<void()> timeoutFunc) :
            mTimeOutFunc(timeoutFunc) {
        LOC_LOGd("AltitudeReqTimer ctor");
    }

    inline ~AltitudeReqTimer() {
        LOC_LOGd("~AltitudeReqTimer");
    }
    virtual void timeOutCallback() {
        mTimeOutFunc();
    }
};

AltitudeReceiverProxy::AltitudeReceiverProxy(const struct s_IzatContext* izatContext) :
    LocationProvider(izatContext),
    mIPCTag("ALT-RECV-PROXY"),
    mQueryStatus(WAIT_FOR_FIRST_FIX) {
    mIzatContext->mIPCMessagingProxyObj->registerResponseObj(mIPCTag.c_str(), this);
}

//This function handles IPC message from NLP_API
void AltitudeReceiverProxy::handleMsg(InPostcard * const in_card) {
    int result = -1;

    do {
        BREAK_IF_ZERO(1, in_card);
        const char* resp = NULL;
        const char* from = NULL;
        BREAK_IF_NON_ZERO(2, in_card->getString("FROM", &from));

        LOC_LOGD("FROM: %s", from);
        if (0 == strcmp(from, "ALT-PROVIDER")) {
            FusedLocationReport location;
            NlpLocation locWithZ;
            const void* blob = nullptr;
            size_t  length = 0;
            memset(&locWithZ, 0, sizeof(NlpLocation));
            memset(&location, 0, sizeof(FusedLocationReport));
            if (0 == in_card->getBlob("LOC_WITH_Z", &blob, &length) &&
                    length == sizeof(NlpLocation)) {
                locWithZ = *(NlpLocation*)blob;
                if ((locWithZ.locationFlagsMask & NlpLocation::ALTITUDERECEIVER_NLPLOC_LAT_LONG) &&
                    (locWithZ.locationFlagsMask & NlpLocation::ALTITUDERECEIVER_NLPLOC_ALTITUDE) &&
                    (locWithZ.locationFlagsMask &
                     NlpLocation::ALTITUDERECEIVER_NLPLOC_VERTICAL_ACCURACY)) {
                    location.mHasAltitudeWrtEllipsoid = true;
                    location.mElapsedRealTimeInNanoSecs =
                        (int64_t)locWithZ.elapsedRealTimeInMs * 1000000;
                    location.mUtcTimestampInMsec = (int64_t) locWithZ.timestamp;
                    location.mLatitude = locWithZ.latitude;
                    location.mLongitude = locWithZ.longitude;
                    location.mAltitudeWrtEllipsoid = locWithZ.altitude;
                    location.mVertUnc = locWithZ.verticalAccuracy;
                }
            }
            mIzatContext->mMsgTask->sendMsg(
                    new (nothrow) reportAltitudeMsg(*this, location));
        }
    } while (0);
}

//Send Altitude Lookup request to NLP_API
void AltitudeReceiverProxy::sendAltitudeLookUpRequest(const FusedLocationReport& location,
        bool isEmergency) {
    OutPostcard* card = OutPostcard::createInstance();
    if (nullptr != card) {
        card->init();
        card->addString("TO", "ALT-PROVIDER");
        card->addString("FROM", mIPCTag.c_str());
        card->addString("INFO", "ALT");
        card->addBool("IS_EMERGENCY", isEmergency);
        card->addDouble("LATITUDE", location.mLatitude);
        card->addDouble("LONGITUDE", location.mLongitude);
        card->addFloat("ACCURACY", location.mHorizontalAccuracy);
        card->addUInt64("TIMESTAMP-MS", (uint64_t)location.mUtcTimestampInMsec);
        card->addUInt64("ELAPSED_REAL_TIME-MS",
                (uint64_t)location.mElapsedRealTimeInNanoSecs/1000000);
        card->finalize();
        //Send post card
        mIzatContext->mIPCMessagingProxyObj->sendMsg(card, "ALT-PROVIDER");
    }
}

//Public APIs for altitude listener
void AltitudeReceiverProxy::getAltitudeFromLocation(const FusedLocationReport& location,
        bool isEmergency) {
    string locStr;
    location.stringify(locStr);
    LOC_LOGd("getAltitudeFromLocation, %s", locStr.c_str());
    mIzatContext->mMsgTask->sendMsg(
            new (nothrow) altitudeReqMsg(*this, location, isEmergency));
}

void AltitudeReceiverProxy::setAltitudeListener(const ILocationResponse* listener, bool enable) {
    if (enable) {
        subscribe(listener);
    } else {
        unsubscribe(listener);
    }
    mIzatContext->mMsgTask->sendMsg(new (nothrow) setListenerMsg(*this, enable));
}

void AltitudeReceiverProxy::reportAltitude(const FusedLocationReport& location, bool status) {
    string locStr;
    location.stringify(locStr);
    LOC_LOGd("reportAltitude:broadcastLocation to client: %s, altitude: %f,"
            "elapsedRealTime: %" PRIi64,
            locStr.c_str(), location.mAltitudeWrtEllipsoid, location.mElapsedRealTimeInNanoSecs);
    //send location with Z to client
    broadcastLocation(&location);
    mLocationUsedInQuery.reset();
}

//Message proc functions
void AltitudeReceiverProxy::altitudeReqMsg::proc() const {
    LOC_LOGd("altitudeReqMsg::proc(), mQueryStatus = %d, Previous query in progress: %d",
            mAltReceiver.mQueryStatus, mAltReceiver.mLocationUsedInQuery.mIsValid);
    if (mAltReceiver.mQueryStatus == WAIT_FOR_FIRST_FIX) {
        mAltReceiver.mIzatContext->mMsgTask->sendMsg(
                new (nothrow) setAltitudeQueryAllowedMsg(mAltReceiver), TIMEOUT_IN_MS);
        mAltReceiver.mQueryStatus = QUERY_PENDING;
    }
    if (mAltReceiver.mQueryStatus == QUERY_ALLOWED) {
        mAltReceiver.mLatestLocation = mLocation;
        if (!mAltReceiver.mLocationUsedInQuery.mIsValid) {
            mAltReceiver.mLocationUsedInQuery = mLocation;
            LOC_LOGd("sendAltitudeLookUpRequest, elapsedRealTime: %" PRIi64,
                    mLocation.mElapsedRealTimeInNanoSecs);
            mAltReceiver.sendAltitudeLookUpRequest(mLocation, mIsEmergency);
            mAltReceiver.mIzatContext->mMsgTask->sendMsg(
                new (nothrow) altitudeReqTimerExpireMsg(mAltReceiver, mLocation), TIMEOUT_IN_MS);
        }
    }
}

void AltitudeReceiverProxy::reportAltitudeMsg::proc() const {
    LOC_LOGd("reportAltitudeMsg::proc()");
    if(!mAltReceiver.mLocationUsedInQuery.mIsValid ||
            mAltReceiver.mLocationUsedInQuery.mElapsedRealTimeInNanoSecs/1000000 !=
            mLocation.mElapsedRealTimeInNanoSecs/1000000) {
        LOC_LOGd("timestamp of loc returned from ZProvider mismatch!");
    } else if (mLocation.mHasAltitudeWrtEllipsoid) {
        mAltReceiver.mLocationUsedInQuery.mHasAltitudeWrtEllipsoid = true;
        mAltReceiver.mLocationUsedInQuery.mAltitudeWrtEllipsoid = mLocation.mAltitudeWrtEllipsoid;
        mAltReceiver.mLocationUsedInQuery.mHasVertUnc = true;
        mAltReceiver.mLocationUsedInQuery.mVertUnc = mLocation.mVertUnc;
        mAltReceiver.reportAltitude(mAltReceiver.mLocationUsedInQuery, true);
    } else if (mAltReceiver.mLatestLocation.mIsValid) {
        mAltReceiver.reportAltitude(mAltReceiver.mLatestLocation, false);
    }
}


void AltitudeReceiverProxy::altitudeReqTimerExpireMsg::proc() const {
    LOC_LOGd("altitudeReqTimerExpireMsg::proc()");
    if (mAltReceiver.mLocationUsedInQuery.mIsValid &&
            mAltReceiver.mLocationUsedInQuery.mElapsedRealTimeInNanoSecs ==
            mLocation.mElapsedRealTimeInNanoSecs) {
        LOC_LOGd("Altitude look up request time out, report latest location");
        mAltReceiver.reportAltitude(mAltReceiver.mLatestLocation, false);
    }
}

void AltitudeReceiverProxy::setAltitudeQueryAllowedMsg::proc() const {
    LOC_LOGd("setAltitudeQueryAllowedMsg::proc()");
    mAltReceiver.mQueryStatus = QUERY_ALLOWED;
}

void AltitudeReceiverProxy::setListenerMsg::proc() const {
    LOC_LOGd("setListenerMsg::proc()");
    mAltReceiver.mLocationUsedInQuery.reset();
    mAltReceiver.mLatestLocation.reset();
    mAltReceiver.mQueryStatus = WAIT_FOR_FIRST_FIX;
}
}
