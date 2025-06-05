/*===========================================================================
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/sim/IRadioSimIndication.h"
#include "ril_utf_rild_sim.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace sim {
class IRadioSimIndicationImpl : public IRadioSimIndication {
public:
  IRadioSimIndicationImpl() {}

  virtual ~IRadioSimIndicationImpl() {}

  ::ndk::ScopedAStatus carrierInfoForImsiEncryption(::aidl::android::hardware::radio::RadioIndicationType in_info) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus cdmaSubscriptionSourceChanged(::aidl::android::hardware::radio::RadioIndicationType in_type,
                        ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource in_cdmaSource) override;

  ::ndk::ScopedAStatus simPhonebookChanged(::aidl::android::hardware::radio::RadioIndicationType in_type) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus simPhonebookRecordsReceived(::aidl::android::hardware::radio::RadioIndicationType in_type, ::aidl::android::hardware::radio::sim::PbReceivedStatus in_status, const std::vector<::aidl::android::hardware::radio::sim::PhonebookRecordInfo>& in_records) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus simRefresh(::aidl::android::hardware::radio::RadioIndicationType in_type, const ::aidl::android::hardware::radio::sim::SimRefreshResult& in_refreshResult) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus simStatusChanged(::aidl::android::hardware::radio::RadioIndicationType in_type) override;

  ::ndk::ScopedAStatus stkEventNotify(::aidl::android::hardware::radio::RadioIndicationType in_type, const std::string& in_cmd) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus stkProactiveCommand(::aidl::android::hardware::radio::RadioIndicationType in_type, const std::string& in_cmd) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus stkSessionEnd(::aidl::android::hardware::radio::RadioIndicationType in_type) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus subscriptionStatusChanged(::aidl::android::hardware::radio::RadioIndicationType in_type, bool in_activate) override;

  ::ndk::ScopedAStatus uiccApplicationsEnablementChanged(::aidl::android::hardware::radio::RadioIndicationType in_type, bool in_enabled) override {
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
}  // namespace sim
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl

namespace aidlsim {
  using namespace aidl::android::hardware::radio::sim;
}