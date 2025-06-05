/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
#ifndef __DIAG_H__
#define __DIAG_H__

#include <stdint.h>
#include <sys/types.h>

#include "circ_buf.h"
#include "hdlc.h"
#include "list.h"
#include "peripheral.h"
#include "watch.h"
#include <log/log.h>
#include "common_log.h"

#define DIAG_ROUTER_LOG_TAG "Diag-Router"

#ifdef ALOGE
#undef ALOGE
#endif
#define ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR,DIAG_ROUTER_LOG_TAG,__VA_ARGS__)

#ifdef ALOGD
#undef ALOGD
#endif
#define ALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,DIAG_ROUTER_LOG_TAG,__VA_ARGS__)

#ifdef ALOGI
#undef ALOGI
#endif
#define ALOGI(...)  __android_log_print(ANDROID_LOG_INFO,DIAG_ROUTER_LOG_TAG,__VA_ARGS__)

#define ALOGM(mask, ...)					\
do {								\
	if (mask & diag_debug_mask)				\
		__android_log_print(ANDROID_LOG_DEBUG,		\
			DIAG_ROUTER_LOG_TAG, __VA_ARGS__);	\
} while(0)

#define DEFAULT_SOCKET_PORT 2500
#define DEFAULT_BAUD_RATE 115200

#define BIT(x) (1 << (x))

#define PRINT_HEX_DUMP_LEN			8

#define DIAG_FEATURE_FEATURE_MASK_SUPPORT				BIT(0)
#define DIAG_FEATURE_DIAG_MASTER_SETS_COMMON_MASK	BIT(1)
#define DIAG_FEATURE_LOG_ON_DEMAND_APPS				BIT(2)
#define DIAG_FEATURE_DIAG_VERSION_RSP_ON_MASTER		BIT(3)
#define DIAG_FEATURE_REQ_RSP_SUPPORT					BIT(4)
#define DIAG_FEATURE_DIAG_PRESET_MASKS					BIT(5)
#define DIAG_FEATURE_APPS_HDLC_ENCODE					BIT(6)
#define DIAG_FEATURE_STM									BIT(9)
#define DIAG_FEATURE_PERIPHERAL_BUFFERING				BIT(10)
#define DIAG_FEATURE_MASK_CENTRALIZATION				BIT(11)
#define DIAG_FEATURE_SOCKETS_ENABLED					BIT(13)
#define DIAG_FEATURE_DCI_EXTENDED_HEADER				BIT(14)
#define DIAG_FEATURE_DIAG_ID								BIT(15)
#define DIAG_FEATURE_PKT_HEADER_UNTAG					BIT(16)
#define DIAG_FEATURE_PD_BUFFERING						BIT(17)
#define DIAG_FEATURE_DIAG_ID_FEATURE_MASK				BIT(19)
#define DIAG_FEATURE_DIAG_MULTI_SIM_SUPPORT			BIT(20)
#define DIAG_FEATURE_DIAG_ID_LOGGING				BIT(21)

#define DIAG_CMD_SUBSYS_DISPATCH       75
#define DIAG_CMD_SUBSYS_DISPATCH_V2	128
#define DIAG_CMD_NO_SUBSYS_DISPATCH	0xff
#define DIAG_CMD_DIAG_SUBSYS 18
#define DIAG_MAX_REQ_SIZE	(16 * 1024)
#define DIAG_MAX_RSP_SIZE	(16 * 1024)
#define DIAG_CMD_RSP_BAD_COMMAND                        0x13
#define DIAG_CMD_RSP_BAD_PARAMS                         0x14
#define DIAG_CMD_RSP_BAD_LENGTH                         0x15
#define DIAG_CMD_DIAG_LOG_ON_DEMAND 0x78
#define DIAGID_V2_FEATURE_COUNT 3
#define NUM_MD_SESSIONS		(NUM_PERIPHERALS + NUM_UPD)
#define DIAG_HW_ACCEL_CMD	0x224
#define DIAG_MAX_BAD_CMD	5
/*
  * HW Acceleration operation definition
  */
#define DIAG_HW_ACCEL_OP_DISABLE	0
#define DIAG_HW_ACCEL_OP_ENABLE	1
#define DIAG_HW_ACCEL_OP_QUERY	2

/*
 * HW Acceleration TYPE definition
 */
#define DIAG_HW_ACCEL_TYPE_ALL	0
#define DIAG_HW_ACCEL_TYPE_STM	1
#define DIAG_HW_ACCEL_TYPE_ATB	2
#define DIAG_HW_ACCEL_TYPE_MAX	2
#define DIAG_HW_ACCEL_VER_MIN 1
#define DIAG_HW_ACCEL_VER_MAX 1
#define DIAG_ID_APPS 1
/*
 * HW Acceleration CMD Error codes
 */
#define DIAG_HW_ACCEL_STATUS_SUCCESS	0
#define DIAG_HW_ACCEL_FAIL	1
#define DIAG_HW_ACCEL_INVALID_TYPE	2
#define DIAGIDV2_FEATURE(f_index)	\
 	diagmem->diagid_v2_feature[f_index]

#define DIAGIDV2_STATUS(f_index)	\
 	diagmem->diagid_v2_status[f_index]

#define P_FMASK_DIAGID_V2(peripheral)	\
 	diagmem->feature[peripheral].diagid_v2_feature_mask
#define DIAG_HW_ACCEL_INVALID_VER	3

/* Diag-ID based command related definitions */
#define DIAG_CMD_DIAG_ID_CMD_REQ	0xA1
#define DIAG_CMD_DIAG_ID_CMD_VERSION_1	1
#define DIAG_ID_CMD_RESP_TIMESTAMP_SIZE	8

#define DIAG_CMD_DIAG_ID_ERR_RESPONSE		0xA2
#define DIAG_CMD_DIAG_ID_ERR_RESPONSE_VER_1	1

/* Definitions for Transport Query */
#define DIAG_ROUTE_TO_UART	1
#define DIAG_ROUTE_TO_USB	2
#define DIAG_ROUTE_TO_PCIE	3

#define MSG_MASKS_TYPE		0x00000001
#define LOG_MASKS_TYPE		0x00000002
#define EVENT_MASKS_TYPE	0x00000004
#define PKT_TYPE		0x00000008
#define DEINIT_TYPE		0x00000010
#define USER_SPACE_DATA_TYPE	0x00000020
#define DCI_DATA_TYPE		0x00000040
#define USER_SPACE_RAW_DATA_TYPE	0x00000080
#define DCI_LOG_MASKS_TYPE	0x00000100
#define DCI_EVENT_MASKS_TYPE	0x00000200
#define DCI_PKT_TYPE		0x00000400
#define TIMESTAMP_SWITCH_TYPE	0x00000800
#define HDLC_SUPPORT_TYPE	0x00001000
#define DCI_BUFFERING_MODE_STATUS_TYPE 0x00002000

#define MD_PERIPHERAL_MASK(x)	(1 << (x))

#define DIAG_MD_LOCAL		0
#define DIAG_MD_LOCAL_LAST	1
#define DIAG_MD_BRIDGE_BASE	DIAG_MD_LOCAL_LAST
#define DIAG_MD_MDM		(DIAG_MD_BRIDGE_BASE)
#define DIAG_MD_MDM2		(DIAG_MD_BRIDGE_BASE + 1)
#define DIAG_MD_BRIDGE_LAST	(DIAG_MD_BRIDGE_BASE + 2)
#define NUM_DIAG_MD_DEV		DIAG_MD_BRIDGE_LAST
#define DIAG_LOCAL_PROC	0
#define DIAG_MD_NONE			0
#define DIAG_MD_PERIPHERAL		1

#define USB_MODE		1
#define MEMORY_DEVICE_MODE	2
#define NO_LOGGING_MODE		3
#define UART_MODE		4
#define SOCKET_MODE		5
#define CALLBACK_MODE		6
#define PCIE_MODE		7
#define QRTR_SOCKET_MODE	8

#define DIAG_PROC_VOTE_REALTIME_DCI			1
#define DIAG_PROC_VOTE_REALTIME_MEMORY_DEVICE		2
#define DIAG_NUM_PROC 1
#define DIAG_GET_MD_DEVICE_SIG_MASK(proc) (0x100000 * (1 << proc))

#define DIAG_STATUS_OPEN (0x00010000)   /* DCI channel open status mask   */
#define DIAG_STATUS_CLOSED (0x00020000)
#define USER_SPACE_DATA 16384
#define USER_SPACE_HDLC_BUF 32771

#define MDM_TOKEN	-1
#define MDM_2_TOKEN	-2

#define VOTE_DOWN			0
#define VOTE_UP				1

#define MAX_DCI_CLIENTS 10

#define ALL_PROC 	-1
#define DIAG_PROC_DCI			1
#define DIAG_PROC_MEMORY_DEVICE		2

#define READ_BUF_SIZE 100000

#define DATA_TYPE_EVENT                 0
#define DATA_TYPE_F3                    1
#define DATA_TYPE_LOG                   2
#define DATA_TYPE_RSP           3
#define DATA_TYPE_DELAYED_RSP   4
#define DATA_TYPE_DCI				0x00000040
#define DATA_TYPE_DCI_LOG			0x00000100
#define DATA_TYPE_DCI_EVENT			0x00000200
#define DATA_TYPE_DCI_PKT			0x00000400
#define DATA_TYPE_MULTI_STREAM_LOG		(DATA_TYPE_DCI_LOG | DATA_TYPE_LOG)

#define MODE_CMD        0x29
#define MODE_CMD_RESET  2

#define DIAG_STM_MODEM	0x01
#define DIAG_STM_LPASS	0x02
#define DIAG_STM_WCNSS	0x04
#define DIAG_STM_APPS	0x08
#define DIAG_STM_SENSORS 0x10
#define DIAG_STM_CDSP	0x20
#define DIAG_STM_NPU	0x40
#define DIAG_STM_NSP1	0x80
#define DIAG_STM_GPDSP0	0x100
#define DIAG_STM_GPDSP1	0x200

/* PD Logging Separation definitions */
#define ENABLE_PKT_HEADER_UNTAGGING	1
#define DISABLE_PKT_HEADER_UNTAGGING	0

#define PID_MAX (4 * 1024 * 1024) /* 4MB */

#if defined FEATURE_SDXKUNO_DIAG
#define UNIX_CLIENT_BUF_COUNT	1
#else
#define UNIX_CLIENT_BUF_COUNT   5
#endif
#define CONTROL_CHAR 0x7E

#define MAX_PKT_LEN 0x4000
#define MIN_PKT_LEN 0X04

#define HDLC_DIFF_LEN 0X02

/* Event packet related definition */
#define EVENT_PKT_HEADER_SIZE	3
#define EVENT_ID_SIZE		2
#define EVENT_ID_MASK		0xFFF

#define QRTR_PKT_MAX_SIZE      (16 * 1024)

#if defined(DIAG_TELE_VM_ENABLED) || defined(DIAG_FOTA_VM_ENABLED)
#define DIAG_VM_ENABLED
#endif

#define DIAG_STREAM_1 1
#define DIAG_STREAM_2 2

/* Packet format select req related definitions */
#define PKT_FORMAT_SELECT_REQ_VERSION1	1

#define PKT_FORMAT_MASK_CMD_REQ_RESP	0x1
#define PKT_FORMAT_MASK_ASYNC_PKT	(0x1 << 1)
#define PKT_FORMAT_INVALID_MASK		~(PKT_FORMAT_MASK_CMD_REQ_RESP | PKT_FORMAT_MASK_ASYNC_PKT)

#define DIAG_PKT_FORMAT_SELECT_TYPE	0x01000000

#define PKT_FORMAT_ALL_MASK		0xFF

extern unsigned int diag_debug_mask;
extern unsigned int etr1_support;
extern unsigned int vm_enabled;

enum diag_dbg_mask {
	DIAG_DBG_MASK_INFO		= 0x00000001,
	DIAG_DBG_MASK_USB		= 0x00000002,
	DIAG_DBG_MASK_MHI		= 0x00000004,
	DIAG_DBG_MASK_CNTL		= 0x00000008,
	DIAG_DBG_MASK_CMD		= 0x00000010,
	DIAG_DBG_MASK_DATA		= 0x00000020,
	DIAG_DBG_MASK_WATCH		= 0x00000040,
	DIAG_DBG_MASK_MUX		= 0x00000080,
	DIAG_DBG_MASK_PCIE		= 0x00000100,
	DIAG_DBG_MASK_DCI		= 0x00000200,
	DIAG_DBG_MASK_LIB_LEGACY	= 0x00000400,
	DIAG_DBG_MASK_LIB_QDSS		= 0x00000800,
	DIAG_DBG_MASK_LIB_VERBOSE	= 0x00001000,
	DIAG_DBG_MASK_ALL		= 0xFFFFFFFF,
};

enum diag_cmd_err_code{
	CMD_ERR_GENERAL = 1,
	CMD_ERR_INVALID_VERSION,
	CMD_ERR_INVALID_OPCODE,
	CMD_ERR_INVALID_DIAGID
};

enum diag_id_cmd_opt{
	DIAG_ID_FLAG_CMD_VALIDITY,
	DIAG_ID_FLAG_GET_DIAGID,
};

struct diag_client {
	const char *name;
	int fd;
	int in_fd;
	int out_fd;
	int hdlc_enc_done;
	int use_mbuf;
	uint64_t pkt_count;
	uint32_t drop_count;
	uint32_t copy_count;
	uint32_t max_pkt_count;

	int pid; /* actual pid of dm */
	int override_pid; /* pid of a different dm that this dm will use instead */

	bool hdlc_encoded;
	bool enabled;

	unsigned char *data_buf;
	unsigned int data_bytes_read;
	unsigned char *rsp_buf;
	uint8_t *hdlc_read_buf;

	struct mbuf *dm_mbuf[UNIX_CLIENT_BUF_COUNT];
	struct circ_buf recv_buf;
	struct hdlc_decoder recv_decoder;

	struct list_head cmd_rsp_q;
	struct list_head outq;
	struct list_head node;
	struct watch_flow *flow;
};

struct diag_md_session_t {
	int pid;
	int peripheral_mask[NUM_DIAG_MD_DEV];
	uint8_t hdlc_disabled;
	uint8_t msg_mask_tbl_count;
	struct diag_mask_info *msg_mask;
	struct diag_mask_info *log_mask;
	struct diag_mask_info *event_mask;
	struct diag_client *dm;
	int fd;
	int kill_count;
	int pd_mask;
};

struct diag_cmd {
	struct list_head node;

	unsigned int first;
	unsigned int last;

	struct peripheral *peripheral;
	int fd;
	int(*cb)(struct diag_client *client, const void *buf, size_t len, int pid);
};

struct diag_cmd_dbg {
	unsigned int first;
	unsigned int last;
	int periph_id;
};

struct diag_logging_mode_param_t {
	uint32_t req_mode;
	uint32_t peripheral_mask;
	uint32_t pd_mask;
	uint8_t mode_param;
	uint8_t diag_id;
	uint8_t pd_val;
	uint8_t reserved;
	int peripheral;
	int device_mask;
	int pid;
} __packed;

struct diag_callback_reg_t {
	int proc;
} __packed;

struct diag_feature_t {
	uint32_t feature_mask;
	uint8_t rcvd_feature_mask;
	uint8_t log_on_demand;
	uint8_t separate_cmd_rsp;
	uint8_t encode_hdlc;
	uint8_t untag_header;
	uint8_t peripheral_buffering;
	uint8_t pd_buffering;
	uint8_t mask_centralization;
	uint8_t stm_support;
	uint8_t sockets_enabled;
	uint8_t sent_feature_mask;
	uint8_t diag_id_support;
	uint8_t diagid_v2_feature_mask;
	uint8_t multi_sim_support;
	uint8_t diag_id_logging;
};

struct real_time_vote_t {
	int client_id;
	uint16_t proc;
	uint8_t real_time_vote;
} __packed;

struct real_time_query_t {
	int real_time;
	int proc;
} __packed;

struct diag_pkt_frame_t {
	uint8_t start;
	uint8_t version;
	uint16_t length;
	unsigned char data[0];
};

struct diag_driver {
	/* DCI related variables */
	pthread_mutex_t dci_mutex;
	struct list_head dci_req_list;
	struct list_head dci_client_list;
	uint8_t time_api;
	uint8_t timesync_enabled;
	uint8_t timestamp_switch;
	uint8_t stm_support[NUM_PERIPHERALS];
	uint8_t stm_state_requested[NUM_PERIPHERALS];
	uint8_t stm_state[NUM_PERIPHERALS];
	uint32_t apps_feature;
	uint32_t diagid_v2_feature[DIAGID_V2_FEATURE_COUNT];
	uint32_t diagid_v2_status[DIAGID_V2_FEATURE_COUNT];
	uint32_t diag_hw_accel[DIAGID_V2_FEATURE_COUNT];
	uint8_t diagid_v2_feature_mask;
	uint16_t delayed_rsp_id;
	struct diag_feature_t feature[NUM_PERIPHERALS];
	int logging_mode[NUM_DIAG_MD_DEV];
	int logging_mask[NUM_DIAG_MD_DEV];
	int pd_logging_mode[NUM_UPD];
	int pd_session_clear[NUM_UPD];
	int num_pd_session;
	uint32_t md_session_mask[NUM_DIAG_MD_DEV];
	uint8_t md_session_mode[NUM_DIAG_MD_DEV];
	struct diag_md_session_t *md_session_map[NUM_DIAG_MD_DEV]
					[NUM_MD_SESSIONS];
	int real_time_mode[DIAG_NUM_PROC];
	uint16_t proc_active_mask;
	uint16_t proc_rt_vote_mask[DIAG_NUM_PROC];
	struct diag_buffering_mode_t buffering_mode[NUM_MD_SESSIONS];
	uint8_t buffering_flag[NUM_MD_SESSIONS];
	struct diag_buffering_mode_t dci_buffering_mode[DIAG_NUM_PROC][NUM_PERIPHERALS];
	int usb_connected;
	int dci_tag;
	int dci_client_id[MAX_DCI_CLIENTS];
	int num_dci_client;
	unsigned char *apps_dci_buf;
	int dci_state;
	int pcie_state;
	int transport_set;
	int mask_set_status;
	pthread_mutex_t session_info_mutex;
	pthread_mutex_t hdlc_disable_mutex;
	uint8_t hdlc_disabled;
	uint8_t p_hdlc_disabled[NUM_MD_SESSIONS];
	uint8_t proc_hdlc_disabled[NUM_DIAG_MD_DEV];
	/* To save priority diag-id configured by tool */
	uint8_t priority_diagid;
	uint8_t diag_id;
};

struct diag_pkt_header_t {
	uint8_t cmd_code;
	uint8_t subsys_id;
	uint16_t subsys_cmd_code;
} __packed;
struct diag_hw_accel_op_t {
	uint8_t hw_accel_type;
	uint8_t hw_accel_ver;
	uint32_t diagid_mask;
} __packed;

struct diag_hw_accel_cmd_req_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t operation;
	uint16_t reserved;
	struct diag_hw_accel_op_t op_req;
} __packed;


struct diag_hw_accel_op_resp_payload_t {
	uint8_t status;
	uint8_t hw_accel_type;
	uint8_t hw_accel_ver;
	uint32_t diagid_status;
}__packed;


struct diag_hw_accel_cmd_op_resp_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t operation;
	uint16_t reserved;
	struct diag_hw_accel_op_resp_payload_t op_rsp;
} __packed;


struct diag_hw_accel_query_sub_payload_rsp_t {
	uint8_t hw_accel_type;
	uint8_t hw_accel_ver;
	uint32_t diagid_mask_supported;
	uint32_t diagid_mask_enabled;
} __packed;


struct diag_hw_accel_query_rsp_payload_t {
	uint8_t status;
	uint8_t diag_transport;
	uint8_t num_accel_rsp;
	struct diag_hw_accel_query_sub_payload_rsp_t
		sub_query_rsp[DIAG_HW_ACCEL_TYPE_MAX][DIAG_HW_ACCEL_VER_MAX];
} __packed;


struct diag_hw_accel_cmd_query_resp_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t operation;
	uint16_t reserved;
	struct diag_hw_accel_query_rsp_payload_t query_rsp;
} __packed;

struct diag_stm_cmd_req_t {
	struct diag_pkt_header_t header;
	uint8_t version;
	union {
		struct {
			uint8_t mask;
			uint8_t cmd;
		} __packed v2;
		struct {
			uint16_t mask;
			uint8_t cmd;
		} __packed v3;
	};
} __packed;

struct diag_con_all_param_t {
	uint32_t diag_con_all;
	uint32_t num_peripherals;
	uint32_t upd_map_supported;
};
struct diag_debug_all_param_t {
	uint8_t time_api;
	uint8_t timesync_enabled;
	uint8_t timestamp_switch;
	uint8_t stm_support[NUM_PERIPHERALS];
	uint8_t stm_state_requested[NUM_PERIPHERALS];
	uint8_t stm_state[NUM_PERIPHERALS];
	uint32_t apps_feature;
	uint32_t diagid_v2_feature[DIAGID_V2_FEATURE_COUNT];
	uint32_t diagid_v2_status[DIAGID_V2_FEATURE_COUNT];
	uint32_t diag_hw_accel[DIAGID_V2_FEATURE_COUNT];

	struct diag_feature_t feature[NUM_PERIPHERALS];
	int logging_mode[NUM_DIAG_MD_DEV];
	int logging_mask[NUM_DIAG_MD_DEV];
	int pd_logging_mode[NUM_UPD];
	int pd_session_clear[NUM_UPD];
	int num_pd_session;
	uint32_t md_session_mask[NUM_DIAG_MD_DEV];
	uint8_t md_session_mode[NUM_DIAG_MD_DEV];

	int real_time_mode[DIAG_NUM_PROC];
	uint16_t proc_active_mask;
	uint16_t proc_rt_vote_mask[DIAG_NUM_PROC];
	struct diag_buffering_mode_t buffering_mode[NUM_MD_SESSIONS];
	struct diag_buffering_mode_t dci_buffering_mode[DIAG_NUM_PROC][NUM_PERIPHERALS];
	uint8_t buffering_flag[NUM_MD_SESSIONS];
	int usb_connected;
};
struct diag_query_pid_t {
	uint32_t peripheral_mask;
	uint32_t pd_mask;
	int pid;
	int device_mask;
	int kill_op;
	int kill_count;
};

typedef struct{
	uint16_t cmd;
	uint16_t subsys;
	uint16_t first;
	uint16_t last;
}diag_cmd_reg_entry;

struct diag_cmd_reg_tbl{
	uint32_t count;
	diag_cmd_reg_entry entries[0];
};

/* structure defined to send packet format select request from router to lib */
struct diag_pkt_format_request {
	int type;
	uint8_t mask;
} __packed;

/**
 * strucutre to store the diag_pkt_format_request info
 * from tool
 */
struct diag_pkt_format_selection {
	bool status;
	uint8_t mask;
};

struct diag_id_cmd_req_header_t {
	uint8_t cmd_code;
	uint8_t version;
	uint8_t diag_id : 5;
	uint8_t reserv : 3;
	uint8_t reserv2;
} __packed;

struct diag_id_cmd_resp_header_t {
	uint8_t cmd_code;
	uint8_t version;
	uint8_t diag_id : 5;
	uint8_t time_type : 3;
	uint8_t reserv;
	uint32_t time_lo;
	uint32_t time_hi;
} __packed;

enum err_resp
{
	ERR_DUPLICATE_CMD,
	ERR_MAX = 0xF,
};

typedef struct
{
	uint32_t diag_id_mask;
} err_data_type_duplicate_cmd;

/* add all types of error data here */
typedef union
{
	err_data_type_duplicate_cmd dup_cmd;
} err_data_type_max_size;

typedef struct
{
	uint8_t err_code;
	uint8_t version;
	uint8_t diag_id :5;
	uint8_t ts_type :3;
	uint8_t err_type;
	uint32_t ts_lo;
	uint32_t ts_hi;
} diag_id_err_resp;

/* structures defined to support diag cmd registration table retrieval */
struct diag_cmd_registration_table_req {
	struct diag_pkt_header_t header;
	uint8_t version;
	uint8_t opcode;
	uint8_t diag_id;
} __packed;

struct cmd_table_list {
	uint8_t diag_id;
	uint8_t cmd_code;
	uint8_t subsys_id;
	uint16_t subsys_cmd_code_s;
	uint16_t subsys_cmd_code_e;
} __packed;

struct diag_cmd_registration_table_rsp {
	struct diag_pkt_header_t header;
	uint32_t status;
	uint16_t delayed_rsp_id;
	int16_t rsp_cnt;
	uint8_t version;
	uint16_t no_of_entries;
	struct cmd_table_list cmd_list[0];
} __packed;

/* structure defined to notify vm diag-id to diag lib */
struct diag_id_notify_pkt {
	int type;
	uint8_t diag_id;
} __packed;

/* strcture defined to support event packet generation in diag-router */
struct event_id_type
{
	uint16_t id             :12;
	uint16_t reserved       :1;
	uint16_t payload_len    :2;	/* payload length (0, 1, 2, see payload) */
	uint16_t time_trunc_flag :1;
};

struct event_header
{
	uint8_t  cmd_code;	/* 96 (0x60) */
	uint16_t length;	/* Length of packet */
	union
	{
		struct event_id_type event_id_field;
	}
	event_id;
	uint32_t ts_lo;		/* Time stamp */
	uint32_t ts_hi;
	int8_t payload_length;
} __packed;

struct process_name_id
{
	int8_t diagid;
	int8_t name_len;
	char pname[30];
} __packed;

struct diag_id_event
{
	struct event_header head;
	struct process_name_id payload;
} __packed;

extern struct diag_client *socket_dm;
extern struct diag_client *pcie_dm;
extern struct diag_driver *diagmem ;
extern pthread_mutex_t write_mutex;

void queue_push(struct diag_client *dm, struct list_head *queue, const void *msg, size_t msglen);
void queue_push_flow(struct diag_client *dm, struct list_head *queue, const void *msg, size_t msglen,
			struct watch_flow *flow);
void queue_push_cntlq(struct peripheral *peripheral, const void *msg,
			size_t msglen);
extern struct list_head diag_cmds;
extern struct list_head apps_cmds;
#ifndef FEATURE_LE_DIAG
extern struct list_head apps_system_cmds;
#endif /* FEATURE_LE_DIAG */

int diag_sock_connect(const char *hostname, unsigned short port);
int diag_uart_open(const char *uartname, unsigned int baudrate);
int diag_usb_open(const char *ffs_name);
int diag_pcie_open(void);
int diag_unix_open(void);

int diag_client_handle_command(struct diag_client *client, uint8_t *data, size_t len, int pid);
void diag_clear_masks(struct diag_client *client, int pid);

int hdlc_enqueue(struct list_head *queue, const void *buf, size_t msglen);
int hdlc_enqueue_flow(struct diag_client *dm, struct list_head *queue, const void *buf,
	size_t msglen, struct watch_flow *flow);
void *hdlc_encode(struct diag_client *dm, const void *src, size_t slen, size_t *dlen);
void register_fallback_cmd(unsigned int cmd,
			   int(*cb)(struct diag_client *client,
				    const void *buf, size_t len, int pid));
void register_fallback_subsys_cmd(unsigned int subsys, unsigned int cmd,
	int(*cb)(struct diag_client *client,
	const void *buf, size_t len, int pid));
void register_fallback_subsys_cmd_range(unsigned int subsys, unsigned int cmd_code_low,
	unsigned int cmd_code_high,
	int(*cb)(struct diag_client *client,
	const void *buf, size_t len, int pid));
void register_fallback_subsys_cmd_v2(unsigned int subsys, unsigned int cmd,
	int(*cb)(struct diag_client *client,
	const void *buf, size_t len, int pid));
void register_system_cmd(unsigned int cmd,
	unsigned int subsys_cmd,
	int(*cb)(struct diag_client *client));

void register_common_cmd(unsigned int cmd, int(*cb)(struct diag_client *client,
						    const void *buf,
						    size_t len, int pid));

void register_app_cmds(void);
void register_common_cmds(void);

void register_common_subsys_cmd(unsigned int subsys, unsigned int cmd,
				int(*cb)(struct diag_client *client,
				const void *buf, size_t len, int pid));
int diag_send_cmd_reg_table(struct diag_cmd_registration_table_req *req, int pid);
void diag_stm_init(void);
int diag_add_apps_cmd_registrations(int fd,
			      unsigned char * buf, size_t len);
int diag_remove_cmd_registrations(int fd);
void pkt_format_select_req_save(uint8_t mask);
bool pkt_format_select_check_mask(uint8_t mask);
uint8_t pkt_format_select_get_mask(void);

#ifdef __cplusplus
extern "C" {
#endif
uint16_t diag_get_next_delayed_rsp_id(void);
int diag_get_remote_mask_info(void);
void diag_query_diag_session_pid(struct diag_query_pid_t *param);
void diag_clear_system_data_ready(void);
int diag_switch_logging(struct diag_client *dm, struct diag_logging_mode_param_t *param, int len);
int diag_process_userspace_data(struct diag_client *dm, unsigned char  *buf, int len, int pid);
int diag_process_userspace_raw_data(struct diag_client *dm, unsigned char  *buf, int len, int pid);
void diag_close_logging_process(struct diag_client *dm, int pid);
int diag_process_vote_realtime(struct real_time_vote_t *vote, int len);
int diag_get_real_time(struct real_time_query_t * rt_query, int len);
int diag_query_pd_logging(struct diag_logging_mode_param_t *param);
int diag_timestamp_switch_update_hidl_client(unsigned char *data);
int diag_hdlc_toggle(int pid, uint8_t hdlc_support);
int diag_mdm_hdlc_toggle(int pid, uint8_t hdlc_support);
void diag_process_apps_data(void *src_buf, int src_len, void *dest_buf, int dest_len, int cmd_rsp_flag);
#ifdef __cplusplus
}
#endif
/* Refactor these so USB doesnt need to expose enable/disable */
int __diag_usb_open(const char *ffs_name);
int diag_usb_bridge_enable(int fd, int device_id);
int diag_usb_bridge_disable(int fd);
struct diag_md_session_t *diag_md_session_get_pid(int pid);
struct diag_md_session_t *diag_md_session_get_peripheral(int proc,
							uint8_t peripheral);

void diag_notify_md_client(uint8_t proc, uint8_t peripheral, int data);
int diag_dci_init(void);
void diag_dci_notify_client(int peripheral_mask, int data, int proc);
void diag_dci_process_peripheral_data(struct peripheral *perif, void *buf,
				      int recd_bytes);
int diag_timestamp_switch_update_client(struct diag_client *dm);
int diag_md_session_match_pid_peripheral(int proc, int pid, uint8_t peripheral);
int diag_start_hdlc_recovery(int pid, size_t msglen);
void diag_process_stm_mask(uint8_t cmd, uint16_t data_mask, int data_type);

#endif
