/******************************************************************************
#  Copyright (c) 2018-2021,2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "HalServiceImplFactory.h"
#include "RadioServiceModule.h"
#include "framework/Log.h"

namespace android::hardware::radio::utils {
bool isUssdOverImsSupported();
}

static load_module<RadioServiceModule> sRadioService;

RadioServiceModule& getRadioServiceModule() {
  return (sRadioService.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
RadioServiceModule::RadioServiceModule() {
  mName = "RadioServiceModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, RadioServiceModule::handleQcrilInit),
    HANDLER(SsrIndicationMessage, RadioServiceModule::handleSsrIndicationMessage),
    HANDLER(RilUnsolRilConnectedMessage, RadioServiceModule::handleRilUnsolRilConnectedMessage),
    HANDLER(RilUnsolIncoming3GppSMSMessage, RadioServiceModule::handleIncoming3GppSMSMessage),
    HANDLER(RilUnsolIncoming3Gpp2SMSMessage, RadioServiceModule::handleIncoming3Gpp2SMSMessage),
    HANDLER(RilUnsolNewSmsOnSimMessage, RadioServiceModule::handleNewSmsOnSimMessage),
    HANDLER(RilUnsolNewSmsStatusReportMessage, RadioServiceModule::handleNewSmsStatusReportMessage),
    HANDLER(RilUnsolNewBroadcastSmsMessage, RadioServiceModule::handleNewBroadcastSmsMessage),
    HANDLER(RilUnsolStkCCAlphaNotifyMessage, RadioServiceModule::handleStkCCAlphaNotifyMessage),
    HANDLER(RilUnsolCdmaRuimSmsStorageFullMessage,
            RadioServiceModule::handleCdmaRuimSmsStorageFullMessage),
    HANDLER(RilUnsolSimSmsStorageFullMessage, RadioServiceModule::handleSimSmsStorageFullMessage),
    HANDLER(RilUnsolImsNetworkStateChangedMessage,
            RadioServiceModule::handleImsNetworkStateChangedMessage),
    HANDLER(QcRilUnsolCallStateChangeMessage,
            RadioServiceModule::handleQcRilUnsolCallStateChangeMessage),
    HANDLER(QcRilUnsolCallRingingMessage, RadioServiceModule::handleQcRilUnsolCallRingingMessage),
    HANDLER(QcRilUnsolSupplementaryServiceMessage,
            RadioServiceModule::handleQcRilUnsolSupplementaryServiceMessage),
    HANDLER(QcRilUnsolSrvccStatusMessage, RadioServiceModule::handleQcRilUnsolSrvccStatusMessage),
    HANDLER(QcRilUnsolRingbackToneMessage, RadioServiceModule::handleQcRilUnsolRingbackToneMessage),
    HANDLER(QcRilUnsolCdmaOtaProvisionStatusMessage,
            RadioServiceModule::handleQcRilUnsolCdmaOtaProvisionStatusMessage),
    HANDLER(QcRilUnsolCdmaCallWaitingMessage,
            RadioServiceModule::handleQcRilUnsolCdmaCallWaitingMessage),
    HANDLER(QcRilUnsolSuppSvcNotificationMessage,
            RadioServiceModule::handleQcRilUnsolSuppSvcNotificationMessage),
    HANDLER(QcRilUnsolOnUssdMessage, RadioServiceModule::handleQcRilUnsolOnUssdMessage),
    HANDLER(QcRilUnsolCdmaInfoRecordMessage,
            RadioServiceModule::handleQcRilUnsolCdmaInfoRecordMessage),
    HANDLER(RilAcknowledgeRequestMessage, RadioServiceModule::handleAcknowledgeRequestMessage),
    HANDLER(RilUnsolNetworkStateChangedMessage,
            RadioServiceModule::handleNetworkStateChangedMessage),
    HANDLER(RilUnsolNitzTimeReceivedMessage, RadioServiceModule::handleNitzTimeReceivedMessage),
    HANDLER(RilUnsolVoiceRadioTechChangedMessage,
            RadioServiceModule::handleVoiceRadioTechChangedMessage),
    HANDLER(RilUnsolSignalStrengthMessage, RadioServiceModule::handleSignalStrengthMessage),
    HANDLER(RilUnsolEmergencyCallbackModeMessage,
            RadioServiceModule::handleEmergencyCallbackModeMessage),
    HANDLER(RilUnsolRadioCapabilityMessage, RadioServiceModule::handlelRadioCapabilityMessage),
    HANDLER(RilUnsolCdmaPrlChangedMessage, RadioServiceModule::handleCdmaPrlChangedMessage),
    HANDLER(RilUnsolRestrictedStateChangedMessage,
            RadioServiceModule::handleRestrictedStateChangedMessage),
    HANDLER(RilUnsolUiccSubsStatusChangedMessage,
            RadioServiceModule::handleUiccSubsStatusChangedMessage),
    HANDLER(RilUnsolRadioStateChangedMessage, RadioServiceModule::handleRadioStateChangedMessage),
    HANDLER(RilUnsolModemRestartMessage, RadioServiceModule::handleModemRestartMessage),
    HANDLER(RilUnsolCdmaSubscriptionSourceChangedMessage,
            RadioServiceModule::handleCdmaSubscriptionSourceChangedMessage),
    HANDLER(RilUnsolEmergencyListIndMessage, RadioServiceModule::handleEmergencyListIndMessage),
    HANDLER(RilUnsolCellInfoListMessage, RadioServiceModule::handleCellInfoListMessage),
    HANDLER(RilUnsolNetworkScanResultMessage, RadioServiceModule::handleNetworkScanResultMessage),
    HANDLER(RilUnsolUiccAppsStatusChangedMessage,
            RadioServiceModule::handleUiccAppsStatusChangedMessage),
    HANDLER(RilUnsolNwRegistrationRejectMessage,
            RadioServiceModule::handleNwRegistrationRejectMessage),
    HANDLER(RilUnsolCellBarringMessage, RadioServiceModule::handleCellBarringMessage),
    HANDLER(UimSimStatusChangedInd, RadioServiceModule::handleUimSimStatusChangedInd),
    HANDLER(UimSimRefreshIndication, RadioServiceModule::handleUimSimRefreshIndication),
    HANDLER(GstkUnsolIndMsg, RadioServiceModule::handleGstkUnsolIndMsg),
    HANDLER(rildata::SetupDataCallRadioResponseIndMessage, RadioServiceModule::handleSetupDataCallRadioResponseIndMessage),
    HANDLER(rildata::DeactivateDataCallRadioResponseIndMessage, RadioServiceModule::handleDeactivateDataCallRadioResponseIndMessage),
    HANDLER(rildata::RadioDataCallListChangeIndMessage, RadioServiceModule::handleRadioDataCallListChangeIndMessage),
    HANDLER(rildata::CarrierInfoForImsiEncryptionRefreshMessage, RadioServiceModule::handleCarrierInfoForImsiEncryptionRefreshMessage),
    HANDLER(rildata::RadioKeepAliveStatusIndMessage,RadioServiceModule::handleRadioKeepAliveStatusIndMessage),
    HANDLER(rildata::LinkCapIndMessage,RadioServiceModule::handleLinkCapIndMessage),
    HANDLER(rildata::RilPCODataMessage, RadioServiceModule::handlePCODataMessage),
    HANDLER(rildata::PhysicalConfigStructUpdateMessage,RadioServiceModule::handlePhysicalConfigStructUpdateMessage),
    HANDLER(rildata::ThrottledApnTimerExpirationMessage, RadioServiceModule::handleUnthrottleApnMessage),
    HANDLER(QcRilUnsolPhonebookRecordsUpdatedMessage, RadioServiceModule::handlePhonebookRecordsChangedMessage),
    HANDLER(QcRilUnsolAdnRecordsOnSimMessage, RadioServiceModule::handleAdnRecordsOnSimMessage),
    HANDLER(rildata::GetDataCallListResponseIndMessage, RadioServiceModule::handleGetDataCallListResponseIndMessage),
  };
}

/* Follow RAII.
 */
RadioServiceModule::~RadioServiceModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void RadioServiceModule::init() {
  Module::init();
}

void RadioServiceModule::handleLinkCapIndMessage(std::shared_ptr<rildata::LinkCapIndMessage> msg)
{
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  if (mRadioService) {
    mRadioService->sendLinkCapInd(msg);
  }
}

void RadioServiceModule::handlePCODataMessage(std::shared_ptr<rildata::RilPCODataMessage> msg)
{
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  if (mRadioService) {
    mRadioService->sendRilPCODataInd(msg);
  }
}

void RadioServiceModule::handlePhysicalConfigStructUpdateMessage(std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg)
{
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  if (mRadioService) {
    mRadioService->sendPhysicalConfigStructUpdateInd(msg);
  }
}

void RadioServiceModule::handleRadioKeepAliveStatusIndMessage(std::shared_ptr<rildata::RadioKeepAliveStatusIndMessage> msg)
{
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  if (mRadioService) {
    mRadioService->sendKeepAliveStatusInd(msg);
  }
}

void RadioServiceModule::handleCarrierInfoForImsiEncryptionRefreshMessage(std::shared_ptr<Message> msg)
{
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  if (mRadioService) {
    mRadioService->sendCarrierInfoForImsiEncryptionInd(msg);
  }
}

void RadioServiceModule::handleRadioDataCallListChangeIndMessage(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg)
{
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  if (mRadioService) {
    mRadioService->sendRadioDataCallListChangeInd(msg);
  }
}

void RadioServiceModule::handleSetupDataCallRadioResponseIndMessage(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> msg)
{
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  if (mRadioService) {
    mRadioService->sendSetupDataCallResponse(msg);
  }
}

void RadioServiceModule::handleDeactivateDataCallRadioResponseIndMessage(std::shared_ptr<rildata::DeactivateDataCallRadioResponseIndMessage> msg)
{
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  if (mRadioService) {
    mRadioService->sendDeactivateDataCallResponse(msg);
  }
}

void RadioServiceModule::handleUnthrottleApnMessage(std::shared_ptr<rildata::ThrottledApnTimerExpirationMessage> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (mRadioService) {
    mRadioService->sendUnthrottleApn(msg);
  }
}

void RadioServiceModule::handleSsrIndicationMessage(std::shared_ptr<SsrIndicationMessage> msg) {
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (msg->getState()) {
    QCRIL_LOG_DEBUG("Modem SSR. Resetting the counts for LTE and nr5g");
    resetLteCounts();
    reset5gnrCounts();
  }
}

void RadioServiceModule::incLteReqCnt() {
  lte_req_count++;
  QCRIL_LOG_DEBUG("LTE req count %u ", lte_req_count);
}

void RadioServiceModule::incLteDisCnt() {
  lte_disabled_count++;
  QCRIL_LOG_DEBUG("Received disabled req; LTE disabled count = %u", lte_disabled_count);
}

void RadioServiceModule::incNr5gReqCnt() {
  nr5g_req_count++;
  QCRIL_LOG_DEBUG("nr5g req count %u ", nr5g_req_count);
}

void RadioServiceModule::incNr5gDisCnt() {
  nr5g_disabled_count++;
  QCRIL_LOG_DEBUG("Received disabled req; nr5g disabled count = %u", nr5g_disabled_count);
}

bool RadioServiceModule::checkLteDisCnt() {
  if (lte_disabled_count == LTE_EXP_COUNT) {
    QCRIL_LOG_DEBUG("All LTE types are disabled. Resetting req and disabled count");
    resetLteCounts();
    return true;
  }
  return false;
}

bool RadioServiceModule::checkNr5gDisCnt() {
  if (nr5g_disabled_count == NR5G_EXP_COUNT) {
    QCRIL_LOG_DEBUG("All NR5G types are disabled. Resetting req and disabled count");
    reset5gnrCounts();
    return true;
  }
  return false;
}

void RadioServiceModule::checkLteReqCnt() {
  if (lte_req_count == LTE_EXP_COUNT) {
    QCRIL_LOG_DEBUG("All LTE reqs received. Resetting req and disabled count");
    resetLteCounts();
  }
}

void RadioServiceModule::checkNr5gReqCnt() {
  if (nr5g_req_count == NR5G_EXP_COUNT) {
    QCRIL_LOG_DEBUG("All NR5G reqs received. Resetting req and disabled count");
    reset5gnrCounts();
  }
}

void RadioServiceModule::resetLteCounts() {
  lte_req_count = 0;
  lte_disabled_count = 0;
}

void RadioServiceModule::reset5gnrCounts() {
  nr5g_req_count = 0;
  nr5g_disabled_count = 0;
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void RadioServiceModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
#ifdef QMI_RIL_UTF
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  Log::getInstance().d("[" + mName +
                       "]: get_instance_id = " + std::to_string(msg->get_instance_id()));

  if (!mRadioService) {
    for (auto svcImpl : getHalServiceImplFactory<RadioServiceBase>()) {
      bool result = svcImpl->registerService(msg->get_instance_id());
      if (result) {
        Log::getInstance().d("[" + mName + "]: Registered!");
        mRadioService = svcImpl;
        break;
      }
    }
  }
#endif
}

void RadioServiceModule::handleRilUnsolRilConnectedMessage(
    std::shared_ptr<RilUnsolRilConnectedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendRilConnected(msg);
  }
}

void RadioServiceModule::handleIncoming3GppSMSMessage(
    std::shared_ptr<RilUnsolIncoming3GppSMSMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendNewSms(msg);
  }
}

void RadioServiceModule::handleIncoming3Gpp2SMSMessage(
    std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendNewCdmaSms(msg);
  }
}

void RadioServiceModule::handleNewSmsOnSimMessage(std::shared_ptr<RilUnsolNewSmsOnSimMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendNewSmsOnSim(msg);
  }
}

void RadioServiceModule::handleNewSmsStatusReportMessage(
    std::shared_ptr<RilUnsolNewSmsStatusReportMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendNewSmsStatusReport(msg);
  }
}

void RadioServiceModule::handleNewBroadcastSmsMessage(
    std::shared_ptr<RilUnsolNewBroadcastSmsMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendNewBroadcastSms(msg);
  }
}

void RadioServiceModule::handleStkCCAlphaNotifyMessage(
    std::shared_ptr<RilUnsolStkCCAlphaNotifyMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendStkCCAlphaNotify(msg);
  }
}

void RadioServiceModule::handleCdmaRuimSmsStorageFullMessage(
    std::shared_ptr<RilUnsolCdmaRuimSmsStorageFullMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendCdmaRuimSmsStorageFull(msg);
  }
}

void RadioServiceModule::handleSimSmsStorageFullMessage(
    std::shared_ptr<RilUnsolSimSmsStorageFullMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendSimSmsStorageFull(msg);
  }
}

void RadioServiceModule::handleImsNetworkStateChangedMessage(
    std::shared_ptr<RilUnsolImsNetworkStateChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendImsNetworkStateChanged(msg);
  }
}

void RadioServiceModule::handleQcRilUnsolCallRingingMessage(
    std::shared_ptr<QcRilUnsolCallRingingMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (msg && !msg->isIms()) {
    if (mRadioService) {
      mRadioService->sendCallRing(msg);
    }
  }
}

void RadioServiceModule::handleQcRilUnsolCallStateChangeMessage(
    std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (msg && (msg->getCallInfo().empty() || msg->hasCsCalls())) {
    if (mRadioService) {
      mRadioService->sendCallStateChanged(msg);
    }
  }
}

void RadioServiceModule::handleQcRilUnsolSupplementaryServiceMessage(
    std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (msg && !msg->isIms()) {
    if (mRadioService) {
      mRadioService->sendOnSupplementaryServiceIndication(msg);
    }
  }
}

void RadioServiceModule::handleQcRilUnsolSrvccStatusMessage(
    std::shared_ptr<QcRilUnsolSrvccStatusMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendSrvccStateNotify(msg);
  }
}

void RadioServiceModule::handleQcRilUnsolRingbackToneMessage(
    std::shared_ptr<QcRilUnsolRingbackToneMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (msg && !msg->isIms()) {
    if (mRadioService) {
      mRadioService->sendIndicateRingbackTone(msg);
    }
  }
}

void RadioServiceModule::handleQcRilUnsolCdmaOtaProvisionStatusMessage(
    std::shared_ptr<QcRilUnsolCdmaOtaProvisionStatusMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendCdmaOtaProvisionStatus(msg);
  }
}

void RadioServiceModule::handleQcRilUnsolCdmaCallWaitingMessage(
    std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendCdmaCallWaiting(msg);
  }
}

void RadioServiceModule::handleQcRilUnsolSuppSvcNotificationMessage(
    std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (msg && !msg->isIms()) {
    if (mRadioService) {
      mRadioService->sendSuppSvcNotify(msg);
    }
  }
}

void RadioServiceModule::handleQcRilUnsolOnUssdMessage(std::shared_ptr<QcRilUnsolOnUssdMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (msg && (msg->isCsDomain() || !(::android::hardware::radio::utils::isUssdOverImsSupported()))) {
    if (mRadioService) {
      mRadioService->sendOnUssd(msg);
    }
  }
}

void RadioServiceModule::handleQcRilUnsolCdmaInfoRecordMessage(
    std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendCdmaInfoRec(msg);
  }
}

void RadioServiceModule::handleUimSimStatusChangedInd(std::shared_ptr<UimSimStatusChangedInd> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendSimStatusChanged(msg);
  }
}

void RadioServiceModule::handleUimSimRefreshIndication(std::shared_ptr<UimSimRefreshIndication> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendSimRefresh(msg);
  }
}

void RadioServiceModule::handleGstkUnsolIndMsg(std::shared_ptr<GstkUnsolIndMsg> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendGstkIndication(msg);
  }
}

void RadioServiceModule::handleAcknowledgeRequestMessage(
    std::shared_ptr<RilAcknowledgeRequestMessage> msg) {
  if (mRadioService) {
    mRadioService->sendAcknowledgeRequest(msg);
  }
}

void RadioServiceModule::handleNetworkStateChangedMessage(
    std::shared_ptr<RilUnsolNetworkStateChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendNetworkStateChanged(msg);
  }
}

void RadioServiceModule::handleNitzTimeReceivedMessage(
    std::shared_ptr<RilUnsolNitzTimeReceivedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendNitzTimeReceived(msg);
  }
}

void RadioServiceModule::handleVoiceRadioTechChangedMessage(
    std::shared_ptr<RilUnsolVoiceRadioTechChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendVoiceRadioTechChanged(msg);
  }
}

void RadioServiceModule::handleSignalStrengthMessage(
    std::shared_ptr<RilUnsolSignalStrengthMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendSignalStrength(msg);
  }
}

void RadioServiceModule::handleEmergencyCallbackModeMessage(
    std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (!msg->isEmergencyIp()) {
    if (mRadioService) {
      mRadioService->sendEmergencyCallbackMode(msg);
    }
  }
}

void RadioServiceModule::handlelRadioCapabilityMessage(
    std::shared_ptr<RilUnsolRadioCapabilityMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendRadioCapability(msg);
  }
}

void RadioServiceModule::handleCdmaSubscriptionSourceChangedMessage(
    std::shared_ptr<RilUnsolCdmaSubscriptionSourceChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendCdmaSubscriptionSourceChanged(msg);
  }
}

void RadioServiceModule::handleCdmaPrlChangedMessage(
    std::shared_ptr<RilUnsolCdmaPrlChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendCdmaPrlChanged(msg);
  }
}

void RadioServiceModule::handleRestrictedStateChangedMessage(
    std::shared_ptr<RilUnsolRestrictedStateChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendRestrictedStateChanged(msg);
  }
}

void RadioServiceModule::handleUiccSubsStatusChangedMessage(
    std::shared_ptr<RilUnsolUiccSubsStatusChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendUiccSubsStatusChanged(msg);
  }
}

void RadioServiceModule::handleRadioStateChangedMessage(
    std::shared_ptr<RilUnsolRadioStateChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendRadioStateChanged(msg);
  }
}

void RadioServiceModule::handleModemRestartMessage(
    std::shared_ptr<RilUnsolModemRestartMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendModemReset(msg);
  }
}

void RadioServiceModule::handleEmergencyListIndMessage(
    std::shared_ptr<RilUnsolEmergencyListIndMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendCurrentEmergencyNumberList(msg);
  }
}

void RadioServiceModule::handleCellInfoListMessage(
    std::shared_ptr<RilUnsolCellInfoListMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendCellInfoList(msg);
  }
}

void RadioServiceModule::handleNetworkScanResultMessage(
    std::shared_ptr<RilUnsolNetworkScanResultMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendNetworkScanResult(msg);
  }
}

void RadioServiceModule::handleUiccAppsStatusChangedMessage(
    std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendUiccAppsStatusChanged(msg);
  }
}

void RadioServiceModule::handlePhonebookRecordsChangedMessage(
    std::shared_ptr<QcRilUnsolPhonebookRecordsUpdatedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendSimPhonebookChanged(msg);
  }
}

void RadioServiceModule::handleAdnRecordsOnSimMessage(
    std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendSimPhonebookRecords(msg);
  }
}

void RadioServiceModule::handleNwRegistrationRejectMessage(
    std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendNwRegistrationReject(msg);
  }
}
void RadioServiceModule::handleCellBarringMessage(std::shared_ptr<RilUnsolCellBarringMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mRadioService) {
    mRadioService->sendCellBarring(msg);
  }
}

void RadioServiceModule::handleGetDataCallListResponseIndMessage(
    std::shared_ptr<rildata::GetDataCallListResponseIndMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if(mRadioService) {
     mRadioService->getDataCallListResponse(msg->getDCList(), msg->getSerial(), msg->getStatus());
  }
}

#ifdef QMI_RIL_UTF
void qcril_qmi_radio_service_init(int instanceId) {
  QCRIL_LOG_DEBUG("qcril_qmi_radio_service_init %d", instanceId);
  auto msg = std::make_shared<QcrilInitMessage>((qcril_instance_id_e_type)instanceId);
  getRadioServiceModule().dispatchSync(msg);
}
#endif
