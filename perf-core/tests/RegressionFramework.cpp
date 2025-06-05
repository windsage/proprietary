/******************************************************************************
  @file  RegressionFramework.cpp
  @brief RegressionFramework for Perf Test

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOG_TAG "ANDR-PERF-RF"
#include "RegressionFramework.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <stdlib.h>
#include "PerfLog.h"
#include <iomanip>
#include <sstream>

using namespace std;

map <int32_t, map<int32_t, UnitTest>> RegressionFramework::mTestsMap;
map <string, int32_t> RegressionFramework::mTestsNameMap;
int16_t RegressionFramework::mNumRegistered = 0;
static pthread_mutex_t perf_veri_mutex = PTHREAD_MUTEX_INITIALIZER;


UnitTest::UnitTest(int32_t moduleid, int32_t testid, TestFn func,const char *name, int32_t testType) {
    char FileName[OUT_LINE_BUF] = {0};
    mModuleId = moduleid;
    mTestId = testid;
    mUID = (moduleid << 16) | testid;
    mTestFn = func;
    mTestType = testType;
    if (name != NULL) {
        mTestName = string(name);
    }
    snprintf(FileName, OUT_LINE_BUF, "%sRF_0x%X_%s",RF_OUT_FILE_PATH,
            mUID,mTestName.c_str());
    mTestResultFile = string(FileName) + string(".csv");
    mTestDetailedFile = string(FileName) + string("_Detailed");
}

void RegressionFramework::Init(int32_t moduleid, const char *name, TestFn fn, int32_t testType) {
    pthread_mutex_lock(&perf_veri_mutex);

    if (name == NULL) {
        QLOGE(LOG_TAG, "Invalid Test Name Passed");
        pthread_mutex_unlock(&perf_veri_mutex);
        return;
    }

    if (testType <= TEST_TYPE_START || testType >= TEST_TYPE_END) {
        QLOGE(LOG_TAG, "Invalid Test Type Passed %" PRId32 " in %s Test", testType, name);
        pthread_mutex_unlock(&perf_veri_mutex);
        return;
    }

    if (moduleid <= MODULES_REGISTERED_START || moduleid > MAX_MODULES_REGISTERED) {
        QLOGE(LOG_TAG, "Invalid Test Module Passed %" PRId32 " in %s Test", moduleid, name);
        pthread_mutex_unlock(&perf_veri_mutex);
        return;
    }

    if (fn != NULL && (mNumRegistered < MAX_UNIT_TESTS_MODULES)) {
        auto tmp = mTestsMap.find(moduleid);
        if (tmp == mTestsMap.end()) {
            mTestsMap[moduleid][1] = UnitTest(moduleid, 1, fn, name, testType);
            mTestsNameMap[name] = mTestsMap[moduleid][1].mUID;
        } else {
            int32_t index = (tmp->second).size() + 1;
            mTestsMap[moduleid][index] = UnitTest(moduleid, index, fn, name, testType);
            mTestsNameMap[name] = mTestsMap[moduleid][index].mUID;
        }
        mNumRegistered++;
    } else {
        QLOGE(LOG_TAG, "name null or reached max limit, can not registered any more modules\n");
    }
    pthread_mutex_unlock(&perf_veri_mutex);
}

RegressionFramework::RegressionFramework(const char *name, TestFn fn, int32_t moduleid, int32_t testType) {
    Init(moduleid, name, fn, testType);
}

char* RegressionFramework::getPassFail(bool rc, char* ptr) {
    if (rc == true) {
        snprintf(ptr,10,"PASSED");
    } else {
        snprintf(ptr,10,"FAILED");
    }
    return ptr;
}
int8_t RegressionFramework::executeTest(map<int32_t, UnitTest> &tmp, TestData_t &data) {
    int8_t ret = -1;
    int32_t moduleid = data.mModuleId;
    int32_t testid = data.mTestId;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[OUT_LINE_BUF] = {0};
    FILE *defval = NULL;

    auto tmp2 = tmp.find(testid);
    if (tmp2 == tmp.end()) {
        QLOGE(LOG_TAG, "Invalid Test: [%" PRId32 "] passed for Module: [%" PRId32 "]",testid, moduleid);
        return 0;
    }
    if (NULL == (tmp2->second).mTestFn) {
        QLOGE(LOG_TAG, "Invalid Module[%s] method",(tmp2->second).mTestName.c_str());
        return 0;
    }
    defval = fopen(((tmp2->second).mTestResultFile).c_str(), "w+");
    if (defval == NULL) {
        QLOGE(LOG_TAG, "Cannot open/create default values file");
        return 0;
    }
    data.mTestName = (tmp2->second).mTestName;
    printf("==================================================\n");
    QLOGE(LOG_TAG, "Running module %s\n", (tmp2->second).mTestName.c_str());
    printf("Running module %s\n", (tmp2->second).mTestName.c_str());
    snprintf(outfile, OUT_LINE_BUF, "%s|%s|%s|%s|%s|%s\n",ITERATION, UID, MODULE, TESTID, TESTNAME, RESULT);
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    printf("%s", outfile);
    for (int16_t j = 1;j <= data.mIterations; j++) {
        data.mTestDetailedFile = (tmp2->second).mTestDetailedFile + "-" + to_string(j) + string(".csv");
        ret = (*((tmp2->second).mTestFn))(data);
            snprintf(outfile, OUT_LINE_BUF, "%" PRId16 "|0x%" PRIX32 "|%s(%" PRId32 ")|%" PRId32 "|%s|%s\n",
                    j,
                    (tmp2->second).mUID,
                    testModuleNames[(tmp2->second).mModuleId],
                    (tmp2->second).mModuleId,
                    (tmp2->second).mTestId,
                    (tmp2->second).mTestName.c_str(),
                    getPassFail(ret == SUCCESS, rc_s));
        QLOGE(LOG_TAG, "%s", outfile);
        printf("%s", outfile);
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }
    fclose(defval);
    return 0;
}

uint8_t RegressionFramework::Run(TestData_t &data) {
    int32_t testid = data.mTestId;

    auto tmp = mTestsMap.find(data.mModuleId);

    if (tmp == mTestsMap.end()) {
        QLOGE(LOG_TAG, "Invalid Module Id received: %" PRId32 "\n", data.mModuleId);
        return 0;
    }
    if (testid == -1) {
        QLOGE(LOG_TAG, "Running All Test in Module: %" PRId32 "\n",data.mModuleId);
        for (auto& tmp2: tmp->second) {
            data.mTestId = (tmp2.second).mTestId;
            executeTest(tmp->second, data);
        }
    } else {
        executeTest(tmp->second, data);
    }
    return 0;
}

static void show_usage_2(const char* name) {
    if (name == NULL) {
        return;
    }
    RegressionFramework rf;
    printf("Usage: \n$%s -r <ModuleId> <TestId>\n"
            "$%s -u <uid>\n"
            "Uid = 0x<16bit moduleid...16bit testid>\n"
            "Options:\n"
            "-h,--help\t\tShow this help message\n"
            "-m, --modules\t\tShows list of modules and test registered.\n"
            "-r, --run\t\tRun a particular module test case using <moduleid> <testid>\n"
            "-u, --uid\t\tRun a partucular module testcase 0x<16bit moduleid...16bit testid>\n"
            "-i, --iter\t\tNumber of iterations to run the test.\n"
            "-n, --name\t\tRun a test with it's name.\n",
            name, name);
    printf("_______________________________________\n");
    rf.getModulesRegistered();
    return;
}

void RegressionFramework::getModulesRegistered() {
    stringstream ss;
    stringstream temp;

    ss << setfill(' ');
    temp.clear();
    temp.str("");
    temp <<" " << UID;
    ss << left << setfill(' ') << setw(10) << temp.str();

    temp.clear();
    temp.str("");
    temp << "| " << MODULE;
    ss << left << setfill(' ') << setw(30) << temp.str();

    temp.clear();
    temp.str("");
    temp << "| " << TESTID;
    ss << left << setfill(' ') << setw(10) << temp.str();

    temp.clear();
    temp.str("");
    temp << "| " << TESTTYPE;
    ss << left << setfill(' ') << setw(30) << temp.str();

    temp.clear();
    temp.str("");
    temp << "| " << TESTNAME;
    ss << left << setfill(' ') << setw(40) << temp.str();
    ss << "\n";

    for (auto& it: mTestsMap) {
        for (auto& tmp2: it.second) {
            temp.clear();
            temp.str("");
            temp << " " << hex << "0x" << (tmp2.second).mUID;
            ss << left << setfill(' ') << setw(10) << temp.str();

            temp.clear();
            temp.str("");
            temp << "| " << testModuleNames[(tmp2.second).mModuleId] << " [ " << dec <<(tmp2.second).mModuleId << " ]";
            ss << left << setfill(' ') << setw(30) << temp.str();

            temp.clear();
            temp.str("");
            temp <<"| " << dec << (tmp2.second).mTestId;
            ss << left << setfill(' ') << setw(10) << temp.str();

            temp.clear();
            temp.str("");
            temp << "| " << testTypeNames[(tmp2.second).mTestType] << " [ " << dec <<(tmp2.second).mTestType << " ]";
            ss << left << setfill(' ') << setw(30) << temp.str();

            temp.clear();
            temp.str("");
            temp << "| " << (tmp2.second).mTestName.c_str();
            ss << left << setfill(' ') << setw(40) << temp.str();
            ss << "\n";
        }
    }
    printf("%s", ss.str().c_str());
}

uint32_t RegressionFramework::getIdFromUID(uint32_t uid, uint8_t flag) {
    if (TEST_ID == flag) {
        return TEST_ID_MASK & uid;
    } else {
        return uid >> 16;
    }
}

int32_t RegressionFramework::getUIDFromName(const char *testName) {
    if (testName == NULL) {
        QLOGE(LOG_TAG, "Invalid test Name Passed");
        return FAILED;
    }
    auto tmp = mTestsNameMap.find(testName);
    if (tmp == mTestsNameMap.end()) {
        QLOGE(LOG_TAG, "Invalid test Name Passed %s", testName);
        return FAILED;
    }
    return tmp->second;
}

int main(int argc, char *argv[]) {
    int32_t testid = -1;
    int32_t moduleid = -1;
    int32_t iterations = 1;
    uint32_t duration = 0;
    int16_t workers = 1; //num of threads
    int i = 1;
    char *testName = NULL;


    if (argc <= 1) {
        show_usage_2(argv[0]);
        return 1;
    }
    RegressionFramework rf;
    while (i < argc) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            QLOGE(LOG_TAG, "%d\n",__LINE__);
            show_usage_2(argv[0]);
            return 0;
        } else if ((arg == "-m") || (arg == "--modules")) {
            rf.getModulesRegistered();
            return 0;
        } else if ((arg == "-r") || (arg == "--run")) {
            char* p;
            i++;
            if (i >= argc) {
                QLOGE(LOG_TAG, "invalid input\n");
                show_usage_2(argv[0]);
                return 0;
            }
            moduleid = strtol(argv[i], &p, 0);
            if (*p) {
                QLOGE(LOG_TAG, "Invalid Module id Passed:\n");
                show_usage_2(argv[0]);
                return 0;
            } else {
                i++;
                if (i < argc) {
                    testid = strtol(argv[i], NULL, 0);
                }
            }
        } else if ((arg == "-n") || (arg == "--name")) {
            i++;
            if (i >= argc) {
                QLOGE(LOG_TAG, "invalid input\n");
                show_usage_2(argv[0]);
                return 0;
            }
            testName = argv[i];
            int32_t val = rf.getUIDFromName(testName);
            if (val != FAILED) {
                moduleid = rf.getIdFromUID(val, MODULE_ID);
                testid = rf.getIdFromUID(val, TEST_ID);
            }
        } else if ((arg == "-u") || (arg == "--uid")) {
            i++;
            if (i >= argc) {
                QLOGE(LOG_TAG, "invalid input\n");
                show_usage_2(argv[0]);
                return 0;
            }
            int32_t val = strtol(argv[i], NULL, 0);
            moduleid = rf.getIdFromUID(val, MODULE_ID);
            testid = rf.getIdFromUID(val, TEST_ID);
        } else if ((arg == "-t") || (arg == "--time")) {
            i++;
            duration = strtol(argv[i], NULL, 0);
        } else if ((arg == "-i") || (arg == "--iter")) {
            i++;
            iterations = strtol(argv[i], NULL, 0);
        } else if ((arg == "-w") || (arg == "--workers")) {
            i++;
            workers = strtol(argv[i], NULL, 0);
        } else {
            QLOGE(LOG_TAG, "%d\n",__LINE__);
            show_usage_2(argv[0]);
            return 0;
        }
        i++;
    }
    if (moduleid < 0 || (testid < 0 && moduleid < 0) || iterations < 0) {
        QLOGE(LOG_TAG, "Invalid Module id/TestId passed Passed\n");
        show_usage_2(argv[0]);
        return 0;
    }
    TestData_t data;
    data.mModuleId = moduleid;
    data.mTestId = testid;
    data.mIterations = iterations;
    data.mDuration = duration;
    if (workers > MAX_THREADS) {
        workers = MAX_THREADS;
    }
    data.mWorkers = workers;
    rf.Run(data);
    return 0;
}
