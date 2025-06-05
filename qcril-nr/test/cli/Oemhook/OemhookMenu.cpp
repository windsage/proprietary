/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * OEMHOOK Menu
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <limits>
#include <utility>

#include "OemhookMenu.hpp"
#include "OemhookUtils.hpp"
#include "csg/CsgMenu.hpp"
#include "nv/NvMenu.hpp"
#include "Qtuner/QtunerMenu.hpp"
#include "interfaces/nas/nas_types.h"

using namespace std;

uint32_t cachSrcTyp = -1;

OemhookMenu::OemhookMenu(std::string appName, std::string cursor, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
}

OemhookMenu::~OemhookMenu() {
}

void OemhookMenu::init() {
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListOemhookSubMenu = {
    CREATE_COMMAND(OemhookMenu::csgMenu, "CSG"),
    CREATE_COMMAND(OemhookMenu::nvMenu, "NV"),
    CREATE_COMMAND(OemhookMenu::qtunerMenu, "QTUNER"),
    CREATE_COMMAND(OemhookMenu::registerForOemhookIndications, "Register for Oemhook Indications"),
    CREATE_COMMAND(OemhookMenu::getMaxDataAllowed, "getMaxDataAllowed"),
    CREATE_COMMAND(OemhookMenu::setUiStatus, "setUiStatus", "isUiReady"),
    CREATE_COMMAND(OemhookMenu::getPreferredNetworkBandPref, "getPreferredNetworkBandPref"),
    CREATE_COMMAND(OemhookMenu::setPreferredNetworkBandPref, "setPreferredNetworkBandPref"),
    CREATE_COMMAND(OemhookMenu::getLPlusLFeatureSupportStatus, "getLPlusLFeatureSupportStatus"),
    CREATE_COMMAND(OemhookMenu::enableEngineerMode, "enableEngineerMode"),
    CREATE_COMMAND(OemhookMenu::sarGetSarRevKey, "sarGetSarRevKey"),
    CREATE_COMMAND(OemhookMenu::sarSetTransmitPower, "sarSetTransmitPower"),
    CREATE_COMMAND(OemhookMenu::startNetworkScan, "startNetworkScan"),
    CREATE_COMMAND(OemhookMenu::stopNetworkScan, "stopNetworkScan"),
    CREATE_COMMAND(OemhookMenu::setProvStatus, "setProvStatus"),
    CREATE_COMMAND(OemhookMenu::getProvStatus, "getProvStatus"),
    CREATE_COMMAND(OemhookMenu::getIccid, "getIccid"),
  };

  addCommands(commandsListOemhookSubMenu);
  ConsoleApp::displayMenu();
}

void OemhookMenu::csgMenu(std::vector<std::string> userInput) {
  CsgMenu csgMenu("CSG Menu", "OEMHOOK CSG> ", rilSession);
  csgMenu.init();
  csgMenu.mainLoop();  // Main loop to continuously read and execute commands
}

void OemhookMenu::nvMenu(std::vector<std::string> userInput) {
  NvMenu nvMenu("NV Menu", "OEMHOOK NV> ", rilSession);
  nvMenu.init();
  nvMenu.mainLoop();  // Main loop to continuously read and execute commands
}

void OemhookMenu::qtunerMenu(std::vector<std::string> userInput) {
  QtunerMenu qtunerMenu("QTUNER Menu", "QTUNER> ", rilSession);
  qtunerMenu.init();
  qtunerMenu.mainLoop();  // Main loop to continuously read and execute commands
}

void OemhookMenu::getMaxDataAllowed(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  // QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ (0x80000 + 93).
  // Request data is null.
  // Response data is uint8_t.
  uint32_t messageId = QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ;
  auto [data, dataLen] = packOemhookRequest(messageId, nullptr, 0);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout << "Got response for oemhookRaw(QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ): " << err
                  << std::endl;
        uint8_t maxDataAllowed = 0;
        if (data && (dataLen == sizeof(maxDataAllowed))) {
          maxDataAllowed = *((uint8_t*)(data));
        }
        std::cout << "QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ: Response:" << std::endl
                  << "  err: " << static_cast<uint32_t>(err) << std::endl
                  << "  maxDataAllowed: " << static_cast<uint32_t>(maxDataAllowed) << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::getLPlusLFeatureSupportStatus(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  // QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ (0x80000 + 94).
  // Request data is null.
  // Response data is uint8_t.
  uint32_t messageId = QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ;
  auto [data, dataLen] = packOemhookRequest(messageId, nullptr, 0);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ)"
            << err << std::endl;
        uint8_t lPlusLStatus = 0;
        if (data && (dataLen == sizeof(lPlusLStatus))) {
          lPlusLStatus = *((uint8_t*)(data));
        }
        std::cout << "QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ: Response:\n"
                  << "  err: " << static_cast<uint32_t>(err) << std::endl
                  << "  lPlusLStatus: " << static_cast<uint32_t>(lPlusLStatus) << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::setUiStatus(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  if (userInput.size() < 2) {
    std::cout << "Incorrect number of parameters. Exiting.." << std::endl;
    return;
  }
  uint8_t isUiReady = std::stoi(userInput[1]);
  // QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS (0x80000 + 26).
  // Request data is uint8_t.
  // Response data is null.
  uint32_t messageId = QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS;
  auto [data, dataLen] = packOemhookRequest(messageId, (const uint8_t *)&isUiReady, sizeof(isUiReady));
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS)"
            << err << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::getPreferredNetworkBandPref(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int32_t _userInput = -1;
  do {
    std::cin.clear();
    std::cout << "Enter rat band type (1 for NONE, 2 for LTE_BAND) (-1 for exit) : ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1);

  if (_userInput == -1) {
    return;
  }
  uint32_t ratBandType = _userInput;
  // QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF (0x80000 + 38).
  // Request data is uint32_t.
  // Response data is uint32_t.
  uint32_t messageId = QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF;
  auto [data, dataLen] = packOemhookRequest(messageId, (const uint8_t *)&ratBandType, sizeof(ratBandType));
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF)"
            << err << std::endl;
        uint32_t ratBandMap = 0;
        if (data && (dataLen == sizeof(ratBandMap))) {
          ratBandMap = *((uint32_t*)(data));
        }
        std::cout << "QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF: Response:\n"
                  << "  err: " << static_cast<uint32_t>(err) << std::endl
                  << "  ratBandMap: " << static_cast<uint32_t>(ratBandMap) << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::setPreferredNetworkBandPref(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int32_t _userInput = -1;
  do {
    std::cin.clear();
    std::cout << "Enter band pref (0 for NONE, 1 for LTE_FULL, 2 for TDD_LTE, 3 for FDD_LTE) (-1 for exit) : ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1);

  if (_userInput == -1) {
    return;
  }
  uint32_t bandPrefMap = _userInput;
  // QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF (0x80000 + 38).
  // Request data is uint32_t.
  // Response data is null.
  uint32_t messageId = QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF;
  auto [data, dataLen] = packOemhookRequest(messageId, (const uint8_t *)&bandPrefMap, sizeof(bandPrefMap));
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF)"
            << err << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

// QCRIL_REQ_HOOK_SET_TRANSMIT_POWER request type
struct sar_rf_state {
  uint32_t rf_state_index;
  uint32_t compatibility_key;
  uint32_t has_compatibility_key;
};

void OemhookMenu::enableEngineerMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int32_t _userInput = -1;
  ftm_subs_status subsStatus;
  subsStatus.is_enable = 0;
  subsStatus.subs_type = (oem_hook_ftm_subscription_source_e_type)0;
  do {
    std::cin.clear();
    std::cout << "Enter isEnable (0 for disable, 1 for enable (-1 for exit) : ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1);
  if (_userInput == -1) {
    return;
  }
  subsStatus.is_enable = _userInput;
  do {
    std::cin.clear();
    std::cout << "Enter subscription src type (0 for CDMA, 1 for GSM, 2 for WCDMA (-1 for exit) : ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1);
  if (_userInput == -1) {
    return;
  }

  subsStatus.subs_type = (oem_hook_ftm_subscription_source_e_type)_userInput;
  cachSrcTyp = subsStatus.subs_type;

  // QCRIL_REQ_HOOK_ENABLE_ENGINEER_MODE (0x80000 + 19).
  // Request data is ftm_subs_status.
  // Response data is null.
  uint32_t messageId = QCRIL_REQ_HOOK_ENABLE_ENGINEER_MODE;
  auto [data, dataLen] = packOemhookRequest(messageId, (const uint8_t *)&subsStatus, sizeof(subsStatus));
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_ENABLE_ENGINEER_MODE)"
            << err << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::sarGetSarRevKey(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  // QCRIL_REQ_HOOK_GET_SAR_REV_KEY (0x80000 + 200).
  // Request data is null.
  // Response data is uint8_t.
  uint32_t messageId = QCRIL_REQ_HOOK_GET_SAR_REV_KEY;
  auto [data, dataLen] = packOemhookRequest(messageId, nullptr, 0);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_GET_SAR_REV_KEY)"
            << err << std::endl;
        uint32_t sarKey = 0;
        if (data && (dataLen == sizeof(sarKey))) {
          sarKey = *((uint8_t*)(data));
        }
        std::cout << "QCRIL_REQ_HOOK_GET_SAR_REV_KEY: Response:\n"
                  << "  err: " << static_cast<uint32_t>(err) << std::endl
                  << "  SAR Key: " << static_cast<uint32_t>(sarKey) << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::sarSetTransmitPower(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int32_t _userInput = -1;
  sar_rf_state sarRfState;
  sarRfState.rf_state_index = 0;
  sarRfState.has_compatibility_key = 0;
  do {
    std::cin.clear();
    std::cout << "Enter rf_state_index (0 to 20 (-1 for exit) : ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1);
  if (_userInput == -1) {
    return;
  }
  sarRfState.rf_state_index = _userInput;
  do {
    std::cin.clear();
    std::cout << "Enter has_compatibility_key (0 for not valid, 1 for valid (-1 for exit) : ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1);
  if (_userInput == -1) {
    return;
  }
  sarRfState.has_compatibility_key = _userInput;
  do {
    std::cin.clear();
    std::cout << "Enter compatibility_key (-1 for exit) : ";
    std::cin >> _userInput;
  } while (std::cin.fail() || _userInput < -1);
  if (_userInput == -1) {
    return;
  }
  sarRfState.compatibility_key = _userInput;

  // QCRIL_REQ_HOOK_SET_TRANSMIT_POWER (0x80000 + 201).
  // Request data is ftm_subs_status.
  // Response data is null.
  uint32_t messageId = QCRIL_REQ_HOOK_SET_TRANSMIT_POWER;
  auto [data, dataLen] = packOemhookRequest(messageId, (const uint8_t *)&sarRfState, sizeof(sarRfState));
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_SET_TRANSMIT_POWER)"
            << err << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::startNetworkScan(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t messageId = QCRIL_REQ_HOOK_PERFORM_INCREMENTAL_NW_SCAN;
  auto [data, dataLen] = packOemhookRequest(messageId, nullptr, 0);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_PERFORM_INCREMENTAL_NW_SCAN)"
            << err << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::stopNetworkScan(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t messageId = QCRIL_REQ_HOOK_CANCEL_QUERY_AVAILABLE_NETWORK;
  auto [data, dataLen] = packOemhookRequest(messageId, nullptr, 0);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_CANCEL_QUERY_AVAILABLE_NETWORK)"
            << err << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::getProvStatus(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t messageId = QCRIL_REQ_HOOK_GET_SUB_PROVISION_PREFERENCE_REQ;
  auto [data, dataLen] = packOemhookRequest(messageId, nullptr, 0);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        int32_t user_pref = -1;
        int32_t sub_pref = -1;
        if (data && (dataLen == 2*sizeof(int32_t))) {
          user_pref = data[0];
          sub_pref = data[1];
        }

        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_GET_SUB_PROVISION_PREFERENCE_REQ)"
            << "error :"<< err << " user_preference is: " << user_pref
            << " sub_preference is: " << sub_pref << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::setProvStatus(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  int32_t sub_pref = -1;
  int32_t slot_id = -1;
  std::cin.clear();
  std::cout << "User SIM provisioning preference 0 - Deactivate, 1 - Activate (-1 for exit) : ";
  std::cin >> sub_pref;
  std::cout << "Enter slot_id  (-1 for exit) : ";
  std::cin >> slot_id;

  int32_t state[2];
  state[0] = sub_pref;
  state[1] = slot_id;

  uint32_t messageId = QCRIL_REQ_HOOK_SET_SUB_PROVISION_PREFERENCE_REQ;
  auto [data, dataLen] = packOemhookRequest(messageId, (const uint8_t *)state, sizeof(state));
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout
            << "Got response for oemhookRaw(QCRIL_REQ_HOOK_SET_SUB_PROVISION_PREFERENCE_REQ): "
            << err <<  std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::getIccid(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;


  uint32_t messageId = QCRIL_REQ_HOOK_GET_SIM_ICCID_REQ;
  auto [data, dataLen] = packOemhookRequest(messageId, nullptr, 0);
  Status s = rilSession.oemhookRaw(
    data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
      std::cout << "Got response for oemhookRaw(QCRIL_REQ_HOOK_GET_SIM_ICCID_REQ)"
          << err << std::endl;
      if (data) {
        std::cout << "  data" << data << std::endl;
      }
    });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void OemhookMenu::processUnsolNetworkScan(const char* data, size_t dataLen) {
  std::cout << "QCRIL_REQ_HOOK_UNSOL_INCREMENTAL_NW_SCAN_IND: "<< std::endl;
  if (data && dataLen) {
    std::cout << "total len: " <<dataLen <<std::endl;

    unsigned char in = 0;
    int st = static_cast<uint8_t>(data[in]);
    std::cout << "scan_status" << st <<std::endl;
    in = in+1;
    int nw_cnt = static_cast<uint8_t>(data[in]);
    std::cout << "nw_count " << nw_cnt <<std::endl;
    in = in+1;
    for (int iter_i = 0; iter_i < nw_cnt; iter_i++ ) {
      uint16_t len = 0;

      memcpy(&len,data+in,sizeof(uint16_t));
      in = in+2;

      char *name = new char[len+1];
      memset(name,0x00,sizeof(name));
      memcpy(name, data+in, len);
      name[len] = '\0';
      std::cout << "long name  is "<< name <<std::endl;
      in = in+len;
      delete[] name;

      memcpy(&len,data+in,sizeof(uint16_t));
      in = in+2;

      name = new char[len+1];
      memset(name,0x00,sizeof(name));
      memcpy(name, data+in, len);
      name[len] = '\0';
      std::cout << "short name  is "<< name <<std::endl;
      in = in+len;
      delete[] name;

      memcpy(&len,data+in,sizeof(uint16_t));
      in = in+2;

      name = new char[len+1];
      memset(name,0x00,sizeof(name));
      memcpy(name, data+in, len);
      name[len] = '\0';
      std::cout << "plmn  is "<< name <<std::endl;
      in = in+len;
      delete[] name;

      memcpy(&len,data+in,sizeof(uint16_t));
      in = in+2;

      name = new char[len+1];
      memset(name,0x00,sizeof(name));
      memcpy(name, data+in, len);
      name[len] = '\0';
      std::cout << "status  is "<< name <<std::endl;
      in = in+len;
      delete[] name;

    }
  }
}

void OemhookMenu::processUnsolProvStateChanged(const char* data, size_t dataLen) {
  int32_t user_pref = -1;
  int32_t sub_pref = -1;
  if (data && (dataLen == 2*sizeof(int32_t))) {
    user_pref = data[0];
    sub_pref = data[1];
 }
 std::cout << "QCRIL_REQ_HOOK_UNSOL_SUB_PROVISION_STATUS: "
   << "user_pref: " << user_pref << " sub_pref:  "<< sub_pref << std::endl;
}

void OemhookMenu::processUnsolMaxActiveDataSubsChanged(const char* data, size_t dataLen) {
  uint8_t maxDataAllowed = 0;
  if (data && (dataLen == sizeof(maxDataAllowed))) {
    maxDataAllowed = *((uint8_t*)(data));
  }
  std::cout << "QCRIL_REQ_HOOK_UNSOL_MAX_DATA_CHANGE_IND: "
            << "maxDataAllowed: " << static_cast<uint32_t>(maxDataAllowed) << std::endl;
}

void OemhookMenu::processUnsolAudioStateChanged(const char* data, size_t dataLen) {
  std::string audioParams;
  if (data && dataLen) {
    audioParams = std::string(data);
  }
  std::cout << "QCRIL_REQ_HOOK_UNSOL_AUDIO_STATE_CHANGED: "
            << "audioParams: " << audioParams << std::endl;
}

void OemhookMenu::processUnsolEngineerMode(const char* data, size_t dataLen) {
  std::ostringstream logStream{};
  logStream << "QCRIL_REQ_HOOK_UNSOL_ENGINEER_MODE:" << std::endl;

  if(cachSrcTyp == FTM_SUBSCRIPTION_CDMA)
  {
    logStream << "cdma_ftm_data :" << std::endl;
    cdma_ftm_data* cdma_data = (cdma_ftm_data*)data;
    logStream << "  srv_status : " << cdma_data->srv_status << std::endl;
    logStream << "  srv_domain : " << cdma_data->srv_domain << std::endl;
    logStream << "  system_mode : " << cdma_data->system_mode << std::endl;
    logStream << "  roam_status : " << cdma_data->roam_status << std::endl;
    logStream << "  mcc : " << cdma_data->mcc << std::endl;
    logStream << "  mnc : " << cdma_data->mnc << std::endl;
    logStream << "  rssi : " << static_cast<uint32_t>(cdma_data->rssi) << std::endl;
    logStream << "  ecio : " << static_cast<uint32_t>(cdma_data->ecio) << std::endl;
    logStream << "  cdma_1x_rx0_agc : " << cdma_data->cdma_1x_rx0_agc << std::endl;
    logStream << "  cdma_1x_rx1_agc : " << cdma_data->cdma_1x_rx1_agc << std::endl;
    logStream << "  cdma_evdo_rx0_agc : " << cdma_data->cdma_evdo_rx0_agc << std::endl;
    logStream << "  cdma_evdo_rx1_agc : " << cdma_data->cdma_evdo_rx1_agc << std::endl;
  }
  else if(cachSrcTyp == FTM_SUBSCRIPTION_GSM)
  {
    gsm_ftm_data* gsm_data = (gsm_ftm_data*)data;
    logStream << "gsm_ftm_data :" << std::endl;
    logStream << "  srv_status : " << gsm_data->srv_status << std::endl;
    logStream << "  srv_domain : " << gsm_data->srv_domain << std::endl;
    logStream << "  system_mode : " << gsm_data->system_mode << std::endl;
    logStream << "  roam_status : " << gsm_data->roam_status << std::endl;
    logStream << "  mcc : " << gsm_data->mcc << std::endl;
    logStream << "  mnc : " << gsm_data->mnc << std::endl;
    logStream << "  lac : " << static_cast<uint32_t>(gsm_data->lac) << std::endl;
    logStream << "  rssi : " << static_cast<uint32_t>(gsm_data->rssi) << std::endl;
    logStream << "  bcch : " << static_cast<uint32_t>(gsm_data->bcch) << std::endl;
    logStream << "  bsic : " << static_cast<uint32_t>(gsm_data->bsic) << std::endl;
    logStream << "  rx_level : " << static_cast<uint32_t>(gsm_data->rx_level) << std::endl;
    logStream << "  rx_qual_full : " << static_cast<uint32_t>(gsm_data->rx_qual_full) << std::endl;
    logStream << "  rx_qual_sub : " << static_cast<uint32_t>(gsm_data->rx_qual_sub) << std::endl;
    logStream << "  ta : " << static_cast<uint32_t>(gsm_data->ta) << std::endl;
    logStream << "  no_of_neigh_cell_info_len : " << gsm_data->no_of_neigh_cell_info_len << std::endl;
    for(int i=0; i<gsm_data->no_of_neigh_cell_info_len; i++)
    {
      logStream << "  gsm_neigh_cell_info[" << i << "]" << std::endl;
      logStream << "    cell_id : " << gsm_data->gsm_neigh_cell[i].cell_id << std::endl;
      logStream << "    bcch : " << static_cast<uint32_t>(gsm_data->gsm_neigh_cell[i].bcch) << std::endl;
      logStream << "    bsic : " << static_cast<uint32_t>(gsm_data->gsm_neigh_cell[i].bsic) << std::endl;
      logStream << "    rx_level : " << static_cast<uint32_t>(gsm_data->gsm_neigh_cell[i].rx_level) << std::endl;
    }
  }
  else if(cachSrcTyp == FTM_SUBSCRIPTION_WCDMA)
  {
    wcdma_ftm_data* wcdma_data = (wcdma_ftm_data*)data;
    logStream << "wcdma_ftm_data :" << std::endl;
    logStream << "  srv_status : " << wcdma_data->srv_status << std::endl;
    logStream << "  srv_domain : " << wcdma_data->srv_domain << std::endl;
    logStream << "  system_mode : " << wcdma_data->system_mode << std::endl;
    logStream << "  roam_status : " << wcdma_data->roam_status << std::endl;
    logStream << "  mcc : " << wcdma_data->mcc << std::endl;
    logStream << "  mnc : " << wcdma_data->mnc << std::endl;
    logStream << "  lac : " << static_cast<uint32_t>(wcdma_data->lac) << std::endl;
    logStream << "  bler : " << static_cast<uint32_t>(wcdma_data->bler) << std::endl;
    logStream << "  ecio : " << static_cast<uint32_t>(wcdma_data->ecio) << std::endl;
    logStream << "  rscp : " << static_cast<uint32_t>(wcdma_data->rscp) << std::endl;
    logStream << "  rx_agc : " << wcdma_data->rx_agc << std::endl;
    logStream << "  tx_agc : " << wcdma_data->tx_agc << std::endl;
    logStream << "  uarfcn : " << static_cast<uint32_t>(wcdma_data->uarfcn) << std::endl;
    logStream << "  psc : " << static_cast<uint32_t>(wcdma_data->psc) << std::endl;
    logStream << "  no_of_neigh_cell_info_len : " << wcdma_data->no_of_neigh_cell_info_len << std::endl;
    for(int i=0; i<wcdma_data->no_of_neigh_cell_info_len; i++)
    {
      logStream << "  wcdma_neigh_cell_info[" << i << "]" << std::endl;
      logStream << "    cell_id : " << wcdma_data->wcdma_neigh_cell[i].cell_id << std::endl;
      logStream << "    uarfcn : " << static_cast<uint32_t>(wcdma_data->wcdma_neigh_cell[i].uarfcn) << std::endl;
      logStream << "    psc : " << static_cast<uint32_t>(wcdma_data->wcdma_neigh_cell[i].psc) << std::endl;
      logStream << "    rscp : " << static_cast<uint32_t>(wcdma_data->wcdma_neigh_cell[i].rscp) << std::endl;
      logStream << "    ecio : " << static_cast<uint32_t>(wcdma_data->wcdma_neigh_cell[i].ecio) << std::endl;
    }
  }
  std::cout << logStream.str();
}

void OemhookMenu::processUnsolSpeechCodecInfo(const char* data, size_t dataLen) {
  int payload[3] = { 0 };
  if (data && (dataLen == sizeof(payload))) {
    memcpy(payload, data, sizeof(payload));
  }
  std::cout << "QCRIL_REQ_HOOK_UNSOL_SPEECH_CODEC_INFO: "
            << " payload[0]: " << static_cast<uint32_t>(payload[0])
            << " payload[1]: " << static_cast<uint32_t>(payload[1])
            << " payload[2]: " << static_cast<uint32_t>(payload[1]) << std::endl;
}

void OemhookMenu::registerForOemhookIndications(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s =
      rilSession.registerOemhookIndicationHandler([this](const char* data, size_t dataLen) -> void {
        std::cout << "Got oemhook indication: dataLen = " << dataLen << std::endl;
        auto [messageId, indData, indDataLen] = unpackOemhookIndication(data, dataLen);
        std::cout << "messageId: " << messageId << std::endl;
        switch (messageId) {
          case QCRIL_REQ_HOOK_UNSOL_ENGINEER_MODE:
            processUnsolEngineerMode(indData, indDataLen);
            break;
          case QCRIL_REQ_HOOK_UNSOL_MAX_DATA_CHANGE_IND:
            processUnsolMaxActiveDataSubsChanged(indData, indDataLen);
            break;
          case QCRIL_REQ_HOOK_UNSOL_AUDIO_STATE_CHANGED:
            processUnsolAudioStateChanged(indData, indDataLen);
            break;
          case QCRIL_REQ_HOOK_UNSOL_INCREMENTAL_NW_SCAN_IND:
            processUnsolNetworkScan(indData, indDataLen);
            break;
          case QCRIL_REQ_HOOK_UNSOL_SUB_PROVISION_STATUS:
            processUnsolProvStateChanged(indData, indDataLen);
            break;
          case QCRIL_REQ_HOOK_UNSOL_SPEECH_CODEC_INFO:
            processUnsolSpeechCodecInfo(indData, indDataLen);
            break;
          default:
            std::cout << "unsupported messageId: " << messageId << std::endl;
            break;
        }
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
}
