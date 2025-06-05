/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  FreeWifiScanObserver Implementation

  Copyright (c) 2015 - 2016, 2022 - 2023 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_FreeWifiScanObserver"

#include <list>
#include <new>
#include <algorithm>
#include <lowi_request.h>
#include <lowi_response.h>
#include <lowi_client.h>
#include <IFreeWifiScanSubscription.h>
#include <mq_client/IPCMessagingProxy.h>
#include <IFreeWifiScanObserver.h>
#include <FreeWifiScanObserver.h>
#include <log_util.h>
#include <IzatDefines.h>
#include <mq_client/IPCMessagingProxy.h>
#include <postcard.h>
#include <DataItemId.h>

using namespace std;
using namespace qc_loc_fw;
using namespace izat_manager;

//TODO: Below LOWI parameters are tentative
const int MEAS_FILTER_PERIOD = 10; //10 seconds
const int FALLBACK_TOLERANCE_PERIOD = 3; //3 seconds

FreeWifiScanObserver * FreeWifiScanObserver :: mInstance = NULL;

FreeWifiScanObserver* FreeWifiScanObserver :: getInstance
(IIPCMessagingRequest * ipcIface, MsgTask * msgTask) {

    if (!mInstance) {
        mInstance = new (nothrow) FreeWifiScanObserver (ipcIface, msgTask);
        if (mInstance) {
            int result = mInstance->mIpcIface->registerResponseObj
                             ( mInstance->mAddress.c_str (), mInstance);
            if (result != 0) {
                return NULL;
            }
        }
    }
    return mInstance;
}

void FreeWifiScanObserver :: destroyInstance () {
    delete mInstance;
    mInstance = NULL;
}

void FreeWifiScanObserver :: Timer:: timeOutCallback() {
    static int timercnt = 0;

    timercnt++;
    LOC_LOGd("timeout timer callback fired %d times", timercnt);
    mClient->mMsgTask->sendMsg (new (nothrow) ReqExpiryMsg(mClient));
}

void FreeWifiScanObserver :: OsSettingRegTimer:: timeOutCallback() {
    static int timercnt = 0;

    timercnt++;
    LOC_LOGd("os setting timer callback fired %d times", timercnt);
    if (timercnt < 10) {
        mClient->mMsgTask->sendMsg (new (nothrow) OsSettingRegExpiryMsg(mClient));
    }
}

// ctors
inline FreeWifiScanObserver :: FreeWifiScanObserver
(IIPCMessagingRequest * ipcIface, MsgTask * msgTask)
:
mIpcIface (ipcIface),
mMsgTask (msgTask),
mAddress ("FreeWifiScanObserver"),
mRegisteredForFreeWifiScan(false),
mOsRegisterd(false),
#ifdef __ANDROID__
mLocationEnabled(false),
#else
mLocationEnabled(true),
#endif
mRequestId(0),
mTimer(this),
mOsRegTimer(this)
{}

inline FreeWifiScanObserver :: LocMsgBase :: LocMsgBase
(FreeWifiScanObserver * parent)
: mParent (parent) {}


inline FreeWifiScanObserver :: InitiateWifiScanLocMsg :: InitiateWifiScanLocMsg
(
FreeWifiScanObserver * parent,
unsigned long int measurementAgeFilterInSecs,
unsigned long int fallbackToleranceSec,
long long int timeoutTimestamp,
bool isEmergencyRequest
)
:
LocMsgBase (parent),
mMeasurementAgeFilterInSecs (measurementAgeFilterInSecs),
mFallbackToleranceSec (fallbackToleranceSec),
mTimeoutTimestamp (timeoutTimestamp),
mIsEmergencyRequest(isEmergencyRequest)
{}


inline FreeWifiScanObserver :: SubscribeLocMsg :: SubscribeLocMsg
(FreeWifiScanObserver * parent,
 const IFreeWifiScanObserver * observer)
: LocMsgBase (parent), mObserver (observer) {}

inline FreeWifiScanObserver :: UnsubscribeLocMsg :: UnsubscribeLocMsg
(FreeWifiScanObserver * parent,
 const IFreeWifiScanObserver * observer)
: LocMsgBase (parent), mObserver (observer) {}

inline FreeWifiScanObserver :: NotifyLocMsg :: NotifyLocMsg
(FreeWifiScanObserver * parent,
 const LOWIResponse * response)
: LocMsgBase (parent), mResponse (response) {}

inline FreeWifiScanObserver :: ReqExpiryMsg :: ReqExpiryMsg
(FreeWifiScanObserver * parent)
: LocMsgBase (parent)
{}

inline FreeWifiScanObserver :: OsSettingRegExpiryMsg :: OsSettingRegExpiryMsg
(FreeWifiScanObserver * parent)
: LocMsgBase (parent)
{}

inline FreeWifiScanObserver :: LocSettingChangeMsg :: LocSettingChangeMsg
(FreeWifiScanObserver * parent,
 bool locSettingEnabled)
: LocMsgBase (parent), mLocSettingEnabled (locSettingEnabled) {}

inline FreeWifiScanObserver :: LocMsgBase :: ~LocMsgBase () {}
inline FreeWifiScanObserver :: InitiateWifiScanLocMsg :: ~InitiateWifiScanLocMsg
() {}
inline FreeWifiScanObserver :: SubscribeLocMsg :: ~SubscribeLocMsg () {}
inline FreeWifiScanObserver :: UnsubscribeLocMsg :: ~UnsubscribeLocMsg () {}
FreeWifiScanObserver :: NotifyLocMsg :: ~NotifyLocMsg () {
    if (mResponse) {
        delete mResponse;
        mResponse = NULL;
    }
}

inline FreeWifiScanObserver :: ReqExpiryMsg :: ~ReqExpiryMsg () {}
inline FreeWifiScanObserver :: OsSettingRegExpiryMsg :: ~OsSettingRegExpiryMsg () {}
inline FreeWifiScanObserver :: LocSettingChangeMsg :: ~LocSettingChangeMsg () {}

// Helpers
void FreeWifiScanObserver :: sendFreeWifiScanRequest
(
    unsigned long int validTime
)
{
    int result = 0;
    OutPostcard * out = NULL;
    ENTRY_LOG ();
    do {
        mTimer.stop();
        mTimer.start(validTime * 1000, false);

        LOC_LOGi ("sendFreeWifiScanRequest requst id %lu, valid time %lu, client %s",
                  mRequestId, validTime, mAddress.c_str());
        LOWIAsyncDiscoveryScanResultRequest request (mRequestId, validTime);
        out = LOWIClient :: composePostCard (&request, mAddress.c_str());
        BREAK_IF_ZERO (1, out);
        BREAK_IF_NON_ZERO (2, mIpcIface->sendMsg (out, qc_loc_fw::SERVER_NAME));
        delete out;
        out = nullptr;

        LOWISnoopRTTRegisterRequest snoopRequest(mRequestId, validTime,
                LOWISnoopRTTRegisterRequest::SNOOP_RTT_REGISTER_CLIENT);
        out = LOWIClient::composePostCard(&snoopRequest, mAddress.c_str());
        BREAK_IF_ZERO(3, out);
        BREAK_IF_NON_ZERO(4, mIpcIface->sendMsg(out, qc_loc_fw::SERVER_NAME));

        mRequestId++;
    } while (0);
    if (out) {
        delete out;
        out = nullptr;
    }

    LOC_LOGD ("Free Wifi-scan request validity period (secs) : %lu", validTime);

    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver :: refreshFreeWifiScanRequest () {
    ENTRY_LOG ();
    sendFreeWifiScanRequest ();
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: cancelFreeWifiScanRequest () {
    ENTRY_LOG ();
    sendFreeWifiScanRequest (0);
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

// proc
/*
  We trigger a Fresh Scan Request for Emergency mode request from modem
  Cache fallback request otwerwise
 */
void FreeWifiScanObserver :: InitiateWifiScanLocMsg :: proc () const {

    int result = 0;
    LOWIDiscoveryScanRequest * request = 0;
    OutPostcard * out = 0;
    ENTRY_LOG ();
    do {
        if(mIsEmergencyRequest) {
            LOC_LOGV ("Send emergency on demand scan request");
            request =
                LOWIDiscoveryScanRequest::createFreshScanRequest
                (
                    this->mParent->mRequestId,
                    LOWIDiscoveryScanRequest::BAND_ALL,
                    LOWIDiscoveryScanRequest::ACTIVE_SCAN,
                    MEAS_FILTER_PERIOD,
                    mTimeoutTimestamp,
                    LOWIDiscoveryScanRequest::FORCED_FRESH
                    );
        } else {
            LOC_LOGV ("Send on demand scan request");
            request =
                LOWIDiscoveryScanRequest::createCacheFallbackRequest
                (
                    this->mParent->mRequestId,
                    LOWIDiscoveryScanRequest::TWO_POINT_FOUR_GHZ,
                    LOWIDiscoveryScanRequest::ACTIVE_SCAN,
                    MEAS_FILTER_PERIOD,
                    FALLBACK_TOLERANCE_PERIOD,
                    mTimeoutTimestamp,
                    false
                    );
        }

        BREAK_IF_ZERO (1, request);

        out = LOWIClient :: composePostCard (request,
                                             this->mParent->mAddress.c_str());

        BREAK_IF_ZERO (2, out);

        BREAK_IF_NON_ZERO (3, this->mParent->mIpcIface->sendMsg
                           (out,
                            qc_loc_fw::SERVER_NAME));

        this->mParent->mRequestId++;

    } while (0);

    if (out) {
        delete out;
    }

    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver :: SubscribeLocMsg :: proc () const {
    ENTRY_LOG ();

    LOC_LOGi("subscribe, before client cnt %zu, mRegisteredForFreeWifiScan %d",
             this->mParent->mClients.size(), this->mParent->mRegisteredForFreeWifiScan);
    if ( this->mParent->mClients.end () ==
         find (this->mParent->mClients.begin (),
               this->mParent->mClients.end (),
               this->mObserver) ) {

        this->mParent->mClients.push_back (mObserver);

        if (!this->mParent->mRegisteredForFreeWifiScan) {
            this->mParent->sendFreeWifiScanRequest (60);
            this->mParent->mRegisteredForFreeWifiScan = true;
        }

        LOC_LOGd("os registered %d", this->mParent->mOsRegisterd);
        if (this->mParent->mOsRegisterd == false) {
            this->mParent->registerOSAgentUpdate();
        }
        else {
            LOC_LOGd("notify client of location setting %d", this->mParent->mLocationEnabled);
            (const_cast <IFreeWifiScanObserver *>(this->mObserver))->
                        notifyLocSettingChange(this->mParent->mLocationEnabled);
        }
    }

    LOC_LOGd("subscribe, after client cnt %zu", this->mParent->mClients.size());
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: UnsubscribeLocMsg :: proc () const {

    ENTRY_LOG ();
    LOC_LOGi("unsubscribe, client cnt %zu", this->mParent->mClients.size());
    list <const IFreeWifiScanObserver *> :: iterator it =
        find (this->mParent->mClients.begin (),
              this->mParent->mClients.end (),
              this->mObserver);

    if (it != this->mParent->mClients.end ()) {
        this->mParent->mClients.erase (it);
        LOC_LOGi("unsubscribe find the client, size %zu", this->mParent->mClients.size());
        if ( this->mParent->mClients.empty () &&
             this->mParent->mRegisteredForFreeWifiScan ) {
            this->mParent->cancelFreeWifiScanRequest ();
            this->mParent->mRegisteredForFreeWifiScan = false;
        }
    }
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: NotifyLocMsg :: proc () const {
    int result = 0;
    ENTRY_LOG ();

    do {
        BREAK_IF_ZERO (1, mResponse);
        list <const IFreeWifiScanObserver *> :: iterator it =
            this->mParent->mClients.begin ();
        for ( ; it != this->mParent->mClients.end (); ++it) {
           (const_cast <IFreeWifiScanObserver *>(*it))->notify (mResponse);
        }
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver :: ReqExpiryMsg :: proc () const {
    ENTRY_LOG ();

    LOC_LOGd("timer expiry msg client cnt %zu", this->mParent->mClients.size ());

    if ( this->mParent->mClients.size() != 0) {
        this->mParent->refreshFreeWifiScanRequest ();
    }
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: OsSettingRegExpiryMsg :: proc () const {
    ENTRY_LOG ();

    LOC_LOGd("os setting registration timer exiry msg ");

    this->mParent->registerOSAgentUpdate ();
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: LocSettingChangeMsg :: proc () const {
    int result = 0;
    ENTRY_LOG ();

    LOC_LOGd("LocSettingChangeMsg os reg done %d, old setting %d, new setting %d, client cnt %zu",
             this->mParent->mOsRegisterd, this->mParent->mLocationEnabled,  mLocSettingEnabled,
             this->mParent->mClients.size());

    this->mParent->mOsRegisterd = true;
    this->mParent->mLocationEnabled = mLocSettingEnabled;

    do {
        list <const IFreeWifiScanObserver *> :: iterator it =
            this->mParent->mClients.begin ();
        for ( ; it != this->mParent->mClients.end (); ++it) {
            LOC_LOGd("LocSettingChangeMsg, notify client of setting %d", mLocSettingEnabled);
           (const_cast <IFreeWifiScanObserver *>(*it))->notifyLocSettingChange(mLocSettingEnabled);
        }
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver::ReqWiFiCapabilitiesMsg::proc() const {
    ENTRY_LOG ();
    int result = 0;
    LOWICapabilityRequest * request = 0;
    OutPostcard * out = 0;
    do {
        request = new (std::nothrow) LOWICapabilityRequest(this->mParent->mRequestId);
        BREAK_IF_ZERO(1, request);
        out = LOWIClient::composePostCard(request, this->mParent->mAddress.c_str());
        BREAK_IF_ZERO (2, out);

        BREAK_IF_NON_ZERO (3, this->mParent->mIpcIface->sendMsg(out, qc_loc_fw::SERVER_NAME));
        this->mParent->mRequestId++;
    } while (0);

    if (out) {
        delete out;
    }

    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver :: initiateWifiScan
(
unsigned long int measurementAgeFilterInSecs,
unsigned long int fallbackToleranceSec,
long long int timeoutTimestamp,
bool isEmergencyRequest
) {
    ENTRY_LOG ();
    mMsgTask->sendMsg (new (nothrow) InitiateWifiScanLocMsg
                                         (this,
                                          measurementAgeFilterInSecs,
                                          fallbackToleranceSec,
                                          timeoutTimestamp,
                                          isEmergencyRequest) );
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: subscribe
(
    const IFreeWifiScanObserver * observer
)
{
    int result = 0;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, observer);
        mMsgTask->sendMsg (new (nothrow) SubscribeLocMsg (this, observer));
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver :: unsubscribe
(
    const IFreeWifiScanObserver * observer
)
{
    int result = 0;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, observer);
        mMsgTask->sendMsg (new (nothrow) UnsubscribeLocMsg (this, observer));
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

int FreeWifiScanObserver::registerOSAgentUpdate ()
{
  OutPostcard * card = NULL;
  int result = -1;

  LOC_LOGd("registerOSAgentUpdate, os registered %d, location setting %d",
           mOsRegisterd, mLocationEnabled);
  do
  {
    BREAK_IF_NON_ZERO(0, mOsRegisterd == true);

    // start timer in 5 seconds
    mOsRegTimer.start(5 * 1000, false);
    card = OutPostcard::createInstance();
    BREAK_IF_ZERO(-2, card);
    BREAK_IF_NON_ZERO(-3, card->init());
    int32_t osstatus_subscribe[1] =
    {
        GPSSTATE_DATA_ITEM_ID
    };

    BREAK_IF_NON_ZERO(-4, card->addArrayInt32("DATA-ITEMS",
                                              sizeof(osstatus_subscribe) / sizeof(int32_t),
                                              osstatus_subscribe));
    BREAK_IF_NON_ZERO(-5, card->addString("TO", "OS-Agent"));
    BREAK_IF_NON_ZERO(-6, card->addString("FROM", mAddress.c_str()));
    BREAK_IF_NON_ZERO(-7, card->addString("REQ", "SUBSCRIBE"));
    BREAK_IF_NON_ZERO(-8, card->finalize());
    LOC_LOGd("%s send post card to os agent", mAddress.c_str());
    BREAK_IF_NON_ZERO(10, mIpcIface->sendMsg(card, qc_loc_fw::SERVER_NAME));

    result = 0;
  } while (0);

  delete card;
  card = NULL;

  if (0 != result)
  {
    LOC_LOGe("registerOSAgentUpdate failed: %d", result);
  }
  return result;
}

void FreeWifiScanObserver :: handleMsg (InPostcard * const inCard) {
    int result = 0;
    ENTRY_LOG ();
    do {
        const char* info = NULL;
        const char* resp = NULL;

        if (inCard->getString("INFO", &info) == 0){
            LOC_LOGd ("info %s", info);
            if (strcmp(info, "REGISTER-EVENT") == 0) {
                const char *client_name = NULL;
                if (inCard->getString("CLIENT", &client_name) == 0) {
                   LOC_LOGi ("REGISTER EVENT for client %s", client_name);
                   if (0 == strcmp(client_name, "LOWI-SERVER")) {
                       mMsgTask->sendMsg (new (nothrow) ReqExpiryMsg (this));
                   }
               }
            } else if (0 == strcmp(info, "OS-STATUS-UPDATE")) {
                InPostcard* locEnabledCard = 0;
                bool location_enabled = false;
                LOC_LOGd ("OS-STATUS-UPDATE received");
                if ((inCard->getCard("GPS_STATE", &locEnabledCard) == 0) &&
                    (locEnabledCard->getBool("IS_GPS_PROVIDER_ENABLED", location_enabled)) == 0) {
                    LOC_LOGd ("OS-STATUS-UPDATE received with setting of %d", location_enabled);
                    mMsgTask->sendMsg (new (nothrow) LocSettingChangeMsg (this, location_enabled));
               }
            }
        } else if (inCard->getString("RESP", &resp) == 0) {
             if (strcmp(resp, "LOWI_ASYNC_DISCOVERY_SCAN_RESULTS") == 0) {
                LOWIAsyncDiscoveryScanResultResponse * response =
                  static_cast <LOWIAsyncDiscoveryScanResultResponse *>
                  (
                      LOWIClient :: parsePostCard (inCard)
                  );
                BREAK_IF_ZERO (1, response);

                LOC_LOGv ("LOWI ASYNC DISCOVERY response received with status:%d",
                          response->scanStatus);

                if (LOWIResponse::SCAN_STATUS_INVALID_REQ == response->scanStatus)
                {
                    LOC_LOGi ("ASYNC DISCOVERY response received with SCAN_STATUS_INVALID_REQ");
                    mMsgTask->sendMsg (new (nothrow) ReqExpiryMsg (this));
                } else if (LOWIResponse::SCAN_STATUS_SUCCESS == response->scanStatus) {
                    mMsgTask->sendMsg (new (nothrow) NotifyLocMsg (this, response));
                }
             }

             if (strcmp(resp, "LOWI_RANGING_SCAN") == 0) {
                 LOC_LOGd("LOWI_RANGING_SCAN response received");

                 LOWIRangingScanResponse* response =
                 static_cast <LOWIRangingScanResponse *>
                 (
                    LOWIClient::parsePostCard(inCard)
                 );
                 BREAK_IF_ZERO(1, response);
                 LOC_LOGd("LOWI_RANGING_SCAN response received with status:%d",
                          response->scanStatus);

                 if (1 == response->scanStatus) { // SUCCESS
                     int num_ap = (int)(response->scanMeasurements.getNumOfElements());
                     LOC_LOGv("num_ap = %d", num_ap);
                     BREAK_IF_ZERO(0, num_ap);

                     for (int i = 0; i < num_ap; ++i)
                     {
                         LOWIScanMeasurement* const ap = response->scanMeasurements[i];
                         BREAK_IF_ZERO(101, ap);

                         int num_of_meas = ap->measurementsInfo.getNumOfElements();
                         if (num_of_meas <= 0) {
                             LOC_LOGv("skipping ap that doesn't have ranging measurement");
                             continue;
                         }

                         LOC_LOGv("ap[%d].bssid = 0x%" PRIx64 " ", i,
                                (uint64_t)ap->bssid.getFull48());
                         LOC_LOGv("ap[%d].targetStatus = %d", i, ap->targetStatus);
                         LOC_LOGv("ap[%d].num_frames_attempted = %d", i, ap->num_frames_attempted);
                         LOC_LOGv("ap[%d].success_cnt = %d", i,
                                  ap->measurementsInfo.getNumOfElements());

                         for (int j = 0; j < ap->measurementsInfo.getNumOfElements(); j++) {
                             LOWIMeasurementInfo * info = ap->measurementsInfo[j];
                             LOC_LOGv("ap[%d].meas[%d].distanceMm = %d", i, j,
                                      uint32((float)info->rtt_ps * RTT_DIST_CONST_PS_MM));
                             LOC_LOGv("ap[%d].meas[%d].rssiDbm = %d", i, j,
                                      info->rssi / 2);
                             LOC_LOGv("ap[%d].meas[%d].txBandwidth = %d", i, j,
                                      info->tx_bw);
                             LOC_LOGv("ap[%d].meas[%d].rxBandwidth = %d", i, j,
                                      info->rx_bw);
                             LOC_LOGv("ap[%d].meas[%d].rx_chain_no = %d", i, j,
                                      info->rx_chain_no);
                         }
                     }
                 }
                 mMsgTask->sendMsg(new (nothrow) NotifyLocMsg(this, response));
             }

             if (strcmp(resp, "LOWI_DISCOVERY_SCAN") == 0) {
                LOWIDiscoveryScanResponse * response =
                  static_cast <LOWIDiscoveryScanResponse *>
                  (
                      LOWIClient :: parsePostCard (inCard)
                  );
                BREAK_IF_ZERO (2, response);
                LOC_LOGv ("LOWI DISCOVERY response received with status:%d",
                          response->scanStatus);
                mMsgTask->sendMsg (new (nothrow) NotifyLocMsg (this, response));
            }

             if (strcmp(resp, "LOWI_CAPABILITY") == 0) {
                LOWICapabilityResponse * response =
                  static_cast <LOWICapabilityResponse *>
                  (
                      LOWIClient :: parsePostCard (inCard)
                  );
                BREAK_IF_ZERO (3, response);
                LOC_LOGD ("LOWI Capability response received with status:%d",
                          response->getStatus());
                mMsgTask->sendMsg (new (nothrow) NotifyLocMsg (this, response));
            }
        }
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver::queryWiFiHwCapabilities() {
    mMsgTask->sendMsg (new (nothrow) ReqWiFiCapabilitiesMsg (this));
}
