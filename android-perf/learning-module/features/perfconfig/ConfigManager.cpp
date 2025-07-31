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

    for (xmlNodePtr child = activityNode->children; child != nullptr; child = child->next) {
        if (child->type != XML_ELEMENT_NODE)
            continue;

        if (strcmp(reinterpret_cast<const char *>(child->name), "fps") == 0) {
            FpsConfig fpsConfig;
            if (parseFpsNodeDirectly(child, &fpsConfig)) {
                activityConfig.fpsConfigs[fpsConfig.fpsValue] = fpsConfig;
            }
        }
    }

    (*context->configs)[packageName].activities[activityName] = activityConfig;
    return true;
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

ConfigQueryResult ConfigManager::findConfig(const std::string &packageName,
                                            const std::string &activityName,
                                            const std::string &fpsValue) const {
    mQueryCount++;

    ConfigQueryResult result;
    result.found = false;

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

    auto fpsIt = activityConfig.fpsConfigs.find(fpsValue);
    if (fpsIt == activityConfig.fpsConfigs.end()) {
        TLOGV("%s: FPS config not found: %s for %s::%s", LOG_TAG_CONFIGMGR2, fpsValue.c_str(),
              packageName.c_str(), activityName.c_str());
        return result;
    }

    const FpsConfig &fpsConfig = fpsIt->second;

    result.found = true;
    result.packageName = packageName;
    result.activityName = activityName;
    result.fpsValue = fpsValue;
    result.perfLock = fpsConfig.perfLock;

    mHitCount++;
    TLOGI("%s: Found config for %s::%s@%s with %zu opcodes", LOG_TAG_CONFIGMGR2,
          packageName.c_str(), activityName.c_str(), fpsValue.c_str(),
          result.perfLock.opcodes.size());

    return result;
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

void ConfigManager::dumpConfig() const {
    TLOGI("%s: ========== Configuration Dump ==========", LOG_TAG_CONFIGMGR2);
    TLOGI("%s: Config Path: %s", LOG_TAG_CONFIGMGR2, mConfigPath.c_str());
    TLOGI("%s: Loaded: %s", LOG_TAG_CONFIGMGR2, mLoaded ? "true" : "false");
    TLOGI("%s: Package Count: %zu", LOG_TAG_CONFIGMGR2, mConfigs.size());

    for (const auto &packagePair : mConfigs) {
        const PackageConfig &packageConfig = packagePair.second;
        TLOGI("%s: Package: %s (%zu activities)", LOG_TAG_CONFIGMGR2,
              packageConfig.packageName.c_str(), packageConfig.activities.size());

        for (const auto &activityPair : packageConfig.activities) {
            const ActivityConfig &activityConfig = activityPair.second;
            TLOGI("%s:   Activity: %s (%zu fps configs)", LOG_TAG_CONFIGMGR2,
                  activityConfig.activityName.c_str(), activityConfig.fpsConfigs.size());

            for (const auto &fpsPair : activityConfig.fpsConfigs) {
                const FpsConfig &fpsConfig = fpsPair.second;
                TLOGI("%s:     FPS: %s (opcodes: %zu)", LOG_TAG_CONFIGMGR2,
                      fpsConfig.fpsValue.c_str(), fpsConfig.perfLock.opcodes.size());

                if (fpsConfig.perfLock.opcodes.size() > 0) {
                    std::stringstream ss;
                    size_t maxToPrint = std::min<size_t>(4, fpsConfig.perfLock.opcodes.size());
                    for (size_t i = 0; i < maxToPrint; i++) {
                        if (i > 0)
                            ss << ", ";
                        ss << "0x" << std::hex << fpsConfig.perfLock.opcodes[i];
                    }
                    if (fpsConfig.perfLock.opcodes.size() > maxToPrint) {
                        ss << ", ...";
                    }
                    TLOGI("%s:       Opcodes: [%s]", LOG_TAG_CONFIGMGR2, ss.str().c_str());
                }
            }
        }
    }
    TLOGI("%s: ==========================================", LOG_TAG_CONFIGMGR2);
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

}    // namespace perfconfig
