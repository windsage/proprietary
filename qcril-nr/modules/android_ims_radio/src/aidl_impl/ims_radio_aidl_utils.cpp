/******************************************************************************
  @file    ims_radio_aidl_utils.cpp
  @brief   ims_radio_aidl_utils

  DESCRIPTION
    Implements the utils to be used by the ImsRadioAidlImpl class

  ---------------------------------------------------------------------------

  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define TAG "RILQ"

#define HIGHER_4BIT(data) (((data)&0xf0) >> 4)
#define LOWER_4BIT(data) ((data)&0x0f)
#define INVALID_HEX_CHAR 0

#include "ims_radio_aidl_utils.h"
#include <string.h>
#include <framework/Log.h>
#include "utils_common.h"
#include "qcril_config.h"

#include <aidl/vendor/qti/hardware/radio/ims/MsimAdditionalCallInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/MsimAdditionalInfoCode.h>
#include <aidl/vendor/qti/hardware/radio/ims/SrtpEncryptionCategories.h>

using qcril::interfaces::EmergencyCallRouting;
using qcril::interfaces::RIL_EccCategoryMask;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace utils {

static inline char IntToHexChar(uint8_t byte_data) {
  if (byte_data >= 0 && byte_data <= 9) return ('0' + byte_data);
  if (byte_data > 9 && byte_data <= 15) return ('a' + byte_data - 10);
  return INVALID_HEX_CHAR;
}

static inline uint32_t revertEndian(uint32_t data) {
  int32_t result = (data & 0xff000000) >> 24;

  result |= ((data & 0x00ff0000) >> 8);
  result |= ((data & 0x0000ff00) << 8);
  result |= ((data & 0x000000ff) << 24);

  return result;
}

static uint32_t convertBigEndianToCurrent(uint32_t data) {
  if (!IS_BIG_ENDIAN())
    return revertEndian(data);
  else
    return data;
}

static uint32_t convertCurrentToBigEndian(uint32_t data) {
  if (!IS_BIG_ENDIAN())
    return revertEndian(data);
  else
    return data;
}

static void getBigEndianArrayFrom32(uint8_t* array32, uint32_t value) {
  if (!array32) return;

  array32[0] = (value & 0xff000000) >> 24;
  array32[1] = (value & 0x00ff0000) >> 16;
  array32[2] = (value & 0x0000ff00) >> 8;
  array32[3] = (value & 0x000000ff);
}

static uint8_t convertDtmfToAscii(uint8_t dtmfDigit) {
  uint8_t asciiDigit;

  switch (dtmfDigit) {
    case 0:
      asciiDigit = 'D';
      break;
    case 1:
      asciiDigit = '1';
      break;
    case 2:
      asciiDigit = '2';
      break;
    case 3:
      asciiDigit = '3';
      break;
    case 4:
      asciiDigit = '4';
      break;
    case 5:
      asciiDigit = '5';
      break;
    case 6:
      asciiDigit = '6';
      break;
    case 7:
      asciiDigit = '7';
      break;
    case 8:
      asciiDigit = '8';
      break;
    case 9:
      asciiDigit = '9';
      break;
    case 10:
      asciiDigit = '0';
      break;
    case 11:
      asciiDigit = '*';
      break;
    case 12:
      asciiDigit = '#';
      break;
    case 13:
      asciiDigit = 'A';
      break;
    case 14:
      asciiDigit = 'B';
      break;
    case 15:
      asciiDigit = 'C';
      break;
    default:
      asciiDigit = 32;  // Invalid DTMF code
      break;
  }

  return asciiDigit;
}

bool convertBytesToHexString(const uint8_t* bytePayload, size_t size, std::string& str) {
  std::string out;
  for (unsigned int byte_idx = 0; byte_idx < size; byte_idx++) {
    char tmp;
    // higher bit is in the earlier index
    if ((tmp = IntToHexChar(HIGHER_4BIT(bytePayload[byte_idx]))) == INVALID_HEX_CHAR) {
      return false;
    }
    out.push_back(tmp);
    if ((tmp = IntToHexChar(LOWER_4BIT(bytePayload[byte_idx]))) == INVALID_HEX_CHAR) {
      return false;
    }
    out.push_back(tmp);
  }
  str = std::move(out);
  return true;
}

bool convertPayloadToCdmaFormat(const uint8_t* payload, size_t size, RIL_CDMA_SMS_Message& cdmaMsg) {
  if (!payload || !size) return false;
  // XXX take care alignment!
  // How to implement data stream in c++?
  uint32_t index = 0;
  // read the first 3 int
  index += sizeof(int) * 3;
  if (index > size) return false;
  auto int_buf = reinterpret_cast<const int*>(payload);
  cdmaMsg.uTeleserviceID = convertBigEndianToCurrent(int_buf[0]);
  cdmaMsg.bIsServicePresent = (convertBigEndianToCurrent(int_buf[1]) == 0) ? 0 : 1;
  cdmaMsg.uServicecategory = convertBigEndianToCurrent(int_buf[2]);

  // read bytes to fill sAddress
  if (index + 5 > size) return false;
  cdmaMsg.sAddress.digit_mode = static_cast<RIL_CDMA_SMS_DigitMode>(payload[index++]);
  cdmaMsg.sAddress.number_mode = static_cast<RIL_CDMA_SMS_NumberMode>(payload[index++]);
  cdmaMsg.sAddress.number_type = static_cast<RIL_CDMA_SMS_NumberType>(payload[index++]);
  cdmaMsg.sAddress.number_plan = static_cast<RIL_CDMA_SMS_NumberPlan>(payload[index++]);
  cdmaMsg.sAddress.number_of_digits = payload[index++];
  if ((cdmaMsg.sAddress.number_of_digits > sizeof(cdmaMsg.sAddress.digits)) ||
      (index + cdmaMsg.sAddress.number_of_digits > size)) {
      return false;
  }
  memcpy(cdmaMsg.sAddress.digits, payload + index, cdmaMsg.sAddress.number_of_digits);
  index += cdmaMsg.sAddress.number_of_digits;

  // read bytes to fill sSubAddress
  if (index + 3 > size) return false;
  cdmaMsg.sSubAddress.subaddressType = static_cast<RIL_CDMA_SMS_SubaddressType>(payload[index++]);
  cdmaMsg.sSubAddress.odd = (payload[index++] == 0) ? 0 : 1;
  cdmaMsg.sSubAddress.number_of_digits = payload[index++];
  if ((cdmaMsg.sSubAddress.number_of_digits > sizeof(cdmaMsg.sSubAddress.digits)) ||
      (index + cdmaMsg.sSubAddress.number_of_digits > size)) {
      return false;
  }
  memcpy(cdmaMsg.sSubAddress.digits, payload + index, cdmaMsg.sSubAddress.number_of_digits);
  index += cdmaMsg.sSubAddress.number_of_digits;

  // read bytes for bear data
  if (index + 1 > size) return false;
  cdmaMsg.uBearerDataLen = payload[index++];
  if ((cdmaMsg.uBearerDataLen > sizeof(cdmaMsg.aBearerData)) ||
      (index + cdmaMsg.uBearerDataLen > size)) {
      return false;
  }
  memcpy(cdmaMsg.aBearerData, payload + index, cdmaMsg.uBearerDataLen);

  QCRIL_LOG_INFO("payload size = %d, read size = %d", size, index + cdmaMsg.uBearerDataLen);

  return true;
}

void convertRilImsResponseToAidlResponse(RIL_Errno rilErr, RIL_RadioTechnologyFamily tech,
                                         std::optional<bool> bLteOnlyReg,
                                         aidlimports::SmsSendStatus& statusReport,
                                         aidlimports::SmsSendFailureReason& failureReason) {
  statusReport = aidlimports::SmsSendStatus::ERROR;
  switch (rilErr) {
    case RIL_E_SUCCESS:
      statusReport = aidlimports::SmsSendStatus::OK;
      failureReason = aidlimports::SmsSendFailureReason::NONE;
      break;
    case RIL_E_SMS_SEND_FAIL_RETRY: {
      bool lteOnly = (bLteOnlyReg ? *bLteOnlyReg : false);
      statusReport = ((tech == RADIO_TECH_3GPP) && lteOnly)
                         ? aidlimports::SmsSendStatus::ERROR_RETRY
                         : aidlimports::SmsSendStatus::ERROR_FALLBACK;
      failureReason = aidlimports::SmsSendFailureReason::GENERIC_FAILURE;
    } break;
    case RIL_E_NO_NETWORK_FOUND:
      failureReason = aidlimports::SmsSendFailureReason::NO_SERVICE;
      break;
    case RIL_E_FDN_CHECK_FAILURE:
      failureReason = aidlimports::SmsSendFailureReason::FDN_CHECK_FAILURE;
      break;
    case RIL_E_RADIO_NOT_AVAILABLE:
      failureReason = aidlimports::SmsSendFailureReason::RADIO_NOT_AVAILABLE;
      break;
    case RIL_E_NETWORK_REJECT:
      failureReason = aidlimports::SmsSendFailureReason::NETWORK_REJECT;
      break;
    case RIL_E_INVALID_ARGUMENTS:
      failureReason = aidlimports::SmsSendFailureReason::INVALID_ARGUMENTS;
      break;
    case RIL_E_INVALID_STATE:
      failureReason = aidlimports::SmsSendFailureReason::INVALID_STATE;
      break;
    case RIL_E_NO_MEMORY:
      failureReason = aidlimports::SmsSendFailureReason::NO_MEMORY;
      break;
    case RIL_E_INVALID_SMS_FORMAT:
      failureReason = aidlimports::SmsSendFailureReason::INVALID_SMS_FORMAT;
      break;
    case RIL_E_SYSTEM_ERR:
      failureReason = aidlimports::SmsSendFailureReason::SYSTEM_ERROR;
      break;
    case RIL_E_MODEM_ERR:
      failureReason = aidlimports::SmsSendFailureReason::MODEM_ERROR;
      break;
    case RIL_E_NETWORK_ERR:
      failureReason = aidlimports::SmsSendFailureReason::NETWORK_ERROR;
      break;
    case RIL_E_ENCODING_ERR:
      failureReason = aidlimports::SmsSendFailureReason::ENCODING_ERROR;
      break;
    case RIL_E_INVALID_SMSC_ADDRESS:
      failureReason = aidlimports::SmsSendFailureReason::INVALID_SMSC_ADDRESS;
      break;
    case RIL_E_OPERATION_NOT_ALLOWED:
      failureReason = aidlimports::SmsSendFailureReason::OPERATION_NOT_ALLOWED;
      break;
    case RIL_E_INTERNAL_ERR:
      failureReason = aidlimports::SmsSendFailureReason::INTERNAL_ERROR;
      break;
    case RIL_E_NO_RESOURCES:
      failureReason = aidlimports::SmsSendFailureReason::NO_RESOURCES;
      break;
    case RIL_E_CANCELLED:
      failureReason = aidlimports::SmsSendFailureReason::CANCELLED;
      break;
    case RIL_E_REQUEST_NOT_SUPPORTED:
      failureReason = aidlimports::SmsSendFailureReason::REQUEST_NOT_SUPPORTED;
      break;
    default:
      failureReason = aidlimports::SmsSendFailureReason::GENERIC_FAILURE;
      break;
  }
}

bool convertCdmaFormatToPseudoPdu(RIL_CDMA_SMS_Message& cdmaMsg, std::vector<uint8_t>& payload) {
  const uint32_t MESSAGE_TYPE_POINT_TO_POINT = 0x00;
  const uint32_t MESSAGE_TYPE_BROADCAST = 0x01;
  const uint32_t MESSAGE_TYPE_ACKNOWLEDGE = 0x02;
  uint32_t messageType;

  if (cdmaMsg.bIsServicePresent) {
    messageType = MESSAGE_TYPE_BROADCAST;
  } else {
    if (cdmaMsg.uTeleserviceID == 0x00) {
      messageType = MESSAGE_TYPE_ACKNOWLEDGE;
    } else {
      messageType = MESSAGE_TYPE_POINT_TO_POINT;
    }
  }
  // fill the first three int
  uint32_t item = convertCurrentToBigEndian(messageType);
  uint8_t* base = reinterpret_cast<uint8_t*>(&item);
  payload.insert(payload.end(), base, base + 4);

  item = convertCurrentToBigEndian(cdmaMsg.uTeleserviceID);
  payload.insert(payload.end(), base, base + 4);

  item = convertCurrentToBigEndian(cdmaMsg.uServicecategory);
  payload.insert(payload.end(), base, base + 4);

  // fill the sAddress
  payload.push_back(cdmaMsg.sAddress.digit_mode);
  payload.push_back(cdmaMsg.sAddress.number_mode);
  payload.push_back(cdmaMsg.sAddress.number_type);
  payload.push_back(cdmaMsg.sAddress.number_plan);
  payload.push_back(cdmaMsg.sAddress.number_of_digits);
  // 4-bit DTMF
  if (cdmaMsg.sAddress.digit_mode == 0) {
    for (int index = 0; index < cdmaMsg.sAddress.number_of_digits; index++)
      payload.push_back(convertDtmfToAscii(cdmaMsg.sAddress.digits[index]));
  } else {
    payload.insert(payload.end(), cdmaMsg.sAddress.digits,
                   cdmaMsg.sAddress.digits + cdmaMsg.sAddress.number_of_digits);
  }

  uint8_t array32[4];
  // bearerReply
  getBigEndianArrayFrom32(array32, 1);
  payload.insert(payload.end(), array32, array32 + 4);
  // replySeqNo (not supported)
  payload.push_back(0);
  // errorClass (not supported)
  payload.push_back(0);
  // causeCode (not supported)
  payload.push_back(0);

  // bear Data length
  getBigEndianArrayFrom32(array32, cdmaMsg.uBearerDataLen);
  payload.insert(payload.end(), array32, array32 + 4);
  payload.insert(payload.end(), cdmaMsg.aBearerData, cdmaMsg.aBearerData + cdmaMsg.uBearerDataLen);

  return true;
}

aidlimports::VerificationStatus convertToAidlVerificationStatus(
    const qcril::interfaces::VerificationStatus& in) {
  switch (in) {
    case qcril::interfaces::VerificationStatus::PASS:
      return aidlimports::VerificationStatus::VALIDATION_PASS;
    case qcril::interfaces::VerificationStatus::FAIL:
      return aidlimports::VerificationStatus::VALIDATION_FAIL;
    default:
      return aidlimports::VerificationStatus::VALIDATION_NONE;
  }
}

void convertRilImsSubConfigInfoToAidlResponse(aidlimports::ImsSubConfigInfo& subConfigInfo,
                                              qcril::interfaces::ImsSubConfigInfo& in) {
  if (in.hasSimultStackCount()) {
    subConfigInfo.simultStackCount = in.getSimultStackCount();
  } else {
    subConfigInfo.simultStackCount = INT32_MAX;
  }
  subConfigInfo.imsStackEnabled = in.getImsStackEnabled();
}

/**
 * Maps the error code
 */
aidlimports::ErrorCode convertRilErrorToAidlImsError(RIL_Errno rilError) {
  aidlimports::ErrorCode ret;

  switch (rilError) {
    case RIL_E_SUCCESS:
      ret = aidlimports::ErrorCode::SUCCESS;
      break;

    case RIL_E_RADIO_NOT_AVAILABLE:
      ret = aidlimports::ErrorCode::RADIO_NOT_AVAILABLE;
      break;

    case RIL_E_GENERIC_FAILURE:
      ret = aidlimports::ErrorCode::GENERIC_FAILURE;
      break;

    case RIL_E_PASSWORD_INCORRECT:
      ret = aidlimports::ErrorCode::PASSWORD_INCORRECT;
      break;

    case RIL_E_REQUEST_NOT_SUPPORTED:
      ret = aidlimports::ErrorCode::REQUEST_NOT_SUPPORTED;
      break;

    case RIL_E_CANCELLED:
      ret = aidlimports::ErrorCode::CANCELLED;
      break;

    case RIL_E_HOLD_RESUME_FAILED:
      ret = aidlimports::ErrorCode::HOLD_RESUME_FAILED;
      break;

    case RIL_E_HOLD_RESUME_CANCELED:
      ret = aidlimports::ErrorCode::HOLD_RESUME_CANCELED;
      break;
    case RIL_E_REINVITE_COLLISION:
      ret = aidlimports::ErrorCode::REINVITE_COLLISION;
      break;

    case RIL_E_FDN_CHECK_FAILURE:
      ret = aidlimports::ErrorCode::FDN_CHECK_FAILURE;
      break;

    case RIL_E_DIAL_MODIFIED_TO_DIAL:
      ret = aidlimports::ErrorCode::DIAL_MODIFIED_TO_DIAL;
      break;

    case RIL_E_DIAL_MODIFIED_TO_DIAL_VIDEO:
      ret = aidlimports::ErrorCode::DIAL_MODIFIED_TO_DIAL_VIDEO;
      break;

    case RIL_E_DIAL_MODIFIED_TO_SS:
      ret = aidlimports::ErrorCode::DIAL_MODIFIED_TO_SS;
      break;

    case RIL_E_DIAL_MODIFIED_TO_USSD:
      ret = aidlimports::ErrorCode::DIAL_MODIFIED_TO_USSD;
      break;

    case RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL:
      ret = aidlimports::ErrorCode::DIAL_VIDEO_MODIFIED_TO_DIAL;
      break;

    case RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
      ret = aidlimports::ErrorCode::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
      break;

    case RIL_E_DIAL_VIDEO_MODIFIED_TO_SS:
      ret = aidlimports::ErrorCode::DIAL_VIDEO_MODIFIED_TO_SS;
      break;

    case RIL_E_DIAL_VIDEO_MODIFIED_TO_USSD:
      ret = aidlimports::ErrorCode::DIAL_VIDEO_MODIFIED_TO_USSD;
      break;

    case RIL_E_SS_MODIFIED_TO_DIAL:
      ret = aidlimports::ErrorCode::SS_MODIFIED_TO_DIAL;
      break;

    case RIL_E_SS_MODIFIED_TO_DIAL_VIDEO:
      ret = aidlimports::ErrorCode::SS_MODIFIED_TO_DIAL_VIDEO;
      break;

    case RIL_E_SS_MODIFIED_TO_SS:
      ret = aidlimports::ErrorCode::SS_MODIFIED_TO_SS;
      break;

    case RIL_E_SS_MODIFIED_TO_USSD:
      ret = aidlimports::ErrorCode::SS_MODIFIED_TO_USSD;
      break;

    case RIL_E_REJECTED_BY_REMOTE:
      ret = aidlimports::ErrorCode::REJECTED_BY_REMOTE;
      break;

    case RIL_E_NETWORK_NOT_SUPPORTED:
      ret = aidlimports::ErrorCode::NETWORK_NOT_SUPPORTED;
      break;

    case RIL_E_INVALID_ARGUMENTS:
      ret = aidlimports::ErrorCode::INVALID_PARAMETER;
      break;

    case RIL_E_USSD_CS_FALLBACK:
      ret = aidlimports::ErrorCode::USSD_CS_FALLBACK;
      break;

    case RIL_E_CF_SERVICE_NOT_REGISTERED:
      ret = aidlimports::ErrorCode::CF_SERVICE_NOT_REGISTERED;
      break;

    case RIL_E_INTERNAL_NO_MEMORY:
      ret = aidlimports::ErrorCode::RIL_INTERNAL_NO_MEMORY;
      break;

    case RIL_E_INTERNAL_INVALID_ARGUMENTS:
      ret = aidlimports::ErrorCode::RIL_INTERNAL_INVALID_ARGUMENTS;
      break;

    case RIL_E_INTERNAL_INVALID_STATE:
      ret = aidlimports::ErrorCode::RIL_INTERNAL_INVALID_STATE;
      break;

    case RIL_E_INTERNAL_GENERIC_FAILURE:
      ret = aidlimports::ErrorCode::RIL_INTERNAL_GENERIC_FAILURE;
      break;

    default:
      ret = aidlimports::ErrorCode::GENERIC_FAILURE;
  }

  QCRIL_LOG_INFO("ril error %d mapped to aidl error %d", rilError, ret);
  return ret;
}  // convertRilErrorToAidlImsError

aidlimports::ErrorCode convertDispatchStatusToAidlErrorCode(Message::Callback::Status status) {
  aidlimports::ErrorCode ret;

  switch (status) {
    case Message::Callback::Status::SUCCESS:
      ret = aidlimports::ErrorCode::SUCCESS;
      break;

    case Message::Callback::Status::NO_HANDLER_FOUND:
      ret = aidlimports::ErrorCode::REQUEST_NOT_SUPPORTED;
      break;

    case Message::Callback::Status::CANCELLED:
    case Message::Callback::Status::TIMEOUT:
    case Message::Callback::Status::FAILURE:
    case Message::Callback::Status::BROADCAST_NOT_ALLOWED:
    default:
      ret = aidlimports::ErrorCode::GENERIC_FAILURE;
  }

  QCRIL_LOG_INFO("dispatch message status %d mapped to aidl error %d", status, ret);
  return ret;
}  // convertDispatchStatusToAidlErrorCode

aidlimports::RadioState convertToAidlRadioState(qcril::interfaces::RadioState inState) {
  switch (inState) {
    case qcril::interfaces::RadioState::STATE_OFF:
      return aidlimports::RadioState::OFF;
    case qcril::interfaces::RadioState::STATE_ON:
      return aidlimports::RadioState::ON;
    case qcril::interfaces::RadioState::STATE_UNAVAILABLE:
      return aidlimports::RadioState::UNAVAILABLE;
    default:
      return aidlimports::RadioState::INVALID;
  }
  return aidlimports::RadioState::INVALID;
}  // convertToAidlRadioState

aidlimports::RegState convertToAidlRegState(const qcril::interfaces::RegState& in) {
  switch (in) {
    case qcril::interfaces::RegState::REGISTERED:
      return aidlimports::RegState::REGISTERED;
    case qcril::interfaces::RegState::NOT_REGISTERED:
      return aidlimports::RegState::NOT_REGISTERED;
    case qcril::interfaces::RegState::REGISTERING:
      return aidlimports::RegState::REGISTERING;
    case qcril::interfaces::RegState::UNKNOWN:
    default:
      return aidlimports::RegState::INVALID;
  }
}  // convertRegState

qcril::interfaces::RegState convertToRilRegState(const aidlimports::RegState& in) {
  switch (in) {
    case aidlimports::RegState::REGISTERED:
      return qcril::interfaces::RegState::REGISTERED;
    case aidlimports::RegState::NOT_REGISTERED:
      return qcril::interfaces::RegState::NOT_REGISTERED;
    case aidlimports::RegState::REGISTERING:
      return qcril::interfaces::RegState::REGISTERING;
    case aidlimports::RegState::INVALID:
    default:
      return qcril::interfaces::RegState::UNKNOWN;
  }
}

RIL_RadioTechnology convertToRilRadioTech(const aidlimports::RadioTechType& in) {
  switch (in) {
    case aidlimports::RadioTechType::ANY:
      return RADIO_TECH_ANY;
    case aidlimports::RadioTechType::UNKNOWN:
      return RADIO_TECH_UNKNOWN;
    case aidlimports::RadioTechType::GPRS:
      return RADIO_TECH_GPRS;
    case aidlimports::RadioTechType::EDGE:
      return RADIO_TECH_EDGE;
    case aidlimports::RadioTechType::UMTS:
      return RADIO_TECH_UMTS;
    case aidlimports::RadioTechType::IS95A:
      return RADIO_TECH_IS95A;
    case aidlimports::RadioTechType::IS95B:
      return RADIO_TECH_IS95B;
    case aidlimports::RadioTechType::RTT_1X:
      return RADIO_TECH_1xRTT;
    case aidlimports::RadioTechType::EVDO_0:
      return RADIO_TECH_EVDO_0;
    case aidlimports::RadioTechType::EVDO_A:
      return RADIO_TECH_EVDO_A;
    case aidlimports::RadioTechType::HSDPA:
      return RADIO_TECH_HSDPA;
    case aidlimports::RadioTechType::HSUPA:
      return RADIO_TECH_HSUPA;
    case aidlimports::RadioTechType::HSPA:
      return RADIO_TECH_HSPA;
    case aidlimports::RadioTechType::EVDO_B:
      return RADIO_TECH_EVDO_B;
    case aidlimports::RadioTechType::EHRPD:
      return RADIO_TECH_EHRPD;
    case aidlimports::RadioTechType::LTE:
      return RADIO_TECH_LTE;
    case aidlimports::RadioTechType::HSPAP:
      return RADIO_TECH_HSPAP;
    case aidlimports::RadioTechType::GSM:
      return RADIO_TECH_GSM;
    case aidlimports::RadioTechType::TD_SCDMA:
      return RADIO_TECH_TD_SCDMA;
    case aidlimports::RadioTechType::WIFI:
      return RADIO_TECH_WIFI;
    case aidlimports::RadioTechType::IWLAN:
      return RADIO_TECH_IWLAN;
    case aidlimports::RadioTechType::NR5G:
      return RADIO_TECH_5G;
    case aidlimports::RadioTechType::C_IWLAN:
      return RADIO_TECH_C_IWLAN;
    case aidlimports::RadioTechType::INVALID:
    default:
      return RADIO_TECH_UNKNOWN;
  }
}

aidlimports::RadioTechType convertToAidlRadioTech(const RIL_RadioTechnology& in) {
  switch (in) {
    case RADIO_TECH_ANY:
      return aidlimports::RadioTechType::ANY;
    case RADIO_TECH_UNKNOWN:
      return aidlimports::RadioTechType::UNKNOWN;
    case RADIO_TECH_GPRS:
      return aidlimports::RadioTechType::GPRS;
    case RADIO_TECH_EDGE:
      return aidlimports::RadioTechType::EDGE;
    case RADIO_TECH_UMTS:
      return aidlimports::RadioTechType::UMTS;
    case RADIO_TECH_IS95A:
      return aidlimports::RadioTechType::IS95A;
    case RADIO_TECH_IS95B:
      return aidlimports::RadioTechType::IS95B;
    case RADIO_TECH_1xRTT:
      return aidlimports::RadioTechType::RTT_1X;
    case RADIO_TECH_EVDO_0:
      return aidlimports::RadioTechType::EVDO_0;
    case RADIO_TECH_EVDO_A:
      return aidlimports::RadioTechType::EVDO_A;
    case RADIO_TECH_HSDPA:
      return aidlimports::RadioTechType::HSDPA;
    case RADIO_TECH_HSUPA:
      return aidlimports::RadioTechType::HSUPA;
    case RADIO_TECH_HSPA:
      return aidlimports::RadioTechType::HSPA;
    case RADIO_TECH_EVDO_B:
      return aidlimports::RadioTechType::EVDO_B;
    case RADIO_TECH_EHRPD:
      return aidlimports::RadioTechType::EHRPD;
    case RADIO_TECH_LTE:
      return aidlimports::RadioTechType::LTE;
    case RADIO_TECH_HSPAP:
      return aidlimports::RadioTechType::HSPAP;
    case RADIO_TECH_GSM:
      return aidlimports::RadioTechType::GSM;
    case RADIO_TECH_TD_SCDMA:
      return aidlimports::RadioTechType::TD_SCDMA;
    case RADIO_TECH_WIFI:
      return aidlimports::RadioTechType::WIFI;
    case RADIO_TECH_IWLAN:
      return aidlimports::RadioTechType::IWLAN;
    case RADIO_TECH_5G:
      return aidlimports::RadioTechType::NR5G;
    case RADIO_TECH_C_IWLAN:
      return aidlimports::RadioTechType::C_IWLAN;
    default:
      return aidlimports::RadioTechType::INVALID;
  }
}  // convertToAidlRadioTech

bool convertRilRegistrationToAidlResponse(aidlimports::RegistrationInfo& out,
                                          const qcril::interfaces::Registration& in) {
  if (in.hasState()) {
    out.state = convertToAidlRegState(in.getState());
  } else {
    out.state = aidlimports::RegState::INVALID;
  }
  if (in.hasErrorCode()) {
    out.errorCode = in.getErrorCode();
  } else {
    out.errorCode = INT32_MAX;
  }
  if (in.hasErrorMessage()) {
    out.errorMessage = in.getErrorMessage();
  }
  if (in.hasRadioTechnology()) {
    out.radioTech = convertToAidlRadioTech(in.getRadioTechnology());
  } else {
    out.radioTech = aidlimports::RadioTechType::INVALID;
  }
  if (in.hasPAssociatedUris()) {
    out.pAssociatedUris = in.getPAssociatedUris();
  }
  return true;
}  // convertRilRegistrationtoAidlResponse

aidlimports::CallType convertToAidlCallType(const qcril::interfaces::CallType& in) {
  switch (in) {
    case qcril::interfaces::CallType::USSD:
      return aidlimports::CallType::USSD;
    case qcril::interfaces::CallType::CALLCOMPOSER:
      return aidlimports::CallType::CALLCOMPOSER;
    case qcril::interfaces::CallType::DC:
      return aidlimports::CallType::DC;
    case qcril::interfaces::CallType::VOICE:
      return aidlimports::CallType::VOICE;
    case qcril::interfaces::CallType::VT_TX:
      return aidlimports::CallType::VT_TX;
    case qcril::interfaces::CallType::VT_RX:
      return aidlimports::CallType::VT_RX;
    case qcril::interfaces::CallType::VT:
      return aidlimports::CallType::VT;
    case qcril::interfaces::CallType::VT_NODIR:
      return aidlimports::CallType::VT_NODIR;
    case qcril::interfaces::CallType::SMS:
      return aidlimports::CallType::SMS;
    case qcril::interfaces::CallType::UT:
      return aidlimports::CallType::UT;
    case qcril::interfaces::CallType::UNKNOWN:
    default:
      return aidlimports::CallType::UNKNOWN;
  }
}  // convertToAidlCallType

qcril::interfaces::CallType convertToRilCallType(const aidlimports::CallType& in) {
  switch (in) {
    case aidlimports::CallType::USSD:
      return qcril::interfaces::CallType::USSD;
    case aidlimports::CallType::CALLCOMPOSER:
      return qcril::interfaces::CallType::CALLCOMPOSER;
    case aidlimports::CallType::DC:
      return qcril::interfaces::CallType::DC;
    case aidlimports::CallType::VOICE:
      return qcril::interfaces::CallType::VOICE;
    case aidlimports::CallType::VT_TX:
      return qcril::interfaces::CallType::VT_TX;
    case aidlimports::CallType::VT_RX:
      return qcril::interfaces::CallType::VT_RX;
    case aidlimports::CallType::VT:
      return qcril::interfaces::CallType::VT;
    case aidlimports::CallType::VT_NODIR:
      return qcril::interfaces::CallType::VT_NODIR;
    case aidlimports::CallType::SMS:
      return qcril::interfaces::CallType::SMS;
    case aidlimports::CallType::UT:
      return qcril::interfaces::CallType::UT;
    case aidlimports::CallType::CS_VS_TX:
    case aidlimports::CallType::CS_VS_RX:
    case aidlimports::CallType::PS_VS_TX:
    case aidlimports::CallType::PS_VS_RX:
    case aidlimports::CallType::UNKNOWN:
    default:
      return qcril::interfaces::CallType::UNKNOWN;
  }
}

int32_t convertToRilClirMode(const aidlimports::ClirMode& in) {
  switch (in) {
    case aidlimports::ClirMode::INVOCATION:
      return static_cast<int32_t>(qcril::interfaces::ClirAction::INVOCATION);
    case aidlimports::ClirMode::SUPRESSION:
      return static_cast<int32_t>(qcril::interfaces::ClirAction::SUPPRESSION);
    case aidlimports::ClirMode::DEFAULT:
    default:
      return static_cast<int32_t>(qcril::interfaces::ClirAction::UNKNOWN);
  }
  return 0;
}

aidlimports::IpPresentation convertToAidlIpPresentation(const qcril::interfaces::Presentation& in) {
  switch (in) {
    case qcril::interfaces::Presentation::ALLOWED:
      return aidlimports::IpPresentation::NUM_ALLOWED;
    case qcril::interfaces::Presentation::RESTRICTED:
      return aidlimports::IpPresentation::NUM_RESTRICTED;
    case qcril::interfaces::Presentation::DEFAULT:
      return aidlimports::IpPresentation::NUM_DEFAULT;
    default:
      return aidlimports::IpPresentation::INVALID;
  }
}

qcril::interfaces::Presentation convertToRilIpPresentation(const aidlimports::IpPresentation& in) {
  switch (in) {
    case aidlimports::IpPresentation::NUM_ALLOWED:
      return qcril::interfaces::Presentation::ALLOWED;
    case aidlimports::IpPresentation::NUM_RESTRICTED:
      return qcril::interfaces::Presentation::RESTRICTED;
    case aidlimports::IpPresentation::NUM_DEFAULT:
    default:
      return qcril::interfaces::Presentation::DEFAULT;
  }
}

aidlimports::RttMode convertToAidlRttMode(const qcril::interfaces::RttMode& in) {
  switch (in) {
    case qcril::interfaces::RttMode::DISABLED:
      return aidlimports::RttMode::DISABLED;
    case qcril::interfaces::RttMode::FULL:
      return aidlimports::RttMode::FULL;
    case qcril::interfaces::RttMode::UNKNOWN:
    default:
      return aidlimports::RttMode::INVALID;
  }
}  // convertToAidlRttMode

qcril::interfaces::RttMode convertToRilRttMode(const aidlimports::RttMode& in) {
  switch (in) {
    case aidlimports::RttMode::DISABLED:
      return qcril::interfaces::RttMode::DISABLED;
    case aidlimports::RttMode::FULL:
      return qcril::interfaces::RttMode::FULL;
    case aidlimports::RttMode::INVALID:
    default:
      return qcril::interfaces::RttMode::UNKNOWN;
  }
}

aidlimports::CallFailCause convertToAidlCallFailCause(const qcril::interfaces::CallFailCause& in) {
  switch (in) {
    case qcril::interfaces::CallFailCause::REJECTED_ELSEWHERE:
      return aidlimports::CallFailCause::REJECTED_ELSEWHERE;
    case qcril::interfaces::CallFailCause::USER_REJECTED_SESSION_MODIFICATION:
      return aidlimports::CallFailCause::USER_REJECTED_SESSION_MODIFICATION;
    case qcril::interfaces::CallFailCause::USER_CANCELLED_SESSION_MODIFICATION:
      return aidlimports::CallFailCause::USER_CANCELLED_SESSION_MODIFICATION;
    case qcril::interfaces::CallFailCause::SESSION_MODIFICATION_FAILED:
      return aidlimports::CallFailCause::SESSION_MODIFICATION_FAILED;
    case qcril::interfaces::CallFailCause::SIP_METHOD_NOT_ALLOWED:
      return aidlimports::CallFailCause::SIP_METHOD_NOT_ALLOWED;
    case qcril::interfaces::CallFailCause::SIP_PROXY_AUTHENTICATION_REQUIRED:
      return aidlimports::CallFailCause::SIP_PROXY_AUTHENTICATION_REQUIRED;
    case qcril::interfaces::CallFailCause::SIP_REQUEST_ENTITY_TOO_LARGE:
      return aidlimports::CallFailCause::SIP_REQUEST_ENTITY_TOO_LARGE;
    case qcril::interfaces::CallFailCause::SIP_REQUEST_URI_TOO_LARGE:
      return aidlimports::CallFailCause::SIP_REQUEST_URI_TOO_LARGE;
    case qcril::interfaces::CallFailCause::SIP_EXTENSION_REQUIRED:
      return aidlimports::CallFailCause::SIP_EXTENSION_REQUIRED;
    case qcril::interfaces::CallFailCause::SIP_INTERVAL_TOO_BRIEF:
      return aidlimports::CallFailCause::SIP_INTERVAL_TOO_BRIEF;
    case qcril::interfaces::CallFailCause::SIP_CALL_OR_TRANS_DOES_NOT_EXIST:
      return aidlimports::CallFailCause::SIP_CALL_OR_TRANS_DOES_NOT_EXIST;
    case qcril::interfaces::CallFailCause::SIP_LOOP_DETECTED:
      return aidlimports::CallFailCause::SIP_LOOP_DETECTED;
    case qcril::interfaces::CallFailCause::SIP_TOO_MANY_HOPS:
      return aidlimports::CallFailCause::SIP_TOO_MANY_HOPS;
    case qcril::interfaces::CallFailCause::SIP_AMBIGUOUS:
      return aidlimports::CallFailCause::SIP_AMBIGUOUS;
    case qcril::interfaces::CallFailCause::SIP_REQUEST_PENDING:
      return aidlimports::CallFailCause::SIP_REQUEST_PENDING;
    case qcril::interfaces::CallFailCause::SIP_UNDECIPHERABLE:
      return aidlimports::CallFailCause::SIP_UNDECIPHERABLE;
    case qcril::interfaces::CallFailCause::RETRY_ON_IMS_WITHOUT_RTT:
      return aidlimports::CallFailCause::RETRY_ON_IMS_WITHOUT_RTT;
    case qcril::interfaces::CallFailCause::UNOBTAINABLE_NUMBER:
      return aidlimports::CallFailCause::UNOBTAINABLE_NUMBER;
    case qcril::interfaces::CallFailCause::NORMAL:
      return aidlimports::CallFailCause::NORMAL;
    case qcril::interfaces::CallFailCause::BUSY:
      return aidlimports::CallFailCause::BUSY;
    case qcril::interfaces::CallFailCause::NORMAL_UNSPECIFIED:
      return aidlimports::CallFailCause::NORMAL_UNSPECIFIED;
    case qcril::interfaces::CallFailCause::CONGESTION:
      return aidlimports::CallFailCause::CONGESTION;
    case qcril::interfaces::CallFailCause::INCOMPATIBILITY_DESTINATION:
      return aidlimports::CallFailCause::INCOMPATIBILITY_DESTINATION;
    case qcril::interfaces::CallFailCause::CALL_BARRED:
      return aidlimports::CallFailCause::CALL_BARRED;
    case qcril::interfaces::CallFailCause::FDN_BLOCKED:
      return aidlimports::CallFailCause::FDN_BLOCKED;
    case qcril::interfaces::CallFailCause::IMEI_NOT_ACCEPTED:
      return aidlimports::CallFailCause::IMEI_NOT_ACCEPTED;
    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_USSD:
      return aidlimports::CallFailCause::DIAL_MODIFIED_TO_USSD;
    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_SS:
      return aidlimports::CallFailCause::DIAL_MODIFIED_TO_SS;
    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_DIAL:
      return aidlimports::CallFailCause::DIAL_MODIFIED_TO_DIAL;
    case qcril::interfaces::CallFailCause::EMERGENCY_TEMP_FAILURE:
      return aidlimports::CallFailCause::EMERGENCY_TEMP_FAILURE;
    case qcril::interfaces::CallFailCause::EMERGENCY_PERM_FAILURE:
      return aidlimports::CallFailCause::EMERGENCY_PERM_FAILURE;
    case qcril::interfaces::CallFailCause::HO_NOT_FEASIBLE:
      return aidlimports::CallFailCause::HO_NOT_FEASIBLE;
    case qcril::interfaces::CallFailCause::USER_BUSY:
      return aidlimports::CallFailCause::USER_BUSY;
    case qcril::interfaces::CallFailCause::USER_REJECT:
      return aidlimports::CallFailCause::USER_REJECT;
    case qcril::interfaces::CallFailCause::LOW_BATTERY:
      return aidlimports::CallFailCause::LOW_BATTERY;
    case qcril::interfaces::CallFailCause::BLACKLISTED_CALL_ID:
      return aidlimports::CallFailCause::BLACKLISTED_CALL_ID;
    case qcril::interfaces::CallFailCause::CS_RETRY_REQUIRED:
      return aidlimports::CallFailCause::CS_RETRY_REQUIRED;
    case qcril::interfaces::CallFailCause::NETWORK_UNAVAILABLE:
      return aidlimports::CallFailCause::NETWORK_UNAVAILABLE;
    case qcril::interfaces::CallFailCause::FEATURE_UNAVAILABLE:
      return aidlimports::CallFailCause::FEATURE_UNAVAILABLE;
    case qcril::interfaces::CallFailCause::SIP_ERROR:
      return aidlimports::CallFailCause::SIP_ERROR;
    case qcril::interfaces::CallFailCause::MISC:
      return aidlimports::CallFailCause::MISC;
    case qcril::interfaces::CallFailCause::ANSWERED_ELSEWHERE:
      return aidlimports::CallFailCause::ANSWERED_ELSEWHERE;
    case qcril::interfaces::CallFailCause::PULL_OUT_OF_SYNC:
      return aidlimports::CallFailCause::PULL_OUT_OF_SYNC;
    case qcril::interfaces::CallFailCause::CAUSE_CALL_PULLED:
      return aidlimports::CallFailCause::CAUSE_CALL_PULLED;
    case qcril::interfaces::CallFailCause::ACCESS_CLASS_BLOCKED:
      return aidlimports::CallFailCause::ACCESS_CLASS_BLOCKED;
    case qcril::interfaces::CallFailCause::SIP_REDIRECTED:
      return aidlimports::CallFailCause::SIP_REDIRECTED;
    case qcril::interfaces::CallFailCause::SIP_BAD_REQUEST:
      return aidlimports::CallFailCause::SIP_BAD_REQUEST;
    case qcril::interfaces::CallFailCause::SIP_FORBIDDEN:
      return aidlimports::CallFailCause::SIP_FORBIDDEN;
    case qcril::interfaces::CallFailCause::SIP_NOT_FOUND:
      return aidlimports::CallFailCause::SIP_NOT_FOUND;
    case qcril::interfaces::CallFailCause::SIP_NOT_SUPPORTED:
      return aidlimports::CallFailCause::SIP_NOT_SUPPORTED;
    case qcril::interfaces::CallFailCause::SIP_REQUEST_TIMEOUT:
      return aidlimports::CallFailCause::SIP_REQUEST_TIMEOUT;
    case qcril::interfaces::CallFailCause::SIP_TEMPORARILY_UNAVAILABLE:
      return aidlimports::CallFailCause::SIP_TEMPORARILY_UNAVAILABLE;
    case qcril::interfaces::CallFailCause::SIP_BAD_ADDRESS:
      return aidlimports::CallFailCause::SIP_BAD_ADDRESS;
    case qcril::interfaces::CallFailCause::SIP_BUSY:
      return aidlimports::CallFailCause::SIP_BUSY;
    case qcril::interfaces::CallFailCause::SIP_REQUEST_CANCELLED:
      return aidlimports::CallFailCause::SIP_REQUEST_CANCELLED;
    case qcril::interfaces::CallFailCause::SIP_NOT_ACCEPTABLE:
      return aidlimports::CallFailCause::SIP_NOT_ACCEPTABLE;
    case qcril::interfaces::CallFailCause::SIP_NOT_REACHABLE:
      return aidlimports::CallFailCause::SIP_NOT_REACHABLE;
    case qcril::interfaces::CallFailCause::SIP_SERVER_INTERNAL_ERROR:
      return aidlimports::CallFailCause::SIP_SERVER_INTERNAL_ERROR;
    case qcril::interfaces::CallFailCause::SIP_SERVER_NOT_IMPLEMENTED:
      return aidlimports::CallFailCause::SIP_SERVER_NOT_IMPLEMENTED;
    case qcril::interfaces::CallFailCause::SIP_SERVER_BAD_GATEWAY:
      return aidlimports::CallFailCause::SIP_SERVER_BAD_GATEWAY;
    case qcril::interfaces::CallFailCause::SIP_SERVICE_UNAVAILABLE:
      return aidlimports::CallFailCause::SIP_SERVICE_UNAVAILABLE;
    case qcril::interfaces::CallFailCause::SIP_SERVER_TIMEOUT:
      return aidlimports::CallFailCause::SIP_SERVER_TIMEOUT;
    case qcril::interfaces::CallFailCause::SIP_SERVER_VERSION_UNSUPPORTED:
      return aidlimports::CallFailCause::SIP_SERVER_VERSION_UNSUPPORTED;
    case qcril::interfaces::CallFailCause::SIP_SERVER_MESSAGE_TOOLARGE:
      return aidlimports::CallFailCause::SIP_SERVER_MESSAGE_TOOLARGE;
    case qcril::interfaces::CallFailCause::SIP_SERVER_PRECONDITION_FAILURE:
      return aidlimports::CallFailCause::SIP_SERVER_PRECONDITION_FAILURE;
    case qcril::interfaces::CallFailCause::SIP_USER_REJECTED:
      return aidlimports::CallFailCause::SIP_USER_REJECTED;
    case qcril::interfaces::CallFailCause::SIP_GLOBAL_ERROR:
      return aidlimports::CallFailCause::SIP_GLOBAL_ERROR;
    case qcril::interfaces::CallFailCause::MEDIA_INIT_FAILED:
      return aidlimports::CallFailCause::MEDIA_INIT_FAILED;
    case qcril::interfaces::CallFailCause::MEDIA_NO_DATA:
      return aidlimports::CallFailCause::MEDIA_NO_DATA;
    case qcril::interfaces::CallFailCause::MEDIA_NOT_ACCEPTABLE:
      return aidlimports::CallFailCause::MEDIA_NOT_ACCEPTABLE;
    case qcril::interfaces::CallFailCause::MEDIA_UNSPECIFIED_ERROR:
      return aidlimports::CallFailCause::MEDIA_UNSPECIFIED_ERROR;
    case qcril::interfaces::CallFailCause::HOLD_RESUME_FAILED:
      return aidlimports::CallFailCause::HOLD_RESUME_FAILED;
    case qcril::interfaces::CallFailCause::HOLD_RESUME_CANCELED:
      return aidlimports::CallFailCause::HOLD_RESUME_CANCELED;
    case qcril::interfaces::CallFailCause::HOLD_REINVITE_COLLISION:
      return aidlimports::CallFailCause::HOLD_REINVITE_COLLISION;
    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_DIAL_VIDEO:
      return aidlimports::CallFailCause::DIAL_MODIFIED_TO_DIAL_VIDEO;
    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL:
      return aidlimports::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL;
    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
      return aidlimports::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_SS:
      return aidlimports::CallFailCause::DIAL_VIDEO_MODIFIED_TO_SS;
    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_USSD:
      return aidlimports::CallFailCause::DIAL_VIDEO_MODIFIED_TO_USSD;
    case qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED:
      return aidlimports::CallFailCause::ERROR_UNSPECIFIED;
    case qcril::interfaces::CallFailCause::SIP_ALTERNATE_EMERGENCY_CALL:
      return aidlimports::CallFailCause::SIP_ALTERNATE_EMERGENCY_CALL;
    case qcril::interfaces::CallFailCause::RADIO_OFF:
      return aidlimports::CallFailCause::RADIO_OFF;
    case qcril::interfaces::CallFailCause::OUT_OF_SERVICE:
      return aidlimports::CallFailCause::OUT_OF_SERVICE;
    case qcril::interfaces::CallFailCause::NO_VALID_SIM:
      return aidlimports::CallFailCause::NO_VALID_SIM;
    case qcril::interfaces::CallFailCause::RADIO_INTERNAL_ERROR:
      return aidlimports::CallFailCause::RADIO_INTERNAL_ERROR;
    case qcril::interfaces::CallFailCause::NETWORK_RESP_TIMEOUT:
      return aidlimports::CallFailCause::NETWORK_RESP_TIMEOUT;
    case qcril::interfaces::CallFailCause::NETWORK_REJECT:
      return aidlimports::CallFailCause::NETWORK_REJECT;
    case qcril::interfaces::CallFailCause::RADIO_ACCESS_FAILURE:
      return aidlimports::CallFailCause::RADIO_ACCESS_FAILURE;
    case qcril::interfaces::CallFailCause::RADIO_LINK_FAILURE:
      return aidlimports::CallFailCause::RADIO_LINK_FAILURE;
    case qcril::interfaces::CallFailCause::RADIO_LINK_LOST:
      return aidlimports::CallFailCause::RADIO_LINK_LOST;
    case qcril::interfaces::CallFailCause::RADIO_UPLINK_FAILURE:
      return aidlimports::CallFailCause::RADIO_UPLINK_FAILURE;
    case qcril::interfaces::CallFailCause::RADIO_SETUP_FAILURE:
      return aidlimports::CallFailCause::RADIO_SETUP_FAILURE;
    case qcril::interfaces::CallFailCause::RADIO_RELEASE_NORMAL:
      return aidlimports::CallFailCause::RADIO_RELEASE_NORMAL;
    case qcril::interfaces::CallFailCause::RADIO_RELEASE_ABNORMAL:
      return aidlimports::CallFailCause::RADIO_RELEASE_ABNORMAL;
    case qcril::interfaces::CallFailCause::NETWORK_DETACH:
      return aidlimports::CallFailCause::NETWORK_DETACH;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_1:
      return aidlimports::CallFailCause::OEM_CAUSE_1;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_2:
      return aidlimports::CallFailCause::OEM_CAUSE_2;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_3:
      return aidlimports::CallFailCause::OEM_CAUSE_3;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_4:
      return aidlimports::CallFailCause::OEM_CAUSE_4;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_5:
      return aidlimports::CallFailCause::OEM_CAUSE_5;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_6:
      return aidlimports::CallFailCause::OEM_CAUSE_6;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_7:
      return aidlimports::CallFailCause::OEM_CAUSE_7;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_8:
      return aidlimports::CallFailCause::OEM_CAUSE_8;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_9:
      return aidlimports::CallFailCause::OEM_CAUSE_9;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_10:
      return aidlimports::CallFailCause::OEM_CAUSE_10;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_11:
      return aidlimports::CallFailCause::OEM_CAUSE_11;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_12:
      return aidlimports::CallFailCause::OEM_CAUSE_12;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_13:
      return aidlimports::CallFailCause::OEM_CAUSE_13;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_14:
      return aidlimports::CallFailCause::OEM_CAUSE_14;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_15:
      return aidlimports::CallFailCause::OEM_CAUSE_15;
    case qcril::interfaces::CallFailCause::NO_CSFB_IN_CS_ROAM:
      return aidlimports::CallFailCause::NO_CSFB_IN_CS_ROAM;
    case qcril::interfaces::CallFailCause::SRV_NOT_REGISTERED:
      return aidlimports::CallFailCause::SRV_NOT_REGISTERED;
    case qcril::interfaces::CallFailCause::CALL_TYPE_NOT_ALLOWED:
      return aidlimports::CallFailCause::CALL_TYPE_NOT_ALLOWED;
    case qcril::interfaces::CallFailCause::EMRG_CALL_ONGOING:
      return aidlimports::CallFailCause::EMRG_CALL_ONGOING;
    case qcril::interfaces::CallFailCause::CALL_SETUP_ONGOING:
      return aidlimports::CallFailCause::CALL_SETUP_ONGOING;
    case qcril::interfaces::CallFailCause::MAX_CALL_LIMIT_REACHED:
      return aidlimports::CallFailCause::MAX_CALL_LIMIT_REACHED;
    case qcril::interfaces::CallFailCause::UNSUPPORTED_SIP_HDRS:
      return aidlimports::CallFailCause::UNSUPPORTED_SIP_HDRS;
    case qcril::interfaces::CallFailCause::CALL_TRANSFER_ONGOING:
      return aidlimports::CallFailCause::CALL_TRANSFER_ONGOING;
    case qcril::interfaces::CallFailCause::PRACK_TIMEOUT:
      return aidlimports::CallFailCause::PRACK_TIMEOUT;
    case qcril::interfaces::CallFailCause::QOS_FAILURE:
      return aidlimports::CallFailCause::QOS_FAILURE;
    case qcril::interfaces::CallFailCause::ONGOING_HANDOVER:
      return aidlimports::CallFailCause::ONGOING_HANDOVER;
    case qcril::interfaces::CallFailCause::VT_WITH_TTY_NOT_ALLOWED:
      return aidlimports::CallFailCause::VT_WITH_TTY_NOT_ALLOWED;
    case qcril::interfaces::CallFailCause::CALL_UPGRADE_ONGOING:
      return aidlimports::CallFailCause::CALL_UPGRADE_ONGOING;
    case qcril::interfaces::CallFailCause::CONFERENCE_WITH_TTY_NOT_ALLOWED:
      return aidlimports::CallFailCause::CONFERENCE_WITH_TTY_NOT_ALLOWED;
    case qcril::interfaces::CallFailCause::CALL_CONFERENCE_ONGOING:
      return aidlimports::CallFailCause::CALL_CONFERENCE_ONGOING;
    case qcril::interfaces::CallFailCause::VT_WITH_AVPF_NOT_ALLOWED:
      return aidlimports::CallFailCause::VT_WITH_AVPF_NOT_ALLOWED;
    case qcril::interfaces::CallFailCause::ENCRYPTION_CALL_ONGOING:
      return aidlimports::CallFailCause::ENCRYPTION_CALL_ONGOING;
    case qcril::interfaces::CallFailCause::CALL_ONGOING_CW_DISABLED:
      return aidlimports::CallFailCause::CALL_ONGOING_CW_DISABLED;
    case qcril::interfaces::CallFailCause::CALL_ON_OTHER_SUB:
      return aidlimports::CallFailCause::CALL_ON_OTHER_SUB;
    case qcril::interfaces::CallFailCause::ONE_X_COLLISION:
      return aidlimports::CallFailCause::COLLISION_1X;
    case qcril::interfaces::CallFailCause::UI_NOT_READY:
      return aidlimports::CallFailCause::UI_NOT_READY;
    case qcril::interfaces::CallFailCause::CS_CALL_ONGOING:
      return aidlimports::CallFailCause::CS_CALL_ONGOING;
    case qcril::interfaces::CallFailCause::SIP_USER_MARKED_UNWANTED:
      return aidlimports::CallFailCause::SIP_USER_MARKED_UNWANTED;
    case qcril::interfaces::CallFailCause::DSDA_CONCURRENT_CALL_NOT_POSSIBLE:
      return aidlimports::CallFailCause::DSDA_CONCURRENT_CALL_NOT_POSSIBLE;
    case qcril::interfaces::CallFailCause::EPSFB_FAILURE:
      return aidlimports::CallFailCause::EPSFB_FAILURE;
    case qcril::interfaces::CallFailCause::TWAIT_EXPIRED:
      return aidlimports::CallFailCause::TWAIT_EXPIRED;
    case qcril::interfaces::CallFailCause::TCP_CONNECTION_REQ:
      return aidlimports::CallFailCause::TCP_CONNECTION_REQ;
    case qcril::interfaces::CallFailCause::UNKNOWN:
      return aidlimports::CallFailCause::INVALID;
    default:
      return static_cast<aidlimports::CallFailCause>(in);
  }
}

qcril::interfaces::CallFailCause convertToRilCallFailCause(const aidlimports::CallFailCause& in) {
  switch (in) {
    case aidlimports::CallFailCause::REJECTED_ELSEWHERE:
      return qcril::interfaces::CallFailCause::REJECTED_ELSEWHERE;
    case aidlimports::CallFailCause::USER_REJECTED_SESSION_MODIFICATION:
      return qcril::interfaces::CallFailCause::USER_REJECTED_SESSION_MODIFICATION;
    case aidlimports::CallFailCause::USER_CANCELLED_SESSION_MODIFICATION:
      return qcril::interfaces::CallFailCause::USER_CANCELLED_SESSION_MODIFICATION;
    case aidlimports::CallFailCause::SESSION_MODIFICATION_FAILED:
      return qcril::interfaces::CallFailCause::SESSION_MODIFICATION_FAILED;
    case aidlimports::CallFailCause::SIP_METHOD_NOT_ALLOWED:
      return qcril::interfaces::CallFailCause::SIP_METHOD_NOT_ALLOWED;
    case aidlimports::CallFailCause::SIP_PROXY_AUTHENTICATION_REQUIRED:
      return qcril::interfaces::CallFailCause::SIP_PROXY_AUTHENTICATION_REQUIRED;
    case aidlimports::CallFailCause::SIP_REQUEST_ENTITY_TOO_LARGE:
      return qcril::interfaces::CallFailCause::SIP_REQUEST_ENTITY_TOO_LARGE;
    case aidlimports::CallFailCause::SIP_REQUEST_URI_TOO_LARGE:
      return qcril::interfaces::CallFailCause::SIP_REQUEST_URI_TOO_LARGE;
    case aidlimports::CallFailCause::SIP_EXTENSION_REQUIRED:
      return qcril::interfaces::CallFailCause::SIP_EXTENSION_REQUIRED;
    case aidlimports::CallFailCause::SIP_INTERVAL_TOO_BRIEF:
      return qcril::interfaces::CallFailCause::SIP_INTERVAL_TOO_BRIEF;
    case aidlimports::CallFailCause::SIP_CALL_OR_TRANS_DOES_NOT_EXIST:
      return qcril::interfaces::CallFailCause::SIP_CALL_OR_TRANS_DOES_NOT_EXIST;
    case aidlimports::CallFailCause::SIP_LOOP_DETECTED:
      return qcril::interfaces::CallFailCause::SIP_LOOP_DETECTED;
    case aidlimports::CallFailCause::SIP_TOO_MANY_HOPS:
      return qcril::interfaces::CallFailCause::SIP_TOO_MANY_HOPS;
    case aidlimports::CallFailCause::SIP_AMBIGUOUS:
      return qcril::interfaces::CallFailCause::SIP_AMBIGUOUS;
    case aidlimports::CallFailCause::SIP_REQUEST_PENDING:
      return qcril::interfaces::CallFailCause::SIP_REQUEST_PENDING;
    case aidlimports::CallFailCause::SIP_UNDECIPHERABLE:
      return qcril::interfaces::CallFailCause::SIP_UNDECIPHERABLE;
    case aidlimports::CallFailCause::RETRY_ON_IMS_WITHOUT_RTT:
      return qcril::interfaces::CallFailCause::RETRY_ON_IMS_WITHOUT_RTT;
    case aidlimports::CallFailCause::UNOBTAINABLE_NUMBER:
      return qcril::interfaces::CallFailCause::UNOBTAINABLE_NUMBER;
    case aidlimports::CallFailCause::NORMAL:
      return qcril::interfaces::CallFailCause::NORMAL;
    case aidlimports::CallFailCause::BUSY:
      return qcril::interfaces::CallFailCause::BUSY;
    case aidlimports::CallFailCause::NORMAL_UNSPECIFIED:
      return qcril::interfaces::CallFailCause::NORMAL_UNSPECIFIED;
    case aidlimports::CallFailCause::CONGESTION:
      return qcril::interfaces::CallFailCause::CONGESTION;
    case aidlimports::CallFailCause::INCOMPATIBILITY_DESTINATION:
      return qcril::interfaces::CallFailCause::INCOMPATIBILITY_DESTINATION;
    case aidlimports::CallFailCause::CALL_BARRED:
      return qcril::interfaces::CallFailCause::CALL_BARRED;
    case aidlimports::CallFailCause::FDN_BLOCKED:
      return qcril::interfaces::CallFailCause::FDN_BLOCKED;
    case aidlimports::CallFailCause::IMEI_NOT_ACCEPTED:
      return qcril::interfaces::CallFailCause::IMEI_NOT_ACCEPTED;
    case aidlimports::CallFailCause::DIAL_MODIFIED_TO_USSD:
      return qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_USSD;
    case aidlimports::CallFailCause::DIAL_MODIFIED_TO_SS:
      return qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_SS;
    case aidlimports::CallFailCause::DIAL_MODIFIED_TO_DIAL:
      return qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_DIAL;
    case aidlimports::CallFailCause::EMERGENCY_TEMP_FAILURE:
      return qcril::interfaces::CallFailCause::EMERGENCY_TEMP_FAILURE;
    case aidlimports::CallFailCause::EMERGENCY_PERM_FAILURE:
      return qcril::interfaces::CallFailCause::EMERGENCY_PERM_FAILURE;
    case aidlimports::CallFailCause::HO_NOT_FEASIBLE:
      return qcril::interfaces::CallFailCause::HO_NOT_FEASIBLE;
    case aidlimports::CallFailCause::USER_BUSY:
      return qcril::interfaces::CallFailCause::USER_BUSY;
    case aidlimports::CallFailCause::USER_REJECT:
      return qcril::interfaces::CallFailCause::USER_REJECT;
    case aidlimports::CallFailCause::LOW_BATTERY:
      return qcril::interfaces::CallFailCause::LOW_BATTERY;
    case aidlimports::CallFailCause::BLACKLISTED_CALL_ID:
      return qcril::interfaces::CallFailCause::BLACKLISTED_CALL_ID;
    case aidlimports::CallFailCause::CS_RETRY_REQUIRED:
      return qcril::interfaces::CallFailCause::CS_RETRY_REQUIRED;
    case aidlimports::CallFailCause::NETWORK_UNAVAILABLE:
      return qcril::interfaces::CallFailCause::NETWORK_UNAVAILABLE;
    case aidlimports::CallFailCause::FEATURE_UNAVAILABLE:
      return qcril::interfaces::CallFailCause::FEATURE_UNAVAILABLE;
    case aidlimports::CallFailCause::SIP_ERROR:
      return qcril::interfaces::CallFailCause::SIP_ERROR;
    case aidlimports::CallFailCause::MISC:
      return qcril::interfaces::CallFailCause::MISC;
    case aidlimports::CallFailCause::ANSWERED_ELSEWHERE:
      return qcril::interfaces::CallFailCause::ANSWERED_ELSEWHERE;
    case aidlimports::CallFailCause::PULL_OUT_OF_SYNC:
      return qcril::interfaces::CallFailCause::PULL_OUT_OF_SYNC;
    case aidlimports::CallFailCause::CAUSE_CALL_PULLED:
      return qcril::interfaces::CallFailCause::CAUSE_CALL_PULLED;
    case aidlimports::CallFailCause::ACCESS_CLASS_BLOCKED:
      return qcril::interfaces::CallFailCause::ACCESS_CLASS_BLOCKED;
    case aidlimports::CallFailCause::SIP_REDIRECTED:
      return qcril::interfaces::CallFailCause::SIP_REDIRECTED;
    case aidlimports::CallFailCause::SIP_BAD_REQUEST:
      return qcril::interfaces::CallFailCause::SIP_BAD_REQUEST;
    case aidlimports::CallFailCause::SIP_FORBIDDEN:
      return qcril::interfaces::CallFailCause::SIP_FORBIDDEN;
    case aidlimports::CallFailCause::SIP_NOT_FOUND:
      return qcril::interfaces::CallFailCause::SIP_NOT_FOUND;
    case aidlimports::CallFailCause::SIP_NOT_SUPPORTED:
      return qcril::interfaces::CallFailCause::SIP_NOT_SUPPORTED;
    case aidlimports::CallFailCause::SIP_REQUEST_TIMEOUT:
      return qcril::interfaces::CallFailCause::SIP_REQUEST_TIMEOUT;
    case aidlimports::CallFailCause::SIP_TEMPORARILY_UNAVAILABLE:
      return qcril::interfaces::CallFailCause::SIP_TEMPORARILY_UNAVAILABLE;
    case aidlimports::CallFailCause::SIP_BAD_ADDRESS:
      return qcril::interfaces::CallFailCause::SIP_BAD_ADDRESS;
    case aidlimports::CallFailCause::SIP_BUSY:
      return qcril::interfaces::CallFailCause::SIP_BUSY;
    case aidlimports::CallFailCause::SIP_REQUEST_CANCELLED:
      return qcril::interfaces::CallFailCause::SIP_REQUEST_CANCELLED;
    case aidlimports::CallFailCause::SIP_NOT_ACCEPTABLE:
      return qcril::interfaces::CallFailCause::SIP_NOT_ACCEPTABLE;
    case aidlimports::CallFailCause::SIP_NOT_REACHABLE:
      return qcril::interfaces::CallFailCause::SIP_NOT_REACHABLE;
    case aidlimports::CallFailCause::SIP_SERVER_INTERNAL_ERROR:
      return qcril::interfaces::CallFailCause::SIP_SERVER_INTERNAL_ERROR;
    case aidlimports::CallFailCause::SIP_SERVER_NOT_IMPLEMENTED:
      return qcril::interfaces::CallFailCause::SIP_SERVER_NOT_IMPLEMENTED;
    case aidlimports::CallFailCause::SIP_SERVER_BAD_GATEWAY:
      return qcril::interfaces::CallFailCause::SIP_SERVER_BAD_GATEWAY;
    case aidlimports::CallFailCause::SIP_SERVICE_UNAVAILABLE:
      return qcril::interfaces::CallFailCause::SIP_SERVICE_UNAVAILABLE;
    case aidlimports::CallFailCause::SIP_SERVER_TIMEOUT:
      return qcril::interfaces::CallFailCause::SIP_SERVER_TIMEOUT;
    case aidlimports::CallFailCause::SIP_SERVER_VERSION_UNSUPPORTED:
      return qcril::interfaces::CallFailCause::SIP_SERVER_VERSION_UNSUPPORTED;
    case aidlimports::CallFailCause::SIP_SERVER_MESSAGE_TOOLARGE:
      return qcril::interfaces::CallFailCause::SIP_SERVER_MESSAGE_TOOLARGE;
    case aidlimports::CallFailCause::SIP_SERVER_PRECONDITION_FAILURE:
      return qcril::interfaces::CallFailCause::SIP_SERVER_PRECONDITION_FAILURE;
    case aidlimports::CallFailCause::SIP_USER_REJECTED:
      return qcril::interfaces::CallFailCause::SIP_USER_REJECTED;
    case aidlimports::CallFailCause::SIP_GLOBAL_ERROR:
      return qcril::interfaces::CallFailCause::SIP_GLOBAL_ERROR;
    case aidlimports::CallFailCause::MEDIA_INIT_FAILED:
      return qcril::interfaces::CallFailCause::MEDIA_INIT_FAILED;
    case aidlimports::CallFailCause::MEDIA_NO_DATA:
      return qcril::interfaces::CallFailCause::MEDIA_NO_DATA;
    case aidlimports::CallFailCause::MEDIA_NOT_ACCEPTABLE:
      return qcril::interfaces::CallFailCause::MEDIA_NOT_ACCEPTABLE;
    case aidlimports::CallFailCause::MEDIA_UNSPECIFIED_ERROR:
      return qcril::interfaces::CallFailCause::MEDIA_UNSPECIFIED_ERROR;
    case aidlimports::CallFailCause::HOLD_RESUME_FAILED:
      return qcril::interfaces::CallFailCause::HOLD_RESUME_FAILED;
    case aidlimports::CallFailCause::HOLD_RESUME_CANCELED:
      return qcril::interfaces::CallFailCause::HOLD_RESUME_CANCELED;
    case aidlimports::CallFailCause::HOLD_REINVITE_COLLISION:
      return qcril::interfaces::CallFailCause::HOLD_REINVITE_COLLISION;
    case aidlimports::CallFailCause::DIAL_MODIFIED_TO_DIAL_VIDEO:
      return qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_DIAL_VIDEO;
    case aidlimports::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL:
      return qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL;
    case aidlimports::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
      return qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
    case aidlimports::CallFailCause::DIAL_VIDEO_MODIFIED_TO_SS:
      return qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_SS;
    case aidlimports::CallFailCause::DIAL_VIDEO_MODIFIED_TO_USSD:
      return qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_USSD;
    case aidlimports::CallFailCause::ERROR_UNSPECIFIED:
      return qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED;
    case aidlimports::CallFailCause::SIP_ALTERNATE_EMERGENCY_CALL:
      return qcril::interfaces::CallFailCause::SIP_ALTERNATE_EMERGENCY_CALL;
    case aidlimports::CallFailCause::RADIO_OFF:
      return qcril::interfaces::CallFailCause::RADIO_OFF;
    case aidlimports::CallFailCause::OUT_OF_SERVICE:
      return qcril::interfaces::CallFailCause::OUT_OF_SERVICE;
    case aidlimports::CallFailCause::NO_VALID_SIM:
      return qcril::interfaces::CallFailCause::NO_VALID_SIM;
    case aidlimports::CallFailCause::RADIO_INTERNAL_ERROR:
      return qcril::interfaces::CallFailCause::RADIO_INTERNAL_ERROR;
    case aidlimports::CallFailCause::NETWORK_RESP_TIMEOUT:
      return qcril::interfaces::CallFailCause::NETWORK_RESP_TIMEOUT;
    case aidlimports::CallFailCause::NETWORK_REJECT:
      return qcril::interfaces::CallFailCause::NETWORK_REJECT;
    case aidlimports::CallFailCause::RADIO_ACCESS_FAILURE:
      return qcril::interfaces::CallFailCause::RADIO_ACCESS_FAILURE;
    case aidlimports::CallFailCause::RADIO_LINK_FAILURE:
      return qcril::interfaces::CallFailCause::RADIO_LINK_FAILURE;
    case aidlimports::CallFailCause::RADIO_LINK_LOST:
      return qcril::interfaces::CallFailCause::RADIO_LINK_LOST;
    case aidlimports::CallFailCause::RADIO_UPLINK_FAILURE:
      return qcril::interfaces::CallFailCause::RADIO_UPLINK_FAILURE;
    case aidlimports::CallFailCause::RADIO_SETUP_FAILURE:
      return qcril::interfaces::CallFailCause::RADIO_SETUP_FAILURE;
    case aidlimports::CallFailCause::RADIO_RELEASE_NORMAL:
      return qcril::interfaces::CallFailCause::RADIO_RELEASE_NORMAL;
    case aidlimports::CallFailCause::RADIO_RELEASE_ABNORMAL:
      return qcril::interfaces::CallFailCause::RADIO_RELEASE_ABNORMAL;
    case aidlimports::CallFailCause::NETWORK_DETACH:
      return qcril::interfaces::CallFailCause::NETWORK_DETACH;
    case aidlimports::CallFailCause::OEM_CAUSE_1:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_1;
    case aidlimports::CallFailCause::OEM_CAUSE_2:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_2;
    case aidlimports::CallFailCause::OEM_CAUSE_3:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_3;
    case aidlimports::CallFailCause::OEM_CAUSE_4:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_4;
    case aidlimports::CallFailCause::OEM_CAUSE_5:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_5;
    case aidlimports::CallFailCause::OEM_CAUSE_6:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_6;
    case aidlimports::CallFailCause::OEM_CAUSE_7:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_7;
    case aidlimports::CallFailCause::OEM_CAUSE_8:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_8;
    case aidlimports::CallFailCause::OEM_CAUSE_9:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_9;
    case aidlimports::CallFailCause::OEM_CAUSE_10:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_10;
    case aidlimports::CallFailCause::OEM_CAUSE_11:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_11;
    case aidlimports::CallFailCause::OEM_CAUSE_12:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_12;
    case aidlimports::CallFailCause::OEM_CAUSE_13:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_13;
    case aidlimports::CallFailCause::OEM_CAUSE_14:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_14;
    case aidlimports::CallFailCause::OEM_CAUSE_15:
      return qcril::interfaces::CallFailCause::OEM_CAUSE_15;
    case aidlimports::CallFailCause::NO_CSFB_IN_CS_ROAM:
      return qcril::interfaces::CallFailCause::NO_CSFB_IN_CS_ROAM;
    case aidlimports::CallFailCause::SRV_NOT_REGISTERED:
      return qcril::interfaces::CallFailCause::SRV_NOT_REGISTERED;
    case aidlimports::CallFailCause::CALL_TYPE_NOT_ALLOWED:
      return qcril::interfaces::CallFailCause::CALL_TYPE_NOT_ALLOWED;
    case aidlimports::CallFailCause::EMRG_CALL_ONGOING:
      return qcril::interfaces::CallFailCause::EMRG_CALL_ONGOING;
    case aidlimports::CallFailCause::CALL_SETUP_ONGOING:
      return qcril::interfaces::CallFailCause::CALL_SETUP_ONGOING;
    case aidlimports::CallFailCause::MAX_CALL_LIMIT_REACHED:
      return qcril::interfaces::CallFailCause::MAX_CALL_LIMIT_REACHED;
    case aidlimports::CallFailCause::UNSUPPORTED_SIP_HDRS:
      return qcril::interfaces::CallFailCause::UNSUPPORTED_SIP_HDRS;
    case aidlimports::CallFailCause::CALL_TRANSFER_ONGOING:
      return qcril::interfaces::CallFailCause::CALL_TRANSFER_ONGOING;
    case aidlimports::CallFailCause::PRACK_TIMEOUT:
      return qcril::interfaces::CallFailCause::PRACK_TIMEOUT;
    case aidlimports::CallFailCause::QOS_FAILURE:
      return qcril::interfaces::CallFailCause::QOS_FAILURE;
    case aidlimports::CallFailCause::ONGOING_HANDOVER:
      return qcril::interfaces::CallFailCause::ONGOING_HANDOVER;
    case aidlimports::CallFailCause::VT_WITH_TTY_NOT_ALLOWED:
      return qcril::interfaces::CallFailCause::VT_WITH_TTY_NOT_ALLOWED;
    case aidlimports::CallFailCause::CALL_UPGRADE_ONGOING:
      return qcril::interfaces::CallFailCause::CALL_UPGRADE_ONGOING;
    case aidlimports::CallFailCause::CONFERENCE_WITH_TTY_NOT_ALLOWED:
      return qcril::interfaces::CallFailCause::CONFERENCE_WITH_TTY_NOT_ALLOWED;
    case aidlimports::CallFailCause::CALL_CONFERENCE_ONGOING:
      return qcril::interfaces::CallFailCause::CALL_CONFERENCE_ONGOING;
    case aidlimports::CallFailCause::VT_WITH_AVPF_NOT_ALLOWED:
      return qcril::interfaces::CallFailCause::VT_WITH_AVPF_NOT_ALLOWED;
    case aidlimports::CallFailCause::ENCRYPTION_CALL_ONGOING:
      return qcril::interfaces::CallFailCause::ENCRYPTION_CALL_ONGOING;
    case aidlimports::CallFailCause::CALL_ONGOING_CW_DISABLED:
      return qcril::interfaces::CallFailCause::CALL_ONGOING_CW_DISABLED;
    case aidlimports::CallFailCause::CALL_ON_OTHER_SUB:
      return qcril::interfaces::CallFailCause::CALL_ON_OTHER_SUB;
    case aidlimports::CallFailCause::COLLISION_1X:
      return qcril::interfaces::CallFailCause::ONE_X_COLLISION;
    case aidlimports::CallFailCause::UI_NOT_READY:
      return qcril::interfaces::CallFailCause::UI_NOT_READY;
    case aidlimports::CallFailCause::CS_CALL_ONGOING:
      return qcril::interfaces::CallFailCause::CS_CALL_ONGOING;
    case aidlimports::CallFailCause::SIP_USER_MARKED_UNWANTED:
      return qcril::interfaces::CallFailCause::SIP_USER_MARKED_UNWANTED;
    case aidlimports::CallFailCause::INVALID:
    default:
      return qcril::interfaces::CallFailCause::UNKNOWN;
  }
}

aidlimports::StatusType convertToAidlStatusType(const qcril::interfaces::StatusType& in) {
  switch (in) {
    case qcril::interfaces::StatusType::DISABLED:
      return aidlimports::StatusType::DISABLED;
    case qcril::interfaces::StatusType::PARTIALLY_ENABLED:
      return aidlimports::StatusType::PARTIALLY_ENABLED;
    case qcril::interfaces::StatusType::ENABLED:
      return aidlimports::StatusType::ENABLED;
    case qcril::interfaces::StatusType::NOT_SUPPORTED:
      return aidlimports::StatusType::NOT_SUPPORTED;
    case qcril::interfaces::StatusType::UNKNOWN:
    default:
      return aidlimports::StatusType::INVALID;
  }
}

bool convertToAidlAccessTechnologyStatus(aidlimports::StatusForAccessTech& out,
                                         const qcril::interfaces::AccessTechnologyStatus& in) {
  if (in.hasNetworkMode()) {
    out.networkMode = convertToAidlRadioTech(in.getNetworkMode());
  } else {
    out.networkMode = aidlimports::RadioTechType::INVALID;
  }

  if (in.hasStatusType()) {
    out.status = ims::utils::convertToAidlStatusType(in.getStatusType());
  } else {
    out.status = aidlimports::StatusType::INVALID;
  }

  if (in.hasRestrictCause()) {
    out.restrictCause = in.getRestrictCause();
  } else {
    out.restrictCause = INT32_MAX;
  }

  out.registration = { .state = aidlimports::RegState::INVALID,
                       .errorCode = INT32_MAX,
                       .radioTech = aidlimports::RadioTechType::INVALID };
  out.hasRegistration = in.hasRegistration();
  if (in.hasRegistration()) {
    convertRilRegistrationToAidlResponse(out.registration, *(in.getRegistration()));
  }
  return true;
}

aidlimports::ServiceStatusInfo convertToAidlServiceStatusInfo(
    const qcril::interfaces::ServiceStatusInfo& in) {
  aidlimports::ServiceStatusInfo out = {};
  out.isValid = true;

  if (in.hasCallType()) {
    out.callType = convertToAidlCallType(in.getCallType());
  } else {
    out.callType = aidlimports::CallType::UNKNOWN;
  }

  if (in.hasStatusType()) {
    out.status = convertToAidlStatusType(in.getStatusType());
  } else {
    out.status = aidlimports::StatusType::INVALID;
  }

  if (in.hasRestrictCause()) {
    out.restrictCause = in.getRestrictCause();
  } else {
    out.restrictCause = INT32_MAX;
  }

  if (in.hasRttMode()) {
    out.rttMode = convertToAidlRttMode(in.getRttMode());
  } else {
    out.rttMode = aidlimports::RttMode::INVALID;
  }

  if (in.hasAccessTechnologyStatus() && in.getAccessTechnologyStatus() != nullptr) {
    out.accTechStatus.resize(1);
    out.accTechStatus[0] = {};
    convertToAidlAccessTechnologyStatus(out.accTechStatus[0], *(in.getAccessTechnologyStatus()));
  }
  return out;
}

bool convertRilServiceStatusInfoListToAidlResponse(
    std::vector<aidlimports::ServiceStatusInfo>& out,
    const qcril::interfaces::ServiceStatusInfoList& in) {
  out.clear();
  for (qcril::interfaces::ServiceStatusInfo inServiceStatusInfo : in.getServiceStatusInfo()) {
    aidlimports::ServiceStatusInfo outServiceStatusInfo =
        convertToAidlServiceStatusInfo(inServiceStatusInfo);
    out.push_back(outServiceStatusInfo);
  }
  return true;
}  // convertRilServiceStatusInfoListToAidlResponse

bool convertRilServiceStatusInfoListToAidlResponse(
    std::vector<aidlimports::ServiceStatusInfo>& out,
    const std::vector<qcril::interfaces::ServiceStatusInfo>& in) {
  out.clear();
  for (qcril::interfaces::ServiceStatusInfo inServiceStatusInfo : in) {
    aidlimports::ServiceStatusInfo outServiceStatusInfo =
        convertToAidlServiceStatusInfo(inServiceStatusInfo);
    out.push_back(outServiceStatusInfo);
  }
  return true;
}  // convertRilServiceStatusInfoListToAidlResponse

aidlimports::ToneOperation convertToAidlToneOperation(
    const qcril::interfaces::RingBackToneOperation& in) {
  switch (in) {
    case qcril::interfaces::RingBackToneOperation::STOP:
      return aidlimports::ToneOperation::STOP;
    case qcril::interfaces::RingBackToneOperation::START:
      return aidlimports::ToneOperation::START;
    default:
      return aidlimports::ToneOperation::INVALID;
  }
}  // convertToAidlToneOperation

bool convertToAidlSipErrorInfo(aidlimports::SipErrorInfo& out,
                               const std::shared_ptr<qcril::interfaces::SipErrorInfo>& in,
                               bool considerErrorString) {
  out.errorCode = INT32_MAX;
  out.errorString = "";

  if (in == nullptr || (considerErrorString && !in->hasErrorString())) {
    return false;
  }

  if (in->hasErrorCode()) {
    out.errorCode = in->getErrorCode();
  }
  if (in->hasErrorString()) {
    out.errorString = in->getErrorString();
  }

  return true;
}  // convertToAidlSipErrorInfo

aidlimports::CallState convertToAidlCallState(const qcril::interfaces::CallState in) {
  switch (in) {
    case qcril::interfaces::CallState::ACTIVE:
      return aidlimports::CallState::ACTIVE;
    case qcril::interfaces::CallState::HOLDING:
      return aidlimports::CallState::HOLDING;
    case qcril::interfaces::CallState::DIALING:
      return aidlimports::CallState::DIALING;
    case qcril::interfaces::CallState::ALERTING:
      return aidlimports::CallState::ALERTING;
    case qcril::interfaces::CallState::INCOMING:
      return aidlimports::CallState::INCOMING;
    case qcril::interfaces::CallState::WAITING:
      return aidlimports::CallState::WAITING;
    case qcril::interfaces::CallState::END:
      return aidlimports::CallState::END;
    default:
      return aidlimports::CallState::INVALID;
  }
}

aidlimports::CallDomain convertToAidlCallDomain(const qcril::interfaces::CallDomain& in) {
  switch (in) {
    case qcril::interfaces::CallDomain::UNKNOWN:
      return aidlimports::CallDomain::UNKNOWN;
    case qcril::interfaces::CallDomain::CS:
      return aidlimports::CallDomain::CS;
    case qcril::interfaces::CallDomain::PS:
      return aidlimports::CallDomain::PS;
    case qcril::interfaces::CallDomain::AUTOMATIC:
      return aidlimports::CallDomain::AUTOMATIC;
    case qcril::interfaces::CallDomain::NOT_SET:
    default:
      return aidlimports::CallDomain::NOT_SET;
  }
}  // convertToAidlCallDomain

qcril::interfaces::CallDomain convertToRilCallDomain(const aidlimports::CallDomain& in) {
  switch (in) {
    case aidlimports::CallDomain::UNKNOWN:
      return qcril::interfaces::CallDomain::UNKNOWN;
    case aidlimports::CallDomain::CS:
      return qcril::interfaces::CallDomain::CS;
    case aidlimports::CallDomain::PS:
      return qcril::interfaces::CallDomain::PS;
    case aidlimports::CallDomain::AUTOMATIC:
      return qcril::interfaces::CallDomain::AUTOMATIC;
    case aidlimports::CallDomain::NOT_SET:
    default:
      return qcril::interfaces::CallDomain::NOT_SET;
  }
}

std::string getCodecStringExtra(const uint32_t codec) {
  std::string result;
  const char* codecstr[] = {
    "Codec=NONE",     /* None   */
    "Codec=QCELP13K", /* QCELP-13K   */
    "Codec=EVRC",     /* EVRC   */
    "Codec=EVRC_B",   /* EVRC-B   */
    "Codec=EVRC_WB",  /* EVRC wideband   */
    "Codec=EVRC_NW",  /* EVRC narrowband-wideband   */
    "Codec=AMR_NB",   /* AMR narrowband   */
    "Codec=AMR_WB",   /* AMR wideband   */
    "Codec=GSM_EFR",  /* GSM enhanced full rate   */
    "Codec=GSM_FR",   /* GSM full rate   */
    "Codec=GSM_HR",   /* GSM half rate   */
    "Codec=G711U",    /* G711U   */
    "Codec=G723",     /* G723   */
    "Codec=G711A",    /* G711A   */
    "Codec=G722",     /* G722   */
    "Codec=G711AB",   /* G711AB   */
    "Codec=G729",     /* G729   */
    "Codec=EVS_NB",   /* EVS narrowband   */
    "Codec=EVS_WB",   /* EVS_WB wideband   */
    "Codec=EVS_SWB",  /* EVS super-wideband   */
    "Codec=EVS_FB",   /* EVS fullband  */
  };

  if (codec < sizeof(codecstr) / sizeof(codecstr[0])) {
    result = codecstr[codec];
  }
  return result;
}

aidlimports::Codec convertToAidlCodec(const qcril::interfaces::AudioCodec& in) {
  switch (in) {
    case qcril::interfaces::AudioCodec::QCELP13K:
      return aidlimports::Codec::QCELP13K;
    case qcril::interfaces::AudioCodec::EVRC:
      return aidlimports::Codec::EVRC;
    case qcril::interfaces::AudioCodec::EVRC_B:
      return aidlimports::Codec::EVRC_B;
    case qcril::interfaces::AudioCodec::EVRC_WB:
      return aidlimports::Codec::EVRC_WB;
    case qcril::interfaces::AudioCodec::EVRC_NW:
      return aidlimports::Codec::EVRC_NW;
    case qcril::interfaces::AudioCodec::AMR_NB:
      return aidlimports::Codec::AMR_NB;
    case qcril::interfaces::AudioCodec::AMR_WB:
      return aidlimports::Codec::AMR_WB;
    case qcril::interfaces::AudioCodec::GSM_EFR:
      return aidlimports::Codec::GSM_EFR;
    case qcril::interfaces::AudioCodec::GSM_FR:
      return aidlimports::Codec::GSM_FR;
    case qcril::interfaces::AudioCodec::GSM_HR:
      return aidlimports::Codec::GSM_HR;
    case qcril::interfaces::AudioCodec::G711U:
      return aidlimports::Codec::G711U;
    case qcril::interfaces::AudioCodec::G723:
      return aidlimports::Codec::G723;
    case qcril::interfaces::AudioCodec::G711A:
      return aidlimports::Codec::G711A;
    case qcril::interfaces::AudioCodec::G722:
      return aidlimports::Codec::G722;
    case qcril::interfaces::AudioCodec::G711AB:
      return aidlimports::Codec::G711AB;
    case qcril::interfaces::AudioCodec::G729:
      return aidlimports::Codec::G729;
    case qcril::interfaces::AudioCodec::EVS_NB:
      return aidlimports::Codec::EVS_NB;
    case qcril::interfaces::AudioCodec::EVS_WB:
      return aidlimports::Codec::EVS_WB;
    case qcril::interfaces::AudioCodec::EVS_SWB:
      return aidlimports::Codec::EVS_SWB;
    case qcril::interfaces::AudioCodec::EVS_FB:
      return aidlimports::Codec::EVS_FB;
    case qcril::interfaces::AudioCodec::NONE:
    default:
      return aidlimports::Codec::INVALID;
  }
}

aidlimports::ComputedAudioQuality convertToAidlComputedAudioQuality(
    const qcril::interfaces::ComputedAudioQuality& in) {
  switch (in) {
    case qcril::interfaces::ComputedAudioQuality::NO_HD:
      return aidlimports::ComputedAudioQuality::NO_HD;
    case qcril::interfaces::ComputedAudioQuality::HD:
      return aidlimports::ComputedAudioQuality::HD;
    case qcril::interfaces::ComputedAudioQuality::HD_PLUS:
      return aidlimports::ComputedAudioQuality::HD_PLUS;
    case qcril::interfaces::ComputedAudioQuality::NONE:
    default:
      return aidlimports::ComputedAudioQuality::INVALID;
  }
}

bool convertToAidlCallFailCauseResponse(
    aidlimports::CallFailCauseResponse& out,
    const std::shared_ptr<qcril::interfaces::CallFailCauseResponse>& in) {
  if (in->hasFailCause()) {
    out.failCause = convertToAidlCallFailCause(in->getFailCause());
  } else {
    out.failCause = aidlimports::CallFailCause::INVALID;
  }

  out.errorInfo.resize(0);
  if (in->hasExtendedFailCause()) {
    std::string extFailCauseStr = std::to_string(in->getExtendedFailCause());
    if (!extFailCauseStr.empty()) {
      out.errorInfo.resize(extFailCauseStr.size());
      for (size_t i = 0; i < extFailCauseStr.size(); i++) {
        out.errorInfo[i] = extFailCauseStr[i];
      }
    }
  }

  if (in->hasNetworkErrorString() && !in->getNetworkErrorString().empty()) {
    out.networkErrorString = in->getNetworkErrorString();
  }

  if (in->hasErrorDetails() && in->getErrorDetails() != nullptr) {
    out.hasErrorDetails = true;
    convertToAidlSipErrorInfo(out.errorDetails, in->getErrorDetails());
  }
  return true;
}

qcril::interfaces::ConfigItem convertToRilConfigItem(const aidlimports::ConfigItem in) {
  qcril::interfaces::ConfigItem outConfigItem = qcril::interfaces::ConfigItem::INVALID_CONFIG;
  switch (in) {
    case aidlimports::ConfigItem::NONE:
      outConfigItem = qcril::interfaces::ConfigItem::NONE;
      break;
    case aidlimports::ConfigItem::VOCODER_AMRMODESET:
      outConfigItem = qcril::interfaces::ConfigItem::VOCODER_AMRMODESET;
      break;
    case aidlimports::ConfigItem::VOCODER_AMRWBMODESET:
      outConfigItem = qcril::interfaces::ConfigItem::VOCODER_AMRWBMODESET;
      break;
    case aidlimports::ConfigItem::SIP_SESSION_TIMER:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_SESSION_TIMER;
      break;
    case aidlimports::ConfigItem::MIN_SESSION_EXPIRY:
      outConfigItem = qcril::interfaces::ConfigItem::MIN_SESSION_EXPIRY;
      break;
    case aidlimports::ConfigItem::CANCELLATION_TIMER:
      outConfigItem = qcril::interfaces::ConfigItem::CANCELLATION_TIMER;
      break;
    case aidlimports::ConfigItem::T_DELAY:
      outConfigItem = qcril::interfaces::ConfigItem::T_DELAY;
      break;
    case aidlimports::ConfigItem::SILENT_REDIAL_ENABLE:
      outConfigItem = qcril::interfaces::ConfigItem::SILENT_REDIAL_ENABLE;
      break;
    case aidlimports::ConfigItem::SIP_T1_TIMER:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_T1_TIMER;
      break;
    case aidlimports::ConfigItem::SIP_T2_TIMER:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_T2_TIMER;
      break;
    case aidlimports::ConfigItem::SIP_TF_TIMER:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_TF_TIMER;
      break;
    case aidlimports::ConfigItem::VLT_SETTING_ENABLED:
      outConfigItem = qcril::interfaces::ConfigItem::VLT_SETTING_ENABLED;
      break;
    case aidlimports::ConfigItem::LVC_SETTING_ENABLED:
      outConfigItem = qcril::interfaces::ConfigItem::LVC_SETTING_ENABLED;
      break;
    case aidlimports::ConfigItem::DOMAIN_NAME:
      outConfigItem = qcril::interfaces::ConfigItem::DOMAIN_NAME;
      break;
    case aidlimports::ConfigItem::SMS_FORMAT:
      outConfigItem = qcril::interfaces::ConfigItem::SMS_FORMAT;
      break;
    case aidlimports::ConfigItem::SMS_OVER_IP:
      outConfigItem = qcril::interfaces::ConfigItem::SMS_OVER_IP;
      break;
    case aidlimports::ConfigItem::PUBLISH_TIMER:
      outConfigItem = qcril::interfaces::ConfigItem::PUBLISH_TIMER;
      break;
    case aidlimports::ConfigItem::PUBLISH_TIMER_EXTENDED:
      outConfigItem = qcril::interfaces::ConfigItem::PUBLISH_TIMER_EXTENDED;
      break;
    case aidlimports::ConfigItem::CAPABILITIES_CACHE_EXPIRATION:
      outConfigItem = qcril::interfaces::ConfigItem::CAPABILITIES_CACHE_EXPIRATION;
      break;
    case aidlimports::ConfigItem::AVAILABILITY_CACHE_EXPIRATION:
      outConfigItem = qcril::interfaces::ConfigItem::AVAILABILITY_CACHE_EXPIRATION;
      break;
    case aidlimports::ConfigItem::CAPABILITIES_POLL_INTERVAL:
      outConfigItem = qcril::interfaces::ConfigItem::CAPABILITIES_POLL_INTERVAL;
      break;
    case aidlimports::ConfigItem::SOURCE_THROTTLE_PUBLISH:
      outConfigItem = qcril::interfaces::ConfigItem::SOURCE_THROTTLE_PUBLISH;
      break;
    case aidlimports::ConfigItem::MAX_NUM_ENTRIES_IN_RCL:
      outConfigItem = qcril::interfaces::ConfigItem::MAX_NUM_ENTRIES_IN_RCL;
      break;
    case aidlimports::ConfigItem::CAPAB_POLL_LIST_SUB_EXP:
      outConfigItem = qcril::interfaces::ConfigItem::CAPAB_POLL_LIST_SUB_EXP;
      break;
    case aidlimports::ConfigItem::GZIP_FLAG:
      outConfigItem = qcril::interfaces::ConfigItem::GZIP_FLAG;
      break;
    case aidlimports::ConfigItem::EAB_SETTING_ENABLED:
      outConfigItem = qcril::interfaces::ConfigItem::EAB_SETTING_ENABLED;
      break;
    case aidlimports::ConfigItem::MOBILE_DATA_ENABLED:
      outConfigItem = qcril::interfaces::ConfigItem::MOBILE_DATA_ENABLED;
      break;
    case aidlimports::ConfigItem::VOICE_OVER_WIFI_ENABLED:
      outConfigItem = qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ENABLED;
      break;
    case aidlimports::ConfigItem::VOICE_OVER_WIFI_ROAMING:
      outConfigItem = qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ROAMING;
      break;
    case aidlimports::ConfigItem::VOICE_OVER_WIFI_MODE:
      outConfigItem = qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_MODE;
      break;
    case aidlimports::ConfigItem::CAPABILITY_DISCOVERY_ENABLED:
      outConfigItem = qcril::interfaces::ConfigItem::CAPABILITY_DISCOVERY_ENABLED;
      break;
    case aidlimports::ConfigItem::EMERGENCY_CALL_TIMER:
      outConfigItem = qcril::interfaces::ConfigItem::EMERGENCY_CALL_TIMER;
      break;
    case aidlimports::ConfigItem::SSAC_HYSTERESIS_TIMER:
      outConfigItem = qcril::interfaces::ConfigItem::SSAC_HYSTERESIS_TIMER;
      break;
    case aidlimports::ConfigItem::VOLTE_USER_OPT_IN_STATUS:
      outConfigItem = qcril::interfaces::ConfigItem::VOLTE_USER_OPT_IN_STATUS;
      break;
    case aidlimports::ConfigItem::LBO_PCSCF_ADDRESS:
      outConfigItem = qcril::interfaces::ConfigItem::LBO_PCSCF_ADDRESS;
      break;
    case aidlimports::ConfigItem::KEEP_ALIVE_ENABLED:
      outConfigItem = qcril::interfaces::ConfigItem::KEEP_ALIVE_ENABLED;
      break;
    case aidlimports::ConfigItem::REGISTRATION_RETRY_BASE_TIME_SEC:
      outConfigItem = qcril::interfaces::ConfigItem::REGISTRATION_RETRY_BASE_TIME_SEC;
      break;
    case aidlimports::ConfigItem::REGISTRATION_RETRY_MAX_TIME_SEC:
      outConfigItem = qcril::interfaces::ConfigItem::REGISTRATION_RETRY_MAX_TIME_SEC;
      break;
    case aidlimports::ConfigItem::SPEECH_START_PORT:
      outConfigItem = qcril::interfaces::ConfigItem::SPEECH_START_PORT;
      break;
    case aidlimports::ConfigItem::SPEECH_END_PORT:
      outConfigItem = qcril::interfaces::ConfigItem::SPEECH_END_PORT;
      break;
    case aidlimports::ConfigItem::SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
      break;
    case aidlimports::ConfigItem::SIP_INVITE_RSP_WAIT_TIME_MSEC:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_INVITE_RSP_WAIT_TIME_MSEC;
      break;
    case aidlimports::ConfigItem::SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
      break;
    case aidlimports::ConfigItem::SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
      break;
    case aidlimports::ConfigItem::SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
      break;
    case aidlimports::ConfigItem::SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
      break;
    case aidlimports::ConfigItem::SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
      break;
    case aidlimports::ConfigItem::SIP_ACK_RETX_WAIT_TIME_MSEC:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_ACK_RETX_WAIT_TIME_MSEC;
      break;
    case aidlimports::ConfigItem::SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
      break;
    case aidlimports::ConfigItem::SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
      outConfigItem = qcril::interfaces::ConfigItem::SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
      break;
    case aidlimports::ConfigItem::AMR_WB_OCTET_ALIGNED_PT:
      outConfigItem = qcril::interfaces::ConfigItem::AMR_WB_OCTET_ALIGNED_PT;
      break;
    case aidlimports::ConfigItem::AMR_WB_BANDWIDTH_EFFICIENT_PT:
      outConfigItem = qcril::interfaces::ConfigItem::AMR_WB_BANDWIDTH_EFFICIENT_PT;
      break;
    case aidlimports::ConfigItem::AMR_OCTET_ALIGNED_PT:
      outConfigItem = qcril::interfaces::ConfigItem::AMR_OCTET_ALIGNED_PT;
      break;
    case aidlimports::ConfigItem::AMR_BANDWIDTH_EFFICIENT_PT:
      outConfigItem = qcril::interfaces::ConfigItem::AMR_BANDWIDTH_EFFICIENT_PT;
      break;
    case aidlimports::ConfigItem::DTMF_WB_PT:
      outConfigItem = qcril::interfaces::ConfigItem::DTMF_WB_PT;
      break;
    case aidlimports::ConfigItem::DTMF_NB_PT:
      outConfigItem = qcril::interfaces::ConfigItem::DTMF_NB_PT;
      break;
    case aidlimports::ConfigItem::AMR_DEFAULT_MODE:
      outConfigItem = qcril::interfaces::ConfigItem::AMR_DEFAULT_MODE;
      break;
    case aidlimports::ConfigItem::SMS_PSI:
      outConfigItem = qcril::interfaces::ConfigItem::SMS_PSI;
      break;
    case aidlimports::ConfigItem::VIDEO_QUALITY:
      outConfigItem = qcril::interfaces::ConfigItem::VIDEO_QUALITY;
      break;
    case aidlimports::ConfigItem::THRESHOLD_LTE1:
      outConfigItem = qcril::interfaces::ConfigItem::THRESHOLD_LTE1;
      break;
    case aidlimports::ConfigItem::THRESHOLD_LTE2:
      outConfigItem = qcril::interfaces::ConfigItem::THRESHOLD_LTE2;
      break;
    case aidlimports::ConfigItem::THRESHOLD_LTE3:
      outConfigItem = qcril::interfaces::ConfigItem::THRESHOLD_LTE3;
      break;
    case aidlimports::ConfigItem::THRESHOLD_1x:
      outConfigItem = qcril::interfaces::ConfigItem::THRESHOLD_1x;
      break;
    case aidlimports::ConfigItem::THRESHOLD_WIFI_A:
      outConfigItem = qcril::interfaces::ConfigItem::THRESHOLD_WIFI_A;
      break;
    case aidlimports::ConfigItem::THRESHOLD_WIFI_B:
      outConfigItem = qcril::interfaces::ConfigItem::THRESHOLD_WIFI_B;
      break;
    case aidlimports::ConfigItem::T_EPDG_LTE:
      outConfigItem = qcril::interfaces::ConfigItem::T_EPDG_LTE;
      break;
    case aidlimports::ConfigItem::T_EPDG_WIFI:
      outConfigItem = qcril::interfaces::ConfigItem::T_EPDG_WIFI;
      break;
    case aidlimports::ConfigItem::T_EPDG_1x:
      outConfigItem = qcril::interfaces::ConfigItem::T_EPDG_1x;
      break;
    case aidlimports::ConfigItem::VWF_SETTING_ENABLED:
      outConfigItem = qcril::interfaces::ConfigItem::VWF_SETTING_ENABLED;
      break;
    case aidlimports::ConfigItem::VCE_SETTING_ENABLED:
      outConfigItem = qcril::interfaces::ConfigItem::VCE_SETTING_ENABLED;
      break;
    case aidlimports::ConfigItem::RTT_SETTING_ENABLED:
      outConfigItem = qcril::interfaces::ConfigItem::RTT_SETTING_ENABLED;
      break;
    case aidlimports::ConfigItem::SMS_APP:
      outConfigItem = qcril::interfaces::ConfigItem::SMS_APP;
      break;
    case aidlimports::ConfigItem::VVM_APP:
      outConfigItem = qcril::interfaces::ConfigItem::VVM_APP;
      break;
    case aidlimports::ConfigItem::VOICE_OVER_WIFI_ROAMING_MODE:
      outConfigItem = qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ROAMING_MODE;
      break;
    case aidlimports::ConfigItem::SET_AUTO_REJECT_CALL_MODE_CONFIG:
      outConfigItem = qcril::interfaces::ConfigItem::AUTO_REJECT_CALL_ENABLED;
      break;
    case aidlimports::ConfigItem::MMTEL_CALL_COMPOSER_CONFIG:
      outConfigItem = qcril::interfaces::ConfigItem::CALL_COMPOSER_SETTING;
      break;
    case aidlimports::ConfigItem::VOWIFI_ENTITLEMENT_ID:
      outConfigItem = qcril::interfaces::ConfigItem::VOWIFI_ENTITLEMENT_ID;
      break;
    case aidlimports::ConfigItem::B2C_ENRICHED_CALLING_CONFIG:
      outConfigItem = qcril::interfaces::ConfigItem::B2C_ENRICHED_CALLING_SETTING;
      break;
    case aidlimports::ConfigItem::DATA_CHANNEL:
      outConfigItem = qcril::interfaces::ConfigItem::IMS_DATA_CHANNEL_SETTING;
      break;
    case aidlimports::ConfigItem::VOLTE_PROVISIONING_RESTRICT_HOME:
      outConfigItem = qcril::interfaces::ConfigItem::VOLTE_PROVISIONING_RESTRICT_HOME;
      break;
    case aidlimports::ConfigItem::VOLTE_PROVISIONING_RESTRICT_ROAMING:
      outConfigItem = qcril::interfaces::ConfigItem::VOLTE_PROVISIONING_RESTRICT_ROAMING;
      break;
    default:
      outConfigItem = qcril::interfaces::ConfigItem::INVALID_CONFIG;
      break;
  }
  return outConfigItem;
}

qcril::interfaces::ConfigFailureCause convertToRilConfigErrorCause(
    const aidlimports::ConfigFailureCause in) {
  qcril::interfaces::ConfigFailureCause outErrorCause =
      qcril::interfaces::ConfigFailureCause::FAILURE_INVALID;
  switch (in) {
    case aidlimports::ConfigFailureCause::NO_ERR:
      outErrorCause = qcril::interfaces::ConfigFailureCause::NO_ERR;
      break;
    case aidlimports::ConfigFailureCause::IMS_NOT_READY:
      outErrorCause = qcril::interfaces::ConfigFailureCause::IMS_NOT_READY;
      break;
    case aidlimports::ConfigFailureCause::FILE_NOT_AVAILABLE:
      outErrorCause = qcril::interfaces::ConfigFailureCause::FILE_NOT_AVAILABLE;
      break;
    case aidlimports::ConfigFailureCause::READ_FAILED:
      outErrorCause = qcril::interfaces::ConfigFailureCause::READ_FAILED;
      break;
    case aidlimports::ConfigFailureCause::WRITE_FAILED:
      outErrorCause = qcril::interfaces::ConfigFailureCause::WRITE_FAILED;
      break;
    case aidlimports::ConfigFailureCause::OTHER_INTERNAL_ERR:
      outErrorCause = qcril::interfaces::ConfigFailureCause::OTHER_INTERNAL_ERR;
      break;
    default:
      outErrorCause = qcril::interfaces::ConfigFailureCause::FAILURE_INVALID;
      break;
  }
  return outErrorCause;
}

void convertToRilConfigInfo(const aidlimports::ConfigInfo& inConfig,
                            qcril::interfaces::ConfigInfo& outConfig) {
  // config_item
  qcril::interfaces::ConfigItem item = convertToRilConfigItem(inConfig.item);
  if (item != qcril::interfaces::ConfigItem::INVALID_CONFIG) {
    outConfig.setItem(item);
  }

  if (inConfig.hasBoolValue) {
    outConfig.setBoolValue(inConfig.boolValue);
  }
  if (inConfig.intValue != INT32_MAX) {
    outConfig.setIntValue(inConfig.intValue);
  }
  if (!inConfig.stringValue.empty()) {
    outConfig.setStringValue(inConfig.stringValue.c_str());
  }
  qcril::interfaces::ConfigFailureCause cause = convertToRilConfigErrorCause(inConfig.errorCause);
  if (cause != qcril::interfaces::ConfigFailureCause::FAILURE_INVALID) {
    outConfig.setErrorCause(cause);
  }
}

aidlimports::ConfigItem convertToAidlConfigItem(qcril::interfaces::ConfigItem inConfigItem) {
  switch (inConfigItem) {
    case qcril::interfaces::ConfigItem::NONE:
      return aidlimports::ConfigItem::NONE;
    case qcril::interfaces::ConfigItem::VOCODER_AMRMODESET:
      return aidlimports::ConfigItem::VOCODER_AMRMODESET;
    case qcril::interfaces::ConfigItem::VOCODER_AMRWBMODESET:
      return aidlimports::ConfigItem::VOCODER_AMRWBMODESET;
    case qcril::interfaces::ConfigItem::SIP_SESSION_TIMER:
      return aidlimports::ConfigItem::SIP_SESSION_TIMER;
    case qcril::interfaces::ConfigItem::MIN_SESSION_EXPIRY:
      return aidlimports::ConfigItem::MIN_SESSION_EXPIRY;
    case qcril::interfaces::ConfigItem::CANCELLATION_TIMER:
      return aidlimports::ConfigItem::CANCELLATION_TIMER;
    case qcril::interfaces::ConfigItem::T_DELAY:
      return aidlimports::ConfigItem::T_DELAY;
    case qcril::interfaces::ConfigItem::SILENT_REDIAL_ENABLE:
      return aidlimports::ConfigItem::SILENT_REDIAL_ENABLE;
    case qcril::interfaces::ConfigItem::SIP_T1_TIMER:
      return aidlimports::ConfigItem::SIP_T1_TIMER;
    case qcril::interfaces::ConfigItem::SIP_T2_TIMER:
      return aidlimports::ConfigItem::SIP_T2_TIMER;
    case qcril::interfaces::ConfigItem::SIP_TF_TIMER:
      return aidlimports::ConfigItem::SIP_TF_TIMER;
    case qcril::interfaces::ConfigItem::VLT_SETTING_ENABLED:
      return aidlimports::ConfigItem::VLT_SETTING_ENABLED;
    case qcril::interfaces::ConfigItem::LVC_SETTING_ENABLED:
      return aidlimports::ConfigItem::LVC_SETTING_ENABLED;
    case qcril::interfaces::ConfigItem::DOMAIN_NAME:
      return aidlimports::ConfigItem::DOMAIN_NAME;
    case qcril::interfaces::ConfigItem::SMS_FORMAT:
      return aidlimports::ConfigItem::SMS_FORMAT;
    case qcril::interfaces::ConfigItem::SMS_OVER_IP:
      return aidlimports::ConfigItem::SMS_OVER_IP;
    case qcril::interfaces::ConfigItem::PUBLISH_TIMER:
      return aidlimports::ConfigItem::PUBLISH_TIMER;
    case qcril::interfaces::ConfigItem::PUBLISH_TIMER_EXTENDED:
      return aidlimports::ConfigItem::PUBLISH_TIMER_EXTENDED;
    case qcril::interfaces::ConfigItem::CAPABILITIES_CACHE_EXPIRATION:
      return aidlimports::ConfigItem::CAPABILITIES_CACHE_EXPIRATION;
    case qcril::interfaces::ConfigItem::AVAILABILITY_CACHE_EXPIRATION:
      return aidlimports::ConfigItem::AVAILABILITY_CACHE_EXPIRATION;
    case qcril::interfaces::ConfigItem::CAPABILITIES_POLL_INTERVAL:
      return aidlimports::ConfigItem::CAPABILITIES_POLL_INTERVAL;
    case qcril::interfaces::ConfigItem::SOURCE_THROTTLE_PUBLISH:
      return aidlimports::ConfigItem::SOURCE_THROTTLE_PUBLISH;
    case qcril::interfaces::ConfigItem::MAX_NUM_ENTRIES_IN_RCL:
      return aidlimports::ConfigItem::MAX_NUM_ENTRIES_IN_RCL;
    case qcril::interfaces::ConfigItem::CAPAB_POLL_LIST_SUB_EXP:
      return aidlimports::ConfigItem::CAPAB_POLL_LIST_SUB_EXP;
    case qcril::interfaces::ConfigItem::GZIP_FLAG:
      return aidlimports::ConfigItem::GZIP_FLAG;
    case qcril::interfaces::ConfigItem::EAB_SETTING_ENABLED:
      return aidlimports::ConfigItem::EAB_SETTING_ENABLED;
    case qcril::interfaces::ConfigItem::MOBILE_DATA_ENABLED:
      return aidlimports::ConfigItem::MOBILE_DATA_ENABLED;
    case qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ENABLED:
      return aidlimports::ConfigItem::VOICE_OVER_WIFI_ENABLED;
    case qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ROAMING:
      return aidlimports::ConfigItem::VOICE_OVER_WIFI_ROAMING;
    case qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_MODE:
      return aidlimports::ConfigItem::VOICE_OVER_WIFI_MODE;
    case qcril::interfaces::ConfigItem::CAPABILITY_DISCOVERY_ENABLED:
      return aidlimports::ConfigItem::CAPABILITY_DISCOVERY_ENABLED;
    case qcril::interfaces::ConfigItem::EMERGENCY_CALL_TIMER:
      return aidlimports::ConfigItem::EMERGENCY_CALL_TIMER;
    case qcril::interfaces::ConfigItem::SSAC_HYSTERESIS_TIMER:
      return aidlimports::ConfigItem::SSAC_HYSTERESIS_TIMER;
    case qcril::interfaces::ConfigItem::VOLTE_USER_OPT_IN_STATUS:
      return aidlimports::ConfigItem::VOLTE_USER_OPT_IN_STATUS;
    case qcril::interfaces::ConfigItem::LBO_PCSCF_ADDRESS:
      return aidlimports::ConfigItem::LBO_PCSCF_ADDRESS;
    case qcril::interfaces::ConfigItem::KEEP_ALIVE_ENABLED:
      return aidlimports::ConfigItem::KEEP_ALIVE_ENABLED;
    case qcril::interfaces::ConfigItem::REGISTRATION_RETRY_BASE_TIME_SEC:
      return aidlimports::ConfigItem::REGISTRATION_RETRY_BASE_TIME_SEC;
    case qcril::interfaces::ConfigItem::REGISTRATION_RETRY_MAX_TIME_SEC:
      return aidlimports::ConfigItem::REGISTRATION_RETRY_MAX_TIME_SEC;
    case qcril::interfaces::ConfigItem::SPEECH_START_PORT:
      return aidlimports::ConfigItem::SPEECH_START_PORT;
    case qcril::interfaces::ConfigItem::SPEECH_END_PORT:
      return aidlimports::ConfigItem::SPEECH_END_PORT;
    case qcril::interfaces::ConfigItem::SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
      return aidlimports::ConfigItem::SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
    case qcril::interfaces::ConfigItem::SIP_INVITE_RSP_WAIT_TIME_MSEC:
      return aidlimports::ConfigItem::SIP_INVITE_RSP_WAIT_TIME_MSEC;
    case qcril::interfaces::ConfigItem::SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
      return aidlimports::ConfigItem::SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
    case qcril::interfaces::ConfigItem::SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
      return aidlimports::ConfigItem::SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
    case qcril::interfaces::ConfigItem::SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
      return aidlimports::ConfigItem::SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
    case qcril::interfaces::ConfigItem::SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
      return aidlimports::ConfigItem::SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
    case qcril::interfaces::ConfigItem::SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
      return aidlimports::ConfigItem::SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
    case qcril::interfaces::ConfigItem::SIP_ACK_RETX_WAIT_TIME_MSEC:
      return aidlimports::ConfigItem::SIP_ACK_RETX_WAIT_TIME_MSEC;
    case qcril::interfaces::ConfigItem::SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
      return aidlimports::ConfigItem::SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
    case qcril::interfaces::ConfigItem::SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
      return aidlimports::ConfigItem::SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
    case qcril::interfaces::ConfigItem::AMR_WB_OCTET_ALIGNED_PT:
      return aidlimports::ConfigItem::AMR_WB_OCTET_ALIGNED_PT;
    case qcril::interfaces::ConfigItem::AMR_WB_BANDWIDTH_EFFICIENT_PT:
      return aidlimports::ConfigItem::AMR_WB_BANDWIDTH_EFFICIENT_PT;
    case qcril::interfaces::ConfigItem::AMR_OCTET_ALIGNED_PT:
      return aidlimports::ConfigItem::AMR_OCTET_ALIGNED_PT;
    case qcril::interfaces::ConfigItem::AMR_BANDWIDTH_EFFICIENT_PT:
      return aidlimports::ConfigItem::AMR_BANDWIDTH_EFFICIENT_PT;
    case qcril::interfaces::ConfigItem::DTMF_WB_PT:
      return aidlimports::ConfigItem::DTMF_WB_PT;
    case qcril::interfaces::ConfigItem::DTMF_NB_PT:
      return aidlimports::ConfigItem::DTMF_NB_PT;
    case qcril::interfaces::ConfigItem::AMR_DEFAULT_MODE:
      return aidlimports::ConfigItem::AMR_DEFAULT_MODE;
    case qcril::interfaces::ConfigItem::SMS_PSI:
      return aidlimports::ConfigItem::SMS_PSI;
    case qcril::interfaces::ConfigItem::VIDEO_QUALITY:
      return aidlimports::ConfigItem::VIDEO_QUALITY;
    case qcril::interfaces::ConfigItem::THRESHOLD_LTE1:
      return aidlimports::ConfigItem::THRESHOLD_LTE1;
    case qcril::interfaces::ConfigItem::THRESHOLD_LTE2:
      return aidlimports::ConfigItem::THRESHOLD_LTE2;
    case qcril::interfaces::ConfigItem::THRESHOLD_LTE3:
      return aidlimports::ConfigItem::THRESHOLD_LTE3;
    case qcril::interfaces::ConfigItem::THRESHOLD_1x:
      return aidlimports::ConfigItem::THRESHOLD_1x;
    case qcril::interfaces::ConfigItem::THRESHOLD_WIFI_A:
      return aidlimports::ConfigItem::THRESHOLD_WIFI_A;
    case qcril::interfaces::ConfigItem::THRESHOLD_WIFI_B:
      return aidlimports::ConfigItem::THRESHOLD_WIFI_B;
    case qcril::interfaces::ConfigItem::T_EPDG_LTE:
      return aidlimports::ConfigItem::T_EPDG_LTE;
    case qcril::interfaces::ConfigItem::T_EPDG_WIFI:
      return aidlimports::ConfigItem::T_EPDG_WIFI;
    case qcril::interfaces::ConfigItem::T_EPDG_1x:
      return aidlimports::ConfigItem::T_EPDG_1x;
    case qcril::interfaces::ConfigItem::VWF_SETTING_ENABLED:
      return aidlimports::ConfigItem::VWF_SETTING_ENABLED;
    case qcril::interfaces::ConfigItem::VCE_SETTING_ENABLED:
      return aidlimports::ConfigItem::VCE_SETTING_ENABLED;
    case qcril::interfaces::ConfigItem::RTT_SETTING_ENABLED:
      return aidlimports::ConfigItem::RTT_SETTING_ENABLED;
    case qcril::interfaces::ConfigItem::SMS_APP:
      return aidlimports::ConfigItem::SMS_APP;
    case qcril::interfaces::ConfigItem::VVM_APP:
      return aidlimports::ConfigItem::VVM_APP;
    case qcril::interfaces::ConfigItem::AUTO_REJECT_CALL_ENABLED:
      return aidlimports::ConfigItem::SET_AUTO_REJECT_CALL_MODE_CONFIG;
    case qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ROAMING_MODE:
      return aidlimports::ConfigItem::VOICE_OVER_WIFI_ROAMING_MODE;
    case qcril::interfaces::ConfigItem::CALL_COMPOSER_SETTING:
      return aidlimports::ConfigItem::MMTEL_CALL_COMPOSER_CONFIG;
    case qcril::interfaces::ConfigItem::VOWIFI_ENTITLEMENT_ID:
      return aidlimports::ConfigItem::VOWIFI_ENTITLEMENT_ID;
    case qcril::interfaces::ConfigItem::B2C_ENRICHED_CALLING_SETTING:
      return aidlimports::ConfigItem::B2C_ENRICHED_CALLING_CONFIG;
    case qcril::interfaces::ConfigItem::IMS_DATA_CHANNEL_SETTING:
      return aidlimports::ConfigItem::DATA_CHANNEL;
    case qcril::interfaces::ConfigItem::VOLTE_PROVISIONING_RESTRICT_HOME:
      return aidlimports::ConfigItem::VOLTE_PROVISIONING_RESTRICT_HOME;
    case qcril::interfaces::ConfigItem::VOLTE_PROVISIONING_RESTRICT_ROAMING:
      return aidlimports::ConfigItem::VOLTE_PROVISIONING_RESTRICT_ROAMING;
    default:
      return aidlimports::ConfigItem::INVALID;
  }
}

aidlimports::ConfigFailureCause convertToAidlConfigErrorCause(
    qcril::interfaces::ConfigFailureCause in) {
  switch (in) {
    case qcril::interfaces::ConfigFailureCause::NO_ERR:
      return aidlimports::ConfigFailureCause::NO_ERR;
    case qcril::interfaces::ConfigFailureCause::IMS_NOT_READY:
      return aidlimports::ConfigFailureCause::IMS_NOT_READY;
    case qcril::interfaces::ConfigFailureCause::FILE_NOT_AVAILABLE:
      return aidlimports::ConfigFailureCause::FILE_NOT_AVAILABLE;
    case qcril::interfaces::ConfigFailureCause::READ_FAILED:
      return aidlimports::ConfigFailureCause::READ_FAILED;
    case qcril::interfaces::ConfigFailureCause::WRITE_FAILED:
      return aidlimports::ConfigFailureCause::WRITE_FAILED;
    case qcril::interfaces::ConfigFailureCause::OTHER_INTERNAL_ERR:
      return aidlimports::ConfigFailureCause::OTHER_INTERNAL_ERR;
    default:
      return aidlimports::ConfigFailureCause::INVALID;
  }
}

void convertToAidlConfigInfo(aidlimports::ConfigInfo& outConfig,
                             const qcril::interfaces::ConfigInfo& inConfig) {
  if (inConfig.hasItem()) {
    outConfig.item = convertToAidlConfigItem(inConfig.getItem());
  } else {
    outConfig.item = aidlimports::ConfigItem::INVALID;
  }
  outConfig.hasBoolValue = inConfig.hasBoolValue();
  if (inConfig.hasBoolValue()) {
    outConfig.boolValue = inConfig.getBoolValue();
  }
  outConfig.intValue = (inConfig.hasIntValue()) ? inConfig.getIntValue() : INT32_MAX;
  if (inConfig.hasStringValue()) {
    outConfig.stringValue = inConfig.getStringValue().c_str();
  }
  outConfig.errorCause = (inConfig.hasErrorCause())
                             ? convertToAidlConfigErrorCause(inConfig.getErrorCause())
                             : aidlimports::ConfigFailureCause::INVALID;
}

aidlimports::ClipStatus convertToAidlClipStatus(const qcril::interfaces::ClipStatus& in) {
  switch (in) {
    case qcril::interfaces::ClipStatus::NOT_PROVISIONED:
      return aidlimports::ClipStatus::NOT_PROVISIONED;
    case qcril::interfaces::ClipStatus::PROVISIONED:
      return aidlimports::ClipStatus::PROVISIONED;
    case qcril::interfaces::ClipStatus::STATUS_UNKNOWN:
      return aidlimports::ClipStatus::STATUS_UNKNOWN;
    case qcril::interfaces::ClipStatus::UNKNOWN:
    default:
      return aidlimports::ClipStatus::INVALID;
  }
}

bool convertToAidlClipInfo(aidlimports::ClipProvisionStatus& out,
                           const std::shared_ptr<qcril::interfaces::ClipInfo>& in) {
  if (in == nullptr) {
    return false;
  }
  out.clipStatus = convertToAidlClipStatus(in->getClipStatus());
  if (in->hasErrorDetails() && in->getErrorDetails() != nullptr) {
    convertToAidlSipErrorInfo(out.errorDetails, in->getErrorDetails());
  }
  return true;
}

bool convertToAidlVerstatInfo(aidlimports::VerstatInfo& out,
                              const std::shared_ptr<qcril::interfaces::VerstatInfo>& in) {
  if (in == nullptr) {
    return false;
  }
  if (in->hasCanMarkUnwantedCall()) {
    out.canMarkUnwantedCall = in->getCanMarkUnwantedCall();
  }
  if (in->hasVerificationStatus()) {
    out.verificationStatus = convertToAidlVerificationStatus(in->getVerificationStatus());
  }
  return true;
}

aidlimports::TirMode convertToAidlTirMode(const qcril::interfaces::TirMode& in) {
  switch (in) {
    case qcril::interfaces::TirMode::TEMPORARY:
      return aidlimports::TirMode::TEMPORARY;
    case qcril::interfaces::TirMode::PERMANENT:
      return aidlimports::TirMode::PERMANENT;
    default:
      return aidlimports::TirMode::INVALID;
  }
}

void convertToAidlCrsType(const qcril::interfaces::CrsType& in, aidlimports::CrsType& crsType) {
  memset(&crsType, 0, sizeof(crsType));
  switch (in) {
    case qcril::interfaces::CrsType::AUDIO:
      crsType = aidlimports::CrsType::AUDIO;
      break;
    case qcril::interfaces::CrsType::VIDEO:
      crsType = aidlimports::CrsType::VIDEO;
      break;
    case qcril::interfaces::CrsType::VIDEO_AND_AUDIO:
      crsType = aidlimports::CrsType::AUDIO_VIDEO;
      break;
    case qcril::interfaces::CrsType::INVALID:
    default:
      crsType = aidlimports::CrsType::INVALID;
      break;
  }
}

aidlimports::CallProgressInfoType convertToAidlCallProgressInfoType(
    const qcril::interfaces::CallProgressInfoType in) {
  switch (in) {
    case qcril::interfaces::CallProgressInfoType::CALL_REJ_Q850:
      return aidlimports::CallProgressInfoType::CALL_REJ_Q850;
    case qcril::interfaces::CallProgressInfoType::CALL_WAITING:
      return aidlimports::CallProgressInfoType::CALL_WAITING;
    case qcril::interfaces::CallProgressInfoType::CALL_FORWARDING:
      return aidlimports::CallProgressInfoType::CALL_FORWARDING;
    case qcril::interfaces::CallProgressInfoType::REMOTE_AVAILABLE:
      return aidlimports::CallProgressInfoType::REMOTE_AVAILABLE;
    case qcril::interfaces::CallProgressInfoType::UNKNOWN:
    default:
      return aidlimports::CallProgressInfoType::INVALID;
  }
  return aidlimports::CallProgressInfoType::INVALID;
}

bool convertToAidlCallProgressInfo(aidlimports::CallProgressInfo& out,
                                   const qcril::interfaces::CallProgressInfo& in) {
  out = { .type = aidlimports::CallProgressInfoType::INVALID,
          .reasonCode = INT16_MAX,
          .reasonText = "" };
  if (in.hasType()) {
    out.type = convertToAidlCallProgressInfoType(in.getType());
  }
  if (in.hasReasonCode()) {
    out.reasonCode = in.getReasonCode();
  }
  if (in.hasReasonText()) {
    out.reasonText = in.getReasonText();
  }
  return true;
}

aidlimports::MsimAdditionalInfoCode convertToAidlMsimAdditionalInfoCode(
    const qcril::interfaces::MsimAdditionalInfoCode in) {
  switch (in) {
    case qcril::interfaces::MsimAdditionalInfoCode::CONCURRENT_CALL_NOT_POSSIBLE:
      return aidlimports::MsimAdditionalInfoCode::CONCURRENT_CALL_NOT_POSSIBLE;
    case qcril::interfaces::MsimAdditionalInfoCode::NONE:
    default:
      return aidlimports::MsimAdditionalInfoCode::NONE;
  }
  return aidlimports::MsimAdditionalInfoCode::NONE;
}

bool convertToAidlMsimAdditionalCallInfo(aidlimports::MsimAdditionalCallInfo& out,
                                         const qcril::interfaces::MsimAdditionalCallInfo& in) {
  out = { .additionalCode = aidlimports::MsimAdditionalInfoCode::NONE };
  if (in.hasAdditionalCode()) {
    out.additionalCode = convertToAidlMsimAdditionalInfoCode(in.getAdditionalCode());
  }
  return true;
}

bool convertToAidlCallInfo(aidlimports::CallInfo& out, const qcril::interfaces::CallInfo& in) {
  if (in.hasCallState()) {
    out.state = convertToAidlCallState(in.getCallState());
  } else {
    out.state = aidlimports::CallState::INVALID;
  }

  if (in.hasIndex()) {
    out.index = in.getIndex();
  } else {
    out.index = INT32_MAX;
  }

  if (in.hasToa()) {
    out.toa = in.getToa();
  } else {
    out.toa = INT32_MAX;
  }

  if (in.hasIsMpty()) {
    out.isMpty = in.getIsMpty();
  }

  if (in.hasIsMt()) {
    out.isMT = in.getIsMt();
  }

  if (in.hasAls()) {
    out.als = in.getAls();
  } else {
    out.als = INT32_MAX;
  }

  if (in.hasIsVoice()) {
    out.isVoice = in.getIsVoice();
  }

  if (in.hasIsVoicePrivacy()) {
    out.isVoicePrivacy = in.getIsVoicePrivacy();
  }

  if (in.hasNumber() && in.getNumber().size() > 0) {
    out.number = in.getNumber();
  }

  if (in.hasNumberPresentation()) {
    out.numberPresentation = in.getNumberPresentation();
  } else {
    out.numberPresentation = INT32_MAX;
  }

  if (in.hasName() && in.getName().size() > 0) {
    out.name = in.getName();
  }

  if (in.hasNamePresentation()) {
    out.namePresentation = in.getNamePresentation();
  } else {
    out.namePresentation = INT32_MAX;
  }

  // Call Details
  if (in.hasCallType()) {
    out.callDetails.callType = convertToAidlCallType(in.getCallType());
  } else {
    out.callDetails.callType = aidlimports::CallType::UNKNOWN;
  }

  if (in.hasCallDomain()) {
    out.callDetails.callDomain = convertToAidlCallDomain(in.getCallDomain());
  } else {
    out.callDetails.callDomain = aidlimports::CallDomain::INVALID;
  }

  std::vector<std::string> extras;
  if (in.hasCodec()) {
    std::string codec = getCodecStringExtra(static_cast<uint32_t>(in.getCodec()));
    if (codec.size()) {
      extras.push_back(codec);
    }
  }
  if (in.hasDisplayText() && in.getDisplayText().size()) {
    extras.push_back("DisplayText=" + in.getDisplayText());
  }
  if (in.hasAdditionalCallInfo() && in.getAdditionalCallInfo().size()) {
    extras.push_back("AdditionalCallInfo=" + in.getAdditionalCallInfo());
  }
  if (in.hasChildNumber() && in.getChildNumber().size()) {
    extras.push_back("ChildNum=" + in.getChildNumber());
  }
  if (in.hasEmergencyServiceCategory()) {
    extras.push_back("EmergencyServiceCategory=" + std::to_string(in.getEmergencyServiceCategory()));
  }

  out.callDetails.extras.resize(0);
  if (extras.size()) {
    out.callDetails.extras.resize(extras.size());
    for (uint32_t i = 0; i < extras.size(); ++i) {
      out.callDetails.extras[i] = extras[i];
    }
  }

  convertRilServiceStatusInfoListToAidlResponse(out.callDetails.localAbility, in.getLocalAbility());

  convertRilServiceStatusInfoListToAidlResponse(out.callDetails.peerAbility, in.getPeerAbility());

  if (in.hasCallSubState()) {
    out.callDetails.callSubstate = in.getCallSubState();
  } else {
    out.callDetails.callSubstate = INT32_MAX;
  }

  if (in.hasMediaId()) {
    out.callDetails.mediaId = in.getMediaId();
  } else {
    out.callDetails.mediaId = INT32_MAX;
  }

  if (in.hasCauseCode()) {
    out.callDetails.causeCode = static_cast<uint32_t>(in.getCauseCode());
  } else {
    out.callDetails.causeCode = INT32_MAX;
  }

  if (in.hasRttMode()) {
    out.callDetails.rttMode = convertToAidlRttMode(in.getRttMode());
  } else {
    out.callDetails.rttMode = aidlimports::RttMode::INVALID;
  }

  if (in.hasSipAlternateUri() && in.getSipAlternateUri().size() > 0) {
    out.callDetails.sipAlternateUri = in.getSipAlternateUri();
  }

  if (in.hasVosSupport()) {
    out.callDetails.isVosSupported = in.getVosSupport();
  }

  //=====

  if (in.hasCallFailCauseResponse()) {
    convertToAidlCallFailCauseResponse(out.failCause, in.getCallFailCauseResponse());
  }

  if (in.hasIsEncrypted()) {
    out.isEncrypted = in.getIsEncrypted();
  }

  if (in.hasIsCalledPartyRinging()) {
    out.isCalledPartyRinging = in.getIsCalledPartyRinging();
  }

  if (in.hasIsVideoConfSupported()) {
    out.isVideoConfSupported = in.getIsVideoConfSupported();
  }

  if (in.hasHistoryInfo() && in.getHistoryInfo().size() > 0) {
    out.historyInfo = in.getHistoryInfo();
  }

  if (in.hasVerstatInfo()) {
    convertToAidlVerstatInfo(out.verstatInfo, in.getVerstatInfo());
  } else {
    out.verstatInfo = { .canMarkUnwantedCall = false,
                        .verificationStatus = aidlimports::VerificationStatus::VALIDATION_NONE };
  }

  // Secondary Line Information
  if (in.hasTerminatingNumber() && in.getTerminatingNumber().size() > 0) {
    out.mtMultiLineInfo.msisdn = in.getTerminatingNumber();
    if (in.hasIsSecondary()) {
      out.mtMultiLineInfo.lineType = in.getIsSecondary()
                                         ? aidlimports::MultiIdentityLineType::SECONDARY
                                         : aidlimports::MultiIdentityLineType::PRIMARY;
    }
  }

  // TIR options allowed
  out.tirMode = aidlimports::TirMode::INVALID;
  if (in.hasTirMode()) {
    out.tirMode = convertToAidlTirMode(in.getTirMode());
  }

  // CRS
  if (in.hasSilentUi()) {
    out.isPreparatory = in.getSilentUi();
  }
  out.crsData.originalCallType = aidlimports::CallType::UNKNOWN;
  if (in.hasOriginalCallType()) {
    out.crsData.originalCallType = convertToAidlCallType(in.getOriginalCallType());
  }
  memset(&out.crsData.type, 0, sizeof(out.crsData.type));
  out.crsData.type = aidlimports::CrsType::INVALID;
  if (in.hasCrsType()) {
    convertToAidlCrsType(in.getCrsType(), out.crsData.type);
  }

  // Call Progress Info
  if (in.hasProgressInfo()) {
    convertToAidlCallProgressInfo(out.callProgInfo, in.getProgressInfo());
  }

  // Diversion Info
  if (in.hasDiversionInfo() && in.getDiversionInfo().size() > 0) {
    out.diversionInfo = in.getDiversionInfo();
  }

  // MSIM additional call info
  if (in.hasMsimAdditionalCallInfo()) {
    aidlimports::MsimAdditionalCallInfo msimAdditionalCallInfo;
    if (convertToAidlMsimAdditionalCallInfo(msimAdditionalCallInfo, in.getMsimAdditionalCallInfo())) {
      out.additionalCallInfo = msimAdditionalCallInfo;
    }
  }

  // Audio quality
  aidlimports::AudioQuality audioQuality = { aidlimports::Codec::INVALID,
                                             aidlimports::ComputedAudioQuality::INVALID };
  if (in.hasCodec()) {
    audioQuality.codec = convertToAidlCodec(in.getCodec());
  }
  if (in.hasComputedAudioQuality()) {
    audioQuality.computedAudioQuality =
        convertToAidlComputedAudioQuality(in.getComputedAudioQuality());
  }
  out.audioQuality = audioQuality;

  // Modem Call Id
  if (in.hasModemCallId()) {
    out.modemCallId = in.getModemCallId();
  } else {
    out.modemCallId = INT32_MAX;
  }

  // Call reason
  if (in.hasCallReason()) {
    out.callReason = in.getCallReason();
  }

  return true;
}  // convertToAidlCallInfo

bool convertToAidlCallInfoList(std::vector<aidlimports::CallInfo>& out,
                               const std::vector<qcril::interfaces::CallInfo>& in) {
  uint32_t count = in.size();
  out.reserve(count);
  for (uint32_t i = 0; i < count; ++i) {
    if (in[i].hasCallDomain() && in[i].getCallDomain() != qcril::interfaces::CallDomain::CS) {
      aidlimports::CallInfo callInfo{};
      convertToAidlCallInfo(callInfo, in[i]);
      out.push_back(std::move(callInfo));
    }
  }
  return true;
}  // convertToAidlCallInfoList

std::string getExtra(std::string key, const std::vector<std::string>& extras) {
  std::string value;
  if (!key.empty()) {
    for (size_t i = 0; i < extras.size(); i++) {
      std::string extra = extras[i];
      if (extra.find(key) != std::string::npos && extra.find("=") != std::string::npos) {
        value = extra.substr(extra.find("=") + 1);
      }
    }
  }
  return value;
}

std::shared_ptr<QcRilRequestDialMessage> makeQcRilRequestDialMessage(
    std::shared_ptr<ImsRadioContext> ctx, const aidlimports::DialRequest& dialRequest) {
  // address is mandatory for normal call, but not mandatory for conference calls
  if (dialRequest.address.empty() && !(dialRequest.isConferenceUri)) {
    QCRIL_LOG_ERROR("Invalid argument: address is empty");
    return nullptr;
  }
  auto msg = std::make_shared<QcRilRequestDialMessage>(ctx);
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocated memory");
    return nullptr;
  }
  msg->setIsImsRequest(true);
  // Set parameters
  if (!dialRequest.address.empty()) {
    msg->setAddress(dialRequest.address);
  }

  if (dialRequest.clirMode != aidlimports::ClirMode::INVALID) {
    msg->setClir(convertToRilClirMode(dialRequest.clirMode));
  }
  if (dialRequest.callDetails.callType != aidlimports::CallType::UNKNOWN) {
    msg->setCallType(convertToRilCallType(dialRequest.callDetails.callType));
  }
  if (dialRequest.callDetails.callDomain != aidlimports::CallDomain::INVALID) {
    msg->setCallDomain(convertToRilCallDomain(dialRequest.callDetails.callDomain));
  }
  if (dialRequest.callDetails.rttMode != aidlimports::RttMode::INVALID) {
    msg->setRttMode(convertToRilRttMode(dialRequest.callDetails.rttMode));
  }

  if (dialRequest.callDetails.extras.size()) {
    std::string displayText = getExtra("DisplayText", dialRequest.callDetails.extras);
    if (!displayText.empty()) {
      msg->setDisplayText(displayText);
    }
  }

  msg->setIsConferenceUri(dialRequest.isConferenceUri);
  msg->setIsCallPull(dialRequest.isCallPull);
  msg->setIsEncrypted(dialRequest.isEncrypted);

  if (!dialRequest.multiLineInfo.msisdn.empty()) {
    msg->setOriginatingNumber(dialRequest.multiLineInfo.msisdn);
    msg->setIsSecondary(
        (dialRequest.multiLineInfo.lineType == aidlimports::MultiIdentityLineType::SECONDARY));
  }
  if (dialRequest.redialInfo.callFailReason != aidlimports::CallFailCause::ERROR_UNSPECIFIED) {
    msg->setRetryCallFailReason(convertToRilCallFailCause(dialRequest.redialInfo.callFailReason));
  }
  if (dialRequest.redialInfo.callFailRadioTech != aidlimports::RadioTechType::INVALID) {
    msg->setRetryCallFailMode(convertToRilRadioTech(dialRequest.redialInfo.callFailRadioTech));
  }
  return msg;
}  // makeQcRilRequestDialMessage

int convertToRilMultiIdentityRegistrationStatus(aidlimports::MultiIdentityRegistrationStatus status) {
  switch (status) {
    case aidlimports::MultiIdentityRegistrationStatus::ENABLE:
      return qcril::interfaces::MultiIdentityInfo::REGISTRATION_STATUS_ACTIVE;
    case aidlimports::MultiIdentityRegistrationStatus::DISABLE:
      return qcril::interfaces::MultiIdentityInfo::REGISTRATION_STATUS_INACTIVE;
    default:
      return qcril::interfaces::MultiIdentityInfo::REGISTRATION_STATUS_UNKNOWN;
  }
}

int convertToRilMultiIdentityLineType(aidlimports::MultiIdentityLineType type) {
  switch (type) {
    case aidlimports::MultiIdentityLineType::PRIMARY:
      return qcril::interfaces::MultiIdentityInfo::LINE_TYPE_PRIMARY;
    case aidlimports::MultiIdentityLineType::SECONDARY:
      return qcril::interfaces::MultiIdentityInfo::LINE_TYPE_SECONDARY;
    default:
      return qcril::interfaces::MultiIdentityInfo::LINE_TYPE_UNKNOWN;
  }
}

void convertToRilMultiIdentityInfo(const aidlimports::MultiIdentityLineInfo& in_line,
                                   qcril::interfaces::MultiIdentityInfo& out_line) {
  out_line.msisdn = in_line.msisdn;
  out_line.registrationStatus =
      convertToRilMultiIdentityRegistrationStatus(in_line.registrationStatus);
  out_line.lineType = convertToRilMultiIdentityLineType(in_line.lineType);
}  // convertToRilMultiIdentityInfo

aidlimports::MultiIdentityRegistrationStatus convertToAidlMultiIdentityRegistrationStatus(int status) {
  switch (status) {
    case qcril::interfaces::MultiIdentityInfo::REGISTRATION_STATUS_ACTIVE:
      return aidlimports::MultiIdentityRegistrationStatus::ENABLE;
    case qcril::interfaces::MultiIdentityInfo::REGISTRATION_STATUS_INACTIVE:
      return aidlimports::MultiIdentityRegistrationStatus::DISABLE;
    default:
      return aidlimports::MultiIdentityRegistrationStatus::UNKNOWN;
  }
}

void convertToAidlMultiIdentityInfo(const std::vector<qcril::interfaces::MultiIdentityInfo>& in_lines,
                                    std::vector<aidlimports::MultiIdentityLineInfo>& out_lines) {
  out_lines.resize(in_lines.size());
  for (unsigned i = 0; i < in_lines.size(); i++) {
    auto& in_line = in_lines[i];
    aidlimports::MultiIdentityLineInfo out_line = {};
    out_line.msisdn = in_line.msisdn;
    out_line.registrationStatus =
        convertToAidlMultiIdentityRegistrationStatus(in_line.registrationStatus);
    std::string logStr = ::android::internal::ToString(out_line);
    QCRIL_LOG_INFO("MultiIdentity Registration Info for line[%d] : %s", i, logStr.c_str());
    out_lines[i] = out_line;
  }
}  // convertToAidlMultiIdentityInfo

qcril::interfaces::Priority convertToRilPriority(const aidlimports::CallPriority& in) {
  switch (in) {
    case aidlimports::CallPriority::URGENT:
      return qcril::interfaces::Priority::URGENT;
    default:
      return qcril::interfaces::Priority::NORMAL;
  }
}

qcril::interfaces::Location convertToRilLocation(const aidlimports::CallLocation& in) {
  qcril::interfaces::Location location = {};
  if (in.radius > qcril::interfaces::Location::POINT_LOCATION) {
    location.setRadius(in.radius);
  }
  location.setLatitude(in.latitude);
  location.setLongitude(in.longitude);
  return location;
}

void convertToRilCallComposerInfo(qcril::interfaces::CallComposerInfo& out,
                                  const aidlimports::CallComposerInfo& in) {
  out.setPriority(convertToRilPriority(in.priority));

  if (in.subject.size()) {
    std::vector<uint16_t> outSubject;
    for (size_t i = 0; i < in.subject.size(); i++) {
      outSubject.push_back(static_cast<uint16_t>(in.subject[i]));
    }
    out.setSubject(outSubject);
  }
  if (!in.imageUrl.empty()) {
    out.setImageUrl(in.imageUrl);
  }
  if (in.location.radius > qcril::interfaces::Location::LOCATION_NOT_SET) {
    out.setLocation(convertToRilLocation(in.location));
  }
}  // convertToRilCallComposerInfo

aidlimports::CallPriority convertToAidlCallComposerPriority(const qcril::interfaces::Priority& in) {
  switch (in) {
    case qcril::interfaces::Priority::URGENT:
      return aidlimports::CallPriority::URGENT;
    case qcril::interfaces::Priority::NORMAL:
    default:
      return aidlimports::CallPriority::NORMAL;
  }
}

aidlimports::CallLocation convertToAidlCallComposerLocation(const qcril::interfaces::Location& in) {
  aidlimports::CallLocation out{};
  out.radius = in.getRadius();
  out.latitude = in.getLatitude();
  out.longitude = in.getLongitude();

  return out;
}

void convertToAidlCallComposerInfo(aidlimports::CallComposerInfo& out,
                                   const qcril::interfaces::CallComposerInfo& in) {
  if (in.hasId()) out.callId = in.getId();
  if (in.hasPriority()) {
    out.priority = convertToAidlCallComposerPriority(in.getPriority());
  }

  for (size_t i = 0; i < in.getSubject().size(); i++) {
    out.subject.push_back(in.getSubject()[i]);
  }
  if (in.hasImageUrl()) {
    out.imageUrl = in.getImageUrl();
  }
  out.location.radius = -1;
  if (in.hasLocation()) {
    out.location = convertToAidlCallComposerLocation(in.getLocation());
  }
  out.organization =  std::make_optional<std::vector<char16_t>>();
  for (size_t i = 0; i < in.getOrganization().size(); i++) {
    out.organization->push_back(in.getOrganization()[i]);
  }
}  // convertToAidlCallComposerInfo

uint64_t convertToRilEmergencyServiceCategories(const int32_t categories) {
  // Categories are uninitialized.
  uint64_t rilCategories = 0;

  if (categories & static_cast<int32_t>(aidlimports::EmergencyServiceCategory::POLICE)) {
    rilCategories |= static_cast<uint64_t>(RIL_EccCategoryMask::POLICE);
  }
  if (categories & static_cast<int32_t>(aidlimports::EmergencyServiceCategory::AMBULANCE)) {
    rilCategories |= static_cast<uint64_t>(RIL_EccCategoryMask::AMBULANCE);
  }
  if (categories & static_cast<int32_t>(aidlimports::EmergencyServiceCategory::FIRE_BRIGADE)) {
    rilCategories |= static_cast<uint64_t>(RIL_EccCategoryMask::FIRE_BRIGADE);
  }
  if (categories & static_cast<int32_t>(aidlimports::EmergencyServiceCategory::MARINE_GUARD)) {
    rilCategories |= static_cast<uint64_t>(RIL_EccCategoryMask::MARINE_GUARD);
  }
  if (categories & static_cast<int32_t>(aidlimports::EmergencyServiceCategory::MOUNTAIN_RESCUE)) {
    rilCategories |= static_cast<uint64_t>(RIL_EccCategoryMask::MOUNTAIN_RESCUE);
  }
  if (categories & static_cast<int32_t>(aidlimports::EmergencyServiceCategory::MIEC)) {
    rilCategories |= static_cast<uint64_t>(RIL_EccCategoryMask::MANUAL_ECALL);
  }
  if (categories & static_cast<int32_t>(aidlimports::EmergencyServiceCategory::AIEC)) {
    rilCategories |= static_cast<uint64_t>(RIL_EccCategoryMask::AUTO_ECALL);
  }
  if (rilCategories == 0) {
    rilCategories = static_cast<uint64_t>(RIL_EccCategoryMask::UNKNOWN);
  }
  return rilCategories;
}  // convertToRilEmergencyServiceCategories

EmergencyCallRouting convertToRilEmergencyCallRoute(aidlimports::EmergencyCallRoute route) {
  EmergencyCallRouting rilRoute = EmergencyCallRouting::UNKNOWN;

  if (route == aidlimports::EmergencyCallRoute::EMERGENCY) {
    rilRoute = EmergencyCallRouting::EMERGENCY;
  } else if (route == aidlimports::EmergencyCallRoute::NORMAL) {
    rilRoute = EmergencyCallRouting::NORMAL;
  }
  return rilRoute;
}  // convertToRilEmergencyCallRoute

aidlimports::ServiceClassStatus convertToAidlServiceClassStatus(
    const qcril::interfaces::ServiceClassStatus& in) {
  switch (in) {
    case qcril::interfaces::ServiceClassStatus::DISABLED:
      return aidlimports::ServiceClassStatus::DISABLED;
    case qcril::interfaces::ServiceClassStatus::ENABLED:
      return aidlimports::ServiceClassStatus::ENABLED;
    case qcril::interfaces::ServiceClassStatus::UNKNOWN:
    default:
      return aidlimports::ServiceClassStatus::INVALID;
  }
}

aidlimports::ServiceClassProvisionStatus convertToAidlServiceClassProvisionStatus(
    const qcril::interfaces::ServiceClassProvisionStatus& in) {
  switch (in) {
    case qcril::interfaces::ServiceClassProvisionStatus::NOT_PROVISIONED:
      return aidlimports::ServiceClassProvisionStatus::NOT_PROVISIONED;
    case qcril::interfaces::ServiceClassProvisionStatus::PROVISIONED:
      return aidlimports::ServiceClassProvisionStatus::PROVISIONED;
    case qcril::interfaces::ServiceClassProvisionStatus::UNKNOWN:
    default:
      return aidlimports::ServiceClassProvisionStatus::INVALID;
  }
}

bool convertToAidlColrInfo(aidlimports::ColrInfo& out,
                           const std::shared_ptr<qcril::interfaces::ColrInfo>& in) {
  if (in == nullptr) {
    return false;
  }
  out.status = convertToAidlServiceClassStatus(in->getStatus());
  out.provisionStatus = convertToAidlServiceClassProvisionStatus(in->getProvisionStatus());
  out.presentation = convertToAidlIpPresentation(in->getPresentation());
  if (in->hasErrorDetails() && in->getErrorDetails() != nullptr) {
    convertToAidlSipErrorInfo(out.errorDetails, in->getErrorDetails());
  }
  return true;
}

void convertToAidlVirtualLines(std::vector<std::string>& out, const std::vector<std::string>& in) {
  for (int i = 0; i < in.size(); i++) {
    out.push_back(in[i]);
  }
}  // convertToAidlVirtualLines

aidlimports::CallModifyFailCause convertToAidlCallModifyFailCause(
    const qcril::interfaces::CallModifyFailCause& in) {
  switch (in) {
    case qcril::interfaces::CallModifyFailCause::SUCCESS:
      return aidlimports::CallModifyFailCause::E_SUCCESS;
    case qcril::interfaces::CallModifyFailCause::RADIO_NOT_AVAILABLE:
      return aidlimports::CallModifyFailCause::E_RADIO_NOT_AVAILABLE;
    case qcril::interfaces::CallModifyFailCause::GENERIC_FAILURE:
      return aidlimports::CallModifyFailCause::E_GENERIC_FAILURE;
    case qcril::interfaces::CallModifyFailCause::REQUEST_NOT_SUPPORTED:
      return aidlimports::CallModifyFailCause::E_REQUEST_NOT_SUPPORTED;
    case qcril::interfaces::CallModifyFailCause::CANCELLED:
      return aidlimports::CallModifyFailCause::E_CANCELLED;
    case qcril::interfaces::CallModifyFailCause::UNUSED:
      return aidlimports::CallModifyFailCause::E_UNUSED;
    case qcril::interfaces::CallModifyFailCause::INVALID_PARAMETER:
      return aidlimports::CallModifyFailCause::E_INVALID_PARAMETER;
    case qcril::interfaces::CallModifyFailCause::REJECTED_BY_REMOTE:
      return aidlimports::CallModifyFailCause::E_REJECTED_BY_REMOTE;
    case qcril::interfaces::CallModifyFailCause::IMS_DEREGISTERED:
      return aidlimports::CallModifyFailCause::E_IMS_DEREGISTERED;
    case qcril::interfaces::CallModifyFailCause::NETWORK_NOT_SUPPORTED:
      return aidlimports::CallModifyFailCause::E_NETWORK_NOT_SUPPORTED;
    case qcril::interfaces::CallModifyFailCause::HOLD_RESUME_FAILED:
      return aidlimports::CallModifyFailCause::E_HOLD_RESUME_FAILED;
    case qcril::interfaces::CallModifyFailCause::HOLD_RESUME_CANCELED:
      return aidlimports::CallModifyFailCause::E_HOLD_RESUME_CANCELED;
    case qcril::interfaces::CallModifyFailCause::REINVITE_COLLISION:
      return aidlimports::CallModifyFailCause::E_REINVITE_COLLISION;
    case qcril::interfaces::CallModifyFailCause::UNKNOWN:
    default:
      return aidlimports::CallModifyFailCause::E_INVALID;
  }
}  // convertToAidlCallModifyFailCause

aidlimports::HandoverType convertToAidlHandoverType(const qcril::interfaces::HandoverType& in) {
  switch (in) {
    case qcril::interfaces::HandoverType::START:
      return aidlimports::HandoverType::START;
    case qcril::interfaces::HandoverType::COMPLETE_SUCCESS:
      return aidlimports::HandoverType::COMPLETE_SUCCESS;
    case qcril::interfaces::HandoverType::COMPLETE_FAIL:
      return aidlimports::HandoverType::COMPLETE_FAIL;
    case qcril::interfaces::HandoverType::CANCEL:
      return aidlimports::HandoverType::CANCEL;
    case qcril::interfaces::HandoverType::NOT_TRIGGERED:
      return aidlimports::HandoverType::NOT_TRIGGERED;
    case qcril::interfaces::HandoverType::NOT_TRIGGERED_MOBILE_DATA_OFF:
      return aidlimports::HandoverType::NOT_TRIGGERED_MOBILE_DATA_OFF;
    default:
      return aidlimports::HandoverType::INVALID;
  }
}  // convertToAidlHandoverType

aidlimports::BlockReasonType convertToAidlBlockReasonType(qcril::interfaces::BlockReasonType reason) {
  switch (reason) {
    case qcril::interfaces::BlockReasonType::PDP_FAILURE:
      return aidlimports::BlockReasonType::PDP_FAILURE;
    case qcril::interfaces::BlockReasonType::REGISTRATION_FAILURE:
      return aidlimports::BlockReasonType::REGISTRATION_FAILURE;
    case qcril::interfaces::BlockReasonType::HANDOVER_FAILURE:
      return aidlimports::BlockReasonType::HANDOVER_FAILURE;
    case qcril::interfaces::BlockReasonType::OTHER_FAILURE:
      return aidlimports::BlockReasonType::OTHER_FAILURE;
    default:
      return aidlimports::BlockReasonType::INVALID;
  }
  return aidlimports::BlockReasonType::INVALID;
}

aidlimports::RegFailureReasonType convertToAidlRegFailureReasonType(
    qcril::interfaces::RegFailureReasonType reason) {
  switch (reason) {
    case qcril::interfaces::RegFailureReasonType::UNSPECIFIED:
      return aidlimports::RegFailureReasonType::UNSPECIFIED;
    case qcril::interfaces::RegFailureReasonType::MOBILE_IP:
      return aidlimports::RegFailureReasonType::MOBILE_IP;
    case qcril::interfaces::RegFailureReasonType::INTERNAL:
      return aidlimports::RegFailureReasonType::INTERNAL;
    case qcril::interfaces::RegFailureReasonType::CALL_MANAGER_DEFINED:
      return aidlimports::RegFailureReasonType::CALL_MANAGER_DEFINED;
    case qcril::interfaces::RegFailureReasonType::SPEC_DEFINED:
      return aidlimports::RegFailureReasonType::TYPE_3GPP_SPEC_DEFINED;
    case qcril::interfaces::RegFailureReasonType::PPP:
      return aidlimports::RegFailureReasonType::PPP;
    case qcril::interfaces::RegFailureReasonType::EHRPD:
      return aidlimports::RegFailureReasonType::EHRPD;
    case qcril::interfaces::RegFailureReasonType::IPV6:
      return aidlimports::RegFailureReasonType::IPV6;
    case qcril::interfaces::RegFailureReasonType::IWLAN:
      return aidlimports::RegFailureReasonType::IWLAN;
    case qcril::interfaces::RegFailureReasonType::HANDOFF:
      return aidlimports::RegFailureReasonType::HANDOFF;
    default:
      return aidlimports::RegFailureReasonType::INVALID;
  }
}

void convertToAidlBlockReasonDetails(aidlimports::BlockReasonDetails& outDetails,
                                     const qcril::interfaces::BlockReasonDetails& in) {
  outDetails.regFailureReasonType = aidlimports::RegFailureReasonType::INVALID;
  if (in.hasRegFailureReasonType()) {
    outDetails.regFailureReasonType =
        convertToAidlRegFailureReasonType(in.getRegFailureReasonType());
    outDetails.regFailureReason = in.hasRegFailureReason() ? in.getRegFailureReason() : INT32_MAX;
  }
}

void convertToAidlRegBlockStatus(aidlimports::BlockStatus& outStatus,
                                 const qcril::interfaces::BlockStatus& in) {
  outStatus.blockReason = aidlimports::BlockReasonType::INVALID;
  if (in.hasBlockReason()) {
    outStatus.blockReason = convertToAidlBlockReasonType(in.getBlockReason());
  }
  if (in.hasBlockReasonDetails()) {
    convertToAidlBlockReasonDetails(outStatus.blockReasonDetails, *(in.getBlockReasonDetails()));
  }
}  // convertToAidlRegBlockStatus

aidlimports::ConferenceCallState convertToAidlConferenceCallState(
    const qcril::interfaces::ConferenceCallState& in) {
  switch (in) {
    case qcril::interfaces::ConferenceCallState::RINGING:
      return aidlimports::ConferenceCallState::RINGING;
    case qcril::interfaces::ConferenceCallState::FOREGROUND:
      return aidlimports::ConferenceCallState::FOREGROUND;
    case qcril::interfaces::ConferenceCallState::BACKGROUND:
      return aidlimports::ConferenceCallState::BACKGROUND;
    default:
      return aidlimports::ConferenceCallState::INVALID;
  }
}  // convertToAidlConferenceCallState

aidlimports::ConfParticipantOperation convertToAidlConfParticipantOperation(
    const qcril::interfaces::ConfParticipantOperation& in) {
  switch (in) {
    case qcril::interfaces::ConfParticipantOperation::ADD:
      return aidlimports::ConfParticipantOperation::ADD;
    case qcril::interfaces::ConfParticipantOperation::REMOVE:
      return aidlimports::ConfParticipantOperation::REMOVE;
    case qcril::interfaces::ConfParticipantOperation::UNKNOWN:
    default:
      return aidlimports::ConfParticipantOperation::INVALID;
  }
}  // convertToAidlConfParticipantOperation

bool isUssdOverImsSupported() {
  bool ussdOverImsSupported = false;
  bool bool_config;
  if (qcril_config_get(PERSIST_VENDOR_RADIO_IS_USSD_SUPPORTED, bool_config) == E_SUCCESS) {
    ussdOverImsSupported = bool_config;
  }

  return ussdOverImsSupported;
}  // isUssdOverImsSupported
bool convertToAidlClirInfo(aidlimports::ClirInfo& out,
                           const std::shared_ptr<qcril::interfaces::ClirInfo>& in) {
  if (in == nullptr) {
    return false;
  }
  out.paramM = in->hasPresentation() ? in->getPresentation() : INT32_MAX;
  out.paramN = in->hasAction() ? in->getAction() : INT32_MAX;
  return true;
}

qcril::interfaces::TtyMode convertToRilTtyMode(const aidlimports::TtyMode& in) {
  switch (in) {
    case aidlimports::TtyMode::OFF:
      return qcril::interfaces::TtyMode::MODE_OFF;
    case aidlimports::TtyMode::FULL:
      return qcril::interfaces::TtyMode::FULL;
    case aidlimports::TtyMode::HCO:
      return qcril::interfaces::TtyMode::HCO;
    case aidlimports::TtyMode::VCO:
      return qcril::interfaces::TtyMode::VCO;
    case aidlimports::TtyMode::INVALID:
    default:
      return qcril::interfaces::TtyMode::UNKNOWN;
  }
}

bool convertToAidlCallFwdTimerInfo(aidlimports::CallFwdTimerInfo& out,
                                   const std::shared_ptr<qcril::interfaces::CallFwdTimerInfo>& in) {
  if (in == nullptr) {
    return false;
  }
  out.year = (in->hasYear()) ? in->getYear() : INT32_MAX;
  out.month = (in->hasMonth()) ? in->getMonth() : INT32_MAX;
  out.day = (in->hasDay()) ? in->getDay() : INT32_MAX;
  out.hour = (in->hasHour()) ? in->getHour() : INT32_MAX;
  out.minute = (in->hasMinute()) ? in->getMinute() : INT32_MAX;
  out.second = (in->hasSecond()) ? in->getSecond() : INT32_MAX;
  out.timezone = (in->hasTimezone()) ? in->getTimezone() : INT32_MAX;

  return true;
}

bool convertToAidlCallForwardInfo(aidlimports::CallForwardInfo& out,
                                  const qcril::interfaces::CallForwardInfo& in) {
  if (in.hasStatus()) {
    out.status = in.getStatus();
  }
  if (in.hasReason()) {
    out.reason = in.getReason();
  }
  if (in.hasServiceClass()) {
    out.serviceClass = in.getServiceClass();
  }
  if (in.hasToa()) {
    out.toa = in.getToa();
  }
  if (in.hasNumber() && !in.getNumber().empty()) {
    out.number = in.getNumber().c_str();
  }
  if (in.hasTimeSeconds()) {
    out.timeSeconds = in.getTimeSeconds();
  }
  if (in.hasCallFwdTimerStart() && in.getCallFwdTimerStart() != nullptr) {
    convertToAidlCallFwdTimerInfo(out.callFwdTimerStart, in.getCallFwdTimerStart());
  }
  if (in.hasCallFwdTimerEnd() && in.getCallFwdTimerEnd() != nullptr) {
    convertToAidlCallFwdTimerInfo(out.callFwdTimerEnd, in.getCallFwdTimerEnd());
  }
  return true;
}

bool convertToAidlCallForwardInfoList(std::vector<aidlimports::CallForwardInfo>& out,
                                      const std::vector<qcril::interfaces::CallForwardInfo>& in) {
  uint32_t count = in.size();
  out.resize(count);
  for (uint32_t i = 0; i < count; ++i) {
    out[i] = {.status = INT32_MAX,
              .reason = INT32_MAX,
              .serviceClass = INT32_MAX,
              .toa = INT32_MAX,
              .timeSeconds = INT32_MAX };
    out[i].callFwdTimerStart = {.year = INT32_MAX,
                                .month = INT32_MAX,
                                .day = INT32_MAX,
                                .hour = INT32_MAX,
                                .minute = INT32_MAX,
                                .second = INT32_MAX,
                                .timezone = INT32_MAX };
    out[i].callFwdTimerEnd = {.year = INT32_MAX,
                              .month = INT32_MAX,
                              .day = INT32_MAX,
                              .hour = INT32_MAX,
                              .minute = INT32_MAX,
                              .second = INT32_MAX,
                              .timezone = INT32_MAX };
    convertToAidlCallForwardInfo(out[i], in[i]);
  }
  return true;
}

bool convertToRilCallFwdTimerInfo(std::shared_ptr<qcril::interfaces::CallFwdTimerInfo>& out,
                                  const aidlimports::CallFwdTimerInfo& in) {
  if (out == nullptr) {
    return false;
  }
  if (in.year != INT32_MAX) {
    out->setYear(in.year);
  }
  if (in.month != INT32_MAX) {
    out->setMonth(in.month);
  }
  if (in.day != INT32_MAX) {
    out->setDay(in.day);
  }
  if (in.hour != INT32_MAX) {
    out->setHour(in.hour);
  }
  if (in.minute != INT32_MAX) {
    out->setMinute(in.minute);
  }
  if (in.second != INT32_MAX) {
    out->setSecond(in.second);
  }
  if (in.timezone != INT32_MAX) {
    out->setTimezone(in.timezone);
  }
  return true;
}

qcril::interfaces::ServiceClassStatus convertToRilServiceClassStatus(
    const aidlimports::ServiceClassStatus& in) {
  switch (in) {
    case aidlimports::ServiceClassStatus::DISABLED:
      return qcril::interfaces::ServiceClassStatus::DISABLED;
    case aidlimports::ServiceClassStatus::ENABLED:
      return qcril::interfaces::ServiceClassStatus::ENABLED;
    case aidlimports::ServiceClassStatus::INVALID:
    default:
      return qcril::interfaces::ServiceClassStatus::UNKNOWN;
  }
}

aidlimports::FacilityType convertToAidlFacilityType(const qcril::interfaces::FacilityType& in) {
  switch (in) {
    case qcril::interfaces::FacilityType::CLIP:
      return aidlimports::FacilityType::CLIP;
    case qcril::interfaces::FacilityType::COLP:
      return aidlimports::FacilityType::COLP;
    case qcril::interfaces::FacilityType::BAOC:
      return aidlimports::FacilityType::BAOC;
    case qcril::interfaces::FacilityType::BAOIC:
      return aidlimports::FacilityType::BAOIC;
    case qcril::interfaces::FacilityType::BAOICxH:
      return aidlimports::FacilityType::BAOICxH;
    case qcril::interfaces::FacilityType::BAIC:
      return aidlimports::FacilityType::BAIC;
    case qcril::interfaces::FacilityType::BAICr:
      return aidlimports::FacilityType::BAICr;
    case qcril::interfaces::FacilityType::BA_ALL:
      return aidlimports::FacilityType::BA_ALL;
    case qcril::interfaces::FacilityType::BA_MO:
      return aidlimports::FacilityType::BA_MO;
    case qcril::interfaces::FacilityType::BA_MT:
      return aidlimports::FacilityType::BA_MT;
    case qcril::interfaces::FacilityType::BS_MT:
      return aidlimports::FacilityType::BS_MT;
    case qcril::interfaces::FacilityType::BAICa:
      return aidlimports::FacilityType::BAICa;
    case qcril::interfaces::FacilityType::UNKNOWN:
    default:
      return aidlimports::FacilityType::INVALID;
  }
}

bool convertToAidlNumInfo(aidlimports::CbNumInfo& out,
                          const qcril::interfaces::CallBarringNumbersInfo& in) {
  out.status = aidlimports::ServiceClassStatus::INVALID;
  if (in.hasStatus()) {
    out.status = convertToAidlServiceClassStatus(in.getStatus());
  }
  if (in.hasNumber() && in.getNumber().size() > 0) {
    out.number = in.getNumber().c_str();
  }
  return true;
}

bool convertToAidlCbNumListInfo(aidlimports::CbNumListInfo& out,
                                const qcril::interfaces::CallBarringNumbersListInfo& in) {
  out.serviceClass = INT32_MAX;
  if (in.hasServiceClass()) {
    out.serviceClass = in.getServiceClass();
  }
  if (in.getCallBarringNumbersInfo().size() > 0) {
    uint32_t size = in.getCallBarringNumbersInfo().size();
    out.cbNumInfo.resize(size);
    for (uint32_t i = 0; i < size; i++) {
      convertToAidlNumInfo(out.cbNumInfo[i], in.getCallBarringNumbersInfo()[i]);
    }
  }
  return true;
}

void convertToAidlSuppServiceStatus(
    aidlimports::SuppServiceStatus& out,
    const qcril::interfaces::SuppServiceStatusInfo& in) {
  out.status = aidlimports::ServiceClassStatus::INVALID;
  if (in.hasStatus()) {
    out.status = convertToAidlServiceClassStatus(in.getStatus());
  }
  out.provisionStatus = aidlimports::ServiceClassProvisionStatus::INVALID;
  if (in.hasProvisionStatus()) {
    out.provisionStatus = convertToAidlServiceClassProvisionStatus(in.getProvisionStatus());
  }
  out.facilityType = aidlimports::FacilityType::INVALID;
  if (in.hasFacilityType()) {
    out.facilityType = convertToAidlFacilityType(in.getFacilityType());
  }
  if (in.getCallBarringNumbersListInfo().size() > 0) {
    uint32_t size = in.getCallBarringNumbersListInfo().size();
    out.cbNumListInfo.resize(size);
    for (uint32_t i = 0; i < size; i++) {
      convertToAidlCbNumListInfo(out.cbNumListInfo[i], in.getCallBarringNumbersListInfo()[i]);
    }
  }
  if (in.hasErrorDetails() && in.getErrorDetails() != nullptr) {
    out.hasErrorDetails = convertToAidlSipErrorInfo(out.errorDetails, in.getErrorDetails());
  }

  // Assume isPasswordRequired to be true if we did not receive a valid value from modem.
  // Otherwise use the received value.
  out.isPasswordRequired = ( ! in.hasIsPasswordRequired()) || ( in.getIsPasswordRequired() );
}

qcril::interfaces::EctType convertToRilEctType(const aidlimports::EctType& in) {
  switch (in) {
    case aidlimports::EctType::BLIND_TRANSFER:
      return qcril::interfaces::EctType::BLIND_TRANSFER;
    case aidlimports::EctType::ASSURED_TRANSFER:
      return qcril::interfaces::EctType::ASSURED_TRANSFER;
    case aidlimports::EctType::CONSULTATIVE_TRANSFER:
      return qcril::interfaces::EctType::CONSULTATIVE_TRANSFER;
    case aidlimports::EctType::INVALID:
    default:
      return qcril::interfaces::EctType::UNKNOWN;
  }
}

qcril::interfaces::FacilityType convertToRilFacilityType(const aidlimports::FacilityType& in) {
  switch (in) {
    case aidlimports::FacilityType::CLIP:
      return qcril::interfaces::FacilityType::CLIP;
    case aidlimports::FacilityType::COLP:
      return qcril::interfaces::FacilityType::COLP;
    case aidlimports::FacilityType::BAOC:
      return qcril::interfaces::FacilityType::BAOC;
    case aidlimports::FacilityType::BAOIC:
      return qcril::interfaces::FacilityType::BAOIC;
    case aidlimports::FacilityType::BAOICxH:
      return qcril::interfaces::FacilityType::BAOICxH;
    case aidlimports::FacilityType::BAIC:
      return qcril::interfaces::FacilityType::BAIC;
    case aidlimports::FacilityType::BAICr:
      return qcril::interfaces::FacilityType::BAICr;
    case aidlimports::FacilityType::BA_ALL:
      return qcril::interfaces::FacilityType::BA_ALL;
    case aidlimports::FacilityType::BA_MO:
      return qcril::interfaces::FacilityType::BA_MO;
    case aidlimports::FacilityType::BA_MT:
      return qcril::interfaces::FacilityType::BA_MT;
    case aidlimports::FacilityType::BS_MT:
      return qcril::interfaces::FacilityType::BS_MT;
    case aidlimports::FacilityType::BAICa:
      return qcril::interfaces::FacilityType::BAICa;
    case aidlimports::FacilityType::INVALID:
    default:
      return qcril::interfaces::FacilityType::UNKNOWN;
  }
}

qcril::interfaces::DeliverStatus convertAidlToRilSmsAckResult(
    aidlimports::SmsDeliverStatus smsdeliverResult) {
  switch (smsdeliverResult) {
    case aidlimports::SmsDeliverStatus::OK:
      return qcril::interfaces::DeliverStatus::STATUS_OK;
    case aidlimports::SmsDeliverStatus::ERROR:
      return qcril::interfaces::DeliverStatus::STATUS_ERROR;
    case aidlimports::SmsDeliverStatus::ERROR_NO_MEMORY:
      return qcril::interfaces::DeliverStatus::STATUS_NO_MEMORY;
    case aidlimports::SmsDeliverStatus::ERROR_REQUEST_NOT_SUPPORTED:
      return qcril::interfaces::DeliverStatus::STATUS_REQUEST_NOT_SUPPORTED;
    default:
      return qcril::interfaces::DeliverStatus::STATUS_ERROR;
  }
}

aidlimports::TtyMode convertToAidlTtyMode(const qcril::interfaces::TtyMode& in) {
  switch (in) {
    case qcril::interfaces::TtyMode::MODE_OFF:
      return aidlimports::TtyMode::OFF;
    case qcril::interfaces::TtyMode::FULL:
      return aidlimports::TtyMode::FULL;
    case qcril::interfaces::TtyMode::HCO:
      return aidlimports::TtyMode::HCO;
    case qcril::interfaces::TtyMode::VCO:
      return aidlimports::TtyMode::VCO;
    case qcril::interfaces::TtyMode::UNKNOWN:
    default:
      return aidlimports::TtyMode::INVALID;
  }
}

aidlimports::NotificationType convertToAidlNotificationType(
    const qcril::interfaces::NotificationType& in) {
  switch (in) {
    case qcril::interfaces::NotificationType::MO:
      return aidlimports::NotificationType::MO;
    case qcril::interfaces::NotificationType::MT:
      return aidlimports::NotificationType::MT;
    case qcril::interfaces::NotificationType::UNKNOWN:
    default:
      return aidlimports::NotificationType::INVALID;
  }
}

void convertToAidlVowifiCallQuality(aidlimports::VoWiFiCallQuality& out,
                                    qcril::interfaces::VowifiQuality in) {
  switch (in) {
    case qcril::interfaces::VowifiQuality::EXCELLENT:
      out = aidlimports::VoWiFiCallQuality::EXCELLENT;
      break;
    case qcril::interfaces::VowifiQuality::FAIR:
      out = aidlimports::VoWiFiCallQuality::FAIR;
      break;
    case qcril::interfaces::VowifiQuality::BAD:
      out = aidlimports::VoWiFiCallQuality::BAD;
      break;
    case qcril::interfaces::VowifiQuality::NONE:
      out = aidlimports::VoWiFiCallQuality::NONE;
      break;
    case qcril::interfaces::VowifiQuality::UNKNOWN:
    default:
      out = aidlimports::VoWiFiCallQuality::INVALID;
      break;
  }
}

bool isServiceTypeCfQuery(aidlimports::SsServiceType serType, aidlimports::SsRequestType reqType) {
  if ((reqType == aidlimports::SsRequestType::INTERROGATION) &&
      (serType == aidlimports::SsServiceType::CFU || serType == aidlimports::SsServiceType::CF_BUSY ||
       serType == aidlimports::SsServiceType::CF_NO_REPLY ||
       serType == aidlimports::SsServiceType::CF_NOT_REACHABLE ||
       serType == aidlimports::SsServiceType::CF_ALL ||
       serType == aidlimports::SsServiceType::CF_ALL_CONDITIONAL ||
       serType == aidlimports::SsServiceType::CFUT)) {
    return true;
  }
  return false;
}

bool isServiceTypeIcbQueryWithNumber(aidlimports::SsServiceType serType,
                                     aidlimports::SsRequestType reqType) {
  if ((reqType == aidlimports::SsRequestType::INTERROGATION) &&
      (serType == aidlimports::SsServiceType::INCOMING_BARRING_DN ||
       serType == aidlimports::SsServiceType::INCOMING_BARRING_ANONYMOUS)) {
    return true;
  }
  return false;
}

aidlimports::UssdModeType convertToAidlUssdModeType(const qcril::interfaces::UssdModeType& in) {
  switch (in) {
    case qcril::interfaces::UssdModeType::NOTIFY:
      return aidlimports::UssdModeType::NOTIFY;
    case qcril::interfaces::UssdModeType::REQUEST:
      return aidlimports::UssdModeType::REQUEST;
    case qcril::interfaces::UssdModeType::NW_RELEASE:
      return aidlimports::UssdModeType::NW_RELEASE;
    case qcril::interfaces::UssdModeType::LOCAL_CLIENT:
      return aidlimports::UssdModeType::LOCAL_CLIENT;
    case qcril::interfaces::UssdModeType::NOT_SUPPORTED:
      return aidlimports::UssdModeType::NOT_SUPPORTED;
    case qcril::interfaces::UssdModeType::NW_TIMEOUT:
      return aidlimports::UssdModeType::NW_TIMEOUT;
    default:
      return aidlimports::UssdModeType::NOTIFY;
  }
  return aidlimports::UssdModeType::NOTIFY;
}

aidlimports::GeoLocationDataStatus convertToAidlGeoLocationDataStatus(
    qcril::interfaces::GeoLocationDataStatus in) {
  switch (in) {
    case qcril::interfaces::GeoLocationDataStatus::TIMEOUT:
      return aidlimports::GeoLocationDataStatus::TIMEOUT;
    case qcril::interfaces::GeoLocationDataStatus::NO_CIVIC_ADDRESS:
      return aidlimports::GeoLocationDataStatus::NO_CIVIC_ADDRESS;
    case qcril::interfaces::GeoLocationDataStatus::ENGINE_LOCK:
      return aidlimports::GeoLocationDataStatus::ENGINE_LOCK;
    case qcril::interfaces::GeoLocationDataStatus::RESOLVED:
      return aidlimports::GeoLocationDataStatus::RESOLVED;
    default:
      return aidlimports::GeoLocationDataStatus::RESOLVED;
  }
}

bool convertToAidlCallForwardStatus(aidlimports::CallForwardStatus& out,
                                    const qcril::interfaces::SetCallForwardStatus& in) {
  out.reason = in.hasReason() ? in.getReason() : INT32_MAX;
  out.status = aidlimports::Result::FAILURE;
  if (in.hasStatus()) {
    out.status = in.getStatus() ? aidlimports::Result::SUCCESS : aidlimports::Result::FAILURE;
  }
  convertToAidlSipErrorInfo(out.errorDetails,
                            (in.hasErrorDetails() ? in.getErrorDetails() : nullptr), false);
  return true;
}

bool convertToAidlCallForwardStatus(std::vector<aidlimports::CallForwardStatus>& out,
                                    const std::vector<qcril::interfaces::SetCallForwardStatus>& in) {
  uint32_t count = in.size();
  out.resize(count);
  for (uint32_t i = 0; i < count; ++i) {
    convertToAidlCallForwardStatus(out[i], in[i]);
  }
  return true;
}

bool convertToAidlCallForwardStatusInfo(
    aidlimports::CallForwardStatusInfo& out,
    const std::shared_ptr<qcril::interfaces::SetCallForwardRespData>& in) {
  out.errorDetails.errorCode = INT32_MAX;
  out.errorDetails.errorString = "";
  out.status.resize(0);
  if (in) {
    if (in->hasErrorDetails() && in->getErrorDetails() != nullptr) {
      convertToAidlSipErrorInfo(out.errorDetails, in->getErrorDetails(), false);
    }
    if (!in->getSetCallForwardStatus().empty()) {
      convertToAidlCallForwardStatus(out.status, in->getSetCallForwardStatus());
    }
  }
  return true;
}

void convertToAidlServiceType(aidlimports::SsServiceType& out, RIL_SsServiceType in) {
  switch (in) {
    case RIL_SsServiceType::SS_CFU:
      out = aidlimports::SsServiceType::CFU;
      break;
    case RIL_SsServiceType::SS_CF_BUSY:
      out = aidlimports::SsServiceType::CF_BUSY;
      break;
    case RIL_SsServiceType::SS_CF_NO_REPLY:
      out = aidlimports::SsServiceType::CF_NO_REPLY;
      break;
    case RIL_SsServiceType::SS_CF_NOT_REACHABLE:
      out = aidlimports::SsServiceType::CF_NOT_REACHABLE;
      break;
    case RIL_SsServiceType::SS_CF_ALL:
      out = aidlimports::SsServiceType::CF_ALL;
      break;
    case RIL_SsServiceType::SS_CF_ALL_CONDITIONAL:
      out = aidlimports::SsServiceType::CF_ALL_CONDITIONAL;
      break;
    case RIL_SsServiceType::SS_CLIP:
      out = aidlimports::SsServiceType::CLIP;
      break;
    case RIL_SsServiceType::SS_CLIR:
      out = aidlimports::SsServiceType::CLIR;
      break;
    case RIL_SsServiceType::SS_COLP:
      out = aidlimports::SsServiceType::COLP;
      break;
    case RIL_SsServiceType::SS_COLR:
      out = aidlimports::SsServiceType::COLR;
      break;
    case RIL_SsServiceType::SS_WAIT:
      out = aidlimports::SsServiceType::WAIT;
      break;
    case RIL_SsServiceType::SS_BAOC:
      out = aidlimports::SsServiceType::BAOC;
      break;
    case RIL_SsServiceType::SS_BAOIC:
      out = aidlimports::SsServiceType::BAOIC;
      break;
    case RIL_SsServiceType::SS_BAOIC_EXC_HOME:
      out = aidlimports::SsServiceType::BAOIC_EXC_HOME;
      break;
    case RIL_SsServiceType::SS_BAIC:
      out = aidlimports::SsServiceType::BAIC;
      break;
    case RIL_SsServiceType::SS_BAIC_ROAMING:
      out = aidlimports::SsServiceType::BAIC_ROAMING;
      break;
    case RIL_SsServiceType::SS_ALL_BARRING:
      out = aidlimports::SsServiceType::ALL_BARRING;
      break;
    case RIL_SsServiceType::SS_OUTGOING_BARRING:
      out = aidlimports::SsServiceType::OUTGOING_BARRING;
      break;
    case RIL_SsServiceType::SS_INCOMING_BARRING:
      out = aidlimports::SsServiceType::INCOMING_BARRING;
      break;
    case RIL_SsServiceType::SS_INCOMING_BARRING_DN:
      out = aidlimports::SsServiceType::INCOMING_BARRING_DN;
      break;
    case RIL_SsServiceType::SS_INCOMING_BARRING_ANONYMOUS:
      out = aidlimports::SsServiceType::INCOMING_BARRING_ANONYMOUS;
      break;
    default:
      out = aidlimports::SsServiceType::INVALID;
      break;
  }
}
void convertToAidlRequestType(aidlimports::SsRequestType& out, RIL_SsRequestType in) {
  switch (in) {
    case RIL_SsRequestType::SS_ACTIVATION:
      out = aidlimports::SsRequestType::ACTIVATION;
      break;
    case RIL_SsRequestType::SS_DEACTIVATION:
      out = aidlimports::SsRequestType::DEACTIVATION;
      break;
    case RIL_SsRequestType::SS_INTERROGATION:
      out = aidlimports::SsRequestType::INTERROGATION;
      break;
    case RIL_SsRequestType::SS_REGISTRATION:
      out = aidlimports::SsRequestType::REGISTRATION;
      break;
    case RIL_SsRequestType::SS_ERASURE:
      out = aidlimports::SsRequestType::ERASURE;
      break;
    default:
      out = aidlimports::SsRequestType::INVALID;
      break;
  }
}
void convertToAidlTeleserviceType(aidlimports::SsTeleserviceType& out, RIL_SsTeleserviceType in) {
  switch (in) {
    case RIL_SsTeleserviceType::SS_ALL_TELE_AND_BEARER_SERVICES:
      out = aidlimports::SsTeleserviceType::ALL_TELE_AND_BEARER_SERVICES;
      break;
    case RIL_SsTeleserviceType::SS_ALL_TELESEVICES:
      out = aidlimports::SsTeleserviceType::ALL_TELESEVICES;
      break;
    case RIL_SsTeleserviceType::SS_TELEPHONY:
      out = aidlimports::SsTeleserviceType::TELEPHONY;
      break;
    case RIL_SsTeleserviceType::SS_ALL_DATA_TELESERVICES:
      out = aidlimports::SsTeleserviceType::ALL_DATA_TELESERVICES;
      break;
    case RIL_SsTeleserviceType::SS_SMS_SERVICES:
      out = aidlimports::SsTeleserviceType::SMS_SERVICES;
      break;
    case RIL_SsTeleserviceType::SS_ALL_TELESERVICES_EXCEPT_SMS:
      out = aidlimports::SsTeleserviceType::ALL_TELESERVICES_EXCEPT_SMS;
      break;
    default:
      out = aidlimports::SsTeleserviceType::INVALID;
      break;
  }
}
// TODO: Need to be converted to an enum.
int convertToRilOperationType(const aidlimports::SuppSvcOperationType& in) {
  switch (in) {
    case aidlimports::SuppSvcOperationType::ACTIVATE:
      return 1;
    case aidlimports::SuppSvcOperationType::DEACTIVATE:
      return 2;
    case aidlimports::SuppSvcOperationType::QUERY:
      return 3;
    case aidlimports::SuppSvcOperationType::REGISTER:
      return 4;
    case aidlimports::SuppSvcOperationType::ERASURE:
      return 5;
    case aidlimports::SuppSvcOperationType::INVALID:
    default:
      return 6;
  }
}

aidlimports::VoiceInfo convertToAidlVoiceInfo(const qcril::interfaces::VoiceInfo& in) {
  switch (in) {
    case qcril::interfaces::VoiceInfo::SILENT:
      return aidlimports::VoiceInfo::SILENT;
    case qcril::interfaces::VoiceInfo::SPEECH:
      return aidlimports::VoiceInfo::SPEECH;
    default:
      return aidlimports::VoiceInfo::UNKNOWN;
  }
}  // convertToAidlVoiceInfo

aidlimports::SystemServiceDomain convertToAidlSystemServiceDomain(
    const qcril::interfaces::SrvDomain& in) {
  switch (in) {
    case qcril::interfaces::SrvDomain::CS_ONLY:
      return aidlimports::SystemServiceDomain::CS_ONLY;
    case qcril::interfaces::SrvDomain::PS_ONLY:
      return aidlimports::SystemServiceDomain::PS_ONLY;
    case qcril::interfaces::SrvDomain::CS_PS:
      return aidlimports::SystemServiceDomain::CS_PS;
    case qcril::interfaces::SrvDomain::CAMPED:
      return aidlimports::SystemServiceDomain::CAMPED;
    default:
      return aidlimports::SystemServiceDomain::NO_SRV;
  }
} //convertToAidlSystemServiceDomain


aidlimports::SmsCallBackMode convertToAidlSmsCallBackMode(
    const qcril::interfaces::SmsCallbackModeStatus& in) {
  switch (in) {
    case qcril::interfaces::SmsCallbackModeStatus::ENTER:
      return aidlimports::SmsCallBackMode::ENTER;
    case qcril::interfaces::SmsCallbackModeStatus::EXIT:
    default:
      return aidlimports::SmsCallBackMode::EXIT;
  }
} // convertToAidlSmsCallBackMode

aidlimports::MessageWaitingIndication convertToAidlMessageWaitingIndication(
  std::shared_ptr<QcRilUnsolMessageWaitingInfoMessage> msg) {

  aidlimports::MessageWaitingIndication mwi = {};

  if (msg->hasMessageSummary() && !msg->getMessageSummary().empty()) {
    auto msgSummary = msg->getMessageSummary();
    mwi.messageSummary.resize(msgSummary.size());
    for (int i = 0; i < msgSummary.size(); i++) {
      mwi.messageSummary[i].type =
          msgSummary[i].hasMessageType()
              ? ims::utils::convertToAidlMessageType(msgSummary[i].getMessageType())
              : aidlimports::MwiMessageType::INVALID;
      mwi.messageSummary[i].newMessageCount =
          msgSummary[i].hasNewMessageCount() ? msgSummary[i].getNewMessageCount() : INT32_MAX;
      mwi.messageSummary[i].oldMessageCount =
          msgSummary[i].hasOldMessageCount() ? msgSummary[i].getOldMessageCount() : INT32_MAX;
      mwi.messageSummary[i].newUrgentMessageCount =
          msgSummary[i].hasNewUrgentMessageCount() ? msgSummary[i].getNewUrgentMessageCount()
                                                   : INT32_MAX;
      mwi.messageSummary[i].oldUrgentMessageCount =
          msgSummary[i].hasOldUrgentMessageCount() ? msgSummary[i].getOldUrgentMessageCount()
                                                   : INT32_MAX;
    }
  }
  if (msg->hasUeAddress() && !msg->getUeAddress().empty()) {
    mwi.ueAddress = msg->getUeAddress().c_str();
  }
  if (msg->hasMessageDetails() && !msg->getMessageDetails().empty()) {
    auto msgDetails = msg->getMessageDetails();
    mwi.messageDetails.resize(msgDetails.size());
    for (int i = 0; i < msgDetails.size(); i++) {
      if (msgDetails[i].hasToAddress() && !msgDetails[i].getToAddress().empty()) {
        mwi.messageDetails[i].toAddress = msgDetails[i].getToAddress().c_str();
      }
      if (msgDetails[i].hasFromAddress() && !msgDetails[i].getFromAddress().empty()) {
        mwi.messageDetails[i].fromAddress = msgDetails[i].getFromAddress().c_str();
      }
      if (msgDetails[i].hasSubject() && !msgDetails[i].getSubject().empty()) {
        mwi.messageDetails[i].subject = msgDetails[i].getSubject().c_str();
      }
      if (msgDetails[i].hasDate() && !msgDetails[i].getDate().empty()) {
        mwi.messageDetails[i].date = msgDetails[i].getDate().c_str();
      }
      mwi.messageDetails[i].priority =
          msgDetails[i].hasPriority()
              ? ims::utils::convertToAidlMessagePriority(msgDetails[i].getPriority())
              : aidlimports::MwiMessagePriority::INVALID;
      if (msgDetails[i].hasMessageId() && !msgDetails[i].getMessageId().empty()) {
        mwi.messageDetails[i].id = msgDetails[i].getMessageId().c_str();
      }
      mwi.messageDetails[i].type =
          msgDetails[i].hasMessageType()
              ? ims::utils::convertToAidlMessageType(msgDetails[i].getMessageType())
              : aidlimports::MwiMessageType::INVALID;
    }
  }

  return mwi;
} // convertToAidlMessageWaitingIndication

aidlimports::MwiMessageType convertToAidlMessageType(const qcril::interfaces::MessageType& in) {
  switch (in) {
    case qcril::interfaces::MessageType::NONE:
      return aidlimports::MwiMessageType::NONE;
    case qcril::interfaces::MessageType::VOICE:
      return aidlimports::MwiMessageType::VOICE;
    case qcril::interfaces::MessageType::VIDEO:
      return aidlimports::MwiMessageType::VIDEO;
    case qcril::interfaces::MessageType::FAX:
      return aidlimports::MwiMessageType::FAX;
    case qcril::interfaces::MessageType::PAGER:
      return aidlimports::MwiMessageType::PAGER;
    case qcril::interfaces::MessageType::MULTIMEDIA:
      return aidlimports::MwiMessageType::MULTIMEDIA;
    case qcril::interfaces::MessageType::TEXT:
      return aidlimports::MwiMessageType::TEXT;
    case qcril::interfaces::MessageType::UNKNOWN:
    default:
      return aidlimports::MwiMessageType::INVALID;
  }
} // convertToAidlMessageType

aidlimports::MwiMessagePriority convertToAidlMessagePriority(
    const qcril::interfaces::MessagePriority& in) {
  switch (in) {
    case qcril::interfaces::MessagePriority::UNKNOWN:
      return aidlimports::MwiMessagePriority::UNKNOWN;
    case qcril::interfaces::MessagePriority::LOW:
      return aidlimports::MwiMessagePriority::LOW;
    case qcril::interfaces::MessagePriority::NORMAL:
      return aidlimports::MwiMessagePriority::NORMAL;
    case qcril::interfaces::MessagePriority::URGENT:
      return aidlimports::MwiMessagePriority::URGENT;
    default:
      return aidlimports::MwiMessagePriority::INVALID;
  }
} // convertToAidlMessagePriority

aidlimports::DtmfInfo convertToAidlDtmfInfo(QcRilUnsolDtmfMessage& msg) {
  aidlimports::DtmfInfo dtmfInfo{};
  if (!msg.getDigitBuffer().empty()) {
    dtmfInfo.dtmf = msg.getDigitBuffer()[0];
  }
  dtmfInfo.callId = msg.getCallId();
  return dtmfInfo;
}

aidlimports::MultiSimVoiceCapability convertToAidlMultiSimVoiceCapability(
    const qcril::interfaces::MultiSimVoiceCapability& in) {
  switch (in) {
    case qcril::interfaces::MultiSimVoiceCapability::DSSS:
      return aidlimports::MultiSimVoiceCapability::DSSS;
    case qcril::interfaces::MultiSimVoiceCapability::DSDS:
      return aidlimports::MultiSimVoiceCapability::DSDS;
    case qcril::interfaces::MultiSimVoiceCapability::DSDA:
      return aidlimports::MultiSimVoiceCapability::DSDA;
    case qcril::interfaces::MultiSimVoiceCapability::PSEUDO_DSDA:
      return aidlimports::MultiSimVoiceCapability::PSEUDO_DSDA;
    case qcril::interfaces::MultiSimVoiceCapability::NONE:
    default:
      return aidlimports::MultiSimVoiceCapability::NONE;
  }
}  // convertToAidlMultiSimVoiceCapability

void convertToAidlEcnamInfo(aidlimports::EcnamInfo& out, const qcril::interfaces::EcnamInfo& in) {
  if (in.hasName() && !in.getName().empty()) {
    out.name = in.getName();
  }

  if (in.hasIconUrl() && !in.getIconUrl().empty()) {
    out.iconUrl = in.getIconUrl();
  }

  if (in.hasInfoUrl() && !in.getInfoUrl().empty()) {
    out.infoUrl = in.getInfoUrl();
  }

  if (in.hasCardUrl() && !in.getCardUrl().empty()) {
    out.cardUrl = in.getCardUrl();
  }
}  // convertToAidlEcnamInfo

void convertToAidlPreAlertingCallInfo(aidlimports::PreAlertingCallInfo& out,
                                      const qcril::interfaces::PreAlertingCallInfo& in) {
  if (in.hasCallId()) {
    out.callId = in.getCallId();
  }

  if (in.hasCallComposerInfo()) {
    aidlimports::CallComposerInfo ccInfo;
    convertToAidlCallComposerInfo(ccInfo, in.getCallComposerInfo());
    out.callComposerInfo = std::move(ccInfo);
  }
  if (in.hasEcnamInfo()) {
    aidlimports::EcnamInfo ecnamInfo;
    convertToAidlEcnamInfo(ecnamInfo, in.getEcnamInfo());
    out.ecnamInfo = std::move(ecnamInfo);
  }
  if (in.hasDataChannelInfo()) {
    qcril::interfaces::DataChannelInfo dcInfo = in.getDataChannelInfo();
    if (dcInfo.hasModemCallId()) {
        out.modemCallId = dcInfo.getModemCallId();
    } else {
        out.modemCallId = INT32_MAX;
    }
    if (dcInfo.hasIsDcCall()) {
        out.isDcCall = dcInfo.getIsDcCall();
    }
  }
}  // convertToAidlPreAlertingCallInfo

aidlimports::CiWlanNotificationInfo convertToAidlCIWlanNotification(
    const qcril::interfaces::CiWlanNotificationInfo& in) {
  switch (in) {
    case qcril::interfaces::CiWlanNotificationInfo::DISABLE_CIWLAN:
      return aidlimports::CiWlanNotificationInfo::DISABLE_CIWLAN;
    case qcril::interfaces::CiWlanNotificationInfo::NONE:
    default:
      return aidlimports::CiWlanNotificationInfo::NONE;
  }

}  // convertToAidlCIWlanNotification

void convertToRilVosActionInfo(qcril::interfaces::VosActionInfo& out,
                               const aidlimports::VosActionInfo& in) {
  if (in.vosMoveInfo) {
    out.setVosMoveInfo(convertToRilVosMoveInfo(*(in.vosMoveInfo)));
  }
  if (in.vosTouchInfo) {
    out.setVosTouchInfo(convertToRilVosTouchInfo(*(in.vosTouchInfo)));
  }
}  // convertToRilVosActionInfo

qcril::interfaces::VosMoveInfo convertToRilVosMoveInfo(const aidlimports::VosMoveInfo& in) {
  qcril::interfaces::VosMoveInfo vosMoveInfo{};
  vosMoveInfo.setStart(convertToRilCoordinate(in.start));
  vosMoveInfo.setEnd(convertToRilCoordinate(in.end));
  return vosMoveInfo;
}  // convertToRilVosMoveInfo

qcril::interfaces::VosTouchInfo convertToRilVosTouchInfo(const aidlimports::VosTouchInfo& in) {
  qcril::interfaces::VosTouchInfo vosTouchInfo{};
  vosTouchInfo.setTouch(convertToRilCoordinate(in.touch));
  vosTouchInfo.setTouchDuration(in.touchDuration);
  return vosTouchInfo;
}  // convertToRilVosTouchInfo

qcril::interfaces::Coordinate2D convertToRilCoordinate(const aidlimports::Coordinate2D& in) {
  qcril::interfaces::Coordinate2D coordinate2D{};
  coordinate2D.setX(in.x);
  coordinate2D.setY(in.y);
  return coordinate2D;
}  // convertToRilCoordinate

aidlimports::SrtpEncryptionInfo convertToAidlSrtpEncryptionInfo(QcRilUnsolImsSrtpEncryptionStatus& in)
{
  aidlimports::SrtpEncryptionInfo srtpInfo;
  srtpInfo.callId = in.getCallId();
  int32_t categories = static_cast<int32_t>(aidlimports::SrtpEncryptionCategories::UNENCRYPTED);
  if (in.getVoiceEnable()) {
    categories |= static_cast<int32_t>(aidlimports::SrtpEncryptionCategories::VOICE);
  }
  if (in.getVideoEnable()) {
    categories |= static_cast<int32_t>(aidlimports::SrtpEncryptionCategories::VIDEO);
  }
  if (in.getTextEnable()) {
    categories |= static_cast<int32_t>(aidlimports::SrtpEncryptionCategories::TEXT);
  }
  srtpInfo.categories = categories;
  return srtpInfo;
}  // convertToAidlSrtpEncryptionInfo

}  // namespace utils
}  // namespace ims
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
