/**
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
* All rights reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <iostream>
#include <cstdlib>
#include "dlfcn.h"
#ifdef RIL_FOR_MDM_LE
#include <signal.h>
#include <time.h>
#include "request/CaptureLogBufferMessage.h"
#include "request/IndicationRegistrationFilterMessage.h"
#else
#include "UnSolMessages/VoiceIndMessage.h"
#include "UnSolMessages/VoiceCallOrigTimeoutMessage.h"
#include "VoiceCallModemEndPoint.h"
#endif
#include "qmi_client.h"
#include "wireless_data_service_v01.h"
#include "DsiWrapper.h"
#include "ds_util.h"
/* Framework includes */
#include "framework/Dispatcher.h"
#include "framework/Looper.h"
#include "framework/ModuleLooper.h"
#include "framework/QcrilInitMessage.h"
#include "framework/TimeKeeper.h"
#include "modules/qmi/QmiIndMessage.h"
#include "modules/qmi/EndpointStatusIndMessage.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include "qtibus/Messenger.h"
#include "data_system_determination_v01.h"

/* Module includes */
#include "local/DataModule.h"
#include "UnSolMessages/DataInitMessage.h"
#include "UnSolMessages/UpdateMtuMessage.h"

#ifndef RIL_FOR_LOW_RAM
#include "request/GetDataPPDataCapabilityMessage.h"
#include "request/UpdateDataPPDataUIOptionMessage.h"
#include "UnSolMessages/DataAllBearerTypeChangedMessage.h"
#include "UnSolMessages/DataBearerTypeChangedMessage.h"
#include "interfaces/nas/NasPhysChanConfigMessage.h"
#include "UnSolMessages/DataPPDataCapabilityChangeIndMessage.h"
#include "UnSolMessages/DataSubRecommendationIndMessage.h"
#endif

#include "request/IWLANCapabilityHandshake.h"
#include "request/GetAllQualifiedNetworkRequestMessage.h"
#include "request/GetIWlanDataCallListRequestMessage.h"
#include "request/GetIWlanDataRegistrationStateRequestMessage.h"
#include "UnSolMessages/IntentToChangeApnPreferredSystemMessage.h"
#include "UnSolMessages/HandoverInformationIndMessage.h"
#include "UnSolMessages/InformHandoverFailureMessage.h"

#include "request/StartKeepAliveRequestMessage.h"
#include "request/StopKeepAliveRequestMessage.h"
#include "UnSolMessages/KeepAliveIndMessage.h"

#include "request/SetApnInfoMessage.h"
#include "request/SetIsDataEnabledMessage.h"
#include "request/SetIsDataRoamingEnabledMessage.h"
#include "request/GoDormantRequestMessage.h"
#include "request/ProcessScreenStateChangeMessage.h"
#include "request/ProcessStackSwitchMessage.h"
#include "request/SetQualityMeasurementMessage.h"
#include "request/ToggleDormancyIndMessage.h"
#include "request/ToggleLimitedSysIndMessage.h"
#include "request/GetDdsSubIdMessage.h"
#include "request/RequestDdsSwitchMessage.h"

#include "request/SetLinkCapRptCriteriaMessage.h"
#include "request/SetLinkCapFilterMessage.h"
#include "request/SetInitialAttachApnRequestMessage.h"
#include "request/RegisterBearerAllocationUpdateRequestMessage.h"
#include "request/GetBearerAllocationRequestMessage.h"
#include "request/GetAllBearerAllocationsRequestMessage.h"
#include "request/SetupDataCallRequestMessage.h"
#include "request/DeactivateDataCallRequestMessage.h"
#include "request/GetRadioDataCallListRequestMessage.h"
#include "request/GetSlicingConfigRequestMessage.h"

#include "request/SetCarrierInfoImsiEncryptionMessage.h"
#include "request/SetDataThrottlingRequestMessage.h"

#include "event/RilEventDataCallback.h"
#include "event/LinkPropertiesChangedMessage.h"
#include "event/DdsSwitchRecommendInternalMessage.h"
#include "request/SetDataProfileRequestMessage.h"
#include "request/SetPreferredDataModemRequestMessage.h"
#include "request/StartLCERequestMessage.h"
#include "request/StopLCERequestMessage.h"
#include "request/PullLCEDataRequestMessage.h"
#include "request/GetSmartTempDdsSwitchCapabilityMessage.h"
#include "request/GetCIWlanCapabilityMessage.h"
#include "request/RegisterDataDuringVoiceMessage.h"
#include "request/UpdateUIOptionMessage.h"
#include "request/SetDataInActivityPeriodMessage.h"
#include "request/TcpMonitoringRequestMessage.h"
#include "UnSolMessages/SetLteAttachPdnListActionResultMessage.h"
#include "UnSolMessages/SetApnPreferredSystemResultMessage.h"
#include "UnSolMessages/PreferredApnUpdatedMessage.h"
#include "UnSolMessages/GetModemAttachParamsRetryMessage.h"

#include "UnSolMessages/DsdSystemStatusPerApnMessage.h"
#include "UnSolMessages/CallStatusMessage.h"
#include "UnSolMessages/CurrentDDSIndMessage.h"
#include "UnSolMessages/CurrentRoamingStatusChangedMessage.h"
#include "UnSolMessages/DDSSwitchResultIndMessage.h"
#include "UnSolMessages/DDSSwitchIPCMessage.h"
#include "UnSolMessages/IAInfoIPCMessage.h"
#include "UnSolMessages/DDSSwitchTimeoutMessage.h"
#include "UnSolMessages/RadioConfigClientConnectedMessage.h"
#include "UnSolMessages/InformDDSSUBChangedMessage.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "UnSolMessages/NewDDSInfoMessage.h"
#include "UnSolMessages/PduSessionParamLookupResultIndMessage.h"
#include "UnSolMessages/ProcessInitialAttachRequestMessage.h"
#include "UnSolMessages/IATimeoutMessage.h"
#include "DataConfig.h"
#include "UnSolMessages/DsdSystemStatusMessage.h"
#include "UnSolMessages/DsiInitCompletedMessage.h"
#include "UnSolMessages/NasRfBandInfoIndMessage.h"
#include "sync/ChangeStackIdSyncMessage.h"
#include "UnSolMessages/PdcRefreshIndication.h"
#include "UnSolMessages/WdsThrottleInfoIndMessage.h"
#include "UnSolMessages/GetPdnThrottleTimeResponseInd.h"
#include "UnSolMessages/QosInitializeMessage.h"
#include "UnSolMessages/RouteChangeUpdateMessage.h"
#include "UnSolMessages/AutoDdsSwitchControlIndMessage.h"
#include "UnSolMessages/DdsSwitchRecommendationIndMessage.h"
#include "UnSolMessages/CleanUpTcpKeepAliveMessage.h"
#include "event/SecureModeChangedCallBackMessage.h"
#include "UnSolMessages/RegisterForSecureModeCbMessage.h"
#include "UnSolMessages/QcrilDataClientConnectedMessage.h"
#include "UnSolMessages/QcrilDataClientDisconnectedMessage.h"
#include "UnSolMessages/ExitFromCurrentStateRequestMessage.h"
#include "DataConfig.h"
#include "qcril_data.h"

#include "AuthModemEndPoint.h"
#include "WDSModemEndPoint.h"
#include "DSDModemEndPoint.h"

#include "ProfileHandler.h"
#include "NetworkServiceHandler.h"

#include "telephony/ril.h"
#include "modules/nas/NasRequestDataShutdownMessage.h"
#include "interfaces/nas/QcRilUnsolCurrentPlmnChangedMessage.h"

#include "QdpAdapter.h"


#define INVALID_MSG_TOKEN -1
#define PROPERTY_DISABLE_PARTIAL_RETRY_DEFAULT  "false"

wds_bind_subscription_enum_v01 global_subs_id = WDS_DEFAULT_SUBS_V01;
int global_instance_id = QCRIL_DEFAULT_INSTANCE_ID;
unsigned int dataMtu = 0;
boolean global_qcril_clat_supported = false;
#include "modules/uim/UimCardAppStatusIndMsg.h"
#include "modules/uim/UimGetMccMncRequestMsg.h"
#include "modules/uim/UimGetCardStatusRequestSyncMsg.h"

#define MCC_LENGTH 4
#define MNC_LENGTH 4
#define LCE_NOT_SUPPORTED -1

static load_module<rildata::DataModule> dataModule;
ProfileHandler::RilRespParams ProfileHandler::m_resp_params={ QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, 0,0};

DECLARE_MSG_ID_INFO(VOICE_ENDPOINT_STATUS_IND);
DECLARE_MSG_ID_INFO(WDSModemEndPoint_ENDPOINT_STATUS_IND);
DECLARE_MSG_ID_INFO(DSDModemEndPoint_ENDPOINT_STATUS_IND);
DECLARE_MSG_ID_INFO(OTTModemEndPoint_ENDPOINT_STATUS_IND);
DECLARE_MSG_ID_INFO(TMDModemEndPoint_ENDPOINT_STATUS_IND);
DECLARE_MSG_ID_INFO(AUTH_QMI_IND);
DECLARE_MSG_ID_INFO(AUTH_ENDPOINT_STATUS_IND);
DEFINE_MSG_ID_INFO(VOICECALL_ENDPOINT_STATUS_IND)


using namespace qdp;
#ifndef RIL_FOR_MDM_LE
using namespace datactl;
#endif

namespace rildata {
#ifndef RIL_FOR_MDM_LE
typedef DataCtlInterface* initDataCtl();
#endif

DataModule& getDataModule() {
    return dataModule.get_module();
}

#ifdef RIL_FOR_MDM_LE

int SignalList[] = {SIGBUS, SIGABRT, SIGSEGV};

RIL_Errno handleRilDataDump(string name) {
  time_t system_time = time(0);
  char timestamp[10] = "";
  strftime (timestamp, 10,"%H.%M.%S", localtime(&system_time));
  std::string dumpsys_file_name = "Ril_Data_Dump_" + name + "_" + std::string(timestamp) + ".txt";
  FILE * dumpsys_file = fopen(dumpsys_file_name.c_str(), "w");
  if (dumpsys_file == nullptr)
  {
    fprintf(stderr, "Error cannot open dumpsys file.\n");
    return RIL_E_INTERNAL_ERR;
  }
  fprintf(dumpsys_file, "TimeStamp: %s", asctime(localtime(&system_time)));
  if (name != "CLI_USER") {
    fprintf(dumpsys_file, "Signal Received: %s\n" , name.c_str());
  } else {
    fprintf(dumpsys_file, "Trigerred Via: %s\n" , name.c_str());
  }
  if (fflush(dumpsys_file) != 0) {
    Log::getInstance().d("[DataModule]: Error in file flushing for Data Dump");
  }
  dump_data_module(fileno(dumpsys_file));
  if (dumpsys_file) {
    if (fclose(dumpsys_file) != 0) {
       Log::getInstance().d("[DataModule]: Error in file closing for Data Dump");
    }
    dumpsys_file = nullptr;
  }
  return RIL_E_SUCCESS;
}

void handleSignal(int sig) {
    std::cout << "\nDaemon is Crashing." << std::endl;
    string name = "";
    if (sig == SIGABRT) {
        name = "SIGABRT";
    }
    else if (sig == SIGBUS) {
        name = "SIGBUS";
    }
    else if (sig == SIGSEGV) {
        name = "SIGSEGV";
    }
    std::cout << "\nSignal Received: " + name << std::endl;
    handleRilDataDump(name);
    _exit(EXIT_SUCCESS);
}
#endif


DataModule::DataModule() : mMessageList("DataModule")  {
  mDataQosInit = nullptr;
#ifdef RIL_FOR_MDM_LE
  struct sigaction action = {};
  action.sa_handler = &handleSignal;
  for (int i : SignalList) {
    if (sigaction(i, &action, NULL) != 0) {
        Log::getInstance().d("[DataModule]: Signal Handler Registration Failed for Signal" + std::to_string(i));
    }
  }
#endif
  global_subs_id = WDS_DEFAULT_SUBS_V01;
  global_instance_id = QCRIL_DEFAULT_INSTANCE_ID;
  dataMtu = 0;
  global_qcril_clat_supported = false;
  std::string dsiex2Version = std::string(dsi_get_lib_version_info());
  Log::getInstance().d("[DataModule]: dsiex2version: " + dsiex2Version);
  if (dsiex2Version == "1.0.0") {
    isDsiex2Supported = true;
  }
  Log::getInstance().d("[DataModule]: isDsiex2Supported: " + std::to_string((int)isDsiex2Supported));
  mName = "DataModule";
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);
  iwlanHandshakeMsgToken = INVALID_MSG_TOKEN;
  wds_endpoint = ModemEndPointFactory<WDSModemEndPoint>::getInstance().buildEndPoint();
  dsd_endpoint = ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint();
  auth_endpoint = ModemEndPointFactory<AuthModemEndPoint>::getInstance().buildEndPoint();
  #ifdef FEATURE_DATA_LQE
    ott_endpoint = ModemEndPointFactory<OTTModemEndPoint>::getInstance().buildEndPoint();
  #endif /* FEATURE_DATA_LQE */
  pdc_endpoint = ModemEndPointFactory<PdcModemEndPoint>::getInstance().buildEndPoint();
  tmd_endpoint = ModemEndPointFactory<TMDModemEndPoint>::getInstance().buildEndPoint();

  preferred_data_sm = std::make_unique<PreferredDataStateMachine>();
  preferred_data_state_info = std::make_shared<PreferredDataInfo_t>();
  preferred_data_state_info->isRilIpcNotifier = false;
  preferred_data_state_info->isDataAllowedOnNDDS = false;
  preferred_data_state_info->mVoiceCallInfo.voiceSubId = SubId::UNSPECIFIED;
  preferred_data_state_info->tempDdsSwitched = false;
  preferred_data_sm->initialize(preferred_data_state_info);
  preferred_data_sm->setCurrentState(Initial);
  preferred_data_state_info->dds = INVALID_SUB_ID;
  preferred_data_state_info->currentDDSSwitchType = DSD_DDS_SWITCH_PERMANENT_V01;
  preferred_data_state_info->switch_type = DSD_DDS_SWITCH_PERMANENT_V01;
  #ifdef RIL_FOR_MDM_LE
  preferred_data_state_info->isRilIpcNotifier = true;
  #endif

  auth_manager = nullptr;
  call_manager = nullptr;
  profile_handler = nullptr;
  network_service_handler = nullptr;
  memset(&mCachedSystemStatus, 0, sizeof(dsd_system_status_ind_msg_v01));
  keep_alive = std::make_shared<KeepAliveHandler>();
#ifndef RIL_FOR_LOW_RAM
  voiceCallEndPointSub0 = nullptr;
  voiceCallEndPointSub1 = nullptr;
#endif

loadDatactl();

  networkavailability_handler = nullptr;
  tmd_manager = std::make_unique<TmdManager>();
  using std::placeholders::_1;

  mMessageHandler = {
#ifndef RIL_FOR_LOW_RAM
                      HANDLER(DataBearerTypeChangedMessage, DataModule::handleDataBearerTypeUpdate),
                      HANDLER(DataAllBearerTypeChangedMessage, DataModule::handleDataAllBearerTypeUpdate),
                      HANDLER(RegisterBearerAllocationUpdateRequestMessage, DataModule::handleToggleBearerAllocationUpdate),
                      HANDLER(GetBearerAllocationRequestMessage, DataModule::handleGetBearerAllocation),
                      HANDLER(GetAllBearerAllocationsRequestMessage, DataModule::handleGetAllBearerAllocations),
                      HANDLER(PreferredApnUpdatedMessage, DataModule::handlePreferredApnUpdated),
                      HANDLER(NasPhysChanConfigReportingStatus, DataModule::handleNasPhysChanConfigReportingStatusMessage),
                      HANDLER(NasPhysChanConfigMessage, DataModule::handleNasPhysChanConfigMessage),
                      HANDLER(DdsSwitchRecommendInternalMessage, DataModule::handleDdsSwitchRecommendInternalMessage),
                      HANDLER(GetSmartTempDdsSwitchCapabilityMessage, DataModule::handleGetSmartTempDdsSwitchCapabilityMessage),
                      HANDLER(GetCIWlanCapabilityMessage, DataModule::handleGetCIWlanCapabilityMessage),
                      HANDLER(GetDataPPDataCapabilityMessage, DataModule::handleGetDataPPDataCapabilityMessage),
                      HANDLER(UpdateDataPPDataUIOptionMessage, DataModule::handleUpdateDataPPDataUIOptionMessage),
                      HANDLER(RegisterDataDuringVoiceMessage, DataModule::handleRegisterForDataDuringVoiceCall),
#endif //RIL_FOR_LOW_RAM
                      HANDLER(UimCardStatusIndMsg, DataModule::handleUimCardStatusIndMsg),
                      HANDLER(IWLANCapabilityHandshake, DataModule::handleIWLANCapabilityHandshake),
                      HANDLER(GetAllQualifiedNetworkRequestMessage, DataModule::handleGetAllQualifiedNetworksMessage),
                      HANDLER(IntentToChangeApnPreferredSystemMessage, DataModule::handleIntentToChangeInd),
                      HANDLER(GetIWlanDataCallListRequestMessage, DataModule::handleGetIWlanDataCallListRequestMessage),
                      HANDLER(GetIWlanDataRegistrationStateRequestMessage, DataModule::handleGetIWlanDataRegistrationStateRequestMessage),
                      HANDLER(HandoverInformationIndMessage, DataModule::handleHandoverInformationIndMessage),
                      HANDLER(InformHandoverFailureMessage, DataModule::handleHandoverFailureMessage),
                      HANDLER(DsdSystemStatusPerApnMessage, DataModule::handleDsdSystemStatusPerApn),
                      HANDLER(SetApnPreferredSystemResultMessage, DataModule::handleSetApnPreferredSystemResult),

                      HANDLER(KeepAliveIndMessage, DataModule::handleKeepAliveIndMessage),
                      HANDLER(StartKeepAliveRequestMessage, DataModule::handleStartKeepAliveRequestMessage),
                      HANDLER(StopKeepAliveRequestMessage, DataModule::handleStopKeepAliveRequestMessage),
                      HANDLER(QcrilInitMessage, DataModule::handleQcrilInitMessage),
                      HANDLER(GoDormantRequestMessage, DataModule::handleGoDormantMessage),
                      HANDLER(ProcessScreenStateChangeMessage, DataModule::handleProcessScreenStateChangeMessage),
                      HANDLER(ProcessStackSwitchMessage, DataModule::handleProcessStackSwitchMessage),
                      HANDLER(SetApnInfoMessage, DataModule::handleSetApnInfoMessage),
                      HANDLER(SetIsDataEnabledMessage, DataModule::handleSetIsDataEnabledMessage),
                      HANDLER(SetIsDataRoamingEnabledMessage, DataModule::handleSetIsDataRoamingEnabledMessage),
                      HANDLER(SetQualityMeasurementMessage, DataModule::handleSetQualityMeasurementMessage),
                      HANDLER(ToggleDormancyIndMessage, DataModule::handleToggleDormancyIndMessage),
                      HANDLER(ToggleLimitedSysIndMessage, DataModule::handleToggleLimitedSysIndMessage),
                      HANDLER(UpdateMtuMessage, DataModule::handleUpdateMtuMessage),
                      HANDLER(GetDdsSubIdMessage, DataModule::handleGetDdsSubIdMessage),
                      HANDLER(RequestDdsSwitchMessage, DataModule::handleDataRequestDDSSwitchMessage),
                      HANDLER_MULTI(QmiIndMessage, AUTH_QMI_IND, DataModule::handleQmiAuthServiceIndMessage),
                      HANDLER_MULTI(EndpointStatusIndMessage, AUTH_ENDPOINT_STATUS_IND, DataModule::handleQmiAuthEndpointStatusIndMessage),
                      HANDLER(SetLinkCapFilterMessage, DataModule::handleSetLinkCapFilterMessage),
                      HANDLER(SetLinkCapRptCriteriaMessage, DataModule::handleSetLinkCapRptCriteriaMessage),
                      HANDLER(SetDataProfileRequestMessage, DataModule::handleSetDataProfileRequestMessage),
                      HANDLER(SetInitialAttachApnRequestMessage, DataModule::handleSetInitialAttachApn),
                      HANDLER(ProcessInitialAttachRequestMessage, DataModule::handleProcessInitialAttachRequestMessage),
                      HANDLER(IATimeoutMessage, DataModule::handleIATimeoutMessage),
                      HANDLER(GetModemAttachParamsRetryMessage, DataModule::handleGetModemAttachParamsRetryMessage),
                      HANDLER(SetLteAttachPdnListActionResultMessage, DataModule::handleSetLteAttachPdnListActionResult),
                      HANDLER(NasRequestDataShutdownMessage, DataModule::handleNasRequestDataShutdown),
                      HANDLER_MULTI(EndpointStatusIndMessage, WDSModemEndPoint_ENDPOINT_STATUS_IND, DataModule::handleQmiWdsEndpointStatusIndMessage),
                      HANDLER(SetPreferredDataModemRequestMessage, DataModule::handleSetPreferredDataModem),
                      HANDLER(CurrentDDSIndMessage, DataModule::handleCurrentDDSIndMessage),
                      HANDLER(CurrentRoamingStatusChangedMessage, DataModule::handleCurrentRoamingStatusChangedMessage),
                      HANDLER(DDSSwitchResultIndMessage, DataModule::handleDDSSwitchResultIndMessage),
                      HANDLER(RadioConfigClientConnectedMessage, DataModule::handleRadioConfigClientConnectedMessage),
                      HANDLER(InformDDSSUBChangedMessage, DataModule::handleInformDDSSUBChangedMessage),
                      HANDLER(DDSSwitchTimeoutMessage, DataModule::handleDDSSwitchTimeoutMessage),
                      HANDLER(DDSSwitchIPCMessage, DataModule::handleDDSSwitchIPCMessage),
                      HANDLER(IAInfoIPCMessage, DataModule::handleIAInfoIPCMessage),
                      HANDLER(CallStatusMessage, DataModule::handleDataConnectionStateChangedMessage),
                      HANDLER_MULTI(EndpointStatusIndMessage, DSDModemEndPoint_ENDPOINT_STATUS_IND, DataModule::handleQmiDsdEndpointStatusIndMessage),
                      HANDLER(DsdSystemStatusMessage, DataModule::handleDsdSystemStatusInd),
                      HANDLER(RilEventDataCallback, DataModule::handleRilEventDataCallback),
                      HANDLER(SetupDataCallRequestMessage, DataModule::handleSetupDataCallRequestMessage_1_4),
#ifndef RIL_FOR_MDM_LE
                      HANDLER(SetupDataCallRequestMessage_1_6, DataModule::handleSetupDataCallRequestMessage_1_6),
#endif
                      HANDLER(DeactivateDataCallRequestMessage, DataModule::handleDeactivateDataCallRequestMessage),
                      HANDLER(GetRadioDataCallListRequestMessage, DataModule::handleGetRadioDataCallListRequestMessage),
                      HANDLER(DataCallTimerExpiredMessage, DataModule::handleDataCallTimerExpiredMessage),
                      HANDLER(UimCardAppStatusIndMsg, DataModule::handleUimCardAppStatusIndMsg),
                      #ifdef FEATURE_DATA_LQE
                      HANDLER_MULTI(EndpointStatusIndMessage, OTTModemEndPoint_ENDPOINT_STATUS_IND, DataModule::handleQmiOttEndpointStatusIndMessage),
                      #endif /* FEATURE_DATA_LQE */
                      HANDLER(LinkPropertiesChangedMessage, DataModule::handleLinkPropertiesChangedMessage),
                      HANDLER(SetCarrierInfoImsiEncryptionMessage, DataModule::handleSetCarrierInfoImsiEncryptionMessage),
                      HANDLER(RegistrationFailureReportingStatusMessage, DataModule::handleRegistrationFailureReportingStatusMessage),
                      HANDLER(StartLCERequestMessage, DataModule::handleStartLCERequestMessage),
                      HANDLER(StopLCERequestMessage, DataModule::handleStopLCERequestMessage),
                      HANDLER(PullLCEDataRequestMessage, DataModule::handlePullLCEDataRequestMessage),
                      HANDLER(PdcRefreshIndication, DataModule::handlePdcRefreshIndication),
                      HANDLER(WdsThrottleInfoIndMessage, DataModule::handleWdsThrottleInfoIndMessage),
                      HANDLER(GetPdnThrottleTimeResponseInd, DataModule::handleGetPdnThrottleTimeResponseInd),
                      HANDLER(QcRilUnsolCurrentPlmnChangedMessage, DataModule::handlePlmnChangedIndMessage),
                      HANDLER(DsiInitCompletedMessage, DataModule::handleDsiInitCompletedMessage),
                      HANDLER(QcrilDataClientDisconnectedMessage, DataModule::handleClientDisconnectedMessage),
                      HANDLER(QcrilDataClientConnectedMessage, DataModule::handleClientConnectedMessage),
                      HANDLER(GetSlicingConfigRequestMessage, DataModule::handleGetSlicingConfigRequestMessage),
                      HANDLER(ExitFromCurrentStateRequestMessage, DataModule::handleExitFromCurrentStateRequestMessage),
#ifdef RIL_FOR_MDM_LE
                      HANDLER(CaptureLogBufferMessage, DataModule::handleCaptureLogBufferMessage),
                      HANDLER(IndicationRegistrationFilterMessage, DataModule::handleIndicationRegistrationFilterMessage),
#else
                      HANDLER(SetDataThrottlingRequestMessage, DataModule::handleSetDataThrottlingRequestMessage),
                      HANDLER(QosInitializeMessage, DataModule::handleQosInitMessage),
                      HANDLER(SecureModeChangedCallBackMessage, DataModule::handleSecureModeChangedCallBackMessage),
                      HANDLER(RegisterForSecureModeCbMessage, DataModule::handleRegisterForSecureModeCbMessage),
                      HANDLER(SetDataInActivityPeriodMessage, DataModule::handleSetDataInActivityMessage),
                      HANDLER(TcpMonitoringRequestMessage, DataModule::handleTcpMonitoringReqMessage),
                      HANDLER(CleanUpTcpKeepAliveMessage, DataModule::handleCleanUpTcpKeepAliveMessage),
                      HANDLER(VoiceIndMessage, DataModule::handleVoiceIndMessage),
                      HANDLER(VoiceCallOrigTimeoutMessage, DataModule::handleVoiceCallOrigTimeoutMessage),
                      #ifdef QMI_RIL_UTF
                      HANDLER_MULTI(EndpointStatusIndMessage, VOICECALL_ENDPOINT_STATUS_IND ,DataModule::handleQmiVoiceEndpointStatusIndMessage),
                      #else
                      HANDLER_MULTI(EndpointStatusIndMessage, VOICE_ENDPOINT_STATUS_IND, DataModule::handleQmiVoiceEndpointStatusIndMessage),
                      #endif
#endif
                      HANDLER_MULTI(EndpointStatusIndMessage, TMDModemEndPoint_ENDPOINT_STATUS_IND, DataModule::handleQmiTmdEndpointStatusIndMessage),
                      HANDLER(NasRfBandInfoIndMessage, DataModule::handleNasRfBandInfoIndMessage),
                      HANDLER(NasRfBandInfoMessage, DataModule::handleNasRfBandInfoMessage),
                      HANDLER(PduSessionParamLookupResultIndMessage, DataModule::handlePduSessionParamLookupResultIndMessage),
                      HANDLER(RouteChangeUpdateMessage, DataModule::handleRouteChangeUpdateMessage),
                      HANDLER(UpdateUIOptionMessage, DataModule::handleUpdateUIOptionMessage),
                    };

  //Read system property to determine IWLAN mode
  //TODO:: change default value to "1" or legacy mode before initial checkin
  //TODO:: change default value to "0" or default mode when we have code in place
  //to determine Radio hal version
  string propValue = readProperty("ro.telephony.iwlan_operation_mode", "AP-assisted");
  Log::getInstance().d("[" + mName + "]: IWLAN mode system property is " + propValue);

  if(propValue.compare("legacy") == 0) {
    propValue = "1";
  }
  else {
    propValue = "2"; // Ap-assisted
  }
#ifdef QMI_RIL_UTF
  mInitTracker.setIWLANMode(rildata::IWLANOperationMode::AP_ASSISTED);
#else
  mInitTracker.setIWLANMode((rildata::IWLANOperationMode)atoi(propValue.c_str()));
#endif
  CallManager::iWlanRegistered = false;
}

DataModule::~DataModule() {
#ifndef RIL_FOR_LOW_RAM
#ifndef RIL_FOR_MDM_LE
    deregisterForSecureModeIndications();
    unloadDataQos();
#endif
#endif
  unloadDatactl();
  CallManager::iWlanRegistered = false;
  mLooper = nullptr;
  //mDsdEndPoint = nullptr;
}

PendingMessageList& DataModule::getPendingMessageList() {
  return mMessageList;
}

void DataModule::init() {
  /* Call base init before doing any other stuff.*/
  Module::init();
}

void DataModule::broadcastReady()
{
    std::shared_ptr<DataInitMessage> data_init_msg =
                       std::make_shared<DataInitMessage>(global_instance_id);
    Dispatcher::getInstance().broadcast(data_init_msg);
}

void DataModule::dump(int fd)
{
  std::ostringstream ss;
  if (call_manager != nullptr) {
    call_manager->dump("", ss);
  }
  ss << "NetworkAvailabilityHandler:" << endl;
#ifndef RIL_FOR_LOW_RAM
  if (networkavailability_handler != nullptr) {
    networkavailability_handler->dump("    ", ss);
  }
  ss << endl;
#endif
  ss << "NetworkServiceHandler:" << endl;
  if (network_service_handler != nullptr) {
    network_service_handler->dump("    ", ss);
  }
  ss << endl;
  ss << "InitTracker:" << endl;
  mInitTracker.dump("    ", ss);
  ss << "    IWlanHandshakeMsgToken=" << iwlanHandshakeMsgToken << endl;
  ss << endl;
  ss << "ProfileHandler:" << endl;
  if (profile_handler != nullptr) {
    profile_handler->dump("    ", ss);
  }
  ss << std::boolalpha << "    profileUpdateProperty=" << profileUpdatePropertySet;
  ss << ", profileUpdateCarrier=" << profileUpdateCarrierSet << endl;

  ss << endl << "Logs:" << endl;
  vector<string> logs = networkAvailabilityLogBuffer.getLogs();
  ss << "NetworkAvailability:" << endl;
  for (const string& msg: logs) {
    ss << msg << endl;
  }
  ss << endl << "DataModule:" << endl;
  logs = logBuffer.getLogs();
  for (const string& msg: logs) {
    ss << msg << endl;
  }
  ss << endl << "QDP:" << endl;
  logs = Manager::getInstance().dump();
  for (const string& msg: logs) {
    ss << msg << endl;
  }
  LocalLogBuffer::toFd(ss.str(), fd);
}

void DataModule::flush() {
  logBuffer.flush();
  networkAvailabilityLogBuffer.flush();
}

void DataModule::handleClientConnectedMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<QcrilDataClientConnectedMessage> m = std::static_pointer_cast<QcrilDataClientConnectedMessage>(msg);
  if( m != NULL ) {
    if( mInitCompleted && call_manager) {
      call_manager->dataCallListChanged();
    }
  }
}

void DataModule::handleClientDisconnectedMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<QcrilDataClientDisconnectedMessage> m = std::static_pointer_cast<QcrilDataClientDisconnectedMessage>(msg);
  if( m != NULL ) {
    if( mInitCompleted && call_manager) {
      Log::getInstance().d("[DataModule]: Clean All Calls ");
      call_manager->cleanAllCalls();
    }
  }
}

void DataModule::handleGetSlicingConfigRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DataModule]:Handling msg = " + msg->dump());
  std::shared_ptr<GetSlicingConfigRequestMessage> m = std::static_pointer_cast<GetSlicingConfigRequestMessage>(msg);
  if( m != NULL && wds_endpoint != NULL) {
     wds_endpoint->getSlicingConfigRequest(m);
  }
}

void DataModule::handleExitFromCurrentStateRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DataModule]:Handling msg = " + msg->dump());
  if(msg != nullptr && preferred_data_state_info->isRilIpcNotifier) {
    PreferredDataEventType exitState;
    exitState.event = ExitState;
    preferred_data_sm->processEvent(exitState);
  }
}

void setDefaultQmiPort(std::string &default_qmi_port) {
  ds_target_t target;
  const char *target_str;
  target = ds_get_target();
  target_str = ds_get_target_str(target);
  do
  {
    if (DS_TARGET_MSM == target ||
        #ifdef RIL_FOR_MDM_LE
        DS_TARGET_LE_PRAIRIE == target ||
        #endif
        DS_TARGET_MSM8994 == target ||
        DS_TARGET_MSM8996 == target)
    {
      default_qmi_port = QMI_PORT_RMNET_0;
    }
    else if (DS_TARGET_SVLTE1 == target ||
             DS_TARGET_SVLTE2 == target ||
             DS_TARGET_CSFB == target)
    {
      default_qmi_port = QMI_PORT_RMNET_SDIO_0;
    }
    else if (DS_TARGET_MDM == target)
    {
      default_qmi_port = QMI_PORT_RMNET_USB_0;
    }
    else if (DS_TARGET_FUSION4_5_PCIE == target ||
             DS_TARGET_FUSION_8084_9X45 == target )
    {
      default_qmi_port = QMI_PORT_RMNET_MHI_0;
    }
    else if (DS_TARGET_DSDA == target || DS_TARGET_SGLTE2 == target)
    {
      if (global_instance_id == QCRIL_DEFAULT_INSTANCE_ID)
      {
         default_qmi_port = QMI_PORT_RMNET_USB_0;
      }
      else
      {
         default_qmi_port = QMI_PORT_RMNET_SMUX_0;
      }
    }
    else if (DS_TARGET_DSDA2 == target)
    {
      if (global_instance_id == QCRIL_DEFAULT_INSTANCE_ID)
      {
         default_qmi_port = QMI_PORT_RMNET_USB_0;
      }
      else
      {
         default_qmi_port = QMI_PORT_RMNET2_USB_0;
      }
    }
  } while (0);
  Log::getInstance().d("default_qmi_port set to : " + default_qmi_port);
}

#ifndef RIL_FOR_MDM_LE
static void qcrilDataConfigureXlat(const char* status)
{
  int rval = -1;
  rval = property_set("persist.vendor.net.doxlat", status);
  if (rval != 0)
  {
    Log::getInstance().d("Could not set persist.vendor.net.doxlat to " + std::string(status));
  }
  else
  {
    Log::getInstance().d("persist.vendor.net.doxlat successfully set to " + std::string(status));
  }
}
#endif

void DataModule::qcrilDataInit()
{
  Log::getInstance().d("qcrilDataInit:: Entry");
  std::string default_qmi_port = QMI_PORT_RMNET_0;
  std::string propValue = "";
  DsiWrapper::getInstance().dsiQcrilDataInit();
  Log::getInstance().d( "qcrilDataInit: subs_id:" + std::to_string(global_subs_id));
#ifdef FEATURE_QCRIL_FUSION
  default_qmi_port = QMI_PORT_RMNET_SDIO_0;
#else
  setDefaultQmiPort(default_qmi_port);
#endif
  Log::getInstance().d("qcrilDataInit:: default_qmi_port : " + default_qmi_port);
#ifndef RIL_FOR_MDM_LE
  if (global_qcril_clat_supported) {
    qcrilDataConfigureXlat(QCRIL_FALSE);
  } else {
    qcrilDataConfigureXlat(QCRIL_TRUE);
  }
#endif
  Log::getInstance().d("qcrilDataInit:: Exit");
}

void DataModule::handleQcrilInitMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
#ifdef QMI_RIL_UTF
  usleep(500000);
#endif
  auto qcril_init_msg = std::static_pointer_cast<QcrilInitMessage>(msg);
  if(qcril_init_msg)
  {
    #ifdef RIL_FOR_MDM_LE
    propertySetMap("persist.vendor.radio.disable_retry",  PERSIST_VENDOR_RADIO_DISABLE_RETRY);
    propertySetMap("persist.vendor.data.profile_update", PERSIST_VENDOR_DATA_PROFILE_UPDATE);
    propertySetMap("persist.vendor.radio.disable_nricon_report",  PERSIST_VENDOR_RADIO_DISABLE_NRICON_REPORT);
    #endif
    string propValue = readProperty("persist.vendor.radio.disable_retry", PROPERTY_DISABLE_PARTIAL_RETRY_DEFAULT);
    Log::getInstance().d("[" + mName + "]: Partial retry disabled property is " + propValue);
    std::transform( propValue.begin(), propValue.end(), propValue.begin(),
                  [](char c) -> char { return std::tolower(c); });
    if ("true" == propValue) {
        mInitTracker.setPartialRetryEnabled(false);
    }
    global_instance_id = qcril_init_msg->get_instance_id();
    logBuffer.setName("RIL" + std::to_string(global_instance_id));
    propValue = readProperty("persist.vendor.data.profile_update", "false");
    std::transform( propValue.begin(), propValue.end(), propValue.begin(),
                [](char c) -> char { return std::tolower(c); });
    profileUpdatePropertySet = ("true" == propValue);
    if (profileUpdatePropertySet) {
      util.setProfileUpdateAllowed(true);
    }
    propValue = readProperty("persist.vendor.radio.adb_log_on", "0");
    logToAdb = ("1" == propValue);

    qdpSubId = convertInstanceIdToSubId((qcril_instance_id_e_type)global_instance_id);
    Manager::getInstance().init(qdpSubId, logToAdb);

    propValue = readProperty("persist.vendor.radio.disable_nricon_report", "false");
    Log::getInstance().d("[" + mName + "]: NrIcon report disabled property is " + propValue);
    if ("true" == propValue) {
      dsd_endpoint->setNrIconReportDisable(true);
    }

    Messenger::get().registerForMessage(DDSSwitchIPCMessage::get_class_message_id(),
                                        std::bind(&DataModule::constructDDSSwitchIPCMessage,
                                        this, std::placeholders::_1));
    Messenger::get().registerForMessage(IAInfoIPCMessage::get_class_message_id(),
                                        std::bind(&DataModule::constructIAInfoIPCMessage,
                                        this, std::placeholders::_1));
    QmiSetupRequestCallback callback("data-token");
    qcril_instance_id_e_type inst_id = static_cast<qcril_instance_id_e_type>(global_instance_id);
    dsd_endpoint->requestSetup("data-token-client-server", inst_id, &callback);

    wds_endpoint->requestSetup("data-token-client-server", inst_id, &callback);

    auth_endpoint->requestSetup("data-token-client-server", inst_id, &callback);

    #ifdef FEATURE_DATA_LQE
      ott_endpoint->requestSetup("data-token-client-server", inst_id, &callback);
    #endif
    pdc_endpoint->requestSetup("data-token-client-server", inst_id, &callback);

    tmd_endpoint->requestSetup("data-token-client-server", inst_id, &callback);

#ifndef RIL_FOR_MDM_LE
    if(datactlInt != nullptr)
    {
      datactlInt->initialize((SubsId)global_instance_id, logFn);
    }
    dataCtlInstanceId = global_instance_id;
#endif
    broadcastReady();
    Log::getInstance().d("[" + mName + "]: Done msg = " + msg->dump());
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received. =" + msg->dump() + "QCRIL DATA Init not triggered!!!");
  }
}

#ifdef RIL_FOR_MDM_LE
void DataModule::handleCaptureLogBufferMessage(std::shared_ptr<Message> msg) {
    if (msg != nullptr) {
        auto m = std::static_pointer_cast<CaptureLogBufferMessage>(msg);
        if (m == nullptr) {
            return;
        }
        RIL_Errno res = handleRilDataDump("CLI_USER");
        auto resp = make_shared<RIL_Errno>(res);
        m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
    }
}
void DataModule::handleIndicationRegistrationFilterMessage(std::shared_ptr<Message> msg) {
    if (msg != nullptr) {
        auto m = std::static_pointer_cast<IndicationRegistrationFilterMessage>(msg);
        if (m == nullptr) {
            return;
        }
        Log::getInstance().d("[" + mName + "]: Processing msg = " + m->dump());
        bool result = true;
        if (dsd_endpoint) {
            bool uiFilterEnabled = (m->getFilterValue() & UI_INFO_REGISTRATION_FLAG) != 0;
            bool check = dsd_endpoint->uiChangeInfoRegistrationRequest(uiFilterEnabled);
            if (check) {
                mUiFilterEnabled = uiFilterEnabled;
            }
            result = result & check;
        }
        else {
            Log::getInstance().d("Dsd Endpoint is not Initialized");
            result = false;
        }
        int res = -1;
        if (result == true) {
            res = 0;
        }
        auto resp = make_shared<int>(res);
        m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
    }
    else {
        Log::getInstance().d("Msg is Nullptr");
    }
}
#endif

void DataModule::handleProcessStackSwitchMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<ProcessStackSwitchMessage> m = std::static_pointer_cast<ProcessStackSwitchMessage>(msg);
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if( m != NULL ) {
    RetVal ret = QCRIL_DS_ERROR;
    ProcessStackSwitchMessage::StackSwitchReq info = m->getParams();
    bool stackSwitchSuccess = true;
    auto msg = std::make_shared<ChangeStackIdDsdSyncMessage>(nullptr);
    auto r = std::make_shared<int>();
    if (msg) {
        Message::Callback::Status apiStatus = Message::Callback::Status::FAILURE;
        msg->setParams(info.newStackId, info.instanceId);
        apiStatus = msg->dispatchSync(r);
        if (apiStatus == Message::Callback::Status::SUCCESS) {
            Log::getInstance().d("ChangeStackId for DSD is Successful");
        }
        else {
            Log::getInstance().d("ChangeStackId for DSD Failed");
            stackSwitchSuccess = false;
        }
    }
    auto msg1 = std::make_shared<ChangeStackIdWdsSyncMessage>(nullptr);
    auto r1 = std::make_shared<int>();
    if (msg1) {
        Message::Callback::Status apiStatus = Message::Callback::Status::FAILURE;
        msg1->setParams(info.newStackId, info.instanceId);
        apiStatus = msg1->dispatchSync(r1);
        if (apiStatus == Message::Callback::Status::SUCCESS) {
            Log::getInstance().d("ChangeStackId for WDS is Successful");
        }
        else {
            Log::getInstance().d("ChangeStackId for WDS Failed");
            stackSwitchSuccess = false;
        }
    }
    if (stackSwitchSuccess) {
      if (dsi_set_modem_subs_id(static_cast<int>(global_subs_id)) == DSI_SUCCESS) {
        ret = QCRIL_DS_SUCCESS;
      }
    }
    int newQdpSubId = convertInstanceIdToSubId(info.instanceId);
    if (newQdpSubId != qdpSubId) {
      qdpSubId = newQdpSubId;
      Manager::getInstance().init(qdpSubId, logToAdb);
    }
    if (info.instanceId != dataCtlInstanceId) {
      dataCtlInstanceId = info.instanceId;
#ifndef RIL_FOR_MDM_LE
      if (mInitCompleted && datactlInt != nullptr) {
        datactlInt->initialize((SubsId)dataCtlInstanceId, logFn);
      }
#endif
    }
    auto resp = std::make_shared<int>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleUimCardStatusIndMsg(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: handling msg = " + msg->dump());
  std::shared_ptr<UimCardStatusIndMsg> m = std::static_pointer_cast<UimCardStatusIndMsg>(msg);
  if( m != NULL ) {
    if((m->get_card_status()).status == QCRIL_CARD_STATUS_UP || (m->get_card_status()).status == QCRIL_CARD_STATUS_SIM_READY)
    {
      Log::getInstance().d("[" + mName + "]: Sim is present in the device");
      mSimPresent = TRUE;
    }
    else if((m->get_card_status()).status == QCRIL_CARD_STATUS_DOWN || (m->get_card_status()).status == QCRIL_CARD_STATUS_ABSENT)
    {
      Log::getInstance().d("[" + mName + "]: Sim is removed");
      mSimPresent = FALSE;
    }
  } else {
      Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

#ifndef RIL_FOR_LOW_RAM
void DataModule::handleDdsSwitchRecommendInternalMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<DdsSwitchRecommendInternalMessage> m = std::static_pointer_cast<DdsSwitchRecommendInternalMessage>(msg);

  if (mIsTurningOffDataDuringVoiceCall) {
    Log::getInstance().d("Ignore DdsSwitchRecommendInternalMessage "+
                          std::to_string((int)mIsTurningOffDataDuringVoiceCall));
    return;
  }
  if (m != nullptr && preferred_data_state_info != nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + m->dump());
    PreferredDataEventType internalEvent;
    DDSSwitchInfo_t eventData;
    eventData.subId = (int)m->getSubId();
    eventData.switch_type = (dsd_dds_switch_type_enum_v01)m->getSwitchType();
    eventData.tempSwitchLevel = m->getLevel();
    internalEvent.event = DDSSwitchInternal;
    internalEvent.data = &eventData;
    preferred_data_sm->processEvent(internalEvent);
  } else {
    Log::getInstance().d("No preferred_data_sm created");
  }
}

void DataModule::handleGetSmartTempDdsSwitchCapabilityMessage(std::shared_ptr<Message> m)
{
  auto msg= std::static_pointer_cast<GetSmartTempDdsSwitchCapabilityMessage>(m);
  SmartDdsSwitchCapability_t cap = SmartDdsSwitchCapability_t::SMART_DDS_SWITCH_CAPABILITY_UNKNOWN;
  if (dsd_endpoint->getDdsSwitchRecCapEnabled().has_value()) {
    cap = dsd_endpoint->getDdsSwitchRecCapEnabled().value() ?
          SmartDdsSwitchCapability_t::SMART_DDS_SWITCH_CAPABILITY_SUPPORTED :
          SmartDdsSwitchCapability_t::SMART_DDS_SWITCH_CAPABILITY_NOT_SUPPORTED;
  }
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS, std::make_shared<SmartDdsSwitchCapability_t>(cap));
  Log::getInstance().d("[DataModule]: handleGetSmartTempDdsSwitchCapabilityMessage="+std::to_string((int)cap));
}

void DataModule::handleGetCIWlanCapabilityMessage(std::shared_ptr<Message> m)
{
  auto msg= std::static_pointer_cast<GetCIWlanCapabilityMessage>(m);
  CIWlanCapabilitySupport_t resp;
  resp.cap = dsd_endpoint->getDsdCIWlanCapability();
  resp.type = dsd_endpoint->getDsdCIWlanCapType();
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS, std::make_shared<CIWlanCapabilitySupport_t>(resp));
  Log::getInstance().d("[DataModule]: handleGetCIWlanCapabilityMessage cap="+std::to_string((int)resp.cap)
                       + "type="+std::to_string((int)resp.type));
}

void DataModule::handleGetDataPPDataCapabilityMessage(std::shared_ptr<Message> m)
{
  auto msg= std::static_pointer_cast<GetDataPPDataCapabilityMessage>(m);
  bool cap = false;
#ifndef RIL_FOR_MDM_LE
  if(datactlInt != nullptr)
  {
    cap = datactlInt->getDataPPDataCapability();
  }
#endif
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS, std::make_shared<bool>(cap));
  Log::getInstance().d("[DataModule]: handleGetDataPPDataCapabilityMessage cap="+std::to_string((int)cap));
}

void DataModule::handleUpdateDataPPDataUIOptionMessage(std::shared_ptr<Message> m)
{
  auto msg= std::static_pointer_cast<UpdateDataPPDataUIOptionMessage>(m);
  Log::getInstance().d("[DataModule]: handleUpdateDataPPDataUIOptionMessage=" +
                       std::to_string((int)msg->getDataPPDataUIOption()));

  bool res = false;
#ifndef RIL_FOR_MDM_LE
  if(datactlInt != nullptr)
  {
    res = datactlInt->updateDataPPDataUIOption(msg->getDataPPDataUIOption());
  }
#endif
  auto resp = std::make_shared<RIL_Errno>(res ? RIL_E_SUCCESS : RIL_E_GENERIC_FAILURE );
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
}

void DataModule::handleRegisterForDataDuringVoiceCall(std::shared_ptr<Message> m)
{
  auto msg= std::static_pointer_cast<RegisterDataDuringVoiceMessage>(m);
  Log::getInstance().d("[DataModule]: handleRegisterForDataDuringVoiceCall=" + std::to_string((int)msg->getUserSelection()));

  if (dsd_endpoint->getUserDdsSwitchSelection() &&
    msg->getUserSelection()==DdsSwitchUserSelection_t::DISABLE_DATA_DURING_VOICE) {
    mIsTurningOffDataDuringVoiceCall = true;
    preferred_data_state_info->tempDdsSwitchRecommendInfo.reset();
  }
  RIL_Errno res = dsd_endpoint->registerForDataDuringVoiceCall
                                (msg->getUserSelection() == DdsSwitchUserSelection_t::ENABLE_DATA_DURING_VOICE);
  auto resp = std::make_shared<RIL_Errno>(res);
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);

  if (res == RIL_E_SUCCESS) {
    AutoDdsSwitchControl_t ctrl = msg->getUserSelection() == DdsSwitchUserSelection_t::ENABLE_DATA_DURING_VOICE?
                              AutoDdsSwitchControl_t::AUTO_DDS_SWITCH_DISABLED:
                              AutoDdsSwitchControl_t::AUTO_DDS_SWITCH_ENABLED;
    auto ctrlMsg = std::make_shared<rildata::AutoDdsSwitchControlIndMessage>(ctrl);
    ctrlMsg->broadcast();
  }
  mIsTurningOffDataDuringVoiceCall = false;
}

/*===========================================================================

  FUNCTION:  handleDataAllBearerTypeUpdate

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when DataBearerTypeChangedMessage is received

    @return
*/
/*=========================================================================*/
void DataModule::handleDataAllBearerTypeUpdate(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<DataAllBearerTypeChangedMessage> m = std::static_pointer_cast<DataAllBearerTypeChangedMessage>(msg);
  if (m != NULL)
  {
    AllocatedBearer_t bearerInfo = m->getBearerInfo();
    if (call_manager) {
      call_manager->handleDataAllBearerTypeUpdate(bearerInfo);
    }
    else {
      Log::getInstance().d("call_manager is null");
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

/*===========================================================================

  FUNCTION:  handleDataBearerTypeUpdate

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when DataBearerTypeChangedMessage is received

    @return
*/
/*=========================================================================*/
void DataModule::handleDataBearerTypeUpdate(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<DataBearerTypeChangedMessage> m = std::static_pointer_cast<DataBearerTypeChangedMessage>(msg);
  if (m != NULL)
  {
    int32_t cid = m->getCid();
    BearerInfo_t bearer = m->getBearerInfo();
    if (call_manager) {
      call_manager->handleDataBearerTypeUpdate(cid, bearer);
    }
    else {
      Log::getInstance().d("call_manager is null");
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

/*===========================================================================

  FUNCTION:  handleToggleBearerAllocationUpdate

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when RegisterBearerAllocationUpdateRequestMessage is received

    @return
*/
/*=========================================================================*/
void DataModule::handleToggleBearerAllocationUpdate(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());

  std::shared_ptr<RegisterBearerAllocationUpdateRequestMessage> m =
    std::static_pointer_cast<RegisterBearerAllocationUpdateRequestMessage>(msg);
  if (m != NULL)
  {
    bool enable = m->getToggleSwitch();
    ResponseError_t ret;
    if (call_manager) {
      ret = call_manager->handleToggleBearerAllocationUpdate(enable);
    }
    else {
      Log::getInstance().d("call_manager is null");
      ret = ResponseError_t::INTERNAL_ERROR;
    }
    auto resp = std::make_shared<ResponseError_t>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

/*===========================================================================

  FUNCTION:  handleGetBearerAllocation

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when GetBearerAllocationRequestMessage is received
    Invokes the callback with the allocated bearers that were retrieved

    @return
*/
/*=========================================================================*/
void DataModule::handleGetBearerAllocation(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());

  std::shared_ptr<GetBearerAllocationRequestMessage> m =
    std::static_pointer_cast<GetBearerAllocationRequestMessage>(msg);
  if (m != NULL)
  {
    int32_t cid = m->getCallId();
    Message::Callback::Status status = Message::Callback::Status::SUCCESS;
    AllocatedBearerResult_t bearerAllocations;
    if (call_manager) {
      bearerAllocations = call_manager->handleGetBearerAllocation(cid);
    }
    else {
      Log::getInstance().d("call_manager is null");
      bearerAllocations.error = ResponseError_t::INTERNAL_ERROR;
    }
    auto resp = std::make_shared<AllocatedBearerResult_t>(bearerAllocations);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}
/*===========================================================================

  FUNCTION:  handleGetAllBearerAllocations

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when GetAllBearerAllocationsRequestMessage is received
    Invokes the callback with the allocated bearers that were retrieved

    @return
*/
/*=========================================================================*/
void DataModule::handleGetAllBearerAllocations(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());

  auto m = std::static_pointer_cast<GetAllBearerAllocationsRequestMessage>(msg);
  if (m != NULL)
  {
    Message::Callback::Status status = Message::Callback::Status::SUCCESS;
    AllocatedBearerResult_t bearerAllocations;
    if (call_manager) {
      bearerAllocations = call_manager->handleGetAllBearerAllocations();
    }
    else {
      Log::getInstance().d("call_manager is null");
      bearerAllocations.error = ResponseError_t::INTERNAL_ERROR;
    }
    auto resp = std::make_shared<AllocatedBearerResult_t>(bearerAllocations);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handlePreferredApnUpdated(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  std::shared_ptr<PreferredApnUpdatedMessage> m = std::static_pointer_cast<PreferredApnUpdatedMessage>(msg);
  if (m != NULL && networkavailability_handler != NULL) {
    Log::getInstance().d("[DataModule]::Invoking processQmiDsdPreferredApnUpdated");
    networkavailability_handler->processQmiDsdPreferredApnUpdated(m);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleNasPhysChanConfigReportingStatusMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<NasPhysChanConfigReportingStatus> m = std::static_pointer_cast<NasPhysChanConfigReportingStatus>(msg);
  if (m != nullptr && call_manager != nullptr) {
    call_manager->enablePhysChanConfigReporting(m->isPhysChanConfigReportingEnabled());
  } else {
    Log::getInstance().d("No call_manager created");
  }
}

void DataModule::handleNasPhysChanConfigMessage(std::shared_ptr<Message> msg) {
  if (call_manager != nullptr) {
    call_manager->handleNasPhysChanConfigMessage(msg);
  } else {
    Log::getInstance().d("No call_manager created");
  }
}
#endif //RIL_FOR_LOW_RAM

void DataModule::processDeferredIntentToChange(DeferredIntentToChange_t deferIntent) {
  string apn(deferIntent.apnPrefSys.apn_pref_info.apn_name);
  Log::getInstance().d("[DataModule] processDeferredIntentToChange = " + apn);

  dsd_intent_to_change_apn_pref_sys_ind_msg_v01 intentToChangeApnPrefSysInd;
  intentToChangeApnPrefSysInd.apn_pref_sys_len = 1;
  memcpy(&intentToChangeApnPrefSysInd.apn_pref_sys[0], &deferIntent.apnPrefSys, sizeof(intentToChangeApnPrefSysInd.apn_pref_sys[0]));
  intentToChangeApnPrefSysInd.apn_status_valid = true;
  intentToChangeApnPrefSysInd.apn_status_len = 1;
  memcpy(&intentToChangeApnPrefSysInd.apn_status[0], &deferIntent.apnStatus, sizeof(intentToChangeApnPrefSysInd.apn_status[0]));

  if (getDataModule().networkavailability_handler) {
    getDataModule().networkavailability_handler->processQmiDsdIntentToChangeApnPrefSysInd(&intentToChangeApnPrefSysInd);
  }
  if (getDataModule().call_manager) {
    getDataModule().call_manager->processQmiDsdIntentToChangeApnPrefSysInd(intentToChangeApnPrefSysInd);
  }
}

/*===========================================================================

  FUNCTION:  handleIWLANCapabilityHandshake

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when IWLANCapabilityHandshake is received.

    @return
*/
/*=========================================================================*/
void DataModule::handleIWLANCapabilityHandshake(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<IWLANCapabilityHandshake> m = std::static_pointer_cast<IWLANCapabilityHandshake>(msg);
  if (m != NULL) {
    Log::getInstance().d("[ getPendingMessageList test ]: insert message = " +
                         msg->get_message_name());
    std::pair<uint16_t, bool> result = getDataModule().getPendingMessageList().insert(msg);
    iwlanHandshakeMsgToken = (int32_t)result.first;
    Log::getInstance().d("[ getPendingMessageList test ]: insert result token = " +
                         std::to_string((int)iwlanHandshakeMsgToken));
    getDataModule().getPendingMessageList().print();
    if (m->isIWLANEnabled()) {
      CallManager::iWlanRegistered = true;
      mInitTracker.setIWLANEnabled(true);
      if (mInitTracker.allServicesReady()) {
        initializeIWLAN();
      }
    } else {
      CallManager::iWlanRegistered = false;
      mInitTracker.setIWLANEnabled(false); //TODO: handled disabled
      deinitializeIWLAN();
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

/*===========================================================================

  FUNCTION:  handleGetAllQualifiedNetworksMessage

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when GetAllQualifiedNetworkRequestMessage is received.

    @return
*/
/*=========================================================================*/
void DataModule::handleGetAllQualifiedNetworksMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<GetAllQualifiedNetworkRequestMessage> m = std::static_pointer_cast<GetAllQualifiedNetworkRequestMessage>(msg);
  if (m != NULL) {
    Message::Callback::Status status = Message::Callback::Status::SUCCESS;
    QualifiedNetworkResult_t result;

    if (!mInitTracker.isAPAssistMode()) {
      result.respErr = ResponseError_t::NOT_SUPPORTED;
    } else if (!mInitTracker.isIWLANEnabled() || !mInitTracker.allServicesReady()) {
      result.respErr = ResponseError_t::NOT_AVAILABLE;
    } else {
      if (networkavailability_handler) {
        result.respErr = ResponseError_t::NO_ERROR;
        networkavailability_handler->getQualifiedNetworks(result.qualifiedNetwork);
      }
      else {
        Log::getInstance().d("networkavailability_handler is null");
        result.respErr = ResponseError_t::INTERNAL_ERROR;
      }
    }
    auto resp = std::make_shared<QualifiedNetworkResult_t>(result);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

/*===========================================================================

  FUNCTION:  handleIntentToChangeInd

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when IntentToChangeApnPreferredSystemMessage is received.

    @return
*/
/*=========================================================================*/
void DataModule::handleIntentToChangeInd(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<IntentToChangeApnPreferredSystemMessage> m =
     std::static_pointer_cast<IntentToChangeApnPreferredSystemMessage>(msg);
  if (m != NULL) {
    dsd_intent_to_change_apn_pref_sys_ind_msg_v01 intent = m->getParams();

    dsd_intent_to_change_apn_pref_sys_ind_msg_v01 intentToChangeApnPrefSysInd;
    memset(&intentToChangeApnPrefSysInd, 0, sizeof(intentToChangeApnPrefSysInd));

    for(uint32_t i=0 ; i<intent.apn_pref_sys_len ; i++) {
      string apnName(intent.apn_pref_sys[i].apn_pref_info.apn_name);
      if(intent.apn_status_valid && call_manager!=nullptr &&
        (intent.apn_status[i] & (DSD_APN_PREF_SYS_APN_STATUS_WWAN_CONNECTED_V01 |
                                 DSD_APN_PREF_SYS_APN_STATUS_IWLAN_CONNECTED_V01))) {
        if(call_manager->shouldDeferIntentToChange(apnName)) {
          DeferredIntentToChange_t deferIntent;
          memset(&deferIntent, 0, sizeof(deferIntent));
          memcpy(&deferIntent.apnPrefSys, &intent.apn_pref_sys[i], sizeof(deferIntent.apnPrefSys));
          memcpy(&deferIntent.apnStatus, &intent.apn_status[i], sizeof(deferIntent.apnStatus));
          call_manager->deferToProcessIntentToChange(deferIntent);
        }
        else {
          memcpy(&intentToChangeApnPrefSysInd.apn_pref_sys[intentToChangeApnPrefSysInd.apn_pref_sys_len],
                 &intent.apn_pref_sys[i],
                 sizeof(dsd_apn_pref_sys_type_ex_v01));
          intentToChangeApnPrefSysInd.apn_pref_sys_len++;
          memcpy(&intentToChangeApnPrefSysInd.apn_status[intentToChangeApnPrefSysInd.apn_status_len],
                &intent.apn_pref_sys[i],
                sizeof(dsd_apn_pref_sys_apn_status_mask_v01));
          intentToChangeApnPrefSysInd.apn_status_len++;
        }
      }
      else {
        memcpy(&intentToChangeApnPrefSysInd.apn_pref_sys[intentToChangeApnPrefSysInd.apn_pref_sys_len],
                &intent.apn_pref_sys[i],
                sizeof(dsd_apn_pref_sys_type_ex_v01));
        intentToChangeApnPrefSysInd.apn_pref_sys_len++;
        if(intent.apn_status_valid) {
          memcpy(&intentToChangeApnPrefSysInd.apn_status[intentToChangeApnPrefSysInd.apn_status_len],
                  &intent.apn_pref_sys[i],
                  sizeof(dsd_apn_pref_sys_apn_status_mask_v01));
          intentToChangeApnPrefSysInd.apn_status_len++;
          intentToChangeApnPrefSysInd.apn_status_valid = true;
        }
      }
    }

    if (networkavailability_handler && intentToChangeApnPrefSysInd.apn_pref_sys_len > 0) {
      networkavailability_handler->processQmiDsdIntentToChangeApnPrefSysInd(&intentToChangeApnPrefSysInd);
    }
    if (call_manager && intentToChangeApnPrefSysInd.apn_pref_sys_len > 0) {
      call_manager->processQmiDsdIntentToChangeApnPrefSysInd(intentToChangeApnPrefSysInd);
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}
void DataModule::handleGetIWlanDataCallListRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  std::shared_ptr<GetIWlanDataCallListRequestMessage> m = std::static_pointer_cast<GetIWlanDataCallListRequestMessage>(msg);
  if (m != NULL) {
    Message::Callback::Status status = Message::Callback::Status::SUCCESS;
    DataCallListResult_t result = {};
    result.respErr = ResponseError_t::NO_ERROR;

    if (call_manager) {
      call_manager->getIWlanDataCallList(result.call);
    } else {
      Log::getInstance().d("call_manager is null");
      result.respErr = ResponseError_t::INTERNAL_ERROR;
    }
    auto resp = std::make_shared<DataCallListResult_t>(result);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleGetIWlanDataRegistrationStateRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<GetIWlanDataRegistrationStateRequestMessage> m = std::static_pointer_cast<GetIWlanDataRegistrationStateRequestMessage>(msg);

  if (m != NULL) {
    Message::Callback::Status status = Message::Callback::Status::SUCCESS;
    IWlanDataRegistrationStateResult_t result;
    result.respErr = ResponseError_t::NO_ERROR;
    result.regState = DataRegState_t::NOT_REG_AND_NOT_SEARCHING;
    result.reasonForDenial = 0;
    if (network_service_handler && mInitTracker.isAPAssistMode()) {
      result.regState = static_cast<ApAssistNetworkServiceHandler *>(network_service_handler.get())->getIWlanDataRegistrationState();
    }
    auto resp = std::make_shared<IWlanDataRegistrationStateResult_t>(result);
    m->sendResponse(msg, status, resp);
  }
}

void DataModule::handleHandoverInformationIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  std::shared_ptr<HandoverInformationIndMessage> m = std::static_pointer_cast<HandoverInformationIndMessage>(msg);
  if (call_manager != NULL) {
     call_manager->handleCallEventMessage(CallEventTypeEnum::HandoverInformationInd, msg);
  } else {
    Log::getInstance().d("No call_manager created");
  }
}

void DataModule::handleHandoverFailureMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  std::shared_ptr<InformHandoverFailureMessage> m = std::static_pointer_cast<InformHandoverFailureMessage>(msg);
  if(networkavailability_handler != NULL) {
    networkavailability_handler->processHandoverfailureForApnType(m->getApnType(), m->getApn());
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

bool DataModule::loadDatactl() {
  if(isDataCtlLoaded)
    return true;

  bool ret = false;

  do {
    datactlLib = dlopen("libqcrildatactl.so", RTLD_LAZY);
    if( NULL == datactlLib ) {
      Log::getInstance().d("[" + mName + "]: Unable to load libqcrildatactl.so");
      break;
    }
    dlerror();
    mInitDataCtl = (initDataCtlFnPtr)dlsym(datactlLib, "datactlInit");
    const char *dlsym_error = dlerror();
    if( dlsym_error ) {
      Log::getInstance().d("[" + mName + "]: Cannot find datactlInit symbol");
      break;
    }

    //Successfully reached the end
    ret = true;
#ifndef RIL_FOR_MDM_LE
    datactlInt = (datactl::DataCtlInterface *)(mInitDataCtl());
#endif
  }
  while( 0 );
  if( ret == false ) {
    if( datactlLib ) {
      dlclose(datactlLib);
    }
  }
  isDataCtlLoaded = ret;
  return ret;
}

void DataModule::unloadDatactl()
{
  mInitDataCtl = nullptr;
  if( datactlLib ) {
    dlclose(datactlLib);
  }
  datactlLib = nullptr;
  isDataCtlLoaded = false;
}

void DataModule::handleDsdSystemStatusPerApn(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<DsdSystemStatusPerApnMessage> m = std::static_pointer_cast<DsdSystemStatusPerApnMessage>(msg);
  if (m != NULL)
  {
    auto apnAvailSys = m->getAvailSys();
    if ( networkavailability_handler ) {
      networkavailability_handler->processQmiDsdSystemStatusInd(apnAvailSys.data(), apnAvailSys.size());
    }
    if ( call_manager ) {
      call_manager->processQmiDsdSystemStatusInd(apnAvailSys.data(), apnAvailSys.size());
    }
  }
  else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetApnPreferredSystemResult(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  std::shared_ptr<SetApnPreferredSystemResultMessage> m = std::static_pointer_cast<SetApnPreferredSystemResultMessage>(msg);
  if (m != NULL && call_manager != NULL) {
    Log::getInstance().d("[DataModule]::Invoking processQmiDsdApnPreferredSystemResultInd");
    call_manager->processQmiDsdApnPreferredSystemResultInd(&(m->getParams()));
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

//Perform intializations that are common to services being initialized, mode
//being determined and IWLAN being enabled
void DataModule::initializeIWLAN() {

    if(iwlanHandshakeMsgToken != INVALID_MSG_TOKEN) {
      Log::getInstance().d("[DataModule] Look for IWLANCapabilityHandshake msg "+
                            std::to_string((int)iwlanHandshakeMsgToken));
      getDataModule().getPendingMessageList().print();
      std::shared_ptr<Message> mmm = getDataModule().getPendingMessageList().find((uint16_t)iwlanHandshakeMsgToken);
      if(mmm!=nullptr) {
        std::shared_ptr<IWLANCapabilityHandshake> iwlanHsMsg = std::static_pointer_cast<IWLANCapabilityHandshake>(mmm);
        if(iwlanHsMsg != nullptr)
        {
          auto resp = std::make_shared<rildata::ModemIWLANCapability_t>(
                                              mInitTracker.getModemCapability() ?
                                              rildata::ModemIWLANCapability_t::present:
                                              rildata::ModemIWLANCapability_t::not_present);
          iwlanHsMsg->sendResponse(mmm, Message::Callback::Status::SUCCESS, resp);
          getDataModule().getPendingMessageList().erase(mmm);
          getDataModule().getPendingMessageList().print();
          iwlanHandshakeMsgToken = INVALID_MSG_TOKEN;
        } else {
          Log::getInstance().d("[DataModule] Invalid IWLANCapabilityHandShake msg. Not sending response");
        }
      }
      else {
        Log::getInstance().d("[DataModule] No IWLANCapabilityHandshake msg found");
        iwlanHandshakeMsgToken = INVALID_MSG_TOKEN;
      }
    }
    else {
      Log::getInstance().d("[DataModule] Invalid IWLANCapabilityHandshake msg token");
    }

    Message::Callback::Status status = dsd_endpoint->registerForAPAsstIWlanInd(true);
    Log::getInstance().d("[DataModule] registerForIndications requst"
                          " result = "+ std::to_string((int) status));

    #ifndef QMI_RIL_UTF
    if(networkavailability_handler == nullptr)
    #endif
      networkavailability_handler = std::make_unique<NetworkAvailabilityHandler>(networkAvailabilityLogBuffer);

    if (mCachedSystemStatus.apn_avail_sys_info_valid) {
      networkavailability_handler->processQmiDsdSystemStatusInd(mCachedSystemStatus.apn_avail_sys_info,
                                                            mCachedSystemStatus.apn_avail_sys_info_len);
    }

#ifndef QMI_RIL_UTF
#ifndef RIL_FOR_MDM_LE
    //send message to datactl to enable IWLAN
    if(datactlInt != nullptr)
    {
      datactlInt->controlIWlan(true);
    }
#endif
#endif
}

void DataModule::deinitializeIWLAN() {

    Message::Callback::Status status = dsd_endpoint->registerForAPAsstIWlanInd(false);
    if (status != Message::Callback::Status::SUCCESS)
    {
       Log::getInstance().d("[DataModule] deregisterForIndications requst failed,"
                          " result = "+ std::to_string((int) status));
    }
    else
    {
       Log::getInstance().d("[DataModule] deregisterForIndications request successful"
                                     ", result = "+ std::to_string((int) status));
    }

    networkavailability_handler.reset();
    networkavailability_handler = nullptr;

    //Send message on datactl to disable IWLAN
#ifndef RIL_FOR_MDM_LE
    if(datactlInt != nullptr)
    {
      datactlInt->controlIWlan(false);
    }
#endif
}

void DataModule::handleStartLCERequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<StartLCERequestMessage> m = std::static_pointer_cast<StartLCERequestMessage>(msg);
  if( m != NULL ) {
    RIL_LceStatusInfo statusInfo;
    memset(&statusInfo,0,sizeof(RIL_LceStatusInfo));
    statusInfo.lce_status = static_cast<char>(LCE_NOT_SUPPORTED);
    auto resp = std::make_shared<RIL_LceStatusInfo>(statusInfo);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleStopLCERequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<StopLCERequestMessage> m = std::static_pointer_cast<StopLCERequestMessage>(msg);
  if( m != NULL ) {
    RIL_LceStatusInfo statusInfo;
    memset(&statusInfo,0,sizeof(RIL_LceStatusInfo));
    statusInfo.lce_status = static_cast<char>(LCE_NOT_SUPPORTED);
    auto resp = std::make_shared<RIL_LceStatusInfo>(statusInfo);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handlePullLCEDataRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<PullLCEDataRequestMessage> m = std::static_pointer_cast<PullLCEDataRequestMessage>(msg);
  if( m != NULL ) {
    RIL_LceDataInfo dataInfo;
    memset(&dataInfo,0,sizeof(RIL_LceDataInfo));
    auto resp = std::make_shared<RIL_LceDataInfo>(dataInfo);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleKeepAliveIndMessage(std::shared_ptr<Message> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    std::shared_ptr<KeepAliveIndMessage> m = std::static_pointer_cast<KeepAliveIndMessage>(msg);
    if( m!= NULL  && keep_alive) {
      KeepAliveEventType eve;
      eve.event = KeepAliveInd;
      keep_alive_ind ind;
      ind.status = m->get_status();
      ind.handle = m->get_handle();
      ind.tcpKaInfo = m->getTcpIndInfo();
      eve.data = &ind;
      keep_alive->processEvent(eve);
    } else {
      Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
    }
}

void DataModule::cleanupKeepAlive(int cid_val) {
  Log::getInstance().d("[" + mName + "]: CleanupKeepAlive");
  if(keep_alive) {
    KeepAliveEventType eve;
    eve.event = KeepAliveCleanCallState;
    int cid = cid_val;
    eve.data = &cid;
    keep_alive->processEvent(eve);
  } else {
    Log::getInstance().d("keep_alive is nullptr ");
  }
}

void DataModule::handleStartKeepAliveRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<StartKeepAliveRequestMessage> m = std::static_pointer_cast<StartKeepAliveRequestMessage>(msg);
  if( m!= NULL ) {
    KeepAliveEventType eve;
    KeepaliveRequest_t request = m->getKeepaliveRequest();
    keep_alive_start_request ka_start_req;
    if (call_manager) {
      std::string apn;
      if(call_manager->getApnByCid(request.cid, apn))
      {
        ka_start_req.apn = apn;
        ka_start_req.ril_ka_req = &request;
        eve.data = &ka_start_req;
      } else {
        Log::getInstance().d("There is no call with given cid " + std::to_string(request.cid));
        eve.data = nullptr;
      }
      eve.event = KeepAliveStartReq;
      eve.msg = msg;
      keep_alive->processEvent(eve);
    }
    else {
        Log::getInstance().d("call_manager is null");
        StartKeepAliveResp_t res = {};
        res.error = ResponseError_t::INTERNAL_ERROR;
        auto resp = std::make_shared<StartKeepAliveResp_t>(res);
        m->sendResponse(m, Message::Callback::Status::SUCCESS, resp);
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleStopKeepAliveRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<StopKeepAliveRequestMessage> m = std::static_pointer_cast<StopKeepAliveRequestMessage>(msg);
  if( m!= NULL && keep_alive ) {
    KeepAliveEventType eve;
    eve.event = KeepAliveStopReq;
    eve.data = m->getHandle();
    eve.msg = msg;
    keep_alive->processEvent(eve);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleGoDormantMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<GoDormantRequestMessage> m = std::static_pointer_cast<GoDormantRequestMessage>(msg);
  if( m != NULL ) {
    std::string devName = m->getParams();
    Log::getInstance().d("[" + mName + "]: device Name " + devName);
    if(call_manager != nullptr)
    {
      RIL_Errno err = RIL_E_SUCCESS;
      bool status = call_manager->handleGoDormantRequest(devName);
      if (status == false) {
        err = RIL_E_OEM_ERROR_3;
      }
      auto resp = std::make_shared<RIL_Errno>(err);
      m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
    } else {
      Log::getInstance().d("[" + mName + "]: callmanager is NULL");
      RIL_Errno err = RIL_E_OEM_ERROR_3;
      auto resp = std::make_shared<RIL_Errno>(err);
      m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleProcessScreenStateChangeMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<ProcessScreenStateChangeMessage> m = std::static_pointer_cast<ProcessScreenStateChangeMessage>(msg);
  if( m != NULL ) {
    Message::Callback::Status status = Message::Callback::Status::FAILURE;
    int ret = QCRIL_DS_ERROR;
    if (dsd_endpoint) {
      Log::getInstance().d("[DataModule]: Sending Screen State information to DSD EndPoint");
      bool screenState = m->screenState;
#ifdef RIL_FOR_MDM_LE
      screenState = mUiFilterEnabled;
#endif
      status = dsd_endpoint->handleScreenStateChangeInd(screenState);
      ret = (status == Message::Callback::Status::SUCCESS ?
            QCRIL_DS_SUCCESS : QCRIL_DS_ERROR);
    }
    auto resp = std::make_shared<int>(ret);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetApnInfoMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetApnInfoMessage> m = std::static_pointer_cast<SetApnInfoMessage>(msg);
  if( m != NULL ) {
    Log::getInstance().d("[" + mName + "]: Legacy set_apn_info request is not supported");
    RIL_Errno ret = RIL_E_SUCCESS;
    auto resp = std::make_shared<RIL_Errno>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetIsDataEnabledMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetIsDataEnabledMessage> m = std::static_pointer_cast<SetIsDataEnabledMessage>(msg);
  if ( m != NULL ) {
    bool is_data_enabled = m->getDataState();
    RIL_Errno ret = dsd_endpoint->setIsDataEnabled(is_data_enabled);
    auto resp = std::make_shared<RIL_Errno>(ret);
    Message::Callback::Status status = (ret == RIL_E_SUCCESS ?
             Message::Callback::Status::SUCCESS : Message::Callback::Status::FAILURE);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetIsDataRoamingEnabledMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetIsDataRoamingEnabledMessage> m = std::static_pointer_cast<SetIsDataRoamingEnabledMessage>(msg);
  if( m != NULL ) {
    bool is_data_roaming_enabled = m->getDataRoamingState();
    RIL_Errno ret = dsd_endpoint->setIsDataRoamingEnabled(is_data_roaming_enabled);
    auto resp = std::make_shared<RIL_Errno>(ret);
    Message::Callback::Status status = (ret == RIL_E_SUCCESS ?
             Message::Callback::Status::SUCCESS : Message::Callback::Status::FAILURE);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

/*===========================================================================

  FUNCTION:  handleSetDataProfileRequestMessage

===========================================================================*/
/*!
    @brief
    Handler to handle SetDataProfileRequestMessage message request

    @return
*/
/*=========================================================================*/
void DataModule::handleSetDataProfileRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  logBuffer.addLogWithTimestamp("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<SetDataProfileRequestMessage> m =
    std::static_pointer_cast<SetDataProfileRequestMessage>(msg);
    if( m != NULL ) {
      if(profile_handler != nullptr && !mIsPdcRefreshInProgress) {
        profile_handler->handleSetDataProfileRequestMessage(msg);
      }
      else {
        RIL_Errno result = RIL_E_INTERNAL_ERR;
        auto resp = std::make_shared<RIL_Errno>(result);
        m->sendResponse(msg, Message::Callback::Status::FAILURE, resp);
        stringstream ss;
        ss << "[" << mName << "]: " << (int)m->getSerial() << "< setDataProfileResponse resp=";
        ss << (int)result;
        Log::getInstance().d(ss.str());
        logBuffer.addLogWithTimestamp(ss.str());
      }

      if (call_manager != nullptr) {
        call_manager->clearThrottledApnsCache();
      }
    } else {
      Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
    }
    if (m != NULL && networkavailability_handler != nullptr) {
      networkavailability_handler->processSetDataProfileRequest(msg);
    }
}

void DataModule::handleSetQualityMeasurementMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetQualityMeasurementMessage> reqMsg = std::static_pointer_cast<SetQualityMeasurementMessage>(msg);
  if( reqMsg != NULL ) {
    qmi_response_type_v01 resp;
    memset(&resp, 0, sizeof(resp));
    resp.result = QMI_RESULT_FAILURE_V01;
    resp.error = QMI_ERR_INTERNAL_V01;
    dsd_set_quality_measurement_info_req_msg_v01 info = reqMsg->getInfo();
    if (dsd_endpoint) {
      resp = dsd_endpoint->setQualityMeasurement(info);
    }
    auto response = std::make_shared<qmi_response_type_v01>(resp);
    reqMsg->sendResponse(msg, Message::Callback::Status::SUCCESS, response);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleToggleDormancyIndMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<ToggleDormancyIndMessage> m = std::static_pointer_cast<ToggleDormancyIndMessage>(msg);
  if( m != NULL ) {
    int ret = QCRIL_DS_ERROR;
    Message::Callback::Status status;
    if (call_manager) {
      mLinkStatusReportEnabled = (m->dormIndSwitch == DORMANCY_INDICATIONS_ON)?true:false;
      ret = call_manager->toggleLinkActiveStateChangeReport(mLinkStatusReportEnabled);
      status = (ret == QCRIL_DS_SUCCESS ?
             Message::Callback::Status::SUCCESS : Message::Callback::Status::FAILURE);
    }
    else {
      Log::getInstance().d("call_manager is null");
      status = Message::Callback::Status::SUCCESS;
      ret = QCRIL_DS_ERROR;
    }

    auto resp = std::make_shared<int>(ret);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleToggleLimitedSysIndMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<ToggleLimitedSysIndMessage> m = std::static_pointer_cast<ToggleLimitedSysIndMessage>(msg);
  if( m != NULL ) {
    int ret = QCRIL_DS_ERROR;
    if (dsd_endpoint) {
      mLimitedIndReportEnabled = (m->sysIndSwitch == LIMITED_SYS_INDICATIONS_ON)?true:false;
      ret = dsd_endpoint->reportSystemStatusChange(false, mLimitedIndReportEnabled);
    }
    Message::Callback::Status status = (ret == QCRIL_DS_SUCCESS ?
             Message::Callback::Status::SUCCESS : Message::Callback::Status::FAILURE);
    auto resp = std::make_shared<int>(ret);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleUpdateMtuMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<UpdateMtuMessage> m = std::static_pointer_cast<UpdateMtuMessage>(msg);
  if( m != NULL ) {
    dataMtu = m->getMtu();
    Log::getInstance().d("Received MTU size through UpdateMtuMessage broadcast"
                         "as"+ std::to_string(dataMtu));
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleGetDdsSubIdMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<GetDdsSubIdMessage> m = std::static_pointer_cast<GetDdsSubIdMessage>(msg);
  if( m != NULL ) {
    DDSSubIdInfo ddsInfo = {};
    ddsInfo.dds_sub_id = preferred_data_state_info->dds;
    ddsInfo.switch_type = (dsd_dds_switch_type)(preferred_data_state_info->currentDDSSwitchType);
    auto resp = std::make_shared<DDSSubIdInfo>(ddsInfo);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleDataRequestDDSSwitchMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RequestDdsSwitchMessage> m = std::static_pointer_cast<RequestDdsSwitchMessage>(msg);
  if( m != NULL ) {
    RIL_Errno ret = RIL_E_INTERNAL_ERR;
    if (dsd_endpoint) {
        int error = 0;
        if (Message::Callback::Status::SUCCESS == dsd_endpoint->triggerDDSSwitch(m->dds_sub_info.dds_sub_id,
             error, static_cast<dsd_dds_switch_type_enum_v01>(m->dds_sub_info.switch_type))){
            ret = RIL_E_SUCCESS;
        }
    }
    auto resp = std::make_shared<RIL_Errno>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleQmiAuthServiceIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  auto shared_indMsg(std::static_pointer_cast<QmiIndMessage>(msg));
  QmiIndMsgDataStruct *indData = shared_indMsg->getData();

  if (indData != nullptr && auth_manager!= nullptr) {
    auth_manager->qmiAuthServiceIndicationHandler(indData->msgId, indData->indData,
          indData->indSize);
  } else {
    Log::getInstance().d("[" + mName + "] Unexpected, null data from message");
  }
}

void DataModule::handleQmiAuthEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
    if(auth_manager == nullptr) {
      auth_manager = std::make_unique<AuthManager>();
    } else {
      //update the key only for SSR case
      auth_manager->updateModemWithCarrierImsiKeyCache();
    }
    auth_manager->qmiAuthServiceRegisterIndications(true);
  }
  else {
    Log::getInstance().d("[" + mName + "]: ModemEndPoint is not operational");
  }
}

void DataModule::handleQmiDsdEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
    if( !mInitTracker.getDsdServiceReady() ) {
       mInitTracker.setDsdServiceReady(true);
       dsd_endpoint->registerForSmartTempDDSInd();
       dsd_endpoint->setV2Capabilities(); //This msg is same as sendAPAssistIWLANSupported
       dsd_endpoint->getDataSystemCapabilitySync();
       dsd_endpoint->generateDsdSystemStatusInd();
       dsd_endpoint->registerForRoamingStatusChanged();
       bool reportNullBearer = true;
       dsd_endpoint->reportSystemStatusChange(reportNullBearer, mLimitedIndReportEnabled);
       dsd_endpoint->registerForUiChangeInd();
       performDataModuleInitialization();
       registerForMasterRILIndications();
    }
  } else {
    //Reset all interested handlers
    Log::getInstance().d("[" + mName + "]: ModemEndPoint is not operational");
    mInitTracker.setDsdServiceReady(false);

    //If Modem is down during Voicecall, revert the tempdds
    PreferredDataEventType serviceDown;
    DDSSwitchInfo_t info;
    serviceDown.event = ServiceDown;
    serviceDown.data = &info;
    preferred_data_sm->processEvent(serviceDown);

    if (dsd_endpoint) {
      Log::getInstance().d("[" + mName + "]: resetDdsSwitchRecCapEnabled");
      dsd_endpoint->resetDdsSwitchRecCapEnabled();
    }
  }
}

void DataModule::logFn(std::string logStr) {
    Log::getInstance().d(logStr);
}

void DataModule::setIsDataAllowed(bool allow) {
  Log::getInstance().d("[" + mName + "]: isDataAllowed");
  PreferredDataEventType e;
  DDSSwitchInfo_t eventData;
  eventData.isDataAllowedOnNDDS = allow;
  e.event = setIsDataAllowedOnNDDS;
  e.data = &eventData;
  preferred_data_sm->processEvent(e);
}

void DataModule::setApAssistMode() {
  //Create AP assist variant of NetworkServiceHandler
  Message::Callback::Status status = dsd_endpoint->sendAPAssistIWLANSupported();

  Log::getInstance().d("[DataModule] sendAPAssistIWLANSupported request "
                   " result = " + std::to_string((int) status));
  if (status == Message::Callback::Status::SUCCESS)
  {
    mInitTracker.setModemCapability(true);
    //call datactl to enable/disable IWLAN
#ifndef RIL_FOR_MDM_LE
    if( mInitTracker.isIWLANEnabled() )
      initializeIWLAN();
    else if(datactlInt != nullptr) {
      datactlInt->controlIWlan(false);
    }
#endif
  } else {
    //This should never happen
    mInitTracker.setModemCapability(false);
    if(iwlanHandshakeMsgToken != INVALID_MSG_TOKEN) {
      Log::getInstance().d("[DataModule] Look for IWLANCapabilityHandshake msg "+
                                    std::to_string((int)iwlanHandshakeMsgToken));
      getDataModule().getPendingMessageList().print();
      std::shared_ptr<Message> mmm = getDataModule().getPendingMessageList().find((uint16_t)iwlanHandshakeMsgToken);
      if(mmm!=nullptr) {
        std::shared_ptr<IWLANCapabilityHandshake> iwlanHsMsg = std::static_pointer_cast<IWLANCapabilityHandshake>(mmm);
        if(iwlanHsMsg != nullptr) {
          auto resp = std::make_shared<rildata::ModemIWLANCapability_t>(rildata::ModemIWLANCapability_t::not_present);
          iwlanHsMsg->sendResponse(mmm, Message::Callback::Status::SUCCESS, resp);
          getDataModule().getPendingMessageList().erase(mmm);
          getDataModule().getPendingMessageList().print();
          iwlanHandshakeMsgToken = INVALID_MSG_TOKEN;
        } else {
          Log::getInstance().d("[DataModule] Invalid IWLANCapabilityHandShake msg. Not sending response");
        }
      } else {
        Log::getInstance().d("[DataModule] No IWLANCapabilityHandshake msg found");
        iwlanHandshakeMsgToken = INVALID_MSG_TOKEN;
      }
    } else {
      Log::getInstance().d("[DataModule] Invalid IWLANCapabilityHandshake msg token");
    }
  }
}

#ifndef RIL_FOR_LOW_RAM
#ifndef RIL_FOR_MDM_LE
void DataModule::dataPPDataCapChgCb(bool capability) {
  auto msg = std::make_shared<DataPPDataCapabilityChangeIndMessage>();
  msg->setDataPPDataSupported(capability);
  msg->broadcast();
}

void DataModule::dataPPDataRecoCb(DataSubsRecommendation_t reco) {
  auto msg = std::make_shared<DataSubRecommendationIndMessage>();
  Recommendation_t recoInfo;
  recoInfo.subs = (SubscriptionType)reco.subs;
  recoInfo.action = (Action)reco.action;
  msg->setDataSubRecommendation(recoInfo);
  msg->broadcast();
  bool action = (recoInfo.action == Action::DataAllowed)?true:false;
  getDataModule().setIsDataAllowed(action);
}
#endif

#endif //RIL_FOR_LOW_RAM

//Perform initialization that are common to services being initialized
//and mode being determined
void DataModule::performDataModuleInitialization() {
  Log::getInstance().d("[" + mName + "]: performDataModuleInitialization");
  //trigger dsi_release in case of SSR
  if(mInitCompleted && call_manager != nullptr)
  {
    if(call_manager->dsiInitStatus == DsiInitStatus_t::PENDING_RELEASE) {
      call_manager->triggerDsiRelease();
    }
    call_manager->triggerDsiInit(!mInitCompleted);
  }

  if( mInitTracker.allServicesReady()) {
    if( !mInitCompleted ) {
      rildata::BringUpCapability capability = rildata::BringUpCapability::BRING_UP_LEGACY;
      if(wds_endpoint != nullptr) {
        rildata::BringUpCapability cap = BringUpCapability::BRING_UP_APN_TYPE;
        Message::Callback::Status status = wds_endpoint->getCallBringUpCapability(cap);
        if(status == Message::Callback::Status::SUCCESS) {
          capability = cap;
        }
      }

      /* Initialize CallManager */
      call_manager = std::make_unique<CallManager>(logBuffer, util);
        call_manager->setProcessDeferredIntentCallback(std::bind(&DataModule::processDeferredIntentToChange, this, std::placeholders::_1));
        #ifdef RIL_FOR_MDM_LE
          propertySetMap("persist.vendor.radio.max_retry_timeout", PERSIST_VENDOR_RADIO_MAX_RETRY_TIMEOUT);
        #endif

      string propValue = readProperty("persist.vendor.radio.max_retry_timeout", "");
      Log::getInstance().d("[" + mName + "]: Partial retry max timeout property is " + propValue);
      unsigned long maxTimeout = DEFAULT_MAX_PARTIAL_RETRY_TIMEOUT;
      if (!propValue.empty()) {
        maxTimeout = stoul(propValue);
      }
      Log::getInstance().d("[CallManager]: Using call bringup capability = " + std::to_string((int)capability));
      call_manager->init(mInitTracker.isAPAssistMode(), mInitTracker.isPartialRetryEnabled(), maxTimeout, capability, !mInitCompleted);
      call_manager->unthrottleAllRadioApns();
      call_manager->unthrottleAllIWlanApns();
      call_manager->registerDataRegistrationRejectCause(mRegistrationFailureCauseReport);
      call_manager->setCleanupKeepAliveCallback(std::bind(&DataModule::cleanupKeepAlive, this, std::placeholders::_1));
      qcrilDataInit();

      /* Initialize profileHandler */
      auto cb = std::make_shared<std::function<void(std::shared_ptr<Message>, RIL_Errno)>>
                        (std::bind(&DataModule::sendIAResponse, this, std::placeholders::_1, std::placeholders::_2));
      profile_handler = std::make_unique<ProfileHandler>(logBuffer, util, cb);
      profile_handler->init(mInitTracker.isAPAssistMode(), profileUpdatePropertySet && profileUpdateCarrierSet);

      #ifndef RIL_FOR_MDM_LE
      loadDataQos();
      registerForSecureModeIndications();
      #endif

      if(mInitTracker.isAPAssistMode()) {
        network_service_handler = std::make_unique<ApAssistNetworkServiceHandler>();
      } else {
        //Create legacy mode variant of NetworkServiceHandler
        network_service_handler = std::make_unique<NetworkServiceHandler>();
      }
    } //if(!mInitCompleted)

    //Execute for both bootup and SSR cases
    /* Set ApAssist/Legacy Mode */
    if( mInitTracker.isAPAssistMode() )
    {
      setApAssistMode();
    }
    else
    { //Legacy mode
      network_service_handler->processQmiDsdSystemStatusInd(&mCachedSystemStatus);
    }

    //With initializeIwlan(), this mCachedSystemStatus gets set
    if(call_manager != nullptr) {
      call_manager->processQmiDsdSystemStatusInd(&mCachedSystemStatus);
      call_manager->unthrottleAllRadioApns();
      call_manager->unthrottleAllIWlanApns();
    }

    if(!mInitCompleted)
      mInitCompleted = true;
  }
}

#ifdef FEATURE_DATA_LQE
void DataModule::handleSetLinkCapFilterMessage(std::shared_ptr<Message> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    std::shared_ptr<SetLinkCapFilterMessage> m = std::static_pointer_cast<SetLinkCapFilterMessage>(msg);
    if (m != nullptr) {
        auto rf = m->getParams();
        int result = -1;
        if (lceHandler.toggleReporting(static_cast<int>(rf))) {
            result = 0;
        }
        auto resp = std::make_shared<int>(result);
        m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
    }
    else {
        Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
    }
}

void DataModule::handleSetLinkCapRptCriteriaMessage(std::shared_ptr<Message> msg)
{
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    std::shared_ptr<SetLinkCapRptCriteriaMessage> m = std::static_pointer_cast<SetLinkCapRptCriteriaMessage>(msg);
    if( m != nullptr ) {
        auto rf = m->getParams();
        Message::Callback::Status status = Message::Callback::Status::SUCCESS;
        LinkCapCriteriaResult_t result = lceHandler.setCriteria(rf);
        if (result != rildata::LinkCapCriteriaResult_t::success) {
            status = Message::Callback::Status::FAILURE;
        }
        auto resp = std::make_shared<rildata::LinkCapCriteriaResult_t>(result);
        m->sendResponse(msg, status, resp);
    } else {
        Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
    }
}
#endif /*FEATURE_DATA_LQE*/

void DataModule::handleRilEventDataCallback(std::shared_ptr<Message> m) {
  auto msg = std::static_pointer_cast<RilEventDataCallback>(m);
  if (msg == nullptr) {
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  logBuffer.addLogWithTimestamp("[" + mName + "]: Handling msg = " + msg->dump());
  if (call_manager != nullptr) {
    if (isDsiex2Supported) {
      EventDataType *pEvtData = msg->getEventData();
      if (pEvtData == nullptr) {
        Log::getInstance().d("[" + mName + "]: EventData is Nullptr");
        return;
      }
      dsi_hndl_t handle = pEvtData->hndl;
      DsiWrapper::getInstance().dsiUpdateInfo(handle, pEvtData->payload_ex);
    }
    call_manager->handleCallEventMessage(CallEventTypeEnum::RilEventDataCallback, msg);
  }
  else {
     Log::getInstance().d("call_manager is null");
  }
}
/*===========================================================================

  FUNCTION:  handleSetInitialAttachApn

===========================================================================*/
/*!
    @brief
    Handler to handle handleSetInitialAttachApn message request in LE
    target

    @return
*/
/*=========================================================================*/
void DataModule::handleSetInitialAttachApn(std::shared_ptr<Message> msg)
{
  std::shared_ptr<SetInitialAttachApnRequestMessage> m =
       std::static_pointer_cast<SetInitialAttachApnRequestMessage>(msg);

  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  logBuffer.addLogWithTimestamp("[" + mName + "]: Handling msg = " + msg->dump());

  if( m != NULL ) {
    if(profile_handler != nullptr && !mIsPdcRefreshInProgress) {
      PreferredDataEventType IAEvent;
      DDSSwitchInfo_t eventData;
      eventData.msg = m;
      //send IAstarted event for masterRil
      IAEvent.event = IAStarted;
      IAEvent.data = &eventData;
      preferred_data_sm->processEvent(IAEvent);
    } else {
      RIL_Errno result = RIL_E_INTERNAL_ERR;
      auto resp = std::make_shared<RIL_Errno>(result);
      m->sendResponse(msg, Message::Callback::Status::FAILURE, resp);
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

/*===========================================================================

FUNCTION:  handleSetInitialAttachApn

===========================================================================*/
/*!
    @brief
    Handler to handle handleProcessInitialAttachRequestMessage request

    @return
*/
/*=========================================================================*/
void DataModule::handleProcessInitialAttachRequestMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "] : handleProcessInitialAttachRequestMessage");

  std::shared_ptr<ProcessInitialAttachRequestMessage> m =
       std::static_pointer_cast<ProcessInitialAttachRequestMessage>(msg);
  if(m != nullptr)
  {
    #ifdef QMI_RIL_UTF
    preferred_data_state_info->isRilIpcNotifier = TRUE;
    #endif
    initialAttachHandler(m->getMessage());
  }
}


void DataModule::initialAttachHandler(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "] : initialAttachHandler");
  std::shared_ptr<SetInitialAttachApnRequestMessage> m =
       std::static_pointer_cast<SetInitialAttachApnRequestMessage>(msg);
  if(m!= nullptr && profile_handler != nullptr) {
    profile_handler->handleInitialAttachRequest(m);
  } else {

    Log::getInstance().d("Could not process the msg");
  }
}

void DataModule::sendIAResponse(std::shared_ptr<Message> msg, RIL_Errno result)
{
  std::shared_ptr<SetInitialAttachApnRequestMessage> m =
            std::static_pointer_cast<SetInitialAttachApnRequestMessage>(msg);
  if ( m != nullptr )
  {
    Log::getInstance().d("[" + mName + "} : Sending response");
    auto resp = std::make_shared<RIL_Errno>(result);
    if( result != RIL_E_SUCCESS )
    {
      m->sendResponse(m, Message::Callback::Status::FAILURE, resp);
    }
    else
    {
      m->sendResponse(m, Message::Callback::Status::SUCCESS, resp);
    }
    stringstream ss;
    ss << "[Datamodule]: " << (int)m->getSerial() << "< setInitialAttachResponse resp=";
    ss << (int)result;
    Log::getInstance().d(ss.str());

    PreferredDataEventType IAComplete;
    DDSSwitchInfo_t eventData;
    eventData.msg = m;
    IAComplete.data = &eventData;
    if(preferred_data_state_info != nullptr && preferred_data_state_info->isRilIpcNotifier) {
      IAComplete.event = IACompletedOnMasterRIL;
    } else {
      IAComplete.event = IACompletedOnSlaveRIL;
    }
    preferred_data_sm->processEvent(IAComplete);
  } else {
    Log::getInstance().d("Could not process the msg");
  }
}

void DataModule::handleIATimeoutMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "] : handleIATimeoutMessage");

  std::shared_ptr<IATimeoutMessage> m =
       std::static_pointer_cast<IATimeoutMessage>(msg);
  if( m != nullptr && preferred_data_state_info != nullptr &&
      preferred_data_state_info->isRilIpcNotifier) {
    PreferredDataEventType IATimeout;
    IATimeout.event = IATimerExpired;
    preferred_data_sm->processEvent(IATimeout);
  }
}

void DataModule::handleGetModemAttachParamsRetryMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "] : handleRetryAttachTimeOutMessage");

  std::shared_ptr<GetModemAttachParamsRetryMessage> m =
       std::static_pointer_cast<GetModemAttachParamsRetryMessage>(msg);

  if(m != nullptr && profile_handler) {
    profile_handler->handleGetModemAttachParamsRetryMessage();
  }
}

/*!
    @brief
    Handler to handle SetInitialAttachApnRequestMessage message request

    @return
*/
/*=========================================================================*/
void DataModule::handleSetLteAttachPdnListActionResult(std::shared_ptr<Message> msg)
{
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    std::shared_ptr<SetLteAttachPdnListActionResultMessage> m = std::static_pointer_cast<SetLteAttachPdnListActionResultMessage>(msg);
    if( m != NULL && profile_handler != NULL ) {
      Log::getInstance().d("[DataModule]::Invoking handleWdsUnSolInd" );
      profile_handler->handleWdsUnSolInd(&(m->getParams()));
    } else {
      Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
    }
}
/*===========================================================================

  FUNCTION:  handleNasRequestDataShutdown

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when NasRequestDataShutdownMessage is received

    @return
*/
/*=========================================================================*/
void DataModule::handleNasRequestDataShutdown(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<NasRequestDataShutdownMessage> m = std::static_pointer_cast<NasRequestDataShutdownMessage>(msg);
  if (m != NULL)
  {
    NasRequestDataShutdownResponse ret = NasRequestDataShutdownResponse::FAILURE;
    if(dsd_endpoint && dsd_endpoint->deviceShutdownRequest()){
      ret = NasRequestDataShutdownResponse::SUCCESS;
    }
    auto resp = std::make_shared<NasRequestDataShutdownResponse>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

/*============================================================================

    retrieveUIMAppStatusFromAppInfo

============================================================================*/
/*!
    @brief
    Retrieve aid buffer application info

    @return
    None
*/
/*=========================================================================*/
int DataModule::retrieveUIMAppStatusFromAppInfo(RIL_UIM_AppStatus *application, string &aid_buffer, RIL_UIM_AppType *app_type)
{
    int res = E_FAILURE;

    Log::getInstance().d("[Datamodule]: handling retrieveUIMAppStatusFromAppInfo");
    if (application && app_type)
    {
        Log::getInstance().d("app type " + std::to_string(application->app_type));
        Log::getInstance().d("app State " + std::to_string(application->app_state));

        if ((application->app_state == RIL_UIM_APPSTATE_READY) &&
            !application->aid_ptr.empty())
        {
          aid_buffer = application->aid_ptr;
          Log::getInstance().d("aid buffer " + aid_buffer);
          *app_type = application->app_type;
          res = E_SUCCESS;
        }
    }
    return res;
}

/*============================================================================

    retrieveUIMCardStatus

============================================================================*/
/*!
    @brief
    Retrieve aid buffer card status info

    @return
    None
*/
/*=========================================================================*/
int DataModule::retrieveUIMCardStatus( std::shared_ptr<RIL_UIM_CardStatus> ril_card_status, string &aid_buffer, RIL_UIM_AppType *app_type)
{
    int               res = E_FAILURE;
    int               index;

    Log::getInstance().d("[Datamodule]: handling retrieveUIMCardStatus");
    if (ril_card_status)
    {
        if (ril_card_status->card_state == RIL_UIM_CARDSTATE_PRESENT)
        {
            Log::getInstance().d("card is present");
            if (ril_card_status->gsm_umts_subscription_app_index != -1)
            {
                index = ril_card_status->gsm_umts_subscription_app_index;
                /* retrieve aid from gsm umts subscription app info */
                res = retrieveUIMAppStatusFromAppInfo(
                                                &ril_card_status->applications[index],
                                                aid_buffer, app_type);
               Log::getInstance().d("res " + std::to_string(res));
            }

            if ((res == E_FAILURE) &&
                (ril_card_status->cdma_subscription_app_index != -1))
            {
                index = ril_card_status->cdma_subscription_app_index;
                /* retrieve aid from cdma subscription app info */
                res = retrieveUIMAppStatusFromAppInfo(
                                                &ril_card_status->applications[index],
                                                aid_buffer, app_type);
            }

            if ((res == E_FAILURE) &&
                (ril_card_status->ims_subscription_app_index != -1))
            {
                index = ril_card_status->ims_subscription_app_index;
                /* retrieve aid from ims subscription app info */
                res = retrieveUIMAppStatusFromAppInfo(
                                                &ril_card_status->applications[index],
                                                aid_buffer, app_type);
            }
        }
    }
    return res;
}

/*============================================================================

    processMccMncInfo

============================================================================*/
/*!
    @brief
    Process mcc mnc info

    @return
    None
*/
/*=========================================================================*/
SimInfo getMccMncInfo
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
)
{
  qcril_uim_mcc_mnc_info_type *uim_mcc_mnc_info = NULL;

  Log::getInstance().d("getMccMncInfo: ENTRY");

  if ((params_ptr == NULL) || (ret_ptr == NULL))
  {
    Log::getInstance().d("ERROR!! Invalid input, cannot process request");
    return {};
  }

  uim_mcc_mnc_info = (qcril_uim_mcc_mnc_info_type*)params_ptr->data;
  if (uim_mcc_mnc_info == NULL)
  {
    Log::getInstance().d("NULL uim_mcc_mnc_info");
    return {};
  }

  if (uim_mcc_mnc_info->err_code != RIL_UIM_E_SUCCESS)
  {
    Log::getInstance().d("uim_get_mcc_mnc_info error:"+ std::to_string(uim_mcc_mnc_info->err_code));
    return {};
  }

  //According to the declaration of size in 'UimGetMccMncRequestMsg.h'
  //each of mcc & mnc is 4 bytes, adding the error check based on this size

  if ( (uim_mcc_mnc_info->mcc[MCC_LENGTH - 1] != '\0')
    || (uim_mcc_mnc_info->mnc[MNC_LENGTH - 1] != '\0') )
  {
    Log::getInstance().d("ERROR!! Improper input received. Either of MCC or MNC is not NULL terminated");
    return {};
  }
  std::string mcc = uim_mcc_mnc_info->mcc;
  std::string mnc = uim_mcc_mnc_info->mnc;
  Log::getInstance().d("mcc:"+ mcc+"mnc="+ mnc);
  return { mcc, mnc };
}

/*===========================================================================

    qcrilDataUimEventAppStatusUpdate

============================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_CM_CARD_APP_STATUS_CHANGED

    @return
    None
*/
/*=========================================================================*/
void DataModule::qcrilDataUimEventAppStatusUpdate ( const qcril_request_params_type *const params_ptr, qcril_request_return_type *const ret_ptr)
{
  RIL_UIM_AppStatus              *card_app_info;
  string                          aid = {};
  RIL_UIM_AppType                 request_app_type;

  Log::getInstance().d("qcrilDataUimEventAppStatusUpdate:: ENTRY");
  QCRIL_NOTUSED(ret_ptr);

  if (!params_ptr)
  {
    Log::getInstance().d("PARAMS ptr is NULL");
    return;
  }

  card_app_info = (RIL_UIM_AppStatus *)(params_ptr->data);

  /* Process only this slots SIM card applications */
  if (card_app_info != NULL &&
      card_app_info->app_state == RIL_UIM_APPSTATE_READY)
  {
    Log::getInstance().d("app type"+std::to_string(card_app_info->app_type)+
                         "app state"+std::to_string(card_app_info->app_state));

    auto card_status = std::make_shared<UimGetCardStatusRequestSyncMsg>(global_instance_id);
    std::shared_ptr<RIL_UIM_CardStatus> ril_card_status = nullptr;

    /* retrieve card status info */
    if (card_status != nullptr && (card_status->dispatchSync(ril_card_status) != Message::Callback::Status::SUCCESS))
    {
      Log::getInstance().d("Get card status request failed");
      return;
    }

    if(ril_card_status != nullptr && ril_card_status->err != RIL_UIM_E_SUCCESS)
    {
      Log::getInstance().d("ril card status failed");
      return;
    }
    /* retrieve aid from card status */
    if ((retrieveUIMCardStatus(ril_card_status, aid, &request_app_type))!= E_SUCCESS)
    {
      Log::getInstance().d("Retrieval of AID from card status failed");
      return;
    }

    Log::getInstance().d("Received SIM aid_buffer="+aid);
    qcril_uim_app_type  app_type = QCRIL_UIM_APP_UNKNOWN;

    //proceed only when memory is allocated
    if(aid.empty())
    {
      Log::getInstance().d("AID Memory allocation failed");
      return;
    }

    switch(request_app_type)
    {
      case RIL_UIM_APPTYPE_SIM:
        app_type = QCRIL_UIM_APP_SIM;
        break;
     case RIL_UIM_APPTYPE_USIM:
        app_type = QCRIL_UIM_APP_USIM;
        break;
     case RIL_UIM_APPTYPE_RUIM:
        app_type = QCRIL_UIM_APP_RUIM;
        break;
     case RIL_UIM_APPTYPE_CSIM:
        app_type = QCRIL_UIM_APP_CSIM;
        break;
     default:
        app_type = QCRIL_UIM_APP_UNKNOWN;
        break;
     }

    GenericCallback<qcril_uim_mcc_mnc_info_type> cb([this](
    std::shared_ptr<Message> msg, Message::Callback::Status status,
    std::shared_ptr<qcril_uim_mcc_mnc_info_type> resp) -> void {
      if (status == Message::Callback::Status::SUCCESS) {
        Log::getInstance().d("[GetMccMncCallback]: [msg = " +
            msg->dump() + "] executed. SUCCESS");
        if(resp)
        {
          qcril_request_params_type params_ptr;
          qcril_request_return_type ret_ptr;
          std::memset(&ret_ptr, 0, sizeof(ret_ptr));
          std::memset(&params_ptr, 0, sizeof(params_ptr));
          params_ptr.modem_id = QCRIL_DEFAULT_MODEM_ID;
          params_ptr.data = static_cast<void*>(new char[sizeof(qcril_uim_mcc_mnc_info_type)]());
          if(params_ptr.data != nullptr) {
              memcpy(params_ptr.data, resp.get(), sizeof(qcril_uim_mcc_mnc_info_type));
          }
    #ifndef RIL_FOR_MDM_LE
          SimInfo info = getMccMncInfo(&params_ptr, &ret_ptr);
          checkProfileUpdateAllowed(info.mcc, info.mnc);
    #endif
          if(params_ptr.data != nullptr) {
              delete[] (static_cast<char*>(params_ptr.data));
              params_ptr.data = nullptr;
          }
        } else {
          Log::getInstance().d("[GetMccMncCallback]: resp is NULL..not processing ");
        }
      } else {
        Log::getInstance().d("[GetMccMncCallback]: Callback[msg = " +
            msg->dump() + "] executed. FAILURE !!");
      }
    });
     std::shared_ptr<UimGetMccMncRequestMsg> req =
     std::make_shared<UimGetMccMncRequestMsg>(aid, app_type, &cb);
     if(req)
     {
       Log::getInstance().d("Dispatching UimGetMccMncRequestMsg Message");
       req->dispatch();
     }

   } else {
       Log::getInstance().d("Card APP info is NULL or slot id mismatch or Card APP status isn't READY");
   }
}

void DataModule::handleUimCardAppStatusIndMsg(std::shared_ptr<Message> m)
{
  qcril_request_return_type ret_ptr;
  qcril_request_params_type params_ptr;
  Log::getInstance().d("[DataModule]: Handling msg = " + m->dump());

  std::shared_ptr<UimCardAppStatusIndMsg> msg =
       std::static_pointer_cast<UimCardAppStatusIndMsg>(m);
  std::memset(&params_ptr, 0, sizeof(params_ptr));
  std::memset(&ret_ptr, 0, sizeof(ret_ptr));

  if( msg != NULL )
  {
    params_ptr.data = static_cast<void *>(new char[sizeof(RIL_UIM_AppStatus)]);
    if(params_ptr.data)
    {
      std::memcpy(params_ptr.data, &msg->get_app_info(), sizeof(RIL_UIM_AppStatus));
      params_ptr.datalen = sizeof(RIL_UIM_AppStatus);
      params_ptr.modem_id = QCRIL_DEFAULT_MODEM_ID;
#ifndef QMI_RIL_UTF
      qcrilDataUimEventAppStatusUpdate (&params_ptr, &ret_ptr);
#endif
      delete[] (static_cast<char*>(params_ptr.data));
      params_ptr.data = nullptr;
    } else
    {
      Log::getInstance().d("[DataModule]: Memory allocation failure");
    }
  } else
  {
    Log::getInstance().d("[" + mName + "]: Improper message received");
  }
}

void DataModule::handlePdcRefreshIndication(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + m->dump());

  std::shared_ptr<PdcRefreshIndication> msg =
       std::static_pointer_cast<PdcRefreshIndication>(m);
  if (msg != nullptr)
  {
    pdc_refresh_event_enum_v01 event = msg->getEvent();
    if(event == PDC_EVENT_REFRESH_START_V01) {
      mIsPdcRefreshInProgress = true;
      if(networkavailability_handler != nullptr) {
        networkavailability_handler->clearNetworkAvailabilityCache();
      }
    } else if (event == PDC_REFRESH_EVENT_ENUM_MAX_ENUM_VAL_V01) {
      /*modem ssr scenario*/
      mIsPdcRefreshInProgress = false;
      Log::getInstance().d("[DataModule]: Modem is non operational. Resetting pdc refresh in progress flag");
    } else {
      mIsPdcRefreshInProgress = false;
    }
    Manager::getInstance().refresh((PDCRefreshStatus)event);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received");
  }
}

void DataModule::handleWdsThrottleInfoIndMessage(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + m->dump());
  std::shared_ptr<WdsThrottleInfoIndMessage> msg =
       std::static_pointer_cast<WdsThrottleInfoIndMessage>(m);
  if (msg != nullptr)
  {
    if (call_manager != nullptr) {
      call_manager->handleWdsThrottleInfoInd(msg->getThrottleInfo());
    }
  } else
  {
    Log::getInstance().d("[" + mName + "]: Improper message received");
  }
}

void DataModule::handleGetPdnThrottleTimeResponseInd(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + m->dump());
  std::shared_ptr<GetPdnThrottleTimeResponseInd> msg =
       std::static_pointer_cast<GetPdnThrottleTimeResponseInd>(m);
  if (msg != nullptr)
  {
    if (call_manager != nullptr) {
      call_manager->handleCallEventMessage(CallEventTypeEnum::ThrottleTimeAvailable, msg);
    }
  } else
  {
    Log::getInstance().d("[" + mName + "]: Improper message received");
  }
}

void DataModule::handlePlmnChangedIndMessage(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + m->dump());
  std::shared_ptr<QcRilUnsolCurrentPlmnChangedMessage> msg =
       std::static_pointer_cast<QcRilUnsolCurrentPlmnChangedMessage>(m);
  if (msg != nullptr)
  {
    auto plmn = msg->getRegisteredPlmn();
    if (call_manager != nullptr && plmn) {
      call_manager->unthrottleAllRadioApns();
    }
  } else
  {
    Log::getInstance().d("[" + mName + "]: Improper message received");
  }
}

/*===========================================================================

  FUNCTION:  handleQmiWdsEndpointStatusIndMessage

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when QMI WDS modem endpoint status is changed

    @return
*/
/*=========================================================================*/
void DataModule::handleQmiWdsEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
    if (!mInitTracker.getWdsServiceReady()) {
      mInitTracker.setWdsServiceReady(true);
      wds_endpoint->registerforWdsIndication();
      wds_endpoint->registerDataRegistrationRejectCause(mRegistrationFailureCauseReport);
      string propValue = readProperty("persist.vendor.radio.nswo_enabled", "0");
      wds_endpoint->setV2Capabilities(propValue == "1");
      performDataModuleInitialization();
    }
  } else if (shared_indMsg->getState() == ModemEndPoint::State::NON_OPERATIONAL) {
    if (call_manager) {
      call_manager->cleanUpAllBearerAllocation();
      call_manager->triggerDsiRelease();
      call_manager->sendFailureForIdleCalls();
    } else {
      Log::getInstance().d("call_manager is null");
    }
    if(keep_alive) {
      KeepAliveEventType eve;
      eve.event = KeepAliveReleaseAllHandles;
      keep_alive->processEvent(eve);
    }
    //Reset all interested handlers
    mInitTracker.setWdsServiceReady(false);
    // release QDP attach profile
    if(profile_handler) {
      profile_handler->releaseQdpAttachProfile();
    }
    Log::getInstance().d("[" + mName + "]: WDSModemEndPoint is not operational");
  }
}

#ifdef FEATURE_DATA_LQE
/*===========================================================================*/
/*!
    @brief
    Handler which gets invoked when QMI OTT modem endpoint status is changed

    @return
*/
/*=========================================================================*/
void DataModule::handleQmiOttEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
    lceHandler.Init();
    if (mInitCompleted) {
      lceHandler.handlemodemSSR();
    }
    Log::getInstance().d("[" + mName + "]: OTTModemEndPoint is operational");
  } else if (shared_indMsg->getState() == ModemEndPoint::State::NON_OPERATIONAL) {
    lceHandler.deInit();
    Log::getInstance().d("[" + mName + "]: OTTModemEndPoint is not operational");
  }
}
#endif /* FEATURE_DATA_LQE */

/*===========================================================================

  FUNCTION:  handleDataConnectionStateChangedMessage

===========================================================================*/
/*!
    @brief
    Handler when data connection state is changed

    @return
*/
/*=========================================================================*/
void DataModule::handleDataConnectionStateChangedMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<CallStatusMessage> m = std::static_pointer_cast<CallStatusMessage>(msg);
  if (m != NULL) {
    const CallStatusParams callParams = m->getCallParams();
    if (callParams.evt == QCRIL_DATA_EVT_CALL_RELEASED) {
      if (call_manager) {
        //call_manager->cleanUpBearerAllocation((int32_t)m->getCallId());
      } else {
        Log::getInstance().d("call_manager is null");
      }
    }
  }
}
/*===========================================================================

  FUNCTION:  handleDsdSystemStatusInd

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when DsdSystemStatusMessage is received.

    @return
*/
/*=========================================================================*/
void DataModule::handleDsdSystemStatusInd(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<DsdSystemStatusMessage> m = std::static_pointer_cast<DsdSystemStatusMessage>(msg);
  if (m != NULL) {
    mCachedSystemStatus = m->getParams();
    if (network_service_handler) {
      network_service_handler->processQmiDsdSystemStatusInd(&mCachedSystemStatus);
    }
    if (networkavailability_handler) {
      //Update global preferred system in NetworkAvailabilityHander
      if(mCachedSystemStatus.avail_sys_valid && mCachedSystemStatus.avail_sys_len >= 1) {
        networkavailability_handler->setGlobalPreferredSystem(mCachedSystemStatus.avail_sys[0]);
      }
      if (mCachedSystemStatus.apn_avail_sys_info_valid) {
        networkavailability_handler->processQmiDsdSystemStatusInd(mCachedSystemStatus.apn_avail_sys_info,
                                                              mCachedSystemStatus.apn_avail_sys_info_len);
      }
    }
    if (call_manager) {
      call_manager->processQmiDsdSystemStatusInd(&mCachedSystemStatus);
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

/*===========================================================================

  FUNCTION:  handleSetPreferredDataModem

===========================================================================*/
/*!
    @brief
    Handler for AP triggered DDS switch

    @return
*/
/*=========================================================================*/
void DataModule::handleSetPreferredDataModem(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<SetPreferredDataModemRequestMessage> m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(msg);
  if (m != NULL) {
    if (TempddsSwitchRequestPending) {
      TimeKeeper::getInstance().clear_timer(tempDDSSwitchRequestTimer);
    }
    PreferredDataEventType setPreferredDataEvent;
    DDSSwitchInfo_t eventData;
    eventData.subId = m->getModemId();
    eventData.msg = m;

    setPreferredDataEvent.event = SetPreferredDataModem;
    setPreferredDataEvent.data = &eventData;
    preferred_data_sm->processEvent(setPreferredDataEvent);
  }
}

/*===========================================================================

  FUNCTION:  handleCurrentDDSIndMessage

===========================================================================*/
/*!
    @brief
    Handler for MP triggered DDS switch

    @return
*/
/*=========================================================================*/
void DataModule::handleCurrentDDSIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<CurrentDDSIndMessage> m = std::static_pointer_cast<CurrentDDSIndMessage>(msg);
  if (m != NULL) {
    Log::getInstance().d("DDS is on sub:" + std::to_string(m->getSubId()) + " with switchType:" + std::to_string(m->getSwitchType()));
    DDSSubIdInfo ddsInfo;
    ddsInfo.dds_sub_id = m->getSubId();
    ddsInfo.switch_type = m->getSwitchType();

    Log::getInstance().d("[" + mName + "]: Sending response to qcril common");
    auto msg = std::make_shared<rildata::NewDDSInfoMessage>(global_instance_id, ddsInfo);
    msg->broadcast();

    // only listen for current DDS indication on main ril

    if (preferred_data_state_info != nullptr) {
      PreferredDataEventType currentDdsEvent;
      DDSSwitchInfo_t eventData;
      eventData.subId = m->getSubId();
      eventData.switch_type = (dsd_dds_switch_type_enum_v01)(m->getSwitchType());
      currentDdsEvent.event = CurrentDDSInd;
      currentDdsEvent.data = &eventData;
      preferred_data_sm->processEvent(currentDdsEvent);
    }
  }
}

void DataModule::handleCurrentRoamingStatusChangedMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<CurrentRoamingStatusChangedMessage> m =
    std::static_pointer_cast<CurrentRoamingStatusChangedMessage>(msg);
  if (m != NULL) {
    if (call_manager != nullptr) {
      call_manager->updateCurrentRoamingStatus(m->isRoaming());
    }
  }
}

/*===========================================================================

  FUNCTION:  handleDDSSwitchResultIndMessage

===========================================================================*/
/*!
    @brief
    Handler when DDS switch started by modem

    @return
*/
/*=========================================================================*/
void DataModule::handleDDSSwitchResultIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<DDSSwitchResultIndMessage> m = std::static_pointer_cast<DDSSwitchResultIndMessage>(msg);
  if (m != NULL) {
    PreferredDataEventType ddsSwitchEvent;
    DDSSwitchInfo_t eventData;
    eventData.msg = m;

    eventData.switchAllowed = DDSSwitchRes::FAIL;
    if (m->getAllowed()) {
      eventData.switchAllowed = DDSSwitchRes::ALLOWED;
    }

    if (m->getError() != TriggerDDSSwitchError::NO_ERROR) {
      eventData.switchAllowed = DDSSwitchRes::ERROR;
    }

    ddsSwitchEvent.event = DDSSwitchInd;
    ddsSwitchEvent.data = &eventData;
    preferred_data_sm->processEvent(ddsSwitchEvent);
  }
}

void DataModule::registerForMasterRILIndications() {
  Log::getInstance().d("[" + mName + "] : registerForMasterRILIndications");
  if (mInitTracker.getDsdServiceReady() && preferred_data_state_info->isRilIpcNotifier)
  {
    dsd_endpoint->registerForCurrentDDSInd();
    DDSSubIdInfo ddsInfo;
    Log::getInstance().d("[" + mName + "]:dispatching currentDDSSync message");
    auto status = ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint()->getCurrentDDSSync(ddsInfo);
    if(status == Message::Callback::Status::SUCCESS) {
      auto msg = std::make_shared<rildata::NewDDSInfoMessage>(global_instance_id, ddsInfo);
      msg->broadcast();

      //posting currentDDS
      PreferredDataEventType currentDdsEvent;
      DDSSwitchInfo_t eventData;
      eventData.subId = ddsInfo.dds_sub_id;
      eventData.switch_type = (dsd_dds_switch_type_enum_v01)ddsInfo.switch_type;
      currentDdsEvent.event = CurrentDDSInd;
      currentDdsEvent.data = &eventData;
      preferred_data_sm->processEvent(currentDdsEvent);
    }
    dsd_endpoint->getMasterRILSpecificCapability();
    dsd_endpoint->registerForCIWLAN();
  }
}

/*===========================================================================

  FUNCTION:  handleRadioConfigClientConnectedMessage

===========================================================================*/
/*!
    @brief
    Handler when client registers response functions with RadioConfig HAL

    @return
*/
/*=========================================================================*/
void DataModule::handleRadioConfigClientConnectedMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  Log::getInstance().d("Client connected to ril instance " + std::to_string(global_instance_id));
  // this flag indicates that the current ril instance will be broadcasting IPC messages
  preferred_data_state_info->isRilIpcNotifier = true;
  QmiSetupRequestCallback callback("voice-token");

  #ifdef QMI_RIL_UTF
  std::shared_ptr<RadioConfigClientConnectedMessage> m = std::static_pointer_cast<RadioConfigClientConnectedMessage>(msg);
  if(m != nullptr)
  {
    preferred_data_state_info->isRilIpcNotifier = m->getRilType();
  }
  #endif
  preferred_data_state_info->tempDdsSwitchRecommendInfo.reset();

#ifndef RIL_FOR_MDM_LE
  if (voiceCallEndPointSub0 == nullptr) {
    voiceCallEndPointSub0 = std::shared_ptr<VoiceCallModemEndPoint>(new VoiceCallModemEndPoint("VoiceSub0EndPoint", SubId::PRIMARY));
    qcril_instance_id_e_type inst_id = static_cast<qcril_instance_id_e_type>(global_instance_id);
    voiceCallEndPointSub0->requestSetup("voice-token-client-server", inst_id, &callback);
  }

  if (voiceCallEndPointSub1 == nullptr) {
    voiceCallEndPointSub1 = std::shared_ptr<VoiceCallModemEndPoint>(new VoiceCallModemEndPoint("VoiceSub1EndPoint", SubId::SECONDARY));
    qcril_instance_id_e_type inst_id = static_cast<qcril_instance_id_e_type>(global_instance_id);
    voiceCallEndPointSub1->requestSetup("voice-token-client-server", inst_id, &callback);
  }
#endif

  registerForMasterRILIndications();

#ifdef QMI_RIL_UTF
  if (preferred_data_state_info->isRilIpcNotifier)
#endif
  {
#ifndef RIL_FOR_LOW_RAM
    if(datactlInt != nullptr) {
      datactlInt->registerDataPPDataInds(dataPPDataCapChgCb, dataPPDataRecoCb);
    }
#endif
  }
}

void DataModule::handleInformDDSSUBChangedMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "] : Handling msg =" + msg->dump());
  auto m = std::static_pointer_cast<InformDDSSUBChangedMessage>(msg);
  if( m != nullptr && call_manager != nullptr)
  {
     call_manager->disconnectCallFromOlderDDSSub();
  } else {
    Log::getInstance().d("[" + mName + "] : Improper message receied = " + msg->dump());
  }
}

/*===========================================================================

  FUNCTION:  handleDDSSwitchTimeoutMessage

===========================================================================*/
/*!
    @brief

    @return
*/
/*=========================================================================*/
void DataModule::handleDDSSwitchTimeoutMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<DDSSwitchTimeoutMessage> m = std::static_pointer_cast<DDSSwitchTimeoutMessage>(msg);
  if (m != NULL) {
    PreferredDataEventType timeoutEvent;
    DDSSwitchInfo_t eventData;
    eventData.msg = m;

    switch (m->getType()) {
       case DDSTimeOutSwitchType::DDSTimeOutSwitch:
         timeoutEvent.event = DDSSwitchTimerExpired;
         break;
       case DDSTimeOutSwitchType::TempDDSTimeOutSwitch:
         timeoutEvent.event = TempDDSSwitchTimerExpired;
         break;
    }
    timeoutEvent.data = &eventData;
    preferred_data_sm->processEvent(timeoutEvent);
    if(preferred_data_state_info->tempDdsSwitchRecommendInfo.has_value()) {
        Log::getInstance().d("[" + mName + "]: Clearing the Recommendation cache");
        preferred_data_state_info->tempDdsSwitchRecommendInfo.reset();
    }
  }
}

/*===========================================================================

  FUNCTION:  handleDDSSwitchIPCMessage

===========================================================================*/
/*!
    @brief

    @return
*/
/*=========================================================================*/
void DataModule::handleDDSSwitchIPCMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<DDSSwitchIPCMessage> m = std::static_pointer_cast<DDSSwitchIPCMessage>(msg);
  if (m != NULL) {
    // do not listen for IPC messages on main ril
    if (preferred_data_state_info != nullptr &&
        !preferred_data_state_info->isRilIpcNotifier) {
      PreferredDataEventType ipcEvent;
      DDSSwitchInfo_t eventData;
      eventData.subId = m->getSubId();
      eventData.switch_type = m->getSwitchType();
      eventData.isTempDDS = m->getIsTempDDSSwitched();
      eventData.permanentDDS = m->getPermanentDDSSub();
      eventData.isDataAllowedOnNDDS = m->getIsDataAllowed();
      ipcEvent.data = &eventData;
      switch (m->getState()) {
         case DDSSwitchIPCMessageState::ApStarted:
           ipcEvent.event = DDSSwitchApStarted;
           preferred_data_sm->processEvent(ipcEvent);
           break;
         case DDSSwitchIPCMessageState::Completed:
           ipcEvent.event = DDSSwitchCompleted;
           preferred_data_sm->processEvent(ipcEvent);
           break;
         case DDSSwitchIPCMessageState::MpStarted:
           ipcEvent.event = DDSSwitchMpStarted;
           preferred_data_sm->processEvent(ipcEvent);
           break;
         case DDSSwitchIPCMessageState::SetDataAllowedOnNDDS:
           ipcEvent.event = setIsDataAllowedOnNDDS;
           preferred_data_sm->processEvent(ipcEvent);
           break;
         default:
           Log::getInstance().d("Invalid DDSSwitchIPCMessage state = " + std::to_string(static_cast<int>(m->getState())));
           break;
      }
    } else {
      Log::getInstance().d("Ignoring IPC message on self");
    }
  }
}

/*===========================================================================

  FUNCTION:  handleIAInfoIPCMessage

===========================================================================*/
/*!
    @brief

    @return
*/
/*=========================================================================*/
void DataModule::handleIAInfoIPCMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<IAInfoIPCMessage> m = std::static_pointer_cast<IAInfoIPCMessage>(msg);
  if (m != NULL) {
    if(preferred_data_state_info != nullptr)
    {
      //Process IACompleted and IAStartRequest on master ril
      PreferredDataEventType ipcEvent;
      if(preferred_data_state_info->isRilIpcNotifier)
      {
        switch(m->getState()) {
          case IAInfoIPCMessageState::IACompletedOnSlaveRIL:
          {
            ipcEvent.event = IACompletedOnSlaveRIL;
            preferred_data_sm->processEvent(ipcEvent);
            break;
          }

          case IAInfoIPCMessageState::IAStartRequest:
          {
            ipcEvent.event = IAStartRequest;
            preferred_data_sm->processEvent(ipcEvent);
            break;
          }
          case IAInfoIPCMessageState::GetCurrentStateResponse:
          {
            ipcEvent.event = GetCurrentStateResponse;
            DDSSwitchInfo_t info;
            if(m->getPreferredDataState() == IAInfoPreferredDataStates::IAHandlingState)
              info.slaveRILState = PreferredDataStateEnum::IAHandling;
            else if(m->getPreferredDataState() == IAInfoPreferredDataStates::InitialState)
              info.slaveRILState = PreferredDataStateEnum::Initial;
            else if(m->getPreferredDataState() == IAInfoPreferredDataStates::InitializedState)
              info.slaveRILState = PreferredDataStateEnum::Initialized;
            else if(m->getPreferredDataState() == IAInfoPreferredDataStates::ApTriggeredState)
              info.slaveRILState = PreferredDataStateEnum::ApTriggered;
            else if(m->getPreferredDataState() == IAInfoPreferredDataStates::MpTriggeredState)
              info.slaveRILState = PreferredDataStateEnum::MpTriggered;

            ipcEvent.data = &info;
            preferred_data_sm->processEvent(ipcEvent);
            break;
          }
          #ifdef QMI_RIL_UTF
          case IAInfoIPCMessageState::IAStartResponse:
          {
            if((preferred_data_state_info->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL)) > 0) {
              TimeKeeper::getInstance().clear_timer(preferred_data_state_info->iaTimer);
              if(preferred_data_state_info->mPreferredDataMsgList->getNextMessageFromList() != nullptr)
              {
                std::shared_ptr<SetInitialAttachApnRequestMessage> msg =
                    std::static_pointer_cast<SetInitialAttachApnRequestMessage>
                    (preferred_data_state_info->mPreferredDataMsgList->getNextMessageFromList()->msg);
                if(msg != nullptr)
                {
                  preferred_data_state_info->isRilIpcNotifier = FALSE;
                  initialAttachHandler(msg);
                }
              }
            }
          }
          #endif
          default:
          break;
        }
      } else {
          switch(m->getState()) {
            case IAInfoIPCMessageState::IAStartResponse:
            {
              if((preferred_data_state_info->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL)) > 0) {
                //handle IAStartResponse on slaveRil
                TimeKeeper::getInstance().clear_timer(preferred_data_state_info->iaTimer);
                if(preferred_data_state_info->mPreferredDataMsgList->getNextMessageFromList() != nullptr)
                {
                  std::shared_ptr<SetInitialAttachApnRequestMessage> msg =
                  std::static_pointer_cast<SetInitialAttachApnRequestMessage>
                  (preferred_data_state_info->mPreferredDataMsgList->getNextMessageFromList()->msg);
                  if(msg != nullptr)
                  {
                    initialAttachHandler(msg);
                  }
                }
              }
              break;
            }
            case IAInfoIPCMessageState::GetCurrentStateRequest:
            {
              PreferredDataEventType ipcEvent;
              ipcEvent.event = GetCurrentStateRequest;
              preferred_data_sm->processEvent(ipcEvent);
              break;
            }
            default:
              break;
          }
        }
    } else {
      Log::getInstance().d("Ignoring IPC message on self");
    }
  }
}
/*===========================================================================

  FUNCTION:  constructDDSSwitchIPCMessage

===========================================================================*/
/*!
    @brief

    @return
*/
/*=========================================================================*/
std::shared_ptr<IPCMessage> DataModule::constructDDSSwitchIPCMessage(IPCIStream& is) {
  // The message will have its members initialized in deserialize
  std::shared_ptr<DDSSwitchIPCMessage> ipcMessage = std::make_shared<DDSSwitchIPCMessage>(
     DDSSwitchIPCMessageState::ApStarted, -1, DSD_DDS_SWITCH_PERMANENT_V01, false, -1, false);
  if (ipcMessage != nullptr) {
    ipcMessage->deserialize(is);
  } else {
    Log::getInstance().d("[DataModule] Invalid DDSSwitchIPCMessage");
  }
  return ipcMessage;
}

/*===========================================================================

  FUNCTION:  constructIAInfoIPCMessage

===========================================================================*/
/*!
    @brief

    @return
*/
/*=========================================================================*/
std::shared_ptr<IPCMessage> DataModule::constructIAInfoIPCMessage(IPCIStream& is) {
  // The message will have its members initialized in deserialize
  std::shared_ptr<IAInfoIPCMessage> ipcMessage = std::make_shared<IAInfoIPCMessage>(
     IAInfoIPCMessageState::IAStartRequest);
  if (ipcMessage != nullptr) {
    ipcMessage->deserialize(is);
  } else {
    Log::getInstance().d("[DataModule] Invalid IAInfoIPCMessage");
  }
  return ipcMessage;
}

/*===========================================================================

  FUNCTION:  map_internalerr_from_reqlist_new_to_ril_err

===========================================================================*/
/*!
    @brief
    Helper API to convert data of IxErrnoType type to RIL_Errno type
    input: data of IxErrnoType type

    @return
*/
/*=========================================================================*/
RIL_Errno DataModule::map_internalerr_from_reqlist_new_to_ril_err(IxErrnoType error) {
  RIL_Errno ret;
  switch (error) {
     case E_SUCCESS:
       ret = RIL_E_SUCCESS;
       break;
     case E_NOT_ALLOWED:
       ret = RIL_E_INVALID_STATE; //needs to be changed after internal discussion
       break;
     case E_NO_MEMORY:
       ret = RIL_E_NO_MEMORY;
       break;
     case E_NO_RESOURCES:
       ret = RIL_E_NO_RESOURCES;
       break;
     case E_RADIO_NOT_AVAILABLE:
       ret = RIL_E_RADIO_NOT_AVAILABLE;
       break;
     case E_INVALID_ARG:
       ret = RIL_E_INVALID_ARGUMENTS;
       break;
     default:
       ret = RIL_E_INTERNAL_ERR;
       break;
  }
  return ret;
}

void DataModule::handleSetupDataCallRequestMessage(std::shared_ptr<Message> msg, DataCallVersion_t reqVersion) {
  std::shared_ptr<SetupDataCallRequestBase> m =
    std::static_pointer_cast<SetupDataCallRequestBase>(msg);
  if (m != nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + m->dump());
    std::vector<std::string> prints;
    if(reqVersion == DataCallVersion_t::RADIO_1_6)
    {
      std::shared_ptr<SetupDataCallRequestMessage_1_6> setup16Msg =
           std::static_pointer_cast<SetupDataCallRequestMessage_1_6>(msg);
      if(setup16Msg != nullptr) {
        prints = setup16Msg->dumpLine();
      }
    }
    else
    {
      std::string logMsg = m->dump();
      size_t pos = 0;
      while (pos < logMsg.length()) {
        size_t logLen = min(logMsg.length() - pos, (size_t)200);
        prints.push_back(logMsg.substr(pos, logLen));
        pos += logLen;
      }
    }
    for (auto s : prints) {
        Log::getInstance().d(s);
    }
    logBuffer.addLogWithTimestamp("[" + mName + "]: Handling msg = " + m->dump());
    #ifndef RIL_FOR_MDM_LE
    if (mSecureMode != STATE_NONSECURE) {
      if ((uint32_t)(m->getSupportedApnTypesBitmap()) & (uint32_t)(ApnTypes_t::EMERGENCY)) {
        Log::getInstance().d("[" + mName + "]: Allowing Emergency Call");
      } else {
        Log::getInstance().d("[" + mName + "]: Call is not Allowed");
        SetupDataCallResponse_t result;
        result.respErr = ResponseError_t::INTERNAL_ERROR;
        result.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
        result.call.suggestedRetryTime = -1;
        result.call.cid = -1;
        auto resp = std::make_shared<SetupDataCallResponse_t>(result);
        m->sendResponse(m, Message::Callback::Status::SUCCESS, resp);
        return;
      }
    }
    #endif
    if (call_manager != nullptr && !mIsPdcRefreshInProgress) {
      // if setupDataCall request message was previously handled from a different version, fail it
      if (call_manager->getDataCallVersion() != reqVersion &&
          call_manager->getDataCallVersion() != DataCallVersion_t::UNDETERMINED) {
        Log::getInstance().d("SetupDataCall request 1.4 and 1.6 cannot be mixed");
        SetupDataCallResponse_t result;
        result.respErr = ResponseError_t::INTERNAL_ERROR;
        result.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
        result.call.suggestedRetryTime = -1;
        auto resp = std::make_shared<SetupDataCallResponse_t>(result);
        m->sendResponse(m, Message::Callback::Status::SUCCESS, resp);
        return;
      }
      call_manager->setDataCallVersion(reqVersion);
      Dispatcher::getInstance().clearTimeoutForMessage(m);
      call_manager->handleCallEventMessage(CallEventTypeEnum::SetupDataCall, m);
    } else {
      Log::getInstance().d("call_manager is null");
      SetupDataCallResponse_t result;
      result.respErr = ResponseError_t::INTERNAL_ERROR;
      result.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
      result.call.suggestedRetryTime = -1;
      auto resp = std::make_shared<SetupDataCallResponse_t>(result);
      m->sendResponse(m, Message::Callback::Status::SUCCESS, resp);
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received");
  }
}

void DataModule::handleSetupDataCallRequestMessage_1_4(std::shared_ptr<Message> msg) {
  handleSetupDataCallRequestMessage(msg, DataCallVersion_t::RADIO_1_4);
}

void DataModule::handleSetupDataCallRequestMessage_1_6(std::shared_ptr<Message> msg) {
  handleSetupDataCallRequestMessage(msg, DataCallVersion_t::RADIO_1_6);
}

void DataModule::handleDeactivateDataCallRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  logBuffer.addLogWithTimestamp("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<DeactivateDataCallRequestMessage> m = std::static_pointer_cast<DeactivateDataCallRequestMessage>(msg);
  if (m != NULL) {
    if (call_manager != nullptr) {
      Dispatcher::getInstance().clearTimeoutForMessage(m);
      call_manager->handleCallEventMessage(CallEventTypeEnum::DeactivateDataCall, msg);
    } else {
      Log::getInstance().d("call_manager is null");
      ResponseError_t result = ResponseError_t::INTERNAL_ERROR;
      auto resp = std::make_shared<ResponseError_t>(result);
      m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleGetRadioDataCallListRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  std::shared_ptr<GetRadioDataCallListRequestMessage> m = std::static_pointer_cast<GetRadioDataCallListRequestMessage>(msg);
  if (m != NULL) {
    Message::Callback::Status status = Message::Callback::Status::SUCCESS;
    DataCallListResult_t result = {};
    result.respErr = ResponseError_t::NO_ERROR;

    if (call_manager) {
      call_manager->getRadioDataCallList(result.call);
    } else {
      Log::getInstance().d("call_manager is null");
      result.respErr = ResponseError_t::INTERNAL_ERROR;
    }
    auto resp = std::make_shared<DataCallListResult_t>(result);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleDataCallTimerExpiredMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  logBuffer.addLogWithTimestamp("[" + mName + "]: Handling msg = " + msg->dump());
  if (call_manager != nullptr) {
    call_manager->handleCallEventMessage(CallEventTypeEnum::TimerExpired, msg);
  } else {
    Log::getInstance().d("No call_manager created");
  }
}

void DataModule::handleLinkPropertiesChangedMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (call_manager) {
    call_manager->handleCallEventMessage(CallEventTypeEnum::LinkPropertiesChanged, msg);
  }
  else {
    Log::getInstance().d("call_manager is null");
  }
}

void DataModule::handleSetCarrierInfoImsiEncryptionMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<SetCarrierInfoImsiEncryptionMessage> m = std::static_pointer_cast<SetCarrierInfoImsiEncryptionMessage>(msg);
  if ( m == NULL ) {
    Log::getInstance().d("Msg is null");
    return;
  }
  RIL_Errno ret = RIL_E_INTERNAL_ERR;
  if (auth_manager) {
    auto rc = auth_manager->setCarrierInfoImsiEncryption(m->getImsiEncryptionInfo());
    if( rc )
      ret = RIL_E_SUCCESS;
  } else {
    Log::getInstance().d("auth_manager is null");
  }
  auto resp = std::make_shared<RIL_Errno>(ret);
  m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
}

void DataModule::handleRegistrationFailureReportingStatusMessage(std::shared_ptr<Message> msg) {
  if (msg != nullptr) {
      std::shared_ptr<RegistrationFailureReportingStatusMessage> m = std::static_pointer_cast<RegistrationFailureReportingStatusMessage>(msg);
      Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
      if (m != nullptr) {
        mRegistrationFailureCauseReport = m->getReportingStatus();
        if (wds_endpoint) {
          wds_endpoint->registerDataRegistrationRejectCause(mRegistrationFailureCauseReport);
        }
        else {
          Log::getInstance().d("wds_endpoint is null");
        }
        if (call_manager) {
          call_manager->registerDataRegistrationRejectCause(mRegistrationFailureCauseReport);
        }
        else {
          Log::getInstance().d("call_manager is null");
        }
      }
      else {
        Log::getInstance().d("[" + mName + "]: Invalid msg received");
      }
  }
}

void DataModule::checkProfileUpdateAllowed(std::string mcc, std::string mnc)
{
  // property is already set, no need to check mcc/mnc
  if (profileUpdatePropertySet) {
    return;
  }

  // update Util if the profileUpdateAllowed changed based on mcc/mnc
  bool newFlag = isProfileUpdateCarrierSet(mcc, mnc);
  if (profileUpdateCarrierSet != newFlag) {
    profileUpdateCarrierSet = newFlag;
    util.setProfileUpdateAllowed(profileUpdateCarrierSet);
    if (profile_handler != nullptr) {
      profile_handler->setUpdateAllowed(profileUpdateCarrierSet);
    }
  }
}

void DataModule::handleDsiInitCompletedMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<DsiInitCompletedMessage> m = std::static_pointer_cast<DsiInitCompletedMessage>(msg);
  if( (m == NULL) || (call_manager == NULL))
  {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump() + "or Callmanager is NULL");
    return;
  }
  if(call_manager->dsiInitStatus  == DsiInitStatus_t::PENDING_RELEASE)
  {
    call_manager->triggerDsiRelease();
    if( (mInitTracker.getDsdServiceReady() ||
              mInitTracker.getWdsServiceReady()) )
    {
      call_manager->triggerDsiInit(!mInitCompleted);
    }
  }
  else
  {
    call_manager->dsiInitCompleted();
    Log::getInstance().d("DSI init is completed");
  }
}

void DataModule::handleNasRfBandInfoIndMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (call_manager != nullptr) {
    call_manager->updateNasRfActiveBandInfo(msg);
  } else {
    Log::getInstance().d("No call_manager created");
  }
}

void DataModule::handleNasRfBandInfoMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (call_manager != nullptr) {
    call_manager->handleNasRfBandInfoMessage(msg);
  } else {
    Log::getInstance().d("No call_manager created");
  }
}

void DataModule::handleQmiTmdEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
    if (tmd_manager != nullptr) {
      tmd_manager->getMitigationDeviceList();
    } else {
      Log::getInstance().d("No tmd_manager created");
    }
    Log::getInstance().d("[" + mName + "]: TMDModemEndPoint is operational");
  }
}


#ifndef RIL_FOR_MDM_LE
void DataModule::handleQmiVoiceEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  if (shared_indMsg->getState() == ModemEndPoint::State::NON_OPERATIONAL) {
    Log::getInstance().d("[" + mName + "]: Resetting switch type to Permanent as modem is down");
    preferred_data_state_info->switch_type = DSD_DDS_SWITCH_PERMANENT_V01;
    //resetting the timer
    if (voiceCallEndPointSub0 != nullptr && voiceCallEndPointSub0->mIsVoiceCallOrigTimer) {
      voiceCallEndPointSub0->mIsVoiceCallOrigTimer = FALSE;
      TimeKeeper::getInstance().clear_timer(voiceCallEndPointSub0->mVoiceCallOrigTimer);
    } else if (voiceCallEndPointSub1 != nullptr && voiceCallEndPointSub1->mIsVoiceCallOrigTimer) {
      voiceCallEndPointSub1->mIsVoiceCallOrigTimer = FALSE;
      TimeKeeper::getInstance().clear_timer(voiceCallEndPointSub1->mVoiceCallOrigTimer);
    }
    if (TempddsSwitchRequestPending) {
      TimeKeeper::getInstance().clear_timer(tempDDSSwitchRequestTimer);
    }
  }
}

void DataModule::handleVoiceIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<VoiceIndMessage> m = std::static_pointer_cast<VoiceIndMessage>(msg);
  if(m != NULL && preferred_data_state_info != nullptr)
  {

    std::vector<VoiceIndResp> voiceInfo = m->getParam();

    /*If there are multiple calls then set the switchtype to tempdds
      when there is at least one call which is not in call_end state*/
    for(auto it = voiceInfo.begin(); it!= voiceInfo.end(); it++)
    {
      if((*it).voiceCallType == VoiceCallTypeEnum::CALL_TYPE_EMERGENCY && !mSimPresent)
      {
        preferred_data_state_info->switch_type = DSD_DDS_SWITCH_PERMANENT_V01;
        Log::getInstance().d("[" + mName + "]: Received voice ind for SIMLess emergency call");
        return;
      }

      if(((*it).voiceSubId) == SubId::PRIMARY && voiceCallEndPointSub0 != nullptr && voiceCallEndPointSub0->mIsVoiceCallOrigTimer)
      {
        voiceCallEndPointSub0->mIsVoiceCallOrigTimer = FALSE;
        TimeKeeper::getInstance().clear_timer(voiceCallEndPointSub0->mVoiceCallOrigTimer);
      }
      else if(((*it).voiceSubId) == SubId::SECONDARY && voiceCallEndPointSub1 != nullptr && voiceCallEndPointSub1->mIsVoiceCallOrigTimer)
      {
        voiceCallEndPointSub1->mIsVoiceCallOrigTimer = FALSE;
        TimeKeeper::getInstance().clear_timer(voiceCallEndPointSub1->mVoiceCallOrigTimer);
      }

      if((*it).voiceCallState != VoiceCallStateEnum::CALL_STATE_END &&
          (preferred_data_state_info->switch_type == DSD_DDS_SWITCH_TEMPORARY_V01 ||
          ((int)((*it).voiceSubId) != preferred_data_state_info->dds &&
           (((*it).voiceSubId == SubId::PRIMARY && !(voiceCallEndPointSub1->isVoiceCallInActiveState())) ||
           ((*it).voiceSubId == SubId::SECONDARY && !(voiceCallEndPointSub0->isVoiceCallInActiveState()))))))
      {
        preferred_data_state_info->switch_type = DSD_DDS_SWITCH_TEMPORARY_V01;
        Log::getInstance().d("SwitchType is set to Temporary");

        if((*it).voiceCallState == VoiceCallStateEnum::CALL_STATE_CC_IN_PROGRESS)
        {
          //set the voiceCallOrigTimer
          if((*it).voiceSubId == SubId::PRIMARY)
          {
            voiceCallEndPointSub0->mIsVoiceCallOrigTimer = TRUE;
            voiceCallEndPointSub0->mVoiceCallOrigTimer = TimeKeeper::getInstance().set_timer(std::bind(&DataModule::handleVoiceCallOrigTimerExpired, this, std::placeholders::_1), nullptr, VOICEIND_WAITING_TIMEOUT);
          }
          else
          {
            voiceCallEndPointSub1->mIsVoiceCallOrigTimer = TRUE;
            voiceCallEndPointSub1->mVoiceCallOrigTimer = TimeKeeper::getInstance().set_timer(std::bind(&DataModule::handleVoiceCallOrigTimerExpired, this, std::placeholders::_1), nullptr, VOICEIND_WAITING_TIMEOUT);
          }
        }
        preferred_data_state_info->mVoiceCallInfo.voiceSubId = (*it).voiceSubId;
        preferred_data_state_info->mVoiceCallInfo.voiceCallState = (*it).voiceCallState;
        break;
      }
      if(std::next(it) == voiceInfo.end() && preferred_data_state_info->switch_type == DSD_DDS_SWITCH_TEMPORARY_V01)
      {
        //not setting switch_type to permanent if there is a call on another dds sub with call_state_hold
        if(((*it).voiceCallState == VoiceCallStateEnum::CALL_STATE_END) &&
           (((*it).voiceSubId == SubId::PRIMARY && (voiceCallEndPointSub1->isVoiceCallInActiveState())) ||
            ((*it).voiceSubId == SubId::SECONDARY && (voiceCallEndPointSub0->isVoiceCallInActiveState()))))
        {
          Log::getInstance().d("Active call on another sub");
          return;
        }
        /*If no voicecall found on nondds sub with callstate != call_end, set the switch type to permanent*/
        preferred_data_state_info->switch_type = DSD_DDS_SWITCH_PERMANENT_V01;
        Log::getInstance().d("SwitchType is set to permanent");

        if((*it).voiceCallState == VoiceCallStateEnum::CALL_STATE_END && ((int)((*it).voiceSubId) == preferred_data_state_info->dds))
        {
          TempddsSwitchRequestPending = true;
          tempDDSSwitchRequestTimer = TimeKeeper::getInstance().set_timer(std::bind(&DataModule::handleTempDDSSwitchTimerExpired, this, std::placeholders::_1), nullptr, TempDDS_SWITCH_REQUEST_TIMEOUT);
        }
      }
    }
  }
}

void DataModule::handleTempDDSSwitchTimerExpired(void *) {
  Log::getInstance().d("Datamodule: onTempDDSSwitchRequestExpired ENTRY");
  DDSTimeOutSwitchType type = DDSTimeOutSwitchType::TempDDSTimeOutSwitch;

  if(preferred_data_state_info->currentDDSSwitchType == DSD_DDS_SWITCH_TEMPORARY_V01) {
    auto msg = std::make_shared<DDSSwitchTimeoutMessage>(type);
    msg->broadcast();
  }
}

void DataModule::handleVoiceCallOrigTimerExpired(void *) {
  Log::getInstance().d("DataModule: handleVoiceCallOrigTimerExpired");
  auto msg = std::make_shared<VoiceCallOrigTimeoutMessage>();
  msg->broadcast();
}

void DataModule::handleVoiceCallOrigTimeoutMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("DataModule: handleVoiceCallOrigTimeoutMessage");
  std::shared_ptr<VoiceCallOrigTimeoutMessage> m = std::static_pointer_cast<VoiceCallOrigTimeoutMessage>(msg);
  if( m != nullptr)
  {
    bool isCallActive = false;
    if(preferred_data_state_info->mVoiceCallInfo.voiceSubId == SubId::PRIMARY)
    {
      voiceCallEndPointSub0->mIsVoiceCallOrigTimer = FALSE;
      isCallActive = voiceCallEndPointSub0->isVoiceCall();
      if(!isCallActive || preferred_data_state_info->mVoiceCallInfo.voiceCallState == VoiceCallStateEnum::CALL_STATE_CC_IN_PROGRESS)
      {
        preferred_data_state_info->switch_type = DSD_DDS_SWITCH_PERMANENT_V01;
      }
    } else {
      voiceCallEndPointSub1->mIsVoiceCallOrigTimer = FALSE;
      isCallActive = voiceCallEndPointSub1->isVoiceCall();
      if(!isCallActive || preferred_data_state_info->mVoiceCallInfo.voiceCallState == VoiceCallStateEnum::CALL_STATE_CC_IN_PROGRESS)
      {
        preferred_data_state_info->switch_type = DSD_DDS_SWITCH_PERMANENT_V01;
      }
    }
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSecureModeChangedCallBackMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<SecureModeChangedCallBackMessage> m = std::static_pointer_cast<SecureModeChangedCallBackMessage>(msg);
  if (m != nullptr) {
    auto state = m->getState();
    if (state == STATE_RESET_CONNECTION) {
      mSecureModeContext = nullptr;
      auto registerMsg = std::make_shared<RegisterForSecureModeCbMessage>();
      if (registerMsg) {
        registerMsg->broadcast();
      }
      return;
    }
    if (mSecureMode != state) {
      mSecureMode = state;
      Log::getInstance().d("[" + mName + "]: Peripheral Status Changed to :" + std::to_string(mSecureMode));
      if (mSecureMode == STATE_SECURE) {
        if (call_manager) {
          call_manager->cleanCallsInSecureMode();
        }
      }
    }
  }
}

int32_t DataModule::SecureModeNotifyEvent(const uint32_t peripheral, const uint8_t state)
{
  Log::getInstance().d("[DataModule]: Received Peripheral Notification from TZ");
  Log::getInstance().d("[DataModule]: Peripheral :" + std::to_string(peripheral));
  Log::getInstance().d("[DataModule]: State :" + std::to_string(state));
  if (peripheral != CPeripheralAccessControl_MODEM_UID) {
    Log::getInstance().d("[DataModule]: Only interested in Modem Peripheral Indication");
    return 0;
  }
  std::shared_ptr<SecureModeChangedCallBackMessage> msg = std::make_shared<SecureModeChangedCallBackMessage>(state);
  if (msg) {
    msg->broadcast();
  }
  return 0;
}

void DataModule::handleRegisterForSecureModeCbMessage(std::shared_ptr<Message> msg)
{
  static int counter = 0;
  const char *dlsym_error = dlerror();
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<RegisterForSecureModeCbMessage> m = std::static_pointer_cast<RegisterForSecureModeCbMessage>(msg);
  if (m != nullptr) {
    mSecureModeTimer = TimeKeeper::no_timer;
    counter ++;
    if (counter > 15) {
      Log::getInstance().d("Unable to Register for 15 times. Setting Mode to NonSecure");
      mSecureMode = STATE_NONSECURE;
      return;
    }
    Log::getInstance().d("Retry Attempt :" + std::to_string(counter));
    if (!mRegisterPeriferalCb) {
      mRegisterPeriferalCb = (registerModemPeripheralCBFnPtr) dlsym (mSecureLibInstance, "registerPeripheralCB");
      dlsym_error = dlerror();
      if( dlsym_error ) {
        Log::getInstance().d("[" + mName + "]: Cannot find registerPeripheralCB symbol");
        return;
      }
    }
    mSecureModeContext = mRegisterPeriferalCb (CPeripheralAccessControl_MODEM_UID,
                                               DataModule::SecureModeNotifyEvent);
    if (mSecureModeContext == NULL) {
      Log::getInstance().d("[" + mName + "]: Registering for Modem Peripheral failed. Adding retry timer");
      mSecureModeTimer = TimeKeeper::getInstance().set_timer(
        std::bind(&DataModule::SecureModeTimeOutHandler, this, std::placeholders::_1),
        nullptr,
        SECURE_MODE_REGISTRATION_RETRY_TIMEOUT);
      return;
    }
    Log::getInstance().d("[" + mName + "]: Secure Peripheral for Modem is registered");
    /*If Registration is successful counter need to be reset and timer need to be killed*/
    counter = 0;
    if (mSecureModeTimer != TimeKeeper::no_timer) {
      TimeKeeper::getInstance().clear_timer(mSecureModeTimer);
      mSecureModeTimer = TimeKeeper::no_timer;
    }
    if (!mGetPeripheralState) {
      mGetPeripheralState = (getModemPeripheralStatusFnPtr) dlsym (mSecureLibInstance, "getPeripheralState");
      dlsym_error = dlerror();
      if( dlsym_error ) {
        Log::getInstance().d("[" + mName + "]: Cannot find getPeripheralState symbol");
        return;
      }
    }
    auto state = mGetPeripheralState(mSecureModeContext);
    if (state == PRPHRL_ERROR) {
      Log::getInstance().d("[" + mName + "]: Get Peripheral Failed. Resetting state to Non Secure");
      mSecureMode = STATE_NONSECURE;
      return;
    }
    if (state == STATE_RESET_CONNECTION) {
      mSecureModeContext = nullptr;
      auto registerMsg = std::make_shared<RegisterForSecureModeCbMessage>();
      if (registerMsg) {
        registerMsg->broadcast();
      }
      return;
    }
    if (mSecureMode != state) {
      mSecureMode = state;
      Log::getInstance().d("[" + mName + "]: Peripheral Status Changed to :" + std::to_string(mSecureMode));
      if (mSecureMode == STATE_SECURE) {
        if (call_manager) {
          call_manager->cleanCallsInSecureMode();
        }
      }
    }
  }
}

void DataModule::SecureModeTimeOutHandler(void* data) {
  std::ignore = data;
  Log::getInstance().d("[" + mName + "]: SecureModeTimeOutHandler");
  auto registerMsg = std::make_shared<RegisterForSecureModeCbMessage>();
  if (registerMsg) {
    registerMsg->broadcast();
  }
}

void DataModule::registerForSecureModeIndications() {
  Log::getInstance().d("[" + mName + "]: registerForSecureModeIndications");
  mSecureLibInstance = dlopen("libPeripheralStateUtils.so", RTLD_LAZY);
  if( NULL == mSecureLibInstance ) {
    Log::getInstance().d("[" + mName + "]: Unable to load securelib instance");
    return;
  }
  const char *dlsym_error = dlerror();
  mRegisterPeriferalCb = (registerModemPeripheralCBFnPtr) dlsym (mSecureLibInstance, "registerPeripheralCB");
  dlsym_error = dlerror();
  if( dlsym_error ) {
    Log::getInstance().d("[" + mName + "]: Cannot find registerPeripheralCB symbol");
    return;
  }
  mSecureModeContext = mRegisterPeriferalCb (CPeripheralAccessControl_MODEM_UID,
                                             DataModule::SecureModeNotifyEvent);
  if (mSecureModeContext == NULL) {
    Log::getInstance().d("[" + mName + "]: Registering for Modem Peripheral failed. Adding retry timer");
    mSecureModeTimer = TimeKeeper::getInstance().set_timer(
        std::bind(&DataModule::SecureModeTimeOutHandler, this, std::placeholders::_1),
        nullptr,
        SECURE_MODE_REGISTRATION_RETRY_TIMEOUT);
    return;
  }
  Log::getInstance().d("[" + mName + "]: Secure Peripheral for Modem is registered");
  mGetPeripheralState = (getModemPeripheralStatusFnPtr) dlsym (mSecureLibInstance, "getPeripheralState");
  dlsym_error = dlerror();
  if( dlsym_error ) {
    Log::getInstance().d("[" + mName + "]: Cannot find getPeripheralState symbol");
    return;
  }
  auto state = mGetPeripheralState(mSecureModeContext);
  if (state == PRPHRL_ERROR) {
    Log::getInstance().d("[" + mName + "]: Get Peripheral Failed. Resetting state to Non Secure");
    mSecureMode = STATE_NONSECURE;
    return;
  }
  if (state == STATE_RESET_CONNECTION) {
    mSecureModeContext = nullptr;
    auto registerMsg = std::make_shared<RegisterForSecureModeCbMessage>();
    if (registerMsg) {
      registerMsg->broadcast();
    }
    return;
  }
  if (mSecureMode != state) {
    mSecureMode = state;
    Log::getInstance().d("[" + mName + "]: Peripheral Status Changed to :" + std::to_string(mSecureMode));
    if (mSecureMode == STATE_SECURE) {
      if (call_manager) {
        call_manager->cleanCallsInSecureMode();
      }
    }
  }
}

void DataModule::deregisterForSecureModeIndications() {
  if (!mSecureLibInstance) {
    return;
  }
  Log::getInstance().d("[" + mName + "]: deregisterForSecureModeIndications");
  dlerror();
  const char *dlsym_error = dlerror();
  mDeregisterPeriferalCb = (deregisterModemPeripheralCBFnPtr) dlsym (mSecureLibInstance, "deregisterPeripheralCB");
  if( dlsym_error ) {
    Log::getInstance().d("[" + mName + "]: Cannot find deregisterPeripheralCB symbol");
  }
  if (mDeregisterPeriferalCb && mSecureModeContext) {
    mDeregisterPeriferalCb(mSecureModeContext);
  }
  mGetPeripheralState = nullptr;
  mRegisterPeriferalCb = nullptr;
  mDeregisterPeriferalCb = nullptr;
  dlclose(mSecureLibInstance);
  mSecureLibInstance = nullptr;
}

void DataModule::handleSetDataInActivityMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg != nullptr && call_manager != nullptr) {
    call_manager->processSetDataInactivityPeriod(msg);
  } else {
    Log::getInstance().d("No call_manager created");
  }
}

void DataModule::handleTcpMonitoringReqMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  auto m = std::static_pointer_cast<TcpMonitoringRequestMessage>(msg);
  KeepAliveEventType eve;
  eve.msg = msg;
  if(m->isStartRequest()) {
    eve.event = KeepAliveStartTcpMonitoringReq;
  }
  else {
    eve.event = KeepAliveStopTcpMonitoringReq;
  }
  keep_alive->processEvent(eve);
}

void DataModule::handleCleanUpTcpKeepAliveMessage(std::shared_ptr<Message> msg)
{
  auto m = std::static_pointer_cast<CleanUpTcpKeepAliveMessage>(msg);
  if(keep_alive) {
    KeepAliveEventType eve;
    eve.event = KeepAliveCleanUpForClientId;
    int client_id = m->getClientId();
    eve.data = &client_id;
    keep_alive->processEvent(eve);
  }
}

void DataModule::handleSetDataThrottlingRequestMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<SetDataThrottlingRequestMessage> m = std::static_pointer_cast<SetDataThrottlingRequestMessage>(msg);

  if (m != nullptr) {
    RIL_Errno err = RIL_E_GENERIC_FAILURE;
    if (tmd_manager != nullptr) {
      tmd_manager->setDataThrottling(m->getDataThrottleAction(), m->getCompleteDurationMills());
      err = RIL_E_SUCCESS;
    }
    auto resp = std::make_shared<RIL_Errno>(err);
    m->sendResponse(m, Message::Callback::Status::SUCCESS, resp);
  }
}

bool DataModule::loadDataQos() {
  bool ret = false;
#ifdef QMI_RIL_UTF
  return true;
#endif
  do {
    dataQosLibInstance = dlopen("libqcrildataqos.so", RTLD_LAZY);
    if( NULL == dataQosLibInstance ) {
      Log::getInstance().d("[" + mName + "]: Unable to load libqcrildataqos.so");
      break;
    }
    dlerror();
    mDataQosInit = (dataQosInitFnPtr)dlsym(dataQosLibInstance, "dataQosInit");
    const char *dlsym_error = dlerror();
    if( dlsym_error ) {
      Log::getInstance().d("[" + mName + "]: Cannot find dataQosInit symbol");
      break;
    }
    mDataQosCleanup = (dataQosCleanUpFnPtr)dlsym(dataQosLibInstance, "dataQosCleanUp");
    dlsym_error = dlerror();
    if( dlsym_error ) {
      Log::getInstance().d("[" + mName + "]: Cannot find dataQosInit symbol");
      break;
    }
    mDataQosDeInit = (dataQosDeInitFnPtr)dlsym(dataQosLibInstance, "dataQosDeInit");
    dlsym_error = dlerror();
    if(dlsym_error) {
        Log::getInstance().d("["+ mName + "]: Cannot find dataQosDeInit symbol");
        break;
    }
    ret = true;
  }
  while( 0 );
  if( ret == false ) {
    unloadDataQos();
  }
  return ret;
}

void DataModule::unloadDataQos() {
#ifdef QMI_RIL_UTF
// @TODO: uncomment once UTF is ready
//   dataQosCleanUp();
   return;
#endif
  if(dataQosLibInstance != nullptr) {
    if(mDataQosDeInit != nullptr) {
      mDataQosDeInit();
    }
    mDataQosInit = nullptr;
    mDataQosCleanup = nullptr;
    mDataQosDeInit = nullptr;
    dlclose(dataQosLibInstance);
    dataQosLibInstance = nullptr;
  }
}

void DataModule::handleQosInitMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  auto m = std::static_pointer_cast<QosInitializeMessage>(msg);

  if(m->isCloseQos() && mDataQosCleanup != nullptr) {
    mDataQosCleanup();
    Log::getInstance().d("[" + mName + "]: dataQos cleanup triggered");
    return;
  }

  dataQosInitParams params;
  params.cid = m->getCid();
  params.muxId = m->getMuxId();
  params.eptype = m->getEpType();
  params.epid = m->getEpId();
  params.slot_id = global_instance_id;
#ifdef QMI_RIL_UTF
// @TODO: uncomment once UTF is ready
//   dataQosInit(&params, logFn);
  return;
#endif
  if(mDataQosInit != nullptr) {
    mDataQosInit(&params, logFn);
  } else {
    Log::getInstance().d("[" + mName + "]: loadDataQos not loaded or encountered error");
  }
}
#endif

void DataModule::handlePduSessionParamLookupResultIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (call_manager != nullptr) {
    call_manager->handlePduSessionParamLookupResultIndMessage(msg);
  } else {
    Log::getInstance().d("No call_manager created");
  }
}

void DataModule::handleRouteChangeUpdateMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RouteChangeUpdateMessage> m = std::static_pointer_cast<RouteChangeUpdateMessage>(msg);
  if (m != nullptr && call_manager != nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + m->dump());
    call_manager->processRouteChangeUpdate(m->getCid(), m->getParams());
  } else {
    Log::getInstance().d("No call_manager created");
  }
}

void DataModule::handleUpdateUIOptionMessage(std::shared_ptr<Message> m)
{
  auto msg= std::static_pointer_cast<UpdateUIOptionMessage>(m);
  Log::getInstance().d("[DataModule]: handleUpdateUIOptionMessage=" + std::to_string((int)msg->getCIWlanUIOption()));

  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  if (mInitTracker.isAPAssistMode()) {
    res = dsd_endpoint->setCIWlanUIOptionSelection(msg->getCIWlanUIOption());
  }
  auto resp = std::make_shared<RIL_Errno>(res);
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
}

#if (defined (QMI_RIL_UTF) || defined(RIL_FOR_MDM_LE))
void dump_data_module(int fd) {
  getDataModule().dump(fd);
  getDataModule().flush();
}
#endif

#ifdef QMI_RIL_UTF
void qcril_qmi_hal_data_module_cleanup() {
  getDataModule().cleanup();
}

void DataModule::cleanup()
{
  std::shared_ptr<DSDModemEndPoint> mDsdModemEndPoint =
      ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint();
  DSDModemEndPointModule* mDsdModemEndPointModule =
      (DSDModemEndPointModule*)mDsdModemEndPoint->mModule;
  mDsdModemEndPointModule->cleanUpQmiSvcClient();

  std::shared_ptr<AuthModemEndPoint> mAuthModemEndPoint =
      ModemEndPointFactory<AuthModemEndPoint>::getInstance().buildEndPoint();
  AuthModemEndPointModule* mAuthModemEndPointModule =
      (AuthModemEndPointModule*)mAuthModemEndPoint->mModule;
  mAuthModemEndPointModule->cleanUpQmiSvcClient();

  std::shared_ptr<OTTModemEndPoint> mOTTModemEndPoint =
      ModemEndPointFactory<OTTModemEndPoint>::getInstance().buildEndPoint();
  OTTModemEndPointModule* mOTTModemEndPointModule =
      (OTTModemEndPointModule*)mOTTModemEndPoint->mModule;
  mOTTModemEndPointModule->cleanUpQmiSvcClient();

  std::shared_ptr<WDSModemEndPoint> mWDSModemEndPoint =
      ModemEndPointFactory<WDSModemEndPoint>::getInstance().buildEndPoint();
  WDSModemEndPointModule* mWDSModemEndPointModule =
      (WDSModemEndPointModule*)mWDSModemEndPoint->mModule;
  mWDSModemEndPointModule->cleanUpQmiSvcClient();

  std::shared_ptr<PdcModemEndPoint> mPDCModemEndPoint =
      ModemEndPointFactory<PdcModemEndPoint>::getInstance().buildEndPoint();
  PdcModemEndPointModule* mPDCModemEndPointModule =
      (PdcModemEndPointModule*)mPDCModemEndPoint->mModule;
  mPDCModemEndPointModule->cleanUpQmiSvcClient();

  std::shared_ptr<TMDModemEndPoint> mTMDModemEndPoint =
      ModemEndPointFactory<TMDModemEndPoint>::getInstance().buildEndPoint();
  TMDModemEndPointModule* mTMDModemEndPointModule =
      (TMDModemEndPointModule*)mTMDModemEndPoint->mModule;
  mTMDModemEndPointModule->cleanUpQmiSvcClient();

  mInitTracker = InitTracker();
  mInitCompleted = false;
  mIsPdcRefreshInProgress = false;
  iwlanHandshakeMsgToken = INVALID_MSG_TOKEN;
  preferred_data_sm = std::make_unique<PreferredDataStateMachine>();
  preferred_data_state_info = std::make_shared<PreferredDataInfo_t>();
  preferred_data_state_info->isRilIpcNotifier = false;
  preferred_data_state_info->mVoiceCallInfo.voiceSubId = SubId::UNSPECIFIED;
  preferred_data_state_info->isDataAllowedOnNDDS = false;
  preferred_data_sm->initialize(preferred_data_state_info);
  preferred_data_sm->setCurrentState(Initial);
  #if !defined(RIL_FOR_LOW_RAM) || defined(RIL_FOR_MDM_LE)
  voiceCallEndPointSub0 = nullptr;
  voiceCallEndPointSub1 = nullptr;
  #endif
  keep_alive = std::make_shared<KeepAliveHandler>();
  auth_manager = nullptr;
  profile_handler = nullptr;
  call_manager = nullptr;
  network_service_handler = nullptr;
  memset(&mCachedSystemStatus, 0, sizeof(dsd_system_status_ind_msg_v01));
  iwlanHandshakeMsgToken = INVALID_MSG_TOKEN;
  getPendingMessageList().clear();
  mInitTracker.setIWLANMode(rildata::IWLANOperationMode::AP_ASSISTED);
  mCachedSystemStatus = {};
  qcril_data_set_pref_data_tech( global_instance_id, QCRIL_DATA_PREF_DATA_TECH_INVALID );
  global_subs_id = WDS_DEFAULT_SUBS_V01;
  global_instance_id = QCRIL_DEFAULT_INSTANCE_ID;
  dataMtu = 0;
  global_qcril_clat_supported = false;
  mSecureMode = STATE_NONSECURE;
#ifdef FEATURE_DATA_LQE
  lceHandler.Release();
#endif
}
#endif
}//namespace
