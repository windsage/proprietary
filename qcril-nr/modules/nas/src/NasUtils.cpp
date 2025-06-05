/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "NasUtils.h"

NasSmsStatus convertNasSmsStatus(nas_sms_status_enum_type_v01 status) {
  NasSmsStatus ret = NasSmsStatus::STATUS_UNKNOWN;
  switch (status) {
    case NAS_SMS_STATUS_NO_SMS_V01:
      ret = NasSmsStatus::STATUS_NO_SMS;
      break;
    case NAS_SMS_STATUS_IMS_V01:
      ret = NasSmsStatus::STATUS_IMS;
      break;
    case NAS_SMS_STATUS_1X_V01:
      ret = NasSmsStatus::STATUS_1X;
      break;
    case NAS_SMS_STATUS_3GPP_V01:
      ret = NasSmsStatus::STATUS_3GPP;
      break;
    default:
      ret = NasSmsStatus::STATUS_UNKNOWN;
  }
  return ret;
}
