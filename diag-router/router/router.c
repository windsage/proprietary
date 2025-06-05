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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "diag.h"
#include "dm.h"
#include "hdlc.h"
#include "peripheral.h"
#include "util.h"
#include "diag_hidl_wrapper.h"
#include "diag_mux.h"
#include "diag_cntl.h"
#include "sys_time.h"

/* registration table retrieval related definitions */
#define MAX_CMD_TABLE_SIZE		4096
#define DELAYED_RESP_COUNT_SIGN_BIT	0x8000 /* msb act as sign bit, means more to come */
#define CMD_TABLE_REG_RETRIEVAL_VER_1	1

/* definitions to support diag-id based command request/response */
#define CMD_FLAG_CMD_DIAG_ID	0x01
#define CMD_FLAG_CMD_HANDLE	0x02
#define CMD_FLAG_CMD_SEARCH	0x04

enum cmd_resp {
	CMD_HANDLED,
	CMD_FOUND,
	CMD_NOT_FOUND,
};

/* parameters that passed to handle the diag-id based error response packet */
struct cmd_param {
	unsigned int key;
	int pid;
	uint8_t diag_id;
	uint8_t flag;
	uint8_t resp;
};

struct cmd_reg_table_param {
	int16_t resp_cnt;
	uint8_t diag_id;
	size_t  length;
	uint32_t cnt;
};

struct list_head fallback_cmds = LIST_INIT(fallback_cmds);
struct list_head common_cmds = LIST_INIT(common_cmds);
struct list_head apps_cmds = LIST_INIT(apps_cmds);

static void diag_rsp_err_command(uint8_t *msg, size_t len, enum err_resp error_type,
				int pid, void *err_data);

int hdlc_enqueue_flow(struct diag_client *dm, struct list_head *queue, const void *msg, size_t msglen, struct watch_flow *flow)
{
	size_t outlen;
	void *outbuf;
	int encode_flag = 0;

	if (dm && dm->hdlc_enc_done) {
		outbuf = (void*)msg;
		outlen = msglen;
	} else {
		outbuf = hdlc_encode(dm, msg, msglen, &outlen);
		if (!outbuf) {
			ALOGE("diag: failed to allocate hdlc destination buffer");
			return -ENOMEM;
		}
		if (dm && (dm->hdlc_read_buf == outbuf))
			encode_flag = 0;
		else
			encode_flag = 1;
	}
	queue_push_flow(dm, queue, outbuf, outlen, flow);
	if (encode_flag) {
		free(outbuf);
		outbuf = NULL;
	}
	return 0;
}
int hdlc_enqueue(struct list_head *queue, const void *msg, size_t msglen)
{
	return hdlc_enqueue_flow(NULL, queue, msg, msglen, NULL);
}

/* handle cmd registered by peripherals */
static int diag_cmd_periph_handle(struct diag_client *client, uint8_t *ptr,
				size_t len, struct cmd_param *param)
{
	struct list_head *item;
	struct diag_cmd *dc;

	param->resp = CMD_NOT_FOUND;

	list_for_each(item, &diag_cmds) {
		dc = container_of(item, struct diag_cmd, node);

		if (param->key < dc->first || param->key > dc->last)
			continue;

		/* if diag-id is passed check whether cmd registered by same diag instance */
		if (param->diag_id && dc->peripheral->root_pd.diag_id != param->diag_id)
			continue;

		/* if the request is to check the presence of cmd, return cmd found status */
		if (param->flag & CMD_FLAG_CMD_SEARCH) {
			param->resp = CMD_FOUND;
			return 0;
		}

		if (dc->cb) {
			dc->cb(client, ptr, len, param->pid);
			param->resp = CMD_HANDLED;
			return 0;
		} else {
			if (param->flag & CMD_FLAG_CMD_DIAG_ID) {
				ptr -= sizeof(struct diag_id_cmd_req_header_t);
				len += sizeof(struct diag_id_cmd_req_header_t);
			}

			if (diag_debug_mask & DIAG_DBG_MASK_CMD) {
				ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: Sending cmd pkt of len: %d to perif: %s\n",
							__func__, len, dc->peripheral->name);
				print_hex_dump(__func__, (void *) ptr, MIN(len, PRINT_HEX_DUMP_LEN));
			}

			param->resp = CMD_HANDLED;
			return peripheral_send(dc->peripheral, ptr, len);
		}
	}

	return 1;
}

static int diag_check_diag_id_based_cmd(uint8_t *ptr, uint8_t *diag_id, uint8_t flag)
{
        struct diag_id_cmd_req_header_t *diag_id_cmd;
        uint8_t diagid;
        int ret = 0;

        if (!ptr)
                return -EINVAL;

        diag_id_cmd = (struct diag_id_cmd_req_header_t *)ptr;

        switch(flag) {
        case DIAG_ID_FLAG_CMD_VALIDITY:

                if (diag_id_cmd->cmd_code == DIAG_CMD_DIAG_ID_CMD_REQ)
                        ret = 1;
                break;
        case DIAG_ID_FLAG_GET_DIAGID:

                if (!diag_id)
                        return -EINVAL;

                if (diag_id_cmd->cmd_code == DIAG_CMD_DIAG_ID_CMD_REQ &&
                    diag_id_cmd->version == DIAG_CMD_DIAG_ID_CMD_VERSION_1) {
                        diagid = diag_id_cmd->diag_id;
                        if (diag_check_diag_id_valid(diagid)) {
                                *diag_id = diagid;
                                ret = 1;
                                ALOGI("diag: %s: valid diag_id %d found\n", __func__, diagid);
                        } else {
                                ret = -EINVAL;
                                ALOGE("diag: %s: invalid diag_id %d recvd\n", __func__, diagid);
                        }
                }
                break;
        default:
                ALOGE("diag: %s: Non-supported option %d\n", __func__, flag);
                break;
        }

        return ret;
}

uint32_t diag_check_duplicate_cmd_entry(struct diag_client *client, uint8_t *ptr, size_t len,
					struct cmd_param *param)
{
	uint32_t diag_id_mask = 0;
	uint8_t last_diag_id;
	int id;

	if (param) {
		last_diag_id = diag_query_last_diag_id();
		for(id = (DIAG_ID_APPS + 1); id <= last_diag_id; id++) {
			param->flag = CMD_FLAG_CMD_SEARCH;
			param->diag_id = id;
			diag_cmd_periph_handle(client, ptr, len, param);
			if (param->resp == CMD_FOUND) {
				/* set diag-id mask based on duplicate cmd registered by peripheral */
				diag_id_mask |= 0x1 << (id-1);
			}
		}
	}

	return diag_id_mask;
}

int diag_cmd_handle_priority_diag_id(struct diag_client *client, uint8_t *ptr, size_t len,
				     struct cmd_param *param, uint8_t diag_id)
{
	int ret = 0;

	if (param) {
		param->diag_id = diagmem->priority_diagid;
		param->flag = CMD_FLAG_CMD_HANDLE;
		if (diag_id)
			param->flag |= CMD_FLAG_CMD_DIAG_ID;

		ret = diag_cmd_periph_handle(client, ptr, len, param);
		/* peripheral that own priority diag id consumed it */
		if (param->resp == CMD_HANDLED)
			ALOGM(DIAG_DBG_MASK_CMD,
				"diag: %s: cmd handled by diag instance with id %d\n",
				__func__, diagmem->priority_diagid);
	}

	return ret;
}

static int diag_cmd_dispatch(struct diag_client *client, uint8_t *ptr,
			     size_t len, int pid)
{
	struct list_head *item;
	struct diag_cmd *dc;
	unsigned int key = 0;
	int handled = 0;
	unsigned char apps_buf[DIAG_MAX_REQ_SIZE];
	struct cmd_param param = {0,};
	uint32_t diag_id_mask = 0;
	int pkt_type = PKT_TYPE;
	uint8_t diag_id = 0;
	int ret = 0;
	int id;

	memset(apps_buf, 0, sizeof(apps_buf));

	ret = diag_check_diag_id_based_cmd(ptr, &diag_id, DIAG_ID_FLAG_GET_DIAGID);
	if (ret) {
		if (ret < 0) {
			return ret;
		} else {
			ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: valid diag-id based cmd found with id %d\n",
				__func__, diag_id);
			/* update ptr to point to starting of legacy header */
			ptr += sizeof(struct diag_id_cmd_req_header_t);
			len -= sizeof(struct diag_id_cmd_req_header_t);
		}
	}

	if (ptr[0] == DIAG_CMD_SUBSYS_DISPATCH ||
	    ptr[0] == DIAG_CMD_SUBSYS_DISPATCH_V2) {
		if (len == 1)
			key = ptr[0] << 24;
		else if (len == 2)
			key = ptr[0] << 24 | ptr[1] << 16;
		else if (len == 3)
			key = ptr[0] << 24 | ptr[1] << 16 | ptr[2];
		else
			key = ptr[0] << 24 | ptr[1] << 16 | ptr[3] << 8 | ptr[2];
	} else {
		key = 0xff << 24 | 0xff << 16 | ptr[0];
	}

	param.key = key;
	param.pid = pid;

	if (ptr[0] == MODE_CMD && ptr[1] != MODE_CMD_RESET) {
		goto periph_send;
	}

	list_for_each(item, &common_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (key < dc->first || key > dc->last)
			continue;
		if (diag_id > DIAG_ID_APPS) {
			/* diag_id based cmd should not be used for common commands */
			ALOGE("diag: %s: diag-d based cmd not supported for common cmd's key = %d\n",
				__func__, key);
			return -EINVAL;
		}
		return dc->cb(client, ptr, len, pid);
	}

	/* pvm received cmd with diag-id that needs to be handled from peripheral */
	if (!vm_enabled && diag_id > DIAG_ID_APPS)
		goto periph_send;

	list_for_each(item, &apps_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (key < dc->first || key > dc->last)
			continue;

		/* if diag_id cmd received on pvm, handle within pvm */
		if (diag_id == DIAG_ID_APPS)
			goto pvm_cmd;

		/**
		 * if legacy cmd received in pvm
		 * [.] send cmd to peripheral if priority diag-id configured is not pvm
		 * [.] send duplcate error response if priority diag-id is not configured & duplicate entry found
		 * [.] handle cmd within PVM if priority diag-id configured by tool is pvm or entry found only with pvm
		 */
		if (diagmem->priority_diagid > DIAG_ID_APPS) {
			/**
			 * if priority diag-id set other than pvm diag-id
			 * cmd should be handled by peripheral if it registered the same cmd
			 */
			ret = diag_cmd_handle_priority_diag_id(client, ptr, len, &param, diag_id);
			if (param.resp == CMD_HANDLED)
				return ret;

		} else if (!diagmem->priority_diagid) {
			/**
			 * if priority diag-id is not set
			 * [.] send duplicate error response if same cmd registerd by peripheral
			 * [.] handle within pvm is not registered by any peripheral
			 */
			diag_id_mask = diag_check_duplicate_cmd_entry(client, ptr, len, &param);

			if (diag_id_mask) {
				ALOGI("diag: %s: Duplicate cmd found for diag_id_mask: 0x%x cmd len: %d\n",
					__func__, diag_id_mask, len);
				diag_rsp_err_command(ptr, len, ERR_DUPLICATE_CMD, pid, &diag_id_mask);
				return 0;
			}
		}

pvm_cmd:
		/* handle the cmd in pvm itself since no duplicate entry found */
		if (dc->cb)
			dc->cb(client, ptr, len, pid);
		else {
			if (len + sizeof(pkt_type) >= MAX_PKT_LEN)
				return -EMSGSIZE;
			memcpy(apps_buf, &pkt_type, sizeof(pkt_type));
			memcpy(apps_buf + sizeof(pkt_type), ptr, len);
			if (diag_debug_mask & DIAG_DBG_MASK_CMD) {
				ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: Sending cmd pkt of len: %d to client fd: %d\n",
					__func__, len, dc->fd);
				print_hex_dump(__func__, (void *)ptr, MIN(len, PRINT_HEX_DUMP_LEN));
			}
			ret = write(dc->fd, apps_buf, len + sizeof(pkt_type));
			if (ret < 0)
				return -ENOENT;
		}

		handled++;
		break;
	}

	if (handled)
		return 0;

#ifndef FEATURE_LE_DIAG
	handled = check_for_diag_system_client_commands(key, ptr, len);
	if (handled) {
		if (diag_debug_mask & DIAG_DBG_MASK_CMD)
			print_hex_dump(__func__, ptr, MIN(len, PRINT_HEX_DUMP_LEN));
		return 0;
	}
#endif /* FEATURE_LE_DIAG */

periph_send:

	if (!diag_id) {

		/**
		 * if legacy command received
		 * [.] cmd should be handled by a peripheral if priority diag-id associated with that peripheral
		 * [.] send duplicate error response if multiple peripherals registered the same cmd
		 * [.] send to peripheral if only one peripheral registered the cmd
		 */
		if (diagmem->priority_diagid > DIAG_ID_APPS) {
			ret = diag_cmd_handle_priority_diag_id(client, ptr, len, &param, 0);
			if (param.resp == CMD_HANDLED)
				return ret;
		}

		diag_id_mask = diag_check_duplicate_cmd_entry(client, ptr, len, &param);

		if (diag_id_mask && ((diag_id_mask & (diag_id_mask - 1)) != 0)) {
			diag_rsp_err_command(ptr, len, ERR_DUPLICATE_CMD, pid, &diag_id_mask);
			return 0;
		}
	}

	param.diag_id = diag_id;
	param.flag = CMD_FLAG_CMD_HANDLE;
	if (diag_id)
		param.flag |= CMD_FLAG_CMD_DIAG_ID;
	ret = diag_cmd_periph_handle(client, ptr, len, &param);
	if (param.resp == CMD_HANDLED)
		return ret;

	list_for_each_entry(dc, &fallback_cmds, node) {
		if (key < dc->first || key > dc->last)
			continue;

		if (diag_debug_mask & DIAG_DBG_MASK_CMD) {
			ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: Diag-router handling cmd pkt of len: %d\n",
				__func__, len);
			print_hex_dump(__func__, (void *) ptr, MIN(len, PRINT_HEX_DUMP_LEN));
		}
		return dc->cb(client, ptr, len, pid);
	}

	return -ENOENT;
}

/* new error response handling */
static void diag_rsp_err_command(uint8_t *msg, size_t len, enum err_resp error_type,
				int pid, void *err_data)
{
	diag_id_err_resp *err_resp = NULL;
	uint8_t *buf = NULL;
	uint32_t resp_len = 0;
	uint32_t ts_lo;
	uint32_t ts_hi;

	/* allocate memory for maximum size error response */
	buf = malloc(sizeof(diag_id_err_resp) + sizeof(err_data_type_max_size) + len);
	if (!buf) {
		ALOGE("diag:%s: failed to allocate error buffer\n", __func__);
		return;
	}

	ts_get_lohi(&ts_lo, &ts_hi);

	err_resp = (diag_id_err_resp *)buf;
	err_resp->err_code = DIAG_CMD_DIAG_ID_ERR_RESPONSE;
	err_resp->version = DIAG_CMD_DIAG_ID_ERR_RESPONSE_VER_1;
	err_resp->diag_id = DIAG_ID_APPS;
	err_resp->ts_type = 0;
	err_resp->err_type = error_type;
	err_resp->ts_lo = ts_lo;
	err_resp->ts_hi = ts_hi;

	switch(error_type) {
	case ERR_DUPLICATE_CMD:
	{
		err_data_type_duplicate_cmd *dup_err_resp;

		dup_err_resp = (err_data_type_duplicate_cmd *) (buf + sizeof(diag_id_err_resp));
		/* err_data will be 4 byte diag_id_mask */
		dup_err_resp->diag_id_mask = *((uint32_t*)err_data);
		resp_len = sizeof(diag_id_err_resp) + sizeof(err_data_type_duplicate_cmd);
		break;
	}
	default:
		ALOGE("diag: %s: Unknown error code %d recvd\n", __func__, error_type);
		break;
	}

	memcpy((void*)(buf + resp_len), (void*)msg, len);
	resp_len += len;

	diag_apps_rsp_send(pid, buf, resp_len);
	free(buf);
	buf = NULL;
}

static void diag_rsp_bad_command(struct diag_client *client, uint8_t *msg,
				 size_t len, int error_code, int pid)
{
	uint32_t req_head_size = sizeof(struct diag_id_cmd_req_header_t);
	uint32_t resp_len = len + 1 + DIAG_ID_CMD_RESP_TIMESTAMP_SIZE;
	uint32_t timestamp[2];
	uint32_t tmp_size;
	uint8_t *buf;
	(void)client;
	int ret;

	buf = malloc(resp_len);
	if (!buf) {
		err(1, "failed to allocate error buffer");
		return;
	}

	buf[0] = error_code;

	ret = diag_check_diag_id_based_cmd(msg, NULL, DIAG_ID_FLAG_CMD_VALIDITY);
	if (ret < 0) {
		ALOGE("diag: %s: Invalid arguments passed\n", __func__);
		ret = 0;
	}

        if (!ret) {
		resp_len = len + 1;
		memcpy(buf + 1, msg, len);
        } else {
		ts_get_lohi(&timestamp[0], &timestamp[1]);
		if (len >= req_head_size) {
			memcpy(buf + 1, msg, req_head_size);
			tmp_size = 1 + req_head_size;
			memcpy(buf + tmp_size, &timestamp[0], DIAG_ID_CMD_RESP_TIMESTAMP_SIZE);
			tmp_size += DIAG_ID_CMD_RESP_TIMESTAMP_SIZE;
			memcpy(buf + tmp_size, (msg + req_head_size), len - req_head_size);
			ALOGI("diag: %s: Sending invalid response for diag-id based cmd\n", __func__);
		}
        }

	diag_apps_rsp_send(pid, buf, resp_len);
	free(buf);
	buf = NULL;
}

int diag_client_handle_command(struct diag_client *client, uint8_t *data, size_t len, int pid)
{
	int ret;

	ret = diag_cmd_dispatch(client, data, len, pid);

	switch (ret) {
	case -ENOENT:
		diag_rsp_bad_command(client, data, len, DIAG_CMD_RSP_BAD_COMMAND, pid);
		break;
	case -EINVAL:
		diag_rsp_bad_command(client, data, len, DIAG_CMD_RSP_BAD_PARAMS, pid);
		break;
	case -EMSGSIZE:
		diag_rsp_bad_command(client, data, len, DIAG_CMD_RSP_BAD_LENGTH, pid);
		break;
	default:
		break;
	}

	return 0;
}

void register_fallback_cmd(unsigned int cmd,
			   int(*cb)(struct diag_client *client,
				    const void *buf, size_t len, int pid))
{
	struct diag_cmd *dc;
	unsigned int key = 0xffff0000 | cmd;

	dc = calloc(1, sizeof(*dc));
	if (!dc) {
		err(1, "failed to allocate diag command\n");
		return;
	}

	dc->first = key;
	dc->last = key;
	dc->cb = cb;

	list_add(&fallback_cmds, &dc->node);
}

void register_fallback_subsys_cmd(unsigned int subsys, unsigned int cmd,
					int(*cb)(struct diag_client *client,
					const void *buf, size_t len, int pid))
{
	struct diag_cmd *dc;
	unsigned int key = DIAG_CMD_SUBSYS_DISPATCH << 24 |
			   (subsys & 0xff) << 16 | cmd;

	dc = calloc(1, sizeof(*dc));
	if (!dc) {
		err(1, "failed to allocate diag command\n");
		return;
	}
	dc->first = key;
	dc->last = key;
	dc->cb = cb;

	list_add(&fallback_cmds, &dc->node);
}
void register_fallback_subsys_cmd_range(unsigned int subsys, unsigned int cmd_code_low,
					unsigned int cmd_code_high,
					int(*cb)(struct diag_client *client,
					const void *buf, size_t len, int pid))
{
	struct diag_cmd *dc;
	unsigned int cmd_code_low_key = DIAG_CMD_SUBSYS_DISPATCH << 24 |
				(subsys & 0xff) << 16 | cmd_code_low;
	unsigned int cmd_code_high_key = DIAG_CMD_SUBSYS_DISPATCH << 24 |
 				(subsys & 0xff) << 16 | cmd_code_high;

	dc = calloc(1, sizeof(*dc));
	if (!dc) {
		err(1, "failed to allocate diag command\n");
		return;
	}
	dc->first = cmd_code_low_key;
	dc->last = cmd_code_high_key;
	dc->cb = cb;

	list_add(&fallback_cmds, &dc->node);
}

void register_fallback_subsys_cmd_v2(unsigned int subsys, unsigned int cmd,
	int(*cb)(struct diag_client *client,
	const void *buf, size_t len, int pid))
{
	struct diag_cmd *dc;
	unsigned int key = DIAG_CMD_SUBSYS_DISPATCH_V2 << 24 |
		(subsys & 0xff) << 16 | cmd;

	dc = calloc(1, sizeof(*dc));
	if (!dc) {
		err(1, "failed to allocate diag command\n");
		return;
	}
	dc->first = key;
	dc->last = key;
	dc->cb = cb;

	list_add(&fallback_cmds, &dc->node);
}

void register_common_cmd(unsigned int cmd, int(*cb)(struct diag_client *client,
						    const void *buf,
						    size_t len , int pid))
{
	struct diag_cmd *dc;
	unsigned int key = 0xffff0000 | cmd;

	dc = calloc(1, sizeof(*dc));
	if (!dc) {
		err(1, "failed to allocate diag command\n");
		return;
	}

	dc->first = key;
	dc->last = key;
	dc->cb = cb;

	list_add(&common_cmds, &dc->node);
}


void register_common_subsys_cmd(unsigned int subsys, unsigned int cmd,
				int(*cb)(struct diag_client *client,
				const void *buf, size_t len, int pid))
{
	struct diag_cmd *dc;
	unsigned int key = DIAG_CMD_SUBSYS_DISPATCH << 24 |
			   (subsys & 0xff) << 16 | cmd;

	dc = calloc(1, sizeof(*dc));
	if (!dc) {
		err(1, "failed to allocate diag command\n");
		return;
	}

	dc->first = key;
	dc->last = key;
	dc->cb = cb;

	list_add(&common_cmds, &dc->node);
}

/* forward vm diag registered commands to pvm to add it in master table */
static int diag_send_cmd_registration_to_pvm(unsigned char *buf, size_t len)
{
	struct diag_cmd_reg_tbl *pkt = (struct diag_cmd_reg_tbl*)buf;
	struct diag_cntl_cmd_reg *ctrl_pkt = NULL;
	struct peripheral *peripheral = NULL;
	size_t cntrl_pkt_len;
	int i;

	if (!buf || (len < (pkt->count * sizeof(diag_cmd_reg_entry))))
		return -EINVAL;

	cntrl_pkt_len = sizeof(*ctrl_pkt) + pkt->count * sizeof(struct cmd_range_reg);

	ctrl_pkt = calloc(1, cntrl_pkt_len);
	if (!ctrl_pkt) {
		warn("malloc failed");
		return -ENOMEM;
	}

	for (i = 0; i < pkt->count; i++) {
		ctrl_pkt->cmd = pkt->entries[i].cmd;
		ctrl_pkt->subsys = pkt->entries[i].subsys;
		ctrl_pkt->ranges[i].first = pkt->entries[i].first;
		ctrl_pkt->ranges[i].last = pkt->entries[i].last;
	}

	/* vm diag will forward the registered cmd to pvm diag */
	ctrl_pkt->count_entries = pkt->count;
	ctrl_pkt->hdr.cmd = DIAG_CNTL_CMD_REGISTER;
	ctrl_pkt->hdr.len = cntrl_pkt_len - sizeof(ctrl_pkt->hdr);

	peripheral = diag_get_periph_info(PERIPHERAL_APPS);
	if (peripheral)
		queue_push_cntlq(peripheral, ctrl_pkt, cntrl_pkt_len);
	free(ctrl_pkt);

	return 0;
}

/* forward vm removed commands to pvm to clean master table */
static int diag_remove_cmd_registrations_from_pvm(struct diag_cmd *dc)
{
	struct diag_cntl_cmd_single_dereg ctrl_pkt;
	struct peripheral *peripheral = NULL;
	size_t cntrl_pkt_len;

	ctrl_pkt.count_entries = 1;
	ctrl_pkt.hdr.cmd = DIAG_CNTL_CMD_DEREGISTER;
	ctrl_pkt.cmd = ((dc->first >> 24) & 0xFF);
	ctrl_pkt.subsys = ((dc->first >> 16) & 0xFF);
	ctrl_pkt.range.first = dc->first;
	ctrl_pkt.range.last = dc->last;
	ctrl_pkt.hdr.len = sizeof(ctrl_pkt) - sizeof(ctrl_pkt.hdr);
	cntrl_pkt_len = ctrl_pkt.hdr.len + sizeof(ctrl_pkt.hdr);
	peripheral = diag_get_periph_info(PERIPHERAL_APPS);
	if (peripheral)
		queue_push_cntlq(peripheral, &ctrl_pkt, cntrl_pkt_len);

	return 0;
}

int diag_add_apps_cmd_registrations(int fd,
			      unsigned char * buf, size_t len)
{
	struct diag_cmd_reg_tbl *pkt = (struct diag_cmd_reg_tbl*)buf;
	struct diag_cmd *dc;
	unsigned int subsys;
	unsigned int cmd;
	unsigned int first;
	unsigned int last;
	int i;

	for (i = 0; i < pkt->count; i++) {
		cmd = pkt->entries[i].cmd;
		subsys = pkt->entries[i].subsys;
		if (cmd == DIAG_CMD_NO_SUBSYS_DISPATCH && subsys != DIAG_CMD_NO_SUBSYS_DISPATCH)
			cmd = DIAG_CMD_SUBSYS_DISPATCH;
		first = cmd << 24 | subsys << 16 | pkt->entries[i].first;
		last = cmd << 24 | subsys << 16 | pkt->entries[i].last;

		dc = malloc(sizeof(*dc));
		if (!dc) {
			warn("malloc failed");
			return -ENOMEM;
		}
		memset(dc, 0, sizeof(*dc));

		dc->first = first;
		dc->last = last;
		dc->fd = fd;

		list_add(&apps_cmds, &dc->node);
	}

	/* vm diag will forward the registered cmd to pvm diag */
	if (vm_enabled)
		return diag_send_cmd_registration_to_pvm(buf, len);

	return 0;
}

int diag_remove_cmd_registrations(int fd)
{
	struct diag_cmd *dc;
	struct list_head *item;
	struct list_head *next;

	list_for_each_safe(item, next, &apps_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (dc->fd == fd) {
			if (vm_enabled)
				diag_remove_cmd_registrations_from_pvm(dc);
			list_del(&dc->node);
			free(dc);
			dc = NULL;
		}
	}

	return 0;
}

#define FILL_CMD_TABLE(id, resp, offset, first, last)					\
{											\
	(resp)->cmd_list[(offset)].diag_id = (id);					\
	(resp)->cmd_list[(offset)].cmd_code = (uint8_t)(((first) >> 24) & 0xFF);	\
	(resp)->cmd_list[(offset)].subsys_id = (uint8_t)(((first) >> 16) & 0xFF);	\
	(resp)->cmd_list[(offset)].subsys_cmd_code_s = (uint16_t)((first) & 0xFFFF);	\
	(resp)->cmd_list[(offset)].subsys_cmd_code_e = (uint16_t)((last) & 0xFFFF);	\
}

static void diag_aggregate_cmd_reg_table(struct diag_cmd_registration_table_rsp *resp,
		struct diag_cmd *dc, struct cmd_reg_table_param *param, int pid)
{
	if ((param->length + sizeof(struct cmd_table_list)) >= MAX_CMD_TABLE_SIZE) {
		/* send the response that already filled, then send pending response */
		resp->no_of_entries = param->cnt;
		if (!param->resp_cnt) {
			param->resp_cnt = 1;
			resp->rsp_cnt = DELAYED_RESP_COUNT_SIGN_BIT;
		} else {
			resp->rsp_cnt = DELAYED_RESP_COUNT_SIGN_BIT + param->resp_cnt++;
		}

		diag_apps_rsp_send(pid, (unsigned char *)resp, param->length);
		param->cnt = 0;
		param->length = sizeof(struct diag_cmd_registration_table_rsp);
	}
	FILL_CMD_TABLE(param->diag_id, resp, param->cnt, dc->first, dc->last);
	param->cnt++;
	param->length += sizeof(struct cmd_table_list);
}

int diag_send_cmd_reg_table(struct diag_cmd_registration_table_req *req, int pid)
{
	struct diag_cmd_registration_table_rsp *resp;
	uint8_t resp_buf[MAX_CMD_TABLE_SIZE] = {0,};
	struct cmd_reg_table_param param = {0,};
	struct list_head *item;
	unsigned int key = 0;
	struct diag_cmd *dc;
	uint8_t diag_id;

	param.length = sizeof(struct diag_cmd_registration_table_rsp);

	diag_id = req->diag_id;

	resp = (struct diag_cmd_registration_table_rsp*)resp_buf;
	/* copy header from request to response buffer */
	memcpy((void*)resp, (void*)req, sizeof(struct diag_pkt_header_t));
	resp->version = CMD_TABLE_REG_RETRIEVAL_VER_1;
	resp->delayed_rsp_id = diag_get_next_delayed_rsp_id();

	if (req->version != CMD_TABLE_REG_RETRIEVAL_VER_1) {
		resp->status = CMD_ERR_INVALID_VERSION;
		diag_apps_rsp_send(pid, (unsigned char *)resp, param.length);
		return 0;
	}

	/* check diag-id value */
	if (diag_id > DIAG_ID_APPS && !diag_check_diag_id_valid(diag_id)) {
		resp->status = CMD_ERR_INVALID_DIAGID;
		diag_apps_rsp_send(pid, (unsigned char *)resp, param.length);
		return 0;
	}

	resp->status = 0;
	resp->rsp_cnt = 0;

	/**
	 * need to send cmd registration table with pvm registered commands
	 * for below cases
	 * - if no diag-id mentioned (need to send full table)
	 * - if pvm diag-id requested
	 */
	if (!diag_id || diag_id == DIAG_ID_APPS) {

		param.diag_id = DIAG_ID_APPS;

		list_for_each_entry(dc, &common_cmds, node) {
			diag_aggregate_cmd_reg_table(resp, dc, &param, pid);
		}

		list_for_each_entry(dc, &apps_cmds, node) {
			diag_aggregate_cmd_reg_table(resp, dc, &param, pid);
		}

		list_for_each_entry(dc, &fallback_cmds, node) {
			diag_aggregate_cmd_reg_table(resp, dc, &param, pid);
		}
	}

	list_for_each_entry(dc, &diag_cmds, node) {
		if (diag_id && (diag_id != dc->peripheral->root_pd.diag_id))
			continue;

		param.diag_id = dc->peripheral->root_pd.diag_id;
		diag_aggregate_cmd_reg_table(resp, dc, &param, pid);
	}

	resp->no_of_entries = param.cnt;
	/* drop sign bit from the resp_cnt since this is the last delayed response */
	if (param.resp_cnt)
		resp->rsp_cnt = param.resp_cnt & (~DELAYED_RESP_COUNT_SIGN_BIT);

	diag_apps_rsp_send(pid, (unsigned char *)resp, param.length);

	return 0;
}
