/*
 * Copyright (c) 2018, 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/gnss/BnLocAidlIzatSubscription.h>
#include <Subscription.h>
#include "LocAidlUtils.h"
#include <IIzatManager.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {


using ::aidl::vendor::qti::gnss::ILocAidlIzatSubscriptionCallback;
using ::aidl::vendor::qti::gnss::ILocAidlIzatSubscription;
using ::aidl::vendor::qti::gnss::LocAidlSubscriptionDataItemId;
using ::aidl::vendor::qti::gnss::LocAidlBoolDataItem;

class OptinListener;
struct LocAidlIzatSubscription : public BnLocAidlIzatSubscription {
    LocAidlIzatSubscription();
    virtual ~LocAidlIzatSubscription();
    // Methods from ::vendor::qti::gnss::ILocAidlIzatSubscription follow.
    ::ndk::ScopedAStatus init(const std::shared_ptr<ILocAidlIzatSubscriptionCallback>& callback,
            bool* _aidl_return) override;
    ::ndk::ScopedAStatus deinit() override;
    ::ndk::ScopedAStatus boolDataItemUpdate(
            const std::vector<LocAidlBoolDataItem>& dataItemArray) override;
    ::ndk::ScopedAStatus stringDataItemUpdate(const LocAidlStringDataItem& dataItem) override;
    ::ndk::ScopedAStatus networkinfoUpdate(const LocAidlNetworkInfoDataItem& dataItem) override;
    ::ndk::ScopedAStatus serviceinfoUpdate(const LocAidlRilServiceInfoDataItem& dataItem) override;
    ::ndk::ScopedAStatus cellLteUpdate(const LocAidlCellLteDataItem& dataItem) override;
    ::ndk::ScopedAStatus cellGwUpdate(const LocAidlCellGwDataItem& dataItem) override;
    ::ndk::ScopedAStatus cellCdmaUpdate(const LocAidlCellCdmaDataItem& dataItem) override;
    ::ndk::ScopedAStatus cellOooUpdate(const LocAidlCellOooDataItem& dataItem) override;
    ::ndk::ScopedAStatus serviceStateUpdate(const LocAidlServiceStateDataItem& dataItem) override;
    ::ndk::ScopedAStatus screenStatusUpdate(const LocAidlScreenStatusDataItem& dataItem) override;
    ::ndk::ScopedAStatus powerConnectStatusUpdate(
            const LocAidlPowerConnectStatusDataItem& dataItem) override;
    ::ndk::ScopedAStatus timezoneChangeUpdate(
            const LocAidlTimeZoneChangeDataItem& dataItem) override;
    ::ndk::ScopedAStatus timeChangeUpdate(const LocAidlTimeChangeDataItem& dataItem) override;
    ::ndk::ScopedAStatus shutdownUpdate() override;
    ::ndk::ScopedAStatus wifiSupplicantStatusUpdate(
            const LocAidlWifiSupplicantStatusDataItem& dataItem) override;
    ::ndk::ScopedAStatus btleScanDataInject(
            const LocAidlBtLeDeviceScanDetailsDataItem& dataItem) override;
    ::ndk::ScopedAStatus btClassicScanDataInject(
            const LocAidlBtDeviceScanDetailsDataItem& dataItem) override;
    ::ndk::ScopedAStatus batteryLevelUpdate(const LocAidlBatteryLevelDataItem& dataItem) override;

private:
    struct IzatSubscriptionCallback: SubscriptionCallbackIface {

        IzatSubscriptionCallback(
                std::shared_ptr<ILocAidlIzatSubscriptionCallback> hidlCbIface) :
                    mHidlCbIface(hidlCbIface) {};
        ~IzatSubscriptionCallback() = default;
        std::shared_ptr<ILocAidlIzatSubscriptionCallback> mHidlCbIface;

        virtual void updateSubscribe(
                const std::unordered_set<DataItemId> & li, bool subscribe) override;
        virtual void requestData(const std::unordered_set<DataItemId> & li) override;
        virtual void unsubscribeAll() override;
};

private:
    std::shared_ptr<ILocAidlIzatSubscriptionCallback> mCallbackIface = nullptr;
    static std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
    IzatSubscriptionCallback* mSubscriptionCb;
    IIzatManager* mIzatManager;
    OptinListener* mListener;
};

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
