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
#include "DisplayRateManager.h"
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

    // 新增成员变量
    bool mDisplayRateEnabled;                     // Display Rate功能是否启用
    std::string mCurrentPackageForRateMonitor;    // 当前监控刷新率的包名
    std::mutex mDisplayRateMutex;                 // Display Rate操作保护

    // 新增方法声明
    bool initDisplayRateManager();
    void onDisplayRateChanged(int newRate);
    std::string selectConfigByDisplayRate(const std::string &packageName,
                                          const std::string &activityName, int displayRate);
    bool shouldUseDisplayRateForPackage(const std::string &packageName,
                                        const std::string &activityName);
    void dumpDisplayRateStatus() const;

    void onDisplayRateChangedSafe(int newRate);
    void applyRateConfigAsync(const std::string &packageName, const std::string &activityName,
                              const std::string &rateConfigValue);

    // 核心功能方法
    bool initConfigManager();

    // 配置查询和选择方法
    perfconfig::ConfigQueryResult queryPerfConfig(const std::string &packageName,
                                                  const std::string &activityName,
                                                  const std::string &fpsValue);

    // 修改：支持智能配置选择
    std::string getConfigIdentifier(const std::string &packageName, const std::string &activityName,
                                    const std::string &defaultValue);
    perfconfig::ConfigQueryResult getBestConfig(const std::string &packageName,
                                                const std::string &activityName,
                                                const std::string &preferredValue);

    // 新增：根据Activity配置类型选择初始配置值
    std::string getInitialConfigValue(const std::string &packageName,
                                      const std::string &activityName);

    // 性能锁管理方法
    void smartApplyPerfLock(const std::string &packageName, const std::string &activityName,
                            const std::string &configValue);
    void releasePackagePerfLock(const std::string &packageName);

    // FPS监控和处理方法
    void startFpsMonitoring(const std::string &packageName, const std::string &activityName);
    void stopFpsMonitoring(const std::string &packageName);
    void onFpsChanged(const std::string &packageName, float newFps);

    // 修改：支持基于配置值的应用
    void applyFpsBasedConfig(const std::string &packageName, const std::string &activityName,
                             const std::string &configValue);

    // 重载：向后兼容的FPS方法
    void applyFpsBasedConfig(const std::string &packageName, const std::string &activityName,
                             float fps);

    // 新增：FPS到Rate值的映射
    std::string mapFpsToRateValue(float fps) const;

    // 事件处理方法
    void handleEventWithDebounce(const AsyncTriggerPayload &payload);
    void executeDelayedPerfLock(const std::string &packageName, uint32_t eventType,
                                const AsyncTriggerPayload &payload);

    void handleActivityStartOrResume(const AsyncTriggerPayload &payload);
    void handleActivityPause(const AsyncTriggerPayload &payload);

    // 防抖逻辑方法
    bool isRapidSwitch(const std::string &packageName, uint64_t currentTime);
    uint32_t getDebounceTimeout(uint32_t hintID);

    // 工具方法
    std::string extractPackageName(const std::string &fullAppInfo) const;
    std::string extractActivityNameFromPayload(const std::string &fullAppInfo) const;
    std::string extractActivityName(const std::string &packageName) const;

    // 新增：生成配置字符串（用于调试和日志）
    std::string generateConfigString(const perfconfig::ConfigQueryResult &config);

    const char *getHintName(uint32_t hintID);
};

#endif /* PERFCONFIGFEATURE_H */
