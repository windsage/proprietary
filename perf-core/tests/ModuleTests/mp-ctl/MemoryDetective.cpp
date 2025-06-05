/******************************************************************************
  @file  MemoryDetective.cpp
  @brief Framework to detect memory leaks

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOCK_TEST_TAG "RF-MEMORY-DETECTIVE"
#include "MemoryDetective.h"
#include <thread>
#include <stdio.h>
#include "client.h"
#include "PerfLog.h"
#include "OptsData.h"
#include "PerfTest.h"
#define SEC_30 30

MemorySensitivityTuner_t defaultSensitivity = {
        MEM_LEAK_PERC_THRES,
        MEM_LEAK_CRITICAL_PERC_THRES,
        MEM_LEAK_COUNT_THRES,
        MIN_NUMBER_SAMPLES,
        MEM_HEAP_LEAK_PERC_THRES,
        MEM_FILE_DES_LEAK_PERC_THRES

    };

vector<string> FailureMsgs = {
    "ANON_HEAP_FAIL",
    "RSS_FILE_DESC_FAIL",
    "OTHER_MEM_FAIL"
    "HEALTHY"
};

int32_t MemoryDetective::getRemainingTime(struct timespec &t1, int &runtime) {

    int32_t time_delta = 0;
    struct timespec testEnd;
    if (runtime < 0) {
        return SEC_30;
    }
    clock_gettime(CLOCK_MONOTONIC, &testEnd);
    time_delta = (testEnd.tv_sec - t1.tv_sec + ((t1.tv_nsec - testEnd.tv_nsec)/(1000*1000*1000)));
    QLOGE(LOCK_TEST_TAG, "aman %d vs %d", time_delta, runtime);
    return (runtime - time_delta);
}


int32_t MemoryDetective::getFieldValue(char *line, const char *field) {
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

int32_t MemoryDetective::getProcessVmRss(FILE *fp, MemoryStat_t &memStat) {
    int32_t numFieldsRead = 0;
    int32_t val = -1;

    if (fp == NULL) {
        QLOGE(LOCK_TEST_TAG, "Failed to open process file, perms issue?\n");
        return val;
    }

    char line[NODE_MAX]={'\0', };
    memStat.mTotal = 0;
    while (fgets(line, sizeof(line), fp)) {
        val = getFieldValue(line, RSS_ANON_FIELD_NAME);
        if (val >= 0) {
            memStat.mRssAnon = val;
            memStat.mTotal += val;
        }

        val = getFieldValue(line, RSS_FILE_FIELD_NAME);
        if (val >= 0) {
            memStat.mRssFile = val;
            memStat.mTotal += val;
        }

        val = getFieldValue(line, VM_RSS_FIELD_NAME);
        if (val >= 0) {
            memStat.mVmRSS = val;
        }

        val = getFieldValue(line, VM_SWAP_FIELD_NAME);
        if (val >= 0) {
            memStat.mVmSwap = val;
            memStat.mTotal += val;
        }

        if (numFieldsRead >= 4) {
            break;
        }
    }

    return memStat.mTotal;
}

int32_t MemoryDetective::getPerfHalPID() {
    char buff[NODE_MAX] = {0};
    int32_t rc = FAILED;
    memset(buff, 0, sizeof(buff));
    FREAD_BUF_STR(PERF_PID_NODE, buff, NODE_MAX, rc);
    int32_t pid = strtol(buff, NULL, 0);
    return pid;
}

int32_t MemoryDetective::writeMemDumpLine(FILE *fp, MemoryStat_t &mem) {
    static int32_t index = 0;
    char outline[OUT_LINE_BUF] = {'\0',};
    string time;
    if (fp == NULL) {
        QLOGE(LOCK_TEST_TAG, "Failed to write mem dump");
        return FAILED;
    }
    index++;
    snprintf(outline, sizeof(outline), "%d|%d|%d|%d|%d|%d|-\n",
            index,
            mem.mRssAnon,
            mem.mRssFile,
            mem.mVmRSS,
            mem.mVmSwap,
            mem.mTotal);
    fwrite(outline, sizeof(char), strlen(outline), fp);
    QLOGE(LOCK_TEST_TAG, "%s", outline);
    return SUCCESS;
}

float MemoryDetective::percIncrease(int32_t end, int32_t start, int type) {
    switch (type) {
        case ANON_HEAP:{
            return ((float)(mMemoryStore[end].mRssAnon - mMemoryStore[start].mRssAnon)/(float)(mMemoryStore[start].mRssAnon )) * 100.0;
            break;
        }
        case RSS_FILE_DESC:{
            return ((float)(mMemoryStore[end].mRssFile - mMemoryStore[start].mRssFile)/(float)(mMemoryStore[start].mRssFile )) * 100.0;
            break;
        }
        case OTHER_MEM_TOTAL:{
            return ((float)(mMemoryStore[end].mTotal - mMemoryStore[start].mTotal)/(float)(mMemoryStore[start].mTotal )) * 100.0;
            break;
        }
        default:
            return 100;
    }

}

int32_t MemoryDetective::isHealthyState(bool trendInclusive) {
    static int32_t trendIncreaseCount = 0;
    static int32_t trendStableCount = 0;
    int32_t size = getMemoryStoreSize();
    int32_t rc = HEALTHY;
    if (size < 0) {
        return rc;
    }
    QLOGE(LOCK_TEST_TAG, "Size %d ", size);
    float total_increased = percIncrease(size - 1, 0, ANON_HEAP);
            QLOGE(LOCK_TEST_TAG, "Mem Leak ANON_HEAP detected %f Percent increased total", total_increased);
    if (total_increased >= mTuner.mRssAnonLeakTh) {
        QLOGE(LOCK_TEST_TAG, "Mem Leak ANON_HEAP detected %f Percent increased total", total_increased);
        return ANON_HEAP;
    }
    total_increased = percIncrease(size - 1, 0, RSS_FILE_DESC);
            QLOGE(LOCK_TEST_TAG, "Mem Leak RSS_FILE_DESC detected %f Percent increased total", total_increased);
    if (total_increased >= mTuner.mRssFileDescLeakTh) {
        QLOGE(LOCK_TEST_TAG, "Mem Leak RSS_FILE_DESC detected %f Percent increased total", total_increased);
        return RSS_FILE_DESC;
    }
    total_increased = percIncrease(size - 1, 0, OTHER_MEM_TOTAL);
            QLOGE(LOCK_TEST_TAG, "Mem Leak OTHER_MEM_TOTAL detected %f Percent increased total", total_increased);
    if (total_increased >= mTuner.mLeakPerc) {
        QLOGE(LOCK_TEST_TAG, "Mem Leak OTHER_MEM_TOTAL detected %f Percent increased total", total_increased);
        return OTHER_MEM_TOTAL;
    }
    if (size > mTuner.mLeakSample && trendInclusive) {
        size--;
        float perc_increase = percIncrease(size, (size - mTuner.mLeakSample) + 1, OTHER_MEM_TOTAL);
        QLOGE(LOCK_TEST_TAG, "Critical Memory Leak Detected of %f percent increase/%d-min", perc_increase, mTuner.mLeakSample);
        if (perc_increase > 0.0) {
            if (perc_increase >= mTuner.mCritialPercThr) {
                trendIncreaseCount = mTuner.mLeakCount;
                QLOGE(LOCK_TEST_TAG, "Critical Memory Leak Detected of %f percent increase/%d-min", perc_increase, mTuner.mLeakSample);
            } else if (perc_increase >= mTuner.mLeakPerc) {
                trendIncreaseCount++;
                trendStableCount = 0;
                QLOGE(LOCK_TEST_TAG, "Mem Leak detected %f Percent increase/%d-min Strike: [%d/%d]", perc_increase, mTuner.mLeakSample, trendIncreaseCount ,mTuner.mLeakCount);
            }
            if (trendIncreaseCount > 0 && perc_increase < mTuner.mLeakPerc) {
                if (perc_increase < mTuner.mLeakPerc && trendStableCount >= mTuner.mLeakCount) {
                    QLOGE(LOCK_TEST_TAG, "resetting Strikes as Memory Stable from last %d Iterations", trendStableCount);
                    trendIncreaseCount = 0;
                    trendStableCount = 0;
                } else {
                    trendStableCount++;
                }
            }
            if (trendIncreaseCount >= mTuner.mLeakCount) {
                float total_increased = percIncrease(size, 0);
                QLOGE(LOCK_TEST_TAG, "Mem Leak detected %f Percent increased total", total_increased);
                QLOGE(LOCK_TEST_TAG, "Test Failed");
                rc = FAILED;
                rc = OTHER_MEM_TOTAL;
            }
        }
    }
    return rc;
}

int32_t MemoryDetective::pushToMemoryStore(MemoryStat_t &memStat) {
    mMemoryStore.push_back(memStat);
    return SUCCESS;
}

int32_t MemoryDetective::getMemoryStoreSize() {
    return mMemoryStore.size();
}

int32_t MemoryDetective::collectMemoryDumps(TestData_t &data,  atomic<bool> &triggerTerminate,
            atomic<int32_t> &exitStatus, int32_t investigationTime, int32_t periodicitySecs,
            MemorySensitivityTuner_t) {
    MemoryStat_t memStat;
    struct timespec mTestStart;
    int32_t rc = FAILED;
    char filename[NODE_MAX] = {'\0',};
    char outline[OUT_LINE_BUF] = {'\0',};
    bool endCaptured = false;
    FILE *fp = NULL;
    char perfStatusFilePath[NODE_MAX] = {'\0',};

    snprintf(filename, sizeof(filename), "%s", data.mTestDetailedFile.c_str());
    char *newNUll = strstr(filename, ".csv");
    if (newNUll)
        *newNUll = '\0';
    snprintf(filename + strlen(filename), sizeof(filename), "%s","_mem.csv");
    QLOGE(LOCK_TEST_TAG, "Collecting Dumps to %s File", filename);
    FILE *outfile = fopen(filename, "w+");
    if (outfile == NULL) {
        return rc;
    }
    snprintf(outline, sizeof(outline), "Index|RssAnon|RssFile|VmRss|VmSwap|Total|RESULT\n");
    fwrite(outline, sizeof(char), strlen(outline), outfile);
    if (outfile) {
        fclose(outfile);
        outfile = NULL;
    }
    int32_t oldPid = getPerfHalPID();
    int32_t perfPid = oldPid;
    snprintf(perfStatusFilePath, sizeof(perfStatusFilePath), "/proc/%d/status", perfPid);
    rc = SUCCESS;

    clock_gettime(CLOCK_MONOTONIC, &mTestStart);
    if (periodicitySecs == investigationTime) {
            QLOGE(LOCK_TEST_TAG, "periodicitySecs %d investigationTime: %d Reducing periodicitySecs by 5th new %d",
                                 investigationTime, periodicitySecs, periodicitySecs/5);
            periodicitySecs /=5;
    }
    do {
        memset(&memStat, 0, sizeof(MemoryStat_t));
        if (PerfTest::checkNodeExist(perfStatusFilePath) == SUCCESS) {
            errno = 0;
            fp = fopen(perfStatusFilePath, "r");
            if (fp == NULL) {
                QLOGE(LOCK_TEST_TAG, "Failed to Read '%s' error: '%s' \nClosing Test", perfStatusFilePath, strerror(errno));
                break;
            }
            int32_t vmrss = getProcessVmRss(fp, memStat);
            if (fp) {
                fclose(fp);
                fp = NULL;
            }
            if (vmrss >= 0) {
                getMemoryStoreSize();
                outfile = fopen(filename, "a");
                if (outfile == NULL) {
                    QLOGE(LOCK_TEST_TAG, "Failed to write to %s", filename);
                    rc = FAILED;
                    break;
                }
                writeMemDumpLine(outfile, memStat);
                if (outfile) {
                    fclose(outfile);
                    outfile = NULL;
                }
                pushToMemoryStore(memStat);
                QLOGE(LOCK_TEST_TAG, "isHealthyState");
                //rc = isHealthyState();
                if (rc == FAILED) {
                    QLOGE(LOCK_TEST_TAG, "Memory leak detected");
                }
            } else {
                QLOGE(LOCK_TEST_TAG, "Not Enough data %d", vmrss);
            }
        } else {
            QLOGE(LOCK_TEST_TAG, "Node Does not exist '%s'", perfStatusFilePath);
            perfPid = getPerfHalPID();
            snprintf(perfStatusFilePath, sizeof(perfStatusFilePath), "/proc/%d/status", perfPid);
            if (oldPid != perfPid) {
                QLOGE(LOCK_TEST_TAG, "Perf PID Changed");
                rc = FAILED;
            }
            break;
        }
        if (rc == FAILED) {
            QLOGE(LOCK_TEST_TAG, "Crash/Leak Detected");
        } else if (getRemainingTime(mTestStart, investigationTime) < periodicitySecs)  {
            if (endCaptured == false) {
                endCaptured = true;
                // collect last data entry after each thread finish
                PerfTest::gotoSleep(1000 * periodicitySecs * 1);
                continue;
            }
        } else {
            // collect data every 1 mins
            PerfTest::gotoSleep(1000 * periodicitySecs * 1);
        }
    } while (rc == SUCCESS && getRemainingTime(mTestStart, investigationTime) >= periodicitySecs &&
            (!triggerTerminate.load()));
    int Healthrc = isHealthyState();
    if (Healthrc != HEALTHY) {
        if (outfile == NULL)
            outfile = fopen(filename, "a");
        snprintf(outline, sizeof(outline), "Failed Type : %d - Detail : %s-\n",
            Healthrc, FailureMsgs[Healthrc].c_str());
        QLOGE(LOCK_TEST_TAG, "%s", outline);
        if (outfile) {
            fwrite(outline, sizeof(char), strlen(outline), outfile);

        }
        rc = FAILED;
    } else {
        rc = SUCCESS;
    }
    if (outfile) {
        fclose(outfile);
    }
    if (fp) {
        fclose(fp);
        fp = NULL;
    }
    exitStatus = rc;
    triggerTerminate = true;
    return rc;
}


int32_t MemoryDetective::assignDetective(TestData_t &data, atomic<bool> &triggerTerminate,
            atomic<int32_t> &exitStatus, int32_t investigationTime, int32_t periodicitySecs,
            MemorySensitivityTuner_t  ss) {
    QLOGE(LOCK_TEST_TAG, "Creating Memory detective Thread %d", investigationTime);
    mTuner = ss;
    thread detectiveThread(&MemoryDetective::collectMemoryDumps, this, std::ref(data),
            std::ref(triggerTerminate), std::ref(exitStatus), investigationTime, periodicitySecs,
            defaultSensitivity);
    detectiveThread.detach();
    return SUCCESS;
}


