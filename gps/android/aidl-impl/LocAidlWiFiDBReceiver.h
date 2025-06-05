/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_LOCAIDLWIFIDBRECEIVER_H
#define VENDOR_QTI_GNSS_LOCAIDLWIFIDBRECEIVER_H

#include "LocAidlUtils.h"

#include <aidl/vendor/qti/gnss/BnLocAidlWiFiDBReceiver.h>
#include <aidl/vendor/qti/gnss/ILocAidlWiFiDBReceiverCallback.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {


using ::aidl::vendor::qti::gnss::ILocAidlWiFiDBReceiver;
using ::aidl::vendor::qti::gnss::ILocAidlWiFiDBReceiverCallback;
using ::aidl::vendor::qti::gnss::LocAidlApLocationData;
using ::aidl::vendor::qti::gnss::LocAidlApSpecialInfo;
using ::aidl::vendor::qti::gnss::LocAidlUlpLocation;
using ::aidl::vendor::qti::gnss::LocAidlWifiDBListStatus;
using ::aidl::vendor::qti::gnss::LocAidlApInfo;

using namespace std;
class WiFiDBUpdaterWrapper;
struct LocAidlWiFiDBReceiver : public BnLocAidlWiFiDBReceiver {
    LocAidlWiFiDBReceiver();
    virtual ~LocAidlWiFiDBReceiver();
    // Methods from ::vendor::qti::gnss::ILocAidlWiFiDBReceiver follow.
    ::ndk::ScopedAStatus init(
            const std::shared_ptr<ILocAidlWiFiDBReceiverCallback>& callback,
            bool* _aidl_return) override;
    ::ndk::ScopedAStatus registerWiFiDBUpdater(
            const std::shared_ptr<ILocAidlWiFiDBReceiverCallback>& callback) override;
    ::ndk::ScopedAStatus unregisterWiFiDBUpdater() override;
    ::ndk::ScopedAStatus sendAPListRequest(int32_t expireInDays) override;
    // Methods from ::vendor::qti::gnss::ILocAidlWiFiDBReceiver follow.
    ::ndk::ScopedAStatus sendScanListRequest() override;
    ::ndk::ScopedAStatus pushAPWiFiDB(
            const vector<LocAidlApLocationData>& apLocationDataList,
            int32_t apLocationDataListSize,
            const vector<LocAidlApSpecialInfo>& apSpecialInfoList,
            int32_t apSpecialInfoListSize, int32_t daysValid, bool is_lookup) override;

    void handleAidlClientSsr();
private:
    WiFiDBUpdaterWrapper* mIzatWiFiDBReceiverApiClient = nullptr;
    std::shared_ptr<ILocAidlWiFiDBReceiverCallback> mCallbackIface = nullptr;
    static std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
};

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif  // VENDOR_QTI_GNSS__LOCHIDLWIFIDBRECEIVER_H
