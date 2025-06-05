/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
# Copyright (c) 2012-2014,2016,2018,2020-2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag Consumer Interface (DCI)

GENERAL DESCRIPTION

Implementation of functions specific to DCI.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header:

when       who    what, where, why
--------   ---    ----------------------------------------------------------
10/08/12   RA     Interface Implementation for DCI I/O
03/20/12   SJ     Created
===========================================================================*/

#include <stdlib.h>
#include "comdef.h"
#include "stdio.h"
#include "diaglogi.h"
#include "diag_lsmi.h"
#include "diagsvc_malloc.h"
#include "diag_lsm_event_i.h"
#include "diag_lsm_log_i.h"
#include "diag_lsm_msg_i.h"
#include "diag.h" /* For definition of diag_cmd_rsp */
#include "diag_lsm_pkt_i.h"
#include "diag_lsm_dci_i.h"
#include "diag_lsm_dci.h"
#include "diag_shared_i.h" /* For different constants */
#include "diag_lsm_comm.h"
#include "diag_lsm.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "errno.h"
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <eventi.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>

#define BITS_PER_BYTE		8
#define BIT(x)				(1 << (x))

int dci_transaction_id;
int num_dci_proc;
pthread_mutex_t dci_init_mutex = PTHREAD_MUTEX_INITIALIZER;
struct diag_dci_client_tbl *dci_client_tbl = NULL;

static inline int diag_dci_get_proc(int handle)
{
	int i;
	for (i = 0; i < num_dci_proc && dci_client_tbl; i++)
		if (dci_client_tbl[i].handle == handle)
			return i;

	return -1;
}

int diag_lsm_dci_init(void)
{
	int i, j, num_remote_proc = 0;
	uint16 remote_proc = 0;
	struct diag_dci_client_tbl *temp = NULL;
	int rc = DIAG_DCI_NO_ERROR;

	pthread_mutex_lock(&dci_init_mutex);
	if (dci_client_tbl)
		goto init_exit;

	diag_has_remote_device(&remote_proc);
	if (remote_proc) {
		for (num_remote_proc = 0; remote_proc; num_remote_proc++)
			remote_proc &= remote_proc - 1;
	} else {
		DIAG_LOGE("diag: Unable to get remote processor info. Continuing with just the local processor\n");
	}

	num_dci_proc = num_remote_proc + 1; /* Add 1 for Local processor */
	dci_client_tbl = (struct diag_dci_client_tbl *)malloc(num_dci_proc * sizeof(struct diag_dci_client_tbl));
	if (!dci_client_tbl) {
		rc = DIAG_DCI_NO_MEM;
		goto init_exit;
	}

	dci_transaction_id = 0;
	for (i = 0; i < num_dci_proc; i++) {
		temp = &dci_client_tbl[i];
		temp->dci_req_buf = NULL;
		diag_pkt_rsp_tracking_tbl *head = &temp->req_tbl_head;
		head->next = head;
		head->prev = head;
		head->info = NULL;
		pthread_mutex_init(&(dci_client_tbl[i].req_tbl_mutex), NULL);
		temp->client_info.notification_list = 0;
		temp->client_info.signal_type = 0;
		temp->client_info.token = i;
		temp->client_info.client_id = INVALID_DCI_CLIENT;
		temp->handle = INVALID_DCI_CLIENT;
		temp->data_signal_flag = DISABLE;
		temp->data_signal_type = DIAG_INVALID_SIGNAL;
		temp->func_ptr_logs = (void *)NULL;
		temp->func_ptr_events = (void *)NULL;
		temp->version = 0;
		temp->dci_tx_mode_params.mode = 0;
		temp->dci_tx_mode_params.low_wm_val = 0;
		temp->dci_tx_mode_params.high_wm_val = 0;
		temp->func_ptr_dci_buffering_status = (void *)NULL;

		memset(&temp->requested_logs, 0, sizeof(struct dci_log_mask));
		for (j = 0; j < DCI_MAX_LOG_EQUIP_ID; j++) {
			temp->requested_logs.equips[j].equip_id = j;
		}
		memset(&temp->requested_events, 0, sizeof(struct dci_event_mask));
		pthread_mutex_init(&temp->req_tbl_mutex, NULL);
	}

init_exit:
	pthread_mutex_unlock(&dci_init_mutex);
	return rc;
}

void diag_lsm_dci_deinit(void)
{
	pthread_mutex_lock(&dci_init_mutex);
	if (dci_client_tbl) {
		free(dci_client_tbl);
		dci_client_tbl = NULL;
	}
	pthread_mutex_unlock(&dci_init_mutex);
}

int diag_dci_get_version(void)
{
	return DCI_VERSION;
}

int diag_dci_set_version(int handle, int version)
{
	int proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_PARAM_FAIL;

	if (!dci_client_tbl) {
		DIAG_LOGE(" diag: In %s, dci_client_tbl is NULL\n", __func__);
		return DIAG_DCI_NO_REG;
	}

	if (version < 0 || version > DCI_VERSION) {
		DIAG_LOGE(" diag: In %s, Unsupported version req:%d cur:%d\n",
			__func__, version, DCI_VERSION);
		return DIAG_DCI_NOT_SUPPORTED;
	}

	dci_client_tbl[proc].version = version;
	return DIAG_DCI_NO_ERROR;
}

static void dci_delete_request_entry(diag_pkt_rsp_tracking_tbl *entry)
{
	if (!entry)
		return;
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
	free(entry->info);
	free(entry);
}

void lookup_pkt_rsp_transaction(unsigned char *ptr, int proc)
{
	int uid, found = 0;
	uint32_t len;
	uint8 delete_flag = 0;
	unsigned char *temp;
	diag_pkt_rsp_tracking_tbl *head = NULL, *walk_ptr = NULL;
	diag_pkt_tracking_info info;

	if (!ptr) {
		DIAG_LOGE("  Invalid pointer in %s\n", __func__);
		return;
	}

	if (!IS_VALID_DCI_PROC(proc)) {
		DIAG_LOGE("  Invalid proc %d in %s\n", proc, __func__);
		return;
	}

	temp = ptr;
	len = *(uint32_t *)temp;
	if (len == 0 || len > DIAG_MAX_RX_PKT_SIZ) {
		DIAG_LOGE("Diag: %s: Invalid length: %d, Max supported "
			"response length: %d\n", __func__, len, DIAG_MAX_RX_PKT_SIZ);
		return;
	}

	temp += sizeof(int);
	delete_flag = *(uint8 *)temp;
	temp += sizeof(uint8);
	uid = *(int *)temp;
	temp += sizeof(int);
	len = len - sizeof(int); /* actual length of response */
	memset(&info, 0, sizeof(diag_pkt_tracking_info));

	if (!dci_client_tbl) {
		DIAG_LOGE("Diag: %s: Invalid dci_client table for proc: %d\n", __func__, proc);
		return;
	}

	pthread_mutex_lock(&(dci_client_tbl[proc].req_tbl_mutex));
	head = &dci_client_tbl[proc].req_tbl_head;
	for (walk_ptr = head->next; walk_ptr && walk_ptr != head; walk_ptr = walk_ptr->next) {
		if (!walk_ptr->info || walk_ptr->info->uid != uid)
			continue;
		/*
		 * Found a match. Copy the response to the buffer and call
		 * the corresponding response handler
		 */
		if (len > 0 && len <= walk_ptr->info->rsp_len) {
			memcpy(&info, walk_ptr->info, sizeof(diag_pkt_tracking_info));
			memcpy(info.rsp_ptr, temp, len);
		} else {
			DIAG_LOGE(" Invalid response in %s, len:%d rsp_len: %d\n", __func__, len, walk_ptr->info->rsp_len);
		}
		/*
		 * Delete Flag will be set if it is safe to delete the entry.
		 * This means that the response is either a regular response or
		 * the last response in a sequence of delayed responses.
		 */
		if (delete_flag)
			dci_delete_request_entry(walk_ptr);
		found = 1;
		break;
	}
	pthread_mutex_unlock(&(dci_client_tbl[proc].req_tbl_mutex));

	if (found) {
		if (info.func_ptr)
			(*(info.func_ptr))(info.rsp_ptr, len, info.data_ptr);
	} else {
		DIAG_LOGE("  In %s, incorrect transaction %d, proc: %d\n", __func__, uid, proc);
	}
}

static int dci_log_reinit(struct diag_dci_client_tbl *cli)
{
	struct dci_log_mask *log_mask = &cli->requested_logs;
	struct diag_dci_stream_header_t *header;
	uint16 *log_array;
	int item_num;
	int buf_len;
	uint8 *buf;
	int count;
	int err;
	int i;

	buf = malloc(DCI_MAX_REQ_BUF_SIZE + sizeof(struct diag_dci_stream_header_t));
	if (!buf) {
		DIAG_LOGE("diag: %s: malloc failed error:%d\n", __func__, errno);
		return -1;
	}

	header = (struct diag_dci_stream_header_t *)buf;
	header->start = DCI_DATA_TYPE;
	header->type = DCI_LOG_TYPE;
	header->client_id = cli->client_info.client_id;
	header->set_flag = ENABLE;

	log_array = buf + sizeof(struct diag_dci_stream_header_t);
	count = 0;
	for (i = 0; i < (MAX_EQUIP_ID * DCI_MAX_ITEMS_PER_LOG_CODE); i++) {
		int equip_id = i / DCI_MAX_ITEMS_PER_LOG_CODE;
		int byte_index = i % DCI_MAX_ITEMS_PER_LOG_CODE;
		int bit_index;
		uint8 mask;

		mask = log_mask->equips[equip_id].items[byte_index];
		if (!mask)
			continue;

		while (mask) {
			uint16 log_code;

			/* find first set bit position */
			bit_index = log2(mask - (mask & (mask - 1)));
			item_num = (byte_index * BITS_PER_BYTE) + bit_index;

			log_code = LOG_CODE(equip_id, item_num);
			*log_array = log_code;
			count++;
			*log_array++;

			/* remove first set bit */
			mask = mask & (mask - 1);

			/* Reducing count by arbitrary 8 to avoid inconsistent checks in diag-router */
			if (count == (DCI_MAX_REQ_BUF_SIZE / sizeof(log_code)) - 8) {
				header->count = count;
				buf_len = sizeof(struct diag_dci_stream_header_t) + (count * sizeof(log_code));

				err = diag_send_data(buf, buf_len);
				if (err != DIAG_DCI_NO_ERROR)
					DIAG_LOGE("diag: %s: failed to send log mask err:%d\n", __func__, err);

				/* reset log_array position and count after sending */
				log_array = buf + sizeof(struct diag_dci_stream_header_t);
				count = 0;
			}
		}
	}

	if (count) {
		header->count = count;
		buf_len = sizeof(struct diag_dci_stream_header_t) + (count * sizeof(uint16));

		err = diag_send_data(buf, buf_len);
		if (err != DIAG_DCI_NO_ERROR)
			DIAG_LOGE("diag: %s: failed to send log mask err:%d\n", __func__, err);
	}
	free(buf);

	return 0;
}

static int dci_event_reinit(struct diag_dci_client_tbl *cli)
{
	struct dci_event_mask *event_mask = &cli->requested_events;
	struct diag_dci_stream_header_t *header;
	uint32 *event_array;
	int buf_len;
	uint8 *buf;
	int count;
	int err;
	int i;

	buf = malloc(DCI_MAX_REQ_BUF_SIZE + sizeof(struct diag_dci_stream_header_t));
	if (!buf) {
		DIAG_LOGE("diag: %s: malloc failed error:%d\n", __func__, errno);
		return -1;
	}

	header = (struct diag_dci_stream_header_t *)buf;
	header->start = DCI_DATA_TYPE;
	header->type = DCI_EVENT_TYPE;
	header->client_id = cli->client_info.client_id;
	header->set_flag = ENABLE;

	event_array = buf + sizeof(struct diag_dci_stream_header_t);
	count = 0;
	for (i = 0; i < DCI_MAX_EVENT_ITEMS; i++) {
		uint8 mask;
		int index;

		mask = event_mask->mask[i];
		if (!mask)
			continue;

		while (mask) {
			uint32 event;

			/* find first set bit position */
			index = log2(mask - (mask & (mask - 1)));

			event = (i * BITS_PER_BYTE) + index;
			*event_array = event;
			count++;
			*event_array++;

			/* remove first set bit */
			mask = mask & (mask - 1);

			/* Reducing count by arbitrary 8 to avoid inconsistent checks in diag-router */
			if (count == (DCI_MAX_REQ_BUF_SIZE / sizeof(event)) - 8) {
				header->count = count;
				buf_len = sizeof(struct diag_dci_stream_header_t) + (count * sizeof(event));

				err = diag_send_data(buf, buf_len);
				if (err != DIAG_DCI_NO_ERROR)
					DIAG_LOGE("diag: %s: failed to send event mask err:%d\n", __func__, err);

				/* reset log_array position and count after sending */
				event_array = buf + sizeof(struct diag_dci_stream_header_t);
				count = 0;
			}
		}
	}

	if (count) {
		header->count = count;
		buf_len = sizeof(struct diag_dci_stream_header_t) + (count * sizeof(uint32));

		err = diag_send_data(buf, buf_len);
		if (err != DIAG_DCI_NO_ERROR)
			DIAG_LOGE("diag: %s: failed to send event mask err:%d\n", __func__, err);
	}
	free(buf);

	return 0;
}

int diag_lsm_dci_reinit(void)
{
	struct diag_dci_reg_tbl_t *client_info;
	int old_client_id;
	int ret;
	int i;

	for (i = 0; i < num_dci_proc; i++) {
		diag_pkt_rsp_tracking_tbl *walk_ptr;
		diag_pkt_rsp_tracking_tbl *head;

		if (dci_client_tbl[i].handle == INVALID_DCI_CLIENT)
			continue;

		client_info = &dci_client_tbl[i].client_info;
		client_info->client_id = INVALID_DCI_CLIENT;

		/* We should really give clients a callback for any outstanding requests in the req table
		 * but for now free resources on reinit, let the client timeout and retry
		 */
		pthread_mutex_lock(&dci_client_tbl[i].req_tbl_mutex);
		head = &dci_client_tbl[i].req_tbl_head;
		for (walk_ptr = head->next; walk_ptr && walk_ptr != head; walk_ptr = head->next)
			dci_delete_request_entry(walk_ptr);
		pthread_mutex_unlock(&dci_client_tbl[i].req_tbl_mutex);

		ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_DCI_REG, client_info, 0);
		if (ret == DIAG_DCI_NO_REG || ret < 0) {
			DIAG_LOGE("diag: %s: reg failed ret: %d error: %d\n", __func__, ret, errno);
			return -1;
		} else {
			client_info->client_id = ret;
		}

		/* Try to reinitialize the cached logs and events */
		dci_log_reinit(&dci_client_tbl[i]);
		dci_event_reinit(&dci_client_tbl[i]);
		if (dci_client_tbl[i].dci_tx_mode_params.mode) {
			diag_dci_configure_buffering_mode(i, 0, dci_client_tbl[i].dci_tx_mode_params.mode,
								dci_client_tbl[i].dci_tx_mode_params.low_wm_val,
								dci_client_tbl[i].dci_tx_mode_params.high_wm_val);
		}
	}
	return 0;
}

int diag_register_dci_client(int *handle, diag_dci_peripherals *list, int proc, void *os_params)
{
	struct diag_dci_client_tbl *client;
	int ret = 0;
	int req_buf_len = DCI_MAX_REQ_BUF_SIZE;

	ret = diag_lsm_dci_init();
	if (ret != DIAG_DCI_NO_ERROR)
		return ret;

	/* Make place for the header - Choose the header that has maximum size */
	if (sizeof(struct diag_dci_req_header_t) > sizeof(struct diag_dci_stream_header_t))
		req_buf_len += sizeof(struct diag_dci_req_header_t);
	else
		req_buf_len += sizeof(struct diag_dci_stream_header_t);

	if (!handle)
		return ret;
	if (!IS_VALID_DCI_PROC(proc))
		return ret;

	if (dci_client_tbl[proc].client_info.client_id != INVALID_DCI_CLIENT ||
		dci_client_tbl[proc].handle != INVALID_DCI_CLIENT) {
		DIAG_LOGE("diag: There is already a DCI client registered for this proc: %d\n", proc);
		return DIAG_DCI_DUP_CLIENT;
	}

	dci_client_tbl[proc].client_info.notification_list = *list;
	dci_client_tbl[proc].client_info.signal_type = *(int *)os_params;
	dci_client_tbl[proc].client_info.token = proc;
	dci_client_tbl[proc].data_signal_flag = DISABLE;
	dci_client_tbl[proc].data_signal_type = DIAG_INVALID_SIGNAL;
	dci_client_tbl[proc].dci_req_buf = (unsigned char *)malloc(req_buf_len);
	if (!dci_client_tbl[proc].dci_req_buf)
		return DIAG_DCI_NO_MEM;

	ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_DCI_REG, &dci_client_tbl[proc].client_info, 0);

	if (ret == DIAG_DCI_NO_REG || ret < 0) {
		DIAG_LOGE(" could not register client, ret: %d error: %d\n", ret, errno);
		dci_client_tbl[proc].client_info.client_id = INVALID_DCI_CLIENT;
		*handle = INVALID_DCI_CLIENT;
		ret = DIAG_DCI_NO_REG;
	} else {
		dci_client_tbl[proc].client_info.client_id = ret;
		dci_client_tbl[proc].handle = proc;
		*handle = proc;
		ret = DIAG_DCI_NO_ERROR;
	}

	return ret;
}

int diag_register_dci_stream(void (*func_ptr_logs)(unsigned char *ptr, int len),
			     void (*func_ptr_events)(unsigned char *ptr, int len))
{
	if (!dci_client_tbl)
		return DIAG_DCI_NO_MEM;
	return diag_register_dci_stream_proc(dci_client_tbl[DIAG_PROC_MSM].handle, func_ptr_logs, func_ptr_events);
}

int diag_register_dci_stream_proc(int handle, void(*func_ptr_logs)(unsigned char *ptr, int len),
				  void(*func_ptr_events)(unsigned char *ptr, int len))
{
	int proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;

	dci_client_tbl[proc].func_ptr_logs = func_ptr_logs;
	dci_client_tbl[proc].func_ptr_events = func_ptr_events;
	return DIAG_DCI_NO_ERROR;
}

int diag_release_dci_client(int *handle)
{
	int result = 0, proc;
	int client_id;
	diag_pkt_rsp_tracking_tbl *head = NULL, *walk_ptr = NULL;

	if (!handle)
		return DIAG_DCI_NO_REG;

	proc = diag_dci_get_proc(*handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;
	client_id = dci_client_tbl[proc].client_info.client_id;

	result = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_DCI_DEINIT, &client_id, sizeof(client_id));
	if (result != DIAG_DCI_NO_ERROR) {
		DIAG_LOGE(" diag: could not remove entries, result: %d error: %d\n", result, errno);
		return DIAG_DCI_ERR_DEREG;
	} else {
		*handle = 0;
		dci_client_tbl[proc].client_info.client_id = INVALID_DCI_CLIENT;
		dci_client_tbl[proc].handle = INVALID_DCI_CLIENT;

		/* Delete the client requests */
		pthread_mutex_lock(&(dci_client_tbl[proc].req_tbl_mutex));
		head = &dci_client_tbl[proc].req_tbl_head;
		for (walk_ptr = head->next; walk_ptr && walk_ptr != head; walk_ptr = head->next)
			dci_delete_request_entry(walk_ptr);
		pthread_mutex_unlock(&(dci_client_tbl[proc].req_tbl_mutex));

		free(dci_client_tbl[proc].dci_req_buf);

		diag_lsm_dci_deinit();

		return DIAG_DCI_NO_ERROR;
	}
}

static diag_pkt_rsp_tracking_tbl *diag_register_dci_pkt(int proc, void (*func_ptr)(unsigned char *ptr, int len, void *data_ptr),
							int uid, unsigned char *rsp_ptr, int rsp_len, void *data_ptr)
{
	diag_pkt_tracking_info *req_info = NULL;
	diag_pkt_rsp_tracking_tbl *temp = NULL;
	diag_pkt_rsp_tracking_tbl *new_req = NULL;
	diag_pkt_rsp_tracking_tbl *head = NULL;
	if (!IS_VALID_DCI_PROC(proc))
		return NULL;

	req_info = (diag_pkt_tracking_info *)malloc(sizeof(diag_pkt_tracking_info));
	if (!req_info)
		return NULL;
	new_req = (diag_pkt_rsp_tracking_tbl *)malloc(sizeof(diag_pkt_rsp_tracking_tbl));
	if (!new_req) {
		free(req_info);
		return NULL;
	}

	req_info->uid = uid;
	req_info->func_ptr = func_ptr;
	req_info->rsp_ptr = rsp_ptr;
	req_info->rsp_len = rsp_len;
	req_info->data_ptr = data_ptr;
	new_req->info = req_info;
	new_req->next = new_req->prev = NULL;

	pthread_mutex_lock(&(dci_client_tbl[proc].req_tbl_mutex));
	head = &dci_client_tbl[proc].req_tbl_head;
	temp = head->prev;
	head->prev = new_req;
	new_req->next = head;
	new_req->prev = temp;
	temp->next = new_req;
	pthread_mutex_unlock(&(dci_client_tbl[proc].req_tbl_mutex));

	return new_req;
}

int diag_send_dci_async_req(int handle, unsigned char buf[], int bytes, unsigned char *rsp_ptr, int rsp_len,
					   void (*func_ptr)(unsigned char *ptr, int len, void *data_ptr), void *data_ptr)
{
	int err = -1, proc;
	diag_pkt_rsp_tracking_tbl *new_req = NULL;
	struct diag_dci_req_header_t header;
	unsigned char *dci_req_buf = NULL;
	unsigned int header_len = sizeof(struct diag_dci_req_header_t);

	proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;

	if ((bytes > DCI_MAX_REQ_BUF_SIZE) || (bytes < 1)) {
		DIAG_LOGE("diag: In %s, huge packet: %d, max supported: %d\n",
			  __func__, bytes, DCI_MAX_REQ_BUF_SIZE);
		return DIAG_DCI_HUGE_PACKET;
	}

	if (!buf) {
		DIAG_LOGE("diag: Request Bufffer is not set\n");
		return DIAG_DCI_NO_MEM;
	}

	dci_req_buf = dci_client_tbl[proc].dci_req_buf;

	if (!dci_req_buf) {
		DIAG_LOGE("diag: Request Buffer not initialized\n");
		return DIAG_DCI_NO_MEM;
	}
	if (!rsp_ptr) {
		DIAG_LOGE("diag: Response Buffer not initialized\n");
		return DIAG_DCI_NO_MEM;
	}
	dci_transaction_id++;
	new_req = diag_register_dci_pkt(proc, func_ptr, dci_transaction_id, rsp_ptr, rsp_len, data_ptr);
	if (!new_req)
		return DIAG_DCI_NO_MEM;
	header.start = DCI_DATA_TYPE;
	header.uid = dci_transaction_id;
	header.client_id = dci_client_tbl[proc].client_info.client_id;
	memcpy(dci_req_buf, &header, header_len);
	memcpy(dci_req_buf + header_len, buf, bytes);
	err = diag_send_data(dci_req_buf, header_len + bytes);

	/* Registration failed. Delete entry from registration table */
	if (err != DIAG_DCI_NO_ERROR) {
		pthread_mutex_lock(&(dci_client_tbl[proc].req_tbl_mutex));
		dci_delete_request_entry(new_req);
		pthread_mutex_unlock(&(dci_client_tbl[proc].req_tbl_mutex));
		err = DIAG_DCI_SEND_DATA_FAIL;
	}

	return err;
}

int diag_get_dci_support_list(diag_dci_peripherals *list)
{
	return diag_get_dci_support_list_proc(DIAG_PROC_MSM, list);
}

int diag_get_dci_support_list_proc(int proc, diag_dci_peripherals *list)
{
	struct diag_dci_peripheral_list_t p_list;
	int err = DIAG_DCI_NO_ERROR;

	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_PARAM_FAIL;

	if (!list)
		return DIAG_DCI_NO_MEM;

	p_list.proc = proc;
	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_DCI_SUPPORT, &p_list, sizeof(p_list));
	if (err == DIAG_DCI_NO_ERROR)
		*list = p_list.list;

	return err;
}

static void dci_cache_log(struct dci_log_mask *log_mask, uint16 log_code, int enable)
{
	uint16 item_num = LOG_GET_ITEM_NUM(log_code);
	uint8 equip_id = LOG_GET_EQUIP_ID(log_code);
	int byte_index = item_num / BITS_PER_BYTE;
	int bit_index = item_num % BITS_PER_BYTE;
	uint8 byte_mask;
	int i;

	if (byte_index >= DCI_MAX_ITEMS_PER_LOG_CODE)
		return;
	if (equip_id >= DCI_MAX_LOG_EQUIP_ID)
		return;

	byte_mask = BIT(bit_index);
	if (enable)
		log_mask->equips[equip_id].items[byte_index] |= byte_mask;
	else
		log_mask->equips[equip_id].items[byte_index] &= ~byte_mask;
}

int diag_log_stream_config(int handle, int set_mask, uint16 log_codes_array[], int num_codes)
{
	int err = -1, proc, i;
	struct diag_dci_stream_header_t header;
	unsigned char *dci_req_buf = NULL;
	unsigned int header_len = sizeof(struct diag_dci_stream_header_t);
	unsigned int data_len = sizeof(uint16) * num_codes;

	proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;
	if (num_codes < 1)
		return DIAG_DCI_PARAM_FAIL;
	dci_req_buf = dci_client_tbl[proc].dci_req_buf;
	if (!dci_req_buf)
		return DIAG_DCI_NO_MEM;
	if (data_len > DCI_MAX_REQ_BUF_SIZE) {
		DIAG_LOGE("diag: In %s, huge packet: %d/%d\n", __func__,
			  data_len, DCI_MAX_REQ_BUF_SIZE);
		return DIAG_DCI_HUGE_PACKET;
	}

	header.start = DCI_DATA_TYPE;
	header.type = DCI_LOG_TYPE;
	header.client_id = dci_client_tbl[proc].client_info.client_id;
	header.set_flag = set_mask;
	header.count = num_codes;
	memcpy(dci_req_buf, &header, header_len);
	memcpy(dci_req_buf + header_len, log_codes_array, data_len);
	err = diag_send_data(dci_req_buf, header_len + data_len);
	if (err != DIAG_DCI_NO_ERROR)
		return DIAG_DCI_SEND_DATA_FAIL;

	for (i = 0; i < num_codes; i++) {
		struct dci_log_mask *log_mask = &dci_client_tbl[proc].requested_logs;
		uint16 log_code = log_codes_array[i];

		dci_cache_log(log_mask, log_code, set_mask);
	}

	return DIAG_DCI_NO_ERROR;
}

static void dci_cache_event(struct dci_event_mask *event_mask, int id, int enable)
{
	int byte_index = id / BITS_PER_BYTE;
	int bit_index = id % BITS_PER_BYTE;
	uint8 byte_mask;

	if (byte_index >= sizeof(*event_mask))
		return;

	byte_mask = BIT(bit_index);
	if (enable)
		event_mask->mask[byte_index] |= byte_mask;
	else
		event_mask->mask[byte_index] &= ~byte_mask;
}

int diag_event_stream_config(int handle, int set_mask, int event_id_array[], int num_id)
{
	int err = -1, proc, i;
	struct diag_dci_stream_header_t header;
	unsigned char *dci_req_buf = NULL;
	unsigned int header_len = sizeof(struct diag_dci_stream_header_t);
	unsigned int data_len = sizeof(int) * num_id;

	proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;
	if (num_id < 1)
		return DIAG_DCI_PARAM_FAIL;
	dci_req_buf = dci_client_tbl[proc].dci_req_buf;
	if (!dci_req_buf)
		return DIAG_DCI_NO_MEM;
	if (data_len > DCI_MAX_REQ_BUF_SIZE) {
		DIAG_LOGE("diag: In %s, huge packet: %d/%d\n", __func__,
			  data_len, DCI_MAX_REQ_BUF_SIZE);
		return DIAG_DCI_HUGE_PACKET;
	}

	header.start = DCI_DATA_TYPE;
	header.type = DCI_EVENT_TYPE;
	header.client_id = dci_client_tbl[proc].client_info.client_id;
	header.set_flag = set_mask;
	header.count = num_id;
	memcpy(dci_req_buf, &header, header_len);
	memcpy(dci_req_buf + header_len, event_id_array, data_len);
	err = diag_send_data(dci_req_buf, header_len + data_len);
	if (err != DIAG_DCI_NO_ERROR) {
		DIAG_LOGE(" diag: error sending log stream config\n");
		return DIAG_DCI_SEND_DATA_FAIL;
	}

	for (i = 0; i < num_id; i++) {
		struct dci_event_mask *mask = &dci_client_tbl[proc].requested_events;
		int event_id = event_id_array[i];

		dci_cache_event(mask, event_id, set_mask);
	}

	return DIAG_DCI_NO_ERROR;
}

int diag_get_health_stats(struct diag_dci_health_stats *dci_health)
{
	return diag_get_health_stats_proc(dci_client_tbl[DIAG_PROC_MSM].handle, dci_health, DIAG_ALL_PROC);
}

int diag_get_health_stats_proc(int handle, struct diag_dci_health_stats *dci_health, int proc)
{
	int err = DIAG_DCI_NO_ERROR, c_proc;
	struct diag_dci_health_stats_proc health_proc;

	c_proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(c_proc))
		return DIAG_DCI_NOT_SUPPORTED;
	if (proc < DIAG_ALL_PROC || proc > DIAG_APPS_PROC)
		return DIAG_DCI_PARAM_FAIL;
	if (!dci_health)
		return DIAG_DCI_NO_MEM;

	health_proc.client_id = dci_client_tbl[c_proc].client_info.client_id;
	health_proc.proc = proc;
	health_proc.health.reset_status = dci_health->reset_status;

	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_DCI_HEALTH_STATS, &health_proc, sizeof(health_proc));
	if (err == DIAG_DCI_NO_ERROR) {
		dci_health->dropped_logs = health_proc.health.dropped_logs;
		dci_health->dropped_events = health_proc.health.dropped_events;
		dci_health->received_logs = health_proc.health.received_logs;
		dci_health->received_events = health_proc.health.received_events;
	}

	return err;
}

int diag_get_log_status(int handle, uint16 log_code, boolean *value)
{
	int err = DIAG_DCI_NO_ERROR, proc;
	struct diag_log_event_stats stats;

	proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;
	if (!value)
		return DIAG_DCI_NO_MEM;

	stats.client_id = dci_client_tbl[proc].client_info.client_id;
	stats.code = log_code;
	stats.is_set = 0;
	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_DCI_LOG_STATUS, &stats, sizeof(stats));
	if (err != DIAG_DCI_NO_ERROR)
		return DIAG_DCI_SEND_DATA_FAIL;
	else
		*value = (stats.is_set == 1) ? TRUE : FALSE;

	return DIAG_DCI_NO_ERROR;
}

int diag_get_event_status(int handle, uint16 event_id, boolean *value)
{
	int err = DIAG_DCI_NO_ERROR, proc;
	struct diag_log_event_stats stats;

	proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;
	if (!value)
		return DIAG_DCI_NO_MEM;

	stats.client_id = dci_client_tbl[proc].client_info.client_id;
	stats.code = event_id;
	stats.is_set = 0;
	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_DCI_EVENT_STATUS, &stats, sizeof(stats));
	if (err != DIAG_DCI_NO_ERROR)
		return DIAG_DCI_SEND_DATA_FAIL;
	else
		*value = (stats.is_set == 1) ? TRUE : FALSE;

	return DIAG_DCI_NO_ERROR;
}

int diag_disable_all_logs(int handle)
{
	int ret = DIAG_DCI_NO_ERROR, proc;
	int client_id;

	proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;
	client_id = dci_client_tbl[proc].client_info.client_id;
	ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_DCI_CLEAR_LOGS, &client_id, sizeof(client_id));
	if (ret != DIAG_DCI_NO_ERROR) {
		DIAG_LOGE(" diag: error clearing all log masks, ret: %d, error: %d\n", ret, errno);
		return DIAG_DCI_SEND_DATA_FAIL;
	}
	return ret;
}

int diag_disable_all_events(int handle)
{
	int ret = DIAG_DCI_NO_ERROR, proc;
	int client_id;

	proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;

	client_id = dci_client_tbl[proc].client_info.client_id;
	ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_DCI_CLEAR_EVENTS, &client_id, sizeof(client_id));
	if (ret != DIAG_DCI_NO_ERROR) {
		DIAG_LOGE(" diag: error clearing all event masks, ret: %d, error: %d\n", ret, errno);
		return DIAG_DCI_SEND_DATA_FAIL;
	}
	return ret;
}

int diag_dci_vote_real_time(int handle, int real_time)
{
	int err = DIAG_DCI_NO_ERROR, proc;
	struct real_time_vote_t vote;

	proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;

	if (!(real_time == MODE_REALTIME || real_time == MODE_NONREALTIME)) {
		DIAG_LOGE("diag: invalid mode change request\n");
		return DIAG_DCI_PARAM_FAIL;
	}
	vote.client_id = dci_client_tbl[proc].client_info.client_id;
	vote.proc = DIAG_PROC_DCI;
	vote.real_time_vote = real_time;
	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_VOTE_REAL_TIME, &vote, sizeof(vote));
	if (err == -1) {
		DIAG_LOGE(" diag: error voting for real time switch, ret: %d, error: %d\n", err, errno);
		err = DIAG_DCI_SEND_DATA_FAIL;
	}
	return DIAG_DCI_NO_ERROR;
}

int diag_dci_get_real_time_status(int *real_time)
{
	return diag_dci_get_real_time_status_proc(DIAG_PROC_MSM, real_time);
}

int diag_dci_get_real_time_status_proc(int proc, int *real_time)
{
	int err = DIAG_DCI_NO_ERROR;
	struct real_time_query_t query;

	if (!real_time) {
		DIAG_LOGE("diag: invalid pointer in %s\n", __func__);
		return DIAG_DCI_PARAM_FAIL;
	}
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;
	query.proc = proc;
	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_GET_REAL_TIME, &query, sizeof(query));
	if (err != 0) {
		DIAG_LOGE(" diag: error in getting real time status, proc: %d, err: %d, error: %d\n", proc, err, errno);
		err = DIAG_DCI_SEND_DATA_FAIL;
	}
	*real_time = query.real_time;
	return DIAG_DCI_NO_ERROR;
}

int diag_register_dci_signal_data(int handle, int signal_type)
{
	int proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;

	if (signal_type <= DIAG_INVALID_SIGNAL)
		return DIAG_DCI_PARAM_FAIL;

	dci_client_tbl[proc].data_signal_flag = ENABLE;
	dci_client_tbl[proc].data_signal_type = signal_type;
	return DIAG_DCI_NO_ERROR;
}

int diag_deregister_dci_signal_data(int handle)
{
	int proc = diag_dci_get_proc(handle);
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_NOT_SUPPORTED;

	if (dci_client_tbl[proc].data_signal_type == DIAG_INVALID_SIGNAL)
		return DIAG_DCI_NO_REG;

	dci_client_tbl[proc].data_signal_flag = DISABLE;
	dci_client_tbl[proc].data_signal_type = DIAG_INVALID_SIGNAL;
	return DIAG_DCI_NO_ERROR;
}

void diag_send_to_output(FILE *op_file, const char *str, ...)
{
	char buffer[6144];
	va_list arglist;
	memset(&arglist, 0, sizeof(va_list));
	va_start(arglist, str);
	if (!op_file)
		return;
	vsnprintf(buffer, 6144, str, arglist);
	fprintf(op_file, "%s", buffer);
	va_end(arglist);
}

int diag_dci_drain_buffer(int proc, int peripheral)
{
	int ret;
	struct param {
		int proc;
		int peripheral;
	} drain_param;

	drain_param.proc = proc;
	drain_param.peripheral = peripheral;
	ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_DCI_DRAIN_IMMEDIATE, &drain_param, sizeof(drain_param));

	if (ret == DIAG_DCI_NO_ERROR) {
		DIAG_LOGE("diag: Successfully configured for drain proc %d periph: %d\n",
					proc, peripheral);
	} else {
		DIAG_LOGE("diag: Error in sending dci drain buf, err: %d\n", ret);
	}
	return  ret;
}

int diag_dci_configure_buffering_mode(int proc, int peripheral, int tx_mode, int low_wm_val, int high_wm_val)
{
	int ret;
	struct diag_dci_periph_buffering_tx_mode
	{
		int client_id;
		int proc;
		struct diag_periph_buffering_tx_mode tx_mode_params;
	}  params;

	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_PARAM_FAIL;

	switch (tx_mode) {
	case DIAG_STREAMING_MODE:
	case DIAG_CIRCULAR_BUFFERING_MODE:
	case DIAG_THRESHOLD_BUFFERING_MODE:
		break;
	default:
		DIAG_LOGE("diag: In %s, invalid tx mode requested %d\n", __func__, tx_mode);
		return -EINVAL;
	}
	if ((((high_wm_val > 100) || (low_wm_val  > 100)) || (low_wm_val  > high_wm_val)) ||
		((low_wm_val == high_wm_val) && ((low_wm_val != 0) && (high_wm_val != 0)))) {
		DIAG_LOGE("diag: In %s, invalid watermark values, low: %d, high: %d\n",
				__func__, low_wm_val, high_wm_val);
		return -EINVAL;
	}

	params.client_id = dci_client_tbl[proc].client_info.client_id;
	params.proc = proc;
	params.tx_mode_params.peripheral = peripheral;
	params.tx_mode_params.mode = tx_mode;
	params.tx_mode_params.low_wm_val = low_wm_val;
	params.tx_mode_params.high_wm_val = high_wm_val;

	ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_VOTE_DCI_BUFFERING_MODE, &params, sizeof(struct diag_dci_periph_buffering_tx_mode));
	if (ret == DIAG_DCI_NO_ERROR) {
		DIAG_LOGE("diag: Successfully voted for buffering mode:%d proc %d periph: %d\n",
				tx_mode, proc, peripheral);
	} else {
		DIAG_LOGE("diag: Error in voting for a mode change, err: %d\n", ret);
	}
	dci_client_tbl[proc].dci_tx_mode_params.low_wm_val = low_wm_val;
	dci_client_tbl[proc].dci_tx_mode_params.mode = tx_mode;
	dci_client_tbl[proc].dci_tx_mode_params.high_wm_val = high_wm_val;

	return ret;

}

int diag_dci_register_for_buffering_mode_status(int proc, void(*func_ptr_dci_buffering_status)(int  data))
{
	if (!IS_VALID_DCI_PROC(proc))
		return DIAG_DCI_PARAM_FAIL;
	if (!func_ptr_dci_buffering_status)
		return DIAG_DCI_PARAM_FAIL;

	dci_client_tbl[proc].func_ptr_dci_buffering_status = func_ptr_dci_buffering_status;

	return DIAG_DCI_NO_ERROR;
}

