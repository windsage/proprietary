/********************************************************************
Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#include <stdint.h>
#include "CRegisterApp.h"
#include "CRegisterModule_open.h"
#include "heap.h"
#include "IOpener.h"
#include "IOpener_invoke.h"
#include "MinkHubLogging.h"
#include "object.h"
#include "ServiceOpener.h"
#include "ServiceManager.h"

// TODO: replace with proper include when ready
#define CTUIListener_UID 262

/***********************************************************************
 * Service opener
 * ********************************************************************/

typedef struct {
  int refs;
  ServiceManager *mgr;
  Object remoteOpener;
  Object credentials;
} ServiceOpener;

static
int32_t ServiceOpener_retain(ServiceOpener *me)
{
  atomic_add(&me->refs, 1);
  return Object_OK;
}

static
int32_t ServiceOpener_release(ServiceOpener *me)
{
  if (atomic_add(&me->refs, -1) == 0) {
    Object_ASSIGN_NULL(me->credentials);
    HEAP_FREE_PTR(me);
  }
  return Object_OK;
}

static
int32_t ServiceOpener_open(ServiceOpener *me, uint32_t id, Object *obj)
{
  /* special case, the local registration service */
  if (id == CRegisterApp_UID) {
    /* TODO: discriminate the opener works for internal or external interaction.
     * Only in internal situation, service registering is allowed.
     *
     * IOpener based interfaces are being replaced by IModule based ones.
     * Not sure whether need further improvement
     */
    return CRegisterModule_open(me->mgr, obj);

  }
  /* Local service lookup first */
  if (Object_isOK(ServiceManager_getService(me->mgr, id, me->credentials, obj))) {
    return Object_OK;
  }
  /* A service implemented in another VM? */
  if (!Object_isNull(me->remoteOpener)) {
    if (id == CTUIListener_UID) {
      return IOpener_open(me->remoteOpener, id, obj);
    }
  }
  return IOpener_ERROR_NOT_FOUND;
}

static
IOpener_DEFINE_INVOKE(ServiceOpener_invoke, ServiceOpener_, ServiceOpener*);

/**
 * New IOpener class on top of our registration framework.
 *
 * If a 'remoteOpener' is passed, then this IOpener is assumed to be exposed
 * locally on a socket, and therefore allows for access to both the
 * local IModule registration service and the services exposed by the remote
 * opener.
 * If a remoteOpener is NOT passed, then this IOpener is assumed to be
 * exposed on a QRTR/VSOCK interface, and it therefore only perform
 * resolution of locally registered services.
 *
 * credentials` is the ICredentials object for the VM this opener
 * is exposed to. It can be NULL.
 * */
Object ServiceOpener_new(ServiceManager *mgr, Object remoteOpener, Object credentials)
{
  ServiceOpener *me = HEAP_ZALLOC_TYPE(ServiceOpener);
  if (!me) {
    return Object_NULL;
  }
  me->refs = 1;
  me->mgr = mgr;
  Object_INIT(me->remoteOpener, remoteOpener);
  Object_INIT(me->credentials, credentials);
  return (Object){ ServiceOpener_invoke, me};
}
