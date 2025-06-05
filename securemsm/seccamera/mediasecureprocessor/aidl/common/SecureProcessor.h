/**
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#pragma once
#ifndef VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_SECUREPROCESSOR_H
#define VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_SECUREPROCESSOR_H

#include "ISecureCamera2App.hpp"
#include "SecureProcessorConfigAidl.h"
#include <aidl/vendor/qti/hardware/secureprocessor/device/BnSecureProcessor.h>
#include <aidl/vendor/qti/hardware/secureprocessor/device/sessionIDout.h>
#include <mutex>

#define MAX_CERT_SIZE (2048)

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace secureprocessor {
namespace device {
using ::aidl::android::hardware::common::NativeHandle;
using ::aidl::vendor::qti::hardware::secureprocessor::common::ErrorCode;
using ::aidl::vendor::qti::hardware::secureprocessor::device::sessionIDout;
using ::ndk::ScopedAStatus;
using ::vendor::qti::hardware::secureprocessor::common::aidl::ConfigEntry;
using ::vendor::qti::hardware::secureprocessor::common::aidl::
    SecureProcessorConfigAidl;

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

class SecureProcessor : public BnSecureProcessor {
public:
  SecureProcessor();
  ~SecureProcessor();

  // Declare the loadApp/unLoadApp as pure virtual methods so that the
  // methods of base class can call the same methods of its derived class.
  virtual ErrorCode loadApp(ISecureCamera2App **appObj, char *appName) {
    return ErrorCode::SECURE_PROCESSOR_FAIL;
  }
  virtual ErrorCode unLoadApp(ISecureCamera2App *appObj) {
    return ErrorCode::SECURE_PROCESSOR_FAIL;
  }

  ScopedAStatus createSession(sessionIDout *out_sessionId,
                              ErrorCode *_aidl_return) override;
  ScopedAStatus deleteSession(int32_t in_sessionId,
                              ErrorCode *_aidl_return) override;
  ScopedAStatus getConfig(int32_t in_sessionId,
                          const std::vector<uint8_t> &in_inConfig,
                          std::vector<uint8_t> *out_outConfig,
                          ErrorCode *_aidl_return) override;
  ScopedAStatus processImage(int32_t in_sessionId, const NativeHandle &in_image,
                             const std::vector<uint8_t> &in_inConfig,
                             std::vector<uint8_t> *out_outConfig,
                             ErrorCode *_aidl_return) override;
  ScopedAStatus setConfig(int32_t in_sessionId,
                          const std::vector<uint8_t> &in_inConfig,
                          ErrorCode *_aidl_return) override;
  ScopedAStatus startSession(int32_t in_sessionId,
                             ErrorCode *_aidl_return) override;
  ScopedAStatus stopSession(int32_t in_sessionId,
                            ErrorCode *_aidl_return) override;
  // changed the definition of resetCamera
  ScopedAStatus resetCamera(int32_t in_sessionId, ErrorCode *_aidl_return);

  // Protected variables specific to this specific implementation of
  // ISecureProcessor
protected:
    CameraSecurityFrameworkVersion_t mCSFVersion_ = CSF_UNKNOWN;
    std::string mTaName_ = "seccamdemo2";

    ErrorCode _getCSFVersion(CameraSecurityFrameworkVersion_t &version);

  // Private variables specific to this specific implementation of
  // ISecureProcessor
private:
  ISecureCamera2App *mSeccam2App = nullptr;

  void _resetImgCfg(uint32_t sessionId);
  ErrorCode _handleSessionCfg(uint32_t sessionId, ConfigEntry *entry);
  ErrorCode _createSessionCfg(uint32_t sessionId,
                              SecureProcessorConfigAidl *cfgWrapper,
                              ConfigEntry *entry);
  ErrorCode _handleImageCfg(uint32_t sessionId, ConfigEntry *entry);
  ErrorCode _processImage(uint32_t sessionId, const NativeHandle &image);
  ErrorCode _processConfig(uint32_t sessionId, uint32_t operation,
                           const std::vector<uint8_t> &inConfig);
  ErrorCode _processConfig(uint32_t sessionId, uint32_t operation,
                           const std::vector<uint8_t> &inConfig,
                           std::vector<uint8_t> &outConfig);
  ErrorCode _updatePlanes(uint32_t sessionId,
                          ISecureCamera2App_PlaneInfo *planeData,
                          uint32_t *numOfInPlanes);
  ErrorCode _resetCamera(uint32_t sessionId);
};

} // namespace device
} // namespace secureprocessor
} // namespace hardware
} // namespace qti
} // namespace vendor
} // namespace aidl

#endif // VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_SECUREPROCESSOR_H
