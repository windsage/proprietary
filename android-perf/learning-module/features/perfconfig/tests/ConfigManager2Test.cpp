/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>

#include "../include/ConfigManager.h"
#include "../include/PerfConfigTypes.h"

#define LOG_TAG_TEST "PERF-CONFIG-MGR2-LIB-TEST"

using namespace perfconfig;

class ConfigManager2LibTest {
public:
    static int runAllTests() {
        TLOGI("%s: ======== ConfigManager2 Library Test Started ========", LOG_TAG_TEST);

        bool allPassed = true;
        int testCount = 0;
        int passedCount = 0;

        if (!setupTestEnvironment()) {
            TLOGE("%s: Failed to setup test environment", LOG_TAG_TEST);
            return 1;
        }

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
        system("mkdir -p /data/vendor/perf");
        system("mkdir -p /vendor/etc/perf");
        return true;
    }

    static bool testBasicParsing() {
        TLOGI("%s: [TEST] Testing basic XML parsing with library...", LOG_TAG_TEST);

        if (!createBasicTestXmlFile()) {
            TLOGE("%s: [TEST] Failed to create test XML file", LOG_TAG_TEST);
            return false;
        }

        ConfigManager manager;

        if (!manager.loadConfig("dummy_path")) {
            TLOGE("%s: [TEST] Failed to load test config", LOG_TAG_TEST);
            return false;
        }

        if (!manager.isLoaded()) {
            TLOGE("%s: [TEST] Manager should be loaded", LOG_TAG_TEST);
            return false;
        }

        if (manager.getPackageCount() != 2) {
            TLOGE("%s: [TEST] Expected 2 packages, got %zu", LOG_TAG_TEST,
                  manager.getPackageCount());
            return false;
        }

        ConfigQueryResult result = manager.findConfig("com.qiyi.video", "common", "60");
        if (!result.found) {
            TLOGE("%s: [TEST] Should find com.qiyi.video::common@60", LOG_TAG_TEST);
            return false;
        }

        if (result.perfLock.opcodes.empty()) {
            TLOGE("%s: [TEST] Should have opcodes in result", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Found %zu opcodes", LOG_TAG_TEST, result.perfLock.opcodes.size());

        manager.dumpConfig();

        TLOGI("%s: [TEST] Basic parsing test PASSED", LOG_TAG_TEST);
        return true;
    }

    static bool testQueryMethods() {
        TLOGI("%s: [TEST] Testing query methods...", LOG_TAG_TEST);

        ConfigManager manager;
        manager.loadConfig("dummy_path");

        if (!manager.hasPackage("com.qiyi.video")) {
            TLOGE("%s: [TEST] Should have package com.qiyi.video", LOG_TAG_TEST);
            return false;
        }

        if (manager.hasPackage("com.nonexistent")) {
            TLOGE("%s: [TEST] Should not have nonexistent package", LOG_TAG_TEST);
            return false;
        }

        if (!manager.hasActivity("com.qiyi.video", "common")) {
            TLOGE("%s: [TEST] Should have activity common", LOG_TAG_TEST);
            return false;
        }

        if (manager.hasActivity("com.qiyi.video", "NonexistentActivity")) {
            TLOGE("%s: [TEST] Should not have nonexistent activity", LOG_TAG_TEST);
            return false;
        }

        auto packageNames = manager.getPackageNames();
        if (packageNames.size() != 2) {
            TLOGE("%s: [TEST] Expected 2 package names", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Query methods test PASSED", LOG_TAG_TEST);
        return true;
    }

    static bool testStatistics() {
        TLOGI("%s: [TEST] Testing statistics...", LOG_TAG_TEST);

        ConfigManager manager;
        manager.loadConfig("dummy_path");

        if (manager.getQueryCount() != 0 || manager.getHitCount() != 0) {
            TLOGE("%s: [TEST] Initial statistics should be zero", LOG_TAG_TEST);
            return false;
        }

        manager.findConfig("com.qiyi.video", "common", "60");           // Hit
        manager.findConfig("com.taobao.taobao", "common", "common");    // Hit
        manager.findConfig("com.nonexistent", "Activity", "60");        // Miss

        if (manager.getQueryCount() != 3) {
            TLOGE("%s: [TEST] Expected 3 queries, got %u", LOG_TAG_TEST, manager.getQueryCount());
            return false;
        }

        if (manager.getHitCount() != 2) {
            TLOGE("%s: [TEST] Expected 2 hits, got %u", LOG_TAG_TEST, manager.getHitCount());
            return false;
        }

        manager.dumpStatistics();

        TLOGI("%s: [TEST] Statistics test PASSED", LOG_TAG_TEST);
        return true;
    }

    static bool testErrorHandling() {
        TLOGI("%s: [TEST] Testing error handling...", LOG_TAG_TEST);

        ConfigManager manager;

        if (manager.loadConfig("")) {
            TLOGE("%s: [TEST] Should not load empty path", LOG_TAG_TEST);
            return false;
        }

        remove("/data/vendor/perf/perf_config.xml");
        remove("/vendor/etc/perf/perf_config.xml");

        if (manager.loadConfig("/nonexistent/file.xml")) {
            TLOGE("%s: [TEST] Should not load when no config files exist", LOG_TAG_TEST);
            return false;
        }

        TLOGI("%s: [TEST] Error handling test PASSED", LOG_TAG_TEST);
        return true;
    }

    static bool createBasicTestXmlFile() {
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
            TLOGE("%s: Failed to create test XML file", LOG_TAG_TEST);
            return false;
        }

        file << xmlContent;
        file.close();

        TLOGI("%s: Created test XML file", LOG_TAG_TEST);
        return true;
    }

    static void cleanup() {
        TLOGI("%s: Cleaning up test files...", LOG_TAG_TEST);
        remove("/data/vendor/perf/perf_config.xml");
        remove("/vendor/etc/perf/perf_config.xml");
    }
};

int main(int argc, char *argv[]) {
    TLOGI("%s: ConfigManager2 Library Test Binary", LOG_TAG_TEST);

    return ConfigManager2LibTest::runAllTests();
}
