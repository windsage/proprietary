/**
 * Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "FACE-HAL"
#define MAX_BUF_COUNT 4

#include "Session.h"
#include <android/binder_manager.h>

using SecureProcessorConfigAidl = vendor::qti::hardware::secureprocessor::
    common::aidl::SecureProcessorConfigAidl;

namespace aidl::android::hardware::biometrics::face
{
constexpr uint64_t kAuthenticatorId = 123;
constexpr int32_t enrollmentId = 456;

// It is only assumed that 9 steps are required to complete face enrollment
int enrollmentSteps = 9;
int loop = 0;

static keymaster::HardwareAuthToken token;
static bool isEnrollment;

static inline void camera_device_on_disconnected(void* context,
                                                 ACameraDevice* device)
{
    ALOGD("Camera(id: %s) is diconnected.", ACameraDevice_getId(device));
    ALOGD("Context: %p", context);
}

static inline void camera_device_on_error(void* context, ACameraDevice* device,
                                          int error)
{
    ALOGD("Error(code: %d) on Camera(id: %s)", error,
          ACameraDevice_getId(device));
    ALOGD("Context: %p", context);
}

void Session::releaseCameraResource()
{
    camera_status_t camera_status = ACAMERA_ERROR_UNKNOWN;

    if (mCaptureRequest != NULL) {
        ACaptureRequest_free(mCaptureRequest);
        mCaptureRequest = NULL;
    }

    if (mImageReader != NULL) {
        AImageReader_delete(mImageReader);
        mImageReader = NULL;
    }

    if (mCameraOutputTarget != NULL) {
        ACameraOutputTarget_free(mCameraOutputTarget);
        mCameraOutputTarget = NULL;
    }

    if (mCameraOutputTarget2 != NULL) {
        ACameraOutputTarget_free(mCameraOutputTarget2);
        mCameraOutputTarget2 = NULL;
    }

    if (mSessionOutput != NULL) {
        ACaptureSessionOutput_free(mSessionOutput);
        mSessionOutput = NULL;
    }

    if (mSessionOutput2 != NULL) {
        ACaptureSessionOutput_free(mSessionOutput2);
        mSessionOutput2 = NULL;
    }

    if (mCaptureSessionOutputContainer != NULL) {
        ACaptureSessionOutputContainer_free(mCaptureSessionOutputContainer);
        mCaptureSessionOutputContainer = NULL;
    }

    if (mCameraDevice != NULL) {
        camera_status = ACameraDevice_close(mCameraDevice);

        if (camera_status != ACAMERA_OK) {
            ALOGE("Failed to close CameraDevice");
        }
        mCameraDevice = NULL;
    }
}

static std::shared_ptr<ISecureProcessorAidl> secureProcessorAidl;
void serviceDiedAidl(void* cookie)
{
    ALOGI("secureprocessorhal just died");
    secureProcessorAidl = nullptr;
}

::ndk::SpAIBinder secureProcessorBinder;
// Using MSP is completely optional
int Session::setUpMSPSession()
{
    CameraSecurityFrameworkVersion_t version = CSF_UNKNOWN;
    const char* serviceType = "unknown";

    // Check CSF version
    version = getCSFVersion();
    if (version == CSF_UNKNOWN) {
        ALOGE("getCSFVersion() failed");
        return -1;
    } else if (version == CSF_2_0) {
        serviceType = "/qti-tee";
    } else if (version == CSF_2_5_0 || version == CSF_2_5_1) {
        serviceType = "/qti-tvm";
    } else {
        ALOGE("Unsupported version");
        return -1;
    }

    // Get MSP AIDL HAL service
    std::string instance =
        std::string() + ISecureProcessorAidl::descriptor + serviceType;
    if (AServiceManager_isDeclared(instance.c_str())) {
        if (secureProcessorAidl == nullptr) {
            // The client can be up before the service, and getService will
            // sometimes return null, using
            // AServiceManager_waitForService(instance.c_str()) instead to wait
            // until the service is up
            secureProcessorBinder = ::ndk::SpAIBinder(
                AServiceManager_waitForService(instance.c_str()));

            if (secureProcessorBinder.get() == nullptr) {
                ALOGE("secureproccessor AIDL service doesn't exist");
            }

            auto deathRecipient = ::ndk::ScopedAIBinder_DeathRecipient(
                AIBinder_DeathRecipient_new(&serviceDiedAidl));
            auto status = ::ndk::ScopedAStatus::fromStatus(AIBinder_linkToDeath(
                secureProcessorBinder.get(), deathRecipient.get(),
                (void*)serviceDiedAidl));

            if (!status.isOk()) {
                ALOGE(
                    "linking secureproccessor AIDL service to death failed: "
                    "%d: %s",
                    status.getStatus(), status.getMessage());
            }

            secureProcessorAidl =
                ISecureProcessorAidl::fromBinder(secureProcessorBinder);

            if (secureProcessorAidl == nullptr) {
                ALOGE("secureproccessor AIDL service doesn't exist");
            }
        }
    }

    // Call MSP HAL createSession() API
    ErrorCodeAidl retCode = ErrorCodeAidl::SECURE_PROCESSOR_FAIL;
    sessionIDout sessionId;
    auto err = secureProcessorAidl->createSession(&sessionId, &retCode);
    mSessionId = sessionId.sID;
    if (!err.isOk() || retCode != ErrorCodeAidl::SECURE_PROCESSOR_OK) {
        ALOGE("createSession failed,retCode = %d,mSessionId = %d", retCode,
              mSessionId);
        return -1;
    }

    // Call MSP HAL startSession() API
    uint32_t entryLimit = 1;
    uint32_t dataLimit = 2;
    SecureProcessorConfigAidl* configBufAidl =
        new SecureProcessorConfigAidl(entryLimit, dataLimit);
    const int num_sensors[] = {1};
    uint32_t tag =
        (uint32_t)ConfigTagAidl::SECURE_PROCESSOR_SESSION_CONFIG_NUM_SENSOR;
    configBufAidl->addEntry(tag, num_sensors, 1);

    retCode = ErrorCodeAidl::SECURE_PROCESSOR_FAIL;

    // Convert AIDL config to config wrapper class
    uint32_t size = configBufAidl->getSize();
    const uint8_t* dummy = (uint8_t*)configBufAidl->releaseAndGetBuffer();
    std::vector<uint8_t> inConfig(dummy, dummy + size);

    err = secureProcessorAidl->setConfig(mSessionId, inConfig, &retCode);
    if (!err.isOk() || retCode != ErrorCodeAidl::SECURE_PROCESSOR_OK) {
        ALOGE("setConfig failed,retCode = %d,mSessionId = %d", retCode,
              mSessionId);
        err = secureProcessorAidl->deleteSession(mSessionId, &retCode);
        if (!err.isOk() || retCode != ErrorCodeAidl::SECURE_PROCESSOR_OK) {
            ALOGE("deleteSession failed,retCode = %d,mSessionId = %d", retCode,
                  mSessionId);
        }
        return -1;
    }

    retCode = ErrorCodeAidl::SECURE_PROCESSOR_FAIL;
    err = secureProcessorAidl->startSession(mSessionId, &retCode);
    if (!err.isOk() || retCode != ErrorCodeAidl::SECURE_PROCESSOR_OK) {
        ALOGE("startSession failed,retCode = %d,mSessionId = %d", retCode,
              mSessionId);
        err = secureProcessorAidl->deleteSession(mSessionId, &retCode);
        if (!err.isOk() || retCode != ErrorCodeAidl::SECURE_PROCESSOR_OK) {
            ALOGE("deleteSession failed,retCode = %d,mSessionId = %d", retCode,
                  mSessionId);
        }
        return -1;
    }

    return 0;
}

void Session::destoryMSPSession()
{
    // Call MSP HAL stopSession() API
    ErrorCodeAidl retCode = ErrorCodeAidl::SECURE_PROCESSOR_FAIL;
    auto err = secureProcessorAidl->stopSession(mSessionId, &retCode);
    if (!err.isOk() || retCode != ErrorCodeAidl::SECURE_PROCESSOR_OK) {
        ALOGE("stopSession failed,retCode = %d,mSessionId = %d", retCode,
              mSessionId);
        return;
    }

    // Call MSP HAL deleteSession() API
    retCode = ErrorCodeAidl::SECURE_PROCESSOR_FAIL;
    err = secureProcessorAidl->deleteSession(mSessionId, &retCode);
    if (!err.isOk() || retCode != ErrorCodeAidl::SECURE_PROCESSOR_OK) {
        ALOGE("deleteSession failed,retCode = %d,mSessionId = %d", retCode,
              mSessionId);
        return;
    }
}

static void OnImageCallback(void* ctx, AImageReader* reader)
{
    reinterpret_cast<Session*>(ctx)->ImageCallback(reader);
}

void Session::ImageCallback(AImageReader* reader)
{
    ALOGD("%s in", __func__);
    if (reader == nullptr) {
        ALOGE("%s: AImageReader is null", __func__);
        return;
    }

    media_status_t status;
    AImage* image = nullptr;
    status = AImageReader_acquireNextImage(reader, &image);
    if (status != AMEDIA_OK) {
        ALOGE("%s: Failed to get image, err is %d", __func__, status);
        return;
    }

    // Time consuming calculation for enroll() / authenticate()
    if (loop == 0 || loop == 100) {
        unsigned long long timeTaken = 0;
        gettimeofday(&mStopTime, NULL);
        timeTaken = ((mStopTime.tv_sec - mStartTime.tv_sec) * 1000000) +
                    (mStopTime.tv_usec - mStartTime.tv_usec);
        ALOGD(
            "%s acquired the frame, frame number: %d, time taken = %llu micro "
            "seconds",
            __func__, loop + 1, timeTaken);
    }

    mHwBuffer = nullptr;
    AImage_getHardwareBuffer(image, &mHwBuffer);
    if (mHwBuffer == nullptr) {
        ALOGE("%s: Failed to get hardware buffer", __func__);
        return;
    }

    // Call MSP HAL processImage() API
    ErrorCodeAidl retCode = ErrorCodeAidl::SECURE_PROCESSOR_OK;
    std::vector<uint8_t> AIDLOutCfg;

    uint32_t entryLimit_image = 4;
    uint32_t dataLimit_image = 8;
    SecureProcessorConfigAidl* imageConfigBuf =
        new SecureProcessorConfigAidl(entryLimit_image, dataLimit_image);

    // Convert AHardwareBuffer to nativeHandle
    const native_handle_t* nativeHandle =
        AHardwareBuffer_getNativeHandle(mHwBuffer);
    if (nativeHandle == nullptr) {
        ALOGE("%s: processImage nativeHandle is null", __func__);
        return;
    }

    AHardwareBuffer_Desc bufDesc;
    AHardwareBuffer_describe(mHwBuffer, &bufDesc);

    // Set frame buffer width
    uint32_t tag = (uint32_t)
        ConfigTagAidl::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_WIDTH;
    int32_t width = bufDesc.width;
    imageConfigBuf->addEntry(tag, &width, 1);

    // Set frame buffer height
    tag = (uint32_t)
        ConfigTagAidl::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_HEIGHT;
    int32_t height = bufDesc.height;
    imageConfigBuf->addEntry(tag, &height, 1);

    // Set frame buffer stride
    tag = (uint32_t)
        ConfigTagAidl::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_STRIDE;
    int32_t stride = bufDesc.stride;
    imageConfigBuf->addEntry(tag, &stride, 1);

    // Set frame buffer height
    tag = (uint32_t)
        ConfigTagAidl::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_FORMAT;
    int32_t format = bufDesc.format;
    imageConfigBuf->addEntry(tag, &format, 1);

    // Convert to AIDL vector
    uint32_t size = imageConfigBuf->getSize();
    const uint8_t* dummy = (uint8_t*)imageConfigBuf->releaseAndGetBuffer();
    std::vector<uint8_t> AIDLInCfg(dummy, dummy + size);

    auto ret = secureProcessorAidl->processImage(
        mSessionId, dupToAidl(nativeHandle), AIDLInCfg, &AIDLOutCfg, &retCode);

    if (!ret.isOk() || retCode != ErrorCodeAidl::SECURE_PROCESSOR_OK) {
        String8 msg("processImage failed");
        if (retCode == ErrorCodeAidl::SECURE_PROCESSOR_FAIL) {
            msg += ": retCode";
        } else {
            msg.appendFormat(": general failure (retCode = %d)", retCode);
        }
        ALOGE("%s", msg.string());
        return;
    }

    // Delete configBuf object
    delete imageConfigBuf;
    AImage_delete(image);

    /**
     * The first 90 loops are used to simulate the process of enrolling faces.
     * Update the remaining steps of enrollment through onEnrollmentProgress
     * notification UI every 10 loops. When the loop is 90, notify the UI that
     * the enrollment is complete.
     */
    if (isEnrollment) {
        if (loop <= 90 && loop % 10 == 0) {
            cb_->onEnrollmentProgress(enrollmentId,
                                      enrollmentSteps - loop / 10);
        }

        loop++;
    } else {
        cb_->onAuthenticationSucceeded(enrollmentId, token);
    }

    /**
     * The last ten loops are actually a delay. Release camera resource after
     * the delay to avoid a green screen issue.
     */
    if (loop == 100) {
        releaseCameraResource();
        destoryMSPSession();
    }
}

int Session::getCameraFrame(bool isPreviewEnabled)
{
    // Get frame from VNDK camera
    ACameraIdList* cameraIdList = NULL;
    ACameraMetadata* cameraMetadata = NULL;
    const char* selectedCameraId = NULL;
    camera_status_t camera_status = ACAMERA_ERROR_UNKNOWN;
    media_status_t media_status = AMEDIA_OK;
    bool isFrontCamera = false;
    ACameraManager* cameraManager = ACameraManager_create();

    camera_status =
        ACameraManager_getCameraIdList(cameraManager, &cameraIdList);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to get camera id list (reason: %d)", camera_status);
        return -1;
    }

    for (int i = 0; i < cameraIdList->numCameras; ++i) {
        camera_status = ACameraManager_getCameraCharacteristics(
            cameraManager, cameraIdList->cameraIds[i], &cameraMetadata);
        if (camera_status != ACAMERA_OK) {
            ALOGE(
                "Unable to query camera characteristics for camera %s. Error = "
                "%d",
                cameraIdList->cameraIds[i], camera_status);
            return -1;
        }

        ACameraMetadata_const_entry entry;
        camera_status = ACameraMetadata_getConstEntry(
            cameraMetadata, ACAMERA_LENS_FACING, &entry);
        if (camera_status != ACAMERA_OK) {
            ALOGE("Unable to request capabilities for camera %s. Error = %d",
                  cameraIdList->cameraIds[i], camera_status);
            continue;
        }

        if (entry.data.i32[0] == ACAMERA_LENS_FACING_FRONT) {
            selectedCameraId = cameraIdList->cameraIds[i];
            isFrontCamera = true;
            break;
        }
    }

    if (!isFrontCamera) {
        ALOGE("No front camera device detected");
        return -1;
    }

    mDeviceStateCallbacks.onDisconnected = camera_device_on_disconnected;
    mDeviceStateCallbacks.onError = camera_device_on_error;
    camera_status =
        ACameraManager_openCamera(cameraManager, selectedCameraId,
                                  &mDeviceStateCallbacks, &mCameraDevice);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to open camera device (id: %s)", selectedCameraId);
        return -1;
    }

    camera_status = ACameraDevice_createCaptureRequest(
        mCameraDevice, TEMPLATE_PREVIEW, &mCaptureRequest);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to create preview capture request (id: %s)",
              selectedCameraId);
        return -1;
    }

    ACaptureSessionOutputContainer_create(&mCaptureSessionOutputContainer);

    ACameraMetadata_free(cameraMetadata);
    ACameraManager_deleteCameraIdList(cameraIdList);
    ACameraManager_delete(cameraManager);

    uint64_t usage_bits;
    static const int GRALLOC1_PRODUCER_USAGE_PROTECTED = 1ULL << 14;
    usage_bits = GRALLOC1_PRODUCER_USAGE_PROTECTED;

    int* formatArray;
    int width, height;
    int numFormat = 1;
    formatArray = new int[numFormat];
    formatArray[0] = AIMAGE_FORMAT_YUV_420_888;
    width = 1920;
    height = 1080;
    media_status =
        AImageReader_newWithUsage(width, height, formatArray[0], usage_bits,
                                  MAX_BUF_COUNT, &mImageReader);
    if (media_status != AMEDIA_OK || mImageReader == nullptr) {
        ALOGE("Failed to create new AImageReader, ret=%d, image_reader_=%p",
              media_status, mImageReader);
        return -1;
    }

    AImageReader_ImageListener listener{
        .context = this, .onImageAvailable = OnImageCallback,
    };
    media_status = AImageReader_setImageListener(mImageReader, &listener);
    if (media_status != AMEDIA_OK) {
        ALOGE("Failed to set image listener");
        return -1;
    }

    media_status =
        AImageReader_getWindowNativeHandle(mImageReader, &mImgReaderAnw);
    if (media_status != AMEDIA_OK) {
        ALOGE("Failed to get image reader native window");
        return -1;
    }

    camera_status =
        ACaptureSessionOutput_create(mImgReaderAnw, &mSessionOutput);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to create mSessionOutput, camera_status %d",
              camera_status);
        return -1;
    }

    if (isPreviewEnabled) {
        camera_status =
            ACaptureSessionOutput_create(mSurfaceRef, &mSessionOutput2);
        if (camera_status != ACAMERA_OK) {
            ALOGE("Failed to create mSessionOutput2, camera_status %d",
                  camera_status);
            return -1;
        }
    }

    camera_status = ACaptureSessionOutputContainer_add(
        mCaptureSessionOutputContainer, mSessionOutput);
    if (camera_status != ACAMERA_OK) {
        ALOGE(
            "Failed to add mSessionOutput in mCaptureSessionOutputContainer, "
            "camera_status %d",
            camera_status);
        return -1;
    }

    if (isPreviewEnabled) {
        camera_status = ACaptureSessionOutputContainer_add(
            mCaptureSessionOutputContainer, mSessionOutput2);
        if (camera_status != ACAMERA_OK) {
            ALOGE(
                "Failed to add mSessionOutput2 in "
                "mCaptureSessionOutputContainer, "
                "camera_status %d",
                camera_status);
            return -1;
        }
    }

    camera_status =
        ACameraOutputTarget_create(mImgReaderAnw, &mCameraOutputTarget);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to create mCameraOutputTarget, camera_status %d",
              camera_status);
        return -1;
    }

    if (isPreviewEnabled) {
        camera_status =
            ACameraOutputTarget_create(mSurfaceRef, &mCameraOutputTarget2);
        if (camera_status != ACAMERA_OK) {
            ALOGE("Failed to create mCameraOutputTarget2, camera_status %d",
                  camera_status);
            return -1;
        }
    }

    camera_status =
        ACaptureRequest_addTarget(mCaptureRequest, mCameraOutputTarget);
    if (camera_status != ACAMERA_OK) {
        ALOGE(
            "Failed to add mCameraOutputTarget in mCaptureRequest, "
            "camera_status "
            "%d",
            camera_status);
        return -1;
    }

    if (isPreviewEnabled) {
        camera_status =
            ACaptureRequest_addTarget(mCaptureRequest, mCameraOutputTarget2);
        if (camera_status != ACAMERA_OK) {
            ALOGE(
                "Failed to add mCameraOutputTarget2 in mCaptureRequest, "
                "camera_status "
                "%d",
                camera_status);
            return -1;
        }
    }

    camera_status = ACameraDevice_createCaptureSession(
        mCameraDevice, mCaptureSessionOutputContainer,
        &mCaptureSessionStateCallbacks, &mCaptureSession);
    if (camera_status != ACAMERA_OK) {
        ALOGE(
            "Failed to create mCaptureSession by CameraDevice, camera_status "
            "%d",
            camera_status);
        return -1;
    }

    camera_status = ACameraCaptureSession_setRepeatingRequest(
        mCaptureSession, NULL, 1, &mCaptureRequest, NULL);
    if (camera_status != ACAMERA_OK) {
        ALOGE(
            "Failed to setRepeatingRequest by mCaptureSession, camera_status "
            "%d",
            camera_status);
        return -1;
    }

    return 0;
}

CameraSecurityFrameworkVersion_t Session::getCSFVersion()
{
    CameraSecurityFrameworkVersion_t version = CSF_UNKNOWN;
    Object clientEnv = Object_NULL;
    Object secCamSrvObj = Object_NULL;
    uint32_t archVer_ptr, maxVer_ptr, minVer_ptr;

    if (TZCom_getClientEnvObject(&clientEnv) != Object_OK) {
        ALOGE("TZCom_getClientEnvObject failed");
        goto exit;
    }

    if (IClientEnv_open(clientEnv, CSecureCamera2_UID, &secCamSrvObj) !=
        Object_OK) {
        ALOGE("IClientEnv_open failed");
        goto exit;
    }

    if (ISecureCamera2_getCSFVersion(secCamSrvObj, &archVer_ptr, &maxVer_ptr,
                                     &minVer_ptr) != Object_OK) {
        ALOGE("ISecureCamera2_getCSFVersion failed");
        goto exit;
    }

    if (archVer_ptr == 2 && maxVer_ptr == 0) {
        version = CSF_2_0;
    } else if (archVer_ptr == 2 && maxVer_ptr == 5 && minVer_ptr == 0) {
        version = CSF_2_5_0;
    } else if (archVer_ptr == 2 && maxVer_ptr == 5 && minVer_ptr == 1) {
        version = CSF_2_5_1;
    } else if (archVer_ptr == 3 && maxVer_ptr == 0) {
        version = CSF_3_0;
    } else {
        ALOGE("Unknown version number");
    }

    ALOGI("CSF version: %d.%d.%d", archVer_ptr, maxVer_ptr, minVer_ptr);

exit:
    Object_ASSIGN_NULL(secCamSrvObj);
    Object_ASSIGN_NULL(clientEnv);

    return version;
}

// Methods from ISession follow.
class CancellationSignal : public common::BnCancellationSignal
{
   private:
    std::shared_ptr<ISessionCallback> cb_;

   public:
    explicit CancellationSignal(std::shared_ptr<ISessionCallback> cb)
        : cb_(std::move(cb))
    {
    }

    ndk::ScopedAStatus cancel() override
    {
        cb_->onError(Error::CANCELED, 0 /* vendorCode */);
        return ndk::ScopedAStatus::ok();
    }
};

Session::Session(std::shared_ptr<ISessionCallback> cb) : cb_(std::move(cb))
{
}

ndk::ScopedAStatus Session::generateChallenge()
{
    ALOGI("%s in", __func__);
    int64_t mRandom;
    int fd = -1;
    fd = open("/dev/urandom", O_RDONLY);
    read(fd, &mRandom, sizeof(mRandom));
    ::close(fd);
    challenge = mRandom;
    if (cb_) {
        cb_->onChallengeGenerated(challenge);
    }
    ALOGI("%s - the challenge is 0x%" PRIx64, __func__, challenge);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::revokeChallenge(int64_t challenge)
{
    ALOGI("%s in", __func__);
    if (cb_) {
        cb_->onChallengeRevoked(challenge);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::getEnrollmentConfig(
    EnrollmentType enrollmentType,
    std::vector<EnrollmentStageConfig>* return_val)
{
    ALOGI("%s in", __func__);

    if (enrollmentType == EnrollmentType::DEFAULT) {
        ALOGI("%s - enrollmentType = DEFAULT", __func__);
    }

    EnrollmentStageConfig enrollmentStageConfig;
    enrollmentStageConfig.stage = EnrollmentStage::WAITING_FOR_CENTERING;

    *return_val = {std::move(enrollmentStageConfig)};
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::enroll(
    const keymaster::HardwareAuthToken& hat, EnrollmentType /*enrollmentType*/,
    const std::vector<Feature>& /*features*/,
    const std::optional<NativeHandle>& previewSurface,
    std::shared_ptr<biometrics::common::ICancellationSignal>* /*return_val*/)
{
    ALOGI("%s in", __func__);

    gettimeofday(&mStartTime, NULL);

    token = hat;

    bool isPreviewEnabled = true;

    if (!previewSurface.has_value()) {
        isPreviewEnabled = false;
        ALOGI("%s - previewSurface is null", __func__);
    } else {
        mSurfaceRef =
            const_cast<native_handle_t*>(makeFromAidl(previewSurface.value()));
    }

    /**
      Enrollment bussiness process
      1.verify token
      2.get the secure camera frame
      3.create a user and save it in SFS
      4.return the enrollment result
    */

    if (setUpMSPSession() != 0) {
        ALOGE("Failed to run setUpMSPSession()");
        return ndk::ScopedAStatus::fromExceptionCode(EX_TRANSACTION_FAILED);
    }

    isEnrollment = true;

    /**
     * OEMs need to use the default Settings apk for multi sensor configuration.
     * Settings apk opens the front camera for non-secure preview, so OEMs need
     * to modify the HAL to open another camera for secure capture. OEMs need to
     * use the recompiled Settings apk for single sensor configuration. The
     * sample HAL uses the front camera for secure preview and secure capture by
     * default.
     */
    if (getCameraFrame(isPreviewEnabled) != 0) {
        ALOGE("Failed to run getCameraFrame()");
        return ndk::ScopedAStatus::fromExceptionCode(EX_TRANSACTION_FAILED);
    }

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::authenticate(
    int64_t keystoreOperationId,
    std::shared_ptr<common::ICancellationSignal>* /*return_val*/)
{
    ALOGI("%s in, operationId = 0x%" PRIx64,  __func__, keystoreOperationId);

    gettimeofday(&mStartTime, NULL);

    /**
      Authentication bussiness process
      1.get operation_id from the request message
      2.get hw auth token from gatekeeper
      3.copy hat into local token
      4.verify token
      5.get the secure camera frame
      6.match a user face
      7.return the authenticate result
    */

    if (setUpMSPSession() != 0) {
        ALOGE("Failed to run setUpMSPSession()");
        return ndk::ScopedAStatus::fromExceptionCode(EX_TRANSACTION_FAILED);
    }

    isEnrollment = false;
    bool isPreviewEnabled = false;

    if (getCameraFrame(isPreviewEnabled) != 0) {
        ALOGE("Failed to run getCameraFrame()");
        return ndk::ScopedAStatus::fromExceptionCode(EX_TRANSACTION_FAILED);
    }

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::detectInteraction(
    std::shared_ptr<common::ICancellationSignal>* /*return_val*/)
{
    ALOGI("%s in", __func__);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::enumerateEnrollments()
{
    ALOGI("%s in", __func__);
    if (cb_) {
        cb_->onEnrollmentsEnumerated(std::vector<int32_t>());
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::removeEnrollments(
    const std::vector<int32_t>& /*enrollmentIds*/)
{
    ALOGI("%s in", __func__);
    if (cb_) {
        cb_->onEnrollmentsRemoved(std::vector<int32_t>());
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::getFeatures()
{
    ALOGI("%s in", __func__);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::setFeature(
    const keymaster::HardwareAuthToken& /*hat*/, Feature /*feature*/,
    bool /*enabled*/)
{
    ALOGI("%s in", __func__);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::getAuthenticatorId()
{
    ALOGI("%s in", __func__);
    if (cb_) {
        cb_->onAuthenticatorIdRetrieved(kAuthenticatorId);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::invalidateAuthenticatorId()
{
    ALOGI("%s in", __func__);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::resetLockout(
    const keymaster::HardwareAuthToken& /*hat*/)
{
    ALOGI("%s in", __func__);
    if (cb_) {
        cb_->onLockoutCleared();
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::close()
{
    ALOGI("%s in", __func__);
    if (cb_) {
        cb_->onSessionClosed();
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::authenticateWithContext(
    int64_t operationId, const common::OperationContext& /*context*/,
    std::shared_ptr<common::ICancellationSignal>* out)
{
    ALOGI("%s in", __func__);
    return authenticate(operationId, out);
}

ndk::ScopedAStatus Session::enrollWithContext(
    const keymaster::HardwareAuthToken& hat,
    EnrollmentType enrollmentType,
    const std::vector<Feature>& features,
    const std::optional<NativeHandle>& previewSurface,
    const common::OperationContext& /*context*/,
    std::shared_ptr<common::ICancellationSignal>* out)
{
    ALOGI("%s in", __func__);
    return enroll(hat, enrollmentType, features, previewSurface, out);
}

ndk::ScopedAStatus Session::detectInteractionWithContext(
    const common::OperationContext& /*context*/,
    std::shared_ptr<common::ICancellationSignal>* out)
{
    ALOGI("%s in", __func__);
    return detectInteraction(out);
}

ndk::ScopedAStatus Session::onContextChanged(const common::OperationContext& /*context*/)
{
    ALOGI("%s in", __func__);
    return ndk::ScopedAStatus::ok();
}

}  // namespace aidl::android::hardware::biometrics::face
