/********************************************************************
 Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#ifndef __REMOTEADAPTER_H
#define __REMOTEADAPTER_H

#include "object.h"

#if defined (__cplusplus)
extern "C" {
#endif

int32_t RemoteAdapter_new(Object endpoint, Object credentials, Object *objOut);

#if defined (__cplusplus)
}
#endif

#endif //_REMOTEADAPTER_H