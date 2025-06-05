/******************************************************************************
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *****************************************************************************/

#include <string>
#include <utils/Log.h>
#include "TouchInputVM.h"
#include "TrustedUIFactory.h"

using namespace std;

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace trustedui {

using ::aidl::vendor::qti::hardware::trustedui::ITrustedInput;

std::shared_ptr<ITrustedInput> TrustedInputFactory::GetInstance(std::string tuiInputImplInstance) {
    std::shared_ptr<ITrustedInput> input_service = nullptr;
    if (tuiInputImplInstance.compare("default") == 0) {
        if (input_service == nullptr) {
            input_service = ndk::SharedRefBase::make<TouchInputVM>();
        }
        return input_service;
    }
    ALOGE("No instance registered with this interface: %s", tuiInputImplInstance.c_str());
    return nullptr;
}

}  // namespace trustedui
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
