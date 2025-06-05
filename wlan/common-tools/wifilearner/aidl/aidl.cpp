/*
 * Copyright (c) 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include "wifistats.h"

extern "C"
{
#include "aidl.h"
#include "wifilearner.h"
}

using aidl::vendor::qti::hardware::wifi::wifilearner::implementation::WifiStats;

/*
 * Function     : wifilearner_aidl_process
 * Description  : registers as server instance for IWifiStats service
 * Input params : pointer to wifilearner_ctx
 * Return       : SUCCESS/FAILURE
 *
 */
int wifilearner_aidl_process(struct wifilearner_ctx *wlc)
{
	bool ret = false;
	android::base::InitLogging(NULL,
			android::base::LogdLogger(android::base::SYSTEM));
	LOG(INFO) << "Wifi Learner Aidl Hal is booting up...";

	ABinderProcess_setThreadPoolMaxThreadCount(0);
	std::shared_ptr<WifiStats> wifilearner =
			ndk::SharedRefBase::make<WifiStats>(wlc);

	if (wifilearner == nullptr) {
		LOG(INFO) << "Wifi Learner Aidl service is null";
		return -1;
	}
	std::string instance = std::string() +
		WifiStats::descriptor + "/default";
	int res = AServiceManager_addService(wifilearner->asBinder().get(),
			instance.c_str());
	if (res != 0) {
		printf("Can't register wifilearner Aidl service");
		return -1;
	}

	ABinderProcess_joinThreadPool();
	LOG(INFO) << "wifilearner Aidl service is terminating...";

	return 0;
}
