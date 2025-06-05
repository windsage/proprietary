/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_IzatSubscription"
#define LOG_NDEBUG 0

#include <log_util.h>
#include <Subscription.h>
#include <DataItemConcreteTypes.h>
#include <DataItemConcreteTypesExt.h>
#include <FrameworkActionRequest.h>
#include <loc_ril.h>
#include <WiperData.h>
#include <IDataItemCore.h>
#include "loc_misc_utils.h"
#include "LocAidlIzatSubscription.h"
#include <OSFramework.h>
using namespace izat_manager;

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::LocAidlSubscriptionDataItemId;
using namespace loc_core;
#define ALL_DATA_ITEM_MASK ((uint64_t)(1 << MAX_DATA_ITEM_ID_1_1) - 1)\
        & (~(1 << RILVERSION_DATA_ITEM_ID | 1 << RILSERVICEINFO_DATA_ITEM_ID  |\
                1 << RILCELLINFO_DATA_ITEM_ID | 1 << SERVICESTATUS_DATA_ITEM_ID))
static bool sSubscribe[MAX_DATA_ITEM_ID];
std::shared_ptr<LocAidlDeathRecipient> LocAidlIzatSubscription::mDeathRecipient = nullptr;

void IzatSubscriptionServiceDied(void* cookie) {
    LOC_LOGe("ILocAidlIzatSubscriptionCallback died.");
    auto thiz = static_cast<LocAidlIzatSubscription*>(cookie);
    if (nullptr != thiz) {
        thiz->deinit();
    }
}

class OptinListener : public IOSListener {
    std::shared_ptr<ILocAidlIzatSubscriptionCallback> mAidlCbIface;
    IzatStreamType mStreamType;
    public:
    inline OptinListener(const std::shared_ptr<ILocAidlIzatSubscriptionCallback>& aidlCbIface,
            IzatStreamType streamType) :
        IOSListener(), mAidlCbIface(aidlCbIface),
        mStreamType(streamType) {}
    inline ~OptinListener() {}

    inline virtual IzatListenerMask listensTo() const override {
        return mStreamType;
    }

    inline std::shared_ptr<ILocAidlIzatSubscriptionCallback>& getAidlCbIface() {
        return mAidlCbIface;
    }

    inline virtual void onLocationChanged(
            const izat_manager::IzatLocation* location, int locCnt,
            const izat_manager::IzatLocationStatus) override {};

    virtual void onStatusChanged(
            const izat_manager::IzatProviderStatus status) override;

    inline virtual void onNmeaChanged(const IzatNmea*) override {}
};

void OptinListener::onStatusChanged (
        const izat_manager::IzatProviderStatus status) {
    if (mAidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }
    TO_AIDL_CLIENT();
    bool optin = (IZAT_PROVIDER_OPTIN_ENABLED == status) ? true : false;
    LOC_LOGd("optin enabled: %s", optin ? "true" : "false");
    std::vector<LocAidlBoolDataItem> optInDataItemUpdate = {{ENH_DATA_ITEM_ID, optin}};
    mAidlCbIface->boolDataItemUpdate(optInDataItemUpdate);
}

void LocAidlIzatSubscription::IzatSubscriptionCallback::updateSubscribe(
        const std::unordered_set<DataItemId>& li, bool subscribe) {

    ENTRY_LOG();

    for (auto item : li) {
        if (INVALID_DATA_ITEM_ID < item && item < MAX_DATA_ITEM_ID) {
            sSubscribe[item] = subscribe;
        }
    }

    if (mHidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }

    // Create a vector with all data items
    std::vector<LocAidlSubscriptionDataItemId> dataItemVec;
    dataItemVec.resize(li.size());
    int idx = 0;
    for (std::unordered_set<DataItemId>::const_iterator it = li.begin();
            it != li.end(); it++, idx++) {

        DataItemId id = *it;
        LocAidlSubscriptionDataItemId sid =
                LocAidlUtils::translateToSubscriptionDataItemId(id);
        dataItemVec[idx] = (LocAidlSubscriptionDataItemId)sid;
    }

    TO_AIDL_CLIENT();
    mHidlCbIface->updateSubscribe(dataItemVec, subscribe);
}

void LocAidlIzatSubscription::IzatSubscriptionCallback::requestData(
        const std::unordered_set<DataItemId>& li) {

    ENTRY_LOG();

    if (mHidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }

    // Create a vector with all data items
    std::vector<LocAidlSubscriptionDataItemId> dataItemVec;
    dataItemVec.resize(li.size());
    int idx = 0;
    for (std::unordered_set<DataItemId>::const_iterator it = li.begin();
            it != li.end(); it++, idx++) {

        DataItemId id = *it;
        LocAidlSubscriptionDataItemId sid =
                LocAidlUtils::translateToSubscriptionDataItemId(id);
        dataItemVec[idx] = (LocAidlSubscriptionDataItemId)sid;
    }

    TO_AIDL_CLIENT();
    mHidlCbIface->requestData(dataItemVec);
}

void LocAidlIzatSubscription::IzatSubscriptionCallback::unsubscribeAll() {

    ENTRY_LOG();

    if (mHidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }

    TO_AIDL_CLIENT();
    mHidlCbIface->unsubscribeAll();
}

LocAidlIzatSubscription::LocAidlIzatSubscription() :
        mCallbackIface(nullptr), mSubscriptionCb(NULL),
        mIzatManager(getIzatManager(OSFramework::getOSFramework())), mListener(NULL) {

    ENTRY_LOG();
    for (int i = 0; i < MAX_DATA_ITEM_ID; i++) {
        sSubscribe[i] = false;
    }
}

LocAidlIzatSubscription::~LocAidlIzatSubscription() {
    if (mSubscriptionCb != nullptr) {
        delete mSubscriptionCb;
        mSubscriptionCb = nullptr;
    }
}

// Methods from ::vendor::qti::gnss::ILocAidlIzatSubscription follow.
::ndk::ScopedAStatus LocAidlIzatSubscription::init(
        const std::shared_ptr<ILocAidlIzatSubscriptionCallback>& callback, bool* _aidl_return) {
    FROM_AIDL_CLIENT();
    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(IzatSubscriptionServiceDied);
    } else if (mCallbackIface != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    if (callback != nullptr) {
        mDeathRecipient->registerToPeer(callback->asBinder().get(), this);
    } else {
        LOC_LOGe("callback is nullptr!");
    }
    mCallbackIface = callback;
    if (mIzatManager) {
        mListener = new OptinListener(mCallbackIface, IZAT_STREAM_OPTIN_STATUS);
        mIzatManager->subscribeListener(mListener);
    }
    mSubscriptionCb =
            new LocAidlIzatSubscription::IzatSubscriptionCallback(callback);
    Subscription::setSubscriptionCallback(mSubscriptionCb, ALL_DATA_ITEM_MASK);

    // create the subscription object.
    (void)Subscription::getSubscriptionObj();

    // create the framework action request object
    (void)FrameworkActionRequest::getFrameworkActionReqObj();

    std::unordered_set<DataItemId> li;
    for (int i = INVALID_DATA_ITEM_ID + 1; i < MAX_DATA_ITEM_ID; i++) {
        if (sSubscribe[i]) {
            li.insert((DataItemId(i)));
        }
    }
    if (!li.empty()) {
        LOC_LOGV("resubscribe");
        mSubscriptionCb->updateSubscribe(li, true);
    }

    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::deinit() {
    FROM_AIDL_CLIENT();
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
        mCallbackIface = nullptr;
        mDeathRecipient = nullptr;
    }
    auto listener = mListener;
    if (mListener && mIzatManager) {
        mIzatManager->unsubscribeListener(listener);
        mListener = NULL;
    }
    // reset SubscriptionCallback
    Subscription::setSubscriptionCallback(mSubscriptionCb, 0);
    if (mSubscriptionCb != nullptr) {
        delete mSubscriptionCb;
        mSubscriptionCb = nullptr;
    }

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::boolDataItemUpdate(
        const std::vector<LocAidlBoolDataItem>& dataItemArray) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    ENHDataItem enh_data_item(false, ENHDataItem::FIELD_CONSENT);
    AirplaneModeDataItem airplaneMode_data_item;
    GPSStateDataItem gpsState_data_item;
    NLPStatusDataItem nlpStatus_data_item;
    WifiHardwareStateDataItem wifistate_data_item;
    AssistedGpsDataItem assistedgps_data_item;

    std::unordered_set<IDataItemCore *> dataItemSet;

    for (int idx = 0; idx < (int)dataItemArray.size(); idx++) {
        switch (dataItemArray[idx].id)
        {
            case ENH_DATA_ITEM_ID:
                enh_data_item.setAction(dataItemArray[idx].enabled ? ENHDataItem::SET :
                    ENHDataItem::CLEAR);
                dataItemSet.insert(&enh_data_item);
                break;
            case AIRPLANEMODE_DATA_ITEM_ID:
                airplaneMode_data_item.mMode = dataItemArray[idx].enabled;
                dataItemSet.insert(&airplaneMode_data_item);
                break;
            case NLPSTATUS_DATA_ITEM_ID:
                nlpStatus_data_item.mEnabled = dataItemArray[idx].enabled;
                dataItemSet.insert(&nlpStatus_data_item);
                break;
            case WIFIHARDWARESTATE_DATA_ITEM_ID:
                wifistate_data_item.mEnabled = dataItemArray[idx].enabled;
                dataItemSet.insert(&wifistate_data_item);
                break;
            case ASSISTED_GPS_DATA_ITEM_ID:
                assistedgps_data_item.mEnabled = dataItemArray[idx].enabled;
                dataItemSet.insert(&assistedgps_data_item);
                break;
        }
    }

    if (dataItemSet.size() != 0) {
        Subscription::mObserverObj->notify(dataItemSet);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::stringDataItemUpdate(
        const LocAidlStringDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    IDataItemCore *dataItem = NULL;

    ModelDataItem modeldesc_data_item;
    ManufacturerDataItem manufacturerdesc_data_item;
    TacDataItem tacdesc_data_item;
    MccmncDataItem mccmncdesc_data_item;

    switch (di.id)
    {
        case MODEL_DATA_ITEM_ID:
            modeldesc_data_item.mModel.assign(di.str.c_str());
            dataItem = &modeldesc_data_item;
            break;
        case MANUFACTURER_DATA_ITEM_ID:
            manufacturerdesc_data_item.mManufacturer.assign(di.str.c_str());
            dataItem = &manufacturerdesc_data_item;
            break;
        case TAC_DATA_ITEM_ID:
            tacdesc_data_item.mValue.assign(di.str.c_str());
            dataItem = &tacdesc_data_item;
            break;
        case MCCMNC_DATA_ITEM_ID:
            mccmncdesc_data_item.mValue.assign(di.str.c_str());
            dataItem = &mccmncdesc_data_item;
            break;
    }

    if (dataItem != NULL) {
        std::unordered_set<IDataItemCore *> dataItemSet;
        dataItemSet.insert(dataItem);
        Subscription::mObserverObj->notify(dataItemSet);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::networkinfoUpdate(
        const LocAidlNetworkInfoDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    loc_core::NetworkInfoDataItem networkinfo_data_item;
    IDataItemCore *dataItem = NULL;

    networkinfo_data_item.mType = di.type;
    loc_core::NetworkType type = loc_core::NetworkInfoDataItem::getNormalizedType(di.type);
    networkinfo_data_item.mAllTypes = (type >= TYPE_UNKNOWN || type < TYPE_MOBILE)?  0:(1<<type);
    networkinfo_data_item.mAvailable = di.available;
    networkinfo_data_item.mConnected = di.connected;
    networkinfo_data_item.mRoaming = di.roaming;

    if (di.typeName.size() > 0) {
        networkinfo_data_item.mTypeName.assign(di.typeName.c_str());
    }

    if (di.subTypeName.size() > 0) {
        networkinfo_data_item.mSubTypeName.assign(di.subTypeName.c_str());
    }

    dataItem = &networkinfo_data_item;

    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(dataItem);
    Subscription::mObserverObj->notify(dataItemSet);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::serviceinfoUpdate(
        const LocAidlRilServiceInfoDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    LOC_RilServiceInfo serviceInfo;
    memset(&serviceInfo, 0, sizeof(LOC_RilServiceInfo));

    switch (di.type)
    {
        case LOC_RILAIRIF_CDMA:
            serviceInfo.valid_mask = LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE;
            serviceInfo.carrierAirIf_type = LOC_RILAIRIF_CDMA;

            if (di.name.size() > 0) {
                serviceInfo.valid_mask |= LOC_RIL_SERVICE_INFO_HAS_CARRIER_NAME;

                if (strlcpy(serviceInfo.carrierName, di.name.c_str(),
                            sizeof(serviceInfo.carrierName))
                        >= sizeof(serviceInfo.carrierName)) {
                    LOC_LOGD("Carrier name is longer than array size to hold it.");
                }
            }
            break;

        case LOC_RILAIRIF_GSM:
            serviceInfo.valid_mask = LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE;
            serviceInfo.carrierAirIf_type = LOC_RILAIRIF_GSM;

            // MCC cannot be zero
            if (di.mcc != 0) {
                serviceInfo.valid_mask |= LOC_RIL_SERVICE_INFO_HAS_CARRIER_MCC;
                serviceInfo.carrierMcc = di.mcc;
            }

            // Mnc can be 0
            serviceInfo.valid_mask |= LOC_RIL_SERVICE_INFO_HAS_CARRIER_MNC;
            serviceInfo.carrierMnc = di.mnc;
            break;
    }

    RilServiceInfoDataItemExt serviceinfo_data_item(&serviceInfo);
    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(&serviceinfo_data_item);
    Subscription::mObserverObj->notify(dataItemSet);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::cellLteUpdate(
        const LocAidlCellLteDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    LOC_RilCellInfo cellInfo;
    memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

    cellInfo.valid_mask = di.mask;
    cellInfo.nwStatus = (LOC_NWstatus)di.status;
    cellInfo.rtType = (LOC_RilTechType)di.type;

    cellInfo.u.lteCinfo.mcc = di.mcc;
    cellInfo.u.lteCinfo.mnc = di.mnc;
    cellInfo.u.lteCinfo.pci = di.pci;
    cellInfo.u.lteCinfo.tac = di.tac;
    cellInfo.u.lteCinfo.cid = di.cid;

    cellInfo.valid_mask |= LOC_RIL_CELLINFO_HAS_CELL_INFO;

    RilCellInfoDataItemExt cellinfo_data_item(&cellInfo);
    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(&cellinfo_data_item);
    Subscription::mObserverObj->notify(dataItemSet);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::cellGwUpdate(
        const LocAidlCellGwDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    LOC_RilCellInfo cellInfo;
    memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

    cellInfo.valid_mask = di.mask;
    cellInfo.nwStatus = (LOC_NWstatus)di.status;
    cellInfo.rtType = (LOC_RilTechType)di.type;

    switch (di.type)
    {
        case LOC_RILAIRIF_GSM:
            cellInfo.u.gsmCinfo.mcc = di.mcc;
            cellInfo.u.gsmCinfo.mnc = di.mnc;
            cellInfo.u.gsmCinfo.lac = di.lac;
            cellInfo.u.gsmCinfo.cid = di.cid;
            break;
        case LOC_RILAIRIF_WCDMA:
            cellInfo.u.wcdmaCinfo.mcc = di.mcc;
            cellInfo.u.wcdmaCinfo.mnc = di.mnc;
            cellInfo.u.wcdmaCinfo.lac = di.lac;
            cellInfo.u.wcdmaCinfo.cid = di.cid;
            break;
        default:
            LOC_LOGE("Invalid interface type %d", di.type);
            return ndk::ScopedAStatus::ok();
    }

    cellInfo.valid_mask |= LOC_RIL_CELLINFO_HAS_CELL_INFO;

    RilCellInfoDataItemExt cellinfo_data_item(&cellInfo);
    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(&cellinfo_data_item);

    Subscription::mObserverObj->notify(dataItemSet);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::cellCdmaUpdate(
        const LocAidlCellCdmaDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    LOC_RilCellInfo cellInfo;
    memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

    cellInfo.valid_mask = di.mask;
    cellInfo.nwStatus = (LOC_NWstatus)di.status;
    cellInfo.rtType = (LOC_RilTechType)di.type;

    cellInfo.u.cdmaCinfo.sid = di.sid;
    cellInfo.u.cdmaCinfo.nid = di.nid;
    cellInfo.u.cdmaCinfo.bsid = di.bsid;
    cellInfo.u.cdmaCinfo.bslat = di.bslat;
    cellInfo.u.cdmaCinfo.bslon = di.bslong;
    cellInfo.u.cdmaCinfo.local_timezone_offset_from_utc = di.timeOffset;
    cellInfo.u.cdmaCinfo.local_timezone_on_daylight_savings = di.inDST;

    cellInfo.valid_mask |= LOC_RIL_CELLINFO_HAS_CELL_INFO;

    RilCellInfoDataItemExt cellinfo_data_item(&cellInfo);
    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(&cellinfo_data_item);
    Subscription::mObserverObj->notify(dataItemSet);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::cellOooUpdate(
        const LocAidlCellOooDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    LOC_RilCellInfo cellInfo;
    memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

    cellInfo.valid_mask = di.mask;
    cellInfo.nwStatus = (LOC_NWstatus)di.status;

    RilCellInfoDataItemExt cellinfo_data_item(&cellInfo);

    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(&cellinfo_data_item);

    Subscription::mObserverObj->notify(dataItemSet);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::serviceStateUpdate(
        const LocAidlServiceStateDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    ServiceStatusDataItem service_status_data_item;
    service_status_data_item.mServiceState = (LOC_NWstatus)di.status;

    IDataItemCore *dataItem = &service_status_data_item;

    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(dataItem);

    Subscription::mObserverObj->notify(dataItemSet);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::screenStatusUpdate(
        const LocAidlScreenStatusDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    ScreenStateDataItem screenstate_data_item;
    screenstate_data_item.mState = di.status;

    IDataItemCore *dataItem = &screenstate_data_item;

    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(dataItem);

    Subscription::mObserverObj->notify(dataItemSet);

    return ndk::ScopedAStatus::ok();
}

// Deprecated
::ndk::ScopedAStatus LocAidlIzatSubscription::powerConnectStatusUpdate(
        const LocAidlPowerConnectStatusDataItem& di) {

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::timezoneChangeUpdate(
        const LocAidlTimeZoneChangeDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    loc_core::TimeZoneChangeDataItem timezonechange_data_item(di.curTimeMillis,
            di.rawOffset, di.dstOffset);

    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(&timezonechange_data_item);

    Subscription::mObserverObj->notify(dataItemSet);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::timeChangeUpdate(
        const LocAidlTimeChangeDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    loc_core::TimeChangeDataItem timechange_data_item(di.curTimeMillis,
            di.rawOffset, di.dstOffset);

    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(&timechange_data_item);

    Subscription::mObserverObj->notify(dataItemSet);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::shutdownUpdate() {
    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    ShutdownStateDataItem shutdown_data_item(true);

    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(&shutdown_data_item);

    Subscription::mObserverObj->notify(dataItemSet);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatSubscription::wifiSupplicantStatusUpdate(
        const LocAidlWifiSupplicantStatusDataItem& di) {

    FROM_AIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return ndk::ScopedAStatus::ok();
    }

    loc_core::WifiSupplicantStatusDataItem wifisupplicantstatus_data_item;

    wifisupplicantstatus_data_item.mState =
        (loc_core::WifiSupplicantStatusDataItem::WifiSupplicantState)di.state;
    wifisupplicantstatus_data_item.mApMacAddressValid = di.apMacAddressValid;
    if ((di.apMacAddress.size() > 0) &&
            (wifisupplicantstatus_data_item.mApMacAddressValid == 1)) {
        for (int i=0; i<MAC_ADDRESS_LENGTH; i++) {
            wifisupplicantstatus_data_item.mApMacAddress[i] = di.apMacAddress[i];
        }
    }

    wifisupplicantstatus_data_item.mWifiApSsidValid = di.apSsidValid;
    wifisupplicantstatus_data_item.mWifiApSsid = di.apSsid;

    IDataItemCore *dataItem = &wifisupplicantstatus_data_item;

    std::unordered_set<IDataItemCore *> dataItemSet;
    dataItemSet.insert(dataItem);

    Subscription::mObserverObj->notify(dataItemSet);
    return ndk::ScopedAStatus::ok();
}
// Deprecated
::ndk::ScopedAStatus LocAidlIzatSubscription::btleScanDataInject(
        const LocAidlBtLeDeviceScanDetailsDataItem& di) {
    return ndk::ScopedAStatus::ok();
}

// Deprecated
::ndk::ScopedAStatus LocAidlIzatSubscription::btClassicScanDataInject(
        const LocAidlBtDeviceScanDetailsDataItem& di) {
    return ndk::ScopedAStatus::ok();
}

// Deprecated
::ndk::ScopedAStatus LocAidlIzatSubscription::batteryLevelUpdate(
        const LocAidlBatteryLevelDataItem& di) {
    return ndk::ScopedAStatus::ok();
}

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
