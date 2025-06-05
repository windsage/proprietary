/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __TIMER_H_
#define __TIMER_H_

#pragma once
#include <mutex>
#include <signal.h>

typedef void (*timer_cb)(union  sigval);

enum Timer_State {
  XM_TIMER_NOT_CREATED = 0x00,
  XM_TIMER_CREATED = 0x01,
  XM_TIMER_ACTIVE = 0x02,
  XM_TIMER_FIRED = 0x03
};

struct alarm_t {
  const char* name;
  Timer_State timer_state;
  uint32_t timeout_ms;
  timer_t timer_id;
  struct sigevent sigevt;
  /* This hold the timer specific data */
  void *data;
} __attribute__((packed));


struct alarm_t *create_timer(const char *, timer_cb cb);
void delete_timer(struct alarm_t *);
void start_timer(struct alarm_t *, uint32_t);
void stop_timer(struct alarm_t *);

#endif /*__TIMER_H_ */
