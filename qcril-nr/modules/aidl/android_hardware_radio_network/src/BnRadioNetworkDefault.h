/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "aidl/android/hardware/radio/network/BnRadioNetwork.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace network {
class BnRadioNetworkDefault : public BnRadioNetwork {
public:
  BnRadioNetworkDefault() {}

  virtual ~BnRadioNetworkDefault() {}

  ::ndk::ScopedAStatus getAllowedNetworkTypesBitmap(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getAvailableBandModes(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getAvailableNetworks(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getBarringInfo(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getCdmaRoamingPreference(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getCellInfoList(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getDataRegistrationState(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getImsRegistrationState(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getNetworkSelectionMode(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getOperator(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getSignalStrength(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getSystemSelectionChannels(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getVoiceRadioTechnology(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getVoiceRegistrationState(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus isNrDualConnectivityEnabled(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus responseAcknowledgement() override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setAllowedNetworkTypesBitmap(int32_t in_serial, int32_t in_networkTypeBitmap) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setBandMode(int32_t in_serial, ::aidl::android::hardware::radio::network::RadioBandMode in_mode) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setBarringPassword(int32_t in_serial, const std::string& in_facility, const std::string& in_oldPassword, const std::string& in_newPassword) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setCdmaRoamingPreference(int32_t in_serial, ::aidl::android::hardware::radio::network::CdmaRoamingType in_type) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setCellInfoListRate(int32_t in_serial, int32_t in_rate) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setIndicationFilter(int32_t in_serial, int32_t in_indicationFilter) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setLinkCapacityReportingCriteria(int32_t in_serial, int32_t in_hysteresisMs, int32_t in_hysteresisDlKbps, int32_t in_hysteresisUlKbps, const std::vector<int32_t>& in_thresholdsDownlinkKbps, const std::vector<int32_t>& in_thresholdsUplinkKbps, ::aidl::android::hardware::radio::AccessNetwork in_accessNetwork) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setLocationUpdates(int32_t in_serial, bool in_enable) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setNetworkSelectionModeAutomatic(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setNetworkSelectionModeManual(int32_t in_serial, const std::string& in_operatorNumeric, ::aidl::android::hardware::radio::AccessNetwork in_ran) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setNrDualConnectivityState(int32_t in_serial, ::aidl::android::hardware::radio::network::NrDualConnectivityState in_nrDualConnectivityState) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setResponseFunctions(const std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkResponse>& in_radioNetworkResponse, const std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkIndication>& in_radioNetworkIndication) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setSignalStrengthReportingCriteria(int32_t in_serial, const std::vector<::aidl::android::hardware::radio::network::SignalThresholdInfo>& in_signalThresholdInfos) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setSuppServiceNotifications(int32_t in_serial, bool in_enable) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setSystemSelectionChannels(int32_t in_serial, bool in_specifyChannels, const std::vector<::aidl::android::hardware::radio::network::RadioAccessSpecifier>& in_specifiers) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus startNetworkScan(int32_t in_serial, const ::aidl::android::hardware::radio::network::NetworkScanRequest& in_request) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus stopNetworkScan(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus supplyNetworkDepersonalization(int32_t in_serial, const std::string& in_netPin) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setUsageSetting(int32_t in_serial, ::aidl::android::hardware::radio::network::UsageSetting in_usageSetting) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getUsageSetting(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setEmergencyMode(int32_t in_serial, ::aidl::android::hardware::radio::network::EmergencyMode in_emcModeType) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus triggerEmergencyNetworkScan(
      int32_t in_serial, const ::aidl::android::hardware::radio::network::EmergencyNetworkScanTrigger& in_request) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus cancelEmergencyNetworkScan(int32_t in_serial, bool in_resetScan) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus exitEmergencyMode(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setNullCipherAndIntegrityEnabled(int32_t in_serial, bool in_enabled) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus isNullCipherAndIntegrityEnabled(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus isN1ModeEnabled(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setN1ModeEnabled(int32_t in_serial, bool in_enable) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
};
}  // namespace network
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
