/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "aidl/android/hardware/radio/config/BnRadioConfig.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace config {
class BnRadioConfigDefault : public BnRadioConfig {
public:
  BnRadioConfigDefault() {}

  virtual ~BnRadioConfigDefault() {}

  ::ndk::ScopedAStatus getHalDeviceCapabilities(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getNumOfLiveModems(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getPhoneCapability(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getSimSlotsStatus(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setNumOfLiveModems(int32_t in_serial, int8_t in_numOfLiveModems) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setPreferredDataModem(int32_t in_serial, int8_t in_modemId) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setResponseFunctions(const std::shared_ptr<::aidl::android::hardware::radio::config::IRadioConfigResponse>& in_radioConfigResponse, const std::shared_ptr<::aidl::android::hardware::radio::config::IRadioConfigIndication>& in_radioConfigIndication) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setSimSlotsMapping(int32_t in_serial, const std::vector<::aidl::android::hardware::radio::config::SlotPortMapping>& in_slotMap) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
};
}  // namespace config
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
