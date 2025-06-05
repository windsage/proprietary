#ifndef DIAG_LSMI_H
#define DIAG_LSMI_H

/*===========================================================================

                   Diag Mapping Layer DLL , internal declarations

DESCRIPTION
  Internal declarations for Diag Service Mapping Layer.

Copyright (c)  2007-2015, 2017-2024 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/08   sj      Added featurization for WM specific code & CBSP2.0
02/04/08   mad     Created File
===========================================================================*/

#include "diag_lsm.h"
#include <pthread.h>
#include <stdbool.h>

#define READ_BUF_SIZE 100000
#define DISK_BUF_SIZE 1024*140
#define DISK_FLUSH_THRESHOLD  1024*128

#define HDLC_CRC_LEN	2
#define NON_HDLC_VERSION	1
/* Non-HDLC Header:
 * 1 byte - Control char
 * 1 byte - Version
 * 2 bytes - Packet length
 */
#define DIAG_NON_HDLC_HEADER_SIZE	4

/*
 * Minimum command size: 1 byte
 * Minimum Non-HDLC pkt size:  6 bytes
 * Minimum HDLC pkt size: 4 bytes
 */
#define DIAG_MIN_NON_HDLC_PKT_SIZE	6
#define MIN_CMD_PKT_SIZE 4

#define FILE_LIST_NAME_SIZE 100
#define MAX_FILES_IN_FILE_LIST 100
#define std_strlprintf     snprintf
#define RENAME_CMD_LEN ((2*FILE_NAME_LEN) + 10)

#define DIAGID_V2_FEATURE_COUNT		3
#define NUM_MD_SESSIONS				(NUM_PERIPHERALS + NUM_UPD + 1) /* Added 1 to account for APPS */
#define DIAG_MD_LOCAL				0
#define DIAG_MD_LOCAL_LAST			1
#define DIAG_MD_BRIDGE_BASE			DIAG_MD_LOCAL_LAST
#define DIAG_MD_MDM					(DIAG_MD_BRIDGE_BASE)
#define DIAG_MD_MDM2				(DIAG_MD_BRIDGE_BASE + 1)
#define DIAG_MD_BRIDGE_LAST			(DIAG_MD_BRIDGE_BASE + 2)
#define NUM_DIAG_MD_DEV				DIAG_MD_BRIDGE_LAST
#define DIAG_NUM_PROC				1

#define DIAG_SEC_TO_USEC(x) ((x) * 1000000ULL)
#define MDLOG_WRITTEN_BYTES_LIMIT (ULONG_MAX - (64*1024))

struct buffer_pool {
	int free;
	int data_ready;
	unsigned int bytes_in_buff[NUM_PROC];
	unsigned char *buffer_ptr[NUM_PROC];
	pthread_mutex_t write_mutex;
	pthread_cond_t write_cond;
	pthread_mutex_t read_mutex;
	pthread_cond_t read_cond;
};

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

enum diag_cond_status {
	DIAG_COND_ERROR,
	DIAG_COND_WAIT,
	DIAG_COND_COMPLETE,
};

enum status {
	NOT_READY,
	READY,
};

enum read_thread_status {
	STATUS_RDTHRD_CLR = 0,
	STATUS_RDTHRD_INIT = 1,
	STATUS_RDTHRD_WAIT = 2,
	STATUS_RDTHRD_EXIT = 4,
};

enum restart_thread_status {
	STATUS_RST_THRD_CLR = 0,
	STATUS_RST_THRD_INIT = 1,
	STATUS_RST_THRD_EXIT = 2,
};

typedef struct
{
	int data_type;
	uint32 diagid_mask_supported;
	uint32 diagid_mask_enabled;
} query_pd_feature_mask;

typedef struct
{
	int data_type;
	int ret_val;
	uint32 diagid_mask;
} diag_hw_accel_diag_id_mask;

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
};

struct diag_buffering_mode_t {
	uint8_t peripheral;
	uint8_t mode;
	uint8_t high_wm_val;
	uint8_t low_wm_val;
};

struct diag_debug_all_param_t {
	uint8_t time_api;
	uint8_t timesync_enabled;
	uint8_t timestamp_switch;
	uint8_t stm_support[NUM_PERIPHERALS + 1];
	uint8_t stm_state_requested[NUM_PERIPHERALS + 1];
	uint8_t stm_state[NUM_PERIPHERALS + 1];
	uint32_t apps_feature;
	uint32_t diagid_v2_feature[DIAGID_V2_FEATURE_COUNT];
	uint32_t diagid_v2_status[DIAGID_V2_FEATURE_COUNT];
	uint32_t diag_hw_accel[DIAGID_V2_FEATURE_COUNT];

	struct diag_feature_t feature[NUM_PERIPHERALS + 1];
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
	struct diag_buffering_mode_t dci_buffering_mode[DIAG_NUM_PROC][NUM_PERIPHERALS + 1];
	uint8_t buffering_flag[NUM_MD_SESSIONS];
	int usb_connected;
};

extern int fd_md[NUM_PROC];
extern int gdwClientID;
extern unsigned int diag_lib_dbg_mask;
extern uint8_t diag_id;

void log_to_device(unsigned char *ptr, int logging_mode, int size, int type);
void send_mask_modem(unsigned char mask_buf[], int count_mask_bytes);

/* === Functions dealing with qshrink4 === */

/* Creates threads to read the qshrink4 database threads. */
int create_diag_qshrink4_db_parser_thread(unsigned int peripheral_mask, unsigned int device_mask);

/* Parses the data for qshrink4 command response */
int parse_data_for_qsr4_db_file_op_rsp(uint8 *ptr, int count_received_bytes, int index);
int parse_data_for_qdss_rsp(uint8* ptr, int count_received_bytes, int index);
int parse_data_for_diag_id_rsp(uint8* ptr, int count_received_bytes, int index, int *update_count);
int parse_data_for_adpl_rsp(uint8* ptr, int count_received_bytes, int index);

/* function for interacting with secure diag key info parser */
int parse_data_for_key_info_resp(uint8* ptr, int count_received_bytes, int proc);
int key_info_enabled(void);
int keys_stored(int proc);
int get_keys_header_size(int proc);
int write_key_header(int fd, int proc);
void diag_kill_key_info_threads(void);

/* function to write the qmdlv2 header for qdss binaries */
int write_qdss_header(int fd, int proc);

/* Add qshrink4 guid information to qmdl2 header */
int add_guid_to_qshrink4_header(unsigned char * guid, int p_type, int peripheral);

void get_time_string(char *buffer, int len);
void diag_kill_qshrink4_threads(void);
void diag_kill_qdss_threads(void);
void diag_kill_adpl_threads(void);
void diag_kill_diagid_threads(void);
int delete_log(int type);
int delete_qdss_log(int type);
int diag_send_cmds_to_disable_adpl(int in_ssr);
int diag_reconfigure_qdss(void);
/* function to check diag-id based logging enablement */
bool is_diagid_logging_format_selected(void);
extern boolean gbRemote;
#define DIAG_LSM_PKT_EVENT_PREFIX "DIAG_SYNC_EVENT_PKT_"
#define DIAG_LSM_MASK_EVENT_PREFIX "DIAG_SYNC_EVENT_MASK_"
#endif /* DIAG_LSMI_H */

