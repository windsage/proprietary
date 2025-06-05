/********************************************************************
Copyright (c) 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/
#include "MinkHub.h"
#include "IModule.h"
#include "IModule_invoke.h"
#include "MinkTransportUtils.h"
#include "ProtocolVsock.h"
#include "ServiceManager.h"
#include "ServiceOpener.h"
#include "VmOsal.h"
#include "heap.h"
#include "minksocket.h"

#ifndef OFFTARGET
#include <linux/vm_sockets.h>
#include "libqrtr.h"
#define MINK_QRTR_VERSION 1
#endif  // OFFTARGET

#ifndef OFFTARGET
static const char *files[] = {"/sys/power/wake_lock",
                              "/sys/power/wake_unlock",
                              "/sys/kernel/vm_set_status/app_status"};

int32_t MinkHub_wakelockHelper(uint op, const char *content)
{
    if (content == NULL) {
        LOG_ERR("Received invalid args: op=%d content=NULL\n", op);
        return -1;
    }

    if (op > OP_STATUS_UPDATE) {
        LOG_ERR("Received invalid args: op=%d content=%s\n", op, content);
        return -1;
    }

    const char *file = files[op];
    int fd = open(file, O_WRONLY | O_APPEND);
    if (fd < 0) {
        LOG_ERR("Error %s opening file %s\n", strerror(errno), file);
        return -1;
    }

    if (write(fd, content, strlen(content)) < 0) {
        LOG_ERR("Error %s writing to file\n", strerror(errno));
        close(fd);
        return -1;
    }

    LOG_MSG("%s file written successfully\n", file);
    return 0;
}
#else
int32_t MinkHub_wakelockHelper(uint op, const char *content)
{
    return 0;
}
#endif

int32_t MinkUNIX_constructSockfd(const char *addr, const char *laEnv, int *fdOut)
{
    int32_t ret = Object_OK;
    char *env_sock = NULL;
    struct sockaddr_un file;

    if (NULL != laEnv) {
        env_sock = getenv(laEnv);
        if (env_sock) {
            LOG_ERR("sockfd LA env of %s existed\n", laEnv);

            errno = 0;
            *fdOut = (int)strtol(env_sock, NULL, 10);
            if (errno) {
                LOG_ERR("strtol for sockfd LA env of %s returned error %d\n", laEnv, errno);
                return Object_ERROR;
            }
            return Object_OK;
        }
    }

    memset(&file, 0, sizeof(file));
    file.sun_family = AF_UNIX;
    snprintf(file.sun_path, sizeof(file.sun_path) - 1, "%s", addr);

    ret = socket(AF_UNIX, SOCK_STREAM, 0);
    if (ret < 0) {
        LOG_ERR("fail to constuct sockfd for sock addess of %s\n", addr);
        return Object_ERROR;
    }
    *fdOut = ret;

    unlink(addr);
    ret = bind(*fdOut, (struct sockaddr *)&file, sizeof(file));
    if (ret) {
        LOG_ERR("fail to bind sockfd, errno %d\n", errno);
        close(*fdOut);
        *fdOut = -1;
        return Object_ERROR;
    }

    return Object_OK;
}

#ifndef OFFTARGET
int32_t MinkQRTR_constructSockfd(const char *port, const char *instanceStr, int *fdOut)
{
    int32_t ret = Object_OK;
    int32_t instance = -1;

    if (NULL == instanceStr) {
        instance = 1;
    } else {
        instance = atoi(instanceStr);
    }

    ret = qrtr_open(0);
    if (ret < 0) {
        LOG_ERR("fail to qrtr_open with error %d\n", ret);
        return ret;
    }

    *fdOut = ret;
    ret = qrtr_publish(*fdOut, atoi(port), MINK_QRTR_VERSION, instance);
    if (ret) {
        LOG_ERR("qrtr_publish failed with error %d\n", ret);
        close(*fdOut);
        *fdOut = -1;
        return ret;
    }

    LOG_MSG("succeed to construct QRTR sockfd %d\n", *fdOut);
    return ret;
}

int32_t MinkVSOCK_constructSockfd(const char *port, int *fdOut)
{
    int32_t ret = Object_OK;
    struct sockaddr_vm svm;

    ret = ProtocolVsock_constructFd();
    if (ret < 0) {
        LOG_ERR("fail to constuct VSOCK sockfd with ret %d\n", ret);
        return ret;
    }

    *fdOut = ret;

    svm = {
        .svm_family = AF_VSOCK, .svm_port = atoi(port), .svm_cid = VMADDR_CID_ANY,
    };

    ret = bind(*fdOut, (struct sockaddr *)&svm, sizeof(svm));
    if (ret) {
        LOG_ERR("fail to bind VSOCK sockfd for port %s with ret %d, errno: %d :%s\n", port, ret,
                errno, strerror(errno));
        close(*fdOut);
        *fdOut = -1;
        return ret;
    }

    LOG_MSG("succeed to construct VSOCK sockfd %d\n", *fdOut);
    return ret;
}

#else
int32_t MinkQRTR_constructSockfd(const char *port, const char *instanceStr, int *fdOut)
{
    *fdOut = -1;
    return Object_ERROR;
}

int32_t MinkVSOCK_constructSockfd(const char *port, int *fdOut)
{
    *fdOut = -1;
    return Object_ERROR;
}
#endif

int32_t MinkHub_constructSockfd(const char *sockAddr, const char *extInfo, int32_t sockType,
                                int *fdOut)
{
    int32_t ret = Object_OK;

    if (NULL == sockAddr || NULL == fdOut) {
        LOG_ERR("invalid input arguments\n");
        return Object_ERROR;
    }

    if (UNIX == sockType || SIMULATED == sockType) {
        ret = MinkUNIX_constructSockfd(sockAddr, extInfo, fdOut);
    } else if (QIPCRTR == sockType) {
        ret = MinkQRTR_constructSockfd(sockAddr, extInfo, fdOut);
    } else {
        ret = MinkVSOCK_constructSockfd(sockAddr, fdOut);
    }
    if (Object_OK != ret) {
        LOG_ERR("fail to construct sockfd with error %d\n", ret);
    }

    return ret;
}

MinkIPC *MinkHub_startService_common(ServiceManager *mgr, int32_t endpointType, int32_t sockType,
                                     const char *hubAddr, const char *extInfo, Object hubCred,
                                     RemoteAddr *remoteHubAddr, uint32_t remoteHubNum)
{
    int ret = Object_ERROR;
    int sockfd = 0;
    Object opener = Object_NULL;
    MinkIPC *conn = NULL;

    ret = MinkHub_constructSockfd(hubAddr, extInfo, sockType, &sockfd);
    if (Object_OK != ret) {
        LOG_ERR("fail to constuct sockfd for addr %s of sockType %d, error %d\n", hubAddr, sockType,
                ret);
        goto fail;
    }

    if (OPENER == endpointType) {
        opener = ServiceOpener_new(mgr, Object_NULL, hubCred);
        if (Object_isNull(opener)) {
            LOG_ERR("ServiceOpener_new failed\n");
            ret = Object_ERROR;
            goto fail;
        }
    } else {
        if (QIPCRTR == sockType || VSOCK == sockType || SIMULATED == sockType) {
            ret = ServiceModuleOpenerRemote_new(mgr, hubCred, &opener);
        } else {
            ret = ServiceModuleOpenerLocal_new(mgr, hubCred, remoteHubAddr, remoteHubNum, &opener);
        }
        if (Object_OK != ret) {
            LOG_ERR("ServiceModuleOpener_new failed with error %d\n", ret);
            goto fail;
        }
    }

    conn = MinkIPC_beginService(NULL, sockfd, sockType, opener, endpointType);
    if (NULL == conn) {
        LOG_ERR("fail to beginService for address %s of sockType %d\n", hubAddr, sockType);
        goto fail;
    }

fail:
    if (Object_OK != ret && sockfd > 0) {
        close(sockfd);
    }
    Object_ASSIGN_NULL(opener);

    return conn;
}

MinkIPC *MinkHub_startService_UNIX(ServiceManager *mgr, const char *hubAddr, const char *extInfo,
                                   Object hubCred, RemoteAddr *remoteHubAddr, uint32_t remoteHubNum)
{
    return MinkHub_startService_common(mgr, OPENER, UNIX, hubAddr, extInfo, hubCred, remoteHubAddr,
                                       remoteHubNum);
}

MinkIPC *MinkHub_startServiceModule_UNIX(ServiceManager *mgr, const char *hubAddr,
                                         const char *extInfo, Object hubCred,
                                         RemoteAddr *remoteHubAddr, uint32_t remoteHubNum)
{
    return MinkHub_startService_common(mgr, MODULE, UNIX, hubAddr, extInfo, hubCred, remoteHubAddr,
                                       remoteHubNum);
}

MinkIPC *MinkHub_startService_simulated(ServiceManager *mgr, const char *hubAddr,
                                        const char *extInfo, Object hubCred)
{
    return MinkHub_startService_common(mgr, OPENER, SIMULATED, hubAddr, extInfo, hubCred, NULL, 0);
}

MinkIPC *MinkHub_startServiceModule_simulated(ServiceManager *mgr, const char *hubAddr,
                                              const char *extInfo, Object hubCred)
{
    return MinkHub_startService_common(mgr, MODULE, SIMULATED, hubAddr, extInfo, hubCred, NULL, 0);
}

MinkIPC *MinkHub_startService_QRTR(ServiceManager *mgr, const char *hubAddr, const char *extInfo,
                                   Object hubCred)
{
    return MinkHub_startService_common(mgr, OPENER, QIPCRTR, hubAddr, extInfo, hubCred, NULL, 0);
}

MinkIPC *MinkHub_startServiceModule_QRTR(ServiceManager *mgr, const char *hubAddr,
                                         const char *extInfo, Object hubCred)
{
    return MinkHub_startService_common(mgr, MODULE, QIPCRTR, hubAddr, extInfo, hubCred, NULL, 0);
}

MinkIPC *MinkHub_startService_VSOCK(ServiceManager *mgr, const char *hubAddr, const char *extInfo,
                                    Object hubCred)
{
    return MinkHub_startService_common(mgr, OPENER, VSOCK, hubAddr, extInfo, hubCred, NULL, 0);
}

MinkIPC *MinkHub_startServiceModule_VSOCK(ServiceManager *mgr, const char *hubAddr,
                                          const char *extInfo, Object hubCred)
{
    return MinkHub_startService_common(mgr, MODULE, VSOCK, hubAddr, extInfo, hubCred, NULL, 0);
}
