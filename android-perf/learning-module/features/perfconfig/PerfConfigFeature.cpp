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
    : Feature(currFeatureInfo),
      mConfigManager(nullptr),
      mDebounceCounter(0),
      mDisplayRateEnabled(false) {
    TLogInit();

    if (!initConfigManager()) {
        TLOGE("Failed to initialize ConfigManager");
        Disable();
        return;
    }

    // 初始化Display Rate Manager
    if (!initDisplayRateManager()) {
        TLOGW("Display Rate Manager initialization failed, continuing without it");
        mDisplayRateEnabled = false;
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

    TLOGI("PerfConfigFeature initialized successfully (DisplayRate: %s)",
          mDisplayRateEnabled ? "enabled" : "disabled");
}

// 修改析构函数
PerfConfigFeature::~PerfConfigFeature() {
    // 停止Display Rate监控 - 移除异常处理
    if (mDisplayRateEnabled) {
        std::lock_guard<std::mutex> lock(mDisplayRateMutex);

        perfconfig::DisplayRateManager::getInstance().unregisterRateChangeCallback();
        perfconfig::DisplayRateManager::getInstance().cleanup();
        TLOGD("Display Rate Manager cleaned up");
    }

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

// 新增：Display Rate变化处理
bool PerfConfigFeature::initDisplayRateManager() {
    auto &displayRateManager = perfconfig::DisplayRateManager::getInstance();

    if (!displayRateManager.initialize()) {
        TLOGE("Failed to initialize DisplayRateManager");
        return false;
    }

    // 注册刷新率变化回调 - 使用安全的回调包装
    displayRateManager.registerRateChangeCallback(
        [this](int newRate) {
            // 移除异常处理，改用安全检查
            if (this && mDisplayRateEnabled) {
                this->onDisplayRateChangedSafe(newRate);
            }
        },
        300    // 300ms检查间隔
    );

    TLOGI("DisplayRateManager initialized and callback registered");
    mDisplayRateEnabled = true;

    // 输出初始状态
    dumpDisplayRateStatus();

    return true;
}

void PerfConfigFeature::onDisplayRateChanged(int newRate) {
    std::lock_guard<std::mutex> lock(mDisplayRateMutex);

    TLOGI("Display rate changed to %d Hz", newRate);

    // 如果当前有活跃的应用在监控，应用新的刷新率配置
    if (!mCurrentPackageForRateMonitor.empty()) {
        std::string packageName = mCurrentPackageForRateMonitor;

        // 获取Activity名称（从perfState中获取）
        std::string activityName = "common";
        {
            std::lock_guard<std::mutex> debounceLock(mDebounceMutex);
            auto it = mPackagePerfStates.find(packageName);
            if (it != mPackagePerfStates.end() && !it->second.currentConfigIdentifier.empty()) {
                std::string configId = it->second.currentConfigIdentifier;
                size_t atPos = configId.find("@");
                if (atPos != std::string::npos) {
                    activityName = configId.substr(0, atPos);
                }
                // 处理可能的嵌套标识符格式
                size_t parenPos = activityName.find("(");
                if (parenPos != std::string::npos) {
                    activityName = activityName.substr(0, parenPos);
                }
            }
        }

        // 检查是否应该使用Display Rate配置
        if (shouldUseDisplayRateForPackage(packageName, activityName)) {
            std::string rateConfigValue =
                selectConfigByDisplayRate(packageName, activityName, newRate);
            if (!rateConfigValue.empty()) {
                TLOGI("Applying rate-based config for %s::%s -> %s (%d Hz)", packageName.c_str(),
                      activityName.c_str(), rateConfigValue.c_str(), newRate);

                // 注意：这里需要临时释放mDisplayRateMutex以避免死锁
                // 因为smartApplyPerfLock可能会获取mDebounceMutex
                std::string tempPackage = packageName;
                std::string tempActivity = activityName;
                std::string tempConfig = rateConfigValue;

                // 使用异步方式应用配置，避免死锁
                std::thread([this, tempPackage, tempActivity, tempConfig]() {
                    try {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));    // 短暂延迟
                        smartApplyPerfLock(tempPackage, tempActivity, tempConfig);
                    } catch (const std::exception &e) {
                        TLOGE("Exception during async perflock apply: %s", e.what());
                    }
                }).detach();

            } else {
                TLOGD("No suitable rate config found for %s::%s with rate %d Hz",
                      packageName.c_str(), activityName.c_str(), newRate);
            }
        } else {
            TLOGV("Package %s::%s doesn't use display rate configs", packageName.c_str(),
                  activityName.c_str());
        }
    } else {
        TLOGV("No active package for rate monitoring");
    }
}

void PerfConfigFeature::onDisplayRateChangedSafe(int newRate) {
    // 替代原来的onDisplayRateChanged，移除所有异常处理
    std::lock_guard<std::mutex> lock(mDisplayRateMutex);

    TLOGI("Display rate changed to %d Hz", newRate);

    if (!mCurrentPackageForRateMonitor.empty()) {
        std::string packageName = mCurrentPackageForRateMonitor;

        // 获取Activity名称
        std::string activityName = "common";
        {
            std::lock_guard<std::mutex> debounceLock(mDebounceMutex);
            auto it = mPackagePerfStates.find(packageName);
            if (it != mPackagePerfStates.end() && !it->second.currentConfigIdentifier.empty()) {
                std::string configId = it->second.currentConfigIdentifier;
                size_t atPos = configId.find("@");
                if (atPos != std::string::npos) {
                    activityName = configId.substr(0, atPos);
                }
                size_t parenPos = activityName.find("(");
                if (parenPos != std::string::npos) {
                    activityName = activityName.substr(0, parenPos);
                }
            }
        }

        // 检查是否应该使用Display Rate配置
        if (shouldUseDisplayRateForPackage(packageName, activityName)) {
            std::string rateConfigValue =
                selectConfigByDisplayRate(packageName, activityName, newRate);
            if (!rateConfigValue.empty()) {
                TLOGI("Applying rate-based config for %s::%s -> %s (%d Hz)", packageName.c_str(),
                      activityName.c_str(), rateConfigValue.c_str(), newRate);

                // 使用安全的异步方式应用配置
                applyRateConfigAsync(packageName, activityName, rateConfigValue);
            } else {
                TLOGD("No suitable rate config found for %s::%s with rate %d Hz",
                      packageName.c_str(), activityName.c_str(), newRate);
            }
        } else {
            TLOGV("Package %s::%s doesn't use display rate configs", packageName.c_str(),
                  activityName.c_str());
        }
    } else {
        TLOGV("No active package for rate monitoring");
    }
}

// 7. 新增：安全的异步配置应用
void PerfConfigFeature::applyRateConfigAsync(const std::string &packageName,
                                             const std::string &activityName,
                                             const std::string &rateConfigValue) {
    // 替代原来的异常+异步线程方式
    std::thread([this, packageName, activityName, rateConfigValue]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));    // 短暂延迟避免死锁

        // 检查对象是否仍然有效（使用原子标志或其他安全机制）
        if (mDisplayRateEnabled) {
            smartApplyPerfLock(packageName, activityName, rateConfigValue);
        }
    }).detach();
}

// 新增：根据显示刷新率选择配置
std::string PerfConfigFeature::selectConfigByDisplayRate(const std::string &packageName,
                                                         const std::string &activityName,
                                                         int displayRate) {
    if (!mConfigManager) {
        TLOGE("ConfigManager not available");
        return "";
    }

    // 标准化刷新率值
    std::string rateValue = perfconfig::DisplayRateManager::normalizeRateValue(displayRate);

    TLOGD("Normalized display rate %d Hz to config value: %s", displayRate, rateValue.c_str());

    // 移除try-catch，直接使用返回值检查
    auto config = mConfigManager->findConfigByType(packageName, activityName, rateValue,
                                                   perfconfig::ConfigType::RATE);
    if (config.found) {
        TLOGD("Found rate config for %s::%s@%s (opcodes: %zu)", packageName.c_str(),
              activityName.c_str(), rateValue.c_str(), config.perfLock.opcodes.size());
        return rateValue;
    }

    // 如果没有找到对应的rate配置，尝试common
    if (rateValue != "common") {
        auto commonConfig = mConfigManager->findConfigByType(packageName, activityName, "common",
                                                             perfconfig::ConfigType::RATE);
        if (commonConfig.found) {
            TLOGD("Using common rate config for %s::%s (opcodes: %zu)", packageName.c_str(),
                  activityName.c_str(), commonConfig.perfLock.opcodes.size());
            return "common";
        }
    }

    // 最后尝试包级common rate配置
    if (activityName != "common") {
        auto packageCommonConfig = mConfigManager->findConfigByType(packageName, "common", "common",
                                                                    perfconfig::ConfigType::RATE);
        if (packageCommonConfig.found) {
            TLOGD("Using package common rate config for %s (opcodes: %zu)", packageName.c_str(),
                  packageCommonConfig.perfLock.opcodes.size());
            return "common";
        }
    }

    TLOGV("No suitable rate config found for %s::%s with display rate %d", packageName.c_str(),
          activityName.c_str(), displayRate);
    return "";
}

// 新增：判断是否应该使用Display Rate配置
bool PerfConfigFeature::shouldUseDisplayRateForPackage(const std::string &packageName,
                                                       const std::string &activityName) {
    if (!mDisplayRateEnabled || !mConfigManager) {
        return false;
    }

    // 移除try-catch，直接使用返回值检查
    bool hasRateConfig =
        mConfigManager->hasConfigType(packageName, activityName, perfconfig::ConfigType::RATE);
    if (hasRateConfig) {
        TLOGV("Package %s::%s has rate configs, using display rate", packageName.c_str(),
              activityName.c_str());
        return true;
    }

    // 检查包级common配置是否为rate类型
    if (activityName != "common") {
        bool hasCommonRateConfig =
            mConfigManager->hasConfigType(packageName, "common", perfconfig::ConfigType::RATE);
        if (hasCommonRateConfig) {
            TLOGV("Package %s has common rate config, using display rate", packageName.c_str());
            return true;
        }
    }

    return false;
}

// 新增：输出Display Rate状态信息
void PerfConfigFeature::dumpDisplayRateStatus() const {
    if (!mDisplayRateEnabled) {
        TLOGI("Display Rate functionality is disabled");
        return;
    }

    // 移除try-catch，使用直接检查
    auto &manager = perfconfig::DisplayRateManager::getInstance();
    if (manager.isAvailable()) {
        auto rateInfo = manager.getDisplayRateInfo();
        TLOGI("Display Rate Status: %s", rateInfo.toString().c_str());

        auto stats = manager.getStats();
        TLOGI("Display Rate Stats: queries=%u, success=%.1f%%, avgTime=%.1fμs", stats.queryCount,
              stats.getSuccessRate() * 100.0, stats.getAverageQueryTime());
    } else {
        TLOGW("Display Rate Manager is not available");
    }
}

// 修改：Activity启动/恢复处理 - 集成Display Rate功能
void PerfConfigFeature::handleActivityStartOrResume(const AsyncTriggerPayload &payload) {
    string packageName = extractPackageName(payload.appName);
    string activityName = extractActivityNameFromPayload(payload.appName);

    // 设置当前监控的包
    {
        std::lock_guard<std::mutex> lock(mDisplayRateMutex);
        mCurrentPackageForRateMonitor = packageName;
    }

    // 检查是否应该使用Display Rate配置 - 移除异常处理
    if (mDisplayRateEnabled && shouldUseDisplayRateForPackage(packageName, activityName)) {
        // 获取当前显示刷新率
        int currentRate = perfconfig::DisplayRateManager::getInstance().getCurrentDisplayRate();
        if (currentRate > 0) {
            std::string rateConfigValue =
                selectConfigByDisplayRate(packageName, activityName, currentRate);
            if (!rateConfigValue.empty()) {
                TLOGI("Using display rate-based config for %s::%s -> %s (%d Hz)",
                      packageName.c_str(), activityName.c_str(), rateConfigValue.c_str(),
                      currentRate);

                smartApplyPerfLock(packageName, activityName, rateConfigValue);

                // 记录配置应用
                auto config = getBestConfig(packageName, activityName, rateConfigValue);
                if (config.found) {
                    mPackageStates[packageName].lastAppliedConfig = generateConfigString(config);
                    TLOGI(
                        "Applied RATE-based perflock for %s::%s (rate:%s from %d Hz, opcodes:%zu)",
                        packageName.c_str(), activityName.c_str(), rateConfigValue.c_str(),
                        currentRate, config.perfLock.opcodes.size());
                }

                // 启动刷新率监控
                auto &manager = perfconfig::DisplayRateManager::getInstance();
                if (!manager.getStats().monitoringActive) {
                    if (manager.startRateMonitoring()) {
                        TLOGD("Started display rate monitoring for %s", packageName.c_str());
                    } else {
                        TLOGW("Failed to start display rate monitoring");
                    }
                }

                return;    // 使用了display rate配置，不再执行默认逻辑
            } else {
                TLOGD("No suitable rate config found for %s::%s at %d Hz, using default",
                      packageName.c_str(), activityName.c_str(), currentRate);
            }
        } else {
            TLOGW("Failed to get current display rate, falling back to default config");
        }
    }

    // 默认逻辑：使用原有的配置选择方式
    string initialConfigValue = getInitialConfigValue(packageName, activityName);
    smartApplyPerfLock(packageName, activityName, initialConfigValue);

    // 启动FPS监控（如果需要）
    startFpsMonitoring(packageName, activityName);

    // 记录应用的配置
    auto config = getBestConfig(packageName, activityName, initialConfigValue);
    if (config.found) {
        mPackageStates[packageName].lastAppliedConfig = generateConfigString(config);
        TLOGI("Applied DEFAULT perflock for %s::%s (value:%s, type:%s, opcodes:%zu)",
              packageName.c_str(), activityName.c_str(), initialConfigValue.c_str(),
              (config.configType == perfconfig::ConfigType::FPS) ? "fps" : "rate",
              config.perfLock.opcodes.size());
    } else {
        TLOGD("No config found for %s::%s (value:%s)", packageName.c_str(), activityName.c_str(),
              initialConfigValue.c_str());
    }
}

// 修改：Activity暂停处理 - 清理Display Rate监控
void PerfConfigFeature::handleActivityPause(const AsyncTriggerPayload &payload) {
    string packageName = extractPackageName(payload.appName);

    // 如果是当前监控的包，清除监控状态
    {
        std::lock_guard<std::mutex> lock(mDisplayRateMutex);
        if (mCurrentPackageForRateMonitor == packageName) {
            mCurrentPackageForRateMonitor.clear();
            TLOGD("Cleared display rate monitoring for %s", packageName.c_str());
        }
    }

    stopFpsMonitoring(packageName);
    releasePackagePerfLock(packageName);

    auto it = mPackageStates.find(packageName);
    if (it != mPackageStates.end()) {
        it->second.lastAppliedConfig.clear();
    }

    TLOGI("Released perflock for %s + stopped monitoring", packageName.c_str());
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

// 新增：智能配置标识符生成 - 根据Activity实际配置类型选择
std::string PerfConfigFeature::getConfigIdentifier(const std::string &packageName,
                                                   const std::string &activityName,
                                                   const std::string &defaultValue) {
    // 首先检查指定activity是否有配置
    auto specific = queryPerfConfig(packageName, activityName, defaultValue);
    if (specific.found) {
        return activityName + "@" + defaultValue + "(" + specific.source + ")";
    }

    // 检查common配置
    auto common = queryPerfConfig(packageName, "common", defaultValue);
    if (common.found) {
        return "common@" + defaultValue + "(" + common.source + ")";
    }

    return "";
}

// 修改：支持智能配置选择
perfconfig::ConfigQueryResult PerfConfigFeature::getBestConfig(const std::string &packageName,
                                                               const std::string &activityName,
                                                               const std::string &preferredValue) {
    // 优先尝试指定activity和指定值
    auto specific = queryPerfConfig(packageName, activityName, preferredValue);
    if (specific.found) {
        TLOGD("Found specific config: %s", specific.getConfigDescription().c_str());
        return specific;
    }

    // 如果没有找到，尝试common配置
    auto common = queryPerfConfig(packageName, "common", preferredValue);
    if (common.found) {
        TLOGD("Found common config: %s", common.getConfigDescription().c_str());
        return common;
    }

    // 新增：如果指定值没找到，尝试查找该activity的默认配置类型
    if (preferredValue != "common") {
        auto defaultConfig = queryPerfConfig(packageName, activityName, "common");
        if (defaultConfig.found) {
            TLOGD("Found default config: %s", defaultConfig.getConfigDescription().c_str());
            return defaultConfig;
        }

        // 最后尝试包级的common配置
        auto packageCommon = queryPerfConfig(packageName, "common", "common");
        if (packageCommon.found) {
            TLOGD("Found package common config: %s", packageCommon.getConfigDescription().c_str());
            return packageCommon;
        }
    }

    TLOGV("No config found for %s::%s with value %s", packageName.c_str(), activityName.c_str(),
          preferredValue.c_str());

    perfconfig::ConfigQueryResult emptyResult;
    return emptyResult;
}

// 修改：智能性能锁应用 - 支持fps和rate配置
void PerfConfigFeature::smartApplyPerfLock(const std::string &packageName,
                                           const std::string &activityName,
                                           const std::string &configValue) {
    string newConfigIdentifier = getConfigIdentifier(packageName, activityName, configValue);
    auto &perfState = mPackagePerfStates[packageName];

    if (perfState.currentConfigIdentifier == newConfigIdentifier) {
        TLOGD("Same config identifier for %s (%s), skipping", packageName.c_str(),
              newConfigIdentifier.c_str());
        return;
    }

    // 释放旧的性能锁
    if (perfState.perfLockHandle > 0) {
        perf_lock_rel(perfState.perfLockHandle);
        perfState.perfLockHandle = -1;
        TLOGD("Released PerfLock for %s (old: %s)", packageName.c_str(),
              perfState.currentConfigIdentifier.c_str());
    }

    // 应用新的性能锁
    if (!newConfigIdentifier.empty()) {
        auto config = getBestConfig(packageName, activityName, configValue);
        if (config.found && !config.perfLock.opcodes.empty()) {
            perfState.perfLockHandle =
                perf_lock_acq(0, 0, const_cast<int *>(config.perfLock.opcodes.data()),
                              config.perfLock.opcodes.size());

            if (perfState.perfLockHandle > 0) {
                perfState.lastAppliedConfigStr = generateConfigString(config);
                TLOGI("Applied new PerfLock for %s (handle=%d, config=%s, type=%s)",
                      packageName.c_str(), perfState.perfLockHandle, newConfigIdentifier.c_str(),
                      (config.configType == perfconfig::ConfigType::FPS) ? "fps" : "rate");
            } else {
                TLOGE("Failed to acquire PerfLock for %s", packageName.c_str());
            }
        } else {
            TLOGW("No valid config found for %s", packageName.c_str());
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
    }
}

// 新增：根据Activity配置类型选择初始配置值
std::string PerfConfigFeature::getInitialConfigValue(const std::string &packageName,
                                                     const std::string &activityName) {
    // 检查Activity是否有rate配置
    if (mConfigManager &&
        mConfigManager->hasConfigType(packageName, activityName, perfconfig::ConfigType::RATE)) {
        TLOGD("Activity %s::%s uses RATE configs, starting with 'common'", packageName.c_str(),
              activityName.c_str());
        return "common";    // rate配置通常从common开始
    }

    // 检查Activity是否有fps配置
    if (mConfigManager &&
        mConfigManager->hasConfigType(packageName, activityName, perfconfig::ConfigType::FPS)) {
        TLOGD("Activity %s::%s uses FPS configs, starting with 'common'", packageName.c_str(),
              activityName.c_str());
        return "common";    // fps配置也从common开始
    }

    // 如果都没有，检查包级common配置
    TLOGD("Activity %s::%s has no specific configs, using 'common'", packageName.c_str(),
          activityName.c_str());
    return "common";
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

// 修改：FPS变化处理 - 支持rate配置的FPS自适应
void PerfConfigFeature::onFpsChanged(const std::string &packageName, float newFps) {
    auto &perfState = mPackagePerfStates[packageName];

    if (newFps <= 0) {
        TLOGW("Invalid FPS value for %s: %.1f", packageName.c_str(), newFps);
        return;
    }

    std::string newFpsCategory = FpsManager::getFpsCategory(newFps);

    if (perfState.currentFpsCategory == newFpsCategory) {
        return;
    }

    perfState.lastValidFps = newFps;
    perfState.currentFpsCategory = newFpsCategory;

    TLOGD("FPS changed for %s: %.1f -> category %s", packageName.c_str(), newFps,
          newFpsCategory.c_str());

    // 获取当前Activity名称
    std::string activityName = perfState.currentConfigIdentifier;
    if (activityName.find("@") != string::npos) {
        activityName = activityName.substr(0, activityName.find("@"));
    }
    if (activityName.empty()) {
        activityName = "common";
    }

    // 新增：检查配置类型决定如何处理FPS变化
    if (mConfigManager) {
        // 如果使用rate配置，FPS变化应该映射到刷新率档位
        if (mConfigManager->hasConfigType(packageName, activityName,
                                          perfconfig::ConfigType::RATE)) {
            std::string rateValue = mapFpsToRateValue(newFps);
            TLOGD("Mapping FPS %.1f to rate value %s for %s", newFps, rateValue.c_str(),
                  packageName.c_str());
            applyFpsBasedConfig(packageName, activityName, rateValue);
            return;
        }
    }

    // 对于fps配置，使用原有逻辑
    applyFpsBasedConfig(packageName, activityName, newFpsCategory);
}

// 新增：FPS到Rate值的映射
std::string PerfConfigFeature::mapFpsToRateValue(float fps) const {
    // 将FPS映射到刷新率档位
    if (fps >= 110.0f) {
        return "120";    // 120Hz刷新率
    } else if (fps >= 80.0f) {
        return "90";    // 90Hz刷新率
    } else if (fps >= 50.0f) {
        return "60";    // 60Hz刷新率
    } else {
        return "common";    // 通用配置
    }
}

// 修改：基于FPS应用配置 - 支持配置值参数
void PerfConfigFeature::applyFpsBasedConfig(const std::string &packageName,
                                            const std::string &activityName,
                                            const std::string &configValue) {
    smartApplyPerfLock(packageName, activityName, configValue);

    TLOGD("Applied config for %s::%s (value=%s)", packageName.c_str(), activityName.c_str(),
          configValue.c_str());
}

// 重载：向后兼容的FPS方法
void PerfConfigFeature::applyFpsBasedConfig(const std::string &packageName,
                                            const std::string &activityName, float fps) {
    std::string fpsCategory = FpsManager::getFpsCategory(fps);
    applyFpsBasedConfig(packageName, activityName, fpsCategory);
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
                                               const AsyncTriggerPayload &payload) {
    std::lock_guard<std::mutex> lock(mDebounceMutex);

    auto &state = mPackageStates[packageName];
    if (!state.timerActive || state.pendingEventType != eventType) {
        TLOGD("Debounce timer cancelled for %s (event %s)", packageName.c_str(),
              getHintName(eventType));
        return;
    }

    state.timerActive = false;

    TLOGI("Executing debounced event %s for %s", getHintName(eventType), packageName.c_str());

    switch (eventType) {
        case VENDOR_HINT_ACTIVITY_START:
        case VENDOR_HINT_ACTIVITY_RESUME:
            handleActivityStartOrResume(payload);
            break;
        case VENDOR_HINT_ACTIVITY_PAUSE:
            handleActivityPause(payload);
            break;
        default:
            TLOGW("Unknown event type: 0x%x", eventType);
            break;
    }
}

bool PerfConfigFeature::isRapidSwitch(const std::string &packageName, uint64_t currentTime) {
    auto it = mPackageStates.find(packageName);
    if (it == mPackageStates.end()) {
        return false;
    }

    uint64_t timeDiff = currentTime - it->second.lastEventTime;
    return timeDiff < perfconfig::RAPID_SWITCH_THRESHOLD_MS;
}

uint32_t PerfConfigFeature::getDebounceTimeout(uint32_t hintID) {
    switch (hintID) {
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

// 新增：生成配置字符串（用于调试和日志）
std::string PerfConfigFeature::generateConfigString(const perfconfig::ConfigQueryResult &config) {
    if (!config.found) {
        return "none";
    }

    return config.packageName + "::" + config.activityName + "@" + config.configValue + "(" +
           (config.configType == perfconfig::ConfigType::FPS ? "fps" : "rate") +
           ",opcodes:" + std::to_string(config.perfLock.opcodes.size()) + ")";
}

// FPS监控相关方法保持不变
void PerfConfigFeature::startFpsMonitoring(const std::string &packageName,
                                           const std::string &activityName) {
    auto &perfState = mPackagePerfStates[packageName];
    if (!perfState.fpsMonitoringActive) {
        perfState.fpsMonitoringActive = true;

        // 注册FPS变化回调
        GlobalFpsMonitor::getInstance().registerCallback(
            packageName, [this, packageName](float fps) { onFpsChanged(packageName, fps); });

        TLOGD("Started FPS monitoring for %s", packageName.c_str());
    }
}

void PerfConfigFeature::stopFpsMonitoring(const std::string &packageName) {
    auto &perfState = mPackagePerfStates[packageName];
    if (perfState.fpsMonitoringActive) {
        perfState.fpsMonitoringActive = false;
        GlobalFpsMonitor::getInstance().unregisterCallback(packageName);
        TLOGD("Stopped FPS monitoring for %s", packageName.c_str());
    }
}
