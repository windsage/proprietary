/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_GnssNi"
#define LOG_NDEBUG 0

#include <log_util.h>
#include "LocAidlGnssNi.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

void GnssNiServiceDied(void* cookie) {
    LOC_LOGe("ILocAidlGnssNiCallback died.");
    auto thiz = static_cast<LocAidlGnssNi*>(cookie);
    if (nullptr != thiz) {
        thiz->handleAidlClientSsr();
        thiz = nullptr;
    }
}

LocAidlGnssNi::LocAidlGnssNi() : mNiClient(LocAidlNiClient::getInstance()),
    mCallbackIface(nullptr) {
    ENTRY_LOG();
}

LocAidlGnssNi::~LocAidlGnssNi() {
    handleAidlClientSsr();
}
void LocAidlGnssNi::handleAidlClientSsr() {
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    mDeathRecipient = nullptr;
    mCallbackIface = nullptr;
}

// Methods from ::vendor::qti::gnss::ILocAidlGnssNi follow.
::ndk::ScopedAStatus LocAidlGnssNi::setVendorCallback(
        const std::shared_ptr<ILocAidlGnssNiCallback>& callback) {
    FROM_AIDL_CLIENT();

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(GnssNiServiceDied);
    } else if (mCallbackIface != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    mCallbackIface = callback;
    if (callback != nullptr) {
        mDeathRecipient->registerToPeer(callback->asBinder().get(), this);
    } else {
        LOC_LOGe("callback is nullptr!");
    }

    if (mNiClient != nullptr) {
        mNiClient->setCallback(mCallbackIface);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnssNi::respond(int32_t notifId,
        LocAidlGnssUserResponseType userResponse) {

    FROM_AIDL_CLIENT();
    if (mNiClient != nullptr) {
        mNiClient->gnssNiRespond(notifId, userResponse);
    }
    return ndk::ScopedAStatus::ok();
}


// Methods from ::android::hidl::base::IBase follow.

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
