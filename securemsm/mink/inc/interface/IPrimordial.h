/********************************************************************
Copyright (c) 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#pragma once

#include <stdbool.h>
#include "object.h"

#define IPrimordial_OP_registerHandler 0

static inline int32_t
IPrimordial_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IPrimordial_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IPrimordial_registerHandler(Object self, Object target, Object handler, Object *releaser)
{
  ObjectArg a[3];
  a[0].o = target;
  a[1].o = handler;

  int32_t result = Object_invoke(self, IPrimordial_OP_registerHandler, a, ObjectCounts_pack(0, 0, 2, 1));

  *releaser = a[2].o;

  return result;
}
