/**
  * Copyright (c) 2024 Qualcomm Technologies, Inc.
  * All Rights Reserved.
  * Confidential and Proprietary - Qualcomm Technologies, Inc.
  */
/* Qualcomm Technologies, Inc. has chosen to take main.c subject to
 * the BSD license and terms.
 */

#include <unistd.h>
#include <log/log.h>
#include <netlink/genl/ctrl.h>

#include "qca-vendor_copy.h"
#include "nl80211_copy.h"
#include "eloop.h"
#include "aidl.h"
#include "qtiwifitool.h"

int main(int argc, char *argv[])
{
	ALOGI("start qtiwifi");

	eloop_init(NULL);

	eloop_register_signal(SIGTERM, qtiwifi_terminate, NULL);
	eloop_register_signal(SIGINT, qtiwifi_terminate, NULL);

	qtiwifi_nl80211_init();

	qtiwifi_aidl_init(global);

	eloop_run();

	qtiwifi_aidl_deinit(global);

	qtiwifi_nl80211_deinit();

	return 0;
}
