/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/Log.h>
#include <framework/MessageContext.h>
#include <utils/StrongPointer.h>

class QcRilAudioContext : public MessageContext {
 private:
 public:
  QcRilAudioContext(qcril_instance_id_e_type instance_id, uint32_t serial)
      : MessageContext(instance_id, serial) {
  }

  ~QcRilAudioContext() {
  }

  std::string toString() {
    return std::string("IQcRilAudio(") + std::to_string(serial) + std::string(")");
  }
};
