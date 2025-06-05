/**
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_SECUREPROCESSORTEEMINK_H
#define VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_SECUREPROCESSORTEEMINK_H

#include "SecureProcessor.h"
#include "object.h"

namespace aidl
{
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

using namespace std;
using ::aidl::vendor::qti::hardware::secureprocessor::common::ErrorCode;

static const char *TYPE_QTI_TEE = "qti-tee";

struct SecureProcessorQTEEMink : public SecureProcessor {
    SecureProcessorQTEEMink(string taName);

    ErrorCode loadApp(ISecureCamera2App **appObj, char *appName) override;
    ErrorCode unLoadApp(ISecureCamera2App *appObj) override;

    // Private variables specific to this specific implementation of
    // SecureProcessorQTEEMink
   private:
    Object mAppController = Object_NULL;
    ErrorCode _getAppController(Object appLoader, std::string const &path,
                               Object *appController);
};

}  // namespace device
}  // namespace secureprocessor
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif  // VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_0_SECUREPROCESSORTEEMINK_H
