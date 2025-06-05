/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "voice_service_v02.h"

class ReleaseAudioRequestMessage : public SolicitedSyncMessage<void>,
                                   public add_message_id<ReleaseAudioRequestMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "ReleaseAudioRequestMessage";

  ~ReleaseAudioRequestMessage() = default;

  inline ReleaseAudioRequestMessage() : SolicitedSyncMessage<void>(get_class_message_id())
  {
    mName = MESSAGE_NAME;
  }

  string dump()
  {
    std::string os = mName;
    return os;
  }
};
