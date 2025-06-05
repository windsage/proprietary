/********************************************************************
Copyright (c) 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#ifndef __SERVIECEMODULEOPENER_H
#define __SERVIECEMODULEOPENER_H

#include "ServiceManager.h"
#include "minksocket.h"
#include "object.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    char addr[MAX_SOCKADDR_LEN];
    int32_t sockType;
} RemoteAddr;

int32_t ServiceModuleOpenerLocal_new(ServiceManager *mgr, Object envCred, RemoteAddr *remoteHubAddr,
                                     uint32_t remoteHubNum, Object *objOut);
int32_t ServiceModuleOpenerRemote_new(ServiceManager *mgr, Object envCred, Object *objOut);

#if defined(__cplusplus)
}
#endif

#endif  // __SERVIECEMODULEOPENER_H
