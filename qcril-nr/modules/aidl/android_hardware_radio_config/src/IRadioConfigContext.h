/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/Log.h>
#include <framework/MessageContext.h>

class IRadioConfigContext : public MessageContext {
 public:
  IRadioConfigContext(qcril_instance_id_e_type instance_id, uint32_t serial)
      : MessageContext(instance_id, serial) {}

  ~IRadioConfigContext() {}

  std::string toString() {
    return std::string("IRadioConfig(") + std::to_string(serial) + std::string(")");
  }
};
