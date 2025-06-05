/* //device/system/rild/rild.c
**
** Copyright 2006 The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#ifndef QMI_RIL_UTF
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>

#include <telephony/ril.h>
#define LOG_TAG "RILD"
#ifdef RIL_FOR_MDM_LE
#include <utils/Log2.h>
#else
#include <log/log.h>
#include <sys/capability.h>
#endif
#include <cutils/properties.h>
#include <cutils/sockets.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libril/ril_ex.h>

#ifdef QCRIL_ENABLE_AFL
#include <ril_afl.hpp>
#endif

#ifndef RIL_FOR_MDM_LE
#include "SetupMinijail.h"
#endif

#define LIB_PATH_PROPERTY   "vendor.rild.libpath"
#define LIB_ARGS_PROPERTY   "vendor.rild.libargs"
#define MAX_LIB_ARGS        16
#define LIB_PATH_NRDEVICEINFO "/vendor/lib64/deviceInfoServiceModuleNr.so"


static void usage(const char *argv0) {
    fprintf(stderr, "Usage: %s -l <ril impl library> [-- <args for impl library>]\n", argv0);
    exit(EXIT_FAILURE);
}

extern char ril_service_name_base[MAX_SERVICE_NAME_LENGTH];
extern char ril_service_name[MAX_SERVICE_NAME_LENGTH];

extern void RIL_register (const RIL_RadioFunctions *callbacks);
extern void rilc_thread_pool ();

extern void RIL_onRequestComplete(RIL_Token t, RIL_Errno e,
        void *response, size_t responselen);

extern void RIL_onRequestAck(RIL_Token t);

extern void RIL_setProcessInstanceId(int clientId);
extern void RIL_dbInit();
extern void RIL_loadPlugins();


#if defined(ANDROID_MULTI_SIM)
extern void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
        size_t datalen, RIL_SOCKET_ID socket_id);
#else
extern void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
        size_t datalen);
#endif

extern void RIL_requestTimedCallback (RIL_TimedCallback callback,
        void *param, const struct timeval *relativeTime);


static struct RIL_Env s_rilEnv = {
    RIL_onRequestComplete,
    RIL_onUnsolicitedResponse,
    RIL_requestTimedCallback,
    RIL_onRequestAck
};

extern void RIL_startEventLoop();

static int make_argv(char * args, char ** argv) {
    // Note: reserve argv[0]
    int count = 1;
    char * tok;
    char * s = args;
    char * lastPtr = NULL;

    while ((tok = strtok_r(s, " \0", &lastPtr))) {
        argv[count] = tok;
        s = NULL;
        count++;
    }
    return count;
}

int main(int argc, char **argv) {
#ifndef RIL_FOR_MDM_LE
#ifdef FEATURE_QCRIL_MINIJAIL
    setupMinijail("/vendor/etc/seccomp_policy/qcrilnr@2.0.policy");
#endif
#endif
    // vendor ril lib path either passed in as -l parameter, or read from rild.libpath property
    const char *rilLibPath = NULL;
    // ril arguments either passed in as -- parameter, or read from rild.libargs property
    char **rilArgv;
    // Pointer to ril init function in vendor ril
    const RIL_RadioFunctions *(*rilInit)(const struct RIL_Env *, int, char **);

    // functions returned by ril init function in vendor ril
    const RIL_RadioFunctions *funcs;
    // flat to indicate if -- parameters are present
    unsigned char hasLibArgs = 0;

    int i;
    // ril/socket id received as -c parameter, otherwise set to 0
    const char *clientId = NULL;

    RLOGD("**RIL Daemon Started**");
    RLOGD("**RILd param count=%d**", argc);
    umask(S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
    for (i = 1; i < argc ;) {
        if (0 == strcmp(argv[i], "-l") && (argc - i > 1)) {
            rilLibPath = argv[i + 1];
            i += 2;
        } else if (0 == strcmp(argv[i], "--")) {
            i++;
            hasLibArgs = 1;
            break;
        } else if (0 == strcmp(argv[i], "-c") &&  (argc - i > 1)) {
            clientId = argv[i+1];
            i += 2;
        } else {
            usage(argv[0]);
        }
    }

    if (clientId == NULL) {
        clientId = "0";
    } else if (atoi(clientId) >= MAX_RILDS) {
        RLOGE("Max Number of rild's supported is: %d", MAX_RILDS);
        exit(0);
    }
#ifndef RIL_FOR_MDM_LE
    if (strncmp(clientId, "0", MAX_CLIENT_ID_LENGTH)) {
        strlcpy(ril_service_name, ril_service_name_base, sizeof(ril_service_name));
        strlcat(ril_service_name, clientId, sizeof(ril_service_name));
    }
#endif

    RIL_setProcessInstanceId(atoi(clientId));
    RIL_dbInit();
    RIL_loadPlugins();
    RIL_startEventLoop();

    rilInit = RIL_Init;

    if (rilInit == NULL) {
        RLOGE("RIL_Init not defined or exported in %s\n", rilLibPath);
        exit(EXIT_FAILURE);
    }

    if (hasLibArgs) {
        rilArgv = argv + i - 1;
        argc = argc -i + 1;
    } else {
        static char * newArgv[MAX_LIB_ARGS];
        static char args[PROPERTY_VALUE_MAX];
        rilArgv = newArgv;
        property_get(LIB_ARGS_PROPERTY, args, "");
        argc = make_argv(args, rilArgv);
    }

    rilArgv[argc++] = "-c";
    rilArgv[argc++] = (char*)clientId;
    RLOGD("RIL_Init argc = %d clientId = %s", argc, rilArgv[argc-1]);

    // Make sure there's a reasonable argv[0]
    rilArgv[0] = argv[0];

#ifdef FEATURE_QCRIL_DEVICE_INFO_SERVICE
    void *handle = dlopen(LIB_PATH_NRDEVICEINFO, RTLD_NOW);
    if (handle == NULL) {
        RLOGE("deviceInfoServiceModuleNr dlopen failed ERROR:%s", dlerror());
    } else {
        RLOGD("deviceInfoServiceModuleNr found and loaded");
    }
#endif

    funcs = rilInit(&s_rilEnv, argc, rilArgv);
    RLOGD("RIL_Init rilInit completed");

    RIL_register(funcs);

    RLOGD("RIL_Init RIL_register completed");

    RLOGD("RIL_register_socket completed");

#ifdef QCRIL_ENABLE_AFL
    RLOGD("Calling RIL AFL setup");
    const char rilSocketPathPrefix[] = "/dev/socket/qcrild/rild";
    char rilSocketPath[sizeof(rilSocketPathPrefix) + 6] = { 0 };
    strlcat(rilSocketPath, rilSocketPathPrefix, sizeof(rilSocketPath));
    strlcat(rilSocketPath, clientId, sizeof(rilSocketPath));
    ril_afl_setup(rilSocketPath);
#endif

#ifndef RIL_FOR_MDM_LE
    rilc_thread_pool();
#endif

    RLOGD("RIL_Init starting sleep loop");
    while (true) {
        sleep(INT32_MAX);
    }
    return 0;
}
#endif

#ifndef RIL_FOR_MDM_LE
#include <android/binder_process.h>
extern void rilc_thread_pool() {
#ifndef QMI_RIL_UTF
//   QCRIL_LOG_INFO("joinRpcThreadPool HIDL");
//   joinRpcThreadpool();
    RLOGD("joinThreadPool Stable AIDL");
    ABinderProcess_joinThreadPool();
#endif
}
#endif
