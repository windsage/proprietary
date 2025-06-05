/**
 * Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "SECURE-PROCESSOR"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include <vendor/qti/hardware/secureprocessor/config/1.0/types.h>
#include <vendor/qti/hardware/secureprocessor/device/1.0/ISecureProcessor.h>
#include "SecureProcessorFactory.h"
#include "SecureProcessorQTEEMink.h"
#include "SecureProcessorTVMMink.h"

using vendor::qti::hardware::secureprocessor::device::V1_0::ISecureProcessor;
using vendor::qti::hardware::secureprocessor::device::V1_0::implementation::
    SecureProcessorFactory;
using vendor::qti::hardware::secureprocessor::device::V1_0::implementation::
    TYPE_QTI_TEE;
using vendor::qti::hardware::secureprocessor::device::V1_0::implementation::
    TYPE_QTI_TVM;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;

int32_t main()
{
    int32_t status;

    ALOGI("Starting qti-tee service instance");

    /*
     * Attempt to create TEE Device to communicate with Trusted App
     * use a factory method to create multiple implementation.
     */
    android::sp<ISecureProcessor> destinationTEE =
        SecureProcessorFactory::CreateSecureProcessor(
            std::string(TYPE_QTI_TEE));

    /*
     * Attempt to create TVM Device to communicate with TVM App
     * use a factory method to create multiple implementation.
     */
    android::sp<ISecureProcessor> destinationTVM =
        SecureProcessorFactory::CreateSecureProcessor(
            std::string(TYPE_QTI_TVM));

    configureRpcThreadpool(1, true /* willJoinThreadpool */);

    if (destinationTEE) {
        status = destinationTEE->registerAsService(TYPE_QTI_TEE);
        if (status != 0) {
            ALOGE("Failed to register qti-tee service instance");
        }
    } else {
        ALOGE("Failed to create qti-tee service instance");
    }

    if (destinationTVM) {
        status = destinationTVM->registerAsService(TYPE_QTI_TVM);
        if (status != 0) {
            ALOGE("Failed to register qti-tvm service instance");
        }
    } else {
        ALOGE("Failed to create qti-tvm service instance");
    }

    ALOGI("Started qti-tee service instance");
    joinRpcThreadpool();
    return -1;  // Should never get here.
}
