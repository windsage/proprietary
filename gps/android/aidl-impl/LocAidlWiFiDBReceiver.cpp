/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_IzatWiFiDBReceiver"
#define LOG_NDEBUG 0

#include "LocAidlWiFiDBReceiver.h"

#include <log_util.h>
#include <gps_extended_c.h>
#include <LocationAPI.h>
#include <izat_remote_api.h>
#include <IzatRemoteApi.h>
#include <IzatTypes.h>
#include <string>
#include <izat_wifi_db_receiver.h>
#include <IzatWiFiDBReceiver.h>

using namespace izat_remote_api;
using namespace izat_manager;

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {


// ======== WiFiDBUpdater ========
class WiFiDBUpdaterWrapper : public WiFiDBUpdater {
    const wifiDBRecvApListUpdateCb mApListAvailCb;
    const wifiDBRecvStatusUpdateCb mStatusCb;
    const wifiDBRecvServiceRequestCb mServiceReqCb;
    const wifiDBRecvEnvNotifyCb mEnvNotifyCb;
    const void *mClientData;
public:
    inline WiFiDBUpdaterWrapper(wifiDBRecvApListUpdateCb apListAvailCb,
                                wifiDBRecvStatusUpdateCb statusCb,
                                wifiDBRecvServiceRequestCb serviceReqCb,
                                wifiDBRecvEnvNotifyCb envNotifyCb,
                                void *clientData) :
            WiFiDBUpdater(),
            mApListAvailCb(apListAvailCb),
            mStatusCb(statusCb),
            mServiceReqCb(serviceReqCb),
            mEnvNotifyCb(envNotifyCb),
            mClientData(clientData) {
        registerSelf();
    }
    inline virtual void statusUpdate(bool isSuccess, const char *errStr) override {
        mStatusCb(isSuccess, errStr, (void *)mClientData);
    }
    inline virtual void apListUpdate(std::vector<APInfo>* ap_list_ptr, const int apListStatus,
            const UlpLocation& ulpLoc, const bool bUlpLocValid) override {
        if (nullptr != ap_list_ptr) {
            bool completeList = true;
            std::vector<APInfo>& ap_list = *ap_list_ptr;
            APInfo_s **ap_list_arr = new APInfo_s *[ap_list.size()];
            if (ap_list_arr!=NULL) {
                for (size_t ii = 0; ii < ap_list.size(); ++ii) {
                    ap_list_arr[ii] = new APInfo_s;
                    if (ap_list_arr[ii]!=NULL) {
                        ap_list_arr[ii]->mac_R48b = ap_list[ii].mac_R48b;
                        ap_list_arr[ii]->cell_type = ap_list[ii].cellType;
                        ap_list_arr[ii]->cell_id1 = ap_list[ii].cellRegionID1;
                        ap_list_arr[ii]->cell_id2 = ap_list[ii].cellRegionID2;
                        ap_list_arr[ii]->cell_id3 = ap_list[ii].cellRegionID3;
                        ap_list_arr[ii]->cell_id4 = ap_list[ii].cellRegionID4;
                        memcpy(ap_list_arr[ii]->ssid, ap_list[ii].ssid,
                               sizeof(ap_list_arr[ii]->ssid));
                        ap_list_arr[ii]->ssid_valid_byte_count = ap_list[ii].ssid_valid_byte_count;
                        ap_list_arr[ii]->timestamp = ap_list[ii].utcTime;
                        ap_list_arr[ii]->fdal_status = ap_list[ii].fdal_status;
                    } else {
                        completeList=false;
                        break;
                    }
                }
                if (true == completeList) {
                    mApListAvailCb(ap_list_arr, ap_list.size(), apListStatus, ulpLoc, bUlpLocValid,
                            (void *)mClientData);
                }
                for (size_t ii = 0; ii < ap_list.size(); ++ii) {
                    if (ap_list_arr[ii]!=NULL) {
                        delete ap_list_arr[ii];
                    }
                }
                delete[] ap_list_arr;
            }
        }
    }
    inline virtual void serviceRequest() override {
        mServiceReqCb((void *)mClientData);
    }
    inline virtual void notifyCallbackEnv() override {
        if (nullptr != mEnvNotifyCb) {
            mEnvNotifyCb((void *)mClientData);
        }
    }
};

static std::shared_ptr<ILocAidlWiFiDBReceiverCallback> sCallback = nullptr;
std::shared_ptr<LocAidlDeathRecipient> LocAidlWiFiDBReceiver::mDeathRecipient = nullptr;

void WiFiDBReceiverServiceDied(void* cookie) {
    LOC_LOGe("ILocAidlWiFiDBReceiverCallback died.");
    sCallback = nullptr;
    auto thiz = static_cast<LocAidlWiFiDBReceiver*>(cookie);
    if (nullptr != thiz) {
        thiz->handleAidlClientSsr();
        thiz = nullptr;
    }
}

static void DBReceiverApListUpdateCb(APInfo_s *ap_list[], size_t ap_list_size,
        int apListStatus, const UlpLocation& gpsLoc, bool bUlpLocValid, void * /*clientData*/) {
    std::vector<LocAidlApInfo> apInfoList;
    int32_t apListSize = 0;

    LOC_LOGE("Native hidl ApList Update: AP list size %d", (int) ap_list_size);

    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }

    for (size_t ii = 0; ii <(size_t) ap_list_size; ii++) {
         LocAidlApInfo info;
         info.mac_R48b = ap_list[ii]->mac_R48b;
         info.cell_type = ap_list[ii]->cell_type;
         info.cell_id1 = ap_list[ii]->cell_id1;
         info.cell_id2 = ap_list[ii]->cell_id2;
         info.cell_id3 = ap_list[ii]->cell_id3;
         info.cell_id4 = ap_list[ii]->cell_id4;
         info.ssid = std::string(ap_list[ii]->ssid, ap_list[ii]->ssid_valid_byte_count);
         info.ssid_valid_byte_count = ap_list[ii]->ssid_valid_byte_count;
         info.utc_time = ap_list[ii]->timestamp;
         info.fdal_status = ap_list[ii]->fdal_status;
         apInfoList.push_back(info);
    }
    apListSize = (int32_t) ap_list_size;

    LocAidlUlpLocation ulpLoc;
    ulpLoc.position_source = gpsLoc.position_source;
    ulpLoc.tech_mask = gpsLoc.tech_mask;
    ulpLoc.gpsLocation.locationFlagsMask = gpsLoc.gpsLocation.flags;
    ulpLoc.gpsLocation.latitude = gpsLoc.gpsLocation.latitude;
    ulpLoc.gpsLocation.longitude = gpsLoc.gpsLocation.longitude;
    ulpLoc.gpsLocation.altitude = gpsLoc.gpsLocation.altitude;
    ulpLoc.gpsLocation.speed = gpsLoc.gpsLocation.speed;
    ulpLoc.gpsLocation.bearing = gpsLoc.gpsLocation.bearing;
    ulpLoc.gpsLocation.accuracy = gpsLoc.gpsLocation.accuracy;
    ulpLoc.gpsLocation.verticalAccuracy = gpsLoc.gpsLocation.vertUncertainity;
    ulpLoc.gpsLocation.timestamp = gpsLoc.gpsLocation.timestamp;
    ulpLoc.gpsLocation.locationTechnologyMask = ulpLoc.tech_mask;

    LOC_LOGD("wifiDbRcvrCb  obtained");
    auto result = sCallback->apListUpdateCallback(apInfoList, apListSize,
            (LocAidlWifiDBListStatus)apListStatus, ulpLoc, bUlpLocValid);
    if (result.isOk()) {
        LOC_LOGd("wifiDbRcvrCb->apListUpdateCallback success.");
    } else {
        LOC_LOGe("wifiDbRcvrCb->apListUpdateCallback failed.");
    }
}

static void DBReceiverStatusUpdateCb(bool status, const char* errStr, void* /*client data*/) {
    string reason;
    if ((status == false) && (errStr != nullptr)) {
        reason.assign(errStr);
    }

    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }
    sCallback->statusUpdateCallback(status, reason);
}

static void DBReceiverServiceRequestCb(void* /*client data*/) {

    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }
    sCallback->serviceRequestCallback();
}

static void DBReceiverEnvNotifyCb(void* /*client data*/) {

    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }
    sCallback->attachVmOnCallback();
}

LocAidlWiFiDBReceiver::LocAidlWiFiDBReceiver() : mCallbackIface(nullptr) {
    ENTRY_LOG();
    mIzatWiFiDBReceiverApiClient = new WiFiDBUpdaterWrapper(DBReceiverApListUpdateCb,
                                                            DBReceiverStatusUpdateCb,
                                                            DBReceiverServiceRequestCb,
                                                            DBReceiverEnvNotifyCb, NULL);
}

LocAidlWiFiDBReceiver::~LocAidlWiFiDBReceiver() {

    ENTRY_LOG();

    if (mIzatWiFiDBReceiverApiClient!=nullptr) {
        delete (WiFiDBUpdaterWrapper*)mIzatWiFiDBReceiverApiClient;
        mIzatWiFiDBReceiverApiClient = nullptr;
    }
}

void LocAidlWiFiDBReceiver::handleAidlClientSsr() {
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    mDeathRecipient = nullptr;
    mCallbackIface = nullptr;
}
// Methods from ::vendor::qti::gnss::ILocAidlWiFiDBReceiver follow.
::ndk::ScopedAStatus LocAidlWiFiDBReceiver::init(
        const std::shared_ptr<ILocAidlWiFiDBReceiverCallback>& callback, bool* _aidl_return) {

    FROM_AIDL_CLIENT();


    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(WiFiDBReceiverServiceDied);
    } else if (mCallbackIface != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    if (callback != nullptr) {
        mDeathRecipient->registerToPeer(callback->asBinder().get(), this);
    } else {
        LOC_LOGe("callback is nullptr!");
    }
    mCallbackIface = callback;

    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}


::ndk::ScopedAStatus LocAidlWiFiDBReceiver::registerWiFiDBUpdater(
        const std::shared_ptr<ILocAidlWiFiDBReceiverCallback>& callback) {
    FROM_AIDL_CLIENT();

    sCallback = callback;
    if (callback == nullptr) {
        LOC_LOGE("Null callback on registering null !");
    }
    mCallbackIface = callback;
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->registerToPeer(callback->asBinder().get(), this);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlWiFiDBReceiver::unregisterWiFiDBUpdater() {
    FROM_AIDL_CLIENT();
    sCallback = nullptr;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlWiFiDBReceiver::sendAPListRequest(int32_t expireInDays) {
    FROM_AIDL_CLIENT();
    // functionality not supported
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlWiFiDBReceiver::pushAPWiFiDB(
        const vector<LocAidlApLocationData>& apLocationDataList,
        int32_t apLocationDataListSize,
        const vector<LocAidlApSpecialInfo>& apSpecialInfoList,
        int32_t apSpecialInfoListSize, int32_t daysValid, bool is_lookup) {

    FROM_AIDL_CLIENT();
    apLocationDataListSize = std::min(apLocationDataListSize, (int32_t)apLocationDataList.size());
    apSpecialInfoListSize = std::min(apSpecialInfoListSize, (int32_t)apSpecialInfoList.size());
    LOC_LOGE("Native hidl ApList size %d ApSpecial size %d",
              (int) apLocationDataListSize, (int) apSpecialInfoListSize);

    std::vector<WiFiDBUpdater::APLocationData> loc_list;
    std::vector<WiFiDBUpdater::APSpecialInfo> ap_list;

    for (size_t ii = 0; ii < (size_t) apLocationDataListSize; ii++) {
        WiFiDBUpdater::APLocationData l_ad;
        l_ad.mac_R48b = apLocationDataList[ii].mac_R48b;
        l_ad.latitude = apLocationDataList[ii].latitude;
        l_ad.longitude = apLocationDataList[ii].longitude;
        l_ad.max_antena_range = apLocationDataList[ii].max_antenna_range;
        l_ad.horizontal_error = apLocationDataList[ii].horizontal_error;
        l_ad.reliability = apLocationDataList[ii].reliability;
        l_ad.valid_mask = apLocationDataList[ii].valid_bits;
        l_ad.altitude = apLocationDataList[ii].altitude;
        l_ad.altRefType = (IzatAltitudeRefType)apLocationDataList[ii].altRefType;
        l_ad.positionQuality = (ApPositionQuality)apLocationDataList[ii].positionQuality;
        l_ad.rttCapability = (WiFiRttCapability)apLocationDataList[ii].rttCapability;
        l_ad.rttRangeBiasInMm = apLocationDataList[ii].rttRangeBiasInMm;
        loc_list.push_back(l_ad);
    }

    for (size_t ii = 0; ii < (size_t) apSpecialInfoListSize; ii++) {
        WiFiDBUpdater::APSpecialInfo s_ad;
        s_ad.mac_R48b = apSpecialInfoList[ii].mac_R48b;
        s_ad.info = apSpecialInfoList[ii].info;
        ap_list.push_back(s_ad);
    }

    ((WiFiDBUpdaterWrapper*)mIzatWiFiDBReceiverApiClient)->pushWiFiDB(
            &loc_list, &ap_list, daysValid, is_lookup);
    return ndk::ScopedAStatus::ok();
}

// Methods from ::vendor::qti::gnss::ILocAidlWiFiDBReceiver follow.
::ndk::ScopedAStatus LocAidlWiFiDBReceiver::sendScanListRequest() {
    FROM_AIDL_CLIENT();
    // funcitonality not supported
    return ndk::ScopedAStatus::ok();
}

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
