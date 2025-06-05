/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2012-2020, The Linux Foundation. All rights reserved.

              Diag DCI support

GENERAL DESCRIPTION

Implementation of communication over DCI channels with peripheral diag and
support logging over DCI from apps userspace clients.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include "diag.h"
#include "diag_cntl.h"
#include "masks.h"
#include "peripheral.h"
#include "util.h"
#include "dm.h"
#include "list.h"
#include "diag_dci.h"
#include "diag_hidl_wrapper.h"
#include <pthread.h>
#include <signal.h>
unsigned int dci_max_reg = 100;
unsigned int dci_max_clients = 10;
#define DIAG_NUM_COMMON_CMD	1

/*
 * The header(32) for the secure log packet contains
 * Command code(1), Version(1), Feature Flags(2), Extended ID(4),
 * Encryption Version(1) Encryption Classifier(1),
 * Encryption Reserved(2), Encryption Sequence Counter(8),
 * the length of the log(2), log code(2) timestamp(8).
 */
#define SECURE_LOG_HEADER_LEN	32
#define SECURE_LOG_LEN_OFFSET	20
#define SECURE_LOG_CODE_OFFSET	22

/* Start of legacy packet encapsulated in the secure log packet format */
#define SECURE_LOG_START_OFFSET 20

#define DCI_CAN_ADD_BUF_TO_LIST(buf)					\
	(buf && buf->data && !buf->in_busy && buf->data_len > 0)	\

static uint8_t common_cmds[DIAG_NUM_COMMON_CMD] = {
	DIAG_CMD_DIAG_LOG_ON_DEMAND
};
struct dci_ops_tbl_t dci_ops_tbl[NUM_DCI_PROC] = {
	{
		.ctx = 0,
		.send_log_mask = diag_send_dci_log_mask,
		.send_event_mask = diag_send_dci_event_mask,
		.peripheral_status = 0,
		.mempool = 0,
	},
};
struct diag_dci_client_tbl *diag_dci_get_client_entry(int client_id)
{
	struct list_head *start;
	struct diag_dci_client_tbl *entry = NULL;

	list_for_each(start, &diagmem->dci_client_list) {
		entry = container_of(start, struct diag_dci_client_tbl, track);
		if (entry->client_info.client_id == client_id)
			return entry;
	}
	return NULL;
}
struct diag_dci_client_tbl *diag_dci_find_client_entry(struct diag_client *dm)
{
	struct list_head *start;
	struct diag_dci_client_tbl *entry = NULL;

	list_for_each(start, &diagmem->dci_client_list) {
		entry = container_of(start, struct diag_dci_client_tbl, track);
		if (entry->dm == dm)
			return entry;
	}
	return NULL;
}
static int diag_dci_init_buffer(struct diag_dci_buffer_t *buffer, int type)
{
	if (!buffer || buffer->data)
		return -EINVAL;
	switch (type) {
	case DCI_BUF_PRIMARY:
		buffer->capacity = IN_BUF_SIZE;
		buffer->data = malloc(buffer->capacity);
		if (!buffer->data)
			return -ENOMEM;
		break;
	case DCI_BUF_SECONDARY:
		buffer->data = NULL;
		buffer->capacity = IN_BUF_SIZE;
		break;
	case DCI_BUF_CMD:
		buffer->capacity = DIAG_MAX_REQ_SIZE + DCI_BUF_SIZE;
		buffer->data = malloc(buffer->capacity);
		if (!buffer->data)
			return -ENOMEM;
		break;
	default:
		printf("diag: In %s, unknown type %d\n", __func__, type);
		return -EINVAL;
	}

	buffer->data_len = 0;
	buffer->in_busy = 0;
	buffer->buf_type = type;

	return 0;
}
static int check_peripheral_dci_support(int peripheral_id, int dci_proc_id)
{
	int dci_peripheral_list = 0;

	if (dci_proc_id < 0 || dci_proc_id >= NUM_DCI_PROC) {
		printf("diag:In %s,not a supported DCI proc id\n", __func__);
		return 0;
	}
	if (peripheral_id < 0 || peripheral_id >= NUM_PERIPHERALS) {
		printf("diag:In %s,not a valid peripheral id\n", __func__);
		return 0;
	}
	dci_peripheral_list = dci_ops_tbl[dci_proc_id].peripheral_status;

	if (dci_peripheral_list <= 0 || dci_peripheral_list > DIAG_CON_ALL) {
		printf("diag:In %s,not a valid dci peripheral mask\n",
			 __func__);
		return 0;
	}

	if ((1 << peripheral_id) & (dci_peripheral_list))
		return 1;
	else
		return 0;
}
int diag_send_dci_event_mask(int token)
{
	struct diag_cntl_cmd_event_mask *pkt;
	int ret = DIAG_DCI_NO_ERROR, i, len = 0;
	unsigned char *event_mask_ptr = NULL;
	struct peripheral * perif;
	(void)token;

	event_mask_ptr = dci_ops_tbl[DCI_LOCAL_PROC].event_mask_composite;
	if (!event_mask_ptr) {
		return -EINVAL;
	}

	/* send event mask update */
	len = DCI_EVENT_MASK_SIZE + sizeof(*pkt);
	pkt = alloca(len);
	if (!pkt)
		return -ENOMEM;
	pkt->hdr.cmd = DIAG_CNTL_CMD_EVENT_MASK;
	pkt->hdr.len = len - sizeof(struct diag_cntl_hdr);
	pkt->stream_id = DCI_MASK_STREAM;
	pkt->status = DIAG_CTRL_MASK_VALID;
	pkt->event_config = 0;
	pkt->event_mask_len = DCI_EVENT_MASK_SIZE;
	for (i = 0; i < DCI_EVENT_MASK_SIZE; i++) {
		if (event_mask_ptr[i] != 0) {
			pkt->event_config = 1;
			break;
		}
	}
	memcpy(pkt->event_mask, event_mask_ptr, DCI_EVENT_MASK_SIZE);
	for (i = 1; i < NUM_PERIPHERALS; i++) {
		/*
		 * Don't send to peripheral if its regular channel
		 * is down. It may also mean that the peripheral doesn't
		 * support DCI.
		 */
		if (check_peripheral_dci_support(i, DCI_LOCAL_PROC)) {
			perif = diag_get_periph_info(i);
			queue_push_cntlq(perif, pkt, len);
		}
	}

	return ret;
}
int diag_dci_query_log_mask(struct diag_dci_client_tbl *entry,
			    uint16_t log_code)
{
	uint16_t item_num;
	uint8_t equip_id, *log_mask_ptr, byte_mask;
	int byte_index, offset;

	if (!entry) {
		printf("diag: In %s, invalid client entry\n", __func__);
		return 0;
	}

	equip_id = LOG_GET_EQUIP_ID(log_code);
	item_num = LOG_GET_ITEM_NUM(log_code);
	byte_index = item_num/8 + 2;
	byte_mask = 0x01 << (item_num % 8);
	offset = equip_id * 514;

	if (offset + byte_index >= DCI_LOG_MASK_SIZE) {
		printf("diag: In %s, invalid offset: %d, log_code: %d, byte_index: %d\n",
				__func__, offset, log_code, byte_index);
		return 0;
	}

	log_mask_ptr = entry->dci_log_mask;
	log_mask_ptr = log_mask_ptr + offset + byte_index;
	return ((*log_mask_ptr & byte_mask) == byte_mask) ? 1 : 0;

}

int diag_dci_query_event_mask(struct diag_dci_client_tbl *entry,
			      uint16_t event_id)
{
	uint8_t *event_mask_ptr, byte_mask;
	int byte_index, bit_index;

	if (!entry) {
		printf("diag: In %s, invalid client entry\n", __func__);
		return 0;
	}

	byte_index = event_id/8;
	bit_index = event_id % 8;
	byte_mask = 0x1 << bit_index;

	if (byte_index >= DCI_EVENT_MASK_SIZE) {
		printf("diag: In %s, invalid, event_id: %d, byte_index: %d\n",
				__func__, event_id, byte_index);
		return 0;
	}

	event_mask_ptr = entry->dci_event_mask;
	event_mask_ptr = event_mask_ptr + byte_index;
	return ((*event_mask_ptr & byte_mask) == byte_mask) ? 1 : 0;
}
static int dci_fill_log_mask(struct diag_cntl_cmd_log_mask *pkt, unsigned char *src_ptr)
{
	int len;

	len = sizeof(*pkt) + DCI_MAX_ITEMS_PER_LOG_CODE;
	pkt->hdr.cmd = DIAG_CNTL_CMD_LOG_MASK;
	pkt->hdr.len = len - sizeof(struct diag_cntl_hdr);
	pkt->stream_id = DCI_MASK_STREAM;
	pkt->status = 3;
	pkt->equip_id = *src_ptr;
	pkt->last_item = BITS_TO_BYTES(DCI_MAX_ITEMS_PER_LOG_CODE);
	pkt->log_mask_size = DCI_MAX_ITEMS_PER_LOG_CODE;
	memcpy(pkt->equip_log_mask, src_ptr + 2, DCI_MAX_ITEMS_PER_LOG_CODE);

	return sizeof(*pkt) + DCI_MAX_ITEMS_PER_LOG_CODE;
}
int diag_send_dci_log_mask(int token)
{
	void *buf;
	int write_len = 0;
	uint8_t *log_mask_ptr = NULL;
	int i, j, ret = DIAG_DCI_NO_ERROR;
	int updated;
	struct peripheral *perif;
	(void)token;

	log_mask_ptr = dci_ops_tbl[DCI_LOCAL_PROC].log_mask_composite;
	if (!log_mask_ptr) {
		return -EINVAL;
	}
	buf = alloca(DCI_MAX_ITEMS_PER_LOG_CODE + sizeof(struct diag_cntl_cmd_log_mask));
	if (!buf)
		return -ENOMEM;
	for (i = 0; i < 16; i++) {
		updated = 1;
		/* Dirty bit is set don't update the mask for this equip id */
		if (!(*(log_mask_ptr + 1))) {
			log_mask_ptr += 514;
			continue;
		}
		write_len = dci_fill_log_mask(buf, log_mask_ptr);
		for (j = 1; j < NUM_PERIPHERALS && write_len; j++) {
			if (check_peripheral_dci_support(j, DCI_LOCAL_PROC)) {
				perif = diag_get_periph_info(j);
				queue_push_cntlq(perif, buf, write_len);
			}
		}
		if (updated)
			*(log_mask_ptr+1) = 0; /* clear dirty byte */
		log_mask_ptr += 514;
	}

	return ret;
}
static void create_dci_log_mask_tbl(unsigned char *mask, uint8_t dirty)
{
	unsigned char *temp = mask;
	uint8_t i;

	if (!mask)
		return;

	/* create hard coded table for log mask with 16 categories */
	for (i = 0; i < DCI_MAX_LOG_CODES; i++) {
		*temp = i;
		temp++;
		*temp = dirty ? 1 : 0;
		temp++;
		memset(temp, 0, DCI_MAX_ITEMS_PER_LOG_CODE);
		temp += DCI_MAX_ITEMS_PER_LOG_CODE;
	}
}

static void create_dci_event_mask_tbl(unsigned char *tbl_buf)
{
	if (tbl_buf)
		memset(tbl_buf, 0, DCI_EVENT_MASK_SIZE);
}
void diag_dci_invalidate_cumulative_event_mask(int token)
{
	int i = 0;
	struct list_head *start;
	struct diag_dci_client_tbl *entry = NULL;
	uint8_t *event_mask_ptr, *update_ptr = NULL;

	update_ptr = dci_ops_tbl[token].event_mask_composite;
	if (!update_ptr) {
		return;
	}

	create_dci_event_mask_tbl(update_ptr);
	list_for_each(start, &diagmem->dci_client_list) {
		entry = container_of(start, struct diag_dci_client_tbl, track);
		if (entry->client_info.token != token)
			continue;
		event_mask_ptr = entry->dci_event_mask;
		for (i = 0; i < DCI_EVENT_MASK_SIZE; i++)
			*(update_ptr+i) |= *(event_mask_ptr+i);
	}

}

void diag_dci_invalidate_cumulative_log_mask(int token)
{
	int i = 0;
	struct list_head *start;
	struct diag_dci_client_tbl *entry = NULL;
	uint8_t *log_mask_ptr, *update_ptr = NULL;

	/* Clear the composite mask and redo all the masks */
	update_ptr = dci_ops_tbl[token].log_mask_composite;
	if (!update_ptr) {
		return;
	}

	create_dci_log_mask_tbl(update_ptr, DCI_LOG_MASK_DIRTY);
	list_for_each(start, &diagmem->dci_client_list) {
		entry = container_of(start, struct diag_dci_client_tbl, track);
		if (entry->client_info.token != token)
			continue;
		log_mask_ptr = entry->dci_log_mask;
		for (i = 0; i < DCI_LOG_MASK_SIZE; i++)
			*(update_ptr+i) |= *(log_mask_ptr+i);
	}

}
int diag_update_userspace_clients(int data_type)
{
	int total_len = 0;
	unsigned char *pkt = NULL;

	if (data_type == DCI_LOG_MASKS_TYPE) {
		pkt = alloca(DCI_LOG_MASK_SIZE +8 );
		if (!pkt) {
			printf("In %s failed to alloc memory\n", __func__);
			return 0;
		}
		memcpy(pkt,&data_type,sizeof(int));
		total_len = total_len + sizeof(int);
		memcpy(pkt+4, &diagmem->num_dci_client, sizeof(int));
		total_len = total_len + sizeof(int);
		memcpy(pkt + 8, (dci_ops_tbl[DCI_LOCAL_PROC].log_mask_composite),
			DCI_LOG_MASK_SIZE);
		total_len = total_len + DCI_LOG_MASK_SIZE;
	} else if (data_type == DCI_EVENT_MASKS_TYPE) {
		pkt = alloca(DCI_EVENT_MASK_SIZE + 8);
		if (!pkt) {
			printf("In %s failed to alloc memory\n", __func__);
			return 0;
		}
		memcpy(pkt,&data_type,sizeof(int));
		total_len = total_len + sizeof(int);
		memcpy(pkt+4, &diagmem->num_dci_client, sizeof(int));
		total_len = total_len + sizeof(int);
		memcpy(pkt + 8, (dci_ops_tbl[DCI_LOCAL_PROC].event_mask_composite),
			   DCI_EVENT_MASK_SIZE);
		total_len = total_len + DCI_EVENT_MASK_SIZE;
	}
	if (pkt != NULL)
		dm_broadcast_to_socket_clients(pkt, total_len);
	return 0;
}
int diag_dci_mask_update_hidl_client(unsigned char *pkt, int data_type)
{
	int total_len = 0;

	if (data_type == DCI_LOG_MASKS_TYPE) {
                memcpy(pkt,&data_type,sizeof(int));
                total_len = total_len + sizeof(int);
                memcpy(pkt+4, &diagmem->num_dci_client, sizeof(int));
                total_len = total_len + sizeof(int);
                memcpy(pkt + 8, (dci_ops_tbl[DCI_LOCAL_PROC].log_mask_composite),
                        DCI_LOG_MASK_SIZE);
                total_len = total_len + DCI_LOG_MASK_SIZE;
        } else if (data_type == DCI_EVENT_MASKS_TYPE) {
                memcpy(pkt,&data_type,sizeof(int));
                total_len = total_len + sizeof(int);
                memcpy(pkt+4, &diagmem->num_dci_client, sizeof(int));
                total_len = total_len + sizeof(int);
                memcpy(pkt + 8, (dci_ops_tbl[DCI_LOCAL_PROC].event_mask_composite),
                           DCI_EVENT_MASK_SIZE);
                total_len = total_len + DCI_EVENT_MASK_SIZE;
        }
	return total_len;
}
int diag_dci_buffering_mode_update_hidl_client(int peripheral, unsigned char *pkt, int data_type)
{
	int total_len = 0, proc = DCI_LOCAL_PROC;
	int mode = diagmem->dci_buffering_mode[DCI_LOCAL_PROC][peripheral].mode;

	if (check_peripheral_dci_support(peripheral, DCI_LOCAL_PROC)) {
		memcpy(pkt,&data_type,sizeof(int));
		total_len = total_len + sizeof(int);
		memcpy(pkt + sizeof(data_type), &proc, sizeof(int));
		total_len = total_len + sizeof(int);
		memcpy(pkt + sizeof(data_type) + sizeof(proc), &mode, sizeof(int));
		total_len = total_len + sizeof(int);
	}

	return total_len;

}
int diag_dci_clear_log_mask(int client_id)
{
	int err = DIAG_DCI_NO_ERROR, token = DCI_LOCAL_PROC;
	uint8_t *update_ptr;
	struct diag_dci_client_tbl *entry = NULL;

	entry = diag_dci_get_client_entry(client_id);
	if (!entry) {
		printf("diag: In %s, invalid client entry\n", __func__);
		return DIAG_DCI_TABLE_ERR;
	}
	token = entry->client_info.token;
	update_ptr = dci_ops_tbl[token].log_mask_composite;

	create_dci_log_mask_tbl(entry->dci_log_mask, DCI_LOG_MASK_CLEAN);
	diag_dci_invalidate_cumulative_log_mask(token);

	/*
	 * Send updated mask to userspace clients only if the client
	 * is registered on the local processor
	 */
	if (token == DCI_LOCAL_PROC) {
		diag_update_userspace_clients(DCI_LOG_MASKS_TYPE);
#ifndef FEATURE_LE_DIAG
		diag_update_hidl_client(DCI_LOG_MASKS_TYPE);
#endif
	}
	/* Send updated mask to peripherals */
	err = dci_ops_tbl[token].send_log_mask(token);
	return err;
}

int diag_dci_clear_event_mask(int client_id)
{
	int err = DIAG_DCI_NO_ERROR, token = DCI_LOCAL_PROC;
	uint8_t *update_ptr;
	struct diag_dci_client_tbl *entry = NULL;

	entry = diag_dci_get_client_entry(client_id);
	if (!entry) {
		printf("diag: In %s, invalid client entry\n", __func__);
		return DIAG_DCI_TABLE_ERR;
	}
	token = entry->client_info.token;
	update_ptr = dci_ops_tbl[token].event_mask_composite;

	create_dci_event_mask_tbl(entry->dci_event_mask);
	diag_dci_invalidate_cumulative_event_mask(token);

	/*
	 * Send updated mask to userspace clients only if the client is
	 * registerted on the local processor
	 */
	if (token == DCI_LOCAL_PROC) {
		diag_update_userspace_clients(DCI_EVENT_MASKS_TYPE);
#ifndef FEATURE_LE_DIAG
		diag_update_hidl_client(DCI_EVENT_MASKS_TYPE);
#endif
	}
	/* Send updated mask to peripherals */
	err = dci_ops_tbl[token].send_event_mask(token);
	return err;
}
void diag_dci_send_buffering_mode_pkt(struct diag_buffering_mode_t *params)
{
	diagmem->buffering_flag[params->peripheral] = 1;
	diag_config_perif_buf(params, DIAG_STREAM_2);

}
int diag_dci_register_client(struct diag_dci_reg_tbl_t *reg_entry, struct diag_client *dm, int pid)
{
	int i, j, err = 0, dci_buffering_mode = 0;
	struct diag_dci_client_tbl *new_entry = NULL;
	struct diag_dci_buf_peripheral_t *proc_buf = NULL;
	struct list_head *start, *temp;
	struct diag_dci_client_tbl *entry = NULL, *temp_entry = NULL;
	struct diag_buffering_mode_t params;

	if (!reg_entry)
		return DIAG_DCI_NO_REG;

	if (!VALID_DCI_TOKEN(reg_entry->token)) {
		printf("diag: Invalid DCI client token, %d\n",
			reg_entry->token);
		return DIAG_DCI_NO_REG;
	}

	if (diagmem->dci_state == DIAG_DCI_NO_REG)
		return DIAG_DCI_NO_REG;

	if (diagmem->num_dci_client >= MAX_DCI_CLIENTS)
		return DIAG_DCI_NO_REG;

	new_entry = malloc(sizeof(struct diag_dci_client_tbl));
	if (!new_entry)
		return DIAG_DCI_NO_REG;
	new_entry->dci_event_mask = NULL;
	new_entry->dci_log_mask = NULL;

	new_entry->client_info.notification_list =
				reg_entry->notification_list;
	new_entry->client_info.signal_type =
				reg_entry->signal_type;
	new_entry->client_info.token = reg_entry->token;
	new_entry->dm = dm;
	new_entry->pid = pid;

	switch (reg_entry->token) {
	case DCI_LOCAL_PROC:
		new_entry->num_buffers = NUM_DCI_PERIPHERALS;
		break;
	case DCI_MDM_PROC:
	case DCI_MDM_2_PROC:
		new_entry->num_buffers = 1;
		break;
	}
	new_entry->buffers = NULL;
	new_entry->real_time = MODE_REALTIME;
	new_entry->in_service = 0;
	for (i = 0; i < NUM_DCI_PROC; i++) {
		for (j = 0; j < NUM_PERIPHERALS; j++) {
			new_entry->vote_buffering_mode[i][j].mode = DIAG_BUFFERING_MODE_STREAMING;
			new_entry->vote_buffering_mode[i][j].peripheral = j;
			new_entry->vote_buffering_mode[i][j].low_wm_val = 0;
			new_entry->vote_buffering_mode[i][j].high_wm_val = 0;
		}
	}
	list_init(&new_entry->list_write_buf);
	new_entry->dci_log_mask =  malloc(DCI_LOG_MASK_SIZE);
	if (!new_entry->dci_log_mask) {
		//printf("diag: Unable to create log mask for DCI client, tgid: %d\n",
			//current->tgid);
		goto fail_alloc;
	}
	create_dci_log_mask_tbl(new_entry->dci_log_mask, DCI_LOG_MASK_CLEAN);
	new_entry->dci_event_mask =  malloc(DCI_EVENT_MASK_SIZE);
	if (!new_entry->dci_event_mask)
		goto fail_alloc;
	create_dci_event_mask_tbl(new_entry->dci_event_mask);

	new_entry->buffers = calloc(new_entry->num_buffers,
				     sizeof(struct diag_dci_buf_peripheral_t));
	if (!new_entry->buffers) {
		printf("diag: Unable to allocate buffers for peripherals in %s\n", __func__);
		goto fail_alloc;
	}
	for (i = 0; i < new_entry->num_buffers; i++) {
		proc_buf = &new_entry->buffers[i];
		if (!proc_buf)
			goto fail_alloc;
		proc_buf->health.dropped_events = 0;
		proc_buf->health.dropped_logs = 0;
		proc_buf->health.received_events = 0;
		proc_buf->health.received_logs = 0;
		proc_buf->buf_primary = calloc(1,
					sizeof(struct diag_dci_buffer_t));
		proc_buf->buf_curr = NULL;
		if (!proc_buf->buf_primary)
			goto fail_alloc;
		proc_buf->buf_cmd = calloc(1, sizeof(struct diag_dci_buffer_t));
		if (!proc_buf->buf_cmd)
			goto fail_alloc;
		err = diag_dci_init_buffer(proc_buf->buf_primary,
					   DCI_BUF_PRIMARY);
		if (err)
			goto fail_alloc;
		err = diag_dci_init_buffer(proc_buf->buf_cmd, DCI_BUF_CMD);
		if (err)
			goto fail_alloc;
		proc_buf->buf_curr = proc_buf->buf_primary;
	}

	for (i = 0; i < MAX_DCI_CLIENTS; i++) {
		if (diagmem->dci_client_id[i] == 0)
			break;
	}

	if (i == MAX_DCI_CLIENTS)
		goto fail_alloc;
	diagmem->dci_client_id[i] = 1;
	new_entry->client_info.client_id = i+1;
	reg_entry->client_id = i+1;
	diagmem->num_dci_client++;
	list_init(&new_entry->track);
	pthread_mutex_lock(&diagmem->dci_mutex);
	list_add(&diagmem->dci_client_list, &new_entry->track);
	pthread_mutex_unlock(&diagmem->dci_mutex);
	if (diagmem->num_dci_client == 1)
		diag_update_proc_vote(DIAG_PROC_VOTE_REALTIME_DCI, VOTE_UP, reg_entry->token);
	diag_send_real_time();

	if (reg_entry->token == DCI_LOCAL_PROC) {
		for (i = 1; i < NUM_PERIPHERALS; i++) {
			if (check_peripheral_dci_support(i, DCI_LOCAL_PROC)) {
				pthread_mutex_lock(&diagmem->dci_mutex);
				list_for_each(start, &diagmem->dci_client_list) {
					entry = container_of(start, struct diag_dci_client_tbl,
								track);
					if (entry->vote_buffering_mode[DCI_LOCAL_PROC][i].mode == DIAG_BUFFERING_MODE_STREAMING) {
						dci_buffering_mode = 0;
						break;
					}
					dci_buffering_mode = 1;
					memcpy(&params, &entry->vote_buffering_mode[DCI_LOCAL_PROC][i], sizeof(struct diag_buffering_mode_t));
				}
				pthread_mutex_unlock(&diagmem->dci_mutex);

				if (dci_buffering_mode) {
					diag_dci_send_buffering_mode_pkt(&params);
					diag_dci_send_buffering_mode_update_to_clients(DCI_LOCAL_PROC, params.mode);
					diagmem->dci_buffering_mode[DCI_LOCAL_PROC][i].mode = params.mode;
				} else {
					/* If current mode is buffering mode and requested mode is streaming mode then fwd streaming mode
					   ctrl pkt to peripheral and status updates to clients */
					if (diagmem->dci_buffering_mode[DCI_LOCAL_PROC][i].mode != DIAG_BUFFERING_MODE_STREAMING) {
						params.peripheral = i;
						params.mode = DIAG_BUFFERING_MODE_STREAMING;
						params.low_wm_val = 0;
						params.high_wm_val = 0;
						diag_dci_send_buffering_mode_pkt(&params);
						diagmem->dci_buffering_mode[DCI_LOCAL_PROC][i].mode = DIAG_BUFFERING_MODE_STREAMING;
						diag_dci_send_buffering_mode_pkt(&params);
						diag_dci_send_buffering_mode_update_to_clients(DCI_LOCAL_PROC, params.mode);
						diagmem->dci_buffering_mode[DCI_LOCAL_PROC][i].mode = params.mode;
					}
				}
			}
		}
	}
	return reg_entry->client_id;

fail_alloc:
	if (new_entry) {
		for (i = 0; ((i < new_entry->num_buffers) &&
			new_entry->buffers); i++) {
			proc_buf = &new_entry->buffers[i];
			if (proc_buf) {
				if (proc_buf->buf_primary) {
					free(proc_buf->buf_primary->data);
					proc_buf->buf_primary->data = NULL;
					free(proc_buf->buf_primary);
					proc_buf->buf_primary = NULL;
					if (proc_buf->buf_cmd) {
						free(proc_buf->buf_cmd->data);
						proc_buf->buf_cmd->data = NULL;
					}
					free(proc_buf->buf_cmd);
					proc_buf->buf_cmd = NULL;
				}
			}
		}
		if (new_entry->dci_event_mask) {
			free(new_entry->dci_event_mask);
			new_entry->dci_event_mask = NULL;
		}
		if (new_entry->dci_log_mask) {
			free(new_entry->dci_log_mask);
			new_entry->dci_log_mask = NULL;
		}
		free(new_entry->buffers);
		new_entry->buffers = NULL;
		free(new_entry);
		new_entry = NULL;
	}

	return DIAG_DCI_NO_REG;
}

void update_dci_cumulative_log_mask(int offset, unsigned int byte_index,
						uint8_t byte_mask, int token)
{
	uint8_t *log_mask_ptr, *update_ptr = NULL;
	bool is_set = false;
	struct list_head *start;
	struct diag_dci_client_tbl *entry = NULL;

	update_ptr = dci_ops_tbl[token].log_mask_composite;
	if (!update_ptr) {
		return;
	}

	update_ptr += offset;
	/* update the dirty bit */
	*(update_ptr+1) = 1;
	update_ptr = update_ptr + byte_index;
	list_for_each(start, &diagmem->dci_client_list) {
		entry = container_of(start, struct diag_dci_client_tbl, track);
		if (entry->client_info.token != token)
			continue;
		log_mask_ptr = entry->dci_log_mask;
		log_mask_ptr = log_mask_ptr + offset + byte_index;
		if ((*log_mask_ptr & byte_mask) == byte_mask) {
			is_set = true;
			/* break even if one client has the log mask set */
			break;
		}
	}

	if (!is_set)
		*update_ptr &= ~byte_mask;
	else
		*update_ptr |= byte_mask;

}

void update_dci_cumulative_event_mask(int offset, uint8_t byte_mask, int token)
{
	uint8_t *event_mask_ptr, *update_ptr = NULL;
	struct list_head *start;
	struct diag_dci_client_tbl *entry = NULL;
	bool is_set = false;

	update_ptr = dci_ops_tbl[token].event_mask_composite;
	if (!update_ptr) {
		return;
	}
	update_ptr += offset;
	list_for_each(start, &diagmem->dci_client_list) {
		entry = container_of(start, struct diag_dci_client_tbl, track);
		if (entry->client_info.token != token)
			continue;
		event_mask_ptr = entry->dci_event_mask;
		event_mask_ptr += offset;
		if ((*event_mask_ptr & byte_mask) == byte_mask) {
			is_set = true;
			/* break even if one client has the event mask set */
			break;
		}
	}
	if (!is_set)
		*update_ptr &= ~byte_mask;
	else
		*update_ptr |= byte_mask;

}
static int diag_dci_filter_commands(struct diag_pkt_header_t *header,
	int header_len)
{
	if (!header)
		return -ENOMEM;

	if (header_len <= 0)
		return -EIO;

	if (header_len) {
		switch (header->cmd_code) {
		case 0x7d: /* Msg Mask Configuration */
		case 0x73: /* Log Mask Configuration */
		case 0x81: /* Event Mask Configuration */
		case 0x82: /* Event Mask Change */
		case 0x60: /* Event Mask Toggle */
				printf("diag: command not supported: %d\n",
				header->cmd_code);
			return 1;
		}
	}

	if (header_len >= (3*sizeof(uint8_t))) {
		if (header->cmd_code == 0x4b && header->subsys_id == 0x12) {
			switch (header->subsys_cmd_code) {
			case 0x60: /* Extended Event Mask Config */
			case 0x61: /* Extended Msg Mask Config */
			case 0x62: /* Extended Log Mask Config */
			case 0x20C: /* Set current Preset ID */
			case 0x20D: /* Get current Preset ID */
			case 0x218: /* HDLC Disabled Command */
				printf("diag: command not supported %d %d %d\n",
					header->cmd_code, header->subsys_id,
					header->subsys_cmd_code);
				return 1;
			}
		}
	}

	return 0;
}

static struct dci_pkt_req_entry_t *diag_register_dci_transaction(int uid,
								 int client_id)
{
	struct dci_pkt_req_entry_t *entry = NULL;

	entry = calloc(1,sizeof(struct dci_pkt_req_entry_t));
	if (!entry)
		return NULL;

	diagmem->dci_tag++;
	entry->client_id = client_id;
	entry->uid = uid;
	entry->tag = diagmem->dci_tag;
	ALOGD("diag: Registering DCI cmd req, client_id: %d, uid: %d, tag:%d\n",
				entry->client_id, entry->uid, entry->tag);
	list_add(&diagmem->dci_req_list, &entry->track);

	return entry;
}

static struct dci_pkt_req_entry_t *diag_dci_get_request_entry(int tag)
{
	struct list_head *start;
	struct dci_pkt_req_entry_t *entry = NULL;

	list_for_each(start, &diagmem->dci_req_list) {
		entry = container_of(start, struct dci_pkt_req_entry_t, track);
		if (entry->tag == tag)
			return entry;
	}
	return NULL;
}

static int diag_dci_remove_req_entry(unsigned char *buf, int len,
				     struct dci_pkt_req_entry_t *entry)
{
	uint16_t rsp_count = 0, delayed_rsp_id = 0;

	if (!buf || len <= 0 || !entry) {
		printf("diag: In %s, invalid input buf: %pK, len: %d, entry: %pK\n",
			__func__, buf, len, entry);
		return -EIO;
	}

	/* It is an immediate response, delete it from the table */
	if (*buf != 0x80) {
		list_del(&entry->track);
		free(entry);
		entry = NULL;
		return 1;
	}

	/* It is a delayed response. Check if the length is valid */
	if (len < MIN_DELAYED_RSP_LEN) {
		printf("diag: Invalid delayed rsp packet length %d\n", len);
		return -EINVAL;
	}

	/*
	 * If the delayed response id field (uint16_t at byte 8) is 0 then
	 * there is only one response and we can remove the request entry.
	 */
	delayed_rsp_id = *(uint16_t *)(buf + 8);
	if (delayed_rsp_id == 0) {
		list_del(&entry->track);
		free(entry);
		entry = NULL;
		return 1;
	}

	/*
	 * Check the response count field (uint16 at byte 10). The request
	 * entry can be deleted it it is the last response in the sequence.
	 * It is the last response in the sequence if the response count
	 * is 1 or if the signed bit gets dropped.
	 */
	rsp_count = *(uint16_t *)(buf + 10);
	if (rsp_count > 0 && rsp_count < 0x1000) {
		list_del(&entry->track);
		free(entry);
		entry = NULL;
		return 1;
	}

	return 0;
}
int diag_check_common_cmd(struct diag_pkt_header_t *header)
{
	int i;

	if (!header)
		return -EIO;

	for (i = 0; i < DIAG_NUM_COMMON_CMD; i++) {
		if (header->cmd_code == common_cmds[i])
			return 1;
	}

	return 0;
}
int diag_dci_write(struct peripheral *periph, unsigned char *buf, int len)
{
	queue_push(NULL, &periph->dci_cmdq, buf, len);
	return 0;
}
static int diag_dci_cmd_dispatch(uint8_t *ptr,
			     size_t len, int tag)
{
	struct list_head *item;
	struct diag_cmd *dc;
	unsigned int key;
	int handled = 0;
	unsigned char apps_buf[DIAG_MAX_REQ_SIZE];
	int pkt_type = DCI_PKT_TYPE;
	int ret = 0, write_len = 0;
	struct diag_dci_pkt_header_t header;

	memset(apps_buf, 0, sizeof(apps_buf));
	if (ptr[0] == DIAG_CMD_SUBSYS_DISPATCH ||
	    ptr[0] == DIAG_CMD_SUBSYS_DISPATCH_V2)
		key = ptr[0] << 24 | ptr[1] << 16 | ptr[3] << 8 | ptr[2];
	else
		key = 0xff << 24 | 0xff << 16 | ptr[0];

	if (ptr[0] == MODE_CMD && ptr[1] != MODE_CMD_RESET)
		goto periph_send;

	list_for_each(item, &apps_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (key < dc->first || key > dc->last)
			continue;

		if (dc->cb)
			dc->cb(NULL, ptr, len, 0);
		else {
			memcpy(apps_buf,&pkt_type,sizeof(pkt_type));
			write_len = write_len + sizeof(pkt_type);
			header.start = 0x7e;
			header.version = 1;
			header.len = len + sizeof(int) + sizeof(uint8_t);
			header.pkt_code = DCI_PKT_RSP_CODE;
			header.tag = tag;
			memcpy(apps_buf+write_len, &header, sizeof(header));
			write_len += sizeof(header);
			memcpy(apps_buf + write_len, ptr, len);
			write_len += len;
			*(uint8_t *)(apps_buf + write_len) = 0x7e;
			write_len += sizeof(uint8_t);
			memcpy(apps_buf + write_len, ptr, len);
			ret = write(dc->fd, apps_buf, write_len);
			if (ret < 0)
				return -ENOENT;
		}

		handled++;
	}
	if (handled)
		return DIAG_DCI_NO_ERROR;
#ifndef FEATURE_LE_DIAG
	handled = check_for_diag_dci_system_client_commands(key,ptr, len, tag);

	if (handled)
		return DIAG_DCI_NO_ERROR;
#endif
periph_send:

	list_for_each(item, &diag_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (key < dc->first || key > dc->last)
			continue;

		if (dc->cb) {
			dc->cb(NULL, ptr, len, 0);
		} else {
			header.start = 0x7e;
			header.version = 1;
			header.len = len + sizeof(int) + sizeof(uint8_t);
			header.pkt_code = DCI_PKT_RSP_CODE;
			header.tag = tag;
			memcpy(apps_buf, &header, sizeof(header));
			write_len += sizeof(header);
			memcpy(apps_buf + write_len, ptr, len);
			write_len += len;
			*(uint8_t *)(apps_buf + write_len) = 0x7e;
			write_len += sizeof(uint8_t);

			diag_dci_write(dc->peripheral, apps_buf, write_len);
		}

		handled++;
		break;
	}

	if (handled)
		return DIAG_DCI_NO_ERROR;

	return -ENOENT;
}
static int diag_process_dci_pkt_rsp(unsigned char *buf, int len)
{
	int ret = DIAG_DCI_TABLE_ERR;
	int common_cmd = 0, header_len = 0;
	struct diag_pkt_header_t *header = NULL;
	unsigned char *temp = buf;
	unsigned char *req_buf = NULL;
	uint32_t read_len = 0, req_len = len;
	struct dci_pkt_req_entry_t *req_entry = NULL;
	struct diag_dci_client_tbl *dci_entry = NULL;
	struct dci_pkt_req_t req_hdr;

	if (!buf)
		return -EIO;

	if (len < sizeof(struct dci_pkt_req_t) ||
		len > DCI_REQ_BUF_SIZE) {
		printf("diag: dci: Invalid length %d len in %s\n",
			len, __func__);
		return -EIO;
	}
	req_hdr = *(struct dci_pkt_req_t *)temp;
	temp += sizeof(struct dci_pkt_req_t);
	read_len += sizeof(struct dci_pkt_req_t);
	req_len -= sizeof(struct dci_pkt_req_t);
	req_buf = temp; /* Start of the Request */
	dci_entry = diag_dci_get_client_entry(req_hdr.client_id);
	if (!dci_entry) {
		printf("diag: Invalid client %d in %s\n",
		       req_hdr.client_id, __func__);
		return DIAG_DCI_NO_REG;
	}
	header = (void *)temp;
	header_len = len - sizeof(struct dci_pkt_req_t);
	if (header_len <= 0) {
		return -EIO;
	}
	if (header_len >= sizeof(uint8_t)) {
		header->cmd_code = (uint16_t)(*(uint8_t *)temp);
		read_len += sizeof(uint8_t);
	}
	if (header_len >= (2 * sizeof(uint8_t))) {
		temp += sizeof(uint8_t);
		header->subsys_id = (uint16_t)(*(uint8_t *)temp);
		read_len += sizeof(uint8_t);
	}
	if (header_len == (3 * sizeof(uint8_t))) {
		temp += sizeof(uint8_t);
		header->subsys_cmd_code = (uint16_t)(*(uint8_t *)temp);
		read_len += sizeof(uint8_t);
	} else if (header_len >=
		(2 * sizeof(uint8_t)) + sizeof(uint16_t)) {
		temp += sizeof(uint8_t);
		header->subsys_cmd_code = (uint16_t)(*(uint16_t *)temp);
		read_len += sizeof(uint16_t);
	}

	if (read_len > DCI_REQ_BUF_SIZE) {
		printf("diag: dci: In %s, invalid read_len: %d\n", __func__,
		       read_len);
		return -EIO;
	}

	/* Check if the command is allowed on DCI */
	if (diag_dci_filter_commands(header, header_len)) {
		return DIAG_DCI_SEND_DATA_FAIL;
	}

	common_cmd = diag_check_common_cmd(header);
	if (common_cmd < 0) {
		printf("diag: error in checking common command, %d\n",
			 common_cmd);
		return DIAG_DCI_SEND_DATA_FAIL;
	}

	/* Register this new DCI packet */
	req_entry = diag_register_dci_transaction(req_hdr.uid,
						  req_hdr.client_id);
	if (!req_entry) {
		printf("diag: registering new DCI transaction failed\n");
		return DIAG_DCI_NO_REG;
	}
	ret = diag_dci_cmd_dispatch(req_buf, req_len,
					req_entry->tag);

	return ret;
}
int diag_process_dci_transaction(unsigned char *buf, int len)
{
	unsigned char *temp = buf;
	uint16_t log_code, item_num;
	int ret = -1, found = 0, client_id = 0, client_token = 0;
	int count, set_mask, num_codes, bit_index, event_id, offset = 0;
	unsigned int byte_index, read_len = 0;
	uint8_t equip_id, *log_mask_ptr, *head_log_mask_ptr, byte_mask;
	uint8_t *event_mask_ptr;
	struct diag_dci_client_tbl *dci_entry = NULL;

	if (!temp || len < sizeof(int)) {
		printf("diag: Invalid input in %s\n", __func__);
		return -EINVAL;
	}

	/* This is Pkt request/response transaction */
	if (*(int *)temp > 0) {
		return diag_process_dci_pkt_rsp(buf, len);
	} else if (*(int *)temp == DCI_LOG_TYPE) {
		/* Minimum length of a log mask config is 12 + 2 bytes for
		 * atleast one log code to be set or reset.
		 */
		if (len < DCI_LOG_CON_MIN_LEN || len > USER_SPACE_DATA) {
			printf("diag: dci: Invalid length in %s\n", __func__);
			return -EIO;
		}

		/* Extract each log code and put in client table */
		temp += sizeof(int);
		read_len += sizeof(int);
		client_id = *(int *)temp;
		temp += sizeof(int);
		read_len += sizeof(int);
		set_mask = *(int *)temp;
		temp += sizeof(int);
		read_len += sizeof(int);
		num_codes = *(int *)temp;
		temp += sizeof(int);
		read_len += sizeof(int);

		/* Find client table entry */
		dci_entry = diag_dci_get_client_entry(client_id);
		if (!dci_entry) {
			ALOGE("diag: In %s, invalid client\n", __func__);
			return ret;
		}
		client_token = dci_entry->client_info.token;

		if (num_codes == 0 || (num_codes >= (USER_SPACE_DATA - 8)/2)) {
			ALOGE("diag: dci: Invalid number of log codes %d\n",
								num_codes);
			return -EIO;
		}

		head_log_mask_ptr = dci_entry->dci_log_mask;
		if (!head_log_mask_ptr) {
			printf("diag: dci: Invalid Log mask pointer in %s\n",
								__func__);
			return -ENOMEM;
		}
		count = 0; /* iterator for extracting log codes */

		while (count < num_codes) {
			if (read_len + sizeof(uint16_t) > len) {
				printf("diag: dci: Invalid length for log type in %s\n",
								__func__);
				return -EIO;
			}
			log_code = *(uint16_t *)temp;
			equip_id = LOG_GET_EQUIP_ID(log_code);
			item_num = LOG_GET_ITEM_NUM(log_code);
			byte_index = item_num/8 + 2;
			if (byte_index >= (DCI_MAX_ITEMS_PER_LOG_CODE+2)) {
				printf("diag: dci: Log type, invalid byte index\n");
				return ret;
			}
			byte_mask = 0x01 << (item_num % 8);
			/*
			 * Parse through log mask table and find
			 * relevant range
			 */
			log_mask_ptr = head_log_mask_ptr;
			found = 0;
			offset = 0;
			while (log_mask_ptr && (offset < DCI_LOG_MASK_SIZE)) {
				if (*log_mask_ptr == equip_id) {
					found = 1;
					printf("diag: find equip id = %x at %pK\n",
						 equip_id, log_mask_ptr);
					break;
				}
				printf("diag: did not find equip id = %x at %d\n",
					 equip_id, *log_mask_ptr);
				log_mask_ptr += 514;
				offset += 514;
			}
			if (!found) {
				ALOGE("diag: dci equip id not found\n");
				return ret;
			}
			*(log_mask_ptr+1) = 1; /* set the dirty byte */
			log_mask_ptr = log_mask_ptr + byte_index;
			if (set_mask)
				*log_mask_ptr |= byte_mask;
			else
				*log_mask_ptr &= ~byte_mask;
			/* add to cumulative mask */
			update_dci_cumulative_log_mask(
				offset, byte_index,
				byte_mask, client_token);
			temp += 2;
			read_len += 2;
			count++;
			ret = DIAG_DCI_NO_ERROR;
		}
		/* send updated mask to userspace clients */
		if (client_token == DCI_LOCAL_PROC) {
			diag_update_userspace_clients(DCI_LOG_MASKS_TYPE);
#ifndef FEATURE_LE_DIAG
			diag_update_hidl_client(DCI_LOG_MASKS_TYPE);
#endif
		}
		/* send updated mask to peripherals */
		ret = dci_ops_tbl[client_token].send_log_mask(client_token);
	} else if (*(int *)temp == DCI_EVENT_TYPE) {
		/* Minimum length of a event mask config is 12 + 4 bytes for
		 * atleast one event id to be set or reset.
		 */
		if (len < DCI_EVENT_CON_MIN_LEN || len > USER_SPACE_DATA) {
			ALOGE("diag: dci: Invalid length in %s\n", __func__);
			return -EIO;
		}

		/* Extract each event id and put in client table */
		temp += sizeof(int);
		read_len += sizeof(int);
		client_id = *(int *)temp;
		temp += sizeof(int);
		read_len += sizeof(int);
		set_mask = *(int *)temp;
		temp += sizeof(int);
		read_len += sizeof(int);
		num_codes = *(int *)temp;
		temp += sizeof(int);
		read_len += sizeof(int);

		/* find client table entry */
		dci_entry = diag_dci_get_client_entry(client_id);
		if (!dci_entry) {
			ALOGE("diag: In %s, invalid client\n", __func__);
			return ret;
		}
		client_token = dci_entry->client_info.token;

		/* Check for positive number of event ids. Also, the number of
		 * event ids should fit in the buffer along with set_mask and
		 * num_codes which are 4 bytes each.
		 */
		if (num_codes == 0 || (num_codes >= (USER_SPACE_DATA - 8)/2)) {
			ALOGE("diag: dci: Invalid number of event ids %d\n",
								num_codes);
			return -EIO;
		}

		event_mask_ptr = dci_entry->dci_event_mask;
		if (!event_mask_ptr) {
			ALOGE("diag: dci: Invalid event mask pointer in %s\n",
								__func__);
			return -ENOMEM;
		}
		count = 0; /* iterator for extracting log codes */
		while (count < num_codes) {
			if (read_len + sizeof(int) > len) {
				ALOGE("diag: dci: Invalid length for event type in %s\n",
								__func__);
				return -EIO;
			}
			event_id = *(int *)temp;
			byte_index = event_id/8;
			if (byte_index >= DCI_EVENT_MASK_SIZE) {
				printf("diag: dci: Event type, invalid byte index\n");
				return ret;
			}
			bit_index = event_id % 8;
			byte_mask = 0x1 << bit_index;
			/*
			 * Parse through event mask table and set
			 * relevant byte & bit combination
			 */
			if (set_mask)
				*(event_mask_ptr + byte_index) |= byte_mask;
			else
				*(event_mask_ptr + byte_index) &= ~byte_mask;
			/* add to cumulative mask */
			update_dci_cumulative_event_mask(byte_index, byte_mask,
							 client_token);
			temp += sizeof(int);
			read_len += sizeof(int);
			count++;
			ret = DIAG_DCI_NO_ERROR;
		}
		/* send updated mask to userspace clients */
		if (dci_entry->client_info.token == DCI_LOCAL_PROC) {
			diag_update_userspace_clients(DCI_EVENT_MASKS_TYPE);
#ifndef FEATURE_LE_DIAG
			diag_update_hidl_client(DCI_EVENT_MASKS_TYPE);
#endif
		}
		/* send updated mask to peripherals */
		ret = dci_ops_tbl[client_token].send_event_mask(client_token);
	} else {
		printf("diag: Incorrect DCI transaction\n");
	}
	return ret;
}
static void dci_add_buffer_to_list(struct diag_dci_client_tbl *client,
				   struct diag_dci_buffer_t *buf)
{
	if (!buf || !client || !buf->data)
		return;

	if (buf->in_list || buf->data_len == 0)
		return;

	list_add(&client->list_write_buf, &buf->buf_track);
	/*
	 * In the case of DCI, there can be multiple packets in one read. To
	 * calculate the wakeup source reference count, we must account for each
	 * packet in a single read.
	 */
	buf->in_busy = 1;
	buf->in_list = 1;

}
static inline int diag_dci_check_buffer(struct diag_dci_buffer_t *buf, int len)
{
	if (!buf)
		return -EINVAL;

	/* Return 1 if the buffer is not busy and can hold new data */
	if ((buf->data_len + len < buf->capacity) && !buf->in_busy)
		return 1;

	return 0;
}
static int diag_dci_get_buffer(struct diag_dci_client_tbl *client,
			       int data_source, int len)
{
	struct diag_dci_buffer_t *buf_primary = NULL;
	struct diag_dci_buffer_t *buf_temp = NULL;
	struct diag_dci_buffer_t *curr = NULL;

	if (!client)
		return -EINVAL;
	if (len < 0 || len > IN_BUF_SIZE)
		return -EINVAL;

	curr = client->buffers[data_source].buf_curr;
	if (curr && diag_dci_check_buffer(curr, len) == 1)
		return 0;

	dci_add_buffer_to_list(client, curr);
	client->buffers[data_source].buf_curr = NULL;
	buf_primary = client->buffers[data_source].buf_primary;
	if (diag_dci_check_buffer(buf_primary, len) == 1) {
		client->buffers[data_source].buf_curr = buf_primary;
		return 0;
	}
	client->buffers[data_source].buf_curr = NULL;
	buf_temp = calloc(1,sizeof(struct diag_dci_buffer_t));
	if (!buf_temp)
		return -EIO;

	if (!diag_dci_init_buffer(buf_temp, DCI_BUF_SECONDARY)) {
		buf_temp->data = calloc(1,IN_BUF_SIZE);
		if (!buf_temp->data) {
			free(buf_temp);
			buf_temp = NULL;
			return -ENOMEM;
		}
		client->buffers[data_source].buf_curr = buf_temp;
		return 0;
	}

	free(buf_temp);
	buf_temp = NULL;
	return -EIO;
}
static void copy_ext_hdr(struct diag_dci_buffer_t *data_buffer, void *ext_hdr)
{
	if (!data_buffer) {
		printf("diag: In %s, data buffer is NULL\n", __func__);
		return;
	}

	*(int *)(data_buffer->data + data_buffer->data_len) =
			DCI_EXT_HDR_TYPE;
	data_buffer->data_len += sizeof(int);
	memcpy(data_buffer->data + data_buffer->data_len, ext_hdr,
			EXT_HDR_LEN);
	data_buffer->data_len += EXT_HDR_LEN;
}

static void copy_dci_log(unsigned char *buf, int len,
			 struct diag_dci_client_tbl *client, int data_source,
			 void *ext_hdr)
{
	uint16_t log_length = 0;
	struct diag_dci_buffer_t *data_buffer = NULL;
	struct diag_dci_buf_peripheral_t *proc_buf = NULL;
	int err = 0, total_len = 0;

	if (!buf || !client) {
		printf("diag: Invalid pointers in %s\n", __func__);
		return;
	}

	log_length = *(uint16_t *)((char*)buf + 2);
	if (log_length > USHRT_MAX - 4) {
		ALOGE("diag: Integer overflow in %s, log_len: %d\n",
				__func__, log_length);
		return;
	}
	total_len = sizeof(int) + log_length;
	if (ext_hdr)
		total_len += sizeof(int) + EXT_HDR_LEN;

	/* Check if we are within the len. The check should include the
	 * first 4 bytes for the Log code(2) and the length bytes (2)
	 */
	if ((log_length + sizeof(uint16_t) + 2) > len) {
		ALOGE("diag: Invalid length in %s, log_len: %d, len: %d\n",
						__func__, log_length, len);
		return;
	}

	proc_buf = &client->buffers[data_source];
	err = diag_dci_get_buffer(client, data_source, total_len);
	if (err) {
		if (err == -ENOMEM)
			proc_buf->health.dropped_logs++;
		else
			printf("diag: In %s, invalid packet\n", __func__);
		return;
	}
	data_buffer = proc_buf->buf_curr;
	proc_buf->health.received_logs++;
	if (!data_buffer->data) {
		return;
	}
	if (ext_hdr)
		copy_ext_hdr(data_buffer, ext_hdr);

	*(int *)(data_buffer->data + data_buffer->data_len) = DCI_LOG_TYPE;
	data_buffer->data_len += sizeof(int);
	memcpy(data_buffer->data + data_buffer->data_len, buf + sizeof(int),
	       log_length);
	data_buffer->data_len += log_length;
	data_buffer->data_source = data_source;
}

void extract_dci_log(unsigned char *buf, int len, int data_source, int token,
			void *ext_hdr)
{
	uint16_t log_code, read_bytes = 0;
	struct list_head *start;
	struct diag_dci_client_tbl *entry = NULL;

	if (!buf) {
		printf("diag: In %s buffer is NULL\n", __func__);
		return;
	}
	/*
	 * The first eight bytes for the incoming log packet contains
	 * Command code (2), the length of the packet (2), the length
	 * of the log (2) and log code (2)
	 */
	if (len < 8) {
		printf("diag: In %s invalid len: %d\n", __func__, len);
		return;
	}

	log_code = *(uint16_t *)((char*)buf + 6);
	read_bytes += sizeof(uint16_t) + 6;

	/* parse through log mask table of each client and check mask */
	pthread_mutex_lock(&diagmem->dci_mutex);
	list_for_each(start, &diagmem->dci_client_list) {
		entry = container_of(start, struct diag_dci_client_tbl, track);
		if (entry->client_info.token != token)
			continue;
		if (diag_dci_query_log_mask(entry, log_code)) {
			ALOGI("\t log code %x set\n",
				  log_code);
			/* copy to client buffer */
			copy_dci_log(buf, len, entry, data_source, ext_hdr);
		}
	}
	pthread_mutex_unlock(&diagmem->dci_mutex);
}

static void copy_dci_secure_log(unsigned char *buf, int len,
				struct diag_dci_client_tbl *client, int data_source,
				void *ext_hdr)
{
	uint16_t log_length = 0;
	struct diag_dci_buffer_t *data_buffer = NULL;
	struct diag_dci_buf_peripheral_t *proc_buf = NULL;
	int err = 0, total_len = 0;

	if (!buf || !client) {
		ALOGE("diag: %s: Invalid pointers\n", __func__);
		return;
	}

	log_length = *(uint16_t *)(buf + SECURE_LOG_LEN_OFFSET);
	if (log_length > USHRT_MAX - 4) {
		ALOGE("diag: %s: Integer overflow log_len: %d\n",
				__func__, log_length);
		return;
	}

	total_len = SECURE_LOG_START_OFFSET + log_length;
	if (ext_hdr)
		total_len += sizeof(int) + EXT_HDR_LEN;

	if (total_len > len) {
		ALOGE("diag: %s: Invalid length log_len: %d, len: %d\n",
						__func__, total_len, len);
		return;
	}

	proc_buf = &client->buffers[data_source];
	err = diag_dci_get_buffer(client, data_source, total_len);
	if (err) {
		if (err == -ENOMEM)
			proc_buf->health.dropped_logs++;
		else
			ALOGE("diag: %s: invalid packet: %d\n", __func__, err);
		return;
	}
	data_buffer = proc_buf->buf_curr;
	proc_buf->health.received_logs++;
	if (!data_buffer->data)
		return;

	if (ext_hdr)
		copy_ext_hdr(data_buffer, ext_hdr);

	*(uint32_t *)(data_buffer->data + data_buffer->data_len) = DCI_LOG_TYPE;
	data_buffer->data_len += sizeof(uint32_t);
	memcpy(data_buffer->data + data_buffer->data_len,
	       buf + SECURE_LOG_START_OFFSET, log_length);
	data_buffer->data_len += log_length;
	data_buffer->data_source = data_source;
}

void extract_dci_secure_log(unsigned char *buf, int len, int data_source, int token,
			void *ext_hdr)
{
	return;

#if 0
	struct diag_dci_client_tbl *entry;
        struct list_head *start;
        uint16_t log_code;
	/* Commenting the handling of secure log until the secure log
	   header design is finalized */
	if (!buf) {
		ALOGE("diag: %s: buffer is NULL\n", __func__);
		return;
	}

	if (len < SECURE_LOG_HEADER_LEN) {
		ALOGE("diag: %s: invalid len: %d\n", __func__, len);
		return;
	}

	log_code = *(uint16_t *)(buf + SECURE_LOG_CODE_OFFSET);

	/* parse through log mask table of each client and check mask */
	pthread_mutex_lock(&diagmem->dci_mutex);
	list_for_each(start, &diagmem->dci_client_list) {
		entry = container_of(start, struct diag_dci_client_tbl, track);
		if (entry->client_info.token != token)
			continue;
		if (diag_dci_query_log_mask(entry, log_code)) {
			ALOGE("\t log code %x set\n", log_code);
			/* copy to client buffer */
			copy_dci_secure_log(buf, len, entry, data_source, ext_hdr);
		}
	}
	pthread_mutex_unlock(&diagmem->dci_mutex);
#endif

}

int diag_dci_deinit_client(struct diag_dci_client_tbl *entry)
{
	int ret = DIAG_DCI_NO_ERROR, real_time = MODE_REALTIME, i, peripheral;
	int dci_buffering_mode = 0;
	struct diag_dci_buf_peripheral_t *proc_buf = NULL;
	struct diag_dci_buffer_t *buf_entry, *temp;
	struct list_head *start, *req_temp;
	struct dci_pkt_req_entry_t *req_entry = NULL;
	int token = DCI_LOCAL_PROC;
	struct diag_buffering_mode_t params;

	if (!entry)
		return DIAG_DCI_NOT_SUPPORTED;

	ALOGI("diag: %s: dci client cleanup for pid: %d\n",
		__func__, entry->pid);

	token = entry->client_info.token;

	if (!list_empty(&entry->track))
		list_del(&entry->track);

	if (entry->client_info.client_id > MAX_DCI_CLIENTS)
		return DIAG_DCI_NO_REG;
	diagmem->num_dci_client--;
	diagmem->dci_client_id[entry->client_info.client_id - 1] = 0;

	free(entry->dci_log_mask);
	entry->dci_log_mask = NULL;
	diag_dci_invalidate_cumulative_log_mask(token);
	if (token == DCI_LOCAL_PROC) {
		diag_update_userspace_clients(DCI_LOG_MASKS_TYPE);
#ifndef FEATURE_LE_DIAG
		diag_update_hidl_client(DCI_LOG_MASKS_TYPE);
#endif
	}
	ret = dci_ops_tbl[token].send_log_mask(token);
	if (ret != DIAG_DCI_NO_ERROR)
		return ret;
	free(entry->dci_event_mask);
	entry->dci_event_mask = NULL;
	diag_dci_invalidate_cumulative_event_mask(token);
	if (token == DCI_LOCAL_PROC) {
		diag_update_userspace_clients(DCI_EVENT_MASKS_TYPE);
#ifndef FEATURE_LE_DIAG
		diag_update_hidl_client(DCI_EVENT_MASKS_TYPE);
#endif
	}
	ret = dci_ops_tbl[token].send_event_mask(token);
	if (ret != DIAG_DCI_NO_ERROR)
		return ret;
	list_for_each_safe(start, req_temp, &diagmem->dci_req_list) {
		req_entry = container_of(start, struct dci_pkt_req_entry_t,
				       track);
		if (req_entry->client_id == entry->client_info.client_id) {
			list_del(&req_entry->track);
			free(req_entry);
			req_entry = NULL;
		}
	}


	list_for_each_entry_safe(buf_entry, temp, &entry->list_write_buf,
							buf_track) {
		if (!list_empty(&buf_entry->buf_track))
			list_del(&buf_entry->buf_track);
		if (buf_entry->buf_type == DCI_BUF_SECONDARY) {
			free(buf_entry->data);
			buf_entry->data = NULL;
			free(buf_entry);
			buf_entry = NULL;
		} else if (buf_entry->buf_type == DCI_BUF_CMD) {
	               peripheral = buf_entry->data_source;
			//if (peripheral == APPS_DATA)
				//continue;
		}
	}
	for (i = 0; i < entry->num_buffers; i++) {
		proc_buf = &entry->buffers[i];
		buf_entry = proc_buf->buf_curr;

		if (buf_entry && buf_entry->buf_type == DCI_BUF_SECONDARY) {
			free(buf_entry->data);
			buf_entry->data = NULL;
			free(buf_entry);
			buf_entry = NULL;
		}

		free(proc_buf->buf_primary->data);
		proc_buf->buf_primary->data = NULL;
		free(proc_buf->buf_cmd->data);
		proc_buf->buf_cmd->data = NULL;
		free(proc_buf->buf_primary);
		proc_buf->buf_primary = NULL;
		free(proc_buf->buf_cmd);
		proc_buf->buf_cmd = NULL;
	}
	free(entry->buffers);
	entry->buffers = NULL;
	free(entry);
	entry = NULL;

	if (diagmem->num_dci_client == 0) {
		diag_update_proc_vote(DIAG_PROC_VOTE_REALTIME_DCI, VOTE_DOWN, token);
		if (token == DCI_LOCAL_PROC) {
			for (i = 1; i < NUM_PERIPHERALS; i++) {
				if (check_peripheral_dci_support(i, token)) {
					if (diagmem->dci_buffering_mode[token][i].mode) {
						params.peripheral = i;
						params.mode = DIAG_BUFFERING_MODE_STREAMING;
						params.low_wm_val = 0;
						params.high_wm_val = 0;
						diag_dci_send_buffering_mode_pkt(&params);
						diagmem->dci_buffering_mode[token][i].mode = DIAG_BUFFERING_MODE_STREAMING;
					}
				}
			}
		}
	} else {
		real_time = diag_dci_get_cumulative_real_time(token);
		diag_update_real_time_vote(DIAG_PROC_VOTE_REALTIME_DCI, real_time, token);
	}
	diag_send_real_time();
	if (token == DCI_LOCAL_PROC) {
			for (i = 1; i < NUM_PERIPHERALS; i++) {
				if (check_peripheral_dci_support(i, token)) {
					list_for_each(start, &diagmem->dci_client_list) {
						entry = container_of(start, struct diag_dci_client_tbl,
											 track);
						if (entry->vote_buffering_mode[token][i].mode == DIAG_BUFFERING_MODE_STREAMING) {
							dci_buffering_mode = 0;
							break;
						}
						dci_buffering_mode = 1;
						memcpy(&params, &entry->vote_buffering_mode[token][i], sizeof(struct diag_buffering_mode_t));
					}
					if (dci_buffering_mode) {
						diag_dci_send_buffering_mode_pkt(&params);
						diagmem->dci_buffering_mode[token][i].mode = params.mode;
					} else {
						if (diagmem->dci_buffering_mode[token][i].mode) {
							params.peripheral = i;
							params.mode = DIAG_BUFFERING_MODE_STREAMING;
							params.low_wm_val = 0;
							params.high_wm_val = 0;
							diag_dci_send_buffering_mode_pkt(&params);
							diagmem->dci_buffering_mode[token][i].mode = DIAG_BUFFERING_MODE_STREAMING;
						}
					}
				}
			}
	}


	return DIAG_DCI_NO_ERROR;
}

static void copy_dci_event(unsigned char *buf, int len,
			struct diag_dci_client_tbl *client, int data_source,
			void *ext_hdr)
{
	struct diag_dci_buffer_t *data_buffer = NULL;
	struct diag_dci_buf_peripheral_t *proc_buf = NULL;
	int err = 0, total_len = 0;

	if (!buf || !client) {
		printf("diag: Invalid pointers in %s\n", __func__);
		return;
	}

	total_len = sizeof(int) + len;
	if (ext_hdr)
		total_len += sizeof(int) + EXT_HDR_LEN;

	proc_buf = &client->buffers[data_source];
	err = diag_dci_get_buffer(client, data_source, total_len);
	if (err) {
		if (err == -ENOMEM)
			proc_buf->health.dropped_events++;
		else
			printf("diag: In %s, invalid packet\n", __func__);
		return;
	}

	data_buffer = proc_buf->buf_curr;

	proc_buf->health.received_events++;
	if (ext_hdr)
		copy_ext_hdr(data_buffer, ext_hdr);

	*(int *)(data_buffer->data + data_buffer->data_len) = DCI_EVENT_TYPE;
	data_buffer->data_len += sizeof(int);
	memcpy(data_buffer->data + data_buffer->data_len, buf, len);
	data_buffer->data_len += len;
	data_buffer->data_source = data_source;
}

void extract_dci_events(unsigned char *buf, int len, int data_source,
		int token, void *ext_hdr)
{
	uint16_t event_id, event_id_packet, length, temp_len;
	uint8_t payload_len, payload_len_field;
	uint8_t timestamp[8] = {0}, timestamp_len;
	unsigned char event_data[MAX_EVENT_SIZE];
	unsigned int total_event_len;
	struct list_head *start;
	struct diag_dci_client_tbl *entry = NULL;

	if (!buf) {
		printf("diag: In %s buffer is NULL\n", __func__);
		return;
	}
	/*
	 * 1 byte for event code and 2 bytes for the length field.
	 * The length field indicates the total length removing the cmd_code
	 * and the length field. The event parsing in that case should happen
	 * till the end.
	 */
	if (len < 3) {
		printf("diag: In %s invalid len: %d\n", __func__, len);
		return;
	}
	length = *(uint16_t *)(buf + 1); /* total length of event series */
	if ((length == 0) || (len != (length + 3))) {
		printf("diag: Incoming dci event length: %d is invalid\n",
			length);
		return;
	}
	/*
	 * Move directly to the start of the event series.
	 * The event parsing should happen from start of event
	 * series till the end.
	 */
	temp_len = 3;
	while (temp_len < length) {
		event_id_packet = *(uint16_t *)(buf + temp_len);
		event_id = event_id_packet & 0x0FFF; /* extract 12 bits */
		if (event_id_packet & 0x8000) {
			/* The packet has the two smallest byte of the
			 * timestamp
			 */
			timestamp_len = 2;
		} else {
			/* The packet has the full timestamp. The first event
			 * will always have full timestamp. Save it in the
			 * timestamp buffer and use it for subsequent events if
			 * necessary.
			 */
			timestamp_len = 8;
			if ((temp_len + timestamp_len + 2) <= len)
				memcpy(timestamp, buf + temp_len + 2,
					timestamp_len);
			else {
				printf("diag: Invalid length in %s, len: %d, temp_len: %d\n",
						__func__, len, temp_len);
				return;
			}
		}
		/* 13th and 14th bit represent the payload length */
		if (((event_id_packet & 0x6000) >> 13) == 3) {
			payload_len_field = 1;
			if ((temp_len + timestamp_len + 3) <= len) {
				payload_len = *(uint8_t *)
					(buf + temp_len + 2 + timestamp_len);
			} else {
				printf("diag: Invalid length in %s, len: %d, temp_len: %d\n",
						__func__, len, temp_len);
				return;
			}
			if ((payload_len < (MAX_EVENT_SIZE - 13)) &&
			((temp_len + timestamp_len + payload_len + 3) <= len)) {
				/*
				 * Copy the payload length and the payload
				 * after skipping temp_len bytes for already
				 * parsed packet, timestamp_len for timestamp
				 * buffer, 2 bytes for event_id_packet.
				 */
				memcpy(event_data + 12, buf + temp_len + 2 +
							timestamp_len, 1);
				memcpy(event_data + 13, buf + temp_len + 2 +
					timestamp_len + 1, payload_len);
			} else {
				printf("diag: event > %d, payload_len = %d, temp_len = %d\n",
				(MAX_EVENT_SIZE - 13), payload_len, temp_len);
				return;
			}
		} else {
			payload_len_field = 0;
			payload_len = (event_id_packet & 0x6000) >> 13;
			/*
			 * Copy the payload after skipping temp_len bytes
			 * for already parsed packet, timestamp_len for
			 * timestamp buffer, 2 bytes for event_id_packet.
			 */
			if ((payload_len < (MAX_EVENT_SIZE - 12)) &&
			((temp_len + timestamp_len + payload_len + 2) <= len))
				memcpy(event_data + 12, buf + temp_len + 2 +
						timestamp_len, payload_len);
			else {
				printf("diag: event > %d, payload_len = %d, temp_len = %d\n",
				(MAX_EVENT_SIZE - 12), payload_len, temp_len);
				return;
			}
		}

		/* Before copying the data to userspace, check if we are still
		 * within the buffer limit. This is an error case, don't count
		 * it towards the health statistics.
		 *
		 * Here, the offset of 2 bytes(uint16_t) is for the
		 * event_id_packet length
		 */
		temp_len += sizeof(uint16_t) + timestamp_len +
						payload_len_field + payload_len;
		if (temp_len > len) {
			printf("diag: Invalid length in %s, len: %d, read: %d\n",
						__func__, len, temp_len);
			return;
		}

		/* 2 bytes for the event id & timestamp len is hard coded to 8,
		 * as individual events have full timestamp.
		 */
		*(uint16_t *)(event_data) = 10 +
					payload_len_field + payload_len;
		*(uint16_t *)(event_data + 2) = event_id_packet & 0x7FFF;
		memcpy(event_data + 4, timestamp, 8);
		/* 2 bytes for the event length field which is added to
		 * the event data.
		 */
		total_event_len = 2 + 10 + payload_len_field + payload_len;
		/* parse through event mask tbl of each client and check mask */
		pthread_mutex_lock(&diagmem->dci_mutex);
		list_for_each(start, &diagmem->dci_client_list) {
			entry = container_of(start, struct diag_dci_client_tbl,
									track);
			if (entry->client_info.token != token)
				continue;
			if (diag_dci_query_event_mask(entry, event_id)) {
				/* copy to client buffer */
				copy_dci_event(event_data, total_event_len,
					       entry, data_source, ext_hdr);
			}
		}
		pthread_mutex_unlock(&diagmem->dci_mutex);
	}
}
void extract_dci_pkt_rsp(unsigned char *buf, int len, int data_source,
			 int token)
{
	int tag;
	struct diag_dci_client_tbl *entry = NULL;
	void *temp_buf = NULL;
	uint8_t dci_cmd_code, cmd_code_len, delete_flag = 0;
	uint32_t rsp_len = 0;
	struct diag_dci_buffer_t *rsp_buf = NULL;
	struct dci_pkt_req_entry_t *req_entry = NULL;
	unsigned char *temp = buf;
	int save_req_uid = 0;
	struct diag_dci_pkt_rsp_header_t pkt_rsp_header;
	int header_len = sizeof(struct diag_dci_pkt_rsp_header_t);

	if (!buf || len <= 0) {
		printf("diag: Invalid pointer in %s\n", __func__);
		return;
	}

	dci_cmd_code = *(uint8_t *)(temp);
	if (dci_cmd_code == DCI_PKT_RSP_CODE) {
		cmd_code_len = sizeof(uint8_t);
	} else if (dci_cmd_code == DCI_DELAYED_RSP_CODE) {
		cmd_code_len = sizeof(uint32_t);
	} else {
		printf("diag: In %s, invalid command code %d\n", __func__,
								dci_cmd_code);
		return;
	}
	if (len < (cmd_code_len + sizeof(int)))
		return;
	temp += cmd_code_len;
	tag = *(int *)temp;
	temp += sizeof(int);

	/*
	 * The size of the response is (total length) - (length of the command
	 * code, the tag (int)
	 */
	if (len >= cmd_code_len + sizeof(int)) {
		rsp_len = len - (cmd_code_len + sizeof(int));
		if ((rsp_len == 0) || (rsp_len > (len - 5))) {
			printf("diag: Invalid length in %s, len: %d, rsp_len: %d\n",
					__func__, len, rsp_len);
			return;
		}
	} else {
		printf("diag:%s: Invalid length(%d) for calculating rsp_len\n",
			__func__, len);
		return;
	}
	pthread_mutex_lock(&diagmem->dci_mutex);
	req_entry = diag_dci_get_request_entry(tag);
	if (!req_entry) {
		printf("diag: No matching client for DCI data\n");
		pthread_mutex_unlock(&diagmem->dci_mutex);
		return;
	}

	entry = diag_dci_get_client_entry(req_entry->client_id);
	if (!entry) {
		printf("diag: In %s, couldn't find client entry, id:%d\n",
						__func__, req_entry->client_id);
		pthread_mutex_unlock(&diagmem->dci_mutex);
		return;
	}

	save_req_uid = req_entry->uid;

	/* Remove the headers and send only the response to this function */
	delete_flag = diag_dci_remove_req_entry(temp, rsp_len, req_entry);
	if (delete_flag < 0) {
		pthread_mutex_unlock(&diagmem->dci_mutex);
		return;
	}

	if (token != entry->client_info.token) {
		ALOGD("diag: %s: dci client token mismatch\n", __func__);
		pthread_mutex_unlock(&diagmem->dci_mutex);
		return;
	}

	rsp_buf = entry->buffers[data_source].buf_cmd;

	/*
	 * Check if we can fit the data in the rsp buffer. The total length of
	 * the rsp is the rsp length (write_len) + dci response packet header
	 * length (sizeof(struct diag_dci_pkt_rsp_header_t))
	 */
	if ((rsp_buf->data_len + header_len + rsp_len) > rsp_buf->capacity) {
		printf("diag: create capacity for pkt rsp\n");
		temp_buf = calloc(1, rsp_buf->capacity + header_len + rsp_len);
		if (!temp_buf) {
			printf("diag: DCI realloc failed\n");
			pthread_mutex_unlock(&diagmem->dci_mutex);
			return;
		}
		rsp_buf->capacity += header_len + rsp_len;
		if (rsp_buf->capacity > rsp_buf->data_len)
			memcpy(temp_buf, rsp_buf->data, rsp_buf->data_len);
		free(rsp_buf->data);
		rsp_buf->data = temp_buf;
	}

	/* Fill in packet response header information */
	pkt_rsp_header.type = DCI_PKT_RSP_TYPE;
	/* Packet Length = Response Length + Length of uid field (int) */
	pkt_rsp_header.length = rsp_len + sizeof(int);
	pkt_rsp_header.delete_flag = delete_flag;
	pkt_rsp_header.uid = save_req_uid;
	memcpy(rsp_buf->data + rsp_buf->data_len, &pkt_rsp_header, header_len);
	rsp_buf->data_len += header_len;
	memcpy(rsp_buf->data + rsp_buf->data_len, temp, rsp_len);
	rsp_buf->data_len += rsp_len;
	rsp_buf->data_source = data_source;

	/*
	 * Add directly to the list for writing responses to the
	 * userspace as these shouldn't be buffered and shouldn't wait
	 * for log and event buffers to be full
	 */
	dci_add_buffer_to_list(entry, rsp_buf);
	pthread_mutex_unlock(&diagmem->dci_mutex);
}
void extract_dci_ext_pkt(unsigned char *buf, int len, int data_source,
		int token)
{
	uint8_t version, pkt_cmd_code = 0;
	unsigned char *pkt = NULL;

	if (!buf) {
		printf("diag: In %s buffer is NULL\n", __func__);
		return;
	}
	if (len < (EXT_HDR_LEN + sizeof(uint8_t))) {
		printf("diag: In %s invalid len: %d\n", __func__, len);
		return;
	}

	version = *(uint8_t *)buf + 1;
	if (version < EXT_HDR_VERSION)  {
		printf("diag: %s, Extended header with invalid version: %d\n",
			__func__, version);
		return;
	}

	pkt = buf + EXT_HDR_LEN;
	pkt_cmd_code = *(uint8_t *)pkt;
	len -= EXT_HDR_LEN;

	switch (pkt_cmd_code) {
	case LOG_CMD_CODE:
		extract_dci_log(pkt, len, data_source, token, buf);
		break;
	case SECURE_LOG_CMD_CODE:
		extract_dci_secure_log(buf, len, data_source, token, NULL);
		break;
	case EVENT_CMD_CODE:
		extract_dci_events(pkt, len, data_source, token, buf);
		break;
	default:
		printf("diag: %s unsupported cmd_code: %d, data_source: %d\n",
			__func__, pkt_cmd_code, data_source);
		return;
	}
}
static int diag_process_single_dci_pkt(unsigned char *buf, int len,
				       int data_source, int token)
{
	uint8_t cmd_code = 0;

	if (!buf || len < 0) {
		printf("diag: Invalid input in %s, buf: %pK, len: %d\n",
			__func__, buf, len);
		return -EIO;
	}

	cmd_code = *(uint8_t *)buf;

	switch (cmd_code) {
	case LOG_CMD_CODE:
		extract_dci_log(buf, len, data_source, token, NULL);
		break;
	case SECURE_LOG_CMD_CODE:
		extract_dci_secure_log(buf, len, data_source, token, NULL);
		break;
	case EVENT_CMD_CODE:
		extract_dci_events(buf, len, data_source, token, NULL);
		break;
	case EXT_HDR_CMD_CODE:
		extract_dci_ext_pkt(buf, len, data_source, token);
		break;
	case DCI_PKT_RSP_CODE:
	case DCI_DELAYED_RSP_CODE:
		extract_dci_pkt_rsp(buf, len, data_source, token);
		break;
	default:
		printf("diag: Unable to process single DCI packet, cmd_code: %d, data_source: %d\n",
			cmd_code, data_source);
		return -EINVAL;
	}

	return 0;
}

static int diag_copy_dci(struct diag_dci_client_tbl *entry, unsigned char *buf, int count, struct watch_flow *flow)
{
	int total_data_len = 0;
	int ret = 0;
	int exit_stat = 1;
	uint8_t drain_again = 0;
	int data_type = DCI_DATA_TYPE;
	struct diag_dci_buffer_t *buf_entry, *temp;

	if (!buf || !entry)
		return exit_stat;

	memcpy(buf+ret, &data_type, sizeof(data_type));
	ret = ret + sizeof(data_type);
	memcpy(buf+ret, &entry->client_info.token, sizeof(int));
	ret = ret + sizeof(data_type);
	ret = ret +sizeof(int);

	list_for_each_entry_safe(buf_entry, temp,  &entry->list_write_buf,
				buf_track) {

		if ((ret + buf_entry->data_len) > count) {
			drain_again = 1;
			break;
		}

		list_del(&buf_entry->buf_track);
		if ((buf_entry->data_len > 0) &&
		    (buf_entry->in_busy) &&
		    (buf_entry->data)) {
			memcpy(buf+ret, (void *)buf_entry->data, buf_entry->data_len);
			ret += buf_entry->data_len;
			total_data_len += buf_entry->data_len;

			buf_entry->in_busy = 0;
			buf_entry->data_len = 0;
			buf_entry->in_list = 0;
			if (buf_entry->buf_type == DCI_BUF_CMD) {
				continue;
			} else if (buf_entry->buf_type == DCI_BUF_SECONDARY) {
				free(buf_entry->data);
				buf_entry->data = NULL;
				free(buf_entry);
				continue;
			} else {
				continue;
			}

		}
	}

	if (total_data_len >= 0) {
		/* Copy the total data length */
		memcpy(buf+8, &total_data_len, 4);
	} else {
		printf("diag: In %s, Trying to copy ZERO bytes, total_data_len: %d\n",
			__func__, total_data_len);
	}

	exit_stat = 0;
	entry->in_service = 0;
	ALOGI("diag: In %s senidng data %d entry %p buf %p\n",__func__, ret, entry->dm, buf);
	if (entry->dm) {
		 dm_send_flow(NULL, entry->dm, buf, ret, flow, 0);
	} else {
		if (entry->pid) {
#ifndef FEATURE_LE_DIAG
			diag_fwd_dci_data(entry->pid, buf, ret);
#endif
		}
	}
	return exit_stat;
}
void add_pending_buffers_to_list(int peripheral, struct diag_dci_client_tbl *entry)
{
	struct diag_dci_buf_peripheral_t *proc_buf = NULL;
	struct diag_dci_buffer_t *buf_temp = NULL;

	if (!entry)
		return;
	proc_buf = &entry->buffers[peripheral];
	if (!proc_buf)
		return;
	buf_temp = proc_buf->buf_primary;
	if (DCI_CAN_ADD_BUF_TO_LIST(buf_temp))
		dci_add_buffer_to_list(entry, buf_temp);

	buf_temp = proc_buf->buf_cmd;
	if (DCI_CAN_ADD_BUF_TO_LIST(buf_temp))
		 	dci_add_buffer_to_list(entry, buf_temp);

	buf_temp = proc_buf->buf_curr;
	if (DCI_CAN_ADD_BUF_TO_LIST(buf_temp)) {
		dci_add_buffer_to_list(entry, buf_temp);
		proc_buf->buf_curr = NULL;
	}
}
/* Process the data read from apps userspace client */
void diag_process_apps_dci_read_data(void *buf, int recd_bytes, struct watch_flow *flow)
{
	int err = 0;
	struct diag_dci_client_tbl *entry;
	unsigned char *dest_buf;
	struct list_head *start;

	if (!buf) {
		printf("diag: In %s, Null buf pointer\n", __func__);
		return;
	}

	err = diag_process_single_dci_pkt(buf, recd_bytes, PERIPHERAL_APPS,
					  DCI_LOCAL_PROC);

	pthread_mutex_lock(&diagmem->dci_mutex);
	list_for_each(start, &diagmem->dci_client_list) {
		dest_buf = alloca(100000);
		if (!dest_buf) {
			pthread_mutex_unlock(&diagmem->dci_mutex);
			return;
		}
		entry = container_of(start, struct diag_dci_client_tbl,
					track);
		add_pending_buffers_to_list(PERIPHERAL_APPS, entry);
		diag_copy_dci(entry, dest_buf, 100000, flow);
	}
	pthread_mutex_unlock(&diagmem->dci_mutex);
	if (err)
		return;

}
void diag_dci_process_peripheral_data(struct peripheral *perif, void *buf,
				      int recd_bytes)
{
	int read_bytes = 0, err = 0;
	uint16_t dci_pkt_len;
	struct diag_dci_pkt_header_t *header = NULL;
	uint8_t recv_pkt_cmd_code;
	struct diag_dci_client_tbl *entry;
	unsigned char *dest_buf;
	struct list_head *start;
	char *buf_ptr = (char *)buf;

	if (!buf || !perif){
		ALOGE("diag: invalid buf\n");
		return;
	}

	if (diagmem->num_dci_client == 0) {
		return;
	}
	while (read_bytes < recd_bytes) {
		header = (struct diag_dci_pkt_header_t *)buf_ptr;
		recv_pkt_cmd_code = header->pkt_code;
		dci_pkt_len = header->len;

		if ((dci_pkt_len + 5) > (recd_bytes - read_bytes)) {
			ALOGE("diag: Invalid length in %s, len: %d, dci_pkt_len: %d\n",
				__func__, recd_bytes, dci_pkt_len);
			return;
		}
		err = diag_process_single_dci_pkt(buf_ptr + 4, dci_pkt_len,
										  perif->periph_id,
										  DCI_LOCAL_PROC);
		if (err) {
			break;
		}
		read_bytes += 5 + dci_pkt_len;
		buf_ptr += 5 + dci_pkt_len;
	}
	pthread_mutex_lock(&diagmem->dci_mutex);
	list_for_each(start, &diagmem->dci_client_list) {
		dest_buf = alloca(100000);
		if (!dest_buf) {
			ALOGE("diag: In %s failed to alloc memory\n", __func__);
			pthread_mutex_unlock(&diagmem->dci_mutex);
			return;
		}
		entry = container_of(start, struct diag_dci_client_tbl,
					track);
		add_pending_buffers_to_list(perif->periph_id, entry);
		diag_copy_dci(entry, dest_buf, 100000, perif->dci_flow);
	}
	pthread_mutex_unlock(&diagmem->dci_mutex);
	if (err)
		return;
}
void diag_dci_notify_client(int peripheral_mask, int data, int proc)
{
	struct diag_dci_client_tbl *entry;
	struct list_head *start;
	union sigval value;
	int ret = 0;

	if (data == DIAG_STATUS_OPEN)
		dci_ops_tbl[proc].peripheral_status |= peripheral_mask;
	else
		dci_ops_tbl[proc].peripheral_status &= ~peripheral_mask;
	list_for_each(start, &diagmem->dci_client_list) {
		entry = list_entry(start, struct diag_dci_client_tbl, track);
		if (entry->client_info.token != proc)
			continue;
		if (entry->client_info.notification_list & peripheral_mask) {
			value.sival_int = 0;
			value.sival_int = peripheral_mask | data;
			ret = sigqueue(entry->pid, entry->client_info.signal_type, value);
			if (ret)
				ALOGE("diag: In %s: sigqueue failed ret: %d errno: %d\n", __func__, ret, errno);
		}
	}
}

static int diag_dci_init_local(void)
{
	struct dci_ops_tbl_t *temp = &dci_ops_tbl[DCI_LOCAL_PROC];

	create_dci_log_mask_tbl(temp->log_mask_composite, DCI_LOG_MASK_CLEAN);
	create_dci_event_mask_tbl(temp->event_mask_composite);
	temp->peripheral_status |= DIAG_CON_APSS;

	return 0;
}
static int diag_dci_init_ops_tbl(void)
{
	int err = 0;

	err = diag_dci_init_local();
	if (err)
		goto err;
	return 0;

err:
	return -ENOMEM;
}
int diag_dci_init(void)
{
	int ret = 0, i;

	diagmem->dci_tag = 0;
	for (i = 0; i < MAX_DCI_CLIENTS; i++)
		diagmem->dci_client_id[i] = 0;
	diagmem->num_dci_client = 0;
	ret = diag_dci_init_ops_tbl();
	if (ret)
		goto err;

	if (diagmem->apps_dci_buf == NULL) {
		diagmem->apps_dci_buf = malloc(DCI_BUF_SIZE);
		if (diagmem->apps_dci_buf == NULL)
			goto err;
	}
	list_init(&diagmem->dci_client_list);
	list_init(&diagmem->dci_req_list);

	return DIAG_DCI_NO_ERROR;
err:
	printf("diag: Could not initialize diag DCI buffers\n");
	free(diagmem->apps_dci_buf);
	diagmem->apps_dci_buf = NULL;

	return DIAG_DCI_NO_REG;
}
int diag_dci_get_support_list(struct diag_dci_peripherals_t *support_list)
{
	if (!support_list)
		return -ENOMEM;

	if (!VALID_DCI_TOKEN(support_list->proc))
		return -EIO;

	support_list->list = dci_ops_tbl[support_list->proc].peripheral_status;
	return DIAG_DCI_NO_ERROR;
}
int diag_dci_copy_health_stats(struct diag_dci_health_stats_proc *stats_proc)
{
	struct diag_dci_client_tbl *entry = NULL;
	struct diag_dci_health_t *health = NULL;
	struct diag_dci_health_stats *stats = NULL;
	int i, proc;

	if (!stats_proc)
		return -EINVAL;

	stats = &stats_proc->health;
	proc = stats_proc->proc;
	if (proc < -1 || proc >= NUM_PERIPHERALS)
		return -EINVAL;

	entry = diag_dci_get_client_entry(stats_proc->client_id);
	if (!entry)
		return DIAG_DCI_NOT_SUPPORTED;

	/*
	 * If the client has registered for remote processor, the
	 * proc field doesn't have any effect as they have only one buffer.
	 */
	if (entry->client_info.token)
		proc = 0;

	stats->stats.dropped_logs = 0;
	stats->stats.dropped_events = 0;
	stats->stats.received_logs = 0;
	stats->stats.received_events = 0;

	if (proc != -1) {
		if (proc >= 0 && proc <= NUM_PERIPHERALS-2) {
			health = &entry->buffers[proc+1].health;
		}
		else if (proc == NUM_PERIPHERALS-1) {
			health = &entry->buffers[0].health;
		}
		stats->stats.dropped_logs = health->dropped_logs;
		stats->stats.dropped_events = health->dropped_events;
		stats->stats.received_logs = health->received_logs;
		stats->stats.received_events = health->received_events;
		if (stats->reset_status) {
			health->dropped_logs = 0;
			health->dropped_events = 0;
			health->received_logs = 0;
			health->received_events = 0;
		}
		return DIAG_DCI_NO_ERROR;
	}

	for (i = 0; i < entry->num_buffers; i++) {
		health = &entry->buffers[i].health;
		stats->stats.dropped_logs += health->dropped_logs;
		stats->stats.dropped_events += health->dropped_events;
		stats->stats.received_logs += health->received_logs;
		stats->stats.received_events += health->received_events;
		if (stats->reset_status) {
			health->dropped_logs = 0;
			health->dropped_events = 0;
			health->received_logs = 0;
			health->received_events = 0;
		}
	}
	return DIAG_DCI_NO_ERROR;
}
int diag_dci_set_real_time(struct diag_dci_client_tbl *entry, uint8_t real_time)
{
	if (!entry) {
		printf("diag: In %s, invalid client entry\n", __func__);
		return 0;
	}
	entry->real_time = real_time;

	return 1;
}
uint8_t diag_dci_get_cumulative_real_time(int token)
{
	uint8_t real_time = MODE_NONREALTIME;
	struct list_head *start;
	struct diag_dci_client_tbl *entry = NULL;

	list_for_each(start, &diagmem->dci_client_list) {
		entry = container_of(start, struct diag_dci_client_tbl, track);
		if (entry->real_time == MODE_REALTIME &&
			entry->client_info.token == token) {
			real_time = 1;
			break;
		}
	}
	return real_time;
}

int diag_cleanup_dci_client_entry(int pid)
{
	struct list_head *start;
	struct diag_dci_client_tbl *entry = NULL, *temp_entry = NULL;
	list_for_each(start, &diagmem->dci_client_list) {
		temp_entry = container_of(start, struct diag_dci_client_tbl, track);
		if (temp_entry->pid == pid) {
			entry = temp_entry;
			break;
		}
	}
	if (!entry) {
		return DIAG_DCI_NOT_SUPPORTED;
	} else  {
		return diag_dci_deinit_client(entry);
	}
}

int diag_dci_vote_peripheral_buffering_mode(struct diag_dci_buffering_mode_t *params, int len)
{
	struct diag_dci_client_tbl *dci_client = NULL;
	struct list_head *start, *temp;
	struct diag_dci_client_tbl *entry = NULL, *temp_entry = NULL;
	int dci_buffering_mode = 0;

	/* Add +1 to peripheral since peripheral id is different from diag lib */
	params->buffering_mode_params.peripheral = params->buffering_mode_params.peripheral + 1;
	if (!check_peripheral_dci_support(params->buffering_mode_params.peripheral, DCI_LOCAL_PROC)) {
		ALOGE("diag: In %s, invalid peripheral %d\n", __func__, params->buffering_mode_params.peripheral);
		return -EINVAL;
	}

	dci_client = diag_dci_get_client_entry(params->client_id);
	if (!dci_client) {
		ALOGE("diag: In %s invalid dci client entry\n", __func__);
		return -EINVAL;
	}

	dci_client->vote_buffering_mode[DCI_LOCAL_PROC][params->buffering_mode_params.peripheral].mode = params->buffering_mode_params.mode;

	pthread_mutex_lock(&diagmem->dci_mutex);
	list_for_each(start, &diagmem->dci_client_list) {
		entry = container_of(start, struct diag_dci_client_tbl,
						track);
		if (entry->vote_buffering_mode[DCI_LOCAL_PROC][params->buffering_mode_params.peripheral].mode == DIAG_BUFFERING_MODE_STREAMING) {
			dci_buffering_mode = 0;
			break;
		}
		dci_buffering_mode = 1;
	}
	pthread_mutex_unlock(&diagmem->dci_mutex);

	if (dci_buffering_mode) {
		diag_dci_send_buffering_mode_pkt(&params->buffering_mode_params);
		diag_dci_send_buffering_mode_update_to_clients(DCI_LOCAL_PROC, params->buffering_mode_params.mode);
		diagmem->dci_buffering_mode[DCI_LOCAL_PROC][params->buffering_mode_params.peripheral].mode = params->buffering_mode_params.mode;
	} else {
		/* If current mode is buffering mode and requested mode is streaming mode then fwd streaming mode
		   ctrl pkt to peripheral and status updates to clients */
		if (params->buffering_mode_params.mode == DIAG_BUFFERING_MODE_STREAMING) {
			if (diagmem->dci_buffering_mode[DCI_LOCAL_PROC][params->buffering_mode_params.peripheral].mode != DIAG_BUFFERING_MODE_STREAMING) {
				diag_dci_send_buffering_mode_pkt(&params->buffering_mode_params);
				diag_dci_send_buffering_mode_update_to_clients(DCI_LOCAL_PROC, params->buffering_mode_params.mode);
				diagmem->dci_buffering_mode[DCI_LOCAL_PROC][params->buffering_mode_params.peripheral].mode = params->buffering_mode_params.mode;
			}
		}
	}

	return 0;
}

int diag_dci_peripheral_data_drain_immediate(struct diag_dci_perif_drain *params, int len)
{
	uint8_t pd, diag_id = 0;
	int peripheral = 0, stream_id = DIAG_STREAM_2;

	pd = params->peripheral;
	if (pd >= 0 && pd < NUM_PERIPHERALS-1)
		pd = pd + 1;
	diag_map_pd_to_diagid(pd, &diag_id, &peripheral);

	if ((peripheral < PERIPHERAL_MODEM) ||
		peripheral >= NUM_PERIPHERALS) {
		ALOGE("diag: In %s, invalid peripheral %d\n", __func__, peripheral);
		return -EINVAL;
	}

	if (!check_peripheral_dci_support(peripheral, DCI_LOCAL_PROC)) {
		ALOGE("diag: In %s, invalid peripheral %d\n", __func__, peripheral);
		return -EINVAL;
	}

	return diag_cntl_drain_perif_buf(diag_id, peripheral, stream_id);
}

int diag_dci_send_buffering_mode_update_to_clients(int proc, int mode)
{
	int total_len = 0, data_type;
	unsigned char *pkt = NULL;

	data_type = DCI_BUFFERING_MODE_STATUS_TYPE;
	pkt = alloca(sizeof(int) * 3);
	if (!pkt) {
		ALOGE("diag: In %s failed to alloc memory\n", __func__);
		return 0;
	}
	memcpy(pkt, &data_type, sizeof(int));
	total_len = total_len + sizeof(int);
	memcpy(pkt + sizeof(proc), &proc, sizeof(int));
	total_len = total_len + sizeof(int);
	memcpy(pkt + sizeof(data_type) + sizeof(proc), &mode, sizeof(int));
	total_len = total_len + sizeof(int);
	if (pkt != NULL)
		dm_broadcast_to_socket_clients(pkt, total_len);
#ifndef FEATURE_LE_DIAG
	diag_update_hidl_client(DCI_BUFFERING_MODE_STATUS_TYPE);
#endif

	return 0;
}

