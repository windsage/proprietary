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
#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>

#include <linux/aio_abi.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "list.h"
#include "mbuf.h"
#include "util.h"
#include "watch.h"
#include "diag.h"
#define FLOW_WATERMARK	10
#define MAX_AIO_SUBMIT	32
#define MAX_IOVEC_NENTRIES	256
#define MAX_IOVEC_NBYTES	16384

#define MAX_STR_LEN		20
#define MAX_PRINT_INDEX		2

#define WATCH_MAX_PKT_RETRY	5

typedef unsigned long aio_context_t;

pthread_mutex_t aio_free_mutex;

void glinkpkt_handle_eventfd(int evfd);

struct watch_flow {
	int packets;
	int ref_count;
};

struct watch {
	int fd;
	int (*cb)(int, void*);
	void *data;
	struct list_head *queue;
	struct list_head *cmd_rsp_queue;

	aio_context_t ioctx;
	struct iocb iocbs[MAX_AIO_SUBMIT];
	int idx;

	struct list_head pending_aio;
	int pending_count;
	int rsp_pending_count;
	int retry_count;

	int max_packetsize;
	int fd_type;
	bool is_write;
	bool use_iovec;
	struct watch_flow *flow;
	int (*aio_complete)(struct mbuf *, void*);

	struct list_head node;
};

struct free_read_watch {
	struct watch *watch;
	struct list_head node;
};

struct timer {
	void (*cb)(void *);
	void *data;
	unsigned int interval;
	bool repeat;

	struct timeval tick;

	struct list_head node;
};

static struct list_head timers = LIST_INIT(timers);

static struct list_head read_watches = LIST_INIT(read_watches);
static struct list_head aio_watches = LIST_INIT(aio_watches);
static struct list_head free_watches = LIST_INIT(free_watches);
static struct list_head quit_watches = LIST_INIT(quit_watches);
static struct list_head free_read_watches = LIST_INIT(free_read_watches);
static bool do_watch_quit;
static void watch_put_iocb(struct iocb *iocb);

static long io_destroy(aio_context_t ctx)
{
	return syscall(__NR_io_destroy, ctx);
}

static long io_getevents(aio_context_t ctx, long min_nr, long nr,
			 struct io_event *events, struct timespec *tmo)
{
	return syscall(__NR_io_getevents, ctx, min_nr, nr, events, tmo);
}

static long io_setup(unsigned nr_reqs, aio_context_t *ctx)
{
	return syscall(__NR_io_setup, nr_reqs, ctx);
}

static long io_submit(aio_context_t ctx, long n, struct iocb **paiocb)
{
	return syscall(__NR_io_submit, ctx, n, paiocb);
}

static long io_cancel(aio_context_t ctx, struct iocb *iocb, struct io_event *res)
{
	return syscall(__NR_io_cancel, ctx, iocb, res);
}

void watch_debug_print(const char *func_str, struct watch *w, struct mbuf *mbuf, int flag)
{
	if (!(diag_debug_mask & DIAG_DBG_MASK_CMD))
		return;

	char dst_str[MAX_PRINT_INDEX][MAX_STR_LEN] = {"peripheral", "sink"};
	char dbg_str[MAX_PRINT_INDEX][MAX_STR_LEN] = {"to be written", "write success"};
	int index = 0;

	if (w->fd_type == CMD_FD) {
		index = 0;
	} else if (mbuf->rsp_flag) {
		index = 1;
	} else {
		return;
	}
	ALOGM(DIAG_DBG_MASK_WATCH, "diag: %s: pkt of len: %d to %s %s\n", func_str, mbuf->size, dst_str[index], dbg_str[flag]);
	print_hex_dump(func_str, mbuf->data, MIN(mbuf->size, PRINT_HEX_DUMP_LEN));
}

struct watch_flow *watch_flow_new(void)
{
	struct watch_flow *flow;

	flow = calloc(1, sizeof(struct watch_flow));
	if (!flow)
		return NULL;

	flow->ref_count++;
	return flow;
}

void watch_reset_flow(struct watch_flow *flow)
{
	if (!flow)
		return;

	flow->packets = 0;
}

void watch_flow_inc(struct watch_flow *flow)
{
	if (!flow)
 		return;

	flow->packets++;
}

static void watch_flow_dec(struct watch_flow *flow)
{
	if (!flow)
		return;

	if (!flow->packets)
		fprintf(stderr, "unbalanced flow control\n");
	else
		flow->packets--;
}

static bool watch_flow_blocked(struct watch_flow *flow)
{
	return flow && flow->packets > FLOW_WATERMARK;
}

struct watch_flow *watch_flow_get(struct watch_flow *flow)
{
	if (!flow)
		return NULL;

	flow->ref_count++;
	return flow;
}

void watch_flow_put(struct watch_flow *flow)
{
	if (!flow)
		return;

	if (!flow->ref_count)
		ALOGE("diag: %s: unbalanced flow ref count\n", __func__);

	flow->ref_count--;
	if (!flow->ref_count)
		free(flow);
}

int watch_check_for_fd(int fd)
{
	struct list_head *item;
	struct list_head *next;
	struct watch *w;

	list_for_each_safe_with_lock(item, next, &aio_watches) {
		w = container_of(item, struct watch, node);
		if (w->fd == fd){
			LIST_UNLOCK(&aio_watches);/*lock is taken in list_for_each_safe_with_lock */
			return 1;
		}
	}
	LIST_UNLOCK(&aio_watches);/*lock is taken in list_for_each_safe_with_lock */
	return 0;
}

void watch_free_watches(void)
{
	struct watch *temp;
	struct watch *w;
	struct iocb *iocb;
	bool pending;
	int i;

	list_for_each_entry_safe_with_lock(w, temp, &free_watches, node) {
		pending = false;
		for (i = 0; i < MAX_AIO_SUBMIT; i++) {
			iocb = &w->iocbs[i];
			if (iocb->aio_data || iocb->aio_buf) {
				pending = true;
				break;
			}
		}
		if (!pending) {
			list_del(&w->node);
			free(w);
		}
	}
	LIST_UNLOCK(&free_watches);
}

void watch_free_read_watches(void)
{
	struct free_read_watch *temp;
	struct free_read_watch *w;

	list_for_each_entry_safe_with_lock(w, temp, &free_read_watches, node) {
		list_del(&w->node);
		watch_flow_put(w->watch->flow);
		free(w->watch);
		w->watch = NULL;
		free(w);
		w = NULL;
	}
	LIST_UNLOCK(&free_read_watches);
}

int watch_add_readfd(int fd, int (*cb)(int, void*), void *data,
		     struct watch_flow *flow)
{
	struct watch *w;

	w = calloc(1, sizeof(struct watch));
	if (!w)
		err(1, "calloc");

	w->fd = fd;
	w->cb = cb;
	w->data = data;
	w->flow = watch_flow_get(flow);
	w->use_iovec = false;
	list_add(&read_watches, &w->node);

	return 0;
}

int watch_add_readq(int fd, struct list_head *queue,
		    int (*cb)(struct mbuf *mbuf, void *data), void *data)
{
	struct watch *w;

	w = calloc(1, sizeof(*w));
	if (!w)
		err(1, "calloc");

	w->fd = fd;
	w->aio_complete = cb;
	w->data = data;
	w->queue = queue;
	w->cmd_rsp_queue = NULL;
	w->fd_type = 0;
	list_init(&w->pending_aio);

	w->is_write = false;
	w->use_iovec = false;
	ALOGM(DIAG_DBG_MASK_WATCH, "diag: add readq fd: %d to aio watches\n", w->fd);
	list_add(&aio_watches, &w->node);

	return 0;
}

void watch_remove_readq(int fd)
{
	struct list_head *item;
	struct list_head *next;
	struct io_event res;
	struct watch *w;
	struct iocb *iocb;
	int i;

	list_for_each_safe_with_lock(item, next, &aio_watches) {
		w = container_of(item, struct watch, node);
		if (w->fd == fd) {
			for (i = 0; i < MAX_AIO_SUBMIT; i++) {
				iocb = &w->iocbs[i];
				if (iocb->aio_data || iocb->aio_buf)
					io_cancel(w->ioctx, iocb, &res);
			}
			ALOGM(DIAG_DBG_MASK_WATCH,
			      "diag: remove readq fd: %d from aio watches\n", w->fd);
			list_del(&w->node);
			w->queue = NULL;
			list_add(&free_watches, &w->node);
		}
	}
	LIST_UNLOCK(&aio_watches);
}

static int watch_free_write_aio(struct mbuf *mbuf, void *data)
{
	(void)data;

	pthread_mutex_lock(&aio_free_mutex);
	if (!mbuf) {
		pthread_mutex_unlock(&aio_free_mutex);
		return -1;
	}

	watch_flow_dec(mbuf->flow);
	watch_flow_put(mbuf->flow);
	if (mbuf->free_flag) {
		memset(mbuf, 0, sizeof(*mbuf) + mbuf->size);
		mbuf->free_flag = 1;
	}
	mbuf->busy_state = 0;
	if (!mbuf->free_flag) {
		free(mbuf);
		mbuf = NULL;
	}
	pthread_mutex_unlock(&aio_free_mutex);
	return 0;
}

static void watch_writeq_free(struct watch *w)
{
	struct io_event res;
	struct iocb *iocb;
	struct mbuf *mbuf = NULL;
	struct iovec *iovec = NULL;
	int i, j, ret;

	for (i = 0; i < MAX_AIO_SUBMIT; i++) {
		iocb = &w->iocbs[i];
		if (!(iocb->aio_data || iocb->aio_buf))
			continue;

		ret = io_cancel(w->ioctx, iocb, &res);
		if (ret < 0) {
			ALOGM(DIAG_DBG_MASK_WATCH,
			      "diag: %s: io_cancel failed with ret: %d fd: %d\n",
			       __func__, ret, w->fd);
			continue;
		}

		if (iocb->aio_data) {
			mbuf = (struct mbuf *)iocb->aio_data;
			if (mbuf){
				watch_put_iocb(iocb);
				LIST_LOCK(w->queue);
				list_del(&mbuf->node);
				LIST_UNLOCK(w->queue);
				watch_free_write_aio(mbuf, w);
			}
		} else if (iocb->aio_buf) {
			iovec = (struct iovec*)iocb->aio_buf;
			for (j = 0; j < iocb->aio_nbytes; j++) {
				mbuf = container_of(iovec[j].iov_base, struct mbuf, data);
				if (mbuf){
					watch_put_iocb(iocb);
					LIST_LOCK(w->queue);
					list_del(&mbuf->node);
					LIST_UNLOCK(w->queue);
					watch_free_write_aio(mbuf, w);
				}
			}
		}
	}
}

int watch_add_writeq(int fd, struct list_head *queue, struct list_head *cmd_rsp_queue, int fd_type)
{
	struct watch *w;

	w = calloc(1, sizeof(*w));
	if (!w)
		err(1, "calloc");

	w->fd = fd;
	w->queue = queue;
	w->cmd_rsp_queue = cmd_rsp_queue;
	w->data = w;
	list_init(&w->pending_aio);

	w->aio_complete = watch_free_write_aio;

	w->is_write = true;
	w->use_iovec = false;
	w->fd_type = fd_type;
	list_push(&aio_watches, &w->node);
	ALOGM(DIAG_DBG_MASK_WATCH,
	      "diag: %s: added fd: %d to aio watches\n", __func__, fd);

	return 0;
}

void watch_set_maxpacket(int fd, int max_packetsize)
{
	struct list_head *item;
	struct list_head *next;
	struct watch *w;

	list_for_each_safe_with_lock(item, next, &aio_watches) {
		w = container_of(item, struct watch, node);
		if (w->fd == fd) {
			ALOGD("diag: setting max_packetsize %d for %d\n",
				max_packetsize, w->fd);
			w->max_packetsize = max_packetsize;
			LIST_UNLOCK(&aio_watches);/*lock is taken in list_for_each_safe_with_lock */
			return;
		}
	}
	LIST_UNLOCK(&aio_watches);/*lock is taken in list_for_each_safe_with_lock */

}

void watch_set_use_iovec(int fd, bool use_iovec)
{
	struct watch *w;

	list_for_each_entry(w, &aio_watches, node) {
		if (w->fd == fd) {
			w->use_iovec = use_iovec;
			ALOGI("diag: %s use iovec for fd: %d\n", use_iovec ? "" : "Do not", w->fd);
			return;
		}
	}
}

static inline void watch_free_mbuf_list(struct watch *w)
{
	struct mbuf *mbuf = NULL;

	while (w->queue && !list_empty(w->queue)) {
		mbuf = list_entry_first(w->queue, struct mbuf, node);
		LIST_LOCK(w->queue);
		list_del(&mbuf->node);
		LIST_UNLOCK(w->queue);
		watch_free_write_aio(mbuf, NULL);
	}
	while (w->cmd_rsp_queue && !list_empty(w->cmd_rsp_queue)) {
		mbuf = list_entry_first(w->cmd_rsp_queue, struct mbuf, node);
		LIST_LOCK(w->cmd_rsp_queue);
		list_del(&mbuf->node);
		LIST_UNLOCK(w->cmd_rsp_queue);
		watch_free_write_aio(mbuf, NULL);
	}

}

void watch_remove_fd(int fd)
{
	struct list_head *item;
	struct list_head *next;
	struct watch *w;
	struct io_event res;
	struct iocb *iocb;
	struct free_read_watch *free_read_w;
	int i;

	free_read_w = calloc(1, sizeof(struct free_read_watch));
	if (!free_read_w)
		err(1, "calloc");

	list_for_each_safe_with_lock(item, next, &read_watches) {
		w = container_of(item, struct watch, node);
		if (w->fd == fd) {
			list_del(&w->node);
			free_read_w->watch = w;
			list_add(&free_read_watches, &free_read_w->node);
		}
	}
	LIST_UNLOCK(&read_watches);/*lock is taken in list_for_each_safe_with_lock */

	list_for_each_safe_with_lock(item, next, &aio_watches) {
		w = container_of(item, struct watch, node);
		if (w->fd == fd) {
			for (i = 0; i < MAX_AIO_SUBMIT; i++) {
				iocb = &w->iocbs[i];
				if (iocb->aio_data || iocb->aio_buf)
					io_cancel(w->ioctx, &w->iocbs[i], &res);
			}
			ALOGM(DIAG_DBG_MASK_WATCH,
			      "diag: remove fd: %d from aio watches\n", w->fd);
			list_del(&w->node);
			list_add(&free_watches, &w->node);
			if (w->is_write) {
				watch_writeq_free(w);
				watch_free_mbuf_list(w);
			}
			w->queue = NULL;
			w->cmd_rsp_queue = NULL;
		}
	}
	LIST_UNLOCK(&aio_watches);/*lock is taken in list_for_each_safe_with_lock */
}

void watch_remove_writeq(int fd)
{
	struct list_head *item;
	struct list_head *next;
	struct watch *w;
	struct io_event res;
	struct iocb *iocb;
	int i;

	list_for_each_safe_with_lock(item, next, &aio_watches) {
		w = container_of(item, struct watch, node);
		if (w->fd == fd) {
			for (i = 0; i < MAX_AIO_SUBMIT; i++) {
				iocb = &w->iocbs[i];
				if (iocb->aio_data || iocb->aio_buf)
					io_cancel(w->ioctx, iocb, &res);
			}
			ALOGM(DIAG_DBG_MASK_WATCH,
			      "diag: remove writeq fd: %d from aio watches\n", w->fd);
			list_del(&w->node);
			list_add(&free_watches, &w->node);
			if (w->is_write) {
				watch_writeq_free(w);
				watch_free_mbuf_list(w);
			}
			w->queue = NULL;
			w->cmd_rsp_queue = NULL;
		}
	}
	LIST_UNLOCK(&aio_watches);/*lock is taken in list_for_each_safe_with_lock */

}

void watch_cancel_writeq(int fd)
{
	struct list_head *item;
	struct list_head *next;
	struct watch *w;
	struct mbuf *mbuf = NULL;
	struct io_event res;
	struct iocb *iocb;
	int i;

	list_for_each_safe_with_lock(item, next, &aio_watches) {
		w = container_of(item, struct watch, node);
		if (w->fd == fd) {
			for (i = 0; i < MAX_AIO_SUBMIT; i++) {
				iocb = &w->iocbs[i];
				if (iocb->aio_data || iocb->aio_buf)
					io_cancel(w->ioctx, iocb, &res);
			}
			if (w->is_write) {
				watch_writeq_free(w);
				while (!list_empty(w->queue)) {
					mbuf = list_entry_first(w->queue, struct mbuf, node);
					LIST_LOCK(w->queue);
					list_del(&mbuf->node);
					LIST_UNLOCK(w->queue);
					watch_free_write_aio(mbuf, NULL);
				}
			}
		}
	}
	LIST_UNLOCK(&aio_watches);/*lock is taken in list_for_each_safe_with_lock */

}

int watch_add_quit(int (*cb)(int, void*), void *data)
{
	struct watch *w;

	w = calloc(1, sizeof(struct watch));
	if (!w)
		err(1, "calloc");

	w->cb = cb;
	w->data = data;

	list_add(&quit_watches, &w->node);

	return 0;
}

static void watch_set_timer(struct timer *timer)
{
	struct timeval now;
	struct timeval tv;
	int ret;

	ret = gettimeofday(&now, NULL);
	if (ret < 0)
		err(1, "failed to gettimeofday");

	tv.tv_sec = timer->interval / 1000;
	tv.tv_usec = (timer->interval % 1000) * 1000;

	timeradd(&now, &tv, &timer->tick);
}

int watch_add_timer(void (*cb)(void *), void *data, unsigned int interval,
		    bool repeat)
{
	struct timer *w;

	w = calloc(1, sizeof(struct timer));
	if (!w)
		err(1, "calloc");

	w->cb = cb;
	w->data = data;
	w->interval = interval;
	w->repeat = repeat;

	list_add(&timers, &w->node);

	watch_set_timer(w);

	return 0;
}

static void watch_free_timer(struct timer *timer)
{
	list_del(&timer->node);
	free(timer);
	timer = NULL;
}

static struct timer *watch_get_next_timer(void)
{
	struct timeval tv;
	struct timer *selected;
	struct timer *timer;

	if (list_empty(&timers))
		return NULL;

	selected = container_of(timers.next, struct timer, node);
	tv = selected->tick;

	list_for_each_entry(timer, &timers, node) {
		if (timercmp(&timer->tick, &tv, <)) {
			selected = timer;
			tv = timer->tick;
		}
	}

	return selected;
}

void watch_quit(void)
{
	do_watch_quit = true;
}

static struct iocb *watch_get_iocb(struct watch *w)
{
	struct iocb* iocb;
	int i;

	for (i = w->idx; i < w->idx + MAX_AIO_SUBMIT; i++) {
		iocb = &w->iocbs[i % MAX_AIO_SUBMIT];
		if (iocb->aio_data || iocb->aio_buf)
			continue;

		w->idx = (i + 1) % MAX_AIO_SUBMIT;
		return iocb;
	}
	return NULL;
}

static void watch_put_iocb(struct iocb *iocb)
{
	memset(iocb, 0, sizeof(*iocb));
}

static void watch_submit_aio(aio_context_t ioctx, int evfd, struct watch *w)
{
	struct iocb *iocb;
	struct mbuf *mbuf = NULL;
	int ret, rsp_flag = 0;
	struct list_head *submit_queue;
	struct iovec *iovec = NULL;
	int iovec_nentries = 0, iovec_nbytes = 0;
	int i, packetsize = 0;
	bool is_iovec;

	while (((w->queue && !list_empty(w->queue)) ||
		(w->cmd_rsp_queue && !list_empty(w->cmd_rsp_queue))) &&
		((w->pending_count + w->rsp_pending_count) < MAX_AIO_SUBMIT)) {
		mbuf = NULL;

		if (w->cmd_rsp_queue &&  !list_empty(w->cmd_rsp_queue)) {
			submit_queue = w->cmd_rsp_queue;
			rsp_flag = 1;
		} else {
			submit_queue = w->queue;
		}
		LIST_LOCK(submit_queue);

		iocb = watch_get_iocb(w);
		if (!iocb) {
			ALOGD("diag: In %s invalid iocb\n", __func__);
			LIST_UNLOCK(submit_queue);
			return;
		}

		/*
		 * The following buffer types cannot be aggregated:
		 *
		 * 1) FD does not support aggregation
		 *
		 * 2) Command and response
		 *
		 * 3) Has a size of 0 byte. This is so recipients such as
		 * USB know it is the end of a transfer
		 *
		 * 4) Has a size of MAX_IOVEC_NBYTES or greater
		 */
		mbuf = list_entry_first(submit_queue, struct mbuf, node);
		if (!w->use_iovec || rsp_flag || !mbuf->size || mbuf->size >= MAX_IOVEC_NBYTES) {
			is_iovec = false;
			mbuf->rsp_flag = rsp_flag;
			iovec_nentries = 1;
			packetsize = mbuf->size;
			iocb->aio_lio_opcode = w->is_write ? IOCB_CMD_PWRITE : IOCB_CMD_PREAD;
			iocb->aio_buf = (uintptr_t)mbuf->data;
			iocb->aio_nbytes = packetsize;
			iocb->aio_data = (uintptr_t)mbuf;
		} else {
			is_iovec = true;
			iovec_nentries = 0;
			iovec_nbytes = 0;

			iovec = calloc(MAX_IOVEC_NENTRIES, sizeof(struct iovec));
			if (!iovec) {
				ALOGE("diag: In %s failed to allocate memory for iovec for fd: %d\n",
				      __func__, w->fd);
				LIST_UNLOCK(submit_queue);
				return;
			}

			list_for_each_entry(mbuf, submit_queue, node) {
				/*
				 * Cannot aggregate the current mbuf if its size is 0 or greater
				 * than or equal to MAX_IOVEC_NBYTES.
				 */
				if (iovec_nbytes &&
				    (!mbuf->size || iovec_nbytes + mbuf->size >= MAX_IOVEC_NBYTES))
					break;

				iovec_nbytes += mbuf->size;

				mbuf->rsp_flag = rsp_flag;

				iovec[iovec_nentries].iov_base = mbuf->data;
				iovec[iovec_nentries].iov_len = mbuf->size;

				if (++iovec_nentries >= MAX_IOVEC_NENTRIES)
					break;
			}

			packetsize = iovec_nbytes;
			iocb->aio_lio_opcode = w->is_write ? IOCB_CMD_PWRITEV : IOCB_CMD_PREADV;
			iocb->aio_buf = (uintptr_t)iovec;
			iocb->aio_nbytes = iovec_nentries;
			iocb->aio_data = rsp_flag;
		}
		LIST_UNLOCK(submit_queue);

		iocb->aio_fildes = w->fd;
		iocb->aio_offset = 0;
		iocb->aio_flags = IOCB_FLAG_RESFD;
		iocb->aio_resfd = evfd;
		if (diag_debug_mask & DIAG_DBG_MASK_WATCH)
			watch_debug_print(__func__, w, mbuf, AIO_SUBMIT);
		ret = io_submit(ioctx, 1, &iocb);
		if (ret != 1) {
			if (errno != EAGAIN)
				ALOGE("diag: io_submit failed: %d (%d) fd %d\n", ret, errno, w->fd);
			watch_put_iocb(iocb);
			return;
		}

		if (ret == 1) {
			for (i = 0; i < iovec_nentries; i++) {
				/* get mbuf for each iovec. For non-iovec, already have the mbuf */
				if (is_iovec)
					mbuf = container_of(iovec[i].iov_base, struct mbuf, data);

				LIST_LOCK(submit_queue);
				list_del(&mbuf->node);
				LIST_UNLOCK(submit_queue);
				list_add(&w->pending_aio, &mbuf->node);
			}

			if (rsp_flag) {
				w->rsp_pending_count++;
				rsp_flag = 0;
			} else {
				w->pending_count++;
			}
			w->ioctx = ioctx;

			if (w->max_packetsize && packetsize &&
					(packetsize % w->max_packetsize == 0)) {
				ALOGE("diag: wrote packet of %d bytes on fd%d, queueing ZLP\n",
						packetsize, w->fd);
				mbuf = mbuf_alloc(0);
				if (!mbuf) {
					ALOGE("diag: In %s failed to allocate memory\n", __func__);
					return;
				}
				list_init(&mbuf->node);
				mbuf->free_flag = 0;
				list_push(w->queue, &mbuf->node);
			}
			if (w->fd_type == CNTL_FD) {
				return;
			}
		}
	}
}

static void watch_handle_eventfd(int evfd, aio_context_t ioctx)
{
	struct io_event ev[32];
	struct iocb *iocb;
	struct mbuf *mbuf = NULL;
	struct watch *next;
	struct watch *w;
	struct iovec *iovec_head, *iovec;
	int iovec_nentries;
	uint64_t evcnt;
	ssize_t n;
	int count, rsp_flag = 0;
	int i, j;
	bool is_iovec;
	bool retry;

	n = read(evfd, &evcnt, sizeof(evcnt));
	if (n < 0) {
		ALOGE("diag: %s failed to read eventfd counter\n", __func__);
		return;
	}

	count = io_getevents(ioctx, 0, 32, ev, NULL);
	list_for_each_entry_safe_with_lock(w, next, &aio_watches, node) {
		struct list_head requeue, *lock_queue;

		retry = false;
		list_init(&requeue);
		for (i = 0; i < count; i++) {
			iocb = (struct iocb *)ev[i].obj;
			if (!iocb || iocb->aio_fildes != w->fd)
				continue;

			mbuf = NULL;

			if (iocb->aio_lio_opcode == IOCB_CMD_PWRITEV ||
			    iocb->aio_lio_opcode == IOCB_CMD_PREADV)
				is_iovec = true;
			else
				is_iovec = false;

			if (is_iovec) {
				iovec_head = (struct iovec *)iocb->aio_buf;
				iovec_nentries = iocb->aio_nbytes;
				if (!iovec_head || !iovec_nentries) {
					ALOGE("diag: %s received NULL or 0 entries for IOVEC for fd: %d\n",
					      __func__, w->fd);
					watch_put_iocb(iocb);
					continue;
				}
			} else {
				iovec_head = NULL;
				iovec_nentries = 1;
			}

			for (j = 0; j < iovec_nentries; j++) {
				if (is_iovec) {
					iovec = &iovec_head[j];
					if (!iovec)
						continue;

					mbuf = container_of(iovec->iov_base, struct mbuf, data);
				} else {
					mbuf = container_of((char*)iocb->aio_buf, struct mbuf, data);
				}

				if (!mbuf) {
					ALOGE("diag: %s received NULL buf for fd%d\n", __func__, w->fd);
					if (!w->is_write)
						w->aio_complete(mbuf, w->data);
					continue;
				}

				/*
				 * rsp_flag is cached differently depending if iovec is used or not. For
				 * iovec, rsp_flag is cached in aio_data as there are multiple mbuf in one
				 * io_submit. For simplicity, rsp_flag will be set to the same value again
				 * and again based on iovec_nentries.
				 */
				rsp_flag = is_iovec ? iocb->aio_data : mbuf->rsp_flag;
				lock_queue = rsp_flag ? w->cmd_rsp_queue : w->queue;

				LIST_LOCK(lock_queue);
				list_del(&mbuf->node);
				LIST_UNLOCK(lock_queue);
				switch (ev[i].res) {
				case -EPIPE:
				case -ECONNRESET:
					ALOGE("diag: %s received err: %lld for fd: %d\n", __func__, ev[i].res, w->fd);
					if (w->fd_type == UNIX_CLIENT_FD) {
						if (w->is_write)
							w->aio_complete(mbuf, w->data);
						continue;
					} else if (!w->fd_type) {
						watch_put_iocb(iocb);
					}
					list_push(&requeue, &mbuf->node);
					continue;
				case -EAGAIN:
					retry = true;
					list_push(&requeue, &mbuf->node);
					continue;
				case -EMSGSIZE:
					ALOGE("diag: %s received err: %lld for fd: %d\n", __func__, ev[i].res, w->fd);
					watch_writeq_free(w);
					continue;
				default:
					if (ev[i].res < 0) {
						if (w->is_write)
							w->aio_complete(mbuf, w->data);
						ALOGE("diag: %s received err: %lld for fd: %d\n", __func__, ev[i].res, w->fd);
						continue;
					}
				}
				if (!w->is_write && ev[i].res >= 0) {
					mbuf->offset = ev[i].res;
				}
				if (diag_debug_mask & DIAG_DBG_MASK_WATCH)
					watch_debug_print(__func__, w, mbuf, HANDLE_EVENTFD);

				w->aio_complete(mbuf, w->data);
				retry = false;
				/* reset the retry count of a watch post aio complete handling */
				w->retry_count = 0;
			}

			if (rsp_flag)
				w->rsp_pending_count--;
			else
				w->pending_count--;
			free(iovec_head);
			watch_put_iocb(iocb);
		}

		/* non-zero retry counter indicate the send failure,increment retry counter for the watch */
		if (retry)
			w->retry_count++;

		while (!list_empty(&requeue)) {
			mbuf = list_entry_first(&requeue, struct mbuf, node);
			LIST_LOCK(&requeue);
			list_del(&mbuf->node);
			LIST_UNLOCK(&requeue);

			if (mbuf->rsp_flag)
				list_push(w->cmd_rsp_queue, &mbuf->node);
			else
				list_push(w->queue, &mbuf->node);

		}
	}
	LIST_UNLOCK(&aio_watches);

	/* Check if any iocbs belong to removed watches */
	list_for_each_entry_safe_with_lock(w, next, &free_watches, node) {
		for (i = 0; i < count; i++) {
			iocb = (struct iocb *)ev[i].obj;
			if (iocb && iocb->aio_fildes == w->fd)
				watch_put_iocb(iocb);
		}
	}
	LIST_UNLOCK(&free_watches);
}

void watch_run(void)
{
	struct timer *timer;
	struct timeval *timeout;
	struct timeval now;
	struct timeval tv;
	aio_context_t ioctx = 0;
	struct watch *next;
	struct watch *w;
	fd_set rfds;
	fd_set exfds;
	int evfd;
	int nfds;
	int ret;
	int set_timeout = 0;

	pthread_mutex_init(&aio_free_mutex, NULL);

	evfd = eventfd(0, 0);
	if (evfd < 0)
		err(1, "failed to create eventfd");

	ret = io_setup(32, &ioctx);
	if (ret < 0)
		err(1, "failed to initialize aio context");

	while (!do_watch_quit) {
		FD_ZERO(&rfds);
		FD_ZERO(&exfds);
		FD_SET(evfd, &rfds);

		nfds = evfd + 1;

		set_timeout = 0;

		list_for_each_entry(w, &read_watches, node) {
			/* Skip read watches with flows that are blocked */
			if (watch_flow_blocked(w->flow))
				continue;
			FD_SET(w->fd, &rfds);
			FD_SET(w->fd, &exfds);
			nfds = MAX(w->fd + 1, nfds);
		}

		list_for_each_entry_with_lock(w, &aio_watches, node) {
			/* Submit AIO if none is pending */
			if ((w->queue && !list_empty(w->queue)) ||
			    (w->cmd_rsp_queue && !list_empty(w->cmd_rsp_queue))) {
				/**
				 * go for sleep if packet retry reaches to the limit
				 * this will help to avoid continuous wakeup and heavy
				 * use of cpu load. This issue will happen if clients
				 * enter into background mode and does't receive the
				 * packets send by diag over unix socket.
				 */
				if (w->retry_count > WATCH_MAX_PKT_RETRY) {
					w->retry_count = 0;
					set_timeout = 1;
					continue;
				}
				watch_submit_aio(ioctx, evfd, w);
			}
			/* Check if there are more pending items in the queue and set tv value*/
			if ((w->queue && !list_empty(w->queue)) ||
			    (w->cmd_rsp_queue && !list_empty(w->cmd_rsp_queue)))
				set_timeout = 1;
		}
		LIST_UNLOCK(&aio_watches);

		timer = watch_get_next_timer();
		if (timer) {
			gettimeofday(&now, NULL);
			timersub(&timer->tick, &now, &tv);

			if (tv.tv_sec < 0)
				tv.tv_sec = tv.tv_usec = 0;
			timeout = &tv;
		} else {
			tv.tv_sec = 5;
			tv.tv_usec = 0;
			if (set_timeout)
				timeout = &tv;
			else
				timeout = NULL;
		}

		ret = select(nfds, &rfds, NULL, &exfds, timeout);
		if (ret < 0) {
			ALOGE("diag: %s: failed to select with error: %d\n", __func__, errno);
			continue;
		}

		if (ret == 0 && timer) {
			if (timer->cb)
				timer->cb(timer->data);

			if (timer->repeat)
				watch_set_timer(timer);
			else
				watch_free_timer(timer);
		}

		if (FD_ISSET(evfd, &rfds))
			watch_handle_eventfd(evfd, ioctx);

		list_for_each_entry_safe(w, next, &read_watches, node) {
			if (FD_ISSET(w->fd, &exfds)) {
				glinkpkt_handle_eventfd(w->fd);
				continue;
			}

			if (FD_ISSET(w->fd, &rfds)) {
				ret = w->cb(w->fd, w->data);
				if (ret < 0) {
					ALOGE("diag: removing watch from read watches fd %d err %d\n", w->fd, ret);
					list_del(&w->node);
				}
			}
		}

		watch_free_watches();
		watch_free_read_watches();
	}

	list_for_each_entry(w, &quit_watches, node)
		w->cb(-1, w->data);

	pthread_mutex_destroy(&aio_free_mutex);

	io_destroy(ioctx);
}
