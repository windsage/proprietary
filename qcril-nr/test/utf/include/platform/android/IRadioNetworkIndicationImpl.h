/*===========================================================================
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/network/IRadioNetworkIndication.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace network {
class IRadioNetworkIndicationImpl : public IRadioNetworkIndication {
public:
  IRadioNetworkIndicationImpl() {}

  virtual ~IRadioNetworkIndicationImpl() {}

  ::ndk::ScopedAStatus barringInfoChanged(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      const ::aidl::android::hardware::radio::network::CellIdentity& in_cellIdentity,
                      const std::vector<::aidl::android::hardware::radio::network::BarringInfo>& in_barringInfos) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus cdmaPrlChanged(::aidl::android::hardware::radio::RadioIndicationType in_type, int32_t in_version) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus cellInfoList(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      const std::vector<::aidl::android::hardware::radio::network::CellInfo>& in_records) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus currentLinkCapacityEstimate(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      const ::aidl::android::hardware::radio::network::LinkCapacityEstimate& in_lce) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus currentPhysicalChannelConfigs(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      const std::vector<::aidl::android::hardware::radio::network::PhysicalChannelConfig>& in_configs) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus currentSignalStrength(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      const ::aidl::android::hardware::radio::network::SignalStrength& in_signalStrength) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus imsNetworkStateChanged(::aidl::android::hardware::radio::RadioIndicationType in_type) override;

  ::ndk::ScopedAStatus networkScanResult(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      const ::aidl::android::hardware::radio::network::NetworkScanResult& in_result) override;

  ::ndk::ScopedAStatus networkStateChanged(::aidl::android::hardware::radio::RadioIndicationType in_type) override;

  ::ndk::ScopedAStatus nitzTimeReceived(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      const std::string& in_nitzTime, int64_t in_receivedTimeMs, int64_t in_ageMs) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus registrationFailed(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      const ::aidl::android::hardware::radio::network::CellIdentity& in_cellIdentity,
                      const std::string& in_chosenPlmn, int32_t in_domain, int32_t in_causeCode,
                      int32_t in_additionalCauseCode) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus restrictedStateChanged(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      ::aidl::android::hardware::radio::network::PhoneRestrictedState in_state) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus suppSvcNotify(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      const ::aidl::android::hardware::radio::network::SuppSvcNotification& in_suppSvc) override;

  ::ndk::ScopedAStatus voiceRadioTechChanged(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      ::aidl::android::hardware::radio::RadioTechnology in_rat) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus emergencyNetworkScanResult(::aidl::android::hardware::radio::RadioIndicationType in_type,
                      const ::aidl::android::hardware::radio::network::EmergencyRegResult& in_result) override {
    return ndk::ScopedAStatus::ok();
  }

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
}  // namespace network
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
