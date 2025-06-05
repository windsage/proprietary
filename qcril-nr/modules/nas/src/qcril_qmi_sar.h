/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "interfaces/sar/RilRequestGetSarRevKeyMessage.h"
#include "interfaces/sar/RilRequestSetTransmitPowerMessage.h"


//===========================================================================
// qcril_qmi_sar_set_max_transmit_power
//===========================================================================
void qcril_qmi_sar_set_max_transmit_power(std::shared_ptr<RilRequestSetTransmitPowerMessage> msg);

//===========================================================================
// qcril_qmi_sar_get_sar_rev_key
//===========================================================================
void qcril_qmi_sar_get_sar_rev_key(std::shared_ptr<RilRequestGetSarRevKeyMessage> msg);
