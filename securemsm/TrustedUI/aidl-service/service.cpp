/*
 * Copyright (c) 2019-2021, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "trustedui-aidl-service-qti"

#include <aidl/vendor/qti/hardware/trustedui/ITrustedInput.h>
#include <aidl/vendor/qti/hardware/trustedui/ITrustedUI.h>
#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <utils/Log.h>
#include "TrustedUIFactory.h"

using aidl::vendor::qti::hardware::trustedui::ITrustedUI;
using aidl::vendor::qti::hardware::trustedui::ITrustedInput;
using aidl::vendor::qti::hardware::trustedui::TrustedUIFactory;
using aidl::vendor::qti::hardware::trustedui::TrustedInputFactory;

int main() {
    const std::string implementation = "default";

    binder_status_t status;
    ALOGD("TrustedUI AIDL service starting...");
    {
        std::shared_ptr<ITrustedUI> service = TrustedUIFactory::GetInstance(implementation);;
        const std::string instance = std::string() + ITrustedUI::descriptor + "/" + implementation;
        status = AServiceManager_addService(service->asBinder().get(), instance.c_str());
        CHECK_EQ(status, STATUS_OK);
    }

    {
        std::shared_ptr<ITrustedInput> input_service = TrustedInputFactory::GetInstance(implementation);;
        const std::string instance_input = std::string() + ITrustedInput::descriptor + "/" + implementation;
        status = AServiceManager_addService(input_service->asBinder().get(), instance_input.c_str());
        CHECK_EQ(status, STATUS_OK);
    }

    // AIDL interfaces do not automatically start threadpools when binder objects are passed
    ABinderProcess_startThreadPool();
    ALOGI("TrustedUI AIDL Service Ready");
    ABinderProcess_joinThreadPool();
    ALOGI("TrustedUI AIDL Service Exiting");
    return EXIT_FAILURE;
}
