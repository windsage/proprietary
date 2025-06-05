/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "telephony/ril.h"
#include "utils_internal.h"

/*=========================================================================
  FUNCTION:  qcril_qmi_convert_radio_tech_to_radio_tech_family

===========================================================================*/
/*!
    @brief
    Converts the reported radio tech to radio tech family


    @return
    radio tech family.
*/
/*=========================================================================*/
unsigned int qcril_qmi_convert_radio_tech_to_radio_tech_family(unsigned int voice_radio_tech) {
  unsigned int voice_radio_tech_family = RADIO_TECH_UNKNOWN;

  /* convert to the radio tech family in terms of radio tech*/
  switch (voice_radio_tech) {
    case RADIO_TECH_GPRS:
    case RADIO_TECH_EDGE:
    case RADIO_TECH_UMTS:
    case RADIO_TECH_TD_SCDMA:
    case RADIO_TECH_HSDPA:
    case RADIO_TECH_HSUPA:
    case RADIO_TECH_HSPA:
    case RADIO_TECH_LTE:
    case RADIO_TECH_5G:
    case RADIO_TECH_HSPAP:
    case RADIO_TECH_GSM:
      voice_radio_tech_family = RADIO_TECH_3GPP;
      break;

    case RADIO_TECH_IS95A:
    case RADIO_TECH_IS95B:
    case RADIO_TECH_1xRTT:
    case RADIO_TECH_EVDO_0:
    case RADIO_TECH_EVDO_A:
    case RADIO_TECH_EVDO_B:
    case RADIO_TECH_EHRPD:
      voice_radio_tech_family = RADIO_TECH_3GPP2;
      break;

    default:
      UTIL_LOG_ERROR("invalid radio tech = %d", voice_radio_tech);
      break;
  }

  return voice_radio_tech_family;
} /* qcril_qmi_voice_nas_control_convert_radio_tech_to_radio_tech_family */

void qcril_qmi_print_hex(const uint8_t* msg, size_t msg_len) {
#define QCRIL_PRINT_MAX_BYTES_PER_LINE 16U
  const char hex_chart[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                             '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };  // Do not change this array
  unsigned char buf[QCRIL_PRINT_MAX_BYTES_PER_LINE * 3 + 1];
  size_t buf_idx = 0;             // 0 to QCRIL_PRINT_MAX_BYTES_PER_LINE * 3
  size_t msg_idx = 0;             // 0 to msg_len - 1
  size_t bytes_per_line_idx = 0;  // 0 to QCRIL_PRINT_MAX_BYTES_PER_LINE - 1
  if (msg && msg_len > 0) {
    while (msg_idx < msg_len) {
      for (bytes_per_line_idx = 0, buf_idx = 0;
           (bytes_per_line_idx < QCRIL_PRINT_MAX_BYTES_PER_LINE) && (msg_idx < msg_len);
           bytes_per_line_idx++, msg_idx++) {
        buf[buf_idx] = hex_chart[(msg[msg_idx] >> 4) & 0x0F];
        buf_idx++;
        buf[buf_idx] = hex_chart[msg[msg_idx] & 0x0F];
        buf_idx++;
        buf[buf_idx] = ' ';
        buf_idx++;
      }
      buf[buf_idx] = '\0';
      UTIL_LOG_DEBUG("%s", buf);
    }
  }
} /* qcril_qmi_print_hex */

