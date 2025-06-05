/*
 * Copyright (c) 2018, Linaro Ltd.
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
#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "diag.h"
#include "dm.h"
#include "watch.h"
#include "masks.h"

/* strlcpy is from OpenBSD and not supported by Linux Embedded.
 * GNU has an equivalent g_strlcpy implementation into glib.
 * Featurized with compile time USE_GLIB flag for Linux Embedded builds.
 */

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif
int socket_counter = 0;
struct list_head client_list = LIST_INIT(client_list);
static int unix_listen(int fd, void *data)
{
	struct diag_pkt_format_request pkt;
	struct diag_client *dm;
	struct ucred scred;
	socklen_t len;
	int client;
	int ret;
	(void)data;

	client = accept(fd, NULL, NULL);
	if (client < 0) {
		ALOGE("diag: %s failed to accept client socket connection\n", __func__);
		return 0;
	}

	ret = fcntl(client, F_SETFL, O_NONBLOCK);
	if (ret < 0) {
		ALOGE("diag: %s failed to set O_NONBLOCK\n", __func__);
		return 0;
	}

	/* get the calling process ID */
	len = sizeof(scred);
	ret = getsockopt(client, SOL_SOCKET, SO_PEERCRED, &scred, &len);
	if (ret) {
		ALOGE("diag: %s: failed to get socket option: %d\n", __func__, errno);
		return 0;
	}
	dm = dm_add("UNIX", client, client, false);
	if (dm) {
		socket_counter++;
		dm_set_pid(dm, scred.pid);
		dm_enable(dm);
		/**
		 * pkt format select request & diag-id update limited to only
		 * vm diag clients to limit the new async packet usage within
		 * vm. Ignore the 'vm_enabled' condition check to extend the use
		 * for pvm as well
		 */
		if (vm_enabled) {
			if (pkt_format_select_check_mask(PKT_FORMAT_ALL_MASK)) {
				/* send pkt format select request to vm diag client */
				pkt.type = DIAG_PKT_FORMAT_SELECT_TYPE;
				pkt.mask = pkt_format_select_get_mask();
				dm_send(dm, (void*)&pkt, sizeof(pkt));
			}
			dm_update_diag_id(dm);
		}
		diag_msg_mask_update_client(dm);
		diag_log_mask_update_client(dm);
		diag_event_mask_update_client(dm);
		diag_timestamp_switch_update_client(dm);
		ALOGE("diag: %s: dm with pid: %d and fd: %d is registered, socket_counter: %d\n", __func__, dm->pid, client, socket_counter);
	} else {
		ALOGE("diag: %s: dm is not valid socket_counter: %d\n", __func__, socket_counter);
	}

	return 0;
}

int diag_unix_open(void)
{
	struct sockaddr_un addr;
	int ret;
	int fd;

	fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (fd < 0) {
		fprintf(stderr, "failed to create unix socket");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strlcpy(addr.sun_path, "\0diag", sizeof(addr.sun_path)-1);
	ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0) {
		fprintf(stderr, "failed to bind diag socket");
		goto err;
	}

	ret = listen(fd, 100);
	if (ret < 0) {
		fprintf(stderr, "failed to listen on diag socket\n");
		goto err;
	}

	watch_add_readfd(fd, unix_listen, NULL, NULL);

	return 0;
err:
	close(fd);
	return -1;
}
