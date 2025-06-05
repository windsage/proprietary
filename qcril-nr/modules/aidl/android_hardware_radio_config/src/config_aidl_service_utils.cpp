/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"
#include "config_aidl_service_utils.h"

aidlconfig::PhoneCapability convertPhoneCapabilityToAidl(const qcril::interfaces::RIL_PhoneCapability& ril_phoneCap){
  aidlconfig::PhoneCapability aidl_phoneCap{};
  aidl_phoneCap.maxActiveData = ril_phoneCap.maxActiveData;
  aidl_phoneCap.maxActiveInternetData = ril_phoneCap.maxActiveInternetData;
  aidl_phoneCap.isInternetLingeringSupported = ril_phoneCap.isInternetLingeringSupported;

  aidl_phoneCap.logicalModemIds.resize(ril_phoneCap.logicalModemList.size());

  for (int i = 0; i < ril_phoneCap.logicalModemList.size(); i++) {
    aidl_phoneCap.logicalModemIds[i] = ril_phoneCap.logicalModemList[i].modemId;
  }
  return aidl_phoneCap;
}

static char * radio_config_iccid_to_string (
  const uint8_t * iccid_ptr, uint16_t iccid_len) {
  char * iccid_str = nullptr;

  if (iccid_ptr == NULL || iccid_len == 0)
  {
    return iccid_str;
  }

  iccid_str = new char[2 * iccid_len + 1]{};

  if (iccid_str == nullptr)
  {
    return iccid_str;
  }

  for (uint32_t i = 0, j = 0; i < iccid_len && j < (2 * iccid_len); i++, j++)
  {
    uint8_t ch = iccid_ptr[i] & 0x0F;

    if (ch > 9)
    {
      iccid_str[j] = ch + 'A' - 0x0A;
    }
    else
    {
      iccid_str[j] = ch + '0';
    }

    j++;
    if (j >= (2 * iccid_len))
    {
      break;
    }

    ch = iccid_ptr[i] >> 4;
    if (ch > 9)
    {
      iccid_str[j] = ch + 'A' - 0x0A;
    }
    else
    {
      iccid_str[j] = ch + '0';
    }
  }

  return iccid_str;
} /* radio_config_iccid_to_string */

char * radio_config_bin_to_hexstring(
  const uint8_t * data_ptr, uint16_t data_len) {

  char*     out_ptr    = NULL;
  uint32_t  string_len = 0;

  if (data_ptr == NULL || data_len == 0)
  {
    return NULL;
  }

  string_len = (2 * data_len) + sizeof(char);

  out_ptr = new char[string_len];

  if (out_ptr != NULL)
  {
    uint16_t i = 0;
    for (i = 0; i < data_len; i++)
    {
      if (((data_ptr[i] >> 4) & 0x0F) < 0x0a)
      {
        out_ptr[i * 2] = ((data_ptr[i] >> 4) & 0x0F) + '0';
      }
      else
      {
        out_ptr[i * 2] = ((data_ptr[i] >> 4) & 0x0F) + 'a' - 10;
      }
      if ((data_ptr[i] & 0x0F) < 0x0a)
      {
        out_ptr[i * 2 + 1] = (data_ptr[i] & 0x0F) + '0';
      }
      else
      {
        out_ptr[i * 2 + 1] = (data_ptr[i] & 0x0F) + 'a' - 10;
      }
    }
    out_ptr[data_len * 2] = 0x0;
  }

  return out_ptr;
} /* radio_config_bin_to_hexstring */

/**
 * Converts UIM slot status info to AIDL Slot Status info
 */
aidlconfig::SimSlotStatus convertUimSlotStatusToAidl( const RIL_UIM_SlotStatus &ril_slot_status ) {

  aidlconfig::SimSlotStatus slot_status{};
  vector<aidlconfig::SimPortInfo> port_info(1);

  QCRIL_LOG_ERROR("In function convertUimSlotStatusToAidl ");

  switch(ril_slot_status.card_state)
  {
    case RIL_UIM_CARDSTATE_ABSENT:
      slot_status.cardState = aidlsim::CardStatus::STATE_ABSENT;
      break;

    case RIL_UIM_CARDSTATE_PRESENT:
      slot_status.cardState = aidlsim::CardStatus::STATE_PRESENT;
      break;

    case RIL_UIM_CARDSTATE_RESTRICTED:
      slot_status.cardState = aidlsim::CardStatus::STATE_RESTRICTED;
      break;

    default:
      slot_status.cardState = aidlsim::CardStatus::STATE_ERROR;
      break;
  }

  if (slot_status.cardState == aidlsim::CardStatus::STATE_PRESENT) {
    char* atr   = nullptr;
    char* eid = nullptr;
    char *iccid = nullptr;
    eid = radio_config_bin_to_hexstring(ril_slot_status.eid.data(),
                                         ril_slot_status.eid.size());

    atr = radio_config_bin_to_hexstring(ril_slot_status.atr.data(),
                                        ril_slot_status.atr.size());

    if (atr != nullptr) {
      std::string aidl_atr(atr);
      slot_status.atr = aidl_atr;
      delete[] atr;
    }

    if (eid != nullptr)
    {
      std::string aidl_eid(eid);
      slot_status.eid = aidl_eid;
      delete[] eid;
    }

    if (ril_slot_status.port_info.is_mep) {
      copy_mep_information(port_info,ril_slot_status);
      slot_status.portInfo = port_info;
      slot_status.supportedMepMode =
          static_cast<aidlconfig::MultipleEnabledProfilesMode>(
              ril_slot_status.port_info.mep_mode);
    } else {
      iccid = radio_config_iccid_to_string(ril_slot_status.iccid.data(),
                                           ril_slot_status.iccid.size());
      if (iccid != nullptr) {
        std::string aidl_iccid(iccid);
        port_info[0].iccId = aidl_iccid;
        delete[] iccid;
      }
      port_info[0].logicalSlotId = ril_slot_status.logical_slot;
      switch (ril_slot_status.slot_state) {
      case RIL_UIM_PHYSICAL_SLOT_STATE_ACTIVE:
        port_info[0].portActive = true;
        break;

      default:
        port_info[0].portActive = false;
        break;
      }
      slot_status.portInfo = port_info;
    }
  }
  else
  {
    if (ril_slot_status.port_info.is_mep) {
      copy_mep_information(port_info,ril_slot_status);
        slot_status.portInfo = port_info;
        slot_status.supportedMepMode =
            static_cast<aidlconfig::MultipleEnabledProfilesMode>(
                ril_slot_status.port_info.mep_mode);
      }
      else {
      port_info[0].logicalSlotId = ril_slot_status.logical_slot;

      switch (ril_slot_status.slot_state) {
      case RIL_UIM_PHYSICAL_SLOT_STATE_ACTIVE:
        port_info[0].portActive = true;
        break;

      default:
        port_info[0].portActive = false;
        break;
      }
      slot_status.portInfo = port_info;
      slot_status.supportedMepMode =
            static_cast<aidlconfig::MultipleEnabledProfilesMode>(
                     ril_slot_status.port_info.mep_mode);
      }
  }
  return slot_status;
}

void convertRilDataResponseErrorToAidl(rildata::ResponseError_t rilError, aidlradio::RadioError &aidlError)
{
  switch (rilError) {
    case rildata::ResponseError_t::NO_ERROR:
      aidlError = aidlradio::RadioError::NONE;
      break;
    case rildata::ResponseError_t::NOT_AVAILABLE:
      aidlError = aidlradio::RadioError::RADIO_NOT_AVAILABLE;
      break;
    case rildata::ResponseError_t::INTERNAL_ERROR:
      aidlError = aidlradio::RadioError::INTERNAL_ERR;
      break;
    case rildata::ResponseError_t::INVALID_ARGUMENT:
      aidlError = aidlradio::RadioError::INVALID_ARGUMENTS;
      break;
    default:
      aidlError = aidlradio::RadioError::GENERIC_FAILURE;
      break;
  }
}
void copy_mep_information (
 std::vector<aidlconfig::SimPortInfo> &port_info,
 const RIL_UIM_SlotStatus &ril_slot_status
)
{
 char *iccid=nullptr;
 port_info.resize(
    ril_slot_status.port_info.uim_port_status_len);

 for (uint32_t i = 0; i < ril_slot_status.port_info.uim_port_status_len;
     i++) {
  iccid = radio_config_iccid_to_string(
      ril_slot_status.port_info.uim_port_status[i].iccid.data(),
      ril_slot_status.port_info.uim_port_status[i].iccid.size());
  if (iccid != nullptr) {
    std::string aidl_iccid(iccid);
    port_info[i].iccId = aidl_iccid;
    delete[] iccid;
  }

  port_info[i].logicalSlotId =
      ril_slot_status.port_info.uim_port_status[i].logical_slot;

  port_info[i].portActive = isPortStateActive(ril_slot_status.port_info.
                            uim_port_status[i].port_state);
 }
}

bool isPortStateActive(RIL_UIM_Port_State port_state)
{
  QCRIL_LOG_DEBUG("portstate update");
  return (port_state == RIL_UIM_PORT_STATE_ACTIVE) ? true : false;
}
