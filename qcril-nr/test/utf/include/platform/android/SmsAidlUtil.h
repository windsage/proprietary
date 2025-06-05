/*===========================================================================
 *
 *    Copyright (c) 2023 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/
#pragma once

#include "ril_utf_rild_sim.h"
#include <telephony/ril_cdma_sms.h>
#include <interfaces/sms/RilRequestCdmaSendSmsMessage.h>
#include <interfaces/sms/RilRequestSendSmsMessage.h>
#include "aidl/android/hardware/radio/messaging/IRadioMessagingIndication.h"
#include "aidl/android/hardware/radio/messaging/SendSmsResult.h"
#include "aidl/android/hardware/radio/messaging/CdmaSmsAck.h"
#include "aidl/android/hardware/radio/messaging/SmsWriteArgs.h"
#include "aidl/android/hardware/radio/messaging/CdmaSmsWriteArgs.h"
#include "aidl/android/hardware/radio/messaging/CdmaBroadcastSmsConfigInfo.h"
#include "aidl/android/hardware/radio/messaging/GsmBroadcastSmsConfigInfo.h"
#include "aidl/android/hardware/radio/messaging/GsmSmsMessage.h"
#include "aidl/android/hardware/radio/messaging/CdmaSmsWriteArgs.h"

namespace aidlradio {
using namespace aidl::android::hardware::radio;
}
namespace aidlmsg {
using namespace aidl::android::hardware::radio::messaging;
}

#define BOOL_TO_INT(x) (x ? 1 : 0)

void convertGsmSmsAidlToRilPayload(const aidlmsg::SendSmsResult& sms, RIL_SMS_Response& data);
void convertGsmSmsPayloadToAidl(void* data, size_t datalen, aidlmsg::GsmSmsMessage& sms);
void convertGsmBcConfigToAidl(void* in,
                              size_t datalen,
                              std::vector<aidlmsg::GsmBroadcastSmsConfigInfo>& out);
int32_t convertSmsWriteArgsStatusToAidl(int in);
void convertGsmSmsWritePayloadToAidl(void* in, size_t datalen, aidlmsg::SmsWriteArgs& out);

void convertCdmaSmsAckToAidl(void* in, size_t datalen, aidlmsg::CdmaSmsAck& out);
bool convertCdmaSmsErrorClassToAidl(RIL_CDMA_SMS_ErrorClass in);
void convertCdmaBcConfigToAidl(void* in,
                               size_t datalen,
                               std::vector<aidlmsg::CdmaBroadcastSmsConfigInfo>& out);
void convertCdmaSmsWritePayloadToAidl(void* in, size_t datalen, aidlmsg::CdmaSmsWriteArgs& out);
int32_t convertCdmaSmsWriteArgsStatusToAidl(int in);
int32_t convertCdmaSmsDigitModeToAidl(RIL_CDMA_SMS_DigitMode in);
int32_t convertCdmaSmsNumberTypeToAidl(RIL_CDMA_SMS_NumberType in);
int32_t convertCdmaSmsNumberPlanToAidl(RIL_CDMA_SMS_NumberPlan in);
int32_t convertCdmaSmsSubAddrTypeToAidl(RIL_CDMA_SMS_Subaddress in);
void convertRilCdmaSmsToAidl(RIL_CDMA_SMS_Message& in, aidlmsg::CdmaSmsMessage& out);

void aidlToRilCdmaSmsMessage(RIL_CDMA_SMS_Message& rcsm, const aidlmsg::CdmaSmsMessage& sms);
