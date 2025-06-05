/********************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/
/**
 * ICallbackObjectExample demonstrates how we can implement a callback interface in the CA,
 * and use it from the TA to call code in the CA.
 */
/** @cond */
#pragma once
#include <stdint.h>
#include "object.h"

#define ICallbackObjectExample_OP_print 0

static inline int32_t
ICallbackObjectExample_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
ICallbackObjectExample_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
ICallbackObjectExample_print(Object self, const void *msg_ptr, size_t msg_len)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { msg_ptr, msg_len * 1 };

  return Object_invoke(self, ICallbackObjectExample_OP_print, a, ObjectCounts_pack(1, 0, 0, 0));
}

