/***********************************************************
 Copyright (c) 2023-2024 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <utils/Log.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "IAppLoader.h"
#include "ISMCIExample.h"
#include "ISMCIExampleApp.h"
#include "TZCom.h"
#include "IAppController.h"
#include "object.h"
#include "IClientEnv.h"
#include "CAppLoader.h"
#include "ICallbackObjectExample.h"
#include "CCallbackObjectExample.c"
#include "CCallbackObjectExample_open.h"
#include "utils.c"
#include <linux/dma-buf.h>
#include <BufferAllocator/BufferAllocatorWrapper.h>
#include "smcinvoke_example.h"

#define TEST_OK(input)                                       \
  {                                                          \
    if (input) {                                             \
      ALOGE("%s failed\n",__func__);                         \
      return 0;                                              \
    }                                                        \
  }

unsigned char *v_addr = NULL;

static const char *stringFromHash(const uint8_t *inHash, size_t inHashLen)
{
  static char outString[(2 * ISMCIExampleApp_HASH_SHA256_SIZE) + 1];
  size_t index = 0;

  for (size_t i = 0; i < inHashLen; i++) {
    index += snprintf(outString + index, sizeof(outString) - index, "%02X", inHash[i]);
  }

  return outString;
}


/* The implementation for this example resides primarily in CSMCIExampleApp and
 * CAdder. This example demonstrates the usage of TA services. Services allow a
 * caling TA to use an interface exposed by a service TA, so long as the calling
 * TA has the privilege to access it defined in its SConscript. */
static int32_t run_service_example(Object *appObj)
{
  int32_t ret = Object_OK;
  ret = ISMCIExampleApp_serviceExample(*appObj);
  return ret;
}

static int32_t run_hash_example(Object *appObj)
{
  int32_t ret = Object_OK;

  const char stringToHash[] = "String to hash";

  /* We use -1 for the size, since we don't want to hash the trailing '\0' character. */
  const size_t strLen = sizeof(stringToHash) - 1;

  uint8_t digest[ISMCIExampleApp_HASH_SHA256_SIZE];
  const size_t digestLen = sizeof(digest);
  size_t digestLenOut = 0;
  const char *printHashString;

  ret = ISMCIExampleApp_computeHash(
    *appObj, &stringToHash, strLen, ISMCIExampleApp_HASH_SHA256, &digest, digestLen, &digestLenOut);

  if (Object_isERROR(ret)) {
    ALOGE("Computing hash failed with error: %d\n", ret);
    goto hashExampleCleanup;
  }

  if (digestLenOut == 0) {
    ALOGE("Returned hash had a length of zero!\n");
    ret = Object_ERROR;
    goto hashExampleCleanup;
  }
  printHashString = stringFromHash(digest, digestLenOut);
  ALOGI("Hash: %s\n", printHashString);

hashExampleCleanup:
  return ret;
}

static int32_t run_shared_memory_example(Object *appObj)
{
  int32_t ret = Object_OK;
  int32_t fd = -1;
  Object sharedMemObj = Object_NULL;
  /* Allocate an Dma buffer for sharing memory. */
  size_t allocSize = sysconf(_SC_PAGESIZE);
  BufferAllocator* dmaHandle = NULL;
  uint8_t *buffer = malloc(sizeof(uint8_t[allocSize]));
  if (!buffer) {
    ALOGE("Malloc failed while allocating memory to buffer \n");
    return Object_ERROR_MEM;
  }

  dmaHandle = CreateDmabufHeapBufferAllocator();

  ret = dma_alloc(dmaHandle, allocSize, &fd, &v_addr);
  if (ret)
    goto sharedMemoryExample_cleanup;

  if (memscpy(v_addr, allocSize, buffer, allocSize)) {
    ret = Object_ERROR_MAXDATA;
    goto sharedMemoryExample_cleanup;
  }

  /* Put a string into this memory. Generate a "random" number for the message. */
  char const message[] = "A random number from the CA:";
  srand(time(0));
  int randNum = rand();
  snprintf(buffer, allocSize, "%s %d", message, randNum);

  /* Call MemObj_new which will create a memory object that represents our dma buffer, with
   * read/write permission - allowing the TA-side to write data to the shared memory.
   */
  ret = TZCom_getFdObject(dup(fd), &sharedMemObj);
  if (Object_isERROR(ret) || Object_isNull(sharedMemObj)) {
    ALOGE("Mapping sharedMemObj failed!\n");
    ret = Object_ERROR;
    sharedMemObj = Object_NULL;
    goto sharedMemoryExample_cleanup;
  }

  ALOGI("Calling ISMCIExampleApp_sharedMemoryExample\n");
  /* Call into ISMCIExampleApp and pass it our memory object. */
  ret = ISMCIExampleApp_sharedMemoryExample(*appObj, sharedMemObj);
  if (Object_isERROR(ret)) {
    ALOGE("ISMCIExampleApp_sharedMemoryExample failed with error: %d!\n", ret);
  }

  /* Ensure that the last character of what we're about to print is \0. */
  ((char *)buffer)[allocSize - 1] = '\0';
  /* Print the buffer to show that the TA has modified it. */
  ALOGI("Modified buffer %s\n", (char *)buffer);

sharedMemoryExample_cleanup:
  ret = dma_unalloc(dmaHandle, allocSize, &fd, &v_addr);
  if (Object_isERROR(ret))
    ALOGE("dma_unalloc failed with return value =%d\n", ret);

  if (buffer)
    free(buffer);
  Object_ASSIGN_NULL(sharedMemObj);
  return ret;
}

static int32_t run_callback_object_example(Object *appObj)
{
  int32_t ret = Object_OK;
  Object callbackObj = Object_NULL;
  ret = CCallbackObjectExample_open(&callbackObj);
  if (Object_isERROR(ret)) {
    ALOGE("CCallbackObjectExample_open failed with error: %d!\n", ret);
    callbackObj = Object_NULL;
    goto out;
  }

  static const char msg[] = "Local object invocation.";

  /* callbackObj resides in the client side. Invoke it locally first. */
  ret = ICallbackObjectExample_print(callbackObj, msg, sizeof(msg));
  if (Object_isERROR(ret)) {
    ALOGE("ICallbackObjectExample_print failed with error: %d!\n", ret);
    goto out;
  }

  /*
   * Now pass the local object as an argument in an ISMCIExampleApp invocation. The TA will make an
   * invocation on the callback object.
   */
  ret = ISMCIExampleApp_callbackObjectExample(*appObj, callbackObj);
  if (Object_isERROR(ret)) {
    ALOGE("ISMCIExampleApp_callbackObjectExample failed with error: %d!\n", ret);
    goto out;
  }
out:
  Object_ASSIGN_NULL(callbackObj);
  return ret;
}

/* This function demonstrates how to open a TA using SMCInvoke APIs. */
int32_t run_smcinvoke_ta_example(char *appPath, char *appPath2)
{
  int32_t ret = Object_OK;
  int32_t unalloc_ret = Object_OK;

  Object clientEnv = Object_NULL;  // A Client Environment that can be used to
                                   // get an IAppLoader object

  Object appLoader = Object_NULL;  // IAppLoader object that allows us to load
                                   // the TA in the trusted environment
  Object appController1 = Object_NULL;  // AppController contains a reference to
                                        // the app itself, after loading.
  Object appController2 = Object_NULL;
  Object appObj = Object_NULL;  // An interface to our TA that allows us to send
                                // commands to it.
  Object sharedMemObj = Object_NULL;
  size_t allocSize = 0;
  int32_t fd = -1;
  uint8_t* buffer = NULL;
  BufferAllocator* dmaHandle = NULL;
  dmaHandle = CreateDmabufHeapBufferAllocator();

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

  ALOGD("Succeeded in getting apploader object.\n");
  ALOGD("appPath is %s\n", appPath);

  /* load the application */
  ret = load_app(appLoader, appPath, &appController1);
  if (Object_isERROR(ret)) {
    ALOGE("Loading the application failed with %d!\n", ret);
    appController1 = Object_NULL;
    goto cleanup;
  }

  ret = get_file_size(appPath2);
  if (ret <= 0) {
    ALOGE("get_file_size for app %s failed with ret = %d\n", appPath2, ret);
    ret = -1;
    goto cleanup;
  }

  allocSize = (size_t)ret;
  buffer = malloc(sizeof(uint8_t[allocSize]));
  if (!buffer) {
    ALOGE("Malloc failed while allocating memory to buffer\n");
    ret = Object_ERROR_MEM;
    goto cleanup;
  }

  ret = read_file(appPath2, allocSize, buffer);
  if (ret < 0)
  {
    ALOGE("read_file for app %s failed with ret = %d\n", appPath2, ret);
    goto cleanup;
  }

  ALOGD("load %s, size %zu, buffer %p\n", appPath2, allocSize, buffer);

  ret = dma_alloc(dmaHandle, allocSize, &fd, &v_addr);
  if (ret) {
    ALOGD("dma_alloc created problem\n");
    goto dma_unalloc_cleanup;
  }
  ALOGI("dma_alloc done\n");
  ret = TZCom_getFdObject(dup(fd), &sharedMemObj);
  if (Object_isERROR(ret) || Object_isNull(sharedMemObj)) {
    ALOGE("Mapping sharedMemObj failed!\n");
    ret = Object_ERROR;
    sharedMemObj = Object_NULL;
    goto dma_unalloc_cleanup;
  }

  if (memscpy(v_addr, allocSize, buffer, allocSize)) {
    ret = Object_ERROR_MAXDATA;
    goto dma_unalloc_cleanup;
  }

  ret = IAppLoader_loadFromRegion(appLoader, sharedMemObj, &appController2);
  if (Object_isERROR(ret)) {
    ALOGE("Loading Example Service TA failed with error: %d!\n", ret);
    appController2 = Object_NULL;
    goto dma_unalloc_cleanup;
  }
  ALOGI("loading TAs succeeded!\n");

  ret = IAppController_getAppObject(appController1, &appObj);
  if (Object_isERROR(ret)) {
    ALOGE("Getting the application object failed with %d!\n", ret);
    appObj = Object_NULL;
    goto dma_unalloc_cleanup;
  }

  ALOGI("Getting the application object succeeded.\n\n");

  /* Run our examples. */
  ret = run_service_example(&appObj);
  if (Object_isERROR(ret)) {
    ALOGE("Running service example failed with error: %d!\n", ret);
    goto dma_unalloc_cleanup;
  }
  ALOGI("Running service example passed\n\n");
  printf("Running service example passed\n\n");

  ret = run_hash_example(&appObj);
  if (Object_isERROR(ret)) {
    ALOGE("Running hash example failed with error: %d!\n", ret);
    goto dma_unalloc_cleanup;
  }
  ALOGI("Running hash example passed\n\n");
  printf("Running hash example passed\n\n");

  ret = run_callback_object_example(&appObj);
  if (Object_isERROR(ret)) {
    ALOGE("Running callback object example failed with error: %d!\n", ret);
  }
  ALOGI("Running callback object example passed\n\n");
  printf("Running callback object example passed\n\n");

  ret = run_shared_memory_example(&appObj);
  if (Object_isERROR(ret)) {
    ALOGE("Running shared memory example failed with error: %d!\n", ret);
    goto dma_unalloc_cleanup;
  }
  ALOGI("Running shared memory example passed\n\n");
  printf("Running shared memory example passed\n\n");

dma_unalloc_cleanup:
  unalloc_ret = dma_unalloc(dmaHandle, allocSize, &fd, &v_addr);
  if (unalloc_ret)
    ALOGE("dma_unalloc failed with return value =%d\n", unalloc_ret);

cleanup:
  if (buffer != NULL)
    free(buffer);
  Object_ASSIGN_NULL(appObj);
  if (!Object_isNull(appController1)) {
    TEST_OK(IAppController_unload(appController1));
    Object_ASSIGN_NULL(appController1);
  }
  if (!Object_isNull(appController2)) {
    TEST_OK(IAppController_unload(appController2));
    Object_ASSIGN_NULL(appController2);
  }
  Object_ASSIGN_NULL(appLoader);
  Object_ASSIGN_NULL(clientEnv);
  Object_ASSIGN_NULL(sharedMemObj);
  return ret;
}
