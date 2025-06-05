/***********************************************************
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
#include "ICallbackObjectExample.h"

#define ICallbackObjectExample_DEFINE_INVOKE(func, prefix, type) \
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
      case ICallbackObjectExample_OP_print: { \
        if (k != ObjectCounts_pack(1, 0, 0, 0)) { \
          break; \
        } \
        const void *msg_ptr = (const void*)a[0].b.ptr; \
        size_t msg_len = a[0].b.size / 1; \
        return prefix##print(me, msg_ptr, msg_len); \
      } \
    } \
    return Object_ERROR_INVALID; \
  }


