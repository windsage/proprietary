/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "framework/Log.h"
#include "qti_radio_config_aidl_service_utils.h"

#undef TAG
#define TAG "RILQ"
#include <aidl/vendor/qti/hardware/radio/qtiradioconfig/DualDataSubscription.h>
#include <aidl/vendor/qti/hardware/radio/qtiradioconfig/DualDataAction.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradioconfig {
namespace utils {

Msim_Preference convert_msim_preference(aidlqtiradioconfig::MsimPreference pref) {
    switch (pref) {
        case aidlqtiradioconfig::MsimPreference::DSDA:
          return MSIM_PREFERENCE_DSDA;
        case aidlqtiradioconfig::MsimPreference::DSDS:
          return MSIM_PREFERENCE_DSDS;
        default:
          return MSIM_PREFERENCE_INVALID;
    }
}

void convertGetSimTypeInfoResponse(std::vector<aidlqtiradioconfig::SimTypeInfo>& simTypeInfo,
                                   std::shared_ptr<RIL_UIM_GetSimTypeResp> respData) {
  if (!respData) {
    return;
  }

  QCRIL_LOG_DEBUG("length %d, %d", respData->sim_type_len, respData->supported_sim_type_len);
  simTypeInfo.resize(respData->sim_type_len);
  for (uint32_t index = 0; (index < respData->sim_type_len) && (index < RIL_UIM_MAX_CARD_COUNT);
       index++) {
    if (respData->sim_type[index] == RIL_UIM_SIM_TYPE_PHYSICAL) {
      simTypeInfo[index].currentSimType = aidlqtiradioconfig::SimType::SIM_TYPE_PHYSICAL;
    } else if (respData->sim_type[index] == RIL_UIM_SIM_TYPE_IUICC) {
      simTypeInfo[index].currentSimType = aidlqtiradioconfig::SimType::SIM_TYPE_INTEGRATED;
    } else if (respData->sim_type[index] == RIL_UIM_SIM_TYPE_EMBEDDED) {
      simTypeInfo[index].currentSimType = aidlqtiradioconfig::SimType::SIM_TYPE_ESIM;
    }
    QCRIL_LOG_DEBUG("current simType %s, %d", toString(simTypeInfo[index].currentSimType).c_str(),
                    respData->sim_type[index]);
  }

  // If supported simType length is more that sim_type_len ignore the excess supported sim type
  // values, ideally this should not happen but, adding this check for safer side.
  uint32_t supported_sim_type_len = (respData->supported_sim_type_len > respData->sim_type_len)
                                        ? respData->sim_type_len
                                        : respData->supported_sim_type_len;

  for (uint32_t index = 0; (index < supported_sim_type_len) && (index < RIL_UIM_MAX_CARD_COUNT);
       index++) {
    if (respData->supported_sim_type[index] == RIL_UIM_SUPPORTED_SIM_TYPE_PHYSICAL) {
      simTypeInfo[index].supportedSimTypes =
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_PHYSICAL);
    } else if (respData->supported_sim_type[index] == RIL_UIM_SUPPORTED_SIM_TYPE_IUICC) {
      simTypeInfo[index].supportedSimTypes =
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_INTEGRATED);
    } else if (respData->supported_sim_type[index] == RIL_UIM_SUPPORTED_SIM_TYPE_EMBEDDED) {
      simTypeInfo[index].supportedSimTypes =
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_ESIM);
    } else if (respData->supported_sim_type[index] == RIL_UIM_SUPPORTED_SIM_TYPE_PHYSICAL_IUICC) {
      simTypeInfo[index].supportedSimTypes =
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_PHYSICAL);
      simTypeInfo[index].supportedSimTypes |=
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_INTEGRATED);
    } else if (respData->supported_sim_type[index] ==
                   RIL_UIM_SUPPORTED_SIM_TYPE_PHYSICAL_EMBEDDED) {
      simTypeInfo[index].supportedSimTypes =
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_PHYSICAL);
      simTypeInfo[index].supportedSimTypes |=
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_ESIM);
    } else if (respData->supported_sim_type[index] ==
                   RIL_UIM_SUPPORTED_SIM_TYPE_INTEGRATED_EMBEDDED) {
      simTypeInfo[index].supportedSimTypes =
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_INTEGRATED);
      simTypeInfo[index].supportedSimTypes |=
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_ESIM);
    } else if (respData->supported_sim_type[index] ==
                   RIL_UIM_SUPPORTED_SIM_TYPE_PHYSICAL_INTEGRATED_EMBEDDED) {
      simTypeInfo[index].supportedSimTypes =
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_PHYSICAL);
      simTypeInfo[index].supportedSimTypes |=
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_INTEGRATED);
      simTypeInfo[index].supportedSimTypes |=
          static_cast<int32_t>(aidlqtiradioconfig::SimType::SIM_TYPE_ESIM);
    }
    QCRIL_LOG_DEBUG("supported simType %d, %d", simTypeInfo[index].supportedSimTypes,
                    respData->supported_sim_type[index]);
  }
}

void convertToUimSimType(std::vector<uint32_t>& out_simType,
                         const std::vector<aidlqtiradioconfig::SimType> in_simType) {
  out_simType.resize(in_simType.size());

  for (uint32_t index = 0; index < in_simType.size(); index++) {
    if (in_simType[index] == aidlqtiradioconfig::SimType::SIM_TYPE_PHYSICAL) {
      out_simType[index] = RIL_UIM_SIM_TYPE_PHYSICAL;
    } else if (in_simType[index] == aidlqtiradioconfig::SimType::SIM_TYPE_INTEGRATED) {
      out_simType[index] = RIL_UIM_SIM_TYPE_IUICC;
    } else if (in_simType[index] == aidlqtiradioconfig::SimType::SIM_TYPE_ESIM) {
      out_simType[index] = RIL_UIM_SIM_TYPE_EMBEDDED;
    }
    QCRIL_LOG_DEBUG("setSimType: simType, type:%s %d", toString(in_simType[index]).c_str(),
                    out_simType[index]);
  }
}

void convert(rildata::CIWlanCapabilityType srcCap,
  aidlqtiradioconfig::CiwlanCapability& targetCap) {
    switch (srcCap) {
      case rildata::CIWlanCapabilityType::None:
        targetCap = aidlqtiradioconfig::CiwlanCapability::NONE;
        return;
      case rildata::CIWlanCapabilityType::DDSSupported:
        targetCap = aidlqtiradioconfig::CiwlanCapability::DDS;
        return;
      case rildata::CIWlanCapabilityType::BothSubsSupported:
        targetCap = aidlqtiradioconfig::CiwlanCapability::BOTH;
        return;
      default:
        targetCap = aidlqtiradioconfig::CiwlanCapability::NONE;
        return;
    }
}

void convert(rildata::Recommendation_t srcRec,
    aidlqtiradioconfig::DualDataRecommendation& targetRec) {
  switch(srcRec.subs) {
    case rildata::SubscriptionType::DDS:
      targetRec.sub = aidlqtiradioconfig::DualDataSubscription::DDS;
      break;
    case rildata::SubscriptionType::NDDS:
      targetRec.sub = aidlqtiradioconfig::DualDataSubscription::NON_DDS;
      break;
    default:
      targetRec.sub = aidlqtiradioconfig::DualDataSubscription::NON_DDS;
      break;
  }

  switch(srcRec.action) {
    case rildata::Action::DataAllowed:
      targetRec.action = aidlqtiradioconfig::DualDataAction::DATA_ALLOW;
      break;
    case rildata::Action::DataNotAllowed:
      targetRec.action = aidlqtiradioconfig::DualDataAction::DATA_DISALLOW;
      break;
    default:
      targetRec.action = aidlqtiradioconfig::DualDataAction::DATA_DISALLOW;
      break;
  }
}
}  // namespace utils
}  // namespace qtiradioconfig
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
