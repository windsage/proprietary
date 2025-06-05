/******************************************************************************
  @file  PerfStressTest.cpp
  @brief test module to test all perflocks

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOCK_TEST_TAG "RF-STRESS-TEST-PERFLOCKS"
#include "PerfTest.h"
#include <thread>
#include <stdio.h>
#include "client.h"
#include "MemoryDetective.h"
#include "PerfLog.h"
#include "OptsData.h"

#define MAX_REQUEST_DURATION 5000
#define HRS_TO_SEC(X) (X * 60 * 60)
#define NUM_TASK 6
//increate this NUM_TASK when adding new methods to stress test.
#define MEM_LEAK_PERC_THRES 5.0
#define MEM_LEAK_CRITICAL_PERC_THRES 15.0
#define MEM_LEAK_COUNT_THRES 5
#define MIN_NUMBER_SAMPLES 5

void PerfStressTest::PerfHintRenewTest() {

    int32_t old_handle = FAILED;
    do {
        //adding same parameters to renew call as we need to submit exact same request to be renewed.
        int32_t tmp =  perf_hint_renew(old_handle, 0x00001081, NULL, MAX_REQUEST_DURATION , 6, 0, NULL);
        if (tmp > 0) {
            old_handle = tmp;
        }
    } while(getRemainingTime() > 30);

}

void PerfStressTest::PerfEventTest() {

    vector<pair<int32_t, pair<int32_t,uint32_t>>> hint_types;

    uint32_t size = mT.GetAllBoostHintType(hint_types);
    if (size == 0 || hint_types.size() <= 0) {
        QLOGE(LOCK_TEST_TAG, "Failed reading Boost Hint Types. Exiting Test %s", __func__);
        return;
    }

    QLOGD(LOCK_TEST_TAG, "Starting Test %s", __func__);
    do {
        for(auto &hint : hint_types) {
            int8_t k = rand() % MAX_REQUEST_DURATION;
            int32_t hint_id = hint.first;
            int32_t hint_type = hint.second.first;
            int32_t duration = rand() % MAX_REQUEST_DURATION;
            if (k % 2 ==0) {
                perf_event(hint_id, NULL, 0,NULL);
            } else {
                int32_t args[2] = {hint_type, duration};
                perf_event(hint_id, NULL, 2, args);
            }
        }
    } while(getRemainingTime() > 30);
    QLOGD(LOCK_TEST_TAG, "Existing Test %s", __func__);

}

void PerfStressTest::PerfEventOffloadTest() {

    vector<pair<int32_t, pair<int32_t,uint32_t>>> hint_types;
    uint32_t size = mT.GetAllBoostHintType(hint_types);
    if (size == 0 || hint_types.size() <= 0) {
        QLOGE(LOCK_TEST_TAG, "Failed reading Boost Hint Types. Exiting Test %s", __func__);
        return;
    }

    QLOGD(LOCK_TEST_TAG, "Starting Test %s", __func__);
    do {
        for(auto &hint : hint_types) {
            int8_t k = rand() % 100;
            int32_t hint_id = hint.first;
            int32_t hint_type = hint.second.first;
            if (k % 2 ==0) {
                perf_event_offload(hint_id, NULL, 0, NULL);
            } else {
                int32_t args[2] = {hint_type, 500};
                perf_event_offload(hint_id, NULL, 2, args);
            }
        }
    } while(getRemainingTime() > 30);
    QLOGD(LOCK_TEST_TAG, "Existing Test %s", __func__);
}

void PerfStressTest::PerfHintAcqRelTest() {

    vector<pair<int32_t, pair<int32_t,uint32_t>>> hint_types;
    uint32_t size = mT.GetAllBoostHintType(hint_types);
    if (size == 0 || hint_types.size() <= 0) {
        QLOGE(LOCK_TEST_TAG, "Failed reading Boost Hint Types. Exiting test %s", __func__);
        return;
    }
    int32_t old_handle = -1;
    int32_t tmp = FAILED;
    QLOGD(LOCK_TEST_TAG, "Starting Test %s", __func__);
    do {
        for(auto &hint : hint_types) {
            int32_t hint_id = hint.first;
            int32_t hint_type = hint.second.first;
            int32_t duration = rand() % MAX_REQUEST_DURATION;
            tmp  =  perf_hint_acq_rel(old_handle, hint_id, NULL, duration, hint_type, 0, NULL);
            if (tmp > 0) {
                old_handle = tmp;
            }
        }
    } while(getRemainingTime() > 30);
    QLOGD(LOCK_TEST_TAG, "Existing Test %s", __func__);
}

void PerfStressTest::PerfHintAcqRelOffloadTest() {
    vector<pair<int32_t, pair<int32_t,uint32_t>>> hint_types;
    uint32_t size = mT.GetAllBoostHintType(hint_types);
    if (size == 0 || hint_types.size() <= 0) {
        QLOGE(LOCK_TEST_TAG, "Failed reading Boost Hint Types. Exiting test %s", __func__);
        return;
    }
    int old_handle = FAILED;
    int32_t tmp = FAILED;
    QLOGD(LOCK_TEST_TAG, "Starting Test %s", __func__);
    do {
        for(auto &hint : hint_types) {
            int32_t hint_id = hint.first;
            int32_t hint_type = hint.second.first;
            tmp  =  perf_hint_acq_rel_offload(old_handle, hint_id, NULL, 500, hint_type, 0, NULL);
            if (tmp > 0) {
                old_handle = tmp;
            }
        }
    } while(getRemainingTime() > 30);
    QLOGD(LOCK_TEST_TAG, "Existing Test %s", __func__);
}

void PerfStressTest::PerfLockAcqRelTest() {
    int32_t qindex = 0;
    int32_t resource_list[2];
    int32_t old_handle = 0;
    srand(time(0));
    QLOGD(LOCK_TEST_TAG, "Starting Test %s", __func__);
    do {
        qindex = rand() % MAX_MINOR_RESOURCES;
        int32_t duration = rand() % MAX_REQUEST_DURATION;
        auto tmp = mPT.mTestResourceConfigInfo.find(qindex);
        if (tmp == mPT.mTestResourceConfigInfo.end()) {
            continue;
        }
        testResourceConfigInfo &resource = tmp->second;
        resource_list[0] = mPT.getOpcode(resource);
        resource_list[1] = resource.mWriteValue;
        old_handle = perf_lock_acq_rel(old_handle, duration, resource_list, 2, 0);


    } while(getRemainingTime() > 30);
    QLOGD(LOCK_TEST_TAG, "Existing Test %s", __func__);
}

int32_t PerfStressTest::getRemainingTime() {
    int32_t time_delta = 0;
    struct timespec mTestEnd;
    clock_gettime(CLOCK_MONOTONIC, &mTestEnd);
    time_delta = (mTestEnd.tv_sec - mTestStart.tv_sec + ((mTestStart.tv_nsec - mTestEnd.tv_nsec)/(1000*1000*1000)));
    if (getKeepRunning() == false) {
        return 0;
    }
    return (mRuntime - time_delta);
}

int32_t PerfStressTest::RunAPIStressTest(TestData_t &data) {
    int32_t rc = FAILED;
    MemoryDetective memDetective;
    clock_gettime(CLOCK_MONOTONIC, &mTestStart);
    mRuntime = HRS_TO_SEC(data.mDuration);
    std::vector<std::thread> workers;
    PerfStressTest &pst = getPerfStressTest();
    data.mWorkers = data.mWorkers < NUM_TASK ? NUM_TASK : data.mWorkers;
    pst.setKeepRunning(true);
    int32_t initialWaitTime = 2000;//ms
    for(int32_t i = 0 ; i < data.mWorkers;i++) {
        switch (i % NUM_TASK) {
            case 0:
                workers.push_back(std::thread([&]() {
                            mPT.gotoSleep(initialWaitTime); //start after initialWaitTime ms so initial data can be collected.
                            pst.PerfHintRenewTest();
                            }));
                break;
            case 1:
                workers.push_back(std::thread([&]() {
                            mPT.gotoSleep(initialWaitTime); //start after initialWaitTime ms so initial data can be collected.
                            pst.PerfEventTest();
                            }));
                break;
            case 2:
                workers.push_back(std::thread([&]() {
                            mPT.gotoSleep(initialWaitTime); //start after initialWaitTime ms so initial data can be collected.
                            pst.PerfEventOffloadTest();
                            }));
                break;
            case 3:
                workers.push_back(std::thread([&]() {
                            mPT.gotoSleep(initialWaitTime); //start after initialWaitTime ms so initial data can be collected.
                            pst.PerfHintAcqRelTest();
                            }));
                break;
            case 4:
                workers.push_back(std::thread([&]() {
                            mPT.gotoSleep(initialWaitTime); //start after initialWaitTime ms so initial data can be collected.
                            pst.PerfHintAcqRelOffloadTest();
                            }));
                break;
            case 5:
                workers.push_back(std::thread([&]() {
                            mPT.gotoSleep(initialWaitTime); //start after initialWaitTime ms so initial data can be collected.
                            pst.PerfLockAcqRelTest();
                            }));
                break;
            default:
                break;
        }
    }
    atomic<int32_t> exitStatus = FAILED;
    atomic<bool> triggerTerminate = false;
    memDetective.assignDetective(data, triggerTerminate, exitStatus, mRuntime);
    while(!triggerTerminate.load()) {
        PerfTest::gotoSleep(1000 * mRuntime * 1);
    }
    setKeepRunning(false);
    rc = exitStatus.load();

    pst.setKeepRunning(false);

    for (uint32_t i = 0;i < workers.size();i++ ) {
        workers[i].join();
    }
    return rc;
}

int32_t PerfStressTest::RunMeoryTest(TestData_t &data) {

    int32_t rc = FAILED;
    MemoryDetective memDetective;
    clock_gettime(CLOCK_MONOTONIC, &mTestStart);
    mRuntime = HRS_TO_SEC(data.mDuration);
    setKeepRunning(true);
    atomic<int32_t> exitStatus = FAILED;
    atomic<bool> triggerTerminate = false;
    memDetective.assignDetective(data, triggerTerminate, exitStatus, mRuntime);
    while(!triggerTerminate.load()) {
        PerfTest::gotoSleep(1000 * mRuntime * 1);
    }
    setKeepRunning(false);
    rc = exitStatus.load();
    return rc;
}

int32_t PerfStressTest::getFieldValue(char *line, const char *field) {
    int32_t value = -1;
    char *match = NULL;

    if ((NULL == line) || (NULL == field)) {
        QLOGE(LOCK_TEST_TAG, "line or field NULL");
        return value;
    }

    match = strstr(line, field);
    if (NULL != match) {
        match = match + strlen(field);
        value = atoi(match);
        QLOGI(LOCK_TEST_TAG, "Matched [%s]: [%d]", line, value);
    }
    return value;
}

int32_t PerfStressTest::getPerfHalPID() {
    char buff[NODE_MAX] = {0};
    int32_t rc = FAILED;
    memset(buff, 0, sizeof(buff));
    FREAD_BUF_STR(PERF_PID_NODE, buff, NODE_MAX, rc);
    int32_t pid = strtol(buff, NULL, 0);
    return pid;
}

void PerfStressTest::setKeepRunning(bool val) {
    mKeepRunning = val;
}

bool PerfStressTest::getKeepRunning() {
    return mKeepRunning;
}

PerfStressTest::PerfStressTest() {
    mRuntime = 0;
}

int32_t PerfStressTest::getSystemTime(string &time) {
    int32_t rc = FAILED;
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    char * timestr = std::ctime(&end_time);
    if (timestr != NULL) {
        time = string(timestr);
        rc = SUCCESS;
    }
    return rc;
}


int32_t PerfStressTest::RunTest(TestData_t &data) {
    int32_t rc = FAILED;
    PerfStressTest &mPST = PerfStressTest::getPerfStressTest();
    if (data.mTestName == RF_PERF_API_STRESS_TEST) {
       rc = mPST.RunAPIStressTest(data);

    } else if (data.mTestName == RF_PERF_MEMORY_TEST) {
       rc = mPST.RunMeoryTest(data);
    } else {
        QLOGE(LOCK_TEST_TAG, "Invalid Test id Passed");
    }
    return rc;
}

PerfStressTest &PerfStressTest::getPerfStressTest() {
    static PerfStressTest mPerfStressTest;
    return mPerfStressTest;
}

//interface for TTF
static RegressionFramework ttfarl = {
   RF_PERF_API_STRESS_TEST,  PerfStressTest::RunTest, PERF_HAL, TESTS_NIGHT_STRESS,
};

static RegressionFramework rfmemoryTest = {
   RF_PERF_MEMORY_TEST,  PerfStressTest::RunTest, PERF_HAL, TESTS_NIGHT_STRESS,
};
