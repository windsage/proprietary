/*****************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "framework/Log.h"

#include "interfaces/mbn/mbn.h"
#include "interfaces/lpa/lpa_service_types.h"
#include <aidl/vendor/qti/hardware/radio/lpa/UimLpaUserConsentType.h>

namespace aidlimports {
using namespace aidl::vendor::qti::hardware::radio::lpa;
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace utils {

UimLpaUserConsentType convertLpaServiceTypesToUserConsentType(lpa_user_consent_type& consent_type);

}  // namespace utils
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
