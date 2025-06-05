/********************************************************************
Copyright (c) 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/
#include "check.h"
#include "MinkHub.h"
#include "MinkTransportUtils.h"
#include "ServiceManager.h"
#include "ServiceModuleOpener.h"
#include "ServiceOpener.h"
#include "VmOsal.h"
#include "heap.h"
#include "minkipc.h"
#include "object.h"
#include "osIndCredentials.h"
#include "vmuuid.h"

#define MAX_SERVICE 10

#define EXAMPLEHUB_STR "exampleHub"

#define EXAMPLEHUB_SOCKNAME  "exampleHub_socket"

#ifndef OFFTARGET
#define EXAMPLEHUB_PVM_SOCKADDR_UNIX      "/dev/socket/exampleHub_PVM"
#define EXAMPLEHUB_PVM_SOCKADDR_QRTR      "5100"
#define EXAMPLEHUB_QTVM_SOCKADDR_UNIX     "/dev/socket/exampleHub_QTVM"
#define EXAMPLEHUB_QTVM_SOCKADDR_QRTR     "5102"
#define EXAMPLEHUB_QTVM_SOCKARRD_VSOCK    "21850"
#define EXAMPLEHUB_OEMVM_SOCKADDR_UNIX    "/dev/socket/exampleHub_OEMVM"
#define EXAMPLEHUB_OEMVM_SOCKADDR_VSOCK   "21860"
#else
#define EXAMPLEHUB_PVM_SOCKADDR_UNIX      "exampleHub_PVM_sockUnix"
#define EXAMPLEHUB_PVM_SOCKADDR_QRTR      "exampleHub_PVM_sockQRTR"
#define EXAMPLEHUB_QTVM_SOCKADDR_UNIX     "exampleHub_QTVM_sockUnix"
#define EXAMPLEHUB_QTVM_SOCKADDR_QRTR     "exampleHub_QTVM_sockQRTR"
#define EXAMPLEHUB_QTVM_SOCKARRD_VSOCK    "exampleHub_QTVM_sockVSOCK"
#define EXAMPLEHUB_OEMVM_SOCKADDR_UNIX    "exampleHub_OEMVM_sockUnix"
#define EXAMPLEHUB_OEMVM_SOCKADDR_VSOCK   "exampleHub_OEMVM_sockVsock"
#endif

#define CHECK_CLEAN(expr) \
  do { if (!(expr)) { CHECK_LOG(); goto cleanup; } } while (0)

typedef enum {
    PVM,
    QTVM,
    OEMVM,
} VM_TYPE;

static uint8_t pvmUuid[] = {CLIENT_VMUID_HLOS};
static uint8_t qtvmUuid[] = {CLIENT_VMUID_TUI};
static uint8_t oemvmUuid[] = {CLIENT_VMUID_OEM};

/*@usage: exampleHub -pvm/-qtvm/oemvm -offtarget/-ontarget
*
* exampleHub is based on IModule while exampleHubLegace is based on IOpener,
* which makes ONLY exampleHub support and request following features:
* 1.exampleHub depends on and enables LinkCred by default.
* 2.exampleHub forces indirect interaction of client--hub--service
*/
int main(int argc, char **argv)
{
    int32_t ret = -1;
    int32_t vmType = 0;
    bool isSimulated = false;
    uint8_t *uuidLocal = NULL;
    uint8_t *uuidCounter = NULL;
    uint8_t *uuidAside = NULL;
    Object envCredLocal = Object_NULL;
    Object envCredCounter = Object_NULL;
    Object envCredAside = Object_NULL;
    char addrForLocal[MAX_SOCKADDR_LEN] = {0};
    char sockNameForLocal[MAX_SOCKADDR_LEN] = {0};
    char addrForCounter[MAX_SOCKADDR_LEN] = {0};
    char addrForAside[MAX_SOCKADDR_LEN] = {0};
    MinkIPC *connIntern = NULL;
    MinkIPC *connExtern = NULL;
    MinkIPC *connAside = NULL;
    int32_t sockTypeRemote[2] = {-1, -1};
    RemoteAddr addrRemote[2] = {0};
    ServiceManager *mgr = NULL;

    if (argc < 3) {
        LOG_ERR("error arguments! \
                usage example: bin -pvm/qtvm/oemvm -offtarget/ontarget\n");
        return ret;
    }

    if (strncmp(argv[1], "-pvm", strlen("-pvm")) == 0) {
        vmType = PVM;
        sockTypeRemote[0] = QIPCRTR;
    } else if (strncmp(argv[1], "-qtvm", strlen("-qtvm")) == 0) {
        vmType = QTVM;
        sockTypeRemote[0] = QIPCRTR;
        sockTypeRemote[1] = VSOCK;
    } else if (strncmp(argv[1], "-oemvm", strlen("-oemvm")) == 0) {
        vmType = OEMVM;
        sockTypeRemote[0] = VSOCK;
    } else {
        LOG_ERR("error, unknown VM environment\n");
        return ret;
    }

    if (strncmp(argv[2], "-offtarget", strlen("-offtarget")) == 0) {
        isSimulated = true;
        sockTypeRemote[0] = SIMULATED;
        sockTypeRemote[1] = SIMULATED;
    } else if (strncmp(argv[2], "-ontarget", strlen("-ontarget")) == 0) {
        isSimulated = false;
    } else {
        LOG_ERR("error, only support offtarget or ontarget\n");
        return ret;
    }

    if(MinkHub_wakelockHelper(OP_ACQUIRE_LOCK, EXAMPLEHUB_STR) < 0) {
        LOG_ERR("failed to acquire wakelock\n");
    };

    if (vmType == PVM) {
        uuidLocal = pvmUuid;
        uuidCounter = qtvmUuid;
        snprintf(addrForLocal, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_PVM_SOCKADDR_UNIX);
        snprintf(sockNameForLocal, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_SOCKNAME);
        snprintf(addrForCounter, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_PVM_SOCKADDR_QRTR);
        snprintf(addrRemote[0].addr, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_QTVM_SOCKADDR_QRTR);
        addrRemote[0].sockType = sockTypeRemote[0];
    } else if (vmType == QTVM) {
        uuidLocal = qtvmUuid;
        uuidCounter = pvmUuid;
        uuidAside = oemvmUuid;
        snprintf(addrForLocal, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_QTVM_SOCKADDR_UNIX);
        snprintf(sockNameForLocal, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_SOCKNAME);
        snprintf(addrForCounter, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_QTVM_SOCKADDR_QRTR);
        snprintf(addrForAside, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_QTVM_SOCKARRD_VSOCK);
        snprintf(addrRemote[0].addr, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_PVM_SOCKADDR_QRTR);
        addrRemote[0].sockType = sockTypeRemote[0];
        snprintf(addrRemote[1].addr, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_OEMVM_SOCKADDR_VSOCK);
        addrRemote[1].sockType = sockTypeRemote[1];
    } else {
        uuidLocal = oemvmUuid;
        uuidCounter = qtvmUuid;
        snprintf(addrForLocal, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_OEMVM_SOCKADDR_UNIX);
        snprintf(sockNameForLocal, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_SOCKNAME);
        snprintf(addrForCounter, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_OEMVM_SOCKADDR_VSOCK);
        snprintf(addrRemote[0].addr, MAX_SOCKADDR_LEN, "%s", EXAMPLEHUB_QTVM_SOCKARRD_VSOCK);
        addrRemote[0].sockType = sockTypeRemote[0];
    }

    TRUE_OR_CLEAN(!OSIndCredentials_newEnvCred("osID", uuidLocal, NULL, "vmDomain",
                  0, &envCredLocal),
                  "OSIndCredentials_newEnvCred failed for uuidLocal\n");
    TRUE_OR_CLEAN(!OSIndCredentials_newEnvCred("osID", uuidCounter, NULL, "vmDomain",
                  0, &envCredCounter),
                  "OSIndCredentials_newEnvCred failed for uuidCounter\n");

    if (vmType == QTVM) {
        TRUE_OR_CLEAN(!OSIndCredentials_newEnvCred("osID", uuidAside, NULL, "vmDomain",
                      0, &envCredAside),
                      "OSIndCredentials_newEnvCred failed for uuidCounter\n");
    }

    mgr = ServiceManager_new(MAX_SERVICE);
    if (!mgr) {
        LOG_ERR("ServiceManager_new failed\n");
        ret = Object_ERROR_MEM;
        goto cleanup;
    }

    if (isSimulated) {
        connExtern = MinkHub_startServiceModule_simulated(mgr, addrForCounter, NULL, envCredCounter);
        if(vmType == QTVM) {
            connAside = MinkHub_startServiceModule_simulated(mgr, addrForAside, NULL, envCredAside);
        }
    } else {
        if (vmType == PVM) {
            connExtern = MinkHub_startServiceModule_QRTR(mgr, addrForCounter, NULL, envCredCounter);
        } else if(vmType == QTVM) {
            connExtern = MinkHub_startServiceModule_QRTR(mgr, addrForCounter, NULL, envCredCounter);
            connAside = MinkHub_startServiceModule_VSOCK(mgr, addrForAside, NULL, envCredAside);
        } else {
            connExtern = MinkHub_startServiceModule_VSOCK(mgr, addrForCounter, NULL, envCredCounter);
        }
    }

    if (!connExtern) {
        LOG_ERR("failed to start service on addr: %s\n", addrForCounter);
        goto cleanup;
    }
    if (vmType == QTVM && !connAside) {
        LOG_ERR("failed to start service on addr: %s\n", addrForAside);
        goto cleanup;
    }

    connIntern = MinkHub_startServiceModule_UNIX(mgr, addrForLocal, sockNameForLocal, envCredLocal,
                                              addrRemote, sizeof(addrRemote) / sizeof(RemoteAddr));
    if (connIntern) {
        LOG_MSG("Serving connIntern interface\n");
        MinkHub_wakelockHelper(OP_STATUS_UPDATE, STATUS_READY);
        if (MinkHub_wakelockHelper(OP_RELEASE_LOCK, EXAMPLEHUB_STR) < 0) {
            LOG_ERR("Could not release wakelock\n");
        }
        MinkIPC_join(connIntern);
    } else {
        LOG_ERR("Failed to create connIntern\n");
    }

cleanup:
    if (MinkHub_wakelockHelper(OP_RELEASE_LOCK, EXAMPLEHUB_STR) < 0) {
        LOG_ERR("Could not release wakelock\n");
    }

    Object_ASSIGN_NULL(envCredLocal);
    Object_ASSIGN_NULL(envCredCounter);
    Object_ASSIGN_NULL(envCredAside);
    if (connIntern) {
        MinkIPC_release(connIntern);
    }
    if (connExtern) {
        MinkIPC_release(connExtern);
    }
    if (connAside) {
        MinkIPC_release(connAside);
    }
    if (mgr) {
        ServiceManager_delete(mgr);
    }
    return 0;
}
