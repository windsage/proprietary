/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "device_management_service_v01.h"
#ifdef USE_MUSL
#include <optional>
#endif
struct DeviceImsCapability {
  std::optional<uint8_t> max_ims_instances;
  std::vector<dms_ims_capability_type_v01> ims_capability;
};

/*===========================================================================*/
/*!
    @brief
    Sets the Audio Parameters with AM client.

    mString       - Audio Parameters
*/
/*==========================================================================*/
class GetDeviceImsCapabilityMessage : public SolicitedSyncMessage<DeviceImsCapability>,
                                      public add_message_id<GetDeviceImsCapabilityMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "GetDeviceImsCapabilityMessage";

  GetDeviceImsCapabilityMessage()
      : SolicitedSyncMessage<DeviceImsCapability>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  ~GetDeviceImsCapabilityMessage() = default;

  inline string dump() {
    return mName;
  }
};
