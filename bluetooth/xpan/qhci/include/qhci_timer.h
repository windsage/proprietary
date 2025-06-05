/*
 *  Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __TIMER_H_
#define __TIMER_H_

#pragma once
#include <mutex>
#include <signal.h>

typedef void (*qhci_timer_cb)(union  sigval);

enum Timer_State {
  QHCI_TIMER_NOT_CREATED = 0x00,
  QHCI_TIMER_CREATED = 0x01,
  QHCI_TIMER_ACTIVE = 0x02,
  QHCI_TIMER_FIRED = 0x03
};

struct qhci_alarm_t {
  const char* name;
  Timer_State timer_state;
  uint32_t timeout_ms;
  timer_t timer_id;
  struct sigevent sigevt;
  /* This hold the timer specific data */
  void *data;
} __attribute__((packed));


struct qhci_alarm_t *qhci_create_timer(const char *, qhci_timer_cb cb);
void qhci_delete_timer(struct qhci_alarm_t *);
void qhci_start_timer(struct qhci_alarm_t *, uint32_t);
void qhci_stop_timer(struct qhci_alarm_t *);

#endif /*__TIMER_H_ */

