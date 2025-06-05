/*==============================================================================
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/
#include <sstream>
#include <string.h>
#include "framework/Log.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include "UnSolMessages/RadioDataCallListChangeIndMessage.h"
#include "UnSolMessages/IWlanDataCallListChangeIndMessage.h"
#include "UnSolMessages/BearerAllocationUpdateMessage.h"
#include "UnSolMessages/HandoverInformationIndMessage.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "UnSolMessages/DataBearerTypeChangedMessage.h"
#include "UnSolMessages/DataAllBearerTypeChangedMessage.h"
#include "UnSolMessages/SetPreferredSystemMessage.h"
#include "UnSolMessages/DataRegistrationFailureCauseMessage.h"
#include "event/LinkPropertiesChangedMessage.h"
#include "event/RilEventDataCallback.h"
#include "UnSolMessages/DsiInitCompletedMessage.h"
#include "UnSolMessages/NasRfBandInfoIndMessage.h"
#include "UnSolMessages/PduSessionParamLookupResultIndMessage.h"
#include "request/NasGetRfBandInfoMessage.h"
#include "UnSolMessages/ThrottledApnTimerExpirationMessage.h"
#include "UnSolMessages/GetPdnThrottleTimeResponseInd.h"
#include "UnSolMessages/ConfigureDeactivateDelayTimeMessage.h"
#include "request/SetDataInActivityPeriodMessage.h"

#include "DataModule.h"
#include "CallManager.h"
#include "CallInfo.h"
#include "CallState.h"
#include "qcril_data.h"
#include "DsiWrapper.h"
#include <sstream>

using namespace rildata;
using namespace qdp;
using std::placeholders::_1;
using std::placeholders::_2;

bool CallManager::iWlanRegistered = false;

unordered_map<NasActiveBand_t,FrequencyRange_t> mNR5GBandToFreqRangeMap = {
  {NasActiveBand_t::NGRAN_BAND_1,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_2,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_3,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_5,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_7,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_8,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_12,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_14,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_18,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_20,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_25,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_26,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_28,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_29,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_30,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_34,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_38,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_39,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_40,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_41,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_48,FrequencyRange_t::HIGH},
  {NasActiveBand_t::NGRAN_BAND_50,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_51,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_53,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_65,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_66,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_70,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_71,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_74,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_75,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_76,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_77,FrequencyRange_t::HIGH},
  {NasActiveBand_t::NGRAN_BAND_78,FrequencyRange_t::HIGH},
  {NasActiveBand_t::NGRAN_BAND_79,FrequencyRange_t::HIGH},
  {NasActiveBand_t::NGRAN_BAND_80,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_81,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_82,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_83,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_84,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_85,FrequencyRange_t::UNKNOWN},
  {NasActiveBand_t::NGRAN_BAND_86,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_89,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_90,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_91,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_92,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_93,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_94,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_95,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_257,FrequencyRange_t::UNKNOWN},
  {NasActiveBand_t::NGRAN_BAND_258,FrequencyRange_t::UNKNOWN},
  {NasActiveBand_t::NGRAN_BAND_259,FrequencyRange_t::UNKNOWN},
  {NasActiveBand_t::NGRAN_BAND_260,FrequencyRange_t::UNKNOWN},
  {NasActiveBand_t::NGRAN_BAND_261,FrequencyRange_t::UNKNOWN},
};

CallManager::CallManager(LocalLogBuffer& setLogBuffer, Util& util): logBuffer(setLogBuffer), util(util) {
  globalInfo.apAssistMode = false;
  globalInfo.partialRetryEnabled = true;
  mSaMode = false;
  mRadioDataAvailable = false;
  mNulBearerReasonAvailable = true;
  mRadioVoiceAvailable = false;
  globalInfo.maxPartialRetryTimeout = DEFAULT_MAX_PARTIAL_RETRY_TIMEOUT;
  mReportPhysicalChannelConfig = false;
  globalInfo.linkStateChangeReport = false;
  globalInfo.dataCallVersion = DataCallVersion_t::UNDETERMINED;
  toggleBearerUpdateToDataConnectionService = false;
  globalInfo.reportPhysicalChannelConfig = mReportPhysicalChannelConfig;
  mPhysicalChannelInfoValid = false;
  mReportRegistrationRejectCause = false;
  mCIWlanCallExist = false;
  dsiInitStatus = DsiInitStatus_t::RELEASED;
  mActiveBandInfo = NasActiveBand_t::NGRAN_BAND_MIN_ENUM_VAL;
  mPduSessionLookupTxId = 1;

  mDcListChangedCallback = [](const vector<rildata::DataCallResult_t>& dcList) -> void {
    auto msg = std::make_shared<rildata::RadioDataCallListChangeIndMessage>(dcList);
    msg->broadcast();
  };

  globalInfo.isRoaming = false;
  globalInfo.serviceStatus.availableRadioDataService.clear();
  mPhysicalConfigStructInfo.clear();
}

CallManager::~CallManager() {
  for (auto apnMap : globalInfo.serviceStatus.throttledApns) {
    ThrottleApn_t throttleApn = apnMap.second;
    for (auto srcMap : throttleApn.timer) {
      TimeKeeper::getInstance().clear_timer(srcMap.second);
    }
  }
  globalInfo.serviceStatus.throttledApns.clear();
  while (!mCallInstances.empty()) {
    mCallInstances.erase(mCallInstances.begin());
  }
  #ifdef QMI_RIL_UTF
  mReportPhysicalChannelConfig = false;
  globalInfo.reportPhysicalChannelConfig = mReportPhysicalChannelConfig;
  #endif
  //cleanUpAllBearerAllocation();
  globalInfo.serviceStatus.availableRadioDataService.clear();
  triggerDsiRelease();
}

void CallManager::init(bool apAssistMode, bool partialRetry, unsigned long maxPartialRetryTimeout,
                       BringUpCapability capability, bool afterBootup)
{
  Log::getInstance().d("[CallManager]: init");
  globalInfo.apAssistMode = apAssistMode;
  globalInfo.partialRetryEnabled = partialRetry;
  globalInfo.maxPartialRetryTimeout = maxPartialRetryTimeout;
  mCallBringupCapability = capability;
  memset(availableCallIds, true, sizeof(availableCallIds));
  while (!mCallInstances.empty()) {
    mCallInstances.erase(mCallInstances.begin());
  }
  wds_endpoint = ModemEndPointFactory<WDSModemEndPoint>::getInstance().buildEndPoint();
  dsd_endpoint = ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint();
  triggerDsiInit(afterBootup);
}

void CallManager::updateCurrentRoamingStatus(bool roaming) {
  Log::getInstance().d("[CallManager] roaming status updated = " + std::to_string((int)roaming));
  globalInfo.isRoaming = roaming;
}

void CallManager::setDataCallVersion(DataCallVersion_t version) {
  globalInfo.dataCallVersion = version;
}

DataCallVersion_t CallManager::getDataCallVersion() {
  return globalInfo.dataCallVersion;
}

HandoffNetworkType_t convertToHandoffNetworkType(dsd_apn_pref_sys_enum_v01 prefSys) {
  switch (prefSys) {
    case DSD_APN_PREF_SYS_WWAN_V01:
      return _eWWAN;
    case DSD_APN_PREF_SYS_WLAN_V01:
      return _eWLAN;
    case DSD_APN_PREF_SYS_IWLAN_V01:
      return _eIWLAN;
    case DSD_APN_PREF_SYS_CIWLAN_V01:
      return _eCIWLAN;
    default:
      Log::getInstance().d("[CallManager]: invalid preferred sys=" +
                           std::to_string(static_cast<int>(prefSys)));
      return _eWWAN;
  }
}

void CallManager::dump(string padding, ostream& os) const
{
  os << padding << "CallManager:" << endl;
  padding += "    ";
  os << padding << std::boolalpha << "ApAssistMode=" << globalInfo.apAssistMode << endl;
  os << padding << std::boolalpha << "PartialRetryEnabled=" << globalInfo.partialRetryEnabled << endl;
  os << padding << std::boolalpha << "SaMode=" << mSaMode << endl;
  os << padding << std::boolalpha << "RadioDataAvailable=" << mRadioDataAvailable << endl;
  os << padding << std::boolalpha << "RadioVoiceAvailable=" << mRadioVoiceAvailable << endl;
  os << padding << std::boolalpha << "NulBearerReasonAvailable=" << mNulBearerReasonAvailable << endl;
  os << padding << "MaxPartialRetryTimeout=" << globalInfo.maxPartialRetryTimeout << endl;
  os << padding << std::boolalpha << "ReportPhysicalChannelConfig=" << mReportPhysicalChannelConfig << endl;
  os << padding << std::boolalpha << "PhysicalChannelInfoValid=" << mPhysicalChannelInfoValid << endl;
  os << padding << std::boolalpha << "ReportDataRegistrationRejectCause=" << mReportRegistrationRejectCause << endl;
  os << padding << std::boolalpha << "ReportLinkActiveStateChange=" << globalInfo.linkStateChangeReport << endl;
  os << padding << "CallBringupCapability=" << (int)mCallBringupCapability << endl;
  os << padding << std::boolalpha << "ToggleBearerUpdate=" << toggleBearerUpdateToDataConnectionService << endl;
  os << endl << padding  << "RadioDCList:" << endl;
  for (const DataCallResult_t& call : radioDcList) {
    call.dump(padding + "    ", os);
    os << endl;
  }
  os << endl << padding  << "IWlanDCList:" << endl;
  for (const DataCallResult_t& call : iwlanDcList) {
    call.dump(padding + "    ", os);
    os << endl;
  }
  os << endl << padding  << "GlobalAvailableRadioDataServices: ";
  for (const RadioDataService_t& service : globalInfo.serviceStatus.availableRadioDataService) {
    service.dump("", os);
    os << ",";
  }
  os << endl << padding  << "ApnPreferredRadioDataServices: " << endl;
  for (auto it = globalInfo.serviceStatus.apnPreferredRadioDataService.begin(); it != globalInfo.serviceStatus.apnPreferredRadioDataService.end(); it++) {
    os << padding << "    " << it->first << " ";
    it->second.dump("", os);
    os << endl;
  }
  os << endl << padding  << "Throttled apns: " << endl;
  for (auto apnMap : globalInfo.serviceStatus.throttledApns) {
    ThrottleApn_t throttleApn = apnMap.second;
    for (auto srcMap : throttleApn.timer) {
      os << padding << "    " << apnMap.first << "," << srcMap.first;
      os << endl;
    }
  }
  os << endl << padding  << "LastPhysicalConfigStructInfo: ";
  for (const PhysicalConfigStructInfo_t& config : mPhysicalConfigStructInfo) {
    config.dump("", os);
  }
  os << endl << padding << "Calls:" << endl;
  for (const CallStateMachine& callInstance : mCallInstances) {
    callInstance.dump(padding + "    ", os);
    os << endl;
  }
}

void CallManager::setCleanupKeepAliveCallback(std::function<void(int)> cb) {
  mCleanupKeepAliveCb = cb;
}

/**
 * @brief find available cid
 * @details find available cid from availableCallIds array and set to not available
 * @return available cid
 */
int CallManager::getAvailableCid(void) {
  int cid=-1;
  for (int i=0 ; i<MaxConcurrentDataCalls ; i++) {
    if(availableCallIds[i] == true) {
      availableCallIds[i] = false;
      cid = i;
      break;
    }
  }

  Log::getInstance().d("[CallManager]: available cid = " + std::to_string(cid));
  return cid;
}

void CallManager::releaseThrottleTimer(const std::string apn, const int src) {
  auto timer_id = globalInfo.serviceStatus.throttledApns[apn].timer[src];
  TimeKeeper::getInstance().clear_timer(timer_id);
  globalInfo.serviceStatus.throttledApns[apn].timer.erase(src);
}

TimeKeeper::timer_id CallManager::startThrottleTimer(std::string apn, RequestSource_t src, TimeKeeper::millisec throttleTime) {
  auto timeoutHandler = [apn, src, this](void *) {
    ThrottleApn_t throttleApn = globalInfo.serviceStatus.throttledApns[apn];
    auto msg = std::make_shared<ThrottledApnTimerExpirationMessage>(throttleApn.profileInfo, src);
    msg->broadcast();
    throttleApn.timer.erase((int)src);
  };
  return TimeKeeper::getInstance().set_timer(timeoutHandler, nullptr, throttleTime);
}

void CallManager::handleWdsThrottleInfoInd(std::list<WdsThrottleInfo> throttleInfo) {
  std::unordered_set<std::string> apnsToUnthrottle;
  Log::getInstance().d("[CallManager]: handleWdsThrottleInfoInd");
  // create a map of all throttled radio apns being tracked
  for (auto apnMap : globalInfo.serviceStatus.throttledApns) {
    ThrottleApn_t throttleApn = apnMap.second;
    for (auto srcMap : throttleApn.timer) {
      if (srcMap.first == (int)RequestSource_t::RADIO) {
        apnsToUnthrottle.insert(apnMap.first);
        Log::getInstance().d("[CallManager]: untrottle apn " + apnMap.first );
      }
    }
  }
  for (auto info : throttleInfo) {
    if (globalInfo.serviceStatus.throttledApns.find(info.getApn()) !=
        globalInfo.serviceStatus.throttledApns.end()) {
      ThrottleApn_t throttleApn = globalInfo.serviceStatus.throttledApns[info.getApn()];
      if(throttleApn.timer.find((int)RequestSource_t::RADIO) != throttleApn.timer.end())
      {
        int64_t rilThrottleTime = -1;
        if (info.hasV4ThrottleTime() && info.hasV6ThrottleTime()) {
          rilThrottleTime = convertToRilThrottleTime(std::min(info.getV4ThrottleTime(), info.getV6ThrottleTime()));
        } else if (info.hasV4ThrottleTime()) {
          rilThrottleTime = convertToRilThrottleTime(info.getV4ThrottleTime());
        } else if (info.hasV6ThrottleTime()) {
          rilThrottleTime = convertToRilThrottleTime(info.getV6ThrottleTime());
        }
        releaseThrottleTimer(info.getApn(), (int)RequestSource_t::RADIO);
        Log::getInstance().d("[CallManager]: throttleTime " + std::to_string(rilThrottleTime));
        // if throttle timer is still running for the apn, it should be removed from the apnsToUnthrottle set
        if (rilThrottleTime < 0 || rilThrottleTime == RIL_INFINITE_THROTTLING) {
          // clear the timer if throttle time not available or infinite
          globalInfo.serviceStatus.throttledApns[info.getApn()].timer[(int)RequestSource_t::RADIO] = TimeKeeper::no_timer;
          apnsToUnthrottle.erase(info.getApn());
        } else if (rilThrottleTime == 0) {
        } else {
          globalInfo.serviceStatus.throttledApns[info.getApn()].timer[(int)RequestSource_t::RADIO] = startThrottleTimer(info.getApn(), RequestSource_t::RADIO, rilThrottleTime);
          apnsToUnthrottle.erase(info.getApn());
        }
      }
    }
  }
  for (auto apn : apnsToUnthrottle) {
    releaseThrottleTimer(apn, (int)RequestSource_t::RADIO);
    ThrottleApn_t throttleApn = globalInfo.serviceStatus.throttledApns[apn];
    auto msg = std::make_shared<ThrottledApnTimerExpirationMessage>(throttleApn.profileInfo, RequestSource_t::RADIO);
    Log::getInstance().d("[CallManager]: broadcasting " + msg->dump());
    msg->broadcast();
  }
}

void CallManager::clearThrottledApnsCache() {
  for (auto apnMap : globalInfo.serviceStatus.throttledApns) {
    ThrottleApn_t throttleApn = apnMap.second;
    for (auto srcMap : throttleApn.timer) {
      TimeKeeper::getInstance().clear_timer(srcMap.second);
    }
  }
  globalInfo.serviceStatus.throttledApns.clear();
}

void CallManager::unthrottleAllRadioApns() {
  for (auto kv : globalInfo.serviceStatus.throttledApns) {
    ThrottleApn_t throttleApn = kv.second;
    if (throttleApn.timer.find((int)RequestSource_t::RADIO) != throttleApn.timer.end()) {
      ThrottleApn_t throttleApn = globalInfo.serviceStatus.throttledApns[kv.first];
      auto msg = std::make_shared<ThrottledApnTimerExpirationMessage>(throttleApn.profileInfo, RequestSource_t::RADIO);
      Log::getInstance().d("[CallManager]: broadcasting " + msg->dump());
      msg->broadcast();
      releaseThrottleTimer(kv.first, (int)RequestSource_t::RADIO);
    }
  }
}

void CallManager::unthrottleAllIWlanApns() {
  for (auto kv : globalInfo.serviceStatus.throttledApns) {
    ThrottleApn_t throttleApn = kv.second;
    if (throttleApn.timer.find((int)RequestSource_t::IWLAN) != throttleApn.timer.end()) {
      ThrottleApn_t throttleApn = globalInfo.serviceStatus.throttledApns[kv.first];
      auto msg = std::make_shared<ThrottledApnTimerExpirationMessage>(throttleApn.profileInfo, RequestSource_t::IWLAN);
      Log::getInstance().d("[CallManager]: broadcasting " + msg->dump());
      msg->broadcast();
      releaseThrottleTimer(kv.first, (int)RequestSource_t::IWLAN);
    }
  }
}

bool CallManager::isApnTypeInvalid(ApnTypes_t apnTypes) {
  int eBit = static_cast<int>(ApnTypes_t::EMERGENCY);
  int apnTypesBitmap = static_cast<int>(apnTypes);
  // if emergency bit is set, apn types should be configured to ONLY emergency type
  // otherwise, qcrildata will treat it as a misconfiguration
  if (((eBit & apnTypesBitmap) == eBit) &&
      (eBit != apnTypesBitmap)) {
    return true;
  }
  return false;
}

bool CallManager::isApnTypeEmergency(ApnTypes_t apnTypes) {
  return apnTypes == ApnTypes_t::EMERGENCY;
}

static DataCallResult_t convertToDcResult(const CallInfo& callInfo) {
  DataCallResult_t callInst = {};
  callInst.cause = callInfo.cause;
  callInst.suggestedRetryTime = -1;
  callInst.cid = callInfo.cid;
  callInst.active = callInfo.consolidatedActive;
  if (callInst.cause == DataCallFailCause_t::NONE) {
    callInst.type = callInfo.ipType;
    callInst.ifname = callInfo.deviceName;
    callInst.addresses = callInfo.convertToLegacyLinkAddresses();
    callInst.linkAddresses = callInfo.convertAddrStringToLinkAddresses();
    callInst.dnses = callInfo.convertToResultDnsAddresses();
    if (!(callInfo.pduInfoList.empty())) {
      callInst.gateways = callInfo.pduInfoList.front().concatGatewayAddress();
      callInst.pcscf = callInfo.pduInfoList.front().concatPcscfAddress();
    }
    callInst.mtu = callInfo.mtu;
    callInst.mtuV4 = callInfo.consolidatedMtuV4;
    callInst.mtuV6 = callInfo.consolidatedMtuV6;
    for(auto psp : callInfo.pduSessionParams) {
      for (auto rp : psp.requestProfiles)
      {
        if (rp.trafficDescriptor.has_value())
        {
          callInst.trafficDescriptors.push_back(rp.trafficDescriptor.value());
        }
      }
    }
  }
  callInst.qosSessions = callInfo.qosSessions;
  return callInst;
}

void CallManager::getRadioDataCallList(vector<DataCallResult_t> &call)
{
  Log::getInstance().d("[CallManager::getRadioDataCallList] from "+ std::to_string((int)mCallInstances.size()));

  auto matchedCalls = mFilter
      .matchCurrentNetwork(_eWWAN)
      .getResults();

  if( !globalInfo.apAssistMode ) {
    auto iwlanMatchedCalls = mFilter
        .matchCurrentNetwork(_eIWLAN)
        .getResults();

    for (auto it = iwlanMatchedCalls.begin(); it != iwlanMatchedCalls.end(); ++it) {
      matchedCalls.push_back(*it);
    }
  }

  call.resize(matchedCalls.size());

  transform(matchedCalls.begin(), matchedCalls.end(),
    call.begin(), [](CallStateMachine& callInstance) -> DataCallResult_t {
      return convertToDcResult(callInstance.getCallInfo());
  });
  stringstream ss;
  ss << "[CallManager]: getRadioDataCallList ";
  for (const DataCallResult_t& c : call) {
    c.dump("", ss);
    ss << ",";
  }
  Log::getInstance().d(ss.str());
  logBuffer.addLogWithTimestamp(ss.str());
}

void CallManager::getIWlanDataCallList(vector<DataCallResult_t> &call)
{
  Log::getInstance().d("[CallManager::getIWlanDataCallList] from "+ std::to_string((int)mCallInstances.size()));

  auto matchedCalls = mFilter
      .matchCurrentNetwork(_eIWLAN)
      .getResults();
  auto matchedCIWlanCalls = mFilter
      .matchCurrentNetwork(_eCIWLAN)
      .getResults();
  for (auto it = matchedCIWlanCalls.begin(); it != matchedCIWlanCalls.end(); ++it) {
    matchedCalls.push_back(*it);
  }
  call.resize(matchedCalls.size());

  transform(matchedCalls.begin(), matchedCalls.end(),
    call.begin(), [](CallStateMachine& callInstance) -> DataCallResult_t {
      return convertToDcResult(callInstance.getCallInfo());
  });
  stringstream ss;
  ss << "[CallManager]: getIWlanDataCallList ";
  for (const DataCallResult_t& c : call) {
    c.dump("", ss);
    ss << ",";
  }
  Log::getInstance().d(ss.str());
  logBuffer.addLogWithTimestamp(ss.str());

}

void CallManager::dataCallListChanged() {
  Log::getInstance().d("[CallManager]: dataCallListChanged for the number of calls = " +
                      std::to_string(mCallInstances.size()));
  bool isRadioDcListChanged = false;
  bool isIWlanDcListChanged = false;
  uint16_t sizeOfradioDcList = radioDcList.size();
  uint16_t sizeOfiwlanDcList = iwlanDcList.size();
  Log::getInstance().d("current size of radioDcList = "+ std::to_string(sizeOfradioDcList));
  Log::getInstance().d("current size of iwlanDcList = "+ std::to_string(sizeOfiwlanDcList));

  radioDcList.clear();
  iwlanDcList.clear();

  bool existCIWlanCall = false;
  auto matchedCIWlanCalls = mFilter
      .matchCurrentNetwork(_eCIWLAN)
      .matchCallStates({_eConnected})
      .getResults();
  if (!matchedCIWlanCalls.empty()) {
    existCIWlanCall = true;
  }
  if (mCIWlanCallExist != existCIWlanCall) {
    mCIWlanCallExist = existCIWlanCall;
    uint64_t delayTime = 0;
    if (mCIWlanCallExist) {
      delayTime = DEFAULT_CIWLAN_DEACTIVATE_DELAY_TIME*1000;
    }
    auto cIWlanMsg = std::make_shared<rildata::ConfigureDeactivateDelayTimeMessage>(delayTime);
    cIWlanMsg->broadcast();
  }

  auto matchedRadioCalls = mFilter
      .matchCurrentNetwork(_eWWAN)
      .matchCallStates({_eConnected, _eHandover, _eDisconnecting, _eDisconnected})
      .getResults();

  if (!globalInfo.apAssistMode) {
    auto iwlanCalls = mFilter
        .matchCurrentNetwork(_eIWLAN)
        .matchCallStates({_eConnected, _eHandover, _eDisconnecting, _eDisconnected})
        .getResults();

    for (auto it = iwlanCalls.begin(); it != iwlanCalls.end(); ++it) {
      matchedRadioCalls.push_back(*it);
    }
  }
  radioDcList.resize(matchedRadioCalls.size());

  transform(matchedRadioCalls.begin(), matchedRadioCalls.end(),
    radioDcList.begin(), [&isRadioDcListChanged](CallStateMachine& callInstance) -> DataCallResult_t {
      CallInfo& callInfo = callInstance.getCallInfo();
      if (callInfo.callParamsChanged) {
        isRadioDcListChanged = true;
      }
      return convertToDcResult(callInfo);
  });

  if (globalInfo.apAssistMode) {
    auto matchedIWlanCalls = mFilter
        .matchCurrentNetwork(_eIWLAN)
        .matchCallStates({_eConnected, _eHandover, _eDisconnecting, _eDisconnected})
        .getResults();
    auto matchedCIWlanCalls = mFilter
        .matchCurrentNetwork(_eCIWLAN)
        .matchCallStates({_eConnected, _eHandover, _eDisconnecting, _eDisconnected})
        .getResults();
    for (auto it = matchedCIWlanCalls.begin(); it != matchedCIWlanCalls.end(); ++it) {
      matchedIWlanCalls.push_back(*it);
    }
    iwlanDcList.resize(matchedIWlanCalls.size());

    transform(matchedIWlanCalls.begin(), matchedIWlanCalls.end(),
      iwlanDcList.begin(), [&isIWlanDcListChanged](CallStateMachine& callInstance) -> DataCallResult_t {
        CallInfo& callInfo = callInstance.getCallInfo();
        if (callInfo.callParamsChanged) {
          isIWlanDcListChanged = true;
        }
        return convertToDcResult(callInfo);
    });
  }

  for (CallStateMachine& callInstance : mCallInstances) {
    CallInfo& callInfo = callInstance.getCallInfo();
    callInfo.callParamsChanged = false;
  }

  if (sizeOfradioDcList != radioDcList.size()) {
    isRadioDcListChanged = true;
  }
  if (sizeOfiwlanDcList != iwlanDcList.size()) {
    isIWlanDcListChanged = true;
  }

  Log::getInstance().d("Radio Call list number " + std::to_string((int)radioDcList.size()));
  if(isRadioDcListChanged) {
    radioDataCallListChanged(radioDcList);
  }

  Log::getInstance().d("IWlan Call list number " + std::to_string((int)iwlanDcList.size()));
  if(isIWlanDcListChanged) {
    auto iwlanMsg = std::make_shared<rildata::IWlanDataCallListChangeIndMessage>(iwlanDcList);
    if (iwlanMsg != nullptr) {
        Log::getInstance().d("[CallManager]: " + std::to_string((int)iwlanDcList.size()) + "< iwlanDataCallListChanged " + iwlanMsg->dump());
        logBuffer.addLogWithTimestamp("[CallManager]: " + std::to_string((int)iwlanDcList.size()) + "< iwlanDataCallListChanged " + iwlanMsg->dump());
        iwlanMsg->broadcast();
    }
  }
}

std::string dumpDcList(const std::vector<rildata::DataCallResult_t>& dcList) {
  stringstream ss;
  ss << "{";
  for (const DataCallResult_t& call : dcList) {
    call.dump("", ss);
    ss << ",";
  }
  ss << "}";
  return ss.str();
}

void CallManager::radioDataCallListChanged(const std::vector<rildata::DataCallResult_t>& dcList) {
  std::string dcListStr;
  if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_4) {
    vector<DataCallResult_t> trimmed(dcList.size());
    transform(dcList.begin(), dcList.end(), trimmed.begin(), CallState::trimDataCallResult);
    dcListStr = dumpDcList(trimmed);
    mDcListChangedCallback(trimmed);
  } else {
    dcListStr = dumpDcList(dcList);
    mDcListChangedCallback(dcList);
  }
  Log::getInstance().d("[CallManager]: " + std::to_string((int)dcList.size()) + "< radioDataCallListChanged " + dcListStr);
  logBuffer.addLogWithTimestamp("[CallManager]: " + std::to_string((int)dcList.size()) + "< radioDataCallListChanged " + dcListStr);
}

void CallManager::processQmiDsdApnPreferredSystemResultInd(const dsd_ap_asst_apn_pref_sys_result_ind_msg_v01 *ind) {
  if(ind != nullptr && globalInfo.apAssistMode) {
    Log::getInstance().d("CallManager::processQmiDsdApnPreferredSystemResultInd ENTRY" + std::to_string((int)ind->result_info.result));
    std::string apnName(ind->result_info.apn_pref_sys.apn_name);
    auto matchedCalls = mFilter
      .matchApn(apnName)
      .getResults();
    if (!matchedCalls.empty()) {
      CallStateMachine& callInstance = matchedCalls.front();
      CallInfo& callInfo = callInstance.getCallInfo();
      HandoffState_t state = HandoffState_t::Failure;
      if(ind->result_info.result == DSD_AP_ASST_APN_PREF_SYS_RESULT_SUCCESS_V01) {
        state = HandoffState_t::PrefSysChangedSuccess;
      } else if (ind->result_info.result == DSD_AP_ASST_APN_PREF_SYS_RESULT_FAILURE_V01) {
        state = HandoffState_t::PrefSysChangedFailure;
      }
      auto msg = std::make_shared<HandoverInformationIndMessage>(
                          state, IpFamilyType_t::NotAvailable, callInfo.cid);
      if (msg != nullptr) {
        HandoffNetworkType_t prefNw = convertToHandoffNetworkType(ind->result_info.apn_pref_sys.pref_sys);
        msg->setPreferredNetwork(prefNw);
        handleCallEventMessage(CallEventTypeEnum::HandoverInformationInd, msg);
      }
    } else {
     Log::getInstance().d("Call not found");
    }
  }
}

bool CallManager::getApnByCid(int cid, string& apn) {
  auto matchedCalls = mFilter
      .matchCid(cid)
      .getResults();
  if (!matchedCalls.empty()) {
    CallStateMachine& callInstance = matchedCalls.front();
    const CallInfo& callInfo = callInstance.getCallInfo();
    if (callInfo.pduSessionParams[0].requestProfiles.size() > 0)
    {
      apn = callInfo.pduSessionParams[0].requestProfiles[0].apn;
      return true;
    }
    else {
      return false;
    }
  }
  return false;
}

void CallManager::processQmiDsdIntentToChangeApnPrefSysInd(const dsd_intent_to_change_apn_pref_sys_ind_msg_v01& ind)
{
  Log::getInstance().d("[CallManager]: process intent to change ind");
  if(globalInfo.apAssistMode) {
    Log::getInstance().d("[CallManager]: number of pref systems = "+std::to_string((int)ind.apn_pref_sys_len));
    for (uint32_t i=0 ; i<ind.apn_pref_sys_len; i++) {
      string apnName(ind.apn_pref_sys[i].apn_pref_info.apn_name);
      HandoffNetworkType_t prefNetwork = convertToHandoffNetworkType(ind.apn_pref_sys[i].apn_pref_info.pref_sys);

      list<reference_wrapper<CallStateMachine>> matchedCalls;
      if (ind.apn_pref_sys[i].apn_type_mask == QMI_DSD_APN_TYPE_MASK_EMERGENCY_V01) {
        matchedCalls = mFilter
          .matchApnTypes(ApnTypes_t::EMERGENCY)
          .getResults();
        if (!matchedCalls.empty()) {
          CallStateMachine& callInstance = matchedCalls.front();
          CallInfo& callInfo = callInstance.getCallInfo();
          callInfo.pduSessionParams[0].dnnName = apnName;
        }
      }
      else {
        matchedCalls = mFilter
          .matchApn(apnName)
          .getResults();
        if (matchedCalls.empty()) {
          matchedCalls = mFilter
            .matchDnn(apnName)
            .getResults();
        }
      }
      if (matchedCalls.empty()) {
        // call is not found for apn, send ack to modem
        Log::getInstance().d("[CallManager]: call is not found for apn = "+apnName+
                             ", ack to modem with pref sys = "+std::to_string((int)prefNetwork));
        CallState::sendSetApnPreferredSystemRequest(dsd_endpoint, apnName, prefNetwork);
      } else {
          // if rat is null bearer and attach in progress is true, QNP will not send
          // pref RAT change to the framework since there is no valid RAT. Hence
          // ack the intent immediately since framework will not do any handoff
          if ((ind.apn_pref_sys[i].apn_pref_info.pref_sys == DSD_APN_PREF_SYS_WWAN_V01) &&
              (ind.apn_pref_sys[i].rat == DSD_SYS_RAT_EX_NULL_BEARER_V01) &&
              (ind.apn_status_valid) &&
              (ind.apn_status[i] & DSD_APN_PREF_SYS_APN_STATUS_ATTACH_IN_PROGRESS_V01)){
            Log::getInstance().d("[CallManager]: rat is null bearer due to attach in progress for apn = "
                                 +apnName + " ack the intent");
            CallState::sendSetApnPreferredSystemRequest(dsd_endpoint, apnName, prefNetwork);
          } else {
            CallStateMachine& callInstance = matchedCalls.front();
            CallInfo& callInfo = callInstance.getCallInfo();
            if ((callInfo.currentRAT == HandoffNetworkType_t::_eIWLAN && prefNetwork == HandoffNetworkType_t::_eCIWLAN) ||
                (callInfo.currentRAT == HandoffNetworkType_t::_eCIWLAN && prefNetwork == HandoffNetworkType_t::_eIWLAN)) {
              Log::getInstance().d("[CallManager]: preferred rat is in same IWLAN transport for apn = "
                                   +apnName + " ack the intent");
              CallState::sendSetApnPreferredSystemRequest(dsd_endpoint, apnName, prefNetwork);
            }
            else {
              if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
                auto msg = std::make_shared<SetPreferredSystemMessage>(callInfo.cid, apnName, prefNetwork);
                handleCallEventMessage(CallEventTypeEnum::SetPreferredSystem, msg);
              }
              else {
                auto msg = std::make_shared<SetPreferredSystemMessage>(callInfo.cid, prefNetwork);
                handleCallEventMessage(CallEventTypeEnum::SetPreferredSystem, msg);
              }
            }
          }
      }
    }
  }
}

/**
 * Toggle whether to send bearer allocation updates via HAL
 */
ResponseError_t CallManager::handleToggleBearerAllocationUpdate(bool enable)
{
  Log::getInstance().d("[CallManager]: handleToggleBearerAllocationUpdate = "+std::to_string((int)enable));
  toggleBearerUpdateToDataConnectionService = enable;
  updateAllBearerAllocations();

  return ResponseError_t::NO_ERROR;
}

/**
 * handle modem bearer type update indication
 */
void CallManager::handleDataBearerTypeUpdate(int32_t cid, rildata::BearerInfo_t bearer)
{
  Log::getInstance().d("[CallManager]: handleDataBearerTypeChanged for cid = "
      +std::to_string((int)cid)
      +", bid = "
      +std::to_string((int)bearer.bearerId));

  auto matchedCalls = mFilter
      .matchCid(cid)
      .getResults();
  if (!matchedCalls.empty()) {
    CallStateMachine& callInstance = matchedCalls.front();
    CallInfo& callInfo = callInstance.getCallInfo();
    std::vector<rildata::BearerInfo_t>::iterator i;
    for (i=callInfo.bearerCollection.bearers.begin() ; i!=callInfo.bearerCollection.bearers.end() ; ++i)
    {
      if(i->bearerId == bearer.bearerId) {
          if ((i->uplink != bearer.uplink) || (i->downlink != bearer.downlink)) {
              i->uplink = bearer.uplink;
              i->downlink = bearer.downlink;
          }
          break;
      }
    }
    if (i==callInfo.bearerCollection.bearers.end()) {
      // new bearer
      callInfo.bearerCollection.bearers.push_back(bearer);
    }
    updateAllBearerAllocations();
  }
  else {
    Log::getInstance().d("[CallManager]: no call for cid = "+std::to_string((int)cid));
  }
}

/**
 * handle modem bearer type update indication
 */
void CallManager::handleDataAllBearerTypeUpdate(rildata::AllocatedBearer_t bearer)
{
  Log::getInstance().d("[CallManager]: handleDataBearerTypeChanged for cid = "
      +std::to_string((int)bearer.cid));
  auto matchedCalls = mFilter
      .matchCid(bearer.cid)
      .getResults();
  if (!matchedCalls.empty()) {
    CallStateMachine& callInstance = matchedCalls.front();
    CallInfo& callInfo = callInstance.getCallInfo();
    callInfo.bearerCollection.bearers.clear();
    callInfo.bearerCollection.bearers.insert(callInfo.bearerCollection.bearers.end(),
        bearer.bearers.begin(), bearer.bearers.end());
    updateAllBearerAllocations();
  } else {
    Log::getInstance().d("[CallManager]: no call for cid = "+std::to_string((int)bearer.cid));
  }
}

/**
 * handle get bearers for one data connection with cid
 */
AllocatedBearerResult_t CallManager::handleGetBearerAllocation(int32_t cid)
{
  Log::getInstance().d("[CallManager]: handleGetBearerAllocation = "+std::to_string((int)cid));

  AllocatedBearerResult_t result = {};
  auto matchedCalls = mFilter
      .matchCid(cid)
      .getResults();
  if (!matchedCalls.empty()) {
    AllocatedBearer_t bearer= {};
    CallStateMachine& callInstance = matchedCalls.front();
    const CallInfo& callInfo = callInstance.getCallInfo();
    result.error = ResponseError_t::NO_ERROR;
    bearer.cid = cid;
    if (callInfo.pduSessionParams[0].requestProfiles.size() > 0) {
      bearer.apn = callInfo.pduSessionParams[0].requestProfiles[0].apn;
    }
    bearer.bearers = callInfo.bearerCollection.bearers;
    result.connections.push_back(bearer);
  } else {
    result.error = ResponseError_t::CALL_NOT_AVAILABLE;
  }

  return result;
}

/**
 * handle get all bearer information request
 */
AllocatedBearerResult_t CallManager::handleGetAllBearerAllocations()
{
  Log::getInstance().d("[CallManager]: handleGetAllBearerAllocations");

  AllocatedBearerResult_t result = {};
  for (auto it = mCallInstances.begin(); it != mCallInstances.end(); it++) {
    const CallInfo& callInfo = it->getCallInfo();
    AllocatedBearer_t bearer= {};
    if (callInfo.pduSessionParams[0].requestProfiles.size() > 0) {
      bearer.apn = callInfo.pduSessionParams[0].requestProfiles[0].apn;
    }
    bearer.cid = callInfo.cid;
    bearer.bearers = callInfo.bearerCollection.bearers;
    result.connections.push_back(bearer);
  }
  result.error = ResponseError_t::NO_ERROR;
  return result;
}

/**
 * Updates all bearer type info.
 *
 * If there is at least one client registered for bearer allocation updates
 * through IDataConnection HAL, the bearer list will be sent to the HAL client.
 */
void CallManager::updateAllBearerAllocations()
{
    Log::getInstance().d("[CallManager]: updateAllBearerAllocations");
    if(toggleBearerUpdateToDataConnectionService) {
        // Update to IDataConnnection HAL
        AllocatedBearerResult_t localBearers;
        localBearers.error = ResponseError_t::NO_ERROR;
        for (auto it = mCallInstances.begin(); it != mCallInstances.end(); it++) {
          const CallInfo& callInfo = it->getCallInfo();
          AllocatedBearer_t bearer= {};
          if (callInfo.pduSessionParams[0].requestProfiles.size() > 0) {
            bearer.apn = callInfo.pduSessionParams[0].requestProfiles[0].apn;
          }
          bearer.cid = callInfo.cid;
          bearer.bearers = callInfo.bearerCollection.bearers;
          localBearers.connections.push_back(bearer);
        }
        std::shared_ptr<AllocatedBearerResult_t> bearers = std::make_shared<AllocatedBearerResult_t>(localBearers);
        auto msg = std::make_shared<BearerAllocationUpdateMessage>(bearers);
        msg->broadcast();
    }

    if (mReportPhysicalChannelConfig) {
      updatePhysicalChannelConfigs();
    }
}

/**
 * clean all bearer type info and update
 */
void CallManager::cleanUpAllBearerAllocation()
{
  Log::getInstance().d("[CallManager]: cleanUpAllBearerAllocation");
  for (CallStateMachine& callInstance : mCallInstances) {
    CallInfo& callInfo = callInstance.getCallInfo();
    callInfo.bearerCollection.bearers.clear();
  }
  if(mReportPhysicalChannelConfig) {
    updatePhysicalChannelConfigs();
  }
}

bool CallManager::convertQmiDsdToAvailableRadioDataService(dsd_system_status_info_type_v01 sys,
                                                    RadioDataService_t &radioDataService)
{
  Log::getInstance().d("[CallManager]: convertQmiDsdToAvailableRadioDataService , tech = "+std::to_string((int)sys.technology)+
                        "rat = "+std::to_string((int)sys.rat_value)+
                        ", mask = "+std::to_string((uint64_t)sys.so_mask));
  bool addToAvailableService = true;
  radioDataService.radioFamily = RadioAccessFamily_t::UNKNOWN;
  radioDataService.radioFrequency.range = FrequencyRange_t::LOW;
  radioDataService.radioFrequency.channelNumber = 0;
  radioDataService.isNSA = false;

  switch (sys.technology) {
    case DSD_SYS_NETWORK_3GPP2_V01:
      radioDataService.techType = DataProfileInfoType_t::THREE_GPP2;
      break;
    default:
      radioDataService.techType = DataProfileInfoType_t::THREE_GPP;
  }

  switch (sys.rat_value) {
    case DSD_SYS_RAT_EX_3GPP_WCDMA_V01:
    case DSD_SYS_RAT_EX_3GPP_TDSCDMA_V01:
      if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSDPAPLUS_V01) ||
        (sys.so_mask & QMI_DSD_3GPP_SO_MASK_DC_HSDPAPLUS_V01) ||
        (sys.so_mask & QMI_DSD_3GPP_SO_MASK_64_QAM_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::HSPAP;
      }
      else if(((sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSDPA_V01) && (sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSUPA_V01)) ||
             (sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::HSPA;
      }
      else if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSDPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::HSDPA;
      }
      else if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSUPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::HSUPA;
      }
      else if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_WCDMA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::UMTS;
      }
      else if(sys.rat_value == DSD_SYS_RAT_EX_3GPP_TDSCDMA_V01) {
        radioDataService.radioFamily = RadioAccessFamily_t::TD_SCDMA;
      }
      break;

    case DSD_SYS_RAT_EX_3GPP_GERAN_V01:
      if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_EDGE_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::EDGE;
      }
      else if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_GPRS_V01) ||
              (sys.so_mask & QMI_DSD_3GPP_SO_MASK_GSM_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::GPRS;
      }
      break;

    case DSD_SYS_RAT_EX_3GPP_LTE_V01:
      radioDataService.radioFamily = RadioAccessFamily_t::LTE;
      radioDataService.radioFrequency.range = FrequencyRange_t::MID;
      break;

    case DSD_SYS_RAT_EX_3GPP2_1X_V01:
      if((sys.so_mask & QMI_DSD_3GPP2_SO_MASK_1X_IS2000_REL_A_V01) ||
        (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_1X_IS2000_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::ONE_X_RTT;
      }
      else if((sys.so_mask & QMI_DSD_3GPP2_SO_MASK_1X_IS95_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::IS95A;
      }
      break;

    case DSD_SYS_RAT_EX_3GPP2_HRPD_V01:
      if((sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVB_DPA_V01) ||
        (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVB_MPA_V01) ||
        (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVB_MMPA_V01) ||
        (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVB_EMPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::EVDO_B;
      }
      else if((sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVA_DPA_V01) ||
              (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVA_MPA_V01) ||
              (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVA_EMPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::EVDO_A;
      }
      else if((sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REV0_DPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::EVDO_0;
      }
      break;

    case DSD_SYS_RAT_EX_3GPP2_EHRPD_V01:
      radioDataService.radioFamily = RadioAccessFamily_t::EHRPD;
      break;

    case DSD_SYS_RAT_EX_3GPP_5G_V01:
      radioDataService.radioFamily = RadioAccessFamily_t::NR;
      if( sys.so_mask & QMI_DSD_3GPP_SO_MASK_5G_NSA_V01 ) {
        radioDataService.isNSA = true;
      }
      if(sys.so_mask & QMI_DSD_3GPP_SO_MASK_5G_MMWAVE_V01) {
        radioDataService.radioFrequency.range = FrequencyRange_t::MMWAVE;
      } else {
        if( sys.so_mask & QMI_DSD_3GPP_SO_MASK_5G_NSA_V01 ) {
          radioDataService.radioFrequency.range = FrequencyRange_t::UNKNOWN;
          Log::getInstance().d("Sending frequency range as UNKNOWN for 5G NSA SUB6");
        } else if ( sys.so_mask & QMI_DSD_3GPP_SO_MASK_5G_SA_V01 ) {
          if( mActiveBandInfo != NasActiveBand_t::NGRAN_BAND_MIN_ENUM_VAL ) {
            radioDataService.radioFrequency.range = mNR5GBandToFreqRangeMap[mActiveBandInfo];
            Log::getInstance().d("Sending frequency range as"+
                                 std::to_string((int)radioDataService.radioFrequency.range)+"for 5G SA SUB6");
          } else {
            //Send message to NasModule to send Nas RfBand Info
            //Not blocking the call here for receiving response from NasModule
            auto nasGetRfBandInfoMessage = std::make_shared<NasGetRfBandInfoMessage>();
            if (nasGetRfBandInfoMessage != nullptr)
            {
              GenericCallback<rildata::NasActiveBand_t> cb ([](std::shared_ptr<Message> NasGetRfBandInfoMessage,
                                             Message::Callback::Status status,
                                             std::shared_ptr<rildata::NasActiveBand_t> rsp) -> void {
                if (NasGetRfBandInfoMessage && status == Message::Callback::Status::SUCCESS && rsp)
                {
                  Log::getInstance().d("dispatching NasRfBandInfoMessage");
                  auto indMsg = std::make_shared<NasRfBandInfoMessage>(*rsp);
                  indMsg->broadcast();
                }
                else
                {
                  Log::getInstance().d(" NasGetRfBandInfoMessage response error");
                }
              });
              nasGetRfBandInfoMessage->setCallback(&cb);
              nasGetRfBandInfoMessage->dispatch();
              Log::getInstance().d("mActiveBandInfo is not updated with Band Info from modem"
                                   "Sending UNKNOWN frequency for 5G SA SUB6 Band");
              //Send UNKNOWN as there is no info in cache
              radioDataService.radioFrequency.range = FrequencyRange_t::UNKNOWN;
            }
            else
            {
              Log::getInstance().d("[CallManager]: NasGetPhyChanConfigMessage error");
            }
          }
        } else {
          Log::getInstance().d("Dsd System Status Indication received for 5G RAT"
                               " with so_mask neither as SA nor as NSA!! Sending UNKNOWN as Frequency range");
          //Send UNKNOWN as there is no info in cache
          radioDataService.radioFrequency.range = FrequencyRange_t::UNKNOWN;
        }
      }
      break;

    case DSD_SYS_RAT_EX_3GPP_WLAN_V01:
      if (sys.so_mask & QMI_DSD_3GPP_SO_MASK_S2B_V01) {
        radioDataService.radioFamily = RadioAccessFamily_t::WLAN;
      } else {
        addToAvailableService = false;
      }
      break;

    case DSD_SYS_RAT_EX_3GPP_CIWLAN_V01:
      radioDataService.radioFamily = RadioAccessFamily_t::WLAN;
      break;

    case DSD_SYS_RAT_EX_NULL_BEARER_V01:
    default:
      addToAvailableService = false;
      break;
  }

  return addToAvailableService;
}

void CallManager::processQmiDsdSystemStatusInd(const dsd_system_status_ind_msg_v01 * ind)
{
  Log::getInstance().d("[CallManager]: processQmiDsdSystemStatusInd v1");

  bool radioDataAvailable = false;
  bool radioVoiceAvailable = false;
  mNulBearerReasonAvailable = true;
  bool fiveGAvailable = false;
  bool currentFiveGAvailable = false;

  for (auto rs : globalInfo.serviceStatus.availableRadioDataService) {
    if(rs.radioFamily == RadioAccessFamily_t::NR && !rs.isNSA) {
      currentFiveGAvailable = true;
      break;
    }
  }
  if(ind != nullptr) {
    if (ind->avail_sys_valid) {
      globalInfo.serviceStatus.availableRadioDataService.clear();
      RadioDataService_t radioService = {};
      for(uint32_t i=0 ; i<ind->avail_sys_len ; i++) {
        if (convertQmiDsdToAvailableRadioDataService(ind->avail_sys[i], radioService)) {
          std::stringstream ss;
          ss << "[CallManager]: availableRadioDataService ";
          radioService.dump("", ss);
          Log::getInstance().d(ss.str());
          logBuffer.addLogWithTimestamp(ss.str());
          globalInfo.serviceStatus.availableRadioDataService.push_back(radioService);
          if(radioService.radioFamily == RadioAccessFamily_t::NR && !radioService.isNSA) {
            fiveGAvailable = true;
          }
        }
      }
      if(!globalInfo.serviceStatus.availableRadioDataService.empty()) {
        Log::getInstance().d("[CallManager]: Radio Data Service is available");
        auto rat = globalInfo.serviceStatus.availableRadioDataService.front().radioFamily;
        if (rat != globalInfo.serviceStatus.lastAvailableRadioDataService.radioFamily) {
          globalInfo.serviceStatus.lastAvailableRadioDataService.radioFamily = rat;
          Log::getInstance().d("[CallManager]: Unthrottling apns due to RAT change");
          unthrottleAllRadioApns();
        }
        radioDataAvailable = true;
      }

      if(ind->null_bearer_reason_valid &&
        (ind->null_bearer_reason & DSD_NULL_BEARER_REASON_CS_ONLY_V01)) {
        Log::getInstance().d("[CallManager]: Radio Voice Service is available");
        radioVoiceAvailable = true;
      }

      if (ind->avail_sys_valid && ind->avail_sys_len >= 1) {
        mSaMode = ((ind->avail_sys[0].so_mask & QMI_DSD_3GPP_SO_MASK_5G_SA_V01) == QMI_DSD_3GPP_SO_MASK_5G_SA_V01);
      }

      // Older modem doesn't support NULL bearer So allowing Call if NULL bearer reason is unspecified
      if(!ind->null_bearer_reason_valid)
      {
        Log::getInstance().d("Null Bearer reason is not set");
        mNulBearerReasonAvailable = false;
      }
    }
    if (ind->apn_pref_sys_valid) {
      for(uint32_t i=0 ; i<ind->apn_pref_sys_len ; i++) {
        RadioDataService_t radioService = {};
        if (convertQmiDsdToAvailableRadioDataService(ind->apn_pref_sys[i].curr_pref_sys, radioService)) {
          std::stringstream ss;
          std::string apn = std::string(ind->apn_pref_sys[i].apn_name);
          ss << "[CallManager]: v1 apnPreferredRadioDataService apn=" << apn << " ";
          radioService.dump("", ss);
          Log::getInstance().d(ss.str());
          globalInfo.serviceStatus.apnPreferredRadioDataService[apn] = radioService;
          if (globalInfo.serviceStatus.throttledApns.find(apn) !=
              globalInfo.serviceStatus.throttledApns.end()) {
            ThrottleApn_t throttleApn = globalInfo.serviceStatus.throttledApns[apn];
            if(throttleApn.timer.find((int)RequestSource_t::IWLAN) != throttleApn.timer.end()) {
              if (radioService.radioFamily == RadioAccessFamily_t::WLAN) {
                ThrottleApn_t throttleApn = globalInfo.serviceStatus.throttledApns[apn];
                auto msg = std::make_shared<ThrottledApnTimerExpirationMessage>(throttleApn.profileInfo, RequestSource_t::IWLAN);
                msg->broadcast();
                releaseThrottleTimer(apn, (int)RequestSource_t::IWLAN);
              }
            }
          }
        }
      }
    }
  }

  if(!currentFiveGAvailable && fiveGAvailable) {
      for (CallStateMachine& callInstance : mCallInstances) {
        CallInfo& callInfo = callInstance.getCallInfo();
        // pdu session param look up for all profiles/tds
      for (auto psp : callInfo.pduSessionParams) {
        for (auto rp : psp.requestProfiles) {
          uint16_t txId = allocatePduSessionLookupTxId();
          mPduSessionLookupTxIdTracker[txId] = {
            .cid = callInfo.cid,
            .profile = rp,
          };
          if (rp.trafficDescriptor.has_value())
          {
            wds_endpoint->getPduSessionParamLookup(txId,
                                                   rp.supportedApnTypesBitmap,
                                                   rp.trafficDescriptor.value(),
                                                   rp.matchAllRuleAllowed);
          }
          else {
            wds_endpoint->getPduSessionParamLookup(txId,
                                                   rp.supportedApnTypesBitmap,
                                                   std::nullopt,
                                                   rp.matchAllRuleAllowed);
          }
        }
      }
    }
  }

  if (radioDataAvailable != mRadioDataAvailable) {
    mRadioDataAvailable = radioDataAvailable;
    wds_endpoint->updateDataRegistrationState(mRadioDataAvailable);
    if(mRadioDataAvailable && mReportRegistrationRejectCause) {
      rildata::PLMN_t plmn = {0,0,"",""};
      auto msg = std::make_shared<rildata::DataRegistrationFailureCauseMessage>(plmn, plmn, 0, false);
      msg->broadcast();
    }
  }
  if (radioVoiceAvailable != mRadioVoiceAvailable) {
    mRadioVoiceAvailable = radioVoiceAvailable;
  }

  if (mReportPhysicalChannelConfig) {
    updatePhysicalChannelConfigs();
  }
}

void CallManager::processQmiDsdSystemStatusInd(dsd_apn_avail_sys_info_type_v01 * apn_sys, uint32_t len) {
  Log::getInstance().d("[CallManager]: processQmiDsdSystemStatusInd v2");

  for (int i = 0; i < len; i++) {
    if (apn_sys[i].apn_avail_sys_len > 0) {
      std::string apn = apn_sys[i].apn_name;
      dsd_system_status_info_type_v01 pref_sys_info = apn_sys[i].apn_avail_sys[0];
      RadioDataService_t radioService = {};
      if (convertQmiDsdToAvailableRadioDataService(pref_sys_info, radioService)) {
          std::stringstream ss;
          ss << "[CallManager]: v2 apnPreferredRadioDataService apn=" << apn << " ";
          radioService.dump("", ss);
          Log::getInstance().d(ss.str());
          globalInfo.serviceStatus.apnPreferredRadioDataService[apn] = radioService;
          if (globalInfo.serviceStatus.throttledApns.find(apn) !=
              globalInfo.serviceStatus.throttledApns.end()) {
            ThrottleApn_t throttleApn = globalInfo.serviceStatus.throttledApns[apn];
            if(throttleApn.timer.find((int)RequestSource_t::IWLAN) != throttleApn.timer.end()) {
              if (radioService.radioFamily == RadioAccessFamily_t::WLAN) {
                ThrottleApn_t throttleApn = globalInfo.serviceStatus.throttledApns[apn];
                auto msg = std::make_shared<ThrottledApnTimerExpirationMessage>(throttleApn.profileInfo, RequestSource_t::IWLAN);
                msg->broadcast();
                releaseThrottleTimer(apn, (int)RequestSource_t::IWLAN);
              }
            }
         }
      }
    }
  }
}

void CallManager::generateCurrentPhysicalChannelConfigs()
{
  Log::getInstance().d("[CallManager]: generateCurrentPhysicalChannelConfigs size = " +
                        std::to_string((int)mPhysicalChannelInfo.size()));

  bool fiveGAvailable = false;
  bool primaryServingValid = false;
  bool secondaryServingValid = false;
  RadioAccessFamily_t ratType = RadioAccessFamily_t::UNKNOWN;
  FrequencyRange_t frequencyRange = FrequencyRange_t::LOW;
  PhysicalConfigStructInfo_t fiveGNsaPrimaryServingInfo;
  PhysicalConfigStructInfo_t fiveGNsaSecondaryServingInfo;
  std::vector<PhysicalConfigStructInfo_t> physicalConfigStructInfo;

  for (auto radioService : globalInfo.serviceStatus.availableRadioDataService) {
    if(radioService.radioFamily == RadioAccessFamily_t::NR) {
      fiveGAvailable = true;
      break;
    }
    ratType = radioService.radioFamily;
    frequencyRange = radioService.radioFrequency.range;
  }

  if(fiveGAvailable) {
    for (auto radioService : globalInfo.serviceStatus.availableRadioDataService) {
      if(radioService.radioFamily == RadioAccessFamily_t::NR) {
        secondaryServingValid = true;
        fiveGNsaSecondaryServingInfo.status = CellConnectionStatus_t::SECONDARY_SERVING;
        fiveGNsaSecondaryServingInfo.rat = radioService.radioFamily;
        fiveGNsaSecondaryServingInfo.rfInfo.range = radioService.radioFrequency.range;
        fiveGNsaSecondaryServingInfo.rfInfo.channelNumber = 0;
      }
      else {
        primaryServingValid = true;
        fiveGNsaPrimaryServingInfo.status = CellConnectionStatus_t::PRIMARY_SERVING;
        fiveGNsaPrimaryServingInfo.rat = radioService.radioFamily;
        fiveGNsaPrimaryServingInfo.rfInfo.range = radioService.radioFrequency.range;
        fiveGNsaPrimaryServingInfo.rfInfo.channelNumber = 0;
      }
    }

    int cid;
    std::map<int32_t, rildata::AllocatedBearer_t>::iterator bIterator;
    for (auto cIterator=mCallInstances.begin(); cIterator!=mCallInstances.end(); ++cIterator) {
      const CallInfo& callInfo = cIterator->getCallInfo();
      cid = callInfo.cid;
      // check all allocated bearers to cid
      if (!callInfo.bearerCollection.bearers.empty()) {
        for (auto i=callInfo.bearerCollection.bearers.begin() ; i!=callInfo.bearerCollection.bearers.end() ; ++i) {
          if (i->downlink == RatType_t::RAT_4G) {
            fiveGNsaPrimaryServingInfo.contextIds.push_back(cid);
          }
          else if (i->downlink == RatType_t::RAT_5G) {
            fiveGNsaSecondaryServingInfo.contextIds.push_back(cid);
          }
          else if (i->downlink == RatType_t::RAT_SPLITED) {
            fiveGNsaPrimaryServingInfo.contextIds.push_back(cid);
            fiveGNsaSecondaryServingInfo.contextIds.push_back(cid);
          }
        }
      } else {
        fiveGNsaPrimaryServingInfo.contextIds.push_back(cid);
      }
    }

    if (primaryServingValid) {
      for (NasPhysChanInfo nasinfo : mPhysicalChannelInfo) {
        if(nasinfo.getStatus() == NasPhysChanInfo::Status::PRIMARY) {
          fiveGNsaPrimaryServingInfo.cellBandwidthDownlink = nasinfo.getBandwidth();
          fiveGNsaPrimaryServingInfo.physicalCellId = nasinfo.getPhysicalCellId();
          physicalConfigStructInfo.push_back(fiveGNsaPrimaryServingInfo);
        }
      }
    }

    if (secondaryServingValid) {
      // cellBandwidthDownlink and physicalCellId are not available for 5G NSA
      fiveGNsaSecondaryServingInfo.cellBandwidthDownlink = 0;
      fiveGNsaSecondaryServingInfo.physicalCellId = 0;
      physicalConfigStructInfo.push_back(fiveGNsaSecondaryServingInfo);
    }
  }
  // No 5G case
  else {
    for (NasPhysChanInfo nasinfo : mPhysicalChannelInfo) {
      PhysicalConfigStructInfo_t servingInfo;
      if(nasinfo.getStatus() == NasPhysChanInfo::Status::PRIMARY) {
        servingInfo.status = CellConnectionStatus_t::PRIMARY_SERVING;
      }
      else {
        servingInfo.status = CellConnectionStatus_t::SECONDARY_SERVING;
      }
      servingInfo.cellBandwidthDownlink = nasinfo.getBandwidth();
      servingInfo.rat = ratType;
      servingInfo.rfInfo.range = frequencyRange;
      servingInfo.rfInfo.channelNumber = 0;
      int cid;
      for (auto cIterator=mCallInstances.begin(); cIterator!=mCallInstances.end(); ++cIterator) {
        cid = cIterator->getCallInfo().cid;
        servingInfo.contextIds.push_back(cid);
      }
      servingInfo.physicalCellId = nasinfo.getPhysicalCellId();
      physicalConfigStructInfo.push_back(servingInfo);
    }
  }

  if (physicalConfigStructInfoChanged(physicalConfigStructInfo)) {
    mPhysicalConfigStructInfo.swap(physicalConfigStructInfo);
    auto physConfmsg = std::make_shared<rildata::PhysicalConfigStructUpdateMessage>(mPhysicalConfigStructInfo);
    if (physConfmsg != nullptr) {
        Log::getInstance().d("[CallManager]: " + physConfmsg->dump());
        logBuffer.addLogWithTimestamp("[CallManager]: " + physConfmsg->dump());
        physConfmsg->broadcast();
    }
  }
}

void CallManager::enablePhysChanConfigReporting(bool enable)
{
  Log::getInstance().d("[CallManager]: enabledPhysChanConfigReporting = "+std::to_string(enable));
  mReportPhysicalChannelConfig = enable;
  globalInfo.reportPhysicalChannelConfig = mReportPhysicalChannelConfig;

  auto msg = std::make_shared<rildata::InformPhysicalChannelConfigReportStatus>(enable);
  msg->broadcast();

  if(mReportPhysicalChannelConfig) {
    updatePhysicalChannelConfigs();
  } else {
    mPhysicalConfigStructInfo.clear();
  }
}

void CallManager::handleNasPhysChanConfigMessage(std::shared_ptr<Message> msg)
{
  std::shared_ptr<NasPhysChanConfigMessage> m = std::static_pointer_cast<NasPhysChanConfigMessage>(msg);
  if(m != nullptr) {
    std::shared_ptr<const std::vector<NasPhysChanInfo>> info = m->getInfo();
    if (info != nullptr) {
      mPhysicalChannelInfoValid = true;
      mPhysicalChannelInfo = *info;
    }
    if (mReportPhysicalChannelConfig) {
      updatePhysicalChannelConfigs();
    }
    else {
      Log::getInstance().d("[CallManager]: PhysicalChannelConfig reporting is nullptr or disabled "+
                            std::to_string((int)mReportPhysicalChannelConfig));
    }
  }
}

int getCid(CallEventTypeEnum event, std::shared_ptr<Message> msg) {
  int cid = -1;
  switch (event) {
    case CallEventTypeEnum::DeactivateDataCall:
    {
      auto m = std::static_pointer_cast<DeactivateDataCallRequestMessage>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::RilEventDataCallback:
    {
      auto m = std::static_pointer_cast<RilEventDataCallback>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::TimerExpired:
    {
      auto m = std::static_pointer_cast<DataCallTimerExpiredMessage>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::HandoverInformationInd:
    {
      auto m = std::static_pointer_cast<HandoverInformationIndMessage>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::SetPreferredSystem:
    {
      auto m = std::static_pointer_cast<SetPreferredSystemMessage>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::LinkPropertiesChanged:
    {
      auto m = std::static_pointer_cast<LinkPropertiesChangedMessage>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::ThrottleTimeAvailable:
    {
      auto m = std::static_pointer_cast<GetPdnThrottleTimeResponseInd>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    default:
      Log::getInstance().d("Unable to read cid from call event message");
      break;
  }
  return cid;
}

void CallManager::cleanCallInstance(int cid) {
  availableCallIds[cid] = true;
  for (auto it = mCallInstances.begin(); it != mCallInstances.end(); it++) {
    CallInfo& callInfo = it->getCallInfo();
    if (callInfo.cid == cid) {
      mCallInstances.erase(it);
      break;
    }
  }
}

void CallManager::setProcessDeferredIntentCallback(std::function<void(DeferredIntentToChange_t)> cb) {
  Log::getInstance().d("[CallManager] setProcessDeferredIntentCallback");
  mProcessDeferredIntentCb = cb;
}

bool CallManager::shouldDeferIntentToChange(string apn) {
  bool ret = true;
  list<reference_wrapper<CallStateMachine>> matchedCalls;
  matchedCalls = mFilter
      .matchDnn(apn)
      .matchCallStates({_eConnecting})
      .getResults();
  if (matchedCalls.empty()) {
    ret = false;
  }
  Log::getInstance().d("[CallManager] shouldDeferIntentToChange = "+apn+", ret = "+std::to_string(ret));
  return ret;
}

void CallManager::deferToProcessIntentToChange(DeferredIntentToChange_t deferIntent) {
  string apn(deferIntent.apnPrefSys.apn_pref_info.apn_name);
  Log::getInstance().d("[CallManager] deferToProcessIntentToChange = " + apn);

  list<reference_wrapper<CallStateMachine>> matchedCalls;
  matchedCalls = mFilter
      .matchDnn(apn)
      .matchCallStates({_eConnecting})
      .getResults();
  if(!matchedCalls.empty()) {
    deferredIntents[apn] = deferIntent;
    CallStateMachine& callSM = matchedCalls.front();
    callSM.setCallStateChangedCallback(std::bind(&CallManager::callStateChangedCallback, this, _1, _2));
  }
}

void CallManager::callStateChangedCallback(string apn, CallStateEnum state) {
  Log::getInstance().d("[CallManager] callStateChangedCallback = " + apn + ", state = " + to_string((int)state));
  if(state != _eConnecting) {
    auto it = deferredIntents.find(apn);
    if(it != deferredIntents.end() && mProcessDeferredIntentCb) {
      mProcessDeferredIntentCb(it->second);
      deferredIntents.erase(apn);
    }
  }
}

bool CallManager::validateSetupDataCallParams(std::shared_ptr<SetupDataCallRequestBase> m) {
  HandoffNetworkType_t NetType = (m->getAccessNetwork() == AccessNetwork_t::IWLAN)?
                      HandoffNetworkType_t::_eIWLAN:HandoffNetworkType_t::_eWWAN;
  SetupDataCallResponse_t result = {};
  result.call.suggestedRetryTime = -1;
  result.call.cid = -1;
  // In AP-assist mode, if setupDataCall request with IWLAN on Radio,
  // it is responded with INVALID ARGUMENT
  if(globalInfo.apAssistMode &&
    (((NetType==HandoffNetworkType_t::_eWWAN)&&(m->getRequestSource()==RequestSource_t::IWLAN))||
    ((NetType==HandoffNetworkType_t::_eIWLAN)&&(m->getRequestSource()==RequestSource_t::RADIO)))) {
    Log::getInstance().d("network type mismatch to request source "+std::to_string((int)m->getRequestSource()));
    result.respErr = ResponseError_t::INVALID_ARGUMENT;
    result.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
    auto resp = std::make_shared<SetupDataCallResponse_t>(result);
    stringstream ss;
    ss << "[CallManager]: " << m->getSerial() << "< setupDataCallResponse resp=";
    result.dump("", ss);
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
    m->sendResponse(m, Message::Callback::Status::FAILURE, resp);
    return false;
  }

  if(isApnTypeEmergency(m->getSupportedApnTypesBitmap())) {
    Log::getInstance().d("[CallManager]: Allow EIMS call");
    return true;
  }

  if (m->getRequestSource()==RequestSource_t::RADIO && !mRadioDataAvailable && !mRadioVoiceAvailable && mNulBearerReasonAvailable) {
    Log::getInstance().d("[CallManager]: Reject request because both PS/CS not available");
    result.respErr = ResponseError_t::NO_ERROR;
    result.call.cause = DataCallFailCause_t::DATA_REGISTRATION_FAIL;
    auto resp = std::make_shared<SetupDataCallResponse_t>(result);
    stringstream ss;
    ss << "[CallManager]: " << m->getSerial() << "< setupDataCallResponse resp=";
    result.dump("", ss);
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
    m->sendResponse(m, Message::Callback::Status::SUCCESS, resp);
    return false;
  }
  return true;
}

void CallManager::processEventSendResponse(CallEventTypeEnum event, std::shared_ptr<Message> msg, ResponseError_t errorCode) {
  switch (event) {
    case CallEventTypeEnum::SetupDataCall:
    {
      auto base = std::static_pointer_cast<SetupDataCallRequestBase>(msg);
      if (base != nullptr) {
        SetupDataCallResponse_t result = {};
        result.respErr = errorCode;
        result.call = {.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_4};
        result.call.suggestedRetryTime = -1;
        result.call.cid = -1;
        auto resp = std::make_shared<SetupDataCallResponse_t>(result);
        stringstream ss;
        ss << "[CallManager]: " << base->getSerial() << "< setupDataCallResponse resp=";
        result.dump("", ss);
        Log::getInstance().d(ss.str());
        logBuffer.addLogWithTimestamp(ss.str());
        base->sendResponse(base, Message::Callback::Status::SUCCESS, resp);
      }
      else {
        Log::getInstance().d("[CallManager]: Corrupted SetupDataCallRequestMessage - not able to send response");
      }
      break;
    }
    case CallEventTypeEnum::DeactivateDataCall:
    {
      auto m = std::static_pointer_cast<DeactivateDataCallRequestMessage>(msg);
      if (m != nullptr) {
        ResponseError_t result = errorCode;
        auto resp = std::make_shared<ResponseError_t>(result);
        Log::getInstance().d("[CallManager]: " + std::to_string(m->getSerial()) + "< deactivateDataCallResponse resp=" +
            std::to_string((int)result) + " cid=" + std::to_string(m->getCid()));
        stringstream ss;
        ss << "[CallManager]: " << m->getSerial() << "< deactivateDataCallResponse resp=" << (int)result << " cid=" << m->getCid();
        Log::getInstance().d(ss.str());
        logBuffer.addLogWithTimestamp(ss.str());
        m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
      }
      else {
        Log::getInstance().d("[CallManager]: Corrupted DeactivateDataCallRequestMessage - not able to send response");
      }
      break;
    }
    default:
      Log::getInstance().d("[CallManager]: Ignoring message " + msg->get_message_name());
      break;
  }
}

void CallManager::handleCallEventMessage(CallEventTypeEnum event, std::shared_ptr<Message> msg) {
  CallEventType callEvent = { .type = event, .msg = msg };
  Log::getInstance().d("[CallManager]: Handling " + callEvent.getEventName() + " msg = " + msg->get_message_name());
  list<reference_wrapper<CallStateMachine>> matchedCalls;
  list<reference_wrapper<CallStateMachine>> matchedDefaultCalls;

  if (event == CallEventTypeEnum::SetupDataCall) {
    // Lookup call instance by apn, apn type, and ip type. Create one if no match is found
    std::shared_ptr<SetupDataCallRequestBase> base =
      std::static_pointer_cast<SetupDataCallRequestBase>(msg);

    if( !(dsiInitStatus == DsiInitStatus_t::COMPLETED) ) {
      Log::getInstance().d("[CallManager]: DSI init not yet completed");
      processEventSendResponse(event, msg, ResponseError_t::INTERNAL_ERROR);
      return;
    }

    if (msg != nullptr && base != nullptr && validateSetupDataCallParams(base)) {
      if (isApnTypeEmergency(base->getSupportedApnTypesBitmap())) {
        matchedCalls = mFilter
            .matchApnTypes(ApnTypes_t::EMERGENCY)
            .getResults();
      } else {
        if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
          std::shared_ptr<SetupDataCallRequestMessage_1_6> setup16Msg =
              std::static_pointer_cast<SetupDataCallRequestMessage_1_6>(msg);
          std::pair<uint16_t, bool> result = getDataModule().getPendingMessageList().insert(msg);
          if (setup16Msg == nullptr)
          {
            return;
          }
          auto status = Message::Callback::Status::SUCCESS;
          //Check if call with same params already exists
          if (setup16Msg->getDataRequestReason() == DataRequestReason_t::NORMAL) {
            if (setup16Msg->getOptionalTrafficDescriptor().has_value())
            {
              matchedCalls = mFilter
                  .matchApnTypes(setup16Msg->getSupportedApnTypesBitmap())
                  .matchTrafficDescriptor(setup16Msg->getOptionalTrafficDescriptor().value())
                  .getResults();
            }
            else {
              matchedCalls = mFilter
                  .matchApnTypes(setup16Msg->getSupportedApnTypesBitmap())
                  .getResults();
            }
            //If no matching call, continue with PDU session lookup
            if (matchedCalls.empty())
            {
              uint16_t txId = allocatePduSessionLookupTxId();
              Log::getInstance().d("[CallManager]: handleCallEventMessage txId="+std::to_string(txId)+
                                   " result.first="+std::to_string(result.first));
              mSetupDataCallTxIdTracker[txId]=result.first;
              status = wds_endpoint->getPduSessionParamLookup(txId,
                                                     setup16Msg->getSupportedApnTypesBitmap(),
                                                     setup16Msg->getOptionalTrafficDescriptor(),
                                                     setup16Msg->getMatchAllRuleAllowed());
              if (status == Message::Callback::Status::SUCCESS)
              {
                // continue call bring up with pdu session param lookup result indication
                return;
              }
              else {
                //Fall through
                mSetupDataCallTxIdTracker.erase(txId);
                getDataModule().getPendingMessageList().erase(msg);
                getDataModule().getPendingMessageList().print();
              }
            }
          }
          // if setupDataCall request is for handover or pdu session param lookup responded with failure
          if ((status != Message::Callback::Status::SUCCESS) ||
              (setup16Msg->getDataRequestReason() == DataRequestReason_t::HANDOVER)) {
            // legacy check for existing connection
            //Loose IPType match is needed for the cases when homeIp and
            //roam ip are different and due to which v4v6 profile was created
            matchedCalls = mFilter
              .matchApn(base->getApn())
              .matchApnTypes(base->getSupportedApnTypesBitmap())
              .matchIpTypeLoosely(base->getProtocol())
              .getResults();
          }
        }
        // up to IRadio 1.5, legacy check for existing connection
        else {
          matchedCalls = mFilter
            .matchApn(base->getApn())
            .matchApnTypes(base->getSupportedApnTypesBitmap())
            .matchIpTypeLoosely(base->getProtocol())
            .getResults();
        }
      }

      if (matchedCalls.empty()) {
        int cid = getAvailableCid();
        if (cid >= 0) {
          mCallInstances.emplace_back(cid, logBuffer, globalInfo, util);
          matchedCalls.push_back(mCallInstances.back());
          CallInfo& callInfo = mCallInstances.back().getCallInfo();
          callInfo.pduSessionParams[0].dnnName = base->getApn();
          callInfo.callBringUpCapability = mCallBringupCapability;
          callInfo.radioRAT = RadioAccessFamily_t::UNKNOWN;
          callInfo.wds_endpoint = wds_endpoint;
          callInfo.dsd_endpoint = dsd_endpoint;
          callInfo.dataCallListChangedCallback = std::bind(&CallManager::dataCallListChanged, this);
          callInfo.cleanupKeepAliveCallback = mCleanupKeepAliveCb;
          if(!globalInfo.serviceStatus.availableRadioDataService.empty()) {
              RadioDataService_t radioData = globalInfo.serviceStatus.availableRadioDataService.front();
              callInfo.radioRAT = radioData.radioFamily;
            }
          } else {
              //No valid cid is available.
              SetupDataCallResponse_t result = {};
              result.respErr = ResponseError_t::CALL_NOT_AVAILABLE;
              result.call.cause = DataCallFailCause_t::INSUFFICIENT_RESOURCES;
              result.call.cid = -1;
              auto resp = std::make_shared<SetupDataCallResponse_t>(result);
              Log::getInstance().d("[CallManager]: no valid cid found");
              base->sendResponse(base, Message::Callback::Status::FAILURE, resp);
              dataCallListChanged();

              stringstream ss;
              for (const CallStateMachine& callInstance : mCallInstances) {
                 callInstance.dump("", ss);
              }
              logBuffer.addLogWithTimestamp(ss.str());
          }
        } else if(base->getDataRequestReason() == DataRequestReason_t::NORMAL){
          //!matchedCalls.empty()
          CallStateMachine& callInstance = matchedCalls.front();
          CallInfo& callInfo = callInstance.getCallInfo();
          HandoffNetworkType_t NetType = (base->getAccessNetwork() == AccessNetwork_t::IWLAN)?
                          HandoffNetworkType_t::_eIWLAN:HandoffNetworkType_t::_eWWAN;
          if(callInfo.currentRAT != NetType)
          {
            Log::getInstance().d("[CallManager]: Request APN connection is already exist on another transport");
            SetupDataCallResponse_t result = {};
            result.respErr = ResponseError_t::NO_ERROR;
            result.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_9;
            result.call.suggestedRetryTime = -1;
            result.call.cid = -1;
            auto resp = std::make_shared<SetupDataCallResponse_t>(result);
            base->sendResponse(base, Message::Callback::Status::FAILURE, resp);
            return;
          }
        }
      } else {
        Log::getInstance().d("[CallManager] : No valid datacall is received");
        return;
      }
  } else {
    // Lookup call instance by cid
    int cid = getCid(event, msg);
    matchedCalls = mFilter
        .matchCid(cid)
        .getResults();
  }

  if (!matchedCalls.empty()) {
    // Dispatch event to state machine
    CallStateMachine& callInstance = matchedCalls.front();
    CallInfo& callInfo = callInstance.getCallInfo();
    callInstance.processEvent(callEvent);
    int cid = callInfo.cid;
    int currentState = callInstance.getCurrentState();
    if (currentState == _eDisconnected) {
      cleanCallInstance(cid);
    }

    else if (currentState == _eLatching) {
      Log::getInstance().d("[CallManager]: latch to call with device name "+callInfo.deviceName);
      Message::Callback::Status status = Message::Callback::Status::FAILURE;
      SetupDataCallResponse_t result = {.respErr = ResponseError_t::INTERNAL_ERROR};
      auto setupBaseMsg = std::static_pointer_cast<SetupDataCallRequestBase>(callInfo.pduSessionParams[0].request);
      list<reference_wrapper<CallStateMachine>> matchedDeviceNameCalls = mFilter
                                                                        .matchDeviceName(callInfo.deviceName)
                                                                        .getResults();
      //As The callInfo object for which dsi_net_err_call_exists is received
      //has this device Name, matchedDeviceNameCalls.size()  is always >= 1
     //So trying to search for any other callInfo object with the given devicename
      if (matchedDeviceNameCalls.size() > 1) {
        CallStateMachine& existingCallInstance = matchedDeviceNameCalls.front();
        CallInfo& existingCallInfo = existingCallInstance.getCallInfo();
        Log::getInstance().d("[CallManager]: matchedDeviceNameCalls cid="+std::to_string(existingCallInfo.cid));

        if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
          auto setup16Msg = std::static_pointer_cast<SetupDataCallRequestMessage_1_6>(callInfo.pduSessionParams[0].request);
          if(setup16Msg != nullptr && setup16Msg->getOptionalTrafficDescriptor().has_value()) {
            // child slice add at the end of list
            existingCallInfo.pduSessionParams.insert(existingCallInfo.pduSessionParams.end(),
                                                    callInfo.pduSessionParams.begin(), callInfo.pduSessionParams.end());
          }
          else {
            // master slice add at the beginning of list
            existingCallInfo.pduSessionParams.insert(existingCallInfo.pduSessionParams.begin(),
                                                    callInfo.pduSessionParams.begin(), callInfo.pduSessionParams.end());
          }
        }
        existingCallInfo.callParamsChanged = true;
        std::stringstream ss;
        existingCallInfo.dump("", ss);
        Log::getInstance().d("[CallManager]: existingCallInfo ="+ss.str());
        result.respErr = ResponseError_t::NO_ERROR;
        result.call = convertToDcResult(existingCallInfo);
        status = Message::Callback::Status::SUCCESS;
      } else {
        Log::getInstance().d("[CallManager] : No call found with device name " + callInfo.deviceName);
      }

      if (setupBaseMsg != nullptr) {
        auto resp = std::make_shared<SetupDataCallResponse_t>(result);
        stringstream ss;
        ss << "[CallManager]: " << setupBaseMsg->getSerial() << "< setupDataCallResponse resp=";
        result.dump("", ss);
        Log::getInstance().d(ss.str());
        logBuffer.addLogWithTimestamp(ss.str());
        setupBaseMsg->sendResponse(setupBaseMsg, status, resp);
      }
      else {
        Log::getInstance().d("[CallManager]: Corrupted SetupDataCallRequestMessage - not able to send response");
      }
      cleanCallInstance(cid);
    } // else if (currentState == _eLatching)
    dataCallListChanged();
  } // if (!matchedCalls.empty())
  else {
    Log::getInstance().d("[CallManager]: No call found for = " + msg->dump());
    processEventSendResponse(event, msg, ResponseError_t::CALL_NOT_AVAILABLE);
  }
}

void CallManager::handlePduSessionParamLookupResultIndMessage(std::shared_ptr<Message> msg)
{
  auto m = std::static_pointer_cast<PduSessionParamLookupResultIndMessage>(msg);
  list<reference_wrapper<CallStateMachine>> matchedCalls;
  if(m != nullptr) {
    Log::getInstance().d("[CallManager]: PduSessionParamLookupResultIndMessage txId="+std::to_string(m->getParams().txId));
    auto it = mSetupDataCallTxIdTracker.find(m->getParams().txId);
    if (it != mSetupDataCallTxIdTracker.end()) {
      uint16_t pendingMessageId = mSetupDataCallTxIdTracker[m->getParams().txId];
      Log::getInstance().d("[CallManager]: PduSessionParamLookupResultIndMessage="+m->dump()+
                           " pendingMessageId="+std::to_string(pendingMessageId));
      getDataModule().getPendingMessageList().print();
      auto pendingMsg = getDataModule().getPendingMessageList().extract(pendingMessageId);
      getDataModule().getPendingMessageList().print();

      PduSessionParamLookupResultStatus_t status = m->getParams().status;
      std::string dnnName = m->getParams().dnnName;
      if (pendingMsg) {
        // pdu session param lookup result for setupDataCall
        auto setupMsg = std::static_pointer_cast<SetupDataCallRequestMessage_1_6>(pendingMsg);
        if (status == PduSessionParamLookupResultStatus_t::SUCCESS) {
          matchedCalls = mFilter
          .matchDnn(dnnName)
          .getResults();
        }
        else {
          if (status == PduSessionParamLookupResultStatus_t::NON_SEAMLESS_OFFLOAD ||
              status == PduSessionParamLookupResultStatus_t::NON_DEFAULT_RULE_NOT_MATCHED ||
              status == PduSessionParamLookupResultStatus_t::ROUTE_NOT_ALLOWED) {
            Log::getInstance().d("[CallManager]: PduSessionParamLookupResultStatus is " + std::to_string((int)status));
            SetupDataCallResponse_t result = {};
            result.respErr = ResponseError_t::CALL_NOT_AVAILABLE;
            result.call.suggestedRetryTime = -1;
            result.call.cid = -1;
            result.call.cause = (status == PduSessionParamLookupResultStatus_t::NON_DEFAULT_RULE_NOT_MATCHED)?
                            (DataCallFailCause_t::MATCH_ALL_RULE_NOT_ALLOWED):(DataCallFailCause_t::ALL_MATCHING_RULES_FAILED);
            auto resp = std::make_shared<SetupDataCallResponse_t>(result);
            setupMsg->sendResponse(setupMsg, Message::Callback::Status::SUCCESS, resp);
            return;
          }
          matchedCalls = mFilter
          .matchApn(setupMsg->getApn())
          .matchApnTypes(setupMsg->getSupportedApnTypesBitmap())
          .matchIpType(setupMsg->getProtocol())
          .getResults();
        }
        if (!matchedCalls.empty()) {
          CallStateMachine& callInstance = matchedCalls.front();
          CallInfo& callInfo = callInstance.getCallInfo();

          Log::getInstance().d("[CallManager]: Existing connection cid="+std::to_string(callInfo.cid)+
                                " found for = " + pendingMsg->dump());
          if (globalInfo.dataCallVersion == DataCallVersion_t::RADIO_1_6) {
            auto setup16Msg = std::static_pointer_cast<SetupDataCallRequestMessage_1_6>(pendingMsg);
            if (setup16Msg != nullptr) {
              if(!dnnName.empty()) {
                callInstance.addRequestProfile(dnnName, setup16Msg->getDataProfileInfo());
              } else {
                callInfo.pduSessionParams[0].requestProfiles.push_back(setup16Msg->getDataProfileInfo());
                callInfo.callParamsChanged = true;
              }
            }
          }
          SetupDataCallResponse_t result = {};
          result.respErr = ResponseError_t::NO_ERROR;
          result.call = convertToDcResult(callInfo);
          auto resp = std::make_shared<SetupDataCallResponse_t>(result);
          stringstream ss;
          ss << "[CallManager]: " << setupMsg->getSerial() << "< setupDataCallResponse resp=";
          result.dump("", ss);
          Log::getInstance().d(ss.str());
          logBuffer.addLogWithTimestamp(ss.str());
          setupMsg->sendResponse(setupMsg, Message::Callback::Status::SUCCESS, resp);
          dataCallListChanged();
        } else {
            Log::getInstance().d("[CallManager]: No call found for = " + pendingMsg->dump());
          int cid = getAvailableCid();
          if (cid >= 0) {
            mCallInstances.emplace_back(cid, logBuffer, globalInfo, util);
            matchedCalls.push_back(mCallInstances.back());
            CallInfo& callInfo = mCallInstances.back().getCallInfo();
            callInfo.cid = cid;
            callInfo.radioRAT = RadioAccessFamily_t::UNKNOWN;
            callInfo.wds_endpoint = wds_endpoint;
            callInfo.dsd_endpoint = dsd_endpoint;
            callInfo.dataCallListChangedCallback = std::bind(&CallManager::dataCallListChanged, this);
            callInfo.cleanupKeepAliveCallback = mCleanupKeepAliveCb;

            if (status == PduSessionParamLookupResultStatus_t::SUCCESS) {
              callInfo.callBringUpCapability = BringUpCapability::BRING_UP_APN_NAME;
              callInfo.pduSessionParams[0].dnnName = dnnName;
            }
            else if (setupMsg->getOptionalTrafficDescriptor().has_value() && setupMsg->getOptionalTrafficDescriptor().value().osAppId.has_value()) {
              //ENTERPRISE CALL
              if(setupMsg->getApn().empty()) {
                SetupDataCallResponse_t result = {};
                result.respErr = ResponseError_t::CALL_NOT_AVAILABLE;
                result.call.suggestedRetryTime = -1;
                result.call.cid = -1;
                result.call.cause = DataCallFailCause_t::SLICE_REJECTED;
                auto resp = std::make_shared<SetupDataCallResponse_t>(result);
                setupMsg->sendResponse(setupMsg, Message::Callback::Status::SUCCESS, resp);
                cleanCallInstance(callInfo.cid);
                return;
              } else {
                callInfo.callBringUpCapability = BringUpCapability::BRING_UP_APN_NAME;
                callInfo.pduSessionParams[0].dnnName = setupMsg->getApn();
              }
            }
            else {
              callInfo.callBringUpCapability = mCallBringupCapability;
              callInfo.pduSessionParams[0].dnnName = setupMsg->getApn();
            }
            if(!globalInfo.serviceStatus.availableRadioDataService.empty()) {
              RadioDataService_t radioData = globalInfo.serviceStatus.availableRadioDataService.front();
              callInfo.radioRAT = radioData.radioFamily;
            }
            Log::getInstance().d("[CallManager]: callInfo.cid = "+std::to_string(callInfo.cid)+
            " setupMsg->getApn() = " + setupMsg->getApn() +
            " dnnName = " + callInfo.pduSessionParams[0].dnnName);
            CallStateMachine& callInstance = mCallInstances.back();
            CallEventType callEvent = { .type = CallEventTypeEnum::SetupDataCall, .msg = setupMsg };
            callInstance.processEvent(callEvent);
            int currentState = callInstance.getCurrentState();
            if (currentState == _eDisconnected) {
              processEventSendResponse(CallEventTypeEnum::SetupDataCall, setupMsg, ResponseError_t::CALL_NOT_AVAILABLE);
              cleanCallInstance(cid);
              dataCallListChanged();
            }
          } else {
              //No valid cid is available.
              SetupDataCallResponse_t result = {};
              result.respErr = ResponseError_t::CALL_NOT_AVAILABLE;
              result.call.cause = DataCallFailCause_t::INSUFFICIENT_RESOURCES;
              result.call.cid = -1;
              auto resp = std::make_shared<SetupDataCallResponse_t>(result);
              Log::getInstance().d("[CallManager]: no valid cid found");
              setupMsg->sendResponse(setupMsg, Message::Callback::Status::FAILURE, resp);
              dataCallListChanged();

              stringstream ss;
              for (const CallStateMachine& callInstance : mCallInstances) {
                 callInstance.dump("", ss);
              }
              logBuffer.addLogWithTimestamp(ss.str());
          }
        }
      }
      mSetupDataCallTxIdTracker.erase(m->getParams().txId);
    }
    else {
      // response received for pdu session lookup when moving from 4g to 5g
      auto routeParams = mPduSessionLookupTxIdTracker[m->getParams().txId];
      mPduSessionLookupTxIdTracker.erase(m->getParams().txId);
      matchedCalls = mFilter
        .matchCid(routeParams.cid)
        .getResults();
      if (!matchedCalls.empty()) {
        CallStateMachine& callInstance = matchedCalls.front();
        CallInfo& callInfo = callInstance.getCallInfo();
        // check if dnn name still matches call
        bool dnnNameMatch = false;
        for (auto psp : callInfo.pduSessionParams)
        {
          if (psp.dnnName == m->getParams().dnnName)
          {
            dnnNameMatch = true;
          }
        }
        if (!dnnNameMatch)
        {
          Log::getInstance().d("[CallManager]: dnnName for profile has changed to " + m->getParams().dnnName
                               + " removing it from cid " + std::to_string(routeParams.cid));
          for (auto &psp : callInfo.pduSessionParams) {
            for (auto it=psp.requestProfiles.begin(); it != psp.requestProfiles.end(); )
            {
              if (*it == routeParams.profile)
              {
                stringstream ss;
                it->dump("",ss);
                Log::getInstance().d("[CallManager]: Erasing profile " + ss.str());
                it = psp.requestProfiles.erase(it);
                break;
              }
              else {
                it++;
              }
            }
            if (psp.requestProfiles.empty())
            {
              //deactivate the data call as all TDs are removed
              Log::getInstance().d("[CallManager]: All profiles deleted for dnn " + psp.dnnName);
              CallEventType callEvent = { .type = CallEventTypeEnum::DisconnectAllTDs, .msg = nullptr };
              if(callInstance.processEvent(callEvent) == IState<CallEventType>::HANDLED) {
                cleanCallInstance(routeParams.cid);
                dataCallListChanged();
                return;
              }
            }
          }
        }

        if (mPduSessionLookupTxIdTracker.empty()) {
          callInfo.callParamsChanged = true;
          dataCallListChanged();
        }
      }
    }
  } else {
    Log::getInstance().d("[CallManager]: PduSessionParamLookupResultIndMessage message is NULL");
  }
}

void CallManager::processRouteChangeUpdate(int cid, vector<TrafficDescriptor_t> tdList)
{
  Log::getInstance().d("[CallManager]: processRouteChangeUpdate cid = " + std::to_string(cid));
  auto matchedCalls = mFilter
    .matchCid(cid)
    .getResults();
  if (!matchedCalls.empty()) {
    CallStateMachine& callInstance = matchedCalls.front();
    CallInfo& callInfo = callInstance.getCallInfo();

    for(auto it = tdList.begin(); it != tdList.end(); it++) {
      for (auto& psp : callInfo.pduSessionParams) {
        for (auto rp = psp.requestProfiles.begin(); rp != psp.requestProfiles.end(); ) {
          if(rp->trafficDescriptor.has_value()) {
            if((*it).dnn.has_value() && (*it).osAppId.has_value())
            {
              if(!(rp->trafficDescriptor.value() == *it)) {
                rp++;
                continue;
              }
            }
            else if((*it).dnn.has_value()) {
              if(!rp->trafficDescriptor.value().dnn.has_value() || rp->trafficDescriptor.value().dnn.value() != (*it).dnn.value()) {
                rp++;
                continue;
              }
            } else if((*it).osAppId.has_value()) {
              if((!rp->trafficDescriptor.value().osAppId.has_value() || rp->trafficDescriptor.value().osAppId.value() != (*it).osAppId.value())) {
                rp++;
                continue;
              }
            }
            rp = psp.requestProfiles.erase(rp);
            callInfo.callParamsChanged = true;
          } else {
            rp++;
          }
        }
      }
    }

    if(callInfo.callParamsChanged) {
      dataCallListChanged();
      #ifdef QMI_RIL_UTF
      usleep(5000);
      #endif
      for (auto& psp : callInfo.pduSessionParams) {
        if(psp.requestProfiles.size() != 0) {
          return;
        }
      }
      //All the tds are removed
      CallEventType callEvent = { .type = CallEventTypeEnum::DisconnectAllTDs, .msg = nullptr };
      if(callInstance.processEvent(callEvent) == IState<CallEventType>::HANDLED) {
        int cid = callInfo.cid;
        cleanCallInstance(cid);
        dataCallListChanged();
      }
    }
 }
}

void CallManager::processSetDataInactivityPeriod(std::shared_ptr<Message> msg)
{
  auto m = std::static_pointer_cast<SetDataInActivityPeriodMessage>(msg);
  rildata::ApnTypes_t apnType = m->getApnType();
  Log::getInstance().d("[CallManager]: processSetDataInactivityPeriod: apnType = " + std::to_string((int)apnType));

  for(CallStateMachine& callInstance : mCallInstances) {
    if (callInstance.getCallInfo().pduSessionParams[0].requestProfiles.size() > 0) {
      int lapnTypes = (int)(callInstance.getCallInfo().pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap);
      Log::getInstance().d("[CallManager]: processSetDataInactivityPeriod: existing apnMask = " + std::to_string(lapnTypes));
    }
  }
  auto matchedCalls = mFilter
    .matchApnTypes(apnType)
    .getResults();
  if (!matchedCalls.empty()) {
    CallStateMachine& callInstance = matchedCalls.front();
    bool result = callInstance.sendDataInactivityReq(m->getInterval());
    auto resp = std::make_shared<ResponseError_t>((result ? ResponseError_t::NO_ERROR
                           : ResponseError_t::INTERNAL_ERROR));
    m->sendResponse(m,
          (result ? Message::Callback::Status::SUCCESS:Message::Callback::Status::FAILURE),
          resp);
  } else {
    auto resp = std::make_shared<ResponseError_t>(ResponseError_t::INTERNAL_ERROR);
    m->sendResponse(m,
          Message::Callback::Status::FAILURE,
          resp);
    Log::getInstance().d("[CallManager]: processSetDataInactivityPeriod no Matched Calls");
  }
}

void CallManager::generateCurrentPhysicalChannelConfigsFromBearers()
{
    Log::getInstance().d("[CallManager]: generateCurrentPhysicalChannelConfigsFromBearers size = " +
                          std::to_string((int)mPhysicalChannelInfo.size()));
    bool fiveGAvailable = false;
    RadioAccessFamily_t ratType = RadioAccessFamily_t::UNKNOWN;
    FrequencyRange_t frequencyRange = FrequencyRange_t::LOW;
    std::vector<PhysicalConfigStructInfo_t> physicalConfigStructInfo;

    for (auto radioService : globalInfo.serviceStatus.availableRadioDataService) {
      if(radioService.radioFamily == RadioAccessFamily_t::NR) {
        fiveGAvailable = true;
        break;
      }
      ratType = radioService.radioFamily;
      frequencyRange = radioService.radioFrequency.range;
    }

    if(fiveGAvailable) {
      PhysicalConfigStructInfo_t fiveGNsaPrimaryServingInfo = {};
      bool primaryServingValid = false;
      for (auto radioService : globalInfo.serviceStatus.availableRadioDataService) {
        if(radioService.radioFamily == RadioAccessFamily_t::NR) {
          primaryServingValid = true;
          fiveGNsaPrimaryServingInfo.status = CellConnectionStatus_t::PRIMARY_SERVING;
          fiveGNsaPrimaryServingInfo.rat = radioService.radioFamily;
          fiveGNsaPrimaryServingInfo.rfInfo.range = radioService.radioFrequency.range;
          fiveGNsaPrimaryServingInfo.rfInfo.channelNumber = 0;
        }
      }

      for (auto cIterator=mCallInstances.begin(); cIterator!=mCallInstances.end(); ++cIterator) {
        // find cid from call list
        const CallInfo& callInfo = cIterator->getCallInfo();
        int cid = callInfo.cid;
        // check all allocated bearers to cid
        for (auto i=callInfo.bearerCollection.bearers.begin() ; i!=callInfo.bearerCollection.bearers.end() ; ++i) {
          if (i->downlink == RatType_t::RAT_5G) {
            fiveGNsaPrimaryServingInfo.contextIds.push_back(cid);
          }
        }
      }
      physicalConfigStructInfo.push_back(fiveGNsaPrimaryServingInfo);
    }
    // No 5G case
    else {
      PhysicalConfigStructInfo_t primaryServingInfo = {.status = CellConnectionStatus_t::PRIMARY_SERVING};
      PhysicalConfigStructInfo_t secondaryServingInfo = {.status = CellConnectionStatus_t::SECONDARY_SERVING};
      for (auto cIterator=mCallInstances.begin(); cIterator!=mCallInstances.end(); ++cIterator) {
        int cid = cIterator->getCallInfo().cid;
        primaryServingInfo.contextIds.push_back(cid);
        secondaryServingInfo.contextIds.push_back(cid);
      }
      physicalConfigStructInfo.push_back(primaryServingInfo);
      physicalConfigStructInfo.push_back(secondaryServingInfo);
    }

    if (physicalConfigStructInfoChanged(physicalConfigStructInfo)) {
      mPhysicalConfigStructInfo.swap(physicalConfigStructInfo);
      auto physConfmsg = std::make_shared<rildata::PhysicalConfigStructUpdateMessage>(mPhysicalConfigStructInfo);
      if (physConfmsg != nullptr) {
        Log::getInstance().d("[CallManager]: " + physConfmsg->dump());
        logBuffer.addLogWithTimestamp("[CallManager]: " + physConfmsg->dump());
        physConfmsg->broadcast();
      }
    }
}

bool CallManager::physicalConfigStructInfoChanged(const std::vector<PhysicalConfigStructInfo_t>& configs) {
  if (configs.size() != mPhysicalConfigStructInfo.size()) {
    return true;
  }
  return !std::equal(mPhysicalConfigStructInfo.begin(), mPhysicalConfigStructInfo.end(), configs.begin(),
    [](const PhysicalConfigStructInfo_t& oldConfigs, const PhysicalConfigStructInfo_t& newConfigs) {
      if (oldConfigs.status != newConfigs.status ||
          oldConfigs.cellBandwidthDownlink != newConfigs.cellBandwidthDownlink ||
          oldConfigs.rat != newConfigs.rat ||
          oldConfigs.rfInfo.range != newConfigs.rfInfo.range ||
          oldConfigs.physicalCellId != newConfigs.physicalCellId ||
          oldConfigs.contextIds.size() != newConfigs.contextIds.size()) {
        return false;
      }

      return std::equal(oldConfigs.contextIds.begin(), oldConfigs.contextIds.end(), newConfigs.contextIds.begin());
  });
}

void CallManager::updatePhysicalChannelConfigs()
{
  Log::getInstance().d("[CallManager]: updatePhysicalChannelConfigs");
  if (mSaMode) {
    generateCurrentPhysicalChannelConfigsFromBearers();
  } else {
    if (mPhysicalChannelInfoValid) {
      generateCurrentPhysicalChannelConfigs();
    } else {
      auto nasGetPhyChanConfigMsg = std::make_shared<NasGetPhyChanConfigMessage>();
      if (nasGetPhyChanConfigMsg != nullptr) {
        GenericCallback<std::vector<NasPhysChanInfo>> cb([](std::shared_ptr<Message> nasGetPhyChanConfigMsg,
                                                      Message::Callback::Status status,
                                                      std::shared_ptr<std::vector<NasPhysChanInfo>> rsp) -> void {
          if (nasGetPhyChanConfigMsg && rsp) {
            if(status == Message::Callback::Status::SUCCESS) {
              Log::getInstance().d("NasPhysChanInfo size = " + std::to_string((int)rsp->size()));
              auto indMsg = std::make_shared<NasPhysChanConfigMessage>(rsp);
              indMsg->broadcast();
            }
            else {
              Log::getInstance().d("[CallManager]: NasGetPhyChanConfigMessage response error");
            }
          }
          if(rsp == nullptr) {
            Log::getInstance().d("[CallManager]: PhysicalChannelConfig reporting is nullptr");
          }
        });
        nasGetPhyChanConfigMsg->setCallback(&cb);
        nasGetPhyChanConfigMsg->dispatch();
      }
      else {
        Log::getInstance().d("[CallManager]: NasGetPhyChanConfigMessage error");
      }
    }
  }
}

int CallManager::toggleLinkActiveStateChangeReport(bool report)
{
  Log::getInstance().d("[CallManager]: toggleLinkActiveStateChangeReport "+std::to_string((int)report));
  if (globalInfo.linkStateChangeReport != report) {
    globalInfo.linkStateChangeReport = report;
    for (auto it=mCallInstances.begin(); it!=mCallInstances.end(); ++it) {
      it->updateLinkStateChangeReport();
    }
  }

  return 0;
}

bool CallManager::handleGoDormantRequest(std::string deviceName)
{
  bool status = false;
  Log::getInstance().d("[CallManager]: goDormant DeviceName "+ deviceName);
  if (!deviceName.empty()) {
    auto matchedCalls = mFilter
        .matchDeviceName(deviceName)
        .getResults();
    if (!matchedCalls.empty()) {
      Log::getInstance().d("[CallManager]: CallInstance Found with "+ deviceName);
      CallStateMachine& callInstance = matchedCalls.front();
      status = callInstance.sendDormancyRequest();
    } else {
      Log::getInstance().d("[CallManager]: No CallInstance Found for "+ deviceName);
    }
  } else {
     for (auto it=mCallInstances.begin(); it!=mCallInstances.end(); ++it) {
      status = it->sendDormancyRequest();
     }
  }
  return status;
}

void CallManager::registerDataRegistrationRejectCause(bool enable)
{
  Log::getInstance().d("[CallManager]: registerDataRegistrationRejectCause "+std::to_string(enable));
  mReportRegistrationRejectCause = enable;
}

void CallManager::disconnectCallFromOlderDDSSub()
{
  Log::getInstance().d("[CallManager]: disconnectCallFromOlderDDSSub");
  auto matchedCalls = mFilter
      .matchApnTypesLoosely(ApnTypes_t::DEFAULT)
      .getResults();
  if(!matchedCalls.empty()) {
    CallStateMachine& callInstance = matchedCalls.front();
    CallEventType callEvent = { .type = CallEventTypeEnum::DisconnectCallFromOlderDDSSub, .msg = nullptr };
    if(callInstance.processEvent(callEvent) == IState<CallEventType>::HANDLED) {
      CallInfo& callInfo = callInstance.getCallInfo();
      int cid = callInfo.cid;
      cleanCallInstance(cid);
    }
  }
}

CallManager::Filter& CallManager::Filter::matchCid(int cid) {
  mCriteriaBitmask |= Criteria::CID;
  mCid = cid;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchApn(std::string apn) {
  mCriteriaBitmask |= Criteria::APN;
  mApn = apn;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchApnTypes(ApnTypes_t apnTypes) {
  mCriteriaBitmask |= Criteria::APN_TYPES;
  mApnTypes = apnTypes;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchApnTypesLoosely(ApnTypes_t apnTypes) {
  mCriteriaBitmask |= Criteria::LOOSE_APN_TYPES;
  mApnTypes = apnTypes;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchIpType(std::string ipType) {
  mCriteriaBitmask |= Criteria::IP_TYPE;
  mIpType = ipType;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchIpTypeLoosely(std::string ipType) {
  mCriteriaBitmask |= Criteria::LOOSE_IP_TYPE;
  mIpType = ipType;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchDeviceName(std::string deviceName) {
  mCriteriaBitmask |= Criteria::DEVICE_NAME;
  mDeviceName = deviceName;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchCurrentNetwork(HandoffNetworkType_t currentNetwork) {
  mCriteriaBitmask |= Criteria::CURRENT_NETWORK;
  mCurrentNetwork = currentNetwork;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchCallStates(const std::set<int>& states) {
  mCriteriaBitmask |= Criteria::STATES;
  mStates = states;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchDnn(std::string dnn) {
  mCriteriaBitmask |= Criteria::DNN;
  mDnn = dnn;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchTrafficDescriptor(TrafficDescriptor_t trafficDescriptor) {
  mCriteriaBitmask |= Criteria::TRAFFIC_DESCRIPTOR;
  mTrafficDescriptor = trafficDescriptor;
  return *this;
}

bool CallManager::Filter::matchesCriteria(CallStateMachine& call) {
  const CallInfo& callInfo = call.getCallInfo();
  for (uint32_t matchType = 1; matchType < Criteria::CRITERIA_MAX; matchType <<= 1) {
    if ((matchType & mCriteriaBitmask) == matchType) {
      switch (matchType) {
        case Criteria::CID:
          if(mCid != callInfo.cid) {
            return false;
          }
          break;
        case Criteria::APN:
          if (callInfo.pduSessionParams[0].requestProfiles.size() == 0)
          {
            return false;
          }
          if (mApn.length() != callInfo.pduSessionParams[0].requestProfiles[0].apn.length())
          {
            return false;
          }
          if(!std::equal(mApn.begin(), mApn.end(), callInfo.pduSessionParams[0].requestProfiles[0].apn.begin(), []
              (auto a, auto b){return std::tolower(a)==std::tolower(b);})) {
            return false;
          }
          break;
        case Criteria::APN_TYPES:
          if (callInfo.pduSessionParams[0].requestProfiles.size() == 0)
          {
            return false;
          }
          if (static_cast<uint32_t>(mApnTypes) != (uint32_t)callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap) {
            return false;
          }
          break;
        case Criteria::LOOSE_APN_TYPES:
          if (callInfo.pduSessionParams[0].requestProfiles.size() == 0)
          {
            return false;
          }
          if ((static_cast<uint32_t>(mApnTypes) & (uint32_t)callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap) !=
               static_cast<uint32_t>(mApnTypes)) {
            return false;
          }
          break;
        case Criteria::IP_TYPE:
          if (callInfo.pduSessionParams[0].requestProfiles.size() == 0)
          {
            return false;
          }
          if (mIpType != callInfo.pduSessionParams[0].requestProfiles[0].protocol) {
            return false;
          }
          break;
        case Criteria::LOOSE_IP_TYPE:
          if(callInfo.pduSessionParams[0].requestProfiles[0].protocol == "IPV4V6") {
            if(mIpType == "IP" || mIpType == "IPV6" || mIpType == "IPV4V6") {
              return true;
            }
          } else if(mIpType == callInfo.pduSessionParams[0].requestProfiles[0].protocol) {
            return true;
          } else {
            return false;
          }
          break;
        case Criteria::DEVICE_NAME:
          if (mDeviceName.length() != callInfo.deviceName.length()) {
            return false;
          }
          if(!std::equal(mDeviceName.begin(), mDeviceName.end(), callInfo.deviceName.begin(), []
              (auto a, auto b){return std::tolower(a)==std::tolower(b);})) {
            return false;
          }
          break;
        case Criteria::CURRENT_NETWORK:
          if (mCurrentNetwork != (mCurrentNetwork & callInfo.currentRAT)) {
            return false;
          }
          break;
        case Criteria::STATES:
          if (mStates.find(call.getCurrentState()) == mStates.end()) {
            return false;
          }
          break;
        case Criteria::DNN:
          {
            Log::getInstance().d("[CallManager::Filter]: DNN="+mDnn);
            for (const auto& psp : callInfo.pduSessionParams) {
              Log::getInstance().d("[CallManager::Filter]: psp.dnnName="+psp.dnnName);
              if (psp.dnnName == mDnn) {
                return true;
              }
            }
            return false;
          }
          break;
        case Criteria::TRAFFIC_DESCRIPTOR:
          {
            for (const auto& psp : callInfo.pduSessionParams) {
              Log::getInstance().d("[CallManager::Filter]: psp.dnnName="+psp.dnnName);

              for (auto rp : psp.requestProfiles)
              {
                if (rp.trafficDescriptor.has_value())
                {
                  if (rp.trafficDescriptor.value().getDnn() == mTrafficDescriptor.getDnn() &&
                        rp.trafficDescriptor.value().getOsAppId() == mTrafficDescriptor.getOsAppId()) return true;
                }
              }
            }
            return false;
          }
          break;
        default:
          Log::getInstance().d("[CallManager::Filter]: invalid criteria");
          return false;
      }
    }
  }
  return true;
}

list<reference_wrapper<CallStateMachine>> CallManager::Filter::getResults() {
  std::list<reference_wrapper<CallStateMachine>> filtered(mCallList.begin(), mCallList.end());
  if (mCriteriaBitmask == 0) {
    return filtered;
  }

  filtered.remove_if([this](CallStateMachine& call) -> bool {
    return !matchesCriteria(call);
  });
  clearCriteria();
  return filtered;
}

void CallManager::Filter::clearCriteria() {
  mCriteriaBitmask = 0;
  mCid = 0;
  mApn = "";
  mApnTypes = ApnTypes_t::NONE;
  mIpType = "";
  mDeviceName = "";
  mCurrentNetwork = HandoffNetworkType_t::_eWWAN;
  mDnn = "";
}

void CallManager::dsiInitCbFunc(void *userData)
{
  (void)userData;
  Log::getInstance().d("Received DSI Init callback");
  auto msg = std::make_shared<DsiInitCompletedMessage>();
  if( msg != NULL ) {
    msg->broadcast();
  }
}

void CallManager::dsiInitCb2Func(dsi_init_mode_t mode, void *userData)
{
  (void)userData;
  (void) mode;
  Log::getInstance().d("Received DSI Init ex2 callback");
  auto msg = std::make_shared<DsiInitCompletedMessage>();
  if( msg != NULL ) {
    msg->broadcast();
  }
}

void CallManager::triggerDsiInit(bool afterBootup)
{
  if( dsiInitStatus == DsiInitStatus_t::RELEASED )
  {
    if (!isDsiex2Supported) {
      if (DsiWrapper::getInstance().dsiInit(!afterBootup, dsiInitCbFunc) == DSI_SUCCESS) {
        dsiInitStatus = DsiInitStatus_t::STARTED;
        Log::getInstance().d("[CallManager]: DSI Init Successful");
      } else {
        Log::getInstance().d("[CallManager]: DSI Init Failed");
      }
    } else {
      if (DsiWrapper::getInstance().dsiInit(dsiInitCb2Func) == DSI_SUCCESS) {
        dsiInitStatus = DsiInitStatus_t::STARTED;
        Log::getInstance().d("[CallManager]: DSI Init2 Successful");
      } else {
        Log::getInstance().d("[CallManager]: DSI Init2 Failed");
      }
    }
  } else {
    Log::getInstance().d("[CallManager]: DSI Init is already Started/Progress/Completed");
  }
}

void CallManager::triggerDsiRelease()
{
  if( (dsiInitStatus == DsiInitStatus_t::COMPLETED) || (dsiInitStatus == DsiInitStatus_t::PENDING_RELEASE) )
  {
    if( DsiWrapper::getInstance().dsiRelease() == DSI_SUCCESS ) {
      Log::getInstance().d("[CallManager]: DSI Release Successful");
      dsiInitStatus = DsiInitStatus_t::RELEASED;
    } else {
      Log::getInstance().d("[CallManager]: DSI Release Failed");
      dsiInitStatus = DsiInitStatus_t::PENDING_RELEASE;
    }
  } else {
    dsiInitStatus = DsiInitStatus_t::PENDING_RELEASE;
    Log::getInstance().d("[CallManager]: DSI Init not yet completed. Ignore DSI release");
  }
}

void CallManager::dsiInitCompleted() {
  DsiWrapper::getInstance().dsiSetSSCCapability();
  if (dsiInitStatus == DsiInitStatus_t::COMPLETED) {
    Log::getInstance().d("[CallManager]: DSI Init Cb Received after NetMgrd Restarts");
    auto matchedCalls =  mFilter
       .matchCallStates({_eIdle})
       .getResults();
    for (auto it : matchedCalls) {
      CallStateMachine& callInstance = it.get();
      CallInfo &callInfo = callInstance.getCallInfo();
      auto m = callInfo.pduSessionParams[0].request;
      if( m != nullptr) {
        Log::getInstance().d("Found Pending Setupdatacall Message");
        CallEventType callEvent = { .type = CallEventTypeEnum::SetupDataCall, .msg = m };
        callInstance.processEvent(callEvent);
      }
    }
  }
  dsiInitStatus = DsiInitStatus_t::COMPLETED;
}

void CallManager::sendFailureForIdleCalls() {
  Log::getInstance().d("[CallManager] : sending failure for the calls in Idle state");
  auto matchedCalls =  mFilter
       .matchCallStates({_eIdle})
       .getResults();
  for (auto it : matchedCalls) {
      CallStateMachine& callInstance = it.get();
      CallInfo &callInfo = callInstance.getCallInfo();
      CallEventType callEvent = { .type = CallEventTypeEnum::CleanUp, .msg = nullptr };
      callInstance.processEvent(callEvent);
      cleanCallInstance(callInfo.cid);
  }
}

void CallManager::cleanAllCalls()
{
  Log::getInstance().d("[CallManager]: cleanAllCalls");
  for (auto it = mCallInstances.begin(); it != mCallInstances.end();) {
    auto it_save = it;
    it++;
    CallEventType callEvent = { .type = CallEventTypeEnum::CleanUp, .msg = nullptr };
    it_save->processEvent(callEvent);
    CallInfo& callInfo = it_save->getCallInfo();
    availableCallIds[callInfo.cid] = true;
    mCallInstances.erase(it_save);
  }
}

void CallManager::cleanCallsInSecureMode()
{
  Log::getInstance().d("[CallManager]: cleanCallsInSecureMode");
  for (auto it = mCallInstances.begin(); it != mCallInstances.end();) {
    auto it_save = it;
    it++;
    CallInfo& callInfo = it_save->getCallInfo();
    if ((callInfo.pduSessionParams[0].requestProfiles.size() > 0) &&
      ((uint32_t)(callInfo.pduSessionParams[0].requestProfiles[0].supportedApnTypesBitmap) & (uint32_t)(ApnTypes_t::EMERGENCY)))
    {
      Log::getInstance().d("[CallManager]: Emergency call found on cid : " + std::to_string(callInfo.cid));
      continue;
    }
    CallEventType callEvent = { .type = CallEventTypeEnum::CleanUp, .msg = nullptr };
    it_save->processEvent(callEvent);
    availableCallIds[callInfo.cid] = true;
    mCallInstances.erase(it_save);
  }
  dataCallListChanged();
}

void CallManager::updateNasRfActiveBandInfo(std::shared_ptr<Message> msg)
{
   std::shared_ptr<NasRfBandInfoIndMessage> m = std::static_pointer_cast<NasRfBandInfoIndMessage>(msg);
  if(m != nullptr) {
    mActiveBandInfo = static_cast<rildata::NasActiveBand_t>(m->getNasRFActiveBand());
    Log::getInstance().d("[CallManager]: Received NAS RF Active Band Info"+std::to_string((int32_t)mActiveBandInfo));
  } else {
    Log::getInstance().d("[CallManager]: NasPhysChanConfigMessage message is NULL");
  }
}

void CallManager::handleNasRfBandInfoMessage(std::shared_ptr<Message> msg)
{
   std::shared_ptr<NasRfBandInfoMessage> m = std::static_pointer_cast<NasRfBandInfoMessage>(msg);
  if(m != nullptr) {
    mActiveBandInfo = static_cast<NasActiveBand_t>(m->getRfBandInfo());
    Log::getInstance().d("[CallManager]: NasRfBandInfoMessage Key"
                         +std::to_string((int32_t)mActiveBandInfo) +"not present in mNR5GBandToFreqRangeMap ");
  } else {
    Log::getInstance().d("[CallManager]: NasRfBandInfoMessage message is NULL");
  }
}
