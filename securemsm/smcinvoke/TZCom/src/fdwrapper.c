/********************************************************************
Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/
#include <unistd.h>
#include "memscpy.h"
#include "object.h"
#include <stdlib.h>
#include "fdwrapper.h"
#include <utils/Log.h>
#include "qlist.h"

// #ifdef LOG_TAG
// #undef LOG_TAG
// #endif
// #define LOG_TAG "vendor.qti.hardware.qteeconnector@1.0-gp"

static inline int atomic_add(int *pn, int n)
{
  return __sync_add_and_fetch(pn, n);  // GCC builtin
}

#define ZALLOC_REC(ty)		((ty *) calloc(1, sizeof(ty)))

/*================================================================
 * DescriptorObject
 *================================================================*/

void FdWrapper_delete(FdWrapper *me)
{
  ALOGV("FdWrapper_delete, me=%p, descriptor=%d", me, me->descriptor);
  QNode_dequeueIf(&me->qn);
  close(me->descriptor);
  free(me);
}

int32_t FdWrapper_release(FdWrapper *me) {
   ALOGV("FdWrapper_release, me=%p, descriptor=%d, before refs=%d", me, me->descriptor, me->refs);
   if (atomic_add(&me->refs, -1) == 0) {
     FdWrapper_delete(me);
   }
   return Object_OK;
}

int32_t FdWrapper_invoke(void *cxt, ObjectOp op, ObjectArg *args, ObjectCounts k)
{
  FdWrapper *me = (FdWrapper*) cxt;
  ObjectOp method = ObjectOp_methodID(op);

  switch (method) {
  case Object_OP_retain:
    atomic_add(&me->refs, 1);
    return Object_OK;

  case Object_OP_release:
    return FdWrapper_release(me);

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
  FdWrapper *me = ZALLOC_REC(FdWrapper);

  if (!me) {
    ALOGE("Zalloc in FdWrapper_new failed.");
    return Object_NULL;
  }

  me->refs = 1;
  me->descriptor = fd;
  QNode_construct(&me->qn);
  ALOGV("FdWrapper_new, me=%p, descriptor=%d", me, me->descriptor);
  return (Object) { FdWrapper_invoke, me };
}
