/******************************************************************************
  @file  RegressionFramework.h
  @brief RegressionFramework

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#ifndef __RF_H__
#define __RF_H__
#include <map>
#include <string>
#include <inttypes.h>
#include "MpctlUtils.h"

#define MAX_UNIT_TESTS_MODULES 128
#define MAX_TEST_NAME_LEN 128
#define MAX_THREADS 256

#define SUCCESS 0
#define FAILED -1

#define TEST_TYPES 4
#define TEST_ID_MASK 0X0000FFFF
#define OUT_LINE_BUF 1000

//Headers Names
#define ITERATION "ITERATION"
#define MODULE "MODULE"
#define TESTID "TESTID"
#define UID "UID"
#define TESTNAME "TESTNAME"
#define RESULT "RESULT"
#define TESTTYPE "TESTTYPE"

// Test names
#define QGPE_TEST "QGPE_TEST_SIMPLE"
#define LL_TEST   "LL_TEST_SIMPLE"
#define Prefapps_Test "PREFAPPS_TEST"
#define Prekill_Test "PREKILL_TEST"
#define Procomp_Test "PROCOMP_TEST"

#define DELIM "|"

enum {
    MODULE_ID,
    TEST_ID,
};

#define RF_OUT_FILE_PATH "/data/vendor/perfd/"

enum {
    TEST_TYPE_START = -1,
    TESTS_SIMPLE,
    TESTS_MULTITHREADED,
    TESTS_STRESS,
    TESTS_NIGHT_STRESS,
    TEST_TYPE_END,
};

static const char *testTypeNames[TEST_TYPE_END] = {
    "TESTS_SIMPLE",
    "TESTS_MULTITHREADED",
    "TESTS_STRESS",
    "TESTS_NIGHT_STRESS",
};

typedef enum {
    MODULES_REGISTERED_START = -1,
    PERF_HAL = 0,
    MPCTL,
    MPCTL_CLIENT,
    PERF_UTIL,
    ACTIVITY_TRIGGER,
    DEFAULT_TYPE,
    QGPE,
    MEM,
    LL,
    DISPLAY_MOD,
    MAX_MODULES_REGISTERED,
} PerfModuleType;

static const char *testModuleNames[MAX_MODULES_REGISTERED] = {
    "PERF_HAL",
    "MPCTL",
    "MPCTL_CLIENT",
    "PERF_UTIL",
    "ACTIVITY_TRIGGER",
    "DEFAULT_TYPE",
    "QGPE",
    "MEM",
    "LL",
    "DISPLAY_MOD"
};

using namespace std;
class TestData_t {
public:
   int32_t mModuleId;
   int32_t mTestId;
   int16_t mIterations;
   uint32_t mDuration;
   int16_t mWorkers;
   string mTestName;
   string mTestDetailedFile;
   TestData_t(){
       mModuleId=0;
       mTestId=0;
       mIterations=0;
       mDuration=0;
       mWorkers=0;
       mTestName="";
       mTestDetailedFile="";
   }
   ~TestData_t(){
   }
} ;
typedef int32_t (*TestFn)(TestData_t &);


class UnitTest {
    public:
    int32_t mModuleId;
    int32_t mTestId;
    uint32_t mUID;
    int32_t mTestType;
    TestFn mTestFn;
    string mTestName;
    string mTestResultFile;
    string mTestDetailedFile;
    UnitTest() {
    }
    UnitTest(int32_t moduleid, int32_t testid, TestFn func,const char *name, int32_t testType);
};

class RegressionFramework {
    static map <int32_t, map<int32_t, UnitTest>> mTestsMap;
    static map <string, int32_t> mTestsNameMap;
    static int16_t mNumRegistered;
    void Init(int32_t moduleid, const char *name, TestFn fn, int32_t testType);
    int8_t executeTest(map<int32_t, UnitTest> &tmp, TestData_t &data);

public:
    RegressionFramework() {
    }
    RegressionFramework(const char *name, TestFn fn, int32_t moduleid=DEFAULT_TYPE, int32_t testType=TESTS_SIMPLE);
    void getModulesRegistered();
    uint32_t getIdFromUID(uint32_t uid, uint8_t flag=MODULE_ID);
    int32_t getUIDFromName(const char *testName);
    uint8_t Run(TestData_t&);
    char* getPassFail(bool rc, char *ptr);
};

#endif
