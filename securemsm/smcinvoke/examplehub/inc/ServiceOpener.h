/********************************************************************
Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#ifndef __CSERVICEOPENER_H
#define __CSERVICEOPENER_H

#include "object.h"
#include "ServiceManager.h"

/**
 * New IOpener class on top of our registration framework.
 *
 * If a 'remoteOpener' is passed, then this IOpener is assumed to be exposed
 * locally on a socket, and therefore allows for access to both the
 * local IModule registration service and the services exposed by the remote
 * opener.
 * If a remoteOpener is NOT passed, then this IOpener is assumed to be
 * exposed on a QRTR/VSOCK interface, and it therefore only perform
 * resolution of locally registered services.
 *
 * credentials` is the ICredentials object for the VM this opener
 * is exposed to. It can be NULL.
 *
 * NOTE: This implies that a ServiceOpener can be exposed to ONLY one
 * VM/interface at a time.
 * */
Object ServiceOpener_new(ServiceManager *mgr, Object remoteOpener, Object credentials);

#endif // __CSERVICEOPENER_H
