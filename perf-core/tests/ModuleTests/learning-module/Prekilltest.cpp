/******************************************************************************
  @file    Prekilltest.cpp
  @brief   Implementation of Prekill Tests

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

#define PREKILL_TEST_LIB_NAME "libprekill-tests.so"
static int (*lm_test_prekill)(const char *fileName) = NULL;


int PrekillTest(TestData_t &data) {
    int8_t flag = -1;

    static void *testlibhandle = dlopen(PREKILL_TEST_LIB_NAME, RTLD_NOW | RTLD_LOCAL);
    if (!testlibhandle) {
        QLOGE(LOG_TAG, "Unable to open %s: %s\n", PREKILL_TEST_LIB_NAME,
                dlerror());
    } else {
        *(void **) (&lm_test_prekill) = dlsym(testlibhandle, "lm_test_prekill");
        char *errc = dlerror();
        if (errc != NULL) {
            QLOGE(LOG_TAG, "NRP: Unable to get lm_test_prekill function handle, %s", errc);
            dlclose(testlibhandle);
            testlibhandle = NULL;
        }
    }
    if (lm_test_prekill != NULL)
        flag = lm_test_prekill(data.mTestDetailedFile.c_str());

    return flag;
}

//interface for TTF
static RegressionFramework prekill = {
   Prekill_Test, PrekillTest, MEM,
};
