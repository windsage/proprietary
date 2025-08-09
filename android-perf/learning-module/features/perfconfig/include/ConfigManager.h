/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CONFIGMANAGER2_H
#define CONFIGMANAGER2_H

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "PerfConfigTypes.h"
#include "TLog.h"
#include "XmlParser.h"

namespace perfconfig {

struct ParseContext {
    std::unordered_map<std::string, PackageConfig> *configs;
    PackageConfig *currentPackage;
    ActivityConfig *currentActivity;
    FpsConfig *currentFpsConfig;                    // 保留兼容性
    PerformanceConfig *currentPerformanceConfig;    // 新增
    PerfLock *currentPerfLock;
    std::string currentElement;

    ParseContext()
        : configs(nullptr),
          currentPackage(nullptr),
          currentActivity(nullptr),
          currentFpsConfig(nullptr),
          currentPerformanceConfig(nullptr),
          currentPerfLock(nullptr) {}
};

class ConfigManager {
private:
    std::unordered_map<std::string, PackageConfig> mConfigs;
    std::string mConfigPath;
    bool mLoaded;
    mutable uint32_t mQueryCount;
    mutable uint32_t mHitCount;

    // 原有解析方法（保留兼容性）
    static void parseConfigCallback(xmlNodePtr node, void *data);
    static bool parsePackageNode(xmlNodePtr packageNode, ParseContext *context);
    static bool parseActivityNode(xmlNodePtr activityNode, ParseContext *context);
    static bool parseFpsNode(xmlNodePtr fpsNode, ParseContext *context);
    static bool parsePerfLockNode(xmlNodePtr perfLockNode, ParseContext *context);

    // 直接解析方法（用于主解析逻辑）
    static bool parseActivityNodeDirectly(xmlNodePtr activityNode, const std::string &packageName,
                                          ParseContext *context);
    static bool parseFpsNodeDirectly(xmlNodePtr fpsNode, FpsConfig *fpsConfig);

    // 新增：支持rate节点的解析方法
    static bool parsePerformanceNodeDirectly(xmlNodePtr perfNode, PerformanceConfig *perfConfig,
                                             ConfigType configType);
    static bool validatePerformanceValue(const std::string &value, ConfigType configType);

    // 工具方法
    static std::string getNodeAttribute(xmlNodePtr node, const char *attrName);
    static std::string getNodeContent(xmlNodePtr node);
    static bool parseOpcodes(const std::string &opcodesStr, std::vector<int> &opcodes);
    static void logParseProgress(const std::string &element, const std::string &detail = "");

    // 内部方法
    bool parseXmlFileWithCallback(const std::string &filePath);
    void clearConfigs();

public:
    ConfigManager();
    ~ConfigManager();

    // 核心功能方法
    bool loadConfig(const std::string &configPath);
    bool reloadConfig();
    bool isLoaded() const { return mLoaded; }
    const std::string &getConfigPath() const { return mConfigPath; }

    // 配置查询方法
    ConfigQueryResult findConfig(const std::string &packageName, const std::string &activityName,
                                 const std::string &fpsValue) const;

    // 新增：按配置类型查询
    ConfigQueryResult findConfigByType(const std::string &packageName,
                                       const std::string &activityName, const std::string &value,
                                       ConfigType configType) const;

    // 检查方法
    bool hasPackage(const std::string &packageName) const;
    bool hasActivity(const std::string &packageName, const std::string &activityName) const;

    // 新增：检查Activity是否包含指定类型的配置
    bool hasConfigType(const std::string &packageName, const std::string &activityName,
                       ConfigType configType) const;

    // 统计信息方法
    size_t getPackageCount() const { return mConfigs.size(); }
    uint32_t getQueryCount() const { return mQueryCount; }
    uint32_t getHitCount() const { return mHitCount; }
    double getHitRate() const {
        return mQueryCount > 0 ? static_cast<double>(mHitCount) / mQueryCount : 0.0;
    }

    // 调试和信息输出
    void dumpConfig() const;
    void dumpStatistics() const;
    std::vector<std::string> getPackageNames() const;

    // 新增：获取配置统计信息
    struct GlobalConfigStats {
        size_t totalPackages = 0;
        size_t totalActivities = 0;
        size_t totalFpsConfigs = 0;
        size_t totalRateConfigs = 0;
        size_t totalPerformanceConfigs = 0;
        size_t packagesWithFpsOnly = 0;
        size_t packagesWithRateOnly = 0;
        size_t packagesWithMixed = 0;    // 理论上应该为0（不允许混用）
    };

    GlobalConfigStats getGlobalStats() const;

    // 新增：获取Activity的配置类型
    ConfigType getActivityConfigType(const std::string &packageName,
                                     const std::string &activityName) const;

    // 新增：获取所有配置值（按类型）
    std::vector<std::string> getConfigValues(const std::string &packageName,
                                             const std::string &activityName,
                                             ConfigType configType) const;

    // 新增：增强的调试输出
    void dumpDetailedStats() const;
};

}    // namespace perfconfig

#endif /* CONFIGMANAGER2_H */
