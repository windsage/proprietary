/********************************************************************
 Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#include "VmOsal.h"
#include "cdefs.h"
#include "check.h"
#include "ConnEventReleaser.h"
#include "heap.h"
#include "IObject_invoke.h"
#include "MinkTransportUtils.h"

static inline
int32_t ConnEventReleaser_retain(ConnEventReleaser *me)
{
  vm_osal_atomic_add(&me->refs, 1);

  return Object_OK;
}

static
int32_t ConnEventReleaser_release(ConnEventReleaser *me)
{
  if (vm_osal_atomic_add(&me->refs, -1) == 0) {
    LOG_TRACE("released connEventReleaser = %p, handler = %p, subReleaser = %p\n", me,
               &me->handler, &me->subReleaser);
    Object_ASSIGN_NULL(me->handler);
    Object_ASSIGN_NULL(me->subReleaser);
    QNode_dequeueIf(&me->node);
    HEAP_FREE_PTR(me);
  }

  return Object_OK;
}

static
IObject_DEFINE_INVOKE(ConnEventReleaser_invoke, ConnEventReleaser_, ConnEventReleaser*);

int32_t ConnEventReleaser_new(Object handler, Object subReleaser, Object *objOut)
{
  ConnEventReleaser *me = HEAP_ZALLOC_REC(ConnEventReleaser);
  if (!me) {
    return Object_ERROR_MEM;
  }

  me->refs = 1;
  //in multiple connection scenario, handler is marshallIn parameter
  //only with reference from releaser can it survives in intermedium
  //endpoint after registration broadcast return
  //So releaser refers to handler to make it survived.
  Object_INIT(me->handler, handler);
  //in multiple connection scenario, subReleaser is marshalOut parameter
  //it was born in intermedium endpoint and still suvives after registration
  //broadcast return.
  //So releaser cannot refer to handler to avoid counts mismatch in furture release
  me->subReleaser = subReleaser;
  QNode_construct(&me->node);

  *objOut = (Object) {ConnEventReleaser_invoke, me};

  return Object_OK;
}
