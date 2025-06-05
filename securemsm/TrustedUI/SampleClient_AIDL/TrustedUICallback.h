/*
 * Copyright (c) 2019, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TRUSTEDUICALLBACK_H
#define TRUSTEDUICALLBACK_H

#include <aidl/vendor/qti/hardware/trustedui/BnTrustedUICallback.h>


namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace trustedui {
namespace implementation {

using ::aidl::vendor::qti::hardware::trustedui::ITrustedUICallback;

class TrustedUICallback : public BnTrustedUICallback {
    public:
    TrustedUICallback() = default;
    virtual ~TrustedUICallback() = default;

    ::ndk::ScopedAStatus onComplete() override;
    ::ndk::ScopedAStatus onError() override;

};

}  // namespace implementation
}  // namespace trustedui
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif
