/******************************************************************************
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *****************************************************************************/

#ifndef TRUSTEDUI_FACTORY_H
#define TRUSTEDUI_FACTORY_H

#include <string>
#include <aidl/vendor/qti/hardware/trustedui/ITrustedUI.h>
#include <aidl/vendor/qti/hardware/trustedui/ITrustedInput.h>

using namespace std;

using ::aidl::vendor::qti::hardware::trustedui::ITrustedUI;
using ::aidl::vendor::qti::hardware::trustedui::ITrustedInput;

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace trustedui {

struct TrustedUIFactory {
    static std::shared_ptr<ITrustedUI> GetInstance(string tuiImplInstance);
};

struct TrustedInputFactory {
    static std::shared_ptr<ITrustedInput> GetInstance(string tuiInputImplInstance);
};

}  // namespace trustedui
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif
