/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include "PerfConfigMeter.h"

#include <chrono>

#include "Meter.h"
#include "TLog.h"

PerfConfigMeter::PerfConfigMeter(const FeatureInfo &fInfo, FeatureState &featureState)
    : MetaMeter(fInfo, featureState), mLastTriggerTime(0) {
    TLOGD("%s: Constructor", LOG_TAG_PERFCONFIG_METER);
}

PerfConfigMeter::~PerfConfigMeter() {
    TLOGD("%s: Destructor", LOG_TAG_PERFCONFIG_METER);
}

bool PerfConfigMeter::writeToDataBase(std::vector<Meter *> meterList) {
    TLOGV("%s: writeToDataBase called with %zu meters", LOG_TAG_PERFCONFIG_METER, meterList.size());

    for (Meter *meter : meterList) {
        if (meter) {
            TLOGV("%s: Processing meter: %s", LOG_TAG_PERFCONFIG_METER,
                  meter->getMeterName().c_str());
        }
    }

    return true;
}

void PerfConfigMeter::logTriggerEvent(const AsyncTriggerPayload &data) {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();

    if (mLastPackageName != data.appName) {
        TLOGI("%s: Package changed: %s -> %s", LOG_TAG_PERFCONFIG_METER, mLastPackageName.c_str(),
              data.appName.c_str());
        mLastPackageName = data.appName;
    }

    mLastTriggerTime = now;
}
