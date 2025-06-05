/**
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "SECURE-PROCESSOR-TVM-MINK"

#include "SecureProcessorTVMMink.h"
#include <sys/mman.h>
#include "BufferAllocator/BufferAllocatorWrapper.h"
#include "CTProcessLoader.h"
#include "IModule.h"
#include "ITProcessLoader.h"
#include "LinkCredentials.h"
#include "fdwrapper.h"
#include "utils.h"
#include "vmuuid.h"
#include "xtzdCredentials.h"
#include "ITAccessPermissions.h"
#include "RemoteShareMemory.h"

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
#define MINK_QRTR_LE_SERVICE 5008
#define MINK_QRTR_LE_SERVICE_OEMVM 5010
// membuf doesn't map physical pages without 2mb alignment
#define SIZE_2MB 0x200000

static uint32_t seccamUID = 293;

/**
 * _readAppFromFile:
 *
 * Read a file to a DMA heap buffer and get its buffer file descriptor.
 *
 * Additionally, the API reads the file from given paths to a DMA
 * heap buffer, and gets the buffer file descriptor.
 *
 * @param appName input the APP name.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Read the TA file and get DMA buffer FD successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Fail to read the TA file or get DMA buffer FD.
 *
 * @return appSize output the TA file size.
 *
 * @return nsBufferFd output DMA heap buffer file descriptor.
 *
 */
ErrorCode SecureProcessorTVMMink::_readAppFromFile(std::string appName,
                                                   size_t *appSize,
                                                   int32_t *nsBufferFd)
{
    int32_t actualRead = -1, bufferFd = -1, appFd = -1;
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    struct stat appStat = {};
    BufferAllocator *bufferAllocator = nullptr;
    unsigned char *vAddr = nullptr;
    std::string appPath;

    T_CHECK_ERR(0 == getSecCamAppPath(appName, appPath),
                ErrorCode::SECURE_PROCESSOR_FAIL);
    LOG_MSG("getSecCamAppPath returned path = %s", appPath.c_str());

    appFd = open(appPath.c_str(), O_RDONLY);
    T_CHECK_ERR(appFd > 0, ErrorCode::SECURE_PROCESSOR_FAIL);

    // app size
    T_CHECK_ERR(0 == fstat(appFd, &appStat), ErrorCode::SECURE_PROCESSOR_FAIL);
    T_CHECK_ERR((appStat.st_size) > 0, ErrorCode::SECURE_PROCESSOR_FAIL);
    (*appSize) = appStat.st_size;

    bufferAllocator = CreateDmabufHeapBufferAllocator();
    T_CHECK_ERR(bufferAllocator != nullptr, ErrorCode::SECURE_PROCESSOR_FAIL);

    bufferFd =
        DmabufHeapAlloc(bufferAllocator, "qcom,display",
                        (*appSize + (SIZE_2MB - 1)) & (~(SIZE_2MB - 1)), 0, 0);
    T_CHECK_ERR(bufferFd >= 0, ErrorCode::SECURE_PROCESSOR_FAIL);

    FreeDmabufHeapBufferAllocator(bufferAllocator);

    vAddr = (unsigned char *)mmap(NULL, *appSize, PROT_READ | PROT_WRITE,
                                  MAP_SHARED, bufferFd, 0);
    T_CHECK_ERR(vAddr != MAP_FAILED, ErrorCode::SECURE_PROCESSOR_FAIL);

    // Read app image into mmap'd region
    actualRead = read(appFd, vAddr, *appSize);
    T_CHECK_ERR(munmap(vAddr, *appSize) == 0, ErrorCode::SECURE_PROCESSOR_FAIL);
    T_CHECK_ERR(actualRead == (*appSize), ErrorCode::SECURE_PROCESSOR_FAIL);

    *nsBufferFd = bufferFd;
    close(appFd);

    LOG_MSG(
        "Successfull, appsize = %x,"
        "buf_fd = %d, vaddr = %x",
        (*appSize), bufferFd, vAddr);

    return ErrorCode::SECURE_PROCESSOR_OK;

exit:
    if (bufferFd >= 0) {
        close(bufferFd);
    }

    if (bufferAllocator) {
        FreeDmabufHeapBufferAllocator(bufferAllocator);
    }

    if (appFd > 0) {
        close(appFd);
    }

    return ret;
}

/**
 * SecureProcessorTVMMink:
 *
 * Initialize member variable mTaName_ of base class SecureProcessor.
 *
 * @param taName TA Name.
 *
 */
SecureProcessorTVMMink::SecureProcessorTVMMink(string taName)
{
    mTaName_ = taName;
}

/**
 * loadApp:
 *
 * Load a TVM TA and get the APP object.
 *
 * Additionally, the API reads the file from the given paths to a DMA heap
 * buffer, and loads the TA from the buffer to TVM with TProcessLoader.
 * once it loads the APP successfully, it will get the APP object.
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
ErrorCode SecureProcessorTVMMink::loadApp(ISecureCamera2App **appObj,
                                          char *appName)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    CredInfo credInfo = {0, 0, 0};
    credInfo.pid = getpid();
    int32_t nsBufferFd = -1;
    Object nsMemoryObj = Object_NULL;  // non-secure mem object
    size_t appSize = 0;
    Object procLoaderObj = Object_NULL;
    Object remoteAppObject = Object_NULL;
    int32_t addrValue;
    int32_t errCode = 0;
    CameraSecurityFrameworkVersion_t version = CSF_UNKNOWN;
    ITAccessPermissions_rules confRules = {};

    if (!appName || !appObj) {
        LOG_MSG("Invalid input.");
        return ret;
    }

    if (!Object_isNull(mProcObj)) {
        LOG_MSG("Unload TA(%s) if it is already loaded.", appName);
        unLoadApp(*appObj);
    }

    if (ErrorCode::SECURE_PROCESSOR_OK == _getCSFVersion(version)) {
        mCSFVersion_ = version;
    } else {
        LOG_MSG("Failed to get the CSF version.");
    }

    if (!(CSF_2_5_0 == mCSFVersion_ || CSF_2_5_1 == mCSFVersion_)) {
        LOG_MSG("Incorrect CSF Version! Please make sure of the DEVCFG image.");
        return ret;
    }

    if (mCSFVersion_ == CSF_2_5_0) {
        addrValue = MINK_QRTR_LE_SERVICE;
    } else if (mCSFVersion_ == CSF_2_5_1) {
        addrValue = MINK_QRTR_LE_SERVICE_OEMVM;
    } else {
        LOG_MSG("Incorrect CSF Version! Please make sure of the DEVCFG image.");
        return ret;
    }

    // open vm interface
    LOG_MSG("Connect to VM server service");
    mVMIntf = MinkIPC_connect_QRTR(addrValue, &mTvmAppOpener);
    if (mVMIntf == nullptr || Object_isNull(mTvmAppOpener)) {
        LOG_MSG(
            "error in connection to tvmmink, service id: %d, TUIVMIntf is "
            "null: %d, opener is "
            "null: %d",
            addrValue, mVMIntf == nullptr,
            Object_isNull(mTvmAppOpener));
        mVMIntf = nullptr;
        return ErrorCode::SECURE_PROCESSOR_FAIL;
    }

    LOG_MSG("Got opener handle from TVM server");

    //Create LinkCredentials
    T_CHECK_ERR(
         0 == LinkCred_new(&credInfo, ELOC_LOCAL, UNIX, &mLinkCredentials),
         ErrorCode::SECURE_PROCESSOR_FAIL);

    T_CALL(_readAppFromFile(appName, &appSize, &nsBufferFd));

    nsMemoryObj = FdWrapper_new(nsBufferFd);
    T_CHECK_ERR(!Object_isNull(nsMemoryObj), ErrorCode::SECURE_PROCESSOR_FAIL);

    // Memory sent to QTVM/OEMVM ProcessLoader should be through MEM_LEND.
    confRules.specialRules = ITAccessPermissions_removeSelfAccess;
    T_CHECK_ERR(0 == RemoteShareMemory_attachConfinement(&confRules, &nsMemoryObj),
                ErrorCode::SECURE_PROCESSOR_FAIL);

    LOG_MSG("Load the App from TVM side");
    errCode = IModule_open(mTvmAppOpener, CTProcessLoader_UID, mLinkCredentials,
                           &procLoaderObj);
    T_CHECK_ERR((errCode == 0) ||
                    (errCode == ITProcessLoader_ERROR_PROC_ALREADY_LOADED),
                ErrorCode::SECURE_PROCESSOR_FAIL);

    T_CHECK_ERR(0 == ITProcessLoader_loadFromBuffer(procLoaderObj, nsMemoryObj,
                                                    &mProcObj),
                ErrorCode::SECURE_PROCESSOR_FAIL);

    T_CHECK_ERR(0 == IModule_open(mTvmAppOpener, seccamUID, mLinkCredentials,
                                  &remoteAppObject),
                ErrorCode::SECURE_PROCESSOR_FAIL);

    *appObj = new ISecureCamera2App(remoteAppObject);
    T_CHECK_ERR(*appObj != nullptr, ErrorCode::SECURE_PROCESSOR_FAIL);

    LOG_MSG("LE has succesfully loaded the App(%s) in SDK", appName);

    ret = ErrorCode::SECURE_PROCESSOR_OK;

exit:
    if (ret != ErrorCode::SECURE_PROCESSOR_OK) {
        unLoadApp(*appObj);
        *appObj = nullptr;
    }

    Object_ASSIGN_NULL(procLoaderObj);
    Object_ASSIGN_NULL(nsMemoryObj);

    return ret;
}

/**
 * unLoadApp:
 *
 * Unload a TA from the TVM.
 *
 * Additionally, the API unloads a TVM TA synchronously, the APP object(appObj)
 * is passed, and it will be released first, and then the whole TVM TA will
 * be unloaded immediately.
 *
 * @param appObj input the APP object.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Unload the TA successfully.
 *
 */
ErrorCode SecureProcessorTVMMink::unLoadApp(ISecureCamera2App *appObj)
{
    if (appObj) {
        LOG_MSG("release appObj");
        delete appObj;
    }

    LOG_MSG("release service");
    Object_ASSIGN_NULL(mProcObj);
    Object_ASSIGN_NULL(mTvmAppOpener);
    Object_ASSIGN_NULL(mLinkCredentials);

    if (mVMIntf != nullptr) {
        MinkIPC_release(mVMIntf);
        mVMIntf = nullptr;
    }

    LOG_MSG("cleanup complete!");
    return ErrorCode::SECURE_PROCESSOR_OK;
}

}  // namespace device
}  // namespace secureprocessor
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
