/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Log.h>
#include <framework/MessageContext.h>

class UimLpaServiceContext : public MessageContext {
 public:
  UimLpaServiceContext(qcril_instance_id_e_type instance_id, int32_t serial)
      : MessageContext(instance_id, serial)
  {
  }

  ~UimLpaServiceContext() = default;

  std::string toString()
  {
    return std::string("UimLpaServiceContext(") + std::to_string(serial) + std::string(")");
  }
};
