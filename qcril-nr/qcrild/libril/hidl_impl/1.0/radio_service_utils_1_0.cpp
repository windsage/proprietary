/*
 * Copyright (c) 2018,2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */
/*
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hidl_impl/1.0/radio_service_utils_1_0.h"
#include "ril_utils.h"
#include "qcril_legacy_apis.h"

#define BOOL_TO_INT(x) (x ? 1 : 0)

#define QCRIL_USSD_IMS_RUNTIME_SUPPORT "vendor.radio.ussd_support_at_runtime"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

hidl_string convertCharPtrToHidlString(const char* ptr) {
  hidl_string ret;
  if (ptr != NULL) {
    // TODO: replace this with strnlen
    ret.setToExternal(ptr, strlen(ptr));
  }
  return ret;
}

RIL_RadioTechnologyFamily convertHidlRadioTechToRil(const V1_0::RadioTechnologyFamily tech) {
    return tech == V1_0::RadioTechnologyFamily::THREE_GPP
        ? RADIO_TECH_3GPP
        : RADIO_TECH_3GPP2;
}

V1_0::RadioError convertMsgToRadioError(Message::Callback::Status status, RIL_Errno e) {
  V1_0::RadioError ret = V1_0::RadioError::INTERNAL_ERR;
  switch (status) {
    case Message::Callback::Status::FAILURE:
    case Message::Callback::Status::SUCCESS:
      ret = (V1_0::RadioError)e;
      break;
    case Message::Callback::Status::CANCELLED:
      ret = V1_0::RadioError::CANCELLED;
      break;
    case Message::Callback::Status::NO_HANDLER_FOUND:
      ret = V1_0::RadioError::REQUEST_NOT_SUPPORTED;
      break;
    default:
      ret = V1_0::RadioError::INTERNAL_ERR;
      break;
  }
  return ret;
}

V1_0::CallState convertCallState(qcril::interfaces::CallState in) {
  switch (in) {
    case qcril::interfaces::CallState::ACTIVE:
      return V1_0::CallState::ACTIVE;
    case qcril::interfaces::CallState::HOLDING:
      return V1_0::CallState::HOLDING;
    case qcril::interfaces::CallState::DIALING:
      return V1_0::CallState::DIALING;
    case qcril::interfaces::CallState::ALERTING:
      return V1_0::CallState::ALERTING;
    case qcril::interfaces::CallState::INCOMING:
      return V1_0::CallState::INCOMING;
    case qcril::interfaces::CallState::WAITING:
      return V1_0::CallState::WAITING;
    default:
      return V1_0::CallState::ACTIVE;
  }
  return V1_0::CallState::ACTIVE;
}

V1_0::CallPresentation convertCallPresentation(uint32_t in) {
  switch (in) {
    case 0:
      return V1_0::CallPresentation::ALLOWED;
    case 1:
      return V1_0::CallPresentation::RESTRICTED;
    case 2:
    default:
      return V1_0::CallPresentation::UNKNOWN;
    case 3:
      return V1_0::CallPresentation::PAYPHONE;
  }
  return V1_0::CallPresentation::UNKNOWN;
}

V1_0::UusDcs convertUusDcsTypeToHal(RIL_UUS_DCS in) {
  switch (in) {
    case RIL_UUS_DCS_USP:
      return V1_0::UusDcs::USP;
    case RIL_UUS_DCS_OSIHLP:
      return V1_0::UusDcs::OSIHLP;
    case RIL_UUS_DCS_X244:
      return V1_0::UusDcs::X244;
    case RIL_UUS_DCS_RMCF:
      return V1_0::UusDcs::RMCF;
    case RIL_UUS_DCS_IA5c:
      return V1_0::UusDcs::IA5C;
    default:
      return V1_0::UusDcs::USP;
  }
  return V1_0::UusDcs::USP;
}

RIL_UUS_DCS convertUusDcsTypeFromHal(V1_0::UusDcs in) {
  switch (in) {
    case V1_0::UusDcs::USP:
      return RIL_UUS_DCS_USP;
    case V1_0::UusDcs::OSIHLP:
      return RIL_UUS_DCS_OSIHLP;
    case V1_0::UusDcs::X244:
      return RIL_UUS_DCS_X244;
    case V1_0::UusDcs::RMCF:
      return RIL_UUS_DCS_RMCF;
    case V1_0::UusDcs::IA5C:
      return RIL_UUS_DCS_IA5c;
    default:
      return RIL_UUS_DCS_USP;
  }
  return RIL_UUS_DCS_USP;
}


V1_0::UusType convertUssTypeToHal(RIL_UUS_Type in) {
  switch (in) {
    case RIL_UUS_TYPE1_IMPLICIT:
      return V1_0::UusType::TYPE1_IMPLICIT;
    case RIL_UUS_TYPE1_REQUIRED:
      return V1_0::UusType::TYPE1_REQUIRED;
    case RIL_UUS_TYPE1_NOT_REQUIRED:
      return V1_0::UusType::TYPE1_NOT_REQUIRED;
    case RIL_UUS_TYPE2_REQUIRED:
      return V1_0::UusType::TYPE2_REQUIRED;
    case RIL_UUS_TYPE2_NOT_REQUIRED:
      return V1_0::UusType::TYPE2_NOT_REQUIRED;
    case RIL_UUS_TYPE3_REQUIRED:
      return V1_0::UusType::TYPE3_REQUIRED;
    case RIL_UUS_TYPE3_NOT_REQUIRED:
      return V1_0::UusType::TYPE3_NOT_REQUIRED;
    default:
      return V1_0::UusType::TYPE1_IMPLICIT;
  }
  return V1_0::UusType::TYPE1_IMPLICIT;
}

RIL_UUS_Type convertUssTypeFromHal(V1_0::UusType in) {
  switch (in) {
    case V1_0::UusType::TYPE1_IMPLICIT:
      return RIL_UUS_TYPE1_IMPLICIT;
    case V1_0::UusType::TYPE1_REQUIRED:
      return RIL_UUS_TYPE1_REQUIRED;
    case V1_0::UusType::TYPE1_NOT_REQUIRED:
      return RIL_UUS_TYPE1_NOT_REQUIRED;
    case V1_0::UusType::TYPE2_REQUIRED:
      return RIL_UUS_TYPE2_REQUIRED;
    case V1_0::UusType::TYPE2_NOT_REQUIRED:
      return RIL_UUS_TYPE2_NOT_REQUIRED;
    case V1_0::UusType::TYPE3_REQUIRED:
      return RIL_UUS_TYPE3_REQUIRED;
    case V1_0::UusType::TYPE3_NOT_REQUIRED:
      return RIL_UUS_TYPE3_NOT_REQUIRED;
    default:
      return RIL_UUS_TYPE1_IMPLICIT;
  }
  return RIL_UUS_TYPE1_IMPLICIT;
}

V1_0::UssdModeType convertUssdModeType(qcril::interfaces::UssdModeType in) {
  switch (in) {
    case qcril::interfaces::UssdModeType::NOTIFY:
      return V1_0::UssdModeType::NOTIFY;
    case qcril::interfaces::UssdModeType::REQUEST:
      return V1_0::UssdModeType::REQUEST;
    case qcril::interfaces::UssdModeType::NW_RELEASE:
      return V1_0::UssdModeType::NW_RELEASE;
    case qcril::interfaces::UssdModeType::LOCAL_CLIENT:
      return V1_0::UssdModeType::LOCAL_CLIENT;
    case qcril::interfaces::UssdModeType::NOT_SUPPORTED:
      return V1_0::UssdModeType::NOT_SUPPORTED;
    case qcril::interfaces::UssdModeType::NW_TIMEOUT:
      return V1_0::UssdModeType::NW_TIMEOUT;
    default:
      return V1_0::UssdModeType::NOTIFY;
  }
  return V1_0::UssdModeType::NOTIFY;
}

V1_0::CallForwardInfoStatus convertCallForwardInfoStatus(int in) {
  switch (in) {
    case 0:  // disable
      return V1_0::CallForwardInfoStatus::DISABLE;
    case 1:  // enable
      return V1_0::CallForwardInfoStatus::ENABLE;
    case 2:  // interrogate
      return V1_0::CallForwardInfoStatus::INTERROGATE;
    case 3:  // registeration
      return V1_0::CallForwardInfoStatus::REGISTRATION;
    case 4:  // erasure
      return V1_0::CallForwardInfoStatus::ERASURE;
  }
  return V1_0::CallForwardInfoStatus::DISABLE;
}

int convertCallForwardInfoStatus(V1_0::CallForwardInfoStatus in) {
  switch (in) {
    case V1_0::CallForwardInfoStatus::DISABLE:
      return 0;  // disable
    case V1_0::CallForwardInfoStatus::ENABLE:
      return 1;  // enable
    case V1_0::CallForwardInfoStatus::INTERROGATE:
      return 2;  // interrogate
    case V1_0::CallForwardInfoStatus::REGISTRATION:
      return 3;  // registeration
    case V1_0::CallForwardInfoStatus::ERASURE:
      return 4;  // erasure
  }
  return 0;
}

V1_0::OperatorStatus convertRilNetworkStatusToHidl(qcril::interfaces::NetworkStatus status) {
  V1_0::OperatorStatus res = V1_0::OperatorStatus::UNKNOWN;
  switch (status) {
    case qcril::interfaces::NetworkStatus::UNKNOWN:
      res = V1_0::OperatorStatus::UNKNOWN;
      break;
    case qcril::interfaces::NetworkStatus::AVAILABLE:
      res = V1_0::OperatorStatus::AVAILABLE;
      break;
    case qcril::interfaces::NetworkStatus::CURRENT:
      res = V1_0::OperatorStatus::CURRENT;
      break;
    case qcril::interfaces::NetworkStatus::FORBIDDEN:
      res = V1_0::OperatorStatus::FORBIDDEN;
      break;
    default:
      res = V1_0::OperatorStatus::UNKNOWN;
      break;
  }

  return res;
}

int convertNwTypeToRadioAccessFamily(V1_0::PreferredNetworkType nwType) {
  uint32_t radioAccessFamily = 0;
  switch (nwType) {
    case V1_0::PreferredNetworkType::GSM_WCDMA:
      radioAccessFamily = QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
      break;
    case V1_0::PreferredNetworkType::GSM_ONLY:
      radioAccessFamily = QCRIL_RAF_GSM;
      break;
    case V1_0::PreferredNetworkType::WCDMA:
      radioAccessFamily = QCRIL_RAF_WCDMA;
      break;
    case V1_0::PreferredNetworkType::GSM_WCDMA_AUTO:
      radioAccessFamily = QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_AUTO;
      break;
    case V1_0::PreferredNetworkType::CDMA_EVDO_AUTO:
      radioAccessFamily = QCRIL_RAF_CDMA | QCRIL_RAF_EVDO;
      break;
    case V1_0::PreferredNetworkType::CDMA_ONLY:
      radioAccessFamily = QCRIL_RAF_CDMA;
      break;
    case V1_0::PreferredNetworkType::EVDO_ONLY:
      radioAccessFamily = QCRIL_RAF_EVDO;
      break;
    case V1_0::PreferredNetworkType::GSM_WCDMA_CDMA_EVDO_AUTO:
      radioAccessFamily = QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO;
      break;
    case V1_0::PreferredNetworkType::LTE_CDMA_EVDO:
      radioAccessFamily = QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO;
      break;
    case V1_0::PreferredNetworkType::LTE_GSM_WCDMA:
      radioAccessFamily = QCRIL_RAF_LTE | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
      break;
    case V1_0::PreferredNetworkType::LTE_CMDA_EVDO_GSM_WCDMA:
      radioAccessFamily =
          QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
      break;
    case V1_0::PreferredNetworkType::LTE_ONLY:
      radioAccessFamily = QCRIL_RAF_LTE;
      break;
    case V1_0::PreferredNetworkType::LTE_WCDMA:
      radioAccessFamily = QCRIL_RAF_LTE | QCRIL_RAF_WCDMA;
      break;
    case V1_0::PreferredNetworkType::TD_SCDMA_ONLY:
      radioAccessFamily = RAF_TD_SCDMA;
      break;
    case V1_0::PreferredNetworkType::TD_SCDMA_WCDMA:
      radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_WCDMA;
      break;
    case V1_0::PreferredNetworkType::TD_SCDMA_LTE:
      radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_LTE;
      break;
    case V1_0::PreferredNetworkType::TD_SCDMA_GSM:
      radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_GSM;
      break;
    case V1_0::PreferredNetworkType::TD_SCDMA_GSM_LTE:
      radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_LTE;
      break;
    case V1_0::PreferredNetworkType::TD_SCDMA_GSM_WCDMA:
      radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
      break;
    case V1_0::PreferredNetworkType::TD_SCDMA_WCDMA_LTE:
      radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_WCDMA | QCRIL_RAF_LTE;
      break;
    case V1_0::PreferredNetworkType::TD_SCDMA_GSM_WCDMA_LTE:
      radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_LTE;
      break;
    case V1_0::PreferredNetworkType::TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO:
      radioAccessFamily =
          RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO;
      break;
    case V1_0::PreferredNetworkType::TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
      radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO |
                          QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
      break;
    default:
      radioAccessFamily = RAF_UNKNOWN;
      break;
  }
  return radioAccessFamily;
}

V1_0::PreferredNetworkType convertRadioAccessFamilyToNwType(int radioAccessFamily) {
  V1_0::PreferredNetworkType nwType = V1_0::PreferredNetworkType::GSM_WCDMA;

  switch (radioAccessFamily) {
    case (QCRIL_RAF_GSM | QCRIL_RAF_WCDMA):
      nwType = V1_0::PreferredNetworkType::GSM_WCDMA;
      break;
    case QCRIL_RAF_GSM:
      nwType = V1_0::PreferredNetworkType::GSM_ONLY;
      break;
    case QCRIL_RAF_WCDMA:
      nwType = V1_0::PreferredNetworkType::WCDMA;
      break;
    case (QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_AUTO):
      nwType = V1_0::PreferredNetworkType::GSM_WCDMA_AUTO;
      break;
    case (QCRIL_RAF_CDMA | QCRIL_RAF_EVDO):
      nwType = V1_0::PreferredNetworkType::CDMA_EVDO_AUTO;
      break;
    case QCRIL_RAF_CDMA:
      nwType = V1_0::PreferredNetworkType::CDMA_ONLY;
      break;
    case QCRIL_RAF_EVDO:
      nwType = V1_0::PreferredNetworkType::EVDO_ONLY;
      break;
    case (QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO):
      nwType = V1_0::PreferredNetworkType::GSM_WCDMA_CDMA_EVDO_AUTO;
      break;
    case (QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO):
      nwType = V1_0::PreferredNetworkType::LTE_CDMA_EVDO;
      break;
    case (QCRIL_RAF_LTE | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA):
      nwType = V1_0::PreferredNetworkType::LTE_GSM_WCDMA;
      break;
    case (QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA):
      nwType = V1_0::PreferredNetworkType::LTE_CMDA_EVDO_GSM_WCDMA;
      break;
    case (QCRIL_RAF_LTE):
      nwType = V1_0::PreferredNetworkType::LTE_ONLY;
      break;
    case (QCRIL_RAF_LTE | QCRIL_RAF_WCDMA):
      nwType = V1_0::PreferredNetworkType::LTE_WCDMA;
      break;
    case RAF_TD_SCDMA:
      nwType = V1_0::PreferredNetworkType::TD_SCDMA_ONLY;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_WCDMA):
      nwType = V1_0::PreferredNetworkType::TD_SCDMA_WCDMA;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_LTE):
      nwType = V1_0::PreferredNetworkType::TD_SCDMA_LTE;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_GSM):
      nwType = V1_0::PreferredNetworkType::TD_SCDMA_GSM;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_LTE):
      nwType = V1_0::PreferredNetworkType::TD_SCDMA_GSM_LTE;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA):
      nwType = V1_0::PreferredNetworkType::TD_SCDMA_GSM_WCDMA;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_WCDMA | QCRIL_RAF_LTE):
      nwType = V1_0::PreferredNetworkType::TD_SCDMA_WCDMA_LTE;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_LTE):
      nwType = V1_0::PreferredNetworkType::TD_SCDMA_GSM_WCDMA_LTE;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO):
      nwType = V1_0::PreferredNetworkType::TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO | QCRIL_RAF_GSM |
          QCRIL_RAF_WCDMA):
      nwType = V1_0::PreferredNetworkType::TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA;
      break;
    default:
      nwType = V1_0::PreferredNetworkType::GSM_WCDMA;
      break;
  }
  return nwType;
}

qcril::interfaces::FacilityType convertFacilityType(std::string in) {
  if (in == "AO") {
    return qcril::interfaces::FacilityType::BAOC;
  } else if (in == "OI") {
    return qcril::interfaces::FacilityType::BAOIC;
  } else if (in == "OX") {
    return qcril::interfaces::FacilityType::BAOICxH;
  } else if (in == "AI") {
    return qcril::interfaces::FacilityType::BAIC;
  } else if (in == "IR") {
    return qcril::interfaces::FacilityType::BAICr;
  } else if (in == "AB") {
    return qcril::interfaces::FacilityType::BA_ALL;
  } else if (in == "AG") {
    return qcril::interfaces::FacilityType::BA_MO;
  } else if (in == "AC") {
    return qcril::interfaces::FacilityType::BA_MT;
  }
  return qcril::interfaces::FacilityType::UNKNOWN;
}

int convertToHidl(V1_0::Call& out, const qcril::interfaces::CallInfo& in) {
  /* each call info */
  out.state = convertCallState(in.getCallState());
  out.index = in.getIndex();
  out.toa = in.getToa();
  out.isMpty = in.getIsMpty();
  out.isMT = in.getIsMt();
  out.als = in.getAls();
  out.isVoice = in.getIsVoice();
  out.isVoicePrivacy = in.getIsVoicePrivacy();
  out.number = in.getNumber().c_str();
  out.numberPresentation = convertCallPresentation(in.getNumberPresentation());
  out.name = in.getName().c_str();
  out.namePresentation = convertCallPresentation(in.getNamePresentation());
  if (in.hasUusInfo() && in.getUusInfo() != NULL) {
    std::shared_ptr<qcril::interfaces::UusInfo> uusInfo = in.getUusInfo();
    if (uusInfo->hasData() && !uusInfo->getData().empty()) {
      out.uusInfo.resize(1);
      out.uusInfo[0].uusType = static_cast<V1_0::UusType>(uusInfo->getType());
      out.uusInfo[0].uusDcs = static_cast<V1_0::UusDcs>(uusInfo->getDcs());
      out.uusInfo[0].uusData = uusInfo->getData().c_str();
    }
  }
  return 0;
}

int convertToHal(V1_0::WcdmaSignalStrength& out, const RIL_WCDMA_SignalStrength& in) {
  out.signalStrength = in.signalStrength;
  out.bitErrorRate = in.bitErrorRate;
  return 0;
}

int convertToHal(V1_0::GsmSignalStrength& out, const RIL_GW_SignalStrength& in) {
  out.signalStrength = in.signalStrength;
  out.bitErrorRate = in.bitErrorRate;
  return 0;
}

int convertToHal(V1_0::GsmSignalStrength& out, const RIL_WCDMA_SignalStrength& in) {
  out.signalStrength = in.signalStrength;
  out.bitErrorRate = in.bitErrorRate;
  return 0;
}

int convertToHal(V1_0::WcdmaSignalStrength& out, const RIL_GW_SignalStrength& in) {
  out.signalStrength = in.signalStrength;
  out.bitErrorRate = in.bitErrorRate;
  return 0;
}

int convertToHal(V1_0::CdmaSignalStrength& out, const RIL_CDMA_SignalStrength& in) {
  out.dbm = in.dbm;
  out.ecio = in.ecio;
  return 0;
}

int convertToHal(V1_0::EvdoSignalStrength& out, const RIL_EVDO_SignalStrength& in) {
  out.dbm = in.dbm;
  out.ecio = in.ecio;
  out.signalNoiseRatio = in.signalNoiseRatio;
  return 0;
}

int convertToHal(V1_0::LteSignalStrength& out, RIL_LTE_SignalStrength_v8 in) {
  // Fixup LTE for backwards compatibility
  // signalStrength: -1 -> 99
  if (in.signalStrength == -1) {
    in.signalStrength = 99;
  }
  // rsrp: -1 -> INT_MAX all other negative value to positive.
  // So remap here
  if (in.rsrp == -1) {
    in.rsrp = INT_MAX;
  } else if (in.rsrp < -1) {
    in.rsrp = -in.rsrp;
  }
  // rsrq: -1 -> INT_MAX
  if (in.rsrq == -1) {
    in.rsrq = INT_MAX;
  }
  // Not remapping rssnr is already using INT_MAX
  // cqi: -1 -> INT_MAX
  if (in.cqi == -1) {
    in.cqi = INT_MAX;
  }

  out.signalStrength = in.signalStrength;
  out.rsrp = in.rsrp;
  out.rsrq = in.rsrq;
  out.rssnr = in.rssnr;
  out.cqi = in.cqi;
  out.timingAdvance = in.timingAdvance;
  return 0;
}

int convertToHal(V1_0::TdScdmaSignalStrength& out, const RIL_TD_SCDMA_SignalStrength& in) {
  out.rscp = in.rscp;
  return 0;
}

int convertRilSignalStrengthToHal(V1_0::SignalStrength& signalStrength,
                                  const RIL_SignalStrength& rilSignalStrength) {
  if (rilSignalStrength.WCDMA_SignalStrength.valid) {
    convertToHal(signalStrength.gw, rilSignalStrength.GW_SignalStrength);
  } else {
    convertToHal(signalStrength.gw, rilSignalStrength.WCDMA_SignalStrength);
  }
  convertToHal(signalStrength.cdma, rilSignalStrength.CDMA_SignalStrength);
  convertToHal(signalStrength.evdo, rilSignalStrength.EVDO_SignalStrength);
  convertToHal(signalStrength.lte, rilSignalStrength.LTE_SignalStrength);
  convertToHal(signalStrength.tdScdma, rilSignalStrength.TD_SCDMA_SignalStrength);
  return 0;
}

V1_0::SendSmsResult makeSendSmsResult(std::shared_ptr<RilSendSmsResult_t> in) {
  V1_0::SendSmsResult result = {};
  if (!in) {
    result.ackPDU = hidl_string();
  } else {
    result.messageRef = in->messageRef;
    result.ackPDU = in->ackPDU;
    result.errorCode = in->errorCode;
  }
  return result;
}

void convertRilNetworkResultToHidl(const std::vector<qcril::interfaces::NetworkInfo>& rilData,
                                   hidl_vec<V1_0::OperatorInfo>& hidlData) {
  hidlData.resize(rilData.size());
  for (size_t i = 0; i < rilData.size(); i++) {
    hidlData[i].alphaLong = rilData[i].alphaLong;
    hidlData[i].alphaShort = rilData[i].alphaShort;
    hidlData[i].operatorNumeric = rilData[i].operatorNumeric;
    hidlData[i].status = convertRilNetworkStatusToHidl(rilData[i].status);
  }
}

void convertRilNeighboringCidResultToHidl(
    const std::vector<qcril::interfaces::RilNeighboringCell_t>& rilCell,
    hidl_vec<V1_0::NeighboringCell>& hidlCell) {
  auto num = rilCell.size();
  hidlCell.resize(num);
  for (unsigned int i = 0; i < num; i++) {
    hidlCell[i].cid = rilCell[i].cid;
    hidlCell[i].rssi = rilCell[i].rssi;
  }
}

void convertRilRadioCapabilityToHal(V1_0::RadioCapability& hidlRc,
                                    const RIL_RadioCapability& rilRc) {
  hidlRc.session = rilRc.session;
  hidlRc.phase = static_cast<V1_0::RadioCapabilityPhase>(rilRc.phase);
  hidlRc.raf = rilRc.rat;
  hidlRc.logicalModemUuid = convertCharPtrToHidlString(rilRc.logicalModemUuid);
  hidlRc.status = static_cast<V1_0::RadioCapabilityStatus>(rilRc.status);
}

void constructCdmaSms(RIL_CDMA_SMS_Message& rcsm, const V1_0::CdmaSmsMessage& sms) {
  rcsm.uTeleserviceID = sms.teleserviceId;
  rcsm.bIsServicePresent = BOOL_TO_INT(sms.isServicePresent);
  rcsm.uServicecategory = sms.serviceCategory;
  rcsm.sAddress.digit_mode = static_cast<RIL_CDMA_SMS_DigitMode>(sms.address.digitMode);
  rcsm.sAddress.number_mode = static_cast<RIL_CDMA_SMS_NumberMode>(sms.address.numberMode);
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

bool isUssdOverImsSupported() {
  bool ussdOverImsSupported = false;
  bool bool_config;
  if (qcril_config_get(PERSIST_VENDOR_RADIO_IS_USSD_SUPPORTED, bool_config) == E_SUCCESS) {
    ussdOverImsSupported = bool_config;
  }
  boolean ussdImsRuntimeSupport = TRUE;
  qmi_ril_get_property_value_from_boolean(QCRIL_USSD_IMS_RUNTIME_SUPPORT, &ussdImsRuntimeSupport,
                                          TRUE);
  return ussdOverImsSupported && ussdImsRuntimeSupport;
}

void convertRilCdmaSignalInfoRecordToHal(V1_0::CdmaSignalInfoRecord& out,
                                         std::shared_ptr<RIL_CDMA_SignalInfoRecord> in) {
  if (in) {
    out.isPresent = in->isPresent;
    out.signalType = in->signalType;
    out.alertPitch = in->alertPitch;
    out.signal = in->signal;
  }
}

void convertRilCdmaSignalInfoRecord(V1_0::CdmaSignalInfoRecord& out,
                                    std::shared_ptr<RIL_CDMA_SignalInfoRecord> in) {
  if (in) {
    out.isPresent = in->isPresent;
    out.signalType = in->signalType;
    out.alertPitch = in->alertPitch;
    out.signal = in->signal;
  }
}

void convertRilCellInfoListToHal(const std::vector<RIL_CellInfo_v12>& rillCellInfo,
                                 hidl_vec<V1_0::CellInfo>& records) {
  auto num = rillCellInfo.size();
  records.resize(num);

  for (unsigned int i = 0; i < num; i++) {
    records[i].cellInfoType = static_cast<V1_0::CellInfoType>(rillCellInfo[i].cellInfoType);
    records[i].registered = rillCellInfo[i].registered;
    records[i].timeStampType = static_cast<V1_0::TimeStampType>(rillCellInfo[i].timeStampType);
    records[i].timeStamp = rillCellInfo[i].timeStamp;
    // All vectors should be size 0 except one which will be size 1. Set everything to
    // size 0 initially.
    records[i].gsm.resize(0);
    records[i].wcdma.resize(0);
    records[i].cdma.resize(0);
    records[i].lte.resize(0);
    records[i].tdscdma.resize(0);
    switch (rillCellInfo[i].cellInfoType) {
      case RIL_CELL_INFO_TYPE_GSM: {
        records[i].gsm.resize(1);
        V1_0::CellInfoGsm* cellInfoGsm = &records[i].gsm[0];

        new (&cellInfoGsm->cellIdentityGsm.mcc)
            hidl_string(rillCellInfo[i].CellInfo.gsm.cellIdentityGsm.mcc,
                        strnlen(rillCellInfo[i].CellInfo.gsm.cellIdentityGsm.mcc,
                                sizeof(rillCellInfo[i].CellInfo.gsm.cellIdentityGsm.mcc) - 1));

        new (&cellInfoGsm->cellIdentityGsm.mnc)
            hidl_string(rillCellInfo[i].CellInfo.gsm.cellIdentityGsm.mnc,
                        strnlen(rillCellInfo[i].CellInfo.gsm.cellIdentityGsm.mnc,
                                sizeof(rillCellInfo[i].CellInfo.gsm.cellIdentityGsm.mnc) - 1));

        cellInfoGsm->cellIdentityGsm.lac = rillCellInfo[i].CellInfo.gsm.cellIdentityGsm.lac;
        cellInfoGsm->cellIdentityGsm.cid = rillCellInfo[i].CellInfo.gsm.cellIdentityGsm.cid;
        cellInfoGsm->cellIdentityGsm.arfcn = rillCellInfo[i].CellInfo.gsm.cellIdentityGsm.arfcn;
        cellInfoGsm->cellIdentityGsm.bsic = rillCellInfo[i].CellInfo.gsm.cellIdentityGsm.bsic;
        cellInfoGsm->signalStrengthGsm.signalStrength =
            rillCellInfo[i].CellInfo.gsm.signalStrengthGsm.signalStrength;
        cellInfoGsm->signalStrengthGsm.bitErrorRate =
            rillCellInfo[i].CellInfo.gsm.signalStrengthGsm.bitErrorRate;
        cellInfoGsm->signalStrengthGsm.timingAdvance =
            rillCellInfo[i].CellInfo.gsm.signalStrengthGsm.timingAdvance;
        break;
      }

      case RIL_CELL_INFO_TYPE_WCDMA: {
        records[i].wcdma.resize(1);
        V1_0::CellInfoWcdma* cellInfoWcdma = &records[i].wcdma[0];

        new (&cellInfoWcdma->cellIdentityWcdma.mcc)
            hidl_string(rillCellInfo[i].CellInfo.wcdma.cellIdentityWcdma.mcc,
                        strnlen(rillCellInfo[i].CellInfo.wcdma.cellIdentityWcdma.mcc,
                                sizeof(rillCellInfo[i].CellInfo.wcdma.cellIdentityWcdma.mcc) - 1));

        new (&cellInfoWcdma->cellIdentityWcdma.mnc)
            hidl_string(rillCellInfo[i].CellInfo.wcdma.cellIdentityWcdma.mnc,
                        strnlen(rillCellInfo[i].CellInfo.wcdma.cellIdentityWcdma.mnc,
                                sizeof(rillCellInfo[i].CellInfo.wcdma.cellIdentityWcdma.mnc) - 1));

        cellInfoWcdma->cellIdentityWcdma.lac = rillCellInfo[i].CellInfo.wcdma.cellIdentityWcdma.lac;
        cellInfoWcdma->cellIdentityWcdma.cid = rillCellInfo[i].CellInfo.wcdma.cellIdentityWcdma.cid;
        cellInfoWcdma->cellIdentityWcdma.psc = rillCellInfo[i].CellInfo.wcdma.cellIdentityWcdma.psc;
        cellInfoWcdma->cellIdentityWcdma.uarfcn =
            rillCellInfo[i].CellInfo.wcdma.cellIdentityWcdma.uarfcn;
        cellInfoWcdma->signalStrengthWcdma.signalStrength =
            rillCellInfo[i].CellInfo.wcdma.signalStrengthWcdma.signalStrength;
        cellInfoWcdma->signalStrengthWcdma.bitErrorRate =
            rillCellInfo[i].CellInfo.wcdma.signalStrengthWcdma.bitErrorRate;
        break;
      }

      case RIL_CELL_INFO_TYPE_CDMA: {
        records[i].cdma.resize(1);
        V1_0::CellInfoCdma* cellInfoCdma = &records[i].cdma[0];
        cellInfoCdma->cellIdentityCdma.networkId =
            rillCellInfo[i].CellInfo.cdma.cellIdentityCdma.networkId;
        cellInfoCdma->cellIdentityCdma.systemId =
            rillCellInfo[i].CellInfo.cdma.cellIdentityCdma.systemId;
        cellInfoCdma->cellIdentityCdma.baseStationId =
            rillCellInfo[i].CellInfo.cdma.cellIdentityCdma.basestationId;
        cellInfoCdma->cellIdentityCdma.longitude =
            rillCellInfo[i].CellInfo.cdma.cellIdentityCdma.longitude;
        cellInfoCdma->cellIdentityCdma.latitude =
            rillCellInfo[i].CellInfo.cdma.cellIdentityCdma.latitude;
        cellInfoCdma->signalStrengthCdma.dbm = rillCellInfo[i].CellInfo.cdma.signalStrengthCdma.dbm;
        cellInfoCdma->signalStrengthCdma.ecio =
            rillCellInfo[i].CellInfo.cdma.signalStrengthCdma.ecio;
        cellInfoCdma->signalStrengthEvdo.dbm = rillCellInfo[i].CellInfo.cdma.signalStrengthEvdo.dbm;
        cellInfoCdma->signalStrengthEvdo.ecio =
            rillCellInfo[i].CellInfo.cdma.signalStrengthEvdo.ecio;
        cellInfoCdma->signalStrengthEvdo.signalNoiseRatio =
            rillCellInfo[i].CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio;
        break;
      }

      case RIL_CELL_INFO_TYPE_LTE: {
        records[i].lte.resize(1);
        V1_0::CellInfoLte* cellInfoLte = &records[i].lte[0];

        new (&cellInfoLte->cellIdentityLte.mcc)
            hidl_string(rillCellInfo[i].CellInfo.lte.cellIdentityLte.mcc,
                        strnlen(rillCellInfo[i].CellInfo.lte.cellIdentityLte.mcc,
                                sizeof(rillCellInfo[i].CellInfo.lte.cellIdentityLte.mcc) - 1));

        new (&cellInfoLte->cellIdentityLte.mnc)
            hidl_string(rillCellInfo[i].CellInfo.lte.cellIdentityLte.mnc,
                        strnlen(rillCellInfo[i].CellInfo.lte.cellIdentityLte.mnc,
                                sizeof(rillCellInfo[i].CellInfo.lte.cellIdentityLte.mnc) - 1));

        cellInfoLte->cellIdentityLte.ci = rillCellInfo[i].CellInfo.lte.cellIdentityLte.ci;
        cellInfoLte->cellIdentityLte.pci = rillCellInfo[i].CellInfo.lte.cellIdentityLte.pci;
        cellInfoLte->cellIdentityLte.tac = rillCellInfo[i].CellInfo.lte.cellIdentityLte.tac;
        cellInfoLte->cellIdentityLte.earfcn = rillCellInfo[i].CellInfo.lte.cellIdentityLte.earfcn;
        cellInfoLte->signalStrengthLte.signalStrength =
            rillCellInfo[i].CellInfo.lte.signalStrengthLte.signalStrength;
        cellInfoLte->signalStrengthLte.rsrp = rillCellInfo[i].CellInfo.lte.signalStrengthLte.rsrp;
        cellInfoLte->signalStrengthLte.rsrq = rillCellInfo[i].CellInfo.lte.signalStrengthLte.rsrq;
        cellInfoLte->signalStrengthLte.rssnr = rillCellInfo[i].CellInfo.lte.signalStrengthLte.rssnr;
        cellInfoLte->signalStrengthLte.cqi = rillCellInfo[i].CellInfo.lte.signalStrengthLte.cqi;
        cellInfoLte->signalStrengthLte.timingAdvance =
            rillCellInfo[i].CellInfo.lte.signalStrengthLte.timingAdvance;
        break;
      }

      case RIL_CELL_INFO_TYPE_TD_SCDMA: {
        records[i].tdscdma.resize(1);
        V1_0::CellInfoTdscdma* cellInfoTdscdma = &records[i].tdscdma[0];

        new (&cellInfoTdscdma->cellIdentityTdscdma.mcc) hidl_string(
            rillCellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.mcc,
            strnlen(rillCellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.mcc,
                    sizeof(rillCellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.mcc) - 1));

        new (&cellInfoTdscdma->cellIdentityTdscdma.mnc) hidl_string(
            rillCellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.mnc,
            strnlen(rillCellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.mnc,
                    sizeof(rillCellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.mnc) - 1));

        cellInfoTdscdma->cellIdentityTdscdma.lac =
            rillCellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.lac;
        cellInfoTdscdma->cellIdentityTdscdma.cid =
            rillCellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.cid;
        cellInfoTdscdma->cellIdentityTdscdma.cpid =
            rillCellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.cpid;
        cellInfoTdscdma->signalStrengthTdscdma.rscp =
            rillCellInfo[i].CellInfo.tdscdma.signalStrengthTdscdma.rscp;
        break;
      }

      default: {
        break;
      }
    }
  }
}

bool makeCdmaSmsMessage(V1_0::CdmaSmsMessage& msg,
                        std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> shared_msg) {
  if (shared_msg) {
    auto& rilMsg = shared_msg->getCdmaSms();

    msg.teleserviceId = rilMsg.uTeleserviceID;
    msg.isServicePresent = rilMsg.bIsServicePresent;
    msg.serviceCategory = rilMsg.uServicecategory;
    msg.address.digitMode = static_cast<V1_0::CdmaSmsDigitMode>(rilMsg.sAddress.digit_mode);
    msg.address.numberMode = static_cast<V1_0::CdmaSmsNumberMode>(rilMsg.sAddress.number_mode);
    msg.address.numberType = static_cast<V1_0::CdmaSmsNumberType>(rilMsg.sAddress.number_type);
    msg.address.numberPlan = static_cast<V1_0::CdmaSmsNumberPlan>(rilMsg.sAddress.number_plan);

    int digitLimit = MIN((rilMsg.sAddress.number_of_digits), RIL_CDMA_SMS_ADDRESS_MAX);
    msg.address.digits.setToExternal(rilMsg.sAddress.digits, digitLimit);

    msg.subAddress.subaddressType =
        static_cast<V1_0::CdmaSmsSubaddressType>(rilMsg.sSubAddress.subaddressType);
    msg.subAddress.odd = rilMsg.sSubAddress.odd;

    digitLimit = MIN((rilMsg.sSubAddress.number_of_digits), RIL_CDMA_SMS_SUBADDRESS_MAX);
    msg.subAddress.digits.setToExternal(rilMsg.sSubAddress.digits, digitLimit);

    digitLimit = MIN((rilMsg.uBearerDataLen), RIL_CDMA_SMS_BEARER_DATA_MAX);
    msg.bearerData.setToExternal(rilMsg.aBearerData, digitLimit);

    return true;
  }
  return false;
}

bool makeSuppSvcNotification(V1_0::SuppSvcNotification& suppSvc,
                             std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg) {
  if (msg) {
    if (msg->hasNotificationType()) {
      if (msg->getNotificationType() == qcril::interfaces::NotificationType::MT) {
        suppSvc.isMT = true;
      } else {
        suppSvc.isMT = false;
      }
    }
    if (msg->hasCode()) {
      suppSvc.code = msg->getCode();
    }
    if (msg->hasIndex()) {
      suppSvc.index = msg->getIndex();
    }
    if (msg->hasType()) {
      suppSvc.type = msg->getType();
    }
    if (msg->hasNumber() && !msg->getNumber().empty()) {
      suppSvc.number = msg->getNumber().c_str();
    }
    return true;
  }
  return false;
}

bool makeCdmaCallWaiting(V1_0::CdmaCallWaiting& callWaitingRecord,
                         std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg) {
  if (msg) {
    if (msg->hasNumber() && !msg->getNumber().empty()) {
      callWaitingRecord.number = msg->getNumber().c_str();
    }
    if (msg->hasNumberPresentation()) {
      callWaitingRecord.numberPresentation =
          static_cast<V1_0::CdmaCallWaitingNumberPresentation>(msg->getNumberPresentation());
    }
    if (msg->hasName() && !msg->getName().empty()) {
      callWaitingRecord.name = msg->getName().c_str();
    }
    if (msg->hasNumberType()) {
      callWaitingRecord.numberType =
          static_cast<V1_0::CdmaCallWaitingNumberType>(msg->getNumberType());
    }
    if (msg->hasNumberPlan()) {
      callWaitingRecord.numberPlan =
          static_cast<V1_0::CdmaCallWaitingNumberPlan>(msg->getNumberPlan());
    }
    if (msg->hasSignalInfoRecord() && msg->getSignalInfoRecord() != nullptr) {
      convertRilCdmaSignalInfoRecordToHal(callWaitingRecord.signalInfoRecord,
                                          msg->getSignalInfoRecord());
    }
    return true;
  }
  return false;
}

bool makeStkCcUnsolSsResult(V1_0::StkCcUnsolSsResult& ss,
                            std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg) {
  if (msg) {
    if (msg->hasServiceType()) {
      ss.serviceType = static_cast<V1_0::SsServiceType>(msg->getServiceType());
    }
    if (msg->hasRequestType()) {
      ss.requestType = static_cast<V1_0::SsRequestType>(msg->getRequestType());
    }
    if (msg->hasTeleserviceType()) {
      ss.teleserviceType = static_cast<V1_0::SsTeleserviceType>(msg->getTeleserviceType());
    }
    if (msg->hasServiceClass()) {
      ss.serviceClass = msg->getServiceClass();
    }
    if (msg->hasResult()) {
      ss.result = static_cast<V1_0::RadioError>(msg->getResult());
    }
    if (isServiceTypeCfQuery(msg->getServiceType(), msg->getRequestType()) &&
        msg->hasCallForwardInfoList() && !msg->getCallForwardInfoList().empty()) {
      std::vector<qcril::interfaces::CallForwardInfo> rilCfInfo = msg->getCallForwardInfoList();
      ss.cfData.resize(1);
      ss.ssInfo.resize(0);
      /* number of call info's */
      ss.cfData[0].cfInfo.resize(rilCfInfo.size());
      for (int i = 0; i < rilCfInfo.size() && i < NUM_SERVICE_CLASSES; i++) {
        qcril::interfaces::CallForwardInfo cf = rilCfInfo[i];
        V1_0::CallForwardInfo* cfInfo = &ss.cfData[0].cfInfo[i];
        if (cf.hasStatus()) {
          cfInfo->status = static_cast<V1_0::CallForwardInfoStatus>(cf.getStatus());
        }
        if (cf.hasReason()) {
          cfInfo->reason = cf.getReason();
        }
        if (cf.hasServiceClass()) {
          cfInfo->serviceClass = cf.getServiceClass();
        }
        if (cf.hasToa()) {
          cfInfo->toa = cf.getToa();
        }
        if (!cf.getNumber().empty()) {
          cfInfo->number = cf.getNumber().c_str();
        }
        if (cf.hasTimeSeconds()) {
          cfInfo->timeSeconds = cf.getTimeSeconds();
        }
      }
    } else {
      ss.ssInfo.resize(1);
      ss.cfData.resize(0);
      if (msg->hasSuppSrvInfoList() && !msg->getSuppSrvInfoList().empty()) {
        /* each int */
        ss.ssInfo[0].ssInfo.resize(SS_INFO_MAX);
        for (int i = 0; i < msg->getSuppSrvInfoList().size() && i < SS_INFO_MAX; i++) {
          ss.ssInfo[0].ssInfo[i] = msg->getSuppSrvInfoList()[i];
        }
      }
    }
    return true;
  }
  return false;
}

bool makeCdmaInformationRecords(V1_0::CdmaInformationRecords& records,
                                std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg) {
  if (msg && msg->hasCdmaInfoRecords() && msg->getCdmaInfoRecords() != nullptr) {
    std::shared_ptr<RIL_CDMA_InformationRecords> recordsRil = msg->getCdmaInfoRecords();

    char* string8 = NULL;
    int num = MIN(recordsRil->numberOfInfoRecs, RIL_CDMA_MAX_NUMBER_OF_INFO_RECS);
#if 0
    if (recordsRil->numberOfInfoRecs > RIL_CDMA_MAX_NUMBER_OF_INFO_RECS) {
      RLOGE(
          "cdmaInfoRecInd: received %d recs which is more than %d, dropping "
          "additional ones",
          recordsRil->numberOfInfoRecs, RIL_CDMA_MAX_NUMBER_OF_INFO_RECS);
    }
#endif
    records.infoRec.resize(num);
    for (int i = 0; i < num; i++) {
      V1_0::CdmaInformationRecord* record = &records.infoRec[i];
      RIL_CDMA_InformationRecord* infoRec = &recordsRil->infoRec[i];
      record->name = static_cast<V1_0::CdmaInfoRecName>(infoRec->name);
      // All vectors should be size 0 except one which will be size 1. Set everything to
      // size 0 initially.
      record->display.resize(0);
      record->number.resize(0);
      record->signal.resize(0);
      record->redir.resize(0);
      record->lineCtrl.resize(0);
      record->clir.resize(0);
      record->audioCtrl.resize(0);
      switch (infoRec->name) {
        case RIL_CDMA_DISPLAY_INFO_REC:
        case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC: {
          if (infoRec->rec.display.alpha_len > CDMA_ALPHA_INFO_BUFFER_LENGTH) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: invalid display info response length %d "
                "expected not more than %d",
                (int)infoRec->rec.display.alpha_len, CDMA_ALPHA_INFO_BUFFER_LENGTH);
#endif
            return 0;
          }
          string8 = (char*)malloc((infoRec->rec.display.alpha_len + 1) * sizeof(char));
          if (string8 == NULL) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: Memory allocation failed for "
                "responseCdmaInformationRecords");
#endif
            return 0;
          }
          memcpy(string8, infoRec->rec.display.alpha_buf, infoRec->rec.display.alpha_len);
          string8[(int)infoRec->rec.display.alpha_len] = '\0';

          record->display.resize(1);
          record->display[0].alphaBuf = string8;
          free(string8);
          string8 = NULL;
          break;
        }

        case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
        case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
        case RIL_CDMA_CONNECTED_NUMBER_INFO_REC: {
          if (infoRec->rec.number.len > CDMA_NUMBER_INFO_BUFFER_LENGTH) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: invalid display info response length %d "
                "expected not more than %d",
                (int)infoRec->rec.number.len, CDMA_NUMBER_INFO_BUFFER_LENGTH);
#endif
            return 0;
          }
          string8 = (char*)malloc((infoRec->rec.number.len + 1) * sizeof(char));
          if (string8 == NULL) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: Memory allocation failed for "
                "responseCdmaInformationRecords");
#endif
            return 0;
          }
          memcpy(string8, infoRec->rec.number.buf, infoRec->rec.number.len);
          string8[(int)infoRec->rec.number.len] = '\0';

          record->number.resize(1);
          record->number[0].number = string8;
          free(string8);
          string8 = NULL;
          record->number[0].numberType = infoRec->rec.number.number_type;
          record->number[0].numberPlan = infoRec->rec.number.number_plan;
          record->number[0].pi = infoRec->rec.number.pi;
          record->number[0].si = infoRec->rec.number.si;
          break;
        }

        case RIL_CDMA_SIGNAL_INFO_REC: {
          record->signal.resize(1);
          record->signal[0].isPresent = infoRec->rec.signal.isPresent;
          record->signal[0].signalType = infoRec->rec.signal.signalType;
          record->signal[0].alertPitch = infoRec->rec.signal.alertPitch;
          record->signal[0].signal = infoRec->rec.signal.signal;
          break;
        }

        case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC: {
          if (infoRec->rec.redir.redirectingNumber.len > CDMA_NUMBER_INFO_BUFFER_LENGTH) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: invalid display info response length %d "
                "expected not more than %d\n",
                (int)infoRec->rec.redir.redirectingNumber.len, CDMA_NUMBER_INFO_BUFFER_LENGTH);
#endif
            return 0;
          }
          string8 = (char*)malloc((infoRec->rec.redir.redirectingNumber.len + 1) * sizeof(char));
          if (string8 == NULL) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: Memory allocation failed for "
                "responseCdmaInformationRecords");
#endif
            return 0;
          }
          memcpy(string8, infoRec->rec.redir.redirectingNumber.buf,
                 infoRec->rec.redir.redirectingNumber.len);
          string8[(int)infoRec->rec.redir.redirectingNumber.len] = '\0';

          record->redir.resize(1);
          record->redir[0].redirectingNumber.number = string8;
          free(string8);
          string8 = NULL;
          record->redir[0].redirectingNumber.numberType =
              infoRec->rec.redir.redirectingNumber.number_type;
          record->redir[0].redirectingNumber.numberPlan =
              infoRec->rec.redir.redirectingNumber.number_plan;
          record->redir[0].redirectingNumber.pi = infoRec->rec.redir.redirectingNumber.pi;
          record->redir[0].redirectingNumber.si = infoRec->rec.redir.redirectingNumber.si;
          record->redir[0].redirectingReason =
              static_cast<V1_0::CdmaRedirectingReason>(infoRec->rec.redir.redirectingReason);
          break;
        }

        case RIL_CDMA_LINE_CONTROL_INFO_REC: {
          record->lineCtrl.resize(1);
          record->lineCtrl[0].lineCtrlPolarityIncluded =
              infoRec->rec.lineCtrl.lineCtrlPolarityIncluded;
          record->lineCtrl[0].lineCtrlToggle = infoRec->rec.lineCtrl.lineCtrlToggle;
          record->lineCtrl[0].lineCtrlReverse = infoRec->rec.lineCtrl.lineCtrlReverse;
          record->lineCtrl[0].lineCtrlPowerDenial = infoRec->rec.lineCtrl.lineCtrlPowerDenial;
          break;
        }

        case RIL_CDMA_T53_CLIR_INFO_REC: {
          record->clir.resize(1);
          record->clir[0].cause = infoRec->rec.clir.cause;
          break;
        }

        case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC: {
          record->audioCtrl.resize(1);
          record->audioCtrl[0].upLink = infoRec->rec.audioCtrl.upLink;
          record->audioCtrl[0].downLink = infoRec->rec.audioCtrl.downLink;
          break;
        }

        case RIL_CDMA_T53_RELEASE_INFO_REC:
          // RLOGE("cdmaInfoRecInd: RIL_CDMA_T53_RELEASE_INFO_REC: INVALID");
          return 0;

        default:
          // RLOGE("cdmaInfoRecInd: Incorrect name value");
          return 0;
      }
    }
    return true;
  }
  return false;
}

V1_0::RadioError convertGetIccCardStatusResponse(V1_0::CardStatus& cardStatus,
                                                 std::shared_ptr<RIL_UIM_CardStatus> p_cur) {
  V1_0::RadioError ret = V1_0::RadioError::INVALID_RESPONSE;
  if (p_cur) {
    ret = static_cast<V1_0::RadioError>(p_cur->err);
    if (p_cur->gsm_umts_subscription_app_index >= p_cur->num_applications ||
        p_cur->cdma_subscription_app_index >= p_cur->num_applications ||
        p_cur->ims_subscription_app_index >= p_cur->num_applications) {
      //      RLOGE("Invalid response");
      if (p_cur->err == RIL_UIM_E_SUCCESS) {
        ret = V1_0::RadioError::INVALID_RESPONSE;
      }
    } else {
      cardStatus.cardState = static_cast<V1_0::CardState>(p_cur->card_state);
      cardStatus.universalPinState = static_cast<V1_0::PinState>(p_cur->universal_pin_state);
      cardStatus.gsmUmtsSubscriptionAppIndex = p_cur->gsm_umts_subscription_app_index;
      cardStatus.cdmaSubscriptionAppIndex = p_cur->cdma_subscription_app_index;
      cardStatus.imsSubscriptionAppIndex = p_cur->ims_subscription_app_index;

      RIL_UIM_AppStatus* rilAppStatus = p_cur->applications;
      cardStatus.applications.resize(p_cur->num_applications);
      V1_0::AppStatus* appStatus = cardStatus.applications.data();
#if VDBG
//      RLOGD("num_applications %d", p_cur->num_applications);
#endif
      for (int i = 0; i < p_cur->num_applications; i++) {
        appStatus[i].appType = static_cast<V1_0::AppType>(rilAppStatus[i].app_type);
        appStatus[i].appState = static_cast<V1_0::AppState>(rilAppStatus[i].app_state);
        appStatus[i].persoSubstate =
            static_cast<V1_0::PersoSubstate>(rilAppStatus[i].perso_substate);
        appStatus[i].aidPtr = rilAppStatus[i].aid_ptr;
        appStatus[i].appLabelPtr = rilAppStatus[i].app_label_ptr;
        appStatus[i].pin1Replaced = rilAppStatus[i].pin1_replaced;
        appStatus[i].pin1 = static_cast<V1_0::PinState>(rilAppStatus[i].pin1);
        appStatus[i].pin2 = static_cast<V1_0::PinState>(rilAppStatus[i].pin2);
      }
    }
  }
  return ret;
}

std::unordered_map<rildata::DataCallFailCause_t, V1_0::DataCallFailCause> failCauseMap = {
    {rildata::DataCallFailCause_t::NONE, V1_0::DataCallFailCause::NONE},
    {rildata::DataCallFailCause_t::OPERATOR_BARRED, V1_0::DataCallFailCause::OPERATOR_BARRED},
    {rildata::DataCallFailCause_t::NAS_SIGNALLING, V1_0::DataCallFailCause::NAS_SIGNALLING},
    {rildata::DataCallFailCause_t::INSUFFICIENT_RESOURCES, V1_0::DataCallFailCause::INSUFFICIENT_RESOURCES},
    {rildata::DataCallFailCause_t::MISSING_UKNOWN_APN, V1_0::DataCallFailCause::MISSING_UKNOWN_APN},
    {rildata::DataCallFailCause_t::UNKNOWN_PDP_ADDRESS_TYPE, V1_0::DataCallFailCause::UNKNOWN_PDP_ADDRESS_TYPE},
    {rildata::DataCallFailCause_t::USER_AUTHENTICATION, V1_0::DataCallFailCause::USER_AUTHENTICATION},
    {rildata::DataCallFailCause_t::ACTIVATION_REJECT_GGSN, V1_0::DataCallFailCause::ACTIVATION_REJECT_GGSN},
    {rildata::DataCallFailCause_t::ACTIVATION_REJECT_UNSPECIFIED, V1_0::DataCallFailCause::ACTIVATION_REJECT_UNSPECIFIED},
    {rildata::DataCallFailCause_t::SERVICE_OPTION_NOT_SUPPORTED, V1_0::DataCallFailCause::SERVICE_OPTION_NOT_SUPPORTED},
    {rildata::DataCallFailCause_t::SERVICE_OPTION_NOT_SUBSCRIBED, V1_0::DataCallFailCause::SERVICE_OPTION_NOT_SUBSCRIBED},
    {rildata::DataCallFailCause_t::SERVICE_OPTION_OUT_OF_ORDER, V1_0::DataCallFailCause::SERVICE_OPTION_OUT_OF_ORDER},
    {rildata::DataCallFailCause_t::NSAPI_IN_USE, V1_0::DataCallFailCause::NSAPI_IN_USE},
    {rildata::DataCallFailCause_t::REGULAR_DEACTIVATION, V1_0::DataCallFailCause::REGULAR_DEACTIVATION},
    {rildata::DataCallFailCause_t::QOS_NOT_ACCEPTED, V1_0::DataCallFailCause::QOS_NOT_ACCEPTED},
    {rildata::DataCallFailCause_t::NETWORK_FAILURE, V1_0::DataCallFailCause::NETWORK_FAILURE},
    {rildata::DataCallFailCause_t::UMTS_REACTIVATION_REQ, V1_0::DataCallFailCause::UMTS_REACTIVATION_REQ},
    {rildata::DataCallFailCause_t::FEATURE_NOT_SUPP, V1_0::DataCallFailCause::FEATURE_NOT_SUPP},
    {rildata::DataCallFailCause_t::TFT_SEMANTIC_ERROR, V1_0::DataCallFailCause::TFT_SEMANTIC_ERROR},
    {rildata::DataCallFailCause_t::TFT_SYTAX_ERROR, V1_0::DataCallFailCause::TFT_SYTAX_ERROR},
    {rildata::DataCallFailCause_t::UNKNOWN_PDP_CONTEXT, V1_0::DataCallFailCause::UNKNOWN_PDP_CONTEXT},
    {rildata::DataCallFailCause_t::FILTER_SEMANTIC_ERROR, V1_0::DataCallFailCause::FILTER_SEMANTIC_ERROR},
    {rildata::DataCallFailCause_t::FILTER_SYTAX_ERROR, V1_0::DataCallFailCause::FILTER_SYTAX_ERROR},
    {rildata::DataCallFailCause_t::PDP_WITHOUT_ACTIVE_TFT, V1_0::DataCallFailCause::PDP_WITHOUT_ACTIVE_TFT},
    {rildata::DataCallFailCause_t::ONLY_IPV4_ALLOWED, V1_0::DataCallFailCause::ONLY_IPV4_ALLOWED},
    {rildata::DataCallFailCause_t::ONLY_IPV6_ALLOWED, V1_0::DataCallFailCause::ONLY_IPV6_ALLOWED},
    {rildata::DataCallFailCause_t::ONLY_SINGLE_BEARER_ALLOWED, V1_0::DataCallFailCause::ONLY_SINGLE_BEARER_ALLOWED},
    {rildata::DataCallFailCause_t::ESM_INFO_NOT_RECEIVED, V1_0::DataCallFailCause::ESM_INFO_NOT_RECEIVED},
    {rildata::DataCallFailCause_t::PDN_CONN_DOES_NOT_EXIST, V1_0::DataCallFailCause::PDN_CONN_DOES_NOT_EXIST},
    {rildata::DataCallFailCause_t::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED, V1_0::DataCallFailCause::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED},
    {rildata::DataCallFailCause_t::MAX_ACTIVE_PDP_CONTEXT_REACHED, V1_0::DataCallFailCause::MAX_ACTIVE_PDP_CONTEXT_REACHED},
    {rildata::DataCallFailCause_t::UNSUPPORTED_APN_IN_CURRENT_PLMN, V1_0::DataCallFailCause::UNSUPPORTED_APN_IN_CURRENT_PLMN},
    {rildata::DataCallFailCause_t::MESSAGE_INCORRECT_SEMANTIC, V1_0::DataCallFailCause::MESSAGE_INCORRECT_SEMANTIC},
    {rildata::DataCallFailCause_t::INVALID_MANDATORY_INFO, V1_0::DataCallFailCause::INVALID_MANDATORY_INFO},
    {rildata::DataCallFailCause_t::MESSAGE_TYPE_UNSUPPORTED, V1_0::DataCallFailCause::MESSAGE_TYPE_UNSUPPORTED},
    {rildata::DataCallFailCause_t::MSG_TYPE_NONCOMPATIBLE_STATE, V1_0::DataCallFailCause::MSG_TYPE_NONCOMPATIBLE_STATE},
    {rildata::DataCallFailCause_t::UNKNOWN_INFO_ELEMENT, V1_0::DataCallFailCause::UNKNOWN_INFO_ELEMENT},
    {rildata::DataCallFailCause_t::CONDITIONAL_IE_ERROR, V1_0::DataCallFailCause::CONDITIONAL_IE_ERROR},
    {rildata::DataCallFailCause_t::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE, V1_0::DataCallFailCause::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE},
    {rildata::DataCallFailCause_t::PROTOCOL_ERRORS, V1_0::DataCallFailCause::PROTOCOL_ERRORS},
    {rildata::DataCallFailCause_t::APN_TYPE_CONFLICT, V1_0::DataCallFailCause::APN_TYPE_CONFLICT},
    {rildata::DataCallFailCause_t::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN, V1_0::DataCallFailCause::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN},
    {rildata::DataCallFailCause_t::EMM_ACCESS_BARRED, V1_0::DataCallFailCause::EMM_ACCESS_BARRED},
    {rildata::DataCallFailCause_t::EMERGENCY_IFACE_ONLY, V1_0::DataCallFailCause::EMERGENCY_IFACE_ONLY},
    {rildata::DataCallFailCause_t::IFACE_MISMATCH, V1_0::DataCallFailCause::IFACE_MISMATCH},
    {rildata::DataCallFailCause_t::COMPANION_IFACE_IN_USE, V1_0::DataCallFailCause::COMPANION_IFACE_IN_USE},
    {rildata::DataCallFailCause_t::IP_ADDRESS_MISMATCH, V1_0::DataCallFailCause::IP_ADDRESS_MISMATCH},
    {rildata::DataCallFailCause_t::IFACE_AND_POL_FAMILY_MISMATCH, V1_0::DataCallFailCause::IFACE_AND_POL_FAMILY_MISMATCH},
    {rildata::DataCallFailCause_t::EMM_ACCESS_BARRED_INFINITE_RETRY, V1_0::DataCallFailCause::EMM_ACCESS_BARRED_INFINITE_RETRY},
    {rildata::DataCallFailCause_t::AUTH_FAILURE_ON_EMERGENCY_CALL, V1_0::DataCallFailCause::AUTH_FAILURE_ON_EMERGENCY_CALL},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_1, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_1},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_2, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_2},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_3, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_3},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_4, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_4},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_5, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_5},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_6, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_6},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_7, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_7},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_8, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_8},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_9, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_9},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_10, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_10},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_11, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_11},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_12, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_12},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_13, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_13},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_14, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_14},
    {rildata::DataCallFailCause_t::OEM_DCFAILCAUSE_15, V1_0::DataCallFailCause::OEM_DCFAILCAUSE_15},
    {rildata::DataCallFailCause_t::VOICE_REGISTRATION_FAIL, V1_0::DataCallFailCause::VOICE_REGISTRATION_FAIL},
    {rildata::DataCallFailCause_t::DATA_REGISTRATION_FAIL, V1_0::DataCallFailCause::DATA_REGISTRATION_FAIL},
    {rildata::DataCallFailCause_t::SIGNAL_LOST, V1_0::DataCallFailCause::SIGNAL_LOST},
    {rildata::DataCallFailCause_t::PREF_RADIO_TECH_CHANGED, V1_0::DataCallFailCause::PREF_RADIO_TECH_CHANGED},
    {rildata::DataCallFailCause_t::RADIO_POWER_OFF, V1_0::DataCallFailCause::RADIO_POWER_OFF},
    {rildata::DataCallFailCause_t::TETHERED_CALL_ACTIVE, V1_0::DataCallFailCause::TETHERED_CALL_ACTIVE},
    {rildata::DataCallFailCause_t::ERROR_UNSPECIFIED, V1_0::DataCallFailCause::ERROR_UNSPECIFIED}
};


V1_0::DataCallFailCause convertDcFailStatusToHidlDcFailCause(const rildata::DataCallFailCause_t &cause) {
    V1_0::DataCallFailCause hidlCause = V1_0::DataCallFailCause::ERROR_UNSPECIFIED;
    if (failCauseMap.find(cause) != failCauseMap.end()) {
        hidlCause = failCauseMap[cause];
    }
    return hidlCause;
}

void convertRilDataCallToHal(const rildata::DataCallResult_t entry,
                        V1_0::SetupDataCallResult& dcResult) {
    dcResult.status = convertDcFailStatusToHidlDcFailCause(entry.cause);
    dcResult.suggestedRetryTime = entry.suggestedRetryTime;
    dcResult.cid = entry.cid;
    dcResult.active = entry.active;
    dcResult.type = entry.type;
    dcResult.ifname = entry.ifname;
    dcResult.addresses = entry.addresses;
    dcResult.dnses = entry.dnses;
    dcResult.gateways = entry.gateways;
    dcResult.pcscf = entry.pcscf;
    dcResult.mtu = entry.mtu;
}

rildata::DataProfileInfo_t convertHidlDataProfileInfoToRil_1_0(const V1_0::DataProfileInfo& profile) {
    rildata::DataProfileInfo_t rilProfile = {};
    rilProfile.profileId = (rildata::DataProfileId_t)profile.profileId;
    rilProfile.apn = profile.apn;
    rilProfile.protocol = profile.protocol;
    rilProfile.roamingProtocol = profile.roamingProtocol;
    rilProfile.authType = (rildata::ApnAuthType_t)profile.authType;
    rilProfile.username = profile.user;
    rilProfile.password = profile.password;
    rilProfile.dataProfileInfoType = (rildata::DataProfileInfoType_t)profile.type,
    rilProfile.maxConnsTime = profile.maxConnsTime;
    rilProfile.maxConns = profile.maxConns;
    rilProfile.waitTime = profile.waitTime;
    rilProfile.enableProfile = profile.enabled;
    rilProfile.supportedApnTypesBitmap = (rildata::ApnTypes_t)profile.supportedApnTypesBitmap;
    rilProfile.bearerBitmap = (rildata::RadioAccessFamily_t)profile.bearerBitmap;
    rilProfile.mtu = profile.mtu;
    rilProfile.preferred = false;
    rilProfile.persistent = false;
    return rilProfile;
}

rildata::AccessNetwork_t convertRadioTechToAccessNetwork(V1_0::RadioTechnology tech)
{
    rildata::AccessNetwork_t accnet = rildata::AccessNetwork_t::UNKNOWN;
    using namespace V1_0;
    switch (tech) {
        case RadioTechnology::GPRS:
        case RadioTechnology::EDGE:
        case RadioTechnology::GSM:
            accnet = rildata::AccessNetwork_t::GERAN;
            break;
        case RadioTechnology::UMTS:
        case RadioTechnology::HSDPA:
        case RadioTechnology::HSUPA:
        case RadioTechnology::HSPA:
        case RadioTechnology::HSPAP:
        case RadioTechnology::TD_SCDMA:
            accnet = rildata::AccessNetwork_t::UTRAN;
            break;
        case RadioTechnology::IS95A:
        case RadioTechnology::IS95B:
        case RadioTechnology::ONE_X_RTT:
        case RadioTechnology::EVDO_0:
        case RadioTechnology::EVDO_A:
        case RadioTechnology::EVDO_B:
        case RadioTechnology::EHRPD:
            accnet = rildata::AccessNetwork_t::CDMA2000;
            break;
        case RadioTechnology::LTE:
        case RadioTechnology::LTE_CA:
            accnet = rildata::AccessNetwork_t::EUTRAN;
            break;
        case RadioTechnology::IWLAN:
            accnet = rildata::AccessNetwork_t::IWLAN;
            break;
        case RadioTechnology::UNKNOWN:
        default:
            Log::getInstance().d("Unknown AccessNetwork ran = "+std::to_string((int)accnet));
            break;
    }
    return accnet;
}

}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android
