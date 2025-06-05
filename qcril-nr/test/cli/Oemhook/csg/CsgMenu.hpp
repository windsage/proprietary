/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * OEMHOOK CSG menu
 */

#ifndef CSGMENU_HPP
#define CSGMENU_HPP

#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

class CsgMenu : public ConsoleApp {
 public:
  CsgMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
  ~CsgMenu();
  void init();

 private:
  RilApiSession& rilSession;

  void getCsgId(std::vector<std::string> userInput);
  void csgPerformNetworkScan(std::vector<std::string> userInput);
  void csgSetNetworkSelectionPreference(std::vector<std::string> userInput);
  void registerForIndications(std::vector<std::string> userInput);

  void processUnsolCsgIdChangedInd(const char* data, size_t dataLen);
};

#endif  // CSGMENU_HPP
