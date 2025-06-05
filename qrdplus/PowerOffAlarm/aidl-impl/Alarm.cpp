/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <log/log.h>

#include "Alarm.h"
#include <sys/time.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <cutils/properties.h>

#define DEFAULT_RTC_DEV_PATH "/dev/rtc0"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace alarm {


// time is the absolute time based on rtc time
::ndk::ScopedAStatus Alarm::setAlarm(int64_t time, int32_t* _aidl_return) {
    int fd, rc;
    struct tm alarm_tm;
    struct rtc_wkalrm rtc_alarm;
    time_t alarm_secs = 0;

    ALOGD("alarm hal setAlarm time");

    fd = open(DEFAULT_RTC_DEV_PATH, O_RDONLY);
    if(fd < 0) {
         ALOGE("Open rtc dev failed when set alarm!");
         *_aidl_return = fd;
         return ndk::ScopedAStatus::ok();
    }

    alarm_secs = time;
    gmtime_r(&alarm_secs, &alarm_tm);

    rtc_alarm.time.tm_sec = alarm_tm.tm_sec;
    rtc_alarm.time.tm_min = alarm_tm.tm_min;
    rtc_alarm.time.tm_hour = alarm_tm.tm_hour;
    rtc_alarm.time.tm_mday = alarm_tm.tm_mday;
    rtc_alarm.time.tm_mon = alarm_tm.tm_mon;
    rtc_alarm.time.tm_year = alarm_tm.tm_year;
    rtc_alarm.time.tm_wday = alarm_tm.tm_wday;
    rtc_alarm.time.tm_yday = alarm_tm.tm_yday;
    rtc_alarm.time.tm_isdst = alarm_tm.tm_isdst;

    rtc_alarm.enabled = 1;

    rc = ioctl(fd,RTC_WKALM_SET, &rtc_alarm);

    close(fd);

    if (rc < 0) {
       ALOGE("Set alarm to rtc failed!");
       *_aidl_return = rc;
       return ndk::ScopedAStatus::ok();
    }
    *_aidl_return = 0;
    return ndk::ScopedAStatus::ok();
}

// Return the alarm time in rtc register(seconds)
::ndk::ScopedAStatus Alarm::getAlarm(int64_t* _aidl_return) {
    int fd, rc;
    struct rtc_wkalrm rtc_alarm;
    long alarm_time;
    struct tm alarm_tm;

    ALOGD("alarm hal getAlarm");

    fd = open(DEFAULT_RTC_DEV_PATH, O_RDONLY);
    if(fd < 0) {
         ALOGE("Open rtc dev failed when get alarm");
         *_aidl_return = fd;
         return ndk::ScopedAStatus::ok();
    }

    rc = ioctl(fd,RTC_WKALM_RD, &rtc_alarm);
    if (rc < 0) {
        ALOGE("Get alarm from rtc failed");
        close(fd);
        *_aidl_return = rc;
        return ndk::ScopedAStatus::ok();
    }

    alarm_tm.tm_sec = rtc_alarm.time.tm_sec;
    alarm_tm.tm_min = rtc_alarm.time.tm_min;
    alarm_tm.tm_hour = rtc_alarm.time.tm_hour;
    alarm_tm.tm_mday = rtc_alarm.time.tm_mday;
    alarm_tm.tm_mon = rtc_alarm.time.tm_mon;
    alarm_tm.tm_year = rtc_alarm.time.tm_year;
    alarm_tm.tm_wday = rtc_alarm.time.tm_wday;
    alarm_tm.tm_yday = rtc_alarm.time.tm_yday;
    alarm_tm.tm_isdst = rtc_alarm.time.tm_isdst;

    alarm_time = timegm(&alarm_tm);

    close(fd);
    *_aidl_return = alarm_time;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Alarm::cancelAlarm(int32_t* _aidl_return) {
    struct rtc_wkalrm rtc_alarm;
    int fd, rc;

    ALOGD("alarm hal cancelAlarm");

    fd = open(DEFAULT_RTC_DEV_PATH, O_RDONLY);
    if(fd < 0) {
        ALOGE("Open rtc dev failed when cancel alarm");
        *_aidl_return = fd;
        return ndk::ScopedAStatus::ok();
    }

    // set 1970-1-1 to cancel alarm
    rtc_alarm.time.tm_sec = 0;
    rtc_alarm.time.tm_min = 0;
    rtc_alarm.time.tm_hour = 0;
    rtc_alarm.time.tm_mday = 1;
    rtc_alarm.time.tm_mon = 0;
    rtc_alarm.time.tm_year = 70;

    rtc_alarm.enabled = 0;

    rc = ioctl(fd, RTC_WKALM_SET, &rtc_alarm);

    close(fd);

    if (rc < 0) {
        ALOGE("Cancel alarm in rtc failed");
        *_aidl_return = rc;
        return ndk::ScopedAStatus::ok();
    }

    *_aidl_return = 0;
    return ndk::ScopedAStatus::ok();
}

// Return rtc time seconds
::ndk::ScopedAStatus Alarm::getRtcTime(int64_t* _aidl_return) {
    int fd, rc;
    struct tm rtc_tm;
    time_t rtc_secs = 0;

    ALOGD("alarm hal getRtcTime");

    fd = open(DEFAULT_RTC_DEV_PATH, O_RDONLY);
    if(fd < 0) {
        ALOGE("Open rtc dev failed when get rtc time");
        *_aidl_return = fd;
        return ndk::ScopedAStatus::ok();
    }

    rc = ioctl(fd,RTC_RD_TIME, &rtc_tm);
    if (rc <0) {
        ALOGE("Get rtc time failed");
        close(fd);
        *_aidl_return = rc;
        return ndk::ScopedAStatus::ok();
    }
    rtc_secs = timegm(&rtc_tm);

    close(fd);
    *_aidl_return = rtc_secs;
    return ndk::ScopedAStatus::ok();
}


}  // namespace alarm
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
