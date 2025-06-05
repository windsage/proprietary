/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag HIDL Implementation

GENERAL DESCRIPTION

Diag HIDL client-->server wrapper implementation.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#ifndef __DIAG_HIDL_WRAPPER_H__
#define __DIAG_HIDL_WRAPPER_H__
#include <stdint.h>
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif

int check_for_diag_system_client_commands(unsigned int key, void * ptr, int len);
int create_diag_hidl_thread();
void diag_update_hidl_client(int type);
void diag_signal_copy_complete(void);
void diag_copy_for_hidl_client(unsigned char *buf, int len, int pid);
void diag_fwd_dci_data(pid_t pid, void *buf, int len);
int check_for_diag_dci_system_client_commands(unsigned int key, void *ptr, int len, int tag);
#ifdef __cplusplus
}
#endif
#endif

