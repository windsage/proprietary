/********************************************************************
Copyright (c) 2020-2021, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "IModule.h"
#include "IOpener.h"
#include "MinkHubLogging.h"
#include "ServiceManager.h"

typedef struct {
  uint32_t uid;
  Object mod;
} Service;

struct ServiceManager {
  Service *services;
  size_t size;
  pthread_mutex_t mutex;
};

static
bool ServiceManager_isRegistered(ServiceManager const *me, uint32_t uid)
{
  Service *services = me->services;
  for (size_t i = 0; i < me->size; i++) {
    if (services[i].uid == uid) {
      return true;
    }
  }
  return false;
}

int ServiceManager_getService(ServiceManager *me, uint32_t uid, Object credentials, Object *objOut)
{
  Object rObj = Object_NULL;
  Service *services = me->services;
  pthread_mutex_lock(&me->mutex);
  for (size_t i = 0; i < me->size; i++) {
    if (services[i].uid == uid) {
      // one further level, call in the process
      int32_t ret = Object_OK;
      ret = IModule_open(services[i].mod, uid, credentials, &rObj);
      if (Object_isERROR(ret)) {
        rObj = Object_NULL;
        goto bail;
      }
      break;
    }
  }
bail:
  pthread_mutex_unlock(&me->mutex);
  if (Object_isNull(rObj)) {
    return IOpener_ERROR_NOT_FOUND;
  }
  // transfer the reference
  *objOut = rObj;
  return Object_OK;
}

int ServiceManager_registerModule(ServiceManager *me, uint32_t uid, Object mod)
{
  int ret = Object_OK;
  Service *services = me->services;

  pthread_mutex_lock(&me->mutex);
  if (ServiceManager_isRegistered(me, uid)) {
    ret = Object_ERROR; // TODO: better error code
    goto exit;
  }

  for (size_t i = 0; i < me->size; i++) {
    if (services[i].uid == 0) {
      services[i].uid = uid;
      Object_INIT(services[i].mod, mod);
      ret = Object_OK;
      goto exit;
    }
  }

  ret = Object_ERROR_MAXDATA;

exit:
  pthread_mutex_unlock(&me->mutex);
  return ret; // TODO: better error code
}

void ServiceManager_deregister(ServiceManager *me, uint32_t uid)
{
  Service *services = me->services;

  pthread_mutex_lock(&me->mutex);
  for (int i=0; i < me->size; i++) {
    if (services[i].uid == uid) {
      /* app object maintains its own refcount, so there is no danger of crashing
       * if we remove entry from here based on deregister. we dont increase ref
       * for each register, so on deregister we just release object and set UID to
       * invalid i.e. 0
       */
      Object_ASSIGN_NULL(services[i].mod);
      services[i].uid = 0;
    }
  }
  pthread_mutex_unlock(&me->mutex);
}

ServiceManager *ServiceManager_new(size_t maxApps)
{
  ServiceManager *mgr = (ServiceManager *)calloc(1, sizeof(ServiceManager));
  if (mgr) {
    Service * services = (Service *)calloc(maxApps, sizeof(Service));
    if (services) {
      mgr->services = services;
      mgr->size = maxApps;
      pthread_mutex_init(&mgr->mutex, NULL);
    } else {
      free(mgr);
      mgr = NULL;
    }
  }
  return mgr;
}

void ServiceManager_delete(ServiceManager *me)
{
  Service *services = me->services;
  for (int i=0; i < me->size; i++) {
    ServiceManager_deregister(me, services[i].uid);
  }
  free(me->services);
  memset(me, 0, sizeof(ServiceManager));
  free(me);
}
