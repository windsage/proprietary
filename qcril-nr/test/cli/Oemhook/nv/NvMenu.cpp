/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * OEMHOOK NV Menu
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <limits>
#include <utility>

#include "../OemhookUtils.hpp"
#include "NvMenu.hpp"
#include "interfaces/nv/nv_items.h"

using namespace std;

NvMenu::NvMenu(std::string appName, std::string cursor, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
}

NvMenu::~NvMenu() {
}

void NvMenu::init() {
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandListQtunerSubMenu = {
    CREATE_COMMAND(NvMenu::nvRead, "nvRead"),
    CREATE_COMMAND(NvMenu::nvWrite, "nvWrite"),
  };

  addCommands(commandListQtunerSubMenu);
  ConsoleApp::displayMenu();
}

static int32_t getNvItemId() {
  int32_t _userInput = -1;
  std::cout << "Enter NV item\n";
  std::cout << " Below are the valid values; -1 to exit\n";
  std::cout << " NV_AUTO_ANSWER_I        = 74\n";
  std::cout << " NV_PREF_VOICE_SO_I      = 285\n";
  std::cout << " NV_ROAM_CNT_I           = 169\n";
  std::cout << " NV_AIR_CNT_I            = 168\n";
  std::cout << " NV_MIN1_I               = 32\n";
  std::cout << " NV_MIN2_I               = 33\n";
  std::cout << " NV_IMSI_MCC_I           = 176\n";
  std::cout << " NV_IMSI_11_12_I         = 177\n";
  std::cout << " NV_IMSI_T_S1_I          = 262\n";
  std::cout << " NV_IMSI_T_S2_I          = 263\n";
  std::cout << " NV_IMSI_T_MCC_I         = 264\n";
  std::cout << " NV_IMSI_T_11_12_I       = 265\n";
  std::cout << " NV_IMSI_T_ADDR_NUM_I    = 266\n";
  std::cout << " NV_PCDMACH_I            = 20\n";
  std::cout << " NV_SCDMACH_I            = 21\n";
  std::cout << " NV_HOME_SID_NID_I       = 259\n";
  std::cout << " NV_DIR_NUMBER_I         = 178\n";
  std::cout << " NV_SID_NID_I            = 38\n";
  std::cout << " NV_MOB_CAI_REV_I        = 6\n";
  std::cout << " NV_NAME_NAM_I           = 43\n";
  std::cin >> _userInput;
  return _userInput;
}

static bool getNvItemValue(nv_auto_answer_type& nvItemValue) {
  std::cout << "Enter NV_AUTO_ANSWER_I :\n"
            << " enable (0 for false, any other value for true): ";
  uint32_t _userInput = 0;
  std::cin >> _userInput;
  nvItemValue.enable = _userInput ? 1 : 0;
  return true;
}

static bool geVoiceSoValue(uint32_t& val) {
  std::cout << "Valid values:\n";
  std::cout << " 0x0000 : Any service option \n";
  std::cout << " 0x0001 : IS-96A \n";
  std::cout << " 0x0003 : EVRC \n";
  std::cout << " 0x0011 : 13K_IS733 \n";
  std::cout << " 0x0038 : Selectable mode vocoder \n";
  std::cout << " 0x0044 : 4GV narrowband \n";
  std::cout << " 0x0046 : 4GV wideband \n";
  std::cout << " 0x0049 : EVRC-SO73 \n";
  std::cout << " 0x8000 : 13K \n";
  std::cout << " 0x8001 : IS-96A \n";
  std::cout << " 0x8023 : WVRC\n";
  std::cin >> val;
  return true;
}
static bool getNvItemValue(nv_pref_voice_so_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter NV_PREF_VOICE_SO_I :\n"
            << " nam (NAM id 0-N) : " << nvItemValue.nam;
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " evrc_capability_enabled (0 for false, any other value for true) : ";
  std::cin >> _userInput;
  nvItemValue.evrc_capability_enabled = _userInput ? 1 : 0;
  std::cout << " home_page_voice_so :\n";
  geVoiceSoValue(_userInput);
  nvItemValue.home_page_voice_so = _userInput;
  std::cout << " home_orig_voice_so :\n";
  geVoiceSoValue(_userInput);
  nvItemValue.home_orig_voice_so = _userInput;
  std::cout << " roam_orig_voice_so :\n";
  geVoiceSoValue(_userInput);
  nvItemValue.roam_orig_voice_so = _userInput;
  return true;
}
static bool getNvItemValue(nv_call_cnt_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_call_cnt_type :\n"
            << " nam (NAM id 0-N) : " << nvItemValue.nam;
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " cnt : ";
  std::cin >> _userInput;
  nvItemValue.cnt = _userInput;
  return true;
}
static bool getNvItemValue(nv_min1_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_min1_type :\n"
            << " nam (NAM id 0-N) : " << nvItemValue.nam;
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " min1 : ";
  std::cin >> _userInput;
  nvItemValue.min1[1] = _userInput;
  return true;
}
static bool getNvItemValue(nv_min2_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_min2_type :\n"
            << " nam (NAM id 0-N) : " << nvItemValue.nam;
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " min2 : ";
  std::cin >> _userInput;
  nvItemValue.min2[1] = _userInput;
  return true;
}
static bool getNvItemValue(nv_imsi_mcc_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_imsi_mcc_type :\n"
            << " nam (NAM id 0-N) : " << nvItemValue.nam;
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " imsi_mcc : ";
  std::cin >> _userInput;
  nvItemValue.imsi_mcc = _userInput;
  return true;
}
static bool getNvItemValue(nv_imsi_11_12_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_imsi_11_12_type :\n"
            << " nam (NAM id 0-N) : " << nvItemValue.nam;
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " imsi_11_12 : ";
  std::cin >> _userInput;
  nvItemValue.imsi_11_12 = _userInput;
  return true;
}
static bool getNvItemValue(nv_imsi_addr_num_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_imsi_addr_num_type :\n"
            << " nam (NAM id 0-N) : " << nvItemValue.nam;
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " num (Length of the IMSI for this NAM) : ";
  std::cin >> _userInput;
  nvItemValue.num = _userInput;
  return true;
}
static bool getNvItemValue(nv_cdmach_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_cdmach_type :\n"
            << " nam (NAM id 0-N) : " << nvItemValue.nam;
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " channel_a (A carrier channel number) : ";
  std::cin >> _userInput;
  nvItemValue.channel_a = _userInput;
  std::cout << " channel_b (B carrier channel number) : ";
  std::cin >> _userInput;
  nvItemValue.channel_b = _userInput;
  return true;
}
static bool getNvItemValue(nv_sid_nid_pair_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_sid_nid_pair_type: \n"
            << " sid : ";
  std::cin >> _userInput;
  nvItemValue.sid = _userInput;
  std::cout << " nid : ";
  std::cin >> _userInput;
  nvItemValue.nid = _userInput;
  return true;
}
static bool getNvItemValue(nv_home_sid_nid_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_home_sid_nid_type :\n"
            << " nam (NAM id 0-N) : " << nvItemValue.nam;
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " Number of SID+NID pairs (Up to 20) : ";
  std::cin >> _userInput;
  uint32_t len = _userInput;
  for (uint32_t i = 0; i < len && i < NV_MAX_HOME_SID_NID; i++) {
    getNvItemValue(nvItemValue.pair[i]);
  }
  return true;
}
static bool getNvItemValue(nv_dir_number_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_dir_number_type :\n"
            << " nam (NAM id 0-N) : ";
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " dir_number : ";
  std::string dirNumber;
  std::cin >> dirNumber;
  strlcpy((char *)nvItemValue.dir_number, dirNumber.c_str(), sizeof(nvItemValue.dir_number));
  return true;
}
static bool getNvItemValue(nv_name_nam_type& nvItemValue) {
  uint32_t _userInput = 0;
  std::cout << "Enter nv_name_nam_type :\n"
            << " nam (NAM id 0-N) : ";
  std::cin >> _userInput;
  nvItemValue.nam = _userInput;
  std::cout << " name (NAM name string) : ";
  std::string name;
  std::cin >> name;
  strlcpy((char *)nvItemValue.name, name.c_str(), sizeof(nvItemValue.name));
  return true;
}
static size_t getNvItemValue(uint32_t nvItemId, nv_item_type& nvItemValue) {
  switch (nvItemId) {
    case NV_AUTO_ANSWER_I:
      getNvItemValue(nvItemValue.auto_answer);
      return sizeof(nvItemValue.auto_answer);
      break;
    case NV_PREF_VOICE_SO_I:
      getNvItemValue(nvItemValue.pref_voice_so);
      return sizeof(nvItemValue.pref_voice_so);
      break;
    case NV_ROAM_CNT_I:
      std::cout << "Enter NV_ROAM_CNT_I :\n";
      getNvItemValue(nvItemValue.roam_cnt);
      return sizeof(nvItemValue.roam_cnt);
      break;
    case NV_AIR_CNT_I:
      std::cout << "Enter NV_AIR_CNT_I :\n";
      getNvItemValue(nvItemValue.air_cnt);
      return sizeof(nvItemValue.air_cnt);
      break;
    case NV_MIN1_I:
      std::cout << "Enter NV_MIN1_I :\n";
      getNvItemValue(nvItemValue.min1);
      return sizeof(nvItemValue.min1);
      break;
    case NV_MIN2_I:
      std::cout << "Enter NV_MIN2_I :\n";
      getNvItemValue(nvItemValue.min2);
      return sizeof(nvItemValue.min2);
      break;
    case NV_IMSI_MCC_I:
      std::cout << "Enter NV_IMSI_MCC_I :\n";
      getNvItemValue(nvItemValue.imsi_mcc);
      return sizeof(nvItemValue.imsi_mcc);
      break;
    case NV_IMSI_11_12_I:
      std::cout << "Enter NV_IMSI_11_12_I :\n";
      getNvItemValue(nvItemValue.imsi_11_12);
      return sizeof(nvItemValue.imsi_11_12);
      break;
    case NV_IMSI_T_S1_I:
      std::cout << "Enter NV_IMSI_T_S1_I :\n";
      getNvItemValue(nvItemValue.imsi_t_s1);
      return sizeof(nvItemValue.imsi_t_s1);
      break;
    case NV_IMSI_T_S2_I:
      std::cout << "Enter NV_IMSI_T_S2_I :\n";
      getNvItemValue(nvItemValue.imsi_t_s2);
      return sizeof(nvItemValue.imsi_t_s2);
      break;
    case NV_IMSI_T_MCC_I:
      std::cout << "Enter NV_IMSI_T_MCC_I :\n";
      getNvItemValue(nvItemValue.imsi_t_mcc);
      return sizeof(nvItemValue.imsi_t_mcc);
      break;
    case NV_IMSI_T_11_12_I:
      std::cout << "Enter NV_IMSI_T_11_12_I :\n";
      getNvItemValue(nvItemValue.imsi_t_11_12);
      return sizeof(nvItemValue.imsi_t_11_12);
      break;
    case NV_IMSI_T_ADDR_NUM_I:
      std::cout << "Enter NV_IMSI_T_ADDR_NUM_I :\n";
      getNvItemValue(nvItemValue.imsi_t_addr_num);
      return sizeof(nvItemValue.imsi_t_addr_num);
      break;
    case NV_PCDMACH_I:
      std::cout << "Enter NV_PCDMACH_I :\n";
      getNvItemValue(nvItemValue.pcdmach);
      return sizeof(nvItemValue.pcdmach);
      break;
    case NV_SCDMACH_I:
      std::cout << "Enter NV_SCDMACH_I :\n";
      getNvItemValue(nvItemValue.scdmach);
      return sizeof(nvItemValue.scdmach);
      break;
    case NV_HOME_SID_NID_I:
      std::cout << "Enter NV_HOME_SID_NID_I :\n";
      getNvItemValue(nvItemValue.home_sid_nid);
      return sizeof(nvItemValue.home_sid_nid);
      break;
    case NV_DIR_NUMBER_I:
      std::cout << "Enter NV_DIR_NUMBER_I :\n";
      getNvItemValue(nvItemValue.dir_number);
      return sizeof(nvItemValue.dir_number);
      break;
    case NV_SID_NID_I:
      std::cout << "NOTE: NV WRITE NV_SID_NID_I is not supported!!!\n";
      //getNvItemValue(nvItemValue.sid_nid);
      return sizeof(nvItemValue.sid_nid);
      break;
    case NV_MOB_CAI_REV_I:
      {
      uint32_t _userInput = 0;
      std::cout << "Enter NV_MOB_CAI_REV_I :\n"
                << " mob_cai_rev : ";
      std::cin >> _userInput;
      nvItemValue.mob_cai_rev = _userInput;
      return sizeof(nvItemValue.mob_cai_rev);
      }
      break;
    case NV_NAME_NAM_I:
      std::cout << "Enter NV_NAME_NAM_I :\n";
      getNvItemValue(nvItemValue.name_nam);
      return sizeof(nvItemValue.name_nam);
      break;
    default:
      std::cout << "Invalid NV: " << nvItemId << "\n";
      break;
  }
  return 0;
}

static void printNvItem(nv_auto_answer_type& nvItemValue) {
  std::cout << "NV_AUTO_ANSWER_I :"
            << " enable = " << nvItemValue.enable << " rings = " << nvItemValue.rings << "\n";
}

static void printNvItem(nv_pref_voice_so_type& nvItemValue) {
  std::cout << "NV_PREF_VOICE_SO_I :"
            << " nam = " << nvItemValue.nam
            << " evrc_capability_enabled = " << nvItemValue.evrc_capability_enabled
            << " home_page_voice_so = " << nvItemValue.home_page_voice_so
            << " home_orig_voice_so = " << nvItemValue.home_orig_voice_so
            << " roam_orig_voice_so = " << nvItemValue.roam_orig_voice_so << "\n";
}
static void printNvItem(nv_item_type& nvItemValue) {
  uint8_t* buf = (uint8_t*)&nvItemValue;
  size_t bufLen = sizeof(nvItemValue);
  std::cout << "nv_item_type : ";
  std::cout << std::hex;
  for (size_t i = 0; i < bufLen; i++) {
    std::cout << static_cast<unsigned int>(buf[i]) << " ";
  }
  std::cout << std::dec;
  std::cout << std::endl;
}

static void printNvItemValue(uint32_t nvItemId, nv_item_type& nvItemValue) {
  printNvItem(nvItemValue);
  switch (nvItemId) {
    case NV_AUTO_ANSWER_I:
      printNvItem(nvItemValue.auto_answer);
      break;
    case NV_PREF_VOICE_SO_I:
      printNvItem(nvItemValue.pref_voice_so);
      break;
    case NV_ROAM_CNT_I:
    case NV_AIR_CNT_I:
    case NV_MIN1_I:
    case NV_MIN2_I:
    case NV_IMSI_MCC_I:
    case NV_IMSI_11_12_I:
    case NV_IMSI_T_S1_I:
    case NV_IMSI_T_S2_I:
    case NV_IMSI_T_MCC_I:
    case NV_IMSI_T_11_12_I:
    case NV_IMSI_T_ADDR_NUM_I:
    case NV_PCDMACH_I:
    case NV_SCDMACH_I:
    case NV_HOME_SID_NID_I:
    case NV_DIR_NUMBER_I:
    case NV_SID_NID_I:
    case NV_MOB_CAI_REV_I:
    case NV_NAME_NAM_I:
      std::cout << "TODO\n";
      break;
    default:
      std::cout << "Invalid NV: " << nvItemId << "\n";
      break;
  }
}

void NvMenu::nvRead(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int32_t _userInput = -1;
  do {
    _userInput = getNvItemId();
  } while (std::cin.fail() || _userInput < -1);
  if (_userInput == -1) {
    return;
  }
  uint32_t nvItemId = _userInput;
  // QCRIL_REQ_HOOK_NV_READ (0x80000 + 1).
  uint32_t messageId = QCRIL_REQ_HOOK_NV_READ;
  auto [data, dataLen] = packOemhookRequest(messageId, (const uint8_t*)&nvItemId, sizeof(nvItemId));
  Status s = rilSession.oemhookRaw(
      data, dataLen, [nvItemId](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout << "Got response for oemhookRaw(QCRIL_REQ_HOOK_NV_READ): " << err << std::endl;
        nv_item_type nvItemValue;
        memset(&nvItemValue, 0, sizeof(nvItemValue));
        if (err == RIL_E_SUCCESS) {
          if (data && dataLen) {
            if (dataLen >= sizeof(nvItemValue)) {
              memcpy(&nvItemValue, data, dataLen);
              printNvItemValue(nvItemId, nvItemValue);
            }
          }
        }
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}

typedef struct __attribute__((__packed__)) {
  uint32_t nvItemId;
  uint32_t nvItemValueSize;
  nv_item_type nvItemValue;
} NvWriteRequestData;

void NvMenu::nvWrite(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int32_t _userInput = -1;
  do {
    _userInput = getNvItemId();
  } while (std::cin.fail() || _userInput < -1);
  if (_userInput == -1) {
    return;
  }
  NvWriteRequestData reqData;
  memset(&reqData, 0, sizeof(reqData));
  reqData.nvItemId = _userInput;
  reqData.nvItemValueSize = getNvItemValue(reqData.nvItemId, reqData.nvItemValue);
  if (reqData.nvItemValueSize == 0) {
    std::cout << "Invalid NV ITEM SIZE\n";
    return;
  }
  size_t reqDataSize =
      sizeof(reqData.nvItemId) + sizeof(reqData.nvItemValueSize) + reqData.nvItemValueSize;
  // QCRIL_REQ_HOOK_NV_WRITE (0x80000 + 1).
  uint32_t messageId = QCRIL_REQ_HOOK_NV_WRITE;
  auto [data, dataLen] = packOemhookRequest(messageId, (const uint8_t*)&reqData, reqDataSize);
  Status s = rilSession.oemhookRaw(
      data, dataLen, [](RIL_Errno err, const char* data, size_t dataLen) -> void {
        std::cout << "Got response for oemhookRaw(QCRIL_REQ_HOOK_NV_WRITE): " << err << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send oemhookRaw" << std::endl;
  }
  delete data;
}
