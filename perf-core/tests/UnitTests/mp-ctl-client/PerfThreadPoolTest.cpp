/******************************************************************************
  @file  PerfThreadPoolTest.cpp
  @brief test module to test PerfThreadPool

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOG_TAG "ANDR-PERF-TEST-TP"
#include <pthread.h>
#include <stdio.h>
#include "PerfThreadPool.h"
#include "RegressionFramework.h"
#include "MpctlUtils.h"
#include <unistd.h>
#include "PerfLog.h"



class PerfThreadPoolTest {

public:
    bool multThrdPass = true;

    int8_t SimpleTest();
    int8_t SimpleTest2();
    int8_t SimpleResizeTest();
    int8_t MultiThreadedTest(uint8_t numthreads = 0);
    int8_t StressTest();
    int8_t OverNightStressTest();
};

#define mSleep(x) [=]() { \
                  usleep(x); \
                  }

int32_t PerfThreadPoolTests(TestData_t &data) {
    int8_t flags = data.mTestId;
    int8_t ret = SUCCESS;
    int32_t ret2 = FAILED;
    PerfThreadPoolTest ptpt;

    //tests
    if (flags == TESTS_SIMPLE) {
        if (ptpt.SimpleTest() == SUCCESS) {
            printf("SimpleTest Passed\n");
        } else {
            ret = FAILED;
            printf("SimpleTest Failed\n");
        }

        if (ptpt.SimpleTest2() == SUCCESS) {
            printf("SimpleTest2 Passed\n");
        } else {
            ret = FAILED;
            printf("SimpleTest2 Failed\n");
        }

        if (ptpt.SimpleResizeTest() == SUCCESS) {
            printf("SimpleResizeTest Failed\n");
        } else {
            ret = FAILED;
            printf("SimpleResizeTest Failed\n");
        }
        if (ret == SUCCESS) {
            ret2 = SUCCESS;
        }
    }

    return ret2;
}

int8_t PerfThreadPoolTest::SimpleTest() {
    int8_t ret = 0;
    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool();
    for (uint8_t i = 0;i < 5; i++) {

        if(ptp.placeTask(mSleep(5000000)) == SUCCESS) {
            ret++;
        }
    }
    if (ret == 4) {
        ret = SUCCESS;
    } else {
        ret = FAILED;
    }
    return ret;
}

int8_t PerfThreadPoolTest::SimpleTest2() {
    int8_t ret = 0;
    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool();
    for (uint8_t i = 0;i < 100; i++) {
        int32_t rc = ptp.placeTask([=]() {
                QLOGE(LOG_TAG, "%" PRIu8, i);
                });
        if(rc == FAILED) {
            ret++;
        }
    }
    if (ret == 0) {
        ret = SUCCESS;
    } else {
        ret = FAILED;
    }
    return ret;
}

int8_t PerfThreadPoolTest::SimpleResizeTest() {
    int8_t ret = 0, rc = SUCCESS;
    bool resized = false;
    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool();
    for (uint8_t i = 0;i < MAX_NUM_THREADS + 1; i++) {
        do {
            try {
                rc = ptp.placeTask(mSleep(10000000));
            } catch (std::exception &e) {
                QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
            } catch (...) {
                QLOGE(LOG_TAG, "Exception caught in %s", __func__);
            }
            if (rc == FAILED && resized == false) {
                ptp.resize(1);
                resized = true;
            } else {
                break;
            }
        } while(true);
        if (rc == FAILED) {
            ret++;
        }
    }
    if (ret <= 1) {
        ret = SUCCESS;
    } else {
        ret = FAILED;
    }
    return ret;
}

void *RegisterMultithreadedTest(void *) {
    return NULL;
}

int8_t PerfThreadPoolTest::MultiThreadedTest(uint8_t numthreads) {
   int32_t rc;
   uint8_t i;
   int8_t ret = 0;
   const uint8_t num_threads = 20;
   pthread_t threads[MAX_THREADS];
   pthread_attr_t attr;
   void *status;


   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if (0 == numthreads) {
       numthreads = num_threads;
   }

   for( i=0; i < numthreads; i++ ) {
        pthread_create(&threads[i], &attr, RegisterMultithreadedTest, (void *)this );
    }

   //free attribute and wait for the other threads
   pthread_attr_destroy(&attr);

   for( i=0; i < numthreads; i++ ){
      rc = pthread_join(threads[i], &status);
   }


    if (!multThrdPass) {
        ret = -1;
        multThrdPass = true;
    }

    return ret;
}

int8_t PerfThreadPoolTest::StressTest() {
    int8_t ret = 0;
    ret = MultiThreadedTest(100);

    for (uint16_t i=0; i<1000;i++) {
        ret = MultiThreadedTest(50);
    }
    return ret;
}

int8_t PerfThreadPoolTest::OverNightStressTest() {
    int8_t ret = 0;
    for (uint32_t i=0; i<100000;i++) {
        ret = StressTest();
    }
    return ret;
}

//interface for TTF
static RegressionFramework ttfclient = {
   "PerfThreadPoolTests_TESTS_SIMPLE", PerfThreadPoolTests, MPCTL_CLIENT,
};

