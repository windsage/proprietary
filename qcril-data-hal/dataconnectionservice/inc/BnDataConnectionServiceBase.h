/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "aidl/vendor/qti/hardware/data/connectionaidl/BnDataConnection.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace data {
namespace connectionaidl {
class BnDataConnectionServiceBase : public BnDataConnection {
public:
  BnDataConnectionServiceBase() {};
  virtual ~BnDataConnectionServiceBase() {};
  ::ndk::ScopedAStatus getAllBearerAllocations(const std::shared_ptr<::aidl::vendor::qti::hardware::data::connectionaidl::IDataConnectionResponse>& , ::aidl::vendor::qti::hardware::data::connectionaidl::StatusCode* ) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus getBearerAllocation(int32_t, const std::shared_ptr<::aidl::vendor::qti::hardware::data::connectionaidl::IDataConnectionResponse>& , ::aidl::vendor::qti::hardware::data::connectionaidl::StatusCode* ) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus getConfig(const std::string&, const std::string&, std::string* ) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
  ::ndk::ScopedAStatus registerForAllBearerAllocationUpdates(const std::shared_ptr<::aidl::vendor::qti::hardware::data::connectionaidl::IDataConnectionIndication>& , ::aidl::vendor::qti::hardware::data::connectionaidl::StatusCode* ) override {
    return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
  }
};
}
}
}
}
}
}