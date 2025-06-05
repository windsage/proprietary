// Copyright (c) 2017, 2021-2023 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.

#ifndef __MINKHUB_LOGGING_H
#define __MINKHUB_LOGGING_H

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_TAG "MinkHub"
#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#define SLOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, SLOG_TAG, __VA_ARGS__))
#define SLOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, SLOG_TAG, __VA_ARGS__))

#else
#include <stdio.h>
#include <unistd.h>
#define ANDROID_LOG_DEBUG 1
#define ANDROID_LOG_INFO 2
#define ANDROID_LOG_WARN 3
#define ANDROID_LOG_ERROR 4
#define LOG(xx_fmt, ...) { printf("(%u) %s:%u " xx_fmt "\n", getpid(), __func__, __LINE__, ##__VA_ARGS__); fflush(stdout); }
#define LOGE LOG
#endif

#define FATAL(...) { LOGE(__VA_ARGS__); exit(-1); }
#define LOGF() LOG("%s:%d\n", __FUNCTION__, __LINE__)

#define CHECK_ELSE(is_valid, else_do)   \
  do {                                  \
    if (!(is_valid)) {                  \
      else_do;                          \
    };                                  \
  } while (0)


#define GUARD(status)                   \
  do {                                  \
    int _st = (status);                 \
    if (_st != 0) return _st;           \
  } while (0)

#define ALOGE LOGE
#define ALOG LOG
#define ALOGD LOG
#define ALOGV LOG
#define LOGD LOG
#define SLOGD SLOG

static inline int atomic_add(int* pn, int n) {
  return __sync_add_and_fetch(pn, n);  // GCC builtin
}
#endif


