/******************************************************************************
  @file    PowerOptDebugLog.h
  @brief   Implementation of memory performance module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef PWROPT_DEBUG_LOG_H
#define PWROPT_DEBUG_LOG_H

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ANDR-PWR-OPT"

#include <log/log.h>
#include <cutils/trace.h>

#ifdef QC_DEBUG

/*
 * ALOGV is currently NOP in dev builds - so map DEBUGV also to ALOGI to get the logs when
 * debug is enabled
 */
#define DEBUGV(t,x,...) ALOGI("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define DEBUGI(t,x,...) ALOGI("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define DEBUGD(t,x,...) ALOGD("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define DEBUGW(t,x,...) ALOGW("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define DEBUGE(t,x,...) ALOGE("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);

#else

#define DEBUGV(t,x,...)
#define DEBUGI(t,x,...)
#define DEBUGD(t,x,...)
#define DEBUGW(t,x,...) ALOGW("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define DEBUGE(t,x,...) ALOGE("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);

#endif /*  QC_DEBUG */
#endif

