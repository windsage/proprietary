/******************************************************************************
#  Copyright (c) 2019, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <string>
#include <vector>
#include "voice_service_v02.h"

namespace qcril {
namespace interfaces {

enum class AudioCallState { INVALID, INACTIVE, ACTIVE, HOLD, LOCAL_HOLD };

inline std::string toString(const AudioCallState& o) {
  if (o == AudioCallState::INVALID) {
    return "INVALID";
  }
  if (o == AudioCallState::INACTIVE) {
    return "INACTIVE";
  }
  if (o == AudioCallState::ACTIVE) {
    return "ACTIVE";
  }
  if (o == AudioCallState::HOLD) {
    return "HOLD";
  }
  if (o == AudioCallState::LOCAL_HOLD) {
    return "LOCAL_HOLD";
  }
  return "<invalid>";
}

struct QcRilGetAudioParams {
  qcril::interfaces::AudioCallState call_state;
  bool is_crs_supported;
};

typedef struct QcRilSetAudioParams {
  uint32_t vsid;
  qcril::interfaces::AudioCallState call_state;
  call_mode_enum_v02 call_mode;
  bool crs_call;
  QcRilSetAudioParams(uint32_t id, qcril::interfaces::AudioCallState state, call_mode_enum_v02 mode,
      bool crs) : vsid(id), call_state(state), call_mode(mode), crs_call(crs){};
} QcRilSetAudioParams;


}  // namespace interfaces
}  // namespace qcril
