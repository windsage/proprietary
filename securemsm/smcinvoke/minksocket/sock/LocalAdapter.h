/********************************************************************
 Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#ifndef __LOCALADAPTER_H
#define __LOCALADAPTER_H

#include "object.h"

#if defined (__cplusplus)
extern "C" {
#endif

int32_t LocalAdapter_new(Object endpoint, Object credentials, Object *objOut);

#if defined (__cplusplus)
}
#endif

#endif //__LOCALADAPTER_H