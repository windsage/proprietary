/******************************************************************************
  @file    qape_oem_extension.h
  @brief   Library header for oem extension qape plugin

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
*****************************************************************************/

#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <log/log.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

int oem_boost_cpu (int orig_val, std::string pkg_name);
int oem_boost_gpu (int orig_val, std::string pkg_name);
int oem_scenario_support (std::string appName, std::string scenario_id);
int oem_start_scenario(std::string appName, std::string scenario_id, int * &plock);
int oem_stop_scenario(std::string appName, std::string scenario_id);

#ifdef __cplusplus
}
#endif

