/*===========================================================================
 *
 *   Copyright (c) 2022 Qualcomm Technologies, Inc.
 *   All Rights Reserved.
 *   Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#ifndef FAN_SERVICE_H
#define FAN_SERVICE_H

#include <log/log.h>

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "fan_service"
#endif

#define THERMAL_SYSFS "/sys/class/thermal/"
#define CDEV_DIR_NAME "cooling_device"
#define COOLING_FAN "fan-max31760"
#define MAX_PATH 255

#define DEBUGV(t,x,...) ALOGI("%s: %s() %d: " x "", t, __FUNCTION__ , __LINE__, ##__VA_ARGS__);

int find_cooling_device(char* type, int status);
int read_line_from_file(const char *path, char *buf, size_t count);

#endif /* FAN_SERVICE_H */
