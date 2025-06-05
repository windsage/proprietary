/******************************************************************************
#  Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "device_management_service_v01.h"
#include <interfaces/ims/QcRilRequestImsQueryCIWLANConfigSyncMessage.h>
#include <interfaces/ims/ims.h>

bool qcril_qmi_sms_external_query_is_voice_over_lte_registered();
NasSmsStatus qcril_qmi_sms_external_get_lte_sms_status();
bool qcril_qmi_sms_external_query_is_1x_sms_blocked();
uint8_t qcril_qmi_sms_external_get_ui_status();
bool qcril_qmi_sms_external_query_is_power_opt_enabled();
bool qcril_qmi_sms_external_query_is_device_registered_for_cs_domain();
bool qcril_qmi_sms_external_query_is_lte_only_registered();
dms_simul_voice_and_data_capability_mask_v01
qcril_qmi_sms_external_query_simul_voice_data_capability();
int qcril_qmi_sms_external_query_reported_data_technology();
