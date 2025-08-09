/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DISPLAYRATEMANAGER_H
#define DISPLAYRATEMANAGER_H

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "TLog.h"

// 前向声明，避免在头文件中包含AIDL头文件
namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace display {
namespace config {
class IDisplayConfig;
}    // namespace config
}    // namespace display
}    // namespace hardware
}    // namespace qti
}    // namespace vendor
}    // namespace aidl

#define LOG_TAG_DISPLAY_RATE "DISPLAY-RATE-MGR"

namespace perfconfig {

/**
 * 显示刷新率管理器
 * 封装高通vendor.qti.hardware.display.config.IDisplayConfig接口
 * 提供刷新率获取和监控功能
 */
class DisplayRateManager {
public:
    /**
     * 刷新率变化回调函数类型
     * @param newRate 新的刷新率值（如60, 90, 120等）
     */
    using RateChangeCallback = std::function<void(int newRate)>;

    /**
     * 刷新率状态信息
     */
    struct DisplayRateInfo {
        int currentRate = 60;           // 当前刷新率
        int supportedRates[8] = {0};    // 支持的刷新率列表
        int supportedRateCount = 0;     // 支持的刷新率数量
        bool isValid = false;           // 信息是否有效
        uint64_t lastUpdateTime = 0;    // 最后更新时间（毫秒）

        void clear() {
            currentRate = 60;
            supportedRateCount = 0;
            isValid = false;
            lastUpdateTime = 0;
            memset(supportedRates, 0, sizeof(supportedRates));
        }

        std::string toString() const {
            std::string result =
                "DisplayRate{current=" + std::to_string(currentRate) + ", supported=[";
            for (int i = 0; i < supportedRateCount; i++) {
                if (i > 0)
                    result += ",";
                result += std::to_string(supportedRates[i]);
            }
            result += "], valid=" + std::string(isValid ? "true" : "false") + "}";
            return result;
        }
    };

    // 单例模式
    static DisplayRateManager &getInstance();

    // 禁用拷贝构造和赋值
    DisplayRateManager(const DisplayRateManager &) = delete;
    DisplayRateManager &operator=(const DisplayRateManager &) = delete;

    /**
     * 初始化Display Config接口
     * @return true 如果初始化成功
     */
    bool initialize();

    /**
     * 清理资源
     */
    void cleanup();

    /**
     * 获取当前显示刷新率
     * @return 当前刷新率（Hz），失败返回-1
     */
    int getCurrentDisplayRate();

    /**
     * 获取详细的显示刷新率信息
     * @return 显示刷新率信息结构
     */
    DisplayRateInfo getDisplayRateInfo();

    /**
     * 刷新显示配置信息（强制重新获取）
     * @return true 如果刷新成功
     */
    bool refreshDisplayConfig();

    /**
     * 检查接口是否可用
     * @return true 如果Display Config接口可用
     */
    bool isAvailable() const;

    /**
     * 注册刷新率变化回调
     * @param callback 回调函数
     * @param intervalMs 检查间隔（毫秒）
     */
    void registerRateChangeCallback(RateChangeCallback callback, uint32_t intervalMs = 500);

    /**
     * 取消刷新率变化监控
     */
    void unregisterRateChangeCallback();

    /**
     * 启动刷新率监控线程
     * @return true 如果启动成功
     */
    bool startRateMonitoring();

    /**
     * 停止刷新率监控线程
     */
    void stopRateMonitoring();

    /**
     * 将刷新率值标准化为配置值
     * @param rate 原始刷新率
     * @return 标准化的配置值字符串
     */
    static std::string normalizeRateValue(int rate);

    /**
     * 获取统计信息（用于调试）
     */
    struct RateManagerStats {
        uint32_t queryCount = 0;
        uint32_t successCount = 0;
        uint32_t errorCount = 0;
        uint64_t lastQueryTime = 0;
        uint64_t totalQueryTime = 0;
        int lastKnownRate = 60;
        bool monitoringActive = false;

        double getSuccessRate() const {
            return queryCount > 0 ? static_cast<double>(successCount) / queryCount : 0.0;
        }

        double getAverageQueryTime() const {
            return queryCount > 0 ? static_cast<double>(totalQueryTime) / queryCount : 0.0;
        }
    };

    RateManagerStats getStats() const;
    void dumpStats() const;

private:
    DisplayRateManager();
    ~DisplayRateManager();

    // Display Config接口相关
    std::shared_ptr<aidl::vendor::qti::hardware::display::config::IDisplayConfig> mDisplayConfig;
    mutable std::mutex mDisplayConfigMutex;

    // 缓存数据
    DisplayRateInfo mCachedInfo;
    mutable std::mutex mCacheMutex;
    static const uint32_t CACHE_VALID_DURATION_MS = 100;    // 缓存有效期100ms

    // 监控线程相关
    std::atomic<bool> mMonitoringActive;
    std::thread mMonitorThread;
    RateChangeCallback mRateChangeCallback;
    uint32_t mMonitorIntervalMs;
    int mLastNotifiedRate;

    // 统计信息
    mutable RateManagerStats mStats;
    mutable std::mutex mStatsMutex;

    // 内部方法
    bool connectToDisplayConfig();
    int queryDisplayRateInternal();
    DisplayRateInfo queryDisplayRateInfoInternal();
    void monitoringThreadFunction();
    void updateStats(bool success, uint64_t queryTime);
    uint64_t getCurrentTimeMs() const;

    // 高通Display Config接口调用
    int getDisplayConfigRate();
    bool getDisplayConfigInfo(DisplayRateInfo &info);
};

}    // namespace perfconfig

#endif /* DISPLAYRATEMANAGER_H */
