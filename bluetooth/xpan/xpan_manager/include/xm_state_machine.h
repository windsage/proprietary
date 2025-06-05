/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __XM_STATE_MACHINE_H_
#define __XM_STATE_MACHINE_H_

#pragma once
#include <mutex>
#include <signal.h>
#include "xpan_utils.h"

/* State Machine comes into picture when audio started
 * streaming on XPAN and then moved to BT. If audio is streaming
 * of BT by default then state machine will not be activated
 */
typedef enum xm_sm_states {
  /*Default state*/
  IDLE = 0,
  /* Waiting for BT bearer indication after configuring HW ports */
  BT_Connecting = 1,
  /* When BT is active transport */
  BT_Connected = 2,
  /* When Disconnecting BT */
  BT_Disconnecting = 3,
  /* Waiting for XPAN bearer switch indications */
  XPAN_P2P_Connecting = 4,
  /* When XPAN is active transport */
  XPAN_P2P_Connected = 5,
  /* When Disconnecting XPAN */
  XPAN_P2P_Disconnecting = 6,
  /* Waiting for XPAN AP bearer switch indications */
  XPAN_AP_Connecting = 7,
  /* When XPAN AP is active transport */
  XPAN_AP_Connected = 8,
  /* When Disconnecting XPAN AP */
  XPAN_AP_Disconnecting = 9
} xm_sm_state;

struct xm_state_machine {
  std::mutex state_machine_lock;
  xm_sm_state prev_state;
  xm_sm_state current_state;
  bdaddr_t active_bdaddr;
} __attribute__((packed));

void xm_sm_move_to_next_state(struct xm_state_machine *);
void xm_sm_revert_current_state(struct xm_state_machine *);
void xm_sm_set_current_state(struct xm_state_machine *, xm_sm_state);
xm_sm_state xm_sm_get_current_state(struct xm_state_machine *);
xm_sm_state xm_sm_get_prev_state(struct xm_state_machine *);
void xm_sm_set_current_active_device(struct xm_state_machine *, bdaddr_t);
bdaddr_t xm_sm_get_current_active_device(struct xm_state_machine *);
void xm_sm_reset_state(struct xm_state_machine *);

static inline char *StateToString(xm_sm_state state)
{
  if (state == IDLE)
    return "IDLE";
  else if(state == BT_Connecting)
    return "BT_Connecting";
  else if (state == BT_Connected)
    return "BT_Connected";
  else if (state == BT_Disconnecting)
    return "BT_Disconnecting";
  else if (state == XPAN_P2P_Connecting)
    return "XPAN_P2P_Connecting";
  else if (state == XPAN_P2P_Connected)
    return "XPAN_P2P_Connected";
  else if (state == XPAN_P2P_Disconnecting)
    return "XPAN_P2P_Disconnecting";
  else if (state == XPAN_AP_Connecting)
    return "XPAN_AP_Connecting";
  else if (state == XPAN_AP_Connected)
    return "XPAN_AP_Connected";
  else if (state == XPAN_AP_Disconnecting)
    return "XPAN_AP_Disconnecting";
  else
    return "INVALID State";
}
#endif /*__XM_STATE_MACHINE_H_*/
