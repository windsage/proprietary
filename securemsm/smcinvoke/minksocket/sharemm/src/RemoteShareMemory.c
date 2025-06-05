/***********************************************************************
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/

#include "RemoteShareMemory.h"
#include "Confinement.h"
#include "fdwrapper.h"
#include "MinkTransportUtils.h"

int32_t RemoteShareMemory_attachConfinement(
  const ITAccessPermissions_rules *userRules,
  Object *memObj)
{
  int32_t ret = Object_OK, bufFd = -1;
  Object conf = Object_NULL;

  // TODO: defaultRules should mean OPEN_ACCESS to all UIDs/DIDs/Credentials
  ITAccessPermissions_rules defaultRules = {0};
  defaultRules.specialRules = ITAccessPermissions_keepSelfAccess;
  FdWrapper *returnedFdWrapper = FdWrapperFromObject(*memObj);

  if (NULL == returnedFdWrapper) {
    LOG_ERR("NOT a fdwrapper object!\n");
    return Object_ERROR_BADOBJ;
  }

  if (!Object_isOK(ret = Object_unwrapFd(*memObj, &bufFd))) {
    LOG_ERR("cannot unwrapfd from fdwrapper with ret=%d.\n", ret);
    return Object_ERROR_BADOBJ;
  }

  if (NULL != userRules) {
    ret = CConfinement_new(userRules, &conf);
    LOG_TRACE("constructed confinement objOut = %p, confinement = %p, userRules = %p\n",
               &conf, conf.context, userRules);
  } else {
    ret = CConfinement_new(&defaultRules, &conf);
    LOG_TRACE("constructed confinement objOut = %p, confinement = %p, defaultRules = %p\n",
               &conf, conf.context, &defaultRules);
  }

  if (!Object_isOK(ret)) {
    LOG_ERR("cannot create confinement with ret=%d.\n", ret);
    Object_ASSIGN_NULL(conf);
    return Object_ERROR;
  }

  returnedFdWrapper->confinement = conf;

  return Object_OK;
}
