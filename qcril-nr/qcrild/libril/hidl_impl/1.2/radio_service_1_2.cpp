/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "hidl_impl/1.2/radio_service_1_2.h"
#include "HalServiceImplFactory.h"

#undef TAG
#define TAG "RILQ"

namespace android {
namespace hardware {
namespace radio {
namespace V1_2 {
namespace implementation {

static void __attribute__((constructor)) registerRadioImpl_1_2();

void registerRadioImpl_1_2() {
  QCRIL_LOG_INFO("Calling registerRadioImpl_1_2");
  getHalServiceImplFactory<RadioServiceBase>().registerImpl<RadioServiceImpl<V1_2::IRadio>>();
}

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_2::IRadio>::getVersion() {
  static HalServiceImplVersion version(1, 2);
  return version;
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace radio
}  // namespace hardware
}  // namespace android
