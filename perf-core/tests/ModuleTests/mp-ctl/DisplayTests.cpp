/******************************************************************************
  @file  DisplayTests.cpp
  @brief test module to test all perflocks with display usecases

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOCK_TEST_TAG "RF-DISPLAY-PERF-TEST"
#include "PerfTest.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "client.h"
#include "HintExtHandler.h"
#include "TargetConfig.h"
#include "ResourceInfo.h"
#include "Request.h"
#include "OptsData.h" // for FREAD_STR
#include <thread>
#include <unistd.h>
#include <math.h>
#include <algorithm>
#include <inttypes.h>
#include <cstdlib>

#define MAX_CONCURRENCY_REQ 5

int32_t PerfTest::DisplayOnOFF(const char * filename) {
    const uint8_t iterations = 100;
    int32_t hint_id = 0;
    uint32_t sleepTime = 100;
    int32_t res = FAILED;
    int32_t rc = FAILED;
    char outfile[OUT_LINE_BUF] = {0};
    char comments[OUT_LINE_BUF] = {0};
    int32_t handle = FAILED;
    char rc_s[NODE_MAX] = {0};
    srand(time(0));
    FILE *defval = NULL;
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file %s\n", filename);
        return res;
    }
    res = SUCCESS;
    snprintf(outfile, OUT_LINE_BUF, "Handle|HintId|Comments|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    for (uint8_t i = 0 ; i < iterations;i++) {
        memset(comments, 0, sizeof(comments));
        snprintf(comments, OUT_LINE_BUF, "None");
        rc  = SUCCESS;
        uint8_t random_number = rand()% iterations;
        if (random_number % 2 == 0) {
            hint_id = VENDOR_HINT_DISPLAY_ON;
            handle = perf_hint(hint_id, filename, 10000000, -1);
        } else {
            hint_id = VENDOR_HINT_DISPLAY_OFF;
            handle = perf_hint(hint_id, filename, 10000000, -1);
            gotoSleep(sleepTime);
            rc = checkDisplayOffHintCount();
            if (handle <= 0) {
                snprintf(comments, OUT_LINE_BUF, "Failed to Send Display off Hint");
            } else if (rc == FAILED) {
                res = FAILED;
                snprintf(comments, OUT_LINE_BUF, "Multiple Display off Hint Exist in Active List");
            }
        }
        snprintf(outfile, OUT_LINE_BUF, "%" PRId32 "|0x%" PRIX32 "|%s|%s\n",handle, hint_id, comments, getPassFail(rc == SUCCESS, rc_s));
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
        if (rc == FAILED) {
            res = FAILED;
            break;
        }
    }
    fclose(defval);
    return res;
}


int32_t PerfTest::DisplayOFF(const char *filename) {

    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};
    char node_val_before[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected Val|Observed Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;
    for (uint16_t idx=0; idx<MAX_MINOR_RESOURCES; idx++) {

        perf_hint(VENDOR_HINT_DISPLAY_OFF, filename, 10000000, -1);

        memset(node_val_after,0, NODE_MAX);
        memset(node_val_before, 0, NODE_MAX);
        memset(rc_s, 0, NODE_MAX);
        rc = FAILED;
        QLOGI(LOCK_TEST_TAG, "Running for Qindex : %" PRIu16, idx);
        auto tmp = mTestResourceConfigInfo.find(idx);
        if (tmp == mTestResourceConfigInfo.end()) {
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8
                    "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%s\n"
                    ,idx, 0, -1, -1,"Not Exist in test xml", -1 ,-1,-1, -1,-1,-1, getPassFail(rc == SUCCESS, rc_s));
        } else {
            testResourceConfigInfo &resource = tmp->second;
            QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor ox%" PRIX16 "", resource.mMajor, resource.mMinor);
            rc = SUCCESS;
            int32_t opcode = FAILED;
            opcode = getOpcode(resource);
            expected_val = resource.mReadVal;
            getNodePath(opcode, resource, node_paths);
            if (resource.mExceptionFlag) {
                rc = SUCCESS;
                snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
            } else if (resource.mSupported == false) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
                snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
            } else if (resource.mSpecialNode) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Special Node");
                snprintf(node_val_after, NODE_MAX, "Special Node");
            } else {
                rc = checkNodeExist(node_paths);
                if (rc == FAILED) {
                    snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                    snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                    res = rc;
                } else {
                    rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                    processString(node_val_before);
                    lock_args[0] = opcode;
                    lock_args[1] = resource.mWriteValue;

                    int32_t handle = perf_lock_acq(0, TIMEOUT_DURATION, lock_args, 2);
                    if (handle > 0) {
                        processString(node_val_before);
                        uint32_t duration = TIMEOUT_DURATION_SLEEP;
                        gotoSleep(duration);
                        rc = getNodeVal(opcode, resource, node_paths, node_val_after);
                        perf_lock_rel(handle);
                        processString(node_val_after);
                        expected_val = resource.mReadVal;
                        rc = checkNodeVal(opcode, resource, node_val_after, expected_val, node_paths);
                        if (rc == SUCCESS) {
                            rc = FAILED;
                        } else {
                            rc = SUCCESS;
                        }

                        if (rc == FAILED && areSameStrings(node_val_before, node_val_after) == SUCCESS) {
                            rc = SUCCESS;
                        }
                    } else {
                        snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                    }
                }
            }
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX16 "|0x%" PRIX16
                    "|%s|%" PRId8 "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s\n",
                    resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                    node_paths, resource.mSpecialNode,
                    resource.mSupported, node_val_before,
                    resource.mWriteValue, expected_val,
                    node_val_after, getPassFail(rc == SUCCESS, rc_s));
        }
        if (rc == FAILED) {
            res = rc;
        }
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }
    perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}


int32_t PerfTest::DisplayOnOFFAlwaysApply(const char *filename) {
    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};
    char node_val_before[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected Val|Observed Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;
    for (uint16_t idx=0; idx<MAX_MINOR_RESOURCES; idx++) {

        perf_hint(VENDOR_HINT_DISPLAY_OFF, filename, 10000000, -1);

        memset(node_val_after,0, NODE_MAX);
        memset(node_val_before, 0, NODE_MAX);
        memset(rc_s, 0, NODE_MAX);
        rc = FAILED;
        QLOGI(LOCK_TEST_TAG, "Running for Qindex : %" PRIu16, idx);
        auto tmp = mTestResourceConfigInfo.find(idx);
        if (tmp == mTestResourceConfigInfo.end()) {
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8
                    "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%s\n",idx, 0, -1, -1,"Not Exist in test xml",
                    -1 ,-1,-1, -1,-1,-1, getPassFail(rc == SUCCESS, rc_s));
        } else {
            testResourceConfigInfo &resource = tmp->second;
            QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor 0x%" PRIX16, resource.mMajor, resource.mMinor);
            rc = SUCCESS;
            uint32_t opcode = FAILED;
            opcode = getOpcode(resource);
            expected_val = resource.mReadVal;
            getNodePath(opcode, resource, node_paths);
            if (resource.mExceptionFlag) {
                rc = SUCCESS;
                snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
            } else if (resource.mSupported == false) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
                snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
            } else if (resource.mSpecialNode) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Special Node");
                snprintf(node_val_after, NODE_MAX, "Special Node");
            } else {
                rc = checkNodeExist(node_paths);
                if (rc == FAILED) {
                    snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                    snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                    res = rc;
                } else {
                    rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                    processString(node_val_before);
                    lock_args[0] = opcode;
                    lock_args[1] = resource.mWriteValue;
                    lock_args[2] = MPCTLV3_ALWAYS_ALLOW_OPCODE;
                    lock_args[3] = 1;

                    int32_t handle = perf_lock_acq(0, TIMEOUT_DURATION, lock_args, 4);
                    if (handle > 0) {
                        processString(node_val_before);
                        uint32_t duration = TIMEOUT_DURATION_SLEEP;
                        gotoSleep(duration);
                        rc = getNodeVal(opcode, resource, node_paths, node_val_after);
                        perf_lock_rel(handle);
                        processString(node_val_after);
                        expected_val = resource.mReadVal;
                        rc = checkNodeVal(opcode, resource, node_val_after, expected_val, node_paths);
                    } else {
                        snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                    }
                }
            }
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX8 "|0x%" PRIX8 "|%s|%" PRId8
                    "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s\n",
                    resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                    node_paths, resource.mSpecialNode,
                    resource.mSupported, node_val_before,
                    resource.mWriteValue, expected_val,
                    node_val_after, getPassFail(rc == SUCCESS, rc_s));
        }
        if (rc == FAILED) {
            res = rc;
        }
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }
    perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}

//DisplayOnOffTest
//TC2
//DISPLAY_ON_OFF_REQUEST_PENDED_TEST
//Send Req -> send display off -> check value applied removed from node even if request exist.
int32_t PerfTest::DisplayOnOFFRequestPended(const char *filename) {

    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};
    char node_val_before[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char expected_pend_val[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected RVal|Observed RVal|Expected Pended Val|Observed Pended Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;
    perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
    gotoSleep(WAIT_FOR_UPDATE);
    for (uint16_t idx=0; idx<MAX_MINOR_RESOURCES; idx++) {

        clearAllReqExistInActiveReq();
        gotoSleep(WAIT_FOR_UPDATE);
        memset(node_val_after,0, NODE_MAX);
        memset(node_val_before, 0, NODE_MAX);
        memset(expected_pend_val, 0, NODE_MAX);
        memset(rc_s, 0, NODE_MAX);
        rc = FAILED;
        QLOGI(LOCK_TEST_TAG, "Running for Qindex : %" PRIu16, idx);
        auto tmp = mTestResourceConfigInfo.find(idx);
        if (tmp == mTestResourceConfigInfo.end()) {
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8
                    "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|-1|-1|%s\n"
                    ,idx, 0, -1, -1,"Not Exist in test xml", -1 ,-1,-1, -1,-1,-1, getPassFail(rc == SUCCESS, rc_s));
        } else {
            testResourceConfigInfo &resource = tmp->second;
            QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor ox%" PRIX16 "", resource.mMajor, resource.mMinor);
            rc = SUCCESS;
            int32_t opcode = FAILED;
            opcode = getOpcode(resource);
            expected_val = resource.mReadVal;
            getNodePath(opcode, resource, node_paths);
            if (resource.mExceptionFlag) {
                rc = SUCCESS;
                snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(expected_pend_val, NODE_MAX, "Exception: %s",resource.mException);
            } else if (resource.mSupported == false) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
                snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
                snprintf(expected_pend_val, NODE_MAX, "Node Not Supported on target");
            } else if (resource.mSpecialNode) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Special Node");
                snprintf(node_val_after, NODE_MAX, "Special Node");
                snprintf(expected_pend_val, NODE_MAX, "Special Node");
            } else {
                rc = checkNodeExist(node_paths);
                if (rc == FAILED) {
                    snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                    snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                    snprintf(expected_pend_val, NODE_MAX, "Node Does not exist on target");
                    res = rc;
                } else {
                        QLOGE(LOCK_TEST_TAG, "==================================");
                    rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                    processString(node_val_before);
                    snprintf(expected_pend_val, NODE_MAX, "%s", node_val_before);
                    processString(expected_pend_val);
                    lock_args[0] = opcode;
                    lock_args[1] = resource.mWriteValue;
                    int32_t handle = perf_lock_acq(0, TIMEOUT_10_SEC, lock_args, 2);
                    if (handle > 0) {
                        gotoSleep(WAIT_FOR_UPDATE);
                        getNodeVal(opcode, resource, node_paths, node_val_after);
                        processString(node_val_after);
                        perf_hint(VENDOR_HINT_DISPLAY_OFF, filename, 10000000, -1); //Turn Display Off
                        gotoSleep(WAIT_FOR_UPDATE);
                        getNodeVal(opcode, resource, node_paths, expected_pend_val);
                        processString(expected_pend_val);
                        int32_t display_off_count = getDisplayOffHintCount();
                        bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);
                        perf_lock_rel(handle);
                        perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
                        gotoSleep(WAIT_FOR_UPDATE);
                        expected_val = resource.mReadVal;
                        rc = checkNodeVal(opcode, resource, node_val_after, expected_val, node_paths);
                        QLOGE(LOCK_TEST_TAG, "node_val_after: %s: Expected: %d",node_val_after, expected_val);
                        QLOGE(LOCK_TEST_TAG, "expected_pend_val: %s", expected_pend_val);
                        QLOGE(LOCK_TEST_TAG, "node_val_before: %s", node_val_before);
                        QLOGE(LOCK_TEST_TAG, "Disply Count: %d, requestSubmitted: %d", display_off_count, requestSubmitted);

                        if (rc == SUCCESS && display_off_count == 1 && requestSubmitted == true && areSameStrings(node_val_before, expected_pend_val) == SUCCESS) {
                            rc = SUCCESS;
                        } else {
                            rc = FAILED;
                        }
                    } else {
                        snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                    }
                }
            }
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX16 "|0x%" PRIX16
                    "|%s|%" PRId8 "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s|%s|%s\n",
                    resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                    node_paths, resource.mSpecialNode,
                    resource.mSupported, node_val_before,
                    resource.mWriteValue, expected_val, node_val_after, node_val_before,
                    expected_pend_val, getPassFail(rc == SUCCESS, rc_s));
                    QLOGE(LOCK_TEST_TAG, "%s", outfile);
        }
        if (rc == FAILED) {
            res = rc;
        }
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }
    perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}

//DisplayOffTest
//TC2
//DISPLAY_OFF_REQUEST_UNPENDED_TEST
//send display off -> sendperflock req  -> send display on ->check value applied removed on display on.
int32_t PerfTest::DisplayOFFCheckUnpended(const char *filename) {

    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};
    char node_val_before[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char observed_unpend_val[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|During DisplayOff|Expected UnPended Val|Observed Unpended Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;
    for (uint16_t idx=0; idx<MAX_MINOR_RESOURCES; idx++) {

        clearAllReqExistInActiveReq();
        gotoSleep(WAIT_FOR_UPDATE);
        memset(node_val_after,0, NODE_MAX);
        memset(node_val_before, 0, NODE_MAX);
        memset(observed_unpend_val, 0, NODE_MAX);
        memset(rc_s, 0, NODE_MAX);
        rc = FAILED;
        QLOGI(LOCK_TEST_TAG, "Running for Qindex : %" PRIu16, idx);
        auto tmp = mTestResourceConfigInfo.find(idx);
        if (tmp == mTestResourceConfigInfo.end()) {
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8
                    "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|-1|%s\n"
                    ,idx, 0, -1, -1,"Not Exist in test xml", -1 ,-1,-1, -1,-1,-1, getPassFail(rc == SUCCESS, rc_s));
        } else {
            testResourceConfigInfo &resource = tmp->second;
            QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor ox%" PRIX16 "", resource.mMajor, resource.mMinor);
            rc = SUCCESS;
            int32_t opcode = FAILED;
            opcode = getOpcode(resource);
            expected_val = resource.mReadVal;
            getNodePath(opcode, resource, node_paths);
            if (resource.mExceptionFlag) {
                rc = SUCCESS;
                snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(observed_unpend_val, NODE_MAX, "Exception: %s",resource.mException);
            } else if (resource.mSupported == false) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
                snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
                snprintf(observed_unpend_val, NODE_MAX, "Node Not Supported on target");
            } else if (resource.mSpecialNode) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Special Node");
                snprintf(node_val_after, NODE_MAX, "Special Node");
                snprintf(observed_unpend_val, NODE_MAX, "Special Node");
            } else {
                rc = checkNodeExist(node_paths);
                if (rc == FAILED) {
                    snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                    snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                    snprintf(observed_unpend_val, NODE_MAX, "Node Does not exist on target");
                    res = rc;
                } else {
                    rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                    processString(node_val_before);
                    gotoSleep(WAIT_FOR_UPDATE);
                    lock_args[0] = opcode;
                    lock_args[1] = resource.mWriteValue;

                    perf_hint(VENDOR_HINT_DISPLAY_OFF, filename, 10000000, -1); //Turn Display Off
                    gotoSleep(WAIT_FOR_UPDATE);
                    int32_t handle = perf_lock_acq(0, TIMEOUT_10_SEC, lock_args, 2);
                    if (handle > 0) {
                        rc = getNodeVal(opcode, resource, node_paths, node_val_after);
                        processString(node_val_after);
                        int32_t display_off_count = getDisplayOffHintCount();
                        bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);
                        perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1); //Turn Display On
                        gotoSleep(WAIT_FOR_UPDATE);
                        int32_t display_off_after_count = getDisplayOffHintCount();
                        rc = getNodeVal(opcode, resource, node_paths, observed_unpend_val);
                        processString(observed_unpend_val);
                        perf_lock_rel(handle);
                        expected_val = resource.mReadVal;
                        rc = checkNodeVal(opcode, resource, observed_unpend_val, expected_val, node_paths);
                        if (rc == SUCCESS && display_off_count == 1 && requestSubmitted == true && areSameStrings(node_val_before, node_val_after) == SUCCESS && display_off_after_count == 0) {
                            rc = SUCCESS;
                        } else {
                            rc = FAILED;
                        }
                    } else {
                        snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                    }
                }
            }
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX16 "|0x%" PRIX16
                    "|%s|%" PRId8 "|%" PRId8 "|%s|%" PRId32 "|%s|%" PRIu32 "|%s|%s\n",
                    resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                    node_paths, resource.mSpecialNode,
                    resource.mSupported, node_val_before,
                    resource.mWriteValue, node_val_after, expected_val,
                    observed_unpend_val, getPassFail(rc == SUCCESS, rc_s));
        }
        if (rc == FAILED) {
            res = rc;
        }
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}


//DisplayOffTest
//TC3
//DISPLAY_OFF_REQUEST_EXPIRED_RELEASED_TEST
//send display off -> sendperflock req  -> let timer expire for request -> send display on ->check value not applied and request not in dump.
int32_t PerfTest::DisplayOFFReqExpired(const char *filename) {

    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};
    char node_val_before[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected Val|Observed Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;
    for (uint16_t idx=0; idx<MAX_MINOR_RESOURCES; idx++) {

        clearAllReqExistInActiveReq();
        perf_hint(VENDOR_HINT_DISPLAY_OFF, filename, 10000000, -1);
        gotoSleep(WAIT_FOR_UPDATE);

        memset(node_val_after,0, NODE_MAX);
        memset(node_val_before, 0, NODE_MAX);
        memset(rc_s, 0, NODE_MAX);
        rc = FAILED;
        QLOGI(LOCK_TEST_TAG, "Running for Qindex : %" PRIu16, idx);
        auto tmp = mTestResourceConfigInfo.find(idx);
        if (tmp == mTestResourceConfigInfo.end()) {
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8
                    "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%s\n"
                    ,idx, 0, -1, -1,"Not Exist in test xml", -1 ,-1,-1, -1,-1,-1, getPassFail(rc == SUCCESS, rc_s));
        } else {
            testResourceConfigInfo &resource = tmp->second;
            QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor ox%" PRIX16 "", resource.mMajor, resource.mMinor);
            rc = SUCCESS;
            int32_t opcode = FAILED;
            opcode = getOpcode(resource);
            expected_val = resource.mReadVal;
            getNodePath(opcode, resource, node_paths);
            if (resource.mExceptionFlag) {
                rc = SUCCESS;
                snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
            } else if (resource.mSupported == false) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
                snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
            } else if (resource.mSpecialNode) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Special Node");
                snprintf(node_val_after, NODE_MAX, "Special Node");
            } else {
                rc = checkNodeExist(node_paths);
                if (rc == FAILED) {
                    snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                    snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                    res = rc;
                } else {
                    rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                    processString(node_val_before);
                    lock_args[0] = opcode;
                    lock_args[1] = resource.mWriteValue;

                    uint32_t duration = TIMEOUT_DURATION;
                    int32_t handle = perf_lock_acq(0, duration, lock_args, 2);
                    if (handle > 0) {
                        gotoSleep(duration);
                        perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
                        gotoSleep(WAIT_FOR_UPDATE);
                        processString(node_val_before);
                        rc = getNodeVal(opcode, resource, node_paths, node_val_after);
                        processString(node_val_after);
                        expected_val = resource.mReadVal;
                        rc = checkNodeVal(opcode, resource, node_val_after, expected_val, node_paths);

                        bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);
                        if (rc == SUCCESS || requestSubmitted == true) {
                            rc = FAILED;
                        } else {
                            rc = SUCCESS;
                        }

                        if (rc == FAILED && areSameStrings(node_val_before, node_val_after) == SUCCESS) {
                           rc = SUCCESS;
                        }
                    } else {
                        snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                    }
                }
            }
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX16 "|0x%" PRIX16
                    "|%s|%" PRId8 "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s\n",
                    resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                    node_paths, resource.mSpecialNode,
                    resource.mSupported, node_val_before,
                    resource.mWriteValue, expected_val,
                    node_val_after, getPassFail(rc == SUCCESS, rc_s));
        }
        if (rc == FAILED) {
            res = rc;
        }
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}

//DisplayOnOffAlwaysApply
//TC:2
//DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPIRED_REL_TEST
//send display off -> sendperflock req with always apply  -> let timer expire for request ->check value not applied and request not in dump.
int32_t PerfTest::DisplayOnOFFAlwaysApplyReqExpired(const char *filename) {
    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};
    char node_val_before[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected Val|Observed Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;
    for (uint16_t idx=0; idx<MAX_MINOR_RESOURCES; idx++) {

        clearAllReqExistInActiveReq();
        perf_hint(VENDOR_HINT_DISPLAY_OFF, filename, 10000000, -1);
        gotoSleep(WAIT_FOR_UPDATE);

        memset(node_val_after,0, NODE_MAX);
        memset(node_val_before, 0, NODE_MAX);
        memset(rc_s, 0, NODE_MAX);
        rc = FAILED;
        QLOGI(LOCK_TEST_TAG, "Running for Qindex : %" PRIu16, idx);
        auto tmp = mTestResourceConfigInfo.find(idx);
        if (tmp == mTestResourceConfigInfo.end()) {
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8
                    "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%s\n",idx, 0, -1, -1,"Not Exist in test xml",
                    -1 ,-1,-1, -1,-1,-1, getPassFail(rc == SUCCESS, rc_s));
        } else {
            testResourceConfigInfo &resource = tmp->second;
            QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor 0x%" PRIX16, resource.mMajor, resource.mMinor);
            rc = SUCCESS;
            uint32_t opcode = FAILED;
            opcode = getOpcode(resource);
            expected_val = resource.mReadVal;
            getNodePath(opcode, resource, node_paths);
            if (resource.mExceptionFlag) {
                rc = SUCCESS;
                snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
            } else if (resource.mSupported == false) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
                snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
            } else if (resource.mSpecialNode) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Special Node");
                snprintf(node_val_after, NODE_MAX, "Special Node");
            } else {
                rc = checkNodeExist(node_paths);
                if (rc == FAILED) {
                    snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                    snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                    res = rc;
                } else {
                    rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                    processString(node_val_before);
                    lock_args[0] = opcode;
                    lock_args[1] = resource.mWriteValue;
                    lock_args[2] = MPCTLV3_ALWAYS_ALLOW_OPCODE;
                    lock_args[3] = 1;

                    uint32_t duration = TIMEOUT_DURATION;
                    int32_t handle = perf_lock_acq(0, duration, lock_args, 4);
                    if (handle > 0) {
                        gotoSleep(duration);
                        gotoSleep(WAIT_FOR_UPDATE);
                        rc = getNodeVal(opcode, resource, node_paths, node_val_after);
                        processString(node_val_after);
                        expected_val = resource.mReadVal;
                        rc = checkNodeVal(opcode, resource, node_val_after, expected_val, node_paths);

                        bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);

                        if (rc == SUCCESS || requestSubmitted == true) {
                            rc = FAILED;
                        } else {
                            rc = SUCCESS;
                        }

                        if (rc == FAILED && areSameStrings(node_val_before, node_val_after) == SUCCESS) {
                           rc = SUCCESS;
                        }
                    } else {
                        snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                    }
                }
            }
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX8 "|0x%" PRIX8 "|%s|%" PRId8
                    "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s\n",
                    resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                    node_paths, resource.mSpecialNode,
                    resource.mSupported, node_val_before,
                    resource.mWriteValue, expected_val,
                    node_val_after, getPassFail(rc == SUCCESS, rc_s));
        }
        if (rc == FAILED) {
            res = rc;
        }
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
    }
    perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}

//DisplayOnOffAlwaysApply
//TC:3
//DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_TEST
//send display off -> sendperflock req with always apply  -> send release req  ->check value not applied and request not in dump.
int32_t PerfTest::DisplayOnOFFAlwaysApplyRelReq(const char *filename) {
    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};
    char node_val_before[NODE_MAX] = {0};
    char node_val_mid[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected Before Rel|Observed Before Rel|Expected Val|Observed Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;
    for (uint16_t idx=0; idx<MAX_MINOR_RESOURCES; idx++) {

        clearAllReqExistInActiveReq();
        perf_hint(VENDOR_HINT_DISPLAY_OFF, filename, 10000000, -1);
        gotoSleep(WAIT_FOR_UPDATE);

        memset(node_val_after,0, NODE_MAX);
        memset(node_val_mid,0, NODE_MAX);
        memset(node_val_before, 0, NODE_MAX);
        memset(rc_s, 0, NODE_MAX);
        rc = FAILED;
        QLOGI(LOCK_TEST_TAG, "Running for Qindex : %" PRIu16, idx);
        auto tmp = mTestResourceConfigInfo.find(idx);
        if (tmp == mTestResourceConfigInfo.end()) {
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8
                    "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%s\n",idx, 0, -1, -1,"Not Exist in test xml",
                    -1 ,-1, -1, -1, -1,-1,-1, getPassFail(rc == SUCCESS, rc_s));
        } else {
            testResourceConfigInfo &resource = tmp->second;
            QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor 0x%" PRIX16, resource.mMajor, resource.mMinor);
            rc = SUCCESS;
            uint32_t opcode = FAILED;
            opcode = getOpcode(resource);
            expected_val = resource.mReadVal;
            getNodePath(opcode, resource, node_paths);
            if (resource.mExceptionFlag) {
                rc = SUCCESS;
                snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(node_val_mid, NODE_MAX, "Exception: %s",resource.mException);
                snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
            } else if (resource.mSupported == false) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
                snprintf(node_val_mid, NODE_MAX, "Node Not Supported on target");
                snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
            } else if (resource.mSpecialNode) {
                rc = SUCCESS;
                snprintf(node_val_before, NODE_MAX, "Special Node");
                snprintf(node_val_mid, NODE_MAX, "Special Node");
                snprintf(node_val_after, NODE_MAX, "Special Node");
            } else {
                rc = checkNodeExist(node_paths);
                if (rc == FAILED) {
                    snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                    snprintf(node_val_mid, NODE_MAX, "Node Does not exist on target");
                    snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                    res = rc;
                } else {
                    rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                    processString(node_val_before);
                    QLOGE(LOCK_TEST_TAG, "node_val_before %s %zu\n", node_val_before, strlen(node_val_before));
                    lock_args[0] = opcode;
                    lock_args[1] = resource.mWriteValue;
                    lock_args[2] = MPCTLV3_ALWAYS_ALLOW_OPCODE;
                    lock_args[3] = 1;

                    uint32_t duration = TIMEOUT_DURATION;
                    int32_t handle = perf_lock_acq(0, duration, lock_args, 4);
                    if (handle > 0) {
                        gotoSleep(duration/2);
                        rc = getNodeVal(opcode, resource, node_paths, node_val_mid);
                        processString(node_val_mid);
                        perf_lock_rel(handle);
                        gotoSleep(WAIT_FOR_UPDATE);
                        expected_val = resource.mReadVal;
                        rc = checkNodeVal(opcode, resource, node_val_mid, expected_val, node_paths);

                        getNodeVal(opcode, resource, node_paths, node_val_after);
                        processString(node_val_after);
                        QLOGE(LOCK_TEST_TAG, " wr %d - ex %d node_val_after %s md%s\n",lock_args[1], expected_val, node_val_after ,node_val_mid);
                        bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);
                        QLOGE(LOCK_TEST_TAG, " %d %d %d \n", rc, requestSubmitted, areSameStrings(node_val_after, node_val_before) );
                        if (rc == FAILED || requestSubmitted == true || areSameStrings(node_val_after, node_val_before) != SUCCESS) {
                            rc = FAILED;
                        } else {
                            rc = SUCCESS;
                        }
                    } else {
                        snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                    }
                }
            }

            snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX8 "|0x%" PRIX8 "|%s|%" PRId8
                    "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s|%s|%s\n",
                    resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                    node_paths, resource.mSpecialNode,
                    resource.mSupported, node_val_before,
                    resource.mWriteValue, expected_val, node_val_mid, node_val_before,
                    node_val_after, getPassFail(rc == SUCCESS, rc_s));
        }
        if (rc == FAILED) {
            res = rc;
        }
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
        QLOGE(LOCK_TEST_TAG, " %s \n", outfile);

    }
    perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}

//DisplayOnOffAlwaysApply
//TC:4
//DISPLAY_ON_OFF_ALWAYS_APPLY_FREQ
//send perflock request -> send display off -> sendperflock req with always apply  -> send release for always apply  ->check value not applied and request not in dump.
int32_t PerfTest::DisplayOnOFFAlwaysApplyRelReqAfterDisaplyOff(const char *filename) {
    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};

    char node_val_before[NODE_MAX] = {0};
    char node_val_mid[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected Before Rel|Observed Before Rel|Expected Val|Observed Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;

    clearAllReqExistInActiveReq();
    perf_hint(VENDOR_HINT_DISPLAY_ON , filename, 10000000, -1);
    gotoSleep(WAIT_FOR_UPDATE);
    memset(node_val_after,0, NODE_MAX);
    memset(node_val_mid,0, NODE_MAX);
    memset(node_val_before, 0, NODE_MAX);
    memset(rc_s, 0, NODE_MAX);
    uint16_t idx = 0;
    rc = FAILED;
    QLOGI(LOCK_TEST_TAG, "Running for Qindex : %" PRIu16, idx);
    auto tmp = mTestResourceConfigInfo.find(CPUFREQ_START_INDEX + CPUFREQ_MIN_FREQ_OPCODE);
    if (tmp == mTestResourceConfigInfo.end()) {
        snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8
                "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%s\n",idx, 0, -1, -1,"Not Exist in test xml",
                -1 ,-1, -1, -1, -1,-1,-1, getPassFail(rc == SUCCESS, rc_s));
    } else {
        testResourceConfigInfo &resource = tmp->second;
        QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor 0x%" PRIX16, resource.mMajor, resource.mMinor);
        rc = SUCCESS;
        uint32_t opcode = FAILED;
        opcode = getOpcode(resource);
        expected_val = resource.mReadVal;
        getNodePath(opcode, resource, node_paths);
        if (resource.mExceptionFlag) {
            rc = SUCCESS;
            snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
            snprintf(node_val_mid, NODE_MAX, "Exception: %s",resource.mException);
            snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
             QLOGI(LOCK_TEST_TAG, "Exception: %s",resource.mException);
        } else if (resource.mSupported == false) {
            rc = SUCCESS;
            snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
            snprintf(node_val_mid, NODE_MAX, "Node Not Supported on target");
            snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
             QLOGI(LOCK_TEST_TAG, "Failed at %s", "Node Not Supported on target");
        } else if (resource.mSpecialNode) {
            rc = SUCCESS;
            snprintf(node_val_before, NODE_MAX, "Special Node");
            snprintf(node_val_mid, NODE_MAX, "Special Node");
            snprintf(node_val_after, NODE_MAX, "Special Node");
             QLOGI(LOCK_TEST_TAG, "Failed at %s", "Special Node");
        } else {
            rc = checkNodeExist(node_paths);
            if (rc == FAILED) {
                snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                snprintf(node_val_mid, NODE_MAX, "Node Does not exist on target");
                snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                res = rc;
                 QLOGI(LOCK_TEST_TAG, "Failed at %s", "Node Does not exist on target");
            } else {
                int32_t firstHandle = -1;
                auto retval = SendRegularSLBRequest(firstHandle);
                if (retval == FAILED)


                perf_hint(VENDOR_HINT_DISPLAY_OFF, filename, 10000000, -1);
                gotoSleep(WAIT_FOR_UPDATE);
                rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                processString(node_val_before);
                lock_args[0] = opcode;
                lock_args[1] = resource.mWriteValue;
                lock_args[2] = MPCTLV3_ALWAYS_ALLOW_OPCODE;
                lock_args[3] = 1;

                uint32_t duration = TIMEOUT_DURATION*10;
                int32_t handle = perf_lock_acq(0, duration, lock_args, 4);
                if (handle > 0) {
                    gotoSleep(duration/2);
                    rc = getNodeVal(opcode, resource, node_paths, node_val_mid);
                     QLOGI(LOCK_TEST_TAG, " getNodeVal %d  ",rc);
                    processString(node_val_mid);
                    gotoSleep(WAIT_FOR_UPDATE);
                    expected_val = resource.mReadVal;
                    rc = checkNodeVal(opcode, resource, node_val_mid, expected_val, node_paths);
                    QLOGI(LOCK_TEST_TAG, " getNodeVal %d %d %s  ",rc,expected_val,node_val_mid);
                    bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);
                    QLOGI(LOCK_TEST_TAG, " rc  %d handle %d requestSubmitted %d ",rc, handle, requestSubmitted);
                    if (rc == SUCCESS && requestSubmitted == true) {
                        rc = SUCCESS;
                    } else {
                        rc = FAILED;
                    }

                    QLOGI(LOCK_TEST_TAG, "Failed at  handle > 0 rc %" PRId32 , rc);
                } else {
                    rc = FAILED;
                    snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                    QLOGI(LOCK_TEST_TAG, "Failed at %s", node_val_after);
                }
                if (rc == SUCCESS) {
                    perf_lock_rel(handle);
                    gotoSleep(WAIT_FOR_UPDATE);
                    char node_val_post_release[NODE_MAX] = {0};
                    memset(node_val_post_release,0,NODE_MAX);
                    bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);

                    auto postReleaseValValid =
                            getNodeVal(opcode, resource, node_paths, node_val_post_release);
                    processString(node_val_post_release);
                    bool isReseted = areSameStrings(node_val_post_release, node_val_before);

                    if (isReseted == FAILED || requestSubmitted == true || postReleaseValValid == FAILED) {
                        rc = FAILED;
                        QLOGI(LOCK_TEST_TAG, "Failed at release\n");
                    }
                    QLOGI(LOCK_TEST_TAG, "done at perf_lock_rel\n");
                }
            }
        }
        snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX8 "|0x%" PRIX8 "|%s|%" PRId8
                "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s|%s|%s\n",
                resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                node_paths, resource.mSpecialNode,
                resource.mSupported, node_val_before,
                resource.mWriteValue, expected_val, node_val_mid, node_val_before,
                node_val_after, getPassFail(rc == SUCCESS, rc_s));
    }
    if (rc == FAILED) {
        res = rc;
    }
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}

int32_t PerfTest::SendRegularSLBRequest(int32_t &passedHandle) {
    int32_t res = FAILED;
    int32_t rc = FAILED;
    char rc_s[NODE_MAX] = {0};

    char node_val_before[NODE_MAX] = {0};
    char node_val_mid[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};
    memset(node_val_after,0, NODE_MAX);
    memset(node_val_mid,0, NODE_MAX);
    memset(node_val_before, 0, NODE_MAX);
    memset(rc_s, 0, NODE_MAX);
    auto tmp = mTestResourceConfigInfo.find(SCHED_START_INDEX + SCHED_LOAD_BOOST_OPCODE);
    if (tmp == mTestResourceConfigInfo.end()) {
        QLOGI(LOCK_TEST_TAG, "Node Not Found\n");
    } else {
        testResourceConfigInfo &resource = tmp->second;
        QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor 0x%" PRIX16, resource.mMajor, resource.mMinor);
        rc = SUCCESS;
        uint32_t opcode = FAILED;
        opcode = getOpcode(resource);
        expected_val = resource.mReadVal;
        getNodePath(opcode, resource, node_paths);
        if (resource.mExceptionFlag) {
            rc = SUCCESS;
             QLOGI(LOCK_TEST_TAG, "Exception: %s",resource.mException);
        } else if (resource.mSupported == false) {
            rc = SUCCESS;
             QLOGI(LOCK_TEST_TAG, "Failed at %s", "Node Not Supported on target");
        } else if (resource.mSpecialNode) {
            rc = SUCCESS;
             QLOGI(LOCK_TEST_TAG, "Failed at %s", "Special Node");
        } else {
            rc = checkNodeExist(node_paths);
            if (rc == FAILED) {
                res = rc;
                 QLOGI(LOCK_TEST_TAG, "Failed at %s", "Node Does not exist on target");
            } else {

                rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                processString(node_val_before);
                lock_args[0] = opcode;
                lock_args[1] = resource.mWriteValue;

                uint32_t duration = 1000000;
                passedHandle = perf_lock_acq(0, duration, lock_args, 2);
                if (passedHandle > 0) {
                    gotoSleep(WAIT_FOR_UPDATE);
                    rc = getNodeVal(opcode, resource, node_paths, node_val_mid);
                     QLOGI(LOCK_TEST_TAG, " getNodeVal %d  ",rc);
                    processString(node_val_mid);
                    expected_val = resource.mReadVal;
                    rc = checkNodeVal(opcode, resource, node_val_mid, expected_val, node_paths);
                    processString(node_val_after);
                    QLOGI(LOCK_TEST_TAG, " checkNodeVal %d  ",rc);
                    bool requestSubmitted = checkReqExistInActiveReq(passedHandle, REQ_DUMP_HANDLE_COLUMN);
                    QLOGI(LOCK_TEST_TAG, " rc  %d passedHandle %d requestSubmitted %d reSameStrings(node_val_after, node_val_before) == false %d",rc, passedHandle, requestSubmitted, areSameStrings(node_val_after, node_val_before));
                    if (rc == SUCCESS || requestSubmitted == true
                        || areSameStrings(node_val_after, node_val_before) == FAILED) {
                        rc = SUCCESS;
                    } else {
                        rc = FAILED;
                    }

                    QLOGI(LOCK_TEST_TAG, "Failed at  passedHandle > 0 rc %" PRId32 , rc);
                } else {
                    rc = FAILED;
                    QLOGI(LOCK_TEST_TAG, "Failed at %s %d", node_val_after, resource.mWriteValue);
                }

            }
        }
    }
    if (rc == FAILED) {
        res = rc;
    }
    return res;
}

int32_t PerfTest::DisplayOnOFFAlwaysApplyRelReqTimedOutAfterDisaplyOff(const char *filename) {
    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};

    char node_val_before[NODE_MAX] = {0};
    char node_val_mid[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};
    if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected Before Rel|Observed Before Rel|Expected Val|Observed Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;

    clearAllReqExistInActiveReq();
    perf_hint(VENDOR_HINT_DISPLAY_ON , filename, 10000000, -1);
    gotoSleep(WAIT_FOR_UPDATE);
    memset(node_val_after,0, NODE_MAX);
    memset(node_val_mid,0, NODE_MAX);
    memset(node_val_before, 0, NODE_MAX);
    memset(rc_s, 0, NODE_MAX);
    uint16_t idx = 0;
    rc = FAILED;
    QLOGI(LOCK_TEST_TAG, "Running for Qindex : %" PRIu16, idx);
    auto tmp = mTestResourceConfigInfo.find(CPUFREQ_START_INDEX + CPUFREQ_MIN_FREQ_OPCODE);
    if (tmp == mTestResourceConfigInfo.end()) {
        snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8
                "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%s\n",idx, 0, -1, -1,"Not Exist in test xml",
                -1 ,-1, -1, -1, -1,-1,-1, getPassFail(rc == SUCCESS, rc_s));
    } else {
        testResourceConfigInfo &resource = tmp->second;
        QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor 0x%" PRIX16, resource.mMajor, resource.mMinor);
        rc = SUCCESS;
        uint32_t opcode = FAILED;
        opcode = getOpcode(resource);
        expected_val = resource.mReadVal;
        getNodePath(opcode, resource, node_paths);
        if (resource.mExceptionFlag) {
            rc = SUCCESS;
            snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
            snprintf(node_val_mid, NODE_MAX, "Exception: %s",resource.mException);
            snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
             QLOGI(LOCK_TEST_TAG, "Exception: %s",resource.mException);
        } else if (resource.mSupported == false) {
            rc = SUCCESS;
            snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
            snprintf(node_val_mid, NODE_MAX, "Node Not Supported on target");
            snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
             QLOGI(LOCK_TEST_TAG, "Failed at %s", "Node Not Supported on target");
        } else if (resource.mSpecialNode) {
            rc = SUCCESS;
            snprintf(node_val_before, NODE_MAX, "Special Node");
            snprintf(node_val_mid, NODE_MAX, "Special Node");
            snprintf(node_val_after, NODE_MAX, "Special Node");
             QLOGI(LOCK_TEST_TAG, "Failed at %s", "Special Node");
        } else {
            rc = checkNodeExist(node_paths);
            if (rc == FAILED) {
                snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                snprintf(node_val_mid, NODE_MAX, "Node Does not exist on target");
                snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                res = rc;
                 QLOGI(LOCK_TEST_TAG, "Failed at %s", "Node Does not exist on target");
            } else {
                int32_t firstHandle = -1;
                auto retval = SendRegularSLBRequest(firstHandle);
                if (retval == FAILED)


                perf_hint(VENDOR_HINT_DISPLAY_OFF, filename, 10000000, -1);
                gotoSleep(WAIT_FOR_UPDATE);
                rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                processString(node_val_before);
                lock_args[0] = opcode;
                lock_args[1] = resource.mWriteValue;
                lock_args[2] = MPCTLV3_ALWAYS_ALLOW_OPCODE;
                lock_args[3] = 1;

                uint32_t duration = TIMEOUT_DURATION*10;
                int32_t handle = perf_lock_acq(0, duration, lock_args, 4);
                if (handle > 0) {
                    rc = getNodeVal(opcode, resource, node_paths, node_val_mid);
                     QLOGI(LOCK_TEST_TAG, " getNodeVal %d %s ",rc, node_val_mid);
                    processString(node_val_mid);
                    gotoSleep(WAIT_FOR_UPDATE);
                    expected_val = resource.mReadVal;
                    rc = checkNodeVal(opcode, resource, node_val_mid, expected_val, node_paths);
                    QLOGI(LOCK_TEST_TAG, " getNodeVal %d  %d %s",rc,expected_val, node_val_mid);
                    bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);
                    QLOGI(LOCK_TEST_TAG, " rc  %d handle %d requestSubmitted %d ",rc, handle, requestSubmitted);
                    if (rc == SUCCESS && requestSubmitted == true) {
                        rc = SUCCESS;
                    } else {
                        rc = FAILED;
                    }

                    QLOGI(LOCK_TEST_TAG, "Failed at  handle > 0 rc %" PRId32 , rc);
                } else {
                    rc = FAILED;
                    snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                    QLOGI(LOCK_TEST_TAG, "Failed at %s", node_val_after);
                }
                if (rc == SUCCESS) {
                    gotoSleep(duration+10);
                    gotoSleep(WAIT_FOR_UPDATE);
                    char node_val_post_release[NODE_MAX] = {0};
                    memset(node_val_post_release,0,NODE_MAX);
                    bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);

                    auto postReleaseValValid =
                            getNodeVal(opcode, resource, node_paths, node_val_post_release);
                    processString(node_val_post_release);
                    bool isReseted = areSameStrings(node_val_post_release, node_val_before);

                    if (isReseted == FAILED || requestSubmitted == true || postReleaseValValid == FAILED) {
                        rc = FAILED;
                        QLOGI(LOCK_TEST_TAG, "Failed at release\n");
                    }
                    QLOGI(LOCK_TEST_TAG, "done at perf_lock_rel\n");
                }
            }
        }
        snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX8 "|0x%" PRIX8 "|%s|%" PRId8
                "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s|%s|%s\n",
                resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                node_paths, resource.mSpecialNode,
                resource.mSupported, node_val_before,
                resource.mWriteValue, expected_val, node_val_mid, node_val_before,
                node_val_after, getPassFail(rc == SUCCESS, rc_s));
    }
    if (rc == FAILED) {
        res = rc;
    }
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}


int32_t PerfTest::DisplayOnOFFAlwaysApplyRelReqAfterDisaplyOffConceruncy(const char *filename) {
    int32_t res = FAILED;
    int32_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = {0};

    char node_val_before[NODE_MAX] = {0};
    char node_val_mid[NODE_MAX] = {0};
    char node_val_after[NODE_MAX] = {0};
    char node_paths[NODE_MAX] = {0};
    uint32_t expected_val = 0;
    int32_t lock_args[4] = {0};

    int32_t newVals_args[MAX_CONCURRENCY_REQ] = {900,800,1000,1200};
    int32_t newVals_args_read_val[MAX_CONCURRENCY_REQ] = {900000,800000,1000000,1200000};
     if (filename == NULL) {
        QLOGE(LOCK_TEST_TAG, "Invalid FileName received for %s",__func__);
        return res;
    }
    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOCK_TEST_TAG, "Cannot open/create results file\n");
        return 0;
    }
    snprintf(outfile, OUT_LINE_BUF, "Qindex|OpCode|Major|Minor|NodePath|Special Resource|Supported|Value Before Perflock|Write Value|Expected Before Rel|Observed Before Rel|Expected Val|Observed Value|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;

    clearAllReqExistInActiveReq();
    perf_hint(VENDOR_HINT_DISPLAY_ON , filename, 10000000, -1);
    gotoSleep(WAIT_FOR_UPDATE);
    memset(node_val_after,0, NODE_MAX);
    memset(node_val_mid,0, NODE_MAX);
    memset(node_val_before, 0, NODE_MAX);
    memset(rc_s, 0, NODE_MAX);
    uint16_t idx = 0;
    rc = FAILED;
    QLOGI(LOCK_TEST_TAG, "Running for Qindex : %" PRIu16, idx);
    auto tmp = mTestResourceConfigInfo.find(CPUFREQ_START_INDEX + CPUFREQ_MIN_FREQ_OPCODE);
    if (tmp == mTestResourceConfigInfo.end()) {
        snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX8 "|%" PRId8 "|%" PRId8 "|%s|%" PRId8
                "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%" PRId8 "|%s\n",idx, 0, -1, -1,"Not Exist in test xml",
                -1 ,-1, -1, -1, -1,-1,-1, getPassFail(rc == SUCCESS, rc_s));
    } else {
        testResourceConfigInfo &resource = tmp->second;
        QLOGI(LOCK_TEST_TAG, "Running for Major 0x%" PRIX16 " minor 0x%" PRIX16, resource.mMajor, resource.mMinor);
        rc = SUCCESS;
        uint32_t opcode = FAILED;
        opcode = getOpcode(resource);
        expected_val = resource.mReadVal;
        getNodePath(opcode, resource, node_paths);
        if (resource.mExceptionFlag) {
            rc = SUCCESS;
            snprintf(node_val_after, NODE_MAX, "Exception: %s",resource.mException);
            snprintf(node_val_mid, NODE_MAX, "Exception: %s",resource.mException);
            snprintf(node_val_before, NODE_MAX, "Exception: %s",resource.mException);
             QLOGI(LOCK_TEST_TAG, "Exception: %s",resource.mException);
        } else if (resource.mSupported == false) {
            rc = SUCCESS;
            snprintf(node_val_before, NODE_MAX, "Node Not Supported on target");
            snprintf(node_val_mid, NODE_MAX, "Node Not Supported on target");
            snprintf(node_val_after, NODE_MAX, "Node Not Supported on target");
             QLOGI(LOCK_TEST_TAG, "Failed at %s", "Node Not Supported on target");
        } else if (resource.mSpecialNode) {
            rc = SUCCESS;
            snprintf(node_val_before, NODE_MAX, "Special Node");
            snprintf(node_val_mid, NODE_MAX, "Special Node");
            snprintf(node_val_after, NODE_MAX, "Special Node");
             QLOGI(LOCK_TEST_TAG, "Failed at %s", "Special Node");
        } else {
            rc = checkNodeExist(node_paths);
            if (rc == FAILED) {
                snprintf(node_val_before, NODE_MAX, "Node Does not exist on target");
                snprintf(node_val_mid, NODE_MAX, "Node Does not exist on target");
                snprintf(node_val_after, NODE_MAX, "Node Does not exist on target");
                res = rc;
                 QLOGI(LOCK_TEST_TAG, "Failed at %s", "Node Does not exist on target");
            } else {
                int32_t firstHandle = -1;
                auto retval = SendRegularSLBRequest(firstHandle);
                if (retval == FAILED)


                perf_hint(VENDOR_HINT_DISPLAY_OFF, filename, 10000000, -1);
                gotoSleep(WAIT_FOR_UPDATE);
                rc = getNodeVal(opcode, resource, node_paths, node_val_before);
                processString(node_val_before);
                int32_t handle = -1;
                for(int i = 0; i < MAX_CONCURRENCY_REQ; i++)
                {
                    lock_args[0] = opcode;
                    if (i) {
                        lock_args[1] = newVals_args[i];
                    } else {
                        lock_args[1] = resource.mWriteValue;
                    }

                    lock_args[2] = MPCTLV3_ALWAYS_ALLOW_OPCODE;
                    lock_args[3] = 1;

                    uint32_t duration = TIMEOUT_DURATION;
                    perf_lock_rel(handle);
                    handle = -1;
                    handle = perf_lock_acq(0, duration, lock_args, 4);
                    if (handle > 0) {
                        rc = getNodeVal(opcode, resource, node_paths, node_val_mid);
                         QLOGI(LOCK_TEST_TAG, " getNodeVal %d  ",rc);
                        processString(node_val_mid);
                        gotoSleep(WAIT_FOR_UPDATE);
                        uint32_t checkAgainst = (uint32_t)newVals_args_read_val[i];
                        rc = checkNodeVal(opcode, resource, node_val_mid, checkAgainst, node_paths);
                        QLOGI(LOCK_TEST_TAG, " getNodeVal %s, asked %d  ",node_val_mid, lock_args[1] );
                        bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);
                        QLOGI(LOCK_TEST_TAG, " rc  %d handle %d requestSubmitted %d ",rc, handle, requestSubmitted);
                        if (rc == SUCCESS && requestSubmitted == true) {
                            rc = SUCCESS;
                        } else {
                            rc = FAILED;
                        }

                        QLOGI(LOCK_TEST_TAG, "Failed at  handle > 0 rc %" PRId32 , rc);
                    } else {
                        rc = FAILED;
                        snprintf(node_val_after,NODE_MAX,"Perflock Failed");
                        QLOGI(LOCK_TEST_TAG, "Failed at %s", node_val_after);
                    }
                }
                perf_lock_rel(handle);
                if (rc == SUCCESS) {
                    gotoSleep(TIMEOUT_DURATION*10+10);
                    gotoSleep(WAIT_FOR_UPDATE);
                    char node_val_post_release[NODE_MAX] = {0};
                    memset(node_val_post_release,0,NODE_MAX);
                    bool requestSubmitted = checkReqExistInActiveReq(handle, REQ_DUMP_HANDLE_COLUMN);

                    auto postReleaseValValid =
                            getNodeVal(opcode, resource, node_paths, node_val_post_release);
                    processString(node_val_post_release);
                    bool isReseted = areSameStrings(node_val_post_release, node_val_before);

                    if (isReseted == FAILED || requestSubmitted == true || postReleaseValValid == FAILED) {
                        rc = FAILED;
                        QLOGI(LOCK_TEST_TAG, "Failed at release\n");
                    }
                    QLOGI(LOCK_TEST_TAG, "done at perf_lock_rel\n");
                }
            }
        }
        snprintf(outfile, OUT_LINE_BUF, "%" PRIu16 "|0x%" PRIX32 "|0x%" PRIX8 "|0x%" PRIX8 "|%s|%" PRId8
                "|%" PRId8 "|%s|%" PRId32 "|%" PRIu32 "|%s|%s|%s|%s\n",
                resource.mQindex,opcode, resource.mMajor, resource.mMinor,
                node_paths, resource.mSpecialNode,
                resource.mSupported, node_val_before,
                resource.mWriteValue, expected_val, node_val_mid, node_val_before,
                node_val_after, getPassFail(rc == SUCCESS, rc_s));
    }
    if (rc == FAILED) {
        res = rc;
    }
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    perf_hint(VENDOR_HINT_DISPLAY_ON, filename, 10000000, -1);
    QLOGE(LOCK_TEST_TAG, "Check %s for Result\n", filename);
    fclose(defval);
    return res;
}
//interface for TTF

static RegressionFramework ttfar1 = {
   DISPLAY_ON_OFF_TEST, PerfTest::RunTest, DISPLAY_MOD,
};

/*static RegressionFramework ttfar2 = {
   DISPLAY_ON_OFF_ALWAYS_APPLY, PerfTest::RunTest, DISPLAY_MOD,
};*/

static RegressionFramework ttfar2 = {
   DISPLAY_OFF_NODE_CHECK, PerfTest::RunTest, DISPLAY_MOD,
};

static RegressionFramework ttfar4 = {
   DISPLAY_ON_OFF_REQUEST_PENDED_TEST,  PerfTest::RunTest, DISPLAY_MOD,
};

static RegressionFramework ttfar5 = {
   DISPLAY_OFF_REQUEST_UNPENDED_TEST,  PerfTest::RunTest, DISPLAY_MOD,
};

static RegressionFramework ttfar6 = {
   DISPLAY_OFF_REQUEST_EXPIRED_RELEASED_TEST,  PerfTest::RunTest, DISPLAY_MOD,
};

static RegressionFramework ttfar7 = {
   DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPIRED_REL_TEST,  PerfTest::RunTest, DISPLAY_MOD,
};

static RegressionFramework ttfar8 = {
   DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_TEST,  PerfTest::RunTest, DISPLAY_MOD,
};

static RegressionFramework ttfar9 = {
   DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_AFTER_OFF_TEST,  PerfTest::RunTest, DISPLAY_MOD,
};

/*static RegressionFramework ttfar10 = {
   DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_EXPLICIT_REL_TIMEDOUT_AFTER_OFF_TEST,  PerfTest::RunTest, DISPLAY_MOD,
};*/

static RegressionFramework ttfar11 = {
   DISPLAY_ON_OFF_ALWAYS_APPLY_REQ_AFTER_OFF_TEST_CONCERUNCY,  PerfTest::RunTest, DISPLAY_MOD,
};

//Register Test here at end
