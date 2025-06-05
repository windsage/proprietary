/********************************************
Copyright (c) 2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************/
#pragma once

#include <stdint.h>
#include "object.h"

/** @cond */

#define IMemRegion_PERM_X UINT32_C(1)
#define IMemRegion_PERM_W UINT32_C(2)
#define IMemRegion_PERM_R UINT32_C(4)
#define IMemRegion_PERM_RW UINT32_C(6)
#define IMemRegion_PERM_RX UINT32_C(5)

#define IMemRegion_ERROR_PERM INT32_C(10)

#define IMemRegion_OP_getData 0
#define IMemRegion_OP_setData 1
#define IMemRegion_OP_createRestrictedRegion 2

static inline int32_t
IMemRegion_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IMemRegion_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IMemRegion_getData(Object self, uint64_t offset_val, void *data_ptr, size_t data_len, size_t *data_lenout)
{
  ObjectArg a[2];
  a[0].b = (ObjectBuf) { &offset_val, sizeof(uint64_t) };
  a[1].b = (ObjectBuf) { data_ptr, data_len * 1 };

  int32_t result = Object_invoke(self, IMemRegion_OP_getData, a, ObjectCounts_pack(1, 1, 0, 0));

  *data_lenout = a[1].b.size / 1;

  return result;
}

static inline int32_t
IMemRegion_setData(Object self, uint64_t offset_val, const void *data_ptr, size_t data_len)
{
  ObjectArg a[2];
  a[0].b = (ObjectBuf) { &offset_val, sizeof(uint64_t) };
  a[1].bi = (ObjectBufIn) { data_ptr, data_len * 1 };

  return Object_invoke(self, IMemRegion_OP_setData, a, ObjectCounts_pack(2, 0, 0, 0));
}

static inline int32_t
IMemRegion_createRestrictedRegion(Object self, uint32_t perms_val, Object *region_ptr)
{
  ObjectArg a[2];
  a[0].b = (ObjectBuf) { &perms_val, sizeof(uint32_t) };

  int32_t result = Object_invoke(self, IMemRegion_OP_createRestrictedRegion, a, ObjectCounts_pack(1, 0, 0, 1));

  *region_ptr = a[1].o;

  return result;
}



