/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2017-2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Library definition for getting secure diag key info by sending diag command request/responses.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdbool.h>
#include "comdef.h"
#include "diagcmd.h"
#include "diag_lsm.h"
#include "diag_lsmi.h"
#include "errno.h"

#define DIAG_KEY_CMD_VERSION		1
#define DIAG_KEY_CMD_OPCODE_LIST	0
#define DIAG_KEY_CMD_OPCODE_RETRIEVE	1
#define DIAG_KEY_CMD_OP_PAYLOAD_ALL	0

static pthread_t ki_tid[NUM_MDLOG_FILE_TYPES];
static int ki_exit;
static int ki_enabled = 0;
static int ki_inited = 0;
static int query_key_status = 0;

PACK(struct) key_info_req {
	uint8_t  cmd_code;
	uint8_t  subsys_id;
	uint16_t subsys_cmd_code;
	uint8_t  cmd_version;
	uint8_t  op_code;
	uint16_t reserved;
	uint8_t  op_payload;
};

PACK(struct) key_info_resp {
	uint8_t cmd_code;
	uint8_t subsys_id;
	uint16_t subsys_cmd_code;
	uint8_t cmd_version;
	uint8_t op_code;
	uint8_t status;
	uint8_t reserved;
	uint8_t op_payload[];
};

/* keys is of size public_len + wrapped_len */
PACK(struct) key_info {
	uint8_t version;
	uint8_t classifier;
	uint8_t public_type;
	uint8_t reserved;
	uint16_t public_len;
	uint16_t wrapped_len;
	char keys[];
};

PACK(struct) key_info_resp_payload {
	uint8_t num_keys;
	struct key_info keys[];
};

struct diag_read_buf_pool {
	unsigned char* rsp_buf;
	size_t rsp_buf_len;
	int data_ready;
	pthread_mutex_t rsp_mutex;
	pthread_cond_t rsp_cond;
};
static struct diag_read_buf_pool ki_resp;
#define KEY_RESP_BUF_SIZE (1024 * 16)

PACK(struct) key_store {
	int valid;
	size_t size;
	void *key_info;
};

/* Number based on classifiers, increase as more peripherals add secure logging support */
#define MAX_KEY_STORE 4
static struct key_store keys[MAX_KEY_STORE];

pthread_mutex_t ki_tid_sync_mutex;
pthread_mutex_t ki_get_info_sync_mutex;

static void reset_keys(int proc);

void diag_notify_key_info_thread(uint32 status)
{
	DIAG_LOGE("%s: Signalling Key info thread for status: %x\n", __func__, status);
	if (status == DIAG_STATUS_OPEN) {
		reset_keys(MSM);
	} else if (status == DIAG_STATUS_CLOSED) {
		if (!ki_inited || ki_exit)
			return;
		pthread_mutex_lock(&ki_tid_sync_mutex);
		query_key_status = 1;
		pthread_mutex_unlock(&ki_tid_sync_mutex);
	}
}

static bool is_key_info_cmd(uint8_t *src)
{
	struct key_info_resp *resp;

	if (!src)
		return false;

	if (*src == DIAG_BAD_CMD_F || *src == DIAG_BAD_LEN_F || *src == DIAG_BAD_PARM_F)
		resp = (struct key_info_resp *)(src + 1);
	else
		resp = (struct key_info_resp *)src;

	if (resp->cmd_code == DIAG_SUBSYS_CMD_F &&
	    resp->subsys_id == DIAG_SUBSYS_DIAG_SERV &&
	    resp->subsys_cmd_code == DIAG_GET_KEY_INFO_MODEM)
		return true;

	return false;
}

static struct key_store *get_keystore(void)
{
	int i;

	for (i = 0; i < MAX_KEY_STORE; i++)
		if (!keys[i].valid)
			return &keys[i];

	return NULL;
}

static void reset_keystore(struct key_store *store)
{
	if (!store->valid)
		return;
	store->valid = 0;

	if (!store->key_info)
		return;
	free(store->key_info);
	store->key_info = NULL;
}

static void reset_keys(int proc)
{
	int i;

	for (i = 0; i < MAX_KEY_STORE; i++)
		reset_keystore(&keys[i]);
}

static int process_key_info_resp(int proc)
{
	struct key_info_resp_payload *payload;
	struct key_info_resp *resp;
	struct key_store *store;
	struct key_info *key;
	size_t key_size;
	uint8_t *ptr, *key_ptr;
	int i;

	ptr = ki_resp.rsp_buf;
	if (*ptr == DIAG_BAD_CMD_F || *ptr == DIAG_BAD_LEN_F || *ptr == DIAG_BAD_PARM_F) {
		DIAG_LOGE("diag: %s: got bad cmd response(%hhu)\n", __func__, *ptr);
		return -1;
	}

	resp = (struct key_info_resp *)ptr;
	if (resp->cmd_version != DIAG_KEY_CMD_VERSION || resp->op_code != DIAG_KEY_CMD_OPCODE_RETRIEVE) {
		DIAG_LOGE("diag: %s: bad cmd_version(%hhu) or op_code(%hhu)\n", __func__, resp->cmd_version, resp->op_code);
		return -1;
	}

	if (resp->status) {
		DIAG_LOGE("diag: %s: bad status(%hhu) returned from peripheral\n", __func__, resp->status);
		return -1;
	}

	/* Received valid response, reset keys for fresh response */
	reset_keys(proc);

	payload = resp->op_payload;
	key_ptr = &payload->keys[0];
	for (i = 0; i < payload->num_keys; i++) {
		key = key_ptr;
		key_size = sizeof(*key) + key->public_len + key->wrapped_len;

		store = get_keystore();
		if (!store) {
			DIAG_LOGE("diag: No space to store key information\n");
			return -1;
		}

		store->key_info = malloc(key_size);
		if (!store->key_info) {
			DIAG_LOGE("diag: failed to alloc key store buffer\n");
			return -1;
		}
		
		memcpy(store->key_info, key, key_size);
		store->size = key_size;
		store->valid = 1;
		key_ptr += key_size;
	}
	pthread_mutex_lock(&ki_tid_sync_mutex);
	query_key_status = 0;
	pthread_mutex_unlock(&ki_tid_sync_mutex);
	return 0;
}

int parse_data_for_key_info_resp(uint8 *ptr, int count_received_bytes, int proc)
{
	unsigned char *dest_ptr = NULL;
	unsigned int src_length = 0, dest_length = 0;
	unsigned int len = 0, i;
	uint8 src_byte;
	uint8 *src_ptr = NULL;
	uint16 payload_len = 0;
	int bytes_read = 0;

	if (ki_exit)
		return -1;

	while (bytes_read < count_received_bytes) {
		src_ptr = ptr + bytes_read;
		src_length = count_received_bytes - bytes_read;

		if (hdlc_disabled[proc]) {
			payload_len = *(uint16 *)(src_ptr + 2);
			if (is_key_info_cmd(src_ptr + 4)) {
				ki_enabled--;
				dest_ptr = ki_resp.rsp_buf;
				dest_length = ki_resp.rsp_buf_len;
				if (payload_len <= dest_length)
					memcpy(dest_ptr, src_ptr + 4, payload_len);
				else
					return -1;

				process_key_info_resp(proc);

				pthread_mutex_lock(&ki_resp.rsp_mutex);
				ki_resp.data_ready = 1;
				pthread_cond_signal(&ki_resp.rsp_cond);
				pthread_mutex_unlock(&ki_resp.rsp_mutex);
				bytes_read += payload_len + 5;
			} else {
				bytes_read += payload_len + 5;
			}
		} else {
			if (is_key_info_cmd(src_ptr)) {
				ki_enabled--;
				dest_ptr = ki_resp.rsp_buf;
				dest_length = ki_resp.rsp_buf_len;
				for (i = 0; i < src_length; i++) {
					src_byte = src_ptr[i];
					if (src_byte == ESC_CHAR) {
						if (i == (src_length - 1)) {
							i++;
							break;
						}
						dest_ptr[len++] = src_ptr[++i] ^ 0x20;
					} else if (src_byte == CTRL_CHAR) {
						if (i == 0 && src_length > 1)
							continue;
						dest_ptr[len++] = src_byte;
						i++;
						break;
					} else {
						dest_ptr[len++] = src_byte;
					}

					if (len >= dest_length) {
						DIAG_LOGE("diag: %s: truncating resp because buf is not big enough len:%d recvd:%d",
							  __func__, len, count_received_bytes);
						i++;
						break;
					}
				}
				bytes_read += i;
				len = 0;

				process_key_info_resp(proc);

				pthread_mutex_lock(&ki_resp.rsp_mutex);
				ki_resp.data_ready = 1;
				pthread_cond_signal(&ki_resp.rsp_cond);
				pthread_mutex_unlock(&ki_resp.rsp_mutex);
			} else {
				for (i = 0; i < src_length; i++) {
					if (src_ptr[i] == CTRL_CHAR) {
						i++;
						break;
					}
				}
				bytes_read += i;
			}
		}
	}
	return 0;
}

static int send_key_info_req(int proc)
{
	uint8_t buf[DIAG_CMD_REQ_BUF_SIZE] = { };
	struct key_info_req *req;
	uint8_t *ptr;
	int len;
	int ret;

	if (proc) {
		DIAG_LOGE("diag: key_info for external socs is not supported yet\n");
		return 0;
	}

	ptr = buf;
	len = 0;

	/* Set data type for diag router to parse */
	*(uint32_t *)ptr = USER_SPACE_RAW_DATA_TYPE;
	ptr += sizeof(uint32_t);
	len += sizeof(uint32_t);

	req = (struct key_info_req *)ptr;
	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_DIAG_SERV;
	req->subsys_cmd_code = DIAG_GET_KEY_INFO_MODEM;
	req->cmd_version = DIAG_KEY_CMD_VERSION;
	req->op_code = DIAG_KEY_CMD_OPCODE_RETRIEVE;
	req->op_payload = DIAG_KEY_CMD_OP_PAYLOAD_ALL;
	len += sizeof(*req);

	ret = diag_send_data(buf, len);
	if (ret)
		DIAG_LOGE("diag: failed to send key_info req %d\n", ret);

	return ret;
}

static int wait_for_key_info_resp(void)
{
	struct timespec time;
	struct timeval now;
	int rc = 0;

	gettimeofday(&now, NULL);
	time.tv_sec = now.tv_sec + 10;
	time.tv_nsec = now.tv_usec * 1000;
	pthread_mutex_lock(&ki_resp.rsp_mutex);
	while (!ki_resp.data_ready && rc != ETIMEDOUT && !ki_exit)
		rc = pthread_cond_timedwait(&ki_resp.rsp_cond, &ki_resp.rsp_mutex, &time);

	ki_resp.data_ready = 0;
	pthread_mutex_unlock(&ki_resp.rsp_mutex);
	return rc;
}

static void *query_key_info(void *arg)
{
	int proc = (int)arg;
	int ret;

	if (ki_exit)
		return arg;

	if (proc == MSM) {
		ki_enabled++;
		ki_resp.data_ready = 0;

		send_key_info_req(proc);
		ret = wait_for_key_info_resp();
		if (ret == ETIMEDOUT || ki_exit) {
			DIAG_LOGE("diag: %s: wait for resp failed:%d\n", __func__, ret);
			ki_enabled--;
		}
	}

	return arg;
}

int keys_stored(int proc)
{
	int count = 0;
	int i;

	for (i = 0; i < MAX_KEY_STORE; i++)
		if (keys[i].valid)
			count++;

	return count;
}

int get_keys_header_size(int proc)
{
	int count = 0;
	int i;

	/* add one byte to account for num_keys field */
	count += sizeof(uint8_t);
	for (i = 0; i < MAX_KEY_STORE; i++)
		if (keys[i].valid)
			count += keys[i].size;
	return count;
}

int write_key_header(int fd, int proc)
{
	uint8_t num_keys;
	int count = 0;
	int ret;
	int i;

	num_keys = (uint8_t)keys_stored(proc);
	ret = write(fd, &num_keys, sizeof(num_keys));
	if (ret <= 0) {
		DIAG_LOGE("diag: %s: failed to write num_keys:%d\n", __func__, ret);
		return -1;
	}
	count += ret;

	for (i = 0; i < MAX_KEY_STORE; i++) {
		if (!keys[i].valid)
			continue;

		ret = write(fd, keys[i].key_info, keys[i].size);
		if (ret <= 0) {
			DIAG_LOGE("diag: %s: failed to write key index:%d (%d)\n", __func__, i, ret);
			return -1;
		}
		count += ret;
	}

	return count;
}

int key_info_enabled(void)
{
	return ki_enabled;
}

void diag_get_secure_diag_info(int proc, int file_type)
{
	int ret;

	if (ki_exit)
		return;

	pthread_mutex_lock(&ki_get_info_sync_mutex);
	pthread_mutex_lock(&ki_tid_sync_mutex);
	if (!ki_inited || !query_key_status) {
		DIAG_LOGE("Skip querying key info, ki_inited: %d, ki_exit: %d, query_key_status: %d\n",
			ki_inited, ki_exit, query_key_status);
		pthread_mutex_unlock(&ki_tid_sync_mutex);
		pthread_mutex_unlock(&ki_get_info_sync_mutex);
		return;
	}
	pthread_mutex_unlock(&ki_tid_sync_mutex);
	ret = pthread_create(&ki_tid[file_type], NULL, query_key_info, (void *)(intptr_t)proc);
	if (ret)
		DIAG_LOGE("%s: Failed to create secure diag thread\n", __func__);

	pthread_join(ki_tid[file_type], NULL);
	ki_tid[file_type] = 0;
	pthread_mutex_unlock(&ki_get_info_sync_mutex);
}

int diag_key_info_init()
{
	pthread_mutex_init(&ki_resp.rsp_mutex, NULL);
	pthread_mutex_init(&ki_tid_sync_mutex, NULL);
	pthread_mutex_init(&ki_get_info_sync_mutex, NULL);
	pthread_cond_init(&ki_resp.rsp_cond, NULL);

	ki_resp.data_ready = 0;
	ki_resp.rsp_buf = malloc(KEY_RESP_BUF_SIZE);
	if (!ki_resp.rsp_buf) {
		DIAG_LOGE("%s: failed to create resp buffer\n", __func__);
		return -1;
	}
	ki_resp.rsp_buf_len = KEY_RESP_BUF_SIZE;

	ki_exit = 0;
	ki_enabled = 0;
	ki_inited = 1;
	query_key_status = 1;
	for (int i = FILE_TYPE_QMDL2; i < NUM_MDLOG_FILE_TYPES; i++)
		ki_tid[i] = 0;
	memset(keys, 0, sizeof(keys));

	return 0;
}

void diag_kill_key_info_threads(void)
{
	int i;

	ki_exit = 1;
	DIAG_LOGE("diag: %s: enter enabled:%d data_ready:%d\n",
		  __func__, ki_enabled, ki_resp.data_ready);

	pthread_cond_broadcast(&ki_resp.rsp_cond);
	sleep(1);

	pthread_cond_destroy(&ki_resp.rsp_cond);
	pthread_mutex_destroy(&ki_resp.rsp_mutex);
	pthread_mutex_destroy(&ki_tid_sync_mutex);
	pthread_mutex_destroy(&ki_get_info_sync_mutex);

	if (ki_resp.rsp_buf)
		free(ki_resp.rsp_buf);

	for (i = 0; i < NUM_PROC; i++)
		reset_keys(i);

	DIAG_LOGE("diag: %s: finished freeing key info resources\n", __func__);
}
