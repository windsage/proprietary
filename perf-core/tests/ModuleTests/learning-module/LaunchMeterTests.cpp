/******************************************************************************
  @file    LaunchMeterTests.cpp
  @brief   Implementation of QGPE Tests

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "RegressionFramework.h"
#include "PerfLog.h"
#include <dlfcn.h>

#define LUNCH_METER_TEST_LIB_NAME "liblunchmeter-tests.so"
static int (*lm_test_launchmeter)(const char *fileName) = NULL;


int LLTests(TestData_t &data) {
    int8_t flag = -1;

    static void *testlibhandle = dlopen(LUNCH_METER_TEST_LIB_NAME, RTLD_NOW | RTLD_LOCAL);
    if (!testlibhandle) {
        QLOGE(LOG_TAG, "Unable to open %s: %s\n", LUNCH_METER_TEST_LIB_NAME,
                dlerror());
    } else {
        *(void **) (&lm_test_launchmeter) = dlsym(testlibhandle, "lm_test_launchmeter");
        char *errc = dlerror();
        if (errc != NULL) {
            QLOGE(LOG_TAG, "NRP: Unable to get lm_test_launchmeter function handle, %s", errc);
            dlclose(testlibhandle);
            testlibhandle = NULL;
        }
    }
    if (lm_test_launchmeter != NULL)
        flag = lm_test_launchmeter(data.mTestDetailedFile.c_str());

    return flag;
}

//interface for TTF
static RegressionFramework ttflunchmeterl = {
   LL_TEST, LLTests, LL,
};
