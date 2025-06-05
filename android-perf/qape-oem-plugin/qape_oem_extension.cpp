/******************************************************************************
  @file    qape_oem_extension.cpp
  @brief   Test code to serve as a template for oem extension plugin

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
*****************************************************************************/

#include "qape_oem_extension.h"
#include "PerfLog.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * If uncomment below code then can overwrite boost value for boost_cpu/gpu
 *
int oem_boost_cpu (int orig_val, std::string appName) {
    // check any system parameters
    SLOGD("OEM extension plugin boost cpu: %d, pkg: %s", orig_val, appName.c_str());
    // You can change return value after adding your own code
    // Example: return orig_val / 2;
    return orig_val;
}

int oem_boost_gpu (int orig_val, std::string appName) {
    // check any system parameters
    SLOGD("OEM extension plugin boost gpu: %d, pkg: %s", orig_val, appName.c_str());
    // You can change return value after adding your own code
    // Example return orig_val + 10;
    return orig_val;
}
*/

int oem_scenario_support (std::string appName, std::string scenario_id) {
    SLOGD("OEM extension plugin get scenario support: %s %s", appName.c_str(), scenario_id.c_str());
    int oem_extension_support = 1;
    if (oem_extension_support) {
        // Supporting OEM scenario
        return 1;
    }
    // Not supporting OEM scenario
    return 0;
}

int oem_start_scenario(std::string appName, std::string scenario_id, int * &plock) {
    int args = 0;
    if (scenario_id.compare("launch") == 0) {
        int x[] = { 0x40C00000, 0x1, 0x40804000, 0xFFF, 0x40804100, 0xFFF, 0x40804200, 0xFFF,
                    0x40800000, 0xFFF, 0x40800100, 0xFFF, 0x40800200, 0xFFF, 0x41848000, 0x104410,
                    0x41844000, 0x104410, 0x40400000, 0x1, 0x42C10000, 0x1, 0x43488000, 0x30D400,
                    0x43458000, 0x193138, 0x4281C000, 2000 };
        args = sizeof(x)/sizeof(x[0]);
        plock = (int *) malloc(sizeof(int) * args);
        if (plock == nullptr)
            return -1;
        for (int i = 0; i < args; i++) {
           plock[i] = x[i];
        }
    } else if (scenario_id.compare("scroll") == 0) {
        int x[] = { 0x4303C000, 0xA6428, 0x40800000, 1000, 0x40800100, 1000, 0x40800200, 1000,
                    0x40C00000, 0x2, 0x43458000, 0xD2F00, 0x40C74000, 0xFF, 0x40408000, 0x63,
                    0x41000000, 3 };
        args = sizeof(x)/sizeof(x[0]);
        plock = (int *) malloc(sizeof(int) * args);
        if (plock == nullptr)
            return -1;
        for (int i = 0; i < args; i++) {
           plock[i] = x[i];
        }

    }
    SLOGD("OEM extension plugin start scenario: %s %s %d", appName.c_str(), scenario_id.c_str(), args);
    return args;
}
int oem_stop_scenario(std::string appName, std::string scenario_id) {
    SLOGD("OEM extension plugin stop scenario: %s %s", appName.c_str(), scenario_id.c_str());
    return 0;
}

#ifdef __cplusplus
}
#endif
