/******************************************************************************
#  Copyright (c) 2020,2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RIL_SERVICE_BASE_H__
#define __RIL_SERVICE_BASE_H__

#include "QtiMutex.h"
#include "framework/legacy.h"
#include "hidl/HidlSupport.h"

#include "RadioContext.h"

#include "interfaces/RilAcknowledgeRequestMessage.h"
#include "interfaces/RilUnsolRilConnectedMessage.h"
#include "interfaces/gstk/GstkUnsolIndMsg.h"
#include "interfaces/nas/RilUnsolNetworkStateChangedMessage.h"
#include "interfaces/nas/RilUnsolNitzTimeReceivedMessage.h"
#include "interfaces/nas/RilUnsolVoiceRadioTechChangedMessage.h"
#include "interfaces/nas/RilUnsolSignalStrengthMessage.h"
#include "interfaces/nas/RilUnsolEmergencyCallbackModeMessage.h"
#include "interfaces/nas/RilUnsolRadioCapabilityMessage.h"
#include "interfaces/nas/RilUnsolCdmaPrlChangedMessage.h"
#include "interfaces/nas/RilUnsolCdmaSubscriptionSourceChangedMessage.h"
#include "interfaces/nas/RilUnsolRestrictedStateChangedMessage.h"
#include "interfaces/nas/RilUnsolUiccSubsStatusChangedMessage.h"
#include "interfaces/nas/RilUnsolRadioStateChangedMessage.h"
#include "interfaces/nas/RilUnsolModemRestartMessage.h"
#include "interfaces/nas/RilUnsolNetworkScanResultMessage.h"
#include "interfaces/nas/RilUnsolEmergencyListIndMessage.h"
#include "interfaces/nas/RilUnsolUiccAppsStatusChangedMessage.h"
#include "interfaces/nas/RilUnsolCellBarringMessage.h"
#include "interfaces/cellinfo/RilUnsolCellInfoListMessage.h"
#include "interfaces/NwRegistration/RilUnsolNwRegistrationRejectMessage.h"
#include "interfaces/pbm/QcRilRequestGetAdnCapacityMessage.h"
#include "interfaces/pbm/QcRilRequestUpdatePhonebookRecordsMessage.h"
#include "interfaces/pbm/QcRilUnsolPhonebookRecordsUpdatedMessage.h"
#include "interfaces/pbm/QcRilUnsolAdnRecordsOnSimMessage.h"
#include "interfaces/pbm/QcRilRequestGetPhonebookRecordsMessage.h"
#include "interfaces/sms/RilUnsolIncoming3GppSMSMessage.h"
#include "interfaces/sms/RilUnsolIncoming3Gpp2SMSMessage.h"
#include "interfaces/sms/RilUnsolSimSmsStorageFullMessage.h"
#include "interfaces/sms/RilUnsolCdmaRuimSmsStorageFullMessage.h"
#include "interfaces/sms/RilUnsolNewSmsOnSimMessage.h"
#include "interfaces/sms/RilUnsolNewSmsStatusReportMessage.h"
#include "interfaces/sms/RilUnsolNewBroadcastSmsMessage.h"
#include "interfaces/sms/RilUnsolStkCCAlphaNotifyMessage.h"
#include "interfaces/sms/RilUnsolImsNetworkStateChangedMessage.h"
#include "interfaces/uim/UimSimStatusChangedInd.h"
#include "interfaces/uim/UimSimRefreshIndication.h"
#include "interfaces/voice/QcRilUnsolCallStateChangeMessage.h"
#include "interfaces/voice/QcRilUnsolCallRingingMessage.h"
#include "interfaces/voice/QcRilUnsolSupplementaryServiceMessage.h"
#include "interfaces/voice/QcRilUnsolSrvccStatusMessage.h"
#include "interfaces/voice/QcRilUnsolRingbackToneMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaOtaProvisionStatusMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaCallWaitingMessage.h"
#include "interfaces/voice/QcRilUnsolSuppSvcNotificationMessage.h"
#include "interfaces/voice/QcRilUnsolOnUssdMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaInfoRecordMessage.h"

#include "modules/android/ClientDisconnectedMessage.h"
#include "request/DeactivateDataCallRequestMessage.h"
#include "request/SetupDataCallRequestMessage.h"
#include "request/SetLinkCapRptCriteriaMessage.h"
#include "request/SetInitialAttachApnRequestMessage.h"
#include "request/SetLinkCapFilterMessage.h"
#include "request/SetDataProfileRequestMessage.h"
#include "request/SetCarrierInfoImsiEncryptionMessage.h"
#include "request/StartLCERequestMessage.h"
#include "request/StopLCERequestMessage.h"
#include "request/StartKeepAliveRequestMessage.h"
#include "request/StopKeepAliveRequestMessage.h"
#include "request/PullLCEDataRequestMessage.h"
#include "request/GetRadioDataCallListRequestMessage.h"
#include "request/SetDataThrottlingRequestMessage.h"
#include "UnSolMessages/LinkCapIndMessage.h"
#include "UnSolMessages/RadioKeepAliveStatusIndMessage.h"
#include "UnSolMessages/RadioDataCallListChangeIndMessage.h"
#include "UnSolMessages/CarrierInfoForImsiEncryptionRefreshMessage.h"
#include "UnSolMessages/RadioKeepAliveStatusIndMessage.h"
#include "UnSolMessages/PhysicalConfigStructUpdateMessage.h"
#include "UnSolMessages/ThrottledApnTimerExpirationMessage.h"
#include "UnSolMessages/RilPCODataMessage.h"
#include "UnSolMessages/GetDataCallListResponseIndMessage.h"


class RadioServiceBase : public ::android::hardware::hidl_death_recipient {
 private:
  qcril_instance_id_e_type mInstanceId;

 protected:
  qtimutex::QtiSharedMutex mCallbackLock;

  /**
   * Clean up the callback handlers.
   */
  virtual void clearCallbacks() = 0;

  /**
   * Handler function for hidl death notification.
   * From hidl_death_recipient
   */
  void serviceDied(uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase>&);

  // Utils APIs
  /**
   * Generate RadioServiceContext
   */
  std::shared_ptr<RadioServiceContext> getContext(uint32_t serial);

  /**
   * Set instance id
   */
  void setInstanceId(qcril_instance_id_e_type instId);

  /**
   * Returns the instance id
   */
  qcril_instance_id_e_type getInstanceId();

 public:
  RadioServiceBase() = default;

  virtual ~RadioServiceBase() = default;

  /**
   * Register the latest version of the service.
   */
  virtual bool registerService(qcril_instance_id_e_type instId) = 0;

  /**
   * Notifies acknowledgeRequest response.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioResponse::acknowledgeRequest
   */
  virtual int sendAcknowledgeRequest(std::shared_ptr<RilAcknowledgeRequestMessage> /*msg*/) {
    return 1;
  }


  /**
   * Notifies rilConnected indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::rilConnected
   */
  virtual int sendRilConnected(std::shared_ptr<RilUnsolRilConnectedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies newSms indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::newSms
   */
  virtual int sendNewSms(std::shared_ptr<RilUnsolIncoming3GppSMSMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies cdmaNewSms indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaNewSms
   */
  virtual int sendNewCdmaSms(std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies newSmsOnSim indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::newSmsOnSim
   */
  virtual int sendNewSmsOnSim(std::shared_ptr<RilUnsolNewSmsOnSimMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies newSmsStatusReport indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::newSmsStatusReport
   */
  virtual int sendNewSmsStatusReport(std::shared_ptr<RilUnsolNewSmsStatusReportMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies newBroadcastSms indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::newBroadcastSms
   */
  virtual int sendNewBroadcastSms(std::shared_ptr<RilUnsolNewBroadcastSmsMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies stkCallControlAlphaNotify indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::stkCallControlAlphaNotify
   */
  virtual int sendStkCCAlphaNotify(std::shared_ptr<RilUnsolStkCCAlphaNotifyMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies cdmaRuimSmsStorageFull indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaRuimSmsStorageFull
   */
  virtual int sendCdmaRuimSmsStorageFull(
      std::shared_ptr<RilUnsolCdmaRuimSmsStorageFullMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies simSmsStorageFull indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::simSmsStorageFull
   */
  virtual int sendSimSmsStorageFull(std::shared_ptr<RilUnsolSimSmsStorageFullMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies imsNetworkStateChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::imsNetworkStateChanged
   */
  virtual int sendImsNetworkStateChanged(
      std::shared_ptr<RilUnsolImsNetworkStateChangedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies callRing indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::callRing
   */
  virtual int sendCallRing(std::shared_ptr<QcRilUnsolCallRingingMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies onUssd indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::onUssd
   */
  virtual int sendOnUssd(std::shared_ptr<QcRilUnsolOnUssdMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies suppSvcNotify indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::suppSvcNotify
   */
  virtual int sendSuppSvcNotify(std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies cdmaCallWaiting indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaCallWaiting
   */
  virtual int sendCdmaCallWaiting(std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies cdmaOtaProvisionStatus indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaOtaProvisionStatus
   */
  virtual int sendCdmaOtaProvisionStatus(
      std::shared_ptr<QcRilUnsolCdmaOtaProvisionStatusMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies indicateRingbackTone indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::indicateRingbackTone
   */
  virtual int sendIndicateRingbackTone(std::shared_ptr<QcRilUnsolRingbackToneMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies srvccStateNotify indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::srvccStateNotify
   */
  virtual int sendSrvccStateNotify(std::shared_ptr<QcRilUnsolSrvccStatusMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies onSupplementaryServiceIndication indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::onSupplementaryServiceIndication
   */
  virtual int sendOnSupplementaryServiceIndication(
      std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies callStateChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::callStateChanged
   */
  virtual int sendCallStateChanged(std::shared_ptr<QcRilUnsolCallStateChangeMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies cdmaInfoRec indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaInfoRec
   */
  virtual int sendCdmaInfoRec(std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies networkStateChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::networkStateChanged
   */
  virtual int sendNetworkStateChanged(std::shared_ptr<RilUnsolNetworkStateChangedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies nitzTimeReceived indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::nitzTimeReceived
   */
  virtual int sendNitzTimeReceived(std::shared_ptr<RilUnsolNitzTimeReceivedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies voiceRadioTechChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::voiceRadioTechChanged
   */
  virtual int sendVoiceRadioTechChanged(
      std::shared_ptr<RilUnsolVoiceRadioTechChangedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies cellInfoList indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cellInfoList
   *   V1_2::IRadioIndication::cellInfoList_1_2
   *   V1_4::IRadioIndication::cellInfoList_1_4
   *   V1_5::IRadioIndication::cellInfoList_1_5
   *   V1_6::IRadioIndication::cellInfoList_1_6
   */
  virtual int sendCellInfoList(std::shared_ptr<RilUnsolCellInfoListMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies currentSignalStrength indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   * V1_6::IRadioIndication::currentSignalStrength_1_6
   * V1_4::IRadioIndication::currentSignalStrength_1_4
   * V1_2::IRadioIndication::currentSignalStrength_1_2
   * V1_0::IRadioIndication::currentSignalStrength
   */
  virtual int sendSignalStrength(std::shared_ptr<RilUnsolSignalStrengthMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies Emergency Callback Mode indications.
   *   V1_0::IRadioIndication::enterEmergencyCallbackMode
   *   V1_0::IRadioIndication::exitEmergencyCallbackMode
   */
  virtual int sendEmergencyCallbackMode(
      std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies radioCapabilityIndication indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::radioCapabilityIndication
   */
  virtual int sendRadioCapability(std::shared_ptr<RilUnsolRadioCapabilityMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies cdmaSubscriptionSourceChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaSubscriptionSourceChanged
   */
  virtual int sendCdmaSubscriptionSourceChanged(
      std::shared_ptr<RilUnsolCdmaSubscriptionSourceChangedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies cdmaPrlChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaPrlChanged
   */
  virtual int sendCdmaPrlChanged(std::shared_ptr<RilUnsolCdmaPrlChangedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies restrictedStateChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::restrictedStateChanged
   */
  virtual int sendRestrictedStateChanged(
      std::shared_ptr<RilUnsolRestrictedStateChangedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies subscriptionStatusChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::subscriptionStatusChanged
   */
  virtual int sendUiccSubsStatusChanged(
      std::shared_ptr<RilUnsolUiccSubsStatusChangedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies radioStateChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::radioStateChanged
   */
  virtual int sendRadioStateChanged(std::shared_ptr<RilUnsolRadioStateChangedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies modemReset indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::modemReset
   */
  virtual int sendModemReset(std::shared_ptr<RilUnsolModemRestartMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies simStatusChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::simStatusChanged
   */
  virtual int sendSimStatusChanged(std::shared_ptr<UimSimStatusChangedInd> /*msg*/) {
    return 1;
  }

  /**
   * Notifies simRefresh indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::simRefresh
   */
  virtual int sendSimRefresh(std::shared_ptr<UimSimRefreshIndication> /*msg*/) {
    return 1;
  }

  /**
   * Notifies STK related indications.
   *   V1_0::IRadioIndication::stkCallSetup
   *   V1_0::IRadioIndication::stkEventNotify
   *   V1_0::IRadioIndication::stkProactiveCommand
   *   V1_0::IRadioIndication::stkSessionEnd
   */
  virtual int sendGstkIndication(std::shared_ptr<GstkUnsolIndMsg> /*msg*/) {
    return 1;
  }

  /**
   * Notifies networkScanResult indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_6::IRadioIndication::networkScanResult_1_6
   *   V1_5::IRadioIndication::networkScanResult_1_5
   *   V1_4::IRadioIndication::networkScanResult_1_4
   *   V1_2::IRadioIndication::networkScanResult_1_2
   *   V1_1::IRadioIndication::networkScanResult
   */
  virtual int sendNetworkScanResult(std::shared_ptr<RilUnsolNetworkScanResultMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies currentEmergencyNumberList indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_4::IRadioIndication::currentEmergencyNumberList
   */
  virtual int sendCurrentEmergencyNumberList(std::shared_ptr<RilUnsolEmergencyListIndMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies uiccApplicationsEnablementChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_5::IRadioIndication::uiccApplicationsEnablementChanged
   */
  virtual int sendUiccAppsStatusChanged(std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies registrationFailed indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_5::IRadioIndication::registrationFailed
   */
  virtual int sendNwRegistrationReject(std::shared_ptr<RilUnsolNwRegistrationRejectMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies barringInfoChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_5::IRadioIndication::barringInfoChanged
   */
  virtual int sendCellBarring(std::shared_ptr<RilUnsolCellBarringMessage> /*msg*/) {
    return 1;
  }
  /**
   * Notifies SetupDataCallRequest Response
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   */
  virtual int sendSetupDataCallResponse(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> /*msg*/) {
    return 1;
  }
  /**
   * Notifies DeactivateDataCallRequest Response
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   */
  virtual int sendDeactivateDataCallResponse(std::shared_ptr<rildata::DeactivateDataCallRadioResponseIndMessage> /*msg*/) {
    return 1;
  }
  /**
   * Notifies RadioDataCallListChange Indication
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   */
  virtual int sendRadioDataCallListChangeInd(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> /*msg*/) {
    return 1;
  }
  /**
   * Notifies CarrierInfoForImsiEncryption Indication
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   */
  virtual int sendCarrierInfoForImsiEncryptionInd(std::shared_ptr<Message> /*msg*/) {
    return 1;
  }
  /**
   * Notifies RadioKeepAliveStatusIndMessage Indication
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   */
  virtual int sendKeepAliveStatusInd(std::shared_ptr<rildata::RadioKeepAliveStatusIndMessage> /*msg*/) {
    return 1;
  }
  /**
   * Notifies LinkCapIndMessage Indication
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   */
  virtual int sendLinkCapInd(std::shared_ptr<rildata::LinkCapIndMessage> /*msg*/) {
    return 1;
  }
  /**
   * Notifies Pco Data Indication
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   */
  virtual int sendRilPCODataInd(std::shared_ptr<rildata::RilPCODataMessage> /*msg*/) {
    return 1;
  }
  /**
   * Notifies PhysicalConfigStructUpdateMessage Indication
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   */
  virtual int sendPhysicalConfigStructUpdateInd(std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> /*msg*/) {
    return 1;
  }

  /**
   * Notifies simPhonebookChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_6::IRadioIndication::simPhonebookChanged
   */
  virtual int sendSimPhonebookChanged(std::shared_ptr<QcRilUnsolPhonebookRecordsUpdatedMessage>  /*msg*/) {
    return 1;
  }

  virtual int sendSimPhonebookRecords(std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage>  /*msg*/) {
    return 1;
  }

  /**
   * Notifies unthrottleApn Indication
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_6::IRadioIndication::unthrottleApn
   */
  virtual int sendUnthrottleApn(std::shared_ptr<rildata::ThrottledApnTimerExpirationMessage> msg) {
    return 1;
  }
#ifdef QMI_RIL_UTF
  virtual int handleQcrilInit() {
    return 1;
  }
#endif

  /**
   * Sends response for GetDataCallList query message
   */
  virtual void getDataCallListResponse(std::shared_ptr<rildata::DataCallListResult_t> resp, int32_t serial, Message::Callback::Status status) {
  }
};

#endif  // __RIL_SERVICE_BASE_H__
