/***********************************************************************
 * Copyright (c) 2023-2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/

#ifndef _MINK_TRANSPORT_UTILS_H_
#define _MINK_TRANSPORT_UTILS_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "VmOsal.h"

void MinkTransportUtils_printf(const char *fmt, ...);

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_TAG "minksocket"
#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#else
#include <stdio.h>
#define LOG(...) MinkTransportUtils_printf(__VA_ARGS__)
#define LOGE LOG
#endif

void MinkTransportUtils_configTrace(void);
bool MinkTransportUtils_getTrace(void);

#define LOG_TRACE(xx_fmt, ...) \
  do {                 \
    if (true == MinkTransportUtils_getTrace()) { \
      LOG("MinkIPC TRACE: (%5u:%-5u) %s:%u " xx_fmt "", \
          vm_osal_getPid(), vm_osal_getTid(), __func__, __LINE__, ##__VA_ARGS__); \
    } \
  } while (0);

#define LOG_MSG(xx_fmt, ...) \
  LOG("MinkIPC MESSAGE: (%5u:%-5u) %s:%u " xx_fmt "", vm_osal_getPid(), \
       vm_osal_getTid(), __func__, __LINE__, ##__VA_ARGS__)

#define LOG_ERR(xx_fmt, ...) \
  LOGE("MinkIPC ERROR: (%5u:%-5u) %s:%u " xx_fmt "", vm_osal_getPid(), \
       vm_osal_getTid(), __func__, __LINE__, ##__VA_ARGS__)

#define TRUE_OR_CLEAN(expr, ...) \
  do {if (!(expr)) {LOG_ERR(__VA_ARGS__); goto cleanup;}} while (0)

#define POSITIVE_OR_CLEAN(expr, ...) \
  do {if ((expr) < 0) {LOG_ERR(__VA_ARGS__); goto cleanup;}} while (0)

#if defined(__cplusplus)
}
#endif

#endif /* _MINK_TRANSPORT_UTILS_H_ */

