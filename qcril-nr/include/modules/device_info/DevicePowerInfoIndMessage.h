/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

class DevicePowerInfoIndMessage : public UnSolicitedMessage,
        public add_message_id<DevicePowerInfoIndMessage> {
  private:
    bool mReportDevicePowerInfo;
    std::vector<uint32_t> mBatteryThresholdData;

  public:
    static constexpr const char *MESSAGE_NAME = "DEVICE_POWER_INFO_INDICATION";
    DevicePowerInfoIndMessage() : UnSolicitedMessage(get_class_message_id()) {
      mName = MESSAGE_NAME;
    }

    ~DevicePowerInfoIndMessage() = default;

    DevicePowerInfoIndMessage(bool reportDevicePowerInfo, std::vector<uint32_t> thresholdData)
            : UnSolicitedMessage(get_class_message_id()) {
      mReportDevicePowerInfo = reportDevicePowerInfo;
      mBatteryThresholdData = thresholdData;
      mName = MESSAGE_NAME;
      dump();
    }

    std::shared_ptr<UnSolicitedMessage> clone() {
      std::shared_ptr<DevicePowerInfoIndMessage> msg =
              std::make_shared<DevicePowerInfoIndMessage>();
      if (msg) {
        msg->setReportDevicePowerInfo(mReportDevicePowerInfo);
        msg->setBatteryThresholdData(mBatteryThresholdData);
      }
      return msg;
    }

    void setReportDevicePowerInfo(bool reportDevicePowerInfo) {
      mReportDevicePowerInfo = reportDevicePowerInfo;
    }

    void setBatteryThresholdData(std::vector<uint32_t> thresholdData) {
      mBatteryThresholdData = thresholdData;
    }

    string dump() {
      std::string str;
      for (auto data : mBatteryThresholdData) {
        str.append(std::to_string(data));
        str.append(" ");
      }
      return mName + "{" + " Report device power info: " +
              std::to_string(mReportDevicePowerInfo) + " " + str + " }";
    }

    bool isDevicePowerInfoSupported() {
      return mReportDevicePowerInfo;
    }

    std::vector<uint32_t> getBatteryThresholdData() {
      return mBatteryThresholdData;
    }

};
