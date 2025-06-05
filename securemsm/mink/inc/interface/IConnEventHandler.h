/********************************************************************
Copyright (c) 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#pragma once

#include <stdbool.h>
#include "object.h"

#define IConnEventHandler_OP_onEvent 0

static inline int32_t
IConnEventHandler_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IConnEventHandler_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IConnEventHandler_onEvent(Object self, uint32_t event)
{
  ObjectArg a[1];
  a[0].b = (ObjectBuf) { &event, sizeof(uint32_t) };

  return Object_invoke(self, IConnEventHandler_OP_onEvent, a, ObjectCounts_pack(1, 0, 0, 0));
}

