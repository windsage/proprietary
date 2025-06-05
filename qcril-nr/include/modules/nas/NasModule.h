/******************************************************************************
#  Copyright (c) 2017-2018, 2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include "modules/qmi/NasModemEndPoint.h"
#include "modules/qmi/QmiAsyncResponseMessage.h"
#include "modules/qmi/EndpointStatusIndMessage.h"

#include "modules/nas/NasUimHeaders.h"
#include "modules/nas/NasRequestDataShutdownMessage.h"

#include <modules/android/RilRequestMessage.h>
#include <modules/ims/ImsActiveSubStatusInd.h>
#include <modules/ims/ImsServiceStatusInd.h>
#include <modules/ims/ImsWfcSettingsStatusInd.h>
#include <modules/ims/ImsPdpStatusInd.h>
#include <interfaces/nas/RilRequestGetVonrCapRequest.h>
#include <interfaces/nas/RilRequestSetVonrCapRequest.h>
#include "modules/nas/NasSetVoiceDomainPreferenceRequest.h"

// NAS exported API
#include "modules/nas/NasSetPsAttachDetachMessage.h"
#include "modules/nas/NasGetSysInfoMessage.h"
#include "interfaces/nas/QcRilGetMaxModemSubscriptionsSyncMessage.h"
#include "interfaces/nas/QcRilGetPowerOptEnabledSyncMessage.h"
#include "interfaces/nas/QcRilGetUIStatusSyncMessage.h"
#include "interfaces/nas/QcRilNasQuery1XSmsBlockedStatusSyncMessage.h"
#include "interfaces/nas/QcRilGetCurrentMccMncSyncMessage.h"
#include "interfaces/nas/QcRilGetLteSmsStatusSyncMessage.h"
#include "interfaces/nas/QcRilNasQueryIsVoiceOverLteRegisteredSyncMessage.h"
#include "interfaces/nas/QcRilNasQueryIsDeviceRegisteredCsDomainSyncMessage.h"
#include "interfaces/nas/QcRilNasQueryIsLteOnlyRegisteredSyncMessage.h"
#include "interfaces/nas/QcRilNasQuerySimulVoiceDataCapabilitySyncMessage.h"
#include "interfaces/nas/QcRilNasQueryReportedDataTechnologySyncMessage.h"
#include "interfaces/nas/QcRilNasCheckCallModeAgainstDevCfgSyncMessage.h"
#include "interfaces/nas/QcRilNasGetCallServiceTypeSyncMessage.h"
#include "interfaces/nas/QcRilNasGetVoiceRadioTechSyncMessage.h"
#include "interfaces/nas/QcRilNasQueryImsAvailableSyncMessage.h"
#include "interfaces/nas/QcRilNasQueryScreenOffSyncMessage.h"

#include "interfaces/nas/QcRilNasGetEscvTypeSyncMessage.h"
#include "interfaces/nas/QcRilNasGetImsAddressFromEmergencyNumberSyncMessage.h"
#include "interfaces/nas/QcRilNasGetRegistrationStatusOverviewSyncMessage.h"
#include "interfaces/nas/QcRilNasGetRoamStatusOverviewSyncMessage.h"
#include "interfaces/nas/QcRilNasQueryEmergencyNumberDisplayOnlySyncMessage.h"
#include "interfaces/nas/QcRilNasQueryEnforcableEmergencyNumberSyncMessage.h"
#include "interfaces/nas/QcRilNasQueryNormalVoiceCallCapabilitySyncMessage.h"
#include "interfaces/nas/QcRilNasQueryNwMccBasedRoamingEmergencyNumberSyncMessage.h"
#include "interfaces/nas/QcRilNasQueryWhetherInApmLeaveWindowSyncMessage.h"
#include "interfaces/nas/QcRilNasGetRilEccMapSyncMessage.h"

//interface header files
#include "interfaces/nas/NasPhysChanConfigMessage.h"
#include "interfaces/nas/RilRequestGetVoiceRegistrationMessage.h"
#include "interfaces/nas/RilRequestGetDataRegistrationMessage.h"
#include "interfaces/nas/RilRequestOperatorMessage.h"
#include "interfaces/nas/RilRequestQueryNetworkSelectModeMessage.h"
#include "interfaces/nas/RilRequestAllowDataMessage.h"
#include "interfaces/nas/RilRequestQueryAvailNetworkMessage.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionManualMessage.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionAutoMessage.h"
#include "interfaces/nas/RilRequestStopNetworkScanMessage.h"
#include "interfaces/nas/RilRequestStartNetworkScanMessage.h"
#include "interfaces/nas/RilRequestGetPrefNetworkTypeMessage.h"
#include "interfaces/nas/RilRequestSetPrefNetworkTypeMessage.h"
#include "interfaces/nas/RilRequestGetSignalStrengthMessage.h"
#include "interfaces/nas/RilRequestGetVoiceRadioTechMessage.h"
#include "interfaces/nas/RilRequestSetCdmaSubscriptionSourceMessage.h"
#include "interfaces/nas/RilRequestExitEmergencyCallbackMessage.h"
#include "interfaces/nas/RilRequestGetNeighborCellIdsMessage.h"
#include "interfaces/nas/RilRequestCdmaSubscriptionMessage.h"
#include "interfaces/nas/RilRequestSetCdmaRoamingPrefMessage.h"
#include "interfaces/nas/RilRequestQueryCdmaRoamingPrefMessage.h"
#include "interfaces/nas/RilRequestGetCdmaSubscriptionSourceMessage.h"
#include "interfaces/nas/RilRequestSetLocationUpdateMessage.h"
#include "interfaces/nas/RilRequestQueryAvailBandModeMessage.h"
#include "interfaces/nas/RilRequestSetBandModeMessage.h"
#include "interfaces/nas/RilRequestSetUiccSubsMessage.h"
#include "interfaces/nas/RilRequestGetRadioCapMessage.h"
#include "interfaces/nas/RilRequestSetRadioCapMessage.h"
#include "interfaces/nas/RilRequestShutDownMessage.h"
#include "interfaces/nas/RilRequestEnableModemMessage.h"
#include "interfaces/nas/RilRequestGetModemStackStatusMessage.h"
#include "interfaces/nas/RilRequestGetSysSelChannelsMessage.h"
#include "interfaces/nas/RilRequestSetSysSelChannelsMessage.h"
#include "interfaces/nas/RilRequestGetPhoneCapabilityMessage.h"
#include "interfaces/nas/NasEnablePhysChanConfigReporting.h"
#include "interfaces/nas/NasSetSignalStrengthCriteriaSupported.h"
#include "interfaces/nas/RilRequestSetUnsolCellInfoListRateMessage.h"
#include "interfaces/nas/RilRequestEnableUiccAppMessage.h"
#include "interfaces/nas/RilRequestGetUiccAppStatusMessage.h"
#include "interfaces/nas/RilRequestSetUnsolRespFilterMessage.h"
#include "interfaces/nas/RilRequestGetBarringInfoMessage.h"
#include "interfaces/nas/RilUnsolCellBarringMessage.h"
#include "interfaces/nas/RilRequestGetLPlusLFeatureSupportStatusMessage.h"
#include "interfaces/nas/RilRequestSetUiStatusMessage.h"
#include "interfaces/nas/RilRequestGetPreferredNeworkBandPrefMessage.h"
#include "interfaces/nas/RilRequestSetPreferredNeworkBandPrefMessage.h"
#include "interfaces/nas/RilRequestGetCsgIdMessage.h"
#include "interfaces/nas/RilRequestCsgPerformNetworkScanMessage.h"
#include "interfaces/nas/RilRequestCsgSetSysSelPrefMessage.h"
#include "interfaces/nas/RilRequestEnableEngineerModeMessage.h"
#include "interfaces/rfrpe/RilRequestSetRfmScenarioMessage.h"
#include "interfaces/rfrpe/RilRequestGetRfmScenarioMessage.h"
#include "interfaces/rfrpe/RilRequestGetProvTableRevMessage.h"
#include "interfaces/sar/RilRequestGetSarRevKeyMessage.h"
#include "interfaces/sar/RilRequestSetTransmitPowerMessage.h"
#include "interfaces/nas/RilRequestGetSubProvisioningPrefMessage.h"
#include "interfaces/nas/RilRequestSetSubProvisioningPrefMessage.h"
#include "interfaces/nas/RilRequestGetSimIccidMessage.h"
#include "interfaces/nas/RilRequestGetUsageSettingMessage.h"
#include "interfaces/nas/RilRequestSetUsageSettingMessage.h"
#include "interfaces/nas/RilRequestSetMsimPreferenceMessage.h"


// 5G related
#include "interfaces/nas/RilRequestEnableEndcMessage.h"
#include "interfaces/nas/RilRequestQueryEndcStatusMessage.h"
#include "interfaces/nas/RilRequestSetNrDisableModeMessage.h"
#include "interfaces/nas/RilRequestQueryNrDisableModeMessage.h"
#include "interfaces/nas/RilRequestGetEnhancedRadioCapabilityMessage.h"

//Data Unsol Messages
#include "modules/nas/NasDataCache.h"
#include "UnSolMessages/CallStatusMessage.h"
#include "UnSolMessages/DataSysStatusMessage.h"
#include "UnSolMessages/SetPrefDataTechMessage.h"
#include "UnSolMessages/DsdSysStatusMessage.h"
#ifndef QMI_RIL_UTF
#include "UnSolMessages/DataRegistrationStateMessage.h"
#endif
#include "UnSolMessages/NewDDSInfoMessage.h"
#include "UnSolMessages/DDSStatusFollowupMessage.h"

#include "interfaces/sms/QcRilSmsNotifyRegisteredOnImsMessage.h"
#include "interfaces/voice/QcRilVoiceNotifyEmergencyCallEndMessage.h"
#include "interfaces/voice/QcRilVoiceNotifyCallInProcessMessage.h"
#include "interfaces/voice/QcRilUnsolVoiceNumberOfCsCallChangedMessage.h"
#include "interfaces/voice/QcRilUnsolVoiceAllCsCallsEndedMessage.h"
#include "interfaces/pbm/QcRilUnsolAssessEccNumberListMessage.h"
#include "interfaces/pbm/QcRilUnsolPbmInitializedMessage.h"

#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "framework/QcrilInitMessage.h"
#include <framework/AddPendingMessageList.h>

#include "request/NasGetRfBandInfoMessage.h"

int32_t toKhz(nas_bandwidth_enum_type_v01 bw);
int32_t toKhz(nas_lte_cphy_ca_bandwidth_enum_v01 bw);

class NasModule : public Module, public AddPendingMessageList {
  public:
    NasModule();
    ~NasModule();
    void init();
#ifdef QMI_RIL_UTF
    void qcrilHalNasModuleCleanup();
#endif

  private:
    bool mReady = false;
    bool mReceivedCardStatusEvent = false;
    qtimutex::QtiRecursiveMutex mMutex;
    bool mSetSignalStrengthCriteriaSupported = true;
    PhysChanInfo chaninfo;

  public:
    bool getSetSignalStrengthCriteriaSupported() {
        return mSetSignalStrengthCriteriaSupported;
    }
    int32_t getLteBw();
    int32_t getNgranBw();
    int32_t getLteBand();
    int32_t getNgranBand();
    void resetPhysChanConfig();
    void reportCurrentPhysChanConfig();
    bool isDmsEndpointReady();
  private:
    void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
    void handleNasQmiIndMessage(std::shared_ptr<Message> msg);
    void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);
    void handleNasEndpointStatusIndMessage(std::shared_ptr<Message> msg);

    void handleGetPreferredNetworkType(std::shared_ptr<RilRequestGetPrefNetworkTypeMessage> msg);
    void handleSetPreferredNetworkType(std::shared_ptr<RilRequestSetPrefNetworkTypeMessage> msg);
    void handleSetCdmaSubscriptionSource(std::shared_ptr<RilRequestSetCdmaSubscriptionSourceMessage> msg);
    void handleExitEmergencyCallbackMode(std::shared_ptr<RilRequestExitEmergencyCallbackMessage> msg);
    void handleGetVoiceRadioTech(std::shared_ptr<RilRequestGetVoiceRadioTechMessage> msg);
    void handleSetUiccSubscription(std::shared_ptr<RilRequestSetUiccSubsMessage> msg);
    void handleEnableEndc(std::shared_ptr<RilRequestEnableEndcMessage> msg);
    void handleQueryEndcStatus(std::shared_ptr<RilRequestQueryEndcStatusMessage> msg);

    void handleShutdown(std::shared_ptr<RilRequestShutDownMessage> msg);
    void handleGetRadioCapability(std::shared_ptr<RilRequestGetRadioCapMessage> msg);
    void handleSetRadioCapability(std::shared_ptr<RilRequestSetRadioCapMessage> msg);
    void handleQueryNetworkSelectionMode(std::shared_ptr<RilRequestQueryNetworkSelectModeMessage> msg);
    void handleSetBandMode(std::shared_ptr<RilRequestSetBandModeMessage> msg);
    void handleGetNeighboringCellIds(std::shared_ptr<RilRequestGetNeighborCellIdsMessage> msg);
    void handleSetNetworkSelectionAutomatic(
        std::shared_ptr<RilRequestSetNetworkSelectionAutoMessage> msg);
    void handleSetNetworkSelectionManual(
        std::shared_ptr<RilRequestSetNetworkSelectionManualMessage> msg);
    void handleVoiceRegistrationState(std::shared_ptr<RilRequestGetVoiceRegistrationMessage> msg);
    void handleDataRegistrationState(std::shared_ptr<RilRequestGetDataRegistrationMessage> msg);
    void handleOperator(std::shared_ptr<RilRequestOperatorMessage> msg);
    void handleAllowData(std::shared_ptr<RilRequestAllowDataMessage> msg);
    void handleCdmaSubscription(std::shared_ptr<RilRequestCdmaSubscriptionMessage> msg);
    void handleCdmaSetRoamingPreference(std::shared_ptr<RilRequestSetCdmaRoamingPrefMessage> msg);
    void handleSetLocationUpdates(std::shared_ptr<RilRequestSetLocationUpdateMessage> msg);
    void handleQueryAvailableNetworks(std::shared_ptr<RilRequestQueryAvailNetworkMessage> msg);
    void handleGetBarringInfo(std::shared_ptr<RilRequestGetBarringInfoMessage> msg);
    void handleQueryAvailableBandMode(std::shared_ptr<RilRequestQueryAvailBandModeMessage> msg);
    void handleSignalStrength(std::shared_ptr<RilRequestGetSignalStrengthMessage> msg);
    void handleSetUnsolRespFilter(std::shared_ptr<RilRequestSetUnsolRespFilterMessage> msg);
    void handleSetUnsolBarringFilter(std::shared_ptr<RilRequestSetUnsolBarringFilterMessage> msg);
    void handleStartNetworkScan(std::shared_ptr<RilRequestStartNetworkScanMessage> msg);
    void handleStopNetworkScan(std::shared_ptr<RilRequestStopNetworkScanMessage> msg);
    void handleGetModemStackStatus(
            std::shared_ptr<RilRequestGetModemStackStatusMessage> msg);
    void handleEnableModem(
            std::shared_ptr<RilRequestEnableModemMessage> msg);
    void handleGetSystemSelectionChannels(
            std::shared_ptr<RilRequestGetSysSelChannelsMessage> msg);
    void handleSetSystemSelectionChannels(
            std::shared_ptr<RilRequestSetSysSelChannelsMessage> msg);
    void handleGetPhoneCapability(
            std::shared_ptr<RilRequestGetPhoneCapabilityMessage> msg);
    void handleSetUnsolCellInfoListRateMessage(
            std::shared_ptr<RilRequestSetUnsolCellInfoListRateMessage> msg);
    void handleEnableUiccApplications(
            std::shared_ptr<RilRequestEnableUiccAppMessage> msg);
    void handleGetUiccApplicationStatus(
            std::shared_ptr<RilRequestGetUiccAppStatusMessage> msg);
    void handleCdmaQueryRoamingPreference(
            std::shared_ptr<RilRequestQueryCdmaRoamingPrefMessage> msg);
    void handleCdmaGetSubscriptionSource(
            std::shared_ptr<RilRequestGetCdmaSubscriptionSourceMessage> msg);
    void handleGetEnhancedRadioCapability(
             std::shared_ptr<RilRequestGetEnhancedRadioCapabilityMessage> msg);
    void handleGetLPlusLFeatureSupportStatusMessage(
             std::shared_ptr<RilRequestGetLPlusLFeatureSupportStatusMessage> msg);
    void handleSetUiStatusMessage(std::shared_ptr<RilRequestSetUiStatusMessage> msg);
    void handleSetPreferredNeworkBandPrefMessage(
             std::shared_ptr<RilRequestSetPreferredNeworkBandPrefMessage> msg);
    void handleGetPreferredNeworkBandPrefMessage(
             std::shared_ptr<RilRequestGetPreferredNeworkBandPrefMessage> msg);
    void handleGetCsgIdMessage(std::shared_ptr<RilRequestGetCsgIdMessage> msg);
    void handleCsgPerformNetworkScanMessage(
             std::shared_ptr<RilRequestCsgPerformNetworkScanMessage> msg);
    void handleCsgSetSysSelPrefMessage(std::shared_ptr<RilRequestCsgSetSysSelPrefMessage> msg);
    void handleSetRfmScenarioMessage(std::shared_ptr<RilRequestSetRfmScenarioMessage> msg);
    void handleGetRfmScenarioMessage(std::shared_ptr<RilRequestGetRfmScenarioMessage> msg);
    void handleGetProvTableRevMessage(std::shared_ptr<RilRequestGetProvTableRevMessage> msg);
    void handleEnableEngineerMode(std::shared_ptr<RilRequestEnableEngineerModeMessage> msg);
    void handleGetSarRevKey(std::shared_ptr<RilRequestGetSarRevKeyMessage> msg);
    void handleSetTransmitPower(std::shared_ptr<RilRequestSetTransmitPowerMessage> msg);
    void handleSetMsimPreference(std::shared_ptr<RilRequestSetMsimPreferenceMessage> msg);

    // 5G related:
    void handleSetNrDisableMode(std::shared_ptr<RilRequestSetNrDisableModeMessage> msg);
    void handleQueryNrDisableMode(std::shared_ptr<RilRequestQueryNrDisableModeMessage> msg);

    // UIM handling
    void handleUimCardStatusIndMsg(std::shared_ptr<UimCardStatusIndMsg> msg);
    void handleUimCardStatusChangeIndMsg(std::shared_ptr<UimCardStatusChangeIndMsg> msg);
    void handleUimCardAppStatusIndMsg(std::shared_ptr<UimCardAppStatusIndMsg> msg);

    // IMS handling
    void handleImsActiveSubStatusInd(std::shared_ptr<ImsActiveSubStatusInd> msg);
    void handleImsServiceStatusInd(std::shared_ptr<ImsServiceStatusInd> msg);
    void handleImsWfcStatusInd(std::shared_ptr<ImsWfcSettingsStatusInd> msg);
    void handleImsPdpStatusInd(std::shared_ptr<ImsPdpStatusInd> msg);
    void handleSetVoiceDomainPreferenceRequest(std::shared_ptr<NasSetVoiceDomainPreferenceRequest> msg);
    void handleSetVonrCapRequest(std::shared_ptr<RilRequestSetVonrCapRequest> msg);
    void handleGetVonrCapRequest(std::shared_ptr<RilRequestGetVonrCapRequest> msg);

    void handleGetPhyChanConfigMessage(std::shared_ptr<NasGetPhyChanConfigMessage> msg);

    //Data Indication Handlers
    void handleDataCallStatusMessage(std::shared_ptr<rildata::CallStatusMessage> msg);
    void handleDataSetPrefDataTechMessage(std::shared_ptr<rildata::SetPrefDataTechMessage> msg);
    void handleDataSysStatusMessage(std::shared_ptr<rildata::DataSysStatusMessage> msg);
    void handleDataDsdSysStatusMessage(std::shared_ptr<rildata::DsdSysStatusMessage> msg);
#ifndef QMI_RIL_UTF
    void handleDataRegistrationStateMessage(std::shared_ptr<rildata::DataRegistrationStateMessage> msg);
#endif
    void handleDataNewDDSInfoMessage(std::shared_ptr<rildata::NewDDSInfoMessage> msg);
    void handleDataDDSStatusFollowupMessage(std::shared_ptr<rildata::DDSStatusFollowupMessage> msg);

    // Nas API
    void handlePsAttachDetachMessage(std::shared_ptr<NasSetPsAttachDetachMessage> msg);
    void handleNasGetSysInfoMessage(std::shared_ptr<NasGetSysInfoMessage> msg);

    void handleGetMaxMaxModemSubscriptionsMessage(
            std::shared_ptr<QcRilGetMaxModemSubscriptionsSyncMessage> msg);

    void handleGetPowerOptEnabledSyncMessage(
            std::shared_ptr<QcRilGetPowerOptEnabledSyncMessage> msg);
    void handleGetUIStatusSyncMessage(
            std::shared_ptr<QcRilGetUIStatusSyncMessage> msg);
    void handleQuery1XSmsBlockedStatusSyncMessage(
            std::shared_ptr<QcRilNasQuery1XSmsBlockedStatusSyncMessage> msg);
    void handleGetCurrentMccMncSyncMessage(
            std::shared_ptr<QcRilGetCurrentMccMncSyncMessage> msg);
    void handleGetLteSmsStatusSyncMessage(
            std::shared_ptr<QcRilGetLteSmsStatusSyncMessage> msg);
    void handleQueryIsVoiceOverLteRegisteredSyncMessage(
            std::shared_ptr<QcRilNasQueryIsVoiceOverLteRegisteredSyncMessage> msg);
    void handleQueryIsDeviceRegisteredCsDomainSyncMessage(
            std::shared_ptr<QcRilNasQueryIsDeviceRegisteredCsDomainSyncMessage> msg);
    void handleQueryIsLteOnlyRegisteredSyncMessage(
            std::shared_ptr<QcRilNasQueryIsLteOnlyRegisteredSyncMessage> msg);
    void handleQuerySimulVoiceDataCapabilitySyncMessage(
            std::shared_ptr<QcRilNasQuerySimulVoiceDataCapabilitySyncMessage> msg);
    void handleQueryReportedDataTechnologySyncMessage(
            std::shared_ptr<QcRilNasQueryReportedDataTechnologySyncMessage> msg);
    void handleCheckCallModeAgainstDevCfgSyncMessage(
            std::shared_ptr<QcRilNasCheckCallModeAgainstDevCfgSyncMessage> msg);
    void handleGetCallServiceTypeSyncMessage(
            std::shared_ptr<QcRilNasGetCallServiceTypeSyncMessage> msg);
    void handleGetVoiceRadioTechSyncMessage(
            std::shared_ptr<QcRilNasGetVoiceRadioTechSyncMessage> msg);
    void handleQueryImsAvailableSyncMessage(
            std::shared_ptr<QcRilNasQueryImsAvailableSyncMessage> msg);
    void handleQueryScreenOffSyncMessage(
            std::shared_ptr<QcRilNasQueryScreenOffSyncMessage> msg);
    void handleGetEscvTypeSyncMessage(
            std::shared_ptr<QcRilNasGetEscvTypeSyncMessage> msg);
    void handleGetImsAddressFromEmergencyNumberSyncMessage(
            std::shared_ptr<QcRilNasGetImsAddressFromEmergencyNumberSyncMessage> msg);
    void handleGetRegistrationStatusOverviewSyncMessage(
            std::shared_ptr<QcRilNasGetRegistrationStatusOverviewSyncMessage> msg);
    void handleGetRoamStatusOverviewSyncMessage(
            std::shared_ptr<QcRilNasGetRoamStatusOverviewSyncMessage> msg);

    void handleQueryEmergencyNumberDisplayOnlySyncMessage(
            std::shared_ptr<QcRilNasQueryEmergencyNumberDisplayOnlySyncMessage> msg);
    void handleQueryEnforcableEmergencyNumberSyncMessage(
            std::shared_ptr<QcRilNasQueryEnforcableEmergencyNumberSyncMessage> msg);
    void handleQueryNormalVoiceCallCapabilitySyncMessage(
            std::shared_ptr<QcRilNasQueryNormalVoiceCallCapabilitySyncMessage> msg);
    void handleQueryNwMccBasedRoamingEmergencyNumberSyncMessage(
            std::shared_ptr<
            QcRilNasQueryNwMccBasedRoamingEmergencyNumberSyncMessage> msg);
    void handleQueryWhetherInApmLeaveWindowSyncMessage(
            std::shared_ptr<QcRilNasQueryWhetherInApmLeaveWindowSyncMessage> msg);
    void handleGetRilEccMapSyncMessage(
            std::shared_ptr<QcRilNasGetRilEccMapSyncMessage> msg);
    void handleRilRequestGetSubProvisioningPrefMessage(
             std::shared_ptr<RilRequestGetSubProvisioningPrefMessage> msg);
    void handleRilRequestSetSubProvisioningPrefMessage(
             std::shared_ptr<RilRequestSetSubProvisioningPrefMessage> msg);
    void handleRilRequestGetSimIccidMessage(
             std::shared_ptr<RilRequestGetSimIccidMessage> msg);

    // Handler for DMS Endpoint Status Indications
    void handleDmsEndpointStatusIndMsg(std::shared_ptr<Message> msg);

    // Settings
    void handleSetSignalStrengthCriteriaSupported(
            std::shared_ptr<NasSetSignalStrengthCriteriaSupported> msg);
    void handleEnablePhysChanConfigReporting(
            std::shared_ptr<NasEnablePhysChanConfigReporting> msg);
    std::shared_ptr<std::vector<NasPhysChanInfo>> getCurrentPhysChanConfig();

    void handleSmsNotifyRegisteredOnImsMessage(
            std::shared_ptr<QcRilSmsNotifyRegisteredOnImsMessage> msg);
    void handleVoiceNotifyEmergencyCallEndMessage(
            std::shared_ptr<QcRilVoiceNotifyEmergencyCallEndMessage> msg);
    void handleVoiceNotifyCallInProcessMessage(
            std::shared_ptr<QcRilVoiceNotifyCallInProcessMessage> msg);
    void handleUnsolVoiceNumberOfCsCallChangedMessage(
            std::shared_ptr<QcRilUnsolVoiceNumberOfCsCallChangedMessage> msg);
    void handleUnsolVoiceAllCsCallsEndedMessage(
            std::shared_ptr<QcRilUnsolVoiceAllCsCallsEndedMessage> msg);
    void handleUnsolAssessEccNumberListMessage(
            std::shared_ptr<QcRilUnsolAssessEccNumberListMessage> msg);
    void handleUnsolPbmInitializedMessage(
            std::shared_ptr<QcRilUnsolPbmInitializedMessage> msg);

    // Physical channel configuration
    inline bool shouldReportScell(const nas_lte_cphy_scell_info_type_v01 &scell_info);
    inline int32_t getBandwidth(nas_radio_if_enum_v01 radio_if);
    inline int32_t getBandwidth();
    inline int32_t getBand(nas_radio_if_enum_v01 radio_if);
    inline int32_t getBand();
    void handleQmiNasRfBandInfoMessage(
            std::shared_ptr<QmiNasRfBandInfoMessage> bandInfoMsg);
    void handleQmiNasLteCphyCaIndMessage(
            std::shared_ptr<QmiNasLteCphyCaIndMessage> lteCphyMsg);
    bool getPhysChanConfigFromModem();
    void handleNasGetRfBandInfoMessage(std::shared_ptr<rildata::NasGetRfBandInfoMessage> msg);

    //Usage Setting
    void handleGetUsageSetting(
        std::shared_ptr<RilRequestGetUsageSettingMessage>);
    void handleSetUsageSetting(
        std::shared_ptr<RilRequestSetUsageSettingMessage>);

};

NasModule &getNasModule();

