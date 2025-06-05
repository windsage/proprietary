/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*
* Not a contribution
*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "aidl/android/hardware/radio/data/BnRadioData.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace data {
class BnRadioDataBase : public BnRadioData {
public:
  BnRadioDataBase() {};
  virtual ~BnRadioDataBase() {};
  ::ndk::ScopedAStatus allocatePduSessionId(int32_t in_serial) override
  {
    std::ignore = in_serial;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus cancelHandover(int32_t in_serial, int32_t in_callId) override
  {
    std::ignore = in_serial;
    std::ignore = in_callId;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus deactivateDataCall(int32_t in_serial, int32_t in_cid, ::aidl::android::hardware::radio::data::DataRequestReason in_reason) override
  {
    std::ignore = in_serial;
    std::ignore = in_cid;
    std::ignore = in_reason;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getDataCallList(int32_t in_serial) override
  {
    std::ignore = in_serial;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus getSlicingConfig(int32_t in_serial) override
  {
    std::ignore = in_serial;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus releasePduSessionId(int32_t in_serial, int32_t in_id) override
  {
    std::ignore = in_serial;
    std::ignore = in_id;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus responseAcknowledgement() override
  {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setDataAllowed(int32_t in_serial, bool in_allow) override
  {
    std::ignore = in_serial;
    std::ignore = in_allow;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setDataProfile(int32_t in_serial, const std::vector<::aidl::android::hardware::radio::data::DataProfileInfo>& in_profiles) override
  {
    std::ignore = in_serial;
    std::ignore = in_profiles;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setDataThrottling(int32_t in_serial, ::aidl::android::hardware::radio::data::DataThrottlingAction in_dataThrottlingAction, int64_t in_completionDurationMillis) override
  {
    std::ignore = in_serial;
    std::ignore = in_dataThrottlingAction;
    std::ignore = in_completionDurationMillis;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setResponseFunctions(const std::shared_ptr<::aidl::android::hardware::radio::data::IRadioDataResponse>& in_radioDataResponse, const std::shared_ptr<::aidl::android::hardware::radio::data::IRadioDataIndication>& in_radioDataIndication) override
  {
    std::ignore = in_radioDataResponse;
    std::ignore = in_radioDataIndication;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus setupDataCall(int32_t in_serial, ::aidl::android::hardware::radio::AccessNetwork in_accessNetwork, const ::aidl::android::hardware::radio::data::DataProfileInfo& in_dataProfileInfo,
          bool in_roamingAllowed, ::aidl::android::hardware::radio::data::DataRequestReason in_reason, const std::vector<::aidl::android::hardware::radio::data::LinkAddress>& in_addresses, const std::vector<std::string>& in_dnses, int32_t in_pduSessionId,
          const std::optional<::aidl::android::hardware::radio::data::SliceInfo>& in_sliceInfo, bool in_matchAllRuleAllowed) override
  {
    std::ignore = in_serial;
    std::ignore = in_accessNetwork;
    std::ignore = in_dataProfileInfo;
    std::ignore = in_roamingAllowed;
    std::ignore = in_reason;
    std::ignore = in_addresses;
    std::ignore = in_dnses;
    std::ignore = in_pduSessionId;
    std::ignore = in_sliceInfo;
    std::ignore = in_matchAllRuleAllowed;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus startHandover(int32_t in_serial, int32_t in_callId) override
  {
    std::ignore = in_serial;
    std::ignore = in_callId;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus startKeepalive(int32_t in_serial, const ::aidl::android::hardware::radio::data::KeepaliveRequest& in_keepalive) override
  {
    std::ignore = in_serial;
    std::ignore = in_keepalive;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  ::ndk::ScopedAStatus stopKeepalive(int32_t in_serial, int32_t in_sessionHandle) override
  {
    std::ignore = in_serial;
    std::ignore = in_sessionHandle;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }

  #ifdef SET_LOCAL_URSP_CONFIG
  ::ndk::ScopedAStatus setLocalUrspConfiguration(int32_t serial, vec<UrspRule> urspRules)
  {
    std::ignore = serial;
    std::ignore = urspRules;
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  #endif
};
}  // namespace data
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
