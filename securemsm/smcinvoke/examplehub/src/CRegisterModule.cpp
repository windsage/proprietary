/********************************************************************
Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "CRegisterModule_open.h"
#include "heap.h"
#include "IRegisterApp_invoke.h"
#include "MinkHubLogging.h"
#include "ServiceManager.h"

typedef struct CRegisterModule {
  int refs;
  ServiceManager *mgr;
} CRegisterModule;

static int32_t CRegisterModule_retain(CRegisterModule* me) {
  atomic_add(&me->refs, 1);
  return Object_OK;
}

static int32_t CRegisterModule_release(CRegisterModule* me) {
  if (atomic_add(&me->refs, -1) == 0) {
      HEAP_FREE_PTR(me);
  }
  return Object_OK;
}

static int32_t CRegisterModule_registerApp(CRegisterModule* me, const uint32_t uid, Object o, const uint8_t *secret_ptr, size_t secret_len)
{
  return ServiceManager_registerModule(me->mgr, uid, o);
}

static int32_t CRegisterModule_deregisterApp(CRegisterModule* me, const uint32_t uid, const uint8_t *secret_ptr, size_t secret_len)
{
  ServiceManager_deregister(me->mgr, uid);
  return 0;
}

static IRegisterApp_DEFINE_INVOKE(IRegisterModule_invoke, CRegisterModule_, CRegisterModule*)

//----------------------------------------------------------------
// Exported functions
//----------------------------------------------------------------

int32_t CRegisterModule_open(ServiceManager *mgr, Object *objOut)
{
  CRegisterModule *me = HEAP_ZALLOC_TYPE(CRegisterModule);
  if (!me) {
      return Object_ERROR_KMEM;
  }
  me->refs = 1;
  me->mgr = mgr;
  *objOut = (Object){ IRegisterModule_invoke, me};
  return Object_OK;
}

