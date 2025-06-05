/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

struct PbmNetworkDetectedEccNumber {
  std::string number;
  uint32_t emerg_srv_categ;
  std::vector<uint8_t> urn;
};

class GetNetworkDetectedEccNumberInfoMessage
    : public SolicitedSyncMessage<PbmNetworkDetectedEccNumber>,
      public add_message_id<GetNetworkDetectedEccNumberInfoMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "GetNetworkDetectedEccNumberInfoMessage";

  inline GetNetworkDetectedEccNumberInfoMessage()
      : SolicitedSyncMessage<PbmNetworkDetectedEccNumber>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  ~GetNetworkDetectedEccNumberInfoMessage() = default;

  std::string dump() {
    return mName;
  }
};
