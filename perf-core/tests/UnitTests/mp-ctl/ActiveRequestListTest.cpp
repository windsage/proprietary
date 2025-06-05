/******************************************************************************
  @file  ActiveRequestList.cpp
  @brief test module to test

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include "Request.h"
#include "ActiveRequestList.h"
#include "RegressionFramework.h"

class ActiveRequestListTest {
public:
    ActiveRequestListTest();
    uint8_t  MAX_REQUESTS;
    static void *DoMultithreadedWork(void *lst);
    int8_t BasicTests(ActiveRequestList *arl);
    int8_t MultithreadedBasicTests(ActiveRequestList *arl);

    int8_t SimpleTest();
    int8_t SimpleTest2();
    int8_t MultiThreadedTest(uint8_t numthreads = 0);
    int8_t StressTest();
    int8_t OverNightStressTest();
};

ActiveRequestListTest::ActiveRequestListTest() {
    MAX_REQUESTS = ActiveRequestList().ACTIVE_REQS_MAX;
}

int32_t ActiveRequestListTests(TestData_t &data) {
    int8_t flags = data.mTestId;
    int8_t ret = -1;
    ActiveRequestListTest aRLt;
    int32_t testRes = FAILED;
    int8_t res = SUCCESS;

    //tests
    if (flags == TESTS_SIMPLE) {
        ret = aRLt.SimpleTest();
        if (ret >= 0)
            printf("SimpleTest Passed\n");
        else {
            printf("SimpleTest Failed\n");
            res = FAILED;
        }
        ret = aRLt.SimpleTest2();
        if (ret >= 0)
            printf("SimpleTest2 Passed\n");
        else {
            printf("SimpleTest2 Failed\n");
            res = FAILED;
        }
        if (res == SUCCESS) {
            testRes = SUCCESS;
        }
    }

    if (flags == TESTS_MULTITHREADED) {
        ret = aRLt.MultiThreadedTest();
        if (ret >= 0) {
            printf("MultiThreadedTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("MultiThreadedTest Failed\n");
    }

    if (flags == TESTS_STRESS) {
        ret = aRLt.StressTest();
        if (ret >= 0) {
            printf("StressTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("StressTest Failed\n");
    }

    if (flags == TESTS_NIGHT_STRESS) {
        ret = aRLt.OverNightStressTest();
        if (ret >= 0) {
            printf("OverNightStressTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("OverNightStressTest Failed\n");
    }

    return testRes;
}

int8_t ActiveRequestListTest::BasicTests(ActiveRequestList *arl) {
    int8_t ret = -1;
    uint8_t i =0;
    Request *req[MAX_REQUESTS];
    uint8_t partial_count = 10;

    if (NULL == arl) {
        return ret;
    }

    for (uint8_t i=0; i<MAX_REQUESTS; i++) {
        req[i] = NULL;
    }

    for (uint8_t i = 0; i<MAX_REQUESTS; i++) {
        req[i] = new Request(2000, (pid_t)(1000+i), (pid_t) (1000+i), REGULAR_CLIENT);
        arl->Add(req[i], i+1);
    }

    ret = 0;
    if (MAX_REQUESTS != arl->GetNumActiveRequests()) {
        ret = -1;
    }

    if (ret < 0) {
        return ret;
    }

    //verify handles
    ret = 0;
    for (i=0; i<MAX_REQUESTS; i++) {
        if (NULL == arl->DoesExists(i+1)) {
            ret = -1;
            break;
        }
    }
    if (ret < 0) {
        return ret;
    }

    //verify active reqs
    for (i=0; i<MAX_REQUESTS; i++) {
        if (NULL == arl->IsActive(i+1, req[i])) {
            ret = -1;
            break;
        }
    }

    if (ret < 0) {
        return ret;
    }

    if (MAX_REQUESTS != arl->GetNumActiveRequests()) {
        ret = -1;
    }

    if (ret < 0) {
        return ret;
    }

    //verify remove
    for (i=0; i<(MAX_REQUESTS - partial_count); i++) {
        arl->Remove(i + 1);
    }
    if ((MAX_REQUESTS - partial_count) != arl->GetNumActiveRequests()) {
        ret = -1;
    }
    if (ret < 0) {
        return ret;
    }
    for (i=0; i<(MAX_REQUESTS - partial_count); i++) {
        if (NULL != arl->DoesExists(i+1)) {
            ret = -1;
            break;
        }
    }
    if (ret < 0) {
        return ret;
    }

    for (i=(MAX_REQUESTS - partial_count); i<MAX_REQUESTS; i++) {
        if (NULL == arl->DoesExists(i+1)) {
            ret = -1;
            break;
        }
    }
    if (ret < 0) {
        return ret;
    }

    //verify reset
    arl->Reset();
    if (0 != arl->GetNumActiveRequests()) {
        ret = -1;
    }

    if (ret < 0) {
        return ret;
    }

    return ret;
}

int8_t ActiveRequestListTest::MultithreadedBasicTests(ActiveRequestList *arl) {
    int8_t ret = -1;
    uint8_t i =0;
    Request *req[MAX_REQUESTS];

    if (NULL == arl) {
        return ret;
    }

    for (uint8_t i=0; i<MAX_REQUESTS; i++) {
        req[i] = NULL;
    }

    for (uint8_t i = 0; i<MAX_REQUESTS; i++) {
        req[i] = new Request(2000, (pid_t)(1000+i), (pid_t) (1000+i), REGULAR_CLIENT);
        arl->Add(req[i], i+1);
    }

    for (i=0; i<MAX_REQUESTS; i++) {
        arl->Remove(i + 1);
    }
    return 0;
}

int8_t ActiveRequestListTest::SimpleTest() {
    int8_t ret = -1;
    ActiveRequestList *arl = new ActiveRequestList;
    if (NULL != arl) {
        ret = BasicTests(arl);
        delete arl;
    }
    return ret;
}

int8_t ActiveRequestListTest::SimpleTest2() {
    int8_t ret = -1;
    ActiveRequestList *arl = NULL;

    for (uint8_t i=0; i<100; i++) {
        arl = new ActiveRequestList;
        if (NULL != arl) {
            ret = BasicTests(arl);
            delete arl;
            if (ret < 0) {
                break;
            }
        }
    }

    return ret;
}


typedef struct {
    ActiveRequestList *aRL;
    ActiveRequestListTest *aRLT;
    int8_t result;
}ArlIOStruct;

void *ActiveRequestListTest::DoMultithreadedWork(void *lst) {
    int8_t ret = -1;
    ArlIOStruct *tmp = (ArlIOStruct *) lst;
    if (NULL == tmp) {
        return NULL;
    }
    ActiveRequestList *arl = tmp->aRL;
    ActiveRequestListTest *arlt = tmp->aRLT;
    if ((NULL != arl) && (NULL != arlt)) {
        ret = arlt->MultithreadedBasicTests(arl);
        tmp->result = ret;
    }

    return NULL;
}

int8_t ActiveRequestListTest::MultiThreadedTest(uint8_t numthreads) {
   int8_t ret = -1, i = 0;
   const uint8_t NUM_THREADS = 20;
   pthread_t threads[MAX_THREADS];
   ArlIOStruct structs[MAX_THREADS];
   ActiveRequestList *arl = NULL;
   pthread_attr_t attr;
   void *status;

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if (numthreads == 0) {
       numthreads = NUM_THREADS;
   }

   arl = new ActiveRequestList;

   for( i=0; i < numthreads; i++ ){
      structs[i].result = -1;
      structs[i].aRL = arl;
      structs[i].aRLT = this;
      pthread_create(&threads[i], &attr, DoMultithreadedWork, (void *)&structs[i]);
   }

   //free attribute and wait for the other threads
   pthread_attr_destroy(&attr);

   for( i=0; i < numthreads; i++ ){
      pthread_join(threads[i], &status);
   }

   if (NULL != arl) {
       ret  = 0;
       if (0 != arl->GetNumActiveRequests()){
           ret =-1;
       }
       delete arl;
   }

   return ret;
}

int8_t ActiveRequestListTest::StressTest() {
    int8_t ret = 0;
    ret = MultiThreadedTest(100);

    for (uint16_t i=0; i<1000;i++) {
        ret = MultiThreadedTest(50);
    }
    return ret;
}

int8_t ActiveRequestListTest::OverNightStressTest() {
    int8_t ret = 0;
    for (uint32_t i=0; i<100000;i++) {
        ret = SimpleTest();
        ret = SimpleTest2();
        ret = MultiThreadedTest(100);

        for (uint16_t j=0; j<1000;j++) {
            ret = MultiThreadedTest(50);
        }
    }
    return ret;
}

//interface for TTF
static RegressionFramework ttfarl = {
   "ActiveRequestListTests_TESTS_SIMPLE", ActiveRequestListTests, MPCTL,
};

static RegressionFramework ttfar2 = {
   "ActiveRequestListTests_TESTS_MULTITHREADED", ActiveRequestListTests, MPCTL,
};

static RegressionFramework ttfar3 = {
   "ActiveRequestListTests_TESTS_STRESS", ActiveRequestListTests, MPCTL,
};

static RegressionFramework ttfar4 = {
   "ActiveRequestListTests_TESTS_NIGHT_STRESS", ActiveRequestListTests, MPCTL,
};
