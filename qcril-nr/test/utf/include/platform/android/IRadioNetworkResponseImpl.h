/*===========================================================================
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/network/IRadioNetworkResponse.h"

#include "ril_utf_rild_sim.h"
#include "platform/android/NasAidlUtil.h"


namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace network {
class IRadioNetworkResponseImpl : public IRadioNetworkResponse {
public:
  IRadioNetworkResponseImpl() {}

  virtual ~IRadioNetworkResponseImpl() {}

  ::ndk::ScopedAStatus acknowledgeRequest(int32_t in_serial) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getAllowedNetworkTypesBitmapResponse(
      const ::aidlradio::RadioResponseInfo& in_info, int32_t in_networkTypeBitmap) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getAvailableBandModesResponse(
      const ::aidlradio::RadioResponseInfo& info,
      const std::vector<::aidlradio::network::RadioBandMode>& bandModes) override;

  ::ndk::ScopedAStatus getAvailableNetworksResponse(
      const ::aidlradio::RadioResponseInfo& info,
      const std::vector<::aidlradio::network::OperatorInfo>& networkInfos) override;

  ::ndk::ScopedAStatus getBarringInfoResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const ::aidlnetwork::CellIdentity& cellIdentity,
      const std::vector<::aidlnetwork::BarringInfo>& barringInfo) override;

  ::ndk::ScopedAStatus getCdmaRoamingPreferenceResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      ::aidlradio::network::CdmaRoamingType in_type) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getCellInfoListResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const std::vector<::aidlradio::network::CellInfo>& in_cellInfo) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getDataRegistrationStateResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const ::aidlradio::network::RegStateResult& in_dataRegResponse) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getImsRegistrationStateResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      bool in_isRegistered,
      ::aidlradio::RadioTechnologyFamily in_ratFamily) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getNetworkSelectionModeResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                                       bool in_manual) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getOperatorResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                           const std::string& in_longName,
                                           const std::string& in_shortName,
                                           const std::string& in_numeric) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getSignalStrengthResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const ::aidlradio::network::SignalStrength& in_signalStrength) override;

  ::ndk::ScopedAStatus getSystemSelectionChannelsResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const std::vector<::aidlradio::network::RadioAccessSpecifier>& in_specifiers) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getVoiceRadioTechnologyResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
      ::aidl::android::hardware::radio::RadioTechnology in_rat) override;

  ::ndk::ScopedAStatus getVoiceRegistrationStateResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const ::aidlradio::network::RegStateResult& in_voiceRegResponse) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus isNrDualConnectivityEnabledResponse(
      const ::aidlradio::RadioResponseInfo& in_info, bool in_isEnabled) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setAllowedNetworkTypesBitmapResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setBandModeResponse(const ::aidlradio::RadioResponseInfo& info) override;

  ::ndk::ScopedAStatus setBarringPasswordResponse(
      const ::aidlradio::RadioResponseInfo& info) override;

  ::ndk::ScopedAStatus setCdmaRoamingPreferenceResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setCellInfoListRateResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setIndicationFilterResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setLinkCapacityReportingCriteriaResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setLocationUpdatesResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setNetworkSelectionModeAutomaticResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setNetworkSelectionModeManualResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setNrDualConnectivityStateResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setSignalStrengthReportingCriteriaResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setSuppServiceNotificationsResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setSystemSelectionChannelsResponse(
      const ::aidlradio::RadioResponseInfo& info) override;

  ::ndk::ScopedAStatus startNetworkScanResponse(const ::aidlradio::RadioResponseInfo& info) override;

  ::ndk::ScopedAStatus stopNetworkScanResponse(const ::aidlradio::RadioResponseInfo& info) override;

  ::ndk::ScopedAStatus supplyNetworkDepersonalizationResponse(
      const ::aidlradio::RadioResponseInfo& in_info, int32_t in_remainingRetries) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setUsageSettingResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus getUsageSettingResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      ::aidlradio::network::UsageSetting in_usageSetting) override;

  ::ndk::ScopedAStatus setEmergencyModeResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
      const ::aidl::android::hardware::radio::network::EmergencyRegResult& in_regState) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus triggerEmergencyNetworkScanResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus exitEmergencyModeResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus cancelEmergencyNetworkScanResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setNullCipherAndIntegrityEnabledResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus isNullCipherAndIntegrityEnabledResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, bool in_isEnabled) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus isN1ModeEnabledResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, bool in_isEnabled) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setN1ModeEnabledResponse(
      const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override {
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
