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
#include <fcntl.h>
#include "diag.h"
#include "dm.h"
#include "hdlc.h"
#include "util.h"
#include "diag_cntl.h"
#include <unistd.h>
#include <linux/reboot.h>
#include "diag_mux.h"
#include "diag_hidl_wrapper.h"
#include "vm.h"
#include "peripheral.h"

/* strlcpy is from OpenBSD and not supported by Linux Embedded.
 * GNU has an equivalent g_strlcpy implementation into glib.
 * Featurized with compile time USE_GLIB flag for Linux Embedded builds.
 */

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

#define DIAG_CMD_KEEP_ALIVE_SUBSYS	50
#define DIAG_CMD_KEEP_ALIVE_CMD		3

#define DIAG_CMD_DIAG_VERSION_ID	28
#define DIAG_PROTOCOL_VERSION_NUMBER	2

#define DIAG_CMD_EXTENDED_BUILD_ID	124
#define DIAG_CMD_DEL_RSP_WRAP	0x04
#define DIAG_CMD_DEL_RSP_WRAP_CNT	0x05
#define DIAG_CMD_EXT_MOBILE_ID 6
#define DIAG_CMD_DIAG_VERSION_NO 0
#define MOBILE_MODEL_NUMBER		0
#define MOBILE_SOFTWARE_REVISION	"OE"
#define MOBILE_MODEL_STRING		"DB410C"
#define MSM_REVISION_NUMBER		2
#define DIAG_CMD_DIAG_GET_DIAG_ID	0x222
#define DIAG_CMD_DIAG_STM 0x214
#define DIAG_CMD_DIAG_TIME_SYNC_QUERY 0x21b
#define DIAG_CMD_DIAG_TIME_SYNC_SWITCH 0x21c
#define DIAG_CMD_QUERY_TRANSPORT	0x223
#define DIAG_CMD_DIAG_FEATURE_QUERY  0x225
#define DIAG_CMD_OP_HDLC_DISABLE	0x218
#define DIAG_CMD_CLEAR_MASKS 0x22f
#define DIAG_CMD_ALL_MASK_CLEAR	0x230
#define DIAG_CMD_PKT_FORMAT_SELECT_REQUEST	0x232
#define DIAG_CMD_REGISTRATION_TABLE_REQUEST	0x233
#define DIAG_CMD_PRIORITY_DIAGID_CONFIG_REQUEST 0x234

#define DIAG_TIMESTAMP_SWITCH  0x00320000

#define STM_REQ_VERSION_2		2
#define STM_REQ_VERSION_3		3

#define DISABLE_STM	0
#define ENABLE_STM	1
#define STATUS_STM	2
#define STM_AUTO_QUERY  3
#define DIAG_TIME_SWITCH "/data/time_switch"
#define SOC_BUF_SIZE	5
#define DIAGPKT_MAX_DELAYED_RSP 0xFFFF

/* diag-id priority config request related definitions */
#define PRIORITY_DIAGID_CONFG_REQ_VER1	1

#define PRIORITY_DIAGID_GET		0
#define PRIORITY_DIAGID_SET		1
#define PRIORITY_DIAGID_CLEAR		2

struct list_head diag_id_list = LIST_INIT(diag_id_list);

struct diag_query_transport_req_t {
	struct diag_pkt_header_t header;
} __packed;

struct diag_query_transport_rsp_t {
	struct diag_pkt_header_t header;
	uint8_t transport;
} __packed;

struct diag_all_mask_clear_req_t {
	struct diag_pkt_header_t header;
	uint8_t version;
} __packed;

struct diag_all_mask_clear_rsp_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t status;
} __packed;

struct diag_cmd_diag_id_query_req_t {
	struct diag_pkt_header_t header;
	uint8_t version;
} __packed;

struct diag_cmd_diag_id_query_rsp_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t num_entries;
	struct diag_id_t entry;
} __packed;
struct diag_cmd_diag_keep_alive_rsp {
	struct diag_pkt_header_t header;
	uint32_t proc_id;
	uint32_t status;
	uint32_t reserved;
} __packed;
struct diag_cmd_time_sync_query_req_t {
	struct diag_pkt_header_t header;
	uint8_t version;
} __packed;

struct diag_cmd_time_sync_query_rsp_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t time_api;
} __packed;

struct diag_cmd_time_sync_switch_req_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t time_api;
	uint8_t persist_time;
} __packed;

struct diag_cmd_time_sync_switch_rsp_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t time_api;
	uint8_t time_api_status;
	uint8_t persist_time_status;
} __packed;

struct diag_cmd_diag_feature_query_req_t {
	struct diag_pkt_header_t header;
} __packed;

struct diag_cmd_diag_feature_query_rsp_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t feature_len;
	uint8_t feature_mask[4];
} __packed;

struct timestamp_switch_pkt {
	int type;
	int timestamp_switch;
} __packed;

struct diag_cmd_hdlc_disable_req_t {
	struct diag_pkt_header_t header;
} __packed;

struct diag_cmd_hdlc_disable_rsp_t {
	struct diag_pkt_header_t header;
	uint8_t framing_version;
	uint8_t result;
} __packed;

struct diag_cmd_diag_clear_mask_req {
         struct diag_pkt_header_t header;
	 uint8_t version;
         uint8_t set_clear_mask_flag;
} __packed;

struct diag_cmd_diag_clear_mask_rsp {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t status;
} __packed;

struct diag_cmd_pkt_format_select_req {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t pkt_format_mask;
} __packed;

struct diag_cmd_pkt_format_select_rsp {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t pkt_format_mask;
	uint8_t status;
} __packed;

struct diag_cmd_diagid_priority_confg_req {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t opcode;
	uint8_t diag_id;
} __packed;

struct diag_cmd_diagid_priority_confg_rsp {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t opcode;
	uint8_t diag_id;
	uint8_t status;
} __packed;

static int wrap_enabled;
static uint16_t wrap_count;
static int send_timestamp_switch = 0;

static int handle_diag_version(struct diag_client *client, const void *buf,
				size_t len, int pid)
{
	uint8_t resp[] = { DIAG_CMD_DIAG_VERSION_ID, DIAG_PROTOCOL_VERSION_NUMBER };
	/* suppress unused parameter warning */
	(void)client;
	(void)buf;
	(void)len;

	return diag_apps_rsp_send(pid, resp, sizeof(resp));
}

static int handle_diag_version_no(struct diag_client *client, const void *buf,
					size_t len, int pid)
{
	uint8_t resp[55];
	/* suppress unused parameter warning */
	(void)client;
	(void)buf;
	(void)len;

	memset(resp, 0, 55);

	return diag_apps_rsp_send(pid, resp, sizeof(resp));
}

static int handle_diag_log_on_demand(struct diag_client *client, const void *buf,
					size_t len, int pid)
{
	struct {
		uint8_t cmd_code;
		uint16_t log_code;
		uint8_t status;
	}__packed *resp;
	size_t resp_size = sizeof(*resp);
	/* suppress unused parameter warning */
	(void)client;
	(void)len;

	/*check for modem status and return err*/
	resp = alloca(resp_size);
	if (!resp) {
                return -EMSGSIZE;
        }
	resp->cmd_code = DIAG_CMD_DIAG_LOG_ON_DEMAND;
	resp->log_code = *(uint16_t *)((char*)buf + 1);
	resp->status = 1;

	return diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
}

static int handle_extended_build_id(struct diag_client *client,
					const void *buf, size_t len, int pid)
{
	struct {
		uint8_t cmd_code;
		uint8_t ver;
		uint16_t reserved;
		uint32_t msm_rev;
		uint32_t mobile_model_number;
		char strings[];
	} __packed *resp;
	size_t resp_size;
	size_t string1_size = strlen(MOBILE_SOFTWARE_REVISION) + 1;
	size_t string2_size = strlen(MOBILE_MODEL_STRING) + 1;
	size_t strings_size = string1_size + string2_size;
	/* suppress unused parameter warning */
	(void)client;
	(void)buf;

	if (len != sizeof(uint8_t))
		return -EMSGSIZE;

	resp_size = sizeof(*resp) + strings_size;

	resp = alloca(resp_size);
	if (!resp)
		return -ENOMEM;
	memset(resp, 0, resp_size);

	resp->cmd_code = DIAG_CMD_EXTENDED_BUILD_ID;
	resp->ver = DIAG_PROTOCOL_VERSION_NUMBER;
	resp->msm_rev = MSM_REVISION_NUMBER;
	resp->mobile_model_number = MOBILE_MODEL_NUMBER;
	strlcpy(resp->strings, MOBILE_SOFTWARE_REVISION, string1_size);
	strlcpy(resp->strings + string1_size, MOBILE_MODEL_STRING, string2_size);

	return diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
}
void diag_process_stm_mask(uint8_t cmd, uint16_t data_mask, int data_type)
{
	int status = 0;
	/* suppress unused parameter warning */
	(void)data_mask;

	if (data_type > PERIPHERAL_APPS && data_type < NUM_PERIPHERALS) {
		if (diagmem->stm_support[data_type]) {
			status = diag_cntl_send_stm_state(data_type, cmd);
			if (status == 0)
				diagmem->stm_state[data_type] = cmd;
		}

	} else if (data_type == PERIPHERAL_APPS) {
		diagmem->stm_state[data_type] = cmd;
	}

	diagmem->stm_state_requested[data_type] = cmd;
}

static uint16_t diag_get_stm_peripheral_mask(void)
{
	uint16_t rsp_supported = 0;

	if (diagmem->stm_support[PERIPHERAL_MODEM])
		rsp_supported |= DIAG_STM_MODEM;

	if (diagmem->stm_support[PERIPHERAL_LPASS])
		rsp_supported |= DIAG_STM_LPASS;

	if (diagmem->stm_support[PERIPHERAL_WCNSS])
		rsp_supported |= DIAG_STM_WCNSS;

	if (diagmem->stm_support[PERIPHERAL_SENSORS])
		rsp_supported |= DIAG_STM_SENSORS;

	if (diagmem->stm_support[PERIPHERAL_CDSP])
		rsp_supported |= DIAG_STM_CDSP;

	if (diagmem->stm_support[PERIPHERAL_NPU])
		rsp_supported |= DIAG_STM_NPU;

	if (diagmem->stm_support[PERIPHERAL_NSP1])
		rsp_supported |= DIAG_STM_NSP1;

	if (diagmem->stm_support[PERIPHERAL_GPDSP0])
		rsp_supported |= DIAG_STM_GPDSP0;

	if (diagmem->stm_support[PERIPHERAL_GPDSP1])
		rsp_supported |= DIAG_STM_GPDSP1;

	rsp_supported |= DIAG_STM_APPS;

	return rsp_supported;
}

static uint16_t diag_get_stm_state_for_peripherals(void)
{
	uint16_t rsp_status = 0;

	if (diagmem->stm_state[PERIPHERAL_MODEM])
		rsp_status |= DIAG_STM_MODEM;

	if (diagmem->stm_state[PERIPHERAL_LPASS])
		rsp_status |= DIAG_STM_LPASS;

	if (diagmem->stm_state[PERIPHERAL_WCNSS])
		rsp_status |= DIAG_STM_WCNSS;

	if (diagmem->stm_state[PERIPHERAL_SENSORS])
		rsp_status |= DIAG_STM_SENSORS;

	if (diagmem->stm_state[PERIPHERAL_CDSP])
		rsp_status |= DIAG_STM_CDSP;

	if (diagmem->stm_state[PERIPHERAL_NPU])
		rsp_status |= DIAG_STM_NPU;

	if (diagmem->stm_state[PERIPHERAL_APPS])
		rsp_status |= DIAG_STM_APPS;

	if (diagmem->stm_state[PERIPHERAL_NSP1])
		rsp_status |= DIAG_STM_NSP1;

	if (diagmem->stm_state[PERIPHERAL_GPDSP0])
		rsp_status |= DIAG_STM_GPDSP0;

	if (diagmem->stm_state[PERIPHERAL_GPDSP1])
		rsp_status |= DIAG_STM_GPDSP1;

	return rsp_status;
}
static void diag_process_stm_mask_for_peripherals(uint8_t cmd, uint16_t mask)
{
	if (mask & DIAG_STM_MODEM)
		diag_process_stm_mask(cmd, DIAG_STM_MODEM,
						PERIPHERAL_MODEM);

	if (mask & DIAG_STM_LPASS)
		diag_process_stm_mask(cmd, DIAG_STM_LPASS,
						PERIPHERAL_LPASS);

	if (mask & DIAG_STM_WCNSS)
		diag_process_stm_mask(cmd, DIAG_STM_WCNSS,
						PERIPHERAL_WCNSS);

	if (mask & DIAG_STM_SENSORS)
		diag_process_stm_mask(cmd, DIAG_STM_SENSORS,
						PERIPHERAL_SENSORS);
	if (mask & DIAG_STM_CDSP)
		diag_process_stm_mask(cmd, DIAG_STM_CDSP,
						PERIPHERAL_CDSP);
	if (mask & DIAG_STM_NPU)
		diag_process_stm_mask(cmd, DIAG_STM_NPU,
						PERIPHERAL_NPU);

	if (mask & DIAG_STM_APPS)
		diag_process_stm_mask(cmd, DIAG_STM_APPS, PERIPHERAL_APPS);

	if (mask & DIAG_STM_NSP1)
		diag_process_stm_mask(cmd, DIAG_STM_NSP1,
						PERIPHERAL_NSP1);

	if (mask & DIAG_STM_GPDSP0)
		diag_process_stm_mask(cmd, DIAG_STM_GPDSP0,
						PERIPHERAL_GPDSP0);

	if (mask & DIAG_STM_GPDSP1)
		diag_process_stm_mask(cmd, DIAG_STM_GPDSP1,
						PERIPHERAL_GPDSP1);

}

static int handle_diag_stm_cmd(struct diag_client *client,
					const void *buf, size_t len, int pid)
{
	struct diag_stm_cmd_req_t *req;
	uint32_t resp_len;
	uint16_t rsp_supported = 0;
	uint16_t rsp_status = 0;
	uint16_t mask;
	uint8_t version, cmd;
	uint8_t *resp;
	(void)client;
	int i;
	/* suppress unused parameter warning */
	(void)client;
	(void)len;

	req = (struct diag_stm_cmd_req_t *) buf;
	version = req->version;
	if (version == STM_REQ_VERSION_2) {
		mask = req->v2.mask;
		cmd = req->v2.cmd;
	} else if (version == STM_REQ_VERSION_3) {
		mask = req->v3.mask;
		cmd = req->v3.cmd;
	}

	/*
	 * allocate response buffer to handle both failure & success case
	 * request length(len) + rsp_supported(max 2 bytes) + rsp_status(max 2 bytes)
	 */
	resp = alloca(len + (2 * sizeof(uint16_t)));
	if (!resp) {
		ALOGE("diag: %s: stm config response alloc failed\n", __func__);
		return -EMSGSIZE;
	}

	if ((version < STM_REQ_VERSION_2) ||
		(version > STM_REQ_VERSION_3) ||
		(cmd > STM_AUTO_QUERY) ||
		((cmd != STATUS_STM && cmd != STM_AUTO_QUERY) &&
		((mask == 0) || (0 != (mask >> (NUM_PERIPHERALS + 1)))))) {

		/* response for invalid request include error cmd code */
		resp_len = len + 1;

		resp[0] = DIAG_CMD_RSP_BAD_COMMAND;
		for (i = 0; i < len; i++)
			resp[i+1] = *((uint8_t *)buf + i);

		return diag_apps_rsp_send(pid, (unsigned char *)resp, resp_len);

	} else if (cmd != STATUS_STM && cmd != STM_AUTO_QUERY)
		diag_process_stm_mask_for_peripherals(cmd, mask);

	for (i = 0; i < len; i++)
		resp[i] = *((uint8_t *)buf + i);

	rsp_supported = diag_get_stm_peripheral_mask();
	rsp_status = diag_get_stm_state_for_peripherals();

	if (version == STM_REQ_VERSION_2) {
		memcpy((void *)(resp + len), (void *)&rsp_supported, sizeof(uint8_t));
		memcpy((void *)(resp + len + sizeof(uint8_t)), (void *)&rsp_status,
			sizeof(uint8_t));
		resp_len = len + (2 * sizeof(uint8_t));
	} else {
		memcpy((void *)(resp + len), (void *)&rsp_supported, sizeof(uint16_t));
		memcpy((void *)(resp + len + sizeof(uint16_t)), (void *)&rsp_status,
			sizeof(uint16_t));
		resp_len = len + (2 * sizeof(uint16_t));
	}

	return diag_apps_rsp_send(pid, (unsigned char *)resp, resp_len);

}

static int handle_diag_query_transport(struct diag_client *client,
					const void *buf, size_t len, int pid)
{
	struct diag_query_transport_req_t *req;
	struct diag_query_transport_rsp_t rsp;
	/* suppress unused parameter warning */
	(void)client;
	(void)len;

	req = (struct diag_query_transport_req_t *)buf;
	rsp.header.cmd_code = req->header.cmd_code;
	rsp.header.subsys_id = req->header.subsys_id;
	rsp.header.subsys_cmd_code = req->header.subsys_cmd_code;
	rsp.transport = diagmem->transport_set;

	return diag_apps_rsp_send(pid, (unsigned char *)&rsp, sizeof(rsp));
}

static int handle_diag_all_mask_clear(struct diag_client *client,
					const void *buf, size_t len, int pid)
{
	struct diag_all_mask_clear_req_t *req;
	struct diag_all_mask_clear_rsp_t rsp;
	(void)len;

	req = (struct diag_all_mask_clear_req_t *)buf;
	rsp.header.cmd_code = req->header.cmd_code;
	rsp.header.subsys_id = req->header.subsys_id;
	rsp.header.subsys_cmd_code = req->header.subsys_cmd_code;
	rsp.version = req->version;
	rsp.status = diag_send_all_mask_clear(client, pid);

	return diag_apps_rsp_send(pid, (unsigned char *)&rsp, sizeof(rsp));
}

static int handle_keep_alive(struct diag_client *client, const void *buf,
							size_t len, int pid)
{
	struct diag_cmd_diag_keep_alive_rsp resp;
	/* suppress unused parameter warning */
	(void)client;
	(void)buf;
	(void)len;

	resp.header.cmd_code = DIAG_CMD_SUBSYS_DISPATCH;
	resp.header.subsys_id = DIAG_CMD_KEEP_ALIVE_SUBSYS;
	resp.header.subsys_cmd_code = DIAG_CMD_KEEP_ALIVE_CMD;
	resp.proc_id = 0;
	resp.status = 0;
	resp.reserved = 0;

	return diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
}

uint16_t diag_get_next_delayed_rsp_id(void)
{
	uint16_t rsp_id = 0;

	rsp_id = diagmem->delayed_rsp_id;
	if (rsp_id < DIAGPKT_MAX_DELAYED_RSP) {
 		rsp_id++;
 	} else {
 		if (wrap_enabled) {
 			rsp_id = 1;
 			wrap_count++;
 		} else
 			rsp_id = DIAGPKT_MAX_DELAYED_RSP;
 	}
 	diagmem->delayed_rsp_id = rsp_id;

 	return rsp_id;
}

static int handle_del_rsp_wrap(struct diag_client *client, const void *buf,
							size_t len, int pid)
{
	struct
	{
		struct diag_pkt_header_t header;
		uint8_t wrap_enabled;
	}__packed resp;
	/* suppress unused parameter warning */
	(void)client;
	(void)buf;
	(void)len;

	resp.header.cmd_code = DIAG_CMD_SUBSYS_DISPATCH;
	resp.header.subsys_id = DIAG_CMD_KEEP_ALIVE_SUBSYS;
	resp.header.subsys_cmd_code = DIAG_CMD_DEL_RSP_WRAP;
	resp.wrap_enabled = wrap_enabled;

	return diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
}

static int handle_del_rsp_wrap_cnt(struct diag_client *client, const void *buf,
							size_t len, int pid)
{
	struct
	{
		struct diag_pkt_header_t header;
		uint16_t wrap_count;
	}__packed resp;
	/* suppress unused parameter warning */
	(void)client;
	(void)buf;
	(void)len;

	resp.header.cmd_code = DIAG_CMD_SUBSYS_DISPATCH;
	resp.header.subsys_id = DIAG_CMD_KEEP_ALIVE_SUBSYS;
	resp.header.subsys_cmd_code = DIAG_CMD_DEL_RSP_WRAP_CNT;
	resp.wrap_count = wrap_count;

	return diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
}

static int handle_mobile_id(struct diag_client *client, const void *buf,
			size_t len, int pid)
{
	struct {
		uint8_t cmd_code;
		uint8_t subsys_id;
		uint16_t subsys_cmd_code;
		uint8_t ver;
		uint8_t reserved[3];
		uint32_t family;
		uint32_t chip_id;
	} __packed *resp;
	size_t resp_size;
	char soc_buf[SOC_BUF_SIZE] = {0};
	int chip_id, fd = -1, bytes_read;
	/* suppress unused parameter warning */
	(void)client;
	(void)buf;
	(void)len;

	fd = open("/sys/devices/soc0/soc_id", O_RDONLY);
	if (fd < 0)
		return -EMSGSIZE;
	bytes_read = read(fd, soc_buf, SOC_BUF_SIZE - 1);
	if (bytes_read < 0) {
		close(fd);
		return -EMSGSIZE;
	}
	close(fd);
	soc_buf[SOC_BUF_SIZE - 1] = 0;
	chip_id = atoi(soc_buf);
	resp_size = sizeof(*resp);
	resp = alloca(resp_size);
	if (!resp)
		return -EMSGSIZE;
	memset(resp, 0, resp_size);
	resp->cmd_code = DIAG_CMD_SUBSYS_DISPATCH;
	resp->subsys_id = DIAG_CMD_KEEP_ALIVE_SUBSYS;
	resp->subsys_cmd_code = DIAG_CMD_EXT_MOBILE_ID;
	resp->ver = 2;
	resp->family = 0;
	resp->chip_id = chip_id;
	return diag_apps_rsp_send(pid, (unsigned char *)resp, resp_size);
}

static int handle_diag_id(struct diag_client *client, const void *buf,
			     size_t len, int pid)
{
	struct diag_cmd_diag_id_query_req_t *req = NULL;
	struct diag_cmd_diag_id_query_rsp_t *resp = NULL;
	struct list_head *start;
	struct diag_id_tbl_t *item = NULL;
	size_t resp_size = DIAG_MAX_RSP_SIZE;
	int num_entries = 0;
	uint8_t process_name_len = 0;
	uint8_t *dest_buf;
	size_t write_len;
	/* suppress unused parameter warning */
	(void)client;

	if (!buf || len < sizeof(struct diag_cmd_diag_id_query_req_t)) {
		return -EMSGSIZE;
	}
	req = (struct diag_cmd_diag_id_query_req_t *) buf;
	resp = alloca(resp_size);
	if (!resp) {
		return -EMSGSIZE;
	}
	dest_buf = (uint8_t *) resp;
	resp->header.cmd_code = req->header.cmd_code;
	resp->header.subsys_id = req->header.subsys_id;
	resp->header.subsys_cmd_code = req->header.subsys_cmd_code;
	resp->version = req->version;
	resp->entry.process_name = NULL;
	resp->entry.len = 0;
	resp->entry.diag_id = 0;
	write_len = sizeof(resp->header) + sizeof(resp->version) +
			sizeof(resp->num_entries);
	resp_size = write_len;

	list_for_each(start, &diag_id_list) {
		item = list_entry(start, struct diag_id_tbl_t, link);
		memcpy(dest_buf + write_len, &item->diag_id,
			sizeof(item->diag_id));
		write_len = write_len + sizeof(item->diag_id);
		process_name_len = strlen(item->process_name) + 1;
		memcpy(dest_buf + write_len, &process_name_len,
			sizeof(process_name_len));
		write_len = write_len + sizeof(process_name_len);
		memcpy(dest_buf + write_len, item->process_name,
			strlen(item->process_name) + 1);
		write_len = write_len + strlen(item->process_name) + 1;
		num_entries++;
	}

	resp->num_entries = num_entries;

	return diag_apps_rsp_send(pid, (unsigned char *)resp, write_len);
}

int diag_timestamp_switch_update_client(struct diag_client *dm)
{
        struct timestamp_switch_pkt *pkt;

        pkt = malloc(sizeof(struct timestamp_switch_pkt));
        if (!pkt) {
                ALOGE("In %s failed to alloc memory\n", __func__);
                return 0;
        }
        pkt->type = DIAG_TIMESTAMP_SWITCH;
        pkt->timestamp_switch = send_timestamp_switch;
        dm_send(dm, pkt, sizeof(struct timestamp_switch_pkt));
	free(pkt);

        return 0;
}

static int handle_diag_time_sync_switch(struct diag_client *client, const void *buf,
											size_t len, int pid)
{
	struct timestamp_switch_pkt *pkt;
	struct diag_cmd_time_sync_switch_req_t *req = NULL;
	struct diag_cmd_time_sync_switch_rsp_t *rsp = NULL;
	/* suppress unused parameter warning */
	(void)client;

	if (!buf || len < sizeof(struct diag_cmd_time_sync_switch_req_t)) {
		return -EMSGSIZE;
	}
	req = (struct diag_cmd_time_sync_switch_req_t *)buf;
	if ((req->version > 1) || (req->time_api > 1) ||
		(req->persist_time > 0)) {
		diagmem->timestamp_switch = 0;
		return -EINVAL;
	}
	rsp = alloca(sizeof(*rsp));
	if (!rsp) {
		return -EMSGSIZE;
	}
	rsp->header.cmd_code = req->header.cmd_code;
	rsp->header.subsys_id = req->header.subsys_id;
	rsp->header.subsys_cmd_code = req->header.subsys_cmd_code;
	rsp->version = req->version;
	rsp->time_api = req->time_api;
	rsp->time_api_status = 0;
	rsp->persist_time_status = 2;
	diagmem->timesync_enabled = 1;
	diagmem->time_api = req->time_api;
	switch (req->time_api) {
	case 0:
		diagmem->timestamp_switch = 0;
		break;
	case 1:
		diagmem->timestamp_switch = 1;
		break;
	default:
		diagmem->timestamp_switch = 0;
		break;
	}
	pkt = malloc(sizeof(struct timestamp_switch_pkt));
	if (!pkt) {
		ALOGE("In %s failed to alloc memory\n", __func__);
		goto periph_send;
	}

	pkt->type = DIAG_TIMESTAMP_SWITCH;
	pkt->timestamp_switch = diagmem->timestamp_switch;
	send_timestamp_switch = diagmem->timestamp_switch;

	#ifndef FEATURE_LE_DIAG
	diag_update_hidl_client(TIMESTAMP_SWITCH_TYPE);
	#endif /* FEATURE_LE_DIAG */

	dm_broadcast_to_socket_clients(pkt, sizeof(struct timestamp_switch_pkt));
	free(pkt);
	pkt = NULL;

periph_send:
	peripheral_broadcast_time_switch(req->time_api);

	return diag_apps_rsp_send(pid, (unsigned char *)rsp, sizeof(*rsp));

}

int diag_timestamp_switch_update_hidl_client(unsigned char *data)
{
	int data_type = TIMESTAMP_SWITCH_TYPE;
	int timestamp_switch = send_timestamp_switch;
	int len = sizeof(timestamp_switch) + sizeof(data_type);

	memcpy(data, &data_type, sizeof(data_type));
	memcpy(data + sizeof(data_type), &timestamp_switch, sizeof(timestamp_switch));

	return len;
}

static int handle_diag_time_sync_query(struct diag_client *client, const void *buf,
					size_t len, int pid)
{
	struct diag_cmd_time_sync_query_req_t *req = NULL;
	struct diag_cmd_time_sync_query_rsp_t *rsp = NULL;
	/* suppress unused parameter warning */
	(void)client;

	if (!buf || len < sizeof(struct diag_cmd_time_sync_query_req_t)) {
		return -EMSGSIZE;
	}

	req = (struct diag_cmd_time_sync_query_req_t *)buf;
	rsp = alloca(sizeof(*rsp));
	if (!rsp) {
		return -EMSGSIZE;
	}
	rsp->header.cmd_code = req->header.cmd_code;
	rsp->header.subsys_id = req->header.subsys_id;
	rsp->header.subsys_cmd_code = req->header.subsys_cmd_code;
	rsp->version = req->version;
	rsp->time_api = diagmem->time_api;

	return diag_apps_rsp_send(pid, (unsigned char *)rsp, sizeof(*rsp));
}

static int handle_diag_feature_query(struct diag_client *client, const void *buf,
					size_t len, int pid)
{
	struct diag_cmd_diag_feature_query_req_t *req = NULL;
	struct diag_cmd_diag_feature_query_rsp_t *rsp;
	/* suppress unused parameter warning */
	(void)client;

	if (!buf || len < sizeof(struct diag_cmd_diag_feature_query_req_t)) {
		return -EMSGSIZE;
	}
	req = (struct diag_cmd_diag_feature_query_req_t*) buf;
	rsp = alloca(sizeof(*rsp));
	if (!rsp) {
		return -EMSGSIZE;
	}
	rsp->header.cmd_code = req->header.cmd_code;
	rsp->header.subsys_id = req->header.subsys_id;
	rsp->header.subsys_cmd_code = req->header.subsys_cmd_code;
	rsp->version = 1;
	rsp->feature_len = sizeof(rsp->feature_mask);
	memcpy(rsp->feature_mask,&diagmem->apps_feature, sizeof(rsp->feature_mask));

	return diag_apps_rsp_send(pid, (unsigned char *)rsp, sizeof(*rsp));

}
static int handle_diag_hdlc_disable_cmd(struct diag_client *client, const void *buf,
					size_t len, int pid)
{
	struct diag_cmd_hdlc_disable_req_t *req = NULL;
	struct diag_cmd_hdlc_disable_rsp_t *rsp;
	struct diag_md_session_t *session_info = NULL;
	int peripheral, i, ret;

	if (!buf || len < sizeof(*req))
		return -EMSGSIZE;

	req = (struct diag_cmd_hdlc_disable_req_t *)buf;
	rsp = alloca(sizeof(*rsp));
	if (!rsp)
		return -EMSGSIZE;

	rsp->header.cmd_code = req->header.cmd_code;
	rsp->header.subsys_id = req->header.subsys_id;
	rsp->header.subsys_cmd_code = req->header.subsys_cmd_code;
	rsp->framing_version = 1;
	rsp->result = 0;
	ret = diag_apps_rsp_send(pid, (unsigned char *)rsp, sizeof(*rsp));
	pthread_mutex_lock(&diagmem->hdlc_disable_mutex);
	session_info = diag_md_session_get_pid(pid);
	if (session_info)
		session_info->hdlc_disabled = 1;
	else
		diagmem->hdlc_disabled = 1;

	peripheral = diag_md_session_match_pid_peripheral(DIAG_LOCAL_PROC,
							  pid, 0);

	for (i = 0; i < NUM_MD_SESSIONS; i++) {
		if (peripheral > 0 && session_info) {
			if (peripheral & (1 << i))
				diagmem->p_hdlc_disabled[i] = session_info->hdlc_disabled;
			else if (!diag_md_session_get_peripheral(DIAG_LOCAL_PROC, i))
				diagmem->p_hdlc_disabled[i] = diagmem->hdlc_disabled;
		} else {
			if (!diag_md_session_get_peripheral(DIAG_LOCAL_PROC, i))
				diagmem->p_hdlc_disabled[i] = diagmem->hdlc_disabled;
		}
	}
	pthread_mutex_unlock(&diagmem->hdlc_disable_mutex);
	return ret;
}

static int handle_diag_clear_masks(struct diag_client *client, const void *buf,
					size_t len, int pid)
{
	struct diag_cmd_diag_clear_mask_req *req;
	struct diag_cmd_diag_clear_mask_rsp resp;
	/* suppress unused parameter warning */
	(void)client;

	if (!buf || len < sizeof(struct diag_cmd_diag_clear_mask_req)) {
		return -EMSGSIZE;
	}

	req = (struct diag_cmd_diag_clear_mask_req *)buf;
	resp.header.cmd_code = req->header.cmd_code;
	resp.header.subsys_id = req->header.subsys_id;
	resp.header.subsys_cmd_code = req->header.subsys_cmd_code;
	resp.version = req->version;

	diag_set_clear_masks_flags(req->set_clear_mask_flag);
	resp.status = req->set_clear_mask_flag;

	return diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
}

/**
 * Tool will send this request to use diag-id based logging or cmd req/resp.
 * PVM will broadcast this to all avilable VM peripherals
 * Pkt format mask supports below options
 *  PKT_FORMAT_MASK_CMD_REQ_RESP	- 0x1
 *  PKT_FORMAT_MASK_ASYNC_PKT		- 0x2
 */
static int handle_diag_pkt_format_configure(struct diag_client *client,
                                const void *buf, size_t len, int pid)
{
	struct diag_cmd_pkt_format_select_req *req;
	struct diag_cmd_pkt_format_select_rsp resp;
	(void) client;

	if (!buf || len < sizeof(*req)) {
		ALOGE("diag: %s: Invalid cmd received\n", __func__);
		return -EMSGSIZE;
	}

	req = (struct diag_cmd_pkt_format_select_req *)buf;
	memcpy((void*)&resp, (void*)req, sizeof(*req));
	resp.status = 0;

	ALOGM(DIAG_DBG_MASK_INFO, "diag: %s: New pkt format requested mask 0x%x version %d\n",
		__func__, req->pkt_format_mask, req->version);

	if (req->pkt_format_mask & PKT_FORMAT_INVALID_MASK ||
		req->version != PKT_FORMAT_SELECT_REQ_VERSION1) {
		resp.status = 1;
		ALOGE("diag: %s: Invalid request, version:%d, mask:0x%x\n",
			__func__, req->version, req->pkt_format_mask);
	} else {
		/**
		 * save the pkt format selection request info to share with gvm
		 * if any gvm comes up later
		 */
		pkt_format_select_req_save(req->pkt_format_mask);
		peripheral_broadcast_pkt_selection_req(req->pkt_format_mask);
	}

	return diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
}

static int handle_diag_id_priority_config_request(struct diag_client *client,
				const void *buf, size_t len, int pid)
{
	struct diag_cmd_diagid_priority_confg_req *req;
	struct diag_cmd_diagid_priority_confg_rsp resp;
	(void)client;

	if (!buf || len < sizeof(*req)) {
		ALOGE("diag: %s: Invalid cmd received\n", __func__);
		return -EMSGSIZE;
	}

	req = (struct diag_cmd_diagid_priority_confg_req *)buf;
	memcpy((void*)&resp, (void*)req, sizeof(*req));
	resp.status = 0;

	if (req->version != PRIORITY_DIAGID_CONFG_REQ_VER1) {
		resp.status = CMD_ERR_INVALID_VERSION;
		ALOGE("diag: %s: Invalid version %d recvd\n", __func__, req->version);
		return diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
	}

	switch(req->opcode)
	{
	case PRIORITY_DIAGID_GET:
		resp.diag_id = diagmem->priority_diagid;
		break;
	case PRIORITY_DIAGID_SET:
		if (diag_check_diag_id_valid(req->diag_id))
			diagmem->priority_diagid = req->diag_id;
		else
			resp.status = CMD_ERR_INVALID_DIAGID;
		break;
	case PRIORITY_DIAGID_CLEAR:
		diagmem->priority_diagid = 0;
		resp.diag_id = diagmem->priority_diagid;
		break;
	default:
		resp.status = CMD_ERR_INVALID_OPCODE;
		break;
	}

	return diag_apps_rsp_send(pid, (unsigned char *)&resp, sizeof(resp));
}

static int handle_diag_cmd_table_retrieval(struct diag_client *client, const void *buf,
					size_t len, int pid)
{
	struct diag_cmd_registration_table_req *req;
	(void) client;

	if (!buf || len < sizeof(*req)) {
		ALOGE("diag: %s: Invalid command recvd\n", __func__);
		return -EMSGSIZE;
	}

	req = (struct diag_cmd_registration_table_req*)buf;
	ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: cmd 0x%x sub_id 0x%x sub_cmd_code 0x%x ver %d opcode %d diag-id %d\n",
		__func__, req->header.cmd_code, req->header.subsys_id, req->header.subsys_cmd_code,
		req->version, req->opcode, req->diag_id);

	return diag_send_cmd_reg_table(req, pid);
}

void register_app_cmds(void)
{
	register_fallback_cmd(DIAG_CMD_DIAG_VERSION_ID, handle_diag_version);
	register_fallback_cmd(DIAG_CMD_DIAG_VERSION_NO, handle_diag_version_no);
	register_fallback_cmd(DIAG_CMD_DIAG_LOG_ON_DEMAND, handle_diag_log_on_demand);
	register_fallback_cmd(DIAG_CMD_EXTENDED_BUILD_ID, handle_extended_build_id);
	register_fallback_subsys_cmd(DIAG_CMD_KEEP_ALIVE_SUBSYS,
					DIAG_CMD_KEEP_ALIVE_CMD, handle_keep_alive);
	register_fallback_subsys_cmd(DIAG_CMD_KEEP_ALIVE_SUBSYS,
					DIAG_CMD_EXT_MOBILE_ID, handle_mobile_id);
	register_fallback_subsys_cmd(DIAG_CMD_KEEP_ALIVE_SUBSYS,
					DIAG_CMD_DEL_RSP_WRAP, handle_del_rsp_wrap);
	register_fallback_subsys_cmd(DIAG_CMD_KEEP_ALIVE_SUBSYS,
					DIAG_CMD_DEL_RSP_WRAP_CNT, handle_del_rsp_wrap_cnt);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_DIAG_GET_DIAG_ID, handle_diag_id);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_DIAG_TIME_SYNC_QUERY, handle_diag_time_sync_query);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_DIAG_TIME_SYNC_SWITCH, handle_diag_time_sync_switch);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_DIAG_FEATURE_QUERY, handle_diag_feature_query);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_DIAG_STM, handle_diag_stm_cmd);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_QUERY_TRANSPORT, handle_diag_query_transport);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_OP_HDLC_DISABLE, handle_diag_hdlc_disable_cmd);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_CLEAR_MASKS, handle_diag_clear_masks);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_ALL_MASK_CLEAR, handle_diag_all_mask_clear);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_PKT_FORMAT_SELECT_REQUEST, handle_diag_pkt_format_configure);
	register_fallback_subsys_cmd(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_PRIORITY_DIAGID_CONFIG_REQUEST, handle_diag_id_priority_config_request);
	register_fallback_subsys_cmd_v2(DIAG_CMD_DIAG_SUBSYS,
					DIAG_CMD_REGISTRATION_TABLE_REQUEST, handle_diag_cmd_table_retrieval);
}
