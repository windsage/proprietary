/*
 * Copyright (c) 2015-2019, 2022-2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * 2015-2016 Qualcomm Atheros, Inc.
 *
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*
 * hostapd / VLAN initialization
 * Copyright 2003, Instant802 Networks, Inc.
 * Copyright 2005-2006, Devicescape Software, Inc.
 * Copyright (c) 2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

/*
 * Copyright (c) 2013-2014 The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This file was originally distributed by Qualcomm Atheros, Inc.
 * under proprietary terms before Copyright ownership was assigned
 * to the Linux Foundation.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <getopt.h>
#include <limits.h>
#include <asm/types.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "netlink/genl/genl.h"
#include "netlink/genl/family.h"
#include "netlink/genl/ctrl.h"
#ifdef FEATURE_DEBUG_DUMP
#include <sys/inotify.h>
#include <wifi_hal_ctrl.h>
#endif
#ifdef ANDROID
#include <wifi_hal_ctrl.h>
#if __has_include(<netlink-private/types.h>)
#include <netlink-private/object-api.h>
#include <netlink-private/types.h>
#else
#include <base/nl-base-utils.h>
#include <nl-priv-dynamic-core/nl-core.h>
#endif /* has netlink-private */
#endif /* ANDROID */
#include <netlink/netlink.h>
#ifndef CNSS_DIAG_PLATFORM_WIN
#include <net/if.h>
#endif
#include <netlink/socket.h>
#ifdef CNSS_DIAG_PLATFORM_WIN
#include <syslog.h>
#endif
#ifndef CNSS_DIAG_PLATFORM_WIN
#include <sys/prctl.h>
#else
#include <linux/prctl.h>
#endif
#include <sys/statvfs.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/wireless.h>
#include <libgen.h>
#include <pthread.h>
#ifdef WIN_AP_HOST_OPEN
#include <linux/nl80211.h>
#endif

#ifndef NO_DIAG_SUPPORT
#include "event.h"
#include "msg.h"

#include "diag_lsm.h"
#include "diagcmd.h"
#include "diag.h"
#endif

#include "cld-diag-parser.h"

#define CNSS_INTF "wlan0"
#define IFACE_UP 1
#define IFACE_DOWN 0
#define DELAY_IN_S 3
#define FREE_MEMORY_THRESHOLD 100
#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)
#define MAX_STR_LEN 255

#if defined(CNSS_DIAG_PLATFORM_WIN) || !defined(ANDROID) || defined(NO_DIAG_SUPPORT)
#define ALOGE printf
#endif

#ifdef WIN_AP_HOST_OPEN
struct nl_sock *nl_handle;
#define CNSS_NL_ATTR_FWLOG	7
#define CNSS_NL_LINK_IDX	8
#define CNSS_NL_DUAL_MAC	9
#define CNSS_NL_MAX_ATTR	10

/* Location of the device index */
#define FWLOG_BUF_IDX_DEV_ID		0
/* Location of the version value in buf */
#define FWLOG_BUF_IDX_VERSION		3

#endif

#ifdef ANDROID
#ifdef FEATURE_DEBUG_DUMP
#define CONFIG_CTRL_IFACE_CLIENT_WLAN0 "/data/vendor/wifi/wpa/sockets/wlan0"
#define DEBUG_DUMP_IN_PROGRESS_NODE "/sys/wifi/dump_in_progress"
/* 10 seconds wait timeout on DUMP_IN_PROGRESS_NODE */
#define DUMP_IN_PROGRESS_FD_TIMEOUT 10
#define CHIPSET_LOG_MAX_SIZE_NODE "/sys/wifi/max_chipset_log_size"
/* 10 seconds wait timeout on CHIPSET_LOG_MAX_SIZE_NODE */
#define MAX_CHIPSET_LOG_SIZE_TIMEOUT 10
/* Default max log size 1024Mb */
#define CHIPSET_LOG_SIZE_DEFAULT_MAX (1024)
/* Default min log size 50Mb */
#define CHIPSET_LOG_SIZE_DEFAULT_MIN (300)
#define DEFAULT_ARCHIVES_NUM 5
#endif
#endif
#define IEVENT_MAX_LEN 512

const char options[] =
"Options:\n\
-f, --logfile(Currently file path is fixed)\n\
-c, --console (prints the logs in the console)\n\
-s, --silent (No print will come when logging)\n\
-S, --syslog (Prints will logs in syslog)\n\
-p, --parse Diag log to syslog\n\
-q, --qxdm  (prints the logs in the qxdm)\n\
-x, --qxdm_sync (QXDM log packet format)\n\
-l, --qxdm_sync_log_file (QXDM log packet format QMDL2 file)\n\
-d, --debug  (more prints in logcat, check logcat \n\
-s ROME_DEBUG, example to use: -q -d or -c -d)\n\
-b --buffer_size ( example to use : -b 64(in KBs)\n\
-T --target_buffer_size ( example to use : -T 64(in KBs)\n\
-m --cnss_diag_config_file_loc ( example to use : -m /data/misc/cnss_diag.conf)\n\
-u, --Write fw/host logs to a local buffer\n\
-w, --This option along with '-u' facilitates to write decoded fw/host logs to buffer\n\
-D, --This option will disable FW logs to display on console\n\
-P, --This option point to continuous packet logs\n\
-n, --This option is to set host log netlink protocol\n\
-e, --This option is to set driver multi if name\n\
-z, --This option is to set absolute directory base path : -z /vendor/firmware\n\
The options can also be given in the abbreviated form --option=x or -o x. \
The options can be given in any order";

char *log_file_name_prefix[LOG_FILE_MAX] = {
	[HOST_LOG_FILE] = "host_driver_logs_",
	[FW_LOG_FILE] = "cnss_fw_logs_",
	[HOST_QXDM_LOG_FILE] = "host_qxdm_driver_logs_",
	[FW_QXDM_LOG_FILE] = "cnss_fw_qxdm_logs_",
	[PKTLOG_FILE] = "txrx_pktlog_"};
char *log_file_name_extn[LOG_FILE_MAX] = {
	[HOST_LOG_FILE] = "txt",
	[FW_LOG_FILE] = "txt",
	[HOST_QXDM_LOG_FILE] = "qmdl2",
	[FW_QXDM_LOG_FILE] = "qmdl2",
	[PKTLOG_FILE] = "dat"};

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
static int sock_fd = -1;
struct msghdr msg;
char hw_version[MAX_SIZE];
t_cnss_log_timer t_log_timer;

const char *progname;
#if defined(CONFIG_CLD80211_LIB) && !defined(ANDROID)
const char *getprogname() { return progname; }
#endif
int32_t optionflag = 0;
int log_path_flag = WRITE_TO_INTERNAL_SDCARD;
int delayed_count = 0;

unsigned long long avail_space = 0;
int max_file_size = MAX_FILE_SIZE;
int max_fw_archives = MAX_FW_FILE_INDEX;
int max_host_archives = MAX_HOST_FILE_INDEX;
int max_archives = MAX_FILE_INDEX;
int max_pktlog_archives = DEFAULT_PKTLOG_ARCHIVES;
int enable_flush_log = 0;

unsigned int configured_buffer_size = 0;
unsigned int target_configured_buffer_size = 0;
unsigned int real_time_write = 0;
int free_mem_threshold = FREE_MEMORY_THRESHOLD;
char wlan_log_dir_path[MAX_FILENAME_SIZE];
char multi_if_name[MAX_MULTI_IF_NAME_SIZE];
char multi_if_file_name[MAX_MULTI_IF_NAME_SIZE+1];
#ifndef NO_DIAG_SUPPORT
int firmware_log_levels_default = 1;
int data_primary = MSM;
#endif /* NO_DIAG_SUPPORT */

int max_no_buffer = 0;
int max_buff_size = DEFAULT_BUFFER_SIZE_MAX;
uint32_t max_pktlog_buff_size = DEFAULT_PKTLOG_BUFFER_SIZE;
/*
 *  disable_radio param represents 16 bit disable bitmask value,
 *  where each bit 'x' corresponds to radio_id value 'x',
 *  when set blocks all cnss fw logs from that radio to be written to file path.
 *  disable_radio=2,LSB represents 00000010,  blocks radio id=1
 *  disable_radio=0,LSB represents 00000000,  unblocks all radio
 *  disable_radio=6,LSB represents 00000110,  blocks radio id=1,2
 */
uint16_t disable_radio = 0;

struct cnss_log_file_data log_file[LOG_FILE_MAX];
uint8_t gwlan_dev = CNSS_DIAG_WLAN_DEV_UNDEF;
char *cnss_diag_wlan_dev_name[CNSS_DIAG_WLAN_DEV_MAX] = {
	[CNSS_DIAG_WLAN_DEV_UNDEF] = "X_X",
	[CNSS_DIAG_WLAN_ROM_DEV] = "QCA6174",
	[CNSS_DIAG_WLAN_TUF_DEV] = "QCA93",
	[CNSS_DIAG_WLAN_HEL_DEV] = "WCN3990",
	[CNSS_DIAG_WLAN_NAP_DEV] = "XXX_XXX"};

int host_log_socket_protocol = NETLINK_USERSOCK;

char *datamsc_file_path = NULL;
#ifdef ANDROID
char *cnssdiag_config_file = "/data/vendor/wifi/cnss_diag.conf";
char log_capture_loc[MAX_FILENAME_SIZE] = "/data/vendor/wifi/wlan_logs/";
char hbuffer_log_file[MAX_FILENAME_SIZE] =
		"/data/vendor/wifi/wlan_logs/buffered_cnsshost_log.txt";
char fbuffer_log_file[MAX_FILENAME_SIZE] =
		"/data/vendor/wifi/wlan_logs/buffered_cnssfw_log.txt";

#elif defined(CONFIG_WLAN_QCLINUX)
char *cnssdiag_config_file = "/etc/cnss_diag.conf";
char log_capture_loc[MAX_FILENAME_SIZE] = "/var/persist/wlan_logs/";
char hbuffer_log_file[MAX_FILENAME_SIZE] =
		 "/var/persist/wlan_logs/buffered_cnsshost_log.txt";
char fbuffer_log_file[MAX_FILENAME_SIZE] =
		 "/var/persist/wlan_logs/buffered_cnssfw_log.txt";
#else
char *cnssdiag_config_file = "/data/cnss_diag.conf";
char log_capture_loc[MAX_FILENAME_SIZE] = "/data/wlan_logs/";
char hbuffer_log_file[MAX_FILENAME_SIZE] =
		 "/data/wlan_logs/buffered_cnsshost_log.txt";
char fbuffer_log_file[MAX_FILENAME_SIZE] =
		 "/data/wlan_logs/buffered_cnssfw_log.txt";
#endif


boolean isDriverLoaded = FALSE;
pthread_t thread_pktlogger;
static unsigned int pktlogger_interval = 5; /* 5 secs */
static unsigned int cnss_diag_running = 0;

#ifdef CONFIG_CLD80211_LIB
void *cldctx = NULL;
#endif

char *line_trim(char *);
int disable_fw_logs_parsing = 0;
bool terminate_pending = false;

#ifndef NO_DIAG_SUPPORT
#ifdef CNSS_DIAG_PLATFORM_WIN
/*
 *  Setting Wlan FW Log level
 *  By default all modules are set to enable Low/Medium/High/Error/Fatal level
 *
 *  Below is the command request structure and follow by the data
 *  {
 *	 uint8_t cmd_code;     // Command 0x7d is MSG Config
 *	 uint8_t sub_cmd;      // Sub_command 4 is setting MSG mask
 *	 uint16_t ssid_first;  // Wlan FW first SSID is 4500, whichis 0x1194
 *	 uint16_t ssid_last;   // Wlan FW last SSID is 4583, which is 0x11e7
 *	 uint8_t status;       // Status is 0
 *	 uint8_t padding;      // Padding is 0
 *  };
 *
 *  FW_LOG_MASK is log level. 0x1F is to enable all level
 *  (Low/Medium/High/Error/Fatal)
 */
#define FW_LOG_MASK 0x1F
static unsigned char diag_config[] = {
	0x7d, 0x04, 0x94, 0x11, 0xe7, 0x11, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0,
	FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0, FW_LOG_MASK, 0, 0, 0
};
#endif
#endif

static void
usage(void)
{
	fprintf(stderr, "Usage:\n%s options\n", progname);
	fprintf(stderr, "%s\n", options);
	exit(-1);
}
/* function to find whether file exists or not */
int doesFileExist(const char *filename) {
	struct stat st;
	int result = stat(filename, &st);
	return result == 0;
}

static uint32_t get_le32(const uint8_t *pos)
{
	return pos[0] | (pos[1] << 8) | (pos[2] << 16) | (pos[3] << 24);
}


/* Opens a directory wlan_logs and searches the same for the presence of
 * host and firmware log files. Sets the index of the file which is used
 * to store the logs before the reboot.
 */


void readDir(const char *dirName, enum fileType type) {
	DIR *fdir;
	struct dirent *dirent;
	int *files = NULL;
	char file_name[256];
	int i = 0, found = 0;
	int archives = 0;

	if (type == PKTLOG_FILE) {
		archives = max_pktlog_archives;
	} else if (type == HOST_LOG_FILE) {
		archives = max_host_archives;
	} else if (type == FW_LOG_FILE) {
		archives = max_fw_archives;
	} else {
		archives = max_archives;
	}
	files = (int *)malloc(sizeof(int) * archives);
	if (NULL == files) {
	    debug_printf("%s: failed to allocate memory to host_files\n", __func__);
	    return;
	}

	memset(files, 0, (sizeof(int) * archives));
	fdir = opendir(dirName);
	if (NULL == fdir) {
	    debug_printf("%s: fdir is NULL\n", __func__);
	    free(files);
	    return;
	}
	chdir(dirName);
	while ((dirent = readdir(fdir)) != NULL) {
		found = 0;
		for (i = 0; i < archives; i++) {
			snprintf(file_name, sizeof(file_name), "%s%s%03d.%s",
					log_file_name_prefix[type], multi_if_file_name, i, log_file_name_extn[type]);

			if ((0 == (strcmp(dirent->d_name, file_name)))) {
				files[i] = 1;
				found = 1;
			}
			if (found)
				break;
		}
	}
/*
 * value 0 at index 'i' indicates, host log file current.txt will be renamed
 * with the filename at 'i'th index.
  */
	i = 0;
	while (i < archives) {
		if (!files[i]) {
			log_file[type].index = i;
			break;
		}
		i++;
	}
	debug_printf("%s: File Index: HOST_LOG_FILE: %d, HOST_QXDM_LOG_FILE: %d, FW_LOG_FILE: %d\n",
		 __func__, log_file[HOST_LOG_FILE].index, log_file[HOST_QXDM_LOG_FILE].index, log_file[FW_LOG_FILE].index);
	free(files);
	closedir(fdir);
}

void cnss_log_timeout_handler(union sigval sv)
{
	timer_t* timerid = (timer_t *) sv.sival_ptr;
	android_printf("%s: cnss log timeout %p\n",__func__, timerid);
	if (!t_log_timer.fw_logs_collected)
		t_log_timer.cnss_log_write_flag = FALSE;
	t_log_timer.fw_logs_collected = 0;
	timer_delete(*timerid);
}

static void initialize_cnsslog_timer(void)
{
   memset(&t_log_timer, 0, sizeof(struct cnss_log_timer));
   memset(&t_log_timer.value, 0, sizeof(struct itimerspec));
   memset(&t_log_timer.sev, 0, sizeof(struct sigevent));

   t_log_timer.value.it_value.tv_sec = CNSS_LOG_TIMER;
   t_log_timer.sev.sigev_notify = SIGEV_THREAD;
   t_log_timer.fw_logs_collected = 0;
   t_log_timer.start_cnss_timer = FALSE;
   t_log_timer.cnss_log_write_flag = TRUE;
   t_log_timer.sev.sigev_notify_function = &cnss_log_timeout_handler;
   t_log_timer.sev.sigev_value.sival_ptr = &t_log_timer.timerid;

   android_printf("%s: create cnss log timer \n",__func__);
   timer_create(CLOCK_REALTIME, &t_log_timer.sev, &t_log_timer.timerid);
}

/*
 * rename host/firmware current.txt logfile with the corresponding
 * host/firmware log file with proper index and delete its next
 * index file to identify the last file name used to store the logs
 * after a reboot.
 */


void backup_file(enum fileType type)
{
	char newFileName[MAX_SIZE];
	char delFileName[MAX_SIZE];
	int ret = 0;
	int archives = 0;

	if (type >= LOG_FILE_MAX)
		return;

	if (type == PKTLOG_FILE) {
		archives = max_pktlog_archives;
	} else if (type == HOST_LOG_FILE) {
		archives = max_host_archives;
	} else if (type == FW_LOG_FILE) {
		archives = max_fw_archives;
	} else {
		archives = max_archives;
	}
	snprintf(newFileName, sizeof(newFileName), "%s%s%s%03d.%s",
			wlan_log_dir_path, log_file_name_prefix[type], multi_if_file_name,
			log_file[type].index, log_file_name_extn[type]);
	errno = 0;
	ret = rename(log_file[type].name, newFileName);
	if (ret < 0) {
		android_printf("failed to rename %s with %s\n", log_file[type].name, newFileName);
	}
	log_file[type].fp = NULL;
	log_file[type].index++;
	// Do not reset file index if "maximum number of file" is "Unlimited"
	if (archives == -1)
		return;
	if (log_file[type].index > archives)
		log_file[type].index = 0;
	if (archives > 1) {
		snprintf(delFileName, sizeof(delFileName), "%s%s%s%03d.%s",
			wlan_log_dir_path, log_file_name_prefix[type], multi_if_file_name,
			log_file[type].index, log_file_name_extn[type]);
		unlink(delFileName);
        }
}

static void cleanup(void)
{
	int i;
	int rc = 0;
	cnss_diag_running = 0;
	if (optionflag & PKT_LOGFILE_FLAG) {
		rc = pthread_join(thread_pktlogger, NULL);
		if (rc != 0) {
			android_printf("Failed to join mutex, ret %d", rc);
		}
	}
	if (sock_fd)
		close(sock_fd);
	for (i = HOST_LOG_FILE; i < LOG_FILE_MAX; i++) {
		if (i == FW_QXDM_LOG_FILE)
			buffer_fw_logs_log_pkt("", TRUE);
		if(log_file[i].fp) {
	                fwrite(log_file[i].buf, sizeof(char), (log_file[i].buf_ptr - log_file[i].buf), log_file[i].fp);
			fflush(log_file[i].fp);
			fclose(log_file[i].fp);
			log_file[i].fp = NULL;
		}
		if (log_file[i].buf) {
			free(log_file[i].buf);
			log_file[i].buf = NULL;
		}
	}
}

static void clean_all_buff(struct cnss_log_file_data *lfd)
{
	t_buffer *next_buff = NULL, *start_buff;

	pthread_mutex_lock(&lfd->buff_lock);
	start_buff = (t_buffer *)lfd->buf;

	while (start_buff)
	{
		next_buff = start_buff->next;
		free(start_buff->start);
		free(start_buff);
		if ((t_buffer *)lfd->buf == next_buff)
			break;
		start_buff = next_buff;
	}
	pthread_mutex_unlock(&lfd->buff_lock);
}

static void stop(void)
{
#ifdef CONFIG_CLD80211_LIB
	exit_cld80211_recv(cldctx);
#endif
	if (optionflag & LOG_BUFF_FLAG) {
		printf("free all buffers\n ");
		clean_all_buff(&log_file[BUFFER_HOST_FILE]);
		log_file[BUFFER_HOST_FILE].buf = NULL;
		clean_all_buff(&log_file[BUFFER_FW_FILE]);
		log_file[BUFFER_FW_FILE].buf = NULL;
	}
	if(optionflag & LOGFILE_FLAG || optionflag & PKT_LOGFILE_FLAG){
		printf("Recording stopped\n");
		cleanup();
	}

	if (pthread_mutex_destroy(&log_file[BUFFER_HOST_FILE].buff_lock))
		ALOGE("Failed to destroy host buff_lock");

	if (pthread_mutex_destroy(&log_file[BUFFER_FW_FILE].buff_lock))
		ALOGE("Failed to destroy firmware buff_lock");

#ifdef WIN_AP_HOST_OPEN
	nl_socket_free(nl_handle);
#endif
	terminate_pending = false;
	exit(0);
}

static void signal_handler(int32_t signum)
{
	UNUSED(signum);
	terminate_pending = true;
#ifndef ANDROID
#ifdef CONFIG_CLD80211_LIB
	if (cldctx)
		cld80211_stop_recv(cldctx, terminate_pending);
	else
		ALOGE("cldctx is not valid, missed to set terminate_pending");
#endif
#endif
}

static void logbuffer_to_file(struct cnss_log_file_data *lfd)
{
	t_buffer *start_pos = (t_buffer *)lfd->buf_ptr;
	t_buffer *buffer_log;
	FILE *fp;
	size_t len;

	if (start_pos == NULL)
		return;
	fp = fopen(lfd->name, "w+");
	if (fp == NULL) {
		android_printf("Failed to open file %s\n", lfd->name);
		return;
	}

	pthread_mutex_lock(&lfd->buff_lock);
	buffer_log = start_pos->next;

	while (1) {
		len = fwrite(buffer_log->start, sizeof(char),
			     buffer_log->end - buffer_log->start, fp);
		if (len != (size_t)(buffer_log->end - buffer_log->start)) {
			android_printf("fwrite failed with len = %zu\n", len);
			break;
		}

		if (buffer_log == start_pos)
			break;
		buffer_log = buffer_log->next;
	}
	pthread_mutex_unlock(&lfd->buff_lock);
	fclose(fp);
}

static void update_buff_to_file(int32_t signum)
{
	UNUSED(signum);

	logbuffer_to_file(&log_file[BUFFER_HOST_FILE]);
	logbuffer_to_file(&log_file[BUFFER_FW_FILE]);

	android_printf("Written buffers successfully into files\n");
}

static void default_handler_sigusr1(int32_t signum)
{
	android_printf("SIGUSR1: %d is reserved for buffer logging\n", signum);
}

void process_cnss_log_file(uint8_t *dbgbuf)
{
	uint16_t length = 0;
	uint32_t dropped = 0;
	uint32_t timestamp = 0;
	uint32_t res =0;
	struct dbglog_slot *slot = (struct dbglog_slot *)dbgbuf;
	if (NULL != log_file[FW_LOG_FILE].fp)
		fseek(log_file[FW_LOG_FILE].fp, ftell(log_file[FW_LOG_FILE].fp), SEEK_SET);
	timestamp = get_le32((uint8_t *)&slot->timestamp);
	length = get_le32((uint8_t *)&slot->length);
	dropped = get_le32((uint8_t *)&slot->dropped);
	if (!((optionflag & SILENT_FLAG) == SILENT_FLAG)) {
		/* don't like this have to fix it */
		printf("Read bytes %ld timestamp=%u length=%u fw dropped=%u\n",
		    (log_file[FW_LOG_FILE].fp != NULL )? ftell(log_file[FW_LOG_FILE].fp) : 0, timestamp, length, dropped);
	}
	if (NULL != log_file[FW_LOG_FILE].fp) {
		if ((res = fwrite(dbgbuf, RECLEN, 1, log_file[FW_LOG_FILE].fp)) != 1) {
			perror("fwrite");
			return;
		}
		fflush(log_file[FW_LOG_FILE].fp);
	}
}

/*
 * This function trims any leading and trailing white spaces
 */
char *line_trim(char *str)
{
	char *ptr;

	if(*str == '\0') return str;

	/* Find the first non white-space */
	for (ptr = str; i_isspace(*ptr); ptr++);
	if (*ptr == '\0')
	    return str;

	/* This is the new start of the string*/
	str = ptr;

	/* Find the last non white-space and null terminate the string */
	ptr += strlen(ptr) - 1;
	for (; ptr != str && i_isspace(*ptr); ptr--);
	ptr[1] = '\0';

	return str;
}
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif
struct macro_table {
	char *macro_name;
	uint32_t macro_value;
};

#define MACRO_TABLE_ENTRY(macro) {#macro, macro}
int32_t get_macro_number(char *macro_name, struct macro_table *table,
			 int table_size)
{
	int cnt;

	for (cnt = 0; cnt < table_size; cnt++) {
		if (strncmp(macro_name, table[cnt].macro_name, MAX_STR_LEN))
			continue;
		else
			return table[cnt].macro_value;
	}
	return -1;
}

#ifndef NO_DIAG_SUPPORT
struct macro_table ssid_mask_table[] = {
	MACRO_TABLE_ENTRY(MSG_SSID_GEN_FIRST),
	MACRO_TABLE_ENTRY(MSG_SSID_DFLT),
	MACRO_TABLE_ENTRY(MSG_SSID_LEGACY),
	MACRO_TABLE_ENTRY(MSG_SSID_AUDFMT),
	MACRO_TABLE_ENTRY(MSG_SSID_AVS),
	MACRO_TABLE_ENTRY(MSG_SSID_BOOT),
	MACRO_TABLE_ENTRY(MSG_SSID_BT),
	MACRO_TABLE_ENTRY(MSG_SSID_CM),
	MACRO_TABLE_ENTRY(MSG_SSID_CMX),
	MACRO_TABLE_ENTRY(MSG_SSID_DIAG),
	MACRO_TABLE_ENTRY(MSG_SSID_DSM),
	MACRO_TABLE_ENTRY(MSG_SSID_FS),
	MACRO_TABLE_ENTRY(MSG_SSID_HS),
	MACRO_TABLE_ENTRY(MSG_SSID_MDSP),
	MACRO_TABLE_ENTRY(MSG_SSID_QDSP),
	MACRO_TABLE_ENTRY(MSG_SSID_REX),
	MACRO_TABLE_ENTRY(MSG_SSID_RF),
	MACRO_TABLE_ENTRY(MSG_SSID_SD),
	MACRO_TABLE_ENTRY(MSG_SSID_SIO),
	MACRO_TABLE_ENTRY(MSG_SSID_VS),
	MACRO_TABLE_ENTRY(MSG_SSID_WMS),
	MACRO_TABLE_ENTRY(MSG_SSID_GPS),
	MACRO_TABLE_ENTRY(MSG_SSID_MMOC),
	MACRO_TABLE_ENTRY(MSG_SSID_RUIM),
	MACRO_TABLE_ENTRY(MSG_SSID_TMC),
	MACRO_TABLE_ENTRY(MSG_SSID_FTM),
	MACRO_TABLE_ENTRY(MSG_SSID_MMGPS),
	MACRO_TABLE_ENTRY(MSG_SSID_SLEEP),
	MACRO_TABLE_ENTRY(MSG_SSID_SAM),
	MACRO_TABLE_ENTRY(MSG_SSID_SRM),
	MACRO_TABLE_ENTRY(MSG_SSID_SFAT),
	MACRO_TABLE_ENTRY(MSG_SSID_JOYST),
	MACRO_TABLE_ENTRY(MSG_SSID_MFLO),
	MACRO_TABLE_ENTRY(MSG_SSID_DTV),
	MACRO_TABLE_ENTRY(MSG_SSID_TCXOMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_EFS),
	MACRO_TABLE_ENTRY(MSG_SSID_IRDA),
	MACRO_TABLE_ENTRY(MSG_SSID_FM_RADIO),
	MACRO_TABLE_ENTRY(MSG_SSID_AAM),
	MACRO_TABLE_ENTRY(MSG_SSID_BM),
	MACRO_TABLE_ENTRY(MSG_SSID_PE),
	MACRO_TABLE_ENTRY(MSG_SSID_QIPCALL),
	MACRO_TABLE_ENTRY(MSG_SSID_FLUTE),
	MACRO_TABLE_ENTRY(MSG_SSID_CAMERA),
	MACRO_TABLE_ENTRY(MSG_SSID_HSUSB),
	MACRO_TABLE_ENTRY(MSG_SSID_FC),
	MACRO_TABLE_ENTRY(MSG_SSID_USBHOST),
	MACRO_TABLE_ENTRY(MSG_SSID_PROFILER),
	MACRO_TABLE_ENTRY(MSG_SSID_MGP),
	MACRO_TABLE_ENTRY(MSG_SSID_MGPME),
	MACRO_TABLE_ENTRY(MSG_SSID_GPSOS),
	MACRO_TABLE_ENTRY(MSG_SSID_MGPPE),
	MACRO_TABLE_ENTRY(MSG_SSID_GPSSM),
	MACRO_TABLE_ENTRY(MSG_SSID_IMS),
	MACRO_TABLE_ENTRY(MSG_SSID_MBP_RF),
	MACRO_TABLE_ENTRY(MSG_SSID_SNS),
	MACRO_TABLE_ENTRY(MSG_SSID_WM),
	MACRO_TABLE_ENTRY(MSG_SSID_LK),
	MACRO_TABLE_ENTRY(MSG_SSID_PWRDB),
	MACRO_TABLE_ENTRY(MSG_SSID_DCVS),
	MACRO_TABLE_ENTRY(MSG_SSID_ANDROID_ADB),
	MACRO_TABLE_ENTRY(MSG_SSID_VIDEO_ENCODER),
	MACRO_TABLE_ENTRY(MSG_SSID_VENC_OMX),
	MACRO_TABLE_ENTRY(MSG_SSID_GAN),
	MACRO_TABLE_ENTRY(MSG_SSID_KINETO_GAN),
	MACRO_TABLE_ENTRY(MSG_SSID_ANDROID_QCRIL),
	MACRO_TABLE_ENTRY(MSG_SSID_A2),
	MACRO_TABLE_ENTRY(MSG_SSID_LINUX_DATA),
	MACRO_TABLE_ENTRY(MSG_SSID_ECALL),
	MACRO_TABLE_ENTRY(MSG_SSID_CHORD),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNE),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_CAD_GENERAL),
	MACRO_TABLE_ENTRY(MSG_SSID_OMADM),
	MACRO_TABLE_ENTRY(MSG_SSID_SIWA),
	MACRO_TABLE_ENTRY(MSG_SSID_APR_MODEM),
	MACRO_TABLE_ENTRY(MSG_SSID_APR_APPS),
	MACRO_TABLE_ENTRY(MSG_SSID_APR_ADSP),
	MACRO_TABLE_ENTRY(MSG_SSID_SRD_GENERAL),
	MACRO_TABLE_ENTRY(MSG_SSID_ACDB_GENERAL),
	MACRO_TABLE_ENTRY(MSG_SSID_DALTF),
	MACRO_TABLE_ENTRY(MSG_SSID_CFM),
	MACRO_TABLE_ENTRY(MSG_SSID_PMIC),
	MACRO_TABLE_ENTRY(MSG_SSID_GPS_SDP),
	MACRO_TABLE_ENTRY(MSG_SSID_TLE),
	MACRO_TABLE_ENTRY(MSG_SSID_TLE_XTM),
	MACRO_TABLE_ENTRY(MSG_SSID_TLE_TLM),
	MACRO_TABLE_ENTRY(MSG_SSID_TLE_TLM_MM),
	MACRO_TABLE_ENTRY(MSG_SSID_WWAN_LOC),
	MACRO_TABLE_ENTRY(MSG_SSID_GNSS_LOCMW),
	MACRO_TABLE_ENTRY(MSG_SSID_QSET),
	MACRO_TABLE_ENTRY(MSG_SSID_QBI),
	MACRO_TABLE_ENTRY(MSG_SSID_ADC),
	MACRO_TABLE_ENTRY(MSG_SSID_MMODE_QMI),
	MACRO_TABLE_ENTRY(MSG_SSID_MCFG),
	MACRO_TABLE_ENTRY(MSG_SSID_SSM),
	MACRO_TABLE_ENTRY(MSG_SSID_MPOWER),
	MACRO_TABLE_ENTRY(MSG_SSID_RMTS),
	MACRO_TABLE_ENTRY(MSG_SSID_ADIE),
	MACRO_TABLE_ENTRY(MSG_SSID_VT_VCEL),
	MACRO_TABLE_ENTRY(MSG_SSID_FLASH_SCRUB),
	MACRO_TABLE_ENTRY(MSG_SSID_STRIDE),
	MACRO_TABLE_ENTRY(MSG_SSID_POLICYMAN),
	MACRO_TABLE_ENTRY(MSG_SSID_TMS),
	MACRO_TABLE_ENTRY(MSG_SSID_LWIP),
	MACRO_TABLE_ENTRY(MSG_SSID_RFS),
	MACRO_TABLE_ENTRY(MSG_SSID_RFS_ACCESS),
	MACRO_TABLE_ENTRY(MSG_SSID_RLC),
	MACRO_TABLE_ENTRY(MSG_SSID_MEMHEAP),
	MACRO_TABLE_ENTRY(MSG_SSID_WCI2),
	MACRO_TABLE_ENTRY(MSG_SSID_LOWI_TEST),
	MACRO_TABLE_ENTRY(MSG_SSID_AOSTLM),
	MACRO_TABLE_ENTRY(MSG_SSID_LOWI_AP),
	MACRO_TABLE_ENTRY(MSG_SSID_LOWI_MP),
	MACRO_TABLE_ENTRY(MSG_SSID_LOWI_LP),
	MACRO_TABLE_ENTRY(MSG_SSID_MRE),
	MACRO_TABLE_ENTRY(MSG_SSID_SLIM),
	MACRO_TABLE_ENTRY(MSG_SSID_WLE),
	MACRO_TABLE_ENTRY(MSG_SSID_WLM),
	MACRO_TABLE_ENTRY(MSG_SSID_Q6ZIP),
	MACRO_TABLE_ENTRY(MSG_SSID_RF_DEBUG),
	MACRO_TABLE_ENTRY(MSG_SSID_NV),
	MACRO_TABLE_ENTRY(MSG_SSID_GEN_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_ONCRPC),
	MACRO_TABLE_ENTRY(MSG_SSID_ONCRPC_MISC_MODEM),
	MACRO_TABLE_ENTRY(MSG_SSID_ONCRPC_MISC_APPS),
	MACRO_TABLE_ENTRY(MSG_SSID_ONCRPC_CM_MODEM),
	MACRO_TABLE_ENTRY(MSG_SSID_ONCRPC_CM_APPS),
	MACRO_TABLE_ENTRY(MSG_SSID_ONCRPC_DB),
	MACRO_TABLE_ENTRY(MSG_SSID_ONCRPC_SND),
	MACRO_TABLE_ENTRY(MSG_SSID_ONCRPC_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_1X),
	MACRO_TABLE_ENTRY(MSG_SSID_1X_ACP),
	MACRO_TABLE_ENTRY(MSG_SSID_1X_DCP),
	MACRO_TABLE_ENTRY(MSG_SSID_1X_DEC),
	MACRO_TABLE_ENTRY(MSG_SSID_1X_ENC),
	MACRO_TABLE_ENTRY(MSG_SSID_1X_GPSSRCH),
	MACRO_TABLE_ENTRY(MSG_SSID_1X_MUX),
	MACRO_TABLE_ENTRY(MSG_SSID_1X_SRCH),
	MACRO_TABLE_ENTRY(MSG_SSID_1X_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_HDR_PROT),
	MACRO_TABLE_ENTRY(MSG_SSID_HDR_DATA),
	MACRO_TABLE_ENTRY(MSG_SSID_HDR_SRCH),
	MACRO_TABLE_ENTRY(MSG_SSID_HDR_DRIVERS),
	MACRO_TABLE_ENTRY(MSG_SSID_HDR_IS890),
	MACRO_TABLE_ENTRY(MSG_SSID_HDR_DEBUG),
	MACRO_TABLE_ENTRY(MSG_SSID_HDR_HIT),
	MACRO_TABLE_ENTRY(MSG_SSID_HDR_PCP),
	MACRO_TABLE_ENTRY(MSG_SSID_HDR_HEAPMEM),
	MACRO_TABLE_ENTRY(MSG_SSID_HDR_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_UMTS),
	MACRO_TABLE_ENTRY(MSG_SSID_WCDMA_L1),
	MACRO_TABLE_ENTRY(MSG_SSID_WCDMA_L2),
	MACRO_TABLE_ENTRY(MSG_SSID_WCDMA_MAC),
	MACRO_TABLE_ENTRY(MSG_SSID_WCDMA_RLC),
	MACRO_TABLE_ENTRY(MSG_SSID_WCDMA_RRC),
	MACRO_TABLE_ENTRY(MSG_SSID_NAS_CNM),
	MACRO_TABLE_ENTRY(MSG_SSID_NAS_MM),
	MACRO_TABLE_ENTRY(MSG_SSID_NAS_MN),
	MACRO_TABLE_ENTRY(MSG_SSID_NAS_RABM),
	MACRO_TABLE_ENTRY(MSG_SSID_NAS_REG),
	MACRO_TABLE_ENTRY(MSG_SSID_NAS_SM),
	MACRO_TABLE_ENTRY(MSG_SSID_NAS_TC),
	MACRO_TABLE_ENTRY(MSG_SSID_NAS_CB),
	MACRO_TABLE_ENTRY(MSG_SSID_WCDMA_LEVEL),
	MACRO_TABLE_ENTRY(MSG_SSID_UMTS_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_L1),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_L2),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_RR),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_GPRS_GCOMMON),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_GPRS_GLLC),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_GPRS_GMAC),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_GPRS_GPL1),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_GPRS_GRLC),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_GPRS_GRR),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_GPRS_GSNDCP),
	MACRO_TABLE_ENTRY(MSG_SSID_GSM_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_ADP),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_CP),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_FTM),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_OEM),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_SEC),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_TRP),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESERVED_1),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESERVED_2),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESERVED_3),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESERVED_4),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESERVED_5),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESERVED_6),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESERVED_7),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESERVED_8),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESERVED_9),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESERVED_10),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_TL),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_BAL),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_SAL),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_SSC),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_HDD),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_SME),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_PE),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_HAL),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_SYS),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_VOSS),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_ATHOS),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_WMI),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_HTT),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_PS_STA),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_PS_IBSS),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_PS_AP),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_SMPS_STA),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_WHAL),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_COEX),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_ROAM),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RESMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_PROTO),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_SCAN),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_BATCH_SCAN),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_EXTSCAN),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RC),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_BLOCKACK),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_TXRX_DATA),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_TXRX_MGMT),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_BEACON),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_OFFLOAD_MGR),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_MACCORE),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_PCIELP),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RTT),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_DCS),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_CACHEMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_ANI),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_P2P),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_CSA),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_NLO),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_CHATTER),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_WOW),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_WMMAC),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_TDLS),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_HB),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_TXBF),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_THERMAL),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_DFS),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_RMC),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_STATS),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_NAN),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_HIF_UART),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_LPI),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_MLME),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_SUPPL),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_ERE),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_OCB),
	MACRO_TABLE_ENTRY(MSG_SSID_WLAN_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_ATS),
	MACRO_TABLE_ENTRY(MSG_SSID_MSGR),
	MACRO_TABLE_ENTRY(MSG_SSID_APPMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_QTF),
	MACRO_TABLE_ENTRY(MSG_SSID_FWS),
	MACRO_TABLE_ENTRY(MSG_SSID_SRCH4),
	MACRO_TABLE_ENTRY(MSG_SSID_CMAPI),
	MACRO_TABLE_ENTRY(MSG_SSID_MMAL),
	MACRO_TABLE_ENTRY(MSG_SSID_QRARB),
	MACRO_TABLE_ENTRY(MSG_SSID_LMTSMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_MCS_RESERVED_7),
	MACRO_TABLE_ENTRY(MSG_SSID_MCS_RESERVED_8),
	MACRO_TABLE_ENTRY(MSG_SSID_IRATMAN),
	MACRO_TABLE_ENTRY(MSG_SSID_CXM),
	MACRO_TABLE_ENTRY(MSG_SSID_VSTMR),
	MACRO_TABLE_ENTRY(MSG_SSID_CFCM),
	MACRO_TABLE_ENTRY(MSG_SSID_MCS_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_DS),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_RLP),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_PPP),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_TCPIP),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_IS707),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_3GMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_PS),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_MIP),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_UMTS),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_GPRS),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_GSM),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_SOCKETS),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_ATCOP),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_SIO),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_BCMCS),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_MLRLP),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_RTP),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_SIPSTACK),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_ROHC),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_DOQOS),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_IPC),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_SHIM),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_ACLPOLICY),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_APPS),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_MUX),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_3GPP),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_LTE),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_WCDMA),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_ACLPOLICY_APPS),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_HDR),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_IPA),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_EPC),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_APPSRV),
	MACRO_TABLE_ENTRY(MSG_SSID_DS_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_CRYPTO),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_SSL),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_IPSEC),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_SFS),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_TEST),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_CNTAGENT),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_RIGHTSMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_ROAP),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_MEDIAMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_IDSTORE),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_IXFILE),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_IXSQL),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_IXCOMMON),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_BCASTCNTAGENT),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_PLAYREADY),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_WIDEVINE),
	MACRO_TABLE_ENTRY(MSG_SSID_SEC_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_APPMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_UI),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QVP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QVP_STATISTICS),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QVP_VENCODER),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QVP_MODEM),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QVP_UI),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QVP_STACK),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QVP_VDECODER),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_ACM),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_HEAP_PROFILE),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_GENERAL),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_DEBUG),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_STATISTICS),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_UI_TASK),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_MP4_PLAYER),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_AUDIO_TASK),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_VIDEO_TASK),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_STREAMING),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_MPEG4_TASK),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_FILE_OPS),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_RTP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_RTCP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_RTSP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_SDP_PARSE),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_ATOM_PARSE),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_TEXT_TASK),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_DEC_DSP_IF),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_STREAM_RECORDING),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_CONFIGURATION),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QCAMERA),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QCAMCORDER),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_BREW),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QDJ),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QDTX),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_QTV_BCAST_FLO),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_MDP_GENERAL),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_PBM),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_GENERAL),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_EGL),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_OPENGL),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_DIRECT3D),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_SVG),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_OPENVG),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_2D),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_QXPROFILER),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_DSP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_GRP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_GRAPHICS_MDP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_CAD),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_DPL),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_FW),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_SIP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_REGMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_RTP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_SDP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_VS),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_XDM),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_HOM),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_IM_ENABLER),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_IMS_CORE),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_FWAPI),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_SERVICES),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_POLICYMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_PRESENCE),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_QIPCALL),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_SIGCOMP),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_PSVT),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_UNKNOWN),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_SETTINGS),
	MACRO_TABLE_ENTRY(MSG_SSID_OMX_COMMON),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_RCS_CD),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_RCS_IM),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_RCS_FT),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_RCS_IS),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_RCS_AUTO_CONFIG),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_RCS_COMMON),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_UT),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_XML),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_IMS_COM),
	MACRO_TABLE_ENTRY(MSG_SSID_APPS_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_KERNEL),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_AFETASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_VOICEPROCTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_VOCDECTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_VOCENCTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_VIDEOTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_VFETASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_VIDEOENCTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_JPEGTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_AUDPPTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_AUDPLAY0TASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_AUDPLAY1TASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_AUDPLAY2TASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_AUDPLAY3TASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_AUDPLAY4TASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_LPMTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_DIAGTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_AUDRECTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_AUDPREPROCTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_MODMATHTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_GRAPHICSTASK),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSPTASKS_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_L4LINUX_KERNEL),
	MACRO_TABLE_ENTRY(MSG_SSID_L4LINUX_KEYPAD),
	MACRO_TABLE_ENTRY(MSG_SSID_L4LINUX_APPS),
	MACRO_TABLE_ENTRY(MSG_SSID_L4LINUX_QDDAEMON),
	MACRO_TABLE_ENTRY(MSG_SSID_L4LINUX_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_IGUANASERVER),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_EFS2),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_QDMS),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_REX),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_SMMS),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_FRAMEBUFFER),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_KEYPAD),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_NAMING),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_SDIO),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_SERIAL),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_TIMER),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_TRAMP),
	MACRO_TABLE_ENTRY(MSG_SSID_L4IGUANA_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_L4AMSS_QDIAG),
	MACRO_TABLE_ENTRY(MSG_SSID_L4AMSS_APS),
	MACRO_TABLE_ENTRY(MSG_SSID_L4AMSS_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_HIT),
	MACRO_TABLE_ENTRY(MSG_SSID_HIT_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_QDSP6),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_SVC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_ENCDEC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_VOC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_VS),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_MIDI),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_POSTPROC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_PREPROC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_AFE),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_MSESSION),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_DSESSION),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_AUD_DCM),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_VID_ENC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_VID_ENCRPC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_VID_DEC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_VID_DECRPC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_VID_COMMONSW),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_VID_HWDRIVER),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_JPG_ENC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_JPG_DEC),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_OMM),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_PWRDEM),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_RESMGR),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_CORE),
	MACRO_TABLE_ENTRY(MSG_SSID_ADSP_RDA),
	MACRO_TABLE_ENTRY(MSG_SSID_DSP_TOUCH_TAFE_HAL),
	MACRO_TABLE_ENTRY(MSG_SSID_DSP_TOUCH_ALGORITHM),
	MACRO_TABLE_ENTRY(MSG_SSID_DSP_TOUCH_FRAMEWORK),
	MACRO_TABLE_ENTRY(MSG_SSID_DSP_TOUCH_SRE),
	MACRO_TABLE_ENTRY(MSG_SSID_DSP_TOUCH_TAFE_DRIVER),
	MACRO_TABLE_ENTRY(MSG_SSID_QDSP6_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_UMB),
	MACRO_TABLE_ENTRY(MSG_SSID_UMB_APP),
	MACRO_TABLE_ENTRY(MSG_SSID_UMB_DS),
	MACRO_TABLE_ENTRY(MSG_SSID_UMB_CP),
	MACRO_TABLE_ENTRY(MSG_SSID_UMB_RLL),
	MACRO_TABLE_ENTRY(MSG_SSID_UMB_MAC),
	MACRO_TABLE_ENTRY(MSG_SSID_UMB_SRCH),
	MACRO_TABLE_ENTRY(MSG_SSID_UMB_FW),
	MACRO_TABLE_ENTRY(MSG_SSID_UMB_PLT),
	MACRO_TABLE_ENTRY(MSG_SSID_UMB_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_RRC),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_MACUL),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_MACDL),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_MACCTRL),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_RLCUL),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_RLCDL),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_PDCPUL),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_PDCPDL),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_ML1),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_DISCOVERY),
	MACRO_TABLE_ENTRY(MSG_SSID_LTE_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_CAPP),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_CENG),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_CREG),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_CMED),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_CAUTH),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_QBAL),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_OSAL),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_OEMCUST),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_MULTI_PROC),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_UPK),
	MACRO_TABLE_ENTRY(MSG_SSID_QCHAT_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_TDSCDMA_L1),
	MACRO_TABLE_ENTRY(MSG_SSID_TDSCDMA_L2),
	MACRO_TABLE_ENTRY(MSG_SSID_TDSCDMA_MAC),
	MACRO_TABLE_ENTRY(MSG_SSID_TDSCDMA_RLC),
	MACRO_TABLE_ENTRY(MSG_SSID_TDSCDMA_RRC),
	MACRO_TABLE_ENTRY(MSG_SSID_TDSCDMA_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_CTA),
	MACRO_TABLE_ENTRY(MSG_SSID_CTA_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_CAC),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_CORE),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_CORE_CAS),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_CORE_CDE),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_CORE_COM),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_CORE_LEE),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_CORE_QMI),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_CORE_SRM),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_GENERIC),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_NETLINK),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_NIMS),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_NSRM),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_NSRM_CORE),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_NSRM_GATESM),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_NSRM_TRG),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_PLCY),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_PLCY_ANDSF),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_TEST),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_WQE),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_WQE_BQE),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_WQE_CQE),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_WQE_ICD),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_WQE_IFSEL),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_WQE_IFSELRSM),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_ATP),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_ATP_PLCY),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_ATP_RPRT),
	MACRO_TABLE_ENTRY(MSG_SSID_QCNEA_LAST),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_COMMON),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_COM),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_QMI),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_DSM),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_CONFIG),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_GENERIC),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_NETLINK),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_FD_MGR),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_CT_MGR),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_NSRM),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_NSRM_CORE),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_NSRM_GATESM),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_NSRM_TRG),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_TEST),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_TCM),
	MACRO_TABLE_ENTRY(MSG_SSID_DPM_LAST),
};

struct macro_table log_level_table[] = {
	MACRO_TABLE_ENTRY(MSG_LVL_FATAL),
	MACRO_TABLE_ENTRY(MSG_LVL_ERROR),
	MACRO_TABLE_ENTRY(MSG_LVL_HIGH),
	MACRO_TABLE_ENTRY(MSG_LVL_MED),
	MACRO_TABLE_ENTRY(MSG_LVL_LOW),
	MACRO_TABLE_ENTRY(MSG_LVL_NONE),
};

/*
 * Read the FW_LOG_LEVELS part in the config file
 *
 * The section for FW_LOG_LEVELS is in the following format (example)
 * FW_LOG_LEVELS = {
 *    MSG_SSID_WLAN_ADP MSG_SSID_WLAN_TRP MSG_LVL_ERROR
 *    MSG_SSID_WLAN_WMI MSG_SSID_WLAN_PROTO MSG_LVL_FATAL
 * }
 * In the above we set log-level of MSG_LVL_ERROR for modules between
 * MSG_SSID_WLAN_ADP and MSG_SSID_WLAN_TRP, log level of MSG_LVL_FATAL
 * for modules between MSG_SSID_WLAN_WMI and MSG_SSID_WLAN_PROTO
 */
void read_config_fw_log_levels(FILE *fp)
{
	char line_string[MAX_STR_LEN + 1] = {0};
	char trimmed_line[MAX_STR_LEN + 1] = {0};
	char *arg_ptr;

	/*
	 * <Command Code 1 byte> is 0x7D  (Extended command)
	 * <Sub Command 1 byte> is 0x04 (set subsystem runtime mask)
	 * <Subsytem ID First 2 bytes>
	 * <Subsytem ID End 2 bytes>
	 * <Padding 2bytes> is 0x0000
	 * Total = 1+1+2+2+2 = 8 bytes
	 */
	const int DIAG_HDR_SIZE = 8;

	while (!feof(fp)) {
		int32_t ssid_log_level;
		char ssid_first_str[MAX_STR_LEN + 1];
		char ssid_last_str[MAX_STR_LEN + 1];
		char ssid_log_level_str[MAX_STR_LEN + 1];
		int16_t ssid_first;
		int16_t ssid_last;
		uint16_t num_ssids;
		uint8_t *buf = NULL;
		uint8_t *buf_ptr;
		int cnt;
		uint32_t buf_size;

		line_string[0] = 0;
		fgets(line_string,
				sizeof(line_string), fp);
		strlcpy(trimmed_line, line_trim(line_string),
			sizeof(trimmed_line));

		/*
		 * Ignore commented line
		 */
		if (*trimmed_line == '#')
			continue;

		/*
		 * End of the FW_LOG_LEVELS section
		 */
		if (*trimmed_line == '}')
			break;

		arg_ptr = trimmed_line;

		/*
		 * Get SSID-FIRST
		 */
		if (sscanf(arg_ptr, "%"STR(MAX_STR_LEN)"s",
			   ssid_first_str) != 1) {
			android_printf("ERROR!! SSID FIRST not "
				       "found for firmware log levels\n");
			continue;
		}
		while (*arg_ptr != ssid_first_str[0])
			arg_ptr++;
		arg_ptr += strnlen(ssid_first_str, MAX_STR_LEN);

		/*
		 * Get SSID-LAST
		 */
		if (sscanf(arg_ptr, "%"STR(MAX_STR_LEN)"s",
			   ssid_last_str) != 1) {
			android_printf("ERROR!! SSID LAST not "
				       "found for firmware log levels\n");
			continue;
		}
		while (*arg_ptr != ssid_last_str[0])
			arg_ptr++;
		arg_ptr += strnlen(ssid_last_str, MAX_STR_LEN);

		/*
		 * Get SSID Log-level
		 */
		if (sscanf(arg_ptr, "%"STR(MAX_STR_LEN)"s",
			   ssid_log_level_str) != 1) {
			android_printf("ERROR!! SSID LOG-LEVEL not "
				       "found for firmware log levels\n");
			continue;
		}

		/*
		 * Get the Subsytem Identifier (SSID) value
		 * from the macro string
		 */
		ssid_first =
		(int16_t)get_macro_number(ssid_first_str,
					  ssid_mask_table,
					  ARRAY_SIZE(ssid_mask_table));
		if (ssid_first < 0) {
			android_printf("ERROR!! SSID_FIRST %s not valid\n",
				       ssid_first_str);
			continue;
		}
		ssid_last =
		(int16_t) get_macro_number(ssid_last_str,
					   ssid_mask_table,
					   ARRAY_SIZE(ssid_mask_table));
		if (ssid_last < 0) {
			android_printf("ERROR!! SSID_LAST %s not valid\n",
				       ssid_last_str);
			continue;
		}

		/*
		 * Get the log level value from the macro string
		 */
		ssid_log_level = get_macro_number(ssid_log_level_str,
						  log_level_table,
						  ARRAY_SIZE(log_level_table));
		if (ssid_log_level < 0) {
			android_printf("ERROR!! LOG-LEVEL %s not valid\n",
				       ssid_log_level_str);
			continue;
		}
		if (ssid_last < ssid_first) {
			android_printf("ERROR!! LAST %s less than FIRST %s\n",
					ssid_last_str, ssid_first_str);
			continue;
		}
		android_printf("SSID FIRST %hd LAST %hd log-level 0x%02x\n",
			       ssid_first, ssid_last, ssid_log_level);

		/*
		 * Get the number of SSIDs we are sending to the firmware
		 */
		num_ssids = (ssid_last - ssid_first) + 1;
		buf_size = ((num_ssids * sizeof(ssid_log_level)) +
			   DIAG_HDR_SIZE);
		buf  = malloc(buf_size);
		if (!buf) {
			android_printf("ERROR!! Failed to allocate buf of size %d",
				       buf_size);
			break;
		}
		buf_ptr = buf;
		buf_ptr[0] = 0x7D; /* Extended command */
		buf_ptr[1] = 0x04; /* Set subsystem runtime mask */
		buf_ptr += 2;

		*((uint16_t *)buf_ptr) = ssid_first;
		buf_ptr += sizeof(ssid_first);

		*((uint16_t *)buf_ptr) = ssid_last;
		buf_ptr += sizeof(ssid_last);

		/* padding */
		*((uint16_t *)buf_ptr) = 0;
		buf_ptr += 2;

		/*
		 * Now fill up with the SSID-Log levels
		 */
		for (cnt = 0; cnt < num_ssids; cnt++) {
			*((uint32_t *)buf_ptr) = ssid_log_level;
			buf_ptr += 4;
		}
		if (diag_callback_send_data(data_primary, buf, buf_size)) {
			android_printf("ERROR!! Failed to send debug masks\n");
			break;
		}
		firmware_log_levels_default = 0;
		// keep it at last to free allocated buffer
		free(buf);
	}
}
#endif /* NO_DIAG_SUPPORT */

void read_config_file(void) {

	FILE *fp = NULL;
	char line_string[MAX_STR_LEN + 1];
	char *line;
	char string[MAX_STR_LEN + 1];
	static int path_flag = 0;
	static int size_flag = 0;
	int real_time_write_flag = 0;
	int fw_archive_flag = 0;
	int host_archive_flag = 0;
	int archive_flag = 0;
	int pktlog_archive_flag = 0;
	int log_buff_flag = 0;
	int pktlog_log_buff_flag = 0;
	int fw_log_level_config_file_flag = 0;
	int host_log_flag = 0;
	int fw_log_flag = 0;
	int memory_threshold_flag = 0;
	int enable_flush_log_flag = 0;
	int disable_fw_parsing_flag = 0;
	int log_storage = 0;

	fp = fopen(cnssdiag_config_file, "a+");
	if (NULL != fp) {
		fseek(fp, 0, SEEK_SET);
		while (!feof(fp)) {
			memset(line_string, 0, sizeof(line_string));
			if (fgets(line_string, sizeof(line_string), fp) == NULL)
				continue;
			line = line_string;
			line = line_trim(line);
			if (*line == '#')
				continue;
			else {
				sscanf(line, "%"STR(MAX_STR_LEN)"s", string);
				if (strcmp(string, "LOG_PATH_FLAG") == 0) {
					sscanf((line + strlen("LOG_PATH_FLAG")
						+ FLAG_VALUE_OFFSET),
							"%"STR(MAX_STR_LEN)"s", string);
					log_path_flag = (int)strtol(string, (char **)NULL, 10);
					path_flag = 1;
					debug_printf("file_path=%d\n", log_path_flag);
				}
				else if (strcmp(string, "MAX_LOG_FILE_SIZE") == 0) {
					sscanf((line +	strlen("MAX_LOG_FILE_SIZE") +
						FLAG_VALUE_OFFSET),
							 "%"STR(MAX_STR_LEN)"s", string);
					max_file_size = (int)strtol(string, (char **)NULL, 10);
					if ((max_file_size > 0) &&
						(max_file_size <= MAX_FILE_SIZE_FROM_USER_IN_MB)) {
						max_file_size = max_file_size * (1024) * (1024);
					} else {
						max_file_size = 0;
					}
					size_flag = 1;
					debug_printf("max_file_size=%d\n", max_file_size);
				} else if (strcmp(string, "REAL_TIME_WRITE")
						== 0) {
					real_time_write_flag = 1;
					sscanf((line +
						strlen("REAL_TIME_WRITE") +
						FLAG_VALUE_OFFSET),
						"%"STR(MAX_STR_LEN)"s",
						string);
					real_time_write =
					(int)strtol(string, (char **)NULL, 10);
					/*
					 * Should be 0 or non-zero
					 */
					real_time_write =
						(!(!real_time_write));
					debug_printf("real_time_write=%d\n",
						     real_time_write);
				}

				else if (strcmp(string, "MAX_ARCHIVES") == 0) {
					sscanf((line +	strlen("MAX_ARCHIVES") +
						FLAG_VALUE_OFFSET),
							 "%"STR(MAX_STR_LEN)"s", string);
					max_archives = (int)strtol(string, (char **)NULL, 10);
					if (max_archives >= 50)
						max_archives = 50;
					archive_flag = 1;
					debug_printf("max_archives=%d\n", max_archives);
				}
				else if (strcmp(string, "MAX_FW_ARCHIVES") == 0) {
					sscanf((line +	strlen("MAX_FW_ARCHIVES") +
						FLAG_VALUE_OFFSET),
							 "%"STR(MAX_STR_LEN)"s", string);
					max_fw_archives = (int)strtol(string, (char **)NULL, 10);
					if (max_fw_archives >= 50)
						max_fw_archives = 50;
					fw_archive_flag = 1;
					debug_printf("max_fw_archives=%d\n", max_fw_archives);
				}
				else if (strcmp(string, "MAX_HOST_ARCHIVES") == 0) {
					sscanf((line +	strlen("MAX_HOST_ARCHIVES") +
						FLAG_VALUE_OFFSET),
							 "%"STR(MAX_STR_LEN)"s", string);
					max_host_archives = (int)strtol(string, (char **)NULL, 10);
					if (max_host_archives >= 5)
						max_host_archives = 5;
					host_archive_flag = 1;
					debug_printf("max_host_archives=%d\n", max_host_archives);
				}
				else if (strcmp(string, "MAX_PKTLOG_ARCHIVES") == 0) {
					sscanf((line + strlen("MAX_PKTLOG_ARCHIVES") +
						FLAG_VALUE_OFFSET),
							 "%99s", string);
					max_pktlog_archives = (int)strtol(string, (char **)NULL, 10);
					pktlog_archive_flag = 1;
					debug_printf("max_pktlog_archives=%d\n", max_pktlog_archives);
				}
				else if (strcmp(string, "AVAILABLE_MEMORY_THRESHOLD") == 0) {
					sscanf((line +	strlen("AVAILABLE_MEMORY_THRESHOLD") +
						FLAG_VALUE_OFFSET), "%"STR(MAX_STR_LEN)"s", string);
					free_mem_threshold = (int)strtol(string, (char **)NULL, 10);
					memory_threshold_flag = 1;
					debug_printf("free_mem_threshold=%d\n", free_mem_threshold);
				} else if (strcmp(string, "LOG_STORAGE_PATH") == 0) {
					sscanf((line +	strlen("LOG_STORAGE_PATH") +
						FLAG_VALUE_OFFSET), "%"STR(MAX_STR_LEN)"s", string);
					if (strlen(string) != 0)
						strlcpy(log_capture_loc, string, sizeof(log_capture_loc));
					android_printf("log_capture_location  = %s", log_capture_loc);
					log_storage = 1;

				} else if (strcmp(string, "MAX_LOG_BUFFER") == 0) {
					sscanf((line +
						strlen("MAX_LOG_BUFFER") +
						FLAG_VALUE_OFFSET),
						"%"STR(MAX_STR_LEN)"s", string);
					max_buff_size = (int)strtol(string, (char **)NULL, 10);
					if ((max_buff_size > 0) &&
						(max_buff_size <= MAX_FILE_SIZE_FROM_USER_IN_MB)) {
						max_buff_size = max_buff_size * (1024) * (1024);
					} else {
						max_buff_size = 0;
					}
					log_buff_flag = 1;
					debug_printf("max_buff_size=%d\n",
						     max_buff_size);
				} else if (strcmp(string, "FW_RADIO_MASK") == 0) {
					sscanf((line +
						strlen("FW_RADIO_MASK") +
						FLAG_VALUE_OFFSET),
						"%"STR(MAX_STR_LEN)"s", string);
					disable_radio = (uint16_t)strtol(string, (char **)NULL, 10);
					debug_printf("disable_radio=%X\n",
						     disable_radio);
				} else if (strcmp(string, "MAX_PKTLOG_BUFFER") == 0) {
					sscanf((line +
						strlen("MAX_PKTLOG_BUFFER") +
						FLAG_VALUE_OFFSET),
						"%99s", string);
					max_pktlog_buff_size = (int)strtol(string, (char **)NULL, 10);
					if ((max_pktlog_buff_size > 0) &&
						(max_pktlog_buff_size <= MAX_PACKETLOG_BUFFER_LIMIT)) {
						max_pktlog_buff_size = max_pktlog_buff_size * (1024) * (1024);
					} else {
						max_pktlog_buff_size = 0;
					}
					pktlog_log_buff_flag = 1;
					debug_printf("max_pktlog_buff_size=%d\n",
						     max_pktlog_buff_size);
				} else if (strcmp(string, "HOST_LOG_FILE") == 0) {
					sscanf((line +
						strlen("HOST_LOG_FILE") +
						FLAG_VALUE_OFFSET), "%"STR(MAX_STR_LEN)"s",
						string);
					if (strlen(string) != 0)
						strlcpy(hbuffer_log_file,
							string,
							sizeof(hbuffer_log_file));

					android_printf("Host_logs_location  = %s",
							 hbuffer_log_file);
					host_log_flag = 1;
				} else if (strcmp(string, "HOST_LOG_FILE_CUSTOM_LOC") == 0) {
					sscanf((line +
						strlen("HOST_LOG_FILE_CUSTOM_LOC") +
						FLAG_VALUE_OFFSET), "%"STR(MAX_STR_LEN)"s",
						string);
					if (strlen(string) != 0)
						strlcpy(log_file[HOST_LOG_FILE].name,
							string,
							sizeof(log_file[HOST_LOG_FILE].name));
					android_printf("Host_logs_custom_location  = %s",
							 log_file[HOST_LOG_FILE].name);
					host_log_flag = 1;
				} else if (strcmp(string, "FIRMWARE_LOG_FILE") == 0) {
					sscanf((line +
						strlen("FIRMWARE_LOG_FILE") +
						FLAG_VALUE_OFFSET),
					       "%"STR(MAX_STR_LEN)"s", string);
					if (strlen(string) != 0)
						strlcpy(fbuffer_log_file,
							string,
							sizeof(fbuffer_log_file));

					android_printf("firmware_logs_location  = %s",
						       fbuffer_log_file);
					fw_log_flag = 1;
				} else if (strcmp(string, "FIRMWARE_LOG_FILE_CUSTOM_LOC") == 0) {
					sscanf((line +
						strlen("FIRMWARE_LOG_FILE_CUSTOM_LOC") +
						FLAG_VALUE_OFFSET),
					       "%"STR(MAX_STR_LEN)"s", string);
					if (strlen(string) != 0)
						strlcpy(log_file[FW_LOG_FILE].name,
							string,
							sizeof(log_file[FW_LOG_FILE].name));
					android_printf("firmware_logs_custom_location  = %s",
						       log_file[FW_LOG_FILE].name);
					fw_log_flag = 1;
				} else if (strcmp(string, "ENABLE_FLUSH_LOG") == 0) {
					sscanf((line + strlen("ENABLE_FLUSH_LOG") +
						FLAG_VALUE_OFFSET),
							 "%99s", string);
					enable_flush_log = (int)strtol(string, (char **)NULL, 10);
					enable_flush_log_flag = 1;
					android_printf("enable_flush_log = %d", enable_flush_log);
#ifndef NO_DIAG_SUPPORT
				} else if (strcmp(string,
						  "FW_LOG_LEVELS") == 0) {
					int ret;

					ret = sscanf((line +
						     strlen("FW_LOG_LEVELS") +
						     FLAG_VALUE_OFFSET),
						     "%"STR(MAX_STR_LEN)"s",
						     string);
					if (ret != 1) {
						android_printf("ERROR!! "
							       "No '{' found "
							       "after "
							       "FW_LOG_LEVELS"
							       "\n");
                                                fclose(fp);
						return;
					}
					if (strlen(string) == 0) {
						android_printf("ERROR!! "
							       "FW_LOG_LEVELS "
							       "need to be "
							       "followed by "
							       "values\n");
						exit(1);
					}
					if (strncmp(string, "{", 1) != 0) {
						android_printf("ERROR!! "
							       "FW_LOG_LEVELS "
							       "need to be "
							       "specified "
							       "after '{'\n");

						exit(1);
					}

					read_config_fw_log_levels(fp);
					fw_log_level_config_file_flag = 1;
				} else if (strcmp(string,
						  "DISABLE_FW_LOGS_PARSING") == 0) {
					sscanf((line + strlen("DISABLE_FW_LOGS_PARSING")
						+ FLAG_VALUE_OFFSET),
							"%"STR(MAX_STR_LEN)"s", string);
					disable_fw_logs_parsing = (int)strtol(string, (char **)NULL, 10);
					disable_fw_parsing_flag = 1;
					android_printf("DISABLE_FW_LOGS_PARSING:%d", disable_fw_logs_parsing);
#endif /* NO_DIAG_SUPPORT */
				} else
					continue;
				}
				if ((1 == path_flag) && (1 == size_flag) &&
				    (archive_flag == 1) && (fw_archive_flag == 1) &&
				    (host_archive_flag == 1) && (pktlog_archive_flag == 1)
				    && (memory_threshold_flag) && log_storage &&
				    log_buff_flag && host_log_flag && fw_log_flag &&
				    pktlog_log_buff_flag && enable_flush_log_flag &&
				    fw_log_level_config_file_flag && real_time_write_flag &&
				    disable_fw_parsing_flag) {
					break;
				}
			}
			if (!path_flag)
				fprintf(fp, "LOG_PATH_FLAG = %d\n", log_path_flag);
			if (!size_flag)
				fprintf(fp, "MAX_LOG_FILE_SIZE = %d\n", MAX_FILE_SIZE /((1024) * (1024)));
			if (!archive_flag)
				fprintf(fp, "MAX_ARCHIVES = %d\n", MAX_FILE_INDEX);
			if (!fw_archive_flag)
				fprintf(fp, "MAX_FW_ARCHIVES = %d\n", MAX_FW_FILE_INDEX);
			if (!host_archive_flag)
				fprintf(fp, "MAX_HOST_ARCHIVES = %d\n", MAX_HOST_FILE_INDEX);
			if (!pktlog_archive_flag)
				fprintf(fp, "MAX_PKTLOG_ARCHIVES = %d\n", max_pktlog_archives);
			if (! log_storage)
				fprintf(fp, "LOG_STORAGE_PATH = %s\n", log_capture_loc);
			if (!memory_threshold_flag)
				fprintf(fp, "AVAILABLE_MEMORY_THRESHOLD = %d\n", FREE_MEMORY_THRESHOLD);
			if (!log_buff_flag)
				fprintf(fp, "MAX_LOG_BUFFER = %d\n",
					DEFAULT_BUFFER_SIZE_MAX/((1024) * (1024)));
			if (!pktlog_log_buff_flag)
				fprintf(fp, "MAX_PKTLOG_BUFFER = %d\n",
					DEFAULT_PKTLOG_BUFFER_SIZE/((1024) * (1024)));
			if (!host_log_flag)
				fprintf(fp, "HOST_LOG_FILE = %s\n",
					hbuffer_log_file);
			if (!fw_log_flag)
				fprintf(fp, "FIRMWARE_LOG_FILE = %s\n",
					fbuffer_log_file);
			if (!enable_flush_log_flag)
				fprintf(fp, "ENABLE_FLUSH_LOG = %d\n", enable_flush_log);
			if (!real_time_write_flag)
				fprintf(fp, "REAL_TIME_WRITE = %d\n",
					real_time_write);
			if (!disable_fw_parsing_flag)
				fprintf(fp, "DISABLE_FW_LOGS_PARSING = %d\n",
					disable_fw_logs_parsing);
	}
	else {
		debug_printf("%s(%s): Configuration file not present "
				"set defualt log file path to internal "
				"sdcard\n", __func__, strerror(errno));
	}
	if (fp)
		fclose(fp);
}



void cnss_open_log_file(int max_size_reached, enum fileType type)
{
	struct stat st;
	int ret;
	int32_t file_version;

	if (log_path_flag == WRITE_TO_FILE_DISABLED) {
		optionflag &= ~(LOGFILE_FLAG);
		debug_printf("%s: write to file flag is disabled\n", __func__);
	}

	do {
		if (!max_size_reached)
			log_file[type].index = 0;

		if (log_path_flag != WRITE_TO_CUSTOM_LOCATION) {
			if(stat(wlan_log_dir_path, &st) == 0 &&
					S_ISDIR(st.st_mode)) {
				android_printf("%s: directory %s created",
						__func__, wlan_log_dir_path);
				chmod(wlan_log_dir_path, S_IRWXU | S_IRWXG);
			}
			else {
				ret = mkdir(wlan_log_dir_path, 770);
				android_printf("%s: create directory %s "
						"ret = %d errno= %d", __func__,
						wlan_log_dir_path, ret, errno);
			}
			readDir(wlan_log_dir_path, type);
		}

		if (NULL == log_file[type].fp) {
			if (max_size_reached) {
				log_file[type].fp = fopen(log_file[type].name, "w");
			} else {
				log_file[type].fp = fopen(log_file[type].name, "a+");
				if ((log_file[type].fp != NULL) &&
						(ftell(log_file[type].fp) >=
							max_file_size) && (type != PKTLOG_FILE)) {
					if ((avail_space  < free_mem_threshold) &&
							(log_path_flag ==
							WRITE_TO_INTERNAL_SDCARD)) {
						android_printf("Device free memory is insufficient");
						break;
					}
					fflush(log_file[type].fp);
					fclose(log_file[type].fp);
					backup_file(type);
					log_file[type].fp = fopen(log_file[type].name, "w");
				} else if (type != PKTLOG_FILE) {
					android_printf("failed to open file a+ mode or file"
						" size %ld is less than max_file_size %d\n",
						(log_file[type].fp != NULL)?
						ftell(log_file[type].fp) : 0,
						max_file_size);
				}
			}
			if (NULL == log_file[type].fp) {
				android_printf("Failed to open file %s: %d\n",
						log_file[type].name, errno);
			} else {
				chmod(log_file[type].name, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			}
		}

		if (NULL == log_file[type].fp) {
			if (MAX_RETRY_COUNT != delayed_count) {
				debug_printf("%s: Sleep and poll again for %s "
						" sdcard\n", __func__,
						(log_path_flag == 1) ? "internal" : "external");
				sleep(DELAY_IN_S);
				delayed_count++;
			}
			else {
				delayed_count = 0;
				if (log_path_flag == WRITE_TO_EXTERNAL_SDCARD) {
					log_path_flag = WRITE_TO_INTERNAL_SDCARD;
					debug_printf("%s: External sdcard not mounted try for"
							" internal sdcard ", __func__);
					continue;
				}
				else {
					debug_printf("%s: Internal sdcard not yet mounted"
						" Disable writing logs to a file\n", __func__);
					log_path_flag = WRITE_TO_FILE_DISABLED;
					break;
				}
			}
		} else {
			if (type == FW_LOG_FILE) {
				for (int i = 0; i < NUM_OF_DRIVERS; i++) {
					file_version = getDataMscFileVersion(i);
					if (file_version > 0) {
						fprintf(log_file[type].fp, "FILE_VERSION: %d\n",
							file_version);
						break;
					}
				}
			}
			break;
		}
	} while(1);
	return;
}


#ifndef NO_DIAG_SUPPORT
#ifdef CNSS_DIAG_PLATFORM_WIN
/* Callback for receiving data from Diag Router*/
int diag_callback(unsigned char *ptr, int len, void *context_data)
{
	if (!ptr || (len<=0)) {
		return 0;
	}
	process_corediag_fw_msg(ptr, len);
	return 0;
}
#endif
#endif


/*
 * Process FW debug, FW event and FW log messages
 * Read the payload and process accordingly.
 *
 */
void process_cnss_diag_msg(int cmd, tAniCLDHdr *wnl)
{
	uint8_t *dbgbuf;
	uint8_t *eventbuf = ((uint8_t *)wnl + sizeof(wnl->radio));
	uint16_t diag_type = 0;
#ifndef NO_DIAG_SUPPORT
	uint32_t event_id = 0;
#endif /* NO_DIAG_SUPPORT */
	uint16_t length = 0;
	struct dbglog_slot *slot;
	uint32_t dropped = 0;

	dbgbuf = eventbuf;
	diag_type = *(uint16_t *)eventbuf;
	eventbuf += sizeof(uint16_t);

	length = *(uint16_t *)eventbuf;
	eventbuf += sizeof(uint16_t);

	if (cmd == WLAN_NL_MSG_CNSS_HOST_MSG) {
		if ((wnl->wmsg.type == ANI_NL_MSG_LOG_HOST_MSG_TYPE) ||
			(wnl->wmsg.type == ANI_NL_MSG_LOG_MGMT_MSG_TYPE)) {
			if ((optionflag & LOGFILE_FLAG) && (!doesFileExist(log_file[HOST_LOG_FILE].name))
				&& (log_path_flag == WRITE_TO_INTERNAL_SDCARD)&& log_file[HOST_LOG_FILE].fp) {
				if (fclose(log_file[HOST_LOG_FILE].fp) == EOF)
					perror("Failed to close host file ");
				log_file[HOST_LOG_FILE].index = 0;
				log_file[HOST_LOG_FILE].fp = fopen(log_file[HOST_LOG_FILE].name, "w");
				if (log_file[HOST_LOG_FILE].fp == NULL) {
					debug_printf("Failed to create a new file");
				}
				else {
					chmod(log_file[HOST_LOG_FILE].name, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				}
			}
			if (!t_log_timer.start_cnss_timer) {
				android_printf("%s: start cnss log timer \n", __func__);
				timer_settime(t_log_timer.timerid, 0, &t_log_timer.value, NULL);
				t_log_timer.start_cnss_timer = TRUE;
			}
			process_cnss_host_message(wnl, optionflag);
		}
		else if (wnl->wmsg.type == ANI_NL_MSG_LOG_FW_MSG_TYPE && !(optionflag & DISABLE_FWLOG_FLAG)) {
			if ((optionflag & LOGFILE_FLAG) && (!doesFileExist(log_file[FW_LOG_FILE].name))
				&&(log_path_flag == WRITE_TO_INTERNAL_SDCARD)&& log_file[FW_LOG_FILE].fp) {
				if (fclose(log_file[FW_LOG_FILE].fp) == EOF)
					perror("Failed to close fw file ");
				log_file[FW_LOG_FILE].index = 0;
				log_file[FW_LOG_FILE].fp = fopen(log_file[FW_LOG_FILE].name, "w");
				if (log_file[FW_LOG_FILE].fp == NULL) {
					debug_printf("Failed to create a new file");
				}
				else {
					chmod(log_file[FW_LOG_FILE].name, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				}
			}
			t_log_timer.fw_logs_collected = 1;
			process_pronto_firmware_logs(wnl, optionflag);
		}
	} else if (cmd == WLAN_NL_MSG_CNSS_HOST_EVENT_LOG &&
		   (wnl->wmsg.type == ANI_NL_MSG_LOG_HOST_EVENT_LOG_TYPE)) {
		process_cnss_host_diag_events_log(
		    (char *)((char *)&wnl->wmsg.length +
			      sizeof(wnl->wmsg.length)),
		    optionflag);
	} else {
		if (diag_type == DIAG_TYPE_FW_EVENT && !(optionflag & DISABLE_FWLOG_FLAG)) {
			eventbuf += sizeof(uint32_t);
#ifndef NO_DIAG_SUPPORT
			event_id = *(uint32_t *)eventbuf;
#endif /* NO_DIAG_SUPPORT */
			eventbuf += sizeof(uint32_t);
#ifndef NO_DIAG_SUPPORT
			if (optionflag & QXDM_FLAG) {
				if (length)
					event_report_payload(event_id, length,
							     eventbuf);
				else
					event_report(event_id);
			}
#endif

		} else if (diag_type == DIAG_TYPE_FW_LOG && !(optionflag & DISABLE_FWLOG_FLAG)) {
			/* Do nothing for now */
		} else if (diag_type == DIAG_TYPE_FW_DEBUG_MSG && !(optionflag & DISABLE_FWLOG_FLAG)) {
			slot =(struct dbglog_slot *)dbgbuf;
			length = get_le32((uint8_t *)&slot->length);
			dropped = get_le32((uint8_t *)&slot->dropped);
			dbglog_parse_debug_logs(&slot->payload[0],
				    length, dropped, wnl->radio);
		} else if (diag_type == DIAG_TYPE_FW_MSG && !(optionflag & DISABLE_FWLOG_FLAG)) {
			uint32_t version = 0;
			slot = (struct dbglog_slot *)dbgbuf;
			length = get_32((uint8_t *)&slot->length);
			version = get_le32((uint8_t *)&slot->dropped);
				if ((optionflag & LOGFILE_FLAG) &&
					(!doesFileExist(log_file[FW_LOG_FILE].name)) &&
					(log_path_flag == WRITE_TO_INTERNAL_SDCARD)&&
					log_file[FW_LOG_FILE].fp) {
					if (fclose(log_file[FW_LOG_FILE].fp) == EOF)
						perror("Failed to close fw file ");
					log_file[FW_LOG_FILE].index = 0;
					log_file[FW_LOG_FILE].fp = fopen(log_file[FW_LOG_FILE].name, "w");
					if (log_file[FW_LOG_FILE].fp == NULL) {
						debug_printf("Failed to create a new file");
					}
					else {
						chmod(log_file[FW_LOG_FILE].name, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					}
				}
				t_log_timer.fw_logs_collected = 1;

				if (disable_fw_logs_parsing) {
					if ((optionflag & LOGFILE_FLAG) &&
					     log_file[FW_LOG_FILE].fp) {
						struct cnss_fw_data fwdata;

						memset(&fwdata, 0, sizeof(struct cnss_fw_data));
						/**
						 * data_format_version = 0 should save
						 * radio_id along with HEADER in the file.
						 */
						fwdata.magic = 0xABACABEA;
						fwdata.radio_id = wnl->radio;
						fwdata.data_format_version = 0;
						cnss_write_buf_logs(sizeof(struct cnss_fw_data),
								    (char *)&fwdata,
								    FW_LOG_FILE);
						cnss_write_buf_logs(sizeof(struct dbglog_slot),
								    (char *)dbgbuf, FW_LOG_FILE);
					}
				}
				process_diagfw_msg((char *)&slot->payload[0], length, optionflag,
						   version, sock_fd, wnl->radio,
						   disable_fw_logs_parsing);
		} else if (diag_type == DIAG_TYPE_HOST_MSG) {
			slot = (struct dbglog_slot *)dbgbuf;
			length = get_32((uint8_t *)&slot->length);
			process_diaghost_msg(slot->payload, length);
		} else {
			/* Do nothing for now */
		}
	}
}

/*
 * Open the socket and bind the socket with src
 * address. Return the socket fd if sucess.
 *
 */
static int32_t create_nl_socket()
{
	int32_t ret;
	int32_t sock_fd;

	sock_fd = socket(PF_NETLINK, SOCK_RAW, host_log_socket_protocol);
	if (sock_fd < 0) {
		fprintf(stderr, "Socket creation failed sock_fd 0x%x \n",
		        sock_fd);
		return -1;
	}

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_groups = 0x01;
	src_addr.nl_pid = getpid(); /* self pid */

	ret = bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
	if (ret < 0)
		{
		close(sock_fd);
		return ret;
	}
	return sock_fd;
}

static int initialize()
{
	char *mesg = "Hello";

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0; /* For Linux Kernel */
	dest_addr.nl_groups = 0; /* unicast */

	if (nlh) {
		free(nlh);
		nlh = NULL;
	}
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSG_SIZE));
	if (nlh == NULL) {
		android_printf("%s Cannot allocate memory for nlh",
			__func__);
		return -1;
	}
	memset(nlh, 0, NLMSG_SPACE(MAX_MSG_SIZE));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_MSG_SIZE);
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_type = WLAN_NL_MSG_CNSS_DIAG;
	nlh->nlmsg_flags = NLM_F_REQUEST;

	memcpy(NLMSG_DATA(nlh), mesg, strlen(mesg));

	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	return 1;
}

void* get_packet_logs(void *arg)
{
	int rc = 0;
	(void)arg;
	if (optionflag & LOG_BUFF_FLAG) {
		android_printf("%s: exit: can't run with local_buffer mode", __func__);
		pthread_exit(NULL);
	}
	android_printf("%s: pktlogger thread starting", __func__);
	while (cnss_diag_running) {
		if (!cnss_diag_running) {
			break;
		}
		rc = capture_pktlog_data();
		if (rc < 0) {
			android_printf("%s: failed to capture the pktlog: %d, will retry after %d seconds", __func__, rc, pktlogger_interval);
		}
		sleep(pktlogger_interval);
	}
	android_printf("%s: pktlogger thread exiting", __func__);
	return NULL;
}

int init_log_file()
{
	boolean enable_log_file[LOG_FILE_MAX] = {FALSE, FALSE, FALSE};
	int i;
	int rc = 0;
	unsigned int buffer_size;

	if (optionflag & LOGFILE_FLAG) {
		enable_log_file[HOST_LOG_FILE] = TRUE;
		enable_log_file[FW_LOG_FILE] = TRUE;
	}
	if (max_pktlog_archives > 0 || max_pktlog_archives == -1)
		enable_log_file[PKTLOG_FILE] = TRUE;
	if (optionflag & LOGFILE_QXDM_FLAG) {
		enable_log_file[HOST_QXDM_LOG_FILE] = TRUE;
		enable_log_file[FW_QXDM_LOG_FILE] = TRUE;
	}
	if (log_path_flag == WRITE_TO_EXTERNAL_SDCARD) {
		snprintf(wlan_log_dir_path, sizeof(wlan_log_dir_path),
				"%s", "/mnt/media_rw/sdcard1/wlan_logs/");
	} else if (log_path_flag == WRITE_TO_INTERNAL_SDCARD) {
		snprintf(wlan_log_dir_path, sizeof(wlan_log_dir_path),
				"%s", log_capture_loc);
	}

	for (i = HOST_LOG_FILE; i < LOG_FILE_MAX; i++) {
		if ((i == BUFFER_HOST_FILE) || (i == BUFFER_FW_FILE)) {
			if (optionflag & LOG_BUFF_FLAG) {
				t_buffer *buff_ctx = NULL;
				if (i == BUFFER_HOST_FILE)
					snprintf(log_file[i].name,
						 sizeof(log_file[i].name),
						 "%s", hbuffer_log_file);
				else
					snprintf(log_file[i].name,
						 sizeof(log_file[i].name),
						 "%s", fbuffer_log_file);
				log_file[i].buf = (char*) malloc(sizeof(t_buffer));
				if (!log_file[i].buf)
					goto free_bufs;

				memset(log_file[i].buf, 0x00, (sizeof(t_buffer)));
				buff_ctx = (t_buffer *)log_file[i].buf;
				buff_ctx->start = (unsigned char *) malloc(EACH_BUF_SIZE);
				if (buff_ctx->start == NULL) {
					free(buff_ctx);
					goto free_bufs;
				}
				buff_ctx->end = buff_ctx->start;
				buff_ctx->next = buff_ctx;

				log_file[i].buf_ptr = log_file[i].buf;
				log_file[i].fp = NULL;
				if (pthread_mutex_init(&log_file[i].buff_lock,
						       NULL)) {
					android_printf("Failed to initialize buff_lock");
					goto free_bufs;
				}
			}
		} else {
			if (enable_log_file[i] == FALSE)
				continue;
			if (log_path_flag != WRITE_TO_CUSTOM_LOCATION) {
				snprintf(log_file[i].name, sizeof(log_file[i].name),
				"%s%s%scurrent.%s", wlan_log_dir_path, log_file_name_prefix[i], multi_if_file_name, log_file_name_extn[i]);
			}
			if (!(optionflag & BUFFER_SIZE_FLAG))
				configured_buffer_size = DEFAULT_LOG_BUFFER_LIMIT;

			buffer_size = configured_buffer_size;
			if ((i == FW_LOG_FILE) && (optionflag & TARGET_BUFFER_SIZE_FLAG))
				buffer_size = target_configured_buffer_size;

			log_file[i].free_buf_mem = buffer_size;

			log_file[i].buf = (char*) malloc(buffer_size * sizeof(char));
			if (!log_file[i].buf) {
				goto free_bufs;
			}
			memset(log_file[i].buf, 0x00, (buffer_size * sizeof(char)));
			log_file[i].buf_ptr = log_file[i].buf;
			cnss_open_log_file(FALSE, i);
		}
	}

	if (optionflag & LOGFILE_QXDM_FLAG) {
		struct qmdl_file_hdr file_hdr;
		file_hdr.hdr_len = sizeof(struct qmdl_file_hdr);
		file_hdr.version = 1;
		file_hdr.data_type = 0;
		file_hdr.guid_list_count = 0;
		cnss_write_buf_logs(sizeof(struct qmdl_file_hdr), (char *)&file_hdr, HOST_QXDM_LOG_FILE);
		cnss_write_buf_logs(sizeof(struct qmdl_file_hdr), (char *)&file_hdr, FW_QXDM_LOG_FILE);
	}

	if (optionflag & PKT_LOGFILE_FLAG) {
		rc = pthread_create(&thread_pktlogger, NULL, get_packet_logs, NULL);
		if (rc < 0) {
			android_printf("Failed to create the packet logger thread, ret %d", rc);
		}
	}
	return 0;

free_bufs:
	android_printf("malloc failed, free bufs allocated so far");
	for (; i >= 0; i--) {
		if (log_file[i].buf) {
			if ((i == BUFFER_HOST_FILE) || (i == BUFFER_FW_FILE)) {
				t_buffer *buff_ctx = (t_buffer *)log_file[i].buf;
				if (buff_ctx->start)
					free(buff_ctx->start);
			}
			free(log_file[i].buf);
		}
	}
	return -1;
}

static unsigned long long getAvailableSpace(const char* path) {
	struct statvfs stat;
	if (statvfs(path, &stat) != 0) {
		return -1;
	}
	/* the available size is f_bsize * f_bavail , return in MBs */
	return (((unsigned long long)stat.f_bsize * (unsigned long long)stat.f_bavail) / (1024 * 1024));
}

static void cnss_diag_find_wlan_dev(char *chip_type)
{
    if(strcmp(chip_type,"ROME") == 0) {
        gwlan_dev = CNSS_DIAG_WLAN_ROM_DEV;
    } else if(strcmp(chip_type,"TUEFFELO") == 0) {
        gwlan_dev = CNSS_DIAG_WLAN_TUF_DEV;
    } else if(strcmp(chip_type,"NAPIER") ==0) {
        gwlan_dev = CNSS_DIAG_WLAN_NAP_DEV;
    } else if(strcmp(chip_type,"HELIUM") == 0) {
        gwlan_dev = CNSS_DIAG_WLAN_HEL_DEV;
    } else {
        gwlan_dev = CNSS_DIAG_WLAN_DEV_UNDEF;
    }
}

#ifndef CNSS_DIAG_PLATFORM_WIN
static int handle_response(struct resp_info *info, char *vendata, int datalen)
{
	int len = 0;
	if (info->subcmd == QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_INFO) {
		struct nlattr *tb_vendor[
				QCA_WLAN_VENDOR_ATTR_WIFI_INFO_GET_MAX + 1];
		nla_parse(tb_vendor, QCA_WLAN_VENDOR_ATTR_WIFI_INFO_GET_MAX,
				(struct nlattr *)vendata, datalen, NULL);
		if (tb_vendor[QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION]){
			len = nla_len(tb_vendor[
			QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION]);
			if ( len > MAX_SIZE) {
				strlcpy(hw_version, nla_data(tb_vendor[
			      QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION]),
			      MAX_SIZE);
			} else {
				strlcpy(hw_version, nla_data(tb_vendor[
			      QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION]),
			      len);
			}
		}
	} else {
		android_printf("Unsupported response type: %d", info->subcmd);
	}
	return 0;
}
#endif

int ack_handler(struct nl_msg *msg, void *arg)
{
	int *err = (int *)arg;
	*err = 0;
	return NL_STOP;
}

int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = (int *)arg;
	*ret = 0;
	return NL_SKIP;
}

int error_handler(struct sockaddr_nl *nla,
        struct nlmsgerr *err, void *arg)
{
	int *ret = (int *)arg;
	*ret = err->error;
	return NL_SKIP;
}

#ifndef CNSS_DIAG_PLATFORM_WIN
static int no_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

static int response_handler_cnss(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *mHeader;
	struct nlattr *mAttributes[NL80211_ATTR_MAX_INTERNAL + 1];
	char *vendata = NULL;
	int datalen;
	struct resp_info *info = (struct resp_info *) arg;
	int status;

	mHeader = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	nla_parse(mAttributes, NL80211_ATTR_MAX_INTERNAL,
			  genlmsg_attrdata(mHeader, 0),
			  genlmsg_attrlen(mHeader, 0), NULL);

	if (mAttributes[NL80211_ATTR_VENDOR_DATA]) {
		vendata =
		((char *)nla_data(mAttributes[NL80211_ATTR_VENDOR_DATA]));
		datalen = nla_len(mAttributes[NL80211_ATTR_VENDOR_DATA]);
		if (!vendata) {
			android_printf("Vendor data not found");
			return -1;
		}
		status = handle_response(info, vendata, datalen);

	} else {
		android_printf("NL80211_ATTR_VENDOR_DATA not found");
	status = -1;
	}

	return status;
}

static int send_nlmsg_cnss(struct nl_sock *cmd_sock, struct nl_msg *nlmsg,
			  struct resp_info *info)
{
	int err = 0;

	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb) {
		android_printf("alloc failed ");
		goto out;
	}

	err = nl_send_auto_complete(cmd_sock, nlmsg);	/* send message */
	if (err < 0) {
		android_printf("nl_send_auto_complete failed ");
		goto out;
	}

	err = 1;

	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, response_handler_cnss, info);

	while (err > 0) {	/* wait for reply */
		int res = nl_recvmsgs(cmd_sock, cb);
		if (res) {
			android_printf("nl80211: %s->nl_recvmsgs failed: %d",
				 __FUNCTION__, res);
		}
	}
out:
	nl_cb_put(cb);
	if (nlmsg)
		nlmsg_free(nlmsg);
	return err;
}

static struct nl_msg *populate_nlmsg(struct nl_msg *nlmsg)
{
	struct nlattr *attr;
	if ((attr = nla_nest_start(nlmsg, NL80211_ATTR_VENDOR_DATA)) != NULL) {
		if (nla_put_u8(nlmsg,QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION, 0)) {
			android_printf("Failed to set"
			      "QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION");
			nla_nest_end(nlmsg, attr);
			return NULL;
		}
		nla_nest_end(nlmsg, attr);
	} else {
		return NULL;
	}
	return nlmsg;
}

static struct nl_msg *prepare_nlmsg(int process_id, int cmdid)
{
	int res;
	struct nl_msg *nlmsg = nlmsg_alloc();
	int ifindex;

	if (nlmsg == NULL) {
		android_printf("Out of memory");
		return NULL;
	}

	genlmsg_put(nlmsg, /* pid = */ 0, /* seq = */ 0,
			process_id, 0, 0, NL80211_CMD_VENDOR, 0);
	res = nla_put_u32(nlmsg, NL80211_ATTR_VENDOR_ID, OUI_QCA);
	if (res < 0) {
		android_printf("Failed to put vendor id");
		goto cleanup;
	}

	res = nla_put_u32(nlmsg, NL80211_ATTR_VENDOR_SUBCMD, cmdid);
	if (res < 0) {
		android_printf("Failed to put vendor sub command");
		goto cleanup;
	}
	ifindex = if_nametoindex("wlan0");
	if (nla_put_u32(nlmsg, NL80211_ATTR_IFINDEX, ifindex) != 0) {
		android_printf("Failed to get iface index for iface: %s", "wlan0");
		goto cleanup;
	}

	return nlmsg;

cleanup:
	if (nlmsg)
		nlmsg_free(nlmsg);
	return NULL;
}

#ifdef ANDROID
static void cnss_diag_find_wlan_chip()
{
	int i , status;
	char buf[512];
	char *hw_name = NULL;
	size_t buf_len=MAX_SIZE;
	struct resp_info info;
	struct nl_sock *sock_fd = NULL;
	struct nl_msg *nlmsg = NULL;

	sock_fd = nl_socket_alloc();
	if (!sock_fd)
		goto cleanup;
	info.subcmd = QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_INFO;
	info.reply_buf = buf;
	info.reply_buf_len = buf_len;

	sock_fd->s_flags |= NL_OWN_PORT;
	sock_fd->s_local.nl_pid = getpid();
	if (nl_connect(sock_fd, NETLINK_GENERIC)) {
		android_printf("Could not connect handle \n");
		goto cleanup;
	}
	int id = genl_ctrl_resolve(sock_fd, "nl80211");
	if (id < 0) {
		android_printf("id is not valid");
		goto cleanup;
	}
	nlmsg = prepare_nlmsg(id, QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_INFO);
	if (nlmsg) {
		if (populate_nlmsg(nlmsg) == NULL) {
			android_printf("Failed to populate nl message");
			nlmsg_free(nlmsg);
			goto cleanup;
		}
		// nlmsg is freed by send_nlmsg_cnss()
		status = send_nlmsg_cnss(sock_fd, nlmsg, &info);
		if (status != 0) {
			android_printf("Failed to send nl message");
			goto cleanup;
		}
	}

	hw_name = strstr(hw_version, "HW:");
	if (!hw_name) {
		goto cleanup;
	}
	hw_name += strlen("HW:");
	gwlan_dev = CNSS_DIAG_WLAN_ROM_DEV;
	for (i = CNSS_DIAG_WLAN_ROM_DEV; i < CNSS_DIAG_WLAN_DEV_MAX; i++) {
		if (strncmp(hw_name, cnss_diag_wlan_dev_name[i], strlen(cnss_diag_wlan_dev_name[i])) == 0) {
			gwlan_dev = i;
			break;
		}
	}

cleanup:
	if(sock_fd){
		nl_socket_free(sock_fd);
		sock_fd = NULL;
	}
}
#endif
#endif

#ifdef CONFIG_CLD80211_LIB
/* Event handlers */
static int response_handler(struct nl_msg *msg, void *arg)
{
    struct genlmsghdr *header;
	struct nlattr *attrs[CLD80211_ATTR_MAX + 1];
	struct nlattr *tb_vendor[CLD80211_ATTR_MAX + 1];
	int result;

	UNUSED(arg);
	header = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	result = nla_parse(attrs, CLD80211_ATTR_MAX, genlmsg_attrdata(header, 0),
		    genlmsg_attrlen(header, 0), NULL);

	if (!result && attrs[CLD80211_ATTR_VENDOR_DATA]) {
		nla_parse(tb_vendor, CLD80211_ATTR_MAX,
				(struct nlattr *)nla_data(attrs[CLD80211_ATTR_VENDOR_DATA]),
				nla_len(attrs[CLD80211_ATTR_VENDOR_DATA]), NULL);

		if (tb_vendor[CLD80211_ATTR_DATA]) {
			android_printf("CLD80211_ATTR_DATA found len: %d",
			               nla_len(tb_vendor[CLD80211_ATTR_DATA]));
		} else {
			android_printf("%s: CLD80211_ATTR_DATA not found", __FUNCTION__);
		}
	} else
		android_printf("No valid data received");

	return NL_OK;
}

static int event_handler(struct nl_msg *msg, void *arg)
{
	struct nlattr *attrs[CLD80211_ATTR_MAX + 1];
	struct genlmsghdr *header;
	struct nlattr *tb_vendor[CLD80211_ATTR_MAX + 1];
	boolean *fetch_free_mem = (boolean *)arg;

	struct  nlmsghdr *nlh = nlmsg_hdr(msg);

	header = (struct genlmsghdr *)nlmsg_data(nlh);
	if (header->cmd == WLAN_NL_MSG_CNSS_HOST_MSG ||
	    (unsigned long)nlh->nlmsg_len > sizeof(struct dbglog_slot)) {
		int result = nla_parse(attrs, CLD80211_ATTR_MAX, genlmsg_attrdata(header, 0),
				genlmsg_attrlen(header, 0), NULL);

		if (result || !attrs[CLD80211_ATTR_VENDOR_DATA]) {
			android_printf("No valid data received");
			return 0;
		}

		nla_parse(tb_vendor, CLD80211_ATTR_MAX,
			  (struct nlattr *)nla_data(attrs[CLD80211_ATTR_VENDOR_DATA]),
			  nla_len(attrs[CLD80211_ATTR_VENDOR_DATA]), NULL);

		if (tb_vendor[CLD80211_ATTR_DATA]) {
			tAniCLDHdr *clh =
			              (tAniCLDHdr *)nla_data(tb_vendor[CLD80211_ATTR_DATA]);
			if (fetch_free_mem && (optionflag & LOGFILE_FLAG ||
						optionflag & PKT_LOGFILE_FLAG)) {
				avail_space = getAvailableSpace(log_capture_loc);
				if (avail_space != -1)
					fetch_free_mem = FALSE;
			}
#ifndef CNSS_DIAG_PLATFORM_WIN
			//Identify driver once on receiving NL MSG from driver
			if (gwlan_dev == CNSS_DIAG_WLAN_DEV_UNDEF) {
				// Default to Rome (compaitble with legacy device logging)
#ifdef ANDROID
				cnss_diag_find_wlan_chip();
#else
				gwlan_dev = CNSS_DIAG_WLAN_ROM_DEV;
#endif
			}
#endif
			process_cnss_diag_msg(header->cmd, clh);
		}
		else
			android_printf("%s: CLD80211_ATTR_DATA not found", __FUNCTION__);

	}

	return 0;
}


int send_nlmsg(void *ctx, int cmd, void *data, int len)
{
	int ret;
	struct nlattr *nla_data = NULL;
	struct nl_msg *nlmsg;

	nlmsg = cld80211_msg_alloc(ctx, cmd, &nla_data, getpid());
	if (!nlmsg) {
		android_printf("Failed to alloc nlmsg: %s\n", __FUNCTION__);
		return -1;
	}

	ret = nla_put(nlmsg, CLD80211_ATTR_DATA, len, data);
	if (ret != 0) {
		android_printf("Failed to put CLD80211_ATTR_DATA err: %d\n", ret);
		goto cleanup;
	}

	nla_nest_end(nlmsg, nla_data);

	ret = cld80211_send_recv_msg(ctx, nlmsg, response_handler, NULL);
	if (ret != 0) {
		android_printf("Failed to send msg: %d\n", ret);
	}

cleanup:
	nlmsg_free(nlmsg);
	return ret;
}
#endif /* CONFIG_CLD80211_LIB */

#ifdef WIN_AP_HOST_OPEN
int nl_fwlog_rx_cb(struct nl_msg *msg, void *arg)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	uint32_t len, radio_num, version, dev_id;
	struct nlattr *td[CNSS_NL_MAX_ATTR];
	uint32_t *buf;
	uint8_t dual_mac = 0;
	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
	genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_TESTDATA] || !tb[NL80211_ATTR_WIPHY])
		return NL_SKIP;

	nla_parse(td, CNSS_NL_MAX_ATTR, nla_data(tb[NL80211_ATTR_TESTDATA]),
		  nla_len(tb[NL80211_ATTR_TESTDATA]), NULL);

	/* Skip processing if FWLOG info is not present in the data */
	if (!td[CNSS_NL_ATTR_FWLOG])
		return NL_SKIP;

	if (td[CNSS_NL_LINK_IDX])
		radio_num = nla_get_u8(td[CNSS_NL_LINK_IDX]);
	else
		radio_num = nla_get_u32(tb[NL80211_ATTR_WIPHY]);

	/* Unique radio id upto max supported radios */

	radio_num = radio_num % NUM_OF_DRIVERS;
	buf = nla_data(td[CNSS_NL_ATTR_FWLOG]);

	dev_id = buf[FWLOG_BUF_IDX_DEV_ID];

	/* associate the device name corresponding to the radio id
	 * for extracting the right msc file
	 */

	if (td[CNSS_NL_DUAL_MAC])
		dual_mac = nla_get_u8(td[CNSS_NL_DUAL_MAC]);

	bind_radio_devicename(radio_num, dev_id, dual_mac);

	len = nla_len(td[CNSS_NL_ATTR_FWLOG]);
	version = buf[FWLOG_BUF_IDX_VERSION];

	if (disable_fw_logs_parsing) {
		if ((optionflag & LOGFILE_FLAG) &&
		     log_file[FW_LOG_FILE].fp) {
			struct cnss_fw_data fwdata;
			struct dbglog_slot dbgslot;

			memset(&fwdata, 0, sizeof(struct cnss_fw_data));
			memset(&dbgslot, 0, sizeof(struct dbglog_slot));
			/**
			 * data_format_version = 0 should save
			 * radio_id along with HEADER in the file.
			 */
			fwdata.magic = 0xABACABEA;
			fwdata.radio_id = radio_num;
			fwdata.data_format_version = 0;
			dbgslot.length = len;
			dbgslot.dropped = version;
			cnss_write_buf_logs(sizeof(struct cnss_fw_data),
					    (char *)&fwdata,
					    FW_LOG_FILE);
			cnss_write_buf_logs(sizeof(struct dbglog_slot),
					    (char *)&dbgslot, FW_LOG_FILE);
		}
	}

	process_diagfw_msg((char *)buf, len,
			   optionflag, version, 0, radio_num, disable_fw_logs_parsing);

	return 0;
}

struct handler_args {
	const char *group;
	int id;
};

static int mcast_family_handler(struct nl_msg *msg, void *arg)
{
	struct handler_args *grp = arg;
	struct nlattr *tb[CTRL_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb_mc[CTRL_ATTR_MCAST_GRP_MAX + 1];
	struct nlattr *mc_grp;
	int rem_mc_grp;

	nla_parse(tb, CTRL_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	/* Check for mcast groups tag in the data */
	if (!tb[CTRL_ATTR_MCAST_GROUPS])
		return NL_SKIP;

	/* loop to fetch all the mcast group date */
	nla_for_each_nested(mc_grp, tb[CTRL_ATTR_MCAST_GROUPS], rem_mc_grp) {

		nla_parse(tb_mc, CTRL_ATTR_MCAST_GRP_MAX,
			  nla_data(mc_grp), nla_len(mc_grp), NULL);

		/* If group name or group id tag is not present reloop */
		if (!tb_mc[CTRL_ATTR_MCAST_GRP_NAME] ||
		    !tb_mc[CTRL_ATTR_MCAST_GRP_ID])
			continue;
		else
			grp->id = nla_get_u32(tb_mc[CTRL_ATTR_MCAST_GRP_ID]);

		/* compare the group name and fetch the group id and store in arg */
		if (strncmp(nla_data(tb_mc[CTRL_ATTR_MCAST_GRP_NAME]),
		    grp->group, nla_len(tb_mc[CTRL_ATTR_MCAST_GRP_NAME])))
			continue;

		grp->id = nla_get_u32(tb_mc[CTRL_ATTR_MCAST_GRP_ID]);
		break;
	}

	return NL_SKIP;
}

void nl80211_fwlog_rx(void)
{
	struct nl_cb *cb;

	cb = nl_socket_get_cb(nl_handle);
	if (!cb)
		return;

	/* process the messages in the loop */
	while(1)
		nl_recvmsgs(nl_handle, cb);
}


int nl80211_get_mcast_id(struct nl_sock *sock, const char *family,
                         const char *group)
{
	struct nl_msg *msg;
	struct nl_cb *cb;
	int ctrl_id, ret = 0;
	struct handler_args grp = {
		.group = group,
		.id = -ENOENT,
	};

	msg = nlmsg_alloc();
	if (!msg)
		return -ENOMEM;

	/* allocate a new callback handle */
	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb) {
		ret = -ENOMEM;
		goto fail;
	}

	/* resolve the family name to get the ctrl id */
	ctrl_id = genl_ctrl_resolve(sock, "nlctrl");

	genlmsg_put(msg, 0, 0, ctrl_id, 0, 0, CTRL_CMD_GETFAMILY, 0);

	NLA_PUT_STRING(msg, CTRL_ATTR_FAMILY_NAME, family);

	ret = nl_send_auto(sock, msg);
	if (ret < 0)
		goto out;

	ret = 1;

	/* setup callbacks */
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &ret);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &ret);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &ret);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, mcast_family_handler, &grp);

	while (ret > 0)
		nl_recvmsgs(sock, cb);

	if (ret == 0)
		ret = grp.id;

nla_put_failure:

out:
	nl_cb_put(cb);
fail:
	nlmsg_free(msg);
	return ret;
}

static void nl80211_fwlog_init(void)
{
	struct nl_cb *cb;
	int ret;
	int nl_id;

	nl_handle = nl_socket_alloc();
	if (!nl_handle) {
		printf("Failed to allocate netlink socket for fwlog rx.\n");
		return;
	}

	if (genl_connect(nl_handle)) {
		printf("Failed to connect to generic netlink for fwlog rx.\n");
		goto fail;
	}

	nl_id = genl_ctrl_resolve(nl_handle, "nl80211");
	if (nl_id < 0) {
		printf("nl80211 ctrl not found.\n");
		goto fail;
	}

	/* get the mcast id and subscribe to the testmode events */
	ret = nl80211_get_mcast_id(nl_handle, "nl80211", "testmode");

	if (ret >= 0) {
		ret = nl_socket_add_membership(nl_handle, ret);
		if (ret) {
			goto fail;
		}
	} else {
		goto fail;
	}

	/* disable sequence check */
	nl_socket_disable_seq_check(nl_handle);

	/* setup rx callback */
	cb = nl_socket_get_cb(nl_handle);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, nl_fwlog_rx_cb, NULL);

	return;

fail:
	nl_socket_free(nl_handle);
}
#endif /*WIN_AP_HOST_OPEN*/

static boolean fetch_free_mem = TRUE;

#ifdef CONFIG_CLD80211_LIB
static void cld80211_handle_event(int events, struct nl_sock *sock,
				  struct nl_cb *cb)
{
	if (events & POLLERR) {
		ALOGE("%s: Error reading from socket", getprogname());
		cld80211_recv_msg(sock, cb);
	} else if (events & POLLHUP) {
		ALOGE("%s: Remote side hung up", getprogname());
	} else if (events & POLLIN) {
		cld80211_recv_msg(sock, cb);
	} else {
		ALOGE("%s: Unknown event - %0x", getprogname(), events);
	}
}
#endif

#ifdef ANDROID
#ifdef FEATURE_DEBUG_DUMP
static bool dumpinprogress = false;
static int inotify_fd = -1;
static int watch_fd = -1;
static int inotify_fd_cset_ms = -1;
static int watch_fd_cset_ms = -1;
/**
* wlan_debug_dump_open_fd(): This api opens an inotify fd, and binds it to
* DEBUG_DUMP_IN_PROGRESS_NODE. It enables poll and allows the request handler
* to listen IN_MODIFY event on the attached file.
*/
void wlan_debug_dump_open_fd() {
	static int iflags = IN_CLOEXEC | IN_NONBLOCK;
	static uint32_t mask = IN_MODIFY;
	int ret, fd;

	fd = access(DEBUG_DUMP_IN_PROGRESS_NODE, F_OK);
	if(fd == -1) {
		android_printf("%s doesn't exist", DEBUG_DUMP_IN_PROGRESS_NODE);
		return;
	}
	fd = access(DEBUG_DUMP_IN_PROGRESS_NODE, R_OK);
	if(fd == -1) {
		android_printf("%s file read permission doesn't exist", DEBUG_DUMP_IN_PROGRESS_NODE);
		return;
	}

	inotify_fd = inotify_init1(iflags);
	if(inotify_fd < 0) {
		android_printf("Failed to perform inotify init: %d", errno);
		return;
	}

	ret = inotify_add_watch(inotify_fd, DEBUG_DUMP_IN_PROGRESS_NODE, mask);
	if(ret < 0) {
		android_printf("Failed to add inotify watch on %s: %d",
			       DEBUG_DUMP_IN_PROGRESS_NODE, errno);
		inotify_fd = -1;
		return;
	}
	watch_fd = ret;
}

void wlan_debug_dump_close_fd() {
	if(inotify_fd == -1)
		return;
	if(inotify_rm_watch(inotify_fd, watch_fd)) {
		android_printf("Failed to close inotify on dump_in_progress node: %d", errno);
	}
}

int cnss_plat_debug_dump_msg_get_fd() {
	if(inotify_fd == -1) {
		wlan_debug_dump_open_fd();
	}
	return inotify_fd;
}

void indicate_log_completion_event()
{
	struct wifihal_ctrl *wifihal_Ctrl;
	char reply_buf[MAX_STR_LEN];
	size_t reply_len = sizeof(reply_buf);
	char flush_dump_to_file_cmd[] = "DRIVER FLUSH_DUMP_TO_FILE";
	int cmd_len = strlen(flush_dump_to_file_cmd);

	if (!dumpinprogress) {
		android_printf("dumpinprogress is false already");
		return;
	}

	wifihal_Ctrl = wifihal_ctrl_open(CONFIG_CTRL_IFACE_CLIENT_WLAN0);
	if (wifihal_Ctrl) {
		if (wifihal_ctrl_request(wifihal_Ctrl, flush_dump_to_file_cmd,
					cmd_len, reply_buf, &reply_len)) {
			android_printf("%s command failed", flush_dump_to_file_cmd);
		}
		android_printf("%s command status: %s", flush_dump_to_file_cmd, reply_buf);
		wifihal_ctrl_close(wifihal_Ctrl);
		reset_dump_in_progress_sysfs_node();
	} else {
		android_printf("Unable to open wifi_hal_ctrl socket : %s",
				CONFIG_CTRL_IFACE_CLIENT_WLAN0);
	}
}

void reset_dump_in_progress_sysfs_node()
{
	FILE *fp;

	fp = fopen(DEBUG_DUMP_IN_PROGRESS_NODE, "w");
	if (fp == NULL) {
		android_printf("Unable to open dump_in_progress sysfs"
				"node: %d", errno);
		return;
	}
	fprintf(fp, "%s\n", "0");
	fclose(fp);
}

void debug_dump_sysfs_handler() {
	struct wifihal_ctrl *wifihal_Ctrl;
	FILE *fp;
	char fline[MAX_STR_LEN];
	char reply_buf[MAX_STR_LEN];
	size_t reply_len = sizeof(reply_buf);
	char debug_dump_cmd[] = "DRIVER DEBUG_DUMP";
	int cmd_len = strlen(debug_dump_cmd);

	fp = fopen(DEBUG_DUMP_IN_PROGRESS_NODE, "r");
	if (fp == NULL) {
		android_printf("Unable to read dump_in_progress sysfs node: %d", errno);
		return;
	}
	fgets(fline, MAX_STR_LEN, fp);
	fclose(fp);

	if (!dumpinprogress && fline[0] == '1') {
		wifihal_Ctrl = wifihal_ctrl_open(CONFIG_CTRL_IFACE_CLIENT_WLAN0);
		if (wifihal_Ctrl) {
			if (wifihal_ctrl_request(wifihal_Ctrl, debug_dump_cmd,
						cmd_len, reply_buf, &reply_len)) {
				android_printf("%s command failed", debug_dump_cmd);
			}
			android_printf("%s command status: %s", debug_dump_cmd, reply_buf);
			wifihal_ctrl_close(wifihal_Ctrl);
		} else {
			android_printf("Unable to open wifi_hal_ctrl socket : %s",
					CONFIG_CTRL_IFACE_CLIENT_WLAN0);
		}
		dumpinprogress = true;
	} else if (dumpinprogress && fline[0] == '0') {
		dumpinprogress = false;
		android_printf("Reset dumpinprogress to false");
	}
}

void chipset_log_max_size_sysfs_handler()
{
	FILE *fp;
	char fline[MAX_STR_LEN] = {0};
	long size;

	fp = fopen(CHIPSET_LOG_MAX_SIZE_NODE, "r");
	if (fp == NULL) {
		android_printf("Unable to read max_chipset_log_size sysfs node: %d", errno);
		return;
	}
	fgets(fline, MAX_STR_LEN, fp);
	fclose(fp);
	/* Value written to this node is interpreted as size in Mb.
	 * Valid range is 50Mb to 1024Mb.
	 */
	size = strtol((const char *)fline, (char **)NULL, 10);
	if (size > 0 && size < CHIPSET_LOG_SIZE_DEFAULT_MIN)
		size = CHIPSET_LOG_SIZE_DEFAULT_MIN;
	else if (size > CHIPSET_LOG_SIZE_DEFAULT_MAX)
		size = CHIPSET_LOG_SIZE_DEFAULT_MAX;
	else if (size < 0)
		size = CHIPSET_LOG_SIZE_DEFAULT_MIN;

	max_host_archives = max_fw_archives = DEFAULT_ARCHIVES_NUM;
	/* max file size should be equal to configured size divide by
	 * total archives + two current files
	 */
	max_file_size = (size * 1024 * 1024) / ((2 * DEFAULT_ARCHIVES_NUM) + 2);
	android_printf("%s:userinput:%s host archive:%d fw archive:%d max file size:%d",
		       __func__, fline, max_host_archives, max_fw_archives,
		       max_file_size);
}

/**
* wlan_chipset_log_max_size_open_fd(): This api opens an inotify fd, and binds it to
* CHIPSET_LOG_MAX_SIZE_NODE. It enables poll and allows the request handler
* to listen IN_MODIFY event on the attached file.
*/
void wlan_chipset_log_max_size_open_fd()
{
	static int iflags = IN_CLOEXEC | IN_NONBLOCK;
	static uint32_t mask = IN_MODIFY;
	int ret, fd;

	fd = access(CHIPSET_LOG_MAX_SIZE_NODE, F_OK);
	if (fd == -1) {
		android_printf("%s doesn't exist", CHIPSET_LOG_MAX_SIZE_NODE);
		return;
	}
	fd = access(CHIPSET_LOG_MAX_SIZE_NODE, R_OK);
	if (fd == -1) {
		android_printf("%s file read permission doesn't exist", CHIPSET_LOG_MAX_SIZE_NODE);
		return;
	}

	inotify_fd_cset_ms = inotify_init1(iflags);
	if (inotify_fd_cset_ms < 0) {
		android_printf("Failed to perform inotify init: %d", errno);
		return;
	}

	ret = inotify_add_watch(inotify_fd_cset_ms, CHIPSET_LOG_MAX_SIZE_NODE, mask);
	if (ret < 0) {
		android_printf("Failed to add inotify watch on %s: %d",
			       CHIPSET_LOG_MAX_SIZE_NODE, errno);
		inotify_fd_cset_ms = -1;
		return;
	}
	/* Fetch the default value if sysfs node is accessible */
	chipset_log_max_size_sysfs_handler();
	watch_fd_cset_ms = ret;
}

void wlan_chipset_log_max_size_close_fd()
{
	if (inotify_fd_cset_ms == -1)
		return;
	if (inotify_rm_watch(inotify_fd_cset_ms, watch_fd_cset_ms)) {
		android_printf("Failed to close inotify on max_chipset_log_size node: %d", errno);
	}
}

int cnss_plat_chipset_log_max_size_get_fd()
{
	if (inotify_fd_cset_ms == -1) {
		wlan_chipset_log_max_size_open_fd();
	}
	return inotify_fd_cset_ms;
}
#endif /* FEATURE_DEBUG_DUMP */

#ifdef CONFIG_CLD80211_LIB
static int handle_diag_cld(void) {
	struct nl_cb *cb;
	int err;
	int num_fds = 2;
	int result;
	struct pollfd pfd[4];
	struct nl_sock *sock;
	int *exit_sockets;
	int ret = 0;

	memset(&pfd[0], 0, 4*sizeof(struct pollfd));
#ifdef FEATURE_DEBUG_DUMP
	int ie;
	ssize_t len;
	const struct inotify_event *event;
	char buf[IEVENT_MAX_LEN] = {0};
	struct timeval now, now_ms;
	int prev_time, prev_time_ms;

	pfd[2].fd = cnss_plat_debug_dump_msg_get_fd();
	pfd[2].events = POLLIN;
	gettimeofday(&now, NULL);
	prev_time = now.tv_sec;

	pfd[3].fd = cnss_plat_chipset_log_max_size_get_fd();
	pfd[3].events = POLLIN;
	gettimeofday(&now_ms, NULL);
	prev_time_ms = now_ms.tv_sec;
#endif
	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb)
		return -ENOMEM;
	err = 1;
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, event_handler, &fetch_free_mem);

	sock = cld80211_get_nl_socket_ctx(cldctx);
	if (!sock) {
		ALOGE("cld sock is NULL");
		ret = -EINVAL;
		goto error;
	}
	pfd[0].fd = nl_socket_get_fd(sock);
	pfd[0].events = POLLIN;

	exit_sockets = cld80211_get_exit_socket_pair(cldctx);
	if (!exit_sockets) {
		ALOGE("cld exit_sockets is NULL");
		ret = -EINVAL;
		goto error;
	}
	pfd[1].fd = exit_sockets[1];
	pfd[1].events = POLLIN;

	do {
		pfd[0].revents = 0;
		pfd[1].revents = 0;
#ifdef FEATURE_DEBUG_DUMP
		pfd[2].revents = 0;
		pfd[3].revents = 0;
		/* poll syscall will ignore invalid fds.
		 * Hence keeping num_fds to 4 (max) to avoid
		 * issues due to hole in fd array
		 */
		num_fds = 4;
#endif
		result = poll(pfd, num_fds, -1);
		if (result < 0) {
			ALOGE("%s: Error polling socket", getprogname());
			continue;
		}

		if (pfd[0].revents & (POLLIN | POLLHUP | POLLERR)) {
			cld80211_handle_event(pfd[0].revents, sock, cb);
		}

		if (pfd[1].revents & (POLLIN | POLLHUP | POLLERR)) {
			android_printf("%s: Exiting poll", getprogname());
			break;
		}

#ifdef FEATURE_DEBUG_DUMP
		if (pfd[2].fd != -1) {
			if (pfd[2].revents & (POLLIN | POLLHUP | POLLERR)) {
				len = read(pfd[2].fd, buf, sizeof(buf));
				if (0 == len) continue;
				/* loop over all events in the buffer */
				for (ie = 0; ie < len;
					ie += sizeof(struct inotify_event) + event->len) {
					event = (struct inotify_event *) &buf[ie];
					if (event->mask & IN_MODIFY) {
						debug_dump_sysfs_handler();
						break;
					}
				}
			}
		}
		/* Try to open the sysfs node for dump_in_progress
		 * after a wait of DUMP_IN_PROGRESS_FD_TIMEOUT */
		else {
			gettimeofday(&now, NULL);
			if (now.tv_sec - prev_time >= DUMP_IN_PROGRESS_FD_TIMEOUT) {
				prev_time = now.tv_sec;
				pfd[2].fd = cnss_plat_debug_dump_msg_get_fd();
			}
		}
		if (pfd[3].fd != -1) {
			if (pfd[3].revents & (POLLIN | POLLHUP | POLLERR)) {
				len = read(pfd[3].fd, buf, sizeof(buf));
				if (0 == len) continue;
				/* loop over all events in the buffer */
				for (ie = 0; ie < len;
					ie += sizeof(struct inotify_event) + event->len) {
					event = (struct inotify_event *) &buf[ie];
					if (event->mask & IN_MODIFY) {
						chipset_log_max_size_sysfs_handler();
						break;
					}
				}
			}
		}
		/* Try to open the sysfs node for max_chipset_log_size
		 * after a wait of MAX_CHIPSET_LOG_SIZE_TIMEOUT */
		else {
			gettimeofday(&now_ms, NULL);
			if (now_ms.tv_sec - prev_time_ms >= MAX_CHIPSET_LOG_SIZE_TIMEOUT) {
				prev_time_ms = now_ms.tv_sec;
				pfd[3].fd = cnss_plat_chipset_log_max_size_get_fd();
			}
		}
#endif
		if (terminate_pending) {
			break;
		}
	} while (1);

error:
	nl_cb_put(cb);
#ifdef FEATURE_DEBUG_DUMP
	wlan_debug_dump_close_fd();
	wlan_chipset_log_max_size_close_fd();
#endif
	if (terminate_pending) {
		ALOGE("%s, Signal to terminate arrived, terminating", __func__);
		stop();
	}
	return ret;
}
#endif /* #ifdef CONFIG_CLD80211_LIB */

/* Remove the functionality of debug dump feature for non cld */
int handle_diag_noncld(void) {
	int num_fds = 1;
	int result;
	struct pollfd pfd[1];
	memset(&pfd[0], 0, 1*sizeof(struct pollfd));

	pfd[0].fd = sock_fd;
	pfd[0].events = POLLIN;

	for(;;) {
		result = poll(pfd, num_fds, -1);
		if (result < 0 )
			continue;

		if (pfd[0].revents & (POLLIN | POLLHUP | POLLERR)) {
			int res = recvmsg(sock_fd, &msg, 0);
			if ((res >= (int)sizeof(struct dbglog_slot)) ||
				(nlh->nlmsg_type == WLAN_NL_MSG_CNSS_HOST_EVENT_LOG)) {
				if (fetch_free_mem && (optionflag & LOGFILE_FLAG ||
					optionflag & PKT_LOGFILE_FLAG)) {
					avail_space = getAvailableSpace(log_capture_loc);
					if (avail_space != -1)
						fetch_free_mem = FALSE;
				}
				process_cnss_diag_msg(nlh->nlmsg_type, (tAniCLDHdr *)(nlh+1));
				memset(nlh,0,NLMSG_SPACE(MAX_MSG_SIZE));
			}
		}
		if (terminate_pending) {
			ALOGE("%s: Signal to terminate arrived, terminating", __func__);
			stop();
			break;
		}
	}
	return 0;
}
#endif /* ifdef ANDROID */

int32_t main(int32_t argc, char *argv[])
{
	int32_t c;
	char chipset_type[10];
#ifndef NO_DIAG_SUPPORT
#ifdef CNSS_DIAG_PLATFORM_WIN
	int errVal = 0;
#endif
#endif
	int proto = 0;
	progname = argv[0];
	int32_t option_index = 0;
	static struct option long_options[] = {
		{"logfile", 0, NULL, 'f'},
		{"console", 0, NULL, 'c'},
		{"syslog", 0, NULL, 'S'},
		{"parser_syslog", 0, NULL, 'p'},
		{"qxdm", 0, NULL, 'q'},
		{"qxdm_sync", 0, NULL, 'x'},
		{"qxdm_sync_log_file", 0, NULL, 'l'},
		{"silent", 0, NULL, 's'},
		{"debug", 0, NULL, 'd'},
		{"log_buff", 0, NULL, 'u'},
		{"decoded_log", 0, NULL, 'w'},
		{"disable fw log",0,NULL,'D'},
		{"buffer_size",required_argument, NULL, 'b'},
		{"target_buffer_size",required_argument, NULL, 'T'},
		{"config_file",required_argument, NULL, 'm'},
		{"chipset_type",required_argument, NULL, 't'},
		{"data_file_path",required_argument, NULL, 'z'},
		{"pktlog", 0, NULL, 'P'},
		{"flush_log", 0, NULL, 'F'},
		{"host_log_socket_protocol",required_argument, NULL, 'n'},
		{"multi_if_name",required_argument, NULL, 'e'},
		{ 0, 0, 0, 0}
	};

	memset(multi_if_name, 0, sizeof(multi_if_name));
	memset(multi_if_file_name, 0, sizeof(multi_if_file_name));
	while (1) {
	c = getopt_long (argc, argv, "fsScpqxluwdDPFb:T:m:t:z:", long_options,
				 &option_index);
		if (c == -1) break;

		switch (c) {
		case 'f':
			optionflag |= LOGFILE_FLAG;
			break;
		case 'b':
			optionflag |= BUFFER_SIZE_FLAG;
			if (optarg != NULL) {
				configured_buffer_size =
					((uint32_t)strtol(optarg, (char **)NULL, 10)) * 1024;
			}
			break;
		case 'T':
			optionflag |= TARGET_BUFFER_SIZE_FLAG;
			if (optarg != NULL) {
				target_configured_buffer_size =
					((uint32_t)strtol(optarg, (char **)NULL, 10)) * 1024;
			}
			break;
		case 'c':
			optionflag |= CONSOLE_FLAG;
			break;
#ifdef CNSS_DIAG_PLATFORM_WIN
		case 'S':
			optionflag |= SYSLOG_FLAG;
			break;
		case 'p':
			optionflag |= DIAG_FWMSG_FLAG;
			break;
#endif
#ifndef NO_DIAG_SUPPORT
		case 'q':
			optionflag |= QXDM_FLAG;
			break;
		case 'x':
			optionflag |= QXDM_SYNC_FLAG;
			break;
		case 'l':
			optionflag |= LOGFILE_QXDM_FLAG;
			break;
#else
		case 'q':
		case 'x':
		case 'l':
			break;
#endif
		case 's':
			optionflag |= SILENT_FLAG;
			break;

		case 'd':
			optionflag |= DEBUG_FLAG;
			break;
		case 'u':
			optionflag |= LOG_BUFF_FLAG;
			break;
		case 'w':
			optionflag |= DECODED_LOG_FLAG;
			break;
		case 'D':
			optionflag |= DISABLE_FWLOG_FLAG;
			break;
		case 'z':
			if (optarg != NULL)
				datamsc_file_path = optarg;
			break;
		case 'm':
			if (optarg != NULL)
				cnssdiag_config_file = optarg;
			break;
		case 't':
			if (optarg != NULL) {
				strlcpy(chipset_type,optarg, sizeof(chipset_type));
				cnss_diag_find_wlan_dev(chipset_type);
			}
			break;
		case 'n':
			if (optarg != NULL) {
				proto = strtol(optarg, (char **)NULL, 10);
				if ((proto >= 0) && (proto < 32)) {
					host_log_socket_protocol = proto;
					printf("host log socket protocol: %d\n",
					       host_log_socket_protocol);
				}
				else {
					printf("host log socket protocol range: [0, 31]\n");
					return -1;
				}
			}
			break;
		case 'e':
			if (optarg != NULL) {
				snprintf(multi_if_name, sizeof(multi_if_name), "%s", optarg);
				snprintf(multi_if_file_name, sizeof(multi_if_file_name), "%s_", optarg);
			}
			break;
		case 'P':
			optionflag |= PKT_LOGFILE_FLAG;
			break;
		case 'F':
			optionflag |= FLUSH_LOGFILE_FLAG;
			break;
		default:
			usage();
		}
	}

	if (!(optionflag & (LOGFILE_FLAG | CONSOLE_FLAG | QXDM_SYNC_FLAG |
						QXDM_FLAG | SILENT_FLAG | DEBUG_FLAG |
						LOGFILE_QXDM_FLAG | BUFFER_SIZE_FLAG |
						LOG_BUFF_FLAG | DECODED_LOG_FLAG |
						FLUSH_LOGFILE_FLAG | PKT_LOGFILE_FLAG
#ifdef CNSS_DIAG_PLATFORM_WIN
						| SYSLOG_FLAG
#endif
						))) {
		usage();
		return -1;
	}


#ifndef NO_DIAG_SUPPORT
		if (optionflag & QXDM_FLAG || optionflag & QXDM_SYNC_FLAG) {
#ifdef CNSS_DIAG_PLATFORM_WIN
			if (!(optionflag & DIAG_FWMSG_FLAG)) {
#endif
				/* Intialize the fd required for diag APIs */
				if (TRUE != Diag_LSM_Init(NULL)) {
					perror("Failed on Diag_LSM_Init\n");
					return -1;
				}
				android_printf("Diag LSM init done!");
				/* Register CALLABACK for QXDM input data */
				DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_WLAN, cnss_wlan_tbl);
#ifdef CNSS_DIAG_PLATFORM_WIN
			}
#endif
		}
#endif /* NO_DIAG_SUPPORT */

	initialize_cnsslog_timer();
#ifdef CONFIG_CLD80211_LIB
	cldctx = cld80211_init();
	int res;
	if (cldctx != NULL) {
		android_printf("cld80211 ctx init done, proceed to add mcast groups");
		res = cld80211_add_mcast_group(cldctx, "fw_logs");
		if (res) {
			android_printf("Failed to add mcast group: fw_logs");
			cld80211_deinit(cldctx);
			return -1;
		}
		res = cld80211_add_mcast_group(cldctx, "host_logs");
		if (res) {
			android_printf("Failed to add mcast group: host_logs");
			cld80211_remove_mcast_group(cldctx, "fw_logs");
			cld80211_deinit(cldctx);
			return -1;
		}
		res = cld80211_add_mcast_group(cldctx, "diag_events");
		if (res) {
			android_printf("Failed to add mcast group:diag_events");
			cld80211_remove_mcast_group(cldctx, "host_logs");
			cld80211_remove_mcast_group(cldctx, "fw_logs");
			cld80211_deinit(cldctx);
			return -1;
		}
	} else {
		android_printf("Failed to initialize cld80211 library,"
		               " proceed with legacy procedure\n");
#endif /* CONFIG_CLD80211_LIB */
		sock_fd = create_nl_socket();
		if (sock_fd < 0) {
			fprintf(stderr, "Socket creation failed sock_fd 0x%x \n",
				sock_fd);
			return -1;
		}
		if (initialize() < 0)
			return -1;
#ifdef CONFIG_CLD80211_LIB
	}
#endif

#ifndef NO_DIAG_SUPPORT
#ifdef CNSS_DIAG_PLATFORM_WIN

	if (optionflag & DIAG_FWMSG_FLAG) {
		if (Diag_LSM_Init(NULL) != TRUE) {
			fprintf(stderr, "Failed on Diag_LSM_Init\n");
			return -1;
		}
		/* Register the callback function for receiving data from MSM */
		diag_register_callback(diag_callback, &data_primary);
		/*
		 * Switch to Callback mode to receive
		 * Diag data in this application
		 */
		diag_switch_logging(CALLBACK_MODE, NULL);

	}
#endif /* NO_DIAG_SUPPORT */
#endif /* CNSS_DIAG_PLATFORM_WIN */

	read_config_file();

#ifndef NO_DIAG_SUPPORT
#ifdef CNSS_DIAG_PLATFORM_WIN
	if ((optionflag & DIAG_FWMSG_FLAG)  && firmware_log_levels_default) {
		errVal = diag_callback_send_data(data_primary,
						 diag_config,
						 sizeof(diag_config));
		if (errVal) {
			fprintf(stderr, "Unable to send Diag log"
					" configuration settings "
					"to MSM errVal = %d\n", errVal);
			Diag_LSM_DeInit();
			return -1;
		}
	}
	sleep(5);
#endif /* NO_DIAG_SUPPORT */
#endif /* CNSS_DIAG_PLATFORM_WIN */

	max_no_buffer = max_buff_size/EACH_BUF_SIZE;
#ifndef WIN_AP_HOST_OPEN
	if (optionflag & LOGFILE_FLAG || optionflag & LOGFILE_QXDM_FLAG ||
	    optionflag & LOG_BUFF_FLAG || (max_pktlog_archives > 0) ||
	    optionflag & PKT_LOGFILE_FLAG) {
		avail_space = getAvailableSpace(log_capture_loc);
		if (avail_space != -1)
			fetch_free_mem = FALSE;
		if (init_log_file())
			goto end;
	}
#endif
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	if (optionflag & LOG_BUFF_FLAG)
		signal(SIGUSR1, update_buff_to_file);
	else
		signal(SIGUSR1, default_handler_sigusr1);

	parser_init();
	cnss_diag_running = 1;

#ifdef WIN_AP_HOST_OPEN
	/* initialize nl testmode mcast group */
	nl80211_fwlog_init();
	if (!nl_handle) {
		goto end;
	}

	nl80211_fwlog_rx();
#endif
#ifdef ANDROID
	int status = 0;
#ifdef CONFIG_CLD80211_LIB
	if (cldctx) {
		status = handle_diag_cld();
	} else
#endif /* CONFIG_CLD80211_LIB */
	{
		status = handle_diag_noncld();
	}
	android_printf("diag status: %d", status);
#else
#ifdef CONFIG_CLD80211_LIB
	if (cldctx) {
		cld80211_recv(cldctx, -1, true, &event_handler, &fetch_free_mem);
		android_printf("event handler registered to cld80211");
		if (terminate_pending) {
			ALOGE("%s: Signal to terminate arrived, terminating",
			      __func__);
			stop();
		}
	}
	else
#endif
	{
		int res;
		while ( 1 ) {
			if ((res = recvmsg(sock_fd, &msg, 0)) < 0)
			continue;
			if ((res >= (int)sizeof(struct dbglog_slot)) ||
			(nlh->nlmsg_type == WLAN_NL_MSG_CNSS_HOST_EVENT_LOG)) {
				if (fetch_free_mem && (optionflag & LOGFILE_FLAG ||
					optionflag & PKT_LOGFILE_FLAG)) {
					avail_space = getAvailableSpace(log_capture_loc);
					if (avail_space != -1)
						fetch_free_mem = FALSE;
				}
				process_cnss_diag_msg(nlh->nlmsg_type, (tAniCLDHdr *)(nlh+1));
				memset(nlh,0,NLMSG_SPACE(MAX_MSG_SIZE));
			}
			if (terminate_pending) {
				ALOGE("%s: Signal to terminate arrived, terminating", __func__);
				stop();
				break;
			}
		}
	}
#endif
end:
#ifndef NO_DIAG_SUPPORT
	/* Release the handle to Diag*/
	if (optionflag & QXDM_FLAG ||
#ifdef CNSS_DIAG_PLATFORM_WIN
		optionflag & DIAG_FWMSG_FLAG ||
#endif
		optionflag & QXDM_SYNC_FLAG) {
		Diag_LSM_DeInit();
	}
#endif

	if (optionflag & LOG_BUFF_FLAG) {
		printf("free all buffers\n ");
		clean_all_buff(&log_file[BUFFER_HOST_FILE]);
		log_file[BUFFER_HOST_FILE].buf = NULL;
		clean_all_buff(&log_file[BUFFER_FW_FILE]);
		log_file[BUFFER_FW_FILE].buf = NULL;
	}
	if (optionflag & LOGFILE_FLAG || optionflag & LOGFILE_QXDM_FLAG
				      || optionflag & PKT_LOGFILE_FLAG)
		cleanup();
	if (pthread_mutex_destroy(&log_file[BUFFER_HOST_FILE].buff_lock))
		android_printf("Failed to destroy host buff_lock");

	if (pthread_mutex_destroy(&log_file[BUFFER_FW_FILE].buff_lock))
		android_printf("Failed to destroy firmware buff_lock");

#ifdef CONFIG_CLD80211_LIB
	if (cldctx) {
		cld80211_remove_mcast_group(cldctx, "host_logs");
		cld80211_remove_mcast_group(cldctx, "fw_logs");
		cld80211_remove_mcast_group(cldctx, "diag_events");
		cld80211_deinit(cldctx);
	} else
#endif
	{
		close(sock_fd);
		free(nlh);
	}
	return 0;
}


