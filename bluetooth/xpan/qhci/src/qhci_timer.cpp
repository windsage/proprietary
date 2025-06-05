/*
 *  Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <utils/Log.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include "qhci_timer.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.qti@1.0-xpan_qhci-timer"
std::mutex qhci_timer_mutex;
struct qhci_alarm_t *qhci_create_timer(const char *name, qhci_timer_cb cb)
{
  struct qhci_alarm_t *timer;

  std::unique_lock<std::mutex> guard(qhci_timer_mutex);
  timer = (qhci_alarm_t*) malloc(sizeof(struct qhci_alarm_t));
  if (!timer){
    ALOGE("%s: failed to timer :%s", __func__, name);
    return NULL;
  }

  memset(timer, 0, sizeof(struct qhci_alarm_t));
  timer->name = name;

  timer->sigevt.sigev_notify_function = cb;
  timer->sigevt.sigev_notify = SIGEV_THREAD;
  timer->sigevt.sigev_value.sival_ptr = &timer->timer_id;
  timer->sigevt.sigev_notify_attributes = NULL;

  if (timer_create(CLOCK_MONOTONIC, &timer->sigevt, &timer->timer_id) != 0) {
    ALOGD("%s: failed to create timer %s", __func__, name);
    free(timer);
    return NULL;
  }

  timer->timer_state = QHCI_TIMER_CREATED;

  ALOGI("%s: created %s timer", __func__, name);
  return timer;
}

void qhci_delete_timer(struct qhci_alarm_t *timer)
{
  std::unique_lock<std::mutex> guard(qhci_timer_mutex);
  timer_delete(timer->timer_id);
  free(timer);
  timer = NULL;
}

void qhci_start_timer(struct qhci_alarm_t *timer, uint32_t timeout_ms)
{
  std::unique_lock<std::mutex> guard(qhci_timer_mutex);

  struct itimerspec ts;
  timer->timeout_ms = timeout_ms;
  ts.it_value.tv_sec = timeout_ms / 1000;
  ts.it_value.tv_nsec = 1000000 * (timeout_ms % 1000);
  ts.it_interval.tv_sec = 0;
  ts.it_interval.tv_nsec = 0;

  if (timer_settime(timer->timer_id, 0, &ts, 0) == -1) {
    ALOGE("%s: Failed to start timer :%s with timeout:%u", __func__, timer->name,
	  timeout_ms);
    return;
  }

  timer->timer_state = QHCI_TIMER_ACTIVE;
  ALOGI("%s: Started %s timer with timeout:%u", __func__, timer->name,
	timeout_ms);
}

void qhci_stop_timer(struct qhci_alarm_t *timer)
{
  std::unique_lock<std::mutex> guard(qhci_timer_mutex);

  struct itimerspec ts;
  memset(&ts, 0, sizeof(struct itimerspec));

  if (timer_settime(timer->timer_id, 0, &ts, 0) == -1) {
    ALOGE("%s: Failed to stop %s timer", __func__, timer->name);
    return;
  }

  timer->timer_state = QHCI_TIMER_CREATED;
}

