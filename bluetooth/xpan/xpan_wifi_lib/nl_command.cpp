/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
 *
 * Not a Contribution
 */

/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <linux/rtnetlink.h>
#include <netpacket/packet.h>
#include <linux/filter.h>
#include <linux/errqueue.h>

#include <linux/pkt_sched.h>
#include <netlink/object-api.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <net/if.h>

#include "nl80211_copy.h"
#include <ctype.h>

#include "nl_command.h"

void appendFmt(char *buf, size_t buf_len, int &offset, const char *fmt, ...)
{
	va_list params;
	va_start(params, fmt);
	offset += vsnprintf(buf + offset, buf_len - offset, fmt, params);
	va_end(params);
}

#define C2S(x)  case x: return #x;

static const char *cmdToString(int cmd)
{
	switch (cmd) {
	C2S(NL80211_CMD_GET_INTERFACE)
	C2S(NL80211_CMD_SET_INTERFACE)
	C2S(NL80211_CMD_NEW_INTERFACE)
	C2S(NL80211_CMD_DEL_INTERFACE)
	C2S(NL80211_CMD_VENDOR)
	default:
	    return "NL80211_CMD_UNKNOWN";
	}
}

const char *WifiEvent::get_cmdString()
{
	return cmdToString(get_cmd());
}


int WifiEvent::parse()
{
	if (mHeader != NULL) {
		return WIFI_SUCCESS;
	}
	mHeader = (genlmsghdr *)nlmsg_data(nlmsg_hdr(mMsg));
	int result = nla_parse(mAttributes, NL80211_ATTR_MAX_INTERNAL,
			       genlmsg_attrdata(mHeader, 0),
			       genlmsg_attrlen(mHeader, 0), NULL);

	return result;
}

wifi_error WifiRequest::create(int family, uint8_t cmd, int flags, int hdrlen)
{
	destroy();

	mMsg = nlmsg_alloc();
	if (mMsg != NULL) {
		genlmsg_put(mMsg, /* pid = */ 0, /* seq = */ 0, family,
			    hdrlen, flags, cmd, /* version = */ 0);
		return WIFI_SUCCESS;
	} else {
		return WIFI_ERROR_OUT_OF_MEMORY;
	}
}

wifi_error WifiRequest::create_vendor_command(uint32_t id, int subcmd)
{
	wifi_error res = create(NL80211_CMD_VENDOR);
	if (res != WIFI_SUCCESS)
		return res;

	res = put_u32(NL80211_ATTR_VENDOR_ID, id);
	if (res != WIFI_SUCCESS)
		return res;

	res = put_u32(NL80211_ATTR_VENDOR_SUBCMD, subcmd);
	if (res != WIFI_SUCCESS)
		return res;

	if (mIface != -1)
		res = set_iface_id(mIface);

	return res;
}


static int no_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

wifi_error WifiCommand::requestResponse()
{
	wifi_error err = create();                 /* create the message */
	if (err != WIFI_SUCCESS)
		return err;

	return requestResponse(mMsg);
}

wifi_error WifiCommand::requestResponse(WifiRequest& request)
{
	int err = 0;

	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb)
		goto out;

	err = nl_send_auto_complete(mInfo->nl_cmd, request.getMessage());    /* send message */
	if (err < 0)
		goto out;

	err = 1;

	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, response_handler, this);

	while (err > 0) {                   /* wait for reply */
		int res = nl_recvmsgs(mInfo->nl_cmd, cb);
		if (res) {
			ALOGE("nl80211: %s->nl_recvmsgs failed: %d", __FUNCTION__, res);
		}
	}
out:
	nl_cb_put(cb);
	mMsg.destroy();
	return mapKernelErrortoWifiHalError(err);
}

/* Event handlers */
int WifiCommand::response_handler(struct nl_msg *msg, void *arg)
{
	WifiCommand *cmd = (WifiCommand *)arg;
	WifiEvent reply(msg);
	int res = reply.parse();
	if (res < 0) {
		ALOGE("Failed to parse reply message = %d", res);
		return NL_SKIP;
	} else {
		return cmd->handleResponse(reply);
	}
}

/* Other event handlers */
int WifiCommand::valid_handler(struct nl_msg *msg, void *arg)
{
	int *err = (int *)arg;
	*err = 0;
	return NL_SKIP;
}

int WifiCommand::ack_handler(struct nl_msg *msg, void *arg)
{
	int *err = (int *)arg;
	*err = 0;
	return NL_STOP;
}

int WifiCommand::finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = (int *)arg;
	*ret = 0;
	return NL_SKIP;
}

int WifiCommand::error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg) {
	int *ret = (int *)arg;
	*ret = err->error;
	return NL_SKIP;
}

WifiVendorCommand::WifiVendorCommand(wifi_handle handle,
				     wifi_request_id id,
				     u32 vendor_id,
				     u32 subcmd)
				     : WifiCommand(handle, id), mVendor_id(vendor_id), mSubcmd(subcmd),
				     mVendorData(NULL), mDataLen(0)
{
	ALOGV("WifiVendorCommand %p created vendor_id:0x%x subcmd:%u",
	       this, mVendor_id, mSubcmd);
}

WifiVendorCommand::~WifiVendorCommand()
{
}

// Override this method to parse reply and dig out data; save it
// in the corresponding object
int WifiVendorCommand::handleResponse(WifiEvent &reply)
{
	struct nlattr **tb = reply.attributes();
	struct genlmsghdr *gnlh = reply.header();

	if (gnlh->cmd == NL80211_CMD_VENDOR) {
		if (tb[NL80211_ATTR_VENDOR_DATA]) {
			mVendorData = (char *)nla_data(tb[NL80211_ATTR_VENDOR_DATA]);
			mDataLen = nla_len(tb[NL80211_ATTR_VENDOR_DATA]);
		}
	}
	return NL_SKIP;
}

void hexdump(void *buf, u16 len)
{
	int i=0;
	char *bytes = (char *)buf;

	if (len) {
		ALOGV("******HexDump len:%d*********", len);
		for (i = 0; ((i + 7) < len); i+=8) {
			ALOGV("%02x %02x %02x %02x   %02x %02x %02x %02x",
			      bytes[i], bytes[i+1],
			      bytes[i+2], bytes[i+3],
			      bytes[i+4], bytes[i+5],
			      bytes[i+6], bytes[i+7]);
		}
		if ((len - i) >= 4) {
			ALOGV("%02x %02x %02x %02x",
			      bytes[i], bytes[i+1],
			      bytes[i+2], bytes[i+3]);
			i+=4;
		}
		for (;i < len;i++) {
			ALOGV("%02x", bytes[i]);
		}
		ALOGV("******HexDump End***********");
	} else {
		return;
	}
}

wifi_error WifiVendorCommand::create()
{
	int ifindex;
	wifi_error ret = mMsg.create(NL80211_CMD_VENDOR, 0, 0);
	if (ret != WIFI_SUCCESS)
		return ret;

	// insert the oui in the msg
	ret = mMsg.put_u32(NL80211_ATTR_VENDOR_ID, mVendor_id);
	if (ret != WIFI_SUCCESS)
		goto out;

	// insert the subcmd in the msg
	ret = mMsg.put_u32(NL80211_ATTR_VENDOR_SUBCMD, mSubcmd);
	if (ret != WIFI_SUCCESS)
		goto out;

	//Insert the vendor specific data
	ret = mMsg.put_bytes(NL80211_ATTR_VENDOR_DATA, mVendorData, mDataLen);
	if (ret != WIFI_SUCCESS)
		goto out;
	hexdump(mVendorData, mDataLen);

	//insert the iface id to be XPAN_DEFAULT_SAP_IFACE
	ifindex = if_nametoindex(XPAN_DEFAULT_SAP_IFACE);
	ret = mMsg.set_iface_id(ifindex);
out:
	return ret;
}

wifi_error WifiVendorCommand::requestResponse()
{
	return WifiCommand::requestResponse(mMsg);
}

wifi_error WifiVendorCommand::put_u8(int attribute, uint8_t value)
{
	return mMsg.put_u8(attribute, value);
}

wifi_error WifiVendorCommand::put_list(int attribute, int len, void* value)
{
	return mMsg.put_list(attribute, len, value);
}

wifi_error WifiVendorCommand::put_u16(int attribute, uint16_t value)
{
	return mMsg.put_u16(attribute, value);
}

wifi_error WifiVendorCommand::put_u32(int attribute, uint32_t value)
{
	return mMsg.put_u32(attribute, value);
}

wifi_error WifiVendorCommand::put_u64(int attribute, uint64_t value)
{
	return mMsg.put_u64(attribute, value);
}

wifi_error WifiVendorCommand::put_s8(int attribute, s8 value)
{
	return mMsg.put_s8(attribute, value);
}

wifi_error WifiVendorCommand::put_s16(int attribute, s16 value)
{
	return mMsg.put_s16(attribute, value);
}

wifi_error WifiVendorCommand::put_s32(int attribute, s32 value)
{
	return mMsg.put_s32(attribute, value);
}

wifi_error WifiVendorCommand::put_s64(int attribute, s64 value)
{
	return mMsg.put_s64(attribute, value);
}

wifi_error WifiVendorCommand::put_flag(int attribute)
{
	return mMsg.put_flag(attribute);
}

u8 WifiVendorCommand::get_u8(const struct nlattr *nla)
{
	return mMsg.get_u8(nla);
}

u16 WifiVendorCommand::get_u16(const struct nlattr *nla)
{
	return mMsg.get_u16(nla);
}

u32 WifiVendorCommand::get_u32(const struct nlattr *nla)
{
	return mMsg.get_u32(nla);
}

u64 WifiVendorCommand::get_u64(const struct nlattr *nla)
{
	return mMsg.get_u64(nla);
}

s8 WifiVendorCommand::get_s8(const struct nlattr *nla)
{
	return mMsg.get_s8(nla);
}

s16 WifiVendorCommand::get_s16(const struct nlattr *nla)
{
	return mMsg.get_s16(nla);
}

s32 WifiVendorCommand::get_s32(const struct nlattr *nla)
{
	return mMsg.get_s32(nla);
}

s64 WifiVendorCommand::get_s64(const struct nlattr *nla)
{
	return mMsg.get_s64(nla);
}

wifi_error WifiVendorCommand::put_ipv6_addr(int attribute, uint8_t value[16])
{
	return mMsg.put_ipv6_addr(attribute, value);
}

wifi_error WifiVendorCommand::put_string(int attribute, const char *value)
{
	return mMsg.put_string(attribute, value);
}

wifi_error WifiVendorCommand::put_addr(int attribute, mac_addr value)
{
	return mMsg.put_addr(attribute, value);
}

struct nlattr * WifiVendorCommand::attr_start(int attribute)
{
	return mMsg.attr_start(attribute);
}

void WifiVendorCommand::attr_end(struct nlattr *attribute)
{
	return mMsg.attr_end(attribute);
}

wifi_error WifiVendorCommand::set_iface_id(const char* name)
{
	unsigned ifindex = if_nametoindex(name);
	return mMsg.set_iface_id(ifindex);
}

wifi_error WifiVendorCommand::put_bytes(int attribute,
					const char *data,
					int len)
{
	return mMsg.put_bytes(attribute, data, len);
}

wifi_error WifiVendorCommand::get_mac_addr(struct nlattr **tb_vendor,
					   int attribute,
					   mac_addr addr)
{
	if (!tb_vendor[attribute]) {
		ALOGE("Failed to get attribute : %d", attribute);
		return WIFI_ERROR_INVALID_ARGS;
	}
	if (!addr) {
		ALOGE("addr is NULL");
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

wifi_error initialize_vendor_cmd(wifi_interface_handle iface,
				 wifi_request_id id,
				 u32 subcmd,
				 WifiVendorCommand **vCommand)
{
	wifi_error ret;
	interface_info *ifaceInfo = getIfaceInfo(iface);
	wifi_handle wifiHandle = getWifiXpanHandle(iface);

	if (vCommand == NULL) {
		ALOGE("%s: Error vCommand NULL", __FUNCTION__);
		return WIFI_ERROR_INVALID_ARGS;
	}

	*vCommand = new WifiVendorCommand(wifiHandle, id,
					  OUI_QCA,
					  subcmd);
	if (*vCommand == NULL) {
		ALOGE("%s: Object creation failed", __FUNCTION__);
		return WIFI_ERROR_OUT_OF_MEMORY;
	}

	/* Create the message */
	ret = (*vCommand)->create();
	if (ret != WIFI_SUCCESS)
		goto cleanup;

	ret = (*vCommand)->set_iface_id(ifaceInfo->name);
	if (ret != WIFI_SUCCESS)
		goto cleanup;

	return WIFI_SUCCESS;

cleanup:
	delete *vCommand;
	return ret;
}

xpan_wifi_data *getWifiXpanHalInfo(wifi_handle handle)
{
	return (xpan_wifi_data *)handle;
}

wifi_handle getWifiXpanHandle(wifi_interface_handle handle)
{
	return getIfaceInfo(handle)->handle;
}

wifi_handle getWifiXpanHandle(xpan_wifi_data *info)
{
	return (wifi_handle)info;
}

interface_info *getIfaceInfo(wifi_interface_handle handle)
{
	return (interface_info *)handle;
}

wifi_error mapKernelErrortoWifiHalError(int kern_err)
{
	if (kern_err >= 0)
		return WIFI_SUCCESS;

	switch (kern_err) {
		case -EOPNOTSUPP:
			return WIFI_ERROR_NOT_SUPPORTED;
		case -EAGAIN:
			return WIFI_ERROR_NOT_AVAILABLE;
		case -EINVAL:
			return WIFI_ERROR_INVALID_ARGS;
		case -ETIMEDOUT:
			return WIFI_ERROR_TIMED_OUT;
		case -ENOMEM:
			return WIFI_ERROR_OUT_OF_MEMORY;
		case -EBUSY:
			return WIFI_ERROR_BUSY;
		case -ENOBUFS:
			return WIFI_ERROR_TOO_MANY_REQUESTS;
	}

	return WIFI_ERROR_UNKNOWN;
}

