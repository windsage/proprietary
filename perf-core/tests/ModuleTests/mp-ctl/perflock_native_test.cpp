/******************************************************************************
  @file    perflock_native_test.cpp
  @brief   mpctl test which tests perflocks, hints, profiles

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define ATRACE_TAG ATRACE_TAG_ALWAYS
#define LOG_TAG           "ANDR-PERFLOCK-TESTER"
#include "PerfLog.h"
#include <config.h>
#include "PerfController.h"

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <cutils/trace.h>
#include <cutils/properties.h>


#define TIME_MSEC_IN_SEC    1000
#define TIME_NSEC_IN_MSEC   1000000
#define TRACE_BUF_SZ 512


typedef struct {
    char value[PROP_VAL_LENGTH];
} PropVal;
static PropVal (*perf_get_prop)(const char *prop , const char *def_val);
static int (*perf_get_prop_extn)(const char *prop, char *buffer, size_t buffersize, const char *def_val);
static int (*perf_lock_acq)(int handle, int duration,
                            int list[], int numArgs) = NULL;
static int (*perf_lock_rel)(int handle) = NULL;
static int (*perf_hint)(int, const char *, int, int) = NULL;
static int (*perf_get_feedback)(int handle, const char *pkgName) = NULL;
static int (*perf_lock_acq_rel)(int , int , int [], int , int) = NULL;
static int (*perf_get_feedback_extn)(int handle, const char *pkgName, int, int []) = NULL;
static void(* perf_event)(int, const char *, int, int[]) = NULL;
static int (*perf_hint_acq_rel)(int, int, const char *, int, int, int, int *) = NULL;
static int (*perf_hint_renew)(int, int, const char *, int, int, int, int *) = NULL;
static int (*perf_hint_offload)(int, const char *, int, int, int , int[]) = NULL;
static int(* perf_event_offload)(int, const char *, int, int[]) = NULL;
static int (*perf_hint_acq_rel_offload)(int, int, const char *, int, int, int, int *) = NULL;
static const char *(*perf_sync_request)(int) = NULL;

char arg_list[120] = {};
int resource_list[OPTIMIZATIONS_MAX] = {};
int args_count = 0;
static void *libhandle = NULL;

static void initialize(void) {
    const char *rc = NULL;
    char qcopt_lib_path[100] = "libqti-perfd-client.so";

    dlerror();

    QLOGE(LOG_TAG, "NRP: lib name %s", qcopt_lib_path);
    libhandle = dlopen(qcopt_lib_path, RTLD_NOW);
    if (!libhandle) {
        QLOGE(LOG_TAG, "NRP: Unable to open %s: %s\n", qcopt_lib_path,
                dlerror());
    }

    if (!libhandle) {
        QLOGE(LOG_TAG, "NRP: lib name %s", qcopt_lib_path);
        QLOGE(LOG_TAG, "NRP: Failed to get qcopt handle.\n");
    } else {
        /*
         * qc-opt handle obtained. Get the perflock acquire/release
         * function pointers.
         */
        *(void **) (&perf_lock_acq) = dlsym(libhandle, "perf_lock_acq");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_lock_acq function handle.\n");
             perf_lock_acq = NULL;
        }

        *(void **) (&perf_lock_rel) = dlsym(libhandle, "perf_lock_rel");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_lock_rel function handle.\n");
             perf_lock_rel = NULL;
        }
        *(void **) (&perf_hint) = dlsym(libhandle, "perf_hint");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint function handle.\n");
             perf_hint = NULL;
        }

        *(void **) (&perf_lock_acq_rel) = dlsym(libhandle, "perf_lock_acq_rel");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_lock_acq_rel function handle.\n");
             perf_lock_acq_rel = NULL;
        }

        *(void **) (&perf_get_feedback_extn) = dlsym(libhandle, "perf_get_feedback_extn");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_get_feedback_extn function handle.\n");
             perf_get_feedback_extn = NULL;
        }

        *(void **) (&perf_event) = dlsym(libhandle, "perf_event");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_event function handle.\n");
             perf_event = NULL;
        }

        *(void **) (&perf_get_prop) = dlsym(libhandle, "perf_get_prop");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_get_prop function handle.\n");
             perf_get_prop = NULL;
        }

        *(void **) (&perf_get_prop_extn) = dlsym(libhandle, "perf_get_prop_extn");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_get_prop_extn function handle.\n");
             perf_get_prop_extn = NULL;
        }

        *(void **) (&perf_event_offload) = dlsym(libhandle, "perf_event_offload");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_event_offload function handle.\n");
             perf_event_offload = NULL;
        }

        *(void **) (&perf_hint_offload) = dlsym(libhandle, "perf_hint_offload");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint_offload function handle.\n");
             perf_hint_offload = NULL;
        }

        *(void **) (&perf_hint_acq_rel) = dlsym(libhandle, "perf_hint_acq_rel");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint_acq_rel function handle.\n");
             perf_hint_acq_rel = NULL;
        }

        *(void **) (&perf_hint_acq_rel_offload) = dlsym(libhandle, "perf_hint_acq_rel_offload");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint_acq_rel_offload function handle.\n");
             perf_hint_acq_rel_offload = NULL;
        }

        *(void **) (&perf_hint_renew) = dlsym(libhandle, "perf_hint_renew");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint_renew function handle.\n");
             perf_hint_renew = NULL;
        }

        *(void **) (&perf_get_feedback) = dlsym(libhandle, "perf_get_feedback");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint function handle.\n");
             perf_get_feedback = NULL;
        }

        *(void **) (&perf_sync_request) = dlsym(libhandle, "perf_sync_request");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_sync_request function handle.\n");
             perf_sync_request = NULL;
        }
    }
}

void show_usage() {
    printf("usage: perflock_native_test <options> <option-args>\n"
           "\t--acq <handle> <duration> <resource, value, resource, value,...>\n"
           "\t--acqrel <handle> <duration> <resource,value,resource,value,...> <numArgs> <reservedArgs>\n"
           "\t--hint <hint_id> [<duration>] [<pkg-name>] [<hint-type>]\n"
           "\t--profile <handle> <profile_id> [<duration>]\n"
           "\t--rel <handle>\n"
           "\t--hintacqrel <handle> <hint_id> [<duration>] [<pkg-name>] [<hint-type>] <numArgs> [<reserved_list>]\n"
           "\t--hintrenew <handle> <hint_id> [<duration>] [<pkg-name>] [<hint-type>] <numArgs> [<reserved_list>]\n"
           "\t--fbex req [<pkg-name>] <numArgs> [<reserved_list>]\n"
           "\t--evt event [<pkg-name>] <numArgs> [<reserved_list>]\n"

           "\t--hint_offload <hint_id> [<duration>] [<pkg-name>] [<hint-type>]\n"
           "\t--evt_offload event [<pkg-name>] <numArgs> [<reserved_list>]\n"
           "\t--hint_offload_acqrel <handle> <hint_id> [<duration>] [<pkg-name>] [<hint-type>] <numArgs> [<reserved_list>]\n"
           "\t--gp <property> <default value>\n"
           "\t--gp <property>\n"
           "\t--gpextn <property> <default value>\n"
           "\t--gpextn <property>\n"

           "\tperflock acq, rel examles\n"
           "\tperflock_native_test --acq 0 5000 0x40C10000,0x1f\n"
           "\tperflock_native_test --acqrel 12 5000 \"0x40C10000,0x1f\" 2 0\n"
           "\tperflock_native_test --acqrel 12 5000 \"0x40C10000,0x1f,0x40C20000,0x1e\" 4 0\n"
           "\tperflock_native_test --rel 5"
           "\tperflock hint examles\n"
           "\tperflock_native_test --hint 0x00001203\n"
           "\tperflock_native_test --hint 0x00001203 20\n"
           "\tperflock_native_test --hint 0x00001203 10 \"helloApp\"\n"

           "\tperflock_native_test --evt_offload 0x00001203 \"helloApp\" 2  0 1\n"
           "\tperflock_native_test --hint_offload 0x00001203\n"
           "\tperflock_native_test --hint_offload 0x00001203 20\n"
           "\tperflock_native_test --hint_offload 0x00001203 10 \"helloApp\"\n"
           "\tperflock_native_test --hint_offload_acqrel 23 0x00001203\n"
           "\tperflock_native_test --hint_offload_acqrel 23 0x00001203 20\n"
           "\tperflock_native_test --hint_offload_acqrel 23 0x00001203 10 \"helloApp\"\n"
           "\tperflock_native_test --hint_offload_acqrel 23 0x00001203 10 \"helloApp\" 3 1 2 3\n"

           "\tperflock_native_test --hintacqrel 23 0x00001203\n"
           "\tperflock_native_test --hintacqrel 23 0x00001203 20\n"
           "\tperflock_native_test --hintacqrel 23 0x00001203 10 \"helloApp\"\n"
           "\tperflock_native_test --hintacqrel 23 0x00001203 10 \"helloApp\" 3 1 2 3\n"
           "\tperflock_native_test --hintrenew 23 0x00001203\n"
           "\tperflock_native_test --hintrenew 23 0x00001203 20\n"
           "\tperflock_native_test --hintrenew 23 0x00001203 10 \"helloApp\"\n"
           "\tperflock_native_test --hintrenew 23 0x00001203 10 \"helloApp\" 3 1 2 3\n"
           "\tperflock_native_test --perfdump\n");

    if (libhandle) {
        dlclose(libhandle);
        libhandle = NULL;
    }

    return;
}

int main(int argc, char *argv[]) {
    int handle = -1;
    int duration = 0;
    char *argstr;
    int hint_id = -1, hint_type = -1;
    char *pkg = NULL;
    int idx = 1;
    char *ptr = NULL;
    int numArgs = 0;
    int rnumArgs = 0;
    int temparr[6] ={0};
    char trace_buf[TRACE_BUF_SZ];

    QLOGE(LOG_TAG, "NRP: %s %d", __func__, argc);

    if (argc < 2) {
        show_usage();
        return 0;
    }

    initialize();

    std::string arg = argv[idx];
    idx++;
    if ((arg == "-h") || (arg == "--help")) {
        show_usage();
        return 0;
    } else if (arg == "--gp") {
        int prop_index = -1;
        if (perf_get_prop) {
            if (idx < argc) {
                prop_index = idx;
                idx++;
            } else {
                show_usage();
                return 0;
            }
            char property_val[PROPERTY_VALUE_MAX];
            if (idx < argc) {
                strlcpy(property_val, perf_get_prop(argv[prop_index], argv[idx]).value, PROPERTY_VALUE_MAX);
            }
            else {
                strlcpy(property_val, perf_get_prop(argv[prop_index], "Undefined").value, PROPERTY_VALUE_MAX);
            }
            property_val[PROPERTY_VALUE_MAX - 1] = '\0';
            printf("prop: %s: Value: %s\n",argv[prop_index], property_val);
        }
    } else if (arg == "--gpextn") {
        int prop_index = -1;
        if (perf_get_prop_extn) {
            if (idx < argc) {
                prop_index = idx;
                idx++;
            } else {
                show_usage();
                return 0;
            }
            char property_val[PROPERTY_VALUE_MAX];
            int rc = -1;
            if (idx < argc) {
               rc = perf_get_prop_extn(argv[prop_index], property_val, sizeof(property_val), argv[idx]);

            }
            else {
               rc = perf_get_prop_extn(argv[prop_index], property_val, sizeof(property_val), "Undefined");
            }
            property_val[PROPERTY_VALUE_MAX - 1] = '\0';
            printf("prop: %s: \nValue: %s\n",argv[prop_index], property_val);
        }
    } else if (arg == "--fb") {
        if (perf_get_feedback) {
            int request = -1;
            if (idx < argc) {
                request = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_get_feedback called request=0x%x", LOG_TAG, request);
                ATRACE_BEGIN(trace_buf);
                int appType = perf_get_feedback(request, argv[idx]);
                ATRACE_END();
                printf("request = %#x, %s type is %#x\n", request, argv[idx], appType);
                return 0;
            } else {
                show_usage();
                return 0;
            }
        }
    } else if (arg == "--fbex") {
        if (perf_get_feedback_extn) {
            int request = -1;
            if (idx < argc) {
                request = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                numArgs = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 0);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_get_feedback_extn called request=0x%x", LOG_TAG, request);
            ATRACE_BEGIN(trace_buf);
            int appType = perf_get_feedback_extn(request, pkg, numArgs, temparr);
            ATRACE_END();
            printf("request = %#x, %s type is %#x\n", request, pkg, appType);
            return 0;
        }
    } else if (arg == "--evt") {
        if (perf_event) {
            int request = -1;
            if (idx < argc) {
                request = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                numArgs = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 0);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_event called event=0x%x", LOG_TAG, request);
            ATRACE_BEGIN(trace_buf);
            perf_event(request, pkg, numArgs, temparr);
            ATRACE_END();
            printf("Event = %#x, %s \n", request, pkg);
            return 0;
        }
    } else if (arg == "--evt_offload") {
        if (perf_event_offload) {
            int request = -1;
            if (idx < argc) {
                request = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                numArgs = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 0);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_event_offload called event=0x%x", LOG_TAG, request);
            ATRACE_BEGIN(trace_buf);
            perf_event_offload(request, pkg, numArgs, temparr);
            ATRACE_END();
            duration = 1000;
            if ((duration > 0)) {
                struct timespec sleep_time;
                QLOGE(LOG_TAG, "NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
            }
            printf("Event = %#x, %s \n", request, pkg);
            return 0;
        }
    } else if (arg == "--acqrel") {
        if (perf_lock_acq_rel) {
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                sscanf(argv[idx], "%s", arg_list);
                argstr = strtok_r(arg_list, ",", &ptr);
                while (argstr != NULL) {
                    resource_list[args_count] = strtoul(argstr, NULL, 0);
                    QLOGE(LOG_TAG, "NRP: resource_list[%d] = %x", args_count, resource_list[args_count]);
                    args_count++;
                    argstr = strtok_r(NULL, ",", &ptr);
                }
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if(idx < argc) {
                numArgs = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if(idx < argc) {
                rnumArgs = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if(numArgs + rnumArgs != args_count) {
                printf("Invalid Number of arguments");
                show_usage();
                return 0;
            }

            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_lock_acq_rel called", LOG_TAG);
            ATRACE_BEGIN(trace_buf);
            handle = perf_lock_acq_rel(handle, duration, resource_list, numArgs,rnumArgs);
            ATRACE_END();
            printf("handle=%d\n", handle);
            if ((duration > 0) && (handle > 0)) {
                struct timespec sleep_time;
                QLOGE(LOG_TAG, "NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
            }
        }
    } else if (arg == "--acq") {
        if (perf_lock_acq) {
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                sscanf(argv[idx], "%s", arg_list);
                argstr = strtok_r(arg_list, ",", &ptr);
                while (argstr != NULL) {
                    resource_list[args_count] = strtoul(argstr, NULL, 0);
                    QLOGE(LOG_TAG, "NRP: resource_list[%d] = %x", args_count, resource_list[args_count]);
                    args_count++;
                    argstr = strtok_r(NULL, ",", &ptr);
                }
                idx++;
            } else {
                show_usage();
                return 0;
            }

            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_lock_acq called", LOG_TAG);
            ATRACE_BEGIN(trace_buf);
            handle = perf_lock_acq(handle, duration, resource_list, args_count);
            ATRACE_END();
            printf("handle=%d\n", handle);
            if ((duration > 0) && (handle > 0)) {
                struct timespec sleep_time;
                QLOGE(LOG_TAG, "NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
            }
        }
    } else if (arg == "--hint") {
        if (perf_hint) {
            duration = -1;
            if (idx < argc) {
                hint_id = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_hint called", LOG_TAG);
            ATRACE_BEGIN(trace_buf);
            handle = perf_hint(hint_id, pkg, duration, hint_type);
            ATRACE_END();
            printf("handle=%d\n", handle);
            QLOGE(LOG_TAG, "Client: perf hint hint_id=0x%x", hint_id);
            if ((duration > 0) && (handle > 0)) {
                struct timespec sleep_time;
                QLOGE(LOG_TAG, "NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
            }
        }
    } else if (arg == "--rel") {
        if (perf_lock_rel) {
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            QLOGE(LOG_TAG, "Client: Release handle=%d", handle);
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_lock_rel called", LOG_TAG);
            ATRACE_BEGIN(trace_buf);
            perf_lock_rel(handle);
            ATRACE_END();
        }
    } else if (arg == "--hint_offload") {
        if (perf_hint_offload) {
            duration = -1;
            if (idx < argc) {
                hint_id = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            QLOGE(LOG_TAG, "Client: perf hint hint_id=0x%x", hint_id);
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_hint_offload called hint=0x%x ", LOG_TAG, hint_id);
            ATRACE_BEGIN(trace_buf);
            handle = perf_hint_offload(hint_id, pkg, duration, hint_type,0 ,NULL);
            ATRACE_END();
            printf("handle=%d\n", handle);
            duration = duration+500;
            if ((duration > 0)) {
                struct timespec sleep_time;
                QLOGE(LOG_TAG, "NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
            }
        }
    } else if (arg == "--hintacqrel") {
        if (perf_hint_acq_rel) {
            duration = -1;
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                hint_id = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                numArgs = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 0);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_hint_acq_rel called hint=0x%x ", LOG_TAG, hint_id);
            ATRACE_BEGIN(trace_buf);
            handle = perf_hint_acq_rel(handle, hint_id, pkg, duration, hint_type, numArgs, temparr);
            ATRACE_END();
            printf("handle=%d\n", handle);
            QLOGE(LOG_TAG, "Client: perf_hint_acq_rel hint_id=0x%x", hint_id);
            if ((duration > 0) && (handle > 0)) {
                struct timespec sleep_time;
                QLOGE(LOG_TAG, "NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
            }
        }
    } else if (arg == "--hint_offload_acqrel") {
        if (perf_hint_acq_rel_offload) {
            duration = -1;
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                hint_id = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                numArgs = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 0);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_hint_acq_rel_offload called hint=0x%x ", LOG_TAG, hint_id);
            ATRACE_BEGIN(trace_buf);
            handle = perf_hint_acq_rel_offload(handle, hint_id, pkg, duration, hint_type, numArgs, temparr);
            ATRACE_END();
            printf("handle=%d\n", handle);
            QLOGE(LOG_TAG, "Client: perf_hint_acq_rel_offload hint_id=0x%x", hint_id);
            if ((duration > 0) && (handle > 0)) {
                struct timespec sleep_time;
                QLOGE(LOG_TAG, "NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
            }
        }
    } else if (arg == "--hintrenew") {
        if (perf_hint_renew) {
            duration = -1;
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                hint_id = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            if (idx < argc) {
                numArgs = strtoul(argv[idx], NULL, 0);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 0);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_hint_renew called hint=0x%x", LOG_TAG, hint_id);
            ATRACE_BEGIN(trace_buf);
            handle = perf_hint_renew(handle, hint_id, pkg, duration, hint_type, numArgs, temparr);
            ATRACE_END();
            printf("handle=%d\n", handle);
            QLOGE(LOG_TAG, "Client: perf_hint_renew hint_id=0x%x", hint_id);
            if ((duration > 0) && (handle > 0)) {
                struct timespec sleep_time;
                QLOGE(LOG_TAG, "NRP: Sleep");
                sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                nanosleep(&sleep_time, NULL);
            }
        }
    } else if (arg == "--perfdump") {
        if (perf_sync_request) {
            QLOGE(LOG_TAG, "Client: Dump Below");
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perfdump called", LOG_TAG);
            ATRACE_BEGIN(trace_buf);
            const char *res = perf_sync_request(SYNC_CMD_DUMP_DEG_INFO);
            if (res != NULL) {
                printf("\n%s\n",res);
                free((void*)res);
            }
            ATRACE_END();
        }
    } else {
        show_usage();
        return 0;
    }

    if (libhandle) {
        dlclose(libhandle);
        libhandle = NULL;
    }

    return handle;
}
