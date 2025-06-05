/******************************************************************************
  @file    MpctlUtils.cpp
  @brief   Implementation of mpctl utils functions

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-UTIL"
#include <cstdio>
#include <config.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "PerfLog.h"

#include "OptsData.h"
#include "MpctlUtils.h"
#include "SecureOperations.h"
#include <dlfcn.h>

#define SLVP_TIMEOUT_MS 500 /* SLVP callback kicked every 500ms */

static uint16_t core_status[MAX_CORES];
pthread_mutex_t reset_nodes_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t core_offline_cond = PTHREAD_COND_INITIALIZER;
extern uint16_t ncpus;

void write_to_file(FILE *output, const char *input_node, const char *input_val) {
    char buf[NODE_MAX];
    char tmp_s[NODE_MAX];
    uint8_t len = 0;
    int8_t rc = 0;

    if (output == NULL) {
        QLOGE(LOG_TAG, "Invalid default value file passed");
        return;
    }

    memset(buf, 0, sizeof(buf));
    FREAD_STR(input_val, buf, NODE_MAX, rc);
    if (rc >= 0) {
        uint8_t buf_len = strlen(buf);
        if ((buf_len > 0) && (buf[buf_len-1] != '\n')) {
            snprintf(tmp_s, NODE_MAX, "%s;%s\n", input_node, buf);
        } else {
            snprintf(tmp_s, NODE_MAX, "%s;%s", input_node, buf);
        }
        QLOGL(LOG_TAG, QLOG_L2, "snprintf : %s", tmp_s);
        len = strlen(tmp_s);
        fwrite(tmp_s, sizeof(char), len, output);
        return;
    }
    QLOGL(LOG_TAG,  QLOG_WARNING, "Unable to write node value %s to default values file", input_node);
}

int8_t  write_to_node(FILE *read_file, const char *node_str, const char *node) {
    char line[NODE_MAX];
    const char *delimit = ";";
    char *token = NULL;
    char *tmp_token = NULL;
    int8_t rc = 0;

    if (read_file == NULL) {
        QLOGE(LOG_TAG, "Invalid default value filefile passed");
        return rc;
    }

    rewind(read_file);

    while (fgets(line, sizeof(line), read_file) != NULL) {
        if (strstr(line, node_str)) {
            token = strtok_r(line, delimit, &tmp_token);
            token = strtok_r(NULL, delimit, &tmp_token);
            if (token != NULL) {
                FWRITE_STR(node, token, strlen(token), rc);
             }
            return rc;
        }
    }
    QLOGL(LOG_TAG, QLOG_WARNING, "Unable to read node value %s from default values file", node_str);
    return rc;
}

void signal_chk_poll_thread(char *node, int8_t rc) {
  if (rc <= 0) {
      QLOGE(LOG_TAG, "read/write failed for %s, rc=%" PRId8, node, rc);
      pthread_mutex_lock (&reset_nodes_mutex);
      /*Signal the thread to look for cores to come online*/
      pthread_cond_signal(&core_offline_cond);
      /*Unlock the mutex*/
      pthread_mutex_unlock (&reset_nodes_mutex);
      /*we failed to write node, bail out of the loop*/
  }
}

/* Function used to populate an array to keep track of core status
 * Warning: the array will be flushed on each call
 * Returns: the number of cores online or -1 on error
 */
inline int8_t get_online_cpus (uint16_t * status_array, uint16_t array_sz) {
    #define co_node "/sys/devices/system/cpu/online"
    #define CO_NODE_MAX_LEN     32
    uint8_t start = 0;
    uint8_t end = 0;
    uint8_t range = 0;
    uint8_t i = 0;
    uint8_t j = 0;
    int8_t online_count = 0;
    char online[CO_NODE_MAX_LEN] = {0};
    uint8_t len = 0;
    int8_t rc = 0;

    FREAD_STR(co_node, online, CO_NODE_MAX_LEN, rc);

    len = strlen(online);
    if ((len > CO_NODE_MAX_LEN) || (NULL == status_array)) {
        return -1;
    }

    memset(status_array, 0, sizeof(uint16_t)*array_sz);

    for (j = 0; j < len; j++) {
        if (isdigit(online[j])) {
            if (range) {
                end = online[j] - '0';
                for (i = start+1; i < end+1; i++) {
                    status_array[i] = 1;
                    online_count++;
                }
                range = 0;
            } else {
                start = online[j] - '0';
                status_array[start] = 1;
                online_count++;
            }
        }
        else if (online[j] == '-') {
           range = 1;
        }
    }
    return online_count;
}

int16_t get_core_status(int8_t cpu) {
    if (cpu < 0 || cpu >= TargetConfig::getTargetConfig().getTotalNumCores()) {
        QLOGL(LOG_TAG, QLOG_WARNING, "Invalid core %" PRId8, cpu);
        return FAILED;
    }
    return (int16_t)core_status[cpu];
}

//Return first online core of given cluster
//Cluster is specified through the first core
//and last core in the cluster.
int8_t get_online_core(int8_t startcpu, int8_t endcpu) {
    int8_t i;

    get_online_cpus(core_status, (sizeof(core_status) / sizeof(core_status[0])));
    for (i = startcpu; i <= endcpu; i++) {
         if (get_core_status(i) > 0) {
             QLOGL(LOG_TAG, QLOG_L1, "First online core in this cluster is %" PRId8, i);
             return i;
         }
    }
    QLOGE(LOG_TAG, "No online core in this cluster");
    return FAILED;
}

extern void set_slvp_cb(sigval_t);

int8_t vid_create_timer(hint_associated_data *data) {
    int8_t rc = 0;
    struct sigevent sigEvent;

    if (NULL == data) {
        return FAILED;
    }

    sigEvent.sigev_notify = SIGEV_THREAD;
    sigEvent.sigev_notify_function = &set_slvp_cb;
    sigEvent.sigev_value.sival_ptr = data;
    sigEvent.sigev_notify_attributes = NULL;
    rc = timer_create(CLOCK_MONOTONIC, &sigEvent, &data->tTimerId);
    if (rc != 0) {
        QLOGE(LOG_TAG, "Failed to create timer");
        return FAILED;
    } else {
        data->timer_created = 1;
        /* first time arm slvp timer */
        rearm_slvp_timer(data);
    }

    return rc;
}

/* Rearms the SLVP timer
 * SLVP - Single Layer Video Playback
 */
void rearm_slvp_timer(hint_associated_data *data) {
    if (NULL == data) {
        return;
    }

    data->mInterval.it_value.tv_sec = SLVP_TIMEOUT_MS / 1000;
    data->mInterval.it_value.tv_nsec = SLVP_TIMEOUT_MS * 1000000;
    data->mInterval.it_interval.tv_sec = 0;
    data->mInterval.it_interval.tv_nsec = 0;
    //arming or disarming can only be possible if a timer exists
    if (data->timer_created) {
        timer_settime(data->tTimerId, 0, &data->mInterval, NULL);
    }
}

/* Cancels SLVP timer*/
void cancel_slvp_timer(hint_associated_data *data) {
    if (NULL == data) {
        return;
    }

    data->mInterval.it_value.tv_sec = 0;
    data->mInterval.it_value.tv_nsec = 0;
    data->mInterval.it_interval.tv_sec = 0;
    data->mInterval.it_interval.tv_nsec = 0;
    //arming or disarming can only be possible if a timer exists
    if (data->timer_created) {
        timer_settime(data->tTimerId, 0, &data->mInterval, NULL);
    }
}

void send_irq_balance(int32_t *msg, int32_t **ret) {
    int32_t balance_fd;

    struct sockaddr_un balance_addr;
    socklen_t len = sizeof(struct sockaddr_un);

    balance_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (balance_fd == -1) {
        QLOGE(LOG_TAG, "Can't make irqbalance socket (%s)", strerror(errno));
        return;
    }

    memset(&balance_addr, 0, sizeof(balance_addr));
    balance_addr.sun_family = AF_UNIX;
    strlcpy(balance_addr.sun_path, "/dev/socket/msm_irqbalance",
            sizeof(balance_addr.sun_path));

    if (connect(balance_fd, (struct sockaddr *)&balance_addr, len) == -1) {
        QLOGE(LOG_TAG, "bad connect (%s)", strerror(errno));
        goto out;
    }

    if (send(balance_fd, msg, ncpus * sizeof(msg[0]), 0) == -1) {
        QLOGE(LOG_TAG, "bad send (%s)", strerror(errno));
        goto out;
    }

    if (ret && recv(balance_fd, *ret, ncpus * sizeof(msg[0]), 0) == -1) {
        QLOGE(LOG_TAG, "bad recv (%s)", strerror(errno));
        goto out;
    }

out:
    close(balance_fd);
}

int8_t update_freq_node(int32_t start, int32_t end, uint32_t val, char *fmtStr, uint32_t len) {
    int32_t i = 0;
    uint32_t ret = 0;
    char *tmp_s;

    if (fmtStr == NULL) {
        QLOGE(LOG_TAG, "Format string is pointing to null");
        return FAILED;
    }
    memset(fmtStr, 0, len * sizeof(char));
    tmp_s = fmtStr;

    if (start >= 0 && end >= 0) {
        for (i = start; i <= end && len > 0; i++) {

             if (i < end) {
                 ret = snprintf(tmp_s, len, "%" PRId32 ":%" PRIu32 " ", i, val);
             } else {
                 ret = snprintf(tmp_s, len, "%" PRId32 ":%" PRIu32, i, val);
             }
             QLOGL(LOG_TAG, QLOG_L3, "snprintf : %s", tmp_s);
             tmp_s += ret;
             len = SecInt::Subtract(len, ret);
        }
        return SUCCESS;
    }
    return FAILED;
}

/* Updates the node_path with the node_strg value only after ensuring
paths are not null and length to write is with in limit ranges.*/
int8_t change_node_val(const char *node_path, const char *node_strg, uint32_t node_strg_l) {
    int8_t rc = FAILED;
    if ((node_path == NULL) || (strlen(node_path) == 0)) {
        return FAILED;
    }

    if ((node_strg == NULL) || (node_strg_l <= 0) || (node_strg_l > NODE_MAX)) {
        return FAILED;
    }

    FWRITE_STR(node_path, node_strg, node_strg_l, rc);
    if (rc > 0) {
        QLOGL(LOG_TAG, QLOG_L2, "Updated %s with %s, return value %" PRId8, node_path, node_strg, rc);
    } else {
        QLOGE(LOG_TAG, "Failed to update node %s with value %s\n", node_path, node_strg);
    }
    return rc;
}

int8_t change_node_val_buf(const char *node_path, const char *node_strg, uint32_t node_strg_l) {
    int8_t rc = FAILED;
    if ((node_path == NULL) || (strlen(node_path) == 0)) {
        return FAILED;
    }

    if ((node_strg == NULL) || (node_strg_l <= 0) || (node_strg_l > NODE_MAX)) {
        return FAILED;
    }

    FWRITE_BUF_STR(node_path, node_strg, node_strg_l, rc);
    if (rc > 0) {
        QLOGL(LOG_TAG, QLOG_L2, "Updated %s with %s, return value %" PRId8, node_path, node_strg, rc);
    } else {
        QLOGE(LOG_TAG, "Failed to update node %s with value %s\n", node_path, node_strg);
    }
    return rc;
}

/* Saves the current value present in path into the value string and returns
the length of the string on success. On failure returns -1.*/
int32_t save_node_val(const char* path, char* value ) {
    int32_t rc = FAILED;
    uint32_t storage_len = 0;
    char *bptr = NULL;

    if ((path == NULL) || (strlen(path) == 0)) {
        return FAILED;
    }
    FREAD_STR(path, value, NODE_MAX, rc);
    if (rc >= 0) {
        /* Ensure the string we are storing has no trailing whitespaces or newlines */
        bptr = value + strlen(value);
        while(bptr != value && (*(bptr - 1) == '\n' || *(bptr - 1) == ' ')) {
            --bptr;
        }
        *bptr = '\0';

        QLOGL(LOG_TAG, QLOG_L2, "%s read with %s return value %" PRId32, path, value, rc);
        storage_len = strlen(value);
        return storage_len;
    } else {
        QLOGE(LOG_TAG, "Failed to read %s", path);
        return FAILED;
    }
}

char* get_devfreq_new_val(uint32_t reqval, uint32_t *list, uint32_t size, char* maxfreq_path) {
    int32_t setval = FAILED;
    char maxfreq_val[NODE_MAX];
    char tmp_s[NODE_MAX];
    uint32_t i;
    int32_t rc = FAILED;
    char* retval = NULL;

    maxfreq_val[0] = 0; //initialize empty array

    if (size < 0 || size > NODE_MAX || maxfreq_path == NULL)
        return NULL;

    for (i = 0; i < size; i++) {
        if (*(list + i) >= reqval) {
            QLOGL(LOG_TAG, QLOG_L2, "Available freq value is %" PRIu32,*(list + i));
            setval = *(list + i);
            break;
        }
    }

    if (i == size) {
        setval = *(list + i - 1);
    }

    if (setval == FAILED) {
        QLOGE(LOG_TAG, "Error! Perflock failed, invalid freq value %" PRId32, setval);
        return NULL;
    }

    snprintf(tmp_s, NODE_MAX, "%" PRId32, setval);
    QLOGL(LOG_TAG, QLOG_L2, "snprintf : %s", tmp_s);
    /* read max_freq  */
    FREAD_STR(maxfreq_path, maxfreq_val, NODE_MAX, rc);

    if (rc < 0 || strlen(maxfreq_val) == 0) //uninitialized maxfreq_val
        return NULL;

    /* min_freq cannot be more than intended value of
     * max_freq, so min_freq maintained same as max_freq
     */
    if (atoi(tmp_s) > atoi(maxfreq_val)) {
        retval = (char*)malloc((strlen(maxfreq_val) + 1) * sizeof(char)); //extra byte for null char
        if (retval != NULL) {
            snprintf(retval, (strlen(maxfreq_val) + 1), "%" PRId32, atoi(maxfreq_val));
        }
    }
    else {
        retval = (char*)malloc((strlen(tmp_s) + 1) * sizeof(char)); //extra byte for null char
        if (retval != NULL) {
            snprintf(retval, (strlen(tmp_s) + 1), "%" PRId32, atoi(tmp_s));
        }
    }
    if (retval != NULL) {
        QLOGL(LOG_TAG, QLOG_L2, "perf_lock: value to be acq %s", retval);
        return retval;
    }
    else
        return NULL;
}

bool minBoundCheck(char* max, char* min, char* reqval, uint32_t max_available_freq) {

    if (!max || !min || !reqval) {
        QLOGE(LOG_TAG, "Null buffers in minBoundCheck");
        return false;
    }

    uint32_t maxfreq = (uint32_t)atoi(max);
    uint32_t minfreq = (uint32_t)atoi(min);
    uint32_t reqfreq = (uint32_t)atoi(reqval);

    if (reqfreq >= minfreq && max_available_freq >= (uint)reqfreq && reqfreq != maxfreq) {
        return true;
    } else { /*deny request if reqval is lower than minfreq*/
        if(reqfreq == maxfreq) {
            QLOGE(LOG_TAG, "Requested value is same as current value");
        }
        return false;
    }
}

bool maxBoundCheck(char* max, char* min, char* reqval) {

    if (!max || !min || !reqval) {
        QLOGE(LOG_TAG, "Null buffers in maxBoundCheck");
        return false;
    }

    uint32_t maxfreq = (uint32_t)atoi(max);
    uint32_t minfreq = (uint32_t)atoi(min);
    uint32_t reqfreq = (uint32_t)atoi(reqval);

    if (reqfreq <= maxfreq) {
        if (minfreq < reqfreq)
            return true;
        else
            return false;
    } else { /*deny request if reqval is higher than maxfreq*/
        return false;
    }
}

int32_t PerfCtxHelper::Init() {
    int32_t rc = SUCCESS;
    errno = 0;
    writePerfHalPID();
    if (mTestlibhandle != NULL) {
        return rc;
    }
    mTestlibhandle = dlopen(TEST_LIB_NAME, RTLD_NOW | RTLD_LOCAL);
    if (!mTestlibhandle) {
        QLOGE(LOG_TAG, "Unable to open %s: %s\n", TEST_LIB_NAME,
                dlerror());
        rc = FAILED;
    } else {
        errno = 0;
        *(void **) (&sys_nodes_test) = dlsym(mTestlibhandle, "sys_nodes_test");
        char *errc = dlerror();
        if (errc != NULL) {
            QLOGE(LOG_TAG, "NRP: Unable to get sys_nodes_test function handle, %s", errc);
            dlclose(mTestlibhandle);
            mTestlibhandle = NULL;
            rc = FAILED;
        }
    }
    return rc;
}

int32_t PerfCtxHelper::writePerfHalPID() {
    int32_t rc = FAILED;
    int32_t pid = getpid();//perf-hal pid
    char buff[NODE_MAX] = {0};
    memset(buff, 0, sizeof(buff));
    snprintf(buff, NODE_MAX, "%" PRId32, pid);
    FWRITE_BUF_STR(PERF_PID_NODE, buff, NODE_MAX, rc);
    return rc;
}

int32_t PerfCtxHelper::Reset() {
    if (mTestlibhandle != NULL) {
        dlclose(mTestlibhandle);
        mTestlibhandle = NULL;
    }
    sys_nodes_test = NULL;
    return SUCCESS;
}

int32_t PerfCtxHelper::Run(mpctl_msg_t *pMsg) {
    int32_t rc = FAILED;
    if (pMsg == NULL || VENDOR_HINT_TEST != pMsg->hint_id) {
        return rc;
    }

    switch (pMsg->hint_type) {
        case PERF_AVC_TEST_TYPE:
            rc = runAvcTest(pMsg);
            break;

        default:
            QLOGE(LOG_TAG, "Invalid PerfContext test type %" PRId32, pMsg->hint_type);

    }
    return rc;
}

int32_t PerfCtxHelper::runAvcTest(mpctl_msg_t *pMsg) {
    int32_t rc = FAILED;
    if (sys_nodes_test == NULL || pMsg == NULL) {
        QLOGE(LOG_TAG, "Failed to call sys_nodes_test");
    } else {
        rc = sys_nodes_test(pMsg->usrdata_str);
    }
    return rc;
}
