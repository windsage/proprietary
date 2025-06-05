/* Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *
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

#include <sys/ioctl.h>
#include <sys/types.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "diag.h"
#include "diag_cntl.h"
#include "dm.h"
#include "hdlc.h"
#include "list.h"
#include "peripheral.h"
#include "util.h"
#include "watch.h"
#include "diag_mux.h"

#define NUM_GLINKPKT_PERIPHERALS 2
#define GLINKPKT_PARTIAL_PKT_SIZE 16384
#define GLINKPKT_FULL_PKT_SIZE 32768
#define OPEN_ERROR 0
#define PERIF_ERROR 1
#define NHDLC_HDR_DELIM_SIZE 5

pthread_t glinkpkt_open_thread;
pthread_t glinkpkt_ssr_thread;

struct glinkpkt_peripheral {
	struct peripheral perif;
	char *partial_pkt;
	int partial_len;
	char *full_pkt;
};

#define to_glinkpkt_peripheral(p) container_of(p, struct glinkpkt_peripheral, perif)

struct glinkpkt_config {
	int peripheral_id;
	char *cntl;
	char *cmd;
	char *data;
};

struct glinkpkt_config glinkpkt_configs[NUM_GLINKPKT_PERIPHERALS] = {
	{
		.peripheral_id = PERIPHERAL_SLATE_APPS,
		.cntl = "/dev/slate_apps_cntl",
		.cmd = "/dev/slate_apps_cmd",
		.data = "/dev/slate_apps_data",
	},
	{
		.peripheral_id = PERIPHERAL_SLATE_ADSP,
		.cntl = "/dev/slate_adsp_cntl",
		.cmd = "/dev/slate_adsp_cmd",
		.data = "/dev/slate_adsp_data",
	}
};

struct non_hdlc_pkt {
	uint8_t start;
	uint8_t version;
	uint16_t length;
	char payload[];
};

struct glinkpkt_watch {
	int fd;
	int type;
	void *data;
	struct list_head node;
};

static struct list_head glinkpkt_fds = LIST_INIT(glinkpkt_fds);
void glinkpkt_handle_eventfd(int evfd);

int glinkpkt_add_readfd(int fd, int type, void *data)
{
	struct glinkpkt_watch *gw;

	gw = calloc(1, sizeof(struct glinkpkt_watch));
	if (!gw) {
		ALOGE("diag: received NULL glinkpkt watch for fd: %d\n", fd);
		return -ENOMEM;
	}

	gw->fd = fd;
	gw->type = type;
	gw->data = data;
	list_add(&glinkpkt_fds, &gw->node);

	return 0;
}

void glinkpkt_remove_fd(int fd)
{
	struct list_head *item;
	struct list_head *next;
	struct glinkpkt_watch *gw;

	list_for_each_safe(item, next, &glinkpkt_fds) {
		gw = container_of(item, struct glinkpkt_watch, node);
		if (gw->fd == fd) {
			list_del(&gw->node);
			free(gw);
			gw = NULL;
		}
	}
}


static int glinkpkt_cntl_recv(int fd, void *data)
{
	struct peripheral *perif = NULL;
	struct circ_buf *recv_buf = NULL;
	ssize_t len;

	if (!data)
		return -EINVAL;

	perif = (struct peripheral *)data;
	recv_buf = &perif->recv_buf;

	if (!perif->cntl_open) {
		perif->cntl_open = true;
		watch_add_writeq(perif->cntl_fd, &perif->cntlq, NULL, CNTL_FD);
		diag_notify_md_client(DIAG_LOCAL_PROC, perif->periph_id, DIAG_STATUS_OPEN);
	}

	len = read(fd, recv_buf->buf, sizeof(recv_buf->buf));
	if (len < 0) {
		if (errno == -ENETRESET) {
			ALOGE("diag: failed to read from cntl channel for fd: %d\n", fd);
			glinkpkt_handle_eventfd(fd);
		}
		return errno;
	}

	return diag_cntl_recv(perif, recv_buf->buf, len);
}

static int glinkpkt_cmd_recv(int fd, void *data)
{
	struct peripheral *perif = NULL;
	struct non_hdlc_pkt *frame;
	struct circ_buf *recv_buf = NULL;
	ssize_t len;
	int read_len = 0;

	if (!data)
		return -EINVAL;

	perif = (struct peripheral *)data;
	recv_buf = &perif->recv_buf;

	len = read(fd, recv_buf->buf, sizeof(recv_buf->buf));
	if (len < 0) {
		if (errno == -ENETRESET) {
			ALOGE("diag: failed to read from cmd channel for fd: %d\n", fd);
			glinkpkt_handle_eventfd(fd);
		}
		return errno;
	}

	while (read_len < len) {
		frame = (struct non_hdlc_pkt *)(recv_buf->buf + read_len);
		read_len += frame->length + 5;
		if (frame->start != 0x7e || frame->version != 1) {
			fprintf(stderr, "invalid non-HDLC frame\n");
			break;
		}
		if (sizeof(*frame) + frame->length + 1 > len) {
			fprintf(stderr, "truncated non-HDLC frame\n");
			break;
		}
		if (frame->payload[frame->length] != 0x7e) {
			fprintf(stderr, "non-HDLC frame is not truncated\n");
			break;
		}
		diag_mux_write(NULL, 0, perif->periph_id, (unsigned char*)frame->payload, frame->length, perif->flow, 1);
	}
	return 0;
}

void copy_partial_pkt(struct glinkpkt_peripheral *gperif, struct non_hdlc_pkt *frame)
{
	if (!gperif || !frame)
		return;

	if (gperif->partial_len < GLINKPKT_PARTIAL_PKT_SIZE)
		memcpy(gperif->partial_pkt, frame, gperif->partial_len);
	else
		gperif->partial_len = 0;
}

static int glinkpkt_data_recv(int fd, void *data)
{
	struct peripheral *perif = NULL;
	struct glinkpkt_peripheral *gperif = NULL;
	struct non_hdlc_pkt *frame;
	struct circ_buf *recv_buf = NULL;
	ssize_t len;
	int full_pkt_len = 0;
	int read_len = 0;

	if (!data)
		return -EINVAL;

	perif = (struct peripheral *)data;
	recv_buf = &perif->recv_buf;
	if (!recv_buf)
		return 0;

	gperif = to_glinkpkt_peripheral(perif);

	if (!perif->data_open) {
		perif->data_open = true;
		watch_add_writeq(perif->data_fd, &perif->dataq, NULL, DATA_FD);
	}

	len = read(fd, recv_buf->buf, sizeof(recv_buf->buf));
	if (len < 0) {
		if (errno == -ENETRESET) {
			ALOGE("diag: failed to read from data channel for fd: %d\n", fd);
			glinkpkt_handle_eventfd(fd);
		}
		return errno;
	}

	memset(gperif->full_pkt, 0, GLINKPKT_FULL_PKT_SIZE);

	if (gperif->partial_len) {
		full_pkt_len = gperif->partial_len + len;
		memcpy(gperif->full_pkt, gperif->partial_pkt, gperif->partial_len);
		memcpy(gperif->full_pkt + gperif->partial_len, recv_buf->buf, len);

		memset(gperif->partial_pkt, 0, GLINKPKT_PARTIAL_PKT_SIZE);
		gperif->partial_len = 0;
	} else {
		frame = recv_buf->buf;
		if (frame->start != 0x7e || frame->version != 1 || frame->length > (GLINKPKT_FULL_PKT_SIZE - NHDLC_HDR_DELIM_SIZE)){
			ALOGE("diag:%s invalid frame length : %d\n", __func__, frame->length);
			return 0;
		}
		full_pkt_len = len;
		memcpy(gperif->full_pkt, recv_buf->buf, len);
	}

	while (read_len < full_pkt_len) {
		frame = (struct non_hdlc_pkt *)(gperif->full_pkt + read_len);
		if (frame->start != 0x7e || frame->version != 1) {
			gperif->partial_len = full_pkt_len - read_len;
			copy_partial_pkt(gperif, frame);
			ALOGE("diag: %s: invalid non-HDLC frame\n", __func__);
			break;
		}
		if (sizeof(*frame) + frame->length + 1 > len) {
			gperif->partial_len = full_pkt_len - read_len;
			copy_partial_pkt(gperif, frame);
			ALOGE("diag: %s: truncated non-HDLC frame\n", __func__);
			break;
		}
		if (frame->payload[frame->length] != 0x7e) {
			gperif->partial_len = full_pkt_len - read_len;
			copy_partial_pkt(gperif, frame);
			ALOGE("diag: %s: non-HDLC frame is not truncated\n", __func__);
			break;
		}
		read_len += frame->length + 5;
		diag_mux_write(NULL, 0, perif->periph_id, (unsigned char*)frame->payload, frame->length, perif->flow, 0);
	}
	return 0;
}

static int glinkpkt_perif_send(struct peripheral *perif, const void *ptr, size_t len)
{
	if (!perif->cmd_open)
		return -ENOENT;

	if (perif->features & DIAG_FEATURE_APPS_HDLC_ENCODE)
		write(perif->cmd_fd, ptr, len);
	else
		hdlc_enqueue(&perif->cmdq, ptr, len);

	return 0;
}

static void glinkpkt_perif_close(struct peripheral *perif)
{
}

struct glinkpkt_config *glinkpkt_get_config(int peripheral)
{
	int i;

	for (i = 0; i < NUM_GLINKPKT_PERIPHERALS; i++) {
		if (peripheral == glinkpkt_configs[i].peripheral_id)
			return &glinkpkt_configs[i];

        }
	return NULL;
}

static void glinkpkt_close(struct glinkpkt_peripheral *gperif, int type)
{
	struct glinkpkt_config *config;
	int fd;

	config = glinkpkt_get_config(gperif->perif.periph_id);
	if (!config)
		return;

	ALOGE("diag: SSR underway for peripheral: %s of channel type: %d\n", gperif->perif.name, type);
	switch (type) {
	case TYPE_CNTL:
		diag_cntl_close(&gperif->perif);
		diag_notify_md_client(DIAG_LOCAL_PROC, gperif->perif.periph_id, DIAG_STATUS_CLOSED);

		fd = gperif->perif.cntl_fd;
		gperif->perif.cntl_open = false;
		break;
	case TYPE_CMD:
		fd = gperif->perif.cmd_fd;
		gperif->perif.cmd_open = false;
		break;
	case TYPE_DATA:
		fd = gperif->perif.data_fd;
		gperif->perif.data_open = false;
		break;
	default:
		ALOGE("diag: %s invalid channel type for peripheral: %s\n", __func__, gperif->perif.name);
		return;
	}
	watch_remove_writeq(fd);
	watch_remove_fd(fd);
	glinkpkt_remove_fd(fd);
	close(fd);
}

static int glinkpkt_open(struct glinkpkt_peripheral *gperif, int type)
{
	struct glinkpkt_config *config;
	int ret;

	config = glinkpkt_get_config(gperif->perif.periph_id);
	if (!config)
		return -EINVAL;

	switch (type) {
	case TYPE_CNTL:
		ret = open(config->cntl, O_RDWR);
		if (ret < 0) {
			ALOGE("diag: %s: failed to create control node for %s\n", __func__, gperif->perif.name);
			goto open_err;
		}
		gperif->perif.cntl_fd = ret;

		watch_add_readfd(gperif->perif.cntl_fd, glinkpkt_cntl_recv, &gperif->perif, NULL);
		break;
	case TYPE_CMD:
		ret = open(config->cmd, O_RDWR);
		if (ret < 0) {
			ALOGE("diag: %s: failed to create command node for %s\n", __func__, gperif->perif.name);
			goto open_err;
		}
		gperif->perif.cmd_fd = ret;

		watch_add_readfd(gperif->perif.cmd_fd, glinkpkt_cmd_recv, &gperif->perif, NULL);
		gperif->perif.cmd_open = true;
		watch_add_writeq(gperif->perif.cmd_fd, &gperif->perif.cmdq, NULL, 0);
		break;
	case TYPE_DATA:
		ret = open(config->data, O_RDWR);
		if (ret < 0) {
			ALOGE("diag: %s: failed to create data node for %s\n", __func__, gperif->perif.name);
			goto open_err;
		}
		gperif->perif.data_fd = ret;

		watch_add_readfd(gperif->perif.data_fd, glinkpkt_data_recv, &gperif->perif, gperif->perif.flow);
		break;
	default:
		ALOGE("diag: %s: invalid channel type: %d to open for peripheral: %s\n", __func__, type, gperif->perif.name);
		return -EINVAL;
	}
	glinkpkt_add_readfd(ret, type, gperif);
open_err:
	return ret;
}

static void glinkpkt_perif_cleanup(struct glinkpkt_peripheral *gperif, int err_type)
{
	switch(err_type) {
	case OPEN_ERROR:
		free(gperif->partial_pkt);
		free(gperif->full_pkt);
	case PERIF_ERROR:
		free(gperif);
		break;
	default:
		break;
	}
}

static void *diag_glinkpkt_ssr_thread(void *arg)
{
	struct glinkpkt_watch *gw = NULL;
	struct glinkpkt_peripheral *gperif = NULL;
	int ret;

	if (!arg)
		goto err;

	gw = (struct glinkpkt_watch *)arg;
	gperif = (struct glinkpkt_peripheral *)gw->data;

	ALOGE("diag: %s: open for perif: %s type: %d\n", __func__, gperif->perif.name, gw->type);
	do {
		ret = glinkpkt_open(gperif, gw->type);
		sleep(1);
	} while(ret < 0);

	free(gw);
err:
	pthread_detach(pthread_self());
	return NULL;
}

static void *diag_glinkpkt_open_thread(void *arg)
{
	struct glinkpkt_peripheral *gperif = NULL;
	int ret, retry_cnt = 0;

	gperif = (struct glinkpkt_peripheral *)arg;

	ret = glinkpkt_open(gperif, TYPE_CNTL);
	while((ret < 0 && (retry_cnt < 20))) {
		sleep(5);
		ret = glinkpkt_open(gperif, TYPE_CNTL);
		ALOGE("diag: %s: Glinkpkt_open failed for perif: %d with err: %d, retry_cnt: %d\n",
				__func__, gperif->perif.periph_id, ret, retry_cnt++);
	}

	if (ret < 0)
		goto open_err;

	ret = glinkpkt_open(gperif, TYPE_DATA);
	if (ret < 0)
		goto open_err_data;

	ret = glinkpkt_open(gperif, TYPE_CMD);
	if (ret < 0)
		goto open_err_cmd;

	list_add(&peripherals, &gperif->perif.node);
	pthread_detach(pthread_self());
	return NULL;

open_err_cmd:
	glinkpkt_close(gperif, TYPE_DATA);
open_err_data:
	glinkpkt_close(gperif, TYPE_CNTL);
open_err:
	glinkpkt_perif_cleanup(gperif, OPEN_ERROR);

	pthread_detach(pthread_self());
	return NULL;
}

void glinkpkt_handle_eventfd(int evfd)
{
	struct list_head *item;
	struct list_head *next;
	struct glinkpkt_watch *gw, *gw_ssr = NULL;
	struct glinkpkt_peripheral *gperif = NULL;

	list_for_each_safe_with_lock(item, next, &glinkpkt_fds) {
		gw = container_of(item, struct glinkpkt_watch, node);
		if (gw->fd == evfd) {
			gw_ssr = (struct glinkpkt_watch*) calloc(1, sizeof(struct glinkpkt_watch));

			if(!gw_ssr) {
				glinkpkt_close(gw->data, gw->type);
				LIST_UNLOCK(&glinkpkt_fds);
				return;
			}

			gw_ssr->data = gw->data;
			gw_ssr->type = gw->type;

			/*
			 * gw is freed in cleanup path but gw struct content is used as argument
			 * while spawning SSR thread. Hence allocate and initialize similar
			 * structure (gw_ssr) with gw struct values.
			 * data variable in watch structure points to global glinkpkt perif
			 */

			glinkpkt_close(gw->data, gw->type);

			pthread_create(&glinkpkt_ssr_thread, NULL, diag_glinkpkt_ssr_thread, gw_ssr);
			if (glinkpkt_ssr_thread == 0) {
				if (gw_ssr->data) {
					gperif = (struct glinkpkt_peripheral *)gw_ssr->data;
					ALOGE("diag: Unable to create glinkpkt ssr thread for %s\n", gperif->perif.name);
					glinkpkt_perif_cleanup(gperif, PERIF_ERROR);
				}
				LIST_UNLOCK(&glinkpkt_fds);
				return;
			}
		}
	}
	LIST_UNLOCK(&glinkpkt_fds);
}

static int glinkpkt_perif_init_subsystem(const char *name, int id)
{
	struct glinkpkt_peripheral *gperif;
	const char *cntl_node;
	const char *cmd_node;
	const char *data_node;
	int i;
	int ret;

	if (!name) {
		ret = -EINVAL;
		goto err;
	}

	gperif = calloc(1, sizeof(*gperif));
	if (!gperif) {
		ret = -ENOMEM;
		goto err;
	}

	gperif->perif.flow = watch_flow_new();
	gperif->perif.name = strdup(name);
	if (!gperif->perif.name) {
		ret = -EINVAL;
		goto gperif_err;
	}

	gperif->perif.periph_id = id;
	gperif->perif.send = glinkpkt_perif_send;
	gperif->perif.close = glinkpkt_perif_close;
	gperif->perif.sockets = false;
	gperif->perif.glinkpkt = true;
	diagmem->buffering_mode[id].mode = DIAG_BUFFERING_MODE_STREAMING;
	diagmem->buffering_mode[id].peripheral = id;
	diagmem->buffering_mode[id].high_wm_val = DEFAULT_HIGH_WM_VAL;
	diagmem->buffering_mode[id].low_wm_val = DEFAULT_LOW_WM_VAL;
	diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][id].mode = DIAG_BUFFERING_MODE_STREAMING;
	diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][id].peripheral = id;
	diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][id].high_wm_val = DEFAULT_HIGH_WM_VAL;
	diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][id].low_wm_val = DEFAULT_LOW_WM_VAL;
	gperif->perif.received_feature_mask = 0;
	gperif->perif.sent_feature_mask = 0;
	gperif->perif.socket_node = -1;

	gperif->partial_pkt = (char *)calloc(1, GLINKPKT_PARTIAL_PKT_SIZE);
	if (!gperif->partial_pkt) {
		ret = -ENOMEM;
		goto gperif_err;
	}

	gperif->partial_len = 0;

	gperif->full_pkt = (char *)calloc(1, GLINKPKT_FULL_PKT_SIZE);
	if (!gperif->full_pkt) {
		free(gperif->partial_pkt);
		ret = -ENOMEM;
		goto gperif_err;
	}

	list_init(&gperif->perif.cmdq);
	list_init(&gperif->perif.cntlq);
	list_init(&gperif->perif.dataq);

	pthread_create(&glinkpkt_open_thread, NULL, diag_glinkpkt_open_thread, gperif);
	if (glinkpkt_open_thread == 0) {
		ALOGE("diag: Unable to create glinkpkt open thread\n");
		free(gperif->full_pkt);
		free(gperif->partial_pkt);
		ret = -EINVAL;
		goto gperif_err;
	}

	return 0;

gperif_err:
	if(gperif) {
		if(gperif->perif.name)
			free(gperif->perif.name);
		watch_flow_put(gperif->perif.flow);
		glinkpkt_perif_cleanup(gperif, PERIF_ERROR);
        }
err:
	return ret;
}

int peripheral_glinkpkt_init(void)
{
	glinkpkt_perif_init_subsystem("slate_apps", PERIPHERAL_SLATE_APPS);
	glinkpkt_perif_init_subsystem("slate_adsp", PERIPHERAL_SLATE_ADSP);
	return 0;
}

