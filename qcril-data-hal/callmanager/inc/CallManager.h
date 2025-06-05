/******************************************************************************
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
* All rights reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef CALLMANAGER
#define CALLMANAGER

#include <map>

#include "framework/Log.h"
#include "framework/Message.h"
#include "dsi_netctrl.h"
#include "CallStateMachine.h"
#include "LocalLogBuffer.h"

#ifdef USE_QCRIL_HAL
#include "modules/nas/NasPhysChanConfigMessage.h"
#else
#include "interfaces/nas/NasPhysChanConfigMessage.h"
#endif

#include "request/SetupDataCallRequestMessage.h"
#include "request/DeactivateDataCallRequestMessage.h"
#include "request/GetIWlanDataCallListRequestMessage.h"
#include "UnSolMessages/PhysicalConfigStructUpdateMessage.h"
#include "UnSolMessages/NasRfBandInfoIndMessage.h"

#define MaxConcurrentDataCalls DSI_MAX_DATA_CALLS

namespace rildata {

struct DeferredIntentToChange_t {
  dsd_apn_pref_sys_type_ex_v01 apnPrefSys;
  dsd_apn_pref_sys_apn_status_mask_v01 apnStatus;
};

struct PduSessionLookupResultRoute_t {
  int32_t cid;
  DataProfileInfo_t profile;
};

/**
 * @brief CallManager class
 * @details CallManager class
 */
class CallManager {
public:
  CallManager(LocalLogBuffer& logBuffer, qdp::Util& util);
  ~CallManager();

  void init(bool apAssistMode, bool partialRetry, unsigned long maxPartialRetryTimeout,
            BringUpCapability capability, bool afterBootup);
  void setDataCallVersion(DataCallVersion_t version);
  DataCallVersion_t getDataCallVersion();
  void handleCallEventMessage(CallEventTypeEnum event, std::shared_ptr<Message> msg);
  void handleDataBearerTypeUpdate(int32_t cid, rildata::BearerInfo_t bearer);
  void handleDataAllBearerTypeUpdate(rildata::AllocatedBearer_t bearer);
  void handleNasPhysChanConfigMessage(std::shared_ptr<Message> msg);

  int toggleLinkActiveStateChangeReport(bool report);

  void getRadioDataCallList(vector<DataCallResult_t> &callList);
  void getIWlanDataCallList(vector<DataCallResult_t> &callList);

  int getAvailableCid(void);
  void dataCallListChanged(void);
  void radioDataCallListChanged(const std::vector<rildata::DataCallResult_t>& dcList);
  bool getApnByCid(int cid, string& apn);
  void cleanCallInstance(int cid);

  void setProcessDeferredIntentCallback(std::function<void(DeferredIntentToChange_t)> cb);
  bool shouldDeferIntentToChange(string apn);
  void deferToProcessIntentToChange(DeferredIntentToChange_t deferIntent);
  void callStateChangedCallback(string apn, CallStateEnum state);

  ResponseError_t handleToggleBearerAllocationUpdate(bool enable);
  AllocatedBearerResult_t handleGetBearerAllocation(int32_t cid);
  AllocatedBearerResult_t handleGetAllBearerAllocations();
  void updateAllBearerAllocations();
  void cleanUpAllBearerAllocation();
  void updatePhysicalChannelConfigs();
  void enablePhysChanConfigReporting(bool enable);
  void generateCurrentPhysicalChannelConfigs();
  void generateCurrentPhysicalChannelConfigsFromBearers();
  void processQmiDsdApnPreferredSystemResultInd(const dsd_ap_asst_apn_pref_sys_result_ind_msg_v01 *ind);
  void processQmiDsdIntentToChangeApnPrefSysInd(const dsd_intent_to_change_apn_pref_sys_ind_msg_v01& ind);
  void processQmiDsdSystemStatusInd(const dsd_system_status_ind_msg_v01 * ind);
  void processQmiDsdSystemStatusInd(dsd_apn_avail_sys_info_type_v01 * apn_sys, uint32_t len);
  bool notifyApAssistApnPreferredSystemChangeSuported(bool flag);
  void updateCurrentRoamingStatus();
  void updateCurrentRoamingStatus(bool isRoaming);
  static bool isApnTypeInvalid(ApnTypes_t mSupportedApnTypesBitmap);
  static bool isApnTypeEmergency(ApnTypes_t mSupportedApnTypesBitmap);
  void dump(string padding, ostream& os) const;
  bool handleGoDormantRequest(std::string deviceName);
  bool sendDormancyRequest(CallInfo& callInfo);
  void registerDataRegistrationRejectCause(bool enable);
  void clearThrottledApnsCache();
  void handleWdsThrottleInfoInd(std::list<WdsThrottleInfo> throttleInfo);
  void unthrottleAllRadioApns();
  void unthrottleAllIWlanApns();
  void releaseThrottleTimer(const std::string apn, const int src);
  void disconnectCallFromOlderDDSSub();

  void setCleanupKeepAliveCallback(std::function<void(int)> cb);

  void cleanAllCalls();
  void cleanCallsInSecureMode();
  void triggerDsiInit(bool);
  void triggerDsiRelease();
  void dsiInitCompleted();
  static void dsiInitCbFunc(void *userData);
  static void dsiInitCb2Func(dsi_init_mode_t mode, void *userData);
  void sendFailureForIdleCalls();
  DsiInitStatus_t dsiInitStatus;
  void updateNasRfActiveBandInfo(std::shared_ptr<Message> msg);
  void handleNasRfBandInfoMessage(std::shared_ptr<Message> msg);
  void handlePduSessionParamLookupResultIndMessage(std::shared_ptr<Message> msg);
  void processSetDataInactivityPeriod(std::shared_ptr<Message> msg);
  void processRouteChangeUpdate(int cid, vector<TrafficDescriptor_t> tdList);
  static bool iWlanRegistered;

private:

  class Filter {
    public:
      Filter(std::list<CallStateMachine>& cl): mCallList(cl) {
        clearCriteria();
      }
      Filter& matchCid(int cid);
      Filter& matchApn(std::string apn);
      Filter& matchApnTypes(ApnTypes_t apnTypes);
      Filter& matchApnTypesLoosely(ApnTypes_t apnTypes);
      Filter& matchIpType(std::string ipType);
      Filter& matchIpTypeLoosely(std::string ipType);
      Filter& matchDeviceName(std::string deviceName);
      Filter& matchCurrentNetwork(HandoffNetworkType_t currentNetwork);
      Filter& matchCallStates(const std::set<int>& states);
      Filter& matchDnn(std::string dnnName);
      Filter& matchTrafficDescriptor(TrafficDescriptor_t trafficDescriptor);
      std::list<std::reference_wrapper<CallStateMachine>> getResults();
    private:
      enum Criteria : uint32_t {
        CID                = 1,
        APN                = 1 << 1,
        APN_TYPES          = 1 << 2,
        IP_TYPE            = 1 << 3,
        DEVICE_NAME        = 1 << 4,
        CURRENT_NETWORK    = 1 << 5,
        STATES             = 1 << 6,
        DNN                = 1 << 7,
        TRAFFIC_DESCRIPTOR = 1 << 8,
        LOOSE_IP_TYPE      = 1 << 9,
        LOOSE_APN_TYPES     = 1 << 10,
        CRITERIA_MAX,
      };
      uint32_t mCriteriaBitmask;
      int mCid;
      std::string mApn;
      ApnTypes_t mApnTypes;
      std::string mIpType;
      std::string mDeviceName;
      HandoffNetworkType_t mCurrentNetwork;
      std::set<int> mStates;
      std::string mDnn;
      TrafficDescriptor_t mTrafficDescriptor;
      std::list<CallStateMachine>& mCallList;
      bool matchesCriteria(CallStateMachine& call);
      void clearCriteria();
  };
  LocalLogBuffer& logBuffer;
  qdp::Util& util;
  GlobalCallInfo globalInfo{};
  bool mApAssistMode;
  bool mSaMode;
  bool mRadioDataAvailable;
  bool mRadioVoiceAvailable;
  bool mNulBearerReasonAvailable;
  bool mReportPhysicalChannelConfig;
  bool mPhysicalChannelInfoValid;
  bool mReportRegistrationRejectCause;
  bool mCIWlanCallExist;
  BringUpCapability mCallBringupCapability;
  std::function<void(int)> mCleanupKeepAliveCb = nullptr;
  std::vector<DataCallResult_t> radioDcList;
  std::vector<DataCallResult_t> iwlanDcList;
  std::list<CallStateMachine> mCallInstances;
  bool toggleBearerUpdateToDataConnectionService;
  bool availableCallIds[MaxConcurrentDataCalls];
  std::shared_ptr<WDSModemEndPoint> wds_endpoint;
  std::shared_ptr<DSDModemEndPoint> dsd_endpoint;
  std::vector<NasPhysChanInfo> mPhysicalChannelInfo;
  std::vector<PhysicalConfigStructInfo_t> mPhysicalConfigStructInfo;
  std::function<void(const std::vector<rildata::DataCallResult_t>&)> mDcListChangedCallback;
  NasActiveBand_t mActiveBandInfo;
  std::unordered_map<string, DeferredIntentToChange_t> deferredIntents;
  std::function<void(DeferredIntentToChange_t)> mProcessDeferredIntentCb = nullptr;
  uint16_t mPduSessionLookupTxId;
  std::unordered_map<uint16_t, uint16_t> mSetupDataCallTxIdTracker;
  std::unordered_map<uint16_t, PduSessionLookupResultRoute_t> mPduSessionLookupTxIdTracker;
  uint16_t allocatePduSessionLookupTxId() {return (mPduSessionLookupTxId++ % 0x7FFF);}
  bool physicalConfigStructInfoChanged(const std::vector<PhysicalConfigStructInfo_t>& configs);
  Filter mFilter{mCallInstances};
  bool validateSetupDataCallParams(std::shared_ptr<SetupDataCallRequestBase> m);
  bool convertQmiDsdToAvailableRadioDataService(dsd_system_status_info_type_v01 sys,
                                                RadioDataService_t &radioDataService);
  void processEventSendResponse(CallEventTypeEnum event, std::shared_ptr<Message> msg, ResponseError_t errorCode);
  TimeKeeper::timer_id startThrottleTimer(std::string apn, RequestSource_t src, TimeKeeper::millisec throttleTime);
};

} /* namespace rildata */

#endif
