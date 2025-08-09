/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

// 只依赖核心的ConfigManager相关头文件
#include "../include/ConfigManager.h"
#include "../include/PerfConfigTypes.h"
#include "../include/TLog.h"

#define LOG_TAG_TEST "PERF-CONFIG-MGR3-TEST"

using namespace perfconfig;

class ConfigManager3Test {
public:
    static int runAllTests() {
        TLOGI("%s: ======== ConfigManager3 XML Parsing Test Started ========", LOG_TAG_TEST);

        bool allPassed = true;
        int testCount = 0;
        int passedCount = 0;

        if (!setupTestEnvironment()) {
            TLOGE("%s: Failed to setup test environment", LOG_TAG_TEST);
            return 1;
        }

        // XML解析基础测试
        testCount++;
        if (testBasicRateXmlParsing())
            passedCount++;
        testCount++;
        if (testBasicFpsXmlParsing())
            passedCount++;
        testCount++;
        if (testMixedFpsRatePackages())
            passedCount++;

        // XML验证和约束测试
        testCount++;
        if (testInvalidMixedNodes())
            passedCount++;
        testCount++;
        if (testInvalidRateValues())
            passedCount++;
        testCount++;
        if (testMissingPerflockNode())
            passedCount++;
        testCount++;
        if (testEmptyActivityNode())
            passedCount++;

        // 配置类型检测测试
        testCount++;
        if (testConfigTypeDetection())
            passedCount++;
        testCount++;
        if (testConfigTypePriority())
            passedCount++;

        // 查询功能测试
        testCount++;
        if (testRateConfigQueries())
            passedCount++;
        testCount++;
        if (testFallbackMechanism())
            passedCount++;

        cleanup();

        TLOGI("%s: ======== Test Results ========", LOG_TAG_TEST);
        TLOGI("%s: Total Tests: %d", LOG_TAG_TEST, testCount);
        TLOGI("%s: Passed: %d", LOG_TAG_TEST, passedCount);
        TLOGI("%s: Failed: %d", LOG_TAG_TEST, testCount - passedCount);

        bool allTestsPassed = (passedCount == testCount);
        TLOGI("%s: ======== Test %s ========", LOG_TAG_TEST, allTestsPassed ? "PASSED" : "FAILED");

        return allTestsPassed ? 0 : 1;
    }

private:
    static bool setupTestEnvironment() {
        // 创建测试目录
        if (system("mkdir -p /data/vendor/perf") != 0) {
            TLOGE("%s: Failed to create test directories", LOG_TAG_TEST);
            return false;
        }
        TLOGI("%s: Test environment setup completed", LOG_TAG_TEST);
        return true;
    }

    // Test 1: 基础Rate节点XML解析
    static bool testBasicRateXmlParsing() {
        TLOGI("%s: [TEST] Testing basic Rate XML parsing...", LOG_TAG_TEST);

        if (!createBasicRateTestXml()) {
            TLOGE("%s: [TEST] Failed to create Rate test XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (!manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Failed to load Rate config", LOG_TAG_TEST);
            return false;
        }

        if (manager.getPackageCount() != 2) {
            TLOGE("%s: [TEST] Expected 2 packages, got %zu", LOG_TAG_TEST,
                  manager.getPackageCount());
            return false;
        }

        // 测试Rate配置查询
        ConfigQueryResult result = manager.findConfig("com.game.rate", "common", "120");
        if (!result.found) {
            TLOGE("%s: [TEST] Should find Rate config com.game.rate::common@120", LOG_TAG_TEST);
            return false;
        }

        if (result.configType != ConfigType::RATE) {
            TLOGE("%s: [TEST] Config type should be RATE", LOG_TAG_TEST);
            return false;
        }

        if (result.fpsValue != "120") {
            TLOGE("%s: [TEST] Rate value should be '120', got '%s'", LOG_TAG_TEST,
                  result.fpsValue.c_str());
            return false;
        }

        // 验证perflock opcodes被正确解析
        if (result.perfLock.opcodes.empty()) {
            TLOGE("%s: [TEST] Should have opcodes in result", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Basic Rate parsing test PASSED", LOG_TAG_TEST);
        return true;
    }

    // Test 2: 基础FPS节点XML解析（向后兼容性）
    static bool testBasicFpsXmlParsing() {
        TLOGI("%s: [TEST] Testing basic FPS XML parsing...", LOG_TAG_TEST);

        if (!createBasicFpsTestXml()) {
            TLOGE("%s: [TEST] Failed to create FPS test XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (!manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Failed to load FPS config", LOG_TAG_TEST);
            return false;
        }

        // 测试FPS配置查询
        ConfigQueryResult result = manager.findConfig("com.game.fps", "common", "90");
        if (!result.found) {
            TLOGE("%s: [TEST] Should find FPS config com.game.fps::common@90", LOG_TAG_TEST);
            return false;
        }

        if (result.configType != ConfigType::FPS) {
            TLOGE("%s: [TEST] Config type should be FPS", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Basic FPS parsing test PASSED", LOG_TAG_TEST);
        return true;
    }

    // Test 3: 混合FPS和Rate包配置
    static bool testMixedFpsRatePackages() {
        TLOGI("%s: [TEST] Testing mixed FPS and Rate packages...", LOG_TAG_TEST);

        if (!createMixedPackagesTestXml()) {
            TLOGE("%s: [TEST] Failed to create mixed packages test XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (!manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Failed to load mixed config", LOG_TAG_TEST);
            return false;
        }

        if (manager.getPackageCount() != 3) {
            TLOGE("%s: [TEST] Expected 3 packages, got %zu", LOG_TAG_TEST,
                  manager.getPackageCount());
            return false;
        }

        // 测试FPS包
        ConfigQueryResult fpsResult = manager.findConfig("com.app.fps", "MainActivity", "90");
        if (!fpsResult.found || fpsResult.configType != ConfigType::FPS) {
            TLOGE("%s: [TEST] FPS package config failed", LOG_TAG_TEST);
            return false;
        }

        // 测试Rate包
        ConfigQueryResult rateResult = manager.findConfig("com.app.rate", "MainActivity", "144");
        if (!rateResult.found || rateResult.configType != ConfigType::RATE) {
            TLOGE("%s: [TEST] Rate package config failed", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Mixed packages test PASSED", LOG_TAG_TEST);
        return true;
    }

    // Test 4: 无效的混合节点（同一activity下同时有fps和rate）
    static bool testInvalidMixedNodes() {
        TLOGI("%s: [TEST] Testing invalid mixed nodes rejection...", LOG_TAG_TEST);

        if (!createInvalidMixedNodesXml()) {
            TLOGE("%s: [TEST] Failed to create invalid mixed nodes XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Should reject XML with both fps and rate in same activity",
                  LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Invalid mixed nodes rejection test PASSED", LOG_TAG_TEST);
        return true;
    }

    // Test 5: 无效的Rate值
    static bool testInvalidRateValues() {
        TLOGI("%s: [TEST] Testing invalid rate values rejection...", LOG_TAG_TEST);

        if (!createInvalidRateValuesXml()) {
            TLOGE("%s: [TEST] Failed to create invalid rate values XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Should reject XML with invalid rate values", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Invalid rate values rejection test PASSED", LOG_TAG_TEST);
        return true;
    }

    // Test 6: 缺失perflock节点
    static bool testMissingPerflockNode() {
        TLOGI("%s: [TEST] Testing missing perflock node rejection...", LOG_TAG_TEST);

        if (!createMissingPerflockXml()) {
            TLOGE("%s: [TEST] Failed to create missing perflock XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Should reject XML missing perflock node", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Missing perflock node rejection test PASSED", LOG_TAG_TEST);
        return true;
    }

    // Test 7: 空的activity节点
    static bool testEmptyActivityNode() {
        TLOGI("%s: [TEST] Testing empty activity node rejection...", LOG_TAG_TEST);

        if (!createEmptyActivityXml()) {
            TLOGE("%s: [TEST] Failed to create empty activity XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Should reject XML with empty activity node", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Empty activity node rejection test PASSED", LOG_TAG_TEST);
        return true;
    }

    // Test 8: 配置类型检测
    static bool testConfigTypeDetection() {
        TLOGI("%s: [TEST] Testing configuration type detection...", LOG_TAG_TEST);

        if (!createMixedPackagesTestXml()) {
            TLOGE("%s: [TEST] Failed to create test XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (!manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Failed to load config", LOG_TAG_TEST);
            return false;
        }

        // 测试FPS包的类型检测
        ConfigType fpsType = manager.getActivityConfigType("com.app.fps", "MainActivity");
        if (fpsType != ConfigType::FPS) {
            TLOGE("%s: [TEST] Wrong config type for FPS package", LOG_TAG_TEST);
            return false;
        }

        // 测试Rate包的类型检测
        ConfigType rateType = manager.getActivityConfigType("com.app.rate", "MainActivity");
        if (rateType != ConfigType::RATE) {
            TLOGE("%s: [TEST] Wrong config type for Rate package", LOG_TAG_TEST);
            return false;
        }

        // 测试不存在包的默认类型
        ConfigType defaultType = manager.getActivityConfigType("com.nonexistent", "Activity");
        if (defaultType != ConfigType::FPS) {
            TLOGE("%s: [TEST] Default config type should be FPS", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Config type detection test PASSED", LOG_TAG_TEST);
        return true;
    }

    // Test 9: 配置类型优先级（Rate > FPS）
    static bool testConfigTypePriority() {
        TLOGI("%s: [TEST] Testing configuration type priority...", LOG_TAG_TEST);

        if (!createMixedActivityTestXml()) {
            TLOGE("%s: [TEST] Failed to create mixed activity test XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (!manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Failed to load config", LOG_TAG_TEST);
            return false;
        }

        // 这个包的common activity应该优先选择Rate类型
        ConfigType type = manager.getActivityConfigType("com.mixed.app", "common");
        if (type != ConfigType::RATE) {
            TLOGE("%s: [TEST] Rate should have priority over FPS", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Config type priority test PASSED", LOG_TAG_TEST);
        return true;
    }

    // Test 10: Rate配置查询功能
    static bool testRateConfigQueries() {
        TLOGI("%s: [TEST] Testing Rate configuration queries...", LOG_TAG_TEST);

        if (!createComprehensiveRateTestXml()) {
            TLOGE("%s: [TEST] Failed to create comprehensive rate test XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (!manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Failed to load rate config", LOG_TAG_TEST);
            return false;
        }

        // 测试所有有效的rate值
        std::vector<std::string> validRateValues = {"60", "90", "120", "144", "common"};

        for (const auto &rateValue : validRateValues) {
            ConfigQueryResult result = manager.findConfig("com.rate.test", "common", rateValue);
            if (!result.found) {
                TLOGE("%s: [TEST] Should find rate config for %s", LOG_TAG_TEST, rateValue.c_str());
                return false;
            }

            if (result.configType != ConfigType::RATE) {
                TLOGE("%s: [TEST] Wrong config type for rate %s", LOG_TAG_TEST, rateValue.c_str());
                return false;
            }

            if (result.fpsValue != rateValue) {
                TLOGE("%s: [TEST] Rate value mismatch: expected %s, got %s", LOG_TAG_TEST,
                      rateValue.c_str(), result.fpsValue.c_str());
                return false;
            }
        }

        TLOGI("%s: [TEST] Rate config queries test PASSED", LOG_TAG_TEST);
        return true;
    }

    // Test 11: 回退机制测试
    static bool testFallbackMechanism() {
        TLOGI("%s: [TEST] Testing fallback mechanism...", LOG_TAG_TEST);

        if (!createFallbackTestXml()) {
            TLOGE("%s: [TEST] Failed to create fallback test XML", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;
        if (!manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Failed to load config", LOG_TAG_TEST);
            return false;
        }

        // 测试找不到特定activity时的回退到common
        ConfigQueryResult result =
            manager.findConfig("com.fallback.test", "UnknownActivity", "common");
        if (!result.found) {
            TLOGE("%s: [TEST] Should fallback to common activity", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Fallback mechanism test PASSED", LOG_TAG_TEST);
        return true;
    }

    // XML创建辅助函数
    static bool createBasicRateTestXml() {
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.game.rate">
        <activity name="common">
            <rate value="60">
                <perflock>0x41440100,0x5F,0x4143C100,1497</perflock>
            </rate>
            <rate value="120">
                <perflock>0x41440100,0x70,0x4143C100,1800</perflock>
            </rate>
            <rate value="144">
                <perflock>0x41440100,0x80,0x4143C100,2000</perflock>
            </rate>
        </activity>
    </package>
    <package name="com.video.rate">
        <activity name="common">
            <rate value="common">
                <perflock>0x40804000,2572,0x40804100,902</perflock>
            </rate>
        </activity>
    </package>
</configs>)";
        return writeXmlToFile(xmlContent);
    }

    static bool createBasicFpsTestXml() {
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.game.fps">
        <activity name="common">
            <fps value="60">
                <perflock>0x41440100,0x5F,0x4143C100,1497</perflock>
            </fps>
            <fps value="90">
                <perflock>0x41440100,0x60,0x4143C100,1600</perflock>
            </fps>
        </activity>
    </package>
    <package name="com.browser.fps">
        <activity name="common">
            <fps value="common">
                <perflock>0x40804000,2572,0x40804100,902</perflock>
            </fps>
        </activity>
    </package>
</configs>)";
        return writeXmlToFile(xmlContent);
    }

    static bool createMixedPackagesTestXml() {
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.app.fps">
        <activity name="MainActivity">
            <fps value="90">
                <perflock>0x41440100,0x60,0x4143C100,1600</perflock>
            </fps>
        </activity>
    </package>
    <package name="com.app.rate">
        <activity name="MainActivity">
            <rate value="144">
                <perflock>0x41440100,0x80,0x4143C100,2000</perflock>
            </rate>
        </activity>
    </package>
    <package name="com.app.common">
        <activity name="common">
            <fps value="common">
                <perflock>0x40804000,2572</perflock>
            </fps>
        </activity>
    </package>
</configs>)";
        return writeXmlToFile(xmlContent);
    }

    static bool createInvalidMixedNodesXml() {
        // 同一activity下同时有fps和rate节点（应该被拒绝）
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.invalid.mixed">
        <activity name="MainActivity">
            <fps value="60">
                <perflock>0x41440100,0x5F</perflock>
            </fps>
            <rate value="120">
                <perflock>0x41440100,0x70</perflock>
            </rate>
        </activity>
    </package>
</configs>)";
        return writeXmlToFile(xmlContent);
    }

    static bool createInvalidRateValuesXml() {
        // 包含无效rate值（应该被拒绝）
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.invalid.rate">
        <activity name="common">
            <rate value="75">
                <perflock>0x41440100,0x5F</perflock>
            </rate>
            <rate value="100">
                <perflock>0x41440100,0x60</perflock>
            </rate>
        </activity>
    </package>
</configs>)";
        return writeXmlToFile(xmlContent);
    }

    static bool createMissingPerflockXml() {
        // 缺少perflock节点（应该被拒绝）
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.missing.perflock">
        <activity name="common">
            <rate value="60">
                <!-- Missing perflock node -->
            </rate>
        </activity>
    </package>
</configs>)";
        return writeXmlToFile(xmlContent);
    }

    static bool createEmptyActivityXml() {
        // 空的activity节点（应该被拒绝）
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.empty.activity">
        <activity name="common">
            <!-- No fps or rate nodes -->
        </activity>
    </package>
</configs>)";
        return writeXmlToFile(xmlContent);
    }

    static bool createMixedActivityTestXml() {
        // 同一包下有多个activity，不同配置类型
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.mixed.app">
        <activity name="common">
            <rate value="120">
                <perflock>0x41440100,0x70,0x4143C100,1800</perflock>
            </rate>
        </activity>
        <activity name="VideoActivity">
            <fps value="60">
                <perflock>0x41440100,0x5F,0x4143C100,1497</perflock>
            </fps>
        </activity>
    </package>
</configs>)";
        return writeXmlToFile(xmlContent);
    }

    static bool createComprehensiveRateTestXml() {
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.rate.test">
        <activity name="common">
            <rate value="60">
                <perflock>0x41440100,0x5F,0x4143C100,1497</perflock>
            </rate>
            <rate value="90">
                <perflock>0x41440100,0x65,0x4143C100,1650</perflock>
            </rate>
            <rate value="120">
                <perflock>0x41440100,0x70,0x4143C100,1800</perflock>
            </rate>
            <rate value="144">
                <perflock>0x41440100,0x80,0x4143C100,2000</perflock>
            </rate>
            <rate value="common">
                <perflock>0x40804000,2572,0x40804100,902</perflock>
            </rate>
        </activity>
    </package>
</configs>)";
        return writeXmlToFile(xmlContent);
    }

    static bool createFallbackTestXml() {
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.fallback.test">
        <activity name="common">
            <rate value="common">
                <perflock>0x40804000,2572,0x40804100,902</perflock>
            </rate>
        </activity>
    </package>
</configs>)";
        return writeXmlToFile(xmlContent);
    }

    static bool writeXmlToFile(const char *content) {
        std::ofstream file("/data/vendor/perf/perf_config.xml");
        if (!file.is_open()) {
            TLOGE("%s: Failed to create test XML file", LOG_TAG_TEST);
            return false;
        }
        file << content;
        file.close();
        return true;
    }

    static void cleanup() {
        TLOGI("%s: Cleaning up test files...", LOG_TAG_TEST);
        remove("/data/vendor/perf/perf_config.xml");
    }
};

int main(int argc, char *argv[]) {
    TLOGI("%s: ConfigManager3 XML Parsing Test Binary", LOG_TAG_TEST);
    return ConfigManager3Test::runAllTests();
}
