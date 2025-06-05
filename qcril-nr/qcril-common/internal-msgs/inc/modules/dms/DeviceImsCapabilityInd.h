/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <modules/dms/GetDeviceImsCapabilityMessage.h>

class DeviceImsCapabilityInd : public UnSolicitedMessage,
                               public add_message_id<DeviceImsCapabilityInd> {
public:
  static constexpr const char *MESSAGE_NAME = "DeviceImsCapabilityInd";

  DeviceImsCapabilityInd(DeviceImsCapability &deviceCap)
      : UnSolicitedMessage(get_class_message_id()), mDeviceImsCap(deviceCap) {
  }

  DeviceImsCapabilityInd() = delete;
  ~DeviceImsCapabilityInd() = default;

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<DeviceImsCapabilityInd>(mDeviceImsCap));
  }

  string dump() {
    return DeviceImsCapabilityInd::MESSAGE_NAME;
  }

  const DeviceImsCapability &getDeviceImsCapability() const {
    return mDeviceImsCap;
  }

private:
  DeviceImsCapability mDeviceImsCap;
};
