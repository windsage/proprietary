/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef RIL_EMER_NUM_FROM_PBM
#define RIL_EMER_NUM_FROM_PBM

#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/**
 * Message to check if the emergency number is received from PBM
 */

class RilEmerNumFromPbm : public SolicitedSyncMessage<bool>,
                          public add_message_id<RilEmerNumFromPbm> {
 private:
  std::string number;

 public:
  static constexpr const char* MESSAGE_NAME = "QCRIL_IS_EMERGENCY_NUM_FROM_PBM";

  inline RilEmerNumFromPbm() : SolicitedSyncMessage<bool>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  ~RilEmerNumFromPbm();
  void setNumber(const std::string& number);
  std::string getNumber();

  string dump();
};

#endif  // RIL_EMER_NUM_FROM_PBM
