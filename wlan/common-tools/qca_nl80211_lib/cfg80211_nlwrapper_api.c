/*
 * Copyright (c) Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifdef KERNEL_NL80211_HEADER
#include <linux/nl80211.h>
#else
#include "../vendorcmdtool/nl80211_copy.h"
#endif
#include <stdbool.h>
#include "cfg80211_nlwrapper_pvt.h"
#include "cfg80211_nlwrapper_api.h"
#include "qca-vendor.h"
#include <stdio.h>

/* Convert an interface name to an index, and vice versa.  */
extern unsigned int if_nametoindex (const char *__ifname);
extern char *if_indextoname (unsigned int __ifindex, char *__ifname);

static const unsigned NL80211_ATTR_MAX_INTERNAL = 256;

/* Other event handlers */
/**
 * valid_handler: valid handler
 * @msg: pointer to struct nl_msg
 * @arg: argument
 *
 * return NL state.
 */

int valid_handler(struct nl_msg *msg, void *arg)
{
	int *err = (int *)arg;
	*err = 0;
	return NL_SKIP;
}

/**
 * ack_handler: ack handler
 * @msg: pointer to struct nl_msg
 * @arg: argument
 *
 * return NL state.
 */

int ack_handler(struct nl_msg *msg, void *arg)
{
	int *err = (int *)arg;
	*err = 0;
	return NL_STOP;
}

/**
 * finish_handler: finish handler
 * @msg: pointer to struct nl_msg
 * @arg: argument
 *
 * return NL state.
 */

int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = (int *)arg;
	*ret = 0;
	return NL_SKIP;
}

/**
 * error_handler: error handler
 * @msg: pointer to struct sockaddr
 * @err: pointer to nlmsgerr
 * @arg: argument
 *
 * return NL state.
 */

int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
	int *ret = (int *)arg;
	*ret = err->error;
	printf("Error received: %d \n", err->error);
	if (err->error > 0) {
		*ret = -(err->error);
	}
	return NL_SKIP;
}

/* Event handlers */
/**
 * response_handler: response handler
 * @msg: pointer to struct nl_msg
 * @data: data
 *
 * return NL state.
 */

int response_handler(struct nl_msg *msg, void *data)
{
	struct genlmsghdr *header = NULL;
	struct nlattr *attributes[NL80211_ATTR_MAX_INTERNAL + 1];
	struct nlattr *attr_vendor[NL80211_ATTR_MAX_INTERNAL];
	char *vendata = NULL;
	int datalen = 0;
	size_t response_len = 0;
	int result = 0;
	struct cfg80211_data *cfgdata = (struct cfg80211_data *)data;
	u_int32_t *temp = NULL;

	header = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	result = nla_parse(attributes, NL80211_ATTR_MAX_INTERNAL, genlmsg_attrdata(header, 0),
			genlmsg_attrlen(header, 0), NULL);

	if (result) {
		printf ("In %s:  nla_parse() failed with %d value", __func__, result);
		return -EINVAL;
	}

	if (attributes[NL80211_ATTR_VENDOR_DATA]) {
		vendata = ((char *)nla_data(attributes[NL80211_ATTR_VENDOR_DATA]));
		datalen = nla_len(attributes[NL80211_ATTR_VENDOR_DATA]);
		if (!vendata) {
			fprintf(stderr, "Vendor data not found\n");
			return -EINVAL;
		}
	} else {
		fprintf(stderr, "NL80211_ATTR_VENDOR_DATA not found\n");
		return -EINVAL;
	}

	if (cfgdata->parse_data)  {
		cfgdata->nl_vendordata = vendata;
		cfgdata->nl_vendordata_len = datalen;
		if (cfgdata->callback) {
			cfgdata->callback(cfgdata);
			return NL_OK;
		}
	}

	/* extract data from NL80211_ATTR_VENDOR_DATA attributes */
	nla_parse(attr_vendor, QCA_WLAN_VENDOR_ATTR_PARAM_MAX,
			(struct nlattr *)vendata,
			datalen, NULL);

	if (attr_vendor[QCA_WLAN_VENDOR_ATTR_PARAM_DATA]) {
		/* memcpy tb_vendor to data */
		temp = nla_data(attr_vendor[QCA_WLAN_VENDOR_ATTR_PARAM_DATA]);
		response_len = nla_get_u32(attr_vendor[QCA_WLAN_VENDOR_ATTR_PARAM_LENGTH]);

		if (response_len <= cfgdata->length) {
			memcpy(cfgdata->data, temp, response_len);
		} else {
			cfgdata->data = temp;
		}

		cfgdata->length = response_len;

		if (cfgdata->callback) {
			cfgdata->callback(cfgdata);
		}
	}

	if (attr_vendor[QCA_WLAN_VENDOR_ATTR_PARAM_FLAGS]) {
		cfgdata->flags = nla_get_u32(attr_vendor[QCA_WLAN_VENDOR_ATTR_PARAM_FLAGS]);
	}

	return NL_OK;
}

/**
 * attr_start: start nest attr
 * @nlmsg: pointer to struct nl_msg
 * @attribute : attribute value
 * return pointer to struct nlattr
 */
struct nlattr *attr_start(struct nl_msg *nlmsg, int attribute)
{
	return nla_nest_start(nlmsg, attribute);
}

/**
 * attr_end: end nest attr
 * @nlmsg: pointer to struct nl_msg
 * @attribute : attribute value
 */
void attr_end(struct nl_msg *nlmsg, struct nlattr *attr)
{
	nla_nest_end(nlmsg, attr);
}

/**
 * start_vendor_data: start nest attr
 * @nlmsg: pointer to struct nl_msg
 * return pointer to struct nlattr
 */
struct nlattr *start_vendor_data(struct nl_msg *nlmsg)
{
	return attr_start(nlmsg, NL80211_ATTR_VENDOR_DATA);
}

/**
 * end_vendor_data: end nest attr
 * @nlmsg: pointer to struct nl_msg
 * @attr : pointer to struct nlattr
 */
void end_vendor_data(struct nl_msg *nlmsg, struct nlattr *attr)
{
	attr_end(nlmsg, attr);
}

/**
 * no_seq_check: no sequence check
 * @msg: pointer to struct nl_msg
 * @arg: argument
 *
 * return NL state.
 */
int no_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

/**
 * put_u32: nla_put wrapper
 * @msg: pointer to struct nl_msg
 * @atrribute: atrribute
 * @value: value
 *
 * return NL state.
 */
int put_u32(struct nl_msg *nlmsg, int attribute, uint32_t value)
{
	return nla_put(nlmsg, attribute, sizeof(value), &value);
}

/**
 * set_iface_id: set interface id
 * @msg: pointer to struct nl_msg
 * @iface: pointer to iface
 *
 * return NL state.
 */
int set_iface_id(struct nl_msg *nlmsg, const char *iface)
{
	if (iface[0] == '\0') {
		return 0;
	}

	if (put_u32(nlmsg, NL80211_ATTR_IFINDEX, if_nametoindex(iface))) {
		return -EINVAL;
	}
	return 0;
}

/**
 * send_nlmsg: sends nl msg
 * @ctx: pointer to wifi_cfg80211_context
 * @nlmsg: pointer to nlmsg
 * @data: data
 *
 * return NL state.
 */
int send_nlmsg(wifi_cfg80211_context *ctx, struct nl_msg *nlmsg, void *data)
{
	int err = -1, res = 0;
	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);

	if (!cb) {
		goto out;
	}

	if (pthread_mutex_lock(&ctx->cmd_sock_lock)) {
		fprintf(stderr, "failed to acquire lock in %s\n", __func__);
		goto out;
	}

	/* send message */
	err = nl_send_auto_complete(ctx->cmd_sock, nlmsg);

	if (err < 0) {
		pthread_mutex_unlock(&ctx->cmd_sock_lock);
		goto out;
	}
	err = 1;

	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, response_handler, data);

	/*   wait for reply */
	while (err > 0) {  /* error will be set by callbacks */
		res = nl_recvmsgs(ctx->cmd_sock, cb);
		if (res) {
			fprintf(stderr, "nl80211: %s->nl_recvmsgs failed: %d\n", __func__, res);
		}
	}
	pthread_mutex_unlock(&ctx->cmd_sock_lock);
out:
	if (cb)
		nl_cb_put(cb);
	if (nlmsg) {
		nlmsg_free(nlmsg);
	}
	return err;
}

/**
 * recv_nlmsg_nb: receive message on event socket
 * @ctx: pointer to wifi_cfg80211_context
 *
 * return 0 on success or a negative error code
 */

int recv_nlmsg_nb(wifi_cfg80211_context *ctx)
{
	int err = -1;

	err = nl_recvmsgs_default(ctx->event_sock);
	return err;
}

/**
 * payload_calculate: calculate the total qca wifi command netlink
 * message size based on payload data length
 * @len:  The payload data length
 *
 * return the calculated size
 *
 * In total 6 NL attributes (each of which takes 4B in value) below will be added first:
 * NL80211_ATTR_VENDOR_ID
 * NL80211_ATTR_VENDOR_SUBCMD
 * NL80211_ATTR_IFINDEX
 * QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND
 * QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH
 * QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS
 *
 * 1 Byte added for QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID
 *
 * Followed by an empty vendor start attribute.
 * Finally plus the attribute(QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA)
 * containing the actual payload.
 */
static int payload_calculate(int len)
{
	int payload_len = GENL_HDRLEN;

	payload_len += (6 * nla_total_size(4));
#if defined(QCANL_LIB_SUPPORT_SINGLE_WIPHY_MLO) || defined(QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO)
	payload_len += nla_total_size(1);
#endif
	payload_len += nla_total_size(0);
	payload_len += nla_total_size(len);

	return nlmsg_total_size(payload_len);
}

struct nl_msg *wifi_cfg80211_prepare_command_len(wifi_cfg80211_context *ctx, int cmdid, const char *ifname, int len)
{
	int res;
	struct nl_msg *nlmsg;

	if (!len) {
		nlmsg = nlmsg_alloc();
	} else {
		int calc_size = payload_calculate(len);
		nlmsg = nlmsg_alloc_size(calc_size);
	}

	if (!nlmsg) {
		fprintf(stderr, "Out of memory\n");
		return NULL;
	}

	genlmsg_put(nlmsg, 0, 0, ctx->nl80211_family_id,
			0, 0, NL80211_CMD_VENDOR, 0);

	res = put_u32(nlmsg, NL80211_ATTR_VENDOR_ID, QCA_VENDOR_OUI);
	if (res < 0) {
		fprintf(stderr, "Failed to put vendor id\n");
		nlmsg_free(nlmsg);
		return NULL;
	}

	/* SET_WIFI_CONFIGURATION = 72 */
	res = put_u32(nlmsg, NL80211_ATTR_VENDOR_SUBCMD, cmdid);
	if (res < 0) {
		fprintf(stderr, "Failed to put vendor sub command\n");
		nlmsg_free(nlmsg);
		return NULL;
	}

	set_iface_id(nlmsg, ifname);

	return nlmsg;
}

struct nl_msg *wifi_cfg80211_prepare_command(wifi_cfg80211_context *ctx, int cmdid, const char *ifname)
{
	return wifi_cfg80211_prepare_command_len(ctx, cmdid, ifname, 0);
}

#ifdef QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO
int wifi_cfg80211_user_send_generic_command(wifi_cfg80211_context *ctx,
					    int vendor_command, int cmdid,
					    int value, const char *ifname, uint8_t link,
					    char *buffer, uint32_t len)
#else
int wifi_cfg80211_user_send_generic_command(wifi_cfg80211_context *ctx,
					    int vendor_command, int cmdid,
					    int value, const char *ifname,
					    char *buffer, uint32_t len)
#endif
{
	struct nl_msg *nlmsg = NULL;
	int res = -EIO;
	struct nlattr *nl_venData = NULL;
	struct cfg80211_data *cfgdata = (struct cfg80211_data *) buffer;
	nlmsg = wifi_cfg80211_prepare_command(ctx, vendor_command, ifname);

	/* Prepare Actual Payload
	   1. nla_put - command ID.
	   2. nla_put - data
	   3. nla_put length
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
	 */
	if (nlmsg) {
		nl_venData = (struct nlattr *)start_vendor_data(nlmsg);
		if (!nl_venData) {
			fprintf(stderr, "failed to start vendor data\n");
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nla_put_u32(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH, len)) {
			fprintf(stderr, "\n Failed nla_put_u32, \n");
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put_u32(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND, cmdid)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put_u32(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS, cfgdata->flags)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
#ifdef QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO
		if (link != MLO_INVALID_LINK_ID) {
			if (vendor_command == QCA_NL80211_VENDOR_SUBCMD_SET_WIFI_CONFIGURATION ||
				vendor_command == QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_CONFIGURATION) {
				if (nla_put_u8(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
					link)) {
					nlmsg_free(nlmsg);
					return -EIO;
				}
			}
		}
#endif
		if (nla_put(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA,
			cfgdata->length, cfgdata->data)) {
			nlmsg_free(nlmsg);
			return -EIO;
		} if (nla_put_u32(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE, value)) {
			nlmsg_free(nlmsg);
			return -EIO;
		} else {
			if (nl_venData) {
				end_vendor_data(nlmsg, nl_venData);
			}
			res = send_nlmsg(ctx, nlmsg, buffer);

			if (res < 0) {
				return res;
			}
			return res;
		}

	} else {
		return -EIO;
	}
	return res;
}

/**
 * swifi_cfg80211_send_mctbl: sends multicast forward table.
 * @vendor_command: vendor command
 * @cmdid: command id
 * @ifname: interface name
 * @link :link id of MLD netdev
 * @buffer: data
 * @size: size in bytes that data occupied.
 *
 * return NL state.
 */
#ifdef QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO
int wifi_cfg80211_send_mctbl(wifi_cfg80211_context *ctx, int vendor_command, int cmdid, const char *ifname, uint8_t link, char *buffer, int size)
#else
int wifi_cfg80211_send_mctbl(wifi_cfg80211_context *ctx, int vendor_command, int cmdid, const char *ifname, char *buffer, int size)
#endif
{
#define SEG_SIZE 64000
	struct cfg80211_data *cfgdata = (struct cfg80211_data *) buffer;
	int remaining = cfgdata->length;
	int res = -EIO;

	cfgdata->length = SEG_SIZE;
	cfgdata->flags = QCA_WLAN_VENDOR_FLAGS_MCTBL_SEG_FIRST;

	/*
	 * size = 0 means flush the table.
	 * and hence this data will be treated as first/last.
	 */
	if (!size) {
		goto last_segment;
	}

	while(remaining > SEG_SIZE){
#ifdef QCANL_LIB_SUPPORT_SINGLE_WIPHY_MLO
		res = wifi_cfg80211_send_generic_command(ctx, vendor_command, cmdid, ifname, MLO_INVALID_LINK_ID, buffer, SEG_SIZE);
#elif QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO
		res = wifi_cfg80211_send_generic_command(ctx, vendor_command, cmdid, ifname, link, buffer, SEG_SIZE);
#else
		res = wifi_cfg80211_send_generic_command(ctx, vendor_command, cmdid, ifname, buffer, SEG_SIZE);
#endif
		if(res < 0){
			fprintf(stderr, "segment send failed, remaining: %d\n", remaining);
			return res;
		}
		cfgdata->flags &= ~QCA_WLAN_VENDOR_FLAGS_MCTBL_SEG_FIRST;
		cfgdata->data += SEG_SIZE;
		remaining -= SEG_SIZE;
		size -= SEG_SIZE;
	}

last_segment:
	cfgdata->flags |= QCA_WLAN_VENDOR_FLAGS_MCTBL_SEG_LAST;
	cfgdata->length = (size > SEG_SIZE)? SEG_SIZE: size;
#ifdef QCANL_LIB_SUPPORT_SINGLE_WIPHY_MLO
	res = wifi_cfg80211_send_generic_command(ctx, vendor_command, cmdid, ifname, MLO_INVALID_LINK_ID, buffer, cfgdata->length);
#elif QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO
	res = wifi_cfg80211_send_generic_command(ctx, vendor_command, cmdid, ifname, link, buffer, cfgdata->length);
#else
	res = wifi_cfg80211_send_generic_command(ctx, vendor_command, cmdid, ifname, buffer, cfgdata->length);
#endif
	if(res < 0){
		fprintf(stderr, "segment send failed in last segment, remaining: %d\n", remaining);
	}

	return res;

}

#ifdef QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO
int wifi_cfg80211_send_generic_command_optional_params(
		wifi_cfg80211_context *ctx, int vendor_command, int cmdid,
		const char *ifname, uint8_t link, char *buffer, int len,
		struct optional_params *opt_params)
#else
int wifi_cfg80211_send_generic_command_optional_params(
		wifi_cfg80211_context *ctx, int vendor_command, int cmdid,
		const char *ifname, char *buffer, int len,
		struct optional_params *opt_params)
#endif
{
	struct nl_msg *nlmsg = NULL;
	int res = -EIO, i = 0;
	struct nlattr *nl_venData = NULL;
	struct cfg80211_data *cfgdata = (struct cfg80211_data *) buffer;

	nlmsg = wifi_cfg80211_prepare_command_len(ctx, vendor_command, ifname, len);

	/* Prepare Actual Payload
	   1. nla_put - command ID.
	   2. nla_put - data
	   3. nla_put length
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
	   optional params:
	   use enum values that are reserved for QCA internal usage
	   Only u32 attributes are supported for the optional params
	 */
	if (nlmsg) {
		nl_venData = (struct nlattr *)start_vendor_data(nlmsg);
		if (!nl_venData) {
			fprintf(stderr, "failed to start vendor data\n");
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nla_put_u32(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH, len)) {
			fprintf(stderr, "\n Failed nla_put_u32, \n");
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put_u32(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND, cmdid)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put_u32(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS, cfgdata->flags)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (opt_params->num_params > MAX_OPTIONAL_PARAMS) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
		/* Fill optional params attributes and values */
		for (i = 0; i < opt_params->num_params; i++) {
			if (nla_put_u32(nlmsg, opt_params->attr_num[i],
						opt_params->attr_value[i])) {
				nlmsg_free(nlmsg);
				return -EIO;
			}
		}
#ifdef QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO
		if (link != MLO_INVALID_LINK_ID) {
			if (vendor_command == QCA_NL80211_VENDOR_SUBCMD_SET_WIFI_CONFIGURATION ||
				vendor_command == QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_CONFIGURATION) {
				if (nla_put_u8(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
					link)) {
					nlmsg_free(nlmsg);
					return -EIO;
				}
			}
		}
#endif
		if (nla_put(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA,
			cfgdata->length, cfgdata->data)) {
			nlmsg_free(nlmsg);
			return -EIO;
		} else {
			if (nl_venData) {
				end_vendor_data(nlmsg, nl_venData);
			}
			res = send_nlmsg(ctx, nlmsg, buffer);

			if (res < 0) {
				return res;
			}
			return res;
		}

	} else {
		return -EIO;
	}
	return res;
}

#ifdef QCANL_LIB_SUPPORT_SINGLE_WIPHY_MLO
int wifi_cfg80211_send_generic_command(wifi_cfg80211_context *ctx, int vendor_command, int cmdid, const char *ifname, uint8_t link, char *buffer, int len)
#else
int wifi_cfg80211_send_generic_command(wifi_cfg80211_context *ctx, int vendor_command, int cmdid, const char *ifname, char *buffer, int len)
#endif
{
	struct nl_msg *nlmsg = NULL;
	int res = -EIO;
	struct nlattr *nl_venData = NULL;
	struct cfg80211_data *cfgdata = (struct cfg80211_data *) buffer;
	nlmsg = wifi_cfg80211_prepare_command_len(ctx, vendor_command, ifname, len);

	/* Prepare Actual Payload
	   1. nla_put - command ID.
	   2. nla_put - data
	   3. nla_put length
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
	 */
	if (nlmsg) {
		nl_venData = (struct nlattr *)start_vendor_data(nlmsg);
		if (!nl_venData) {
			fprintf(stderr, "failed to start vendor data\n");
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nla_put_u32(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH, len)) {
			fprintf(stderr, "\n Failed nla_put_u32, \n");
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put_u32(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND, cmdid)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put_u32(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS, cfgdata->flags)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
#ifdef QCANL_LIB_SUPPORT_SINGLE_WIPHY_MLO
		if (link != MLO_INVALID_LINK_ID) {
			if (vendor_command == QCA_NL80211_VENDOR_SUBCMD_SET_WIFI_CONFIGURATION ||
				vendor_command == QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_CONFIGURATION) {
				if (nla_put_u8(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
					link)) {
					nlmsg_free(nlmsg);
					return -EIO;
				}
			}
		}
#endif
		if (nla_put(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA,
			cfgdata->length, cfgdata->data)) {
			nlmsg_free(nlmsg);
			return -EIO;
		} else {
			if (nl_venData) {
				end_vendor_data(nlmsg, nl_venData);
			}
			res = send_nlmsg(ctx, nlmsg, buffer);

			if (res < 0) {
				return res;
			}
			return res;
		}

	} else {
		return -EIO;
	}
	return res;
}

int wifi_cfg80211_send_getparam_command_linkid(wifi_cfg80211_context *ctx, int cmdid,
		int param, const char *ifname, uint8_t link, char *buffer, int len)
{
        struct nl_msg *nlmsg = NULL;
        int res = -EIO;
        struct nlattr *nl_venData = NULL;
        nlmsg = wifi_cfg80211_prepare_command(ctx,
                        QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_CONFIGURATION,
                        ifname);

        /* Prepare Actual Payload
           1. nla_put - command ID.
           2. nla_put - data
           3. nla_put length
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND,
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE,
           QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
         */
        if (nlmsg) {
                nl_venData = (struct nlattr *)start_vendor_data(nlmsg);
                if (!nl_venData) {
                        fprintf(stderr, "failed to start vendor data\n");
                        nlmsg_free(nlmsg);
                        return -EIO;
                }
                if (link != MLO_INVALID_LINK_ID) {
                        if (nla_put_u8(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
                                link)) {
                                nlmsg_free(nlmsg);
                                return -EIO;
                        }
                }
                if (nla_put_u32(nlmsg,
                        QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND, cmdid)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }
                if (nla_put_u32(nlmsg,
                        QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE, param)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }

                if (nl_venData) {
                        end_vendor_data(nlmsg, nl_venData);
                }
                res = send_nlmsg(ctx, nlmsg, buffer);

                if (res < 0) {
                        return res;
                }
                return res;
        } else {
                return -EIO;
        }

        return res;
}

int wifi_cfg80211_send_getparam_command(wifi_cfg80211_context *ctx, int cmdid,
		int param, const char *ifname, char *buffer, int len)
{
	struct nl_msg *nlmsg = NULL;
	int res = -EIO;
	struct nlattr *nl_venData = NULL;
	char vap_name[IF_NAME_SIZE];

	if (ctx->chip_type == WLAN_CHIP_TYPE_MCC &&
		strncmp(ifname, MCC_VAP_NAME_PREFIX, MCC_VAP_NAME_PREFIX_SIZE)) {
		get_vap_name_from_radio_name(ifname, vap_name);

		nlmsg = wifi_cfg80211_prepare_command(ctx,
				QCA_NL80211_VENDOR_SUBCMD_SET_WIFI_CONFIGURATION,
				vap_name);
	} else {
		nlmsg = wifi_cfg80211_prepare_command(ctx,
				QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_CONFIGURATION,
				ifname);
	}

	/* Prepare Actual Payload
	   1. nla_put - command ID.
	   2. nla_put - data
	   3. nla_put length
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND,
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE,
	 */
	if (nlmsg) {
		nl_venData = (struct nlattr *)start_vendor_data(nlmsg);
		if (!nl_venData) {
			fprintf(stderr, "failed to start vendor data\n");
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put_u32(nlmsg,
			QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND, cmdid)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put_u32(nlmsg,
			QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE, param)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nl_venData) {
			end_vendor_data(nlmsg, nl_venData);
		}
		res = send_nlmsg(ctx, nlmsg, buffer);

		if (res < 0) {
			return res;
		}
		return res;
	} else {
		return -EIO;
	}

	return res;
}

int wifi_cfg80211_send_setparam_command_optional_params_linkid(
		wifi_cfg80211_context *ctx, int cmdid,
		int param, const char *ifname, uint8_t link, char *buffer,
		int param_attribute_length, unsigned int param_attribute_flags,
		struct optional_params *opt_params)
{
        struct nl_msg *nlmsg = NULL;
        int res = -EIO, i;
        struct nlattr *nl_venData = NULL;
        struct cfg80211_data *cfgdata = (struct cfg80211_data *) buffer;
        nlmsg = wifi_cfg80211_prepare_command(ctx,
                        QCA_NL80211_VENDOR_SUBCMD_SET_WIFI_CONFIGURATION,
                        ifname);

        /* Prepare Actual Payload
           1. nla_put - command ID.
           2. nla_put - data
           3. nla_put length
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND(17),
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE(18),
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA(19),
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH(20),
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS(21),
           QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
           optional params:
           use enum values that are reserved for QCA internal usage
           Only u32 attributes are supported for the optional params
         */
        if (nlmsg) {
                nl_venData = (struct nlattr *)start_vendor_data(nlmsg);
                if (!nl_venData) {
                        fprintf(stderr, "failed to start vendor data\n");
                        nlmsg_free(nlmsg);
                        return -EIO;
                }

                if (nla_put_u32(nlmsg,
                        QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND, cmdid)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }
                if (nla_put_u32(nlmsg,
                        QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE, param)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }

                if (nla_put_u32(nlmsg,
                        QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH,
                        param_attribute_length)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }

                if (opt_params->num_params > MAX_OPTIONAL_PARAMS) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }
                /* Fill optional params attributes and values */
                for (i = 0; i < opt_params->num_params; i++) {
                        fprintf(stderr, "Optional Params%d: num: %d : value:%d\n",
                                i, opt_params->attr_num[i], opt_params->attr_value[i]);
                        if (nla_put_u32(nlmsg, opt_params->attr_num[i],
                                                opt_params->attr_value[i])) {
                                nlmsg_free(nlmsg);
                                return -EIO;
                        }
                }
                if (link != MLO_INVALID_LINK_ID) {
                        if (nla_put_u8(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
                                link)) {
                                nlmsg_free(nlmsg);
                                return -EIO;
                        }
                }
                if (nla_put(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA,
                        cfgdata->length, cfgdata->data)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }

                if (nla_put_u32(nlmsg,
                        QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS,
                        param_attribute_flags)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }

                if (nl_venData) {
                        end_vendor_data(nlmsg, nl_venData);
                }
                res = send_nlmsg(ctx, nlmsg, buffer);

                if (res < 0) {
                        return res;
                }
                return res;
        } else {
                return -EIO;
        }

        return res;
}

int wifi_cfg80211_send_setparam_command_optional_params(
		wifi_cfg80211_context *ctx, int cmdid,
		int param, const char *ifname, char *buffer,
		int param_attribute_length, unsigned int param_attribute_flags,
		struct optional_params *opt_params)
{
	struct nl_msg *nlmsg = NULL;
	int res = -EIO, i;
	struct nlattr *nl_venData = NULL;
	struct cfg80211_data *cfgdata = (struct cfg80211_data *) buffer;
	nlmsg = wifi_cfg80211_prepare_command(ctx,
			QCA_NL80211_VENDOR_SUBCMD_SET_WIFI_CONFIGURATION,
			ifname);

	/* Prepare Actual Payload
	   1. nla_put - command ID.
	   2. nla_put - data
	   3. nla_put length
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND(17),
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE(18),
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA(19),
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH(20),
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS(21),
	   optional params:
	   use enum values that are reserved for QCA internal usage
	   Only u32 attributes are supported for the optional params
	 */
	if (nlmsg) {
		nl_venData = (struct nlattr *)start_vendor_data(nlmsg);
		if (!nl_venData) {
			fprintf(stderr, "failed to start vendor data\n");
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nla_put_u32(nlmsg,
			QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND, cmdid)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put_u32(nlmsg,
			QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE, param)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nla_put_u32(nlmsg,
			QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH,
			param_attribute_length)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (opt_params->num_params > MAX_OPTIONAL_PARAMS) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
		/* Fill optional params attributes and values */
		for (i = 0; i < opt_params->num_params; i++) {
			fprintf(stderr, "Optional Params%d: num: %d : value:%d\n",
				i, opt_params->attr_num[i], opt_params->attr_value[i]);
			if (nla_put_u32(nlmsg, opt_params->attr_num[i],
						opt_params->attr_value[i])) {
				nlmsg_free(nlmsg);
				return -EIO;
			}
		}
		if (nla_put(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA,
			cfgdata->length, cfgdata->data)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nla_put_u32(nlmsg,
			QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS,
			param_attribute_flags)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nl_venData) {
			end_vendor_data(nlmsg, nl_venData);
		}
		res = send_nlmsg(ctx, nlmsg, buffer);

		if (res < 0) {
			return res;
		}
		return res;
	} else {
		return -EIO;
	}

	return res;
}

int wifi_cfg80211_send_setparam_command_linkid(wifi_cfg80211_context *ctx, int cmdid,
		int param, const char *ifname, uint8_t link, char *buffer,
		int param_attribute_length, unsigned int param_attribute_flags)
{
        struct nl_msg *nlmsg = NULL;
        int res = -EIO;
        struct nlattr *nl_venData = NULL;
        struct cfg80211_data *cfgdata = (struct cfg80211_data *) buffer;
        nlmsg = wifi_cfg80211_prepare_command(ctx,
                        QCA_NL80211_VENDOR_SUBCMD_SET_WIFI_CONFIGURATION,
                        ifname);

        /* Prepare Actual Payload
           1. nla_put - command ID.
           2. nla_put - data
           3. nla_put length
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND(17),
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE(18),
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA(19),
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH(20),
           QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS(21),
           QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
         */
        if (nlmsg) {
                nl_venData = (struct nlattr *)start_vendor_data(nlmsg);
                if (!nl_venData) {
                        fprintf(stderr, "failed to start vendor data\n");
                        nlmsg_free(nlmsg);
                        return -EIO;
                }

                if (nla_put_u32(nlmsg,
                        QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND, cmdid)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }
                if (nla_put_u32(nlmsg,
                        QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE, param)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }

                if (nla_put_u32(nlmsg,
                        QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH,
                        param_attribute_length)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }
                if (link != MLO_INVALID_LINK_ID) {
                        if (nla_put_u8(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
                                link)) {
                                nlmsg_free(nlmsg);
                                return -EIO;
                        }
                }
                if (nla_put(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA,
                        cfgdata->length, cfgdata->data)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }

                if (nla_put_u32(nlmsg,
                        QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS,
                        param_attribute_flags)) {
                        nlmsg_free(nlmsg);
                        return -EIO;
                }

                if (nl_venData) {
                        end_vendor_data(nlmsg, nl_venData);
                }
                res = send_nlmsg(ctx, nlmsg, buffer);

                if (res < 0) {
                        return res;
                }
                return res;
        } else {
                return -EIO;
        }

        return res;
}

int wifi_cfg80211_send_setparam_command(wifi_cfg80211_context *ctx, int cmdid,
		int param, const char *ifname, char *buffer,
		int param_attribute_length, unsigned int param_attribute_flags)
{
	struct nl_msg *nlmsg = NULL;
	int res = -EIO;
	struct nlattr *nl_venData = NULL;
	struct cfg80211_data *cfgdata = (struct cfg80211_data *) buffer;
	char vap_name[IF_NAME_SIZE];

	if (ctx->chip_type == WLAN_CHIP_TYPE_MCC &&
		strncmp(ifname, MCC_VAP_NAME_PREFIX, MCC_VAP_NAME_PREFIX_SIZE)) {
		get_vap_name_from_radio_name(ifname, vap_name);

		nlmsg = wifi_cfg80211_prepare_command(ctx,
				QCA_NL80211_VENDOR_SUBCMD_SET_WIFI_CONFIGURATION,
				vap_name);
	} else {
		nlmsg = wifi_cfg80211_prepare_command(ctx,
				QCA_NL80211_VENDOR_SUBCMD_SET_WIFI_CONFIGURATION,
				ifname);
	}

	/* Prepare Actual Payload
	   1. nla_put - command ID.
	   2. nla_put - data
	   3. nla_put length
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND(17),
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE(18),
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA(19),
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH(20),
	   QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS(21),
	 */
	if (nlmsg) {
		nl_venData = (struct nlattr *)start_vendor_data(nlmsg);
		if (!nl_venData) {
			fprintf(stderr, "failed to start vendor data\n");
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nla_put_u32(nlmsg,
			QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_COMMAND, cmdid)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put_u32(nlmsg,
			QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_VALUE, param)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nla_put_u32(nlmsg,
			QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_LENGTH,
			param_attribute_length)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}
		if (nla_put(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_DATA,
			cfgdata->length, cfgdata->data)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nla_put_u32(nlmsg,
			QCA_WLAN_VENDOR_ATTR_CONFIG_GENERIC_FLAGS,
			param_attribute_flags)) {
			nlmsg_free(nlmsg);
			return -EIO;
		}

		if (nl_venData) {
			end_vendor_data(nlmsg, nl_venData);
		}
		res = send_nlmsg(ctx, nlmsg, buffer);

		if (res < 0) {
			return res;
		}
		return res;
	} else {
		return -EIO;
	}

	return res;
}

struct nl_msg *wifi_cfg80211_prepare_standard_command(wifi_cfg80211_context *ctx,
		const char *wifi_name, const char *vap_name, char *buffer)
{
	signed long long devidx = 0;
	struct nlwrapper_data *data = NULL;
	struct nl_msg *nlmsg = NULL;

	data = (struct nlwrapper_data *)buffer;
	nlmsg = nlmsg_alloc();
	if (nlmsg == NULL) {
		fprintf(stderr, "Out of memory\n");
		return NULL;
	}

	if (wifi_name && vap_name) {
		switch (data->cmd) {
		case NL80211_CMD_NEW_INTERFACE:
			devidx = if_nametoindex(wifi_name);
		break;
		case NL80211_CMD_DEL_INTERFACE:
		case NL80211_CMD_SET_INTERFACE:
			devidx = if_nametoindex(vap_name);
		break;

		default:
			devidx = 0;
		break;
		}
	} else if (wifi_name) {
		devidx = if_nametoindex(wifi_name);
	} else if (vap_name) {
		devidx = if_nametoindex(vap_name);
	}

	if (devidx == 0)
		devidx = -1;

	genlmsg_put(nlmsg, 0, 0, ctx->nl80211_family_id, 0,
		0, data->cmd, 0);

	NLA_PUT_U32(nlmsg, NL80211_ATTR_IFINDEX, devidx);

	switch (data->cmd) {
		case NL80211_CMD_NEW_INTERFACE:
			if (vap_name) {
				NLA_PUT_STRING(nlmsg, NL80211_ATTR_IFNAME, vap_name);
			}
			if (data->value) {
				NLA_PUT_U32(nlmsg, NL80211_ATTR_IFTYPE, data->value);
			}
			break;
		case NL80211_CMD_SET_INTERFACE:
			if ((data->value & NL80211_ATTR_4ADDR) == NL80211_ATTR_4ADDR) {
				NLA_PUT_U8(nlmsg, NL80211_ATTR_4ADDR, 1);
			}
			break;
		case NL80211_CMD_SET_WIPHY:
			if (data->value) {
				if (data->flags & NL80211_ATTR_32BIT)
					NLA_PUT_U32(nlmsg, data->attr, data->value);
				else if (data->flags & NL80211_ATTR_8BIT)
					NLA_PUT_U8(nlmsg, data->attr, data->value);
			}
			break;

		default:
			/* nl80211 cmd default */
			break;
	}

	return nlmsg;

nla_put_failure:
	fprintf(stderr, "nla_put_failure\n");
	nlmsg_free(nlmsg);
	return NULL;
}

int wifi_cfg80211_send_nl80211_standard_command(wifi_cfg80211_context *ctx,
		uint8_t *wifi_name, uint8_t *vap_name, char *buffer)
{
	int err = 1, res=0;
	struct nl_cb *cb;
	struct nl_cb *s_cb;
	struct nl_msg *nlmsg;
	struct cfg80211_data data;

	nlmsg = wifi_cfg80211_prepare_standard_command(ctx, (const char *)wifi_name, (const char *)vap_name, buffer);
	if (!nlmsg) {
		fprintf(stderr, "failed to prepare netlink message\n");
		return 2;
	}

	if (pthread_mutex_lock(&ctx->cmd_sock_lock)) {
		nlmsg_free(nlmsg);
		fprintf(stderr, "failed to acquire lock in %s\n", __func__);
		return -1;
	}

	cb = nl_cb_alloc(NL_CB_DEFAULT);
	s_cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb || !s_cb) {
		fprintf(stderr, "failed to allocate netlink callbacks\n");
		err = 2;
		goto out;
	}

	nl_socket_set_cb(ctx->cmd_sock, s_cb);
	err = nl_send_auto_complete(ctx->cmd_sock, nlmsg);
	if (err < 0) {
		goto out;
	}
	err = 1;

	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, valid_handler, (void *)&data);
	while (err > 0) {
		res = nl_recvmsgs(ctx->cmd_sock, cb);
		if (res) {
			printf("nl80211: %s->nl_recvmsgs failed: %d\n", __func__, res);
		}
	}
out:
	pthread_mutex_unlock(&ctx->cmd_sock_lock);
	nl_cb_put(cb);
	nl_cb_put(s_cb);
	nlmsg_free(nlmsg);
	return err;
}

/**
 * wifi_cfg80211_sendcmd: sends cfg80211 sendcmd
 * @ctx: pointer to wifi_cfg80211_context
 * @cmdid: command id
 * @ifname: interface name
 * @buffer: buffer data
 * @len: length
 *
 * return NL state.
 */

int wifi_cfg80211_sendcmd(wifi_cfg80211_context *ctx, int cmdid, const char *ifname,
		char *buffer, int len)
{
	struct nl_msg *nlmsg = NULL;
	int res;
	struct cfg80211_data *cfgdata = (struct cfg80211_data *) buffer;
	nlmsg = wifi_cfg80211_prepare_command(ctx, cmdid, ifname);

	if (nlmsg) {
		if (nla_put(nlmsg, NL80211_ATTR_VENDOR_DATA, cfgdata->length, cfgdata->data)) {
			printf("\n nla_put failed for cmdid %d\n", cmdid);
			fprintf(stderr, "\n Failed nla_put, %d, cmdid %d\n", len, cmdid);
			nlmsg_free(nlmsg);
			return -EIO;
		} else {
			res = send_nlmsg(ctx, nlmsg, buffer);
			if (res < 0) {
				return res;
			}
			return res;
		}
	} else {
		return -ENOMEM;
	}
}

/**
 * wifi_socket_set_local_port: set local port
 * @sock: pointer to nl_sock
 * @port: port
 *
 */
void wifi_socket_set_local_port(struct nl_sock *sock, uint32_t port)
{
#define PID_MASK 0x3FFFFF
#define PORT_SHITF_VAL 22
	uint32_t pid;
	pid = getpid() & PID_MASK;

	if (port == 0) {
		sock->s_flags &= ~NL_OWN_PORT;
	} else {
		sock->s_flags |= NL_OWN_PORT;
	}

	sock->s_local.nl_pid = pid + (port << PORT_SHITF_VAL);
#undef PID_MASK
#undef PORT_SHITF_VAL
}

/**
 * wifi_create_nl_socket: set nl socket
 * @port: pointer to port
 * @protocol: protocol
 *
 * return nl_sock sturct
 */

struct nl_sock *wifi_create_nl_socket(int port, int protocol)
{
	struct nl_sock *sock = nl_socket_alloc();
#if CFG80211_DEBUG
	struct sockaddr_nl *addr_nl = &(sock->s_local);
#endif
	if (sock == NULL) {
		fprintf(stderr, "Failed to create NL socket\n");
		return NULL;
	}

	wifi_socket_set_local_port(sock, port);

#if CFG80211_DEBUG
	printf("socket address is %d:%d:%u:%d\n",
			addr_nl->nl_family, addr_nl->nl_pad, addr_nl->nl_pid,
			addr_nl->nl_groups);

	printf("sizeof(sockaddr) = %zu, sizeof(sockaddr_nl) = %zu\n", sizeof(struct sockaddr),
			sizeof(*addr_nl));
#endif
	if (nl_connect(sock, protocol)) {
		fprintf(stderr, "Could not connect handle\n");
		nl_socket_free(sock);
		return NULL;
	}

	return sock;
}

void *event_thread (void *arg)
{
	wifi_cfg80211_context *ctx = arg;
	ctx->event_thread_running = 1;
	while (ctx->event_thread_running) {
		nl_recvmsgs_default(ctx->event_sock);
	}
	return NULL;
}


static void nl80211_vendor_event(struct nlattr **tb, char *ifname,
wifi_cfg80211_context *nl80211_ctx)
{
	uint32_t vendor_id, subcmd;
	uint8_t *data = NULL;
	size_t len = 0;
	int wiphy_idx = -1;

	if (!tb[NL80211_ATTR_VENDOR_ID] ||
			!tb[NL80211_ATTR_VENDOR_SUBCMD])
		return;

	vendor_id = nla_get_u32(tb[NL80211_ATTR_VENDOR_ID]);
	subcmd = nla_get_u32(tb[NL80211_ATTR_VENDOR_SUBCMD]);

	if (tb[NL80211_ATTR_VENDOR_DATA]) {
		data = nla_data(tb[NL80211_ATTR_VENDOR_DATA]);
		len = nla_len(tb[NL80211_ATTR_VENDOR_DATA]);
	}

	if (tb[NL80211_ATTR_WIPHY]) {
		wiphy_idx = nla_get_u32(tb[NL80211_ATTR_WIPHY]);
	}

	switch (vendor_id) {
		case OUI_QCA:
			if (subcmd == QCA_NL80211_VENDOR_SUBCMD_AFC_EVENT) {
				nl80211_ctx->event_callback_afc_vendor_cmd(ifname, subcmd, data, len, wiphy_idx);
			} else {
				nl80211_ctx->event_callback(ifname, subcmd, data, len);
			}
			break;
	}
}

#define MAX_INTERFACE_NAME_LEN 20
static int process_global_event(struct nl_msg *msg, void *arg)
{
	wifi_cfg80211_context *ctx = (wifi_cfg80211_context *) arg;
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	int ifidx = -1;
	char ifname[MAX_INTERFACE_NAME_LEN] = {0};

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (tb[NL80211_ATTR_IFINDEX])
		ifidx = nla_get_u32(tb[NL80211_ATTR_IFINDEX]);

	if (ifidx != -1) {
		if_indextoname(ifidx, ifname);
	}

	switch (gnlh->cmd) {
		case NL80211_CMD_VENDOR:
			nl80211_vendor_event(tb, ifname, ctx);
			break;
	}

	return NL_SKIP;
}

struct handler_args {
	const char *group;
	int id;
};
static int family_handler(struct nl_msg *msg, void *arg)
{
	struct handler_args *grp = arg;
	struct nlattr *tb[CTRL_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *mcgrp;
	int rem_mcgrp;

	nla_parse(tb, CTRL_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[CTRL_ATTR_MCAST_GROUPS])
		return NL_SKIP;

	nla_for_each_nested(mcgrp, tb[CTRL_ATTR_MCAST_GROUPS], rem_mcgrp) {
		struct nlattr *tb_mcgrp[CTRL_ATTR_MCAST_GRP_MAX + 1];

		nla_parse(tb_mcgrp, CTRL_ATTR_MCAST_GRP_MAX,
				nla_data(mcgrp), nla_len(mcgrp), NULL);

		if (!tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME] ||
				!tb_mcgrp[CTRL_ATTR_MCAST_GRP_ID])
			continue;
		else
			grp->id = nla_get_u32(tb_mcgrp[CTRL_ATTR_MCAST_GRP_ID]);

		if (strncmp(nla_data(tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME]),
				grp->group,
				nla_len(tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME])))
			continue;

		grp->id = nla_get_u32(tb_mcgrp[CTRL_ATTR_MCAST_GRP_ID]);
		break;
	}

	return NL_SKIP;
}

static int nl_get_multicast_id(struct nl_sock *sock, const char *family,
		const char *group)
{
	struct nl_msg *msg;
	struct nl_cb *cb;
	int ret, ctrlid;
	struct handler_args grp = {
		.group = group,
		.id = -ENOENT,
	};

	msg = nlmsg_alloc();
	if (!msg)
		return -ENOMEM;

	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb) {
		ret = -ENOMEM;
		goto out_fail_cb;
	}

	ctrlid = genl_ctrl_resolve(sock, "nlctrl");

#ifdef ANDROID
	genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, GENL_ID_CTRL, 0, 0,
			CTRL_CMD_GETFAMILY, 1);
#else
	genlmsg_put(msg, 0, 0, ctrlid, 0, 0, CTRL_CMD_GETFAMILY, 0);
#endif

	ret = -ENOBUFS;
	NLA_PUT_STRING(msg, CTRL_ATTR_FAMILY_NAME, family);

	ret = nl_send_auto_complete(sock, msg);
	if (ret < 0)
		goto out;

	ret = 1;

	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &ret);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &ret);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &ret);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, family_handler, &grp);

	while (ret > 0)
		nl_recvmsgs(sock, cb);

	if (ret == 0)
		ret = grp.id;
nla_put_failure:
out:
	nl_cb_put(cb);
out_fail_cb:
	nlmsg_free(msg);
	return ret;
}

static int setup_event_mechanism(wifi_cfg80211_context *ctx);

/**
 * wifi_init_nl80211: initiliaze nlsocket
 * @ctx: wifi cfg80211 context
 *
 * return 1/0
 */
int wifi_init_nl80211_largebuf(wifi_cfg80211_context *ctx, uint8_t largebuf)
{
	struct nl_sock *cmd_sock = NULL;

	/**
	* Use private command socket if the application asks to create using
	* a custom socket id. This option is set by the application daemons.
	* Use the default command socket if private socket is not specified.
	*/
	if (ctx->pvt_cmd_sock_id > 0) {
		cmd_sock = wifi_create_nl_socket(ctx->pvt_cmd_sock_id, NETLINK_GENERIC);
	} else {
		cmd_sock = wifi_create_nl_socket(NCT_CMD_SOCK_PORT, NETLINK_GENERIC);
	}

	if (cmd_sock == NULL) {
		fprintf(stderr, "Failed to create command socket port\n");
	        return -EIO;
	}

	/* Size of the nl socket buffer*/
#define ALLOC_SIZE (256*1024)
	/* Set the socket buffer size */
	if (nl_socket_set_buffer_size(cmd_sock, (ALLOC_SIZE), (largebuf? (ALLOC_SIZE) : 0) < 0)) {
		fprintf(stderr, "Could not set nl_socket RX buffer size for cmd_sock: %s\n",
				strerror(errno));
		goto cleanup;
	}

	if (largebuf) {
		nl_socket_set_msg_buf_size(cmd_sock, (ALLOC_SIZE));
	}

	ctx->cmd_sock = cmd_sock;

	ctx->nl80211_family_id = genl_ctrl_resolve(cmd_sock, "nl80211");
	if (ctx->nl80211_family_id < 0) {
		fprintf(stderr, "Could not resolve nl80211 familty id\n");
		goto cleanup;
	}
	if (setup_event_mechanism(ctx)) {
		goto cleanup;
	}
	if (pthread_mutex_init(&ctx->cmd_sock_lock, NULL) != 0) {
		goto cleanup;
	}

	ctx->chip_type = get_wlan_chip_type();
	return 0;
cleanup:
	if (ctx->event_sock) {
		nl_socket_free(ctx->event_sock);
		ctx->event_sock = NULL;
	}
	ctx->cmd_sock = NULL;
	nl_socket_free(cmd_sock);
	return -EIO;
}

/**
 * wifi_init_nl80211: initiliaze nlsocket
 * @ctx: wifi cfg80211 context
 *
 * return 1/0
 */
int wifi_init_nl80211(wifi_cfg80211_context *ctx)
{
	return wifi_init_nl80211_largebuf(ctx, 0);
}

static int setup_event_mechanism(wifi_cfg80211_context *ctx)
{
	struct nl_cb *cb = NULL;
	int err = 0;

	if (!ctx->event_callback && !ctx->event_callback_afc_vendor_cmd) {
		/*
		 * If event_callback is not specified
		 * no need to start event thread and other things
		 */
		return 0;
	}

	/*
	* Use private event socket if the application asks to create using
	* a custom socket id. This option is set by the application daemons.
	* Use the default event socket if private socket is not specified.
	*/
	if (ctx->pvt_event_sock_id > 0) {
		ctx->event_sock = wifi_create_nl_socket(ctx->pvt_event_sock_id,
			NETLINK_GENERIC);
	} else {
		ctx->event_sock = wifi_create_nl_socket(NCT_EVENT_SOCK_PORT,
			NETLINK_GENERIC);
	}

	if (ctx->event_sock == NULL) {
		fprintf(stderr, "Failed to create event socket port\n");
		return -EIO;
	}

	/* replace this with genl_ctrl_resolve_grp() once we move to libnl3 */
	err = nl_get_multicast_id(ctx->event_sock, "nl80211", "vendor");
	if (err >= 0) {
		err = nl_socket_add_membership(ctx->event_sock, err);
		if (err) {
			printf("failed to join testmode group!\n");
			goto cleanup;
		}
	} else
		goto cleanup;

	/* Set the socket buffer size */
	if (nl_socket_set_buffer_size(ctx->event_sock, (ALLOC_SIZE), 0) < 0) {
		fprintf(stderr,
		"Could not set nl_socket RX buffer size for event_sock: %s\n",
		strerror(errno));
		goto cleanup;
	}

	/*
	 * Enable peek mode so drivers can send large amounts
	 * of data in blobs without problems.
	 */
	nl_socket_enable_msg_peek(ctx->event_sock);

	/*
	 * disable sequence checking to handle events.
	 */
	nl_socket_disable_seq_check(ctx->event_sock);

	cb = nl_socket_get_cb(ctx->event_sock);
	if (cb == NULL) {
		fprintf(stderr,
		"Failed to get NL control block for event socket port\n");
		goto cleanup;
	}

	err = 1;
	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, process_global_event,
			ctx);
	nl_cb_put(cb);
	return 0; /* success */
cleanup:
	nl_socket_free(ctx->event_sock);
	return -EIO;

}

/**
 * wifi_nl80211_start_event_thread: Start the thread which processes
 *                                  the async netlink events
 * @ctx: wifi cfg80211 context
 *
 * return 1/0
 */
int wifi_nl80211_start_event_thread(wifi_cfg80211_context *ctx)
{
	if (pthread_create(&ctx->event_thread_handle, NULL,
				event_thread, ctx)) {
		return -EFAULT;
	}
	while(!ctx->event_thread_running);
	return 0;
}

/**
 * wifi_destroy_nl80211: destriy nl80211 socket
 * @ctx: wifi cfg80211 context
 *
 * return 1/0
 */
void wifi_destroy_nl80211(wifi_cfg80211_context *ctx)
{
	pthread_mutex_destroy(&ctx->cmd_sock_lock);
	nl_socket_free(ctx->cmd_sock);
	if (ctx->event_thread_running) {
		/*
		 * Make the thread exit
		 */
		ctx->event_thread_running = 0;
		pthread_cancel(ctx->event_thread_handle);
		pthread_join(ctx->event_thread_handle, NULL);
	}
	if (ctx->event_sock) {
		nl_socket_free(ctx->event_sock);
	}
}

 /**
 * execute_system_cmd: execute system command and return the output of the command
 *
 * @cmd: system command
 * @result: output of the command
 * @result_len: output length of the command
 *
 * return 0 on success; otherwise -1
 */
int execute_system_cmd(const char *cmd, char *result, int result_len) {
	if (cmd == NULL || result == NULL)
		return -1;

	FILE *fp = popen(cmd, "r");
	if (fp) {
		if (fgets(result, result_len, fp)) {
			pclose(fp);
			return 0;
		} else
			pclose(fp);
	}

	return -1;
}

 /**
 * get_vap_name_from_radio_name: find the vap name of the given radio name
 *
 * @radio_name: radio name
 * @vap_name: vap name
 *
 * return 0 on success; otherwise -1
 */
int get_vap_name_from_radio_name(const char *radio_name, char *vap_name) {
	char command[MAX_COMMAND_STR_LEN] = {0};
	char command_result[MAX_COMMAND_RESULT_LEN] = {0};

	memset(command, 0, MAX_COMMAND_STR_LEN);
	snprintf(command, MAX_COMMAND_STR_LEN, GET_VAP_IFACE_NAME_CMD, radio_name);
	if (!execute_system_cmd(command, command_result, MAX_COMMAND_RESULT_LEN)) {
		strlcpy(vap_name, command_result, IF_NAME_SIZE);
		vap_name[IF_NAME_SIZE - 1] = '\0';
		return 0;
	}

	return -1;
}

 /**
 * get_wlan_chip_type: get wlan chip type
 *
 * return wlan chip type
 */
enum wlan_chip_type get_wlan_chip_type() {
#ifdef SDX_MCC
	return WLAN_CHIP_TYPE_MCC;
#else
	return WLAN_CHIP_TYPE_WIN;
#endif /* SDX_MCC */
}
