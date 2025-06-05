/******************************************************************************
#  Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RADIO_SERVICE_UTILS_1_0_H__
#define __RADIO_SERVICE_UTILS_1_0_H__

#include "android/hardware/radio/1.0/types.h"
#include "telephony/ril.h"
#include "framework/Message.h"

#include "interfaces/voice/voice.h"
#include "interfaces/dms/dms_types.h"
#include "interfaces/nas/nas_types.h"
#include "interfaces/sms/qcril_qmi_sms_types.h"
#include "interfaces/uim/qcril_uim_types.h"
#include "interfaces/sms/RilUnsolIncoming3Gpp2SMSMessage.h"
#include "interfaces/voice/QcRilUnsolSupplementaryServiceMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaCallWaitingMessage.h"
#include "interfaces/voice/QcRilUnsolSuppSvcNotificationMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaInfoRecordMessage.h"
#include "MessageCommon.h"

#include "qcril_config.h"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

hidl_string convertCharPtrToHidlString(const char* ptr);
V1_0::RadioError convertMsgToRadioError(Message::Callback::Status status, RIL_Errno e);

int convertToHal(V1_0::WcdmaSignalStrength& out, const RIL_WCDMA_SignalStrength& in);
int convertToHal(V1_0::GsmSignalStrength& out, const RIL_GW_SignalStrength& in);
int convertToHal(V1_0::GsmSignalStrength& out, const RIL_WCDMA_SignalStrength& in);
int convertToHal(V1_0::WcdmaSignalStrength& out, const RIL_GW_SignalStrength& in);
int convertToHal(V1_0::CdmaSignalStrength& out, const RIL_CDMA_SignalStrength& in);
int convertToHal(V1_0::EvdoSignalStrength& out, const RIL_EVDO_SignalStrength& in);
int convertToHal(V1_0::LteSignalStrength& out, RIL_LTE_SignalStrength_v8 in);
int convertToHal(V1_0::TdScdmaSignalStrength& out, const RIL_TD_SCDMA_SignalStrength& in);
int convertRilSignalStrengthToHal(V1_0::SignalStrength& signalStrength,
                                  const RIL_SignalStrength& rilSignalStrength);
void convertRilNetworkResultToHidl(const std::vector<qcril::interfaces::NetworkInfo>& rilData,
                                   hidl_vec<V1_0::OperatorInfo>& hidlData);
int convertNwTypeToRadioAccessFamily(V1_0::PreferredNetworkType nwType);
V1_0::PreferredNetworkType convertRadioAccessFamilyToNwType(int radioAccessFamily);
void convertRilNeighboringCidResultToHidl(
    const std::vector<qcril::interfaces::RilNeighboringCell_t>& rilCell,
    hidl_vec<V1_0::NeighboringCell>& hidlCell);
void convertRilRadioCapabilityToHal(V1_0::RadioCapability& hidlRc, const RIL_RadioCapability& rilRc);

template <class T>
void __attribute__((noinline)) fillCellIdentityGsm(T& out, const RIL_CellIdentityGsm_v12& in);
template <class T>
void __attribute__((noinline)) fillCellIdentityWcdma(T& out, const RIL_CellIdentityWcdma_v12& in);
template <class T>
void __attribute__((noinline)) fillCellIdentityCdma(T& out, const RIL_CellIdentityCdma& in);
template <class T>
void __attribute__((noinline)) fillCellIdentityLte(T& out, const RIL_CellIdentityLte_v12& in);
template <class T>
void __attribute__((noinline)) fillCellIdentityTdscdma(T& out, const RIL_CellIdentityTdscdma& in);

template <class T>
void fillCellIdentityGsm(T& out, const RIL_CellIdentityGsm_v12& in) {
  out.mcc = in.mcc;
  out.mnc = in.mnc;
  out.lac = in.lac;
  out.cid = in.cid;
  out.arfcn = in.arfcn;
  out.bsic = in.bsic;
}

template <class T>
void fillCellIdentityWcdma(T& out, const RIL_CellIdentityWcdma_v12& in) {
  out.mcc = in.mcc;
  out.mnc = in.mnc;
  out.lac = in.lac;
  out.cid = in.cid;
  out.psc = in.psc;
  out.uarfcn = in.uarfcn;
}

template <class T>
void fillCellIdentityCdma(T& out, const RIL_CellIdentityCdma& in) {
  out.networkId = in.networkId;
  out.systemId = in.systemId;
  out.baseStationId = in.basestationId;
  out.longitude = in.longitude;
  out.latitude = in.latitude;
}

template <class T>
void fillCellIdentityLte(T& out, const RIL_CellIdentityLte_v12& in) {
  out.mcc = in.mcc;
  out.mnc = in.mnc;
  out.ci = in.ci;
  out.pci = in.pci;
  out.tac = in.tac;
  out.earfcn = in.earfcn;
}

template <class T>
void fillCellIdentityTdscdma(T& out, const RIL_CellIdentityTdscdma& in) {
  out.mcc = in.mcc;
  out.mnc = in.mnc;
  out.lac = in.lac;
  out.cid = in.cid;
  out.cpid = in.cpid;
}

template <class T>
void fillCellIdentityResponse(T& cellIdentity, const RIL_CellIdentity_v16& rilCellIdentity) {
  cellIdentity.cellIdentityGsm.resize(0);
  cellIdentity.cellIdentityWcdma.resize(0);
  cellIdentity.cellIdentityCdma.resize(0);
  cellIdentity.cellIdentityTdscdma.resize(0);
  cellIdentity.cellIdentityLte.resize(0);
  cellIdentity.cellInfoType = static_cast<V1_0::CellInfoType>(rilCellIdentity.cellInfoType);
  switch (rilCellIdentity.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM: {
      cellIdentity.cellIdentityGsm.resize(1);
      fillCellIdentityGsm(cellIdentity.cellIdentityGsm[0], rilCellIdentity.cellIdentityGsm);
      break;
    }

    case RIL_CELL_INFO_TYPE_WCDMA: {
      cellIdentity.cellIdentityWcdma.resize(1);
      fillCellIdentityWcdma(cellIdentity.cellIdentityWcdma[0], rilCellIdentity.cellIdentityWcdma);
      break;
    }

    case RIL_CELL_INFO_TYPE_CDMA: {
      cellIdentity.cellIdentityCdma.resize(1);
      fillCellIdentityCdma(cellIdentity.cellIdentityCdma[0], rilCellIdentity.cellIdentityCdma);
      break;
    }

    case RIL_CELL_INFO_TYPE_LTE: {
      cellIdentity.cellIdentityLte.resize(1);
      fillCellIdentityLte(cellIdentity.cellIdentityLte[0], rilCellIdentity.cellIdentityLte);
      break;
    }

    case RIL_CELL_INFO_TYPE_TD_SCDMA: {
      cellIdentity.cellIdentityTdscdma.resize(1);
      fillCellIdentityTdscdma(cellIdentity.cellIdentityTdscdma[0],
                              rilCellIdentity.cellIdentityTdscdma);
      break;
    }

    default: {
      break;
    }
  }
}

template <class T>
int fillVoiceRegistrationStateResponse(T& voiceRegResponse,
                                       const RIL_VoiceRegistrationStateResponse& voiceRegState) {
  voiceRegResponse.regState = static_cast<V1_0::RegState>(voiceRegState.regState);
  voiceRegResponse.rat = voiceRegState.rat;
  ;
  voiceRegResponse.cssSupported = voiceRegState.cssSupported;
  voiceRegResponse.roamingIndicator = voiceRegState.roamingIndicator;
  voiceRegResponse.systemIsInPrl = voiceRegState.systemIsInPrl;
  voiceRegResponse.defaultRoamingIndicator = voiceRegState.defaultRoamingIndicator;
  voiceRegResponse.reasonForDenial = voiceRegState.reasonForDenial;
  fillCellIdentityResponse(voiceRegResponse.cellIdentity, voiceRegState.cellIdentity);
  return 0;
}

template <class T>
void fillDataRegistrationStateResponse(T& out, const RIL_DataRegistrationStateResponse& in) {
  out.regState = static_cast<V1_0::RegState>(in.regState);
  out.rat = in.rat;
  out.reasonDataDenied = in.reasonDataDenied;
  out.maxDataCalls = in.maxDataCalls;
  fillCellIdentityResponse(out.cellIdentity, in.cellIdentity);
}
void convertRilCellInfoListToHal(const std::vector<RIL_CellInfo_v12>& rillCellInfo,
                                 hidl_vec<V1_0::CellInfo>& records);

int convertToHidl(V1_0::Call& out, const qcril::interfaces::CallInfo& in);
V1_0::CallPresentation convertCallPresentation(uint32_t in);
V1_0::CallState convertCallState(qcril::interfaces::CallState in);
V1_0::UssdModeType convertUssdModeType(qcril::interfaces::UssdModeType in);
V1_0::UusType convertUssTypeToHal(RIL_UUS_Type in);
RIL_UUS_Type convertUssTypeFromHal(V1_0::UusType in);
V1_0::UusDcs convertUusDcsTypeToHal(RIL_UUS_DCS in);
RIL_UUS_DCS convertUusDcsTypeFromHal(V1_0::UusDcs in);
void convertRilCdmaSignalInfoRecordToHal(V1_0::CdmaSignalInfoRecord& out,
                                         std::shared_ptr<RIL_CDMA_SignalInfoRecord> in);
void convertRilCdmaSignalInfoRecord(V1_0::CdmaSignalInfoRecord& out,
                                    std::shared_ptr<RIL_CDMA_SignalInfoRecord> in);
V1_0::CallForwardInfoStatus convertCallForwardInfoStatus(int in);
int convertCallForwardInfoStatus(V1_0::CallForwardInfoStatus in);
qcril::interfaces::FacilityType convertFacilityType(std::string in);
bool isUssdOverImsSupported();

V1_0::SendSmsResult makeSendSmsResult(std::shared_ptr<RilSendSmsResult_t> in);
void constructCdmaSms(RIL_CDMA_SMS_Message& rcsm, const V1_0::CdmaSmsMessage& sms);
bool makeCdmaSmsMessage(V1_0::CdmaSmsMessage& msg,
                        std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> rilMsg);
bool makeSuppSvcNotification(V1_0::SuppSvcNotification& suppSvc,
                             std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg);
bool makeCdmaCallWaiting(V1_0::CdmaCallWaiting& callWaitingRecord,
                         std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg);
bool makeStkCcUnsolSsResult(V1_0::StkCcUnsolSsResult& ss,
                            std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg);
bool makeCdmaInformationRecords(V1_0::CdmaInformationRecords& records,
                                std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg);

V1_0::RadioError convertGetIccCardStatusResponse(V1_0::CardStatus& cardStatus,
                                                 std::shared_ptr<RIL_UIM_CardStatus> p_cur);
V1_0::DataCallFailCause convertDcFailStatusToHidlDcFailCause(const rildata::DataCallFailCause_t &cause);
void convertRilDataCallToHal(const rildata::DataCallResult_t resPonse,
                        V1_0::SetupDataCallResult& dcResult);
rildata::DataProfileInfo_t convertHidlDataProfileInfoToRil_1_0(const V1_0::DataProfileInfo& profile);
RIL_RadioTechnologyFamily convertHidlRadioTechToRil(const V1_0::RadioTechnologyFamily tech);
rildata::AccessNetwork_t convertRadioTechToAccessNetwork(V1_0::RadioTechnology tech);
}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_UTILS_1_0_H__
