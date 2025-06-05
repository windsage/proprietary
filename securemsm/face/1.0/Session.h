/**
 * Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <aidl/android/hardware/biometrics/face/BnSession.h>
#include <aidl/android/hardware/biometrics/face/ISessionCallback.h>
#include <aidl/android/hardware/biometrics/common/BnCancellationSignal.h>
#include <aidl/vendor/qti/hardware/secureprocessor/config/ConfigTag.h>
#include <aidl/vendor/qti/hardware/secureprocessor/config/ConfigType.h>
#include <aidl/vendor/qti/hardware/secureprocessor/device/BnSecureProcessor.h>

#include <aidlcommonsupport/NativeHandle.h>
#include <android-base/logging.h>
#include <cstdlib>
#include <hardware/hw_auth_token.h>
#include <inttypes.h>
#include <log/log.h>
#include <random>
#include <sys/time.h>

#include <NdkCameraDevice.h>
#include <NdkCameraManager.h>
#include <NdkImageReader.h>
#include <media/NdkImage.h>
#include <ui/GraphicBuffer.h>
#include <vndk/hardware_buffer.h>

#include "CSecureCamera2.h"
#include "IClientEnv.h"
#include "ISecureCamera2.h"
#include "SecureProcessorConfigAidl.h"
#include "TZCom.h"
#include "object.h"
#include "util/CancellationSignal.h"

namespace aidl::android::hardware::biometrics::face
{
namespace common = aidl::android::hardware::biometrics::common;
namespace keymaster = aidl::android::hardware::keymaster;

using aidl::android::hardware::common::NativeHandle;
using ISecureProcessorAidl =
    ::aidl::vendor::qti::hardware::secureprocessor::device::ISecureProcessor;
using ErrorCodeAidl =
    ::aidl::vendor::qti::hardware::secureprocessor::common::ErrorCode;
using ConfigTagAidl =
    ::aidl::vendor::qti::hardware::secureprocessor::config::ConfigTag;
using sessionIDout =
    ::aidl::vendor::qti::hardware::secureprocessor::device::sessionIDout;

typedef enum CameraSecurityFrameworkVersion {
    CSF_2_0 = 0,
    CSF_2_5_0,
    CSF_2_5_1,
    CSF_3_0,
    CSF_UNKNOWN,
} CameraSecurityFrameworkVersion_t;

class Session : public BnSession
{
   public:
    explicit Session(std::shared_ptr<ISessionCallback> cb);

    ndk::ScopedAStatus generateChallenge() override;

    ndk::ScopedAStatus revokeChallenge(int64_t challenge) override;

    ndk::ScopedAStatus getEnrollmentConfig(
        EnrollmentType enrollmentType,
        std::vector<EnrollmentStageConfig>* return_val) override;

    ndk::ScopedAStatus enroll(
        const keymaster::HardwareAuthToken& hat, EnrollmentType enrollmentType,
        const std::vector<Feature>& features,
        const std::optional<NativeHandle>& previewSurface,
        std::shared_ptr<common::ICancellationSignal>* return_val) override;

    ndk::ScopedAStatus authenticate(
        int64_t keystoreOperationId,
        std::shared_ptr<common::ICancellationSignal>* returnVal) override;

    ndk::ScopedAStatus detectInteraction(
        std::shared_ptr<common::ICancellationSignal>* returnVal) override;

    ndk::ScopedAStatus enumerateEnrollments() override;

    ndk::ScopedAStatus removeEnrollments(
        const std::vector<int32_t>& enrollmentIds) override;

    ndk::ScopedAStatus getFeatures() override;

    ndk::ScopedAStatus setFeature(const keymaster::HardwareAuthToken& hat,
                                  Feature feature, bool enabled) override;

    ndk::ScopedAStatus getAuthenticatorId() override;

    ndk::ScopedAStatus invalidateAuthenticatorId() override;

    ndk::ScopedAStatus resetLockout(
        const keymaster::HardwareAuthToken& hat) override;

    ndk::ScopedAStatus close() override;

    ndk::ScopedAStatus authenticateWithContext(
        int64_t operationId, const common::OperationContext& context,
        std::shared_ptr<common::ICancellationSignal>* out) override;

    ndk::ScopedAStatus enrollWithContext(
        const keymaster::HardwareAuthToken& hat, EnrollmentType enrollmentType,
        const std::vector<Feature>& features, const std::optional<NativeHandle>& previewSurface,
        const common::OperationContext& context,
        std::shared_ptr<common::ICancellationSignal>* out) override;

    ndk::ScopedAStatus detectInteractionWithContext(
        const common::OperationContext& context,
        std::shared_ptr<common::ICancellationSignal>* out) override;

    ndk::ScopedAStatus onContextChanged(const common::OperationContext& context) override;

   private:
    std::shared_ptr<ISessionCallback> cb_;
    int64_t challenge;
    struct timeval mStartTime;
    struct timeval mStopTime;

   public:
    uint32_t mSessionId;
    void ImageCallback(AImageReader* reader);
    void releaseCameraResource();
    void destoryMSPSession();
    int setUpMSPSession();
    int getCameraFrame(bool isPreviewEnabled);
    CameraSecurityFrameworkVersion_t getCSFVersion();

   protected:
    ACameraDevice* mCameraDevice;
    ACaptureRequest* mCaptureRequest;
    ACaptureSessionOutputContainer* mCaptureSessionOutputContainer;
    ACameraCaptureSession* mCaptureSession;

    ACameraDevice_StateCallbacks mDeviceStateCallbacks;
    ACameraCaptureSession_stateCallbacks mCaptureSessionStateCallbacks;

    AImageReader* mImageReader;
    AHardwareBuffer* mHwBuffer;
    ACameraOutputTarget* mCameraOutputTarget;
    ACameraOutputTarget* mCameraOutputTarget2;
    ACaptureSessionOutput* mSessionOutput;
    ACaptureSessionOutput* mSessionOutput2;
    native_handle_t* mImgReaderAnw;
    native_handle_t* mSurfaceRef;
};

}  // namespace aidl::android::hardware::biometrics::face
