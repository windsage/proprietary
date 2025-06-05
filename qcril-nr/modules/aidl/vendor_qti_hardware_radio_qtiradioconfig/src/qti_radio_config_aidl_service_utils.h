/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_RADIO_CONFIG_AIDL_SERVICE_UTILS_H_
#define _QTI_RADIO_CONFIG_AIDL_SERVICE_UTILS_H_
#include <telephony/ril.h>
#include <aidl/vendor/qti/hardware/radio/qtiradioconfig/MsimPreference.h>
#include <aidl/vendor/qti/hardware/radio/qtiradioconfig/SimType.h>
#include <aidl/vendor/qti/hardware/radio/qtiradioconfig/SimTypeInfo.h>
#include "interfaces/uim/qcril_uim_types.h"
#include <aidl/vendor/qti/hardware/radio/qtiradioconfig/CiwlanCapability.h>
#include <aidl/vendor/qti/hardware/radio/qtiradioconfig/DualDataRecommendation.h>
#include "UnSolMessages/DataSubRecommendationIndMessage.h"

namespace aidlqtiradioconfig {
using namespace aidl::vendor::qti::hardware::radio::qtiradioconfig;
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradioconfig {
namespace utils {

Msim_Preference convert_msim_preference(aidlqtiradioconfig::MsimPreference pref);
void convertGetSimTypeInfoResponse(std::vector<aidlqtiradioconfig::SimTypeInfo>& simTypeInfo,
                                   std::shared_ptr<RIL_UIM_GetSimTypeResp> respData);
void convertToUimSimType(std::vector<uint32_t>& out_simType,
                         const std::vector<aidlqtiradioconfig::SimType> in_simType);
void convert(rildata::CIWlanCapabilityType dataCap, aidlqtiradioconfig::CiwlanCapability& cap);
void convert(rildata::Recommendation_t srcRec,
    aidlqtiradioconfig::DualDataRecommendation& targetRec);
}  // namespace utils
}  // namespace qtiradioconfig
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
#endif  //_QTI_RADIO_CONFIG_AIDL_SERVICE_UTILS_H_
