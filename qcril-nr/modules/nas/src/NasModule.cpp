/******************************************************************************
#  Copyright (c) 2017-2018 , 2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#define TAG "[NasModule]"
#include <cstring>
#include <framework/ModuleLooper.h>
#include <framework/QcrilInitMessage.h>
#include <framework/PolicyManager.h>
#include <framework/SingleDispatchRestriction.h>
#include <framework/SingleDispatchRestrictionHandleDuplicate.h>
#include <framework/SingleDispatchRestrictionHandleDuplicateOnDispatch.h>
#include <framework/RejectSimultaneousDispatchRestriction.h>
#include <framework/ThreadPoolManager.h>

#include <modules/android/ril_message_factory.h>

#include "modules/nas/qcril_qmi_prov.h"
#include "qcril_qmi_client.h"
#include "qcril_qmi_err_utils.h"
#include "NasUtils.h"

#include "modules/nas/qcril_qmi_nas.h"
#include "modules/nas/qcril_qmi_nas2.h"
#include "modules/nas/qcril_arb.h"
#include "modules/nas/qcril_nas_legacy.h"
#include "modules/nas/NasUimHeaders.h"
#include "qcril_qmi_nas_csg.h"
#include "qcril_qmi_rfrpe.h"
#include "qcril_qmi_sar.h"

#include "modules/nas/NasModule.h"
#include "modules/android/LegacyHandlerSubModule.h"
#include "modules/android/LegacyHandlerMessage.h"
#include "modules/qmi/QmiIndMessage.h"
#include "modules/qmi/EndpointStatusIndMessage.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/NasModemEndPoint.h"
#include "modules/nas/NasImsHelper.h"
#include "modules/qmi/QmiNasSetupRequestCallback.h"

//Data related
#include "modules/nas/NasDataCache.h"
#include "UnSolMessages/NasRfBandInfoIndMessage.h"
#include "DataCommon.h"

using rildata::CallStatusMessage;
using rildata::DataSysStatusMessage;
using rildata::SetPrefDataTechMessage;
using rildata::DsdSysStatusMessage;
#ifndef QMI_RIL_UTF
using rildata::DataRegistrationStateMessage;
#endif
using rildata::NewDDSInfoMessage;
using rildata::DDSStatusFollowupMessage;

#include "telephony/ril.h"

static load_module<NasModule> nas_module;

DECLARE_MSG_ID_INFO(NAS_QMI_IND);
DECLARE_MSG_ID_INFO(NAS_ENDPOINT_STATUS_IND);

NasModule &getNasModule() {
    return (nas_module.get_module());
}

//TODO: Remove this while cleaning up
static qcril_arb_pref_data_tech_e_type map_data_pref_data_tech_to_arb_pref_data_tech(qcril_data_pref_data_tech_e_type pref_data_tech);
static qcril_qmi_nas_data_call_event_type map_data_call_event_to_internal(qcril_data_net_evt_t evt);
static dds_status_info map_dds_status_info_from_data(DDSStatusInfo status_info);
/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
NasModule::NasModule() : AddPendingMessageList("NasModule") {
  mName = "NasModule";
  ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint();

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, NasModule::handleQcrilInit),

    HANDLER(RilRequestGetVoiceRegistrationMessage, NasModule::handleVoiceRegistrationState),
    HANDLER(RilRequestGetDataRegistrationMessage, NasModule::handleDataRegistrationState),
    HANDLER(RilRequestOperatorMessage, NasModule::handleOperator),
    HANDLER(RilRequestQueryNetworkSelectModeMessage, NasModule::handleQueryNetworkSelectionMode),
    HANDLER(RilRequestAllowDataMessage, NasModule::handleAllowData),
    HANDLER(RilRequestQueryAvailNetworkMessage, NasModule::handleQueryAvailableNetworks),
    HANDLER(RilRequestGetBarringInfoMessage, NasModule::handleGetBarringInfo),
    HANDLER(RilRequestSetNetworkSelectionManualMessage, NasModule::handleSetNetworkSelectionManual),
    HANDLER(RilRequestSetNetworkSelectionAutoMessage, NasModule::handleSetNetworkSelectionAutomatic),
    HANDLER(RilRequestStopNetworkScanMessage, NasModule::handleStopNetworkScan),
    HANDLER(RilRequestStartNetworkScanMessage, NasModule::handleStartNetworkScan),
    HANDLER(RilRequestGetPrefNetworkTypeMessage, NasModule::handleGetPreferredNetworkType),
    HANDLER(RilRequestSetPrefNetworkTypeMessage, NasModule::handleSetPreferredNetworkType),
    HANDLER(RilRequestGetSignalStrengthMessage, NasModule::handleSignalStrength),
    HANDLER(RilRequestGetVoiceRadioTechMessage, NasModule::handleGetVoiceRadioTech),
    HANDLER(RilRequestSetCdmaSubscriptionSourceMessage, NasModule::handleSetCdmaSubscriptionSource),
    HANDLER(RilRequestExitEmergencyCallbackMessage, NasModule::handleExitEmergencyCallbackMode),
    HANDLER(RilRequestGetNeighborCellIdsMessage, NasModule::handleGetNeighboringCellIds),
    HANDLER(RilRequestCdmaSubscriptionMessage, NasModule::handleCdmaSubscription),
    HANDLER(RilRequestSetCdmaRoamingPrefMessage, NasModule::handleCdmaSetRoamingPreference),
    HANDLER(RilRequestQueryCdmaRoamingPrefMessage, NasModule::handleCdmaQueryRoamingPreference),
    HANDLER(RilRequestGetCdmaSubscriptionSourceMessage, NasModule::handleCdmaGetSubscriptionSource),
    HANDLER(RilRequestSetLocationUpdateMessage, NasModule::handleSetLocationUpdates),
    HANDLER(RilRequestQueryAvailBandModeMessage, NasModule::handleQueryAvailableBandMode),
    HANDLER(RilRequestSetBandModeMessage, NasModule::handleSetBandMode),
    HANDLER(RilRequestSetUiccSubsMessage, NasModule::handleSetUiccSubscription),
    HANDLER(RilRequestGetRadioCapMessage, NasModule::handleGetRadioCapability),
    HANDLER(RilRequestSetRadioCapMessage, NasModule::handleSetRadioCapability),
    HANDLER(RilRequestShutDownMessage, NasModule::handleShutdown),
    HANDLER(RilRequestEnableEndcMessage, NasModule::handleEnableEndc),
    HANDLER(RilRequestQueryEndcStatusMessage, NasModule::handleQueryEndcStatus),
    HANDLER(RilRequestSetUnsolRespFilterMessage, NasModule::handleSetUnsolRespFilter),
    HANDLER(RilRequestSetUnsolBarringFilterMessage, NasModule::handleSetUnsolBarringFilter),
    HANDLER(NasSetSignalStrengthCriteriaSupported, NasModule::handleSetSignalStrengthCriteriaSupported),
    HANDLER(NasEnablePhysChanConfigReporting, NasModule::handleEnablePhysChanConfigReporting),
    HANDLER(QmiNasRfBandInfoMessage, NasModule::handleQmiNasRfBandInfoMessage),
    HANDLER(QmiNasLteCphyCaIndMessage, NasModule::handleQmiNasLteCphyCaIndMessage),
    HANDLER(RilRequestGetSysSelChannelsMessage, NasModule::handleGetSystemSelectionChannels),
    HANDLER(RilRequestSetSysSelChannelsMessage, NasModule::handleSetSystemSelectionChannels),
    HANDLER(RilRequestEnableModemMessage, NasModule::handleEnableModem),
    HANDLER(RilRequestGetModemStackStatusMessage, NasModule::handleGetModemStackStatus),
    HANDLER(RilRequestGetPhoneCapabilityMessage, NasModule::handleGetPhoneCapability),
    HANDLER(RilRequestSetUnsolCellInfoListRateMessage, NasModule::handleSetUnsolCellInfoListRateMessage),
    HANDLER(RilRequestEnableUiccAppMessage, NasModule::handleEnableUiccApplications),
    HANDLER(RilRequestGetUiccAppStatusMessage, NasModule::handleGetUiccApplicationStatus),
    HANDLER(RilRequestSetNrDisableModeMessage, NasModule::handleSetNrDisableMode),
    HANDLER(RilRequestQueryNrDisableModeMessage, NasModule::handleQueryNrDisableMode),
    HANDLER(RilRequestGetEnhancedRadioCapabilityMessage, NasModule::handleGetEnhancedRadioCapability),
    HANDLER(RilRequestGetLPlusLFeatureSupportStatusMessage,
            NasModule::handleGetLPlusLFeatureSupportStatusMessage),
    HANDLER(RilRequestGetPreferredNeworkBandPrefMessage,
            NasModule::handleGetPreferredNeworkBandPrefMessage),
    HANDLER(RilRequestSetPreferredNeworkBandPrefMessage,
            NasModule::handleSetPreferredNeworkBandPrefMessage),
    HANDLER(RilRequestSetUiStatusMessage, NasModule::handleSetUiStatusMessage),
    HANDLER(RilRequestGetCsgIdMessage, NasModule::handleGetCsgIdMessage),
    HANDLER(RilRequestCsgPerformNetworkScanMessage, NasModule::handleCsgPerformNetworkScanMessage),
    HANDLER(RilRequestCsgSetSysSelPrefMessage, NasModule::handleCsgSetSysSelPrefMessage),
    HANDLER(RilRequestSetRfmScenarioMessage, NasModule::handleSetRfmScenarioMessage),
    HANDLER(RilRequestGetRfmScenarioMessage, NasModule::handleGetRfmScenarioMessage),
    HANDLER(RilRequestGetProvTableRevMessage, NasModule::handleGetProvTableRevMessage),
    HANDLER(RilRequestEnableEngineerModeMessage, NasModule::handleEnableEngineerMode),
    HANDLER(RilRequestGetSarRevKeyMessage, NasModule::handleGetSarRevKey),
    HANDLER(RilRequestSetTransmitPowerMessage, NasModule::handleSetTransmitPower),
    HANDLER(RilRequestGetUsageSettingMessage, NasModule::handleGetUsageSetting),
    HANDLER(RilRequestSetUsageSettingMessage, NasModule::handleSetUsageSetting),

    HANDLER_MULTI(QmiIndMessage, NAS_QMI_IND ,NasModule::handleNasQmiIndMessage),
    HANDLER_MULTI(EndpointStatusIndMessage,NAS_ENDPOINT_STATUS_IND,
            NasModule::handleNasEndpointStatusIndMessage),
    // Qmi Async Response
    HANDLER(QmiAsyncResponseMessage, NasModule::handleQmiAsyncRespMessage),

    // UIM indications
    HANDLER(UimCardStatusIndMsg, NasModule::handleUimCardStatusIndMsg),
    HANDLER(UimCardStatusChangeIndMsg, NasModule::handleUimCardStatusChangeIndMsg),
    HANDLER(UimCardAppStatusIndMsg, NasModule::handleUimCardAppStatusIndMsg),

    // IMS handling
    HANDLER(ImsServiceStatusInd, NasModule::handleImsServiceStatusInd),
    HANDLER(ImsWfcSettingsStatusInd, NasModule::handleImsWfcStatusInd),
    HANDLER(ImsPdpStatusInd, NasModule::handleImsPdpStatusInd),
    HANDLER(ImsActiveSubStatusInd, NasModule::handleImsActiveSubStatusInd),
    HANDLER(NasSetVoiceDomainPreferenceRequest, NasModule::handleSetVoiceDomainPreferenceRequest),
    HANDLER(QcRilSmsNotifyRegisteredOnImsMessage, NasModule::handleSmsNotifyRegisteredOnImsMessage),
    HANDLER(QcRilVoiceNotifyEmergencyCallEndMessage, NasModule::handleVoiceNotifyEmergencyCallEndMessage),
    HANDLER(QcRilVoiceNotifyCallInProcessMessage, NasModule::handleVoiceNotifyCallInProcessMessage),
    HANDLER(QcRilUnsolVoiceNumberOfCsCallChangedMessage,
        NasModule::handleUnsolVoiceNumberOfCsCallChangedMessage),
    HANDLER(QcRilUnsolVoiceAllCsCallsEndedMessage,
        NasModule::handleUnsolVoiceAllCsCallsEndedMessage),
    HANDLER(QcRilUnsolAssessEccNumberListMessage,
        NasModule::handleUnsolAssessEccNumberListMessage),
    HANDLER(QcRilUnsolPbmInitializedMessage,
        NasModule::handleUnsolPbmInitializedMessage),
    HANDLER(RilRequestSetVonrCapRequest, NasModule::handleSetVonrCapRequest),
    HANDLER(RilRequestGetVonrCapRequest, NasModule::handleGetVonrCapRequest),

#if !defined(QMI_RIL_UTF) && !defined(RIL_FOR_MDM_DISABLE_DATA)
    //Messages from Data
    HANDLER(NasGetPhyChanConfigMessage, NasModule::handleGetPhyChanConfigMessage),

    //Data indications
    HANDLER(CallStatusMessage, NasModule::handleDataCallStatusMessage),
    HANDLER(SetPrefDataTechMessage, NasModule::handleDataSetPrefDataTechMessage),
    HANDLER(DataSysStatusMessage, NasModule::handleDataSysStatusMessage),
    HANDLER(DsdSysStatusMessage, NasModule::handleDataDsdSysStatusMessage),
    HANDLER(DataRegistrationStateMessage, NasModule::handleDataRegistrationStateMessage),
    HANDLER(NewDDSInfoMessage, NasModule::handleDataNewDDSInfoMessage),
    HANDLER(DDSStatusFollowupMessage, NasModule::handleDataDDSStatusFollowupMessage),
#endif

    HANDLER(NasSetPsAttachDetachMessage, NasModule::handlePsAttachDetachMessage),
    HANDLER(NasGetSysInfoMessage, NasModule::handleNasGetSysInfoMessage),

    HANDLER(QcRilGetMaxModemSubscriptionsSyncMessage, NasModule::handleGetMaxMaxModemSubscriptionsMessage),
    HANDLER(QcRilGetPowerOptEnabledSyncMessage, NasModule::handleGetPowerOptEnabledSyncMessage),
    HANDLER(QcRilGetUIStatusSyncMessage, NasModule::handleGetUIStatusSyncMessage),
    HANDLER(QcRilNasQuery1XSmsBlockedStatusSyncMessage,
            NasModule::handleQuery1XSmsBlockedStatusSyncMessage),
    HANDLER(QcRilGetCurrentMccMncSyncMessage, NasModule::handleGetCurrentMccMncSyncMessage),
    HANDLER(QcRilGetLteSmsStatusSyncMessage, NasModule::handleGetLteSmsStatusSyncMessage),
    HANDLER(QcRilNasQueryIsVoiceOverLteRegisteredSyncMessage,
            NasModule::handleQueryIsVoiceOverLteRegisteredSyncMessage),
    HANDLER(QcRilNasQueryIsDeviceRegisteredCsDomainSyncMessage,
            NasModule::handleQueryIsDeviceRegisteredCsDomainSyncMessage),
    HANDLER(QcRilNasQueryIsLteOnlyRegisteredSyncMessage,
            NasModule::handleQueryIsLteOnlyRegisteredSyncMessage),
    HANDLER(QcRilNasQuerySimulVoiceDataCapabilitySyncMessage,
            NasModule::handleQuerySimulVoiceDataCapabilitySyncMessage),
    HANDLER(QcRilNasQueryReportedDataTechnologySyncMessage,
            NasModule::handleQueryReportedDataTechnologySyncMessage),
    HANDLER(QcRilNasCheckCallModeAgainstDevCfgSyncMessage,
            NasModule::handleCheckCallModeAgainstDevCfgSyncMessage),
    HANDLER(QcRilNasGetCallServiceTypeSyncMessage,
            NasModule::handleGetCallServiceTypeSyncMessage),
    HANDLER(QcRilNasGetVoiceRadioTechSyncMessage,
            NasModule::handleGetVoiceRadioTechSyncMessage),
    HANDLER(QcRilNasQueryImsAvailableSyncMessage,
            NasModule::handleQueryImsAvailableSyncMessage),
    HANDLER(QcRilNasQueryScreenOffSyncMessage,
            NasModule::handleQueryScreenOffSyncMessage),
    HANDLER(QcRilNasGetEscvTypeSyncMessage,
            NasModule::handleGetEscvTypeSyncMessage),
    HANDLER(QcRilNasGetImsAddressFromEmergencyNumberSyncMessage,
            NasModule::handleGetImsAddressFromEmergencyNumberSyncMessage),
    HANDLER(QcRilNasGetRegistrationStatusOverviewSyncMessage,
            NasModule::handleGetRegistrationStatusOverviewSyncMessage),
    HANDLER(QcRilNasGetRoamStatusOverviewSyncMessage,
            NasModule::handleGetRoamStatusOverviewSyncMessage),
    HANDLER(QcRilNasQueryEmergencyNumberDisplayOnlySyncMessage,
            NasModule::handleQueryEmergencyNumberDisplayOnlySyncMessage),
    HANDLER(QcRilNasQueryEnforcableEmergencyNumberSyncMessage,
            NasModule::handleQueryEnforcableEmergencyNumberSyncMessage),
    HANDLER(QcRilNasQueryNormalVoiceCallCapabilitySyncMessage,
            NasModule::handleQueryNormalVoiceCallCapabilitySyncMessage),
    HANDLER(QcRilNasQueryNwMccBasedRoamingEmergencyNumberSyncMessage,
            NasModule::handleQueryNwMccBasedRoamingEmergencyNumberSyncMessage),
    HANDLER(QcRilNasQueryWhetherInApmLeaveWindowSyncMessage,
            NasModule::handleQueryWhetherInApmLeaveWindowSyncMessage),
    HANDLER(QcRilNasGetRilEccMapSyncMessage,
            NasModule::handleGetRilEccMapSyncMessage),
    HANDLER(RilRequestGetSubProvisioningPrefMessage,
            NasModule::handleRilRequestGetSubProvisioningPrefMessage),
    HANDLER(RilRequestSetSubProvisioningPrefMessage,
            NasModule::handleRilRequestSetSubProvisioningPrefMessage),
    HANDLER(RilRequestGetSimIccidMessage,
            NasModule::handleRilRequestGetSimIccidMessage),
    HANDLER(RilRequestSetMsimPreferenceMessage,
            NasModule::handleSetMsimPreference),




    HANDLER(rildata::NasGetRfBandInfoMessage, NasModule::handleNasGetRfBandInfoMessage),
  };
}

/* Follow RAII.
*/
NasModule::~NasModule() {}

/*
 * Module specific initialization that does not belong to RAII .
 */
void NasModule::init()
{
  Module::init();

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestGetVoiceRegistrationMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestrictionHandleDuplicate>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestGetDataRegistrationMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestrictionHandleDuplicate>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestOperatorMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestrictionHandleDuplicate>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestQueryNetworkSelectModeMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestrictionHandleDuplicate>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestSetNetworkSelectionManualMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestSetNetworkSelectionAutoMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestStopNetworkScanMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestGetSignalStrengthMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestGetBarringInfoMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestrictionHandleDuplicateOnDispatch>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestGetVoiceRadioTechMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestSetCdmaSubscriptionSourceMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestExitEmergencyCallbackMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestGetNeighborCellIdsMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestCdmaSubscriptionMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestSetCdmaRoamingPrefMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestSetLocationUpdateMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestQueryAvailBandModeMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestSetBandModeMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestSetUiccSubsMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestGetRadioCapMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestShutDownMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  std::vector<std::string> ring1{
    RilRequestGetPrefNetworkTypeMessage::MESSAGE_NAME,
    RilRequestSetPrefNetworkTypeMessage::MESSAGE_NAME,
  };
  PolicyManager::getInstance().setMessageRestriction(ring1,
         std::make_shared<SingleDispatchRestriction>());

  std::vector<std::string> rejsimul{ RilRequestQueryAvailNetworkMessage::MESSAGE_NAME,
    RilRequestSetRadioCapMessage::MESSAGE_NAME };
  PolicyManager::getInstance().setMessageRestriction(rejsimul,
         std::make_shared<RejectSimultaneousDispatchRestriction>());

  std::vector<std::string> setsysPref {RilRequestSetSysSelChannelsMessage::MESSAGE_NAME,
    RilRequestGetSysSelChannelsMessage::MESSAGE_NAME };
  PolicyManager::getInstance().setMessageRestriction(setsysPref,
        std::make_shared<SingleDispatchRestriction>());


  std::vector<std::string> ring3{RilRequestEnableModemMessage::MESSAGE_NAME,
   RilRequestGetModemStackStatusMessage::MESSAGE_NAME};

  PolicyManager::getInstance().setMessageRestriction(ring3,
         std::make_shared<SingleDispatchRestriction>());

  /* Make sure there are no race conditions due to sim on/off, flex map requests
   * reaching RIL at the same time
   */
  std::vector<std::string> ring4{
   RilRequestSetRadioCapMessage::MESSAGE_NAME,
   RilRequestEnableUiccAppMessage::MESSAGE_NAME,
   RilRequestGetUiccAppStatusMessage::MESSAGE_NAME,
   };

  PolicyManager::getInstance().setMessageRestriction(ring4,
         std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(
      std::vector<std::string>{ QmiNasLteCphyCaIndMessage::MESSAGE_NAME },
      std::make_shared<SingleDispatchRestrictionHandleDuplicateOnDispatch>());
  PolicyManager::getInstance().setMessageRestriction(
      std::vector<std::string>{ QmiNasRfBandInfoMessage::MESSAGE_NAME },
      std::make_shared<SingleDispatchRestrictionHandleDuplicateOnDispatch>());

  std::vector<std::string> ring5{RilRequestSetNrDisableModeMessage::MESSAGE_NAME,
    RilRequestQueryNrDisableModeMessage::MESSAGE_NAME};

  PolicyManager::getInstance().setMessageRestriction(ring5,
         std::make_shared<SingleDispatchRestriction>());

  PolicyManager::getInstance().setMessageRestriction(std::vector<std::string>{
        RilRequestCsgSetSysSelPrefMessage::MESSAGE_NAME},
        std::make_shared<SingleDispatchRestriction>());

  std::vector<std::string> device_setting{RilRequestSetUsageSettingMessage::MESSAGE_NAME,
    RilRequestGetUsageSettingMessage::MESSAGE_NAME};

  PolicyManager::getInstance().setMessageRestriction(device_setting,
    std::make_shared<SingleDispatchRestriction>());

}

bool NasModule::isDmsEndpointReady() {
  return qcril_qmi_client_is_client_initialized(QCRIL_QMI_CLIENT_DMS);
}

/*
 * List of individual private handlers for the subscribed messages.
 */

void NasModule::handleUimCardStatusChangeIndMsg(std::shared_ptr<UimCardStatusChangeIndMsg> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());

  if (isDmsEndpointReady()) {
    qcril_qmi_nas_trigger_bootup_optimization();
  }
  mReceivedCardStatusEvent = true;
}

void NasModule::handleUimCardAppStatusIndMsg(std::shared_ptr<UimCardAppStatusIndMsg> msg) {
  qcril_request_return_type ret_ptr;

  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  qcril_request_params_type params_ptr;
  std::memset(&params_ptr, 0, sizeof(params_ptr));
  params_ptr.data = static_cast<void *>(new char[sizeof(RIL_UIM_AppStatus)]);
  if(params_ptr.data)
  {
    std::memcpy(params_ptr.data, &msg->get_app_info(), sizeof(RIL_UIM_AppStatus));
    params_ptr.datalen = sizeof(RIL_UIM_AppStatus);
    params_ptr.modem_id = QCRIL_DEFAULT_MODEM_ID;

    qcril_qmi_nas_event_app_status_update (&params_ptr, &ret_ptr);

    delete [] (char*)params_ptr.data;
  }
  else
  {
    Log::getInstance().d("[NasModule]: Memory allocation failure");
  }
}


void NasModule::handleUimCardStatusIndMsg(std::shared_ptr<UimCardStatusIndMsg> msg) {
  qcril_request_return_type ret_ptr;

  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  qcril_request_params_type params_ptr;
  std::memset(&params_ptr, 0, sizeof(params_ptr));
  params_ptr.data = static_cast<void *>(new char[sizeof(qcril_uim_card_info_type)]);
  if(params_ptr.data)
  {
    std::memcpy(params_ptr.data, &msg->get_card_status(), sizeof(qcril_uim_card_info_type));
    params_ptr.datalen = sizeof(qcril_uim_card_info_type);
    params_ptr.modem_id = QCRIL_DEFAULT_MODEM_ID;

    qcril_qmi_nas_event_card_status_update (&params_ptr, &ret_ptr);

    delete [] (char*)params_ptr.data;
  }
  else
  {
    Log::getInstance().d("[NasModule]: Memory allocation failure");
  }
}

void NasModule::handleNasQmiIndMessage(std::shared_ptr<Message> msg) {
  auto shared_indMsg(
        std::static_pointer_cast<QmiIndMessage>(msg));

  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  QmiIndMsgDataStruct *indData = shared_indMsg->getData();
  if (indData != nullptr) {
    qcril_qmi_nas_unsolicited_indication_cb_helper(indData->msgId, indData->indData,
        indData->indSize);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void NasModule::handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  QmiAsyncRespData *asyncResp = msg->getData();
  if (asyncResp != nullptr && asyncResp->cb != nullptr) {
    asyncResp->cb(asyncResp->msgId, asyncResp->respData, asyncResp->respDataSize,
        asyncResp->cbData, asyncResp->traspErr);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}


void NasModule::handleVoiceRegistrationState(std::shared_ptr<RilRequestGetVoiceRegistrationMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    auto cbData = new ScheduleCallbackData(shared_msg);
    if (cbData) {
        ThreadPoolManager::getInstance().scheduleExecution(
            qcril_qmi_nas_request_registration_state_wrapper, cbData);
    } else {
        shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
            std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_NO_MEMORY, nullptr));
    }
}


void NasModule::handleDataRegistrationState(std::shared_ptr<RilRequestGetDataRegistrationMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    auto cbData = new ScheduleCallbackData(shared_msg);
    if (cbData) {
        ThreadPoolManager::getInstance().scheduleExecution(
            qcril_qmi_nas_request_data_registration_state_wrapper, cbData);
    } else {
        shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
            std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_NO_MEMORY, nullptr));
    }
}


void NasModule::handleOperator(std::shared_ptr<RilRequestOperatorMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    auto cbData = new ScheduleCallbackData(shared_msg);
    if (cbData) {
        ThreadPoolManager::getInstance().scheduleExecution(
            qcril_qmi_nas_request_operator_wrapper, cbData);
    } else {
        shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
            std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_NO_MEMORY, nullptr));
    }
}

void NasModule::handleQueryNetworkSelectionMode(
        std::shared_ptr<RilRequestQueryNetworkSelectModeMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    auto cbData = new ScheduleCallbackData(shared_msg);
    if (cbData) {
        ThreadPoolManager::getInstance().scheduleExecution(
            qcril_qmi_nas_query_network_selection_mode_wrapper, cbData);
    } else {
        shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
            std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_NO_MEMORY, nullptr));
    }
}


void NasModule::handleAllowData(std::shared_ptr<RilRequestAllowDataMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_request_allow_data(shared_msg);
}


void NasModule::handleCdmaSubscription(std::shared_ptr<RilRequestCdmaSubscriptionMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_request_cdma_subscription(shared_msg);
}


void NasModule::handleCdmaSetRoamingPreference(std::shared_ptr<RilRequestSetCdmaRoamingPrefMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_request_set_roaming_preference(shared_msg);
}

void NasModule::handleCdmaQueryRoamingPreference(
    std::shared_ptr<RilRequestQueryCdmaRoamingPrefMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());
    qcril_qmi_nas_request_query_roaming_preference(shared_msg);
}

void NasModule::handleCdmaGetSubscriptionSource(
    std::shared_ptr<RilRequestGetCdmaSubscriptionSourceMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());
    qcril_qmi_nas_get_subscription_source(shared_msg);
}

void NasModule::handleSetLocationUpdates(std::shared_ptr<RilRequestSetLocationUpdateMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_set_location_updates(shared_msg);
}


void NasModule::handleQueryAvailableNetworks(std::shared_ptr<RilRequestQueryAvailNetworkMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_query_available_networks(shared_msg);
}

void NasModule::handleGetBarringInfo(std::shared_ptr<RilRequestGetBarringInfoMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_get_barring_info(shared_msg);
}

void NasModule::handleQueryAvailableBandMode(std::shared_ptr<RilRequestQueryAvailBandModeMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_query_available_band_mode(shared_msg);
}

void NasModule::handleSetUsageSetting(std::shared_ptr<RilRequestSetUsageSettingMessage> shared_msg){
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());
    qcril_qmi_nas_set_usage_setting(shared_msg);
}

void NasModule::handleGetUsageSetting(std::shared_ptr<RilRequestGetUsageSettingMessage> shared_msg){
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());
    qcril_qmi_nas_get_usage_setting(shared_msg);
}

void NasModule::handleSignalStrength(std::shared_ptr<RilRequestGetSignalStrengthMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_request_signal_strength(shared_msg);
}


void NasModule::handleSetPreferredNetworkType(std::shared_ptr<RilRequestSetPrefNetworkTypeMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_request_set_preferred_network_type(shared_msg);
}


void NasModule::handleGetPreferredNetworkType(std::shared_ptr<RilRequestGetPrefNetworkTypeMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_request_get_preferred_network_type(shared_msg);
}

void NasModule::handleSetCdmaSubscriptionSource(
        std::shared_ptr<RilRequestSetCdmaSubscriptionSourceMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_set_subscription_source(shared_msg);
}

void NasModule::handleEnableEndc(std::shared_ptr<RilRequestEnableEndcMessage> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    qcril_qmi_nas_request_enable_endc(msg);
}

void NasModule::handleQueryEndcStatus(std::shared_ptr<RilRequestQueryEndcStatusMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_nas_request_query_endc_status(msg);
}

void NasModule::handleExitEmergencyCallbackMode(std::shared_ptr<RilRequestExitEmergencyCallbackMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_exit_emergency_callback_mode_msg(shared_msg);
}


void NasModule::handleGetVoiceRadioTech(std::shared_ptr<RilRequestGetVoiceRadioTechMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_request_radio_tech(shared_msg);
}

void NasModule::handleEnableEngineerMode(std::shared_ptr<RilRequestEnableEngineerModeMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_nas_enable_engineer_mode(msg);
}

void NasModule::handleGetSarRevKey(std::shared_ptr<RilRequestGetSarRevKeyMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_sar_get_sar_rev_key(msg);
}

void NasModule::handleSetTransmitPower(std::shared_ptr<RilRequestSetTransmitPowerMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_sar_set_max_transmit_power(msg);
}

void NasModule::handleSetUiccSubscription(std::shared_ptr<RilRequestSetUiccSubsMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_set_uicc_subscription(shared_msg);
}

void NasModule::handleShutdown(std::shared_ptr<RilRequestShutDownMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_request_shutdown(shared_msg);
}


void NasModule::handleGetRadioCapability(std::shared_ptr<RilRequestGetRadioCapMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_get_radio_capability(shared_msg);
}


void NasModule::handleSetRadioCapability(std::shared_ptr<RilRequestSetRadioCapMessage> shared_msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

    qcril_qmi_nas_set_radio_capability(shared_msg);
}


void NasModule::handleSetNetworkSelectionAutomatic(std::shared_ptr<RilRequestSetNetworkSelectionAutoMessage> shared_msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

  qcril_qmi_nas_set_network_selection_automatic(shared_msg);
}


void NasModule::handleSetNetworkSelectionManual(
    std::shared_ptr<RilRequestSetNetworkSelectionManualMessage> shared_msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

  qcril_qmi_nas_set_network_selection_manual(shared_msg);
}

void NasModule::handleSetBandMode(std::shared_ptr<RilRequestSetBandModeMessage> shared_msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

  qcril_qmi_nas_set_band_mode(shared_msg);
}


void NasModule::handleGetNeighboringCellIds(std::shared_ptr<RilRequestGetNeighborCellIdsMessage> shared_msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + shared_msg->dump());

  qcril_qmi_nas_get_neighboring_cell_ids(shared_msg);
}

void NasModule::handleSetUnsolRespFilter(std::shared_ptr<RilRequestSetUnsolRespFilterMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));
  qcril_qmi_nas_request_set_unsol_response_filter(msg);
}

void NasModule::handleSetUnsolBarringFilter(std::shared_ptr<RilRequestSetUnsolBarringFilterMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));
  qcril_qmi_nas_request_set_unsol_barring_filter(msg);
}

void NasModule::handleSetUnsolCellInfoListRateMessage(
    std::shared_ptr<RilRequestSetUnsolCellInfoListRateMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));
  RIL_Errno rilErr = RIL_E_INVALID_ARGUMENTS;
  if (msg != nullptr) {
    if (msg->hasRate()) {
      int32_t rate = msg->getRate();
      if (rate < 0) {
        rilErr = RIL_E_INVALID_ARGUMENTS;
      } else {
        qcril_qmi_nas_set_cell_info_list_rate(rate);
        rilErr = RIL_E_SUCCESS;
      }
    }
    std::shared_ptr<QcRilRequestMessageCallbackPayload> respPayload =
        std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
  }
  QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleStartNetworkScan(std::shared_ptr<RilRequestStartNetworkScanMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  Log::getInstance().d("[NasModule]: request Source = " +
                        std::to_string(static_cast<int>(msg->getRequestSource())));

  if(msg->getRequestSource() == RilRequestStartNetworkScanMessage::RequestSource::OEM) {
    qcril_qmi_nas_perform_incremental_network_scan(msg);
  } else {
    qcril_qmi_nas_start_advanced_scan(msg);
  }
}

void NasModule::handleStopNetworkScan(std::shared_ptr<RilRequestStopNetworkScanMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());

    qcril_qmi_nas_cancel_query_available_networks(msg);
}

void NasModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_nas_pre_init();
  /* Init QMI services.*/
  QmiNasSetupRequestCallback qmiNasSetupCallback("NasModule-Token");
  ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint()->requestSetup(
        "NasModule-Token", msg->get_instance_id(), &qmiNasSetupCallback);
  if (isDmsEndpointReady() && mReceivedCardStatusEvent)  {
    qcril_qmi_nas_trigger_bootup_optimization();
  }
}


void NasModule::handleNasEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
     if (!mReady) {
       mReady = true;
       qcril_qmi_nas_init();
     }
  } else {
    // state is not operational
    mReady = false;
    clearPendingMessageList();
    qcril_qmi_nas_cleanup();

    // invalidate physical channel configuration cache
    chaninfo.bandwidth_info_list_valid = false;
    chaninfo.pcell_info_valid = false;
    chaninfo.scell_info_list_valid = false;
  }
}

// IMS handling
void NasModule::handleImsServiceStatusInd(std::shared_ptr<ImsServiceStatusInd> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_nas_handle_ims_service_status_change(msg->getServiceStatusInfo());
}

void NasModule::handleImsWfcStatusInd(std::shared_ptr<ImsWfcSettingsStatusInd> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_nas_handle_ims_wfc_status_change(msg->getStatusInfo());
}

void NasModule::handleImsPdpStatusInd(std::shared_ptr<ImsPdpStatusInd> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_nas_handle_ims_pdp_status_change(msg->isPdpConnected());
}

void NasModule::handleImsActiveSubStatusInd(std::shared_ptr<ImsActiveSubStatusInd> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_nas_handle_ims_active_sub_status_change(msg->isSubActive());
}

void NasModule::handleSetVoiceDomainPreferenceRequest(std::shared_ptr<NasSetVoiceDomainPreferenceRequest> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_nas_set_voice_domain_preference(msg);
}

void NasModule::handleSetVonrCapRequest(std::shared_ptr<RilRequestSetVonrCapRequest> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_nas_set_vonr_cap_request(msg);
}

void NasModule::handleGetVonrCapRequest(std::shared_ptr<RilRequestGetVonrCapRequest> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_nas_get_vonr_cap_request(msg);
}

void NasModule::handleGetPhyChanConfigMessage(std::shared_ptr<NasGetPhyChanConfigMessage> msg)
{
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS, getCurrentPhysChanConfig());
}

#if !defined(QMI_RIL_UTF) && !defined(RIL_FOR_MDM_DISABLE_DATA)
/* Data Indication Handlers */
void NasModule::handleDataCallStatusMessage(std::shared_ptr<CallStatusMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  const rildata::CallStatusParams data = msg->getCallParams();
  NasDataCache::getInstance().setNumDataCalls(data.num_calls);

  //CALL FUNCTION IN QCRIL to notify this indication.
  qcril_qmi_nas_handle_data_call_status_notification(map_data_call_event_to_internal(data.evt));
}
void NasModule::handleDataSetPrefDataTechMessage(std::shared_ptr<SetPrefDataTechMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  //NasDataCache::getInstance().setsetPreferredDataTech(indMsg->get());

  //Call Function in QCRIL to notify this indication
  qcril_arb_set_pref_data_tech(map_data_pref_data_tech_to_arb_pref_data_tech(msg->get()));
}
void NasModule::handleDataSysStatusMessage(std::shared_ptr<DataSysStatusMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  //TODO: Revisit this
  //qcril_arb_set_data_sys_status(&(msg->getParams()));
}
void NasModule::handleDataDsdSysStatusMessage(std::shared_ptr<DsdSysStatusMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  //TODO: Revisit this
  qcril_arb_set_dsd_sys_status(&(msg->getParams()));
}

void NasModule::handleDataRegistrationStateMessage(std::shared_ptr<DataRegistrationStateMessage> msg) {
  // qcril-data will not be sending DSD indication if 5G rat available and when 5G is OOS.
  // This logic will make sure that in same file, we process 5G and legacy rats.
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg->getRatValue() != rildata::Rat::UNKNOWN) {
    dsd_system_status_ind_msg_v01 dsd_ind_msg;
    memset(&dsd_ind_msg, 0, sizeof(dsd_ind_msg));
    dsd_ind_msg.avail_sys_valid = 1;
    dsd_ind_msg.avail_sys_len = 2;
    dsd_ind_msg.avail_sys[0].technology = DSD_SYS_NETWORK_3GPP_V01;
    dsd_ind_msg.avail_sys[0].rat_value = DSD_SYS_RAT_EX_3GPP_5G_V01;
    if (msg->getRatValue() == rildata::Rat::NR_NSA) {
       dsd_ind_msg.avail_sys[0].so_mask = QMI_DSD_3GPP_SO_MASK_5G_NSA_V01;
    } else if (msg->getRatValue() == rildata::Rat::NR_SA) {
       dsd_ind_msg.avail_sys[0].so_mask = QMI_DSD_3GPP_SO_MASK_5G_SA_V01;
    } else if (msg->getRatValue() == rildata::Rat::NR_NSA_MMWV) {
       dsd_ind_msg.avail_sys[0].so_mask = QMI_DSD_3GPP_SO_MASK_5G_NSA_V01 |
                                          QMI_DSD_3GPP_SO_MASK_5G_MMWAVE_V01;
    } else if (msg->getRatValue() == rildata::Rat::NR_SA_MMWV) {
       dsd_ind_msg.avail_sys[0].so_mask = QMI_DSD_3GPP_SO_MASK_5G_SA_V01 |
                                          QMI_DSD_3GPP_SO_MASK_5G_MMWAVE_V01;
    }

    dsd_ind_msg.avail_sys[1].technology = DSD_SYS_NETWORK_3GPP_V01;
    dsd_ind_msg.avail_sys[1].rat_value = DSD_SYS_RAT_EX_3GPP_LTE_V01;
    qcril_arb_set_dsd_sys_status(&dsd_ind_msg);
  }
}

void NasModule::handleDataNewDDSInfoMessage(std::shared_ptr<rildata::NewDDSInfoMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_qmi_nas_data_event_new_dds_info(msg->getDDS());
}

void NasModule::handleDataDDSStatusFollowupMessage(std::shared_ptr<rildata::DDSStatusFollowupMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  DDSStatusInfo status_info = msg->getDDSStatusInfo();
  qcril_qmi_nas_data_event_dds_status_follow_up(map_dds_status_info_from_data(status_info));
}

dds_status_info map_dds_status_info_from_data(DDSStatusInfo status_info)
{
  dds_status_info ret;
  memset(&ret, 0, sizeof(ret));

  ret.result = status_info.result;
  ret.error_cause = status_info.error_cause;

  return ret;
}

qcril_arb_pref_data_tech_e_type map_data_pref_data_tech_to_arb_pref_data_tech(qcril_data_pref_data_tech_e_type pref_data_tech) {
  qcril_arb_pref_data_tech_e_type ret = QCRIL_ARB_PREF_DATA_TECH_INVALID;
  switch (pref_data_tech) {
  case QCRIL_DATA_PREF_DATA_TECH_UNKNOWN:
      ret = QCRIL_ARB_PREF_DATA_TECH_UNKNOWN;
      break;
  case QCRIL_DATA_PREF_DATA_TECH_CDMA:
      ret = QCRIL_ARB_PREF_DATA_TECH_CDMA;
      break;
  case QCRIL_DATA_PREF_DATA_TECH_EVDO:
      ret = QCRIL_ARB_PREF_DATA_TECH_EVDO;
      break;
  case QCRIL_DATA_PREF_DATA_TECH_GSM:
      ret = QCRIL_ARB_PREF_DATA_TECH_GSM;
      break;
  case QCRIL_DATA_PREF_DATA_TECH_UMTS:
      ret = QCRIL_ARB_PREF_DATA_TECH_UMTS;
      break;
  case QCRIL_DATA_PREF_DATA_TECH_EHRPD:
      ret = QCRIL_ARB_PREF_DATA_TECH_EHRPD;
      break;
  case QCRIL_DATA_PREF_DATA_TECH_LTE:
      ret = QCRIL_ARB_PREF_DATA_TECH_LTE;
      break;
  case QCRIL_DATA_PREF_DATA_TECH_TDSCDMA:
      ret = QCRIL_ARB_PREF_DATA_TECH_TDSCDMA;
      break;
  default:
      ret = QCRIL_ARB_PREF_DATA_TECH_INVALID;
      break;
  }
  return ret;
}
static qcril_qmi_nas_data_call_event_type map_data_call_event_to_internal(qcril_data_net_evt_t evt) {
  qcril_qmi_nas_data_call_event_type ret = QCRIL_QMI_NAS_DATA_EVT_UNKNOWN;
  switch(evt) {
  case QCRIL_DATA_EVT_CALL_CONNECTED:
     ret = QCRIL_QMI_NAS_DATA_EVT_CALL_CONNECTED;
     break;
  case QCRIL_DATA_EVT_CALL_PHYSLINK_UP:
     ret = QCRIL_QMI_NAS_DATA_EVT_CALL_PHYSLINK_UP;
     break;
  case QCRIL_DATA_EVT_CALL_PHYSLINK_DOWN:
     ret = QCRIL_QMI_NAS_DATA_EVT_CALL_PHYSLINK_DOWN;
     break;
  case QCRIL_DATA_EVT_CALL_RELEASED:
     ret = QCRIL_QMI_NAS_DATA_EVT_CALL_RELEASED;
     break;
  default:
     ret = QCRIL_QMI_NAS_DATA_EVT_UNKNOWN;
     break;
  }
  return ret;
}
#endif

void NasModule::handlePsAttachDetachMessage(std::shared_ptr<NasSetPsAttachDetachMessage> msg) {
  nas_srv_domain_pref_enum_type_v01 pref = NAS_SRV_DOMAIN_PREF_ENUM_TYPE_MIN_ENUM_VAL_V01;
  RIL_Errno res = RIL_E_INVALID_ARGUMENTS;

  switch (msg->getAction()) {
  case NasSetPsAttachDetachMessage::PsAttachAction::PS_ATTACH:
    pref = QMI_SRV_DOMAIN_PREF_PS_ATTACH_V01;
    break;
  case NasSetPsAttachDetachMessage::PsAttachAction::PS_DETACH:
    pref = QMI_SRV_DOMAIN_PREF_PS_DETACH_V01;
    break;
  case NasSetPsAttachDetachMessage::PsAttachAction::PS_ATTACH_ON_DEMAND:
    pref = QMI_SRV_DOMAIN_PREF_ON_DEMAND_PS_ATTACH_V01;
    break;
  case NasSetPsAttachDetachMessage::PsAttachAction::PS_ATTACH_FORCED:
    pref = QMI_SRV_DOMAIN_PREF_FORCE_PS_DETACH_V01;
    break;
  default:
    break;
  }
  if (pref != NAS_SRV_DOMAIN_PREF_ENUM_TYPE_MIN_ENUM_VAL_V01) {
    res = qcril_qmi_nas_send_attach_detach_request(pref);
  }

  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<RIL_Errno>(res));
}

void NasModule::handleNasGetSysInfoMessage(std::shared_ptr<NasGetSysInfoMessage> msg) {

  QCRIL_LOG_FUNC_ENTRY();

  nas_get_sys_info_resp_msg_v01     sys_info_list;
  memset(&sys_info_list, 0, sizeof(nas_get_sys_info_resp_msg_v01));
  qcril_qmi_nas_get_sys_info(&sys_info_list);

  QCRIL_LOG_DEBUG("Received NasGetSysInfoMessage Msg. Executing handleNasGetSysInfoMessage  ");

  QCRIL_LOG_DEBUG("sys info list - service valid=%d & service domain=%d", sys_info_list.lte_sys_info.common_sys_info.srv_domain_valid,sys_info_list.lte_sys_info.common_sys_info.srv_domain);
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
                               std::make_shared<nas_get_sys_info_resp_msg_v01>(sys_info_list));
  QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleGetMaxMaxModemSubscriptionsMessage(
        std::shared_ptr<QcRilGetMaxModemSubscriptionsSyncMessage> msg) {
  int subs = qcril_qmi_dms_get_max_modem_subscritions();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
          std::make_shared<int>(subs));
}

void NasModule::handleGetPowerOptEnabledSyncMessage(
        std::shared_ptr<QcRilGetPowerOptEnabledSyncMessage> msg) {
  auto res = qcril_qmi_nas_is_feature_power_opt_enable();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(res));
}

void NasModule::handleGetUIStatusSyncMessage(
        std::shared_ptr<QcRilGetUIStatusSyncMessage> msg) {
  auto res = qcril_qmi_nas_get_atel_ui_status_from_cache();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<uint8_t>(res));
}

void NasModule::handleQuery1XSmsBlockedStatusSyncMessage(
        std::shared_ptr<QcRilNasQuery1XSmsBlockedStatusSyncMessage> msg) {
  auto res = qcril_qmi_nas_is_1x_sms_blocked();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(res));
}

void NasModule::handleGetCurrentMccMncSyncMessage(
        std::shared_ptr<QcRilGetCurrentMccMncSyncMessage> msg) {
  char current_mcc_str[NAS_MCC_MNC_MAX_SIZE] = {0};
  char current_mnc_str[NAS_MCC_MNC_MAX_SIZE] = {0};

  bool success = qcril_qmi_nas_fetch_mcc_mnc_of_current_system(current_mcc_str,
        current_mnc_str, msg->is3gpp(), msg->is3gpp2());

  auto res = std::make_shared<QcRilGetMccMncType>();
  if (res != nullptr) {
    res->mcc = current_mcc_str;
    res->mnc = current_mnc_str;
    res->success = success;

    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, res);
  } else {
    msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
  }
}

void NasModule::handleGetLteSmsStatusSyncMessage(
        std::shared_ptr<QcRilGetLteSmsStatusSyncMessage> msg) {
  NasSmsStatus status = NasSmsStatus::STATUS_UNKNOWN;

  uint8_t lte_sms_status_valid = FALSE;
  nas_sms_status_enum_type_v01 lte_sms_status;
  qcril_qmi_nas_fetch_lte_sms_status(&lte_sms_status_valid, &lte_sms_status);

  if (lte_sms_status_valid) {
    status = convertNasSmsStatus(lte_sms_status);
  }
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<NasSmsStatus>(status));
}

void NasModule::handleQueryIsVoiceOverLteRegisteredSyncMessage(
    std::shared_ptr<QcRilNasQueryIsVoiceOverLteRegisteredSyncMessage> msg) {
  bool res = qcril_qmi_nas_is_voice_over_lte_registered();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(res));
}

void NasModule::handleQueryIsDeviceRegisteredCsDomainSyncMessage(
    std::shared_ptr<QcRilNasQueryIsDeviceRegisteredCsDomainSyncMessage> msg) {
  bool res = qcril_qmi_nas_is_device_registered_for_cs_domain();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(res));
}

void NasModule::handleQueryIsLteOnlyRegisteredSyncMessage(
    std::shared_ptr<QcRilNasQueryIsLteOnlyRegisteredSyncMessage> msg) {
  bool res = qcril_qmi_nas_is_lte_only_registered();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(res));
}

void NasModule::handleQuerySimulVoiceDataCapabilitySyncMessage(
    std::shared_ptr<QcRilNasQuerySimulVoiceDataCapabilitySyncMessage> msg) {
  uint64_t res = qcril_qmi_nas_retrieve_sv_capability();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<uint64_t>(res));
}

void NasModule::handleQueryReportedDataTechnologySyncMessage(
    std::shared_ptr<QcRilNasQueryReportedDataTechnologySyncMessage> msg) {
  int res = qcril_qmi_nas_retrieve_reported_data_technology();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<int>(res));
}

void NasModule::handleCheckCallModeAgainstDevCfgSyncMessage(
    std::shared_ptr<QcRilNasCheckCallModeAgainstDevCfgSyncMessage> msg) {
  bool res = qmi_ril_nw_reg_voice_is_voice_call_mode_reasonable_against_dev_cfg(
        msg->getCallMode());
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(res));
}

void NasModule::handleGetCallServiceTypeSyncMessage(
    std::shared_ptr<QcRilNasGetCallServiceTypeSyncMessage> msg) {
  auto type = qcril_qmi_nas_setting_srv_type_based_on_elaboration_and_rat(
        msg->getDialOnCs());
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<voice_dial_call_service_type_enum_v02>(type));
}

void NasModule::handleGetVoiceRadioTechSyncMessage(
    std::shared_ptr<QcRilNasGetVoiceRadioTechSyncMessage> msg) {
  VoiceRadioTechType tech;
  tech.currentVoiceRadioTech = qmi_ril_nw_reg_get_current_rte_of_kind(
        QMI_RIL_RTE_KIND_VOICE);
  tech.reportedVoiceRadioTech = qcril_qmi_nas_get_reported_voice_radio_tech();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<VoiceRadioTechType>(tech));
}

void NasModule::handleQueryImsAvailableSyncMessage(
    std::shared_ptr<QcRilNasQueryImsAvailableSyncMessage> msg) {
  bool res = qcril_qmi_nas_is_ims_available();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(res));
}

void NasModule::handleQueryScreenOffSyncMessage(
    std::shared_ptr<QcRilNasQueryScreenOffSyncMessage> msg) {
  bool res = qcril_qmi_ril_domestic_service_is_screen_off();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(res));
}

void NasModule::handleGetEscvTypeSyncMessage(
    std::shared_ptr<QcRilNasGetEscvTypeSyncMessage> msg) {
  auto res = qcril_qmi_nas_get_escv_type(msg->getNum().c_str());
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<int>(res));
}

void NasModule::handleGetImsAddressFromEmergencyNumberSyncMessage(
    std::shared_ptr<QcRilNasGetImsAddressFromEmergencyNumberSyncMessage> msg) {
  char address[256] = {0};
  strlcpy(address, msg->getNum().c_str(), sizeof(address));
  (void)qmi_ril_nwreg_convert_emergency_number_to_ims_address(
        address, sizeof(address));
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<std::string>(address));
}

void NasModule::handleGetRegistrationStatusOverviewSyncMessage(
    std::shared_ptr<QcRilNasGetRegistrationStatusOverviewSyncMessage> msg) {
  auto res = qmi_ril_nw_reg_get_status_overview();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<uint32_t>(res));
}

void NasModule::handleGetRoamStatusOverviewSyncMessage(
    std::shared_ptr<QcRilNasGetRoamStatusOverviewSyncMessage> msg) {
  auto res = qmi_ril_nw_reg_get_roam_status_overview();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<uint32_t>(res));
}

void NasModule::handleQueryEmergencyNumberDisplayOnlySyncMessage(
    std::shared_ptr<QcRilNasQueryEmergencyNumberDisplayOnlySyncMessage> msg) {
  auto res = qmi_ril_nwreg_is_designated_number_emergency_for_display_purposes_only(
    msg->getNum().c_str());
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(!!res));
}

void NasModule::handleQueryEnforcableEmergencyNumberSyncMessage(
    std::shared_ptr<QcRilNasQueryEnforcableEmergencyNumberSyncMessage> msg) {
  auto res = qmi_ril_nwreg_is_designated_number_enforcable_ncl(msg->getNum().c_str());
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(!!res));
}

void NasModule::handleQueryNormalVoiceCallCapabilitySyncMessage(
    std::shared_ptr<QcRilNasQueryNormalVoiceCallCapabilitySyncMessage> msg) {
  auto res = qcril_qmi_nas_has_normal_voice_call_capability();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(!!res));
}

void NasModule::handleQueryNwMccBasedRoamingEmergencyNumberSyncMessage(
    std::shared_ptr<QcRilNasQueryNwMccBasedRoamingEmergencyNumberSyncMessage> msg) {
  auto res = qmi_ril_is_designated_number_nw_mcc_roaming_emergency(
    msg->getNum().c_str());
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(!!res));
}

void NasModule::handleQueryWhetherInApmLeaveWindowSyncMessage(
    std::shared_ptr<QcRilNasQueryWhetherInApmLeaveWindowSyncMessage> msg) {
  auto res = qmi_ril_in_apm_leave_window();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<bool>(!!res));
}

void NasModule::handleGetRilEccMapSyncMessage(
    std::shared_ptr<QcRilNasGetRilEccMapSyncMessage> msg) {
  qcril::interfaces::RIL_EccMapType eccMap;
  qcril_qmi_nas_get_ril_ecc_map(eccMap);
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<qcril::interfaces::RIL_EccMapType>(
        std::move(eccMap)));
}

#ifdef QMI_RIL_UTF
void NasModule::qcrilHalNasModuleCleanup()
{
    std::shared_ptr<NasModemEndPoint> mNasModemEndPoint =
                ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint();
    NasModemEndPointModule* mNasModemEndPointModule =
               (NasModemEndPointModule*)mNasModemEndPoint->mModule;
    mNasModemEndPointModule->cleanUpQmiSvcClient();
    mReady = false;
}
#endif

int32_t toKhz(nas_bandwidth_enum_type_v01 bw) {
    int32_t ret = INT_MAX;
    switch(bw) {
        case NAS_LTE_BW_NRB_6_V01:
            ret = 1400;
            break;
        case NAS_LTE_BW_NRB_15_V01:
            ret = 3000;
            break;
        case NAS_LTE_BW_NRB_25_V01:
            ret = 5000;
            break;
        case NAS_LTE_BW_NRB_50_V01:
            ret = 10000;
            break;
        case NAS_LTE_BW_NRB_75_V01:
            ret = 15000;
            break;
        case NAS_LTE_BW_NRB_100_V01:
            ret = 20000;
            break;
        case NAS_GSM_BW_NRB_2_V01:
            ret = 200;
            break;
        case NAS_WCDMA_BW_NRB_5_V01:
            ret = 5000;
            break;
        case NAS_WCDMA_BW_NRB_10_V01:
            ret = 10000;
            break;
        default:
            QCRIL_LOG_ERROR("Unknown bw value");
    }
    return ret;
}

int32_t toKhz(nas_lte_cphy_ca_bandwidth_enum_v01 bw) {

    switch(bw) {
        case NAS_LTE_CPHY_CA_BW_NRB_6_V01  : return 1400;  // 1.4 MHz bandwidth
        case NAS_LTE_CPHY_CA_BW_NRB_15_V01 : return 3000;  // 3 MHz bandwidth
        case NAS_LTE_CPHY_CA_BW_NRB_25_V01 : return 5000;  // 5 MHz bandwidth
        case NAS_LTE_CPHY_CA_BW_NRB_50_V01 : return 10000; // 10 MHz bandwidth
        case NAS_LTE_CPHY_CA_BW_NRB_75_V01 : return 15000; // 15 MHz bandwidth
        case NAS_LTE_CPHY_CA_BW_NRB_100_V01: return 20000; // 20 MHz bandwidth
        default:
            QCRIL_LOG_ERROR("Unknown bw value");
            return INT_MAX;
    }
}

void NasModule::handleSetSignalStrengthCriteriaSupported(
        std::shared_ptr<NasSetSignalStrengthCriteriaSupported> msg)
{
    mSetSignalStrengthCriteriaSupported = msg->isSupported();
}

inline bool NasModule::shouldReportScell(const nas_lte_cphy_scell_info_type_v01 &scell_info) {
    return (scell_info.scell_state == NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_ACTIVATED_V01 ||
            scell_info.scell_state == NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED_V01);
}

inline int32_t NasModule::getBandwidth(nas_radio_if_enum_v01 radio_if) {
    if (radio_if == NAS_RADIO_IF_LTE_V01 && chaninfo.pcell_info_valid) { // LTE CA
        return toKhz(chaninfo.pcell_info.cphy_ca_dl_bandwidth);
    } else if (chaninfo.bandwidth_info_list_valid) { // LTE non-CA and all other RATs
        unsigned int i = 0;
        for (; i < chaninfo.bandwidth_info_list_len; i++) {
            if (chaninfo.bandwidth_info_list[i].radio_if == radio_if) {
                return toKhz(chaninfo.bandwidth_info_list[i].bandwidth);
            }
        }

        if (i == chaninfo.bandwidth_info_list_len) {
            QCRIL_LOG_DEBUG("Couldn't find LTE bandwidth in bandwidth info list.");
        }
    }

    return std::numeric_limits<int32_t>::max();
}

inline int32_t NasModule::getBandwidth() {
    if (chaninfo.radio_if_valid) {
        return getBandwidth(chaninfo.radio_if);
    }
    return std::numeric_limits<int32_t>::max();
}

inline int32_t toRilEutranBand(nas_active_band_enum_v01 qmi_band) {
    switch(qmi_band) {
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_1_V01:
            return EUTRAN_BAND_1;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_2_V01:
            return EUTRAN_BAND_2;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_3_V01:
            return EUTRAN_BAND_3;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_4_V01:
            return EUTRAN_BAND_4;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_5_V01:
            return EUTRAN_BAND_5;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_6_V01:
            return EUTRAN_BAND_6;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_7_V01:
            return EUTRAN_BAND_7;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_8_V01:
            return EUTRAN_BAND_8;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_9_V01:
            return EUTRAN_BAND_9;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_10_V01:
            return EUTRAN_BAND_10;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_11_V01:
            return EUTRAN_BAND_11;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_12_V01:
            return EUTRAN_BAND_12;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_13_V01:
            return EUTRAN_BAND_13;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_14_V01:
            return EUTRAN_BAND_14;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_17_V01:
            return EUTRAN_BAND_17;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_33_V01:
            return EUTRAN_BAND_33;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_34_V01:
            return EUTRAN_BAND_34;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_35_V01:
            return EUTRAN_BAND_35;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_36_V01:
            return EUTRAN_BAND_36;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_37_V01:
            return EUTRAN_BAND_37;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_38_V01:
            return EUTRAN_BAND_38;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_39_V01:
            return EUTRAN_BAND_39;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01:
            return EUTRAN_BAND_40;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_18_V01:
            return EUTRAN_BAND_18;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_19_V01:
            return EUTRAN_BAND_19;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_20_V01:
            return EUTRAN_BAND_20;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_21_V01:
            return EUTRAN_BAND_21;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_24_V01:
            return EUTRAN_BAND_24;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_25_V01:
            return EUTRAN_BAND_25;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_41_V01:
            return EUTRAN_BAND_41;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_42_V01:
            return EUTRAN_BAND_42;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_43_V01:
            return EUTRAN_BAND_43;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_23_V01:
            return EUTRAN_BAND_23;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_26_V01:
            return EUTRAN_BAND_26;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_125_V01:
            return EUTRAN_BAND_125;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_126_V01:
            return EUTRAN_BAND_126;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_127_V01:
            return EUTRAN_BAND_127;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_28_V01:
            return EUTRAN_BAND_28;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_29_V01:
            return EUTRAN_BAND_29;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_30_V01:
            return EUTRAN_BAND_30;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_66_V01:
            return EUTRAN_BAND_66;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_250_V01:
            return EUTRAN_BAND_250;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_46_V01:
            return EUTRAN_BAND_46;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_27_V01:
            return EUTRAN_BAND_27;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_31_V01:
            return EUTRAN_BAND_31;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_71_V01:
            return EUTRAN_BAND_71;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_47_V01:
            return EUTRAN_BAND_47;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_48_V01:
            return EUTRAN_BAND_48;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_67_V01:
            return EUTRAN_BAND_67;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_68_V01:
            return EUTRAN_BAND_68;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_49_V01:
            return EUTRAN_BAND_49;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_85_V01:
            return EUTRAN_BAND_85;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_72_V01:
            return EUTRAN_BAND_72;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_73_V01:
            return EUTRAN_BAND_73;
        case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_32_V01:
            return EUTRAN_BAND_32;
        default:
            return std::numeric_limits<int32_t>::max();
    }
}

inline int32_t toRilNgranBand(nas_active_band_enum_v01 qmi_band) {
    switch(qmi_band) {
        case NAS_ACTIVE_BAND_NR5G_BAND_1_V01:
            return NGRAN_BAND_1;
        case NAS_ACTIVE_BAND_NR5G_BAND_2_V01:
            return NGRAN_BAND_2;
        case NAS_ACTIVE_BAND_NR5G_BAND_3_V01:
            return NGRAN_BAND_3;
        case NAS_ACTIVE_BAND_NR5G_BAND_5_V01:
            return NGRAN_BAND_5;
        case NAS_ACTIVE_BAND_NR5G_BAND_7_V01:
            return NGRAN_BAND_7;
        case NAS_ACTIVE_BAND_NR5G_BAND_8_V01:
            return NGRAN_BAND_8;
        case NAS_ACTIVE_BAND_NR5G_BAND_20_V01:
            return NGRAN_BAND_20;
        case NAS_ACTIVE_BAND_NR5G_BAND_28_V01:
            return NGRAN_BAND_28;
        case NAS_ACTIVE_BAND_NR5G_BAND_38_V01:
            return NGRAN_BAND_38;
        case NAS_ACTIVE_BAND_NR5G_BAND_41_V01:
            return NGRAN_BAND_41;
        case NAS_ACTIVE_BAND_NR5G_BAND_50_V01:
            return NGRAN_BAND_50;
        case NAS_ACTIVE_BAND_NR5G_BAND_51_V01:
            return NGRAN_BAND_51;
        case NAS_ACTIVE_BAND_NR5G_BAND_66_V01:
            return NGRAN_BAND_66;
        case NAS_ACTIVE_BAND_NR5G_BAND_70_V01:
            return NGRAN_BAND_70;
        case NAS_ACTIVE_BAND_NR5G_BAND_71_V01:
            return NGRAN_BAND_71;
        case NAS_ACTIVE_BAND_NR5G_BAND_74_V01:
            return NGRAN_BAND_74;
        case NAS_ACTIVE_BAND_NR5G_BAND_75_V01:
            return NGRAN_BAND_75;
        case NAS_ACTIVE_BAND_NR5G_BAND_76_V01:
            return NGRAN_BAND_76;
        case NAS_ACTIVE_BAND_NR5G_BAND_77_V01:
            return NGRAN_BAND_77;
        case NAS_ACTIVE_BAND_NR5G_BAND_78_V01:
            return NGRAN_BAND_78;
        case NAS_ACTIVE_BAND_NR5G_BAND_79_V01:
            return NGRAN_BAND_79;
        case NAS_ACTIVE_BAND_NR5G_BAND_80_V01:
            return NGRAN_BAND_80;
        case NAS_ACTIVE_BAND_NR5G_BAND_81_V01:
            return NGRAN_BAND_81;
        case NAS_ACTIVE_BAND_NR5G_BAND_82_V01:
            return NGRAN_BAND_82;
        case NAS_ACTIVE_BAND_NR5G_BAND_83_V01:
            return NGRAN_BAND_83;
        case NAS_ACTIVE_BAND_NR5G_BAND_84_V01:
            return NGRAN_BAND_84;
        case NAS_ACTIVE_BAND_NR5G_BAND_85_V01:
            return NGRAN_BAND_85;
        case NAS_ACTIVE_BAND_NR5G_BAND_257_V01:
            return NGRAN_BAND_257;
        case NAS_ACTIVE_BAND_NR5G_BAND_258_V01:
            return NGRAN_BAND_258;
        case NAS_ACTIVE_BAND_NR5G_BAND_259_V01:
            return NGRAN_BAND_259;
        case NAS_ACTIVE_BAND_NR5G_BAND_260_V01:
            return NGRAN_BAND_260;
        case NAS_ACTIVE_BAND_NR5G_BAND_261_V01:
            return NGRAN_BAND_261;
        case NAS_ACTIVE_BAND_NR5G_BAND_12_V01:
            return NGRAN_BAND_12;
        case NAS_ACTIVE_BAND_NR5G_BAND_25_V01:
            return NGRAN_BAND_25;
        case NAS_ACTIVE_BAND_NR5G_BAND_34_V01:
            return NGRAN_BAND_34;
        case NAS_ACTIVE_BAND_NR5G_BAND_39_V01:
            return NGRAN_BAND_39;
        case NAS_ACTIVE_BAND_NR5G_BAND_40_V01:
            return NGRAN_BAND_40;
        case NAS_ACTIVE_BAND_NR5G_BAND_65_V01:
            return NGRAN_BAND_65;
        case NAS_ACTIVE_BAND_NR5G_BAND_86_V01:
            return NGRAN_BAND_86;
        case NAS_ACTIVE_BAND_NR5G_BAND_48_V01:
            return NGRAN_BAND_48;
        default:
            return std::numeric_limits<int32_t>::max();
    }
}

inline int32_t NasModule::getBand(nas_radio_if_enum_v01 radio_if) {
    if (chaninfo.bandwidth_info_list_valid) {
        unsigned int i = 0;
        for (; i < chaninfo.band_info_list_ext_len; i++) {
            if (chaninfo.band_info_list_ext[i].radio_if == radio_if &&
                radio_if == NAS_RADIO_IF_LTE_V01) {
                return (int32_t)toRilEutranBand(chaninfo.band_info_list_ext[i].active_band);
            }
            else if (chaninfo.band_info_list_ext[i].radio_if == radio_if &&
                     radio_if == NAS_RADIO_IF_NR5G_V01) {
                return (int32_t)toRilNgranBand(chaninfo.band_info_list_ext[i].active_band);
            }
        }

        if (i == chaninfo.band_info_list_ext_len) {
            QCRIL_LOG_DEBUG("Couldn't find LTE Band in band info list.");
        }
    }

    return std::numeric_limits<int32_t>::max();
}

inline int32_t NasModule::getBand() {
    if (chaninfo.radio_if_valid) {
        return getBand(chaninfo.radio_if);
    }
    return std::numeric_limits<int32_t>::max();
}

std::shared_ptr<std::vector<NasPhysChanInfo>> NasModule::getCurrentPhysChanConfig() {
    std::shared_ptr<std::vector<NasPhysChanInfo>> physChanInfo = std::make_shared<std::vector<NasPhysChanInfo>>();

    if(physChanInfo == nullptr) {
        Log::getInstance().d("[NasModule]: getCurrentPhysChanConfig: Failed to create vector.");
        return physChanInfo;
    }

    if (chaninfo.physical_cell_id_valid) {
        physChanInfo->push_back(
            NasPhysChanInfo {
                NasPhysChanInfo::Status::PRIMARY,
                getBandwidth(),
                chaninfo.physical_cell_id
            }
        );
    }

    if (chaninfo.scell_info_list_valid) { // Only applicable to LTE CA
        for (size_t i = 0; i < chaninfo.scell_info_list_len; i++) {
            if (shouldReportScell(chaninfo.scell_info_list[i].scell_info)) {
                physChanInfo->push_back(
                    NasPhysChanInfo {
                        NasPhysChanInfo::Status::SECONDARY,
                        toKhz(chaninfo.scell_info_list[i].scell_info.cphy_ca_dl_bandwidth),
                        chaninfo.scell_info_list[i].scell_info.pci
                    }
                );
            }
        }
    }

    return physChanInfo;
}

void NasModule::reportCurrentPhysChanConfig() {
    QCRIL_LOG_FUNC_ENTRY();

    /* If the physical channel configuration obtained from the modem
       is the same as the physical channel configuration in the cache,
       do not send an update. */
    if (!getPhysChanConfigFromModem()) {
        return;
    }

    std::shared_ptr<std::vector<NasPhysChanInfo>> physChanInfo = getCurrentPhysChanConfig();

    if(physChanInfo == nullptr) {
        Log::getInstance().d("[NasModule]: reportCurrentPhysChanConfig: physChanInfo is null");
    }

    if (physChanInfo != nullptr && !physChanInfo->empty()) {
        auto msg = std::make_shared<NasPhysChanConfigMessage>(physChanInfo);
        if (msg) {
            msg->broadcast();
        } else {
            QCRIL_LOG_DEBUG("Unable to allocate msg");
        }
    }
    QCRIL_LOG_FUNC_RETURN();
}

int32_t NasModule::getLteBw() {
    int32_t bandwidth = INT_MAX;

    // if the bandwidth information in the cache isn't valid query the modem
    // for the bandwidth information
    if (!chaninfo.pcell_info_valid && !chaninfo.bandwidth_info_list_valid) {
        getPhysChanConfigFromModem();
    }

    bandwidth = getBandwidth(NAS_RADIO_IF_LTE_V01);
    QCRIL_LOG_ESSENTIAL("returning bandwidth: %d", bandwidth);
    return bandwidth;
}

int32_t NasModule::getNgranBw() {
    int32_t bandwidth = INT_MAX;

    // if the bandwidth information in the cache isn't valid query the modem
    // for the bandwidth information
    if (!chaninfo.pcell_info_valid && !chaninfo.bandwidth_info_list_valid) {
        getPhysChanConfigFromModem();
    }

    bandwidth = getBandwidth(NAS_RADIO_IF_NR5G_V01);
    QCRIL_LOG_ESSENTIAL("returning bandwidth: %d", bandwidth);
    return bandwidth;
}

int32_t NasModule::getLteBand() {
    int32_t band = INT_MAX;

    // if the band information in the cache isn't valid query the modem
    // for the band information
    if (!chaninfo.pcell_info_valid && !chaninfo.bandwidth_info_list_valid) {
        getPhysChanConfigFromModem();
    }

    band = getBand(NAS_RADIO_IF_LTE_V01);
    QCRIL_LOG_ESSENTIAL("returning band : %d", band);
    return band;
}

int32_t NasModule::getNgranBand() {
    int32_t band = INT_MAX;

    // if the band information in the cache isn't valid query the modem
    // for the band information
    if (!chaninfo.pcell_info_valid && !chaninfo.bandwidth_info_list_valid) {
        getPhysChanConfigFromModem();
    }

    band = getBand(NAS_RADIO_IF_NR5G_V01);
    QCRIL_LOG_ESSENTIAL("returning band : %d", band);
    return band;
}

void NasModule::resetPhysChanConfig()
{
    QCRIL_LOG_FUNC_ENTRY();
    if (chaninfo.compareAndAssign({})) {
        reportCurrentPhysChanConfig();
    }
    QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleQmiNasRfBandInfoMessage(
        std::shared_ptr<QmiNasRfBandInfoMessage> bandInfoMsg)
{
    QCRIL_LOG_FUNC_ENTRY();

    if (bandInfoMsg) {
      const nas_rf_band_info_ind_msg_v01 bandInfo = bandInfoMsg->getPayload();
      auto msg = std::make_shared<rildata::NasRfBandInfoIndMessage>(bandInfo);
      if( msg != nullptr ) {
        QCRIL_LOG_DEBUG("broadcasting message NasRfBandInfoIndMessage to DataModule");
        msg->broadcast();
      } else {
        QCRIL_LOG_DEBUG("msg is NULL. Failed to broadcast NasRfBandInfoIndMessage to DataModule");
      }
    } else {
      QCRIL_LOG_ERROR("NULL bandInfoMsg");
    }
    reportCurrentPhysChanConfig();
    QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleQmiNasLteCphyCaIndMessage(
        QCRIL_UNUSED(std::shared_ptr<QmiNasLteCphyCaIndMessage> lteCphyMsg))
{
    QCRIL_LOG_FUNC_ENTRY();
    reportCurrentPhysChanConfig();
    QCRIL_LOG_FUNC_RETURN();
}

bool NasModule::getPhysChanConfigFromModem() {
    std::shared_ptr<NasModemEndPoint> nasModemEndpoint = ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint();
    if (nasModemEndpoint) {
        PhysChanInfo phyChanInfo = {};
        nasModemEndpoint->getPhysChanConfig(phyChanInfo);
        return chaninfo.compareAndAssign(phyChanInfo);
    }
    return false;
}

void NasModule::handleEnablePhysChanConfigReporting(
        std::shared_ptr<NasEnablePhysChanConfigReporting> enableCfgMsg)
{
    if (!enableCfgMsg) {
        QCRIL_LOG_ERROR("No message");
        return;
    }
    auto ctx = enableCfgMsg->getContext();
    bool enable = enableCfgMsg->isEnable();
    auto nasModemEndPoint = ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint();

    if (nasModemEndPoint) {
        nasModemEndPoint->registerForIndications(
                { { QMI_NAS_RF_BAND_INFO_IND_V01, enable },
                  { QMI_NAS_LTE_CPHY_CA_IND_V01,  enable },
                },
                ctx,
                [enableCfgMsg, ctx, this]
                (std::shared_ptr<Message> msg,
                    Message::Callback::Status status,
                    std::shared_ptr<QmiNasRegisterForIndResponse> rsp)
                {
                        QCRIL_NOTUSED(rsp);
                        auto lmsg = std::static_pointer_cast<QmiNasRegisterForIndRequest>(msg);
                        if (lmsg) {
                            QCRIL_LOG_INFO("Status: %d", static_cast<int>(status));
                                        qcril::interfaces::NasSettingResultCode rc =
                                            qcril::interfaces::NasSettingResultCode::FAILURE;
                                        RIL_Errno e = RIL_E_INTERNAL_ERR;
                                        if (status == Message::Callback::Status::SUCCESS &&
                                                rsp) {
                                            e =
                                                qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                                                        rsp->err,
                                                        &rsp->payload.resp
                                                        );
                                            if (e == RIL_E_SUCCESS) {
                                                rc = qcril::interfaces::NasSettingResultCode::SUCCESS;
                                            }
                                        }
                                        QCRIL_LOG_INFO("Status: %d RIL_Errno: %d",
                                                static_cast<int>(status),
                                                e);
                                        enableCfgMsg->sendResponse(enableCfgMsg,
                                                status,
                                                std::make_shared<qcril::interfaces::NasSettingResultCode>(rc));
                        }
                        this->reportCurrentPhysChanConfig();
                });
    }
}


void NasModule::handleGetModemStackStatus(
    std::shared_ptr<RilRequestGetModemStackStatusMessage> msg)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_qmi_nas_get_modem_stack_status(msg);
    QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleEnableModem(
    std::shared_ptr<RilRequestEnableModemMessage> msg)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_qmi_nas_request_enable_modem_stack(msg);
    QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleGetSystemSelectionChannels(
    std::shared_ptr<RilRequestGetSysSelChannelsMessage> msg)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_qmi_nas_request_get_system_selection_channels(msg);
    QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleSetSystemSelectionChannels(
    std::shared_ptr<RilRequestSetSysSelChannelsMessage> msg)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_qmi_nas_request_set_system_selection_channels(msg);
    QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleGetPhoneCapability(
    std::shared_ptr<RilRequestGetPhoneCapabilityMessage> msg)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_qmi_nas_request_get_phone_capability(msg);
    QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleEnableUiccApplications(
    std::shared_ptr<RilRequestEnableUiccAppMessage> msg)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_qmi_prov_handle_sim_enable_request(msg);
    QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleGetUiccApplicationStatus(
    std::shared_ptr<RilRequestGetUiccAppStatusMessage> msg)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_qmi_prov_handle_get_sim_enable_status(msg);
    QCRIL_LOG_FUNC_RETURN();
}

void NasModule::handleNasGetRfBandInfoMessage(std::shared_ptr<rildata::NasGetRfBandInfoMessage> msg)
{
  if( msg == NULL ) {
    Log::getInstance().d("NasGetRfBandInfoMessage msg is NULL");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  rildata::NasActiveBand_t bandInfo = static_cast<rildata::NasActiveBand_t>(getNgranBand());
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
                    std::make_shared<rildata::NasActiveBand_t>(bandInfo));
}

void NasModule::handleSetNrDisableMode(std::shared_ptr<RilRequestSetNrDisableModeMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_nas_request_set_nr_disable_mode(msg);
}

void NasModule::handleQueryNrDisableMode(std::shared_ptr<RilRequestQueryNrDisableModeMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_nas_request_query_nr_disable_mode(msg);
}

void NasModule::handleGetEnhancedRadioCapability(std::shared_ptr<RilRequestGetEnhancedRadioCapabilityMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_nas_get_enhanced_radio_capability(msg);
}

void NasModule::handleGetLPlusLFeatureSupportStatusMessage(
    std::shared_ptr<RilRequestGetLPlusLFeatureSupportStatusMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  auto result = std::make_shared<qcril::interfaces::RilLPlusLFeatureSupportStatus_t>(false);
  RIL_Errno errNo = RIL_E_GENERIC_FAILURE;
  if (result) {
    errNo = qcril_qmi_nas_get_l_plus_l_feature_support_status(result->mSupported);
  }
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
      std::make_shared<QcRilRequestMessageCallbackPayload>(errNo, result));
}

void NasModule::handleGetPreferredNeworkBandPrefMessage(
    std::shared_ptr<RilRequestGetPreferredNeworkBandPrefMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  qcril_qmi_nas_request_get_preferred_network_band_pref(msg);
}

void NasModule::handleSetPreferredNeworkBandPrefMessage(
    std::shared_ptr<RilRequestSetPreferredNeworkBandPrefMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  qcril_qmi_nas_request_set_preferred_network_band_pref(msg);
}

void NasModule::handleSetUiStatusMessage( std::shared_ptr<RilRequestSetUiStatusMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  qcril_qmi_nas_oem_hook_set_atel_ui_status_to_modem(msg);
}

void NasModule::handleGetCsgIdMessage(std::shared_ptr<RilRequestGetCsgIdMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  auto result = std::make_shared<qcril::interfaces::GetCsgIdResult_t>(QCRIL_CSG_ID_INVALID);
  RIL_Errno errNo = RIL_E_GENERIC_FAILURE;
  if (result) {
    errNo = qcril_qmi_nas_get_csg_id(result->mCsgId);
  }
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
      std::make_shared<QcRilRequestMessageCallbackPayload>(errNo, result));
}

void NasModule::handleCsgPerformNetworkScanMessage(
    std::shared_ptr<RilRequestCsgPerformNetworkScanMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  qcril_qmi_nas_csg_perform_network_scan(msg);
}

void NasModule::handleCsgSetSysSelPrefMessage(
    std::shared_ptr<RilRequestCsgSetSysSelPrefMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  qcril_qmi_nas_csg_set_system_selection_preference(msg);
}

void NasModule::handleSetRfmScenarioMessage(std::shared_ptr<RilRequestSetRfmScenarioMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  qcril_qmi_nas_set_rfm_scenario_req(msg);
}

void NasModule::handleGetRfmScenarioMessage(std::shared_ptr<RilRequestGetRfmScenarioMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  qcril_qmi_nas_get_rfm_scenario_req(msg);
}

void NasModule::handleGetProvTableRevMessage(std::shared_ptr<RilRequestGetProvTableRevMessage> msg) {
  Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
  qcril_qmi_nas_get_provisioned_table_revision_req(msg);
}

void NasModule::handleSmsNotifyRegisteredOnImsMessage(
        std::shared_ptr<QcRilSmsNotifyRegisteredOnImsMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_nas_set_registered_on_ims(msg->isRegistered());
}

void NasModule::handleVoiceNotifyEmergencyCallEndMessage(
        std::shared_ptr<QcRilVoiceNotifyEmergencyCallEndMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_nas_connected_emergency_call_end_hdlr(msg);
}

void NasModule::handleVoiceNotifyCallInProcessMessage(
        std::shared_ptr<QcRilVoiceNotifyCallInProcessMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    if (msg->getAlertingCsCall()) {
        qcril_qmi_nas_invalidate_data_snapshot_in_case_of_csfb_in_alerting_state();
    } else {
        qcril_qmi_nas_wave_data_reg_in_case_of_csfb(TRUE);
    }
}

void NasModule::handleUnsolVoiceNumberOfCsCallChangedMessage(
        std::shared_ptr<QcRilUnsolVoiceNumberOfCsCallChangedMessage> /*msg*/) {
    qcril_qmi_nas_initiate_voice_rte_change_propagation();
}

void NasModule::handleUnsolVoiceAllCsCallsEndedMessage(
        std::shared_ptr<QcRilUnsolVoiceAllCsCallsEndedMessage> /*msg*/) {
    qcril_qmi_nas_control_signal_nas_on_current_calls_change();
}

void NasModule::handleUnsolAssessEccNumberListMessage(
        std::shared_ptr<QcRilUnsolAssessEccNumberListMessage> msg) {
    qcril_qmi_nas_assess_emergency_number_list_handler(msg->ensureFreshCheck());
}

void NasModule::handleUnsolPbmInitializedMessage(
        std::shared_ptr<QcRilUnsolPbmInitializedMessage> /*msg*/) {
    qcril_qmi_nas_start_wait_for_pbm_ind_timer();
}

void NasModule::handleRilRequestGetSubProvisioningPrefMessage(
     std::shared_ptr<RilRequestGetSubProvisioningPrefMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_prov_get_sub_prov_pref_req_handler(msg);
}

void NasModule::handleRilRequestSetSubProvisioningPrefMessage(
     std::shared_ptr<RilRequestSetSubProvisioningPrefMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_prov_set_sub_prov_pref_req_handler(msg);
}

void NasModule::handleRilRequestGetSimIccidMessage(
     std::shared_ptr<RilRequestGetSimIccidMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_prov_get_sim_iccid_req_handler(msg);
}

void NasModule::handleSetMsimPreference(
      std::shared_ptr<RilRequestSetMsimPreferenceMessage> msg) {
    Log::getInstance().d("[NasModule]: Handling msg = " + msg->dump());
    qcril_qmi_nas_set_msim_preference(msg);
}
