/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag MHI communication support

GENERAL DESCRIPTION

Implementation of communication layer between diag and MHI.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include "diag.h"
#include "diag_mhi.h"
#include "watch.h"
#include "util.h"
#include "diag_mux.h"

#define NUM_MHI_DEVICES		8
#define DIAG_MHI_MAX_PKT_SIZE	4096
#define UEVENT_BUFFER_SIZE	4096

#define MSM		0
#define MDM		1
#define MDM_2	2

enum uevent_state_type {
	UEVENT_REMOVED,
	UEVENT_ADDED,
};

struct uevent_info {
	int fd;
	unsigned char *buf;
};

struct dev_info {
	int fd;
	int uevent_state;
	int remote_check;
	char device_name[100];
	char uevent_name[100];
};

struct diag_mhi {
	unsigned char buf[DIAG_MHI_MAX_PKT_SIZE];
	unsigned int buf_len;
	int dev_id;
	struct dev_info mhi;
	struct dev_info usb;
};

struct diag_mhi diag_mhi_info[NUM_MHI_DEVICES] =
{
	{
		.mhi = {
			.device_name = "/dev/mhi_0306_02.01.00_pipe_4",
			.uevent_name = "mhi_0306_02.01.00_pipe_4",
			.fd = -1,
			.uevent_state = UEVENT_REMOVED,
			.remote_check = MDM,
		},
		.usb = {
			.device_name = "/dev/ffs-diag-1",
			.fd = -1,
		},
	},
	{
		.mhi = {
			.device_name = "/dev/mhi_0308_01.01.00_pipe_4",
			.uevent_name = "mhi_0308_01.01.00_pipe_4",
			.fd = -1,
			.uevent_state = UEVENT_REMOVED,
			.remote_check = MDM,
		},
		.usb = {
			.device_name = "/dev/ffs-diag-1",
			.fd = -1,
		},
	},
	{
                .mhi = {
                        .device_name = "/dev/mhi_0309_01.01.00_pipe_4",
                        .uevent_name = "mhi_0309_01.01.00_pipe_4",
                        .fd = -1,
                        .uevent_state = UEVENT_REMOVED,
                        .remote_check = MDM,
                },
                .usb = {
                        .device_name = "/dev/ffs-diag-1",
                        .fd = -1,
                },
        },
	{
		.mhi = {
			.device_name = "/dev/mhi_1103_00.01.00_pipe_4",
			.uevent_name = "mhi_1103_00.01.00_pipe_4",
			.fd = -1,
			.uevent_state = UEVENT_REMOVED,
			.remote_check = MDM_2,
		},
		.usb = {
			.device_name = "/dev/ffs-diag-2",
			.fd = -1,
		},
	},
	{
		.mhi = {
			.device_name = "/dev/mhi_1107_01.01.00_pipe_4",
			.uevent_name = "mhi_1107_01.01.00_pipe_4",
			.fd = -1,
			.uevent_state = UEVENT_REMOVED,
			.remote_check = MDM_2,
		},
		.usb = {
			.device_name = "/dev/ffs-diag-2",
			.fd = -1,
		},
	},
	{
                .mhi = {
                        .device_name = "/dev/mhi_1107_00.01.00_pipe_4",
                        .uevent_name = "mhi_1107_00.01.00_pipe_4",
                        .fd = -1,
                        .uevent_state = UEVENT_REMOVED,
                        .remote_check = MDM_2,
                },
                .usb = {
                        .device_name = "/dev/ffs-diag-2",
                        .fd = -1,
                },
        },
	{
		.mhi = {
			.device_name = "/dev/mhi_110a_01.01.00_pipe_4",
			.uevent_name = "mhi_110a_01.01.00_pipe_4",
			.fd = -1,
			.uevent_state = UEVENT_REMOVED,
			.remote_check = MDM_2,
		},
		.usb = {
			.device_name = "/dev/ffs-diag-2",
			.fd = -1,
		},
	},
	{
		.mhi = {
			.device_name = "/dev/mhi_110e_00.01.00_pipe_4",
			.uevent_name = "mhi_110e_00.01.00_pipe_4",
			.fd = -1,
			.uevent_state = UEVENT_REMOVED,
			.remote_check = MDM_2,
		},
		.usb = {
			.device_name = "/dev/ffs-diag-2",
			.fd = -1,
		},
	},
};

static struct dev_info *diag_mhi_get_active_device(int remote_token)
{
	struct dev_info *mhi;
	int i;

	for(i = 0; i < NUM_MHI_DEVICES; i++) {
		mhi = &(diag_mhi_info[i].mhi);
		if ((mhi->remote_check == remote_token) && (mhi->fd != -1))
			return mhi;
	}
	return NULL;
}

int diag_mhi_write(void* ptr, int len, int remote_token)
{
	struct dev_info *mhi;
	int ret;

	mhi = diag_mhi_get_active_device(remote_token);
	if (!mhi)
		return 0;

	ret = write(mhi->fd, ptr, len);
	if (ret < 0)
		ALOGE("%s: failed fd(%d) err:%d\n", __func__, mhi->fd, errno);

	return 0;
}

static int diag_mhi_recv(int fd, void *data)
{
	struct diag_mhi *mhi_info = (struct diag_mhi *)data;
	int len;

	if (!mhi_info)
		return -EINVAL;

	len = read(fd, (void *)mhi_info->buf, DIAG_MHI_MAX_PKT_SIZE);
	if (len < 0) {
		printf("%s: failed fd(%d) err:%d\n", __func__, fd, errno);
		return 0;
	}
	diag_mux_write(NULL, mhi_info->mhi.remote_check, 0, mhi_info->buf, len, NULL, 0);

	return 0;
}

/* Forward declare to be used in timer callback */
static void diag_mhi_open(void *data);

static void __diag_mhi_open(struct diag_mhi *mhi_info)
{
	struct dev_info *mhi = &mhi_info->mhi;
	struct dev_info *usb = &mhi_info->usb;

	mhi->fd = open(mhi->device_name, O_RDWR | O_CLOEXEC);
	if (mhi->fd < 0) {
		printf("%s: %s errno:%d\n", __func__, mhi->device_name, errno);
		if (mhi->uevent_state == UEVENT_ADDED)
			watch_add_timer(diag_mhi_open, (void *)mhi_info, 500, false);
		return;
	}
	watch_add_readfd(mhi->fd, diag_mhi_recv, mhi_info, NULL);
	diag_usb_bridge_enable(usb->fd, mhi_info->mhi.remote_check);
}

static void diag_mhi_open(void *data)
{
	struct diag_mhi *mhi_info = (struct diag_mhi *)data;
	struct dev_info *mhi = &mhi_info->mhi;

	if (mhi->uevent_state != UEVENT_ADDED || mhi->fd > 0)
		return;

	__diag_mhi_open(mhi_info);
	diag_notify_md_client(mhi_info->mhi.remote_check, 1, DIAG_STATUS_OPEN);
}

static void diag_mhi_close(struct diag_mhi *mhi_info)
{
	struct dev_info *mhi = &mhi_info->mhi;
	struct dev_info *usb = &mhi_info->usb;

	diag_usb_bridge_disable(usb->fd);
	watch_remove_fd(mhi->fd);
	close(mhi->fd);
	mhi->fd = -1;
	diag_notify_md_client(mhi_info->mhi.remote_check, 1, DIAG_STATUS_CLOSED);
}

static int diag_mhi_uevent_notify(int fd, void *data)
{
	struct uevent_info *uinfo = data;
	struct diag_mhi *mhi_info;
	struct dev_info *mhi;
	ssize_t n;
	int i;
	(void)fd;

	if (!uinfo) {
		printf("%s: invalid data pointer\n", __func__);
		return 0;
	}

	uinfo->buf = alloca(UEVENT_BUFFER_SIZE);
	if (!uinfo->buf) {
		ALOGE("diag: %s: failed to allocate memory for uinfo->buf\n", __func__);
		return 0;
	}

	n = recv(uinfo->fd, uinfo->buf, UEVENT_BUFFER_SIZE, 0);
	if (n < 0) {
		ALOGE("%s: recv failed: %d\n", __func__, errno);
		return 0;
	}

	for (i = 0; i < NUM_MHI_DEVICES; i++) {
		mhi_info = &diag_mhi_info[i];
		mhi = &mhi_info->mhi;

		if (!strstr((const char *)uinfo->buf, mhi->uevent_name))
			continue;

		if (strstr((const char *)uinfo->buf, "add@")) {
			mhi->uevent_state = UEVENT_ADDED;
			diag_mhi_open(mhi_info);
		} else if (strstr((const char *)uinfo->buf, "remove@")) {
			mhi->uevent_state = UEVENT_REMOVED;
			diag_mhi_close(mhi_info);
		}
		ALOGI("diag: %s: uevent recvd %s\n", __func__, uinfo->buf);
	}

	return 0;
}

static int diag_mhi_uevent_init(void)
{
	struct uevent_info *uinfo;
	struct sockaddr_nl addr;

	uinfo = calloc(1, sizeof(*uinfo));
	if (!uinfo) {
		printf("failed to allocate uevent state variable %d\n", errno);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_groups = 0xffffffff;

	uinfo->fd = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_KOBJECT_UEVENT);
	if (uinfo->fd < 0) {
		printf("Creating netlink socket failed %d", errno);
		free(uinfo);
		uinfo = NULL;
		return -1;
	}

	/* Let the kernel auto-bind an address to the socket */
	if (bind(uinfo->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		printf("Binding to netlink socket failed %d", errno);
		close(uinfo->fd);
		free(uinfo);
		uinfo = NULL;
		return -1;
	}

	watch_add_readfd(uinfo->fd, diag_mhi_uevent_notify, uinfo, NULL);
	return 0;
}

int diag_mhi_init(void)
{
	struct diag_mhi *mhi_info;
	int i, j;

	if (diag_mhi_uevent_init()) {
		err(1, "failed to create mhi status notifier\n");
		return -1;
	}

	for (i = 0; i < NUM_MHI_DEVICES; i++) {
		mhi_info = &diag_mhi_info[i];
		mhi_info->dev_id = i;


		if (mhi_info->usb.fd < 0)
			mhi_info->usb.fd = __diag_usb_open(mhi_info->usb.device_name);
		if (mhi_info->usb.fd < 0) {
			ALOGE("failed to create mhi usb handle %d\n",mhi_info->usb.fd);
			continue;
		}

		/* Update USB fd for other mhi device entries that use same ffs */
		for (j = 0; j < NUM_MHI_DEVICES; j++) {
			if (!strcmp(mhi_info->usb.device_name, diag_mhi_info[j].usb.device_name))
				diag_mhi_info[j].usb.fd = mhi_info->usb.fd;
		}

		/* Try to open in case link is up before diag-router */
		__diag_mhi_open(mhi_info);
	}

	return 0;
}

int diag_get_mhi_remote_mask(void)
{
	int remote_mask = 0, i, shift_bit = 0;

	for (i = 0; i < NUM_MHI_DEVICES; i++) {
		shift_bit = diag_mhi_info[i].mhi.remote_check;

		if (diag_mhi_info[i].mhi.fd >= 0 && (shift_bit > 0))
			remote_mask = remote_mask | 1 << (shift_bit - 1);
	}
	return remote_mask;
}
