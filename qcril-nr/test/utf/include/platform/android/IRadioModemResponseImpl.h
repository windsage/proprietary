/*===========================================================================
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "ril_utf_rild_sim.h"
#include "NasAidlUtil.h"
#include "aidl/android/hardware/radio/modem/IRadioModemResponse.h"
#include "ril_utf_rild_sim.h"
#include "platform/android/NasAidlUtil.h"

namespace aidlmodem {
using namespace aidl::android::hardware::radio::modem;
}

namespace aidlradio {
using namespace aidl::android::hardware::radio;
}

#if defined(FEATURE_TARGET_GLIBC_x86) || defined(QMI_RIL_UTF)
extern "C" size_t strlcpy(char*, const char*, size_t);
#endif

namespace aidlmodem {
  using namespace aidl::android::hardware::radio::modem;
}

namespace aidlradio {
  using namespace aidl::android::hardware::radio;
}

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace modem {

class IRadioModemResponseImpl : public IRadioModemResponse {
public:
  IRadioModemResponseImpl() {}

  virtual ~IRadioModemResponseImpl() {}

  ::ndk::ScopedAStatus acknowledgeRequest(int32_t in_serial) override;

  ::ndk::ScopedAStatus enableModemResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus getBasebandVersionResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const std::string& in_version) override;

  ::ndk::ScopedAStatus getDeviceIdentityResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const std::string& in_imei, const std::string& in_imeisv,
      const std::string& in_esn, const std::string& in_meid) override;

  ::ndk::ScopedAStatus getHardwareConfigResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const std::vector<::aidlmodem::HardwareConfig>& in_config) override;

  ::ndk::ScopedAStatus getModemActivityInfoResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const ::aidlmodem::ActivityStatsInfo& in_activityInfo) override;

  ::ndk::ScopedAStatus getModemStackStatusResponse(
      const ::aidlradio::RadioResponseInfo& in_info, bool in_isEnabled) override;

  ::ndk::ScopedAStatus getRadioCapabilityResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const ::aidlmodem::RadioCapability& in_rc) override;

  ::ndk::ScopedAStatus nvReadItemResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const std::string& in_result) override;

  ::ndk::ScopedAStatus nvResetConfigResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus nvWriteCdmaPrlResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus nvWriteItemResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus requestShutdownResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus sendDeviceStateResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setRadioCapabilityResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const ::aidlmodem::RadioCapability& in_rc) override;

  ::ndk::ScopedAStatus setRadioPowerResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus getImeiResponse(
      const aidlradio::RadioResponseInfo& in_responseInfo,
      const std::optional<aidlmodem::ImeiInfo>& in_imeiInfo) override;

  ::ndk::SpAIBinder asBinder() override {
    return ::ndk::SpAIBinder();
  }

  bool isRemote() override {
    return false;
  }

  ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) override {
    return ndk::ScopedAStatus::ok();
  }
};
}  // namespace modem
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl

namespace aidlmodem {
  using namespace aidl::android::hardware::radio::modem;
}
