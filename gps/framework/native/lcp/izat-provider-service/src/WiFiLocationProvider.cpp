/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_WiFiLocationProvider"

#include "WiFiLocationProvider.h"
#include "Utils.h"
#include "IzatDefines.h"
#include "loc_cfg.h"
#include <log_util.h>
#include "SystemStatus.h"
#include <math.h>

namespace izat_manager
{
using namespace qc_loc_fw;

unsigned int WiFiLocationProvider::mMaxAcuracyThresholdToFilterWiFiPosition = 25000;
unsigned int WiFiLocationProvider::mMaxZppWifiConsistencyCheckInMeters = 200;
ILocationProvider* WiFiLocationProvider::mWifiLocationProvider = NULL;
WiFiLocationProvider::ProviderMode WiFiLocationProvider::mProviderMode =
    WiFiLocationProvider::ProviderMode::MODE_DISABLED;

#define BREAK_AND_DELETE_IF_NON_ZERO(ERR, X, OBJ) \
if (0!=(X)) {result = (ERR); delete (OBJ); break;}
#define LOC_MAX_PARAM_STRING               172
// NLP session started data item time threshold value
#define TIME_THRESHOLD_TO_RE_NOTIFY_NLP_STATUS_MS 300*1000

ILocationProvider* WiFiLocationProvider::getInstance(const struct s_IzatContext* izatContext)
{
    int result = -1;
    do {
        //WifiLocationProvider object is already created
        BREAK_IF_NON_ZERO(0, mWifiLocationProvider);
        BREAK_IF_ZERO(1, izatContext);

        readConfig();
        // dont create QNP if GTP mode is disabled.
        BREAK_IF_ZERO(2, (mProviderMode != MODE_DISABLED));

        mWifiLocationProvider = new (nothrow) WiFiLocationProvider(izatContext);
        BREAK_IF_ZERO(4, mWifiLocationProvider);
        result = 0;
    } while (0);

    return mWifiLocationProvider;
}

void WiFiLocationProvider::readConfig()
{
    // read Provider MODE, create wifiLocationProvider only in SDK mode.
    static char conf_gtp_mode[LOC_MAX_PARAM_STRING];

    static loc_param_s_type izat_conf_param_table[] = {
       {"GTP_MODE", &conf_gtp_mode, NULL, 's'}
    };

    UTIL_READ_CONF(LOC_PATH_IZAT_CONF, izat_conf_param_table);
    LOC_LOGD("GTP MODE mode set in izat.conf: %s", conf_gtp_mode);

    mProviderMode = MODE_DISABLED;
    if (strncmp(conf_gtp_mode, "SDK", 3) == 0) {
        LOC_LOGD("GTP WiFi is enabled, gtp mode is SDK.");
        mProviderMode = MODE_SDK;
    }
}

WiFiLocationProvider::WiFiLocationProvider(const struct s_IzatContext* izatContext) :
        LocationProvider(izatContext), mProviderTag("GTP-WIFI-PROXY"),
        mTxId(0), mTimestampLastTrackingRequestInNs(0), mLBSAdapter(nullptr),
        mTBFTimer([this](){
            this->mIzatContext->mMsgTask->sendMsg(
                    new (nothrow) TBFTimerMsg(this));
        }),
        mRequestTimer([this](){
            this->mIzatContext->mMsgTask->sendMsg(
                    new (nothrow) RequestTimerMsg(this));
        }) {
    int result = -1;

    do {
        // Initialize consolidated location request
        mConsolidatedReq.action = LocationRequestAction_Stop;
        mConsolidatedReq.intervalInMsec = 0;
        mConsolidatedReq.emergencyRequest = false;
        mConsolidatedReq.accuracy = LocationRequestAccuracy_Nominal;
        mConsolidatedReq.uid = 0;

        BREAK_IF_ZERO(1, mIzatContext->mIPCMessagingProxyObj);

        // register self with IPCMessagingProxyObj here
        BREAK_IF_NON_ZERO(2,
            mIzatContext->mIPCMessagingProxyObj->registerResponseObj(mProviderTag.c_str(), this));

        mLBSAdapter = LBSAdapter::get(0);
        BREAK_IF_ZERO(3, mLBSAdapter);
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

// IMQServerProxyResponse Overrides
void WiFiLocationProvider::handleMsg(InPostcard * const in_card)
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(2, in_card);

        const char* resp = NULL;
        const char* from = NULL;
        const char* info = NULL;
        int response_txid = 0;

        BREAK_IF_NON_ZERO(3, in_card->getString("FROM", &from));

        //Handle xtwifi SSR case
        //Send positioning request if state is WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION
        if (in_card->getString("INFO", &info) == 0) {
            if (strcmp(info, "WiFi-INIT") == 0) {
                if (0 == strcmp(from, "XTWiFi-SESS-MGR")) {
                    bool status = false;
                    in_card->getBool("STATUS", status);
                    if (status) {
                        mIzatContext->mMsgTask
                                ->sendMsg(new (nothrow) HandleGtpWifiSSRMsg(this));
                    }
                }
            }
        }
        BREAK_IF_NON_ZERO(4, in_card->getString("RESP", &resp));
        BREAK_IF_NON_ZERO(5, in_card->getInt32("TX-ID", response_txid));

        // Automation -log, Please do not touch
        LOC_LOGD("FROM: %s RESP: %s TX-ID: %d", from, resp, response_txid);
        if (0 == strcmp(resp, "POSITION")) {
            InPostcard * copy = InPostcard::createInstance();
            BREAK_AND_DELETE_IF_NON_ZERO(4, copyInCard(in_card, copy), copy);

            mIzatContext->mMsgTask->sendMsg(new (nothrow) PositionMsgWifi(this, copy));
        } else {
            result = 6;
            break;
        }

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return;
}

int WiFiLocationProvider::setRequest(const LocationRequest * request) {
    LOC_LOGD ("LocTech-Label :: QNP :: Position Request In");
    LOC_LOGD ("LocTech-Value :: Action (Single Shot = 0, Start = 1, Stop = 2) : %d",
            request->action);
    LOC_LOGD ("LocTech-Value :: Interval In milliseconds %u", request->intervalInMsec);
    LOC_LOGD ("LocTech-Value :: Emergency Request %d", request->emergencyRequest);
    LOC_LOGD ("LocTech-Value :: Accuracy %d", request->accuracy);
    mIzatContext->mMsgTask->sendMsg(new (nothrow) RequestWifiLocationMsg(this, request));
    return 1;
}

void WiFiLocationProvider::startSession(bool forceToUpdateStatus) {
    //Request GTP WiFi fixes
    sendRequest();
    //Start Request timer, save current timestamp
    mRequestTimer.start(GTP_WIFI_PROVIDER_REQUEST_TIME_OUT_IN_MS, false);

    GetTimeSinceBoot(mTimestampLastTrackingRequestInNs);

    //Send NLP started data item when
    //1, requestFixes is called first time in a session;
    //2, TBF is larger than 300s and requestFixes is called when TBFTimer timeout
    if (forceToUpdateStatus ||
            mConsolidatedReq.intervalInMsec >= TIME_THRESHOLD_TO_RE_NOTIFY_NLP_STATUS_MS) {
        SystemStatus *sysStat = SystemStatus::getInstance(mIzatContext->mMsgTask);
        if (sysStat) {
            sysStat->eventNlpSessionStatus(true);
        } else {
            LOC_LOGe("SystemStatus is nullptr");
        }
    }
}

//Stop session when:
//  1, called by uppper layer;
//  2, in resetSession, if req is singleshot
void WiFiLocationProvider::stopSession(bool forceToUpdateStatus) {
    //reset several local variables
    //reset timer
    mRequestTimer.stop();
    mTBFTimer.stop();
    //Send NLP stop data item when
    //1, stopTrackingSession is called from upper layer;
    //2, TBF is larger than 300s and stopFixes is called when position report comes
    //3, stopSession is called and current session is single shot
    if (forceToUpdateStatus ||
            mConsolidatedReq.intervalInMsec >= TIME_THRESHOLD_TO_RE_NOTIFY_NLP_STATUS_MS ||
            mConsolidatedReq.action == LocationRequestAction_SingleShot) {
        SystemStatus *sysStat = SystemStatus::getInstance(mIzatContext->mMsgTask);
        if (sysStat) {
            sysStat->eventNlpSessionStatus(false);
        } else {
            LOC_LOGe("SystemStatus is nullptr");
        }
    }
}

//resetSession will be called when
//  1, GTP-WiFi report position;
//  2, GTP-WiFi report error;
//  3, Request Timer timeout.
void WiFiLocationProvider::resetSession() {
    //Stop Request timer and TBF timer first
    stopSession();
    //Stop session if req is singleshot or session already stopped
    if (mConsolidatedReq.action == LocationRequestAction_SingleShot ||
            mConsolidatedReq.action == LocationRequestAction_Stop) {
        mConsolidatedReq.action = LocationRequestAction_Stop;
        mConsolidatedReq.intervalInMsec = 0;
        mConsolidatedReq.emergencyRequest = false;
        mConsolidatedReq.accuracy = LocationRequestAccuracy_Nominal;
        return;
    }
    //check timestamp and compare with TBF
    uint32_t timeToNextFixInMsec = 0;
    int64 nowInNanoSec = 0;
    GetTimeSinceBoot(nowInNanoSec);

    if (mTimestampLastTrackingRequestInNs <= 0 ||
            mTimestampLastTrackingRequestInNs > nowInNanoSec) {
        timeToNextFixInMsec = 0;
    } else {
        //If Current timetamp - request timestamp >= TBF, request fix immdiately
        //Else start TBFTimer with (TBF - deltaTime)
        uint32_t deltaTime = (uint32_t)
            ((nowInNanoSec - mTimestampLastTrackingRequestInNs) / 1000000);

        if (deltaTime >= mConsolidatedReq.intervalInMsec) {
            timeToNextFixInMsec = 0;
        } else {
            timeToNextFixInMsec = mConsolidatedReq.intervalInMsec - deltaTime;
        }
    }
    if (timeToNextFixInMsec == 0) {
        mIzatContext->mMsgTask->sendMsg(new (nothrow) TBFTimerMsg(this));
    } else {
        mTBFTimer.start(timeToNextFixInMsec, false);
    }
}

int WiFiLocationProvider::sendRequest() {
    int result = -1;
    OutPostcard* card = NULL;
    ENTRY_LOG();
    do {
        card = OutPostcard::createInstance();
        BREAK_IF_ZERO(1, card);
        BREAK_IF_NON_ZERO(2, card->init());
        BREAK_IF_NON_ZERO(3, card->addString("TO", "TERRESTRIAL-LOC-SESS-MGR"));
        BREAK_IF_NON_ZERO(4, card->addString("FROM", mProviderTag.c_str()));
        BREAK_IF_NON_ZERO(5, card->addString("REQ", "POSITION"));
        BREAK_IF_NON_ZERO(6, card->addInt32("TX-ID", ++mTxId));
        BREAK_IF_NON_ZERO(7, card->addBool("EMERGENCY-REQUEST",
                mConsolidatedReq.emergencyRequest));
        BREAK_IF_NON_ZERO(8, card->addBool("HIGH-ACCURACY-REQUEST",
                    mConsolidatedReq.accuracy == LocationRequestAccuracy_High));
        BREAK_IF_NON_ZERO(9, card->finalize());
        BREAK_IF_NON_ZERO(10, mIzatContext->mIPCMessagingProxyObj->sendMsg(
                    card, "TERRESTRIAL-LOC-SESS-MGR"));
        LOC_LOGD("fireGTPFixSession when gtp wifi SSR: Fire a position request to "
                "WiFi-SESS-MGR %d", mTxId);
        result = 0;
    } while (0);

    if (card != NULL) {
        delete card;
        card = NULL;
    }
    EXIT_LOG_WITH_ERROR("%d", 0);
    return result;
}

void WiFiLocationProvider::consolidateRequest(const LocationRequest& req) {
    if ((mConsolidatedReq.action == LocationRequestAction_Stop) &&
            (req.action == LocationRequestAction_SingleShot ||
            req.action == LocationRequestAction_Start)) {
        //Session start
        mConsolidatedReq.action = req.action;
        mConsolidatedReq.intervalInMsec = req.intervalInMsec;
        mConsolidatedReq.emergencyRequest = req.emergencyRequest;
        mConsolidatedReq.accuracy = req.accuracy;
        startSession(true);
    } else if (mConsolidatedReq.action == LocationRequestAction_SingleShot) {
        //single shot session can switch to start or stop
        //If new req is start, update parameters
        //If new req is stop, no need to call stop session here
        mConsolidatedReq.action = req.action;
        mConsolidatedReq.intervalInMsec = req.intervalInMsec;
        mConsolidatedReq.emergencyRequest = req.emergencyRequest;
        mConsolidatedReq.accuracy = req.accuracy;
    } else if (mConsolidatedReq.action == LocationRequestAction_Start) {
        switch (req.action) {
            case LocationRequestAction_Start:
            {
                //Compare and Consolidation
                if (req.intervalInMsec < mConsolidatedReq.intervalInMsec) {
                    mConsolidatedReq.intervalInMsec = req.intervalInMsec;
                }
                if (req.emergencyRequest > mConsolidatedReq.emergencyRequest) {
                    mConsolidatedReq.emergencyRequest = req.emergencyRequest;
                }
                if (req.accuracy > mConsolidatedReq.accuracy) {
                    mConsolidatedReq.accuracy = req.accuracy;
                }
                break;
            }
            case LocationRequestAction_Stop:
            {
                //Session stop
                mConsolidatedReq.action = LocationRequestAction_Stop;
                mConsolidatedReq.intervalInMsec = 0;
                mConsolidatedReq.emergencyRequest = false;
                mConsolidatedReq.accuracy = LocationRequestAccuracy_Nominal;
                stopSession(true);
                break;
            }
            case LocationRequestAction_SingleShot:
            {
                //send a request directly
                startSession();
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void WiFiLocationProvider::cacheFixAndReport(const WifiMeasurementReport& wifiRpt) {

    // UTC time
    WifiMeasurementReport wifiLocRpt = wifiRpt;
    struct timespec time_info_current = {};
    if (0 == clock_gettime(CLOCK_REALTIME, &time_info_current)) {
        wifiLocRpt.mUtcTimestampInMsec = (time_info_current.tv_sec)*1e3 +
                            (time_info_current.tv_nsec)/1e6;
        wifiLocRpt.mHasUtcTimestampInMsec = true;
    }

    // Elapsed real time in nanoseconds of when the fix was made
    if (0 == GetTimeSinceBoot(wifiLocRpt.mElapsedRealTimeInNanoSecs)) {
        wifiLocRpt.mHasElapsedRealTimeInNanoSecs = true;
    }

    //Consistency check
    bool checkResult = false;
    if (mZppLocReport.isValid() && !zppHasCPIContentOrWiFiContent()
        && isLocReportedWithinTime(mZppLocReport, 30)) {
        LOC_LOGD("ZPP is not CPI and is within 30s, so do consistency check");
        checkResult = consistencyCheckWifiFix(mZppLocReport, &wifiLocRpt);
    } else {
        //Not able to run consistency check, report fixes
        checkResult = true;
    }
    //Save GTP position report
    mWifiLocReport = wifiLocRpt;

    if (checkResult || mConsolidatedReq.emergencyRequest) {
        LOC_LOGD ("LocTech-Label :: QNP :: Position Report Out");
        if (mWifiLocReport.mHasPositionSource) {
            LOC_LOGD ("LocTech-Value :: Position source: %d", mWifiLocReport.mPositionSource);
        } else {
            LOC_LOGE("No position source in QNP position report");
        }
        // Automation -log - Please do not touch
        LOC_LOGD ("Reporting WiFi fix (%f, %f), unc %f time (ms) %" PRId64,
                mWifiLocReport.mLatitude, mWifiLocReport.mLongitude,
                mWifiLocReport.mHorizontalAccuracy,
                mWifiLocReport.mUtcTimestampInMsec);

        broadcastLocation(&mWifiLocReport, 1);
    }
}

bool WiFiLocationProvider::zppHasCPIContentOrWiFiContent()
{
    LOC_LOGd("Source technology mask = %d %d",
        mZppLocReport.mHasSourceTechMask, mZppLocReport.mSourceTechMask);

    if (mZppLocReport.mHasSourceTechMask &&
        ((mZppLocReport.mSourceTechMask & LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION) ||
         (mZppLocReport.mSourceTechMask & LOC_POS_TECH_MASK_WIFI))) {
        return true;
    }

    return false;
}

bool WiFiLocationProvider::isLocReportedWithinTime(LocationReport& locReport, int timeInSeconds) {
    ENTRY_LOG();
    bool result = false;

    struct timespec time_info_current = {};

    do {
        int64 currentTimeInMilliSec;
        BREAK_IF_ZERO(1, locReport.isValid());
        BREAK_IF_ZERO(2, locReport.mHasUtcTimestampInMsec);
        BREAK_IF_NON_ZERO(3, clock_gettime(CLOCK_REALTIME, &time_info_current));
        currentTimeInMilliSec = (time_info_current.tv_sec)*1e3 +
                           (time_info_current.tv_nsec)/1e6;

        int64 ageOfFixInMilliSec = currentTimeInMilliSec - locReport.mUtcTimestampInMsec;
        int ageOfFixInSec = ageOfFixInMilliSec / 1000;
        LOC_LOGD("Age of Location is %d seconds", ageOfFixInSec);
        if (ageOfFixInSec <= timeInSeconds)
            result = true;

    } while (0);

    EXIT_LOG_WITH_ERROR("%d", 0);
    return result;
}

double WiFiLocationProvider::getGreatCircleDistanceMeter(const LocationReport *loc1,
        const LocationReport *loc2) {
    const double CONVERT_TO_RADIANS  = 3.14 / 180;
    const double earth_radius_meter = 6367435.68;
    double lat1, lon1, lat2, lon2;

    // calculation taken from http://www.movable-type.co.uk/scripts/latlong.html
    lat1 = loc1->mLatitude * CONVERT_TO_RADIANS;
    lat2 = loc2->mLatitude * CONVERT_TO_RADIANS;
    lon1 = loc1->mLongitude * CONVERT_TO_RADIANS;
    lon2 = loc2->mLongitude * CONVERT_TO_RADIANS;

    double lat_diff = lat1 - lat2;
    double lon_diff = lon1 - lon2;
    double sin_half_lat_diff = sin(lat_diff / 2);
    double sin_half_lon_diff = sin(lon_diff / 2);
    double h = sin_half_lat_diff * sin_half_lat_diff
               + sin_half_lon_diff * sin_half_lon_diff * cos(lat1) * cos(lat2);
    double m = sqrt(h);
    double m1 = sqrt(1 - h);

    double c = 2 * atan2(m, m1);
    LOC_LOGD("lat1 = %f, lat2 =%f, long1 = %f, long2 = %f, lat_diff = %f "
                  "long_diff = %f, sin_half_lat_diff = %f "
                  "sin_half_long_diff = %f, h = %f, m = %f, m1 = %f, c = %f",
                  lat1, lat2, lon1, lon2, lat_diff, lon_diff, sin_half_lat_diff,
                  sin_half_lon_diff, h, m, m1, c);
    return earth_radius_meter * c;
}

void WiFiLocationProvider::increasePUNC(LocationReport & locReport)
{
    int result = -1;

    // Assuming an increase in punc by travelling at 56 miles / hour
    const double puncIncreaseMetersPerSecond = 25;

    // PUNC scaling from from 99% confidence to 68% confidence is (1.07/2.14) = 0.5
    const double ScaleFactorPuncConf = 0.5;
    struct timespec time_info_current = {};

    do {
        int64 currentTimeInMilliSec;
        BREAK_IF_ZERO(1, locReport.isValid());
        BREAK_IF_ZERO(2, locReport.mHasUtcTimestampInMsec);
        BREAK_IF_NON_ZERO(3, clock_gettime(CLOCK_REALTIME, &time_info_current));
        currentTimeInMilliSec = (time_info_current.tv_sec)*1e3 +
                           (time_info_current.tv_nsec)/1e6;

        int64 ageOfFixInMilliSec = currentTimeInMilliSec - locReport.mUtcTimestampInMsec;
        int ageOfFixInSec = ageOfFixInMilliSec / 1000;

        LOC_LOGD("currentTime = %" PRId64 ", UtcTimeInMsec = %" PRId64,
            currentTimeInMilliSec, locReport.mUtcTimestampInMsec);

        if (ageOfFixInSec > 0) {
            float puncIncreaseInMeters = ageOfFixInSec * puncIncreaseMetersPerSecond;
            LOC_LOGD("Increase punc by %f for %d secs", puncIncreaseInMeters, ageOfFixInSec);

            float accuracy_99 = puncIncreaseInMeters +
                (locReport.mHorizontalAccuracy / ScaleFactorPuncConf);
            float accuracy_68 = accuracy_99 * ScaleFactorPuncConf;

            LOC_LOGD("Previous punc (68%%) is %f, adjusted for punc growth to %f (68%%)",
                locReport.mHorizontalAccuracy, accuracy_68);

            locReport.mHorizontalAccuracy = accuracy_68;
            locReport.mUtcTimestampInMsec = currentTimeInMilliSec;
        }

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

float WiFiLocationProvider::getPUNCIncrease(const LocationReport & locReport)
{
    int result = -1;
    float puncIncreaseInMeters = 0;

    // Assuming an increase in punc by travelling at 56 miles / hour
    const double puncIncreaseMetersPerSecond = 25;
    struct timespec time_info_current = {};

    do {
        int64 currentTimeInMilliSec;
        BREAK_IF_ZERO(1, locReport.isValid());
        BREAK_IF_ZERO(2, locReport.mHasUtcTimestampInMsec);
        BREAK_IF_NON_ZERO(3, clock_gettime(CLOCK_REALTIME, &time_info_current));
        currentTimeInMilliSec = (time_info_current.tv_sec)*1e3 +
                           (time_info_current.tv_nsec)/1e6;

        int64 ageOfFixInMilliSec = currentTimeInMilliSec - locReport.mUtcTimestampInMsec;
        int ageOfFixInSec = ageOfFixInMilliSec / 1000;

        LOC_LOGD("currentTime = %" PRId64 " UtcTimeInMsec = %" PRId64,
            currentTimeInMilliSec, locReport.mUtcTimestampInMsec);

        if (ageOfFixInSec > 0) {
            puncIncreaseInMeters = ageOfFixInSec * puncIncreaseMetersPerSecond;
            LOC_LOGD("Increase punc by %f for %d secs", puncIncreaseInMeters, ageOfFixInSec);
        }

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return puncIncreaseInMeters;
}

bool WiFiLocationProvider::consistencyCheckWifiFix(LocationReport& locReportToCompareWith,
        WifiMeasurementReport* wifiLocReport) {
    ENTRY_LOG();
    if (wifiLocReport == nullptr) {
        return false;
    }
    bool result = true;

    // PUNC scaling from from 99% confidence to 68% confidence is (1.07/2.14) = 0.5
    const double ScaleFactorPuncConf = 0.5;

    // grow uncertainity circle of fix based on its accuracy and timestamp first
    increasePUNC(locReportToCompareWith);
    // grow the Zpp circle to accomodate the wifi fix
    float increasedPUNC = getPUNCIncrease(*wifiLocReport);
    float accuracy_99 = increasedPUNC +
        (locReportToCompareWith.mHorizontalAccuracy / ScaleFactorPuncConf);
    float accuracy_68 = accuracy_99 * ScaleFactorPuncConf;

    double distance = getGreatCircleDistanceMeter(wifiLocReport, &locReportToCompareWith);

    // Automation -log - Please do not touch
    LOC_LOGD("Distance between ZPP and XTWiFi fixes: %f m, "
               "while accuracy of ZPP fix is %f m (99%%), "
               "accuracy of ZPP fix is %f m (68%%)", distance,
               accuracy_99, accuracy_68);

    // If none of the below two conditions are met, wifi fix is no good
    // 1. Wifi fix falls into the uncertainity circle of wwan/zpp fix
    // 2. Distance of wifi fix frm zpp/wwan fix is lesser than mMaxZppWifiConsistencyCheckInMeters
    if ((distance >= accuracy_99) && (distance >= mMaxZppWifiConsistencyCheckInMeters)) {
        result = false;
    }

    if (wifiLocReport->mHorizontalAccuracy > mMaxAcuracyThresholdToFilterWiFiPosition) {
        LOC_LOGD("Dropping final fix from src = %d Accuracy = %f AccuracyThreshold = %d",
            wifiLocReport->mPositionSource, wifiLocReport->mHorizontalAccuracy,
            mMaxAcuracyThresholdToFilterWiFiPosition);
        result = false;
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
    return result;
}

void WiFiLocationProvider::reportProviderError(IZatLocationError::LocationErrorType errorType) {
    IZatLocationError locError;
    locError.mHasErrorType = true;
    locError.mLocationErrorType = errorType;
    string errorReport;
    locError.stringify(errorReport);
    LOC_LOGd ("LocTech-Value :: Error Report: %s", errorReport.c_str ());
}

void WiFiLocationProvider::getZppFixSync() {
    int result = -1;
    ENTRY_LOG();

    do {
        LocGpsLocation gpsLocation = {};
        LocPosTechMask tech_mask = 0;
        if (!mLBSAdapter) {
            LOC_LOGe("mLBSAdapter is nullptr");
            return;
        }
        mZppLocReport.reset();
        bool ret = mLBSAdapter->getZppFixSync(gpsLocation, tech_mask);
        if (!ret) {
            LOC_LOGe("get ZPP location fail!");
            break;
        }
        if ((gpsLocation.flags & LOC_GPS_LOCATION_HAS_LAT_LONG) &&
             (gpsLocation.flags & LOC_GPS_LOCATION_HAS_ACCURACY)) {

            LOC_LOGd("got Zpp location (%.7lf,%.7lf) at error %.3f",
             gpsLocation.latitude, gpsLocation.longitude, gpsLocation.accuracy);

            mZppLocReport.mHasLatitude = true;
            mZppLocReport.mLatitude = gpsLocation.latitude;
            mZppLocReport.mHasLongitude = true;
            mZppLocReport.mLongitude = gpsLocation.longitude;
            mZppLocReport.mHasHorizontalAccuracy = true;
            mZppLocReport.mHorizontalAccuracy = gpsLocation.accuracy;
            mZppLocReport.mHasPositionSource = true;
            mZppLocReport.mPositionSource = OTHER;
            mZppLocReport.mHasProcessorSource = true;
            mZppLocReport.mProcessorSource = LocationReport::ProcessorSrc_Modem;
            mZppLocReport.mHasUtcTimestampInMsec = true;
            mZppLocReport.mUtcTimestampInMsec = gpsLocation.timestamp;

            if (gpsLocation.flags & LOC_GPS_LOCATION_HAS_ALTITUDE) {
               mZppLocReport.mAltitudeWrtEllipsoid = gpsLocation.altitude;
               mZppLocReport.mHasAltitudeWrtEllipsoid = true;
            }

            // elapsed real time nano seconds of when the fix was made
            if (0 == GetTimeSinceBoot(mZppLocReport.mElapsedRealTimeInNanoSecs)) {
                mZppLocReport.mHasElapsedRealTimeInNanoSecs = true;
            }

            // LOC_POS_TECH_MASK_DEFAULT is returned if modem does not send a valid techmask
            if (tech_mask != 0) {
                mZppLocReport.mSourceTechMask = tech_mask;
                mZppLocReport.mHasSourceTechMask = true;
            }
        }
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);

}

void WiFiLocationProvider::RequestWifiLocationMsg::proc() const {
    if (mLocationRequest.action == LocationRequestAction_Realign_tbf_Immediate) {
        //send a request directly
        mWifiProvider->startSession();
    } else {
        mWifiProvider->consolidateRequest(mLocationRequest);
    }
}

void WiFiLocationProvider::RequestTimerMsg::proc() const {
    //If NLP request timeout, reset session
    mWifiProvider->resetSession();
    mWifiProvider->reportProviderError(IZatLocationError::LocationError_Timeout);
}

void WiFiLocationProvider::TBFTimerMsg::proc() const {
    //TBF timer timeout, send request to GTP WiFi
    mWifiProvider->startSession();
}

void WiFiLocationProvider::PositionMsgWifi::proc() const {
    int result = -1;
    ENTRY_LOG();
    WifiMeasurementReport rssiRpt;
    WifiMeasurementReport rttRpt;

    do {

        if (mWifiProvider->mConsolidatedReq.action == LocationRequestAction_Stop) {
            LOC_LOGD("Wrong state to receive a position in, ignore");
            result = 1;
            break;
        }

        BREAK_IF_ZERO(2, mIncard);

        int response_txid = 0;
        BREAK_IF_NON_ZERO(3, mIncard->getInt32("TX-ID", response_txid));

        InPostcard* rtt_card = 0;
        InPostcard* rssi_card = 0;
        double rttlat_deg = 0;
        double rttlon_deg = 0;
        float rttaccuracy = 0;
        const char* rttApSource = NULL;

        bool position_valid = false;
        // To parse RTT fix
        if ((0 == mIncard->getCard("RTT-POSITION", &rtt_card)) &&
           (0 == rtt_card->getDouble("LATITUDE_DEG", rttlat_deg)) &&
           (0 == rtt_card->getDouble("LONGITUDE_DEG", rttlon_deg)) &&
           (0 == rtt_card->getFloat("HOR_UNC_M", rttaccuracy)) &&
           (0 == rtt_card->getString("AP-LOCATION-SOURCE", &rttApSource))) {
            LOC_LOGD("got rtt final location (%.7lf,%.7lf) at error %.3f",
                    rttlat_deg, rttlon_deg, rttaccuracy);

            position_valid = true;
            rttRpt.mHasLatitude = true;
            rttRpt.mLatitude = rttlat_deg;
            rttRpt.mHasLongitude = true;
            rttRpt.mLongitude = rttlon_deg;
            rttRpt.mHasHorizontalAccuracy = true;
            rttRpt.mHorizontalAccuracy = rttaccuracy;
            rttRpt.mHasPositionSource = true;
            rttRpt.mPositionSource = WIFI;
            rttRpt.mHasProcessorSource = true;
            rttRpt.mProcessorSource = LocationReport::ProcessorSrc_AP;
            rttRpt.mHasPositionSourceProvider = true;
            rttRpt.mPositionSourceProvider = INTERNAL;
            rttRpt.mIsRttFix = true;
            rttRpt.mHasNetworkPositionSource = true;
            if (0 == strncmp(rttApSource, "LCI", 3)) {
                rttRpt.mNetworkPositionSource = IZAT_NETWORK_POSITION_FROM_WIFI_RTT_FTM;
            } else {
                rttRpt.mNetworkPositionSource = IZAT_NETWORK_POSITION_FROM_WIFI_RTT_SERVER;
            }

            int32_t altitudeRefType = 0;
            float altitude = 0.0;
            float altitudeAcc = 0.0;
            if ((0 == rtt_card->getInt32("ALTITUDE_REF_TYPE", altitudeRefType)) &&
                (0 == rtt_card->getFloat("ALTITUDE", altitude)) &&
                (0 == rtt_card->getFloat("ALTITUDE_ACCURACY", altitudeAcc))) {
                rttRpt.mHasAltitudeWrtEllipsoid = true;
                rttRpt.mAltitudeWrtEllipsoid = altitude;
                rttRpt.mHasVertUnc = true;
                rttRpt.mVertUnc = altitudeAcc;
                rttRpt.mHasAltitudeRefType = true;
                rttRpt.mAltitudeRefType = (IzatAltitudeRefType)altitudeRefType;
            }
        }
        // To parse RSSI fix
        if (0 == mIncard->getCard("RSSI-POSITION", &rssi_card)) {
            double lat_deg = 0;
            double lon_deg = 0;
            float accuracy = 0;

            if ((0 == rssi_card->getDouble("LATITUDE_DEG", lat_deg)) &&
                    (0 == rssi_card->getDouble("LONGITUDE_DEG", lon_deg)) &&
                    (0 == rssi_card->getFloat("HOR_UNC_M", accuracy))) {
                rssiRpt.mHasLatitude = true;
                rssiRpt.mLatitude = lat_deg;
                rssiRpt.mHasLongitude = true;
                rssiRpt.mLongitude = lon_deg;
                rssiRpt.mHasHorizontalAccuracy = true;
                rssiRpt.mHorizontalAccuracy = accuracy;
                rssiRpt.mHasPositionSource = true;
                rssiRpt.mPositionSource = WIFI;
                rssiRpt.mHasNetworkPositionSource = true;
                rssiRpt.mNetworkPositionSource = IZAT_NETWORK_POSITION_FROM_WIFI_RSSI;
                rssiRpt.mHasProcessorSource = true;
                rssiRpt.mProcessorSource = LocationReport::ProcessorSrc_AP;
                rssiRpt.mHasPositionSourceProvider = true;
                rssiRpt.mPositionSourceProvider = INTERNAL;
            }
            LOC_LOGD("Got rssi final indicator");
            if (rssiRpt.isValid()) {
                LOC_LOGD("got rssi final location (%.7lf,%.7lf) at error %.3f",
                    lat_deg, lon_deg, accuracy);
            }
        }
        delete rtt_card;
        rtt_card = nullptr;
        delete rssi_card;
        rssi_card = nullptr;
        result = 0;
    } while (0);
    bool reportRSSI = rssiRpt.isValid();
    bool reportRTT = rttRpt.isValid();
    if (reportRSSI || reportRTT) {
        //Get ZPP fix
        mWifiProvider->getZppFixSync();
        //Report position
        mWifiProvider->cacheFixAndReport(reportRTT ? rttRpt : rssiRpt);
    } else {
        //Report error
        mWifiProvider->reportProviderError(IZatLocationError::LocationError_NoPosition);
    }
    mWifiProvider->resetSession();
    EXIT_LOG_WITH_ERROR("%d", result);
}

WiFiLocationProvider::PositionMsgWifi::~PositionMsgWifi() {
        if (mIncard) { delete mIncard; mIncard = NULL;}
}

void WiFiLocationProvider::HandleGtpWifiSSRMsg::proc() const {
    //GTP-WiFi SSR, send NLP request if in session
    if (mWifiProvider->mConsolidatedReq.action != LocationRequestAction_Stop) {
        mWifiProvider->startSession();
    }
}

}
