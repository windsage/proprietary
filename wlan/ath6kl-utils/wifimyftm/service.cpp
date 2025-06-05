/*==============================================================================
*  Copyright (c) 2020, 2023 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
#include "wifimyftm.h"
#include <stdio.h>
#ifdef CONFIG_MYFTM_USE_AIDL
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <android-base/macros.h>
#else /* CONFIG_MYFTM_USE_AIDL */
#include <hidl/LegacySupport.h>
#endif /* CONFIG_MYFTM_USE_AIDL */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.wifi.wifimyftm-service"
#define UNUSED(x) ((void)x)

#ifdef CONFIG_MYFTM_USE_AIDL
using aidl::vendor::qti::hardware::wifi::wifimyftm::implementation::WifiMyFtm;
#else /* CONFIG_MYFTM_USE_AIDL */
using vendor::qti::hardware::wifi::wifimyftm::V1_0::IWifiMyFtm;
using vendor::qti::hardware::wifi::wifimyftm::V1_0::implementation::WifiMyFtm;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;
#endif /* CONFIG_MYFTM_USE_AIDL */

int main() {
	int res;
	printf("WifiMyFTM Service Intiating");
#ifdef CONFIG_MYFTM_USE_AIDL
	ABinderProcess_setThreadPoolMaxThreadCount(0);
	std::shared_ptr<WifiMyFtm> myftm = ndk::SharedRefBase::make<WifiMyFtm>();
#else /* CONFIG_MYFTM_USE_AIDL */
	android::sp<IWifiMyFtm> myftm = WifiMyFtm::getInstance();
	configureRpcThreadpool(1, true);
#endif /* CONFIG_MYFTM_USE_AIDL */

	if (myftm != nullptr) {
#ifdef CONFIG_MYFTM_USE_AIDL
		std::string instance = std::string() +
			WifiMyFtm::descriptor + "/default";
		res = AServiceManager_addService(myftm->asBinder().get(),
			instance.c_str());
#else /* CONFIG_MYFTM_USE_AIDL */
		res = myftm->registerAsService();
#endif /* CONFIG_MYFTM_USE_AIDL */
		if (res != 0) {
			printf("Can't register WifiMyFTM service");
			return 0;
		}
	} else {
		printf("Can't create instance of WifiMyFtm, nullptr");
		return 0;
	}
#ifdef CONFIG_MYFTM_USE_AIDL
	ABinderProcess_joinThreadPool();
#else /* CONFIG_MYFTM_USE_AIDL */
	joinRpcThreadpool();
#endif /* CONFIG_MYFTM_USE_AIDL */
	printf("WifiMyFTM Service Terminated");

	return 0; // should never get here
}
