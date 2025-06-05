/******************************************************************************
  @file    client.h
  @brief   Android performance iop library

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2014-2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __IOPCLIENT_H__
#define __IOPCLIENT_H__

#define PREFERRED_OUT_LENGTH 12288

int perf_io_prefetch_start(int pid, const char *pkg_name,  const char *code_path);
int perf_io_prefetch_stop();
int perf_ux_engine_events(int opcode, int pid, const char *pkg_name, int lat);
void perf_ux_engine_trigger(int opcode, char *preferred_apps);
#endif //__IOPCLIENT_H__

#ifdef __cplusplus
}
#endif
