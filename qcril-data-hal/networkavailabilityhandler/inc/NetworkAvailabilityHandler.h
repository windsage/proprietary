/*===========================================================================

  Copyright (c) 2018, 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef NETWORKAVAILABILITYHANDLER
#define NETWORKAVAILABILITYHANDLER

#include "framework/Log.h"
#include "framework/Message.h"

#include "data_system_determination_v01.h"

#include "DataCommon.h"
#include "UnSolMessages/DsdSystemStatusMessage.h"
#include "UnSolMessages/PreferredApnUpdatedMessage.h"
#include "MessageCommon.h"
#include "WDSModemEndPoint.h"
#include "DSDModemEndPoint.h"
#include "LocalLogBuffer.h"
#include "modules/qmi/ModemEndPointFactory.h"

namespace rildata {

struct ConsolidatedNetwork_t {
  bool hasPendingIntentToChange;
  vector<int32_t> apnTypes;
  vector<int32_t> networks;
  void dump(string padding, std::ostream& os);
};

struct ApnTypeInfo_t {
  vector<int32_t> networks;
  uint16_t tx_id;
  void dump(string padding, std::ostream& os);
};

/**
 * @brief NetworkAvailabilityHandler class
 * @details NetworkAvailabilityHandler class
 */
class NetworkAvailabilityHandler {
public:
  NetworkAvailabilityHandler(LocalLogBuffer& logBuffer);
  ~NetworkAvailabilityHandler();

  // QMI indication process functions
  void processSetDataProfileRequest(std::shared_ptr<Message> msg);
  void processQmiDsdSystemStatusInd(dsd_apn_avail_sys_info_type_v01 * apn_sys, uint32_t len);
  void processQmiDsdIntentToChangeApnPrefSysInd(dsd_intent_to_change_apn_pref_sys_ind_msg_v01 *ind_data);
  void processQmiDsdPreferredApnUpdated(std::shared_ptr<PreferredApnUpdatedMessage> msg);
  void getQualifiedNetworks(vector<QualifiedNetwork_t> &qualifiedNetworks);
  void setGlobalPreferredSystem(dsd_system_status_info_type_v01 globalPreferredSystem);
  void processHandoverfailureForApnType(ApnTypes_t type, string apn);
  void clearNetworkAvailabilityCache();
  static std::string convertToString(const AccessNetwork_t &ran);
  static std::string convertToString(const ApnTypes_t &apnType);
  static std::string convertToString(const QualifiedNetwork_t &qualNetwork);
  static std::string convertToString(const vector<int32_t> apnTypes, const vector<int32_t> qNetworks);
  void dump(string padding, std::ostream& os);
  void updateNetworkAvailabilityCache(string dnnName, ApnTypes_t apnType) {
    ConsolidatedNetwork_t network = {false, convertToRadioApnTypeList(apnType), vector<int32_t>()};
    mNetworkAvailabilityCache[dnnName] = network;
  }

private:
  LocalLogBuffer& logBuffer;
  std::shared_ptr<DSDModemEndPoint> dsd_endpoint;
  std::unordered_map<std::string, ConsolidatedNetwork_t> mNetworkAvailabilityCache;
  std::unordered_map<int32_t,ApnTypeInfo_t> mLastReportedNetworkAvailability;
  AccessNetwork_t globalPrefNw;
  void updateNetworkAvailabilityCache(std::shared_ptr<Message> msg);
  void updateNetworkAvailabilityCache(dsd_apn_avail_sys_info_type_v01 *, uint32_t);
  void updateNetworkAvailabilityCache(dsd_apn_pref_sys_type_ex_v01 *, uint32_t);
  AccessNetwork_t convertToRadioAccessNetwork(dsd_system_status_info_type_v01 );
  vector<int32_t> convertToRadioAccessNetworkList(dsd_apn_avail_sys_info_type_v01 );
  int32_t convertToRadioApnType(uint64_t qmiApnType);
  vector<int32_t> convertToRadioApnTypeList(const ApnTypes_t &apnTypes);
  uint64_t convertToDsdApnType(int32_t radioApnType);
  void dumpCache();
  vector<int32_t> convertResultList(vector<QualifiedNetwork_t> &resultList,
                         vector<QualifiedNetwork_t> &qualifiedNetworks);

  void processConflictingApnTypes(vector<int32_t> conflictingApnTypes);
  bool isWlanRAT(int network);
  bool isRadioRAT(int network);
  bool isSameRadioFamily(int prioNetwork, int newNetwork);
};

} /* namespace rildata */

#endif
