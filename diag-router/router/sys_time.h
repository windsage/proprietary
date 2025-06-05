/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Internal Header file for system time API support

GENERAL DESCRIPTION

Declaration of API's used to read system time.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef __TIME_H__
#define __TIME_H__

#include <stdint.h>

void ts_get_lohi(uint32_t *ts_lo, uint32_t *ts_hi);

#endif /* __TIME_H__ */
