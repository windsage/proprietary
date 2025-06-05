/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <string>
#include <vector>
#include <Status.hpp>

typedef enum {
    IS_FILE, // path is a file
    IS_DIR,  // path is a directory
    DNE      // path doesn't exist
} dir_check_result_t;

Status fs_utils_mkdir(std::string path, std::string &error_msg);
dir_check_result_t fs_utils_check_dir(std::string path, std::string &error_msg);
