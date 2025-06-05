/*
 * Copyright (c) 2013-2014, 2017-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * qdss_config.h : Header file for qdss_config
 */
#include "msg.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <errno.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>

/* QDSS defines */
#define QDSSDIAG_PROCESSOR_APPS   0x0100
#define QDSS_DIAG_PROC_ID QDSSDIAG_PROCESSOR_APPS

#define QDSS_QUERY_STATUS              0x00
#define QDSS_TRACE_SINK                0x01
#define QDSS_FILTER_ETM                0x02
#define QDSS_FILTER_STM                0x03
#define QDSS_FILTER_HWEVENT_ENABLE     0x04

#define QDSS_FILTER_HWEVENT_CONFIGURE  0x31
#define QDSS_QTIMER_TS_SYNC            0x60
#define QDSS_ETR1_TRACE_SINK	0x62

#define   TMC_TRACESINK_ETB            0
#define   TMC_TRACESINK_RAM            1
#define   TMC_TRACESINK_TPIU           2
#define   TMC_TRACESINK_USB            3
#define   TMC_TRACESINK_USB_BUFFERED   4
#define   TMC_TRACESINK_SD             6
#define   TMC_TRACESINK_PCIE           8

#define QDSS_RSP_SUCCESS  0
#define QDSS_RSP_FAIL  1

#define QDSS_CLK_FREQ_HZ 19200000
#define QTIMER_CLK_FREQ_HZ 19200000

#define QDSS_ETB_SINK_FILE "/sys/bus/coresight/devices/coresight-tmc-etf/curr_sink"
#define QDSS_ETB_SINK_FILE_2 "/sys/bus/coresight/devices/coresight-tmc-etf/enable_sink"
#define QDSS_ETR_SINK_FILE "/sys/bus/coresight/devices/coresight-tmc-etr/curr_sink"
#define QDSS_ETR_SINK_FILE_2 "/sys/bus/coresight/devices/coresight-tmc-etr/enable_sink"
#define QDSS_ETR_OUTMODE_FILE "/sys/bus/coresight/devices/coresight-tmc-etr/out_mode"
#define QDSS_ETR1_SINK_FILE "/sys/bus/coresight/devices/coresight-tmc-etr1/curr_sink"
#define QDSS_ETR1_SINK_FILE_2 "/sys/bus/coresight/devices/coresight-tmc-etr1/enable_sink"
#define QDSS_ETR1_OUTMODE_FILE "/sys/bus/coresight/devices/coresight-tmc-etr1/out_mode"
#define QDSS_TPIU_SINK_FILE "/sys/bus/coresight/devices/coresight-tpiu/curr_sink"
#define QDSS_TPIU_OUTMODE_FILE "/sys/bus/coresight/devices/coresight-tpiu/out_mode"
#define QDSS_STM_FILE "/sys/bus/coresight/devices/coresight-stm/enable"
#define QDSS_STM_FILE_2 "/sys/bus/coresight/devices/coresight-stm/enable_source"
#define QDSS_HWEVENT_FILE "/sys/bus/coresight/devices/coresight-hwevent/enable"
#define QDSS_STM_HWEVENT_FILE "/sys/bus/coresight/devices/coresight-stm/hwevent_enable"
#define QDSS_HWEVENT_SET_REG_FILE "/sys/bus/coresight/devices/coresight-hwevent/setreg"
#define QDSS_SWAO_CSR_TIMESTAMP "/sys/bus/coresight/devices/coresight-swao-csr/timestamp"
#define QDSS_ATB_FILE "/sys/bus/coresight/devices/coresight-modem-diag/enable_source"

/* QDSS */
typedef PACK(struct)
{
  uint8 cmdCode;        // Diag Message ID
  uint8 subsysId;       // Subsystem ID (DIAG_SUBSYS_QDSS)
  uint16 subsysCmdCode; // Subsystem command code
} qdss_diag_pkt_hdr;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
} qdss_diag_pkt_req;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr; // Header
  uint8 result;          //See QDSS_CMDRESP_... definitions
} qdss_diag_pkt_rsp;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8 trace_sink;
} qdss_trace_sink_req;

typedef qdss_diag_pkt_rsp qdss_trace_sink_rsp; //generic response
typedef qdss_trace_sink_req qdss_etr1_trace_sink_req;
typedef qdss_trace_sink_rsp qdss_etr1_trace_sink_rsp;
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8  state;
} qdss_filter_etm_req;

typedef qdss_diag_pkt_rsp qdss_filter_etm_rsp;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8  state;
} qdss_filter_stm_req;

typedef qdss_diag_pkt_rsp qdss_filter_stm_rsp;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8  state;
} qdss_filter_hwevents_req;

typedef qdss_diag_pkt_rsp qdss_filter_hwevents_rsp;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint32 register_addr;
  uint32 register_value;
} qdss_filter_hwevents_configure_req;

typedef qdss_diag_pkt_rsp qdss_filter_hwevents_configure_rsp;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
} qdss_query_status_req;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8 trace_sink;
  uint8 stm_enabled;
  uint8 hw_events_enabled;
  uint8 atb_enabled;
} qdss_query_status_rsp;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
} qdss_qtimer_ts_sync_req;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint32 status;
  uint64 qdss_ticks;
  uint64 qtimer_ticks;
  uint64 qdss_freq;
  uint64 qtimer_freq;
} qdss_qtimer_ts_sync_rsp;

void image_version_init(void);
void qdss_diag_pkt_hdlr_init(void);
void hw_accel_diag_pkt_hdlr_init(void);
