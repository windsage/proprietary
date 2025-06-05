/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2012-2019, The Linux Foundation. All rights reserved.

              Diag DCI support

GENERAL DESCRIPTION

Implementation of communication over DCI channels with peripheral diag and
support logging over DCI from apps userspace clients.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#ifndef __DIAG_DCI_H__
#define __DIAG_DCI_H__

#include "watch.h"

/* 16 log code categories, each has:
 * 1 bytes equip id + 1 dirty byte + 512 byte max log mask
 */

#define DCI_LOG_MASK_SIZE		(16*514)
#define DCI_EVENT_MASK_SIZE		512
#define DCI_MASK_STREAM			2
#define DCI_MAX_LOG_CODES		16
#define DCI_MAX_ITEMS_PER_LOG_CODE	512

#define MIN_DELAYED_RSP_LEN		12
/*
 * Maximum data size that peripherals send = 8.5K log +
 * DCI header + footer (6 bytes)
 */
#define MAX_DCI_PACKET_SZ		8710

#define DCI_LOG_MASK_CLEAN		0
#define DCI_LOG_MASK_DIRTY		1
#define NUM_DCI_PROC		1
#define DCI_REMOTE_DATA	0
#define VALID_DCI_TOKEN(x)	((x >= 0 && x < NUM_DCI_PROC) ? 1 : 0)
#define DCI_PKT_RSP_CODE	0x93
#define DCI_DELAYED_RSP_CODE	0x94
#define DCI_CONTROL_PKT_CODE	0x9A
#define EXT_HDR_CMD_CODE	0x98
#define LOG_CMD_CODE		0x10
#define SECURE_LOG_CMD_CODE	0x9E
#define EVENT_CMD_CODE		0x60
#define DCI_PKT_RSP_TYPE	0
#define DCI_LOG_TYPE		-1
#define DCI_EVENT_TYPE		-2
#define DCI_EXT_HDR_TYPE	-3
#define SET_LOG_MASK		1
#define DISABLE_LOG_MASK	0
#define MAX_EVENT_SIZE		512
#define DCI_CLIENT_INDEX_INVALID -1
#define DCI_LOG_CON_MIN_LEN		16
#define DCI_EVENT_CON_MIN_LEN		16

#define EXT_HDR_LEN		8
#define EXT_HDR_VERSION		1

#define DCI_BUF_PRIMARY		1
#define DCI_BUF_SECONDARY	2
#define DCI_BUF_CMD		3
#define DCI_LOCAL_PROC		0
#define DCI_REMOTE_BASE		1
#define DCI_MDM_PROC		DCI_REMOTE_BASE
#define DCI_MDM_2_PROC		(DCI_REMOTE_BASE + 1)
#define DCI_REMOTE_LAST		(DCI_REMOTE_BASE + 2)
#define NUM_DCI_PERIPHERALS	(NUM_PERIPHERALS)
#define IN_BUF_SIZE		16384
#define DCI_HDR_SIZE					\
	((sizeof(struct diag_dci_pkt_header_t) >	\
	sizeof(struct diag_dci_header_t)) ?		\
	(sizeof(struct diag_dci_pkt_header_t) + 1) :	\
	(sizeof(struct diag_dci_header_t) + 1))		\

#define DCI_BUF_SIZE (uint32_t)(DIAG_MAX_REQ_SIZE + DCI_HDR_SIZE)

#define DCI_REQ_HDR_SIZE				\
	((sizeof(struct dci_pkt_req_t) >		\
	sizeof(struct dci_stream_req_t)) ?		\
	(sizeof(struct dci_pkt_req_t)) :		\
	(sizeof(struct dci_stream_req_t)))		\

#define DCI_REQ_BUF_SIZE (uint32_t)(DIAG_MAX_REQ_SIZE + DCI_REQ_HDR_SIZE)
#define DCI_MAGIC		(0xAABB1122)

struct dci_pkt_req_t {
	int uid;
	int client_id;
} __packed;

struct dci_stream_req_t {
	int type;
	int client_id;
	int set_flag;
	int count;
} __packed;

struct dci_pkt_req_entry_t {
	int client_id;
	int uid;
	int tag;
	struct list_head track;
} __packed;
enum {
	DIAG_DCI_NO_ERROR = 1001,	/* No error */
	DIAG_DCI_NO_REG,		/* Could not register */
	DIAG_DCI_NO_MEM,		/* Failed memory allocation */
	DIAG_DCI_NOT_SUPPORTED,	/* This particular client is not supported */
	DIAG_DCI_HUGE_PACKET,	/* Request/Response Packet too huge */
	DIAG_DCI_SEND_DATA_FAIL,/* writing to kernel or peripheral fails */
	DIAG_DCI_TABLE_ERR	/* Error dealing with registration tables */
};
struct diag_dci_buffer_t {
	unsigned char *data;
	unsigned int data_len;
	uint8_t in_busy;
	uint8_t buf_type;
	int data_source;
	int capacity;
	uint8_t in_list;
	struct list_head buf_track;
};
struct diag_dci_reg_tbl_t {
	int client_id;
	uint16_t notification_list;
	int signal_type;
	int token;
} __packed;
struct diag_dci_health_t {
	int dropped_logs;
	int dropped_events;
	int received_logs;
	int received_events;
};
struct diag_dci_client_tbl {
	int pid;
	struct diag_dci_reg_tbl_t client_info;
	struct diag_client *dm;
	unsigned char *dci_log_mask;
	unsigned char *dci_event_mask;
	uint8_t real_time;
	struct list_head track;
	struct diag_dci_buf_peripheral_t *buffers;
	uint8_t num_buffers;
	uint8_t in_service;
	struct list_head list_write_buf;
	struct diag_buffering_mode_t vote_buffering_mode[NUM_DCI_PROC][NUM_PERIPHERALS];
};
struct diag_dci_buf_peripheral_t {
	struct diag_dci_buffer_t *buf_curr;
	struct diag_dci_buffer_t *buf_primary;
	struct diag_dci_buffer_t *buf_cmd;
	struct diag_dci_health_t health;
};
struct diag_dci_health_stats {
	struct diag_dci_health_t stats;
	int reset_status;
};

struct diag_dci_health_stats_proc {
	int client_id;
	struct diag_dci_health_stats health;
	int proc;
} __packed;
/* This is used for querying DCI Log or Event Mask */
struct diag_log_event_stats {
	int client_id;
	uint16_t code;
	int is_set;
} __packed;

struct diag_dci_pkt_rsp_header_t {
	int type;
	int length;
	uint8_t delete_flag;
	int uid;
} __packed;

struct diag_dci_pkt_header_t {
	uint8_t start;
	uint8_t version;
	uint16_t len;
	uint8_t pkt_code;
	int tag;
} __packed;

struct diag_dci_header_t {
	uint8_t start;
	uint8_t version;
	uint16_t length;
	uint8_t cmd_code;
} __packed;

struct dci_ops_tbl_t {
	int ctx;
	int mempool;
	unsigned char log_mask_composite[DCI_LOG_MASK_SIZE];
	unsigned char event_mask_composite[DCI_EVENT_MASK_SIZE];
	int (*send_log_mask)(int token);
	int (*send_event_mask)(int token);
	uint16_t peripheral_status;
} __packed;
struct diag_dci_peripherals_t {
	int proc;
	uint16_t list;
} __packed;
struct diag_dci_buffering_mode_t {
	int client_id;
	int proc;
	struct diag_buffering_mode_t buffering_mode_params;
};
struct diag_dci_perif_drain {
	int proc;
	int peripheral;
};
#define DCI_HDR_SIZE					\
	((sizeof(struct diag_dci_pkt_header_t) >	\
	  sizeof(struct diag_dci_header_t)) ?		\
	(sizeof(struct diag_dci_pkt_header_t) + 1) :	\
	(sizeof(struct diag_dci_header_t) + 1))		\

#define DCI_BUF_SIZE (uint32_t)(DIAG_MAX_REQ_SIZE + DCI_HDR_SIZE)

#define DCI_REQ_HDR_SIZE				\
	((sizeof(struct dci_pkt_req_t) >		\
	  sizeof(struct dci_stream_req_t)) ?		\
	(sizeof(struct dci_pkt_req_t)) :		\
	(sizeof(struct dci_stream_req_t)))		\

#define DCI_REQ_BUF_SIZE (uint32_t)(DIAG_MAX_REQ_SIZE + DCI_REQ_HDR_SIZE)
#ifdef __cplusplus
extern "C" {
#endif

int diag_dci_register_client(struct diag_dci_reg_tbl_t *reg_entry, struct diag_client *dm, int pid);
int diag_dci_get_support_list(struct diag_dci_peripherals_t *support_list);
int diag_dci_copy_health_stats(struct diag_dci_health_stats_proc *stats_proc);
struct diag_dci_client_tbl *diag_dci_get_client_entry(int client_id);
int diag_dci_clear_log_mask(int client_id);
int diag_dci_clear_event_mask(int client_id);
int diag_dci_deinit_client(struct diag_dci_client_tbl *entry);
int diag_dci_query_log_mask(struct diag_dci_client_tbl *entry,
                                uint16_t log_code);
int diag_dci_query_event_mask(struct diag_dci_client_tbl *entry,
                                uint16_t event_id);
int diag_process_dci_transaction(unsigned char *buf, int len);
void diag_process_apps_dci_read_data(void *buf, int recd_bytes, struct watch_flow *flow);
int diag_dci_mask_update_hidl_client(unsigned char *data, int data_type);
int diag_cleanup_dci_client_entry(int pid);
int diag_dci_vote_peripheral_buffering_mode(struct diag_dci_buffering_mode_t *params, int len);
int diag_dci_peripheral_data_drain_immediate(struct diag_dci_perif_drain *params, int len);
int diag_dci_send_buffering_mode_update_to_clients(int proc, int mode);
int diag_dci_buffering_mode_update_hidl_client(int peripheral, unsigned char *pkt, int data_type);
#ifdef __cplusplus
}
#endif
/* DCI Log streaming functions */
void update_dci_cumulative_log_mask(int offset, unsigned int byte_index,
					uint8_t byte_mask, int token);
void diag_dci_invalidate_cumulative_log_mask(int token);
int diag_send_dci_log_mask(int token);
void extract_dci_log(unsigned char *buf, int len, int data_source, int token,
			void *ext_hdr);
/* DCI event streaming functions */
void update_dci_cumulative_event_mask(int offset, uint8_t byte_mask, int token);
void diag_dci_invalidate_cumulative_event_mask(int token);
int diag_send_dci_event_mask(int token);
void extract_dci_events(unsigned char *buf, int len, int data_source,
			int token, void *ext_hdr);
/* DCI extended header handling functions */
void extract_dci_ext_pkt(unsigned char *buf, int len, int data_source,
				int token);

uint8_t diag_dci_get_cumulative_real_time(int token);
int diag_dci_set_real_time(struct diag_dci_client_tbl *entry,
				uint8_t real_time);
struct diag_dci_client_tbl *diag_dci_find_client_entry(struct diag_client *dm);
#endif
