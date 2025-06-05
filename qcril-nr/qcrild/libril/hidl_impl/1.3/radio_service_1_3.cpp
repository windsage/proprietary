/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "hidl_impl/1.3/radio_service_1_3.h"
#include "HalServiceImplFactory.h"

#undef TAG
#define TAG "RILQ"

namespace android {
namespace hardware {
namespace radio {
namespace V1_3 {
namespace implementation {

static void __attribute__((constructor)) registerRadioImpl_1_3();

void registerRadioImpl_1_3() {
  QCRIL_LOG_INFO("Calling registerRadioImpl_1_3");
  getHalServiceImplFactory<RadioServiceBase>().registerImpl<RadioServiceImpl<V1_3::IRadio>>();
}

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_3::IRadio>::getVersion() {
  static HalServiceImplVersion version(1, 3);
  return version;
}

}  // namespace implementation
}  // namespace V1_3
}  // namespace radio
}  // namespace hardware
}  // namespace android
