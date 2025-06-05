/**
 * Copyright (c) 2019, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include <stdint.h>
#include "object.h"
#include "IMemRegion.h"
#include "IListenerCBO.h"

#define IListenerCBO_DEFINE_INVOKE(func, prefix, type) \
  int32_t func(ObjectCxt h, ObjectOp op, ObjectArg *a, ObjectCounts k) \
  { \
    type me = (type) h; \
    switch (ObjectOp_methodID(op)) { \
      case Object_OP_release: { \
        if (k != ObjectCounts_pack(0, 0, 0, 0)) { \
          break; \
        } \
        return prefix##release(me); \
      } \
      case Object_OP_retain: { \
        if (k != ObjectCounts_pack(0, 0, 0, 0)) { \
          break; \
        } \
        return prefix##retain(me); \
      } \
      case IListenerCBO_OP_request: { \
        if (k != ObjectCounts_pack(0, 2, 0, 4) || \
          a[1].b.size != 4) { \
          break; \
        } \
        uint32_t *embeddedBufOffsets_ptr = (uint32_t*)a[0].b.ptr; \
        size_t embeddedBufOffsets_len = a[0].b.size / sizeof(uint32_t); \
        uint32_t *is64_ptr = (uint32_t*)a[1].b.ptr; \
        int32_t r = prefix##request(me, embeddedBufOffsets_ptr, embeddedBufOffsets_len, &embeddedBufOffsets_len, is64_ptr, &a[2].o, &a[3].o, &a[4].o, &a[5].o); \
        a[0].b.size = embeddedBufOffsets_len * sizeof(uint32_t); \
        return r; \
      } \
      case IListenerCBO_OP_wait: { \
        if (k != ObjectCounts_pack(0, 0, 0, 0)) { \
          break; \
        } \
        return prefix##wait(me); \
      } \
    } \
    return Object_ERROR_INVALID; \
  }


