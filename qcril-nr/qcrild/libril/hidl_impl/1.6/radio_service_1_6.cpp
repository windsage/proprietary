/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "hidl_impl/1.6/radio_service_1_6.h"
#include "HalServiceImplFactory.h"

#undef TAG
#define TAG "RILQ"

namespace android {
namespace hardware {
namespace radio {
namespace V1_6 {
namespace implementation {

static void __attribute__((constructor)) registerRadioImpl_1_6();

void registerRadioImpl_1_6() {
  QCRIL_LOG_INFO("Calling registerRadioImpl_1_6");
  getHalServiceImplFactory<RadioServiceBase>().registerImpl<RadioServiceImpl<V1_6::IRadio>>();
}

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_6::IRadio>::getVersion() {
  static HalServiceImplVersion version(1, 6);
  return version;
}

}  // namespace implementation
}  // namespace V1_6
}  // namespace radio
}  // namespace hardware
}  // namespace android
