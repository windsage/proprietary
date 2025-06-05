/**
 * Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "SECURE-PROCESSOR-FACTORY"

#include "SecureProcessorFactory.h"
#include "SecureProcessorQTEEMink.h"
#include "SecureProcessorTVMMink.h"

namespace vendor
{
namespace qti
{
namespace hardware
{
namespace secureprocessor
{
namespace device
{
namespace V1_0
{
namespace implementation
{
using namespace std;

using vendor::qti::hardware::secureprocessor::device::V1_0::implementation::
    SecureProcessorQTEEMink;
using vendor::qti::hardware::secureprocessor::device::V1_0::implementation::
    SecureProcessorTVMMink;
using vendor::qti::hardware::secureprocessor::device::V1_0::implementation::
    TYPE_QTI_TEE;
using vendor::qti::hardware::secureprocessor::device::V1_0::implementation::
    TYPE_QTI_TVM;

// Methods from ::android::hidl::base::V1_0::IBase follow.

ISecureProcessor *SecureProcessorFactory::CreateSecureProcessor(
    const string &type)
{
    ISecureProcessor *dest_sp = nullptr;

    ALOGI("%s: Create Secure Processor type %s", __func__, type.c_str());

    if (type.compare(TYPE_QTI_TEE) == 0) {
        dest_sp = new SecureProcessorQTEEMink("seccamdemo2");
    }

    if (type.compare(TYPE_QTI_TVM) == 0) {
        dest_sp = new SecureProcessorTVMMink("seccamdemo25");
    } else {
        ALOGE("Failed to create secure processor type %s", type.c_str());
    }

    return dest_sp;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace device
}  // namespace secureprocessor
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
