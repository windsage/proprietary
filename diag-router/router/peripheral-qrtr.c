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
#include <libqrtr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "diag.h"
#include "diag_cntl.h"
#include "dm.h"
#include "peripheral-qrtr.h"
#include "watch.h"
#include "util.h"
#include "diag_mux.h"
#include "peripheral.h"
#include "vm.h"

#ifndef INT_MAX
#define INT_MAX  2147483647
#endif

#define DIAG_SERVICE_ID		4097

#define DIAG_INSTANCE_BASE_MODEM	0
#define DIAG_INSTANCE_BASE_LPASS	64
#define DIAG_INSTANCE_BASE_WCNSS	128
#define DIAG_INSTANCE_BASE_SENSORS	192
#define DIAG_INSTANCE_BASE_CDSP		256
#define DIAG_INSTANCE_BASE_WDSP		320
#define DIAG_INSTANCE_BASE_NPU		384
#define DIAG_INSTANCE_BASE_NSP1		448 /* NSP1 and CDSP1 are same*/
#define DIAG_INSTANCE_BASE_GPDSP0	512
#define DIAG_INSTANCE_BASE_GPDSP1	576
#define DIAG_INSTANCE_BASE_HELIOS_M55	640
#define DIAG_INSTANCE_BASE_TELE_GVM	704
#define DIAG_INSTANCE_BASE_FOTA_GVM	768
#define DIAG_INSTANCE_BASE_SOCCP	832

enum {
	DIAG_INSTANCE_CNTL,
	DIAG_INSTANCE_CMD,
	DIAG_INSTANCE_DATA,
	DIAG_INSTANCE_DCI_CMD,
	DIAG_INSTANCE_DCI,
};

static int qrtr_cntl_recv(int fd, void *data)
{
	struct peripheral *perif = data;
	struct sockaddr_qrtr sq;
	struct sockaddr_qrtr cntlsq;
	struct qrtr_packet pkt;
        socklen_t sl, sl2;
	ssize_t n;
	int ret;
	struct circ_buf *buf = &perif->recv_buf;

	memset(&sq, 0, sizeof(sq));

	sl = sizeof(sq);
	n = recvfrom(fd, buf->buf, sizeof(buf->buf), 0, (void *)&sq, &sl);
	if (n < 0) {
		ret = -errno;
		if (ret != -ENETRESET)
			ALOGE("In %s [DIAG-QRTR] recv failed with error: %d for periph: %d\n",__func__, ret, perif->periph_id);
		return ret;
	}

	ret = qrtr_decode(&pkt, buf, n, &sq);
	if (ret < 0) {
		 ALOGE("In %s [DIAG-QRTR] qrtr decode failed: %d for periph: %d\n",__func__, ret, perif->periph_id);
		return ret;
	}

	switch (pkt.type) {
	case QRTR_TYPE_NEW_SERVER:
		if (pkt.node == 0 && pkt.port == 0)
			break;
		if (!perif->cntl_open) {
			/* Reconstruct diag instance ID */
			pkt.instance = ((pkt.instance << 8) | pkt.version);
			ALOGI("diag: %s: Connecting cntl socket with Service[0x%x:0x%x] port[0x%x:0x%x] perif:%s\n",
				__func__, pkt.service, pkt.instance, pkt.node, pkt.port, perif->name);;
			cntlsq.sq_family = AF_QIPCRTR;
			cntlsq.sq_node = pkt.node;
			cntlsq.sq_port = pkt.port;

			ret = connect(perif->cntl_fd, (struct sockaddr *)&cntlsq, sizeof(cntlsq));
			if (ret < 0) {
				ALOGE("diag: failed to connect to [0x%x:0x%x][0x%x:0x%x]\n",
					pkt.service, pkt.instance, pkt.node, pkt.port);
				return -1;
			}
			watch_add_writeq(perif->cntl_fd, &perif->cntlq, NULL, CNTL_FD);
			perif->cntl_open =  true;

			sl2 = sizeof(sq);
			if (getsockname(fd, (void *)&sq, &sl2))
				return -1;
			perif->cntl_ch.node = sq.sq_node;
			perif->cntl_ch.port = sq.sq_port;
			perif->cntl_ch.service = pkt.service;
			perif->cntl_ch.instance = pkt.instance;
			/* save remote socket address to handle further control event from qrtr */
			perif->cntl_ch_remote.node = pkt.node;
			perif->cntl_ch_remote.port = pkt.port;
			ALOGI("diag: %s: CNTL socket [0x%x:0x%x] connected with service [0x%x:0x%x] socket [0x%x:0x%x]\n",
				__func__, sq.sq_node, sq.sq_port, pkt.service, pkt.instance, pkt.node, pkt.port);
			if (vm_enabled)
				diag_vm_send_feature_mask(perif);
		}
		break;
	case QRTR_TYPE_DEL_SERVER:
		ALOGI("diag: %s: Received del server notify on cntl ch of periph:%s\n", __func__, perif->name);
		if (perif->cntl_open) {
			if ((perif->cntl_ch_remote.node == pkt.node) &&
				(perif->cntl_ch_remote.port == pkt.node)) {
			 	/* Reconstruct diag instance ID */
				pkt.instance = ((pkt.instance << 8) | pkt.version);
				diag_cntl_close(perif);
				watch_remove_writeq(perif->cntl_fd);
				perif->cntl_open = false;
				ALOGI("diag: %s: Received del server on [0x%x:0x%x] for perif: %s from service [0x%x:0x%x]\n",
					__func__, perif->cntl_ch.node, perif->cntl_ch.port,
					perif->name, pkt.service, pkt.instance);
			}
		}
		break;
	case QRTR_TYPE_DATA:
		if (!perif->cntl_open) {
			ALOGI("diag: received qrtr new client notify for cntl ch periph:%d\n", perif->periph_id);
			connect(perif->cntl_fd, (struct sockaddr *)&sq, sizeof(sq));
			sl2 = sizeof(sq);
			if (getsockname(fd, (void *)&sq, &sl2))
				return -1;
			perif->cntl_ch.node = sq.sq_node;
			perif->cntl_ch.port = sq.sq_port;
			perif->cntl_ch.service = pkt.service;
			perif->cntl_ch.instance = pkt.instance;
			perif->cntl_open = true;
			perif->socket_node = pkt.node;
			/* save remote socket address to handle further control event from qrtr */
			perif->cntl_ch_remote.node = pkt.node;
			perif->cntl_ch_remote.port = pkt.port;
			watch_add_writeq(perif->cntl_fd, &perif->cntlq, NULL, CNTL_FD);
			diag_notify_md_client(DIAG_LOCAL_PROC, perif->periph_id, DIAG_STATUS_OPEN);
			ALOGI("diag: rcvd qrtr new client on ctrl chnl for perif: %d, node: %x, port: %x, svc: %d, ins: %d\n", perif->periph_id, sq.sq_node, sq.sq_port, pkt.service, pkt.instance);
		}

		return diag_cntl_recv(perif, pkt.data, pkt.data_len);
	case QRTR_TYPE_DEL_CLIENT:
		/* handle del client case if remote peripheral close the connection */
		if (perif->cntl_open) {
			if ((perif->cntl_ch_remote.node == pkt.node) &&
				(perif->cntl_ch_remote.port == pkt.port)) {
				diag_cntl_close(perif);
				watch_remove_writeq(perif->cntl_fd);
				perif->cntl_open = false;
				diag_notify_md_client(DIAG_LOCAL_PROC, perif->periph_id, DIAG_STATUS_CLOSED);
				ALOGI("diag: %s: rcvd qrtr del client on ctrl chnl for perif: %s from [0x%x:0x%x]\n",
					__func__, perif->name, pkt.node, pkt.port);
			}
		}
		break;
	case QRTR_TYPE_BYE:
		if (perif->socket_node == pkt.node) {
			ALOGI("diag: received qrtr bye for cntl ch periph:%d\n", perif->periph_id);
			sl2 = sizeof(sq);
			if (getsockname(fd, (void *)&sq, &sl2))
				return -1;
			diag_cntl_close(perif);
			watch_remove_writeq(perif->cntl_fd);
			perif->cntl_open = false;
			diag_notify_md_client(DIAG_LOCAL_PROC, perif->periph_id, DIAG_STATUS_CLOSED);
			ALOGI("diag: rcvd qrtr bye on ctrl chnl for perif: %d, node: %x, port: %x, svc: %d, ins: %d\n", perif->periph_id, sq.sq_node, sq.sq_port, pkt.service, pkt.instance);
		}
		break;
	default:
		fprintf(stderr, "Unhandled DIAG CNTL message from %d:%d (%d)\n",
			pkt.node, pkt.port, pkt.type);
		break;
	}

	return 0;
}

struct non_hdlc_pkt {
	uint8_t start;
	uint8_t version;
	uint16_t length;
	char payload[];
};

void diag_fwd_data(struct peripheral *perif, int peripheral, char *buf, int len, int is_cmd)
{
	struct non_hdlc_pkt *frame;
	int read_len = 0;

        if (peripheral < NUM_MD_SESSIONS && diagmem->p_hdlc_disabled[peripheral]) {
		if (is_cmd && (diag_debug_mask & DIAG_DBG_MASK_CMD)) {
			ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: NHDLC cmd rsp from perif: %s of len: %d\n",
				__func__, perif->name, len);
			print_hex_dump(__func__, (void *)buf, MIN(len, PRINT_HEX_DUMP_LEN));
		}
		diag_mux_write(NULL, 0, peripheral, buf, len, perif->flow, is_cmd);
		return;
	}

	while (read_len < len) {
		frame = buf + read_len;
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

		if (is_cmd && (diag_debug_mask & DIAG_DBG_MASK_CMD)) {
			ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: cmd rsp from perif: %s of len: %d\n",
				__func__, perif->name, frame->length);
			print_hex_dump(__func__, (void *)frame->payload, MIN(frame->length, PRINT_HEX_DUMP_LEN));
		}
		if (!is_cmd && (diag_debug_mask & DIAG_DBG_MASK_DATA)) {
			ALOGM(DIAG_DBG_MASK_DATA, "diag: %s: data pkt from perif: %s of len: %d\n",
				__func__, perif->name, frame->length);
			print_hex_dump(__func__, (void *)frame->payload, MIN(frame->length, PRINT_HEX_DUMP_LEN));
		}
		diag_mux_write(NULL, 0, peripheral, frame->payload, frame->length, perif->flow, is_cmd);
	}
}

struct tagged_pkt {
	uint8_t diag_id;
	uint8_t padding;
	uint16_t len;
	char payload[];
};

static int qrtr_cmd_recv(int fd, void *data)
{
	struct peripheral *perif = data;
	struct non_hdlc_pkt *frame;
	struct sockaddr_qrtr cmdsq;
	struct sockaddr_qrtr sq;
	struct qrtr_packet pkt;
	struct circ_buf *buf = &perif->recv_buf;
	socklen_t sl, sl2;
	ssize_t n;
	int ret, i;
	int read_len;
	int pd_val = 0;

	memset(&sq, 0, sizeof(sq));

	sl = sizeof(sq);
	n = recvfrom(fd, buf->buf, sizeof(buf->buf), 0, (void *)&sq, &sl);
	if (n < 0) {
		ret = -errno;
		if (ret != -ENETRESET)
			 ALOGE("In %s [DIAG-QRTR] recv failed with error: %d for periph:%d\n",__func__, ret, perif->periph_id);
		return ret;
	}

	ret = qrtr_decode(&pkt, buf->buf, n, &sq);
	if (ret < 0) {
		 ALOGE("In %s [DIAG-QRTR] qrtr decode failed: %d for periph: %d\n",__func__, ret, perif->periph_id);
		return ret;
	}

	switch (pkt.type) {
	case QRTR_TYPE_DEL_CLIENT:
		break;
	case QRTR_TYPE_DATA:
		if (!perif->cmd_open) {
			ALOGI("diag: %s: Connecting client: %s on CMD channel\n", __func__, perif->name);
			connect(perif->cmd_fd, (struct sockaddr *)&sq, sizeof(sq));
			sl2 = sizeof(sq);
			if (getsockname(fd, (void *)&sq, &sl2))
				return -1;

			perif->cmd_ch.node = sq.sq_node;
			perif->cmd_ch.port = sq.sq_port;
			perif->cmd_ch.service = DIAG_SERVICE_ID;
			perif->cmd_ch.instance = perif->base_instance_id + DIAG_INSTANCE_CMD;
			perif->cmd_open = true;
			watch_add_writeq(perif->cmd_fd, &perif->cmdq, NULL, CMD_FD);
			perif->socket_node = sq.sq_node;
			/* save remote socket address to handle further control event from qrtr */
			perif->cmd_ch_remote.node = pkt.node;
			perif->cmd_ch_remote.port = pkt.port;
			ALOGI("diag: %s: Connected client: %s [0x%x:0x%x] on CMD channel SVC[0x%x:0x%x] [0x%x:0x%x]\n",
				__func__, perif->name, pkt.node, pkt.port,
				perif->cmd_ch.service, perif->cmd_ch.instance, sq.sq_node, sq.sq_port);
		}

		if (vm_enabled) {
			ALOGM(DIAG_DBG_MASK_CNTL, "diag: %s: Received CMD 0x%x 0x%x 0x%x 0x%x \n", __func__,
					*((char*)pkt.data), *((char*)pkt.data+1),
					*((char*)pkt.data+2), *((char*)pkt.data+3));
			diag_client_handle_command(NULL, (uint8_t*)pkt.data, pkt.data_len, 0);
			break;
		}

		if (!diagmem->feature[perif->periph_id].untag_header) {
			diag_fwd_data(perif, perif->periph_id, pkt.data, pkt.data_len, 1);
			break;
		}

		read_len = 0;
		while(read_len < pkt.data_len) {
			struct tagged_pkt *tpkt = (char *)pkt.data + read_len;

			if (perif->root_pd.diag_id == tpkt->diag_id) {
				pd_val = perif->root_pd.pd_val;
			} else {
				for (i = 0; i < MAX_PERIPHERAL_UPD; i++) {
					if (perif->upd_info[i].diag_id == tpkt->diag_id) {
						pd_val = perif->upd_info[i].pd_val;
						break;
					}
				}
			}
			if (pd_val)
				diag_fwd_data(perif, pd_val, &tpkt->payload, tpkt->len, 1);
			read_len += sizeof(*tpkt) + tpkt->len;
		}
		break;
	case QRTR_TYPE_NEW_SERVER:
		if (pkt.node == 0 && pkt.port == 0)
			break;
		if (!perif->cmd_open) {
			ALOGI("diag: Connecting CMD socket to %d: %d perif: %d\n", pkt.node, pkt.port, perif->periph_id);
			cmdsq.sq_family = AF_QIPCRTR;
			cmdsq.sq_node = pkt.node;
			cmdsq.sq_port = pkt.port;

			ret = connect(perif->cmd_fd, (struct sockaddr *)&cmdsq, sizeof(cmdsq));
			if (ret < 0)
				err(1, "failed to connect to %d:%d", cmdsq.sq_node, cmdsq.sq_port);
			watch_add_writeq(perif->cmd_fd, &perif->cmdq, NULL, CMD_FD);
			perif->cmd_open = 1;

			sl2 = sizeof(sq);
			if (getsockname(fd, (void *)&sq, &sl2))
				return -1;
			perif->cmd_ch.node = sq.sq_node;
			perif->cmd_ch.port = sq.sq_port;
			perif->cmd_ch.service = pkt.service;
			perif->cmd_ch.instance = pkt.instance;
			ALOGI("diag: rcvd qrtr new server on cmd chnl for perif: %d, node: %x, port: %x, svc: %d, ins: %d\n", perif->periph_id, sq.sq_node, sq.sq_port, pkt.service, pkt.instance);
		}
		break;
	case QRTR_TYPE_DEL_SERVER:
		ALOGI("diag: received del server notify for cmd ch of periph:%d\n", perif->periph_id);
		watch_remove_writeq(perif->cmd_fd);
		perif->cmd_open = 0;
		sl2 = sizeof(sq);
		if (getsockname(fd, (void *)&sq, &sl2))
			return -1;
		ALOGI("diag: rcvd qrtr bye on cmd chnl for perif: %d, node: %x, port: %x, svc: %d, ins: %d\n", perif->periph_id, sq.sq_node, sq.sq_port, pkt.service, pkt.instance);
		break;
	default:
		fprintf(stderr, "Unhandled DIAG CMD message from %d:%d (%d)\n",
			pkt.node, pkt.port, pkt.type);
		break;
	}

	return 0;
}

static int qrtr_data_recv(int fd, void *data)
{
	struct peripheral *perif = NULL;
	struct sockaddr_qrtr datasq;
	struct sockaddr_qrtr sq;
	struct qrtr_packet pkt;
        socklen_t sl, sl2;
	ssize_t n;
	int ret, i;
	int read_len;
	struct non_hdlc_pkt *frame;
	struct circ_buf *buf = NULL;
	int pd_val = 0;

	memset(&sq, 0, sizeof(sq));

	if (!data)
		return -EINVAL;

	perif = (struct peripheral *)data;
	buf = &perif->recv_buf;

	sl = sizeof(sq);
	n = recvfrom(fd, buf->buf, sizeof(buf->buf), 0, (void *)&sq, &sl);
	if (n < 0) {
		ret = -errno;
		if (ret != -ENETRESET)
			fprintf(stderr, "[DIAG-QRTR] recvfrom failed: %d\n", ret);
		return ret;
	}

	ret = qrtr_decode(&pkt, buf, n, &sq);
	if (ret < 0) {
		fprintf(stderr, "[PD-MAPPER] unable to decode qrtr packet\n");
		return ret;
	}

	switch (pkt.type) {
	case QRTR_TYPE_NEW_SERVER:
		if (pkt.node == 0 && pkt.port == 0)
			break;
		if (!perif->data_open) {
			/* Reconstruct diag instance ID */
			pkt.instance = ((pkt.instance << 8) | pkt.version);
			ALOGI("diag: %s: Connecting data socket to Service[0x%x:0x%x] port[0x%x:0x%x] perif:%s\n",
				__func__, pkt.service, pkt.instance, pkt.node, pkt.port, perif->name);
			datasq.sq_family = AF_QIPCRTR;
			datasq.sq_node = pkt.node;
			datasq.sq_port = pkt.port;

			ret = connect(perif->data_fd, (struct sockaddr *)&datasq, sizeof(datasq));
			if (ret < 0) {
				ALOGE("diag: failed to connect to [0x%x:0x%x][0x%x:0x%x]\n",
					pkt.service, pkt.instance, pkt.node, pkt.port);
				return -1;
			}
			watch_add_writeq(perif->data_fd, &perif->dataq, NULL, 0);
			perif->data_open =  true;

			sl2 = sizeof(sq);
			if (getsockname(fd, (void *)&sq, &sl2))
				return -1;
			perif->data_ch.node = sq.sq_node;
			perif->data_ch.port = sq.sq_port;
			perif->data_ch.service = pkt.service;
			perif->data_ch.instance = pkt.instance;
			/* save remote socket address to handle further control event from qrtr */
			perif->data_ch_remote.node = pkt.node;
			perif->data_ch_remote.port = pkt.port;
			ALOGI("diag: %s: data socket [0x%x:0x%x] connected with service [0x%x:0x%x] socket [0x%x:0x%x]\n",
				__func__, sq.sq_node, sq.sq_port, pkt.service, pkt.instance, pkt.node, pkt.port);
		}
		break;
	case QRTR_TYPE_DEL_SERVER:
		ALOGI("diag: %s: Received del server notify on data ch of periph:%s\n", __func__, perif->name);
		if (perif->data_open) {
			if ((perif->data_ch_remote.node == pkt.node) &&
				(perif->data_ch_remote.port == pkt.node)) {
				/* Reconstruct diag instance ID */
				pkt.instance = ((pkt.instance << 8) | pkt.version);
				watch_remove_writeq(perif->data_fd);
				perif->data_open = false;
				ALOGI("diag: %s: Received del server on [0x%x:0x%x] for perif:'%s' from service [0x%x:0x%x]\n",
					__func__, perif->data_ch.node, perif->data_ch.port,
					perif->name, pkt.service, pkt.instance);
			}
		}
		break;
	case QRTR_TYPE_DATA:
		if (!perif->data_open) {
			connect(perif->data_fd, (struct sockaddr *)&sq, sizeof(sq));
			perif->data_open = true;
			perif->socket_node = pkt.node;
			sl2 = sizeof(sq);
			if (getsockname(fd, (void *)&sq, &sl2))
				return -1;
			perif->data_ch.node = sq.sq_node;
			perif->data_ch.port = sq.sq_port;
			perif->data_ch.service = pkt.service;
			perif->data_ch.instance = pkt.instance;
			/* save remote socket address to handle further control event from qrtr */
			perif->data_ch_remote.node = pkt.node;
			perif->data_ch_remote.port = pkt.port;
			ALOGI("diag: rcvd qrtr new client on data chnl for perif[%s], node: %x, port: %x, svc: %d, ins: %d\n", perif->name, sq.sq_node, sq.sq_port, pkt.service, pkt.instance);

			watch_add_writeq(perif->data_fd, &perif->dataq, NULL, DATA_FD);
		}

		if (!diagmem->feature[perif->periph_id].untag_header) {
			diag_fwd_data(perif, perif->periph_id, pkt.data, pkt.data_len, 0);
			break;
		}

		read_len = 0;
		while(read_len < pkt.data_len) {
			struct tagged_pkt *tpkt = (char *)pkt.data + read_len;

			if (perif->root_pd.diag_id == tpkt->diag_id) {
				pd_val = perif->root_pd.pd_val;
			} else {
				for (i = 0; i < MAX_PERIPHERAL_UPD; i++) {
					if (perif->upd_info[i].diag_id == tpkt->diag_id) {
						pd_val = perif->upd_info[i].pd_val;
						break;
					}
				}
			}
			if (pd_val)
				diag_fwd_data(perif, pd_val, tpkt->payload, tpkt->len, 0);
			read_len += sizeof(*tpkt) + tpkt->len;
		}

		break;
	case QRTR_TYPE_DEL_CLIENT:
		/* handle del client by using remote socket info */
		if (perif->data_open) {
			if ((perif->data_ch_remote.node == pkt.node) &&
				(perif->data_ch_remote.port == pkt.node)) {
				watch_remove_writeq(perif->data_fd);
				perif->data_open = false;
				ALOGI("diag: %s: rcvd qrtr del client on data chnl for perif[%s] from [0x%x:0x%x]\n",
					__func__, perif->name, pkt.node, pkt.node);
			}
		}
		break;
	case QRTR_TYPE_BYE:
		if (perif->socket_node == pkt.node) {
			sl2 = sizeof(sq);
			if (getsockname(fd, (void *)&sq, &sl2))
				return -1;
			ALOGI("diag: rcvd qrtr bye on data chnl for perif[%s] node: %x, port: %x, svc: %d, ins: %d\n", perif->name, sq.sq_node, sq.sq_port, pkt.service, pkt.instance);
			watch_remove_writeq(perif->data_fd);
			perif->data_open = false;
		}

		break;
	default:
		fprintf(stderr, "Unhandled DIAG DATA message from %d:%d (%d)\n",
			pkt.node, pkt.port, pkt.type);
		break;
	}

	return 0;
}

static int qrtr_dci_recv(int fd, void *data)
{
	struct peripheral *perif;
	struct sockaddr_qrtr sq;
	struct qrtr_packet pkt;
	struct circ_buf *buf;
	socklen_t sl, sl2;
	ssize_t n;
	int ret;

	memset(&sq, 0, sizeof(sq));

	if (!data)
		return -EINVAL;

	perif = (struct peripheral *)data;
	buf = &perif->recv_buf;

	sl = sizeof(sq);
	n = recvfrom(fd, buf->buf, sizeof(buf->buf), 0, (void *)&sq, &sl);
	if (n < 0) {
		ret = -errno;
		if (ret != -ENETRESET)
			fprintf(stderr, "[DIAG-QRTR] recvfrom failed: %d\n", ret);
		return ret;
	}

	ret = qrtr_decode(&pkt, buf, n, &sq);
	if (ret < 0) {
		fprintf(stderr, "[PD-MAPPER] unable to decode qrtr packet\n");
		return ret;
	}

	switch (pkt.type) {
	case QRTR_TYPE_DEL_CLIENT:
		break;
	case QRTR_TYPE_BYE:
		if (perif->socket_node == pkt.node) {
			sl2 = sizeof(sq);
			if (getsockname(fd, (void *)&sq, &sl2))
				return -1;
			ALOGI("diag: rcvd qrtr bye on dci data chnl for perif: %d, node: %x, port: %x, svc: %d, ins: %d\n", perif->periph_id, sq.sq_node, sq.sq_port, pkt.service, pkt.instance);
			watch_remove_writeq(perif->dci_data_fd);
			perif->dci_data_open = false;
		}
		break;
	case QRTR_TYPE_DATA:
		if (!perif->dci_data_open) {
			connect(perif->dci_data_fd, (struct sockaddr *)&sq, sizeof(sq));
			perif->dci_data_open = true;
			perif->socket_node = pkt.node;
			sl2 = sizeof(sq);
			if (getsockname(fd, (void *)&sq, &sl2))
				return -1;
			perif->dci_data_ch.node = sq.sq_node;
			perif->dci_data_ch.port = sq.sq_port;
			perif->dci_data_ch.service = pkt.service;
			perif->dci_data_ch.instance = pkt.instance;
			ALOGI("diag: rcvd qrtr new client on dci data chnl for perif: %d, node: %x, port: %x, svc: %d, ins: %d\n", perif->periph_id, sq.sq_node, sq.sq_port, pkt.service, pkt.instance);

			watch_add_writeq(perif->dci_data_fd, &perif->dci_dataq, NULL, DCI_FD);
		}
		diag_dci_process_peripheral_data(perif, pkt.data, pkt.data_len);
		break;
	default:
		fprintf(stderr, "Unhandled DIAG DATA message from %d:%d (%d)\n",
			pkt.node, pkt.port, pkt.type);
		break;
	}

	return 0;
}
static int qrtr_dci_cmd_recv(int fd, void *data)
{
	struct peripheral *perif = data;
	struct sockaddr_qrtr cmdsq;
	struct sockaddr_qrtr sq;
	struct qrtr_packet pkt;
	struct circ_buf *buf = &perif->recv_buf;
	socklen_t sl, sl2;
	ssize_t n;
	int ret;

	memset(&sq, 0, sizeof(sq));

	sl = sizeof(sq);
	n = recvfrom(fd, buf->buf, sizeof(buf->buf), 0, (void *)&sq, &sl);
	if (n < 0) {
		ret = -errno;
		if (ret != -ENETRESET)
			fprintf(stderr, "[DIAG-QRTR] recvfrom failed: %d\n", ret);
		return ret;
	}

	ret = qrtr_decode(&pkt, buf->buf, n, &sq);
	if (ret < 0) {
		fprintf(stderr, "[PD-MAPPER] unable to decode qrtr packet\n");
		return ret;
	}

	switch (pkt.type) {
	case QRTR_TYPE_DEL_CLIENT:
		break;
	case QRTR_TYPE_DATA:
		diag_dci_process_peripheral_data(perif, pkt.data, pkt.data_len);
		break;
	case QRTR_TYPE_NEW_SERVER:
		if (pkt.node == 0 && pkt.port == 0)
			break;

		printf("diag: Connecting CMD socket to %d:%d\n", pkt.node, pkt.port);
		cmdsq.sq_family = AF_QIPCRTR;
		cmdsq.sq_node = pkt.node;
		cmdsq.sq_port = pkt.port;

		sl2 = sizeof(sq);
		if (getsockname(fd, (void *)&sq, &sl2))
			return -1;
		perif->dci_cmd_ch.node = sq.sq_node;
		perif->dci_cmd_ch.port = sq.sq_port;
		perif->dci_cmd_ch.service = pkt.service;
		perif->dci_cmd_ch.instance = pkt.instance;
		ALOGI("diag: rcvd qrtr new server on dci cmd chnl for perif: %d, node: %x, port: %x, svc: %d, ins: %d\n", perif->periph_id, sq.sq_node, sq.sq_port, pkt.service, pkt.instance);

		ret = connect(perif->dci_cmd_fd, (struct sockaddr *)&cmdsq, sizeof(cmdsq));
		if (ret < 0)
			err(1, "failed to connect to %d:%d", cmdsq.sq_node, cmdsq.sq_port);
		watch_add_writeq(perif->dci_cmd_fd, &perif->dci_cmdq, NULL, DCI_CMD_FD);
		ALOGI("diag: rcvd dci new server notify\n");
		diag_dci_notify_client(PERIPHERAL_MASK(perif->periph_id), DIAG_STATUS_OPEN, DIAG_LOCAL_PROC);
		break;
	case QRTR_TYPE_DEL_SERVER:
		sl2 = sizeof(sq);
		if (getsockname(fd, (void *)&sq, &sl2))
			return -1;
		ALOGI("diag: rcvd qrtr bye on dci cmd chnl for perif: %d, node: %x, port: %x, svc: %d, ins: %d\n", perif->periph_id, sq.sq_node, sq.sq_port, pkt.service, pkt.instance);
		watch_remove_writeq(perif->dci_cmd_fd);
		diag_dci_notify_client(PERIPHERAL_MASK(perif->periph_id), DIAG_STATUS_CLOSED, DIAG_LOCAL_PROC);
		break;
	default:
		fprintf(stderr, "Unhandled DIAG CMD message from %d:%d (%d)\n",
			pkt.node, pkt.port, pkt.type);
		break;
	}

	return 0;
}

int qrtr_perif_send(struct peripheral *perif, const void *ptr, size_t len)
{
	if (!perif->cmd_open)
		return -ENOENT;

	ALOGM(DIAG_DBG_MASK_INFO, "diag: %s: Sending cmd pkt of len: %zu to perif: %s with port: node [%x: %x]\n",
		__func__, len, perif->name, perif->cmd_ch.port, perif->cmd_ch.node);
	if (diag_debug_mask & DIAG_DBG_MASK_CMD)
		print_hex_dump(__func__, ptr, MIN(len, PRINT_HEX_DUMP_LEN));

	if (perif->features & DIAG_FEATURE_APPS_HDLC_ENCODE)
		queue_push(NULL, &perif->cmdq, ptr, len);
	else
		hdlc_enqueue(&perif->cmdq, ptr, len);

	return 0;
}
void qrtr_perif_flow_reset(int p_mask)
{
	struct peripheral *peripheral;
	struct list_head *item;
	int i;

	list_for_each(item, &peripherals) {
		peripheral = container_of(item, struct peripheral, node);
		i = peripheral->periph_id;
		if (p_mask & MD_PERIPHERAL_MASK(i))
			watch_reset_flow(peripheral->flow);
	}
}

void qrtr_perif_close(struct peripheral *perif)
{
	(void)perif;
}

/* set self peripheral name which will be used to tag with diag ID */
static void qrtr_set_perif_name(struct peripheral *perif, int inst_id)
{
	switch(inst_id)
	{
	case DIAG_INSTANCE_BASE_TELE_GVM:
		perif->self_name = strdup("tele_gvm");
		break;
	case DIAG_INSTANCE_BASE_FOTA_GVM:
		perif->self_name = strdup("fota_gvm");
		break;
	default:
		perif->self_name = strdup("unknown");
		break;
	}
}

static int qrtr_perif_init_subsystem(const char *name, int instance_base, int id)
{
	struct peripheral *perif;
	int flags, ret;
	int i;
	int rx_buf_sz = INT_MAX;

	perif = calloc(1, sizeof(*perif));
	if (!perif)
		return -ENOMEM;

	perif->flow = watch_flow_new();
	perif->dci_flow = watch_flow_new();
	perif->name = strdup(name);
	perif->periph_id = id;
	perif->base_instance_id = instance_base;
	perif->send = qrtr_perif_send;
	perif->close = qrtr_perif_close;
	perif->sockets = true;
	perif->glinkpkt = false;
	diagmem->buffering_mode[id].mode = DIAG_BUFFERING_MODE_STREAMING;
	diagmem->buffering_mode[id].peripheral = id;
	diagmem->buffering_mode[id].high_wm_val = DEFAULT_HIGH_WM_VAL;
	diagmem->buffering_mode[id].low_wm_val = DEFAULT_LOW_WM_VAL;
	diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][id].mode = DIAG_BUFFERING_MODE_STREAMING;
	diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][id].peripheral = id;
	diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][id].high_wm_val = DEFAULT_HIGH_WM_VAL;
	diagmem->dci_buffering_mode[DIAG_LOCAL_PROC][id].low_wm_val = DEFAULT_LOW_WM_VAL;
	perif->received_feature_mask = 0;
	perif->sent_feature_mask = 0;
	perif->socket_node = -1;

	perif->cntl_fd = qrtr_open(0);
	if (perif->cntl_fd < 0) {
		ALOGE("diag: %s: failed to create control socket\n", __func__);
		goto exit;
	}

	perif->data_fd = qrtr_open(0);
	if (perif->data_fd < 0) {
		ALOGE("diag: %s: failed to create data socket\n", __func__);
		goto exit_cntl;
	}

	perif->cmd_fd = qrtr_open(0);
	if (perif->cmd_fd < 0) {
		ALOGE("diag: %s: failed to create command socket\n", __func__);
		goto exit_cmd;
	}

	if (!vm_enabled) {
		perif->self_name = strdup("apps");

		perif->dci_data_fd = qrtr_open(0);
		if (perif->dci_data_fd < 0) {
			ALOGE("diag: %s: failed to create dci command socket\n", __func__);
			goto exit_cmd;
		}

		perif->dci_cmd_fd = qrtr_open(0);
		if (perif->dci_cmd_fd < 0) {
			ALOGE("diag: %s: failed to create dci command socket\n", __func__);
			goto exit_dci_data;
		}

		ret = setsockopt(perif->data_fd, SOL_SOCKET, SO_RCVBUF, &rx_buf_sz, sizeof(int));
		if (ret < 0) {
			ALOGE("diag: %s: failed to set rx buffer size for data channel\n", __func__);
			goto exit_dci_cmd;
		}
		/*
		 * DIAG does not use the normal packing of "instance << 8 | version" in
		 * the one 32-bit "instance" field of the service notifications, so
		 * pass the DIAG instance information as "version" into these functions
		 * instead.
		 */
		flags = fcntl(perif->cmd_fd, F_GETFL, 0);
		fcntl(perif->cmd_fd, F_SETFL, flags | O_NONBLOCK);
		flags = fcntl(perif->cntl_fd, F_GETFL, 0);
		fcntl(perif->cntl_fd, F_SETFL, flags | O_NONBLOCK);
		flags = fcntl(perif->dci_cmd_fd, F_GETFL, 0);
		fcntl(perif->dci_cmd_fd, F_SETFL, flags | O_NONBLOCK);
		qrtr_publish(perif->cntl_fd, DIAG_SERVICE_ID, instance_base + DIAG_INSTANCE_CNTL, 0);
		qrtr_new_lookup(perif->cmd_fd, DIAG_SERVICE_ID, instance_base + DIAG_INSTANCE_CMD, 0);
		qrtr_publish(perif->data_fd, DIAG_SERVICE_ID, instance_base + DIAG_INSTANCE_DATA, 0);
		qrtr_publish(perif->dci_data_fd, DIAG_SERVICE_ID, instance_base + DIAG_INSTANCE_DCI, 0);
		qrtr_new_lookup(perif->dci_cmd_fd, DIAG_SERVICE_ID, instance_base + DIAG_INSTANCE_DCI_CMD, 0);
		watch_add_readfd(perif->cntl_fd, qrtr_cntl_recv, perif, NULL);
		watch_add_readfd(perif->cmd_fd, qrtr_cmd_recv, perif, NULL);
		watch_add_readfd(perif->data_fd, qrtr_data_recv, perif, perif->flow);
		watch_add_readfd(perif->dci_cmd_fd, qrtr_dci_cmd_recv, perif, NULL);
		watch_add_readfd(perif->dci_data_fd, qrtr_dci_recv, perif, perif->dci_flow);
	} else {
		qrtr_set_perif_name(perif, instance_base);
		/* Enable cmd, cntl & data channel for gvm peripherals */
		flags = fcntl(perif->cmd_fd, F_GETFL, 0);
		fcntl(perif->cmd_fd, F_SETFL, flags | O_NONBLOCK);
		flags = fcntl(perif->cntl_fd, F_GETFL, 0);
		fcntl(perif->cntl_fd, F_SETFL, flags | O_NONBLOCK);
		qrtr_new_lookup(perif->cntl_fd, DIAG_SERVICE_ID, instance_base + DIAG_INSTANCE_CNTL, 0);
		qrtr_new_lookup(perif->data_fd, DIAG_SERVICE_ID, instance_base + DIAG_INSTANCE_DATA, 0);
		qrtr_publish(perif->cmd_fd, DIAG_SERVICE_ID, instance_base + DIAG_INSTANCE_CMD, 0);
		watch_add_readfd(perif->cntl_fd, qrtr_cntl_recv, perif, NULL);
		watch_add_readfd(perif->data_fd, qrtr_data_recv, perif, perif->flow);
		watch_add_readfd(perif->cmd_fd, qrtr_cmd_recv, perif, NULL);
	}
	list_add(&peripherals, &perif->node);
	perif->root_pd.pd_val = -1;
	for (i = 0; i < MAX_PERIPHERAL_UPD; i++) {
		perif->upd_info[i].pd_val = -1;
	}

	list_init(&perif->cmdq);
	list_init(&perif->cntlq);
	list_init(&perif->dataq);
	list_init(&perif->dci_cmdq);
	list_init(&perif->dci_dataq);

	return 0;

exit_dci_cmd:
	qrtr_close(perif->dci_cmd_fd);
exit_dci_data:
	qrtr_close(perif->dci_data_fd);
exit_cmd:
	qrtr_close(perif->cmd_fd);
exit_data:
	free(perif->self_name);
	qrtr_close(perif->data_fd);
exit_cntl:
	qrtr_close(perif->cntl_fd);
exit:
	free(perif->name);
	watch_flow_put(perif->flow);
	free(perif);

	return 0;
}

int peripheral_qrtr_init(void)
{
	if(!vm_enabled) {
		qrtr_perif_init_subsystem("modem", DIAG_INSTANCE_BASE_MODEM, PERIPHERAL_MODEM);
#ifndef FEATURE_SDXKUNO_DIAG
		qrtr_perif_init_subsystem("lpass", DIAG_INSTANCE_BASE_LPASS, PERIPHERAL_LPASS);
		qrtr_perif_init_subsystem("wcnss", DIAG_INSTANCE_BASE_WCNSS, PERIPHERAL_WCNSS);
		qrtr_perif_init_subsystem("sensors", DIAG_INSTANCE_BASE_SENSORS, PERIPHERAL_SENSORS);
		qrtr_perif_init_subsystem("cdsp", DIAG_INSTANCE_BASE_CDSP, PERIPHERAL_CDSP);
		qrtr_perif_init_subsystem("wdsp", DIAG_INSTANCE_BASE_WDSP, PERIPHERAL_WDSP);
		qrtr_perif_init_subsystem("npu", DIAG_INSTANCE_BASE_NPU, PERIPHERAL_NPU);
		qrtr_perif_init_subsystem("nsp1", DIAG_INSTANCE_BASE_NSP1, PERIPHERAL_NSP1);
		qrtr_perif_init_subsystem("gpdsp0", DIAG_INSTANCE_BASE_GPDSP0, PERIPHERAL_GPDSP0);
		qrtr_perif_init_subsystem("gpdsp1", DIAG_INSTANCE_BASE_GPDSP1, PERIPHERAL_GPDSP1);
		qrtr_perif_init_subsystem("helios_m55", DIAG_INSTANCE_BASE_HELIOS_M55, PERIPHERAL_HELIOS_M55);
		qrtr_perif_init_subsystem("soccp", DIAG_INSTANCE_BASE_SOCCP, PERIPHERAL_SOCCP);
#endif /* FEATURE_SDXKUNO_DIAG */
		qrtr_perif_init_subsystem("tele-gvm", DIAG_INSTANCE_BASE_TELE_GVM, PERIPHERAL_TELE_GVM);
		qrtr_perif_init_subsystem("fota-gvm", DIAG_INSTANCE_BASE_FOTA_GVM, PERIPHERAL_FOTA_GVM);
	} else {
		/* For gvm, apps is the remote subsystem */
#ifdef DIAG_TELE_VM_ENABLED
		qrtr_perif_init_subsystem("apps", DIAG_INSTANCE_BASE_TELE_GVM, PERIPHERAL_APPS);
#endif /* DIAG_TELE_VM_ENABLED */
#ifdef DIAG_FOTA_VM_ENABLED
		qrtr_perif_init_subsystem("apps", DIAG_INSTANCE_BASE_FOTA_GVM, PERIPHERAL_APPS);
#endif /* DIAG_FOTA_VM_ENABLED */
	}

	return 0;
}
