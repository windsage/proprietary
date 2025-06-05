/*===========================================================================
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/config/IRadioConfigResponse.h"

namespace aidlradio {
using namespace aidl::android::hardware::radio;
}

namespace aidlconfig {
using namespace aidl::android::hardware::radio::config;
}

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace config {
class IRadioConfigResponseImpl : public IRadioConfigResponse {
public:
  IRadioConfigResponseImpl() {}

  virtual ~IRadioConfigResponseImpl() {}

  ::ndk::ScopedAStatus getHalDeviceCapabilitiesResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                                        bool in_modemReducedFeatureSet1) override;

  ::ndk::ScopedAStatus getNumOfLiveModemsResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                                  int8_t in_numOfLiveModems) override;

  ::ndk::ScopedAStatus getPhoneCapabilityResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const ::aidlconfig::PhoneCapability& in_phoneCapability) override;

  ::ndk::ScopedAStatus getSimSlotsStatusResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const std::vector<::aidlconfig::SimSlotStatus>& in_slotStatus) override;

  ::ndk::ScopedAStatus setNumOfLiveModemsResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setPreferredDataModemResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setSimSlotsMappingResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

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
}  // namespace config
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
