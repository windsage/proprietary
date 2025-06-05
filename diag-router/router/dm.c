/*
 * Copyright (c) 2016-2018, The Linux Foundation. All rights reserved.
 * Copyright (c) 2016-2018, Linaro Ltd.
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
#include <unistd.h>
#include <stdio.h>
#include "diag.h"
#include "dm.h"
#include "watch.h"
#include "diag_cntl.h"
#include "masks.h"
#include "diag_mux.h"
#include "diag_mhi.h"
#include "diag_dci.h"
#include "mbuf.h"
#include "sys_time.h"

#define DATA_TYPE_CMD_REG				5
#define DATA_TYPE_CMD_DEREG				6
#define DATA_TYPE_SWITCH_LOGGING			7
#define DATA_TYPE_GET_DELAYED_RSP_ID			8
#define DATA_TYPE_LSM_DEINIT				9
#define DATA_TYPE_DCI_INIT				20
#define DATA_TYPE_DCI_DEINIT				21
#define DATA_TYPE_DCI_SUPPORT				22
#define DATA_TYPE_DCI_REG				23
#define DATA_TYPE_DCI_STREAM_INIT			24
#define DATA_TYPE_DCI_HEALTH_STATS			25
#define DATA_TYPE_DCI_LOG_STATUS			26
#define DATA_TYPE_DCI_EVENT_STATUS			27
#define DATA_TYPE_DCI_CLEAR_LOGS			28
#define DATA_TYPE_DCI_CLEAR_EVENTS			29
#define DATA_TYPE_USER_CMD				50
#define DATA_TYPE_QUERY_DEVICE_MASK			51
#define DATA_TYPE_VOTE_REALTIME 			33
#define DATA_TYPE_GET_REALTIME 				34
#define DATA_TYPE_PERIPHERAL_BUF_CONFIG			35
#define DATA_TYPE_PERIPHERAL_BUF_DRAIN			36
#define DATA_TYPE_REGISTER_CALLBACK			37
#define DATA_TYPE_HDLC_TOGGLE				38
#define DATA_TYPE_QUERY_CON_ALL 			40
#define DATA_TYPE_QUERY_MD_PID 				41
#define DATA_TYPE_QUERY_PD_FEATUREMASK			42
#define DATA_TYPE_PASSTHRU_CONTROL 			43
#define DATA_TYPE_MDM_HDLC_TOGGLE			44
#define DATA_TYPE_SET_OVERRIDE_PID			47
#define DATA_TYPE_UPDATE_QDSS_ETR1_SUPPORT		48
#define DATA_TYPE_PERIPHERAL_DCI_BUF_CONFIG		49
#define DATA_TYPE_DCI_PERIPHERAL_BUF_DRAIN_IMMEDIATE	50

#define DATA_TYPE_QUERY_REG_TABLE			75
#define DATA_TYPE_QUERY_DEBUG_ALL			76
#define DATA_TYPE_UPDATE_DYN_DBG_MASK			77
#define DATA_TYPE_GET_TRANSPORT_MODE			78

#define USER_SPACE_DATA_TYPE		0x00000020
#define DATA_TYPE_DCI			0x00000040
#define USER_SPACE_RAW_DATA_TYPE	0x00000080
#define PASSTHRU_MASK_TYPE 		0x00002000
#define DELAYED_RSP_MASK_TYPE 		0x00004000
#define DEVICE_MASK_TYPE 		0x00010000
#define DCI_REG_CLIENT_ID_MASK 		0x00020000
#define DCI_SUPPORT_MASK 		0x00040000
#define DCI_HEALTH_STATS_MASK 		0x00080000
#define DCI_LOG_STATUS_MASK		0x00100000
#define DCI_EVENT_STATUS_MASK 		0x00200000
#define GET_REAL_TIME_MASK 		0x00400000
#define DIAG_SWITCH_LOGGING_MASK	0x00160000
#define DIAG_TIMESTAMP_SWITCH  		0x00320000
#define DIAG_ID_TYPE			0x00640000
#define REG_TABLE_MASK			0x10000000
#define DEBUG_ALL_MASK			0x20000000
#define DIAG_SET_DYNAMIC_DEBUG_MASK	0x40000000
#define DIAG_END_OF_PACKET		0xABABABAB

#define FD_MAX_NUM_READ			10
#define DIAG_DM_DBG_MASK_PKT_SIZE	8
#define UINT64_LIMIT			(UINT64_C(18446744073709551615))
/**
 * DOC: Diagnostic Monitor
 */

struct list_head diag_clients = LIST_INIT(diag_clients);
struct list_head diag_sock_clients = LIST_INIT(diag_sock_clients);
static bool clear_masks = true;
extern int socket_counter;
unsigned int etr1_support;

/*
 * dm_add() - register new DM
 * @dm:		DM object to register
 */
struct diag_client *dm_add(const char *name, int in_fd, int out_fd, bool hdlc_encoded)
{
	struct diag_client *dm = NULL;
	int unix_fd = 0;
#ifdef FEATURE_LE_DIAG
	int i, j;
#endif

	if (!name)
		return NULL;

	dm = calloc(1, sizeof(*dm));
	if (!dm)
		err(1, "failed to allocate DM context\n");

	dm->name = strdup(name);
	if (!dm->name)
		goto err_fail;

	dm->in_fd = in_fd;
	dm->out_fd = out_fd;
	dm->hdlc_encoded = hdlc_encoded;
	dm->flow = NULL;
	list_init(&dm->outq);
	list_init(&dm->cmd_rsp_q);

	if (dm->in_fd >= 0) {
		if (!strcmp(dm->name, "UNIX")) {
			unix_fd = UNIX_CLIENT_FD;
			dm->data_buf = malloc(USER_SPACE_HDLC_BUF);
			if (!dm->data_buf) {
				ALOGE("diag: %s: data buf allocation failure\n", __func__);
				goto data_buf_fail;
			}
			memset(dm->data_buf, 0, USER_SPACE_HDLC_BUF);
			dm->data_bytes_read = 0;
			dm->rsp_buf = malloc(USER_SPACE_HDLC_BUF);
			if (!dm->rsp_buf) {
                                ALOGE("diag: %s: rsp buf allocation failure\n", __func__);
                                goto rsp_buf_fail;
                        }
                        memset(dm->rsp_buf, 0, USER_SPACE_HDLC_BUF);

			dm->hdlc_read_buf = malloc(USER_SPACE_HDLC_BUF);
			if (!dm->hdlc_read_buf) {
				ALOGE("diag: %s: hdlc read buf allocation failure\n", __func__);
				goto hdlc_buf_fail;
			}
			memset(dm->hdlc_read_buf, 0, USER_SPACE_HDLC_BUF);
			dm->hdlc_enc_done = 0;
			dm->use_mbuf = 0;

#ifdef FEATURE_LE_DIAG
			for (i = 0; i < UNIX_CLIENT_BUF_COUNT; i++) {
				dm->dm_mbuf[i] = mbuf_alloc(USER_SPACE_HDLC_BUF - sizeof(struct mbuf*));
				if (!dm->dm_mbuf[i]) {
					ALOGE("diag: %s: dm_mbuf[%d] allocation failure\n", i, __func__);
					for (j = i-1; j >= 0; j--) {
						if (dm->dm_mbuf[j]) {
							free(dm->dm_mbuf[j]);
							dm->dm_mbuf[j] = NULL;
							goto mbuf_fail;
						}
					}
				}
				dm->dm_mbuf[i]->free_flag = 1;
				dm->dm_mbuf[i]->busy_state= 0;
				list_init(&dm->dm_mbuf[i]->node);
			}
			dm->use_mbuf = 1;
#endif
			dm->flow = watch_flow_new();
			watch_add_readfd(dm->in_fd, dm_recv, dm, dm->flow);
		} else if (!strcmp(dm->name, "PCIe")) {
			watch_add_readfd(dm->in_fd, pcie_dm_recv, dm, NULL);
		} else {
			watch_add_readfd(dm->in_fd, dm_recv, dm, NULL);
		}
	}
	watch_add_writeq(dm->out_fd, &dm->outq, &dm->cmd_rsp_q, unix_fd);
	if (!strcmp(dm->name,"UNIX"))
		list_add(&diag_sock_clients, &dm->node);
	else
		list_add(&diag_clients, &dm->node);

	/* Disable DM by default, so that data wont be sent out until usb enable event is received */
	dm->enabled = false;

	return dm;

#ifdef FEATURE_LE_DIAG
mbuf_fail:
	free(dm->hdlc_read_buf);
	dm->hdlc_read_buf = NULL;
#endif
hdlc_buf_fail:
	free(dm->rsp_buf);
	dm->rsp_buf = NULL;
rsp_buf_fail:
	free(dm->data_buf);
	dm->data_buf = NULL;
data_buf_fail:
	free((void*)dm->name);
	dm->name = NULL;
err_fail:
	free(dm);
	dm = NULL;
	return NULL;
}

int dm_del(int fd)
{
	struct diag_client *dm;
	struct list_head *item;
	struct list_head *temp;
	int i;

	list_for_each_safe_with_lock(item, temp, &diag_sock_clients) {
		dm = container_of(item, struct diag_client, node);
		if (dm->in_fd == fd) {
			if (dm->flow) {
				watch_flow_put(dm->flow);
				dm->flow = NULL;
			}
			list_del(&dm->node);
			if (dm->name) {
				free((void*)dm->name);
				dm->name = NULL;
			}
			for (i = 0; i < UNIX_CLIENT_BUF_COUNT; i++) {
				if (dm->dm_mbuf[i]) {
					list_del(&dm->dm_mbuf[i]->node);
					list_destroy(&dm->dm_mbuf[i]->node);
					free(dm->dm_mbuf[i]);
					dm->dm_mbuf[i] = NULL;
				}
			}
			if (dm->hdlc_read_buf) {
				free(dm->hdlc_read_buf);
				dm->hdlc_read_buf = NULL;
			}

			if (dm->data_buf) {
				free(dm->data_buf);
				dm->data_buf = NULL;
			}
			if (dm->rsp_buf) {
				free(dm->rsp_buf);
				dm->rsp_buf = NULL;
			}

			free(dm);
			dm = NULL;
			break;
		}
	}

	LIST_UNLOCK(&diag_sock_clients);/*lock is taken in list_for_each_safe_with_lock */

	return 0;

}

static int dm_recv_hdlc(struct diag_client *dm)
{
	size_t msglen;
	ssize_t n;
	void *msg;
	struct diag_pkt_frame_t *pkt_ptr;
	struct circ_buf circ_loc_buf = {0};

	n = circ_read(dm->in_fd, &circ_loc_buf);

	if (n < 0 && dm == pcie_dm)
		return 0;

	if (n < 0 && errno != EAGAIN) {
		warn("Failed to read from %s\n", dm->name);
		return -errno;
	}

	if (diagmem->hdlc_disabled) {
		pkt_ptr = (struct diag_pkt_frame_t *)&circ_loc_buf;
		msglen = pkt_ptr->length;
		if (pkt_ptr->start != CONTROL_CHAR || *(uint8_t *)(pkt_ptr->data+pkt_ptr->length) != CONTROL_CHAR)
			diag_start_hdlc_recovery(0, msglen);
		else
			diag_client_handle_command(dm, &pkt_ptr->data[0], msglen, 0);
	} else {
		for (;;) {
			msg = hdlc_decode_one(&dm->recv_decoder, &circ_loc_buf, &msglen);
			if (!msg)
				break;

			diag_client_handle_command(dm, msg, msglen, 0);
		}
	}

	return 0;
}

void diag_send_passthru_control_pkt(struct diag_client *dm, struct diag_hw_accel_cmd_req_t *req_params, int len)
{
	struct diag_hw_diag_id_mask
	{
		int data_type;
		int ret_val;
		uint32_t diagid_mask;
	}__packed;
	struct diag_hw_diag_id_mask params;
	int ret;
	(void)len;

	ret = diag_cntl_send_passthru_control_pkt(req_params);
	params.data_type = PASSTHRU_MASK_TYPE;
	params.ret_val = ret;
	params.diagid_mask = req_params->op_req.diagid_mask;
	dm_send(dm, &params, sizeof(params));

}

void diag_query_con_all(struct diag_client *dm, struct diag_con_all_param_t *req_params, int len)
{
	struct diag_query_con_all_pkt{
		int data_type;
		struct diag_con_all_param_t params;
	}__packed;
	struct diag_query_con_all_pkt pkt;
	(void)req_params;
	(void)len;

	pkt.data_type = 0x16000;
	pkt.params.diag_con_all = DIAG_CON_ALL;
	pkt.params.num_peripherals = NUM_PERIPHERALS;
	pkt.params.upd_map_supported = 1;
	dm_send(dm, &pkt, sizeof(pkt));
}

static void diag_query_session_pid(struct diag_query_pid_t *param)
{
	int prev_pid = 0, test_pid = 0, i = 0, count = 0;
	int local_proc = 0;
	int proc = 0;

	for (proc = 0; proc < NUM_DIAG_MD_DEV; proc++) {
		local_proc = 1<<proc;
		if (param->device_mask & (local_proc)) {
			count = 0;
			for (i = 0; i < NUM_PERIPHERALS; i++) {
				if (diagmem->md_session_map[proc][i]) {
					test_pid =
					diagmem->md_session_map[proc][i]->pid;
					count++;
					if (!prev_pid)
						prev_pid = test_pid;
					if (test_pid != prev_pid) {
						param->pid = -EINVAL;
					}
				}
			}
			if (i == count && prev_pid)
				param->pid = prev_pid;
		}
	}
}

void diag_query_diag_session_pid(struct diag_query_pid_t *param)
{
	int prev_pid = 0, test_pid = 0, i = 0, j = 0;
	unsigned int proc = DIAG_LOCAL_PROC;
	uint32_t pd_mask = 0, peripheral_mask = 0;
	struct diag_md_session_t *info = NULL;

	param->pid = 0;

	if (param->pd_mask && param->peripheral_mask) {
		param->pid = -EINVAL;
		return;
	} else if (param->peripheral_mask) {
		if (param->peripheral_mask == DIAG_CON_ALL) {
			diag_query_session_pid(param);
		} else {
			peripheral_mask = param->peripheral_mask;
			for (i = 0; i < NUM_PERIPHERALS; i++) {
				if (diagmem->md_session_map[proc][i] &&
					(peripheral_mask &
					MD_PERIPHERAL_MASK(i))) {
					info =
					diagmem->md_session_map[proc][i];
					if (peripheral_mask !=
					info->peripheral_mask[proc]) {
						param->pid = -EINVAL;
						return;
					}
					test_pid = info->pid;
					if (!prev_pid)
						prev_pid = test_pid;
					if (test_pid != prev_pid) {
						param->pid = -EINVAL;
						return;
					}
				}
			}
			param->pid = prev_pid;
		}
	} else if (param->pd_mask) {
		pd_mask = param->pd_mask;
		for (i = UPD_WLAN, j = DIAG_CON_UPD_WLAN;
					(i < NUM_MD_SESSIONS && j <= DIAG_CON_UPD_LAST); i++) {
			if (diagmem->md_session_map[proc][i] &&
					(pd_mask & j)) {
				info =
				diagmem->md_session_map[proc][i];
                                pd_mask = 1 << i;
				if (pd_mask !=
				info->peripheral_mask[proc]) {
					param->pid = -EINVAL;
					return;
				}
				test_pid = info->pid;
				if (!prev_pid)
					prev_pid = test_pid;
				if (test_pid != prev_pid) {
					param->pid = -EINVAL;
					return;
				}
			}
			j = j << 1;
		}
		param->pid = prev_pid;
	}

	if (param->pid > 0 && param->kill_op) {
		info = diag_md_session_get_pid(param->pid);
		if (info && info->pid == param->pid) {
			param->kill_count = ++info->kill_count;
		}
	}
}

void diag_query_md_pid(struct diag_client *dm, struct diag_query_pid_t *query_params, int len)
{
	struct diag_query_pid_pkt{
		int data_type;
		struct diag_query_pid_t params;
	}__packed;
	struct diag_query_pid_pkt pkt;
	(void)len;

	pkt.data_type = 0x32000;
	diag_query_diag_session_pid(query_params);
	memcpy(&pkt.params, query_params, sizeof(*query_params));
	dm_send(dm, &pkt, sizeof(pkt));
}

struct diag_md_session_t *diag_md_session_get_peripheral(int proc,
						uint8_t peripheral)
{
	if (peripheral >= NUM_MD_SESSIONS)
		return NULL;
	return diagmem->md_session_map[proc][peripheral];
}

int diag_hdlc_toggle(int pid, uint8_t hdlc_support)
{
	struct diag_md_session_t *info;
	int peripheral;
	int i;

	pthread_mutex_lock(&diagmem->hdlc_disable_mutex);
	info = diag_md_session_get_pid(pid);
	if (info) {
		info->hdlc_disabled = hdlc_support;
		diagmem->proc_hdlc_disabled[DIAG_LOCAL_PROC] = hdlc_support;
	} else {
		diagmem->hdlc_disabled = hdlc_support;
	}

	peripheral = diag_md_session_match_pid_peripheral(DIAG_LOCAL_PROC, pid, 0);
	for (i = 0; i < NUM_MD_SESSIONS; i++) {
		if (peripheral >= 0 && info) {
			if (peripheral & (1 << i))
				diagmem->p_hdlc_disabled[i] = info->hdlc_disabled;
			else if (!diag_md_session_get_peripheral(DIAG_LOCAL_PROC, i))
				diagmem->p_hdlc_disabled[i] = diagmem->hdlc_disabled;
		} else {
			if (!diag_md_session_get_peripheral(DIAG_LOCAL_PROC, i))
				diagmem->p_hdlc_disabled[i] = diagmem->hdlc_disabled;
		}
	}
	pthread_mutex_unlock(&diagmem->hdlc_disable_mutex);

	return 0;
}

int diag_mdm_hdlc_toggle(int pid, uint8_t hdlc_support)
{
	struct diag_md_session_t *info;
	int i;

	info = diag_md_session_get_pid(pid);
	if (info) {
		for (i = DIAG_LOCAL_PROC + 1; i < NUM_DIAG_MD_DEV; i++) {
			diagmem->proc_hdlc_disabled[i] = hdlc_support;
		}
	}

	return 0;
}

int diag_md_session_create(struct diag_client *dm, int mode, struct diag_logging_mode_param_t *param, int proc)
{
	int i;
	int err = 0;
	struct diag_md_session_t *new_session = NULL;
	(void)mode;

	/*
	 * If a session is running with a peripheral mask and a new session
	 * request comes in with same peripheral mask value then return
	 * invalid param
	 */
	if (diagmem->md_session_mode[proc] == DIAG_MD_PERIPHERAL &&
	    (diagmem->md_session_mask[proc] & param->peripheral_mask) != 0)
		return -EINVAL;

	pthread_mutex_lock(&diagmem->session_info_mutex);

	new_session = diag_md_session_get_pid(param->pid);
	if (!new_session) {
		new_session = malloc(sizeof(struct diag_md_session_t));
		if (!new_session) {
			pthread_mutex_unlock(&diagmem->session_info_mutex);
			return -ENOMEM;
		}
		memset(new_session, 0, sizeof(*new_session));
		new_session->peripheral_mask[proc] = 0;
		new_session->pid = param->pid;
		new_session->kill_count = 0;
		new_session->pd_mask = 0;
		new_session->log_mask = malloc(sizeof(struct diag_mask_info));
		if (!new_session->log_mask) {
			err = -ENOMEM;
			goto fail_peripheral;
		}
		new_session->event_mask = malloc(sizeof(struct diag_mask_info));
		if (!new_session->event_mask) {
			err = -ENOMEM;
			goto fail_peripheral;
		}
		new_session->msg_mask =malloc(sizeof(struct diag_mask_info));
		if (!new_session->msg_mask) {
			err = -ENOMEM;
			goto fail_peripheral;
		}

		err = diag_log_mask_copy(new_session->log_mask, &log_mask);
		if (err) {
			ALOGE("diag: failed to copy the log mask to new md session, err %d\n", err);
			goto fail_peripheral;
		}
		err = diag_event_mask_copy(new_session->event_mask,
								   &event_mask);
		if (err) {
			ALOGE("diag: failed to copy the event mask to new md session, err %d\n", err);
			goto fail_peripheral;
		}
		new_session->msg_mask_tbl_count = 0;
		err = diag_msg_mask_copy(new_session, new_session->msg_mask,
					&msg_mask);
		if (err) {
			ALOGE("diag: failed to copy the msg mask to new md session, err %d\n", err);
			goto fail_peripheral;
		}
		new_session->dm = dm;
	}
	for (i = 0; i < NUM_MD_SESSIONS; i++) {
		if ((MD_PERIPHERAL_MASK(i) & param->peripheral_mask) == 0)
			continue;
		if (diagmem->md_session_map[proc][i] != NULL) {
			ALOGE("diag: another md instance active for peripheral: %d\n", i);
			err = -EEXIST;
			goto fail_peripheral;
		}
		new_session->peripheral_mask[proc] |= MD_PERIPHERAL_MASK(i);
		if (param->pd_mask)
			new_session->pd_mask = param->pd_mask;
		diagmem->md_session_map[proc][i] = new_session;
		diagmem->md_session_mask[proc] |= MD_PERIPHERAL_MASK(i);
	}

	diagmem->md_session_mode[proc] = DIAG_MD_PERIPHERAL;
	pthread_mutex_unlock(&diagmem->session_info_mutex);
	ALOGD("diag: created session in peripheral mode\n");
	return 0;

fail_peripheral:
	diag_log_mask_free(new_session->log_mask);
	free(new_session->log_mask);
	new_session->log_mask = NULL;
	diag_event_mask_free(new_session->event_mask);
	free(new_session->event_mask);
	new_session->event_mask = NULL;
	diag_msg_mask_free(new_session->msg_mask,
		new_session);
	free(new_session->msg_mask);
	new_session->msg_mask = NULL;
	free(new_session);
	new_session = NULL;
	pthread_mutex_unlock(&diagmem->session_info_mutex);

	return err;
}

static void diag_md_session_close(int pid)
{
	int i;
	uint8_t found = 0;
	struct diag_md_session_t *session_info = NULL;
	int proc;

	pthread_mutex_lock(&diagmem->session_info_mutex);
	session_info = diag_md_session_get_pid(pid);
	if (!session_info) {
		pthread_mutex_unlock(&diagmem->session_info_mutex);
		return;
	}

	for (proc = 0; proc < NUM_DIAG_MD_DEV; proc++) {
		for (i = 0; i < NUM_MD_SESSIONS; i++) {
			if (diagmem->md_session_map[proc][i] != session_info)
				continue;
			diagmem->md_session_map[proc][i] = NULL;
			diagmem->md_session_mask[proc] &=
				~session_info->peripheral_mask[proc];
			pthread_mutex_lock(&diagmem->hdlc_disable_mutex);
			diagmem->p_hdlc_disabled[i] = diagmem->hdlc_disabled;
			pthread_mutex_unlock(&diagmem->hdlc_disable_mutex);
		}
	}
	diag_log_mask_free(session_info->log_mask);
	free(session_info->log_mask);
	session_info->log_mask = NULL;
	diag_msg_mask_free(session_info->msg_mask,
		session_info);
	free(session_info->msg_mask);
	session_info->msg_mask = NULL;
	diag_event_mask_free(session_info->event_mask);
	free(session_info->event_mask);
	session_info->event_mask = NULL;
	for (proc = 0; proc < NUM_DIAG_MD_DEV; proc++) {
		for (i = 0; i < NUM_MD_SESSIONS && !found; i++) {
			if (diagmem->md_session_map[proc][i] != NULL)
				found = 1;
		}
		diagmem->md_session_mode[proc] = (found) ? DIAG_MD_PERIPHERAL :
								DIAG_MD_NONE;
		found = 0;
	}
	free(session_info);
	session_info = NULL;
	pthread_mutex_unlock(&diagmem->session_info_mutex);
	ALOGI("diag: Cleared up session for pid: %d\n", pid);
}

struct diag_md_session_t *diag_md_session_get_pid(int pid)
{
	int i;
	int proc;

	if (pid <= 0)
		return NULL;

	for (proc = 0; proc < NUM_DIAG_MD_DEV; proc++) {
		for (i = 0; i < NUM_MD_SESSIONS; i++) {
			if (diagmem->md_session_map[proc][i] &&
				diagmem->md_session_map[proc][i]->pid == pid)
				return diagmem->md_session_map[proc][i];
		}
	}
	return NULL;
}

/*
 * diag_md_session_match_pid_peripheral
 *
 *	1. Pass valid PID and get all the peripherals in logging session
 *		for that PID
 *	2. Pass valid Peipheral and get the pid logging for that peripheral
 *
 */

int diag_md_session_match_pid_peripheral(int proc, int pid,
	uint8_t peripheral)
{
	int i, flag = 0;

	if (pid <= 0 || peripheral >= NUM_MD_SESSIONS)
		return -EINVAL;

	if (!peripheral) {
		for (proc = 0; proc < NUM_DIAG_MD_DEV; proc++) {
			for (i = 0; i < NUM_MD_SESSIONS; i++) {
				if (diagmem->md_session_map[proc][i] &&
					diagmem->md_session_map[proc][i]->pid ==
								pid) {
					peripheral |= MD_PERIPHERAL_MASK(i);
					flag = 1;
				}
			}
			if (flag)
				return peripheral;
		}
	}

	if (!pid) {
		if (diagmem->md_session_map[proc][peripheral])
			return diagmem->md_session_map[proc][peripheral]->pid;
	}

	return -EINVAL;
}

static int diag_md_peripheral_switch(int proc, int pid,
				int peripheral_mask, int req_mode)
{
	int i, bit = 0;
	struct diag_md_session_t *session_info = NULL;

	session_info = diag_md_session_get_pid(pid);
	if (!session_info)
		return -EINVAL;
	if (req_mode != DIAG_USB_MODE && req_mode != DIAG_MEMORY_DEVICE_MODE &&
		req_mode != DIAG_PCIE_MODE)
		return -EINVAL;

	/*
	 * check that md_session_map for i == session_info,
	 * if not then race condition occurred and bail
	 */
	for (i = 0; i < NUM_MD_SESSIONS; i++) {
		bit = MD_PERIPHERAL_MASK(i) & peripheral_mask;
		if (!bit)
			continue;
		if (req_mode == DIAG_USB_MODE || req_mode == DIAG_PCIE_MODE) {
			if (diagmem->md_session_map[proc][i] != session_info)
				return -EINVAL;
			diagmem->md_session_map[proc][i] = NULL;
			diagmem->md_session_mask[proc] &= ~bit;
			session_info->peripheral_mask[proc] &= ~bit;

		} else {
			if (diagmem->md_session_map[proc][i] != NULL)
				return -EINVAL;
			diagmem->md_session_map[proc][i] = session_info;
			diagmem->md_session_mask[proc] |= bit;
			session_info->peripheral_mask[proc] |= bit;

		}
	}

	diagmem->md_session_mode[proc] = DIAG_MD_PERIPHERAL;
	ALOGI("diag: %s: Switched logging for p_mask: 0x%x to mode: %d\n",
		__func__, peripheral_mask, req_mode);
	return 0;
}

static int diag_md_session_check(struct diag_client *dm, int proc, int curr_mode, int req_mode,
				 const struct diag_logging_mode_param_t *param,
				 uint8_t *change_mode)
{
	int i, j, bit = 0, err = 0, peripheral_mask = 0, pd_val = -1;
	int change_mask = 0;
	struct diag_md_session_t *session_info = NULL;
	struct peripheral *perif = NULL;

	if (!param || !change_mode)
		return -EIO;

	*change_mode = 0;

	switch (curr_mode) {
	case DIAG_USB_MODE:
	case DIAG_MEMORY_DEVICE_MODE:
	case DIAG_MULTI_MODE:
	case DIAG_PCIE_MODE:
		break;
	default:
		ALOGE("diag: invalid curr mode: %d\n", curr_mode);
		return -EINVAL;
	}

	if (req_mode != DIAG_USB_MODE && req_mode != DIAG_MEMORY_DEVICE_MODE &&
		req_mode != DIAG_PCIE_MODE)
		return -EINVAL;

	if (curr_mode == req_mode)
		return 0;

	if ((req_mode ==  DIAG_USB_MODE && curr_mode == DIAG_PCIE_MODE) ||
		(req_mode == DIAG_PCIE_MODE && curr_mode == DIAG_USB_MODE)) {
		*change_mode = 1;
		return 0;
	} else if ((req_mode == DIAG_USB_MODE || req_mode == DIAG_PCIE_MODE)
		&& (curr_mode == DIAG_MEMORY_DEVICE_MODE ||
			curr_mode == DIAG_MULTI_MODE)) {
		if (diagmem->md_session_mode[proc] == DIAG_MD_NONE
		    && diagmem->md_session_mask[proc] == 0 &&
			diagmem->logging_mask[proc]) {
			*change_mode = 1;
			return 0;
		}
		/*
		 * curr_mode is either DIAG_MULTI_MODE or DIAG_MD_MODE
		 * Check if requested peripherals are already in usb mode
		 */
		for (i = 0; i < NUM_MD_SESSIONS; i++) {
			bit = MD_PERIPHERAL_MASK(i) & param->peripheral_mask;
			if (!bit)
				continue;
			if (bit & diagmem->logging_mask[proc])
				change_mask |= bit;
		}
		if (!change_mask) {
			return 0;
		}

		/*
		 * Change is needed. Check if this md_session has set all the
		 * requested peripherals. If another md session set a requested
		 * peripheral then we cannot switch that peripheral to USB.
		 * If this session owns all the requested peripherals, then
		 * call function to switch the modes/masks for the md_session
		 */
		session_info = diag_md_session_get_pid(param->pid);
		if (!session_info) {
			*change_mode = 1;
			return 0;
		}
		peripheral_mask = session_info->peripheral_mask[proc];
		if ((change_mask & peripheral_mask)
							!= change_mask) {
			ALOGE("diag: Another MD Session owns a requested peripheral\n");
			return -EINVAL;
		}
		*change_mode = 1;

		/* If all peripherals are being set to USB Mode, call close */
		if (~change_mask & peripheral_mask) {
			err = diag_md_peripheral_switch(proc, param->pid,
					change_mask, DIAG_USB_MODE);
		} else
			diag_md_session_close(param->pid);

		return err;

	} else if (req_mode == DIAG_MEMORY_DEVICE_MODE) {
		/*
		 * Get bit mask that represents what peripherals already have
		 * been set. Check that requested peripherals already set are
		 * owned by this md session
		 */
		change_mask = diagmem->md_session_mask[proc] &
				param->peripheral_mask;
		session_info = diag_md_session_get_pid(param->pid);

		if (session_info && diagmem->md_session_mode[proc] !=
							DIAG_MD_NONE) {
			if ((session_info->peripheral_mask[proc] & change_mask)
							!= change_mask) {
				ALOGE("diag: Another MD Session owns a requested peripheral\n");
				return -EINVAL;
			}
			err = diag_md_peripheral_switch(proc, param->pid,
					change_mask, req_mode);
		} else {
			if (change_mask) {
				ALOGE("diag: Another MD Session owns a requested peripheral\n");
				return -EINVAL;
			}
			err = diag_md_session_create(dm, DIAG_MD_PERIPHERAL,
				param, proc);

			for (i = 0; i < NUM_MD_SESSIONS; i++) {
				if (param->peripheral_mask == DIAG_CON_ALL) {
					diagmem->p_hdlc_disabled[i] = 0;
				} else if (param->peripheral_mask & MD_PERIPHERAL_MASK(i)) {
					diagmem->p_hdlc_disabled[i] = 0;
					perif = diag_get_periph_info(i);
					if (perif) {
						for (j = 0; j < MAX_PERIPHERAL_UPD; j++) {
							pd_val = perif->upd_info[j].pd_val;
							if (pd_val > 0 && pd_val < NUM_MD_SESSIONS) {
								diagmem->p_hdlc_disabled[pd_val] = 0;
							}
						}
					}
				}
			}
		}
		*change_mode = 1;
		return err;
	}
	return -EINVAL;
}

static int diag_switch_logging_proc(struct diag_client *dm, struct diag_logging_mode_param_t *param,
					int new_mode, uint32_t peripheral_mask)
{
	int proc = 0, local_proc = 0, err = 0, curr_mode;
	uint8_t do_switch = 1;

	for (proc = 0; proc < NUM_DIAG_MD_DEV; proc++) {
		local_proc = 1 << proc;
		if (param->device_mask & (local_proc)) {
			curr_mode = diagmem->logging_mode[proc];
			ALOGI("diag: %s: Switch logging from (curr_mode: %d mask: %0x) to (new_mode: %d p_mask: %0x) on proc: %d\n",
				__func__, curr_mode, diagmem->md_session_mask[proc],
				new_mode, peripheral_mask, proc);

			err = diag_md_session_check(dm, proc, curr_mode, new_mode,
							param, &do_switch);
			if (err) {
				ALOGE("diag: %s: Failure in diag_md_session_check with err: %d\n",
					__func__, err);
				return err;
			}

			if (do_switch == 0) {
				ALOGE("diag: %s: Not switching modes from curr_mode: %d new_mode: %d on proc: %d\n",
					__func__, curr_mode, new_mode, proc);
				continue;
			}

			err = diag_mux_switch_logging(dm, proc, &new_mode,
					&peripheral_mask);
			if (err) {
				ALOGE("diag: %s: Unable to switch mode from %d to %d, err: %d on proc: %d\n",
					__func__, curr_mode, new_mode, err, proc);
				diagmem->logging_mode[proc] = curr_mode;
				return err;
			}
			diagmem->logging_mode[proc] = new_mode;
			diagmem->logging_mask[proc] = peripheral_mask;
			ALOGI("diag: %s: Logging switched to new_mode: %d for p_mask: %0x on proc: %d\n",
				__func__, new_mode, peripheral_mask, proc);

			/* Update to take peripheral_mask */
			/* Update realtime vote to MODE_REALTIME if switching to mode
			 * other than MEMORY_DEVICE_MODE or MULTI_MODE. */
			if (new_mode != DIAG_MEMORY_DEVICE_MODE &&
				new_mode != DIAG_MULTI_MODE) {
				diag_update_real_time_vote(
						proc,
						MODE_REALTIME,
						ALL_PROC);
			} else {
				diag_update_proc_vote(DIAG_PROC_MEMORY_DEVICE,
							VOTE_UP,
							ALL_PROC);
			}
			if(new_mode == DIAG_USB_MODE &&
				curr_mode == DIAG_MEMORY_DEVICE_MODE) {
				diag_update_proc_vote(DIAG_PROC_MEMORY_DEVICE,
							VOTE_DOWN,
							ALL_PROC);
			}
			diag_send_real_time();
		}
		peripheral_mask = param->peripheral_mask;
	}
	return err;
}

int diag_switch_logging(struct diag_client *dm, struct diag_logging_mode_param_t *param, int len)
{

		int new_mode, i = 0, j = 0;
		int err = 0, peripheral = 0;
		uint32_t peripheral_mask = 0, pd_mask = 0;
		(void)len;

		if (!param)
			return -EINVAL;

		if (!param->peripheral_mask) {
			ALOGE("diag: asking for mode switch with no peripheral mask set\n");
			return -EINVAL;
		}
		if (!param->device_mask) {
			ALOGE("diag: asking for mode switch with no device mask set\n");
			return -EINVAL;
		}

		if (param->pd_mask) {
			pd_mask = param->pd_mask;
			param->diag_id = 0;
			param->pd_val = 0;
			param->peripheral = -EINVAL;

			for (i = UPD_WLAN, j = DIAG_CON_UPD_WLAN;
					(i < NUM_MD_SESSIONS && j <= DIAG_CON_UPD_LAST); i++) {
				if (pd_mask & j) {
					if (diag_query_diag_id_from_pd(i, &param->diag_id,
						&param->peripheral)) {
						param->pd_val = i;
						break;
					}
				}
				j = j << 1;
			}

			ALOGI("diag: %s: pd_mask = %d, diag_id = %d, peripheral = %d, pd_val = %d\n",
				__func__, param->pd_mask, param->diag_id,
				param->peripheral, param->pd_val);

			if (!param->diag_id ||
				(param->pd_val < UPD_WLAN) ||
				(param->pd_val >= NUM_MD_SESSIONS)) {
				ALOGE("diag: diag_id support is not present for the pd mask = %d\n",
				param->pd_mask);
				return -EINVAL;
			}

			peripheral = param->peripheral;
			if ((peripheral < PERIPHERAL_APPS) ||
				(peripheral >= NUM_PERIPHERALS)) {
				ALOGE("diag: Invalid peripheral: %d\n", peripheral);
				return -EINVAL;
			}
			i = param->pd_val - UPD_WLAN;
			if (diagmem->md_session_map[DIAG_LOCAL_PROC][peripheral] &&
				(MD_PERIPHERAL_MASK(peripheral) &
				diag_mux->mux_mask[DIAG_LOCAL_PROC]) &&
				!diagmem->pd_session_clear[i]) {
				ALOGE("diag: User PD is already logging onto active peripheral logging\n");
				diagmem->pd_session_clear[i] = 0;
				return -EINVAL;
			}
			param->peripheral_mask = 1<<param->pd_val;
			if (!diagmem->pd_session_clear[i]) {
				diagmem->pd_logging_mode[i] = 1;
				diagmem->num_pd_session += 1;
			}
			diagmem->pd_session_clear[i] = 0;
		}
		peripheral_mask = param->peripheral_mask;
		switch (param->req_mode) {
		case CALLBACK_MODE:
		case UART_MODE:
		case SOCKET_MODE:
		case MEMORY_DEVICE_MODE:
			new_mode = DIAG_MEMORY_DEVICE_MODE;
			break;
		case USB_MODE:
			new_mode = DIAG_USB_MODE;
			break;
		case PCIE_MODE:
			new_mode = DIAG_PCIE_MODE;
			break;
		default:
			ALOGE("diag: %s: request to switch to invalid mode: %d\n",
				   __func__, param->req_mode);
			return -EINVAL;
		}
		err = diag_switch_logging_proc(dm, param, new_mode, peripheral_mask);

	return err;
}
int diag_get_pid_from_fd(int fd)
{
	int proc, i;

	for (proc = 0; proc < NUM_DIAG_MD_DEV; proc++) {
		for (i = 0; i < NUM_MD_SESSIONS; i++) {
			if (!diagmem->md_session_map[proc][i])
				continue;
			if (diagmem->md_session_map[proc][i] && diagmem->md_session_map[proc][i]->dm &&
				diagmem->md_session_map[proc][i]->dm->in_fd == fd)
				return diagmem->md_session_map[proc][i]->pid;
		}
	}
	return 0;
}

void diag_clear_masks(struct diag_client *client, int pid)
{
	char cmd_disable_log_mask[] = { 0x73, 0, 0, 0, 0, 0, 0, 0};
	char cmd_disable_msg_mask[] = { 0x7D, 0x05, 0, 0, 0, 0, 0, 0};
	char cmd_disable_event_mask[] = { 0x60, 0};
	char cmd_disable_qtrace_mask[] = { 0x4B, 0x44, 0x01, 0x90, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	ALOGI("diag: %s: Mask clear initiated for session with pid: %d\n", __func__, pid);

	diag_client_handle_command(client,
		(uint8_t *)cmd_disable_log_mask, sizeof(cmd_disable_log_mask), pid);

	diag_client_handle_command(client,
		(uint8_t *)cmd_disable_msg_mask, sizeof(cmd_disable_msg_mask), pid);

	diag_client_handle_command(client,
		(uint8_t *)cmd_disable_event_mask, sizeof(cmd_disable_event_mask), pid);
	diag_client_handle_command(client,
		(uint8_t *)cmd_disable_qtrace_mask, sizeof(cmd_disable_qtrace_mask), pid);
}

void diag_set_clear_masks_flags(int state)
{
	printf("diag: %s: set clear mask state: %d\n", __func__, state);
	if (state)
		clear_masks = true;
	else
		clear_masks = false;
}

int diag_send_all_mask_clear(struct diag_client *client, int pid)
{
	if (clear_masks) {
		diag_clear_masks(client, pid);
		/* return 1 to indicate the mask clear initiated status */
		return 1;
	} else {
		ALOGD("diag: %s: Skipping clearing masks as per user requirement\n", __func__);
		/* return 0 to indicate clearing masks are skipped */
		return 0;
	}
}

void diag_close_logging_process(struct diag_client *dm, int pid)
{
	int i, j;
	int session_mask = 0;
	int device_mask = 0;
	struct diag_md_session_t *session_info = NULL;
	struct diag_logging_mode_param_t params;

	session_info = diag_md_session_get_pid(pid);
	if (!session_info) {
		ALOGE("diag: %s: Exiting client (pid: %d) is not an MD client\n", __func__, pid);
		return;
	}
	for (i = 0; i < NUM_DIAG_MD_DEV; i++) {
		if (session_info->peripheral_mask[i]) {
			session_mask = session_info->peripheral_mask[i];
			device_mask = device_mask | (1 << i);
		}
	}

	diag_send_all_mask_clear(session_info->dm, pid);

	for (i = 0; i < NUM_MD_SESSIONS; i++)
		if (MD_PERIPHERAL_MASK(i) & session_mask)
			diag_mux_close_peripheral(DIAG_LOCAL_PROC, i);

	params.req_mode = USB_MODE;
	params.mode_param = 0;
	params.pd_mask = 0;
	params.peripheral_mask = session_mask;
	params.device_mask = device_mask;
	params.pid = pid;

	if (diagmem->num_pd_session > 0) {
		for (i = UPD_WLAN; (i < NUM_MD_SESSIONS); i++) {
			if (session_mask & MD_PERIPHERAL_MASK(i)) {
				j = i - UPD_WLAN;
				diagmem->pd_session_clear[j] = 1;
				diagmem->pd_logging_mode[j] = 0;
				diagmem->num_pd_session -= 1;
				if (session_info->pd_mask)
					params.pd_mask = session_info->pd_mask;
				else
					params.pd_mask = session_mask;
			}
		}
	}
	ALOGD("diag: %s: Closing md session on pid: %d\n", __func__, pid);
	diag_md_session_close(pid);
	diag_switch_logging(dm, &params, sizeof(params));

}

int get_remote_token(int token)
{
	switch (token) {
	case MDM_TOKEN:
	case MDM_2_TOKEN:
		return -token;
	default:
		return 0;
	}
}

int diag_process_userspace_data(struct diag_client *dm, unsigned char  *buf, int len, int pid)
{
	struct hdlc_decoder recv_decoder;
	struct circ_buf recv_buf;
	size_t msglen;
	void *msg;
	int remote_token = 0, token_offset, hdlc_disabled;
	struct diag_pkt_frame_t *pkt_ptr = NULL;
	struct diag_md_session_t *session_info = NULL;

	if (!buf)
		return -EINVAL;

	if (len <= 0 || len >= HDLC_BUF_SIZE) {
		ALOGE("diag: %s: Invalid len received :%d\n", __func__, len);
		return 0;
	}

	if ( len >= sizeof(int))
		remote_token = *(int *) buf;
	remote_token = get_remote_token(remote_token);
	if (remote_token) {
		token_offset = sizeof(int);
		if (len <= sizeof(int))
			return 0;
		len = len - token_offset;
		diag_mhi_write(buf + token_offset, len, remote_token);
	} else {
		session_info = diag_md_session_get_pid(pid);
		if (session_info)
			hdlc_disabled = session_info->hdlc_disabled;
		else
			hdlc_disabled = diagmem->hdlc_disabled;
		if (hdlc_disabled) {
			pkt_ptr = buf;
			msglen = pkt_ptr->length;
			if (pkt_ptr->start != CONTROL_CHAR || *(uint8_t *)(pkt_ptr->data+pkt_ptr->length) != CONTROL_CHAR) {
				diag_start_hdlc_recovery(pid, msglen);
			} else {
				diag_client_handle_command(dm, &pkt_ptr->data[0], msglen, pid);
			}
		} else {
			memset(&recv_decoder, 0, sizeof(recv_decoder));
			memcpy(recv_buf.buf, buf, len);
			recv_buf.tail = 0;
			recv_buf.head = len;
			msg = hdlc_decode_one(&recv_decoder, &recv_buf, &msglen);
			if (!msg)
				return 0;

			diag_client_handle_command(dm, msg, msglen, pid);
		}
	}
	return 0;
}

int diag_query_pd_logging(struct diag_logging_mode_param_t *param)
{
	int peripheral = -EINVAL;
	int i, j;

	if (!param)
		return -EINVAL;

	if (!param->pd_mask) {
		ALOGE("diag: query with no pd mask set, returning error\n");
		return -EINVAL;
	}

	for (i = UPD_WLAN, j = DIAG_CON_UPD_WLAN;
			i < NUM_MD_SESSIONS || j > DIAG_CON_UPD_LAST; i++) {
		if (param->pd_mask & j) {
			peripheral = diag_query_peripheral_from_pd(i);
			break;
		}
		j = j << 1;
	}

	if (peripheral < 0) {
		ALOGE("diag: diag_id support is not present for the pd mask = %d\n",
			param->pd_mask);
		return -EINVAL;
	}

	if (!diagmem->feature[peripheral].untag_header)
		return -EINVAL;

	return 0;
}

int diag_process_userspace_raw_data(struct diag_client *dm, unsigned char  *buf, int len, int pid)
{
	int remote_token = 0, token_offset;
	void *outbuf;
	size_t outlen;

	if (len >= sizeof(int))
		remote_token = *(int *) buf;
	remote_token = get_remote_token(remote_token);
	if (remote_token) {
		token_offset = sizeof(int);
		if (len <= sizeof(int))
			return 0;
		len = len - token_offset;
		if (dm && dm->hdlc_enc_done) {
			outbuf = buf;
			outlen = len;
		} else {
			outbuf = hdlc_encode(dm, buf + token_offset, (size_t)len, &outlen);
			if (!outbuf) {
				ALOGE("diag: failed to allocate hdlc destination buffer\n");
				return -ENOMEM;
			}
		}
		diag_mhi_write(outbuf, outlen, remote_token);
		if (!dm) {
			free(outbuf);
			outbuf = NULL;
		}
	} else {
		diag_client_handle_command(dm, buf, len, pid);
	}

	return 0;
}
int diag_process_register_callback(struct diag_client *dm, unsigned char  *buf, int len)
{
	int err = 0;
	int proc = * (int *)buf;
	struct diag_reg_cb{
		int data_type;
		int ret_val;
	}__packed;
	struct diag_reg_cb cb;
	(void)len;
	cb.data_type = 0x64000;

	if (proc < 0 || proc >= NUM_DIAG_MD_DEV) {
		ALOGE("diag: In %s, invalid proc %d for callback registration\n",
			__func__, proc);
		err = -EINVAL;
	}
	if (diagmem->md_session_mode[proc] == DIAG_MD_PERIPHERAL)
		err = -EIO;
	cb. ret_val = err;
	dm_send(dm, &cb, sizeof(cb));
	return err;

}
int diag_process_vote_realtime(struct real_time_vote_t *vote, int len)
{
	int temp_proc, real_time;
	struct diag_dci_client_tbl *dci_client = NULL;
	(void)len;

	if (vote->proc > DIAG_PROC_VOTE_REALTIME_MEMORY_DEVICE ||
		vote->real_time_vote > MODE_UNKNOWN ||
		vote->client_id < 0) {
		ALOGE("diag: %s, invalid params, proc: %d, vote: %d, client_id: %d\n",
			__func__, vote->proc, vote->real_time_vote,
			vote->client_id);
		return -EINVAL;
	}
	real_time = vote->real_time_vote;
	if (vote->proc == DIAG_PROC_VOTE_REALTIME_DCI) {
		dci_client = diag_dci_get_client_entry(vote->client_id);
		if (!dci_client) {
 			return DIAG_DCI_NOT_SUPPORTED;
			}
		diag_dci_set_real_time(dci_client, vote->real_time_vote);
		diag_update_real_time_vote(vote->proc, real_time, dci_client->client_info.token);
		}
	else {
		temp_proc = vote->client_id;
		diag_update_real_time_vote(vote->proc, real_time, temp_proc);
	}
	diag_send_real_time();
	return 0;
}
int diag_get_real_time(struct real_time_query_t * rt_query, int len)
{
	int i;
	(void)len;

	if (!rt_query || rt_query->proc >= DIAG_NUM_PROC)
		return -EINVAL;

	rt_query->real_time = diagmem->real_time_mode[rt_query->proc];
        if (rt_query->proc == DIAG_LOCAL_PROC) {
		for (i = 0; i < NUM_PERIPHERALS; i++) {
                    if (!diagmem->feature[i].peripheral_buffering)
                        continue;
                    switch (diagmem->buffering_mode[i].mode) {
                        case DIAG_BUFFERING_MODE_CIRCULAR:
                        case DIAG_BUFFERING_MODE_THRESHOLD:
                             rt_query->real_time = MODE_UNKNOWN;
                             break;
                    }
               }
        }
	return 0;

}


int diag_peripheral_data_drain_immediate(struct diag_perif_drain *buf, int len)
{
	uint8_t pd, diag_id = 0;
	int peripheral = 0, stream_id = DIAG_STREAM_1;
	(void)len;

	pd = buf->peripheral;
	if (pd >=0 && pd < NUM_PERIPHERALS-1)
		pd = pd + 1;
	diag_map_pd_to_diagid(pd, &diag_id, &peripheral);
	if ((peripheral < 1) ||
		peripheral >= NUM_PERIPHERALS) {
		ALOGE("diag: In %s, invalid peripheral %d\n", __func__, peripheral);
		return -EINVAL;
	}
	if (pd > NUM_PERIPHERALS &&
		!diagmem->feature[peripheral].pd_buffering) {
		ALOGE("diag: In %s, pd buffering not supported for peripheral:%d\n",
			__func__, peripheral);
		return -EIO;
	}

	return diag_cntl_drain_perif_buf(diag_id, peripheral, stream_id);
}

int diag_set_peripheral_buffering_mode(struct diag_buffering_mode_t *params, int len)
{
	int peripheral = 0;
	uint8_t diag_id = 0;
	(void)len;

	if (params->peripheral >=0 && params->peripheral < NUM_PERIPHERALS-1)
		params->peripheral = params->peripheral + 1;
	diag_map_pd_to_diagid(params->peripheral, &diag_id, &peripheral);
	if ((peripheral < 1) ||
		peripheral >= NUM_PERIPHERALS) {
		ALOGE("diag: In %s, invalid peripheral = %d\n", __func__,
			peripheral);
		return -EIO;
	}
	if (params->peripheral > NUM_PERIPHERALS &&
		!diagmem->feature[peripheral].pd_buffering) {
		ALOGE("diag: In %s, pd buffering not supported for peripheral:%d\n",
			__func__, peripheral);
		return -EIO;
	}

	if (!diagmem->feature[peripheral].peripheral_buffering) {
		ALOGE("diag: In %s, peripheral %d doesn't support buffering\n",
			__func__, peripheral);
		return -EIO;
	}
	diagmem->buffering_flag[params->peripheral] = 1;

	return diag_config_perif_buf(params, DIAG_STREAM_1);
}

static void diag_query_reg_table(struct diag_client *dm)
{
	int data_type;
	unsigned char *ptr;
	struct list_head *item;
	struct diag_cmd *dc;
	struct diag_cmd_dbg *dc_dbg;

	ptr = alloca(sizeof(data_type) + (sizeof(struct diag_cmd_dbg)));
	if (!ptr)
		return;

	dc_dbg = calloc(1, sizeof(*dc_dbg));
	if (!dc_dbg) {
		ALOGE("failed to allocate dc_dbg\n");
		free(ptr);
		ptr = NULL;
		return;
	}
	data_type = REG_TABLE_MASK;
	memcpy(ptr, &data_type, sizeof(data_type));

	list_for_each(item, &apps_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (dc) {
			dc_dbg->first = dc->first;
			dc_dbg->last = dc->last;
			dc_dbg->periph_id = dc->fd;
			memcpy(ptr + sizeof(data_type), dc_dbg, sizeof(struct diag_cmd_dbg));
		}
		dm_send(dm, ptr, sizeof(data_type) + (sizeof(struct diag_cmd_dbg)));
	}
#ifndef FEATURE_LE_DIAG
	list_for_each(item, &apps_system_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (dc) {
			dc_dbg->first = dc->first;
			dc_dbg->last = dc->last;
			dc_dbg->periph_id = dc->fd;
			memcpy(ptr + sizeof(data_type), dc_dbg, sizeof(struct diag_cmd_dbg));
		}
		dm_send(dm, ptr, sizeof(data_type) + (sizeof(struct diag_cmd_dbg)));
	}
#endif /* FEATURE_LE_DIAG */
	list_for_each(item, &diag_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (dc) {
			dc_dbg->first = dc->first;
			dc_dbg->last = dc->last;
			dc_dbg->periph_id = dc->peripheral->periph_id;
			memcpy(ptr + sizeof(data_type), dc_dbg, sizeof(struct diag_cmd_dbg));
		}
		dm_send(dm, ptr, sizeof(data_type) + (sizeof(struct diag_cmd_dbg)));
	}
	dc_dbg->first = DIAG_END_OF_PACKET;
	memcpy(ptr + sizeof(data_type), dc_dbg, sizeof(struct diag_cmd_dbg));
	dm_send(dm, ptr, sizeof(data_type) + (sizeof(struct diag_cmd_dbg)));
	free(dc_dbg);
	dc_dbg = NULL;
}

void diag_query_debug_all(struct diag_client *dm, struct diag_debug_all_param_t *req_params, int len)
{
	int i = 0, j = 0;
	struct diag_query_debug_all_pkt{
		int data_type;
		struct diag_debug_all_param_t params;
	}__packed;
	struct diag_query_debug_all_pkt pkt;
	(void)req_params;
	(void)len;

	pkt.data_type = DEBUG_ALL_MASK;
	pkt.params.time_api = diagmem->time_api;
	pkt.params.timesync_enabled = diagmem->timesync_enabled;
	pkt.params.timestamp_switch = diagmem->timestamp_switch;
	for (i = 0; i < NUM_PERIPHERALS; i++) {
		pkt.params.stm_support[i] = diagmem->stm_support[i];
		pkt.params.stm_state_requested[i] = diagmem->stm_state_requested[i];
		pkt.params.stm_state[i] = diagmem->stm_state[i];
	}
	pkt.params.apps_feature = diagmem->apps_feature;
	for (i = 0; i < DIAGID_V2_FEATURE_COUNT; i++) {
		pkt.params.diagid_v2_feature[i] = diagmem->diagid_v2_feature[i];
		pkt.params.diagid_v2_status[i] = diagmem->diagid_v2_status[i];
		pkt.params.diag_hw_accel[i] = diagmem->diag_hw_accel[i];
	}
	for (i = 0; i < NUM_PERIPHERALS; i++)
		pkt.params.feature[i].feature_mask = diagmem->feature[i].feature_mask;
	for (i = 0; i < NUM_DIAG_MD_DEV; i++) {
		pkt.params.logging_mode[i] = diagmem->logging_mode[i];
		pkt.params.logging_mask[i] = diagmem->logging_mask[i];
	}
	for (i = 0; i < NUM_UPD; i++) {
		pkt.params.pd_logging_mode[i] = diagmem->pd_logging_mode[i];
		pkt.params.pd_session_clear[i] = diagmem->pd_session_clear[i];
	}
	pkt.params.num_pd_session = diagmem->num_pd_session;
	for (i = 0; i < NUM_DIAG_MD_DEV; i++) {
		pkt.params.md_session_mask[i] = diagmem->md_session_mask[i];
		pkt.params.md_session_mode[i] = diagmem->md_session_mode[i];
	}
	for (i = 0; i < DIAG_NUM_PROC; i++)
		pkt.params.real_time_mode[i] = diagmem->real_time_mode[i];
	pkt.params.proc_active_mask = diagmem->proc_active_mask;
	for (i = 0; i < DIAG_NUM_PROC; i++)
		pkt.params.proc_rt_vote_mask[i] = diagmem->proc_rt_vote_mask[i];
	for (i = 0; i < NUM_MD_SESSIONS; i++) {
		pkt.params.buffering_mode[i].peripheral = diagmem->buffering_mode[i].peripheral;
		pkt.params.buffering_mode[i].mode = diagmem->buffering_mode[i].mode;
		pkt.params.buffering_mode[i].high_wm_val = diagmem->buffering_mode[i].high_wm_val;
		pkt.params.buffering_mode[i].low_wm_val = diagmem->buffering_mode[i].low_wm_val;
		pkt.params.buffering_flag[i] = diagmem->buffering_flag[i];
	}
	for (i = 0; i < DIAG_NUM_PROC; i++) {
		for (j = 0; j < NUM_PERIPHERALS; j++) {
			pkt.params.dci_buffering_mode[i][j].peripheral = diagmem->dci_buffering_mode[i][j].peripheral;
			pkt.params.dci_buffering_mode[i][j].mode = diagmem->dci_buffering_mode[i][j].mode;
			pkt.params.dci_buffering_mode[i][j].high_wm_val = diagmem->dci_buffering_mode[i][j].high_wm_val;
			pkt.params.dci_buffering_mode[i][j].low_wm_val = diagmem->dci_buffering_mode[i][j].low_wm_val;
		}
	}
	pkt.params.usb_connected = diagmem->usb_connected;
	dm_send(dm, &pkt, sizeof(pkt));
}

int diag_clean_up_dci_entry(struct diag_client *dm)
{
	struct diag_dci_client_tbl *dci_client;
	int result;

	dci_client = diag_dci_find_client_entry(dm);
	if (!dci_client) {
			return DIAG_DCI_NOT_SUPPORTED;
	}
	result = diag_dci_deinit_client(dci_client);

	return result;
}

void diag_flush_aggregate_buf(struct diag_client *dm)
{
	if (!dm)
		return;
	dm->hdlc_enc_done = 1;
	diag_mux_write(dm, 0, PERIPHERAL_APPS, dm->data_buf, dm->data_bytes_read, dm->flow, 0);
	dm->hdlc_enc_done = 0;
	memset(dm->data_buf, 0, dm->data_bytes_read);
	dm->data_bytes_read = 0;
	dm->copy_count = 0;
}

/*
 * extract the event id from the packet. Event_id is cleared
 * in failure scenario.
 */
void diag_extract_event_id(uint8_t *pkt, ssize_t len, uint16_t *event_id)
{
	uint16_t id;

	if(!pkt || !event_id)
		return;

	/* Event packet format :
	 * Event header [CMD_CODE(1byte) + Length(2bytes)] + Event_ID(2bytes)
	 */
	if (len < (EVENT_PKT_HEADER_SIZE + EVENT_ID_SIZE)) {
		*event_id = 0;
		return;
	}

	/* Event ID will be available next after event header */
	id = *(uint16_t *)(pkt + EVENT_PKT_HEADER_SIZE);
	/* Mask least 12 bits which holds event id */
	id &= EVENT_ID_MASK;
	*event_id = id;
}

static void diag_prepare_response(struct diag_client *dm, unsigned char *buf, ssize_t n)
{
	struct diag_id_cmd_resp_header_t ext_header = {0,};
	struct diag_pkt_frame_t header;
	int ext_header_size = 0;
	uint32_t ts_lo;
	uint32_t ts_hi;
	int len = 0;

	if ((n + sizeof(header)) > DIAG_MAX_RSP_SIZE) {
		ALOGE("diag: %s: Recvd size %zu too big\n", __func__, n);
		return;
	}

	/* fill diag-id header if the diag-id based req/resp enabled by the tool */
	if (vm_enabled && pkt_format_select_check_mask(PKT_FORMAT_MASK_CMD_REQ_RESP)) {
		if ((n + sizeof(header) + sizeof(ext_header)) > DIAG_MAX_RSP_SIZE) {
			ALOGE("diag: %s: Recvd resp size %zu cannot handle\n",
				__func__, n);
			return;
		}
		ext_header.cmd_code = DIAG_CMD_DIAG_ID_CMD_REQ;
		ext_header.version = DIAG_CMD_DIAG_ID_CMD_VERSION_1;
		ext_header.diag_id = diagmem->diag_id;
		ext_header.time_type = 0;
		ts_get_lohi(&ts_lo, &ts_hi);
		ext_header.time_lo = ts_lo;
		ext_header.time_hi = ts_hi;
		ext_header_size = sizeof(struct diag_id_cmd_resp_header_t);
	}

	memset(&header, 0, sizeof(header));
	header.start = CONTROL_CHAR;
	header.version = 1;
	header.length = n - 4 + ext_header_size;
	/* copy nHDLC header first */
	memcpy(dm->rsp_buf, &header, sizeof(header));
	len += sizeof(header);

	if (vm_enabled && pkt_format_select_check_mask(PKT_FORMAT_MASK_CMD_REQ_RESP)) {
		/* copy diag-id header */
		memcpy(dm->rsp_buf + len, &ext_header, sizeof(ext_header));
		len += sizeof(ext_header);
	}

	/* copy rest of the response from the client */
	memcpy(dm->rsp_buf + len, buf + 4, n - 4);
	len += (n-4);
	*(uint8_t *)(dm->rsp_buf + len) = CONTROL_CHAR;
	len += sizeof(uint8_t);
	if (diag_debug_mask & DIAG_DBG_MASK_CMD) {
		ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: cmd rsp (NHDLC) from client of pid: %d (fd: %d) of len: %d\n",
			__func__, dm->pid, dm->in_fd, len);
		print_hex_dump(__func__, (void *)dm->rsp_buf, MIN(len, PRINT_HEX_DUMP_LEN));
	}
	diag_mux_write(NULL, 0, PERIPHERAL_APPS, dm->rsp_buf, len, NULL, 1);
}

static int dm_recv_raw(struct diag_client *dm)
{
	int ret, fd;
	unsigned char buf[DIAG_MAX_RSP_SIZE];
	ssize_t n;
	int i =0, client_id, pid, debug_pid;
	int num_read;
	int data_flush = 0, dm_buff_fill = 0;
	int data_type;
	uint16_t mask = 0;
	unsigned char *ptr, *data_buf_ptr = NULL;
	struct dm_pkt{
		int type;
		unsigned char pkt;
	};
	struct dm_pkt *dmpkt;
	size_t outlen;
	void *outbuf;
	struct dm_pkt pkt_type = {0,0};
	uint16_t event_id = 0;
	int max_pkt_size = USER_SPACE_HDLC_BUF;

	if (!dm)
		return -EINVAL;

	if (dm->data_buf)
		data_buf_ptr = dm->data_buf;

	/* maximum packet size over qrtr vm socket is limited to 16KB */
	if (vm_enabled)
		max_pkt_size = QRTR_PKT_MAX_SIZE;

	for (num_read = 0; num_read < FD_MAX_NUM_READ; num_read++) {
		n = read(dm->in_fd, buf, sizeof(buf));
		if (!n) {
			ALOGE("diag: Empty read, Close logging pid: %d, fd %d \n", dm->pid, dm->in_fd);
			diag_clean_up_dci_entry(dm);
			pid = diag_get_pid_from_fd(dm->in_fd);
			diag_close_logging_process(dm, pid);
			watch_remove_fd(dm->in_fd);
			diag_remove_cmd_registrations(dm->in_fd);
			fd = dm->in_fd;
			dm_del(dm->in_fd);
			close(fd);
			socket_counter--;
			ALOGE("diag: While empty read socket counter:%d\n", socket_counter);
			return 0;
		} else if (n < 0 && errno == EAGAIN) {
			break;
		} else if (n < 0) {
			ALOGE("diag: Failed to read from %s, pid: %d\n", dm->name, dm->pid);
			ALOGE("diag: Invalid read, Close logging for pid: %d, fd %d err: %ld\n", dm->pid, dm->in_fd, n);
			diag_clean_up_dci_entry(dm);
			pid = diag_get_pid_from_fd(dm->in_fd);
			diag_close_logging_process(dm, pid);
			watch_remove_fd(dm->in_fd);
			diag_remove_cmd_registrations(dm->in_fd);
			fd = dm->in_fd;
			dm_del(dm->in_fd);
			close(fd);
			socket_counter--;
			ALOGE("diag: While invalid read socket_counter:%d\n", socket_counter);
			return 0;
		} else {
			if (num_read == (FD_MAX_NUM_READ - 1))
				data_flush = 1;
		}
		dmpkt = (struct dm_pkt *)buf;
repeat:
		switch (dmpkt->type) {
		case DATA_TYPE_MULTI_STREAM_LOG:
			/* Multi stream log packet has to be processed on legacy and DCI */
			ALOGM(DIAG_DBG_MASK_DCI, "diag: %s: Received multistream log pkt\n",
				__func__);
			if (dmpkt->type & DATA_TYPE_DCI_LOG) {
				diag_process_apps_dci_read_data((char*)buf + 4, n - 4, dm->flow);
				dmpkt->type &= ~DATA_TYPE_DCI_LOG;
			}

			if (!(dmpkt->type & DATA_TYPE_LOG))
				break;
		case DATA_TYPE_EVENT:
		case DATA_TYPE_LOG:
		case DATA_TYPE_F3:
		{
			struct diag_pkt_frame_t header;
			int len = 0;

			if (diagmem->p_hdlc_disabled[PERIPHERAL_APPS] || vm_enabled) {
				if (dm->data_buf && ((dm->data_bytes_read + (n - 4) + 5 > max_pkt_size) || data_flush)) {
					diag_mux_write(NULL, 0, PERIPHERAL_APPS, dm->data_buf, dm->data_bytes_read, dm->flow, 0);
					memset(dm->data_buf, 0, dm->data_bytes_read);
					dm->data_bytes_read = 0;
					dm->copy_count = 0;
					data_buf_ptr = dm->data_buf;
					dm_buff_fill = 0;
				}

				if (data_buf_ptr && (((n - 4) + 5 ) < max_pkt_size)) {
					memset(&header, 0, sizeof(header));
					header.start = CONTROL_CHAR;
					header.version = 1;
					header.length = n - 4;
					memcpy(data_buf_ptr, &header, sizeof(header));
					len = len+ sizeof(header);
					memcpy(data_buf_ptr + len, buf + 4, n - 4);
					if (dm->pkt_count == UINT64_LIMIT) {
						ALOGD("diag: %s: Received packet count from %d client has reached maximum limit. Resetting count to zero\n", __func__, dm->pid);
						dm->pkt_count = 0;
						dm->max_pkt_count++;
					}
					dm->pkt_count++;
					dm->copy_count++;
					len = len + (n - 4);
					*(uint8_t *)(data_buf_ptr + len) = CONTROL_CHAR;
					len += sizeof(uint8_t);
					data_buf_ptr += len;
					dm->data_bytes_read += len;
					dm_buff_fill++;
				}
			} else {
				if ((dm->data_bytes_read + (n - 4) > USER_SPACE_HDLC_BUF) || data_flush) {
					diag_flush_aggregate_buf(dm);
					data_buf_ptr = dm->data_buf;
					dm_buff_fill = 0;
				}
				if (data_buf_ptr && (n - 4 < USER_SPACE_DATA)) {
					outbuf = hdlc_encode(dm, buf + 4, n - 4, &outlen);
					if (outlen + dm->data_bytes_read > USER_SPACE_HDLC_BUF) {
						diag_flush_aggregate_buf(dm);
						data_buf_ptr = dm->data_buf;
						dm_buff_fill = 0;
					}
					if (!outbuf)
						continue;
					memcpy(data_buf_ptr, outbuf, outlen);
					if (dm->pkt_count == UINT64_LIMIT) {
						ALOGD("diag: %s: Received packet count from %d client has reached maximum limit. Resetting count to zero\n", __func__, dm->pid);
						dm->pkt_count = 0;
						dm->max_pkt_count++;
					}
					dm->pkt_count++;
					dm->copy_count++;
					memset(outbuf, 0, outlen);
					data_buf_ptr += outlen;
					dm->data_bytes_read += outlen;
					dm_buff_fill++;
				}
			}
			break;
		}
		case DATA_TYPE_RSP:
		case DATA_TYPE_DELAYED_RSP:
		{
			if (diagmem->p_hdlc_disabled[PERIPHERAL_APPS]) {
				diag_prepare_response(dm, buf, n);
			} else {
				if (diag_debug_mask & DIAG_DBG_MASK_CMD) {
					ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: cmd rsp from client of pid: %d (fd: %d) of len: %d\n",
						__func__, dm->pid, dm->in_fd, n - 4);
					print_hex_dump(__func__, (void *)(buf + 4), MIN(n - 4, PRINT_HEX_DUMP_LEN));
				}
				diag_mux_write(NULL, 0, PERIPHERAL_APPS, buf + 4, n - 4, NULL, 1);
			}
			break;
		}
		case DATA_TYPE_CMD_REG:
			diag_add_apps_cmd_registrations(dm->in_fd, buf + 4, n - 4);
			break;
		case DATA_TYPE_CMD_DEREG:
			diag_remove_cmd_registrations(dm->in_fd);
			break;
		case DATA_TYPE_LSM_DEINIT:
			ALOGD("diag: Initiate Deinit for pid: %d, fd %d\n", dm->pid, dm->in_fd);
			data_type = DEINIT_TYPE;
			dm_send(dm, &data_type, sizeof(data_type));
			pid = diag_get_pid_from_fd(dm->in_fd);
			diag_close_logging_process(dm, pid);
			diag_remove_cmd_registrations(dm->in_fd);
			break;
		case DATA_TYPE_GET_DELAYED_RSP_ID:
		{
			uint16_t delayed_rsp_id;
			delayed_rsp_id = diag_get_next_delayed_rsp_id();
			ptr = alloca(sizeof(data_type) + sizeof(delayed_rsp_id));
			if (!ptr)
				break;
			data_type = DELAYED_RSP_MASK_TYPE;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), &delayed_rsp_id, sizeof(delayed_rsp_id));
			dm_send(dm, ptr, sizeof(data_type) + sizeof(delayed_rsp_id));
			break;
		}
		case DATA_TYPE_PASSTHRU_CONTROL:
			diag_send_passthru_control_pkt(dm, (struct diag_hw_accel_cmd_req_t *)(buf + 4), n - 4);
			break;
		case DATA_TYPE_QUERY_PD_FEATUREMASK:
			diag_cntl_query_pd_featuremask(dm, (struct diag_hw_accel_query_sub_payload_rsp_t *)(buf + 4));
			break;
		case DATA_TYPE_QUERY_CON_ALL:
			diag_query_con_all(dm, (struct diag_con_all_param_t *)(buf + 4), n - 4);
			break;
		case DATA_TYPE_QUERY_MD_PID:
			diag_query_md_pid(dm, (struct diag_query_pid_t *)(buf + 4), n - 4);
			break;
		case DATA_TYPE_SWITCH_LOGGING:
			/* loggin path switch won't support in gvm */
			if (vm_enabled)
				break;
			ret = diag_switch_logging(dm, (struct diag_logging_mode_param_t *)(buf + 4), n - 4);
			ptr = alloca(sizeof(data_type) + sizeof(ret));
			if (!ptr)
				break;
			data_type = DIAG_SWITCH_LOGGING_MASK;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), &ret, sizeof(ret));
			dm_send(dm, ptr, sizeof(data_type) + sizeof(ret));
			break;
		case USER_SPACE_DATA_TYPE:
		{
			pid = dm->override_pid ? dm->override_pid : dm->pid;
			diag_process_userspace_data(dm, buf + 4 , n - 4, pid);
			break;
		}
		case USER_SPACE_RAW_DATA_TYPE:
		{
			pid = dm->override_pid ? dm->override_pid : dm->pid;
			diag_process_userspace_raw_data(dm, buf + 4 , n -4, pid);
			break;
		}
		case DATA_TYPE_REGISTER_CALLBACK:
			diag_process_register_callback(dm, buf + 4, n - 4);
			break;
		case DATA_TYPE_HDLC_TOGGLE:
		{
			int pid;

			pid = diag_get_pid_from_fd(dm->in_fd);
			diag_hdlc_toggle(pid, *(buf + 4));
			break;
		}
		case DATA_TYPE_MDM_HDLC_TOGGLE:
		{
			int pid;

			pid = diag_get_pid_from_fd(dm->in_fd);
			diag_mdm_hdlc_toggle(pid, *(buf + 4));
			break;
		}
		case DATA_TYPE_VOTE_REALTIME:
			diag_process_vote_realtime((struct real_time_vote_t *)(buf + 4), n -4);
			break;
		case DATA_TYPE_GET_REALTIME:
			{
				struct real_time_query_t * rt_query = (struct real_time_query_t *) (buf + 4);
				ptr = alloca(sizeof(data_type) + sizeof(rt_query->real_time));
				if (!ptr)
					break;
				data_type = GET_REAL_TIME_MASK;
				rt_query->real_time = diagmem->real_time_mode[rt_query->proc];
				if (rt_query->proc == DIAG_LOCAL_PROC) {
					for (i = 0; i < NUM_PERIPHERALS; i++) {
						if (!diagmem->feature[i].peripheral_buffering)
							continue;
						switch (diagmem->buffering_mode[i].mode) {
						case DIAG_BUFFERING_MODE_CIRCULAR:
						case DIAG_BUFFERING_MODE_THRESHOLD:
							rt_query->real_time = MODE_UNKNOWN;
							break;
						}
					}
				}
				memcpy(ptr, &data_type, sizeof(data_type));
				memcpy(ptr + sizeof(data_type), &rt_query->real_time, sizeof(rt_query->real_time));
				dm_send(dm, ptr, sizeof(data_type) + sizeof(rt_query->real_time));
				break;
			}
		case DATA_TYPE_PERIPHERAL_BUF_DRAIN:
			diag_peripheral_data_drain_immediate((struct diag_perif_drain *)(buf + 4), n - 4);
			break;
		case DATA_TYPE_PERIPHERAL_BUF_CONFIG:
			diag_set_peripheral_buffering_mode((struct diag_buffering_mode_t *)(buf + 4), n - 4);
			break;
		case DATA_TYPE_QUERY_DEVICE_MASK:
			mask = diag_get_remote_mask_info();
			ptr = alloca(sizeof(data_type) + sizeof(mask));
			if (!ptr)
				break;
			data_type = DEVICE_MASK_TYPE;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), &mask, sizeof(mask));
			dm_send(dm, ptr, sizeof(data_type) + sizeof(mask));
			break;
		case DATA_TYPE_QUERY_REG_TABLE:
			diag_query_reg_table(dm);
			break;
		case DATA_TYPE_QUERY_DEBUG_ALL:
			diag_query_debug_all(dm, (struct diag_debug_all_param_t *)(buf + 4), n - 4);
			break;
#ifndef DIAG_VM_ENABLED
		case DATA_TYPE_DCI_REG:
		{
			int pid = 0;
			pid = *(int *) (buf + 4);
			client_id = diag_dci_register_client(buf + 8, dm, pid);
			ptr = alloca(sizeof(data_type) + sizeof(client_id));
			if (!ptr) {
				ALOGE("diag: In %s failed to allocate memory for data type %d\n", __func__, dmpkt->type);
				break;
			}
			data_type = DCI_REG_CLIENT_ID_MASK;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), &client_id, sizeof(client_id));
			dm_send(dm, ptr, sizeof(data_type) + sizeof(client_id));
			break;
		}
		case DATA_TYPE_DCI_SUPPORT:
			{
				struct diag_dci_peripherals_t *dci_support = (struct diag_dci_peripherals_t *) (buf + 4);
				diag_dci_get_support_list(dci_support);
				ALOGI("diag: DCI support query for periph:%d proc %d\n", dci_support->list, dci_support->proc);
				ptr = alloca(sizeof(data_type) + sizeof(uint16_t));
				if (!ptr) {
					ALOGE("diag: In %s failed to allocate memory for data type %d\n", __func__, dmpkt->type);
					break;
				}
				data_type = DCI_SUPPORT_MASK;
				memcpy(ptr, &data_type, sizeof(data_type));
				memcpy(ptr + sizeof(data_type), &dci_support->list, sizeof(uint16_t));
				dm_send(dm, ptr, sizeof(data_type) + sizeof(uint16_t));
				break;
			}
		case DATA_TYPE_DCI_HEALTH_STATS:
			{
				struct diag_dci_health_stats_proc *stats = (struct diag_dci_health_stats_proc *) (buf + 4);
				diag_dci_copy_health_stats(stats);
				ptr = alloca(sizeof(data_type) + sizeof(*stats));
				if (!ptr) {
					ALOGE("diag: In %s failed to allocate memory for data type %d\n", __func__, dmpkt->type);
					break;
				}
				data_type = DCI_HEALTH_STATS_MASK;
				memcpy(ptr, &data_type, sizeof(data_type));
				memcpy(ptr + sizeof(data_type), stats, sizeof(*stats));
				dm_send(dm, ptr, sizeof(data_type) + sizeof(*stats));
				break;
			}
		case DATA_TYPE_DCI_LOG_STATUS:
			{
				struct diag_log_event_stats le_stats;
				struct diag_dci_client_tbl *dci_client = NULL;
				memcpy(&le_stats, buf+4, sizeof(le_stats));
				dci_client = diag_dci_get_client_entry(le_stats.client_id);
				le_stats.is_set = diag_dci_query_log_mask(dci_client, le_stats.code);
				ptr = alloca(sizeof(data_type) + sizeof(le_stats));
				if (!ptr) {
					ALOGE("diag: In %s failed to allocate memory for data type %d\n", __func__, dmpkt->type);
					break;
				}
				data_type = DCI_LOG_STATUS_MASK;
				memcpy(ptr, &data_type, sizeof(data_type));
				memcpy(ptr + sizeof(data_type), &le_stats.is_set, sizeof(int));
				dm_send(dm, ptr, sizeof(data_type) + sizeof(int));
				break;
			}
		case DATA_TYPE_DCI_EVENT_STATUS:
			{
				struct diag_log_event_stats le_stats;
				struct diag_dci_client_tbl *dci_client = NULL;
				memcpy(&le_stats, buf+4, sizeof(le_stats));
				dci_client = diag_dci_get_client_entry(le_stats.client_id);
				le_stats.is_set = diag_dci_query_event_mask(dci_client, le_stats.code);
				ptr = alloca(sizeof(data_type) + sizeof(le_stats));
				if (!ptr) {
					ALOGE("diag: In %s failed to allocate memory for data type %d\n", __func__, dmpkt->type);
					break;
				}
				data_type = DCI_EVENT_STATUS_MASK;
				memcpy(ptr, &data_type, sizeof(data_type));
				memcpy(ptr + sizeof(data_type), &le_stats.is_set, sizeof(int));
				dm_send(dm, ptr, sizeof(data_type) + sizeof(int));
				break;
			}
		case DATA_TYPE_DCI_CLEAR_LOGS:
			{
				int result;
				client_id = * (unsigned int *)((char*)buf + 4);
				result = diag_dci_clear_log_mask(client_id);
				break;
			}
		case DATA_TYPE_DCI_CLEAR_EVENTS:
			{
				int result;
				client_id = * (unsigned int *)((char*)buf + 4);
				result = diag_dci_clear_event_mask(client_id);
				break;
			}
		case DATA_TYPE_DCI:
			diag_process_dci_transaction(buf + 4, n - 4);
			break;
		case DATA_TYPE_DCI_DEINIT:
			{
				struct diag_dci_client_tbl *dci_client;
				int result;
				client_id = * (unsigned int *)((char*)buf + 4);
				ALOGD("diag: %s: dci deinit for client_id: %d\n", __func__, client_id);
				dci_client = diag_dci_get_client_entry(client_id);
				if (!dci_client) {
					return DIAG_DCI_NOT_SUPPORTED;
				}
				result = diag_dci_deinit_client(dci_client);
				break;
			}
		case DATA_TYPE_DCI_LOG:
		case DATA_TYPE_DCI_EVENT:
		case DATA_TYPE_DCI_PKT:
			ALOGM(DIAG_DBG_MASK_DCI, "diag: %s: received dci pkt type 0x%x\n",
				__func__, dmpkt->type);
			diag_process_apps_dci_read_data((char*)buf + 4, n - 4, dm->flow);
			/*
			 * For the case of pkt type set with
			 * 'DATA_TYPE_DCI_EVENT(0x200) | DATA_TYPE_EVENT (0x0)',
			 * process dci event pkt here and if valid event id is set in legacy mask,
			 * process the pkt in legacy packet handling.
			 */
			if (dmpkt->type & DATA_TYPE_DCI_EVENT) {
				diag_extract_event_id((char*)buf + 4, n - 4, &event_id);
				if (event_id && diag_check_event_mask(event_id)) {
					ALOGM(DIAG_DBG_MASK_DCI, "diag: %s: Event mask for id 0x%x is enabled in legacy\n",
						__func__, event_id);
					pkt_type.type = DATA_TYPE_EVENT;
					dmpkt = &pkt_type;
					goto repeat;
				}
			}
			break;
#endif /* DIAG_VM_ENABLED */
		case DATA_TYPE_SET_OVERRIDE_PID:
			pid = *(int *)(buf + sizeof(data_type));
			if (pid < 0 || pid >= PID_MAX) {
				ALOGE("diag: %s: pid: %d invalid override_pid: %d\n", __func__, dm->pid, pid);
				return -EINVAL;
			}
			ALOGE("diag: %s: fd: %d pid: %d set override_pid: %d\n", __func__, dm->in_fd, dm->pid, pid);
			dm_set_override_pid(dm, pid);
			break;
		case DATA_TYPE_UPDATE_DYN_DBG_MASK:
			diag_debug_mask = *(uint32_t *)(buf + sizeof(data_type));
			ALOGM(DIAG_DBG_MASK_INFO, "diag: Update dynamic debug mask: 0x%x\n", diag_debug_mask);
			if (n <= DIAG_DM_DBG_MASK_PKT_SIZE)
				break;
			debug_pid = *(int *)(buf + DIAG_DM_DBG_MASK_PKT_SIZE);
			data_type = DIAG_SET_DYNAMIC_DEBUG_MASK;
			ptr = alloca(sizeof(data_type) + sizeof(ret));
			if (!ptr) {
				ALOGE("diag: In %s: failed to allocate memory for data type %d\n", __func__, dmpkt->type);
				break;
			}
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), &diag_debug_mask, sizeof(int));
			dm_send_to_pid(ptr, sizeof(data_type) +
					sizeof(int), debug_pid);

			ALOGM(DIAG_DBG_MASK_INFO, "diag: Update dynamic debug pid: %d\n", debug_pid);
			break;
		case DATA_TYPE_UPDATE_QDSS_ETR1_SUPPORT:
			etr1_support = *(uint32_t *)(buf + sizeof(data_type));
			break;
		case DATA_TYPE_GET_TRANSPORT_MODE:
		{
			uint16_t transport_mode;
			ptr = alloca(sizeof(data_type) + sizeof(diagmem->transport_set));
			if (!ptr) {
				ALOGE("diag: %s: failed to allocate memory for data type %d\n", __func__, dmpkt->type);
				break;
			}
			data_type = DATA_TYPE_GET_TRANSPORT_MODE;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), &diagmem->transport_set, sizeof(diagmem->transport_set));
			dm_send(dm, ptr, sizeof(data_type) + sizeof(diagmem->transport_set));
			break;
		}
		case DATA_TYPE_DCI_PERIPHERAL_BUF_DRAIN_IMMEDIATE:
			diag_dci_peripheral_data_drain_immediate(buf + 4, n - 4);
			break;
		case DATA_TYPE_PERIPHERAL_DCI_BUF_CONFIG:
			diag_dci_vote_peripheral_buffering_mode(buf + 4, n - 4);
			break;
		default:
			break;
		}

		if (dmpkt->type == DATA_TYPE_LSM_DEINIT)
			break;
	}
	if (dm_buff_fill) {
		if (dm->data_buf && (diagmem->p_hdlc_disabled[PERIPHERAL_APPS] || vm_enabled)) {
			diag_mux_write(NULL, 0, PERIPHERAL_APPS, dm->data_buf, dm->data_bytes_read, dm->flow, 0);
			memset(dm->data_buf, 0, dm->data_bytes_read);
			dm->data_bytes_read = 0;
			dm->copy_count = 0;
		} else {
			diag_flush_aggregate_buf(dm);
		}
	}
	return 0;
}

/*
 * pcie_dm_recv()
 * Removing pcie device fd will fail diag over pcie functionality.
 * Ensure to return 0 to avoid removing fd for MHI pipe from read watches.
 */
int pcie_dm_recv(int fd, void* data)
{
	dm_recv(fd, data);
	return 0;
}

/**
 * dm_recv() - read and handle data from a DM
 * @fd:		the file descriptor associated with the DM
 * @data:	private data, must be a diag_client object
 */
int dm_recv(int fd, void* data)
{
	struct diag_client *dm = (struct diag_client *)data;
	(void)fd;

	if (dm->hdlc_encoded)
		return dm_recv_hdlc(dm);
	else
		return dm_recv_raw(dm);
}

ssize_t dm_send_flow(struct diag_client *unix_dm, struct diag_client *sink_dm, const void *ptr, size_t len,
			    struct watch_flow *flow, int cmd_rsp_flag)
{
	if (!sink_dm)
		return 0;
	if (!sink_dm->enabled)
  		return 0;

	if (cmd_rsp_flag) {
		if (sink_dm->hdlc_encoded)
			hdlc_enqueue_flow(NULL, &sink_dm->cmd_rsp_q, ptr, len, flow);
		else
			queue_push_flow(NULL, &sink_dm->cmd_rsp_q, ptr, len, flow);
	} else {
		if (sink_dm->hdlc_encoded)
			hdlc_enqueue_flow(unix_dm, &sink_dm->outq, ptr, len, flow);
		else
			queue_push_flow(unix_dm, &sink_dm->outq, ptr, len, flow);
	}

	return 0;
}

/**
 * dm_send() - enqueue message to DM
 * @dm:		dm to be receiving the message
 * @ptr:	pointer to raw message to be sent
 * @len:	length of message
 */
ssize_t dm_send(struct diag_client *dm, const void *ptr, size_t len)
{

	return dm_send_flow(NULL, dm, ptr, len, NULL, 0);
}

/**
 * dm_broadcast() - send message to all registered DMs
 * @ptr:	pointer to raw message to be sent
 * @len:	length of message
 */
void dm_broadcast(const void *ptr, size_t len, struct watch_flow *flow, int cmd_rsp_flag)
{
	struct diag_client *dm;
	struct list_head *item;

	list_for_each_with_lock(item, &diag_clients) {
		dm = container_of(item, struct diag_client, node);
		if (!strcmp(dm->name, "USB client") || !strcmp(dm->name, "Socket Client"))
			dm_send_flow(NULL, dm, ptr, len, flow, cmd_rsp_flag);
	}
	LIST_UNLOCK(&diag_clients);/*lock is taken in list_for_each_safe_with_lock */
}

void dm_broadcast_to_socket_clients(const void *ptr, size_t len)
{
	struct diag_client *dm;
	struct list_head *item;
	if (!ptr)
                return;

	list_for_each_with_lock(item, &diag_sock_clients) {
		dm = container_of(item, struct diag_client, node);
		dm_send(dm, ptr, len);
	}
	LIST_UNLOCK(&diag_sock_clients);/*lock is taken in list_for_each_safe_with_lock */
}

void dm_send_to_pid(const void *ptr, size_t len, int pid)
{
	struct diag_client *dm = NULL;
	struct list_head *item;

	if (!ptr)
                return;

	list_for_each_with_lock(item, &diag_sock_clients) {
		dm = container_of(item, struct diag_client, node);
		if (dm && dm->pid == pid) {
			dm_send(dm, ptr, len);
			break;
		}
	}
	LIST_UNLOCK(&diag_sock_clients);/*lock is taken in list_for_each_safe_with_lock */
}

void dm_update_diag_id(struct diag_client *dm)
{
	struct diag_id_notify_pkt pkt;

	pkt.type = DIAG_ID_TYPE;
	pkt.diag_id = diagmem->diag_id;
	dm_send(dm, &pkt, sizeof(pkt));
}

void dm_set_pid(struct diag_client *dm, int pid)
{
	dm->pid = pid;
}

void dm_set_override_pid(struct diag_client *dm, int pid)
{
	dm->override_pid = pid;
}

void dm_enable(struct diag_client *dm)
{
	dm->enabled = true;
}

void dm_disable(struct diag_client *dm)
{
	dm->enabled = false;
}

int dm_query_status(struct diag_client *dm)
{
	return dm->enabled;
}
