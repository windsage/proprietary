/******************************************************************************
  @file  MemoryDetective.h
  @brief Framework to detect memory leaks

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#ifndef __MEMORYDETECTIVE_H__
#define __MEMORYDETECTIVE_H__
#include <atomic>
#include <vector>

#include "RegressionFramework.h"


#define RSS_ANON_FIELD_NAME "RssAnon:"
#define RSS_FILE_FIELD_NAME "RssFile:"
#define VM_RSS_FIELD_NAME "VmRSS:"
#define VM_SWAP_FIELD_NAME "VmSwap:"
#define MEM_LEAK_PERC_THRES 5.0
#define MEM_LEAK_CRITICAL_PERC_THRES 15.0
#define MEM_LEAK_COUNT_THRES 5
#define MIN_NUMBER_SAMPLES 5
#define MEM_HEAP_LEAK_PERC_THRES 10
#define MEM_FILE_DES_LEAK_PERC_THRES 10

typedef struct memoryStat {
    int32_t mRssAnon;
    int32_t mRssFile;
    int32_t mVmRSS;
    int32_t mVmSwap;
    int32_t mTotal;
} MemoryStat_t;

typedef struct MemorySensitivityTuner {
    float mLeakPerc;
    float mCritialPercThr;
    int32_t mLeakCount;
    int32_t mLeakSample;
    int32_t mRssAnonLeakTh; // Heap memory allocations
    int32_t mRssFileDescLeakTh; // File Descriptors leak

} MemorySensitivityTuner_t;

enum MemHealthFailType {
    ANON_HEAP,
    RSS_FILE_DESC,
    OTHER_MEM_TOTAL,
    HEALTHY
};

extern vector<string> FailureMsgs;

extern MemorySensitivityTuner_t defaultSensitivity;

class MemoryDetective {
    private:
        vector<MemoryStat_t> mMemoryStore;
        MemorySensitivityTuner_t mTuner;

        int32_t getRemainingTime(struct timespec &t1, int &runtime);
    public:
        int32_t getFieldValue(char *line, const char *field);
        int32_t getProcessVmRss(FILE *fp, MemoryStat_t &memStat);
        int32_t getPerfHalPID();
        int32_t collectMemoryDumps(TestData_t &data, atomic<bool> &triggerTerminate,
                        atomic<int32_t> &exitStatus, int32_t investigationTime,
                        int32_t periodicitySecs, MemorySensitivityTuner_t  ss);
        int32_t writeMemDumpLine(FILE *fp, MemoryStat_t &mem);
        int32_t pushToMemoryStore(MemoryStat_t &memStat);
        int32_t getMemoryStoreSize();
        int32_t isHealthyState(bool trendInclusive = false);
        float percIncrease(int32_t end, int32_t start, int type = OTHER_MEM_TOTAL);
        int32_t assignDetective(TestData_t &data, atomic<bool> &triggerTerminate,
                        atomic<int32_t> &exitStatus, int32_t investigationTime = -1,
                        int32_t periodicitySecs = 60,
                        MemorySensitivityTuner_t  ss = defaultSensitivity);


};
#endif //__MEMORYDETECTIVE_H__
