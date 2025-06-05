/********************************************************************
 Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#include "VmOsal.h"
#include "heap.h"
#include "IOpener.h"
#include "IOpener_invoke.h"
#include "IModule.h"
#include "object.h"
#include "LocalAdapter.h"
#include "MinkTransportUtils.h"

typedef struct
{
  int refs;
  Object endpoint;
  Object linkCred;
} LocalAdapter;

/**************************************************************************
* Chance are there race condition occurs between retain() and release()
* Extra check are introduced to mitigate possible impact. But it cannot
* eliminate the risk because me would turn to be NULL during the 2 check
* Long way to overcome it
**************************************************************************/
static inline
int32_t LocalAdapter_retain(LocalAdapter *me)
{
  if (NULL != me && vm_osal_atomic_add(&me->refs, 0) > 0) {
    vm_osal_atomic_add(&me->refs, 1);
    return Object_OK;
  }

  return Object_ERROR;
}

static inline
int32_t LocalAdapter_release(LocalAdapter *me)
{
  if (vm_osal_atomic_add(&me->refs, -1) == 0) {
    LOG_TRACE("released openerForwarder = %p, endpoint = %p, linkCred = %p\n", me,
               &me->endpoint, &me->linkCred);
    Object_ASSIGN_NULL(me->endpoint);
    Object_ASSIGN_NULL(me->linkCred);
    HEAP_FREE_PTR(me);
  }

  return Object_OK;
}

static inline
int32_t LocalAdapter_open(LocalAdapter *me, uint32_t id, Object *obj)
{
  LOG_TRACE("open openerForwarder = %p, endpoint = %p, linkCred = %p, id = %d, \
             objOut = %p\n", me, &me->endpoint, &me->linkCred, id, obj);
  return IModule_open(me->endpoint, id, me->linkCred, obj);
}

static
IOpener_DEFINE_INVOKE(LocalAdapter_invoke, LocalAdapter_, LocalAdapter*);

int32_t LocalAdapter_new(Object endpoint, Object credentials, Object *objOut)
{
  LocalAdapter *me = HEAP_ZALLOC_TYPE(LocalAdapter);
  if (!me) {
    return Object_ERROR_MEM;
  }

  me->refs = 1;
  Object_INIT(me->endpoint, endpoint);
  Object_INIT(me->linkCred, credentials);
  *objOut = (Object) {LocalAdapter_invoke, me};

  return Object_OK;
}
