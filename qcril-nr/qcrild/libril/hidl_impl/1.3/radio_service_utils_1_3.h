/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RADIO_SERVICE_UTILS_1_3_H__
#define __RADIO_SERVICE_UTILS_1_3_H__

#include "android/hardware/radio/1.1/types.h"
#include "telephony/ril.h"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

RIL_Errno fillSetSystemSelectionChannelRequest(
    const ::android::hardware::hidl_vec<::android::hardware::radio::V1_1::RadioAccessSpecifier>&
        specifiers,
    RIL_SysSelChannels& ril_request);

}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_UTILS_1_3_H__
