/*===========================================================================
 *  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/voice/IRadioVoiceIndication.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace voice {
class IRadioVoiceIndicationImpl : public IRadioVoiceIndication {
public:
  IRadioVoiceIndicationImpl() {}

  virtual ~IRadioVoiceIndicationImpl() {}

  ::ndk::ScopedAStatus callRing(
      ::aidl::android::hardware::radio::RadioIndicationType in_type,
      bool in_isGsm,
      const ::aidl::android::hardware::radio::voice::CdmaSignalInfoRecord& in_record) override;

  ::ndk::ScopedAStatus callStateChanged(
      ::aidl::android::hardware::radio::RadioIndicationType in_type) override;

  ::ndk::ScopedAStatus cdmaCallWaiting(
      ::aidl::android::hardware::radio::RadioIndicationType in_type,
      const ::aidl::android::hardware::radio::voice::CdmaCallWaiting& in_callWaitingRecord) override;

  ::ndk::ScopedAStatus cdmaInfoRec(
      ::aidl::android::hardware::radio::RadioIndicationType in_type,
      const std::vector<::aidl::android::hardware::radio::voice::CdmaInformationRecord>& in_records)
      override;

  ::ndk::ScopedAStatus cdmaOtaProvisionStatus(
      ::aidl::android::hardware::radio::RadioIndicationType in_type,
      ::aidl::android::hardware::radio::voice::CdmaOtaProvisionStatus in_status) override;

  ::ndk::ScopedAStatus currentEmergencyNumberList(
      ::aidl::android::hardware::radio::RadioIndicationType in_type,
      const std::vector<::aidl::android::hardware::radio::voice::EmergencyNumber>&
          in_emergencyNumberList) override;

  ::ndk::ScopedAStatus enterEmergencyCallbackMode(
      ::aidl::android::hardware::radio::RadioIndicationType in_type) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus exitEmergencyCallbackMode(
      ::aidl::android::hardware::radio::RadioIndicationType in_type) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus indicateRingbackTone(
      ::aidl::android::hardware::radio::RadioIndicationType in_type, bool in_start) override;

  ::ndk::ScopedAStatus onSupplementaryServiceIndication(
      ::aidl::android::hardware::radio::RadioIndicationType in_type,
      const ::aidl::android::hardware::radio::voice::StkCcUnsolSsResult& in_ss) override;

  ::ndk::ScopedAStatus resendIncallMute(
      ::aidl::android::hardware::radio::RadioIndicationType in_type) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus srvccStateNotify(
      ::aidl::android::hardware::radio::RadioIndicationType in_type,
      ::aidl::android::hardware::radio::voice::SrvccState in_state) override;

  ::ndk::ScopedAStatus onUssd(::aidl::android::hardware::radio::RadioIndicationType in_type,
                              ::aidl::android::hardware::radio::voice::UssdModeType in_modeType,
                              const std::string& in_msg) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus stkCallControlAlphaNotify(
      ::aidl::android::hardware::radio::RadioIndicationType in_type,
      const std::string& in_alpha) override;

  ::ndk::ScopedAStatus stkCallSetup(::aidl::android::hardware::radio::RadioIndicationType in_type,
                                    int64_t in_timeout) override
  {
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
}  // namespace voice
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
