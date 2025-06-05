/*===========================================================================
 *
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#pragma once

#include <aidl/vendor/qti/hardware/radio/qtiradio/IQtiRadioResponse.h>

namespace qtiradioaidl {
  using namespace aidl::vendor::qti::hardware::radio::qtiradio;
}

class IQtiRadioAidlResponseClientImpl : public qtiradioaidl::IQtiRadioResponse {
  virtual ::ndk::ScopedAStatus onNrIconTypeResponse(int32_t in_serial, int32_t in_errorCode, ::qtiradioaidl::NrIconType in_iconType) override;
  virtual ::ndk::ScopedAStatus onEnableEndcResponse(int32_t in_serial, int32_t in_errorCode, ::qtiradioaidl::Status in_status) override;
  virtual ::ndk::ScopedAStatus onEndcStatusResponse(int32_t in_serial, int32_t in_errorCode, ::qtiradioaidl::EndcStatus in_endcStatus) override;
  virtual ::ndk::ScopedAStatus setNrConfigResponse(int32_t in_serial, int32_t in_errorCode, ::qtiradioaidl::Status in_status) override;
  virtual ::ndk::ScopedAStatus onNrConfigResponse(int32_t in_serial, int32_t in_errorCode, ::qtiradioaidl::NrConfig in_config) override;
  virtual ::ndk::ScopedAStatus getQtiRadioCapabilityResponse(int32_t in_serial, int32_t in_errorCode, ::qtiradioaidl::RadioAccessFamily in_raf) override;
  virtual ::ndk::ScopedAStatus getCallForwardStatusResponse(int32_t in_serial, int32_t in_errorCode, const std::vector<::qtiradioaidl::CallForwardInfo> & in_callForwardInfoList) override;
  virtual ::ndk::ScopedAStatus getFacilityLockForAppResponse(int32_t in_serial, int32_t in_errorCode, int32_t in_response) override;
  virtual ::ndk::ScopedAStatus getImeiResponse(int32_t in_serial, int32_t in_errorCode, const ::qtiradioaidl::ImeiInfo & in_info) override;
  virtual ::ndk::ScopedAStatus getDdsSwitchCapabilityResponse(int32_t in_serial, int32_t in_errorCode, bool in_support) override;
  virtual ::ndk::ScopedAStatus sendUserPreferenceForDataDuringVoiceCallResponse(int32_t in_serial, int32_t in_errorCode) override;
  virtual ::ndk::ScopedAStatus setNrUltraWidebandIconConfigResponse(int32_t in_serial, int32_t in_errorCode) override;
  virtual ::ndk::ScopedAStatus getNetworkSelectionModeResponse(int32_t in_serial, int32_t in_errorCode, const ::qtiradioaidl::NetworkSelectionMode & in_mode) override;
  virtual ::ndk::ScopedAStatus setNetworkSelectionModeAutomaticResponse(int32_t in_serial, int32_t in_errorCode) override;
  virtual ::ndk::ScopedAStatus setNetworkSelectionModeManualResponse(int32_t in_serial, int32_t in_errorCode) override;
  virtual ::ndk::ScopedAStatus startNetworkScanResponse(int32_t in_serial, int32_t in_errorCode) override;
  virtual ::ndk::ScopedAStatus stopNetworkScanResponse(int32_t in_serial, int32_t in_errorCode) override;
  virtual ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) override;
  virtual ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) override;
  ::ndk::SpAIBinder asBinder() {
    return ::ndk::SpAIBinder();
  }
  bool isRemote() {
    return false;
  }
};
