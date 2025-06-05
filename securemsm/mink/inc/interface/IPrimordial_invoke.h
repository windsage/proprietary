/********************************************************************
Copyright (c) 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#pragma once

#include <stdint.h>
#include "object.h"
#include "IPrimordial.h"

#define IPrimordial_DEFINE_INVOKE(func, prefix, type) \
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
      case IPrimordial_OP_registerHandler: { \
        if (k != ObjectCounts_pack(0, 0, 2, 1)) {\
          break; \
        } \
        return prefix##registerHandler(me, a[0].o, a[1].o, &a[2].o); \
      } \
    } \
    return Object_ERROR_INVALID; \
  }
