/*===========================================================================
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/config/IRadioConfigIndication.h"

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
class IRadioConfigIndicationImpl : public IRadioConfigIndication {
public:
  IRadioConfigIndicationImpl() {}

  virtual ~IRadioConfigIndicationImpl() {}

  ::ndk::ScopedAStatus simSlotsStatusChanged(
      ::aidlradio::RadioIndicationType in_type,
      const std::vector<::aidlconfig::SimSlotStatus>& in_slotStatus) override;

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
