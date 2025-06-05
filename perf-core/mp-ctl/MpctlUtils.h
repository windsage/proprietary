/******************************************************************************
  @file    MpctlUtils.h
  @brief   Implementation of mpctl utils functions

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __MPCTL_UTILS__H_
#define __MPCTL_UTILS__H_


#include <string.h>
#define FAILED -1
#define SUCCESS 0
#define PRE_ACTION_SF_RE_VAL -2
#define PRE_ACTION_NO_FPS_UPDATE -3

#define MIN_FREQ_REQ      0
#define MAX_FREQ_REQ      1

#define PERF_PID_NODE "/data/vendor/perfd/perf_pid"

#define NODE_MAX 150
#define TRACE_BUF_SZ 512
#define PROP_NAME           "vendor.debug.trace.perf"

static uint8_t PERF_SYSTRACE;
static uint8_t perf_debug_output = 0;

static int32_t renderThreadTidOfTopApp = -1;

//originally defined in kernel:include/linux/sched.h
enum task_boost_type {
    TASK_BOOST_NONE = 0,
    TASK_BOOST_ON_MID,
    TASK_BOOST_ON_MAX,
    TASK_BOOST_STRICT_MAX,
    TASK_BOOST_END,
};

#define TIME_MSEC_IN_SEC    1000
#define TIME_NSEC_IN_MSEC   1000000

struct hint_associated_data;

void write_to_file(FILE *output, const char *input_node, const char *input_val);
int8_t  write_to_node(FILE *read_file, const char *node_str, const char *node);
void signal_chk_poll_thread(char *node, int8_t rc);
inline int8_t get_online_cpus (uint16_t * status_array, uint16_t array_sz);
int8_t get_online_core(int8_t cpu, int8_t max_num_cpu_clustr);
int16_t get_core_status(int8_t cpu);
int8_t vid_create_timer(hint_associated_data *data);
void rearm_slvp_timer(hint_associated_data *data);
void cancel_slvp_timer(hint_associated_data *data);
void send_irq_balance(int32_t *msg, int32_t **ret);
int8_t update_freq_node(int32_t start, int32_t end, uint32_t val, char *fmtStr, uint32_t len);
char* get_devfreq_new_val(uint32_t, uint32_t *, uint32_t, char* );
int32_t save_node_val(const char*, char* );
int8_t change_node_val(const char *, const char *, uint32_t);
int8_t change_node_val_buf(const char *, const char *, uint32_t);
bool minBoundCheck(char* max, char* min, char* reqval, unsigned int max_available_freq);
bool maxBoundCheck(char* max, char* min, char* reqval);

struct mpctl_msg_t;
class PerfCtxHelper {

    PerfCtxHelper() {
        mTestlibhandle = NULL;
        sys_nodes_test = NULL;
    }

    PerfCtxHelper(PerfCtxHelper const& oh);
    PerfCtxHelper& operator=(PerfCtxHelper const& oh);
    int32_t runAvcTest(mpctl_msg_t *pMsg);
    int8_t (*sys_nodes_test)(const char *fileName) = NULL;
    void *mTestlibhandle;
    const char TEST_LIB_NAME[30] = "libqti-perfd-tests.so";

    public:
    ~PerfCtxHelper() {
        Reset();
    }

    static PerfCtxHelper &getPerfCtxHelper() {
        static PerfCtxHelper mPerfCtxHelper;
        return mPerfCtxHelper;
    }

    int32_t Init();
    int32_t Reset();
    int32_t Run(mpctl_msg_t *pMsg);
    int32_t writePerfHalPID();

    static int32_t PerfCtxHelperExcluder(mpctl_msg_t *) {
        return SUCCESS;
    }
};
#endif /*__MPCTL_UTILS__H_*/
