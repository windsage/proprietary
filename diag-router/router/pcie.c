/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2012-2015, 2018-2021, 2023-2024, Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag over PCIe communication protocol

GENERAL DESCRIPTION

Implementation of diag-router interaction with PCIe

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <linux/types.h>
#include <linux/netlink.h>

#include "diag.h"
#include "diag_mhi.h"
#include "diag_mux.h"
#include "dm.h"
#include "mbuf.h"
#include "hdlc.h"
#include "util.h"
#include "watch.h"

#define NUM_DIAG_PCIE_DEV			1
#define DIAG_PCIE_LOCAL				0
#define DIAG_PCIE_NAME_SZ			24
#define DIAG_READ_BUF_SZ			16384
#define DIAG_MAX_PCIE_PKT_SZ			2048
#define DIAG_LEGACY				"DIAG_PCIE"
#define MHI_CLIENT_DIAG_OUT			4
#define MHI_CLIENT_DIAG_IN			5
#define MHI_UEVENT_REQUEST_PATH 		"/sys/devices/virtual/mhi/mhi_ctrl/uevent"
#define MHI_UEVENT_BUFFER_SIZE			1024

#define DIAG_MHI_PIPE_UNKNOWN			0x0000

#define DIAG_MHI_PIPE_MASK_R			0x00FF
#define DIAG_MHI_PIPE_CONNECTED_R		0x0001
#define DIAG_MHI_PIPE_DISCONNECTED_R		0x0002
#define DIAG_MHI_PIPE_CONFIGURED_R		0x0003

#define DIAG_MHI_PIPE_MASK_W			0xFF00
#define DIAG_MHI_PIPE_CONNECTED_W		0x0100
#define DIAG_MHI_PIPE_DISCONNECTED_W		0x0200
#define DIAG_MHI_PIPE_CONFIGURED_W		0x0300

#define DIAG_MHI_PIPE_CONNECTED_RW \
  (DIAG_MHI_PIPE_CONNECTED_R | DIAG_MHI_PIPE_CONNECTED_W)

#define DIAG_MHI_PIPE_DISCONNECTED_RW \
  (DIAG_MHI_PIPE_DISCONNECTED_R | DIAG_MHI_PIPE_DISCONNECTED_W)

#define DIAG_MHI_CHANNEL			"MHI_CHANNEL_STATE"
#define DIAG_MHI_CHANNEL_CONNECTED_R		"MHI_CHANNEL_STATE_4=CONNECTED"
#define DIAG_MHI_CHANNEL_CONFIGURED_R		"MHI_CHANNEL_STATE_4=CONFIGURED"
#define DIAG_MHI_CHANNEL_DISCONNECTED_R		"MHI_CHANNEL_STATE_4=DISCONNECTED"
#define DIAG_MHI_CHANNEL_CONNECTED_W		"MHI_CHANNEL_STATE_5=CONNECTED"
#define DIAG_MHI_CHANNEL_CONFIGURED_W 		"MHI_CHANNEL_STATE_5=CONFIGURED"
#define DIAG_MHI_CHANNEL_DISCONNECTED_W		"MHI_CHANNEL_STATE_5=DISCONNECTED"

#define DIAG_MHI_READ_DEVFILE_PATH 		"/dev/mhi_pipe_4"

#define DIAG_MHI_UEVENT_BROADCAST_GROUP 	0x00000001
#define DIAG_CTRL_XFER_SIZE 			4096

struct diag_client *pcie_dm = NULL;

struct diag_pcie_info {
	int id;
	int ctxt;
	int diag_state;
	int enabled;
	int dev_fd;
	int status;
	int out_fd;
	int read_ch_status;
	int write_ch_status;
	int uevent_fd;

	char name[DIAG_PCIE_NAME_SZ];

	uint16_t mhi_ctrl_state;

	uint32_t out_chan;
	/* read channel - always even */
	uint32_t in_chan;

	struct diag_mux_ops *ops;
	struct diag_client *dm;
	struct list_head outq;

	unsigned char *read_buf;
};

struct diag_pcie_info diag_pcie = {
	.id = DIAG_PCIE_LOCAL,
	.name = DIAG_LEGACY,
	.enabled = 0,
	.ops = NULL,
	.in_chan = MHI_CLIENT_DIAG_OUT,
	.out_chan = MHI_CLIENT_DIAG_IN,
	.read_ch_status = DIAG_MHI_PIPE_UNKNOWN,
	.write_ch_status = DIAG_MHI_PIPE_UNKNOWN,
	.dev_fd = 0,
	.dm = NULL,
	.read_buf = NULL,
};

void diag_pcie_mhi_pipe_open(void);
void diag_pcie_mhi_pipe_close(void);

static void diag_update_mhi_channel_state(uint16_t new_mhi_pipe_state_rw)
{
	uint16_t previous_mhi_pipe_state_rw;
	uint16_t current_mhi_pipe_state_rw;
	uint16_t new_mhi_pipe_state_r;
	uint16_t new_mhi_pipe_state_w;

	previous_mhi_pipe_state_rw = diag_pcie.mhi_ctrl_state;
	current_mhi_pipe_state_rw  = previous_mhi_pipe_state_rw;

	new_mhi_pipe_state_r = new_mhi_pipe_state_rw & DIAG_MHI_PIPE_MASK_R;
	new_mhi_pipe_state_w = new_mhi_pipe_state_rw & DIAG_MHI_PIPE_MASK_W;

	if (new_mhi_pipe_state_r != DIAG_MHI_PIPE_UNKNOWN &&
			new_mhi_pipe_state_r != (previous_mhi_pipe_state_rw & DIAG_MHI_PIPE_MASK_R)) {
		current_mhi_pipe_state_rw &= ~DIAG_MHI_PIPE_MASK_R;
		current_mhi_pipe_state_rw |= new_mhi_pipe_state_r;
	}

	if (new_mhi_pipe_state_w != DIAG_MHI_PIPE_UNKNOWN &&
			new_mhi_pipe_state_w != (previous_mhi_pipe_state_rw & DIAG_MHI_PIPE_MASK_W)) {
		current_mhi_pipe_state_rw &= ~DIAG_MHI_PIPE_MASK_W;
		current_mhi_pipe_state_rw |= new_mhi_pipe_state_w;
	}

	if (current_mhi_pipe_state_rw != previous_mhi_pipe_state_rw) {
		ALOGM(DIAG_DBG_MASK_INFO, "diag: %s: MHI channel state changed from 0x%04x to 0x%04x\n",
		__func__, previous_mhi_pipe_state_rw, current_mhi_pipe_state_rw);
		diag_pcie.mhi_ctrl_state = current_mhi_pipe_state_rw;

		if (current_mhi_pipe_state_rw != DIAG_MHI_PIPE_CONNECTED_RW) {
			/*! If not CONNECTED_RW, any other state suffices. For simplicity and
				clarity overwrite it with DICONNECTED_RW */
			current_mhi_pipe_state_rw = DIAG_MHI_PIPE_DISCONNECTED_RW;
		}
		if (diag_pcie.mhi_ctrl_state == DIAG_MHI_PIPE_CONNECTED_RW)
			diag_pcie_mhi_pipe_open();
		else if (diag_pcie.mhi_ctrl_state == DIAG_MHI_PIPE_DISCONNECTED_RW)
			diag_pcie_mhi_pipe_close();
	}
}

static uint16_t diag_pcie_parse_mhi_uevent(const char *msg, ssize_t msg_len)
{
	char parsed_msg[MHI_UEVENT_BUFFER_SIZE] = {0};
	char *mhi_str = NULL;
	ssize_t index = 0;

	while (index < msg_len)
		index += snprintf(parsed_msg + index, msg_len - index, "%s ", msg + index);

	mhi_str = strstr(parsed_msg, DIAG_MHI_CHANNEL);
	if (NULL != mhi_str) {
	  /* The passed msg contains information about MHI state. Try to get the
		 state */
	  /*! Check if the parsed uevent msg carry MHI_CHANNEL_4 updates */
	  if (strstr(mhi_str, DIAG_MHI_CHANNEL_CONFIGURED_R) != NULL) {
		ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Received MHI uevent with MHI_CHANNEL_STATE_4=CONFIGURED state\n", __func__);
		diag_pcie.read_ch_status = DIAG_MHI_PIPE_CONFIGURED_R;
	  } else if (strstr(mhi_str, DIAG_MHI_CHANNEL_DISCONNECTED_R) != NULL) {
		ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Received MHI uevent with MHI_CHANNEL_STATE_4=DISCONNECTED state\n", __func__);
		diag_pcie.read_ch_status = DIAG_MHI_PIPE_DISCONNECTED_R;
	  } else if (strstr(mhi_str, DIAG_MHI_CHANNEL_CONNECTED_R) != NULL) {
		ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Received MHI uevent with MHI_CHANNEL_STATE_4=CONNECTED state\n", __func__);
		diag_pcie.read_ch_status = DIAG_MHI_PIPE_CONNECTED_R;
	  }

	  /*! Check if the parsed uevent msg carry MHI_CHANNEL_5 updates */
	  if (strstr(mhi_str, DIAG_MHI_CHANNEL_CONFIGURED_W) != NULL) {
		ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Received MHI uevent with MHI_CHANNEL_STATE_5=CONFIGURED state\n", __func__);
		diag_pcie.write_ch_status = DIAG_MHI_PIPE_CONFIGURED_W;
	  } else if (strstr(mhi_str, DIAG_MHI_CHANNEL_DISCONNECTED_W) != NULL) {
		ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Received MHI uevent with MHI_CHANNEL_STATE_5=DISCONNECTED state\n", __func__);
		diag_pcie.write_ch_status = DIAG_MHI_PIPE_DISCONNECTED_W;
	  } else if (strstr(mhi_str, DIAG_MHI_CHANNEL_CONNECTED_W) != NULL) {
		ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Received MHI uevent with MHI_CHANNEL_STATE_5=CONNECTED state\n", __func__);
		diag_pcie.write_ch_status = DIAG_MHI_PIPE_CONNECTED_W;
	  }
	}

	return (diag_pcie.read_ch_status | diag_pcie.write_ch_status);

}

static uint16_t diag_pcie_mhi_check_uevent_mhi_state(void)
{
	int uevent_fd = -1;
	ssize_t msg_len = 0;
	char msg[MHI_UEVENT_BUFFER_SIZE] = {0};
	uint16_t mhi_state = DIAG_MHI_PIPE_UNKNOWN;

	uevent_fd = open(MHI_UEVENT_REQUEST_PATH, O_RDONLY | O_CLOEXEC);
	if (uevent_fd < 0) {
		ALOGE("diag: Error opening MHI uevent file, error: %d\n", errno);
		return mhi_state;
	}
	ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Reading MHI state from uevent file\n", __func__);
	msg_len = read(uevent_fd, msg, sizeof(msg) - 1);
	close(uevent_fd);

	if (msg_len < 0) {
		ALOGE("diag: Error in reading MHI state from MHI uevent file, error: %d\n", errno);
	} else if (msg_len == 0) {
		ALOGE("diag: %s: mhi uevent file is empty\n", __func__);
	} else {
		msg[msg_len] = '\0';
		mhi_state = diag_pcie_parse_mhi_uevent(msg, msg_len + 1);
		ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Read MHI state: 0x%04x from uevent file\n", __func__, mhi_state);
	}
	return mhi_state;
}

static int diag_pcie_open_netlink_socket(int protocol, pid_t pid, uint32_t group)
{
	int netlink_sock_fd = -1;
	struct sockaddr_nl netlink_sock_src_addr;
	int netlink_sock_buf_size = 1024;
	int ret = -1;
	(void)pid;

	memset(&netlink_sock_src_addr, 0, sizeof(netlink_sock_src_addr));
	netlink_sock_src_addr.nl_family  = AF_NETLINK;
	netlink_sock_src_addr.nl_pid     = 0;
	netlink_sock_src_addr.nl_groups  = group;
	netlink_sock_fd = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, protocol);
	if (netlink_sock_fd < 0) {
		ALOGE("diag: %s: Error creating uevent netlink socket, err: %d\n", __func__, errno);
		return ret;
	}

	if (setsockopt(netlink_sock_fd, SOL_SOCKET, SO_RCVBUF | SO_REUSEADDR,
		&netlink_sock_buf_size, sizeof(netlink_sock_buf_size)) < 0) {
		ALOGE("diag: %s: Error setting netlink socket options, err: %d\n", __func__, errno);
		close(netlink_sock_fd);
	} else if (bind(netlink_sock_fd, (const struct sockaddr *)&netlink_sock_src_addr,
			sizeof(netlink_sock_src_addr)) < 0) {
		ALOGE("diag: %s: Error binding netlink socket, err: %d\n", __func__, errno);
		close(netlink_sock_fd);
	} else {
		ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Binding netlink socket successful\n", __func__);
		ret = netlink_sock_fd;
	}
	return ret;
}

static bool diag_pcie_mhi_open_read_uevent(int *uevent_fd)
{
	uint16_t mhi_state = DIAG_MHI_PIPE_UNKNOWN;

	*uevent_fd = diag_pcie_open_netlink_socket(NETLINK_KOBJECT_UEVENT, getpid(), DIAG_MHI_UEVENT_BROADCAST_GROUP);
	if (*uevent_fd < 0) {
		ALOGE("diag: Couldn't create netlink socket. Terminating Monitor Thread\n");
		return false;
	}
	mhi_state = diag_pcie_mhi_check_uevent_mhi_state();
	diag_update_mhi_channel_state(mhi_state);
	return true;
}

int diag_pcie_mhi_uevent_read_cb(int fd, void* data)
{
	int uevent_fd = 0;
	ssize_t recv_msg_len = 0;
	char msg[MHI_UEVENT_BUFFER_SIZE];
	uint16_t mhi_state = DIAG_MHI_PIPE_UNKNOWN;
	struct diag_pcie_info *pcie_info = NULL;
	(void)fd;

	if (!data)
		return 0;

	pcie_info = (struct diag_pcie_info *)data;

	uevent_fd = pcie_info->uevent_fd;

	recv_msg_len = recv(uevent_fd, (void *)&msg, sizeof(msg), MSG_DONTWAIT | MSG_TRUNC);
	if (recv_msg_len <= 0) {
		if (recv_msg_len == 0) {
			ALOGE("diag: %s: Received empty message, ignoring\n", __func__);
		} else {
			/* In case of ENOBUFS error returned by OS close the
			 * socket, reopen it and read the uevent device file of mhi
			 * and based on the read value update the same to mhi
			 * read context.
			 */
			if (errno == ENOBUFS) {
				ALOGE("diag: Received with error (%d), closing current socket fd: %d\n", errno, uevent_fd);
				close(uevent_fd);
				if(!diag_pcie_mhi_open_read_uevent(&uevent_fd))
					return 0;
				diag_pcie.uevent_fd = uevent_fd;
			} else {
				ALOGE("diag: Error receiving uevent msg: %d. Ignoring msg\n", errno);
			}
		}
	} else if (recv_msg_len > sizeof(msg) - 1) {
		/* Discard the msg if its truncated. MHI driver uevents msgs are
		 * of size smaller than MHI_UEVENT_BUFFER_SIZE. So MHI msgs
		 * should never be truncated.
		 */
		ALOGE("diag: Error uevent datagram is truncated %ld\n", recv_msg_len);
	} else {
		/* Ensure that msg received is null terminated */
		msg[recv_msg_len] = '\0';
		mhi_state = diag_pcie_parse_mhi_uevent(msg, recv_msg_len + 1);
		if (mhi_state != DIAG_MHI_PIPE_UNKNOWN)
			diag_update_mhi_channel_state(mhi_state);
		ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Received mhi state is 0x%04x\n", __func__, mhi_state);
	}
	return 0;
}

void diag_pcie_mhi_pipe_open(void)
{
	if (diag_pcie.dev_fd > 0 && diag_pcie.dm) {
		ALOGE("diag: %s: pcie dm registered already\n", __func__);
		return;
	}
	diag_pcie.dev_fd = open(DIAG_MHI_READ_DEVFILE_PATH, O_RDWR);
	if (diag_pcie.dev_fd < 0) {
		ALOGE("diag: Couldn't open mhi pipe (%s), err: %d\n", DIAG_MHI_READ_DEVFILE_PATH, errno);
		return;
	}

	/*! Register mhi pipe for polling */
	diag_pcie.dm = dm_add("PCIe", diag_pcie.dev_fd, diag_pcie.dev_fd, true);
	if (!diag_pcie.dm) {
		ALOGE("diag: %s: Diag monitor register failed for pcie client\n", __func__);
		close(diag_pcie.dev_fd);
		return;
	}
	ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Diag monitor registered for pcie client\n", __func__);
	pcie_dm = diag_pcie.dm;
	dm_enable(diag_pcie.dm);

	diagmem->pcie_state = 1;
	diagmem->transport_set = DIAG_ROUTE_TO_PCIE;
	diag_pcie.enabled = 1;
}

void diag_pcie_mhi_pipe_close(void)
{
	if (!diag_pcie.dm)
		return;

	ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Processing request to close mhi pipe\n", __func__);
	diagmem->pcie_state = 0;
	diag_pcie.enabled = 0;

	dm_disable(diag_pcie.dm);
	watch_remove_fd(diag_pcie.dm->in_fd);
	watch_remove_writeq(diag_pcie.dm->in_fd);

	dm_del(diag_pcie.dev_fd);
	close(diag_pcie.dev_fd);
	diag_pcie.dm = NULL;
	ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Diag monitor de-registered for pcie client\n", __func__);
}

int diag_pcie_open(void)
{
	int uevent_fd = 0;

	diag_pcie.read_buf = malloc(DIAG_READ_BUF_SZ);
	if (!diag_pcie.read_buf) {
		ALOGE("diag: couldn't allocate pcie read buffer\n");
		return -1;
	}

	if (!diag_pcie_mhi_open_read_uevent(&uevent_fd))
		goto close_buf;

	diag_pcie.uevent_fd = uevent_fd;
	watch_add_readfd(uevent_fd, diag_pcie_mhi_uevent_read_cb, &diag_pcie, NULL);
	return 0;

close_buf:
	free(diag_pcie.read_buf);
	diag_pcie.read_buf = NULL;
	return -1;
}

int diag_pcie_close(void)
{
	watch_remove_fd(diag_pcie.uevent_fd);
	close(diag_pcie.uevent_fd);

	diag_pcie_mhi_pipe_close();

	if (diag_pcie.read_buf) {
		free(diag_pcie.read_buf);
		diag_pcie.read_buf = NULL;
	}
	ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: pcie dm is closed\n", __func__);
	return 0;
}

int diag_pcie_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag)
{
	(void)proc;
	(void)peripheral;

	if (diag_pcie.enabled && diag_pcie.write_ch_status
		&& diag_pcie.dev_fd > 0) {

		if (diag_debug_mask & DIAG_DBG_MASK_PCIE) {
			ALOGM(DIAG_DBG_MASK_PCIE, "diag: %s: Packet of len: %d written to pcie\n", __func__, len);
			print_hex_dump(__func__, (void *)buf, MIN(len, PRINT_HEX_DUMP_LEN));
		}

		if (diagmem->p_hdlc_disabled[peripheral]) {
			if (cmd_rsp_flag)
				queue_push_flow(NULL, &diag_pcie.dm->cmd_rsp_q, buf, len, flow);
			else
				queue_push_flow(dm, &diag_pcie.dm->outq, buf, len, flow);
		} else {
			dm_send_flow(dm, diag_pcie.dm, buf, len, flow, cmd_rsp_flag);
		}
	}
	return 0;
}
