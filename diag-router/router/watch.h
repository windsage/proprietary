/*
 * Copyright (c) 2016, Bjorn Andersson <bjorn@kryo.se>
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
#ifndef __WATCH_H__
#define __WATCH_H__

#include <stdbool.h>
#include "list.h"

#define DATA_FD			1
#define CNTL_FD			2
#define DCI_FD			3
#define CMD_FD			4
#define DCI_CMD_FD		5
#define UNIX_CLIENT_FD		6
#define USB_FD			7

#define AIO_SUBMIT		0
#define HANDLE_EVENTFD		1

#ifdef __cplusplus
extern "C" {
#endif

struct mbuf;

struct watch_flow;

struct watch_flow *watch_flow_new(void);
void watch_reset_flow(struct watch_flow *flow);
void watch_flow_inc(struct watch_flow *flow);
struct watch_flow *watch_flow_get(struct watch_flow *flow);
void watch_flow_put(struct watch_flow *flow);
int watch_add_readfd(int fd, int (*cb)(int, void*), void *data, struct watch_flow *flow);
int watch_add_readq(int fd, struct list_head *queue,
		    int (*cb)(struct mbuf *mbuf, void *data), void *data);
int watch_add_writeq(int fd, struct list_head *queue, struct list_head *cmd_rsp_queue, int fd_type);
void watch_remove_fd(int fd);
void watch_remove_writeq(int fd);
void watch_cancel_writeq(int fd);
void watch_remove_readq(int fd);
int watch_add_quit(int (*cb)(int, void*), void *data);
int watch_add_timer(void (*cb)(void *), void *data,
		    unsigned int interval, bool repeat);
void watch_quit(void);
void watch_run(void);
int watch_check_for_fd(int fd);
void watch_set_maxpacket(int fd, int max_packetsize);
void watch_set_use_iovec(int fd, bool use_iovec);

#ifdef __cplusplus
}
#endif

#endif
