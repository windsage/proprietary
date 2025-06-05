/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat Provider

  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include "IzatProvider.h"

#define LOG_TAG "Izat_Provider"
#define NETWORK_PROVIDER IzatRequest::IzatProviderType::IZAT_STREAM_NETWORK
#define ACCURACY_THRESHOLD 25000

std::mutex IzatProvider::mIzatProviderLock;
IzatProviderBase* IzatProvider::sIzatProviderHandle = NULL;

IzatProvider::IzatProvider() : mLocationApi(NULL), mLocationCbs({}),
    mSessionId(0), mCapabilitiesMask(0), mGnssInterface(NULL),
    mIzatContext(NULL), mZaxisProvider(NULL), mZlocationResponseHandler(this),
    mNLPResponseListener(this), mEmeregency(false), mIsOdcpiSessionInProgress(false),
    mZaxisListernerAdded(false), mFlpSessionStarted(false), mIzatManager(NULL),
    mBroadcaster(NULL), mNlpReqActive(false) {
}


IzatProviderBase* IzatProvider::getInstance() {
    mIzatProviderLock.lock();
    if (NULL == sIzatProviderHandle) {
        sIzatProviderHandle = new IzatProvider();
    }
    mIzatProviderLock.unlock();
    return sIzatProviderHandle;
}

extern "C" IzatProviderBase* getIzatProvider() {
    return IzatProvider::getInstance();
}


void IzatProvider::connectToZProvider() {
    //Altitute Receiver Proxy is loaded once Z-Provider is connected
    if (NULL == mZaxisProvider && NULL != mIzatManager) {
        mZaxisProvider = new AltitudeReceiverProxy(mIzatContext);
    }

    // Check if any active odcpi session, add ZlocatioResponseHandler listener
    if (mIsOdcpiSessionInProgress && mEmeregency && mZaxisProvider) {
        LOC_LOGd("Odcpi session active, add ZlocatioResponseHandler listener");
        mZaxisProvider->setAltitudeListener(&mZlocationResponseHandler, true);
        mZaxisListernerAdded = true;
    }
}

void IzatProvider::setIzatContext(void* izatManager, void* context) {
    mIzatContext = static_cast<struct s_IzatContext*>(context);
    mIzatManager = static_cast<IzatManager*>(izatManager);
    mIzatManager->subscribeListener(&mNLPResponseListener);
    //Register Flp callback wrapper with LocationAPI
    LOC_LOGd("Register location Cb's");
    registerLocationCbs();
    //Send WiFi hardware on data item here for KaiOS
    WifiHardwareStateDataItem wifistate_data_item;
    wifistate_data_item.mEnabled = true;
    mIzatContext->mSystemStatusOsObsrvr->notify({&wifistate_data_item});
}

void IzatProvider::getLocationWithZ(Location location, trackingCallback broadcaster) {
    LOC_LOGd("Received HW-FLP intermidiate fix, accuracy: %f", location.accuracy);
    if (location.accuracy > 25000) {
        LOC_LOGd("Location accuracy is larger than 25km, ignore");
        return;
    }
    mBroadcaster = broadcaster;
    if (mZaxisProvider && mZaxisListernerAdded) {
        FusedLocationReport fusedLocationReport = {};
        if (location.flags & (LOCATION_HAS_LAT_LONG_BIT | LOCATION_HAS_ACCURACY_BIT)) {
            mZlocationResponseHandler.convertToFusedLocationReport(fusedLocationReport,
                    location);
            mZaxisProvider->getAltitudeFromLocation(fusedLocationReport, mEmeregency);
        } else {
            LOC_LOGe("location does not have valid messages, cannot convert.");
        }

    } else {
        LOC_LOGd("No Z Provider, inject directly");
        broadCastLocationWithZ(location);
    }
}

void IzatProvider::broadCastLocationWithZ(Location location) {
    if (mBroadcaster) {
        mBroadcaster(location);
    }
    if (mEmeregency) {
        odcpiInject(location);
    }
}

void IzatProvider::registerIzatOdcpiCallback() {
    if (mIzatManager) {
        mGnssInterface = mIzatManager->getGnssInterface();
        if (mGnssInterface) {
            LOC_LOGd("Registering ODCPI callback...");
            odcpiRequestCallback cb = [this](const OdcpiRequestInfo& odcpiRequest) {
                odcpiRequestCb(odcpiRequest);
            };
            mGnssInterface->odcpiInit(cb, OdcpiPrioritytype::ODCPI_HANDLER_PRIORITY_MEDIUM,
                    (NON_EMERGENCY_ODCPI | EMERGENCY_ODCPI));
        } else {
            LOC_LOGe("Failed to registering ODCPI callback...");
        }
    }
}

void IzatProvider::setNlpRequestStatus(bool isReqActive) {
    LOC_LOGd("NLP request status: %d", isReqActive);
    IzatRequest* req = IzatRequest::createCriteriaRequest(
            IzatHorizontalAccuracy::IZAT_HORIZONTAL_FINE,
            IzatOtherAccuracy:: IZAT_OTHER_HIGH,
            IzatOtherAccuracy:: IZAT_OTHER_HIGH,
            10000, 0, false, false);
    req->setProvider(IzatRequest::IzatProviderType::IZAT_STREAM_NETWORK);
    if (isReqActive && !mNlpReqActive) {
        mNlpReqActive = true;
        mIzatManager->addRequest(req);
    } else if (!isReqActive) {
        mNlpReqActive = false;
        mIzatManager->removeRequest(req);
    }
}

// Add Z-axis listener non-emergency and emergency case as well
// But reporting the location with Z is Activated for only Emergency request
// Below odcpiRequestCb would handle
//   CASE 1: For non-emergency request, where NLP fix is triggerd as part tracking session ODCPI
//   CASE 2: For emergency request, start FLP session. But regardless, we start NLP here,
//           because for handling ODCPI, modem is not going to send us another ODCPI request
//           as part of the regular FLP call flow
void IzatProvider::odcpiRequestCb(const OdcpiRequestInfo& odcpiRequest) {
    ENTRY_LOG();
    LOC_LOGd("Odcpi request in start:0 stop:1: %d", odcpiRequest.type);
    if (ODCPI_REQUEST_TYPE_START == odcpiRequest.type) {
        mEmeregency = odcpiRequest.isEmergencyMode;
        if (mZaxisProvider) {
            if (mEmeregency && !mZaxisListernerAdded) {
                LOC_LOGd("ZlocatioResponseHandler Listener added");
                mZaxisProvider->setAltitudeListener(&mZlocationResponseHandler, true);
                mZaxisListernerAdded = true;
            }
        }
        // Start FLP session in E-911 case
        if (mEmeregency && !mFlpSessionStarted) {
            LOC_LOGd("Start FLP session");
            mFlpSessionStarted = true;
            startFLPSession();
        }
        // Check if there is existing NLP session on-going
        if (mIsOdcpiSessionInProgress) {
            LOC_LOGd("Odcpi Session started Already");
            return;
        }

        if (!mNlpReqActive) {
            // Add NLP fix request and listener when mNlpReqActive is false
            IzatRequest* request = mIzatManager->createIzatReqest(odcpiRequest, NETWORK_PROVIDER);
            request->setProvider(NETWORK_PROVIDER);
            request->setInEmergency(true);
            mIzatManager->addRequest(request);
        }
        mIsOdcpiSessionInProgress = true;
    } else if (ODCPI_REQUEST_TYPE_STOP == odcpiRequest.type) {
        if (mZaxisProvider) {
            if (mEmeregency && mZaxisListernerAdded) {
                LOC_LOGd("ZlocatioResponseHandler Listener removed");
                mZaxisProvider->setAltitudeListener(&mZlocationResponseHandler, false);
                mZaxisListernerAdded = false;
            }
        }
        // stop active FLP session
        if (mEmeregency && mFlpSessionStarted) {
            LOC_LOGd("Stop FLP session");
            stopFLPSession();
        }

        if (!mNlpReqActive) {
            //Remove NLP fix request and listener
            IzatRequest* request = mIzatManager->createIzatReqest(odcpiRequest, NETWORK_PROVIDER);
            request->setProvider(NETWORK_PROVIDER);
            mIzatManager->removeRequest(request);
        }
        mIsOdcpiSessionInProgress = false;
        mEmeregency = false;
    }
}

void IzatProvider::startFLPSession() {
    if (mSessionId > 0) {
        LOC_LOGd("Already FLPSession started with session id: %d", mSessionId);
    }
    TrackingOptions options;
    options.size = sizeof(LocationOptions);
    options.minInterval = (uint32_t)(1000);
    options.qualityLevelAccepted = QUALITY_ANY_VALID_FIX;
    if (mCapabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSB_BIT) {
        options.mode = GNSS_SUPL_MODE_MSB;
    }
    if (mLocationApi) {
        mSessionId = mLocationApi->startTracking(options);
        LOC_LOGd("FLPSession started with session id: %d", mSessionId);
    }
}

void IzatProvider::stopFLPSession() {
    if (mSessionId > 0 && mLocationApi) {
        mLocationApi->stopTracking(mSessionId);
        LOC_LOGd("FLPSession stopped with session id: %d", mSessionId);
        mFlpSessionStarted = false;
        mSessionId = 0;
    }
}

void IzatProvider::onResponseCb(LocationError locErrCode, uint32_t sessionId) {
    LOC_LOGd("ResponseCb: LocationError: %d, SessionId: %d", locErrCode, sessionId);
    if (LOCATION_ERROR_SUCCESS != locErrCode) {
        mFlpSessionStarted = false;
    }
}

void IzatProvider::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) {
    LOC_LOGd("capabilitiesMask: 0x%x", capabilitiesMask);
    mCapabilitiesMask = capabilitiesMask;
}

void IzatProvider::registerLocationCbs() {
    if (NULL == mLocationApi) {
        mLocationCbs.size = sizeof(mLocationCbs);
        mLocationCbs.trackingCb = nullptr;
        mLocationCbs.batchingCb = nullptr;
        mLocationCbs.geofenceBreachCb = nullptr;
        mLocationCbs.geofenceStatusCb = nullptr;
        mLocationCbs.gnssNiCb = nullptr;
        mLocationCbs.gnssSvCb = nullptr;
        mLocationCbs.gnssNmeaCb =  nullptr;
        mLocationCbs.gnssMeasurementsCb = nullptr;
        mLocationCbs.batchingStatusCb = nullptr;

        // mandatory callback
        mLocationCbs.capabilitiesCb = [this](LocationCapabilitiesMask capabilitiesMask) {
            onCapabilitiesCb(capabilitiesMask);
        };

        // mandatory callback
        mLocationCbs.responseCb = [this](LocationError err, uint32_t id_t) {
            onResponseCb(err, id_t);
        };

        // mandatory callback
        mLocationCbs.collectiveResponseCb = [this](size_t count, LocationError* errs,
                uint32_t* ids) {
            onCollectiveResponseCb(count, errs, ids);
        };

        mLocationCbs.trackingCb = [this](const Location& locInfo) {
            onTrackingCb(locInfo);
        };
        mLocationApi = LocationAPI::createInstance(mLocationCbs);
        if (mLocationApi == NULL) {
            LOC_LOGd("failed to create LocationAPI instance");
        }
    }
}

void IzatProvider::odcpiInject(Location location) {
    if (mGnssInterface) {
        LOC_LOGd("Inject Fused location");
        location.techMask |= LOCATION_TECHNOLOGY_HYBRID_ALE_BIT;
        mGnssInterface->odcpiInject(location);
    }
}

void IzatProvider::injectNLPFix(double latitude, double longitude, float accuracy) {
    if (mGnssInterface) {
        LOC_LOGd("Inject NLP fix");
        mGnssInterface->injectLocation(latitude, longitude, accuracy);
    }
}

// NLP fix listener
void NLPResponseListener::onLocationChanged(const IzatLocation *izatLocation, int locCnt,
        const IzatLocationStatus status) {
    if (IZAT_LOCATION_STATUS_FINAL == status) {
        IzatLocation* nlpLocation = nullptr;
        for (int i=0; i<locCnt; ++i) {
            //report non GTP Wifi RTT fix to NLP listener, eg. GTP Cell, GTP Wifi RSSI
            if (izatLocation[i].mHasNetworkPositionSource &&
                    izatLocation[i].mNetworkPositionSource !=
                    IZAT_NETWORK_POSITION_FROM_WIFI_RTT_FTM &&
                    izatLocation[i].mNetworkPositionSource !=
                    IZAT_NETWORK_POSITION_FROM_WIFI_RTT_SERVER) {
                nlpLocation = const_cast<izat_manager::IzatLocation*>(izatLocation) + i;
                break;
            }
        }
        if (nlpLocation != nullptr && nlpLocation->isValid()) {
            mIzatProvider->injectNLPFix(nlpLocation->mLatitude, nlpLocation->mLongitude,
                    nlpLocation->mHorizontalAccuracy);
        }
    }
}
