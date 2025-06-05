/*===========================================================================
 *
 *    Copyright (c) 2023 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/
#include "platform/android/SmsAidlUtil.h"

void convertGsmSmsAidlToRilPayload(const aidlmsg::SendSmsResult& sms, RIL_SMS_Response& data)
{
  data.messageRef = sms.messageRef;
  data.errorCode = sms.errorCode;
  if (sms.ackPDU.size() > 0) {
    data.ackPDU = (char*)malloc(sms.ackPDU.size());
    strlcpy(data.ackPDU, sms.ackPDU.c_str(), sms.ackPDU.size());
  } else {
    data.ackPDU = NULL;
  }
}

void convertGsmBcConfigToAidl(void* in,
                              size_t datalen,
                              std::vector<aidlmsg::GsmBroadcastSmsConfigInfo>& out)
{
  auto payload = static_cast<RIL_GSM_BroadcastSmsConfigInfo**>(in);
  out.resize(datalen / sizeof(const RIL_GSM_BroadcastSmsConfigInfo*));
  for (int i = 0; i < out.size(); i++) {
    out[i].fromServiceId = payload[i]->fromServiceId;
    out[i].toServiceId = payload[i]->toServiceId;
    out[i].fromCodeScheme = payload[i]->fromCodeScheme;
    out[i].toCodeScheme = payload[i]->toCodeScheme;
    out[i].selected = ((payload[i]->selected == 0) ? false : true);
  }
}

void convertGsmSmsPayloadToAidl(void* data, size_t datalen, aidlmsg::GsmSmsMessage& sms)
{
  char** payload = static_cast<char**>(data);
  sms.smscPdu = std::string(payload[0]);
  sms.pdu = std::string(payload[1]);
}

void convertCdmaSmsAckToAidl(void* in, size_t datalen, aidlmsg::CdmaSmsAck& out)
{
  RIL_CDMA_SMS_Ack* payload = static_cast<RIL_CDMA_SMS_Ack*>(in);
  out.smsCauseCode = payload->uSMSCauseCode;
  out.errorClass = convertCdmaSmsErrorClassToAidl(payload->uErrorClass);
}

bool convertCdmaSmsErrorClassToAidl(RIL_CDMA_SMS_ErrorClass in)
{
  // True = we recieved error, false = no error
  return (in == RIL_CDMA_SMS_ERROR);
}

void convertCdmaSmsWritePayloadToAidl(void* in, size_t datalen, aidlmsg::CdmaSmsWriteArgs& out)
{
  auto payload = static_cast<RIL_CDMA_SMS_WriteArgs*>(in);
  out.status = convertCdmaSmsWriteArgsStatusToAidl(payload->status);
  convertRilCdmaSmsToAidl(payload->message, out.message);
}

int32_t convertCdmaSmsWriteArgsStatusToAidl(int in)
{
  switch (in) {
    case 1:
      return aidlmsg::CdmaSmsWriteArgs::STATUS_REC_READ;
    case 2:
      return aidlmsg::CdmaSmsWriteArgs::STATUS_STO_UNSENT;
    case 3:
      return aidlmsg::CdmaSmsWriteArgs::STATUS_STO_SENT;
    case 0:
    default:
      return aidlmsg::CdmaSmsWriteArgs::STATUS_REC_UNREAD;
  }
}

void convertCdmaBcConfigToAidl(void* in,
                               size_t datalen,
                               std::vector<aidlmsg::CdmaBroadcastSmsConfigInfo>& out)
{
  auto payload = static_cast<RIL_CDMA_BroadcastSmsConfigInfo**>(in);
  out.resize(datalen / sizeof(const RIL_CDMA_BroadcastSmsConfigInfo*));
  for (int i = 0; i < out.size(); i++) {
    out[i].serviceCategory = payload[i]->service_category;
    out[i].language = payload[i]->language;
    out[i].selected = ((payload[i]->selected == 0) ? false : true);
  }
}

int32_t convertCdmaSmsDigitModeToAidl(RIL_CDMA_SMS_DigitMode in)
{
  switch (in) {
    case RIL_CDMA_SMS_DIGIT_MODE_8_BIT:
      return aidlmsg::CdmaSmsAddress::DIGIT_MODE_EIGHT_BIT;
    case RIL_CDMA_SMS_DIGIT_MODE_4_BIT:
    default:
      return aidlmsg::CdmaSmsAddress::DIGIT_MODE_FOUR_BIT;
  }
}

int32_t convertCdmaSmsNumberTypeToAidl(RIL_CDMA_SMS_NumberType in)
{
  switch (in) {
    case RIL_CDMA_SMS_NUMBER_TYPE_INTERNATIONAL_OR_DATA_IP:
      return aidlmsg::CdmaSmsAddress::NUMBER_TYPE_INTERNATIONAL_OR_DATA_IP;
    case RIL_CDMA_SMS_NUMBER_TYPE_NATIONAL_OR_INTERNET_MAIL:
      return aidlmsg::CdmaSmsAddress::NUMBER_TYPE_NATIONAL_OR_INTERNET_MAIL;
    case RIL_CDMA_SMS_NUMBER_TYPE_NETWORK:
      return aidlmsg::CdmaSmsAddress::NUMBER_TYPE_NETWORK;
    case RIL_CDMA_SMS_NUMBER_TYPE_SUBSCRIBER:
      return aidlmsg::CdmaSmsAddress::NUMBER_TYPE_SUBSCRIBER;
    case RIL_CDMA_SMS_NUMBER_TYPE_ALPHANUMERIC:
      return aidlmsg::CdmaSmsAddress::NUMBER_TYPE_ALPHANUMERIC;
    case RIL_CDMA_SMS_NUMBER_TYPE_ABBREVIATED:
      return aidlmsg::CdmaSmsAddress::NUMBER_TYPE_ABBREVIATED;
    case RIL_CDMA_SMS_NUMBER_TYPE_RESERVED_7:
      return aidlmsg::CdmaSmsAddress::NUMBER_TYPE_RESERVED_7;
    case RIL_CDMA_SMS_NUMBER_TYPE_UNKNOWN:
    default:
      return aidlmsg::CdmaSmsAddress::NUMBER_TYPE_UNKNOWN;
  }
}
int32_t convertCdmaSmsNumberPlanToAidl(RIL_CDMA_SMS_NumberPlan in)
{
  switch (in) {
    case RIL_CDMA_SMS_NUMBER_PLAN_TELEPHONY:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_TELEPHONY;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_2:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_2;
    case RIL_CDMA_SMS_NUMBER_PLAN_DATA:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_DATA;
    case RIL_CDMA_SMS_NUMBER_PLAN_TELEX:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_TELEX;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_5:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_5;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_6:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_6;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_7:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_7;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_8:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_8;
    case RIL_CDMA_SMS_NUMBER_PLAN_PRIVATE:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_PRIVATE;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_10:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_10;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_11:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_11;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_12:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_12;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_13:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_13;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_14:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_14;
    case RIL_CDMA_SMS_NUMBER_PLAN_RESERVED_15:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_RESERVED_15;
    case RIL_CDMA_SMS_NUMBER_PLAN_UNKNOWN:
    default:
      return aidlmsg::CdmaSmsAddress::NUMBER_PLAN_UNKNOWN;
  }
}

int32_t convertCdmaSmsSubAddrTypeToAidl(RIL_CDMA_SMS_SubaddressType in)
{
  switch (in) {
    case RIL_CDMA_SMS_SUBADDRESS_TYPE_USER_SPECIFIED:
      return aidlmsg::CdmaSmsSubaddress::SUBADDRESS_TYPE_USER_SPECIFIED;
    case RIL_CDMA_SMS_SUBADDRESS_TYPE_NSAP:
    default:
      return aidlmsg::CdmaSmsSubaddress::SUBADDRESS_TYPE_NSAP;
  }
}

int32_t convertSmsWriteArgsStatusToAidl(int in)
{
  switch (in) {
    case 1:
      return aidlmsg::SmsWriteArgs::STATUS_REC_READ;
    case 2:
      return aidlmsg::SmsWriteArgs::STATUS_STO_UNSENT;
    case 3:
      return aidlmsg::SmsWriteArgs::STATUS_STO_SENT;
    case 0:
    default:
      return aidlmsg::SmsWriteArgs::STATUS_REC_UNREAD;
  }
}

void convertGsmSmsWritePayloadToAidl(void* in, size_t datalen, aidlmsg::SmsWriteArgs& out)
{
  auto payload = static_cast<RIL_SMS_WriteArgs*>(in);
  out.smsc = payload->smsc;
  out.pdu = payload->pdu;
  out.status = convertSmsWriteArgsStatusToAidl(payload->status);
}

void convertRilCdmaSmsToAidl(RIL_CDMA_SMS_Message& in, aidlmsg::CdmaSmsMessage& out)
{
  out.teleserviceId = in.uTeleserviceID;
  out.isServicePresent = ((in.bIsServicePresent == 0) ? false : true);
  out.serviceCategory = in.uServicecategory;
  out.address.digitMode = convertCdmaSmsDigitModeToAidl(in.sAddress.digit_mode);
  out.address.isNumberModeDataNetwork = static_cast<int32_t>(in.sAddress.number_mode);
  out.address.numberType = convertCdmaSmsNumberTypeToAidl(in.sAddress.number_type);
  out.address.numberPlan = convertCdmaSmsNumberPlanToAidl(in.sAddress.number_plan);
  out.address.digits.resize(in.sAddress.number_of_digits);
  for (int i = 0; i < in.sAddress.number_of_digits; i++) {
    out.address.digits[i] = in.sAddress.digits[i];
  }

  out.subAddress.subaddressType = convertCdmaSmsSubAddrTypeToAidl(in.sSubAddress.subaddressType);
  out.subAddress.odd = ((in.sSubAddress.odd == 0) ? false : true);
  out.subAddress.digits.resize(in.sSubAddress.number_of_digits);
  for (int i = 0; i < in.sSubAddress.number_of_digits; i++) {
    out.subAddress.digits[i] = in.sSubAddress.digits[i];
  }

  out.bearerData.resize(in.uBearerDataLen);
  for (int i = 0; i < in.uBearerDataLen; i++) {
    out.bearerData[i] = in.aBearerData[i];
  }
}

void aidlToRilCdmaSmsMessage(RIL_CDMA_SMS_Message& rcsm, const aidlmsg::CdmaSmsMessage& sms)
{
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