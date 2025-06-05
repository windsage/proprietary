/********************************************************************
 Copyright (c) 2016, 2021-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#ifndef __MINKSOCKET_H
#define __MINKSOCKET_H

#include "VmOsal.h"
#include "msforwarder.h"
#include "object.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef struct MinkSocket MinkSocket;

typedef enum
{
    UNIX,
    QIPCRTR,
    VSOCK,
    SIMULATED,
} SOCK_TYPE;

#define MAX_SOCKADDR_LEN   108

#define MAX_QRTR_PAYLOAD    (64 * 1024)
#define MAX_VSOCK_PAYLOAD   (64 * 1024)
#define VSOCK_PORT_NO       0x5555

int32_t MinkSocket_attachForwarder(MinkSocket *me, MSForwarder *msFwd);
int32_t MinkSocket_detachForwarder(MinkSocket *me, MSForwarder *msFwd);
int32_t MinkSocket_detachForwarderAll(MinkSocket *me);
void MinkSocket_preDeleteForwarder(MinkSocket *me, MSForwarder *msFwd);
int32_t MinkSocket_populatePeerIdentity(MinkSocket *me, int sock);
int32_t MinkSocket_populatePeerIdentity_remote(MinkSocket *me, uint32_t node, uint32_t port);
int32_t MinkSocket_initObjectTable(MinkSocket *me, Object endpoint, bool workForModule);
MinkSocket *MinkSocket_new(Object endpoint, int32_t sockType, int32_t sock, uint32_t node, uint32_t port);
int32_t process_message(MinkSocket *me, int sock, void *data, size_t data_len);
void MinkSocket_retain(MinkSocket *me);
void MinkSocket_release(MinkSocket *me);
int MinkSocket_detach(MinkSocket *me);
/** MinkSocket_attachObject attempts to attach an object
    with a specified handle much like MinkIPC_Connect does with an endpoint.
 **/
int32_t MinkSocket_attachObject(MinkSocket *me, int handle, Object *obj);
int MinkSocket_detachObject(Object *obj);
int32_t MinkSocket_invoke(MinkSocket *me, int32_t h,
                  ObjectOp op, ObjectArg *args, ObjectCounts k);
int32_t MinkSocket_sendClose(MinkSocket *me, int handle);
void *MinkSocket_dispatch(void *me);
void MinkSocket_start(MinkSocket *me, int sock);
void MinkSocket_close(MinkSocket *me, int32_t err);
void MinkSocket_delete(MinkSocket *me);
bool MinkSocket_isConnected(MinkSocket *me);
void MinkSocket_notifyClose(MinkSocket *me, uint32_t event);
int MinkSocket_initPrimordial(MinkSocket *me, Object pmd);
int MinkSocket_deinitPrimordial(MinkSocket *me);

#if defined (__cplusplus)
}
#endif

#endif //__MINKSOCKET_H
