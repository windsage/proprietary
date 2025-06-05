/*===========================================================================
 *  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/voice/IRadioVoiceResponse.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace voice {
class IRadioVoiceResponseImpl : public IRadioVoiceResponse {
public:
  IRadioVoiceResponseImpl() {}

  virtual ~IRadioVoiceResponseImpl() {}

  ::ndk::ScopedAStatus acceptCallResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus acknowledgeRequest(int32_t in_serial) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus cancelPendingUssdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus conferenceResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus dialResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus emergencyDialResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus exitEmergencyCallbackModeResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus explicitCallTransferResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus getCallForwardStatusResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
      const std::vector<::aidl::android::hardware::radio::voice::CallForwardInfo>&
          in_callForwardInfos) override;

  ::ndk::ScopedAStatus getCallWaitingResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
      bool in_enable,
      int32_t in_serviceClass) override;

  ::ndk::ScopedAStatus getClipResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
      ::aidl::android::hardware::radio::voice::ClipStatus in_status) override;

  ::ndk::ScopedAStatus getClirResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
      int32_t in_n,
      int32_t in_m) override;

  ::ndk::ScopedAStatus getCurrentCallsResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
      const std::vector<::aidl::android::hardware::radio::voice::Call>& in_calls) override;

  ::ndk::ScopedAStatus getLastCallFailCauseResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
      const ::aidl::android::hardware::radio::voice::LastCallFailCauseInfo& in_failCauseinfo) override;

  ::ndk::ScopedAStatus getMuteResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, bool in_enable) override;

  ::ndk::ScopedAStatus getPreferredVoicePrivacyResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, bool in_enable) override;

  ::ndk::ScopedAStatus getTtyModeResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
      ::aidl::android::hardware::radio::voice::TtyMode in_mode) override;

  ::ndk::ScopedAStatus handleStkCallSetupRequestFromSimResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus hangupConnectionResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus hangupForegroundResumeBackgroundResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus hangupWaitingOrBackgroundResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus isVoNrEnabledResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, bool in_enable) override;

  ::ndk::ScopedAStatus rejectCallResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus sendBurstDtmfResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus sendCdmaFeatureCodeResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus sendDtmfResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus sendUssdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus separateConnectionResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setCallForwardResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setCallWaitingResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setClirResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setMuteResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setPreferredVoicePrivacyResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setTtyModeResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setVoNrEnabledResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus startDtmfResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus stopDtmfResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus switchWaitingOrHoldingAndActiveResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

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

namespace aidlvoice {
  using namespace aidl::android::hardware::radio::voice;
}
