/**
 * Copyright (c) 2019-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "SECURE-PROCESSOR"

#include "SecureProcessor.h"
#include "CSecureCamera2.h"
#include "IClientEnv.h"
#include "ISecureCamera2.h"
#include "ISecureCamera2App_invoke.hpp"
#include "RemoteShareMemory.h"
#include "TZCom.h"
#include "fdwrapper.h"
#include "utils.h"

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
using vendor::qti::hardware::secureprocessor::common::V1_0::ErrorCode;
using vendor::qti::hardware::secureprocessor::config::V1_0::ConfigType;
using vendor::qti::hardware::secureprocessor::config::V1_0::ConfigTag;

#define VALID_SESSION(id) (((id) < MAX_SESSION) && (sConfig_[(id)].active))

#define GET_CONFIG 0
#define SET_CONFIG 1
#define SESSION_CONFIG 0
#define IMAGE_CONFIG 1
#define GET_SESSION_CONFIG (SESSION_CONFIG << 1 | GET_CONFIG)  // 0
#define SET_SESSION_CONFIG (SESSION_CONFIG << 1 | SET_CONFIG)  // 1
#define GET_IMAGE_CONFIG (IMAGE_CONFIG << 1 | GET_CONFIG)      // 2
#define SET_IMAGE_CONFIG (IMAGE_CONFIG << 1 | SET_CONFIG)      // 3
#define SET_CONFIG_MASK 0x1

// Extending ConfigTag
#define SECURE_PROCESSOR_CUSTOM_CONFIG_USECASE_LICENSE \
    ((uint32_t)ConfigTag::SECURE_PROCESSOR_CUSTOM_CONFIG_START + 1)
#define SECURE_PROCESSOR_CUSTOM_CONFIG_USECASE_RESET_CAMERA \
    ((uint32_t)ConfigTag::SECURE_PROCESSOR_CUSTOM_CONFIG_START + 2)

// Extending ErrorCode
#define SECURE_PROCESSOR_LICENSE_ERROR \
    ((int32_t)ErrorCode::SECURE_PROCESSOR_CUSTOM_STATUS + 1)

// 1024 byte license
static const uint8_t gLicense[1024] = {0};

/**
 * _processConfig:
 *
 * Process session configuration. The input configuration buffer (inConfig)
 * contains set of required tag entries. the operation is the command for
 * _processConfig to process image and session operations.
 *
 * @param sessionId Session identifier.
 *
 * @param operation operation command to process different configurations.
 *
 * @param inConfig Input configuration buffer.
 *     It contains required tags to be queried.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Process configuration successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Failed to process configuration.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid configuration parameter passed.
 *
 */
ErrorCode SecureProcessor::_processConfig(uint32_t sessionId,
                                          uint32_t operation,
                                          const hidl_vec<uint8_t> &inConfig)
{
    SecureProcessorCfgBuf outConfig = {0};

    return _processConfig(sessionId, operation, inConfig, outConfig);
}

/**
 * _processConfig:
 *
 * Process session configuration. The input configuration buffer (inConfig)
 * contains set of required tag entries. The output configuration buffer
 * (outConfig) is populated with set of pairs having <tag, value> entries
 * for requested configuration tags. Both input and output configuration
 * buffers are expected to be prepared using SecureProcessorConfig common
 * helper class.
 *
 * @param sessionId Session identifier.
 *
 * @param operation operation command to process different configurations.
 *
 * @param inConfig Input configuration buffer.
 *     It contains required tags to be queried.
 *
 * @param outConfig output configuration buffer.
 *     It contains set of pairs having <tag, value> entries.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Process configuration successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Failed to process configuration.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid configuration parameter passed.
 *
 * @return outConfig Output configuration buffer.
 *     It contains set of <tag, value> pairs for requested tags.
 *
 */
ErrorCode SecureProcessor::_processConfig(uint32_t sessionId,
                                          uint32_t operation,
                                          const hidl_vec<uint8_t> &inConfig,
                                          hidl_vec<uint8_t> &outConfig)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    SecureProcessorConfig *cfgOutWrapper = nullptr;
    uint32_t numEntries = 0;
    // Convert HIDL config to config wrapper class
    SecureProcessorConfig *cfgInWrapper = nullptr;

    convertFromHidl(&inConfig, &cfgInWrapper);
    T_CHECK_ERR(cfgInWrapper != nullptr, ErrorCode::SECURE_PROCESSOR_FAIL);

    // Retrive number of config entries
    numEntries = cfgInWrapper->entryCount();
    LOG_MSG("numEntries: %d, op: 0x%x", numEntries, operation);

    if ((operation & SET_CONFIG_MASK) == GET_CONFIG) {
        // Allocate the SecureProcessorConfig class for outConfig and
        // it's associated config buffer with requested number of entries
        cfgOutWrapper = new SecureProcessorConfig(numEntries);
        T_CHECK_ERR(cfgOutWrapper != nullptr, ErrorCode::SECURE_PROCESSOR_FAIL);
    }

    // Loop over numEntries and process one entry at a time
    for (uint32_t index = 0; index < numEntries; index++) {
        auto entry = cfgInWrapper->getEntryByIndex(index);
        switch (operation) {
            case GET_IMAGE_CONFIG:
                // TBD: To be implemented based on needs
                break;
            case SET_IMAGE_CONFIG:
                T_CALL(_handleImageCfg(sessionId, &entry));
                break;
            case GET_SESSION_CONFIG:
                T_CALL(_createSessionCfg(sessionId, cfgOutWrapper, &entry));
                break;
            case SET_SESSION_CONFIG:
                T_CALL(_handleSessionCfg(sessionId, &entry));
                break;
            default:
                ret = ErrorCode::SECURE_PROCESSOR_BAD_VAL;
                break;
        }
    }

    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    LOG_MSG("ret: %d, op: 0x%x", ret, operation);
    // Delete cfgInWrapper object after detaching the externally set buffer
    if (cfgInWrapper != nullptr) {
        (void)cfgInWrapper->releaseAndGetBuffer();
        // Free the config wrapper class
        delete cfgInWrapper;
    }

    if ((operation & 0x1) == GET_CONFIG) {
        // Populate outConfig HIDL vector
        convertToHidl(cfgOutWrapper, &outConfig);

        // Delete cfgOutWrapper object
        delete cfgOutWrapper;
    }

    return ret;
}

/**
 * resetCamera:
 *
 * Reset camera to clear secure memory of a session that
 * has already been started.
 *
 * @param sessionId Session identifier.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Camera is reset successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Camera reset fails.
 *
 */
ErrorCode SecureProcessor::_resetCamera(uint32_t sessionId)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;

    T_CHECK_ERR(VALID_SESSION(sessionId), ErrorCode::SECURE_PROCESSOR_BAD_VAL);
    T_CHECK_ERR(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_RUNNING,
                ErrorCode::SECURE_PROCESSOR_FAIL);

    sConfig_[sessionId].state = SECURE_PROCESSOR_STATE_RESET;

    T_CHECK_ERR(mSeccam2App->resetCamera() == Object_OK,
                ErrorCode::SECURE_PROCESSOR_FAIL);

    sConfig_[sessionId].state = SECURE_PROCESSOR_STATE_RUNNING;

    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    return ret;
}

/**
 * _handleSessionCfg:
 *
 * Handle configuration for a use case session.
 *
 * @param sessionId Session identifier.
 *
 * @param entry configuration entry of this session.
 *     It contains <tag, value> entry.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Handle session configuration successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Failed to handle session configuration.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid configuration parameter passed.
 *
 */
ErrorCode SecureProcessor::_handleSessionCfg(uint32_t sessionId,
                                             ConfigEntry *entry)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    /* To support customs tags and avoid compiler warnings
       tag should be of type uint32_t*/
    uint32_t tag = entry->tag;
    ConfigType type = (ConfigType)entry->type;

    switch (tag) {
        case (uint32_t)ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_NUM_SENSOR: {
            T_CHECK_ERR(
                sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_INIT,
                ErrorCode::SECURE_PROCESSOR_FAIL);
            T_CHECK_ERR(type == ConfigType::INT32,
                        ErrorCode::SECURE_PROCESSOR_FAIL);
            if (entry->count != 1) {
                LOG_MSG("unexpected count[%d], expected[1] for tag[0x%x]",
                        entry->count, tag);
                break;
            }

            int32_t value = *(entry->data.i32);
            LOG_MSG("TAG: 0x%x, VALUE = %d", tag, value);
            sConfig_[sessionId].numSensor = value;
            ret = ErrorCode::SECURE_PROCESSOR_OK;
            break;
        }
        case (uint32_t)
            ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_USECASE_IDENTIFIER: {
            T_CHECK_ERR(
                sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_INIT,
                ErrorCode::SECURE_PROCESSOR_FAIL);
            T_CHECK_ERR(type == ConfigType::BYTE,
                        ErrorCode::SECURE_PROCESSOR_FAIL);
            uint8_t *value = entry->data.u8;
            uint32_t size = entry->count;

            T_CHECK_ERR(value != NULL, ErrorCode::SECURE_PROCESSOR_FAIL);
            if (0 != memcmp((void *)sConfig_[sessionId].taName.c_str(),
                            (void *)value, size)) {
                // TA name is different or app is not loaded
                sConfig_[sessionId].taName.resize(size);
                common::V1_0::memcpy_s(
                    (void *)(sConfig_[sessionId].taName.data()),
                    sConfig_[sessionId].taName.size(), value, size);
                LOG_MSG("TAG: 0x%x, VALUE = %s", tag, value);

                T_CALL(loadApp(&mSeccam2App,
                               (char *)(sConfig_[sessionId].taName.data())));
            }

            ret = ErrorCode::SECURE_PROCESSOR_OK;
            break;
        }
        case SECURE_PROCESSOR_CUSTOM_CONFIG_USECASE_LICENSE: {
            T_CHECK_ERR(
                sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_INIT,
                ErrorCode::SECURE_PROCESSOR_FAIL);
            T_CHECK_ERR(type == ConfigType::BYTE,
                        ErrorCode::SECURE_PROCESSOR_FAIL);
            uint8_t *value = entry->data.u8;
            uint32_t size = entry->count;

            T_CHECK_ERR(size > 0 && size < MAX_CERT_SIZE,
                        (ErrorCode)SECURE_PROCESSOR_LICENSE_ERROR);

            sConfig_[sessionId].license_size = size;
            common::V1_0::memcpy_s((void *)(sConfig_[sessionId].license),
                                   MAX_CERT_SIZE, value, size);
            LOG_MSG("TAG: 0x%x, VALUE = %s", tag, value);
            ret = ErrorCode::SECURE_PROCESSOR_OK;

            break;
        }
        case SECURE_PROCESSOR_CUSTOM_CONFIG_USECASE_RESET_CAMERA: {
            T_CHECK_ERR(
                sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_RUNNING,
                ErrorCode::SECURE_PROCESSOR_FAIL);

            ret = _resetCamera(sessionId);
            LOG_MSG("TAG: 0x%x, sessionid = %d, ret = %d", tag, sessionId, ret);
            break;
        }
        default:
            LOG_MSG("unexpected TAG: 0x%x, ignored", tag);
            ret = ErrorCode::SECURE_PROCESSOR_BAD_VAL;
            break;
    }

exit:
    return ret;
}

/**
 * _handleImageCfg:
 *
 * Handle configuration for a image type session.
 *
 * @param sessionId Session identifier.
 *
 * @param entry configuration entry of this session.
 *     It contains <tag, value> entry.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Handle image configuration successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Failed to handle image confuguration.
 *
 */
ErrorCode SecureProcessor::_handleImageCfg(uint32_t sessionId,
                                           ConfigEntry *entry)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_OK;
    uint32_t tag = entry->tag;
    ConfigType type = (ConfigType)entry->type;

    switch (tag) {
        case (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_CAMERA_ID: {
            T_CHECK_ERR(type == ConfigType::INT32,
                        ErrorCode::SECURE_PROCESSOR_FAIL);
            T_CHECK_ERR(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int32_t value = *(entry->data.i32);
            LOG_MSG("TAG: 0x%x, VALUE = %d", tag, value);
            sConfig_[sessionId].imgCfg.cameraId = value;
            break;
        }
        case (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_NUMBER: {
            T_CHECK_ERR(type == ConfigType::INT64,
                        ErrorCode::SECURE_PROCESSOR_FAIL);
            T_CHECK_ERR(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int64_t value = *(entry->data.i64);
            LOG_MSG("TAG: 0x%x, VALUE = %ld", tag, value);
            sConfig_[sessionId].imgCfg.frameNum = value;
            break;
        }
        case (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_TIMESTAMP: {
            T_CHECK_ERR(type == ConfigType::INT64,
                        ErrorCode::SECURE_PROCESSOR_FAIL);
            T_CHECK_ERR(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int64_t value = *(entry->data.i64);
            LOG_MSG("TAG: 0x%x, VALUE = %ld", tag, value);
            sConfig_[sessionId].imgCfg.timeStamp = value;
            break;
        }
        case (uint32_t)
            ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_WIDTH: {
            T_CHECK_ERR(type == ConfigType::INT32,
                        ErrorCode::SECURE_PROCESSOR_FAIL);
            T_CHECK_ERR(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int32_t value = *(entry->data.i32);
            LOG_MSG("TAG: 0x%x, VALUE = %d", tag, value);
            sConfig_[sessionId].imgCfg.buffer.width = value;
            break;
        }
        case (uint32_t)
            ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_HEIGHT: {
            T_CHECK_ERR(type == ConfigType::INT32,
                        ErrorCode::SECURE_PROCESSOR_FAIL);
            T_CHECK_ERR(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int32_t value = *(entry->data.i32);
            LOG_MSG("TAG: 0x%x, VALUE = %d", tag, value);
            sConfig_[sessionId].imgCfg.buffer.height = value;
            break;
        }
        case (uint32_t)
            ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_STRIDE: {
            T_CHECK_ERR(type == ConfigType::INT32,
                        ErrorCode::SECURE_PROCESSOR_FAIL);
            T_CHECK_ERR(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int32_t value = *(entry->data.i32);
            LOG_MSG("TAG: 0x%x, VALUE = %d", tag, value);
            sConfig_[sessionId].imgCfg.buffer.stride = value;
            break;
        }
        case (uint32_t)
            ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_FORMAT: {
            T_CHECK_ERR(type == ConfigType::INT32,
                        ErrorCode::SECURE_PROCESSOR_FAIL);
            T_CHECK_ERR(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int32_t value = *(entry->data.i32);
            LOG_MSG("TAG: 0x%x, VALUE = %d", tag, value);
            sConfig_[sessionId].imgCfg.buffer.format = value;
            break;
        }
        default:
            LOG_MSG("unexpected TAG: 0x%x, ignored", tag);
            ret = ErrorCode::SECURE_PROCESSOR_FAIL;
            break;
    }

exit:
    return ret;
}

/**
 * _createSessionCfg:
 *
 * Handle configuration for creating a session.
 *
 * @param sessionId Session identifier.
 *
 * @param cfgWrapper SecureProcessorConfig helper class.
 *
 * @param entry configuration entry for this session.
 *     It contains <tag, value> entry.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Create session configuration successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Failed to create session configuration.
 *
 */
ErrorCode SecureProcessor::_createSessionCfg(uint32_t sessionId,
                                             SecureProcessorConfig *cfgWrapper,
                                             ConfigEntry *entry)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    uint32_t tag = entry->tag;

    switch (tag) {
        case (uint32_t)ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_NUM_SENSOR: {
            cfgWrapper->addEntry(entry->tag, &sConfig_[sessionId].numSensor, 1);
            LOG_MSG("created TAG: 0x%x, VALUE = %d", tag,
                    sConfig_[sessionId].numSensor);
            ret = ErrorCode::SECURE_PROCESSOR_OK;
            break;
        }
        case (uint32_t)
            ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_USECASE_IDENTIFIER: {
            uint8_t *value = (uint8_t *)sConfig_[sessionId].taName.data();
            cfgWrapper->addEntry(entry->tag, value,
                                 sConfig_[sessionId].taName.size());
            LOG_MSG("created TAG: 0x%x, VALUE = %s", tag, value);
            ret = ErrorCode::SECURE_PROCESSOR_OK;
            break;
        }
        default:
            LOG_MSG("unexpected TAG: 0x%x, ignored", tag);
            break;
    }

    return ret;
}

/**
 * _resetImgCfg:
 *
 * Clear the configurations of all session IDs.
 *
 * @param sessionId Session identifier.
 *
 *
 */
void SecureProcessor::_resetImgCfg(uint32_t sessionId)
{
    std::memset(&sConfig_[sessionId].imgCfg, 0,
                sizeof(&sConfig_[sessionId].imgCfg));
}

/**
 * _updatePlanes:
 *
 * Update the plane data for a session.
 *
 * Additionally, the API follows the configuration of a session to update
 * the plane data and get the number of planes. both of plane data buffer
 * the pointer of the number of planes will be populated.
 *
 * @param sessionId Session identifier.
 *
 * @param planeData output plane data buffer.
 *
 * @param numOfInPlanes the number of planes.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Update plane data successfully.
 *
 * @return planeData output plane data buffer.
 *
 * @return numOfInPlanes output the number of planes.
 *
 */
ErrorCode SecureProcessor::_updatePlanes(uint32_t sessionId,
                                         ISecureCamera2App_PlaneInfo *planeData,
                                         uint32_t *numOfInPlanes)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    uint32_t yStride = 0;
    uint32_t uvStride = 0;
    uint32_t uvOffset = 0;
    uint32_t numPlanes = 1;
    ImageBuffer *buffer = &sConfig_[sessionId].imgCfg.buffer;
    bool yuvSP = isYUVSPFormat(buffer->format);

    if (!yuvSP) {
        planeData[0].rowStride = buffer->stride;
        planeData[0].offset = 0;
        planeData[0].pixelStride = 1;
        ret = ErrorCode::SECURE_PROCESSOR_OK;
        goto exit;
    }

    // YUV SemiPlanar
    yStride = calcYStride(buffer->format, buffer->stride);
    uvStride = calcUVStride(buffer->format, buffer->stride);
    uvOffset = calcUVOffset(buffer->format, buffer->stride, buffer->height);
    numPlanes = 3;
    planeData[0].offset = 0;
    planeData[0].rowStride = yStride;
    planeData[0].pixelStride = 1;
    planeData[1].offset = uvOffset;
    planeData[1].rowStride = uvStride;
    planeData[1].pixelStride = 2;
    planeData[2].offset = uvOffset + 1;
    planeData[2].rowStride = uvStride;
    planeData[2].pixelStride = 2;

    *numOfInPlanes = numPlanes;

    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    return ret;
}

/**
 * _processImage:
 *
 * Process secure image data on selected secure destination.
 *
 * Additionally, the API allows set/get of the image specific configuration.
 * The input configuration buffer (inConfig) contains configuration data
 * associated with current image and the output configuration buffer
 * (outConfig) is expected to be populated with new configuration request to
 * be applied to current secure data capture (source) session based on
 * current image data processing on secure destination.
 *
 * The input/output configuration buffers are expected to be prepared
 * using SecureProcessorConfig common helper class.
 *
 * @param sessionId Session identifier.
 *
 * @param image Image handle for secure image data buffer.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Image processing successful.
 *     SECURE_PROCESSOR_FAIL:
 *         Image processing failed.
 *
 */
ErrorCode SecureProcessor::_processImage(uint32_t sessionId,
                                         const hidl_handle &image)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    ISecureCamera2App_BufInfo bufInfo = {};
    ISecureCamera2App_PlaneData *planeData = &bufInfo.planeData[0];
    uint32_t timeTaken = 0;
    int64_t startTime = getTimeUsec();
    int64_t endTime = 0;
    int32_t memFd = -1;
    Object memObj = Object_NULL;
    IMemObject bufRef;
    ITAccessPermissions_rules confRules = {};
    confRules.specialRules = ITAccessPermissions_smmuProxyControlled;

    bufInfo.frameInfo.camId = sConfig_[sessionId].imgCfg.cameraId;
    bufInfo.frameInfo.frameNumber = sConfig_[sessionId].imgCfg.frameNum;
    bufInfo.frameInfo.timeStamp = sConfig_[sessionId].imgCfg.timeStamp;

    T_CALL(_updatePlanes(sessionId, planeData->inPlanes,
                         &planeData->numOfInPlanes));

    memFd = image->data[0];
    memObj = FdWrapper_new(memFd);
    T_CHECK_ERR(!Object_isNull(memObj), ErrorCode::SECURE_PROCESSOR_FAIL);
    if (mCSFVersion_ == CSF_2_5_1) {
        confRules.specialRules = ITAccessPermissions_mixedControlled;
    }

    T_CHECK_ERR(
        Object_isOK(RemoteShareMemory_attachConfinement(&confRules, &memObj)),
        ErrorCode::SECURE_PROCESSOR_FAIL);
    bufRef.consume(memObj);

    T_CHECK_ERR(mSeccam2App->processFrame(bufInfo, bufRef) == Object_OK,
                ErrorCode::SECURE_PROCESSOR_FAIL);

    endTime = getTimeUsec();
    if (endTime >= startTime) {
        timeTaken = (int32_t)(endTime - startTime);
    } else {
        timeTaken = (int32_t)((LLONG_MAX - startTime) + endTime);
    }

    LOG_MSG("_processImage completed, duration: %uus", timeTaken);

    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    return ret;
}

#ifdef DISABLE_CSF_VERSION_CHECK

/**
 * _getCSFVersion:
 *
 * Get CSF version when QTEE secure camera service is not available.
 *
 * @return version output CSF number equals to CSF_2_0.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Getting CSF version successfully.
 *
 */
ErrorCode SecureProcessor::_getCSFVersion(
    CameraSecurityFrameworkVersion_t &version)
{
    version = CSF_2_0;
    return ErrorCode::SECURE_PROCESSOR_OK;
}

#else

/**
 * _getCSFVersion:
 *
 * Get CSF version from QTEE secure camera service.
 *
 * Specifically, the QTEE ISecureCamera2 exposes APIs to HLOS,
 * The secure processor HAL needs to get the version to differentiate
 * which HAL should be valid.
 * (version) is the version number
 *
 * @return version output CSF number.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Getting CSF version successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Getting CSF version failed.
 *
 */
ErrorCode SecureProcessor::_getCSFVersion(
    CameraSecurityFrameworkVersion_t &version)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    Object clientEnv = Object_NULL;
    Object secCamSrvObj = Object_NULL;
    uint32_t archVer_ptr, maxVer_ptr, minVer_ptr;

    if (mCSFVersion_ != CSF_UNKNOWN) {
        version = mCSFVersion_;
        LOG_MSG("Have obtained CSF version");
        return ErrorCode::SECURE_PROCESSOR_OK;
    }

    T_CHECK_ERR(TZCom_getClientEnvObject(&clientEnv) == Object_OK,
                ErrorCode::SECURE_PROCESSOR_FAIL);
    T_CHECK_ERR(IClientEnv_open(clientEnv, CSecureCamera2_UID, &secCamSrvObj) ==
                    Object_OK,
                ErrorCode::SECURE_PROCESSOR_FAIL);
    T_CHECK_ERR(
        ISecureCamera2_getCSFVersion(secCamSrvObj, &archVer_ptr, &maxVer_ptr,
                                     &minVer_ptr) == Object_OK,
        ErrorCode::SECURE_PROCESSOR_FAIL);

    if (archVer_ptr == 2 && maxVer_ptr == 0) {
        version = CSF_2_0;
    } else if (archVer_ptr == 2 && maxVer_ptr == 5 && minVer_ptr == 0) {
        version = CSF_2_5_0;
    } else if (archVer_ptr == 2 && maxVer_ptr == 5 && minVer_ptr == 1) {
        version = CSF_2_5_1;
    } else if (archVer_ptr == 3 && maxVer_ptr == 0) {
        version = CSF_3_0;
    } else {
        version = CSF_UNKNOWN;
    }

    LOG_MSG("CSF version: %d.%d.%d", archVer_ptr, maxVer_ptr, minVer_ptr);
    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    Object_ASSIGN_NULL(secCamSrvObj);
    Object_ASSIGN_NULL(clientEnv);
    return ret;
}
#endif /** DISABLE_CSF_VERSION_CHECK */


/**
 * SecureProcessor:
 *
 * Initialize and clear the configuration table.
 *
 */
SecureProcessor::SecureProcessor()
{
    std::memset(sConfig_, 0, sizeof(sConfig_));
}

/**
 * ~SecureProcessor:
 *
 * Stop and delete all the sessions.
 *
 */

SecureProcessor::~SecureProcessor()
{
    // sessionId start with 1 instead of 0
    for (uint32_t sessionId = 1; sessionId < MAX_SESSION; sessionId++) {
        if (sConfig_[sessionId].active) {
            stopSession(sessionId);
            deleteSession(sessionId);
        }
    }
}

// Methods from ::vendor::qti::hardware::secureprocessor::V1_0::ISecureProcessor
// follow.

/**
 * createSession:
 *
 * Create a new secure data processor session for image data processing.
 * It creates and returns a unique session identifier for subsequent
 * interactions to this session.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         New session created successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         New session creation failed.
 *
 * @return sessionId New session identifier.
 *
 */
Return<void> SecureProcessor::createSession(createSession_cb _hidl_cb)
{
    uint32_t sessionId = 0;
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    char *taName = const_cast<char *>(mTaName_.c_str());
    std::lock_guard<std::mutex> lock(mLock_);

    // sessionId start with 1 instead of 0
    for (sessionId = 1; sessionId < MAX_SESSION; sessionId++) {
        if (sConfig_[sessionId].active) {
            continue;
        }

        T_CALL(loadApp(&mSeccam2App, taName));

        sConfig_[sessionId].active = true;
        sConfig_[sessionId].state = SECURE_PROCESSOR_STATE_INIT;
        sConfig_[sessionId].taName.resize(sizeof(taName));
        common::V1_0::memcpy_s((void *)(sConfig_[sessionId].taName.data()),
                               sConfig_[sessionId].taName.size(), taName,
                               sizeof(taName));
        // Use default license
        sConfig_[sessionId].license_size = sizeof(gLicense);
        common::V1_0::memcpy_s(sConfig_[sessionId].license, MAX_CERT_SIZE,
                               gLicense, sizeof(gLicense));
        break;
    }

    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    _hidl_cb(ret, sessionId);
    return Void();
}

/**
 * setConfig:
 *
 * Set session configuration. The configuration buffer (inConfig) contains
 * set of pairs having <tag, value> entries.
 * The configuration buffer is expected to be prepared using
 * SecureProcessorConfig common helper class.
 *
 * @param sessionId Session identifier.
 *
 * @param inConfig Input configuration buffer.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Session configuration applied successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Session configuration failed to apply.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid configuration parameter passed.
 *
 */
Return<ErrorCode> SecureProcessor::setConfig(uint32_t sessionId,
                                             const hidl_vec<uint8_t> &inConfig)
{
    std::lock_guard<std::mutex> lock(mLock_);
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;

    T_CHECK_ERR(VALID_SESSION(sessionId), ErrorCode::SECURE_PROCESSOR_BAD_VAL);

    ret = _processConfig(sessionId, SET_SESSION_CONFIG, inConfig);

exit:
    return ret;
}

/**
 * getConfig:
 *
 * Get session configuration. The input configuration buffer (inConfig)
 * contains set of required tag entries. The output configuration buffer
 * (outConfig) is populated with set of pairs having <tag, value> entries
 * for requested configuration tags. Both input and output configuration
 * buffers are expected to be prepared using SecureProcessorConfig common
 * helper class.
 *
 * @param sessionId Session identifier.
 *
 * @param inConfig Input configuration buffer.
 *     It contains required tags to be queried.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Session configuration get successful.
 *     SECURE_PROCESSOR_FAIL:
 *         Session configuration get failed.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid configuration parameter passed.
 *
 * @return outConfig Output configuration buffer.
 *     It contains set of <tag, value> pairs for requested tags.
 *
 */
Return<void> SecureProcessor::getConfig(uint32_t sessionId,
                                        const hidl_vec<uint8_t> &inConfig,
                                        getConfig_cb _hidl_cb)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    SecureProcessorCfgBuf outConfig;
    std::lock_guard<std::mutex> lock(mLock_);

    if (!VALID_SESSION(sessionId)) {
        LOG_MSG("unexpected sessionId[%d]", sessionId);
        _hidl_cb(ErrorCode::SECURE_PROCESSOR_BAD_VAL, outConfig);
        return Void();
    }

    ret = _processConfig(sessionId, GET_SESSION_CONFIG, inConfig, outConfig);

    _hidl_cb(ret, outConfig);
    return Void();
}

/**
 * startSession:
 *
 * Start requested session. This API allocates essential resources on secure
 * destination and makes them ready for secure data processing.
 * The mandatory session configs are expected to be set before calling
 * this API.
 *
 * @param sessionId Session identifier.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Session started successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Session start failed.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid parameter passed.
 *
 */
Return<ErrorCode> SecureProcessor::startSession(uint32_t sessionId)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    uint8_t cert[MAX_CERT_SIZE];
    uint32_t certSize;
    std::lock_guard<std::mutex> lock(mLock_);

    T_CHECK_ERR(VALID_SESSION(sessionId), ErrorCode::SECURE_PROCESSOR_BAD_VAL);
    T_CHECK_ERR(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_INIT,
                ErrorCode::SECURE_PROCESSOR_FAIL);

    // Process any session specific configs as necessary
    // Set License should be first
    T_CHECK_ERR(sConfig_[sessionId].license_size < MAX_CERT_SIZE,
                (ErrorCode)SECURE_PROCESSOR_LICENSE_ERROR);

    certSize = sConfig_[sessionId].license_size;
    common::V1_0::memcpy_s(cert, MAX_CERT_SIZE, sConfig_[sessionId].license,
                           certSize);

    T_CHECK_ERR(mSeccam2App->initialize(sConfig_[sessionId].numSensor,
                                        (uint8_t *)cert, certSize) == Object_OK,
                ErrorCode::SECURE_PROCESSOR_FAIL);

    // Set State RUNNING
    sConfig_[sessionId].state = SECURE_PROCESSOR_STATE_RUNNING;

    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    return ret;
}

/**
 * processImage:
 *
 * Process secure image data on selected secure destination.
 *
 * Additionally, the API allows set/get of the image specific configuration.
 * The input configuration buffer (inConfig) contains configuration data
 * associated with current image and the output configuration buffer
 * (outConfig) is expected to be populated with new configuration request to
 * be applied to current secure data capture (source) session based on
 * current image data processing on secure destination.
 *
 * The input/output configuration buffers are expected to be prepared
 * using SecureProcessorConfig common helper class.
 *
 * @param sessionId Session identifier.
 *
 * @param image Image handle for secure image data buffer.
 *
 * @param inConfig Input configuration buffer.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Image processing successful.
 *     SECURE_PROCESSOR_FAIL:
 *         Image processing failed.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid parameter passed.
 *     SECURE_PROCESSOR_NEED_CALIBRATE:
 *         Image processing successful. Additionally, client need to
 *         process outConfig for image source calibration.
 *
 * @return outConfig Output configuration buffer.
 *
 */
Return<void> SecureProcessor::processImage(uint32_t sessionId,
                                           const hidl_handle &image,
                                           const hidl_vec<uint8_t> &inConfig,
                                           processImage_cb _hidl_cb)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    SecureProcessorCfgBuf outConfig;
    std::lock_guard<std::mutex> lock(mLock_);

    if (!VALID_SESSION(sessionId)) {
        LOG_MSG("unexpected sessionId[%d]", sessionId);
        ret = ErrorCode::SECURE_PROCESSOR_BAD_VAL;
        goto exit;
    }

    if (sConfig_[sessionId].state != SECURE_PROCESSOR_STATE_RUNNING) {
        LOG_MSG("bad state: current = %d, expected = %d",
                sConfig_[sessionId].state, SECURE_PROCESSOR_STATE_RUNNING);
        goto exit;
    }

    // Reset old image config
    _resetImgCfg(sessionId);

    T_CALL(_processConfig(sessionId, SET_IMAGE_CONFIG, inConfig));

    // Process image on requested secure destination w/ requested inCfg
    T_CALL(_processImage(sessionId, image));

// TBD: Prepare outCfg based on image data processing (if required)

exit:
    _hidl_cb(ret, outConfig);
    return Void();
}

/**
 * stopSession:
 *
 * Stop requested session. This API releases resources on secure destination
 * which were allocated during startSession call.
 * No secure data processing is allowed post this API call.
 * This API is expected to be called after completely stopping the
 * secure data capture requests on source.
 *
 * @param sessionId Session identifier.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Session started successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Session start failed.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid parameter passed.
 *
 */
Return<ErrorCode> SecureProcessor::stopSession(uint32_t sessionId)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    std::lock_guard<std::mutex> lock(mLock_);

    T_CHECK_ERR(VALID_SESSION(sessionId), ErrorCode::SECURE_PROCESSOR_BAD_VAL);
    T_CHECK_ERR(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_RUNNING,
                ErrorCode::SECURE_PROCESSOR_FAIL);

    sConfig_[sessionId].state = SECURE_PROCESSOR_STATE_INIT;

    T_CHECK_ERR(mSeccam2App->shutdown() == Object_OK,
                ErrorCode::SECURE_PROCESSOR_FAIL);

    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    return ret;
}

/**
 * deleteSession:
 *
 * Delete a previously allocated session.
 *
 * @param sessionId Session identifier.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Session started successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Session start failed.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid parameter passed.
 *
 */
Return<ErrorCode> SecureProcessor::deleteSession(uint32_t sessionId)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    std::lock_guard<std::mutex> lock(mLock_);

    T_CHECK_ERR(VALID_SESSION(sessionId), ErrorCode::SECURE_PROCESSOR_BAD_VAL);
    T_CHECK_ERR(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_INIT,
                ErrorCode::SECURE_PROCESSOR_FAIL);

    std::memset(&sConfig_[sessionId], 0, sizeof(sConfig_[0]));

    T_CALL(unLoadApp(mSeccam2App));
    mSeccam2App = nullptr;

    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    return ret;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace device
}  // namespace secureprocessor
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
