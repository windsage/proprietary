#ifndef TS_LINUX_H
#define TS_LINUX_H

/*===========================================================================

                   header file for timestamp

DESCRIPTION
  API to extract timestamp from system. Used only for Linux


# Copyright (c) 2007-2011, 2018, 2020-2021 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "comdef.h"
/*===========================================================================
FUNCTION   ts_get

DESCRIPTION
  Extracts timestamp from system

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/
 void ts_get (void *timestamp);

/*===========================================================================
FUNCTION   ts_get_lohi

DESCRIPTION
  Extracts timestamp from system and places into lo and hi parameters

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/
 void ts_get_lohi(uint32 *ts_lo, uint32 *ts_hi);

 #endif /* TS_LINUX_H */
