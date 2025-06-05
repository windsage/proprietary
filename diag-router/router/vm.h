/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Internal Header file for vm diag support

GENERAL DESCRIPTION

Declaration of API's used to communicate with vm diag.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef __VM_H__
#define __VM_H__

#include "diag.h"
#include "peripheral.h"
#include "diag_cntl.h"

void diag_vm_send_feature_mask(struct peripheral *peripheral);
int diag_vm_query_diag_id(struct peripheral *peripheral, const char* process_name);
int diag_vm_process_msg_mask(struct peripheral *peripheral, struct diag_cntl_hdr *hdr, size_t len);
int diag_vm_process_event_mask(struct peripheral *peripheral, struct diag_cntl_hdr *hdr, size_t len);
int diag_vm_process_log_mask(struct peripheral *peripheral, struct diag_cntl_hdr *hdr, size_t len);
int diag_vm_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag);
void diag_vm_process_pkt_format_request(struct peripheral *peripheral, struct diag_cntl_hdr *hdr, size_t len);
#endif
