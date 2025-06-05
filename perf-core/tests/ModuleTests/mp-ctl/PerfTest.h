/******************************************************************************
  @file  PerfTest.h
  @brief test module to test all perflocks

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#ifndef __PERFTEST__
#define __PERFTEST__
#include <vector>
#include "RegressionFramework.h"
#include "TargetConfig.h"
#include "Target.h"
#include "XmlParser.h"

enum {
    RF_NODE_TEST = 0,
    RF_LOCK_TEST,
    RF_HINT_TEST,
};

#define RF_PERF_NODE_TEST "PERF_NODE_TEST"
#define RF_PERF_LOCK_TEST "PERF_LOCK_VALUE_TEST"
#define RF_PERF_HINT_TEST "PERF_BOOST_HINTS_TEST"
#define AVC_DENIAL_TEST "PERF_AVC_SYSFS_NODES_TEST"
#define DISPLAY_TEST_EARLYWAKEUP "Display_Tests_EarlyWakeup"
#define DISPLAY_TESTS_LARGECOMP "DISPLAY_TESTS_LARGECOMP"
#define DISPLAY_TESTS_FPS_UPDATE "Display_Tests_FPS_Update"
#define DISPLAY_ON_OFF_TEST "DISPLAY_ON_OFF_TEST"
#define DISPLAY_ON_OFF_ALWAYS_APPLY "DISPLAY_ON_OFF_ALWAYS_APPLY"
#define DISPLAY_OFF_NODE_CHECK "DISPLAY_OFF_NODE_CHECK"
#define PERF_GET_PROP_TEST "PERF_GET_PROP_TEST"
#define RF_PERF_API_STRESS_TEST "PERF_API_STRESS_TEST"
#define RF_PERF_MEMORY_TEST "PERF_MEMORY_TEST"
#define DISPLAY_ON_OFF_REQUEST_PENDED_TEST "DISPLAY_ON_OFF_REQUEST_PENDED_TEST"
#define DISPLAY_OFF_REQUEST_UNPENDED_TEST "DISPLAY_OFF_REQUEST_UNPENDED_TEST"
#define DISPLAY_OFF_REQUEST_EXPIRED_RELEASED_TEST \
                        "DISPLAY_OFF_REQUEST_EXPIRED_RELEASED_TEST"
#define DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPIRED_REL_TEST \
                        "DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPIRED_REL_TEST"
#define DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_TEST \
                        "DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_TEST"
#define DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_AFTER_OFF_TEST \
                        "DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_AFTER_OFF_TEST"
#define DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_TIMEDOUT_AFTER_OFF_TEST \
            "DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_TIMEDOUT_AFTER_OFF_TEST"
#define DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_AFTER_OFF_TEST_CONCERUNCY \
            "DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_AFTER_OFF_TEST_CONCERUNCY"
#define SUCCESS 0
#define FAILED -1
#define TIMEOUT_DURATION 500
#define TIME_MSEC_IN_SEC    1000
#define TIME_NSEC_IN_MSEC   1000000
#define TIMEOUT_DURATION_SLEEP (TIMEOUT_DURATION/2)
#define TIMEOUT_10_SEC 10000
#define WAIT_FOR_UPDATE 100

//0 based indexing
//columns in active request list dump.
enum {
    REQ_DUMP_PID_TID_COLUMN = 0,
    REQ_DUMP_PKG_NAME_COLUMN = 1,
    REQ_DUMP_DURATION_COLUMN = 2,
    REQ_DUMP_TIMESTAMP_COLUMN = 3,
    REQ_DUMP_HANDLE_COLUMN = 4,
    REQ_DUMP_HINT_TYPE_COLUMN = 5,
    REQ_DUMP_RESOURCES_COLUMN = 6
};

using namespace std;
class testResourceConfigInfo {
    public:
        explicit testResourceConfigInfo(uint16_t qindex, int16_t major, int16_t minor, int32_t wval, int32_t rval, bool special_node, int8_t cluster, char *exceptn);
        int16_t mMajor;
        int16_t mMinor;
        int8_t mCluster;
        int16_t mQindex;
        char mNodePath[NODE_MAX];
        char mException[NODE_MAX];
        bool mExceptionFlag;
        int32_t mWriteValue;
        int32_t mReadVal;
        bool mSpecialNode;
        bool mSupported;
        void setSupported(bool flag) {
            mSupported = flag;
        }
        int8_t setNodePath(const char*);
};

class PerfTest {
    private:
        PerfTest();
        PerfTest(PerfTest const& oh);
        PerfTest& operator=(PerfTest const& oh);
    public:
        bool mTestEnabled;
        uint8_t mRunningTest; //This Flag is used to enable exception's in Hint Test for some nodes
        TargetConfig &mTc = TargetConfig::getTargetConfig();
        Target &mT = Target::getCurTarget();
        static map<uint16_t, testResourceConfigInfo> mTestResourceConfigInfo;
        map<int, bool> mSkipHints;
        int32_t setResourceInfo(uint16_t qindex, testResourceConfigInfo &resource);
        int32_t getOpcode(testResourceConfigInfo &resource);
        int16_t getMajorMinor(int16_t &major, int16_t &minor);
        int32_t getNodeVal(int32_t, testResourceConfigInfo &resource, const char *, char *value);
        uint32_t checkNodeVal(int32_t opcode, testResourceConfigInfo &resource, const char* observed, uint32_t &expected, char node_path[NODE_MAX]);
        int8_t getNodePath(int32_t opcode, testResourceConfigInfo &resource, char *node_path);
        int8_t readPerfTestXml();
        static void UpdatePerfResource(int16_t major, int16_t minor, int32_t wval, int32_t rval, bool special_node, int8_t cluster, char *exceptn);
        static long int ConvertNodeValueToInt(xmlNodePtr node, const char *tag, long int defaultvalue);
        static void PerfCommonResourcesCB(xmlNodePtr node, void *);
        static void PerfTargetResourcesCB(xmlNodePtr node, void *);
        int8_t processString(char *);
        int8_t getCoreClusterfromOpcode(int32_t opcode, int8_t &cluster, int8_t &core);
        int8_t getAvailableFreqNode(const char* source, char* node_path);
        uint32_t getNextAvailableFreq(const char* node, uint32_t &val);
        uint32_t getNextCPUFreq(int32_t opcode, uint32_t &val);
        uint32_t getNextGPUFreq(uint32_t &val);
        int32_t processHintActions(mpctl_msg_t *, uint32_t fps, int32_t &fpsEqHandle);
        int8_t getMaxNode(const char* source, char* node_path);
        const char *getActiveReqDump();
        int8_t checkDisplayOffHintCount();
        int8_t getDisplayOffHintCount();
        int8_t areSameStrings(const char*, const char*);
        char *getPassFail(bool, char*);
        bool checkReqExistInActiveReq(int32_t , int32_t );
        int32_t clearAllReqExistInActiveReq();

        ///////Test Cases
        int32_t PerfNodeTest(const char* filename);
        int32_t PerfLockTest(const char* filename);
        int32_t PerfHintTest(const char* filename);
        int32_t DisplayTestsEW(const char* filename);
        int32_t DisplayTestsLC(const char* filename);
        int32_t DisplayTestsFPSU(const char* filename);
        int32_t AVCDenialTest(const char* filename);
        int32_t DisplayOnOFF(const char* filename);
        int32_t DisplayOnOFFAlwaysApply(const char* filename);
        int32_t DisplayOFF(const char* filename);
        int32_t PerfPropTest(const char *filename);

        int32_t DisplayOnOFFRequestPended(const char *filename);
        int32_t DisplayOFFCheckUnpended(const char *filename);
        int32_t DisplayOFFReqExpired(const char *filename);
        int32_t DisplayOnOFFAlwaysApplyReqExpired(const char *filename);
        int32_t DisplayOnOFFAlwaysApplyRelReq(const char *filename);
        int32_t DisplayOnOFFAlwaysApplyRelReqAfterDisaplyOff(const char *filename);
        int32_t SendRegularSLBRequest(int32_t &);
        int32_t DisplayOnOFFAlwaysApplyRelReqTimedOutAfterDisaplyOff(const char *filename);
        int32_t DisplayOnOFFAlwaysApplyRelReqAfterDisaplyOffConceruncy(const char *filename);
        bool isTestEnabled() {
            return mTestEnabled;
        }

    public:
        static PerfTest &getPerfTest();
        static int32_t RunTest(TestData_t &data);
        static int8_t gotoSleep(uint32_t);
        static int8_t checkNodeExist(const char* node_path);
};

class PerfStressTest {
    private:
        PerfStressTest();
        PerfStressTest(PerfStressTest const& oh);
        PerfStressTest& operator=(PerfStressTest const& oh);
        int32_t getRemainingTime();
        int32_t mRuntime;
        struct timespec mTestStart;
        Target &mT = Target::getCurTarget();
        PerfTest &mPT = PerfTest::getPerfTest();
        bool mKeepRunning;
    public:
        int32_t getFieldValue(char *line, const char *field);
        int32_t getPerfHalPID();
        int32_t collectMemoryDumps(TestData_t &data);
        int32_t startCollectingDumps(TestData_t &data);
        int32_t getSystemTime(string &time);
        void setKeepRunning(bool value);
        bool getKeepRunning();
        int32_t getMemoryStoreSize();
        int32_t isHealthyState();
        float percIncrease(int32_t end, int32_t start);

        //Test Functions
        static int32_t RunTest(TestData_t &data);
        int32_t RunAPIStressTest(TestData_t &data);
        int32_t RunMeoryTest(TestData_t &data);
        void PerfHintRenewTest();
        void PerfEventTest();
        void PerfEventOffloadTest();
        void PerfHintAcqRelTest();
        void PerfHintAcqRelOffloadTest();
        void PerfLockAcqRelTest();

        static PerfStressTest &getPerfStressTest();

};
#endif //__PERFTEST__
