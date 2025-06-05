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
#ifndef __DM_H__
#define __DM_H__

#include "diag.h"

struct diag_client;

struct diag_client *dm_add(const char *name, int in_fd, int out_fd, bool hdlc_encoded);
int dm_del(int fd);
int pcie_dm_recv(int fd, void* data);
int dm_recv(int fd, void* data);
ssize_t dm_send(struct diag_client *dm, const void *ptr, size_t len);
void dm_broadcast_to_socket_clients(const void *ptr, size_t len);
void dm_send_to_pid(const void *ptr, size_t len, int pid);
void dm_enable(struct diag_client *dm);
void dm_disable(struct diag_client *dm);
int dm_query_status(struct diag_client *dm);
ssize_t dm_send_flow(struct diag_client *unix_dm, struct diag_client *sink_dm, const void *ptr, size_t len,
			    struct watch_flow *flow, int cmd_rsp_flag);
void dm_update_diag_id(struct diag_client *dm);
void diag_set_clear_masks_flags(int state);
int diag_send_all_mask_clear(struct diag_client *client, int pid);
#ifdef __cplusplus
extern "C" {
#endif
void dm_set_pid(struct diag_client *dm, int pid);
void dm_set_override_pid(struct diag_client *dm, int pid);
void dm_broadcast(const void *ptr, size_t len,struct watch_flow *flow, int cmd_rsp_flag);
#ifdef __cplusplus
}
#endif
#endif

