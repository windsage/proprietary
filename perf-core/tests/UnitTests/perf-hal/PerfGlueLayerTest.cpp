/******************************************************************************
  @file  PerfGlueLayerTest.cpp
  @brief test module to test hal

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include "PerfGlueLayer.h"
#include "RegressionFramework.h"
class PerfGlueLayerTest: public PerfGlueLayer {

public:
    bool multThrdPass = true;

    int8_t SimpleTest();
    int8_t SimpleTest2();
    int8_t SimpleReloadTest();
    int8_t SimpleNonExistantLibLoadTest();
    int8_t MultiThreadedTest(uint8_t numthreads = 0);
    int8_t StressTest();
    int8_t OverNightStressTest();
    int8_t Verify();
};

int32_t PerfGlueLayerTests(TestData_t &data) {
    int8_t flags = data.mTestId;
    int8_t ret = -1, ret2 = -1;
    PerfGlueLayerTest tGL;
    int32_t testRes = FAILED;
    int8_t res = SUCCESS;

    //tests
    if (flags == TESTS_SIMPLE) {
        ret = tGL.SimpleTest();
        ret2 = tGL.Verify();
        tGL.PerfGlueLayerExit();
        if ((ret >= 0) && !ret2) {
            printf("SimpleTest Passed\n");
        }
        else {
            printf("SimpleTest Failed\n");
            res = FAILED;
        }

        ret = tGL.SimpleTest2();
        ret2 = tGL.Verify();
        if ((ret >= 0) && (ret2 == FAILED))
            printf("SimpleTest2 Passed\n");
        else {
            res = FAILED;
            printf("SimpleTest2 Failed\n");
        }

        ret = tGL.SimpleReloadTest();
        //ret2 = tGL.Verify();
        if ((ret >= 0))
            printf("SimpleReloadTest Passed\n");
        else {
            printf("SimpleReloadTest Failed\n");
            res = FAILED;
        }

        ret = tGL.SimpleNonExistantLibLoadTest();
        //ret2 = tGL.Verify();
        if ((ret >= 0))
            printf("SimpleNonExistantLibLoadTest Passed\n");
        else {
            printf("SimpleNonExistantLibLoadTest Failed\n");
            res = FAILED;
        }
        if (res == SUCCESS) {
            testRes = SUCCESS;
        }
    }

    if (flags == TESTS_MULTITHREADED) {
        ret = tGL.MultiThreadedTest();
        ret2 = tGL.Verify();
        if ((ret >= 0) && (ret2 >= 0)) {
            printf("MultiThreadedTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("MultiThreadedTest Failed\n");
    }

    if (flags == TESTS_STRESS) {
        ret = tGL.StressTest();
        ret2 = tGL.Verify();
        if ((ret >= 0) && (ret2 >= 0)) {
            printf("StressTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("StressTest Failed\n");
    }

    if (flags == TESTS_NIGHT_STRESS) {
        ret = tGL.OverNightStressTest();
        ret2 = tGL.Verify();
        if ((ret >= 0) && (ret2 >= 0)) {
            printf("OverNightStressTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("OverNightStressTest Failed\n");
    }

    tGL.PerfGlueLayerExit();

    return testRes;
}

int8_t PerfGlueLayerTest::Verify() {
    int8_t ret = 0;

    for (uint8_t i=0; i<MAX_MODULES; i++) {
        if (!mModules[i].IsEmpty()) {
            ret++;
        }
    }
    return ret == MAX_MODULES ? 0 : -1;
}

int8_t PerfGlueLayerTest::SimpleTest() {
    int8_t ret = -1, numReg = 0;

    PerfGlueLayerExit();

    ret = LoadPerfLib("libqti-tests-mod1.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = LoadPerfLib("libqti-tests-mod2.so");
    if (ret >= 0) {
        numReg++;
    }

    ret = LoadPerfLib("libqti-tests-mod3.so");
    if (ret >= 0) {
        numReg++;
    }

    ret = LoadPerfLib("libqti-tests-mod4.so");
    if (ret >= 0) {
        numReg++;
    }

    ret = LoadPerfLib("libqti-tests-mod5.so");
    if (ret >= 0) {
        numReg++;
    }

    ret = LoadPerfLib("libqti-tests-mod6.so");
    if (ret >= 0) {
        numReg++;
    }

    ret = -1;
    //possible that already registered modules with glue will lead to less successful load test libs
    if (numReg == MAX_MODULES) {
        ret = 0;
    }

    return ret;
}

int8_t PerfGlueLayerTest::SimpleTest2() {
    int8_t ret = -1, numReg = 0;

    PerfGlueLayerExit();

    for (uint8_t i=0; i<5; i++) {
        ret = LoadPerfLib("libqti-tests-mod1.so");
        if (ret >= 0) {
            numReg++;
        }
        ret = LoadPerfLib("libqti-tests-mod2.so");
        if (ret >= 0) {
            numReg++;
        }
        ret = LoadPerfLib("libqti-tests-mod3.so");
        if (ret >= 0) {
            numReg++;
        }
        ret = LoadPerfLib("libqti-tests-mod4.so");
        if (ret >= 0) {
            numReg++;
        }
        ret = -1;
        if (numReg == MAX_MODULES) {
           ret = 0;
        }

        numReg = 0;
        PerfGlueLayerExit();
    }

    return ret;
}

int8_t PerfGlueLayerTest::SimpleReloadTest() {
    int8_t ret = -1, numReg = 0;

    PerfGlueLayerExit();

    for (uint8_t i=0; i<10; i++) {
        ret = LoadPerfLib("libqti-tests-mod1.so");
        if (ret >= 0) {
            numReg++;
        }
    }

    ret = -1;
    if (numReg == 1) {
       ret = 0;
    }

    PerfGlueLayerExit();

    return ret;
}

int8_t PerfGlueLayerTest::SimpleNonExistantLibLoadTest() {
    int8_t ret = -1, numReg = 0;

    //clear all if any registered modules
    PerfGlueLayerExit();

    for (uint8_t i=0; i<10; i++) {
        ret = LoadPerfLib("libqti-nonexist.so");
        if (ret >= 0) {
            numReg++;
        }
    }

    ret = -1;
    if (numReg == 0) {
       ret = 0;
    }

    PerfGlueLayerExit();

    return ret;
}

void *RegisterMultithreadedMods(void *data) {
    int8_t ret = -1, numReg = 0;
    const uint8_t NUM_LOAD_LIBS = 6;

    PerfGlueLayerTest *glue1 = (PerfGlueLayerTest *)data;

    if (NULL == glue1) {
        return NULL;
    }

    ret = glue1->LoadPerfLib("libqti-tests-mod1.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = glue1->LoadPerfLib("libqti-tests-mod2.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = glue1->LoadPerfLib("libqti-tests-mod3.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = glue1->LoadPerfLib("libqti-tests-mod4.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = glue1->LoadPerfLib("libqti-tests-mod5.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = glue1->LoadPerfLib("libqti-tests-mod6.so");
    if (ret >= 0) {
        numReg++;
    }

    if (numReg != NUM_LOAD_LIBS)
        glue1->multThrdPass = false;

    return NULL;
}

int8_t PerfGlueLayerTest::MultiThreadedTest(uint8_t numthreads) {
   int8_t rc;
   uint8_t i;
   int8_t ret = 0;
   const uint8_t NUM_THREADS = 20;
   pthread_t threads[MAX_THREADS];
   pthread_attr_t attr;
   void *status;

   PerfGlueLayerExit();

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if (0 == numthreads) {
       numthreads = NUM_THREADS;
   }

   for( i=0; i < numthreads; i++ ){
        pthread_create(&threads[i], &attr, RegisterMultithreadedMods, (void *)this );
   }

   //free attribute and wait for the other threads
   pthread_attr_destroy(&attr);

   for( i=0; i < numthreads; i++ ){
      rc = pthread_join(threads[i], &status);
   }

   PerfGlueLayerExit();

    if (!multThrdPass) {
        ret = -1;
        multThrdPass = true;
    }

    return ret;
}

int8_t PerfGlueLayerTest::StressTest() {
    int8_t ret = 0;
    ret = MultiThreadedTest(100);

    for (uint16_t i=0; i<1000;i++) {
        ret = MultiThreadedTest(50);
    }
    return ret;
}

int8_t PerfGlueLayerTest::OverNightStressTest() {
    int8_t ret = 0;
    for (uint32_t i=0; i<100000;i++) {
        ret = StressTest();
    }
    return ret;
}

//interface for TTF
static RegressionFramework ttfglue1 = {
    "PerfGlueLayerTests_TESTS_SIMPLE", PerfGlueLayerTests, PERF_HAL,
};
static RegressionFramework ttfglue2 = {
    "PerfGlueLayerTests_TESTS_MULTITHREADED", PerfGlueLayerTests, PERF_HAL,
};
static RegressionFramework ttfglue3 = {
    "PerfGlueLayerTests_TESTS_STRESS", PerfGlueLayerTests, PERF_HAL,
};

static RegressionFramework ttfglue4 = {
    "PerfGlueLayerTests_TESTS_NIGHT_STRESS", PerfGlueLayerTests, PERF_HAL,
};

