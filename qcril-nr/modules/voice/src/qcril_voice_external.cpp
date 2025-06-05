/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <stdint.h>
#include <comdef.h>

#include "qcril_voice_external.h"

#include <interfaces/nas/QcRilGetPowerOptEnabledSyncMessage.h>
#include <interfaces/nas/QcRilGetUIStatusSyncMessage.h>
#include <interfaces/dms/QcRilDmsGetRadioStateSyncMessage.h>
#include <interfaces/nas/QcRilNasGetVoiceRadioTechSyncMessage.h>
#include <interfaces/nas/QcRilNasQueryImsAvailableSyncMessage.h>
#include <interfaces/nas/QcRilNasQueryScreenOffSyncMessage.h>
#include <interfaces/nas/QcRilNasGetCallServiceTypeSyncMessage.h>
#include <interfaces/nas/QcRilNasCheckCallModeAgainstDevCfgSyncMessage.h>

#include <interfaces/nas/QcRilNasGetEscvTypeSyncMessage.h>
#include <interfaces/nas/QcRilNasGetImsAddressFromEmergencyNumberSyncMessage.h>
#include <interfaces/nas/QcRilNasGetRegistrationStatusOverviewSyncMessage.h>
#include <interfaces/nas/QcRilNasQueryEmergencyNumberDisplayOnlySyncMessage.h>
#include <interfaces/nas/QcRilNasQueryEnforcableEmergencyNumberSyncMessage.h>
#include <interfaces/nas/QcRilNasQueryNormalVoiceCallCapabilitySyncMessage.h>
#include <interfaces/nas/QcRilNasQueryNwMccBasedRoamingEmergencyNumberSyncMessage.h>
#include <interfaces/nas/QcRilNasQueryWhetherInApmLeaveWindowSyncMessage.h>

uint8_t qcril_qmi_voice_external_get_ui_status() {
  auto msg = std::make_shared<QcRilGetUIStatusSyncMessage>();
  assert(msg != nullptr);
  uint8_t atel_ui_status = FALSE;

  std::shared_ptr<uint8_t> shared_ui_status;
  auto res = msg->dispatchSync(shared_ui_status);
  if (res == Message::Callback::Status::SUCCESS && shared_ui_status != nullptr)
    atel_ui_status = *shared_ui_status;

  return atel_ui_status;
}

RIL_RadioState qcril_qmi_voice_external_get_radio_state() {
  auto msg = std::make_shared<QcRilDmsGetRadioStateSyncMessage>();
  assert(msg != nullptr);
  RIL_RadioState state = RADIO_STATE_UNAVAILABLE;

  std::shared_ptr<RIL_RadioState> shared_state;
  auto res = msg->dispatchSync(shared_state);
  if (res == Message::Callback::Status::SUCCESS && shared_state != nullptr) state = *shared_state;

  return state;
}

voice_dial_call_service_type_enum_v02 qcril_qmi_voice_external_get_call_service_type(
    bool is_dialed_on_cs) {
  auto msg = std::make_shared<QcRilNasGetCallServiceTypeSyncMessage>(is_dialed_on_cs);
  assert(msg != nullptr);
  voice_dial_call_service_type_enum_v02 service_type = VOICE_DIAL_CALL_SRV_TYPE_CS_ONLY_V02;

  std::shared_ptr<voice_dial_call_service_type_enum_v02> shared_type;
  auto res = msg->dispatchSync(shared_type);
  if (res == Message::Callback::Status::SUCCESS && shared_type != nullptr)
    service_type = *shared_type;

  return service_type;
}

bool qcril_qmi_voice_external_is_ims_available() {
  auto msg = std::make_shared<QcRilNasQueryImsAvailableSyncMessage>();
  assert(msg != nullptr);
  bool avail = true;

  std::shared_ptr<bool> shared_avail;
  auto res = msg->dispatchSync(shared_avail);
  if (res == Message::Callback::Status::SUCCESS && shared_avail != nullptr) avail = *shared_avail;

  return avail;
}

VoiceRadioTechType qcril_qmi_voice_external_get_voice_radio_tech() {
  auto msg = std::make_shared<QcRilNasGetVoiceRadioTechSyncMessage>();
  assert(msg != nullptr);
  VoiceRadioTechType tech = { QMI_RIL_RTE_NONE, RADIO_TECH_UNKNOWN };

  std::shared_ptr<VoiceRadioTechType> shared_tech;
  auto res = msg->dispatchSync(shared_tech);
  if (res == Message::Callback::Status::SUCCESS && shared_tech != nullptr) tech = *shared_tech;

  return tech;
}

bool qcril_qmi_voice_external_is_call_mode_reasonable_based_on_devcfg(call_mode_enum_v02 call_mode) {
  auto msg = std::make_shared<QcRilNasCheckCallModeAgainstDevCfgSyncMessage>(call_mode);
  assert(msg != nullptr);
  bool match = true;

  std::shared_ptr<bool> shared_match;
  auto res = msg->dispatchSync(shared_match);
  if (res == Message::Callback::Status::SUCCESS && shared_match != nullptr) match = *shared_match;

  return match;
}

bool qcril_qmi_voice_external_is_screen_off() {
  auto msg = std::make_shared<QcRilNasQueryScreenOffSyncMessage>();
  assert(msg != nullptr);
  bool off = false;

  std::shared_ptr<bool> shared_off;
  auto res = msg->dispatchSync(shared_off);
  if (res == Message::Callback::Status::SUCCESS && shared_off != nullptr) off = *shared_off;

  return off;
}

bool qcril_qmi_voice_external_ecc_number_for_display_only(const char* number) {
  auto msg = std::make_shared<QcRilNasQueryEmergencyNumberDisplayOnlySyncMessage>(number);
  assert(msg != nullptr);
  bool display_only = false;

  std::shared_ptr<bool> shared_query;
  auto res = msg->dispatchSync(shared_query);
  if (res == Message::Callback::Status::SUCCESS && shared_query != nullptr)
    display_only = *shared_query;

  return display_only;
}

bool qcril_qmi_voice_external_designated_number_nw_mcc_roaming_emergency(const char* number) {
  auto msg = std::make_shared<QcRilNasQueryNwMccBasedRoamingEmergencyNumberSyncMessage>(number);
  assert(msg != nullptr);
  bool ret_value = false;

  std::shared_ptr<bool> shared_query;
  auto res = msg->dispatchSync(shared_query);
  if (res == Message::Callback::Status::SUCCESS && shared_query != nullptr)
    ret_value = *shared_query;

  return ret_value;
}

bool qcril_qmi_voice_external_designated_number_enforcable_ecc_number(const char* number) {
  auto msg = std::make_shared<QcRilNasQueryEnforcableEmergencyNumberSyncMessage>(number);
  assert(msg != nullptr);
  bool ret_value = false;

  std::shared_ptr<bool> shared_query;
  auto res = msg->dispatchSync(shared_query);
  if (res == Message::Callback::Status::SUCCESS && shared_query != nullptr)
    ret_value = *shared_query;

  return ret_value;
}

std::string qcril_qmi_voice_external_convert_emergency_number_to_ims_address(std::string number) {
  auto msg = std::make_shared<QcRilNasGetImsAddressFromEmergencyNumberSyncMessage>(number);
  assert(msg != nullptr);
  std::string ret_value = number;

  std::shared_ptr<std::string> shared_query;
  auto res = msg->dispatchSync(shared_query);
  if (res == Message::Callback::Status::SUCCESS && shared_query != nullptr)
    ret_value = *shared_query;

  return ret_value;
}

bool qcril_qmi_voice_external_in_apm_leave_window() {
  auto msg = std::make_shared<QcRilNasQueryWhetherInApmLeaveWindowSyncMessage>();
  assert(msg != nullptr);
  bool ret_value = false;

  std::shared_ptr<bool> shared_query;
  auto res = msg->dispatchSync(shared_query);
  if (res == Message::Callback::Status::SUCCESS && shared_query != nullptr)
    ret_value = *shared_query;

  return ret_value;
}

bool qcril_qmi_voice_external_has_normal_voice_call_capability() {
  auto msg = std::make_shared<QcRilNasQueryNormalVoiceCallCapabilitySyncMessage>();
  assert(msg != nullptr);
  bool ret_value = false;

  std::shared_ptr<bool> shared_query;
  auto res = msg->dispatchSync(shared_query);
  if (res == Message::Callback::Status::SUCCESS && shared_query != nullptr)
    ret_value = *shared_query;

  return ret_value;
}

int qcril_qmi_voice_external_get_escv_type(const char* number) {
  auto msg = std::make_shared<QcRilNasGetEscvTypeSyncMessage>(number);
  assert(msg != nullptr);
  int ret_value = 0;

  std::shared_ptr<int> shared_query;
  auto res = msg->dispatchSync(shared_query);
  if (res == Message::Callback::Status::SUCCESS && shared_query != nullptr)
    ret_value = *shared_query;

  return ret_value;
}

int qcril_qmi_voice_external_get_reg_status_overview() {
  auto msg = std::make_shared<QcRilNasGetRegistrationStatusOverviewSyncMessage>();
  assert(msg != nullptr);
  uint32_t ret_value = 0;

  std::shared_ptr<uint32_t> shared_query;
  auto res = msg->dispatchSync(shared_query);
  if (res == Message::Callback::Status::SUCCESS && shared_query != nullptr)
    ret_value = *shared_query;

  return ret_value;
}

bool qcril_qmi_voice_external_query_is_power_opt_enabled() {
  auto msg = std::make_shared<QcRilGetPowerOptEnabledSyncMessage>();
  assert(msg != nullptr);
  // set default as disabled
  bool power_opt_enabled = false;
  std::shared_ptr<bool> shared_enabled;
  auto res = msg->dispatchSync(shared_enabled);
  if (res == Message::Callback::Status::SUCCESS && shared_enabled != nullptr)
    power_opt_enabled = *shared_enabled;
  return power_opt_enabled;
}
