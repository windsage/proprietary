/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "hidl_impl/1.0/radio_service_1_0.h"
#include "HalServiceImplFactory.h"

#undef TAG
#define TAG "RILQ"

namespace android {
namespace hardware {
namespace radio {
namespace V1_0 {
namespace implementation {

static void __attribute__((constructor)) registerRadioImpl_1_0();

void registerRadioImpl_1_0() {
  QCRIL_LOG_INFO("Calling registerRadioImpl_1_0");
  getHalServiceImplFactory<RadioServiceBase>().registerImpl<RadioServiceImpl<V1_0::IRadio>>();
}

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_0::IRadio>::getVersion() {
  static HalServiceImplVersion version(1, 0);
  return version;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace radio
}  // namespace hardware
}  // namespace android
