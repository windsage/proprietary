/***********************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <utils/Log.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include "IAppLoader.h"
#include "object.h"
#include "IClientEnv.h"
#include "CAppLoader.h"

#include <linux/msm_ion.h>
#include <linux/dma-buf.h>
#include <BufferAllocator/BufferAllocatorWrapper.h>

//needed for loadApp
int get_file_size(const char* filename) {
  FILE* file = NULL;
  int size = 0;
  int ret = 0;

  do {
    file = fopen(filename, "r");
    if (file == NULL) {
      ALOGE("Failed to open file %s: %s (%d)\n", filename, strerror(errno), errno);
      size = -1;
      break;
    }

    ret = fseek(file, 0L, SEEK_END);
    if (ret) {
      ALOGE("Error seeking in file %s: %s (%d)\n", filename, strerror(errno), errno);
      size = -1;
      break;
    }

    size = ftell(file);
    if (size == -1) {
      ALOGE("Error telling size of file %s: %s (%d)\n", filename, strerror(errno), errno);
      size = -1;
      break;
    }
  } while (0);
  if (file) {
    fclose(file);
  }
  return size;
}

//needed for loadApp
int read_file(const char* filename, size_t size, uint8_t* buffer) {
  FILE* file = NULL;
  size_t readBytes = 0;
  int ret = 0;
  do {
    file = fopen(filename, "r");
    if (file == NULL) {
      ALOGE("Failed to open file %s: %s (%d)\n", filename, strerror(errno), errno);
      ret = -1;
      break;
    }
    readBytes = fread(buffer, 1, size, file);
    if (readBytes != size) {
      ALOGE("Error reading the file %s: %zu vs %zu bytes: %s (%d)\n",
                filename,
                readBytes,
                size,
                strerror(errno),
                errno);
      ret = -1;
      break;
    }
      ret = size;
  } while (0);
  if (file) {
    fclose(file);
  }
  return ret;
}

int32_t load_app(Object appLoader, char *path, Object *appController)
{
  int32_t ret = Object_OK;
  size_t size       = 0;
  uint8_t * buffer  = NULL;

  do {
    ret = get_file_size(path);
    if (ret <= 0) {
      ret = -1;
      break;
    }
    size = (size_t)ret;
    buffer = malloc(sizeof(uint8_t[size]));
    if (!buffer) {
      ALOGE("Malloc failed while allocating memory to buffer\n");
      ret = Object_ERROR_KMEM;
      break;
    }

    ret = read_file(path, size, buffer);

    if (ret < 0) break;
    ALOGI("load %s, size %zu, buffer %p\n", path, size, buffer);
    ret = IAppLoader_loadFromBuffer(appLoader, buffer, size, appController);

    } while (0);

    if (buffer) free(buffer);

    if (!ret && !Object_isNull(*appController)) {
      ALOGI("Load app %s succeeded\n", path);
    }
    else {
      ALOGE("Load app %s failed: %d\n", path, ret);
      if (Object_isNull(*appController)) {
        ALOGE("appController is NULL!\n");
        ret = -1;
      }
    }

    return ret;
}

int32_t dma_alloc(BufferAllocator* dmaHandle, size_t allocSize, int32_t *fd, unsigned char** v_addr)
{
  int32_t ret = -1;
  int32_t buffer_fd = -1;

  if (dmaHandle == NULL) {
    ALOGE("CreateDmabufHeapBufferAllocator() failed.\n");
    goto alloc_fail;
  }

  buffer_fd = DmabufHeapAlloc(dmaHandle, "qcom,qseecom-ta", allocSize, 0, 0);

  if (buffer_fd < 0) {
    ALOGE("Error: DMA-Buf allocation failed from heap %d, len %d, errno = %d\n",
                                             ION_QSECOM_TA_HEAP_ID, allocSize, errno);
    goto alloc_fail;
  }
  *v_addr =  (unsigned char *)mmap(NULL, allocSize, PROT_READ | PROT_WRITE,
                                             MAP_SHARED, buffer_fd, 0);

  if (*v_addr == MAP_FAILED) {
    ALOGE("mmap failed\n");
    goto alloc_fail;
  }
  *fd = buffer_fd;

  ret = DmabufHeapCpuSyncStart(dmaHandle, buffer_fd, kSyncReadWrite, NULL, NULL);

  if (ret) {
    ALOGE("Error: DMA_BUF_IOCTL_SYNC start failed, ret = %d", ret);
    goto sync_fail;
  }
  return ret;

  sync_fail:
    if (*v_addr != MAP_FAILED) {
      munmap(*v_addr, allocSize);
      *v_addr = MAP_FAILED;
    }

  alloc_fail:
    if (dmaHandle != NULL)
      FreeDmabufHeapBufferAllocator(dmaHandle);
    if (*fd >= 0) {
      close(*fd);
      *fd = -1;
    }
    allocSize = 0;
    return ret;
}

int32_t dma_unalloc(BufferAllocator* dmaHandle, size_t allocSize, int32_t *fd, unsigned char** v_addr)
{
  int32_t ret = -1;

  ret = DmabufHeapCpuSyncEnd(dmaHandle, *fd, kSyncReadWrite, NULL, NULL);
  if (ret) {
    ALOGE("Error: DMA_BUF_IOCTL_SYNC start failed, ret = %d", ret);
  }

  if (*v_addr != MAP_FAILED) {
    munmap(*v_addr, allocSize);
    *v_addr = MAP_FAILED;
  }

  if (*fd >= 0) {
    close(*fd);
    *fd = -1;
  }
  allocSize = 0;

  if (dmaHandle != NULL)
    FreeDmabufHeapBufferAllocator(dmaHandle);

  return ret;
}
