/*===========================================================================
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/vendor/qti/hardware/radio/qtiradioconfig/IQtiRadioConfigIndication.h"
#include "ril_utf_rild_sim.h"

namespace aidlqtiradioconfig {
  using namespace aidl::vendor::qti::hardware::radio::qtiradioconfig;
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradioconfig {
class IQtiRadioConfigIndicationImpl : public IQtiRadioConfigIndication {
 public:
  IQtiRadioConfigIndicationImpl()
  {
  }

  virtual ~IQtiRadioConfigIndicationImpl()
  {
  }

  ::ndk::ScopedAStatus onSecureModeStatusChange(bool in_enabled) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus onDualDataCapabilityChanged(bool in_support) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus onDualDataRecommendation(
    const ::aidlqtiradioconfig::DualDataRecommendation& in_rec) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus onCiwlanCapabilityChanged(
    ::aidlqtiradioconfig::CiwlanCapability in_capability) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::SpAIBinder asBinder() override
  {
    return ::ndk::SpAIBinder();
  }

  bool isRemote() override
  {
    return false;
  }

  ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus onDdsSwitchCapabilityChanged(bool in_capability) override
  {
    return ::ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus onDdsSwitchRecommendation(int32_t in_recommendedSlotId) override
  {
    return ::ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus onDdsSwitchCriteriaChanged(bool in_telephonyDdsSwitch) override
  {
    return ::ndk::ScopedAStatus::ok();
  }
};
}  // namespace qtiradioconfig
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
