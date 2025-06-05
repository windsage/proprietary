/******************************************************************************
  @file    Powercallback.h
  @brief   perf call back HAL header

  DESCRIPTION

  Copyright (c) 2020-2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef VENDOR_QTI_HARDWARE_IPERFCALLBACK_V2_1_H
#define VENDOR_QTI_HARDWARE_IPERFCALLBACK_V2_1_H

#include <aidl/vendor/qti/hardware/perf2/BnPerf.h>
#include <aidl/vendor/qti/hardware/perf2/BnPerfCallback.h>
#include "PowerCore.h"

#define LOG_CALLBACK "PowerCallback"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace perf2{

using ::ndk::ScopedAStatus;


class PowerCallback : public BnPerfCallback {
private:
    void Init();
    PowerCore *mPowerCore = mPowerCore->getInstance();
public:
    PowerCallback();
    ~PowerCallback();
    ScopedAStatus notifyCallback (int32_t hint, const std::string& userDataStr, int32_t userData1, int32_t userData2, const std::vector<int32_t>& reserved) override;
};

}  // namespace perf2
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
#endif  // VENDOR_QTI_HARDWARE_IPERFCALLBACK_V2_1_H
