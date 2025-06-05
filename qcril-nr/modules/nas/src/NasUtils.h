/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "interfaces/nas/QcRilGetLteSmsStatusSyncMessage.h"
#include "network_access_service_v01.h"

NasSmsStatus convertNasSmsStatus(nas_sms_status_enum_type_v01 status);
