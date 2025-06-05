/*===========================================================================

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/thermal.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/mngt.h>
#include <netlink/genl/ctrl.h>
#include <netlink/netlink.h>

#include "thermal.h"

enum thermal_nl_cb_type {
	THERMAL_NL_TRIP,
	THERMAL_NL_TEMP_SAMPLE,
};

union thermal_nl_cb {
	nl_trip_cb trip_cb;
	nl_temp_sample_cb temp_cb;
};

struct thermal_nl_cb_data {
	union thermal_nl_cb cb;
	enum thermal_nl_cb_type type;
	void *data;
	struct thermal_nl_cb_data *next;
};

struct thermal_nl_socket {
	struct nl_sock *soc;
	int grp_id;
	struct thermal_nl_cb_data *head_ptr;
	pthread_mutex_t cb_mutex;
};

static struct thermal_nl_socket nl_socket = {
	.soc = NULL,
	.grp_id = -1,
	.head_ptr = NULL,
};

static struct thermal_nl_socket nl_sample_socket = {
	.soc = NULL,
	.grp_id = -1,
	.head_ptr = NULL,
};

static bool stop;
static pthread_t thermal_sensor_event_thread;
static pthread_t thermal_sensor_sample_thread;

static int thermal_nl_parse_family(struct nl_msg *msg, void *data)
{
	struct nlattr *tb[CTRL_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *mc_group;
	int rem_mcgrp;

	nla_parse(tb, CTRL_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[CTRL_ATTR_MCAST_GROUPS]) {
		msg("Multicast group not available\n");
		return -1;
	}

	nla_for_each_nested(mc_group, tb[CTRL_ATTR_MCAST_GROUPS], rem_mcgrp) {

		struct nlattr *nl_group[CTRL_ATTR_MCAST_GRP_MAX + 1];

		nla_parse(nl_group, CTRL_ATTR_MCAST_GRP_MAX,
			  nla_data(mc_group), nla_len(mc_group), NULL);

		if (!nl_group[CTRL_ATTR_MCAST_GRP_NAME] ||
		    !nl_group[CTRL_ATTR_MCAST_GRP_ID])
			continue;

		dbgmsg("Family:%s\n", (char *)nla_data(nl_group[CTRL_ATTR_MCAST_GRP_NAME]));
		if (!strncmp(nla_data(nl_group[CTRL_ATTR_MCAST_GRP_NAME]),
			    THERMAL_GENL_EVENT_GROUP_NAME,
			    nla_len(nl_group[CTRL_ATTR_MCAST_GRP_NAME])))
			nl_socket.grp_id = nla_get_u32(
					nl_group[CTRL_ATTR_MCAST_GRP_ID]);

		if (!strncmp(nla_data(nl_group[CTRL_ATTR_MCAST_GRP_NAME]),
			    THERMAL_GENL_SAMPLING_GROUP_NAME,
			    nla_len(nl_group[CTRL_ATTR_MCAST_GRP_NAME])))
			nl_sample_socket.grp_id = nla_get_u32(
					nl_group[CTRL_ATTR_MCAST_GRP_ID]);
	}

	return 0;
}

static int thermal_nl_send_msg(struct nl_msg *msg)
{
	int ret = 0;

	ret = nl_send_auto(nl_socket.soc, msg);
	if (ret < 0) {
		msg("Message send error. :%d\n", ret);
		return ret;
	}
	nl_socket_disable_seq_check(nl_socket.soc);
	nl_socket_modify_cb(nl_socket.soc, NL_CB_VALID, NL_CB_CUSTOM,
			thermal_nl_parse_family, NULL);
	ret = nl_recvmsgs_default(nl_socket.soc);

	return ret;
}

static int thermal_nl_fetch_id(void)
{
	struct nl_msg *msg;
	int ctrlid, ret = 0;

	msg = nlmsg_alloc();
	if (!msg)
		return -ENOMEM;

	ctrlid = genl_ctrl_resolve(nl_socket.soc, "nlctrl");
	genlmsg_put(msg, 0, 0, ctrlid, 0, 0, CTRL_CMD_GETFAMILY, 0);

	nla_put_string(msg, CTRL_ATTR_FAMILY_NAME,
			THERMAL_GENL_FAMILY_NAME);
	thermal_nl_send_msg(msg);

	nlmsg_free(msg);

	if (nl_socket.grp_id != -1 &&
			nl_sample_socket.grp_id != -1) {
		ret = nl_socket_add_membership(nl_socket.soc,
				nl_socket.grp_id);
		if (ret) {
			msg("Event Socket membership add err:%d\n", ret);
			return ret;
		}
		ret = nl_socket_add_membership(nl_sample_socket.soc,
				nl_sample_socket.grp_id);
		if (ret) {
			msg("Sample Socket membership add err:%d\n", ret);
			return ret;
		}
	}
	return 0;
}

static void notify_event_cb(struct thermal_nl_socket *soc_data,
		int tz_id, int trip_id, enum thermal_nl_cb_type type,
		bool is_up)
{
	struct thermal_nl_cb_data *ptr = soc_data->head_ptr;

	if (tz_id == -1 || trip_id == -1)
		return;
	pthread_mutex_lock(&soc_data->cb_mutex);
	ptr = soc_data->head_ptr;
	while (ptr) {
		if (ptr->type != type) {
			ptr = ptr->next;
			continue;
		}
		(ptr->cb.trip_cb)(tz_id, trip_id, is_up, ptr->data);
		ptr = ptr->next;
	}
	pthread_mutex_unlock(&soc_data->cb_mutex);
}

static int thermal_nl_event_cb(struct nl_msg *n, void *data)
{
	struct thermal_nl_socket *soc_data =
		(struct thermal_nl_socket*)data;
	struct nlmsghdr *nl_hdr = nlmsg_hdr(n);
	struct genlmsghdr *genlhdr = genlmsg_hdr(nl_hdr);
	struct nlattr *attrs[THERMAL_GENL_ATTR_MAX + 1];
	int tz_id = -1, trip_id = -1;

	genlmsg_parse(nl_hdr, 0, attrs, THERMAL_GENL_ATTR_MAX, NULL);

	switch (genlhdr->cmd) {
	case THERMAL_GENL_EVENT_TZ_TRIP_UP:
	case THERMAL_GENL_EVENT_TZ_TRIP_DOWN:
		if (attrs[THERMAL_GENL_ATTR_TZ_ID])
			tz_id = nla_get_u32(attrs[THERMAL_GENL_ATTR_TZ_ID]);

		if (attrs[THERMAL_GENL_ATTR_TZ_TRIP_ID])
			trip_id = nla_get_u32(
					attrs[THERMAL_GENL_ATTR_TZ_TRIP_ID]);
		dbgmsg("thermal_nl_event: TZ:%d Trip:%d event:%d\n",
				tz_id, trip_id,	genlhdr->cmd);
		notify_event_cb(soc_data, tz_id, trip_id, THERMAL_NL_TRIP,
				(genlhdr->cmd == THERMAL_GENL_EVENT_TZ_TRIP_UP) ?
				true : false);
		break;
	}

	return 0;
}

static void *thermal_sensor_netlink(void *data)
{
	if (nl_socket.grp_id == -1)
		return NULL;

	nl_socket_disable_seq_check(nl_socket.soc);
	nl_socket_modify_cb(nl_socket.soc, NL_CB_VALID, NL_CB_CUSTOM,
				thermal_nl_event_cb, &nl_socket);

	while (!stop)
		nl_recvmsgs_default(nl_socket.soc);

	return NULL;
}

static void notify_sample_cb(struct thermal_nl_socket *soc_data,
		int tz_id, int temp, enum thermal_nl_cb_type type)
{
	struct thermal_nl_cb_data *ptr = soc_data->head_ptr;

	if (tz_id == -1 || temp == -1)
		return;
	pthread_mutex_lock(&soc_data->cb_mutex);
	ptr = soc_data->head_ptr;
	while (ptr) {
		if (ptr->type != type) {
			ptr = ptr->next;
			continue;
		}
		(ptr->cb.temp_cb)(tz_id, temp, ptr->data);
		ptr = ptr->next;
	}
	pthread_mutex_unlock(&soc_data->cb_mutex);
}

static int thermal_nl_sample_cb(struct nl_msg *n, void *data)
{
	struct thermal_nl_socket *soc_data =
		(struct thermal_nl_socket*)data;
	struct nlmsghdr *nl_hdr = nlmsg_hdr(n);
	struct genlmsghdr *genlhdr = genlmsg_hdr(nl_hdr);
	struct nlattr *attrs[THERMAL_GENL_ATTR_MAX + 1];
	int tz_id = -1, temp = -1;

	genlmsg_parse(nl_hdr, 0, attrs, THERMAL_GENL_ATTR_MAX, NULL);

	switch (genlhdr->cmd) {
	case THERMAL_GENL_SAMPLING_TEMP:
		if (attrs[THERMAL_GENL_ATTR_TZ_ID])
			tz_id = nla_get_u32(attrs[THERMAL_GENL_ATTR_TZ_ID]);

		if (attrs[THERMAL_GENL_ATTR_TZ_TEMP])
			temp = nla_get_u32(attrs[THERMAL_GENL_ATTR_TZ_TEMP]);

		dbgmsg("thermal_sample_event: TZ:%d temp:%d\n",
				tz_id, temp);
		notify_sample_cb(soc_data, tz_id, temp, THERMAL_NL_TEMP_SAMPLE);
		break;
	}

	return 0;
}

static void *thermal_sensor_netlink_sample(void *data)
{
	if (nl_sample_socket.grp_id == -1)
		return NULL;

	nl_socket_disable_seq_check(nl_sample_socket.soc);
	nl_socket_modify_cb(nl_sample_socket.soc, NL_CB_VALID, NL_CB_CUSTOM,
				thermal_nl_sample_cb, &nl_sample_socket);

	while (!stop)
		nl_recvmsgs_default(nl_sample_socket.soc);

	return NULL;
}

void thermal_nl_stop(void)
{
	struct thermal_nl_cb_data *ptr = NULL, *n = NULL;
	stop = true;
	pthread_join(thermal_sensor_event_thread, NULL);
	pthread_join(thermal_sensor_sample_thread, NULL);
	nl_socket_free(nl_socket.soc);
	nl_socket_free(nl_sample_socket.soc);
	nl_sample_socket.soc = NULL;
	nl_socket.soc = NULL;
	pthread_mutex_lock(&nl_socket.cb_mutex);
	ptr = nl_socket.head_ptr;
	while (ptr) {
		n = ptr->next;
		free(ptr);
		ptr = n;
	}
	nl_socket.head_ptr = NULL;
	pthread_mutex_unlock(&nl_socket.cb_mutex);

	pthread_mutex_lock(&nl_sample_socket.cb_mutex);
	ptr = nl_sample_socket.head_ptr;
	while (ptr) {
		n = ptr->next;
		free(ptr);
		ptr = n;
	}
	nl_sample_socket.head_ptr = NULL;
	pthread_mutex_unlock(&nl_sample_socket.cb_mutex);
}

int thermal_nl_init(void)
{
	int ret = -1;

	if (nl_socket.soc != NULL)
		return 0;

	if (!nl_socket.soc) {
		nl_socket.soc = nl_socket_alloc();
		if (!nl_socket.soc) {
			msg("Event socket alloc failed:%d\n", errno);
			return -1;
		}

		if (genl_connect(nl_socket.soc)) {
			msg("Event socket connect failed:%d\n", errno);
			nl_socket_free(nl_socket.soc);
			nl_socket.soc = NULL;
			return -1;
		}

		nl_sample_socket.soc = nl_socket_alloc();
		if (!nl_sample_socket.soc) {
			msg("Sample socket alloc failed:%d\n", errno);
			nl_socket_free(nl_socket.soc);
			nl_socket.soc = NULL;
			return -1;
		}

		if (genl_connect(nl_sample_socket.soc)) {
			msg("Sample socket connect failed:%d\n", errno);
			nl_socket_free(nl_socket.soc);
			nl_socket_free(nl_sample_socket.soc);
			nl_sample_socket.soc = NULL;
			nl_socket.soc = NULL;
			return -1;
		}
		stop = false;
	}

	ret = thermal_nl_fetch_id();
	if (ret) {
		nl_socket_free(nl_socket.soc);
		nl_socket_free(nl_sample_socket.soc);
		nl_sample_socket.soc = NULL;
		nl_socket.soc = NULL;
		return ret;
	}

	dbgmsg("%s: Netlink Event ID:%d Sampling ID:%d\n", __func__,
					nl_socket.grp_id,
					nl_sample_socket.grp_id);
	pthread_mutex_init(&nl_socket.cb_mutex, NULL);
	pthread_mutex_init(&nl_sample_socket.cb_mutex, NULL);
	pthread_create(&thermal_sensor_event_thread, NULL,
				thermal_sensor_netlink, NULL);
	pthread_create(&thermal_sensor_sample_thread, NULL,
				thermal_sensor_netlink_sample, NULL);
	return 0;
}

static int thermal_nl_add_cb(struct thermal_nl_socket *soc,
		union thermal_nl_cb cb, void *data,
		enum thermal_nl_cb_type type)
{
	struct thermal_nl_cb_data *ptr = NULL;

	ptr = malloc(sizeof(*ptr));
	if (!ptr)
		return -1;
	ptr->cb = cb;
	ptr->data = data;
	ptr->type = type;
	pthread_mutex_lock(&soc->cb_mutex);
	ptr->next = soc->head_ptr;
	soc->head_ptr = ptr;
	pthread_mutex_unlock(&soc->cb_mutex);

	return 0;
}

int thermal_nl_register_trip(nl_trip_cb cb, void *data)
{
	union thermal_nl_cb local_cb;

	if (!cb)
		return -1;
	local_cb.trip_cb = cb;

	return thermal_nl_add_cb(&nl_socket, local_cb, data, THERMAL_NL_TRIP);
}

int thermal_nl_register_temp_sample(nl_temp_sample_cb cb, void *data)
{
	union thermal_nl_cb local_cb;

	if (!cb)
		return -1;
	local_cb.temp_cb = cb;

	return thermal_nl_add_cb(&nl_sample_socket, local_cb, data,
				THERMAL_NL_TEMP_SAMPLE);
}
