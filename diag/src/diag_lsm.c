/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2007-2024 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag Legacy Service Mapping DLL

GENERAL DESCRIPTION

Implementation of entry point and Initialization functions for Diag_LSM.dll.


EXTERNALIZED FUNCTIONS
DllMain
Diag_LSM_Init
Diag_LSM_DeInit

INITIALIZATION AND SEQUENCING REQUIREMENTS


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include <stdlib.h>
#include "comdef.h"
#include "stdio.h"
#include "stringl.h"
#include "diag_lsmi.h"
#include "diagsvc_malloc.h"
#include "diag_lsm_event_i.h"
#include "diag_lsm_log_i.h"
#include "diag_lsm_msg_i.h"
#include "diag.h" /* For definition of diag_cmd_rsp */
#include "diag_lsm_pkt_i.h"
#include "diag_lsm_dci_i.h"
#include "diag_lsm_dci.h"
#include "diag_shared_i.h" /* For different constants */
#include "diag_lsm_comm.h"
#include "diag_lsm.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "errno.h"
#include <pthread.h>
#include <stdint.h>
#include <eventi.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/select.h>
#include <ctype.h>
#include <limits.h>

/* strlcpy is from OpenBSD and not supported by Linux Embedded.
 * GNU has an equivalent g_strlcpy implementation into glib.
 * Featurized with compile time USE_GLIB flag for Linux Embedded builds.
 */
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

#ifdef FEATURE_LE_DIAG
int tgkill(int tgid, int tid, int sig) {
 return syscall(__NR_tgkill, tgid, tid, sig);
}
#endif
#define DIAG_QUERY_REMOTE_DEV_MASK 		0x00010000
#define DIAG_QUERY_DCI_CLIENT_ID		0x00020000
#define DIAG_QUERY_DCI_SUPPORT_LIST		0x00040000
#define DIAG_QUERY_DCI_HEALTH_STATS		0x00080000
#define DIAG_QUERY_CON_ALL_MASK 		0x00016000
#define DIAG_QUERY_MD_PID_MASK  		0x00032000
#define DIAG_QUERY_REG_CB_MASK			0x00064000
#define DIAG_QUERY_DCI_LOG_MASK			0x00100000
#define DIAG_QUERY_DCI_EVENT_MASK		0x00200000
#define DIAG_QUERY_REAL_TIME			0x00400000
#define DIAG_SWITCH_LOGGING_MASK		0x00160000
#define DIAG_TIMESTAMP_SWITCH  			0x00320000
/* diag-router updates diag-id to diag lib for diag-id based async packets */
#define DIAG_UPDATE_DIAG_ID_TYPE		0x00640000
/* diag-router updates pkt format mask that tool configured */
#define DIAG_UPDATE_PKT_FORMAT_SELECT_MASK_TYPE	0x01000000
#define DIAG_QUERY_REG_TABLE_MASK		0x10000000
#define DIAG_QUERY_DEBUG_ALL_MASK		0x20000000
#define DIAG_SET_DYNAMIC_DEBUG_MASK		0x40000000
#define DIAG_QUERY_END_OF_PACKET		0xABABABAB

#define MAX_USER_PKT_SIZE		16384
#define USER_SPACE_DATA_TYPE_SIZE	4

#define DCI_HEADER_LENGTH	sizeof(int)
#define DCI_LEN_FIELD_LENGTH	sizeof(int)
#define DCI_EVENT_OFFSET	sizeof(uint16)
#define DCI_DEL_FLAG_LEN	sizeof(uint8)
#define DCI_EXT_HDR_LENGTH	8

#define CALLBACK_TYPE_LEN	4
#define CALLBACK_PROC_TYPE_LEN	4

#define MAX_GUID_ENTRIES 128
#define DIAG_QMDL2_HDR_MIN_LEN 10

#define SIGTIMER (SIGRTMAX - 1)

#define DIAG_DEINIT_TIMEOUT_IN_MSEC	200
#define DIAG_THRD_JOIN_TIMEOUT_IN_MSEC	50

#define CONNECTION_RETRY 0
#define MASK_SYNC_RETRY 1

/**
 * Bit mask of packet format select request has 2 bits as
 * listed below
 * Bit 0 : Enable/disable diag-id based cmd request/response
 * Bit 1 : Enable/disable diag-id based async packet
 * Below macro is to mask diag-id based async packet bit
*/
#define PKT_FORMAT_MASK_ASYNC_PKT	(0x1 << 1)

/* enum defined to handle full/partial packet case */
typedef enum {
        PKT_START,
        PKT_HEADER,
        PKT_PAYLOAD
} diag_pkt_states;

static unsigned int diag_peripheral_mask = 0;
static unsigned int diag_device_mask = 0;
static unsigned int diag_pd_mask = 0;
int gdwClientID = 0;
int diag_fd = -1;		/* File descriptor for DIAG device */
/* File descriptor for Memory device */
int fd_md[NUM_PROC] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int fd_uart = -1;	/* File descriptor for UART device */
int fd_socket[MAX_SOCKET_CHANNELS] = {-1, -1, -1};	/* File descriptor for socket */
int fd_dev = -1;	/* Generic file descriptor */
int num_bytes_read;
int disable_hdlc;
unsigned long count_written_bytes[NUM_PROC];
/* This is for non MEMORY logging*/
unsigned long count_written_bytes_1 = 0;
char file_name_curr[NUM_PROC][FILE_NAME_LEN];
extern char qdss_file_name_curr[NUM_PROC][FILE_NAME_LEN];
extern int qsr_state;

char file_name_del[FILE_NAME_LEN] = "/sdcard/diag_logs/diag_log_";
char qdss_file_name_del[FILE_NAME_LEN] = "/sdcard/diag_logs/diag_qdss_log_";

char mask_file_proc[NUM_PROC][FILE_NAME_LEN] =
{	"/sdcard/diag_logs/Diag.cfg",
	"/sdcard/diag_logs/Diag.cfg",
	"/sdcard/diag_logs/Diag.cfg",
	"/sdcard/diag_logs/Diag.cfg",
	"/sdcard/diag_logs/Diag.cfg",
	"/sdcard/diag_logs/Diag.cfg",
	"/sdcard/diag_logs/Diag.cfg",
	"/sdcard/diag_logs/Diag.cfg",
	"/sdcard/diag_logs/Diag.cfg",
	"/sdcard/diag_logs/Diag.cfg"
};
char mask_file2_proc[NUM_PROC][FILE_NAME_LEN] =
{	"/sdcard/diag_logs/Diag.cfg2",
	"/sdcard/diag_logs/Diag.cfg2",
	"/sdcard/diag_logs/Diag.cfg2",
	"/sdcard/diag_logs/Diag.cfg2",
	"/sdcard/diag_logs/Diag.cfg2",
	"/sdcard/diag_logs/Diag.cfg2",
	"/sdcard/diag_logs/Diag.cfg2",
	"/sdcard/diag_logs/Diag.cfg2",
	"/sdcard/diag_logs/Diag.cfg2",
	"/sdcard/diag_logs/Diag.cfg2",
};
char output_dir[NUM_PROC][FILE_NAME_LEN] = {
			"/sdcard/diag_logs/"
};
char qsr4_xml_file_name[FILE_NAME_LEN] = "/sdcard/diag_logs/";
/* This array is used for proc names */
char proc_name[NUM_PROC][6] = {"", "/mdm", "/mdm2", "/mdm3", "/mdm4", "/qsc", "",
							"", "", ""};
int logging_mode = USB_MODE;
int uart_logging_proc = MSM;
char* file_list[NUM_PROC] = {NULL, NULL, NULL, NULL, NULL,
				NULL, NULL, NULL, NULL, NULL};
char* qdss_file_list[NUM_PROC] = {NULL, NULL, NULL, NULL, NULL,
				NULL, NULL, NULL, NULL, NULL};

int file_list_size[NUM_PROC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int qdss_file_list_size[NUM_PROC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int file_list_index[NUM_PROC] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int qdss_file_list_index[NUM_PROC] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int kill_mdlog_flag = 0;

char dir_name[FILE_NAME_LEN];
char peripheral_name[FILE_NAME_LEN];
char upd_name[FILE_NAME_LEN];
int use_qmdl2_v2;
int use_qmdl2_v2_hdlc_disable;
int diagid_guid_mapping = 0;
int qmdl2_v2_header_inited[NUM_PROC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int qmdl2_v3_header_key_inited[NUM_PROC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
extern int qshrink4_filelist_rsp[NUM_PROC];

static struct diag_callback_tbl_t cb_clients[NUM_PROC];
static int socket_inited = 0;
static int (*socket_cb_ptr)(void *socket_cb_data_ptr, int socket_id);
static void *socket_cb_data_ptr;
static int socket_token[MAX_SOCKET_CHANNELS];
static unsigned int file_count[NUM_PROC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
extern unsigned int qdss_file_count[NUM_PROC];

static unsigned int circular_logging_inited = 0;
static int lsm_init_count = 0;
static void init_circular_logging(void);
static int create_oldest_file_list(char *oldest_dir, int type);
static struct timespec calculate_waiting_time(int ms);
static boolean diag_retry_initialization (uint8_t operation);

/* Globals related to diag wakelocks */
#define WAKELOCK_FILE		"/sys/power/wake_lock"
#define WAKEUNLOCK_FILE		"/sys/power/wake_unlock"
static int wl_inited;
static int fd_wl;
static int fd_wul;
static char *wl_name;
uint8 hdlc_disabled[NUM_PROC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8 write_qshrink_header[NUM_PROC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define NUM_SYNC_EVENTS_DIAG_LSM 2
#define SYNC_EVENT_DIAG_LSM_PKT_IDX 0
#define SYNC_EVENT_DIAG_LSM_MASK_IDX 1
char mask_file_list[FILE_NAME_LEN] = "/sdcard/diag_logs/Diag_list.txt";
static char default_mask_file[NUM_PROC][36] = {
    "/sdcard/diag_logs/Diag.cfg",
    "/sdcard/diag_logs/mdm/Diag.cfg",
    "/sdcard/diag_logs/mdm2/Diag.cfg",
    "/sdcard/diag_logs/mdm3/Diag.cfg",
    "/sdcard/diag_logs/mdm4/Diag.cfg",
    "/sdcard/diag_logs/mdm5/Diag.cfg",
    "/sdcard/diag_logs/Diag.cfg",
    "/sdcard/diag_logs/Diag.cfg",
    "/sdcard/diag_logs/Diag.cfg",
    "/sdcard/diag_logs/Diag.cfg" };
int mask_file_mdm_entered = 0;
int mask_file_entered = 0;
static void *CreateWaitThread(void* param);
unsigned char read_buffer[READ_BUF_SIZE];

pthread_t read_thread_hdl;	/* Diag Read thread handle */
pthread_t disk_write_hdl;	/* Diag disk write thread handle */
pthread_t restart_thread_hdl;   /* Diag Restart thread handle */

unsigned long max_file_size = 100000000;
unsigned long min_file_size = 80000000; /* 80 percent of max size */
unsigned int log_to_memory ;
unsigned int diag_lib_dbg_mask ;
int cleanup_mask;	/*Control sending of empty mask to modem */
int diag_disable_console = 0; /*Variable to control console message */
unsigned int max_file_num = 0x7FFFFFFF;

int rename_file_names = 0;	/* Rename file name on close to current time */
int rename_dir_name = 0;	/* Rename directory name to current time when ODL is halted */

/* Static array for workaround */
static unsigned char static_buffer[6][DISK_BUF_SIZE];

/* Externalized functions */
static pthread_once_t mask_sync_is_inited = PTHREAD_ONCE_INIT;
static pthread_mutex_t mask_sync_mutex;
static int diag_sync_mask = 0;
static pthread_mutex_t lsm_init_count_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t hdlc_toggle_mutex = PTHREAD_MUTEX_INITIALIZER;

#define MASK_SYNC_COMPLETE (MSG_MASKS_TYPE | LOG_MASKS_TYPE | EVENT_MASKS_TYPE)
#define MAX_MASK_SYNC_COUNT 500

extern qsr4_db_file_parser_state parser_state;
extern int db_thread_initialized;

char pid_file[DIAG_MDLOG_PID_FILE_SZ];
extern int qdss_state;
extern int diag_id_state;
extern int adpl_parser_state;
extern int diagid_query_status[NUM_PROC];
int diagid_guid_status[NUM_PROC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int buffering_mode_periph_map_supported = 0;
int diag_con_all = 0;
struct diag_con_all_param_t all_con_params;
struct diag_query_pid_t query_md_pid_params;
static uint16_t local_remote_mask = 0;
static int switch_logging_ret = 0;
int mask_file_list_entered = 0;
static int read_thread_exit = STATUS_RDTHRD_CLR;
static int restart_thread_exit = STATUS_RST_THRD_CLR;
struct query_reg_cb_params
{
	int proc;
	int ret_val;
} reg_cb;

/* diag-id shared by diag-router to use diag-id based cmd request/ logging format */
uint8_t diag_id;
uint8_t diag_id_async_cmd_mask;

void diag_cmd_rsp_wrap(const uint8_t *rsp, unsigned int length, void *param) {}

int cb_func_ptr_wrap(unsigned char *ptr, int len, void *context_data) {
	return 0;
}

void wrapper_init(void) {
	diag_cmd_rsp rsp_func;
	struct diag_callback_tbl_t cb_client;
	rsp_func = &diag_cmd_rsp_wrap;
	cb_client.cb_func_ptr = &cb_func_ptr_wrap;
}

void mask_sync_initialize(void)
{
	pthread_mutex_init(&mask_sync_mutex, NULL);
}

static void clear_mask_sync(void)
{
	pthread_once(&mask_sync_is_inited, mask_sync_initialize);
	pthread_mutex_lock(&mask_sync_mutex);
	diag_sync_mask = 0;
	pthread_mutex_unlock(&mask_sync_mutex);
}

int get_sync_mask(void)
{
	int sync_mask;

	pthread_once(&mask_sync_is_inited, mask_sync_initialize);
	pthread_mutex_lock(&mask_sync_mutex);
	sync_mask = diag_sync_mask;
	pthread_mutex_unlock(&mask_sync_mutex);

	return sync_mask;
}

void update_sync_mask(int mask)
{
	pthread_once(&mask_sync_is_inited, mask_sync_initialize);
	pthread_mutex_lock(&mask_sync_mutex);
	diag_sync_mask |= mask;
	pthread_mutex_unlock(&mask_sync_mutex);
}

int do_mask_sync(void)
{
	int sync_mask = get_sync_mask();
	int count = 0;
	int success = 1;

	while (sync_mask != MASK_SYNC_COMPLETE)
	{
		usleep(1000);
		sync_mask = get_sync_mask();
		count++;
		if (count > MAX_MASK_SYNC_COUNT) {
			DIAG_LOGE("diag: In %s, mask sync error, count: %d\n",
				__func__, count);
			success = 0;
			break;
		}
	}

	return success;
}

void diag_wakelock_init(char *wakelock_name)
{
	/* The max permissible length of the wakelock string. The value
	   25 was chosen randomly */
	int wl_name_len = 25;

	if (!wakelock_name) {
		DIAG_LOGE("diag: In %s, invalid wakelock name\n", __func__);
		goto fail_init;
	}

	/* Wake lock is already initialized */
	if (wl_inited && wl_name) {
		/* Check if the wakelock name is the same as the wakelock held.
		   If not, just print a warning and continue to use the old
		   wakelock name */
		if (strncmp(wakelock_name, wl_name, wl_name_len))
			DIAG_LOGE("diag: %s, already holding another wakelock in this process\n", __func__);
		return;
	}

	wl_name = (char *)malloc(wl_name_len);
	if (!wl_name) {
		DIAG_LOGE("diag: In %s, cannot allocate memory for wl_name\n", __func__);
		goto fail_init;
	}
	strlcpy(wl_name, wakelock_name, wl_name_len);

	fd_wl = open(WAKELOCK_FILE, O_WRONLY|O_APPEND);
	if (fd_wl < 0) {
		DIAG_LOGE("diag: could not open wakelock file, errno: %d\n", errno);
		goto fail_open;
	} else {
		fd_wul = open(WAKEUNLOCK_FILE, O_WRONLY|O_APPEND);
		if (fd_wul < 0) {
			DIAG_LOGE("diag: could not open wake-unlock file, errno: %d\n", errno);
			close(fd_wl);
			goto fail_open;
		}
	}

	wl_inited = TRUE;
	return;

fail_open:
	free(wl_name);
fail_init:
	wl_inited = FALSE;
}

void diag_wakelock_destroy()
{
	if (!wl_inited) {
		DIAG_LOGE("diag: %s, wakelock files are not initialized\n", __func__);
		return;
	}

	close(fd_wl);
	close(fd_wul);
	free(wl_name);
}

int diag_is_wakelock_init()
{
	return wl_inited;
}

void diag_wakelock_acquire()
{
	int status = -1;
	if (!wl_inited) {
		DIAG_LOGE("diag: %s, wakelock files are not initialized\n", __func__);
		return;
	}

	if (!wl_name) {
		DIAG_LOGE("diag: In %s, invalid wakelock name\n", __func__);
		return;
	}

	status = write(fd_wl, wl_name, strlen(wl_name));
	if(status != (int)strlen(wl_name))
		DIAG_LOGE("diag: error writing to wakelock file");
}

void diag_wakelock_release()
{
	int status = -1;
	if (!wl_inited) {
		DIAG_LOGE("diag: %s, wakelock files are not initialized\n", __func__);
		return;
	}

	if (!wl_name) {
		DIAG_LOGE("diag: In %s, invalid wakelock name\n", __func__);
		return;
	}

	status = write(fd_wul, wl_name, strlen(wl_name));
	if(status != (int)strlen(wl_name))
		DIAG_LOGE("diag: error writing to wake unlock file");
}


/*========= variables for optimisation =======*/

static int curr_write;
static int curr_read;
static int  write_in_progress;
pthread_mutex_t stop_mutex;
pthread_cond_t stop_cond;

pthread_cond_t deinit_cond;
pthread_mutex_t deinit_mutex;
static int deinit_rcvd = 0;
static int sig_rcvd = 0;

pthread_cond_t restart_cond;
pthread_mutex_t restart_mutex;
pthread_mutex_t read_thread_mutex;

int try_restart = 0;
int kill_restart = 0;

pthread_cond_t query_con_cond;
pthread_mutex_t query_con_mutex;
int query_con_rcvd = 0;

pthread_cond_t query_md_pid_cond;
pthread_mutex_t query_md_pid_mutex;
int query_md_pid_rcvd = 0;

pthread_cond_t query_switch_logging_cond;
pthread_mutex_t query_switch_logging_mutex;
int query_switch_logging_rcvd = 0;

pthread_cond_t query_reg_cb_cond;
pthread_mutex_t query_reg_cb_mutex;
int query_reg_cb_rcvd = 0;

pthread_cond_t query_remote_mask_cond;
pthread_mutex_t query_remote_mask_mutex;
int query_remote_mask_rcvd = 0;

pthread_cond_t query_dci_client_id_cond;
pthread_mutex_t query_dci_client_id_mutex;
int query_dci_client_id_rcvd = 0;

int dci_client_id = 0;
pthread_cond_t query_dci_support_cond;
pthread_mutex_t query_dci_support_mutex;
int query_dci_support_rcvd = 0;
uint16_t dci_support_list = 0;

pthread_cond_t query_dci_health_stats_cond;
pthread_mutex_t query_dci_health_stats_mutex;
int query_dci_health_stats_rcvd = 0;

struct diag_dci_health_stats_proc health_stats;
pthread_cond_t query_dci_log_status_cond;
pthread_mutex_t query_dci_log_status_mutex;
int query_dci_log_status_rcvd = 0;
int dci_log_status = 0;

pthread_cond_t query_dci_event_status_cond;
pthread_mutex_t query_dci_event_status_mutex;
int query_dci_event_status_rcvd = 0;
int dci_event_status = 0;

pthread_cond_t query_real_time_cond;
pthread_mutex_t query_real_time_mutex;
int query_real_time_rcvd = 0;
int real_time = 0;
pthread_cond_t query_reg_table_mask_cond;
pthread_mutex_t query_reg_table_mask_mutex;
int query_reg_table_mask_rcvd = 0;

pthread_cond_t query_debug_all_mask_cond;
pthread_mutex_t query_debug_all_mask_mutex;
int query_debug_all_mask_rcvd = 0;

pthread_mutex_t timestamp_switch_mutex;
int time_stamp_switch_rcvd = 0;

static char cmd_disable_log_mask[] = { 0x73, 0, 0, 0, 0, 0, 0, 0};
static char cmd_disable_msg_mask[] = { 0x7D, 0x05, 0, 0, 0, 0, 0, 0};
static char cmd_disable_event_mask[] = { 0x60, 0};
static char cmd_disable_qtrace_mask[] = { 0x4B, 0x44, 0x01, 0x90, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static char cmd_all_mask_clear[] = { 0x4B, 0x12, 0x30, 0x02, 0x01};

typedef PACK(struct) {
	uint32 header_length;
	uint8 version;
	uint8 hdlc_data_type;
	uint32 guid_list_entry_count;
	uint8 guid[MAX_GUID_ENTRIES][GUID_LEN];
}qshrink4_header;

typedef PACK(struct) {
	uint8 diag_id;
	char process_name[30];
	uint8 guid[GUID_LEN];
}diagid_guid_struct;

static qshrink4_header qshrink4_data[NUM_PROC];

static uint32 diagid_entry_count[NUM_PROC];
static uint32 guid_count[NUM_PROC][NUM_PERIPHERALS];
static diagid_guid_struct diagid_guid[NUM_PROC][MAX_GUID_ENTRIES];

unsigned char *pool0_buffers[NUM_PROC];
unsigned char *pool1_buffers[NUM_PROC];
enum status buffer_init[NUM_PROC];
int token_list[NUM_PROC] = {0, -1, -2, -3, -4, -5, -6, -7, -8, -9};

struct buffer_pool pools[] = {
	[0] = {
		.free		=	1,
		.data_ready	=	0,
	},
	[1] = {
		.free		=	1,
		.data_ready	=	0,
	},

};

int diag_get_max_channels(void)
{
	return MAX_SOCKET_CHANNELS;
}

int to_integer(char *str)
{
	int hex_int = 0;
	int i = 0, is_hex = 0;
	char *p;
	if (str) {
		while(str[i] != '\0') {
			if(str[i] == 'x' || str[i] == 'X') {
				sscanf(str, "%i", &hex_int);
				is_hex = 1;
				break;
			}
			i++;
		}
		if(is_hex == 0)
			hex_int = strtol(str, &p, 10);
	}
	return hex_int;
}

/*==========================================================================
FUNCTION   diag_qshrink4_init

DESCRIPTION
 In case for HDLC encoding is disabled the qmdl2 file will
 contain the header information for the qshrink4 messages.

 The current header structure is as follows:


Field				Length (in bytes)			Description
HeaderLength				4			Number of bytes reserved for header.

Version				1			Version value to set

HDLC DataType				1			0 - indicates hdlc encoding removed
                                                                1-indicates hdlc encoding enabled

GuidListEntryCount			4			Number of Guids available to read

GUID[GuidListEntryCount]	16*( GuidListEntryCount)	An array of available GUIDs.This can expand
							upto the maximum number of GUIDlist entries reserved in diag.

Any data will be followed by header

SIDE EFFECTS
  None

===========================================================================*/

void diag_qshrink4_init(void)
{
	int i, j;

	for (i = 0; i < NUM_PROC; i++) {
		qshrink4_data[i].header_length = DIAG_QMDL2_HDR_MIN_LEN;
		qshrink4_data[i].guid_list_entry_count = 0;

		if (use_qmdl2_v2) {
			qshrink4_data[i].version = 2;

			if (use_qmdl2_v2_hdlc_disable)
				qshrink4_data[i].hdlc_data_type = 0;
			else
				qshrink4_data[i].hdlc_data_type = 1;

			diagid_entry_count[i] = 0;
			for (j = 0; j < NUM_PERIPHERALS; j++)
				guid_count[i][j] = 0;

		} else {
			qshrink4_data[i].version = 1;
			qshrink4_data[i].hdlc_data_type = 0;
		}
	}
}

static void diag_socket_init(void)
{
	if (!socket_inited) {
		int i;
		for (i = 0; i < MAX_SOCKET_CHANNELS; i++) {
			fd_socket[i] = -1;
			if (i > MSM) {
				socket_token[i] = -i;
			}
			else
			socket_token[i] = 0;
		}
		socket_cb_ptr = NULL;
		socket_cb_data_ptr = NULL;
		socket_inited = 1;
	} else {
		DIAG_LOGE("diag: In %s, Socket info already initialized\n", __func__);
	}
}

/*==========================================================================
FUNCTION   diag_has_remote_device

DESCRIPTION
  This function queries the kernel to determine if the target device has a
  remote device or not.
  remote_mask - 0, if the target does not have a remote device; otherwise
		a bit mask representing remote channels supported by the target

RETURN VALUE
  Returns 1 on success.  Anything else is a failure.

SIDE EFFECTS
  None

===========================================================================*/
int diag_has_remote_device(uint16 *remote_mask)
{
	return(diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_REMOTE_DEV, remote_mask, 0));
}

/*==========================================================================
FUNCTION   diag_register_socket_cb

DESCRIPTION
  This function is used to register a callback function in the case that
  remote data is discovered that does not have a corresponding socket.
  The purpose of the callback function is so that app can create a socket
  to be used to read/write the remote data.

  callback_ptr - the function pointer to be called
  cb_data_ptr -	a pointer to data that will be passed when the callback
		function is called

  The callback function must have the following signature:
  int (*callback_ptr)(void *data_ptr, int socket_id)

  Where:
  data_ptr -	is the cb_data_ptr
  socket_id -	the id to be used when calling diag_send_socket_data()
		to send data for that socket

RETURN VALUE
  Returns 1 on success.  0 on failure.

SIDE EFFECTS
  None

===========================================================================*/
int diag_register_socket_cb(int (*callback_ptr)(void *data_ptr, int socket_id), void *cb_data_ptr)
{
	int success = 0;
	if (callback_ptr) {
		DIAG_LOGE("diag: In %s, registered socket callback function\n", __func__);
		socket_cb_ptr = callback_ptr;
		socket_cb_data_ptr = cb_data_ptr;
		success = 1;
	} else {
		DIAG_LOGE("diag: in %s, Unable to register socket callback function\n", __func__);
	}

	return success;
}

/*==========================================================================
FUNCTION   diag_set_socket_fd

DESCRIPTION
  This function is to be called when registering a socket file descriptor.
  This function should be called anytime the socket file descriptor is changed.
  If the socket is closed, this function should be called with a socket file
  descriptor that has a value of -1.

  socket_id -	the socket id is used internal to the diag library. If the socket
		is for MSM data its value must be 0, regardless of whether the
		target device has a remote device of not.  In the case of a target
		with a remote device, the socket id is assigned by the diag library
		and sent in the socket callback function, see diag_register_socket_cb().
  socket_fd -	the socket file descriptor.

RETURN VALUE
  Returns 1 on success.  0 on failure.

SIDE EFFECTS
  None

===========================================================================*/
int diag_set_socket_fd(int socket_id, int socket_fd)
{
	int success;
	if ((socket_id >= 0) && (socket_id < MAX_SOCKET_CHANNELS)) {
		fd_socket[socket_id] = socket_fd;
		/* DIAG_LOGE("diag: In %s, setting fd_socket[%d] to %d\n", __func__, socket_id, socket_fd); */
		success = 1;
	} else {
		DIAG_LOGE("diag: In %s, Setting of socket fd failed. Invalid socket id: %d\n",
			__func__, socket_id);
		success = 0;
	}

	return success;
}

/*==========================================================================
FUNCTION   diag_send_socket_data

DESCRIPTION
  This function is to be called when sending data read from the socket.

  socket_id -	the socket id is used internal to the diag library. If the
		socket is for MSM data (target with remote device or or not)
		its value must be 0.  In the case of a target with a remote
		device, the socket id is assigned by the diag library and sent
		in the socket callback function, see diag_register_socket_cb().
  buf -		the buffer containing the data
  num_bytes -	the number of bytes of data in the buffer that is to be sent

RETURN VALUE
  Returns 1 on success.  0 on failure.

SIDE EFFECTS
  None

===========================================================================*/
int diag_send_socket_data(int id, unsigned char buf[], int num_bytes)
{
	static unsigned char send_buf[MAX_SOCKET_CHANNELS][MAX_USER_PKT_SIZE];
	static unsigned char extra_header = USER_SPACE_DATA_TYPE_SIZE;
	static unsigned char tmp_header[DIAG_NON_HDLC_HEADER_SIZE];
	static diag_pkt_states pkt_state = PKT_START;
	static int hdlc_pkt_pending = FALSE;
	static uint32_t total_pkt_size = 0;
	static uint32_t bytes_required = 0;
	static int s_char = 0, e_char = 0;
	static uint32_t pkt_start_off = 0;
	static uint32_t saved_bytes = 0;
	int status = PKT_PROCESS_DONE;
	int packet_len_index = 0;
	uint16_t packet_len = 0;
	int i = 0, j = 0;

	if ((id < 0) || id >= MAX_SOCKET_CHANNELS) {
		DIAG_LOGE("diag_socket_log: %s: Error sending socket data. socket id: %d, num_bytes: %d\n",
			__func__, id, num_bytes);
		return PKT_PROCESS_DONE;
	}

	for (i = 0; i < num_bytes; i++) {
		status = PKT_PROCESS_ONGOING;
		switch (pkt_state)
		{
		case PKT_START:
			if (buf[i] == CONTROL_CHAR && !hdlc_pkt_pending) {
				s_char = buf[i];
				pkt_start_off = i;
				/* probably it is non-HDLC packet */
				if (num_bytes >= (pkt_start_off + DIAG_NON_HDLC_HEADER_SIZE)) {
					/* received full header */
					packet_len_index = pkt_start_off + 2;
					packet_len = (uint16_t)(*(uint16_t *)(buf + packet_len_index));
					total_pkt_size = DIAG_NON_HDLC_HEADER_SIZE + packet_len + 1;
					if (total_pkt_size <= (num_bytes - pkt_start_off)) {
						e_char = buf[total_pkt_size-1];
						if (s_char == e_char) {
							/* full non-HDLC packet received */
							memset(send_buf[id], 0, MAX_USER_PKT_SIZE);
							*(int *)send_buf[id] = USER_SPACE_DATA_TYPE;
							if (socket_token[id] != 0) {
								*(int *)(send_buf[id] + extra_header) = socket_token[id];
								extra_header += sizeof(int);
							}
							memcpy(send_buf[id] + extra_header, buf, total_pkt_size);
							/* send it to diag core */
							diag_send_data(send_buf[id], total_pkt_size + extra_header);
							i += total_pkt_size - 1;
							s_char = 0;
							e_char = 0;
							total_pkt_size = 0;
							extra_header = USER_SPACE_DATA_TYPE_SIZE;
							status = PKT_PROCESS_DONE;
						}
					} else {
						/* full header + partial packet received */
						pkt_state = PKT_PAYLOAD;
						memset(send_buf[id], 0, MAX_USER_PKT_SIZE);
						*(int *)send_buf[id] = USER_SPACE_DATA_TYPE;
						if (socket_token[id] != 0) {
							*(int *)(send_buf[id] + extra_header) = socket_token[id];
							extra_header += sizeof(int);
						}
						memcpy(send_buf[id] + extra_header, (buf + pkt_start_off), (num_bytes - pkt_start_off));
						saved_bytes = (num_bytes - pkt_start_off + extra_header);
						bytes_required = total_pkt_size - (saved_bytes - extra_header);
						i += num_bytes - 1;
						DIAG_LOGD("%s:PKT_START: full header + partial pkt received, total_pkt_size %d recvd %d pending %d\n",
							__func__, total_pkt_size, (saved_bytes - extra_header), bytes_required);
					}
				} else {
					/* partial header received */
					memcpy(tmp_header, (buf + pkt_start_off), (num_bytes - pkt_start_off));
					saved_bytes = (num_bytes - pkt_start_off);
					bytes_required = DIAG_NON_HDLC_HEADER_SIZE - saved_bytes;
					i += num_bytes - 1;
					pkt_state = PKT_HEADER;
					DIAG_LOGD("%s:PKT_START: partial header received, recvd %d pending %d\n",
						__func__, saved_bytes, bytes_required);
				}
			} else {
				/* HDLC packet will enter here */
				if (!hdlc_pkt_pending) {
					hdlc_pkt_pending = TRUE;
					memset(send_buf[id], 0, MAX_USER_PKT_SIZE);
					*(int *)send_buf[id] = USER_SPACE_DATA_TYPE;
					if (socket_token[id] != 0) {
						*(int *)(send_buf[id] + extra_header) = socket_token[id];
						extra_header += sizeof(int);
					}
					saved_bytes = extra_header;
				}
				/* iterate through the packet to find the delimiter */
				for (j = 0; j < num_bytes; j++) {
					if (buf[j] == CONTROL_CHAR) {
						if (j == (num_bytes - 1)) {
							/* delimiter found at the end of current packet
							 * probably this is end of HDLC packet
							 */
							if (saved_bytes + num_bytes >= MAX_USER_PKT_SIZE) {
								DIAG_LOGE("%s:hdlc: command too large, dropping pkt\n", __func__);
								hdlc_pkt_pending = FALSE;
								saved_bytes = 0;
								i += num_bytes - 1;
								break;
							}
							memcpy(send_buf[id] + saved_bytes, buf, num_bytes);
							/* send it to diag core */
							diag_send_data(send_buf[id], saved_bytes + num_bytes);
							hdlc_pkt_pending = FALSE;
							i += num_bytes - 1;
							saved_bytes = 0;
							extra_header = USER_SPACE_DATA_TYPE_SIZE;
							status = PKT_PROCESS_DONE;
						} else {
							/* delimiter character may come at the middle of packet
							 * just ignore as we cant handle this case
							 */
							DIAG_LOGE("%s:hdlc: Delimiter found at the middle index %d\n", __func__, j);
						}
					}
				}
				/* full HDLC packet has not received */
				if (hdlc_pkt_pending) {
					if (saved_bytes + num_bytes >= MAX_USER_PKT_SIZE) {
						DIAG_LOGE("%s:hdlc: command too large, dropping pkt\n", __func__);
						hdlc_pkt_pending = FALSE;
						saved_bytes = 0;
						i += num_bytes - 1;
						break;
					}
					memcpy(send_buf[id] + saved_bytes, buf, num_bytes);
					saved_bytes += num_bytes;
					i += num_bytes - 1;
					DIAG_LOGD("%s:hdlc: Partial Packet received, recvd %d\n", __func__,
						(saved_bytes - extra_header));
				}
			}
			break;

		case PKT_HEADER:
			if(num_bytes >= bytes_required){
				/* we have full header now */
				memcpy(tmp_header+saved_bytes, buf, bytes_required);
				packet_len = (uint16_t)(*(uint16_t *)(tmp_header + 2));
				total_pkt_size = DIAG_NON_HDLC_HEADER_SIZE + packet_len + 1;
				if (total_pkt_size <= (num_bytes + saved_bytes)) {
					/* might received full packet */
					e_char = buf[total_pkt_size - saved_bytes - 1];
					if (s_char == e_char) {
						/* full non-HDLC packet received */
						memset(send_buf[id], 0, MAX_USER_PKT_SIZE);
						*(int *)send_buf[id] = USER_SPACE_DATA_TYPE;
						if (socket_token[id] != 0) {
							*(int *)(send_buf[id] + extra_header) = socket_token[id];
							extra_header += sizeof(int);
						}
						memcpy(send_buf[id] + extra_header, tmp_header, saved_bytes);
						memcpy(send_buf[id] + extra_header + saved_bytes, buf,
								total_pkt_size - saved_bytes);
						/* send it to diag core */
						diag_send_data(send_buf[id], total_pkt_size + extra_header);
						i += total_pkt_size - saved_bytes - 1;
						s_char = 0;
						e_char = 0;
						total_pkt_size = 0;
						bytes_required = 0;
						saved_bytes = 0;
						extra_header = USER_SPACE_DATA_TYPE_SIZE;
						pkt_state = PKT_START;
						status = PKT_PROCESS_DONE;
					}
				} else {
					/* full header + partial packet received */
					pkt_state = PKT_PAYLOAD;
					memset(send_buf[id], 0, MAX_USER_PKT_SIZE);
					*(int *)send_buf[id] = USER_SPACE_DATA_TYPE;
					if (socket_token[id] != 0) {
						*(int *)(send_buf[id] + extra_header) = socket_token[id];
						extra_header += sizeof(int);
					}
					/* copy partial header received */
					memcpy(send_buf[id] + extra_header, tmp_header, saved_bytes);
					memcpy(send_buf[id] + extra_header + saved_bytes, buf, num_bytes);
					bytes_required = total_pkt_size - saved_bytes - num_bytes;
					i += num_bytes - 1;
					saved_bytes += num_bytes + extra_header;
					DIAG_LOGD("%s:PKT_HEADER: full header + partial pkt received, total_pkt_size %d recvd %d pending %d\n",
						__func__, total_pkt_size, (saved_bytes - extra_header), bytes_required);
				}
			} else {
				/* still full header not yet received */
				memcpy(tmp_header + saved_bytes, buf, num_bytes);
				saved_bytes += num_bytes;
				bytes_required = DIAG_NON_HDLC_HEADER_SIZE - saved_bytes;
				i += num_bytes - 1;
				DIAG_LOGD("%s:PKT_HEADER: still partial header received, recvd %d pending %d\n",
					__func__, saved_bytes, bytes_required);
			}
			break;

		case PKT_PAYLOAD:
			if(num_bytes >= bytes_required){
				/* received pending bytes */
				e_char = buf[bytes_required - 1];
				if (s_char == e_char) {
					/* full non-HDLC packet received */
					memcpy(send_buf[id] + saved_bytes, buf, bytes_required);
					/* send it to diag core */
					diag_send_data(send_buf[id], total_pkt_size + extra_header);
					i += bytes_required - 1;
					s_char = 0;
					e_char = 0;
					total_pkt_size = 0;
					bytes_required = 0;
					saved_bytes = 0;
					extra_header = USER_SPACE_DATA_TYPE_SIZE;
					pkt_state = PKT_START;
					status = PKT_PROCESS_DONE;
				}
			} else {
				/* still not yet received the full packet */
				memcpy(send_buf[id] + saved_bytes, buf, num_bytes);
				bytes_required = total_pkt_size - (saved_bytes -
								extra_header) - num_bytes;
				i += num_bytes - 1;
				saved_bytes += num_bytes;
				DIAG_LOGD("%s:PKT_PAYLOAD: Still waiting for full packet,saved %d pending %d\n",
							__func__, (saved_bytes - extra_header), bytes_required);
			}
			break;
		default:
			DIAG_LOGD("%s:default: Unexpected packet state\n",
					__func__);
			break;
		}
	}

	return status;
}

/*==========================================================================
FUNCTION   get_remote_socket_fd

DESCRIPTION
  This function is used internal to the diag library and returns the socket
  file descriptor for the remote data. If there is no valid socket file
  descriptor for the remote data, the registered socket callback function
  (see diag_register_socket_cb) will be called to notify the app that a new
  socket must be created for the remote data.

  token -	the embedded token data that identifies the data as being
		remote data.  For instance, MDM_TOKEN.

RETURN VALUE
  Returns the socket file descriptor on success; -1 otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static int get_remote_socket_fd(int token)
{
	int i;
	int found = 0;
	int fd = -1;
	int status;

	for (i = 1; i < MAX_SOCKET_CHANNELS; i++) {
		if (socket_token[i] == token) {
			fd = fd_socket[i];
			found = i;
			break;
		}
	}

	/* If there is no socket for this token */
	if (!found) {
		for (i = 1; i < MAX_SOCKET_CHANNELS; i++) {
			/* If we have found an unused socket entry */
			if (fd_socket[i] == -1) {
				/* If the socket entry has not begun initialization */
				if (socket_token[i] != token) {
					/* Notify the app that another socket needs to be opened */
					if (socket_cb_ptr) {
						status = (*(socket_cb_ptr))(socket_cb_data_ptr, i);
						socket_token[i] = token;
					} else {
						DIAG_LOGE("diag: In %s, Error, socket callback function has not been registered\n",
							__func__);
					}
				}
				break;
			}
		}
	}

	return fd;
}

/*=========================================================================
 * FUNCTION send_empty_mask
 * DESCRIPTION
 *    This function send empty mask to modem, thus modem stops generating
 *    logs and can enter low power mode.
 * RETURN VALUE
 *    NONE
 * SIDE EFFECTS
 *    NONE
 *========================================================================*/

void send_empty_mask(int type)
{
	int offset = 0;
	int length = 0;
	const uint8 size = 40;
	unsigned char mask_buf[size];
	unsigned char *ptr = mask_buf;

	*(int *)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (type) {
		*(int*)(ptr + offset) = -type;
		offset += sizeof(int);
	}

	/* Send Disable Log Mask Command */
	length = sizeof(cmd_disable_log_mask) / sizeof(char);
	if (length + offset <= size) {
		memcpy(ptr + offset, cmd_disable_log_mask, length);
		diag_send_data(ptr, offset + length);
	}

	/* Send Disable Msg Mask Command */
	length = sizeof(cmd_disable_msg_mask) / sizeof(char);
	if (length + offset <= size) {
		memcpy(ptr + offset, cmd_disable_msg_mask, length);
		diag_send_data(ptr, offset + length);
	}

	/* Send Disable Event Mask Command */
	length = sizeof(cmd_disable_event_mask) / sizeof(char);
	if (length + offset <= size) {
		memcpy(ptr + offset, cmd_disable_event_mask, length);
		diag_send_data(ptr, offset + length);
	}
	/* Send Disable Qtrace Mask Command */
	length = sizeof(cmd_disable_qtrace_mask) / sizeof(char);
	if (length + offset <= size) {
		memcpy(ptr + offset, cmd_disable_qtrace_mask, length);
		diag_send_data(ptr, offset + length);
	}
}

void send_all_mask_clear(int type)
{
	int offset = 0;
	int length = 0;
	const uint8 size = 40;
	unsigned char mask_buf[size];
	unsigned char *ptr = mask_buf;

	*(int *)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (type) {
		*(int*)(ptr + offset) = -type;
		offset += sizeof(int);
	}

	length = sizeof(cmd_all_mask_clear) / sizeof(char);
	if (length + offset <= size) {
		memcpy(ptr + offset, cmd_all_mask_clear, length);
		diag_send_data(ptr, offset + length);
	}
}

/*  signal handler to flush logs to disk */
volatile int flush_log;
volatile int in_read;
volatile int flush_in_progress;

/* Global variable to decide the type of PROC */
int proc_type = MSM;

/*==========================================================================
 * FUNCTION   dummy_handler
 *
 * DESCRIPTION
 *   A dummy signal handler for read thraed only.
 * RETURN VALUE
 *  None
 *
 *  SIDE EFFECTS
 *    None
 *
 *========================================================================*/
void dummy_handler(int signal)
{
	(void)signal;
	sig_rcvd = 1;
#if 0
	int sys_getpid, sys_gettid;

#ifdef FEATURE_LE_DIAG
	sys_getpid = SYS_getpid;
	sys_gettid = SYS_gettid;
#else
	sys_getpid = __NR_getpid;
	sys_gettid = __NR_gettid;
#endif
	 DIAG_LOGE("diag:%s: pid-tid %ld-%ld \n", __func__,
			 (long int)syscall(sys_getpid),
			 (long int)syscall(sys_gettid));
#endif

}

static void diag_kill_feature_threads(void)
{
	diag_kill_qshrink4_threads();

	diag_kill_qdss_threads();

	diag_kill_adpl_threads();

	diag_kill_diagid_threads();

	diag_kill_key_info_threads();
}

/*==========================================================================
FUNCTION   flush_buffer

DESCRIPTION
  This function flushes the pending data in the buffers.
  If application receives signal then it also stops the application.
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void flush_buffer(int signal)
{
	int status = 0;
	struct stat pid_stat;
	int success = 0;
	uint16 remote_mask = 0;
	int buf_mode = MODE_REALTIME;
	char temp_xml_buf[GUID_LIST_END_XML_TAG_SIZE];
	int index = 0, dev_idx, i = 0;
	int ret = 0;
	struct timespec time;
	DIAG_LOGE("diag: Signal received\n");

	/* Dismiss the signal if we are already processing a signal */
	if (signal) {
		if (flush_in_progress)
			return;
		flush_in_progress = 1;
	}

	status = diag_get_real_time_status(&buf_mode);
	if (status) {
		DIAG_LOGE("diag: In %s, error in querying for real time mode status: %d, errno: %d\n",
			  __func__, status, errno);
		buf_mode = MODE_REALTIME;
	}
	if (buf_mode == MODE_UNKNOWN)
		DIAG_LOGE("diag: One of the peripherals is in buffering mode.\n");

	/*
	 * If we are going to exit and we are logging in non-real-time mode
	 * then let the kernel know it should move from nont-real-time mode
	 * to real-time mode. Then wait for a few seconds to allow diag on the
	 * peripherals to change to real-time logging mode and flush their
	 * buffers over the smd. This data will then be read from the kernel
	 * and placed into the this libraries buffers for later writing. Wait
	 * for a few seconds before progressing with the exit to allow this
	 * to take place.
	 */
	if (signal && buf_mode == MODE_NONREALTIME) {
		int num_secs = 5;
		errno = 0;
		status = diag_vote_md_real_time(MODE_REALTIME);
		if (status == -1)
			DIAG_LOGE("diag: In %s unable to set mode to real time mode. errno = %d\n", __func__, errno);

		DIAG_LOGE("diag: In %s, Waiting for %d seconds for non-real-time data to arrive\n",
					__func__, num_secs);
		sleep(num_secs);
	}

	if (signal && cleanup_mask) {
		DIAG_LOGE("diag: Sending empty mask MSM\n");
		send_empty_mask(MSM);

		success = diag_has_remote_device(&remote_mask);
		if (success == 1) {
			for (dev_idx = 1; dev_idx <= NUM_PROC; dev_idx++) {
				if ((remote_mask & (1 << (dev_idx - 1))) &&
					(diag_device_mask & (1 << dev_idx))) {
					DIAG_LOGE("diag: Sending empty mask to device %d\n", dev_idx);
					send_empty_mask(dev_idx);
				}
			}
		} else {
			DIAG_LOGE("diag: error in getting remote processor mask, err: %d, errno: %d\n", success, errno);
		}
	} else {
		DIAG_LOGE("diag: Sending mask clear for MSM\n");
		send_all_mask_clear(MSM);
	}

	diag_kill_feature_threads();
	flush_log = 1;

	if (in_read) {
		DIAG_LOGE("diag: sending signal to unblock read thread\n");
		if (read_thread_hdl)
			pthread_kill(read_thread_hdl, SIGUSR2);
	}

	while(flush_log < 3) {
		sleep(1);
	}
	pthread_mutex_lock(&stop_mutex);

	/* Clean up */
	write_in_progress = 0;
	in_read = 0;
	curr_write = curr_read = 0;
	for (i = 0; i < NUM_PROC; i++) {
		qmdl2_v2_header_inited[i] = 0;
		qmdl2_v3_header_key_inited[i] = 0;
	}

	pools[0].free =  pools[1].free = 1;
	pools[0].data_ready = pools[1].data_ready = 0;
	flush_log = 0;
	/* Signal received destroy the mutexes and stop the application */
	if (signal) {
		DIAG_LOGE("diag: Sending cond to CreateWaitThread\n");
		pthread_cond_signal(&stop_cond);
		pthread_mutex_unlock(&stop_mutex);

		success = Diag_LSM_DeInit();
		if (success) {
			if (diag_fd == DIAG_INVALID_HANDLE) {
				pthread_mutex_lock(&deinit_mutex);
				time = calculate_waiting_time(DIAG_DEINIT_TIMEOUT_IN_MSEC);
				while (!(read_thread_exit & STATUS_RDTHRD_EXIT)) {
					ret = pthread_cond_timedwait(&deinit_cond, &deinit_mutex, &time);
					if (ret == ETIMEDOUT){
						DIAG_LOGE("diag: %s: Timeout on read thread exit: %d\n",
								__func__, ret);
						break;
					}
				}
				pthread_mutex_unlock(&deinit_mutex);
			}

			pthread_mutex_destroy(&(pools[0].write_mutex));
			pthread_cond_destroy(&(pools[0].write_cond));
			pthread_mutex_destroy(&(pools[0].read_mutex));
			pthread_cond_destroy(&(pools[0].read_cond));
			pthread_mutex_destroy(&(pools[1].write_mutex));
			pthread_cond_destroy(&(pools[1].write_cond));
			pthread_mutex_destroy(&(pools[1].read_mutex));
			pthread_cond_destroy(&(pools[1].read_cond));
			pthread_mutex_destroy(&restart_mutex);
			pthread_mutex_destroy(&read_thread_mutex);
			pthread_cond_destroy(&restart_cond);
			pthread_mutex_destroy(&deinit_mutex);
			pthread_cond_destroy(&deinit_cond);
		} else {
			DIAG_LOGE("diag: DIAG_LSM_DeInit() Failed\n");
		}

		if (!hdlc_disabled[MSM]) {
			ret = std_strlprintf(temp_xml_buf, GUID_LIST_END_XML_TAG_SIZE, "%s", "</guidlist>");
			for (index = 0; index < NUM_PROC; index++) {
				if ((fd_qsr4_xml[index] >= 0) && (ret > 0)) {
					status = write(fd_qsr4_xml[index], temp_xml_buf, ret);
					if (status != ret)
						DIAG_LOGE("diag:In %s failed to write to xml file with err %d\n", __func__, errno);
					status = close(fd_qsr4_xml[index]);
					if (status != 0)
						DIAG_LOGE("diag:In %s failed to close xml file with err %d\n", __func__, errno);
					fd_qsr4_xml[index] = -1;
				}
			}
		}

		/* Release the wakelock if held */
		if (diag_is_wakelock_init()) {
			diag_wakelock_release();
			diag_wakelock_destroy();
		}

		/* Delete the PID file */
		if (stat(pid_file, &pid_stat) == 0)
			unlink(pid_file);
		else
			DIAG_LOGE("diag: PID file is not present\n");

		DIAG_LOGE("diag: Exiting program \n");
		_exit(0);
	}
	else
		pthread_mutex_unlock(&stop_mutex);

}


int valid_token(int token)
{
	int i;

	if (token >= 0)
		return 0;
	for (i = 0; i < NUM_PROC; i++)
		if(token == token_list[i])
			return -token;
	return 0;
}

static void parse_data_for_command_rsp(uint8* ptr, int count_received_bytes, int index, int *update_count)
{

	if (diag_id_state)
		parse_data_for_diag_id_rsp(ptr, count_received_bytes, index, update_count);

	if (key_info_enabled())
		parse_data_for_key_info_resp(ptr, count_received_bytes, index);

	if (qdss_state)
		parse_data_for_qdss_rsp(ptr, count_received_bytes, index);

	if (parser_state)
		parse_data_for_qsr4_db_file_op_rsp(ptr, count_received_bytes, index);

	if (adpl_parser_state)
		parse_data_for_adpl_rsp(ptr, count_received_bytes, index);

}

/*===========================================================================
FUNCTION  fill_pkt_buffer

DESCRIPTION
  This function creates a single byte stream of logs by removing header
  information.
DEPENDENCIES
  valid data type to be passed in

RETURN VALUE
  TRUE - if pkt is filled correctly
  FALSE - pkt is not filled correctly due to invalid token

SIDE EFFECTS
  None

===========================================================================*/
static boolean fill_pkt_buffer(unsigned char *ptr)
{
	int num_data_fields = *(int *)ptr;
	int i, index = 0, z;
	uint32 count_received_bytes = 0;
	int update_count = 0;
	int status = 0;
	char temp_xml_buf[GUID_LIST_END_XML_TAG_SIZE];
	int ret = 0;
	unsigned char *buffer = NULL;
	unsigned int *bytes_in_buff = NULL;

	ptr += 4;
	for (i = 0; i < num_data_fields; i++) {
		index = valid_token(*(int *)ptr);
		if (index == -1) {
			DIAG_LOGE("diag: In %s, invalid Token number %d\n", __func__, *(int *)ptr);
			return FALSE;
		}
		buffer = pools[curr_read].buffer_ptr[index];
		bytes_in_buff = &pools[curr_read].bytes_in_buff[index];
		if (!buffer || !bytes_in_buff)
			continue;
		if (index > 0) {
			ptr += 4;
			if (buffer_init[index] != READY) {

				/* This check is added as calloc is failing */
				if (index > 2) {
					/* Allocate buffer */
					pool0_buffers[index] =
						calloc(DISK_BUF_SIZE, 1);
					if (!pool0_buffers[index]) {
						DIAG_LOGE("\nbuffer alloc failed \n");
						continue;
					}
					pool1_buffers[index] =
						calloc(DISK_BUF_SIZE, 1);
					if (!pool1_buffers[index]) {
						DIAG_LOGE("\nbuffer alloc failed \n");
						free(pool0_buffers[index]);
						continue;
					}
					pools[0].buffer_ptr[index] =
						pool0_buffers[index];
					pools[1].buffer_ptr[index] =
						pool1_buffers[index];
					pools[0].bytes_in_buff[index] = 0;
					pools[1].bytes_in_buff[index] = 0;
				}

				/* Create directory for this proc */
				(void)strlcat(output_dir[index],
					proc_name[index], FILE_NAME_LEN);
				status = mkdir(output_dir[index], 0770);
				if (status == -1) {
					if (errno == EEXIST) {
						DIAG_LOGE("diag: In %s, Warning output directory already exists: %s\n",
							__func__, output_dir[index]);
						DIAG_LOGE("diag: Proceeding...\n");
					} else {
						DIAG_LOGE("diag: In %s, Unable to create directory: %s, errno: %d\n",
							__func__, output_dir[index], errno);
						continue;
					}
				}
				if (!hdlc_disabled[MSM]) {
					if (fd_qsr4_xml[index] < 0) {
						(void)std_strlprintf(qsr4_xml_file_name,
								     FILE_NAME_LEN, "%s%s%s%s",
								     output_dir[index],"/diag_qsr4_guid_list_",
								     (char *)buffer, ".xml");
						fd_qsr4_xml[index] =  open(qsr4_xml_file_name, O_CREAT | O_RDWR | O_SYNC | O_TRUNC, 0644);
						if (fd_qsr4_xml[index] < 0)
							DIAG_LOGE("diag: In %s failed to create xml file with err %d", __func__, errno);
						ret = std_strlprintf(temp_xml_buf, GUID_LIST_XML_TAG_SIZE, "%s\n", "<guidlist>");
						if ((fd_qsr4_xml[index] >= 0) && (ret > 0)) {
							status = write(fd_qsr4_xml[index], temp_xml_buf, ret);
							if (status != ret)
								DIAG_LOGE("diag: In %s failed to write to xml file with err %d", __func__, errno);
						}
					}

				}
				buffer_init[index] = READY;
				buffer = pools[curr_read].buffer_ptr[index];
				*bytes_in_buff =
					pools[curr_read].bytes_in_buff[index];
			}
		}

		count_received_bytes = *(uint32*)ptr;
		ptr += sizeof(uint32);

		if (count_received_bytes > 0) {
			parse_data_for_command_rsp(ptr, count_received_bytes, index, &update_count);
			if (update_count)
				count_received_bytes += update_count;
		}

		if (disable_hdlc && !hdlc_disabled[index])
			return FALSE;

		if (count_received_bytes >= (DISK_BUF_SIZE - *bytes_in_buff)) {
			pools[curr_read].data_ready = 1;
			pools[curr_read].free = 0;
			for (z = 0; z < NUM_PROC; z++) {
				if (curr_read)
					pools[curr_read].buffer_ptr[z] =
							pool1_buffers[z];
				else
					pools[curr_read].buffer_ptr[z] =
							pool0_buffers[z];
			}
			pthread_cond_signal(&pools[curr_read].write_cond);
			pthread_mutex_unlock(&(pools[curr_read].write_mutex));
			curr_read = !curr_read;
			pthread_mutex_lock(&(pools[curr_read].read_mutex));
			if (!pools[curr_read].free) {
				pthread_mutex_unlock(&(pools[curr_read].write_mutex));
				pthread_cond_wait(
					&(pools[curr_read].read_cond),
					&(pools[curr_read].read_mutex));
				pthread_mutex_lock(&(pools[curr_read].write_mutex));
			}
			pthread_mutex_unlock(&(pools[curr_read].read_mutex));
			buffer = pools[curr_read].buffer_ptr[index];
			bytes_in_buff =
					&pools[curr_read].bytes_in_buff[index];
		}
		if (count_received_bytes > 0) {
			memcpy(buffer + *bytes_in_buff, ptr, count_received_bytes);
			*bytes_in_buff += count_received_bytes;
			ptr += count_received_bytes;
		}
	}
	return TRUE;
}

/*===========================================================================
FUNCTION   process_ext_header_packet

DESCRIPTION
	This function handles parsing an extended header packet.

DEPENDENCIES
	valid data type to be passed in

RETURN VALUE
	Bytes read from read_buffer in this function

SIDE EFFECTS
	None

===========================================================================*/
static int process_ext_header_packet(int dci_proc, unsigned char* read_buffer) {

	//check version of dci application
	int dci_version = dci_client_tbl[dci_proc].version;
	int read_bytes = 0, len = 0;
	uint32 pkt_cmd_type;
	void (*callback_ptr)(unsigned char *, int) = NULL;
	unsigned char* buf_ptr = NULL;

	if (!read_buffer)
		return 0;

	/* Move read_buffer up to account for DCI HEADER for ext header*/
	read_bytes += DCI_HEADER_LENGTH;
	read_buffer += DCI_HEADER_LENGTH;

	/* Default buf_ptr to start of extended header packet */
	buf_ptr = read_buffer;
	read_buffer += DCI_EXT_HDR_LENGTH;

	/* Read type of enclose packet */
	pkt_cmd_type = *(uint32 *)read_buffer;
	read_buffer += DCI_HEADER_LENGTH;

	/* Read length of enclosed packet */
	len  =  *(uint16 *)read_buffer;

	if (dci_version > 0) {
		/* Client supports ext header packets
		 */
		len += DCI_EXT_HDR_LENGTH + DCI_HEADER_LENGTH;
	} else {
		/* Client does not support ext header packets
		 * strip ext header
		 */
		read_bytes += DCI_EXT_HDR_LENGTH + DCI_HEADER_LENGTH;
		buf_ptr = read_buffer;
	}

	switch (pkt_cmd_type) {
	case DCI_LOG_TYPE:
		callback_ptr = dci_client_tbl[dci_proc].func_ptr_logs;
		break;
	case DCI_EVENT_TYPE:
		len += DCI_EVENT_OFFSET;
		callback_ptr = dci_client_tbl[dci_proc].func_ptr_events;
		break;
	default:
		DIAG_LOGE("diag: dci: received invalid packet type %u in extended packet\n",
				pkt_cmd_type);
		break;
	}
	read_bytes += len;

	if (!callback_ptr) {
		DIAG_LOGE("diag: dci: no callback function registered for packet type %u\n",
				pkt_cmd_type);
		return 0;
	}

	(*callback_ptr)(buf_ptr, len);
	return read_bytes;
}

static void inline diag_print_client_nums(boolean upd)
{
	printf("Client IDs: Modem: %d, LPASS: %d, WCNSS: %d, SLPI: %d, WDSP: %d, "
		"CDSP: %d, NPU: %d, NSP1: %d, GPDSP0:%d, GPDSP1: %d, HELIOS: %d, "
		"SLATE_APPS: %d, SLATE_ADSP: %d, TELE_GVM: %d, FOTA_GVM: %d\n",
			DIAG_MODEM_PROC + 1, DIAG_LPASS_PROC + 1, DIAG_WCNSS_PROC + 1,
			DIAG_SENSORS_PROC + 1, DIAG_WDSP_PROC + 1, DIAG_CDSP_PROC + 1,
			DIAG_NPU_PROC + 1, DIAG_NSP1_PROC + 1, DIAG_GPDSP0_PROC + 1,
			DIAG_GPDSP1_PROC + 1, DIAG_HELIOS_M55_PROC + 1,
			DIAG_SLATE_APPS_PROC + 1, DIAG_SLATE_ADSP_PROC + 1,
			DIAG_TELE_GVM_PROC + 1, DIAG_FOTA_GVM_PROC + 1);

	if (upd)
		printf("Client IDs: WLAN: %d, AUDIO: %d, SENSORS: %d, CHARGER: %d, "
		       " OEM: %d, OIS: %d\n", UPD_WLAN, UPD_AUDIO, UPD_SENSORS, UPD_CHARGER,
			UPD_OEM, UPD_OIS);
}

void diag_print_debug_info(unsigned char *ptr)
{
	struct diag_debug_all_param_t debug_all_params;
	int i = 0, j = 0;

	debug_all_params = *(struct diag_debug_all_param_t*)ptr;

	diag_print_client_nums(TRUE);

	printf("\n");
	printf("time_api: %d "
		"(0: Uptime/Network time, 1: Uptime,  2: Network time)\n",
		debug_all_params.time_api);
	printf("timesync_enabled: %d\n", debug_all_params.timesync_enabled);
	printf("timestamp_switch: %d\n", debug_all_params.timestamp_switch);
	printf("\n");

	for (i = 0; i < (NUM_PERIPHERALS + 1); i++) {
		printf("stm_support[%2d]: %d\t", i, debug_all_params.stm_support[i]);
		printf("stm_state_requested[%2d]: %d\t", i, debug_all_params.stm_state_requested[i]);
		printf("stm_state[%2d]: %d\n", i, debug_all_params.stm_state[i]);
	}

	printf("\n");
	printf("apps_feature: %d "
		"(1: Event report, 2: HW acceleration, 4: MultiSIM)\n",
		debug_all_params.apps_feature);

	for (i = 0; i < DIAGID_V2_FEATURE_COUNT; i++) {
		printf("diagid_v2_feature[%d]: %d\t", i, debug_all_params.diagid_v2_feature[i]);
		printf("diagid_v2_status[%d]: %d\t", i, debug_all_params.diagid_v2_status[i]);
		printf("diag_hw_accel[%d]: %d\n", i, debug_all_params.diag_hw_accel[i]);
	}
	printf("\n");

	for (i = 0; i < (NUM_PERIPHERALS + 1); i++) {
		printf("feature_mask[%2d]: 0x%x\n", i, debug_all_params.feature[i].feature_mask);
	}

	printf("\n");

	for (i = 0; i < NUM_DIAG_MD_DEV; i++) {
		printf("logging_mode[%d]: %d\t", i, debug_all_params.logging_mode[i]);
		printf("logging_mask[%d]: 0x%x\n", i, debug_all_params.logging_mask[i]);
	}

	for (i = 0; i < NUM_UPD; i++) {
		printf("pd_logging_mode[%d]: %d\t", i, debug_all_params.pd_logging_mode[i]);
		printf("pd_session_clear[%d]: %d\n", i, debug_all_params.pd_session_clear[i]);
	}

	printf("num_pd_session: %d\n", debug_all_params.num_pd_session);

	for (i = 0; i < NUM_DIAG_MD_DEV; i++) {
		printf("md_session_mask[%d]: 0x%x\t", i, debug_all_params.md_session_mask[i]);
		printf("md_session_mode[%d]: %d\n", i, debug_all_params.md_session_mode[i]);
	}

	printf("\n");

	for (i = 0; i < DIAG_NUM_PROC; i++)
		printf("real_time_mode[%d]: %d\n", i, debug_all_params.real_time_mode[i]);

	printf("proc_active_mask: %d\n", debug_all_params.proc_active_mask);

	for (i = 0; i < DIAG_NUM_PROC; i++)
		printf("proc_rt_vote_mask[%d]: 0x%x\n", i, debug_all_params.proc_rt_vote_mask[i]);

	printf("\n");

	for (i = 0; i < NUM_MD_SESSIONS; i++) {
		printf("buffering_mode[%2d].peripheral: %2d ", i, debug_all_params.buffering_mode[i].peripheral);
		printf("buffering_mode[%2d].mode: %d ", i, debug_all_params.buffering_mode[i].mode);
		printf("buffering_mode[%2d].high_wm_val: %2d ", i, debug_all_params.buffering_mode[i].high_wm_val);
		printf("buffering_mode[%2d].low_wm_val: %2d ", i, debug_all_params.buffering_mode[i].low_wm_val);
		printf("buffering_flag[%2d]: %d\n", i, debug_all_params.buffering_flag[i]);
	}

	printf("\n");

	for (i = 0; i < DIAG_NUM_PROC; i++)
	{
		for (j = 0; j < NUM_PERIPHERALS; j++)
		{
			printf("dci buffering_mode[%2d].peripheral: %d\t", j, debug_all_params.dci_buffering_mode[i][j].peripheral);
			printf("dci buffering_mode[%2d].mode: %d\t", j, debug_all_params.dci_buffering_mode[i][j].mode);
			printf("dci buffering_mode[%2d].high_wm_val: %d\t", j, debug_all_params.dci_buffering_mode[i][j].high_wm_val);
			printf("dci buffering_mode[%2d].low_wm_val: %d\n", j, debug_all_params.dci_buffering_mode[i][j].low_wm_val);
		}
	}

	printf("\n");
	printf("usb_connected: %d\n", debug_all_params.usb_connected);
	printf("\n");
}

void get_timeout_time(struct timespec *time)
{
	struct timeval now;

	gettimeofday(&now, NULL);
	time->tv_sec = now.tv_sec + (1000 / 1000);
	time->tv_nsec = now.tv_usec + (10000 % 1000) * 1000000;
}

int wait_for_query_con_rsp(struct diag_con_all_param_t *buf)
{
	struct timespec time;
	int rt = 0;

	get_timeout_time(&time);
	pthread_mutex_lock(&query_con_mutex);
	if (!query_con_rcvd)
		rt = pthread_cond_timedwait(&query_con_cond, &query_con_mutex, &time);
	query_con_rcvd = 0;
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag: %s: Timeout while querying for con all parameters\n", __func__);
		pthread_mutex_unlock(&query_con_mutex);
		return -ETIMEDOUT;
	}
	buf->diag_con_all = all_con_params.diag_con_all;
	buf->num_peripherals = all_con_params.num_peripherals;
	buf->upd_map_supported = all_con_params.upd_map_supported;
	pthread_mutex_unlock(&query_con_mutex);
	return 0;
}
void wait_for_query_md_pid_rsp(struct diag_query_pid_t *buf)
{
	pthread_mutex_lock(&query_md_pid_mutex);
	if (!query_md_pid_rcvd)
	        pthread_cond_wait(&query_md_pid_cond, &query_md_pid_mutex);
	query_md_pid_rcvd = 0;
	pthread_mutex_unlock(&query_md_pid_mutex);
	buf->pid = query_md_pid_params.pid;
	buf->kill_count = query_md_pid_params.kill_count;
}

int wait_for_switch_logging_rsp(void)
{
	pthread_mutex_lock(&query_switch_logging_mutex);
	if (!query_switch_logging_rcvd)
		pthread_cond_wait(&query_switch_logging_cond, &query_switch_logging_mutex);
	query_switch_logging_rcvd = 0;
	pthread_mutex_unlock(&query_switch_logging_mutex);
	return switch_logging_ret;
}

int wait_for_query_register_callback()
{
	pthread_mutex_lock(&query_reg_cb_mutex);
	if (!query_reg_cb_rcvd)
		pthread_cond_wait(&query_reg_cb_cond, &query_reg_cb_mutex);
	query_reg_cb_rcvd = 0;
	pthread_mutex_unlock(&query_reg_cb_mutex);
	return reg_cb.ret_val;

}
void wait_for_query_remote_device_mask(uint16_t *buf)
{
	pthread_mutex_lock(&query_remote_mask_mutex);
        if (!query_remote_mask_rcvd)
		pthread_cond_wait(&query_remote_mask_cond, &query_remote_mask_mutex);
	query_remote_mask_rcvd = 0;
	pthread_mutex_unlock(&query_remote_mask_mutex);
	*buf = local_remote_mask;
}
void wait_for_query_reg_table_mask()
{
	struct timespec time;
	int rt = 0;

	get_timeout_time(&time);
	pthread_mutex_lock(&query_reg_table_mask_mutex);
	if (!query_reg_table_mask_rcvd)
		rt = pthread_cond_timedwait(&query_reg_table_mask_cond, &query_reg_table_mask_mutex, &time);
	query_reg_table_mask_rcvd = 0;
	if (rt == ETIMEDOUT)
		DIAG_LOGE("diag: %s: Timeout while querying for reg table mask\n", __func__);
	pthread_mutex_unlock(&query_reg_table_mask_mutex);
}
void wait_for_query_debug_all_mask()
{
	struct timespec time;
	int rt = 0;

	get_timeout_time(&time);
	pthread_mutex_lock(&query_debug_all_mask_mutex);
	if (!query_debug_all_mask_rcvd)
		rt = pthread_cond_timedwait(&query_debug_all_mask_cond, &query_debug_all_mask_mutex, &time);
	query_debug_all_mask_rcvd = 0;
	if (rt == ETIMEDOUT)
		DIAG_LOGE("diag: %s: Timeout while querying for debug all mask\n", __func__);
	pthread_mutex_unlock(&query_debug_all_mask_mutex);
}
int wait_for_dci_reg_client_id(void)
{
	pthread_mutex_lock(&query_dci_client_id_mutex);
        while (!query_dci_client_id_rcvd)
                pthread_cond_wait(&query_dci_client_id_cond, &query_dci_client_id_mutex);
        query_dci_client_id_rcvd = 0;
        pthread_mutex_unlock(&query_dci_client_id_mutex);
        return dci_client_id;

}
int wait_for_dci_support(struct diag_dci_peripheral_list_t *ptr)
{
	pthread_mutex_lock(&query_dci_support_mutex);
        if (!query_dci_support_rcvd)
                pthread_cond_wait(&query_dci_support_cond, &query_dci_support_mutex);
        query_dci_support_rcvd = 0;
        pthread_mutex_unlock(&query_dci_support_mutex);
	ptr->list = dci_support_list;
        return 0;

}
int wait_for_dci_health_stats(struct diag_dci_health_stats_proc *ptr)
{
	pthread_mutex_lock(&query_dci_health_stats_mutex);
        if (!query_dci_health_stats_rcvd)
                pthread_cond_wait(&query_dci_health_stats_cond, &query_dci_health_stats_mutex);
        query_dci_health_stats_rcvd = 0;
        pthread_mutex_unlock(&query_dci_health_stats_mutex);
	memcpy(ptr, &health_stats, sizeof(health_stats));
        return 0;

}
int wait_for_dci_log_status(struct diag_log_event_stats *ptr)
{
	pthread_mutex_lock(&query_dci_log_status_mutex);
        if (!query_dci_log_status_rcvd)
                pthread_cond_wait(&query_dci_log_status_cond, &query_dci_log_status_mutex);
        query_dci_log_status_rcvd = 0;
        pthread_mutex_unlock(&query_dci_log_status_mutex);
	ptr->is_set = dci_log_status;
        return 0;

}
int wait_for_dci_event_status(struct diag_log_event_stats *ptr)
{
	pthread_mutex_lock(&query_dci_event_status_mutex);
        if (!query_dci_event_status_rcvd)
                pthread_cond_wait(&query_dci_event_status_cond, &query_dci_event_status_mutex);
        query_dci_event_status_rcvd = 0;
        pthread_mutex_unlock(&query_dci_event_status_mutex);
	ptr->is_set = dci_event_status;
        return 0;

}
int wait_for_query_real_time(struct real_time_query_t *ptr)
{
	pthread_mutex_lock(&query_real_time_mutex);
        if (!query_real_time_rcvd)
                pthread_cond_wait(&query_real_time_cond, &query_real_time_mutex);
        query_real_time_rcvd = 0;
        pthread_mutex_unlock(&query_real_time_mutex);
	ptr->real_time = real_time;
        return 0;

}

static void query_reg_table_mask(char *ptr)
{
	static int reg_tbl_cnt = 0;

	if (reg_tbl_cnt == 0) {
		printf("Master registration table information\n");
		diag_print_client_nums(FALSE);
		printf("count \t first \t\t last \t\t peripheral/pid\n");
	}
	reg_tbl_cnt++;
	if (*(int *)ptr != DIAG_QUERY_END_OF_PACKET)
		printf("%d \t 0x%x \t 0x%x \t %d\n", reg_tbl_cnt, *(int *)ptr, *(int *)(ptr+4), *(int *)(ptr+8));
	if (*(int *)ptr == DIAG_QUERY_END_OF_PACKET) {
		pthread_mutex_lock(&query_reg_table_mask_mutex);
		query_reg_table_mask_rcvd = 1;
		pthread_cond_signal(&query_reg_table_mask_cond);
		pthread_mutex_unlock(&query_reg_table_mask_mutex);
	}
}

/*===========================================================================
FUNCTION   process_diag_payload

DESCRIPTION
  This looks at the type of data being passed and then calls
  the appropriate function for processing request.

DEPENDENCIES
  valid data type to be passed in

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void process_diag_payload(void)
{
	int type = *(int *)read_buffer, read_bytes = 0;
	int dci_data_len, dci_proc = DIAG_PROC_MSM;
	int ret, payload_len = 0;
	unsigned char* ptr = read_buffer+4;
	boolean result = FALSE;
	int dci_bytes_processed = 0;

	if (num_bytes_read < (int)sizeof(type))
		return;
	payload_len = num_bytes_read - sizeof(type); /* Subtract size of the data type */

	if (type == MSG_MASKS_TYPE) {
		msg_update_mask(ptr, payload_len);
		update_sync_mask(MSG_MASKS_TYPE);
	} else if (type == LOG_MASKS_TYPE) {
		log_update_mask(ptr, payload_len);
		update_sync_mask(LOG_MASKS_TYPE);
	} else if (type == EVENT_MASKS_TYPE) {
		event_update_mask(ptr, payload_len);
		update_sync_mask(EVENT_MASKS_TYPE);
	} else if (type == DCI_LOG_MASKS_TYPE) {
		log_update_dci_mask(ptr, payload_len);
	} else if (type == DCI_EVENT_MASKS_TYPE) {
		event_update_dci_mask(ptr, payload_len);
	} else if (type == DCI_BUFFERING_MODE_STATUS_TYPE) {
		int mode;
		dci_proc = *(int *) ptr;
		ptr += sizeof(int);
		mode = *(int *) ptr;
		if (dci_client_tbl)
			if (dci_client_tbl[dci_proc].func_ptr_dci_buffering_status)
				(*(dci_client_tbl[dci_proc].func_ptr_dci_buffering_status))(mode);
	} else if (type == PKT_TYPE || type == DCI_PKT_TYPE) {
		diagpkt_LSM_process_request((void*)ptr, (uint16)payload_len, type);
	} else if (type == HDLC_SUPPORT_TYPE) {
		pthread_mutex_lock(&hdlc_toggle_mutex);
		hdlc_disabled[MSM] = *(uint8*)ptr;
		pthread_mutex_unlock(&hdlc_toggle_mutex);
	} else if (type == DIAG_QUERY_REMOTE_DEV_MASK) {
		memcpy(&local_remote_mask, ptr, sizeof(local_remote_mask));
		pthread_mutex_lock(&query_remote_mask_mutex);
		query_remote_mask_rcvd = 1;
		pthread_cond_signal(&query_remote_mask_cond);
		pthread_mutex_unlock(&query_remote_mask_mutex);
	} else if (type == DIAG_QUERY_DCI_CLIENT_ID) {
		memcpy(&dci_client_id, ptr, sizeof(dci_client_id));
		pthread_mutex_lock(&query_dci_client_id_mutex);
		query_dci_client_id_rcvd = 1;
		pthread_cond_signal(&query_dci_client_id_cond);
		pthread_mutex_unlock(&query_dci_client_id_mutex);
	} else if (type == DIAG_QUERY_DCI_SUPPORT_LIST) {
		memcpy(&dci_support_list, ptr, sizeof(dci_support_list));
		pthread_mutex_lock(&query_dci_support_mutex);
		query_dci_support_rcvd = 1;
		pthread_cond_signal(&query_dci_support_cond);
		pthread_mutex_unlock(&query_dci_support_mutex);
	} else if (type == DIAG_QUERY_DCI_HEALTH_STATS) {
		memcpy(&health_stats, ptr, sizeof(health_stats));
		pthread_mutex_lock(&query_dci_health_stats_mutex);
		query_dci_health_stats_rcvd = 1;
		pthread_cond_signal(&query_dci_health_stats_cond);
		pthread_mutex_unlock(&query_dci_health_stats_mutex);
	} else if (type == DIAG_QUERY_DCI_LOG_MASK) {
		memcpy(&dci_log_status, ptr, sizeof(dci_log_status));
		pthread_mutex_lock(&query_dci_log_status_mutex);
		query_dci_log_status_rcvd = 1;
		pthread_cond_signal(&query_dci_log_status_cond);
		pthread_mutex_unlock(&query_dci_log_status_mutex);
	} else if (type == DIAG_QUERY_DCI_EVENT_MASK) {
		memcpy(&dci_event_status, ptr, sizeof(dci_event_status));
		pthread_mutex_lock(&query_dci_event_status_mutex);
		query_dci_event_status_rcvd = 1;
		pthread_cond_signal(&query_dci_event_status_cond);
		pthread_mutex_unlock(&query_dci_event_status_mutex);
	} else if (type == DIAG_QUERY_REAL_TIME) {
		memcpy(&real_time, ptr, sizeof(real_time));
		pthread_mutex_lock(&query_real_time_mutex);
		query_real_time_rcvd = 1;
		pthread_cond_signal(&query_real_time_cond);
		pthread_mutex_unlock(&query_real_time_mutex);
	} else if (type == DIAG_QUERY_CON_ALL_MASK) {
		all_con_params = *(struct diag_con_all_param_t*)ptr;
		pthread_mutex_lock(&query_con_mutex);
		query_con_rcvd = 1;
		pthread_cond_signal(&query_con_cond);
		pthread_mutex_unlock(&query_con_mutex);
	} else if (type == DIAG_QUERY_MD_PID_MASK) {
		memcpy(&query_md_pid_params, ptr, sizeof(struct diag_query_pid_t));
		pthread_mutex_lock(&query_md_pid_mutex);
		query_md_pid_rcvd = 1;
		pthread_cond_signal(&query_md_pid_cond);
		pthread_mutex_unlock(&query_md_pid_mutex);
	} else if (type == DIAG_SWITCH_LOGGING_MASK) {
		memcpy(&switch_logging_ret, ptr, sizeof(switch_logging_ret));
		pthread_mutex_lock(&query_switch_logging_mutex);
		query_switch_logging_rcvd = 1;
		pthread_cond_signal(&query_switch_logging_cond);
		pthread_mutex_unlock(&query_switch_logging_mutex);
	} else if (type == DIAG_QUERY_REG_CB_MASK) {
		memcpy(&reg_cb, ptr, sizeof(reg_cb));
		pthread_mutex_lock(&query_reg_cb_mutex);
		query_reg_cb_rcvd = 1;
		pthread_cond_signal(&query_reg_cb_cond);
		pthread_mutex_unlock(&query_reg_cb_mutex);
	} else if (type == DIAG_TIMESTAMP_SWITCH) {
		pthread_mutex_lock(&timestamp_switch_mutex);
		time_stamp_switch_rcvd = *(int*)ptr;
		pthread_mutex_unlock(&timestamp_switch_mutex);
	} else if (type == DIAG_QUERY_REG_TABLE_MASK) {
		query_reg_table_mask((char*)ptr);
	} else if (type == DIAG_SET_DYNAMIC_DEBUG_MASK) {
		diag_lib_dbg_mask = *(uint32_t *)ptr;
	} else if (type == DIAG_QUERY_DEBUG_ALL_MASK) {
			diag_print_debug_info(ptr);
			pthread_mutex_lock(&query_debug_all_mask_mutex);
			query_debug_all_mask_rcvd = 1;
			pthread_cond_signal(&query_debug_all_mask_cond);
			pthread_mutex_unlock(&query_debug_all_mask_mutex);
	} else if (type == DIAG_UPDATE_DIAG_ID_TYPE) {
		diag_id = *ptr;
		DIAG_LOGD("diag: %s: diag-id %d received from diag-router\n", __func__, diag_id);
	} else if (type == DIAG_UPDATE_PKT_FORMAT_SELECT_MASK_TYPE) {
		diag_id_async_cmd_mask = *ptr;
		DIAG_LOGD("diag: %s: diag-id based logging mask 0x%x\n", __func__, diag_id_async_cmd_mask);
	} else if(type == USER_SPACE_DATA_TYPE){
		if (logging_mode != MEMORY_DEVICE_MODE)
			log_to_device(ptr, logging_mode, payload_len, -1);
		else {
			pthread_mutex_lock(&(pools[curr_read].write_mutex));

			/* Wait if no buffer is free */
			pthread_mutex_lock(&(pools[curr_read].read_mutex));
			if (!pools[curr_read].free) {
				pthread_mutex_unlock(&(pools[curr_read].write_mutex));
				pthread_cond_wait(
					&(pools[curr_read].read_cond),
					&(pools[curr_read].read_mutex));
				pthread_mutex_lock(&(pools[curr_read].write_mutex));
			}
			pthread_mutex_unlock(&(pools[curr_read].read_mutex));

			/* One buffer got free continue writing */
			result = fill_pkt_buffer(ptr);
			if (result == FALSE) {
				pthread_mutex_unlock(&pools[curr_read].write_mutex);
				return;
			}

			pthread_mutex_unlock(&pools[curr_read].write_mutex);
		}
	}
	else if(type == DCI_DATA_TYPE) {
		if (!dci_client_tbl)
			return;
		while (dci_bytes_processed < num_bytes_read) {
			if (dci_bytes_processed == 0) {
				dci_proc = *(int *)ptr;
				if (!IS_VALID_DCI_PROC(dci_proc))
					return;
				ptr += sizeof(int);
				dci_data_len = *(int *)ptr;
				ptr += DCI_LEN_FIELD_LENGTH;
			} else {
				/* If the buffer has packets from multiple devices then DCI_DATA_TYPE will be encountered in the buffer for multiple devices*/
				type = *(int *)ptr;
				if (type == DCI_DATA_TYPE) {
					ptr += sizeof(int);
					dci_proc = *(int *)ptr;
					if (!IS_VALID_DCI_PROC(dci_proc))
						return;
					ptr += sizeof(int);
					dci_data_len = *(int *)ptr;
					ptr += DCI_LEN_FIELD_LENGTH;
				}
			}
			while (read_bytes < dci_data_len) {
				if (dci_client_tbl->data_signal_flag == ENABLE)
					if (raise(dci_client_tbl->data_signal_type))
						DIAG_LOGE("diag: dci: signal sending failed\n");
				if (*(int *)ptr == DCI_PKT_RSP_TYPE) {
					ptr += DCI_HEADER_LENGTH;
					read_bytes += DCI_HEADER_LENGTH;
					lookup_pkt_rsp_transaction(ptr, dci_proc);
					read_bytes += DCI_LEN_FIELD_LENGTH + DCI_DEL_FLAG_LEN + *(int *)(ptr);
					ptr += DCI_LEN_FIELD_LENGTH + DCI_DEL_FLAG_LEN + *(int *)(ptr);
				} else if (*(int *)ptr == DCI_LOG_TYPE) {
					ptr += DCI_HEADER_LENGTH;
					read_bytes += DCI_HEADER_LENGTH;
					if (dci_client_tbl[dci_proc].func_ptr_logs)
						(*(dci_client_tbl[dci_proc].func_ptr_logs))(ptr, *(uint16 *)(ptr));
					else
						DIAG_LOGE("diag: dci: no callback function registered for received log stream\n");
					read_bytes += *(uint16 *)(ptr);
					ptr += *(uint16 *)(ptr);
				} else if (*(int *)ptr == DCI_EVENT_TYPE) {
					ptr += DCI_HEADER_LENGTH;
					read_bytes += DCI_HEADER_LENGTH;
					if (dci_client_tbl[dci_proc].func_ptr_events)
						(*(dci_client_tbl[dci_proc].func_ptr_events))(ptr, DCI_EVENT_OFFSET + *(uint16 *)(ptr));
					else
						DIAG_LOGE("diag: dci: no callback function registered for received event stream\n");
					read_bytes += DCI_EVENT_OFFSET + *(uint16 *)(ptr);
					ptr += DCI_EVENT_OFFSET + *(uint16 *)(ptr);
				} else if (*(int *)ptr == DCI_EXT_HDR_TYPE) {
					ret = process_ext_header_packet(dci_proc, ptr);
					read_bytes += ret;
					ptr += ret;
				} else {
					DIAG_LOGE("diag: dci: unknown log type %d\n", *(int *)ptr);
					break;
				}
			}
			/* advance read bytes by 12 bytes to consider dci data type, mdm/msm token, dci type like pkt/rsp,log,event */
			dci_bytes_processed += read_bytes+ 3 * DCI_HEADER_LENGTH;
			read_bytes = 0;
		}
	}
}

void get_time_string(char *buffer, int len)
{
	struct timeval tv;
	struct tm *tm;
	unsigned long long milliseconds = 0;
	char timestamp_buf[30];

	if (!buffer || len <= 0)
		return;

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);

	if (!tm)
		return;

	milliseconds = (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
	strftime(timestamp_buf, 30, "%Y%m%d_%H%M%S", tm);

	(void)std_strlprintf(buffer, len, "%s%lld",
		timestamp_buf, milliseconds);
}

static void rename_logging_directory(void)
{
	char timestamp_buf[30];
	char new_dirname[FILE_NAME_LEN];
	char rename_cmd[RENAME_CMD_LEN];
	int replace_pos = -1;
	int index = 0;
	int len = 0;
	int status = 0;
	int i;

	if (!rename_dir_name)
		return;

	/* Loop backwards through the MSM directory name to find the date/time sub-directory */
	len = strlen(output_dir[0]);
	index = len - 1;
	for (i = 0; i < len && index >= 0; i++) {
		if (output_dir[0][index] == '/') {
			if (index != len - 1) {
				replace_pos = index + 1;
				break;
			}
		} else if ((output_dir[0][index] != '_') &&
				(output_dir[0][index] < '0') &&
				(output_dir[0][index] > '9')) {
			/* The last subdirectory is not in date/time format,
			 * do not rename the directory */
			replace_pos = -1;
			break;
		}
		index--;
	}

	if (replace_pos < 0) {
		DIAG_LOGE("diag: In %s, Not able to rename directory, invalid directory format, dir: %s\n",
			__func__, output_dir[0]);
		return;
	}

	get_time_string(timestamp_buf, sizeof(timestamp_buf));

	strlcpy(new_dirname, output_dir[0], FILE_NAME_LEN);
	new_dirname[replace_pos] = '\0';
	strlcat(new_dirname, timestamp_buf, FILE_NAME_LEN);

	/* Create rename command and issue it */
	(void)std_strlprintf(rename_cmd, RENAME_CMD_LEN, "mv %s %s",
			output_dir[0], new_dirname);

	status = system(rename_cmd);
	if (status == -1) {
		DIAG_LOGE("diag: In %s, Directory rename error (mv), errno: %d\n",
			__func__, errno);
		DIAG_LOGE("diag: Unable to rename directory %s to %s\n",
			output_dir[0], new_dirname);
	} else {
		/* Update current directory names */
		for (i = 0; i < NUM_PROC; i++)
			strlcpy(output_dir[i], new_dirname, FILE_NAME_LEN);
		DIAG_LOGE("diag: Renamed logging directory to: %s\n",
				output_dir[0]);
	}
}

int diag_get_pd_name_from_mask(char *buf,
					unsigned int len,
					unsigned int pd_mask)
{

	if (!buf || !len)
		return -1;

	if (pd_mask & DIAG_CON_UPD_WLAN) {
		strlcat(buf, "_wlan_pd", len);
		pd_mask  = pd_mask ^ DIAG_CON_UPD_WLAN;
	}
	if (pd_mask & DIAG_CON_UPD_AUDIO) {
		strlcat(buf, "_audio_pd", len);
		pd_mask  = pd_mask ^ DIAG_CON_UPD_AUDIO;
	}
	if (pd_mask & DIAG_CON_UPD_SENSORS) {
		strlcat(buf, "_sensors_pd", len);
		pd_mask = pd_mask ^ DIAG_CON_UPD_SENSORS;
	}
	if (pd_mask & DIAG_CON_UPD_CHARGER) {
		strlcat(buf, "_charger_pd", len);
		pd_mask = pd_mask ^ DIAG_CON_UPD_CHARGER;
	}
	if (pd_mask & DIAG_CON_UPD_OEM) {
		strlcat(buf, "_oem_pd", len);
		pd_mask = pd_mask ^ DIAG_CON_UPD_OEM;
	}
	if (pd_mask & DIAG_CON_UPD_OIS) {
		strlcat(buf, "_ois_pd", len);
		pd_mask = pd_mask ^ DIAG_CON_UPD_OIS;
	}
	if (pd_mask > 0)
		DIAG_LOGE("diag: Invalid pd mask set %d", pd_mask);

	return pd_mask;
}

void diag_get_peripheral_name_from_mask(char *buf,
					unsigned int len,
					unsigned int peripheral_mask)
{
	int ret = 0;

	if (!buf || !len)
		return;

	if (peripheral_mask & DIAG_CON_APSS) {
		strlcat(buf, "_apps", len);
		peripheral_mask  = peripheral_mask ^ DIAG_CON_APSS;
	}
	if (peripheral_mask & DIAG_CON_MPSS) {
		strlcat(buf, "_mpss", len);
		peripheral_mask  = peripheral_mask ^ DIAG_CON_MPSS;
	}
	if (peripheral_mask & DIAG_CON_LPASS) {
		strlcat(buf, "_adsp", len);
		peripheral_mask  = peripheral_mask ^ DIAG_CON_LPASS;
	}
	if (peripheral_mask & DIAG_CON_WCNSS) {
		strlcat(buf, "_wcnss", len);
		peripheral_mask  = peripheral_mask ^ DIAG_CON_WCNSS;
	}
	if (peripheral_mask & DIAG_CON_SENSORS) {
		strlcat(buf, "_slpi", len);
		peripheral_mask = peripheral_mask ^ DIAG_CON_SENSORS;
	}
	if (peripheral_mask & DIAG_CON_WDSP) {
		strlcat(buf, "_wdsp", len);
		peripheral_mask = peripheral_mask ^ DIAG_CON_WDSP;
	}
	if (peripheral_mask & DIAG_CON_CDSP) {
		strlcat(buf, "_cdsp", len);
		peripheral_mask = peripheral_mask ^ DIAG_CON_CDSP;
	}
	if (peripheral_mask & DIAG_CON_NPU) {
		strlcat(buf, "_npu", len);
		peripheral_mask = peripheral_mask ^ DIAG_CON_NPU;
	}
	if (peripheral_mask & DIAG_CON_NSP1) {
		strlcat(buf, "_cdsp1", len);
		peripheral_mask = peripheral_mask ^ DIAG_CON_NSP1;
	}
	if (peripheral_mask & DIAG_CON_GPDSP0) {
		strlcat(buf, "_gpdsp", len);
		peripheral_mask = peripheral_mask ^ DIAG_CON_GPDSP0;
	}
	if (peripheral_mask & DIAG_CON_GPDSP1) {
		strlcat(buf, "_gpdsp1", len);
		peripheral_mask = peripheral_mask ^ DIAG_CON_GPDSP1;
	}
	if (peripheral_mask & DIAG_CON_HELIOS_M55) {
		strlcat(buf, "_helios_m55", len);
		peripheral_mask = peripheral_mask ^ DIAG_CON_HELIOS_M55;
	}
	if (diag_peripheral_mask & peripheral_mask & DIAG_CON_SLATE_APPS) {
		strlcat(buf, "_slate_apps", len);
		peripheral_mask = peripheral_mask ^ DIAG_CON_SLATE_APPS;
	}
	if (diag_peripheral_mask & peripheral_mask & DIAG_CON_SLATE_ADSP) {
		strlcat(buf, "_slate_adsp", len);
		peripheral_mask = peripheral_mask ^ DIAG_CON_SLATE_ADSP;
	}

	ret = diag_get_pd_name_from_mask(buf, len, peripheral_mask);
	if (!ret)
		peripheral_mask = 0;

	if (peripheral_mask > 0)
		DIAG_LOGE("diag: Invalid peripheral mask set %d", peripheral_mask);

}

void diagid_set_qshrink4_status(int peripheral_type)
{
	diagid_guid_status[peripheral_type] = 1;
}

static void close_logging_file(int type)
{
	close(fd_md[type]);
	fd_md[type] = -1;

	if (rename_file_names && file_name_curr[type][0] != '\0') {
		int status;
		char timestamp_buf[30] = {0};
		char new_filename[FILE_NAME_LEN];
		char rename_cmd[RENAME_CMD_LEN];

		get_time_string(timestamp_buf, sizeof(timestamp_buf));

		if (hdlc_disabled[type] || use_qmdl2_v2) {
			if (diag_peripheral_mask)
				(void)std_strlprintf(new_filename,
					FILE_NAME_LEN, "%s%s%s%s%s%s",
					output_dir[type], "/diag_log",
					peripheral_name, "_", timestamp_buf, ".qmdl2");
			else if (diag_pd_mask)
				(void)std_strlprintf(new_filename,
					FILE_NAME_LEN, "%s%s%s%s%s%s",
					output_dir[type], "/diag_log",
					upd_name, "_", timestamp_buf, ".qmdl2");
			else
				(void)std_strlprintf(new_filename,
					FILE_NAME_LEN, "%s%s%s%s",
					output_dir[type],"/diag_log_",
					timestamp_buf, ".qmdl2");
		} else {
			if (diag_peripheral_mask)
				(void)std_strlprintf(new_filename,
					FILE_NAME_LEN, "%s%s%s%s%s%s",
					output_dir[type], "/diag_log",
					peripheral_name, "_",
					timestamp_buf, ".qmdl");
			else if (diag_pd_mask)
				(void)std_strlprintf(new_filename,
					FILE_NAME_LEN, "%s%s%s%s%s%s",
					output_dir[type], "/diag_log",
					upd_name, "_",
					timestamp_buf, ".qmdl");
			else
				(void)std_strlprintf(new_filename,
					FILE_NAME_LEN, "%s%s%s%s",
					output_dir[type], "/diag_log_",
					timestamp_buf, ".qmdl");
		}

		/* Create rename command and issue it */
		(void)std_strlprintf(rename_cmd, RENAME_CMD_LEN, "mv %s %s",
				file_name_curr[type], new_filename);

		status = system(rename_cmd);
		if (status == -1) {
			DIAG_LOGE("diag: In %s, File rename error (mv), errno: %d\n",
				__func__, errno);
			DIAG_LOGE("diag: Unable to rename file %s to %s\n",
				file_name_curr[type], new_filename);
		} else {
			/* Update current filename */
			strlcpy(file_name_curr[type], new_filename, FILE_NAME_LEN);
		}
	}
}

#define S_64K (64*1024)
void *WriteToDisk (void *ptr)
{
	unsigned int i;
	int z, rc = 0;
	unsigned int chunks, last_chunk;
	(void) ptr;
	sigset_t set;

	if ((sigemptyset((sigset_t *) &set) == -1) ||
	(sigaddset(&set, SIGUSR2) == -1) ||
	(sigaddset(&set, SIGTERM) == -1) ||
	(sigaddset(&set, SIGHUP) == -1) ||
	(sigaddset(&set, SIGUSR1) == -1) ||
	(sigaddset(&set, SIGINT) == -1))
		DIAG_LOGE("diag:%s: Failed to initialize block set\n", __func__);

	rc = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (rc != 0)
		DIAG_LOGE("diag:%s: Failed to block signal for write thread\n", __func__);

	while (1) {
		pthread_mutex_lock(&(pools[curr_write].write_mutex));
		if (!pools[curr_write].data_ready){
			pthread_cond_wait(&(pools[curr_write].write_cond),
						&(pools[curr_write].write_mutex));
		}
		write_in_progress = 1;

		for (z = 0; z < NUM_PROC; z++) {
			if (buffer_init[z] == READY) {
				chunks = pools[curr_write].bytes_in_buff[z] /
									S_64K;
				last_chunk =
					pools[curr_write].bytes_in_buff[z] %
									S_64K;
				for(i = 0; i < chunks; i++){
					log_to_device(
					pools[curr_write].buffer_ptr[z],
					MEMORY_DEVICE_MODE, S_64K, z);
					pools[curr_write].buffer_ptr[z] +=
									S_64K;
					}
				if( last_chunk > 0)
					log_to_device(
					pools[curr_write].buffer_ptr[z],
					MEMORY_DEVICE_MODE, last_chunk, z);
			}
		}

		write_in_progress = 0;

		/* File pool structure */
		pools[curr_write].data_ready = 0;

		for (z = 0; z < NUM_PROC; z++) {
			if (buffer_init[z] == READY) {
				pools[curr_write].bytes_in_buff[z] = 0;
				if(curr_write)
					pools[curr_write].buffer_ptr[z] =
							pool1_buffers[z];
				else
					pools[curr_write].buffer_ptr[z] =
							pool0_buffers[z];
			}
		}
		pools[curr_write].free = 1;
		if ( flush_log == 2){
			for (z = 0; z < NUM_PROC; z++) {
				if (buffer_init[z] == READY) {
					close_logging_file(z);
				}
			}

			if (rename_dir_name) {
				rename_logging_directory();
			}

			pthread_mutex_unlock(
				&(pools[curr_write].write_mutex));
			DIAG_LOGE(" Exiting....%s \n", __func__);
			pthread_mutex_lock(&stop_mutex);
			flush_log++;
			pthread_mutex_unlock(&stop_mutex);
			pthread_exit(NULL);
		}
		/* Free Read thread if waiting on same buffer */
		pthread_mutex_lock(&(pools[curr_write].read_mutex));
		pthread_cond_signal(&(pools[curr_write].read_cond));
		pthread_mutex_unlock(&(pools[curr_write].read_mutex));
		pthread_mutex_unlock(&(pools[curr_write].write_mutex));

		curr_write = !curr_write;
	}
	return NULL;
}

static void delete_oldest_file_list(int type)
{
	if (type >= 0 && type < NUM_PROC) {
		if (file_list[type])
			free(file_list[type]);
		file_list[type] = NULL;
		file_list_index[type] = -1;
		file_list_size[type] = 0;
	}
}

static void delete_qdss_oldest_file_list(int type)
{
	if (type >= 0 && type < NUM_PROC) {
		if (qdss_file_list[type])
			free(qdss_file_list[type]);
		qdss_file_list[type] = NULL;
		qdss_file_list_index[type] = -1;
		qdss_file_list_size[type] = 0;
	}
}

#define LOG_FILENAME_PREFIX_LEN 9

static int create_oldest_file_list(char *oldest_dir, int type)
{
	struct dirent **dirent_list = NULL;
	int num_entries = 0;
	int num_entries_capped = 0;
	char *name_ptr;
	int i;
	int status = 1;
	int num_bytes = 0;

	if (type < 0 || type >= NUM_PROC) {
		DIAG_LOGE("diag: In %s, Invalid type: %d, for directory: %s\n",
			__func__, type, oldest_dir);
		return 0;
	}

	/* If we need to find what files are in the directory */
	if (NULL == file_list[type]) {
		DIAG_LOGE("diag: Determining contents of directory %s for circular logging ...\n",
			oldest_dir);
		num_entries = scandir(oldest_dir, &dirent_list, 0,
			(int(*)(const struct dirent **, const struct dirent **))alphasort);
		if(!dirent_list) {
			DIAG_LOGE("diag: In %s, couldn't get the dirent_list, errno: %d, directory: %s\n",
				__func__, errno, oldest_dir);
			return 0;
		} else if (num_entries < 0) {
			DIAG_LOGE("diag: In %s, error determining directory entries, errno: %d, directory: %s\n",
				__func__, errno, oldest_dir);
			return 0;
		}

		/* We don't need the files "." and ".." */
		if (num_entries - 2 > 0) {
			file_list_size[type] = num_entries - 2;

			num_bytes = FILE_LIST_NAME_SIZE * file_list_size[type];
			file_list[type] = malloc(num_bytes);
		}

		/* If the directory contains some files */
		if (file_list[type]) {
			file_list_index[type] = 0;

			/* Copy the file names into our list */
			for (i = 0; i < num_entries; i++)
			{
				/* Don't copy any file names that are not logging files
				 * Since we are creating only one adpl logging file,
				 * We should not delete file where file name prefix is
				 * "diag_log_ADPL" (len: LOG_FILENAME_PREFIX_LEN +
				 * len of "ADPL" i.e. 4)
				 *
				 * Skip deleting QDSS logging file in this path since
				 * since its handled in qdss path.
				 */
				if ((strncmp(dirent_list[i]->d_name, "diag_log_",
					LOG_FILENAME_PREFIX_LEN) != 0) ||
					(strncmp(dirent_list[i]->d_name, "diag_log_ADPL",
					LOG_FILENAME_PREFIX_LEN + 4) == 0) ||
					(strncmp(dirent_list[i]->d_name, "diag_qdss_log_",
					LOG_FILENAME_PREFIX_LEN + 5) == 0))
					continue;

				if (file_list_index[type] < file_list_size[type]) {
					name_ptr = file_list[type] +
						(file_list_index[type] * FILE_LIST_NAME_SIZE);
					strlcpy(name_ptr, dirent_list[i]->d_name, FILE_LIST_NAME_SIZE);
					*(name_ptr + (FILE_LIST_NAME_SIZE - 1)) = 0;
					file_list_index[type]++;
				}

				/* Limit the size of the list so we aren't working with too many files */
				if (num_entries_capped++ > MAX_FILES_IN_FILE_LIST)
					break;
			}

			if (file_list_index[type] > 0) {
				if (file_list_index[type] < file_list_size[type]) {
					/* There are files in the directory that are
					 * not logging files. Reduce the memory size
					 * allocated to the list */
					int new_size = FILE_LIST_NAME_SIZE *file_list_index[type];
					char *temp_ptr = realloc(file_list[type], new_size);
					if (temp_ptr)
						file_list[type] = temp_ptr;
				}
				file_list_size[type] = file_list_index[type];
				file_list_index[type] = 0;
			} else {
				/* There were no logging files in the directory. Clean up */
				delete_oldest_file_list(type);
			}
		} else if (num_bytes > 0) {
			DIAG_LOGE("diag: In %s, memory allocation error for directory: %s, type: %d\n",
				__func__, oldest_dir, type);
			status = 0;
		}

		/* Deallocate directory entry list */
		i = num_entries;
		while (i--) {
			free(dirent_list[i]);
		}
		free(dirent_list);
	}

	return status;
}

static int create_qdss_oldest_file_list(char *oldest_dir, int type)
{
	struct dirent **dirent_list = NULL;
	int num_entries = 0;
	int num_entries_capped = 0;
	char *name_ptr;
	int i;
	int status = 1;
	int num_bytes = 0;

	if (type < 0 || type >= NUM_PROC) {
		DIAG_LOGE("diag: In %s, Invalid type: %d, for directory: %s\n",
			__func__, type, oldest_dir);
		return 0;
	}

	/* If we need to find what files are in the directory */
	if (NULL == qdss_file_list[type]) {
		DIAG_LOGE("diag: Determining contents of directory %s for circular logging ...\n",
			oldest_dir);
		num_entries = scandir(oldest_dir, &dirent_list, 0,
			(int(*)(const struct dirent **, const struct dirent **))alphasort);
		if(!dirent_list) {
			DIAG_LOGE("diag: In %s, couldn't get the dirent_list, errno: %d, directory: %s\n",
				__func__, errno, oldest_dir);
			return 0;
		} else if (num_entries < 0) {
			DIAG_LOGE("diag: In %s, error determining directory entries, errno: %d, directory: %s\n",
				__func__, errno, oldest_dir);
			return 0;
		}

		/* We don't need the files "." and ".." */
		if (num_entries - 2 > 0) {
			qdss_file_list_size[type] = num_entries - 2;

			num_bytes = FILE_LIST_NAME_SIZE * qdss_file_list_size[type];
			qdss_file_list[type] = malloc(num_bytes);
		}

		/* If the directory contains some files */
		if (qdss_file_list[type]) {
			qdss_file_list_index[type] = 0;

			/* Copy the file names into our list */
			for (i = 0; i < num_entries; i++)
			{
				/* Don't copy any file names that are not logging files.
				 * Add file names that are only with name "diag_qdss_log"
				 * (len: LOG_FILENAME_PREFIX_LEN + len of "qdss_" i.e. 5)
				 */
				if ((strncmp(dirent_list[i]->d_name, "diag_qdss_log_",
					LOG_FILENAME_PREFIX_LEN + 5) != 0))
					continue;

				if (qdss_file_list_index[type] < qdss_file_list_size[type]) {
					name_ptr = qdss_file_list[type] +
						(qdss_file_list_index[type] * FILE_LIST_NAME_SIZE);
					strlcpy(name_ptr, dirent_list[i]->d_name, FILE_LIST_NAME_SIZE);
					*(name_ptr + (FILE_LIST_NAME_SIZE - 1)) = 0;
					qdss_file_list_index[type]++;
				}

				/* Limit the size of the list so we aren't working with too many files */
				if (num_entries_capped++ > MAX_FILES_IN_FILE_LIST)
					break;
			}
			if (qdss_file_list_index[type] > 0) {
				if (qdss_file_list_index[type] < qdss_file_list_size[type]) {
					/* There are files in the directory that are
					 * not logging files. Reduce the memory size
					 * allocated to the list */
					int new_size = FILE_LIST_NAME_SIZE * qdss_file_list_index[type];
					char *temp_ptr = realloc(qdss_file_list[type], new_size);
					if (temp_ptr)
						qdss_file_list[type] = temp_ptr;
				}
				qdss_file_list_size[type] = qdss_file_list_index[type];
				qdss_file_list_index[type] = 0;
			} else {
				/* There were no logging files in the directory. Clean up */
				delete_qdss_oldest_file_list(type);
			}
		} else if (num_bytes > 0) {
			DIAG_LOGE("diag: In %s, memory allocation error for directory: %s, type: %d\n",
				__func__, oldest_dir, type);
			status = 0;
		}

		/* Deallocate directory entry list */
		i = num_entries;
		while (i--) {
			free(dirent_list[i]);
		}
		free(dirent_list);
	}

	return status;
}

static int get_oldest_file(char* oldest_file, char *oldest_dir, int type)
{
	int status = 0;

	if (type < 0 || type >= NUM_PROC) {
		DIAG_LOGE("diag: In %s, Invalid type: %d, for directory: %s\n",
			__func__, type, oldest_dir);
		return 0;
	}

	/* If we need to find what files are in the directory */
	if (NULL == file_list[type])
		status = create_oldest_file_list(oldest_dir, type);

	if (file_list[type]) {
		if (oldest_file) {
			strlcpy(oldest_file, (file_list[type] +
				(file_list_index[type] * FILE_LIST_NAME_SIZE)),
				FILE_LIST_NAME_SIZE);
			file_list_index[type]++;
			/* If we have exhausted the list */
			if (file_list_index[type] >= file_list_size[type]) {
				/* Deallocate the file list and set up for determining
				 * directory entries on next call */
				delete_oldest_file_list(type);
			}
			status = 1;
		} else {
			DIAG_LOGE("diag: In %s, oldest_file is NULL\n", __func__);
		}
	} else {
		status = 0;
		if (file_list_size[type] == 0) {
			/* The directory does not have any logging files in it,
			 * so we must return an error status since we cannot return
			 * an oldest file name */
			DIAG_LOGE("diag: In %s, Directory %s contains no logging files\n",
			__func__, oldest_dir);
		} else {
			DIAG_LOGE("diag: In %s, Error determining directory file list for directory: %s, type: %d\n",
				__func__, oldest_dir, type);
		}
	}

	return status;
}

static int get_qdss_oldest_file(char* oldest_file, char *oldest_dir, int type)
{
	int status = 0;

	if (type < 0 || type >= NUM_PROC) {
		DIAG_LOGE("diag: In %s, Invalid type: %d, for directory: %s\n",
			__func__, type, oldest_dir);
		return 0;
	}

	/* If we need to find what files are in the directory */
	if (NULL == qdss_file_list[type])
		status = create_qdss_oldest_file_list(oldest_dir, type);

	if (qdss_file_list[type]) {
		if (oldest_file) {
			strlcpy(oldest_file, (qdss_file_list[type] +
				(qdss_file_list_index[type] * FILE_LIST_NAME_SIZE)),
				FILE_LIST_NAME_SIZE);
			qdss_file_list_index[type]++;
			/* If we have exhausted the list */
			if (qdss_file_list_index[type] >= qdss_file_list_size[type]) {
				/* Deallocate the file list and set up for determining
				 * directory entries on next call */
				delete_qdss_oldest_file_list(type);
			}
			status = 1;
		} else {
			DIAG_LOGE("diag: In %s, oldest_file is NULL\n", __func__);
		}
	} else {
		status = 0;
		if (qdss_file_list_size[type] == 0) {
			/* The directory does not have any logging files in it,
			 * so we must return an error status since we cannot return
			 * an oldest file name */
			DIAG_LOGE("diag: In %s, Directory %s contains no logging files\n",
			__func__, oldest_dir);
		} else {
			DIAG_LOGE("diag: In %s, Error determining directory file list for directory: %s, type: %d\n",
				__func__, oldest_dir, type);
		}
	}

	return status;
}

int delete_log(int type)
{
	int status;
	char oldest_file[FILE_LIST_NAME_SIZE] = "";
	struct stat file_stat;

	status = get_oldest_file(oldest_file,
				output_dir[type], type);
	if (0 == status) {
		DIAG_LOGE("diag: In %s, Unable to determine oldest file for deletion\n",
			__func__);
		return -1;
	}

	std_strlprintf(file_name_del,
			FILE_NAME_LEN, "%s%s%s",
			output_dir[type], "/", oldest_file);

	if (!strncmp(file_name_curr[type], file_name_del, FILE_NAME_LEN)) {
		DIAG_LOGE("diag: In %s, Cannot delete file, file %s is in use \n",
			__func__, file_name_curr[type]);
		return -1;
	}

	stat(file_name_del, &file_stat);

	/* Convert size to KB */
	file_stat.st_size /= 1024;

	if (unlink(file_name_del)) {
		DIAG_LOGE("diag: In %s, Unable to delete file: %s, errno: %d\n",
				__func__, file_name_del, errno);
		return -1;
	} else {
		DIAG_LOGE("diag: In %s, Deleting logfile %s of size %lld KB\n",
				__func__, file_name_del,
				(long long int) file_stat.st_size);
	}
	return 0;
}

int delete_qdss_log(int type)
{
	int status;
	char oldest_file[FILE_LIST_NAME_SIZE] = "";
	struct stat file_stat;

	status = get_qdss_oldest_file(oldest_file,
				output_dir[type], type);
	if (0 == status) {
		DIAG_LOGE("diag: In %s, Unable to determine oldest file for deletion\n",
			__func__);
		return -1;
	}

	std_strlprintf(qdss_file_name_del,
			FILE_NAME_LEN, "%s%s%s",
			output_dir[type], "/", oldest_file);

	if (!strncmp(qdss_file_name_curr[type], qdss_file_name_del, FILE_NAME_LEN)) {
		DIAG_LOGE("diag: In %s, Cannot delete file, file %s is in use \n",
			__func__, qdss_file_name_curr[type]);
		return -1;
	}

	stat(qdss_file_name_del, &file_stat);

	/* Convert size to KB */
	file_stat.st_size /= 1024;

	if (unlink(qdss_file_name_del)) {
		DIAG_LOGE("diag: In %s, Unable to delete file: %s, errno: %d\n",
				__func__, qdss_file_name_del, errno);
		return -1;
	} else {
		DIAG_LOGE("diag: In %s, Deleting logfile %s of size %lld KB\n",
				__func__, qdss_file_name_del,
				(long long int) file_stat.st_size);
	}
	return 0;
}

static void get_circular_logging_info(int type)
{
	if (type < 0 || type >= NUM_PROC)
		return;
	if (!circular_logging_inited) {

		if (!file_list[type]) {
			char dir_name[FILE_NAME_LEN];
			strlcpy(dir_name, output_dir[type], FILE_NAME_LEN);
			if (buffer_init[type] != READY) {
				/* The output directory name has not been fully constructed yet */
				(void)strlcat(dir_name, proc_name[type], FILE_NAME_LEN);
			}
			create_oldest_file_list(dir_name, type);
		}
		if (!qdss_file_list[type]) {
			char qdss_dir_name[FILE_NAME_LEN];
			strlcpy(qdss_dir_name, output_dir[type], FILE_NAME_LEN);
			(void)strlcat(qdss_dir_name, proc_name[type], FILE_NAME_LEN);
			create_qdss_oldest_file_list(qdss_dir_name, type);
		}
	}
}

static void init_circular_logging(void)
{
	int i;
	int num_lists = 0, qdss_num_lists = 0;

	for (i = 0; i < NUM_PROC; i++) {
		file_count[i] = 0;
		if (file_list[i] && file_list_size[i] > 0) {
			file_count[i] += file_list_size[i];
			num_lists++;
		}

		if (qdss_file_list[i] && qdss_file_list_size[i] > 0) {
		        qdss_file_count[i] = 0;
			qdss_file_count[i] += qdss_file_list_size[i];
			qdss_num_lists++;
		}
	}

	/* If circular logging is enabled and there are too many logging files on the SD card */
	for (i = 0; i < NUM_PROC; i++) {
		if(max_file_num > 1 && (file_count[i] >= max_file_num)) {
			int status = -1;
			DIAG_LOGE("diag: Initializing circular logging, reducing number of pre-existing logging files, current: %d, max: %d\n",
						  file_count[i], max_file_num);
			while ((file_count[i] >= max_file_num) && (num_lists > 0)) {
				/* Iterate over the lists to reduce the logging files
				 * in each logging directory one by one */
				num_lists = 0;
				if (file_list[i] && file_list_size[i] > 0) {
					status = delete_log(i);
					/* Break out if there was an error */
					if (status)
						break;
					file_count[i]--;
					num_lists++;
				}
			}
			if (status) {
				DIAG_LOGE("diag: In %s, Problem initializing circular logging, continuing ...\n",
					__func__);
			}
		}
		if(max_file_num > 1 && (qdss_file_count[i] > max_file_num)) {
			int qdss_status = -1;
			DIAG_LOGE("diag: Initializing qdss circular logging, reducing number of pre-existing qdss logging files, current: %d, max: %d\n",
						  qdss_file_count[i], max_file_num);
				while ((qdss_file_count[i] > max_file_num) && (qdss_num_lists > 0)) {
					/* Iterate over the lists to reduce the logging files
					 * in each logging directory one by one */
					qdss_num_lists = 0;
					if (qdss_file_list[i] && qdss_file_list_size[i] > 0) {
						qdss_status = delete_qdss_log(i);
						/* Break out if there was an error */
						if (qdss_status)
							break;

						qdss_file_count[i]--;
						qdss_num_lists++;
					}
				}

			if (qdss_status) {
				DIAG_LOGE("diag: In %s, Problem initializing circular logging for qdss session, continuing ...\n",
					__func__);
			}
		}
	}
}

int write_qdss_header(int fd, int proc)
{
	size_t legacy_header_len = 0;
	size_t diagid_entry_size = 0;
	size_t key_info_size = 0;
	qshrink4_header header;
	int count = 0;
	int ret;

	if (!diagid_query_status[proc] || !diagid_guid_status[proc])
		return -1;

	/* Query for key on every new file */
	diag_get_secure_diag_info(proc, FILE_TYPE_QDSS);

	/* Copy qmdl header */
	memcpy(&header, &qshrink4_data[proc], sizeof(header));
	legacy_header_len = DIAG_QMDL2_HDR_MIN_LEN + (header.guid_list_entry_count * GUID_LEN);

	/* Update header version with correct qmdl2 version and data type for qdss */
	header.hdlc_data_type = 3;
	if (keys_stored(proc)) {
		key_info_size = get_keys_header_size(proc);
		header.version = 3;
	}
	diagid_entry_size = sizeof(diagid_entry_count[proc]) +
			    (diagid_entry_count[proc] * sizeof(diagid_guid_struct));

	/* Update calculated header length */
	header.header_length = legacy_header_len + diagid_entry_size + key_info_size;

	/* Write first part of header */
	ret = write(fd, &header, legacy_header_len);
	if (ret > 0)
		count += ret;

	/* Write the diagid count info in qmdl2 v2 header */
	ret = write(fd, &diagid_entry_count[proc], sizeof(diagid_entry_count[proc]));
	if (ret > 0)
		count += ret;

	/* Write the diagid guid mapping info in qmdl2 v2 header */
	ret = write(fd, &diagid_guid[proc], diagid_entry_count[proc] * sizeof(diagid_guid_struct));
	if (ret > 0)
		count += ret;

	/* Add secure key information after diagid guid mapping */
	if (header.version == 3) {
		ret = write_key_header(fd, proc);
		if (ret > 0)
			count += ret;
	}

	return count;
}

/*
 * NOTE: Please always pass type as -1, type is only valid for mdlog.
 *
 */
void log_to_device(unsigned char *ptr, int logging_mode, int size, int type)
{
	uint32 count_received_bytes;
	int i, ret, rc, proc = MSM, z;
	int num_data_fields = 0, len;
	unsigned char *base_ptr = ptr;
	char timestamp_buf[30];
	int token, diagid_entry_size = 0;
	unsigned int bytes_read = 0;
	unsigned int bytes_remaining;
	unsigned char *sock_ptr;
	struct stat logging_file_stat;
	size_t key_info_size = 0;

	if (ptr == NULL || size <= 0) {
		DIAG_LOGE("diag: Invalid ptr in %s size is %d\n",__func__,size);
		return;
	}
	num_data_fields = *(int *)ptr;
	bytes_remaining = size;
	ptr += sizeof(int);
	bytes_read += sizeof(int);
	if (CALLBACK_MODE == logging_mode) {
		for (i = 0; i < num_data_fields && bytes_remaining > 0; i++) {
			token = valid_token(*(int *)ptr);
			if (token == -1) {
				DIAG_LOGE("diag: Invalid token in %s, CALLBACK MODE", __func__);
				return;
			}
			if (token > 0) {
				ptr += sizeof(int);
				bytes_read += sizeof(int);
			}
			count_received_bytes = *(uint32 *)ptr;
			ptr += sizeof(uint32);
			bytes_read += sizeof(uint32);
			if (!cb_clients[token].cb_func_ptr) {
				DIAG_LOGE("diag: In %s, no callback function registered for proc %d\n",
					  __func__, token);
				return;
			}

			if (bytes_remaining < (bytes_read + count_received_bytes)) {
				DIAG_LOGE("diag: In %s received packet of invalid length bytes remaining:%u bytes_read:%u count_received_bytes:%u\n",
					  __func__, bytes_remaining, bytes_read, count_received_bytes);
				return;
			}

			(*(cb_clients[token].cb_func_ptr))((void *) ptr,
							   count_received_bytes,
							   cb_clients[token].context_data);
			ptr += count_received_bytes;
			bytes_read += count_received_bytes;
			bytes_remaining -= bytes_read;
			bytes_read = 0;
		}
		return;
	}

	if (type >= 0 && (buffer_init[type] == READY))
		fd_dev = fd_md[type];
	else if (logging_mode == MEMORY_DEVICE_MODE) {
		DIAG_LOGE("\n Buffer not initialized %d\n", type);
		return;
	}

	if ((logging_mode == MEMORY_DEVICE_MODE) &&
		((count_written_bytes[type] + size >= max_file_size) ||
		(count_written_bytes[type] + size > MDLOG_WRITTEN_BYTES_LIMIT))) {
		close_logging_file(type);
		fd_dev = fd_md[type];
		count_written_bytes[type] = 0;
	}

	if (fd_dev < 0) {
		if(logging_mode == MEMORY_DEVICE_MODE) {
			if (!circular_logging_inited) {
				init_circular_logging();
				circular_logging_inited = 1;
			}
			/* Check if we are to start circular logging on the basis
			 * of maximum number of logging files in the logging
			 * directories on the SD card */
			if(max_file_num > 1 && (file_count[type] >= max_file_num)) {
				DIAG_LOGE("diag: In %s, File count reached max file num %u so deleting oldest file\n",
					__func__, max_file_num);
				rc = -1;
				if (buffer_init[type] == READY) {
					if (!delete_log(type)) {
						file_count[type]--;
						rc = 0;
					}
				}

				if (rc) {
					DIAG_LOGE("Delete failed \n");
					return;
				}
			}

			/* Construct the file name using the current time stamp */
			get_time_string(timestamp_buf, sizeof(timestamp_buf));

			if (hdlc_disabled[type] || use_qmdl2_v2) {
				if (diag_peripheral_mask)
					(void)std_strlprintf(file_name_curr[type],
							FILE_NAME_LEN, "%s%s%s%s%s%s",
							output_dir[type], "/diag_log",
							peripheral_name, "_", timestamp_buf, ".qmdl2");
				else if (diag_pd_mask)
					(void)std_strlprintf(file_name_curr[type],
							FILE_NAME_LEN, "%s%s%s%s%s%s",
							output_dir[type], "/diag_log",
							upd_name, "_", timestamp_buf, ".qmdl2");
				else
					(void)std_strlprintf(file_name_curr[type],
							FILE_NAME_LEN, "%s%s%s%s",
							output_dir[type],"/diag_log_",
							timestamp_buf, ".qmdl2");
			} else {
				if (diag_peripheral_mask)
					(void)std_strlprintf(file_name_curr[type],
							FILE_NAME_LEN, "%s%s%s%s%s%s",
							output_dir[type], "/diag_log",
							peripheral_name, "_", timestamp_buf, ".qmdl");
				else if (diag_pd_mask)
					(void)std_strlprintf(file_name_curr[type],
							FILE_NAME_LEN, "%s%s%s%s%s%s",
							output_dir[type], "/diag_log",
							upd_name, "_", timestamp_buf, ".qmdl");
				else
					(void)std_strlprintf(file_name_curr[type],
							FILE_NAME_LEN, "%s%s%s%s",
							output_dir[type],"/diag_log_",
							timestamp_buf, ".qmdl");
				}

			fd_md[type] = open(file_name_curr[type],
					O_CREAT | O_RDWR | O_SYNC | O_TRUNC,
					0644);
			fd_dev = fd_md[type];
			if (fd_md[type] < 0) {
				DIAG_LOGE(" File open error, please check");
				DIAG_LOGE(" memory device %d, errno: %d \n",
							fd_md[type], errno);
			}
			else {
				DIAG_LOGE(" creating new file %s \n",
							file_name_curr[type]);
				file_count[type]++;
				/*
				 * The qshrink4 header needs to be the first
				 * information written in the qmdl2 file
				 */
				 if (hdlc_disabled[type] || use_qmdl2_v2)
					write_qshrink_header[type] = 1;
			}
		} else if ((logging_mode == UART_MODE) ||
					(logging_mode == SOCKET_MODE))
				DIAG_LOGE(" Invalid file descriptor\n");
	}
	if (fd_dev != -1) {
		if(logging_mode == MEMORY_DEVICE_MODE ) {
			errno = 0;
			if (type < 0 || (use_qmdl2_v2 && (!diagid_query_status[type] || !diagid_guid_status[type])))
				return;
			if ((use_qmdl2_v2 || hdlc_disabled[type]) && write_qshrink_header[type]) {
				write_qshrink_header[type] = 0;
				if (use_qmdl2_v2) {
					diagid_entry_size = sizeof(diagid_entry_count[type]) +
						(diagid_entry_count[type] * sizeof(diagid_guid_struct));

					if (!qmdl2_v2_header_inited[type] && diagid_entry_count[type]) {
						qshrink4_data[type].header_length += diagid_entry_size;
						qmdl2_v2_header_inited[type] = 1;
					}

					if (qmdl2_v2_header_inited[type] &&
						(qshrink4_data[type].header_length > diagid_entry_size)) {

						/* Query for key on every new file */
						diag_get_secure_diag_info(type, FILE_TYPE_QMDL2);

						/*
						 * Write the header information for the QSHRINK4 in
						 * case for qmdl2 v2 file.
						 */
						len = DIAG_QMDL2_HDR_MIN_LEN + (qshrink4_data[type].guid_list_entry_count * GUID_LEN);

						/* Update header to v3 if secure key info available */
						if (!qmdl2_v3_header_key_inited[type] && keys_stored(type)) {
							key_info_size = get_keys_header_size(type);
							qshrink4_data[type].header_length += key_info_size;
							qshrink4_data[type].version = 3;
							qmdl2_v3_header_key_inited[type] = 1;
						}

						ret = write(fd_dev, (const void*)&qshrink4_data[type], len);
						if (ret > 0)
							count_written_bytes[type] += ret;

						/*
						 * Write the diagid count info in qmdl2 v2 header
						 */
						ret = write(fd_dev, (const void*)&diagid_entry_count[type],
							sizeof(diagid_entry_count[type]));
						if (ret > 0)
							count_written_bytes[type] += sizeof(diagid_entry_count[type]);

						/*
						 * Write the diagid guid mapping info in qmdl2 v2 header
						 */
						ret = write(fd_dev, (const void*)&diagid_guid[type],
							diagid_entry_count[type] * sizeof(diagid_guid_struct));
						if (ret > 0)
							count_written_bytes[type] +=
							(diagid_entry_count[type] * sizeof(diagid_guid_struct));
					} else {
						DIAG_LOGD("%s: Adding qmdl2 header without DIAGID-GUID mapping: p_type: %d:, qshrink4_filelist_rsp[type]: %d, qmdl2_v2_header_inited: %d, header_length: %d, diagid_entry_count: %d\n",
							__func__, type, qshrink4_filelist_rsp[type], qmdl2_v2_header_inited[type],
							qshrink4_data[type].header_length, diagid_entry_count[type]);

						ret = write(fd_dev, (const void*)&qshrink4_data[type],
							qshrink4_data[type].header_length);
						if (ret > 0)
							count_written_bytes[type] +=
								qshrink4_data[type].header_length;
					}

					/* Add secure key information after diagid guid mapping */
					if (keys_stored(type)) {
						ret = write_key_header(fd_dev, type);
						if (ret > 0)
							count_written_bytes[type] += ret;
					}
				} else {
					ret = write(fd_dev, (const void*)&qshrink4_data[type],
						qshrink4_data[type].header_length);
					if (ret > 0)
						count_written_bytes[type] +=
							qshrink4_data[type].header_length;
				}
			}
			if (!stat(file_name_curr[type], &logging_file_stat)) {
				ret = write(fd_dev, (const void*) base_ptr, size);
			} else {
				close(fd_md[type]);
				fd_md[type] = -1;
				ret = -EINVAL;
			}

			if ( ret > 0) {
				count_written_bytes[type] += size;
			} else {
				DIAG_LOGE("diag: In %s, error writing to sd card, %s, errno: %d\n",
					__func__, strerror(errno), errno);
				if (errno == ENOSPC) {
					/* Delete oldest file */
					rc = -1;
					for (z = 0; z < NUM_PROC; z++) {
						if (buffer_init[z] == READY)
							if (!delete_log(z))
								rc = 0;
					}

					if (rc) {
						DIAG_LOGE("Delete failed \n");
						return;
					}

					/* Close file if it is big enough */
					if(count_written_bytes[type] >
							min_file_size) {
						close_logging_file(type);
						fd_dev = fd_md[type];
						count_written_bytes[type] = 0;
					}else {
						DIAG_LOGE(" Disk Full "
							"Continuing with "
						"same file [%d] \n", type);
					}

					log_to_device(base_ptr,
						MEMORY_DEVICE_MODE, size,
						type);
					return;
				} else
					DIAG_LOGE(" failed to write "
						"to file, device may"
						" be absent, errno: %d\n",
						errno);
			}
		} else if (SOCKET_MODE == logging_mode) {
			int is_mdm;

			for (i = 0; i < num_data_fields; i++) {
				if (*(int *)ptr < 0) {
					is_mdm = 1;
					token = *(int *)ptr;
					ptr += 4;
				} else {
					is_mdm = 0;
				}

				memcpy((char*)&count_received_bytes, ptr, sizeof(uint32));
				ptr += sizeof(uint32);
				sock_ptr = ptr;
				bytes_remaining = (int)count_received_bytes;

				if (is_mdm) {
					fd_dev = get_remote_socket_fd(token);
					if (fd_dev == -1) {
						/*
						 * There is currently no socket fd for this token.
						 * Dismiss this data by incrementing through the buffer
						 * to the next data field.
						 */
						ptr += count_received_bytes;
						continue;
					}
				} else {
					fd_dev = fd_socket[MSM];
				}

				while ((bytes_remaining > 0) && (fd_dev != -1)) {
					ret = send(fd_dev, (const void*)sock_ptr,
							bytes_remaining, MSG_NOSIGNAL);
					if (ret > 0) {
						/*
						 * There is a possibility that not all
						 * the data was written to the socket.
						 * We must continue sending data on
						 * the socket until all the data is
						 * written.
						 */
						bytes_remaining -= ret;
						count_written_bytes_1 += ret;
						sock_ptr += ret;
					} else {
						if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
							fd_set write_fs;
							int status;
							FD_ZERO(&write_fs);
							FD_SET(fd_dev, &write_fs);
							status = select(fd_dev+1, NULL, &write_fs, NULL, NULL);
							if (status == -1) {
								DIAG_LOGE("In %s: Error calling select for write, %s, errno: %d\n",
									__func__, strerror(errno), errno);
							} else if (!FD_ISSET(fd_dev, &write_fs)) {
								DIAG_LOGE("In %s: FD_ISSET is false after write select call\n",
									__func__);
							}
						} else {
							DIAG_LOGE("%s, Error writing to socket: %s, errno: %d, "
								"count_received_bytes: %u\n",
								__func__, strerror(errno), errno,
								(unsigned int)count_received_bytes);
							break;
						}
					}
				}
				ptr += count_received_bytes;
			}
		} else if (UART_MODE == logging_mode) {
			for (i = 0; i < num_data_fields; i++) {
				/* Check if data is from MDM Proc
				 * For QSC we can check -2 also and
				 * based on that set value of is_mdm
				 */
				proc = valid_token(*(int *)ptr);
				if (proc > 0)
					ptr += 4;
				else
					proc= MSM;

				memcpy((char*)&count_received_bytes,ptr,sizeof(uint32));
				ptr += sizeof(uint32);
				if (proc == uart_logging_proc) {
					ret = write(fd_dev, (const void*) ptr,
							count_received_bytes);
					if ( ret > 0) {
						count_written_bytes_1 +=
							count_received_bytes;
					} else {
						DIAG_LOGE("failed to write"
							"%u bytes to file,"
							"device may be"
							"absent,errno: %d \n",
						(unsigned int)count_received_bytes,
						errno);
					}
				}

				ptr += count_received_bytes;
			}
		} else
			DIAG_LOGE("diag: Incorrect logging mode\n");
	}
}
int diag_read_mask_file()
{
	return diag_read_mask_file_proc(proc_type);
}
/*===========================================================================
FUNCTION   diag_read_mask_file

DESCRIPTION
  This reads the mask file

DEPENDENCIES
  valid data type to be passed in

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

int diag_set_masks(int proc_type, const char *mask_file)
{
	unsigned char mask_buf[MASK_FILE_BUF_SIZE];
	int count_mask_bytes = 0;
	int ch, index, i = 0;
	int payload = 0, flag = 0;
	int found_cmd = 0;
	FILE *read_mask_fp;
	char *open_mask_file = NULL;

	*(int *)mask_buf = USER_SPACE_DATA_TYPE;
	count_mask_bytes = 4;

	index = valid_token(-1 * proc_type);
	if (mask_file)
		open_mask_file = strndup(mask_file, strlen(mask_file));
	else if (!hdlc_disabled[index])
		open_mask_file = mask_file_proc[index];
	else
		open_mask_file = mask_file2_proc[index];

	if (open_mask_file) {
		if ((read_mask_fp = fopen(open_mask_file, "rb")) == NULL) {
				DIAG_LOGE("Sorry, can't open mask file: %s,"
				" for index: %d, please check the device, errno: %d\n",
					open_mask_file, index, errno);
			return -1;
		}
	} else {
		DIAG_LOGE("Invalid proc type\n");
		return -1;
	}
	DIAG_LOGE("Reading the mask file: %s\n", open_mask_file);
	get_circular_logging_info(proc_type);

	if (!hdlc_disabled[index]) {
		while (1) {
			ch = fgetc(read_mask_fp);
			if (ch == EOF)
				break;
			if (count_mask_bytes < MASK_FILE_BUF_SIZE) {
				if ((index > 0) && (count_mask_bytes == 4)) {
					*(int *)(mask_buf + count_mask_bytes) =
									token_list[index];
					count_mask_bytes = 8;
				}

				mask_buf[count_mask_bytes] = ch;
				if (mask_buf[count_mask_bytes] == CONTROL_CHAR) {
#ifdef DIAG_DEBUG
					DIAG_LOGE("********************************** \n");
					for (i = 0; i <= count_mask_bytes; i++) {
						DIAG_LOGE("%2x ", mask_buf[i]);
					}
					DIAG_LOGE("********************************** \n");
#endif
					if (!found_cmd)
						found_cmd = 1;
					diag_send_data(mask_buf, count_mask_bytes+1);
					*(int *)mask_buf = USER_SPACE_DATA_TYPE;
					count_mask_bytes = 4;
				} else {
					count_mask_bytes++;
				}
			}
		}
		if (!found_cmd) {
			DIAG_LOGE("Sorry, could not find valid commands in the mask file,"
					"please check the mask file again\n");
			return -1;
		}
	} else {
		while (1) {
			ch = fgetc(read_mask_fp);
			if (ch == EOF)
				break;
			if (count_mask_bytes < MASK_FILE_BUF_SIZE) {
				if ((index > 0) && (count_mask_bytes == 4)) {
					*(int *)(mask_buf + count_mask_bytes) =
									token_list[index];
					count_mask_bytes = 8;
				}
				if (i > 3 && !flag) {
					/* Calculate the payload */
					payload = *(uint16 *)(mask_buf + count_mask_bytes - 2);
					flag = 1;
				}

				mask_buf[count_mask_bytes] = ch;
				if (mask_buf[count_mask_bytes] != CONTROL_CHAR && i == 0) {
					DIAG_LOGE("Sorry, the mask file doesn't adhere to framing definition,"
						"please check the mask file again\n");
					return -1;
				}
				if (count_mask_bytes > payload && mask_buf[count_mask_bytes] == CONTROL_CHAR && i != 0) {
#ifdef DIAG_DEBUG
				DIAG_LOGE("********************************** \n");
				for (i = 0; i <= count_mask_bytes; i++) {
					DIAG_LOGE("%2x ", mask_buf[i]);
				}
				DIAG_LOGE("********************************** \n");
#endif
					i = 0;
					flag = 0;
					diag_send_data(mask_buf, count_mask_bytes+1);
					*(int *)mask_buf = USER_SPACE_DATA_TYPE;
					count_mask_bytes = 4;
				} else {
					i++;
					count_mask_bytes++;
				}
			}
		}
	}

	fclose(read_mask_fp);

	return 0;
}

int diag_read_mask_file_proc(int proc_type)
{
	return diag_set_masks(proc_type, NULL);
}

int diag_read_mask_file_list(char *mask_list_file)
{
	FILE *list_fp;
	char line[FILE_NAME_LEN+8];
	char *end_ptr;
	char *file_name;
	long val;
	int num_files = 0;
	int print_help = 0;
	uint16 remote_mask = 0;
	uint16 mask;
	int val_is_valid;
	int status;

	DIAG_LOGE("Mask list file name is: %s\n", mask_list_file);
	list_fp = fopen(mask_list_file, "rb");
	if (list_fp == NULL) {
		DIAG_LOGE("Sorry, can't open mask list file,"
			"please check the device, errno: %d\n", errno);
		return 0;
	}

	diag_has_remote_device(&remote_mask);
	while (fgets(line, (FILE_NAME_LEN+8), list_fp) != NULL) {
		errno = 0;
		val_is_valid = 0;
		/* Discard if the line is a comment */
		if (line[0] == ';')
			continue;
		val = strtol(line, &end_ptr, 0);
		if (((errno == ERANGE) && (val == LONG_MAX || val == LONG_MIN)) ||
			(errno != 0 && val == 0)) {
				DIAG_LOGE("Skipping line. Invalid processor type found. line: %s\n", line);
				print_help = 1;
				continue;
		} else if (end_ptr == line) {
			print_help = 1;
			DIAG_LOGE("Skipping line. No processor type present. line: %s\n", line);
		}

		if (remote_mask) {
			if ((val > 0) && (val < NUM_PROC - 1)) {
				mask = 1 << (val - 1);
				if ((mask & remote_mask) && (diag_device_mask & (1 << val))) {
					val_is_valid = 1;
				} else {
					DIAG_LOGE("Skipping line. Remote processor: %ld is not present.\n", val);
					continue;
				}
			} else if (val == 0){
				val_is_valid = 1;
			} else {
				DIAG_LOGE("Skipping line. Invalid processor type: %ld specified. line; %s\n", val, line);
				print_help = 1;
				continue;
			}
		} else {
			if (val != 0) {
				DIAG_LOGE("Skipping line. No remote processors present. proc_type: %ld, line: %s\n", val, line);
				continue;
			}
		}
		/*
		 * Determine the name of the mask file. We are counting on the
		 * call to fopen in read_mask_file to do file name validation.
		 */
		file_name = end_ptr;
		while (*file_name != 0) {
			/* Find the first non-blank */
			if (*file_name != ' ')
				break;
			file_name++;
		}
		end_ptr = file_name;
		while (*end_ptr != 0) {
			if (*end_ptr == ';' || *end_ptr == ' ' ||
				isprint(*end_ptr) == 0) {
				*end_ptr = 0;
				break;
			}
			end_ptr++;
		}

		if (file_name == end_ptr) {
			DIAG_LOGE("Skipping line. No file name found. line: %s\n", line);
			print_help = 1;
			continue;
		}
		proc_type = (int)val;
		strlcpy(mask_file_proc[proc_type], file_name, FILE_NAME_LEN);
		DIAG_LOGE("Mask list read for proc_type: %d, mask file: %s\n",
				proc_type, mask_file_proc[proc_type]);
		status = diag_read_mask_file_proc(proc_type);
		if (status != 0) {
			DIAG_LOGE("Error reading mask file: %s\n", mask_file_proc[proc_type]);
		} else {
			num_files++;
		}
	}

	fclose(list_fp);

	DIAG_LOGE("Reading list of mask files complete. Successfully read %d files\n", num_files);
	if (print_help) {
		DIAG_LOGE("File format: proc_type full_path_to_config_file\n");
		DIAG_LOGE("Supported proc_types:\n");
		DIAG_LOGE("0 - MSM\n");
		DIAG_LOGE("Additional proc_types only valid for devices with remote processors\n");
		DIAG_LOGE("1 - MDM\n");
		DIAG_LOGE("2 - MDM2\n");
		DIAG_LOGE("3 - MDM3\n");
		DIAG_LOGE("4 - MDM4\n");
		DIAG_LOGE("5 - QSC (SMUX)\n");
	}

	return num_files;
}

void create_qshrink_thread(void)
{
	if (db_thread_initialized)
		return;
	if (logging_mode == MEMORY_DEVICE_MODE) {
		if (!create_diag_qshrink4_db_parser_thread(diag_peripheral_mask, diag_device_mask)) {
			if (diag_is_wakelock_init()) {
				diag_wakelock_release();
				diag_wakelock_destroy();
			}
			exit(-1);
		}
	}
}

static int diag_reinit_switch_logging(void)
{
	struct diag_logging_mode_param_t params;
	struct diag_con_all_param_t cons;
	int ret;

	if (logging_mode == USB_MODE)
		return 0;

	params.req_mode = logging_mode;
	params.mode_param = DIAG_MD_PERIPHERAL;

	params.peripheral_mask = (diag_peripheral_mask) ?
			diag_peripheral_mask : all_con_params.diag_con_all;
	params.pd_mask = diag_pd_mask;
	params.diag_id = 0;
	params.pd_val = 0;
	params.peripheral = -EINVAL;
	if (diag_device_mask) {
		params.device_mask = diag_device_mask;
	} else {
		params.device_mask = (local_remote_mask << 1) | DIAG_MSM_MASK;
	}

	ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_SWITCH_LOGGING, &params,
				  sizeof(struct diag_logging_mode_param_t));
	if (ret) {
		DIAG_LOGE("diag: reinit switch logging mode to %d, err: %d, errno: %d\n",
			  logging_mode, ret, errno);
		return -1;
	}

	return 0;
}

static int __diag_switch_logging(struct diag_logging_mode_param_t *params, char *dir_location_msm)
{
	int z;
	int err = 0;
	int mode = 0;

	if (!params)
		return -1;

	mode = params->req_mode;
	if (mode == logging_mode) {
		DIAG_LOGE("diag: no actual logging switch required\n");
		return 0;
	}

	/*
	 * If previously logging mode is MEMORY DEVICE then flush
	 * the buffer as we are switching to a different
	 * logging mode.
	 */

	if(log_to_memory) {
		flush_buffer(0);
		log_to_memory = 0;
	}

	if(logging_mode == MEMORY_DEVICE_MODE) {
		for (z = 0; z < NUM_PROC; z++) {
			if (buffer_init[z] == READY) {
				close_logging_file(z);
				count_written_bytes[z] = 0;
				pools[0].buffer_ptr[z] = pool0_buffers[z];
				pools[1].buffer_ptr[z] = pool1_buffers[z];
			}
		}
	}

	if(mode == MEMORY_DEVICE_MODE) {
		fd_dev = -1;
		peripheral_name[0] = '\0';
		if (diag_peripheral_mask)
			diag_get_peripheral_name_from_mask(peripheral_name,
							FILE_NAME_LEN,
							diag_peripheral_mask);
		if (diag_pd_mask)
			diag_get_pd_name_from_mask(upd_name,
							FILE_NAME_LEN,
							diag_pd_mask);
		pthread_create( &disk_write_hdl, NULL, WriteToDisk, NULL);
		if (disk_write_hdl == 0) {
			DIAG_LOGE("Failed to create write thread");
			DIAG_LOGE(" Exiting...........\n");
			if (diag_is_wakelock_init()) {
				diag_wakelock_release();
				diag_wakelock_destroy();
			}
			exit(-1);
		}
		log_to_memory = 1;
	} else if(mode == UART_MODE) {
		fd_dev = fd_uart;
		if (dir_location_msm)
			uart_logging_proc = *(int *)dir_location_msm;
	} else if (mode == SOCKET_MODE) {
		if (params->device_mask & DIAG_MSM_MASK) {
			fd_dev = fd_socket[MSM];
		}
		else if (params->device_mask & DIAG_MDM_MASK) {
			fd_dev = fd_socket[MDM];
		}
		else if (params->device_mask & DIAG_MDM2_MASK) {
			fd_dev = fd_socket[MDM_2];
		}
	} else if (mode == CALLBACK_MODE) {
		/* make sure callback function is registered */
		for (z = 0; z < NUM_PROC; z++) {
			if (cb_clients[z].inited && !(cb_clients[z].cb_func_ptr)) {
				DIAG_LOGE("diag: callback function not registered for proc %d\n", z);
				DIAG_LOGE("diag: unable to change logging mode \n");
				return -1;
			}
		}
	}

	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_SWITCH_LOGGING, params, sizeof(struct diag_logging_mode_param_t));
	if (err) {
		DIAG_LOGE("diag: unable to switch logging mode to %d, err: %d, errno: %d\n",
			  mode, err, errno);
		return -1;
	} else {
		if (logging_mode == MEMORY_DEVICE_MODE) {
			DIAG_LOGE("diag: Sending signal to thread\n");
			pthread_mutex_lock(&stop_mutex);
			pthread_cond_signal(&stop_cond);
			pthread_mutex_unlock(&stop_mutex);
		}
		logging_mode = mode;

		DIAG_LOGE(" logging switched \n");

		if (!diagid_guid_mapping) {
			qsr_state = QSR4_INIT;
			create_qshrink_thread();
			qsr_state = QSR4_THREAD_CREATE;
			create_qshrink_thread();
		}

		if (dir_location_msm &&
		    (mode == MEMORY_DEVICE_MODE)) {
			strlcpy(output_dir[MSM], dir_location_msm,FILE_NAME_LEN);
			DIAG_LOGE("Output dirs %s --- %s\n", output_dir[MSM], output_dir[MDM]);
		}
	}
	return 0;
}

int diag_switch_logging_proc(struct diag_logging_mode_param_t *params)
{
	return __diag_switch_logging(params, output_dir[MSM]);
}

/*===========================================================================
FUNCTION   diag_switch_logging

DESCRIPTION
  This switches the logging mode from default USB to memory device logging

DEPENDENCIES
  valid data type to be passed in:
  In case of ODL second argument is to specify the directory location for logs  In case of UART  logging second argument is to specify PROC type.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diag_switch_logging(int requested_mode, char *dir_location_msm)
{
	int err, success, dev_idx, device_mask = DIAG_MSM_MASK;
	uint16 remote_mask = 0;
	struct diag_logging_mode_param_t params;
	struct diag_con_all_param_t params_con;

	params_con.diag_con_all = DIAG_CON_ALL;
	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_QUERY_CON_ALL, &params_con, sizeof(struct diag_con_all_param_t));
	if (err) {
		DIAG_LOGD("diag: %s: Querying peripheral info from kernel unsupported, Using default stats, err: %d, errno: %d\n",
			 __func__, err, errno);
		params_con.diag_con_all = DIAG_CON_ALL ^ DIAG_CON_LEGACY_UNSUPPORTED;
	} else {
		DIAG_LOGD("diag: %s: kernel supported: NUM_PERIPHERALS = %d, DIAG_CON_ALL: %d\n",
			__func__, params_con.num_peripherals, params_con.diag_con_all);
	}

	params.req_mode = requested_mode;
	params.mode_param = DIAG_MD_PERIPHERAL;
	params.peripheral_mask = params_con.diag_con_all;
	params.pd_mask = 0;
	params.diag_id = 0;
	params.pd_val = 0;
	params.peripheral = -EINVAL;
	success = diag_has_remote_device(&remote_mask);
	if (success == 1) {
		/* remote mask is shifted to account for MSM */
		device_mask = (remote_mask << 1) | DIAG_MSM_MASK;
	}
	params.device_mask = device_mask;
	__diag_switch_logging(&params, dir_location_msm);
}

void diag_set_override_pid(int pid)
{
	int err;

	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_SET_OVERRIDE_PID, &pid, sizeof(pid));
	if (err)
		DIAG_LOGE("diag: %s: Unable to set override pid err: %d\n", __func__, err);
}

static int diag_reinit_callbacks(void)
{
	struct diag_callback_reg_t reg = { 0 };
	int ret;
	int i;

	for (i = 0; i < NUM_PROC; i++) {
		if (!cb_clients[i].inited)
			continue;
		reg.proc = i;
		ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_REGISTER_CALLBACK, &reg, sizeof(reg));
		if (ret) {
			DIAG_LOGE("diag: %s: failed:%d\n", __func__, ret);
			return -1;
		}
	}
	return 0;
}

void diag_register_callback(int (*client_cb_func_ptr)(unsigned char *ptr, int len, void *context_data), void *context_data)
{
	int err = 0;
	struct diag_callback_reg_t reg;

	if (!client_cb_func_ptr) {
		DIAG_LOGE("diag: Unable to register callback\n");
		return;
	}

	cb_clients[MSM].inited = 1;
	cb_clients[MSM].cb_func_ptr = client_cb_func_ptr;
	cb_clients[MSM].context_data = context_data;
	reg.proc = MSM;

	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_REGISTER_CALLBACK, &reg, sizeof(reg));
	if (err)
		DIAG_LOGE("diag: In %s, Unable to register with the driver, err: %d\n", __func__, err);
}

void diag_register_remote_callback(int (*client_rmt_cb_func_ptr)(unsigned char *ptr, int len, void *context_data), int proc, void *context_data)
{
	int err = 0;
	uint16 remote_proc = 0;
	struct diag_callback_reg_t reg;

	if (!client_rmt_cb_func_ptr) {
		DIAG_LOGE("diag: Unable to register callback\n");
		return;
	}
	if (proc <= 0 || proc >= NUM_PROC) {
		DIAG_LOGE("diag: Invalid processor ID\n");
		return;
	}
	diag_has_remote_device(&remote_proc);
	if ((remote_proc & proc) != proc) {
		DIAG_LOGE("diag: Cannot register callback. Processor not supported, requested: %d\n", proc);
		return;
	}
	cb_clients[proc].inited = 1;
	cb_clients[proc].cb_func_ptr = client_rmt_cb_func_ptr;
	cb_clients[proc].context_data = context_data;
	reg.proc = proc;

	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_REGISTER_CALLBACK, &reg, sizeof(reg));
	if (err)
		DIAG_LOGE("diag: In %s, Unable to register with the driver, err: %d\n", __func__, err);
}

void send_mask_modem(unsigned char mask_buf[], int count_mask_bytes)
{
	diag_send_data(mask_buf, count_mask_bytes);
}

int diag_send_data(unsigned char buf[], int bytes)
{
	int bytes_written = 0;
	errno = 0;
	if (diag_fd == DIAG_INVALID_HANDLE)
		return -EINVAL;

	bytes_written = diag_lsm_comm_write(diag_fd, buf, bytes, 0);
	if (*(int *)buf == DCI_DATA_TYPE) {
		if (bytes_written != DIAG_DCI_NO_ERROR) {
			DIAG_LOGE(" DCI send data failed, bytes written: %d, error: %d\n", bytes_written, errno);
			return DIAG_DCI_SEND_DATA_FAIL;
		}
	} else if (bytes_written != 0)
		DIAG_LOGE(" Send data failed, bytes written: %d, error: %d\n", bytes_written, errno);

	return bytes_written;
}

static int diag_callback_send_data_int(int type, int proc, unsigned char *buf, int len)
{
	int offset = CALLBACK_TYPE_LEN;
	uint16 remote_proc = 0;
	unsigned char data_buf[len + CALLBACK_TYPE_LEN + CALLBACK_PROC_TYPE_LEN];

	if (proc < MSM || proc >= NUM_PROC) {
		DIAG_LOGE("diag: Invalid processor ID\n");
		return -1;
	}
	if (len <= 0) {
		DIAG_LOGE("diag: Invalid length %d in %s", len, __func__);
		return -1;
	}
	if (!buf) {
		DIAG_LOGE("diag: Invalid buffer in %s", __func__);
		return -1;
	}
	switch (type) {
	case USER_SPACE_RAW_DATA_TYPE:
	case USER_SPACE_DATA_TYPE:
		break;
	default:
		DIAG_LOGE("diag: Invalid type %d in %s\n", type, __func__);
		return -1;
	}

	*(int *)data_buf = type;
	if (proc != MSM) {
		diag_has_remote_device(&remote_proc);
		if ((remote_proc & proc) != proc) {
			DIAG_LOGE("diag: Processor not supported, requested: %d\n", proc);
			return -1;
		}
		*(int *)(data_buf + offset) = -proc;
		offset += CALLBACK_PROC_TYPE_LEN;
	}
	memcpy(data_buf + offset, buf, len);
	return diag_send_data(data_buf, len + offset);
}

int diag_callback_send_data(int proc, unsigned char * buf, int len)
{
	return diag_callback_send_data_int(USER_SPACE_RAW_DATA_TYPE, proc, buf, len);
}

int diag_callback_send_data_hdlc(int proc, unsigned char * buf, int len)
{
	return diag_callback_send_data_int(USER_SPACE_DATA_TYPE, proc, buf, len);
}

int diag_vote_md_real_time(int real_time)
{
	return diag_vote_md_real_time_proc(MSM, real_time);
}

int diag_vote_md_real_time_proc(int proc, int real_time)
{
	int ret = -1;
	struct real_time_vote_t vote;

	if (proc < MSM || proc > MDM) {
		DIAG_LOGE("diag: invalid proc %d in %s\n", proc, __func__);
		return -1;
	}

	if (!(real_time == MODE_REALTIME || real_time == MODE_NONREALTIME)) {
		DIAG_LOGE("diag: invalid mode change request\n");
		return -1;
	}

	vote.client_id = proc;
	vote.proc = DIAG_PROC_MEMORY_DEVICE;
	vote.real_time_vote = real_time;
	ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_VOTE_REAL_TIME, &vote, sizeof(vote));
	return ret;
}

int diag_get_real_time_status(int *real_time)
{
	return diag_get_real_time_status_proc(MSM, real_time);
}

int diag_get_real_time_status_proc(int proc, int *real_time)
{
	int err = 0;
	struct real_time_query_t query;

	if (!real_time) {
		DIAG_LOGE("diag: invalid pointer in %s\n", __func__);
		return -1;
	}
	if (proc < MSM || proc > MDM) {
		DIAG_LOGE("diag: invalid proc %d in %s\n", proc, __func__);
		return -1;
	}
	query.proc = proc;
	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_GET_REAL_TIME, &query, sizeof(query));
	if (err != 0) {
		DIAG_LOGE(" diag: error in getting real time status, proc: %d, err: %d, error: %d\n", proc, err, errno);
		return -1;
	}

	*real_time = query.real_time;
	return 0;
}

static void diag_callback_init(void)
{
	int i;

	for (i = 0; i < NUM_PROC; i++) {
		cb_clients[i].inited = 0;
		cb_clients[i].cb_func_ptr = NULL;
		cb_clients[i].context_data = NULL;
	}
}

int hw_accel_operation_handler(diag_hw_accel_cmd_req_t *pReq,
	diag_hw_accel_query_sub_payload_rsp_t *query_rsp, uint8 operation)
{
	int err = 0;
	int transport_mode = 0;

	if (operation == DIAG_HW_ACCEL_OP_QUERY) {
		err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_QUERY_PD_FEATUREMASK, query_rsp,
			sizeof(diag_hw_accel_query_sub_payload_rsp_t));
		if (err) {
			DIAG_LOGE("diag: Error in querying PD featuremask, err: %d, errno: %d\n",err, errno);
			return -1;
		}
	} else if (operation == DIAG_TRANSPORT_MODE_QUERY) {
		err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_GET_TRANSPORT_MODE, &transport_mode, 0);
		if (err) {
			DIAG_LOGE("diag: Error while requesting transport mode, err: %d, errno: %d\n",err, errno);
			return -1;
		}
		return transport_mode;
	} else {
		err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_PASSTHRU_CONTROL, pReq,
			sizeof(diag_hw_accel_cmd_req_t));
		if (err) {
			DIAG_LOGE("diag:Error in Sending Passthru contorl packet, err: %d, errno: %d\n",
				 err, errno);
			return -1;
		}
	}
	return 0;
}

#define DIAG_SEC_TO_USEC(x) ((x) * 1000000ULL)
#define DIAG_USEC_FILL_TIMEVAL(timeval, x) timeval.tv_sec = ((x) / 1000000ULL);  \
					 timeval.tv_usec = ((x) % 1000000ULL);

static int diag_timed_retry(int (*diag_client_reset_fn)(void), unsigned long long timeout_us,
			    unsigned long long retry_wait_us)
{
	struct timeval start_tv;
	struct timeval current_tv;
	struct timeval wait_tv = { 0 };
	unsigned long long start_time_us;
	unsigned long long current_time_us;
	unsigned long long elapsed_time_us = 0;
	int ret = 0;

	gettimeofday(&start_tv, NULL);
	start_time_us = DIAG_SEC_TO_USEC(start_tv.tv_sec) + start_tv.tv_usec;
	while (elapsed_time_us < timeout_us) {
		DIAG_USEC_FILL_TIMEVAL(wait_tv, retry_wait_us);

		ret = diag_client_reset_fn();
		if (!ret)
			break;

		if (kill_restart) {
			DIAG_LOGD("diag: %s: Exiting...., kill_restart: %d\n", __func__, kill_restart);
			break;
		}
		select(0, NULL, NULL, NULL, &wait_tv);
		gettimeofday(&current_tv, NULL);
		current_time_us = DIAG_SEC_TO_USEC(current_tv.tv_sec) + current_tv.tv_usec;

		/* In the case where time has wrapped around, skip any further commit attempt */
		if (current_time_us < start_time_us)
			break;
		elapsed_time_us = current_time_us - start_time_us;
	}
	return ret;
}

static int diag_reset_diag_fd(void)
{
	int fd;

	fd = diag_lsm_comm_open();
	if (fd < 0) {
		DIAG_LOGE("diag: %s: open failed: error:%d\n", __func__, errno);
		if (kill_restart) {
			DIAG_LOGD("diag: Exiting from %s, as Deinit is initiated, kill_restart:%d\n", __func__, kill_restart);
			return 0;
		}
		return -1;
	}
	diag_fd = fd;
	DIAG_LOGD("diag: %s: diag_fd successfully reset fd:%d\n", __func__, diag_fd);

	return 0;
}

static int diag_reset_connection(void)
{
	struct timeval wait_tv = { 0 };
	char buf[32];
	int err = 0;
	int ret;

	ret = recv(diag_fd, buf, sizeof(buf), MSG_PEEK | MSG_DONTWAIT);
	/* Socket is still valid if return is:
	 *   - greater than 0 (valid data to read)
	 *   - error is returned with EWOULDBLOCK
	 * If socket is still valid, return without trying to reset the socket
	 */
	if (ret > 0)
		return 0;
	if (ret < 0 && errno == EWOULDBLOCK)
		return 0;
	DIAG_LOGD("diag: %s: reset necessary test recv ret:%d errno:%d\n", __func__, ret, errno);

	/* Do some clean up to prevent some diag_fd access races:
	 *   - Clear masks to prevent extra logging
	 *   - close diag_fd
	 *   - wait 1 second to give diag-router time to restart
	 */
	msg_clear_mask();
	log_clear_mask();
	log_clear_dci_mask();
	event_clear_mask();
	event_clear_dci_mask();
	clear_mask_sync();

	if (diag_fd > 0)
		close(diag_fd);
	diag_fd = DIAG_INVALID_HANDLE;
	wait_tv.tv_sec = 1;
	select(0, NULL, NULL, NULL, &wait_tv);

	if (kill_restart) {
		DIAG_LOGD("diag: %s: Exiting...., kill_restart: %d\n", __func__, kill_restart);
		return 0;
	}

	ret = diag_timed_retry(&diag_reset_diag_fd, DIAG_SEC_TO_USEC(10),
			       DIAG_SEC_TO_USEC(1));
	if (diag_fd == DIAG_INVALID_HANDLE) {
		DIAG_LOGE("diag: %s: failed to reconnect to diag-router\n", __func__);
		return -1;
	}

	if (!do_mask_sync()) {
		DIAG_LOGE("diag: %s: failed to resync masks after reconnect\n", __func__);
		return -1;
	}

	ret = diagpkt_reinit();
	if (ret) {
		DIAG_LOGE("diag: %s: failed to reinit diagpkt\n", __func__);
		return -1;
	}

	ret = diag_reinit_callbacks();
	if (ret) {
		DIAG_LOGE("diag: %s: failed to reinit callbacks\n", __func__);
		return -1;
	}

	ret = diag_lsm_dci_reinit();
	if (ret) {
		DIAG_LOGE("diag: %s: failed to reinit dci\n", __func__);
		return -1;
	}

	ret = diag_reinit_switch_logging();
	if (ret) {
		DIAG_LOGE("diag: %s: failed to reinit switch logging\n", __func__);
		return -1;
	}

	if (logging_mode == MEMORY_DEVICE_MODE) {
		diag_reconfigure_masks(MSM);
		diag_reconfigure_qdss();
	}

	return 0;
}

static void diag_signal_try_restart(void)
{
	pthread_mutex_lock(&restart_mutex);
	try_restart++;
	pthread_cond_signal(&restart_cond);
	pthread_mutex_unlock(&restart_mutex);
}

static void diag_signal_kill_restart(void)
{
	pthread_mutex_lock(&restart_mutex);
	kill_restart++;
	pthread_cond_signal(&restart_cond);
	pthread_mutex_unlock(&restart_mutex);
}

static void *diag_restart_thread(void *param)
{
	while (1) {
		pthread_mutex_lock(&restart_mutex);
		while (!try_restart && !kill_restart) {
			pthread_cond_wait(&restart_cond, &restart_mutex);
		}
		if (kill_restart) {
			pthread_mutex_unlock(&restart_mutex);
			break;
		}
		pthread_mutex_unlock(&restart_mutex);

		if (!diag_reset_connection()) {
			pthread_mutex_lock(&restart_mutex);
			try_restart = 0;
			pthread_mutex_unlock(&restart_mutex);
		}
	}
	pthread_mutex_lock(&deinit_mutex);
	pthread_cond_signal(&deinit_cond);
	restart_thread_exit |= STATUS_RST_THRD_EXIT;
	DIAG_LOGE("diag: %s: restart_thread_exit: %d, ....Exiting\n",
		  __func__, restart_thread_exit);
	pthread_mutex_unlock(&deinit_mutex);
	pthread_detach(pthread_self());

	return param;
}

static struct timespec calculate_waiting_time(int ms)
{
	struct timespec time;
	struct timeval now;

	gettimeofday(&now, NULL);
	/* We will wait for 200ms here, but if current time's millisecond value is >= 800, then adding 200ms to it
	will increment the current second's value by 1, hence handle these 2 cases of waiting time below separately*/
	if ((now.tv_usec / 1000) < 1000 - ms) {
		time.tv_sec = now.tv_sec;
		time.tv_nsec = (now.tv_usec * 1000) + (ms * 1000 * 1000);
	} else {
		time.tv_sec = now.tv_sec + 1;
		time.tv_nsec = (now.tv_usec * 1000) + (ms * 1000 * 1000) - (1000 * 1000 * 1000);
	}
	return time;
}

static int retry_mask_sync(void)
{
	if (!do_mask_sync())
		return -1;

	return 0;
}

/*===========================================================================
FUNCTION   Diag_LSM_Init

DESCRIPTION
  Initializes the Diag Legacy Mapping Layer. This should be called
  only once per process.

DEPENDENCIES
  Successful initialization requires Diag CS component files to be present
  and accessible in the file system.

RETURN VALUE
  FALSE = failure, else TRUE

SIDE EFFECTS
  None

===========================================================================*/

boolean Diag_LSM_Init (uint8_t* pIEnv)
{
   sigset_t set;
   int ret;
   (void)pIEnv;

   DIAG_LOGE("diag: %s: invoked for pid: %d with init_count: %d\n",
		__func__, getpid(), lsm_init_count);

   pthread_mutex_lock(&lsm_init_count_mutex);
	if(lsm_init_count == 0) {
		// Open a handle to the diag driver
		if (diag_fd == DIAG_INVALID_HANDLE) {
#ifdef FEATURE_LOG_STDOUT
			diag_fd = 0; // Don't need to open anything to use stdout, so fake it.
#else // not FEATURE_LOG_STDOUT
			diag_fd = diag_lsm_comm_open();
#endif // FEATURE_LOG_STDOUT

			if (diag_fd == DIAG_INVALID_HANDLE) {
				if (kill_mdlog_flag)
					diag_timed_retry(&diag_reset_diag_fd, DIAG_SEC_TO_USEC(10),DIAG_SEC_TO_USEC(1));

				if (diag_fd == DIAG_INVALID_HANDLE) {
					DIAG_LOGE(" Diag_LSM_Init: Failed to open handle to diag driver,"
							" error = %d", errno);
					pthread_mutex_unlock(&lsm_init_count_mutex);
					return FALSE;
				}
			}

			pthread_mutex_init(&(pools[0].write_mutex), NULL);
			pthread_cond_init(&(pools[0].write_cond), NULL);
			pthread_mutex_init(&(pools[0].read_mutex), NULL);
			pthread_cond_init(&(pools[0].read_cond), NULL);
			pthread_mutex_init(&(pools[1].write_mutex), NULL);
			pthread_cond_init(&(pools[1].write_cond), NULL);
			pthread_mutex_init(&(pools[1].read_mutex), NULL);
			pthread_cond_init(&(pools[1].read_cond), NULL);
			pthread_mutex_init(&deinit_mutex, NULL);
			pthread_cond_init(&deinit_cond, NULL);
			pthread_mutex_init(&stop_mutex, NULL);
			pthread_cond_init(&stop_cond, NULL);
			pthread_mutex_init(&query_md_pid_mutex, NULL);
			pthread_mutex_init(&query_con_mutex, NULL);
			pthread_cond_init(&query_con_cond, NULL);
			pthread_mutex_init(&query_reg_cb_mutex, NULL);
			pthread_cond_init(&query_reg_cb_cond, NULL);
			pthread_cond_init(&query_md_pid_cond, NULL);
			pthread_mutex_init(&query_remote_mask_mutex, NULL);
			pthread_cond_init(&query_remote_mask_cond, NULL);
			pthread_mutex_init(&query_reg_table_mask_mutex, NULL);
			pthread_cond_init(&query_reg_table_mask_cond, NULL);
			pthread_mutex_init(&query_debug_all_mask_mutex, NULL);
			pthread_cond_init(&query_debug_all_mask_cond, NULL);
			pthread_mutex_init(&query_dci_client_id_mutex, NULL);
			pthread_cond_init(&query_dci_client_id_cond, NULL);
			pthread_mutex_init(&query_dci_support_mutex, NULL);
			pthread_cond_init(&query_dci_support_cond, NULL);
			pthread_mutex_init(&query_dci_health_stats_mutex, NULL);
			pthread_cond_init(&query_dci_health_stats_cond, NULL);
			pthread_mutex_init(&query_dci_log_status_mutex, NULL);
			pthread_cond_init(&query_dci_log_status_cond, NULL);
			pthread_mutex_init(&query_dci_event_status_mutex, NULL);
			pthread_cond_init(&query_dci_event_status_cond, NULL);
			pthread_mutex_init(&query_real_time_mutex, NULL);
			pthread_cond_init(&query_real_time_cond, NULL);
			pthread_mutex_init(&timestamp_switch_mutex, NULL);

			flush_in_progress = 0;

			/* Allocate initial buffer of MSM data only */
#if 0
			/* Since calloc allocation is not working */
			pool0_buffers[0] = calloc(DISK_BUF_SIZE, 1);
			if(!pool0_buffers[0])
			return FALSE;

			pool1_buffers[0] = calloc(DISK_BUF_SIZE, 1);
			if(!pool1_buffers[0]) {
				free(pool0_buffers[0]);
				return FALSE;
			}
#endif
			/* Manually doing the initialisation as dynamic alloc is not working */

			pool0_buffers[0] = static_buffer[0];
			pool0_buffers[1] = static_buffer[2];
			pool0_buffers[2] = static_buffer[4];
			pool1_buffers[0] = static_buffer[1];
			pool1_buffers[1] = static_buffer[3];
			pool1_buffers[2] = static_buffer[5];

			pools[0].bytes_in_buff[0] = pools[0].bytes_in_buff[1] = pools[0].bytes_in_buff[2] = 0;
			pools[1].bytes_in_buff[0] = pools[1].bytes_in_buff[1] = pools[1].bytes_in_buff[2] = 0;
			buffer_init[0] = READY;

			pools[0].buffer_ptr[0] = pool0_buffers[0];
			pools[1].buffer_ptr[0] = pool1_buffers[0];

			pools[0].buffer_ptr[1] = pool0_buffers[1];
			pools[1].buffer_ptr[1] = pool1_buffers[1];

			pools[0].buffer_ptr[2] = pool0_buffers[2];
			pools[1].buffer_ptr[2] = pool1_buffers[2];

			/* Block SIGUSR2 for the main application */
			if ((sigemptyset((sigset_t *) &set) == -1) ||
				(sigaddset(&set, SIGUSR2) == -1))
				DIAG_LOGE("diag: Failed to initialize block set\n");

			ret = sigprocmask(SIG_BLOCK, &set, NULL);
			if (ret != 0)
				DIAG_LOGE("diag: Failed to block signal for main thread\n");

			diag_socket_init();
			gdwClientID = getpid();

			/* Initialize buffers needed for Diag event, log, F3 services */

			if (!DiagSvc_Malloc_Init())
				goto failure_case;

			/* Initialize the services */
			if (!Diag_LSM_Pkt_Init())
				goto failure_case;

			if (!Diag_LSM_Log_Init())
				goto failure_case;

			if (!Diag_LSM_Msg_Init())
				goto failure_case;

			if (!Diag_LSM_Event_Init())
				goto failure_case;

			diag_callback_init();

			diag_qshrink4_init();

			try_restart = 0;
			kill_restart = 0;
			pthread_mutex_init(&restart_mutex, NULL);
			pthread_cond_init(&restart_cond, NULL);
			if (!diag_use_dev_node) {
				restart_thread_exit |= STATUS_RST_THRD_INIT;
				pthread_create(&restart_thread_hdl, NULL, diag_restart_thread, NULL);
				if (!restart_thread_hdl) {
					DIAG_LOGE("diag: %s: Failed to create restart thread", __func__);
					goto failure_case;
				}
			}
			pthread_mutex_init(&read_thread_mutex, NULL);
			read_thread_exit |= STATUS_RDTHRD_INIT;
			/* Creating read thread which listens for various masks & pkt
			* requests */
			pthread_create(&read_thread_hdl, NULL, CreateWaitThread, NULL);
			if (read_thread_hdl == 0) {
				DIAG_LOGE("diag: %s: Failed to create read thread", __func__);
				goto failure_case;
			}
			/* Performe mask synchronization to ensure
			 * we have the masks before proceeding */
			if (!do_mask_sync()) {
				if (kill_mdlog_flag) {
					if (diag_timed_retry(&retry_mask_sync, DIAG_SEC_TO_USEC(10),DIAG_SEC_TO_USEC(1)) != 0)
						goto failure_case;
				} else {
					goto failure_case;
				}
			}
		}
	}
	lsm_init_count++;
	pthread_mutex_unlock(&lsm_init_count_mutex);
	DIAG_LOGE("diag: %s: done for pid: %d with init_count: %d\n",
		__func__, getpid(), lsm_init_count);
	return TRUE;

failure_case:
	lsm_init_count++;
	pthread_mutex_unlock(&lsm_init_count_mutex);
	DIAG_LOGE("diag: %s: failed for pid: %d with init_count: %d\n",
		__func__, getpid(), lsm_init_count);
	Diag_LSM_DeInit();
	return FALSE;
}


/*===========================================================================

FUNCTION    Diag_LSM_DeInit

DESCRIPTION
  De-Initialize the Diag service.

DEPENDENCIES
  None.

RETURN VALUE
  FALSE = failure, else TRUE.
  Currently all the internal boolean return functions called by
  this function just returns TRUE w/o doing anything.

SIDE EFFECTS
  None

===========================================================================*/

boolean Diag_LSM_DeInit(void)
{
	boolean bReturn = TRUE;
	int i, rt = 0;
	struct timespec time;

	DIAG_LOGE("diag: %s: invoked for pid: %d\n", __func__, getpid());
	time = calculate_waiting_time(DIAG_DEINIT_TIMEOUT_IN_MSEC);
	pthread_mutex_lock(&lsm_init_count_mutex);
	if (lsm_init_count > 1) {
		lsm_init_count--;
		pthread_mutex_unlock(&lsm_init_count_mutex);
		return bReturn;
	}

	if (diag_fd != DIAG_INVALID_HANDLE || try_restart) {
		int ret;

		socket_inited = 0;

		/* Free the buffers mallocated for events, logs, messages and packet req/res */
		DiagSvc_Malloc_Exit();

		if (!Diag_LSM_Pkt_DeInit())
			bReturn  = FALSE;

		deinit_rcvd = 0;
		sig_rcvd = 0;
		DIAG_LOGE("diag: %s: Initiating deinit pkt transfer\n", __func__);
		if ((ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_LSM_DEINIT, NULL, 0)) != 1) {
			DIAG_LOGE("diag: %s: DeviceIOControl failed. ret: %d, error: %d\n", __func__, ret, errno);
			bReturn = FALSE;
		}

		if (in_read) {
			DIAG_LOGE("diag: %s: sending signal to unblock read thread\n", __func__);
			pthread_mutex_lock(&read_thread_mutex);
			if (read_thread_hdl)
				pthread_kill(read_thread_hdl, SIGUSR2);
			pthread_mutex_unlock(&read_thread_mutex);
		}

		if (!diag_use_dev_node)
			diag_signal_kill_restart();

		pthread_mutex_lock(&deinit_mutex);
		if (!diag_use_dev_node) {
			time = calculate_waiting_time(DIAG_DEINIT_TIMEOUT_IN_MSEC);
			while (!(restart_thread_exit & STATUS_RST_THRD_EXIT)) {
				rt = pthread_cond_timedwait(&deinit_cond, &deinit_mutex, &time);
				if (rt == ETIMEDOUT){
					DIAG_LOGE("diag: %s: Restart thread exit timeout\n", __func__);
					break;
				}
			}
		}
		pthread_mutex_unlock(&deinit_mutex);

		if (diag_fd > 0) {
			ret = close(diag_fd);
			if (ret < 0) {
				DIAG_LOGE("diag: %s: error closing file, ret: %d, errno: %d\n",
					__func__, ret, errno);
				bReturn = FALSE;
			} else {
				DIAG_LOGE("diag: %s: Closed diag fd for pid: %d\n", __func__, getpid());
			}
		}

		diag_fd = DIAG_INVALID_HANDLE;

		pthread_mutex_lock(&deinit_mutex);
		read_thread_exit |= STATUS_RDTHRD_WAIT;

		DIAG_LOGE("diag: %s: Waiting for read thread, deinit_rcvd: %d, sig_rcvd: %d, read_thread_exit: %d\n",
			  __func__, deinit_rcvd, sig_rcvd, read_thread_exit);

		time = calculate_waiting_time(DIAG_DEINIT_TIMEOUT_IN_MSEC);

		while (!(read_thread_exit & STATUS_RDTHRD_EXIT) || (!deinit_rcvd && !sig_rcvd)) {
			rt = pthread_cond_timedwait(&deinit_cond, &deinit_mutex, &time);
			if (rt == ETIMEDOUT) {
				DIAG_LOGE("diag: %s: Read thread exit timeout\n", __func__);
				break;
			}
		}
		pthread_mutex_unlock(&deinit_mutex);

		deinit_rcvd = 0;
		sig_rcvd = 0;

		for (i = 0; i < NUM_PROC; i++) {
			delete_oldest_file_list(i);
			delete_qdss_oldest_file_list(i);
		}
		circular_logging_inited = 0;

		Diag_LSM_Log_DeInit();

		Diag_LSM_Event_DeInit();

		Diag_LSM_Msg_DeInit();
	
		lsm_init_count = 0;
	}

	if (read_thread_exit & STATUS_RDTHRD_EXIT)
		read_thread_exit = STATUS_RDTHRD_CLR;
	if (restart_thread_exit & STATUS_RST_THRD_EXIT)
		restart_thread_exit = STATUS_RST_THRD_CLR;

	DIAG_LOGE("diag: %s: done for pid: %d\n", __func__, getpid());
	pthread_mutex_unlock(&lsm_init_count_mutex);

	return bReturn;
}     /* Diag_LSM_DeInit */

/* Internal functions */
static void *CreateWaitThread(void* param)
{
	sigset_t set_1;
	int rc, z;
	struct  sigaction sact;
	(void)param;

	sigemptyset( &sact.sa_mask );
	sact.sa_flags = 0;
	sact.sa_handler = dummy_handler;
	sigaction(SIGUSR2, &sact, NULL);

	if ((sigfillset((sigset_t *) &set_1) == -1) ||
		(sigdelset(&set_1, SIGUSR2) == -1))
		DIAG_LOGE("diag: Failed to initialize block set\n");

	rc = pthread_sigmask(SIG_SETMASK, &set_1, NULL);
	if (rc != 0)
		DIAG_LOGE("diag: Failed to unbock signal for read thread\n");

	do{
		if (flush_log) {

			while(write_in_progress) {
				sleep(1);
			}

			DIAG_LOGE(" %s exiting ...[%d]..\n",
					__func__, curr_read);
			pools[curr_read].data_ready = 1;
			pools[curr_read].free = 0;

			for (z = 0; z < NUM_PROC; z++) {
				if (curr_read)
					pools[curr_read].buffer_ptr[z] =
						pool1_buffers[z];
				else
					pools[curr_read].buffer_ptr[z] =
						pool0_buffers[z];
			}

			pthread_mutex_lock(
					&pools[curr_read].write_mutex);
			pthread_mutex_lock(&stop_mutex);
			flush_log++;
			pthread_mutex_unlock(&stop_mutex);

			pthread_cond_signal(
					&pools[curr_read].write_cond);
			pthread_mutex_unlock(
					&pools[curr_read].write_mutex);
			curr_read = !curr_read;
			/* As cleanup started now wait for cleanup to
			 * complete.
			 */
			pthread_mutex_lock(&stop_mutex);
			if (flush_log)
				pthread_cond_wait(&stop_cond,
						&stop_mutex);
			pthread_mutex_unlock(&stop_mutex);
			break;
		} else {
			in_read = 1;
			num_bytes_read = 0;
			memset(read_buffer, 0, READ_BUF_SIZE);
			num_bytes_read = read(diag_fd, (void*)read_buffer,
					READ_BUF_SIZE);

			in_read = 0;
			/* read might return 0 across suspend */
			if (!num_bytes_read) {
				if (diag_use_dev_node) {
					continue;
				} else {
					/* 0 byte read means end of file, close and try to reopen */
					DIAG_LOGE("diag: %s: EOF detected\n", __func__);
					if (read_thread_exit & STATUS_RDTHRD_WAIT)
						break;
					diag_signal_try_restart();
					continue;
				}
			}

			if (*(int *)read_buffer == DEINIT_TYPE) {
				DIAG_LOGE("diag: %s, Read DEINIT_TYPE packet\n", __func__);
				deinit_rcvd = 1;
				break;
			}
			if (num_bytes_read < 0) {
				if (diag_use_dev_node)	{
					break;
				} else {
					if (flush_log) {
						continue;
					} else {
						if (read_thread_exit & STATUS_RDTHRD_WAIT)
							break;
						diag_signal_try_restart();
						continue;
					}
				}
			}

			if (!flush_log)
				process_diag_payload();
		}
	} while(1);

	pthread_mutex_lock(&deinit_mutex);
	pthread_cond_signal(&deinit_cond);
	read_thread_exit |= STATUS_RDTHRD_EXIT;
	pthread_mutex_unlock(&deinit_mutex);

	pthread_detach(pthread_self());

	pthread_mutex_lock(&read_thread_mutex);
	read_thread_hdl = 0;
	pthread_mutex_unlock(&read_thread_mutex);

	DIAG_LOGE("diag: %s, Exiting...\n", __func__);
	return 0;
}

int diag_get_periph_id_from_buf_periph_id(uint8 id)
{
	int ret;
	struct diag_con_all_param_t params_con;

	params_con.upd_map_supported = 0;
	ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_QUERY_CON_ALL, &params_con, sizeof(struct diag_con_all_param_t));
	if (ret || (params_con.upd_map_supported == 0))
		buffering_mode_periph_map_supported = 0;
	else
		buffering_mode_periph_map_supported = 1;

	switch (id) {
	case BUF_DIAG_MODEM_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_MODEM_PROC;
		else
			return BUF_DIAG_MODEM_PROC;
	case BUF_DIAG_LPASS_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_LPASS_PROC;
		else
			return BUF_DIAG_LPASS_PROC;
	case BUF_DIAG_WCNSS_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_WCNSS_PROC;
		else
			return BUF_DIAG_WCNSS_PROC;
	case BUF_DIAG_SENSORS_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_SENSORS_PROC;
		else
			return BUF_DIAG_SENSORS_PROC;
	case BUF_DIAG_WDSP_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_WDSP_PROC;
		else
			return BUF_DIAG_WDSP_PROC;
	case BUF_DIAG_CDSP_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_CDSP_PROC;
		else
			return BUF_DIAG_CDSP_PROC;
	case BUF_DIAG_APPS_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_APPS_PROC;
		else
			return BUF_DIAG_APPS_PROC;
	case BUF_UPD_WLAN:
		if (buffering_mode_periph_map_supported)
			return UPD_WLAN;
		else
			return BUF_UPD_WLAN;
	case BUF_UPD_AUDIO:
		if (buffering_mode_periph_map_supported)
			return UPD_AUDIO;
		else
			return BUF_UPD_AUDIO;
	case BUF_UPD_SENSORS:
		if (buffering_mode_periph_map_supported)
			return UPD_SENSORS;
		else
			return BUF_UPD_SENSORS;
	case BUF_UPD_CHARGER:
		if (buffering_mode_periph_map_supported)
			return UPD_CHARGER;
		else
			return BUF_UPD_CHARGER;
	case BUF_DIAG_NPU_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_NPU_PROC;
		else
			return BUF_DIAG_NPU_PROC;
	case BUF_UPD_OEM:
		if (buffering_mode_periph_map_supported)
			return UPD_OEM;
		else
			return BUF_UPD_OEM;
	case BUF_UPD_OIS:
		if (buffering_mode_periph_map_supported)
			return UPD_OIS;
		else
			return BUF_UPD_OIS;
	case BUF_DIAG_NSP1_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_NSP1_PROC;
		else
			return BUF_DIAG_NSP1_PROC;
	case BUF_DIAG_GPDSP0_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_GPDSP0_PROC;
		else
			return BUF_DIAG_GPDSP0_PROC;
	case BUF_DIAG_GPDSP1_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_GPDSP1_PROC;
		else
			return BUF_DIAG_GPDSP1_PROC;
	case BUF_DIAG_HELIOS_M55_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_HELIOS_M55_PROC;
		else
			return BUF_DIAG_HELIOS_M55_PROC;
	case BUF_DIAG_SLATE_APPS_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_SLATE_APPS_PROC;
		else
			return BUF_DIAG_SLATE_APPS_PROC;
	case BUF_DIAG_SLATE_ADSP_PROC:
		if (buffering_mode_periph_map_supported)
			return DIAG_SLATE_ADSP_PROC;
		else
			return BUF_DIAG_SLATE_ADSP_PROC;
	default:
		return -1;
	}
}

int diag_configure_peripheral_buffering_tx_mode(uint8 peripheral_id, uint8 tx_mode,
						uint8 low_wm_val, uint8 high_wm_val)
{
	int err = 0;
	struct diag_periph_buffering_tx_mode params;
	uint8 peripheral;

	switch (tx_mode) {
	case DIAG_STREAMING_MODE:
	case DIAG_CIRCULAR_BUFFERING_MODE:
	case DIAG_THRESHOLD_BUFFERING_MODE:
		break;
	default:
		DIAG_LOGE("diag: In %s, invalid tx mode requested %d\n", __func__, tx_mode);
		return -EINVAL;
	}
	peripheral = diag_get_periph_id_from_buf_periph_id(peripheral_id);
	if (peripheral >= 0) {
		DIAG_LOGE("diag: In %s, request for buffer configuring pd %d\n", __func__, peripheral);
	} else {
		DIAG_LOGE("diag: In %s, invalid peripheral %d\n", __func__, peripheral);
		return -EINVAL;
	}

	if ((((high_wm_val > 100) || (low_wm_val  > 100)) || (low_wm_val  > high_wm_val)) ||
	    ((low_wm_val == high_wm_val) && ((low_wm_val != 0) && (high_wm_val != 0)))) {
		DIAG_LOGE("diag: In %s, invalid watermark values, low: %d, high: %d\n",
			  __func__, low_wm_val, high_wm_val);
		return -EINVAL;
	}

	params.peripheral = peripheral;
	params.mode = tx_mode;
	params.low_wm_val = low_wm_val;
	params.high_wm_val = high_wm_val;

	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_CONFIG_BUFFERING_TX_MODE, &params, sizeof(params));
	if (err) {
		DIAG_LOGE("diag: In %s, unable to set peripheral buffering mode, ret: %d, error: %d\n",
			  __func__, err, errno);
		return err;
	}

	return 1;
}

int diag_peripheral_buffering_drain_immediate(uint8 peripheral)
{
	int err = 0;

	if (peripheral >= NUM_PERIPHERALS) {
		if (peripheral <= (NUM_PERIPHERALS + NUM_UPD)) {
			DIAG_LOGE("diag_buffering_test: PD buffer configuration requested: %d\n", peripheral);
		} else {
			DIAG_LOGE("diag_buffering_test: Invalid peripheral: %d\n", peripheral);
			return -EINVAL;
		}
	}

	peripheral = diag_get_periph_id_from_buf_periph_id(peripheral);
	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_BUFFERING_DRAIN_IMMEDIATE, &peripheral,
		    sizeof(peripheral));
	if (err) {
		DIAG_LOGE("diag: In %s, unable to send ioctl to drain immediate, ret: %d, error: %d\n",
			  __func__, err, errno);
		return err;
	}

	return 1;
}

int diag_hdlc_toggle(uint8 hdlc_support)
{
	int err = 0;
	errno = 0;

	if (hdlc_support > 1) {
		DIAG_LOGE("diag: In %s, invalid request %d\n", __func__, hdlc_support);
		return -EINVAL;
	}

	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_HDLC_TOGGLE, &hdlc_support,
		sizeof(hdlc_support));
	if (err) {
		DIAG_LOGE("diag: In %s, unable to send ioctl to change hdlc support, ret: %d, error: %d\n",
			__func__, err, errno);
		return err;
	}

	pthread_mutex_lock(&hdlc_toggle_mutex);
	hdlc_disabled[MSM] = hdlc_support;
	pthread_mutex_unlock(&hdlc_toggle_mutex);

	return 1;
}

int diag_hdlc_toggle_mdm(uint8 hdlc_support, int proc)
{
	int err = 0;
	errno = 0;

	if (hdlc_support > 1) {
		DIAG_LOGE("diag: In %s, invalid request %d\n", __func__, hdlc_support);
		return -EINVAL;
	}

	err = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_MDM_HDLC_TOGGLE, &hdlc_support,
		sizeof(hdlc_support));
	if (err) {
		DIAG_LOGE("diag: In %s, unable to send ioctl to change hdlc support, ret: %d, error: %d\n",
			__func__, err, errno);
		return err;
	}

	pthread_mutex_lock(&hdlc_toggle_mutex);
	hdlc_disabled[proc] = hdlc_support;
	pthread_mutex_unlock(&hdlc_toggle_mutex);

	return 1;
}

void diag_reset_guid_count(int p_type, int peripheral)
{
	int count = 0;
	count = guid_count[p_type][peripheral];
	guid_count[p_type][peripheral] = 0;
	diagid_entry_count[p_type] -= count;
}

int add_guid_to_qshrink4_header(unsigned char * guid, int p_type, int peripheral)
{
	diag_id_list *item = NULL;
	qshrink4_header *qshrink4_data_ptr;

	qshrink4_data_ptr = &qshrink4_data[p_type];

	memcpy(&qshrink4_data_ptr->guid[qshrink4_data_ptr->guid_list_entry_count],
		guid, GUID_LEN);

	if (use_qmdl2_v2) {

		item = get_diag_id(p_type, peripheral);
		if (item) {
			diagid_guid[p_type][qshrink4_data_ptr->guid_list_entry_count].diag_id =
				item->diag_id;

			strlcpy((char *)&diagid_guid[p_type][qshrink4_data_ptr->guid_list_entry_count].process_name,
				item->process_name, MAX_DIAGID_STR_LEN);
		}
		memcpy(&diagid_guid[p_type][qshrink4_data_ptr->guid_list_entry_count].guid,
			guid, GUID_LEN);

		diagid_guid_status[p_type] = 1;
		signal_writing_qdss_header(p_type);
		DIAG_LOGE("diag: %s: diagid guid mapping is done (diagid_guid_status[%d]: %d)\n",
			__func__, p_type, diagid_guid_status[p_type]);

		if (!guid_count[p_type][peripheral]) {
			guid_count[p_type][peripheral]++;
			diagid_entry_count[p_type]++;
		}
	}
	qshrink4_data_ptr->header_length += GUID_LEN;
	qshrink4_data_ptr->guid_list_entry_count++;
	return 0;
}
void diag_set_peripheral_mask(unsigned int peripheral_mask)
{
	diag_peripheral_mask = peripheral_mask;
	if (!(peripheral_mask & DIAG_CON_MPSS) && !(peripheral_mask & DIAG_CON_WCNSS))
		diagid_guid_status[MSM] = 1;
}
void diag_set_upd_mask(unsigned int pd_mask)
{
	diag_pd_mask = pd_mask;
	diagid_guid_status[MSM] = 1;
}
void diag_set_device_mask(unsigned int device_mask)
{
	diag_device_mask = device_mask;
}
unsigned int diag_get_upd_mask(void)
{
	return diag_pd_mask;
}

int read_mask_file_default(int proc_index)
{
	int status = 0;
	if (proc_index < 0 || proc_index >=NUM_PROC) {
		status = -ENODEV;
		return status;
	}

	proc_type = proc_index;
	DIAG_LOGE("diag_mdlog: Default mask file being read for proc_type: %d\n",
									proc_type);
	strlcpy(mask_file_proc[proc_type], default_mask_file[proc_type], FILE_NAME_LEN);

	status = diag_read_mask_file_proc(proc_type);

	return status;
}

int diag_reconfigure_masks(int proc_type)
{
	int status = 0;
	int num_mask_reads_succeeded = 0;

	/* Read mask file to tell On Device Logging what you are interested in */
	if (mask_file_list_entered) {
		status = diag_read_mask_file_list(mask_file_list);
		if (!status) {
			DIAG_LOGE("diag_mdlog: Error reading mask file list. Exiting ...\n");
		}
	} else {

		if (diag_device_mask & ( 1 << proc_type)) {
			DIAG_LOGE("\ndiag_mdlog: Reading mask for  proc_type: %d\n", proc_type);
				if ((proc_type == 1 && mask_file_mdm_entered)|| mask_file_entered)
						status = diag_read_mask_file_proc(proc_type);
					else
						status = read_mask_file_default(proc_type);

					if (status) {
						DIAG_LOGE("diag_mdlog: Error reading mask file, proc_type: %d, file: %s\n",
							proc_type, mask_file_proc[proc_type]);
					} else {
						num_mask_reads_succeeded++;
					}
		}


		/*
		 * If no mask files have been successfully read,
		 * try reading from a mask list file
		 */
		if (num_mask_reads_succeeded == 0) {
			DIAG_LOGE("\ndiag_mdlog: No successful mask file reads. Trying default mask list file.\n");
			status = diag_read_mask_file_list(mask_file_list);
			if (!status) {
				DIAG_LOGE("diag_mdlog: No mask files have been successfully read.\n");
				DIAG_LOGE("diag_mdlog: Running with masks that were set prior to diag_mdlog start-up.\n");
			}
		}
		status = 1;
	}
	return status;
}

static boolean diag_retry_initialization (uint8_t operation)
{
	uint8_t retry_count = 5;
	struct timeval wait_tv = { 0 };
	wait_tv.tv_sec = 1;

	while (retry_count) {
		select(0, NULL, NULL, NULL, &wait_tv);
		if (operation == CONNECTION_RETRY) {
			diag_fd = diag_lsm_comm_open();
			if (diag_fd != DIAG_INVALID_HANDLE)
				break;
		} else if (operation == MASK_SYNC_RETRY) {
			if (do_mask_sync())
			    break;
		}
		retry_count--;
	}

	if (retry_count == 0)
		return FALSE;

	return TRUE;
}

bool is_diagid_logging_format_selected(void)
{
	return ((diag_id_async_cmd_mask & PKT_FORMAT_MASK_ASYNC_PKT) &&
		diag_id) ?
		true : false;
}
