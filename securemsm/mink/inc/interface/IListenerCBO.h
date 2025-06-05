/**
 * Copyright (c) 2019, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include <stdint.h>
#include "object.h"
#include "IMemRegion.h"

#define IListenerCBO_OP_request 0
#define IListenerCBO_OP_wait 1

static inline int32_t
IListenerCBO_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IListenerCBO_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IListenerCBO_request(Object self, uint32_t *embeddedBufOffsets_ptr, size_t embeddedBufOffsets_len, size_t *embeddedBufOffsets_lenout, uint32_t *is64_ptr, Object *smo1_ptr, Object *smo2_ptr, Object *smo3_ptr, Object *smo4_ptr)
{
  ObjectArg a[6]={{{0,0}}};
  a[0].b = (ObjectBuf) { embeddedBufOffsets_ptr, embeddedBufOffsets_len * sizeof(uint32_t) };
  a[1].b = (ObjectBuf) { is64_ptr, sizeof(uint32_t) };

  int32_t result = Object_invoke(self, IListenerCBO_OP_request, a, ObjectCounts_pack(0, 2, 0, 4));

  *embeddedBufOffsets_lenout = a[0].b.size / sizeof(uint32_t);
  *smo1_ptr = a[2].o;
  *smo2_ptr = a[3].o;
  *smo3_ptr = a[4].o;
  *smo4_ptr = a[5].o;

  return result;
}

static inline int32_t
IListenerCBO_wait(Object self)
{
  return Object_invoke(self, IListenerCBO_OP_wait, 0, 0);
}


