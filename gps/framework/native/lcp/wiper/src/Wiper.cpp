/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Wiper module assists in various features like GNSS, Geofence, SUPL-WIFI
  Passive inject of Coarse Position
  Trigger of wifi scan requests (SUPL WIFI)
  Inject of Wifi supplicant State

  Copyright  (c) 2015-2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_Wiper"

#include <string>
#include "Wiper.h"
#include "Utils.h"
#include "loc_cfg.h"
#include "IzatDefines.h"
#include "DataItemId.h"
#include "DataItemsFactoryProxy.h"
#include "IzatManager.h"
#include "IOSFramework.h"
#include "LocContext.h"
#include <list>
#include <gps_extended_c.h>

namespace izat_manager
{

using namespace std;
using namespace izat_manager;
using namespace qc_loc_fw;
using namespace lbs_core;

#define LOWIResponseDefines qc_loc_fw::LOWIResponse

Wiper * Wiper :: mInstance = NULL;

Wiper* Wiper :: getInstance
    (const struct s_IzatContext * izatContext) {

    unsigned int wiperFlag = 0;
    WiperConfigData wiperConfData;
    std::string confFile(LOC_PATH_IZAT_CONF);

    char conf_feature_free_wifi_scan_inject[LOC_MAX_PARAM_STRING] = {0};
    char conf_feature_supl_wifi[LOC_MAX_PARAM_STRING] = {0};
    char conf_feature_wifi_supplicant_info[LOC_MAX_PARAM_STRING] = {0};

    memset(&wiperConfData, 0, sizeof(wiperConfData));
    static loc_param_s_type izat_conf_param_table[] = {
        {"FREE_WIFI_SCAN_INJECT",    &conf_feature_free_wifi_scan_inject,   NULL, 's'},
        {"SUPL_WIFI",                &conf_feature_supl_wifi,               NULL, 's'},
        {"WIFI_SUPPLICANT_INFO",     &conf_feature_wifi_supplicant_info,    NULL, 's'},
        {"WIFI_WAIT_TIMEOUT_SELECT", &wiperConfData.mWifiWaitTimeoutSelect, NULL, 'n'},
    };

    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_NON_ZERO(0, mInstance);
        BREAK_IF_ZERO(1, izatContext);

        // Read the default configurations from the izat.conf first
        UTIL_READ_CONF(confFile.c_str(), izat_conf_param_table);

        // Set service mask for FREE_WIFI_SCAN_INJECT
        if(strcmp(conf_feature_free_wifi_scan_inject, "BASIC") == 0) {
            LOC_LOGD("Setting FREE_WIFI_SCAN_INJECT to mode: BASIC");
            wiperFlag |= FEATURE_FREE_WIFI_SCAN_INJECT_MASK;
        }
        else if(strcmp(conf_feature_free_wifi_scan_inject, "DISABLED") == 0) {
            LOC_LOGD("Setting FREE_WIFI_SCAN_INJECT to mode: DISABLED");
        }
        else if(strcmp(conf_feature_free_wifi_scan_inject, "PREMIUM") == 0) {
            LOC_LOGD("Unrecognized value for FREE_WIFI_SCAN_INJECT mode."\
                "Setting FREE_WIFI_SCAN_INJECT to default mode: BASIC");
            wiperFlag |= FEATURE_FREE_WIFI_SCAN_INJECT_MASK;
        }

        // Set service mask for SUPL_WIFI
        if(strcmp(conf_feature_supl_wifi, "BASIC") == 0) {
            LOC_LOGD("Setting SUPL_WIFI to mode: BASIC");
            wiperFlag |= FEATURE_SUPL_WIFI_MASK;
        }
        else if(strcmp(conf_feature_supl_wifi, "DISABLED") == 0) {
            LOC_LOGD("Setting SUPL_WIFI to mode: DISABLED");
        }
        else if(strcmp(conf_feature_supl_wifi, "PREMIUM") == 0) {
            LOC_LOGD("Unrecognized value for SUPL_WIFI mode."\
                "Setting SUPL_WIFI to default mode: BASIC");
            wiperFlag |= FEATURE_SUPL_WIFI_MASK;
        }

        // Set service mask for WIFI_SUPPLICANT_INFO
        if(strcmp(conf_feature_wifi_supplicant_info, "BASIC") == 0) {
            LOC_LOGD("Setting WIFI_SUPPLICANT_INFO to mode: BASIC");
            wiperFlag |= FEATURE_WIFI_SUPPLICANT_INFO_MASK;
        }
        else if(strcmp(conf_feature_wifi_supplicant_info, "DISABLED") == 0) {
            LOC_LOGD("Setting WIFI_SUPPLICANT_INFO to mode: DISABLED");
        }
        else if(strcmp(conf_feature_wifi_supplicant_info, "PREMIUM") == 0) {
            LOC_LOGD("Unrecognized value for WIFI_SUPPLICANT_INFO mode."\
                "Setting IZAT_FEATURE_MASK_WIFI_SUPPLICANT_INFO to default mode: BASIC");
            wiperFlag |= FEATURE_WIFI_SUPPLICANT_INFO_MASK;
        }

        mInstance = new (nothrow) Wiper (izatContext, wiperFlag, wiperConfData);
        BREAK_IF_ZERO(3, mInstance);

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mInstance;
}

void Wiper :: destroyInstance() {
    FreeWifiScanObserver::destroyInstance();

    delete mInstance;
    mInstance = NULL;
}

// ctors
Wiper :: Wiper
(const struct s_IzatContext * izatContext,
    unsigned int wiperFlag, WiperConfigData &wiperConfData)
:
mIzatContext(izatContext),
mLbsAdapter(NULL),
mWifiScanObserver(NULL),
mWiperFlag (wiperFlag),
mWiperConfigData(wiperConfData),
mIsWifiScanInSession(false), mWifiScanRequestedUTCTimesInMs(0),
mLatestSupplicantState(WifiSupplicantStatusDataItem::DISCONNECTED),
mSupportWIFIInject2V2(false),
mLowiStatusValid(false),
mCapabilityQueryCnt(0) {
    int result = -1;

    ENTRY_LOG ();

    do {
        memset(&mWifiScanSet, 0, sizeof(WifiScanSet));

        setFeatureRegistrationBits();

        // Do wiper related subscriptions after LBSAdapter is created/initialized
         mIzatContext->mMsgTask->sendMsg(
            new (nothrow) wiperSubscribeInit(this, wiperFlag));
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

// dtors
Wiper :: ~Wiper ()
{
}

void Wiper :: setFeatureRegistrationBits()
{
    int result = -1;
    LOC_API_ADAPTER_EVENT_MASK_T mask = LOC_API_ADAPTER_BIT_POSITION_INJECTION_REQUEST;
    // Note:: CPI is enabled by default. IzatManager will report back passive network locations.

    ENTRY_LOG ();

    do {
        // Enable mask to get wifi ap scan request from modem
        mask |= LOC_API_ADAPTER_BIT_REQUEST_WIFI_AP_DATA;
        // Enable mask for get timezone request from modem, for timezone injection from HLOS.
        mask |= LOC_API_ADAPTER_BIT_REQUEST_TIMEZONE;

        mLbsAdapter = LBSAdapter::get(mask);
        BREAK_IF_ZERO(1, mLbsAdapter);

        LOC_LOGD("WiperConfigData WifiWaitTimeout: %d(s)\n",
                mWiperConfigData.mWifiWaitTimeoutSelect);

        mLbsAdapter->setWifiWaitTimeoutValue(mWiperConfigData.mWifiWaitTimeoutSelect);

        // check what version of feature is supported on QMI LOC
        mSupportWIFIInject2V2 = mLbsAdapter->isFeatureSupported(
            LOC_SUPPORTED_FEATURE_WIFI_AP_DATA_INJECT_2_V02);
        LOC_LOGD("Support WifiInject2V2:%d", mSupportWIFIInject2V2);
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void Wiper :: stringify (WifiSupplicantInfo & wsi, string & valueStr) {

    valueStr += "Attach state: ";
    char t[50];
    memset (t, '\0', 50);
    snprintf (t, 50, "%d", wsi.attachState);
    valueStr += t;

    valueStr += ", Mac address valid: ";
    memset (t, '\0', 50);
    snprintf (t, 50, "%c", wsi.apMacAddressValid);
    valueStr += t;

    valueStr += ", AP MAC address: ";
    memset (t, '\0', 50);
    snprintf
    (
        t,
        50,
        "[%02x:%02x:%02x:%02x:%02x:%02x]",
        wsi.apMacAddress[0],
        wsi.apMacAddress[1],
        wsi.apMacAddress[2],
        wsi.apMacAddress[3],
        wsi.apMacAddress[4],
        wsi.apMacAddress[5]
    );
    valueStr += t;

    valueStr += ", Wifi-Ap SSID Valid: ";
    memset (t, '\0', 50);
    snprintf (t, 50, "%02x", wsi.wifiApSsidValid);
    valueStr += t;

    valueStr += ", SSID: ";
    memset (t, '\0', 50);
    snprintf (t, 50, "%s", wsi.ssid);
    valueStr += t;
}

void Wiper :: stringify (WifiApInfo & wai, string & valueStr) {
    valueStr += "AP Num: ";
    char t[50];
    memset (t, '\0', 50);
    snprintf (t, 50, "%d", wai.apLen);
    valueStr += t;

    int macBoundaryIndex = 0;

    valueStr += ", onDemandScan:";
    memset (t, '\0', 50);
    snprintf (t, 50, "%d,", wai.onDemandScan);
    valueStr += t;

    valueStr += ", Mac Addresses [";
    for (int i = 0; i < wai.apLen; i++) {
            memset (t, '\0', 50);
            snprintf
            (
                t,
                50,
                "%02x:%02x:%02x:%02x:%02x:%02x, ",
                wai.mac_address[macBoundaryIndex + 0], wai.mac_address[macBoundaryIndex + 1],
                wai.mac_address[macBoundaryIndex + 2], wai.mac_address[macBoundaryIndex + 3],
                wai.mac_address[macBoundaryIndex + 4], wai.mac_address[macBoundaryIndex + 5]
            );
            valueStr += t;
            macBoundaryIndex += MAC_ADDRESS_LENGTH;
   }

    valueStr += "]";

    valueStr += ", RSSI [";

    for (int i=0; i<wai.apLen; i++) {
        memset (t, '\0', 50);
        snprintf (t, 50, "%d,", wai.rssi[i]);
        valueStr += t;
    }

    valueStr += "]";

    valueStr += ", RSSI - HighRes [";

    for (int i=0; i<wai.apLen; i++) {
        memset (t, '\0', 50);
        snprintf (t, 50, "%d,", wai.apHighResolutionRssi[i]);
        valueStr += t;
    }

    valueStr += "]";

    valueStr += ", RSSI Timestamp [";

    for (int i=0; i<wai.apLen; i++) {
        memset (t, '\0', 50);
        snprintf (t, 50, "%" PRIu64, wai.rssi_timestamp[i]);
        valueStr += t;
    }

    valueStr += "]";

    valueStr += ", Channel/Frequency [";

    for (int i=0; i<wai.apLen; i++) {
        memset (t, '\0', 50);
        snprintf (t, 50, "%d,", wai.channel[i]);
        valueStr += t;
    }

    valueStr += "]";

    valueStr += ", Measurement Age [";

    for (int i=0; i<wai.apLen; i++) {
        memset (t, '\0', 50);
        snprintf (t, 50, "%d,", wai.measurement_age[i]);
        valueStr += t;
    }

    valueStr += "]";

    valueStr += ", Serving Access Point [";

    for (int i=0; i<wai.apLen; i++) {
        memset (t, '\0', 50);
        snprintf (t, 50, "%d,", wai.servingAccessPoint[i]);
        valueStr += t;
    }

    valueStr += "]";

    if (wai.wifiApSsidValid) {
        valueStr += ", SSID[";
        for (uint32_t i=0; i<wai.wifiApSsidInfoLen; i++) {
            Wiper :: stringify (wai.wifiApSsidInfo[i], valueStr);
            valueStr += ", ";
        }
        valueStr += "]";
    }

    valueStr += "]";
}

void Wiper :: stringify (WifiApSsidInfo & ws, string & valueStr) {
    valueStr += ws.ssid;
}

// IFreeWifiScanObserver overrides
void Wiper::notify(const LOWIResponse * response)
{
    int result = -1;
    LOWIDiscoveryScanResponse *scanResponse = NULL;
    LOWICapabilityResponse *capabilityResponse = NULL;
    LOWIResponseDefines::eResponseType scanRespType = LOWIResponseDefines::RESPONSE_TYPE_UNKNOWN;

    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, response);
        LOC_LOGD ("LocTech-Label :: WIPER :: Wifi Scan Notification In");

        // First get the type of response
        scanRespType = const_cast<LOWIResponse *>(response)->getResponseType();

        LOC_LOGD ("LocTech-Label :: WIPER :: Wifi Scan Notification In,response to scan type:%d",
                  scanRespType);

        if ((LOWIResponseDefines::DISCOVERY_SCAN == scanRespType) ||
            (LOWIResponseDefines::ASYNC_DISCOVERY_SCAN_RESULTS == scanRespType)) {
            scanResponse = static_cast<LOWIDiscoveryScanResponse *>
                    (const_cast<LOWIResponse *>(response));
            // if scanResponse is NULL, there is nothing to do
            BREAK_IF_ZERO(2, scanResponse);
            LOC_LOGD ("LocTech-Label :: WIPER :: number of scan results returned:%d",
                scanResponse->scanMeasurements.getNumOfElements());

            if (LOWIResponseDefines::SCAN_STATUS_SUCCESS == scanResponse->scanStatus) {
                BREAK_IF_ZERO(3, scanResponse->scanMeasurements.getNumOfElements());
                mIzatContext->mMsgTask->sendMsg(new (nothrow) wifiScanNotificationMsg(this,
                    scanResponse->scanMeasurements,
                    (scanRespType ==
                            LOWIResponseDefines::DISCOVERY_SCAN) ? ON_DEMAND_SCAN : FREE_WIFI_SCAN,
                    scanResponse->self_mac));
            } else {
                WifiScanResponseType errType;
                if (LOWIResponseDefines::SCAN_STATUS_UNKNOWN == scanResponse->scanStatus) {
                    errType = WIFI_SCAN_RESPONSE_UNKNOWN;
                } else if (scanResponse->scanStatus == LOWIResponseDefines::SCAN_STATUS_NO_WIFI) {
                    errType = WIFI_SCAN_RESPONSE_NO_WIFI_AVAILABLE;
                } else {
                    errType = WIFI_SCAN_RESPONSE_NO_MEASUREMENTS_AVAILABLE;
                }
                mIzatContext->mMsgTask->sendMsg(new (nothrow) wifiScanNotificationErrorMsg(this,
                    errType,
                    (scanRespType == LOWIResponseDefines::DISCOVERY_SCAN) ?
                            ON_DEMAND_SCAN : FREE_WIFI_SCAN));
            }
        } else if (LOWIResponseDefines::CAPABILITY == scanRespType) {
            capabilityResponse = static_cast<LOWICapabilityResponse *>
                    (const_cast<LOWIResponse *>(response));
            // if scanResponse is NULL, there is nothing to do
            BREAK_IF_ZERO(4, capabilityResponse);
            mIzatContext->mMsgTask->sendMsg(
                    new (nothrow) wifiCapabilitiesNotificationMsg(this, capabilityResponse));
        }

        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", 0);
}

// empty impl
void Wiper::notifyLocSettingChange(bool locationEnabled) {
}

// IDataItemObserver overrides
void Wiper::getName (string & name) {
    name = "Wiper";
}

void Wiper::notify(const std::unordered_set<IDataItemCore *> & dlist)
{
    ENTRY_LOG();
    mIzatContext->mMsgTask->sendMsg(new (nothrow) handleOsObserverUpdateMsg(this, dlist));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void Wiper::handleWifiApDataRequest(const WifiApDataRequest &request)
{
    if (NULL == mInstance) {
        LOC_LOGE("mInstance is NULL");
    } else {
        Wiper::mInstance->mIzatContext->mMsgTask->sendMsg(
            new (nothrow) wifiScanRequestMsg(mInstance, request));
    }
}

void Wiper::handleSsrInform()
{
    if (NULL == mInstance) {
        LOC_LOGE("mInstance is NULL");
    } else {
        // Handle SSR for wiper module.
        Wiper::mInstance->mIzatContext->mMsgTask->sendMsg(
            new (nothrow) handleSsrMsg(mInstance));
    }
}

void Wiper::handleTimeZoneInfoRequest()
{
    if (NULL == mInstance) {
        LOC_LOGE("mInstance is NULL");
    } else {
        Wiper::mInstance->mIzatContext->mMsgTask->sendMsg(
            new (nothrow) handleTimeZoneInfoRequestMsg(mInstance));
    }
}

void Wiper::wifiScanRequestMsg::proc() const
{
    int result = -1;
    struct timespec time_info_current = {};
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(0, mWiperObj->mWifiScanObserver);
        BREAK_IF_NON_ZERO(1, mWiperObj->mIsWifiScanInSession);

        mWiperObj->mWifiScanObserver->initiateWifiScan(0,
                                                       0,
                                                       0,
                                                       mRequest.emergencyRequest);
        mWiperObj->mIsWifiScanInSession = true;

        // update the wifi scan requested timestamp.
        clock_gettime(CLOCK_REALTIME,&time_info_current);
        mWiperObj->mWifiScanRequestedUTCTimesInMs = (time_info_current.tv_sec)*1e3 +
                           (time_info_current.tv_nsec)/1e6;
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

Wiper::wifiScanNotificationMsg::wifiScanNotificationMsg(Wiper *wiperObj,
    qc_loc_fw::vector<LOWIScanMeasurement *> lowiScanMeasurements, WifiScanType scanType,
    LOWIMacAddress self_mac)
:   mWiperObj(wiperObj), mWifiScanType(scanType), mSelfMacAddr(self_mac)
{
    ENTRY_LOG();
    bool  associatedApFound = false;
    for (unsigned int i = 0; i < lowiScanMeasurements.getNumOfElements(); i++) {
        LOWIScanMeasurement* scanReceived = lowiScanMeasurements[i];
        if (nullptr == scanReceived) {
            continue;
        }

        if (scanReceived->associatedToAp) {
            associatedApFound = true;
        }

        if ((mLowiScanMeasurements.getNumOfElements() < MAX_REPORTED_APS) ||
                (scanReceived->associatedToAp)) {

            LOWIScanMeasurement* newScanMeasurement =
                    new (nothrow) LOWIScanMeasurement(*scanReceived);
            if (nullptr == newScanMeasurement) {
                break;
            }

            if (mLowiScanMeasurements.getNumOfElements() < MAX_REPORTED_APS) {
                mLowiScanMeasurements.push_back(newScanMeasurement);
            } else { // this can only happen if scanReceived->associatedToAp is true
                delete mLowiScanMeasurements[mLowiScanMeasurements.getNumOfElements()-1];
                mLowiScanMeasurements[mLowiScanMeasurements.getNumOfElements()-1] =
                        newScanMeasurement;
                break;
            }
        }
    }

    LOC_LOGd("Associated AP found %d, num of scan elements %d, saved meas cnt %d",
             associatedApFound, lowiScanMeasurements.getNumOfElements(),
             mLowiScanMeasurements.getNumOfElements());


    EXIT_LOG_WITH_ERROR("%d", 0);
}

Wiper::wifiScanNotificationMsg::~wifiScanNotificationMsg()
{
    for (unsigned int i = 0; i < mLowiScanMeasurements.getNumOfElements(); i++)
    {
        delete mLowiScanMeasurements[i];
    }
}

void Wiper::wifiScanNotificationMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        // clear the WifiScanSet structure
        memset(&mWiperObj->mWifiScanSet, 0, sizeof(WifiScanSet));

        LOC_LOGd("Num of elements returned by LOWI %d, Self Mac Address:" LOWI_MACADDR_FMT,
            mLowiScanMeasurements.getNumOfElements(), LOWI_MACADDR(mSelfMacAddr));

        // fill up the WifiApInfo and WifiApSsidInfo fields in the WifiScanSet structure
        mWiperObj->mWifiScanSet.apInfoValid = 1;
        mWiperObj->mWifiScanSet.apInfo.wifiScanRespType = WIFI_SCAN_RESPONSE_SUCCESS;

        // record the self mac addr
        for (int i =0; i < MAC_ADDRESS_LENGTH; i++) {
            mWiperObj->mWifiScanSet.apInfo.selfMacAddr[i] = mSelfMacAddr[i];
        }

        mWiperObj->mWifiScanSet.apInfo.onDemandScan = (mWifiScanType == ON_DEMAND_SCAN ? 1 : 0);
        mWiperObj->mWifiScanSet.apInfo.apLen = mLowiScanMeasurements.getNumOfElements();
        mWiperObj->mWifiScanSet.apInfo.wifiApSsidValid = 1;
        mWiperObj->mWifiScanSet.apInfo.wifiApSsidInfoLen =
            mLowiScanMeasurements.getNumOfElements();

        int macBoundaryIndex = 0;

        for (unsigned int iScanIndex = 0; iScanIndex < mLowiScanMeasurements.getNumOfElements();
                iScanIndex++ )
        {
            LOWIScanMeasurement scanMeasurement (*mLowiScanMeasurements[iScanIndex]);

            //record the MAC address
            for (int iMacLenIndex = 0; iMacLenIndex < MAC_ADDRESS_LENGTH; iMacLenIndex++) {
                mWiperObj->mWifiScanSet.apInfo.mac_address[macBoundaryIndex] =
                        scanMeasurement.bssid[iMacLenIndex];
                macBoundaryIndex++;
            }

            // record channel
            /* Note:The channel field being set earlier was actually the frequency,
                    frequency field was introduced for better readibility.
                    Channel numbers could actually be repeated for different
                    frequencies, but for backward compatibility we continue to
                    input frequency in channel filed as well*/
            mWiperObj->mWifiScanSet.apInfo.channel[iScanIndex] = scanMeasurement.frequency;
            mWiperObj->mWifiScanSet.apInfo.frequency[iScanIndex] = scanMeasurement.frequency;

            // Record whether its an associated AP,
            mWiperObj->mWifiScanSet.apInfo.servingAccessPoint[iScanIndex] =
                scanMeasurement.associatedToAp;

            // record the rssi

            // MeasurementInfo in LOWIScanMeasurement is a vector, but for
            // discovery scan there must be only one MeasurementInfo.
            unsigned int iMeasurementInfoLength =
                    scanMeasurement.measurementsInfo.getNumOfElements();
            if ( iMeasurementInfoLength > 1) {
                LOC_LOGW("MeasurementInfo must be 1 for a Discovery scan");
            }

            for (unsigned int measureIndex = 0; measureIndex < iMeasurementInfoLength;
                    measureIndex++) {
                LOWIMeasurementInfo measurementInfo
                        (*scanMeasurement.measurementsInfo[measureIndex]);
                //scale rssi to 1 dbm, lowi provides on the scale of 0.5 dbm
                mWiperObj->mWifiScanSet.apInfo.rssi[iScanIndex] = measurementInfo.rssi/2;

                // Since LOWI gives rssi at the scale of 0.5 dbm, apHighResolutionRssi accounts for
                // loss of rssi when scaling to 1dbm above.
                if (measurementInfo.rssi % 2) {
                    mWiperObj->mWifiScanSet.apInfo.apHighResolutionRssi[iScanIndex] = 5;
                }

                mWiperObj->mWifiScanSet.apInfo.rssi_timestamp[iScanIndex] =
                        measurementInfo.rssi_timestamp;
                mWiperObj->mWifiScanSet.apInfo.measurement_age[iScanIndex] =
                        measurementInfo.meas_age;
            }

            // record the ssid
            if (scanMeasurement.ssid.isSSIDValid()) {
                int ssidlen =
                        sizeof(mWiperObj->mWifiScanSet.apInfo.wifiApSsidInfo[iScanIndex].ssid);
                int errssid = scanMeasurement.ssid.getSSID((unsigned char*)mWiperObj->
                        mWifiScanSet.apInfo.wifiApSsidInfo[iScanIndex].ssid, &ssidlen);
                if (errssid != 0) {
                    LOC_LOGW("Failed to copy SSID from ScanMeasurementInfo, error %d", errssid);
                }
            }
        }

        // update the timestamps
        if (ON_DEMAND_SCAN == mWifiScanType) {
            struct timespec time_info_current = {};
            int64 wifiScanReceivedUTCTimeInMs = 0;
            clock_gettime(CLOCK_REALTIME,&time_info_current);
            wifiScanReceivedUTCTimeInMs = (time_info_current.tv_sec)*1e3 +
                    (time_info_current.tv_nsec)/1e6;

            mWiperObj->mWifiScanSet.apInfo.requestTimestamp =
                    mWiperObj->mWifiScanRequestedUTCTimesInMs;
            mWiperObj->mWifiScanSet.apInfo.receiveTimestamp = wifiScanReceivedUTCTimeInMs;
        }

        //Active wifi scan injection
        // Inject free wifi scan using QMI_LOC_INJECT_WIFI_AP_DATA_REQ_V02
        // only if mSupportWIFIInject2V2 is true
        if ( (mWiperObj->mSupportWIFIInject2V2) || (mWiperObj->mIsWifiScanInSession)) {
            LOC_LOGD ("LocTech-Label :: WIPER :: Wifi AP Info OUT");
            string wai;
            Wiper :: stringify (mWiperObj->mWifiScanSet.apInfo, wai);
            LOC_LOGV ("LocTech-Value :: Wifi AP Info: %s", wai.c_str());
            mWiperObj->mLbsAdapter->injectWifiApInfo(mWiperObj->mWifiScanSet.apInfo);


           if (mWiperObj->mWifiScanSet.apInfo.onDemandScan) {
               mWiperObj->mIsWifiScanInSession = false;
           }
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void Wiper::wifiScanNotificationErrorMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        memset(&mWiperObj->mWifiScanSet, 0, sizeof(WifiScanSet));

        // fill up the WifiApInfo
        mWiperObj->mWifiScanSet.apInfoValid = 1;
        mWiperObj->mWifiScanSet.apInfo.wifiScanRespType = mErrType;
        mWiperObj->mWifiScanSet.apInfo.onDemandScan = (mWifiScanType == ON_DEMAND_SCAN ? 1 : 0);

        if (mWiperObj->mWifiScanSet.apInfo.onDemandScan) {
            mWiperObj->mIsWifiScanInSession = false;
        }

        LOC_LOGD ("LocTech-Label :: WIPER :: Wifi Scan Error OUT");
        mWiperObj->mLbsAdapter->injectWifiApInfo(mWiperObj->mWifiScanSet.apInfo);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

Wiper::handleOsObserverUpdateMsg::handleOsObserverUpdateMsg(Wiper *wiperObj,
           const std::unordered_set<IDataItemCore *> & dataItemList) : mWiperObj(wiperObj)
{
    int result = -1;
    do {
        std::unordered_set<IDataItemCore *>::const_iterator it = dataItemList.begin();
        for (; it != dataItemList.end(); it++) {
            IDataItemCore *updatedDataItem = *it;

            IDataItemCore * dataitem =
                    DataItemsFactoryProxy::createNewDataItem(updatedDataItem);
            BREAK_IF_ZERO(2, dataitem);

            mDataItemList.push_back(dataitem);
        }
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void Wiper::handleOsObserverUpdateMsg::proc() const
{
    ENTRY_LOG();
    int result = -1;

    do {
        // the assumption here is that the first response to phone context request message
        // will contain all the settings requested for.

        std::list<IDataItemCore *>::const_iterator it = mDataItemList.begin();
        for (; it != mDataItemList.end(); it++) {
            IDataItemCore* dataItem = *it;
            switch (dataItem->getId())
            {
                case WIFIHARDWARESTATE_DATA_ITEM_ID:
                {
                    WifiHardwareStateDataItem *wifiState =
                            static_cast<WifiHardwareStateDataItem*>(dataItem);
                    mWiperObj->mLbsAdapter->wifiEnabledStatusInject(wifiState->mEnabled);
                    if (wifiState->mEnabled && !mWiperObj->mLowiStatusValid) {
                        //Query WiFi HW capabilities via LOWI API
                        if (mWiperObj->mWifiScanObserver != NULL) {
                            mWiperObj->mWifiScanObserver->queryWiFiHwCapabilities();
                        }
                        mWiperObj->mLowiStatusValid = true;
                    }
                }
                break;
                case WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID:
                {
                    WifiSupplicantInfo supplicantInfo;
                    memset(&supplicantInfo, 0, sizeof(WifiSupplicantInfo));

                    WifiSupplicantStatusDataItem *wifiSupplicantStatus =
                            static_cast<WifiSupplicantStatusDataItem*>(dataItem);

                    supplicantInfo.apMacAddressValid = wifiSupplicantStatus->mApMacAddressValid;
                    supplicantInfo.wifiApSsidValid = wifiSupplicantStatus->mWifiApSsidValid;

                    if (supplicantInfo.apMacAddressValid) {
                        memcpy(&supplicantInfo.apMacAddress, wifiSupplicantStatus->mApMacAddress,
                               sizeof(supplicantInfo.apMacAddress));
                    }
                    if (supplicantInfo.wifiApSsidValid) {
                        strlcpy(supplicantInfo.ssid, wifiSupplicantStatus->mWifiApSsid.c_str(),
                                sizeof(supplicantInfo.ssid));
                    }

                    if (WifiSupplicantStatusDataItem::COMPLETED == wifiSupplicantStatus->mState) {
                        if (WifiSupplicantStatusDataItem::DISCONNECTED ==
                                mWiperObj->mLatestSupplicantState) {
                            supplicantInfo.attachState = WIFI_ACCESS_POINT_ATTACHED;
                            mWiperObj->mLbsAdapter->wifiAttachmentStatusInject(supplicantInfo);
                        } else if (WifiSupplicantStatusDataItem::COMPLETED ==
                                mWiperObj->mLatestSupplicantState) {
                            supplicantInfo.attachState = WIFI_ACCESS_POINT_HANDOVER;
                            mWiperObj->mLbsAdapter->wifiAttachmentStatusInject(supplicantInfo);
                        }
                        mWiperObj->mLatestSupplicantState = WifiSupplicantStatusDataItem::COMPLETED;
                    } else if ((WifiSupplicantStatusDataItem::DISCONNECTED ==
                            wifiSupplicantStatus->mState) &&
                            (WifiSupplicantStatusDataItem::DISCONNECTED !=
                            mWiperObj->mLatestSupplicantState)) {
                        supplicantInfo.attachState = WIFI_ACCESS_POINT_DETACHED;
                        mWiperObj->mLatestSupplicantState =
                                WifiSupplicantStatusDataItem::DISCONNECTED;
                        mWiperObj->mLbsAdapter->wifiAttachmentStatusInject(supplicantInfo);
                    }
                    LOC_LOGD ("LocTech-Label :: WIPER :: Wifi Supplicant Info In");
                    string wsi;
                    Wiper :: stringify (supplicantInfo, wsi);
                    LOC_LOGD ("LocTech-Value :: Wifi Supplicant Info: %s", wsi.c_str ());
                }
                break;
                case SHUTDOWN_STATE_DATA_ITEM_ID:
                {
                    ShutdownStateDataItem *shutdownState =
                            static_cast<ShutdownStateDataItem*>(dataItem);
                    if (shutdownState->mState)
                    {
                        const MsgTask *msgTask = new MsgTask("LBS_shutdown");
                        if (msgTask == NULL) {
                            LOC_LOGE("Failed to create msgTask");
                            break;
                        }

                        ContextBase *context = new (nothrow) ContextBase(msgTask, 0,
                                LocContext::mLBSLibName);
                        if (context == NULL) {
                            LOC_LOGE("Failed to create contextBase");
                            break;
                        }
                    }
                }
                break;
                case TIMEZONE_CHANGE_DATA_ITEM_ID:
                {
                    TimeZoneChangeDataItem *tz = static_cast<TimeZoneChangeDataItem*>(dataItem);
                    mWiperObj->mLbsAdapter->timeInfoInject((long)tz->mCurrTimeMillis,
                            tz->mRawOffsetTZ, tz->mDstOffsetTZ);
                }
                break;
                case TIME_CHANGE_DATA_ITEM_ID:
                {
                    TimeChangeDataItem *tz = static_cast<TimeChangeDataItem*>(dataItem);
                    mWiperObj->mLbsAdapter->timeInfoInject((long)tz->mCurrTimeMillis,
                            tz->mRawOffsetTZ, tz->mDstOffsetTZ);
                }
                break;

                default:
                break;
            }
        }
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
}

Wiper::handleOsObserverUpdateMsg::~handleOsObserverUpdateMsg()
{
    std::list<IDataItemCore *>::const_iterator it = mDataItemList.begin();
    for (; it != mDataItemList.end(); it++)
    {
        delete *it;
    }
}

void Wiper::handleSsrMsg::proc() const
{
    LOC_LOGD ("LocTech-Label :: WIPER :: handleSsrMsg");
    // Send back the required registration bits to modem
    // eg: so that ODCPI can be started again if required
    mWiperObj->setFeatureRegistrationBits();
}

void Wiper::wiperSubscribeInit::proc() const
{
    ENTRY_LOG();

    std::unordered_set<DataItemId> dataItemIdSet;

    // Subscribe for shutdown events
    dataItemIdSet.insert(SHUTDOWN_STATE_DATA_ITEM_ID);

    // Subscribe for time / timezone change events
    dataItemIdSet.insert(TIME_CHANGE_DATA_ITEM_ID);
    dataItemIdSet.insert(TIMEZONE_CHANGE_DATA_ITEM_ID);

    // Subscribe for Wifi on/off action. If free wifi scan injection (mask 0x2) is required then even
    // wifi on/off is required by geofence. There is no separate bit for WIFI ON/OFF requirement.
    if (FEATURE_FREE_WIFI_SCAN_INJECT_MASK & mWiperFlag) {
        dataItemIdSet.insert(WIFIHARDWARESTATE_DATA_ITEM_ID);
    }

    // Enable FEATURE_WIFI_SUPPLICANT_INFO_MASK -->> 0x8
    if (FEATURE_WIFI_SUPPLICANT_INFO_MASK & mWiperFlag) {
        dataItemIdSet.insert(WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID);
    }
    mWiperObj->mIzatContext->mSystemStatusOsObsrvr->subscribe(dataItemIdSet, mWiperObj);

    // Always subscribe for free wifi scan notification.
    mWiperObj->mWifiScanObserver =
        FreeWifiScanObserver::getInstance(mWiperObj->mIzatContext->mIPCMessagingProxyObj,
            mWiperObj->mIzatContext->mMsgTask);
    if (mWiperObj->mWifiScanObserver == NULL) {
        LOC_LOGE("Failed to get FreeWifiScanObserver");
    } else {
        mWiperObj->mWifiScanObserver->subscribe(mWiperObj);
        //Query WiFi HW capabilities via LOWI API
        mWiperObj->mWifiScanObserver->queryWiFiHwCapabilities();
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void Wiper::lbsShutDownMsg::proc() const
{
    LBSApiBase* lbsApi = (LBSApiBase*)mAdapter->getContext()->getLocApi()->getSibling();
    lbsApi->shutdown();
    mAdapter->getContext()->modemPowerVote(false);
}

void Wiper::handleTimeZoneInfoRequestMsg::proc() const
{
    std::unordered_set<DataItemId> dataItemIdSet;
    // Request for TIMEZONE_CHANGE_DATA_ITEM_ID or TIME_CHANGE_DATA_ITEM_ID - will give both
    // time & timezone information.
    dataItemIdSet.insert(TIMEZONE_CHANGE_DATA_ITEM_ID);
    mWiperObj->mIzatContext->mSystemStatusOsObsrvr->requestData(dataItemIdSet, mWiperObj);
}

void Wiper::wiperTimer::timeOutCallback() {
    if (mWiperObj->mWifiScanObserver != NULL) {
        mWiperObj->mWifiScanObserver->queryWiFiHwCapabilities();
    }
    delete this;
}

void Wiper::wifiCapabilitiesNotificationMsg::proc() const {
    LOC_LOGd("wifiCapabilitiesNotificationMsg, capabilities Mask from LOWI, status: %d", mStatus);

    if (!mStatus) {
        if (mWiperObj->mCapabilityQueryCnt < 3) {
            //Set a timer and request capability again after 5 seconds,
            //Repeat re-query 3 times
            wiperTimer * timer = new Wiper::wiperTimer(mWiperObj);
            timer->start(5*1000, false);
            mWiperObj->mCapabilityQueryCnt++;
        } else {
            mWiperObj->mCapabilityQueryCnt = 0;
        }
        return;
    }

    // Capablities status is true, reset mCapabilityQueryCnt
    mWiperObj->mCapabilityQueryCnt = 0;
    //Set HW capabilities
    LocationHwCapabilitiesMask mask = 0;
    if (mCapabilities.discoveryScanSupported) {
        mask |= LOCATION_WIFI_CAPABILITY_RSSI;
    }
    if (mCapabilities.rangingScanSupported) {
        mask |= LOCATION_WIFI_CAPABILITY_RTT;
    }
    //Report final feature mask
    mWiperObj->mLbsAdapter->getContext()->setHwCapabilities(mask);
    //Check qwes status
    int fids[] = {816, 817};
    LocFeatureStatus statusList[2];
    mWiperObj->mLbsAdapter->getContext()->checkFeatureStatus(fids, statusList, 2);
}

}//namespace izat_manager

void WiperApDataRequest::proc() const
{
    Wiper::handleWifiApDataRequest(mRequest);
}

void WiperSsrInform::proc() const
{
    Wiper::handleSsrInform();
}

void TimeZoneInfoRequest::proc() const {
    Wiper::handleTimeZoneInfoRequest();
}
