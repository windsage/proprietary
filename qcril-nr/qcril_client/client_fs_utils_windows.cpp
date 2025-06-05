/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <client_fs_utils.hpp>
#include <filesystem>
#include <system_error>

Status fs_utils_mkdir(std::string path, std::string &error_msg) {
  std::filesystem::path fsPath(path);
  std::error_code ec;
  bool ret = std::filesystem::create_directories(fsPath, ec);
  if (ret) {
    return Status::SUCCESS;
  }
  else {
    error_msg = ec.message();
    return Status::FAILURE;
  }
}


dir_check_result_t fs_utils_check_dir(std::string path, std::string &error_msg) {
  std::filesystem::path fsPath(path);
  std::error_code exist_ec, is_dir_ec;
  dir_check_result_t ret;
  if (std::filesystem::exists(fsPath, exist_ec)) {
    if (std::filesystem::is_directory(fsPath, is_dir_ec)) {
      ret = IS_DIR;
    }
    else {
      ret = IS_FILE;
    }
  }
  else {
    ret = DNE;
  }
  if (exist_ec.value() != 0) {
    error_msg += exist_ec.message() + " ";
  }
  if (is_dir_ec.value() != 0) {
    error_msg += is_dir_ec.message();
  }
  return ret;
}
