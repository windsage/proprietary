/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag MHI communication support

GENERAL DESCRIPTION

Implementation of communication layer between diag and MHI driver.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#ifndef __DIAG_MHI_H__
#define __DIAG_MHI_H__
int diag_mhi_init(void);
int diag_mhi_write(void *ptr, int len, int remote_token);
int diag_get_mhi_remote_mask(void);
#endif
