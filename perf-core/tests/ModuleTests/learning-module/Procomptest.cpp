/******************************************************************************
  @file    Procomptest.cpp
  @brief   Implementation of Procomp Tests

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

#define PROCOMP_TEST_LIB_NAME "libprocomp-tests.so"
static int (*lm_test_procomp)(const char *fileName) = NULL;


int ProcompTest(TestData_t &data) {
    int8_t flag = -1;

    static void *testlibhandle = dlopen(PROCOMP_TEST_LIB_NAME, RTLD_NOW | RTLD_LOCAL);
    if (!testlibhandle) {
        QLOGE(LOG_TAG, "Unable to open %s: %s\n", PROCOMP_TEST_LIB_NAME,
                dlerror());
    } else {
        *(void **) (&lm_test_procomp) = dlsym(testlibhandle, "lm_test_procomp");
        char *errc = dlerror();
        if (errc != NULL) {
            QLOGE(LOG_TAG, "NRP: Unable to get lm_test_procomp function handle, %s", errc);
            dlclose(testlibhandle);
            testlibhandle = NULL;
        }
    }
    if (lm_test_procomp != NULL)
        flag = lm_test_procomp(data.mTestDetailedFile.c_str());

    return flag;
}

//interface for TTF
static RegressionFramework procomp1 = {
   Procomp_Test, ProcompTest, MEM,
};
