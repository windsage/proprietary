// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.

#ifndef __LOGGING_H
#define __LOGGING_H

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_TAG "minksocket"
#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#else
#include <stdio.h>
#define ANDROID_LOG_DEBUG 1
#define ANDROID_LOG_INFO 2
#define ANDROID_LOG_WARN 3
#define ANDROID_LOG_ERROR 4
#define LOG(...) { printf(__VA_ARGS__); fflush(stdout); }
#define LOGE LOG
#endif

#define FATAL(...) { LOGE(__VA_ARGS__); exit(-1); }
#define LOGF() LOG("%s:%d\n", __FUNCTION__, __LINE__)

#define ALOGE LOGE
#define ALOG LOG
#define ALOGD LOG
#define ALOGV LOG
#endif


