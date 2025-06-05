/******************************************************************************
  @file    PerfController.cpp
  @brief   Implementation of performance server module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015,2017,2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG           "ANDR-PERF-MPCTL"
#include "PerfLog.h"
#include <config.h>
#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include "PerfController.h"

#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "MpctlUtils.h"
#include <poll.h>
#include <cutils/properties.h>
#include <cutils/trace.h>
#include <utils/ThreadDefs.h>
#include <dlfcn.h>
#include <PerfThreadPool.h>


#include "Request.h"
#include "ActiveRequestList.h"
#include "ResourceQueues.h"
#include "MpctlUtils.h"
#include "ResourceInfo.h"
#include "EventQueue.h"
#include "client.h"
#include "PerfGlueLayer.h"
#include "HintExtHandler.h"
#include "TargetConfig.h"
#include "BoostConfigReader.h"
#include "OptsHandlerExtn.h"

#define ON                  1
#define POLL_TIMER_MS       60000
#define MAX_REQS_TO_RESTART_COUNTER 2147483648
#define MAX_PROC_PATH 15
#define MPCTL_SERVER_NAME "PERFD-SERVER"
#define MPCTL_CORE_CHECK_NAME "PERFD-CORECHECK"
#define MAX_OFFLOAD_TIMEOUT 500 //duration in ms
#define TIMEOUT 10 //10msec dutration for timed mutex

#define MAX_RESOURCE_LEN 11 /*Resource ID lenght and value lenght. Eg:0x40C00000 + 1 byte for space*/

int32_t set_platform_boost();
Request * internal_perf_lock_acq_verification(int32_t &handle, uint32_t duration, int32_t list[],
                                              uint32_t num_args, pid_t client_pid, pid_t client_tid,
                                              bool renew, bool isHint, enum client_t client = REGULAR_CLIENT);
bool isDisplayOnReq(Request *req);
bool isDisplayRelatedResourceinReq(Request *req, bool isHint);

int32_t internal_perf_lock_rel(int32_t handle);
int32_t internal_perf_lock_acq_apply(int32_t handle, Request *req);
static void *chk_core_online_poll (void *data);

void handleDisplayOnOffReq(Request *req, int32_t &handle, int32_t &display_hint_handle,
                            int32_t &display_doze_hint_handle, int32_t &retHandle,
                            int32_t &handled);
void handleDisplayDozeReq(Request *req, int32_t &handle, int32_t &display_hint_handle,
                            int32_t &display_doze_hint_handle, int32_t &retHandle,
                            int32_t &handled);

static ActiveRequestList request_list;
static ResourceQueue resource_qs;
static pthread_t mpctl_server_thread;
static mpctl_msg_t msg;
static timer_t poll_timer;
static bool poll_timer_created = false;
static bool exit_mpctl_server = false;
static timed_mutex perf_veri_timeout_mutex;
static pthread_mutex_t subm_req_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t shared_q_mutex = PTHREAD_MUTEX_INITIALIZER;
static EventQueue evqueue;
static pthread_t chk_core_online_thrd;
extern pthread_mutex_t reset_nodes_mutex;
extern pthread_cond_t core_offline_cond;

#define LOAD_LIB "libqti-perfd.so"

using namespace android;
void readSfHwcPid() {
    FILE *pFile;
    OptsData &d = OptsData::getInstance();
    pFile = fopen(SF_PID_FILE, "r");
    if (pFile != NULL) {
        fscanf(pFile,"%" PRId32, &d.sfPid);
        fclose(pFile);
        QLOGL(LOG_TAG, QLOG_L1, "SurfaceFlinger pid %" PRId32, d.sfPid);
    }

    pFile = fopen(HWC_PID_FILE, "r");
    if (pFile != NULL) {
        fscanf(pFile,"%" PRId32, &d.hwcPid);
        fclose(pFile);
        QLOGL(LOG_TAG, QLOG_L1, "Hardware Composer pid %" PRId32, d.hwcPid);
    }

    pFile = fopen(RE_TID_FILE, "r");
    if (pFile != NULL) {
        fscanf(pFile,"%" PRId32, &d.reTid);
        fclose(pFile);
        QLOGL(LOG_TAG, QLOG_L1, "RenderEngine tid %" PRId32, d.reTid);
    }

    return;
}

float getCurrentFps() {
    return get_display_fps();
}

uint16_t ncpus = 0;
static void initialiseNcpus(){
    ncpus = sysconf(_SC_NPROCESSORS_CONF);
    if (ncpus > 4096)
        ncpus = 0;
}

/* SLVP callback function - gets called every time
* the timer gets rearmed, only function that
* acquires and release SLVP hint
*/
void set_slvp_cb(sigval_t tdata) {
    struct itimerspec mInterval_out;
    /* perlock hint parameters
    * 2704 - Sets Above Hispeed Dealy of 40ms
    * 2B5F - Sets Go Hispeed Dealy of 95
    * 2C07 - Sets Hispeed Freq of 768 MHz
    * 2F5A - Sets Target Loads of 90
    * 3204 - Above Hispeed Delay of 40ms for sLVT
    * 365F - Go Hispeed Load of 95 for sLVT
    * 3707 - Hispeed Freq of 729 MHz for sLVT
    * 3A5A - Target Load of 90 for sLVT
    */
    int arg[] = {0x2704, 0x2B5F, 0x2C07, 0x2F5A, 0x3204, 0x365F, 0x3707, 0x3A5A};
    static int vid_handle;
    hint_associated_data *data = NULL;

    if (tdata.sival_ptr) {
        data = (hint_associated_data *) tdata.sival_ptr;
    }

    if (NULL == data) {
        return;
    }

    /* Condition1 - Video is stopped, release handle and cancel timer */
    if (data->vid_hint_state < 1) {
        QLOGL(LOG_TAG, QLOG_L3, "Video stopped, release hint, timer cancel");

        data->slvp_perflock_set = 0;
        /* release handle */
        perf_lock_rel(vid_handle);
        cancel_slvp_timer(data);
    }
    /* Condition2 - Video is still on but multiple display layers
    * Release SLVP hint, but rearm timer
    */
    else if (data->slvp_perflock_set == 1 && data->disp_single_layer != 1) {
        QLOGL(LOG_TAG, QLOG_L3, "Disp many layers, release hint, rearm timer");
        data->slvp_perflock_set = 0;
        /* release handle */
        perf_lock_rel(vid_handle);

        rearm_slvp_timer(data);
    }
    /* Condition3 - Single instance of video and
    * Single layer Display Hint sent
    * Acquire Lock and Cancel Timer
    */
    else if (!data->slvp_perflock_set && data->vid_hint_state == 1 && data->disp_single_layer == 1 &&
        data->vid_enc_start < 1) {
        QLOGL(LOG_TAG, QLOG_L3, "SLVP success, timer cancel");

        data->slvp_perflock_set = 1;
        /* acquire lock */
        vid_handle = perf_lock_acq(0, 0, arg, sizeof(arg)/sizeof(arg[0]));

        cancel_slvp_timer(data);
    }
    /* Condition4 - Video encode and decode both
    * are started, so WFD use case, release handle
    * and cancel timer.
    */
    else if (data->slvp_perflock_set == 1 && data->vid_enc_start == 1) {
        QLOGL(LOG_TAG, QLOG_L3, "Video encode and decode - WFD");

        data->slvp_perflock_set = 0;

        /* release handle */
        perf_lock_rel(vid_handle);

        cancel_slvp_timer(data);
    }
    /* For every other condition, rearm timer */
    else {
        rearm_slvp_timer(data);
    }
    return;
}



/*=========================================================================================================================*/
/* PerfLock related functions                                                                                              */
/*=========================================================================================================================*/
void reset()
{
    QLOGV(LOG_TAG, "Initiating PerfLock reset");
    OptsData &d = OptsData::getInstance();
    ResetHandler::getInstance().reset_to_default_values(d);
    request_list.Reset();
    resource_qs.Reset();
    QLOGV(LOG_TAG, "PerfLock reset completed");
}

static void arm_poll_timer(uint8_t arm)
{
    int8_t rc = -1;
    uint32_t uTime = 0;

    if (arm) {

       uTime = POLL_TIMER_MS;
    }

    struct itimerspec mTimeSpec;
    mTimeSpec.it_value.tv_sec = uTime / TIME_MSEC_IN_SEC;
    mTimeSpec.it_value.tv_nsec = (uTime % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
    mTimeSpec.it_interval.tv_sec = 0;
    mTimeSpec.it_interval.tv_nsec = 0;

    if (poll_timer_created) {
        rc = timer_settime(poll_timer, 0, &mTimeSpec, NULL);
        if (rc != 0) {
            QLOGE(LOG_TAG, "Failed to arm poll timer");
        }
    }
}

/* Timer callback function for polling clients */
static void poll_cb(sigval_t)
{
    perf_lock_cmd(MPCTL_CMD_PERFLOCKPOLL);
    arm_poll_timer(1);
}

/* Poll to see if clients of PerfLock are still alive
 * If client is no longer active, release its lock
 */
static void poll_client_status() {
    uint8_t size = 0;
    int8_t rc = 0;
    int32_t handles[ActiveRequestList::ACTIVE_REQS_MAX];
    uint32_t pids[ActiveRequestList::ACTIVE_REQS_MAX];
    char buf[MAX_PROC_PATH];

    size = request_list.GetActiveReqsInfo(handles, pids);
    QLOGL(LOG_TAG, QLOG_L3, "Total ACTIVE_REGS: %" PRIu8, size);

    for (uint8_t i = 0; i < size; i++) {
        QLOGL(LOG_TAG, QLOG_L3, "ACTIVE_REQS process pid: %" PRIu32, pids[i]);
        snprintf(buf, MAX_PROC_PATH , "proc/%" PRIu32, pids[i]);
        rc = access(buf, F_OK);
        //access() returns 0 on success(path exists) and -1 on failure(path does not exists)
        if (rc == -1) {
            QLOGL(LOG_TAG, QLOG_L3, "Client %" PRIu32 " does not exist, remove lock (handle=%" PRId32 ") associated with client",
                  pids[i], handles[i]);
            perf_lock_rel(handles[i]);
        }
    }
}

bool isDisplayOnReq(Request *req) {
    bool rc = false;
    bool disp_hint_req_typ_on = false;
    ResourceInfo *disp_hint_resource = NULL;
    if (req != NULL && req->GetPriority() == HIGH_PRIORITY) {

        disp_hint_resource = req->GetResource(0);

        disp_hint_req_typ_on = disp_hint_resource &&
            (disp_hint_resource->GetMajor() == DISPLAY_OFF_MAJOR_OPCODE) &&
            disp_hint_resource->GetValue();

        //Display on, value Passed is 1
        if (disp_hint_req_typ_on) {
            rc = true;
        }
    }
    return rc;
}

/*This function return true when these is a display resource in perflock request,
  and this request is not made from hint_api.*/
bool isDisplayRelatedResourceinReq(Request *req, bool isHint) {
    bool rc = false;
    if (isHint == true) {
        return rc;
    }
    ResourceInfo *request_resource = NULL;
    if (req == NULL) {
        return rc;
    }
    request_resource = req->GetResource(0);
    if (request_resource) {
        switch(request_resource->GetQIndex()) {
            case MISC_START_INDEX + DISPLAY_DOZE_OPCODE:
                {
                    QLOGE(LOG_TAG, "Invalid Request with Display Doze resource Major: 0x%" PRIX32 " Minor: 0x%" PRIX32, request_resource->GetMajor(), request_resource->GetMinor());
                    rc = true;
                    break;
                }
            case DISPLAY_START_INDEX + DISPLAY_OFF_OPCODE:
                {
                    QLOGE(LOG_TAG, "Invalid Request with Display off resource Major: 0x%" PRIX32 " Minor: 0x%" PRIX32, request_resource->GetMajor(), request_resource->GetMinor());
                    rc = true;
                }
                break;
            default:
                break;
        }
    }
    return rc;
}

Request* internal_perf_lock_acq_verification(int32_t &handle, uint32_t duration, int32_t list[], uint32_t num_args,
                                              pid_t client_pid, pid_t client_tid, bool renew, bool isHint, enum client_t client) {
    char trace_buf[TRACE_BUF_SZ] = {'\0'};
    char trace_args_buf[TRACE_BUF_SZ] = "list=";
    uint32_t arg = 0;
    int32_t tmpHandle = -1;
    static uint32_t handle_counter = 0;
    int32_t rc = 0;
    Request *req = NULL, *exists = NULL;
    struct q_node *current_resource = NULL, *pended = NULL;
    struct q_node *recent =NULL, *iter = NULL;
    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool();
    bool ret = false;
    bool resized = false;
    bool reqdispon = false;
    uint32_t offset = 0;
    char *trace_buffer = NULL;

    //args sanity
    //1.num_args specified but no list
    //2.num_args not specified but is not a high profile client
    if ((num_args && !list) || (num_args <= 0)) {
        QLOGE(LOG_TAG, "Num args is not proper, No optimizations performed");
        return NULL;
    }

    //1. Check if we can submit request, active request limit check.
    //   this check is not required for renewhints and locks.
    if (renew == false && request_list.CanSubmit() == false) {
        QLOGE(LOG_TAG, "Perflock Submit request limit reached.");
        request_list.Dump();
        return NULL;
    }

    if (getPerfLogLevel() >= QLOG_L1) {
        offset = snprintf(trace_buf, TRACE_BUF_SZ, "perf_lock_acq: client_pid=%d, client_tid=%d, inupt handle=%" PRId32 ", duration=%" PRIu32 " ms, num_args=%" PRIu32 ", %s",\
                  client_pid, client_tid, handle, duration, num_args, trace_args_buf);

        /* Check if there is still room for Resource values in static trace_buf */
        if ((TRACE_BUF_SZ - offset) > (num_args * MAX_RESOURCE_LEN)) {
            uint32_t n = 0;

            for (arg = 0; arg < num_args; arg++) {
                n = snprintf(trace_buf + offset, TRACE_BUF_SZ, "0x%0X ", list[arg]);
                offset = offset + n;
            }
            QLOGL(LOG_TAG, QLOG_L1, "%s", trace_buf);
            if (PERF_SYSTRACE) {
                ATRACE_BEGIN(trace_buf);
            }
        } else {
            uint32_t n = 0;
            uint32_t buffer_len = (num_args * MAX_RESOURCE_LEN) + offset;

            trace_buffer = (char*)malloc(buffer_len);
            if (trace_buffer != NULL) {
                strlcpy(trace_buffer, trace_buf, offset+1);
                for (arg = 0; arg < num_args; arg++) {
                    n = snprintf(trace_buffer + offset, buffer_len, "0x%0X ", list[arg]);
                    offset = offset + n;
                }
                QLOGL(LOG_TAG, QLOG_L1, "%s", trace_buffer);
                if (PERF_SYSTRACE) {
                    ATRACE_BEGIN(trace_buffer);
                }
                free(trace_buffer);
            } else {
                QLOGE(LOG_TAG, "malloc failed for trace_buffer.");
                if (PERF_SYSTRACE) {
                    ATRACE_BEGIN("malloc failed");
                }
            }
        }
    }

    //2. create a new request
    req = new(std::nothrow) Request(duration, client_pid, client_tid, client);
    if (NULL == req) {
        QLOGE(LOG_TAG, "Failed to create new request, no optimizations performed");
        if (PERF_SYSTRACE) {
            ATRACE_END();
        }
        return NULL;
    }

    //3.process and validate the request
    if (!req->Process(num_args, list)) {
        delete req;
        req = NULL;
        QLOGE(LOG_TAG, "invalid request, no optimizations performed");
        if (PERF_SYSTRACE) {
            ATRACE_END();
        }
        return NULL;
    }

    if (req && isDisplayRelatedResourceinReq(req, isHint)) {
        QLOGE(LOG_TAG, "Invalid request with display resource");
        delete req;
        req = NULL;
        return req;
    }

    //4.avoid duplicate requests and renew older request time.
    exists = request_list.IsActive(handle, req);
    if (exists != NULL) {
        QLOGL(LOG_TAG, QLOG_L2, "Request exists reset the timer");
        /* if the request exists and is not of an indefinite_duration
           simply reset its timer and return handle to client */
        request_list.ResetRequestTimer(handle, exists);
        if (PERF_SYSTRACE) {
            ATRACE_END();
        }

        //As this request already exist in list, remove it
        delete req;
        req = NULL;
        return req;
    }
    reqdispon = isDisplayOnReq(req);

    //5. Check if we can submit request, active request limit check
    //   this check is required for renewhints and locks if older request has timed out.
    //   this request will be treated as new request then.
    if (renew == true && request_list.CanSubmit() == false && reqdispon == false) {
        QLOGE(LOG_TAG, "Perflock Submit request limit reached.");
        request_list.Dump();
        delete req;
        req = NULL;
        if (PERF_SYSTRACE) {
            ATRACE_END();
        }
        return req;
    }

    bool delReq = false;
    try {
        unique_lock<timed_mutex> lck(perf_veri_timeout_mutex, defer_lock);
        bool lck_acquired = lck.try_lock_for(std::chrono::milliseconds(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE(LOG_TAG, "failed to get mutex lock for handle creation");
            delReq = true;
        } else {
            handle_counter = (handle_counter + 1) % MAX_REQS_TO_RESTART_COUNTER;
            if (handle_counter == 0)
                handle_counter++;
            tmpHandle = handle_counter;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
        delReq = true;
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
        delReq = true;
    }
    if (delReq) {
        delete req;
        req = NULL;
        if (PERF_SYSTRACE) {
            ATRACE_END();
        }
        return req;
    }

    handle = tmpHandle;
    if (PERF_SYSTRACE) {
        ATRACE_END();
    }

    snprintf(trace_buf, TRACE_BUF_SZ, "perf_lock_acq: output handle=%" PRId32, tmpHandle);
    QLOGL(LOG_TAG, QLOG_L1, "%s", trace_buf);

    if (PERF_SYSTRACE) {
        ATRACE_BEGIN(trace_buf);
        ATRACE_END();
    }
    if (PERF_SYSTRACE) {
        ATRACE_END();
    }
    return req;
}

void handleDisplayOnOffReq(Request *req, int32_t &handle, int32_t &display_hint_handle,
                            int32_t &display_doze_hint_handle, int32_t &retHandle,
                            int32_t &handled) {
    ResourceInfo *disp_hint_resource = NULL;
    bool disp_hint_req_typ_on = false;

    if (req == NULL) {
        retHandle = FAILED;
        handled = 1;
        return;
    }
    disp_hint_resource = req->GetResource(0);
    if (disp_hint_resource != NULL) {
        disp_hint_req_typ_on = (disp_hint_resource->GetMajor() == DISPLAY_OFF_MAJOR_OPCODE) &&
            disp_hint_resource->GetValue();

        QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow handleDisplayOnOffReq disp_hint_resource major: \
                        %" PRIu16 " value: %" PRId32 " display_hint_handle: %" PRId32,
                        disp_hint_resource->GetMajor(),
                        disp_hint_resource->GetValue(), display_hint_handle);
    }
    //Display on, value Passed is 1
    if (disp_hint_req_typ_on) {
        QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow Handling Display ON");
        arm_poll_timer(1); //enable poll timer when display is on
        if (display_hint_handle) {
            internal_perf_lock_rel(display_hint_handle);
            QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow DisplayOFF->DisplayON Handle \
                    %" PRId32 " released", display_hint_handle);
            handle = display_hint_handle;
            display_hint_handle = 0;
            retHandle = handle;
            handled = 1;
        } else if(display_doze_hint_handle) {
            // Release doze handle and unlock pend locks
            QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow DisplayDoze->DisplayON Handle \
                    %" PRId32 " released", display_doze_hint_handle);
            internal_perf_lock_rel(display_doze_hint_handle);
            handle = display_doze_hint_handle;
            display_doze_hint_handle = 0;
            retHandle = handle;
            handled = 1;
        } else {
            QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow DisplayON->DisplayON. No Action Needed.");
            retHandle = handle;
            handled = 1;
        }
        if(req != NULL && handled) {
            delete req;
            req = NULL;
        }
    } else { //Display off, if value Passed is 0
        QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow Handling Display OFF");
        arm_poll_timer(0); //disable poll timer when display is off
        //If Display off request exists, return previous handle
        QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow:disp_hint_resource released handle %" PRId32, display_hint_handle);
        if (display_hint_handle) {
            QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow DisplayOFF->DisplayOFF. \
                    Return Previous Handle. Removing handle %" PRId32, handle);
            request_list.Remove(handle);
            req = NULL;
            handle = retHandle = display_hint_handle;
            handled = 1;
        } else if(display_doze_hint_handle) {
            // Release doze handle and unlock pend locks
            QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow DisplayDoze->DisplayOFF \
                    Handle %" PRId32 " released", display_doze_hint_handle);
            internal_perf_lock_rel(display_doze_hint_handle);
            display_doze_hint_handle = 0;
            QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow DisplayDoze->DisplayOFF release done.");
            handled = 0;
        }
        //store the handle of display off
        display_hint_handle = handle;
    }
}

void handleDisplayDozeReq(Request *req, int32_t &handle, int32_t &display_hint_handle,
                            int32_t &display_doze_hint_handle, int32_t &retHandle,
                            int32_t &handled) {
    QLOGV(LOG_TAG, "DispCallFlow handleDisplayDozeReq");
    if (req == NULL) {
        retHandle = FAILED;
        handled = 1;
        return;
    }
    // Already in Doze. For now dont create new handle.
    if(display_doze_hint_handle) {
        QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow DisplayDoze->DisplayDoze. \
                No Action Needed. Removing handle %" PRId32, handle);
        request_list.Remove(handle);
        req = NULL;
        retHandle = display_doze_hint_handle;
        handled = 1;
    } else {
        // Already in DisplayOFF. Can't Doze now
        if (display_hint_handle) {
            QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow DisplayOFF->DisplayDoze \
                Not allowed. Removing handle %" PRId32, handle);
            request_list.Remove(handle);
            req = NULL;
            retHandle = FAILED;
            handled = 1;
        } else {
            display_doze_hint_handle = handle;
            QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow Entering Display Doze. \
                Handle %" PRId32, display_doze_hint_handle);
            handled = 0;
        }
    }
}

//Main function used when PerfLock acquire is requested
int32_t internal_perf_lock_acq_apply(int32_t handle, Request *req) {
    int32_t rc = FAILED;
    char trace_buf[TRACE_BUF_SZ] = {0};

    static int32_t display_hint_handle = 0;
    static int32_t display_doze_hint_handle = 0;

    snprintf(trace_buf, TRACE_BUF_SZ, "internal_perf_lock_acq_apply: handle=%" PRId32, handle);
    if (PERF_SYSTRACE) {
        QLOGL(LOG_TAG, QLOG_L1, "%s", trace_buf);
        ATRACE_BEGIN(trace_buf);
    }

    // display/doze section
    ResourceInfo *hint_resource = req->GetResource(0);
    if(req->GetPriority() == HIGH_PRIORITY) {
        QLOGL(LOG_TAG, QLOG_L1, "DispCallFlow Received HIGH_PRIORITY request");

        int32_t retHandle = FAILED;
        int32_t handled = 0;
        if(hint_resource != NULL) {
            QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow Before Handling handle: %" PRId32 " display_hint_handle: %" PRId32 " \
                display_doze_hint_handle: %" PRId32 " retHandle: %" PRId32 " handled: %" PRId32, handle,
                display_hint_handle, display_doze_hint_handle, retHandle, handled);
            if(hint_resource->GetQIndex() == DISPLAY_OFF_INDEX) {
                QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow Handling DISPLAY_OFF_INDEX resource");
                handleDisplayOnOffReq(req, handle, display_hint_handle,
                    display_doze_hint_handle, retHandle, handled);
            } else if(hint_resource->GetQIndex() == DISPLAY_DOZE_INDEX) {
                QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow Handling DISPLAY_DOZE_INDEX resource");
                handleDisplayDozeReq(req, handle, display_hint_handle,
                    display_doze_hint_handle, retHandle, handled);
            }
            QLOGL(LOG_TAG, QLOG_L2, "DispCallFlow After Handling handle: %" PRId32 " display_hint_handle: %" PRId32 " \
                display_doze_hint_handle: %" PRId32 " retHandle: %" PRId32 " handled: %" PRId32, handle,
                display_hint_handle, display_doze_hint_handle, retHandle, handled);
        }
        if(handled == 1) {
            if (PERF_SYSTRACE) {
                ATRACE_END();
            }
            return retHandle;
        }
        resource_qs.PendCurrentRequests();
    }

    // Create Timer for this request
    if (req->GetDuration() != INDEFINITE_DURATION) {
        rc = req->CreateTimer(handle);
        QLOGL(LOG_TAG, QLOG_L1, "timer rc for create_timer %" PRId32 " handle=%" PRId32, rc, handle);
        if (rc == FAILED) {
            QLOGE(LOG_TAG, "Failed to create timer, removing optimizations performed Handle%" PRId32,handle);
            request_list.Remove(handle);
            req = NULL;
            return handle;
        }
    } else {
        QLOGL(LOG_TAG, QLOG_L1, "Lock with Indfeinite_duration %" PRIu32 " handle=%" PRId32, req->GetDuration(), handle);
    }

    //apply operation
    //even if we fail to apply all resource locks we still add this request
    //to our db as we had sent the handle to client, once client asks for
    //release, we delete from our db, no need to check for failure of the request
    //at this point of time
    resource_qs.AddAndApply(req);


    if (1 == handle)
        arm_poll_timer(1); //enable poll timer when getting first request.

    // handle display off/doze case
    if (req->GetPriority() == HIGH_PRIORITY) {
        if(hint_resource != NULL) {
            if(hint_resource->GetQIndex() == DISPLAY_OFF_INDEX ||
                    hint_resource->GetQIndex() == DISPLAY_DOZE_INDEX) {
                QLOGL(LOG_TAG, QLOG_L2, "LockCurrentState For display off/doze");
                resource_qs.LockCurrentState(req);
            }
        }
    }

    // add this request to our database

    if (req->GetDuration() != INDEFINITE_DURATION) {
        rc = req->SetTimer();
    } else {
        QLOGL(LOG_TAG, QLOG_L1, "Lock with Indfeinite_duration %" PRIu32, req->GetDuration());
    }

    if (rc == FAILED && req->GetDuration() != INDEFINITE_DURATION) {
            QLOGE(LOG_TAG, "Releasing Handle:%" PRId32 "Settimer Failed", handle);
            perf_lock_rel(handle);
    }

    if (PERF_SYSTRACE) {
        ATRACE_END();
    }

    return handle;
}

/* Main function used when PerfLock release is requested */
int32_t internal_perf_lock_rel(int32_t handle) {
    char trace_buf[TRACE_BUF_SZ];
    Request *req = NULL;
    ResourceInfo r;

    struct q_node *del = NULL;
    struct q_node *pending;
    struct q_node *current_resource;

    snprintf(trace_buf, TRACE_BUF_SZ, "perf_lock_rel: input handle=%" PRId32, handle);
    QLOGL(LOG_TAG, QLOG_L1, "%s", trace_buf);

    if (PERF_SYSTRACE) {
        ATRACE_BEGIN(trace_buf);
    }

    //1.check whether the request exists
    req = request_list.DoesExists(handle);
    if (req == NULL) {
        QLOGL(LOG_TAG, QLOG_WARNING, "Request does not exist, nothing released");
        if (PERF_SYSTRACE) {
            ATRACE_END();
        }
        return FAILED;
    }

    //2.apply operation i.e. release
    resource_qs.RemoveAndApply(req);

    //handle display on case
    if (req->GetPriority() == HIGH_PRIORITY) {
        resource_qs.UnlockCurrentState(req);
    }

    //3.remove from our db
    request_list.Remove(handle);
    req = NULL;

    if (0 == request_list.GetNumActiveRequests())
        arm_poll_timer(0); //disable poll timer when removing last request.

    if (PERF_SYSTRACE) {
        ATRACE_END();
    }
    return SUCCESS;
}

/*=========================================================================================================================*/
static void Init() {
    Target &t = Target::getCurTarget();
    t.InitializeTarget();
    t.setTraceEnabled(PERF_SYSTRACE);
    OptsData::getInstance().Init();
    OptsHandler::getInstance().Init();
    ResetHandler::getInstance().Init();
    resource_qs.Init();
    PerfCtxHelper::getPerfCtxHelper().Init();

}

int32_t set_platform_boost() {
    int pid = getpid();//perf-hal server
    int tid = gettid();//mpctl-server
    int list[] = {
        MPCTLV3_SCHED_LOW_LATENCY, tid,
        MPCTLV3_SCHED_WAKE_UP_IDLE, tid,
        MPCTLV3_SCHED_LOW_LATENCY, pid,
        MPCTLV3_SCHED_WAKE_UP_IDLE, pid
    };
    int32_t handle = perf_lock_acq(0, 0, list, sizeof(list)/sizeof(list[0]));
    QLOGL(LOG_TAG, QLOG_L1, "%s pid:[%" PRIu32 "] tid:[%" PRIu32 "] Handle: %" PRId32,__func__, pid, tid, handle);
    return handle;
}

static void *mpctl_server(void *data) {
    int32_t rc;
    uint8_t cmd;
    struct sigevent sigEvent;
    int32_t platform_boost_handle = -1;
    mpctl_msg_t *msg = NULL;
    Request *req = NULL;
    (void)data;
    int32_t post_boot_status = post_boot_init_completed();
    if (post_boot_status == FAILED) {
        return NULL;
    }
    QLOGL(LOG_TAG, QLOG_L1, "Boot complete, post boot executed");

    initialiseNcpus();
    Init();
    readSfHwcPid();
    getCurrentFps();

    rc = pthread_create(&chk_core_online_thrd, NULL, chk_core_online_poll, NULL);
    if (rc != 0) {
        QLOGE(LOG_TAG, "perflockERR: Unable to create Chk Core Thread\n");
        return NULL;
    } else {
        rc = pthread_setname_np(chk_core_online_thrd, MPCTL_CORE_CHECK_NAME);
        if (rc != 0) {
            QLOGE(LOG_TAG, "Failed to name Core Online Thread:  %s", MPCTL_CORE_CHECK_NAME);
        }
    }

    //sleep of 100 msec for allowing time to ensure the creation of above threads
    usleep(100000);


    OptsData &d = OptsData::getInstance();
    ResetHandler::getInstance().reset_to_default_values(d);

    if (!poll_timer_created) {
        /* create timer to poll client status */
        sigEvent.sigev_notify = SIGEV_THREAD;
        sigEvent.sigev_notify_function = &poll_cb;
        sigEvent.sigev_notify_attributes = NULL;

        rc = timer_create(CLOCK_MONOTONIC, &sigEvent, &poll_timer);
        if (rc != 0) {
            QLOGE(LOG_TAG, "Failed to create timer for client poll");
        } else {
            poll_timer_created = true;
        }
    }
    TargetConfig &tc = TargetConfig::getTargetConfig();
    tc.setInitCompleted(true);

    FpsUpdateAction::getInstance().SetFps();
    platform_boost_handle = set_platform_boost();
    setpriority(PRIO_PROCESS, 0, android::PRIORITY_HIGHEST);
    /* Main loop */
    for (;;) {
        //wait for perflock commands
        EventData *evData = evqueue.Wait();

        if (!evData || !evData->mEvData) {
            continue;
        }

        cmd = evData->mEvType;

        if (cmd == MPCTL_CMD_EXIT) {
            exit_mpctl_server = true;
            pthread_cond_signal(&core_offline_cond);
            break;
        }

        msg = (mpctl_msg_t *)evData->mEvData;

        for (uint8_t i=0; i < msg->data && i < MAX_ARGS_PER_REQUEST; i++) {
            QLOGL(LOG_TAG, QLOG_L3, "Main loop pl_args[%" PRIu8 "] = %" PRIx32, i, msg->pl_args[i]);
        }

        QLOGL(LOG_TAG, QLOG_L1, "Received time=%" PRId32 ", data=%" PRIu32 ", hint_id=0x%" PRIx32 ", hint_type=%" PRId32 " handle=%" PRId32,
                msg->pl_time, msg->data, msg->hint_id, msg->hint_type, msg->pl_handle) ;
        QLOGL(LOG_TAG, QLOG_L2, "pid:%d tid:%d ", msg->client_pid, msg->client_tid);

        switch (cmd) {
            case MPCTL_CMD_PERFEVENT:
            case MPCTL_CMD_PERFLOCKHINTACQ:
            case MPCTL_CMD_PERFLOCKACQ:
                req = (Request *)msg->userdata;
                internal_perf_lock_acq_apply(msg->pl_handle, req);
                break;

            case MPCTL_CMD_PERFLOCKREL:
                internal_perf_lock_rel(msg->pl_handle);
                break;

            case MPCTL_CMD_PERFLOCKPOLL:
                poll_client_status();
                break;

            case MPCTL_CMD_PERFLOCKRESET:
                reset();
                break;

            case MPCTL_CMD_PERFLOCK_RESTORE_GOV_PARAMS:
                QLOGE(LOG_TAG, "Calling Generic reset function - reset_to_default_values");
                ResetHandler::getInstance().reset_to_default_values(d);
                break;

            default:
                QLOGE(LOG_TAG, "Unknown command %" PRIu8, cmd);
        }

        //give the event data back to memory pool
        evqueue.GetDataPool().Return(evData);
    }
    if (platform_boost_handle > 0) {
        QLOGL(LOG_TAG, QLOG_L1, "platform boost released for perf-hal-service and %s", MPCTL_SERVER_NAME);
        internal_perf_lock_rel(platform_boost_handle);
    }
    QLOGL(LOG_TAG, QLOG_L2, "%s thread exit due to rc=%" PRId32, MPCTL_SERVER_NAME, rc);
    return NULL;
}

static void *chk_core_online_poll (void *) {
    int32_t rc = 0;
    int8_t one_cpu_per_clstr_online = FAILED;
    struct pollfd fds;
    char buf[NODE_MAX];
    char pollPath[NODE_MAX];
    Target &t = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();
    memset(pollPath, 0, sizeof(pollPath));
    PerfDataStore *store = PerfDataStore::getPerfDataStore();
    store->GetSysNode(POLLPATH, pollPath);

    fds.fd  = open(pollPath, O_RDONLY);
    if (fds.fd < 0) {
        QLOGE(LOG_TAG, "Unable to open %d", fds.fd);
        return 0;
    }
    rc = pread(fds.fd, buf, NODE_MAX, 0);
    fds.events = POLLERR|POLLPRI;
    fds.revents = 0;
    while (true) {
        /*Mutex Lock*/
        pthread_mutex_lock (&reset_nodes_mutex);
        /*Wait on Signal from the update-files or drop-old files when write fails*/
        QLOGE(LOG_TAG, " before pthread_cond_wait %d, %s", __LINE__, __FUNCTION__);
        pthread_cond_wait(&core_offline_cond, &reset_nodes_mutex);
        QLOGE(LOG_TAG, " after pthread_cond_wait %d, %s", __LINE__, __FUNCTION__);
        /*Unlock the mutex and now wait on poll and do the job in thread*/
        pthread_mutex_unlock (&reset_nodes_mutex);

        /*Break from this loop once mpctl server stops*/
        if (exit_mpctl_server)
            break;

        QLOGE(LOG_TAG, " after pthread_mutex_unlock %d, %s", __LINE__, __FUNCTION__);
        do {
            one_cpu_per_clstr_online = FAILED;
            QLOGE(LOG_TAG, "Block on poll()");
            rc = poll(&fds, 1, 3000);
            if (rc < 0) {
                QLOGE(LOG_TAG, "poll() has returned error for %s", pollPath);
            } else if (rc == 0) {
                QLOGE(LOG_TAG, "poll() has timed out for %s", pollPath);
            } else {
                QLOGE(LOG_TAG, "Poll has found event, rc = %" PRId32 ", line = %d, func = %s",
                        rc,  __LINE__, __FUNCTION__);
                rc = pread(fds.fd, buf, NODE_MAX, 0);
                for (int8_t i=0; i < tc.getNumCluster(); i++) {
                      if (!i) {
                          if ((one_cpu_per_clstr_online = get_online_core(0,
                               t.getLastCoreIndex(0))) == FAILED)
                                 break;
                      } else {
                          if ((one_cpu_per_clstr_online = get_online_core(t.getLastCoreIndex(i -1)+1,
                               t.getLastCoreIndex(i))) == FAILED)
                                 break;
                      }
               }
            }
        } while (one_cpu_per_clstr_online == FAILED);

        /*Call reset nodes here and again continue in loop to wait for further
         * signals*/
        pthread_mutex_lock (&reset_nodes_mutex);
        perf_lock_cmd(MPCTL_CMD_PERFLOCK_RESTORE_GOV_PARAMS);
        pthread_mutex_unlock (&reset_nodes_mutex);
    }
    close(fds.fd);
    return 0;
}

//callbacks for eventqueue
static void *Alloccb() {
    void *mem = (void *) new(std::nothrow) mpctl_msg_t;
    if (NULL ==  mem)
        QLOGE(LOG_TAG, "memory not allocated");
    return mem;
}

static void Dealloccb(void *mem) {
    if (NULL != mem) {
        delete (mpctl_msg_t *)mem;
    }
}


////////////////////////////////////////////////////////////////////////
///perf module interface
////////////////////////////////////////////////////////////////////////
static int mpctlEvents[] = {
   VENDOR_PERF_HINT_BEGIN,
   VENDOR_POWER_HINT_END,  //As perfd need to incorporate power hint also
   VENDOR_HINT_DISPLAY_OFF,//Hint event for display off required by Perfd
   VENDOR_HINT_DISPLAY_ON,//Hint event for display on required by Perfd
   VENDOR_HINT_DISPLAY_DOZE,
};

static PerfGlueLayer mpctlglue = {
    LOAD_LIB,
    mpctlEvents,
    sizeof(mpctlEvents)/sizeof(mpctlEvents[0])
};

//interface implementation
int32_t perfmodule_init() {
    int32_t rc = 0;
    char trace_prop[PROPERTY_VALUE_MAX];

    /* Enable systraces by adding vendor.debug.trace.perf=1 into build.prop */
    if (property_get(PROP_NAME, trace_prop, NULL) > 0) {
        if (trace_prop[0] == '1') {
            perf_debug_output = PERF_SYSTRACE = atoi(trace_prop);
        }
    }

    QLOGL(LOG_TAG, QLOG_L1, "MPCTL server starting");
    evqueue.GetDataPool().SetCBs(Alloccb, Dealloccb);
    evqueue.GetDataPool().EnableSecondaryPool();

    rc = pthread_create(&mpctl_server_thread, NULL, mpctl_server, NULL);
    if (rc != 0) {
        QLOGE(LOG_TAG, "perflockERR: Unable to create mpctl server thread");
        return FAILED;
    }
    rc = pthread_setname_np(mpctl_server_thread, MPCTL_SERVER_NAME);
    if (rc != 0) {
        QLOGE(LOG_TAG, "Failed to name mpctl_server:  %s", MPCTL_SERVER_NAME);
    }

    return SUCCESS;
}

void perfmodule_exit() {
    PerfCtxHelper::getPerfCtxHelper().Reset();
    pthread_join(mpctl_server_thread, NULL);
    pthread_cond_destroy(&core_offline_cond);
    pthread_mutex_destroy(&reset_nodes_mutex);
    pthread_mutex_destroy(&subm_req_mutex);
    pthread_mutex_destroy(&shared_q_mutex);
}

bool inline isRelReq(uint8_t cmd) {
    return MPCTL_CMD_PERFLOCKREL == cmd;
}

int32_t perfmodule_submit_request(mpctl_msg_t *msg) {
    uint32_t size = 0;
    Target &t = Target::getCurTarget();
    TargetConfig &tc = TargetConfig::getTargetConfig();
    HintExtHandler &hintExt = HintExtHandler::getInstance();
    int32_t handle = -1;
    EventData *evData;
    int32_t *pl_args = NULL;
    int32_t timeout = 0;
    Request *req = NULL;
    uint8_t cmd = 0;
    int32_t before_handle = -1;
    bool reqExist = false;
    int16_t preActionRetVal = 0;
    bool isHint = false;

    if (NULL == msg) {
        return handle;
    }

    if (tc.getInitCompleted() == false) {
        QLOGE(LOG_TAG, "Request not accepted, as target initialization is not complete.");
        return handle;
    }

    QLOGL(LOG_TAG, QLOG_L1, "Received time=%" PRId32 ", data=%" PRIu32 ", hint_id=0x%" PRIx32 " hint_type=%" PRId32,
              msg->pl_time, msg->data, msg->hint_id, msg->hint_type) ;

    handle = msg->pl_handle;
    cmd = msg->req_type;
    pl_args = msg->pl_args;
    size = msg->data;

    if (isRelReq(cmd) == true) {//check if request is release request.
        evData = evqueue.GetDataPool().Get(SECONDARY);
    } else {
        evData = evqueue.GetDataPool().Get();
    }
    if ((NULL == evData) || (NULL == evData->mEvData)) {
        QLOGE(LOG_TAG, "perf request submission limit reached.");
        if (evData) {
            evqueue.GetDataPool().Return(evData);
        }
        return handle;
    }

    if (msg->req_type == MPCTL_CMD_EXIT) {
        evData->mEvType = MPCTL_CMD_EXIT;
        evqueue.Wakeup(evData);
        return 0;
    }

    pthread_mutex_lock(&subm_req_mutex);
    mpctl_msg_t *pMsg = (mpctl_msg_t *)evData->mEvData;
    memset(pMsg, 0x00, sizeof(mpctl_msg_t));
    pMsg->data = msg->data;
    pMsg->pl_handle = msg->pl_handle;
    pMsg->req_type = msg->req_type;
    pMsg->pl_time = msg->pl_time;
    pMsg->client_pid = msg->client_pid;
    pMsg->client_tid = msg->client_tid;
    pMsg->hint_id = msg->hint_id;
    pMsg->hint_type = msg->hint_type;
    pMsg->renewFlag = msg->renewFlag;
    pMsg->offloadFlag = msg->offloadFlag;
    pMsg->app_workload_type = msg->app_workload_type;
    pMsg->app_pid = msg->app_pid;
    strlcpy(pMsg->usrdata_str, msg->usrdata_str, MAX_MSG_APP_NAME_LEN);
    for (uint16_t i=0; (i < msg->data) && (msg->data < MAX_ARGS_PER_REQUEST); i++) {
        pMsg->pl_args[i] = msg->pl_args[i];
    }
    if (hintExt.FetchHintExcluder(pMsg) == SUCCESS) {
        QLOGL(LOG_TAG, QLOG_L1, "Hint has been excluded hint_id=0x%" PRIX32 ", hint_type=%" PRId32 " pkg %s\n",
                pMsg->hint_id, pMsg->hint_type, pMsg->usrdata_str);
        int32_t rc = FAILED;
        rc = PerfCtxHelper::getPerfCtxHelper().Run(pMsg);
        pthread_mutex_unlock(&subm_req_mutex);
        if (evData != NULL) {
            evqueue.GetDataPool().Return(evData);
            evData = NULL;
        }
        return rc;
    }
    switch (cmd) {
        case MPCTL_CMD_PERFEVENT:
        case MPCTL_CMD_PERFLOCKHINTACQ:
            //Check if this hint to be ignored, while the context aware hint is active.
            if (hintExt.checkToIgnore(pMsg->hint_id)) {
                QLOGL(LOG_TAG, QLOG_L1, "Ignoring Hint ID:%" PRIX32, pMsg->hint_id);
                pthread_mutex_unlock(&subm_req_mutex);
                if (evData != NULL) {
                    evqueue.GetDataPool().Return(evData);
                    evData = NULL;
                }
                return FAILED;
            }

            isHint = true;
            preActionRetVal = hintExt.FetchConfigPreAction(pMsg);
            if (preActionRetVal == PRE_ACTION_SF_RE_VAL) {
                QLOGL(LOG_TAG, QLOG_L1, "Special handling for PRE_ACTION_SF_RE_VAL");
                pthread_mutex_unlock(&subm_req_mutex);
                if (evData != NULL) {
                    evqueue.GetDataPool().Return(evData);
                    evData = NULL;
                }
                return preActionRetVal;
            }
            else if (preActionRetVal == PRE_ACTION_NO_FPS_UPDATE) {
                handle = -1;
                QLOGL(LOG_TAG, QLOG_L1, "Preaction FPS, dont update FPS");
                break;
            }
            size = t.getBoostConfig(pMsg->hint_id, pMsg->hint_type, pMsg->pl_args, &timeout,
                    FpsUpdateAction::getInstance().GetFps());

            if (size == 0) {
                //hint lookup failed, bailout
                handle = -1;
                QLOGL(LOG_TAG, QLOG_WARNING, "Unsupported hint_id=0x%" PRIX32 ", hint_type=%d for this target", pMsg->hint_id, pMsg->hint_type);
                break;
            }
            pMsg->data = size;
            hintExt.FetchConfigPostAction(pMsg);

            if (pMsg->pl_time <= 0) {
                pMsg->pl_time = timeout;
            }
            if ((pMsg->pl_time == 0 || pMsg->pl_time > MAX_OFFLOAD_TIMEOUT) && pMsg->offloadFlag == true) {
                QLOGE(LOG_TAG, "Cannot submit offload Hint=0x%" PRIX32 " of %" PRId32 " duration, Max Allowed duration: %" PRIu8, pMsg->hint_id, pMsg->pl_time, MAX_OFFLOAD_TIMEOUT);
                handle = -1;
                break;
            }
            if (pMsg->pl_time < 0) {
                QLOGE(LOG_TAG, "No valid timer value for perflock request");
                handle = -1;
                break;
            }
            char trace_buf[TRACE_BUF_SZ];
            if (perf_debug_output) {
                snprintf(trace_buf, TRACE_BUF_SZ, "perf_lock_acq2: Hint Type=%" PRId32" Offload Flag=%" PRId8, pMsg->hint_type, pMsg->offloadFlag);
                QLOGE(LOG_TAG, "%s", trace_buf);
            }
            if (PERF_SYSTRACE) {
                ATRACE_BEGIN(trace_buf);
                ATRACE_END();
            }
            strlcpy(pMsg->usrdata_str, msg->usrdata_str, MAX_MSG_APP_NAME_LEN);
            //intentional fall through
            [[fallthrough]];
        case MPCTL_CMD_PERFLOCKACQ:
            before_handle = pMsg->pl_handle;
            req = internal_perf_lock_acq_verification(pMsg->pl_handle, pMsg->pl_time, pMsg->pl_args, size,
                    pMsg->client_pid, pMsg->client_tid, pMsg->renewFlag, isHint);
            handle = pMsg->pl_handle;
            if (req != NULL) {
                pMsg->userdata = (void *) req;
                bool isDisplayOn = isDisplayOnReq(req);
                if (isDisplayOn == false) {
                    request_list.Add(req, handle, pMsg->hint_id, pMsg->hint_type);
                }else if ((isDisplayOn == true) && (pMsg->hint_id != VENDOR_HINT_DISPLAY_ON)){
                    delete req;
                    req = NULL;
                    handle = -1;
                    break;
                }
            } else if ((handle > 0) && (handle == before_handle)) {
                reqExist = true;
            } else {
                handle = -1;
            }
            FpsUpdateAction::getInstance().saveFpsHintHandle(handle, pMsg->hint_id);
            break;
        default:
            break;
    }

    if (reqExist || (handle < 0)) {
        evqueue.GetDataPool().Return(evData);
        evData = NULL;
    }

    if (NULL != evData) {
        evData->mEvType = cmd;
        evqueue.Wakeup(evData);
    }
    pthread_mutex_unlock(&subm_req_mutex);
    return handle;
}

void perfmodule_sync_request_ext(int32_t cmd, char **debugInfo) {
    switch (cmd) {
        case SYNC_CMD_DUMP_DEG_INFO:
            {
                int debuggable = 0;
                char value[PROPERTY_VALUE_MAX];
                if (property_get(BUILD_DEBUGGABLE, value, "0")) {
                    debuggable = atoi(value);
                }
                if (debuggable) {
                    std::string dump = request_list.DumpActive();
                    int size = dump.size(); //It doesn't contain the end char '\0'
                    if(debugInfo != NULL) {
                        *debugInfo = (char *)malloc((size+1) * sizeof(char));
                        if (!(*debugInfo)) {
                            QLOGE(LOG_TAG, "malloc failed for debugInfo.");
                            return;
                        }
                        memset(*debugInfo, 0, sizeof(char)*(size+1));
                        strlcpy(*debugInfo, dump.c_str(), size+1);
                    }
                }
            }
            break;
    }
}
