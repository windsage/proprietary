/***********************************************************************
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/

#include "Confinement.h"
#include "IConfinement.h"
#include "IConfinement_invoke.h"
#include "ITAccessPermissions.h"
#include "cdefs.h"
#include "fdwrapper.h"
#include "heap.h"
#include "MinkTransportUtils.h"
#include "memscpy.h"
#include "object.h"

/////////////////////////////////////////////
//        Confinement definition         ////
/////////////////////////////////////////////

static int32_t CConfinement_retain(Confinement *me)
{
  vm_osal_atomic_add(&me->refs, 1);

  return Object_OK;
}

static int32_t CConfinement_release(Confinement *me)
{
  if (vm_osal_atomic_add(&me->refs, -1) == 0) {
    LOG_TRACE("released confinement = %p\n", me);
    HEAP_FREE_PTR(me);
  }

  return Object_OK;
}

static int32_t CConfinement_getSpecialRules(Confinement *me,
                                            uint64_t *specialRules)
{
  const ITAccessPermissions_rules *confRules = &(me->confRules);

  *specialRules = confRules->specialRules;

#ifdef ANDROID
  LOG_TRACE("get specialRules = %llu from confinement = %p\n", *specialRules, me);
#else
  LOG_TRACE("get specialRules = %ld from confinement = %p\n", *specialRules, me);
#endif
  return Object_OK;
}

static int32_t CConfinement_getConfinementRules(
  Confinement *me, ITAccessPermissions_rules *outConfRules)
{
  memscpy(outConfRules, sizeof(ITAccessPermissions_rules), &(me->confRules),
          sizeof(ITAccessPermissions_rules));

  LOG_TRACE("get onfRules = %p from confinement = %p\n", outConfRules, me);
  return Object_OK;
}

static IConfinement_DEFINE_INVOKE(CConfinement_invoke, CConfinement_,
                                  Confinement *);

int32_t CConfinement_new(const ITAccessPermissions_rules *userRules,
                         Object *objOut)
{
  int32_t ret = Object_OK;

  if (!userRules) {
    return Object_ERROR;
  }

  Confinement *me = HEAP_ZALLOC_TYPE(Confinement);
  if (!me) {
    return Object_ERROR_MEM;
  }

  me->refs = 1;
  memscpy(&(me->confRules), sizeof(ITAccessPermissions_rules), userRules,
          sizeof(ITAccessPermissions_rules));

  *objOut = (Object){CConfinement_invoke, me};

  return ret;
}

Confinement *ConfinementFromObject(Object obj)
{
  return (obj.invoke == CConfinement_invoke ? (Confinement *)obj.context
                                            : NULL);
}
