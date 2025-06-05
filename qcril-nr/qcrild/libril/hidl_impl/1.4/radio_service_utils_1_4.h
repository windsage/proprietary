/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RADIO_SERVICE_UTILS_1_4_H__
#define __RADIO_SERVICE_UTILS_1_4_H__

#include "android/hardware/radio/1.4/types.h"
#include "telephony/ril.h"
#include "framework/Message.h"
#include "MessageCommon.h"
#include "interfaces/voice/voice.h"
#include "interfaces/nas/nas_types.h"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

uint64_t toRilCategories(const hidl_bitfield<V1_4::EmergencyServiceCategory>& categories);
qcril::interfaces::EmergencyCallRouting toRilRouting(const V1_4::EmergencyCallRouting& routing);
void convertToHal(V1_4::NrSignalStrength& out, const RIL_NR_SignalStrength& in);
void convertRilSignalStrengthToHal(const RIL_SignalStrength& rilSignalStrength,
                                   V1_4::SignalStrength& signalStrength);
int convertRilEccListToHal(const qcril::interfaces::RIL_EccList& ril_ecc_list,
                           ::android::hardware::hidl_vec<V1_4::EmergencyNumber>& ecc_list);
void convertRilCellInfoToHal_1_4(V1_4::CellInfo& hidlCellInfo, const RIL_CellInfo_v12& rilCellInfo);
void convertRilCellInfoListToHal_1_4(const std::vector<RIL_CellInfo_v12>& rillCellInfo,
                                     hidl_vec<V1_4::CellInfo>& records);
void fillDataRegistrationStateResponse(V1_4::DataRegStateResult& out,
                                       const RIL_DataRegistrationStateResponse& in);
rildata::DataProfileInfo_t convertHidlDataProfileInfoToRil(const V1_4::DataProfileInfo& profile);
V1_4::SetupDataCallResult convertDcResultToHidlDcResult_1_4(const rildata::DataCallResult_t& result);
V1_4::RadioTechnology convertRadioAccessFamilyTypeToRadioTechnology(rildata::RadioAccessFamily_t accessFamily);
V1_4::FrequencyRange convertFrequencyRangeToHalType(rildata::FrequencyRange_t range);
V1_4::PdpProtocolType convertStringToPdpProtocolType(const string &type);
vector<hidl_string> convertAddrStringToHidlStringVector(const string &addr);
string convertPdpProtocolTypeToString(const V1_4::PdpProtocolType protocol);
rildata::AccessNetwork_t convertHidlAccessNetworkToDataAccessNetwork_1_4(V1_4::AccessNetwork ran);
V1_4::DataCallFailCause convertDcFailCauseToHidlDcFailCause_1_4(const rildata::DataCallFailCause_t &cause);
hidl_bitfield<V1_4::RadioAccessFamily> convertRilNetworkTypeBitmapToHidl_1_4(int pref);
int convertHidlNetworkTypeBitmapToRil_1_4(hidl_bitfield<V1_4::RadioAccessFamily> pref);

}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_UTILS_1_4_H__
