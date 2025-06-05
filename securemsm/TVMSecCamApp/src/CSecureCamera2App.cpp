/*
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cstddef>
#include <cstdint>
#include <new>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "CSecureCamera2App.hpp"
#include "CTAccessControl.h"
#include "CTrustedCameraMemory.h"
#include "ISecureCamera2App_invoke.hpp"
#include "ITEnv.h"
#include "ITMemoryService.h"
#include "object.h"

extern "C" {
#include "seccam.h"
}

/*=======================================================================================
 *                   ISecureCamera2App SMC Invoke Functions
 *=======================================================================================*/
class CSecureCamera2App : public ISecureCamera2AppImplBase
{
   public:
    CSecureCamera2App();
    virtual ~CSecureCamera2App();

    virtual int32_t initialize(uint32_t numSensors,
                               const uint8_t *havenLicensePtr,
                               size_t havenLicenseLen);
    virtual int32_t processFrame(const ISecureCamera2App_BufInfo &bufInfoRef,
                                 const IMemObject &bufRef);
    virtual int32_t resetCamera();
    virtual int32_t shutdown();
};

CSecureCamera2App::CSecureCamera2App()
{
}

CSecureCamera2App::~CSecureCamera2App()
{
}

/* Description: Function to initialize the callback object
 *              set the license and number of sensors
 * In:          numSensors - the number of sensors
 *              havenLicensePtr - pointer containing certificate
 *              havenLicenseLen - size of pointer containing certificate
 * Out:         void
 * Return:      Object_OK on success.
 *              Object_ERROR on failure.
 */
int32_t CSecureCamera2App::initialize(uint32_t numSensors,
                                      const uint8_t *havenLicensePtr,
                                      size_t havenLicenseLen)
{
    int32_t ret = Object_OK;

    const uint8_t *license = NULL;
    size_t licenseSize = 0;

    T_CHECK(open_sc_object() == SC_SUCCESS);

    T_CHECK(register_callback() == SC_SUCCESS);

    T_CHECK(set_sensors(numSensors) == SC_SUCCESS);

    license = havenLicensePtr;
    licenseSize = havenLicenseLen;
    T_CHECK(set_license(license, licenseSize) == SC_SUCCESS);

exit:
    return ret;
}

/* Description: Example to process secure frame.
 *              Should only be called while secure camera streaming active.
 * In:          bufInfoRef - the camera buffer info Struct from HLOS
 *              bufRef - IMemObject class to manage camera memory object
 * Out:         void
 * Return:      Object_OK on success.
 *              Any other error code on failure.
 */
// to-do: move to IMemObject.idl interface for memory objects
int32_t CSecureCamera2App::processFrame(
    const ISecureCamera2App_BufInfo &bufInfoRef, const IMemObject &bufRef)
{
    int32_t ret = Object_OK;

    Object bufObj = Object_NULL;
    Object lockObj = Object_NULL;
    Object tACObject = Object_NULL;
    seccam_data_buf_t dataBuf = {};
    seccam_frame_info_t frameInfo = {};
    int32_t fd = -1;
    uint64_t sz = 0;
    void *ptr = nullptr;
    struct stat memObjStat = {};
    ITAccessPermissions_rules confRules = {0};
    confRules.uidPermsList[0] = {CTrustedCameraMemory_UID, 1};

    bufObj = bufRef.get();
    T_CHECK_ERR(!Object_isNull(bufObj), Object_ERROR_INVALID);

    T_CHECK_ERR(check_camera_state() == SC_SUCCESS,
                ERROR_CAMERA_NOT_PROTECTED);

    T_CALL(ITEnv_open(gTVMEnv, CTAccessControl_UID, &tACObject));
    T_CALL(ITAccessControl_acquireLock(tACObject, &confRules,
                                       bufObj, &lockObj));

    // Get fd
    T_CHECK_ERR(Object_unwrapFd(bufObj, &fd) == Object_OK && fd >= 0,
                Object_ERROR_INVALID);

    T_CHECK(fstat(fd, &memObjStat) == 0);
    T_CHECK_ERR(memObjStat.st_size > 0, Object_ERROR_INVALID);
    sz = memObjStat.st_size;
    ptr = mmap(nullptr, sz, PROT_READ, MAP_SHARED, fd, 0);
    T_CHECK_ERR(ptr != MAP_FAILED, ERROR_MAP_FAILURE);

    dataBuf.buf = ptr;
    dataBuf.buf_size = sz;
    frameInfo.cam_id = bufInfoRef.frameInfo.camId;
    frameInfo.frame_number = bufInfoRef.frameInfo.frameNumber;
    frameInfo.time_stamp = bufInfoRef.frameInfo.timeStamp;
    T_CHECK(process_frame_algo(&dataBuf, &frameInfo, NULL) == Object_OK);

exit:
    Object_ASSIGN_NULL(lockObj);
    Object_ASSIGN_NULL(tACObject);
    if (ptr != nullptr) munmap(ptr, sz);
    return ret;
}

/* Description: Example to reset camera to clear secure memory between
 *              authenticated users.
 * In:          void
 * Out:         void
 * Return:      Object_OK on success.
 *              Object_ERROR on failure.
 */
int32_t CSecureCamera2App::resetCamera()
{
    int32_t ret = Object_OK;

exit:
    return ret;
}

/* Description: Example to release callback and camera objects.
 * In:          void
 * Out:         void
 * Return:      Object_OK on success.
 *              Object_ERROR on failure.
 */
int32_t CSecureCamera2App::shutdown()
{
    int32_t ret = Object_OK;

    seccam_shutdown();

    return ret;
}

/* Description: This function creates a new ISecureCamera2App object.
 * In:          obj - ISecureCamera2App object
 * Out:         void
 * Return:      Object_OK on success.
 *              Object_ERROR_MEM on failure.
 */
int32_t CSecureCamera2App_open(Object *obj)
{
    CSecureCamera2App *me = new (std::nothrow) CSecureCamera2App();
    if (!me) {
        LOG_MSG("Memory allocation for CSecureCamera2App failed!");
        return Object_ERROR_MEM;
    }

    *obj = (Object){ImplBase::invoke, me};

    LOG_MSG("Succeed in getting a camera APP object.");

    return Object_OK;
}
