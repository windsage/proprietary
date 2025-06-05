/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "diag.h"
#include "hdlc.h"
#include "masks.h"
#include "mbuf.h"
#include "peripheral.h"
#include "util.h"
#include "watch.h"
#include "diag_hidl_service.h"
#include "diag_hidl_wrapper.h"
#include "diag_mhi.h"
#include "diag_mux.h"
#include "diag_cntl.h"
#include <pthread.h>
#ifndef FEATURE_LE_DIAG
#include "qdss_config.h"
#endif

#define SET_APPS_FEATURE(diagmem, n) ((diagmem->apps_feature) |= (1 << (n)))

#define F_DIAG_EVENT_REPORT		0
#define F_DIAG_HW_ACCELERATION		1
#define F_DIAG_MULTI_SIM_MASK		2
#define F_DIAG_DIAGID_BASED_CMD_PKT	3
#define F_DIAG_DIAGID_BASED_ASYNC_PKT	5

struct diag_driver *diagmem = NULL;
struct list_head diag_cmds = LIST_INIT(diag_cmds);
struct diag_pkt_format_selection pkt_format_select_req;
pthread_mutex_t write_mutex;
int hdlc_reset_timer_in_progress = 0;
static int hdlc_pid;
unsigned int diag_debug_mask = DIAG_DBG_MASK_INFO;
unsigned int vm_enabled;

void queue_push_flow(struct diag_client *dm, struct list_head *queue, const void *msg, size_t msglen, struct watch_flow *flow)
{
	struct mbuf *mbuf = NULL;
	void *ptr;
	int i;

	if (dm && dm->use_mbuf) {
		for (i = 0; i < UNIX_CLIENT_BUF_COUNT; i++) {
			if (dm->dm_mbuf[i] && !dm->dm_mbuf[i]->busy_state) {
				mbuf = dm->dm_mbuf[i];
				break;
			}
		}
		if (!mbuf) {
			dm->drop_count += dm->copy_count;
			return;
		}
		mbuf->busy_state = 1;
		mbuf->size = msglen;
	} else {
		mbuf = mbuf_alloc(msglen);
		if (!mbuf) {
			ALOGE("diag: %s: failed to allocate memory\n", __func__);
			return;
		}
		mbuf->free_flag = 0;
		list_init(&mbuf->node);
	}

	ptr = mbuf_put(mbuf, msglen);
	if (!ptr) {
		ALOGE("diag: %s: invalid ptr, dropping pkt of len: %zu\n", __func__, msglen);
		if (mbuf->free_flag)
			return;
		free(mbuf);
		mbuf = NULL;
		return;
	}
	memcpy(ptr, msg, msglen);
	mbuf->flow = watch_flow_get(flow);
	watch_flow_inc(flow);
	list_add(queue, &mbuf->node);
}
void queue_push(struct diag_client *dm, struct list_head *queue, const void *msg, size_t msglen)
{
	queue_push_flow(dm, queue, msg, msglen, NULL);
}

void queue_push_cntlq(struct peripheral *peripheral, const void *msg,
			size_t msglen)
{
	if (!peripheral)
		return;

	if ((msglen > sizeof(struct diag_cntl_hdr)) && (msglen < MAX_CNTL_CHNL_BUF_SIZE)) {
		ALOGM(DIAG_DBG_MASK_INFO, "diag: %s: Sending cntl pkt of len: %zu to perif: %s with port: node [%x: %x]\n", __func__, msglen, peripheral->name, peripheral->cntl_ch.port, peripheral->cntl_ch.node);
		if (peripheral->glinkpkt)
			write(peripheral->cntl_fd, msg, msglen);
		else
			queue_push_flow(NULL, &peripheral->cntlq, msg, msglen, NULL);
	} else {
		ALOGI("diag: %s: Skip sending pkt of len(%zu) to cntl chnl\n", __func__, msglen);
	}
}

static void usage(void)
{
	fprintf(stderr,
		"User space application for diag interface\n"
		"\n"
		"usage: diag [-hsud]\n"
		"\n"
		"options:\n"
		"   -h   show this usage\n"
		"   -s   <socket address[:port]>\n"
		"   -u   <uart device name[@baudrate]>\n"
		"   -d   <debug level mask>\n"
	);

	exit(1);
}

static void diag_apps_feature_init(void)
{

	diagmem->apps_feature = 0;
	SET_APPS_FEATURE(diagmem, F_DIAG_EVENT_REPORT);
	SET_APPS_FEATURE(diagmem, F_DIAG_HW_ACCELERATION);
	SET_APPS_FEATURE(diagmem, F_DIAG_MULTI_SIM_MASK);
#ifdef DIAG_ID_BASED_PKT_ENABLED
	/**
	 * share diag-id based pkt feature mask with tool only if
	 * the target support this feature
	 */
	SET_APPS_FEATURE(diagmem, F_DIAG_DIAGID_BASED_CMD_PKT);
	SET_APPS_FEATURE(diagmem, F_DIAG_DIAGID_BASED_ASYNC_PKT);
#endif /* DIAG_ID_BASED_PKT_ENABLED */
}

void diag_notify_md_client(uint8_t proc, uint8_t peripheral, int data)
{
	union sigval value;
	int pid, ret = 0;
	errno = 0;

	if ((diagmem->logging_mode[proc] != DIAG_MEMORY_DEVICE_MODE) &&
		(diagmem->logging_mode[proc] != DIAG_MULTI_MODE))
		return;
	if (!diagmem->md_session_map[proc][peripheral] ||
		diagmem->md_session_map[proc][peripheral]->pid <= 0) {
		printf("diag: md_session_map[%d] is invalid\n", peripheral);
		return;
	}
	pid = diagmem->md_session_map[proc][peripheral]->pid;
	value.sival_int = 0;
	value.sival_int = value.sival_int | (DIAG_GET_MD_DEVICE_SIG_MASK(proc) | data);
	if (proc == DIAG_LOCAL_PROC)
		value.sival_int = value.sival_int | (PERIPHERAL_MASK(peripheral) | data);
	ret = sigqueue(pid, SIGCONT, value);
	if (ret)
		ALOGE("diag: In %s: sigqueue failed ret: %d errno: %d\n", __func__, ret, errno);
}

int diag_md_init(void)
{
	int proc, i;

	for (proc = 0; proc < NUM_DIAG_MD_DEV; proc++)
		diagmem->logging_mode[proc] = DIAG_USB_MODE;
	for (i = 0; i < NUM_UPD; i++) {
		diagmem->pd_logging_mode[i] = 0;
		diagmem->pd_session_clear[i] = 0;
	}
	for (proc = 0; proc < NUM_DIAG_MD_DEV; proc++) {
		diagmem->md_session_mask[proc] = 0;
		diagmem->md_session_mode[proc] = DIAG_MD_NONE;
		for (i = 0; i < NUM_MD_SESSIONS; i++)
			diagmem->md_session_map[proc][i] = NULL;
	}
	return 0;
}
static int diag_real_time_info_init(void)
{
	int i;

	for (i = 0; i < DIAG_NUM_PROC; i++) {
		diagmem->real_time_mode[i] = 1;
		diagmem->proc_rt_vote_mask[i] |= DIAG_PROC_VOTE_REALTIME_DCI;
		diagmem->proc_rt_vote_mask[i] |= DIAG_PROC_VOTE_REALTIME_MEMORY_DEVICE;
	}
	diagmem->proc_active_mask = 0;

	return 0;
}

int diag_get_remote_mask_info()
{
	return diag_get_mhi_remote_mask();
}

void diag_hdlc_reset(void *data)
{
	struct diag_md_session_t *session_info = NULL;
	int i;
	(void)data;

	diagmem->hdlc_disabled = 0;
	for (i = 0; i < NUM_MD_SESSIONS; i++) {
		session_info = diag_md_session_get_peripheral(DIAG_LOCAL_PROC,
								i);
		if (!session_info)
			diagmem->p_hdlc_disabled[i] =
				diagmem->hdlc_disabled;
	}
	hdlc_reset_timer_in_progress = 0;
}

void diag_md_hdlc_reset(void *data)
{
	int peripheral = -EINVAL, i = 0;
	struct diag_md_session_t *session_info = NULL;
	int pid = *(int *) data;

	session_info = diag_md_session_get_pid(pid);
	if (session_info)
		session_info->hdlc_disabled = 0;
	peripheral =
		diag_md_session_match_pid_peripheral(DIAG_LOCAL_PROC,
							pid, 0);
	if (peripheral > 0 && session_info) {
		for (i = 0; i < NUM_MD_SESSIONS; i++) {
			if (peripheral & (1 << i))
				diagmem->p_hdlc_disabled[i] =
				session_info->hdlc_disabled;
		}
	}
	hdlc_reset_timer_in_progress = 0;
}

int diag_start_hdlc_recovery(int pid, size_t msglen)
{
	static uint32_t bad_byte_counter;
	static uint32_t bad_cmd_counter;
	int i, peripheral = -EINVAL;
	struct diag_md_session_t *info = NULL;
	hdlc_pid = pid;
	info = diag_md_session_get_pid(pid);

	if (!hdlc_reset_timer_in_progress)
	{
		if (info)
			watch_add_timer(diag_md_hdlc_reset, &hdlc_pid, 200, 0);
		else
			watch_add_timer(diag_hdlc_reset, &hdlc_pid, 200, 0);
		hdlc_reset_timer_in_progress = 1;
	}
	bad_byte_counter += msglen;
	bad_cmd_counter++;
	if (bad_cmd_counter > DIAG_MAX_BAD_CMD) {
		bad_byte_counter = 0;
		bad_cmd_counter = 0;
		ALOGE("diag: In %s, re-enabling HDLC encoding\n",
			__func__);
		pthread_mutex_lock(&diagmem->hdlc_disable_mutex);
		info = diag_md_session_get_pid(pid);
		if (info)
			info->hdlc_disabled = 0;
		else
			diagmem->hdlc_disabled = 0;

		peripheral =
			diag_md_session_match_pid_peripheral(
							DIAG_LOCAL_PROC,
							pid, 0);
		for (i = 0; i < NUM_MD_SESSIONS; i++) {
			if (peripheral > 0 && info) {
				if (peripheral & (1 << i))
					diagmem->p_hdlc_disabled[i] =
						info->hdlc_disabled;
				else if (!diag_md_session_get_peripheral(DIAG_LOCAL_PROC, i))
						diagmem->p_hdlc_disabled[i] =
							diagmem->hdlc_disabled;
			} else {
				if (!diag_md_session_get_peripheral(
						DIAG_LOCAL_PROC, i))
					diagmem->p_hdlc_disabled[i] =
						diagmem->hdlc_disabled;
			}
		}
		pthread_mutex_unlock(&diagmem->hdlc_disable_mutex);
	}

	return 0;
}
void diag_process_apps_data(void *src_buf, int src_len, void *dest_buf, int dest_len, int cmd_rsp_flag)
{
	struct diag_pkt_frame_t header;
	int len = 0;
	(void)dest_len;

	if (diagmem->hdlc_disabled) {
		memset(&header, 0, sizeof(header));
		header.start = CONTROL_CHAR;
		header.version = 1;
		header.length = src_len;
		memcpy(dest_buf, &header, sizeof(header));
		len = len + sizeof(header);
		memcpy(dest_buf + len, src_buf, src_len);
		len = len + src_len;
		*(uint8_t *)(dest_buf + len) = CONTROL_CHAR;
		len += sizeof(uint8_t);
		diag_mux_write(NULL, 0, PERIPHERAL_APPS, dest_buf, len, NULL, cmd_rsp_flag);

	} else {
		diag_mux_write(NULL, 0, PERIPHERAL_APPS, src_buf, src_len, NULL, cmd_rsp_flag);
	}
}

/**
 * save packet format select request mask received from tool
 * to configure the diag-id based async packets and cmd req/resp
 */
void pkt_format_select_req_save(uint8_t mask)
{
	pkt_format_select_req.status = true;
	pkt_format_select_req.mask = mask;
}

/* check mask enabled in the packet format select request */
bool pkt_format_select_check_mask(uint8_t mask)
{
	return pkt_format_select_req.status ?
		(pkt_format_select_req.mask & mask) :
		false;
}

/* get mask configured in packet format select request */
uint8_t pkt_format_select_get_mask(void)
{
	return pkt_format_select_req.mask;
}

int main(int argc, char **argv)
{
	char *host_address = NULL;
	int host_port = DEFAULT_SOCKET_PORT;
	char *uartdev = NULL;
	int baudrate = DEFAULT_BAUD_RATE;
	char *token, *saveptr;
	int ret;
	int c, i;
	int diagid_mask = 0;

#ifdef DIAG_VM_ENABLED
	/* set vm_enabled variable to indicate that this diag-router
	 * instance is running on vm
	 */
	vm_enabled = 1;
#endif /* DIAG_VM_ENABLED */

	for (;;) {
		c = getopt(argc, argv, "hd:s:u:");
		if (c < 0)
			break;
		switch (c) {
		case 's':
			host_address = strtok_r(strdup(optarg), ":", &saveptr);
			token = strtok_r(NULL, "", &saveptr);
			if (token)
				host_port = atoi(token);
			break;
		case 'u':
			uartdev = strtok_r(strdup(optarg), "@", &saveptr);
			token = strtok_r(NULL, "", &saveptr);
			if (token)
				baudrate = atoi(token);
			break;
		case 'd':
			diag_debug_mask = atoi(optarg);
			break;
		default:
		case 'h':
			usage();
			break;
		}
	}

	if (host_address) {
		ret = diag_sock_connect(host_address, host_port);
		if (ret < 0)
			err(1, "failed to connect to client");
	} else if (uartdev) {
		ret = diag_uart_open(uartdev, baudrate);
		if (ret < 0)
			errx(1, "failed to open uart\n");
	}
	pthread_mutex_init(&write_mutex, NULL);
	diagmem = calloc(1, sizeof(*diagmem));
        if (!diagmem)
                errx(1, "failed to allocate memory for diagmem\n");
	diag_mhi_init();
	diag_usb_open("/dev/ffs-diag");

	ret = diag_unix_open();
	if (ret < 0)
		errx(1, "failed to create unix socket dm\n");

	ret = peripheral_init();
	if (ret < 0)
		errx(1, "failed to initialize peripherals\n");

	diag_masks_init();
	diagmem->timesync_enabled = 0;
	diagmem->timestamp_switch = 0;
	diagmem->time_api = 0;
	pthread_mutex_init(&diagmem->session_info_mutex, NULL);
	pthread_mutex_init(&diagmem->hdlc_disable_mutex, NULL);
	/**
	 * set default priority diag-id as PVM to route all legacy commands to pvm
	 * in case of any duplicate command entry to avoid backward compatibility issue
	 */
	diagmem->priority_diagid = DIAG_ID_APPS;
	/* disable hdlc encoding for vm by default */
	if (vm_enabled) {
		diag_hdlc_toggle(0, 1);
	} else {
		diagmem->hdlc_disabled = 0;
		/* set diag-id of pvm in diagmem */
		diagmem->diag_id = DIAG_ID_APPS;
	}
	pthread_mutex_init(&diagmem->dci_mutex, NULL);
	diag_md_init();
	diag_mux_init();
	diag_stm_init();
	diag_real_time_info_init();
	diag_apps_feature_init();
#ifndef FEATURE_LE_DIAG
	register_diag_hidl_service();
	create_diag_hidl_thread();
#endif /* FEATURE_LE_DIAG */
	for (i = 0; i < NUM_PERIPHERALS; i++) {
		diagmem->feature[i].diagid_v2_feature_mask = 0;
	}
	diagmem->dci_state = diag_dci_init();
	register_app_cmds();
	register_common_cmds();
#ifndef FEATURE_LE_DIAG
	image_version_init();
	qdss_diag_pkt_hdlr_init();
	hw_accel_diag_pkt_hdlr_init();
#endif
	diagid_mask = (BITMASK_DIAGID_FMASK | BITMASK_HW_ACCEL_STM_V1);
	process_diagid_v2_feature_mask(DIAG_ID_APPS, diagid_mask);

	watch_run();
	free(diagmem);
	diagmem = NULL;
	return 0;
}
