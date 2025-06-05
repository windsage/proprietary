/*
 * Copyright (c) 2016-2018, The Linux Foundation. All rights reserved.
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
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "diag.h"
#include "masks.h"
#include "peripheral.h"
#include "util.h"
#include "dm.h"
#include "diag_hidl_wrapper.h"
#include "diag_dci.h"

#define APPS_BUF_SIZE 16384
#define MASK_UPDATE_BUF_SIZ 100000

struct diag_mask_info msg_mask;
struct diag_mask_info msg_bt_mask;
struct diag_mask_info log_mask;
struct diag_mask_info event_mask;

int diag_subid_info[MAX_SIM_NUM] = {[0 ... (MAX_SIM_NUM - 1)] =
       INVALID_INDEX};

static int __diag_multisim_mask_init(struct diag_mask_info *mask_info,
	int mask_len, int subid_index);

static int diag_create_log_mask_table(
	struct diag_log_mask_t *mask,
	int subid_index);

/*
 * diag_get_ms_ptr_index(struct diag_multisim_masks *ms_ptr, int subid_index)
 *
 * Input:
 * ms_ptr = Head pointer to multisim mask (mask_info->ms_ptr)
 * subid_index = Index of required subscription's mask
 *
 * Return:
 * Function will return multisim mask pointer corresponding to given
 * subid_index by iterating through the list
 * Function will return NULL if no multisim mask is present for given
 * subid_index or having invalid sub_ptr (ms_ptr->sub_ptr)
 *
 */
struct diag_multisim_masks
	*diag_get_ms_ptr_index(struct diag_multisim_masks *ms_ptr,
	int subid_index)
{
	struct diag_multisim_masks *temp = NULL;

	if (!ms_ptr)
		return NULL;

	temp = ms_ptr;
	while ((subid_index > 0) && temp && temp->next) {
		temp = temp->next;
		subid_index--;
	}
	if (subid_index == 0 && temp && temp->sub_ptr)
		return temp;
	else
		return NULL;
}

static int __diag_multisim_mask_init(struct diag_mask_info *mask_info,
		int mask_len, int subid_index)
{
	struct diag_multisim_masks *temp = NULL;
	struct diag_multisim_masks *ms_ptr = NULL;

	if (!mask_info || mask_len <= 0 || subid_index < 0)
		return -EINVAL;

	if (mask_len > 0) {
		temp = calloc(1 , sizeof(struct diag_multisim_masks));
		if (!temp)
			return -ENOMEM;
		temp->sub_ptr = calloc(mask_len, 1);
		if (!temp->sub_ptr) {
			free(temp);
			temp = NULL;
			return -ENOMEM;
		}
		temp->next = NULL;

		if (mask_info->ms_ptr) {
			ms_ptr = mask_info->ms_ptr;

			while (ms_ptr->next)
				ms_ptr = ms_ptr->next;
			ms_ptr->next = temp;
		} else {
			mask_info->ms_ptr = temp;
		}
	}

	return 0;
}


static int diag_multisim_msg_mask_init(int subid_index,
		struct diag_md_session_t *info)
{
	int err = 0;
	struct diag_mask_info *mask_info = NULL;
	struct diag_msg_mask_t *mask;
	struct diag_multisim_masks *ms_mask = NULL;

	mask_info = (!info) ? &msg_mask : info->msg_mask;

	err = __diag_multisim_mask_init(mask_info, MSG_MASK_SIZE,
			subid_index);
	if (err)
		return err;

	if (subid_index >= 0){
	   ms_mask = diag_get_ms_ptr_index(mask_info->ms_ptr, subid_index);
	}

	if (ms_mask){
		mask = (struct diag_msg_mask_t *)ms_mask->sub_ptr;
	} else {
		mask = (struct diag_msg_mask_t *)msg_mask.ptr;
	}

	err = diag_create_msg_mask_table(mask, subid_index);
	if (err) {
		ALOGE("diag: Unable to create msg masks, err: %d\n", err);
		return err;
	}

	return 0;
}

static int diag_multisim_log_mask_init(int subid_index,
		struct diag_md_session_t *info)
{
	int err_no = 0;
	struct diag_mask_info *mask_info = NULL;
	struct diag_multisim_masks *ms_mask = NULL;
	struct diag_log_mask_t *mask = NULL;

	mask_info = (!info) ? &log_mask : info->log_mask;

	err_no = __diag_multisim_mask_init(mask_info, LOG_MASK_SIZE,
			subid_index);
	if (err_no)
		goto err;

	if (subid_index >= 0)
		ms_mask = diag_get_ms_ptr_index(mask_info->ms_ptr, subid_index);

	if (ms_mask)
		mask = (struct diag_log_mask_t *)ms_mask->sub_ptr;
	else
		mask = (struct diag_log_mask_t *)log_mask.ptr;

	err_no = diag_create_log_mask_table(mask, subid_index);
	if (err_no)
		goto err;
err:
	return err_no;
}

static int diag_multisim_event_mask_init(int subid_index,
		struct diag_md_session_t *info)
{
	int err = 0;
	struct diag_mask_info *mask_info = NULL;

	mask_info = (!info) ? &event_mask : info->event_mask;

	err = __diag_multisim_mask_init(mask_info, EVENT_MASK_SIZE,
			subid_index);

	return err;
}


int diag_check_subid_mask_index(uint32_t subid, int pid)
{
	int err = 0, i = 0;
	struct diag_md_session_t *info = NULL;

	for (i = 0; (i < MAX_SIM_NUM) && (diag_subid_info[i] != INVALID_INDEX);
		i++) {
		if (diag_subid_info[i] == subid)
			return i;
	}
	if (i == MAX_SIM_NUM) {
		ALOGE("diag: Reached maximum number of subid supported: %d\n",
				MAX_SIM_NUM);
		return -EINVAL;
	}

	diag_subid_info[i] = subid;

	info = diag_md_session_get_pid(pid);

	err = diag_multisim_msg_mask_init(i, info);
	if (err)
		goto fail;
	err = diag_multisim_log_mask_init(i, info);
	if (err)
		goto fail;
	err = diag_multisim_event_mask_init(i, info);
	if (err)
		goto fail;

	return i;
fail:
	ALOGE("diag: Could not initialize diag mask for subid: %d buffers\n",
		subid);
	return -ENOMEM;
}



uint16_t event_max_num_bits;
struct diag_client_pkt {
	int type;
	unsigned char buf[0];
};
int clients_broadcast_event_mask(void)
{
	struct diag_client_pkt *pkt;
	int len;
	struct diag_md_session_t *info = NULL;
	struct diag_event_mask_t *event_item;

	if (diagmem->md_session_mask[DIAG_LOCAL_PROC] != 0) {
		if (diagmem->md_session_mask[DIAG_LOCAL_PROC] &
			MD_PERIPHERAL_MASK(PERIPHERAL_APPS)) {
			if (diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS])
				info =  diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS];
		}
	}
	event_item = (info) ? info->event_mask->ptr : event_mask.ptr;

	len = BITS_TO_BYTES(event_max_num_bits) + 4;
	pkt = alloca(len);
	if (!pkt) {
		ALOGE("In %s failed to alloc memory\n", __func__);
		return 0;
	}
#ifndef FEATURE_LE_DIAG
	diag_update_hidl_client(EVENT_MASKS_TYPE);
#endif /* FEATURE_LE_DIAG */
	pkt->type = EVENT_MASKS_TYPE;
	memcpy(pkt->buf, event_item, BITS_TO_BYTES(event_max_num_bits));
	dm_broadcast_to_socket_clients(pkt, len);

	return 0;
}
int clients_broadcast_log_mask(void)
{
	unsigned char *pkt;
	int data_type = LOG_MASKS_TYPE;
	struct diag_log_mask_t *log_item;
	struct diag_log_mask_userspace_t header;
	int i;
	int copy_len = 0, total_len = 0;
	struct diag_md_session_t *info = NULL;

	if (diagmem->md_session_mask[DIAG_LOCAL_PROC] != 0) {
 		if (diagmem->md_session_mask[DIAG_LOCAL_PROC] &
 			MD_PERIPHERAL_MASK(PERIPHERAL_APPS)) {
 			if (diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS])
 				info =  diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS];
		}
	}
	log_item = (info) ? info->log_mask->ptr : log_mask.ptr;
	pkt = alloca(MASK_UPDATE_BUF_SIZ);
	if (!pkt) {
		ALOGE("In %s failed to alloc memory\n", __func__);
		return 0;
	}
#ifndef FEATURE_LE_DIAG
	diag_update_hidl_client(LOG_MASKS_TYPE);
#endif /* FEATURE_LE_DIAG */
	memcpy(pkt,&data_type,sizeof(int));
	total_len = total_len + 4;
	for (i = 0; i < MAX_EQUIP_ID; i++, log_item++){
		header.equip_id = log_item->equip_id;
		header.num_items = log_item->num_items_tools;
		memcpy(pkt+total_len, &header, sizeof(header));
		total_len += sizeof(header);
		copy_len = BITS_TO_BYTES(log_item->num_items_tools);
		if ((total_len + copy_len) > MASK_UPDATE_BUF_SIZ) {
			printf("diag: In %s, no space to update log mask, equip_id: %d\n",
					__func__, log_item->equip_id);
			continue;
		}
		memcpy(pkt + total_len, log_item->ptr, copy_len);
		total_len += copy_len;
	}
	dm_broadcast_to_socket_clients(pkt, total_len);

	return 0;
}

int clients_broadcast_msg_mask(void)
{
	unsigned char *pkt;
	int len ;
	int data_type = MSG_MASKS_TYPE;
	struct diag_msg_mask_t *msg_item;
	struct diag_msg_mask_userspace_t header;
	int i;
	int copy_len = 0, total_len = 0;
	struct diag_md_session_t *info = NULL;

	if (diagmem->md_session_mask[DIAG_LOCAL_PROC] != 0) {
 		if (diagmem->md_session_mask[DIAG_LOCAL_PROC] &
 			MD_PERIPHERAL_MASK(PERIPHERAL_APPS)) {
 			if (diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS])
 				info =  diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS];
		}
	}
	msg_item = (info) ? info->msg_mask->ptr : msg_mask.ptr;
#ifndef FEATURE_LE_DIAG
	diag_update_hidl_client(MSG_MASKS_TYPE);
#endif /* FEATURE_LE_DIAG */
	pkt = alloca(MASK_UPDATE_BUF_SIZ);
	if (!pkt) {
		ALOGE("In %s failed to alloc memory\n", __func__);
		return 0;
	}
	memcpy(pkt,&data_type,sizeof(int));
	total_len = total_len +4;
	for (i = 0; i < MSG_MASK_TBL_CNT; i++, msg_item++){
		len = 0;
		header.ssid_first = msg_item->ssid_first;
		header.ssid_last = msg_item->ssid_last_tools;
		header.range = msg_item->range_tools;
		memcpy(pkt+total_len, &header, sizeof(header));
		total_len += sizeof(header);
		copy_len = (sizeof(uint32_t) * msg_item->range_tools);
		if ((len + copy_len) > MASK_UPDATE_BUF_SIZ) {
			ALOGE("diag: In %s, no space to update msg mask, first: %d, last: %d\n",
			       __func__, msg_item->ssid_first,
			       msg_item->ssid_last_tools);
			continue;
		}
		memcpy(pkt + total_len, msg_item->ptr, copy_len);
		total_len += copy_len;
	}
	dm_broadcast_to_socket_clients(pkt, total_len);

	return 0;
}

int diag_event_mask_update_client(struct diag_client *dm)
{
	struct diag_client_pkt *pkt;
	int len ;
	struct diag_md_session_t *info = NULL;
	struct diag_event_mask_t *event_item;

	if (diagmem->md_session_mask[DIAG_LOCAL_PROC] != 0) {
 		if (diagmem->md_session_mask[DIAG_LOCAL_PROC] &
 			MD_PERIPHERAL_MASK(PERIPHERAL_APPS)) {
 			if (diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS])
 				info =  diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS];
		}
	}
	event_item = (info) ? info->event_mask->ptr : event_mask.ptr;
	len = BITS_TO_BYTES(event_max_num_bits) + 4;
	pkt = calloc(1, len);
	if (!pkt) {
		ALOGE("In %s failed to alloc memory\n", __func__);
		return 0;
	}
	pkt->type = EVENT_MASKS_TYPE;
	memcpy(pkt->buf, event_item, BITS_TO_BYTES(event_max_num_bits));
	dm_send(dm, pkt, len);
	free(pkt);
	pkt = NULL;
	return 0;
}

int diag_log_mask_update_client(struct diag_client *dm)
{
	unsigned char *pkt;
	int data_type = LOG_MASKS_TYPE;
	struct diag_log_mask_t *log_item = log_mask.ptr;
	struct diag_log_mask_userspace_t header;
	int i;
	int copy_len = 0, total_len = 0;
	struct diag_md_session_t *info = NULL;

	if (diagmem->md_session_mask[DIAG_LOCAL_PROC] != 0) {
 		if (diagmem->md_session_mask[DIAG_LOCAL_PROC] &
 			MD_PERIPHERAL_MASK(PERIPHERAL_APPS)) {
 			if (diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS])
 				info =  diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS];
		}
	}
	log_item = (info) ? info->log_mask->ptr : log_mask.ptr;
	pkt = calloc(1, MASK_UPDATE_BUF_SIZ);
	if (!pkt) {
		ALOGE("In %s failed to alloc memory\n", __func__);
		return 0;
	}
	memcpy(pkt,&data_type,sizeof(int));
	total_len = total_len + sizeof(data_type);
	for (i = 0; i < MAX_EQUIP_ID; i++, log_item++){
		header.equip_id = log_item->equip_id;
		header.num_items = log_item->num_items_tools;
		memcpy(pkt+total_len, &header, sizeof(header));
		total_len += sizeof(header);
		copy_len = BITS_TO_BYTES(log_item->num_items_tools);
		if ((total_len + copy_len) > MASK_UPDATE_BUF_SIZ) {
			printf("diag: In %s, no space to update log mask, equip_id: %d\n",
					__func__, log_item->equip_id);
			continue;
		}
		memcpy(pkt + total_len, log_item->ptr, copy_len);
		total_len += copy_len;
	}
	dm_send(dm, pkt, total_len);
	free(pkt);
	pkt = NULL;
	return 0;
}
int diag_msg_mask_update_client(struct diag_client *dm)
{
	unsigned char *pkt;
	int len;
	int data_type = MSG_MASKS_TYPE;
	struct diag_msg_mask_t *msg_item = msg_mask.ptr;
	struct diag_msg_mask_userspace_t header;
	int i;
	int copy_len = 0, total_len = 0;
	struct diag_md_session_t *info = NULL;

	if (diagmem->md_session_mask[DIAG_LOCAL_PROC] != 0) {
 		if (diagmem->md_session_mask[DIAG_LOCAL_PROC] &
 			MD_PERIPHERAL_MASK(PERIPHERAL_APPS)) {
 			if (diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS])
 				info =  diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS];
		}
	}
	msg_item = (info) ? info->msg_mask->ptr : msg_mask.ptr;
	pkt = calloc(1, MASK_UPDATE_BUF_SIZ);
	if (!pkt) {
		ALOGE("In %s failed to alloc memory\n", __func__);
		return 0;
	}
	memcpy(pkt,&data_type,sizeof(int));
	total_len = total_len + sizeof(data_type);
	for (i = 0; i < MSG_MASK_TBL_CNT; i++, msg_item++) {

		len = 0;
		header.ssid_first = msg_item->ssid_first;
		header.ssid_last = msg_item->ssid_last_tools;
		header.range = msg_item->range_tools;
		memcpy(pkt+total_len, &header, sizeof(header));
		total_len += sizeof(header);
		copy_len = (sizeof(uint32_t) * msg_item->range_tools);
		if ((len + copy_len) > MASK_UPDATE_BUF_SIZ) {
			ALOGE("diag: In %s, no space to update msg mask, first: %d, last: %d\n",
			       __func__, msg_item->ssid_first,
			       msg_item->ssid_last_tools);
			continue;
		}
		memcpy(pkt + total_len, msg_item->ptr, copy_len);
		total_len += copy_len;
	}
	dm_send(dm, pkt, total_len);
	free(pkt);
	pkt = NULL;
	return 0;
}
static int diag_mask_init(struct diag_mask_info *mask_info, int mask_len,
			    int update_buf_len)
{
	if (!mask_info || mask_len < 0 || update_buf_len < 0)
		return -EINVAL;

	mask_info->status = DIAG_CTRL_MASK_INVALID;
	mask_info->mask_len = mask_len;
	if (mask_len > 0) {
		mask_info->ptr = malloc(mask_len);
		if (!mask_info->ptr)
			return -ENOMEM;
		memset(mask_info->ptr, 0, mask_len);
	}

	return 0;
}

static int diag_create_msg_mask_table_entry(struct diag_msg_mask_t *msg_mask,
				    uint32_t entry, int subid_index)
{
	if (!msg_mask)
		return -EIO;

	if (entry >= NUM_OF_MASK_RANGES)
		return -EINVAL;

	msg_mask->ssid_first = ssid_first_arr[entry];
	msg_mask->ssid_last = ssid_last_arr[entry];
	msg_mask->ssid_last_tools = ssid_last_arr[entry];
	msg_mask->range = msg_mask->ssid_last - msg_mask->ssid_first + 1;

	if (msg_mask->range < MAX_SSID_PER_RANGE)
		msg_mask->range = MAX_SSID_PER_RANGE;
	msg_mask->range_tools = msg_mask->range;

	if (subid_index >= 0) {
		msg_mask->id_valid = 1;
		msg_mask->sub_id = diag_subid_info[subid_index];
	} else {
		msg_mask->id_valid = 0;
		msg_mask->sub_id = 0;
	}


	if (msg_mask->range > 0) {
		msg_mask->ptr = malloc(msg_mask->range * sizeof(uint32_t));
		if (!msg_mask->ptr)
			return -ENOMEM;
		memset(msg_mask->ptr, 0, msg_mask->range * sizeof(uint32_t));
	}

	return 0;
}

static int diag_create_build_time_mask(void)
{
	int err = 0;
	struct diag_msg_mask_t *build_mask = msg_bt_mask.ptr;
	int i;

	for (i = 0; i < MSG_MASK_TBL_CNT; i++, build_mask++) {
		err = diag_create_msg_mask_table_entry(build_mask, i, INVALID_INDEX);
		if (err)
			break;
	}

	return err;
}

int diag_create_msg_mask_table(struct diag_msg_mask_t *mask, int subid_index)
{
	int err = 0;
	int i;

	for (i = 0; i < MSG_MASK_TBL_CNT; i++, mask++) {
		err = diag_create_msg_mask_table_entry(mask, i, subid_index);
		if (err)
			break;
	}

	return err;
}

static int diag_msg_mask_init(void)
{
	int ret;

	ret = diag_mask_init(&msg_mask, MSG_MASK_SIZE, APPS_BUF_SIZE);
	if (ret)
		return ret;

	ret = diag_create_msg_mask_table(msg_mask.ptr, INVALID_INDEX);
	if (ret) {
		printf("diag: Unable to create msg masks, err: %d\n", ret);
		return ret;
	}

	return ret;
}

static void diag_msg_mask_exit(struct diag_msg_mask_t *mask)
{
	int i;

	if (mask) {
		for (i = 0; i < MSG_MASK_TBL_CNT; i++, mask++) {
			free(mask->ptr);
			mask->ptr = NULL;
		}
	}
}

static int diag_build_time_mask_init(void)
{
	int err;

	/* There is no need for update buffer for Build Time masks */
	err = diag_mask_init(&msg_bt_mask, MSG_MASK_SIZE, 0);
	if (err)
		return err;

	err = diag_create_build_time_mask();
	if (err) {
		printf("diag: Unable to create msg build time masks, err: %d\n", err);
		return err;
	}

	return 0;
}

static void diag_build_time_mask_exit(void)
{
	struct diag_msg_mask_t *mask = msg_bt_mask.ptr;
	int i;

	if (mask) {
		for (i = 0; i < MSG_MASK_TBL_CNT; i++, mask++) {
			free(mask->ptr);
			mask->ptr = NULL;
		}
		free(msg_bt_mask.ptr);
		msg_bt_mask.ptr = NULL;
	}
}

static int diag_create_log_mask_table(struct diag_log_mask_t *mask, int subid_index)
{
	uint8_t equip_id;
	int err = 0;

	for (equip_id = 0; equip_id < MAX_EQUIP_ID; equip_id++, mask++) {
		mask->equip_id = equip_id;
		mask->num_items = LOG_GET_ITEM_NUM(log_code_last_tbl[equip_id]);
		mask->num_items_tools = mask->num_items;
		if (BITS_TO_BYTES(mask->num_items) > MAX_ITEMS_PER_EQUIP_ID)
			mask->range = BITS_TO_BYTES(mask->num_items);
		else
			mask->range = MAX_ITEMS_PER_EQUIP_ID;
		mask->range_tools = mask->range;
		mask->ptr = malloc(mask->range);
		if (!mask->ptr) {
			err = -ENOMEM;
			break;
		}
		memset(mask->ptr, 0, mask->range);

		if (subid_index >= 0) {
			mask->id_valid = 1;
			mask->sub_id = diag_subid_info[subid_index];
		} else {
			mask->id_valid = 0;
			mask->sub_id = 0;
		}
	}

	return err;
}
int diag_msg_mask_copy(struct diag_md_session_t *new_session,
	struct diag_mask_info *dest, struct diag_mask_info *src)
{
	int i, err = 0, mask_size = 0;
	struct diag_msg_mask_t *src_mask = NULL;
	struct diag_msg_mask_t *dest_mask = NULL;

	if (!src || !dest)
		return -EINVAL;

	new_session->msg_mask_tbl_count = MSG_MASK_TBL_CNT;
	err = diag_mask_init(dest,
		(new_session->msg_mask_tbl_count *
		sizeof(struct diag_msg_mask_t)), APPS_BUF_SIZE);
	if (err) {
		return err;
	}
	src_mask = (struct diag_msg_mask_t *)src->ptr;
	dest_mask = (struct diag_msg_mask_t *)dest->ptr;

	dest->mask_len = src->mask_len;
	dest->status = src->status;
	for (i = 0; i < new_session->msg_mask_tbl_count; i++) {
		err = diag_create_msg_mask_table_entry(dest_mask, i, INVALID_INDEX);
		if (err)
			break;
		if (src_mask->range_tools < dest_mask->range)
			mask_size = src_mask->range_tools * sizeof(uint32_t);
		else
			mask_size = dest_mask->range * sizeof(uint32_t);
		memcpy(dest_mask->ptr, src_mask->ptr, mask_size);
		src_mask++;
		dest_mask++;
	}
	return err;
}

void diag_msg_mask_free(struct diag_mask_info *mask_info,
	struct diag_md_session_t *session_info)
{
	(void)session_info;

	if (!mask_info)
        return;
	diag_msg_mask_exit(mask_info->ptr);
}
int diag_event_mask_copy(struct diag_mask_info *dest,
			 struct diag_mask_info *src)
{
	int err = 0;

	if (!src || !dest)
		return -EINVAL;

	err = diag_mask_init(dest, EVENT_MASK_SIZE, APPS_BUF_SIZE);
	if (err)
		return err;

	dest->mask_len = src->mask_len;
	dest->status = src->status;

	memcpy(dest->ptr, src->ptr, dest->mask_len);

	return err;
}
static void diag_event_mask_exit(void *mask_info)
{
	free(mask_info);
	mask_info = NULL;
}
void diag_event_mask_free(struct diag_mask_info *mask_info)
{
	if (!mask_info)
		return;

	diag_event_mask_exit(mask_info->ptr);
}
int diag_log_mask_copy(struct diag_mask_info *dest, struct diag_mask_info *src)
{
	int i, err = 0;
	struct diag_log_mask_t *src_mask = NULL;
	struct diag_log_mask_t *dest_mask = NULL;

	if (!src || !dest)
		return -EINVAL;

	err = diag_mask_init(dest, LOG_MASK_SIZE, APPS_BUF_SIZE);
	if (err)
		return err;

	return diag_create_log_mask_table(dest->ptr, INVALID_INDEX);
	if (err)
		return err;

	src_mask = (struct diag_log_mask_t *)(src->ptr);
	dest_mask = (struct diag_log_mask_t *)(dest->ptr);

	dest->mask_len = src->mask_len;
	dest->status = src->status;

	for (i = 0; i < MAX_EQUIP_ID; i++, src_mask++, dest_mask++) {
		memcpy(dest_mask->ptr, src_mask->ptr, dest_mask->range_tools);
	}

	return err;
}


static int diag_log_mask_init(void)
{
	int ret;

	ret = diag_mask_init(&log_mask, LOG_MASK_SIZE, APPS_BUF_SIZE);
	if (ret)
		return ret;

	return diag_create_log_mask_table(log_mask.ptr, INVALID_INDEX);
}

static void diag_log_mask_exit(struct diag_log_mask_t *mask)
{
	int i;

	if (mask) {
		for (i = 0; i < MAX_EQUIP_ID; i++, mask++) {
			free(mask->ptr);
			mask->ptr = NULL;
		}
	}
}
void diag_log_mask_free(struct diag_mask_info *mask_info)
{
	if (!mask_info)
        return;
	diag_log_mask_exit(mask_info->ptr);
	free(mask_info->ptr);
	mask_info->ptr = NULL;
}
static int diag_event_mask_init(void)
{
	event_max_num_bits = APPS_EVENT_LAST_ID;

	return diag_mask_init(&event_mask, EVENT_MASK_SIZE, APPS_BUF_SIZE);
}



int diag_masks_init()
{
	if (diag_msg_mask_init() ||
		diag_build_time_mask_init() ||
		diag_log_mask_init() ||
		diag_event_mask_init()) {
		diag_masks_exit();
		printf("diag: Could not initialize diag mask buffers\n");

		return -ENOMEM;
	}

	return 0;
}

void diag_masks_exit()
{
	diag_msg_mask_exit(msg_mask.ptr);
	diag_build_time_mask_exit();
	diag_log_mask_exit(log_mask.ptr);
	free(log_mask.ptr);
	log_mask.ptr = NULL;
	diag_event_mask_exit(event_mask.ptr);
}

uint8_t diag_get_log_mask_status(struct diag_mask_info * mask_info)
{
	return mask_info->status;
}

void diag_cmd_disable_log(int pid, uint32_t sub_id)
{
	struct diag_log_mask_t *log_item;
	int i;


	log_item = (struct diag_log_mask_t *)diag_get_mask(sub_id, &log_mask, pid);

	if(!log_item)
		return;

	for (i = 0; i < MAX_EQUIP_ID; i++, log_item++) {
		memset(log_item->ptr, 0, log_item->range);
	}

	diag_masks_status_opr(sub_id, &log_mask, pid, mask_status_set, DIAG_CTRL_MASK_ALL_DISABLED);

}

void diag_cmd_get_log_range(uint32_t *ranges, uint32_t count,
		uint32_t sub_id, int pid)
{
	struct diag_log_mask_t *log_item = (struct diag_log_mask_t *)diag_get_mask(sub_id, &log_mask, pid);
	int i;

	if (!ranges || !log_item)
		return;

	for (i = 0; i < MIN(MAX_EQUIP_ID, count); i++, log_item++) {
		ranges[i] = log_item->num_items_tools;
	}
}

int diag_cmd_set_log_mask(uint8_t equip_id, uint32_t *num_items,
	uint8_t *mask, uint32_t *mask_size, int pid, uint32_t sub_id)
{
	struct diag_log_mask_t *log_item;
	void *tmp_buf;
	int i;

	log_item = (struct diag_log_mask_t *)diag_get_mask(sub_id, &log_mask, pid);


	if (!log_item || !log_item->ptr)
		return -EINVAL;

	for (i = 0; i < MAX_EQUIP_ID; i++, log_item++) {
		if (log_item->equip_id != equip_id)
			continue;

#if 0
		diag_dbg(DIAG_DBG_MASKS, "Found equip_id=%d\n"
				"current num_items=%u range=%u\n"
				"request num_items=%u range=%u\n",
				log_item->equip_id,
				log_item->num_items_tools, log_item->range_tools,
				*num_items, BITS_TO_BYTES(*num_items));
#endif

		log_item->num_items_tools = MIN(*num_items, MAX_ITEMS_ALLOWED);
		*mask_size = BITS_TO_BYTES(log_item->num_items_tools);
		memset(log_item->ptr, 0, log_item->range_tools);

		if (*mask_size > log_item->range_tools)
		{
			tmp_buf = realloc(log_item->ptr, *mask_size);

			if (!tmp_buf)
			{
				diag_masks_status_opr(sub_id, &log_mask, pid,
					mask_status_set , DIAG_CTRL_MASK_INVALID);

				warn("Failed to reallocate log mask\n");
				return -errno;
			}
			log_item->ptr = tmp_buf;
			memset(log_item->ptr, 0, *mask_size);
			log_item->range_tools = *mask_size;
		}

		*num_items = log_item->num_items_tools;

		if(mask)
			memcpy(log_item->ptr, mask, *mask_size);

		diag_masks_status_opr(sub_id, &log_mask, pid,
			mask_status_set , DIAG_CTRL_MASK_VALID);

		return 0; /*SUCCESS*/
	}

	return 1;
}

int diag_cmd_get_log_mask(uint32_t equip_id, uint32_t *num_items, uint8_t ** mask, uint32_t *mask_size, struct diag_log_mask_t *log_item)
{
	int i;

	for (i = 0; i < MAX_EQUIP_ID; i++, log_item++) {
		if (log_item->equip_id != equip_id) {
			continue;
		}

		*num_items = log_item->num_items_tools;
		*mask_size = BITS_TO_BYTES(log_item->num_items_tools);
		*mask = malloc(*mask_size);
		if (!*mask) {
			warn("Failed to allocate log mask\n");

			return -errno;
		}
		memcpy(*mask, log_item->ptr, *mask_size);

		return 0;
	}

	return 1;
}

void* diag_get_mask(
	uint32_t sub_id,
	struct diag_mask_info *diag_mask,
	int pid)
{
	int sub_index = INVALID_INDEX;
	struct diag_md_session_t *info = NULL;
	struct diag_multisim_masks *ms_ptr = NULL;
	struct diag_mask_info *mask_info = NULL;
	void *mask_ptr = NULL;

	if(diag_mask == NULL)
	{
		return NULL;
	}

	info = diag_md_session_get_pid(pid);

	if (diag_mask == &event_mask){
		mask_info = (!info) ? diag_mask : info->event_mask;
	} else if (diag_mask == &msg_mask) {
		mask_info = (!info) ? diag_mask : info->msg_mask;
	} else if(diag_mask == &log_mask) {
		mask_info = (!info) ? diag_mask : info->log_mask;
	} else if (diag_mask == &msg_bt_mask) {
		mask_info = &msg_bt_mask;
	}

	if (sub_id != INVALID_INDEX) /*multi-sim */
	{
		mask_info = (!info) ? diag_mask : info->msg_mask;
		if (!mask_info)
			return NULL;

		sub_index = diag_check_subid_mask_index(sub_id, pid);

		ms_ptr = diag_get_ms_ptr_index(mask_info->ms_ptr, sub_index);

		if (!ms_ptr)
			return NULL;

		mask_ptr = (void *)(ms_ptr->sub_ptr);
	} else if (mask_info) {
		mask_ptr = mask_info->ptr;
	}
	return mask_ptr;
}

uint8_t diag_masks_status_opr(
	uint32_t sub_id,
	struct diag_mask_info *diag_mask,
	int pid,
	enum mask_status_opr opr,
	uint8_t status)
{
	int sub_index = INVALID_INDEX;
	struct diag_md_session_t *info = NULL;
	struct diag_multisim_masks *ms_ptr = NULL;
	struct diag_mask_info *mask_info = NULL;

	if (diag_mask == NULL)
	  return DIAG_CTRL_MASK_INVALID;

	info = diag_md_session_get_pid(pid);

	if (diag_mask == &event_mask){
		mask_info = (!info) ? diag_mask : info->event_mask;
	} else if (diag_mask == &msg_mask) {
		mask_info = (!info) ? diag_mask : info->msg_mask;
	} else if(diag_mask == &log_mask) {
		mask_info = (!info) ? diag_mask : info->log_mask;
	}

	if (sub_id != INVALID_INDEX && mask_info) {
		sub_index = diag_check_subid_mask_index(sub_id, pid);
		ms_ptr = diag_get_ms_ptr_index(mask_info->ms_ptr, sub_index);

		if (ms_ptr) {
			if(opr == mask_status_set) {
				ms_ptr->status = status;
			} else if (opr == mask_status_get) {
				return ms_ptr->status;
			}
		}
	} else if (mask_info) {
		if(opr == mask_status_set) {
			mask_info->status = status;
		} else if (opr == mask_status_get) {
			return mask_info->status;
		}
	}
	return DIAG_CTRL_MASK_INVALID;
}

void diag_cmd_get_ssid_range(
	uint32_t *count,
	struct diag_ssid_range_t **ranges,
	uint32_t sub_id,
	int pid )
{
	struct diag_ssid_range_t *range;
	struct diag_msg_mask_t *mask_ptr = NULL;
	int i;

	mask_ptr = (struct diag_msg_mask_t *)diag_get_mask(sub_id, &msg_mask, pid);

	if (!mask_ptr || !mask_ptr->ptr) {
		ALOGE("diag: In %s, Invalid mask\n",
			__func__);
		return;
	}

	*count = MSG_MASK_TBL_CNT;
	*ranges = calloc(*count, sizeof(**ranges));
	if (!*ranges) {
		warn("Failed to allocate ssid ranges\n");
		return;
	}

	range = *ranges;
	for (i = 0; i < *count; i++, mask_ptr++, range++) {
		range->ssid_first = mask_ptr->ssid_first;
		range->ssid_last = mask_ptr->ssid_last_tools;
	}
}

uint8_t diag_get_build_mask_status()
{
	return msg_bt_mask.status;
}

int diag_cmd_get_build_mask(struct diag_ssid_range_t *range,
	uint32_t **mask,
	uint32_t sub_id,
	int pid)
{
	uint32_t num_entries = 0;
	uint32_t mask_size = 0;
	struct diag_msg_mask_t *mask_ptr = NULL;
	int i;

	mask_ptr = (struct diag_msg_mask_t *)diag_get_mask(sub_id, &msg_bt_mask, pid);

	if (!mask_ptr || !mask_ptr->ptr) {
		ALOGE("diag: In %s, Invalid mask\n",
			__func__);
		return -EINVAL;
	}

	if (!range)
		return -EINVAL;

	for (i = 0;i < MSG_MASK_TBL_CNT; i++, mask_ptr++)
	{
		if (mask_ptr->ssid_first != range->ssid_first) {
			continue;
		}
		num_entries = range->ssid_last - range->ssid_first + 1;
		if (num_entries > mask_ptr->range) {
			ALOGE("diag: Truncating ssid range for ssid_first: %d ssid_last %d\n",
				range->ssid_first, range->ssid_last);
			num_entries = mask_ptr->range;
			range->ssid_last = range->ssid_first + mask_ptr->range;
		}
		mask_size = num_entries * sizeof(uint32_t);
		*mask = malloc(mask_size);
		if (!*mask) {
			ALOGE("Failed to allocate build mask\n");
			return -errno;
		}
		memcpy(*mask, mask_ptr->ptr, mask_size);

		return 0;
	}

	return 1;
}

uint8_t diag_get_msg_mask_status(struct diag_mask_info * mask_info)
{
	return mask_info->status;
}

int diag_cmd_get_msg_mask(struct diag_ssid_range_t *range,
	uint32_t sub_id,
	uint32_t **mask,
	int pid)
{
	uint32_t mask_size = 0;
	struct diag_msg_mask_t *msg_item;
	int i;

	msg_item = (struct diag_msg_mask_t *)diag_get_mask(sub_id, &msg_mask, pid);

	if (!msg_item || !msg_item->ptr) {
		ALOGE("diag: In %s, Invalid mask\n",
			__func__);
		return -EINVAL;
	}

	if (!range)
		return -EINVAL;

	for (i = 0; i < MSG_MASK_TBL_CNT; i++, msg_item++) {
		if ((range->ssid_first < msg_item->ssid_first) ||
		    (range->ssid_first > msg_item->ssid_last_tools)) {
			continue;
		}

		mask_size = msg_item->range * sizeof(**mask);
		range->ssid_first = msg_item->ssid_first;
		range->ssid_last = msg_item->ssid_last;
		*mask = malloc(mask_size);
		if (!*mask) {
			warn("Failed to allocate event mask\n");
			return -errno;
		}
		memcpy(*mask, msg_item->ptr, mask_size);

		return 0;
	}

	return 1;
}

int diag_cmd_set_msg_mask(struct diag_ssid_range_t range,
	const uint32_t *mask,
	uint32_t sub_id,
	int pid)
{
	struct diag_msg_mask_t *msg_item = 0;
	uint32_t num_msgs = 0;
	struct diag_msg_mask_t *mask_next = NULL;

	uint32_t offset = 0;
	void *tmp_buf;
	int i;

	msg_item = (struct diag_msg_mask_t *)diag_get_mask(sub_id, &msg_mask, pid);

	if (!msg_item || !msg_item->ptr) {
		ALOGE("diag: In %s, Invalid mask\n",
			__func__);
		return -EINVAL;
	}

	for (i = 0; i < MSG_MASK_TBL_CNT; i++, msg_item++) {
		if (i < (MSG_MASK_TBL_CNT - 1)) {
			mask_next = msg_item;
			mask_next++;
		} else {
			mask_next = NULL;
		}

		if ((range.ssid_first < msg_item->ssid_first) ||
			(range.ssid_first > msg_item->ssid_first + MAX_SSID_PER_RANGE) ||
			(mask_next && (range.ssid_first >= mask_next->ssid_first))) {
			continue;
		}

		mask_next = NULL;
		num_msgs = range.ssid_last - range.ssid_first + 1;

		if (num_msgs > MAX_SSID_PER_RANGE) {
			warn("diag: Truncating ssid range, %d-%d to max allowed: %d\n",
				msg_item->ssid_first, msg_item->ssid_last,
				MAX_SSID_PER_RANGE);
			num_msgs = MAX_SSID_PER_RANGE;
			msg_item->range_tools = MAX_SSID_PER_RANGE;
			msg_item->ssid_last_tools = msg_item->ssid_first + msg_item->range_tools;
		}

		if (range.ssid_last > msg_item->ssid_last_tools) {
			if (num_msgs != MAX_SSID_PER_RANGE)
				msg_item->ssid_last_tools = range.ssid_last;

			msg_item->range_tools = msg_item->ssid_last_tools - msg_item->ssid_first + 1;
			tmp_buf = realloc(msg_item->ptr, msg_item->range_tools * sizeof(*mask));

			if (!tmp_buf) {
				diag_masks_status_opr(sub_id, &msg_mask, pid,
						mask_status_set, DIAG_CTRL_MASK_INVALID);
				warn("Failed to reallocate msg mask\n");
				return -errno;
			}
			msg_item->ptr = tmp_buf;
		}

		offset = range.ssid_first - msg_item->ssid_first;
		if (offset + num_msgs > msg_item->range_tools) {
			warn("diag: Not in msg mask range, num_msgs: %d, offset: %d\n",
			       num_msgs, offset);

			return 1;
		}
		memcpy(msg_item->ptr + offset, mask, num_msgs * sizeof(*mask));

		diag_masks_status_opr(sub_id, &msg_mask, pid,
				mask_status_set, DIAG_CTRL_MASK_VALID);

		return 0;
	}

	return 1;
}

void diag_cmd_set_all_msg_mask(uint32_t mask,
	uint32_t sub_id,
	int pid)
{
	struct diag_msg_mask_t *msg_item;
	int i;

	uint8_t status = mask ? DIAG_CTRL_MASK_ALL_ENABLED :
							DIAG_CTRL_MASK_ALL_DISABLED;

	msg_item = (struct diag_msg_mask_t *)diag_get_mask(sub_id, &msg_mask, pid);

	diag_masks_status_opr(sub_id, &msg_mask, pid,
			mask_status_set, status);
   if (msg_item) {
		for (i = 0; i < MSG_MASK_TBL_CNT; i++, msg_item++) {
   			memset(msg_item->ptr , mask , msg_item->range_tools * sizeof(mask));
		}
	}
}

uint8_t diag_get_event_mask_status(struct diag_mask_info *mask_info)
{
	return mask_info->status;
}

int diag_cmd_get_event_mask(uint16_t num_bits, uint8_t **mask, void *event_item)
{
	uint32_t mask_size = BITS_TO_BYTES(num_bits);

	if (num_bits > event_max_num_bits)
		return 1;

	*mask = malloc(mask_size);
	if (!*mask) {
		warn("Failed to allocate event mask\n");
		return -errno;
	}
	memcpy(*mask, event_item, mask_size);

	return 0;
}


int diag_cmd_update_event_mask(uint16_t num_bits,
	const uint8_t *mask, uint32_t sub_id, int pid)
{
	void *tmp_buf;
	struct diag_md_session_t *info = NULL;
	struct diag_mask_info *event_item;
	struct diag_multisim_masks *ms_ptr = NULL;

	info = diag_md_session_get_pid(pid);
	event_item = (!info) ? &event_mask : info->event_mask;

	if(sub_id == INVALID_INDEX)
	{
		if (num_bits > event_max_num_bits ) {
			tmp_buf = realloc(event_item->ptr, BITS_TO_BYTES(num_bits));
			if (!tmp_buf) {
				event_item->status = DIAG_CTRL_MASK_INVALID;
				warn("Failed to reallocate event mask\n");

				return -errno;
			}

			event_item->ptr = tmp_buf;
			event_max_num_bits = num_bits;
			event_item->mask_len = BITS_TO_BYTES(num_bits);
		}

		event_item->status = DIAG_CTRL_MASK_VALID;
		memset(event_item->ptr, 0, event_item->mask_len);
		memcpy(event_item->ptr, mask, BITS_TO_BYTES(num_bits));
	} else {

		int sub_index = diag_check_subid_mask_index(sub_id, pid);
		ms_ptr = diag_get_ms_ptr_index(event_item->ms_ptr, sub_index);

		if(!ms_ptr)
			return 0;

		if (num_bits > event_max_num_bits )
		{
			tmp_buf = realloc(ms_ptr->sub_ptr, BITS_TO_BYTES(num_bits));

			if (!tmp_buf) {

				diag_masks_status_opr(sub_id, event_item, pid, mask_status_set, DIAG_CTRL_MASK_INVALID);
				warn("Failed to reallocate event mask\n");
				return -errno;
			}

			ms_ptr->sub_ptr = tmp_buf;
			event_max_num_bits = num_bits;

			event_item->mask_len = BITS_TO_BYTES(num_bits);
		}
		diag_masks_status_opr(sub_id, event_item, pid, mask_status_set, DIAG_CTRL_MASK_VALID);

		memset(ms_ptr->sub_ptr, 0, event_item->mask_len);
		memcpy(ms_ptr->sub_ptr, mask, BITS_TO_BYTES(num_bits));

	}
	return 0;
}

void diag_cmd_toggle_events(bool enabled, int pid, uint32_t sub_id)
{
	struct diag_md_session_t *info = NULL;
	int mask_len = 0;
	struct diag_mask_info *event_item;
	void *event_item_ptr = NULL;
	info = diag_md_session_get_pid(pid);

	event_item = (!info) ? &event_mask : info->event_mask;

	mask_len = event_item->mask_len;

	event_item_ptr = diag_get_mask(sub_id, &event_mask, pid);
	if (!event_item_ptr)
		return;

	if (enabled) {
		memset(event_item_ptr, 0xff, mask_len);
		diag_masks_status_opr(sub_id, &event_mask, pid,
			mask_status_set, DIAG_CTRL_MASK_ALL_ENABLED);
	} else {
		memset(event_item_ptr, 0x00, mask_len);
		diag_masks_status_opr(sub_id, &event_mask, pid,
			mask_status_set, DIAG_CTRL_MASK_ALL_DISABLED);
	}

}

int diag_check_event_mask(uint16_t event_id)
{
	struct diag_md_session_t *info = NULL;
	char *event_mask_ptr;
	uint8_t byte_mask;
	int byte_index;
	int bit_index;

	if (diagmem->md_session_mask[DIAG_LOCAL_PROC] != 0) {
		if (diagmem->md_session_mask[DIAG_LOCAL_PROC] &
			MD_PERIPHERAL_MASK(PERIPHERAL_APPS)) {
			if (diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS])
				info =  diagmem->md_session_map[DIAG_LOCAL_PROC][PERIPHERAL_APPS];
		}
	}

	event_mask_ptr = (info) ? info->event_mask->ptr : event_mask.ptr;
	byte_index = event_id / 8;
	bit_index = event_id % 8;
	byte_mask = 0x1 << bit_index;

	if (byte_index >= DCI_EVENT_MASK_SIZE) {
		ALOGE("diag: %s Invalid event ID: %d byte_index: %d \n",
			__func__, event_id, byte_index);
		return 0;
	}
	event_mask_ptr += byte_index;
	return ((*event_mask_ptr & byte_mask) == byte_mask) ? 1 : 0;
}

int diag_event_mask_update_hidl_client(unsigned char *data)
{
	struct diag_md_session_t *info;
	int data_type = EVENT_MASKS_TYPE;
	struct diag_log_mask_t *event_item;
	int len = 0;

	pthread_mutex_lock(&diagmem->session_info_mutex);
	info = diag_md_session_get_peripheral(DIAG_LOCAL_PROC, PERIPHERAL_APPS);
	event_item = info ? info->event_mask->ptr : event_mask.ptr;

	if (!event_item) {
		pthread_mutex_unlock(&diagmem->session_info_mutex);
		return len;
	}

	memcpy(data, &data_type, sizeof(data_type));
	memcpy(data + sizeof(data_type), event_item, BITS_TO_BYTES(event_max_num_bits));
	len = BITS_TO_BYTES(event_max_num_bits) + sizeof(data_type);
	pthread_mutex_unlock(&diagmem->session_info_mutex);

	return len;
}

int diag_log_mask_update_hidl_client(unsigned char *data)
{
	int data_type = LOG_MASKS_TYPE;
	struct diag_md_session_t *info;
	struct diag_log_mask_t *log_item;
	struct diag_log_mask_userspace_t header;
	int i;
	int copy_len = 0, total_len = 0;

	pthread_mutex_lock(&diagmem->session_info_mutex);
	info = diag_md_session_get_peripheral(DIAG_LOCAL_PROC, PERIPHERAL_APPS);
	log_item = info ? info->log_mask->ptr : log_mask.ptr;

	memcpy(data,&data_type,sizeof(int));
	total_len = total_len + sizeof(data_type);
	for (i = 0; i < MAX_EQUIP_ID; i++, log_item++){
		if (!log_item->ptr)
			continue;
		header.equip_id = log_item->equip_id;
		header.num_items = log_item->num_items_tools;
		memcpy(data+total_len, &header, sizeof(header));
		total_len += sizeof(header);
		copy_len = BITS_TO_BYTES(log_item->num_items_tools);
		if ((total_len + copy_len) > MASK_UPDATE_BUF_SIZ) {
			printf("diag: In %s, no space to update log mask, equip_id: %d\n",
					__func__, log_item->equip_id);
			continue;
		}
		memcpy(data + total_len, log_item->ptr, copy_len);
		total_len += copy_len;
	}
	pthread_mutex_unlock(&diagmem->session_info_mutex);
	return total_len;
}

int diag_msg_mask_update_hidl_client(unsigned char *data)
{

	int len;
	int data_type = MSG_MASKS_TYPE;
	struct diag_md_session_t *info;
	struct diag_msg_mask_t *msg_item;
	struct diag_msg_mask_userspace_t header;
	int i;
	int copy_len = 0, total_len = 0;

	pthread_mutex_lock(&diagmem->session_info_mutex);
	info = diag_md_session_get_peripheral(DIAG_LOCAL_PROC, PERIPHERAL_APPS);
	msg_item = info ? info->msg_mask->ptr : msg_mask.ptr;

	memcpy(data,&data_type,sizeof(int));
	total_len = total_len + sizeof(data_type);
	for (i = 0; i < MSG_MASK_TBL_CNT; i++, msg_item++) {

		if (!msg_item->ptr)
			continue;
		len = 0;
		header.ssid_first = msg_item->ssid_first;
		header.ssid_last = msg_item->ssid_last_tools;
		header.range = msg_item->range_tools;
		memcpy(data+total_len, &header, sizeof(header));
		total_len += sizeof(header);
		copy_len = (sizeof(uint32_t) * msg_item->range_tools);
		if ((len + copy_len) > MASK_UPDATE_BUF_SIZ) {
			printf("diag: In %s, no space to update msg mask, first: %d, last: %d\n",
			       __func__, msg_item->ssid_first,
			       msg_item->ssid_last_tools);
			continue;
		}
		memcpy(data + total_len, msg_item->ptr, copy_len);
		total_len += copy_len;
	}

	pthread_mutex_unlock(&diagmem->session_info_mutex);
	return total_len;
}

