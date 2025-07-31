/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include "TLog.h"

#include <cutils/properties.h>

#include <cstdlib>

int gTLogRuntimeLevel = TLOG_LEVEL_INFO;

void TLogInit() {
    char prop_value[PROPERTY_VALUE_MAX];

    if (property_get("vendor.perf.config.log.level", prop_value, NULL) > 0) {
        int level = atoi(prop_value);
        if (level >= TLOG_LEVEL_VERBOSE && level <= TLOG_LEVEL_OFF) {
            gTLogRuntimeLevel = level;
        }
    }

#if TLOG_COMPILE_LEVEL <= TLOG_LEVEL_DEBUG
    TLOGD("TLog initialized - Compile Level: %d, Runtime Level: %d", TLOG_COMPILE_LEVEL,
          gTLogRuntimeLevel);
#endif
}
