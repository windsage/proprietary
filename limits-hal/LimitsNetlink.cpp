/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <unistd.h>
#include <linux/thermal.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>

#include "LimitsNetlink.h"

namespace vendor::qti::hardware::limits::implementation {

using pollCB = std::function<bool()>;

void limits_listen(struct nl_sock *soc, const pollCB &stopCB)
{
	while (!stopCB())
		nl_recvmsgs_default(soc);

	LOG(INFO) << "limits_listen_event Exit" << std::endl;
	return;
}

int limits_event_cb(struct nl_msg *n, void *data)
{
	LimitsNetlink *t = (LimitsNetlink *)data;

	return t->event_parse(n, NULL);
}

int limits_family_cb(struct nl_msg *n, void *data)
{
	LimitsNetlink *t = (LimitsNetlink *)data;
	return t->family_msg_cb(n, NULL);
}

LimitsNetlink::LimitsNetlink(const eventMonitorCB &inp_event_cb):
	event_group(-1),
	event_cb(inp_event_cb)
{
	monitor_shutdown = false;
}

LimitsNetlink::~LimitsNetlink()
{
	monitor_shutdown = true;
	event_th.join();
	if (event_soc)
		nl_socket_free(event_soc);
}

int LimitsNetlink::event_parse(struct nl_msg *n, void *data)
{
	struct nlmsghdr *nl_hdr = nlmsg_hdr(n);
	struct genlmsghdr *hdr = genlmsg_hdr(nl_hdr);
	struct nlattr *attrs[THERMAL_GENL_ATTR_MAX + 1];
	int cdev_id = -1, cur_state = -1;

	genlmsg_parse(nl_hdr, 0, attrs, THERMAL_GENL_ATTR_MAX, NULL);

	switch (hdr->cmd) {
	case THERMAL_GENL_EVENT_CDEV_STATE_UPDATE:
		if (attrs[THERMAL_GENL_ATTR_CDEV_ID])
			cdev_id = nla_get_u32(
					attrs[THERMAL_GENL_ATTR_CDEV_ID]);

		if (attrs[THERMAL_GENL_ATTR_CDEV_CUR_STATE])
			cur_state = nla_get_u32(
				attrs[THERMAL_GENL_ATTR_CDEV_CUR_STATE]);
		LOG(DEBUG) << "thermal_nl_event: CDEV:" << cdev_id <<
			" cur_state:" << cur_state << " event:" << hdr->cmd
			<< std::endl;
		event_cb(cdev_id, cur_state);
		break;
	}

	return 0;

}

int LimitsNetlink::family_msg_cb(struct nl_msg *msg, void *data)
{
	struct nlattr *tb[CTRL_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = genlmsg_hdr(nlmsg_hdr(msg));
	struct nlattr *mc_group;
	int rem_mcgrp;

	nla_parse(tb, CTRL_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[CTRL_ATTR_MCAST_GROUPS]) {
		LOG(ERROR) << "Multicast group not available\n";
		return -1;
	}

	nla_for_each_nested(mc_group, tb[CTRL_ATTR_MCAST_GROUPS], rem_mcgrp) {

		struct nlattr *nl_group[CTRL_ATTR_MCAST_GRP_MAX + 1];

		nla_parse(nl_group, CTRL_ATTR_MCAST_GRP_MAX,
			  (struct nlattr *)nla_data(mc_group),
			  nla_len(mc_group), NULL);

		if (!nl_group[CTRL_ATTR_MCAST_GRP_NAME] ||
		    !nl_group[CTRL_ATTR_MCAST_GRP_ID])
			continue;
		std::string family((char *)nla_data(
					nl_group[CTRL_ATTR_MCAST_GRP_NAME]));
		if (family == THERMAL_GENL_EVENT_GROUP_NAME)
			event_group = nla_get_u32(
					nl_group[CTRL_ATTR_MCAST_GRP_ID]);
	}

	return 0;
}

int LimitsNetlink::send_nl_msg(struct nl_msg *msg)
{
	int ret = 0;

	ret = nl_send_auto(event_soc, msg);
	if (ret < 0) {
		LOG(ERROR) << "Error sending NL message\n";
		return ret;
	}
	nl_socket_disable_seq_check(event_soc);
	nl_socket_modify_cb(event_soc, NL_CB_VALID, NL_CB_CUSTOM,
			limits_family_cb, this);
	ret = nl_recvmsgs_default(event_soc);

	return ret;
}

int LimitsNetlink::fetch_group_id(void)
{
	struct nl_msg *msg;
	int ctrlid, ret = 0;

	msg = nlmsg_alloc();
	if (!msg)
		return -1;

	ctrlid = genl_ctrl_resolve(event_soc, "nlctrl");
	genlmsg_put(msg, 0, 0, ctrlid, 0, 0, CTRL_CMD_GETFAMILY, 0);
	nla_put_string(msg, CTRL_ATTR_FAMILY_NAME, THERMAL_GENL_FAMILY_NAME);
	send_nl_msg(msg);
	nlmsg_free(msg);

	if (event_group != -1) {
		LOG(DEBUG) << "Event: " << event_group << std::endl;
		ret = nl_socket_add_membership(event_soc, event_group);
		if (ret) {
			LOG(ERROR) << "Event Socket membership add error\n";
			return ret;
		}
	}
	return 0;
}

void LimitsNetlink::start()
{
	struct nl_msg *msg;

	event_soc = nl_socket_alloc();
	if (!event_soc) {
		LOG(ERROR) << "Event socket alloc failed\n";
		return;
	}

	if (genl_connect(event_soc)) {
		LOG(ERROR) << "Event socket connect failed\n";
		nl_socket_free(event_soc);
		event_soc = nullptr;
		return;
	}
	if (fetch_group_id())
		return;
	nl_socket_disable_seq_check(event_soc);
	nl_socket_modify_cb(event_soc, NL_CB_VALID, NL_CB_CUSTOM,
			limits_event_cb, this);
	event_th = std::thread(limits_listen, event_soc,
		std::bind(&LimitsNetlink::stopPolling, this));
}

}  // namespace vendor::qti::hardware::limits::implementation
