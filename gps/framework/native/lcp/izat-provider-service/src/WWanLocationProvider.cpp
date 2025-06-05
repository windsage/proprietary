/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_WWanLocationProvider"

#include "WWanLocationProvider.h"
#include "Utils.h"
#include "IzatDefines.h"
#include "loc_cfg.h"
#include <log_util.h>
#include "SystemStatus.h"
#include <thread>

namespace izat_manager
{
using namespace qc_loc_fw;
using namespace loc_core;

ILocationProvider* WWanLocationProvider::mWWanLocationProvider = NULL;

#define BREAK_AND_DELETE_IF_NON_ZERO(ERR, X, OBJ) \
if (0!=(X)) {result = (ERR); delete (OBJ); break;}
// NLP session started data item time threshold value
#define TIME_THRESHOLD_TO_RE_NOTIFY_NLP_STATUS_MS 300*1000

ILocationProvider* WWanLocationProvider::getInstance(const struct s_IzatContext* izatContext)
{
    int result = -1;
    do {
        // QNP object is already created
        BREAK_IF_NON_ZERO(0, mWWanLocationProvider);
        BREAK_IF_ZERO(1, izatContext);

        mWWanLocationProvider = new (nothrow) WWanLocationProvider(izatContext);
        BREAK_IF_ZERO(4, mWWanLocationProvider);
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return mWWanLocationProvider;
}

void WWanLocationProvider::readConfig() {
    ENTRY_LOG();

    static loc_param_s_type izat_conf_param_table[] = {
       {"WWAN_LOCATION_PROVIDER_CACHE_POSITION_REPORT_THRESHOLD",
           &mCachePositionReportThresholdInSec, NULL, 'n'}
    };

    UTIL_READ_CONF(LOC_PATH_IZAT_CONF, izat_conf_param_table);
    LOC_LOGD("WWAN cache position report threshold set in izat.conf: %d",
            mCachePositionReportThresholdInSec);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

WWanLocationProvider::WWanLocationProvider(const struct s_IzatContext* izatContext) :
        LocationProvider(izatContext), mProviderTag("WWAN-LOCATION-PROVIDER"),
        mTxId(0), mTimestampLastTrackingRequestInNs(0), mTimestampLastPositionReportInNs(0),
        mCachePositionReportThresholdInSec(TIME_THRESHOLD_TO_RE_NOTIFY_NLP_STATUS_SEC),
        mTBFTimer([this](){
            this->mIzatContext->mMsgTask->sendMsg(
                    new (nothrow) TBFTimerMsg(this));
        }),
        mRequestTimer([this](){
            this->mIzatContext->mMsgTask->sendMsg(
                    new (nothrow) RequestTimerMsg(this));
        }) {
    ENTRY_LOG();

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

        readConfig();
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

// IMQServerProxyResponse Overrides
void WWanLocationProvider::handleMsg(InPostcard * const in_card) {
    int result = -1;
    ENTRY_LOG();
    do {
        BREAK_IF_ZERO(2, in_card);

        const char* resp = NULL;
        const char* req =NULL;
        const char* from = NULL;
        const char* info = NULL;
        int response_txid;

        BREAK_IF_NON_ZERO(3, in_card->getString("FROM", &from));

        //Handle xtwifi SSR case
        if (in_card->getString("INFO", &info) == 0) {
            if (strcmp(info, "WWAN-INIT") == 0) {
                if (0 == strcmp(from, "TERRESTRIAL-LOC-SESS-MGR")) {
                    bool status = false;
                    in_card->getBool("STATUS", status);
                    if (status) {
                        mIzatContext->mMsgTask
                                ->sendMsg(new (nothrow) HandleGtpWWanSSRMsg(this));
                    }
                }
            }
        }

        BREAK_IF_NON_ZERO(4, in_card->getInt32("TX-ID", response_txid));

        // Automation -log, Please do not touch

        if (0 == in_card->getString("RESP", &resp) && 0 == strcmp(resp, "POSITION")) {
            LOC_LOGD("FROM: %s RESP: %s TX-ID: %d", from, resp, response_txid);
            mIzatContext->mMsgTask->sendMsg(new (nothrow) PositionMsgWWan(this, in_card));
        } else if (0 == in_card->getString("REQ", &req)) {
            LOC_LOGD("FROM: %s REQ: %s TX-ID: %d", from, req, response_txid);
            if (0 == strncmp("WWAN-CONN", req, sizeof("WWAN-CONN"))) {
                mIzatContext->mMsgTask->sendMsg(new (nothrow) DataConnectionMsgWWan(this, in_card));
            } else {
                result = 7;
                break;
            }
        } else {
            result = 8;
            break;
        }

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return;
}

int WWanLocationProvider::setRequest(const LocationRequest * request) {
    LOC_LOGD ("LocTech-Label :: QNP :: Position Request In");
    LOC_LOGD ("LocTech-Value :: Action (Single Shot = 0, Start = 1, Stop = 2) : %d",
            request->action);
    LOC_LOGD ("LocTech-Value :: Interval In milliseconds %u", request->intervalInMsec);
    LOC_LOGD ("LocTech-Value :: Emergency Request %d", request->emergencyRequest);
    LOC_LOGD ("LocTech-Value :: Accuracy %d", request->accuracy);
    mIzatContext->mMsgTask->sendMsg(new (nothrow) RequestWWanLocationMsg(this, request));
    return 1;
}

void WWanLocationProvider::notifyNetworkStatus(const IzatNetworkStatus& status) {
    ENTRY_LOG();
    bool conn_status = false;
    string resp_type;
    if (status.status == IZAT_DATA_CONN_OPEN) {
        conn_status = true;
        resp_type = "BRING-UP";
        LOC_LOGd("data connection open!");
    } else if (status.status == IZAT_DATA_CONN_FAILED || status.status == IZAT_DATA_CONN_CLOSE) {
        conn_status = false;
        resp_type = "TEARDOWN";
        LOC_LOGd("data connection close!");
    } else {
        LOC_LOGe("notifyNetworkStatus EXIT: Status = %d", status.status);
        return;
    }
    if (!resp_type.empty()) {
        sendWWanConnResp(conn_status, resp_type.c_str());
    }
    return;
}

void WWanLocationProvider::startSession(bool forceToUpdateStatus) {
    //Request WWAN fixes
    sendWWanPostioningReq();
    //Start Request timer, save current timestamp
    mRequestTimer.start(WWAN_PROVIDER_REQUEST_TIME_OUT_IN_MS, false);

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
void WWanLocationProvider::stopSession(bool forceToUpdateStatus) {
    //reset several local variables
    //reset timer
    mRequestTimer.stop();
    mTBFTimer.stop();
    //Send NLP stop data item when
    //1, stopTrackingSession is called from upper layer;
    //2, TBF is larger than 300s and stopFixes is called when position report comes
    //3, stopSession is called and current session is single shot
    if (forceToUpdateStatus ||
            mConsolidatedReq.intervalInMsec >= TIME_THRESHOLD_TO_RE_NOTIFY_NLP_STATUS_MS
            || mConsolidatedReq.action == LocationRequestAction_SingleShot) {
        SystemStatus *sysStat = SystemStatus::getInstance(mIzatContext->mMsgTask);
        if (sysStat) {
            sysStat->eventNlpSessionStatus(false);
        } else {
            LOC_LOGe("SystemStatus is nullptr");
        }
    }
}

//resetSession will be called when
//  1, GTP-WWan report position;
//  2, GTP-WWan report error;
//  3, Request Timer timeout.
void WWanLocationProvider::resetSession() {
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

int WWanLocationProvider::sendWWanPostioningReq() {
    int result = -1;
    OutPostcard* card = NULL;
    ENTRY_LOG();

    //Check if cache fix is within the certain time threshold
    int64 nowInNanoSec = 0;
    GetTimeSinceBoot(nowInNanoSec);
    if ((mTimestampLastPositionReportInNs > 0) &&
            ((nowInNanoSec - mTimestampLastPositionReportInNs) <
            ((int64)mCachePositionReportThresholdInSec * 1000000000))) {
        if (mWWanLocReport.isValid()) {
            broadcastLocation(&mWWanLocReport, 1);
        }
        return 0;
    }
    do {
        OutPostcard* card = OutPostcard::createInstance();
        BREAK_IF_ZERO(1, card);
        BREAK_IF_NON_ZERO(2, card->init());
        BREAK_IF_NON_ZERO(3, card->addString("TO", "TERRESTRIAL-LOC-SESS-MGR"));
        BREAK_IF_NON_ZERO(4, card->addString("FROM", mProviderTag.c_str()));
        BREAK_IF_NON_ZERO(5, card->addString("REQ", "POSITION"));
        BREAK_IF_NON_ZERO(6, card->addInt32("TX-ID", ++mTxId));
        BREAK_IF_NON_ZERO(7, card->addBool("EMERGENCY-REQUEST",
                mConsolidatedReq.emergencyRequest));
        BREAK_IF_NON_ZERO(8, card->addBool("PREMIUM",
                    mConsolidatedReq.accuracy == LocationRequestAccuracy_High));
        BREAK_IF_NON_ZERO(9, card->addString("TECH", "WWAN"));
        BREAK_IF_NON_ZERO(10, card->addInt32("APP-UID", mConsolidatedReq.uid));
        BREAK_IF_NON_ZERO(10, card->finalize());
        BREAK_IF_NON_ZERO(11, mIzatContext->mIPCMessagingProxyObj->sendMsg(
                    card, "TERRESTRIAL-LOC-SESS-MGR"));
        LOC_LOGD("fireGTPFixSession: Fire a position request to "
                "WWAN-SESS-MGR %d, uid %d", mTxId, mConsolidatedReq.uid);
        result = 0;
    } while (0);

    if (card != NULL) {
        delete card;
        card = NULL;
    }
    EXIT_LOG_WITH_ERROR("%d", 0);
    return result;
}

void WWanLocationProvider::consolidateRequest(const LocationRequest& req) {
    if ((mConsolidatedReq.action == LocationRequestAction_Stop) &&
            (req.action == LocationRequestAction_SingleShot ||
            req.action == LocationRequestAction_Start)) {
        //Session start
        mConsolidatedReq.action = req.action;
        mConsolidatedReq.intervalInMsec = req.intervalInMsec;
        mConsolidatedReq.emergencyRequest = req.emergencyRequest;
        mConsolidatedReq.accuracy = req.accuracy;
        mConsolidatedReq.uid = req.uid;
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
                mConsolidatedReq.uid = req.uid;
                break;
            }
            case LocationRequestAction_Stop:
            {
                //Session stop
                mConsolidatedReq.action = LocationRequestAction_Stop;
                mConsolidatedReq.intervalInMsec = 0;
                mConsolidatedReq.emergencyRequest = false;
                mConsolidatedReq.accuracy = LocationRequestAccuracy_Nominal;
                mConsolidatedReq.uid = 0;
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

void WWanLocationProvider::reportProviderError(IZatLocationError::LocationErrorType errorType) {
    IZatLocationError locError;
    locError.mHasErrorType = true;
    locError.mLocationErrorType = errorType;
    string errorReport;
    locError.stringify(errorReport);
    LOC_LOGd ("LocTech-Value :: Error Report: %s", errorReport.c_str ());
}

void WWanLocationProvider::RequestWWanLocationMsg::proc() const {
    ENTRY_LOG();
    if (mLocationRequest.action == LocationRequestAction_Realign_tbf_Immediate) {
        //send a request directly
        mProvider->startSession();
    } else {
        mProvider->consolidateRequest(mLocationRequest);
    }
}

void WWanLocationProvider::RequestTimerMsg::proc() const {
    //If NLP request timeout, reset session
    mProvider->resetSession();
    mProvider->reportProviderError(IZatLocationError::LocationError_Timeout);
}

void WWanLocationProvider::TBFTimerMsg::proc() const {
    //TBF timer timeout, send request
    mProvider->startSession();
}

int WWanLocationProvider::sendWWanConnResp(bool status, const char* resp_type) {
    int result = -1;
    OutPostcard* card = NULL;
    ENTRY_LOG();
    do {
        OutPostcard* card = OutPostcard::createInstance();
        BREAK_IF_ZERO(1, card);
        BREAK_IF_NON_ZERO(2, card->init());
        BREAK_IF_NON_ZERO(3, card->addString("TO", "TERRESTRIAL-LOC-SESS-MGR"));
        BREAK_IF_NON_ZERO(4, card->addString("FROM", mProviderTag.c_str()));
        BREAK_IF_NON_ZERO(5, card->addString("RESP", "WWAN-CONN"));
        BREAK_IF_NON_ZERO(6, card->addInt32("TX-ID", mTxId));
        BREAK_IF_NON_ZERO(7, card->addBool("STATUS", status));
        BREAK_IF_NON_ZERO(8, card->addString("RESP-TYPE", resp_type));
        BREAK_IF_NON_ZERO(9, card->addString("TECH", "WWAN"));
        BREAK_IF_NON_ZERO(9, card->finalize());
        BREAK_IF_NON_ZERO(10, mIzatContext->mIPCMessagingProxyObj->sendMsg(
                    card, "TERRESTRIAL-LOC-SESS-MGR"));
        LOC_LOGD("Fire a data connection response to "
                "WWAN-SESS-MGR %d", mTxId);
        result = 0;
    } while (0);

    if (card != NULL) {
        delete card;
        card = NULL;
    }
    EXIT_LOG_WITH_ERROR("%d", 0);
    return result;
}

void WWanLocationProvider::DataConnectionMsgWWan::proc() const {
    ENTRY_LOG();
    int result = -1;
    const char* req_type = 0;
    do {
        BREAK_IF_ZERO(1, mIncard);
        BREAK_IF_NON_ZERO(2, mIncard->getString("REQ-TYPE", &req_type));
        if (0 == strncmp("BRINGUP", req_type, sizeof("BRINGUP"))) {
            mProvider->mDataConnectionReqFunc(true);
        } else if (0 == strncmp("TEARDOWN", req_type, sizeof("TEARDOWN"))) {
            mProvider->mDataConnectionReqFunc(false);
        } else {
            LOC_LOGe("ERROR: invalid connection request type: %s", req_type);
            break;
        }
        result = 0;
    } while (0);

    if (result != 0) {
        LOC_LOGd("ConnectionWWan handle failed, result = %d", result);
    }
    EXIT_LOG_WITH_ERROR("%d", 0);
    return;
}

void WWanLocationProvider::PositionMsgWWan::proc() const {
    ENTRY_LOG();
    int result = -1;
    InPostcard *wwan_fix_card = 0;
    LocationReport locReport = {};
    float altElpM = 0.0;
    long long timestamp = 0;
    do {
        BREAK_IF_ZERO(1, mIncard);
        wwan_fix_card = InPostcard::createInstance();
        BREAK_IF_NON_ZERO(2, mIncard->getCard("WWAN-POSITION", &wwan_fix_card));
        BREAK_IF_NON_ZERO(3, wwan_fix_card->getInt64("TIMESTAMP", timestamp));
        BREAK_IF_NON_ZERO(4, wwan_fix_card->getDouble("LATITUDE_DEG", locReport.mLatitude));
        BREAK_IF_NON_ZERO(5, wwan_fix_card->getDouble("LONGITUDE_DEG", locReport.mLongitude));
        BREAK_IF_NON_ZERO(6, wwan_fix_card->getFloat("HOR_UNC_M", locReport.mHorizontalAccuracy));
        BREAK_IF_NON_ZERO(7, wwan_fix_card->getFloat("ALT_ELP_M", altElpM));
        BREAK_IF_NON_ZERO(8, wwan_fix_card->getFloat("VER_UNC_M", locReport.mVertUnc));

        if (0 == wwan_fix_card->getInt64("TIMESTAMP", timestamp)) {
            locReport.mHasUtcTimestampInMsec = true;
            locReport.mUtcTimestampInMsec = static_cast<long>(timestamp);
        }
        if (0 == wwan_fix_card->getDouble("LATITUDE_DEG", locReport.mLatitude)) {
            locReport.mHasLatitude = true;
        }
        if (0 == wwan_fix_card->getDouble("LONGITUDE_DEG", locReport.mLongitude)) {
            locReport.mHasLongitude = true;
        }
        if (0 == wwan_fix_card->getFloat("HOR_UNC_M", locReport.mHorizontalAccuracy)) {
            locReport.mHasHorizontalAccuracy = true;
        }
        if (0 == wwan_fix_card->getFloat("ALT_ELP_M", altElpM)) {
            locReport.mHasAltitudeWrtEllipsoid = true;
            locReport.mAltitudeWrtEllipsoid = static_cast<double>(altElpM);
        }
        if (0 == wwan_fix_card->getFloat("VER_UNC_M", locReport.mVertUnc)) {
            locReport.mHasVertUnc = true;
        }
        locReport.mHasPositionSource = true;
        locReport.mPositionSource = ENH_CELLID;
        locReport.mHasNetworkPositionSource = true;
        locReport.mNetworkPositionSource = IZAT_NETWORK_POSITION_FROM_CELL;
        locReport.mHasProcessorSource = true;
        locReport.mProcessorSource = LocationReport::ProcessorSrc_AP;
        locReport.mHasPositionSourceProvider = true;
        locReport.mPositionSourceProvider = INTERNAL;

        mProvider->mWWanLocReport = locReport;
        mProvider->broadcastLocation(&locReport, 1);

        result = 0;
    } while (0);

    GetTimeSinceBoot(mProvider->mTimestampLastPositionReportInNs);
    if (wwan_fix_card) {
        delete wwan_fix_card;
        wwan_fix_card = 0;
    }

    //Comment out for now since currently wwan positioning only support single shot
    //This line shall be retrieved when wwan positioning app tracking is supported
    //mProvider->resetSession();
    if (result != 0) {
        LOC_LOGd("PositionMsgWWan handle failed, result = %d", result);
    }
    EXIT_LOG_WITH_ERROR("%d", 0);

    return;
}

WWanLocationProvider::PositionMsgWWan::~PositionMsgWWan() {
    if (mIncard) { delete mIncard; mIncard = NULL;}
}
WWanLocationProvider::DataConnectionMsgWWan::~DataConnectionMsgWWan() {
    if (mIncard) { delete mIncard; mIncard = NULL;}
}

void WWanLocationProvider::HandleGtpWWanSSRMsg::proc() const {
    //Release data connection if connected
    mProvider->mDataConnectionReqFunc(false);
    //SSR, send NLP request if in session
    if (mProvider->mConsolidatedReq.action != LocationRequestAction_Stop) {
        mProvider->startSession();
    }
}

}
