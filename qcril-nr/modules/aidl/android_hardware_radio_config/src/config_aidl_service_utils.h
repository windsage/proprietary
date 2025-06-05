/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "config_aidl_service.h"
#include "MessageCommon.h"

aidlconfig::PhoneCapability convertPhoneCapabilityToAidl(const qcril::interfaces::RIL_PhoneCapability& ril_phoneCap);
char* radio_config_bin_to_hexstring(const uint8_t* data_ptr, uint16_t data_len);
aidlconfig::SimSlotStatus convertUimSlotStatusToAidl(const RIL_UIM_SlotStatus &ril_slot_status);
void convertRilDataResponseErrorToAidl(rildata::ResponseError_t rilError, aidlradio::RadioError &aidlError);
void copy_mep_information(std::vector<aidlconfig::SimPortInfo> &port_info,const RIL_UIM_SlotStatus &ril_slot_status);
bool isPortStateActive(RIL_UIM_Port_State port_state);
