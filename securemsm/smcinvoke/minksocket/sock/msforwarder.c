/********************************************************************
 Copyright (c) 2016, 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#include "VmOsal.h"
#include "check.h"
#include "cdefs.h"
#include "ConnEventHandler.h"
#include "ConnEventReleaser.h"
#include "heap.h"
#include "IConnEventHandler.h"
#include "lxcom_sock.h"
#include "MinkTransportUtils.h"
#include "minksocket.h"
#include "memscpy.h"
#include "msforwarder.h"

static
ConnEventReleaser *MSForwarder_popConnEventReleaser(MSForwarder *me)
{
  ConnEventReleaser *releaser;

  vm_osal_mutex_lock(&me->mutex);
  QNode *qNode = QList_pop(&me->qlConnEventReleser);
  if (NULL == qNode) {
    releaser = NULL;
  } else {
    releaser = c_containerof(qNode, ConnEventReleaser, node);
  }

  vm_osal_mutex_unlock(&me->mutex);
  return releaser;
}

/*=========================================================================
 * it is where ConnEventHandler actually be registered to target object
 * ConnEventHandler is wrapped by ConnEventReleaser
 *========================================================================*/
void MSForwarder_attachConnEventReleaser(Object fwdObj, Object releaserObj)
{
  MSForwarder* fwd = (MSForwarder *)fwdObj.context;
  ConnEventReleaser *releaser = (ConnEventReleaser *)releaserObj.context;

  vm_osal_mutex_lock(&fwd->mutex);
  QList_appendNode(&fwd->qlConnEventReleser, &releaser->node);
  vm_osal_mutex_unlock(&fwd->mutex);

  return;
}

/*=========================================================================
 * trigger invocation of ConnEventHandler
 * a MSForwarder object can be monitored by several ConnEventHandlers from
 * different remote clients. They are listed by qlConnEventReleser
 *========================================================================*/
void MSForwarder_notifyConnEvent(MSForwarder *me, uint32_t event)
{
  ConnEventReleaser *releaser;

  if (NULL == me) {
    return;
  }

  while ((releaser = MSForwarder_popConnEventReleaser(me))) {
    IConnEventHandler_onEvent((releaser->handler), event);
  }

  return;
}

static inline
void MSForwarder_delete(MSForwarder *me)
{
  LOG_TRACE("deleted msforwarder = %p, minksocket = %p\n", me, me->conn);

  MinkSocket_preDeleteForwarder(me->conn, me);
  vm_osal_mutex_deinit(&me->mutex);
  heap_free(me);
}

int32_t MSForwarder_detach(MSForwarder *me)
{
  int32_t handle = me->handle;

  MSForwarder_notifyConnEvent(me, EVENT_CONN_DETACH);
  MinkSocket_detachForwarder(me->conn, me);

  return handle;
}

int32_t MSForwarder_derivePrimordial(MSForwarder *fwd, Object *pmd)
{
  return MSForwarder_new(fwd->conn, PRIMORDIAL_HANDLE, pmd);
}

static int32_t
MSForwarder_invoke(void *cxt, ObjectOp op, ObjectArg *args, ObjectCounts k)
{
  MSForwarder *me = (MSForwarder*) cxt;
  ObjectOp method = ObjectOp_methodID(op);

  if (ObjectOp_isLocal(op)) {
    switch (method) {
    case Object_OP_retain:
      if (me->refs > INT_MAX - 1)
        return Object_ERROR_MAXDATA;
      vm_osal_atomic_add(&me->refs, 1);
      return Object_OK;

    case Object_OP_release:
      if (me->refs < 1)
        return Object_ERROR_MAXDATA;
      if (vm_osal_atomic_add(&me->refs, -1) == 0) {
        MSForwarder_delete(me);
      }
      return Object_OK;

    case Object_OP_unwrapFd:
      if (k != ObjectCounts_pack(0, 1, 0, 0)) {
        break;
      }
      int fd = -1;
      memscpy(args[0].b.ptr, args[0].b.size, &fd, sizeof(fd));
      return Object_OK;
    }

    return Object_ERROR;
  }

  return MinkSocket_invoke(me->conn, me->handle, op, args, k);
}

MSForwarder *MSForwarderFromObject(Object obj)
{
  return (obj.invoke == MSForwarder_invoke ? (MSForwarder*) obj.context : NULL);
}

int32_t MSForwarder_new(MinkSocket *conn, int handle, Object *objOut)
{
  MSForwarder *me = HEAP_ZALLOC_REC(MSForwarder);
  if (!me) {
    return Object_ERROR_MEM;
  }

  me->refs = 1;
  me->handle = handle;
  me->conn = conn;
  QNode_construct(&me->node);
  vm_osal_mutex_init(&me->mutex, NULL);
  QList_construct(&me->qlConnEventReleser);

  if (MinkSocket_attachForwarder(conn, me)) {
    heap_free(me);
    return Object_ERROR;
  }

  *objOut = (Object) {MSForwarder_invoke, me};

  LOG_TRACE("constructed msforwarder = %p, handle = %d, minksocket = %p\n", me,
             me->handle, me->conn);
  return Object_OK;
}
