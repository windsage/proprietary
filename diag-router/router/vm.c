/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *               GVM Diag communication support
 *
 *GENERAL DESCRIPTION
 *
 *Implementation of diag GVM specific API's to communicate with PVM diag.
 *
 **====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "diag.h"
#include "diag_cntl.h"
#include "vm.h"
#include "dm.h"
#include "peripheral.h"
#include "sys_time.h"

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

#define EVENT_CMD_CODE			0x60
#define EVENT_DIAG_CONNECT_ID		0xD43
#define EVENT_DIAG_PROCESS_NAME_ID	0xB32

/* static function delcarations */
static int diag_vm_send_diag_id_event(struct peripheral *peripheral, uint16_t event_id);
static void diag_vm_send_diag_id_events(struct peripheral *peripheral);

/* static global variable declarations */
static bool diag_connect_id_event_send = false;

/* send feature mask to pvm */
void diag_vm_send_feature_mask(struct peripheral *peripheral)
{
	peripheral->features = 0;
	peripheral->features |= DIAG_FEATURE_FEATURE_MASK_SUPPORT;
	if (peripheral->cmd_fd >= 0)
		peripheral->features |= DIAG_FEATURE_REQ_RSP_SUPPORT;
	if (peripheral->sockets)
		peripheral->features |= DIAG_FEATURE_SOCKETS_ENABLED;
	peripheral->features |= DIAG_FEATURE_DIAG_ID;
	peripheral->features |= DIAG_FEATURE_DIAG_ID_LOGGING;
	peripheral->features |= DIAG_FEATURE_APPS_HDLC_ENCODE;

	ALOGI("diag: %s: %s Sending feature mask to pvm\n", __func__, peripheral->self_name);
	diag_cntl_send_feature_mask(peripheral, peripheral->features);
}

/* send diag ID query request to pvm */
int diag_vm_query_diag_id(struct peripheral *peripheral, const char* process_name)
{
	struct diag_cntl_cmd_diag_id *ctrl_pkt;
	size_t len;

	if (!process_name || !peripheral)
		return -EINVAL;

	ctrl_pkt = malloc(sizeof(*ctrl_pkt));
	if (!ctrl_pkt)
		return -ENOMEM;

	ALOGI("diag: %s: Requesting diag ID for process %s\n",__func__, process_name);
	ctrl_pkt->hdr.cmd = DIAG_CNTL_CMD_DIAG_ID;
	ctrl_pkt->version = DIAGID_VERSION_1;
	ctrl_pkt->diag_id = 0;
	strlcpy((char *)&ctrl_pkt->process_name, process_name,
		sizeof(ctrl_pkt->process_name));
	ctrl_pkt->hdr.len = sizeof(ctrl_pkt->diag_id) + sizeof(ctrl_pkt->version) +
			strlen(process_name) + 1;
	len = ctrl_pkt->hdr.len+ sizeof(ctrl_pkt->hdr);
	queue_push_cntlq(peripheral, ctrl_pkt, len);
	free(ctrl_pkt);
	return 0;
}

/* process msg mask received from pvm */
int diag_vm_process_msg_mask(struct peripheral *peripheral,
			      struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_msg_mask *pkt = to_cmd_msg_mask(hdr);
	int ret = 1;

	if (sizeof(*pkt) + (pkt->msg_mask_len * sizeof(uint32_t)) != len)
		return -EMSGSIZE;

	if (pkt->status != DIAG_CTRL_MASK_INVALID &&
		pkt->stream_id == 1) {
		ret = diag_cmd_set_msg_mask(pkt->range, pkt->range_msg_mask, INVALID_INDEX, 0);
		clients_broadcast_msg_mask();
	}

	return ret;
}

/* process event mask received from pvm */
int diag_vm_process_event_mask(struct peripheral *peripheral,
				struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_event_mask *pkt = to_cmd_event_mask(hdr);
	int ret = 1;

	if (sizeof(*pkt) + pkt->event_mask_len != len)
		return -EMSGSIZE;

	/* handle only valid mask packet with stream_id 1 */
	if(pkt->status != DIAG_CTRL_MASK_INVALID &&
		pkt->stream_id == 1) {
		ret = diag_cmd_update_event_mask((pkt->event_mask_len * 8),
				pkt->event_mask, INVALID_INDEX, 0);
		clients_broadcast_event_mask();
		/* sent diag-id event for each event mask update */
		diag_vm_send_diag_id_events(peripheral);
	}

	return ret;
}

/* process log mask received from pvm */
int diag_vm_process_log_mask(struct peripheral *peripheral,
			      struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_log_mask *pkt = to_cmd_log_mask(hdr);
	uint32_t num_items;
	uint32_t mask_size;
	int ret = 1;

	num_items = MIN(pkt->last_item, MAX_ITEMS_ALLOWED);
	mask_size = pkt->log_mask_size;

	if (sizeof(*pkt) + mask_size != len)
		return -EMSGSIZE;

	if (pkt->status != DIAG_CTRL_MASK_INVALID &&
		pkt->stream_id == 1) {
		ret = diag_cmd_set_log_mask(pkt->equip_id, &num_items, pkt->equip_log_mask, &mask_size, 0, INVALID_INDEX);
		clients_broadcast_log_mask();
	}

	return ret;
}

/* send data packet to pvm */
int diag_vm_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf,
		   int len, struct watch_flow *flow, int cmd_rsp_flag)
{
	struct peripheral *perif = NULL;
	struct list_head *queue = NULL;
	int ret = -1;

	perif = diag_get_periph_info(peripheral);
	if (perif != NULL) {
		queue = cmd_rsp_flag ? &perif->cmdq : &perif->dataq;
		queue_push(NULL, queue, buf, len);
		ret = 0;
	}

	return ret;
}

/* process pkt format select request from pvm & forward to diag clients */
void diag_vm_process_pkt_format_request(struct peripheral *peripheral,
					struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_pkt_format_req *ctrl_pkt = to_cmd_pkt_format_req(hdr);
	struct diag_pkt_format_request pkt;
	(void) peripheral;
	(void) len;

	if (ctrl_pkt->mask & PKT_FORMAT_INVALID_MASK ||
		ctrl_pkt->version != PKT_FORMAT_SELECT_REQ_VERSION1) {
		ALOGE("diag:%s: Invalid cntrl pkt version %d mask 0x%x \n",
			__func__, ctrl_pkt->version, ctrl_pkt->mask);
		return;
	}

	ALOGI("diag: %s: received pkt format select req version %d mask 0x%x\n",
		__func__, ctrl_pkt->version, ctrl_pkt->mask);

	/* Set mask locally to use while sending async or cmd resp from vm to pvm */
	pkt_format_select_req_save(ctrl_pkt->mask);

	/* broadcast pkt format select request to vm diag clients */
	pkt.type = DIAG_PKT_FORMAT_SELECT_TYPE;
	pkt.mask = ctrl_pkt->mask;
	dm_broadcast_to_socket_clients((void*)&pkt, sizeof(pkt));
}

/* send diag-id events to tool */
static void diag_vm_send_diag_id_events(struct peripheral *peripheral)
{
	diag_vm_send_diag_id_event(peripheral, EVENT_DIAG_PROCESS_NAME_ID);
	/* diag connect id event needs to be sent only once during intial connection
	 * with tool or after each SSR. In case of tool reconnection, PVM will
	 * send all diag_id:process name pair to tool
	 */
	if (!diag_connect_id_event_send &&
		!diag_vm_send_diag_id_event(peripheral, EVENT_DIAG_CONNECT_ID)) {
			diag_connect_id_event_send = true;
	}
}

/* send requested event to tool */
static int diag_vm_send_diag_id_event(struct peripheral *peripheral, uint16_t event_id)
{
	struct diag_id_event *event = NULL;
	struct peripheral *perif = NULL;
	struct diag_pkt_frame_t header = {0};
	char * event_p = NULL;
	uint32_t len = 0;
	uint32_t ts_lo;
	uint32_t ts_hi;
	int ret = 1;

	if (diag_check_event_mask(event_id)) {

		len = sizeof(header);

		event_p = malloc(sizeof(*event) + len + 1);
		if (!event_p)
			return -ENOMEM;

		/* fill non-HDLC header */
		header.start = CONTROL_CHAR;
		header.version = 1;
		header.length = sizeof(*event);
		memcpy((void*)event_p, (void*)&header, len);

		event = (struct diag_id_event *) (event_p + len);

		/* update event header and event payload */
		event->head.cmd_code = EVENT_CMD_CODE;
		ts_get_lohi(&ts_lo, &ts_hi);
		event->head.ts_lo = ts_lo;
		event->head.ts_hi = ts_hi;
		event->head.event_id.event_id_field.id = event_id;
		event->head.event_id.event_id_field.time_trunc_flag = 0;
		event->head.payload_length = sizeof(struct process_name_id);
		event->head.length = sizeof(struct event_header) - offsetof(struct event_header, event_id) +
					sizeof(struct process_name_id);
		event->head.event_id.event_id_field.payload_len = 0x3;

		event->payload.diagid = diagmem->diag_id;
		perif = peripheral_get_vm();
		if (perif) {
			ALOGM(DIAG_DBG_MASK_INFO, "diag: %s: diag-id:process name %d:%s\n",
				__func__, diagmem->diag_id, perif->self_name);

			strlcpy((char*)&event->payload.pname[0], (char*)perif->self_name,
				sizeof(event->payload.pname));
			event->payload.name_len = strlen(perif->self_name) + 1;
			len += sizeof(*event);
			*(uint8_t *)(event_p + len) = CONTROL_CHAR;
			len += sizeof(uint8_t);

			queue_push(NULL, &peripheral->dataq, (void*)event_p, len);
			free(event_p);
			ret = 0;
		}
	}

	return ret;
}
