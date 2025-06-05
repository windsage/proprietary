/*===========================================================================
 *
 *   Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 *   All Rights Reserved.
 *   Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <cutils/properties.h>
#include "fan_service.h"

int main(int argc, char* argv[])
{
	char fan_prop[PROPERTY_VALUE_MAX];
	int fan_status = atoi(argv[1]);
	int handle = find_cooling_device(COOLING_FAN, fan_status);
	sleep(2);
	DEBUGV(LOG_TAG, "Fan controller running status: %d, fan_status: %d\n",
					handle, fan_status);
	return 0;
}

int find_cooling_device(char* type, int status)
{
	DIR *tdir = NULL;
	struct dirent *tdirent = NULL;
	char name[MAX_PATH] = {0};
	char cmd[MAX_PATH] = {0};
	FILE *fp;
	int running_status = 0;
	int fan_speed = 0;

	DEBUGV(LOG_TAG, "Matching type: %s", type);

	if (chdir(THERMAL_SYSFS) < 0) {
		DEBUGV(LOG_TAG, "Unable to change path to %s", THERMAL_SYSFS);
		return -1;
	}

	tdir = opendir(THERMAL_SYSFS);
	if (!tdir) {
		DEBUGV(LOG_TAG, "Unable to open %s\n", THERMAL_SYSFS);
		return -1;
	}

	while ((tdirent = readdir(tdir))) {
		char buf[MAX_PATH];
		DIR *cdevdir = NULL;

		if (strncmp(tdirent->d_name, CDEV_DIR_NAME, strlen(CDEV_DIR_NAME)))
			continue;

		cdevdir = opendir(tdirent->d_name);
		if (!cdevdir)
			continue;

		if (snprintf(name, MAX_PATH, "/sys/class/thermal/%s/type", tdirent->d_name) < 0)
			DEBUGV(LOG_TAG, "Error type path: %s", name);

		int handle = read_line_from_file(name, buf, sizeof(buf));
		if (handle <= 0) {
			closedir(cdevdir);
			continue;
		}

		buf[strlen(buf) - 1] = '\0';

		if (status == 1)
			fan_speed = 100;

		if (strncmp(buf, type, strlen(type)) == 0) {
			running_status = 1;
			snprintf(cmd, MAX_PATH, "echo %d > /sys/class/thermal/%s/cur_state 2>&1",
					fan_speed, tdirent->d_name);
			fp = popen(cmd, "r");
			if (fp == NULL)
				DEBUGV(LOG_TAG, "CMD echo status failed");

			return running_status;
		}
	}

	closedir(tdir);
	return running_status;
}

int read_line_from_file(const char *path, char *buf, size_t count)
{
	char *fgets_ret;
	FILE *fd;
	int rv;

	fd = fopen(path, "r");
	if (fd == NULL)
		return -1;

	fgets_ret = fgets(buf, (int)count, fd);
	if (NULL != fgets_ret) {
		rv = (int)strlen(buf);
	} else
		rv = ferror(fd);

	fclose(fd);
	return rv;
}
