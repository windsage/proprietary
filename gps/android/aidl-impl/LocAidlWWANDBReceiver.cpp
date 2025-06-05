/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_IzatWWANDBReceiver"
#define LOG_NDEBUG 0

#include "LocAidlWWANDBReceiver.h"

#include <log_util.h>
#include <gps_extended_c.h>
#include <LocationAPI.h>
#include <izat_remote_api.h>
#include <IzatRemoteApi.h>
#include <IzatTypes.h>
#include <string>
#include <wwan_db_receiver.h>

using namespace izat_remote_api;
using namespace izat_manager;

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

static std::shared_ptr<ILocAidlWWANDBReceiverCallback> sCallback = nullptr;
std::shared_ptr<LocAidlDeathRecipient> LocAidlWWANDBReceiver::mDeathRecipient = nullptr;

void WWANDBReceiverServiceDied(void* cookie) {
    LOC_LOGe("LocAidlWWANDBReceiver died.");
    auto thiz = static_cast<LocAidlWWANDBReceiver*>(cookie);
    if (nullptr != thiz) {
        thiz->handleAidlClientSsr();
        thiz = nullptr;
    }
    sCallback = nullptr;
}

LocAidlWWANDBReceiver::LocAidlWWANDBReceiver() :
        mLBSAdapter(nullptr), mCallbackIface(nullptr) {

    ENTRY_LOG();
}

LocAidlWWANDBReceiver::~LocAidlWWANDBReceiver() {

    ENTRY_LOG();
}

static void DBReceiverBsListUpdateCb(BSInfo_s *bs_list, size_t bs_list_size, uint8_t status,
        const UlpLocation &location, void * /*clientData*/) {
    std::vector<LocAidlBsInfo> bsInfoList;
    int32_t bsListSize = 0;

    LOC_LOGE("Native hidl BsList Update: BS list size %d", (int) bs_list_size);

    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }

    for (size_t i = 0; i <(size_t) bs_list_size; i++) {
         LocAidlBsInfo info;
         info.cell_type = bs_list[i].cell_type;
         info.cell_id1 = bs_list[i].cell_id1;
         info.cell_id2 = bs_list[i].cell_id2;
         info.cell_id3 = bs_list[i].cell_id3;
         info.cell_id4 = bs_list[i].cell_id4;
         info.timestamp = bs_list[i].cell_timestamp;
         bsInfoList.push_back(info);
    }
    bsListSize = (int32_t) bs_list_size;

    LocAidlUlpLocation ulpLoc;
    ulpLoc.position_source = location.position_source;
    ulpLoc.tech_mask = location.tech_mask;
    ulpLoc.gpsLocation.locationFlagsMask = location.gpsLocation.flags;
    ulpLoc.gpsLocation.latitude = location.gpsLocation.latitude;
    ulpLoc.gpsLocation.longitude = location.gpsLocation.longitude;
    ulpLoc.gpsLocation.altitude = location.gpsLocation.altitude;
    ulpLoc.gpsLocation.speed = location.gpsLocation.speed;
    ulpLoc.gpsLocation.bearing = location.gpsLocation.bearing;
    ulpLoc.gpsLocation.accuracy = location.gpsLocation.accuracy;
    ulpLoc.gpsLocation.verticalAccuracy = location.gpsLocation.vertUncertainity;
    ulpLoc.gpsLocation.timestamp = location.gpsLocation.timestamp;

    LOC_LOGD("wwanDbRcvrCb  obtained");
    sCallback->bsListUpdateCallback(bsInfoList, bsListSize, status, ulpLoc);
}

static void DBReceiverStatusUpdateCb(bool status, const char* errStr, void* /*client data*/) {
    string reason;
    if ((false == status) && (nullptr != errStr)) {
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

::ndk::ScopedAStatus LocAidlWWANDBReceiver::unregisterWWANDBUpdater() {

    FROM_AIDL_CLIENT();

    if (nullptr != mLBSAdapter) {
        mLBSAdapter->unregisterFdclCommand();
    } else {
        LOC_LOGE("mLBSAdapter is NULL");
    }
    sCallback = nullptr;

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlWWANDBReceiver::sendBSListRequest(int32_t expireInDays) {

    FROM_AIDL_CLIENT();

    if (nullptr != mLBSAdapter) {
        mLBSAdapter->requestFdclDataCommand(expireInDays);
    } else {
        LOC_LOGE("mLBSAdapter is NULL");
    }

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlWWANDBReceiver::pushBSWWANDB(
        const vector<LocAidlBsLocationData>& bsLocationDataList,
        int32_t bsLocationDataListSize,
        const vector<LocAidlBsSpecialInfo>& bsSpecialInfoList,
        int32_t bsSpecialInfoListSize, int32_t daysValid) {

    FROM_AIDL_CLIENT();

    LOC_LOGd("Native hidl BsLoc size %d %d, BsSpecial size %d %d, days valid %d",
             (int) bsLocationDataListSize, (int32_t)bsLocationDataList.size(),
             (int) bsSpecialInfoListSize, (int32_t)bsSpecialInfoList.size(),
             daysValid);

    bsLocationDataListSize = std::min(bsLocationDataListSize, (int32_t)bsLocationDataList.size());
    bsSpecialInfoListSize = std::min(bsSpecialInfoListSize, (int32_t)bsSpecialInfoList.size());

    FdclData fdclData = {};
    fdclData.daysValid = daysValid;
    for (size_t i = 0; i < (size_t) bsLocationDataListSize; i++) {
        BSLocationData_s l_ad;
        l_ad.cellType = bsLocationDataList[i].cellType;
        l_ad.cellRegionID1 = bsLocationDataList[i].cellRegionID1;
        l_ad.cellRegionID2 = bsLocationDataList[i].cellRegionID2;
        l_ad.cellRegionID3 = bsLocationDataList[i].cellRegionID3;
        l_ad.cellRegionID4 = bsLocationDataList[i].cellRegionID4;
        l_ad.latitude = bsLocationDataList[i].latitude;
        l_ad.longitude = bsLocationDataList[i].longitude;
        l_ad.valid_bits = bsLocationDataList[i].valid_bits;
        l_ad.horizontal_coverage_radius = bsLocationDataList[i].horizontal_coverage_radius;
        l_ad.horizontal_confidence = bsLocationDataList[i].horizontal_confidence;
        l_ad.horizontal_reliability = bsLocationDataList[i].horizontal_reliability;
        l_ad.altitude = bsLocationDataList[i].altitude;
        l_ad.altitude_uncertainty = bsLocationDataList[i].altitude_uncertainty;
        l_ad.altitude_confidence = bsLocationDataList[i].altitude_confidence;
        l_ad.altitude_reliability = bsLocationDataList[i].altitude_reliability;
        LOC_LOGd("cell with loc info: %u %u %u %u %u, loc info: %f %f %f",
                 l_ad.cellType, l_ad.cellRegionID1, l_ad.cellRegionID2,
                 l_ad.cellRegionID3, l_ad.cellRegionID4, l_ad.latitude,
                 l_ad.longitude, l_ad.horizontal_coverage_radius);

        fdclData.bsLocationDataList.push_back(l_ad);
    }

    for (size_t i = 0; i < (size_t) bsSpecialInfoListSize; i++) {
        BSSpecialInfo_s s_ad;
        s_ad.cellType = bsSpecialInfoList[i].cellType;
        s_ad.cellRegionID1 = bsSpecialInfoList[i].cellRegionID1;
        s_ad.cellRegionID2 = bsSpecialInfoList[i].cellRegionID2;
        s_ad.cellRegionID3 = bsSpecialInfoList[i].cellRegionID3;
        s_ad.cellRegionID4 = bsSpecialInfoList[i].cellRegionID4;
        s_ad.info = bsSpecialInfoList[i].info;

        LOC_LOGd("special cell info: %u %u %u %u %u, info %d",
                 s_ad.cellType, s_ad.cellRegionID1, s_ad.cellRegionID2,
                 s_ad.cellRegionID3, s_ad.cellRegionID4, s_ad.info);
        fdclData.bsSpecialInfoList.push_back(s_ad);
    }

    if (nullptr != mLBSAdapter) {
        mLBSAdapter->injectFdclDataCommand(fdclData);
    } else {
        LOC_LOGE("mLBSAdapter is NULL");
    }

    return ndk::ScopedAStatus::ok();
}

// Methods from ::vendor::qti::gnss::IWWANDBReceiver follow.
::ndk::ScopedAStatus LocAidlWWANDBReceiver::init(
        const std::shared_ptr<ILocAidlWWANDBReceiverCallback>& callback, bool* _aidl_return) {

    FROM_AIDL_CLIENT();

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(WWANDBReceiverServiceDied);
    }

    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    mCallbackIface = callback;
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->registerToPeer(mCallbackIface->asBinder().get(), this);
    } else {
        LOC_LOGe("callback is nullptr!");
    }

    if (nullptr == mLBSAdapter) {
        mLBSAdapter = LBSAdapter::get(LOC_API_ADAPTER_BIT_FDCL_SERVICE_REQ);
        if (nullptr == mLBSAdapter) {
            LOC_LOGE("mLBSAdapter is NULL after get");
        }
    }
    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus LocAidlWWANDBReceiver::registerWWANDBUpdater  (
        const std::shared_ptr<ILocAidlWWANDBReceiverCallback>& callback) {

    FROM_AIDL_CLIENT();

    sCallback = callback;

    void *clientData = NULL;
    if (nullptr != mLBSAdapter) {
        FdclCallbacks fdclCallbacks {DBReceiverBsListUpdateCb,
                                     DBReceiverStatusUpdateCb,
                                     DBReceiverServiceRequestCb,
                                     DBReceiverEnvNotifyCb};
        mLBSAdapter->registerFdclCommand(fdclCallbacks, clientData);
    } else {
        LOC_LOGE("mLBSAdapter is NULL");
    }

    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    mCallbackIface = callback;
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->registerToPeer(mCallbackIface->asBinder().get(), this);
    } else {
        LOC_LOGe("callback is nullptr!");
    }

    return ndk::ScopedAStatus::ok();
}

void LocAidlWWANDBReceiver::handleAidlClientSsr() {
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    mDeathRecipient = nullptr;
    mCallbackIface = nullptr;
}

// Methods from ::android::hidl::base::IBase follow.

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
