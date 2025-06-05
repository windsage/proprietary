/*===========================================================================

  Copyright (c) 2010-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <string.h>
#include <dirent.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <signal.h>
#include "thermal.h"
#ifdef ANDROID
#  include "cutils/properties.h"
#  include <bits/epoll_event.h>
#endif

#define THERMAL_SYSFS "/sys/devices/virtual/thermal"

/* Sys proc ID */
#define SYSFS_PLATFORMID_DEPRECATED   "/sys/devices/system/soc/soc0/id"
#define SYSFS_HW_PLATFORM_DEPRECATED  "/sys/devices/system/soc/soc0/hw_platform"
#define SYSFS_MSM_VERSION_DEPRECATED  "/sys/devices/system/soc/soc0/version"
#define SYSFS_HW_SUBTYPE_DEPRECATED   "/sys/devices/system/soc/soc0/platform_subtype_id"
#define SYSFS_PMIC_MODEL_DEPRECATED   "/sys/devices/system/soc/soc0/pmic_model"
#define SYSFS_PLATFORMID   "/sys/devices/soc0/soc_id"
#define SYSFS_HW_PLATFORM  "/sys/devices/soc0/hw_platform"
#define SYSFS_MSM_VERSION  "/sys/devices/soc0/revision"
#define SYSFS_HW_SUBTYPE   "/sys/devices/soc0/platform_subtype_id"
#define SYSFS_PMIC_MODEL  "/sys/devices/soc0/pmic_model"

#define MAX_INT_ARRAY_STR_BUF  768
#define MAX_SOC_INFO_NAME_LEN (15)
#define MAX_CONFIG_PATH  (50)
#ifdef ANDROID
#define DEFAULT_CONFIG_PATH  "/system/etc/"
#else
#define DEFAULT_CONFIG_PATH  "/etc/"
#endif

#define MAX_EPOLL_EVENTS (32 * 2) //tiwce of MAX_INSTANCES_SUPPORTED
#define LIMIT_PROFILE1_TEMP 118000
#define LIMIT_PROFILE_TZ    "aoss-0"

static pthread_t callback_thread;
static int epollfd = -1;

struct event_data {
	int fd;
	void *callback;
	int id;
};

struct therm_msm_soc_type {
	enum therm_msm_id msm_id;
	int  soc_id;
};

struct therm_hw_platform_type {
	enum therm_hw_platform platform;
	char *platform_name;
};

struct therm_msm_version_type {
	enum therm_msm_version version;
	int version_id;
};

struct target_thermal_config {
	enum therm_msm_id msm_id;
	char *config_file;
};

struct therm_pmic_type {
	enum therm_pmic_model pmic;
	int pmic_id;
};

static struct therm_msm_soc_type msm_soc_table[] = {
	{THERM_MDM_9607,   290},
	{THERM_MDM_9607,   296},
	{THERM_MDM_9607,   297},
	{THERM_MDM_9607,   298},
	{THERM_MDM_9607,   299},
	{THERM_WAIPIO, 457},
	{THERM_WAIPIO, 482},
	{THERM_WAIPIO, 552}, // This SoC ID is for WAIPIO-LTE
	{THERM_CAPE, 530},
	{THERM_CAPE, 531},
	{THERM_CAPE, 540},
	{THERM_DIWALI, 506},
	{THERM_KALAMA, 519}, //Kalama
	{THERM_KALAMA, 536}, //Kalamap
	{THERM_KALAMA, 600}, //Kalama_sg
	{THERM_KALAMA, 601}, //Kalamap_sg
	{THERM_KHAJE, 518}, //Divar
	{THERM_PINEAPPLE, 557}, //Pineapple
	{THERM_PINEAPPLE, 577}, //Pineapplep
	{THERM_PINEAPPLE, 682}, //Pineapple Gaming
	{THERM_PINEAPPLE, 696}, //Pineapple Compute
	{THERM_MONACO_AUTO, 605},
	{THERM_MONACO_AUTO, 606},
	{THERM_MONACO_AUTO, 607},
	{THERM_HOLI, 454}, //holi
	{THERM_HOLI, 507}, //blair
	{THERM_HOLI, 578}, //Conic
	{THERM_CLIFFS, 632}, //cliff7
	{THERM_CLIFFS, 614}, //cliff
	{THERM_MONACO, 486}, //Atherton
	{THERM_MONACO, 517}, //AthertonAPQ
	{THERM_PITTI, 623}, //Kalpeni
	{THERM_NIOBE, 629}, //Matrix
	{THERM_NIOBE, 652}, //Matrix_4k
	{THERM_VOLCANO, 636}, //Milos
	{THERM_VOLCANO, 640}, //Milos6
	{THERM_VOLCANO, 658}, //Milos IOT
	{THERM_VOLCANO, 657}, //Milos IOT with modem
	{THERM_ANORAK, 549},
	{THERM_ANORAK, 649}, // Halliday Pro
};

static struct therm_hw_platform_type platform_table[] = {
	{THERM_PLATFORM_UNKNOWN,    "Unknown"},
	{THERM_PLATFORM_SURF,       "Surf"},
	{THERM_PLATFORM_FFA,        "FFA"},
	{THERM_PLATFORM_FLUID,      "Fluid"},
	{THERM_PLATFORM_SVLTE_FFA,  "SVLTE_FFA"},
	{THERM_PLATFORM_SVLTE_SURF, "SLVTE_SURF"},
	{THERM_PLATFORM_MTP,        "MTP"},
	{THERM_PLATFORM_LIQUID,     "Liquid"},
	{THERM_PLATFORM_DRAGON,     "Dragon"},
	{THERM_PLATFORM_QRD,        "QRD"},
	{THERM_PLATFORM_IDP,        "IDP"},
	{THERM_PLATFORM_ATP,        "ATP"},
	{THERM_PLATFORM_QXR,        "QXR"},
};

static struct therm_msm_version_type version_table[] = {
	{THERM_VERSION_V1,  1},
	{THERM_VERSION_V2,  2},
};

static struct target_thermal_config thermal_config_table[] = {
};

static struct therm_pmic_type pmic_type_table[] = {
};

static int get_soc_info(char *buf, char *soc_node, char *soc_node_dep)
{
	int ret = 0;

	ret = read_line_from_file(soc_node, buf, MAX_SOC_INFO_NAME_LEN);
	if (ret < 0) {
		ret = read_line_from_file(soc_node_dep, buf,
					  MAX_SOC_INFO_NAME_LEN);
		if (ret < 0) {
			msg("Error getting platform_id %d", ret);
			return ret;
		}
	}
	if (ret && buf[ret - 1] == '\n')
		buf[ret - 1] = '\0';

	return ret;
}

enum therm_msm_id therm_get_msm_id(void)
{
	static enum therm_msm_id msm_id;
	static uint8_t msm_id_init;

	if (!msm_id_init) {
		int idx;
		char buf[MAX_SOC_INFO_NAME_LEN] = {[0 ... MAX_SOC_INFO_NAME_LEN - 1] = 0};

		if (soc_id < 0) {
			get_soc_info(buf, SYSFS_PLATFORMID, SYSFS_PLATFORMID_DEPRECATED);
			soc_id = atoi(buf);
		}

		for (idx = 0; idx < ARRAY_SIZE(msm_soc_table); idx++) {
			if (soc_id == msm_soc_table[idx].soc_id) {
				msm_id = msm_soc_table[idx].msm_id;
				break;
			}
		}
		if (!msm_id)
			msg("Unknown target identified with soc id %d\n", soc_id);

		msm_id_init = 1;
	}
	return msm_id;
}

enum therm_hw_platform therm_get_hw_platform(void)
{
	static enum therm_hw_platform platform;
	static uint8_t hw_platform_init;

	if (!hw_platform_init) {
		int idx;
		char buf[MAX_SOC_INFO_NAME_LEN] = {[0 ... MAX_SOC_INFO_NAME_LEN - 1] = 0};

		get_soc_info(buf, SYSFS_HW_PLATFORM,
				   SYSFS_HW_PLATFORM_DEPRECATED);

		for (idx = 0; idx < ARRAY_SIZE(platform_table); idx++) {
			if (strncmp(platform_table[idx].platform_name, buf,
			    MAX_SOC_INFO_NAME_LEN) == 0) {
				platform = platform_table[idx].platform;
				break;
			}
		}
		hw_platform_init = 1;
	}
	return platform;
}

enum therm_msm_version therm_get_msm_version(void)
{
	static enum therm_msm_version version;
	static uint8_t msm_version_init;

	if (!msm_version_init) {
		int idx;
		int version_id;
		char buf[MAX_SOC_INFO_NAME_LEN] = {[0 ... MAX_SOC_INFO_NAME_LEN - 1] = 0};

		get_soc_info(buf, SYSFS_MSM_VERSION,
				   SYSFS_MSM_VERSION_DEPRECATED);

		version_id = atoi(buf);

		for (idx = 0; idx < ARRAY_SIZE(version_table); idx++) {
			if (version_id == version_table[idx].version_id) {
				version = version_table[idx].version;
				break;
			}
		}
		msm_version_init = 1;
	}
	return version;
}

enum therm_hw_platform_subtype therm_get_hw_platform_subtype(void)
{
	static enum therm_hw_platform_subtype subtype =
					THERM_PLATFORM_SUB_UNKNOWN;
	static uint8_t subtype_id_init;

	if (!subtype_id_init) {
		int subtype_id = 0;
		char buf[MAX_SOC_INFO_NAME_LEN] = {[0 ... MAX_SOC_INFO_NAME_LEN - 1] = 0};

		get_soc_info(buf, SYSFS_HW_SUBTYPE, SYSFS_HW_SUBTYPE_DEPRECATED);
		subtype_id = atoi(buf);
		if (subtype_id > THERM_PLATFORM_SUB_UNKNOWN)
			subtype = subtype_id;

		subtype_id_init = 1;
	}
	return subtype;
}

char *get_target_default_thermal_config_file(void)
{
	static char config_file_path[MAX_CONFIG_PATH] = {0};
	static uint8_t config_init;

	if (!config_init) {
		int idx;
		char *config = NULL;
		enum therm_msm_id msm_id = therm_get_msm_id();

		for (idx = 0; idx < ARRAY_SIZE(thermal_config_table); idx++) {
			if (msm_id == thermal_config_table[idx].msm_id) {
				config = thermal_config_table[idx].config_file;
				break;
			}
		}
		if (config) {
			snprintf(config_file_path, MAX_CONFIG_PATH, "%s%s",
			         DEFAULT_CONFIG_PATH, config);
		}
		config_init = 1;
	}
	if (config_file_path[0] == 0)
		return NULL;
	else
		return config_file_path;
}

enum therm_pmic_model therm_get_pmic_model(void)
{
	static enum therm_pmic_model pmic = THERM_PMIC_UNKNOWN;
	static uint8_t pmic_id_init;

	if (!pmic_id_init) {
		int idx;
		int pmic_id = 0;
		char buf[MAX_SOC_INFO_NAME_LEN] = {0};

		get_soc_info(buf, SYSFS_PMIC_MODEL,
				SYSFS_PMIC_MODEL_DEPRECATED);
		pmic_id = atoi(buf);

		for (idx = 0; idx < ARRAY_SIZE(pmic_type_table); idx++) {
			if (pmic_id == pmic_type_table[idx].pmic_id) {
				pmic = pmic_type_table[idx].pmic;
				break;
			}
		}
		if (!pmic)
			msg("Unknown pmic identified with pmic id %d\n",
			    pmic_id);

		pmic_id_init = 1;
	}

	return pmic;
}

enum therm_limit_profile therm_get_limit_profile(void)
{
	static enum therm_limit_profile profile =
					THERM_LIMIT_PROFILE_UNKNOWN;
	static uint8_t limit_profile_init;
	int tzn = -1, ret = 0, cpu_trip_temp = -1;

	if (!limit_profile_init) {
		char tz_file_path[MAX_PATH] = {0}, buf[UINT_BUF_MAX] = {0};

		tzn = get_tzn(LIMIT_PROFILE_TZ);
		if (tzn < 0) {
			msg("Not able to get thermal zone %s,defaulting to 0\n",
				LIMIT_PROFILE_TZ);
			return THERM_LIMIT_PROFILE_0;
		}

		snprintf(tz_file_path, MAX_PATH, TZ_SET_TEMP, tzn, 1);
		ret = read_line_from_file(tz_file_path, buf, UINT_BUF_MAX);
		if (ret < 0) {
			msg("Error getting trip temp %d, defaulting to 0\n",
				      ret);
			return THERM_LIMIT_PROFILE_0;
		}
		if (ret && buf[ret - 1] == '\n')
			buf[ret - 1] = '\0';
		cpu_trip_temp = atoi(buf);
		profile = cpu_trip_temp == LIMIT_PROFILE1_TEMP ?
			THERM_LIMIT_PROFILE_1: THERM_LIMIT_PROFILE_0;
		limit_profile_init = 1;
		dbgmsg("cpu_trip_temp %d limit profile:%d\n", cpu_trip_temp, profile);
	}

	return profile;
}

unsigned int read_int_list_from_file(const char *path, int *arr, uint32_t arr_sz)
{
	char buf[MAX_INT_ARRAY_STR_BUF];
	int bytes_read = 0;
	uint32_t ints_read = 0;
	char *int_str = NULL;
	char *saveptr;

	if (path == NULL || arr == NULL || arr_sz == 0) {
		msg("%s: Bad arg.", __func__);
		return 0;
	}

	/* Read integer list from file. */
	bytes_read = read_line_from_file(path, buf, sizeof(buf));

	if (bytes_read <= 0) {
		msg("%s: Read error %d.", __func__, bytes_read);
		return 0;
	}

	/* Parse out individual values into array */
	int_str = strtok_r(buf, " \n\r\t", &saveptr);
	while ((int_str != NULL) && ((uint32_t)ints_read < arr_sz)) {
		arr[ints_read] = atoi(int_str);
		ints_read++;
		int_str = strtok_r(NULL, " \n\r\t", &saveptr);
	}

	return ints_read;
}

static int compare_ascending_int(const void *x, const void *y)
{
	int i = *((int*)x);
	int j = *((int*)y);

	if (i < j)
		return -1;
	else if (i > j)
		return 1;
	return 0;
}

static int compare_descending_int(const void *x, const void *y)
{
	int i = *((int*)x);
	int j = *((int*)y);

	if (j < i)
		return -1;
	else if (j > i)
		return 1;
	return 0;
}

void sort_int_arr(int *arr, uint32_t arr_sz, uint8_t ascending)
{
	if (arr == NULL || arr_sz == 0) {
		msg("%s: Bad arg.", __func__);
		return;
	}

	if (ascending)
		qsort(arr, arr_sz, sizeof(int), compare_ascending_int);
	else
		qsort(arr, arr_sz, sizeof(int), compare_descending_int);
}


/*===========================================================================
FUNCTION open_file

Utility function to open file.

ARGUMENTS
	path - pathname for file
	flags - file open flags

RETURN VALUE
	file descriptor on success,
	-1 on failure.
===========================================================================*/
static int open_file(const char *path, int flags)
{
	int rv;

	if (!path) return -EINVAL;

	rv = open(path, flags);
	if (rv < 0)
		rv = -errno;

	return rv;
}

/*===========================================================================
FUNCTION write_to_fd

Utility function to write to provided file descriptor.

ARGUMENTS
	fd - file descriptor
	buf - destination buffer to write to
	count - number of bytes to write to fd

RETURN VALUE
	number of bytes written on success, -errno on failure.
===========================================================================*/
static int write_to_fd(int fd, const char *buf, size_t count)
{
	ssize_t pos = 0;
	ssize_t rv = 0;
	/* When thermal-engine is in dump_conf mode we do not want to
	interfere with any other instance of thermal-engine that is running */
	if (output_conf)
		return 0;

	do {
		dbgmsg("writing:%s in fd:%d bytes:%zu\n", buf, fd, count);
		rv = write(fd, buf + (size_t)pos, count - (size_t)pos);
		if (rv < 0)
			return -errno;
		pos += rv;
	} while ((ssize_t)count > pos);

	return (int)count;
}

/*===========================================================================
FUNCTION read_line_from_file

Utility function to read characters from file and stores them in a string
until (count-1) characters are read or either a newline or EOF is reached.

ARGUMENTS
	path - file path
	buf - destination buffer to read from
	count - max number of bytes to read from file

RETURN VALUE
	number of bytes read on success, -errno on failure.
===========================================================================*/
int read_line_from_file(const char *path, char *buf, size_t count)
{
	char * fgets_ret;
	FILE * fd;
	int rv;

	fd = fopen(path, "r");
	if (fd == NULL)
	{
		return -errno;
	}

	fgets_ret = fgets(buf, (int)count, fd);
	if (NULL != fgets_ret) {
		rv = (int)strlen(buf);
	} else {
		rv = ferror(fd);
	}

	fclose(fd);

	return rv;
}

/*===========================================================================
FUNCTION write_to_file

Utility function to write to provided file path.

ARGUMENTS
	path - file path
	buf - destination buffer to write to
	count - number of bytes to write to file

RETURN VALUE
	number of bytes written on success, -errno on failure.
===========================================================================*/
int write_to_file(const char *path, const char *buf, size_t count)
{
	int fd, rv;
	/* When thermal-engine is in dump_conf mode we do not want to
	interfere with any other instance of thermal-engine that is running */
	if (output_conf)
		return 0;

	fd = open_file(path, O_RDWR);
	if (fd < 0) {
		msg("open%s failed%d", path, errno);
		return fd;
	}

	rv = write_to_fd(fd, buf, count);
	close(fd);

	return rv;
}

/*===========================================================================
FUNCTION connect_local_file_socket

Utility function to open and connect to local UNIX filesystem socket.

ARGUMENTS
	socket_name - name of local UNIX filesystem socket to be connected

RETURN VALUE
	Connected socket_fd on success,
	-1 on failure.
===========================================================================*/
static int connect_local_file_socket(const char *socket_name)
{
	int socket_fd = 0;
	struct sockaddr_un serv_addr;

	socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		dbgmsg("socket error - %s\n", strerror(errno));
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	snprintf(serv_addr.sun_path, UNIX_PATH_MAX, "%s", socket_name);
	serv_addr.sun_family = AF_LOCAL;

	if (connect(socket_fd, (struct sockaddr *) &serv_addr,
		    (socklen_t)(sizeof(sa_family_t) + strlen(socket_name)) ) != 0) {
		dbgmsg("connect error on %s - %s\n",
		       socket_name, strerror(errno));
		close(socket_fd);
		return -1;
	}

	return socket_fd;
}

/*===========================================================================
FUNCTION write_to_local_file_socket

Utility function to write to local filesystem UNIX socket.

ARGUMENTS
	socket_name - socket name to be written
	msg - message to be written on socket
	count - size of msg buffer to be written

RETURN VALUE
	Number of bytes written on success,
	-1 on failure.
===========================================================================*/
int write_to_local_file_socket(const char *socket_name, const char *msg, size_t count)
{
	int socket_fd = 0;
	int rv;

	if (minimum_mode) {
		return -1;
	}

	socket_fd = connect_local_file_socket(socket_name);
        if (socket_fd < 0) {
		return -1;
	}

	rv = write_to_fd(socket_fd, msg, count);
	close(socket_fd);

	return rv;
}

#define MAX_SOCKET_FD 3
static int socket_fd[MAX_SOCKET_FD] = { [0 ... MAX_SOCKET_FD-1] = -1};
static int socket_rule_fd[MAX_SOCKET_FD] = { [0 ... MAX_SOCKET_FD-1] = -1};
static pthread_mutex_t local_socket_mtx = PTHREAD_MUTEX_INITIALIZER;

void add_local_socket_fd(enum socket_type type, int sfd)
{
	int i;
	char buf;
	int *soc_fd = NULL;

	pthread_mutex_lock(&local_socket_mtx);

	if (type == SOCKET_RPT_LOG)
		soc_fd = socket_fd;
	else
		soc_fd = socket_rule_fd;

	for (i = 0; i < MAX_SOCKET_FD; i++) {
		/* Add socket_id if the socket index is empty (-1) or if the socket_fd is invalid */
		if ((soc_fd[i] == -1) ||
		    (recv(soc_fd[i], &buf, sizeof(buf), MSG_PEEK | MSG_DONTWAIT) == 0)) {
			if (soc_fd[i] != -1)
				close(soc_fd[i]);
			soc_fd[i] = sfd;
			dbgmsg("Socket fd %d added to index %d\n", sfd, i);
			break;
		}
	}

	if (i == MAX_SOCKET_FD) {
		msg("Exceeded max local sockets request\n");
		close(sfd);
	}
	pthread_mutex_unlock(&local_socket_mtx);
}

/*===========================================================================
FUNCTION write_to_local_socket

Utility function to write to abstract local UNIX socket.

ARGUMENTS
        msg - message to be written on socket
        count - size of msg buffer to be written

RETURN VALUE
        Number of bytes written on success,
        -1 on failure.
===========================================================================*/
int write_to_local_socket(enum socket_type type, const char *msg, size_t count)
{
	int i, rv = 0;
	int *soc_fd = NULL;

	pthread_mutex_lock(&local_socket_mtx);
	if (type == SOCKET_RPT_LOG)
		soc_fd = socket_fd;
	else
		soc_fd = socket_rule_fd;

	for (i = 0; i < MAX_SOCKET_FD; i++) {
		if (soc_fd[i] >= 0) {
			if ((rv = write_to_fd(soc_fd[i], msg, count)) < 0) {
				close(soc_fd[i]);
				soc_fd[i] = -1;
			}
		}
	}
	pthread_mutex_unlock(&local_socket_mtx);
	return rv;
}
/*===========================================================================
FUNCTION remove_local_socket_fd

Utility function to remove fd from abstract local UNIX socket or
thermal rule socket.

ARGUMENTS
        sfd - socket fd to be removed

RETURN VALUE
	None
===========================================================================*/
void remove_local_socket_fd(int sfd)
{
	int i;

	pthread_mutex_lock(&local_socket_mtx);
	for (i = 0; i < MAX_SOCKET_FD; i++) {
		if (socket_fd[i] == sfd) {
			socket_fd[i] = -1;
			break;
		} else if (socket_rule_fd[i] == sfd) {
			socket_rule_fd[i] = -1;
			break;
		}
	}
	pthread_mutex_unlock(&local_socket_mtx);
}

static int get_tzn_and_cdev(const char *sensor_name, int is_cdev)
{
	DIR *tdir = NULL;
	struct dirent *tdirent = NULL;
	int found = -1;
	int tzn = 0;
	char name[MAX_PATH] = {0};
	char cwd[MAX_PATH] = {0};
	int ret = 0;

	if (!getcwd(cwd, sizeof(cwd)))
		return found;

	ret = chdir(THERMAL_SYSFS); /* Change dir to read the entries. Doesnt
				       work otherwise */
	if (ret) {
		msg("Unable to change to %s\n", THERMAL_SYSFS);
		return found;
	}
	tdir = opendir(THERMAL_SYSFS);
	if (!tdir) {
		msg("Unable to open %s\n", THERMAL_SYSFS);
		return found;
	}

	while ((tdirent = readdir(tdir))) {
		char buf[50] = {[0 ... 49] = 0};
		struct dirent *tzdirent;
		DIR *tzdir = NULL;

		if (is_cdev) {
			if (strncmp(tdirent->d_name, CDEV_DIR_NAME,
					strlen(CDEV_DIR_NAME)) != 0)
				continue;
		} else {
			if (strncmp(tdirent->d_name, CDEV_DIR_NAME,
					strlen(CDEV_DIR_NAME)) == 0)
				continue;
		}
		tzdir = opendir(tdirent->d_name);
		if (!tzdir)
			continue;
		while ((tzdirent = readdir(tzdir))) {
			if (strcmp(tzdirent->d_name, "type"))
				continue;
			if (snprintf(name, MAX_PATH, THERMAL_TYPE,
					tdirent->d_name) < 0)
				msg("%s: truncation error\n", __func__);
			dbgmsg("Opening %s\n", name);
			ret = read_line_from_file(name, buf, sizeof(buf));
			if (ret <= 0) {
				msg("%s: sensor name read error for tz:%s\n",
					__func__, tdirent->d_name);
				break;
			}
			if (buf[ret - 1] == '\n')
				buf[ret - 1] = '\0';
			else
				buf[ret] = '\0';

			if (!strcmp(buf, sensor_name)) {
				found = 1;
				break;
			}
		}
		closedir(tzdir);
		if (found == 1)
			break;
	}

	if (found == 1) {
		if (is_cdev) {
			sscanf(tdirent->d_name, CDEV_DIR_FMT, &tzn);
			dbgmsg("cdev %s found at tz: %d\n",
					sensor_name, tzn);
		} else {
			sscanf(tdirent->d_name, TZ_DIR_FMT, &tzn);
			dbgmsg("Sensor %s found at tz: %d\n",
					sensor_name, tzn);
		}
		found = tzn;
	}

	closedir(tdir);
	ret = chdir(cwd); /* Restore current working dir */

	return found;
}

/*===========================================================================
FUNCTION get_cdevn

Utility function to match a cdev name with cooling device id.

ARGUMENTS
	cdev_name - name of cooling device to match

RETURN VALUE
	Cooling device id on success,
	-1 on failure.
===========================================================================*/
int get_cdevn(const char *cdev_name)
{
	return get_tzn_and_cdev(cdev_name, 1);
}

/*===========================================================================
FUNCTION get_tzn

Utility function to match a sensor name with thermal zone id.

ARGUMENTS
	sensor_name - name of sensor to match

RETURN VALUE
	Thermal zone id on success,
	-1 on failure.
===========================================================================*/
int get_tzn(const char *sensor_name)
{
	return get_tzn_and_cdev(sensor_name, 0);
}

/*===========================================================================
FUNCTION get_num_cpus

Get number of CPU's

ARGUMENTS
	None

RETURN VALUE
	Number of CPUs
===========================================================================*/
int get_num_cpus(void)
{
	static int ncpus;
	static uint8_t ncpus_init;

	if (!ncpus_init) {
		ncpus = (int)sysconf(_SC_NPROCESSORS_CONF);
		if (ncpus < 1)
			msg("%s: Error retrieving number of cores", __func__);
		else
			ncpus_init = 1;
	}
	dbgmsg("Number of CPU cores %d\n", ncpus);
	return ncpus;
}

#ifdef _PLATFORM_BASE
#define SYS_DEV_DIR  "/sys"_PLATFORM_BASE
#else
#define SYS_DEV_DIR "/sys/devices/"
#endif
/*===========================================================================
FUNCTION get_sysdev_dt_dir_name

Find path for driver name in "/sys/devices" directory.

ARGUMENTS
	dt_dir - char buffer of size MAX_PATH
	driver_name - name to search for in "/sys/devices"

RETURN VALUE
	Number of CPUs
===========================================================================*/
int get_sysdev_dt_dir_name(char *dt_dir, const char *driver_name)
{
	DIR *tdir = NULL;
	struct dirent *tdirent = NULL;
	char cwd[MAX_PATH] = {0};
	int ret = -ENODEV, err;

	if (!getcwd(cwd, sizeof(cwd)))
		return ret;

	err = chdir(SYS_DEV_DIR);
	if (err) {
		msg("Unable to change to %s\n", SYS_DEV_DIR);
		return -EFAULT;
	}
	tdir = opendir(SYS_DEV_DIR);
	if (!tdir) {
		msg("Unable to open %s\n", SYS_DEV_DIR);
		return -EFAULT;
	}

	while ((tdirent = readdir(tdir))) {
		if (strstr(tdirent->d_name, driver_name)) {
			if (snprintf(dt_dir, MAX_PATH, "%s%s/", SYS_DEV_DIR,
				 tdirent->d_name) < 0)
				msg("%s: value truncation\n", __func__);
			dbgmsg("%s: Found %s at %s\n", __func__, driver_name,
			       dt_dir);
			ret = 0;
			break;
		}
	}

	closedir(tdir);
	err = chdir(cwd);

	return ret;
}

/*===========================================================================
FUNCTION search_sysfs_dir_name

Find path for driver name in a given directory.

ARGUMENTS
	dest_dir - final absolute path returned back to caller on success
	path   - Sysfs directory where it needs to search
	driver_name - name to search in path mentioned in 2nd argument

RETURN VALUE
	Return 0 on sucess, negative integer on failure.
===========================================================================*/
int search_sysfs_dir_name(char *dest_dir, char *path, const char *driver_name)
{
	DIR *tdir = NULL;
	struct dirent *tdirent = NULL;
	char cwd[MAX_PATH] = {0};
	int ret = -ENODEV, err;

	if (!getcwd(cwd, sizeof(cwd)))
		return ret;

	err = chdir(path);
	if (err) {
		msg("Unable to change to %s. err:%d\n", path, err);
		return err;
	}
	tdir = opendir(path);
	if (!tdir) {
		msg("Unable to open %s\n", path);
		ret = chdir(cwd);
		return -EFAULT;
	}

	while ((tdirent = readdir(tdir))) {
		if (strstr(tdirent->d_name, driver_name)) {
			if (snprintf(dest_dir, MAX_PATH, "%s%s/", path,
				 tdirent->d_name) < 0)
				msg("%s: value truncation\n", __func__);
			dbgmsg("%s: Found %s at %s\n", __func__, driver_name,
			       dest_dir);
			ret = 0;
			break;
		}
	}

	closedir(tdir);
	err = chdir(cwd);

	return ret;
}

/*===========================================================================
FUNCTION check_node

Check the node is present.

ARGUMENTS
	node_name - name of the node

RETURN VALUE
	0 - if the node is present
       -1 - if the node is not present/failure
===========================================================================*/
int check_node(char *node_name)
{
	int fd, ret = 0;

	if (node_name) {
		fd = open(node_name, O_RDONLY);
		if (fd < 0)
			ret = -1;
		else
			close(fd);
	} else {
		ret = -1;
	}

	return ret;
}

ssize_t write_to_trace(const char *msg, size_t count)
{
	ssize_t ret = 0;

	if (trace_fd > -1)
		ret = write(trace_fd, msg, count);
	return ret;
}

/*===========================================================================

FUNCTION timer_thread_start

detect the timer which is expired, then call the timer_expired function to operate

ARGUMENTS
	void*

RETURN VALUE
       NULL - if this function crash
===========================================================================*/
static void* timer_thread_start(void* arg)
{
	int i;
	struct event_data *data;
	int buf[2], ret = 0;

	while (1) {
		struct epoll_event events[MAX_EPOLL_EVENTS];
		int nevents;
		nevents = epoll_wait(epollfd, events, MAX_EPOLL_EVENTS, -1);
		if (nevents == -1) {
			if (errno != EINTR)
				msg("%s: epoll wait failed, errno = %d\n",
					__func__, errno);
			continue;
		}

		for (i = 0; i < nevents; ++i) {
			data = events[i].data.ptr;
			ret = read(data->fd, buf, sizeof(buf));
			if (ret < 0)
				msg("epoll callback read error:%d\n", errno);
			if (data->callback)
				(*(void (*)(int))data->callback)(data->id);
		}
	}
	return NULL;
}

/*===========================================================================
FUNCTION timer_init

initialize the timer,create a epoll for timer and create a thread to detect timer

ARGUMENTS
	void

RETURN VALUE
	0 - if timer init is success
       others - if the init is failure
===========================================================================*/

static int timer_init(void)
{
	int ret_val;

	epollfd = epoll_create(MAX_EPOLL_EVENTS);
	if (epollfd == -1) {
		msg("%s: epoll_create failed, errno = %d\n", __func__, errno);
		return -errno;
	}

	ret_val = pthread_create(&callback_thread, NULL, timer_thread_start, NULL);
	if (ret_val) {
		msg("%s: Error cannot create timer callback thread, errno = %d\n", __func__, errno);
		return -1;
	}

	return ret_val;
}

/*===========================================================================
FUNCTION timer_register_event

register timer

ARGUMENTS
	id - timer ID
	handler - the timer expired function

RETURN VALUE
	timer_id - the file handle of timer ID
===========================================================================*/
int timer_register_event(int id, timer_expired_callback handler)
{
	struct epoll_event ev;
	int timer_id;
	int ret_val;
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	static int timer_inited = 0;
	struct event_data *data = (struct event_data *)malloc(sizeof(struct event_data));

	if (data == NULL) {
		msg("%s: Failed to alloc event data\n", __func__);
		goto data_malloc_err;
	}

	pthread_mutex_lock(&mutex);

	if (timer_inited == 0) {
		ret_val = timer_init();
		if (ret_val) {
			pthread_mutex_unlock(&mutex);
			goto timerfd_create_err;
		}else {
			timer_inited = 1;
		}
	}

	pthread_mutex_unlock(&mutex);

	timer_id = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if (timer_id == -1) {
		msg("%s: timerfd_create failed\n", __func__);
		goto timerfd_create_err;
	}

	data->fd = timer_id;
	data->callback = handler;
	data->id = id;
	ev.events = EPOLLIN | EPOLLPRI;
	ev.data.ptr = (void *)(data);

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, data->fd, &ev) == -1) {
		msg("%s, epoll_ctl failed\n", __func__);
		goto epoll_add_err;
	}

	return timer_id;

epoll_add_err:
	close(timer_id);
timerfd_create_err:
	free(data);
data_malloc_err:
	return -1;
}
