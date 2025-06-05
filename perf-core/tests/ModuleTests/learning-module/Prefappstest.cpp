/******************************************************************************
  @file    Prefappstest.cpp
  @brief   Implementation of Prefapps Tests

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "RegressionFramework.h"
#include "PerfLog.h"
#include <dlfcn.h>

#define PREFAPPS_TEST_LIB_NAME "libprefapps-tests.so"
static int (*lm_test_prefapps)(const char *fileName) = NULL;


int PrefappsTest(TestData_t &data) {
    int8_t flag = -1;

    static void *testlibhandle = dlopen(PREFAPPS_TEST_LIB_NAME, RTLD_NOW | RTLD_LOCAL);
    if (!testlibhandle) {
        QLOGE(LOG_TAG, "Unable to open %s: %s\n", PREFAPPS_TEST_LIB_NAME,
                dlerror());
    } else {
        *(void **) (&lm_test_prefapps) = dlsym(testlibhandle, "lm_test_prefapps");
        char *errc = dlerror();
        if (errc != NULL) {
            QLOGE(LOG_TAG, "NRP: Unable to get lm_test_prefapps function handle, %s", errc);
            dlclose(testlibhandle);
            testlibhandle = NULL;
        }
    }
    if (lm_test_prefapps != NULL)   
        flag = lm_test_prefapps(data.mTestDetailedFile.c_str());

    return flag;
}

//interface for TTF
static RegressionFramework prefapss1 = {
   Prefapps_Test, PrefappsTest, MEM,
};
