/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#ifndef THERMALMITIGATIONDEVICEMANAGER
#define THERMALMITIGATIONDEVICEMANAGER

#include <string>
#include "DataCommon.h"
#include "TMDModemEndPoint.h"
#include "request/SetDataThrottlingRequestMessage.h"
#include "framework/TimeKeeper.h"

namespace rildata {

class TmdManager {
 private:
  std::unordered_map<std::string, DeviceMitigationLevel_t> deviceMitigationLevel = {
    {"modem_lte_dsc",     {DataThrottlingDevice_t::ANCHOR_CARRIER,    0, 0, 0, 0, TimeKeeper::no_timer, 0}},
    {"modem_nr_dsc",      {DataThrottlingDevice_t::ANCHOR_CARRIER,    0, 0, 0, 0, TimeKeeper::no_timer, 0}},
    {"modem_nr_scg_dsc",  {DataThrottlingDevice_t::SECONDARY_CARRIER, 0, 0, 0, 0, TimeKeeper::no_timer, 0}},
  };
  std::shared_ptr<TMDModemEndPoint> tmd_endpoint;

 public:
  TmdManager();
  ~TmdManager(){}

  void cancelTimer(std::string device);
  void startTimer(std::string device, uint8_t target, TimeKeeper::millisec timeout);
  void timeoutHandler(std::string device, void *data);
  void getMitigationDeviceList();
  void getCurrentMitigationLevel(std::string device);
  void setDataThrottling(DataThrottleAction_t action, int64_t durationMills);
};
} // namespace

#endif
