/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
  Not a Contribution

* Copyright (C) 2014 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <string>

#include "xpan_wifi_hal.h"
#include "nl_command.h"
#include "qca-vendor_oem.h"
#include "xpan_event_handler.h"
#include "xpancommand.h"

#include <stdlib.h>

#include <netlink/object-api.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/attr.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netinet/ether.h>

#include <cutils/properties.h>

#define XPAN_WIFI_CMD_SOCK_PORT       744
#define XPAN_WIFI_EVENT_SOCK_PORT     745

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define PATH_SIZE 100

static inline int is_zero_ether_addr(const u8 *a)
{
	return !(a[0] | a[1] | a[2] | a[3] | a[4] | a[5]);
}

static int ack_handler(struct nl_msg *msg, void *arg)
{
	int *err = (int *)arg;

	*err = 0;
	return NL_STOP;
}

static int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = (int *)arg;

	*ret = 0;
	return NL_SKIP;
}

static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err,
		void *arg)
{
	int *ret = (int *)arg;

	*ret = err->error;
	ALOGI("%s received : %d - %s", __func__,
	      err->error, strerror(err->error));

	return NL_SKIP;
}

static void print_iface_list(iface_info *ifinfo)
{
	iface_info *temp = ifinfo;

	while (temp != NULL) {
		ALOGI("ifname:%s ifindex:%d iftype:%d\n",
			temp->ifname, temp->if_index, temp->type);
		temp = temp->next;
	}
}

static iface_info *get_ifaceinfo_from_list(iface_info *ifinfo, iface_type type)
{
	iface_info *temp = ifinfo;

	if (temp == NULL)
		return NULL;
	while (temp->type != type
		&& temp->next != NULL) {
		temp = temp->next;
	}
	if (temp->type != type)
		return NULL;
	return temp;
}

static void add_iface_node(xpan_wifi_data* data, iface_info *ifinfo)
{
	if (!data || !ifinfo) {
		ALOGE("%s: Error, data or ifinfo null", __func__);
		return;
	}
	iface_info *last = data->ifinfo;
	ifinfo->next = NULL;
	if (data->ifinfo == NULL) {
		ifinfo->prev = NULL;
		data->ifinfo = ifinfo;
		return;
	}
	while (last->next != NULL)
		last = last->next;
	last->next = ifinfo;
	ifinfo->prev = last;
}

static void remove_iface_node(xpan_wifi_data* data, iface_info *ifinfo)
{
	if (!data || !ifinfo) {
		ALOGE("%s: Error, data or ifinfo null", __func__);
		return;
	}

	if (data->ifinfo == ifinfo)
		data->ifinfo = ifinfo->next;

	if (ifinfo->next != NULL)
		ifinfo->next->prev = ifinfo->prev;

	if (ifinfo->prev != NULL)
		ifinfo->prev->next = ifinfo->next;

	free(ifinfo);
	ifinfo = NULL;
	return;
}

static void delete_iface_list(xpan_wifi_data *data)
{
	iface_info *temp = data->ifinfo, *next;

	while (temp != NULL) {
		next = temp->next;
		free(temp);
		temp = next;
	}
	data->ifinfo = NULL;
}

static nl_sock * xpan_wifi_create_nl_socket(int port, int protocol)
{
	uint32_t pid = getpid() & 0x3FFFFF;
	struct nl_sock *sock = nl_socket_alloc();

	if (sock == NULL) {
		ALOGE("Could not create handle");
		return NULL;
	}

	nl_socket_set_local_port(sock, pid | (port << 22));

	if (nl_connect(sock, protocol)) {
		ALOGE("Could not connect handle");
		nl_socket_free(sock);
		return NULL;
	}

	return sock;
}

static enum bandwidth get_chan_width(int bw_index)
{
	switch (bw_index) {
		case NL80211_CHAN_WIDTH_20_NOHT:
		case NL80211_CHAN_WIDTH_20:
			return CHAN_WIDTH_20;
		case NL80211_CHAN_WIDTH_40:
			return CHAN_WIDTH_40;
		case NL80211_CHAN_WIDTH_80:
			return CHAN_WIDTH_80;
		case NL80211_CHAN_WIDTH_80P80:
			return CHAN_WIDTH_80P80;
		case NL80211_CHAN_WIDTH_160:
			return CHAN_WIDTH_160;
		case NL80211_CHAN_WIDTH_320:
			return CHAN_WIDTH_320;
		default:
			return CHAN_WIDTH_20;
	}
}

xpan_wifi_status get_chan_switch_tsf(xpan_wifi_data *data, channel_params *chan_params)
{
	struct nlattr *nlData = NULL;
	wifi_handle wifiHandle = NULL;
	uint64_t tsf_ptr = 0;
	int ret;
	iface_info *iface;

	if (!data) {
		ALOGE("%s: Error xpan_wifi_data NULL", __func__);
		return XPAN_WIFI_STATUS_FAILURE;
	}

	wifiHandle = getWifiXpanHandle(data);
	WifihalGeneric cmd(wifiHandle, get_requestid(), OUI_QCA,
			   QCA_NL80211_VENDOR_SUBCMD_TSF);

	ret = cmd.create();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to create NL msg. Error:%d", __func__, ret);
		return XPAN_WIFI_STATUS_FAILURE;
	}
	iface = get_ifaceinfo_from_list(data->ifinfo, IFACE_TYPE_SAP);
	if (!iface) {
		ALOGE("%s iface is not present in the list", __func__);
		return XPAN_WIFI_STATUS_FAILURE;
	}
	cmd.set_iface_id(iface->ifname);
	cmd.setChanSwitchTsfPointer(&tsf_ptr);

	nlData = cmd.attr_start(NL80211_ATTR_VENDOR_DATA);
	if (!nlData) {
		ALOGE("failed attr_start for VENDOR_DATA for tsf commands");
		return XPAN_WIFI_STATUS_FAILURE;
	}

	ret = cmd.put_u32(QCA_WLAN_VENDOR_ATTR_TSF_CMD, QCA_TSF_SYNC_GET_CSA_TIMESTAMP);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		return XPAN_WIFI_STATUS_FAILURE;
	}
	cmd.attr_end(nlData);

	ret = cmd.requestResponse();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: requestResponse Error:%d", __func__, ret);
		return XPAN_WIFI_STATUS_FAILURE;
	}

	if (tsf_ptr > 0)
		chan_params->tsf = tsf_ptr;

	return XPAN_WIFI_STATUS_SUCCESS;
}

static int internal_valid_message_handler(nl_msg *msg, void *arg)
{
	wifi_handle handle = (wifi_handle)arg;
	xpan_wifi_data *data = (xpan_wifi_data *)handle;

	WifiEvent event(msg);
	int res = event.parse();
	if (res < 0) {
		ALOGE("Failed to parse event: %d", res);
		return NL_SKIP;
	}

	int cmd = event.get_cmd();
	uint32_t vendor_id = 0;
	int subcmd = 0, ifidx, wiphy;
	uint64_t wdev;
	iface_info *ifsap;

	if (cmd == NL80211_CMD_VENDOR) {
		vendor_id = event.get_u32(NL80211_ATTR_VENDOR_ID);
		subcmd = event.get_u32(NL80211_ATTR_VENDOR_SUBCMD);
		/* Restrict printing GSCAN_FULL_RESULT which is causing lot
		   of logs in bug report */
		if (subcmd !=
		    QCA_NL80211_VENDOR_SUBCMD_GSCAN_FULL_SCAN_RESULT) {
			ALOGV("event received %s, vendor_id = 0x%0x,"
					"subcmd = 0x%0x",
					event.get_cmdString(), vendor_id,
					subcmd);
		}

		ifidx = event.get_u32(NL80211_ATTR_IFINDEX);
		wiphy = event.get_u32(NL80211_ATTR_WIPHY);
		wdev = event.get_u64(NL80211_ATTR_WDEV);

		if (ifidx || wiphy || wdev) {
			/* Check interface index for non-broadcast events and
			 * ignore the events addressed to other interfaces
			 */
			ifsap = get_ifaceinfo_from_list(data->ifinfo, IFACE_TYPE_SAP);
			if (!ifsap || ifidx != if_nametoindex(ifsap->ifname))
				return NL_SKIP;
		}

		EventHandler *handler = new EventHandler();
		handler->setCallbackHandler(*(data->cb));
		switch(subcmd)
		{
		case QCA_NL80211_VENDOR_SUBCMD_CONFIG_TWT:
			ALOGI("Received twt event on xsap interface");
			handler->twtEventHandler(event);
			break;
		case QCA_NL80211_VENDOR_SUBCMD_DO_ACS:
			ALOGI("Received acs event on xsap interface");
			handler->acsEventHandler(event);
			break;
		case QCA_NL80211_VENDOR_SUBCMD_DOZED_AP:
			ALOGI("Received power save event on xsap interface");
			handler->psEventHandler(event);
			break;
		case QCA_NL80211_VENDOR_SUBCMD_HANG:
		case QCA_NL80211_VENDOR_SUBCMD_DRIVER_READY:
			ALOGI("Received ssr started/completed event on xsap interface");
			handler->ssrEventHandler(event);
			break;
		case QCA_NL80211_VENDOR_SUBCMD_AUDIO_TRANSPORT_SWITCH:
			ALOGI("Received switch request/response event on xsap interface");
			handler->switchEventHandler(event);
			break;
		case QCA_NL80211_VENDOR_SUBCMD_HIGH_AP_AVAILABILITY:
			ALOGI("Received ap availability event on xsap interface");
			handler->apAvailabilityEventHandler(event);
			break;
		default:
			ALOGV("Event ignored");
			break;
		}
	} else if (cmd == NL80211_CMD_CH_SWITCH_STARTED_NOTIFY) {
		channel_params chan_params;

		ifidx = event.get_u32(NL80211_ATTR_IFINDEX);
		ifsap = get_ifaceinfo_from_list(data->ifinfo, IFACE_TYPE_SAP);
		if (!ifsap || ifidx != if_nametoindex(ifsap->ifname))
			return NL_SKIP;

		chan_params.freq = event.get_u32(NL80211_ATTR_CENTER_FREQ1);
		chan_params.bw = get_chan_width(event.get_u32(NL80211_ATTR_CHANNEL_WIDTH));

		ALOGI("channel switch started, frequency = %d, bandwidth = %d",
		      chan_params.freq, chan_params.bw);
		if (get_chan_switch_tsf(data, &chan_params) != XPAN_WIFI_STATUS_SUCCESS) {
			ALOGE("Failed to get tsf value");
			return NL_SKIP;
		}

		ALOGI("TSF = %llu", chan_params.tsf);
		// trigger the callback
		if (data->cb && data->cb->cb_channel_switch_started)
			data->cb->cb_channel_switch_started(chan_params);
	} else {
		ALOGV("event received %s", event.get_cmdString());
	}

	return NL_OK;
}

void register_callbacks(void *ctx, struct wifi_callbacks *callback)
{

	xpan_wifi_data *data = (xpan_wifi_data *)ctx;

	if (!data || !callback) {
		ALOGE("%s: Error, xpan_wifi_data NULL or No callback present", __func__);
		return;
	}

	data->cb = callback;
}

void deregister_callbacks(void *ctx)
{
	xpan_wifi_data *data = (xpan_wifi_data *)ctx;

	if (!data) {
		ALOGE("%s: Error, xpan_wifi_data NULL", __func__);
		return;
	}

	data->cb = NULL;
}

static int no_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

static bool is_wifi_interface(const char *name)
{
	// filter out bridge interface
	if (strstr(name, "br") != NULL) {
		return false;
	}

	if (strncmp(name, "wlan", 4) != 0 && strncmp(name, "p2p", 3) != 0
			&& strncmp(name, "wifi", 4) != 0
			&& strncmp(name, "swlan", 5) != 0
			&& strncmp(name, "xsap", 4) != 0) {
		/* not a wifi interface; ignore it */
		return false;
	} else {
		return true;
	}
}

static int get_interface(const char *name, interface_info *info)
{
	if (!info)
		return WIFI_ERROR_UNKNOWN;

	strlcpy(info->name, name, (IFNAMSIZ + 1));
	info->id = if_nametoindex(name);

	return WIFI_SUCCESS;
}

/*
 * Initialise the wifi interfaces and store wifi interface indices in xpan
 * data. This information is required in creating the xpan interface.
 */

wifi_error wifi_init_interfaces(wifi_handle handle)
{
	if (!handle)
		return WIFI_ERROR_UNKNOWN;

	xpan_wifi_data *data = (xpan_wifi_data *)handle;
	struct dirent *de;

	DIR *d = opendir("/sys/class/net");
	if (d == 0)
		return WIFI_ERROR_UNKNOWN;

	int n = 0;
	while ((de = readdir(d))) {
		if (de->d_name[0] == '.')
			continue;
		if (is_wifi_interface(de->d_name) ) {
			n++;
		}
	}
	closedir(d);
	d = opendir("/sys/class/net");
	if (d == 0)
		return WIFI_ERROR_UNKNOWN;
	data->interfaces = (interface_info **)malloc(sizeof(interface_info *) *
						     n);
	if (data->interfaces == NULL) {
		ALOGE("%s: Error info->interfaces NULL", __func__);
		return WIFI_ERROR_OUT_OF_MEMORY;
	}

	int i = 0;
	while ((de = readdir(d))) {
		if (de->d_name[0] == '.')
			continue;
		if (is_wifi_interface(de->d_name)) {
			interface_info *ifinfo
				= (interface_info *)malloc(sizeof(interface_info));
			if (ifinfo == NULL) {
				ALOGE("%s: Error ifinfo NULL", __func__);
				while (i > 0) {
					free(data->interfaces[i-1]);
					i--;
				}
				free(data->interfaces);
				return WIFI_ERROR_OUT_OF_MEMORY;
			}
			if (get_interface(de->d_name, ifinfo) != WIFI_SUCCESS) {
				free(ifinfo);
				continue;
			}
			ifinfo->handle = handle;
			data->interfaces[i] = ifinfo;
			i++;
		}
	}

	closedir(d);
	data->num_interfaces = n;

	return WIFI_SUCCESS;
}

wifi_error wifi_reload_ifaces(wifi_handle handle)
{
	if (!handle)
		return WIFI_ERROR_UNKNOWN;

	xpan_wifi_data *data = (xpan_wifi_data *)handle;
	/* In case of dynamic interface add/remove, interface handles need to be
	 * updated so that, interface specific APIs could be instantiated.
	 * Reload here to get interfaces which are dynamically added. */

	if (data->num_interfaces > 0 && data->interfaces != NULL) {
		for (int i = 0; i < data->num_interfaces; i++) {
			if (data->interfaces[i] != NULL)
				free(data->interfaces[i]);
		}
		free(data->interfaces);
	}

	wifi_error ret = wifi_init_interfaces(handle);
	if (ret != WIFI_SUCCESS) {
		ALOGE("Failed to init interfaces while wifi_reload_ifaces");
		return ret;
	}

	return WIFI_SUCCESS;
}

class GetMulticastIdCommand : public WifiCommand
{
private:
	const char *mName;
	const char *mGroup;
	int   mId;
public:
	GetMulticastIdCommand(wifi_handle handle, const char *name,
			const char *group) : WifiCommand(handle, 0)
	{
		mName = name;
		mGroup = group;
		mId = -1;
	}

	int getId() {
		return mId;
	}

	virtual wifi_error create() {
		int nlctrlFamily = genl_ctrl_resolve(mInfo->nl_cmd, "nlctrl");
		if  (nlctrlFamily < 0)
			return mapKernelErrortoWifiHalError(nlctrlFamily);

		wifi_error ret = mMsg.create(nlctrlFamily, CTRL_CMD_GETFAMILY,
					     0, 0);
		if (ret != WIFI_SUCCESS)
			return ret;

		ret = mMsg.put_string(CTRL_ATTR_FAMILY_NAME, mName);
		return ret;
	}

	virtual int handleResponse(WifiEvent& reply) {
		struct nlattr **tb = reply.attributes();
		struct nlattr *mcgrp = NULL;
		int i;

		if (!tb[CTRL_ATTR_MCAST_GROUPS]) {
			ALOGI("No multicast groups found");
			return NL_SKIP;
		}

		for_each_attr(mcgrp, tb[CTRL_ATTR_MCAST_GROUPS], i) {
			struct nlattr *tb2[CTRL_ATTR_MCAST_GRP_MAX + 1];
			nla_parse(tb2, CTRL_ATTR_MCAST_GRP_MAX,
				  (nlattr *)nla_data(mcgrp),
				  nla_len(mcgrp), NULL);
			if (!tb2[CTRL_ATTR_MCAST_GRP_NAME] ||
			    !tb2[CTRL_ATTR_MCAST_GRP_ID])
			{
				continue;
			}

			char *grpName = (char *)nla_data(tb2[CTRL_ATTR_MCAST_GRP_NAME]);
			int grpNameLen = nla_len(tb2[CTRL_ATTR_MCAST_GRP_NAME]);


			if (strncmp(grpName, mGroup, grpNameLen) != 0)
				continue;

			mId = nla_get_u32(tb2[CTRL_ATTR_MCAST_GRP_ID]);
			break;
		}

		return NL_SKIP;
	}
};

static int wifi_get_multicast_id(wifi_handle handle, const char *name,
				 const char *group)
{
	GetMulticastIdCommand cmd(handle, name, group);
	int res = cmd.requestResponse();
	if (res < 0)
		return res;
	else
		return cmd.getId();
}

static int wifi_add_membership(wifi_handle handle, const char *group)
{
	xpan_wifi_data *data = (xpan_wifi_data *)handle;

	int id = wifi_get_multicast_id(handle, "nl80211", group);
	if (id < 0) {
		ALOGE("Could not find group %s", group);
	return id;
	}

	int ret = nl_socket_add_membership(data->nl_event, id);
	if (ret < 0) {
		ALOGE("Could not add membership to group %s", group);
	}

	return ret;
}

static bool is_xpan_supported(xpan_wifi_data *data, bool *flag)
{
	wifi_error ret = WIFI_ERROR_UNKNOWN;

	if (!data || data->num_interfaces < 1) {
		ALOGE("%s: xpan_wifi_data NULL or base interface not present", __func__);
		return false;
	}

	wifi_handle wifiHandle = getWifiXpanHandle(data);
	WifihalGeneric cmd(wifiHandle, get_requestid(), OUI_QCA,
			   QCA_NL80211_VENDOR_SUBCMD_GET_FEATURES);

	/* Create the NL message. */
	ret = cmd.create();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to create NL msg. Error:%d", __func__, ret);
		return false;
	}
	ret = cmd.put_u32(NL80211_ATTR_IFINDEX, data->interfaces[0]->id);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: Unable to put NL80211_ATTR_IFINDEX :%d", __func__, ret);
		return false;
	}
	cmd.setXpanFeaturePtr(flag);

	/* Send the NL msg. */
	ret = cmd.requestResponse();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: requestResponse Error:%d", __func__, ret);
		return false;
	}

	return *flag;
}

void* init_xpan_wifi_lib(wifi_handle *handle)
{
	wifi_error ret = WIFI_ERROR_UNKNOWN;
	struct nl_sock *cmd_sock = NULL;
	struct nl_sock *event_sock = NULL;
	struct nl_cb *cb = NULL;
	int status = 0, res = -1;
	int index;
	bool flag;

	if (!handle)
		return NULL;

	ALOGI("Initializing xpan_wifi_lib");
	xpan_wifi_data *data = (xpan_wifi_data *) malloc(sizeof(xpan_wifi_data));
	if (data == NULL) {
		ALOGE("Could not allocate xpan_data");
		return NULL;
	}
	memset(data, 0, sizeof(*data));
	cmd_sock = xpan_wifi_create_nl_socket(XPAN_WIFI_CMD_SOCK_PORT,
					      NETLINK_GENERIC);
	if (cmd_sock == NULL) {
		ALOGE("Could not create command sock port");
		free(data);
		return NULL;
	}
	event_sock = xpan_wifi_create_nl_socket(XPAN_WIFI_EVENT_SOCK_PORT,
						NETLINK_GENERIC);
	if (event_sock == NULL) {
		ALOGE("Could not create event sock port");
		goto unload;
	}

	/* Set the socket buffer size */
	if (nl_socket_set_buffer_size(event_sock, (256*1024), 0) < 0) {
		ALOGE("Could not set nl_socket RX buffer size for event_sock: %s",
				strerror(errno));
		/* continue anyway with the default (smaller) buffer */
	}

	cb = nl_socket_get_cb(event_sock);

	if (cb == NULL) {
		ALOGE("Failed to get NL control block for event socket port");
		goto unload;
	}

	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &data->event_sock_arg);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler,
		  &data->event_sock_arg);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler,
		  &data->event_sock_arg);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, internal_valid_message_handler,
		  data);
	nl_cb_put(cb);

	data->nl_cmd = cmd_sock;
	data->nl_event = event_sock;

	data->nl80211_family_id = genl_ctrl_resolve(cmd_sock, "nl80211");
	if (data->nl80211_family_id < 0) {
		ALOGE("Could not resolve nl80211 familty id");
		goto unload;
	}

	*handle = (wifi_handle) data;
	ret = wifi_init_interfaces(*handle);

	if (ret != WIFI_SUCCESS) {
		ALOGE("Failed to init interfaces");
		goto unload;
	}
	if (data->num_interfaces == 0) {
		ALOGE("No interfaces found");
		ret = WIFI_ERROR_UNINITIALIZED;
		goto unload;
	}
	data->exit_sockets[0] = -1;
	data->exit_sockets[1] = -1;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, data->exit_sockets) == -1) {
		ALOGE("Failed to create exit socket pair");
		ret = WIFI_ERROR_UNKNOWN;
		goto unload;
	}

	wifi_add_membership(*handle, "scan");
	wifi_add_membership(*handle, "mlme");
	wifi_add_membership(*handle, "regulatory");
	wifi_add_membership(*handle, "vendor");

	if (!is_xpan_supported(data, &flag)) {
		ALOGE("XPAN feature is disabled");
		goto unload;
	}
	ALOGI("enhance_audio_flag: %d", flag);
	ALOGI("Initialized Wifi_Xpan_Lib Successfully");

	return data;
unload:
	if (cmd_sock)
		nl_socket_free(cmd_sock);
	if (event_sock)
		nl_socket_free(event_sock);
	free(data);

	return NULL;
}

static struct nl_msg *prepare_nlmsg(xpan_wifi_data *data,
				    char *ifname, int cmdid, int subcmd,
				    int flag)
{
	int res;
	struct nl_msg *nlmsg = nlmsg_alloc();
	int ifindex;

	if (nlmsg == NULL) {
		ALOGE("Out of memory");
		return NULL;
	}

	genlmsg_put(nlmsg, /* pid = */ 0, /* seq = */ 0,
		    data->nl80211_family_id, 0, flag, cmdid, 0);

	if (cmdid == NL80211_CMD_VENDOR) {
		res = nla_put_u32(nlmsg, NL80211_ATTR_VENDOR_ID, OUI_QCA);
		if (res < 0) {
			ALOGE("Failed to put vendor id");
			goto cleanup;
		}

		res = nla_put_u32(nlmsg, NL80211_ATTR_VENDOR_SUBCMD, subcmd);
		if (res < 0) {
			ALOGE("Failed to put vendor sub command");
			goto cleanup;
		}
	}

	if (!(ifname && (strlen(ifname) > 0)))
		ifname = XPAN_DEFAULT_SAP_IFACE;

	ifindex = if_nametoindex(ifname);

	if (ifindex == 0) {
		ALOGE("Interface %s doesnot exist", ifname);
		goto cleanup;
	}

	if (nla_put_u32(nlmsg, NL80211_ATTR_IFINDEX, ifindex) != 0) {
		ALOGE("Failed to get iface index for iface: %s", ifname);
		goto cleanup;
	}

	return nlmsg;

cleanup:
	nlmsg_free(nlmsg);
	return NULL;
}

static struct nl_msg *prepare_vendor_nlmsg(xpan_wifi_data *data,
					   char *ifname, int subcmd)
{
	return prepare_nlmsg(data, ifname, NL80211_CMD_VENDOR, subcmd, 0);
}

static int send_nlmsg(struct nl_sock *cmd_sock, struct nl_msg *nlmsg,
		      nl_recvmsg_msg_cb_t customer_cb, void *arg)
{
	int err = 0;
	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);

	if (!cb)
		goto out;

	err = nl_send_auto_complete(cmd_sock, nlmsg);	/* send message */
	if (err < 0)
		goto out;

	err = 1;

	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
	if (customer_cb)
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, customer_cb, arg);

	while (err > 0) {				   /* wait for reply */
		int res = nl_recvmsgs(cmd_sock, cb);

		if (res)
			ALOGE("nl80211: %s->nl_recvmsgs failed: %d",
				 __func__, res);
	}
out:
	nl_cb_put(cb);
	nlmsg_free(nlmsg);
	return err;
}

static xpan_wifi_status xpan_wifi_send_oem_data_cmd(xpan_wifi_data *data,
						    char *ifname, u8 *oem_buf,
						    size_t oem_buf_len,
						    struct resp_info *info)
{
	struct nl_msg *nlmsg;
	struct nlattr *attr;
	int status;

	nlmsg = prepare_vendor_nlmsg(data, ifname,
				     QCA_NL80211_VENDOR_SUBCMD_OEM_DATA);
	if (!nlmsg) {
		ALOGE("Failed to allocate nl message");
		return XPAN_WIFI_STATUS_FAILURE;
	}

	attr = nla_nest_start(nlmsg, NL80211_ATTR_VENDOR_DATA);
	if (!attr) {
		ALOGE("Failed to put NL80211_ATTR_VENDOR_DATA");
		nlmsg_free(nlmsg);
		return XPAN_WIFI_STATUS_FAILURE;
	}

	if (nla_put(nlmsg, QCA_WLAN_VENDOR_ATTR_OEM_DATA_CMD_DATA,
		    oem_buf_len, oem_buf)) {
		ALOGE("Failed to put QCA_WLAN_VENDOR_ATTR_OEM_DATA_CMD_DATA");
		nlmsg_free(nlmsg);
		return XPAN_WIFI_STATUS_FAILURE;
	}

	nla_nest_end(nlmsg, attr);
	status = send_nlmsg((struct nl_sock *)data->nl_cmd, nlmsg,
			    NULL, NULL);

	if (status != 0) {
		ALOGE("Failed to send nl message with err %d", status);
		return XPAN_WIFI_STATUS_FAILURE;
	}

	return XPAN_WIFI_STATUS_SUCCESS;
}

static u16 oem_tlv_write_tlv(struct oem_data_tlv *in_tlv, u8 *out_tlv)
{
	u16 write_len = 0;
	u16 i;

	if (!in_tlv) {
		ALOGE("NULL in_tlv");
		return write_len;
	}

	if (!out_tlv) {
		ALOGE("NULL out_tlv");
		return write_len;
	}

	*out_tlv++ = in_tlv->type & 0xFF;
	*out_tlv++ = (in_tlv->type & 0xFF00) >> 8;
	write_len += 2;

	*out_tlv++ = in_tlv->length & 0xFF;
	*out_tlv++ = (in_tlv->length & 0xFF00) >> 8;
	write_len += 2;

	for (i = 0; i < in_tlv->length; ++i)
		*out_tlv++ = in_tlv->value[i];

	write_len += in_tlv->length;

	ALOGV("WRITE OEM TLV type %u, length %u, write_len %u",
	      in_tlv->type, in_tlv->length, write_len);

	return write_len;
}

static u8 *add_tlv(u16 type, u16 length, const u8 *value, u8 *out_tlv)
{
	struct oem_data_tlv oem_tlv;
	u16 len;

	oem_tlv.type = type;
	oem_tlv.length = length;
	oem_tlv.value = (u8 *)value;

	len = oem_tlv_write_tlv(&oem_tlv, out_tlv);

	return (out_tlv + len);
}

static u8 *init_oem_data_cmd_hdr(u8 *in_tlv, enum oem_data_cmd cmd)
{
	struct oem_data_header oem_hdr;
	static u16 request_idx = 0;

	ALOGI("oem_data_cmd=%d", cmd);

	oem_hdr.cmd_id = cmd;
	oem_hdr.request_idx = request_idx++;

	return add_tlv(OEM_DATA_TLV_TYPE_HEADER, sizeof(oem_hdr),
		       (u8 *)&oem_hdr, in_tlv);
}

static u8 *parse_oem_data_stats_cmd_tlv(u8 *in_tlv, u8 enable, u32 interval)
{
	u8 *out_tlv;
	out_tlv = add_tlv(OEM_DATA_TLV_TYPE_ENABLE,
			  sizeof(enable), (u8 *)&enable, in_tlv);
	out_tlv = add_tlv(OEM_DATA_TLV_TYPE_XPAN_STATS_INTERVAL,
			  sizeof(interval), (u8 *)&interval, out_tlv);

	return out_tlv;

}

xpan_wifi_status set_xpan_wifi_stats_enabled(void *ctx, bool enable,
					     int interval)
{
	xpan_wifi_data *data = (xpan_wifi_data* )ctx;
	u8 *oem_buf, *tlvs;
	size_t oem_buf_len;
	xpan_wifi_status ret = XPAN_WIFI_STATUS_FAILURE;
	iface_info *iface;

	if (!data) {
		ALOGE("%s: Error, xpan_wifi_data NULL", __func__);
		return ret;
	}

	oem_buf_len = SIZEOF_TLV_HDR + sizeof(struct oem_data_header)
		    + SIZEOF_TLV_HDR + sizeof(u8)	/* enable */
		    + SIZEOF_TLV_HDR + sizeof(u32);	/* interval */
	oem_buf = (u8 *)malloc(oem_buf_len);
	if (!oem_buf) {
		ALOGE("Failed to malloc oem_buf for ENABLE_XPAN_STATS");
		return ret;
	}

	tlvs = oem_buf;
	tlvs = init_oem_data_cmd_hdr(tlvs, OEM_CMD_ENABLE_STATS);
	tlvs = parse_oem_data_stats_cmd_tlv(tlvs, enable,interval);

	if (!tlvs)
		goto out;

	iface = get_ifaceinfo_from_list(data->ifinfo, data->cmd_iface);
	if (!iface) {
		ALOGE("%s no iface found for iface_type:%d",
		      __func__, data->cmd_iface);
		return XPAN_WIFI_STATUS_FAILURE;
	}
	ret = xpan_wifi_send_oem_data_cmd(data, iface->ifname, oem_buf,
					  oem_buf_len, NULL);

	if (ret == XPAN_WIFI_STATUS_SUCCESS)
		ALOGI("xpan_wifi_stats enable:%d, interval:%d sent to FW", enable, interval);
out:
	free(oem_buf);
	return ret;
}

static u8 *parse_oem_data_usecase_cmd_tlv(u8 *in_tlv,
					  struct xpan_usecase_params *params)
{
	u8 *out_tlv;
	u8 mode;

	mode = params->mode;
	out_tlv = add_tlv(OEM_DATA_TLV_TYPE_XPAN_USECASE,
			  sizeof(mode), (u8 *)&mode, in_tlv);

	out_tlv = add_tlv(OEM_DATA_TLV_TYPE_XPAN_VBC_SI,
			  sizeof(params->xpan_gaming_vbc_si),
			  &(params->xpan_gaming_vbc_si), out_tlv);

	out_tlv = add_tlv(OEM_DATA_TLV_TYPE_XPAN_RIGHT_EARBUD_OFFSET,
			  sizeof(params->xpan_right_earbud_offset),
			  (u8 *)&(params->xpan_right_earbud_offset), out_tlv);

	out_tlv = add_tlv(OEM_DATA_TLV_TYPE_XPAN_LEFT_EARBUD_MAC,
			  sizeof(params->left_mac_addr),
			  params->left_mac_addr, out_tlv);

	out_tlv = add_tlv(OEM_DATA_TLV_TYPE_XPAN_RIGHT_EARBUD_MAC,
			  sizeof(params->right_mac_addr),
			  params->right_mac_addr, out_tlv);

	return out_tlv;

}

xpan_wifi_status set_xpan_usecase_params(void *ctx,
					 struct xpan_usecase_params *params)
{
	u8 use_case;
	wifi_error ret;
	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	struct nlattr *nlData = NULL;
	WiFiConfigCommand *wifiConfigCommand;
	u8 *oem_buf = NULL, *tlvs;
	size_t oem_buf_len;
	xpan_wifi_status oem_ret = XPAN_WIFI_STATUS_FAILURE;
	iface_info *iface;

	xpan_wifi_data *data = (xpan_wifi_data *)ctx;
	if (!data || data->num_interfaces < 1) {
		ALOGE("%s: Error xpan_handle NULL or base interface not present", __func__);
		return  XPAN_WIFI_STATUS_FAILURE;
	}

	wifi_handle handle = getWifiXpanHandle(data);
	iface = get_ifaceinfo_from_list(data->ifinfo, data->cmd_iface);
	if (!iface) {
		ALOGE("%s no iface found for iface_type:%d",
		      __func__, data->cmd_iface);
		return XPAN_WIFI_STATUS_FAILURE;
	}

	if (data->cmd_iface == IFACE_TYPE_STA) {
		// mark Vendor status as success and skip vendor command
		status = XPAN_WIFI_STATUS_SUCCESS;
		goto set_oem_usecase;
	}

	wifiConfigCommand = new WiFiConfigCommand(handle, get_requestid(),
						  OUI_QCA,
						  QCA_NL80211_VENDOR_SUBCMD_CONCURRENT_POLICY);


	wifiConfigCommand->create();
	if (wifiConfigCommand == NULL) {
		ALOGE("%s: Error wifiConfigCommand NULL", __func__);
		return  XPAN_WIFI_STATUS_FAILURE;
	}
	if (wifiConfigCommand->put_u32(NL80211_ATTR_IFINDEX,
				       if_nametoindex(iface->ifname))) {
		ALOGE("%s: Failed to put iface id", __func__);
		goto cleanup;
	}

	switch (params->mode) {
	case XPAN_USECASE_NONE:
		use_case = QCA_WLAN_CONCURRENT_AP_POLICY_UNSPECIFIED;
		break;
	case XPAN_USECASE_GAMING:
		use_case = QCA_WLAN_CONCURRENT_AP_POLICY_GAMING_AUDIO;
		break;
	case XPAN_USECASE_LOSSLESS_STREAMING:
		use_case = QCA_WLAN_CONCURRENT_AP_POLICY_LOSSLESS_AUDIO_STREAMING;
		break;
	case XPAN_USECASE_GAMING_VBC:
		//send gaming usecase to driver in case of vbc
		use_case = QCA_WLAN_CONCURRENT_AP_POLICY_GAMING_AUDIO;
		break;
	default:
		use_case = QCA_WLAN_CONCURRENT_AP_POLICY_UNSPECIFIED;
		break;
	}

	nlData = wifiConfigCommand->attr_start(NL80211_ATTR_VENDOR_DATA);
	if (!nlData) {
		ALOGE("%s: failed attr_start for NL80211_ATTR_VENDOR_DATA", __func__);
		goto cleanup;
	}
	ret = wifiConfigCommand->put_u8(QCA_WLAN_VENDOR_ATTR_CONCURRENT_POLICY_AP_CONFIG,
				  use_case);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: Unable to put vendor attribute%d", __func__,ret);
		goto cleanup;
	}

	wifiConfigCommand->attr_end(nlData);

	/* Send the NL msg. */
	ret = wifiConfigCommand->sendCommand();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: sendCommand Error:%d", __func__,ret);
		goto cleanup;
	}
	ALOGI("Successfully sent usecase params to driver");
	status = XPAN_WIFI_STATUS_SUCCESS;

	//Also send oem command
set_oem_usecase:
	oem_buf_len = SIZEOF_TLV_HDR + sizeof(struct oem_data_header)
		+ SIZEOF_TLV_HDR + sizeof(u8)	/* mode */
		+ SIZEOF_TLV_HDR + sizeof(u8)	/* vbc_si */
		+ SIZEOF_TLV_HDR + sizeof(u32)	/* offset */
		+ SIZEOF_TLV_HDR + 6 * sizeof(u8)	/* left_mac_addr */
		+ SIZEOF_TLV_HDR + 6 * sizeof(u8);	/* right_mac_addr */
	oem_buf = (u8 *)malloc(oem_buf_len);
	if (!oem_buf) {
		ALOGE("Failed to malloc oem_buf for SET_USECASE_PARAMS");
		return XPAN_WIFI_STATUS_FAILURE;
	}

	tlvs = oem_buf;
	tlvs = init_oem_data_cmd_hdr(tlvs, OEM_CMD_SET_USECASE_PARAMS);
	tlvs = parse_oem_data_usecase_cmd_tlv(tlvs, params);

	if (!tlvs)
		goto cleanup;

	oem_ret = xpan_wifi_send_oem_data_cmd(data, iface->ifname, oem_buf,
					      oem_buf_len, NULL);
	if (oem_ret == XPAN_WIFI_STATUS_SUCCESS)
		ALOGI("Successfully sent usecase params to FW");

cleanup:
	free(oem_buf);
	delete wifiConfigCommand;
	if(status == XPAN_WIFI_STATUS_SUCCESS &&
	   oem_ret == XPAN_WIFI_STATUS_SUCCESS) {
		return XPAN_WIFI_STATUS_SUCCESS;
	} else {
		return XPAN_WIFI_STATUS_FAILURE;
	}

}

xpan_wifi_status wifi_create_ap_iface(void *ctx, char *ifname)
{
	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	WiFiConfigCommand *wifiConfigCommand;
	struct ifreq ifr;
	int sock;
	xpan_wifi_data *data = (xpan_wifi_data *)ctx;
	wifi_handle handle = getWifiXpanHandle(data);
	iface_info *ifinfo, *iface;
	char *new_ifname = XPAN_DEFAULT_SAP_IFACE;

	if (!data || data->num_interfaces < 1) {
		ALOGE("%s: Error wifi_handle NULL or base interface not present",
		       __func__);
		return status;
	}

	char buf[PROPERTY_VALUE_MAX] = {};
	if (property_get("vendor.wifi.lohs.sap.interface", buf, "") != 0
			 && if_nametoindex(buf)) {
		ALOGE("xsap interface %s is already present", buf);
		return XPAN_WIFI_STATUS_FAILURE_EEXIST;
	}

	iface = get_ifaceinfo_from_list(data->ifinfo, IFACE_TYPE_SAP);
	if (iface == NULL) {
		if (ifname && strlen(ifname) > 0)
			new_ifname = ifname;
	} else {
		ALOGE("%s: SAP iface:%s exists", __func__, iface->ifname);
		return XPAN_WIFI_STATUS_FAILURE_EEXIST;
	}

	wifiConfigCommand = new WiFiConfigCommand(handle, get_requestid(),
						  0, 0);
	if (wifiConfigCommand == NULL) {
		ALOGE("%s: Error wifiConfigCommand NULL", __func__);
		return status;
	}

	wifi_error ret = wifiConfigCommand->create_generic(NL80211_CMD_NEW_INTERFACE);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: nlcommand_create Error:%d", __func__, ret);
		goto done;
	}
	ret = wifiConfigCommand->put_u32(NL80211_ATTR_IFINDEX,
					 data->interfaces[0]->id);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: Unable to put NL80211_ATTR_IFINDEX :%d", __func__, ret);
		goto done;
	}
	ret = wifiConfigCommand->put_string(NL80211_ATTR_IFNAME, new_ifname);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: Unable to put NL80211_ATTR_IFNAME :%d", __func__, ret);
		goto done;
	}
	ret = wifiConfigCommand->put_u32(NL80211_ATTR_IFTYPE,
					 NL80211_IFTYPE_AP);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: Unable to put NL80211_ATTR_IFTYPE :%d", __func__, ret);
		goto done;
	}

	/* Send the NL msg. */
	ret = wifiConfigCommand->sendCommand();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: sendCommand Error:%d", __func__,ret);
		goto done;
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		ALOGE("%s :socket error, Failed to bring up iface \n", __func__);
		goto done;
	}
	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, new_ifname, sizeof(ifr.ifr_name));
	if (ioctl(sock, SIOCGIFFLAGS, &ifr) != 0) {
		close(sock);
		ALOGE("%s :Could not read interface %s flags \n", __func__, new_ifname);
		goto done;
	}
	ifr.ifr_flags |= IFF_UP;
	if (ioctl(sock, SIOCSIFFLAGS, &ifr) != 0) {
		close(sock);
		ALOGE("%s :Could not bring iface %s up \n", __func__, new_ifname);
		goto done;
	}

	close(sock);

	//setting vendor property for xpan interface
	property_set("vendor.wifi.lohs.sap.interface", new_ifname);
	property_set("vendor.wifi.lohs.sap.iface.inuse", "true");

	ifinfo = (iface_info *)malloc(sizeof(iface_info));
	if (!ifinfo) {
		ALOGE("%s: Error, ifinfo NULL", __func__);
		return status;
	}

	strlcpy(ifinfo->ifname, new_ifname, sizeof(ifinfo->ifname));
	ifinfo->if_index = if_nametoindex(new_ifname);
	ifinfo->type = IFACE_TYPE_SAP;
	add_iface_node(data, ifinfo);

	//set usecase params
	xpan_usecase_params params;
	memset(&params, 0, sizeof(params));
	params.mode = XPAN_USECASE_LOSSLESS_STREAMING;
	data->cmd_iface = IFACE_TYPE_SAP;
	if(set_xpan_usecase_params(data, &params) != XPAN_WIFI_STATUS_SUCCESS)
		ALOGE("Unable to set default usecase for the interface");

	//update interface info in xpan_data
	ret = wifi_reload_ifaces(handle);
	if (ret != WIFI_SUCCESS) {
		ALOGE("Failed to update interface info");
		goto done;
	}

	status = XPAN_WIFI_STATUS_SUCCESS;
	ALOGI("Created xsap interface: %s", ifinfo->ifname);
	print_iface_list(data->ifinfo);

done:
	delete wifiConfigCommand;
	return status;
}

xpan_wifi_status wifi_delete_ap_iface(void *ctx)
{
	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	WiFiConfigCommand *wifiConfigCommand;
	xpan_wifi_data *data = (xpan_wifi_data *)ctx;
	iface_info ifinfo, *ifsap;

	if (!data) {
		ALOGE("%s: Error xpan_wifi_data NULL", __func__);
		return status;
	}

	wifi_handle handle = getWifiXpanHandle(data);

	if (!handle) {
		ALOGE("%s: Error wifi_handle NULL", __func__);
		return status;
	}

	ifsap = get_ifaceinfo_from_list(data->ifinfo, IFACE_TYPE_SAP);
	if (ifsap == NULL) {
		//check for xsap interface in vendor properties
		char buf[PROPERTY_VALUE_MAX] = {};
		if (property_get("vendor.wifi.lohs.sap.interface", buf, "") != 0) {
			strlcpy(ifinfo.ifname, buf, sizeof(ifinfo.ifname));
		} else {
			ALOGI("xsap interface is not present");
			return XPAN_WIFI_STATUS_SUCCESS;
		}
	} else {
		strlcpy(ifinfo.ifname, ifsap->ifname, sizeof(ifinfo.ifname));
		remove_iface_node(data, ifsap);
	}
	ifinfo.if_index = if_nametoindex(ifinfo.ifname);

	ALOGD("%s: ifname=%s delete", __func__, ifinfo.ifname);

	wifiConfigCommand = new WiFiConfigCommand(handle, get_requestid(),
						  0, 0);
	if (wifiConfigCommand == NULL) {
		ALOGE("%s: Error wifiConfigCommand NULL", __func__);
		return status;
	}
	wifi_error ret = wifiConfigCommand->create_generic(NL80211_CMD_DEL_INTERFACE);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: nlcommand_create error:%d", __func__, ret);
		goto done;
	}
	ret = wifiConfigCommand->put_u32(NL80211_ATTR_IFINDEX, ifinfo.if_index);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: Unable to put NL80211_ATTR_IFINDEX %d", __func__, ret);
		goto done;
	}
	/* Send the NL msg. */
	ret = wifiConfigCommand->sendCommand();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: sendCommand Error:%d", __func__, ret);
	} else {
		ALOGI("Deleted xsap interface");
		//unset the xsap iface vendor properties
		property_set("vendor.wifi.lohs.sap.iface.inuse","false");
		property_set("vendor.wifi.lohs.sap.interface","");

		//update interface info in xpan_data
		ret = wifi_reload_ifaces(handle);
		if (ret != WIFI_SUCCESS) {
			ALOGE("Failed to update interface info");
			goto done;
		}

		status = XPAN_WIFI_STATUS_SUCCESS;
	}

done:
	delete wifiConfigCommand;
	return status;
}

xpan_wifi_status wifi_do_acs(void *ctx, int *freq_list, int freq_list_size)
{

	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	struct nlattr *nlData = NULL;
	wifi_handle wifiHandle = NULL;
	int i;
	int hw_mode = HOSTAPD_MODE_IEEE80211A;
	u16 ch_width = ACS_CHANNEL_WIDTH_DEFAULT;
	int age_out = SCAN_AGE_OUT_DEFAULT;
	u32 *freqs;
	bool is2Ghz = false, is5Ghz = false;
	wifi_error ret;
	iface_info *iface;
	iface_type iftype;

	xpan_wifi_data *data = (xpan_wifi_data*)ctx;
	if (!data) {
		ALOGE("%s: Error, xpan_wifi_data NULL", __func__);
		return status;
	} else if (data->cmd_iface != IFACE_TYPE_SAP) {
		ALOGE("%s: command not supported for iface type %d",
		      __func__, data->cmd_iface);
		return status;
	} else if (!freq_list || freq_list_size <= 0) {
		ALOGE("%s: Error, Invalid Frequency list. size %d",
		      __func__, freq_list_size);
		return status;
	}

	wifiHandle = getWifiXpanHandle(data);
	WifihalGeneric cmd(wifiHandle, get_requestid(), OUI_QCA,
				 QCA_NL80211_VENDOR_SUBCMD_DO_ACS);

	freqs = (u32 *)malloc(sizeof(u32) * freq_list_size);
	if (!freqs)
		return status;

	for (i = 0; i < freq_list_size; i++) {
		freqs[i] = freq_list[i];
		if (freqs[i] > 5000)
			is5Ghz = true;
		else if (freqs[i] > 2000)
			is2Ghz = true;
	}

	if (is5Ghz && is2Ghz) {
		hw_mode = HOSTAPD_MODE_IEEE80211ANY;
	} else if (is5Ghz) {
		hw_mode = HOSTAPD_MODE_IEEE80211A;
	} else if (is2Ghz) {
		hw_mode = HOSTAPD_MODE_IEEE80211G;
	} else {
		ALOGE("%s: Invalid band");
		goto cleanup;
	}

	ALOGI("ACS Params: HW_MODE: %d HT: 1 VHT: 1 BW: %d SCAN_AGE: %d",
	      hw_mode, ch_width, age_out);

	/* Create the NL message. */
	ret = cmd.create();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to create NL msg. Error:%d", __func__, ret);
		goto cleanup;
	}

	iface = get_ifaceinfo_from_list(data->ifinfo, IFACE_TYPE_SAP);
	if (!iface) {
		ALOGE("%s iface is not present in the list", __func__);
		return XPAN_WIFI_STATUS_FAILURE;
	}

	cmd.set_iface_id(iface->ifname);

	/* Add the vendor specific attributes for the NL command. */
	nlData = cmd.attr_start(NL80211_ATTR_VENDOR_DATA);
	if (!nlData) {
		ALOGE("%s: failed attr_start for VENDOR_DATA", __func__);
		goto cleanup;
	}

	ret = cmd.put_u8(QCA_WLAN_VENDOR_ATTR_ACS_HW_MODE, hw_mode);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		goto cleanup;
	}

	ret = cmd.put_flag(QCA_WLAN_VENDOR_ATTR_ACS_HT_ENABLED);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		goto cleanup;
	}

	ret = cmd.put_flag(QCA_WLAN_VENDOR_ATTR_ACS_VHT_ENABLED);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		goto cleanup;
	}

	ret = cmd.put_u16(QCA_WLAN_VENDOR_ATTR_ACS_CHWIDTH, ch_width);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		goto cleanup;
	}

	ret = cmd.put_list(QCA_WLAN_VENDOR_ATTR_ACS_FREQ_LIST,
			   sizeof(u32) * freq_list_size, freqs);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		goto cleanup;
	}

	ret = cmd.put_u32(QCA_WLAN_VENDOR_ATTR_ACS_LAST_SCAN_AGEOUT_TIME, age_out);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		goto cleanup;
	}

	cmd.attr_end(nlData);

	if(data->cb)
		cmd.setWifiCallbackHandler(*(data->cb));
	else
		ALOGE("%s: callback not set in XpanWifiHAL", __func__);
	/* Send the NL msg. */
	ret = cmd.requestResponse();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: requestResponse Error:%d", __func__, ret);
		goto cleanup;
	}
	ALOGI("DO_ACS sent on xsap interface");

	status = XPAN_WIFI_STATUS_SUCCESS;

cleanup:
	free(freqs);
	return status;
}

xpan_wifi_status deinit_xpan_wifi_lib(void *ctx)
{
	xpan_wifi_data *data = (xpan_wifi_data *)ctx;

	if (!data)
		return XPAN_WIFI_STATUS_FAILURE;

	if (data->nl_cmd) {
		nl_socket_free(data->nl_cmd);
	}

	if (data->nl_event) {
		nl_socket_free(data->nl_event);
	}

	if (data->exit_sockets[0] >= 0) {
		close(data->exit_sockets[0]);
	}

	if (data->exit_sockets[1] >= 0) {
		close(data->exit_sockets[1]);
	}

	delete_iface_list(data);

	free(data);
	return XPAN_WIFI_STATUS_SUCCESS;
}


xpan_wifi_status set_xpan_ap_power_save(void *ctx, bool enable,
					uint64_t *cookie)
{

	struct nlattr *nlData = NULL;
	wifi_handle wifiHandle = NULL;
	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	xpan_wifi_data *data = (xpan_wifi_data*)ctx;
	iface_info *ifsap;

	if (!data) {
		ALOGE("%s: Error, xpan_wifi_data NULL", __func__);
		return status;
	} else if (data->cmd_iface != IFACE_TYPE_SAP) {
		ALOGE("%s: command not supported for iface type %d",
		      __func__, data->cmd_iface);
		return status;
	}

	ifsap = get_ifaceinfo_from_list(data->ifinfo, IFACE_TYPE_SAP);
	if (ifsap == NULL) {
		ALOGE("%s: SAP iface doesn't exists", __func__);
		return status;
	}

	wifiHandle = getWifiXpanHandle(data);
	WifihalGeneric cmd(wifiHandle, get_requestid(), OUI_QCA,
			   QCA_NL80211_VENDOR_SUBCMD_DOZED_AP);

	/* Create the NL message. */
	wifi_error ret = cmd.create();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to create NL msg. Error:%d", __func__, ret);
		return status;
	}

	cmd.set_iface_id(ifsap->ifname);
	cmd.setCookiePointer(cookie);
	/* Add the vendor specific attributes for the NL command. */
	nlData = cmd.attr_start(NL80211_ATTR_VENDOR_DATA);
	if (!nlData) {
		ALOGE("%s: failed attr_start for VENDOR_DATA", __func__);
		return status;
	}

	if (enable)
		ret = cmd.put_u8(QCA_WLAN_VENDOR_ATTR_DOZED_AP_STATE,
						QCA_WLAN_DOZED_AP_ENABLE);
	else
		ret = cmd.put_u8(QCA_WLAN_VENDOR_ATTR_DOZED_AP_STATE,
						QCA_WLAN_DOZED_AP_DISABLE);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		return status;
	}

	cmd.attr_end(nlData);

	/* Send the NL msg. */
	ret = cmd.requestResponse();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: requestResponse Error:%d", __func__, ret);
		return status;
	}

	ALOGI("Setting ap power save mode is successful");
	if (cookie)
		ALOGI("Cookie Id: %llu", *cookie);
	return XPAN_WIFI_STATUS_SUCCESS;
}

xpan_wifi_status xpan_audio_transport_switch(void *ctx,
					     transport_switch_type switch_type,
					     transport_switch_status switch_status)
{
	struct nlattr *nlData = NULL;
	wifi_handle wifiHandle = NULL;
	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	xpan_wifi_data *data = (xpan_wifi_data*)ctx;
	iface_info *iface;
	iface_type iftype;

	if (!data) {
		ALOGE("%s: Error, xpan_wifi_data NULL", __func__);
		return status;
	} else if (data->cmd_iface != IFACE_TYPE_SAP) {
		ALOGE("%s: command not supported for iface type %d",
		      __func__, data->cmd_iface);
		return status;
	}

	wifiHandle = getWifiXpanHandle(data);
	WifihalGeneric cmd(wifiHandle, get_requestid(), OUI_QCA,
			   QCA_NL80211_VENDOR_SUBCMD_AUDIO_TRANSPORT_SWITCH);

	/* Create the NL message. */
	wifi_error ret = cmd.create();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to create NL msg. Error:%d", __func__, ret);
		return status;
	}

	iface = get_ifaceinfo_from_list(data->ifinfo, data->cmd_iface);
	if (!iface) {
		ALOGE("%s iface is not present in the list", __func__);
		return XPAN_WIFI_STATUS_FAILURE;
	}
	cmd.set_iface_id(iface->ifname);

	/* Add the vendor specific attributes for the NL command. */
	nlData = cmd.attr_start(NL80211_ATTR_VENDOR_DATA);
	if (!nlData) {
		ALOGE("%s: failed attr_start for VENDOR_DATA", __func__);
		return status;
	}

	switch (switch_type) {
	case TRANSPORT_SWITCH_TYPE_NON_WLAN:
		ret = cmd.put_u8(QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_TYPE,
				 QCA_WLAN_AUDIO_TRANSPORT_SWITCH_TYPE_NON_WLAN);
		break;
	case TRANSPORT_SWITCH_TYPE_WLAN:
		ret = cmd.put_u8(QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_TYPE,
				 QCA_WLAN_AUDIO_TRANSPORT_SWITCH_TYPE_WLAN);
		break;
	default:
		ALOGE("Requested invalid switch type");
		return status;
	}
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		return status;
	}
	switch (switch_status) {
	case TRANSPORT_SWITCH_STATUS_REQUEST:
		break;
	case TRANSPORT_SWITCH_STATUS_REJECTED:
		ret = cmd.put_u8(QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_STATUS,
				 QCA_WLAN_AUDIO_TRANSPORT_SWITCH_STATUS_REJECTED);
		break;
	case TRANSPORT_SWITCH_STATUS_COMPLETED:
		ret = cmd.put_u8(QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_STATUS,
				 QCA_WLAN_AUDIO_TRANSPORT_SWITCH_STATUS_COMPLETED);
		break;
	default:
		ALOGE("Received invalid switch status");
		return status;
	}
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		return status;
	}

	cmd.attr_end(nlData);

	/* Send the NL msg. */
	ret = cmd.requestResponse();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: requestResponse Error:%d", __func__, ret);
		return status;
	}

	ALOGI("Sent switch type: %d and status: %d to driver", switch_type, switch_status);
	return XPAN_WIFI_STATUS_SUCCESS;

}

xpan_wifi_status set_xpan_ap_available(void *ctx, uint32_t duration,
				       uint16_t *cookie)
{
	struct nlattr *nlData = NULL;
	wifi_handle wifiHandle = NULL;
	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	xpan_wifi_data *data = (xpan_wifi_data*)ctx;
	iface_info *iface;

	if (!data) {
		ALOGE("%s: Error, xpan_wifi_data NULL", __func__);
		return status;
	} else if (data->cmd_iface != IFACE_TYPE_SAP) {
		ALOGE("%s: command not supported for iface type %d",
		      __func__, data->cmd_iface);
		return status;
	}

	wifiHandle = getWifiXpanHandle(data);
	WifihalGeneric cmd(wifiHandle, get_requestid(), OUI_QCA,
			   QCA_NL80211_VENDOR_SUBCMD_HIGH_AP_AVAILABILITY);

	/* Create the NL message. */
	wifi_error ret = cmd.create();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to create NL msg. Error:%d", __func__, ret);
		return status;
	}

	iface = get_ifaceinfo_from_list(data->ifinfo, IFACE_TYPE_SAP);
	if (!iface) {
		ALOGE("%s iface is not present in the list", __func__);
		return XPAN_WIFI_STATUS_FAILURE;
	}
	cmd.set_iface_id(iface->ifname);
	cmd.setCookiePointer16(cookie);
	/* Add the vendor specific attributes for the NL command. */
	nlData = cmd.attr_start(NL80211_ATTR_VENDOR_DATA);
	if (!nlData) {
		ALOGE("%s: failed attr_start for VENDOR_DATA", __func__);
		return status;
	}

	ret = cmd.put_u8(QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_OPERATION,
			 QCA_HIGH_AP_AVAILABILITY_OPERATION_REQUEST);
	if (ret != WIFI_SUCCESS) {
		ALOGE("Failed to put vendor data(QCA_HIGH_AP_AVAILABILITY_OPERATION_REQUEST)");
		return status;
	}

	ret = cmd.put_u32(QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_DURATION,
			  duration);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: Failed to put vendor data(duration). Error:%d", __func__, ret);
		return status;
	}

	cmd.attr_end(nlData);

	/* Send the NL msg. */
	ret = cmd.requestResponse();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: requestResponse Error:%d", __func__, ret);
		return status;
	}

	ALOGI("Setting xpan ap availability is successful");
	if (cookie)
		ALOGI("Cookie received for XPAN_HIGH_AP_AVAILABILITY:: %hu", *cookie);
	return XPAN_WIFI_STATUS_SUCCESS;
}

xpan_wifi_status cancel_xpan_ap_available(void *ctx, uint16_t cookie)
{
	struct nlattr *nlData = NULL;
	wifi_handle wifiHandle = NULL;
	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	xpan_wifi_data *data = (xpan_wifi_data*)ctx;
	iface_info *iface;

	if (!data) {
		ALOGE("%s: Error, xpan_wifi_data NULL", __func__);
		return status;
	} else if (data->cmd_iface != IFACE_TYPE_SAP) {
		ALOGE("%s: command not supported for iface type %d",
		      __func__, data->cmd_iface);
		return status;
	}

	wifiHandle = getWifiXpanHandle(data);
	WifihalGeneric cmd(wifiHandle, get_requestid(), OUI_QCA,
			   QCA_NL80211_VENDOR_SUBCMD_HIGH_AP_AVAILABILITY);

	/* Create the NL message. */
	wifi_error ret = cmd.create();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to create NL msg. Error:%d", __func__, ret);
		return status;
	}

	iface = get_ifaceinfo_from_list(data->ifinfo, IFACE_TYPE_SAP);
	if (!iface) {
		ALOGE("%s iface is not present in the list", __func__);
		return XPAN_WIFI_STATUS_FAILURE;
	}
	cmd.set_iface_id(iface->ifname);
	/* Add the vendor specific attributes for the NL command. */
	nlData = cmd.attr_start(NL80211_ATTR_VENDOR_DATA);
	if (!nlData) {
		ALOGE("%s: failed attr_start for VENDOR_DATA", __func__);
		return status;
	}

	ret = cmd.put_u8(QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_OPERATION,
					QCA_HIGH_AP_AVAILABILITY_OPERATION_CANCEL);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data. Error:%d", __func__, ret);
		return status;
	}

	ret = cmd.put_u16(QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE, cookie);
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: failed to put vendor data(cookie). Error:%d", __func__, ret);
		return status;
	}
	cmd.attr_end(nlData);

	/* Send the NL msg. */
	ret = cmd.requestResponse();
	if (ret != WIFI_SUCCESS) {
		ALOGE("%s: requestResponse Error:%d", __func__, ret);
		return status;
	}

	ALOGI("cancelling xpan ap availability is successful");
	return XPAN_WIFI_STATUS_SUCCESS;
}

static u8 *parse_oem_data_audio_pair_cmd_tlv(u8 *in_tlv,
					  struct xpan_audio_pair_params *params)
{
	u8 *out_tlv;
	u8 peer_id;

	if (params == NULL) {
		ALOGE("Invalid xpan_audio_pair_params");
		return NULL;
	}

	peer_id = params->xpan_peer_set_id;
	if (peer_id >= 255) {
		ALOGE("Invalid xpan_peer_set_id");
		return NULL;
	}
	out_tlv = add_tlv(OEM_DATA_TLV_TYPE_XPAN_EB_PAIR_INDEX,
			  sizeof(peer_id), (u8 *)&peer_id, in_tlv);

	out_tlv = add_tlv(OEM_DATA_TLV_TYPE_XPAN_LEFT_EARBUD_MAC,
			  sizeof(params->left_mac_addr),
			  params->left_mac_addr, out_tlv);

	out_tlv = add_tlv(OEM_DATA_TLV_TYPE_XPAN_RIGHT_EARBUD_MAC,
			  sizeof(params->right_mac_addr),
			  params->right_mac_addr, out_tlv);

	return out_tlv;
}

xpan_wifi_status set_xpan_peer_audio_pair_location(void *ctx,
						      struct xpan_audio_pair_params *params)
{
	u8 *oem_buf = NULL, *tlvs;
	size_t oem_buf_len;
	xpan_wifi_data *data = (xpan_wifi_data *)ctx;
	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	iface_info *iface;

	if (!data) {
		ALOGE("%s: Error xpan_wifi_data NULL", __func__);
		return status;
	}

	iface = get_ifaceinfo_from_list(data->ifinfo, data->cmd_iface);
	if (!iface) {
		ALOGE("%s iface is not present in the list", __func__);
		return XPAN_WIFI_STATUS_FAILURE;
	}

	oem_buf_len = SIZEOF_TLV_HDR + sizeof(struct oem_data_header)
		      + SIZEOF_TLV_HDR + sizeof(u8)		/* set_id */
		      + SIZEOF_TLV_HDR + 6 * sizeof(u8)		/* left_mac_addr */
		      + SIZEOF_TLV_HDR + 6 * sizeof(u8);	/* right_mac_addr */
	oem_buf = (u8 *)malloc(oem_buf_len);
	if (!oem_buf) {
		ALOGE("Failed to malloc oem_buf for SET_AUDIO_PAIR_PARAMS");
		return XPAN_WIFI_STATUS_FAILURE;
	}

	tlvs = oem_buf;
	tlvs = init_oem_data_cmd_hdr(tlvs, OEM_CMD_SET_XPAN_EB_PAIR_AUDIO_LOCATION);
	tlvs = parse_oem_data_audio_pair_cmd_tlv(tlvs, params);

	if (!tlvs)
		goto cleanup;

	status = xpan_wifi_send_oem_data_cmd(data, iface->ifname, oem_buf,
					     oem_buf_len, NULL);
	if (status == XPAN_WIFI_STATUS_SUCCESS)
		ALOGI("Successfully sent audio pair params to FW");

cleanup:
	free(oem_buf);
	return status;
}

xpan_wifi_status register_iface(void *ctx, u8 *mac, iface_type type)
{
	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	xpan_wifi_data *data = (xpan_wifi_data*)ctx;
	iface_info *ifinfo = NULL;
	FILE *fp;
	char path[PATH_SIZE], buf[50];
	struct ether_addr *addr;
	u8 sta_mac[ETH_ALEN];
	int rem = 0;

	if (!data) {
		ALOGE("%s: Error, xpan_wifi_data NULL", __func__);
		return status;
	}

	/* For now allow registration of IFACE_TYPE_STA */
	if (type != IFACE_TYPE_STA) {
		ALOGE("%s: Error, Not STA iface type", __func__);
		return status;
	}

	if (!mac || is_zero_ether_addr(mac)) {
		ALOGE("%s: mac is invalid", __func__);
		return status;
	}

	//update interface info in xpan_data
	wifi_reload_ifaces(getWifiXpanHandle(data));

	for (int i = 0; i < data->num_interfaces; i++) {
		memset(path, 0, PATH_SIZE);
		rem = strlcat(path, "/sys/class/net/", PATH_SIZE);
		if (rem < PATH_SIZE)
			rem = strlcat(path, data->interfaces[i]->name, PATH_SIZE - rem);
		if (rem < PATH_SIZE)
			rem = strlcat(path, "/address", PATH_SIZE - rem);
		fp = fopen(path, "r");
		if (fp == NULL) {
			ALOGE("Unable to read mac address");
			return status;
		}
		fscanf(fp, "%s", buf);
		addr = ether_aton(buf);
		fclose(fp);
		memcpy(sta_mac, addr, sizeof(sta_mac));

		if (is_zero_ether_addr(sta_mac) ||
			memcmp(mac, sta_mac, sizeof(mac_addr)))
			continue;
		ifinfo = (iface_info *)malloc(sizeof(iface_info));
		if (!ifinfo) {
			ALOGE("%s: Error, ifinfo NULL", __func__);
			return status;
		}
		ifinfo->if_index = data->interfaces[i]->id;
		strlcpy(ifinfo->ifname, data->interfaces[i]->name, (IFNAMSIZ + 1));
		ifinfo->type = type;
		ALOGI("MAC of interface:%s is " MACSTR,
		      data->interfaces[i]->name, MAC2STR(sta_mac));
		add_iface_node(data, ifinfo);
		return XPAN_WIFI_STATUS_SUCCESS;
	}

	ALOGE("%s: MAC not found", __func__);
	return status;
}

xpan_wifi_status unregister_iface(void *ctx, iface_type type)
{
	xpan_wifi_status status = XPAN_WIFI_STATUS_FAILURE;
	xpan_wifi_data *data = (xpan_wifi_data*)ctx;
	iface_info *ifinfo = NULL;

	if (!data) {
		ALOGE("%s: Error, xpan_wifi_data NULL", __func__);
		return status;
	}

	if (type != IFACE_TYPE_STA) {
		ALOGE("%s: Error, Not STA iface type", __func__);
		return status;
	}

	ifinfo = get_ifaceinfo_from_list(data->ifinfo, type);
	if (!ifinfo) {
		ALOGE("%s iface is not present in the list", __func__);
		return status;
	}
	remove_iface_node(data, ifinfo);

	return XPAN_WIFI_STATUS_SUCCESS;
}

static void internal_cleaned_up_handler(wifi_handle handle)
{
	xpan_wifi_data *data = getWifiXpanHalInfo(handle);
	wifi_cleaned_up_handler cleaned_up_handler;

	if (!data)
		return;

	cleaned_up_handler = data->cleaned_up_handler;

	if (data->nl_cmd != 0) {
		nl_socket_free(data->nl_cmd);
		data->nl_cmd = NULL;
	}

	if (data->nl_event != 0) {
		nl_socket_free(data->nl_event);
		data->nl_event = NULL;
	}

	if (data->interfaces) {
		for (int i = 0; i < data->num_interfaces; i++)
			free(data->interfaces[i]);
		free(data->interfaces);
	}

	if (data->exit_sockets[0] >= 0) {
		close(data->exit_sockets[0]);
		data->exit_sockets[0] = -1;
	}

	if (data->exit_sockets[1] >= 0) {
		close(data->exit_sockets[1]);
		data->exit_sockets[1] = -1;
	}

	(*cleaned_up_handler)(handle);
	free(data);
}

static int internal_pollin_handler(wifi_handle handle, struct nl_sock *sock)
{
	struct nl_cb *cb = nl_socket_get_cb(sock);
	int res = nl_recvmsgs(sock, cb);
	if(res)
		ALOGE("Error :%d while reading nl msg", res);
	nl_cb_put(cb);

	return res;
}

static void internal_event_handler(wifi_handle handle, int events,
				   struct nl_sock *sock)
{
	if (events & POLLERR) {
		ALOGE("Error reading from socket");
		internal_pollin_handler(handle, sock);
	} else if (events & POLLHUP) {
		ALOGE("Remote side hung up");
	} else if (events & POLLIN) {
		internal_pollin_handler(handle, sock);
	} else {
		ALOGE("Unknown event - %0x", events);
	}
}

static bool exit_event_handler(int fd)
{
	char buf[4];
	memset(buf, 0, sizeof(buf));

	TEMP_FAILURE_RETRY(read(fd, buf, sizeof(buf)));
	ALOGI("exit_event_handler, buf=%s", buf);
	if (strncmp(buf, "E", 1) == 0) {
		return true;
	}

	return false;
}

void xpan_wifi_event_loop(void *ctx)
{
	xpan_wifi_data *data = (xpan_wifi_data *)ctx;
	if(!data) {
		ALOGE("%s: Error, xpan_wifi_data NULL", __func__);
		return;
	}

	if (data->in_event_loop) {
		return;
	} else {
		data->in_event_loop = true;
	}

	wifi_handle handle = getWifiXpanHandle(data);

	pollfd pfd[2];
	memset(&pfd, 0, 2*sizeof(pfd[0]));

	pfd[0].fd = nl_socket_get_fd(data->nl_event);
	pfd[0].events = POLLIN;

	pfd[1].fd = data->exit_sockets[1];
	pfd[1].events = POLLIN;

	do {
		pfd[0].revents = 0;
		pfd[1].revents = 0;
		int result = poll(pfd, 2, -1);
		if (result < 0) {
			ALOGE("Error polling socket");
		} else {
			if (pfd[0].revents & (POLLIN | POLLHUP | POLLERR)) {
				internal_event_handler(handle, pfd[0].revents,
						       data->nl_event);
			}
			if (pfd[1].revents & POLLIN) {
				if (exit_event_handler(pfd[1].fd)) {
					break;
				}
			}
		}
	} while (!data->clean_up);
	internal_cleaned_up_handler(handle);
	ALOGI("xpan_wifi_event_loop() exits success");
}

xpan_wifi_status set_softap_state(enum xpan_sap_state state)
{
	xpan_wifi_status ret = XPAN_WIFI_STATUS_FAILURE;

	switch (state) {
	case XPAN_SAP_STATE_ENABLED:
		if (property_set("vendor.wifi.lohs.sap.iface.inuse", "false") == 0)
			ret = XPAN_WIFI_STATUS_SUCCESS;
		break;
	case XPAN_SAP_STATE_ENABLING:
	case XPAN_SAP_STATE_DISABLED:
	case XPAN_SAP_STATE_DISABLING:
	case XPAN_SAP_STATE_FAILED:
		ret = XPAN_WIFI_STATUS_SUCCESS;
		break;
	default:
		break;
	}

	return ret;
}

xpan_wifi_status init_xpan_wifi_lib_function_table(struct wifi_drv_ops *fn)
{
	if (fn == NULL)
		return XPAN_WIFI_STATUS_FAILURE;

	fn->init_xpan_wifi_lib = init_xpan_wifi_lib;
	fn->deinit_xpan_wifi_lib = deinit_xpan_wifi_lib;
	fn->register_callbacks = register_callbacks;
	fn->deregister_callbacks = deregister_callbacks;
	fn->xpan_wifi_event_loop = xpan_wifi_event_loop;
	fn->set_xpan_wifi_stats_enabled = set_xpan_wifi_stats_enabled;
	fn->set_xpan_usecase_params = set_xpan_usecase_params;
	fn->set_xpan_ap_power_save = set_xpan_ap_power_save;
	fn->wifi_create_ap_iface = wifi_create_ap_iface;
	fn->wifi_delete_ap_iface = wifi_delete_ap_iface;
	fn->wifi_do_acs = wifi_do_acs;
	fn->set_softap_state = set_softap_state;
	fn->xpan_audio_transport_switch = xpan_audio_transport_switch;
	fn->set_xpan_ap_available = set_xpan_ap_available;
	fn->cancel_xpan_ap_available = cancel_xpan_ap_available;
	fn->set_xpan_peer_audio_pair_location = set_xpan_peer_audio_pair_location;
	fn->register_iface = register_iface;
	fn->unregister_iface = unregister_iface;

	return XPAN_WIFI_STATUS_SUCCESS;
}
