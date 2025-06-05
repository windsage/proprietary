/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * OEMHOOK menu
 */

#ifndef OEMHOOKMENU_HPP
#define OEMHOOKMENU_HPP

#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

class OemhookMenu : public ConsoleApp {
 public:
  OemhookMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
  ~OemhookMenu();
  void init();

 private:
  RilApiSession& rilSession;

  void csgMenu(std::vector<std::string> userInput);
  void nvMenu(std::vector<std::string> userInput);
  void qtunerMenu(std::vector<std::string> userInput);
  void registerForOemhookIndications(std::vector<std::string> userInput);
  void getLPlusLFeatureSupportStatus(std::vector<std::string> userInput);
  void enableEngineerMode(std::vector<std::string> userInput);
  void sarGetSarRevKey(std::vector<std::string> userInput);
  void sarSetTransmitPower(std::vector<std::string> userInput);
  void getMaxDataAllowed(std::vector<std::string> userInput);
  void setUiStatus(std::vector<std::string> userInput);
  void getPreferredNetworkBandPref(std::vector<std::string> userInput);
  void setPreferredNetworkBandPref(std::vector<std::string> userInput);
  void startNetworkScan(std::vector<std::string> userInput);
  void stopNetworkScan(std::vector<std::string> userInput);
  void setProvStatus(std::vector<std::string> userInput);
  void getProvStatus(std::vector<std::string> userInput);
  void getIccid(std::vector<std::string> userInput);

  void processUnsolMaxActiveDataSubsChanged(const char* data, size_t dataLen);
  void processUnsolAudioStateChanged(const char* data, size_t dataLen);
  void processUnsolEngineerMode(const char* data, size_t dataLen);
  void processUnsolSpeechCodecInfo(const char* data, size_t dataLen);
  void processUnsolNetworkScan(const char* data, size_t dataLen);
  void processUnsolProvStateChanged(const char* data, size_t dataLen);

};

#endif  // OEMHOOKMENU_HPP
