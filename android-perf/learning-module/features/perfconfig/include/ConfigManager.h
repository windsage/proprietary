/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CONFIGMANAGER2_H
#define CONFIGMANAGER2_H

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <memory>
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
    FpsConfig *currentFpsConfig;
    PerfLock *currentPerfLock;
    std::string currentElement;

    ParseContext()
        : configs(nullptr),
          currentPackage(nullptr),
          currentActivity(nullptr),
          currentFpsConfig(nullptr),
          currentPerfLock(nullptr) {}
};

class ConfigManager {
private:
    std::unordered_map<std::string, PackageConfig> mConfigs;
    std::string mConfigPath;
    bool mLoaded;
    mutable uint32_t mQueryCount;
    mutable uint32_t mHitCount;

    static void parseConfigCallback(xmlNodePtr node, void *data);
    static bool parsePackageNode(xmlNodePtr packageNode, ParseContext *context);
    static bool parseActivityNode(xmlNodePtr activityNode, ParseContext *context);
    static bool parseFpsNode(xmlNodePtr fpsNode, ParseContext *context);
    static bool parsePerfLockNode(xmlNodePtr perfLockNode, ParseContext *context);

    static std::string getNodeAttribute(xmlNodePtr node, const char *attrName);
    static std::string getNodeContent(xmlNodePtr node);
    static bool parseOpcodes(const std::string &opcodesStr, std::vector<int> &opcodes);
    static void logParseProgress(const std::string &element, const std::string &detail = "");

    bool parseXmlFileWithCallback(const std::string &filePath);
    void clearConfigs();
    static bool parseActivityNodeDirectly(xmlNodePtr activityNode, const std::string &packageName,
                                          ParseContext *context);
    static bool parseFpsNodeDirectly(xmlNodePtr fpsNode, FpsConfig *fpsConfig);

public:
    ConfigManager();
    ~ConfigManager();

    bool loadConfig(const std::string &configPath);
    bool reloadConfig();
    bool isLoaded() const { return mLoaded; }
    const std::string &getConfigPath() const { return mConfigPath; }

    ConfigQueryResult findConfig(const std::string &packageName, const std::string &activityName,
                                 const std::string &fpsValue) const;

    bool hasPackage(const std::string &packageName) const;
    bool hasActivity(const std::string &packageName, const std::string &activityName) const;

    size_t getPackageCount() const { return mConfigs.size(); }
    uint32_t getQueryCount() const { return mQueryCount; }
    uint32_t getHitCount() const { return mHitCount; }
    double getHitRate() const { return mQueryCount > 0 ? (double)mHitCount / mQueryCount : 0.0; }

    void dumpConfig() const;
    void dumpStatistics() const;
    std::vector<std::string> getPackageNames() const;
};

}    // namespace perfconfig

#endif    // CONFIGMANAGER2_H
