/******************************************************************************
#  Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <stdint.h>
#include <comdef.h>
#include <interfaces/nas/QcRilNasGetRoamStatusOverviewSyncMessage.h>
#include <interfaces/nas/QcRilGetPowerOptEnabledSyncMessage.h>
#include <interfaces/nas/QcRilGetUIStatusSyncMessage.h>
#include <interfaces/nas/QcRilNasQuery1XSmsBlockedStatusSyncMessage.h>
#include <interfaces/nas/QcRilGetLteSmsStatusSyncMessage.h>
#include <interfaces/nas/QcRilNasQueryIsVoiceOverLteRegisteredSyncMessage.h>
#include <interfaces/nas/QcRilNasQueryIsDeviceRegisteredCsDomainSyncMessage.h>
#include <interfaces/nas/QcRilNasQueryIsLteOnlyRegisteredSyncMessage.h>
#include <interfaces/nas/QcRilNasQuerySimulVoiceDataCapabilitySyncMessage.h>
#include <interfaces/nas/QcRilNasQueryReportedDataTechnologySyncMessage.h>

#include <interfaces/ims/QcRilRequestImsQueryCIWLANConfigSyncMessage.h>

#include <telephony/ril.h>
#include "qcril_sms_external.h"

bool qcril_qmi_sms_external_query_is_voice_over_lte_registered() {
  auto msg = std::make_shared<QcRilNasQueryIsVoiceOverLteRegisteredSyncMessage>();
  assert(msg != nullptr);
  bool is_voice_over_lte = false;

  std::shared_ptr<bool> shared_result;
  auto ret = msg->dispatchSync(shared_result);
  if (ret == Message::Callback::Status::SUCCESS && shared_result != nullptr)
    is_voice_over_lte = *shared_result;

  return is_voice_over_lte;
}

NasSmsStatus qcril_qmi_sms_external_get_lte_sms_status() {
  auto lteSms_msg = std::make_shared<QcRilGetLteSmsStatusSyncMessage>();
  assert(lteSms_msg != nullptr);
  NasSmsStatus lte_sms_status = NasSmsStatus::STATUS_UNKNOWN;

  std::shared_ptr<NasSmsStatus> shared_status;
  auto ret = lteSms_msg->dispatchSync(shared_status);
  if (ret == Message::Callback::Status::SUCCESS && shared_status != nullptr)
    lte_sms_status = *shared_status;

  return lte_sms_status;
}

bool qcril_qmi_sms_external_query_is_1x_sms_blocked() {
  auto msg = std::make_shared<QcRilNasQuery1XSmsBlockedStatusSyncMessage>();
  assert(msg != nullptr);
  bool is_1x_sms_blocked = false;

  std::shared_ptr<bool> shared_result;
  auto res = msg->dispatchSync(shared_result);
  if (res == Message::Callback::Status::SUCCESS && shared_result != nullptr)
    is_1x_sms_blocked = *shared_result;

  return is_1x_sms_blocked;
}

uint8_t qcril_qmi_sms_external_get_ui_status() {
  auto msg = std::make_shared<QcRilGetUIStatusSyncMessage>();
  assert(msg != nullptr);
  uint8_t atel_ui_status = FALSE;

  std::shared_ptr<uint8_t> shared_ui_status;
  auto res = msg->dispatchSync(shared_ui_status);
  if (res == Message::Callback::Status::SUCCESS && shared_ui_status != nullptr)
    atel_ui_status = *shared_ui_status;

  return atel_ui_status;
}

bool qcril_qmi_sms_external_query_is_power_opt_enabled() {
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

bool qcril_qmi_sms_external_query_is_device_registered_for_cs_domain() {
  auto msg = std::make_shared<QcRilNasQueryIsDeviceRegisteredCsDomainSyncMessage>();
  assert(msg != nullptr);
  std::shared_ptr<bool> shared_result;
  bool result = false;
  auto ret = msg->dispatchSync(shared_result);
  if (ret == Message::Callback::Status::SUCCESS && shared_result != nullptr) {
    result = *shared_result;
  }
  return result;
}

bool qcril_qmi_sms_external_query_is_lte_only_registered() {
  auto msg = std::make_shared<QcRilNasQueryIsLteOnlyRegisteredSyncMessage>();
  assert(msg != nullptr);
  std::shared_ptr<bool> shared_result;
  bool result = false;
  auto ret = msg->dispatchSync(shared_result);
  if (ret == Message::Callback::Status::SUCCESS && shared_result != nullptr) {
    result = *shared_result;
  }
  return result;
}

dms_simul_voice_and_data_capability_mask_v01
qcril_qmi_sms_external_query_simul_voice_data_capability() {
  auto msg = std::make_shared<QcRilNasQuerySimulVoiceDataCapabilitySyncMessage>();
  assert(msg != nullptr);
  std::shared_ptr<uint64_t> shared_result;
  uint64_t result = 0;
  auto ret = msg->dispatchSync(shared_result);
  if (ret == Message::Callback::Status::SUCCESS && shared_result != nullptr) {
    result = *shared_result;
  }
  return static_cast<dms_simul_voice_and_data_capability_mask_v01>(result);
}

int qcril_qmi_sms_external_query_reported_data_technology() {
  auto msg = std::make_shared<QcRilNasQueryReportedDataTechnologySyncMessage>();
  assert(msg != nullptr);
  std::shared_ptr<int> shared_result;
  int result = RADIO_TECH_UNKNOWN;
  auto ret = msg->dispatchSync(shared_result);
  if (ret == Message::Callback::Status::SUCCESS && shared_result != nullptr) {
    result = *shared_result;
  }
  return result;
}
