/***********************************************************************
 * Copyright (c) 2023-2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/

#ifdef __ANDROID__
#include <cutils/properties.h>
#endif /* __ANDROID__ */
#include "MinkTransportUtils.h"

#define MAX_COMBINED_LOG_MSG_LEN 3072

static bool gIsTraceEnable = false;

/* Avoid stack size increasing since they appear inside each log.*/
static char __combinedLog[MAX_COMBINED_LOG_MSG_LEN];

/* Ensure multiple-thread safety */
static pthread_mutex_t fmtPrintfMtx __attribute__((unused)) = PTHREAD_MUTEX_INITIALIZER;

void MinkTransportUtils_configTrace(void)
{
#ifdef OFFTARGET
    char* isTraceEnable = getenv("minksocketFullLog");
    if (NULL != isTraceEnable) {
        gIsTraceEnable = true;
    }
#else
#ifdef __ANDROID__
#ifdef MINK_TRACE_ON
    char traceEnableStr[PROPERTY_VALUE_MAX];
    if (property_get("vendor.minksocketFullLog", traceEnableStr, NULL)) {
        gIsTraceEnable = true;
    }
#endif /* MINK_TRACE_ON */
#else
    if (access("/tmp/minksocketFullLog", F_OK) == 0) {
        gIsTraceEnable = true;
    }
#endif /* __ANDROID__ */
#endif /* OFFTARGET */
}

bool MinkTransportUtils_getTrace(void)
{
    return gIsTraceEnable;
}

void MinkTransportUtils_printf(const char *fmt, ...)
{
    va_list args;
    memset(&args, 0, sizeof(va_list));
    if (!vm_osal_mutex_lock(&fmtPrintfMtx)) {
        va_start(args, fmt);
        vsnprintf(__combinedLog, sizeof(__combinedLog), fmt, args);
        va_end(args);
        write(STDOUT_FILENO, __combinedLog, strlen(__combinedLog));
        vm_osal_mutex_unlock(&fmtPrintfMtx);
    }
}
