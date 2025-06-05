/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/gnss/BnLocAidlWiFiDBProvider.h>
#include <aidl/vendor/qti/gnss/LocAidlApObsData.h>
#include <aidl/vendor/qti/gnss/LocAidlApScanData.h>
#include "LocAidlUtils.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::ILocAidlWiFiDBProvider;
using ::aidl::vendor::qti::gnss::ILocAidlWiFiDBProviderCallback;
using ::aidl::vendor::qti::gnss::LocAidlApObsData;
using ::aidl::vendor::qti::gnss::LocAidlApScanData;
using ::aidl::vendor::qti::gnss::LocAidlRangingBandWidth;
using ::aidl::vendor::qti::gnss::LocAidlApRangingData;

struct LocAidlWiFiDBProvider : public BnLocAidlWiFiDBProvider {
    LocAidlWiFiDBProvider();
    ~LocAidlWiFiDBProvider();
    // Methods from ILocAidlWiFiDBProvider follow.
    ::ndk::ScopedAStatus init(const std::shared_ptr<ILocAidlWiFiDBProviderCallback>& callback,
            bool* _aidl_return) override;
    ::ndk::ScopedAStatus registerWiFiDBProvider(
            const std::shared_ptr<ILocAidlWiFiDBProviderCallback>& callback) override;
    ::ndk::ScopedAStatus unregisterWiFiDBProvider() override;
    ::ndk::ScopedAStatus sendAPObsLocDataRequest() override;

    void handleAidlClientSsr();
    // Methods from ::android::hidl::base::IBase follow.

private:
    void* mIzatWiFiDBProviderApiClient = nullptr;
    std::shared_ptr<ILocAidlWiFiDBProviderCallback> mCallbackIface = nullptr;
    static std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
};

//extern "C" ILocAidlWiFiDBProvider* HIDL_FETCH_ILocAidlWiFiDBProvider(const char* name);

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
