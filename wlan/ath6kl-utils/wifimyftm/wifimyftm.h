/*==============================================================================
*  Copyright (c) 2020, 2023 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
#pragma once

#include <log/log.h>
#include <stdlib.h>
#include <utils/Log.h>
#ifndef CONFIG_MYFTM_USE_AIDL
#include <vendor/qti/hardware/wifi/wifimyftm/1.0/IWifiMyFtm.h>
#include <vendor/qti/hardware/wifi/wifimyftm/1.0/types.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#else
#include <aidl/vendor/qti/hardware/wifi/wifimyftm/MyFtmStatus.h>
#include <aidl/vendor/qti/hardware/wifi/wifimyftm/BnWifiMyFtm.h>
#endif

#ifdef CONFIG_MYFTM_USE_AIDL
namespace aidl {
#endif
namespace vendor {
namespace qti {
namespace hardware {
namespace wifi {
namespace wifimyftm {
#ifndef CONFIG_MYFTM_USE_AIDL
namespace V1_0 {
#endif
namespace implementation {

#ifndef CONFIG_MYFTM_USE_AIDL
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hardware::hidl_handle;
struct WifiMyFtm : public IWifiMyFtm
#else
class WifiMyFtm: public BnWifiMyFtm
#endif
{
public:
#ifdef CONFIG_MYFTM_USE_AIDL
	::ndk::ScopedAStatus myftmCmd(const std::string& arg, ::aidl::vendor::qti::hardware::wifi::wifimyftm::MyFtmStatus* _aidl_return);
#else
	// Methods from ::vendor::qti::wifi::wifimyftm::V1_0::IWifiMyFtm follow.
	Return<void> myftmCmd(const hidl_string& arg, myftmCmd_cb _hidl_cb);
	// Methods from ::android::hidl::base::V1_0::IBase follow.
	static IWifiMyFtm* getInstance(void);
#endif
};

}  // namespace implementation
#ifndef CONFIG_MYFTM_USE_AIDL
}  // namespace V1_0
#endif
}  // namespace wifimyftm
}  // namespace wifi
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
#ifdef CONFIG_MYFTM_USE_AIDL
}  // namespace aidl
#endif
