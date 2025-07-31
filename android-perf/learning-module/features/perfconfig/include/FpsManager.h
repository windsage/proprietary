/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPS_MANAGER_H
#define FPS_MANAGER_H

#include <string>
#include <vector>

class FpsManager {
public:
    static const uint32_t FPS_MONITOR_INTERVAL_MS = 1000;
    static const float FPS_DEBOUNCE_THRESHOLD;
    static const char *FPS_NODE_PATH;
    static const std::vector<int> FPS_THRESHOLDS;

    static std::string getFpsCategory(float actualFps);

    static bool shouldSwitchFpsConfig(const std::string &currentCategory,
                                      const std::string &newCategory, float currentFps,
                                      float newFps);

private:
    FpsManager() = delete;
};

#endif /* FPS_MANAGER_H */
