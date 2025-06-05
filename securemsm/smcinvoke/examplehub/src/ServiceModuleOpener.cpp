/********************************************************************
Copyright (c) 2023-2024 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/
#include <openssl/sha.h>

#include "ServiceModuleOpener.h"
#include "CRegisterApp.h"
#include "CRegisterModule_open.h"
#include "ICredentials.h"
#include "IModule.h"
#include "IModule_invoke.h"
#include "MinkHub.h"
#include "MinkTransportUtils.h"
#include "VmOsal.h"
#include "heap.h"
#include "memscpy.h"
#include "minkipc.h"
#include "object.h"
#include "osIndCredentials.h"
#include "vmuuid.h"

#define CBORCredentials 32

static uint32_t temp_appDebug = 1;
static uint32_t temp_appVersion = 2;
static char testDomain[] = "multiplevm";
static char testAppName[] = "testing123";
static uint64_t temp_permissions = 5;
static SHA256Hash appHash_1[SHA256_DIGEST_LENGTH] = {1, 2, 3, 4, 0xAA, 0xFF};
static SHA256Hash appHash_2[SHA256_DIGEST_LENGTH] = {1, 2, 3, 4, 0xAA, 0xFF};
static uint8_t legacyCBOR_1[CBORCredentials] = {1, 2, 3, 4, 0xAA, 0xFF};
static uint8_t legacyCBOR_2[CBORCredentials] = {1, 2, 3, 4, 0xAA, 0xFF};
static SHA256Hash *testHashes[] = {appHash_1, appHash_2};
static uint8_t *testLegacyCBOR[] = {legacyCBOR_1, legacyCBOR_2};

// indicate MinkHub serves for interaction within-VM or across-VM
typedef enum {
    LOCAL,
    REMOTE,
} HubType;

typedef struct {
    int refs;
    HubType hubType;
    ServiceManager *mgr;
    RemoteAddr *remoteHubAddr;
    Object *remoteModuleOpener;
    MinkIPC **remoteConn;
    uint32_t remoteHubNum;
    uint32_t remoteConnNum;
    Object envCred;
    pthread_mutex_t mutex;
} ServiceModuleOpener;

static int32_t ServiceModuleOpener_getCallerHubCred(ServiceModuleOpener *me, Object receivedCred,
                                                    Object hubInitCred, Object *objOut)
{
    int32_t ret = Object_OK;
    uint32_t callerPid = -1;
    size_t callerPid_len = -1;
    uint32_t callerUid = -1;
    size_t callerUid_len = -1;
    size_t callerVMuuid_len = -1;
    uint8_t callerVMuuid[VMUUID_MAX_SIZE] = {0};
    Object callerInfoCred = Object_NULL;
    Object callerEnvCred = Object_NULL;

    if (NULL == me || NULL == objOut) {
        LOG_ERR("invalid parameters\n");
        return Object_ERROR;
    }

    if (LOCAL == me->hubType) {
        if (Object_isNull(hubInitCred)) {
            LOG_ERR("failed to get local envCred\n");
            goto fail;
        }
        ret = ICredentials_getValueByName(receivedCred, "lpid", strlen("lpid"), &callerPid,
                                          sizeof(callerPid), &callerPid_len);
        if (Object_OK != ret) {
            LOG_ERR("get pid form local link cred failed\n");
            goto fail;
        }
        ret = ICredentials_getValueByName(receivedCred, "luid", strlen("luid"), &callerUid,
                                          sizeof(callerUid), &callerUid_len);
        if (Object_OK != ret) {
            LOG_ERR("get uid form local link cred failed\n");
            goto fail;
        }
        ret = OSIndCredentials_newProcessCred(temp_permissions, testAppName, testHashes[0],
                                              temp_appDebug, temp_appVersion, testLegacyCBOR[0],
                                              sizeof(testLegacyCBOR[0]), testDomain, callerPid,
                                              callerUid, &callerInfoCred);

        if (Object_OK != ret) {
            LOG_ERR("OSIndCredentials_newProcessCred failed\n");
            goto fail;
        }

        Object_ASSIGN(callerEnvCred, hubInitCred);

    } else {
        if (Object_isNull(hubInitCred)) {
            ret = ICredentials_getValueByName(receivedCred, "lvmuuid", strlen("lvmuuid"), &callerVMuuid,
                                              sizeof(callerVMuuid) / sizeof(callerVMuuid[0]),
                                              &callerVMuuid_len);
            if (Object_OK != ret) {
                LOG_ERR("query callerVMuuid failed\n");
                goto fail;
            }

            ret = OSIndCredentials_newEnvCred("osID", callerVMuuid, callerVMuuid, "vmDomain", 0,
                                              &callerEnvCred);
            if (Object_OK != ret) {
                LOG_ERR("Credentials_newEnvCred failed\n");
                goto fail;
            }
        } else {
            Object_INIT(callerEnvCred, hubInitCred);
        }
        Object_ASSIGN(callerInfoCred, receivedCred);
    }

    ret = OSIndCredentials_WrapCredentials(&callerInfoCred, &callerEnvCred, objOut);
    if (Object_OK != ret) {
        LOG_ERR("OSIndCredentials_WrapCredentials failed\n");
        goto fail;
    }

fail:
    if (!Object_isNull(callerInfoCred)) {
        Object_ASSIGN_NULL(callerInfoCred);
    }
    if (!Object_isNull(callerEnvCred)) {
        Object_ASSIGN_NULL(callerEnvCred);
    }

    return ret;
}

static int32_t ServiceModuleOpener_refreshRemoteConn(ServiceModuleOpener *me)
{
    int32_t ret = Object_OK;

    if (NULL == me) {
        LOG_ERR("invalid parameters\n");
        return Object_ERROR;
    }

    pthread_mutex_lock(&me->mutex);
    if (0 != me->remoteHubNum && me->remoteConnNum != me->remoteHubNum) {
        for (int i = 0; i < me->remoteHubNum; i++) {
            if (NULL == me->remoteConn[i]) {
                me->remoteConn[i] =
                    MinkIPC_connect_common(me->remoteHubAddr[i].addr, &me->remoteModuleOpener[i],
                                           me->remoteHubAddr[i].sockType);
                if (NULL == me->remoteConn[i] || Object_isNull(me->remoteModuleOpener[i])) {
                    LOG_ERR("connect address %s of sockType %d failed\n", me->remoteHubAddr[i].addr,
                            me->remoteHubAddr[i].sockType);
                    me->remoteConn[i] = NULL;
                } else {
                    me->remoteConnNum++;
                }
            }
        }
    }
    pthread_mutex_unlock(&me->mutex);

    if (0 == me->remoteConnNum) {
        LOG_ERR("no remote connection available\n");
        ret = Object_ERROR_UNAVAIL;
    }

    return ret;
}

static int32_t ServiceModuleOpener_shutdown(ServiceModuleOpener *me)
{
    return Object_OK;
}

static int32_t ServiceModuleOpener_retain(ServiceModuleOpener *me)
{
    vm_osal_atomic_add(&me->refs, 1);

    return Object_OK;
}

static int32_t ServiceModuleOpener_release(ServiceModuleOpener *me)
{
    if (vm_osal_atomic_add(&me->refs, -1) == 0) {
        HEAP_FREE_PTR(me->remoteHubAddr);
        for (int i = 0; i < me->remoteHubNum; i++) {
            Object_ASSIGN_NULL(me->remoteModuleOpener[i]);
            if (me->remoteConn[i]) {
                MinkIPC_release(me->remoteConn[i]);
            }
        }
        Object_ASSIGN_NULL(me->envCred);
        pthread_mutex_destroy(&me->mutex);
        HEAP_FREE_PTR(me);
    }

    return Object_OK;
}

/******************************************************************************
1.contrust CRegisterModule if requested id is CRegisterApp_UID
2.seach local ServiceManager for module with requested id
3.only when working for interactions within-VM, the ServiceManagerOpener iterates
and searches all remote ServiceManagers for module with requested id
Of course, connection to remote ServiceManagers have to be established at first
*******************************************************************************/
static int32_t ServiceModuleOpener_open(ServiceModuleOpener *me, uint32_t id, Object credentials,
                                        Object *objOut)
{
    int32_t ret = Object_OK;
    Object callerHubCred = Object_NULL;

    if (id == CRegisterApp_UID) {
        if (LOCAL == me->hubType) {
            return CRegisterModule_open(me->mgr, objOut);
        } else {
            LOG_ERR("service registration is NOT support across-VM\n");
            return Object_ERROR_REMOTE;
        }
    }

    ret = ServiceModuleOpener_getCallerHubCred(me, credentials, me->envCred, &callerHubCred);
    if (Object_OK != ret) {
        LOG_ERR("ServiceModuleOpener_getCallerHubCred failed\n");
        goto cleanup;
    }

    ret = ServiceManager_getService(me->mgr, id, callerHubCred, objOut);

    if (Object_OK != ret && LOCAL == me->hubType) {
        ret = ServiceModuleOpener_refreshRemoteConn(me);
        if (Object_OK != ret) {
            LOG_ERR("ServiceModuleOpener_refreshRemoteConn failed\n");
            goto cleanup;
        }

        for (int i = 0; i < me->remoteHubNum; i++) {
            if (NULL != me->remoteConn[i]) {
                ret = IModule_open(me->remoteModuleOpener[i], id, callerHubCred, objOut);
                if (Object_OK == ret) {
                    goto cleanup;
                }
                if (Object_ERROR_UNAVAIL == ret || Object_ERROR_DEFUNCT == ret) {
                    LOG_ERR("connection to remote hub[%d] down. Refresh next time\n", i);
                    me->remoteConnNum--;
                    me->remoteConn[i] = NULL;
                }
            }
        }
    }

cleanup:
    if (!Object_isNull(callerHubCred)) {
        Object_ASSIGN_NULL(callerHubCred);
    }

    return ret;
}

static IModule_DEFINE_INVOKE(ServiceModuleOpener_invoke, ServiceModuleOpener_,
                             ServiceModuleOpener *);

/****************************************************************************************
 *@brief: construct the ServiceModuleOpener instance listening to within-VM insteraction.
 *        It is the request dispatcher and service extractor behind MinkHub.
 *
 *@param[in] mgr: Service manager of this hub
 *@param[in] envCred: Credential of local VM
 *@param[in] remoteHubAddr: Address of remote hubs to be connected
 *@param[in] remoteHubNum: Number of remote hubs to be connected
 *@param[out] objOut: Constructed ServiceModuleOpener instance
*****************************************************************************************/
int32_t ServiceModuleOpenerLocal_new(ServiceManager *mgr, Object envCred, RemoteAddr *remoteHubAddr,
                                     uint32_t remoteHubNum, Object *objOut)
{
    ServiceModuleOpener *me = NULL;
    int ret = Object_OK;

    me = HEAP_ZALLOC_TYPE(ServiceModuleOpener);
    if (!me) {
        LOG_ERR("allocate ServiceModuleOpener failed\n");
        return Object_ERROR_MEM;
    }

    if (NULL != remoteHubAddr && 0 != remoteHubNum) {
        me->remoteHubAddr = HEAP_ZALLOC_ARRAY(RemoteAddr, remoteHubNum);
        if (!me->remoteHubAddr) {
            LOG_ERR("allocate remoteHubAddr failed\n");
            ret = Object_ERROR_MEM;
            goto bail;
        }
        memset(me->remoteHubAddr, 0, sizeof(RemoteAddr) * remoteHubNum);
        memscpy(me->remoteHubAddr, sizeof(RemoteAddr) * remoteHubNum, remoteHubAddr,
                sizeof(RemoteAddr) * remoteHubNum);

        me->remoteModuleOpener = HEAP_ZALLOC_ARRAY(Object, remoteHubNum);
        if (!me->remoteModuleOpener) {
            LOG_ERR("allocate remoteModuleOpener failed\n");
            ret = Object_ERROR_MEM;
            goto bail;
        }
        memset(me->remoteModuleOpener, 0, sizeof(Object) * remoteHubNum);

        me->remoteConn = HEAP_ZALLOC_ARRAY(MinkIPC *, remoteHubNum);
        if (!me->remoteConn) {
            LOG_ERR("allocate remoteConn failed\n");
            ret = Object_ERROR_MEM;
            goto bail;
        }
        memset(me->remoteConn, 0, sizeof(MinkIPC *) * remoteHubNum);
    }

    me->refs = 1;
    me->hubType = LOCAL;
    me->mgr = mgr;
    me->remoteHubNum = remoteHubNum;
    Object_INIT(me->envCred, envCred);
    pthread_mutex_init(&me->mutex, NULL);
    *objOut = (Object){ServiceModuleOpener_invoke, me};
    return ret;

bail:
    if (me->remoteConn) {
        HEAP_FREE_PTR(me->remoteConn);
    }
    if (me->remoteModuleOpener) {
        HEAP_FREE_PTR(me->remoteModuleOpener);
    }
    if (me->remoteHubAddr) {
        HEAP_FREE_PTR(me->remoteHubAddr);
    }
    if (me) {
        HEAP_FREE_PTR(me);
    }
    return ret;
}

/****************************************************************************************
 *@brief: Construct the ServiceModuleOpener instance listening to across-VM insteraction.
 *        It is the request dispatcher and service extractor behind MinkHub.
 *
 *@param[in] mgr: Service manager of this hub
 *@param[in] envCred: Credentials of VM the MinkHub listening to.
 *@param[out] objOut: Constructed ServiceModuleOpener instance
*****************************************************************************************/
int32_t ServiceModuleOpenerRemote_new(ServiceManager *mgr, Object envCred, Object *objOut)
{
    ServiceModuleOpener *me = NULL;

    me = HEAP_ZALLOC_TYPE(ServiceModuleOpener);
    if (!me) {
        LOG_ERR("allocate ServiceModuleOpener failed\n");
        return Object_ERROR_MEM;
    }

    me->refs = 1;
    me->hubType = REMOTE;
    me->mgr = mgr;
    Object_INIT(me->envCred, envCred);
    pthread_mutex_init(&me->mutex, NULL);
    *objOut = (Object){ServiceModuleOpener_invoke, me};

    return Object_OK;
}
