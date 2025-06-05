/******************************************************************************
  @file    ioctl_fallthrough.cpp
  @brief   Dummy implementation of ioctl call made to display driver

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define LOG_TAG "ANDR-PERF-IOCTL-FALLTHROUGH"

#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "ioctl.h"

#include "PerfLog.h"

int early_wakeup_ioctl(int connectorId) {
    QLOGV(LOG_TAG, "drmIOCTLLib fallthrough connectorId: %d", connectorId);
    (void)connectorId;
    return 0;
}
