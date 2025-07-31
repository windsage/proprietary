/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TLOG_H
#define TLOG_H

#include <cutils/log.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TLOG_LEVEL_VERBOSE 0
#define TLOG_LEVEL_DEBUG 1
#define TLOG_LEVEL_INFO 2
#define TLOG_LEVEL_WARN 3
#define TLOG_LEVEL_ERROR 4
#define TLOG_LEVEL_OFF 5

#ifndef TLOG_COMPILE_LEVEL
#define TLOG_COMPILE_LEVEL TLOG_LEVEL_VERBOSE
#endif

#ifndef TLOG_ENABLE
#define TLOG_ENABLE 1
#endif

extern int gTLogRuntimeLevel;

void TLogInit();

#define TLOG_IS_ENABLED(level) \
    (TLOG_ENABLE && (level) >= TLOG_COMPILE_LEVEL && (level) >= gTLogRuntimeLevel)

#if TLOG_ENABLE

#define TLOGV(fmt, ...)                          \
    do {                                         \
        if (TLOG_IS_ENABLED(TLOG_LEVEL_VERBOSE)) \
            ALOGV(fmt, ##__VA_ARGS__);           \
    } while (0)

#define TLOGD(fmt, ...)                        \
    do {                                       \
        if (TLOG_IS_ENABLED(TLOG_LEVEL_DEBUG)) \
            ALOGD(fmt, ##__VA_ARGS__);         \
    } while (0)

#define TLOGI(fmt, ...)                       \
    do {                                      \
        if (TLOG_IS_ENABLED(TLOG_LEVEL_INFO)) \
            ALOGI(fmt, ##__VA_ARGS__);        \
    } while (0)

#define TLOGW(fmt, ...)                       \
    do {                                      \
        if (TLOG_IS_ENABLED(TLOG_LEVEL_WARN)) \
            ALOGW(fmt, ##__VA_ARGS__);        \
    } while (0)

#define TLOGE(fmt, ...)                        \
    do {                                       \
        if (TLOG_IS_ENABLED(TLOG_LEVEL_ERROR)) \
            ALOGE(fmt, ##__VA_ARGS__);         \
    } while (0)

#else

#define TLOGV(fmt, ...) \
    do {                \
    } while (0)
#define TLOGD(fmt, ...) \
    do {                \
    } while (0)
#define TLOGI(fmt, ...) \
    do {                \
    } while (0)
#define TLOGW(fmt, ...) \
    do {                \
    } while (0)
#define TLOGE(fmt, ...) \
    do {                \
    } while (0)

#endif

#define ALOGV_IF_ENABLED(fmt, ...) TLOGV(fmt, ##__VA_ARGS__)
#define ALOGD_IF_ENABLED(fmt, ...) TLOGD(fmt, ##__VA_ARGS__)
#define ALOGI_IF_ENABLED(fmt, ...) TLOGI(fmt, ##__VA_ARGS__)
#define ALOGW_IF_ENABLED(fmt, ...) TLOGW(fmt, ##__VA_ARGS__)
#define ALOGE_IF_ENABLED(fmt, ...) TLOGE(fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* TLOG_H */
