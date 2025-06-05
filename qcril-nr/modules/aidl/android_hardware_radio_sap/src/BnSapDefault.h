/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "QtiMutex.h"
#include "framework/legacy.h"
#include <aidl/android/hardware/radio/sap/BnSap.h>

#undef TAG
#define TAG "RILQ"

namespace aidlimports {
using namespace aidl::android::hardware::radio::sap;
}

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace sap {

class BnSapDefault : public BnSap {
public:
  BnSapDefault() {}
  virtual ~BnSapDefault() {}

  ::ndk::ScopedAStatus setCallback(
      const std::shared_ptr<aidlimports::ISapCallback> &in_uimSapResponse)
      override
  {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus
  apduReq(int32_t token,
          ::aidl::android::hardware::radio::sap::SapApduType simlockOp,
          const std::vector<uint8_t> &simlockData) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus connectReq(int32_t in_serial,
                                  int32_t in_maxMsgSizeBytes) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus disconnectReq(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus powerReq(int32_t in_serial, bool in_powerOn) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus resetSimReq(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setTransferProtocolReq(
      int32_t in_serial,
      ::aidl::android::hardware::radio::sap::SapTransferProtocol
          in_transferProtocol) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus transferAtrReq(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus transferCardReaderStatusReq(int32_t in_serial) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
};

} // namespace sap
} // namespace radio
} // namespace hardware
} // namespace android
} // namespace aidl
