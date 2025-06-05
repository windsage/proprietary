/***********************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/

#include <iostream>
using namespace std;
#include <utils/Log.h>

#include "object.h"
#include "IAppController.hpp"
#include "IAppLoader.hpp"
#include "IClientEnv.hpp"
#include "ISMCIExample.hpp"
#include "TZCom.h"
#include "CAppLoader.hpp"
#include "utils.cpp"

#define TEST_OK(input)                                            \
  {                                                               \
    if(input) {                                                   \
      ALOGE("%s failed\n",__func__);                              \
      return 0;                                                   \
    }                                                             \
  }

/* This function demonstrates how to open a TA using SMCInvoke APIs. */
int32_t run_smcinvoke_ta_example(char *appPath)
{
  int32_t ret = Object_OK;

  Object clientEnv = Object_NULL;      // A Client Environment that can be used to
                                       // get an IAppLoader object
  IAppLoader iAppLoader;      // IAppLoader object that allows us to load
                                       // the TA in the trusted environment
  IAppController iAppController;  // AppController contains a reference to
                                       // the app itself, after loading
  ISMCIExample iSMCIExample;
  IClientEnv iClientEnv;
  uint32_t val1 = 2;
  uint32_t val2 = 5;
  uint32_t addResult;
  size_t size = 0;
  uint8_t * buffer = NULL;

  /* Before we can obtain an AppLoader object, a ClientEnv object is required
   * from the emulated TZ daemon. */
  ret = TZCom_getClientEnvObject(&clientEnv);
  if (Object_isERROR(ret)) {
    ALOGE("Failed to obtain clientenv from TZCom with ret = %d\n", ret);
    clientEnv = Object_NULL;
    goto cleanup;
  }

  iClientEnv = clientEnv;
  clientEnv = Object_NULL;  // reference was consumed by IClientEnv instantiation
  /* Using the ClientEnv object we retrieved, obtain an appLoader by
   * specifying its UID */
  ret = iClientEnv.open(CAppLoader_UID, iAppLoader);
  if (Object_isERROR(ret)) {
    ALOGE("Failed to get apploader object with %d\n", ret);
    goto cleanup;
  }

  ALOGI("Succeeded in getting apploader object.\n");
  ALOGD("appPath is %s\n", appPath);
   /* load the application */
  ret = load_app(appPath, &size, &buffer);
  if (Object_isERROR(ret)) {
    ALOGE("Reading/getting the appsize and buffer failed with %d\n", ret);
    goto cleanup;
  }

  ret = iAppLoader.loadFromBuffer(buffer, size, iAppController);
  if (Object_isERROR(ret)) {
    ALOGE("Loading the application failed with %d\n", ret);
    goto cleanup;
  }
  ALOGI("Loading the application succeeded.\n");

  ret = iAppController.getAppObject(iSMCIExample);
  if (Object_isERROR(ret)) {
    ALOGE("Getting the application object failed with %d\n", ret);
    goto cleanup;
  }
  ALOGI("Getting the application object succeeded.\n");

  /* run the ISMCIExample_add function from the ISMCIExample interface. */
  ret = iSMCIExample.add(val1, val2, &addResult);

  if (Object_isERROR(ret)) {
    ALOGE("Addition returned error %d\n", ret);
  } else {
    ALOGI("Add result: %d\n", addResult);
    cout<<"Add result: "<< addResult<<"\n";
  }

cleanup:
  if (buffer)
    free(buffer);
  TEST_OK(iAppController.unload());
  return ret;
}
