/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2017-2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag QDSS support

GENERAL DESCRIPTION

Implementation of configuring diag over qdss using diag command request/responses
and reading data from qdss, writing qdss data to file.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include <stdlib.h>
#include "comdef.h"
#include "stdio.h"
#include "stringl.h"
#include "diag_lsmi.h"
#include "diag_lsmi.h"
#include "diag_shared_i.h"
#include "stdio.h"
#include "string.h"
#include "diag_lsm.h"
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include "diagcmd.h"
#include "errno.h"
#include "diagdiag.h"
#include "diag_lsm_comm.h"
#ifndef FEATURE_LE_DIAG
#include <cutils/log.h>
#endif

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

#define ERESTARTSYS		512

#define QDSS_RSP_BUF_SIZE 100
#define QDSS_CMD_REQ_BUF_SIZE	50

#define BLOCK_SIZE "/sys/bus/coresight/devices/coresight-tmc-etr/block_size"
#define ETR1_BLOCK_SIZE "/sys/bus/coresight/devices/coresight-tmc-etr1/block_size"
#define MEM_TYPE "/sys/bus/coresight/devices/coresight-tmc-etr/mem_type"
#define ETR1_BUF_SIZE "/sys/bus/coresight/devices/coresight-tmc-etr1/buffer_size"
#define ETR_BUF_SIZE "/sys/bus/coresight/devices/coresight-tmc-etr/buffer_size"
#define RESET_SOURCE_SINK "/sys/bus/coresight/reset_source_sink"
#define MHI_QDSS_MODE "/sys/class/qdss_bridge/mhi_qdss/mode"

#define BLOCK_SIZE_VALUE "65536"
#define MEM_TYPE_CONTIG "contig"
#define MEM_SIZE_CONTIG "0x100000"
#define MEM_TYPE_SG "sg"
#define MHI_QDSS_MODE_UCI "uci"
#define MHI_QDSS_MODE_USB "usb"
#define HW_ACCEL_ENABLE 1
#define HW_ACCEL_DISABLE 0
#define ETR_BUF_SIZE_LEN 40
#define VALUE_LEN 20
static unsigned int qdss_mask;
static unsigned int device_mask;
unsigned int qdss_file_count[NUM_PROC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

extern unsigned int max_file_num;
extern int use_qmdl2_v2;
volatile int qdss_mdm_down;

char qdss_file_name_curr[NUM_PROC][FILE_NAME_LEN];
unsigned char qdss_read_buffer[READ_BUF_SIZE];
unsigned char qdss_read_buffer_mdm[READ_BUF_SIZE];
unsigned char qdss_cmd_req_buf[50];
pthread_t qdss_read_thread_hdl; /* Diag Read thread handle */
pthread_t qdss_read_thread_hdl_mdm;
pthread_t qdss_write_thread_hdl;    /* Diag disk write thread handle */
pthread_t qdss_write_thread_hdl_mdm;
pthread_t qdss_config_thread_hdl;	/* Diag disk config thread handle */
pthread_t qdss_config_thread_hdl_mdm;
static unsigned long qdss_count_written_bytes;
static unsigned long qdss_count_written_bytes_mdm;
int in_wait_for_qdss_peripheral_status = 0;
int in_wait_for_qdss_mdm_status = 0;
int in_wait_for_qdss_mdm_up_status = 0;

int qdss_diag_fd_md[NUM_PROC] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int qdss_diag_fd_dev = -1;
int qdss_diag_fd_dev_mdm = -1;
volatile int diag_qdss_node_fd = -1;
volatile int diag_qdss_node_fd_mdm = -1;
int qdss_state = 0;
volatile int qdss_init_done = DIAG_COND_WAIT;
volatile int qdss_init_done_mdm = DIAG_COND_WAIT;
volatile int in_qdss_read = 0;
volatile int in_qdss_read_mdm = 0;
/* Static array for workaround */
static unsigned char qdss_static_buffer[4][DISK_BUF_SIZE];
int rt_mask[NUM_PROC];
static int hw_accel_type[NUM_PROC][TOTAL_PD_COUNT];
static int etr_buffer_size_to_set = 0;
static int use_etr1_buffer = 0;
static boolean etr_buffer_size_updated = FALSE;

#define DIAG_DIAG_STM				0x214
#define DIAG_QDSS_TRACE_SINK		0x101
#define DIAG_QDSS_FILTER_STM		0x103
#define DIAG_DIAG_HW_ACCEL_CMD		0x224
#define DIAG_DIAG_FEATURE_QUERY		0x225
#define DIAG_QDSS_ETR1_TRACE_SINK	0x162
#define DIAG_QDSS_FILTER_SWTRACE	0x06
#define DIAG_QDSS_FILTER_ENTITY		0x08

#define DIAG_QDSS_PROCESSOR_APPS	0x0100
#define DIAG_QDSS_PROCESSOR_MODEM	0x0200
#define DIAG_QDSS_PROCESSOR_WCNSS	0x0300
#define DIAG_QDSS_PROCESSOR_LPASS	0x0500
#define DIAG_QDSS_PROCESSOR_SENSOR	0x0800
#define DIAG_QDSS_PROCESSOR_CDSP	0x0d00
#define DIAG_QDSS_PROCESSOR_NPU		0x0e00
#define DIAG_QDSS_PROCESSOR_GPDSP0	0x1000
#define DIAG_QDSS_PROCESSOR_GPDSP1	0x1100
#define DIAG_QDSS_PROCESSOR_NSP1	0x1200

#define SINK_ETB		0
#define SINK_DDR		1
#define SINK_TPIU_A		2
#define SINK_USB		3
#define SINK_USB_HSIC	4
#define SINK_TPIU_B		5
#define SINK_SD			6
#define SINK_FILE		7
#define SINK_PCIE		8

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

#define ETR_PRINT	(use_etr1_buffer) ? "etr1" : "etr"

static int qdss_curr_write;
static int qdss_curr_read;
static int qdss_write_in_progress;
int qdss_in_write = 0;
int qdss_in_read = 0;

static int qdss_curr_write_mdm;
static int qdss_curr_read_mdm;
static int qdss_write_in_progress_mdm;
int qdss_in_write_mdm = 0;
int qdss_in_read_mdm = 0;

static uint16 remote_mask = 0;
static int hw_accel_support[NUM_PROC][DIAG_HW_ACCEL_TYPE_MAX + 1];
static int trace_sink[NUM_PROC];
int qdss_kill_thread = 0;
static int qdss_kill_rw_thread = 0;
unsigned char default_etr_buffer_size[ETR_BUF_SIZE_LEN];
extern int diagid_guid_status[NUM_PROC];
extern unsigned long max_file_size;
extern unsigned long min_file_size;
char qdss_peripheral_name[FILE_NAME_LEN];
struct qdss_read_buf_pool {
	unsigned char* rsp_buf;
	int data_ready;
	pthread_mutex_t read_rsp_mutex;
	pthread_mutex_t write_rsp_mutex;
	pthread_cond_t write_rsp_cond;
	pthread_cond_t read_rsp_cond;
};

typedef enum {
	QDSS_KILL_STATE_OFF,
	QDSS_KILL_STATE_ON,
	QDSS_KILL_STATE_DONE,
} qdss_kill_state;

typedef enum {
	READ_ONLY,
	WRITE_ONLY,
	READ_WRITE,
} flag;
static struct qdss_read_buf_pool qdss_read_buffer_pool[2];
pthread_mutex_t qdss_diag_mutex;
pthread_mutex_t qdss_mdm_diag_mutex;

pthread_mutex_t qdss_config_mutex;
pthread_mutex_t qdss_mdm_config_mutex;

pthread_mutex_t qdss_set_data_ready_mutex;
pthread_mutex_t qdss_clear_data_ready_mutex;

pthread_mutex_t qdss_mdm_set_data_ready_mutex;
pthread_mutex_t qdss_mdm_clear_data_ready_mutex;

pthread_mutex_t qdss_mdm_down_mutex;
pthread_cond_t qdss_mdm_down_cond;

pthread_mutex_t qdss_kill_mutex;
pthread_cond_t qdss_kill_cond;

pthread_mutex_t diagid_guid_map_mutex[MDM + 1];
pthread_cond_t diagid_guid_map_cond[MDM + 1];

pthread_cond_t qdss_diag_cond;
pthread_cond_t qdss_mdm_diag_cond;

pthread_cond_t qdss_config_cond;
pthread_cond_t qdss_mdm_config_cond;

volatile int qdss_curr_read_idx = 0;
volatile int qdss_curr_write_idx = 0;
typedef PACK(struct) {
	unsigned int peripheral_mask;
	unsigned int peripheral_type;
} qdss_peripheral_info;

qdss_peripheral_info qdss_periph_info;

unsigned int p_type_mask;
unsigned int msm_peripheral_mask;
unsigned int mdm_peripheral_mask;

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 version;
	uint16 processor_mask;
	uint8 stm_cmd;
} diag_qdss_config_req_v3;

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 version;
	uint8 processor_mask;
	uint8 stm_cmd;
} diag_qdss_config_req_v2;

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 state;
} diag_enable_qdss_tracer_req;

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 state;
	uint8 entity_id;
} diag_enable_qdss_diag_tracer_req;

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 state;
} diag_enable_qdss_req;

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 sink;
} diag_set_out_mode;

struct buffer_pool qdss_pools[] = {
	[0] = {
		.free		=	1,
		.data_ready	=	0,
	},
	[1] = {
		.free		=	1,
		.data_ready	=	0,
	},

};
struct buffer_pool qdss_pools_mdm[] = {
	[0] = {
		.free		=	1,
		.data_ready	=	0,
	},
	[1] = {
		.free		=	1,
		.data_ready	=	0,
	},

};

static void* qdss_read_thread(void* param);
static void* qdss_write_thread(void* param);
void* qdss_config_thread(void* param);
static void* qdss_read_thread_mdm(void* param);
static void* qdss_write_thread_mdm(void* param);
void* qdss_config_thread_mdm(void* param);
static int diag_qdss_create_file(int type);

void diag_set_etr_buffer_size(int buf_size)
{
	etr_buffer_size_to_set = buf_size;
}
void diag_notify_qdss_thread(int peripheral_type, int peripheral_mask)
{
	if (qdss_kill_thread)
		return;

	qdss_periph_info.peripheral_type |= peripheral_type;
	qdss_periph_info.peripheral_mask |= peripheral_mask;
	if (peripheral_type & DIAG_MSM_MASK)
		msm_peripheral_mask = peripheral_mask;
	if (peripheral_type & DIAG_MDM_MASK)
		mdm_peripheral_mask = peripheral_mask;
	pthread_cond_signal(&qdss_diag_cond);
	pthread_cond_signal(&qdss_mdm_diag_cond);
	pthread_cond_signal(&qdss_mdm_down_cond);
	DIAG_LOGD("diag: %s: Signalled qdss threads for peripheral_type: %d, peripheral_mask: %d\n",
		__func__, peripheral_type, peripheral_mask);
}

static int wait_for_response(void)
{
	struct timespec time;
	struct timeval now;
	int rt = 0;

	/****************************************************************
	 * Wait time is 10 sec while setting the QDSS environment		*
	 ****************************************************************/

	gettimeofday(&now, NULL);
	time.tv_sec = now.tv_sec + 10000 / 1000;
	time.tv_nsec = now.tv_usec + (10000 % 1000) * 1000000;
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
	if (!qdss_read_buffer_pool[qdss_curr_write_idx].data_ready)
		rt = pthread_cond_timedwait(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_cond), &(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex), &time);
	return rt;
}

static int wait_for_kill_response(void)
{
	struct timespec time;
	struct timeval now;
	int rt = 0;

	/****************************************************************
	 * Wait time is 5 sec while resetting the QDSS environment		*
	 ****************************************************************/
	gettimeofday(&now, NULL);
	time.tv_sec = now.tv_sec + 5000 / 1000;
	time.tv_nsec = now.tv_usec + (5000 % 1000) * 1000000;
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
	if (!qdss_read_buffer_pool[qdss_curr_write_idx].data_ready)
		rt = pthread_cond_timedwait(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_cond), &(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex), &time);
	return rt;
}

static int diag_set_coresight_sysfs(const char *block_file_path, const char *val, const char *str, int flag)
{
	int block_fd, ret;
	char value[VALUE_LEN];

	if (!block_file_path || (strlen(val) >= sizeof(value))) {
		return -1;
	}

	memset(value, '\0', sizeof(value));
	if (flag == WRITE_ONLY || flag == READ_WRITE) {
		block_fd = open(block_file_path, O_WRONLY);
		if (block_fd < 0) {
			DIAG_LOGE("diag: %s open fail: %s error: %s\n", str, block_file_path, strerror(errno));
			goto err;
		}

		ret = write(block_fd, val, strlen(val) + 1);
		if (ret < 0) {
			DIAG_LOGE("diag: %s write fail: %s error: %s\n", str, block_file_path, strerror(errno));
			close(block_fd);
			goto err;
		}
		close(block_fd);
	}
	if (flag == READ_WRITE) {
		block_fd = open(block_file_path, O_RDONLY);
		if (block_fd < 0) {
			DIAG_LOGE("diag: %s open fail: %s error: %s\n", str, block_file_path, strerror(errno));
			goto err;
		}

		ret = read(block_fd, value, (VALUE_LEN - 1));
		if (ret < 0) {
			DIAG_LOGE("diag: %s read fail: %s error: %s\n", str, block_file_path, strerror(errno));
			close(block_fd);
			goto err;
		} else {
			value[ret] = '\0';
		}
		DIAG_LOGD("diag: Value set to %s = %s\n", block_file_path, value);

		close(block_fd);
	}
	return 0;
err:
	return -1;
}

void qdss_close_qdss_node_mdm(void)
{
	if (diag_qdss_node_fd_mdm >= 0) {
		close(diag_qdss_node_fd_mdm);
		diag_qdss_node_fd_mdm = -1;
	}
}

int diag_set_etr_buf_size(void)
{
	int fd, block_fd = -1, ret;
	char etr_buffer_size[ETR_BUF_SIZE_LEN];

	if (use_etr1_buffer)
		block_fd = open(ETR1_BUF_SIZE, O_RDONLY);
	else
		block_fd = open(ETR_BUF_SIZE, O_RDONLY);
	if (block_fd < 0) {
		DIAG_LOGE("diag: %s buffer size open fail error: %s\n", ETR_PRINT,
			  strerror(errno));
		return block_fd;
	}

	ret = read(block_fd, default_etr_buffer_size, sizeof(default_etr_buffer_size) - 1);
	if (ret < 0) {
		DIAG_LOGE("diag: %s buffer size read fail error: %s\n", ETR_PRINT,
			  strerror(errno));
		close(block_fd);
		return ret;
	} else {
		default_etr_buffer_size[ret] = '\0';
	}
	close(block_fd);

	std_strlprintf(etr_buffer_size, ETR_BUF_SIZE_LEN, "%d", etr_buffer_size_to_set);

	if (use_etr1_buffer)
		ret = diag_set_coresight_sysfs(ETR1_BUF_SIZE, etr_buffer_size, "ETR1 BUFFER SIZE", READ_WRITE);
	else
		ret = diag_set_coresight_sysfs(ETR_BUF_SIZE, etr_buffer_size, "ETR BUFFER SIZE", READ_WRITE);

	return ret;
}

int diag_qdss_init(void)
{
	uint16 z = 1, proc_type;
	int ret = 0, local_remote_mask, local_device_mask = 0;
	int msm_qdss_config_fail = 0;
	int status;

	in_qdss_read = 0;
	pthread_mutex_init(&qdss_diag_mutex, NULL);
	pthread_mutex_init(&qdss_config_mutex, NULL);
	pthread_cond_init(&qdss_diag_cond, NULL);
	pthread_cond_init(&qdss_config_cond, NULL);

	pthread_mutex_init(&diagid_guid_map_mutex[MSM], NULL);
	pthread_mutex_init(&diagid_guid_map_mutex[MDM], NULL);
	pthread_cond_init(&diagid_guid_map_cond[MSM], NULL);
	pthread_cond_init(&diagid_guid_map_cond[MDM], NULL);

	pthread_cond_init(&qdss_mdm_diag_cond, NULL);
	pthread_cond_init(&qdss_mdm_config_cond, NULL);
	pthread_mutex_init(&qdss_mdm_diag_mutex, NULL);
	pthread_mutex_init(&qdss_mdm_config_mutex, NULL);

	pthread_mutex_init(&qdss_set_data_ready_mutex, NULL);
	pthread_mutex_init(&qdss_mdm_set_data_ready_mutex, NULL);
	pthread_mutex_init(&qdss_clear_data_ready_mutex, NULL);
	pthread_mutex_init(&qdss_mdm_clear_data_ready_mutex, NULL);

	pthread_mutex_init(&(qdss_read_buffer_pool[0].read_rsp_mutex), NULL);
	pthread_mutex_init(&(qdss_read_buffer_pool[1].read_rsp_mutex), NULL);
	pthread_mutex_init(&(qdss_read_buffer_pool[0].write_rsp_mutex), NULL);
	pthread_mutex_init(&(qdss_read_buffer_pool[1].write_rsp_mutex), NULL);
	pthread_cond_init(&(qdss_read_buffer_pool[0].read_rsp_cond), NULL);
	pthread_cond_init(&(qdss_read_buffer_pool[0].write_rsp_cond), NULL);
	pthread_cond_init(&(qdss_read_buffer_pool[1].read_rsp_cond), NULL);
	pthread_cond_init(&(qdss_read_buffer_pool[1].write_rsp_cond), NULL);

	pthread_mutex_init(&qdss_kill_mutex, NULL);
	pthread_cond_init(&qdss_kill_cond, NULL);

	qdss_read_buffer_pool[0].rsp_buf = malloc(QDSS_RSP_BUF_SIZE);
	if (!qdss_read_buffer_pool[0].rsp_buf){
		DIAG_LOGE("%s:failed to create rsp buffer zero\n", __func__);
		return FALSE;
	}
	qdss_read_buffer_pool[1].rsp_buf = malloc(QDSS_RSP_BUF_SIZE);
	if (!qdss_read_buffer_pool[1].rsp_buf){
		DIAG_LOGE("%s:failed to create rsp buffer one\n", __func__);
		free(qdss_read_buffer_pool[0].rsp_buf);
		return FALSE;
	}
	qdss_read_buffer_pool[0].data_ready = 0;
	qdss_read_buffer_pool[1].data_ready = 0;

	pthread_cond_init(&(qdss_pools[0].write_cond), NULL);
	pthread_cond_init(&(qdss_pools[0].read_cond), NULL);
	pthread_cond_init(&(qdss_pools[1].write_cond), NULL);
	pthread_cond_init(&(qdss_pools[1].read_cond), NULL);

	pthread_cond_init(&(qdss_pools_mdm[0].write_cond), NULL);
	pthread_cond_init(&(qdss_pools_mdm[0].read_cond), NULL);
	pthread_cond_init(&(qdss_pools_mdm[1].write_cond), NULL);
	pthread_cond_init(&(qdss_pools_mdm[1].read_cond), NULL);

	diag_has_remote_device(&remote_mask);
	qdss_mdm_down = 0;

	if (qdss_mask) {
		if (device_mask & DIAG_MSM_MASK) {
			qdss_periph_info.peripheral_type = DIAG_MSM_MASK | (device_mask & (remote_mask << 1));
		} else if ((device_mask & DIAG_MDM_MASK) || (device_mask & DIAG_MDM2_MASK)) {
			qdss_periph_info.peripheral_type = (device_mask & (remote_mask << 1));
		}
		msm_peripheral_mask = qdss_mask;
		mdm_peripheral_mask = qdss_mask;
		qdss_periph_info.peripheral_mask = qdss_mask;
		p_type_mask = qdss_periph_info.peripheral_type;
	}

	qdss_pools[0].buffer_ptr[0] = qdss_static_buffer[0];
	qdss_pools[1].buffer_ptr[0] = qdss_static_buffer[1];
	qdss_pools[0].bytes_in_buff[0] = 0;
	qdss_pools[1].bytes_in_buff[0] = 0;

	qdss_pools_mdm[0].buffer_ptr[0] = qdss_static_buffer[2];
	qdss_pools_mdm[1].buffer_ptr[0] = qdss_static_buffer[3];
	qdss_pools_mdm[0].bytes_in_buff[0] = 0;
	qdss_pools_mdm[1].bytes_in_buff[0] = 0;

	/****************************************************************
	 * Necessary to set Block size before accessing /dev/byte-cntr  *
	 ****************************************************************/

	status = system("ls /dev/byte-cntr1");
	if (status != -1 && etr_buffer_size_to_set) {
		use_etr1_buffer = 1;
		ret = diag_set_coresight_sysfs(ETR1_BLOCK_SIZE, BLOCK_SIZE_VALUE,
						"byte_cntr1 - block_size", READ_WRITE);
	} else {
		ret = diag_set_coresight_sysfs(BLOCK_SIZE, BLOCK_SIZE_VALUE,
						"byte_cntr - block_size", READ_WRITE);
	}
	if (ret) {
		DIAG_LOGE(" %s: block size write failed\n", __func__);
		free(qdss_read_buffer_pool[0].rsp_buf);
		free(qdss_read_buffer_pool[1].rsp_buf);
		return -1;
	}

	ret = diag_lsm_comm_ioctl(diag_fd, DIAG_IOCTL_UPDATE_QDSS_ETR1_SUP,
				  &use_etr1_buffer, sizeof(use_etr1_buffer));
	if (ret)
		DIAG_LOGE("diag: IOCTL to Update QDSS ETR1 Support not present\n");

	diag_get_peripheral_name_from_mask(qdss_peripheral_name,
							FILE_NAME_LEN, qdss_mask);

	diag_set_coresight_sysfs(RESET_SOURCE_SINK, "1", "reset source sink", WRITE_ONLY);

	if (device_mask & DIAG_MSM_MASK) {
		pthread_mutex_lock(&qdss_config_mutex);
		qdss_init_done = DIAG_COND_WAIT;
		pthread_create(&qdss_config_thread_hdl, NULL, qdss_config_thread, NULL);
		if (qdss_config_thread_hdl == 0) {
			pthread_mutex_unlock(&qdss_config_mutex);
			DIAG_LOGE("%s: Failed to create config thread", __func__);
			goto failure_case;
		}

		while (qdss_init_done == DIAG_COND_WAIT)
			pthread_cond_wait(&qdss_config_cond, &qdss_config_mutex);
		if (qdss_init_done == DIAG_COND_COMPLETE) {
			pthread_create(&qdss_read_thread_hdl, NULL, qdss_read_thread, NULL);
			if (qdss_read_thread_hdl == 0) {
				pthread_mutex_unlock(&qdss_config_mutex);
				DIAG_LOGE("%s: Failed to create read thread", __func__);
				goto failure_case;
			}
			diag_qdss_create_file(MSM);
			pthread_create(&qdss_write_thread_hdl, NULL, qdss_write_thread, NULL);
			if (qdss_write_thread_hdl == 0) {
				pthread_mutex_unlock(&qdss_config_mutex);
				DIAG_LOGE("%s: Failed to create write thread", __func__);
				goto failure_case;
			}
		} else {
			msm_qdss_config_fail = 1;
		}
		pthread_mutex_unlock(&qdss_config_mutex);
	}
	/* check if any mdm devices are selected using -j option. Right shift by 1 to check values for
	 * MDMs and clear MSM bit mask */
	local_remote_mask = remote_mask;
	while(local_remote_mask) {
		if(local_remote_mask & 1 ) {
 			proc_type = z;
 			local_device_mask = local_device_mask | (1 << proc_type);
 		}
 		z++;
		local_remote_mask = local_remote_mask >> 1;
	}

	if (device_mask & local_device_mask & (1 << MDM)) {
		pthread_mutex_lock(&qdss_mdm_config_mutex);
		qdss_init_done_mdm = DIAG_COND_WAIT;
		pthread_create(&qdss_config_thread_hdl_mdm, NULL, qdss_config_thread_mdm, NULL);
		if (qdss_config_thread_hdl_mdm == 0) {
			DIAG_LOGE("%s: Failed to create config thread", __func__);
			pthread_mutex_unlock(&qdss_mdm_config_mutex);
			goto failure_case;
		}

		while (qdss_init_done_mdm == DIAG_COND_WAIT)
			pthread_cond_wait(&qdss_mdm_config_cond, &qdss_mdm_config_mutex);
		if (qdss_init_done_mdm == DIAG_COND_ERROR) {
			pthread_mutex_unlock(&qdss_mdm_config_mutex);
			goto failure_case;
		}
		pthread_mutex_unlock(&qdss_mdm_config_mutex);

		pthread_create(&qdss_read_thread_hdl_mdm, NULL, qdss_read_thread_mdm, NULL);
		if (qdss_read_thread_hdl_mdm == 0) {
			DIAG_LOGE("%s: Failed to create read thread", __func__);
			goto failure_case;
		}

		diag_qdss_create_file(MDM);
		pthread_create(&qdss_write_thread_hdl_mdm, NULL, qdss_write_thread_mdm, NULL);
		if (qdss_write_thread_hdl_mdm == 0) {
			DIAG_LOGE("%s: Failed to create write thread", __func__);
			goto failure_case;
		}
	} else if (msm_qdss_config_fail) {
			goto failure_case;
	}

	return 0;

 failure_case:
	close(diag_qdss_node_fd);
	close(diag_qdss_node_fd_mdm);
	free(qdss_read_buffer_pool[0].rsp_buf);
	free(qdss_read_buffer_pool[1].rsp_buf);
	return -1;

}
static int diag_qdss_reset_read_buffer(void)
{
	qdss_read_buffer_pool[qdss_curr_write_idx].data_ready = 0;
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_write_idx].read_rsp_mutex));
	pthread_cond_signal(&(qdss_read_buffer_pool[qdss_curr_write_idx].read_rsp_cond));
	pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].read_rsp_mutex));
	pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));

	if (!qdss_curr_write_idx)
		qdss_curr_write_idx = 1;
	else
		qdss_curr_write_idx = 0;

	return 0;
}
static int diag_set_diag_transport(int peripheral_type, int peripheral, uint8 stm_cmd)
{
	int offset = 0, length = 0, ret = 0;
	uint16 proc_mask;
	void * req = NULL;
	unsigned char* ptr = qdss_cmd_req_buf;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM_2) ||
		(peripheral < DIAG_MODEM_PROC || peripheral > (NUM_PERIPHERALS - 1))) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type) {
		*(int*)(ptr + offset) = -peripheral_type;
		offset += sizeof(int);
	}
	ptr = ptr + offset;

	/*************************************************
	 * 	4B 12 214 02 X Y							 *
	 * 												 *
	 * 	X = STM Processor Mask						 *
	 * 	Y = STM Enable(1)/Disable(0)				 *
	 *************************************************/
	if (diag_use_dev_node) {
		req = (diag_qdss_config_req_v2*)ptr;
		((diag_qdss_config_req_v2*)req)->cmd_code = DIAG_SUBSYS_CMD_F;
		((diag_qdss_config_req_v2*)req)->subsys_id = DIAG_SUBSYS_DIAG_SERV;
		((diag_qdss_config_req_v2*)req)->subsys_cmd_code = DIAG_DIAG_STM;
		((diag_qdss_config_req_v2*)req)->version = 2;
	} else {
		req = (diag_qdss_config_req_v3*)ptr;
		((diag_qdss_config_req_v3*)req)->subsys_id = DIAG_SUBSYS_DIAG_SERV;
		((diag_qdss_config_req_v3*)req)->cmd_code = DIAG_SUBSYS_CMD_F;
		((diag_qdss_config_req_v3*)req)->subsys_cmd_code = DIAG_DIAG_STM;
		((diag_qdss_config_req_v3*)req)->version = 3;
	}

	switch (peripheral) {
	case DIAG_MODEM_PROC :
		proc_mask = DIAG_STM_MODEM;
		break;
	case DIAG_LPASS_PROC :
		proc_mask= DIAG_STM_LPASS;
		break;
	case DIAG_WCNSS_PROC :
		proc_mask = DIAG_STM_WCNSS;
		break;
	case DIAG_APPS_PROC :
		proc_mask = DIAG_STM_APPS;
		break;
	case DIAG_CDSP_PROC :
		proc_mask = DIAG_STM_CDSP;
		break;
	case DIAG_NPU_PROC :
		proc_mask = DIAG_STM_NPU;
		break;
	case DIAG_NSP1_PROC :
		proc_mask = DIAG_STM_NSP1;
		break;
	case DIAG_GPDSP0_PROC :
		proc_mask = DIAG_STM_GPDSP0;
		break;
	case DIAG_GPDSP1_PROC :
		proc_mask = DIAG_STM_GPDSP1;
		break;
	default:
		DIAG_LOGE("diag:%s Invalid peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}
	if (diag_use_dev_node) {
		((diag_qdss_config_req_v2*)req)->processor_mask = proc_mask;
		((diag_qdss_config_req_v2*)req)->stm_cmd = stm_cmd;
		length = sizeof(diag_qdss_config_req_v2);
	} else  {
		((diag_qdss_config_req_v3*)req)->processor_mask = proc_mask;
		((diag_qdss_config_req_v3*)req)->stm_cmd = stm_cmd;
		length = sizeof(diag_qdss_config_req_v3);
	}

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;

}
static int diag_set_diag_qdss_tracer(int peripheral_type, int peripheral, uint8 state)
{
	int offset = 0, length = 0, ret = 0;
	unsigned char* ptr = qdss_cmd_req_buf;
	diag_enable_qdss_tracer_req* req = NULL;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM_2) ||
		(peripheral < DIAG_MODEM_PROC || peripheral > (NUM_PERIPHERALS - 1))) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type) {
		*(int*)(ptr + offset) = -peripheral_type;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_enable_qdss_tracer_req*)ptr;

	/*************************************************
	 * 	4B 5A 06 X Y 0D								 *
	 * 												 *
	 * 	X = QDSS PROCESSOR MASK						 *
	 * 	Y = State Enable(1)/Disable(0)				 *
	 *************************************************/
	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_QDSS;
	req->subsys_cmd_code = DIAG_QDSS_FILTER_SWTRACE;

	switch (peripheral) {
	case DIAG_APPS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_APPS;
		break;
	case DIAG_MODEM_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_MODEM;
		break;
	case DIAG_WCNSS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_WCNSS;
		break;
	case DIAG_LPASS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_LPASS;
		break;
	case DIAG_SENSORS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_SENSOR;
		break;
	case DIAG_CDSP_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_CDSP;
		break;
	case DIAG_NPU_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_NPU;
		break;
	case DIAG_NSP1_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_NSP1;
		break;
	case DIAG_GPDSP0_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_GPDSP0;
		break;
	case DIAG_GPDSP1_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_GPDSP1;
		break;
	default:
		DIAG_LOGE("diag:%s support for peripheral = %d, peripheral_type = %d not present yet\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	req->state = state;
	length = sizeof(diag_enable_qdss_tracer_req);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;

}

static int diag_set_diag_qdss_diag_tracer(int peripheral_type, int peripheral, uint8 state)
{
	int offset = 0, length = 0, ret = 0;
	unsigned char* ptr = qdss_cmd_req_buf;
	diag_enable_qdss_diag_tracer_req* req = NULL;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM_2) ||
		(peripheral < DIAG_MODEM_PROC || peripheral > (NUM_PERIPHERALS - 1))) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type) {
		*(int*)(ptr + offset) = -peripheral_type;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_enable_qdss_diag_tracer_req*)ptr;

	/*************************************************
	 * 	4B 5A 08 X Y 0D								 *
	 * 												 *
	 * 	X = QDSS PROCESSOR MASK						 *
	 * 	Y = State Enable(1)/Disable(0)				 *
	 *************************************************/
	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_QDSS;
	req->subsys_cmd_code = DIAG_QDSS_FILTER_ENTITY;

	switch (peripheral) {
	case DIAG_APPS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_APPS;
		break;
	case DIAG_MODEM_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_MODEM;
		break;
	case DIAG_WCNSS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_WCNSS;
		break;
	case DIAG_LPASS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_LPASS;
		break;
	case DIAG_SENSORS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_SENSOR;
		break;
	case DIAG_CDSP_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_CDSP;
		break;
	case DIAG_NPU_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_NPU;
		break;
	case DIAG_NSP1_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_NSP1;
		break;
	case DIAG_GPDSP0_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_GPDSP0;
		break;
	case DIAG_GPDSP1_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_GPDSP1;
		break;
	default:
		DIAG_LOGE("diag:%s support for peripheral = %d, peripheral_type = %d is not present\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	req->state = state;
	req->entity_id = 0x0D;
	length = sizeof(diag_enable_qdss_diag_tracer_req);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;

}
int diag_send_enable_qdss_req(int peripheral_type, int peripheral, uint8 state)
{
	int offset = 0, length = 0, ret = 0;
	diag_enable_qdss_req* req = NULL;
	unsigned char* ptr = qdss_cmd_req_buf;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM_2) ||
		(peripheral < DIAG_MODEM_PROC || peripheral > (NUM_PERIPHERALS - 1))) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type) {
		*(int*)(ptr + offset) = -peripheral_type;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_enable_qdss_req*)ptr;

	/*************************************************
	 * 	4B 5A 0103 X								 *
	 *												 *
	 * 	X = 1 To enable STM							 *
	 * 	X = 0 To Disable STM						 *
	 *************************************************/
	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_QDSS;
	req->subsys_cmd_code = DIAG_QDSS_FILTER_STM;
	req->state = state;
	length = sizeof(diag_enable_qdss_req);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;
}
int diag_send_enable_hw_accel_req(int peripheral_type, int peripheral, int diag_id,
					int hw_accel_type, int hw_accel_ver, uint8 state)
{
	int offset = 0, length = 0, ret = 0;
	diag_hw_accel_cmd_req_t* req = NULL;
	unsigned char* ptr = qdss_cmd_req_buf;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM_2) ||
		(peripheral < DIAG_MODEM_PROC || peripheral > (NUM_PERIPHERALS - 1))) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type) {
		*(int*)(ptr + offset) = -peripheral_type;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_hw_accel_cmd_req_t*)ptr;
	req->header.cmd_code = DIAG_SUBSYS_CMD_F;
	req->header.subsys_id = DIAG_SUBSYS_DIAG_SERV;
	req->header.subsys_cmd_code = DIAG_DIAG_HW_ACCEL_CMD;
	req->version = 1 ;
	req->operation = state;
	req->op_req.hw_accel_type = hw_accel_type;
	req->op_req.hw_accel_ver = hw_accel_ver;
	req->op_req.diagid_mask = 1 << (diag_id - 1);
	length = sizeof(diag_hw_accel_cmd_req_t);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;
}

int diag_set_etr_out_mode(int peripheral_type, int peripheral, uint8 sink)
{
	int offset = 0, length = 0, ret = 0;
	diag_set_out_mode* req = NULL;
	unsigned char* ptr = qdss_cmd_req_buf;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM_2) ||
		(peripheral < DIAG_MODEM_PROC || peripheral > (NUM_PERIPHERALS - 1))) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type) {
		*(int*)(ptr + offset) = -peripheral_type;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_set_out_mode*)ptr;

	/*************************************************
	 * 	4B 5A 0101 X								 *
	 * 												 *
	 * 	X = 01 for DDR								 *
	 * 	X = 03 for USB								 *
	 * 	X = 08 for PCIe								 *
	 *************************************************/
	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_QDSS;
	req->subsys_cmd_code = DIAG_QDSS_TRACE_SINK;
	req->sink = sink;
	length = sizeof(diag_set_out_mode);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;
}
int diag_set_etr1_out_mode(int peripheral_type, int peripheral, uint8 sink)
{
	int offset = 0, length = 0, ret = 0;
	diag_set_out_mode* req = NULL;
	unsigned char* ptr = qdss_cmd_req_buf;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM_2) ||
		(peripheral < DIAG_MODEM_PROC || peripheral > (NUM_PERIPHERALS - 1))) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type) {
		*(int*)(ptr + offset) = -peripheral_type;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_set_out_mode*)ptr;

	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_QDSS;
	req->subsys_cmd_code = DIAG_QDSS_ETR1_TRACE_SINK;
	req->sink = sink;
	length = sizeof(diag_set_out_mode);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}
	return TRUE;
}

int diag_configure_mdm_proc(const char *mode)
{
	int rt;

	rt = diag_set_coresight_sysfs(MHI_QDSS_MODE, mode, "mhi_qdss mode", READ_WRITE);
	if (rt) {
		DIAG_LOGE("diag: Failed to set mhi_qdss mode to uci\n");
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		return -1;
	}

	return 0;
}

int diag_send_cmds_to_peripheral_init(int peripheral_type, int pd)
{
	int rt;
	uint8 sink = 0;
	int diag_id, peripheral = -1;
	diag_id_list *item = NULL;

	qdss_state = 1;

	peripheral = pd;
	if ((pd > NUM_PERIPHERALS) && (pd < TOTAL_PD_COUNT)) {
		peripheral = get_peripheral_by_pd(peripheral_type, pd);
		if (peripheral < 0) {
			qdss_state = 0;
			return -1;
		}
		DIAG_LOGE("diag: %s: pd: %d, peripheral: %d\n", __func__, pd, peripheral);
	} else if (pd >= TOTAL_PD_COUNT) {
		qdss_state = 0;
		return -1;
	}
	/*************************************
	*		Set ETR routing     	 	 *
	**************************************/
	if (peripheral_type)
		sink = SINK_PCIE;
	else
		sink = SINK_DDR;

	if (etr_buffer_size_to_set && (!etr_buffer_size_updated)) {
		rt = diag_set_etr_buf_size();
		if (rt < 0) {
			qdss_state = 0;
			return -1;
		}
		etr_buffer_size_updated = TRUE;
	}

	if (use_etr1_buffer)
		rt = diag_set_etr1_out_mode(peripheral_type, peripheral, sink);
	else
		rt = diag_set_etr_out_mode(peripheral_type, peripheral, sink);
	if (rt == FALSE) {
		qdss_state = 0;
		DIAG_LOGE(" %s: failed to send diag_set_%s_out_mode\n", __func__, ETR_PRINT);
		return -1;
	}
	rt = wait_for_response();
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag:%s time out while waiting OUT Mode cmd response p_type: %d pd: %d, peripheral: %d\n",
			__func__, peripheral_type, pd, peripheral);
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}

	diag_qdss_reset_read_buffer();

	/*************************************
	*		Enable QDSS ETM				 *
	**************************************/
	if (hw_accel_support[peripheral_type][DIAG_HW_ACCEL_TYPE_ATB] ||
		hw_accel_support[peripheral_type][DIAG_HW_ACCEL_TYPE_STM]) {
		item = get_diag_id(peripheral_type, pd);
		if (!item) {
			qdss_state = 0;
			return -1;
		}
		diag_id = item->diag_id;
		if (hw_accel_support[peripheral_type][DIAG_HW_ACCEL_TYPE_ATB] & ( 1 << (diag_id-1))) {
			hw_accel_type[peripheral_type][pd] = DIAG_HW_ACCEL_TYPE_ATB;
		} else if (hw_accel_support[peripheral_type][DIAG_HW_ACCEL_TYPE_STM] & ( 1 << (diag_id-1))) {
			hw_accel_type[peripheral_type][pd] = DIAG_HW_ACCEL_TYPE_STM;
		} else {
			goto default_stm;
		}
		DIAG_LOGE("diag: sent enable cmd for hw accel type %d for p_type: %d pd: %d, peripheral %d\n",
				hw_accel_type[peripheral_type][peripheral], peripheral_type, pd, peripheral);
		rt = diag_send_enable_hw_accel_req(peripheral_type, peripheral, diag_id,
							hw_accel_type[peripheral_type][pd],
							DIAG_HW_ACCEL_VER_MAX,
							HW_ACCEL_ENABLE);
		if (rt == FALSE) {
			qdss_state = 0;
			DIAG_LOGE(" %s: failed to send diag_send_enable_hw_accel_req\n", __func__);
			return -1;
		}
		rt = wait_for_response();
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag:%s time out while waiting for enable hw accel cmd response p_type: %d, pd: %d, peipheral:%d\n",
					__func__, peripheral_type, pd, peripheral);
			pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
			qdss_state = 0;
			return -1;
		}
		diag_qdss_reset_read_buffer();
		qdss_state = 0;
		return 0;
	} else {
		default_stm:
		rt = diag_send_enable_qdss_req(peripheral_type, peripheral, 1);
		if (rt == FALSE) {
			qdss_state = 0;
			DIAG_LOGE(" %s: failed to send diag_send_enable_qdss_req\n", __func__);
			return -1;
		}
		rt = wait_for_response();
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag:%s time out while waiting for enable QDSS cmd response p_type:%d, pd: %d, peipheral:%d\n",
					__func__, peripheral_type, pd, peripheral);
			pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
			qdss_state = 0;
			return -1;
		}
		diag_qdss_reset_read_buffer();

		/*************************************
		*	Send command to diag to 		 *
		*	set STM Mask for the peripheral	 *
		**************************************/
		rt = diag_set_diag_transport(peripheral_type, peripheral, 1);
		if (rt == FALSE) {
			qdss_state = 0;
			DIAG_LOGE(" %s: failed to send diag_set_diag_transport\n", __func__);
			return -1;
		}
		rt = wait_for_response();
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag:%s time out while waiting for set QDSS cmd response for p_type:%d, pd: %d, peipheral:%d\n",
					__func__, peripheral_type, pd, peripheral);
			pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
			qdss_state = 0;
			return -1;
		}

		diag_qdss_reset_read_buffer();

		/*************************************
		*	Enable QDSS tracer			 	 *
		**************************************/
		rt = diag_set_diag_qdss_tracer(peripheral_type, peripheral, 1);
		if (rt == FALSE) {
			qdss_state = 0;
			DIAG_LOGE(" %s: failed to send diag_set_diag_qdss_tracer\n", __func__);
			return -1;
		}
		rt = wait_for_response();
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag:%s time out while waiting for set diag qdss tracer cmd response for p_type:%d, pd: %d, peipheral:%d\n",
					__func__, peripheral_type, pd, peripheral);
			pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
			qdss_state = 0;
			return -1;
		}
		diag_qdss_reset_read_buffer();

		/*************************************
		*	Enable QDSS tracer DIAG entity 	 *
		**************************************/
		rt = diag_set_diag_qdss_diag_tracer(peripheral_type, peripheral, 1);
		if (rt == FALSE) {
			qdss_state = 0;
			DIAG_LOGE(" %s: failed to send diag_set_diag_qdss_diag_tracer\n", __func__);
			return -1;
		}
		rt = wait_for_response();
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag:%s time out while waiting for set qdss tracer diag entity cmd response for p_type:%d, pd: %d, peipheral:%d\n",
					__func__, peripheral_type, pd, peripheral);
			pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
			qdss_state = 0;
			return -1;
		}
		diag_qdss_reset_read_buffer();
	}

	/*********************************************
	*	Commands to set QDSS environment sent	 *
	**********************************************/
	qdss_state = 0;
	return 0;
}

int diag_send_cmds_to_peripheral_kill(int peripheral_type, int pd)
{
	int rt, diag_id, peripheral;
	diag_id_list *item = NULL;

	qdss_state = 1;

#if 0
	/*************************************************
	 * 		Reset mem_type to Scatter Gather		 *
	 *************************************************/
	rt = diag_set_coresight_sysfs(MEM_TYPE, MEM_TYPE_SG, "coresight - mem_type");
	if (rt) {
		qdss_state = 0;
		DIAG_LOGE("diag: Failed to set memtype to sg\n");
		return -1;
	}
#endif
	peripheral = pd;
	if ((pd > NUM_PERIPHERALS) && (pd < TOTAL_PD_COUNT)) {
		peripheral = get_peripheral_by_pd(peripheral_type, pd);
		if (peripheral < 0) {
			qdss_state = 0;
			return -1;
		}
		DIAG_LOGE("diag: %s: pd: %d, peripheral: %d\n", __func__, pd, peripheral);
	} else if (pd >= TOTAL_PD_COUNT) {
		qdss_state = 0;
		return -1;
	}

	if (hw_accel_type[peripheral_type][pd] == DIAG_HW_ACCEL_TYPE_ATB ||
		hw_accel_type[peripheral_type][pd] == DIAG_HW_ACCEL_TYPE_STM) {
		item = get_diag_id(peripheral_type, pd);
		if (!item) {
			qdss_state = 0;
			return -1;
		}
		diag_id = item->diag_id;
		DIAG_LOGE("diag:sent disable command for pd: %d, peripheral: %d, diag_id: %d, type %d\n",
				pd, peripheral, diag_id, hw_accel_type[peripheral_type][pd]);
		rt = diag_send_enable_hw_accel_req(peripheral_type, peripheral, diag_id,
						hw_accel_type[peripheral_type][pd],
						DIAG_HW_ACCEL_VER_MAX,
						HW_ACCEL_DISABLE);
		if (rt == FALSE) {
			qdss_state = 0;
			DIAG_LOGE(" %s: failed to send diag_send_enable_hw_accel_req\n", __func__);
			return -1;
		}
		rt = wait_for_response();
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag:%s time out while waiting for enable ATB cmd response p_type:%d peipheral:%d\n",
					__func__, peripheral_type, peripheral);
			pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
			qdss_state = 0;
			return -1;
		}
		diag_qdss_reset_read_buffer();
	} else {
		/*************************************
	 	* 		Disable QDSS ETM			 *
	 	*************************************/
		rt = diag_send_enable_qdss_req(peripheral_type, peripheral, 0);
		if (rt == FALSE) {
			qdss_state = 0;
			DIAG_LOGE(" %s: failed to send kill diag_send_enable_qdss_req\n", __func__);
			return -1;
		}
		rt = wait_for_kill_response();
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag:%s time out while waiting for disable QDSS cmd response p_type: %d, pd: %d, peipheral:%d\n",
					__func__, peripheral_type, pd, peripheral);
			pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
			qdss_state = 0;
			return -1;
		}
		diag_qdss_reset_read_buffer();

		/*************************************
		*	Send command to diag to 		 *
		*	Clear STM Mask for the peripheral	 *
		**************************************/
		rt = diag_set_diag_transport(peripheral_type, peripheral, 0);
		if (rt == FALSE) {
			qdss_state = 0;
			DIAG_LOGE(" %s: failed to send kill diag_set_diag_transport\n", __func__);
			return -1;
		}
		rt = wait_for_kill_response();
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag:%s time out while waiting for clear QDSS cmd response for p_type: %d, pd: %d, peipheral:%d\n",
					__func__, peripheral_type, pd, peripheral);
			pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
			qdss_state = 0;
			return -1;
		}
		diag_qdss_reset_read_buffer();

		/*************************************
		 * Disable QDSS tracer				 *
		 *************************************/
		rt = diag_set_diag_qdss_tracer(peripheral_type, peripheral, 0);
		if (rt == FALSE) {
			qdss_state = 0;
			DIAG_LOGE(" %s: failed to send kill diag_set_diag_qdss_tracer\n", __func__);
			return -1;
		}
		rt = wait_for_kill_response();
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag:%s time out while waiting for clear diag qdss tracer cmd response for p_type:%d peipheral:%d\n",
					__func__, peripheral_type, peripheral);
			pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
			qdss_state = 0;
			return -1;
		}
		diag_qdss_reset_read_buffer();

		/*************************************
		 * Disable QDSS tracer DIAG entity	 *
		 *************************************/
		rt = diag_set_diag_qdss_diag_tracer(peripheral_type, peripheral, 0);
		if (rt == FALSE) {
			qdss_state = 0;
			DIAG_LOGE(" %s: failed to send kill diag_set_diag_qdss_diag_tracer\n", __func__);
			return -1;
		}
		rt = wait_for_kill_response();
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag:%s time out while waiting for clear qdss tracer diag entity cmd response for p_type: %d, pd: %d, peipheral:%d\n",
					__func__, peripheral_type, pd, peripheral);
			pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
			qdss_state = 0;
			return -1;
		}
		diag_qdss_reset_read_buffer();
	}

	/*********************************************
	*	Commands to reset QDSS environment sent	 *
	**********************************************/

	qdss_state = 0;
	return 0;
}

int diag_qdss_query_feature_mask(int peripheral_type)
{
	diag_feature_query_req *req;
	int offset = 0, length = 0, ret = 0;
	unsigned char* ptr = qdss_cmd_req_buf;

	if (!ptr)
		return FALSE;

	if (peripheral_type < MSM || peripheral_type > MDM_2) {
		DIAG_LOGE("diag:%s cmd sent failed for  peripheral_type = %d\n", __func__, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type) {
		*(int*)(ptr + offset) = -peripheral_type;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_feature_query_req*)ptr;

	/*************************************************
	 * 	4B 12 0225 X								 *
	 *												 *
	 * 	QUERY FEATURE MASK						 *
	 *************************************************/
	req->header.cmd_code = DIAG_SUBSYS_CMD_F;
	req->header.subsys_id = DIAG_SUBSYS_DIAG_SERV;
	req->header.subsys_cmd_code = DIAG_DIAG_FEATURE_QUERY;
	req->version = 1;
	length = sizeof(diag_feature_query_req);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}
	return TRUE;
}

int process_diag_feature_mask_response(int peripheral_type)
{
	diag_feature_query_rsp* rsp;
	unsigned char* buf_ptr;
	(void)peripheral_type;

	buf_ptr = &(qdss_read_buffer_pool[qdss_curr_write_idx].rsp_buf[0]);
	if (buf_ptr[0] == DIAG_BAD_CMD_F)
		return FALSE;
	else {
		rsp = (diag_feature_query_rsp*)buf_ptr;
		if ((rsp->version != 1) || (rsp->header.subsys_cmd_code != DIAG_DIAG_FEATURE_QUERY)) {
			return FALSE;
		}
		if (rsp->feature_mask[0] & ( 1 << 1)) {
			return TRUE;
		} else
			return FALSE;
	}
}
int diag_qdss_query_hw_accel(int peripheral_type)
{
	diag_hw_accel_query_cmd_req *req;
	int offset = 0, length = 0, ret = 0;
	unsigned char* ptr = qdss_cmd_req_buf;

	if (!ptr)
		return FALSE;

	if (peripheral_type < MSM || peripheral_type > MDM_2) {
		DIAG_LOGE("diag:%s cmd sent failed for  peripheral_type = %d\n", __func__,peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type) {
		*(int*)(ptr + offset) = -peripheral_type;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_hw_accel_query_cmd_req *)ptr;

	/*************************************************
	 * 	4B 12 0224 X								 *
	 *												 *
	 * 	QUERY FOR HW ACCEL SUPPORT						 *
	 *************************************************/
	req->header.cmd_code = DIAG_SUBSYS_CMD_F;
	req->header.subsys_id = DIAG_SUBSYS_DIAG_SERV;
	req->header.subsys_cmd_code = DIAG_HW_ACCEL_CMD;
	req->version = 1;
	req->op = DIAG_HW_ACCEL_OP_QUERY;
	req->hw_accel_type = DIAG_HW_ACCEL_TYPE_ALL;
	req->hw_accel_version = DIAG_HW_ACCEL_VER_MAX;
	length = sizeof(diag_hw_accel_query_cmd_req);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}
	return TRUE;
}
int process_diag_hw_accel_query_rsp(int peripheral_type)
{
	diag_hw_accel_cmd_query_resp_t *rsp = NULL;
	int i = 0;
	int mask = 0;
	unsigned char* buf_ptr;
	int diag_id_hw_accel_mask = 0;

	buf_ptr = &(qdss_read_buffer_pool[qdss_curr_write_idx].rsp_buf[0]);
	if (buf_ptr[0] == DIAG_BAD_CMD_F)
		return FALSE;
	else {
		rsp = (diag_hw_accel_cmd_query_resp_t*)buf_ptr;
		if ((rsp->version != 1) || (rsp->header.subsys_cmd_code != DIAG_HW_ACCEL_CMD)) {
			return FALSE;
		}
		trace_sink[peripheral_type] = rsp->query_rsp.diag_transport;
		for (i = DIAG_HW_ACCEL_TYPE_STM; i <= DIAG_HW_ACCEL_TYPE_MAX; i++) {
			diag_id_hw_accel_mask = 0x7fffffff;
			hw_accel_support[peripheral_type][i] = diag_id_hw_accel_mask & rsp->query_rsp.sub_query_rsp[i-1][DIAG_HW_ACCEL_VER_MAX - 1].diagid_mask_supported;
		}
		return mask;
	}
}

void* qdss_config_thread(void* param)
{
	(void)param;
	int rt, ret;

	while (1) {
		if (qdss_periph_info.peripheral_type & DIAG_MSM_MASK) {
			qdss_state = 1;
			diag_qdss_query_feature_mask(MSM);
			rt = wait_for_response();
			if (rt == ETIMEDOUT) {
				DIAG_LOGE("diag:%s time out while waiting for query feature mask response\n", __func__);
				pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
				qdss_state = 0;
				pthread_mutex_lock(&qdss_config_mutex);
				qdss_init_done = DIAG_COND_ERROR;
				pthread_cond_signal(&qdss_config_cond);
				pthread_mutex_unlock(&qdss_config_mutex);

				return 0;
			}
			ret = process_diag_feature_mask_response(MSM);
			diag_qdss_reset_read_buffer();
			if (ret) {
				diag_qdss_query_hw_accel(MSM);
				rt = wait_for_response();
				if (rt == ETIMEDOUT) {
					DIAG_LOGE("diag:%s time out while waiting for query hw accel cmd response\n", __func__);
					pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
					qdss_state = 0;
	                                pthread_mutex_lock(&qdss_config_mutex);
					qdss_init_done = DIAG_COND_ERROR;
					pthread_cond_signal(&qdss_config_cond);
					pthread_mutex_unlock(&qdss_config_mutex);

					return 0;
				}
				process_diag_hw_accel_query_rsp(MSM);
				diag_qdss_reset_read_buffer();
			}

			if (msm_peripheral_mask & DIAG_CON_MPSS) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_MODEM_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_MPSS;
				if (qdss_kill_thread == 1)
					return 0;
			}

			if (msm_peripheral_mask & DIAG_CON_LPASS) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_LPASS_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_LPASS;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_WCNSS) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_WCNSS_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_WCNSS;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_SENSORS) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_SENSORS_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_SENSORS;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_WDSP) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_WDSP_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_WDSP;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_CDSP) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_CDSP_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_CDSP;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_NPU) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_NPU_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_NPU;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_NSP1) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_NSP1_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_NSP1;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_GPDSP0) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_GPDSP0_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_GPDSP0;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_GPDSP1) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_GPDSP1_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_GPDSP1;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_APSS) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_APPS_PROC);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_APSS;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_UPD_WLAN) {
				diag_send_cmds_to_peripheral_init(MSM, UPD_WLAN);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_UPD_WLAN;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_UPD_AUDIO) {
				diag_send_cmds_to_peripheral_init(MSM, UPD_AUDIO);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_UPD_AUDIO;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_UPD_SENSORS) {
				diag_send_cmds_to_peripheral_init(MSM, UPD_SENSORS);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_UPD_SENSORS;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_UPD_CHARGER) {
				diag_send_cmds_to_peripheral_init(MSM, UPD_CHARGER);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_UPD_CHARGER;
				if (qdss_kill_thread == 1)
					return 0;
			}
			if (msm_peripheral_mask & DIAG_CON_UPD_OEM) {
				diag_send_cmds_to_peripheral_init(MSM, UPD_OEM);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_UPD_OEM;
				if (qdss_kill_thread == 1)
					return 0;
			}

			if (msm_peripheral_mask & DIAG_CON_UPD_OIS) {
				diag_send_cmds_to_peripheral_init(MSM, UPD_OIS);
				msm_peripheral_mask = msm_peripheral_mask ^ DIAG_CON_UPD_OIS;
				if (qdss_kill_thread == 1)
					return 0;
			}

			qdss_periph_info.peripheral_type = qdss_periph_info.peripheral_type ^ DIAG_MSM_MASK;

			/************************************************************************
			 * Open Device node after setting QDSS environment by sending commands	*
			 * Signal the condition to qdss_init for successful/failure case		*
			 ************************************************************************/

			pthread_mutex_lock(&qdss_config_mutex);
			if (diag_qdss_node_fd < 0) {
				if (use_etr1_buffer) {
					diag_qdss_node_fd = open("/dev/byte-cntr1", O_RDONLY);
					if (diag_qdss_node_fd == DIAG_INVALID_HANDLE) {
						DIAG_LOGE("diag: %s: Failed to open /dev/byte-cntr1 handle to qdss driver, error = %d\n", __func__, errno);
						diag_set_coresight_sysfs(ETR1_BLOCK_SIZE, "0", "byte_cntr1 - block_size", READ_WRITE);
						qdss_init_done = DIAG_COND_ERROR;
						pthread_cond_signal(&qdss_config_cond);
						pthread_mutex_unlock(&qdss_config_mutex);
						return 0;
					}
				} else {
					diag_qdss_node_fd = open("/dev/byte-cntr", O_RDONLY);
					if (diag_qdss_node_fd == DIAG_INVALID_HANDLE) {
						DIAG_LOGE("diag: %s: Failed to open /dev/byte-cntr handle to qdss driver, error = %d\n", __func__, errno);
						diag_set_coresight_sysfs(BLOCK_SIZE, "0", "byte_cntr - block_size", READ_WRITE);
						qdss_init_done = DIAG_COND_ERROR;
						pthread_cond_signal(&qdss_config_cond);
						pthread_mutex_unlock(&qdss_config_mutex);
						return 0;
					}
				}
			}
			qdss_init_done = DIAG_COND_COMPLETE;
			pthread_cond_signal(&qdss_config_cond);
			pthread_mutex_unlock(&qdss_config_mutex);
		}
		pthread_mutex_lock(&qdss_diag_mutex);
		while (!msm_peripheral_mask) {
			in_wait_for_qdss_peripheral_status = 1;
			pthread_cond_wait(&qdss_diag_cond, &qdss_diag_mutex);
			in_wait_for_qdss_peripheral_status = 0;
			if (qdss_kill_thread == 1) {
				pthread_mutex_unlock(&qdss_diag_mutex);
				return 0;
			}
		}
		pthread_mutex_unlock(&qdss_diag_mutex);
	}
}

void* qdss_config_thread_mdm(void* param)
{
	(void)param;
	int dev_idx;
	int peripheral_mask = 0;
	int rt, ret;

	while (1) {
		for (dev_idx = 1; dev_idx < NUM_PROC; dev_idx++) {
			if ((qdss_periph_info.peripheral_type & (1 << dev_idx)) &&
				(remote_mask & (1 << (dev_idx - 1)))) {

				if (dev_idx == 1) {
					if (diag_configure_mdm_proc(MHI_QDSS_MODE_UCI)) {
						pthread_mutex_lock(&qdss_mdm_config_mutex);
						qdss_init_done_mdm = DIAG_COND_ERROR;
						pthread_cond_signal(&qdss_mdm_config_cond);
						pthread_mutex_unlock(&qdss_mdm_config_mutex);
						return 0;
					}
				}

				peripheral_mask = mdm_peripheral_mask;
				qdss_state = 1;
				diag_qdss_query_feature_mask(dev_idx);
				rt = wait_for_response();
				if (rt == ETIMEDOUT) {
					DIAG_LOGE("diag:%s time out while waiting for query feature mask response\n", __func__);
					pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
					qdss_state = 0;
					return 0;
				}
				ret = process_diag_feature_mask_response(dev_idx);
				diag_qdss_reset_read_buffer();
				if (ret) {
					diag_qdss_query_hw_accel(dev_idx);
					rt = wait_for_response();
					if (rt == ETIMEDOUT) {
						DIAG_LOGE("diag:%s time out while waiting for query hw accel cmd response\n", __func__);
						pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
						qdss_state = 0;
						return 0;
					}
					process_diag_hw_accel_query_rsp(dev_idx);
					diag_qdss_reset_read_buffer();
				}

				if (peripheral_mask & DIAG_CON_MPSS) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_MODEM_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_MPSS;
					if (qdss_kill_thread == 1)
						return 0;
				}
				if (peripheral_mask & DIAG_CON_LPASS) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_LPASS_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_LPASS;
					if (qdss_kill_thread == 1)
						return 0;
				}
				if (peripheral_mask & DIAG_CON_WCNSS) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_WCNSS_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_WCNSS;
					if (qdss_kill_thread == 1)
						return 0;
				}
				if (peripheral_mask & DIAG_CON_SENSORS) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_SENSORS_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_SENSORS;
					if (qdss_kill_thread == 1)
						return 0;
				}
				if (peripheral_mask & DIAG_CON_WDSP) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_WDSP_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_WDSP;
					if (qdss_kill_thread == 1)
						return 0;
				}
				if (peripheral_mask & DIAG_CON_CDSP) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_CDSP_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_CDSP;
					if (qdss_kill_thread == 1)
						return 0;
				}
				if (peripheral_mask & DIAG_CON_NPU) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_NPU_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_NPU;
					if (qdss_kill_thread == 1)
						return 0;
				}
				if (peripheral_mask & DIAG_CON_NSP1) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_NSP1_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_NSP1;
					if (qdss_kill_thread == 1)
						return 0;
				}
				if (peripheral_mask & DIAG_CON_GPDSP0) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_GPDSP0_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_GPDSP0;
					if (qdss_kill_thread == 1)
						return 0;
				}
				if (peripheral_mask & DIAG_CON_GPDSP1) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_GPDSP1_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_GPDSP1;
					if (qdss_kill_thread == 1)
						return 0;
				}
				if (peripheral_mask & DIAG_CON_APSS) {
					diag_send_cmds_to_peripheral_init(dev_idx, DIAG_APPS_PROC);
					peripheral_mask = peripheral_mask ^ DIAG_CON_APSS;
					if (qdss_kill_thread == 1)
						return 0;
				}
				qdss_periph_info.peripheral_type = qdss_periph_info.peripheral_type ^ (1 << dev_idx);
				/************************************************************************
			 	* Open Device node after setting QDSS environment by sending commands	*
			 	* Signal the condition to qdss_init for successful/failure case		*
				************************************************************************/

				if (dev_idx == 1 && diag_qdss_node_fd_mdm < 0) {
					diag_qdss_node_fd_mdm = open("/dev/mhi_qdss", O_RDONLY);
					if (diag_qdss_node_fd_mdm == DIAG_INVALID_HANDLE) {
						DIAG_LOGE(" %s: Failed to open /dev/mhi_qdss handle to qdss driver, error = %d\n", __func__, errno);
						pthread_mutex_lock(&qdss_mdm_config_mutex);
						qdss_init_done_mdm = DIAG_COND_ERROR;
						pthread_cond_signal(&qdss_mdm_config_cond);
						pthread_mutex_unlock(&qdss_mdm_config_mutex);
						return 0;
					}
					DIAG_LOGE(" %s: Successful in opening /dev/mhi_qdss handle to qdss driver\n", __func__);
					if (qdss_mdm_down)
						qdss_mdm_down = 0;
					DIAG_LOGE("In %s: qdss_mdm_down: %d\n", __func__, qdss_mdm_down);
				}
			}
		}
		mdm_peripheral_mask = 0;
		pthread_mutex_lock(&qdss_mdm_config_mutex);
		qdss_init_done_mdm = DIAG_COND_COMPLETE;
		pthread_cond_signal(&qdss_mdm_config_cond);
		pthread_mutex_unlock(&qdss_mdm_config_mutex);
		pthread_mutex_lock(&qdss_mdm_diag_mutex);
		while (!mdm_peripheral_mask) {
			in_wait_for_qdss_mdm_status = 1;
			pthread_cond_wait(&qdss_mdm_diag_cond, &qdss_mdm_diag_mutex);
			in_wait_for_qdss_mdm_status = 0;
			if (qdss_kill_thread == 1) {
				pthread_mutex_unlock(&qdss_mdm_diag_mutex);
				return 0;
			}
		}
		pthread_mutex_unlock(&qdss_mdm_diag_mutex);
	}
	return 0;
}

void fill_qdss_buffer(void* ptr, int len, int type)
{
	unsigned char* buffer = NULL;
	unsigned int* bytes_in_buff = NULL;

	if (!type) {
		buffer = qdss_pools[qdss_curr_read].buffer_ptr[0];
		bytes_in_buff = &qdss_pools[qdss_curr_read].bytes_in_buff[0];
		if (!buffer || !bytes_in_buff)
			return;

		if (len >= (DISK_BUF_SIZE - *bytes_in_buff)) {
			pthread_mutex_lock(&qdss_set_data_ready_mutex);
			qdss_pools[qdss_curr_read].data_ready = 1;
			qdss_pools[qdss_curr_read].free = 0;
			pthread_cond_signal(&qdss_pools[qdss_curr_read].write_cond);
			pthread_mutex_unlock(&qdss_set_data_ready_mutex);

			if (!qdss_curr_read)
				qdss_curr_read = 1;
			else
				qdss_curr_read = 0;

			pthread_mutex_lock(&qdss_clear_data_ready_mutex);
			if (qdss_pools[qdss_curr_read].data_ready) {
				qdss_in_read = 1;
				pthread_cond_wait(&(qdss_pools[qdss_curr_read].read_cond),
								  &qdss_clear_data_ready_mutex);
				qdss_in_read = 0;
			}
			pthread_mutex_unlock(&qdss_clear_data_ready_mutex);
			buffer = qdss_pools[qdss_curr_read].buffer_ptr[0];
			bytes_in_buff =
			&qdss_pools[qdss_curr_read].bytes_in_buff[0];
		}
			if (len > 0) {
				memcpy(buffer + *bytes_in_buff, ptr, len);
				*bytes_in_buff += len;
			}
	}	else {
		buffer = qdss_pools_mdm[qdss_curr_read_mdm].buffer_ptr[0];
		bytes_in_buff = &qdss_pools_mdm[qdss_curr_read_mdm].bytes_in_buff[0];
		if (!buffer || !bytes_in_buff)
			return;
		if (len >= (DISK_BUF_SIZE - *bytes_in_buff)) {
			pthread_mutex_lock(&qdss_mdm_set_data_ready_mutex);
			qdss_pools_mdm[qdss_curr_read_mdm].data_ready = 1;
			qdss_pools_mdm[qdss_curr_read_mdm].free = 0;
			pthread_cond_signal(&qdss_pools_mdm[qdss_curr_read_mdm].write_cond);
			pthread_mutex_unlock(&qdss_mdm_set_data_ready_mutex);

			if (!qdss_curr_read_mdm)
				qdss_curr_read_mdm = 1;
			else
				qdss_curr_read_mdm = 0;

			pthread_mutex_lock(&qdss_mdm_clear_data_ready_mutex);
			if (qdss_pools_mdm[qdss_curr_read_mdm].data_ready) {
				qdss_in_read_mdm = 1;
				pthread_cond_wait(&(qdss_pools_mdm[qdss_curr_read_mdm].read_cond),
								  &qdss_mdm_clear_data_ready_mutex);
				qdss_in_read_mdm = 0;
			}
			pthread_mutex_unlock(&qdss_mdm_clear_data_ready_mutex);
			buffer = qdss_pools_mdm[qdss_curr_read_mdm].buffer_ptr[0];
			bytes_in_buff =
			&qdss_pools_mdm[qdss_curr_read_mdm].bytes_in_buff[0];
		}
		if (len > 0) {
			memcpy(buffer + *bytes_in_buff, ptr, len);
			*bytes_in_buff += len;
		}
	}
}

void sig_dummy_handler(int signal)
{
	(void)signal;
}

static void* qdss_read_thread(void* param)
{
	int num_bytes_read = 0, type = 0, rc;
	(void) param;
	sigset_t set_1, set_2;
	struct  sigaction sact;

	sigemptyset( &sact.sa_mask );
	sact.sa_flags = 0;
	sact.sa_handler = sig_dummy_handler;
	sigaction(SIGUSR2, &sact, NULL);

	if ((sigemptyset((sigset_t *) &set_1) == -1) ||
		(sigaddset(&set_1, SIGUSR2) == -1))
		DIAG_LOGE("diag: Failed to initialize block set\n");

	rc = pthread_sigmask(SIG_UNBLOCK, &set_1, NULL);
	if (rc != 0)
		DIAG_LOGE("diag: Failed to unblock signal for qdss read thread\n");

	if ((sigemptyset((sigset_t *) &set_2) == -1) ||
		(sigaddset(&set_2, SIGTERM) == -1) ||
		(sigaddset(&set_2, SIGHUP) == -1) ||
		(sigaddset(&set_2, SIGUSR1) == -1) ||
		(sigaddset(&set_2, SIGINT) == -1))
		DIAG_LOGE("diag:%s: Failed to initialize block set\n", __func__);

	rc = pthread_sigmask(SIG_BLOCK, &set_2, NULL);
	if (rc != 0)
		DIAG_LOGE("diag:%s: Failed to block signal for qdss read thread\n", __func__);
	do {
		if ((qdss_init_done != DIAG_COND_COMPLETE) || diag_qdss_node_fd == DIAG_INVALID_HANDLE)
			continue;
		in_qdss_read = 1;
		num_bytes_read = read(diag_qdss_node_fd, (void*)qdss_read_buffer,
							  READ_BUF_SIZE);
		in_qdss_read = 0;
		if (num_bytes_read > READ_BUF_SIZE)
			continue;
		if (num_bytes_read <= 0) {
			num_bytes_read = errno;
			if ((qdss_kill_thread == 1) &&
				(qdss_kill_rw_thread >= QDSS_KILL_STATE_ON)) {
				if (num_bytes_read == EINVAL) {
					DIAG_LOGD("diag: %s: Received -EINVAL\n", __func__);
				} else {
					DIAG_LOGD("diag: %s: Received error on read: %d\n", __func__, errno);
				}
				pthread_mutex_lock(&qdss_kill_mutex);
				pthread_cond_signal(&qdss_kill_cond);
				pthread_mutex_unlock(&qdss_kill_mutex);
				if (use_etr1_buffer)
					diag_set_coresight_sysfs(ETR1_BLOCK_SIZE, "0", "byte_cntr1 - block_size", READ_WRITE);
				else
					diag_set_coresight_sysfs(BLOCK_SIZE, "0", "byte_cntr - block_size", READ_WRITE);
				DIAG_LOGD("diag: %s: Exit read thread once QDSS exit signalled\n",
					__func__);
				return 0;
			}
			continue;
		}

		fill_qdss_buffer(qdss_read_buffer, num_bytes_read, type);
		num_bytes_read = 0;
		memset(qdss_read_buffer, 0, READ_BUF_SIZE);

		if ((qdss_kill_thread == 1) && (qdss_kill_rw_thread == QDSS_KILL_STATE_DONE)) {
			DIAG_LOGD("diag: %s: Exit read thread\n", __func__);
			return 0;
		}
	} while (1);

	return 0;
}
static void* qdss_read_thread_mdm(void* param)
{
	int num_bytes_read = 0, type = 1, rc;
	(void) param;
	sigset_t set_1, set_2;
	struct  sigaction sact;

	sigemptyset( &sact.sa_mask );
	sact.sa_flags = 0;
	sact.sa_handler = sig_dummy_handler;
	sigaction(SIGUSR2, &sact, NULL);

	if ((sigemptyset((sigset_t *) &set_1) == -1) ||
		(sigaddset(&set_1, SIGUSR2) == -1))
		DIAG_LOGE("diag: Failed to initialize block set\n");

	rc = pthread_sigmask(SIG_UNBLOCK, &set_1, NULL);
	if (rc != 0)
		DIAG_LOGE("diag: Failed to unblock signal for qdss read thread mdm\n");

	if ((sigemptyset((sigset_t *) &set_2) == -1) ||
		(sigaddset(&set_2, SIGTERM) == -1) ||
		(sigaddset(&set_2, SIGHUP) == -1) ||
		(sigaddset(&set_2, SIGUSR1) == -1) ||
		(sigaddset(&set_2, SIGINT) == -1))
		DIAG_LOGE("diag:%s: Failed to initialize block set\n", __func__);

	rc = pthread_sigmask(SIG_BLOCK, &set_2, NULL);
	if (rc != 0)
		DIAG_LOGE("diag:%s: Failed to block signal for qdss read thread mdm\n", __func__);
	do {
		if (qdss_mdm_down || (qdss_init_done_mdm != DIAG_COND_COMPLETE)
			|| diag_qdss_node_fd_mdm == DIAG_INVALID_HANDLE)
			continue;
		in_qdss_read_mdm = 1;
		num_bytes_read = read(diag_qdss_node_fd_mdm, (void*)qdss_read_buffer_mdm,
							  READ_BUF_SIZE);
		in_qdss_read_mdm = 0;
		if (qdss_mdm_down || num_bytes_read == -ERESTARTSYS) {
			if (num_bytes_read == -ERESTARTSYS) {
				DIAG_LOGD("diag: %s, num_bytes_read: ERESTARTSYS\n", __func__);
				qdss_close_qdss_node_mdm();
			}
			DIAG_LOGD("diag: %s, qdss_mdm_down: qdss_mdm_down: %d\n", __func__, qdss_mdm_down);
			pthread_mutex_lock(&qdss_mdm_down_mutex);
			in_wait_for_qdss_mdm_up_status = 1;
			pthread_cond_wait(&qdss_mdm_down_cond, &qdss_mdm_down_mutex);
			in_wait_for_qdss_mdm_up_status = 0;
			pthread_mutex_unlock(&qdss_mdm_down_mutex);
		}

		if (num_bytes_read <= 0) {
			if (qdss_kill_thread == 1) {
				DIAG_LOGD("diag: %s, Exit read thread for invalid read length: num_bytes_read: %d\n",
					__func__, num_bytes_read);
				return 0;
			}
			continue;
		}
		fill_qdss_buffer(qdss_read_buffer_mdm, num_bytes_read, type);
		num_bytes_read = 0;
		memset(qdss_read_buffer_mdm, 0, READ_BUF_SIZE);

		if (qdss_kill_thread == 1) {
			DIAG_LOGD("diag: In %s, Exit read thread for mdm\n", __func__);
			return 0;
		}
	} while (1);

	return 0;
}

static void close_qdss_logging_file(int type)
{
	if (qdss_diag_fd_md[type] > 0)
		close(qdss_diag_fd_md[type]);
	qdss_diag_fd_md[type] = -1;

	if (rename_file_names && qdss_file_name_curr[type][0] != '\0') {
		int status;
		char timestamp_buf[30];
		char new_filename[FILE_NAME_LEN];
		char rename_cmd[RENAME_CMD_LEN];

		get_time_string(timestamp_buf, sizeof(timestamp_buf));

		(void)std_strlprintf(new_filename,
					 FILE_NAME_LEN, "%s%s%s%s%s%s",
					 output_dir[type], "/diag_qdss_log",
					 qdss_peripheral_name, "_",
					 timestamp_buf,
					 (use_qmdl2_v2) ? ".qdss" : ".bin");

		/* Create rename command and issue it */
		(void)std_strlprintf(rename_cmd, RENAME_CMD_LEN, "mv %s %s",
							 qdss_file_name_curr[type], new_filename);

		status = system(rename_cmd);
		if (status == -1) {
			DIAG_LOGE("diag: In %s, File rename error (mv), errno: %d\n",
					  __func__, errno);
			DIAG_LOGE("diag: Unable to rename file %s to %s\n",
					  qdss_file_name_curr[type], new_filename);
		} else {
			/* Update current filename */
			strlcpy(qdss_file_name_curr[type], new_filename, FILE_NAME_LEN);
		}
	}
}

void diag_set_qdss_mask(unsigned int diag_qdss_mask, unsigned int diag_device_mask)
{
	qdss_mask = diag_qdss_mask;
	device_mask = diag_device_mask;
}

void signal_writing_qdss_header(int p_type)
{
	if (p_type != MSM && p_type != MDM)
		return;

	pthread_mutex_lock(&diagid_guid_map_mutex[p_type]);
	pthread_cond_signal(&diagid_guid_map_cond[p_type]);
	pthread_mutex_unlock(&diagid_guid_map_mutex[p_type]);
}

static void wait_for_diagid_guid_map_completion(int type)
{
	struct timespec time;
	struct timeval now;
	int rt = 0;

	gettimeofday(&now, NULL);
	time.tv_sec = now.tv_sec;
	time.tv_nsec = (now.tv_usec * 1000) + (800 * 1000 * 1000);
	pthread_mutex_lock(&diagid_guid_map_mutex[type]);
	if (!diagid_guid_status[type]) {
		rt = pthread_cond_timedwait(&diagid_guid_map_cond[type], &diagid_guid_map_mutex[type], &time);
		if (rt == ETIMEDOUT) {
			DIAG_LOGE("diag: %s: Timeout while waiting for completion of diagid-guid mapping for type: %d\n", __func__, type);
			DIAG_LOGE("diag: %s: Continuing to write qdss header without diagid-guid mapping for type: %d\n", __func__, type);
		}
	}
	pthread_mutex_unlock(&diagid_guid_map_mutex[type]);
}

#define S_64K (64*1024)

static int diag_qdss_create_file(int type)
{
	unsigned long *qdss_written_bytes;
	int *qdss_diag_fd;
	int bin_or_qdss_cond;
	int ret = 0;
	char timestamp_buf[30];

	if (qdss_diag_fd_md[type] >= 0)
		return 0;

	if (type == MSM) {
		qdss_diag_fd = &qdss_diag_fd_dev;
		qdss_written_bytes = &qdss_count_written_bytes;
	} else {
		qdss_diag_fd = &qdss_diag_fd_dev_mdm;
		qdss_written_bytes = &qdss_count_written_bytes_mdm;
	}

	/* Check if we are to start circular logging on the basis of maximum
	 * number of logging files in the logging directories on the SD card
	 */
	if (max_file_num > 1 && (qdss_file_count[type] >= max_file_num)) {
		DIAG_LOGE("diag: %s: File count reached max file num %u so deleting oldest file\n",
			  __func__, max_file_num);
		ret = -1;
		if (!delete_qdss_log(type)) {
			qdss_file_count[type]--;
			ret = 0;
		}
		if (ret) {
			DIAG_LOGE("diag: qdss file delete for type: %d failed\n", type);
			return ret;
		}
	}

	if (use_qmdl2_v2)
		wait_for_diagid_guid_map_completion(type);

	bin_or_qdss_cond = use_qmdl2_v2 && diagid_guid_status[type];
	get_time_string(timestamp_buf, sizeof(timestamp_buf));
	(void)std_strlprintf(qdss_file_name_curr[type], FILE_NAME_LEN, "%s%s%s%s%s%s",
			     output_dir[type], "/diag_qdss_log", qdss_peripheral_name, "_",
			     timestamp_buf, bin_or_qdss_cond ? ".qdss" : ".bin");
	qdss_diag_fd_md[type] = open(qdss_file_name_curr[type], O_CREAT | O_RDWR, 0644);
	*qdss_diag_fd = qdss_diag_fd_md[type];
	if (*qdss_diag_fd < 0) {
		DIAG_LOGE("diag: File open error, please check");
		DIAG_LOGE("diag: memory device %d, errno: %d \n", fd_md[type], errno);
		return errno;
	}

	DIAG_LOGE("creating new file %s \n", qdss_file_name_curr[type]);
	qdss_file_count[type]++;

	if (bin_or_qdss_cond) {
		ret = write_qdss_header(*qdss_diag_fd, type);
		if (ret <= 0) {
			DIAG_LOGE("diag: Failed to write header to QDSS file (%d)\n", ret);
			return ret;
		}

		*qdss_written_bytes += ret;
	}

	return 0;
}

static void write_to_qdss_file(void *buf, int len, int type)
{
	struct stat logging_file_stat;
	int rc, ret;

	if ((qdss_count_written_bytes + len >= max_file_size) ||
		(qdss_count_written_bytes + len > MDLOG_WRITTEN_BYTES_LIMIT)) {
		close_qdss_logging_file(0);
		qdss_diag_fd_dev = qdss_diag_fd_md[type];
		qdss_count_written_bytes = 0;
	}

	ret = diag_qdss_create_file(type);
	if (ret)
		return;

	if (qdss_diag_fd_dev != -1) {
		if (!stat(qdss_file_name_curr[type], &logging_file_stat)) {
			ret = write(qdss_diag_fd_dev, (const void *)buf, len);
			if (ret > 0) {
				qdss_count_written_bytes = qdss_count_written_bytes + len;
			} else {
				DIAG_LOGE("diag: In %s, error writing to sd card, %s, errno: %d\n",
						  __func__, strerror(errno), errno);
				if (errno == ENOSPC) {
				/* Delete oldest file */
					DIAG_LOGE("diag: %s: No space left so deleting oldest file\n",
							__func__);
					rc = -1;
					if (!delete_qdss_log(type)) {
						qdss_file_count[type]--;
						rc = 0;
					}

					if (rc) {
						DIAG_LOGE("qdss file delete for type: %d failed while no space\n", type);
						return;
					}

					/* Close file if it is big enough */
					if (qdss_count_written_bytes >
						min_file_size) {
						close_qdss_logging_file(type);
						qdss_diag_fd_dev = qdss_diag_fd_md[type];
						qdss_count_written_bytes = 0;
					} else {
						DIAG_LOGE(" Disk Full "
								  "Continuing with "
								  "same file [%d] \n", type);
					}
					write_to_qdss_file(buf, len,
								type);
					return;
				} else {
					DIAG_LOGE(" failed to write "
							 "to file, device may"
							 " be absent, errno: %d\n",
							 errno);
				}
			}
		} else {
			close(qdss_diag_fd_dev);
			qdss_diag_fd_md[type] = -1;
			ret = -EINVAL;
		}
	}
}

static void write_to_qdss_file_mdm(void *buf, int len, int type) {
	struct stat logging_file_stat;
	int rc, ret;

	if ((qdss_count_written_bytes_mdm + len >= max_file_size) ||
		(qdss_count_written_bytes_mdm + len > MDLOG_WRITTEN_BYTES_LIMIT)) {
		close_qdss_logging_file(type);
		qdss_diag_fd_dev_mdm = qdss_diag_fd_md[type];
		qdss_count_written_bytes_mdm = 0;
	}

	ret = diag_qdss_create_file(type);
	if (ret)
		return;

	if (qdss_diag_fd_dev_mdm != -1) {
		if (!stat(qdss_file_name_curr[type], &logging_file_stat)) {
			ret = write(qdss_diag_fd_dev_mdm, (const void *)buf, len);
			if (ret > 0) {
				qdss_count_written_bytes_mdm = qdss_count_written_bytes_mdm + len;
			} else {
				DIAG_LOGE("diag: In %s, error writing to sd card, %s, errno: %d\n",
						  __func__, strerror(errno), errno);
				if (errno == ENOSPC) {
				/* Delete oldest file */
					DIAG_LOGE("diag: %s: No space left so deleting oldest file\n",
						__func__);
					rc = -1;
					if (!delete_qdss_log(type)) {
						qdss_file_count[type]--;
						rc = 0;
					}

					if (rc) {
						DIAG_LOGE("qdss file delete for type: %d failed while no space\n", type);
						return;
					}
					/* Close file if it is big enough */
					if (qdss_count_written_bytes_mdm >
						min_file_size) {
						close_qdss_logging_file(type);
						qdss_diag_fd_dev_mdm = qdss_diag_fd_md[type];
						qdss_count_written_bytes_mdm = 0;
					} else {
						DIAG_LOGE(" Disk Full "
								  "Continuing with "
								  "same file [%d] \n", type);
					}
					write_to_qdss_file_mdm(buf, len,
								type);
					return;
				} else {
					DIAG_LOGE(" failed to write "
						 "to file, device may"
						 " be absent, errno: %d\n",
						 errno);
				}
			}
		} else {
			close(qdss_diag_fd_dev_mdm);
			qdss_diag_fd_md[type] = -1;
			ret = -EINVAL;
		}
	}
}

static void* qdss_write_thread(void* param) {
	unsigned int i;
	int z = 0, type = 0;
	unsigned int chunks, last_chunk;
	unsigned char *temp_ptr = NULL;;
	(void)param;

	while (1) {
		if ((qdss_kill_thread == 1) && (qdss_kill_rw_thread == QDSS_KILL_STATE_DONE)) {
			DIAG_LOGD("diag: %s, exiting write thread for MSM due to kill thread: %d\n",
				__func__, qdss_kill_thread);
			return NULL;
		}

		if (qdss_curr_write != 0 && qdss_curr_write != 1) {
			DIAG_LOGD("diag: %s: write thread exit due to invalid qdss_curr_write\n",
				__func__);
			return NULL;
		}

		temp_ptr = qdss_pools[qdss_curr_write].buffer_ptr[0];
		if (!temp_ptr) {
			DIAG_LOGD("diag: %s: write thread exit due to invalid buffer_ptr\n",
				__func__);
			return NULL;
		}

		pthread_mutex_lock(&qdss_set_data_ready_mutex);
		if (!qdss_pools[qdss_curr_write].data_ready) {
			qdss_in_write = 1;
			pthread_cond_wait(&(qdss_pools[qdss_curr_write].write_cond),
							  &qdss_set_data_ready_mutex);
			qdss_in_write = 0;
		}
		pthread_mutex_unlock(&qdss_set_data_ready_mutex);

final_chunk:
		qdss_write_in_progress = 1;

		chunks = qdss_pools[qdss_curr_write].bytes_in_buff[0] /
										S_64K;
		last_chunk = qdss_pools[qdss_curr_write].bytes_in_buff[z] %
										S_64K;
		for (i = 0; i < chunks; i++) {
			write_to_qdss_file(
						qdss_pools[qdss_curr_write].buffer_ptr[0],
						S_64K, type);
			qdss_pools[qdss_curr_write].buffer_ptr[0] +=
			S_64K;
		}
		if (last_chunk > 0)
			write_to_qdss_file(
						qdss_pools[qdss_curr_write].buffer_ptr[z],
						last_chunk, type);
		qdss_write_in_progress = 0;

		/* File pool structure */
		pthread_mutex_lock(&qdss_clear_data_ready_mutex);
		qdss_pools[qdss_curr_write].data_ready = 0;
		qdss_pools[qdss_curr_write].bytes_in_buff[0] = 0;
		qdss_pools[qdss_curr_write].buffer_ptr[0] = temp_ptr;
		qdss_pools[qdss_curr_write].free = 1;
		/* Free Read thread if waiting on same buffer */
		pthread_cond_signal(&(qdss_pools[qdss_curr_write].read_cond));
		pthread_mutex_unlock(&qdss_clear_data_ready_mutex);

		if (!qdss_curr_write)
			qdss_curr_write = 1;
		else
			qdss_curr_write = 0;

		if ((qdss_kill_thread == 1) && (qdss_kill_rw_thread == QDSS_KILL_STATE_DONE)) {
			if (qdss_pools[qdss_curr_write].bytes_in_buff[0] > 0) {
				DIAG_LOGD("diag: %s: Draining final_chunk: %d of data\n", __func__,
					qdss_pools[qdss_curr_write].bytes_in_buff[0]);
				goto final_chunk;
			}
			DIAG_LOGD("diag: %s: Exit write thread after write completion\n", __func__);
			return 0;
		}
	}
	return NULL;
}

static void* qdss_write_thread_mdm(void* param) {
	unsigned int i;
	unsigned int chunks, last_chunk;
	unsigned char *temp_ptr = NULL;
	int type = 1;
	(void)param;

	while (1) {
		if (qdss_kill_thread == 1) {
			DIAG_LOGD("diag: %s, exiting write thread for mdm due to kill thread: %d\n",
				__func__, qdss_kill_thread);
			return NULL;
		}

		if (qdss_curr_write_mdm != 0 && qdss_curr_write_mdm != 1)
			return NULL;

		temp_ptr = qdss_pools_mdm[qdss_curr_write_mdm].buffer_ptr[0];
		if (!temp_ptr || (qdss_kill_thread == 1))
			return NULL;

		pthread_mutex_lock(&qdss_mdm_set_data_ready_mutex);
		if (!qdss_pools_mdm[qdss_curr_write_mdm].data_ready) {
			qdss_in_write_mdm = 1;
			pthread_cond_wait(&(qdss_pools_mdm[qdss_curr_write_mdm].write_cond),
							  &qdss_mdm_set_data_ready_mutex);
			qdss_in_write_mdm = 0;
		}
		pthread_mutex_unlock(&qdss_mdm_set_data_ready_mutex);

		qdss_write_in_progress_mdm = 1;

		chunks = qdss_pools_mdm[qdss_curr_write_mdm].bytes_in_buff[0] /
										S_64K;
		last_chunk = qdss_pools_mdm[qdss_curr_write_mdm].bytes_in_buff[0] %
										S_64K;
		for (i = 0; i < chunks; i++) {
			write_to_qdss_file_mdm(
						qdss_pools_mdm[qdss_curr_write_mdm].buffer_ptr[0],
						S_64K, type);
			qdss_pools_mdm[qdss_curr_write_mdm].buffer_ptr[0] +=
			S_64K;
		}
		if (last_chunk > 0)
			write_to_qdss_file_mdm(
						qdss_pools_mdm[qdss_curr_write_mdm].buffer_ptr[0],
						last_chunk, type);
		qdss_write_in_progress_mdm = 0;

		/* File pool structure */
		pthread_mutex_lock(&qdss_mdm_clear_data_ready_mutex);
		qdss_pools_mdm[qdss_curr_write_mdm].data_ready = 0;
		qdss_pools_mdm[qdss_curr_write_mdm].bytes_in_buff[0] = 0;
		qdss_pools_mdm[qdss_curr_write_mdm].buffer_ptr[0] = temp_ptr;
		qdss_pools_mdm[qdss_curr_write_mdm].free = 1;
		/* Free Read thread if waiting on same buffer */
		pthread_cond_signal(&(qdss_pools_mdm[qdss_curr_write_mdm].read_cond));
		pthread_mutex_unlock(&qdss_mdm_clear_data_ready_mutex);

		if (!qdss_curr_write_mdm)
			qdss_curr_write_mdm = 1;
		else
			qdss_curr_write_mdm = 0;

		if (qdss_kill_thread == 1) {
			DIAG_LOGD("diag: In %s: Exit write thread for mdm after write completion\n", __func__);
			return 0;
		}
	}
	return NULL;
}

static int check_for_qdss_cmd(uint8* src_ptr)
{
	uint16 cmd_code = 0;
	if (!src_ptr)
		return FALSE;

	if (((*src_ptr == DIAG_SUBSYS_CMD_F && *(src_ptr + 1) == DIAG_SUBSYS_DIAG_SERV) ||
		 (((*src_ptr == DIAG_BAD_CMD_F) || (*src_ptr == DIAG_BAD_LEN_F) || (*src_ptr == DIAG_BAD_PARM_F)) &&
		 *(src_ptr + 1) == DIAG_SUBSYS_CMD_F && *(src_ptr + 2) == DIAG_SUBSYS_DIAG_SERV)) ||
		((*src_ptr == DIAG_SUBSYS_CMD_F && *(src_ptr + 1) == DIAG_SUBSYS_QDSS) ||
		 (((*src_ptr == DIAG_BAD_CMD_F) || (*src_ptr == DIAG_BAD_LEN_F) || (*src_ptr == DIAG_BAD_PARM_F)) &&
		 *(src_ptr + 1) == DIAG_SUBSYS_CMD_F && *(src_ptr + 2) == DIAG_SUBSYS_QDSS)))
	{
		if (*src_ptr == DIAG_SUBSYS_CMD_F) {
			memcpy(&cmd_code, src_ptr + 2, sizeof(cmd_code));
		} else {
			memcpy(&cmd_code,src_ptr + 3, sizeof(cmd_code));
		}

		switch (cmd_code) {
		case DIAG_DIAG_STM:
			break;
		case DIAG_QDSS_FILTER_STM:
			break;
		case DIAG_QDSS_TRACE_SINK:
			break;
		case DIAG_DIAG_FEATURE_QUERY:
			break;
		case DIAG_HW_ACCEL_CMD:
			break;
		case DIAG_QDSS_ETR1_TRACE_SINK:
			break;
		case 0x206:
			break;
		case 0x208:
			break;
		case 0x506:
			break;
		case 0x508:
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
	else
		return FALSE;
}

static void request_qdss_read_buffer(void)
{
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_mutex));
	if (qdss_read_buffer_pool[qdss_curr_read_idx].data_ready) {
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
		pthread_cond_wait(&(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_cond),
						  &(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_mutex));
		pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
	}
	pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_mutex));
}

int parse_data_for_qdss_rsp(uint8* ptr, int count_received_bytes, int index)
{
	uint8_t* src_ptr = NULL;
	unsigned char* dest_ptr = NULL;
	unsigned int src_length = 0, dest_length = 0;
	unsigned int len = 0;
	unsigned int i;
	uint8_t src_byte;
	int bytes_read = 0;
	uint16_t payload_len = 0;

	if (!ptr)
		return -1;

	while (bytes_read < count_received_bytes) {

		src_ptr = ptr + bytes_read;
		src_length = count_received_bytes - bytes_read;

		if (hdlc_disabled[index]) {
			payload_len = *(uint16_t *)(src_ptr + 2);
			if (check_for_qdss_cmd(src_ptr + 4))
			{
				request_qdss_read_buffer();
				dest_ptr = &(qdss_read_buffer_pool[qdss_curr_read_idx].rsp_buf[0]);
				dest_length = QDSS_RSP_BUF_SIZE;
				if (payload_len <= QDSS_RSP_BUF_SIZE)
					memcpy(dest_ptr, src_ptr + 4, payload_len);
				else
					return -1;
				qdss_read_buffer_pool[qdss_curr_read_idx].data_ready = 1;
				pthread_cond_signal(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_cond));
				pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
				if (!qdss_curr_read_idx)
					qdss_curr_read_idx = 1;
				else
					qdss_curr_read_idx = 0;
				bytes_read += payload_len + 5;

			}
			else
				bytes_read += payload_len + 5;

		} else {
			if (check_for_qdss_cmd(src_ptr)) {
				request_qdss_read_buffer();
				dest_ptr = &(qdss_read_buffer_pool[qdss_curr_read_idx].rsp_buf[0]);
				dest_length = QDSS_RSP_BUF_SIZE;
				for (i = 0; i < src_length; i++) {
					src_byte = src_ptr[i];

					if (src_byte == ESC_CHAR) {
						if (i == (src_length - 1)) {
							i++;
							break;
						} else {
							dest_ptr[len++] = src_ptr[++i]
								^ 0x20;
						}
					} else if (src_byte == CTRL_CHAR) {
						if (i == 0 && src_length > 1)
							continue;
						dest_ptr[len++] = src_byte;
						i++;
						break;
					} else {
						dest_ptr[len++] = src_byte;
					}

					if (len >= dest_length) {
						i++;
						break;
					}
				}
				bytes_read += i;
				i = 0;
				len = 0;
				qdss_read_buffer_pool[qdss_curr_read_idx].data_ready = 1;
				pthread_cond_signal(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_cond));
				pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
				if (!qdss_curr_read_idx)
					qdss_curr_read_idx = 1;
				else
					qdss_curr_read_idx = 0;
			} else {
				for (i = 0; i < src_length; i++) {
					if (src_ptr[i] == CTRL_CHAR) {
						i++;
						break;
					}
				}
				bytes_read += i;
				i = 0;
				len = 0;

			}
		}
	}
	return 0;
}

void diag_kill_qdss_threads(void)
{
	int ret = 0, local_remote_mask, local_device_mask = 0;
	int dev_idx, local_qdss_mask = 0;
	uint16 z = 1, proc_type;
	struct timespec time;
	struct timeval now;

	if (!qdss_mask)
		return;

	local_qdss_mask = qdss_mask;

	/****************************
	 * Signal the config thread *
	 ****************************/

	qdss_kill_thread = 1;
	qdss_kill_rw_thread = QDSS_KILL_STATE_OFF;
	DIAG_LOGE("diag: %s: Initiate qdss threads kill (qdss_kill_thread: %d)\n",
		__func__, qdss_kill_thread);

	/****************************************************
	 * Reset the QDSS environment for the peripheral	*
	 ****************************************************/
	 if (p_type_mask & DIAG_MSM_MASK) {
		if (qdss_mask & DIAG_CON_MPSS) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_MODEM_PROC);
			qdss_mask ^= DIAG_CON_MPSS;
		}
		if (qdss_mask & DIAG_CON_LPASS) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_LPASS_PROC);
			qdss_mask ^= DIAG_CON_LPASS;
		}
		if (qdss_mask & DIAG_CON_WCNSS) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_WCNSS_PROC);
			qdss_mask ^= DIAG_CON_WCNSS;
		}
		if (qdss_mask & DIAG_CON_SENSORS) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_SENSORS_PROC);
			qdss_mask ^= DIAG_CON_SENSORS;
		}
		if (qdss_mask & DIAG_CON_WDSP) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_WDSP_PROC);
			qdss_mask ^= DIAG_CON_WDSP;
		}
		if (qdss_mask & DIAG_CON_CDSP) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_CDSP_PROC);
			qdss_mask ^= DIAG_CON_CDSP;
		}
		if (qdss_mask & DIAG_CON_NPU) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_NPU_PROC);
			qdss_mask ^= DIAG_CON_NPU;
		}
		if (qdss_mask & DIAG_CON_NSP1) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_NSP1_PROC);
			qdss_mask ^= DIAG_CON_NSP1;
		}
		if (qdss_mask & DIAG_CON_GPDSP0) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_GPDSP0_PROC);
			qdss_mask ^= DIAG_CON_GPDSP0;
		}
		if (qdss_mask & DIAG_CON_GPDSP1) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_GPDSP1_PROC);
			qdss_mask ^= DIAG_CON_GPDSP1;
		}
		if (qdss_mask & DIAG_CON_APSS) {
			diag_send_cmds_to_peripheral_kill(MSM, DIAG_APPS_PROC);
			qdss_mask ^= DIAG_CON_APSS;
		}
		if (qdss_mask & DIAG_CON_UPD_WLAN) {
			diag_send_cmds_to_peripheral_kill(MSM, UPD_WLAN);
			qdss_mask ^= DIAG_CON_UPD_WLAN;
		}
		if (qdss_mask & DIAG_CON_UPD_AUDIO) {
			diag_send_cmds_to_peripheral_kill(MSM, UPD_AUDIO);
			qdss_mask ^= DIAG_CON_UPD_AUDIO;
		}
		if (qdss_mask & DIAG_CON_UPD_SENSORS) {
			diag_send_cmds_to_peripheral_kill(MSM, UPD_SENSORS);
			qdss_mask ^= DIAG_CON_UPD_SENSORS;
		}
		if (qdss_mask & DIAG_CON_UPD_CHARGER) {
			diag_send_cmds_to_peripheral_kill(MSM, UPD_CHARGER);
			qdss_mask ^= DIAG_CON_UPD_CHARGER;
		}
		if (qdss_mask & DIAG_CON_UPD_OEM) {
			diag_send_cmds_to_peripheral_kill(MSM, UPD_OEM);
			qdss_mask ^= DIAG_CON_UPD_OEM;
		}
		if (qdss_mask & DIAG_CON_UPD_OIS) {
			diag_send_cmds_to_peripheral_kill(MSM, UPD_OIS);
			qdss_mask ^= DIAG_CON_UPD_OIS;
		}

		/****************************
		* Kill the config thread 	*
		****************************/

		if (in_wait_for_qdss_peripheral_status)
			pthread_cond_signal(&qdss_diag_cond);

		ret = pthread_join(qdss_config_thread_hdl, NULL);
		if (ret != 0) {
			DIAG_LOGE("diag: In %s, Error trying to join with qdss config thread: %d\n",
					  __func__, ret);
		}
		DIAG_LOGD("diag: In %s, Successful in killing qdss config thread for MSM, qdss_mask: %d\n", __func__, qdss_mask);

		gettimeofday(&now, NULL);
		time.tv_sec = now.tv_sec + 1;

		qdss_kill_rw_thread = QDSS_KILL_STATE_ON;

		pthread_mutex_lock(&qdss_kill_mutex);
		DIAG_LOGD("diag: %s: Waiting for -EINVAL signal for MSM\n", __func__);
		ret = pthread_cond_timedwait(&qdss_kill_cond, &qdss_kill_mutex, &time);
		if (ret == ETIMEDOUT)
			DIAG_LOGE("diag: %s time out while waiting for -EINVAL\n", __func__);
		pthread_mutex_unlock(&qdss_kill_mutex);

		qdss_kill_rw_thread = QDSS_KILL_STATE_DONE;
		DIAG_LOGD("diag: %s: Initiate read and write thread exits, (qdss_kill_rw_thread: %d)\n", __func__, qdss_kill_rw_thread);

		if (qdss_in_write)
			pthread_cond_signal(&qdss_pools[qdss_curr_write].write_cond);

		ret = pthread_join(qdss_write_thread_hdl, NULL);
		if (ret != 0) {
			DIAG_LOGE("diag: In %s, Error trying to join with qdss write thread: %d\n",
					  __func__, ret);
		}
		DIAG_LOGD("diag: In %s, Successful in killing qdss write thread for MSM\n", __func__);

		if (qdss_in_read)
			pthread_cond_signal(&qdss_pools[qdss_curr_read].read_cond);
		if (in_qdss_read)
			pthread_kill(qdss_read_thread_hdl, SIGUSR2);

		ret = pthread_join(qdss_read_thread_hdl, NULL);
		if (ret != 0) {
			DIAG_LOGE("diag: In %s, Error trying to join with qdss read thread: %d\n",
					  __func__, ret);
		}
		DIAG_LOGD("diag: In %s, Successful in killing qdss read thread for MSM\n", __func__);

		if (diag_qdss_node_fd >= 0) {
			close(diag_qdss_node_fd);
			diag_qdss_node_fd = -1;
			if (use_etr1_buffer)
				diag_set_coresight_sysfs(ETR1_BLOCK_SIZE, "0", "byte_cntr1 - block_size", READ_WRITE);
			else
				diag_set_coresight_sysfs(BLOCK_SIZE, "0", "byte_cntr - block_size", READ_WRITE);
		}
	}
	local_remote_mask = remote_mask;
	while(local_remote_mask) {
		if(local_remote_mask & 1 ) {
			proc_type = z;
			local_device_mask = local_device_mask | (1 << proc_type);
		}
		z++;
		local_remote_mask = local_remote_mask >> 1;
	}
	qdss_mask = local_qdss_mask;
	if (device_mask & local_device_mask) {
		diag_configure_mdm_proc(MHI_QDSS_MODE_USB);
		for (dev_idx = 1; dev_idx <=  NUM_PROC; dev_idx++) {
			if ((remote_mask & (1 << (dev_idx - 1))) &&
				(p_type_mask & (1 << dev_idx))) {
				if (qdss_mask & DIAG_CON_MPSS) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_MODEM_PROC);
					qdss_mask ^= DIAG_CON_MPSS;
				}
				if (qdss_mask & DIAG_CON_LPASS) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_LPASS_PROC);
					qdss_mask ^= DIAG_CON_LPASS;
				}
				if (qdss_mask & DIAG_CON_WCNSS) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_WCNSS_PROC);
					qdss_mask ^= DIAG_CON_WCNSS;
				}
				if (qdss_mask & DIAG_CON_SENSORS) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_SENSORS_PROC);
					qdss_mask ^= DIAG_CON_SENSORS;
				}
				if (qdss_mask & DIAG_CON_WDSP) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_WDSP_PROC);
					qdss_mask ^= DIAG_CON_WDSP;
				}
				if (qdss_mask & DIAG_CON_CDSP) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_CDSP_PROC);
					qdss_mask ^= DIAG_CON_CDSP;
				}
				if (qdss_mask & DIAG_CON_NPU) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_NPU_PROC);
					qdss_mask ^= DIAG_CON_NPU;
				}
				if (qdss_mask & DIAG_CON_NSP1) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_NSP1_PROC);
					qdss_mask ^= DIAG_CON_NSP1;
				}
				if (qdss_mask & DIAG_CON_GPDSP0) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_GPDSP0_PROC);
					qdss_mask ^= DIAG_CON_GPDSP0;
				}
				if (qdss_mask & DIAG_CON_GPDSP1) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_GPDSP1_PROC);
					qdss_mask ^= DIAG_CON_GPDSP1;
				}
				if (qdss_mask & DIAG_CON_APSS) {
					diag_send_cmds_to_peripheral_kill(dev_idx, DIAG_APPS_PROC);
					qdss_mask ^= DIAG_CON_APSS;
				}
				if (qdss_mask & DIAG_CON_UPD_WLAN) {
					diag_send_cmds_to_peripheral_kill(dev_idx, UPD_WLAN);
					qdss_mask ^= DIAG_CON_UPD_WLAN;
				}
				if (qdss_mask & DIAG_CON_UPD_AUDIO) {
					diag_send_cmds_to_peripheral_kill(dev_idx, UPD_AUDIO);
					qdss_mask ^= DIAG_CON_UPD_AUDIO;
				}
				if (qdss_mask & DIAG_CON_UPD_SENSORS) {
					diag_send_cmds_to_peripheral_kill(dev_idx, UPD_SENSORS);
					qdss_mask ^= DIAG_CON_UPD_SENSORS;
				}
				if (qdss_mask & DIAG_CON_UPD_CHARGER) {
					diag_send_cmds_to_peripheral_kill(dev_idx, UPD_CHARGER);
					qdss_mask ^= DIAG_CON_UPD_CHARGER;
				}
				if (qdss_mask & DIAG_CON_UPD_OEM) {
					diag_send_cmds_to_peripheral_kill(dev_idx, UPD_OEM);
					qdss_mask ^= DIAG_CON_UPD_OEM;
				}
				if (qdss_mask & DIAG_CON_UPD_OIS) {
					diag_send_cmds_to_peripheral_kill(dev_idx, UPD_OIS);
					qdss_mask ^= DIAG_CON_UPD_OIS;
				}
				if (in_wait_for_qdss_mdm_status)
					pthread_cond_signal(&qdss_mdm_diag_cond);

				if (dev_idx == 1) {
					ret = pthread_join(qdss_config_thread_hdl_mdm, NULL);
					if (ret != 0) {
						DIAG_LOGE("diag: In %s, Error trying to join with qdss config thread for mdm: %d\n",
								  __func__, ret);
					}
					DIAG_LOGD("diag: In %s, Successful in killing qdss config thread for mdm, qdss_mask: %d\n", __func__, qdss_mask);

					if (qdss_in_write_mdm)
						pthread_cond_signal(&qdss_pools_mdm[qdss_curr_write_mdm].write_cond);

					ret = pthread_join(qdss_write_thread_hdl_mdm, NULL);
					if (ret != 0) {
						DIAG_LOGE("diag: In %s, Error trying to join with qdss write thread for mdm: %d\n",
								  __func__, ret);
					}
					DIAG_LOGD("diag: In %s, Successful in killing qdss write thread for mdm\n", __func__);

					if (qdss_in_read_mdm)
						pthread_cond_signal(&qdss_pools[qdss_curr_read_mdm].read_cond);
					if (in_qdss_read_mdm)
						pthread_kill(qdss_read_thread_hdl_mdm, SIGUSR2);
					ret = pthread_join(qdss_read_thread_hdl_mdm, NULL);
					if (ret != 0) {
						DIAG_LOGE("diag: In %s, Error trying to join with qdss read thread  for mdm: %d\n",
								  __func__, ret);
					}
					DIAG_LOGD("diag: In %s, Successful in killing qdss read thread for mdm\n", __func__);

					if (in_wait_for_qdss_mdm_up_status)
						pthread_cond_signal(&qdss_mdm_down_cond);
				}
			}
		}
		qdss_close_qdss_node_mdm();
	}
	/****************************
	 * Kill the write thread 	*
	 ****************************/

	pthread_mutex_destroy(&qdss_diag_mutex);
	pthread_mutex_destroy(&qdss_mdm_diag_mutex);

	pthread_mutex_destroy(&diagid_guid_map_mutex[MSM]);
	pthread_mutex_destroy(&diagid_guid_map_mutex[MDM]);
	pthread_cond_destroy(&diagid_guid_map_cond[MSM]);
	pthread_cond_destroy(&diagid_guid_map_cond[MDM]);

	pthread_mutex_destroy(&qdss_kill_mutex);
	pthread_cond_destroy(&qdss_kill_cond);

	pthread_mutex_destroy(&(qdss_read_buffer_pool[0].read_rsp_mutex));
	pthread_mutex_destroy(&(qdss_read_buffer_pool[1].read_rsp_mutex));
	pthread_mutex_destroy(&(qdss_read_buffer_pool[0].write_rsp_mutex));
	pthread_mutex_destroy(&(qdss_read_buffer_pool[1].write_rsp_mutex));
	pthread_cond_destroy(&(qdss_read_buffer_pool[0].read_rsp_cond));
	pthread_cond_destroy(&(qdss_read_buffer_pool[0].write_rsp_cond));
	pthread_cond_destroy(&(qdss_read_buffer_pool[1].read_rsp_cond));
	pthread_cond_destroy(&(qdss_read_buffer_pool[1].write_rsp_cond));

	pthread_cond_destroy(&qdss_diag_cond);
	pthread_cond_destroy(&qdss_mdm_diag_cond);

	pthread_cond_destroy(&(qdss_pools[0].write_cond));
	pthread_cond_destroy(&(qdss_pools[0].read_cond));
	pthread_cond_destroy(&(qdss_pools[1].write_cond));
	pthread_cond_destroy(&(qdss_pools[1].read_cond));

	pthread_cond_destroy(&(qdss_pools_mdm[0].write_cond));
	pthread_cond_destroy(&(qdss_pools_mdm[0].read_cond));
	pthread_cond_destroy(&(qdss_pools_mdm[1].write_cond));
	pthread_cond_destroy(&(qdss_pools_mdm[1].read_cond));

	if (qdss_read_buffer_pool[0].rsp_buf)
		free(qdss_read_buffer_pool[0].rsp_buf);
	if (qdss_read_buffer_pool[1].rsp_buf)
		free(qdss_read_buffer_pool[1].rsp_buf);

	diag_set_coresight_sysfs(RESET_SOURCE_SINK, "1", "reset source sink", WRITE_ONLY);

	if (etr_buffer_size_to_set) {
		if (use_etr1_buffer)
			diag_set_coresight_sysfs(ETR1_BUF_SIZE, default_etr_buffer_size, "ETR1 BUFFER SIZE", READ_WRITE);
		else
			diag_set_coresight_sysfs(ETR_BUF_SIZE, default_etr_buffer_size, "ETR BUFFER SIZE", READ_WRITE);
	}

	DIAG_LOGE("diag:In %s finished killing qdss threads\n", __func__);
}

int diag_reconfigure_qdss(void)
{
    if (qdss_mask)
        diag_notify_qdss_thread(DIAG_MSM_MASK, qdss_mask);

    return 0;
}

