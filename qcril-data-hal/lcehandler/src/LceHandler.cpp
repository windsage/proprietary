/*===========================================================================

  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifdef FEATURE_DATA_LQE
#include "LceHandler.h"
#include "framework/Log.h"
#include "sync/ToggleLqeIndicationSyncMessage.h"
#include "sync/SetCapReportCriteriaSyncMessage.h"
#include "UnSolMessages/LceInitMessage.h"
#include "UnSolMessages/LceDeInitMessage.h"
#include "over_the_top_v01.h"

using namespace rildata;

LceHandler::LceHandler()
{
  lqeInited = false;
  lqeParams = {};
}

void LceHandler::deInit()
{
  auto msg = std::make_shared<LceDeInitMessage>();
  if (msg) {
    msg->broadcast();
  }
  lqeInited = false;
}

void LceHandler::Init()
{
  if (lqeInited == false) {
    auto initMsg = std::make_shared<LceInitMessage>();
    if (initMsg) {
      initMsg->broadcast();
    }
    lqeInited = true;
    Log::getInstance().d("Lqe Inited Successfully");
    Log::getInstance().d("Threshold-based reporting is supported!");
    auto msg = std::make_shared<ToggleLqeIndicationSyncMessage>(nullptr);
    auto r = std::make_shared<int>();
    if (msg) {
      Message::Callback::Status apiStatus = Message::Callback::Status::FAILURE;
      msg->setParams(LqeDataDirection::LQE_DATA_UPLINK_DIRECTION, LqeIndicationSwitch::LQE_TURN_ON_INDICATIONS);
      apiStatus = msg->dispatchSync(r);
      if (apiStatus == Message::Callback::Status::SUCCESS) {
        Log::getInstance().d("Indication Turned on Successfully for Uplink Direction");
        lqeParams.ul_report_enabled = 1;
      }
      else {
        lqeParams.ul_report_enabled = 0;
        Log::getInstance().d("Turning On Indication for Uplink Direction Failed");
      }
    }
    else {
      Log::getInstance().d("Turning On Indication for Uplink Direction Failed");
    }
    msg = std::make_shared<ToggleLqeIndicationSyncMessage>(nullptr);
    if (msg) {
      Message::Callback::Status apiStatus = Message::Callback::Status::FAILURE;
      msg->setParams(LqeDataDirection::LQE_DATA_DOWNLINK_DIRECTION, LqeIndicationSwitch::LQE_TURN_ON_INDICATIONS);
      r = std::make_shared<int>();
      apiStatus = msg->dispatchSync(r);
      if (apiStatus == Message::Callback::Status::SUCCESS) {
        lqeParams.dl_report_enabled = 1;
        Log::getInstance().d("Indication Turned on Successfully for Downlink Direction");
      }
      else {
        lqeParams.dl_report_enabled = 0;
        Log::getInstance().d("Turning On Indication for Downlink Direction Failed");
      }
    }
    else {
      Log::getInstance().d("Turning On Indication for Downlink Direction Failed");
    }
  }
  else
  {
    Log::getInstance().d("Lqe is already Inited");
  }
}

bool LceHandler::toggleReporting(int enableBit)
{
  if (lqeInited == false) {
    Log::getInstance().d("Lqe is not Inited");
    return false;
  }
  if (enableBit == 0) {
    if (lqeParams.dl_report_enabled == enableBit && lqeParams.ul_report_enabled == enableBit) {
      Log::getInstance().d("Indications are already disabled");
      return true;
    }
    if (lqeParams.ul_report_enabled != enableBit)
    {
      auto msg = std::make_shared<ToggleLqeIndicationSyncMessage>(nullptr);
      auto r = std::make_shared<int>();
      if (msg) {
        Message::Callback::Status apiStatus = Message::Callback::Status::FAILURE;
        msg->setParams(LqeDataDirection::LQE_DATA_UPLINK_DIRECTION, LqeIndicationSwitch::LQE_TURN_OFF_INDICATIONS);
        apiStatus = msg->dispatchSync(r);
        if (apiStatus == Message::Callback::Status::SUCCESS) {
          Log::getInstance().d("Indication Turned off Successfully for Uplink Direction");
          lqeParams.ul_report_enabled = 0;
        }
        else {
          Log::getInstance().d("Turning Off Indication for Uplink Direction Failed");
        }
      }
    }
    if (lqeParams.dl_report_enabled != enableBit)
    {
      auto msg = std::make_shared<ToggleLqeIndicationSyncMessage>(nullptr);
      auto r = std::make_shared<int>();
      if (msg) {
        Message::Callback::Status apiStatus = Message::Callback::Status::FAILURE;
        msg->setParams(LqeDataDirection::LQE_DATA_DOWNLINK_DIRECTION, LqeIndicationSwitch::LQE_TURN_OFF_INDICATIONS);
        apiStatus = msg->dispatchSync(r);
        if (apiStatus == Message::Callback::Status::SUCCESS) {
          Log::getInstance().d("Indication Turned off Successfully for Downlink Direction");
          lqeParams.dl_report_enabled = 0;
        }
        else {
          Log::getInstance().d("Turning Off Indication for Downlink Direction Failed");
        }
      }
    }
    if (lqeParams.dl_report_enabled == enableBit && lqeParams.ul_report_enabled == enableBit) {
      Log::getInstance().d("Indications are Disabled Successfully");
      return true;
    }
  }
  else
  {
    if (lqeParams.dl_report_enabled == enableBit && lqeParams.ul_report_enabled == enableBit) {
      Log::getInstance().d("Indications are already enabled");
      return true;
    }
    if (lqeParams.ul_report_enabled != enableBit)
    {
      auto msg = std::make_shared<ToggleLqeIndicationSyncMessage>(nullptr);
      auto r = std::make_shared<int>();
      if (msg) {
        Message::Callback::Status apiStatus = Message::Callback::Status::FAILURE;
        msg->setParams(LqeDataDirection::LQE_DATA_UPLINK_DIRECTION, LqeIndicationSwitch::LQE_TURN_ON_INDICATIONS);
        apiStatus = msg->dispatchSync(r);
        if (apiStatus == Message::Callback::Status::SUCCESS) {
          Log::getInstance().d("Indication Turned on Successfully for Uplink Direction");
          lqeParams.ul_report_enabled = 1;
        }
        else {
          Log::getInstance().d("Turning On Indication for Uplink Direction Failed");
        }
      }
    }
    if (lqeParams.dl_report_enabled != enableBit)
    {
      auto msg = std::make_shared<ToggleLqeIndicationSyncMessage>(nullptr);
      auto r = std::make_shared<int>();
      if (msg) {
        Message::Callback::Status apiStatus = Message::Callback::Status::FAILURE;
        msg->setParams(LqeDataDirection::LQE_DATA_DOWNLINK_DIRECTION, LqeIndicationSwitch::LQE_TURN_ON_INDICATIONS);
        apiStatus = msg->dispatchSync(r);
        if (apiStatus == Message::Callback::Status::SUCCESS) {
          Log::getInstance().d("Indication Turned ON Successfully for Downlink Direction");
          lqeParams.dl_report_enabled = 1;
        }
        else {
          Log::getInstance().d("Turning On Indication for Downlink Direction Failed");
        }
      }
    }
    if (lqeParams.dl_report_enabled == enableBit && lqeParams.ul_report_enabled == enableBit) {
      Log::getInstance().d("Indications are Enabled Successfully");
      return true;
    }
  }
  return false;
}

LinkCapCriteriaResult_t LceHandler::setCriteria(LinkCapCriteria_t criteria)
{
  if (lqeInited == false) {
    Log::getInstance().d("Lqe is not Inited");
    return LinkCapCriteriaResult_t::internal_err;
  }
  if ((criteria.ran != AccessNetwork_t::UTRAN) &&
    (criteria.ran != AccessNetwork_t::EUTRAN) &&
    (criteria.ran != AccessNetwork_t::NGRAN)) {
    Log::getInstance().d("[ SetCriteria ]: RAN " + std::to_string((int)criteria.ran) +
       " not supported by modem for bandwidth reporting!");
    return LinkCapCriteriaResult_t::request_not_supported;
  }
  if (criteria.thresholdsDownlinkKbps.size() >= QMI_OTT_DL_THRESHOLD_LIST_LEN_V01 ||
    criteria.thresholdsUplinkKbps.size() >= QMI_OTT_UL_THRESHOLD_LIST_LEN_V01) {
    Log::getInstance().d("[ SetCriteria ]: threshold list size out of bounds! downlink=" +
       std::to_string(criteria.thresholdsDownlinkKbps.size()) + ", uplink=" +
       std::to_string(criteria.thresholdsUplinkKbps.size()));
    return LinkCapCriteriaResult_t::internal_err;
  }
  std::sort(criteria.thresholdsUplinkKbps.begin(), criteria.thresholdsUplinkKbps.end());
  std::sort(criteria.thresholdsDownlinkKbps.begin(), criteria.thresholdsDownlinkKbps.end());
  Log::getInstance().d("RAN = " + std::to_string(static_cast<int>(criteria.ran)));
  Log::getInstance().d("HysteresisMs = " + std::to_string(criteria.hysteresisMs));
  Log::getInstance().d("HysteresisDlKbps = " + std::to_string(criteria.hysteresisDlKbps));
  Log::getInstance().d("HysteresisUlKbps = " + std::to_string(criteria.hysteresisUlKbps));
  for (int i = 0; i < criteria.thresholdsDownlinkKbps.size(); i++) {
    Log::getInstance().d("ThresholdsDownlinkKbps [" + std::to_string(i) + "] = " + std::to_string(criteria.thresholdsDownlinkKbps[i]));
  }
  for (int i = 0; i < criteria.thresholdsUplinkKbps.size(); i++) {
    Log::getInstance().d("ThresholdsUplinkKbps [" + std::to_string(i) + "] = " + std::to_string(criteria.thresholdsUplinkKbps[i]));
  }
  if (criteria.thresholdsDownlinkKbps.size() >= 2) {
    int32_t delta = criteria.thresholdsDownlinkKbps[1] - criteria.thresholdsDownlinkKbps[0];
    for (int i = 2 ; i < criteria.thresholdsDownlinkKbps.size() ; i++) {
      delta = MIN(delta, criteria.thresholdsDownlinkKbps[i] - criteria.thresholdsDownlinkKbps[i-1]);
    }
    if (criteria.hysteresisDlKbps >= delta) {
      Log::getInstance().d("HysteresisDlKbps must be smaller than the smallest UL threshold delta!");
      return LinkCapCriteriaResult_t::invalid_arguments;
    }
  }
  if (criteria.thresholdsUplinkKbps.size() >= 2) {
    int32_t delta = criteria.thresholdsUplinkKbps[1] - criteria.thresholdsUplinkKbps[0];
    for (int i = 2 ; i < criteria.thresholdsUplinkKbps.size() ; i++) {
      delta = MIN(delta, criteria.thresholdsUplinkKbps[i]-criteria.thresholdsUplinkKbps[i-1]);
    }
    if (criteria.hysteresisUlKbps >= delta) {
      Log::getInstance().d("HysteresisUlKbps must be smaller than the smallest UL threshold delta!");
      return LinkCapCriteriaResult_t::invalid_arguments;
    }
  }
  auto msg = std::make_shared<SetCapReportCriteriaSyncMessage>(nullptr);
  auto r = std::make_shared<int>();
  if (msg) {
    Message::Callback::Status apiStatus = Message::Callback::Status::FAILURE;
    msg->setParams(criteria);
    apiStatus = msg->dispatchSync(r);
    if (apiStatus == Message::Callback::Status::SUCCESS) {
      Log::getInstance().d("SetCapReportCriteria is Successful");
      lqeParams.criteria_list_valid = true;
      lqeParams.criteria_list[criteria.ran] = criteria;
    }
    else {
      Log::getInstance().d("SetCapReportCriteria Failed");
      return LinkCapCriteriaResult_t::internal_err;
    }
  }
  return LinkCapCriteriaResult_t::success;
}

void LceHandler::handlemodemSSR()
{
  Log::getInstance().d("Resetting Criteria on Modem after Modem SSR");
  if (lqeInited && lqeParams.criteria_list_valid) {
    for (auto it = lqeParams.criteria_list.begin(); it != lqeParams.criteria_list.end(); it++) {
      setCriteria(it->second);
    }
  }
}

void LceHandler::Release()
{
  auto msg = std::make_shared<LceDeInitMessage>();
  if (msg) {
    msg->broadcast();
  }
  lqeInited = false;
  lqeParams.ul_report_enabled = 0;
  lqeParams.dl_report_enabled = 0;
  lqeParams.criteria_list.clear();
  lqeParams.criteria_list_valid = false;
}
#endif