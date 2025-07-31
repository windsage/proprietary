/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GLOBAL_FPS_MONITOR_H
#define GLOBAL_FPS_MONITOR_H

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

class GlobalFpsMonitor {
private:
    static GlobalFpsMonitor *sInstance;
    static std::mutex sInstanceMutex;

    std::thread mMonitorThread;
    std::atomic<bool> mMonitoring;
    float mLastValidFps;

    std::unordered_map<std::string, std::function<void(float)>> mCallbacks;
    std::mutex mCallbackMutex;

    GlobalFpsMonitor();
    ~GlobalFpsMonitor();

    void monitorThreadFunc();
    float parseFpsFromNode() const;
    void startMonitoringIfNeeded();
    void stopMonitoringIfNotNeeded();

public:
    GlobalFpsMonitor(const GlobalFpsMonitor &) = delete;
    GlobalFpsMonitor &operator=(const GlobalFpsMonitor &) = delete;
    GlobalFpsMonitor(GlobalFpsMonitor &&) = delete;
    GlobalFpsMonitor &operator=(GlobalFpsMonitor &&) = delete;

    static GlobalFpsMonitor &getInstance();
    void registerCallback(const std::string &packageName, std::function<void(float)> callback);
    void unregisterCallback(const std::string &packageName);
};

#endif /* GLOBAL_FPS_MONITOR_H */
