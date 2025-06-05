/***********************************************************************
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/

#ifndef _SHARE_MEMORY_H_
#define _SHARE_MEMORY_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "VmOsal.h"
#include "ITAccessPermissions.h"
#include "object.h"

int32_t ShareMemory_GetMemParcelHandle(int32_t dmaBufFd, Object conf,
                                       char *destVMName, int64_t *outMPHandle);

int32_t ShareMemory_GetMSMem(int64_t memparcelHandle, char *destVMName,
                             ITAccessPermissions_rules *confRules,
                             Object *objOut);

int32_t ShareMemory_ReclaimMemBuf(int32_t fd, int64_t memparcelHandle);

#if defined(__cplusplus)
}
#endif

#endif
