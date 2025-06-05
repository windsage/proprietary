/******************************************************************************
  @file  ActivityTriggerTest.cpp
  @brief test module to test activity triggers

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include "RegressionFramework.h"
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>

class ActivityTriggerTest {
public:
    void *dlhandle;
    void (*atstart)(const char *, int8_t *);
    void (*atresume)(const char *);
    void (*atpause)(const char *);
    void (*atstop)(const char *);
    void (*atinit)(void);
    void (*atdeinit)(void);
    void (*atmisc)(int8_t, const char *, int8_t, int8_t, float *);

    int8_t Load();
    int8_t Unload();
public:
    ActivityTriggerTest();
    ~ActivityTriggerTest();

    int8_t SimpleLoadUnloadLibTest();
    int8_t SimpleTest();
    int8_t SimpleTest2();
    int8_t MultiThreadedTest(uint8_t numthreads = 0);
    int8_t MultiThreadedTest2(uint8_t numthreads = 0);
    int8_t StressTest();
    int8_t OverNightStressTest();

    static void *DoTrigger(void *);
    static void *DoTrigger2(void *);
};

int32_t ActivityTriggerTests(TestData_t &data) {
    int8_t flags = data.mTestId;
    int8_t ret = -1;
    int32_t testRes = FAILED;
    int8_t res = SUCCESS;

    ActivityTriggerTest att;

    //tests
    if (flags == TESTS_SIMPLE) {
        ret = att.SimpleLoadUnloadLibTest();
        if (ret >= 0)
            printf("SimpleLoadUnloadLibTest Passed\n");
        else {
            printf("SimpleLoadUnloadLibTest Failed\n");
            res = FAILED;
        }
        ret = att.SimpleTest();
        if (ret >= 0)
            printf("SimpleTest Passed\n");
        else {
            printf("SimpleTest Failed\n");
            res = FAILED;
        }
        ret = att.SimpleTest2();
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

    res = SUCCESS;
    if (flags == TESTS_MULTITHREADED) {
        ret = att.MultiThreadedTest();
        if (ret >= 0)
            printf("MultiThreadedTest Passed\n");
        else {
            printf("MultiThreadedTest Failed\n");
            res = FAILED;
        }
        ret = att.MultiThreadedTest2();
        if (ret >= 0)
            printf("MultiThreadedTest2 Passed\n");
        else {
            printf("MultiThreadedTest2 Failed\n");
            res = FAILED;
        }
        if (res == SUCCESS) {
            testRes = SUCCESS;
        }
    }

    if (flags == TESTS_STRESS) {
        ret = att.StressTest();
        if (ret >= 0) {
            printf("StressTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("StressTest Failed\n");
    }

    if (flags == TESTS_NIGHT_STRESS) {
        ret = att.OverNightStressTest();
        if (ret >= 0) {
            printf("OverNightStressTest Passed\n");
            testRes = SUCCESS;
        }
        else
            printf("OverNightStressTest Failed\n");
    }
    return testRes;
}

ActivityTriggerTest::ActivityTriggerTest() {
    dlhandle = NULL;
    atstart = NULL;
    atresume = NULL;
    atpause = NULL;
    atstop = NULL;
    atinit = NULL;
    atdeinit = NULL;
    atmisc = NULL;
}

ActivityTriggerTest::~ActivityTriggerTest() {
}

int8_t ActivityTriggerTest::Load() {
    char *rc = NULL;

    dlhandle = dlopen("libqti-at.so", RTLD_NOW | RTLD_LOCAL);
    rc = dlerror();
    if (dlhandle == NULL) {
        if (rc != NULL) {
            printf("load dlopen failed: %s\n", rc);
        } else {
            printf("load dlopen failed\n");
        }
        return -1;
    }

    //clear off old ones
    dlerror();

    *(void **) (&atinit) = dlsym(dlhandle, "activity_trigger_init");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atstart) = dlsym(dlhandle, "activity_trigger_start");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atresume) = dlsym(dlhandle, "activity_trigger_resume");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atpause) = dlsym(dlhandle, "activity_trigger_pause");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atstop) = dlsym(dlhandle, "activity_trigger_stop");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atmisc) = dlsym(dlhandle, "activity_trigger_misc");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atdeinit) = dlsym(dlhandle, "activity_trigger_deinit");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    return 0;
}

int8_t ActivityTriggerTest::Unload() {
    int8_t ret = 0;
    ret = dlclose(dlhandle);
    return ret;
}

int8_t ActivityTriggerTest::SimpleLoadUnloadLibTest() {
    int8_t ret = 0;

    for (uint8_t i=0; i<100; i++) {
        ret = Load();
        if (ret < 0) {
            ret = -1;
            break;
        }

        ret = Unload();
        if (ret != 0) {
            ret = -1;
            break;
        }
        ret = 0;
    }

    return ret;
}

int8_t ActivityTriggerTest::SimpleTest() {
    int8_t ret = 0;
    int8_t flags = 0;
    float value = 0.0;

    ret = Load();
    if (ret < 0) {
        return -1;
    }

    if (atinit) {
        (*atinit)();
    }

    for (uint8_t i=0; i<100; i++) {
        if (atstart) {
            (*atstart)("sampleapp/sample/1.2", &flags);
        }

        if (atpause) {
            (*atpause)("sampleapp/sample/1.2");
        }

        if (atresume) {
            (*atresume)("sampleapp/sample/1.2");
        }

        if (atmisc) {
            (*atmisc)(1, "sampleapp/sample/1.2", 0, 1, &value);
        }

        if (atstop) {
            (*atstop)("sampleapp/sample/1.2");
        }
    }

    if (atdeinit) {
        (*atdeinit)();
    }

    ret = Unload();
    if (ret != 0) {
        ret = -1;
    }

    return ret;
}

int8_t ActivityTriggerTest::SimpleTest2() {
    int8_t ret = 0;
    int8_t flags = 0;
    float value = 0.0;
    char buf[128];

    ret = Load();
    if (ret < 0) {
        return -1;
    }

    if (atinit) {
        (*atinit)();
    }

    for (uint8_t i=0; i<100; i++) {
        if (atstart) {
            snprintf(buf, 128, "sampleapp%" PRIu8 "/sample%" PRIu8 "/1.2", i, i);
            (*atstart)(buf, &flags);
        }
    }

    for (uint8_t i=0; i<100; i++) {
        if (atpause) {
            snprintf(buf, 128, "sampleapp%" PRIu8 "/sample%" PRIu8 "/1.2", i, i);
            (*atpause)(buf);
        }
    }

    for (uint8_t i=0; i<100; i++) {
        if (atresume) {
            snprintf(buf, 128, "sampleapp%" PRIu8 "/sample%" PRIu8 "/1.2", i, i);
            (*atresume)(buf);
        }
    }

    for (uint8_t i=0; i<100; i++) {
        if (atmisc) {
            snprintf(buf, 128, "sampleapp%" PRIu8 "/sample%" PRIu8 "/1.2", i, i);
            (*atmisc)(i, buf, i, i+1, &value);
        }
    }

    for (uint8_t i=0; i<100; i++) {
        if (atstop) {
            snprintf(buf, 128, "sampleapp%" PRIu8 "/sample%" PRIu8 "/1.2", i, i);
            (*atstop)(buf);
        }
    }

    if (atdeinit) {
        (*atdeinit)();
    }

    ret = Unload();
    if (ret != 0) {
        ret = -1;
    }

    return ret;
}


void *ActivityTriggerTest::DoTrigger(void *ptr) {
    int8_t flags = 0;
    float value = 0.0;

    ActivityTriggerTest *att = (ActivityTriggerTest *) ptr;

    if (NULL == att) {
        return NULL;
    }

    for (uint8_t i=0; i<10; i++) {
        if (att->atstart) {
            (*att->atstart)("sampleapp/sample/1.2", &flags);
        }

        if (att->atpause) {
            (*att->atpause)("sampleapp/sample/1.2");
        }

        if (att->atresume) {
            (*att->atresume)("sampleapp/sample/1.2");
        }

        if (att->atmisc) {
            (*att->atmisc)(1, "sampleapp/sample/1.2", 0, 1, &value);
        }

        if (att->atstop) {
            (*att->atstop)("sampleapp/sample/1.2");
        }
    }

    return NULL;
}

void *ActivityTriggerTest::DoTrigger2(void *ptr) {
    int8_t flags = 0;
    float value = 0.0;
    char buf[128];

    ActivityTriggerTest *att = (ActivityTriggerTest *) ptr;

    if (NULL == att) {
        return NULL;
    }

    for (uint8_t i=0; i<5; i++) {
        if (att->atstart) {
            snprintf(buf, 128, "sampleapp%" PRIu8 "/sample%" PRIu8 "/1.2", i, i);
            (*att->atstart)(buf, &flags);
        }
    }

    for (uint8_t i=0; i<5; i++) {
        if (att->atpause) {
            snprintf(buf, 128, "sampleapp%" PRIu8 "/sample%" PRIu8 "/1.2", i, i);
            (*att->atpause)(buf);
        }
    }

    for (uint8_t i=0; i<5; i++) {
        if (att->atresume) {
            snprintf(buf, 128, "sampleapp%" PRIu8 "/sample%" PRIu8 "/1.2", i, i);
            (*att->atresume)(buf);
        }
    }

    for (uint8_t i=0; i<5; i++) {
        if (att->atmisc) {
            snprintf(buf, 128, "sampleapp%" PRIu8 "/sample%" PRIu8 "/1.2", i, i);
            (*att->atmisc)(i, buf, i, i+1, &value);
        }
    }

    for (uint8_t i=0; i<5; i++) {
        if (att->atstop) {
            snprintf(buf, 128, "sampleapp%" PRIu8 "/sample%" PRIu8 "/1.2", i, i);
            (*att->atstop)(buf);
        }
    }

    return NULL;
}

int8_t ActivityTriggerTest::MultiThreadedTest(uint8_t numthreads) {
   int8_t rc;
   uint8_t i;
   const uint8_t NUM_THREADS = 20;
   pthread_t threads[MAX_THREADS];
   pthread_attr_t attr;
   int8_t ret =0;
   void *status;

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if (numthreads == 0) {
       numthreads = NUM_THREADS;
   }

   ret = Load();
   if (ret < 0) {
       return ret;
   }

   if (atinit) {
       (*atinit)();
   }

   for( i=0; i < numthreads; i++ ){
      pthread_create(&threads[i], &attr, DoTrigger, (void *)this );
   }

   //free attribute and wait for the other threads
   pthread_attr_destroy(&attr);

   for( i=0; i < numthreads; i++ ){
      rc = pthread_join(threads[i], &status);
   }

   if (atdeinit) {
       (*atdeinit)();
   }

   ret = Unload();
   if (ret != 0) {
       ret = -1;
   }

   //success
   return ret;
}

int8_t ActivityTriggerTest::MultiThreadedTest2(uint8_t numthreads) {
   int8_t rc;
   uint8_t i;
   const uint8_t NUM_THREADS = 20;
   pthread_t threads[MAX_THREADS];
   pthread_attr_t attr;
   int8_t ret =0;
   void *status;

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if (numthreads == 0) {
       numthreads = NUM_THREADS;
   }

   ret = Load();
   if (ret < 0) {
       return ret;
   }

   if (atinit) {
       (*atinit)();
   }

   for( i=0; i < numthreads; i++ ){
      pthread_create(&threads[i], &attr, DoTrigger2, (void *)this );
   }

   //free attribute and wait for the other threads
   pthread_attr_destroy(&attr);

   for( i=0; i < numthreads; i++ ){
      rc = pthread_join(threads[i], &status);
   }

   if (atdeinit) {
       (*atdeinit)();
   }

   ret = Unload();
   if (ret != 0) {
       ret = -1;
   }

   //success
   return ret;
}

int8_t ActivityTriggerTest::StressTest() {
    int8_t ret = 0;
    int8_t flags = 0;
    float value = 0.0;
    char buf[128];

    ret = Load();
    if (ret < 0) {
        return -1;
    }

    if (atinit) {
        (*atinit)();
    }

    //same app opening
    for (uint8_t i=0; i<10; i++) {
        if (atstart) {
            (*atstart)("sampleapp/sample/1.2", &flags);
        }
    }

    //different app opening
    for (uint8_t i=0; i<10; i++) {
        snprintf(buf, 128, "sampleapp-%" PRIu8 "/sample-%" PRIu8 "/1.2", i, i);
        if (atstart) {
            (*atstart)(buf, &flags);
        }
    }

    //stop in loop
    for (uint8_t i=0; i<10; i++) {
        snprintf(buf, 128, "sampleapp-%" PRIu8 "/sample-%" PRIu8 "/1.2", i, i);
        if (atstop) {
            (*atstop)(buf);
        }
    }

    for (uint8_t i=0; i<10; i++) {
        if (atstart) {
            (*atstart)("sampleapp/sample/1.2", &flags);
        }

        if (atpause) {
            (*atpause)("sampleapp/sample/1.2");
        }

        if (atresume) {
            (*atresume)("sampleapp/sample/1.2");
        }

        if (atmisc) {
            (*atmisc)(1, "sampleapp/sample/1.2", 0, 1, &value);
        }

        if (atstop) {
            (*atstop)("sampleapp/sample/1.2");
        }
    }


    for (uint8_t i=0; i<10; i++) {
        SimpleLoadUnloadLibTest();
        SimpleTest();
        SimpleTest2();
        MultiThreadedTest(200);
        MultiThreadedTest2(200);
    }

    if (atdeinit) {
        (*atdeinit)();
    }

    ret = Unload();
    if (ret != 0) {
        ret = -1;
    }

    return ret;
}

int8_t ActivityTriggerTest::OverNightStressTest() {
    int8_t ret = 0;
    for (uint16_t i=0; i<1000;i++) {
        ret = StressTest();
    }
    return ret;
}

//interface for TTF
static RegressionFramework ttftrig1 = {
   "ActivityTriggerTests_TESTS_SIMPLE", ActivityTriggerTests, ACTIVITY_TRIGGER,
};
static RegressionFramework ttftrig2 = {
   "ActivityTriggerTests_TESTS_MULTITHREADED", ActivityTriggerTests, ACTIVITY_TRIGGER,
};
static RegressionFramework ttftrig3 = {
   "ActivityTriggerTests_TESTS_STRESS", ActivityTriggerTests, ACTIVITY_TRIGGER,
};
static RegressionFramework ttftrig4 = {
   "ActivityTriggerTests_TESTS_NIGHT_STRESS", ActivityTriggerTests, ACTIVITY_TRIGGER,
};

