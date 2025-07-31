/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include "PerfConfigFeature.h"

#include <cutils/properties.h>

#include <fstream>
#include <sstream>

#include "ConfigManager.h"
#include "FpsManager.h"
#include "GlobalFpsMonitor.h"
#include "MatchingAlgorithm.h"
#include "PerfConfigMeter.h"
#include "PerfConfigTypes.h"
#include "PerfLockAction.h"
#include "TLog.h"
#include "VendorIPerf.h"
#include "client.h"

using namespace std;

#undef LOG_TAG
#define LOG_TAG "PERF-CONFIG-FEATURE"

FeatureRegistry<PerfConfigFeature> PerfConfigFeature::registerFeature(PERFCONFIG_FEATURE_NAME);

PerfConfigFeature::PerfConfigFeature(FeatureInfo currFeatureInfo)
    : Feature(currFeatureInfo), mConfigManager(nullptr), mDebounceCounter(0) {
    TLogInit();

    if (!initConfigManager()) {
        TLOGE("Failed to initialize ConfigManager");
        Disable();
        return;
    }
    mMetaMeter = new (std::nothrow) PerfConfigMeter(mfeatureInfo, mFeatureState);
    if (!mMetaMeter) {
        TLOGE("Failed to create MetaMeter");
        Disable();
        return;
    }

    mAlgorithm = new (std::nothrow) MatchingAlgorithm(mFeatureState);
    if (!mAlgorithm) {
        TLOGE("Failed to create Algorithm");
        Disable();
        return;
    }

    mAction = &PerfLockAction::getInstance();
    if (!mAction) {
        TLOGE("Failed to get Action");
        Disable();
        return;
    }

    TLOGI("PerfConfigFeature initialized successfully");
}

PerfConfigFeature::~PerfConfigFeature() {
    {
        std::lock_guard<std::mutex> lock(mDebounceMutex);
        mPackageStates.clear();

        for (auto &pair : mPackagePerfStates) {
            if (pair.second.fpsMonitoringActive) {
                GlobalFpsMonitor::getInstance().unregisterCallback(pair.first);
            }
            if (pair.second.perfLockHandle > 0) {
                perf_lock_rel(pair.second.perfLockHandle);
            }
        }
        mPackagePerfStates.clear();
    }
    delete mMetaMeter;
    mMetaMeter = nullptr;
    delete mAlgorithm;
    mAlgorithm = nullptr;
    TLOGI("PerfConfigFeature destroyed");
}

bool PerfConfigFeature::initConfigManager() {
    mConfigManager = std::make_unique<perfconfig::ConfigManager>();

    if (!mConfigManager->loadConfig("perf_config.xml")) {
        return false;
    }

    return true;
}

perfconfig::ConfigQueryResult PerfConfigFeature::queryPerfConfig(const std::string &packageName,
                                                                 const std::string &activityName,
                                                                 const std::string &fpsValue) {
    if (!mConfigManager) {
        perfconfig::ConfigQueryResult emptyResult;
        return emptyResult;
    }
    return mConfigManager->findConfig(packageName, activityName, fpsValue);
}

void PerfConfigFeature::onReceiveAsyncData(const AsyncTriggerPayload &payload) {
    TLOGI("Received event: %s for app: %s", getHintName(payload.hintID), payload.appName.c_str());

    handleEventWithDebounce(payload);
}

void PerfConfigFeature::runAsync(AsyncTriggerPayload payload) {
    TLOGI("=== PerfConfig runAsync === hintID: 0x%x (%s), appName: %s", payload.hintID,
          getHintName(payload.hintID), payload.appName.c_str());

    if (!isEnabled() || !mConfigManager || !mConfigManager->isLoaded()) {
        return;
    }
    Feature::runAsync(payload);

    onReceiveAsyncData(payload);
}

std::string PerfConfigFeature::getConfigIdentifier(const std::string &packageName,
                                                   const std::string &activityName,
                                                   const std::string &fpsValue) {
    auto specific = queryPerfConfig(packageName, activityName, fpsValue);
    if (specific.found) {
        return activityName;
    }

    auto common = queryPerfConfig(packageName, "common", fpsValue);
    if (common.found) {
        return "common";
    }

    return "";
}

perfconfig::ConfigQueryResult PerfConfigFeature::getBestConfig(const std::string &packageName,
                                                               const std::string &activityName,
                                                               const std::string &fpsValue) {
    auto specific = queryPerfConfig(packageName, activityName, fpsValue);
    if (specific.found) {
        return specific;
    }

    return queryPerfConfig(packageName, "common", fpsValue);
}

void PerfConfigFeature::smartApplyPerfLock(const std::string &packageName,
                                           const std::string &activityName,
                                           const std::string &fpsValue) {
    string newConfigIdentifier = getConfigIdentifier(packageName, activityName, fpsValue);
    auto &perfState = mPackagePerfStates[packageName];

    if (perfState.currentConfigIdentifier == newConfigIdentifier) {
        TLOGD("Same config identifier for %s (%s), skipping", packageName.c_str(),
              newConfigIdentifier.c_str());
        return;
    }

    if (perfState.perfLockHandle > 0) {
        perf_lock_rel(perfState.perfLockHandle);
        perfState.perfLockHandle = -1;
        TLOGD("Released PerfLock for %s (old: %s)", packageName.c_str(),
              perfState.currentConfigIdentifier.c_str());
    }

    if (!newConfigIdentifier.empty()) {
        auto config = getBestConfig(packageName, activityName, fpsValue);
        if (config.found && !config.perfLock.opcodes.empty()) {
            perfState.perfLockHandle =
                perf_lock_acq(0, 0, const_cast<int *>(config.perfLock.opcodes.data()),
                              config.perfLock.opcodes.size());

            if (perfState.perfLockHandle > 0) {
                perfState.lastAppliedConfigStr = generateConfigString(config);
                TLOGI("Applied new PerfLock for %s (handle=%d, config=%s)", packageName.c_str(),
                      perfState.perfLockHandle, newConfigIdentifier.c_str());
            } else {
                TLOGE("Failed to acquire PerfLock for %s", packageName.c_str());
            }
        }
    }

    perfState.currentConfigIdentifier = newConfigIdentifier;
}

void PerfConfigFeature::releasePackagePerfLock(const std::string &packageName) {
    auto it = mPackagePerfStates.find(packageName);
    if (it != mPackagePerfStates.end() && it->second.perfLockHandle > 0) {
        perf_lock_rel(it->second.perfLockHandle);
        TLOGI("Released PerfLock for %s (handle=%d)", packageName.c_str(),
              it->second.perfLockHandle);
        it->second.perfLockHandle = -1;
        it->second.currentConfigIdentifier.clear();
        it->second.lastAppliedConfigStr.clear();
    }
}

void PerfConfigFeature::startFpsMonitoring(const std::string &packageName,
                                           const std::string &activityName) {
    auto &perfState = mPackagePerfStates[packageName];

    if (!perfState.fpsMonitoringActive) {
        // Register FPS change callback
        GlobalFpsMonitor::getInstance().registerCallback(
            packageName,
            [this, packageName, activityName](float fps) { this->onFpsChanged(packageName, fps); });

        perfState.fpsMonitoringActive = true;
        TLOGI("Started FPS monitoring for %s::%s", packageName.c_str(), activityName.c_str());
    }
}

void PerfConfigFeature::stopFpsMonitoring(const std::string &packageName) {
    auto it = mPackagePerfStates.find(packageName);
    if (it != mPackagePerfStates.end() && it->second.fpsMonitoringActive) {
        GlobalFpsMonitor::getInstance().unregisterCallback(packageName);
        it->second.fpsMonitoringActive = false;
        it->second.currentFpsCategory.clear();
        TLOGI("Stopped FPS monitoring for %s", packageName.c_str());
    }
}

void PerfConfigFeature::onFpsChanged(const std::string &packageName, float newFps) {
    std::lock_guard<std::mutex> lock(mDebounceMutex);

    auto it = mPackagePerfStates.find(packageName);
    if (it == mPackagePerfStates.end() || !it->second.fpsMonitoringActive) {
        return;
    }

    auto &perfState = it->second;
    std::string newFpsCategory = FpsManager::getFpsCategory(newFps);

    if (!perfState.currentFpsCategory.empty()) {
        if (!FpsManager::shouldSwitchFpsConfig(perfState.currentFpsCategory, newFpsCategory,
                                               perfState.lastValidFps, newFps)) {
            TLOGV("FPS debounce: %s staying at %s (%.1f -> %.1f)", packageName.c_str(),
                  perfState.currentFpsCategory.c_str(), perfState.lastValidFps, newFps);
            return;
        }
    }

    perfState.lastValidFps = newFps;
    perfState.currentFpsCategory = newFpsCategory;

    TLOGD("FPS changed for %s: %.1f -> category %s", packageName.c_str(), newFps,
          newFpsCategory.c_str());

    std::string activityName = perfState.currentConfigIdentifier;
    if (activityName == "common") {
        activityName = "common";
    }

    applyFpsBasedConfig(packageName, activityName, newFps);
}

void PerfConfigFeature::applyFpsBasedConfig(const std::string &packageName,
                                            const std::string &activityName, float fps) {
    std::string fpsCategory = FpsManager::getFpsCategory(fps);

    smartApplyPerfLock(packageName, activityName, fpsCategory);

    TLOGD("Applied FPS-based config for %s::%s (fps=%.1f, category=%s)", packageName.c_str(),
          activityName.c_str(), fps, fpsCategory.c_str());
}

void PerfConfigFeature::handleEventWithDebounce(const AsyncTriggerPayload &payload) {
    string packageName = extractPackageName(payload.appName);
    if (packageName.empty()) {
        TLOGE("Empty package name, skipping");
        return;
    }

    std::lock_guard<std::mutex> lock(mDebounceMutex);

    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();

    if (isRapidSwitch(packageName, currentTime)) {
        TLOGD("Rapid switch detected for %s, extending debounce", packageName.c_str());
    }

    perfconfig::DebounceState &state = mPackageStates[packageName];
    state.packageName = packageName;
    state.lastEventTime = currentTime;

    if (state.timerActive) {
        state.timerActive = false;
    }

    state.pendingEventType = payload.hintID;
    state.timerActive = true;

    uint32_t timeout = getDebounceTimeout(payload.hintID);

    std::thread([this, packageName, payload, timeout]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
        executeDelayedPerfLock(packageName, payload.hintID, payload);
    }).detach();

    TLOGD("Debounce timer started for %s, event %s, timeout %ums", packageName.c_str(),
          getHintName(payload.hintID), timeout);
}

void PerfConfigFeature::executeDelayedPerfLock(const std::string &packageName, uint32_t eventType,
                                               AsyncTriggerPayload payload) {
    std::lock_guard<std::mutex> lock(mDebounceMutex);

    auto it = mPackageStates.find(packageName);
    if (it == mPackageStates.end() || !it->second.timerActive ||
        it->second.pendingEventType != eventType) {
        TLOGD("Debounce timer cancelled for %s", packageName.c_str());
        return;
    }

    it->second.timerActive = false;

    switch (eventType) {
        case VENDOR_HINT_ACTIVITY_START:
        case VENDOR_HINT_ACTIVITY_RESUME:
            handleActivityStartOrResume(payload);
            break;
        case VENDOR_HINT_ACTIVITY_PAUSE:
            handleActivityPause(payload);
            break;
        default:
            TLOGW("Unknown event type: %u", eventType);
            break;
    }

    TLOGD("Debounce timer executed for %s, event %s", packageName.c_str(), getHintName(eventType));
}

bool PerfConfigFeature::isRapidSwitch(const std::string &packageName, uint64_t currentTime) {
    auto it = mPackageStates.find(packageName);
    if (it != mPackageStates.end()) {
        uint64_t timeDiff = currentTime - it->second.lastEventTime;
        return timeDiff < perfconfig::RAPID_SWITCH_THRESHOLD_MS;
    }
    return false;
}

bool PerfConfigFeature::isSameConfig(const std::string &packageName, const std::string &configStr) {
    auto it = mPackageStates.find(packageName);
    if (it != mPackageStates.end()) {
        return it->second.lastAppliedConfig == configStr;
    }
    return false;
}

std::string PerfConfigFeature::generateConfigString(const perfconfig::ConfigQueryResult &result) {
    std::string configStr = result.packageName + "|" + result.activityName + "|" + result.fpsValue;
    for (int opcode : result.perfLock.opcodes) {
        configStr += "|" + std::to_string(opcode);
    }
    return configStr;
}

uint32_t PerfConfigFeature::getDebounceTimeout(uint32_t eventType) {
    switch (eventType) {
        case VENDOR_HINT_ACTIVITY_PAUSE:
            return perfconfig::PAUSE_DEBOUNCE_MS;
        case VENDOR_HINT_ACTIVITY_RESUME:
            return perfconfig::RESUME_DEBOUNCE_MS;
        case VENDOR_HINT_ACTIVITY_START:
            return perfconfig::START_DEBOUNCE_MS;
        default:
            return perfconfig::RESUME_DEBOUNCE_MS;
    }
}

void PerfConfigFeature::handleActivityStartOrResume(const AsyncTriggerPayload &payload) {
    string packageName = extractPackageName(payload.appName);
    string activityName = extractActivityNameFromPayload(payload.appName);
    string fpsValue = "common";

    smartApplyPerfLock(packageName, activityName, fpsValue);

    startFpsMonitoring(packageName, activityName);

    auto config = getBestConfig(packageName, activityName, fpsValue);
    if (config.found) {
        mPackageStates[packageName].lastAppliedConfig = generateConfigString(config);
        TLOGI(
            "Applied START/RESUME perflock for %s::%s (fps:%s) + started FPS "
            "monitoring",
            packageName.c_str(), activityName.c_str(), fpsValue.c_str());
    } else {
        TLOGD("No config found for %s::%s (fps:%s)", packageName.c_str(), activityName.c_str(),
              fpsValue.c_str());
    }
}

void PerfConfigFeature::handleActivityPause(const AsyncTriggerPayload &payload) {
    string packageName = extractPackageName(payload.appName);

    stopFpsMonitoring(packageName);
    releasePackagePerfLock(packageName);

    auto it = mPackageStates.find(packageName);
    if (it != mPackageStates.end()) {
        it->second.lastAppliedConfig.clear();
    }

    TLOGI("Released perflock for %s + stopped FPS monitoring", packageName.c_str());
}

string PerfConfigFeature::extractPackageName(const string &fullAppInfo) const {
    size_t separatorPos = fullAppInfo.find('/');
    if (separatorPos != string::npos) {
        return fullAppInfo.substr(0, separatorPos);
    }
    return fullAppInfo;
}

string PerfConfigFeature::extractActivityNameFromPayload(const string &fullAppInfo) const {
    size_t separatorPos = fullAppInfo.find('/');
    if (separatorPos != string::npos && separatorPos + 1 < fullAppInfo.length()) {
        string fullActivityName = fullAppInfo.substr(separatorPos + 1);

        size_t lastDotPos = fullActivityName.find_last_of('.');
        if (lastDotPos != string::npos && lastDotPos + 1 < fullActivityName.length()) {
            return fullActivityName.substr(lastDotPos + 1);
        }

        return fullActivityName;
    }

    return "common";
}

string PerfConfigFeature::extractActivityName(const string &packageName) const {
    return "common";
}

const char *PerfConfigFeature::getHintName(uint32_t hintID) {
    switch (hintID) {
        case VENDOR_HINT_ACTIVITY_START:
            return "ACTIVITY_START";
        case VENDOR_HINT_ACTIVITY_RESUME:
            return "ACTIVITY_RESUME";
        case VENDOR_HINT_ACTIVITY_PAUSE:
            return "ACTIVITY_PAUSE";
        default:
            return "UNKNOWN";
    }
}
