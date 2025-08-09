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

// 新增：性能配置类型枚举
enum class ConfigType {
    FPS = 0,    // 帧率配置（原有兼容）
    RATE = 1    // 刷新率配置（新增）
};

// 新增：通用性能配置结构
struct PerformanceConfig {
    std::string value;    // 配置值（如"60", "90", "common"等）
    ConfigType type;      // 配置类型（FPS或RATE）
    PerfLock perfLock;    // 性能锁配置

    PerformanceConfig() : type(ConfigType::FPS) {}

    PerformanceConfig(const std::string &val, ConfigType configType)
        : value(val), type(configType) {}

    void clear() {
        value.clear();
        type = ConfigType::FPS;
        perfLock.clear();
    }

    // 获取配置标识符（用于日志和调试）
    std::string getConfigTypeString() const { return (type == ConfigType::FPS) ? "fps" : "rate"; }

    // 检查是否为有效配置
    bool isValid() const { return !value.empty() && !perfLock.isEmpty(); }
};

// 保留原有FpsConfig结构，用于向后兼容
struct FpsConfig {
    std::string fpsValue;
    PerfLock perfLock;

    void clear() {
        fpsValue.clear();
        perfLock.clear();
    }

    // 新增：从PerformanceConfig转换
    static FpsConfig fromPerformanceConfig(const PerformanceConfig &perfConfig) {
        FpsConfig fpsConfig;
        fpsConfig.fpsValue = perfConfig.value;
        fpsConfig.perfLock = perfConfig.perfLock;
        return fpsConfig;
    }
};

struct ActivityConfig {
    std::string activityName;

    // 新增：使用PerformanceConfig统一存储fps和rate配置
    std::unordered_map<std::string, PerformanceConfig> performanceConfigs;

    // 保留：向后兼容的fpsConfigs（通过performanceConfigs动态生成）
    std::unordered_map<std::string, FpsConfig> fpsConfigs;

    void clear() {
        activityName.clear();
        performanceConfigs.clear();
        fpsConfigs.clear();
    }

    // 新增：添加性能配置的统一接口
    void addPerformanceConfig(const std::string &value, ConfigType type, const PerfLock &perfLock) {
        PerformanceConfig config(value, type);
        config.perfLock = perfLock;
        performanceConfigs[value] = config;

        // 同步更新fpsConfigs以保持向后兼容
        syncFpsConfigs();
    }

    // 新增：获取性能配置
    const PerformanceConfig *getPerformanceConfig(const std::string &value) const {
        auto it = performanceConfigs.find(value);
        return (it != performanceConfigs.end()) ? &it->second : nullptr;
    }

    // 新增：检查是否包含指定类型的配置
    bool hasConfigType(ConfigType type) const {
        for (const auto &pair : performanceConfigs) {
            if (pair.second.type == type) {
                return true;
            }
        }
        return false;
    }

    // 新增：获取配置类型统计
    struct ConfigStats {
        size_t fpsCount = 0;
        size_t rateCount = 0;
        size_t totalCount = 0;
    };

    ConfigStats getConfigStats() const {
        ConfigStats stats;
        for (const auto &pair : performanceConfigs) {
            if (pair.second.type == ConfigType::FPS) {
                stats.fpsCount++;
            } else if (pair.second.type == ConfigType::RATE) {
                stats.rateCount++;
            }
        }
        stats.totalCount = performanceConfigs.size();
        return stats;
    }

private:
    // 内部方法：同步fpsConfigs以保持向后兼容
    void syncFpsConfigs() {
        fpsConfigs.clear();
        for (const auto &pair : performanceConfigs) {
            fpsConfigs[pair.first] = FpsConfig::fromPerformanceConfig(pair.second);
        }
    }
};

struct PackageConfig {
    std::string packageName;
    std::unordered_map<std::string, ActivityConfig> activities;

    void clear() {
        packageName.clear();
        activities.clear();
    }

    // 新增：获取包级配置统计
    struct PackageStats {
        size_t activityCount = 0;
        size_t totalFpsConfigs = 0;
        size_t totalRateConfigs = 0;
        size_t totalPerformanceConfigs = 0;
    };

    PackageStats getPackageStats() const {
        PackageStats stats;
        stats.activityCount = activities.size();

        for (const auto &activityPair : activities) {
            auto activityStats = activityPair.second.getConfigStats();
            stats.totalFpsConfigs += activityStats.fpsCount;
            stats.totalRateConfigs += activityStats.rateCount;
            stats.totalPerformanceConfigs += activityStats.totalCount;
        }

        return stats;
    }
};

struct ConfigQueryResult {
    bool found = false;
    PerfLock perfLock;
    std::string packageName;
    std::string activityName;
    std::string fpsValue;    // 保留原有字段名以兼容现有代码
    std::string source;

    // 新增：配置类型信息
    ConfigType configType = ConfigType::FPS;
    std::string configValue;    // 实际配置值（与fpsValue相同，但语义更明确）

    void clear() {
        found = false;
        perfLock.clear();
        packageName.clear();
        activityName.clear();
        fpsValue.clear();
        source.clear();
        configType = ConfigType::FPS;
        configValue.clear();
    }

    // 新增：从PerformanceConfig设置结果
    void setFromPerformanceConfig(const PerformanceConfig &config, const std::string &pkg,
                                  const std::string &activity) {
        found = true;
        perfLock = config.perfLock;
        packageName = pkg;
        activityName = activity;
        fpsValue = config.value;       // 保持向后兼容
        configValue = config.value;    // 新的语义明确的字段
        configType = config.type;
        source = config.getConfigTypeString();
    }

    // 新增：获取配置描述（用于日志）
    std::string getConfigDescription() const {
        if (!found)
            return "not found";
        return packageName + "::" + activityName + "@" + configValue +
               " (type: " + (configType == ConfigType::FPS ? "fps" : "rate") + ")";
    }
};

// 其他结构保持不变
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
