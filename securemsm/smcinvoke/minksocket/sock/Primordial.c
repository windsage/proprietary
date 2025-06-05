/********************************************************************
 Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#include "VmOsal.h"
#include "cdefs.h"
#include "check.h"
#include "heap.h"
#include "MinkTransportUtils.h"
#include "msforwarder.h"
#include "Primordial.h"
#include "IPrimordial_invoke.h"

struct Primordial {
  int refs;
  RegHndFunc regHndFunc;
};

static inline
int32_t Primordial_retain(Primordial *me)
{
  vm_osal_atomic_add(&me->refs, 1);

  return Object_OK;
}

static inline
int32_t Primordial_release(Primordial *me)
{
  if (vm_osal_atomic_add(&me->refs, -1) == 0) {
    LOG_TRACE("released primordial = %p, regHndFunc = %p\n", me, me->regHndFunc);
    me->regHndFunc = NULL;
    HEAP_FREE_PTR(me);
  }

  return Object_OK;
}

static
int32_t Primordial_registerHandler(Primordial *me, Object target,
                                   Object handler, Object *releaser)
{
  if (NULL == me->regHndFunc) {
    return Object_ERROR_UNAVAIL;
  }

  LOG_TRACE("released handler from primordial = %p, regHndFunc = %p\n", me,
             me->regHndFunc);
  return (*me->regHndFunc)(target, handler, releaser);
}

static
IPrimordial_DEFINE_INVOKE(Primordial_invoke, Primordial_, Primordial*);

int32_t Primordial_setRegisterHandlerFunc(Object *pmdObj, RegHndFunc func)
{
  Primordial *me = (Primordial *)pmdObj->context;
  if (!me) {
    return Object_ERROR_BADOBJ;
  }

  me->regHndFunc = func;

  LOG_TRACE("set handler to primordial = %p, regHndFunc = %p\n", me,
             me->regHndFunc);
  return Object_OK;
}

bool isPrimordialOrPrimordialFwd(Object obj)
{
  MSForwarder *msf = MSForwarderFromObject(obj);
  if (msf && (PRIMORDIAL_HANDLE == msf->handle)) {
    return true;
  }

  if (Primordial_invoke == obj.invoke) {
    return true;
  }

  return false;
}

int32_t Primordial_new(Object *objOut)
{
  Primordial *me = HEAP_ZALLOC_REC(Primordial);
  if (!me) {
    return Object_ERROR_MEM;
  }

  me->refs = 1;
  *objOut = (Object) {Primordial_invoke, me};

  return Object_OK;
}
