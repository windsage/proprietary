/********************************************
Copyright (c) 2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************/
#pragma once

#include <stdint.h>
#include "object.h"
#include "IListenerCBO.h"

#define IRegisterListenerCBO_ERROR_MAX_REGISTERED INT32_C(10)
#define IRegisterListenerCBO_ERROR_ALIGNMENT INT32_C(11)
#define IRegisterListenerCBO_ERROR_ID_IN_USE INT32_C(12)
#define IRegisterListenerCBO_ERROR_ID_RESERVED INT32_C(13)

#define IRegisterListenerCBO_OP_register 0

static inline int32_t
IRegisterListenerCBO_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IRegisterListenerCBO_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IRegisterListenerCBO_register(Object self, uint32_t listenerId_val, Object cbo_val, Object memRegion_val)
{
  ObjectArg a[3];
  a[0].b = (ObjectBuf) { &listenerId_val, sizeof(uint32_t) };
  a[1].o = cbo_val;
  a[2].o = memRegion_val;

  return Object_invoke(self, IRegisterListenerCBO_OP_register, a, ObjectCounts_pack(1, 0, 2, 0));
}



