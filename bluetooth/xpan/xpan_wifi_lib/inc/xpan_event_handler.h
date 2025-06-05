/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __XPAN_EVENT_HANDLER_H__
#define __XPAN_EVENT_HANDLER_H__

#include "nl_command.h"

class EventHandler
{
private:
	wifi_callbacks mHandler;
	int twtSetupHandleEvent(struct nlattr **tb_vendor);
	int twtTerminateHandleEvent(struct nlattr **tb_vendor);
	int twtSuspendHandleEvent(struct nlattr **tb_vendor);
	int twtResumeHandleEvent(struct nlattr **tb_vendor);
public:
	wifi_error twtEventHandler(WifiEvent &event);
	void setCallbackHandler(wifi_callbacks nHandler);
	wifi_error acsEventHandler(WifiEvent &event);
	wifi_error psEventHandler(WifiEvent &event);
	wifi_error ssrEventHandler(WifiEvent &event);
	wifi_error switchEventHandler(WifiEvent &event);
	wifi_error apAvailabilityEventHandler(WifiEvent &event);
};

#endif
