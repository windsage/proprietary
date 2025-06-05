/*!
  @file
  sendcmd.h

  @brief
	Contains functions required to send commands to Android through the
	IAtFwd Service
*/

/*===========================================================================

Copyright (c) 2015, 2017, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who      what, where, why
--------   ---      ---------------------------------------------------------
01/06/23   snehb     Switch to AIDL service with native server
                     and Java client
04/11/11   jaimel    First cut.


===========================================================================*/

#ifndef __ATFWDDAEMON_H
#define __ATFWDDAEMON_H

#include "AtCmdFwd.h"

#ifdef __cplusplus
extern "C" {
#endif
void registerHalService();
AtCmdResponse *processAtCommand(const AtCmd *cmd);
#ifdef __cplusplus
}
#endif

#endif /* __ATFWDDAEMON_H */
