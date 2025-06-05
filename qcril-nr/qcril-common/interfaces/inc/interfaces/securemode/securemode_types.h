/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <string>
#include <vector>
#include "interfaces/common.h"

namespace qcril {
namespace interfaces {

enum class PeripheralStatus {
  UNKNOWN,
  SECURE = 0x01,
  NON_SECURE = 0x02,
  EMERGENCY_CALL = 0x03,
  RESET_CONNECTION = 0xFF,
};

struct RilSecureModeStatus_t: public qcril::interfaces::BasePayload {
  PeripheralStatus mState;
  explicit inline RilSecureModeStatus_t() {
    mState = PeripheralStatus::UNKNOWN;
  }
  explicit inline RilSecureModeStatus_t(const uint8_t state) {
    switch(state) {
      case 1:
        mState = PeripheralStatus::SECURE;
        break;
      case 2:
        mState = PeripheralStatus::NON_SECURE;
        break;
      case 3:
        mState = PeripheralStatus::EMERGENCY_CALL;
        break;
      case 255:
        mState = PeripheralStatus::RESET_CONNECTION;
        break;
      default:
        mState = PeripheralStatus::UNKNOWN;
  }

  }
};

}  // namespace interfaces
}  // namespace qcril
