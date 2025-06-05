/********************************************************************
Copyright (c) 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#pragma once

#include <stdint.h>
#include "object.h"
#include "IConnEventHandler.h"

#define IConnEventHandler_DEFINE_INVOKE(func, prefix, type) \
  int32_t func(ObjectCxt h, ObjectOp op, ObjectArg *a, ObjectCounts k) \
  { \
    type me = (type) h; \
    switch (ObjectOp_methodID(op)) { \
      case Object_OP_retain: { \
        if (k != ObjectCounts_pack(0, 0, 0, 0)) { \
          break; \
        } \
        return prefix##retain(me); \
      } \
      case Object_OP_release: { \
        if (k != ObjectCounts_pack(0, 0, 0, 0)) { \
          break; \
        } \
        return prefix##release(me); \
      } \
      case IConnEventHandler_OP_onEvent: { \
        if (k != ObjectCounts_pack(1, 0, 0, 0) || \
          a[0].b.size != 4) { \
          break; \
        } \
        const uint32_t *event = (const uint32_t*) a[0].b.ptr; \
        return prefix##onEvent(me, *event); \
      } \
    } \
    return Object_ERROR_INVALID; \
  }
