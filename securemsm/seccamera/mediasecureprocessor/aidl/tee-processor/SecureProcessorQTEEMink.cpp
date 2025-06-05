/**
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "SECURE-PROCESSOR-QTEE-MINK"

#include "SecureProcessorQTEEMink.h"
#include "CAppLoader.h"
#include "IAppController.h"
#include "IAppLoader.h"
#include "IClientEnv.h"
#include "IOpener.h"
#include "ISecureCamera2App_invoke.hpp"
#include "TZCom.h"
#include "utils.h"

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
using ::aidl::vendor::qti::hardware::secureprocessor::common::ErrorCode;
/**
 * _getAppController:
 *
 * Load a TA from a file and get the TA's APP controller.
 *
 * Additionally, the API reads the file from a path to a buffer, and
 * loads the TA from the buffer to QTEE. the path is the full path of
 * the TA. once it loads the TA successfully, it will get the APP
 * controller and populate the appController.
 *
 * @param appLoader input the APP loader object.
 *
 * @param path input the full path of the TA.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Load the TA and get the APP controller successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Failed to load the TA or get the APP controller.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid parameter passed.
 *
 * @return appController Output APP controller.
 *
 */
ErrorCode SecureProcessorQTEEMink::_getAppController(Object appLoader,
                                                     std::string const &path,
                                                     Object *appController)
{
    size_t size = 0;
    uint8_t *buffer = NULL;
    int32_t ret = 0;

    if (appController == NULL) {
        LOG_MSG("appController is a null pointer!");
        return ErrorCode::SECURE_PROCESSOR_BAD_VAL;
    }

    do {
        ret = getFileSize(path);
        if (ret <= 0) {
            LOG_MSG("failed to get file(%s) size, ret:%d", path.c_str(), ret);
            break;
        }

        size = (size_t)ret;
        buffer = new uint8_t[size];

        ret = readFile(path, size, buffer);
        if (ret < 0) {
            LOG_MSG("Failed to read %s. ret:%d", path.c_str(), ret);
            break;
        }

        LOG_MSG("load %s, size %zu, buf %p", path.c_str(), size, buffer);

        ret = IAppLoader_loadFromBuffer(appLoader, buffer, size, appController);

    } while (0);

    if (buffer) delete[] buffer;

    if (!Object_isERROR(ret) && !Object_isNull(*appController)) {
        LOG_MSG("Loading app %s succeeded", path.c_str());
        return ErrorCode::SECURE_PROCESSOR_OK;
    } else {
        LOG_MSG("Failed to load app %s: %d", path.c_str(), ret);
    }

    return ErrorCode::SECURE_PROCESSOR_FAIL;
}

/**
 * SecureProcessorQTEEMink:
 *
 * Initialize member variable mTaName_ of base class SecureProcessor.
 *
 * @param taName TA Name.
 *
 */
SecureProcessorQTEEMink::SecureProcessorQTEEMink(string taName)
{
    mTaName_ = taName;
}

/**
 * loadApp:
 *
 * Load a QTEE TA and get the APP object.
 *
 * Additionally, the API reads the file from the given paths to a buffer,
 * and loads the TA from the buffer to QTEE. once it gets the APP controller,
 * it will get the APP object with the APP controller.
 *
 *
 * @param appObj output the APP object.
 *
 * @param path input the name of the QTEE TA.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Load the TA and get the APP object successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Failed to load the TA or get the APP object.
 *
 * @return appObj Output APP object.
 *
 */
ErrorCode SecureProcessorQTEEMink::loadApp(ISecureCamera2App **appObj,
                                           char *appName)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    std::string secCamAppPath;
    Object clientEnv = Object_NULL;
    Object appLoader = Object_NULL;
    Object remoteAppObject = Object_NULL;
    std::string fileName;
    CameraSecurityFrameworkVersion_t version = CSF_UNKNOWN;

    if (!appName || !appObj) {
        LOG_MSG("Invalid input.");
        return ret;
    }

    if (!Object_isNull(mAppController)) {
        LOG_MSG("Unload TA(%s) if it is already loaded.", appName);
        unLoadApp(*appObj);
    }

    if (ErrorCode::SECURE_PROCESSOR_OK == _getCSFVersion(version)) {
        mCSFVersion_ = version;
    } else {
        LOG_MSG("Failed to get the CSF version.");
    }

    if (CSF_2_0 != mCSFVersion_) {
        LOG_MSG("Incorrect CSF Version! Please make sure of the DEVCFG image.");
        return ret;
    }

    fileName = appName;

    LOG_MSG("start to load App: %s", appName);

    T_CHECK_ERR(TZCom_getClientEnvObject(&clientEnv) == Object_OK,
                ErrorCode::SECURE_PROCESSOR_FAIL);
    T_CHECK_ERR(
        IClientEnv_open(clientEnv, CAppLoader_UID, &appLoader) == Object_OK,
        ErrorCode::SECURE_PROCESSOR_FAIL);

    fileName += ".mbn";
    T_CHECK_ERR(0 == getSecCamAppPath(fileName, secCamAppPath),
                ErrorCode::SECURE_PROCESSOR_FAIL);
    LOG_MSG("Foud TA = %s", secCamAppPath.c_str());
    T_CHECK_ERR(_getAppController(appLoader, secCamAppPath, &mAppController) ==
                    ErrorCode::SECURE_PROCESSOR_OK,
                ErrorCode::SECURE_PROCESSOR_FAIL);
    LOG_MSG("App [%s] got the App controller successfully from [%s]", appName,
            secCamAppPath.c_str());

    T_CHECK_ERR(IAppController_getAppObject(mAppController, &remoteAppObject) ==
                    Object_OK,
                ErrorCode::SECURE_PROCESSOR_FAIL);
    *appObj = new ISecureCamera2App(remoteAppObject);
    LOG_MSG("Load %s successfully", appName);
    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    Object_ASSIGN_NULL(appLoader);
    Object_ASSIGN_NULL(clientEnv);

    return ret;
}

/**
 * unLoadApp:
 *
 * Unload a TA from the QTEE.
 *
 * Additionally, the API unloads a QTEE TA synchronously, the APP object(appObj)
 * is passed, and it will be released first, and then the whole QTEE TA will
 * be unloaded immediately.
 *
 * @param appObj input the APP object.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Unload the TA successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Fail to unload the TA.
 *
 */
ErrorCode SecureProcessorQTEEMink::unLoadApp(ISecureCamera2App *appObj)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;

    if (appObj) {
        delete appObj;
    }

    T_CHECK_ERR(IAppController_unload(mAppController) == Object_OK,
                ErrorCode::SECURE_PROCESSOR_FAIL);

    LOG_MSG("Unloaded the TA.");

    ret = ErrorCode::SECURE_PROCESSOR_OK;
    Object_ASSIGN_NULL(mAppController);

exit:
    return ret;
}

}  // namespace device
}  // namespace secureprocessor
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
