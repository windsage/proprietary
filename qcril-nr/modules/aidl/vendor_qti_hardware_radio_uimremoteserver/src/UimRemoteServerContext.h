/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Log.h>
#include <framework/MessageContext.h>

class UimRemoteServerContext : public MessageContext {
 public:
  UimRemoteServerContext(qcril_instance_id_e_type instance_id, int32_t serial)
      : MessageContext(instance_id, serial)
  {
  }

  ~UimRemoteServerContext() = default;

  std::string toString()
  {
    return std::string("UimRemoteServerContext(") + std::to_string(serial) + std::string(")");
  }
};
