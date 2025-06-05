/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * OEMHOOK NV menu
 */

#ifndef NVMENU_HPP
#define NVMENU_HPP

#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

class NvMenu : public ConsoleApp {
 public:
  NvMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
  ~NvMenu();
  void init();

 private:
  RilApiSession& rilSession;

  void nvRead(std::vector<std::string> userInput);
  void nvWrite(std::vector<std::string> userInput);
};

#endif  // NvMenu_HPP
