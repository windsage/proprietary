/******************************************************************************
#  Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RADIO_SERVICE_UTILS_1_2_H__
#define __RADIO_SERVICE_UTILS_1_2_H__

#include "android/hardware/radio/1.2/types.h"
#include "telephony/ril.h"
#include "MessageCommon.h"
#include "interfaces/nas/NasSetSignalStrengthCriteria.h"

#include "interfaces/voice/voice.h"
#include "interfaces/nas/nas_types.h"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

bool checkThresholdAndHysteresis(int32_t hysteresisMs, int32_t hysteresisDb,
                                 const hidl_vec<int32_t>& thresholdsDbm);
RIL_Errno sanityCheckSignalStrengthCriteriaParams(int32_t hysteresisMs, int32_t hysteresisDb,
                                                  const hidl_vec<int32_t>& thresholdsDbm,
                                                  V1_2::AccessNetwork ran);
RIL_RadioAccessNetworks convertHidlAccessNetworkToRilAccessNetwork(V1_2::AccessNetwork ran);
void fillInSignalStrengthCriteria(std::vector<qcril::interfaces::SignalStrengthCriteriaEntry>& out,
                                  int32_t hysteresisMs, int32_t hysteresisDb,
                                  const hidl_vec<int32_t>& thresholdsDbm, V1_2::AccessNetwork ran);

RIL_Errno fillNetworkScanRequest_1_2(const V1_2::NetworkScanRequest& request,
                                     RIL_NetworkScanRequest& scanRequest);

int convertToHidl(V1_2::AudioQuality& aq, enum RIL_AudioQuality rilAudioQuality);

int convertToHidl(V1_2::Call& out, const qcril::interfaces::CallInfo& in);

int convertRilSignalStrengthToHal(V1_2::SignalStrength& signalStrength,
                                  const RIL_SignalStrength& rilSignalStrength);
void fillCellIdentityGsm(V1_2::CellIdentityGsm& out, const RIL_CellIdentityGsm_v12& in);
void fillCellIdentityWcdma(V1_2::CellIdentityWcdma& out, const RIL_CellIdentityWcdma_v12& in);
void fillCellIdentityCdma(V1_2::CellIdentityCdma& out, const RIL_CellIdentityCdma& in);
void fillCellIdentityLte(V1_2::CellIdentityLte& out, const RIL_CellIdentityLte_v12& in);
void fillCellIdentityTdscdma(V1_2::CellIdentityTdscdma& out, const RIL_CellIdentityTdscdma& in);
void fillCellIdentityResponse(V1_2::CellIdentity& cellIdentity,
                              const RIL_CellIdentity_v16& rilCellIdentity);
int fillVoiceRegistrationStateResponse(V1_2::VoiceRegStateResult& voiceRegResponse,
                                       const RIL_VoiceRegistrationStateResponse& voiceRegState);
void fillDataRegistrationStateResponse(V1_2::DataRegStateResult& out,
                                       const RIL_DataRegistrationStateResponse& in);
void convertRilCellInfoToHal_1_2(V1_2::CellInfo& hidlCellInfo, const RIL_CellInfo_v12& rilCellInfo);
void convertRilCellInfoListToHal_1_2(const std::vector<RIL_CellInfo_v12>& rillCellInfo,
                                     hidl_vec<V1_2::CellInfo>& records);

void convertToHal(V1_2::TdscdmaSignalStrength& out, const RIL_TD_SCDMA_SignalStrength& in);
int convertToHal(V1_2::WcdmaSignalStrength& out, const RIL_WCDMA_SignalStrength& in);
rildata::AccessNetwork_t convertHidlAccessNetworkToDataAccessNetwork(V1_2::AccessNetwork ran);
RIL_Errno convertLcResultToRilError(rildata::LinkCapCriteriaResult_t result);
}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_UTILS_1_2_H__
