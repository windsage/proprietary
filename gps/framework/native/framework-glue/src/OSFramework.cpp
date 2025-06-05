/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017,2020-2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/


#define LOG_TAG "OSFramework"
#define LOG_NDEBUG 0

#include "OSFramework.h"
#include <loc_pla.h>

#include "Subscription.h"
#include "FrameworkActionRequest.h"
#include "IzatDefines.h"
#include "OSFrameworkCleaner.h"
#include "OsNpGlue.h"
#include "IzatLocation.h"

using namespace loc_util;

void Listener::onLocationChanged(
        const izat_manager::IzatLocation * izatLocation, int locCnt,
        const izat_manager::IzatLocationStatus) {
    izat_manager::IzatLocation* locToReport = nullptr;
    for (int i=0; i<locCnt; ++i) {
        //report non GTP Wifi RTT fix to OSFramework::listener, eg. GTP Cell, GTP Wifi RSSI
        if (izatLocation[i].mHasNetworkPositionSource &&
                izatLocation[i].mNetworkPositionSource != IZAT_NETWORK_POSITION_FROM_WIFI_RTT_FTM &&
                izatLocation[i].mNetworkPositionSource !=
                IZAT_NETWORK_POSITION_FROM_WIFI_RTT_SERVER) {
            locToReport = const_cast<izat_manager::IzatLocation*>(izatLocation) + i;
            break;
        }
    }

    if (locToReport != nullptr && locToReport->isValid()) {
        Location location = {};
        if (locToReport->mHasLatitude && locToReport->mHasLongitude) {
            location.flags |= LOCATION_HAS_LAT_LONG_BIT;
            location.latitude = locToReport->mLatitude;
            location.longitude = locToReport->mLongitude;
        }

        if (locToReport->mHasAltitudeWrtEllipsoid) {
            location.flags |= LOCATION_HAS_ALTITUDE_BIT;
            location.altitude = locToReport->mAltitudeWrtEllipsoid;
        }

        if (locToReport->mHasSpeed) {
            location.flags |= LOCATION_HAS_SPEED_BIT;
            location.speed = locToReport->mSpeed;
        }

        if (locToReport->mHasBearing) {
            location.flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
            location.bearing = locToReport->mBearing;
        }

        if (locToReport->mHasHorizontalAccuracy) {
            location.flags |= LOCATION_HAS_ACCURACY_BIT;
            location.accuracy = locToReport->mHorizontalAccuracy;
        }

        if (locToReport->mHasVertUnc) {
            location.flags |= LOCATION_HAS_VERTICAL_ACCURACY_BIT;
            location.verticalAccuracy = locToReport->mVertUnc;
        }

        if (locToReport->mHasSpeedUnc) {
            location.flags |= LOCATION_HAS_SPEED_ACCURACY_BIT;
            location.speedAccuracy = locToReport->mSpeedUnc;
        }

        if (locToReport->mHasBearingUnc) {
            location.flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
            location.bearingAccuracy = locToReport->mBearingUnc;
        }

        if (locToReport->mHasPositionTechMask) {
            location.techMask = locToReport->mPositionTechMask;
        }

        if (locToReport->mHasUtcTimestampInMsec) {
            location.timestamp = locToReport->mUtcTimestampInMsec;
        }

        TCallbackCol::const_iterator it = mCallbackSet.begin();
        while (it != mCallbackSet.end()) {
            (* *it)(location);
            ++it;
        }
    }
}

void Listener::onStatusChanged(const izat_manager::IzatProviderStatus status) {
    LOC_LOGd("provider status: %d", status);
}

OSFramework* OSFramework::mOSFramework = NULL;

OSFramework::OSFramework()
{
    int result = -1;
    ENTRY_LOG();

    do {
        // Create IzatManager
        IIzatManager *pIzatManager = getIzatManager(this);
        BREAK_IF_ZERO(1, pIzatManager);
        pIzatManager->subscribeListener(&mListener);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

OSFramework::~OSFramework()
{
    ENTRY_LOG();

    IIzatManager *pIzatManager = getIzatManager(this);
    if (!pIzatManager) {
        LOC_LOGe("null izat manager");
        return;
    }
    pIzatManager->unsubscribeListener(&mListener);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

OSFramework* OSFramework::getOSFramework()
{
    int result = -1;

    ENTRY_LOG();
    do {
          // already initialized
          BREAK_IF_NON_ZERO(0, mOSFramework);

          mOSFramework = new (std::nothrow) OSFramework();
          BREAK_IF_ZERO(2, mOSFramework);

          result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mOSFramework;
}

//Not recommended to call this function in anywhere
void OSFramework::destroyInstance()
{
    ENTRY_LOG();

    if (mOSFramework) {
        delete mOSFramework;
        mOSFramework = NULL;
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

// For use in dload from Opensource modules
extern "C" void createOSFramework()
{
    OSFramework::getOSFramework();
}

//Not recommended to call this function, otherwise it may has risk
//for non pointer deference issue
extern "C" void destroyOSFramework()
{
    OSFramework::destroyInstance();
}

extern "C" IIzatManager* getOSFrameworkIzatManager()
{
    IIzatManager* izatManager = getIzatManager(OSFramework::getOSFramework());
    return izatManager;
}

// IOSFramework overrides
IDataItemSubscription* OSFramework::getDataItemSubscriptionObj()
{
    return Subscription::getSubscriptionObj();
}

IFrameworkActionReq* OSFramework::getFrameworkActionReqObj()
{
    return FrameworkActionRequest::getFrameworkActionReqObj();
}

IOSLocationProvider * OSFramework::getOSNP()
{
    return OsNpGlue::getInstance();
}

IOSFrameworkCleaner * OSFramework::getOSFrameworkCleaner()
{
    return OSFrameworkCleaner::getInstance();
}

void OSFramework::enableNetworkProvider()
{
    IIzatManager* izatManager = getIzatManager(OSFramework::getOSFramework());
    if (!izatManager) {
        LOC_LOGe("null izat manager");
        return ;
    }
    izatManager->enableProvider(IZAT_STREAM_NETWORK);
}

void OSFramework::disableNetworkProvider()
{
    IIzatManager* izatManager = getIzatManager(OSFramework::getOSFramework());
    if (!izatManager) {
        LOC_LOGe("null izat manager");
        return ;
    }
    izatManager->disableProvider(IZAT_STREAM_NETWORK);
}

void OSFramework::startNetworkLocation(trackingCallback* callback)
{
    struct StartNetworkLocationReqMsg : public LocMsg {
        StartNetworkLocationReqMsg(trackingCallback* callback) :
                mCallback(callback) {}
        virtual ~StartNetworkLocationReqMsg() {}
        void proc() const {
            OSFramework* pOSFramework = OSFramework::getOSFramework();
            IIzatManager* izatManager = getIzatManager(pOSFramework);
            if (pOSFramework && izatManager) {
                pOSFramework->mListener.mCallbackSet.insert(mCallback);
                IzatRequest* request = IzatRequest::createProviderRequest(
                        IZAT_STREAM_NETWORK, 0, 0, false);
                izatManager->addRequest(request);
            }
        }
        trackingCallback* mCallback;
    };

    MsgTask* pMsgTask = getIZatManagerMsgTask();
        if (pMsgTask) {
            pMsgTask->sendMsg(new (nothrow) StartNetworkLocationReqMsg(callback));
        }
    }

void OSFramework::stopNetworkLocation(trackingCallback* callback)
{
    struct StopNetworkLocationReqMsg : public LocMsg {
        StopNetworkLocationReqMsg(trackingCallback* callback) :
                mCallback(callback) {}
        virtual ~StopNetworkLocationReqMsg() {}
        void proc() const {
            OSFramework* pOSFramework = OSFramework::getOSFramework();
            IIzatManager* izatManager = getIzatManager(pOSFramework);
            if (pOSFramework && izatManager) {
                pOSFramework->mListener.mCallbackSet.erase(mCallback);
                if (0 == pOSFramework->mListener.mCallbackSet.size()) {
                        IzatRequest* request = IzatRequest::createProviderRequest(
                                IZAT_STREAM_NETWORK, 0, 0, false);
                        if (request) {
                            izatManager->removeRequest(request);
                        }
                    }
                }
            }
        trackingCallback* mCallback;
    };

    MsgTask* pMsgTask = getIZatManagerMsgTask();
        if (pMsgTask) {
            pMsgTask->sendMsg(new (nothrow) StopNetworkLocationReqMsg(callback));
        }
    }

#ifdef USE_GLIB
// For use in dload from Opensource modules
extern "C" void enableNetworkProvider()
{
    OSFramework::enableNetworkProvider();
}

extern "C" void disableNetworkProvider()
{
    OSFramework::disableNetworkProvider();
}

extern "C" void startNetworkLocation(trackingCallback* callback)
{
    OSFramework::startNetworkLocation(callback);
}

extern "C" void stopNetworkLocation(trackingCallback* callback)
{
    OSFramework::stopNetworkLocation(callback);
}
#endif
