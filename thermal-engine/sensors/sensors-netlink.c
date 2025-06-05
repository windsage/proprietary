/*===========================================================================

  sensor-netlink.c

  DESCRIPTION
  Access functions for the thermal sensors registered via thermal sys.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  setup() function should be called before get_temperature().
  shutdown() function should be called to clean up resources.

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <limits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/mngt.h>
#include <netlink/genl/ctrl.h>
#include <netlink/netlink.h>

#include "sensors-thermal.h"
#include "sensors_manager_internal.h"
#include "thermal.h"

#define THERMAL_TRIP_TEMP "/sys/class/thermal/thermal_zone%d/trip_point_0_temp"
#define THERMAL_TRIP_HYST "/sys/class/thermal/thermal_zone%d/trip_point_0_hyst"
#define THERMAL_MODE "/sys/class/thermal/thermal_zone%d/mode"
#define LVL_BUF_MAX (12)

struct thermal_sensor_data {
	pthread_t thermal_sensor_thread;
	pthread_mutex_t thermal_sensor_mutex;
	pthread_cond_t thermal_sensor_condition;
	int threshold_reached;
	int sensor_shutdown;
	int temperature;
	int last_active_trip;
	int last_active_hyst;
	struct sensor_info *sensor;
	struct thermal_sensor_data *next_ptr;
};

struct __sensor_list_data {
	pthread_mutex_t thermal_sensor_list_mutex;
	struct thermal_sensor_data *head_ptr;
	int sensor_count;
	int8_t sensor_shutdown;
};
static struct __sensor_list_data sensor_list_ptr;
void thermal_sensor_init(void);

static struct thermal_sensor_data *fetch_sensor(
		struct __sensor_list_data *sensor_list, int tzn)
{
	struct thermal_sensor_data *curr = sensor_list->head_ptr;

	while (curr != NULL) {
		if (tzn == curr->sensor->tzn)
			break;
		curr = curr->next_ptr;
	}

	return curr;
}

static void search_and_notify(struct __sensor_list_data *sensor_list,
				int zone_num)
{
	struct thermal_sensor_data *sensor_dt = NULL;

	dbgmsg("%s: Notify TZ:%d\n", __func__, zone_num);
	pthread_mutex_lock(&sensor_list->thermal_sensor_list_mutex);
	sensor_dt = fetch_sensor(sensor_list, zone_num);
	if (!sensor_dt) {
		msg("sensor name fetch error for TZ:%d\n", zone_num);
		pthread_mutex_unlock(
				&sensor_list->thermal_sensor_list_mutex);
		return;
	}
	pthread_mutex_lock(&(sensor_dt->thermal_sensor_mutex));
	sensor_dt->threshold_reached = 1;
	pthread_cond_broadcast(&(sensor_dt->thermal_sensor_condition));
	pthread_mutex_unlock(&(sensor_dt->thermal_sensor_mutex));
	pthread_mutex_unlock(&sensor_list->thermal_sensor_list_mutex);
}

static void thermal_sensor_trip_violation(int tz_id, int trip_id, bool trip_up, void *data)
{
	struct __sensor_list_data *sensor_list =
		(struct __sensor_list_data *)data;

	search_and_notify(sensor_list, tz_id);
}

static void thermal_sensor_temp_sample(int tz_id, int temp, void *data)
{
	struct __sensor_list_data *sensor_list =
		(struct __sensor_list_data *)data;
	struct thermal_sensor_data *sensor_dt = NULL;

	dbgmsg("%s: Tz:%d temp:%d\n", __func__, tz_id, temp);
	pthread_mutex_lock(&sensor_list->thermal_sensor_list_mutex);
	sensor_dt = fetch_sensor(sensor_list, tz_id);
	if (!sensor_dt) {
		msg("sensor name fetch error for TZ:%d\n", tz_id);
		pthread_mutex_unlock(&sensor_list->thermal_sensor_list_mutex);
		return;
	}
	pthread_mutex_lock(&(sensor_dt->thermal_sensor_mutex));
	sensor_dt->temperature = temp;
	pthread_mutex_unlock(&(sensor_dt->thermal_sensor_mutex));
	pthread_mutex_unlock(&sensor_list->thermal_sensor_list_mutex);

	/* There is a corner case, where the temperature could have gone
	 * above the threshold that is being set. In this case, we just
	 * get the temperature sample event and not trip violation event.
	 * Till this is fixed in kernel, notify the sensors if there is
	 * an immediate violation*/
	search_and_notify(sensor_list, tz_id);
}

void thermal_sensor_interrupt_wait(struct sensor_info *sensor)
{
	struct thermal_sensor_data *sensor_data;

	if (NULL == sensor ||
	    NULL == sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	sensor_data = (struct thermal_sensor_data *) sensor->data;

	if (sensor->interrupt_enable) {
		/* Wait for sensor threshold condition */
		pthread_mutex_lock(&(sensor_data->thermal_sensor_mutex));
		while (!sensor_data->threshold_reached) {
			pthread_cond_wait(&(sensor_data->thermal_sensor_condition),
					&(sensor_data->thermal_sensor_mutex));
		}
		sensor_data->threshold_reached = 0;
		pthread_mutex_unlock(&(sensor_data->thermal_sensor_mutex));
	}
}

int thermal_sensor_setup(struct sensor_info *sensor)
{
	int fd = -1;
	int tzn = 0;
	char name[MAX_PATH] = {0};
	struct thermal_sensor_data *sensor_data = NULL;

	if (sensor == NULL) {
		msg("thermal_sensor_tz:unexpected NULL");
		return 0;
	}

	tzn = get_tzn(sensor->name);
	if (tzn < 0) {
		msg("No thermal zone device found in the kernel for sensor %s\n", sensor->name);
		return sensor_list_ptr.sensor_count;
	}
	sensor->tzn = tzn;

	snprintf(name, MAX_PATH, TZ_TEMP, sensor->tzn);
	fd = open(name, O_RDONLY);
	if (fd < 0) {
		msg("%s: Error opening %s\n", __func__, name);
		return sensor_list_ptr.sensor_count;
	}

	/* Allocate for thermal sensor data */
	sensor_data =
		(struct thermal_sensor_data *) malloc(sizeof(*sensor_data));
	if (NULL == sensor_data) {
		msg("%s: malloc failed", __func__);
		close(fd);
		return sensor_list_ptr.sensor_count;
	}
	memset(sensor_data, 0, sizeof(struct thermal_sensor_data));
	sensor->data = (void *) sensor_data;
	sensor->fd = fd;

	pthread_mutex_init(&(sensor_data->thermal_sensor_mutex), NULL);
	pthread_cond_init(&(sensor_data->thermal_sensor_condition), NULL);
	sensor_data->sensor_shutdown = 0;
	sensor_data->threshold_reached = 0;
	sensor_data->temperature = INT_MIN;
	sensor_data->sensor = sensor;
	sensor_data->last_active_trip = INT_MAX;
	sensor_data->last_active_hyst = INT_MIN;

	thermal_sensor_init();
	pthread_mutex_lock(&sensor_list_ptr.thermal_sensor_list_mutex);
	sensor_list_ptr.sensor_count++;
	sensor_data->next_ptr = sensor_list_ptr.head_ptr;
	sensor_list_ptr.head_ptr = sensor_data;
	pthread_mutex_unlock(&sensor_list_ptr.thermal_sensor_list_mutex);

	return sensor_list_ptr.sensor_count;
}

int thermal_sensor_get_trip_temp(struct sensor_info *sensor)
{
	struct thermal_sensor_data *sensor_data = NULL;
	int temp;

	if (NULL == sensor) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	sensor_data = (struct thermal_sensor_data *)sensor->data;
	pthread_mutex_lock(&(sensor->read_mtx));
	temp = sensor_data->temperature;
	pthread_mutex_unlock(&(sensor->read_mtx));

	return temp;
}

int thermal_sensor_get_temperature(struct sensor_info *sensor)
{
	char buf[10] = {0};
	int temp = -273;

	if (NULL == sensor) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	pthread_mutex_lock(&(sensor->read_mtx));
	ssize_t len = read(sensor->fd, buf, sizeof(buf)-1);
	if (len != -1) {
		buf[len] = '\0';
		temp = atoi(buf);
	}

	lseek(sensor->fd, 0, SEEK_SET);
	pthread_mutex_unlock(&(sensor->read_mtx));

	if (sensor->scaling_factor)
		temp = CONV(temp)/sensor->scaling_factor;

	return temp;
}

void thermal_sensor_shutdown(struct sensor_info *sensor)
{
	struct thermal_sensor_data *sensor_data;

	if (NULL == sensor ||
	    NULL == sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	sensor_data = (struct thermal_sensor_data *) sensor->data;
	sensor_data->sensor_shutdown = 1;

	if (sensor->fd > -1)
		close(sensor->fd);

	if (sensor->interrupt_enable)
		pthread_join(sensor_data->thermal_sensor_thread, NULL);
	pthread_mutex_lock(&sensor_list_ptr.thermal_sensor_list_mutex);
	sensor_list_ptr.sensor_count--;
	if (!sensor_list_ptr.sensor_count)
		thermal_nl_stop();
	pthread_mutex_unlock(&sensor_list_ptr.thermal_sensor_list_mutex);
	free(sensor_data);
}

static void set_thresholds(struct thermal_sensor_data *sensor_data,
			   struct thresholds_req_t *thresh)
{
	char hystname[MAX_PATH]= {0};
	char tripname[MAX_PATH]= {0};
	char modename[MAX_PATH]= {0};
	char buf[LVL_BUF_MAX] = {0};
	int ret = 0;
	int high_thresh = INT_MAX, low_thresh = INT_MIN;

	if (NULL == sensor_data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	snprintf(hystname, MAX_PATH, THERMAL_TRIP_HYST,
			sensor_data->sensor->tzn);
	snprintf(tripname, MAX_PATH, THERMAL_TRIP_TEMP,
			sensor_data->sensor->tzn);
	snprintf(modename, MAX_PATH, THERMAL_MODE,
			sensor_data->sensor->tzn);

	if (thresh->high_valid || thresh->low_valid) {
		high_thresh = thresh->high;
		if (!thresh->high_valid) {
			if (read_line_from_file(tripname, buf, LVL_BUF_MAX)
					<= 0)
				return;
			high_thresh = atoi(buf);
		}
		if ((thresh->low_valid) && (INT_MIN + thresh->low < high_thresh))
			low_thresh = high_thresh - thresh->low;
		else {
			if (thresh->low_valid)
				info("%s-%d: thresh->low_valid = %d, high_thresh = %d, low_thresh = %d,thresh->low = %d\n",
					__func__, __LINE__, thresh->low_valid, high_thresh,
					low_thresh, thresh->low);
			low_thresh = 0;
		}
	}

	if (thresh->high_valid) {
		if (sensor_data->last_active_trip != high_thresh) {
			dbgmsg("Setting up thresholds high: %d\n", high_thresh);
			snprintf(buf, LVL_BUF_MAX, "%d", high_thresh);
			ret = write_to_file(tripname, buf, strlen(buf));
			if (ret <= 0)
				msg("threshold high failed to set %d\n",
				    high_thresh);
			sensor_data->last_active_trip = high_thresh;
		}
	} else {
		sensor_data->last_active_trip = INT_MAX;
	}

	if (thresh->low_valid) {
		if (sensor_data->last_active_hyst != low_thresh) {
			dbgmsg("Setting up hysteresis: %d\n", low_thresh);
			snprintf(buf, LVL_BUF_MAX, "%d", low_thresh);
			ret = write_to_file(hystname, buf, strlen(buf));
			if (ret <= 0)
				msg("hysteresis threshold failed to set %d\n",
				    low_thresh);
			sensor_data->last_active_hyst = low_thresh;
		}
	} else {
		sensor_data->last_active_hyst = INT_MIN;
	}

	if (read_line_from_file(modename, buf, LVL_BUF_MAX) <=0)
		return;
	if (strncmp(buf, "disabled", strlen("disabled")))
		return;
	dbgmsg("Enabling thermal zone:%d\n", sensor_data->sensor->tzn);
	snprintf(buf, LVL_BUF_MAX, "enabled");
	ret = write_to_file(modename, buf, strlen(buf));
	if (ret <= 0) {
		msg("thermal zone disable failed. err:%d\n", ret);
		return;
	}
}

void thermal_sensor_update_thresholds(struct sensor_info *sensor,
				    struct thresholds_req_t *thresh)
{
	struct thermal_sensor_data *sensor_data;

	if (NULL == thresh ||
	    NULL == sensor ||
	    NULL == sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	sensor_data = (struct thermal_sensor_data *) sensor->data;
	set_thresholds(sensor_data, thresh);
}

void thermal_sensor_init(void)
{
	static int initialized;

	if (initialized)
		return;
	initialized = 1;
	sensor_list_ptr.head_ptr = NULL;
	sensor_list_ptr.sensor_count = 0;
	sensor_list_ptr.sensor_shutdown = 0;
	pthread_mutex_init(&sensor_list_ptr.thermal_sensor_list_mutex, NULL);
	if (thermal_nl_init()) {
		initialized = 0;
		return;
	}
	thermal_nl_register_trip(thermal_sensor_trip_violation,
					&sensor_list_ptr);
	thermal_nl_register_temp_sample(thermal_sensor_temp_sample,
						&sensor_list_ptr);
}
