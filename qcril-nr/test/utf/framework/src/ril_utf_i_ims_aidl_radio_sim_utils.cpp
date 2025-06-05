/******************************************************************************
#  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#  All rights reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "ril_utf_i_ims_aidl_radio_sim_utils.h"

#define INVALID_HEX_CHAR 16
static uint8_t hexCharToInt(uint8_t c) {
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (c - 'a' + 10);

    return INVALID_HEX_CHAR;
}

static uint8_t * convertHexStringToBytes(void *response, size_t responseLen) {
    if (responseLen % 2 != 0) {
        return NULL;
    }

    uint8_t *bytes = new uint8_t[responseLen/2];
    if (bytes == NULL) {
        QCRIL_LOG_ERROR("convertHexStringToBytes: cannot allocate memory for bytes string");
        return NULL;
    }
    uint8_t *hexString = (uint8_t *)response;

    for (size_t i = 0; i < responseLen; i += 2) {
        uint8_t hexChar1 = hexCharToInt(hexString[i]);
        uint8_t hexChar2 = hexCharToInt(hexString[i + 1]);

        if (hexChar1 == INVALID_HEX_CHAR || hexChar2 == INVALID_HEX_CHAR) {
            QCRIL_LOG_ERROR("convertHexStringToBytes: invalid hex char %d %d",
                    hexString[i], hexString[i + 1]);
            delete []bytes;
            return NULL;
        }
        bytes[i/2] = ((hexChar1 << 4) | hexChar2);
    }

    return bytes;
}

static bool isError(ims_Error ret) {
    return (ret != ims_Error_E_SUCCESS);
}

template <typename T>
static uint32_t getCount(T** ptr) {
    if (ptr == nullptr) {
        return 0;
    }

    uint32_t count = 0;
    while(*ptr) {
        count++;
        ptr++;
    }
    return count;
}


namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace aidl {
namespace utils {
// ========================================

ims_Error convertFromAidl(const imsaidl::ErrorCode& in) {
  switch(in) {
      case imsaidl::ErrorCode::SUCCESS:
        return ims_Error_E_SUCCESS;
      case imsaidl::ErrorCode::RADIO_NOT_AVAILABLE:
        return ims_Error_E_RADIO_NOT_AVAILABLE;
      case imsaidl::ErrorCode::GENERIC_FAILURE:
        return ims_Error_E_GENERIC_FAILURE;
      case imsaidl::ErrorCode::PASSWORD_INCORRECT:
        return ims_Error_E_PASSWORD_INCORECT;
      case imsaidl::ErrorCode::REQUEST_NOT_SUPPORTED:
        return ims_Error_E_REQUEST_NOT_SUPPORTED;
      case imsaidl::ErrorCode::CANCELLED:
        return ims_Error_E_CANCELLED;
      case imsaidl::ErrorCode::UNUSED:
        return ims_Error_E_UNUSED;
      case imsaidl::ErrorCode::INVALID_PARAMETER:
        return ims_Error_E_INVALID_PARAMETER;
      case imsaidl::ErrorCode::REJECTED_BY_REMOTE:
        return ims_Error_E_REJECTED_BY_REMOTE;
      case imsaidl::ErrorCode::IMS_DEREGISTERED:
        return ims_Error_E_IMS_DEREGISTERED;
      case imsaidl::ErrorCode::NETWORK_NOT_SUPPORTED:
        return ims_Error_E_NETWORK_NOT_SUPPORTED;
      case imsaidl::ErrorCode::HOLD_RESUME_FAILED:
        return ims_Error_E_HOLD_RESUME_FAILED;
      case imsaidl::ErrorCode::HOLD_RESUME_CANCELED:
        return ims_Error_E_HOLD_RESUME_CANCELED;
      case imsaidl::ErrorCode::REINVITE_COLLISION:
        return ims_Error_E_REINVITE_COLLISION;
      case imsaidl::ErrorCode::FDN_CHECK_FAILURE:
        return ims_Error_E_FDN_CHECK_FAILURE;
      case imsaidl::ErrorCode::SS_MODIFIED_TO_DIAL:
        return ims_Error_E_SS_MODIFIED_TO_DIAL;
      case imsaidl::ErrorCode::SS_MODIFIED_TO_USSD:
        return ims_Error_E_SS_MODIFIED_TO_USSD;
      case imsaidl::ErrorCode::SS_MODIFIED_TO_SS:
        return ims_Error_E_SS_MODIFIED_TO_SS;
      case imsaidl::ErrorCode::SS_MODIFIED_TO_DIAL_VIDEO:
        return ims_Error_E_SS_MODIFIED_TO_DIAL_VIDEO;
      case imsaidl::ErrorCode::DIAL_MODIFIED_TO_USSD:
        return ims_Error_E_DIAL_MODIFIED_TO_USSD;
      case imsaidl::ErrorCode::DIAL_MODIFIED_TO_SS:
        return ims_Error_E_DIAL_MODIFIED_TO_SS;
      case imsaidl::ErrorCode::DIAL_MODIFIED_TO_DIAL:
        return ims_Error_E_DIAL_MODIFIED_TO_DIAL;
      case imsaidl::ErrorCode::DIAL_MODIFIED_TO_DIAL_VIDEO:
        return ims_Error_E_DIAL_MODIFIED_TO_DIAL_VIDEO;
      case imsaidl::ErrorCode::DIAL_VIDEO_MODIFIED_TO_USSD:
        return ims_Error_E_DIAL_VIDEO_MODIFIED_TO_USSD;
      case imsaidl::ErrorCode::DIAL_VIDEO_MODIFIED_TO_SS:
        return ims_Error_E_DIAL_VIDEO_MODIFIED_TO_SS;
      case imsaidl::ErrorCode::DIAL_VIDEO_MODIFIED_TO_DIAL:
        return ims_Error_E_DIAL_VIDEO_MODIFIED_TO_DIAL;
      case imsaidl::ErrorCode::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
        return ims_Error_E_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
      case imsaidl::ErrorCode::CF_SERVICE_NOT_REGISTERED:
        return ims_Error_E_CF_SERVICE_NOT_REGISTERED;
      case imsaidl::ErrorCode::RIL_INTERNAL_INVALID_ARGUMENTS:
        return ims_Error_E_INTERNAL_INVALID_ARGUMENTS;
      default:
        return static_cast<ims_Error>(in);
    }
}

void convertFromAidl(const imsaidl::CallFailCause& in,
      ims_CallFailCause& out);
void convertToAidl(const ims_CallFailCause& in,
      imsaidl::CallFailCause& out);
void convertFromAidl(const imsaidl::ServiceStatusInfo& in,
        ims_Info& out);

void convertFromAidl(const imsaidl::Result &in,
    ims_Result& out) {
  switch(in) {
    case imsaidl::Result::SUCCESS:
      out = SUCCESS;
      break;
    case imsaidl::Result::FAILURE:
    default:
      out = FAILURE;
      break;
  }
}

void convertToAidl(const RIL_IMS_SMS_Message& in,
    imsaidl::SmsSendRequest& out) {
  out.shallRetry = !!in.retry;
  out.messageRef = in.messageRef;
  if (in.tech == RADIO_TECH_3GPP) {
    out.format = "3gpp";
    if (in.message.gsmMessage[0] != nullptr)
        out.smsc = in.message.gsmMessage[0];

    auto hex_len = strlen(in.message.gsmMessage[1]);
    uint8_t* buf = convertHexStringToBytes(in.message.gsmMessage[1], hex_len);
    if(buf != nullptr) {
      std::copy(buf, buf + (hex_len/2), std::back_inserter(out.pdu));
    }
  } else if (in.tech == RADIO_TECH_3GPP2) {
    out.format = "3gpp2";
  } else {
    out.format = "unknown";
  }
}

void convertFromAidl(const imsaidl::CallProgressInfoType& in,
    ims_CallProgressInfoType& out) {
  switch(in) {
    case imsaidl::CallProgressInfoType::CALL_REJ_Q850:
      out = ims_CallProgressInfoType_CALL_REJ_Q850;
      break;
    case imsaidl::CallProgressInfoType::CALL_WAITING:
      out = ims_CallProgressInfoType_CALL_WAITING;
      break;
    case imsaidl::CallProgressInfoType::CALL_FORWARDING:
      out = ims_CallProgressInfoType_CALL_FORWARDING;
      break;
    case imsaidl::CallProgressInfoType::REMOTE_AVAILABLE:
      out = ims_CallProgressInfoType_REMOTE_AVAILABLE;
      break;
    case imsaidl::CallProgressInfoType::INVALID:
      out = ims_CallProgressInfoType_UNKNOWN;
      break;
    default:
      out = ims_CallProgressInfoType_UNKNOWN;
      break;
  }
}

void convertFromAidl(const imsaidl::TtyMode& in,
    ims_Tty_Mode_Type& out) {
    switch (in) {
        case imsaidl::TtyMode::FULL:
            out = ims_Tty_Mode_Type_TTY_MODE_FULL;
            break;
        case imsaidl::TtyMode::HCO:
            out = ims_Tty_Mode_Type_TTY_MODE_HCO;
            break;
        case imsaidl::TtyMode::VCO:
            out = ims_Tty_Mode_Type_TTY_MODE_VCO;
            break;
        case imsaidl::TtyMode::OFF:
        default:
            out = ims_Tty_Mode_Type_TTY_MODE_OFF;
            break;
    }
}

void convertFromAidl(const imsaidl::VerificationStatus& in,
    ims_VerstatVerificationStatus& out) {
  switch(in) {
    case imsaidl::VerificationStatus::VALIDATION_PASS:
      out = ims_VerstatVerificationStatus_PASS;
      break;
    case imsaidl::VerificationStatus::VALIDATION_FAIL:
      out = ims_VerstatVerificationStatus_FAIL;
      break;
    default:
      out = ims_VerstatVerificationStatus_NONE;
      break;
  }
}

void convertFromAidl(const imsaidl::ConfParticipantOperation &in,
    ims_ConfParticipantOperation &out) {
  switch (in) {
    case imsaidl::ConfParticipantOperation::ADD:
      out = ims_ConfParticipantOperation_ADD;
      break;
    case imsaidl::ConfParticipantOperation::REMOVE:
      out = ims_ConfParticipantOperation_REMOVE;
      break;
    case imsaidl::ConfParticipantOperation::INVALID:
    default:
      out = ims_ConfParticipantOperation_ADD;
      break;
  }
}

void convertFromAidl(const imsaidl::ToneOperation &in,
    ims_RingBackTone_ToneFlag& out) {
  switch (in) {
    case imsaidl::ToneOperation::STOP:
      out = ims_RingBackTone_ToneFlag_STOP;
      break;
    case imsaidl::ToneOperation::START:
      out = ims_RingBackTone_ToneFlag_START;
      break;
    case imsaidl::ToneOperation::INVALID:
    default:
      out = ims_RingBackTone_ToneFlag_STOP;
      break;
  }
}

void convertFromAidl(const imsaidl::NotificationType& in,
    ims_NotificationType& out) {
  switch (in) {
    case imsaidl::NotificationType::MO:
      out = ims_NotificationType_MO;
      break;
    case imsaidl::NotificationType::MT:
      out = ims_NotificationType_MT;
      break;
    default:
      out = ims_NotificationType_MO;
      break;
  }
}

void convertFromAidl(const imsaidl::TirMode& tirMode,
      ims_TirMode& out) {
    switch(tirMode) {
        case imsaidl::TirMode::TEMPORARY:
            out = ims_TirMode_TEMPORARY;
            break;
        case imsaidl::TirMode::PERMANENT:
            out = ims_TirMode_PERMANENT;
            break;
        default:
            out = ims_TirMode_INVALID;
            break;
    }
}

void convertFromAidl(const imsaidl::CallModifyFailCause &in,
    ims_Error &out) {
  switch(in)
  {
    case imsaidl::CallModifyFailCause::E_SUCCESS:
      out = ims_Error_E_SUCCESS;
      break;
    case imsaidl::CallModifyFailCause::E_RADIO_NOT_AVAILABLE:
      out = ims_Error_E_RADIO_NOT_AVAILABLE;
      break;
    case imsaidl::CallModifyFailCause::E_GENERIC_FAILURE:
      out = ims_Error_E_GENERIC_FAILURE;
      break;
    case imsaidl::CallModifyFailCause::E_REQUEST_NOT_SUPPORTED:
      out = ims_Error_E_REQUEST_NOT_SUPPORTED;
      break;
    case imsaidl::CallModifyFailCause::E_CANCELLED:
      out = ims_Error_E_CANCELLED;
      break;
    case imsaidl::CallModifyFailCause::E_UNUSED:
      out = ims_Error_E_UNUSED;
      break;
    case imsaidl::CallModifyFailCause::E_INVALID_PARAMETER:
      out = ims_Error_E_INVALID_PARAMETER;
      break;
    case imsaidl::CallModifyFailCause::E_REJECTED_BY_REMOTE:
      out = ims_Error_E_REJECTED_BY_REMOTE;
      break;
    case imsaidl::CallModifyFailCause::E_IMS_DEREGISTERED:
      out = ims_Error_E_IMS_DEREGISTERED;
      break;
    case imsaidl::CallModifyFailCause::E_NETWORK_NOT_SUPPORTED:
      out = ims_Error_E_NETWORK_NOT_SUPPORTED;
      break;
    case imsaidl::CallModifyFailCause::E_HOLD_RESUME_FAILED:
      out = ims_Error_E_HOLD_RESUME_FAILED;
      break;
    case imsaidl::CallModifyFailCause::E_HOLD_RESUME_CANCELED:
      out = ims_Error_E_HOLD_RESUME_CANCELED;
      break;
    case imsaidl::CallModifyFailCause::E_REINVITE_COLLISION:
      out = ims_Error_E_REINVITE_COLLISION;
      break;
    case imsaidl::CallModifyFailCause::E_INVALID:
    default:
      out = ims_Error_E_SUCCESS;
      break;
  }
}

void convertToAidl(const ims_Error &in,
   imsaidl::CallModifyFailCause &out)
{
  bool ret = true;
  switch(in)
  {
    case ims_Error_E_SUCCESS:
      out = imsaidl::CallModifyFailCause::E_SUCCESS;
      break;
    case ims_Error_E_RADIO_NOT_AVAILABLE:
      out = imsaidl::CallModifyFailCause::E_RADIO_NOT_AVAILABLE;
      break;
    case ims_Error_E_GENERIC_FAILURE:
      out = imsaidl::CallModifyFailCause::E_GENERIC_FAILURE;
      break;
    case ims_Error_E_REQUEST_NOT_SUPPORTED:
      out = imsaidl::CallModifyFailCause::E_REQUEST_NOT_SUPPORTED;
      break;
    case ims_Error_E_CANCELLED:
      out = imsaidl::CallModifyFailCause::E_CANCELLED;
      break;
    case ims_Error_E_UNUSED:
      out = imsaidl::CallModifyFailCause::E_UNUSED;
      break;
    case ims_Error_E_INVALID_PARAMETER:
      out = imsaidl::CallModifyFailCause::E_INVALID_PARAMETER;
      break;
    case ims_Error_E_REJECTED_BY_REMOTE:
      out = imsaidl::CallModifyFailCause::E_REJECTED_BY_REMOTE;
      break;
    case ims_Error_E_IMS_DEREGISTERED:
      out = imsaidl::CallModifyFailCause::E_IMS_DEREGISTERED;
      break;
    case ims_Error_E_NETWORK_NOT_SUPPORTED:
      out = imsaidl::CallModifyFailCause::E_NETWORK_NOT_SUPPORTED;
      break;
    case ims_Error_E_HOLD_RESUME_FAILED:
      out = imsaidl::CallModifyFailCause::E_HOLD_RESUME_FAILED;
      break;
    case ims_Error_E_HOLD_RESUME_CANCELED:
      out = imsaidl::CallModifyFailCause::E_HOLD_RESUME_CANCELED;
      break;
    case ims_Error_E_REINVITE_COLLISION:
      out = imsaidl::CallModifyFailCause::E_REINVITE_COLLISION;
      break;
    default:
      out = imsaidl::CallModifyFailCause::E_INVALID;
      break;
  }
}

ims_Error convertFromAidl(
    const imsaidl::RegState& in,
    ims_Registration_RegState& out) {
    switch (in) {
        case imsaidl::RegState::REGISTERED:
            out = ims_Registration_RegState_REGISTERED;
            break;
        case imsaidl::RegState::NOT_REGISTERED:
            out = ims_Registration_RegState_NOT_REGISTERED;
            break;
        case imsaidl::RegState::REGISTERING:
            out = ims_Registration_RegState_REGISTERING;
            break;
        default:
            out = ims_Registration_RegState_NOT_REGISTERED;
            break;
    }
    return ims_Error_E_SUCCESS;
}

void convertFromAidl(const imsaidl::ConfigItem in, ims_ConfigItem& out) {
    switch (in) {
        case imsaidl::ConfigItem::NONE:
            out = ims_ConfigItem_CONFIG_ITEM_NONE;
            break;
        case imsaidl::ConfigItem::VOCODER_AMRMODESET:
            out = ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRMODESET;
            break;
        case imsaidl::ConfigItem::VOCODER_AMRWBMODESET:
            out = ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRWBMODESET;
            break;
        case imsaidl::ConfigItem::SIP_SESSION_TIMER:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_SESSION_TIMER;
            break;
        case imsaidl::ConfigItem::MIN_SESSION_EXPIRY:
            out = ims_ConfigItem_CONFIG_ITEM_MIN_SESSION_EXPIRY;
            break;
        case imsaidl::ConfigItem::CANCELLATION_TIMER:
            out = ims_ConfigItem_CONFIG_ITEM_CANCELLATION_TIMER;
            break;
        case imsaidl::ConfigItem::T_DELAY:
            out = ims_ConfigItem_CONFIG_ITEM_T_DELAY;
            break;
        case imsaidl::ConfigItem::SILENT_REDIAL_ENABLE:
            out = ims_ConfigItem_CONFIG_ITEM_SILENT_REDIAL_ENABLE;
            break;
        case imsaidl::ConfigItem::SIP_T1_TIMER:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_T1_TIMER;
            break;
        case imsaidl::ConfigItem::SIP_T2_TIMER:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_T2_TIMER;
            break;
        case imsaidl::ConfigItem::SIP_TF_TIMER:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_TF_TIMER;
            break;
        case imsaidl::ConfigItem::VLT_SETTING_ENABLED:
            out = ims_ConfigItem_CONFIG_ITEM_VLT_SETTING_ENABLED;
            break;
        case imsaidl::ConfigItem::LVC_SETTING_ENABLED:
            out = ims_ConfigItem_CONFIG_ITEM_LVC_SETTING_ENABLED;
            break;
        case imsaidl::ConfigItem::DOMAIN_NAME:
            out = ims_ConfigItem_CONFIG_ITEM_DOMAIN_NAME;
            break;
        case imsaidl::ConfigItem::SMS_FORMAT:
            out = ims_ConfigItem_CONFIG_ITEM_SMS_FORMAT;
            break;
        case imsaidl::ConfigItem::SMS_OVER_IP:
            out = ims_ConfigItem_CONFIG_ITEM_SMS_OVER_IP;
            break;
        case imsaidl::ConfigItem::PUBLISH_TIMER:
            out = ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER;
            break;
        case imsaidl::ConfigItem::PUBLISH_TIMER_EXTENDED:
            out = ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER_EXTENDED;
            break;
        case imsaidl::ConfigItem::CAPABILITIES_CACHE_EXPIRATION:
            out = ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION;
            break;
        case imsaidl::ConfigItem::AVAILABILITY_CACHE_EXPIRATION:
            out = ims_ConfigItem_CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION;
            break;
        case imsaidl::ConfigItem::CAPABILITIES_POLL_INTERVAL:
            out = ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL;
            break;
        case imsaidl::ConfigItem::SOURCE_THROTTLE_PUBLISH:
            out = ims_ConfigItem_CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH;
            break;
        case imsaidl::ConfigItem::MAX_NUM_ENTRIES_IN_RCL:
            out = ims_ConfigItem_CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL;
            break;
        case imsaidl::ConfigItem::CAPAB_POLL_LIST_SUB_EXP:
            out = ims_ConfigItem_CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP;
            break;
        case imsaidl::ConfigItem::GZIP_FLAG:
            out = ims_ConfigItem_CONFIG_ITEM_GZIP_FLAG;
            break;
        case imsaidl::ConfigItem::EAB_SETTING_ENABLED:
            out = ims_ConfigItem_CONFIG_ITEM_EAB_SETTING_ENABLED;
            break;
        case imsaidl::ConfigItem::MOBILE_DATA_ENABLED:
            out = ims_ConfigItem_CONFIG_ITEM_MOBILE_DATA_ENABLED;
            break;
        case imsaidl::ConfigItem::VOICE_OVER_WIFI_ENABLED:
            out = ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED;
            break;
        case imsaidl::ConfigItem::VOICE_OVER_WIFI_ROAMING:
            out = ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING;
            break;
        case imsaidl::ConfigItem::VOICE_OVER_WIFI_MODE:
            out = ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_MODE;
            break;
        case imsaidl::ConfigItem::CAPABILITY_DISCOVERY_ENABLED:
            out = ims_ConfigItem_CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED;
            break;
        case imsaidl::ConfigItem::EMERGENCY_CALL_TIMER:
            out = ims_ConfigItem_CONFIG_ITEM_EMERGENCY_CALL_TIMER;
            break;
        case imsaidl::ConfigItem::SSAC_HYSTERESIS_TIMER:
            out = ims_ConfigItem_CONFIG_ITEM_SSAC_HYSTERESIS_TIMER;
            break;
        case imsaidl::ConfigItem::VOLTE_USER_OPT_IN_STATUS:
            out = ims_ConfigItem_CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS;
            break;
        case imsaidl::ConfigItem::LBO_PCSCF_ADDRESS:
            out = ims_ConfigItem_CONFIG_ITEM_LBO_PCSCF_ADDRESS;
            break;
        case imsaidl::ConfigItem::KEEP_ALIVE_ENABLED:
            out = ims_ConfigItem_CONFIG_ITEM_KEEP_ALIVE_ENABLED;
            break;
        case imsaidl::ConfigItem::REGISTRATION_RETRY_BASE_TIME_SEC:
            out = ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC;
            break;
        case imsaidl::ConfigItem::REGISTRATION_RETRY_MAX_TIME_SEC:
            out = ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC;
            break;
        case imsaidl::ConfigItem::SPEECH_START_PORT:
            out = ims_ConfigItem_CONFIG_ITEM_SPEECH_START_PORT;
            break;
        case imsaidl::ConfigItem::SPEECH_END_PORT:
            out = ims_ConfigItem_CONFIG_ITEM_SPEECH_END_PORT;
            break;
        case imsaidl::ConfigItem::SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
            break;
        case imsaidl::ConfigItem::SIP_INVITE_RSP_WAIT_TIME_MSEC:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC;
            break;
        case imsaidl::ConfigItem::SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            break;
        case imsaidl::ConfigItem::SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
            break;
        case imsaidl::ConfigItem::SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
            break;
        case imsaidl::ConfigItem::SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
            break;
        case imsaidl::ConfigItem::SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
            break;
        case imsaidl::ConfigItem::SIP_ACK_RETX_WAIT_TIME_MSEC:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC;
            break;
        case imsaidl::ConfigItem::SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
            break;
        case imsaidl::ConfigItem::SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
            out = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            break;
        case imsaidl::ConfigItem::AMR_WB_OCTET_ALIGNED_PT:
            out = ims_ConfigItem_CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT;
            break;
        case imsaidl::ConfigItem::AMR_WB_BANDWIDTH_EFFICIENT_PT:
            out = ims_ConfigItem_CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT;
            break;
        case imsaidl::ConfigItem::AMR_OCTET_ALIGNED_PT:
            out = ims_ConfigItem_CONFIG_ITEM_AMR_OCTET_ALIGNED_PT;
            break;
        case imsaidl::ConfigItem::AMR_BANDWIDTH_EFFICIENT_PT:
            out = ims_ConfigItem_CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT;
            break;
        case imsaidl::ConfigItem::DTMF_WB_PT:
            out = ims_ConfigItem_CONFIG_ITEM_DTMF_WB_PT;
            break;
        case imsaidl::ConfigItem::DTMF_NB_PT:
            out = ims_ConfigItem_CONFIG_ITEM_DTMF_NB_PT;
            break;
        case imsaidl::ConfigItem::AMR_DEFAULT_MODE:
            out = ims_ConfigItem_CONFIG_ITEM_AMR_DEFAULT_MODE;
            break;
        case imsaidl::ConfigItem::SMS_PSI:
            out = ims_ConfigItem_CONFIG_ITEM_SMS_PSI;
            break;
        case imsaidl::ConfigItem::VIDEO_QUALITY:
            out = ims_ConfigItem_CONFIG_ITEM_VIDEO_QUALITY;
            break;
        case imsaidl::ConfigItem::THRESHOLD_LTE1:
            out = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE1;
            break;
        case imsaidl::ConfigItem::THRESHOLD_LTE2:
            out = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE2;
            break;
        case imsaidl::ConfigItem::THRESHOLD_LTE3:
            out = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE3;
            break;
        case imsaidl::ConfigItem::THRESHOLD_1x:
            out = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_1x;
            break;
        case imsaidl::ConfigItem::THRESHOLD_WIFI_A:
            out = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_A;
            break;
        case imsaidl::ConfigItem::THRESHOLD_WIFI_B:
            out = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_B;
            break;
        case imsaidl::ConfigItem::T_EPDG_LTE:
            out = ims_ConfigItem_CONFIG_ITEM_T_EPDG_LTE;
            break;
        case imsaidl::ConfigItem::T_EPDG_WIFI:
            out = ims_ConfigItem_CONFIG_ITEM_T_EPDG_WIFI;
            break;
        case imsaidl::ConfigItem::T_EPDG_1x:
            out = ims_ConfigItem_CONFIG_ITEM_T_EPDG_1x;
            break;
        case imsaidl::ConfigItem::VWF_SETTING_ENABLED:
            out = ims_ConfigItem_CONFIG_ITEM_VWF_SETTING_ENABLED;
            break;
        case imsaidl::ConfigItem::VCE_SETTING_ENABLED:
            out = ims_ConfigItem_CONFIG_ITEM_VCE_SETTING_ENABLED;
            break;
        case imsaidl::ConfigItem::RTT_SETTING_ENABLED:
            out = ims_ConfigItem_CONFIG_ITEM_RTT_SETTING_ENABLED;
            break;
        case imsaidl::ConfigItem::SMS_APP:
            out = ims_ConfigItem_CONFIG_ITEM_SMS_APP;
            break;
        case imsaidl::ConfigItem::VVM_APP:
            out = ims_ConfigItem_CONFIG_ITEM_VVM_APP;
            break;
        case imsaidl::ConfigItem::VOICE_OVER_WIFI_ROAMING_MODE:
            out = ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING_MODE;
            break;
        case imsaidl::ConfigItem::SET_AUTO_REJECT_CALL_MODE_CONFIG:
            out = ims_ConfigItem_CONFIG_ITEM_SET_AUTO_REJECT_CALL_MODE_CONFIG;
            break;
        case imsaidl::ConfigItem::VOWIFI_ENTITLEMENT_ID:
            out = ims_ConfigItem_CONFIG_ITEM_VOWIFI_ENTITLEMENT_ID;
            break;
        case imsaidl::ConfigItem::B2C_ENRICHED_CALLING_CONFIG:
            out = ims_ConfigItem_CONFIG_ITEM_B2C_ENRICHED_CALLING_ID;
            break;
        case imsaidl::ConfigItem::DATA_CHANNEL:
            out = ims_ConfigItem_CONFIG_ITEM_DATA_CHANNEL_ID;
            break;
        case imsaidl::ConfigItem::VOLTE_PROVISIONING_RESTRICT_HOME:
            out = ims_ConfigItem_CONFIG_ITEM_VOLTE_PROVISIONING_RESTRICT_HOME;
            break;
        case imsaidl::ConfigItem::VOLTE_PROVISIONING_RESTRICT_ROAMING:
            out = ims_ConfigItem_CONFIG_ITEM_VOLTE_PROVISIONING_RESTRICT_ROAMING;
            break;
        default:
            out = ims_ConfigItem_CONFIG_ITEM_NONE;
            break;
    }
}

void convertToAidl(const ims_ConfigItem& in,
    imsaidl::ConfigItem& out) {
    switch (in) {
        case ims_ConfigItem_CONFIG_ITEM_NONE:
            out = imsaidl::ConfigItem::NONE;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRMODESET:
            out = imsaidl::ConfigItem::VOCODER_AMRMODESET;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRWBMODESET:
            out = imsaidl::ConfigItem::VOCODER_AMRWBMODESET;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_SESSION_TIMER:
            out = imsaidl::ConfigItem::SIP_SESSION_TIMER;
            break;
        case ims_ConfigItem_CONFIG_ITEM_MIN_SESSION_EXPIRY:
            out = imsaidl::ConfigItem::MIN_SESSION_EXPIRY;
            break;
        case ims_ConfigItem_CONFIG_ITEM_CANCELLATION_TIMER:
            out = imsaidl::ConfigItem::CANCELLATION_TIMER;
            break;
        case ims_ConfigItem_CONFIG_ITEM_T_DELAY:
            out = imsaidl::ConfigItem::T_DELAY;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SILENT_REDIAL_ENABLE:
            out = imsaidl::ConfigItem::SILENT_REDIAL_ENABLE;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_T1_TIMER:
            out = imsaidl::ConfigItem::SIP_T1_TIMER;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_T2_TIMER:
            out = imsaidl::ConfigItem::SIP_T2_TIMER;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_TF_TIMER:
            out = imsaidl::ConfigItem::SIP_TF_TIMER;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VLT_SETTING_ENABLED:
            out = imsaidl::ConfigItem::VLT_SETTING_ENABLED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_LVC_SETTING_ENABLED:
            out = imsaidl::ConfigItem::LVC_SETTING_ENABLED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_DOMAIN_NAME:
            out = imsaidl::ConfigItem::DOMAIN_NAME;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SMS_FORMAT:
            out = imsaidl::ConfigItem::SMS_FORMAT;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SMS_OVER_IP:
            out = imsaidl::ConfigItem::SMS_OVER_IP;
            break;
        case ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER:
            out = imsaidl::ConfigItem::PUBLISH_TIMER;
            break;
        case ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER_EXTENDED:
            out = imsaidl::ConfigItem::PUBLISH_TIMER_EXTENDED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION:
            out = imsaidl::ConfigItem::CAPABILITIES_CACHE_EXPIRATION;
            break;
        case ims_ConfigItem_CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION:
            out = imsaidl::ConfigItem::AVAILABILITY_CACHE_EXPIRATION;
            break;
        case ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL:
            out = imsaidl::ConfigItem::CAPABILITIES_POLL_INTERVAL;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH:
            out = imsaidl::ConfigItem::SOURCE_THROTTLE_PUBLISH;
            break;
        case ims_ConfigItem_CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL:
            out = imsaidl::ConfigItem::MAX_NUM_ENTRIES_IN_RCL;
            break;
        case ims_ConfigItem_CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP:
            out = imsaidl::ConfigItem::CAPAB_POLL_LIST_SUB_EXP;
            break;
        case ims_ConfigItem_CONFIG_ITEM_GZIP_FLAG:
            out = imsaidl::ConfigItem::GZIP_FLAG;
            break;
        case ims_ConfigItem_CONFIG_ITEM_EAB_SETTING_ENABLED:
            out = imsaidl::ConfigItem::EAB_SETTING_ENABLED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_MOBILE_DATA_ENABLED:
            out = imsaidl::ConfigItem::MOBILE_DATA_ENABLED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED:
            out = imsaidl::ConfigItem::VOICE_OVER_WIFI_ENABLED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING:
            out = imsaidl::ConfigItem::VOICE_OVER_WIFI_ROAMING;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_MODE:
            out = imsaidl::ConfigItem::VOICE_OVER_WIFI_MODE;
            break;
        case ims_ConfigItem_CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED:
            out = imsaidl::ConfigItem::CAPABILITY_DISCOVERY_ENABLED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_EMERGENCY_CALL_TIMER:
            out = imsaidl::ConfigItem::EMERGENCY_CALL_TIMER;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SSAC_HYSTERESIS_TIMER:
            out = imsaidl::ConfigItem::SSAC_HYSTERESIS_TIMER;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS:
            out = imsaidl::ConfigItem::VOLTE_USER_OPT_IN_STATUS;
            break;
        case ims_ConfigItem_CONFIG_ITEM_LBO_PCSCF_ADDRESS:
            out = imsaidl::ConfigItem::LBO_PCSCF_ADDRESS;
            break;
        case ims_ConfigItem_CONFIG_ITEM_KEEP_ALIVE_ENABLED:
            out = imsaidl::ConfigItem::KEEP_ALIVE_ENABLED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC:
            out = imsaidl::ConfigItem::REGISTRATION_RETRY_BASE_TIME_SEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC:
            out = imsaidl::ConfigItem::REGISTRATION_RETRY_MAX_TIME_SEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SPEECH_START_PORT:
            out = imsaidl::ConfigItem::SPEECH_START_PORT;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SPEECH_END_PORT:
            out = imsaidl::ConfigItem::SPEECH_END_PORT;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
            out = imsaidl::ConfigItem::SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC:
            out = imsaidl::ConfigItem::SIP_INVITE_RSP_WAIT_TIME_MSEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
            out = imsaidl::ConfigItem::SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
            out = imsaidl::ConfigItem::SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
            out = imsaidl::ConfigItem::SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
            out = imsaidl::ConfigItem::SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
            out = imsaidl::ConfigItem::SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC:
            out = imsaidl::ConfigItem::SIP_ACK_RETX_WAIT_TIME_MSEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
            out = imsaidl::ConfigItem::SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
            out = imsaidl::ConfigItem::SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            break;
        case ims_ConfigItem_CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT:
            out = imsaidl::ConfigItem::AMR_WB_OCTET_ALIGNED_PT;
            break;
        case ims_ConfigItem_CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT:
            out = imsaidl::ConfigItem::AMR_WB_BANDWIDTH_EFFICIENT_PT;
            break;
        case ims_ConfigItem_CONFIG_ITEM_AMR_OCTET_ALIGNED_PT:
            out = imsaidl::ConfigItem::AMR_OCTET_ALIGNED_PT;
            break;
        case ims_ConfigItem_CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT:
            out = imsaidl::ConfigItem::AMR_BANDWIDTH_EFFICIENT_PT;
            break;
        case ims_ConfigItem_CONFIG_ITEM_DTMF_WB_PT:
            out = imsaidl::ConfigItem::DTMF_WB_PT;
            break;
        case ims_ConfigItem_CONFIG_ITEM_DTMF_NB_PT:
            out = imsaidl::ConfigItem::DTMF_NB_PT;
            break;
        case ims_ConfigItem_CONFIG_ITEM_AMR_DEFAULT_MODE:
            out = imsaidl::ConfigItem::AMR_DEFAULT_MODE;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SMS_PSI:
            out = imsaidl::ConfigItem::SMS_PSI;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VIDEO_QUALITY:
            out = imsaidl::ConfigItem::VIDEO_QUALITY;
            break;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE1:
            out = imsaidl::ConfigItem::THRESHOLD_LTE1;
            break;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE2:
            out = imsaidl::ConfigItem::THRESHOLD_LTE2;
            break;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE3:
            out = imsaidl::ConfigItem::THRESHOLD_LTE3;
            break;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_1x:
            out = imsaidl::ConfigItem::THRESHOLD_1x;
            break;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_A:
            out = imsaidl::ConfigItem::THRESHOLD_WIFI_A;
            break;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_B:
            out = imsaidl::ConfigItem::THRESHOLD_WIFI_B;
            break;
        case ims_ConfigItem_CONFIG_ITEM_T_EPDG_LTE:
            out = imsaidl::ConfigItem::T_EPDG_LTE;
            break;
        case ims_ConfigItem_CONFIG_ITEM_T_EPDG_WIFI:
            out = imsaidl::ConfigItem::T_EPDG_WIFI;
            break;
        case ims_ConfigItem_CONFIG_ITEM_T_EPDG_1x:
            out = imsaidl::ConfigItem::T_EPDG_1x;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VWF_SETTING_ENABLED:
            out = imsaidl::ConfigItem::VWF_SETTING_ENABLED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VCE_SETTING_ENABLED:
            out = imsaidl::ConfigItem::VCE_SETTING_ENABLED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_RTT_SETTING_ENABLED:
            out = imsaidl::ConfigItem::RTT_SETTING_ENABLED;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SMS_APP:
            out = imsaidl::ConfigItem::SMS_APP;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VVM_APP:
            out = imsaidl::ConfigItem::VVM_APP;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING_MODE:
            out = imsaidl::ConfigItem::VOICE_OVER_WIFI_ROAMING_MODE;
            break;
        case ims_ConfigItem_CONFIG_ITEM_SET_AUTO_REJECT_CALL_MODE_CONFIG:
            out = imsaidl::ConfigItem::SET_AUTO_REJECT_CALL_MODE_CONFIG;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VOWIFI_ENTITLEMENT_ID:
            out = imsaidl::ConfigItem::VOWIFI_ENTITLEMENT_ID;
            break;
        case ims_ConfigItem_CONFIG_ITEM_B2C_ENRICHED_CALLING_ID:
            out = imsaidl::ConfigItem::B2C_ENRICHED_CALLING_CONFIG;
            break;
        case ims_ConfigItem_CONFIG_ITEM_DATA_CHANNEL_ID:
            out = imsaidl::ConfigItem::DATA_CHANNEL;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VOLTE_PROVISIONING_RESTRICT_HOME:
            out = imsaidl::ConfigItem::VOLTE_PROVISIONING_RESTRICT_HOME;
            break;
        case ims_ConfigItem_CONFIG_ITEM_VOLTE_PROVISIONING_RESTRICT_ROAMING:
            out = imsaidl::ConfigItem::VOLTE_PROVISIONING_RESTRICT_ROAMING;
            break;
        default:
            out = imsaidl::ConfigItem::INVALID;
            break;
    }
}

void convertToAidl(const ims_ConfigFailureCause& in,
    imsaidl::ConfigFailureCause& out) {
    switch (in) {
    case ims_ConfigFailureCause_CONFIG_NO_ERR:
        out = imsaidl::ConfigFailureCause::NO_ERR;
        break;
    case ims_ConfigFailureCause_CONFIG_IMS_NOT_READY:
        out = imsaidl::ConfigFailureCause::IMS_NOT_READY;
        break;
    case ims_ConfigFailureCause_CONFIG_FILE_NOT_AVAILABLE:
        out = imsaidl::ConfigFailureCause::FILE_NOT_AVAILABLE;
        break;
    case ims_ConfigFailureCause_CONFIG_READ_FAILED:
        out = imsaidl::ConfigFailureCause::READ_FAILED;
        break;
    case ims_ConfigFailureCause_CONFIG_WRITE_FAILED:
        out = imsaidl::ConfigFailureCause::WRITE_FAILED;
        break;
    case ims_ConfigFailureCause_CONFIG_OTHER_INTERNAL_ERR:
        out = imsaidl::ConfigFailureCause::OTHER_INTERNAL_ERR;
        break;
    default:
        out = imsaidl::ConfigFailureCause::INVALID;
        break;
    }
}

void convertFromAidl(const imsaidl::ConfigFailureCause& in,
        ims_ConfigFailureCause& out) {
    switch (in) {
      case imsaidl::ConfigFailureCause::NO_ERR:
        out = ims_ConfigFailureCause_CONFIG_NO_ERR;
        break;
      case imsaidl::ConfigFailureCause::IMS_NOT_READY:
        out = ims_ConfigFailureCause_CONFIG_IMS_NOT_READY;
        break;
      case imsaidl::ConfigFailureCause::FILE_NOT_AVAILABLE:
        out = ims_ConfigFailureCause_CONFIG_FILE_NOT_AVAILABLE;
        break;
      case imsaidl::ConfigFailureCause::READ_FAILED:
        out = ims_ConfigFailureCause_CONFIG_READ_FAILED;
        break;
      case imsaidl::ConfigFailureCause::WRITE_FAILED:
        out = ims_ConfigFailureCause_CONFIG_WRITE_FAILED;
        break;
      case imsaidl::ConfigFailureCause::OTHER_INTERNAL_ERR:
        out = ims_ConfigFailureCause_CONFIG_OTHER_INTERNAL_ERR;
        break;
      default:
        break;
    }
}

void convertFromAidl(
    const imsaidl::IpPresentation& in,
    ims_IpPresentation& out) {
    switch (in) {
        case imsaidl::IpPresentation::NUM_ALLOWED:
            out = ims_IpPresentation_IP_PRESENTATION_NUM_ALLOWED;
            break;
        case imsaidl::IpPresentation::NUM_RESTRICTED:
            out = ims_IpPresentation_IP_PRESENTATION_NUM_RESTRICTED;
            break;
        case imsaidl::IpPresentation::NUM_DEFAULT:
        default:
            out = ims_IpPresentation_IP_PRESENTATION_NUM_DEFAULT;
            break;
    }
}

void convertToAidl(
    const ims_IpPresentation& in,
    imsaidl::IpPresentation& out) {
    switch (in) {
        case ims_IpPresentation_IP_PRESENTATION_NUM_ALLOWED:
            out = imsaidl::IpPresentation::NUM_ALLOWED;
            break;
        case ims_IpPresentation_IP_PRESENTATION_NUM_RESTRICTED:
            out = imsaidl::IpPresentation::NUM_RESTRICTED;
            break;
        case ims_IpPresentation_IP_PRESENTATION_NUM_DEFAULT:
            out = imsaidl::IpPresentation::NUM_DEFAULT;
            break;
        default:
            out = imsaidl::IpPresentation::INVALID;
            break;
    }
}

void convertToAidl(
    const ims_Registration_RegState& in,
    imsaidl::RegState& out) {
    switch (in) {
        case ims_Registration_RegState_REGISTERED:
            out = imsaidl::RegState::REGISTERED;
            break;
        case ims_Registration_RegState_NOT_REGISTERED:
            out = imsaidl::RegState::NOT_REGISTERED;
            break;
        case ims_Registration_RegState_REGISTERING:
            out = imsaidl::RegState::REGISTERING;
            break;
        default:
            out = imsaidl::RegState::NOT_REGISTERED;
            break;
    }
}

void convertFromAidl(const imsaidl::CallType& in,
    ims_CallType& out) {
    switch (in) {
        case imsaidl::CallType::USSD:
            out = ims_CallType_CALL_TYPE_USSD;
            break;
        case imsaidl::CallType::CALLCOMPOSER:
            out = ims_CallType_CALL_TYPE_CALLCOMPOSER;
            break;
        case imsaidl::CallType::DC:
            out = ims_CallType_CALL_TYPE_DC;
            break;
        case imsaidl::CallType::VOICE:
            out = ims_CallType_CALL_TYPE_VOICE;
            break;
        case imsaidl::CallType::VT_TX:
            out = ims_CallType_CALL_TYPE_VT_TX;
            break;
        case imsaidl::CallType::VT_RX:
            out = ims_CallType_CALL_TYPE_VT_RX;
            break;
        case imsaidl::CallType::VT:
            out = ims_CallType_CALL_TYPE_VT;
            break;
        case imsaidl::CallType::VT_NODIR:
            out = ims_CallType_CALL_TYPE_VT_NODIR;
            break;
        case imsaidl::CallType::CS_VS_TX:
            out = ims_CallType_CALL_TYPE_CS_VS_TX;
            break;
        case imsaidl::CallType::CS_VS_RX:
            out = ims_CallType_CALL_TYPE_CS_VS_RX;
            break;
        case imsaidl::CallType::PS_VS_TX:
            out = ims_CallType_CALL_TYPE_PS_VS_TX;
            break;
        case imsaidl::CallType::PS_VS_RX:
            out = ims_CallType_CALL_TYPE_PS_VS_RX;
            break;
        case imsaidl::CallType::SMS:
            out = ims_CallType_CALL_TYPE_SMS;
            break;
        case imsaidl::CallType::UT:
            out = ims_CallType_CALL_TYPE_UT;
            break;
        case imsaidl::CallType::UNKNOWN:
            out = ims_CallType_CALL_TYPE_UNKNOWN;
            break;
        default:
            break;
    }
}


void convertToAidl(const ims_CallType& in,
    imsaidl::CallType& out) {
    switch (in) {
        case ims_CallType_CALL_TYPE_USSD:
            out = imsaidl::CallType::USSD;
            break;
        case ims_CallType_CALL_TYPE_CALLCOMPOSER:
            out = imsaidl::CallType::CALLCOMPOSER;
            break;
        case ims_CallType_CALL_TYPE_VOICE:
            out = imsaidl::CallType::VOICE;
            break;
        case ims_CallType_CALL_TYPE_VT_TX:
            out = imsaidl::CallType::VT_TX;
            break;
        case ims_CallType_CALL_TYPE_VT_RX:
            out = imsaidl::CallType::VT_RX;
            break;
        case ims_CallType_CALL_TYPE_VT:
            out = imsaidl::CallType::VT;
            break;
        case ims_CallType_CALL_TYPE_VT_NODIR:
            out = imsaidl::CallType::VT_NODIR;
            break;
        case ims_CallType_CALL_TYPE_CS_VS_TX:
            out = imsaidl::CallType::CS_VS_TX;
            break;
        case ims_CallType_CALL_TYPE_CS_VS_RX:
            out = imsaidl::CallType::CS_VS_RX;
            break;
        case ims_CallType_CALL_TYPE_PS_VS_TX:
            out = imsaidl::CallType::PS_VS_TX;
            break;
        case ims_CallType_CALL_TYPE_PS_VS_RX:
            out = imsaidl::CallType::PS_VS_RX;
            break;
        case ims_CallType_CALL_TYPE_SMS:
            out = imsaidl::CallType::SMS;
            break;
        case ims_CallType_CALL_TYPE_UT:
            out = imsaidl::CallType::UT;
            break;
        case ims_CallType_CALL_TYPE_UNKNOWN:
        default:
            out = imsaidl::CallType::UNKNOWN;
            break;
    }
}

void convertFromAidl(const imsaidl::CallDomain& in,
      ims_CallDomain& out) {
    switch (in) {
        case imsaidl::CallDomain::UNKNOWN:
            out = ims_CallDomain_CALL_DOMAIN_UNKNOWN;
            break;
        case imsaidl::CallDomain::CS:
            out = ims_CallDomain_CALL_DOMAIN_CS;
            break;
        case imsaidl::CallDomain::PS:
            out = ims_CallDomain_CALL_DOMAIN_PS;
            break;
        case imsaidl::CallDomain::AUTOMATIC:
            out = ims_CallDomain_CALL_DOMAIN_AUTOMATIC;
            break;
        case imsaidl::CallDomain::NOT_SET:
        default:
            out = ims_CallDomain_CALL_DOMAIN_NOT_SET;
            break;
    }
}

void convertToAidl(const ims_CallDomain& in,
    imsaidl::CallDomain& out) {
    switch (in) {
        case ims_CallDomain_CALL_DOMAIN_UNKNOWN:
            out = imsaidl::CallDomain::UNKNOWN;
            break;
        case ims_CallDomain_CALL_DOMAIN_CS:
            out = imsaidl::CallDomain::CS;
            break;
        case ims_CallDomain_CALL_DOMAIN_PS:
            out = imsaidl::CallDomain::PS;
            break;
        case ims_CallDomain_CALL_DOMAIN_AUTOMATIC:
            out = imsaidl::CallDomain::AUTOMATIC;
            break;
        case ims_CallDomain_CALL_DOMAIN_NOT_SET:
            out = imsaidl::CallDomain::NOT_SET;
            break;
        default:
            out = imsaidl::CallDomain::INVALID;
            break;
    }
}

void convertToAidl(const ims_StatusType& in,
        imsaidl::StatusType& out) {
    switch (in) {
        case ims_StatusType_STATUS_DISABLED:
            out = imsaidl::StatusType::DISABLED;
            break;
        case ims_StatusType_STATUS_PARTIALLY_ENABLED:
            out = imsaidl::StatusType::PARTIALLY_ENABLED;
            break;
        case ims_StatusType_STATUS_ENABLED:
            out = imsaidl::StatusType::ENABLED;
            break;
        case ims_StatusType_STATUS_NOT_SUPPORTED:
            out = imsaidl::StatusType::NOT_SUPPORTED;
            break;
        default:
            out = imsaidl::StatusType::INVALID;
            break;
    }
}

void convertFromAidl(const imsaidl::RadioTechType& in,
        ims_RadioTechType& out) {
    switch (in) {
        case imsaidl::RadioTechType::ANY:
            out = ims_RadioTechType_RADIO_TECH_ANY;
            break;
        case imsaidl::RadioTechType::UNKNOWN:
            out = ims_RadioTechType_RADIO_TECH_UNKNOWN;
            break;
        case imsaidl::RadioTechType::GPRS:
            out = ims_RadioTechType_RADIO_TECH_GPRS;
            break;
        case imsaidl::RadioTechType::EDGE:
            out = ims_RadioTechType_RADIO_TECH_EDGE;
            break;
        case imsaidl::RadioTechType::UMTS:
            out = ims_RadioTechType_RADIO_TECH_UMTS;
            break;
        case imsaidl::RadioTechType::IS95A:
            out = ims_RadioTechType_RADIO_TECH_IS95A;
            break;
        case imsaidl::RadioTechType::IS95B:
            out = ims_RadioTechType_RADIO_TECH_IS95B;
            break;
        case imsaidl::RadioTechType::RTT_1X:
            out = ims_RadioTechType_RADIO_TECH_1xRTT;
            break;
        case imsaidl::RadioTechType::EVDO_0:
            out = ims_RadioTechType_RADIO_TECH_EVDO_0;
            break;
        case imsaidl::RadioTechType::EVDO_A:
            out = ims_RadioTechType_RADIO_TECH_EVDO_A;
            break;
        case imsaidl::RadioTechType::HSDPA:
            out = ims_RadioTechType_RADIO_TECH_HSDPA;
            break;
        case imsaidl::RadioTechType::HSUPA:
            out = ims_RadioTechType_RADIO_TECH_HSUPA;
            break;
        case imsaidl::RadioTechType::HSPA:
            out = ims_RadioTechType_RADIO_TECH_HSPA;
            break;
        case imsaidl::RadioTechType::EVDO_B:
            out = ims_RadioTechType_RADIO_TECH_EVDO_B;
            break;
        case imsaidl::RadioTechType::EHRPD:
            out = ims_RadioTechType_RADIO_TECH_EHRPD;
            break;
        case imsaidl::RadioTechType::LTE:
            out = ims_RadioTechType_RADIO_TECH_LTE;
            break;
        case imsaidl::RadioTechType::HSPAP:
            out = ims_RadioTechType_RADIO_TECH_HSPAP;
            break;
        case imsaidl::RadioTechType::GSM:
            out = ims_RadioTechType_RADIO_TECH_GSM;
            break;
        case imsaidl::RadioTechType::TD_SCDMA:
            out = ims_RadioTechType_RADIO_TECH_TD_SCDMA;
            break;
        case imsaidl::RadioTechType::WIFI:
            out = ims_RadioTechType_RADIO_TECH_WIFI;
            break;
        case imsaidl::RadioTechType::IWLAN:
            out = ims_RadioTechType_RADIO_TECH_IWLAN;
            break;
        case imsaidl::RadioTechType::NR5G:
            out = ims_RadioTechType_RADIO_TECH_NR5G;
            break;
        case imsaidl::RadioTechType::C_IWLAN:
            out = ims_RadioTechType_RADIO_TECH_C_IWLAN;
            break;
        default:
            out = ims_RadioTechType_RADIO_TECH_UNKNOWN;
            break;
    }
}

void convertToAidl(const ims_RadioTechType& in,
        imsaidl::RadioTechType& out) {
    switch (in) {
        case ims_RadioTechType_RADIO_TECH_ANY:
            out = imsaidl::RadioTechType::ANY;
            break;
        case ims_RadioTechType_RADIO_TECH_UNKNOWN:
            out = imsaidl::RadioTechType::UNKNOWN;
            break;
        case ims_RadioTechType_RADIO_TECH_GPRS:
            out = imsaidl::RadioTechType::GPRS;
            break;
        case ims_RadioTechType_RADIO_TECH_EDGE:
            out = imsaidl::RadioTechType::EDGE;
            break;
        case ims_RadioTechType_RADIO_TECH_UMTS:
            out = imsaidl::RadioTechType::UMTS;
            break;
        case ims_RadioTechType_RADIO_TECH_IS95A:
            out = imsaidl::RadioTechType::IS95A;
            break;
        case ims_RadioTechType_RADIO_TECH_IS95B:
            out = imsaidl::RadioTechType::IS95B;
            break;
        case ims_RadioTechType_RADIO_TECH_1xRTT:
            out = imsaidl::RadioTechType::RTT_1X;
            break;
        case ims_RadioTechType_RADIO_TECH_EVDO_0:
            out = imsaidl::RadioTechType::EVDO_0;
            break;
        case ims_RadioTechType_RADIO_TECH_EVDO_A:
            out = imsaidl::RadioTechType::EVDO_A;
            break;
        case ims_RadioTechType_RADIO_TECH_HSDPA:
            out = imsaidl::RadioTechType::HSDPA;
            break;
        case ims_RadioTechType_RADIO_TECH_HSUPA:
            out = imsaidl::RadioTechType::HSUPA;
            break;
        case ims_RadioTechType_RADIO_TECH_HSPA:
            out = imsaidl::RadioTechType::HSPA;
            break;
        case ims_RadioTechType_RADIO_TECH_EVDO_B:
            out = imsaidl::RadioTechType::EVDO_B;
            break;
        case ims_RadioTechType_RADIO_TECH_EHRPD:
            out = imsaidl::RadioTechType::EHRPD;
            break;
        case ims_RadioTechType_RADIO_TECH_LTE:
            out = imsaidl::RadioTechType::LTE;
            break;
        case ims_RadioTechType_RADIO_TECH_HSPAP:
            out = imsaidl::RadioTechType::HSPAP;
            break;
        case ims_RadioTechType_RADIO_TECH_GSM:
            out = imsaidl::RadioTechType::GSM;
            break;
        case ims_RadioTechType_RADIO_TECH_TD_SCDMA:
            out = imsaidl::RadioTechType::TD_SCDMA;
            break;
        case ims_RadioTechType_RADIO_TECH_WIFI:
            out = imsaidl::RadioTechType::WIFI;
            break;
        case ims_RadioTechType_RADIO_TECH_IWLAN:
            out = imsaidl::RadioTechType::IWLAN;
            break;
        case ims_RadioTechType_RADIO_TECH_NR5G:
            out = imsaidl::RadioTechType::NR5G;
            break;
        default:
            out = imsaidl::RadioTechType::INVALID;
            break;
    }
}

void convertFromAidl(const imsaidl::StatusType& in,
        ims_StatusType& out) {
    switch (in) {
        case imsaidl::StatusType::DISABLED:
            out = ims_StatusType_STATUS_DISABLED;
            break;
        case imsaidl::StatusType::PARTIALLY_ENABLED:
            out = ims_StatusType_STATUS_PARTIALLY_ENABLED;
            break;
        case imsaidl::StatusType::ENABLED:
            out = ims_StatusType_STATUS_ENABLED;
            break;
        case imsaidl::StatusType::NOT_SUPPORTED:
            out = ims_StatusType_STATUS_NOT_SUPPORTED;
            break;
        default:
            out = ims_StatusType_STATUS_DISABLED;
            break;
    }
}

void convertToAidl(
    const ims_Registration& in,
    imsaidl::RegistrationInfo& out) {
    if (in.has_state) {
        convertToAidl(in.state, out.state);
    } else {
        out.state = imsaidl::RegState::INVALID;
    }

    if (in.has_errorCode) {
        out.errorCode = in.errorCode;
    } else {
        out.errorCode = INT32_MAX;
    }

    out.errorMessage = static_cast<const char*>(in.errorMessage.arg);

    if (in.has_radioTech) {
        convertToAidl(in.radioTech, out.radioTech);
    } else {
        out.radioTech = imsaidl::RadioTechType::INVALID;
    }

    const char* ptr = static_cast<const char*>(in.pAssociatedUris.arg);
    if (ptr != nullptr) {
        out.pAssociatedUris = ptr;
    }
}

ims_Error convertFromAidl(
    const imsaidl::RegistrationInfo& inRegistration,
    ims_Registration& outRegistration) {
    if (inRegistration.state != imsaidl::RegState::INVALID) {
        outRegistration.has_state = true;
        convertFromAidl(inRegistration.state, outRegistration.state);
    }
    if (inRegistration.errorCode !=  INT32_MAX) {
        outRegistration.has_errorCode = true;
        outRegistration.errorCode = inRegistration.errorCode;
    }
    const uint32_t errorMessageLen = inRegistration.errorMessage.size();
    if (errorMessageLen > 0) {
        size_t destSize = sizeof(char) * (errorMessageLen + 1);
        outRegistration.errorMessage.arg = qcril_malloc(destSize);

        if (outRegistration.errorMessage.arg == nullptr) {
            return ims_Error_E_GENERIC_FAILURE;
        }

        strlcpy((char*)outRegistration.errorMessage.arg, inRegistration.errorMessage.c_str(),
                destSize);
    }
    if (inRegistration.radioTech != imsaidl::RadioTechType::INVALID) {
        outRegistration.has_radioTech = true;
        convertFromAidl(inRegistration.radioTech, outRegistration.radioTech);
    }
    const uint32_t pAssociatedUrisLen = inRegistration.pAssociatedUris.size();
    if (pAssociatedUrisLen > 0) {
        size_t destSize = sizeof(char) * (pAssociatedUrisLen + 1);
        outRegistration.pAssociatedUris.arg = qcril_malloc(destSize);

        if (outRegistration.pAssociatedUris.arg == nullptr) {
            if (outRegistration.errorMessage.arg != nullptr) {
                qcril_free(outRegistration.errorMessage.arg);
                outRegistration.errorMessage.arg = nullptr;
            }
            return ims_Error_E_GENERIC_FAILURE;
        }

        strlcpy((char*)outRegistration.pAssociatedUris.arg, inRegistration.pAssociatedUris.c_str(),
                destSize);
    }
    return ims_Error_E_SUCCESS;
}

void convertFromAidl(const imsaidl::StatusForAccessTech& in,
        ims_StatusForAccessTech& out) {
    if (in.networkMode != imsaidl::RadioTechType::INVALID) {
        out.has_networkMode = true;
        convertFromAidl(in.networkMode, out.networkMode);
    }
    if (in.status != imsaidl::StatusType::INVALID) {
        out.has_status = true;
        convertFromAidl(in.status, out.status);
    }
    if (in.restrictCause != INT32_MAX) {
        out.has_restrictionCause = true;
        out.restrictionCause = in.restrictCause;
    }
    if (in.hasRegistration) {
        out.has_registered = true;
        convertFromAidl(in.registration, out.registered);
    }
}

void convertToAidl(const ims_StatusForAccessTech& in,
        imsaidl::StatusForAccessTech& out) {
    if (in.has_networkMode) {
        convertToAidl(in.networkMode, out.networkMode);
    } else {
        out.networkMode = imsaidl::RadioTechType::INVALID;
    }

    if (in.has_status) {
        convertToAidl(in.status, out.status);
    } else {
        out.status = imsaidl::StatusType::INVALID;
    }

    if (in.has_restrictionCause) {
        out.restrictCause = in.restrictionCause;
    } else {
        out.restrictCause = INT32_MAX;
    }

    out.hasRegistration = in.has_registered;
    if (in.has_registered) {
      convertToAidl(in.registered, out.registration);
    }
}

void convertToAidl(const ims_Info& in,
        imsaidl::ServiceStatusInfo& out) {
    if (in.has_isValid) {
        out.isValid = in.isValid;
    }

    if (in.has_callType) {
        convertToAidl(in.callType, out.callType);
    } else {
        out.callType = imsaidl::CallType::UNKNOWN;
    }

    if (in.has_status) {
        convertToAidl(in.status, out.status);
    } else {
        out.status = imsaidl::StatusType::INVALID;
    }

    if (in.has_restrictCause) {
        out.restrictCause = in.restrictCause;
    } else {
        out.restrictCause = INT32_MAX;
    }

    if(in.has_rttMode) {
       out.rttMode = (in.rttMode) ? imsaidl::RttMode::FULL : imsaidl::RttMode::DISABLED;
    } else {
        out.rttMode = imsaidl::RttMode::INVALID;
    }
    QCRIL_LOG_INFO("Out rrtMode %d ", out.rttMode);

    const ims_StatusForAccessTech** ptr = (const ims_StatusForAccessTech**)in.accTechStatus.arg;
    if (ptr != nullptr) {
        uint32_t count = getCount(ptr);
        out.accTechStatus.resize(count);
        for(uint32_t i = 0; i < count; ++i) {
            out.accTechStatus[i] = {};
            convertToAidl(*(ptr[i]), out.accTechStatus[i]);
        }
    }
}

void convertFromAidl(const std::vector<imsaidl::ServiceStatusInfo>& in,
        ims_Info** out) {
    ims_Error ret = ims_Error_E_SUCCESS;
    uint32_t inListLen = in.size();
    do {
        if (inListLen > 0) {
            out = (ims_Info**)qcril_malloc((inListLen + 1) * sizeof(ims_Info*));

            if (out == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for service status info list.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            for (uint32_t i = 0; i < inListLen; ++i) {
                out[i] = (ims_Info*)qcril_malloc(sizeof(ims_Info));

                if (out[i] == nullptr) {
                    QCRIL_LOG_ERROR("Failed to allocate memory for service status info.");
                    ret = ims_Error_E_GENERIC_FAILURE;
                    break;
                }
                convertFromAidl(in[i], *(out[i]));
            }
        }
    } while(0);

    if (isError(ret)) {
        if (out != nullptr) {
            while (*out) {
                qcril_free(*out);
                *out = nullptr;
                out++;
            }
            qcril_free(out);
            out = nullptr;
        }
    }
}

void convertToAidl(const ims_Info** const in,
        std::vector<imsaidl::ServiceStatusInfo>& out) {
    if (in != nullptr) {
        uint32_t count = getCount(in);
        out.resize(count);
        for(uint32_t i = 0; i < count; ++i) {
            out[i] = {};
            convertToAidl(*(in[i]), out[i]);
        }
    }
}

void convertToAidl(const ims_RttMode& in,
    imsaidl::RttMode& out) {
    switch (in) {
        case ims_RttMode_RTT_MODE_DISABLED:
            out = imsaidl::RttMode::DISABLED;
            break;
        case ims_RttMode_RTT_MODE_FULL:
            out = imsaidl::RttMode::FULL;
            break;
        default:
            out = imsaidl::RttMode::INVALID;
            break;
    }
}

void convertFromAidl(const imsaidl::RttMode& in,
    ims_RttMode& out) {
    switch (in) {
        case imsaidl::RttMode::DISABLED:
            out = ims_RttMode_RTT_MODE_DISABLED;
            break;
        case imsaidl::RttMode::FULL:
            out = ims_RttMode_RTT_MODE_FULL;
            break;
        default:
            out = ims_RttMode_RTT_MODE_DISABLED;
            break;
    }
}

void convertFromAidl(
        const std::vector<std::string>& in,
        char** out) {
    for (uint32_t i = 0; i < in.size(); i++) {
        size_t destSize = sizeof(char) * (in[i].size() + 1);
        out[i] =  (char *) qcril_malloc(destSize);

        if (out[i] == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory for call details extras.");
            break;
        }

        strlcpy(out[i], in[i].c_str(), destSize);
        QCRIL_LOG_INFO("ims_radio_utils: dial extras[%d] %s", i, out[i]);
    }
}

void convertFromAidl(const imsaidl::CallDetails& in,
    ims_CallDetails& out) {
    ims_Error ret = ims_Error_E_SUCCESS;
    convertFromAidl(in.callType, out.callType);
    out.has_callType = true;
    convertFromAidl(in.callDomain, out.callDomain);
    out.has_callDomain = true;

    uint32_t extrasLen = in.extras.size();
    do {
        if (extrasLen > 0) {
            out.extras.arg = qcril_malloc((extrasLen + 1) * sizeof(char*));
            if (out.extras.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for call detail extras.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }
            convertFromAidl(in.extras, (char**)out.extras.arg);
        }

        if (in.localAbility.size()) {
          out.has_localAbility = TRUE;
        }
        convertFromAidl(in.localAbility,
          (ims_Info**)(out.localAbility.SrvStatusInfo.arg));
        if (in.peerAbility.size()) {
          out.has_peerAbility = TRUE;
        }
        convertFromAidl(in.peerAbility,
          (ims_Info**)(out.peerAbility.SrvStatusInfo.arg));

        if (in.callSubstate != INT32_MAX) {
            out.has_callSubstate = true;
            out.callSubstate = (ims_CallSubstate)in.callSubstate;
        }
        if (in.mediaId != INT32_MAX) {
            out.has_mediaId = true;
            out.mediaId = in.mediaId;
        }

        if (in.causeCode != INT32_MAX) {
            out.has_causeCode = true;
            out.causeCode = (ims_CauseCodes)in.causeCode;
        }

        if(in.rttMode !=  imsaidl::RttMode::INVALID){
            out.has_rttMode = true;
            convertFromAidl(in.rttMode, out.rttMode);
        }

        const uint32_t sipUriStrLen = in.sipAlternateUri.size();
        if (sipUriStrLen > 0) {
            size_t destSize = sizeof(char) * (sipUriStrLen + 1);
            out.sipAlternateUri.arg = qcril_malloc(destSize);

            if (out.sipAlternateUri.arg != nullptr) {
                strlcpy((char*)out.sipAlternateUri.arg,
                            in.sipAlternateUri.c_str(),
                            destSize);
            } else {
                QCRIL_LOG_ERROR("Failed to allocate memory for call detail sipAlternateUri.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }
        }

        out.isVosSupported = in.isVosSupported;
    } while(0);

    if (isError(ret)) {
        qcril_free(out.extras.arg);
        out.extras.arg = nullptr;
    }
}

void convertToAidl(const ims_CallDetails& in,
    imsaidl::CallDetails& out) {
    if (in.has_callType) {
        convertToAidl(in.callType, out.callType);
    }
    else {
        out.callType =
          imsaidl::CallType::UNKNOWN;
    }

    if (in.has_callDomain) {
        convertToAidl(in.callDomain, out.callDomain);
    } else {
        out.callDomain =
          imsaidl::CallDomain::INVALID;
    }

    const char** ptr = (const char**)(in.extras.arg);
    uint32_t count = getCount(ptr);
    out.extras.resize(count);
    for(uint32_t i = 0; i < count; ++i) {
        out.extras.push_back(std::string(ptr[i], strlen(ptr[i])));
    }

    convertToAidl((const ims_Info**)
            (in.localAbility.SrvStatusInfo.arg),
            out.localAbility);

    convertToAidl((const ims_Info**)
            (in.peerAbility.SrvStatusInfo.arg),
            out.peerAbility);

    if (in.has_callSubstate) {
        out.callSubstate = in.callSubstate;
    } else {
        out.callSubstate = INT32_MAX;
    }

    if (in.has_mediaId) {
        out.mediaId = in.mediaId;
    } else {
        out.mediaId = INT32_MAX;
    }
    if (in.has_causeCode) {
        out.causeCode = in.causeCode;
    } else {
        out.causeCode = INT32_MAX;
    }

    if(in.has_rttMode) {
       convertToAidl(in.rttMode, out.rttMode);
    } else {
        out.rttMode =
          imsaidl::RttMode::INVALID;
    }
    QCRIL_LOG_INFO("Out rrtMode %d ", out.rttMode);

    const char* ptr1 = (const char*)in.sipAlternateUri.arg;
    if (ptr1 != NULL) {
        out.sipAlternateUri = std::string(ptr1, strlen(ptr1));
        QCRIL_LOG_INFO("Out Sip Redirect Uri %s ", out.sipAlternateUri.c_str());
    }
}

void convertToAidl(const ims_Dial& in,
    imsaidl::EmergencyDialRequest& out) {
  convertToAidl(in, out.dialRequest);
  out.categories =
    static_cast<imsaidl::EmergencyServiceCategory>(in.emergencyDialExtra.categories);
  out.urns.resize(0);
  out.route =
    static_cast<imsaidl::EmergencyCallRoute>(in.emergencyDialExtra.route);
  out.hasKnownUserIntentEmergency =
    in.emergencyDialExtra.hasKnownUserIntentEmergency;
  out.isTesting = in.emergencyDialExtra.isTesting;
}

void convertToAidl(const uint32_t in,
    imsaidl::ClirMode& out) {
  switch(in) {
    case 1:
      out = imsaidl::ClirMode::INVOCATION;
      break;
    case 2:
      out = imsaidl::ClirMode::SUPRESSION;
      break;
    default:
      out = imsaidl::ClirMode::DEFAULT;
      break;
  }
}

void convertToAidl(const ims_Dial& in,
    imsaidl::DialRequest& out) {
  do {
    // Copy address
    if (in.address.arg != nullptr) {
      out.address = (const char *)in.address.arg;
    }
    // CLIR
    out.clirMode = imsaidl::ClirMode::DEFAULT;
    if (in.has_clir) {
      convertToAidl(in.clir, out.clirMode);
    }
    // Call Detals
    if (in.has_callDetails) {
      convertToAidl(in.callDetails, out.callDetails);
    }
    // Conference URI
    out.isConferenceUri = false;
    if (in.has_isConferenceUri) {
      out.isConferenceUri = in.isConferenceUri;
    }
    // call pull
    out.isCallPull = false;
    if (in.has_isCallPull) {
      out.isCallPull = in.isCallPull;
    }
    // Encrypted
    out.isEncrypted = false;
    if (in.has_isEncrypted) {
      out.isEncrypted = in.isEncrypted;
    }
    // MultiIdentityLineInfo
    if (in.originatingNumber.arg) {
      out.multiLineInfo.msisdn = (const char *)in.originatingNumber.arg;
    }
    if (in.isSecondary) {
      out.multiLineInfo.lineType =
        imsaidl::MultiIdentityLineType::SECONDARY;
    }
    out.redialInfo.callFailReason = imsaidl::CallFailCause::ERROR_UNSPECIFIED;
    out.redialInfo.callFailRadioTech = imsaidl::RadioTechType::UNKNOWN;
    if (in.has_redialInfo) {
      convertToAidl(in.redialInfo.callFailReason, out.redialInfo.callFailReason);
      convertToAidl(in.redialInfo.callFailRadioTech, out.redialInfo.callFailRadioTech);
    }
  } while (0);
}

ims_Error convertFromAidl(
    const imsaidl::SmsSendResponse& in) {
  if (in.smsStatus == imsaidl::SmsSendStatus::OK) {
    return ims_Error_E_SUCCESS;
  } else if (in.smsStatus == imsaidl::SmsSendStatus::ERROR_FALLBACK ||
        in.smsStatus == imsaidl::SmsSendStatus::ERROR_RETRY) {
    return ims_Error_E_SMS_SEND_FAIL_RETRY;
  }

  switch(in.reason) {
    case imsaidl::SmsSendFailureReason::FDN_CHECK_FAILURE:
      return ims_Error_E_FDN_CHECK_FAILURE;
    case imsaidl::SmsSendFailureReason::RADIO_NOT_AVAILABLE:
      return ims_Error_E_RADIO_NOT_AVAILABLE;
    case imsaidl::SmsSendFailureReason::CANCELLED:
      return ims_Error_E_CANCELLED;
    case imsaidl::SmsSendFailureReason::REQUEST_NOT_SUPPORTED:
      return ims_Error_E_REQUEST_NOT_SUPPORTED;
    default:
      return ims_Error_E_GENERIC_FAILURE;
  }
}

ims_Error convertFromAidl(
    const imsaidl::SmsSendResponse& in, ims_SmsSendResponse &out) {
  out.msgRef = in.msgRef;

  switch(in.smsStatus) {
    case imsaidl::SmsSendStatus::OK:
      out.smsStatus = ims_Error_E_SUCCESS;
      break;
    case imsaidl::SmsSendStatus::ERROR_RETRY:
    case imsaidl::SmsSendStatus::ERROR_FALLBACK:
      out.smsStatus = ims_Error_E_SMS_SEND_FAIL_RETRY;
      break;
    case imsaidl::SmsSendStatus::ERROR:
    default:
      out.smsStatus = ims_Error_E_GENERIC_FAILURE;
      break;
  }

  switch(in.reason) {
    case imsaidl::SmsSendFailureReason::NONE:
      out.reason = ims_Error_E_SUCCESS;
      break;
    case imsaidl::SmsSendFailureReason::FDN_CHECK_FAILURE:
      out.reason = ims_Error_E_FDN_CHECK_FAILURE;
      break;
    case imsaidl::SmsSendFailureReason::RADIO_NOT_AVAILABLE:
      out.reason = ims_Error_E_RADIO_NOT_AVAILABLE;
      break;
    case imsaidl::SmsSendFailureReason::CANCELLED:
      out.reason = ims_Error_E_CANCELLED;
      break;
    case imsaidl::SmsSendFailureReason::REQUEST_NOT_SUPPORTED:
      out.reason = ims_Error_E_REQUEST_NOT_SUPPORTED;
      break;
    default:
      out.reason = ims_Error_E_GENERIC_FAILURE;
      break;
  }

  out.networkErrorCode = in.networkErrorCode;
  out.transportErrorCode = in.transportErrorCode;

  switch(in.radioTech) {
    case imsaidl::RadioTechType::LTE:
      out.radioTech = ims_RadioTechType_RADIO_TECH_LTE;
      break;
    default:
      out.radioTech = ims_RadioTechType_RADIO_TECH_UNKNOWN;
      break;
  }

  return ims_Error_E_SUCCESS;
}

ims_Error convertFromAidl(const imsaidl::IncomingSms& in,
    utf_ims_mt_sms_t& out) {
  out.tech = (in.format == "3gpp") ? RADIO_TECH_3GPP : RADIO_TECH_3GPP2;
  size_t len = in.pdu.size();
  out.pdu = (uint8_t*)malloc(len);
  assert(out.pdu != nullptr);
  memcpy(out.pdu, in.pdu.data(), len);
  out.size = len;
  return ims_Error_E_SUCCESS;
}

ims_Error convertFromAidl(const imsaidl::SmsSendStatusReport& in,
    utf_ims_sms_status_report_t& out) {
  out.tech = (in.format == "3gpp") ? RADIO_TECH_3GPP : RADIO_TECH_3GPP2;
  size_t len = in.pdu.size();
  out.pdu = (uint8_t*)malloc(len);
  assert(out.pdu != nullptr);
  memcpy(out.pdu, in.pdu.data(), len);
  out.size = len;
  return ims_Error_E_SUCCESS;
}

void convertToAidl(const ims_Answer& in,
    imsaidl::AnswerRequest& out) {
  out.callType = imsaidl::CallType::UNKNOWN;
  if (in.has_call_type) {
    convertToAidl(in.call_type, out.callType);
  }
  out.presentation = imsaidl::IpPresentation::INVALID;
  if (in.has_presentation) {
    convertToAidl(in.presentation, out.presentation);
  }
  out.mode = imsaidl::RttMode::INVALID;
  if (in.has_rttMode) {
    convertToAidl(in.rttMode, out.mode);
  }
}

void convertFromAidl(const imsaidl::CallFailCause& in,
      ims_CallFailCause& out) {
    switch(in) {
        case imsaidl::CallFailCause::SRV_NOT_REGISTERED:
            out = ims_CallFailCause_CALL_FAIL_SRV_NOT_REGISTERED;
            break;
        case imsaidl::CallFailCause::CALL_TYPE_NOT_ALLOWED:
            out = ims_CallFailCause_CALL_FAIL_CALL_TYPE_NOT_ALLOWED;
            break;
        case imsaidl::CallFailCause::EMRG_CALL_ONGOING:
            out = ims_CallFailCause_CALL_FAIL_EMRG_CALL_ONGOING;
            break;
        case imsaidl::CallFailCause::CALL_SETUP_ONGOING:
            out = ims_CallFailCause_CALL_FAIL_CALL_SETUP_ONGOING;
            break;
        case imsaidl::CallFailCause::MAX_CALL_LIMIT_REACHED:
            out = ims_CallFailCause_CALL_FAIL_MAX_CALL_LIMIT_REACHED;
            break;
        case imsaidl::CallFailCause::UNSUPPORTED_SIP_HDRS:
            out = ims_CallFailCause_CALL_FAIL_UNSUPPORTED_SIP_HDRS;
            break;
        case imsaidl::CallFailCause::CALL_TRANSFER_ONGOING:
            out = ims_CallFailCause_CALL_FAIL_CALL_TRANSFER_ONGOING;
            break;
        case imsaidl::CallFailCause::PRACK_TIMEOUT:
            out = ims_CallFailCause_CALL_FAIL_PRACK_TIMEOUT;
            break;
        case imsaidl::CallFailCause::QOS_FAILURE:
            out = ims_CallFailCause_CALL_FAIL_QOS_FAILURE;
            break;
        case imsaidl::CallFailCause::ONGOING_HANDOVER:
            out = ims_CallFailCause_CALL_FAIL_ONGOING_HANDOVER;
            break;
        case imsaidl::CallFailCause::VT_WITH_TTY_NOT_ALLOWED:
            out = ims_CallFailCause_CALL_FAIL_VT_WITH_TTY_NOT_ALLOWED;
            break;
        case imsaidl::CallFailCause::CALL_UPGRADE_ONGOING:
            out = ims_CallFailCause_CALL_FAIL_CALL_UPGRADE_ONGOING;
            break;
        case imsaidl::CallFailCause::CONFERENCE_WITH_TTY_NOT_ALLOWED:
            out = ims_CallFailCause_CALL_FAIL_CONFERENCE_WITH_TTY_NOT_ALLOWED;
            break;
        case imsaidl::CallFailCause::CALL_CONFERENCE_ONGOING:
            out = ims_CallFailCause_CALL_FAIL_CALL_CONFERENCE_ONGOING;
            break;
        case imsaidl::CallFailCause::VT_WITH_AVPF_NOT_ALLOWED:
            out = ims_CallFailCause_CALL_FAIL_VT_WITH_AVPF_NOT_ALLOWED;
            break;
        case imsaidl::CallFailCause::ENCRYPTION_CALL_ONGOING:
            out = ims_CallFailCause_CALL_FAIL_ENCRYPTION_CALL_ONGOING;
            break;
        case imsaidl::CallFailCause::CALL_ONGOING_CW_DISABLED:
            out = ims_CallFailCause_CALL_FAIL_CALL_ONGOING_CW_DISABLED;
            break;
        case imsaidl::CallFailCause::CALL_ON_OTHER_SUB:
            out = ims_CallFailCause_CALL_FAIL_CALL_ON_OTHER_SUB;
            break;
        case imsaidl::CallFailCause::COLLISION_1X:
            out = ims_CallFailCause_CALL_FAIL_1X_COLLISION;
            break;
        case imsaidl::CallFailCause::UI_NOT_READY:
            out = ims_CallFailCause_CALL_FAIL_UI_NOT_READY;
            break;
        case imsaidl::CallFailCause::CS_CALL_ONGOING:
            out = ims_CallFailCause_CALL_FAIL_CS_CALL_ONGOING;
            break;
        case imsaidl::CallFailCause::REJECTED_ELSEWHERE:
            out = ims_CallFailCause_CALL_FAIL_REJECTED_ELSEWHERE;
            break;
        case imsaidl::CallFailCause::USER_REJECTED_SESSION_MODIFICATION:
            out = ims_CallFailCause_CALL_FAIL_USER_REJECTED_SESSION_MODIFICATION;
            break;
        case imsaidl::CallFailCause::SESSION_MODIFICATION_FAILED:
            out = ims_CallFailCause_CALL_FAIL_SESSION_MODIFICATION_FAILED;
            break;
        case imsaidl::CallFailCause::USER_CANCELLED_SESSION_MODIFICATION:
            out = ims_CallFailCause_CALL_FAIL_USER_CANCELLED_SESSION_MODIFICATION;
            break;
        case imsaidl::CallFailCause::SIP_METHOD_NOT_ALLOWED:
            out = ims_CallFailCause_CALL_FAIL_SIP_METHOD_NOT_ALLOWED;
            break;
        case imsaidl::CallFailCause::SIP_PROXY_AUTHENTICATION_REQUIRED:
            out = ims_CallFailCause_CALL_FAIL_SIP_PROXY_AUTHENTICATION_REQUIRED;
            break;
        case imsaidl::CallFailCause::SIP_REQUEST_ENTITY_TOO_LARGE:
            out = ims_CallFailCause_CALL_FAIL_SIP_REQUEST_ENTITY_TOO_LARGE;
            break;
        case imsaidl::CallFailCause::SIP_REQUEST_URI_TOO_LARGE:
            out = ims_CallFailCause_CALL_FAIL_SIP_REQUEST_URI_TOO_LARGE;
            break;
        case imsaidl::CallFailCause::SIP_EXTENSION_REQUIRED:
            out = ims_CallFailCause_CALL_FAIL_SIP_EXTENSION_REQUIRED;
            break;
        case imsaidl::CallFailCause::SIP_INTERVAL_TOO_BRIEF:
            out = ims_CallFailCause_CALL_FAIL_SIP_INTERVAL_TOO_BRIEF;
            break;
        case imsaidl::CallFailCause::SIP_CALL_OR_TRANS_DOES_NOT_EXIST:
            out = ims_CallFailCause_CALL_FAIL_SIP_CALL_OR_TRANS_DOES_NOT_EXIST;
            break;
        case imsaidl::CallFailCause::SIP_LOOP_DETECTED:
            out = ims_CallFailCause_CALL_FAIL_SIP_LOOP_DETECTED;
            break;
        case imsaidl::CallFailCause::SIP_TOO_MANY_HOPS:
            out = ims_CallFailCause_CALL_FAIL_SIP_TOO_MANY_HOPS;
            break;
        case imsaidl::CallFailCause::SIP_AMBIGUOUS:
            out = ims_CallFailCause_CALL_FAIL_SIP_AMBIGUOUS;
            break;
        case imsaidl::CallFailCause::SIP_REQUEST_PENDING:
            out = ims_CallFailCause_CALL_FAIL_SIP_REQUEST_PENDING;
            break;
        case imsaidl::CallFailCause::SIP_UNDECIPHERABLE:
            out = ims_CallFailCause_CALL_FAIL_SIP_UNDECIPHERABLE;
            break;
        case imsaidl::CallFailCause::RETRY_ON_IMS_WITHOUT_RTT:
            out = ims_CallFailCause_CALL_FAIL_RETRY_ON_IMS_WITHOUT_RTT;
            break;
        case imsaidl::CallFailCause::SIP_USER_MARKED_UNWANTED:
            out = ims_CallFailCause_CALL_FAIL_SIP_USER_MARKED_UNWANTED;
            break;
        case imsaidl::CallFailCause::RADIO_OFF:
            out = ims_CallFailCause_CALL_FAIL_RADIO_OFF;
            break;
        case imsaidl::CallFailCause::OUT_OF_SERVICE:
            out = ims_CallFailCause_CALL_FAIL_OUT_OF_SERVICE;
            break;
        case imsaidl::CallFailCause::NO_VALID_SIM:
            out = ims_CallFailCause_CALL_FAIL_NO_VALID_SIM;
            break;
        case imsaidl::CallFailCause::RADIO_INTERNAL_ERROR:
            out = ims_CallFailCause_CALL_FAIL_RADIO_INTERNAL_ERROR;
            break;
        case imsaidl::CallFailCause::NETWORK_RESP_TIMEOUT:
            out = ims_CallFailCause_CALL_FAIL_NETWORK_RESP_TIMEOUT;
            break;
        case imsaidl::CallFailCause::NETWORK_REJECT:
            out = ims_CallFailCause_CALL_FAIL_NETWORK_REJECT;
            break;
        case imsaidl::CallFailCause::RADIO_ACCESS_FAILURE:
            out = ims_CallFailCause_CALL_FAIL_RADIO_ACCESS_FAILURE;
            break;
        case imsaidl::CallFailCause::RADIO_LINK_FAILURE:
            out = ims_CallFailCause_CALL_FAIL_RADIO_LINK_FAILURE;
            break;
        case imsaidl::CallFailCause::RADIO_LINK_LOST:
            out = ims_CallFailCause_CALL_FAIL_RADIO_LINK_LOST;
            break;
        case imsaidl::CallFailCause::RADIO_UPLINK_FAILURE:
            out = ims_CallFailCause_CALL_FAIL_RADIO_UPLINK_FAILURE;
            break;
        case imsaidl::CallFailCause::RADIO_SETUP_FAILURE:
            out = ims_CallFailCause_CALL_FAIL_RADIO_SETUP_FAILURE;
            break;
        case imsaidl::CallFailCause::RADIO_RELEASE_NORMAL:
            out = ims_CallFailCause_CALL_FAIL_RADIO_RELEASE_NORMAL;
            break;
        case imsaidl::CallFailCause::RADIO_RELEASE_ABNORMAL:
            out = ims_CallFailCause_CALL_FAIL_RADIO_RELEASE_ABNORMAL;
            break;
        case imsaidl::CallFailCause::NETWORK_DETACH:
            out = ims_CallFailCause_CALL_FAIL_NETWORK_DETACH;
            break;
        case imsaidl::CallFailCause::NO_CSFB_IN_CS_ROAM:
            out = ims_CallFailCause_CALL_FAIL_NO_CSFB_IN_CS_ROAM;
            break;
        case imsaidl::CallFailCause::SIP_ALTERNATE_EMERGENCY_CALL:
            out = ims_CallFailCause_CALL_FAIL_SIP_ALTERNATE_EMERGENCY_CALL;
            break;
      case imsaidl::CallFailCause::UNOBTAINABLE_NUMBER:
            out = ims_CallFailCause_CALL_FAIL_UNOBTAINABLE_NUMBER;
            break;
      case imsaidl::CallFailCause::NORMAL:
            out = ims_CallFailCause_CALL_FAIL_NORMAL;
            break;
      case imsaidl::CallFailCause::BUSY:
            out = ims_CallFailCause_CALL_FAIL_BUSY;
            break;
      case imsaidl::CallFailCause::NORMAL_UNSPECIFIED:
            out = ims_CallFailCause_CALL_FAIL_NORMAL_UNSPECIFIED;
            break;
      case imsaidl::CallFailCause::CONGESTION:
            out = ims_CallFailCause_CALL_FAIL_CONGESTION;
            break;
      case imsaidl::CallFailCause::INCOMPATIBILITY_DESTINATION:
            out = ims_CallFailCause_CALL_FAIL_INCOMPATIBILITY_DESTINATION;
            break;
      case imsaidl::CallFailCause::CALL_BARRED:
            out = ims_CallFailCause_CALL_FAIL_CALL_BARRED;
            break;
      case imsaidl::CallFailCause::FDN_BLOCKED:
            out = ims_CallFailCause_CALL_FAIL_FDN_BLOCKED;
            break;
      case imsaidl::CallFailCause::IMEI_NOT_ACCEPTED:
            out = ims_CallFailCause_CALL_FAIL_IMEI_NOT_ACCEPTED;
            break;
      case imsaidl::CallFailCause::DIAL_MODIFIED_TO_USSD:
            out = ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_USSD;
            break;
      case imsaidl::CallFailCause::DIAL_MODIFIED_TO_SS:
            out = ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_SS;
            break;
      case imsaidl::CallFailCause::DIAL_MODIFIED_TO_DIAL:
            out = ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_DIAL;
            break;
      case imsaidl::CallFailCause::EMERGENCY_TEMP_FAILURE:
            out = ims_CallFailCause_CALL_FAIL_EMERGENCY_TEMP_FAILURE;
            break;
      case imsaidl::CallFailCause::EMERGENCY_PERM_FAILURE:
            out = ims_CallFailCause_CALL_FAIL_EMERGENCY_PERM_FAILURE;
            break;
      case imsaidl::CallFailCause::HO_NOT_FEASIBLE:
            out = ims_CallFailCause_CALL_FAIL_HO_NOT_FEASIBLE;
            break;
      case imsaidl::CallFailCause::USER_BUSY:
            out = ims_CallFailCause_CALL_FAIL_USER_BUSY;
            break;
      case imsaidl::CallFailCause::USER_REJECT:
            out = ims_CallFailCause_CALL_FAIL_USER_REJECT;
            break;
      case imsaidl::CallFailCause::LOW_BATTERY:
            out = ims_CallFailCause_CALL_FAIL_LOW_BATTERY;
            break;
      case imsaidl::CallFailCause::BLACKLISTED_CALL_ID:
            out = ims_CallFailCause_CALL_FAIL_BLACKLISTED_CALL_ID;
            break;
      case imsaidl::CallFailCause::CS_RETRY_REQUIRED:
            out = ims_CallFailCause_CALL_FAIL_CS_RETRY_REQUIRED;
            break;
      case imsaidl::CallFailCause::NETWORK_UNAVAILABLE:
            out = ims_CallFailCause_CALL_FAIL_NETWORK_UNAVAILABLE;
            break;
      case imsaidl::CallFailCause::FEATURE_UNAVAILABLE:
            out = ims_CallFailCause_CALL_FAIL_FEATURE_UNAVAILABLE;
            break;
      case imsaidl::CallFailCause::SIP_ERROR:
            out = ims_CallFailCause_CALL_FAIL_SIP_Error;
            break;
      case imsaidl::CallFailCause::MISC:
            out = ims_CallFailCause_CALL_FAIL_MISC;
            break;
      case imsaidl::CallFailCause::ANSWERED_ELSEWHERE:
            out = ims_CallFailCause_CALL_FAIL_ANSWERED_ELSEWHERE;
            break;
      case imsaidl::CallFailCause::PULL_OUT_OF_SYNC:
            out = ims_CallFailCause_CALL_FAIL_PULL_OUT_OF_SYNC;
            break;
      case imsaidl::CallFailCause::CAUSE_CALL_PULLED:
            out = ims_CallFailCause_CALL_FAIL_CAUSE_CALL_PULLED;
            break;
      case imsaidl::CallFailCause::SIP_REDIRECTED:
            out = ims_CallFailCause_CALL_FAIL_SIP_REDIRECTED;
            break;
      case imsaidl::CallFailCause::SIP_BAD_REQUEST:
            out = ims_CallFailCause_CALL_FAIL_SIP_BAD_REQUEST;
            break;
      case imsaidl::CallFailCause::SIP_FORBIDDEN:
            out = ims_CallFailCause_CALL_FAIL_SIP_FORBIDDEN;
            break;
      case imsaidl::CallFailCause::SIP_NOT_FOUND:
            out = ims_CallFailCause_CALL_FAIL_SIP_NOT_FOUND;
            break;
      case imsaidl::CallFailCause::SIP_NOT_SUPPORTED:
            out = ims_CallFailCause_CALL_FAIL_SIP_NOT_SUPPORTED;
            break;
      case imsaidl::CallFailCause::SIP_REQUEST_TIMEOUT:
            out = ims_CallFailCause_CALL_FAIL_SIP_REQUEST_TIMEOUT;
            break;
      case imsaidl::CallFailCause::SIP_TEMPORARILY_UNAVAILABLE:
            out = ims_CallFailCause_CALL_FAIL_SIP_TEMPORARILY_UNAVAILABLE;
            break;
      case imsaidl::CallFailCause::SIP_BAD_ADDRESS:
            out = ims_CallFailCause_CALL_FAIL_SIP_BAD_ADDRESS;
            break;
      case imsaidl::CallFailCause::SIP_BUSY:
            out = ims_CallFailCause_CALL_FAIL_SIP_BUSY;
            break;
      case imsaidl::CallFailCause::SIP_REQUEST_CANCELLED:
            out = ims_CallFailCause_CALL_FAIL_SIP_REQUEST_CANCELLED;
            break;
      case imsaidl::CallFailCause::SIP_NOT_ACCEPTABLE:
            out = ims_CallFailCause_CALL_FAIL_SIP_NOT_ACCEPTABLE;
            break;
      case imsaidl::CallFailCause::SIP_NOT_REACHABLE:
            out = ims_CallFailCause_CALL_FAIL_SIP_NOT_REACHABLE;
            break;
      case imsaidl::CallFailCause::SIP_SERVER_INTERNAL_ERROR:
            out = ims_CallFailCause_CALL_FAIL_SIP_SERVER_INTERNAL_ERROR;
            break;
      case imsaidl::CallFailCause::SIP_SERVER_NOT_IMPLEMENTED:
            out = ims_CallFailCause_CALL_FAIL_SIP_SERVER_NOT_IMPLEMENTED;
            break;
      case imsaidl::CallFailCause::SIP_SERVER_BAD_GATEWAY:
            out = ims_CallFailCause_CALL_FAIL_SIP_SERVER_BAD_GATEWAY;
            break;
      case imsaidl::CallFailCause::SIP_SERVICE_UNAVAILABLE:
            out = ims_CallFailCause_CALL_FAIL_SIP_SERVICE_UNAVAILABLE;
            break;
      case imsaidl::CallFailCause::SIP_SERVER_TIMEOUT:
            out = ims_CallFailCause_CALL_FAIL_SIP_SERVER_TIMEOUT;
            break;
      case imsaidl::CallFailCause::SIP_SERVER_VERSION_UNSUPPORTED:
            out = ims_CallFailCause_CALL_FAIL_SIP_SERVER_VERSION_UNSUPPORTED;
            break;
      case imsaidl::CallFailCause::SIP_SERVER_MESSAGE_TOOLARGE:
            out = ims_CallFailCause_CALL_FAIL_SIP_SERVER_MESSAGE_TOOLARGE;
            break;
      case imsaidl::CallFailCause::SIP_SERVER_PRECONDITION_FAILURE:
            out = ims_CallFailCause_CALL_FAIL_SIP_SERVER_PRECONDITION_FAILURE;
            break;
      case imsaidl::CallFailCause::SIP_USER_REJECTED:
            out = ims_CallFailCause_CALL_FAIL_SIP_USER_REJECTED;
            break;
      case imsaidl::CallFailCause::SIP_GLOBAL_ERROR:
            out = ims_CallFailCause_CALL_FAIL_SIP_GLOBAL_ERROR;
            break;
      case imsaidl::CallFailCause::MEDIA_INIT_FAILED:
            out = ims_CallFailCause_CALL_FAIL_MEDIA_INIT_FAILED;
            break;
      case imsaidl::CallFailCause::MEDIA_NO_DATA:
            out = ims_CallFailCause_CALL_FAIL_MEDIA_NO_DATA;
            break;
      case imsaidl::CallFailCause::MEDIA_NOT_ACCEPTABLE:
            out = ims_CallFailCause_CALL_FAIL_MEDIA_NOT_ACCEPTABLE;
            break;
      case imsaidl::CallFailCause::MEDIA_UNSPECIFIED_ERROR:
            out = ims_CallFailCause_CALL_FAIL_MEDIA_UNSPECIFIED_ERROR;
            break;
      case imsaidl::CallFailCause::HOLD_RESUME_FAILED:
            out = ims_CallFailCause_CALL_FAIL_HOLD_RESUME_FAILED;
            break;
      case imsaidl::CallFailCause::HOLD_RESUME_CANCELED:
            out = ims_CallFailCause_CALL_FAIL_HOLD_RESUME_CANCELED;
            break;
      case imsaidl::CallFailCause::HOLD_REINVITE_COLLISION:
            out = ims_CallFailCause_CALL_FAIL_HOLD_REINVITE_COLLISION;
            break;
      case imsaidl::CallFailCause::ACCESS_CLASS_BLOCKED:
            out = ims_CallFailCause_CALL_FAIL_ACCESS_CLASS_BLOCKED;
            break;
      case imsaidl::CallFailCause::DIAL_MODIFIED_TO_DIAL_VIDEO:
            out = ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_DIAL_VIDEO;
            break;
      case imsaidl::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL:
            out = ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL;
            break;
      case imsaidl::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
            out = ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
            break;
      case imsaidl::CallFailCause::DIAL_VIDEO_MODIFIED_TO_SS:
            out = ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_SS;
            break;
      case imsaidl::CallFailCause::DIAL_VIDEO_MODIFIED_TO_USSD:
            out = ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_USSD;
            break;
      case imsaidl::CallFailCause::DSDA_CONCURRENT_CALL_NOT_POSSIBLE:
        out = ims_CallFailCause_CALL_FAIL_DSDA_CONCURRENT_CALL_NOT_POSSIBLE;
        break;
      case imsaidl::CallFailCause::EPSFB_FAILURE:
        out = ims_CallFailCause_CALL_FAIL_EPSFB_FAILURE;
        break;
      case imsaidl::CallFailCause::TWAIT_EXPIRED:
        out = ims_CallFailCause_CALL_FAIL_TWAIT_EXPIRED;
        break;
      case imsaidl::CallFailCause::TCP_CONNECTION_REQ:
        out = ims_CallFailCause_CALL_FAIL_TCP_CONNECTION_REQ;
        break;
      case imsaidl::CallFailCause::ERROR_UNSPECIFIED:
      case imsaidl::CallFailCause::INVALID:
      default:
            out = ims_CallFailCause_CALL_FAIL_ERROR_UNSPECIFIED;
            break;
    }
}

void convertToAidl(const ims_CallFailCause& in,
      imsaidl::CallFailCause& out) {
    switch (in) {
        case ims_CallFailCause_CALL_FAIL_UNOBTAINABLE_NUMBER:
            out = imsaidl::CallFailCause::UNOBTAINABLE_NUMBER;
            break;
        case ims_CallFailCause_CALL_FAIL_NORMAL:
            out = imsaidl::CallFailCause::NORMAL;
            break;
        case ims_CallFailCause_CALL_FAIL_BUSY:
            out = imsaidl::CallFailCause::BUSY;
            break;
        case ims_CallFailCause_CALL_FAIL_NORMAL_UNSPECIFIED:
            out = imsaidl::CallFailCause::NORMAL_UNSPECIFIED;
            break;
        case ims_CallFailCause_CALL_FAIL_CONGESTION:
            out = imsaidl::CallFailCause::CONGESTION;
            break;
        case ims_CallFailCause_CALL_FAIL_INCOMPATIBILITY_DESTINATION:
            out = imsaidl::CallFailCause::INCOMPATIBILITY_DESTINATION;
            break;
        case ims_CallFailCause_CALL_FAIL_CALL_BARRED:
            out = imsaidl::CallFailCause::CALL_BARRED;
            break;
        case ims_CallFailCause_CALL_FAIL_FDN_BLOCKED:
            out = imsaidl::CallFailCause::FDN_BLOCKED;
            break;
        case ims_CallFailCause_CALL_FAIL_IMEI_NOT_ACCEPTED:
            out = imsaidl::CallFailCause::IMEI_NOT_ACCEPTED;
            break;
        case ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_USSD:
            out = imsaidl::CallFailCause::DIAL_MODIFIED_TO_USSD;
            break;
        case ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_SS:
            out = imsaidl::CallFailCause::DIAL_MODIFIED_TO_SS;
            break;
        case ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_DIAL:
            out = imsaidl::CallFailCause::DIAL_MODIFIED_TO_DIAL;
            break;
        case ims_CallFailCause_CALL_FAIL_EMERGENCY_TEMP_FAILURE:
            out = imsaidl::CallFailCause::EMERGENCY_TEMP_FAILURE;
            break;
        case ims_CallFailCause_CALL_FAIL_EMERGENCY_PERM_FAILURE:
            out = imsaidl::CallFailCause::EMERGENCY_PERM_FAILURE;
            break;
        case ims_CallFailCause_CALL_FAIL_HO_NOT_FEASIBLE:
            out = imsaidl::CallFailCause::HO_NOT_FEASIBLE;
            break;
        case ims_CallFailCause_CALL_FAIL_USER_BUSY:
            out = imsaidl::CallFailCause::USER_BUSY;
            break;
        case ims_CallFailCause_CALL_FAIL_USER_REJECT:
            out = imsaidl::CallFailCause::USER_REJECT;
            break;
        case ims_CallFailCause_CALL_FAIL_LOW_BATTERY:
            out = imsaidl::CallFailCause::LOW_BATTERY;
            break;
        case ims_CallFailCause_CALL_FAIL_BLACKLISTED_CALL_ID:
            out = imsaidl::CallFailCause::BLACKLISTED_CALL_ID;
            break;
        case ims_CallFailCause_CALL_FAIL_CS_RETRY_REQUIRED:
            out = imsaidl::CallFailCause::CS_RETRY_REQUIRED;
            break;
        case ims_CallFailCause_CALL_FAIL_NETWORK_UNAVAILABLE:
            out = imsaidl::CallFailCause::NETWORK_UNAVAILABLE;
            break;
        case ims_CallFailCause_CALL_FAIL_FEATURE_UNAVAILABLE:
            out = imsaidl::CallFailCause::FEATURE_UNAVAILABLE;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_Error:
            out = imsaidl::CallFailCause::SIP_ERROR;
            break;
        case ims_CallFailCause_CALL_FAIL_MISC:
            out = imsaidl::CallFailCause::MISC;
            break;
        case ims_CallFailCause_CALL_FAIL_ANSWERED_ELSEWHERE:
            out = imsaidl::CallFailCause::ANSWERED_ELSEWHERE;
            break;
        case ims_CallFailCause_CALL_FAIL_PULL_OUT_OF_SYNC:
            out = imsaidl::CallFailCause::PULL_OUT_OF_SYNC;
            break;
        case ims_CallFailCause_CALL_FAIL_CAUSE_CALL_PULLED:
            out = imsaidl::CallFailCause::CAUSE_CALL_PULLED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_REDIRECTED:
            out = imsaidl::CallFailCause::SIP_REDIRECTED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_BAD_REQUEST:
            out = imsaidl::CallFailCause::SIP_BAD_REQUEST;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_FORBIDDEN:
            out = imsaidl::CallFailCause::SIP_FORBIDDEN;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_NOT_FOUND:
            out = imsaidl::CallFailCause::SIP_NOT_FOUND;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_NOT_SUPPORTED:
            out = imsaidl::CallFailCause::SIP_NOT_SUPPORTED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_REQUEST_TIMEOUT:
            out = imsaidl::CallFailCause::SIP_REQUEST_TIMEOUT;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_TEMPORARILY_UNAVAILABLE:
            out = imsaidl::CallFailCause::SIP_TEMPORARILY_UNAVAILABLE;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_BAD_ADDRESS:
            out = imsaidl::CallFailCause::SIP_BAD_ADDRESS;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_BUSY:
            out = imsaidl::CallFailCause::SIP_BUSY;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_REQUEST_CANCELLED:
            out = imsaidl::CallFailCause::SIP_REQUEST_CANCELLED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_NOT_ACCEPTABLE:
            out = imsaidl::CallFailCause::SIP_NOT_ACCEPTABLE;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_NOT_REACHABLE:
            out = imsaidl::CallFailCause::SIP_NOT_REACHABLE;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_INTERNAL_ERROR:
            out = imsaidl::CallFailCause::SIP_SERVER_INTERNAL_ERROR;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_NOT_IMPLEMENTED:
            out = imsaidl::CallFailCause::SIP_SERVER_NOT_IMPLEMENTED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_BAD_GATEWAY:
            out = imsaidl::CallFailCause::SIP_SERVER_BAD_GATEWAY;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVICE_UNAVAILABLE:
            out = imsaidl::CallFailCause::SIP_SERVICE_UNAVAILABLE;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_TIMEOUT:
            out = imsaidl::CallFailCause::SIP_SERVER_TIMEOUT;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_VERSION_UNSUPPORTED:
            out = imsaidl::CallFailCause::SIP_SERVER_VERSION_UNSUPPORTED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_MESSAGE_TOOLARGE:
            out = imsaidl::CallFailCause::SIP_SERVER_MESSAGE_TOOLARGE;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_PRECONDITION_FAILURE:
            out = imsaidl::CallFailCause::SIP_SERVER_PRECONDITION_FAILURE;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_USER_REJECTED:
            out = imsaidl::CallFailCause::SIP_USER_REJECTED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_GLOBAL_ERROR:
            out = imsaidl::CallFailCause::SIP_GLOBAL_ERROR;
            break;
        case ims_CallFailCause_CALL_FAIL_MEDIA_INIT_FAILED:
            out = imsaidl::CallFailCause::MEDIA_INIT_FAILED;
            break;
        case ims_CallFailCause_CALL_FAIL_MEDIA_NO_DATA:
            out = imsaidl::CallFailCause::MEDIA_NO_DATA;
            break;
        case ims_CallFailCause_CALL_FAIL_MEDIA_NOT_ACCEPTABLE:
            out = imsaidl::CallFailCause::MEDIA_NOT_ACCEPTABLE;
            break;
        case ims_CallFailCause_CALL_FAIL_MEDIA_UNSPECIFIED_ERROR:
            out = imsaidl::CallFailCause::MEDIA_UNSPECIFIED_ERROR;
            break;
        case ims_CallFailCause_CALL_FAIL_HOLD_RESUME_FAILED:
            out = imsaidl::CallFailCause::HOLD_RESUME_FAILED;
            break;
        case ims_CallFailCause_CALL_FAIL_HOLD_RESUME_CANCELED:
            out = imsaidl::CallFailCause::HOLD_RESUME_CANCELED;
            break;
        case ims_CallFailCause_CALL_FAIL_HOLD_REINVITE_COLLISION:
            out = imsaidl::CallFailCause::HOLD_REINVITE_COLLISION;
            break;
        case ims_CallFailCause_CALL_FAIL_ACCESS_CLASS_BLOCKED:
            out = imsaidl::CallFailCause::ACCESS_CLASS_BLOCKED;
            break;
        case ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_DIAL_VIDEO:
            out = imsaidl::CallFailCause::DIAL_MODIFIED_TO_DIAL_VIDEO;
            break;
        case ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL:
            out = imsaidl::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL;
            break;
        case ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
            out = imsaidl::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
            break;
        case ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_SS:
            out = imsaidl::CallFailCause::DIAL_VIDEO_MODIFIED_TO_SS;
            break;
        case ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_USSD:
            out = imsaidl::CallFailCause::DIAL_VIDEO_MODIFIED_TO_USSD;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_ALTERNATE_EMERGENCY_CALL:
            out = imsaidl::CallFailCause::SIP_ALTERNATE_EMERGENCY_CALL;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_USER_MARKED_UNWANTED:
            out = imsaidl::CallFailCause::SIP_USER_MARKED_UNWANTED;
            break;
        case ims_CallFailCause_CALL_FAIL_RADIO_OFF:
            out = imsaidl::CallFailCause::RADIO_OFF;
            break;
        case ims_CallFailCause_CALL_FAIL_OUT_OF_SERVICE:
            out = imsaidl::CallFailCause::OUT_OF_SERVICE;
            break;
        case ims_CallFailCause_CALL_FAIL_NO_VALID_SIM:
            out = imsaidl::CallFailCause::NO_VALID_SIM;
            break;
        case ims_CallFailCause_CALL_FAIL_RADIO_INTERNAL_ERROR:
            out = imsaidl::CallFailCause::RADIO_INTERNAL_ERROR;
            break;
        case ims_CallFailCause_CALL_FAIL_NETWORK_RESP_TIMEOUT:
            out = imsaidl::CallFailCause::NETWORK_RESP_TIMEOUT;
            break;
        case ims_CallFailCause_CALL_FAIL_NETWORK_REJECT:
            out = imsaidl::CallFailCause::NETWORK_REJECT;
            break;
        case ims_CallFailCause_CALL_FAIL_RADIO_ACCESS_FAILURE:
            out = imsaidl::CallFailCause::RADIO_ACCESS_FAILURE;
            break;
        case ims_CallFailCause_CALL_FAIL_RADIO_LINK_FAILURE:
            out = imsaidl::CallFailCause::RADIO_LINK_FAILURE;
            break;
        case ims_CallFailCause_CALL_FAIL_RADIO_LINK_LOST:
            out = imsaidl::CallFailCause::RADIO_LINK_LOST;
            break;
        case ims_CallFailCause_CALL_FAIL_RADIO_UPLINK_FAILURE:
            out = imsaidl::CallFailCause::RADIO_UPLINK_FAILURE;
            break;
        case ims_CallFailCause_CALL_FAIL_RADIO_SETUP_FAILURE:
            out = imsaidl::CallFailCause::RADIO_SETUP_FAILURE;
            break;
        case ims_CallFailCause_CALL_FAIL_RADIO_RELEASE_NORMAL:
            out = imsaidl::CallFailCause::RADIO_RELEASE_NORMAL;
            break;
        case ims_CallFailCause_CALL_FAIL_RADIO_RELEASE_ABNORMAL:
            out = imsaidl::CallFailCause::RADIO_RELEASE_ABNORMAL;
            break;
        case ims_CallFailCause_CALL_FAIL_NETWORK_DETACH:
            out = imsaidl::CallFailCause::NETWORK_DETACH;
            break;
        case ims_CallFailCause_CALL_FAIL_ERROR_UNSPECIFIED:
            out = imsaidl::CallFailCause::ERROR_UNSPECIFIED;
            break;
        case ims_CallFailCause_CALL_FAIL_NO_CSFB_IN_CS_ROAM:
            out = imsaidl::CallFailCause::NO_CSFB_IN_CS_ROAM;
            break;
        case ims_CallFailCause_CALL_FAIL_SRV_NOT_REGISTERED:
            out = imsaidl::CallFailCause::SRV_NOT_REGISTERED;
            break;
        case ims_CallFailCause_CALL_FAIL_CALL_TYPE_NOT_ALLOWED:
            out = imsaidl::CallFailCause::CALL_TYPE_NOT_ALLOWED;
            break;
        case ims_CallFailCause_CALL_FAIL_EMRG_CALL_ONGOING:
            out = imsaidl::CallFailCause::EMRG_CALL_ONGOING;
            break;
        case ims_CallFailCause_CALL_FAIL_CALL_SETUP_ONGOING:
            out = imsaidl::CallFailCause::CALL_SETUP_ONGOING;
            break;
        case ims_CallFailCause_CALL_FAIL_MAX_CALL_LIMIT_REACHED:
            out = imsaidl::CallFailCause::MAX_CALL_LIMIT_REACHED;
            break;
        case ims_CallFailCause_CALL_FAIL_UNSUPPORTED_SIP_HDRS:
            out = imsaidl::CallFailCause::UNSUPPORTED_SIP_HDRS;
            break;
        case ims_CallFailCause_CALL_FAIL_CALL_TRANSFER_ONGOING:
            out = imsaidl::CallFailCause::CALL_TRANSFER_ONGOING;
            break;
        case ims_CallFailCause_CALL_FAIL_PRACK_TIMEOUT:
            out = imsaidl::CallFailCause::PRACK_TIMEOUT;
            break;
        case ims_CallFailCause_CALL_FAIL_QOS_FAILURE:
            out = imsaidl::CallFailCause::QOS_FAILURE;
            break;
        case ims_CallFailCause_CALL_FAIL_ONGOING_HANDOVER:
            out = imsaidl::CallFailCause::ONGOING_HANDOVER;
            break;
        case ims_CallFailCause_CALL_FAIL_VT_WITH_TTY_NOT_ALLOWED:
            out = imsaidl::CallFailCause::VT_WITH_TTY_NOT_ALLOWED;
            break;
        case ims_CallFailCause_CALL_FAIL_CALL_UPGRADE_ONGOING:
            out = imsaidl::CallFailCause::CALL_UPGRADE_ONGOING;
            break;
        case ims_CallFailCause_CALL_FAIL_CONFERENCE_WITH_TTY_NOT_ALLOWED:
            out = imsaidl::CallFailCause::CONFERENCE_WITH_TTY_NOT_ALLOWED;
            break;
        case ims_CallFailCause_CALL_FAIL_CALL_CONFERENCE_ONGOING:
            out = imsaidl::CallFailCause::CALL_CONFERENCE_ONGOING;
            break;
        case ims_CallFailCause_CALL_FAIL_VT_WITH_AVPF_NOT_ALLOWED:
            out = imsaidl::CallFailCause::VT_WITH_AVPF_NOT_ALLOWED;
            break;
        case ims_CallFailCause_CALL_FAIL_ENCRYPTION_CALL_ONGOING:
            out = imsaidl::CallFailCause::ENCRYPTION_CALL_ONGOING;
            break;
        case ims_CallFailCause_CALL_FAIL_CALL_ONGOING_CW_DISABLED:
            out = imsaidl::CallFailCause::CALL_ONGOING_CW_DISABLED;
            break;
        case ims_CallFailCause_CALL_FAIL_CALL_ON_OTHER_SUB:
            out = imsaidl::CallFailCause::CALL_ON_OTHER_SUB;
            break;
        case ims_CallFailCause_CALL_FAIL_1X_COLLISION:
            out = imsaidl::CallFailCause::COLLISION_1X;
            break;
        case ims_CallFailCause_CALL_FAIL_UI_NOT_READY:
            out = imsaidl::CallFailCause::UI_NOT_READY;
            break;
        case ims_CallFailCause_CALL_FAIL_CS_CALL_ONGOING:
            out = imsaidl::CallFailCause::CS_CALL_ONGOING;
            break;
        case ims_CallFailCause_CALL_FAIL_REJECTED_ELSEWHERE:
            out = imsaidl::CallFailCause::REJECTED_ELSEWHERE;
            break;
        case ims_CallFailCause_CALL_FAIL_USER_REJECTED_SESSION_MODIFICATION:
            out = imsaidl::CallFailCause::USER_REJECTED_SESSION_MODIFICATION;
            break;
        case ims_CallFailCause_CALL_FAIL_SESSION_MODIFICATION_FAILED:
            out = imsaidl::CallFailCause::SESSION_MODIFICATION_FAILED;
            break;
        case ims_CallFailCause_CALL_FAIL_USER_CANCELLED_SESSION_MODIFICATION:
            out = imsaidl::CallFailCause::USER_CANCELLED_SESSION_MODIFICATION;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_METHOD_NOT_ALLOWED:
            out = imsaidl::CallFailCause::SIP_METHOD_NOT_ALLOWED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_PROXY_AUTHENTICATION_REQUIRED:
            out = imsaidl::CallFailCause::SIP_PROXY_AUTHENTICATION_REQUIRED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_REQUEST_ENTITY_TOO_LARGE:
            out = imsaidl::CallFailCause::SIP_REQUEST_ENTITY_TOO_LARGE;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_REQUEST_URI_TOO_LARGE:
            out = imsaidl::CallFailCause::SIP_REQUEST_URI_TOO_LARGE;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_EXTENSION_REQUIRED:
            out = imsaidl::CallFailCause::SIP_EXTENSION_REQUIRED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_INTERVAL_TOO_BRIEF:
            out = imsaidl::CallFailCause::SIP_INTERVAL_TOO_BRIEF;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_CALL_OR_TRANS_DOES_NOT_EXIST:
            out = imsaidl::CallFailCause::SIP_CALL_OR_TRANS_DOES_NOT_EXIST;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_LOOP_DETECTED:
            out = imsaidl::CallFailCause::SIP_LOOP_DETECTED;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_TOO_MANY_HOPS:
            out = imsaidl::CallFailCause::SIP_TOO_MANY_HOPS;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_AMBIGUOUS:
            out = imsaidl::CallFailCause::SIP_AMBIGUOUS;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_REQUEST_PENDING:
            out = imsaidl::CallFailCause::SIP_REQUEST_PENDING;
            break;
        case ims_CallFailCause_CALL_FAIL_SIP_UNDECIPHERABLE:
            out = imsaidl::CallFailCause::SIP_UNDECIPHERABLE;
            break;
        case ims_CallFailCause_CALL_FAIL_RETRY_ON_IMS_WITHOUT_RTT:
            out = imsaidl::CallFailCause::RETRY_ON_IMS_WITHOUT_RTT;
            break;
        default:
            out = imsaidl::CallFailCause::INVALID;
            break;
    }
}

void convertToAidl(const ims_SipErrorInfo& in,
    imsaidl::SipErrorInfo& out) {
  if (in.has_sipErrorCode) {
    out.errorCode = in.sipErrorCode;
  }

  const char* ptr = (const char*)in.sipErrorString.arg;
  if (ptr != nullptr) {
    out.errorString = ptr;
  }
}

void convertFromAidl(const imsaidl::CallFailCauseResponse& in,
        ims_CallFailCauseResponse& out) {
    qcril_binary_data_type* ptr = nullptr;
    ims_Error ret = ims_Error_E_SUCCESS;

    do {
        if (in.failCause != imsaidl::CallFailCause::INVALID) {
            out.has_failcause = true;
            convertFromAidl(in.failCause, out.failcause);
        }

        const uint32_t errorInfoLen = in.errorInfo.size();
        if (errorInfoLen > 0) {
            out.errorinfo.arg = qcril_malloc(sizeof(qcril_binary_data_type));

            if (out.errorinfo.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for error infg.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            ptr = (qcril_binary_data_type*)out.errorinfo.arg;
            ptr->len = errorInfoLen;
            ptr->data = (uint8_t*)qcril_malloc(errorInfoLen);

            if (ptr->data == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for error infg.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            memcpy(ptr->data, in.errorInfo.data(), errorInfoLen);
        }

        const uint32_t errorStringLen = in.networkErrorString.size();
        if (errorStringLen > 0) {
            size_t destSize = sizeof(char) * (errorStringLen + 1);
            out.networkErrorString.arg = qcril_malloc(destSize);

            if (out.networkErrorString.arg == nullptr) {
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            strlcpy((char*)out.networkErrorString.arg,
                    in.networkErrorString.c_str(), destSize);
        }
        if (in.hasErrorDetails) {
            out.has_errorDetails = true;
            convertFromAidl(in.errorDetails, out.errorDetails);
        }
    } while(0);

    if (isError(ret)) {
        if (ptr != nullptr) {
            if (ptr->data != nullptr) {
                qcril_free(ptr->data);
                ptr->data = nullptr;
            }
            qcril_free(ptr);
            ptr = nullptr;
        }

        if (out.networkErrorString.arg != nullptr) {
            qcril_free(out.networkErrorString.arg);
            out.networkErrorString.arg = nullptr;
        }
    }
}

void convertToAidl(const ims_CallFailCauseResponse &in,
    imsaidl::CallFailCauseResponse &out) {
  if (in.has_failcause) {
    convertToAidl(in.failcause, out.failCause);
  }

  qcril_binary_data_type *ptr = (qcril_binary_data_type *)(in.errorinfo.arg);
  if (ptr != nullptr && ptr->data != nullptr) {
    std::copy(ptr->data,
      ptr->data + ptr->len,
      std::back_inserter(out.errorInfo));
  }

  const char *networkErrorStringPtr = (const char *)(in.networkErrorString.arg);
  if (networkErrorStringPtr != nullptr) {
    out.networkErrorString = networkErrorStringPtr;
  }

  if (in.has_errorDetails) {
    convertToAidl(in.errorDetails, out.errorDetails);
  }
}

void convertToAidl(const ims_Hangup& in,
    imsaidl::HangupRequestInfo& out) {
  out.connIndex = INT32_MAX;
  if (in.has_conn_index) {
    out.connIndex = in.conn_index;
  }
  out.multiParty = in.multi_party;

  if (in.conn_uri.arg) {
    out.connUri = (char *)in.conn_uri.arg;
  }
  out.conf_id = INT32_MAX;
  if (in.has_conf_id) {
    out.conf_id = in.conf_id;
  }
  if (in.has_failCauseResponse) {
    convertToAidl(in.failCauseResponse, out.failCauseResponse);
  }
}

void convertFromAidl(const imsaidl::ServiceStatusInfo& in,
        ims_Info& out) {
    ims_Error ret = ims_Error_E_SUCCESS;
    do {
        out.has_isValid = true;
        out.isValid = in.isValid;

        out.has_callType = true;
        convertFromAidl(in.callType, out.callType);

        if (in.status != imsaidl::StatusType::INVALID) {
            out.has_status = true;
            convertFromAidl(in.status, out.status);
        }
        if (in.restrictCause != INT32_MAX) {
            out.has_restrictCause = true;
            out.restrictCause = in.restrictCause;
        }
        const uint32_t accTechStatusLen = in.accTechStatus.size();
        if (accTechStatusLen > 0) {
            out.accTechStatus.arg = qcril_malloc(sizeof(ims_StatusForAccessTech*) *
                    (accTechStatusLen + 1));

            if (out.accTechStatus.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for access tech status.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            for(uint32_t i = 0; i < accTechStatusLen; ++i) {
                ((ims_StatusForAccessTech**)out.accTechStatus.arg)[i] =
                        (ims_StatusForAccessTech*)qcril_malloc(sizeof(ims_StatusForAccessTech));

                if (((ims_StatusForAccessTech**)(out.accTechStatus.arg))[i] == nullptr) {
                    QCRIL_LOG_ERROR("Failed to allocate memory for status access tech status.");
                    ret = ims_Error_E_GENERIC_FAILURE;
                    break;
                }

                ims_StatusForAccessTech* ptr =
                        ((ims_StatusForAccessTech**)out.accTechStatus.arg)[i];
                convertFromAidl(in.accTechStatus[i], *ptr);
            }

            if(in.rttMode != imsaidl::RttMode::INVALID){
                out.has_rttMode = true;
                convertFromAidl(in.rttMode, out.rttMode);
            }
        }
    } while(0);
    if (isError(ret)) {
        if (out.userdata.arg != nullptr) {
            qcril_binary_data_type* ptr = (qcril_binary_data_type*)out.userdata.arg;

            qcril_free(ptr->data);
            ptr->data = nullptr;

            qcril_free(ptr);
            ptr = nullptr;
        }

        if (out.accTechStatus.arg != nullptr) {
            imsaidl::StatusForAccessTech** ptr = (imsaidl::StatusForAccessTech**)out.accTechStatus.arg;

            while(*ptr) {
                qcril_free(*ptr);
                *ptr = nullptr;
                ptr++;
            }

            qcril_free(out.accTechStatus.arg);
            out.accTechStatus.arg = nullptr;
        }
    }
}

void convertFromAidl(
    const const std::vector<imsaidl::ServiceStatusInfo>& in,
    ims_SrvStatusList& out) {
  int len = in.size();
  RIL_UTF_DEBUG("\n in.size() = %d", len);

  if (len > 0) {
    ims_Info **dptr = (ims_Info **)qcril_malloc(sizeof(ims_Info *) * (len + 1));
    out.SrvStatusInfo.arg = dptr;
    for (int i = 0; i < len; i++) {
      ims_Info *ptr = (ims_Info *)qcril_malloc(sizeof(ims_Info));
      convertFromAidl(in[i], *ptr);
      dptr[i] = ptr;
    }
  }
}

void convertToAidl(const ims_CallFwdTimerInfo& in,
        imsaidl::CallFwdTimerInfo& out) {
    out.year = (in.has_year) ? in.year : INT32_MAX;
    out.month = (in.has_month) ? in.month :
            INT32_MAX;
    out.day = (in.has_day) ? in.day : INT32_MAX;
    out.hour = (in.has_hour) ? in.hour : INT32_MAX;
    out.minute = (in.has_minute) ? in.minute :
            INT32_MAX;
    out.second = (in.has_second) ? in.second :
            INT32_MAX;
    out.timezone = (in.has_timezone) ?
            in.timezone : INT32_MAX;
}

void convertFromAidl(const imsaidl::CallFwdTimerInfo& in,
         ims_CallFwdTimerInfo& out) {
    if (in.year != INT32_MAX) {
         out.has_year = true;
         out.year = in.year;
    }

    if (in.month != INT32_MAX) {
         out.has_month = true;
         out.month = in.month;
    }

    if (in.day != INT32_MAX) {
         out.has_day = true;
         out.day = in.day;
    }

    if (in.hour != INT32_MAX) {
         out.has_hour = true;
         out.hour = in.hour;
    }

    if (in.minute != INT32_MAX) {
         out.has_minute = true;
         out.minute = in.minute;
    }

    if (in.second != INT32_MAX) {
         out.has_second = true;
         out.second = in.second;
    }

    if (in.timezone != INT32_MAX) {
         out.has_timezone = true;
         out.timezone = in.timezone;
    }
}

bool hasValidCallFwdTimer(const imsaidl::CallFwdTimerInfo& in) {
    if(in.year == INT32_MAX || in.year == 0 ||
       in.month == INT32_MAX || in.month == 0 ||
       in.day == INT32_MAX || in.day == 0 ||
       in.hour == INT32_MAX || in.hour == 0 ||
       in.minute == INT32_MAX || in.hour == 0 ||
       in.second == INT32_MAX || in.second == 0 ||
       in.timezone == INT32_MAX || in.timezone == 0) {
       return false;
    }
    return true;
}

void convertFromAidl(const imsaidl::CallForwardInfo& in,
        ims_CallForwardInfoList_CallForwardInfo& out) {

    out.number.arg = (void*)in.number.c_str();

    if (in.status != INT32_MAX) {
        out.has_status = true;
        out.status = in.status;
    }

    if (in.reason != INT32_MAX) {
        out.has_reason = true;
        out.reason = in.reason;
    }

    if (in.serviceClass != INT32_MAX) {
        out.has_service_class = true;
        out.service_class = in.serviceClass;
    }

    if (in.toa != INT32_MAX) {
        out.has_toa = true;
        out.toa = in.toa;
    }

    if (in.timeSeconds != INT32_MAX) {
        out.has_time_seconds = true;
        out.time_seconds = in.timeSeconds;
    }

    if(hasValidCallFwdTimer(in.callFwdTimerStart)) {
        out.has_callFwdTimerStart = true;
        convertFromAidl(in.callFwdTimerStart,
            out.callFwdTimerStart);
    }

    if(hasValidCallFwdTimer(in.callFwdTimerEnd)) {
        out.has_callFwdTimerEnd = true;
        convertFromAidl(in.callFwdTimerEnd,
            out.callFwdTimerEnd);
    }
}

void convertToAidl(
        const ims_CallForwardInfoList_CallForwardInfo& in,
        imsaidl::CallForwardInfo& out) {
    out.status = (in.has_status) ? in.status :
            INT32_MAX;
    out.reason = (in.has_reason) ? in.reason :
            INT32_MAX;
    out.serviceClass = (in.has_service_class) ?
            in.service_class : INT32_MAX;
    out.toa = (in.has_toa) ? in.toa : INT32_MAX;
    if(in.number.arg != nullptr) {
        out.number = (const char*)in.number.arg;
    }
    out.timeSeconds = (in.has_time_seconds) ?
            in.time_seconds : INT32_MAX;
    out.callFwdTimerStart = {.year = INT32_MAX,
                             .month = INT32_MAX,
                             .day = INT32_MAX,
                             .hour = INT32_MAX,
                             .minute = INT32_MAX,
                             .second = INT32_MAX,
                             .timezone = INT32_MAX};
    if (in.has_callFwdTimerStart) {
        convertToAidl(in.callFwdTimerStart,
          out.callFwdTimerStart);
    }

    out.callFwdTimerEnd = {.year = INT32_MAX,
                           .month = INT32_MAX,
                           .day = INT32_MAX,
                           .hour = INT32_MAX,
                           .minute = INT32_MAX,
                           .second = INT32_MAX,
                           .timezone = INT32_MAX};
    if (in.has_callFwdTimerEnd) {
        convertToAidl(in.callFwdTimerEnd,
          out.callFwdTimerEnd);
    }
    out.expectMore = in.expectMore;
}

void convertToAidl(
    const ims_CallForwardInfoList_CallForwardInfo** in,
    std::vector<imsaidl::CallForwardInfo>& out) {
  if (in== NULL) {
    return;
  }

  uint32_t count = getCount(in);
  out.resize(count);
  for (uint32_t i = 0; i < count; ++i) {
    out[i] = {};
    convertToAidl(*(in[i]), out[i]);
  }
}

void convertToAidl(const ims_DeflectCall& in,
    imsaidl::DeflectRequestInfo& out) {
  if (in.has_conn_index) {
    out.connIndex = in.conn_index;
  }
  if (in.number.arg) {
    out.number = (char *)in.number.arg;
  }
}

void convertToAidl(const ims_ExplicitCallTransfer& in,
    imsaidl::ExplicitCallTransferInfo& out) {
  do {
    if (in.has_callId) {
      out.callId = in.callId;
    }
    out.ectType = imsaidl::EctType::INVALID;
    if (in.has_ectType) {
      if (in.ectType == ims_EctType_BLIND_TRANSFER) {
        out.ectType = imsaidl::EctType::BLIND_TRANSFER;
      } else if (in.ectType == ims_EctType_ASSURED_TRANSFER) {
        out.ectType = imsaidl::EctType::ASSURED_TRANSFER;
      } else if (in.ectType == ims_EctType_CONSULTATIVE_TRANSFER) {
        out.ectType = imsaidl::EctType::CONSULTATIVE_TRANSFER;
      }
    }
    if (in.targetAddress.arg) {
      out.targetAddress = (char *)in.targetAddress.arg;
    }
    out.targetCallId = INT32_MAX;
    if (in.has_targetCallId) {
      out.targetCallId = in.targetCallId;
    }
  } while (0);
}

void convertToAidl(const ims_GeoLocationInfo& in,
    imsaidl::GeoLocationInfo& out) {
  out.lat = in.lat;
  out.lon = in.lon;
  if (in.addressInfo.city.arg) {
    out.addressInfo.city = (char *)in.addressInfo.city.arg;
  }
  if (in.addressInfo.state.arg) {
    out.addressInfo.state = (char *)in.addressInfo.state.arg;
  }
  if (in.addressInfo.country.arg) {
    out.addressInfo.country = (char *)in.addressInfo.country.arg;
  }
  if (in.addressInfo.postalCode.arg) {
    out.addressInfo.postalCode = (char *)in.addressInfo.postalCode.arg;
  }
  if (in.addressInfo.countryCode.arg) {
    out.addressInfo.countryCode = (char *)in.addressInfo.countryCode.arg;
  }
  if (in.addressInfo.street.arg) {
    out.addressInfo.street = (char *)in.addressInfo.street.arg;
  }
  if (in.addressInfo.houseNumber.arg) {
    out.addressInfo.houseNumber = (char *)in.addressInfo.houseNumber.arg;
  }
}

void convertToAidl(const ims_Clir& in,
    imsaidl::ClirInfo& out) {
    out.paramM = (in.has_param_m) ?
        in.param_m : INT32_MAX;
    out.paramN = (in.has_param_n) ?
        in.param_n : INT32_MAX;
}

void convertFromAidl(const imsaidl::ClirInfo &in,
    ims_Clir &out) {
  out.has_param_m = FALSE;
  if (in.paramM != INT32_MAX) {
    out.has_param_m = TRUE;
    out.param_m = in.paramM;
  }

  out.has_param_n = FALSE;
  if (in.paramN != INT32_MAX) {
    out.has_param_n = TRUE;
    out.param_n = in.paramN;
  }
}

void convertFromAidl(const imsaidl::ServiceClassStatus& in,
    ims_ServiceClassStatus& out) {
    switch (in) {
        case imsaidl::ServiceClassStatus::ENABLED:
            out = ims_ServiceClassStatus_ENABLED;
            break;
        case imsaidl::ServiceClassStatus::DISABLED:
        default:
            out = ims_ServiceClassStatus_DISABLED;
            break;
    }
}

void convertToAidl(const ims_ServiceClassStatus& in,
    imsaidl::ServiceClassStatus& out) {
    switch (in) {
        case ims_ServiceClassStatus_ENABLED:
            out = imsaidl::ServiceClassStatus::ENABLED;
            break;
        case ims_ServiceClassStatus_DISABLED:
            out = imsaidl::ServiceClassStatus::DISABLED;
            break;
        default:
            out = imsaidl::ServiceClassStatus::INVALID;
            break;
    }
}

void convertFromAidl(const imsaidl::ServiceClassProvisionStatus& in,
        ims_ServiceClassProvisionStatus& out) {
    switch (in) {
        case imsaidl::ServiceClassProvisionStatus::PROVISIONED:
            out = ims_ServiceClassProvisionStatus_PROVISIONED;
            break;
        case imsaidl::ServiceClassProvisionStatus::NOT_PROVISIONED:
        default:
            out = ims_ServiceClassProvisionStatus_NOT_PROVISIONED;
            break;
    }
}

void convertToAidl(const ims_ServiceClassProvisionStatus& in,
    imsaidl::ServiceClassProvisionStatus& out) {
    switch (in) {
        case ims_ServiceClassProvisionStatus_NOT_PROVISIONED:
            out = imsaidl::ServiceClassProvisionStatus::NOT_PROVISIONED;
            break;
        case ims_ServiceClassProvisionStatus_PROVISIONED:
            out = imsaidl::ServiceClassProvisionStatus::PROVISIONED;
            break;
        default:
            out = imsaidl::ServiceClassProvisionStatus::INVALID;
            break;
    }
}

void convertFromAidl(const imsaidl::ColrInfo& in,
      ims_Colr& out) {
    if (in.status != imsaidl::ServiceClassStatus::INVALID) {
      out.has_status = TRUE;
      convertFromAidl(in.status, out.status);
    }
    if (in.provisionStatus !=
        imsaidl::ServiceClassProvisionStatus::INVALID) {
      out.has_provisionStatus = TRUE;
      convertFromAidl(in.provisionStatus, out.provisionStatus);
    }
    if (in.presentation != imsaidl::IpPresentation::INVALID) {
      out.has_presentation = TRUE;
      convertFromAidl(in.presentation, out.presentation);
    }
}

void convertToAidl(const ims_Colr& in,
    imsaidl::ColrInfo& out) {
    out.status = imsaidl::ServiceClassStatus::INVALID;
    if(in.has_status) {
      convertToAidl(in.status, out.status);
    }
    out.provisionStatus =
      imsaidl::ServiceClassProvisionStatus::INVALID;
    if(in.has_provisionStatus) {
      convertToAidl(in.provisionStatus, out.provisionStatus);
    }
    out.presentation = imsaidl::IpPresentation::INVALID;
    if(in.has_presentation){
      convertToAidl(in.presentation, out.presentation);
    }
    if(in.has_errorDetails) {
      convertToAidl(in.errorDetails, out.errorDetails);
    }
}

void convertFromAidl(const imsaidl::FacilityType& in,
     uint32_t& out) {
     switch (in) {
        case imsaidl::FacilityType::CLIP:
            out = ims_SuppSvcFacilityType_FACILITY_CLIP;
            break;
        case imsaidl::FacilityType::COLP:
            out = ims_SuppSvcFacilityType_FACILITY_COLP;
            break;
        case imsaidl::FacilityType::BAOC:
            out = ims_SuppSvcFacilityType_FACILITY_BAOC;
            break;
        case imsaidl::FacilityType::BAOIC:
            out = ims_SuppSvcFacilityType_FACILITY_BAOIC;
            break;
        case imsaidl::FacilityType::BAOICxH:
            out = ims_SuppSvcFacilityType_FACILITY_BAOICxH;
            break;
        case imsaidl::FacilityType::BAIC:
            out = ims_SuppSvcFacilityType_FACILITY_BAIC;
            break;
        case imsaidl::FacilityType::BAICr:
            out = ims_SuppSvcFacilityType_FACILITY_BAICr;
            break;
        case imsaidl::FacilityType::BA_ALL:
            out = ims_SuppSvcFacilityType_FACILITY_BA_ALL;
            break;
        case imsaidl::FacilityType::BA_MO:
            out = ims_SuppSvcFacilityType_FACILITY_BA_MO;
            break;
        case imsaidl::FacilityType::BA_MT:
            out = ims_SuppSvcFacilityType_FACILITY_BA_MT;
            break;
        case imsaidl::FacilityType::BS_MT:
            out = ims_SuppSvcFacilityType_FACILITY_BS_MT;
            break;
        case imsaidl::FacilityType::BAICa:
            out = ims_SuppSvcFacilityType_FACILITY_BAICa;
            break;
        default:
            out = INT32_MAX;
            break;
    }
}

void convertToAidl(uint32_t in,
    imsaidl::FacilityType& out) {
    switch (in) {
      case ims_SuppSvcFacilityType_FACILITY_CLIP:
        out = imsaidl::FacilityType::CLIP;
        break;
      case ims_SuppSvcFacilityType_FACILITY_COLP:
        out = imsaidl::FacilityType::COLP;
        break;
      case ims_SuppSvcFacilityType_FACILITY_BAOC:
        out = imsaidl::FacilityType::BAOC;
        break;
      case ims_SuppSvcFacilityType_FACILITY_BAOIC:
        out = imsaidl::FacilityType::BAOIC;
        break;
      case ims_SuppSvcFacilityType_FACILITY_BAOICxH:
        out = imsaidl::FacilityType::BAOICxH;
        break;
      case ims_SuppSvcFacilityType_FACILITY_BAIC:
        out = imsaidl::FacilityType::BAIC;
        break;
      case ims_SuppSvcFacilityType_FACILITY_BAICr:
        out = imsaidl::FacilityType::BAICr;
        break;
      case ims_SuppSvcFacilityType_FACILITY_BA_ALL:
        out = imsaidl::FacilityType::BA_ALL;
        break;
      case ims_SuppSvcFacilityType_FACILITY_BA_MO:
        out = imsaidl::FacilityType::BA_MO;
        break;
      case ims_SuppSvcFacilityType_FACILITY_BA_MT:
        out = imsaidl::FacilityType::BA_MT;
        break;
      case ims_SuppSvcFacilityType_FACILITY_BS_MT:
        out = imsaidl::FacilityType::BS_MT;
        break;
      case ims_SuppSvcFacilityType_FACILITY_BAICa:
        out = imsaidl::FacilityType::BAICa;
      break;
      default:
        out = imsaidl::FacilityType::INVALID;
        break;
  }
}

void convertToAidl(const ims_CbNumList& in, imsaidl::CbNumInfo& out) {
    out.status = imsaidl::ServiceClassStatus::INVALID;
    if(in.has_status == true) {
        convertToAidl(in.status, out.status);
    }
    if (in.number.arg != nullptr) {
        out.number = (char *) in.number.arg;
    }
}

void convertFromAidl(const imsaidl::CbNumInfo& in, ims_CbNumList& out) {
    if (in.status != imsaidl::ServiceClassStatus::INVALID) {
        out.has_status = true;
        convertFromAidl(in.status, out.status);
    }
    size_t destSize = sizeof(char) * (in.number.size() + 1);
    out.number.arg = (char*)qcril_malloc(destSize);
    if(out.number.arg != nullptr) {
        strlcpy((char *)out.number.arg, in.number.c_str(), destSize);
    }
}

void convertFromAidl(const imsaidl::CbNumListInfo& inCbNumListInfo,
        ims_CbNumListType& outCbNumListInfo) {
    if (inCbNumListInfo.serviceClass != INT32_MAX) {
        outCbNumListInfo.has_serviceClass = true;
        outCbNumListInfo.serviceClass.has_service_Class = true;
        outCbNumListInfo.serviceClass.service_Class = inCbNumListInfo.serviceClass;
    }

    ims_Error ret = ims_Error_E_SUCCESS;
    uint32_t inListLen = inCbNumListInfo.cbNumInfo.size();
    ims_CbNumList** outList = nullptr;
    outCbNumListInfo.cb_num_list.arg = nullptr;
    do {
        if (inListLen > 0) {
            outList = (ims_CbNumList**)qcril_malloc((inListLen + 1) * sizeof(ims_CbNumList*));

            if (outList == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for service status info list.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            for(uint32_t i = 0; i < inListLen; ++i) {
                outList[i] = (ims_CbNumList*)qcril_malloc(sizeof(ims_CbNumList));

                if (outList[i] == nullptr) {
                    QCRIL_LOG_ERROR("Failed to allocate memory for service status info.");
                    ret = ims_Error_E_GENERIC_FAILURE;
                    break;
                }
                convertFromAidl(inCbNumListInfo.cbNumInfo[i], *(outList[i]));
            }
        }
        outCbNumListInfo.cb_num_list.arg = (void *)outList;
    } while(0);

    if (isError(ret)) {
        if (outList != nullptr) {
            while(*outList) {
                qcril_free(*outList);
                *outList = nullptr;
                outList++;
            }
            qcril_free(outList);
            outList = nullptr;
        }
    }
}

void convertToAidl(const ims_CbNumListType& in,
    imsaidl::CbNumListInfo& out) {
    out.serviceClass = (in.has_serviceClass
        && in.serviceClass.has_service_Class) ?
            in.serviceClass.service_Class : INT32_MAX;
    const ims_CbNumList** ptr = (const ims_CbNumList**) in.cb_num_list.arg;
    if (ptr != nullptr) {
        uint32_t size = getCount(ptr);
        out.cbNumInfo.resize(size);
        for (uint32_t i = 0; i < size; i++) {
            out.cbNumInfo[i] = {};
            convertToAidl(*(ptr[i]), out.cbNumInfo[i]);
        }
    }
}

void convertToAidl(const ims_SuppSvcRequest& in,
    imsaidl::SuppServiceStatusRequest& out) {
    if (in.has_operationType) {
      out.operationType =
        static_cast<imsaidl::SuppSvcOperationType>(in.operationType);
    }
    if (in.has_facilityType) {
      convertToAidl(in.facilityType, out.facilityType);
    }
    imsaidl::CbNumListInfo cbNumListInfo = {};
    if (in.has_cbNumListType) {
      convertToAidl(in.cbNumListType, out.cbNumListInfo);
    }
    out.password = in.password;
    out.expectMore = in.expectMore;
}

void convertToAidl(const ims_Tty_Mode_Type& in,
    imsaidl::TtyMode& out) {
    switch (in) {
        case ims_Tty_Mode_Type_TTY_MODE_FULL:
            out = imsaidl::TtyMode::FULL;
            break;
        case ims_Tty_Mode_Type_TTY_MODE_HCO:
            out = imsaidl::TtyMode::HCO;
            break;
        case ims_Tty_Mode_Type_TTY_MODE_VCO:
            out = imsaidl::TtyMode::VCO;
            break;
        case ims_Tty_Mode_Type_TTY_MODE_OFF:
            out = imsaidl::TtyMode::OFF;
            break;
        default:
            out = imsaidl::TtyMode::INVALID;
            break;
    }
}

void convertToAidl(const ims_ConfigMsg& in,
      imsaidl::ConfigInfo& out) {
    out.item = imsaidl::ConfigItem::INVALID;
    if(in.has_item) {
      convertToAidl(in.item, out.item);
    }
    out.hasBoolValue = in.has_boolValue;
    if (in.has_boolValue) {
        out.boolValue = in.boolValue;
    }
    out.intValue = (in.has_intValue) ? in.intValue : INT32_MAX;
    if(in.stringValue.arg != nullptr) {
        out.stringValue = (const char*)in.stringValue.arg;
    }

    out.errorCause = imsaidl::ConfigFailureCause::INVALID;
    if(in.has_errorCause) {
      convertToAidl(in.errorCause, out.errorCause);
    }
}

void convertFromAidl(const imsaidl::ConfigInfo& in,
      ims_ConfigMsg& out) {
    out.has_item = false;
    convertFromAidl(in.item, out.item);
    if(out.item != ims_ConfigItem_CONFIG_ITEM_NONE) {
      out.has_item = true;
    }

    out.has_boolValue = in.hasBoolValue;
    if (in.hasBoolValue) {
        out.boolValue = in.boolValue;
    }

    out.has_intValue = false;
    if (in.intValue != INT32_MAX) {
        out.has_intValue = true;
        out.intValue = in.intValue;
    }

    size_t destSize = sizeof(char) * (in.stringValue.size() + 1);
    out.stringValue.arg = (char*)qcril_malloc(destSize);
    if(out.stringValue.arg != nullptr) {
        strlcpy((char *)out.stringValue.arg, in.stringValue.c_str(), destSize);
    }

    out.has_errorCause = false;
    if(in.errorCause != imsaidl::ConfigFailureCause::INVALID) {
        out.has_errorCause = true;
        convertFromAidl(in.errorCause, out.errorCause);
    }
}

void convertToAidl(const ims_CallModify &in,
    imsaidl::CallModifyInfo &out)
{
  if (in.has_callIndex) {
    out.callIndex = in.callIndex;
  }

  if (in.has_callDetails) {
    convertToAidl(in.callDetails, out.callDetails);
  }

  if (in.has_error) {
    convertToAidl(in.error, out.failCause);
  } else {
    out.failCause = imsaidl::CallModifyFailCause::E_INVALID;
  }
}

void convertFromAidl(const std::vector<imsaidl::MultiIdentityLineInfo> &in,
                                  ims_MultiIdentityStatus &out) {
  out.lineInfo_len = in.size();
  if (in.size()) {
    for (uint32_t i = 0; i < in.size() && i < 10; i++) {
      strlcpy(out.lineInfo[i].msisdn, in[i].msisdn.c_str(), in[i].msisdn.size()+1);
      out.lineInfo[i].registrationStatus =
          (ims_MultiIdentityRegistrationStatus) (in[i].registrationStatus);
      out.lineInfo[i].lineType = (ims_MultiIdentityLineType) (in[i].lineType);
    }
  }
}

void convertToAidl(const ims_MultiIdentityStatus& in,
  std::vector<imsaidl::MultiIdentityLineInfo>& out) {
  if (in.lineInfo_len) {
    out.resize(in.lineInfo_len);
    for (uint32_t i = 0; i < in.lineInfo_len; i++) {
      out[i].msisdn = in.lineInfo[i].msisdn;
      out[i].registrationStatus =
        (imsaidl::MultiIdentityRegistrationStatus) (in.lineInfo[i].registrationStatus);
      out[i].lineType = (imsaidl::MultiIdentityLineType) (in.lineInfo[i].lineType);
    }
  }
}

void convertFromAidl(const imsaidl::SipErrorInfo& in,
        ims_SipErrorInfo& out) {
    if (in.errorCode != INT32_MAX) {
        out.has_sipErrorCode = TRUE;
        out.sipErrorCode = in.errorCode;
    }
    const uint32_t errorStrLen = in.errorString.size();
    if (errorStrLen > 0) {
        size_t destSize = sizeof(char) * (errorStrLen + 1);
        out.sipErrorString.arg = qcril_malloc(destSize);
        if (out.sipErrorString.arg != nullptr) {
          strlcpy((char*)out.sipErrorString.arg, in.errorString.c_str(), destSize);
        }
    }
}

void convertFromAidl(const imsaidl::SuppServiceStatus &in,
    ims_SuppSvcResponse &out) {
  out.has_status = (in.status != imsaidl::ServiceClassStatus::INVALID);
  convertFromAidl(in.status, out.status);

  out.has_facilityType = (in.facilityType != imsaidl::FacilityType::INVALID);
  convertFromAidl(in.facilityType, out.facilityType);

  if (!in.failureCause.empty()) {
    size_t destSize = sizeof(char) * (in.failureCause.size() + 1);
    out.failureCause.arg = qcril_malloc(destSize);
    if (out.failureCause.arg != nullptr) {
      strlcpy((char *)out.failureCause.arg, in.failureCause.c_str(), destSize);
    }
  }
  int len = in.cbNumListInfo.size();
  RIL_UTF_DEBUG("\n in.cbNumListInfo.size = %d", len);
  if (len > 0) {
    ims_CbNumListType **dptr =
        (ims_CbNumListType **)qcril_malloc(sizeof(ims_CbNumListType *) * (len + 1));
    out.cbNumListType.arg = dptr;
    for (int i = 0; i < len; i++) {
      ims_CbNumListType *ptr = (ims_CbNumListType *)qcril_malloc(sizeof(ims_CbNumListType));
      convertFromAidl(in.cbNumListInfo[i], *ptr);
      dptr[i] = ptr;
    }
  }
  if (in.hasErrorDetails) {
    out.has_errorDetails = true;
    convertFromAidl(in.errorDetails, out.errorDetails);
  }
  out.has_isPasswordRequired = true;
  out.isPasswordRequired = static_cast<bool>(in.isPasswordRequired);
}

void convertFromAidl(const imsaidl::CallForwardStatusInfo& in,
    ims_CallForwardStatusInfo& out) {
  out.has_errorDetails = true;
  convertFromAidl(in.errorDetails, out.errorDetails);

  out.status_len = in.status.size();

  if (out.status_len) {
    for(uint32_t i = 0; i < out.status_len; i++) {
      out.status[i].reason = in.status[i].reason;
      convertFromAidl(in.status[i].status, out.status[i].status);
      convertFromAidl(in.status[i].errorDetails, out.status[i].errorDetails);
    }
  }
}

void convertFromAidl(const imsaidl::ImsSubConfigInfo &in,
    ims_ImsSubConfig &out) {
  out.has_simultStackCount = FALSE;
  if (in.simultStackCount != INT32_MAX) {
    out.has_simultStackCount = TRUE;
    out.simultStackCount = in.simultStackCount;
  }
  out.imsStackEnabled.arg = NULL;
  if (in.imsStackEnabled.size()) {
    size_t len = in.imsStackEnabled.size();
    uint64_t **stackEnabled = (uint64_t **)qcril_malloc(sizeof(uint64_t*) * (len+1));
    for (size_t i = 0; i < len; i++) {
      stackEnabled[i] = (uint64_t *)qcril_malloc(sizeof(uint64_t));
      *(stackEnabled[i]) = in.imsStackEnabled[i];
    }
    out.imsStackEnabled.arg = stackEnabled;
  }
}

void convertFromAidl(const imsaidl::HandoverType &in,
    ims_Handover_Msg_Type& out) {
  switch (in) {
    case imsaidl::HandoverType::START:
      out = ims_Handover_Msg_Type_START;
      break;
    case imsaidl::HandoverType::COMPLETE_SUCCESS:
      out = ims_Handover_Msg_Type_COMPLETE_SUCCESS;
      break;
    case imsaidl::HandoverType::COMPLETE_FAIL:
      out = ims_Handover_Msg_Type_COMPLETE_FAIL;
      break;
    case imsaidl::HandoverType::CANCEL:
      out = ims_Handover_Msg_Type_CANCEL;
      break;
    case imsaidl::HandoverType::NOT_TRIGGERED:
      out = ims_Handover_Msg_Type_NOT_TRIGGERED;
      break;
    case imsaidl::HandoverType::NOT_TRIGGERED_MOBILE_DATA_OFF:
      out = ims_Handover_Msg_Type_NOT_TRIGGERED_MOBILE_DATA_OFF;
      break;
    case imsaidl::HandoverType::INVALID:
    default:
      out = ims_Handover_Msg_Type_START;
      break;
  }
}

void convertFromAidl(const imsaidl::Extra &in,
    ims_Extra &out) {
  out.has_type = (in.type != imsaidl::ExtraType::INVALID);
  if (in.type == imsaidl::ExtraType::LTE_TO_IWLAN_HO_FAIL) {
    out.type = ims_Extra_Type_LTE_TO_IWLAN_HO_FAIL;
  }

  int len = in.extraInfo.size();
  RIL_UTF_DEBUG("\n in.extraInfo.size() = %d", len);

  qcril_binary_data_type *extraPtr =
      (qcril_binary_data_type *)qcril_malloc(sizeof(qcril_binary_data_type));
  extraPtr->len = len;
  extraPtr->data = (uint8_t *)in.extraInfo.data();
  out.extraInfo.arg = extraPtr;
}

void convertFromAidl(const imsaidl::HandoverInfo &in,
    ims_Handover &out) {
  out.has_type = (in.type != imsaidl::HandoverType::INVALID);
  convertFromAidl(in.type, out.type);

  out.has_srcTech = (in.srcTech != imsaidl::RadioTechType::INVALID);
  convertFromAidl(in.srcTech, out.srcTech);

  out.has_targetTech = (in.targetTech != imsaidl::RadioTechType::INVALID);
  convertFromAidl(in.targetTech, out.targetTech);

  convertFromAidl(in.hoExtra, out.hoExtra);

  if (!in.errorCode.empty()) {
    size_t destSize = sizeof(char) * (in.errorCode.size() + 1);
    out.errorCode.arg = qcril_malloc(destSize);
    if (out.errorCode.arg != nullptr) {
      strlcpy((char *)out.errorCode.arg, in.errorCode.c_str(), destSize);
    }
  }

  if (!in.errorMessage.empty()) {
    size_t destSize = sizeof(char) * (in.errorMessage.size() + 1);
    out.errorMessage.arg = qcril_malloc(destSize);
    if (out.errorMessage.arg != nullptr) {
      strlcpy((char *)out.errorMessage.arg, in.errorMessage.c_str(), destSize);
    }
  }
}

void convertFromAidl(const imsaidl::CallState &in,
    ims_CallState& out) {
  switch (in) {
    case imsaidl::CallState::ACTIVE:
      out = ims_CallState_CALL_ACTIVE;
      break;
    case imsaidl::CallState::HOLDING:
      out = ims_CallState_CALL_HOLDING;
      break;
    case imsaidl::CallState::DIALING:
      out = ims_CallState_CALL_DIALING;
      break;
    case imsaidl::CallState::ALERTING:
      out = ims_CallState_CALL_ALERTING;
      break;
    case imsaidl::CallState::INCOMING:
      out = ims_CallState_CALL_INCOMING;
      break;
    case imsaidl::CallState::WAITING:
      out = ims_CallState_CALL_WAITING;
      break;
    case imsaidl::CallState::END:
      out = ims_CallState_CALL_END;
      break;
    case imsaidl::CallState::INVALID:
    default:
      out = ims_CallState_CALL_END;
      break;
  }
}

void convertFromAidl(const imsaidl::MsimAdditionalInfoCode& in, ims_MsimAdditionalInfoCode& out) {
  switch (in) {
    case imsaidl::MsimAdditionalInfoCode::CONCURRENT_CALL_NOT_POSSIBLE:
      out = ims_MsimAdditionalInfoCode_CONCURRENT_CALL_NOT_POSSIBLE;
      break;
    case imsaidl::MsimAdditionalInfoCode::NONE:
    default:
      out = ims_MsimAdditionalInfoCode_NONE;
  }
}

void convertFromAidl(const imsaidl::Codec& in, ims_Codec& out) {
  switch (in) {
    case imsaidl::Codec::QCELP13K:
      out = ims_Codec_QCELP13K;
      break;
    case imsaidl::Codec::EVRC:
      out = ims_Codec_EVRC;
      break;
    case imsaidl::Codec::EVRC_B:
      out = ims_Codec_EVRC_B;
      break;
    case imsaidl::Codec::EVRC_WB:
      out = ims_Codec_EVRC_WB;
      break;
    case imsaidl::Codec::EVRC_NW:
      out = ims_Codec_EVRC_NW;
      break;
    case imsaidl::Codec::AMR_NB:
      out = ims_Codec_AMR_NB;
      break;
    case imsaidl::Codec::AMR_WB:
      out = ims_Codec_AMR_WB;
      break;
    case imsaidl::Codec::GSM_EFR:
      out = ims_Codec_GSM_EFR;
      break;
    case imsaidl::Codec::GSM_FR:
      out = ims_Codec_GSM_FR;
      break;
    case imsaidl::Codec::GSM_HR:
      out = ims_Codec_GSM_HR;
      break;
    case imsaidl::Codec::G711U:
      out = ims_Codec_G711U;
      break;
    case imsaidl::Codec::G723:
      out = ims_Codec_G723;
      break;
    case imsaidl::Codec::G711A:
      out = ims_Codec_G711A;
      break;
    case imsaidl::Codec::G722:
      out = ims_Codec_G722;
      break;
    case imsaidl::Codec::G711AB:
      out = ims_Codec_G711AB;
      break;
    case imsaidl::Codec::G729:
      out = ims_Codec_G729;
      break;
    case imsaidl::Codec::EVS_NB:
      out = ims_Codec_EVS_NB;
      break;
    case imsaidl::Codec::EVS_WB:
      out = ims_Codec_EVS_WB;
      break;
    case imsaidl::Codec::EVS_SWB:
      out = ims_Codec_EVS_SWB;
      break;
    case imsaidl::Codec::EVS_FB:
      out = ims_Codec_EVS_FB;
      break;
    case imsaidl::Codec::INVALID:
    default:
      out = ims_Codec_NONE;
  }
}

void convertFromAidl(const imsaidl::ComputedAudioQuality& in, ims_ComputedAudioQuality& out) {
  switch (in) {
    case imsaidl::ComputedAudioQuality::NO_HD:
      out = ims_ComputedAudioQuality_NO_HD;
      break;
    case imsaidl::ComputedAudioQuality::HD:
      out = ims_ComputedAudioQuality_HD;
      break;
    case imsaidl::ComputedAudioQuality::HD_PLUS:
      out = ims_ComputedAudioQuality_HD_PLUS;
      break;
    case imsaidl::ComputedAudioQuality::INVALID:
    default:
      out = ims_ComputedAudioQuality_NONE;
  }
}

void convertFromAidl(const imsaidl::CallInfo &in,
     ims_CallList_Call &out) {
  out.has_state = (in.state != imsaidl::CallState::INVALID);
  convertFromAidl(in.state, out.state);

  out.has_index = (in.index != INT32_MAX);
  out.index = in.index;

  out.has_toa = (in.toa != INT32_MAX);
  out.toa = in.toa;

  out.has_isMpty = true; //in.isMpty;
  out.isMpty = in.isMpty;

  out.has_isMT = true;//in.isMT;
  out.isMT = in.isMT;

  out.has_als = (in.als != INT32_MAX);
  out.als = in.als;

  out.has_isVoice = in.isVoice;
  out.isVoice = in.isVoice;

  out.has_isVoicePrivacy = true; //in.isVoicePrivacy;
  out.isVoicePrivacy = in.isVoicePrivacy;

  if (!in.number.empty()) {
    size_t destSize = sizeof(char) * (in.number.size() + 1);
    out.number.arg = qcril_malloc(destSize);
    if (out.number.arg != nullptr) {
      strlcpy((char *)out.number.arg, in.number.c_str(), destSize);
    }
  }

  out.has_numberPresentation = (in.numberPresentation != INT32_MAX);
  out.numberPresentation = in.numberPresentation;

  if (!in.name.empty()) {
    size_t destSize = sizeof(char) * (in.name.size() + 1);
    out.name.arg = qcril_malloc(destSize);
    if (out.name.arg != nullptr) {
      strlcpy((char *)out.name.arg, in.name.c_str(), destSize);
    }
  }

  out.has_namePresentation = (in.namePresentation != INT32_MAX);
  out.namePresentation = in.namePresentation;

  out.has_callDetails = true;
  convertFromAidl(in.callDetails, out.callDetails);

  out.has_failCause = false;
  if(in.failCause.failCause != imsaidl::CallFailCause::INVALID) {
    out.has_failCause = true;
    convertFromAidl(in.failCause, out.failCause);
  }

  out.has_isEncrypted = in.isEncrypted;
  out.isEncrypted = in.isEncrypted;

  out.has_isCalledPartyRinging = in.isCalledPartyRinging;
  out.isCalledPartyRinging = in.isCalledPartyRinging;

  if (!in.historyInfo.empty()) {
    size_t destSize = sizeof(char) * (in.historyInfo.size() + 1);
    out.historyInfo.arg = qcril_malloc(destSize);
    if (out.historyInfo.arg != nullptr) {
      strlcpy((char *)out.historyInfo.arg, in.historyInfo.c_str(), destSize);
    }
  }

  out.has_peerConfAbility = in.isVideoConfSupported;
  out.peerConfAbility.has_isVideoConfSupported = in.isVideoConfSupported;
  out.peerConfAbility.isVideoConfSupported = in.isVideoConfSupported;

  if (!in.mtMultiLineInfo.msisdn.empty()) {
    strlcpy(out.terminatingNumber, in.mtMultiLineInfo.msisdn.c_str(), sizeof(out.terminatingNumber));
  }
  if (in.mtMultiLineInfo.lineType == imsaidl::MultiIdentityLineType::SECONDARY) {
    out.isSecondary = TRUE;
  }
  convertFromAidl(in.tirMode, out.tirMode);

  if (in.callProgInfo.type != imsaidl::CallProgressInfoType::INVALID) {
    convertFromAidl(in.callProgInfo.type,out.callProgInfo.type);
    if (in.callProgInfo.reasonCode != INT16_MAX) {
      out.callProgInfo.reasonCode = in.callProgInfo.reasonCode;
    }
    strlcpy(out.callProgInfo.reasonText, in.callProgInfo.reasonText.c_str(),
            sizeof(out.callProgInfo.reasonText));
  }

  if (in.additionalCallInfo &&
      in.additionalCallInfo->additionalCode != imsaidl::MsimAdditionalInfoCode::NONE) {
    convertFromAidl(in.additionalCallInfo->additionalCode, out.additionalCallInfo.additionalCode);
  }

  if (in.audioQuality && in.audioQuality->codec != imsaidl::Codec::INVALID) {
    convertFromAidl(in.audioQuality->codec, out.audioQuality.codec);
  }

  if (in.audioQuality &&
      in.audioQuality->computedAudioQuality != imsaidl::ComputedAudioQuality::INVALID) {
    convertFromAidl(in.audioQuality->computedAudioQuality, out.audioQuality.computedAudioQuality);
  }
}

void convertFromAidl(const std::vector<imsaidl::CallInfo> &in,
    ims_CallList &out) {
  int len = in.size();
  RIL_UTF_DEBUG("\n in.size() = %d", len);
  if (len > 0) {
    ims_CallList_Call **dptr =
        (ims_CallList_Call **)qcril_malloc(sizeof(ims_CallList_Call *) * (len + 1));
    out.callAttributes.arg = dptr;
    for (int i = 0; i < len; i++) {
      ims_CallList_Call *callPtr = (ims_CallList_Call *)qcril_malloc(sizeof(ims_CallList_Call));
      convertFromAidl(in[i], *callPtr);
      dptr[i] = callPtr;
    }
  }
}

void convertFromAidl(const imsaidl::CallModifyInfo &in,
    ims_CallModify &out) {
  ims_Error ret = ims_Error_E_SUCCESS;
  out.has_callIndex = TRUE;
  out.callIndex = in.callIndex;

  convertFromAidl(in.callDetails, out.callDetails);
  out.has_callDetails = true;

  convertFromAidl(in.failCause, out.error);
  if(out.error != ims_Error_E_SUCCESS) {
      out.has_error = true;
  }
}

void convertFromAidl(const imsaidl::SuppServiceNotification &in,
    ims_SuppSvcNotification& out) {
  if (in.notificationType != imsaidl::NotificationType::INVALID) {
    out.has_notificationType = TRUE;
    convertFromAidl(in.notificationType, out.notificationType);
  }
  if (in.code != INT32_MAX) {
    out.has_code = TRUE;
    out.code = in.code;
  }

  if (in.index != INT32_MAX) {
    out.has_index = TRUE;
    out.index = in.index;
  }

  if (in.type != INT32_MAX) {
    out.has_type = TRUE;
    out.type = in.type;
  }

  if (!in.number.empty()) {
    size_t destSize = sizeof(char) * (in.number.size() + 1);
    out.number.arg = qcril_malloc(destSize);
    if (out.number.arg != nullptr) {
      strlcpy((char *)out.number.arg, in.number.c_str(), destSize);
    }
  }

  if (in.connId != INT32_MAX) {
    out.has_connId = TRUE;
    out.connId = in.connId;
  }

  if (!in.historyInfo.empty()) {
    size_t destSize = sizeof(char) * (in.historyInfo.size() + 1);
    out.history_info.arg = qcril_malloc(destSize);
    if (out.history_info.arg != nullptr) {
      strlcpy((char *)out.history_info.arg, in.historyInfo.c_str(), destSize);
    }
  }

  out.has_hold_tone = in.hasHoldTone;
  if (in.hasHoldTone) {
    out.hold_tone = in.holdTone;
  }
}

void convertFromAidl(const imsaidl::ParticipantStatusInfo &in,
    ims_ParticipantStatusInfo &out) {
  if (in.callId != INT32_MAX) {
    out.has_callId = TRUE;
    out.callId = in.callId;
  }
  if (in.operation != imsaidl::ConfParticipantOperation::INVALID) {
    out.has_operation = TRUE;
    convertFromAidl(in.operation, out.operation);
  }
  if (in.sipStatus != INT32_MAX) {
    out.has_sipStatus = TRUE;
    out.sipStatus = in.sipStatus;
  }

  if (!in.participantUri.empty()) {
    size_t destSize = sizeof(char) * (in.participantUri.size() + 1);
    out.participantUri.arg = qcril_malloc(destSize);
    if (out.participantUri.arg != nullptr) {
      strlcpy((char *)out.participantUri.arg, in.participantUri.c_str(), destSize);
    }
  }
  out.has_isEct = in.isEct;
  out.isEct = in.isEct;
}

void convertFromAidl(const imsaidl::BlockStatus &in,
    ims_BlockStatus &out) {
  out.has_blockReason = TRUE;
  switch (in.blockReason) {
    case imsaidl::BlockReasonType::PDP_FAILURE:
      out.blockReason = ims_BlockReasonType_BLOCK_REASON_PDP_FAILURE;
      break;
    case imsaidl::BlockReasonType::REGISTRATION_FAILURE:
      out.blockReason = ims_BlockReasonType_BLOCK_REASON_REGISTRATION_FAILURE;
      break;
    case imsaidl::BlockReasonType::HANDOVER_FAILURE:
      out.blockReason = ims_BlockReasonType_BLOCK_REASON_HANDOVER_FAILURE;
      break;
    case imsaidl::BlockReasonType::OTHER_FAILURE:
      out.blockReason = ims_BlockReasonType_BLOCK_REASON_OTHER_FAILURE;
      break;
    default:
      out.has_blockReason = FALSE;
      break;
  }

  out.has_blockReasonDetails = TRUE;
  out.blockReasonDetails.has_regFailureReasonType = TRUE;
  switch (in.blockReasonDetails.regFailureReasonType) {
    case imsaidl::RegFailureReasonType::UNSPECIFIED:
      out.blockReasonDetails.regFailureReasonType = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_UNSPECIFIED;
      break;
    case imsaidl::RegFailureReasonType::MOBILE_IP:
      out.blockReasonDetails.regFailureReasonType = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_MOBILE_IP;
      break;
    case imsaidl::RegFailureReasonType::INTERNAL:
      out.blockReasonDetails.regFailureReasonType = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_INTERNAL;
      break;
    case imsaidl::RegFailureReasonType::CALL_MANAGER_DEFINED:
      out.blockReasonDetails.regFailureReasonType = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_CALL_MANAGER_DEFINED;
      break;
    case imsaidl::RegFailureReasonType::TYPE_3GPP_SPEC_DEFINED:
      out.blockReasonDetails.regFailureReasonType = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_3GPP_SPEC_DEFINED;
      break;
    case imsaidl::RegFailureReasonType::PPP:
      out.blockReasonDetails.regFailureReasonType = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_PPP;
      break;
    case imsaidl::RegFailureReasonType::EHRPD:
      out.blockReasonDetails.regFailureReasonType = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_EHRPD;
      break;
    case imsaidl::RegFailureReasonType::IPV6:
      out.blockReasonDetails.regFailureReasonType = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_IPV6;
      break;
    case imsaidl::RegFailureReasonType::IWLAN:
      out.blockReasonDetails.regFailureReasonType = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_IWLAN;
      break;
    case imsaidl::RegFailureReasonType::HANDOFF:
      out.blockReasonDetails.regFailureReasonType = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_HANDOFF;
      break;
    default:
      out.blockReasonDetails.has_regFailureReasonType = FALSE;
      break;
  }
  out.blockReasonDetails.has_RegFailureReason = FALSE;
  if (in.blockReasonDetails.regFailureReason != INT32_MAX) {
    out.blockReasonDetails.has_RegFailureReason = TRUE;
    out.blockReasonDetails.RegFailureReason = in.blockReasonDetails.regFailureReason;
  }
}

void convertFromAidl(const imsaidl::RegistrationBlockStatusInfo& in,
    ims_RegistrationBlockStatus& out) {
  out.has_imsBlockStatusOnWwan = true;
  convertFromAidl(in.blockStatusOnWwan, out.imsBlockStatusOnWwan);
  out.has_imsBlockStatusOnWlan = true;
  convertFromAidl(in.blockStatusOnWlan, out.imsBlockStatusOnWlan);
}

void convertFromAidl(const imsaidl::AutoCallRejectionInfo& in,
    ims_AutoCallRejectionInfo& out) {
  convertFromAidl(in.callType, out.callType);
  if (in.autoRejectionCause != imsaidl::CallFailCause::INVALID) {
    convertFromAidl(in.autoRejectionCause, out.autoRejectionCause);
  }
  out.sipErrorCode = in.sipErrorCode;
  if (!in.number.empty()) {
    out.hasNumber = TRUE;
    out.number.arg = strdup(in.number.c_str());
  }
  convertFromAidl(in.verificationStatus, out.verificationStatus);
}

void convertFromAidl(const imsaidl::CallPriority& in,
    ims_CallComposerPriority& out) {
  switch (in) {
  case imsaidl::CallPriority::URGENT:
    out = ims_CallComposerPriority_URGENT;
    break;
  case imsaidl::CallPriority::NORMAL:
  default:
    out = ims_CallComposerPriority_NORMAL;
    break;
  }
}

void convertFromAidl(const imsaidl::CallLocation& in,
    ims_CallComposerLocation& out) {
  out.radius = in.radius;
  out.latitude = in.latitude;
  out.longitude = in.longitude;
}

void convertFromAidl(const imsaidl::CallComposerInfo& in,
    ims_CallComposer& out) {
  out.call_id = in.callId;
  convertFromAidl(in.priority, out.priority);
  out.subject_len = in.subject.size();
  if (out.subject_len > 0) {
    std::copy(in.subject.data(), in.subject.data() + in.subject.size(), out.subject);
  }
  out.organization_len = in.organization->size();
  if (out.organization_len > 0) {
    std::copy(in.organization->data(), in.organization->data() + in.organization->size(), out.organization);
  }
  convertFromAidl(in.location, out.location);
  strlcpy(out.image_url, in.imageUrl.c_str(), IMS_CALL_COMPOSER_PIC_URL_MAX_LEN);
}

void convertToAidl(const int& in,
    imsaidl::SmsDeliverStatus& out) {
  switch(in) {
    case 0:
      out = imsaidl::SmsDeliverStatus::OK;
      break;
    case 1:
      out = imsaidl::SmsDeliverStatus::ERROR;
      break;
    case 2:
      out = imsaidl::SmsDeliverStatus::ERROR_NO_MEMORY;
      break;
    case 3:
      out = imsaidl::SmsDeliverStatus::ERROR_REQUEST_NOT_SUPPORTED;
      break;
    default:
      out = imsaidl::SmsDeliverStatus::ERROR;
      break;
  }
}

void convertFromAidl(const imsaidl::MultiSimVoiceCapability& in, ims_MultiSimVoiceCapability& out) {
  switch (in) {
    case imsaidl::MultiSimVoiceCapability::NONE:
      out.voiceCapability = ims_Voice_Capability_NONE;
      break;
    case imsaidl::MultiSimVoiceCapability::DSSS:
      out.voiceCapability = ims_Voice_Capability_DSSS;
      break;
    case imsaidl::MultiSimVoiceCapability::DSDS:
      out.voiceCapability = ims_Voice_Capability_DSDS;
      break;
    case imsaidl::MultiSimVoiceCapability::PSEUDO_DSDA:
      out.voiceCapability = ims_Voice_Capability_PSEUDO_DSDA;
      break;
    case imsaidl::MultiSimVoiceCapability::DSDA:
      out.voiceCapability = ims_Voice_Capability_DSDA;
      break;
    default:
      out.voiceCapability = ims_Voice_Capability_UNKNOWN;
      break;
  }
}

void convertFromAidl(const imsaidl::EcnamInfo& in, ims_EcnamInfo& out) {
  strlcpy(out.name, in.name.c_str(), IMS_CALL_ECNAM_NAME_MAX_LEN);
  strlcpy(out.icon_url, in.iconUrl.c_str(), IMS_CALL_ECNAM_ICON_URL_MAX_LEN);
  strlcpy(out.info_url, in.infoUrl.c_str(), IMS_CALL_ECNAM_INFO_URL_MAX_LEN);
  strlcpy(out.card_url, in.cardUrl.c_str(), IMS_CALL_ECNAM_CARD_URL_MAX_LEN);
}

void convertFromAidl(const imsaidl::PreAlertingCallInfo& in, ims_PreAlertingCallInfo& out) {
  out.call_id = in.callId;
  if (in.callComposerInfo) {
    convertFromAidl(*in.callComposerInfo, out.callComposerInfo);
  }
  if (in.ecnamInfo) {
    convertFromAidl(*in.ecnamInfo, out.ecnamInfo);
  }
  out.modemCallId = in.modemCallId;
  out.isDcCall = in.isDcCall;
}

void convertFromAidl(const imsaidl::CiWlanNotificationInfo& in, ims_CiWlanNotificationInfo& out) {
  switch (in) {
    case imsaidl::CiWlanNotificationInfo::DISABLE_CIWLAN:
      out.notificationInfo = ims_CIWlan_Notification_DISABLE_CIWLAN;
      break;
    case imsaidl::CiWlanNotificationInfo::NONE:
    default:
      out.notificationInfo = ims_CIWlan_Notification_NONE;
      break;
  }

}

imsaidl::VosActionInfo convertToAidl(const ims_VosActionInfo &in) {
  imsaidl::VosActionInfo vosActionInfo {};
  if (in.has_vosMoveInfo) {
    vosActionInfo.vosMoveInfo = convertToAidl(in.vosMoveInfo);
  }

  if (in.has_vosTouchInfo) {
    vosActionInfo.vosTouchInfo = convertToAidl(in.vosTouchInfo);
  }
  return vosActionInfo;
}

imsaidl::VosMoveInfo convertToAidl(const ims_VosMoveInfo &in) {
  imsaidl::VosMoveInfo vosMoveInfo {};
  if (in.has_start) {
    vosMoveInfo.start = convertToAidl(in.start);
  }

  if (in.has_end) {
    vosMoveInfo.end = convertToAidl(in.end);
  }
  return vosMoveInfo;
}

imsaidl::VosTouchInfo convertToAidl(const ims_VosTouchInfo &in) {
  imsaidl::VosTouchInfo vosTouchInfo {};
  if (in.has_touch) {
    vosTouchInfo.touch = convertToAidl(in.touch);
  }
  vosTouchInfo.touchDuration = in.touchDuration;
  return vosTouchInfo;
}

imsaidl::Coordinate2D convertToAidl(const ims_Coordinate2D &in) {
  imsaidl::Coordinate2D coordinate2D {};
  coordinate2D.x = in.x;
  coordinate2D.y = in.y;
  return coordinate2D;
}

}  // namespace utils
}  // namespace aidl
}  // namespace ims
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
