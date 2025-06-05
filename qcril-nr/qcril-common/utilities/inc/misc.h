/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QCRIL_QMI_UTILITIES_MISC_H
#define QCRIL_QMI_UTILITIES_MISC_H

unsigned int qcril_qmi_convert_radio_tech_to_radio_tech_family(unsigned int voice_radio_tech);

void qcril_qmi_print_hex(const uint8_t* msg, size_t msg_len);

#endif
