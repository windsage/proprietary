/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "utils/StrongPointer.h"

#include "framework/Log.h"
#include "framework/MessageContext.h"

class RadioDataContext : public MessageContext {
 public:
  RadioDataContext(qcril_instance_id_e_type instance_id, uint32_t serial)
      : MessageContext(instance_id, serial) {}

  ~RadioDataContext() {}

  std::string toString() {
    return std::string("RadioData(") + std::to_string(serial) + std::string(")");
  }
};
