/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_NDEBUG 0
#include <stdint.h>
#include <inttypes.h>
#include <IzatRemoteApi.h>
#include "IzatWiFiDBReceiver.h"
#include "IzatWiFiDBProvider.h"
#include "IzatAltitudeReceiver.h"
#include "NlpApiStatusNotifier.h"
#include "MapDataApiStatusNotifier.h"
#include <mq_client/IPCMessagingProxy.h>
#include <IzatTypes.h>
#include <algorithm>
#include <vector>
#include <IzatDefines.h>
#include <loc_pla.h>
#include <log_util.h>
#include "loc_cfg.h"
#include "DataItemId.h"
#include "DataItemConcreteTypeFieldNames.h"
#include <IzatNotifierProxy.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "IzatRemoteApi"

using namespace std;
using namespace qc_loc_fw;
using namespace izat_manager;

namespace izat_remote_api {

IzatNotifier::IzatNotifier(const char* const name, const OutCard* subCard) :
        mNotifierProxy(IzatNotifierProxy::get(name)) {
}

IzatNotifier::~IzatNotifier() {
    mNotifierProxy->removeNotifier(this);
    IzatNotifierProxy::drop(mNotifierProxy);
}

void IzatNotifier::registerSelf() {
    mNotifierProxy->addNotifier(this, nullptr);
}

static const char* const sToTag = "TO";
static const char* const sFromTag = "FROM";
static const char* const sReqTag = "REQ";
static const char* const sInfoTag = "INFO";
static const char* const sAPListTag = "AP-LIST";
static const char* const sAPRttListTag = "AP-RTT-LIST";
static const char* const sAPRttMeasTag = "AP-RTT-MEAS";
static const char* const sAPLocationTag = "AP-LOC";
static const char* const sAPListCountTag = "AP-LIST-COUNT";
static const char* const sAPLocCountTag = "AP-LOC-COUNT";
static const char* const sScanListTag = "SCAN-LIST";
static const char* const sAPListStatusTag = "AP-LIST-STATUS";
static const char* const sULPLocationTag = "ULP-LOC";
static const char* const sAPOSLOCListTag = "AP-OBSLOC-DATA";
static const char* const sRequestDataConnectionTag = "REQUEST-DATA-CONNECTION";
static const char* const sReleaseDataConnectionTag = "RELEASE-DATA-CONNECTION";
static const char* const sLookUpReqTag = "LOOKUP-REQUEST";

const char WiFiDBUpdater::sName[] = "FDAL-UPDATER";

void WiFiDBUpdater::pushWiFiDB(vector<APLocationData>* l_data_ptr,
                               vector<APSpecialInfo>* s_data_ptr,
                               int days_valid, bool is_lookup) {
    const char DAYS_VALID[] = "DAYS-VALID";
    if (mNotifierProxy) {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr != card) {
            card->init();
            if (mDesQueue.size() > 0) {
                card->addString(sToTag, mDesQueue.front().c_str());
                mDesQueue.pop();
            }
            card->addString(sFromTag, sName);
            card->addString(sReqTag, sAPLocationTag);
            for (vector<APLocationData>::iterator it = l_data_ptr->begin();
                 it != l_data_ptr->end(); ++it) {
                APLocationData* ap_loc = &(*it);
                OutPostcard* locCard = OutPostcard::createInstance();
                if (locCard == nullptr) {
                    LOC_LOGe("locCard is null");
                } else {
                    locCard->init();
                    locCard->addUInt64("MAC-ADDRESS", ap_loc->mac_R48b);
                    locCard->addFloat("LATITUDE", ap_loc->latitude);
                    locCard->addFloat("LONGITUDE", ap_loc->longitude);
                    if (0 != (ap_loc->valid_mask & APLocationData::WIFIDBUPDATER_APLOC_MAR_VALID)) {
                        locCard->addFloat("MAX-ANTENA-RANGE", ap_loc->max_antena_range);
                    }
                    if (0 != (ap_loc->valid_mask &
                              APLocationData::WIFIDBUPDATER_APLOC_HORIZONTAL_ERR_VALID)) {
                        locCard->addFloat("HORIZONTAL-ERROR", ap_loc->horizontal_error);
                    }
                    if (0 != (ap_loc->valid_mask &
                              APLocationData::WIFIDBUPDATER_APLOC_RELIABILITY_VALID)) {
                        locCard->addUInt8("RELIABILITY", ap_loc->reliability);
                    }
                    if ((0 != (ap_loc->valid_mask &
                              APLocationData::WIFIDBUPDATER_APLOC_RTT_CAPABILITY_VALID)) &&
                           ap_loc->rttCapability != NOT_SUPPORTED) {
                        locCard->addFloat("ALTITUDE", ap_loc->altitude);
                        locCard->addFloat("RTT-RANGE-BIAS-MM", ap_loc->rttRangeBiasInMm);
                        locCard->addUInt8("ALTITUDE-REF-TYPE", (uint8_t)ap_loc->altRefType);
                        locCard->addUInt8("AP-LOCATION-QUALITY", (uint8_t)ap_loc->positionQuality);
                        locCard->addUInt8("RTT-CAPABILITY", (uint8_t)ap_loc->rttCapability);
                    }
                    locCard->finalize();
                    card->addCard(sAPLocationTag, locCard);
                    delete locCard;
                }
            }
            card->addUInt32(sAPLocCountTag, l_data_ptr->size());
            int mob_ap_cnt = 0, unk_ap_cnt = 0;
            for (vector<APSpecialInfo>::iterator it = s_data_ptr->begin();
                 it != s_data_ptr->end(); ++it) {
                APSpecialInfo* ap_info = &(*it);
                OutPostcard* locCard = OutPostcard::createInstance();
                if (locCard == nullptr) {
                    LOC_LOGe("locCard is null");
                } else {
                    locCard->init();
                    // corresponding to IZatAPBSSpecialInfoTypes::MOVING_AP_BS
                    if (1 == ap_info->info) {
                      locCard->addUInt64("AP-MOB-MAC", ap_info->mac_R48b);
                      locCard->finalize();
                      card->addCard("MOB-AP-LIST", locCard);
                      mob_ap_cnt++;
                    }
                    // corresponding to IZatAPBSSpecialInfoTypes::NO_INFO_AVAILABLE
                    else if (0 == ap_info->info) {
                      locCard->addUInt64("AP-UNKNOWN-MAC", ap_info->mac_R48b);
                      locCard->finalize();
                      card->addCard("UNK-AP-LIST", locCard);
                      unk_ap_cnt++;
                    }
                    // ignore other values of IZatAPBSSpecialInfoTypes, e.g. NOT_RESOLVED
                    delete locCard;
                }
            }
            card->addUInt32("AP-MOB-COUNT", mob_ap_cnt);
            card->addUInt32("AP-UNKNOWN-COUNT", unk_ap_cnt);

            card->addInt32(DAYS_VALID, days_valid);
            card->finalize();

            mNotifierProxy->sendCard(card);
        }
    }
}

void WiFiDBUpdater::reportNlpApiConnected() {
    if (mNotifierProxy) {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr != card) {
            card->init();
            card->addString(sToTag, "IZAT-MANAGER");
            card->addString(sFromTag, sName);
            card->addString(sReqTag, "NLP-API-CONNECTED");
            card->finalize();

            mNotifierProxy->sendCard(card);
        }
    }
}

void WiFiDBUpdater::handleMsg(InPostcard * const in_msg) {
    const char* info = nullptr;
    const char* req = nullptr;
    const char* status = nullptr;
    const char STATUS[] = "STATUS";
    const char SUCCESS[] = "SUCCESS";
    const char SERVICE[] = "SERVICE";
    const char REGISTER[] = "REGISTER-EVENT";
    uint32_t ap_count = 0;
    uint8_t aplist_status = 0;
    vector<APInfo> ap_list;
    UlpLocation ulpLoc;
    const void* ulpBlob = NULL;
    size_t ulpBloblength = 0;
    bool ulpLocValid = false;

    if (nullptr != in_msg) {
        if (0 == in_msg->getString(sInfoTag, &info)) {
            // result of handling sendAPListReq()
            if (0 == strncmp(sAPListTag, info,
                             sizeof(sAPListTag) - 1)) {

                if (0 != in_msg->getUInt32(sAPListCountTag, ap_count)) {
                  ap_count = 0;
                }

                const char* lookUpReq = nullptr;
                if (0 == in_msg->getString("STATUS", &lookUpReq)) {
                    if (0 == strncmp(sLookUpReqTag, lookUpReq, sizeof(sLookUpReqTag) - 1)) {
                        const char* reqFrom = nullptr;
                        if (0 == in_msg->getString("FROM", &reqFrom) && reqFrom != nullptr) {
                            std::string destination(reqFrom);
                            mDesQueue.push(destination);
                        }
                    }
                }

                // get AP list status
                if (0 != in_msg->getUInt8(sAPListStatusTag, aplist_status)) {
                  aplist_status = E_STD_FINAL;
                }

                // get current location if there is one
                memset(&ulpLoc, 0, sizeof(ulpLoc));
                if (0 == in_msg->getBlob (sULPLocationTag, &ulpBlob, &ulpBloblength)
                        && ulpBloblength == sizeof(ulpLoc))
                {
                  ulpLoc = *(UlpLocation *)ulpBlob;
                  ulpLocValid = true;
                }

                // process all AP cards
                for (uint32_t ii = 0; ii < ap_count; ++ii) {
                    InPostcard* ap_info_card = 0;
                    if (0 == in_msg->getCard(sAPListTag, &ap_info_card, ii) &&
                        nullptr != ap_info_card) {
                        APInfo ap_info;
                        unsigned long long mac, utcTime;
                        ap_info_card->getUInt64("MAC-ADDRESS", mac);
                        ap_info.mac_R48b = (uint64_t)mac;

                        // get cell ids if available
                        if (0 == ap_info_card->getUInt8("CELL-TYPE", ap_info.cellType)) {
                          ap_info_card->getUInt16("CELL-REG-1", ap_info.cellRegionID1);
                          ap_info_card->getUInt16("CELL-REG-2", ap_info.cellRegionID2);
                          ap_info_card->getUInt16("CELL-REG-3", ap_info.cellRegionID3);
                          ap_info_card->getUInt32("CELL-REG-4", ap_info.cellRegionID4);
                        }
                        else {
                          // unknown cell
                          ap_info.cellType = 0;
                          ap_info.cellRegionID1 = 0;
                          ap_info.cellRegionID2 = 0;
                          ap_info.cellRegionID3 = 0;
                          ap_info.cellRegionID4 = 0;
                        }

                        ap_info_card->getUInt8("SSID-BYTE-CNT", ap_info.ssid_valid_byte_count);
                        ap_info.ssid_valid_byte_count =
                                std::min((uint8_t)(ap_info.ssid_valid_byte_count),
                                         (uint8_t)AP_SSID_LENGTH);
                        //clear buffer
                        memset(ap_info.ssid, 0, sizeof(ap_info.ssid));
                        ap_info_card->getArrayInt8("SSID", (int*)&(ap_info.ssid_valid_byte_count),
                                                   (PostcardBase::INT8*)ap_info.ssid);
                        ap_info_card->getUInt64("UTC-TIME", utcTime);
                        ap_info_card->getUInt8("FDAL-STATUS", ap_info.fdal_status);
                        ap_info.utcTime = (uint64_t)utcTime;
                        ap_list.push_back(ap_info);
                        delete ap_info_card;
                    }
                }

                // send with location
                apListUpdate(&ap_list, (int)aplist_status, ulpLoc, ulpLocValid);
              // result of handling sendScanListReq()
              } else if (0 == strncmp(sScanListTag, info,
                             sizeof(sScanListTag) - 1)) {

                if (0 != in_msg->getUInt32(sAPListCountTag, ap_count)) {
                  ap_count = 0;
                }

                // get AP list status
                if (0 != in_msg->getUInt8(sAPListStatusTag, aplist_status)) {
                  aplist_status = E_SCAN_FINAL;
                }

                // process all AP cards
                for (uint32_t ii = 0; ii < ap_count; ++ii) {
                    InPostcard* ap_info_card = 0;
                    if (0 == in_msg->getCard(sAPListTag, &ap_info_card, ii) &&
                        nullptr != ap_info_card) {
                        APInfo ap_info;
                        unsigned long long mac, utcTime;

                        // clear ap_info (which also set cellType to invalid)
                        memset(&ap_info, 0, sizeof(ap_info));

                        // extract content of postcard
                        ap_info_card->getUInt64("MAC-ADDRESS", mac);
                        ap_info.mac_R48b = (uint64_t)mac;
                        ap_info_card->getUInt8("SSID-BYTE-CNT", ap_info.ssid_valid_byte_count);
                        ap_info.ssid_valid_byte_count =
                                std::min((uint8_t)(ap_info.ssid_valid_byte_count),
                                         (uint8_t)AP_SSID_LENGTH);
                        //clear buffer
                        memset(ap_info.ssid, 0, sizeof(ap_info.ssid));
                        ap_info_card->getArrayInt8("SSID", (int*)&(ap_info.ssid_valid_byte_count),
                                                   (PostcardBase::INT8*)ap_info.ssid);
                        ap_info_card->getUInt64("UTC-TIME", utcTime);
                        ap_info.utcTime = (uint64_t)utcTime;
                        ap_list.push_back(ap_info);
                        delete ap_info_card;
                    }
                }

                // send with invalid location for scan list request (no location is needed)
                memset(&ulpLoc, 0, sizeof(ulpLoc));
                apListUpdate(&ap_list, (int)aplist_status, ulpLoc, false);
                } else if (0 == strncmp(STATUS, info, sizeof(STATUS) - 1) &&
                       0 == in_msg->getString(STATUS, &status)) {
                if (0 == strncmp(status, SUCCESS, sizeof(SUCCESS) - 1)) {
                    statusUpdate(true, nullptr);
                } else {
                    statusUpdate(false, status);
                }
            } else if (0 == strncmp(REGISTER, info, sizeof(REGISTER) - 1)) {
                static bool isCallbackNotified = false;
                if (!isCallbackNotified) {
                    notifyCallbackEnv();
                    isCallbackNotified = true;
                }
            }
        }
        if (0 == in_msg->getString(sReqTag, &req) &&
            0 == strncmp(SERVICE, req, sizeof(SERVICE) - 1)) {
            serviceRequest();
        }
    }
}

const char WiFiDBUploader::sName[] = "CS-PROVIDER";

void WiFiDBUploader::sendAPOBSLocDataReq() {
    if (mNotifierProxy) {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr != card) {
            card->init();
            card->addString(sToTag, "XT-CS");
            card->addString(sFromTag, sName);
            card->addString(sReqTag, sAPOSLOCListTag);
            card->finalize();

            mNotifierProxy->sendCard(card);
        }
    }
}
void WiFiDBUploader::handleMsg(InPostcard * const in_msg) {
    const char* info = nullptr;
    const char* req = nullptr;
    const char SERVICE[] = "SERVICE";
    const char REGISTER[] = "REGISTER-EVENT";
    uint32_t ap_count = 0;
    uint32_t ap_rtt_count = 0;
    uint32_t scan_count = 0;
    uint8_t aplist_status = 0;
    InPostcard* scan_card = 0;
    vector<APObsData> ap_obs_data;
    UlpLocation GpsLoc;
    APCellInfo cell_info;

    if (nullptr != in_msg) {
        if (0 == in_msg->getString(sInfoTag, &info)) {
            // result of handling sendAPOBSLocDataReq()
            if (0 == strncmp(sAPOSLOCListTag, info,
                             sizeof(sAPOSLOCListTag) - 1) &&
                0 == in_msg->getUInt32("NUM_OF_SCANS", scan_count) ) {
                // get AP list status
                in_msg->getUInt8(sAPListStatusTag, aplist_status);
                LOC_LOGd("AP obs data: num of scans: %d", scan_count);

                for (uint32_t jj = 0; jj < scan_count; ++jj)
                {
                    if (0 == in_msg->getCard("SCAN-CARD", &scan_card, jj) &&
                        nullptr != scan_card) {
                        // get current location
                        APObsData apObsData;
                        memset(&apObsData, 0, sizeof(apObsData));
                        memset(&GpsLoc, 0, sizeof(GpsLoc));
                        InPostcard* loc_info_card = 0;
                        if (0 == scan_card->getCard("LOC-CARD", &loc_info_card) &&
                            nullptr != loc_info_card) {
                            unsigned long long utcTime = 0;
                            loc_info_card->getUInt64 ("TIMESTAMP-MS", utcTime);
                            apObsData.gpsLoc.gpsLocation.timestamp = (uint64_t)utcTime;
                            loc_info_card->getUInt16 ("VALID-MASK",
                                                      apObsData.gpsLoc.gpsLocation.flags);
                            loc_info_card->getDouble("LATITUDE",
                                                     apObsData.gpsLoc.gpsLocation.latitude);
                            loc_info_card->getDouble("LONGITUDE",
                                                     apObsData.gpsLoc.gpsLocation.longitude);
                            loc_info_card->getDouble("ALTITUDE",
                                                     apObsData.gpsLoc.gpsLocation.altitude);
                            loc_info_card->getFloat("ACCURACY",
                                                    apObsData.gpsLoc.gpsLocation.accuracy);
                            loc_info_card->getFloat("VER-ACCURACY",
                                                    apObsData.gpsLoc.gpsLocation.vertUncertainity);
                            loc_info_card->getFloat("SPEED",
                                                    apObsData.gpsLoc.gpsLocation.speed);
                            loc_info_card->getFloat("BEARING",
                                                    apObsData.gpsLoc.gpsLocation.bearing);
                            loc_info_card->getUInt16 ("POS-SOURCE",
                                                      apObsData.gpsLoc.position_source);
                            loc_info_card->getBool("POS-VALID",
                                                   apObsData.bUlpLocValid);
                        }
                        delete loc_info_card;

                        InPostcard* ril_info_card = 0;
                        apObsData.cellInfo.cellType = 0;
                        if (0 == scan_card->getCard("RIL-CARD", &ril_info_card) &&
                            nullptr != ril_info_card) {
                            ril_info_card->getUInt8("CELL-TYPE", apObsData.cellInfo.cellType);
                            ril_info_card->getUInt16("CELL-REG-1",
                                                     apObsData.cellInfo.cellRegionID1);
                            ril_info_card->getUInt16("CELL-REG-2",
                                                     apObsData.cellInfo.cellRegionID2);
                            ril_info_card->getUInt32("CELL-REG-3",
                                                     apObsData.cellInfo.cellRegionID3);
                            unsigned long long cellRegID4 = 0;
                            ril_info_card->getUInt64("CELL-REG-4",cellRegID4);
                            apObsData.cellInfo.cellRegionID4 = (uint64_t) cellRegID4;
                            ril_info_card->getUInt32("FREQUENCY",
                                                     apObsData.cellInfo.frequency);
                            ril_info_card->getUInt32("PHYSICALID",
                                                     apObsData.cellInfo.physicalId);
                        }
                        delete ril_info_card;

                        unsigned long long scanTimestamp = 0;
                        scan_card->getUInt64("SCAN-TIMESTAMP", scanTimestamp);
                        apObsData.scanTimestamp_ms = (uint64_t)scanTimestamp;
                        scan_card->getUInt32("AP-LIST-COUNT", ap_count);
                        if (ap_count > 0) {
                            // process all AP cards
                            for (uint32_t ii = 0; ii < ap_count; ++ii) {
                                InPostcard* ap_info_card = 0;
                                if (0 == scan_card->getCard(sAPListTag, &ap_info_card, ii)
                                    && nullptr != ap_info_card) {
                                    APScanInfo ap_info;
                                    unsigned long long mac;
                                    // extract content of postcard
                                    ap_info_card->getUInt64("MAC-ADDRESS", mac);
                                    ap_info.mac_R48b = (uint64_t)mac;
                                    ap_info_card->getUInt8("SSID-BYTE-CNT",
                                                           ap_info.ssid_valid_byte_count);
                                    ap_info.ssid_valid_byte_count =
                                        std::min((uint8_t)(ap_info.ssid_valid_byte_count),
                                                 (uint8_t)AP_SSID_LENGTH);
                                    //clear buffer
                                    memset(ap_info.ssid, 0, sizeof(ap_info.ssid));
                                    ap_info_card->getArrayInt8("SSID",
                                            (int*)&(ap_info.ssid_valid_byte_count),
                                            (PostcardBase::INT8*)ap_info.ssid);
                                    ap_info_card->getInt16("RSSI", ap_info.rssi);
                                    unsigned long long ap_scanTimestamp = 0;
                                    ap_info_card->getUInt64("AP-SCAN-TIMESTAMP-MS",
                                            ap_scanTimestamp);
                                    ap_info.age_usec = (uint64_t)
                                        (scanTimestamp-ap_scanTimestamp)*1000;
                                    ap_info_card->getInt32("CHANNEL-ID", ap_info.channel_id);
                                    uint8_t serviceStatus = 0;
                                    uint8_t apBandWidth = 0;
                                    ap_info_card->getUInt8("SERVICE-STATUS", serviceStatus);
                                    ap_info_card->getUInt32("AP-FREQUENCY", ap_info.frequency);
                                    ap_info_card->getUInt8("AP-BANDWIDTH", apBandWidth);
                                    ap_info.serviceStatus = (APServiceStatus) serviceStatus;
                                    ap_info.bandwidth = (APRangingBandwidth) apBandWidth;
                                    apObsData.ap_scan_info.push_back(ap_info);
                                    LOC_LOGd("basetimestamp %f ms, ap timestamp %f ms, age %f us",
                                             (double) scanTimestamp, (double) ap_scanTimestamp,
                                             (double) ap_info.age_usec);
                                    delete ap_info_card;
                                }
                            }
                        } else {
                            // process all AP RTT cards
                            scan_card->getUInt32("AP-RTT-LIST-COUNT", ap_rtt_count);
                            if (ap_rtt_count > 0) {
                                for (uint32_t ii = 0; ii < ap_rtt_count; ++ii) {
                                    InPostcard* ap_rtt_info_card = 0;
                                    if (0 == scan_card->getCard(sAPRttListTag,
                                            &ap_rtt_info_card, ii)
                                            && nullptr != ap_rtt_info_card) {
                                        unsigned long long mac = 0;
                                        uint16_t numAttempted = 0;
                                        unsigned long long timeStamp_ap = 0;
                                        uint16_t rttMeasNum = 0;
                                        std::vector<APRangingMeasurement> rttMeasVec;
                                        ap_rtt_info_card->getUInt64("MAC-ADDRESS", mac);
                                        ap_rtt_info_card->getUInt16("NUM-ATTEMPTED", numAttempted);
                                        ap_rtt_info_card->getUInt64("SCAN-TIMESTAMP-MS",
                                                timeStamp_ap);
                                        uint64_t age =
                                                (uint64_t)(scanTimestamp - timeStamp_ap) * 1000;
                                        ap_rtt_info_card->getUInt16("RTT-MEAS-NUM", rttMeasNum);
                                        LOC_LOGd("WiFiDBUploader, mac: %" PRIx64 ", "
                                                "num ATTEMPTED: %d, age: %" PRIu64 ", "
                                                "rttMeasNum: %d",
                                                (uint64_t)mac, numAttempted, age, rttMeasNum);
                                        if (rttMeasNum > 0) {
                                            for (uint32_t kk = 0; kk < rttMeasNum; ++kk) {
                                                InPostcard* ap_rtt_meas_card = 0;
                                                if (0 == ap_rtt_info_card->getCard(sAPRttMeasTag,
                                                        &ap_rtt_meas_card, kk)
                                                        && nullptr != ap_rtt_meas_card) {
                                                    uint32_t distanceMm = 0;
                                                    int16_t rssi = 0;
                                                    uint8_t txBandwidth = 0;
                                                    uint8_t rxBandwidth = 0;
                                                    int16_t chainNumber = 0;
                                                    ap_rtt_meas_card->getUInt32("DISTANCE-MM",
                                                            distanceMm);
                                                    ap_rtt_meas_card->getInt16("RSSI", rssi);
                                                    ap_rtt_meas_card->getUInt8("TX-BANDWIDTH",
                                                            txBandwidth);
                                                    ap_rtt_meas_card->getUInt8("RX-BANDWIDTH",
                                                            rxBandwidth);
                                                    ap_rtt_meas_card->getInt16("CHAIN-NUM",
                                                            chainNumber);
                                                    APRangingMeasurement rttMeas(distanceMm,
                                                            rssi,
                                                            (APRangingBandwidth) txBandwidth,
                                                            (APRangingBandwidth) rxBandwidth,
                                                            chainNumber);
                                                    rttMeasVec.push_back(rttMeas);
                                                    LOC_LOGd("WiFiDBUploader, rtt meas, distanceMm"
                                                            " %d, rssi: %d, tx: %d, rx: %d,"
                                                            "chain no: %d",
                                                            distanceMm, rssi, txBandwidth,
                                                            rxBandwidth, chainNumber);
                                                    delete ap_rtt_meas_card;
                                                }
                                            }
                                        }
                                        APRttScanInfo rttScanInfo((uint64_t)mac, numAttempted,
                                                age, rttMeasVec);
                                        apObsData.ap_rtt_scan_info.push_back(rttScanInfo);
                                        delete ap_rtt_info_card;
                                    }
                                }
                            }
                        }
                        ap_obs_data.push_back(apObsData);
                        delete scan_card;
                    } else {
                        LOC_LOGe("get scan card failure for index = %d for scancount %d",
                                 jj, scan_count);
                    }
                }
                // send with location and cell info
                onAPObsLocDataAvailable(&ap_obs_data, (int)aplist_status);
            }
        }
        if (0 == in_msg->getString(sReqTag, &req) &&
            0 == strncmp(SERVICE, req, sizeof(SERVICE) - 1)) {
            serviceRequest();
        }
        else if (0 == strncmp(REGISTER, info, sizeof(REGISTER) - 1)) {
            static bool isCallbackNotified = false;
            if (!isCallbackNotified) {
                notifyCallbackEnv();
                isCallbackNotified = true;
            }
        }
    }
}

// =========================================================

std::string NlpApiStatusNotifier::sName = "NLP-API" + to_string(getpid());

NlpApiStatusNotifier::NlpApiStatusNotifier() : IzatNotifier(sName.c_str(), nullptr) {
    registerLocationUpdate();

    char prefApn[LOC_MAX_PARAM_STRING] = "";
    memset(prefApn, 0, LOC_MAX_PARAM_STRING);
    mBackhaulContext = { "nlp-client", LOC_PRIMARY_SUB, prefApn, LOC_APN_IP_IPV4 };

    // initialize the parameters for connect/disconnect backhaul
    loc_param_s_type data_conf_params[] = {
        {"INTERNET_APN", &prefApn, NULL, 's'},
        {"INTERNET_IP_TYPE", &mBackhaulContext.prefIpType, NULL, 'n'},
        {"INTERNET_SUB_ID", &mBackhaulContext.prefSub, NULL, 'n'}
    };
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, data_conf_params);

    mBackhaulContext.prefApn = prefApn;
    LOC_LOGd("prefApn = %s", mBackhaulContext.prefApn.c_str());
    LOC_LOGd("prefIpType = %d", mBackhaulContext.prefIpType);
    LOC_LOGd("prefSub = %d", mBackhaulContext.prefSub);
}

bool NlpApiStatusNotifier::requestOrReleaseDataConnection(bool connectIfTrueElseRelease) {
    if (mNotifierProxy) {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr != card) {
            card->init();
            card->addString(sToTag, "OS-Agent");
            card->addString(sFromTag, sName.c_str());
            card->addString(sReqTag,
                    connectIfTrueElseRelease ? sRequestDataConnectionTag :
                            sReleaseDataConnectionTag);
            BackhaulContext& ctxt = getBackhaulContext();
            LOC_LOGd("clientName = %s prefSub = %d prefApn = %s prefIpType = %d",
                      ctxt.clientName.c_str(), ctxt.prefSub,
                      ctxt.prefApn.c_str(), ctxt.prefIpType);
            card->addString("BACKHAUL-CLIENT", ctxt.clientName.c_str());
            card->addUInt16("BACKHAUL-SUB", ctxt.prefSub);
            card->addString("BACKHAUL-APN", ctxt.prefApn.c_str());
            card->addUInt16("BACKHAUL-IPTYPE", ctxt.prefIpType);
            card->finalize();

            mNotifierProxy->sendCard(card);
            return true;
        }
        return false;
    }
    return false;
}

void NlpApiStatusNotifier::handleMsg(InPostcard * const in_msg) {
    LOC_LOGv( "NlpApiStatusNotifier handleMsg >>> \n");

    const char * req = 0;
    const char * info = 0;

    if (0 == (in_msg->getString("INFO", &info))) {
        LOC_LOGd("Info [%s]", info);

        if (0 == strncmp("OS-STATUS-UPDATE", info, sizeof("OS-STATUS-UPDATE"))) {
            handleOptInStatusChange(in_msg);
            handleConnectivityStatusChange(in_msg);
            handleEmergencyStatusChange(in_msg);
        } else if (0 == strncmp("REGISTER-EVENT", info, sizeof("REGISTER-EVENT"))) {
            registerOSAgentUpdate();
        } else if (0 == strncmp("LOCATION-UPDATE", info, sizeof("LOCATION-UPDATE"))) {
            handleLocationChange(in_msg);
        }
    }

    LOC_LOGv( "NlpApiStatusNotifier handleMsg <<< \n");
}

void NlpApiStatusNotifier::handleEmergencyStatusChange(InPostcard * const in_msg)
{
    LOC_LOGv( "handleEmergencyStatusChange >>> \n");

    if (nullptr == in_msg) {
        LOC_LOGe("handleEmergencyStatusChange in_msg is nullptr");
        return;
    }
    bool is_emergency = false;
    InPostcard* esCard = 0;
    if (0 != in_msg->getCard("IN_EMERGENCY_CALL", &esCard)) {
        LOC_LOGe("handleEmergencyStatusChange ES card not found");
    } else {
        if (0 != esCard->getBool("IS_EMERGENCY", is_emergency)) {
            LOC_LOGe("handleEmergencyStatusChange:failed to retrieve IS_EMERGENCY, ignoring");
        } else {
            LOC_LOGd("handleEmergencyStatusChange: isEmergency = %d", is_emergency);
            onLocationRequestUpdate(is_emergency);
        }
    }
    if (nullptr != esCard) {
        delete esCard;
    }
}

void NlpApiStatusNotifier::handleConnectivityStatusChange(InPostcard * const in_msg)
{
    LOC_LOGv( "handleConnectivityStatusChange >>> \n");

    int  result = 0;
    bool isConected = 0;

    Network networkList[10] = {};

    uint8_t validNetworkIndex = 0;

    InPostcard* active_network_info = 0;

    do
    {
        BREAK_IF_ZERO (-2, in_msg);

        if (0 != in_msg->getBool(NETWORKINFO_FIELD_CONNECTED, isConected))
        {
            LOC_LOGe( "handleConnectivityStatusChange: failed to retrieve "
                      "NETWORKINFO_FIELD_CONNECTED, ignoring");
            result = -2;
            break;
        } else {
            LOC_LOGv( "handleConnectivityStatusChange isConected: %u", isConected);
        }

        // If connectivity is available, retrieve ACTIVE_NETWORK_INFO
        if (0 != in_msg->getCard(NETWORKINFO_CARD, &active_network_info))
        {
            // we do not have active network info
            LOC_LOGv("handleConnectivityStatusChange, no information for active_network_info");
            result = -1;
            break;
        }
    } while (0);

    LOC_LOGd( "handleConnectivityStatusChange isConected: %u validNetworkIndex: %u",
        isConected, validNetworkIndex);
    if (active_network_info != nullptr) {
        delete active_network_info;
        active_network_info = nullptr;
    }

    if (0 == result) {
        onNetworkStatusUpdate(isConected, networkList, validNetworkIndex);
    }
}

void NlpApiStatusNotifier::handleOptInStatusChange(InPostcard * const in_msg)
{
    LOC_LOGv( "handleOptInStatusChange >>> \n");

    if (nullptr == in_msg) {
        LOC_LOGe("handleOptInStatusChange in_msg is nullptr");
        return;
    }
    int  result = 0;
    bool isOptIn = false;
    InPostcard* enhCard = 0;
    if (0 != in_msg->getCard("ENH", &enhCard)) {
        LOC_LOGe("handleOptInStatusChange ENH card not found");
    } else {
        if (0 != enhCard->getBool(ENH_FIELD_ENABLED, isOptIn)) {
            LOC_LOGe("handleOptInStatusChange:failed to retrieve ENH_FIELD_ENABLED, ignoring");
        } else {
            LOC_LOGd("handleOptInStatusChange: isOptIn = %d", isOptIn);
            onOptInStatusUpdate(isOptIn);
        }
    }
    if (nullptr != enhCard) {
        delete enhCard;
    }
}

void NlpApiStatusNotifier::handleLocationChange(InPostcard* const in_msg) {
    LOC_LOGv( "handleLocationChange >>> \n");
    const void* blob = nullptr;
    UlpLocation  ulpLocation;
    GpsLocationExtended locExt;
    size_t  length = 0;
    memset(&ulpLocation, 0, sizeof(ulpLocation));
    memset(&locExt, 0, sizeof(locExt));
    if (0 == in_msg->getBlob("ULPLOC", &blob, &length) && length == sizeof(ulpLocation)) {
        ulpLocation = *(UlpLocation*)blob;
    }
    if (0 == in_msg->getBlob("LOCEXTENDED", &blob, &length) && length == sizeof(locExt)) {
        locExt = *(GpsLocationExtended*)blob;
    }
    onLocationChange(ulpLocation, locExt);
    LOC_LOGv( "handleLocationChange <<< \n");
}

int NlpApiStatusNotifier::registerOSAgentUpdate()
{
    LOC_LOGv( "registerOSAgentUpdate >>> \n");

    int result = -1;

    int32_t osstatusSubscribe[] =
    {
        ENH_DATA_ITEM_ID,
        NETWORKINFO_DATA_ITEM_ID,
        IN_EMERGENCY_CALL_DATA_ITEM_ID
    };

    do
    {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr == card)
        {
            LOC_LOGe("registerOSAgentUpdate,failed to create postcard \n");
            result = -1;
            break;
        }

        if (card->init() != 0)
        {
            LOC_LOGe("registerOSAgentUpdate,failed to init postcard \n");
            result = -1;
            break;
        }

        if (card->addString (sToTag, "OS-Agent") != 0 ||
            card->addString (sFromTag, sName.c_str()) != 0 ||
            card->addString (sReqTag, "SUBSCRIBE") != 0 ||
            card->addArrayInt32("DATA-ITEMS",
                sizeof(osstatusSubscribe) / sizeof(int32_t),
                osstatusSubscribe) != 0)
        {
            LOC_LOGe("registerOSAgentUpdate,failed to add elements to postcard \n");
            result = -1;
            break;
        }
        if (card->finalize  () != 0)
        {
            LOC_LOGe("registerOSAgentUpdate,failed to finalize postcard \n");
            result = -1;
            break;
        }
        mNotifierProxy->sendCard(card);

        result = 0;
    } while (0);

    if (0 != result)
    {
        LOC_LOGe("registerOSAgentUpdate failed: %d\n", result);
    }
  return result;
}

int NlpApiStatusNotifier::registerLocationUpdate() {
    OutPostcard *card = nullptr;
    int result = 0;
    static const izat_manager::IzatListenerMask listensTo =
        (IZAT_STREAM_FUSED | IZAT_STREAM_NETWORK | IZAT_STREAM_GNSS);

    do {
        if (mNotifierProxy) {
            card = OutPostcard::createInstance();
            BREAK_IF_ZERO(100, card);
            BREAK_IF_NON_ZERO(101, card->init());
            BREAK_IF_NON_ZERO(101, card->addString("TO", "IZAT-MANAGER"));
            BREAK_IF_NON_ZERO(102, card->addString("FROM", sName.c_str()));
            BREAK_IF_NON_ZERO(103, card->addString("REQ", "PASSIVE-LOCATION"));
            BREAK_IF_NON_ZERO(104, card->addUInt16("LISTENS-TO", listensTo));
        }
    } while (0);
    if (0 != result) {
        LOC_LOGe("failed: %d", result);
    }
    return result;
}

const char AltitudeReceiverUpdater::sName[] = "ALT-PROVIDER";

void AltitudeReceiverUpdater::reportZAxisApiConnected() {
    if (mNotifierProxy) {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr != card) {
            card->init();
            card->addString(sToTag, "IZAT-MANAGER");
            card->addString(sFromTag, AltitudeReceiverUpdater::sName);
            card->addString(sReqTag, "ZAXIS-API-CONNECTED");
            card->finalize();

            mNotifierProxy->sendCard(card);
        }
    }
}

void AltitudeReceiverUpdater::handleMsg(InPostcard * const in_msg) {
    LOC_LOGv( "AltitudeReceiverUpdater handleMsg >>> \n");

    const char * info = nullptr;

    if (nullptr != in_msg) {
        if (0 == in_msg->getString("INFO", &info)) {
            if (0 == strncmp("ALT", info, sizeof("ALT"))) {
                NlpLocation location = {};
                location.locationFlagsMask = NlpLocation::ALTITUDERECEIVER_NLPLOC_LAT_LONG |
                        NlpLocation::ALTITUDERECEIVER_NLPLOC_ACCURACY;
                bool isEmergency;
                qc_loc_fw::PostcardBase::UINT64 elapsedRealTimeInMs, timestamp;
                in_msg->getUInt64("ELAPSED_REAL_TIME-MS", elapsedRealTimeInMs);
                in_msg->getUInt64("TIMESTAMP-MS", timestamp);
                in_msg->getDouble("LATITUDE", location.latitude);
                in_msg->getDouble("LONGITUDE", location.longitude);
                in_msg->getBool("IS_EMERGENCY", isEmergency);
                in_msg->getFloat("ACCURACY", location.accuracy);
                location.elapsedRealTimeInMs = (uint64_t)elapsedRealTimeInMs;
                location.timestamp = (uint64_t)timestamp;
                onAltitudeLookUp(location, isEmergency);
            }
        }
    }

    LOC_LOGv( "AltitudeReceiverUpdater handleMsg <<< \n");
}

void AltitudeReceiverUpdater::pushAltitude(const NlpLocation location) {
    if (mNotifierProxy) {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr != card) {
            card->init();
            card->addString(sToTag, "ALT-RECV-PROXY");
            card->addString(sFromTag, AltitudeReceiverUpdater::sName);
            card->addString(sReqTag, "LOCATION_WITH_ZAXIS");
            card->addString(sInfoTag, "LOCATION_WITH_ZAXIS");
            const void* blob = (const void*) &location;
            size_t length = sizeof (location);
            card->addBlob("LOC_WITH_Z", blob, length);
            card->finalize();

            mNotifierProxy->sendCard(card);
        }
    }
}

// =========================================================

const char MapDataApiStatusNotifier::sName[] = "MAP-DATA-API";

MapDataApiStatusNotifier::MapDataApiStatusNotifier() : IzatNotifier(sName, nullptr),
        isInChina(POSITION_UNKNOWN) {
    subOrUnsubLocationUpdate();
    subOrUnsubOSAgentUpdate();
}

void MapDataApiStatusNotifier::handleMsg(InPostcard * const in_msg) {
    LOC_LOGd("MapDataApiStatusNotifier handleMsg >>> \n");

    const char * info = 0;
    InPostcard* innerCard = nullptr;
    bool trackingState = false;
    if (0 == (in_msg->getString("INFO", &info))) {
        LOC_LOGd("Info [%s]", info);

        if (0 == strncmp("OS-STATUS-UPDATE", info, sizeof("OS-STATUS-UPDATE"))) {
            handleMccMncChange(in_msg);
            if ((0 == in_msg->getCard("TRACKING_STARTED", &innerCard)) && (innerCard != nullptr)
                    && (0 == innerCard->getBool("IS_TRACKING_STARTED", trackingState))) {
                LOC_LOGd("isTracking: %s", trackingState ? "true" : "false");
                onTrackingStateChange(trackingState);
            }
        } else if (0 == strncmp("REGISTER-EVENT", info, sizeof("REGISTER-EVENT"))) {
            const char * client = 0;
            if (0 == (in_msg->getString("CLIENT", &client))) {
                if (0 == strncmp("IZAT-MANAGER", client, sizeof("IZAT-MANAGER"))) {
                    LOC_LOGd("Received izat manager registration event");
                    subOrUnsubLocationUpdate();
                } else if (0 == strncmp("OS-Agent", client, sizeof("OS-Agent"))) {
                    LOC_LOGd("Received OS agent registration event");
                    subOrUnsubOSAgentUpdate();
                }
            }
        } else if (0 == strncmp("LOCATION-UPDATE", info, sizeof("LOCATION-UPDATE"))) {
            handleLocationChange(in_msg);
        }
    }
    if (innerCard) {
        delete innerCard;
    }
    LOC_LOGd("MapDataApiStatusNotifier handleMsg <<< \n");
}

void MapDataApiStatusNotifier::handleMccMncChange(InPostcard * const in_msg) {
    LOC_LOGv("handleMccMncChange >>> \n");
    int  result = 0;

    const char* mccMncStr = NULL;
    InPostcard* mccmnc_info = 0;
    do {
        BREAK_IF_ZERO(-1, in_msg);
        if (0 != in_msg->getCard(MCCMNC_CARD, &mccmnc_info)) {
            // we do not have mccmnc info
            LOC_LOGe("No information for mccmnc_info");
            result = -2;
            break;
        } else {
            if (0 != mccmnc_info->getStringDup(MCCMNC_FIELD_NAME, &mccMncStr)) {
                LOC_LOGe("handleMccMncChange: failed to retrieve "
                    "MCCMNC_FIELD_NAME, ignoring");
                result = -3;
                break;
            } else {
                LOC_LOGd("MCCMNC_DATA_ITEM_ID mccMncStr: %s", mccMncStr);
            }
        }
    } while (0);

    if (0 == result) {
        // MCC is the first 3 characters in mccMncStr
        if (isdigit(mccMncStr[0])) {
            if (0 == strncmp(mccMncStr, MCC_CHINA1, 3) || 0 == strncmp(mccMncStr, MCC_CHINA2, 3)) {
                isInChina = POSITION_IN_CHINA;
            } else {
                isInChina = POSITION_NOT_IN_CHINA;
            }
        } else {
            isInChina = POSITION_UNKNOWN;
        }
        LOC_LOGd("isInChina: %d", isInChina);
    }
    if (mccmnc_info) {
        delete mccmnc_info;
        mccmnc_info = nullptr;
    }
    LOC_LOGv("handleMccMncChange <<< \n");
}

void MapDataApiStatusNotifier::handleLocationChange(InPostcard* const in_msg) {
    LOC_LOGv("handleLocationChange >>> \n");
    const void* blob = nullptr;
    UlpLocation  ulpLocation;
    GpsLocationExtended locExt;
    size_t  length = 0;
    int  result = 0;

    memset(&ulpLocation, 0, sizeof(ulpLocation));
    memset(&locExt, 0, sizeof(locExt));
    do {
        if (0 == in_msg->getBlob("ULPLOC", &blob, &length) && length == sizeof(ulpLocation)) {
            ulpLocation = *(UlpLocation*)blob;
        }
        else {
            LOC_LOGe("Cannot get ulpLocation!");
            result = -1;
            break;
        }

        if (0 == in_msg->getBlob("LOCEXTENDED", &blob, &length) && length == sizeof(locExt)) {
            locExt = *(GpsLocationExtended*)blob;
        }
        else {
            LOC_LOGe("Cannot get locExt!");
            result = -2;
            break;
        }
    } while (0);

    if (0 == result) {
        onLocationChange(ulpLocation, locExt);
    }
    LOC_LOGv("handleLocationChange <<< \n");
}

int MapDataApiStatusNotifier::subOrUnsubLocationUpdate(bool unsubscribe) {
    OutPostcard *card = nullptr;
    int result = 0;
    //    static const izat_manager::IzatListenerMask listensTo =
    //        (IZAT_STREAM_FUSED | IZAT_STREAM_NETWORK | IZAT_STREAM_GNSS);

    LOC_LOGd("Enter mNotifierProxy=%p sName=%s", mNotifierProxy, sName);
    do {
        if (mNotifierProxy) {
            card = OutPostcard::createInstance();
            BREAK_IF_ZERO(100, card);
            BREAK_IF_NON_ZERO(101, card->init());
            BREAK_IF_NON_ZERO(102, card->addString("TO", "IZAT-MANAGER"));
            BREAK_IF_NON_ZERO(103, card->addString("FROM", sName));
            if (unsubscribe) {
                BREAK_IF_NON_ZERO(104, card->addString("REQ", "PASSIVE-LOCATION-UNSUBSCRIBE"));
            } else {
                BREAK_IF_NON_ZERO(104, card->addString("REQ", "PASSIVE-LOCATION"));
                //BREAK_IF_NON_ZERO(105, card->addUInt16("LISTENS-TO", listensTo));
                BREAK_IF_NON_ZERO(105, card->addUInt16("LISTENS-TO", IZAT_STREAM_GNSS));
            }
            BREAK_IF_NON_ZERO(106, card->finalize());
            mNotifierProxy->sendCard(card);
        }
        LOC_LOGd("sending request for location update");
    } while (0);

    if (0 != result) {
        LOC_LOGe("failed: %d", result);
    }
    return result;
}

int MapDataApiStatusNotifier::sendMapData(const string maString) {
    OutPostcard *card = nullptr;
    int result = 0;
    uint32_t dataSize;

    dataSize = maString.length();
    LOC_LOGd("Enter dataSize=%d", dataSize);
    do {
        if (mNotifierProxy) {
            card = OutPostcard::createInstance();
            BREAK_IF_ZERO(100, card);
            BREAK_IF_NON_ZERO(101, card->init());
            BREAK_IF_NON_ZERO(101, card->addString("TO", "IZAT-MANAGER"));
            BREAK_IF_NON_ZERO(102, card->addString("FROM", sName));
            BREAK_IF_NON_ZERO(103, card->addString("REQ", "MAP-DATA"));
            BREAK_IF_NON_ZERO(104, card->addUInt32("DATA-SIZE", dataSize));
            for (uint16_t i = 0; i < dataSize; i++) {
                std::string fieldName = "DATA_" + std::to_string(i);
//                LOC_LOGv("send fieldName=%s *(data+%d)=0x%X", fieldName.c_str(), i, maString[i]);
                BREAK_IF_NON_ZERO(105, card->addInt8(fieldName.c_str(), maString[i]));
            }
            if (0 != result) {
                break;
            }
            BREAK_IF_NON_ZERO(106, card->finalize());
            mNotifierProxy->sendCard(card);
        }
        LOC_LOGd("sending Map Data");
    } while (0);

    if (0 != result) {
        LOC_LOGe("failed: %d", result);
    }
    return result;
}

int MapDataApiStatusNotifier::subOrUnsubOSAgentUpdate(bool unsubscribe)
{
    LOC_LOGv("subOrUnsubOSAgentUpdate >>> \n");

    int result = -1;

    int32_t osstatusSubscribe[] =
    {
        MCCMNC_DATA_ITEM_ID,
        TRACKING_STARTED_DATA_ITEM_ID
    };

    do {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr == card) {
            LOC_LOGe("subOrUnsubOSAgentUpdate, failed to create postcard \n");
            result = -1;
            break;
        }

        if (card->init() != 0) {
            LOC_LOGe("subOrUnsubOSAgentUpdate, failed to init postcard \n");
            result = -2;
            break;
        }
        if (unsubscribe) {
            card->addString(sReqTag, "UNSUBSCRIBE");
        } else {
            card->addString(sReqTag, "SUBSCRIBE");
        }
        if (card->addString(sToTag, "OS-Agent") != 0 ||
            card->addString(sFromTag, sName) != 0 ||
            card->addArrayInt32("DATA-ITEMS",
                                sizeof(osstatusSubscribe) / sizeof(int32_t),
                                osstatusSubscribe) != 0) {
            LOC_LOGe("subOrUnsubOSAgentUpdate, failed to add elements to postcard \n");
            result = -3;
            break;
        }
        if (card->finalize() != 0) {
            LOC_LOGe("subOrUnsubOSAgentUpdate, failed to finalize postcard \n");
            result = -4;
            break;
        }
        mNotifierProxy->sendCard(card);

        result = 0;
    } while (0);

    if (0 != result)
    {
        LOC_LOGe("subOrUnsubOSAgentUpdate failed: %d\n", result);
    }
    return result;
}

} // namespace izat_remote_api
