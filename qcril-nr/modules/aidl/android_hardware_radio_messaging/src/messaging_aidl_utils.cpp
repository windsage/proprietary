/*===========================================================================
   Copyright (c) 2022 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "messaging_aidl_service.h"
#include "messaging_aidl_utils.h"

#define BOOL_TO_INT(x) (x ? 1 : 0)

aidlmessaging::SendSmsResult makeSendSmsResult(std::shared_ptr<RilSendSmsResult_t> in) {
  aidlmessaging::SendSmsResult result = { -1, "", -1 };
  if (in) {
    result.messageRef = in->messageRef;
    result.ackPDU = in->ackPDU;
    result.errorCode = in->errorCode;
  }

  return result;
}

void constructCdmaSms(RIL_CDMA_SMS_Message& rcsm, const aidlmessaging::CdmaSmsMessage& sms) {
  rcsm.uTeleserviceID = sms.teleserviceId;
  rcsm.bIsServicePresent = BOOL_TO_INT(sms.isServicePresent);
  rcsm.uServicecategory = sms.serviceCategory;
  rcsm.sAddress.digit_mode = static_cast<RIL_CDMA_SMS_DigitMode>(sms.address.digitMode);
  if (sms.address.isNumberModeDataNetwork) {
    rcsm.sAddress.number_mode = RIL_CDMA_SMS_NUMBER_MODE_DATA_NETWORK;
  } else {
    rcsm.sAddress.number_mode = RIL_CDMA_SMS_NUMBER_MODE_NOT_DATA_NETWORK;
  }
  rcsm.sAddress.number_type = static_cast<RIL_CDMA_SMS_NumberType>(sms.address.numberType);
  rcsm.sAddress.number_plan = static_cast<RIL_CDMA_SMS_NumberPlan>(sms.address.numberPlan);

  rcsm.sAddress.number_of_digits = sms.address.digits.size();
  int digitLimit = MIN((rcsm.sAddress.number_of_digits), RIL_CDMA_SMS_ADDRESS_MAX);
  for (int i = 0; i < digitLimit; i++) {
    rcsm.sAddress.digits[i] = sms.address.digits[i];
  }

  rcsm.sSubAddress.subaddressType =
      static_cast<RIL_CDMA_SMS_SubaddressType>(sms.subAddress.subaddressType);
  rcsm.sSubAddress.odd = BOOL_TO_INT(sms.subAddress.odd);

  rcsm.sSubAddress.number_of_digits = sms.subAddress.digits.size();
  digitLimit = MIN((rcsm.sSubAddress.number_of_digits), RIL_CDMA_SMS_SUBADDRESS_MAX);
  for (int i = 0; i < digitLimit; i++) {
    rcsm.sSubAddress.digits[i] = sms.subAddress.digits[i];
  }

  rcsm.uBearerDataLen = sms.bearerData.size();
  digitLimit = MIN((rcsm.uBearerDataLen), RIL_CDMA_SMS_BEARER_DATA_MAX);
  for (int i = 0; i < digitLimit; i++) {
    rcsm.aBearerData[i] = sms.bearerData[i];
  }
}

void makeCdmaSmsMessage(aidlmessaging::CdmaSmsMessage& msg, RIL_CDMA_SMS_Message& rilMsg) {
  msg.teleserviceId = rilMsg.uTeleserviceID;
  msg.isServicePresent = rilMsg.bIsServicePresent;
  msg.serviceCategory = rilMsg.uServicecategory;
  msg.address.digitMode = static_cast<int32_t>(rilMsg.sAddress.digit_mode);
  if (rilMsg.sAddress.number_mode == RIL_CDMA_SMS_NUMBER_MODE_DATA_NETWORK) {
    msg.address.isNumberModeDataNetwork =  true;
  } else {
    msg.address.isNumberModeDataNetwork = false;
  }
  msg.address.numberType = static_cast<int32_t>(rilMsg.sAddress.number_type);
  msg.address.numberPlan = static_cast<int32_t>(rilMsg.sAddress.number_plan);

  int digitLimit = MIN((rilMsg.sAddress.number_of_digits), RIL_CDMA_SMS_ADDRESS_MAX);
  msg.address.digits.resize(digitLimit);
  for (int i = 0; i < digitLimit; i++) {
    msg.address.digits[i] = rilMsg.sAddress.digits[i];
  }

  msg.subAddress.subaddressType = static_cast<int32_t>(rilMsg.sSubAddress.subaddressType);
  msg.subAddress.odd = rilMsg.sSubAddress.odd;

  digitLimit = MIN((rilMsg.sSubAddress.number_of_digits), RIL_CDMA_SMS_SUBADDRESS_MAX);
  msg.subAddress.digits.resize(digitLimit);
  for (int i = 0; i < digitLimit; i++) {
    msg.subAddress.digits[i] = rilMsg.sSubAddress.digits[i];
  }

  digitLimit = MIN((rilMsg.uBearerDataLen), RIL_CDMA_SMS_BEARER_DATA_MAX);
  msg.bearerData.resize(digitLimit);
  for (int i = 0; i < digitLimit; i++) {
    msg.bearerData[i] = rilMsg.aBearerData[i];
  }
}

RIL_RadioTechnologyFamily convertAidlRadioTechToRil(aidlradio::RadioTechnologyFamily tech) {
    return tech == aidlradio::RadioTechnologyFamily::THREE_GPP
        ? RADIO_TECH_3GPP
        : RADIO_TECH_3GPP2;
}
