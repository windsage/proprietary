/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/Log.h>
#include <framework/MessageContext.h>

class IQtiRadioConfigContext : public MessageContext {
 public:
  IQtiRadioConfigContext(qcril_instance_id_e_type instance_id, uint32_t serial)
      : MessageContext(instance_id, serial) {}

  ~IQtiRadioConfigContext() {}

  std::string toString() {
    return std::string("IQtiRadioConfig(") + std::to_string(serial) + std::string(")");
  }
};
