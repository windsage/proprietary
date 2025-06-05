/***********************************************************************
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/
#ifndef _REMOTE_SHARE_MEMORY_H_
#define _REMOTE_SHARE_MEMORY_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "VmOsal.h"
#include "object.h"
#include "ITAccessPermissions.h"

/////////////////////////////////////////////
//      RemoteShareMemory definition     ////
/////////////////////////////////////////////

int32_t RemoteShareMemory_attachConfinement(
  const ITAccessPermissions_rules *userRules,
  Object *memObj);

#if defined(__cplusplus)
}
#endif

#endif
