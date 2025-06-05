/********************************************************************
 Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#include "cdefs.h"
#include "check.h"
#include "ConnEventHandler.h"
#include "heap.h"
#include "IConnEventHandler_invoke.h"
#include "MinkTransportUtils.h"

struct ConnEventHandler {
  int refs;
  HandlerFunc onEvent;
};

static inline
int32_t ConnEventHandler_retain(ConnEventHandler *me)
{
  vm_osal_atomic_add(&me->refs, 1);

  return Object_OK;
}

static inline
int32_t ConnEventHandler_release(ConnEventHandler *me)
{
  if (vm_osal_atomic_add(&me->refs, -1) == 0) {
    LOG_TRACE("released connEventHandler = %p, HandlerFunc = %p\n", me, me->onEvent);
    me->onEvent = NULL;
    HEAP_FREE_PTR(me);
  }

  return Object_OK;
}

static
int32_t ConnEventHandler_onEvent(ConnEventHandler *me, int32_t event)
{
  if (NULL == me->onEvent) {
    return Object_ERROR_UNAVAIL;
  }

  LOG_TRACE("closeNotification triggered on connEventHandler = %p, HandlerFunc \
             = %p, event=%d\n", me, me->onEvent, event);
  (*me->onEvent)(event);
  return Object_OK;
}

static
IConnEventHandler_DEFINE_INVOKE(ConnEventHandler_invoke, ConnEventHandler_,
                                ConnEventHandler*);

bool isConnEventHandler(Object obj)
{
  return (ConnEventHandler_invoke == obj.invoke ? true : false);
}

int32_t ConnEventHandler_new(HandlerFunc func, Object *objOut)
{
  if (NULL == func) {
    return Object_ERROR;
  }

  ConnEventHandler *me = HEAP_ZALLOC_REC(ConnEventHandler);
  if (!me) {
    return Object_ERROR_MEM;
  }

  me->refs = 1;
  me->onEvent = func;
  *objOut = (Object) {ConnEventHandler_invoke, me};

  return Object_OK;
}
