/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_IzatWWANDBProvider"
#define LOG_NDEBUG 0

#include <log_util.h>
#include <gps_extended_c.h>
#include <LocationAPI.h>
#include <izat_remote_api.h>
#include <IzatRemoteApi.h>
#include <IzatTypes.h>
#include <string>
#include "wwan_db_provider.h"
#include "LocAidlWWANDBProvider.h"
#include "LocAidlUtils.h"

using namespace izat_remote_api;
using namespace izat_manager;

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::ILocAidlWWANDBProviderCallback;
static std::shared_ptr<ILocAidlWWANDBProviderCallback> sCallback = nullptr;
std::shared_ptr<LocAidlDeathRecipient> LocAidlWWANDBProvider::mDeathRecipient = nullptr;

LocAidlWWANDBProvider::LocAidlWWANDBProvider() :
         mLBSAdapter(nullptr), mCallbackIface(nullptr) {
    ENTRY_LOG();
}

LocAidlWWANDBProvider::~LocAidlWWANDBProvider() {
    ENTRY_LOG();
}

void WWANDBProviderServiceDied(void* cookie) {
    LOC_LOGe("LocAidlWWANDBProvider died.");
    auto thiz = static_cast<LocAidlWWANDBProvider*>(cookie);
    if (nullptr != thiz) {
        thiz->handleAidlClientSsr();
        thiz = nullptr;
    }
    sCallback = nullptr;
}

// Methods from ILocAidlWWANDBProvider follow.
::ndk::ScopedAStatus LocAidlWWANDBProvider::init(
        const std::shared_ptr<ILocAidlWWANDBProviderCallback>& callback, bool* _aidl_return) {
    FROM_AIDL_CLIENT();

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(WWANDBProviderServiceDied);
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
        mLBSAdapter = LBSAdapter::get(LOC_API_ADAPTER_BIT_BS_OBS_DATA_SERVICE_REQ);
        if (nullptr == mLBSAdapter) {
            LOC_LOGE("mLBSAdapter is NULL after get");
        }
    }

    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

static void DBProvObsLocDataUpdateCb(BSObsData_s *bs_obs_data, size_t bs_obs_data_size,
        int bsListStatus, void */*clientData*/) {
    std::vector<LocAidlBSObsData> bsObsDataList;
    int32_t bsObsListSize = 0;

    LOC_LOGE("Native hidl ObsBsList Update: BS list size %d", (int) bs_obs_data_size);

    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }

    for (size_t i = 0; i <(size_t) bs_obs_data_size; i++) {
         LocAidlBSObsData obsData;

         obsData.scanTimestamp_ms = bs_obs_data[i].scanTimestamp_ms;
         obsData.bUlpLocValid = bs_obs_data[i].bUlpLocValid;
         memset(&obsData.gpsLoc, 0, sizeof(obsData.gpsLoc));
         if (obsData.bUlpLocValid) {
            obsData.gpsLoc.position_source = bs_obs_data[i].gpsLoc.position_source;
            obsData.gpsLoc.tech_mask = bs_obs_data[i].gpsLoc.tech_mask;
            obsData.gpsLoc.gpsLocation.locationFlagsMask = bs_obs_data[i].gpsLoc.gpsLocation.flags;
            obsData.gpsLoc.gpsLocation.latitude = bs_obs_data[i].gpsLoc.gpsLocation.latitude;
            obsData.gpsLoc.gpsLocation.longitude = bs_obs_data[i].gpsLoc.gpsLocation.longitude;
            obsData.gpsLoc.gpsLocation.altitude = bs_obs_data[i].gpsLoc.gpsLocation.altitude;
            obsData.gpsLoc.gpsLocation.speed = bs_obs_data[i].gpsLoc.gpsLocation.speed;
            obsData.gpsLoc.gpsLocation.bearing = bs_obs_data[i].gpsLoc.gpsLocation.bearing;
            obsData.gpsLoc.gpsLocation.speedAccuracy = bs_obs_data[i].speed_unc;
            obsData.gpsLoc.gpsLocation.bearingAccuracy = bs_obs_data[i].bearing_unc;
            obsData.gpsLoc.gpsLocation.accuracy = bs_obs_data[i].gpsLoc.gpsLocation.accuracy;
            obsData.gpsLoc.gpsLocation.verticalAccuracy =
                    bs_obs_data[i].gpsLoc.gpsLocation.vertUncertainity;
            obsData.gpsLoc.gpsLocation.timestamp = bs_obs_data[i].gpsLoc.gpsLocation.timestamp;
         }
         obsData.cellInfo.cell_type = bs_obs_data[i].cellInfo.cell_type;
         obsData.cellInfo.cell_id1 = bs_obs_data[i].cellInfo.cell_id1;
         obsData.cellInfo.cell_id2 = bs_obs_data[i].cellInfo.cell_id2;
         obsData.cellInfo.cell_id3 = bs_obs_data[i].cellInfo.cell_id3;
         obsData.cellInfo.cell_id4 = bs_obs_data[i].cellInfo.cell_id4;

         bsObsDataList.push_back(obsData);
    }
    bsObsListSize = (int32_t) bs_obs_data_size;
    sCallback->bsObsLocDataUpdateCallback(bsObsDataList, bsObsListSize, bsListStatus);
}

static void DBProvServiceRequestCb(void */*clientData*/) {
    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }
    sCallback->serviceRequestCallback();
}

static void DBProvEnvNotifyCb(void */*clientData*/) {
    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }
    sCallback->attachVmOnCallback();
}

::ndk::ScopedAStatus LocAidlWWANDBProvider::registerWWANDBProvider(
        const std::shared_ptr<ILocAidlWWANDBProviderCallback>& callback) {
    FROM_AIDL_CLIENT();

    sCallback = callback;

    void *clientData = NULL;
    if (nullptr != mLBSAdapter) {
        CellCSCallbacks cellCsCallBacks {DBProvObsLocDataUpdateCb,
                                         DBProvServiceRequestCb,
                                         DBProvEnvNotifyCb};
        mLBSAdapter->registerCellCSCommand(cellCsCallBacks, clientData);
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

::ndk::ScopedAStatus LocAidlWWANDBProvider::unregisterWWANDBProvider() {
    if (nullptr != mLBSAdapter) {
        LOC_LOGE("mLBSAdapter is not NULL");
        mLBSAdapter->unregisterCellCSCommand();
    } else {
        LOC_LOGE("mLBSAdapter is NULL");
    }
    sCallback = nullptr;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlWWANDBProvider::sendBSObsLocDataRequest() {
    FROM_AIDL_CLIENT();

    if (nullptr != mLBSAdapter) {
        LOC_LOGE("mLBSAdapter is not NULL");
        mLBSAdapter->requestCellCSDataCommand();
    } else {
        LOC_LOGE("mLBSAdapter is NULL");
    }

    return ndk::ScopedAStatus::ok();
}

void LocAidlWWANDBProvider::handleAidlClientSsr() {
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    mDeathRecipient = nullptr;
    mCallbackIface = nullptr;
}

// Methods from ::android::hidl::base::IBase follow.

//ILocAidlWWANDBProvider* HIDL_FETCH_ILocAidlWWANDBProvider(const char* /* name */) {
//    return new LocAidlWWANDBProvider();
//}

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
