/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "telephony/ril.h"
#include "voice_service_v02.h"
#include "interfaces/nas/QcRilNasGetVoiceRadioTechSyncMessage.h"

uint8_t qcril_qmi_voice_external_get_ui_status();

RIL_RadioState qcril_qmi_voice_external_get_radio_state();

voice_dial_call_service_type_enum_v02 qcril_qmi_voice_external_get_call_service_type(
    bool is_dialed_on_cs);

bool qcril_qmi_voice_external_is_ims_available();

VoiceRadioTechType qcril_qmi_voice_external_get_voice_radio_tech();

bool qcril_qmi_voice_external_is_call_mode_reasonable_based_on_devcfg(call_mode_enum_v02 call_mode);

bool qcril_qmi_voice_external_is_screen_off();

bool qcril_qmi_voice_external_ecc_number_for_display_only(const char* number);
bool qcril_qmi_voice_external_designated_number_nw_mcc_roaming_emergency(const char* number);
bool qcril_qmi_voice_external_designated_number_enforcable_ecc_number(const char* number);
std::string qcril_qmi_voice_external_convert_emergency_number_to_ims_address(std::string number);
bool qcril_qmi_voice_external_in_apm_leave_window();
bool qcril_qmi_voice_external_has_normal_voice_call_capability();
int qcril_qmi_voice_external_get_escv_type(const char* number);
int qcril_qmi_voice_external_get_reg_status_overview();
bool qcril_qmi_voice_external_query_is_power_opt_enabled();
