/********************************************************************
Copyright (c) 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/
#ifndef __MINKHUB_H
#define __MINKHUB_H

#include "ServiceManager.h"
#include "ServiceModuleOpener.h"
#include "ServiceOpener.h"
#include "minkipc.h"
#include "object.h"

#define OP_ACQUIRE_LOCK   0
#define OP_RELEASE_LOCK   1
#define OP_STATUS_UPDATE  2
#define STATUS_READY     "1"

int32_t MinkHub_wakelockHelper(uint op, const char *content);
int32_t MinkUNIX_constructSockfd(const char *addr, const char *laEnv, int *fdOut);
int32_t MinkQRTR_constructSockfd(const char *port, const char *instanceStr, int *fdOut);
int32_t MinkVSOCK_constructSockfd(const char *port, int *fdOut);

MinkIPC *MinkHub_startService_UNIX(ServiceManager *mgr, const char *hubAddr, const char *extInfo,
                                   Object hubCred, RemoteAddr *remoteHubAddr,
                                   uint32_t remoteHubNum);

MinkIPC *MinkHub_startServiceModule_UNIX(ServiceManager *mgr, const char *hubAddr,
                                         const char *extInfo, Object hubCred,
                                         RemoteAddr *remoteHubAddr, uint32_t remoteHubNum);

MinkIPC *MinkHub_startService_simulated(ServiceManager *mgr, const char *hubAddr,
                                        const char *extInfo, Object hubCred);

MinkIPC *MinkHub_startServiceModule_simulated(ServiceManager *mgr, const char *hubAddr,
                                              const char *extInfo, Object hubCred);

MinkIPC *MinkHub_startService_QRTR(ServiceManager *mgr, const char *hubAddr, const char *extInfo,
                                   Object hubCred);

MinkIPC *MinkHub_startServiceModule_QRTR(ServiceManager *mgr, const char *hubAddr,
                                         const char *extInfo, Object hubCred);

MinkIPC *MinkHub_startService_VSOCK(ServiceManager *mgr, const char *hubAddr, const char *extInfo,
                                    Object hubCred);

MinkIPC *MinkHub_startServiceModule_VSOCK(ServiceManager *mgr, const char *hubAddr,
                                          const char *extInfo, Object hubCred);

#endif  // __MINKHUB_H