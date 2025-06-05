/***********************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/
/**
 * Interface to the SMCInvoke Example App functionality.
 */
/** @cond */
#pragma once
#include <stdint.h>
#include "object.h"
#include "ICallbackObjectExample.h"

#define ISMCIExampleApp_HASH_SHA256_SIZE UINT32_C(32)
#define ISMCIExampleApp_HASH_SHA256 UINT32_C(1)

#define ISMCIExampleApp_ERROR_INPUT_BUFFER_TOO_SMALL INT32_C(10)

#define ISMCIExampleApp_OP_serviceExample 0
#define ISMCIExampleApp_OP_computeHash 1
#define ISMCIExampleApp_OP_sharedMemoryExample 2
#define ISMCIExampleApp_OP_callbackObjectExample 3

static inline int32_t
ISMCIExampleApp_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
ISMCIExampleApp_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
ISMCIExampleApp_serviceExample(Object self)
{
  return Object_invoke(self, ISMCIExampleApp_OP_serviceExample, 0, 0);
}

static inline int32_t
ISMCIExampleApp_computeHash(Object self, const void *input_ptr, size_t input_len, uint32_t algorithm_val, void *hash_ptr, size_t hash_len, size_t *hash_lenout)
{
  ObjectArg a[3]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { input_ptr, input_len * 1 };
  a[1].b = (ObjectBuf) { &algorithm_val, sizeof(uint32_t) };
  a[2].b = (ObjectBuf) { hash_ptr, hash_len * 1 };

  int32_t result = Object_invoke(self, ISMCIExampleApp_OP_computeHash, a, ObjectCounts_pack(2, 1, 0, 0));

  *hash_lenout = a[2].b.size / 1;

  return result;
}

static inline int32_t
ISMCIExampleApp_sharedMemoryExample(Object self, Object memObj_val)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].o = memObj_val;

  return Object_invoke(self, ISMCIExampleApp_OP_sharedMemoryExample, a, ObjectCounts_pack(0, 0, 1, 0));
}

static inline int32_t
ISMCIExampleApp_callbackObjectExample(Object self, Object callbackObj_val)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].o = callbackObj_val;

  return Object_invoke(self, ISMCIExampleApp_OP_callbackObjectExample, a, ObjectCounts_pack(0, 0, 1, 0));
}



