/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "xpan_wifi_hal.h"
#include "nl_command.h"
#include "xpan_event_handler.h"
#include "qca-vendor_copy.h"
#include "qca-vendor_oem.h"

#define TWT_WAKE_INTERVAL_TU_FACTOR 1024
#define TWT_WAKE_DURATION_FACTOR 256

void EventHandler::setCallbackHandler(wifi_callbacks handler)
{
	memset(&mHandler, 0, sizeof(mHandler));
	mHandler = handler;
}

static wifi_error get_mac_addr(struct nlattr **tb_vendor,
			       int attribute, mac_addr addr)
{
	if (!tb_vendor[attribute]) {
		ALOGE("Failed to get attribute : %d", attribute);
		return WIFI_ERROR_INVALID_ARGS;
	}

	if (nla_len(tb_vendor[attribute]) != sizeof(mac_addr)) {
		ALOGE("Invalid mac addr length\n");
		return WIFI_ERROR_INVALID_ARGS;
	}

	memcpy(addr, (u8 *)nla_data(tb_vendor[attribute]),
	       nla_len(tb_vendor[attribute]));

	return WIFI_SUCCESS;
}

/* Parses the information received from twt_setup event and calls the callback
 * registered via cb_wifi_twt_event.
 * Returns 0 on success.
 */
int EventHandler::twtSetupHandleEvent(struct nlattr **tb_vendor)
{
	int attr_id, val;
	uint32_t wake_dur, wake_dur_us, wake_inter_tu, wake_inter_us;
	struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAX + 1];

	if (nla_parse_nested(tb, QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAX,
		 tb_vendor[QCA_WLAN_VENDOR_ATTR_CONFIG_TWT_PARAMS], NULL)) {
		ALOGE("nla_parse failed for vendor_data\n");
		return -EINVAL;
	}

	twt_event_response response;
	memset(&response, 0, sizeof(twt_event_response));

	response.event_type = TWT_OPER_EVENT_SETUP;

	attr_id = QCA_WLAN_VENDOR_ATTR_TWT_SETUP_WAKE_DURATION;
	if (!tb[attr_id]) {
		ALOGE("TWT_SETUP_WAKE_DURATION is missing");
		return -EINVAL;
	}
	wake_dur = nla_get_u32(tb[attr_id]);
	wake_dur_us = wake_dur * TWT_WAKE_DURATION_FACTOR;

	attr_id = QCA_WLAN_VENDOR_ATTR_TWT_SETUP_WAKE_INTVL2_MANTISSA;
	if (!tb[attr_id]) {
		attr_id = QCA_WLAN_VENDOR_ATTR_TWT_SETUP_WAKE_INTVL_MANTISSA;
		if (!tb[attr_id]) {
			ALOGE("TWT_SETUP_WAKE_INTVL_MANTISSA is missing");
			return -EINVAL;
		}
		wake_inter_tu = nla_get_u32(tb[attr_id]);
		wake_inter_us = wake_inter_tu * TWT_WAKE_INTERVAL_TU_FACTOR;
	} else {
		wake_inter_us = nla_get_u32(tb[attr_id]);
	}

	response.wake_dur_us = wake_dur_us;
	response.wake_int_us = wake_inter_us;
	if(get_mac_addr(tb, QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAC_ADDR,
				response.addr)) {
			ALOGE("%s TWT peer bssid missing", __func__);
			return -EINVAL;
		}

	if (mHandler.cb_wifi_twt_event)
		(*mHandler.cb_wifi_twt_event)(response);
	else
		ALOGE("No callbacks registered for twt events");

	return 0;
}

/* Parses the information received from twt_terminate event and calls the
 * callback registered via cb_wifi_twt_event.
 * Returns 0 on success.
 */
int EventHandler::twtTerminateHandleEvent(struct nlattr **tb_vendor)
{
	struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAX + 1];

	if (nla_parse_nested(tb, QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAX,
			     tb_vendor[QCA_WLAN_VENDOR_ATTR_CONFIG_TWT_PARAMS],
			     NULL)) {
		ALOGE("nla_parse failed for vendor_data\n");
		return -EINVAL;
	}

	twt_event_response response;
	memset(&response, 0, sizeof(twt_event_response));
	response.event_type = TWT_OPER_EVENT_TERMINATE;

	if(get_mac_addr(tb, QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAC_ADDR,
			response.addr)) {
		ALOGE("%s TWT peer bssid missing", __func__);
		return -EINVAL;
	}

	if (mHandler.cb_wifi_twt_event)
		(*mHandler.cb_wifi_twt_event)(response);
	else
		ALOGE("No callbacks registered for twt events");

	return 0;
}

/* Parses the information received from twt_suspend event and calls the callback
 * registered via cb_wifi_twt_event.
 * Returns 0 on success.
 */
int EventHandler::twtSuspendHandleEvent(struct nlattr **tb_vendor)
{
	struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAX + 1];

	if (nla_parse_nested(tb, QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAX,
			     tb_vendor[QCA_WLAN_VENDOR_ATTR_CONFIG_TWT_PARAMS],
			     NULL)) {
		ALOGE("nla_parse failed for vendor_data\n");
		return -EINVAL;
	}

	twt_event_response response;
	memset(&response, 0, sizeof(twt_event_response));
	response.event_type = TWT_OPER_EVENT_SUSPEND;

	if(get_mac_addr(tb, QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAC_ADDR,
			response.addr)) {
		ALOGE("%s TWT peer bssid missing", __func__);
		return -EINVAL;
	}


	if (mHandler.cb_wifi_twt_event)
		(*mHandler.cb_wifi_twt_event)(response);
	else
		ALOGE("No callbacks registered for twt events");

	return 0;
}

/* Parses the information received from twt_resume event and calls the callback
 * registered via cb_wifi_twt_event.
 * Returns 0 on success.
 */
int EventHandler::twtResumeHandleEvent(struct nlattr **tb_vendor)
{
	struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAX + 1];

	if (nla_parse_nested(tb, QCA_WLAN_VENDOR_ATTR_TWT_SETUP_MAX,
			     tb_vendor[QCA_WLAN_VENDOR_ATTR_CONFIG_TWT_PARAMS],
			     NULL)) {
		ALOGE("nla_parse failed for vendor_data\n");
		return -EINVAL;
	}

	twt_event_response response;
	memset(&response, 0, sizeof(twt_event_response));
	response.event_type = TWT_OPER_EVENT_RESUME;

	if(get_mac_addr(tb, QCA_WLAN_VENDOR_ATTR_TWT_RESUME_MAC_ADDR,
			response.addr)) {
		ALOGE("%s TWT peer bssid missing", __func__);
		return -EINVAL;
	}


	if (mHandler.cb_wifi_twt_event)
		(*mHandler.cb_wifi_twt_event)(response);
	else
		ALOGE("No callbacks registered for twt events");

	return 0;
}



/* This function will be the main handler for incoming twt event.
 * Call the appropriate callback handler after parsing the vendor data.
 */
wifi_error EventHandler::twtEventHandler(WifiEvent &event)
{
	int ret = -EINVAL;
	char *mVendorData = NULL;
	u32 mDataLen = 0;
	struct nlattr **tb = event.attributes();

	if (!tb[NL80211_ATTR_VENDOR_DATA]) {
		ALOGE("NL80211_ATTR_VENDOR_DATA not present");
		goto out;
	}

	mVendorData = (char *)nla_data(tb[NL80211_ATTR_VENDOR_DATA]);
	mDataLen = nla_len(tb[NL80211_ATTR_VENDOR_DATA]);

	nla_parse(tb, QCA_WLAN_VENDOR_ATTR_CONFIG_TWT_MAX,
		  (struct nlattr *)mVendorData,
		  mDataLen, NULL);

	u8 twt_operation_type;

	if (!tb[QCA_WLAN_VENDOR_ATTR_CONFIG_TWT_OPERATION]) {
		ALOGE("QCA_WLAN_VENDOR_ATTR_CONFIG_TWT_OPERATION not present");
		goto out;
	}

	twt_operation_type = nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_CONFIG_TWT_OPERATION]);

	switch(twt_operation_type)
	{
		case QCA_WLAN_TWT_SET:
			ret = twtSetupHandleEvent(tb);
			break;
		case QCA_WLAN_TWT_TERMINATE:
			ret = twtTerminateHandleEvent(tb);
			break;
		case QCA_WLAN_TWT_SUSPEND:
			ret = twtSuspendHandleEvent(tb);
			break;
		case QCA_WLAN_TWT_RESUME:
			ret = twtResumeHandleEvent(tb);
			break;
		default:
			ALOGE("Unknown event received");
			break;
	}

out:
	return mapKernelErrortoWifiHalError(ret);
}

wifi_error EventHandler::acsEventHandler(WifiEvent &event)
{
	int freq = 0;
	char *mVendorData = NULL;
	u32 mDataLen = 0;
	struct nlattr **tb = event.attributes();

	if (!tb[NL80211_ATTR_VENDOR_DATA]) {
		ALOGE("NL80211_ATTR_VENDOR_DATA not present");
		return WIFI_ERROR_INVALID_ARGS;
	}

	mVendorData = (char *)nla_data(tb[NL80211_ATTR_VENDOR_DATA]);
	mDataLen = nla_len(tb[NL80211_ATTR_VENDOR_DATA]);

	if (nla_parse(tb, QCA_WLAN_VENDOR_ATTR_ACS_MAX,
		      (struct nlattr *)mVendorData,
		       mDataLen, NULL)) {
		ALOGE("nla_parse failed for QCA_NL80211_VENDOR_SUBCMD_DO_ACS");
		return WIFI_ERROR_INVALID_ARGS;
	}

	if (!tb[QCA_WLAN_VENDOR_ATTR_ACS_PRIMARY_FREQUENCY]) {
		ALOGE("QCA_WLAN_VENDOR_ATTR_ACS_PRIMARY_FREQUENCY not present");
		return WIFI_ERROR_INVALID_ARGS;
	}

	freq = nla_get_u32(tb[QCA_WLAN_VENDOR_ATTR_ACS_PRIMARY_FREQUENCY]);

	if (mHandler.cb_wifi_acs_results)
		(*mHandler.cb_wifi_acs_results)(freq, ACS_COMPLETED);
	else
		ALOGE("No callbacks registered for acs event");

	return WIFI_SUCCESS;
}

wifi_error EventHandler::psEventHandler(WifiEvent &event)
{

	char *mVendorData = NULL;
	u32 mDataLen = 0;
	struct nlattr **tb = event.attributes();
	ap_power_save_data power_save_data;

	if (!tb[NL80211_ATTR_VENDOR_DATA]) {
		ALOGE("NL80211_ATTR_VENDOR_DATA not present");
		return WIFI_ERROR_INVALID_ARGS;
	}

	mVendorData = (char *)nla_data(tb[NL80211_ATTR_VENDOR_DATA]);
	mDataLen = nla_len(tb[NL80211_ATTR_VENDOR_DATA]);

	nla_parse(tb, QCA_WLAN_VENDOR_ATTR_DOZED_AP_MAX,
		  (struct nlattr *)mVendorData,
		  mDataLen, NULL);
	if(!tb[QCA_WLAN_VENDOR_ATTR_DOZED_AP_COOKIE])
	{
		ALOGE("%s: QCA_WLAN_VENDOR_ATTR_DOZED_AP_COOKIE not found", __func__);
		return WIFI_ERROR_INVALID_ARGS;
	}

	power_save_data.cookie = nla_get_u64(tb[QCA_WLAN_VENDOR_ATTR_DOZED_AP_COOKIE]);

	if(!tb[QCA_WLAN_VENDOR_ATTR_DOZED_AP_NEXT_TSF])
	{
		ALOGE("%s: QCA_WLAN_VENDOR_ATTR_DOZED_AP_NEXT_TSF not found", __func__);
		return WIFI_ERROR_INVALID_ARGS;
	}

	power_save_data.next_tsf = nla_get_u64(tb[QCA_WLAN_VENDOR_ATTR_DOZED_AP_NEXT_TSF]);

	if(!tb[QCA_WLAN_VENDOR_ATTR_DOZED_AP_BI_MULTIPLIER])
	{
		ALOGE("%s: QCA_WLAN_VENDOR_ATTR_DOZED_AP_BI_MULTIPLIER not found", __func__);
		return WIFI_ERROR_INVALID_ARGS;
	}

	power_save_data.power_save_bi_multiplier = nla_get_u16(tb[QCA_WLAN_VENDOR_ATTR_DOZED_AP_BI_MULTIPLIER]);

	if(mHandler.cb_ap_power_save_event)
		(mHandler.cb_ap_power_save_event)(power_save_data);
	else
		ALOGE("No callbacks registered for power_save events");

	return WIFI_SUCCESS;
}

wifi_error EventHandler::ssrEventHandler(WifiEvent &event)
{
	int cmd = event.get_cmd();
	int subcmd = 0;
	wlan_ssr_status ssr_status;

	if (cmd == NL80211_CMD_VENDOR) {
		subcmd = event.get_u32(NL80211_ATTR_VENDOR_SUBCMD);
	}

	if (subcmd == QCA_NL80211_VENDOR_SUBCMD_HANG) {
		ALOGI("Received ssr event: WLAN_SSR_STARTED");
		ssr_status =  WLAN_SSR_STARTED;
	}
	else if (subcmd == QCA_NL80211_VENDOR_SUBCMD_DRIVER_READY) {
		ALOGI("Received ssr event: WLAN_SSR_COMPLETED");
		ssr_status = WLAN_SSR_COMPLETED;
	}
	else {
		return WIFI_ERROR_UNKNOWN;
	}

	if (mHandler.cb_wlan_ssr_event)
		(mHandler.cb_wlan_ssr_event)(ssr_status);
	else
		ALOGE("No callbacks registered for wlan_ssr events");

	return WIFI_SUCCESS;
}

wifi_error EventHandler::switchEventHandler(WifiEvent &event)
{
	char *mVendorData = NULL;
	u32 mDataLen = 0;
	struct nlattr **tb = event.attributes();
	transport_switch_type switch_type;
	transport_switch_status switch_status;

	if (!tb[NL80211_ATTR_VENDOR_DATA]) {
		ALOGE("NL80211_ATTR_VENDOR_DATA not present");
		return WIFI_ERROR_INVALID_ARGS;
	}

	mVendorData = (char *)nla_data(tb[NL80211_ATTR_VENDOR_DATA]);
	mDataLen = nla_len(tb[NL80211_ATTR_VENDOR_DATA]);

	nla_parse(tb, QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_MAX,
		  (struct nlattr *)mVendorData,
		  mDataLen, NULL);
	if(!tb[QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_TYPE])
	{
		ALOGE("%s: QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_TYPE not found", __func__);
		return WIFI_ERROR_INVALID_ARGS;
	}

	switch (nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_TYPE])) {
		case QCA_WLAN_AUDIO_TRANSPORT_SWITCH_TYPE_NON_WLAN:
			switch_type = TRANSPORT_SWITCH_TYPE_NON_WLAN;
			break;
		case QCA_WLAN_AUDIO_TRANSPORT_SWITCH_TYPE_WLAN:
			switch_type = TRANSPORT_SWITCH_TYPE_WLAN;
			break;
		default:
			ALOGE("Received switch request/response of invalid type");
			return WIFI_ERROR_INVALID_ARGS;
	}

	if(!tb[QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_STATUS])
	{
		ALOGI("Received request for transport switch from driver");
		switch_status = TRANSPORT_SWITCH_STATUS_REQUEST;
	} else {
		ALOGI("Received response for transport switch from driver");
		switch (nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_STATUS])) {
			case QCA_WLAN_AUDIO_TRANSPORT_SWITCH_STATUS_REJECTED:
				switch_status = TRANSPORT_SWITCH_STATUS_REJECTED;
				break;
			case QCA_WLAN_AUDIO_TRANSPORT_SWITCH_STATUS_COMPLETED:
				switch_status = TRANSPORT_SWITCH_STATUS_COMPLETED;
				break;
			default:
				ALOGE("Invalid switch status received");
				return WIFI_ERROR_INVALID_ARGS;
		}
	}

	if(mHandler.cb_wifi_audio_transport_switch)
		(mHandler.cb_wifi_audio_transport_switch)(switch_type, switch_status);
	else
		ALOGE("No callbacks registered for audio transport switch events");

	return WIFI_SUCCESS;
}

wifi_error EventHandler::apAvailabilityEventHandler(WifiEvent &event)
{
	char *mVendorData = NULL;
	u32 mDataLen = 0;
	struct nlattr **tb = event.attributes();
	uint16_t cookie;
	xpan_ap_availability_status status;

	if (!tb[NL80211_ATTR_VENDOR_DATA]) {
		ALOGE("NL80211_ATTR_VENDOR_DATA not present");
		return WIFI_ERROR_INVALID_ARGS;
	}

	mVendorData = (char *)nla_data(tb[NL80211_ATTR_VENDOR_DATA]);
	mDataLen = nla_len(tb[NL80211_ATTR_VENDOR_DATA]);

	nla_parse(tb, QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_MAX,
		  (struct nlattr *)mVendorData, mDataLen, NULL);

	if(!tb[QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE])
	{
		ALOGE("%s: QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE not found", __func__);
		return WIFI_ERROR_INVALID_ARGS;
	}

	cookie = nla_get_u16(tb[QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE]);

	if (!tb[QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_OPERATION])
	{
		ALOGI("Received request for ap availability from driver");
		status = AP_AVAILABILITY_STARTED;
	} else {
		ALOGI("Received response for ap availability from driver");
		switch (nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_OPERATION])) {
			case QCA_HIGH_AP_AVAILABILITY_OPERATION_COMPLETED:
				status = AP_AVAILABILITY_COMPLETED;
				break;
			case QCA_HIGH_AP_AVAILABILITY_OPERATION_CANCELLED:
				status = AP_AVAILABILITY_CANCELLED;
				break;
			default:
				ALOGE("Invalid status received");
				return WIFI_ERROR_INVALID_ARGS;
		}
	}

	if(mHandler.cb_xpan_ap_availability)
		(mHandler.cb_xpan_ap_availability)(cookie, status);
	else
		ALOGE("No callbacks registered for ap availability events");

	return WIFI_SUCCESS;
}
