/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * QTUNER menu
 */

#ifndef QTUNERMENU_HPP
#define QTUNERMENU_HPP

#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

class QtunerMenu : public ConsoleApp {
 public:
  QtunerMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
  ~QtunerMenu();
  void init();

 private:
  RilApiSession& rilSession;

  void setRfmScenarioReq(std::vector<std::string> userInput);
  void getRfmScenarioReq(std::vector<std::string> userInput);
  void getProvisionedTableRevisionReq(std::vector<std::string> userInput);
};

#endif  // QTUNERMENU_HPP
