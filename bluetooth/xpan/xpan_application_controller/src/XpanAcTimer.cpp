/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <log/log.h>
#include "xpan_ac_int.h"
#include "signal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.bluetooth.xpan_ac.XpanAcTimer"

/* Xpan Application Controller Timer Implementation. */
namespace xpan {
namespace ac {

XpanAcTimer::XpanAcTimer(std::string name, xpan_ac_notify_cb cb, void *data) {
  mName = name;
  mCallbackFunction = cb;
  cb_data = data;
  mIsTimerCreated = false;
  mIsTimerStarted = false;
  Create();
}

XpanAcTimer::~XpanAcTimer() {
  ALOGD("%s: Timer (%s) ", __func__, mName.c_str());
  mIsTimerCreated = false;
  mIsTimerStarted = false;
  timer_delete(mTimer);
}

void XpanAcTimer::Create() {
  ALOGD("%s: Timer (%s) ", __func__, mName.c_str());

  int retval = 0;
  struct sigevent signal_event;

  signal_event.sigev_notify = SIGEV_THREAD;
  signal_event.sigev_value.sival_ptr = this;
  signal_event.sigev_notify_function = TimerCallback;
  signal_event.sigev_notify_attributes = NULL;

  retval = timer_create(CLOCK_MONOTONIC, &signal_event, &mTimer);

  if (retval == -1) {
    ALOGE("%s: Error creating timer", __func__);
    mIsTimerCreated = false;
    return;
  }

  // Set that the timer is created
  mIsTimerCreated = true;
}

bool XpanAcTimer::StartTimer(uint32_t timeout_ms) {
  ALOGD("%s: Timer (%s) - %d", __func__, mName.c_str(), timeout_ms);
  mTimeoutMs = timeout_ms;

  if (!mIsTimerCreated) {
    ALOGE("%s: Timer(%s) not created for starting", __func__, mName.c_str());
    return false;
  }

  int retval = 0;
  struct itimerspec timer_var;

  timer_var.it_value.tv_sec = mTimeoutMs/1000;
  timer_var.it_value.tv_nsec = 1000000 * (mTimeoutMs % 1000);
  timer_var.it_interval.tv_sec = 0;
  timer_var.it_interval.tv_nsec = 0;

  retval = timer_settime(mTimer, 0, &timer_var, 0);
  if (retval == -1) {
    ALOGE("%s: Couldnt start timer %s", __func__, mName.c_str());
    return false;
  }

  // Set that the timer is started
  mIsTimerStarted = true;
  return true;
}

void XpanAcTimer::TimerCallback(union sigval arg) {
  XpanAcTimer *sysTimer = static_cast<XpanAcTimer *>(arg.sival_ptr);
  if (!sysTimer) {
    ALOGE("%s: Invalid timer instance", __func__);
    return;
  }

  ALOGD("%s: Timer %s timed out", __func__, sysTimer->mName.c_str());
  sysTimer->mCallbackFunction(sysTimer->cb_data);
}

bool XpanAcTimer::StopTimer() {
  ALOGD("%s: %s ", __func__, mName.c_str());

  struct itimerspec time_spec = {};
  int retval = timer_settime(mTimer, 0, &time_spec, 0);
  if (retval == -1) {
    ALOGE("%s: Couldnt stop timer %s", __func__, mName.c_str());
    return false;
  }

  mIsTimerStarted = false;
  return true;
}

bool XpanAcTimer::DeleteTimer() {
  ALOGD("%s: %s ", __func__, mName.c_str());
  timer_delete(mTimer);
  mIsTimerCreated = false;
  return true;
}

} // ac
} // xpan
