/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/gnss/BnLocAidlWWANDBProvider.h>
#include <location_interface.h>
#include "LBSAdapter.h"
#include "LocAidlUtils.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::ILocAidlWWANDBProvider;
using ::aidl::vendor::qti::gnss::ILocAidlWWANDBProviderCallback;
using ::aidl::vendor::qti::gnss::LocAidlBSObsData;

struct LocAidlWWANDBProvider : public BnLocAidlWWANDBProvider {
    LocAidlWWANDBProvider();
    ~LocAidlWWANDBProvider();
    // Methods from ILocAidlWWANDBProvider follow.
    ::ndk::ScopedAStatus init(
            const std::shared_ptr<ILocAidlWWANDBProviderCallback>& callback,
            bool* _aidl_return) override;
    ::ndk::ScopedAStatus registerWWANDBProvider(
            const std::shared_ptr<ILocAidlWWANDBProviderCallback>& callback) override;
    ::ndk::ScopedAStatus unregisterWWANDBProvider() override;
    ::ndk::ScopedAStatus sendBSObsLocDataRequest() override;

    void handleAidlClientSsr();
    // Methods from ::android::hidl::base::IBase follow.
private:
    LBSAdapter* mLBSAdapter = nullptr;
    std::shared_ptr<ILocAidlWWANDBProviderCallback> mCallbackIface = nullptr;
    static std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
};

extern "C" ILocAidlWWANDBProvider* HIDL_FETCH_ILocAidlWWANDBProvider(const char* name);

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
