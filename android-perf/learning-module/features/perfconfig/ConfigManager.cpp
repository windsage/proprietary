/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ConfigManager.h"

#include <algorithm>
#include <cstring>
#include <sstream>

#include "ConfigFileManager.h"
#include "StringUtils.h"
#include "XmlParser.h"

#define LOG_TAG_CONFIGMGR2 "PERF-CONFIG-MGR2"

namespace perfconfig {

ConfigManager::ConfigManager() : mLoaded(false), mQueryCount(0), mHitCount(0) {
    TLOGI("%s: Constructor called", LOG_TAG_CONFIGMGR2);
}

ConfigManager::~ConfigManager() {
    TLOGI("%s: Destructor called", LOG_TAG_CONFIGMGR2);
    clearConfigs();
}

void ConfigManager::clearConfigs() {
    TLOGD("%s: Clearing configurations", LOG_TAG_CONFIGMGR2);
    mConfigs.clear();
    mConfigPath.clear();
    mLoaded = false;
    mQueryCount = 0;
    mHitCount = 0;
}

bool ConfigManager::loadConfig(const std::string &configPath) {
    TLOGI("%s: Loading configuration from: %s", LOG_TAG_CONFIGMGR2, configPath.c_str());

    if (configPath.empty()) {
        TLOGE("%s: Config path is empty", LOG_TAG_CONFIGMGR2);
        return false;
    }

    clearConfigs();

    if (!parseXmlFileWithCallback(configPath)) {
        TLOGE("%s: Failed to parse XML file: %s", LOG_TAG_CONFIGMGR2, configPath.c_str());
        return false;
    }

    mConfigPath = configPath;
    mLoaded = true;

    TLOGI("%s: Configuration loaded successfully: %zu packages", LOG_TAG_CONFIGMGR2,
          mConfigs.size());

    if (mConfigs.size() > 0) {
        TLOGI("%s: Package list:", LOG_TAG_CONFIGMGR2);
        for (const auto &pair : mConfigs) {
            const PackageConfig &pkg = pair.second;
            TLOGI("%s:   - %s (%zu activities)", LOG_TAG_CONFIGMGR2, pkg.packageName.c_str(),
                  pkg.activities.size());
        }
    }

    return true;
}

bool ConfigManager::reloadConfig() {
    if (mConfigPath.empty()) {
        TLOGE("%s: No previous config path available for reload", LOG_TAG_CONFIGMGR2);
        return false;
    }

    TLOGI("%s: Reloading configuration from: %s", LOG_TAG_CONFIGMGR2, mConfigPath.c_str());
    return loadConfig(mConfigPath);
}

bool ConfigManager::parseXmlFileWithCallback(const std::string &filePath) {
    TLOGD("%s: Starting XML parsing process", LOG_TAG_CONFIGMGR2);

    std::string actualPath = ConfigFileManager::getConfigFilePath("perf_config.xml");
    if (actualPath.empty()) {
        TLOGE("%s: Cannot find perf_config.xml file", LOG_TAG_CONFIGMGR2);
        return false;
    }

    TLOGI("%s: Using actual config file path: %s", LOG_TAG_CONFIGMGR2, actualPath.c_str());

    std::string configContent;
    if (!ConfigFileManager::readAndDecryptConfig(actualPath, configContent)) {
        TLOGE("%s: Failed to read config file: %s", LOG_TAG_CONFIGMGR2, actualPath.c_str());
        return false;
    }

    TLOGI("%s: Successfully read config file (size: %zu bytes)", LOG_TAG_CONFIGMGR2,
          configContent.size());

    TLOGI("%s: Full XML content:\n%s", LOG_TAG_CONFIGMGR2, configContent.c_str());
    std::string preview = configContent.substr(0, 200);
    TLOGD("%s: Config content preview: %s%s", LOG_TAG_CONFIGMGR2, preview.c_str(),
          configContent.size() > 200 ? "..." : "");

    AppsListXmlParser *xmlParser = new (std::nothrow) AppsListXmlParser();
    if (!xmlParser) {
        TLOGE("%s: Failed to create XML parser", LOG_TAG_CONFIGMGR2);
        return false;
    }

    TLOGD("%s: XML parser created successfully", LOG_TAG_CONFIGMGR2);

    ParseContext context;
    context.configs = &mConfigs;

    const std::string xmlRoot("configs");
    const std::string xmlChild("package");

    TLOGI("%s: Registering XML parser callback (root: %s, child: %s)", LOG_TAG_CONFIGMGR2,
          xmlRoot.c_str(), xmlChild.c_str());

    int8_t idnum = xmlParser->Register(xmlRoot, xmlChild, parseConfigCallback, &context);
    if (idnum < 0) {
        TLOGE("%s: Failed to register XML parser callback (returned %d)", LOG_TAG_CONFIGMGR2,
              idnum);
        delete xmlParser;
        return false;
    }

    TLOGD("%s: XML parser callback registered with ID: %d", LOG_TAG_CONFIGMGR2, idnum);

    TLOGI("%s: Starting ParseFromMemory...", LOG_TAG_CONFIGMGR2);
    int8_t parseResult = xmlParser->ParseFromMemory(configContent);

    TLOGI("%s: ParseFromMemory completed with result: %d", LOG_TAG_CONFIGMGR2, parseResult);

    xmlParser->DeRegister(idnum);
    delete xmlParser;

    if (parseResult != 0) {
        TLOGE("%s: XML parsing failed with result: %d", LOG_TAG_CONFIGMGR2, parseResult);
        return false;
    }

    TLOGI("%s: Successfully parsed %zu packages from XML", LOG_TAG_CONFIGMGR2, mConfigs.size());
    return !mConfigs.empty();
}

void ConfigManager::parseConfigCallback(xmlNodePtr node, void *data) {
    TLOGI("%s: Callback invoked for node", LOG_TAG_CONFIGMGR2);

    ParseContext *context = static_cast<ParseContext *>(data);
    if (!context || !node) {
        TLOGE("%s: Invalid callback parameters", LOG_TAG_CONFIGMGR2);
        return;
    }

    const char *nodeName = reinterpret_cast<const char *>(node->name);
    if (strcmp(nodeName, "activity") == 0) {
        xmlNodePtr packageNode = node->parent;
        if (packageNode &&
            strcmp(reinterpret_cast<const char *>(packageNode->name), "package") == 0) {
            std::string packageName = getNodeAttribute(packageNode, "name");
            if (packageName.empty()) {
                TLOGE("%s: Package node missing 'name' attribute", LOG_TAG_CONFIGMGR2);
                return;
            }

            if (context->configs->find(packageName) == context->configs->end()) {
                TLOGI("%s: Processing new package: %s", LOG_TAG_CONFIGMGR2, packageName.c_str());
                PackageConfig packageConfig;
                packageConfig.packageName = packageName;
                (*context->configs)[packageName] = packageConfig;
            }

            parseActivityNodeDirectly(node, packageName, context);
        }
    }
}

bool ConfigManager::parseActivityNodeDirectly(xmlNodePtr activityNode,
                                              const std::string &packageName,
                                              ParseContext *context) {
    std::string activityName = getNodeAttribute(activityNode, "name");
    if (activityName.empty()) {
        TLOGE("%s: Activity node missing 'name' attribute", LOG_TAG_CONFIGMGR2);
        return false;
    }

    TLOGI("%s: Processing activity: %s in package: %s", LOG_TAG_CONFIGMGR2, activityName.c_str(),
          packageName.c_str());

    ActivityConfig activityConfig;
    activityConfig.activityName = activityName;

    // 统计解析的配置节点
    int fpsNodeCount = 0;
    int rateNodeCount = 0;

    for (xmlNodePtr child = activityNode->children; child != nullptr; child = child->next) {
        if (child->type != XML_ELEMENT_NODE)
            continue;

        const char *nodeName = reinterpret_cast<const char *>(child->name);

        if (strcmp(nodeName, "fps") == 0) {
            // 解析fps节点
            PerformanceConfig perfConfig;
            if (parsePerformanceNodeDirectly(child, &perfConfig, ConfigType::FPS)) {
                activityConfig.addPerformanceConfig(perfConfig.value, perfConfig.type,
                                                    perfConfig.perfLock);
                fpsNodeCount++;
                TLOGI("%s: Added fps config: %s", LOG_TAG_CONFIGMGR2, perfConfig.value.c_str());
            }
        } else if (strcmp(nodeName, "rate") == 0) {
            // 解析rate节点
            PerformanceConfig perfConfig;
            if (parsePerformanceNodeDirectly(child, &perfConfig, ConfigType::RATE)) {
                activityConfig.addPerformanceConfig(perfConfig.value, perfConfig.type,
                                                    perfConfig.perfLock);
                rateNodeCount++;
                TLOGI("%s: Added rate config: %s", LOG_TAG_CONFIGMGR2, perfConfig.value.c_str());
            }
        }
    }

    // 验证配置的合法性（不能混用fps和rate）
    if (fpsNodeCount > 0 && rateNodeCount > 0) {
        TLOGE("%s: Activity %s contains both fps and rate configurations, which is not allowed",
              LOG_TAG_CONFIGMGR2, activityName.c_str());
        return false;
    }

    if (fpsNodeCount == 0 && rateNodeCount == 0) {
        TLOGE("%s: Activity %s has no performance configurations", LOG_TAG_CONFIGMGR2,
              activityName.c_str());
        return false;
    }

    (*context->configs)[packageName].activities[activityName] = activityConfig;

    TLOGI("%s: Successfully parsed activity %s with %d fps configs and %d rate configs",
          LOG_TAG_CONFIGMGR2, activityName.c_str(), fpsNodeCount, rateNodeCount);

    return true;
}

bool ConfigManager::parsePerformanceNodeDirectly(xmlNodePtr perfNode, PerformanceConfig *perfConfig,
                                                 ConfigType configType) {
    if (!perfNode || !perfConfig) {
        TLOGE("%s: Invalid parsePerformanceNodeDirectly parameters", LOG_TAG_CONFIGMGR2);
        return false;
    }

    std::string nodeValue = getNodeAttribute(perfNode, "value");
    if (nodeValue.empty()) {
        const char *nodeTypeName = (configType == ConfigType::FPS) ? "fps" : "rate";
        TLOGE("%s: %s node missing 'value' attribute", LOG_TAG_CONFIGMGR2, nodeTypeName);
        return false;
    }

    // 验证配置值的合法性
    if (!validatePerformanceValue(nodeValue, configType)) {
        const char *nodeTypeName = (configType == ConfigType::FPS) ? "fps" : "rate";
        TLOGE("%s: Invalid %s value: %s", LOG_TAG_CONFIGMGR2, nodeTypeName, nodeValue.c_str());
        return false;
    }

    TLOGI("%s: Processing %s: %s", LOG_TAG_CONFIGMGR2,
          (configType == ConfigType::FPS) ? "fps" : "rate", nodeValue.c_str());

    perfConfig->value = nodeValue;
    perfConfig->type = configType;

    // 解析perflock子节点
    for (xmlNodePtr child = perfNode->children; child != nullptr; child = child->next) {
        if (child->type != XML_ELEMENT_NODE)
            continue;

        if (strcmp(reinterpret_cast<const char *>(child->name), "perflock") == 0) {
            std::string perfLockContent = getNodeContent(child);
            TLOGD("%s: PerfLock content: %s", LOG_TAG_CONFIGMGR2, perfLockContent.c_str());

            if (!parseOpcodes(perfLockContent, perfConfig->perfLock.opcodes)) {
                TLOGW("%s: Failed to parse opcodes: %s", LOG_TAG_CONFIGMGR2,
                      perfLockContent.c_str());
            } else {
                TLOGD("%s: Successfully parsed %zu opcodes", LOG_TAG_CONFIGMGR2,
                      perfConfig->perfLock.opcodes.size());
            }

            perfConfig->perfLock.timeout = 0;
            break;
        }
    }

    return true;
}

// 新增：验证性能配置值的合法性
bool ConfigManager::validatePerformanceValue(const std::string &value, ConfigType configType) {
    static const std::set<std::string> validFpsValues = {"30", "60", "90", "120", "144", "common"};
    static const std::set<std::string> validRateValues = {"common", "60", "90", "120", "144"};

    if (configType == ConfigType::FPS) {
        return validFpsValues.find(value) != validFpsValues.end();
    } else if (configType == ConfigType::RATE) {
        return validRateValues.find(value) != validRateValues.end();
    }

    return false;
}

// 修改findConfig方法，支持从PerformanceConfig查询
ConfigQueryResult ConfigManager::findConfig(const std::string &packageName,
                                            const std::string &activityName,
                                            const std::string &fpsValue) const {
    mQueryCount++;

    ConfigQueryResult result;
    result.clear();

    TLOGV("%s: Query: %s::%s@%s", LOG_TAG_CONFIGMGR2, packageName.c_str(), activityName.c_str(),
          fpsValue.c_str());

    if (!mLoaded) {
        TLOGW("%s: Configuration not loaded", LOG_TAG_CONFIGMGR2);
        return result;
    }

    auto packageIt = mConfigs.find(packageName);
    if (packageIt == mConfigs.end()) {
        TLOGV("%s: Package not found: %s", LOG_TAG_CONFIGMGR2, packageName.c_str());
        return result;
    }

    const PackageConfig &packageConfig = packageIt->second;

    auto activityIt = packageConfig.activities.find(activityName);
    if (activityIt == packageConfig.activities.end()) {
        TLOGV("%s: Activity not found: %s in package: %s", LOG_TAG_CONFIGMGR2, activityName.c_str(),
              packageName.c_str());
        return result;
    }

    const ActivityConfig &activityConfig = activityIt->second;

    // 优先从performanceConfigs中查找
    const PerformanceConfig *perfConfig = activityConfig.getPerformanceConfig(fpsValue);
    if (perfConfig) {
        result.setFromPerformanceConfig(*perfConfig, packageName, activityName);
        mHitCount++;
        TLOGI("%s: Found config for %s with %zu opcodes (type: %s)", LOG_TAG_CONFIGMGR2,
              result.getConfigDescription().c_str(), result.perfLock.opcodes.size(),
              perfConfig->getConfigTypeString().c_str());
        return result;
    }

    // 向后兼容：如果performanceConfigs中没有，从fpsConfigs中查找
    auto fpsIt = activityConfig.fpsConfigs.find(fpsValue);
    if (fpsIt != activityConfig.fpsConfigs.end()) {
        const FpsConfig &fpsConfig = fpsIt->second;

        result.found = true;
        result.packageName = packageName;
        result.activityName = activityName;
        result.fpsValue = fpsValue;
        result.configValue = fpsValue;
        result.configType = ConfigType::FPS;    // 兼容模式默认为FPS
        result.perfLock = fpsConfig.perfLock;
        result.source = "fps_compat";

        mHitCount++;
        TLOGI("%s: Found legacy config for %s::%s@%s with %zu opcodes", LOG_TAG_CONFIGMGR2,
              packageName.c_str(), activityName.c_str(), fpsValue.c_str(),
              result.perfLock.opcodes.size());
    }

    return result;
}

// 修改dumpConfig方法，显示新的配置信息
void ConfigManager::dumpConfig() const {
    TLOGI("%s: ========== Configuration Dump ==========", LOG_TAG_CONFIGMGR2);
    TLOGI("%s: Config Path: %s", LOG_TAG_CONFIGMGR2, mConfigPath.c_str());
    TLOGI("%s: Loaded: %s", LOG_TAG_CONFIGMGR2, mLoaded ? "true" : "false");
    TLOGI("%s: Package Count: %zu", LOG_TAG_CONFIGMGR2, mConfigs.size());

    for (const auto &packagePair : mConfigs) {
        const PackageConfig &packageConfig = packagePair.second;
        auto packageStats = packageConfig.getPackageStats();

        TLOGI("%s: Package: %s (%zu activities, %zu fps configs, %zu rate configs)",
              LOG_TAG_CONFIGMGR2, packageConfig.packageName.c_str(), packageStats.activityCount,
              packageStats.totalFpsConfigs, packageStats.totalRateConfigs);

        for (const auto &activityPair : packageConfig.activities) {
            const ActivityConfig &activityConfig = activityPair.second;
            auto activityStats = activityConfig.getConfigStats();

            TLOGI("%s:   Activity: %s (fps: %zu, rate: %zu, total: %zu)", LOG_TAG_CONFIGMGR2,
                  activityConfig.activityName.c_str(), activityStats.fpsCount,
                  activityStats.rateCount, activityStats.totalCount);

            // 显示性能配置详情
            for (const auto &perfPair : activityConfig.performanceConfigs) {
                const PerformanceConfig &perfConfig = perfPair.second;
                TLOGI("%s:     %s: %s (opcodes: %zu)", LOG_TAG_CONFIGMGR2,
                      perfConfig.getConfigTypeString().c_str(), perfConfig.value.c_str(),
                      perfConfig.perfLock.opcodes.size());

                if (perfConfig.perfLock.opcodes.size() > 0) {
                    std::stringstream ss;
                    size_t maxToPrint = std::min<size_t>(4, perfConfig.perfLock.opcodes.size());
                    for (size_t i = 0; i < maxToPrint; i++) {
                        if (i > 0)
                            ss << ", ";
                        ss << "0x" << std::hex << perfConfig.perfLock.opcodes[i];
                    }
                    if (perfConfig.perfLock.opcodes.size() > maxToPrint) {
                        ss << ", ...";
                    }
                    TLOGI("%s:       Opcodes: [%s]", LOG_TAG_CONFIGMGR2, ss.str().c_str());
                }
            }
        }
    }
    TLOGI("%s: ==========================================", LOG_TAG_CONFIGMGR2);
}

bool ConfigManager::parseFpsNodeDirectly(xmlNodePtr fpsNode, FpsConfig *fpsConfig) {
    if (!fpsNode || !fpsConfig) {
        TLOGE("%s: Invalid parseFpsNodeDirectly parameters", LOG_TAG_CONFIGMGR2);
        return false;
    }

    std::string fpsValue = getNodeAttribute(fpsNode, "value");
    if (fpsValue.empty()) {
        TLOGE("%s: FPS node missing 'value' attribute", LOG_TAG_CONFIGMGR2);
        return false;
    }

    TLOGI("%s: Processing fps: %s", LOG_TAG_CONFIGMGR2, fpsValue.c_str());

    fpsConfig->fpsValue = fpsValue;

    for (xmlNodePtr child = fpsNode->children; child != nullptr; child = child->next) {
        if (child->type != XML_ELEMENT_NODE)
            continue;

        if (strcmp(reinterpret_cast<const char *>(child->name), "perflock") == 0) {
            std::string perfLockContent = getNodeContent(child);
            TLOGD("%s: PerfLock content: %s", LOG_TAG_CONFIGMGR2, perfLockContent.c_str());

            if (!parseOpcodes(perfLockContent, fpsConfig->perfLock.opcodes)) {
                TLOGW("%s: Failed to parse opcodes: %s", LOG_TAG_CONFIGMGR2,
                      perfLockContent.c_str());
            } else {
                TLOGD("%s: Successfully parsed %zu opcodes", LOG_TAG_CONFIGMGR2,
                      fpsConfig->perfLock.opcodes.size());
            }

            fpsConfig->perfLock.timeout = 0;
            break;
        }
    }

    return true;
}
bool ConfigManager::parsePackageNode(xmlNodePtr packageNode, ParseContext *context) {
    if (!packageNode || !context || !context->configs) {
        TLOGE("%s: Invalid parsePackageNode parameters", LOG_TAG_CONFIGMGR2);
        return false;
    }

    xmlChar *nodeContent = xmlNodeGetContent(packageNode);
    TLOGD("%s: Package node content: %s", LOG_TAG_CONFIGMGR2,
          nodeContent ? reinterpret_cast<const char *>(nodeContent) : "NULL");
    if (nodeContent)
        xmlFree(nodeContent);
    std::string packageName = getNodeAttribute(packageNode, "name");
    if (packageName.empty()) {
        TLOGE("%s: Package node missing 'name' attribute", LOG_TAG_CONFIGMGR2);
        TLOGE("%s: Available attributes:", LOG_TAG_CONFIGMGR2);
        for (xmlAttrPtr attr = packageNode->properties; attr != nullptr; attr = attr->next) {
            const char *attrName = reinterpret_cast<const char *>(attr->name);
            xmlChar *attrValue = xmlGetProp(packageNode, attr->name);
            TLOGE("%s:   %s = %s", LOG_TAG_CONFIGMGR2, attrName ? attrName : "NULL",
                  attrValue ? reinterpret_cast<const char *>(attrValue) : "NULL");
            if (attrValue)
                xmlFree(attrValue);
        }
        return false;
    }

    TLOGI("%s: Parsing package: %s", LOG_TAG_CONFIGMGR2, packageName.c_str());

    PackageConfig packageConfig;
    packageConfig.packageName = packageName;
    context->currentPackage = &packageConfig;

    int activityCount = 0;

    for (xmlNodePtr child = packageNode->children; child != nullptr; child = child->next) {
        if (child->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (strcmp(reinterpret_cast<const char *>(child->name), "activity") == 0) {
            TLOGD("%s: Found activity node in package: %s", LOG_TAG_CONFIGMGR2,
                  packageName.c_str());

            if (parseActivityNode(child, context)) {
                activityCount++;
            } else {
                TLOGW("%s: Failed to parse activity node in package: %s", LOG_TAG_CONFIGMGR2,
                      packageName.c_str());
            }
        } else {
            TLOGV("%s: Skipping non-activity node: %s", LOG_TAG_CONFIGMGR2,
                  reinterpret_cast<const char *>(child->name));
        }
    }

    (*context->configs)[packageName] = packageConfig;
    context->currentPackage = nullptr;

    TLOGI("%s: Successfully parsed package: %s with %d activities", LOG_TAG_CONFIGMGR2,
          packageName.c_str(), activityCount);
    return true;
}

bool ConfigManager::parseActivityNode(xmlNodePtr activityNode, ParseContext *context) {
    if (!activityNode || !context || !context->currentPackage) {
        TLOGE("%s: Invalid parseActivityNode parameters", LOG_TAG_CONFIGMGR2);
        return false;
    }

    std::string activityName = getNodeAttribute(activityNode, "name");
    if (activityName.empty()) {
        TLOGE("%s: Activity node missing 'name' attribute", LOG_TAG_CONFIGMGR2);
        return false;
    }

    TLOGI("%s: Parsing activity: %s", LOG_TAG_CONFIGMGR2, activityName.c_str());

    ActivityConfig activityConfig;
    activityConfig.activityName = activityName;
    context->currentActivity = &activityConfig;

    int fpsCount = 0;

    for (xmlNodePtr child = activityNode->children; child != nullptr; child = child->next) {
        if (child->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (strcmp(reinterpret_cast<const char *>(child->name), "fps") == 0) {
            TLOGD("%s: Found fps node in activity: %s", LOG_TAG_CONFIGMGR2, activityName.c_str());

            if (parseFpsNode(child, context)) {
                fpsCount++;
            } else {
                TLOGW("%s: Failed to parse fps node in activity: %s", LOG_TAG_CONFIGMGR2,
                      activityName.c_str());
            }
        } else {
            TLOGV("%s: Skipping non-fps node: %s", LOG_TAG_CONFIGMGR2,
                  reinterpret_cast<const char *>(child->name));
        }
    }

    context->currentPackage->activities[activityName] = activityConfig;
    context->currentActivity = nullptr;

    TLOGI("%s: Successfully parsed activity: %s with %d fps configs", LOG_TAG_CONFIGMGR2,
          activityName.c_str(), fpsCount);
    return true;
}

bool ConfigManager::parseFpsNode(xmlNodePtr fpsNode, ParseContext *context) {
    if (!fpsNode || !context || !context->currentActivity) {
        TLOGE("%s: Invalid parseFpsNode parameters", LOG_TAG_CONFIGMGR2);
        return false;
    }

    std::string fpsValue = getNodeAttribute(fpsNode, "value");
    if (fpsValue.empty()) {
        TLOGE("%s: FPS node missing 'value' attribute", LOG_TAG_CONFIGMGR2);
        return false;
    }

    TLOGI("%s: Parsing fps: %s", LOG_TAG_CONFIGMGR2, fpsValue.c_str());

    FpsConfig fpsConfig;
    fpsConfig.fpsValue = fpsValue;
    context->currentFpsConfig = &fpsConfig;

    bool foundPerfLock = false;

    for (xmlNodePtr child = fpsNode->children; child != nullptr; child = child->next) {
        if (child->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (strcmp(reinterpret_cast<const char *>(child->name), "perflock") == 0) {
            TLOGD("%s: Found perflock node in fps: %s", LOG_TAG_CONFIGMGR2, fpsValue.c_str());

            if (parsePerfLockNode(child, context)) {
                foundPerfLock = true;
            } else {
                TLOGW("%s: Failed to parse perflock node in fps: %s", LOG_TAG_CONFIGMGR2,
                      fpsValue.c_str());
            }
        } else {
            TLOGV("%s: Skipping non-perflock node: %s", LOG_TAG_CONFIGMGR2,
                  reinterpret_cast<const char *>(child->name));
        }
    }

    context->currentActivity->fpsConfigs[fpsValue] = fpsConfig;
    context->currentFpsConfig = nullptr;

    TLOGI("%s: Successfully parsed fps: %s (perflock: %s)", LOG_TAG_CONFIGMGR2, fpsValue.c_str(),
          foundPerfLock ? "yes" : "no");
    return true;
}

bool ConfigManager::parsePerfLockNode(xmlNodePtr perfLockNode, ParseContext *context) {
    if (!perfLockNode || !context || !context->currentFpsConfig) {
        TLOGE("%s: Invalid parsePerfLockNode parameters", LOG_TAG_CONFIGMGR2);
        return false;
    }

    TLOGD("%s: Parsing perflock node", LOG_TAG_CONFIGMGR2);

    PerfLock perfLock;
    context->currentPerfLock = &perfLock;

    std::string perfLockContent = getNodeContent(perfLockNode);

    TLOGD("%s: PerfLock content: %s", LOG_TAG_CONFIGMGR2, perfLockContent.c_str());

    if (!perfLockContent.empty()) {
        if (!parseOpcodes(perfLockContent, perfLock.opcodes)) {
            TLOGW("%s: Failed to parse opcodes from content: %s", LOG_TAG_CONFIGMGR2,
                  perfLockContent.c_str());
        } else {
            TLOGD("%s: Successfully parsed %zu opcodes", LOG_TAG_CONFIGMGR2,
                  perfLock.opcodes.size());
        }
    }

    perfLock.timeout = 0;

    context->currentFpsConfig->perfLock = perfLock;
    context->currentPerfLock = nullptr;

    TLOGI("%s: Successfully parsed perflock with %zu opcodes", LOG_TAG_CONFIGMGR2,
          perfLock.opcodes.size());
    return true;
}

std::string ConfigManager::getNodeAttribute(xmlNodePtr node, const char *attrName) {
    if (!node || !attrName) {
        TLOGV("%s: getNodeAttribute called with null parameters", LOG_TAG_CONFIGMGR2);
        return "";
    }

    xmlChar *attrValue = xmlGetProp(node, reinterpret_cast<const xmlChar *>(attrName));
    if (!attrValue) {
        TLOGV("%s: Attribute '%s' not found", LOG_TAG_CONFIGMGR2, attrName);
        return "";
    }

    std::string result = reinterpret_cast<const char *>(attrValue);
    xmlFree(attrValue);

    TLOGV("%s: Attribute '%s' = '%s'", LOG_TAG_CONFIGMGR2, attrName, result.c_str());
    return result;
}

std::string ConfigManager::getNodeContent(xmlNodePtr node) {
    if (!node) {
        TLOGV("%s: getNodeContent called with null node", LOG_TAG_CONFIGMGR2);
        return "";
    }

    xmlChar *content = xmlNodeGetContent(node);
    if (!content) {
        TLOGV("%s: Node has no content", LOG_TAG_CONFIGMGR2);
        return "";
    }

    std::string result = reinterpret_cast<const char *>(content);
    xmlFree(content);

    std::string trimmed = StringUtils::trim(result);
    TLOGV("%s: Node content (trimmed): '%s'", LOG_TAG_CONFIGMGR2, trimmed.c_str());
    return trimmed;
}

bool ConfigManager::parseOpcodes(const std::string &opcodesStr, std::vector<int> &opcodes) {
    if (opcodesStr.empty()) {
        TLOGD("%s: Empty opcodes string", LOG_TAG_CONFIGMGR2);
        return true;    // Empty opcodes is valid
    }

    TLOGD("%s: Parsing opcodes from: '%s'", LOG_TAG_CONFIGMGR2, opcodesStr.c_str());

    std::string cleanStr = StringUtils::removeAllWhitespace(opcodesStr);
    std::vector<std::string> tokens = StringUtils::split(cleanStr, ',');

    TLOGD("%s: Split into %zu tokens", LOG_TAG_CONFIGMGR2, tokens.size());

    opcodes.clear();
    opcodes.reserve(tokens.size());

    for (size_t i = 0; i < tokens.size(); i++) {
        const std::string &token = tokens[i];
        if (token.empty()) {
            TLOGV("%s: Skipping empty token at index %zu", LOG_TAG_CONFIGMGR2, i);
            continue;
        }

        long value;
        int base = (token.find("0x") == 0 || token.find("0X") == 0) ? 16 : 10;

        if (!StringUtils::parseLong(token, value, base)) {
            TLOGE("%s: Invalid opcode format at index %zu: '%s'", LOG_TAG_CONFIGMGR2, i,
                  token.c_str());
            return false;
        }

        opcodes.push_back(static_cast<int>(value));
        TLOGV("%s: Parsed opcode[%zu]: %s -> %d (base %d)", LOG_TAG_CONFIGMGR2, i, token.c_str(),
              static_cast<int>(value), base);
    }

    TLOGI("%s: Successfully parsed %zu opcodes from: %s", LOG_TAG_CONFIGMGR2, opcodes.size(),
          opcodesStr.c_str());
    return true;
}

void ConfigManager::logParseProgress(const std::string &element, const std::string &detail) {
    if (!detail.empty()) {
        TLOGV("%s: Parsing %s: %s", LOG_TAG_CONFIGMGR2, element.c_str(), detail.c_str());
    } else {
        TLOGV("%s: Parsing %s", LOG_TAG_CONFIGMGR2, element.c_str());
    }
}

bool ConfigManager::hasPackage(const std::string &packageName) const {
    bool found = mLoaded && mConfigs.find(packageName) != mConfigs.end();
    TLOGV("%s: hasPackage(%s) = %s", LOG_TAG_CONFIGMGR2, packageName.c_str(),
          found ? "true" : "false");
    return found;
}

bool ConfigManager::hasActivity(const std::string &packageName,
                                const std::string &activityName) const {
    if (!hasPackage(packageName)) {
        return false;
    }

    const PackageConfig &packageConfig = mConfigs.at(packageName);
    bool found = packageConfig.activities.find(activityName) != packageConfig.activities.end();
    TLOGV("%s: hasActivity(%s, %s) = %s", LOG_TAG_CONFIGMGR2, packageName.c_str(),
          activityName.c_str(), found ? "true" : "false");
    return found;
}

void ConfigManager::dumpStatistics() const {
    TLOGI("%s: ========== Statistics ==========", LOG_TAG_CONFIGMGR2);
    TLOGI("%s: Query Count: %u", LOG_TAG_CONFIGMGR2, mQueryCount);
    TLOGI("%s: Hit Count: %u", LOG_TAG_CONFIGMGR2, mHitCount);
    TLOGI("%s: Hit Rate: %.2f%%", LOG_TAG_CONFIGMGR2, getHitRate() * 100.0);
    TLOGI("%s: ===============================", LOG_TAG_CONFIGMGR2);
}

std::vector<std::string> ConfigManager::getPackageNames() const {
    std::vector<std::string> names;
    names.reserve(mConfigs.size());

    for (const auto &pair : mConfigs) {
        names.push_back(pair.first);
    }

    std::sort(names.begin(), names.end());
    return names;
}

// 新增：按配置类型查询
perfconfig::ConfigQueryResult ConfigManager::findConfigByType(
    const std::string &packageName, const std::string &activityName, const std::string &value,
    perfconfig::ConfigType configType) const {
    mQueryCount++;

    perfconfig::ConfigQueryResult result;
    result.clear();

    TLOGV("%s: Query by type: %s::%s@%s (type: %s)", LOG_TAG_CONFIGMGR2, packageName.c_str(),
          activityName.c_str(), value.c_str(),
          (configType == perfconfig::ConfigType::FPS) ? "fps" : "rate");

    if (!mLoaded) {
        TLOGW("%s: Configuration not loaded", LOG_TAG_CONFIGMGR2);
        return result;
    }

    auto packageIt = mConfigs.find(packageName);
    if (packageIt == mConfigs.end()) {
        TLOGV("%s: Package not found: %s", LOG_TAG_CONFIGMGR2, packageName.c_str());
        return result;
    }

    const perfconfig::PackageConfig &packageConfig = packageIt->second;
    auto activityIt = packageConfig.activities.find(activityName);
    if (activityIt == packageConfig.activities.end()) {
        TLOGV("%s: Activity not found: %s in package: %s", LOG_TAG_CONFIGMGR2, activityName.c_str(),
              packageName.c_str());
        return result;
    }

    const perfconfig::ActivityConfig &activityConfig = activityIt->second;
    const perfconfig::PerformanceConfig *perfConfig = activityConfig.getPerformanceConfig(value);

    if (perfConfig && perfConfig->type == configType) {
        result.setFromPerformanceConfig(*perfConfig, packageName, activityName);
        mHitCount++;
        TLOGI("%s: Found typed config: %s with %zu opcodes", LOG_TAG_CONFIGMGR2,
              result.getConfigDescription().c_str(), result.perfLock.opcodes.size());
    }

    return result;
}

// 新增：检查Activity是否包含指定类型的配置
bool ConfigManager::hasConfigType(const std::string &packageName, const std::string &activityName,
                                  perfconfig::ConfigType configType) const {
    if (!mLoaded) {
        return false;
    }

    auto packageIt = mConfigs.find(packageName);
    if (packageIt == mConfigs.end()) {
        return false;
    }

    const perfconfig::PackageConfig &packageConfig = packageIt->second;
    auto activityIt = packageConfig.activities.find(activityName);
    if (activityIt == packageConfig.activities.end()) {
        return false;
    }

    const perfconfig::ActivityConfig &activityConfig = activityIt->second;
    return activityConfig.hasConfigType(configType);
}

// 新增：获取全局配置统计信息
ConfigManager::GlobalConfigStats ConfigManager::getGlobalStats() const {
    GlobalConfigStats stats;

    stats.totalPackages = mConfigs.size();

    for (const auto &packagePair : mConfigs) {
        const perfconfig::PackageConfig &packageConfig = packagePair.second;
        auto packageStats = packageConfig.getPackageStats();

        stats.totalActivities += packageStats.activityCount;
        stats.totalFpsConfigs += packageStats.totalFpsConfigs;
        stats.totalRateConfigs += packageStats.totalRateConfigs;
        stats.totalPerformanceConfigs += packageStats.totalPerformanceConfigs;

        // 统计包级配置类型
        if (packageStats.totalFpsConfigs > 0 && packageStats.totalRateConfigs == 0) {
            stats.packagesWithFpsOnly++;
        } else if (packageStats.totalRateConfigs > 0 && packageStats.totalFpsConfigs == 0) {
            stats.packagesWithRateOnly++;
        } else if (packageStats.totalFpsConfigs > 0 && packageStats.totalRateConfigs > 0) {
            stats.packagesWithMixed++;    // 理论上应该为0
        }
    }

    return stats;
}

// 新增：获取Activity的配置类型
perfconfig::ConfigType ConfigManager::getActivityConfigType(const std::string &packageName,
                                                            const std::string &activityName) const {
    if (!mLoaded) {
        return perfconfig::ConfigType::FPS;    // 默认返回FPS
    }

    auto packageIt = mConfigs.find(packageName);
    if (packageIt == mConfigs.end()) {
        return perfconfig::ConfigType::FPS;
    }

    const perfconfig::PackageConfig &packageConfig = packageIt->second;
    auto activityIt = packageConfig.activities.find(activityName);
    if (activityIt == packageConfig.activities.end()) {
        return perfconfig::ConfigType::FPS;
    }

    const perfconfig::ActivityConfig &activityConfig = activityIt->second;

    // 检查配置类型优先级：rate > fps
    if (activityConfig.hasConfigType(perfconfig::ConfigType::RATE)) {
        return perfconfig::ConfigType::RATE;
    } else if (activityConfig.hasConfigType(perfconfig::ConfigType::FPS)) {
        return perfconfig::ConfigType::FPS;
    }

    return perfconfig::ConfigType::FPS;    // 默认
}

// 新增：获取所有配置值（按类型）
std::vector<std::string> ConfigManager::getConfigValues(const std::string &packageName,
                                                        const std::string &activityName,
                                                        perfconfig::ConfigType configType) const {
    std::vector<std::string> values;

    if (!mLoaded) {
        return values;
    }

    auto packageIt = mConfigs.find(packageName);
    if (packageIt == mConfigs.end()) {
        return values;
    }

    const perfconfig::PackageConfig &packageConfig = packageIt->second;
    auto activityIt = packageConfig.activities.find(activityName);
    if (activityIt == packageConfig.activities.end()) {
        return values;
    }

    const perfconfig::ActivityConfig &activityConfig = activityIt->second;

    // 遍历所有性能配置，筛选指定类型
    for (const auto &perfPair : activityConfig.performanceConfigs) {
        if (perfPair.second.type == configType) {
            values.push_back(perfPair.first);
        }
    }

    std::sort(values.begin(), values.end());
    return values;
}

// 新增：增强的调试输出方法
void ConfigManager::dumpDetailedStats() const {
    auto globalStats = getGlobalStats();

    TLOGI("%s: ========== Detailed Statistics ==========", LOG_TAG_CONFIGMGR2);
    TLOGI("%s: Total Packages: %zu", LOG_TAG_CONFIGMGR2, globalStats.totalPackages);
    TLOGI("%s: Total Activities: %zu", LOG_TAG_CONFIGMGR2, globalStats.totalActivities);
    TLOGI("%s: Total FPS Configs: %zu", LOG_TAG_CONFIGMGR2, globalStats.totalFpsConfigs);
    TLOGI("%s: Total Rate Configs: %zu", LOG_TAG_CONFIGMGR2, globalStats.totalRateConfigs);
    TLOGI("%s: Total Performance Configs: %zu", LOG_TAG_CONFIGMGR2,
          globalStats.totalPerformanceConfigs);
    TLOGI("%s: Packages with FPS only: %zu", LOG_TAG_CONFIGMGR2, globalStats.packagesWithFpsOnly);
    TLOGI("%s: Packages with Rate only: %zu", LOG_TAG_CONFIGMGR2, globalStats.packagesWithRateOnly);
    TLOGI("%s: Packages with Mixed configs: %zu", LOG_TAG_CONFIGMGR2,
          globalStats.packagesWithMixed);
    TLOGI("%s: Query Count: %u", LOG_TAG_CONFIGMGR2, mQueryCount);
    TLOGI("%s: Hit Count: %u", LOG_TAG_CONFIGMGR2, mHitCount);
    TLOGI("%s: Hit Rate: %.2f%%", LOG_TAG_CONFIGMGR2, getHitRate() * 100.0);
    TLOGI("%s: ==========================================", LOG_TAG_CONFIGMGR2);
}

}    // namespace perfconfig
