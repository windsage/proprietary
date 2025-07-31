/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PERFCONFIGMETER_H
#define PERFCONFIGMETER_H

#include "MetaMeter.h"

#define LOG_TAG_PERFCONFIG_METER "PERF-CONFIG-METER"

class PerfConfigMeter : public MetaMeter {
public:
    PerfConfigMeter(const FeatureInfo &fInfo, FeatureState &featureState);
    ~PerfConfigMeter();
    bool writeToDataBase(std::vector<Meter *> meterList) override;

private:
    std::string mLastPackageName;
    std::string mLastActivityName;
    uint64_t mLastTriggerTime;
    void logTriggerEvent(const AsyncTriggerPayload &data);
};

#endif /* PERFCONFIGMETER_H */
