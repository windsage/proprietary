/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_LOCAIDLWWANDBRECEIVER_H
#define VENDOR_QTI_GNSS_LOCAIDLWWANDBRECEIVER_H

#include "LocAidlUtils.h"

#include <aidl/vendor/qti/gnss/BnLocAidlWWANDBReceiver.h>
#include <location_interface.h>
#include <LBSAdapter.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {


using ::aidl::vendor::qti::gnss::ILocAidlWWANDBReceiver;
using ::aidl::vendor::qti::gnss::ILocAidlWWANDBReceiverCallback;
using ::aidl::vendor::qti::gnss::LocAidlBsLocationData;
using ::aidl::vendor::qti::gnss::LocAidlBsSpecialInfo;
using ::aidl::vendor::qti::gnss::LocAidlUlpLocation;

using namespace std;

struct LocAidlWWANDBReceiver : public BnLocAidlWWANDBReceiver {

    LocAidlWWANDBReceiver();
    virtual ~LocAidlWWANDBReceiver();

    // Methods from ::vendor::qti::gnss::ILocAidlWWANDBReceiver follow.
    ::ndk::ScopedAStatus unregisterWWANDBUpdater() override;
    ::ndk::ScopedAStatus sendBSListRequest(int32_t expireInDays) override;
    ::ndk::ScopedAStatus pushBSWWANDB(
            const vector<LocAidlBsLocationData>& bsLocationDataList,
            int32_t bsLocationDataListSize,
            const vector<LocAidlBsSpecialInfo>& bsSpecialInfoList,
            int32_t bsSpecialInfoListSize, int32_t daysValid) override;

    // Methods from ::vendor::qti::gnss::ILocAidlWWANDBReceiver follow.
    ::ndk::ScopedAStatus init(
            const std::shared_ptr<ILocAidlWWANDBReceiverCallback>& callback,
            bool* _aidl_return) override;
    ::ndk::ScopedAStatus registerWWANDBUpdater  (
            const std::shared_ptr<ILocAidlWWANDBReceiverCallback>& callback) override;
    void handleAidlClientSsr();
private:
    LBSAdapter* mLBSAdapter = nullptr;
    std::shared_ptr<ILocAidlWWANDBReceiverCallback> mCallbackIface = nullptr;
    static std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
};

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor


#endif  // VENDOR_QTI_GNSS__LOCHIDLWWANDBRECEIVER_H
