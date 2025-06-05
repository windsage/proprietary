/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "interfaces/nas/nas_types.h"

using qcril::interfaces::RIL_EccMapType;
void qcril_qmi_pbm_external_get_ril_ecc_map(RIL_EccMapType& map);
