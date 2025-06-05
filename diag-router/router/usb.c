/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * Not a contribution.

 * Copyright (C) 2007 The Android Open Source Project

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 *    http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _DEFAULT_SOURCE /* for endian.h */

#include <endian.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/functionfs.h>
#include <pthread.h>
#include <signal.h>

#include "diag.h"
#include "dm.h"
#include "mbuf.h"
#include "hdlc.h"
#include "util.h"
#include "watch.h"
#include "diag_mhi.h"

#define USB_FFS_EP0_NAME	"ep0"
#define USB_FFS_OUT_NAME	"ep1"
#define USB_FFS_IN_NAME		"ep2"

#define USB_PROTOCOL_DIAG	0x30

#ifdef FEATURE_LE_DIAG
#define DIAG_MARKER_SYS_NODE	"/sys/kernel/boot_kpi/kpi_values"
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define cpu_to_le16(x)		(x)
#define cpu_to_le32(x)		(x)
#else
#define cpu_to_le16(x) ((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8))
#define cpu_to_le32(x) ((((x) & 0xff000000u) >> 24) | \
			(((x) & 0x00ff0000u) >>  8) | \
			(((x) & 0x0000ff00u) <<  8) | \
			(((x) & 0x000000ffu) << 24))
#endif

static const struct {
	struct usb_functionfs_descs_head_v2 header;
	__le32 fs_count;
	__le32 hs_count;
	__le32 ss_count;
	struct {
		struct usb_interface_descriptor intf;
		struct usb_endpoint_descriptor_no_audio source;
		struct usb_endpoint_descriptor_no_audio sink;
	} __packed fs_descs, hs_descs;

	struct {
		struct usb_interface_descriptor intf;
		struct usb_endpoint_descriptor_no_audio source;
		struct usb_ss_ep_comp_descriptor source_comp;
		struct usb_endpoint_descriptor_no_audio sink;
		struct usb_ss_ep_comp_descriptor sink_comp;
	} __packed ss_descs;
} __packed descriptors = {
	.header = {
		.magic = cpu_to_le32(FUNCTIONFS_DESCRIPTORS_MAGIC_V2),
		.flags = cpu_to_le32(FUNCTIONFS_HAS_FS_DESC |
				     FUNCTIONFS_HAS_HS_DESC |
				     FUNCTIONFS_HAS_SS_DESC),
		.length = cpu_to_le32(sizeof(descriptors)),
	},
	.fs_count = cpu_to_le32(3),
	.fs_descs = {
		.intf = {
			.bLength = sizeof(descriptors.fs_descs.intf),
			.bDescriptorType = USB_DT_INTERFACE,
			.bNumEndpoints = 2,
			.bInterfaceClass = USB_CLASS_VENDOR_SPEC,
			.bInterfaceSubClass = USB_SUBCLASS_VENDOR_SPEC,
			.bInterfaceProtocol = USB_PROTOCOL_DIAG,
		},
		.source = {
			.bLength = sizeof(descriptors.fs_descs.source),
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 1 | USB_DIR_OUT,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			.wMaxPacketSize = cpu_to_le16(64),
		},
		.sink = {
			.bLength = sizeof(descriptors.fs_descs.sink),
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 2 | USB_DIR_IN,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			.wMaxPacketSize = cpu_to_le16(64),
		},
	},
	.hs_count = cpu_to_le32(3),
	.hs_descs = {
		.intf = {
			.bLength = sizeof(descriptors.hs_descs.intf),
			.bDescriptorType = USB_DT_INTERFACE,
			.bNumEndpoints = 2,
			.bInterfaceClass = USB_CLASS_VENDOR_SPEC,
			.bInterfaceSubClass = USB_SUBCLASS_VENDOR_SPEC,
			.bInterfaceProtocol = USB_PROTOCOL_DIAG,
		},
		.source = {
			.bLength = sizeof(descriptors.hs_descs.source),
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 1 | USB_DIR_OUT,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			.wMaxPacketSize = cpu_to_le16(512),
		},
		.sink = {
			.bLength = sizeof(descriptors.hs_descs.sink),
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 2 | USB_DIR_IN,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			.wMaxPacketSize = cpu_to_le16(512),
		},
	},
	.ss_count = cpu_to_le32(5),
	.ss_descs = {
		.intf = {
			.bLength = sizeof(descriptors.ss_descs.intf),
			.bDescriptorType = USB_DT_INTERFACE,
			.bNumEndpoints = 2,
			.bInterfaceClass = USB_CLASS_VENDOR_SPEC,
			.bInterfaceSubClass = USB_SUBCLASS_VENDOR_SPEC,
			.bInterfaceProtocol = 0x30,
		},
		.source = {
			.bLength = sizeof(descriptors.ss_descs.source),
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 1 | USB_DIR_OUT,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			.wMaxPacketSize = cpu_to_le16(1024),
		},
		.source_comp = {
			.bLength = sizeof(descriptors.ss_descs.source_comp),
			.bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
		},
		.sink = {
			.bLength = sizeof(descriptors.ss_descs.sink),
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 2 | USB_DIR_IN,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			.wMaxPacketSize = cpu_to_le16(1024),
		},
		.sink_comp = {
			.bLength = sizeof(descriptors.ss_descs.sink_comp),
			.bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
		},
	},
};

#define STR_INTERFACE_ "Diag interface"

static const struct {
	struct usb_functionfs_strings_head header;
	struct {
		__le16 code;
		const char str1[sizeof(STR_INTERFACE_)];
	} __attribute__((packed)) lang0;
} __attribute__((packed)) strings = {
	.header = {
		.magic = cpu_to_le32(FUNCTIONFS_STRINGS_MAGIC),
		.length = cpu_to_le32(sizeof(strings)),
		.str_count = cpu_to_le32(1),
		.lang_count = cpu_to_le32(1),
	},
	.lang0 = {
		cpu_to_le16(0x0409), /* en-us */
		STR_INTERFACE_,
	},
};

static struct list_head usb_handles = LIST_INIT(usb_handles);

struct usb_handle {
	int ep0;
	int bulk_out; /* "out" from the host's perspective => source for diagd */
	int bulk_in;  /* "in" from the host's perspective => sink for diagd */

	struct list_head node;
	int mux_id;
	int remote_id;
	bool usb_enable;
	bool bridge_enable;

	struct diag_client *dm;
	struct list_head outq;
	struct mbuf *out_buf;
	int (*recv)(struct mbuf *, void *);
};

struct diag_usb_info {
	int diag_state;
	struct usb_handle *ffs;

};
struct diag_usb_info diag_usb[3] =
{
	{
		.diag_state = 0,
	},
	{
		.diag_state = 0,
	},
	{
		.diag_state = 0,
	}
};

static int ffs_diag_init(const char *ffs_name, struct usb_handle *h)
{
	int ffs_fd;
	ssize_t n;
	int ret;

	ret = open(ffs_name, O_DIRECTORY);
	if (ret < 0) {
		warn("cannot open device folder %s", ffs_name);
		goto err_out;
	}
	ffs_fd = ret;

	ret = openat(ffs_fd, USB_FFS_EP0_NAME, O_RDWR);
	if (ret < 0) {
		warn("cannot open control endpoint");
		goto err_close_ffs;
	}
	h->ep0 = ret;

	n = write(h->ep0, &descriptors, sizeof(descriptors));
	if (n < 0) {
		warn("failed to write descriptors");
		ret = n;
		goto err_close_ep0;
	}

	n = write(h->ep0, &strings, sizeof(strings));
	if (n < 0) {
		warn("failed to write strings");
		ret = n;
		goto err_close_ep0;
	}

	ret = openat(ffs_fd, USB_FFS_OUT_NAME, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (ret < 0) {
		warn("cannot open bulk-out ep");
		goto err_close_ep0;
	}
	h->bulk_out = ret;

	ret = openat(ffs_fd, USB_FFS_IN_NAME, O_RDWR | O_NONBLOCK);
	if (ret < 0) {
		warn("cannot open bulk-in ep");
		goto err_close_bulk_out;
	}
	h->bulk_in = ret;

	close(ffs_fd);

	return 0;

err_close_bulk_out:
	close(h->bulk_out);
err_close_ep0:
	close(h->ep0);
err_close_ffs:
	close(ffs_fd);
err_out:
	return ret;
}

static int diag_ffs_recv(struct mbuf *mbuf, void *data)
{
	struct diag_pkt_frame_t *pkt_ptr;
	struct hdlc_decoder recv_decoder;
	struct circ_buf recv_buf;
	struct usb_handle *ffs = data;
	size_t msglen;
	void *msg;

	if (!mbuf || mbuf->offset >= HDLC_BUF_SIZE) {
		ALOGE("diag: %s invalid buf or invalid offset\n", __func__);
		list_add(&ffs->outq, &ffs->out_buf->node);
		return 0;
	}
	memset(&recv_decoder, 0, sizeof(recv_decoder));

	memcpy(recv_buf.buf, mbuf->data, mbuf->offset);
	recv_buf.tail = 0;
	recv_buf.head = mbuf->offset;

	if (diag_debug_mask & DIAG_DBG_MASK_USB)
		print_hex_dump("[USB:rx]", mbuf->data, MIN(mbuf->offset, 16));

	if (diagmem->hdlc_disabled) {
		pkt_ptr = (struct diag_pkt_frame_t *)mbuf->data;
		msglen = pkt_ptr->length;
		if (pkt_ptr->start != CONTROL_CHAR || *(uint8_t *)(pkt_ptr->data+pkt_ptr->length) != CONTROL_CHAR) {
			diag_start_hdlc_recovery(0, msglen);
		} else {
			diag_client_handle_command(ffs->dm, &pkt_ptr->data[0], msglen, 0);
		}
	} else {
		for (;;) {
			msg = hdlc_decode_one(&recv_decoder, &recv_buf, &msglen);
			if (!msg)
				break;

			// print_hex_dump("  [MSG]", msg, MIN(msglen, 256));
			diag_client_handle_command(ffs->dm, msg, msglen, 0);
		}
	}
	mbuf->offset = 0;
	list_add(&ffs->outq, &mbuf->node);

	return 0;
}

static int diag_ffs_recv_bridge(struct mbuf *mbuf, void *data)
{
	struct usb_handle *ffs = data;
	struct circ_buf recv_buf;

	if (!mbuf)
		return 0;

	memcpy(recv_buf.buf, mbuf->data, mbuf->offset);
	diag_mhi_write(&recv_buf.buf, mbuf->offset, ffs->remote_id);

	mbuf->offset = 0;
	list_add(&ffs->outq, &mbuf->node);

	return 0;
}

static void diag_ffs_enable(struct usb_handle *ffs)
{
	struct usb_endpoint_descriptor desc;
	int max_packetsize;

	if (!ffs)
		return;

	if (!ffs->usb_enable || !ffs->bridge_enable)
		return;

	/* initialize maximum packet size */
	desc.wMaxPacketSize = __cpu_to_le16(512);

	if (ioctl(ffs->bulk_in, FUNCTIONFS_ENDPOINT_DESC, (unsigned long)&desc))
		ALOGE("diag: Failed to get FFS bulk_in, init default max pkt size, errno:%d\n", errno);

	max_packetsize = __le16_to_cpu(desc.wMaxPacketSize);

	ALOGE("diag: %s: Add USB fd: %d with mux_id: %d to readq\n",
		__func__, ffs->bulk_out, ffs->mux_id);

	list_add(&ffs->outq, &ffs->out_buf->node);
	watch_add_readq(ffs->bulk_out, &ffs->outq, ffs->recv, ffs);
	if (!watch_check_for_fd(ffs->bulk_in))
		watch_add_writeq(ffs->bulk_in, &ffs->dm->outq, &ffs->dm->cmd_rsp_q, USB_FD);
	watch_set_maxpacket(ffs->bulk_in, max_packetsize);
	watch_set_use_iovec(ffs->bulk_in, true);
	dm_enable(ffs->dm);
}
#ifdef FEATURE_LE_DIAG
void diag_write_marker_to_sys(const char *marker)
{
	int fd = -1, ret = 0;

	errno = 0;

	fd = open(DIAG_MARKER_SYS_NODE, O_WRONLY);
	if (fd < 0) {
		ALOGE("diag: %s sysnode opening failed with errno %d\n", __func__, errno);
		return;
	}
	ret = write(fd, marker, strlen(marker));
	if (ret < 0)
		ALOGE("diag: %s sysnode writing failed with errno %d\n", __func__, errno);

	close(fd);
}
#endif
static int ep0_recv(int fd, void *data)
{
	struct usb_functionfs_event event;
	struct usb_handle *ffs = data;
	ssize_t n;

	n = read(fd, &event, sizeof(event));
	if (n <= 0) {
		warn("failed to read ffs ep0");
		return 0;
	}

	switch (event.type) {
	case FUNCTIONFS_ENABLE:
		ALOGD("diag: %s: received usb enable event for mux id: %d\n",
		      __func__, ffs->mux_id);
		if (dm_query_status(ffs->dm)) {
			watch_remove_readq(ffs->bulk_out);
			watch_remove_writeq(ffs->bulk_in);
		}
		ffs->usb_enable = true;
		diagmem->usb_connected = 1;
		diag_ffs_enable(ffs);
#ifdef FEATURE_LE_DIAG
		diag_write_marker_to_sys("M - Diag interface ready");
#endif /* FEATURE_LE_DIAG */
		break;
	case FUNCTIONFS_DISABLE:
	case FUNCTIONFS_UNBIND:
		ALOGD("diag: %s: received usb disable/unbind event(%d) for mux id :%d\n",
		      __func__, event.type, ffs->mux_id);
		ffs->usb_enable = false;
		watch_remove_readq(ffs->bulk_out);
		watch_remove_writeq(ffs->bulk_in);
		dm_disable(ffs->dm);
		diagmem->usb_connected = 0;
		if (diagmem->md_session_mode[DIAG_LOCAL_PROC] == DIAG_MD_NONE)
			diag_send_all_mask_clear(ffs->dm, 0);
		break;
	}

	return 0;
}

int diag_usb_bridge_enable(int fd, int remote_id)
{
	struct usb_handle *ffs;

	list_for_each_entry(ffs, &usb_handles, node) {
		if (ffs->ep0 == fd) {
			ffs->bridge_enable = true;
			ffs->remote_id = remote_id;
			diag_ffs_enable(ffs);

			ALOGI("diag: %s mux_id: %d fd: %d bulk_out: %d\n", __func__, ffs->mux_id, fd, ffs->bulk_out);
			return 0;
		}
	}

	return -1;
}

int diag_usb_bridge_disable(int fd)
{
	struct usb_handle *ffs;

	list_for_each_entry(ffs, &usb_handles, node) {
		if (ffs->ep0 == fd) {
			ffs->bridge_enable = false;
			ffs->remote_id = -1;
			watch_remove_readq(ffs->bulk_out);
			watch_remove_writeq(ffs->bulk_in);
			dm_disable(ffs->dm);

			ALOGI("diag: %s mux_id: %d fd: %d bulk_out: %d\n", __func__, ffs->mux_id, fd, ffs->bulk_out);
			return 0;
		}
	}

	return -1;
}

int __diag_usb_open(const char *ffs_name)
{
	struct usb_handle *ffs;
	int ret;

	ffs = calloc(1, sizeof(struct usb_handle));
	if (!ffs)
		err(1, "couldn't allocate usb_handle");

	ffs->out_buf = mbuf_alloc(16384);
	if (!ffs->out_buf)
		err(1, "couldn't allocate usb out buffer");

	ret = ffs_diag_init(ffs_name, ffs);
	if (ret < 0) {
		free(ffs->out_buf);
		ffs->out_buf = NULL;
		free(ffs);
		ffs = NULL;
		return -1;
	}
	ffs->out_buf->free_flag = 1;

	list_init(&ffs->outq);
	list_init(&ffs->out_buf->node);

	if (!strcmp(ffs_name, "/dev/ffs-diag")){
		ffs->dm = dm_add("USB client", -1, ffs->bulk_in, true);
		ffs->mux_id = 0;
		ffs->bridge_enable = true;
		ffs->recv = diag_ffs_recv;
		diag_usb[0].ffs = ffs;
		printf("diag: adding usb client");
	} else if (!strcmp(ffs_name, "/dev/ffs-diag-1")) {
		/* MDM USB Enumeration */
		ffs->dm = dm_add("USB client 1", -1, ffs->bulk_in, true);
		ffs->mux_id = 1;
		ffs->bridge_enable = false;
		ffs->recv = diag_ffs_recv_bridge;
		diag_usb[1].ffs = ffs;
		printf("diag: usb client 1");
	} else if (!strcmp(ffs_name, "/dev/ffs-diag-2")) {
		/* MDM2 (WLAN) USB Enumeration */
		ffs->dm = dm_add("USB client 2", -1, ffs->bulk_in, true);
		ffs->mux_id = 2;
		ffs->bridge_enable = false;
		ffs->recv = diag_ffs_recv_bridge;
		diag_usb[2].ffs = ffs;
		printf("diag: usb client 2");
	}
	printf("diag: adding recv for ep0%d %p\n", ffs->ep0, ffs);
	watch_add_readfd(ffs->ep0, ep0_recv, ffs, NULL);

	list_add(&usb_handles, &ffs->node);

	return ffs->ep0;
}

int diag_usb_open(const char *ffs_name)
{
	int ret;

	if (vm_enabled)
		return 0;

	ret = __diag_usb_open(ffs_name);
	if (ret < 0)
		return ret;

	return 0;
}
int diag_usb_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag)
{

	if (diag_debug_mask & DIAG_DBG_MASK_USB && cmd_rsp_flag)
		print_hex_dump("[USB:wr]", buf, MIN(len, 16));

	if (proc) {
		if (diag_usb[proc].ffs->usb_enable)
			queue_push(dm, &diag_usb[proc].ffs->dm->outq, buf, len);
	} else {
		if (diagmem->p_hdlc_disabled[peripheral]) {
			if (cmd_rsp_flag)
				queue_push_flow(dm, &diag_usb[proc].ffs->dm->cmd_rsp_q, buf, len, flow);
			else
				queue_push_flow(dm, &diag_usb[proc].ffs->dm->outq, buf, len, flow);
		} else
			if (diag_usb[proc].ffs->usb_enable)
				dm_send_flow(dm, diag_usb[proc].ffs->dm, buf, len, flow, cmd_rsp_flag);
	}
	return 0;
}

void diag_usb_mux_switch_cleanup(void)
{
	struct usb_handle *ffs;

	ffs = diag_usb[0].ffs;

	watch_cancel_writeq(ffs->bulk_in);
}

int diag_usb_disconnect(int proc)
{
	struct usb_handle *ffs;
	int i;

	ffs = diag_usb[proc].ffs;
	if (ffs) {
		ALOGD("diag: received usb disconnect for proc: %d\n", proc);
		watch_remove_readq(ffs->bulk_out);
		watch_remove_writeq(ffs->bulk_in);
		dm_disable(ffs->dm);
		if (diagmem->md_session_mode[DIAG_LOCAL_PROC] == DIAG_MD_NONE) {
			diagmem->hdlc_disabled = 0;
			/*
			 * HDLC encoding is re-enabled when
			 * there is logical/physical disconnection of diag
			 * to USB.
			 */
			ALOGE("diag: In %s Enabling HDLC encoding post usb disconnect\n", __func__);
			for (i = 0; i < NUM_MD_SESSIONS; i++)
				diagmem->p_hdlc_disabled[i] =
					diagmem->hdlc_disabled;
		}

	}

	return 0;
}
int diag_usb_connect(int proc)
{
	struct usb_handle *ffs;

	ffs = diag_usb[proc].ffs;
	if (ffs) {
		ALOGD("diag: received usb connect for proc: %d\n", proc);
		diag_ffs_enable(ffs);
		diagmem->transport_set = DIAG_ROUTE_TO_USB;
	}

	return 0;

}

int diag_usb_disconnect_all()
{
	struct usb_handle *ffs;
	int i;

	for (i = 0; i < 3; i++) {
		ffs = diag_usb[i].ffs;
		watch_remove_readq(ffs->bulk_out);
		watch_remove_writeq(ffs->bulk_in);
		dm_disable(ffs->dm);
	}

	return 0;
}
int diag_usb_connect_all()
{
	struct usb_handle *ffs;
	int i;

	for (i = 0; i < 3; i++) {
		ffs = diag_usb[i].ffs;
		diag_ffs_enable(ffs);
	}

	return 0;
}
