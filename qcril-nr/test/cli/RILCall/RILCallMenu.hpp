/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * Voice class provides Voice functionalitites
 */

#ifndef __RIL_CALL_MENU_HPP_
#define __RIL_CALL_MENU_HPP_

#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

class RILCallMenu : public ConsoleApp {
 public:
  RILCallMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
  ~RILCallMenu();
  void init();

 private:
  RilApiSession& rilSession;

 private:
  void registerForIndications(std::vector<std::string> userInput);
  void getCurrentCalls(std::vector<std::string> userInput);
  void hangup(std::vector<std::string> userInput);
  void udub(std::vector<std::string> userInput);
  void hangupWaitingOrBackground(std::vector<std::string> userInput);
  void hangupForegroundResumeBackground(std::vector<std::string> userInput);
  void switchWaitingOrHoldingAndActive(std::vector<std::string> userInput);
  void deflectCall(std::vector<std::string> userInput);
  void lastCallFailCause(std::vector<std::string> userInput);
  void setCallForward(std::vector<std::string> userInput);
  void queryCallForwardStatus(std::vector<std::string> userInput);
  void setCallWaiting(std::vector<std::string> userInput);
  void queryCallWaiting(std::vector<std::string> userInput);
  void changeBarringPassword(std::vector<std::string> userInput);
  void sendUssd(std::vector<std::string> userInput);
  void cancelUssd(std::vector<std::string> userInput);
  void exitEmergencyCallbackMode(std::vector<std::string> userInput);
  void queryClip(std::vector<std::string> userInput);
  void getClir(std::vector<std::string> userInput);
  void setClir(std::vector<std::string> userInput);
  void setSuppSvcNotification(std::vector<std::string> userInput);
  void suppSvcStatus(std::vector<std::string> userInput);
  void separateConnection(std::vector<std::string> userInput);
  void setTtyMode(std::vector<std::string> userInput);
  void getTtyMode(std::vector<std::string> userInput);
  void hold(std::vector<std::string> userInput);
  void resume(std::vector<std::string> userInput);
  void getColr(std::vector<std::string> userInput);
  void setColr(std::vector<std::string> userInput);
  void addParticipant(std::vector<std::string> userInput);
  void modifyCallInitiate(std::vector<std::string> userInput);
  void modifyCallConfirm(std::vector<std::string> userInput);
  void cancelModifyCall(std::vector<std::string> userInput);
  void sendUiTtyMode(std::vector<std::string> userInput);
  void sendRttMessage(std::vector<std::string> userInput);
  void playDtmfTone(std::vector<std::string> userInput);
  void startDtmfTone(std::vector<std::string> userInput);
  void stopDtmfTone(std::vector<std::string> userInput);
  void explicitCallTransfer(std::vector<std::string> userInput);
  void conference(std::vector<std::string> userInput);
  void dial(std::vector<std::string> userInput);
  void acceptCall(std::vector<std::string> userInput);
};

#endif  // __RIL_CALL_MENU_HPP_
