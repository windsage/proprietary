/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#include <unistd.h>
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include "TmdManager.h"

using namespace rildata;

TmdManager::TmdManager()
{
  Log::getInstance().d("[TmdManager]: TmdManager");
  tmd_endpoint = ModemEndPointFactory<TMDModemEndPoint>::getInstance().buildEndPoint();
}

void TmdManager::cancelTimer(std::string device)
{
  Log::getInstance().d("[TmdManager]: cancelTimer="+device);
  if (deviceMitigationLevel[device].timerId != TimeKeeper::no_timer) {
    TimeKeeper::getInstance().clear_timer(deviceMitigationLevel[device].timerId);
    deviceMitigationLevel[device].interval = 0;
    deviceMitigationLevel[device].timerId = TimeKeeper::no_timer;
  }
}

void TmdManager::startTimer(std::string device, uint8_t target, TimeKeeper::millisec timeout)
{
  #ifdef QMI_RIL_UTF
  timeout = 200;
  #endif
  auto handler = std::bind(&TmdManager::timeoutHandler, this, device, std::placeholders::_1);
  TimeKeeper::timer_id tId = TimeKeeper::getInstance().set_timer(handler, nullptr, timeout);
  deviceMitigationLevel[device].timerId = tId;
  deviceMitigationLevel[device].target = target;
  deviceMitigationLevel[device].interval = timeout;
  Log::getInstance().d("[TmdManager]: startTimer timerId="+std::to_string(tId)+
                       ", target="+std::to_string(target)+
                       ", timeout="+std::to_string(timeout)+", device="+device);
}

void TmdManager::timeoutHandler(std::string device, void *data)
{
  std::ignore = data;
  Log::getInstance().d("[TmdManager]: timeoutHandler="+device);

  if (deviceMitigationLevel[device].target > deviceMitigationLevel[device].requested) {
    deviceMitigationLevel[device].requested++;
    tmd_endpoint->setMitigationLevel(device, deviceMitigationLevel[device].requested);
  }
  else if (deviceMitigationLevel[device].target < deviceMitigationLevel[device].requested) {
    deviceMitigationLevel[device].requested--;
    tmd_endpoint->setMitigationLevel(device, deviceMitigationLevel[device].requested);
  }
  deviceMitigationLevel[device].timerId = TimeKeeper::no_timer;

  if(deviceMitigationLevel[device].target != deviceMitigationLevel[device].requested) {
    startTimer(device, deviceMitigationLevel[device].target, deviceMitigationLevel[device].interval);
  }
}

void TmdManager::getMitigationDeviceList()
{
  Log::getInstance().d("[TmdManager]: getMitigationDeviceList");
  int rc;
  tmd_get_mitigation_device_list_resp_msg_v01 resp;
  memset(&resp, 0, sizeof(resp));

  rc = ModemEndPointFactory<TMDModemEndPoint>::getInstance().buildEndPoint()->sendRawSync(
                    QMI_TMD_GET_MITIGATION_DEVICE_LIST_REQ_V01,
                    nullptr, 0,
                    &resp, sizeof(resp));
  if( rc != QMI_NO_ERR || (resp.resp.result != QMI_RESULT_SUCCESS_V01))
  {
    Log::getInstance().d("[TmdManager]: failed with rc=" + std::to_string(rc) +
    ", qmi_err=" + std::to_string(resp.resp.error));
  }

  if (rc == QMI_NO_ERR && resp.mitigation_device_list_valid)
  {
    for (uint32_t i=0 ; i<resp.mitigation_device_list_len ; i++) {
      std::string devId(resp.mitigation_device_list[i].mitigation_dev_id.mitigation_dev_id);
      auto it = deviceMitigationLevel.find(devId);
      if(it != deviceMitigationLevel.end()) {
        it->second.maxLevel = resp.mitigation_device_list[i].max_mitigation_level;
        Log::getInstance().d("[TmdManager]: device=" + devId + ", max=" +std::to_string(it->second.maxLevel));
      }
    }
  }
}

void TmdManager::getCurrentMitigationLevel(std::string device)
{
  Log::getInstance().d("[TmdManager]: getCurrentMitigationLevel device="+device);
  DeviceMitigationLevel_t level = {};
  level = tmd_endpoint->getCurrentMitigationLevel(device);
  deviceMitigationLevel[device].current = level.current;
  deviceMitigationLevel[device].requested = level.requested;
  Log::getInstance().d("[TmdManager]: current="+std::to_string(deviceMitigationLevel[device].current)+
                       " ,requested="+std::to_string(deviceMitigationLevel[device].requested));
}

void TmdManager::setDataThrottling(DataThrottleAction_t action, int64_t durationMills)
{
  Log::getInstance().d("[TmdManager]: setDataThrottlingAction");
  for (auto device : deviceMitigationLevel) {
    cancelTimer(device.first);
  }
  for (auto device : deviceMitigationLevel) {
    getCurrentMitigationLevel(device.first);
  }

  switch(action) {
    case DataThrottleAction_t::NoDataThrottle:
      {
        for (auto device : deviceMitigationLevel) {
          device.second.target = 0;
          if (device.second.target < device.second.requested) {
            device.second.interval = durationMills / (device.second.requested-device.second.target);
            startTimer(device.first, device.second.target, device.second.interval);
          }
        }
      }
      break;
    case DataThrottleAction_t::ThrottleSecondaryCarrier:
      {
        for (auto device : deviceMitigationLevel) {
          if(device.second.carrier == DataThrottlingDevice_t::ANCHOR_CARRIER) {
            device.second.target = 0;
            if (device.second.target < device.second.requested) {
              device.second.interval = durationMills / (device.second.requested-device.second.target);
            }
          }
          else {  // DataThrottlingDevice_t::SECONDARY_CARRIER
            device.second.target = device.second.maxLevel;
            if (device.second.target > device.second.requested) {
              device.second.interval = durationMills / (device.second.target-device.second.requested);
            }
          }
          if(device.second.target != device.second.requested) {
            startTimer(device.first, device.second.target, device.second.interval);
          }
        }
      }
      break;
    case DataThrottleAction_t::ThrottleAnchorCarrier:
      {
        for (auto device : deviceMitigationLevel) {
          device.second.target = device.second.maxLevel;
          if (device.second.target > device.second.requested) {
            device.second.interval = durationMills / (device.second.target-device.second.requested);
          }
          if(device.second.target != device.second.requested) {
            startTimer(device.first, device.second.target, device.second.interval);
          }
        }
      }
      break;
    case DataThrottleAction_t::Hold:
    default:
      {
        for (auto device : deviceMitigationLevel) {
          device.second.target = device.second.current;
          if (device.second.target>device.second.requested) {
            device.second.interval = durationMills / (device.second.target-device.second.requested);
          }
          else if (device.second.target<device.second.requested) {
            device.second.interval = durationMills / (device.second.requested-device.second.target);
          }
          if(device.second.target != device.second.requested) {
            startTimer(device.first, device.second.target, device.second.interval);
          }
        }
      }
      break;
  }
}
