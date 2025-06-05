/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag HIDL Implementation

GENERAL DESCRIPTION

Diag HIDL service registration.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#ifndef __DIAG_HIDL_SERVICE_H__
#define __DIAG_HIDL_SERVICE_H__

#include <stdint.h>
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif
int register_diag_hidl_service();
#ifdef __cplusplus
}
#endif
#endif
