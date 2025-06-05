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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "diag.h"
#include "diag_cntl.h"
#include "masks.h"
#include "peripheral.h"
#include "util.h"
#include "dm.h"
#include "vm.h"

/* strlcpy is from OpenBSD and not supported by Linux Embedded.
 * GNU has an equivalent g_strlcpy implementation into glib.
 * Featurized with compile time USE_GLIB flag for Linux Embedded builds.
 */

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

extern int diag_subid_info[];

#define QUERY_PD_FEATURE_MASK_MASK_TYPE 0x8000
struct list_head diag_settings_list = LIST_INIT(diag_settings_list);
static void diag_cntl_send_diag_settings(struct peripheral *peripheral);
static void diag_send_hw_accel_status(struct peripheral *peripheral);
struct peripheral * diag_get_periph_info(int periph_id)
{
	struct peripheral *peripheral;
	struct list_head *item;

	list_for_each(item, &peripherals) {
		peripheral = container_of(item, struct peripheral, node);
		if (peripheral->periph_id == periph_id)
			return peripheral;
	}
	return NULL;
}

static int diag_cntl_register(struct peripheral *peripheral,
			      struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_reg *pkt = to_cmd_reg(hdr);
	struct diag_cmd *dc;
	unsigned int subsys;
	unsigned int cmd;
	unsigned int first;
	unsigned int last;
	int i;

	if (!pkt || len == 0)
		return -EINVAL;

	for (i = 0; i < pkt->count_entries; i++) {
		cmd = pkt->cmd;
		subsys = pkt->subsys;

		if (cmd == DIAG_CMD_NO_SUBSYS_DISPATCH && subsys != DIAG_CMD_NO_SUBSYS_DISPATCH)
			cmd = DIAG_CMD_SUBSYS_DISPATCH;

		first = cmd << 24 | subsys << 16 | pkt->ranges[i].first;
		last = cmd << 24 | subsys << 16 | pkt->ranges[i].last;

		// printf("[%s] register 0x%x - 0x%x\n",
		//	  peripheral->name, first, last);

		dc = malloc(sizeof(*dc));
		if (!dc) {
			warn("malloc failed\n");
			return -ENOMEM;
		}
		memset(dc, 0, sizeof(*dc));

		dc->first = first;
		dc->last = last;
		dc->peripheral = peripheral;

		list_add(&diag_cmds, &dc->node);
	}

	return 0;
}

void diag_stm_init()
{
	int periph_id;

	for(periph_id = 0; periph_id < NUM_PERIPHERALS; periph_id++)
	{
		diagmem->stm_support[periph_id] = 0;
		diagmem->stm_state[periph_id] = 0;
		diagmem->stm_state_requested[periph_id] = 0;
	}
}

static void diag_enable_stm_feature(struct peripheral *peripheral)
{
	if (!peripheral)
		return;

	diagmem->stm_support[peripheral->periph_id] = 1;
}

int diag_cntl_send_stm_state(uint32_t periph_id, uint8_t control_data)
{
	struct diag_cntl_cmd_diag_stm_state *ctrl_pkt = NULL;
	int len = sizeof(struct diag_cntl_cmd_diag_stm_state);
	struct peripheral *peripheral =  diag_get_periph_info(periph_id);

	if (!peripheral)
		return -EINVAL;

	ctrl_pkt = malloc(len);
	if (!ctrl_pkt) {
		printf("diag: Failed to allocate memory for stm ctrl pkt\n");
		return -ENOMEM;
	}
	ctrl_pkt->hdr.cmd = DIAG_CNTL_CMD_STM_STATE;
	ctrl_pkt->hdr.len = len - sizeof(ctrl_pkt->hdr);
	ctrl_pkt->version = 1;
	ctrl_pkt->control_data = control_data;
	queue_push_cntlq(peripheral, ctrl_pkt, len);
	free(ctrl_pkt);
	ctrl_pkt = NULL;

	return 0;
}

static int diag_cntl_feature_mask(struct peripheral *peripheral,
					struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_feature *pkt = to_cmd_feature(hdr);
	uint32_t local_mask = 0;
	uint32_t mask;

	if (!pkt || len == 0 || !peripheral)
		return -EINVAL;

	if (peripheral->received_feature_mask) {
		ALOGE("diag:%s: Feature mask already received from %s\n",
			__func__, peripheral->name);
		return -EPERM;
	}

	peripheral->received_feature_mask = 1;
	mask = pkt->mask;

	diagmem->feature[peripheral->periph_id].feature_mask = mask;

	local_mask |= DIAG_FEATURE_FEATURE_MASK_SUPPORT;
	local_mask |= DIAG_FEATURE_DIAG_MASTER_SETS_COMMON_MASK;
	if (peripheral->cmd_fd >= 0)
		local_mask |= DIAG_FEATURE_REQ_RSP_SUPPORT;
	local_mask |= DIAG_FEATURE_APPS_HDLC_ENCODE;
	if (peripheral->sockets)
		local_mask |= DIAG_FEATURE_SOCKETS_ENABLED;
	local_mask |= DIAG_FEATURE_DIAG_ID;
	local_mask |= DIAG_FEATURE_DIAG_ID_FEATURE_MASK;
	local_mask |= DIAG_FEATURE_PERIPHERAL_BUFFERING;
	local_mask |= DIAG_FEATURE_PD_BUFFERING;
	local_mask |= DIAG_FEATURE_DCI_EXTENDED_HEADER;
	local_mask |= DIAG_FEATURE_DIAG_MULTI_SIM_SUPPORT;
	local_mask |= DIAG_FEATURE_PKT_HEADER_UNTAG;
	local_mask |= DIAG_FEATURE_DIAG_ID_LOGGING;

	ALOGI("diag: In %s received feature mask from [%s] mask\n",__func__, peripheral->name);

	if (mask & DIAG_FEATURE_FEATURE_MASK_SUPPORT)
		printf(" FEATURE_MASK_SUPPORT");
	if (mask & DIAG_FEATURE_DIAG_MASTER_SETS_COMMON_MASK)
		printf(" DIAG_MASTER_SETS_COMMON_MASK");
	if (mask & DIAG_FEATURE_LOG_ON_DEMAND_APPS)
		printf(" LOG_ON_DEMAND");
	if (mask & DIAG_FEATURE_DIAG_VERSION_RSP_ON_MASTER)
		printf(" DIAG_VERSION_RSP_ON_MASTER");
	if (mask & DIAG_FEATURE_REQ_RSP_SUPPORT)
		printf(" REQ_RSP");
	if (mask & DIAG_FEATURE_APPS_HDLC_ENCODE)
		printf(" APPS_HDLC_ENCODE");
	if (mask & DIAG_FEATURE_STM){
		diag_enable_stm_feature(peripheral);
		printf(" STM");
	}
	if (mask & DIAG_FEATURE_PERIPHERAL_BUFFERING) {
		printf(" PERIPHERAL-BUFFERING");
		diagmem->feature[peripheral->periph_id].peripheral_buffering = 1;
	}
	if (mask & DIAG_FEATURE_PD_BUFFERING) {
		printf(" PERIPHERAL-BUFFERING");
		diagmem->feature[peripheral->periph_id].pd_buffering = 1;
	}
	if (mask & DIAG_FEATURE_MASK_CENTRALIZATION)
		printf(" MASK-CENTERALIZATION");
	if (mask & DIAG_FEATURE_SOCKETS_ENABLED)
		printf(" SOCKETS");
	if (mask & DIAG_FEATURE_DIAG_ID)
		printf(" DIAG-ID");
	if (mask & DIAG_FEATURE_DIAG_ID_FEATURE_MASK) {
		printf(" DIAG-ID-FEATURE-MASK");
		diagmem->feature[peripheral->periph_id].diagid_v2_feature_mask = 1;
	}
	if (mask & DIAG_FEATURE_DCI_EXTENDED_HEADER) {
		printf(" DCI EXTENDED HEADER");
	}
	if (mask & DIAG_FEATURE_DIAG_MULTI_SIM_SUPPORT) {
		printf(" DIAG_MULTI_SIM_SUPPORT");
		diagmem->feature[peripheral->periph_id].multi_sim_support= 1;
	}
	if (mask & DIAG_FEATURE_PKT_HEADER_UNTAG) {
		printf(" PD LOGGING SEPARATION");
		diagmem->feature[peripheral->periph_id].untag_header = ENABLE_PKT_HEADER_UNTAGGING;
	}
	if (mask & DIAG_FEATURE_DIAG_ID_LOGGING) {
		printf(" DIAG_FEATURE_DIAG_ID_LOGGING");
		diagmem->feature[peripheral->periph_id].diag_id_logging = 1;
	}

	printf(" (0x%x)\n", mask);

	peripheral->features = mask & local_mask;

	diag_cntl_send_feature_mask(peripheral, peripheral->features);

	/**
	 * vm diag-router instace should not try to send masks since
	 * it is running in peripheral mode
	 */
	if (!(mask & DIAG_FEATURE_DIAG_ID) && !vm_enabled) {
		ALOGI("diag: %s: Sending masks to peripheral (%d) not supporting diagid\n", __func__, peripheral->periph_id);
		diag_cntl_send_masks(peripheral);
	}

	/* vm diag-router has to query diag-id from pvm */
	if (vm_enabled) {
		ALOGI("diag: %s: Sending diag id query for %s\n", __func__, peripheral->self_name);
		diag_vm_query_diag_id(peripheral, peripheral->self_name);
	}

	return 0;
}
/*
 * diag_query_pd_name(char *process_name, char *search_str)
 *
 * The function returns the PD information based on the presence of
 * the pd specific string in the control packet's string from peripheral.
 *
 */
static int diag_query_pd_name(char *process_name, char *search_str)
{
	if (!process_name)
		return -EINVAL;

	if (strstr(process_name, search_str))
		return 1;

	return 0;
}

int diag_query_pd(char *process_name)
{
	if (!process_name)
		return -EINVAL;

	if (diag_query_pd_name(process_name, "apps"))
		return PERIPHERAL_APPS;
	if (diag_query_pd_name(process_name, "modem/root_pd"))
		return PERIPHERAL_MODEM;
	if (diag_query_pd_name(process_name, "adsp/root_pd"))
		return PERIPHERAL_LPASS;
	if (diag_query_pd_name(process_name, "wpss/root_pd"))
		return PERIPHERAL_WCNSS;
	if (diag_query_pd_name(process_name, "slpi/root_pd"))
		return PERIPHERAL_SENSORS;
	if (diag_query_pd_name(process_name, "cdsp/root_pd"))
		return PERIPHERAL_CDSP;
	if (diag_query_pd_name(process_name, "npu/root_pd"))
		return PERIPHERAL_NPU;
	if (diag_query_pd_name(process_name, "cdsp1/root_pd"))
		return PERIPHERAL_NSP1;
	if (diag_query_pd_name(process_name, "gpdsp/root_pd"))
		return PERIPHERAL_GPDSP0;
	if (diag_query_pd_name(process_name, "gpdsp1/root_pd"))
		return PERIPHERAL_GPDSP1;
	if (diag_query_pd_name(process_name, "soccp/root_pd"))
		return PERIPHERAL_SOCCP;
	if (diag_query_pd_name(process_name, "wlan_pd"))
		return UPD_WLAN;
	if (diag_query_pd_name(process_name, "audio_pd"))
		return UPD_AUDIO;
	if (diag_query_pd_name(process_name, "sensor_pd"))
		return UPD_SENSORS;
	if (diag_query_pd_name(process_name, "charger_pd"))
		return UPD_CHARGER;
	if (diag_query_pd_name(process_name, "oem_pd"))
		return UPD_OEM;
	if (diag_query_pd_name(process_name, "ois_pd"))
		return UPD_OIS;
	if (diag_query_pd_name(process_name, "tele-gvm"))
		return PERIPHERAL_TELE_GVM;
	if (diag_query_pd_name(process_name, "fota-gvm"))
		return PERIPHERAL_FOTA_GVM;

	return PD_UNKNOWN;
}

int diag_add_diag_id_to_list(uint8_t diag_id, char *process_name,
	uint8_t pd_val, struct peripheral *perif)
{
	struct diag_id_tbl_t *new_item = NULL;
	int process_len = 0;

	if (!process_name || diag_id == 0)
		return -EINVAL;
	new_item = malloc(sizeof(struct diag_id_tbl_t));
	if (!new_item)
		return -ENOMEM;
	process_len = strlen(process_name);
	new_item->process_name = malloc(process_len + 1);
	if (!new_item->process_name) {
		free(new_item);
		new_item = NULL;
		return -ENOMEM;
	}
	new_item->diag_id = diag_id;
	new_item->pd_val = pd_val;
	new_item->periph_id = perif->periph_id;
	strlcpy(new_item->process_name, process_name, process_len + 1);
	list_add(&diag_id_list, &new_item->link);

	return 0;
}

int diag_query_diag_id(char *process_name, uint8_t *diag_id)
{
	struct list_head *start;
	struct diag_id_tbl_t *item = NULL;

	if (!process_name || !diag_id)
		return -EINVAL;

	list_for_each(start, &diag_id_list) {
		item = list_entry(start, struct diag_id_tbl_t, link);
		if (strcmp(item->process_name, process_name) == 0) {
			*diag_id = item->diag_id;
			return 1;
		}
	}

	return 0;
}

/**
 * This returns last diag-id saved in the diag_id_list, last diag-id
 * will be the highest value of diag-id since diag-id's are assigning
 * in incremental order starting from 1
 */
int diag_query_last_diag_id(void)
{
	struct diag_id_tbl_t *item = NULL;
	struct list_head *start;

	list_for_each(start, &diag_id_list) {
		item = list_entry(start, struct diag_id_tbl_t, link);
	}

	if(!item)
		return 0;

	return item->diag_id;
}

static void diag_add_fmask_to_diagid_table(uint8_t diag_id,
	uint32_t pd_feature_mask)
{
	struct list_head *start;
	struct diag_id_tbl_t *item = NULL;

	if (!diag_id)
		return;

	list_for_each(start, &diag_id_list) {
		item = list_entry(start, struct diag_id_tbl_t, link);
		if (diag_id == item->diag_id) {
			item->pd_feature_mask = pd_feature_mask;
			return;
		}
	}
}
void diag_map_index_to_hw_accel(uint8_t index,
	uint8_t *hw_accel_type, uint8_t *hw_accel_ver)
{
	*hw_accel_type = 0;
	*hw_accel_ver = 0;

	switch (index) {
	case DIAG_HW_ACCEL_TYPE_STM:
		*hw_accel_type = DIAG_HW_ACCEL_TYPE_STM;
		*hw_accel_ver = DIAG_HW_ACCEL_VER_MIN;
		break;
	case DIAG_HW_ACCEL_TYPE_ATB:
		*hw_accel_type = DIAG_HW_ACCEL_TYPE_ATB;
		*hw_accel_ver = DIAG_HW_ACCEL_VER_MIN;
		break;
	default:
		break;
	}
}
void process_diagid_v2_feature_mask(uint32_t diag_id,
		uint32_t pd_feature_mask)
{
	int i = 0;
	uint32_t diagid_mask_bit = 0, feature_id_mask = 0;
	uint8_t hw_accel_type = 0, hw_accel_ver = 0;

	if (!pd_feature_mask)
		return;
	diagid_mask_bit = 1 << (diag_id - 1);
	for (i = 0; i < DIAGID_V2_FEATURE_COUNT; i++) {
		feature_id_mask = (pd_feature_mask & (1 << i));
		if (feature_id_mask){
			diagmem->diagid_v2_feature[i] |= diagid_mask_bit;
		}
		feature_id_mask = 0;

		diag_map_index_to_hw_accel(i, &hw_accel_type, &hw_accel_ver);
		if (hw_accel_type && hw_accel_ver)
			diagmem->diag_hw_accel[i] = 1;
	}
}
static int diag_cntl_process_diag_id(struct peripheral *peripheral,
				  struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_diag_id *pkt = to_cmd_diag_id(hdr);
	struct diag_cntl_cmd_diag_id *ctrl_pkt;
	struct diag_cntl_cmd_diag_id_v2 *pkt_v2 = to_cmd_diag_id_v2(hdr);
	struct pd_info *tmp_pd_info = NULL;
	uint32_t version = 0, feature_len = 0;
	uint32_t pd_feature_mask = 0;
	uint8_t diagid_v2_feature_mask = 0;
	char *process_name = NULL;
	int pd_val, ret, i;
	char *root_str = NULL;
	char *gvm_str = NULL;
	uint8_t local_diag_id = 0;
	static uint8_t diag_id = DIAG_ID_APPS;
	uint8_t new_request = 0;

	if (!pkt || len == 0)
		return -EINVAL;
	diagid_v2_feature_mask =
		P_FMASK_DIAGID_V2(peripheral->periph_id);
	version = pkt->version;
	ALOGE("diag: %s: received diag id request ctrl pkt from perif: %s\n",
		__func__, peripheral->name);
	if (diagid_v2_feature_mask && version > DIAGID_VERSION_1) {
		if (len < (sizeof(struct diag_cntl_cmd_diag_id_v2 ) -
			(MAX_DIAGID_STR_LEN - MIN_DIAGID_STR_LEN))) {
			return -EINVAL;
		}
		feature_len = (uint32_t)pkt_v2->feature_len;
		memcpy((uint32_t *)&pd_feature_mask,
			&pkt_v2->pd_feature_mask, feature_len);
		process_name = (char *)&pkt_v2->feature_len +
			sizeof(feature_len) + feature_len;
	} else {
		if (len < (sizeof(struct diag_cntl_cmd_diag_id) -
				   (MAX_DIAGID_STR_LEN - MIN_DIAGID_STR_LEN))) {
			return -EINVAL;
		}
		process_name = (char*)&pkt->process_name;

		/**
		 * gvm will use diag-id query version1, hence below code will hit for gvm
		 * Further code in this API will be executed only for pvm.
		 */
		if (vm_enabled) {
			pd_val = diag_query_pd(process_name);
			ALOGI("diag: %s: Saving Diag ID %d with process name %s pd_val %d\n",
				__func__, pkt->diag_id, process_name, pd_val);
			diagmem->diag_id = pkt->diag_id;
			diag_add_diag_id_to_list(pkt->diag_id, process_name, pd_val, peripheral);
			return 0;
		}
	}

	root_str = strstr(process_name, DIAG_ID_ROOT_STRING);
	gvm_str = strstr(process_name, DIAG_ID_GVM_STRING);
	if (gvm_str)
		ALOGI("diag:%s: String %s found in %s\n", __func__, DIAG_ID_GVM_STRING, process_name);

	ret = diag_query_diag_id(process_name, &local_diag_id);
	if (!ret) {
		if (version == DIAGID_VERSION_3)
			diag_id = pkt_v2->diag_id;
		else
			diag_id++;
		new_request = 1;
		pd_val = diag_query_pd(process_name);
		diag_add_diag_id_to_list(diag_id, process_name,
			pd_val, peripheral);
		if (root_str || gvm_str) {
			if (peripheral->root_pd.pd_val == -1) {
				peripheral->root_pd.buf = malloc(HDLC_BUF_SIZE);
				if (!peripheral->root_pd.buf) {
					ALOGE("%s: failed to allocate buf for pd:%d diag_id:%d\n",
						__func__, pd_val, diag_id);
					return -ENOMEM;
				}
				peripheral->root_pd.pd_val = pd_val;
				peripheral->root_pd.diag_id = diag_id;
				tmp_pd_info = &peripheral->root_pd;
			}
		} else {
			for (i = 0; i < MAX_PERIPHERAL_UPD; i++) {
				if (peripheral->upd_info[i].pd_val == -1) {
					peripheral->upd_info[i].buf = malloc(HDLC_BUF_SIZE);
					if (!peripheral->upd_info[i].buf) {
						ALOGE("%s: failed to allocate buf for pd:%d diag_id:%d\n",
							__func__, pd_val, diag_id);
						return -ENOMEM;
					}
					peripheral->upd_info[i].pd_val = pd_val;
					peripheral->upd_info[i].diag_id = diag_id;
					tmp_pd_info = &peripheral->upd_info[i];
					break;
				}
			}
		}
		if (diagid_v2_feature_mask) {
			diag_add_fmask_to_diagid_table(diag_id, pd_feature_mask);
			process_diagid_v2_feature_mask(diag_id, pd_feature_mask);
		}
		local_diag_id = diag_id;
	}

	ctrl_pkt = malloc(sizeof(*ctrl_pkt));
	if (!ctrl_pkt) {
		if (tmp_pd_info && tmp_pd_info->buf) {
			free(tmp_pd_info->buf);
			tmp_pd_info->buf = NULL;
		}
		return -ENOMEM;
	}
	ctrl_pkt->diag_id = local_diag_id;
	ctrl_pkt->hdr.cmd = DIAG_CNTL_CMD_DIAG_ID;
	ctrl_pkt->version = DIAGID_VERSION_1;
	strlcpy((char *)&ctrl_pkt->process_name, process_name,
		sizeof(ctrl_pkt->process_name));
	ctrl_pkt->hdr.len = sizeof(ctrl_pkt->diag_id) + sizeof(ctrl_pkt->version) +
			strlen(process_name) + 1;
	len = ctrl_pkt->hdr.len+ sizeof(ctrl_pkt->hdr);
	ALOGE("diag: %s: sent diag id ctrl pkt with id: %d for process: %s\n",
		__func__, diag_id, process_name);
	queue_push_cntlq(peripheral, ctrl_pkt, len);

	/**
	 * send other control packets after sending feature mask.
	 * gvm_str is used to identify the gvm peripheral so that pvm can
	 * forward the mask to gvm after diag id assignment
	 */
	if (root_str || gvm_str) {
		if (!peripheral_is_gvm(peripheral)) {
			diag_config_perif_buf(&diagmem->buffering_mode[peripheral->periph_id], DIAG_STREAM_1);
			diag_config_perif_buf(&diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][peripheral->periph_id], DIAG_STREAM_2);
			diag_cntl_send_diag_settings(peripheral);
			diag_cntl_send_stm_state(peripheral->periph_id, diagmem->stm_state_requested[peripheral->periph_id]);
			if (diagmem->timesync_enabled)
				diag_cntl_send_time_switch_msg(peripheral, diagmem->time_api);
			diag_send_hw_accel_status(peripheral);
		} else {
			/**
			 * send pkt format select request to vm if pvm already received from tool
			 * to configure the diag-id based async packets & cmd req/resp
			 */
			if (pkt_format_select_check_mask(PKT_FORMAT_ALL_MASK))
				diag_cntl_send_pkt_format_request(peripheral, pkt_format_select_get_mask());
		}
		diag_cntl_send_masks(peripheral);
	}
	free(ctrl_pkt);
	ctrl_pkt = NULL;
	return 0;
}


static void diag_cntl_send_diag_settings(struct peripheral *peripheral)
{
	struct list_head *start;
	struct diag_settings_tbl_t *item = NULL;
	struct diag_cntl_cmd_diag_settings *ctrl_pkt = NULL;
	int len = 0;

	list_for_each(start, &diag_settings_list) {
		item = list_entry(start, struct diag_settings_tbl_t, link);
		if (item->periph_id == peripheral->periph_id) {
			len = DIAG_CNTL_PKT_DIAG_SETTINGS_HDR_SIZ + item->payload_len;
			ctrl_pkt = malloc(len);
			if (!ctrl_pkt) {
				ALOGE("diag: failed to allocate memory while forwarding diag settings\n");
				return;
			}

			ctrl_pkt->hdr.cmd = DIAG_CNTL_CMD_DIAG_SETTINGS;
			ctrl_pkt->hdr.len = (DIAG_CNTL_PKT_DIAG_SETTINGS_HDR_SIZ - sizeof(struct diag_cntl_hdr)) + item->payload_len;
			ctrl_pkt->version = 1;
			ctrl_pkt->settings_id = item->settings_id;
			ctrl_pkt->settings_version = item->settings_version;
			ctrl_pkt->diag_id = item->diag_id;
			ctrl_pkt->settings_payload_len = item->payload_len;
			memcpy(&ctrl_pkt->settings_payload[0], item->payload, item->payload_len);
			queue_push_cntlq(peripheral, ctrl_pkt, len);
			ALOGE("diag: sent diag settings ctrl pkt to peripheral %d\n", peripheral->periph_id);
			free(ctrl_pkt);
			ctrl_pkt= NULL;
		}
	}

	return ;
}
struct diag_settings_tbl_t * diag_query_diag_settings(struct peripheral *peripheral,
							struct diag_cntl_cmd_diag_settings *pkt)
{
	struct list_head *start;
	struct diag_settings_tbl_t *item = NULL;

	list_for_each(start, &diag_settings_list) {
		item = list_entry(start, struct diag_settings_tbl_t, link);
		if (item->periph_id == peripheral->periph_id &&
			item->settings_id == pkt->settings_id &&
			item->diag_id == pkt->diag_id)
			return item;
	}
	return NULL;

}
/* Save the diag settings received from peripheral and forward them back
 * after SSR . This is useful for qtrace mask saving.
 */
static int diag_cntl_process_diag_settings(struct peripheral *peripheral,
				  struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_diag_settings *pkt = (struct diag_cntl_cmd_diag_settings *)to_cmd_diag_id(hdr);
	struct diag_settings_tbl_t *new_item = NULL;

	if (!pkt || len == 0)
		return -EINVAL;

	new_item = diag_query_diag_settings(peripheral,pkt);
	if (!new_item) {
		new_item = malloc(sizeof(*new_item));
		if (!new_item)
			return -ENOMEM;
		new_item->payload = malloc(pkt->settings_payload_len);
		if (!new_item->payload) {
			free(new_item);
			new_item = NULL;
			return -ENOMEM;
		}
		new_item->periph_id = peripheral->periph_id;
		new_item->settings_id = pkt->settings_id;
		new_item->settings_version = pkt->settings_version;
		new_item->diag_id = pkt->diag_id;
		new_item->payload_len = pkt->settings_payload_len;
		memcpy(new_item->payload, &pkt->settings_payload[0], pkt->settings_payload_len);
		list_init(&new_item->link);
		list_add(&diag_settings_list, &new_item->link);
	} else {
		new_item->periph_id = peripheral->periph_id;
		new_item->settings_id = pkt->settings_id;
		new_item->settings_version = pkt->settings_version;
		new_item->diag_id = pkt->diag_id;
		new_item->payload_len = pkt->settings_payload_len;
		new_item->payload = realloc(new_item->payload , pkt->settings_payload_len);
		if (!new_item->payload)
			return -ENOMEM;
		memcpy(new_item->payload, &pkt->settings_payload[0], pkt->settings_payload_len);
	}
	return 0;
}

void diag_cntl_send_log_mask(struct peripheral *peripheral,
	uint32_t equip_id,
	uint32_t sub_id,
	int pid,
	uint8_t preset_id)
{
	struct diag_cntl_cmd_log_mask *pkt;
	struct diag_ctrl_log_mask_sub *ctrl_pkt_sub;

	size_t len = sizeof(*pkt);
	uint32_t num_items = 0;
	uint8_t *mask = NULL;
	uint32_t mask_size = 0;
	struct diag_log_mask_t *log_item = NULL;
	uint8_t status;

	if (peripheral == NULL)
		return;

	if (peripheral->cntl_fd == -1) {
		warn("Peripheral %s has no control channel. Skipping!\n", peripheral->name);
		return;
	}

	if (!peripheral->sent_feature_mask) {
		printf("diag:Feature mask not forwarded, failed to send log mask to periph %s \n", peripheral->name);
		return;
	}

	log_item = (struct diag_log_mask_t *)diag_get_mask(sub_id, &log_mask, pid);
	if (!log_item)
		return;

	status = diag_masks_status_opr(sub_id, &log_mask, pid, mask_status_get, 0);

	if (status == DIAG_CTRL_MASK_VALID) {
		diag_cmd_get_log_mask(equip_id, &num_items, &mask, &mask_size, log_item);
	} else {
		equip_id = 0;
	}

   if (sub_id != INVALID_INDEX && preset_id > 0 /*preset_id is used in modem only*/
		&& diagmem->feature[peripheral->periph_id].multi_sim_support) /*multi-sim*/
	{
		len = sizeof(struct diag_ctrl_log_mask_sub);
		len += mask_size;

		ctrl_pkt_sub = malloc(len);
		if (!ctrl_pkt_sub)
			return;
		ctrl_pkt_sub->cmd_type = DIAG_CTRL_MSG_LOG_MS_MASK;
		ctrl_pkt_sub->version = 1;
		ctrl_pkt_sub->preset_id = preset_id;
		if (sub_id >= 0) {
			ctrl_pkt_sub->id_valid = 1;
			ctrl_pkt_sub->sub_id = sub_id;
		} else {
			ctrl_pkt_sub->id_valid = 0;
			ctrl_pkt_sub->sub_id = 0;
		}
		ctrl_pkt_sub->stream_id = 1;
		ctrl_pkt_sub->status = status;
		if (status == DIAG_CTRL_MASK_VALID) {
			ctrl_pkt_sub->equip_id = equip_id;
			ctrl_pkt_sub->num_items = num_items;
			ctrl_pkt_sub->log_mask_size = mask_size;
		} else {
			ctrl_pkt_sub->equip_id = 0;
			ctrl_pkt_sub->num_items = 0;
			ctrl_pkt_sub->log_mask_size = 0;
		}
		ctrl_pkt_sub->data_len = len - sizeof(struct diag_cntl_hdr);//LOG_MASK_CTRL_HEADER_LEN_SUB + mask_size;

		if (mask != NULL) {
			memcpy(ctrl_pkt_sub->equip_log_mask, mask, mask_size);
			free(mask);
			mask = NULL;
		}

		queue_push_cntlq(peripheral, ctrl_pkt_sub, len);
		free(ctrl_pkt_sub);
		ctrl_pkt_sub = NULL;
	} else if(sub_id == INVALID_INDEX) {
		len += mask_size;

		pkt = malloc(len);
		if (!pkt)
			return;
		pkt->hdr.cmd = DIAG_CNTL_CMD_LOG_MASK;
		pkt->hdr.len = len - sizeof(struct diag_cntl_hdr);
		pkt->stream_id = 1;
		pkt->status = status;
		pkt->equip_id = equip_id;
		pkt->last_item = num_items;
		pkt->log_mask_size = mask_size;
		if (mask != NULL) {
			memcpy(pkt->equip_log_mask, mask, mask_size);
			free(mask);
			mask = NULL;
		}
		queue_push_cntlq(peripheral, pkt, len);
		free(pkt);
		pkt = NULL;
	}
}

void diag_cntl_send_msg_mask(struct peripheral *peripheral,
	struct diag_ssid_range_t *range,
	uint32_t sub_id,
	int pid,
	int preset)
{
	struct diag_cntl_cmd_msg_mask *pkt;
	struct diag_ctrl_msg_mask_sub *header_sub = NULL;

	size_t len = 0;
	uint32_t num_items = 0;
	uint32_t *mask = NULL;
	uint32_t mask_size = 0;
	uint8_t status;
	struct diag_ssid_range_t local_range;

	if (peripheral == NULL)
		return;
	if (peripheral->cntl_fd == -1) {
		warn("Peripheral %s has no control channel. Skipping!\n", peripheral->name);
		return;
	}
	if (!peripheral->sent_feature_mask) {
		printf("diag: Feature mask not forwarded, failed to send msg mask for periph %s\n", peripheral->name);
		return;
	}

	status = diag_masks_status_opr(sub_id, &msg_mask, pid, mask_status_get, 0);

	printf("diag: %s: sending msg mask with status %d \n", __func__, status);

	memcpy(&local_range, range, sizeof(*range));

	diag_cmd_get_msg_mask(&local_range, sub_id, &mask, pid);

	if (status == DIAG_CTRL_MASK_VALID) {
		num_items = local_range.ssid_last - local_range.ssid_first + 1;
	} else if (status == DIAG_CTRL_MASK_ALL_DISABLED) {
		num_items = 0;
	} else if (status == DIAG_CTRL_MASK_ALL_ENABLED) {
		num_items = 1;
	}

	mask_size = num_items * sizeof(*mask);

	if(sub_id != INVALID_INDEX
		&& diagmem->feature[peripheral->periph_id].multi_sim_support) /*multi-sim*/
	{
		len = sizeof(struct diag_ctrl_msg_mask_sub);
		len += mask_size;

		header_sub = malloc(len);
		if (!header_sub) {
			ALOGE("diag: %s: Failure to allocate pkt for msg mask for periph: %s\n",
				__func__, peripheral->name);
			return;
		}

		header_sub->cmd_type = DIAG_CTRL_MSG_F3_MS_MASK;
		header_sub->version = 1;
		header_sub->preset_id = preset;

		if (sub_id >= 0) {
			header_sub->id_valid = 1;
			header_sub->sub_id = sub_id;
		} else {
			header_sub->id_valid = 0;
			header_sub->sub_id = 0;
		}

		header_sub->status = status;
		header_sub->stream_id = 1;
		header_sub->msg_mode = 0;
		header_sub->ssid_first = local_range.ssid_first;
		header_sub->ssid_last = local_range.ssid_last;
		header_sub->msg_mask_size = num_items;
		header_sub->data_len = len - sizeof(struct diag_cntl_hdr);//MSG_MASK_CTRL_HEADER_LEN_SUB + mask_size;

		if (mask != NULL) {
			memcpy((char*)header_sub + sizeof(struct diag_ctrl_msg_mask_sub) , mask, mask_size);//memcpy(header_sub->range_msg_mask, mask, mask_size);
			free(mask);
			mask = NULL;
		}

		queue_push_cntlq(peripheral, header_sub, len);
		printf("diag: %s: Sent multisim mask to periph: %s, len: %d\n",
			__func__, peripheral->name, header_sub->data_len);
		free(header_sub);
		header_sub = NULL;

	} else if(sub_id == INVALID_INDEX) { /*legacy cmd*/
		len = sizeof(struct diag_cntl_cmd_msg_mask);
		len += mask_size;

		pkt = malloc(len);
		if (!pkt) {
			ALOGE("diag: %s: Failure to allocate pkt for msg mask for periph: %s \n",
				__func__, peripheral->name);
			return;
		}

		pkt->hdr.cmd = DIAG_CNTL_CMD_MSG_MASK;
		pkt->hdr.len = len - sizeof(struct diag_cntl_hdr);
		pkt->stream_id = 1;
		pkt->status = status;
		pkt->msg_mode = 0;
		pkt->range = local_range;
		pkt->msg_mask_len = num_items;
		if (mask != NULL) {
			memcpy(pkt->range_msg_mask, mask, mask_size);
			free(mask);
			mask = NULL;
		}
		queue_push_cntlq(peripheral, pkt, len);
		printf("diag: %s: Sent legacy mask to periph: %s, len: %d\n",
			__func__, peripheral->name, pkt->hdr.len);
		free(pkt);
		pkt = NULL;
	}

}

void diag_cntl_send_masks(struct peripheral *peripheral)
{
	struct diag_ssid_range_t range;
	struct diag_md_session_t *info = NULL;
	int i, j, pid = 0;

	info = diag_md_session_get_peripheral(DIAG_LOCAL_PROC, peripheral->periph_id);
	if (info) {
		pid = info->pid;
		if (!(info->peripheral_mask[DIAG_LOCAL_PROC] &
			MD_PERIPHERAL_MASK(peripheral->periph_id)))
			return;
	}
	if (!diagmem->mask_set_status) {
		ALOGD("diag: No mask set, skip updating empty masks to peripheral: %s\n", peripheral->name);
		return;
	}
	for (i = 0; i < MSG_MASK_TBL_CNT; i++) {
		range.ssid_first = ssid_first_arr[i];
		range.ssid_last = ssid_last_arr[i];
		diag_cntl_send_msg_mask(peripheral, &range, INVALID_INDEX, pid, 0);
	}
	diag_cntl_send_event_mask(peripheral, INVALID_INDEX, 0, pid);
	for (i = 0; i < MAX_EQUIP_ID; i++)
		diag_cntl_send_log_mask(peripheral, i, INVALID_INDEX , pid, 0);

	if(diagmem->feature[peripheral->periph_id].multi_sim_support) {/*after SSR send multisim masks too*/
		for (i = 0; (i < MAX_SIM_NUM) && (diag_subid_info[i] != INVALID_INDEX);i++) {
			printf("diag: %s: sending multisim masks to peripheral: %s for sub_id: %d\n", __func__, peripheral->name, diag_subid_info[i]);
			for (j = 0; j < MSG_MASK_TBL_CNT; j++) {
				range.ssid_first = ssid_first_arr[j];
				range.ssid_last = ssid_last_arr[j];

				diag_cntl_send_msg_mask(peripheral, &range, diag_subid_info[i], pid, 0);
			}
			diag_cntl_send_event_mask(peripheral, diag_subid_info[i], 0, pid);
			for (j = 0; j < MAX_EQUIP_ID; j++)
				diag_cntl_send_log_mask(peripheral, j, diag_subid_info[i] , pid, 0);
		}
	}

}

void diag_cntl_send_event_mask(struct peripheral *peripheral,
	uint32_t sub_id, uint8_t preset_id, int pid)
{
	struct diag_cntl_cmd_event_mask *pkt;
	size_t len = 0;
	uint8_t *mask = NULL;
	uint16_t mask_size = 0;
	uint8_t status;
	uint8_t event_config;
	void *event = NULL;

	if (peripheral == NULL)
		return;
	if (peripheral->cntl_fd == -1) {
		warn("Peripheral %s has no control channel. Skipping!\n", peripheral->name);
		return;
	}
	if (!peripheral->sent_feature_mask) {
		printf("diag: Feature mask not forwarded, failed to send event mask to %s\n", peripheral->name);
		return;
	}

	if(sub_id != INVALID_INDEX
		&& diagmem->feature[peripheral->periph_id].multi_sim_support) /*multi-sim*/
	{

		struct diag_ctrl_event_mask_sub *header_sub;
		len = sizeof(struct diag_ctrl_event_mask_sub);

		event = diag_get_mask(sub_id, &event_mask, pid);
		if (!event)
			return;

		status = diag_masks_status_opr(sub_id, &event_mask, pid, mask_status_get, DIAG_CTRL_MASK_INVALID);


		if (diag_cmd_get_event_mask(event_max_num_bits , &mask, event) == 0) {
			mask_size = BITS_TO_BYTES(event_max_num_bits);
		}
		len += mask_size;

		header_sub = (struct diag_ctrl_event_mask_sub *)calloc(1, len);

		if(!header_sub)
			return;

		header_sub->cmd_type = DIAG_CTRL_MSG_EVENT_MS_MASK;

		header_sub->event_config = (status == DIAG_CTRL_MASK_ALL_ENABLED
											|| status == DIAG_CTRL_MASK_VALID) ? 0x1 : 0x0;

		header_sub->event_mask_size = mask_size;
		header_sub->version = 1;
		header_sub->stream_id = 1;
		header_sub->preset_id = preset_id;
		header_sub->status = status;
		if (sub_id >= 0) {
			header_sub->id_valid = 1;
			header_sub->sub_id = sub_id;
		} else {
			header_sub->id_valid = 0;
			header_sub->sub_id = 0;
		}
		header_sub->data_len = len - sizeof(struct diag_cntl_hdr);//EVENT_MASK_CTRL_HEADER_LEN_SUB +header_sub->event_mask_size;

		if (mask != NULL) {
			memcpy(header_sub->event_mask, mask, mask_size);
			free(mask);
			mask = NULL;
		}
		queue_push_cntlq(peripheral, header_sub, len);
		free(header_sub);
		header_sub = NULL;

	} else if (sub_id == INVALID_INDEX ) {
		struct diag_md_session_t *info = NULL;
		len = sizeof(struct diag_cntl_cmd_event_mask);
		event = diag_get_mask(sub_id, &event_mask, pid);
		if (!event)
			return;
		status = diag_masks_status_opr(sub_id, &event_mask, pid, mask_status_get, DIAG_CTRL_MASK_INVALID);
		event_config = (status == DIAG_CTRL_MASK_ALL_ENABLED || status == DIAG_CTRL_MASK_VALID) ? 0x1 : 0x0;

		if (status == DIAG_CTRL_MASK_VALID) {
			if (diag_cmd_get_event_mask(event_max_num_bits , &mask, event) == 0)
					mask_size = BITS_TO_BYTES(event_max_num_bits);
		}

		len += mask_size;
		pkt = malloc(len);
		if(!pkt) {
			free(mask);
			mask = NULL;
			return;
		}

		pkt->hdr.cmd = DIAG_CNTL_CMD_EVENT_MASK;
		pkt->hdr.len = len - sizeof(struct diag_cntl_hdr);
		pkt->stream_id = 1;
		pkt->status = status;
		pkt->event_config = event_config;
		pkt->event_mask_len = mask_size;
		if (mask != NULL) {
			memcpy(pkt->event_mask, mask, mask_size);
			free(mask);
			mask = NULL;
		}
		queue_push_cntlq(peripheral, pkt, len);
		free(pkt);
		pkt = NULL;
	}

}

static int diag_cntl_deregister(struct peripheral *peripheral,
			      struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_dereg *pkt = to_cmd_dereg(hdr);
	struct diag_cmd *dc;
	unsigned int subsys;
	unsigned int cmd;
	unsigned int first;
	unsigned int last;
	int i;
	struct list_head *item;
	struct list_head *next;

	if (len <= 0)
		return -EINVAL;
	for (i = 0; i < pkt->count_entries; i++) {
		cmd = pkt->cmd;
		subsys = pkt->subsys;

		if (cmd == DIAG_CMD_NO_SUBSYS_DISPATCH && subsys != DIAG_CMD_NO_SUBSYS_DISPATCH)
			cmd = DIAG_CMD_SUBSYS_DISPATCH;

		first = cmd << 24 | subsys << 16 | pkt->ranges[i].first;
		last = cmd << 24 | subsys << 16 | pkt->ranges[i].last;

		list_for_each_safe(item, next, &diag_cmds) {
			dc = container_of(item, struct diag_cmd, node);
			if (dc->peripheral == peripheral && dc->first == first && dc->last == last) {
				list_del(&dc->node);
				free(dc);
				dc = NULL;
			}
		}
	}

	return 0;
}

void diag_cntl_send_feature_mask(struct peripheral *peripheral, uint32_t mask)
{
	struct diag_cntl_cmd_feature *pkt;
	size_t len = sizeof(*pkt) + 2;

	if (!peripheral)
		return;

	if (peripheral->sent_feature_mask) {
		ALOGE("diag:%s: Feature mask already send to %s \n", __func__, peripheral->name);
		return;
	}

	if (peripheral->cntl_fd == -1) {
		warn("Peripheral %s has no control channel. Skipping!\n", peripheral->name);
		return;
	}

	pkt = malloc(len);
	if (!pkt)
		return;

	pkt->hdr.cmd = DIAG_CNTL_CMD_FEATURE_MASK;
	pkt->hdr.len = len - sizeof(struct diag_cntl_hdr);
	pkt->mask_len = sizeof(pkt->mask);
	pkt->mask = mask;

	ALOGI("diag: %s: Sending feature mask to perif %s\n", __func__, peripheral->name);
	queue_push_cntlq(peripheral, pkt, len);
	peripheral->sent_feature_mask = 1;
	free(pkt);
	pkt = NULL;
}

void diag_cntl_set_real_time_mode(struct peripheral *perif, bool real_time, int diag_id)
{
	struct diag_cntl_cmd_diag_mode_v2 pkt_v2;
	struct diag_cntl_cmd_diag_mode pkt;
	if (diag_id) {
		pkt_v2.hdr.cmd = DIAG_CNTL_CMD_DIAG_MODE;
		pkt_v2.hdr.len = sizeof(struct diag_cntl_cmd_diag_mode_v2) - sizeof(struct diag_cntl_hdr);
		pkt_v2.version = 2;
		pkt_v2.sleep_vote = real_time;
		pkt_v2.real_time = real_time;
		pkt_v2.use_nrt_values = 0;
		pkt_v2.commit_threshold = 0;
		pkt_v2.sleep_threshold = 0;
		pkt_v2.sleep_time = 0;
		pkt_v2.drain_timer_val = 0;
		pkt_v2.event_stale_time_val = 0;
		pkt_v2.diag_id = diag_id;

		queue_push_cntlq(perif, &pkt_v2, sizeof(pkt_v2));
	} else {
		pkt.hdr.cmd = DIAG_CNTL_CMD_DIAG_MODE;
		pkt.hdr.len = sizeof(struct diag_cntl_cmd_diag_mode) - sizeof(struct diag_cntl_hdr);
		pkt.version = 1;
		pkt.sleep_vote = real_time;
		pkt.real_time = real_time;
		pkt.use_nrt_values = 0;
		pkt.commit_threshold = 0;
		pkt.sleep_threshold = 0;
		pkt.sleep_time = 0;
		pkt.drain_timer_val = 0;
		pkt.event_stale_time_val = 0;

		queue_push_cntlq(perif, &pkt, sizeof(pkt));
	}
}

int diag_cntl_set_buffering_mode(struct peripheral *perif, struct diag_buffering_mode_t *params, int diag_id, int stream_id)
{
	struct diag_cntl_cmd_buffering_tx_mode_v2 pkt_v2;
	struct diag_cntl_cmd_buffering_tx_mode pkt;

	switch (params->mode) {
	case DIAG_BUFFERING_MODE_STREAMING:
	case DIAG_BUFFERING_MODE_THRESHOLD:
	case DIAG_BUFFERING_MODE_CIRCULAR:
		break;
	default:
		printf("diag: In %s, invalid tx mode: %d\n", __func__,
			params->mode);
		return -EINVAL;
	}

	if (diag_id && diagmem->feature[perif->periph_id].pd_buffering) {
		pkt_v2.hdr.cmd = DIAG_CNTL_CMD_BUFFERING_TX_MODE;
		pkt_v2.hdr.len = sizeof(uint32_t) +  (3 * sizeof(uint8_t));
		pkt_v2.version = 2;
		pkt_v2.diag_id = diag_id;
		pkt_v2.stream_id = stream_id;
		pkt_v2.tx_mode = params->mode;
		queue_push_cntlq(perif, &pkt_v2, sizeof(pkt_v2));
	} else {
		pkt.hdr.cmd = DIAG_CNTL_CMD_BUFFERING_TX_MODE;
		pkt.hdr.len = sizeof(uint32_t) +  (2 * sizeof(uint8_t));
		pkt.version = 1;
		pkt.stream_id = stream_id;
		pkt.tx_mode = params->mode;
		queue_push_cntlq(perif, &pkt, sizeof(pkt));
	}
	return 0;
}
int diag_cntl_config_wm_values(struct peripheral *perif, struct diag_buffering_mode_t *params, int diag_id, int stream_id)
{
	struct diag_cntl_cmd_config_wm_val_v2 pkt_v2;
	struct diag_cntl_cmd_config_wm_val pkt;

	switch (params->mode) {
	case DIAG_BUFFERING_MODE_STREAMING:
	case DIAG_BUFFERING_MODE_THRESHOLD:
	case DIAG_BUFFERING_MODE_CIRCULAR:
		break;
	default:
		printf("diag: In %s, invalid tx mode: %d\n", __func__,
		       params->mode);
		return -EINVAL;
	}

	if (diag_id && diagmem->feature[perif->periph_id].pd_buffering) {
		pkt_v2.hdr.cmd = DIAG_CNTL_CMD_CONFIG_PERIPHERAL_WMQ_VAL;
		pkt_v2.hdr.len = sizeof(uint32_t) +  (4 * sizeof(uint8_t));
		pkt_v2.version = 2;
		pkt_v2.diag_id = diag_id;
		pkt_v2.stream_id = stream_id;
		pkt_v2.high_wm_val = params->high_wm_val;
		pkt_v2.low_wm_val = params->low_wm_val;
		queue_push_cntlq(perif, &pkt_v2, sizeof(pkt_v2));
	} else {
		pkt.hdr.cmd = DIAG_CNTL_CMD_BUFFERING_TX_MODE;
		pkt.hdr.len = sizeof(uint32_t) +  (3 * sizeof(uint8_t));
		pkt.version = 1;
		pkt.stream_id = stream_id;
		pkt.high_wm_val = params->high_wm_val;
		pkt.low_wm_val = params->low_wm_val;

		queue_push_cntlq(perif, &pkt, sizeof(pkt));
	}
	return 0;
}

bool diag_check_diag_id_valid(uint8_t diag_id)
{
	struct diag_id_tbl_t *item = NULL;

	list_for_each_entry(item, &diag_id_list, link) {
		if (diag_id == item->diag_id)
			return true;
	}

	return false;
}

uint8_t diag_query_diag_id_from_pd(uint8_t pd_val,
				uint8_t *diag_id, int *periph_id)
{
	struct list_head *start;
	struct diag_id_tbl_t *item = NULL;

	list_for_each(start, &diag_id_list) {
		item = list_entry(start, struct diag_id_tbl_t, link);
		if (pd_val == item->pd_val) {
			*periph_id = item->periph_id;
			*diag_id = item->diag_id;
			return 1;
		}
	}

	return 0;
}
void diag_map_pd_to_diagid(uint8_t pd, uint8_t *diag_id, int *peripheral)
{
        if (!diag_query_diag_id_from_pd(pd, (void *)diag_id,
                (void *)peripheral)) {
                *diag_id = 0;
                if ((pd >0) && pd < NUM_PERIPHERALS)
                        *peripheral = pd;
                else
                        *peripheral = -EINVAL;
        }

        if (*peripheral > 0)
                if (!diagmem->feature[*peripheral].pd_buffering)
                        *diag_id = 0;
}

int diag_query_peripheral_from_pd(uint8_t pd_val)
{
	struct list_head *start;
	struct diag_id_tbl_t *item = NULL;

	list_for_each(start, &diag_id_list) {
		item = list_entry(start, struct diag_id_tbl_t, link);
		if (pd_val == item->pd_val) {
			return item->periph_id;
		}
	}

	return -EINVAL;
}
int diag_config_perif_buf(struct diag_buffering_mode_t *params, int stream_id)
{
	int periph_id;
	uint8_t diag_id = 0;
	int rt_mode;
	struct peripheral *perif;
	if (!params)
		return -1;

	diag_map_pd_to_diagid(params->peripheral, &diag_id, &periph_id);
	ALOGM(DIAG_DBG_MASK_INFO, "diag: %s: Config request for buffering mode: %d peripheral:%d diag id:%d periph id:%d\n",
		__func__, params->mode, params->peripheral, diag_id, periph_id);
	if (periph_id < 0)
		return -1;
        perif = diag_get_periph_info(periph_id);
	if (!perif)
                return -EINVAL;
	if (!diagmem->buffering_flag[params->peripheral])
		return -EINVAL;
	if (!diagmem->feature[periph_id].peripheral_buffering)
		return -EINVAL;
	switch (params->mode) {
	case DIAG_BUFFERING_MODE_STREAMING:
		rt_mode = MODE_REALTIME;
		break;
	case DIAG_BUFFERING_MODE_THRESHOLD:
	case DIAG_BUFFERING_MODE_CIRCULAR:
		rt_mode = MODE_NONREALTIME;
		break;
	default:
		ALOGE("diag: %s: invalid tx mode: %d\n", __func__,
			   params->mode);
		return -EINVAL;
	}
	if (((params->high_wm_val > DIAG_MAX_WM_VAL) ||
		(params->low_wm_val > DIAG_MAX_WM_VAL)) ||
		(params->low_wm_val > params->high_wm_val) ||
		((params->low_wm_val == params->high_wm_val) &&
		(params->low_wm_val != DIAG_MIN_WM_VAL))) {
		ALOGE("diag: %s: invalid watermark values, high: %d, low: %d, peripheral: %d\n",
			__func__, params->high_wm_val, params->low_wm_val,
			params->peripheral);
		return -EINVAL;
	}
	diag_cntl_set_buffering_mode(perif, params, diag_id, stream_id);
	diag_cntl_config_wm_values(perif, params, diag_id, stream_id);
	if (stream_id == DIAG_STREAM_1) {
		diag_cntl_set_real_time_mode(perif, rt_mode, diag_id);
	}
	if (stream_id == DIAG_STREAM_2) {
		diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][params->peripheral].mode = params->mode;
		diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][params->peripheral].low_wm_val  = params->low_wm_val;
		diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][params->peripheral].high_wm_val  = params->high_wm_val;
		diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][params->peripheral].peripheral = params->peripheral;
	} else {
		diagmem->buffering_mode[params->peripheral].mode = params->mode;
		diagmem->buffering_mode[params->peripheral].low_wm_val  = params->low_wm_val;
		diagmem->buffering_mode[params->peripheral].high_wm_val  = params->high_wm_val;
		diagmem->buffering_mode[params->peripheral].peripheral = params->peripheral;
	}
	if (params->mode == DIAG_BUFFERING_MODE_STREAMING)
 		diagmem->buffering_flag[params->peripheral] = 0;
	return 0;
}

int diag_cntl_drain_perif_buf(int diag_id, int periph_id, int stream_id)
{
	struct diag_cntl_cmd_drain_immediate_v2 pkt_v2;
	struct diag_cntl_cmd_drain_immediate pkt;
	struct peripheral *perif;

	perif = diag_get_periph_info(periph_id);
	if (!perif)
		return -EINVAL;

	if (diag_id && diagmem->feature[periph_id].pd_buffering) {
		pkt_v2.hdr.cmd = DIAG_CNTL_CMD_PERIPH_BUF_DRAIN_IMM;
		pkt_v2.hdr.len = sizeof(uint32_t) +  (2 * sizeof(uint8_t));
		pkt_v2.version = 2;
		pkt_v2.diag_id = diag_id;
		pkt_v2.stream_id = stream_id;
		printf("sending v2 drain to perif %d",periph_id);
		queue_push_cntlq(perif, &pkt_v2, sizeof(pkt_v2));
	} else {
		pkt.hdr.cmd = DIAG_CNTL_CMD_PERIPH_BUF_DRAIN_IMM;
		pkt.hdr.len = sizeof(uint32_t) +  sizeof(uint8_t);
		pkt.version = 1;
		pkt.stream_id = stream_id;
		printf("sending drain to perif %d",periph_id);
		queue_push_cntlq(perif, &pkt, sizeof(pkt));
	}
	return 0;
}
void diag_cntl_send_time_switch_msg(struct peripheral *peripheral,
					uint8_t time_api)
{
	struct diag_cntl_cmd_time_sync time_sync_msg;

	time_sync_msg.hdr.cmd = DIAG_CNTL_CMD_TIME_SYNC;
	time_sync_msg.hdr.len = sizeof(time_sync_msg) - sizeof(time_sync_msg.hdr);
	time_sync_msg.version = 1;
	time_sync_msg.time_api = time_api;
	queue_push_cntlq(peripheral, &time_sync_msg, sizeof(time_sync_msg));

	return;
}


int diag_map_hw_accel_type_ver(uint8_t hw_accel_type, uint8_t hw_accel_ver)
{
	int index = -EINVAL;

	if (hw_accel_ver == DIAG_HW_ACCEL_VER_MIN) {
		switch (hw_accel_type) {
		case DIAG_HW_ACCEL_TYPE_STM:
			index = DIAG_HW_ACCEL_TYPE_STM;
			break;
		case DIAG_HW_ACCEL_TYPE_ATB:
			index = DIAG_HW_ACCEL_TYPE_ATB;
			break;
		default:
			index = -EINVAL;
			break;
		}
	}
	return index;
}
struct query_pd_feature_mask
{
	int data_type;
	uint32_t diagid_mask_supported;
	uint32_t diagid_mask_enabled;
}__packed;

int diag_cntl_query_pd_featuremask_local(
	struct diag_hw_accel_query_sub_payload_rsp_t *query_params)
{
	int f_index = -1;

	if (!query_params)
		return -EINVAL;

	if (query_params->hw_accel_type > DIAG_HW_ACCEL_TYPE_MAX ||
		query_params->hw_accel_ver > DIAG_HW_ACCEL_VER_MAX) {
		return -EINVAL;
	}

	f_index = diag_map_hw_accel_type_ver(query_params->hw_accel_type,
				query_params->hw_accel_ver);

	if (f_index < 0) {
		query_params->diagid_mask_supported = 0;
		query_params->diagid_mask_enabled = 0;
	} else {
		query_params->diagid_mask_supported = DIAGIDV2_FEATURE(f_index);
		query_params->diagid_mask_enabled = DIAGIDV2_STATUS(f_index);
	}
	return 0;
}
int diag_cntl_query_pd_featuremask(struct diag_client *dm,
	struct diag_hw_accel_query_sub_payload_rsp_t *query_params)
{
	int f_index = -1;
	struct query_pd_feature_mask *params;
	if (!query_params)
		return -EINVAL;

	if (query_params->hw_accel_type > DIAG_HW_ACCEL_TYPE_MAX ||
		query_params->hw_accel_ver > DIAG_HW_ACCEL_VER_MAX) {
		return -EINVAL;
	}

	f_index = diag_map_hw_accel_type_ver(query_params->hw_accel_type,
				query_params->hw_accel_ver);
	params = malloc(sizeof(*params));
	if (!params)
		return -ENOMEM;
	params->data_type = QUERY_PD_FEATURE_MASK_MASK_TYPE;
	if (f_index < 0) {
		params->diagid_mask_supported = 0;
		params->diagid_mask_enabled = 0;
	} else {
		params->diagid_mask_supported = DIAGIDV2_FEATURE(f_index);
		params->diagid_mask_enabled = DIAGIDV2_STATUS(f_index);
	}
	dm_send(dm, params, sizeof(*params));
	free(params);
	params = NULL;
	return 0;
}
int diag_cntl_send_passthru_control_pkt(struct diag_hw_accel_cmd_req_t *req_params)
{
	struct diag_cntl_cmd_passthru pkt;
	int f_index = -1;
	uint32_t diagid_mask = 0, diagid_status = 0;
	uint8_t i, hw_accel_type, hw_accel_ver;
	struct peripheral *peripheral;
	if (!req_params || req_params->operation > DIAG_HW_ACCEL_OP_QUERY) {
		return -EINVAL;
	}
	hw_accel_type = req_params->op_req.hw_accel_type;
	hw_accel_ver = req_params->op_req.hw_accel_ver;
	if (hw_accel_type > DIAG_HW_ACCEL_TYPE_MAX ||
		hw_accel_ver > DIAG_HW_ACCEL_VER_MAX) {
		return -EINVAL;
	}
	f_index = diag_map_hw_accel_type_ver(hw_accel_type, hw_accel_ver);
	if (f_index < 0) {
		return -EINVAL;
	}
	diagid_mask = req_params->op_req.diagid_mask;
	diagid_status = (DIAGIDV2_FEATURE(f_index) & diagid_mask);

	if (req_params->operation == DIAG_HW_ACCEL_OP_DISABLE) {
		DIAGIDV2_STATUS(f_index) &= ~diagid_status;
	} else {
		DIAGIDV2_STATUS(f_index) |= diagid_status;
		for (i = 0; i < DIAGID_V2_FEATURE_COUNT; i++) {
			if (i == f_index || !diagmem->diag_hw_accel[i])
				continue;
			DIAGIDV2_STATUS(i) &=
				~(DIAGIDV2_FEATURE(i) & diagid_mask);
		}
	}

	req_params->op_req.diagid_mask = DIAGIDV2_STATUS(f_index);
	pkt.hdr.cmd = DIAG_CTRL_MSG_PASS_THRU;
	pkt.hdr.len = sizeof(pkt)- sizeof(pkt.hdr);
	pkt.version = 1;
	pkt.diagid_mask = diagid_mask;
	pkt.hw_accel_type = hw_accel_type;
	pkt.hw_accel_ver = hw_accel_ver;
	pkt.control_data = req_params->operation;
	ALOGE("diag: recvd request to %s with HW Accel type: %d, ver: %d for diagid mask: %d\n",
		(req_params->operation ? "Enable" : "Disable"), hw_accel_type, hw_accel_ver, diagid_mask);
	for (i = 1; i < NUM_PERIPHERALS; i++) {
		if (!P_FMASK_DIAGID_V2(i))
			continue;
		peripheral = diag_get_periph_info(i);
		if (!peripheral)
			return -EINVAL;

		queue_push_cntlq(peripheral, &pkt, sizeof(pkt));
	}

	if ((diagid_mask & DIAG_ID_APPS) &&
		(hw_accel_type == DIAG_HW_ACCEL_TYPE_STM)) {
		diag_process_stm_mask(req_params->operation,
			DIAG_STM_APPS, PERIPHERAL_APPS);
	}

	return 0;

}
static void diag_send_hw_accel_status(struct peripheral *peripheral)
{
	struct diag_hw_accel_cmd_req_t req_params;
	uint32_t diagid_mask_bit = 0;
	uint8_t hw_accel_type = 0, hw_accel_ver = 0;
	int feature = 0, diag_id;
	struct list_head *start;
	struct diag_id_tbl_t *item = NULL;

	for (feature = 0; feature < DIAGID_V2_FEATURE_COUNT; feature++) {
		if (!diagmem->diag_hw_accel[feature])
			continue;
		list_for_each(start, &diag_id_list) {
			item = list_entry(start, struct diag_id_tbl_t, link);
			if (item->periph_id == peripheral->periph_id) {
				diag_id = item->diag_id;
				diagid_mask_bit =
					1 << (diag_id - 1);
				if (!diagid_mask_bit)
					continue;
				if (diagmem->diagid_v2_feature[feature] &
					diagmem->diagid_v2_status[feature] &
					diagid_mask_bit) {
					diag_map_index_to_hw_accel(feature, &hw_accel_type, &hw_accel_ver);
					req_params.header.cmd_code = DIAG_CMD_SUBSYS_DISPATCH;
					req_params.header.subsys_id = DIAG_CMD_DIAG_SUBSYS;
					req_params.header.subsys_cmd_code = DIAG_HW_ACCEL_CMD;
					req_params.version = 1;
					req_params.reserved = 0;
					req_params.operation = DIAG_HW_ACCEL_OP_ENABLE;
					req_params.op_req.hw_accel_type = hw_accel_type;
					req_params.op_req.hw_accel_ver = hw_accel_ver;
					req_params.op_req.diagid_mask = diagid_mask_bit;
					if (P_FMASK_DIAGID_V2(peripheral->periph_id))
						diag_cntl_send_passthru_control_pkt(&req_params);
				}
			}
		}
	}
}

/* send packet format select request to supported peripherals */
void diag_cntl_send_pkt_format_request(struct peripheral *peripheral, uint8_t mask)
{
	struct diag_cntl_cmd_pkt_format_req ctrl_pkt;

	ctrl_pkt.hdr.cmd = DIAG_CNTL_CMD_PKT_FORMAT_REQ;
	ctrl_pkt.hdr.len = sizeof(ctrl_pkt) - sizeof(ctrl_pkt.hdr);
	ctrl_pkt.version = PKT_FORMAT_SELECT_REQ_VERSION1;
	ctrl_pkt.mask = mask;

	ALOGM(DIAG_DBG_MASK_CNTL, "diag: %s: sending pkt format req to %s, mask 0x%x\n",
		__func__, peripheral->name, ctrl_pkt.mask);

	queue_push_cntlq(peripheral, &ctrl_pkt, sizeof(ctrl_pkt));
}

int diag_cntl_recv(struct peripheral *peripheral, const void *buf, size_t n)
{
	struct diag_cntl_hdr *hdr;
	size_t offset = 0;

	for (;;) {
		if (offset + sizeof(struct diag_cntl_hdr) > n)
			break;

		hdr = (struct diag_cntl_hdr *)((char*)buf + offset);
		if (offset + sizeof(struct diag_cntl_hdr) + hdr->len > n) {
			warnx("truncated diag cntl command");
			break;
		}

		switch (hdr->cmd) {
		case DIAG_CNTL_CMD_REGISTER:
			diag_cntl_register(peripheral, hdr, n);
			break;
		case DIAG_CNTL_CMD_FEATURE_MASK:
			diag_cntl_feature_mask(peripheral, hdr, n);
			break;
		case DIAG_CNTL_CMD_NUM_PRESETS:
			break;
		case DIAG_CNTL_CMD_DEREGISTER:
			diag_cntl_deregister(peripheral, hdr, n);
			break;
		case DIAG_CNTL_CMD_DIAG_ID:
			diag_cntl_process_diag_id(peripheral, hdr, n);
			break;
		case DIAG_CNTL_CMD_DIAG_SETTINGS:
			diag_cntl_process_diag_settings(peripheral, hdr, n);
			break;
		case DIAG_CNTL_CMD_MSG_MASK:
			diag_vm_process_msg_mask(peripheral, hdr, n);
			break;
		case DIAG_CNTL_CMD_EVENT_MASK:
			diag_vm_process_event_mask(peripheral, hdr, n);
			break;
		case DIAG_CNTL_CMD_LOG_MASK:
			diag_vm_process_log_mask(peripheral, hdr, n);
			break;
		case DIAG_CNTL_CMD_PKT_FORMAT_REQ:
			diag_vm_process_pkt_format_request(peripheral, hdr, n);
			break;
		default:
			warnx("[%s] unsupported control packet: %d",
			      peripheral->name, hdr->cmd);
			if (diag_debug_mask & DIAG_DBG_MASK_CNTL)
				print_hex_dump("CNTL", buf, n);
			break;
		}

		offset += sizeof(struct diag_cntl_hdr) + hdr->len;
	}

	return 0;
}

void diag_cntl_close(struct peripheral *peripheral)
{
	struct list_head *item;
	struct list_head *next;
	struct diag_cmd *dc;
	int pid = peripheral->periph_id;

	list_for_each_safe(item, next, &diag_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (dc->peripheral == peripheral) {
			list_del(&dc->node);
			free(dc);
			dc = NULL;
		}
	}

	peripheral->sent_feature_mask = 0;
	peripheral->received_feature_mask = 0;
	diagmem->feature[pid].diag_id_support = 0;
	diagmem->feature[pid].log_on_demand = 0;
	diagmem->stm_support[pid] = 0;
	diagmem->stm_state[pid] = 0;
	diagmem->stm_state_requested[pid] = 0;
}

int diag_send_realtime_update(int peripheral, int real_time)
{
	struct peripheral *perif = NULL;
	int i;

	for (i = 0; i < NUM_PERIPHERALS; i++) {
		if (!diagmem->buffering_flag[i])
			continue;
		/*
		 * One of the peripherals is in buffering mode. Don't set
		 * the RT value.
		 */
		return -EINVAL;
	}
	perif = diag_get_periph_info(peripheral);
	if (!perif)
		return -EINVAL;

	diag_cntl_set_real_time_mode(perif, real_time, 0);

	return 0;

}
void diag_update_proc_vote(uint16_t proc, uint8_t vote, int index)
{
	int i;


	if (vote)
		diagmem->proc_active_mask |= proc;
	else {
		diagmem->proc_active_mask &= ~proc;
		if (index == -1) {
			for (i = 0; i < DIAG_NUM_PROC; i++)
				diagmem->proc_rt_vote_mask[i] |= proc;
		} else {
			diagmem->proc_rt_vote_mask[index] |= proc;
		}
	}
}

void diag_update_real_time_vote(uint16_t proc, uint8_t real_time, int index)
{
	int i;

	if (index >= DIAG_NUM_PROC) {
		printf("diag: In %s, invalid index %d\n", __func__, index);
		return;
	}

	if (index == -1) {
		for (i = 0; i < DIAG_NUM_PROC; i++) {
			if (real_time)
				diagmem->proc_rt_vote_mask[i] |= proc;
			else
				diagmem->proc_rt_vote_mask[i] &= ~proc;
		}
	} else {
		if (real_time)
			diagmem->proc_rt_vote_mask[index] |= proc;
		else
			diagmem->proc_rt_vote_mask[index] &= ~proc;
	}

}

static int diag_compute_real_time(int idx)
{
	int real_time = MODE_REALTIME;

	if (diagmem->proc_active_mask == 0) {
		real_time = MODE_REALTIME;
	} else if (diagmem->proc_rt_vote_mask[idx] & diagmem->proc_active_mask) {
		/*
		 * Atleast one process is alive and is voting for Real Time
		 * data - Diag should be in real time mode irrespective of USB
		 * connection.
		 */
		real_time = MODE_REALTIME;
	} else if (diagmem->usb_connected) {
		/*
		 * If USB is connected, check individual process. If Memory
		 * Device Mode is active, set the mode requested by Memory
		 * Device process. Set to realtime mode otherwise.
		 */
		if ((diagmem->proc_rt_vote_mask[idx] &
						DIAG_PROC_VOTE_REALTIME_MEMORY_DEVICE) == 0)
			real_time = MODE_NONREALTIME;
		else
			real_time = MODE_REALTIME;
	} else {
		/*
		 * We come here if USB is not connected and the active
		 * processes are voting for Non realtime mode.
		 */
		real_time = MODE_NONREALTIME;
	}
	return real_time;
}

void diag_send_real_time(void)
{
	int temp_real_time = MODE_REALTIME, i, j;
	uint8_t send_update = 1, peripheral = 0;

	for (i = 1; i < NUM_MD_SESSIONS; i++) {
		peripheral = i;
		if (peripheral > NUM_PERIPHERALS)
			peripheral = diag_query_peripheral_from_pd(i);

		if (peripheral < 0 || peripheral >= NUM_PERIPHERALS)
			continue;

		if (!diagmem->feature[peripheral].peripheral_buffering)
			continue;
		switch (diagmem->buffering_mode[i].mode) {
		case DIAG_BUFFERING_MODE_THRESHOLD:
		case DIAG_BUFFERING_MODE_CIRCULAR:
			send_update = 0;
			break;
		}
	}

	for (i = 0; i < DIAG_NUM_PROC; i++) {
		temp_real_time = diag_compute_real_time(i);
		if (temp_real_time == diagmem->real_time_mode[i]) {
			printf("diag: did not update real time mode on proc %d, already in the req mode %d\n",
				i, temp_real_time);
			continue;
		} else {
			diagmem->real_time_mode[i] = temp_real_time ;
		}

		if (i == DIAG_LOCAL_PROC) {
			if (!send_update) {
				printf("diag: In %s, cannot send real time mode pkt since one of the periperhal is in buffering mode\n",
					 __func__);
				break;
			}
			for (j = 1; j < NUM_PERIPHERALS; j++) {
				diag_send_realtime_update(j, temp_real_time);
			}
		}
	}
}
