/********************************************************************
 Copyright (c) 2016-2017,2021-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#ifndef __MINKIPC_H
#define __MINKIPC_H

#include "VmOsal.h"
#include "object.h"

#if defined (__cplusplus)
extern "C" {
#endif

#define OPENER 0
#define MODULE 1

typedef struct MinkIPC MinkIPC;

/*@brief: actual implementation to start Mink service for kinds of scenarios
 *
 *@param[in] address: string of socket file name where service works
             Only apply to sockType of UNIX or SIMULATED. New socket file
             descriptor will be constructed internally according to it.
 *@param[in] sock: constructed socket file descriptor which service works on
 *           Argument of 'address' and 'sock' are exlusive
 *@param[in] sockType: protocol type based on which service works
             Only if service plays the role of hub/xtzd, it can work on sockType
             of SIMULATED, which simulates QIPCRTR/VSOCK interaction with UNIX one
 *@param[in] endpoint: the Mink object to be exposed as service
 *@param[in] endpointType: interface type of exposed service
 *           'OPENER' implements IOpener interface. 'MODULE' implements IModule interface
 *           If the service plays the role of hub/xtzd, it must be 'MODULE' type
 *
 *@return: MinkIPC instance points to started Mink service
*/
MinkIPC *MinkIPC_beginService(const char *address, int32_t sock, int32_t sockType,
                                     Object endpoint, int32_t endpointType);

/*@brief: start a Mink serive to expose functionality of given object.
 *        specified for UNIX protocol and IOpener based object
 *
 *@param[in] address: string of socket file name where service works
 *           Only apply to sockType of UNIX or SIMULATED. New socket file
 *           descriptor will be constructed internally according to it.
 *@param[in] endpoint: the Mink object to be exposed as service
*/
MinkIPC *MinkIPC_startService(const char *address, Object endpoint);

/*@brief: start a Mink serive to expose functionality of given object.
 *        specified for UNIX protocol and IOpener based object
 *
 *@param[in] sock: constructed socket file descriptor which service works on
 *@param[in] endpoint: the Mink object to be exposed as service
*/
MinkIPC *MinkIPC_startServiceOnSocket(int32_t sock, Object endpoint);

/*@brief: start a Mink serive to expose functionality of given object.
 *        specified for UNIX protocol and IModule based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_startServiceModule(const char *address, Object endpoint);

/*@brief: start a Mink serive to expose functionality of given object.
 *        specified for UNIX protocol and IModule based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_startServiceModuleOnSocket(int32_t sock, Object endpoint);

/*@brief: start a Mink serive to expose functionality of given object.
 *        specified for SIMULATED scenario and IModule based object
 *        The interface only applies to object plays the role of hub/xtzd
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_startServiceModule_simulated(const char *address, Object endpoint);

/*@brief: start a Mink serive to expose functionality of given object.
 *        specified for SIMULATED scenario and IModule based object
 *        The interface only applies to object plays the role of hub/xtzd
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_startServiceModuleOnSocket_simulated(int32_t sock, Object endpoint);

/*@brief: start a Mink serive to expose functionality of given object.
 *        specified for VSOCK protocol and IOpener based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_startServiceOnSocket_vsock(int32_t sock, Object endpoint);

/*@brief: start a Mink serive to expose functionality of given object.
 *        specified for VSOCK protocol and IModule based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_startServiceModuleOnSocket_vsock(int32_t sock, Object endpoint);

/*@brief: start a Mink serive to expose functionality of given object.
 *        specified for QIPCRTR protocol and IOpener based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_startServiceOnSocket_QRTR(int32_t sock, Object endpoint);

/*@brief: start a Mink serive to expose functionality of given object.
 *        specified for QIPCRTR protocol and IModule based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_startServiceModuleOnSocket_QRTR(int32_t sock, Object endpoint);

/*@brief: actual implementation to connect target Mink service for kinks of scenarios
 *
 *@param[in] address: string of socket file name or port number of target Mink service
             when sockType is QIPCRTR or VSOCK, it must be string of socket port number
 *@param[out] proxyOut: object output standing for proxy of target service
 *@param[in] sockType: protocol type based on which established interaction works
 *
 *@return MinkIPC instance points to established Mink interaction
*/
MinkIPC* MinkIPC_connect_common(const char *address, Object *proxyOut, int32_t sockType);

/*@brief: connect target Mink Service then interact with output proxy.
 *        specified for UNIX protocol and IOpener based object
 *        IOpener or IModule interface make no difference during connection.
 *        Discriminate them just to match function list of MinkIPC_startServiceXXX()
 *
 *@param[in] address: string of socket file name or port number of target Mink service
 *           when sockType is QIPCRTR or VSOCK, it must be string of socket port number
 *@param[out] proxyOut: object output standing for proxy of target service
*/
MinkIPC *MinkIPC_connect(const char *address, Object *proxyOut);

/*@brief: connect target Mink Service then interact with output proxy.
 *        specified for simulated and IOpener based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_connect_simulated(const char *address, Object *proxyOut);

/*@brief: connect target Mink Service then interact with output proxy.
 *        specified for QIPCRTR protocol and IOpener based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_connect_QRTR(int32_t addrValue, Object *proxyOut);

/*@brief: connect target Mink Service then interact with output proxy.
 *        specified for VSOCK protocol and IOpener based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_connect_vsock(int32_t addrValue, Object *proxyOut);

/*@brief: connect target Mink Service then interact with output proxy.
 *        specified for UNIX protocol and IModule based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_connectModule(const char *address, Object *proxyOut);

/*@brief: connect target Mink Service then interact with output proxy.
 *        specified for simulated and IModule based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_connectModule_simulated(const char *address, Object *proxyOut);

/*@brief: connect target Mink Service then interact with output proxy.
 *        specified for QIPCRTR protocol and IModule based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_connectModule_QRTR(int32_t addrValue, Object *proxyOut);

/*@brief: connect target Mink Service then interact with output proxy.
 *        specified for VSOCK protocol and IModule based object
 *
 *@param[in] the same to annotation above
*/
MinkIPC *MinkIPC_connectModule_vsock(int32_t addrValue, Object *proxyOut);

/** uid: out param - contains client user ID
    pid: out param - contains client group ID
    on success, the function returns 0

    This function is DEPRECATED. The client identity will be inherently
    availalbe to the primordial object in future.
**/
int MinkIPC_getClientInfo(uid_t* uid, gid_t* gid);

/**
   wait for the service to finish ..
   waits until stopped or the service dies
**/
void MinkIPC_join(MinkIPC *me);

/**
   Increment reference count to keep the object live.
**/
void MinkIPC_retain(MinkIPC *me);

/**
   Decrement reference count.
   When the count goes to 0, *me* is deleted.
**/
void MinkIPC_release(MinkIPC *me);

/**
   Wrap an fd into a Object.
   This takes ownership of the Fd.
   The caller must relinquish control
   of the descriptor after calling this method.

   int fd: The fd to wrap
   Object* obj: The Obj that represents the fd
**/
void MinkIPC_wrapFd(int fd, Object *obj);

/**
    register handler to target and return releaser for deregistration

    the registration need to be broadcast to next endpoint if the target
    is a proxy. With the help of static primordial of MinkSocket,
    the function will be invoked recursively until reach the endpoint actually
    implementing the target.
**/
int32_t MinkIPC_registerConnEventHandler(Object target, Object handler,
                                        Object *releaser);

/**
    releaser can work for deregistation because handler wrapped inside

    whenever client loses interest in target or handler has been triggered,
    we need to invoke the function.
**/
static inline void MinkIPC_deregisterConnEventHandler(Object *releaser) {
  return Object_ASSIGN_NULL(*releaser);
}

#if defined (__cplusplus)
}
#endif

#endif //__MINKIPC_H
