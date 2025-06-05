/********************************************************************
 Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#include "VmOsal.h"
#include "heap.h"
#include "IModule.h"
#include "IModule_invoke.h"
#include "MinkTransportUtils.h"
#include "LinkCredentials.h"
#include "RemoteAdapter.h"
#include "object.h"
#include "xtzdCredentials.h"

typedef struct
{
  int refs;
  Object endpoint;
  Object linkCred;
} RemoteAdapter;

static inline
int32_t RemoteAdapter_shutdown(RemoteAdapter *me)
{
  return Object_OK;
}

/**************************************************************************
* Chance are there race condition occurs between retain() and release()
* Extra check are introduced to mitigate possible impact. But it cannot
* eliminate the risk because me would turn to be NULL during the 2 check
* Long way to overcome it
**************************************************************************/
static inline
int32_t RemoteAdapter_retain(RemoteAdapter *me)
{
  if (NULL != me && vm_osal_atomic_add(&me->refs, 0) > 0) {
    vm_osal_atomic_add(&me->refs, 1);
    return Object_OK;
  }

  return Object_ERROR;
}

static inline
int32_t RemoteAdapter_release(RemoteAdapter *me)
{
  if (vm_osal_atomic_add(&me->refs, -1) == 0) {
    LOG_TRACE("released moduleForwarder = %p, linkCred = %p, endpoint = %p\n", me,
               &me->linkCred, &me->endpoint);
    Object_ASSIGN_NULL(me->linkCred);
    Object_ASSIGN_NULL(me->endpoint);
    HEAP_FREE_PTR(me);
  }

  return Object_OK;
}

static
int32_t RemoteAdapter_open(RemoteAdapter *me, uint32_t id, Object credentials,
                           Object *objOut)
{
  int32_t res;
  Object credComposite = Object_NULL;

  if (Object_isOK(LinkCredComposite_new(me->linkCred, credentials, &credComposite))) {
    res = IModule_open(me->endpoint, id, credComposite, objOut);
    Object_ASSIGN_NULL(credComposite);
    LOG_TRACE("open moduleForwarder = %p, linkCred = %p, endpoint = %p, \
               objOut = %p\n", me, &me->linkCred, &me->endpoint, objOut);
    return res;
  }

  LOG_ERR("LinkCredentials_newFromCred failed, moduleForwarder = %p, \
           credentials = %p\n", me, &credentials);
  return Object_ERROR_UNAVAIL;
}

static
IModule_DEFINE_INVOKE(RemoteAdapter_invoke, RemoteAdapter_, RemoteAdapter*);

int32_t RemoteAdapter_new(Object endpoint, Object credentials, Object *objOut)
{
  RemoteAdapter *me = HEAP_ZALLOC_TYPE(RemoteAdapter);
  if (!me) {
    return Object_ERROR_MEM;
  }

  me->refs = 1;
  Object_INIT(me->endpoint, endpoint);
  Object_INIT(me->linkCred, credentials);
  *objOut = (Object){RemoteAdapter_invoke, me};

  return Object_OK;
}
