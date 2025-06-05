/**
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
* All rights reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DATAMODULE
#define DATAMODULE
#include "framework/PendingMessageList.h"
#include "module/IDataModule.h"
#include "DataCommon.h"
#include "AuthManager.h"
#include "LceHandler.h"
#include "ProfileHandler.h"
#include "NetworkServiceHandler.h"
#include "NetworkAvailabilityHandler.h"
#include "request/IWLANCapabilityHandshake.h"
#include "InitTracker.h"
#include "CallManager.h"
#include "PreferredDataStateMachine.h"
#include "VoiceCallModemEndPoint.h"
#include "modules/uim/UimCardStatusIndMsg.h"
#include "qtibus/IPCMessage.h"
#include "KeepAliveHandler.h"
#include "TmdManager.h"
#include "Util.h"

#ifndef RIL_FOR_MDM_LE
#include "DataCtlInterface.h"
#include "IPeripheralState.h"
#include "CPeripheralAccessControl.h"
#include "peripheralStateUtils.h"
#endif
#include "interfaces/uim/qcril_uim_types.h"
#ifdef FEATURE_DATA_LQE
  #include "OTTModemEndPoint.h"
#endif /* FEATURE_DATA_LQE */
#include "PdcModemEndPoint.h"
#include "TMDModemEndPoint.h"
#include "framework/TimeKeeper.h"

#ifdef RIL_DATA_UTF
extern "C" {
  void dataQosInit(void* params, void (*logFn)(std::string));
  void dataQosCleanUp();
  void dataQosDeInit();
}
#endif
namespace rildata {

struct dataQosInitParams {
  int cid;
  int muxId;
  int eptype;
  int epid;
  int slot_id;
};
typedef void (*dataQosInitFnPtr)(void*, void (*logFn)(std::string));
typedef void (*dataQosCleanUpFnPtr)();
typedef void (*dataQosDeInitFnPtr)();
typedef void* (*initDataCtlFnPtr)();

#ifndef RIL_FOR_MDM_LE
typedef int32_t (*getModemPeripheralStatusFnPtr)(void *context);
typedef void* (*registerModemPeripheralCBFnPtr)(uint32_t peripheral, PeripheralStateCB NotifyEvent);
typedef int32_t (*deregisterModemPeripheralCBFnPtr)(void *context);
#endif

class DataModule : public IDataModule {
 public:
  DataModule();
  ~DataModule();
  PendingMessageList& getPendingMessageList();
  void dump(int fd);
  void flush();
  void init();
#ifdef QMI_RIL_UTF
  virtual void cleanup();
#endif
  void setIsDataAllowed(bool isAllowed);

 private:
  #ifdef QMI_RIL_UTF
  static constexpr TimeKeeper::millisec TempDDS_SWITCH_REQUEST_TIMEOUT = 50;
  #else
  static constexpr TimeKeeper::millisec TempDDS_SWITCH_REQUEST_TIMEOUT = 1200;
  #endif
  LocalLogBuffer logBuffer{string("RIL"), 200};
  LocalLogBuffer networkAvailabilityLogBuffer{string("NAH"), 200};
  qdp::Util util;
  dsd_system_status_ind_msg_v01 mCachedSystemStatus = { };
  InitTracker mInitTracker;
  int32_t iwlanHandshakeMsgToken;
  bool mAuthServiceReady = false;
  PendingMessageList mMessageList;
  TimeKeeper::timer_id tempDDSSwitchRequestTimer;
  bool TempddsSwitchRequestPending = false;
  bool mLinkStatusReportEnabled = false;
  bool mLimitedIndReportEnabled = false;
  bool mInitCompleted = false;
  bool mRegistrationFailureCauseReport = false;
  bool mSimPresent = false;
  bool profileUpdatePropertySet = false;
  bool profileUpdateCarrierSet = false;
  bool logToAdb = false;
  int qdpSubId;
  int dataCtlInstanceId;
  bool mUiFilterEnabled = false;
  bool mIsPdcRefreshInProgress = false;
  bool mIsTurningOffDataDuringVoiceCall = false;
  bool isDataCtlLoaded = false;

#ifndef RIL_FOR_MDM_LE
  datactl::DataCtlInterface * datactlInt = nullptr;
  uint8_t mSecureMode = STATE_NONSECURE;
  void* mSecureLibInstance = nullptr;
  getModemPeripheralStatusFnPtr mGetPeripheralState = nullptr;
  registerModemPeripheralCBFnPtr mRegisterPeriferalCb = nullptr;
  deregisterModemPeripheralCBFnPtr mDeregisterPeriferalCb = nullptr;
  void* mSecureModeContext = nullptr;
  TimeKeeper::timer_id mSecureModeTimer = TimeKeeper::no_timer;
  static constexpr TimeKeeper::millisec SECURE_MODE_REGISTRATION_RETRY_TIMEOUT = 1000;
  static void dataPPDataCapChgCb(bool capability);
  static void dataPPDataRecoCb(datactl::DataSubsRecommendation_t reco);
  std::shared_ptr<VoiceCallModemEndPoint> voiceCallEndPointSub0;
  std::shared_ptr<VoiceCallModemEndPoint> voiceCallEndPointSub1;
#endif

  std::unique_ptr<AuthManager> auth_manager;
  std::unique_ptr<CallManager> call_manager;
  std::unique_ptr<ProfileHandler> profile_handler;
  std::unique_ptr<NetworkServiceHandler> network_service_handler;
  std::unique_ptr<TmdManager> tmd_manager;
  std::shared_ptr<KeepAliveHandler> keep_alive;

  std::unique_ptr<NetworkAvailabilityHandler> networkavailability_handler;

  std::shared_ptr<DSDModemEndPoint> dsd_endpoint;
  std::shared_ptr<WDSModemEndPoint> wds_endpoint;
  std::shared_ptr<AuthModemEndPoint> auth_endpoint;
  #ifdef FEATURE_DATA_LQE
    std::shared_ptr<OTTModemEndPoint> ott_endpoint;
  #endif /* FEATURE_DATA_LQE */
  std::shared_ptr<PdcModemEndPoint> pdc_endpoint;
  std::shared_ptr<TMDModemEndPoint> tmd_endpoint;
  std::shared_ptr<PreferredDataInfo_t> preferred_data_state_info;

  bool loadDatactl();
  void unloadDatactl();
  dataQosInitFnPtr mDataQosInit;
  dataQosCleanUpFnPtr mDataQosCleanup;
  dataQosDeInitFnPtr mDataQosDeInit;
  initDataCtlFnPtr mInitDataCtl = nullptr;
  void* dataQosLibInstance = nullptr;
  void* datactlLib = nullptr;
  static void logFn(std::string);
  void processDeferredIntentToChange(DeferredIntentToChange_t deferIntent);

  LceHandler lceHandler;
  std::unique_ptr<PreferredDataStateMachine> preferred_data_sm;

   void handleQcrilInitMessage(std::shared_ptr<Message> msg);
   void broadcastReady();
   void checkProfileUpdateAllowed(std::string mcc, std::string mnc);
   void handleUimCardStatusIndMsg(std::shared_ptr<Message> msg);

#ifndef RIL_FOR_LOW_RAM
   void handlePullLceDataMessage(std::shared_ptr<Message> msg);
   void handleStartLceMessage(std::shared_ptr<Message> msg);
   void handleStopLceMessage(std::shared_ptr<Message> msg);
   void handlePreferredApnUpdated(std::shared_ptr<Message> msg);
   void handleNasPhysChanConfigReportingStatusMessage(std::shared_ptr<Message> msg);
   void handleNasPhysChanConfigMessage(std::shared_ptr<Message> msg);
   void handleDdsSwitchRecommendInternalMessage(std::shared_ptr<Message> msg);
   void handleGetSmartTempDdsSwitchCapabilityMessage(std::shared_ptr<Message> m);
   void handleGetCIWlanCapabilityMessage(std::shared_ptr<Message> m);
   void handleGetDataPPDataCapabilityMessage(std::shared_ptr<Message> m);
   void handleUpdateDataPPDataUIOptionMessage(std::shared_ptr<Message> m);
   void handleRegisterForDataDuringVoiceCall(std::shared_ptr<Message> m);
#endif //RIL_FOR_LOW_RAM

   void setApAssistMode();
   void handleIWLANCapabilityHandshake(std::shared_ptr<Message> msg);
   void handleGetAllQualifiedNetworksMessage(std::shared_ptr<Message> msg);
   void handleIntentToChangeInd(std::shared_ptr<Message> msg);
   void handleGetIWlanDataCallListRequestMessage(std::shared_ptr<Message> msg);
   void handleGetIWlanDataRegistrationStateRequestMessage(std::shared_ptr<Message> msg);
   void handleHandoverInformationIndMessage(std::shared_ptr<Message> msg);
   void handleHandoverFailureMessage(std::shared_ptr<Message> msg);
   void handleDsdSystemStatusPerApn(std::shared_ptr<Message> msg);
   void handleSetApnPreferredSystemResult(std::shared_ptr<Message> msg);
   void initializeIWLAN();
   void deinitializeIWLAN();
   void handleStartKeepAliveRequestMessage(std::shared_ptr<Message> msg);
   void handleStopKeepAliveRequestMessage(std::shared_ptr<Message> msg);
   void handleKeepAliveIndMessage(std::shared_ptr<Message> msg);
   void cleanupKeepAlive(int cid_val);
   void handleGoDormantMessage(std::shared_ptr<Message> msg);
   void handleProcessScreenStateChangeMessage(std::shared_ptr<Message> msg);
   void handleProcessStackSwitchMessage(std::shared_ptr<Message> msg);
   void handleSetApnInfoMessage(std::shared_ptr<Message> msg);
   void handleSetIsDataEnabledMessage(std::shared_ptr<Message> msg);
   void handleSetIsDataRoamingEnabledMessage(std::shared_ptr<Message> msg);
   void handleSetQualityMeasurementMessage(std::shared_ptr<Message> msg);
   void handleToggleDormancyIndMessage(std::shared_ptr<Message> msg);
   void handleToggleLimitedSysIndMessage(std::shared_ptr<Message> msg);
   void handleUpdateMtuMessage(std::shared_ptr<Message> msg);
   void handleGetDdsSubIdMessage(std::shared_ptr<Message> msg);
   void handleDataRequestDDSSwitchMessage(std::shared_ptr<Message> msg);
   void handleSetLteAttachPdnListActionResult(std::shared_ptr<Message> msg);
   void handleLinkPropertiesChangedMessage(std::shared_ptr<Message> msg);

   void handleSetLteAttachProfileMessage_v15(std::shared_ptr<Message> msg);
   void handleSetDataProfileRequestMessage(std::shared_ptr<Message> msg);
   void handleQmiAuthServiceIndMessage(std::shared_ptr<Message> msg);
   void handleQmiAuthEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   void handleQmiDsdEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   void handleSetLinkCapFilterMessage(std::shared_ptr<Message> msg);
   void handleSetLinkCapRptCriteriaMessage(std::shared_ptr<Message> msg);

   #ifdef FEATURE_DATA_LQE
     void handleQmiOttEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   #endif /* FEATURE_DATA_LQE */
   void handleNasRequestDataShutdown(std::shared_ptr<Message> msg);
   void handleDataAllBearerTypeUpdate(std::shared_ptr<Message> msg);
   void handleDataBearerTypeUpdate(std::shared_ptr<Message> msg);
   void handleToggleBearerAllocationUpdate(std::shared_ptr<Message> msg);
   void handleGetBearerAllocation(std::shared_ptr<Message> msg);
   void handleGetAllBearerAllocations(std::shared_ptr<Message> msg);
   void handleQmiWdsEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   void handleDataConnectionStateChangedMessage(std::shared_ptr<Message> msg);
   void handleDsdSystemStatusInd(std::shared_ptr<Message> msg);
   void handleSetPreferredDataModem(std::shared_ptr<Message> msg);
   void handleCurrentDDSIndMessage(std::shared_ptr<Message> msg);
   void handleCurrentRoamingStatusChangedMessage(std::shared_ptr<Message> msg);
   void registerForMasterRILIndications();
   void handleDDSSwitchResultIndMessage(std::shared_ptr<Message> msg);
   void handleRadioConfigClientConnectedMessage(std::shared_ptr<Message> msg);
   void handleInformDDSSUBChangedMessage(std::shared_ptr<Message> msg);
   void handleDDSSwitchTimeoutMessage(std::shared_ptr<Message> msg);
   void handleDDSSwitchIPCMessage(std::shared_ptr<Message> msg);
   void handleIAInfoIPCMessage(std::shared_ptr<Message> msg);
   std::shared_ptr<IPCMessage> constructDDSSwitchIPCMessage(IPCIStream &is);
   std::shared_ptr<IPCMessage> constructIAInfoIPCMessage(IPCIStream &is);
   RIL_Errno map_internalerr_from_reqlist_new_to_ril_err(IxErrnoType error);
   void performDataModuleInitialization();
   void qcrilDataInit();
   void handleRilEventDataCallback(std::shared_ptr<Message> msg);
   void handleSetupDataCallRequestMessage(std::shared_ptr<Message> msg, DataCallVersion_t version);
   void handleSetupDataCallRequestMessage_1_4(std::shared_ptr<Message> msg);
   void handleSetupDataCallRequestMessage_1_6(std::shared_ptr<Message> msg);
   void handleDeactivateDataCallRequestMessage(std::shared_ptr<Message> msg);
   void handleGetRadioDataCallListRequestMessage(std::shared_ptr<Message> msg);
   void handleDataCallTimerExpiredMessage(std::shared_ptr<Message> msg);
   void handleSetInitialAttachApn(std::shared_ptr<Message> msg);
   void handleProcessInitialAttachRequestMessage(std::shared_ptr<Message> msg);
   void handleIATimeoutMessage(std::shared_ptr<Message> msg);
   void initialAttachHandler(std::shared_ptr<Message> msg);
   void handleSetCarrierInfoImsiEncryptionMessage(std::shared_ptr<Message> msg);
   void handleRegistrationFailureReportingStatusMessage(std::shared_ptr<Message> msg);
   void handleStartLCERequestMessage(std::shared_ptr<Message> msg);
   void handleStopLCERequestMessage(std::shared_ptr<Message> msg);
   void handlePullLCEDataRequestMessage(std::shared_ptr<Message> msg);
   void handleDsiInitCompletedMessage(std::shared_ptr<Message> msg);
   void handleNasRfBandInfoIndMessage(std::shared_ptr<Message> msg);
   void handleNasRfBandInfoMessage(std::shared_ptr<Message> msg);
   void handleGetModemAttachParamsRetryMessage(std::shared_ptr<Message> msg);
   void handlePdcRefreshIndication(std::shared_ptr<Message> msg);
   void handleWdsThrottleInfoIndMessage(std::shared_ptr<Message> msg);
   void handlePlmnChangedIndMessage(std::shared_ptr<Message> msg);
   void handleGetPdnThrottleTimeResponseInd(std::shared_ptr<Message> msg);
   void handleUimCardAppStatusIndMsg(std::shared_ptr<Message> msg);
   void handleQmiTmdEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   int retrieveUIMCardStatus( std::shared_ptr<RIL_UIM_CardStatus> ril_card_status, string &aid_buffer, RIL_UIM_AppType *app_type);
   int retrieveUIMAppStatusFromAppInfo(RIL_UIM_AppStatus *application, string &aid_buffer, RIL_UIM_AppType *app_type);
   void qcrilDataUimEventAppStatusUpdate ( const qcril_request_params_type *const params_ptr, qcril_request_return_type *const ret_ptr);
   void handleClientDisconnectedMessage(std::shared_ptr<Message> msg);
   void handleClientConnectedMessage(std::shared_ptr<Message> msg);
   void handleGetSlicingConfigRequestMessage(std::shared_ptr<Message> msg);
   void handlePduSessionParamLookupResultIndMessage(std::shared_ptr<Message> msg);
   void handleUpdateNetworkAvailabilityCacheMessage(std::shared_ptr<Message> msg);
   void handleRouteChangeUpdateMessage(std::shared_ptr<Message> msg);
   void handleUpdateUIOptionMessage(std::shared_ptr<Message> m);
   void sendIAResponse(std::shared_ptr<Message> msg, RIL_Errno result);
   void handleExitFromCurrentStateRequestMessage(std::shared_ptr<Message> msg);
#ifdef RIL_FOR_MDM_LE
   void handleCaptureLogBufferMessage(std::shared_ptr<Message> msg);
   void handleIndicationRegistrationFilterMessage(std::shared_ptr<Message> msg);
#else
   void handleSetDataThrottlingRequestMessage(std::shared_ptr<Message> msg);
   void handleQosInitMessage(std::shared_ptr<Message> msg);
   void handleSetDataInActivityMessage(std::shared_ptr<Message> msg);
   void handleTcpMonitoringReqMessage(std::shared_ptr<Message> msg);
   void handleCleanUpTcpKeepAliveMessage(std::shared_ptr<Message> msg);
   void handleSecureModeChangedCallBackMessage(std::shared_ptr<Message> msg);
   void handleRegisterForSecureModeCbMessage(std::shared_ptr<Message> msg);

   bool loadDataQos();
   void unloadDataQos();

   void registerForSecureModeIndications();
   void deregisterForSecureModeIndications();
   void SecureModeTimeOutHandler(void*);

   void handleVoiceIndMessage(std::shared_ptr<Message> msg);
   void handleTempDDSSwitchTimerExpired(void *resp);
   void handleVoiceCallOrigTimerExpired(void *resp);
   void handleVoiceCallOrigTimeoutMessage(std::shared_ptr<Message> msg);
   void handleQmiVoiceEndpointStatusIndMessage(std::shared_ptr<Message> msg);
#ifdef QMI_RIL_UTF
  public:
#endif
   static int32_t SecureModeNotifyEvent(const uint32_t peripheral, const uint8_t state);
#endif
};
void qcrilDataprocessMccMncInfo
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
);

DataModule& getDataModule();

} //namespace

#endif
