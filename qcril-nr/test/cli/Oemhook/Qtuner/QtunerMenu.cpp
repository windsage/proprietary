/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * OEMHOOK QTUNER Menu
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <limits>
#include <utility>

#include "../OemhookUtils.hpp"
#include "QtunerMenu.hpp"

#include "services/qtuner_v01.h"
#include "qmi_client.h"

using namespace std;

QtunerMenu::QtunerMenu(std::string appName, std::string cursor, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
}

QtunerMenu::~QtunerMenu() {
}

void QtunerMenu::init() {
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandListQtunerSubMenu = {
    // CREATE_COMMAND(QtunerMenu::registerForOemhookIndications, "Register for Oemhook Indications"),
    CREATE_COMMAND(QtunerMenu::setRfmScenarioReq, "setRfmScenarioReq"),
    CREATE_COMMAND(QtunerMenu::getRfmScenarioReq, "getRfmScenarioReq"),
    CREATE_COMMAND(QtunerMenu::getProvisionedTableRevisionReq, "getProvisionedTableRevisionReq"),
  };

  addCommands(commandListQtunerSubMenu);
  ConsoleApp::displayMenu();
}

void QtunerMenu::setRfmScenarioReq(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Qtuner_set_scenario_req_v01 set_req{};
  int32_t _userInput = -1;

  do {
    std::cin.clear();
    std::cout << "Enter scenarios_len (max 32, -1 to Exit): ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1 || _userInput > 32);
  if (_userInput == -1) {
    return;
  }
  set_req.scenarios_len = _userInput;
  for (uint32_t i = 0; i < set_req.scenarios_len; i++) {
    std::cout << "Enter scenarios[" << i << "]: ";
    std::cin >> set_req.scenarios[i];
  }
  auto [payload, payloadLen] =
      packGenericRequestPayload((uint16_t)TUNNELING_SERVICE_QTUNER,
                                QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01, &set_req, sizeof(set_req));

  // QCRIL_REQ_HOOK_REQ_GENERIC (0x80000 + 100).
  uint32_t messageId = QCRIL_REQ_HOOK_REQ_GENERIC;
  auto [data, dataLen] = packOemhookRequest(messageId, payload, payloadLen);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout << "Got response for oemhookRaw(QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01): " << err
                  << std::endl;
        auto [errNo, payload, payloadLen] = unpackGenericResponsePayload(data, dataLen);
        std::cout << "QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01: Respnse:\n"
                  << "  errNo: " << static_cast<uint32_t>(errNo) << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void QtunerMenu::getRfmScenarioReq(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  auto [payload, payloadLen] = packGenericRequestPayload(
      (uint16_t)TUNNELING_SERVICE_QTUNER, QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01, nullptr, 0);

  // QCRIL_REQ_HOOK_REQ_GENERIC (0x80000 + 100).
  uint32_t messageId = QCRIL_REQ_HOOK_REQ_GENERIC;
  auto [data, dataLen] = packOemhookRequest(messageId, payload, payloadLen);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout << "Got response for oemhookRaw(QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01): " << err
                  << std::endl;
        auto [errNo, payload, payloadLen] = unpackGenericResponsePayload(data, dataLen);
        std::cout << "QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01: Respnse:\n"
                  << "  errNo: " << static_cast<uint32_t>(errNo) << std::endl;
        Qtuner_get_rfm_scenarios_resp_v01* resp = (Qtuner_get_rfm_scenarios_resp_v01*)payload;
        if (resp) {
          std::cout << "  active_scenarios_valid: " << (uint32_t)resp->active_scenarios_valid
                    << std::endl
                    << "  active_scenarios_len: " << resp->active_scenarios_len << std::endl
                    << "  active_scenarios: ";
          for (size_t i = 0; i < resp->active_scenarios_len; i++) {
            std::cout << (uint32_t)resp->active_scenarios[i];
          }
          std::cout << std::endl;
        }
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void QtunerMenu::getProvisionedTableRevisionReq(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  auto [payload, payloadLen] =
      packGenericRequestPayload((uint16_t)TUNNELING_SERVICE_QTUNER,
                                QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01, nullptr, 0);

  // QCRIL_REQ_HOOK_REQ_GENERIC (0x80000 + 100).
  uint32_t messageId = QCRIL_REQ_HOOK_REQ_GENERIC;
  auto [data, dataLen] = packOemhookRequest(messageId, payload, payloadLen);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01): "
            << err << std::endl;
        auto [errNo, payload, payloadLen] = unpackGenericResponsePayload(data, dataLen);
        std::cout << "QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01: Respnse:\n"
                  << "  errNo: " << static_cast<uint32_t>(errNo) << std::endl;
        Qtuner_get_provisioned_table_revision_resp_v01* resp =
            (Qtuner_get_provisioned_table_revision_resp_v01*)payload;
        if (resp) {
          std::cout << "  provisioned_table_revision_valid: "
                    << (uint32_t)resp->provisioned_table_revision_valid << std::endl
                    << "  provisioned_table_revision: " << resp->provisioned_table_revision
                    << std::endl;
          std::cout << "  provisioned_table_OEM_valid: "
                    << (uint32_t)resp->provisioned_table_OEM_valid << std::endl
                    << "  provisioned_table_OEM_len: " << resp->provisioned_table_OEM_len
                    << std::endl
                    << "  provisioned_table_OEM: ";
          for (size_t i = 0; i < resp->provisioned_table_OEM_len; i++) {
            std::cout << (uint32_t)resp->provisioned_table_OEM[i];
          }
          std::cout << std::endl;
        }
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}
