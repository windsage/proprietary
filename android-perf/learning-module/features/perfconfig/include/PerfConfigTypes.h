/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PERFCONFIGTYPES_H
#define PERFCONFIGTYPES_H

#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace perfconfig {

struct PerfLock {
    std::vector<int> opcodes;
    int timeout = 0;

    bool isEmpty() const { return opcodes.empty(); }
    void clear() {
        opcodes.clear();
        timeout = 0;
    }
};

struct FpsConfig {
    std::string fpsValue;
    PerfLock perfLock;

    void clear() {
        fpsValue.clear();
        perfLock.clear();
    }
};

struct ActivityConfig {
    std::string activityName;
    std::unordered_map<std::string, FpsConfig> fpsConfigs;

    void clear() {
        activityName.clear();
        fpsConfigs.clear();
    }
};

struct PackageConfig {
    std::string packageName;
    std::unordered_map<std::string, ActivityConfig> activities;

    void clear() {
        packageName.clear();
        activities.clear();
    }
};

struct ConfigQueryResult {
    bool found = false;
    PerfLock perfLock;
    std::string packageName;
    std::string activityName;
    std::string fpsValue;
    std::string source;

    void clear() {
        found = false;
        perfLock.clear();
        packageName.clear();
        activityName.clear();
        fpsValue.clear();
        source.clear();
    }
};

struct DebounceState {
    std::string packageName;
    std::string lastAppliedConfig;
    uint64_t lastEventTime;
    std::thread::id debounceTimerId;
    uint32_t pendingEventType;
    bool timerActive;

    DebounceState() : lastEventTime(0), pendingEventType(0), timerActive(false) {}
};

struct PackagePerfState {
    std::string currentConfigIdentifier;

    int perfLockHandle;
    std::string lastAppliedConfigStr;
    float lastValidFps;
    std::string currentFpsCategory;
    bool fpsMonitoringActive;

    PackagePerfState() : perfLockHandle(-1), lastValidFps(-1.0f), fpsMonitoringActive(false) {}
};

enum DebounceTimeouts {
    PAUSE_DEBOUNCE_MS = 50,
    RESUME_DEBOUNCE_MS = 200,
    START_DEBOUNCE_MS = 100,
    RAPID_SWITCH_THRESHOLD_MS = 100
};

}    // namespace perfconfig

#endif /* PERFCONFIGTYPES_H */
