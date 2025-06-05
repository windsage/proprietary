/*
 * Copyright (c) 2018, 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#include "wifilearner.h"
#include "nl_utils.h"
#include "utils.h"
#ifdef CONFIG_WIFILEARNER_USE_AIDL
#include "aidl.h"
#else
#include "hidl.h"
#endif

int main(int argc, char *argv[])
{
	struct wifilearner_ctx wlc;
	int status = 0;

	wlc.stdout_debug = WL_MSG_INFO;
	if (nl80211_init(&wlc)) {
		wl_print(&wlc, WL_MSG_ERROR, "netlink initialization failed.");
		return -1;
	}

#ifdef CONFIG_WIFILEARNER_USE_AIDL
	if (wifilearner_aidl_process(&wlc)) {
#else
	if (wifilearner_hidl_process(&wlc)) {
#endif
		wl_print(&wlc, WL_MSG_ERROR, "hidl/aidl initialization failed.");
		status = -1;
		goto out;
	}

out:
	nl80211_deinit(&wlc);
	return status;
}
