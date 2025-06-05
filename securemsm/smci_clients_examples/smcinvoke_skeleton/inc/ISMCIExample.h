/***********************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/
/**
 * Interface to the SMCInvoke skeleton_app functionality.
 */
/** @cond */
#pragma once
#include <stdint.h>
#include "object.h"

#define ISMCIExample_OP_add 0

static inline int32_t
ISMCIExample_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
ISMCIExample_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
ISMCIExample_add(Object self, uint32_t val1_val, uint32_t val2_val, uint32_t *result_ptr)
{
  ObjectArg a[2]={{{0,0}}};
  struct {
    uint32_t m_val1;
    uint32_t m_val2;
  } i;
  a[0].b = (ObjectBuf) { &i, 8 };
  i.m_val1 = val1_val;
  i.m_val2 = val2_val;
  a[1].b = (ObjectBuf) { result_ptr, sizeof(uint32_t) };

  return Object_invoke(self, ISMCIExample_OP_add, a, ObjectCounts_pack(1, 1, 0, 0));
}

