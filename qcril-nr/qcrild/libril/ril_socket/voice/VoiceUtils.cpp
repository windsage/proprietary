/******************************************************************************
#  Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <interfaces/voice/voice.h>
#include <telephony/ril_call.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {
namespace voice {

qcril::interfaces::RttMode convertRttMode(const RIL_RttMode in) {
  switch (in) {
  case RIL_RTT_FULL:
    return qcril::interfaces::RttMode::FULL;
  case RIL_RTT_DISABLED:
  default:
    return qcril::interfaces::RttMode::DISABLED;
  }
}

qcril::interfaces::CallType convertCallType(const RIL_CallType in) {
  switch (in) {
    case RIL_CALL_TYPE_VOICE:
      return qcril::interfaces::CallType::VOICE;
    case RIL_CALL_TYPE_VT_TX:
      return qcril::interfaces::CallType::VT_TX;
    case RIL_CALL_TYPE_VT_RX:
      return qcril::interfaces::CallType::VT_RX;
    case RIL_CALL_TYPE_VT:
      return qcril::interfaces::CallType::VT;
    case RIL_CALL_TYPE_VT_NODIR:
      return qcril::interfaces::CallType::VT_NODIR;
    case RIL_CALL_TYPE_SMS:
      return qcril::interfaces::CallType::SMS;
    case RIL_CALL_TYPE_UT:
      return  qcril::interfaces::CallType::UT;
    case RIL_CALL_TYPE_USSD:
      return  qcril::interfaces::CallType::USSD;
    case RIL_CALL_TYPE_CALLCOMPOSER:
      return  qcril::interfaces::CallType::CALLCOMPOSER;
    case RIL_CALL_TYPE_DC:
      return  qcril::interfaces::CallType::DC;
    case RIL_CALL_TYPE_UNKNOWN:
    default:
      return qcril::interfaces::CallType::UNKNOWN;
  }
}

qcril::interfaces::CallDomain convertCallDomain(const RIL_CallDomain in) {
  switch (in) {
  case RIL_CALLDOMAIN_CS:
    return qcril::interfaces::CallDomain::CS;
  case RIL_CALLDOMAIN_PS:
    return qcril::interfaces::CallDomain::PS;
  case RIL_CALLDOMAIN_AUTOMATIC:
    return qcril::interfaces::CallDomain::AUTOMATIC;
  case RIL_CALLDOMAIN_UNKNOWN:
  default:
    return qcril::interfaces::CallDomain::UNKNOWN;
  }
}

qcril::interfaces::Presentation convertIpPresentation(const RIL_IpPresentation in) {
  switch (in) {
  case RIL_IP_PRESENTATION_NUM_ALLOWED:
    return qcril::interfaces::Presentation::ALLOWED;
  case RIL_IP_PRESENTATION_NUM_RESTRICTED:
    return qcril::interfaces::Presentation::RESTRICTED;
  case RIL_IP_PRESENTATION_NUM_DEFAULT:
    return qcril::interfaces::Presentation::DEFAULT;
  case RIL_IP_PRESENTATION_INVALID:
  default:
    return qcril::interfaces::Presentation::UNKNOWN;
  }
}

qcril::interfaces::TtyMode convertTtyMode(const RIL_TtyModeType &in) {
  switch (in) {
    case RIL_TTY_MODE_OFF:
      return qcril::interfaces::TtyMode::MODE_OFF;
    case RIL_TTY_MODE_FULL:
      return qcril::interfaces::TtyMode::FULL;
    case RIL_TTY_MODE_HCO:
      return qcril::interfaces::TtyMode::HCO;
    case RIL_TTY_MODE_VCO:
      return qcril::interfaces::TtyMode::VCO;
    case RIL_TTY_MODE_INVALID:
    default:
      return qcril::interfaces::TtyMode::UNKNOWN;
  }
}

}  // namespace voice
}  // namespace api
}  // namespace socket
}  // namespace ril
