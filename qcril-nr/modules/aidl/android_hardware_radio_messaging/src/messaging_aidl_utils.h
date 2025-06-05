/*===========================================================================
   Copyright (c) 2022 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#pragma once

#include "messaging_aidl_service.h"
#include "interfaces/sms/qcril_qmi_sms_types.h"

aidlmessaging::SendSmsResult makeSendSmsResult(std::shared_ptr<RilSendSmsResult_t> in);
void constructCdmaSms(RIL_CDMA_SMS_Message& rcsm, const aidlmessaging::CdmaSmsMessage& sms);
RIL_RadioTechnologyFamily convertAidlRadioTechToRil(aidlradio::RadioTechnologyFamily tech);
void makeCdmaSmsMessage(aidlmessaging::CdmaSmsMessage& msg, RIL_CDMA_SMS_Message& rilMsg);
