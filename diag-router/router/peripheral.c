/*
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
#include <err.h>
#include <errno.h>
#include <fcntl.h>
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
#include "peripheral-qrtr.h"
#include "peripheral-rpmsg.h"
#include "peripheral-glinkpkt.h"
#include "util.h"
#include "watch.h"

struct list_head peripherals = LIST_INIT(peripherals);

int peripheral_send(struct peripheral *peripheral, const void *ptr, size_t len)
{
	return peripheral->send(peripheral, ptr, len);
}

void peripheral_close(struct peripheral *peripheral)
{
	peripheral->close(peripheral);
}

int peripheral_apps_init(void)
{
	struct peripheral *perif;

	perif = calloc(1, sizeof(*perif));
	if (!perif)
		return -ENOMEM;
	perif->name = "APPS";
	perif->periph_id = PERIPHERAL_APPS;
	diag_add_diag_id_to_list(DIAG_ID_APPS, "APPS", PERIPHERAL_APPS, perif);
	return 0;
}

int peripheral_init(void)
{
	int ret = 0;

	//peripheral_rpmsg_init();
	ret = peripheral_qrtr_init();
	if (ret < 0)
		return ret;

	/* bypass Apps init for vm diag-router instace */
	if (!vm_enabled) {
		ret = peripheral_apps_init();
		if (ret < 0)
			return ret;
		peripheral_glinkpkt_init();
	}

	return 0;
}

void perif_flow_reset(int p_mask)
{
	qrtr_perif_flow_reset(p_mask);
}

static int diag_check_update(int md_peripheral, int pid)
 {
	int ret = 0;
	struct diag_md_session_t *info = NULL;

	info = diag_md_session_get_pid(pid);
	ret = (!info || (info &&
		(info->peripheral_mask[DIAG_LOCAL_PROC] &
		MD_PERIPHERAL_MASK(md_peripheral))));

	return ret;
}

void peripheral_broadcast_event_mask(uint32_t sub_id,
			uint8_t preset_id, int pid)
{
	struct peripheral *peripheral;
	int i = 0, periph_id;

	for (i = 1; i < NUM_MD_SESSIONS; i++) {
		periph_id = i;
		if (diag_check_update(i, pid)) {
			if (i > NUM_PERIPHERALS)
				periph_id = diag_query_peripheral_from_pd(i);
			peripheral = diag_get_periph_info(periph_id);
			diag_cntl_send_event_mask(peripheral, sub_id, preset_id, pid);
		}
	}
}

void peripheral_broadcast_log_mask(unsigned int equip_id,
			uint32_t sub_id,
			int pid,
			uint8_t preset_id)
{
	struct peripheral *peripheral;
	int i = 0, periph_id;

	for (i = 1; i < NUM_MD_SESSIONS; i++) {
		periph_id = i;
		if (diag_check_update(i, pid)) {
			if (i > NUM_PERIPHERALS)
				periph_id = diag_query_peripheral_from_pd(i);
			peripheral = diag_get_periph_info(periph_id);
			diag_cntl_send_log_mask(peripheral, equip_id, sub_id, pid, preset_id);
		}
	}
}

void peripheral_broadcast_msg_mask(
	struct diag_ssid_range_t *range,
	uint32_t sub_id,
	int pid,
	int preset)
{
	struct peripheral *peripheral;
	int i = 0, periph_id;

	for (i = 1; i < NUM_MD_SESSIONS; i++) {
		periph_id = i;
		if (diag_check_update(i, pid)) {
			if (i > NUM_PERIPHERALS)
				periph_id = diag_query_peripheral_from_pd(i);
			peripheral = diag_get_periph_info(periph_id);
			diag_cntl_send_msg_mask(peripheral, range, sub_id, pid, preset);
		}
	}
}

void peripheral_broadcast_time_switch(uint8_t time_api)
{

	struct peripheral *peripheral;
	struct list_head *item;

	list_for_each(item, &peripherals) {
		peripheral = container_of(item, struct peripheral, node);

		diag_cntl_send_time_switch_msg(peripheral, time_api);
	}
}

/* To check whether peripheral is a gvm peripheral or not */
int peripheral_is_gvm(struct peripheral *perif)
{
	int ret = 0;

	if(!perif)
		return ret;

	if (strstr(perif->name, "gvm") != NULL)
		ret = 1;

	return ret;
}

void peripheral_broadcast_pkt_selection_req(uint8_t request)
{
	struct peripheral *peripheral;
	struct list_head *item;

	list_for_each(item, &peripherals) {
		peripheral = container_of(item, struct peripheral, node);
		if (diagmem->feature[peripheral->periph_id].diag_id_logging)
			diag_cntl_send_pkt_format_request(peripheral, request);
	}
}

/**
 * This API will give the peripheral instance of the vm
 * this has to be called from vm itself
 */
struct peripheral * peripheral_get_vm(void)
{
	struct peripheral *peripheral;
	struct list_head *item;

	list_for_each(item, &peripherals) {
		peripheral = container_of(item, struct peripheral, node);
		if (strstr(peripheral->self_name, "gvm") != NULL)
			return peripheral;
	}

	return NULL;
}
