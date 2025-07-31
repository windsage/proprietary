/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include "GlobalFpsMonitor.h"

#include <chrono>
#include <fstream>
#include <sstream>

#include "FpsManager.h"
#include "TLog.h"

#undef LOG_TAG
#define LOG_TAG "PERF-CONFIG-FPS-MONITOR"

GlobalFpsMonitor *GlobalFpsMonitor::sInstance = nullptr;
std::mutex GlobalFpsMonitor::sInstanceMutex;

GlobalFpsMonitor::GlobalFpsMonitor() : mMonitoring(false), mLastValidFps(-1.0f) {
    TLOGI("GlobalFpsMonitor created");
}

GlobalFpsMonitor::~GlobalFpsMonitor() {
    if (mMonitoring.load()) {
        mMonitoring = false;
        if (mMonitorThread.joinable()) {
            mMonitorThread.join();
        }
    }
    TLOGI("GlobalFpsMonitor destroyed");
}

GlobalFpsMonitor &GlobalFpsMonitor::getInstance() {
    std::lock_guard<std::mutex> lock(sInstanceMutex);
    if (sInstance == nullptr) {
        sInstance = new GlobalFpsMonitor();
    }
    return *sInstance;
}

void GlobalFpsMonitor::registerCallback(const std::string &packageName,
                                        std::function<void(float)> callback) {
    std::lock_guard<std::mutex> lock(mCallbackMutex);
    mCallbacks[packageName] = callback;
    TLOGI("FPS callback registered for %s", packageName.c_str());

    startMonitoringIfNeeded();
}

void GlobalFpsMonitor::unregisterCallback(const std::string &packageName) {
    std::lock_guard<std::mutex> lock(mCallbackMutex);
    mCallbacks.erase(packageName);
    TLOGI("FPS callback unregistered for %s", packageName.c_str());

    stopMonitoringIfNotNeeded();
}

void GlobalFpsMonitor::startMonitoringIfNeeded() {
    if (!mMonitoring.load() && !mCallbacks.empty()) {
        mMonitoring = true;
        mMonitorThread = std::thread(&GlobalFpsMonitor::monitorThreadFunc, this);
        TLOGI("FPS monitoring started");
    }
}

void GlobalFpsMonitor::stopMonitoringIfNotNeeded() {
    if (mMonitoring.load() && mCallbacks.empty()) {
        mMonitoring = false;
        if (mMonitorThread.joinable()) {
            mMonitorThread.join();
        }
        TLOGI("FPS monitoring stopped");
    }
}

void GlobalFpsMonitor::monitorThreadFunc() {
    while (mMonitoring.load()) {
        float currentFps = parseFpsFromNode();
        TLOGI("FPS value from node is %.1f", currentFps);
        if (currentFps < 0 && mLastValidFps >= 0) {
            currentFps = mLastValidFps;
            TLOGW("FPS node read failed, using last valid FPS: %.1f", currentFps);
        } else if (currentFps >= 0) {
            mLastValidFps = currentFps;
        }

        {
            std::lock_guard<std::mutex> lock(mCallbackMutex);
            for (const auto &pair : mCallbacks) {
                if (currentFps >= 0) {
                    pair.second(currentFps);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(FpsManager::FPS_MONITOR_INTERVAL_MS));
    }
}

float GlobalFpsMonitor::parseFpsFromNode() const {
    std::ifstream file(FpsManager::FPS_NODE_PATH);
    if (!file.is_open()) {
        TLOGE("Failed to open FPS node: %s", FpsManager::FPS_NODE_PATH);
        return -1.0f;
    }

    std::string line;
    if (std::getline(file, line)) {
        // Format: "fps: 141.9 duration:1000000 frame_count:142"
        std::istringstream iss(line);
        std::string token;

        if (iss >> token && token == "fps:") {
            float fps;
            if (iss >> fps) {
                TLOGV("Parsed FPS: %.1f", fps);
                return fps;
            }
        }
    }

    TLOGE("Failed to parse FPS from node content: %s", line.c_str());
    return -1.0f;
}
