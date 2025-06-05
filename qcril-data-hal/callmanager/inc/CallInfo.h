/*===========================================================================

  Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef CALLINFO
#define CALLINFO

#include <stdint.h>
#include <set>
#include <unordered_map>

#include "WDSModemEndPoint.h"
#include "DSDModemEndPoint.h"
#include "WDSCallModemEndPoint.h"

#include "DsiWrapper.h"
#include "IProfileKey.h"

#define SuggestedRetryBackoffTime -1; // no value is suggested
#define DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(t) ((t == RadioAccessFamily_t::ONE_X_RTT)|| \
                                                (t == RadioAccessFamily_t::IS95A) || \
                                                (t == RadioAccessFamily_t::EVDO_B) || \
                                                (t == RadioAccessFamily_t::EVDO_A) || \
                                                (t == RadioAccessFamily_t::EVDO_0) )

#define DATA_IS_RIL_RADIO_TECH_3GPP(t) ((t == RadioAccessFamily_t::GPRS) || \
                                        (t == RadioAccessFamily_t::EDGE) || \
                                        (t == RadioAccessFamily_t::UMTS)|| \
                                        (t == RadioAccessFamily_t::LTE)|| \
                                        (t == RadioAccessFamily_t::HSPAP)|| \
                                        (t == RadioAccessFamily_t::HSDPA) || \
                                        (t == RadioAccessFamily_t::HSUPA) || \
                                        (t == RadioAccessFamily_t::HSPA) || \
                                        (t == RadioAccessFamily_t::GSM) || \
                                        (t == RadioAccessFamily_t::TD_SCDMA) || \
                                        (t == RadioAccessFamily_t::LTE_CA) || \
                                        (t == RadioAccessFamily_t::NR))

#define MIN_V6_MTU 1280

using namespace std;

namespace rildata {

struct DsiEventDataType {
  dsi_net_evt_t netEvent;
  void *eventData;
  std::string getEventName() const;
};

class CallEndReason {
public:
  static DataCallFailCause_t getCallEndReason(dsi_ce_reason_t dsiReason);
  static bool isPermanentFailure(DataCallFailCause_t rilReason);
  static constexpr int AP_ASSIST_HANDOVER = -1;

private:
  CallEndReason();
  typedef unordered_map<int, DataCallFailCause_t> CallEndReasonMap_t;
  static unordered_map<dsi_ce_reason_type_t, CallEndReasonMap_t> callEndReasonTypeMap;
  static CallEndReasonMap_t mipReasonMap;
  static CallEndReasonMap_t internalReasonMap;
  static CallEndReasonMap_t cmReasonMap;
  static CallEndReasonMap_t _3gppReasonMap;
  static CallEndReasonMap_t pppReasonMap;
  static CallEndReasonMap_t ehrpdReasonMap;
  static CallEndReasonMap_t ipv6ReasonMap;
  static CallEndReasonMap_t handoffReasonMap;
  static CallEndReasonMap_t apAssistHandoffReasonMap;
};

enum class DataCallVersion_t : int {
  UNDETERMINED,
  RADIO_1_4,
  RADIO_1_6,
};

struct PduSessionParam_t {
  string dnnName;
  std::shared_ptr<qdp::IProfileKey> profileKey3gpp;
  std::shared_ptr<qdp::IProfileKey> profileKey3gpp2;
  bool pendingAck;
  std::shared_ptr<Message> request;
  vector<DataProfileInfo_t> requestProfiles;
  void dump(std::string padding, std::ostream& os) const {
    os << padding << "{" << dnnName << ",";
    for (auto rp : requestProfiles) {
      rp.dump("requestProfile=",os);
    }
    os << ", pk3gpp=";
    if(profileKey3gpp!=nullptr) {
      os << profileKey3gpp->dump();
    }
    os << ", pk3gpp2=";
    if(profileKey3gpp2!=nullptr) {
      os << profileKey3gpp2->dump();
    }
    os << ", pa=" << std::boolalpha << pendingAck;
    os <<"}";
  }
};

struct GlobalCallInfo {
  bool linkStateChangeReport;
  bool partialRetryEnabled;
  unsigned long maxPartialRetryTimeout;
  bool isRoaming;
  bool apAssistMode;
  bool reportPhysicalChannelConfig;
  RadioServiceStatus_t serviceStatus;
  DataCallVersion_t dataCallVersion;
};

/**
 * @brief Call information class
 * @details All related information for one data call is maintained
 * This object is created when a data call bring up is requested and deleted
 * when its call is terminated
  */
struct CallInfo {
  int cid;
  LinkActiveState consolidatedActive;
  bool v4Connected;
  bool v6Connected;
  bool dualIpHandover;
  HandOverState v4HandedOver;
  HandOverState v6HandedOver;
  BringUpCapability callBringUpCapability;
  HandoffNetworkType_t currentRAT;
  HandoffNetworkType_t preferredRAT;
  RadioAccessFamily_t radioRAT;
  string deviceName;
  string ipType;
  int32_t mtu;
  int32_t consolidatedMtuV4;
  int32_t consolidatedMtuV6 = 0;
  bool callParamsChanged;
  DataCallFailCause_t cause;
  bool roamingAllowed;
  std::list<PduInfo> pduInfoList;
  unordered_map<DataCallTimerType, TimeKeeper::timer_id> timers;
  dsi_hndl_t dsiHandle;

  std::vector<PduSessionParam_t> pduSessionParams;
  AllocatedBearer_t bearerCollection;

  std::unique_ptr<WDSCallModemEndPoint> wds_v4_call_endpoint;
  std::unique_ptr<WDSCallModemEndPoint> wds_v6_call_endpoint;
  std::shared_ptr<WDSModemEndPoint> wds_endpoint;
  std::shared_ptr<DSDModemEndPoint> dsd_endpoint;
  std::function<void()> dataCallListChangedCallback;
  vector<QosSession_t> qosSessions;
  void dump(string padding, ostream& os) const;
  string convertToLegacyLinkAddresses() const;
  vector<LinkAddress_t> convertAddrStringToLinkAddresses() const;
  string convertToResultDnsAddresses() const;
  void calculateConsolidatedMtu();
  void calculateConsolidatedActive();

  std::function<void(int)> cleanupKeepAliveCallback;
};

} /* namespace rildata */

#endif
