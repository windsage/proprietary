/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "uim_lpa_stable_aidl_service_utils.h"

#undef TAG
#define TAG "RILQ"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace utils {

UimLpaUserConsentType convertLpaServiceTypesToUserConsentType(lpa_user_consent_type& consent_type)
{
  UimLpaUserConsentType userConsentType{};
  switch (consent_type) {
    case LPA_NO_CONFIRMATION_REQD: {
      userConsentType = UimLpaUserConsentType::UIM_LPA_NO_CONFIRMATION_REQD;
      break;
    }
    case LPA_STRONG_CONFIMRATION_REQD: {
      userConsentType = UimLpaUserConsentType::UIM_LPA_SIMPLE_CONFIRMATION_REQD;
      break;
    }
    case LPA_SIMPLE_CONFIRMATION_REQD: {
      userConsentType = UimLpaUserConsentType::UIM_LPA_STRONG_CONFIRMATION_REQD;
      break;
    }
  }
  return userConsentType;
}

}  // namespace utils
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl