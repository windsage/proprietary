/*=============================================================/
 * Copyright (c) 2018-2020, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * =============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "inc/thermal.h"
#include "inc/thermal_config.h"

#define MAX_CDEV_PRINT_SPACE 184
#define MAX_PRINT_LINE 300
#define TZ_CONFIG_SIZE 1024

static void print_tzone_cdev_limits(struct trip_temp trip_list[], uint trip_count)
{
	char up_lim_line[MAX_PRINT_LINE];
	char low_lim_line[MAX_PRINT_LINE];
	char temp_line[MAX_PRINT_LINE];
	char temp_ul[MAX_PRINT_LINE];
	char temp_ll[MAX_PRINT_LINE];

	/*  %*s, -15: this indicates a left-justified width of 15 for string */
	snprintf(up_lim_line, MAX_PRINT_LINE, "%*s", -15, "upper_limit");
	snprintf(low_lim_line, MAX_PRINT_LINE, "%*s", -15, "lower_limit");

	for (uint i = 0; i < trip_count; ++i) {
		if (trip_list[i].cdev_num > 0) {
			snprintf(temp_ul, MAX_PRINT_LINE, "%.2i",
					trip_list[i].cdev_list[0].upper_limit);
			snprintf(temp_ll, MAX_PRINT_LINE, "%.2i",
					trip_list[i].cdev_list[0].lower_limit);

			for(uint j = 1; j < (trip_list[i].cdev_num); ++j) {
				snprintf(temp_line, MAX_PRINT_LINE, "+%.2i",
						trip_list[i].cdev_list[j].upper_limit);
				strlcat(temp_ul, temp_line, MAX_CDEV_PRINT_SPACE);

				snprintf(temp_line, MAX_PRINT_LINE, "+%.2i",
						trip_list[i].cdev_list[j].lower_limit);
				strlcat(temp_ll, temp_line, MAX_CDEV_PRINT_SPACE);
			}

			snprintf(temp_line, MAX_PRINT_LINE, "%s ", temp_ul);
			strlcat(up_lim_line, temp_line, MAX_CDEV_PRINT_SPACE);

			snprintf(temp_line, MAX_PRINT_LINE, "%s ", temp_ll);
			strlcat(low_lim_line, temp_line, MAX_CDEV_PRINT_SPACE);
		} else {
			snprintf(temp_line, MAX_PRINT_LINE, "%s ", "-");
			strlcat(up_lim_line, temp_line, MAX_CDEV_PRINT_SPACE);
			strlcat(low_lim_line, temp_line, MAX_CDEV_PRINT_SPACE);
		}
	}

	PRINT_SETTING(PRINT_TO_STDOUT, "%s", up_lim_line);
	PRINT_SETTING(PRINT_TO_STDOUT, "%s", low_lim_line);
}

static void print_tzone_trip_points(struct trip_temp trip_list[], uint trip_count)
{
	bool no_cdevs = true;
	bool no_cdev_limits = true;
	char set_line[MAX_PRINT_LINE];
	char clr_line[MAX_PRINT_LINE];
	char device_line[MAX_PRINT_LINE];
	char temp_line[MAX_PRINT_LINE];
	char temp_dev[MAX_PRINT_LINE];

	/*  %*s, -15: this indicates a left-justified width of 15 for string */
	snprintf(set_line, MAX_PRINT_LINE, "%*s", -15, "set_temp");
	snprintf(clr_line, MAX_PRINT_LINE, "%*s", -15, "clr_temp");
	snprintf(device_line, MAX_PRINT_LINE, "%*s", -15, "device");

	for (uint i = 0; i < trip_count; ++i) {
		snprintf(temp_line, MAX_PRINT_LINE, "%d ", trip_list[i].set_temp);
		strlcat(set_line, temp_line, MAX_CDEV_PRINT_SPACE);

		snprintf(temp_line, MAX_PRINT_LINE, "%d ", trip_list[i].clr_temp);
		strlcat(clr_line, temp_line, MAX_CDEV_PRINT_SPACE);

		if (trip_list[i].cdev_num > 0) {
			no_cdevs = false;
			if (trip_list[i].cdev_list[0].upper_limit != INT_MAX)
				no_cdev_limits = false;

			snprintf(temp_dev, MAX_PRINT_LINE, "%s",
					trip_list[i].cdev_list[0].cdev_name);
			for(uint j = 1; j < (trip_list[i].cdev_num); ++j) {
				snprintf(temp_line, MAX_PRINT_LINE, "+%s",
						trip_list[i].cdev_list[j].cdev_name);
				strlcat(temp_dev, temp_line, MAX_CDEV_PRINT_SPACE);
			}
			snprintf(temp_line, MAX_PRINT_LINE, "%s ", temp_dev);
			strlcat(device_line, temp_line, MAX_CDEV_PRINT_SPACE);
		} else {
			snprintf(temp_line, MAX_PRINT_LINE, "%s ", "-");
			strlcat(device_line, temp_line, MAX_CDEV_PRINT_SPACE);
		}
	}

	PRINT_SETTING(PRINT_TO_STDOUT, "%s", set_line);
	PRINT_SETTING(PRINT_TO_STDOUT, "%s", clr_line);

	if (!no_cdevs) {
		PRINT_SETTING(PRINT_TO_STDOUT, "%s", device_line);
		if (!no_cdev_limits)
			print_tzone_cdev_limits(trip_list, trip_count);
	}
}

void print_thermal_zones(void)
{
	struct thermal_zone_info *zone;

	PRINT_SETTING(PRINT_TO_STDOUT, "Thermal Zones\n");
	for (uint index = 0; index < tz_list.num_tz; ++index) {

		zone = &tz_list.tz_info_list[index];

		if (zone->had_error) {
			PRINT_SETTING(PRINT_TO_STDOUT, "[Error_THERMAL_ZONE_%d]\n", index);
		} else if (zone->config) {
			PRINT_SETTING(PRINT_TO_STDOUT, "[THERMAL_ZONE_%d]", index);
			PRINT_SETTING(PRINT_TO_STDOUT, "%s", zone->config);
		} else {
			 /* %*s, -15: this indicates a left-justified width of 15 for string s */
			PRINT_SETTING(PRINT_TO_STDOUT, "[THERMAL_ZONE_%d]", index);
			PRINT_SETTING(PRINT_TO_STDOUT, "%*s%s", -15, "sensor", zone->sensor);
			PRINT_SETTING(PRINT_TO_STDOUT, "%*s%s", -15, "algo_type", zone->algo_type);
			PRINT_SETTING(PRINT_TO_STDOUT, "%*s%s", -15, "mode",
					zone->mode?"enabled":"disabled");
			if (zone->polling_delay != INT_MAX)
				PRINT_SETTING(PRINT_TO_STDOUT, "%*s%d", -15, "polling_delay",
						zone->polling_delay);
			if (zone->passive_delay != INT_MAX)
				PRINT_SETTING(PRINT_TO_STDOUT, "%*s%d", -15, "passive_delay",
						zone->passive_delay);
			if (zone->trip_temp_num > 0)
				print_tzone_trip_points(zone->trip_temp_list, zone->trip_temp_num);
			PRINT_SETTING(PRINT_TO_STDOUT, " ");
		}
	}
}

static int convert_str_to_int(const char *str, int *int_field)
{
	char *unused;
	long temp;

	temp = strtol(str, &unused, 10);
	if (*unused!='\0')
		return -1;
	else if (temp > INT_MAX || temp < INT_MIN)
		return -1;

	*int_field = (int)temp;
	return 0;
}

static int remove_newline(char *str)
{
	int start_len = (int)strlen(str);
	char *newLine = strchr(str, '\n');

	if(newLine != NULL)
		*newLine = '\0';

	if ((start_len - (int)strlen(str)) > 1)
		return -1;
	else
		return ((int)strlen(str));
}

static int write_string(const char *path, const char *buf, size_t count)
{
	int fd;
	ssize_t rv = 0, pos = 0;

	if (!path) return -EINVAL;

	fd = open(path, O_RDWR);
	if (fd < 0) {
		msg("open%s failed%d", path, errno);
		return fd;
	}

	do {
		dbgmsg("writing:%s in fd:%d bytes:%zu\n", buf, fd, count);
		rv = write(fd, buf + (size_t)pos, count - (size_t)pos);
		if (rv < 0)
			return -errno;
		pos += rv;
	} while ((ssize_t)count > pos);
	close(fd);

	return (int)count;
}

static int read_string(const char path[], char info[], const size_t info_size, const char calling_func[])
{
	int ret;

	ret = read_line_from_file(path, info, info_size);
	if (ret < 0) {
		if(ret != -ENOENT)
			msg("%s: Error reading from %s: errno %d", calling_func, path, -ret);
		return ret;
	}

	ret = remove_newline(info);
	if (ret <= 0)
		msg("%s: remove_newline error %s empty or truncated", calling_func, path);

	return ret;
}

static int read_config(const char path[], char info[], const size_t info_size, const char calling_func[])
{
	int fd;
	ssize_t rv = 0;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		if(fd != -ENOENT)
			msg("%s: Error opening from %s: errno %d\n", calling_func, path, -fd);
		return fd;
	}

	rv = read(fd, info, info_size);
	if (rv <= 0) {
		if(errno != ENOENT)
			msg("%s: Error reading from %s: errno %d\n", calling_func, path, -errno);
		close(fd);
		return -errno;
	}
	close(fd);

	return (int)rv;
}

static int read_int(const char path[], const char calling_func[], int *int_field)
{
	char info_string[UINT_BUF_MAX];
	int ret;

	ret = read_string(path, info_string, UINT_BUF_MAX, calling_func);
	if (ret < 0)
		return ret;

	ret = convert_str_to_int(info_string, int_field);
	if (ret < 0) {
		msg("%s: Invalid number value %s", calling_func, path);
		return -1;
	}

	return 0;
}

static void get_cdevs(int zone_num, struct thermal_zone_info * zone)
{
	int read_return = 0, ret = 0;
	int trip_num = -1;
	int total_cdev_count = 0;
	struct cooling_device *cdev;
	struct trip_temp *trip;
	char cdev_info_path[MAX_PATH];
	char cdev_desc[MAX_TZ_NAME_LEN];

	while (read_return >= 0) {
		snprintf(cdev_info_path, MAX_PATH, TZ_CDEV_TRIP_POINT, zone_num,
				total_cdev_count);
		read_return = read_int(cdev_info_path, __func__, &trip_num);
		if (read_return < 0) {
			if (read_return != -ENOENT)
				zone->had_error = true;
			continue;
		}

		if((trip_num >= 0) && ((uint)trip_num < (zone->trip_temp_num))) {
			trip = &(zone->trip_temp_list[trip_num]);
			cdev = &(trip->cdev_list[trip->cdev_num]);
			++(trip->cdev_num);
		} else {
			zone->had_error = true;
			msg("%s: cdev associated trip point does not exist: %s",
					__func__, cdev_info_path);
			continue;
		}

		snprintf(cdev_info_path, MAX_PATH, TZ_CDEV_TYPE, zone_num,
				total_cdev_count);
		read_return = read_string(cdev_info_path, cdev_desc, MAX_TZ_NAME_LEN, __func__);
		if(read_return <= 0) {
			zone->had_error = true;
			continue;
		} else {
			snprintf(cdev->cdev_name, MAX_TZ_NAME_LEN, "%s", cdev_desc);
		}

		trip->cdev_print_space = (trip->cdev_print_space) +
			((int)strlen(cdev->cdev_name) + 1);

		snprintf(cdev_info_path, MAX_PATH, TZ_CDEV_UPPER_LIM, zone_num,
				total_cdev_count);
		ret = read_int(cdev_info_path, __func__, &(cdev->upper_limit));
		if (ret < 0) {
			cdev->upper_limit = INT_MAX;
			goto config_exit;
		}

		snprintf(cdev_info_path, MAX_PATH, TZ_CDEV_LOWER_LIM, zone_num,
				total_cdev_count);
		read_return = read_int(cdev_info_path, __func__, &(cdev->lower_limit));
		if (read_return < 0) {
			zone->had_error = true;
			continue;
		}

config_exit:
		cdev->cdevID = total_cdev_count;
		++total_cdev_count;
	}
}

static void get_trip_temps(int zone_num, struct thermal_zone_info *zone)
{
	int ret = 0, read_return = 0;
	zone->trip_temp_num = 0;
	char trip_info_path[MAX_PATH];

	while (read_return >= 0) {
		struct trip_temp *trip = &(zone->trip_temp_list[zone->trip_temp_num]);

		snprintf(trip_info_path, MAX_PATH, TZ_SET_TEMP, zone_num, zone->trip_temp_num);
		read_return = read_int(trip_info_path, __func__, &(trip->set_temp));
		if (read_return < 0) {
			if (read_return != -ENOENT)
				zone->had_error = true;
			continue;
		}

		snprintf(trip_info_path, MAX_PATH, TZ_HYST, zone_num, zone->trip_temp_num);
		ret = read_int(trip_info_path, __func__, &(trip->clr_temp));
		if (ret < 0)
			goto config_exit;

config_exit:
		trip->cdev_num = 0;
		trip->cdev_print_space = 0;
		++(zone->trip_temp_num);
	}
}

void get_thermal_zone_info(void)
{
	char info_path[MAX_PATH];
	int ret = 0, read_return = 0;
	int delay_val = 0;
	int index = 0;
	struct thermal_zone_info *zone;
	char mode[MAX_TZ_NAME_LEN] = {0};

	for (index = 0; index < MAX_NUM_TZ; index++) {
		zone = &tz_list.tz_info_list[index];
		zone->had_error = false;

		snprintf(info_path, MAX_PATH, TZ_TYPE, index);
		read_return = read_string(info_path, zone->sensor, MAX_TZ_NAME_LEN, __func__);
		if (read_return < 0)
			break;

		if (output_conf) {
			snprintf(info_path, MAX_PATH, TZ_CONFIG);
			if (!access(info_path, F_OK)) {
				ret = write_string(info_path, zone->sensor, strlen(zone->sensor));
				if (ret <= 0) {
					msg("failed to write sensor name %s to debug config node. ret : %d\n",
							zone->sensor, ret);
					break;
				}

				if (zone->config == NULL) {
					zone->config = calloc(TZ_CONFIG_SIZE, sizeof(char));
					if (zone->config == NULL) {
						msg("%s: calloc error for zone config\n", __func__);
						break;
					}
				}

				read_return = read_config(info_path, zone->config, TZ_CONFIG_SIZE, __func__);
				if (read_return < 0) {
					if (read_return == -ENOENT)
						break;

					zone->had_error = true;
					snprintf(zone->tz_name, MAX_TZ_NAME_LEN, "THERMAL_ZONE_%d ERROR", index);
				} else {
					snprintf(zone->tz_name, MAX_TZ_NAME_LEN, "THERMAL_ZONE_%d", index);
				}
			}
		}

		snprintf(info_path, MAX_PATH, TZ_ALGO, index);
		read_return = read_string(info_path, zone->algo_type, MAX_TZ_NAME_LEN, __func__);
		if (read_return < 0) {
			if (read_return == -ENOENT)
				break;

			zone->had_error = true;
			snprintf(zone->tz_name, MAX_TZ_NAME_LEN, "THERMAL_ZONE_%d ERROR", index);
			continue;
		}

		snprintf(info_path, MAX_PATH, TZ_MODE, index);
		read_return = read_string(info_path, mode, MAX_TZ_NAME_LEN, __func__);
		if (read_return < 0) {
			zone->had_error = true;
			snprintf(zone->tz_name, MAX_TZ_NAME_LEN, "THERMAL_ZONE_%d ERROR", index);
			continue;
		}
		if ((strcmp(mode, "enabled")) == 0)
			zone->mode = true;

		snprintf(info_path, MAX_PATH, TZ_POLLING_DELAY, index);
		read_return = read_int(info_path, __func__, &delay_val);
		if (read_return < 0)
			zone->polling_delay = INT_MAX;
		else
			zone->polling_delay = (uint)delay_val;

		snprintf(info_path, MAX_PATH, TZ_PASSIVE_DELAY, index);
		read_return = read_int(info_path, __func__, &delay_val);
		if (read_return < 0)
			zone->passive_delay = INT_MAX;
		else
			zone->passive_delay = (uint)delay_val;

		get_trip_temps(index, zone);

		if(!zone->had_error)
			get_cdevs(index, zone);

		if (zone->had_error)
			snprintf(zone->tz_name, MAX_TZ_NAME_LEN, "THERMAL_ZONE_%d ERROR", index);
		else
			snprintf(zone->tz_name, MAX_TZ_NAME_LEN, "THERMAL_ZONE_%d", index);
	}

	tz_list.num_tz = (uint)index;
}
