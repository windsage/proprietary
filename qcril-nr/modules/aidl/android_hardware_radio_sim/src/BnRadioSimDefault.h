/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "aidl/android/hardware/radio/sim/BnRadioSim.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace sim {
class BnRadioSimDefault : public BnRadioSim {
 public:
  BnRadioSimDefault() {
  }

  virtual ~BnRadioSimDefault() {
  }

  ::ndk::ScopedAStatus areUiccApplicationsEnabled(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus changeIccPin2ForApp(int32_t in_serial, const std::string& in_oldPin2,
                                           const std::string& in_newPin2,
                                           const std::string& in_aid) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus changeIccPinForApp(int32_t in_serial, const std::string& in_oldPin,
                                          const std::string& in_newPin,
                                          const std::string& in_aid) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus enableUiccApplications(int32_t in_serial, bool in_enable) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getAllowedCarriers(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getCdmaSubscription(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getCdmaSubscriptionSource(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getFacilityLockForApp(int32_t in_serial, const std::string& in_facility,
                                             const std::string& in_password, int32_t in_serviceClass,
                                             const std::string& in_appId) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getIccCardStatus(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getImsiForApp(int32_t in_serial, const std::string& in_aid) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getSimPhonebookCapacity(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getSimPhonebookRecords(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus iccCloseLogicalChannel(int32_t in_serial, int32_t in_channelId) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus iccIoForApp(
      int32_t in_serial, const ::aidl::android::hardware::radio::sim::IccIo& in_iccIo) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus iccOpenLogicalChannel(int32_t in_serial, const std::string& in_aid,
                                             int32_t in_p2) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus iccTransmitApduBasicChannel(
      int32_t in_serial, const ::aidl::android::hardware::radio::sim::SimApdu& in_message) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus iccTransmitApduLogicalChannel(
      int32_t in_serial, const ::aidl::android::hardware::radio::sim::SimApdu& in_message) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus reportStkServiceIsRunning(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus requestIccSimAuthentication(int32_t in_serial, int32_t in_authContext, const std::string& in_authData, const std::string& in_aid) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus responseAcknowledgement() override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus sendEnvelope(int32_t in_serial, const std::string& in_contents) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus sendEnvelopeWithStatus(int32_t in_serial,
                                              const std::string& in_contents) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus sendTerminalResponseToSim(int32_t in_serial, const std::string& in_contents) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setAllowedCarriers(
      int32_t in_serial,
      const ::aidl::android::hardware::radio::sim::CarrierRestrictions& in_carriers,
      ::aidl::android::hardware::radio::sim::SimLockMultiSimPolicy in_multiSimPolicy) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setCarrierInfoForImsiEncryption(
      int32_t in_serial,
      const ::aidl::android::hardware::radio::sim::ImsiEncryptionInfo& in_imsiEncryptionInfo)
      override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setCdmaSubscriptionSource(
      int32_t in_serial,
      ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource in_cdmaSub) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setFacilityLockForApp(int32_t in_serial, const std::string& in_facility,
                                             bool in_lockState, const std::string& in_password,
                                             int32_t in_serviceClass,
                                             const std::string& in_appId) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setResponseFunctions(
      const std::shared_ptr<::aidl::android::hardware::radio::sim::IRadioSimResponse>&
          in_radioSimResponse,
      const std::shared_ptr<::aidl::android::hardware::radio::sim::IRadioSimIndication>&
          in_radioSimIndication) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setSimCardPower(
      int32_t in_serial, ::aidl::android::hardware::radio::sim::CardPowerState in_powerUp) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setUiccSubscription(
      int32_t in_serial,
      const ::aidl::android::hardware::radio::sim::SelectUiccSub& in_uiccSub) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus supplyIccPin2ForApp(int32_t in_serial, const std::string& in_pin2,
                                           const std::string& in_aid) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus supplyIccPinForApp(int32_t in_serial, const std::string& in_pin,
                                          const std::string& in_aid) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus supplyIccPuk2ForApp(int32_t in_serial, const std::string& in_puk2,
                                           const std::string& in_pin2,
                                           const std::string& in_aid) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus supplyIccPukForApp(int32_t in_serial, const std::string& in_puk,
                                          const std::string& in_pin,
                                          const std::string& in_aid) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus supplySimDepersonalization(
      int32_t in_serial, ::aidl::android::hardware::radio::sim::PersoSubstate in_persoType,
      const std::string& in_controlKey) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus updateSimPhonebookRecords(
      int32_t in_serial,
      const ::aidl::android::hardware::radio::sim::PhonebookRecordInfo& in_recordInfo) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus iccCloseLogicalChannelWithSessionInfo(
      int32_t in_serial,
      const ::aidl::android::hardware::radio::sim::SessionInfo& in_sessionInfo) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
};
}  // namespace sim
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
