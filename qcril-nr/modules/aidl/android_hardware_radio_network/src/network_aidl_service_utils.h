/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "network_aidl_service.h"
#include "aidl/android/hardware/radio/RadioTechnology.h"

#include <aidl/android/hardware/radio/network/CellInfo.h>
#include <aidl/android/hardware/radio/network/SignalStrength.h>
#include <aidl/android/hardware/radio/network/RegStateResult.h>
#include <aidl/android/hardware/radio/network/BarringInfo.h>

#include "aidl/android/hardware/radio/RadioError.h"
#include "aidl/android/hardware/radio/network/GeranBands.h"
#include "aidl/android/hardware/radio/network/RadioAccessSpecifier.h"

#include "interfaces/nas/nas_types.h"
#include "interfaces/sms/qcril_qmi_sms_types.h"
#include "telephony/ril.h"
#include "MessageCommon.h"

namespace qti {
namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace network {
namespace utils {

aidlnetwork::SuppSvcNotification convertRILSuppSvcNotificationToAidl(
    std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage>);

aidlnetwork::OperatorInfo convertRilNetworkInfotoAidl(
    const qcril::interfaces::NetworkInfo&);

qcril::interfaces::FacilityType convertFacilityType(
    const std::string&);

RIL_RadioAccessNetworks convertAidlRadioAccessNetworkToRilRadioAccessNetwork(
    aidlradio::AccessNetwork);

RIL_Errno convertAidlRasToRilRas(RIL_RadioAccessSpecifier&,
    const aidlnetwork::RadioAccessSpecifier&);

RIL_Errno rasArgsCheck(const std::vector<aidlnetwork::RadioAccessSpecifier>&);

RIL_Errno fillSetSystemSelectionChannelRequest(const std::vector<aidlnetwork::RadioAccessSpecifier>&,
  RIL_SysSelChannels&);

RIL_Errno fillNetworkScanRequest(const aidlnetwork::NetworkScanRequest&,
    RIL_NetworkScanRequest&);

template <typename T>
bool getAidlPlmnFromCellIdentity(std::string& mcc, std::string& mnc, const T& cellIdentity);

template <typename T>
void convert(const T& rilCellIdentity, aidlnetwork::OperatorInfo& aidlOperatorInfo);

void convert(const RIL_CellIdentityGsm_v12& rilGsmCellIdentity,
    aidlnetwork::CellIdentityGsm& aidlGsmCellIdentity);

void convert(const RIL_GSM_SignalStrength_v12& rilGsmSignalStrength,
    aidlnetwork::GsmSignalStrength& aidlGsmSignalStrength);

void convert(const RIL_CellInfoGsm_v12& rilGsmCellInfo,
    aidlnetwork::CellInfoGsm& aidlGsmCellInfo);

void convert(const RIL_CellIdentityWcdma_v12& rilWcdmaCellIdentity,
    aidlnetwork::CellIdentityWcdma& aidlWcdmaCellIdentity);

void convert(const RIL_SignalStrengthWcdma& rilWcdmaSignalStrength,
    aidlnetwork::WcdmaSignalStrength& aidlWcdmaSignalStrength);

void convert(const RIL_CellInfoWcdma_v12& rilWcdmaCellInfo,
    aidlnetwork::CellInfoWcdma& aidlWcdmaCellInfo);

void convert(const RIL_CellIdentityCdma& rilCdmaCellIdentity,
    aidlnetwork::CellIdentityCdma& aidlCdmaCellIdentity);

void convert(const RIL_CDMA_SignalStrength& rilCdmaSignalStrength,
    aidlnetwork::CdmaSignalStrength& aidlCdmaSignalStrength);

void convert(const RIL_EVDO_SignalStrength& rilEvdoSignalStrength,
    aidlnetwork::EvdoSignalStrength& aidlEvdoSignalStrength);

void convert(const RIL_CellInfoCdma& rilCdmaCellInfo,
    aidlnetwork::CellInfoCdma& aidlCdmaCellInfo);

aidlnetwork::EutranBands mapEarfcnToEutranBand(int earfcn);

void convert(const RIL_CellIdentityLte_v12& rilLteCellIdentity,
    aidlnetwork::CellIdentityLte& aidlLteCellIdentity);

void convert(const RIL_LTE_SignalStrength_v8& rilLteSignalStrength,
    aidlnetwork::LteSignalStrength& aidlLteSignalStrength);

void convert(const RIL_CellInfoLte_v12& rilLteCellInfo,
    aidlnetwork::CellInfoLte& aidlLteCellInfo);

void convert(const RIL_CellIdentityNr& rilNrCellIdentity,
    aidlnetwork::CellIdentityNr& aidlNrCellIdentity);

void convert(const RIL_NR_SignalStrength& rilNrSignalStrength,
    aidlnetwork::NrSignalStrength& aidlNrSignalStrength);

void convert(const RIL_CellInfoNr& rilNrCellInfo,
    aidlnetwork::CellInfoNr& aidlNrCellInfo);

void convert(const RIL_CellIdentityTdscdma& rilTdscdmaCellIdentity,
    aidlnetwork::CellIdentityTdscdma& aidlTdscdmaCellIdentity);

void convert(const RIL_TD_SCDMA_SignalStrength& rilTdscdmaSignalStrength,
    aidlnetwork::TdscdmaSignalStrength& aidlTdscdmaSignalStrength);

void convert(const RIL_CellInfoTdscdma& rilTdscdmaCellInfo,
    aidlnetwork::CellInfoTdscdma& aidlTdscdmaCellInfo);

void convert(const RIL_CellInfo_v12& rilCellInfo, aidlnetwork::CellInfo& aidlCellInfo);

void convert(const std::vector<RIL_CellInfo_v12>& rilCellInfos,
    std::vector<aidlnetwork::CellInfo>& aidlCellInfos);

void convert(const RIL_GW_SignalStrength& rilGsmSignalStrength,
    aidlnetwork::GsmSignalStrength& aidlGsmSignalStrength);

void convert(const RIL_WCDMA_SignalStrength& rilWcdmaSignalStrength,
    aidlnetwork::WcdmaSignalStrength& aidlWcdmaSignalStrength);

void convert(const RIL_SignalStrength& rilSignalStrength,
    aidlnetwork::SignalStrength& aidlSignalStrength);

void convert(const RIL_CellIdentity_v16& rilCellIdentity,
    aidlnetwork::CellIdentity& aidlCellIdentity);

std::string getRegisteredPlmnFromCellIdentity(const RIL_CellIdentity_v16& cellIdentity);

void convert(const NrVopsInfo& rilNrVopsInfo, aidlnetwork::NrVopsInfo& aidlNrVopsInfo);

void convert(const RIL_DataRegistrationStateResponse& rilDataRegState,
    aidlnetwork::RegStateResult& aidlRegState);

void convert(const RIL_VoiceRegistrationStateResponse& rilVoiceRegState,
    aidlnetwork::RegStateResult& aidlRegState);

void convert(const qcril::interfaces::RilBarringServiceType& rilBarringServiceType,
    int32_t& aidlBarringServiceType);

void convert(const qcril::interfaces::RilBarringType& rilBarringType,
    int32_t& aidlBarringType);

void convert (const qcril::interfaces::RILBarringInfo& rilBarringInfo,
    aidlnetwork::BarringInfo& aidlBarringInfo);

void convert(const std::vector<qcril::interfaces::RILBarringInfo>& rilBarringInfos,
    std::vector<aidlnetwork::BarringInfo>& aidlBarringInfos);

bool checkThresholdAndHysteresis(int32_t, int32_t,
    const std::vector<int32_t>&);

RIL_Errno sanityCheckSignalStrengthCriteriaParams(
    const std::vector<aidlnetwork::SignalThresholdInfo>&);

RIL_SignalMeasureType convertAidlSigMTypetoRilSigMType(int32_t);

std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> fillInSignalStrengthCriteria(
    const std::vector<aidlnetwork::SignalThresholdInfo>&);
RIL_Errno fillSetSystemSelectionChannelRequest(
  const std::vector<aidlnetwork::RadioAccessSpecifier>&, RIL_SysSelChannels&);
RIL_Errno fillNetworkScanRequest(const aidlnetwork::NetworkScanRequest&,
    RIL_NetworkScanRequest&);
RIL_Errno convertAidlUsageSettingtoRil(const aidlnetwork::UsageSetting&,
    RIL_UsageSettingMode&);
RIL_Errno convertRilUsageSettingtoAidlUsageSetting(
  const RIL_UsageSettingMode& rilMode, aidlnetwork::UsageSetting& aidlMode);

aidlnetwork::GeranBands convertToAidl(RIL_GeranBands in);

RIL_Errno convertRadioAccessSpecifierToAidl(aidlnetwork::RadioAccessSpecifier& out,
                                            const RIL_RadioAccessSpecifier& in);

RIL_Errno convertRadioAccessSpecifiersToAidl(std::vector<aidlnetwork::RadioAccessSpecifier>& out,
                                             const RIL_SysSelChannels& in);

rildata::AccessNetwork_t convertHidlAccessNetworkToDataAccessNetwork(aidlradio::AccessNetwork ran);

RIL_Errno convertLcResultToRilError(rildata::LinkCapCriteriaResult_t result);

aidlradio::RadioError convertMsgToRadioError(Message::Callback::Status status, RIL_Errno e);
aidlradio::RadioTechnology convertRadioAccessFamilyTypeToRadioTechnology(
    rildata::RadioAccessFamily_t accessFamily);
RIL_RadioTechnology getRilRadioTechnologyFromRan(aidlradio::AccessNetwork ran);
}  // namespace utils
}  // namespace network
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
}  // namespace qti

