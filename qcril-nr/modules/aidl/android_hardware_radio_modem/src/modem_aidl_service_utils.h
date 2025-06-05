/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "aidl/android/hardware/radio/modem/ActivityStatsInfo.h"
#include "aidl/android/hardware/radio/modem/RadioCapability.h"
#include "aidl/android/hardware/radio/modem/ImeiInfo.h"
#include "telephony/ril.h"
#include "interfaces/dms/dms_types.h"

namespace aidlmodem {
  using namespace aidl::android::hardware::radio;
  using namespace aidl::android::hardware::radio::modem;
}

namespace qti {
namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace modem {
namespace utils {

bool convert(const RIL_ActivityStatsInfo& from, aidlmodem::ActivityStatsInfo& to);
bool convert(const RIL_RadioCapability& from, aidlmodem::RadioCapability& to);
bool convert(const aidlmodem::RadioCapability& from, RIL_RadioCapability& to);
bool convert(const qcril::interfaces::RilGetImeiInfoResult_t& to, aidlmodem::ImeiInfo& from);

}  // namespace utils
}  // namespace modem
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
}  // namespace qti
