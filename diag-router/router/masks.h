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
#ifndef MASKS_H_
#define MASKS_H_

#include "diag.h"
#include "util.h"


#define MSG_MASK_TBL_CNT		29
#define APPS_EVENT_LAST_ID		0x0D72

#define MAX_SSID_PER_RANGE	200

extern uint16_t event_max_num_bits;

/* LOG CODES */
static const uint32_t log_code_last_tbl[] = {
	0x0,	/* EQUIP ID 0 */
	0x1E51,	/* EQUIP ID 1 */
	0x0,	/* EQUIP ID 2 */
	0x0,	/* EQUIP ID 3 */
	0x4910,	/* EQUIP ID 4 */
	0x5420,	/* EQUIP ID 5 */
	0x0,	/* EQUIP ID 6 */
	0x74FF,	/* EQUIP ID 7 */
	0x0,	/* EQUIP ID 8 */
	0x0,	/* EQUIP ID 9 */
	0xA38A,	/* EQUIP ID 10 */
	0xB9FF,	/* EQUIP ID 11 */
	0x0,	/* EQUIP ID 12 */
	0xD1FF,	/* EQUIP ID 13 */
	0x0,	/* EQUIP ID 14 */
	0x0,	/* EQUIP ID 15 */
};

#define NUM_OF_MASK_RANGES 29
static const uint32_t ssid_first_arr[NUM_OF_MASK_RANGES] =
	{     0,  500,  1000,  2000,  3000,  4000,  4500,  4600,
	   5000,  5500,  6000,  6500,  7000,  7100,  7200,  8000,
	   8500,  9000,  9500, 10200, 10251, 10300, 10350, 10400,
	  10500,  10600, 10801, 11057, 0xC000 };

static const uint32_t ssid_last_arr[NUM_OF_MASK_RANGES] =
	{   142,   506,  1007,  2008,  3014,  4010,  4584,  4616,
	   5037,  5517,  6082,  6521,  7003,  7111,  7201,  8000,
	   8532,  9008,  9521, 10210, 10255, 10300, 10377, 10416,
	  10505,  10620, 10836, 11079, 0xC063 };

#define LOG_GET_ITEM_NUM(xx_code)	(xx_code & 0x0FFF)
#define LOG_GET_EQUIP_ID(xx_code)	((xx_code & 0xF000) >> 12)
#define MSG_RANGE_TO_SIZE(range)	(((range).ssid_last - (range).ssid_first +1) * 4)

#define MAX_SIM_NUM 4
#define INVALID_INDEX -1
#define LEGACY_MASK_CMD 0
#define SUBID_CMD 1


#define MAX_EQUIP_ID	16
#define EVENT_MASK_SIZE 513
#define MAX_ITEMS_PER_EQUIP_ID	512
#define MAX_ITEMS_ALLOWED	0xFFF

#define DIAG_CTRL_MASK_INVALID		0
#define DIAG_CTRL_MASK_ALL_DISABLED	1
#define DIAG_CTRL_MASK_ALL_ENABLED	2
#define DIAG_CTRL_MASK_VALID		3

#define LOG_MASK_CTRL_HEADER_LEN_SUB	18
#define MSG_MASK_CTRL_HEADER_LEN_SUB	18
#define EVENT_MASK_CTRL_HEADER_LEN_SUB	14

#define LOG_STATUS_SUCCESS	0
#define LOG_STATUS_INVALID	1
#define LOG_STATUS_FAIL		2


struct diag_log_mask_t {
	uint8_t equip_id;
	uint32_t num_items;
	uint32_t num_items_tools;
	uint32_t range;
	uint32_t range_tools;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t *ptr;
}__packed;

struct diag_ssid_range_temp {
	uint16_t ssid_first;
	uint16_t ssid_last;
};


struct diag_ssid_range_t {
	uint16_t ssid_first;
	uint16_t ssid_last;
}__packed;

struct diag_msg_mask_t {
	uint32_t ssid_first;
	uint32_t ssid_last;
	uint32_t ssid_last_tools;
	uint32_t range;
	uint32_t range_tools;
	uint8_t id_valid;
	uint32_t sub_id;
	uint32_t *ptr;
}__packed;

struct diag_multisim_masks {
	uint8_t *sub_ptr;
	uint8_t status;
	struct diag_multisim_masks *next;
};

enum mask_status_opr{
	mask_status_set,
	mask_status_get,
};

struct diag_mask_info {
	void *ptr;
	struct diag_multisim_masks *ms_ptr;
	int mask_len;
	uint8_t status;
}__packed;

struct diag_msg_mask_userspace_t {
	uint32_t ssid_first;
	uint32_t ssid_last;
	uint32_t range;
} __packed;

struct diag_log_mask_userspace_t {
	uint8_t equip_id;
	uint32_t num_items;
} __packed;
extern struct diag_mask_info msg_mask;
extern struct diag_mask_info msg_bt_mask;
extern struct diag_mask_info log_mask;
extern struct diag_mask_info event_mask;

struct diag_msg_ssid_query_sub_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
} __packed;

struct diag_log_cmd_mask {
	uint32_t equip_id;
	uint32_t num_items;
	uint8_t mask[0];
}__packed;

typedef struct _diag_cmd_op_get_msg_mask_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id; /*possible values 1-2 number identifying the preset ID*/
	uint8_t reserved; /*set to 0*/
	uint16_t ssid_first;
	uint16_t ssid_last;
} __packed diag_cmd_op_get_msg_mask_sub_req_t;

typedef struct _diag_cmd_op_get_msg_mask_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;/*should be same as preset ID field in request*/
	uint8_t status;
	uint16_t ssid_first;
	uint16_t ssid_last;
	uint32_t rt_masks[0];
} __packed diag_cmd_op_get_msg_mask_sub_rsp_t;

typedef struct _diag_cmd_op_get_build_mask_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint16_t ssid_first;
	uint16_t ssid_last;
} __packed diag_cmd_op_get_build_mask_sub_req_t;

typedef struct _diag_cmd_op_get_build_mask_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t reserved;
	uint8_t status;
	uint16_t ssid_first;
	uint16_t ssid_last;
	uint32_t masks[0];
} __packed diag_cmd_op_get_build_mask_sub_rsp_t;

typedef struct _diag_cmd_op_get_ssid_range_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
} __packed diag_cmd_op_get_ssid_range_sub_req_t;

typedef struct _diag_cmd_op_get_ssid_range_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t status;
	uint8_t reserved;
	uint32_t count;
	struct diag_ssid_range_t ranges[0];
} __packed diag_cmd_op_get_ssid_range_sub_rsp_t;

typedef struct _diag_cmd_op_set_log_mask_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t operation_code;
	uint8_t preset_id;
	uint8_t status;
	uint32_t equip_id;
	uint32_t num_items;
	uint8_t mask[0];
} __packed diag_cmd_op_set_log_mask_sub_req_t;

typedef struct _diag_cmd_op_set_log_mask_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t operation_code;
	uint8_t preset_id;
	uint8_t status;
	uint32_t equip_id;
	uint32_t num_items;
	uint8_t mask[0];
} __packed diag_cmd_op_set_log_mask_sub_rsp_t;

typedef struct _diag_cmd_op_get_log_range_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t operation_code;
} __packed diag_cmd_op_get_log_range_sub_req_t;

typedef struct _diag_cmd_op_get_log_range_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t operation_code;
	uint8_t preset_id;
	uint8_t status;
	uint32_t ranges[MAX_EQUIP_ID];
} __packed diag_cmd_op_get_log_range_sub_rsp_t;

typedef struct _diag_cmd_op_log_disable_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t operation_code;
	uint8_t preset_id;
	uint8_t reserved;
} __packed diag_cmd_op_log_disable_sub_req_t;

typedef struct _diag_cmd_op_log_disable_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t operation_code;
	uint8_t preset_id;
	uint8_t status;
} __packed diag_cmd_op_log_disable_sub_rsp_t;

typedef struct _diag_cmd_op_set_all_msg_mask_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;
	uint8_t status;
	uint32_t rt_mask;
} __packed diag_cmd_op_set_all_msg_mask_sub_req_t;

typedef struct _diag_cmd_op_set_all_msg_mask_sub_rsp_t{

	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;
	uint8_t status;
	uint32_t rt_mask;
} __packed diag_cmd_op_set_all_msg_mask_sub_rsp_t;

typedef struct _diag_cmd_op_set_msg_mask_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;
	uint8_t reserved;
	uint16_t ssid_first;
	uint16_t ssid_last;
	uint32_t masks[0];
} __packed diag_cmd_op_set_msg_mask_sub_req_t;

typedef struct _diag_cmd_op_set_msg_mask_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;
	uint8_t status;
	uint16_t ssid_first;
	uint16_t ssid_last;
	uint32_t rt_masks[0];
} __packed diag_cmd_op_set_msg_mask_sub_rsp_t;

typedef struct _diag_cmd_op_get_event_mask_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;
	uint8_t status;
} __packed diag_cmd_op_get_event_mask_sub_req_t;

typedef struct _diag_cmd_op_get_event_mask_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;
	uint8_t status;
	uint16_t num_bits;
	uint8_t mask[0];
} __packed diag_cmd_op_get_event_mask_sub_rsp_t;

typedef struct _diag_cmd_op_set_event_mask_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;
	uint8_t status;
	uint16_t num_bits;
	uint8_t mask[0];
} __packed diag_cmd_op_set_event_mask_sub_req_t;

typedef struct _diag_cmd_op_set_event_mask_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;
	uint8_t status;
	uint16_t num_bits;
	uint8_t mask[0];
} __packed diag_cmd_op_set_event_mask_sub_rsp_t;


typedef struct _diag_cmd_op_get_log_mask_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t operation_code;
	uint8_t preset_id;
	uint8_t status; /*reserved in the FR doc*/
	uint32_t equip_id;
}__packed diag_cmd_op_get_log_mask_sub_req_t;

typedef struct _diag_cmd_op_get_log_mask_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t operation_code;
	uint8_t preset_id;
	uint8_t status;
	uint32_t equip_id;
	uint32_t num_items;
	uint8_t mask[0];
}__packed diag_cmd_op_get_log_mask_sub_rsp_t;

typedef struct _diag_cmd_op_event_toggle_sub_req_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;
	uint8_t operation_switch;
} __packed diag_cmd_op_event_toggle_sub_req_t;

typedef struct _diag_cmd_op_event_toggle_sub_rsp_t{
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t sub_cmd;
	uint8_t preset_id;
	uint8_t status;
} __packed diag_cmd_op_event_toggle_sub_rsp_t;

int diag_check_subid_mask_index(uint32_t subid, int pid);

#define MSG_MASK_SIZE	(MSG_MASK_TBL_CNT * sizeof(struct diag_msg_mask_t))
#define LOG_MASK_SIZE	(MAX_EQUIP_ID * sizeof(struct diag_log_mask_t))

int diag_masks_init(void);
void diag_masks_exit(void);

uint8_t diag_get_log_mask_status(struct diag_mask_info * mask_info);
void diag_cmd_disable_log(int pid, uint32_t sub_id);

void diag_cmd_get_log_range(uint32_t *ranges, uint32_t count,
		uint32_t sub_id, int pid);

int diag_cmd_set_log_mask(uint8_t equip_id, uint32_t *num_items,
	uint8_t *mask, uint32_t *mask_size, int pid, uint32_t sub_id);

int diag_cmd_get_log_mask(uint32_t equip_id, uint32_t *num_items, uint8_t ** mask, uint32_t *mask_size,
						  struct diag_log_mask_t *log_item);

uint8_t diag_get_build_mask_status();
void diag_cmd_get_ssid_range(
	uint32_t *count, 
	struct diag_ssid_range_t **ranges,
	uint32_t sub_id,
	int pid );

int diag_cmd_get_build_mask(struct diag_ssid_range_t *range, 
	uint32_t **mask, uint32_t sub_id, int pid);

uint8_t diag_get_msg_mask_status(struct diag_mask_info * mask_info);

int diag_cmd_get_msg_mask(struct diag_ssid_range_t *range,
	uint32_t sub_id,
	uint32_t **mask,
	int pid );

int diag_cmd_set_msg_mask(struct diag_ssid_range_t range, 
							const uint32_t *mask,	uint32_t sub_id, int pid);

void diag_cmd_set_all_msg_mask(uint32_t mask, 	uint32_t sub_id, int pid);

uint8_t diag_get_event_mask_status(struct diag_mask_info * mask_info);
int diag_cmd_get_event_mask(uint16_t num_bits, uint8_t **mask, void *event_item);
int diag_cmd_update_event_mask(uint16_t num_bits, const uint8_t *mask, uint32_t sub_id, int pid);
void diag_cmd_toggle_events(bool enabled, int pid, uint32_t sub_id);

int clients_broadcast_msg_mask(void);
int clients_broadcast_log_mask(void);
int clients_broadcast_event_mask(void);
int diag_event_mask_update_client(struct diag_client *dm);
int diag_log_mask_update_client(struct diag_client *dm);
int diag_msg_mask_update_client(struct diag_client *dm);
int diag_log_mask_copy(struct diag_mask_info *dest,
			struct diag_mask_info *src);
int diag_msg_mask_copy(struct diag_md_session_t *new_session,
			struct diag_mask_info *dest, struct diag_mask_info *src);
int diag_event_mask_copy(struct diag_mask_info *dest,
			struct diag_mask_info *src);
void diag_log_mask_free(struct diag_mask_info *mask_info);
void diag_msg_mask_free(struct diag_mask_info *mask_info, struct diag_md_session_t *session_info);
void diag_event_mask_free(struct diag_mask_info *mask_info);
int diag_check_event_mask(uint16_t event_id);

int diag_create_msg_mask_table(
	struct diag_msg_mask_t *mask,
	int subid_index);

void * diag_get_mask(
	uint32_t sub_id,
	struct diag_mask_info *diag_mask,
	int pid );

uint8_t diag_masks_status_opr(
	uint32_t sub_id,
	struct diag_mask_info *diag_mask,
	int pid,
	enum mask_status_opr opr,
	uint8_t status);

#ifdef __cplusplus
extern "C" {
#endif
int diag_event_mask_update_hidl_client(unsigned char *data);
int diag_log_mask_update_hidl_client(unsigned char *data);
int diag_msg_mask_update_hidl_client(unsigned char *data);
#ifdef __cplusplus
}
#endif
#endif /* MASKS_H_ */
