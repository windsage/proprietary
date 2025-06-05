/*!
  @file
  dsi_netctrl_multimodem.h

  @brief
  This file declares multi modem specific routines used by
  dsi_netctrli.c

*/

/*===========================================================================

  Copyright (c) 2010, 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/restricted/linux/android/ril/qcril_dsi.c $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/10   js      created file

===========================================================================*/

#include "dsi_netctrl.h"

#ifndef _DSI_NETCTRL_MM_
#define _DSI_NETCTRL_MM_

extern int dsi_mm_get_pref_modem();

extern int dsi_mm_iface_look_up
(
  dsi_store_t * st_hndl,
  int *piface,
  int *pmodem,
  int *ptech
);

extern int dsi_mm_check_matching_call
(
  dsi_store_t               *st_hndl,
  dsi_matching_call_check_t *call_params,
  char                      *device_name,
  int                       device_name_len
);

#endif /* _DSI_NETCTRL_MM_ */
