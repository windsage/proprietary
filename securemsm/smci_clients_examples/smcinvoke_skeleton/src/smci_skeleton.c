/***********************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <utils/Log.h>

#include "object.h"
#include "IAppController.h"
#include "IAppLoader.h"
#include "IClientEnv.h"
#include "ISMCIExample.h"
#include "TZCom.h"
#include "CAppLoader.h"
#include "utils.c"

#define TEST_OK(input)                                       \
  {                                                          \
    if(input) {                                              \
      ALOGE("%s failed\n",__func__);                         \
      return 0;                                              \
    }                                                        \
  }

/* This function demonstrates how to open a TA using SMCInvoke APIs. */
int32_t run_smcinvoke_ta_example(char *appPath)
{
  int32_t ret = Object_OK;

  Object clientEnv = Object_NULL;      // A Client Environment that can be used to
                                       // get an IAppLoader object
  Object appLoader = Object_NULL;      // IAppLoader object that allows us to load
                                       // the TA in the trusted environment
  Object appController = Object_NULL;  // AppController contains a reference to
                                       // the app itself, after loading
  Object appObj = Object_NULL;         // An interface to our TA that allows us to send
                                       // commands to it.
  uint32_t val1 = 2;
  uint32_t val2 = 5;
  uint32_t addResult = 0;

  /* Before we can obtain an AppLoader object, a ClientEnv object is required
   * from the emulated TZ daemon. */
  ret = TZCom_getClientEnvObject(&clientEnv);
  if (Object_isERROR(ret)) {
    ALOGE("Failed to obtain clientenv from TZCom with ret = %d\n", ret);
    clientEnv = Object_NULL;
    goto cleanup;
  }

  /* Using the ClientEnv object we retrieved, obtain an appLoader by
   * specifying its UID */
  ret = IClientEnv_open(clientEnv, CAppLoader_UID, &appLoader);
  if (Object_isERROR(ret)) {
    ALOGE("Failed to get apploader object with %d!\n", ret);
    appLoader = Object_NULL;
    goto cleanup;
  }

  ALOGI("Succeeded in getting apploader object.\n");
  ALOGD("appPath is %s\n", appPath);
   /* load the application */
  ret = load_app(appLoader, appPath, &appController);
  if (Object_isERROR(ret)) {
    ALOGE("Loading the application failed with %d!\n", ret);
    appController = Object_NULL;
    goto cleanup;
  }
  ALOGI("Loading the application succeeded.\n");

  ret = IAppController_getAppObject(appController, &appObj);
  if (Object_isERROR(ret)) {
    ALOGE("Getting the application object failed with %d!\n", ret);
    appObj = Object_NULL;
    goto cleanup;
  }
  ALOGI("Getting the application object succeeded.\n");

  /* run the ISMCIExample_add function from the ISMCIExample interface. */
  ret = ISMCIExample_add(appObj, val1, val2, &addResult);

  if (Object_isERROR(ret)) {
    ALOGE("Addition returned error %d!\n", ret);
  } else {
    ALOGI("Add result: %d\n", addResult);
    printf("Add result: %d\n", addResult);
  }

cleanup:
  Object_ASSIGN_NULL(appObj);
  if (!Object_isNull(appController)) {
    TEST_OK(IAppController_unload(appController));
    Object_ASSIGN_NULL(appController);
  }
  Object_ASSIGN_NULL(appLoader);
  Object_ASSIGN_NULL(clientEnv);
  return ret;
}
