/******************************************************************************
  @file    PerfLog.h
  @brief   Common library for everything related to logging

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __PERF_LOG__
#define __PERF_LOG__

#include <inttypes.h>

#ifndef LOG_TAG
#define LOG_TAG "ANDR-PERF"
#endif

#include <log/log.h>

/*
 * QLOGL API
 */
enum {
    QLOG_LEVEL_MIN = 0,
    /*Valid Log level starts from here*/
    QLOG_WARNING,
    QLOG_L1,
    QLOG_L2,
    QLOG_L3,
    /*Valid Log level ends here*/
    QLOG_LEVEL_MAX
};

int8_t PerfLogInit();
uint8_t getPerfLogLevel();

#define QLOGL(tag, level, fmt, ...)                                \
    ALOGD_IF((level <= getPerfLogLevel()), "%s: %s() %d: " fmt "",  \
                    tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define QLOGE(t,x, ...) ALOGE("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define LMDB_QLOGE(x, ...)  ALOGE("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__)

#if QC_DEBUG
#define QLOGW(t,x, ...)  ALOGW("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define QLOGV(t,x, ...)  ALOGV("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define QLOGI(t,x, ...)  ALOGI("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define QLOGD(t,x, ...)  ALOGD("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define QCLOGE(t,x, ...) ALOGE("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__)

/*
 * ALOGV is currently NOP in dev builds - so map DEBUGV also to ALOGI to get the logs when
 * debug is enabled
 */
#define DEBUGV(t, x, ...) ALOGI("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define DEBUGI(t, x, ...) ALOGI("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define DEBUGD(t, x, ...) ALOGD("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define DEBUGW(t, x, ...) ALOGW("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define DEBUGE(t, x, ...) ALOGE("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);

#else
#define QLOGW(t,x, ...)
#define QLOGV(t,x, ...)
#define QLOGI(t,x, ...)
#define QLOGD(t,x, ...)
#define QCLOGE(t,x, ...)

#define DEBUGV(t, x, ...)
#define DEBUGI(t, x, ...)
#define DEBUGD(t, x, ...)
#define DEBUGW(t, x, ...)
#define DEBUGE(t, x, ...) ALOGE("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);

#endif


#ifdef LMDB_DATABASE_DEBUG
#define LMDB_DATATABLE_DEBUG
#define BUSY_LIST_DEBUG true

#define LMDB_QLOGW(x, ...)  ALOGW("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define LMDB_QLOGV(x, ...)  ALOGV("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define LMDB_QLOGI(x, ...)  ALOGI("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define LMDB_QLOGD(x, ...)  ALOGD("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__)

#define QLOGV_DBG(x, ...)               LMDB_QLOGV(x, __VA_ARGS__)

#else
#define BUSY_LIST_DEBUG false

#define LMDB_QLOGW(x, ...)
#define LMDB_QLOGV(x, ...)
#define LMDB_QLOGI(x, ...)
#define LMDB_QLOGD(x, ...)

#define QLOGV_DBG(x, ...)
#endif //LMDB_DATABASE_DEBUG

#define LMDB_DEBUG_SQL_ERR(x, ...)      LMDB_QLOGE(x, __VA_ARGS__)
#define LMDB_DEBUG_SQL(x, ...)          QLOGV_DBG(x, __VA_ARGS__)
#define LMDB_DEBUG_SQL_DEBUG(x, ...)    QLOGV_DBG(x, __VA_ARGS__)
#define LMDB_DEBUG_SQL_NODEBUG(x, ...)

#define LMDB_DEBUG_LIB(x, ...)          LMDB_QLOGD(x, __VA_ARGS__)
#define LMDB_DEBUG_UTEST(x, ...)        LMDB_QLOGD(x, __VA_ARGS__)
#define LMDB_DEBUG_LIB_DETAILED(x, ...) QLOGV_DBG(x, __VA_ARGS__)

#define LMDB_DEBUG_SQL_SHADOW_RD
#define LMDB_DEBUG_SQL_SHADOW_WR
#define LMDB_DEBUG_SQL_SHADOW_DEL
#define LMDB_DEBUG_SQL_SHADOW_COL_LIST

#undef SLOGW
#undef SLOGI
#undef SLOGV
#undef SLOGE

#if QTI_DEBUG
#define SLOGW(...)    ALOGW(__VA_ARGS__)
#define SLOGI(...)    ALOGI(__VA_ARGS__)
#define SLOGV(...)    ALOGV(__VA_ARGS__)
#define SLOGE(...)    ALOGE(__VA_ARGS__)
#else
#define SLOGW(...)
#define SLOGI(...)
#define SLOGV(...)
#define SLOGE(...)    ALOGE(__VA_ARGS__)
#endif

#endif
