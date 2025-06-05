/*===========================================================================
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/sim/IRadioSimResponse.h"
#include "ril_utf_rild_sim.h"
#include "ril_utf_if.h"

namespace aidlradio {
  using namespace ::aidl::android::hardware::radio;
}

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace sim {
class IRadioSimResponseImpl : public IRadioSimResponse {
public:
  IRadioSimResponseImpl() {}

  virtual ~IRadioSimResponseImpl() {}

  ::ndk::ScopedAStatus acknowledgeRequest(int32_t in_serial) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus areUiccApplicationsEnabledResponse(const aidlradio::RadioResponseInfo& in_info,
                                                          bool in_enabled) override;

  ::ndk::ScopedAStatus changeIccPin2ForAppResponse(const aidlradio::RadioResponseInfo& in_info,
                                                   int32_t in_remainingRetries) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus changeIccPinForAppResponse(const aidlradio::RadioResponseInfo& in_info,
                                                  int32_t in_remainingRetries) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus enableUiccApplicationsResponse(
      const aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getAllowedCarriersResponse(
      const aidlradio::RadioResponseInfo& in_info,
      const aidlradio::sim::CarrierRestrictions& in_carriers,
      aidlradio::sim::SimLockMultiSimPolicy in_multiSimPolicy) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getCdmaSubscriptionResponse(const aidlradio::RadioResponseInfo& in_info,
                                                   const std::string& in_mdn,
                                                   const std::string& in_hSid,
                                                   const std::string& in_hNid,
                                                   const std::string& in_min,
                                                   const std::string& in_prl) override;

  ::ndk::ScopedAStatus getCdmaSubscriptionSourceResponse(
      const aidlradio::RadioResponseInfo& in_info,
      aidlradio::sim::CdmaSubscriptionSource in_source) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getFacilityLockForAppResponse(const aidlradio::RadioResponseInfo& in_info,
                                                     int32_t in_response) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getIccCardStatusResponse(
      const aidlradio::RadioResponseInfo& in_info,
      const aidlradio::sim::CardStatus& in_cardStatus) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getImsiForAppResponse(const aidlradio::RadioResponseInfo& in_info,
                                             const std::string& in_imsi) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getSimPhonebookCapacityResponse(
      const aidlradio::RadioResponseInfo& in_info,
      const aidlradio::sim::PhonebookCapacity& in_capacity) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getSimPhonebookRecordsResponse(
      const aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus iccCloseLogicalChannelResponse(
      const aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus iccIoForAppResponse(const aidlradio::RadioResponseInfo& in_info,
                                           const aidlradio::sim::IccIoResult& in_iccIo) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus iccOpenLogicalChannelResponse(
      const aidlradio::RadioResponseInfo& in_info,
      int32_t in_channelId,
      const std::vector<uint8_t>& in_selectResponse) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus iccTransmitApduBasicChannelResponse(
      const aidlradio::RadioResponseInfo& in_info,
      const aidlradio::sim::IccIoResult& in_result) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus iccTransmitApduLogicalChannelResponse(
      const aidlradio::RadioResponseInfo& in_info,
      const aidlradio::sim::IccIoResult& in_result) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus reportStkServiceIsRunningResponse(
      const aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus requestIccSimAuthenticationResponse(
      const aidlradio::RadioResponseInfo& in_info,
      const aidlradio::sim::IccIoResult& in_result) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus sendEnvelopeResponse(const aidlradio::RadioResponseInfo& in_info,
                                            const std::string& in_commandResponse) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus sendEnvelopeWithStatusResponse(
      const aidlradio::RadioResponseInfo& in_info,
      const aidlradio::sim::IccIoResult& in_iccIo) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus sendTerminalResponseToSimResponse(
      const aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setAllowedCarriersResponse(const aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setCarrierInfoForImsiEncryptionResponse(
      const aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setCdmaSubscriptionSourceResponse(
      const aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setFacilityLockForAppResponse(const aidlradio::RadioResponseInfo& in_info,
                                                     int32_t in_retry) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setSimCardPowerResponse(const aidlradio::RadioResponseInfo& in_info) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus setUiccSubscriptionResponse(
      const aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus supplyIccPin2ForAppResponse(const aidlradio::RadioResponseInfo& in_info,
                                                   int32_t in_remainingRetries) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus supplyIccPinForAppResponse(const aidlradio::RadioResponseInfo& in_info,
                                                  int32_t in_remainingRetries) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus supplyIccPuk2ForAppResponse(const aidlradio::RadioResponseInfo& in_info,
                                                   int32_t in_remainingRetries) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus supplyIccPukForAppResponse(const aidlradio::RadioResponseInfo& in_info,
                                                  int32_t in_remainingRetries) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus supplySimDepersonalizationResponse(const aidlradio::RadioResponseInfo& in_info,
                                                          aidlradio::sim::PersoSubstate in_persoType,
                                                          int32_t in_remainingRetries) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus updateSimPhonebookRecordsResponse(const aidlradio::RadioResponseInfo& in_info,
                                                         int32_t in_updatedRecordIndex) override
  {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus iccCloseLogicalChannelWithSessionInfoResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) override {
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
