/**
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_0_SECUREPROCESSOR_H
#define VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_0_SECUREPROCESSOR_H

#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <vendor/qti/hardware/secureprocessor/config/1.0/types.h>
#include <vendor/qti/hardware/secureprocessor/device/1.0/ISecureProcessor.h>
#include <mutex>
#include "ISecureCamera2App.hpp"
#include "SecureProcessorConfig.h"
#include "SecureProcessorUtils.h"

#define MAX_CERT_SIZE (2048)

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
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_handle;
using ::android::sp;
using vendor::qti::hardware::secureprocessor::common::V1_0::ConfigEntry;
using vendor::qti::hardware::secureprocessor::common::V1_0::
    SecureProcessorConfig;
using vendor::qti::hardware::secureprocessor::common::V1_0::ErrorCode;

typedef enum SecureProcessorState {
    SECURE_PROCESSOR_STATE_NONE = 0,
    SECURE_PROCESSOR_STATE_INIT,
    SECURE_PROCESSOR_STATE_RUNNING,
    SECURE_PROCESSOR_STATE_RESET,
} SecureProcessorState_t;

typedef enum CameraSecurityFrameworkVersion {
    CSF_2_0 = 0,
    CSF_2_5_0,
    CSF_2_5_1,
    CSF_3_0,
    CSF_UNKNOWN,
} CameraSecurityFrameworkVersion_t;

struct ImageBuffer {
    int32_t size;
    int32_t width;
    int32_t height;
    int32_t stride;
    int32_t format;
};

struct ImageConfig {
    int32_t cameraId;
    int64_t frameNum;
    int64_t timeStamp;
    ImageBuffer buffer;
};

struct SessionConfig {
    bool active;
    SecureProcessorState_t state;
    int32_t numSensor;
    uint32_t license_size;
    uint8_t license[MAX_CERT_SIZE];
    std::string taName;
    ImageConfig imgCfg;
};

#define MAX_SESSION 10
#define SHARED_BUFFER_SIZE 8192

class SecureProcessor : public ISecureProcessor
{
   public:
    SecureProcessor();
    ~SecureProcessor();

    // Declare the loadApp/unLoadApp as pure virtual methods so that the
    // methods of base class can call the same methods of its derived class.
    virtual ErrorCode loadApp(ISecureCamera2App **appObj, char *appName) = 0;
    virtual ErrorCode unLoadApp(ISecureCamera2App *appObj) = 0;

    // Methods from
    // ::vendor::qti::hardware::SecureProcessor::V1_0::ISecureProcessor
    // follow.
    Return<void> createSession(createSession_cb _hidl_cb) override;
    Return<common::V1_0::ErrorCode> setConfig(
        uint32_t sessionId, const hidl_vec<uint8_t> &inConfig) override;
    Return<void> getConfig(uint32_t sessionId,
                           const hidl_vec<uint8_t> &inConfig,
                           getConfig_cb _hidl_cb) override;
    Return<common::V1_0::ErrorCode> startSession(uint32_t sessionId) override;
    Return<void> processImage(uint32_t sessionId, const hidl_handle &image,
                              const hidl_vec<uint8_t> &inConfig,
                              processImage_cb _hidl_cb) override;
    Return<common::V1_0::ErrorCode> stopSession(uint32_t sessionId) override;
    Return<common::V1_0::ErrorCode> deleteSession(uint32_t sessionId) override;
    Return<common::V1_0::ErrorCode> resetCamera(uint32_t sessionId);

    // Methods from ::android::hidl::base::V1_0::IBase follow.

    // Protected variables specific to this specific implementation of
    // ISecureProcessor
   protected:
    std::string mTaName_ = "seccamdemo2";
    CameraSecurityFrameworkVersion_t mCSFVersion_ = CSF_UNKNOWN;

    ErrorCode _getCSFVersion(CameraSecurityFrameworkVersion_t &version);

    // Private variables specific to this specific implementation of
    // ISecureProcessor
   private:
    SessionConfig sConfig_[MAX_SESSION];
    mutable std::mutex mLock_;
    ISecureCamera2App *mSeccam2App = nullptr;

    void _resetImgCfg(uint32_t sessionId);
    ErrorCode _handleSessionCfg(uint32_t sessionId, ConfigEntry *entry);
    ErrorCode _createSessionCfg(uint32_t sessionId,
                                SecureProcessorConfig *cfgWrapper,
                                ConfigEntry *entry);
    ErrorCode _handleImageCfg(uint32_t sessionId, ConfigEntry *entry);
    ErrorCode _processImage(uint32_t sessionId, const hidl_handle &image);
    ErrorCode _processConfig(uint32_t sessionId, uint32_t operation,
                             const hidl_vec<uint8_t> &inConfig);
    ErrorCode _processConfig(uint32_t sessionId, uint32_t operation,
                             const hidl_vec<uint8_t> &inConfig,
                             hidl_vec<uint8_t> &outConfig);
    ErrorCode _updatePlanes(uint32_t sessionId,
                            ISecureCamera2App_PlaneInfo *planeData,
                            uint32_t *numOfInPlanes);
    ErrorCode _resetCamera(uint32_t sessionId);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace device
}  // namespace secureprocessor
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_0_SECUREPROCESSOR_H
