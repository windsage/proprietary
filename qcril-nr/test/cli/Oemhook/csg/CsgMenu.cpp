/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * OEMHOOK CSG Menu
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <limits>
#include <utility>

#include "../OemhookUtils.hpp"
#include "CsgMenu.hpp"
#include "interfaces/nv/nv_items.h"

using namespace std;

CsgMenu::CsgMenu(std::string appName, std::string cursor, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
}

CsgMenu::~CsgMenu() {
}

void CsgMenu::init() {
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandListQtunerSubMenu = {
    CREATE_COMMAND(CsgMenu::getCsgId, "getCsgId"),
    CREATE_COMMAND(CsgMenu::csgPerformNetworkScan, "csgPerformNetworkScan"),
    CREATE_COMMAND(CsgMenu::csgSetNetworkSelectionPreference, "csgSetNetworkSelectionPreference"),
    CREATE_COMMAND(CsgMenu::registerForIndications, "registerForIndications"),
  };

  addCommands(commandListQtunerSubMenu);
  ConsoleApp::displayMenu();
}

void CsgMenu::getCsgId(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  // QCRIL_REQ_HOOK_GET_CSG_ID (0x80000 + 24).
  uint32_t messageId = QCRIL_REQ_HOOK_GET_CSG_ID;
  auto [data, dataLen] = packOemhookRequest(messageId, nullptr, 0);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout << "Got response for oemhookRaw(QCRIL_REQ_HOOK_GET_CSG_ID): " << err
                  << std::endl;
        printBuffer(data, dataLen);
        uint32_t csgId = 0;
        if (err == RIL_E_SUCCESS) {
          if (data && dataLen) {
            if (dataLen >= sizeof(csgId)) {
              memcpy(&csgId, data, dataLen);
            }
          }
        }
        std::cout << "QCRIL_REQ_HOOK_GET_CSG_ID: Response:\n"
                  << "  err: " << static_cast<uint32_t>(err) << std::endl
                  << "  csgId: " << static_cast<uint32_t>(csgId) << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

enum NetworkScanType { SCAN_RESULT_TYPE = 0x13, CIG_INFO_TYPE = 0x14, CSG_SIG_INFO_TYPE = 0x15 };

void printCsgPerformNwScanResult(const char* data, size_t dataLen) {
  if (!data || !dataLen) {
    std::cout << "data is not valid!!" << std::endl;
    return;
  }
  std::ostringstream logStream{};

  for (int i = 0; i < dataLen;) {
    uint16_t len = 0;
    uint8_t type = 0;
    memcpy(&type, data, 1);
    i += 1;
    data += 1;
    switch (type) {
      case SCAN_RESULT_TYPE:
        logStream << "TLV SCAN_RESULT_TYPE(0x13):" << std::endl;
        if (i + 2 < dataLen) {
          memcpy(&len, data, 2);
          logStream << "  len = " << static_cast<uint32_t>(len) << std::endl;
          i += 2;
          data += 2;
          if (len == 4) {
            uint32_t scan_result = 0;
            memcpy(&scan_result, data, 4);
            i += 4;
            data += 4;
            logStream << "  scan_result = " << scan_result << std::endl;
          } else {
            logStream << "Invalid value len";
          }
        } else {
          logStream << "Invalid len";
        }
        break;
      case CIG_INFO_TYPE:
        logStream << "TLV CIG_INFO_TYPE(0x14):" << std::endl;
        if (i + 2 < dataLen) {
          memcpy(&len, data, 2);
          i += 2;
          data += 2;
          logStream << "  len = " << static_cast<uint32_t>(len) << std::endl;
          uint8_t num_instances = 0;
          memcpy(&num_instances, data, 1);
          logStream << "  num_instances = " << static_cast<uint32_t>(num_instances) << std::endl;
          i += 1;
          data += 1;
          for (int j = 0; j < num_instances; j++) {
            logStream << "  csg_info[" << j << "] =" << std::endl;
            uint16_t mcc;
            memcpy(&mcc, data, 2);
            i += 2;
            data += 2;
            logStream << "    mcc = " << static_cast<uint32_t>(mcc) << std::endl;
            uint16_t mnc;
            memcpy(&mnc, data, 2);
            i += 2;
            data += 2;
            logStream << "    mnc = " << static_cast<uint32_t>(mnc) << std::endl;
            uint8_t mnc_includes_pcs_digit;
            memcpy(&mnc_includes_pcs_digit, data, 1);
            i += 1;
            data += 1;
            logStream << "    mnc_includes_pcs_digit = "
                      << static_cast<uint32_t>(mnc_includes_pcs_digit) << std::endl;
            uint32_t csg_list_cat;
            memcpy(&csg_list_cat, data, 4);
            i += 4;
            data += 4;
            logStream << "    csg_list_cat = " << static_cast<uint32_t>(csg_list_cat) << std::endl;
            // nas_csg_info_type_v01 csg_info;
            uint32_t csg_info_id;
            memcpy(&csg_info_id, data, 4);
            i += 4;
            data += 4;
            logStream << "    csg_info_id = " << static_cast<uint32_t>(csg_info_id) << std::endl;
            uint32_t csg_info_name_len;
            memcpy(&csg_info_name_len, data, 1);
            i += 1;
            data += 1;
            logStream << "    csg_info_name_len = " << static_cast<uint32_t>(csg_info_name_len)
                      << std::endl;
            uint16_t csg_info_name[48];
            memcpy(&csg_info_name, data, csg_info_name_len * 2);
            i += csg_info_name_len * 2;
            data += csg_info_name_len * 2;
            logStream << "    csg_info_name (in UTF16) = ";
            for (int n = 0; n < csg_info_name_len; n++) {
              logStream << static_cast<uint32_t>(csg_info_name[n]) << " ";
            }
            logStream << std::endl;
          }
        } else {
          logStream << "Invalid len";
        }
        break;
      case CSG_SIG_INFO_TYPE:
        logStream << "TLV CSG_SIG_INFO_TYPE(0x15):" << std::endl;
        if (i + 2 < dataLen) {
          memcpy(&len, data, 2);
          i += 2;
          data += 2;
          logStream << "  len = " << static_cast<uint32_t>(len) << std::endl;
          uint8_t num_instances = 0;
          memcpy(&num_instances, data, 1);
          logStream << "  num_instances = " << static_cast<uint32_t>(num_instances) << std::endl;
          i += 1;
          data += 1;
          for (int j = 0; j < num_instances; j++) {
            logStream << "  csg_sig_info[" << j << "] =" << std::endl;
            uint16_t mcc;
            memcpy(&mcc, data, 2);
            i += 2;
            data += 2;
            logStream << "    mcc = " << static_cast<uint32_t>(mcc) << std::endl;
            uint16_t mnc;
            memcpy(&mnc, data, 2);
            i += 2;
            data += 2;
            logStream << "    mnc = " << static_cast<uint32_t>(mnc) << std::endl;
            uint8_t mnc_includes_pcs_digit;
            memcpy(&mnc_includes_pcs_digit, data, 1);
            i += 1;
            data += 1;
            logStream << "    mnc_includes_pcs_digit = "
                      << static_cast<uint32_t>(mnc_includes_pcs_digit) << std::endl;
            uint32_t csg_id;
            memcpy(&csg_id, data, 4);
            i += 4;
            data += 4;
            logStream << "    csg_id = " << static_cast<uint32_t>(csg_id) << std::endl;
            int32_t signal_strength;
            memcpy(&signal_strength, data, 4);
            i += 4;
            data += 4;
            logStream << "    signal_strength = " << static_cast<uint32_t>(signal_strength)
                      << std::endl;
          }
        } else {
          logStream << "Invalid len";
        }
        break;
    }
  }
  std::cout << logStream.str() << std::endl;
}

void CsgMenu::csgPerformNetworkScan(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  // QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN (0x80000 + 150).
  uint32_t messageId = QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN;
  uint8_t inputData[8];
  uint32_t network_type, scan_type;
  inputData[0] = 0x10;  // type : QCRIL_OEMHOOK_CSG_TAG_NETWORK_TYPE
  inputData[1] = 0x01;  // length
  inputData[2] = 0x00;  // length
  std::cout << "Bitmask representing the network type to scan. Values"
            << "  Bit 0 -- GSM" << std::endl
            << "  Bit 1 -- UMTS" << std::endl
            << "  Bit 2 -- LTE" << std::endl
            << "  Bit 3 -- TD-SCDMA" << std::endl
            << "  Bit 4 -- NR5G" << std::endl
            << "  Any combination of the bit positions can be used" << std::endl
            << "Enter the CSG network type : " << std::endl;
  std::cin >> network_type;
  inputData[3] = network_type & 0xFF;  // value : NAS_NETWORK_TYPE_LTE_ONLY_V01
  inputData[4] = 0x11;                 // type : QCRIL_OEMHOOK_CSG_TAG_SCAN_TYPE
  inputData[5] = 0x01;                 // length
  inputData[6] = 0x00;                 // length
  std::cout << "Network scan type. Values:" << std::endl
            << "  0 (NAS_SCAN_TYPE_PLMN_V01)" << std::endl
            << "  1 (NAS_SCAN_TYPE_CSG_V01)" << std::endl
            << "  2 (NAS_SCAN_TYPE_MODE_PREF_V01)" << std::endl
            << "  3 (NAS_SCAN_TYPE_PCI_V01)" << std::endl
            << "  4 (NAS_SCAN_TYPE_CELL_SEARCH_V01)" << std::endl
            << "  5 (NAS_SCAN_TYPE_PCI_EXT" << std::endl
            << "  6 (NAS_SCAN_TYPE_SNPN_SEARCH" << std::endl
            << "Enter the scan type : " << std::endl;
  std::cin >> scan_type;
  inputData[7] = scan_type & 0xFF;  // value : NAS_SCAN_TYPE_CSG_V01

  auto [data, dataLen] = packOemhookRequest(messageId, inputData, 8);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout << "Got response for oemhookRaw(QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN): "
                  << std::endl;
        std::cout << "QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN: Response: err: "
                  << static_cast<uint32_t>(err) << std::endl;
        printBuffer(data, dataLen);
        printCsgPerformNwScanResult(data, dataLen);
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void CsgMenu::csgSetNetworkSelectionPreference(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  // QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF (0x80000 + 151).
  uint32_t messageId = QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF;
  uint8_t inputData[13];
  inputData[0] = 0x20; // type : QCRIL_OEMHOOK_CSG_TAG_CSG_INFO
  inputData[1] = 0x0A; // length
  inputData[2] = 0x00; // length
  std::cout << "Enter the mcc and mnc values for setting network selection preference" << std::endl;
  uint32_t csg_id;
  uint32_t mcc, mnc;
  uint32_t in_pcs, csg_rat;
  std::cout << "Enter mcc: ";
  std::cin >> mcc;
  std::cout << "Enter mnc: ";
  std::cin >> mnc;
  std::cout << "mnc_includes_pcs_digit: Values:" << std::endl
            << "  0 (FALSE)" << std::endl
            << "  1 (TRUE)" << std::endl
            << "Enter mnc_includes_pcs_digit: ";
  std::cin >> in_pcs;
  inputData[4] = mcc & 0xFF; // value : mcc
  inputData[3] = (mcc >> 8) & 0xFF; // value : mcc
  inputData[6] = mnc & 0xFF; // value : mnc
  inputData[5] = (mnc >> 8) & 0xFF; // value : mnc
  inputData[7] = in_pcs & 0xFF; // value : mnc_includes_pcs_digit
  std::cout << "Enter the closed subscriber group identifier: ";
  std::cin >> csg_id;
  inputData[11] = csg_id & 0xFF; // value : id
  inputData[10] = (csg_id >> 8) & 0xFF; // value : id
  inputData[9] = (csg_id >> 16) & 0xFF; // value : id
  inputData[8] = (csg_id >> 24) & 0xFF; // value : id
  std::cout << "Radio interface technology of the CSG network. Values:" << std::endl
            << "  4 (RADIO_IF_GSM)" << std::endl
            << "  5 (RADIO_IF_UMTS)" << std::endl
            << "  8 (RADIO_IF_LTE)" << std::endl
            << "  9 (RADIO_IF_TDSCDMA)" << std::endl
            << "Enter RAT: ";
  std::cin >> csg_rat;
  inputData[12] = csg_rat & 0xFF; // value : rat

  auto [data, dataLen] = packOemhookRequest(messageId, inputData, 12);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout << "Got response for oemhookRaw(QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF): " << err
                  << std::endl;
        std::cout << "QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF: Response:\n"
                  << "  err: " << static_cast<uint32_t>(err) << std::endl;
        printBuffer(data, dataLen);
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

void CsgMenu::processUnsolCsgIdChangedInd(const char* data, size_t dataLen) {
  uint32_t csgId = 0;
  if (data && (dataLen == sizeof(csgId))) {
    csgId = *((uint32_t*)(data));
  }
  std::cout << "QCRIL_REQ_HOOK_UNSOL_CSG_ID_CHANGE_IND: "
            << "csgId: " << static_cast<uint32_t>(csgId) << std::endl;
}


void CsgMenu::registerForIndications(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s =
      rilSession.registerOemhookIndicationHandler([this](const char* data, size_t dataLen) -> void {
        std::cout << "Got oemhook indication: dataLen = " << dataLen << std::endl;
        auto [messageId, indData, indDataLen] = unpackOemhookIndication(data, dataLen);
        std::cout << "messageId: " << messageId << std::endl;
        switch (messageId) {
          case QCRIL_REQ_HOOK_UNSOL_CSG_ID_CHANGE_IND:
            processUnsolCsgIdChangedInd(indData, indDataLen);
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
