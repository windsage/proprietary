/******************************************************************************
  @file    perflock_native_test_server.cpp
  @brief   mpctl test which tests perflocks, hints, profiles

  ---------------------------------------------------------------------------
  Copyright (c) 2017,2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOG_TAG "ANDR-PERF-NTS"
#define ATRACE_TAG ATRACE_TAG_ALWAYS

#include "PerfController.h"

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <cutils/trace.h>
#include <cutils/properties.h>

#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include<thread>

#define PORT 5555
#define MSG_BUF_SIZE 2048


#include "PerfLog.h"

#define TIME_MSEC_IN_SEC    1000
#define TIME_NSEC_IN_MSEC   1000000
#define TRACE_BUF_SZ 512

static int (*perf_lock_acq)(int handle, int duration,
                            int list[], int numArgs) = NULL;
static int (*perf_lock_rel)(int handle) = NULL;
static int (*perf_hint)(int, const char *, int, int) = NULL;
static int (*perf_lock_use_profile)(int handle, int profile) = NULL;
static int (*perf_get_feedback)(int handle, const char *pkgName) = NULL;
static int (*perf_lock_acq_rel)(int , int , int [], int , int) = NULL;
static int (*perf_get_feedback_extn)(int handle, const char *pkgName, int, int []) = NULL;
static void(* perf_event)(int, const char *, int, int[]) = NULL;
static int (*perf_hint_acq_rel)(int, int, const char *, int, int, int, int *) = NULL;
static int (*perf_hint_renew)(int, int, const char *, int, int, int, int *) = NULL;
static int (*perf_hint_offload)(int, const char *, int, int, int , int[]) = NULL;
static int(* perf_event_offload)(int, const char *, int, int[]) = NULL;
static int (*perf_hint_acq_rel_offload)(int, int, const char *, int, int, int, int *) = NULL;
static int (*perf_lock_rel_offload)(int handle) = NULL;

static void *libhandle = NULL;

int native_test_call(int argc, char *argv[]);

static void initialize(void) {
    const char *rc = NULL;
    char qcopt_lib_path[100] = "libqti-perfd-client.so";
    int ret_code = pthread_setname_np(pthread_self(), LOG_TAG);
    if (ret_code != 0) {
        QLOGE(LOG_TAG, "Failed to name Thread:  %s", LOG_TAG);
    }

    dlerror();

    QLOGE(LOG_TAG, "NRP: lib name %s", qcopt_lib_path);
    libhandle = dlopen(qcopt_lib_path, RTLD_NOW);
    if (!libhandle) {
        QLOGE(LOG_TAG, "NRP: Unable to open %s: %s\n", qcopt_lib_path,
                dlerror());
    }

    if (!libhandle) {
        QLOGE(LOG_TAG, "NRP: Failed to get qcopt handle.\n");
    } else {
        /*
         * qc-opt handle obtained. Get the perflock acquire/release
         * function pointers.
         */
        *(void **) (&perf_lock_acq) = dlsym(libhandle, "perf_lock_acq");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_lock_acq function handle.\n");
        }

        *(void **) (&perf_lock_rel) = dlsym(libhandle, "perf_lock_rel");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_lock_rel function handle.\n");
        }

        *(void **) (&perf_hint) = dlsym(libhandle, "perf_hint");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint function handle.\n");
        }

        *(void **) (&perf_lock_acq_rel) = dlsym(libhandle, "perf_lock_acq_rel");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_lock_acq_rel function handle.\n");
        }

        *(void **) (&perf_get_feedback_extn) = dlsym(libhandle, "perf_get_feedback_extn");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_get_feedback_extn function handle.\n");
        }

        *(void **) (&perf_event) = dlsym(libhandle, "perf_event");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_event function handle.\n");
        }

        *(void **) (&perf_hint_acq_rel) = dlsym(libhandle, "perf_hint_acq_rel");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint_acq_rel function handle.\n");
        }

        *(void **) (&perf_event_offload) = dlsym(libhandle, "perf_event_offload");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_event_offload function handle.\n");
        }

        *(void **) (&perf_hint_offload) = dlsym(libhandle, "perf_hint_offload");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint_offload function handle.\n");
        }

        *(void **) (&perf_hint_acq_rel_offload) = dlsym(libhandle, "perf_hint_acq_rel_offload");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint_acq_rel_offload function handle.\n");
        }

        *(void **) (&perf_lock_rel_offload) = dlsym(libhandle, "perf_lock_rel_offload");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_lock_rel_offload function handle.\n");
        }

        *(void **) (&perf_hint_renew) = dlsym(libhandle, "perf_hint_renew");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint_renew function handle.\n");
        }


        *(void **) (&perf_lock_use_profile) = dlsym(libhandle, "perf_lock_use_profile");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint function handle.\n");
        }
        *(void **) (&perf_get_feedback) = dlsym(libhandle, "perf_get_feedback");
        if ((rc = dlerror()) != NULL) {
             QLOGE(LOG_TAG, "NRP: Unable to get perf_hint function handle.\n");
        }
    }
}
void client_connect(int connfd) {
    char sendBuff[MSG_BUF_SIZE];
    char recvBuff[MSG_BUF_SIZE];
    int readbytes = 0;
    char *ptr = NULL;
    char *array[50];
    int i = -1;
    char *p = NULL;

    memset(recvBuff, '0',sizeof(recvBuff));
    memset(sendBuff, '0', sizeof(sendBuff));
    readbytes = read(connfd, recvBuff, sizeof(recvBuff));
    if (readbytes > 0 && readbytes < MSG_BUF_SIZE) //receiving message from client
    {
        recvBuff[readbytes] = '\0';
        printf("========================\nClient[%d]: %s\n",connfd, recvBuff);

        p = strtok_r(recvBuff, " ",&ptr);
        i = 0;
        while (p != NULL) {
            array[i++] = p;
            p = strtok_r(NULL, " ",&ptr);
        }
        i = native_test_call(i, array);
        printf("\n========================\n");
    }

    snprintf(sendBuff, MSG_BUF_SIZE,"%d", i);
    write(connfd, sendBuff, strlen(sendBuff));//sending message to client
    close(connfd);//closing client connection
}

void show_usage() {
    QLOGE(LOG_TAG, "Invalid Input Check client for usage");
    printf("\nInvalid Input Check client for usage\n");
}

int native_test_call(int argc, char *argv[]) {
    QLOGE(LOG_TAG, "NRP: %s %d", __func__, argc);
    char arg_list[120] = {};
    int resource_list[OPTIMIZATIONS_MAX] = {};
    int args_count = 0;

    int handle = -1, profile = 0;
    int duration = 0;
    char *argstr;
    int hint_id = -1, hint_type = -1;
    char *pkg = NULL;
    int idx = 0;
    char *ptr = NULL;
    int numArgs = 0;
    int rnumArgs = 0;
    int temparr[6] ={0};
    char trace_buf[TRACE_BUF_SZ];
    for(int i = 0; i < argc; i++) {
        printf("\n%s ", argv[i]);
    }

    std::string arg = argv[idx];
    idx++;
    if (arg == "--fb") {
        if (perf_get_feedback) {
            int request = -1;
            if (idx < argc) {
                request = strtoul(argv[idx], NULL, 16);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_get_feedback called request=0x%x", __func__, request);
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
                request = strtoul(argv[idx], NULL, 16);
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
                numArgs = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 10);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_get_feedback_extn called request=0x%x", __func__, request);
            ATRACE_BEGIN(trace_buf);
            int appType = perf_get_feedback_extn(request, pkg, numArgs, temparr);
            ATRACE_END();
            printf("request = %#x, %s type is %#x\n", request, pkg, appType);
            return appType;
        }
    } else if (arg == "--evt") {
        if (perf_event) {
            int request = -1;
            if (idx < argc) {
                request = strtoul(argv[idx], NULL, 16);
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
                numArgs = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 10);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_event called event=0x%x", __func__, request);
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
                request = strtoul(argv[idx], NULL, 16);
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
                numArgs = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 10);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_event_offload called event=0x%x", __func__, request);
            ATRACE_BEGIN(trace_buf);
            int offloaded = perf_event_offload(request, pkg, numArgs, temparr);
            ATRACE_END();
            duration = 1000;
            printf("Event = %#x, %s \n", request, pkg);
            return offloaded;
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
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                sscanf(argv[idx], "%s", arg_list);
                argstr = strtok_r(arg_list, ",", &ptr);
                while (argstr != NULL) {
                    sscanf(argstr, "%X", &resource_list[args_count]);
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
                numArgs = strtoul(argv[idx], NULL, 10);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if(idx < argc) {
                rnumArgs = strtoul(argv[idx], NULL, 10);
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

            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_lock_acq_rel called", __func__);
            ATRACE_BEGIN(trace_buf);
            handle = perf_lock_acq_rel(handle, duration, resource_list, numArgs,rnumArgs);
            ATRACE_END();
            printf("\nhandle=%d\n", handle);
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
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                sscanf(argv[idx], "%s", arg_list);
                argstr = strtok_r(arg_list, ",", &ptr);
                while (argstr != NULL) {
                    sscanf(argstr, "%X", &resource_list[args_count]);
                    QLOGE(LOG_TAG, "NRP: resource_list[%d] = %x", args_count, resource_list[args_count]);
                    args_count++;
                    argstr = strtok_r(NULL, ",", &ptr);
                }
                idx++;
            } else {
                show_usage();
                return 0;
            }

            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_lock_acq called", __func__);
            ATRACE_BEGIN(trace_buf);
            handle = perf_lock_acq(handle, duration, resource_list, args_count);
            ATRACE_END();
            printf("\nhandle=%d\n", handle);
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
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_hint called", __func__);
            ATRACE_BEGIN(trace_buf);
            handle = perf_hint(hint_id, pkg, duration, hint_type);
            ATRACE_END();
            printf("\nhandle=%d\n", handle);
            QLOGE(LOG_TAG, "Client: perf hint hint_id=0x%x", hint_id);
        }
    } else if (arg == "--profile") {
        duration = -1;
        if (perf_lock_use_profile) {
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                profile = strtoul(argv[idx], NULL, 10);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            }

            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_lock_use_profile called", __func__);
            ATRACE_BEGIN(trace_buf);
            handle = perf_lock_use_profile(handle, profile);
            ATRACE_END();
            printf("\nhandle=%d\n", handle);
            QLOGE(LOG_TAG, "Client: lock profile handle=%d profile=%d", handle, profile);
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
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_lock_rel called", __func__);
            ATRACE_BEGIN(trace_buf);
            perf_lock_rel(handle);
            ATRACE_END();
        }
    } else if (arg == "--rel_offload") {
        if (perf_lock_rel_offload) {
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            QLOGE(LOG_TAG, "Client: Release handle=%d", handle);
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_lock_rel_offload called", __func__);
            ATRACE_BEGIN(trace_buf);
            perf_lock_rel_offload(handle);
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
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            QLOGE(LOG_TAG, "Client: perf hint hint_id=0x%x", hint_id);
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_hint_offload called hint=0x%x ", __func__, hint_id);
            ATRACE_BEGIN(trace_buf);
            handle = perf_hint_offload(hint_id, pkg, duration, hint_type,0 ,NULL);
            ATRACE_END();
            printf("\nhandle=%d\n", handle);
            duration = duration+500;
        }
    } else if (arg == "--hintacqrel") {
        if (perf_hint_acq_rel) {
            duration = -1;
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 10);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                hint_id = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            if (idx < argc) {
                numArgs = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 10);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_hint_acq_rel called hint=0x%x ", __func__, hint_id);
            ATRACE_BEGIN(trace_buf);
            handle = perf_hint_acq_rel(handle, hint_id, pkg, duration, hint_type, numArgs, temparr);
            ATRACE_END();
            printf("\nhandle=%d\n", handle);
            QLOGE(LOG_TAG, "Client: perf_hint_acq_rel hint_id=0x%x", hint_id);
        }
    } else if (arg == "--hintacqrel_offload") {
        if (perf_hint_acq_rel_offload) {
            duration = -1;
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 10);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                hint_id = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            if (idx < argc) {
                numArgs = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 10);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_hint_acq_rel_offload called hint=0x%x ", __func__, hint_id);
            ATRACE_BEGIN(trace_buf);
            handle = perf_hint_acq_rel_offload(handle, hint_id, pkg, duration, hint_type, numArgs, temparr);
            ATRACE_END();
            printf("\nhandle=%d\n", handle);
            QLOGE(LOG_TAG, "Client: perf_hint_acq_rel_offload hint_id=0x%x", hint_id);
        }
    } else if (arg == "--hintrenew") {
        if (perf_hint_renew) {
            duration = -1;
            if (idx < argc) {
                handle = strtoul(argv[idx], NULL, 10);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                hint_id = strtoul(argv[idx], NULL, 0);
                idx++;
            } else {
                show_usage();
                return 0;
            }
            if (idx < argc) {
                duration = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            if (idx < argc) {
                pkg = argv[idx];
                idx++;
            }
            if (idx < argc) {
                hint_type = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            if (idx < argc) {
                numArgs = strtoul(argv[idx], NULL, 10);
                idx++;
            }
            for (int index_i = 0; idx < argc && index_i < numArgs; idx++,index_i++)
            {
                temparr[index_i] = strtoul(argv[idx], NULL, 10);
            }
            snprintf(trace_buf, TRACE_BUF_SZ,"%s perf_hint_renew called hint=0x%x", __func__, hint_id);
            ATRACE_BEGIN(trace_buf);
            handle = perf_hint_renew(handle, hint_id, pkg, duration, hint_type, numArgs, temparr);
            ATRACE_END();
            printf("\nhandle=%d\n", handle);
            QLOGE(LOG_TAG, "Client: perf_hint_renew hint_id=0x%x", hint_id);
        }
    } else {
        show_usage();
        return 0;
    }

    return handle;
}

int main(int argc, char *argv[]) {
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    int port = PORT;
    if (argc > 1 && argc <3) {
        port = atoi(argv[1]);
    }


    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (::bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        if (EADDRINUSE == errno) {
            printf("\nError in Running Server on default PORT: %d\n", port);
            printf("try running server on different using perflock_native_test_server <NEW_PORT>\n");
        } else {
            printf("\nError in running server try again\n");
        }
        return -1;
    }

    listen(listenfd, 100);
    printf("\nServer listening for connections!\n");


    initialize();
    while (1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        printf("\nAccepted a connection from Client[%d]\n",connfd);
        {
            std::thread worker(
                    [=](int client_connfd) {
                    client_connect(client_connfd);
                    int duration = 2000;
                    if ((duration > 0)) {
                    struct timespec sleep_time;
                    QLOGE(LOG_TAG, "NRP: Sleep");
                    sleep_time.tv_sec = duration / TIME_MSEC_IN_SEC;
                    sleep_time.tv_nsec = (duration % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
                    nanosleep(&sleep_time, NULL);
                    }
                    }, connfd);
            worker.detach();
        }
    }
    if (libhandle) {
        dlclose(libhandle);
        libhandle = NULL;
    }
}
