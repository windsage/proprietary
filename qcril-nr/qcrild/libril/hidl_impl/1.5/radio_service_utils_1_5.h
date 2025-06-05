/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RADIO_SERVICE_UTILS_1_5_H__
#define __RADIO_SERVICE_UTILS_1_5_H__

#include "android/hardware/radio/1.5/types.h"
#include "telephony/ril.h"
#include "framework/Message.h"
#include "MessageCommon.h"
#include "interfaces/voice/voice.h"
#include "interfaces/nas/nas_types.h"
#include "interfaces/uim/qcril_uim_types.h"

#include "interfaces/nas/NasSetSignalStrengthCriteria.h"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

RIL_Errno fillSetSystemSelectionChannelRequest_1_5(
    const hidl_vec<V1_5::RadioAccessSpecifier>& specifiers, RIL_SysSelChannels& request);
RIL_Errno fillNetworkScanRequest_1_5(const V1_5::NetworkScanRequest& request,
                                     RIL_NetworkScanRequest& scanRequest);
RIL_Errno sanityCheckSignalStrengthCriteriaParams(const V1_5::SignalThresholdInfo& threshold,
                                                  V1_5::AccessNetwork ran);
void fillInSignalStrengthCriteria(std::vector<qcril::interfaces::SignalStrengthCriteriaEntry>& out,
                                  const V1_5::SignalThresholdInfo& signalThresholdInfo,
                                  V1_5::AccessNetwork ran, bool& disableAllTypes);
void convertCellIdentityRilToHidl(const RIL_CellIdentity_v16& in, V1_5::CellIdentity& out);
void convertRilBarringInfoList(const std::vector<qcril::interfaces::RILBarringInfo>& rilBarInfo,
                               hidl_vec<V1_5::BarringInfo>& records);
void convertGsmCellIdentityRilToHidl(const RIL_CellIdentityGsm_v12& rilGsmCellIdentity,
                                     V1_5::CellIdentityGsm& hidlGsmCellIdentity);

void convertWcdmaCellIdentityRilToHidl(const RIL_CellIdentityWcdma_v12& rilWcdmaCellIdentity,
                                       V1_5::CellIdentityWcdma& hidlWcdmaCellIdentity);

void convertTdscdmaCellIdentityRilToHidl(const RIL_CellIdentityTdscdma& rilTdscdmaCellIdentity,
                                         V1_5::CellIdentityTdscdma& hidlTdscdmaCellIdentity);

void convertCdmaCellIdentityRilToHidl(const RIL_CellIdentityCdma& rilCdmaCellIdentity,
                                      V1_2::CellIdentityCdma& hidlCdmaCellIdentity);

void convertLteCellIdentityRilToHidl(const RIL_CellIdentityLte_v12& rilLteCellIdentity,
                                     V1_5::CellIdentityLte& hidlLteCellIdentity);

void convertNrCellIdentityRilToHidl(const RIL_CellIdentityNr& rilNrCellIdentity,
                                    V1_5::CellIdentityNr& hidlNrCellIdentity);

void fillVoiceRegistrationStateResponse(V1_5::RegStateResult& out,
                                        const RIL_VoiceRegistrationStateResponse& in);
void fillDataRegistrationStateResponse(V1_5::RegStateResult& out,
                                       const RIL_DataRegistrationStateResponse& in);

RIL_Errno convertToHidl(hidl_vec<V1_5::RadioAccessSpecifier>& out,
                        const RIL_SysSelChannels& in);

RIL_RadioTechnology getRilRadioTechnologyFromRan(V1_5::RadioAccessNetworks ran);
RIL_UIM_PersoSubstate convertHidlToRilPersoType(V1_5::PersoSubstate persoType);

void convertRilCellInfoList(const std::vector<RIL_CellInfo_v12>& rilCellInfo,
                            hidl_vec<V1_5::CellInfo>& records);
rildata::AccessNetwork_t convertHidlAccessNetworkToDataAccessNetwork_1_5(V1_5::AccessNetwork ran);

V1_5::SetupDataCallResult convertDcResultToHidlDcResult_1_5(const rildata::DataCallResult_t& result);

rildata::DataProfileInfo_t convertHidlDataProfileInfoToRil(const V1_5::DataProfileInfo& profile);

vector<V1_5::LinkAddress> convertLinkAddressToHidlVector(vector<rildata::LinkAddress_t> addresses);

V1_0::RadioError convertGetIccCardStatusResponse_1_5(V1_5::CardStatus& cardStatus,
                                                 std::shared_ptr<RIL_UIM_CardStatus> p_cur);
V1_5::EutranBands calculateBandFromEarfcn(int earfcn);
RIL_Errno convertHidlRasToRilRas(RIL_RadioAccessSpecifier& ras_to,
                                 const V1_5::RadioAccessSpecifier& ras_from);
}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_UTILS_1_5_H__
