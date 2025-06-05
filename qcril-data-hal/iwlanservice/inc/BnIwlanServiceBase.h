/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "aidl/vendor/qti/hardware/data/iwlandata/BnIWlan.h"
#pragma once

using namespace ::aidl::android::hardware::radio;
using namespace ::aidl::android::hardware::radio::data;

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace data {
namespace iwlandata {

class BnIwlanServiceBase : public BnIWlan {
public:
  BnIwlanServiceBase() {};
  virtual ~BnIwlanServiceBase() {};

  /*NDK APIS*/
  ::ndk::ScopedAStatus deactivateDataCall(int32_t /*in_serial*/,
                                          int32_t /*in_cid*/,
                                          DataRequestReason /*in_reason*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus getAllQualifiedNetworks(int32_t /*in_serial*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus getDataCallList(int32_t /*in_serial*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus getDataRegistrationState(int32_t /*in_serial*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus iwlanDisabled() override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus responseAcknowledgement() override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus setResponseFunctions(const std::shared_ptr<IIWlanResponse>& /*in_dataResponse*/,
                                            const std::shared_ptr<IIWlanIndication>& /*in_dataIndication*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus setupDataCall(int32_t /*in_serial*/,
                                     AccessNetwork /*in_accessNetwork*/,
                                     const DataProfileInfo& /*in_dataProfileInfo*/,
                                     bool /*in_roamingAllowed*/,
                                     DataRequestReason /*in_reason*/,
                                     const std::vector<LinkAddress>& /*in_addresses*/,
                                     const std::vector<std::string>& /*in_dnses*/,
                                     int32_t /*in_pduSessionId*/,
                                     const std::optional<SliceInfo>& /*in_sliceInfo*/,
                                     bool /*in_matchAllRuleAllowed*/) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
};

}  // namespace iwlandata
}  // namespace data
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl