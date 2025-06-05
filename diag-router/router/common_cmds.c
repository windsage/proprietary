/*
 * Copyright (c) 2016-2018, The Linux Foundation. All rights reserved.
 * Copyright (c) 2016, Linaro Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "diag.h"
#include "dm.h"
#include "hdlc.h"
#include "masks.h"
#include "peripheral.h"
#include "util.h"
#include "diag_mux.h"


#define DIAG_CMD_STATUS_SUCCESS					0
#define DIAG_CMD_STATUS_INVALID_EQUIPMENT_ID	1

#define DIAG_CMD_MS_LOG_SUBSYS_CMD			0x6E
#define DIAG_CMD_LOGGING_CONFIGURATION 	0x73
#define DIAG_CMD_OP_LOG_DISABLE				0
#define DIAG_CMD_OP_GET_LOG_RANGE			1
#define DIAG_CMD_OP_SET_LOG_MASK				3
#define DIAG_CMD_OP_GET_LOG_MASK				4

#define DIAG_CMD_MS_MSG_SUBSYS_CMD						0x6D
#define DIAG_CMD_EXTENDED_MESSAGE_CONFIGURATION 	0x7d
#define DIAG_CMD_OP_GET_SSID_RANGE						1
#define DIAG_CMD_OP_GET_BUILD_MASK						2
#define DIAG_CMD_OP_GET_MSG_MASK							3
#define DIAG_CMD_OP_SET_MSG_MASK							4
#define DIAG_CMD_OP_SET_ALL_MSG_MASK					5

#define DIAG_CMD_MSG_STATUS_UNSUCCESSFUL		0
#define DIAG_CMD_MSG_STATUS_SUCCESSFUL			1

#define DIAG_CMD_GET_MASK 							0x81
#define DIAG_CMD_EVENT_ERROR_CODE_OK			0
#define DIAG_CMD_EVENT_ERROR_CODE_FAIL			1

#define DIAG_CMD_SET_MASK	0x82

#define DIAG_CMD_MS_EVENT_SUBSYS_CMD	0x6F
#define DIAG_CMD_OP_GET_EVENT_MSK		1
#define DIAG_CMD_OP_SET_EVENT_MSK		2
#define DIAG_CMD_OP_EVENT_TOGGLE			3

#define DIAG_CMD_EVENT_REPORT_CONTROL	0x60

static int handle_logging_configuration(struct diag_client *client,
					const void *buf, size_t len, int pid)
{
	const struct diag_log_cmd_header {
		uint8_t cmd_code;
		uint8_t reserved[3];
		uint32_t operation;
	}__packed *request_header = NULL;
	/* suppress unused parameter warning */
	(void)client;

	if (!buf)
		return -EINVAL;
	request_header = (struct diag_log_cmd_header *) buf;
	if (len < sizeof(*request_header))
		return -EMSGSIZE;
	switch (request_header->operation) {
	case DIAG_CMD_OP_LOG_DISABLE: {
		struct {
			struct diag_log_cmd_header header;
			uint32_t status;
		} __packed resp;
		if (sizeof(*request_header) != len)
			return -EMSGSIZE;

		memcpy(&resp, request_header, sizeof(*request_header));
		diag_cmd_disable_log(pid, INVALID_INDEX);
		resp.status = DIAG_CMD_STATUS_SUCCESS;

		diagmem->mask_set_status = 1;
		peripheral_broadcast_log_mask(0, INVALID_INDEX, pid, 0);
		clients_broadcast_log_mask();
		diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
		break;
	}
	case DIAG_CMD_OP_GET_LOG_RANGE: {
		struct {
			struct diag_log_cmd_header header;
			uint32_t status;
			uint32_t ranges[MAX_EQUIP_ID];
		} __packed resp;
		if (sizeof(*request_header) != len)
			return -EMSGSIZE;

		memcpy(&resp, request_header, sizeof(*request_header));
		diag_cmd_get_log_range(resp.ranges, MAX_EQUIP_ID, INVALID_INDEX, 0);
		resp.status = DIAG_CMD_STATUS_SUCCESS;

		diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
		break;
	}
	case DIAG_CMD_OP_SET_LOG_MASK: {
		struct diag_log_cmd_mask *mask_to_set = (struct diag_log_cmd_mask*)((char*)buf + sizeof(struct diag_log_cmd_header));
		struct {
			struct diag_log_cmd_header header;
			uint32_t status;
			struct diag_log_cmd_mask mask_structure;
		} __packed *resp;
		uint32_t resp_size = sizeof(*resp);
		uint32_t num_items, num_items_bound, num_items_size;
		uint32_t mask_size, rsp_mask_size;
		num_items_bound = MIN(mask_to_set->num_items, MAX_ITEMS_ALLOWED);
		num_items_size = BITS_TO_BYTES(num_items_bound);
		mask_size = num_items_size;
		rsp_mask_size = num_items_size;

		diagmem->mask_set_status = 1;
		if (sizeof(*request_header) + mask_size + sizeof(*mask_to_set) != len)
			return -EMSGSIZE;

		resp_size += rsp_mask_size;
		resp = malloc(resp_size);
		if (!resp) {
			warn("Failed to allocate response packet\n");
			return -errno;
		}
		memcpy(resp, request_header, sizeof(*request_header));
		num_items = mask_to_set->num_items;
		diag_cmd_set_log_mask(mask_to_set->equip_id, &num_items, 
			mask_to_set->mask, &mask_size, pid, INVALID_INDEX);

		mask_to_set->num_items = num_items;
		resp->mask_structure.num_items = mask_to_set->num_items;
		resp->mask_structure.equip_id = mask_to_set->equip_id;
		memcpy(&resp->mask_structure.mask, mask_to_set->mask, rsp_mask_size); // num_items might have been capped!!!
		resp->status = DIAG_CMD_STATUS_SUCCESS;

		peripheral_broadcast_log_mask(resp->mask_structure.equip_id, INVALID_INDEX, pid, 0);
		clients_broadcast_log_mask();
		diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
		free(resp);
		resp = NULL;

		break;
	}
	case DIAG_CMD_OP_GET_LOG_MASK: {
		uint32_t *equip_id = (uint32_t *)((char*)buf + sizeof(struct diag_log_cmd_header));
		struct get_log_response_resp {
			struct diag_log_cmd_header header;
			uint32_t status;
			struct diag_log_cmd_mask mask_structure;
		} __packed *resp;
		uint32_t num_items = 0;
		uint8_t *mask;
		uint32_t mask_size = 0;
		uint32_t resp_size = sizeof(*resp);
		struct diag_md_session_t *info = NULL;
		struct diag_log_mask_t *log_item;

		info = diag_md_session_get_pid(pid);
		log_item = (!info) ? log_mask.ptr : info->log_mask->ptr;
		if (sizeof(*request_header) + sizeof(*equip_id) != len)
			return -EMSGSIZE;

		if (diag_cmd_get_log_mask(*equip_id, &num_items, &mask, &mask_size, log_item) == 0) {
			resp_size += mask_size;
			resp = malloc(resp_size);
			if (!resp) {
				warn("Failed to allocate get log mask response packet err:%d\n", errno);
				free(mask);
				mask = NULL;
				return -errno;
			}
			memcpy(resp, request_header, sizeof(*request_header));
			resp->mask_structure.equip_id = *equip_id;
			resp->mask_structure.num_items = num_items;
			if (mask != NULL) {
				memcpy(&resp->mask_structure.mask, mask, mask_size);
				free(mask);
				mask = NULL;
			}
			resp->status = DIAG_CMD_STATUS_SUCCESS;
		} else {
			resp = malloc(resp_size);
			if (!resp) {
				warn("Failed to allocate get log mask response packet for invalid equip id err %d\n", errno);
				return -errno;
			}
			memcpy(resp, request_header, sizeof(*request_header));
			resp->mask_structure.equip_id = *equip_id;
			resp->mask_structure.num_items = num_items;
			resp->status = DIAG_CMD_STATUS_INVALID_EQUIPMENT_ID;
		}

		diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
		free(resp);
		resp = NULL;
		break;
	}
	default:
		warn("Unrecognized operation %d!!!\n", request_header->operation);
		return -EINVAL;
	}

	return 0;
}

static int handle_extended_message_configuration(struct diag_client *client,
						 const void *buf, size_t len, int pid)
{
	const struct diag_msg_cmd_header {
		uint8_t cmd_code;
		uint8_t operation;
	}__packed *request_header = NULL;
	/* suppress unused parameter warning */
	(void)client;

	if (!buf)
		return -EINVAL;
	request_header = (struct diag_msg_cmd_header *) buf;
	if ( len < sizeof(*request_header))
		return -EMSGSIZE;
	switch (request_header->operation) {
	case DIAG_CMD_OP_GET_SSID_RANGE: {
		struct {
			struct diag_msg_cmd_header header;
			uint8_t status;
			uint8_t reserved;
			uint32_t range_cnt;
			struct diag_ssid_range_t ranges[];
		} __packed *resp;
		uint32_t resp_size = sizeof(*resp);
		uint32_t count = 0;
		struct diag_ssid_range_t *ranges = NULL;
		uint32_t ranges_size = 0;

		if (sizeof(*request_header) != len)
			return -EMSGSIZE;

		diag_cmd_get_ssid_range(&count, &ranges, INVALID_INDEX, pid);
		ranges_size = count * sizeof(*ranges);
		resp_size += ranges_size;
		resp = malloc(resp_size);
		if (!resp) {
			warn("Failed to allocate  get msg ssid range response packet err:%d\n", errno);
			free(ranges);
			ranges = NULL;
			return -errno;
		}
		memcpy(resp, request_header, sizeof(*request_header));
		resp->range_cnt = count;
		if (ranges != NULL) {
			memcpy(resp->ranges, ranges, ranges_size);
			free(ranges);
			ranges = NULL;
		}
		resp->status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;

		diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
		free(resp);
		resp = NULL;
		break;
	}
	case DIAG_CMD_OP_GET_BUILD_MASK: {
		struct diag_ssid_range_t range;
		struct {
			uint8_t cmd;
			uint8_t subcmd;
			struct diag_ssid_range_t range;
			uint8_t status;
			uint8_t reserved;
			uint32_t masks[];
		} __packed *resp;
		uint32_t resp_size = sizeof(*resp);
		uint32_t *masks;
		uint32_t masks_size;

		if (sizeof(*request_header) + sizeof(range) != len)
			return -EMSGSIZE;

		memcpy(&range, (char*)buf + sizeof(struct diag_msg_cmd_header), sizeof(range));

		if (diag_cmd_get_build_mask(&range, &masks, INVALID_INDEX, pid) == 0) {
			masks_size = MSG_RANGE_TO_SIZE(range);
			resp_size += masks_size;

			resp = malloc(resp_size);
			if (!resp) {
				free(masks);
				masks = NULL;
				return -ENOMEM;
			}
			memset(resp, 0, resp_size);
			resp->cmd = DIAG_CMD_EXTENDED_MESSAGE_CONFIGURATION;
			resp->subcmd = DIAG_CMD_OP_GET_BUILD_MASK;

			resp->range.ssid_first = range.ssid_first;
			resp->range.ssid_last = range.ssid_last;

			resp->status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;
			if (masks)
				memcpy(resp->masks, masks, masks_size);

			free(masks);
			masks = NULL;
		} else {
			resp = malloc(resp_size);
			if (!resp)
				return -ENOMEM;
			memset(resp, 0, resp_size);
			resp->cmd = DIAG_CMD_EXTENDED_MESSAGE_CONFIGURATION;
			resp->subcmd = DIAG_CMD_OP_GET_BUILD_MASK;
			resp->status = DIAG_CMD_MSG_STATUS_UNSUCCESSFUL;
		}

		diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
		free(resp);
		resp = NULL;
		break;
	}
	case DIAG_CMD_OP_GET_MSG_MASK: {
		struct diag_ssid_range_t range;
		struct {
			struct diag_msg_cmd_header header;
			uint8_t status;
			uint8_t rsvd;
			uint32_t rt_masks[];
		} __packed *resp;
		uint32_t resp_size = sizeof(*resp);
		uint32_t *masks = NULL;
		uint32_t masks_size = 0;
		struct diag_md_session_t *info = NULL;
		struct diag_msg_mask_t *msg_item;

		info = diag_md_session_get_pid(pid);
		msg_item = (!info) ? msg_mask.ptr : info->msg_mask->ptr;

		if (sizeof(*request_header) + sizeof(range) != len)
			return -EMSGSIZE;

		memcpy(&range, (char*)buf + sizeof(struct diag_msg_cmd_header), sizeof(range));

		if (diag_cmd_get_msg_mask(&range, INVALID_INDEX, &masks, pid) == 0) {
			masks_size = MSG_RANGE_TO_SIZE(range);
			resp_size += masks_size;

			resp = malloc(resp_size);
			if(!resp) {
				free(masks);
				masks = NULL;
				return -ENOMEM;
			}
			memset(resp, 0, resp_size);

			memcpy(resp, request_header, sizeof(*request_header));
			if (masks)
				memcpy(resp->rt_masks, masks, masks_size);
			resp->status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;
		} else {

			resp = malloc(resp_size);
			if(!resp)
				return -ENOMEM;

			memset(resp, 0, resp_size);

			memcpy(resp, request_header, sizeof(*request_header));
			resp->status = DIAG_CMD_MSG_STATUS_UNSUCCESSFUL;
		}

		diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
		free(masks);
		masks = NULL;
		free(resp);
		resp = NULL;
		break;
	}
	case DIAG_CMD_OP_SET_MSG_MASK: {
		const struct {
			struct diag_msg_cmd_header header;
			struct diag_ssid_range_t range;
			uint16_t rsvd;
			uint32_t masks[];
		} __packed *req = buf;
		struct {
			struct diag_msg_cmd_header header;
			struct diag_ssid_range_t range;
			uint8_t status;
			uint8_t rsvd;
			uint32_t rt_masks[0];
		} __packed *resp;
		uint32_t resp_size = sizeof(*resp);
		uint16_t masks_size = MSG_RANGE_TO_SIZE(req->range);

		diagmem->mask_set_status = 1;
		if (sizeof(*req) + masks_size != len)
			return -EMSGSIZE;

		if (diag_cmd_set_msg_mask(req->range, req->masks, INVALID_INDEX, pid) == 0) {
			resp_size += masks_size;
			resp = malloc(resp_size);
			if (!resp) {
				warn("Failed to mallocte  set msg mask response packet err:%d\n", errno);
				return -errno;
			}
			resp->header = req->header;
			resp->range = req->range;
			resp->status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;
			resp->rsvd = req->rsvd;
			memcpy(resp->rt_masks, req->masks, masks_size);

			peripheral_broadcast_msg_mask(&resp->range, INVALID_INDEX, pid, req->rsvd);
			clients_broadcast_msg_mask();
		} else {
			resp = malloc(resp_size);
			if (!resp) {
				warn("Failed to allocate set msg mask response packet for unknown ssid range\n");
				return -errno;
			}
			resp->header = req->header;
			resp->range = req->range;
			resp->rsvd = req->rsvd;
			resp->status = DIAG_CMD_MSG_STATUS_UNSUCCESSFUL;
		}

		diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
		free(resp);
		resp = NULL;
		break;
	}
	case DIAG_CMD_OP_SET_ALL_MSG_MASK: {
		const struct {
			struct diag_msg_cmd_header header;
			uint16_t rsvd;
			uint32_t mask;
		} __packed *req = buf;
		struct {
			struct diag_msg_cmd_header header;
			uint8_t status;
			uint8_t rsvd;
			uint32_t rt_mask;
		} __packed resp;
		struct diag_ssid_range_t range;
		int i;
		diagmem->mask_set_status = 1;

		if (sizeof(*req) != len)
			return -EMSGSIZE;

		diag_cmd_set_all_msg_mask(req->mask,INVALID_INDEX, pid);
		resp.header = req->header;
		resp.rsvd = req->rsvd;
		resp.rt_mask = req->mask;
		resp.status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;
		for (i = 0; i < MSG_MASK_TBL_CNT; i++) {
			range.ssid_first = ssid_first_arr[i];
			range.ssid_last = ssid_last_arr[i];
			peripheral_broadcast_msg_mask(&range, INVALID_INDEX, pid, req->rsvd);
		}
		clients_broadcast_msg_mask();
		diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
		break;
	}
	default:
		warnx("Unknown extended message configuration: %d\n", request_header->operation);
		return -EINVAL;
	}

	return 0;
}

static int handle_event_get_mask(struct diag_client *client, const void *buf,
				 size_t len, int pid)
{
	const struct diag_event_cmd_header {
		uint8_t cmd_code;
		uint8_t pad;
		uint16_t reserved;
	} __packed *req = NULL;
	struct {
		uint8_t cmd_code;
		uint8_t error_code;
		uint16_t reserved;
		uint16_t num_bits;
		uint8_t mask[0];
	} __packed *resp;
	uint32_t resp_size = sizeof(*resp);
	uint16_t num_bits = event_max_num_bits;
	uint16_t mask_size = 0;
	uint8_t *mask = NULL;
	struct diag_md_session_t *info = NULL;
	struct diag_event_mask_t *event_item = NULL;
	/* suppress unused parameter warning */
	(void)client;

	info = diag_md_session_get_pid(pid);

	if (!buf)
		return -EINVAL;
	req = (struct diag_event_cmd_header *) buf;
	if (sizeof(*req) != len)
		return -EMSGSIZE;
	event_item = (info) ? info->event_mask->ptr : event_mask.ptr;
	if (diag_cmd_get_event_mask(num_bits, &mask, event_item) == 0) {
		mask_size = BITS_TO_BYTES(num_bits);
		resp_size += mask_size;
		resp = malloc(resp_size);
		if (!resp) {
			warn("Failed to allocate get event mask response packet err:%d\n", errno);
			return -errno;
		}
		resp->cmd_code = req->cmd_code;
		resp->reserved = req->reserved;
		resp->num_bits = num_bits;
		if (mask != NULL) {
			memcpy(&resp->mask, mask, mask_size);
			free(mask);
			mask = NULL;
		}
		resp->error_code = DIAG_CMD_EVENT_ERROR_CODE_OK;
	} else {
		resp = malloc(resp_size);
		if (!resp) {
			warn("Failed to allocate get event mask response packet in error case err:%d\n", errno);
			return -errno;
		}
		resp->cmd_code = req->cmd_code;
		resp->reserved = req->reserved;
		resp->num_bits = 0;
		resp->error_code = DIAG_CMD_EVENT_ERROR_CODE_FAIL;
	}

	diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
	free(resp);
	resp = NULL;

	return 0;
}

static int handle_event_set_mask(struct diag_client *client,
				 const void *buf, size_t len, int pid)
{
	const struct diag_event_cmd_header {
		uint8_t cmd_code;
		uint8_t pad;
		uint16_t reserved;
		uint16_t num_bits;
		uint8_t mask[0];
	} __packed *req = NULL;
	struct {
		uint8_t cmd_code;
		uint8_t error_code;
		uint16_t reserved;
		uint16_t num_bits;
		uint8_t mask[0];
	} __packed *resp;
	uint32_t resp_size = sizeof(*resp);
	uint16_t mask_size;
	/* suppress unused parameter warning */
	(void)client;
	(void)len;

	if (!buf)
		return -EINVAL;
	req = (struct diag_event_cmd_header *) buf;
	mask_size = BITS_TO_BYTES(req->num_bits);

	if (diag_cmd_update_event_mask(req->num_bits, req->mask, INVALID_INDEX, pid) == 0) {
		resp_size += mask_size;
		resp = malloc(resp_size);
		if (!resp) {
			warn("Failed to allocate set event mask response packet err:%d\n", errno);
			return -errno;
		}
		resp->cmd_code = req->cmd_code;
		resp->reserved = req->reserved;
		resp->num_bits = req->num_bits;
		memcpy(resp->mask, req->mask, mask_size);
		resp->error_code = DIAG_CMD_EVENT_ERROR_CODE_OK;

		diagmem->mask_set_status = 1;
		peripheral_broadcast_event_mask(INVALID_INDEX, 0, pid);
		clients_broadcast_event_mask();
	} else {
		resp = malloc(resp_size);
		if (!resp) {
			warn("Failed to allocate set event mask response packet in failure case  err:%d\n", errno);
			return -errno;
		}
		resp->cmd_code = req->cmd_code;
		resp->reserved = req->reserved;
		resp->num_bits = 0;
		resp->error_code = DIAG_CMD_EVENT_ERROR_CODE_FAIL;
	}

	diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
	free(resp);
	resp = NULL;
	return 0;
}

static int handle_event_report_control(struct diag_client *client,
				const void *buf, size_t len, int pid)
{
	const struct diag_event_cmd_header {
		uint8_t cmd_code;
		uint8_t operation_switch;
	} __packed *req = NULL;
	struct {
		uint8_t cmd_code;
		uint16_t length;
	} __packed pkt;
	/* suppress unused parameter warning */
	(void)client;

	if (!buf)
		return -EINVAL;
	req = (struct diag_event_cmd_header *) buf;

	if (sizeof(*req) != len)
		return -EMSGSIZE;

	diagmem->mask_set_status = 1;
	diag_cmd_toggle_events(!!req->operation_switch, pid, INVALID_INDEX);
	peripheral_broadcast_event_mask(INVALID_INDEX, 0, pid);
	clients_broadcast_event_mask();

	pkt.cmd_code = DIAG_CMD_EVENT_REPORT_CONTROL;
	pkt.length = 0;

	diag_apps_rsp_send(pid, (unsigned char *)&pkt, sizeof(pkt));

	return 0;
}

static int handle_ms_logging_configuration(
	struct diag_client *client,
	const void *buf, size_t len, int pid)
{

	int subid_index = INVALID_INDEX;

	struct diag_msg_ssid_query_sub_t *req_msg = (struct diag_msg_ssid_query_sub_t*)buf;

	if(client && buf){
		ALOGD("diag: %s: client: %s, sub_cmd: %d, sub_id: %d, valid: %d, pid: %d", __func__,
			client->name, req_msg->sub_cmd, req_msg->sub_id, req_msg->id_valid, pid);
	} else {
		ALOGE("diag: %s: invalid client or buff is NULL", __func__);
	}


	if (!buf)
		return -EINVAL;

	if (len < sizeof(struct diag_msg_ssid_query_sub_t))
		return -EMSGSIZE;

	if (req_msg->id_valid)
		subid_index = diag_check_subid_mask_index(req_msg->sub_id, pid);

	if (req_msg->id_valid  && (subid_index < 0)){
		ALOGE("%s: return req_msg->id_valid   %d  subid_index =%d %d sub_id \n",
			__func__, req_msg->id_valid, subid_index, req_msg->sub_id);
		return -ENOMEM;
	}

	switch (req_msg->sub_cmd) {
		case DIAG_CMD_OP_LOG_DISABLE:
		{
			diag_cmd_op_log_disable_sub_req_t *req = (diag_cmd_op_log_disable_sub_req_t *)buf;
			diag_cmd_op_log_disable_sub_rsp_t resp = {0};

			if (sizeof(diag_cmd_op_log_disable_sub_req_t) > len)
			{
				ALOGE("%s: length is less than size of req_sub struct, len = %zu, cmd = %d \n", __func__, len, req_msg->sub_cmd);
				return -EMSGSIZE;
			}

			diagmem->mask_set_status = 1;
			memcpy(&resp, req, sizeof(diag_cmd_op_log_disable_sub_rsp_t));

			diag_cmd_disable_log(pid ,req->sub_id);

			resp.status = DIAG_CMD_STATUS_SUCCESS;
			resp.operation_code = DIAG_CMD_OP_LOG_DISABLE;

			peripheral_broadcast_log_mask(0, req->sub_id, pid, req->preset_id);
			if(req_msg->sub_id == INVALID_INDEX){
				clients_broadcast_log_mask(); /*for apps client we dont support multi-sim feature
														so no need to send mask update to apps clients*/
			}
			diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
			break;

		}

		case DIAG_CMD_OP_GET_LOG_RANGE:
		{
			diag_cmd_op_get_log_range_sub_req_t *req = (diag_cmd_op_get_log_range_sub_req_t *)buf;
			diag_cmd_op_get_log_range_sub_rsp_t rsp_sub = {0};

			if (sizeof(diag_cmd_op_get_log_range_sub_req_t) >  len){
				ALOGE("%s: length is less than size of req_sub struct, len = %zu, cmd = %d \n", __func__, len, req_msg->sub_cmd);
				return -EMSGSIZE;
			}

			memcpy(&rsp_sub, req, sizeof(diag_cmd_op_get_log_range_sub_req_t));
			diag_cmd_get_log_range(rsp_sub.ranges, MAX_EQUIP_ID, rsp_sub.sub_id, pid);

			rsp_sub.operation_code = DIAG_CMD_OP_GET_LOG_RANGE;
			rsp_sub.preset_id = 0;
			rsp_sub.status = DIAG_CMD_STATUS_SUCCESS;

			diag_apps_rsp_send(pid, (unsigned char *)&rsp_sub, sizeof(rsp_sub));
			break;
		}

		case DIAG_CMD_OP_SET_LOG_MASK:
		{
			diag_cmd_op_set_log_mask_sub_req_t *req_sub = (diag_cmd_op_set_log_mask_sub_req_t*)buf;
			diag_cmd_op_set_log_mask_sub_rsp_t *rsp_sub = NULL;

			uint32_t mask_size = 0;
			uint32_t resp_size = sizeof(*rsp_sub);
			uint32_t status = LOG_STATUS_SUCCESS;
			uint32_t num_items = 0;

			diagmem->mask_set_status = 1;

			if (len < sizeof(diag_cmd_op_set_log_mask_sub_req_t)) {
				ALOGE("%s: length is less than size of req_sub struct, len = %zu, cmd = %d \n", __func__, len, req_msg->sub_cmd);
				return -EMSGSIZE;
			}

			if (req_sub->equip_id >= MAX_EQUIP_ID) {
				ALOGE("diag: In %s, Invalid logging mask request, equip_id: %d\n",
						 __func__, req_sub->equip_id);
				status = LOG_STATUS_INVALID;
			}

			if (req_sub->num_items == 0) {
				ALOGE("diag: In %s, Invalid number of items in log mask request, equip_id: %d\n",
					 	  __func__, req_sub->equip_id);
				status = LOG_STATUS_INVALID;
			}

			mask_size = BITS_TO_BYTES(req_sub->num_items);
			num_items = req_sub->num_items;

			if (diag_cmd_set_log_mask(req_sub->equip_id,
				&num_items,
				req_sub->mask, &mask_size,
				pid, req_sub->sub_id))
			{
				status = LOG_STATUS_INVALID;
			}

			req_sub->num_items = num_items;

			if (sizeof(*req_sub) + mask_size != len){

				ALOGE("diag: In %s, length is less returning, equip_id: %d 0x%lx exp =  0x%lx \n",
					__func__, req_sub->equip_id, len, sizeof(*req_sub) + mask_size );

				return -EMSGSIZE;
			}
			resp_size += mask_size;
			rsp_sub = malloc(resp_size);
			if (!rsp_sub) {
				warn("Failed to allocate response packet\n");
				return -errno;
			}
			memcpy(rsp_sub, req_sub, sizeof(*req_sub));

			memcpy(&rsp_sub->mask, req_sub->mask, mask_size); // num_items might have been capped!!!

			rsp_sub->operation_code = DIAG_CMD_OP_SET_LOG_MASK;
			rsp_sub->status = status;

			peripheral_broadcast_log_mask(rsp_sub->equip_id,
				rsp_sub->sub_id, pid, rsp_sub->preset_id);

			if(rsp_sub->sub_id!=INVALID_INDEX)
				clients_broadcast_log_mask();

			diag_apps_rsp_send(pid, (unsigned char *)rsp_sub, resp_size);
			free(rsp_sub);
			rsp_sub = NULL;
			break;
		}
		case DIAG_CMD_OP_GET_LOG_MASK:
		{
			diag_cmd_op_get_log_mask_sub_req_t *req_sub = (diag_cmd_op_get_log_mask_sub_req_t *)buf;
			diag_cmd_op_get_log_mask_sub_rsp_t *rsp_sub = NULL;

			struct diag_log_mask_t *log_item;
			uint32_t equip_id = 0;
			uint32_t num_items = 0;
			uint8_t *mask;
			uint32_t mask_size = 0;
			uint32_t resp_size = sizeof(*rsp_sub);

			if (len < sizeof(*req_sub)){
				ALOGE("%s: length is less than size of req_sub struct, len = %zu, cmd = %d \n", __func__, len, req_msg->sub_cmd);
				return -EMSGSIZE;
			}

			equip_id = req_sub->equip_id;

			log_item = (struct diag_log_mask_t *)diag_get_mask(req_sub->sub_id, &log_mask, pid);

			if (!log_item || !log_item->ptr) {
				ALOGE("diag: In %s, Invalid mask\n",
					__func__);
				return -EINVAL;
			}

			if (diag_cmd_get_log_mask(equip_id, &num_items, &mask, &mask_size, log_item) == 0) {
				resp_size += mask_size;
				rsp_sub = malloc(resp_size);
				if (!rsp_sub) {
					warn("Failed to allocate get log mask response packet err:%d\n", errno);
					free(mask);
					mask = NULL;
					return -ENOMEM;
				}
				memcpy(rsp_sub, req_sub, sizeof(*req_sub));
				rsp_sub->equip_id = equip_id;
				rsp_sub->num_items = num_items;
				if (mask != NULL) {
					memcpy(&rsp_sub->mask, mask, mask_size);
					free(mask);
					mask = NULL;
				}
				rsp_sub->status = LOG_STATUS_SUCCESS;
				rsp_sub->operation_code = DIAG_CMD_OP_GET_LOG_MASK;

			} else {
				rsp_sub = malloc(resp_size);
				if (!rsp_sub) {
					warn("Failed to allocate get log mask response packet for invalid equip id err %d\n", errno);
					return -ENOMEM;
				}
				memcpy(rsp_sub, req_sub, sizeof(*req_sub));
				rsp_sub->equip_id = equip_id;
				rsp_sub->num_items = num_items;
				rsp_sub->operation_code = DIAG_CMD_OP_GET_LOG_MASK;
				rsp_sub->status = LOG_STATUS_FAIL;
			}

			diag_apps_rsp_send(pid, (unsigned char *)rsp_sub, resp_size);
			free(rsp_sub);
			rsp_sub = NULL;
			break;
		}
	}

	return 0;
}


static int handle_ms_extended_message_configuration(
	struct diag_client *client,
	const void *buf, size_t len, int pid)
{
	int subid_index = INVALID_INDEX;
	uint32_t ms_mask_cmd_size = 0;
	struct diag_msg_ssid_query_sub_t *req_msg = (struct diag_msg_ssid_query_sub_t*)buf;

	if (!buf)
		return -EINVAL;

	if(client && buf){
		ALOGD("diag: %s: client: %s, sub_cmd: %d, sub_id: %d, valid: %d, pid: %d", __func__,
			client->name, req_msg->sub_cmd, req_msg->sub_id, req_msg->id_valid, pid);
	} else {
		ALOGE("diag: %s: invalid client or buff is NULL ", __func__);
	}

	ms_mask_cmd_size = sizeof(struct diag_msg_ssid_query_sub_t);

	if (len < ms_mask_cmd_size){
		ALOGE("%s: return len  %zu \n", __func__, len);
		return -EMSGSIZE;;
	}

	if (req_msg->id_valid)
		subid_index = diag_check_subid_mask_index(req_msg->sub_id, pid);

	if (req_msg->id_valid  && (subid_index < 0)){
		ALOGE("%s: return req_msg->id_valid   %d  subid_index =%d \n", __func__, req_msg->id_valid, subid_index);
		return 0;
	}

	switch (req_msg->sub_cmd) {
		case DIAG_CMD_OP_GET_SSID_RANGE:
		{
			diag_cmd_op_get_ssid_range_sub_req_t *req_msg = (diag_cmd_op_get_ssid_range_sub_req_t *)buf;
			diag_cmd_op_get_ssid_range_sub_rsp_t *rsp_ms = NULL;

			struct diag_ssid_range_t *ranges = NULL;
			uint32_t resp_size = sizeof(diag_cmd_op_get_ssid_range_sub_rsp_t);
			uint32_t count = 0;
			uint32_t range_size = sizeof(struct diag_ssid_range_t);
			uint32_t ranges_size = 0;

			diag_cmd_get_ssid_range(&count, &ranges, req_msg->sub_id, pid);

			ranges_size = count * range_size;
			resp_size += ranges_size;
			rsp_ms = malloc(resp_size);
			if (!rsp_ms) {
				ALOGE("Failed to allocate  get msg ssid range response packet err:%d\n", errno);
				if(ranges != NULL) {
					free(ranges);
					ranges = NULL;
				}
				return 0;
			}
			memcpy(rsp_ms, req_msg, sizeof(diag_cmd_op_get_ssid_range_sub_req_t));

			rsp_ms->status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;
			rsp_ms->reserved = 0;
			rsp_ms->count = count;

			if (ranges != NULL) {
				memcpy(rsp_ms->ranges, ranges, ranges_size);
				free(ranges);
				ranges = NULL;
			}
			diag_apps_rsp_send(pid, (unsigned char *)rsp_ms, resp_size);

			free(rsp_ms);
			rsp_ms = NULL;
		}
		break;

		case DIAG_CMD_OP_GET_BUILD_MASK:
		{
			struct diag_ssid_range_t range;
			diag_cmd_op_get_build_mask_sub_req_t *req_sub = (diag_cmd_op_get_build_mask_sub_req_t*)buf;
			diag_cmd_op_get_build_mask_sub_rsp_t *resp = NULL;
			uint32_t resp_size = sizeof(diag_cmd_op_get_build_mask_sub_rsp_t);
			uint32_t *masks = NULL;
			uint16_t masks_size;

			if (len < sizeof(diag_cmd_op_get_build_mask_sub_req_t)){
				ALOGE("%s: return len  %zu \n", __func__, len);
				return -EMSGSIZE;
			}

			range.ssid_first = req_sub->ssid_first;
			range.ssid_last = req_sub->ssid_last;

			if (diag_cmd_get_build_mask(&range, &masks, req_sub->sub_id, pid) == 0)
			{

				masks_size = MSG_RANGE_TO_SIZE(range);
				resp_size += masks_size;

				resp = malloc(resp_size);
				if (!resp) {
					free(masks);
					masks = NULL;
					return -ENOMEM;
				}

				memset(resp, 0, resp_size);

				resp->header.cmd_code = req_sub->header.cmd_code;
				resp->sub_cmd = DIAG_CMD_OP_GET_BUILD_MASK;
				resp->sub_id = req_sub->sub_id;
				resp->id_valid = req_sub->id_valid;
				resp->header.subsys_id = req_sub->header.subsys_id;
				resp->header.subsys_cmd_code = req_sub->header.subsys_cmd_code;
				resp->version = req_sub->version;

				resp->ssid_first = range.ssid_first;
				resp->ssid_last = range.ssid_last;

				resp->status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;
				if(masks){
					memcpy(resp->masks, masks, masks_size);
					free(masks);
					masks = NULL;
				}
			} else {
				resp = malloc(resp_size);
				if (!resp)
					return -ENOMEM;
				memset(resp, 0, resp_size);
				resp->header.cmd_code = DIAG_CMD_EXTENDED_MESSAGE_CONFIGURATION;
				resp->sub_cmd = DIAG_CMD_OP_GET_BUILD_MASK;
				resp->status = DIAG_CMD_MSG_STATUS_UNSUCCESSFUL;
			}
			diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
			free(resp);
			resp = NULL;
		}
		break;
		case DIAG_CMD_OP_GET_MSG_MASK:
		{
			diag_cmd_op_get_msg_mask_sub_req_t *req_sub = (diag_cmd_op_get_msg_mask_sub_req_t *)buf;
			diag_cmd_op_get_msg_mask_sub_rsp_t *resp;

			struct diag_ssid_range_t range;
			uint32_t resp_size = sizeof(*resp);
			uint32_t *masks = NULL;
			uint16_t masks_size = 0;

			if (len < sizeof(diag_cmd_op_get_msg_mask_sub_req_t)) {
				ALOGE("%s: length is less than size of req_sub struct, len = %zu, cmd = %d \n", __func__, len, req_msg->sub_cmd);
				return -EMSGSIZE;
			}
			range.ssid_first = req_sub->ssid_first;
			range.ssid_last = req_sub->ssid_last;

			if (diag_cmd_get_msg_mask(&range, req_sub->sub_id, &masks, pid) == 0)
			{

				masks_size = MSG_RANGE_TO_SIZE(range);
				resp_size += masks_size;

				resp = malloc(resp_size);
				if (!resp) {
					free(masks);
					masks = NULL;
					return -ENOMEM;
				}

				memset(resp, 0, resp_size);

				memcpy(resp, req_sub, sizeof(diag_cmd_op_get_msg_mask_sub_rsp_t));

				if(masks){
					memcpy(resp->rt_masks, masks, masks_size);
					free(masks);
					masks = NULL;
				}

				resp->status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;
			} else {
				resp = malloc(resp_size);
				if (!resp)
					return -ENOMEM;

				memset(resp, 0, resp_size);

				memcpy(resp, req_sub, sizeof(diag_cmd_op_get_msg_mask_sub_rsp_t));
				resp->status = DIAG_CMD_MSG_STATUS_UNSUCCESSFUL;
			}

			diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
			free(resp);
			resp = NULL;
			break;
		}

		case DIAG_CMD_OP_SET_MSG_MASK:
		{

			diag_cmd_op_set_msg_mask_sub_req_t *req_cmd = (diag_cmd_op_set_msg_mask_sub_req_t*)buf;
			diag_cmd_op_set_msg_mask_sub_rsp_t *resp = NULL;

			uint32_t resp_size = sizeof(diag_cmd_op_set_msg_mask_sub_rsp_t);
			uint16_t masks_size = 0;
			struct diag_ssid_range_t range;

			diagmem->mask_set_status = 1;

			if (sizeof(diag_cmd_op_set_msg_mask_sub_req_t) > len)
			{
				ALOGE("%s: length is less than size of req_sub struct, len = %zu, cmd = %d \n", __func__, len, req_msg->sub_cmd);
				return -EMSGSIZE;
			}
			range.ssid_first = req_cmd->ssid_first;
			range.ssid_last = req_cmd->ssid_last;

			masks_size = MSG_RANGE_TO_SIZE(range);

			if (diag_cmd_set_msg_mask(range, req_cmd->masks, req_cmd->sub_id, pid) == 0)
			{
				resp_size += masks_size;

				resp = malloc(resp_size);
				if (!resp) {
					ALOGE("Failed to allocate  set msg mask response packet err:%d\n", errno);
					return -ENOMEM;
				}

				memcpy(resp, req_cmd, sizeof(diag_cmd_op_set_msg_mask_sub_rsp_t));

				resp->status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;

				memcpy(resp->rt_masks, req_cmd->masks, masks_size);

				peripheral_broadcast_msg_mask(&range, req_cmd->sub_id,
						pid, req_cmd->preset_id);

				if(req_msg->sub_id == INVALID_INDEX)
					clients_broadcast_msg_mask();

			} else {
				resp = malloc(resp_size);
				if (!resp) {
					warn("Failed to allocate set msg mask response packet for unknown ssid range\n");
					return-ENOMEM;
				}
				memcpy(resp, req_cmd, sizeof(diag_cmd_op_set_msg_mask_sub_req_t));

				resp->status = DIAG_CMD_MSG_STATUS_UNSUCCESSFUL;
			}

			diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
			free(resp);
			resp = NULL;
			break;
		}
		case DIAG_CMD_OP_SET_ALL_MSG_MASK:
		{
			diag_cmd_op_set_all_msg_mask_sub_req_t*req_sub = (diag_cmd_op_set_all_msg_mask_sub_req_t*)buf;
			diag_cmd_op_set_all_msg_mask_sub_rsp_t rsp_sub = {0};
			struct diag_ssid_range_t range;
			int i;

			diagmem->mask_set_status = 1;

			if (sizeof(diag_cmd_op_set_all_msg_mask_sub_req_t) >  len){
				ALOGE("%s: length is less than size of req_sub struct, len = %zu, cmd = %d \n", __func__, len, req_msg->sub_cmd);
				return -EMSGSIZE;
			}

			diag_cmd_set_all_msg_mask(req_sub->rt_mask, req_sub->sub_id, pid);

			memcpy(&rsp_sub, req_sub, sizeof(rsp_sub));

			rsp_sub.status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;

			for (i = 0; i < MSG_MASK_TBL_CNT; i++) {
				range.ssid_first = ssid_first_arr[i];
				range.ssid_last = ssid_last_arr[i];
				peripheral_broadcast_msg_mask(&range, req_sub->sub_id, pid, req_sub->preset_id);
			}

			if(req_msg->sub_id == INVALID_INDEX){
				clients_broadcast_msg_mask();
			}
			diag_apps_rsp_send(pid, (unsigned char *)&rsp_sub, sizeof(rsp_sub));
			break;
		}

	}

	return 0;
}

static int handle_ms_event_report_control(
	struct diag_client *client,
	const void *buf, size_t len, int pid)
{
	int subid_index = INVALID_INDEX;
	uint32_t ms_mask_cmd_size = 0;
	struct diag_msg_ssid_query_sub_t *req_msg = (struct diag_msg_ssid_query_sub_t*)buf;

	if(!buf)	{
		return-EINVAL;
	}

	if(client && buf){
		ALOGD("diag: %s: client: %s, sub_cmd: %d, sub_id: %d, valid: %d, pid: %d", __func__,
			client->name, req_msg->sub_cmd, req_msg->sub_id, req_msg->id_valid, pid);
	}
	else {
		ALOGE("diag: %s: invalid client or buff is NULL ", __func__);
	}


	ms_mask_cmd_size = sizeof(struct diag_msg_ssid_query_sub_t);

	if (len < ms_mask_cmd_size){
		ALOGE("%s: return len  %zu \n", __func__, len);
		return -EMSGSIZE;;
	}

	if (req_msg->id_valid)
		subid_index = diag_check_subid_mask_index(req_msg->sub_id, pid);

	if (req_msg->id_valid  && (subid_index < 0))
		return 0;

	switch (req_msg->sub_cmd) {
		case DIAG_CMD_OP_GET_EVENT_MSK:
		{
			diag_cmd_op_get_event_mask_sub_req_t *req = (diag_cmd_op_get_event_mask_sub_req_t*)buf;
			diag_cmd_op_get_event_mask_sub_rsp_t *rsp_sub = NULL;

			uint32_t resp_size = sizeof(*rsp_sub);
			uint16_t num_bits = event_max_num_bits;
			uint16_t mask_size = 0;
			uint8_t *mask = NULL;
			void *event_item = NULL;

			if (len < sizeof(diag_cmd_op_get_event_mask_sub_req_t)){
				ALOGE("%s: length is less than size of req_sub struct, len = %zu, cmd = %d \n", __func__, len, req_msg->sub_cmd);
				return -EINVAL;
			}
			event_item = diag_get_mask(req->sub_id, &event_mask, pid);

			if (!event_item ) {
				ALOGE("diag: In %s, Invalid mask\n",
				__func__);
				return -errno;
			}
			if (diag_cmd_get_event_mask(num_bits, &mask, event_item) == 0) {

				mask_size = BITS_TO_BYTES(num_bits);
				resp_size += mask_size;
				rsp_sub = malloc(resp_size);
				if (!rsp_sub) {
					warn("Failed to allocate get event mask response packet err:%d\n", errno);
					free(mask);
					mask = NULL;
					return -errno;
				}
				memcpy(rsp_sub, req, sizeof(*rsp_sub));

				rsp_sub->num_bits = num_bits;
				rsp_sub->status = DIAG_CMD_MSG_STATUS_SUCCESSFUL;

				if (mask != NULL) {
					memcpy(&rsp_sub->mask, mask, mask_size);
					free(mask);
					mask = NULL;
				}

			} else {
				rsp_sub = malloc(resp_size);
				if (!rsp_sub) {
					warn("Failed to allocate get event mask response packet in error case err:%d\n", errno);
					return -errno;
				}

				memcpy(rsp_sub, req, sizeof(*rsp_sub));
				rsp_sub->num_bits = 0;
				rsp_sub->status = DIAG_CMD_MSG_STATUS_UNSUCCESSFUL;
			}

			diag_apps_rsp_send(pid, (unsigned char *)rsp_sub, resp_size);
			free(rsp_sub);
			rsp_sub = NULL;
		}
		break;
		case DIAG_CMD_OP_SET_EVENT_MSK:
		{
			diag_cmd_op_set_event_mask_sub_req_t *req_sub = (diag_cmd_op_set_event_mask_sub_req_t *)buf;
			diag_cmd_op_set_event_mask_sub_rsp_t *rsp_sub = NULL;
			int resp_size = sizeof(*rsp_sub), mask_size = 0;

			diagmem->mask_set_status = 1;

			if (len < sizeof(diag_cmd_op_set_event_mask_sub_req_t))
			{
				ALOGE("%s: length is less than size of req_sub struct, len = %zu, cmd = %d \n", __func__, len, req_msg->sub_cmd);
				return -EMSGSIZE;
			}

			mask_size = BITS_TO_BYTES(req_sub->num_bits);

			if ((sizeof(*req_sub) + mask_size) > len){
				ALOGE("%s:Error len = %zu , expected len = %zu\n", __func__, len, sizeof(*req_sub) + mask_size);
				return -EMSGSIZE;
			}

			if (diag_cmd_update_event_mask(req_sub->num_bits, req_sub->mask,
					req_sub->sub_id, pid) == 0)
			{
				resp_size += mask_size;
				rsp_sub = malloc(resp_size);
				if (!rsp_sub) {
					warn("Failed to allocate set event mask response packet err:%d\n", errno);
					return -errno;
				}
				memcpy(rsp_sub, req_sub, sizeof(*req_sub));
				rsp_sub->status = DIAG_CMD_EVENT_ERROR_CODE_OK;

				memcpy(rsp_sub->mask, req_sub->mask, mask_size);

				peripheral_broadcast_event_mask(req_sub->sub_id,
														req_sub->preset_id, pid);

				if(req_sub->sub_id == INVALID_INDEX)
					clients_broadcast_event_mask();
			}
			else {
				rsp_sub = malloc(resp_size);
				if (!rsp_sub) {
					warn("Failed to allocate set event mask response packet in failure case	err:%d\n", errno);
					return -errno;
				}
				memcpy(rsp_sub, req_sub, sizeof(*req_sub));
				rsp_sub->status = DIAG_CMD_EVENT_ERROR_CODE_FAIL;
			}

			diag_apps_rsp_send(pid, (unsigned char *)rsp_sub, resp_size);
			free(rsp_sub);
			rsp_sub = NULL;
		}
		break;
		case DIAG_CMD_OP_EVENT_TOGGLE:
		{
			diag_cmd_op_event_toggle_sub_req_t *req = (diag_cmd_op_event_toggle_sub_req_t *)buf;
			diag_cmd_op_event_toggle_sub_rsp_t resp = {0};
			uint8_t toggle = 0;
			int preset = 0;

			diagmem->mask_set_status = 1;

			if (len < sizeof(diag_cmd_op_event_toggle_sub_req_t))
			{
				ALOGE("%s: length is less than size of req_sub struct, len = %zu, cmd = %d \n", __func__, len, req_msg->sub_cmd);
				return -EMSGSIZE;
			}

			toggle = req->operation_switch;
			preset = req->preset_id;

			diag_cmd_toggle_events(toggle, pid, req->sub_id);
			peripheral_broadcast_event_mask(req->sub_id, preset, pid);

			if (req->sub_id==INVALID_INDEX)
				clients_broadcast_event_mask();

			memcpy(&resp, req, sizeof(diag_cmd_op_event_toggle_sub_rsp_t ));

			diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(diag_cmd_op_event_toggle_sub_rsp_t ));
			return 0;
		}
		break;
	}

	return 0;
}

void register_common_cmds(void)
{
	register_common_cmd(DIAG_CMD_LOGGING_CONFIGURATION, handle_logging_configuration);
	register_common_cmd(DIAG_CMD_EXTENDED_MESSAGE_CONFIGURATION, handle_extended_message_configuration);
	register_common_cmd(DIAG_CMD_GET_MASK, handle_event_get_mask);
	register_common_cmd(DIAG_CMD_SET_MASK, handle_event_set_mask);
	register_common_cmd(DIAG_CMD_EVENT_REPORT_CONTROL, handle_event_report_control);

	register_common_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_MS_LOG_SUBSYS_CMD, handle_ms_logging_configuration);
	register_common_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_MS_MSG_SUBSYS_CMD, handle_ms_extended_message_configuration);
	register_common_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_MS_EVENT_SUBSYS_CMD, handle_ms_event_report_control);
}

