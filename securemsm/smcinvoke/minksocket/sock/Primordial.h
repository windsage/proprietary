/********************************************************************
 Copyright (c) 2022 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#ifndef _Primordial_H
#define _Primordial_H

#include "object.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef int32_t (*RegHndFunc)(Object target, Object handler, Object *releaser);

typedef struct Primordial Primordial;

int32_t Primordial_setRegisterHandlerFunc(Object *pmdObj, RegHndFunc func);
bool isPrimordialOrPrimordialFwd(Object obj);
int32_t Primordial_new(Object *objOut);

#if defined (__cplusplus)
}
#endif

#endif //_Primordial_H