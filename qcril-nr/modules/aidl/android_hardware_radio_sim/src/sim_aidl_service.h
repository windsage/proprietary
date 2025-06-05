/*===========================================================================
   Copyright (c) 2022 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#pragma once

#include <framework/legacy.h>
#include <QtiMutex.h>
#include <binder/IBinder.h>
#include "BnRadioSimDefault.h"
#include "IRadioSimContext.h"

#include "interfaces/gstk/GstkUnsolIndMsg.h"
#include "interfaces/gstk/qcril_gstk_types.h"
#include "interfaces/nas/nas_types.h"
#include "interfaces/nas/RilUnsolCdmaSubscriptionSourceChangedMessage.h"
#include "interfaces/nas/RilUnsolUiccAppsStatusChangedMessage.h"
#include "interfaces/nas/RilUnsolUiccSubsStatusChangedMessage.h"
#include "interfaces/pbm/pbm.h"
#include "interfaces/pbm/QcRilUnsolAdnRecordsOnSimMessage.h"
#include "interfaces/pbm/QcRilUnsolPhonebookRecordsUpdatedMessage.h"
#include "interfaces/uim/qcril_uim_types.h"
#include "interfaces/uim/UimSimRefreshIndication.h"
#include "interfaces/uim/UimSimStatusChangedInd.h"

#include "UnSolMessages/CarrierInfoForImsiEncryptionRefreshMessage.h"

namespace aidlsim {
using namespace aidl::android::hardware::radio::sim;
}

namespace aidlradio {
using namespace aidl::android::hardware::radio;
}

class IRadioSimImpl : public aidlsim::BnRadioSimDefault {
 private:
  std::shared_ptr<aidlsim::IRadioSimResponse> mIRadioSimResponse;
  std::shared_ptr<aidlsim::IRadioSimIndication> mIRadioSimIndication;
  AIBinder_DeathRecipient* mDeathRecipient = nullptr;
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;

  // protected:FIXME
  std::shared_ptr<aidlsim::IRadioSimResponse> getResponseCallback();
  std::shared_ptr<aidlsim::IRadioSimIndication> getIndicationCallback();

  void sendResponseForChangeIccPin2ForApp(int32_t serial, aidlradio::RadioError errorCode,
                                          std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData);
  void sendResponseForChangeIccPinForApp(int32_t serial, aidlradio::RadioError errorCode,
                                         std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData);
  void sendResponseForAreUiccApplicationsEnabled(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetUiccAppStatusResult_t> result);
  void sendResponseForEnableUiccApplications(int32_t serial, RIL_Errno errorCode);
  void sendResponseForGetAllowedCarriers(int32_t serial, RIL_Errno errorCode);
  void sendResponseForGetCdmaSubscription(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetCdmaSubscriptionResult_t> rilCdmaSubResult);
  void sendResponseForGetCdmaSubscriptionSource(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetCdmaSubscriptionSourceResult_t> srcResult);
  void sendResponseForGetFacilityLockForApp(int32_t serial, RIL_Errno errorCode, int32_t response);
  void sendResponseForGetIccCardStatus(int32_t serial, aidlradio::RadioError errorCode,
                                       std::shared_ptr<RIL_UIM_CardStatus> respData);
  void sendResponseForGetImsiForApp(int32_t serial, aidlradio::RadioError errorCode,
                                    std::shared_ptr<RIL_UIM_IMSI_Response> respData);
  void sendResponseForGetSimPhonebookCapacity(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info> adnCountInfo);
  void sendResponseForGetSimPhonebookRecords(int32_t serial, RIL_Errno errorCode);
  void sendResponseForIccCloseLogicalChannel(int32_t serial, aidlradio::RadioError errorCode);
  void sendResponseForIccIOForApp(int32_t serial, aidlradio::RadioError errorCode,
                                  std::shared_ptr<RIL_UIM_SIM_IO_Response> respData);
  void sendResponseForIccOpenLogicalChannel(int32_t serial, aidlradio::RadioError errorCode,
                                            std::shared_ptr<RIL_UIM_OpenChannelResponse> respData);
  void sendResponseForIccTransmitApduBasicChannel(int32_t serial, aidlradio::RadioError errorCode,
                                                  std::shared_ptr<RIL_UIM_SIM_IO_Response> respData);
  void sendResponseForIccTransmitApduLogicalChannel(
      int32_t serial, aidlradio::RadioError errorCode,
      std::shared_ptr<RIL_UIM_SIM_IO_Response> respData);
  void sendResponseForReportStkServiceIsRunning(int32_t serial, aidlradio::RadioError errorCode);
  void sendResponseForRequestIccSimAuthentication(int32_t serial, aidlradio::RadioError errorCode,
                                                  std::shared_ptr<RIL_UIM_SIM_IO_Response> respData);
  void sendResponseForSendEnvelope(int32_t serial, aidlradio::RadioError errorCode,
                                   std::shared_ptr<RIL_GSTK_EnvelopeResponse> respData);
  void sendResponseForSendEnvelopeWithStatus(int32_t serial, aidlradio::RadioError errorCode,
                                             std::shared_ptr<RIL_GSTK_EnvelopeResponse> respData);
  void sendResponseForSendTerminalResponseToSim(int32_t serial, aidlradio::RadioError errorCode);
  void sendResponseForSetAllowedCarriers(int32_t serial, RIL_Errno errorCode);
  void sendCarrierInfoForImsiEncryptionResponse(aidlradio::RadioResponseInfo responseInfo);
  void sendResponseForSetCdmaSubscriptionSource(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetFacilityLockForApp(int32_t serial, RIL_Errno errorCode, int32_t retry);
  void sendResponseForSetSimCardPower(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetUiccSubscription(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSupplyIccPin2ForApp(int32_t serial, aidlradio::RadioError errorCode,
                                          std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData);
  void sendResponseForSupplyIccPinForApp(int32_t serial, aidlradio::RadioError errorCode,
                                         std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData);
  void sendResponseForSupplyIccPuk2ForApp(int32_t serial, aidlradio::RadioError errorCode,
                                          std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData);
  void sendResponseForSupplyIccPukForApp(int32_t serial, aidlradio::RadioError errorCode,
                                         std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData);
  void sendResponseForSimDepersonalization(int32_t serial, aidlradio::RadioError errorCode,
                                           aidlsim::PersoSubstate persotype, int retries);
  void sendResponseForUpdateSimPhonebookRecords(int32_t serial, RIL_Errno errorCode,
                                                int32_t recordIndex);
  void sendResponseForCloseLogicalChannelWithSessionInfoResponse(
      int32_t serial, aidlradio::RadioError errorCode);

public:
  IRadioSimImpl(qcril_instance_id_e_type instance);
  ~IRadioSimImpl();

  void setResponseFunctions_nolock(
      const std::shared_ptr<aidlsim::IRadioSimResponse>& radioSimResponse,
      const std::shared_ptr<aidlsim::IRadioSimIndication>& radioSimIndication);

  void clearCallbacks();

  qcril_instance_id_e_type getInstanceId() {
    return mInstanceId;
  }

  std::shared_ptr<IRadioSimContext> getContext(uint32_t serial) {
    std::shared_ptr<IRadioSimContext> ctx = std::make_shared<IRadioSimContext>(mInstanceId, serial);
    return ctx;
  }

  void notifySimClientConnected();
  void deathNotifier();

  // AIDL APIs
  ::ndk::ScopedAStatus setResponseFunctions(
      const std::shared_ptr<aidlsim::IRadioSimResponse>& in_radioSimResponse,
      const std::shared_ptr<aidlsim::IRadioSimIndication>& in_radioSimIndication) override;

  ::ndk::ScopedAStatus areUiccApplicationsEnabled(int32_t in_serial) override;
  ::ndk::ScopedAStatus changeIccPin2ForApp(int32_t in_serial, const std::string& in_oldPin2,
                                           const std::string& in_newPin2,
                                           const std::string& in_aid) override;
  ::ndk::ScopedAStatus changeIccPinForApp(int32_t in_serial, const std::string& in_oldPin,
                                          const std::string& in_newPin,
                                          const std::string& in_aid) override;

  ::ndk::ScopedAStatus enableUiccApplications(int32_t in_serial, bool in_enable) override;
  ::ndk::ScopedAStatus getAllowedCarriers(int32_t in_serial) override;
  ::ndk::ScopedAStatus getCdmaSubscription(int32_t in_serial) override;

  ::ndk::ScopedAStatus getCdmaSubscriptionSource(int32_t in_serial) override;

  ::ndk::ScopedAStatus getFacilityLockForApp(int32_t in_serial, const std::string& in_facility,
                                             const std::string& in_password, int32_t in_serviceClass,
                                             const std::string& in_appId) override;

  ::ndk::ScopedAStatus getIccCardStatus(int32_t in_serial) override;
  ::ndk::ScopedAStatus getImsiForApp(int32_t in_serial, const std::string& in_aid) override;
  ::ndk::ScopedAStatus getSimPhonebookCapacity(int32_t in_serial) override;
  ::ndk::ScopedAStatus getSimPhonebookRecords(int32_t in_serial) override;

  ::ndk::ScopedAStatus iccCloseLogicalChannel(int32_t in_serial, int32_t in_channelId) override;

  ::ndk::ScopedAStatus iccIoForApp(int32_t in_serial, const aidlsim::IccIo& in_iccIo) override;

  ::ndk::ScopedAStatus iccOpenLogicalChannel(int32_t in_serial, const std::string& in_aid,
                                             int32_t in_p2) override;

  ::ndk::ScopedAStatus iccTransmitApduBasicChannel(int32_t in_serial,
                                                   const aidlsim::SimApdu& in_message) override;

  ::ndk::ScopedAStatus iccTransmitApduLogicalChannel(int32_t in_serial,
                                                     const aidlsim::SimApdu& in_message) override;
  ::ndk::ScopedAStatus reportStkServiceIsRunning(int32_t in_serial) override;
  ::ndk::ScopedAStatus requestIccSimAuthentication(int32_t in_serial, int32_t in_authContext,
                                                   const std::string& in_authData,
                                                   const std::string& in_aid) override;

  ::ndk::ScopedAStatus responseAcknowledgement() override;
  ::ndk::ScopedAStatus sendEnvelope(int32_t in_serial, const std::string& in_command) override;
  ::ndk::ScopedAStatus sendEnvelopeWithStatus(int32_t in_serial,
                                              const std::string& in_contents) override;
  ::ndk::ScopedAStatus sendTerminalResponseToSim(int32_t in_serial,
                                                 const std::string& in_commandResponse) override;
  ::ndk::ScopedAStatus setAllowedCarriers(int32_t in_serial,
                                          const aidlsim::CarrierRestrictions& in_carriers,
                                          aidlsim::SimLockMultiSimPolicy in_multiSimPolicy) override;
  ::ndk::ScopedAStatus setCarrierInfoForImsiEncryption(
      int32_t in_serial, const aidlsim::ImsiEncryptionInfo& in_imsiEncryptionInfo) override;
  ::ndk::ScopedAStatus setCdmaSubscriptionSource(int32_t in_serial,
                                                 aidlsim::CdmaSubscriptionSource in_cdmaSub) override;
  ::ndk::ScopedAStatus setFacilityLockForApp(int32_t in_serial, const std::string& in_facility,
                                             bool in_lockState, const std::string& in_password,
                                             int32_t in_serviceClass,
                                             const std::string& in_appId) override;
  ::ndk::ScopedAStatus setSimCardPower(int32_t in_serial,
                                       aidlsim::CardPowerState in_powerUp) override;
  ::ndk::ScopedAStatus setUiccSubscription(int32_t in_serial,
                                           const aidlsim::SelectUiccSub& in_uiccSub) override;
  ::ndk::ScopedAStatus supplyIccPin2ForApp(int32_t in_serial, const std::string& in_pin2,
                                           const std::string& in_aid) override;
  ::ndk::ScopedAStatus supplyIccPinForApp(int32_t in_serial, const std::string& in_pin,
                                          const std::string& in_aid) override;
  ::ndk::ScopedAStatus supplyIccPuk2ForApp(int32_t in_serial, const std::string& in_puk2,
                                           const std::string& in_pin2,
                                           const std::string& in_aid) override;
  ::ndk::ScopedAStatus supplyIccPukForApp(int32_t in_serial, const std::string& in_puk,
                                          const std::string& in_pin,
                                          const std::string& in_aid) override;
  ::ndk::ScopedAStatus supplySimDepersonalization(int32_t in_serial,
                                                  aidlsim::PersoSubstate in_persoType,
                                                  const std::string& in_controlKey) override;
  ::ndk::ScopedAStatus updateSimPhonebookRecords(
      int32_t in_serial, const aidlsim::PhonebookRecordInfo& in_recordInfo) override;

  ::ndk::ScopedAStatus iccCloseLogicalChannelWithSessionInfo(
      int32_t serial, const aidlsim::SessionInfo &sessionInfo) override;

  int sendCarrierInfoForImsiEncryptionInd(std::shared_ptr<rildata::CarrierInfoForImsiEncryptionRefreshMessage> msg);
  int sendCdmaSubscriptionSourceChanged(
      std::shared_ptr<RilUnsolCdmaSubscriptionSourceChangedMessage> msg);
  int sendSimPhonebookChanged(std::shared_ptr<QcRilUnsolPhonebookRecordsUpdatedMessage> msg);
  int sendSimPhonebookRecords(std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg);
  int sendSimRefresh(std::shared_ptr<UimSimRefreshIndication> msg);
  int sendSimStatusChanged(std::shared_ptr<UimSimStatusChangedInd> msg);
  int sendGstkIndication(std::shared_ptr<GstkUnsolIndMsg> msg);
  int sendUiccSubsStatusChanged(std::shared_ptr<RilUnsolUiccSubsStatusChangedMessage> msg);
  int sendUiccAppsStatusChanged(std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg);
};
