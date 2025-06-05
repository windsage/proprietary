/********************************************************************
---------------------------------------------------------------------
 Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
Not a contribution.

 *  Copyright (C) 2012 The Android Open Source Project
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you
 *  may not use this file except in compliance with the License.  You may
 *  obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *  implied.  See the License for the specific language governing
 *  permissions and limitations under the License.
----------------------------------------------------------------------*/

/*===========================================================================
                      EDIT HISTORY FOR FILE
when       who     what, where, why
--------   ---     ----------------------------------------------------------
6/10/20    sn      Added sanity checks and FID,License value changes
11/28/19   yb      Initial version
===========================================================================*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <BufferAllocator/BufferAllocatorWrapper.h>
#include <linux/dma-buf.h>
#include <ion/ion.h>
#include <utils/Log.h>
#include "common_log.h"
#include "QSEEComAPI.h"
#include <sys/mman.h>
#include "comdef.h"

/** adb log */
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "SP_LICENSE_SAMPLE: "
#ifdef LOG_NDDEBUG
#undef LOG_NDDEBUG
#endif
#define LOG_NDDEBUG 0 //Define to enable LOGD
#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#endif
#define LOG_NDEBUG  0 //Define to enable LOGV



#define SPU_LICENSE_INSTALL_REQUEST                          1

#define ION_BUFFER_LENGTH (4*1024)
/* Error code: status sent as response to command from app client*/

#define SPU_LICENSE_SUCCESS                                 0
#define SPU_LICENSE_GENERAL_FAILED                         -2
#define SPU_LICENSE_INVALID_LICENSE_FAILED                 -3
#define SPU_LICENSE_INVALID_PARAM_FAILED                   -4
#define SPU_LICENSE_UNSUPPORTED_CMD_FAILED                 -5

#define SPU_LICENSE_APP_VERSION_MAJOR 0x1
#define SPU_LICENSE_APP_VERSION_MINOR 0x0

#pragma pack (push, 1)

 typedef struct sp_install_license_cmd{
        uint32_t cmd_id;
        uint32_t feature_id;
        uint64_t license_addr;
        uint32_t license_len;
  } sp_install_license_cmd_t;


typedef struct sp_install_license_rsp{
    int32_t status;
 } sp_install_license_rsp_t;

struct qsc_ion_info {
     int32_t ifd_data_fd;
     unsigned char * ion_sbuffer;
     uint32_t sbuf_len;
 };

 struct qsc_send_cmd_rsp {
   uint32_t data;
   int32_t status;
 };

#pragma pack (pop)
//---------------------------------------------------------------------
size_t get_file_size(const char *path)
{
    int ret;
    struct stat st = {0};

    if (!path) {
        LOGE("invalid param.\n");
        return 0;
    }

    ret = stat(path, &st);
    if (ret != 0) {
        LOGE("stat() file [%s] failed [%d]\n", path, ret);
        return 0;
    }

    return (size_t) st.st_size;
}

//-----------------------------------------------------------------------------
// return file size on sucess , negative value on error
static int read_file_to_buf(const char *path, void* buf, size_t buf_size)
{
    int ret = 0;
    int fd = -1;
    size_t file_size = 0;
    int err = 0;

    file_size = get_file_size(path);
    if (file_size <= 0) {
        LOGD("can't get file [%s] size.\n", path);
        return -EINVAL;
    }

    if (file_size > buf_size) {
        LOGD("file [%s] size [0x%x] > buf size [0x%x].\n", path, (int) file_size, (int) buf_size);
        return -EINVAL;
    }

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        LOGE("open() file [%s] fd [%d] errno [%d] [%s]", path, fd, errno, strerror(errno));
        return -errno;
    }

    ret = read(fd, buf, buf_size);
    err = errno; // errno is overwritten by close() syscall.
    close(fd);

    if (ret < 0) {
        LOGE("read() file [%s] ret [%d] errno [%d] [%s]", path, ret, err, strerror(err));
        return -err;
    }

    // KW doesn't know that file read() return max of buf_size, so potentially it can returns a bigger size.
    if (ret > buf_size) {
        LOGD("invalid read() size [%d].\n", ret);
        return -EINVAL;
    }

    return file_size;
}
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------
// Copied from qseecom_sample_client.c
static int32_t qsc_ION_memalloc(struct qsc_ion_info *handle, uint32_t size)
{
    int32_t buffer_fd = -1, ret = -1;
    struct dma_buf_sync buf_sync;
    unsigned char *v_addr = NULL;
    uint32_t len = (size + 4095) & (~4095);

    BufferAllocator* bufferAllocator = CreateDmabufHeapBufferAllocator();
    if (bufferAllocator == NULL) {
        LOGE("CreateDmabufHeapBufferAllocator() failed.\n");
        goto alloc_fail;
    }

    ret = MapDmabufHeapNameToIonHeap (bufferAllocator,
                                  "qcom,qseecom",
                                  "qsecom",
                                  0, // ion heap flags
                                  0, // ION_HEAP(ION_QSECOM_HEAP_ID),
                                  0); //dmabuf heap flag
    if (ret < 0) {
         LOGE("MapDmabufHeapNameToIonHeap() failed\n");
         goto alloc_fail;
    }

    buffer_fd = DmabufHeapAlloc(bufferAllocator, "qcom,qseecom", len, 0, 0);

    if (buffer_fd < 0) {
        LOGE("Error: DMA-Buf allocation failed len %d, errno = %d\n", len, errno);
        goto alloc_fail;
    }
    v_addr = (unsigned char *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, buffer_fd, 0);
    if (v_addr == MAP_FAILED) {
        LOGE("Error: MMAP failed: len %d, errno = %d\n", len, errno);
        goto map_fail;
    }

    handle->ifd_data_fd = buffer_fd;
    handle->ion_sbuffer = v_addr;
    handle->sbuf_len = size;

    return ret;

sync_fail:
    if (v_addr) {
        munmap(v_addr, len);
        handle->ion_sbuffer = NULL;
    }
map_fail:
    if (handle->ifd_data_fd >=0)
        close(handle->ifd_data_fd);
alloc_fail:
    handle->ifd_data_fd = -1;
    return ret;
}

static int qsc_ion_dealloc(struct qsc_ion_info *handle)
{
    struct dma_buf_sync buf_sync;
    uint32_t len = (handle->sbuf_len + 4095) & (~4095);
    int ret = 0;

    if (handle->ion_sbuffer) {
        munmap(handle->ion_sbuffer, len);
        handle->ion_sbuffer = NULL;
    }
    if (handle->ifd_data_fd >= 0 ) {
        close(handle->ifd_data_fd);
        handle->ifd_data_fd= -1;
    }

    return ret;
}

int qsc_sync_start(int dma_buf_fd)
{
    int ret = 0;
    struct dma_buf_sync buf_sync = {0};

    buf_sync.flags = DMA_BUF_SYNC_START | DMA_BUF_SYNC_RW;
    ret = ioctl(dma_buf_fd, DMA_BUF_IOCTL_SYNC, &buf_sync);
    if (ret) {
        LOGE("Failed DMA_BUF_IOCTL_SYNC START, fd [%d] ret [%d]\n", dma_buf_fd, ret);
        return -EFAULT;
    }

    LOGD("ioctl(DMA_BUF_IOCTL_SYNC) START OK, dma_buf_fd [%d].\n", dma_buf_fd);

    return 0;
}

int qsc_sync_end(int dma_buf_fd)
{
    int ret = 0;
    struct dma_buf_sync buf_sync = {0};

    buf_sync.flags = DMA_BUF_SYNC_END | DMA_BUF_SYNC_RW;
    ret = ioctl(dma_buf_fd, DMA_BUF_IOCTL_SYNC, &buf_sync);
    if (ret) {
        LOGE("Failed DMA_BUF_IOCTL_SYNC END, fd [%d] ret [%d]\n", dma_buf_fd, ret);
        return -EFAULT;
    }

    LOGD("ioctl(DMA_BUF_IOCTL_SYNC) END OK, dma_buf_fd [%d].\n", dma_buf_fd);

    return 0;
}


 /**@brief:  Implement simple application start
  *
  * @param[in/out]    handle.
  * @param[in]        appname.
  * @param[in]        buffer size.
  * @return    zero on success or error count on failure.
  */
 int32_t qsc_start_app(struct QSEECom_handle **l_QSEEComHandle,
                         const char *appname, int32_t buf_size)
 {
     int32_t ret = 0;

     /* start the application */
    /* load split images */
    ret = QSEECom_start_app(l_QSEEComHandle, "/vendor/firmware_mnt/image", appname, buf_size);

     if (ret) {
        LOGE("Loading app [%s] failed err:%d",appname,ret);
     } else {
        LOGD("Loading app [%s] succeed",appname);
     }
     return ret;
 }

 /**@brief:  Implement simple shutdown app
  * @param[in]    handle.
  * @return    zero on success or error count on failure.
  */
 int32_t qsc_shutdown_app(struct QSEECom_handle **l_QSEEComHandle)
 {
     int32_t ret = 0;

     LOGD("qsc_shutdown_app: start");
     /* shutdown the application */
     if (*l_QSEEComHandle != NULL) {
        ret = QSEECom_shutdown_app(l_QSEEComHandle);
        if (ret) {
           LOGE("Shutdown app failed with ret = %d", ret);
        } else {
           LOGD("shutdown app: pass");
        }
     } else {
         LOGE("cannot shutdown as the handle is NULL");
     }
     return ret;
 }

int main(int argc, char *argv[])
{
    struct QSEECom_handle *l_QSEEComHandle = NULL;
    struct QSEECom_ion_fd_info  ion_fd_info;
    struct qseecom_app_info app_info;

    sp_install_license_cmd_t* cmd;
    sp_install_license_rsp_t* rsp;
    struct qsc_ion_info ihandle;

    char app_name[20] = "sp_license";
    int32_t ret = -1;
    int32_t cmd_ret = -1;
    int32_t req_len = sizeof(sp_install_license_cmd_t);
    int32_t rsp_len = sizeof(sp_install_license_rsp_t);
    char *pfm_file_path = NULL; // License File Path
    int feature_id = 0;

    if (argc < 3) {
           printf("USAGE:\n");
           printf("       sp_license_sample <path to license PFM file> <featre id>\n");
           return 0;
    }

    pfm_file_path = argv[1];
    LOGD("pfm_file_path [%s]\n", pfm_file_path);
    feature_id = atoi(argv[2]);
    LOGD("feature_id [%d]\n", feature_id);

    ret = qsc_start_app(&l_QSEEComHandle,app_name, 1024);
    if (ret)
    {
      LOGE("Start app: fail");
      return -1;
    }

    memset(&ihandle, 0, sizeof(ihandle));
    ret = qsc_ION_memalloc(&ihandle, ION_BUFFER_LENGTH);
    if (ret) {
        LOGD("Error allocating memory in ion\n");
        return -1;
    }
    memset(&ion_fd_info, 0, sizeof(struct QSEECom_ion_fd_info));

    ret = QSEECom_get_app_info(l_QSEEComHandle, &app_info);
    if (ret) {
        LOGE("Error to get app info\n");
        goto out;
    }

    if (!app_info.is_secure_app_64bit) {
        LOGD("send modified cmd to 32bit app - Invalid app info.");
        goto out;
    }

    cmd=(sp_install_license_cmd_t *)l_QSEEComHandle->ion_sbuffer;
    cmd->cmd_id = SPU_LICENSE_INSTALL_REQUEST;
    cmd->feature_id = feature_id;
    cmd->license_len = get_file_size(pfm_file_path);
    LOGD("PFM License Size [%d]", cmd->license_len);

    ion_fd_info.data[0].fd = ihandle.ifd_data_fd;
    ion_fd_info.data[0].cmd_buf_offset = 2*sizeof(uint32_t);

    req_len = sizeof(sp_install_license_cmd_t);

    if (req_len & QSEECOM_ALIGN_MASK)
            req_len = QSEECOM_ALIGN(req_len);

    if (rsp_len & QSEECOM_ALIGN_MASK)
            rsp_len = QSEECOM_ALIGN(rsp_len);

    rsp = (sp_install_license_rsp_t *)l_QSEEComHandle->ion_sbuffer + req_len;
    rsp->status = 0;

    // Sync Start shared buffer - invalidate cache
    qsc_sync_start(ihandle.ifd_data_fd);
    LOGD("Shared-buffer Sync-Start");

    // reset shared buffer
    memset(ihandle.ion_sbuffer, 0, ION_BUFFER_LENGTH);

    // Read license to shared buffer
    ret = read_file_to_buf(pfm_file_path, ihandle.ion_sbuffer, ION_BUFFER_LENGTH);
    if (ret < 0)
        return ret;

    // Sync End shared buffer - invalidate cache
    qsc_sync_end(ihandle.ifd_data_fd);
    LOGD("Shared Buffer Sync-End");

    /* send request from HLOS to sp_license QSEEApp */
    LOGD("Call QSEECom_send_modified_cmd_64()");
    cmd_ret = QSEECom_send_modified_cmd_64(l_QSEEComHandle, cmd,
                req_len, rsp,
                rsp_len, &ion_fd_info);
    if (cmd_ret < 0) {
        LOGE("QSEECom_send_modified_cmd_64() failed, ret [%d] errno [%d]", cmd_ret, errno);
    } else {
        LOGD("QSEECom_send_modified_cmd_64() completed ok.");

        if (rsp->status != 0) {
            cmd_ret = rsp->status; // return the error code
            LOGE("Install License failed, rsp->status [0x%x]", rsp->status);
        } else {
            LOGD("Install License completed ok");
        }
    }

out:
    ret = qsc_ion_dealloc(&ihandle);
    if (ret) {
        LOGD("Error de-allocating ion memory\n");
    }

    ret = qsc_shutdown_app(&l_QSEEComHandle);
    if (ret) {
        LOGE("Failed to shutdown app: %d", ret);
    }

    exit(cmd_ret); // app exit code

    return cmd_ret;
}
