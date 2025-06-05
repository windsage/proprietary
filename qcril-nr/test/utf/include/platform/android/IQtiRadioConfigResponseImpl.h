/*===========================================================================
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/vendor/qti/hardware/radio/qtiradioconfig/IQtiRadioConfigResponse.h"
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
class IQtiRadioConfigResponseImpl : public IQtiRadioConfigResponse {
 public:
  IQtiRadioConfigResponseImpl()
  {
  }

  virtual ~IQtiRadioConfigResponseImpl()
  {
  }

  ::ndk::ScopedAStatus getSecureModeStatusResponse(int32_t in_serial,
                                                   int32_t in_errorCode,
                                                   bool in_status) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setMsimPreferenceResponse(int32_t in_serial, int32_t in_errorCode) override;

  ::ndk::ScopedAStatus getSimTypeInfoResponse(
      int32_t in_serial,
      int32_t in_errorCode,
      const std::vector<::aidlqtiradioconfig::SimTypeInfo>& in_simTypeInfo) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setSimTypeResponse(int32_t in_serial, int32_t in_errorCode) override
  {
    return ndk::ScopedAStatus::ok();
  }

    ::ndk::ScopedAStatus getDualDataCapabilityResponse(
      const ::aidl::vendor::qti::hardware::radio::RadioResponseInfo& in_info,
      bool in_support) override
    {
      return ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus setDualDataUserPreferenceResponse(
      const ::aidl::vendor::qti::hardware::radio::RadioResponseInfo& in_info) override
    {
      return ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus getCiwlanCapabilityResponse(
      const ::aidl::vendor::qti::hardware::radio::RadioResponseInfo& in_info,
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

  ::ndk::ScopedAStatus sendUserPreferenceForDataDuringVoiceCallResponse
         (const ::aidl::vendor::qti::hardware::radio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getDdsSwitchCapabilityResponse(const ::aidl::vendor::qti::hardware::radio::RadioResponseInfo& in_info, bool in_support) override
  {
    return ndk::ScopedAStatus::ok();
  }
};
}  // namespace qtiradioconfig
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
