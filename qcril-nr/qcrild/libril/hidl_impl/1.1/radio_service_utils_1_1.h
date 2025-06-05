/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RADIO_SERVICE_UTILS_1_1_H__
#define __RADIO_SERVICE_UTILS_1_1_H__

#include "android/hardware/radio/1.1/types.h"
#include "telephony/ril.h"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

RIL_Errno fillNetworkScanRequest_1_1(const V1_1::NetworkScanRequest& request,
                                     RIL_NetworkScanRequest& scanRequest);

}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_UTILS_1_1_H__
