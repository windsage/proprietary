/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modem_aidl_service_utils.h"

#if defined(FEATURE_TARGET_GLIBC_x86) || defined(QMI_RIL_UTF)
extern "C" size_t strlcpy(char*, const char*, size_t);
#endif

namespace qti {
namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace modem {
namespace utils {

bool convert(const RIL_ActivityStatsInfo& from, aidlmodem::ActivityStatsInfo& to) {
  aidlmodem::ActivityStatsTechSpecificInfo techSpecificInfo {};
  techSpecificInfo.rat = aidlmodem::AccessNetwork::UNKNOWN;
  techSpecificInfo.frequencyRange = aidlmodem::ActivityStatsTechSpecificInfo::FREQUENCY_RANGE_UNKNOWN;
  techSpecificInfo.rxModeTimeMs = from.rx_mode_time_ms;

  for (int i = 0; i < RIL_NUM_TX_POWER_LEVELS; i++) {
    techSpecificInfo.txmModetimeMs.push_back(from.tx_mode_time_ms[i]);
  }

  to.sleepModeTimeMs = from.sleep_mode_time_ms;
  to.idleModeTimeMs = from.idle_mode_time_ms;
  to.techSpecificInfo.push_back(techSpecificInfo);

  return true;
}

bool convert(const RIL_RadioCapability& from, aidlmodem::RadioCapability& to) {
  switch (from.phase) {
    case 0:
      to.phase = aidlmodem::RadioCapability::PHASE_CONFIGURED;
      break;
    case 1:
      to.phase = aidlmodem::RadioCapability::PHASE_START;
      break;
    case 2:
      to.phase = aidlmodem::RadioCapability::PHASE_APPLY;
      break;
    case 3:
      to.phase = aidlmodem::RadioCapability::PHASE_UNSOL_RSP;
      break;
    case 4:
      to.phase = aidlmodem::RadioCapability::PHASE_FINISH;
      break;
    default:
      return false;
  }

  switch (from.status) {
    case 0:
      to.status = aidlmodem::RadioCapability::STATUS_NONE;
      break;
    case 1:
      to.status = aidlmodem::RadioCapability::STATUS_SUCCESS;
      break;
    case 2:
      to.status = aidlmodem::RadioCapability::STATUS_FAIL;
      break;
    default:
      return false;
  }

  to.session = from.session;
  to.raf = from.rat;
  to.logicalModemUuid = from.logicalModemUuid;

  return true;
}

bool convert(const aidlmodem::RadioCapability& from, RIL_RadioCapability& to) {
  switch (from.phase) {
    case aidlmodem::RadioCapability::PHASE_CONFIGURED:
      to.phase = 0;
      break;
    case aidlmodem::RadioCapability::PHASE_START:
      to.phase = 1;
      break;
    case aidlmodem::RadioCapability::PHASE_APPLY:
      to.phase = 2;
      break;
    case aidlmodem::RadioCapability::PHASE_UNSOL_RSP:
      to.phase = 3;
      break;
    case aidlmodem::RadioCapability::PHASE_FINISH:
      to.phase = 4;
      break;
    default:
      return false;
  }

  switch (from.status) {
    case aidlmodem::RadioCapability::STATUS_NONE:
      to.status = 0;
      break;
    case aidlmodem::RadioCapability::STATUS_SUCCESS:
      to.status = 1;
      break;
    case aidlmodem::RadioCapability::STATUS_FAIL:
      to.status = 2;
      break;
    default:
      return false;
  }

  to.session = from.session;
  to.rat = from.raf;

  if (strlcpy(to.logicalModemUuid, from.logicalModemUuid.c_str(), sizeof(to.logicalModemUuid))
      >= sizeof(to.logicalModemUuid)) {
    return false;
  }

  return true;
}

bool convert(const qcril::interfaces::RilGetImeiInfoResult_t& from, aidlmodem::ImeiInfo& to)
{
  switch (from.getImeiType()) {
    case qcril::interfaces::ImeiType::IMEI_TYPE_PRIMARY:
      to.type = ::aidlmodem::ImeiInfo::ImeiType::PRIMARY;
      break;
    case qcril::interfaces::ImeiType::IMEI_TYPE_SECONDARY:
      to.type = ::aidlmodem::ImeiInfo::ImeiType::SECONDARY;
      break;
    case qcril::interfaces::ImeiType::IMEI_TYPE_INVALID:
    default:
      return false;
  }

  to.imei = from.getImei();
  to.svn = from.getImeiSv();
  return true;
}

}  // namespace utils
}  // namespace modem
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
}  // namespace qti
