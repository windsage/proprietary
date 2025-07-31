/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>

#include "../include/ConfigManager.h"
#include "../include/TLog.h"

#define LOG_TAG_STANDALONE_TEST "PERF-CONFIG-MGR2-STANDALONE-TEST"

class ConfigManagerTest {
public:
    static int runAllTests() {
        TLOGI("%s: ======== ConfigManager Standalone Test Started ========",
              LOG_TAG_STANDALONE_TEST);

        bool allPassed = true;
        int testCount = 0;
        int passedCount = 0;

        // Ensure test directories exist
        if (!setupTestEnvironment()) {
            TLOGE("%s: Failed to setup test environment", LOG_TAG_STANDALONE_TEST);
            return 1;
        }

        // Run individual tests
        testCount++;
        if (testBasicParsing())
            passedCount++;
        testCount++;
        if (testQueryMethods())
            passedCount++;
        testCount++;
        if (testStatistics())
            passedCount++;
        testCount++;
        if (testErrorHandling())
            passedCount++;
        testCount++;
        if (testConfigFileManager())
            passedCount++;

        // Cleanup test files
        cleanup();

        TLOGI("%s: ======== Test Results ========", LOG_TAG_STANDALONE_TEST);
        TLOGI("%s: Total Tests: %d", LOG_TAG_STANDALONE_TEST, testCount);
        TLOGI("%s: Passed: %d", LOG_TAG_STANDALONE_TEST, passedCount);
        TLOGI("%s: Failed: %d", LOG_TAG_STANDALONE_TEST, testCount - passedCount);
        TLOGI("%s: Success Rate: %.1f%%", LOG_TAG_STANDALONE_TEST,
              (passedCount * 100.0) / testCount);

        bool allTestsPassed = (passedCount == testCount);
        TLOGI("%s: ======== Test %s ========", LOG_TAG_STANDALONE_TEST,
              allTestsPassed ? "PASSED" : "FAILED");

        return allTestsPassed ? 0 : 1;
    }

private:
    static bool setupTestEnvironment() {
        // Create test directories if they don't exist
        if (mkdir("/data/vendor", 0755) != 0 && errno != EEXIST) {
            TLOGE("%s: Failed to create /data/vendor directory", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        if (mkdir("/data/vendor/perf", 0755) != 0 && errno != EEXIST) {
            TLOGE("%s: Failed to create /data/vendor/perf directory", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        if (mkdir("/vendor/etc", 0755) != 0 && errno != EEXIST) {
            // This might fail on some systems, but that's OK
            TLOGW("%s: Warning: Failed to create /vendor/etc directory", LOG_TAG_STANDALONE_TEST);
        }

        if (mkdir("/vendor/etc/perf", 0755) != 0 && errno != EEXIST) {
            // This might fail on some systems, but that's OK
            TLOGW("%s: Warning: Failed to create /vendor/etc/perf directory",
                  LOG_TAG_STANDALONE_TEST);
        }

        TLOGI("%s: Test environment setup completed", LOG_TAG_STANDALONE_TEST);
        return true;
    }

    static bool testBasicParsing() {
        TLOGI("%s: [TEST] Testing basic XML parsing...", LOG_TAG_STANDALONE_TEST);

        // Create test XML file with correct format
        if (!createBasicTestXmlFile()) {
            TLOGE("%s: [TEST] Failed to create test XML file", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        perfconfig::ConfigManager manager;

        // Test loading config
        if (!manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Failed to load test config", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        // Verify basic properties
        if (!manager.isLoaded()) {
            TLOGE("%s: [TEST] Manager should be loaded", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        if (manager.getPackageCount() != 2) {
            TLOGE("%s: [TEST] Expected 2 packages, got %zu", LOG_TAG_STANDALONE_TEST,
                  manager.getPackageCount());
            return false;
        }

        // Test package names
        auto packageNames = manager.getPackageNames();
        if (packageNames.size() != 2) {
            TLOGE("%s: [TEST] Expected 2 package names", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Package names found: %s, %s", LOG_TAG_STANDALONE_TEST,
              packageNames[0].c_str(), packageNames[1].c_str());

        // Dump config for verification
        manager.dumpConfig();

        TLOGI("%s: [TEST] Basic parsing test PASSED", LOG_TAG_STANDALONE_TEST);
        return true;
    }

    static bool testQueryMethods() {
        TLOGI("%s: [TEST] Testing query methods...", LOG_TAG_STANDALONE_TEST);

        perfconfig::ConfigManager manager;
        manager.loadConfig("dummy_path");

        // Test valid query
        auto result = manager.findConfig("com.qiyi.video", "common", "60");
        if (!result.found) {
            TLOGE("%s: [TEST] Should find valid config for com.qiyi.video::common@60",
                  LOG_TAG_STANDALONE_TEST);
            return false;
        }

        if (result.packageName != "com.qiyi.video" || result.activityName != "common" ||
            result.fpsValue != "60") {
            TLOGE("%s: [TEST] Query result has wrong values", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        if (result.perfLock.opcodes.empty()) {
            TLOGE("%s: [TEST] Should have opcodes in result", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Found %zu opcodes for valid query", LOG_TAG_STANDALONE_TEST,
              result.perfLock.opcodes.size());

        // Test invalid query
        auto invalidResult = manager.findConfig("com.nonexistent", "Activity", "30");
        if (invalidResult.found) {
            TLOGE("%s: [TEST] Should not find invalid config", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        // Test has methods
        if (!manager.hasPackage("com.qiyi.video")) {
            TLOGE("%s: [TEST] Should have package com.qiyi.video", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        if (manager.hasPackage("com.nonexistent")) {
            TLOGE("%s: [TEST] Should not have nonexistent package", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        if (!manager.hasActivity("com.qiyi.video", "common")) {
            TLOGE("%s: [TEST] Should have activity common", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        if (manager.hasActivity("com.qiyi.video", "NonexistentActivity")) {
            TLOGE("%s: [TEST] Should not have nonexistent activity", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Query methods test PASSED", LOG_TAG_STANDALONE_TEST);
        return true;
    }

    static bool testStatistics() {
        TLOGI("%s: [TEST] Testing statistics...", LOG_TAG_STANDALONE_TEST);

        perfconfig::ConfigManager manager;
        manager.loadConfig("dummy_path");

        // Initial statistics should be zero
        if (manager.getQueryCount() != 0 || manager.getHitCount() != 0) {
            TLOGE("%s: [TEST] Initial statistics should be zero", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        // Perform some queries
        manager.findConfig("com.qiyi.video", "common", "60");           // Hit
        manager.findConfig("com.taobao.taobao", "common", "common");    // Hit
        manager.findConfig("com.nonexistent", "Activity", "60");        // Miss

        if (manager.getQueryCount() != 3) {
            TLOGE("%s: [TEST] Expected 3 queries, got %u", LOG_TAG_STANDALONE_TEST,
                  manager.getQueryCount());
            return false;
        }

        if (manager.getHitCount() != 2) {
            TLOGE("%s: [TEST] Expected 2 hits, got %u", LOG_TAG_STANDALONE_TEST,
                  manager.getHitCount());
            return false;
        }

        double expectedHitRate = 2.0 / 3.0;
        double actualHitRate = manager.getHitRate();
        if (actualHitRate < expectedHitRate - 0.01 || actualHitRate > expectedHitRate + 0.01) {
            TLOGE("%s: [TEST] Expected hit rate %.2f, got %.2f", LOG_TAG_STANDALONE_TEST,
                  expectedHitRate, actualHitRate);
            return false;
        }

        manager.dumpStatistics();

        TLOGI("%s: [TEST] Statistics test PASSED", LOG_TAG_STANDALONE_TEST);
        return true;
    }

    static bool testErrorHandling() {
        TLOGI("%s: [TEST] Testing error handling...", LOG_TAG_STANDALONE_TEST);

        perfconfig::ConfigManager manager;

        // Test empty path
        if (manager.loadConfig("")) {
            TLOGE("%s: [TEST] Should not load empty path", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        // Test nonexistent file (remove test file first)
        remove("/data/vendor/perf/perf_config.xml");
        remove("/vendor/etc/perf/perf_config.xml");

        if (manager.loadConfig("/nonexistent/file.xml")) {
            TLOGE("%s: [TEST] Should not load when no config files exist", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        // Test invalid XML
        if (!createInvalidXmlFile()) {
            TLOGE("%s: [TEST] Failed to create invalid XML file", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        perfconfig::ConfigManager manager2;
        if (manager2.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Should not load invalid XML", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        // Test reload without previous config
        perfconfig::ConfigManager manager3;
        if (manager3.reloadConfig()) {
            TLOGE("%s: [TEST] Should not reload without previous config", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Error handling test PASSED", LOG_TAG_STANDALONE_TEST);
        return true;
    }

    static bool testConfigFileManager() {
        TLOGI("%s: [TEST] Testing ConfigFileManager integration...", LOG_TAG_STANDALONE_TEST);

        // Create test files in both locations
        if (!createVendorTestFile() || !createDataTestFile()) {
            TLOGE("%s: [TEST] Failed to create test files", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        perfconfig::ConfigManager manager;

        // Should load the data file (higher priority)
        if (!manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Should load config from data directory", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        // Verify it loaded the data version (has 3 packages vs 2 in vendor)
        if (manager.getPackageCount() != 3) {
            TLOGE("%s: [TEST] Expected 3 packages from data file, got %zu", LOG_TAG_STANDALONE_TEST,
                  manager.getPackageCount());
            return false;
        }

        // Remove data file and test fallback to vendor
        remove("/data/vendor/perf/perf_config.xml");

        perfconfig::ConfigManager manager2;
        if (!manager2.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Should load config from vendor directory", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        if (manager2.getPackageCount() != 2) {
            TLOGE("%s: [TEST] Expected 2 packages from vendor file, got %zu",
                  LOG_TAG_STANDALONE_TEST, manager2.getPackageCount());
            return false;
        }

        TLOGI("%s: [TEST] ConfigFileManager integration test PASSED", LOG_TAG_STANDALONE_TEST);
        return true;
    }

    static bool createBasicTestXmlFile() {
        // Create a simplified version of the actual perf_config.xml format
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.qiyi.video">
        <activity name="common">
            <fps value="60">
                <perflock>0x41440100,0x5F,0x4143C100,1497,0x40804300,1190</perflock>
            </fps>
            <fps value="90">
                <perflock>0x41440100,0x5F,0x4143C100,1497,0x40804300,1190</perflock>
            </fps>
        </activity>
    </package>
    <package name="com.taobao.taobao">
        <activity name="common">
            <fps value="common">
                <perflock>0x40804000,2572,0x40804100,902,0x40804200,672</perflock>
            </fps>
        </activity>
    </package>
</configs>)";

        std::ofstream file("/data/vendor/perf/perf_config.xml");
        if (!file.is_open()) {
            TLOGE("%s: Failed to create basic test XML file", LOG_TAG_STANDALONE_TEST);
            return false;
        }

        file << xmlContent;
        file.close();

        TLOGI("%s: Created basic test XML file", LOG_TAG_STANDALONE_TEST);
        return true;
    }

    static bool createInvalidXmlFile() {
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<invalid_root>
    <package name="test">
        <activity name="test">
            <!-- Missing closing tag and wrong root -->
        </activity>
    </package>
</invalid_root>)";

        std::ofstream file("/data/vendor/perf/perf_config.xml");
        if (!file.is_open()) {
            return false;
        }

        file << xmlContent;
        file.close();
        return true;
    }

    static bool createVendorTestFile() {
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.vendor.app1">
        <activity name="common">
            <fps value="60">
                <perflock>0x40800000,0x1</perflock>
            </fps>
        </activity>
    </package>
    <package name="com.vendor.app2">
        <activity name="common">
            <fps value="30">
                <perflock>0x40800000,0x2</perflock>
            </fps>
        </activity>
    </package>
</configs>)";

        std::ofstream file("/vendor/etc/perf/perf_config.xml");
        if (!file.is_open()) {
            return false;
        }

        file << xmlContent;
        file.close();
        return true;
    }

    static bool createDataTestFile() {
        const char *xmlContent = R"(<?xml version="1.0" encoding="utf-8"?>
<configs>
    <package name="com.data.app1">
        <activity name="common">
            <fps value="60">
                <perflock>0x50800000,0x1</perflock>
            </fps>
        </activity>
    </package>
    <package name="com.data.app2">
        <activity name="common">
            <fps value="30">
                <perflock>0x50800000,0x2</perflock>
            </fps>
        </activity>
    </package>
    <package name="com.data.app3">
        <activity name="common">
            <fps value="90">
                <perflock>0x50800000,0x3</perflock>
            </fps>
        </activity>
    </package>
</configs>)";

        std::ofstream file("/data/vendor/perf/perf_config.xml");
        if (!file.is_open()) {
            return false;
        }

        file << xmlContent;
        file.close();
        return true;
    }

    static void cleanup() {
        TLOGI("%s: Cleaning up test files...", LOG_TAG_STANDALONE_TEST);
        remove("/data/vendor/perf/perf_config.xml");
        remove("/vendor/etc/perf/perf_config.xml");
    }
};

// Main function for standalone binary
int main(int argc, char *argv[]) {
    TLOGI("%s: ConfigManager Standalone Test Binary", LOG_TAG_STANDALONE_TEST);
    TLOGI("%s: Usage: %s", LOG_TAG_STANDALONE_TEST, argv[0]);

    return ConfigManagerTest::runAllTests();
}
