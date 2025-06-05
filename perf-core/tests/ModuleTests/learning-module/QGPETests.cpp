/******************************************************************************
  @file    QGPETests.cpp
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

#define APENGINE_TEST_LIB_NAME "libapengine-tests.so"
static int (*lm_test_apengine)(const char *fileName) = NULL;


int QGPETests(TestData_t &data) {
    int8_t flag = -1;

    static void *testlibhandle = dlopen(APENGINE_TEST_LIB_NAME, RTLD_NOW | RTLD_LOCAL);
    if (!testlibhandle) {
        QLOGE(LOG_TAG, "Unable to open %s: %s\n", APENGINE_TEST_LIB_NAME,
                dlerror());
    } else {
        *(void **) (&lm_test_apengine) = dlsym(testlibhandle, "lm_test_apengine");
        char *errc = dlerror();
        if (errc != NULL) {
            QLOGE(LOG_TAG, "NRP: Unable to get lm_test_apengine function handle, %s", errc);
            dlclose(testlibhandle);
            testlibhandle = NULL;
        }
    }
    if (lm_test_apengine != NULL)
        flag = lm_test_apengine(data.mTestDetailedFile.c_str());

    return flag;
}

//interface for TTF
static RegressionFramework ttfapenginel = {
   QGPE_TEST, QGPETests, QGPE,
};
