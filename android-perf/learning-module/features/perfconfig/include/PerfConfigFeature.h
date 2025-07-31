/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PERFCONFIGFEATURE_H
#define PERFCONFIGFEATURE_H

#include <chrono>
#include <memory>
#include <thread>
#include <unordered_map>

#include "ConfigManager.h"
#include "Feature.h"
#include "FeatureRegistry.h"
#include "PerfConfigTypes.h"
#include "PerfLockAction.h"

#define LOG_TAG_PERFCONFIG "PERF-CONFIG-FEATURE"
#define PERFCONFIG_FEATURE_NAME "PerfConfigFeature"

namespace perfconfig {
class ConfigManager;
}

class PerfConfigFeature : public Feature {
public:
    explicit PerfConfigFeature(FeatureInfo currFeatureInfo);
    ~PerfConfigFeature();

    void onReceiveAsyncData(const AsyncTriggerPayload &payload);

    static FeatureRegistry<PerfConfigFeature> registerFeature;

    void runAsync(AsyncTriggerPayload payload) override;

private:
    std::unique_ptr<perfconfig::ConfigManager> mConfigManager;
    PerfLockAction *mAction;

    std::unordered_map<std::string, perfconfig::DebounceState> mPackageStates;
    std::unordered_map<std::string, perfconfig::PackagePerfState> mPackagePerfStates;
    std::mutex mDebounceMutex;
    std::atomic<uint32_t> mDebounceCounter;

    bool initConfigManager();
    perfconfig::ConfigQueryResult queryPerfConfig(const std::string &packageName,
                                                  const std::string &activityName,
                                                  const std::string &fpsValue);

    std::string getConfigIdentifier(const std::string &packageName, const std::string &activityName,
                                    const std::string &fpsValue);
    perfconfig::ConfigQueryResult getBestConfig(const std::string &packageName,
                                                const std::string &activityName,
                                                const std::string &fpsValue);
    void smartApplyPerfLock(const std::string &packageName, const std::string &activityName,
                            const std::string &fpsValue);
    void releasePackagePerfLock(const std::string &packageName);

    void startFpsMonitoring(const std::string &packageName, const std::string &activityName);
    void stopFpsMonitoring(const std::string &packageName);
    void onFpsChanged(const std::string &packageName, float newFps);
    void applyFpsBasedConfig(const std::string &packageName, const std::string &activityName,
                             float fps);

    void handleEventWithDebounce(const AsyncTriggerPayload &payload);
    void processDebounceEvent(const std::string &packageName, uint32_t eventType,
                              const AsyncTriggerPayload &payload);
    bool isRapidSwitch(const std::string &packageName, uint64_t currentTime);
    bool isSameConfig(const std::string &packageName, const std::string &configStr);
    std::string generateConfigString(const perfconfig::ConfigQueryResult &result);
    uint32_t getDebounceTimeout(uint32_t eventType);
    void executeDelayedPerfLock(const std::string &packageName, uint32_t eventType,
                                AsyncTriggerPayload payload);

    void handleActivityStartOrResume(const AsyncTriggerPayload &payload);
    void handleActivityPause(const AsyncTriggerPayload &payload);

    std::string extractPackageName(const std::string &fullAppInfo) const;
    std::string extractActivityNameFromPayload(const std::string &fullAppInfo) const;
    std::string extractActivityName(const std::string &packageName) const;
    const char *getHintName(uint32_t hintID);
};

#endif /* PERFCONFIGFEATURE_H */
