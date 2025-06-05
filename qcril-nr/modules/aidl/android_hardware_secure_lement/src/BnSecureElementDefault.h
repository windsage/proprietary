/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "QtiMutex.h"
#include "framework/legacy.h"
#include <aidl/android/hardware/secure_element/BnSecureElement.h>

#undef TAG
#define TAG "RILQ"

namespace aidlimports {
using namespace aidl::android::hardware::secure_element;
}

namespace aidl {
namespace android {
namespace hardware {
namespace secure_element {

class BnSecureElementDefault : public BnSecureElement {
public:
  BnSecureElementDefault() {}
  virtual ~BnSecureElementDefault() {}

  ::ndk::ScopedAStatus closeChannel(int8_t /*in_channelNumber*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus getAtr(std::vector<uint8_t>* /*_aidl_return*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus
  init(const std::shared_ptr<aidlimports::ISecureElementCallback>&
      /*in_clientCallback*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus isCardPresent(bool* /*_aidl_return*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus
  openBasicChannel(const std::vector<uint8_t>& /*in_aid*/, int8_t /*in_p2*/,
                   std::vector<uint8_t>* /*_aidl_return*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus openLogicalChannel(
      const std::vector<uint8_t>& /*in_aid*/, int8_t /*in_p2*/,
      aidlimports::LogicalChannelResponse* /*_aidl_return*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus reset() override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus transmit(const std::vector<uint8_t>& /*in_data*/,
                                std::vector<uint8_t>* /*_aidl_return*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
};

} // namespace sap
} // namespace radio
} // namespace hardware
} // namespace android
