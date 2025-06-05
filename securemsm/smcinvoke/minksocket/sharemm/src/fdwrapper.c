/********************************************************************
 Copyright (c) 2018, 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#include "VmOsal.h"
#include "memscpy.h"
#include "object.h"
#include "heap.h"
#include "fdwrapper.h"
#include "MinkTransportUtils.h"

/*================================================================
 * DescriptorObject
 *================================================================*/

static inline void
FdWrapper_delete(FdWrapper *me)
{
  LOG_TRACE("deleted fdWrapper = %p, fd = %d, dependency = %p\n", me, me->descriptor,
           &me->dependency);

  vm_osal_mem_close(me->descriptor);
  Object_ASSIGN_NULL(me->dependency);
  Object_ASSIGN_NULL(me->confinement);
  heap_free(me);
}

static int32_t
FdWrapper_invoke(void *cxt, ObjectOp op, ObjectArg *args, ObjectCounts k)
{
  FdWrapper *me = (FdWrapper*) cxt;
  ObjectOp method = ObjectOp_methodID(op);

  switch (method) {
  case Object_OP_retain:
    vm_osal_atomic_add(&me->refs, 1);
    return Object_OK;

  case Object_OP_release:
    if (vm_osal_atomic_add(&me->refs, -1) == 0) {
      FdWrapper_delete(me);
    }
    return Object_OK;

  case Object_OP_unwrapFd:
    if (k != ObjectCounts_pack(0, 1, 0, 0)) {
      break;
    }
    memscpy(args[0].b.ptr, args[0].b.size,
            &me->descriptor, sizeof(me->descriptor));
    return Object_OK;
  }

  return Object_ERROR;
}

FdWrapper *FdWrapperFromObject(Object obj)
{
  return (obj.invoke == FdWrapper_invoke ? (FdWrapper*) obj.context : NULL);
}

Object FdWrapper_new(int fd)
{
  FdWrapper *me = HEAP_ZALLOC_REC(FdWrapper);

  if (!me) {
    return Object_NULL;
  }

  me->refs = 1;
  me->descriptor = fd;
  return (Object) { FdWrapper_invoke, me };
}

// Return true if obj is an FdWrapper with a valid fd
// On success, the output parameter "fd" is populated
bool isWrappedFd(Object obj, int* fd) {
  if (!fd) {
    return false;
  }
  *fd = -1;

  return (!Object_isNull(obj) &&
          FdWrapperFromObject(obj) != NULL &&
          Object_isOK(Object_unwrapFd(obj, fd)) &&
          *fd > 0);
}
