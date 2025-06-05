/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <utils/Log.h>
#include <string.h>
#include "xm_state_machine.h"
#include "xpan_utils.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.xpan@1.0-statemachine"

void xm_sm_move_to_next_state(struct xm_state_machine *xm_state)
{
  std::unique_lock<std::mutex> guard(xm_state->state_machine_lock);	
  if (xm_state->current_state == BT_Connecting ||
      xm_state->current_state == XPAN_P2P_Connecting ||
      xm_state->current_state == XPAN_AP_Connecting) {
    xm_state->current_state = (xm_sm_state)((uint32_t)xm_state->current_state + 1);
    ALOGI("%s moving from %s to %s", __func__,
    StateToString((xm_sm_state)((uint32_t)(xm_state->current_state - 1))),
    StateToString(xm_state->current_state));
  } else {
    ALOGI("%s State machine might have gone bad. reseting to default",
          __func__);
    xm_state->current_state = IDLE;
  }

  xm_state->prev_state = IDLE;
}

void xm_sm_revert_current_state(struct xm_state_machine *xm_state)
{
  std::unique_lock<std::mutex> guard(xm_state->state_machine_lock);
  ALOGI("%s reverting from %s to %s", __func__,
	StateToString(xm_state->current_state),
	StateToString(xm_state->prev_state));
  xm_state->current_state = xm_state->prev_state;
  xm_state->prev_state = IDLE;
}

void xm_sm_set_current_state(struct xm_state_machine *xm_state,
		xm_sm_state current_state)
{
  std::unique_lock<std::mutex> guard(xm_state->state_machine_lock);
  ALOGI("%s Moving from %s to %s", __func__,
	StateToString(xm_state->current_state),
	StateToString(current_state));
  xm_state->prev_state = xm_state->current_state;
  xm_state->current_state = current_state;
}

xm_sm_state xm_sm_get_current_state(struct xm_state_machine *xm_state)
{
  std::unique_lock<std::mutex> guard(xm_state->state_machine_lock);
  return xm_state->current_state;
}

xm_sm_state xm_sm_get_prev_state(struct xm_state_machine *xm_state)
{
  std::unique_lock<std::mutex> guard(xm_state->state_machine_lock);
  return xm_state->prev_state;
}

void xm_sm_set_current_active_device(struct xm_state_machine *xm_state, bdaddr_t addr)
{
  std::unique_lock<std::mutex> guard(xm_state->state_machine_lock);
  memcpy(&xm_state->active_bdaddr, &addr, sizeof(bdaddr_t));
}

bdaddr_t xm_sm_get_current_active_device(struct xm_state_machine *xm_state)
{
  std::unique_lock<std::mutex> guard(xm_state->state_machine_lock);
  return xm_state->active_bdaddr;
}

void xm_sm_reset_state(struct xm_state_machine *xm_state)
{
  std::unique_lock<std::mutex> guard(xm_state->state_machine_lock);
  memset(xm_state, 0, sizeof(struct xm_state_machine));
}
