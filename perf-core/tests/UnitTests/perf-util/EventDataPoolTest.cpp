/******************************************************************************
  @file  EventDataPoolTest.cpp
  @brief EventDataPool unit tests

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include "EventDataPool.h"
#include "RegressionFramework.h"

//callbacks for eventqueue
typedef struct msg {
    int num;
}msg_t;

static void *Alloccb() {
    void *mem = (void *) new msg_t;
    return mem;
}

static void Dealloccb(void *mem) {
    if (NULL != mem) {
        delete (msg_t *)mem;
    }
}

class EventDataPoolTest: public EventDataPool {
private:
    static void *DoAllocFree(void *ptr);
public:
    int8_t SimpleAllocFreeTest();
    int8_t ResizePoolTest();
    int8_t SimpleMultiObjectTest();
    int8_t MultiThreadedAllocFreeTest(uint8_t numthreads = 0);
    int8_t StressTest();
    int8_t OverNightStressTest();
};

int32_t EventDataPoolTests(TestData_t &data) {
    int8_t flags = data.mTestId;
    int8_t ret = -1;
    int32_t testRes = FAILED;
    int8_t res = SUCCESS;

    //EventData pool
    EventDataPoolTest dataPool;

    dataPool.SetCBs(Alloccb, Dealloccb);

    //tests
    if (flags == TESTS_SIMPLE) {
        ret = dataPool.SimpleAllocFreeTest();
        if (ret >= 0)
            printf("SimpleAllocFreeTest Passed\n");
        else {
            printf("SimpleAllocFreeTest Failed\n");
            res = FAILED;
        }
        ret = dataPool.ResizePoolTest();
        if (ret >= 0)
            printf("ResizePoolTest Passed\n");
        else {
            printf("ResizePoolTest Failed\n");
            res = FAILED;
        }

        ret = dataPool.SimpleMultiObjectTest();
        if (ret >= 0)
            printf("SimpleMultiObjectTest Passed\n");
        else {
            printf("SimpleMultiObjectTest Failed\n");
            res = FAILED;
        }
        if (res == SUCCESS) {
            testRes = SUCCESS;
        }
    }

    if (flags == TESTS_MULTITHREADED) {
        ret = dataPool.MultiThreadedAllocFreeTest();
        if (ret >= 0) {
            printf("MultiThreadedAllocFreeTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("MultiThreadedAllocFreeTest Failed\n");
    }

    if (flags == TESTS_STRESS) {
        ret = dataPool.StressTest();
        if (ret >= 0) {
            printf("StressTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("StressTest Failed\n");
    }


    if (flags == TESTS_NIGHT_STRESS) {
        ret = dataPool.OverNightStressTest();
        if (ret >= 0) {
            printf("OverNightStressTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("OverNightStressTest Failed\n");
    }

    return testRes;
}

int8_t EventDataPoolTest::ResizePoolTest() {
    uint16_t n = mSize;
    uint16_t allocsize1 = 0, allocsize2 = 0, freedsize=0;
    EventData **data, **data2;

    data = new EventData *[mSize*3];
    data2 = new EventData *[mSize*3];

    if (!data || !data2) {
        printf("allocation in test client failed\n");
        return -1;
    }

    //expecting empty pool
    for (uint16_t i =0; i < n+1; i++) {
        data[i] = Get();
    }

    //allocated size=n+1, freepool size=mSize-(n+1)
    allocsize1 = n+1;
    freedsize = mSize-(n+1);

    if (mSize != 2*n) {
        //failed
        for (uint16_t i =0; i < mSize; i++) {
            Return(data[i]);
        }
        delete[] data;
        delete[] data2;
        return 1; //pass as resize is not allowed in EventDataPool
    }
    Dump();

    //allocate one more time
    n = mSize;

    for (uint16_t i =0; i < allocsize1+1; i++) {
        data2[i] = Get();
    }

    //allocated size=n+1, freepool size=mSize-(n+1)
    allocsize2 = allocsize1+1;
    freedsize = mSize-allocsize2;

    if (mSize != 2*n) {
        delete[] data;
        delete[] data2;
        return -1;
    }

    //free now
    for (uint16_t i =0; i < allocsize1; i++) {
        Return(data[i]);
    }

    for (uint16_t i =0; i < allocsize2; i++) {
        Return(data2[i]);
    }

    Dump();

    delete[] data;
    delete[] data2;

    if (mAllocPool != NULL) {
        //failed
        return -1;
    }

    //pass
    return 1;
}

int8_t EventDataPoolTest::SimpleAllocFreeTest() {
    EventData *data[256];

    for (uint8_t j = 0; j< 3; j++) {

        //test scenario - simple alloc/frees, expecting empty pool
        //alloc
        for (uint8_t i =0; i < 10; i++) {
            data[i] = Get();
        }
        Dump();

        //return
        for (uint8_t i =0; i < 10; i++) {
            Return(data[i]);
        }
        Dump();

        if (mAllocPool != NULL) {
            //failed
            return -1;
        }

        //test scenario - alloc/frees in the middle, expecting empty alloc pool
        //alloc
        for (uint8_t i =0; i < 10; i=i+3) {
            data[i] = Get();
        }
        for (uint8_t i =1; i < 10; i=i+3) {
            data[i] = Get();
        }
        for (uint8_t i =0; i < 10; i=i+3) {
            Return(data[i]);
        }
        for (uint8_t i =2; i < 10; i=i+3) {
            data[i] = Get();
        }
        for (uint8_t i =1; i < 10; i=i+3) {
            Return(data[i]);
        }
        for (uint8_t i =2; i < 10; i=i+3) {
            Return(data[i]);
        }
        Dump();

        if (mAllocPool != NULL) {
            //failed
            return -1;
        }
    }
    return 1;
}

int8_t EventDataPoolTest::SimpleMultiObjectTest() {
    EventData *data1[20];
    EventData *data2[20];
    EventData *data3[20];
    EventData *data4[20];
    EventData *data5[20];

    for (uint8_t j = 0; j< 100; j++) {
        EventDataPool pool1, pool2, pool3, pool4, pool5;

        //test scenario - simple alloc/frees, expecting empty pool
        //alloc
        for (uint8_t i =0; i < 10; i++) {
            data1[i] = pool1.Get();
            data2[i] = pool2.Get();
            data3[i] = pool3.Get();
            data4[i] = pool4.Get();
            data5[i] = pool5.Get();
        }

        //return
        for (uint8_t i =0; i < 10; i++) {
            pool1.Return(data1[i]);
            pool2.Return(data2[i]);
            pool3.Return(data3[i]);
            pool4.Return(data4[i]);
            pool5.Return(data5[i]);
        }
        //Dump();

        if (mAllocPool != NULL) {
            //failed
            return -1;
        }
    }
    return 1;
}

void* EventDataPoolTest::DoAllocFree(void *ptr) {
    EventData *data[256];

    EventDataPoolTest *t = (EventDataPoolTest *) ptr;

    if (NULL == t) {
        return NULL;
    }

    //alloc 20
    for (uint8_t i =0; i < 20; i++) {
        data[i] = t->Get();
    }
    //return 10
    for (uint8_t i =10; i < 20; i++) {
        t->Return(data[i]);
    }
    //alloc 10
    for (uint8_t i =20; i < 30; i++) {
        data[i] = t->Get();
    }
    //return remaining
    for (uint8_t i =0; i < 10; i++) {
        t->Return(data[i]);
    }
    for (uint8_t i =20; i < 30; i++) {
        t->Return(data[i]);
    }

    return ptr;
}

int8_t EventDataPoolTest::MultiThreadedAllocFreeTest(uint8_t numthreads) {
   int8_t rc;
   uint8_t i;
   const uint8_t NUM_THREADS = 20;
   pthread_t threads[MAX_THREADS];
   pthread_attr_t attr;
   void *status;

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if (numthreads == 0) {
       numthreads = NUM_THREADS;
   }

   for( i=0; i < numthreads; i++ ){
      pthread_create(&threads[i], &attr, DoAllocFree, (void *)this );
   }

   //free attribute and wait for the other threads
   pthread_attr_destroy(&attr);

   for( i=0; i < numthreads; i++ ){
      rc = pthread_join(threads[i], &status);
   }

   if (mAllocPool != NULL) {
       //failed
       return -1;
   }

   //success
   return 1;
}

int8_t EventDataPoolTest::StressTest() {
    int8_t ret = 0;
    ret = MultiThreadedAllocFreeTest(30);
    if (ret < 0) {
        return ret;
    }

    ret = ResizePoolTest();
    if (ret < 0) {
        return ret;
    }
    for (uint16_t i=0; i<1000;i++) {
        ret = MultiThreadedAllocFreeTest(150);
        if (ret < 0) {
            break;
        }
    }
    return ret;
}

int8_t EventDataPoolTest::OverNightStressTest() {
    int8_t ret = 0;
    for (uint32_t i=0; i<100000;i++) {
        ret = StressTest();
    }
    return ret;
}

//interface for TTF
static RegressionFramework ttf1 = {
   "EventDataPoolTests_TESTS_SIMPLE", EventDataPoolTests, PERF_UTIL,
};

static RegressionFramework ttf2 = {
   "EventDataPoolTests_TESTS_MULTITHREADED", EventDataPoolTests, PERF_UTIL,
};

static RegressionFramework ttf3 = {
   "EventDataPoolTests_TESTS_STRESS", EventDataPoolTests, PERF_UTIL,
};

static RegressionFramework ttf4 = {
   "EventDataPoolTests_TESTS_NIGHT_STRESS", EventDataPoolTests, PERF_UTIL,
};
