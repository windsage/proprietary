/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *               System time read support
 *
 *GENERAL DESCRIPTION
 *
 *Implementation of time related API's to read time from different sources.
 *
 **====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "sys_time.h"

#define DIAG_MODEM_EPOCH_DIFFERENCE	315964800
#define TS_LOW_MASK			0xFFFFFFFF
#define TS_HIGH_MASK			0xFFFFFFFF00000000
#define TS_HIGH_SHIFT			32

static unsigned long long get_time_of_day(void)
{
	struct timeval tv;
	unsigned long long seconds, microseconds;

	gettimeofday(&tv, NULL);
	seconds = (unsigned long long)tv.tv_sec;
	/* Offset to sync timestamps between Modem & Apps Proc.
	Number of seconds between Jan 1, 1970 & Jan 6, 1980 */
	if (seconds >= DIAG_MODEM_EPOCH_DIFFERENCE)
               seconds = seconds - DIAG_MODEM_EPOCH_DIFFERENCE;

	seconds = (unsigned long long)(seconds * 1000);
	microseconds = (unsigned long long)tv.tv_usec;
	microseconds = microseconds/1000;
	seconds = seconds + microseconds;
	seconds = seconds*4;
	seconds = seconds/5;
	seconds = seconds << 16;
	return seconds;
}

/* read system time */
void ts_get_lohi(uint32_t *ts_lo, uint32_t *ts_hi)
{
	unsigned long long seconds;

	seconds = get_time_of_day();

	*ts_lo = (uint32_t)(seconds & TS_LOW_MASK);
	*ts_hi = (uint32_t)((seconds & TS_HIGH_MASK) >> TS_HIGH_SHIFT);
}
