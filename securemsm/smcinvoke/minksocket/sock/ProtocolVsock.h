/********************************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#ifndef __PROTOCOL_VSOCK_H
#define __PROTOCOL_VSOCK_H

#if defined (__cplusplus)
extern "C" {
#endif

#include "VmOsal.h"

#ifndef AF_QMSGQ
#define AF_QMSGQ        27
#define PF_QMSGQ        AF_QMSGQ
#endif

/**
*  @brief: try to construct socket fd of AF_VSOCK protocol. If it
*          fails, try again to construct socket fd of AF_QMSGQ
*          protocol. If it still fails, return error.
*
*  AF_QMSGS is more generic procotol including AF_VSOCK
*/
int32_t ProtocolVsock_constructFd(void);

#if defined (__cplusplus)
}
#endif

#endif //__PROTOCOL_VSOCK_H
