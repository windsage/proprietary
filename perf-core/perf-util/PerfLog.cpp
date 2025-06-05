/******************************************************************************
  @file  PerfLog.cpp
  @brief Implementation of Dynamic logging API

  QLOGL Debug logs are controlled by property value vendor.debug.trace.perf.level
  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define LOG_TAG "ANDR-PERF-LOG"
#include "PerfLog.h"
#include <log/log.h>
#include <string>
#include <stdint.h>
#include <cutils/properties.h>

#define DEBUG_PROP_NAME "vendor.debug.trace.perf.level"

static uint8_t PerfLogLevel = 0;

uint8_t getPerfLogLevel() {
    return PerfLogLevel;
}

int8_t PerfLogInit() {
    char debug_prop[PROPERTY_VALUE_MAX];

    if (property_get(DEBUG_PROP_NAME, debug_prop, "0") > 0) {
        PerfLogLevel = std::atoi(debug_prop);
        if((PerfLogLevel <= QLOG_LEVEL_MIN) || (PerfLogLevel >= QLOG_LEVEL_MAX)) {
            PerfLogLevel = 0;
            ALOGE("perf log level is misconfigured");
            return -1;
        }
        ALOGI("PerfLogLevel: %" PRIu8, PerfLogLevel);
    } else {
        ALOGE("Could not find property: %s", DEBUG_PROP_NAME);
        return -1;
    }
    return 0;
}
