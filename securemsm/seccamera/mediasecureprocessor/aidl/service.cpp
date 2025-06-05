/**
 * Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#define LOG_TAG "SECURE-PROCESSOR"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include "SecureProcessorQTEEMink.h"
#include "SecureProcessorTVMMink.h"

using ::aidl::vendor::qti::hardware::secureprocessor::device::SecureProcessorQTEEMink;
using ::aidl::vendor::qti::hardware::secureprocessor::device::SecureProcessorTVMMink;

using ::aidl::vendor::qti::hardware::secureprocessor::device::TYPE_QTI_TEE;
using ::aidl::vendor::qti::hardware::secureprocessor::device::TYPE_QTI_TVM;

int32_t main()
{
    ALOGD("Starting qti-tee AIDL service instance");
    /*
     * Attempt to create TEE Device to communicate with Trusted App
     * use a factory method to create multiple implementation.
     */
    std::shared_ptr<SecureProcessorQTEEMink> destinationTEE =
        ndk::SharedRefBase::make<SecureProcessorQTEEMink>("seccamdemo2");

    const std::string instanceTEE =
        std::string() + SecureProcessorQTEEMink::descriptor + "/qti-tee";

    /*
     * Attempt to create TVM Device to communicate with TVM App
     * use a factory method to create multiple implementation.
     */
    std::shared_ptr<SecureProcessorTVMMink> destinationTVM =
        ndk::SharedRefBase::make<SecureProcessorTVMMink>("seccamdemo25");

    const std::string instanceTVM =
        std::string() + SecureProcessorTVMMink::descriptor + "/qti-tvm";

    binder_status_t status;

    if (destinationTEE) {
        status = AServiceManager_addService(
            destinationTEE->asBinder().get(), instanceTEE.c_str());
        ALOGD("register qti-tee AIDL service instance %d", status);
        if (status != 0) {
            ALOGE("Failed to register qti-tee service instance %d", status);
        }
    } else {
        ALOGE("Failed to create qti-tee service instance");
    }
    CHECK_EQ(status, STATUS_OK);

    if (destinationTVM) {
        status = AServiceManager_addService(
            destinationTVM->asBinder().get(), instanceTVM.c_str());
            ALOGD("register qti-tvm AIDL service instance %d", status);
        if (status != 0) {
            ALOGE("Failed to register qti-tvm service instance%d", status);
        }
    } else {
        ALOGE("Failed to create qti-tvm service instance");
    }

    CHECK_EQ(status, STATUS_OK);

    // AIDL interfaces do not automatically start threadpools when binder objects are passed
    ABinderProcess_startThreadPool();
       ALOGI("Seccam AIDL Service Ready");
    ABinderProcess_joinThreadPool();
       ALOGI("Seccam AIDL Service Exiting");
    return EXIT_FAILURE;
}
