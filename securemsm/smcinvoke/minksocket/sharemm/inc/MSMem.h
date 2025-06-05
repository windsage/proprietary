/***********************************************************************
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/
#ifndef _MSMEM_H_
#define _MSMEM_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "VmOsal.h"
#include "ITAccessPermissions.h"
#include "cdefs.h"
#include "object.h"

typedef struct {
  int32_t refs;
  int32_t dmaBufFd;
  Object dependency;
  bool isLocal;
  // A callback object to the MemoryService which holds a ref to the pool
  Object bufCallBack;
  ITAccessPermissions_rules confRules;
} MSMem;

int32_t MSMem_new(int32_t fd, Object bufCallBack, Object *objOut);
bool isMSMem(Object obj, int32_t *fd);
int32_t MSMem_new_remote(int32_t fd, ITAccessPermissions_rules *confRules,
                         Object *objOut);
MSMem *MSMemFromObject(Object obj);

#ifdef STUB
int32_t MSChangeToROFd(Object obj, int32_t fd);
#endif

#if defined(__cplusplus)
}
#endif

#endif
