/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DisplayRateManager.h"

#include <aidl/vendor/qti/hardware/display/config/Attributes.h>
#include <aidl/vendor/qti/hardware/display/config/DisplayType.h>
#include <aidl/vendor/qti/hardware/display/config/IDisplayConfig.h>
#include <android/binder_manager.h>

#include <chrono>
#include <thread>

using namespace aidl::vendor::qti::hardware::display::config;
using namespace std::chrono;

#undef LOG_TAG
#define LOG_TAG "PERF-CONFIG-DISPLAY-RATE-MGR"

namespace perfconfig {

DisplayRateManager &DisplayRateManager::getInstance() {
    static DisplayRateManager instance;
    return instance;
}

DisplayRateManager::DisplayRateManager()
    : mDisplayConfig(nullptr),
      mMonitoringActive(false),
      mMonitorIntervalMs(500),
      mLastNotifiedRate(-1) {
    mCachedInfo.clear();
    memset(&mStats, 0, sizeof(mStats));
    TLOGI("%s: DisplayRateManager created", LOG_TAG);
}

DisplayRateManager::~DisplayRateManager() {
    stopRateMonitoring();
    cleanup();
    TLOGI("%s: DisplayRateManager destroyed", LOG_TAG);
}

bool DisplayRateManager::initialize() {
    std::lock_guard<std::mutex> lock(mDisplayConfigMutex);

    if (mDisplayConfig) {
        TLOGD("%s: Already initialized", LOG_TAG);
        return true;
    }

    if (!connectToDisplayConfig()) {
        TLOGE("%s: Failed to connect to Display Config service", LOG_TAG);
        return false;
    }

    // 初始化成功后，获取一次显示信息用于验证
    auto info = queryDisplayRateInfoInternal();
    if (!info.isValid) {
        TLOGW("%s: Initial display rate query failed, but interface is connected", LOG_TAG);
    } else {
        TLOGI("%s: Initialized successfully with %s", LOG_TAG, info.toString().c_str());
    }

    return true;
}

void DisplayRateManager::cleanup() {
    std::lock_guard<std::mutex> lock(mDisplayConfigMutex);

    if (mDisplayConfig) {
        mDisplayConfig.reset();
        TLOGD("%s: Display Config interface cleaned up", LOG_TAG);
    }

    std::lock_guard<std::mutex> cacheLock(mCacheMutex);
    mCachedInfo.clear();
}

bool DisplayRateManager::connectToDisplayConfig() {
    const std::string serviceName = "vendor.qti.hardware.display.config.IDisplayConfig/default";

    TLOGD("%s: Connecting to Display Config service: %s", LOG_TAG, serviceName.c_str());

    ndk::SpAIBinder binder(AServiceManager_checkService(serviceName.c_str()));
    if (binder.get() == nullptr) {
        TLOGE("%s: Display Config AIDL service not available: %s", LOG_TAG, serviceName.c_str());
        return false;
    }

    mDisplayConfig = IDisplayConfig::fromBinder(binder);
    if (mDisplayConfig == nullptr) {
        TLOGE("%s: Failed to create Display Config interface from binder", LOG_TAG);
        return false;
    }

    TLOGI("%s: Successfully connected to Display Config service", LOG_TAG);
    return true;
}

int DisplayRateManager::getCurrentDisplayRate() {
    // 先检查缓存
    {
        std::lock_guard<std::mutex> lock(mCacheMutex);
        uint64_t currentTime = getCurrentTimeMs();
        if (mCachedInfo.isValid &&
            (currentTime - mCachedInfo.lastUpdateTime) < CACHE_VALID_DURATION_MS) {
            TLOGV("%s: Returning cached rate: %d", LOG_TAG, mCachedInfo.currentRate);
            return mCachedInfo.currentRate;
        }
    }

    // 缓存无效，重新查询
    int rate = queryDisplayRateInternal();

    // 更新缓存
    if (rate > 0) {
        std::lock_guard<std::mutex> lock(mCacheMutex);
        mCachedInfo.currentRate = rate;
        mCachedInfo.isValid = true;
        mCachedInfo.lastUpdateTime = getCurrentTimeMs();
    }

    return rate;
}

DisplayRateManager::DisplayRateInfo DisplayRateManager::getDisplayRateInfo() {
    // 检查缓存
    {
        std::lock_guard<std::mutex> lock(mCacheMutex);
        uint64_t currentTime = getCurrentTimeMs();
        if (mCachedInfo.isValid &&
            (currentTime - mCachedInfo.lastUpdateTime) < CACHE_VALID_DURATION_MS) {
            TLOGV("%s: Returning cached info: %s", LOG_TAG, mCachedInfo.toString().c_str());
            return mCachedInfo;
        }
    }

    // 缓存无效，重新查询
    DisplayRateInfo info = queryDisplayRateInfoInternal();

    // 更新缓存
    if (info.isValid) {
        std::lock_guard<std::mutex> lock(mCacheMutex);
        mCachedInfo = info;
    }

    return info;
}

int DisplayRateManager::queryDisplayRateInternal() {
    auto startTime = high_resolution_clock::now();
    int rate = getDisplayConfigRate();
    auto endTime = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(endTime - startTime).count();
    updateStats(rate > 0, static_cast<uint64_t>(duration));

    if (rate > 0) {
        TLOGV("%s: Query successful, rate: %d Hz (took %lld μs)", LOG_TAG, rate,
              static_cast<long long>(duration));
    } else {
        TLOGW("%s: Query failed (took %lld μs)", LOG_TAG, static_cast<long long>(duration));
    }

    return rate;
}

DisplayRateManager::DisplayRateInfo DisplayRateManager::queryDisplayRateInfoInternal() {
    DisplayRateInfo info;
    auto startTime = high_resolution_clock::now();

    bool success = getDisplayConfigInfo(info);

    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(endTime - startTime).count();
    updateStats(success, static_cast<uint64_t>(duration));

    if (success) {
        info.lastUpdateTime = getCurrentTimeMs();
        TLOGV("%s: Info query successful: %s (took %lld μs)", LOG_TAG, info.toString().c_str(),
              static_cast<long long>(duration));
    } else {
        TLOGW("%s: Info query failed (took %lld μs)", LOG_TAG, static_cast<long long>(duration));
        info.clear();
    }

    return info;
}

int DisplayRateManager::getDisplayConfigRate() {
    std::lock_guard<std::mutex> lock(mDisplayConfigMutex);

    if (!mDisplayConfig) {
        TLOGE("%s: Display Config interface not initialized", LOG_TAG);
        return -1;
    }

    int32_t displayIndex = -1;
    Attributes displayAttr;

    // 获取主显示器的活跃配置索引
    auto result = mDisplayConfig->getActiveConfig(DisplayType::PRIMARY, &displayIndex);
    if (!result.isOk()) {
        TLOGE("%s: Failed to get active config: %s", LOG_TAG, result.getMessage());
        return -1;
    }

    if (displayIndex < 0) {
        TLOGE("%s: Invalid display index: %d", LOG_TAG, displayIndex);
        return -1;
    }

    // 获取显示属性
    result = mDisplayConfig->getDisplayAttributes(displayIndex, DisplayType::PRIMARY, &displayAttr);
    if (!result.isOk()) {
        TLOGE("%s: Failed to get display attributes: %s", LOG_TAG, result.getMessage());
        return -1;
    }

    // 计算刷新率：1秒 = 1,000,000,000纳秒
    if (displayAttr.vsyncPeriod <= 0) {
        TLOGE("%s: Invalid vsync period: %d", LOG_TAG, displayAttr.vsyncPeriod);
        return -1;
    }

    static const int64_t NSEC_PER_SEC = 1000000000LL;
    int rate =
        static_cast<int>((NSEC_PER_SEC + displayAttr.vsyncPeriod / 2) / displayAttr.vsyncPeriod);

    TLOGD("%s: Display attributes - vsyncPeriod: %d ns, calculated rate: %d Hz", LOG_TAG,
          displayAttr.vsyncPeriod, rate);

    return rate;
}

bool DisplayRateManager::getDisplayConfigInfo(DisplayRateInfo &info) {
    info.clear();

    // 获取当前刷新率
    int currentRate = getDisplayConfigRate();
    if (currentRate <= 0) {
        return false;
    }

    info.currentRate = currentRate;
    info.isValid = true;

    // 填充常见的支持刷新率（基于经验值）
    // 实际项目中可能需要通过其他接口获取支持的刷新率列表
    const int commonRates[] = {60, 90, 120, 144};
    int rateCount = 0;

    for (int rate : commonRates) {
        if (rateCount < 8) {    // 最多支持8个刷新率
            info.supportedRates[rateCount++] = rate;
        }
    }
    info.supportedRateCount = rateCount;

    return true;
}

bool DisplayRateManager::refreshDisplayConfig() {
    std::lock_guard<std::mutex> lock(mCacheMutex);
    mCachedInfo.clear();    // 清空缓存，强制重新查询
    TLOGD("%s: Display config cache cleared", LOG_TAG);
    return true;
}

bool DisplayRateManager::isAvailable() const {
    std::lock_guard<std::mutex> lock(mDisplayConfigMutex);
    return mDisplayConfig != nullptr;
}

void DisplayRateManager::registerRateChangeCallback(RateChangeCallback callback,
                                                    uint32_t intervalMs) {
    mRateChangeCallback = callback;
    mMonitorIntervalMs = intervalMs;

    if (callback && !mMonitoringActive.load()) {
        startRateMonitoring();
    }

    TLOGI("%s: Rate change callback registered (interval: %u ms)", LOG_TAG, intervalMs);
}

void DisplayRateManager::unregisterRateChangeCallback() {
    stopRateMonitoring();
    mRateChangeCallback = nullptr;
    TLOGI("%s: Rate change callback unregistered", LOG_TAG);
}

bool DisplayRateManager::startRateMonitoring() {
    if (mMonitoringActive.load()) {
        TLOGD("%s: Rate monitoring already active", LOG_TAG);
        return true;
    }

    if (!isAvailable()) {
        TLOGE("%s: Cannot start monitoring - Display Config not available", LOG_TAG);
        return false;
    }

    mMonitoringActive.store(true);
    mLastNotifiedRate = -1;

    // 使用std::thread的构造函数错误检查
    mMonitorThread = std::thread(&DisplayRateManager::monitoringThreadFunction, this);

    // 检查线程是否成功创建
    if (!mMonitorThread.joinable()) {
        mMonitoringActive.store(false);
        TLOGE("%s: Failed to create monitoring thread", LOG_TAG);
        return false;
    }

    // 更新统计信息
    {
        std::lock_guard<std::mutex> lock(mStatsMutex);
        mStats.monitoringActive = true;
    }

    TLOGI("%s: Rate monitoring started (interval: %u ms)", LOG_TAG, mMonitorIntervalMs);
    return true;
}

void DisplayRateManager::stopRateMonitoring() {
    if (!mMonitoringActive.load()) {
        return;
    }

    mMonitoringActive.store(false);

    if (mMonitorThread.joinable()) {
        mMonitorThread.join();
    }

    // 更新统计信息
    {
        std::lock_guard<std::mutex> lock(mStatsMutex);
        mStats.monitoringActive = false;
    }

    TLOGI("%s: Rate monitoring stopped", LOG_TAG);
}

void DisplayRateManager::monitoringThreadFunction() {
    TLOGI("%s: Monitoring thread started", LOG_TAG);

    while (mMonitoringActive.load()) {
        int currentRate = getCurrentDisplayRate();

        if (currentRate > 0 && currentRate != mLastNotifiedRate) {
            if (mRateChangeCallback) {
                TLOGI("%s: Display rate changed: %d -> %d Hz", LOG_TAG, mLastNotifiedRate,
                      currentRate);

                // 安全调用回调函数 - 使用函数指针检查而非异常
                if (mRateChangeCallback) {
                    mRateChangeCallback(currentRate);
                }
            }
            mLastNotifiedRate = currentRate;

            // 更新统计信息
            std::lock_guard<std::mutex> lock(mStatsMutex);
            mStats.lastKnownRate = currentRate;
        }

        // 等待指定间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(mMonitorIntervalMs));
    }

    TLOGI("%s: Monitoring thread ended", LOG_TAG);
}

std::string DisplayRateManager::normalizeRateValue(int rate) {
    // 将刷新率标准化为配置值
    if (rate >= 140) {
        return "144";
    } else if (rate >= 115) {
        return "120";
    } else if (rate >= 85) {
        return "90";
    } else if (rate >= 55) {
        return "60";
    } else if (rate >= 40) {
        return "45";    // 如果支持45Hz
    } else if (rate >= 25) {
        return "30";    // 如果支持30Hz
    } else {
        return "common";    // 默认通用配置
    }
}

void DisplayRateManager::updateStats(bool success, uint64_t queryTimeUs) {
    std::lock_guard<std::mutex> lock(mStatsMutex);

    mStats.queryCount++;
    if (success) {
        mStats.successCount++;
    } else {
        mStats.errorCount++;
    }

    mStats.lastQueryTime = queryTimeUs;
    mStats.totalQueryTime += queryTimeUs;
}

uint64_t DisplayRateManager::getCurrentTimeMs() const {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

DisplayRateManager::RateManagerStats DisplayRateManager::getStats() const {
    std::lock_guard<std::mutex> lock(mStatsMutex);
    return mStats;
}

void DisplayRateManager::dumpStats() const {
    auto stats = getStats();

    TLOGI("%s: ========== DisplayRateManager Stats ==========", LOG_TAG);
    TLOGI("%s: Query Count: %u", LOG_TAG, stats.queryCount);
    TLOGI("%s: Success Count: %u", LOG_TAG, stats.successCount);
    TLOGI("%s: Error Count: %u", LOG_TAG, stats.errorCount);
    TLOGI("%s: Success Rate: %.2f%%", LOG_TAG, stats.getSuccessRate() * 100.0);
    TLOGI("%s: Average Query Time: %.2f μs", LOG_TAG, stats.getAverageQueryTime());
    TLOGI("%s: Last Query Time: %llu μs", LOG_TAG,
          static_cast<unsigned long long>(stats.lastQueryTime));
    TLOGI("%s: Last Known Rate: %d Hz", LOG_TAG, stats.lastKnownRate);
    TLOGI("%s: Monitoring Active: %s", LOG_TAG, stats.monitoringActive ? "true" : "false");
    TLOGI("%s: ==============================================", LOG_TAG);
}

}    // namespace perfconfig
