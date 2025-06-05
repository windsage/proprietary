/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <aidl/vendor/qti/gnss/BnLocAidlGnssNi.h>
#include "LocAidlNiClient.h"
#include <memory>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::ILocAidlGnssNi;
using ::aidl::vendor::qti::gnss::ILocAidlGnssNiCallback;
using ::aidl::vendor::qti::gnss::LocAidlGnssUserResponseType;

struct LocAidlGnssNi : public BnLocAidlGnssNi {
    LocAidlGnssNi();
    virtual ~LocAidlGnssNi();
    ::ndk::ScopedAStatus respond(int32_t notifId,
            LocAidlGnssUserResponseType userResponse) override;

    // Methods from ::vendor::qti::gnss::ILocAidlGnssNi follow.
    ::ndk::ScopedAStatus setVendorCallback(
            const std::shared_ptr<ILocAidlGnssNiCallback>& callback) override;

    void handleAidlClientSsr();
private:
    LocAidlNiClient* mNiClient;
    std::shared_ptr<ILocAidlGnssNiCallback> mCallbackIface = nullptr;
    std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
};

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
