/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <aidl/vendor/qti/hardware/radio/qtiradioconfig/BnQtiRadioConfig.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradioconfig {
namespace implementation {

class BnQtiRadioConfigDefault : public BnQtiRadioConfig {
public:
  BnQtiRadioConfigDefault() {}
  virtual ~BnQtiRadioConfigDefault() {}

  ::ndk::ScopedAStatus getSecureModeStatus(int32_t in_serial) {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
};

}  // namespace implementation
}  // namespace qtiradio
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
