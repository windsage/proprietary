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
#ifndef __DIAG_CNTL_H__
#define __DIAG_CNTL_H__

#include "peripheral.h"
#include "masks.h"

#define DIAG_CTRL_MSG_DTR               2
#define DIAG_CTRL_MSG_DIAGMODE          3
#define DIAG_CTRL_MSG_DIAGDATA          4
#define DIAG_CTRL_MSG_FEATURE           8
#define DIAG_CTRL_MSG_EQUIP_LOG_MASK    9
#define DIAG_CTRL_MSG_EVENT_MASK_V2     10
#define DIAG_CTRL_MSG_F3_MASK_V2        11
#define DIAG_CTRL_MSG_NUM_PRESETS       12
#define DIAG_CTRL_MSG_SET_PRESET_ID     13
#define DIAG_CTRL_MSG_LOG_MASK_WITH_PRESET_ID   14
#define DIAG_CTRL_MSG_EVENT_MASK_WITH_PRESET_ID 15
#define DIAG_CTRL_MSG_F3_MASK_WITH_PRESET_ID    16
#define DIAG_CTRL_MSG_CONFIG_PERIPHERAL_TX_MODE 17
#define DIAG_CTRL_MSG_PERIPHERAL_BUF_DRAIN_IMM  18
#define DIAG_CTRL_MSG_CONFIG_PERIPHERAL_WMQ_VAL 19
#define DIAG_CTRL_MSG_DCI_CONNECTION_STATUS     20
#define DIAG_CTRL_MSG_LAST_EVENT_REPORT         22
#define DIAG_CTRL_MSG_LOG_RANGE_REPORT          23
#define DIAG_CTRL_MSG_SSID_RANGE_REPORT         24
#define DIAG_CTRL_MSG_BUILD_MASK_REPORT         25
#define DIAG_CTRL_MSG_DEREG                     27
#define DIAG_CTRL_MSG_DCI_HANDSHAKE_PKT         29
#define DIAG_CTRL_MSG_PD_STATUS                 30
#define DIAG_CTRL_MSG_TIME_SYNC_PKT             31
#define DIAG_CTRL_MSG_PASS_THRU                 35
#define DIAG_CTRL_MSG_F3_MS_MASK                36
#define DIAG_CTRL_MSG_LOG_MS_MASK               37
#define DIAG_CTRL_MSG_EVENT_MS_MASK             38

#define MAX_CNTL_CHNL_BUF_SIZE		8192

#define DIAG_ID_ROOT_STRING "root"
#define DIAG_ID_GVM_STRING "gvm"

#define DIAGID_VERSION_1       1
#define DIAGID_VERSION_2      2
#define DIAGID_VERSION_3	3
#define MAX_DIAGID_STR_LEN       30
#define MIN_DIAGID_STR_LEN       5

#define BITMASK_DIAGID_FMASK		0x0001
#define BITMASK_HW_ACCEL_STM_V1		0x0002
#define BITMASK_HW_ACCEL_ATB_V1		0x0004

struct diag_cntl_hdr {
	uint32_t cmd;
	uint32_t len;
};

struct cmd_range_reg {
	uint16_t first;
	uint16_t last;
	uint32_t data;
};

#define DIAG_CNTL_CMD_REGISTER	1
struct diag_cntl_cmd_reg {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint16_t cmd;
	uint16_t subsys;
	uint16_t count_entries;
	uint16_t port;
	struct cmd_range_reg ranges[];
} __packed;
#define to_cmd_reg(h) container_of(h, struct diag_cntl_cmd_reg, hdr)

#define DIAG_CNTL_CMD_DIAG_MODE 3
struct diag_cntl_cmd_diag_mode
{
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint32_t sleep_vote;
	uint32_t real_time;
	uint32_t use_nrt_values;
	uint32_t commit_threshold;
	uint32_t sleep_threshold;
	uint32_t sleep_time;
	uint32_t drain_timer_val;
	uint32_t event_stale_time_val;
} __packed;

struct diag_cntl_cmd_diag_mode_v2
{
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint32_t sleep_vote;
	uint32_t real_time;
	uint32_t use_nrt_values;
	uint32_t commit_threshold;
	uint32_t sleep_threshold;
	uint32_t sleep_time;
	uint32_t drain_timer_val;
	uint32_t event_stale_time_val;
	uint8_t diag_id;
} __packed;

#define DIAG_CNTL_CMD_FEATURE_MASK 8
struct diag_cntl_cmd_feature {
	struct diag_cntl_hdr hdr;
	uint32_t mask_len;
	uint32_t mask;
} __packed;
#define to_cmd_feature(h) container_of(h, struct diag_cntl_cmd_feature, hdr)

#define DIAG_CNTL_CMD_LOG_MASK 9
struct diag_cntl_cmd_log_mask {
	struct diag_cntl_hdr hdr;
	uint8_t stream_id;
	uint8_t status;
	uint8_t equip_id;
	uint32_t last_item;
	uint32_t log_mask_size;
	uint8_t equip_log_mask[];
} __packed;
#define to_cmd_log_mask(h) container_of(h, struct diag_cntl_cmd_log_mask, hdr)

#define DIAG_CNTL_CMD_MSG_MASK 11
struct diag_cntl_cmd_msg_mask {
	struct diag_cntl_hdr hdr;
	uint8_t stream_id;
	uint8_t status;
	uint8_t msg_mode;
	struct diag_ssid_range_t range;
	uint32_t msg_mask_len;
	uint8_t range_msg_mask[];
} __packed;
#define to_cmd_msg_mask(h) container_of(h, struct diag_cntl_cmd_msg_mask, hdr)

struct diag_ctrl_msg_mask_sub {
	uint32_t cmd_type;
	uint32_t data_len;
	uint8_t version;
	uint8_t stream_id;
	uint8_t preset_id;
	uint8_t status;
	uint8_t msg_mode;
	uint8_t id_valid;
	uint32_t sub_id;
	uint16_t ssid_first;
	uint16_t ssid_last;
	uint32_t msg_mask_size;
	uint8_t range_msg_mask[];
} __packed;

struct diag_ctrl_event_mask_sub {
	uint32_t cmd_type;
	uint32_t data_len;
	uint8_t version;
	uint8_t stream_id;
	uint8_t preset_id;
	uint8_t status;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t event_config;
	uint32_t event_mask_size;
	uint8_t event_mask[];
	/* Copy event mask here */
} __packed;

struct diag_ctrl_log_mask_sub {
	uint32_t cmd_type;
	uint32_t data_len;
	uint8_t version;
	uint8_t stream_id;
	uint8_t preset_id;
	uint8_t status;
	uint8_t id_valid;
	uint32_t sub_id;
	uint8_t equip_id;
	uint32_t num_items;
	uint32_t log_mask_size;
	uint8_t equip_log_mask[];
	/* Copy log mask here */
} __packed;


#define DIAG_CNTL_CMD_EVENT_MASK 10
struct diag_cntl_cmd_event_mask {
	struct diag_cntl_hdr hdr;
	uint8_t stream_id;
	uint8_t status;
	uint8_t event_config;
	uint32_t event_mask_len;
	uint8_t event_mask[];
} __packed;

#define to_cmd_event_mask(h) container_of(h, struct diag_cntl_cmd_event_mask, hdr)

#define DIAG_CNTL_CMD_NUM_PRESETS 12
struct diag_cntl_num_presets {
	struct diag_cntl_hdr hdr;
	uint8_t num;
};

struct cmd_range_dereg {
	uint16_t first;
	uint16_t last;
};

#define DIAG_CNTL_CMD_BUFFERING_TX_MODE	17
#define DIAG_CNTL_CMD_PERIPH_BUF_DRAIN_IMM	18
#define DIAG_CNTL_CMD_CONFIG_PERIPHERAL_WMQ_VAL	19
#define DIAG_MIN_WM_VAL		0
#define DIAG_MAX_WM_VAL		100
 
#define DEFAULT_LOW_WM_VAL	15
#define DEFAULT_HIGH_WM_VAL	85
#define MODE_NONREALTIME	0
#define MODE_REALTIME		1
#define MODE_UNKNOWN		2

struct diag_cntl_cmd_buffering_tx_mode
{
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint8_t stream_id;
	uint8_t tx_mode;
} __packed;

struct diag_cntl_cmd_buffering_tx_mode_v2
{
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint8_t diag_id;
	uint8_t stream_id;
	uint8_t tx_mode;
} __packed;

struct diag_cntl_cmd_config_wm_val{
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint8_t stream_id;
	uint8_t high_wm_val;
	uint8_t low_wm_val;
} __packed;
 
struct diag_cntl_cmd_config_wm_val_v2 {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint8_t diag_id;
	uint8_t stream_id;
	uint8_t high_wm_val;
	uint8_t low_wm_val;
} __packed;

struct diag_cntl_cmd_drain_immediate {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint8_t stream_id;
} __packed;

struct diag_cntl_cmd_drain_immediate_v2 {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint8_t diag_id;
	uint8_t stream_id;
} __packed;
 

#define DIAG_BUFFERING_MODE_STREAMING   0
#define DIAG_BUFFERING_MODE_THRESHOLD   1
#define DIAG_BUFFERING_MODE_CIRCULAR    2

#define DIAG_CNTL_CMD_DEREGISTER	27
struct diag_cntl_cmd_dereg {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint16_t cmd;
	uint16_t subsys;
	uint16_t count_entries;
	struct cmd_range_dereg ranges[];
} __packed;
#define to_cmd_dereg(h) container_of(h, struct diag_cntl_cmd_dereg, hdr)

struct diag_cntl_cmd_single_dereg {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint16_t cmd;
	uint16_t subsys;
	uint16_t count_entries;
	struct cmd_range_dereg range;
} __packed;

#define DIAG_CNTL_CMD_DIAG_ID 33
struct diag_cntl_cmd_diag_id {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint32_t diag_id;
	char process_name[MAX_DIAGID_STR_LEN];
} __packed;
#define to_cmd_diag_id(h) container_of(h, struct diag_cntl_cmd_diag_id, hdr)
struct diag_cntl_cmd_diag_id_v2 {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint32_t diag_id;
	uint32_t feature_len;
	uint32_t pd_feature_mask;
	char process_name[MAX_DIAGID_STR_LEN];
} __packed;
 #define to_cmd_diag_id_v2(h) container_of(h, struct diag_cntl_cmd_diag_id_v2, hdr)
struct diag_id_tbl_t {
	struct list_head link;
	uint8_t diag_id;
	uint8_t pd_val;
	int periph_id;
	uint8_t pd_feature_mask;
	char *process_name;
} __packed;

struct diag_id_t {
	uint8_t diag_id;
	uint8_t len;
	char *process_name;
} __packed;

struct diag_perif_drain {
	int peripheral;
};
#define DIAG_CNTL_CMD_DIAG_SETTINGS 40
#define DIAG_CNTL_PKT_DIAG_SETTINGS_HDR_SIZ 28
struct diag_cntl_cmd_diag_settings {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint32_t settings_id;
	uint32_t settings_version;
	uint32_t diag_id;
	uint32_t settings_payload_len;
	uint8_t settings_payload[0];

};

struct diag_settings_tbl_t {
	struct list_head link;
	int periph_id;
	uint32_t settings_id;
	uint32_t settings_version;
	uint32_t diag_id;
	uint32_t payload_len;
	char *payload;
} __packed;
 
#define to_cmd_diag_settings(h) container_of(h, struct diag_cntl_cmd_diag_settings, hdr)

#define DIAG_CNTL_CMD_STM_STATE 21
struct diag_cntl_cmd_diag_stm_state {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint8_t control_data;

} __packed;
extern struct list_head diag_id_list;
#define DIAG_CNTL_CMD_TIME_SYNC		31
struct diag_cntl_cmd_time_sync {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint8_t time_api;
} __packed;
struct diag_cntl_cmd_passthru {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint32_t diagid_mask;
	uint8_t hw_accel_type;
	uint8_t hw_accel_ver;
	uint8_t control_data;
} __packed;

#define DIAG_CNTL_CMD_PKT_FORMAT_REQ	41
struct diag_cntl_cmd_pkt_format_req {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint8_t mask;
} __packed;

#define to_cmd_pkt_format_req(h) container_of(h, struct diag_cntl_cmd_pkt_format_req, hdr)

bool diag_check_diag_id_valid(uint8_t diag_id);
int diag_query_last_diag_id(void);
int diag_query_diag_id(char *process_name, uint8_t *diag_id);
int diag_add_diag_id_to_list(uint8_t diag_id, char *process_name,
				uint8_t pd_val, struct peripheral *perif);
void process_diagid_v2_feature_mask(uint32_t diag_id,
			uint32_t pd_feature_mask);
uint8_t diag_query_diag_id_from_pd(uint8_t pd_val,
			uint8_t *diag_id, int *periph_id);
int diag_cntl_recv(struct peripheral *perif, const void *buf, size_t len);

void diag_cntl_send_log_mask(struct peripheral *peripheral,
	uint32_t equip_id,
	uint32_t sub_id,
	int pid,
	uint8_t preset_id);

void diag_cntl_send_msg_mask(struct peripheral *peripheral,
	struct diag_ssid_range_t *range,
	uint32_t sub_id,
	int pid,
	int preset);

void diag_cntl_send_event_mask(struct peripheral *peripheral,
	uint32_t sub_id, uint8_t preset_id, int pid);

void diag_cntl_send_time_switch_msg(struct peripheral *peripheral, uint8_t time_api);
void diag_cntl_close(struct peripheral *peripheral);

void diag_cntl_send_masks(struct peripheral *peripheral);

void diag_cntl_set_real_time_mode(struct peripheral *perif, bool real_time, int diag_id);
int diag_cntl_set_buffering_mode(struct peripheral *perif, struct diag_buffering_mode_t *params,
				int mode, int stream_id);
int diag_config_perif_buf(struct diag_buffering_mode_t *params, int stream_id);
int diag_cntl_drain_perif_buf(int diag_id, int periph_id, int stream_id);
int diag_cntl_send_stm_state(uint32_t periph_id, uint8_t control_data);
int diag_cntl_send_passthru_control_pkt(struct diag_hw_accel_cmd_req_t *req_params);
int diag_cntl_query_pd_featuremask(struct diag_client *dm,
	struct diag_hw_accel_query_sub_payload_rsp_t *query_params);
int diag_cntl_query_pd_featuremask_local(
	struct diag_hw_accel_query_sub_payload_rsp_t *query_params);
int diag_query_peripheral_from_pd(uint8_t pd_val);
struct peripheral * diag_get_periph_info(int periph_id);
void diag_map_pd_to_diagid(uint8_t pd, uint8_t *diag_id, int *peripheral);
void diag_send_real_time(void);
void diag_update_proc_vote(uint16_t proc, uint8_t vote, int index);
void diag_update_real_time_vote(uint16_t proc, uint8_t real_time, int index);
void diag_cntl_send_pkt_format_request(struct peripheral *peripheral, uint8_t mask);
#ifdef __cplusplus
extern "C" {
#endif
int diag_peripheral_data_drain_immediate(struct diag_perif_drain *buf, int len);
int diag_set_peripheral_buffering_mode(struct diag_buffering_mode_t *params, int len);
void diag_cntl_send_feature_mask(struct peripheral *peripheral, uint32_t mask);
#ifdef __cplusplus
}
#endif

#endif
