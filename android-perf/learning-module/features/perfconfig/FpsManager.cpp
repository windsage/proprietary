/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FpsManager.h"

#include "TLog.h"

#undef LOG_TAG
#define LOG_TAG "PERF-CONFIG-FPS-MANAGER"

const float FpsManager::FPS_DEBOUNCE_THRESHOLD = 2.0f;
const char *FpsManager::FPS_NODE_PATH = "/sys/class/drm/sde-crtc-0/measured_fps";
const std::vector<int> FpsManager::FPS_THRESHOLDS = {30, 60, 90, 120, 144};

std::string FpsManager::getFpsCategory(float actualFps) {
    if (actualFps == 0.0f || actualFps == -1.0f) {
        return "common";
    }

    if (actualFps <= 30)
        return "30";
    if (actualFps <= 60)
        return "60";
    if (actualFps <= 90)
        return "90";
    if (actualFps <= 120)
        return "120";
    return "144";
}

bool FpsManager::shouldSwitchFpsConfig(const std::string &currentCategory,
                                       const std::string &newCategory, float currentFps,
                                       float newFps) {
    if (currentCategory == newCategory) {
        return false;
    }

    if (currentCategory == "30" && newCategory == "60") {
        return newFps > (30 + FPS_DEBOUNCE_THRESHOLD);
    }
    if (currentCategory == "60" && newCategory == "30") {
        return newFps < (30 - FPS_DEBOUNCE_THRESHOLD);
    }
    if (currentCategory == "60" && newCategory == "90") {
        return newFps > (60 + FPS_DEBOUNCE_THRESHOLD);
    }
    if (currentCategory == "90" && newCategory == "60") {
        return newFps < (60 - FPS_DEBOUNCE_THRESHOLD);
    }
    if (currentCategory == "90" && newCategory == "120") {
        return newFps > (90 + FPS_DEBOUNCE_THRESHOLD);
    }
    if (currentCategory == "120" && newCategory == "90") {
        return newFps < (90 - FPS_DEBOUNCE_THRESHOLD);
    }
    if (currentCategory == "120" && newCategory == "144") {
        return newFps > (120 + FPS_DEBOUNCE_THRESHOLD);
    }
    if (currentCategory == "144" && newCategory == "120") {
        return newFps < (120 - FPS_DEBOUNCE_THRESHOLD);
    }

    return true;
}
