/********************************************************************
Copyright (c) 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/
#include "check.h"
#include "MinkHub.h"
#include "MinkTransportUtils.h"
#include "ServiceManager.h"
#include "ServiceOpener.h"
#include "VMCredentials.h"
#include "VmOsal.h"
#include "heap.h"
#include "minkipc.h"
#include "object.h"
#include "osIndCredentials.h"
#include "vmuuid.h"

#define MAX_SERVICE 10

#define EXAMPLEHUB_STR "exampleLegacyHub"

#define EXAMPLEHUB_SOCKNAME  "exampleHubLegacy_socket"

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

typedef enum {
    PVM,
    QTVM,
    OEMVM,
} VM_TYPE;

static uint8_t pvmUuid[] = {CLIENT_VMUID_HLOS};
static uint8_t qtvmUuid[] = {CLIENT_VMUID_TUI};
static uint8_t oemvmUuid[] = {CLIENT_VMUID_OEM};

/*@usage: exampleHub -offtarget/-ontarget
*
* exampleHubLegacy is obsoleted and doesn't support much features.
* It is reserved just to check backwards compatibility on deprecated
* scenarios of direct interaction of client--service.
*/
int main(int argc, char **argv)
{
    int32_t ret = -1;
    int32_t uuidLen = sizeof(qtvmUuid)/sizeof(qtvmUuid[0]);
    bool isSimulated = false;
    Object credQtvm = Object_NULL;
    Object credHlos = Object_NULL;
    Object credOemvm = Object_NULL;
    ServiceManager *mgr = NULL;
    MinkIPC *connIntern = NULL;
    MinkIPC *connHlos = NULL;
    MinkIPC *connOemvm = NULL;

    if (argc < 2) {
        LOG_ERR("error arguments! usage example: bin -offtarget/ontarget\n");
        return ret;
    }
    if (strncmp(argv[1], "-offtarget", strlen("-offtarget")) == 0) {
        isSimulated = true;
    } else if (strncmp(argv[1], "-ontarget", strlen("-ontarget")) == 0) {
        isSimulated = false;
    } else {
        LOG_ERR("error, only support offtarget or ontarget\n");
        return ret;
    }

    if (MinkHub_wakelockHelper(OP_ACQUIRE_LOCK, EXAMPLEHUB_STR) < 0) {
        LOG_ERR("Could not acquire wakelock in oemvmtzd\n");
    }

    TRUE_OR_CLEAN(!VMCredentials_open(qtvmUuid, uuidLen, &credQtvm),
                  "VMCredentials_open() failed on credQtvm construction\n");
    TRUE_OR_CLEAN(!VMCredentials_open(pvmUuid, uuidLen, &credHlos),
                  "VMCredentials_open() failed on credHlos construction\n");
    TRUE_OR_CLEAN(!VMCredentials_open(oemvmUuid, uuidLen, &credOemvm),
                 "VMCredentials_open() failed on credOemvm construction\n");

    mgr = ServiceManager_new(MAX_SERVICE);
    if (!mgr) {
        LOG_ERR("ServiceManager_new failed\n");
        ret = Object_ERROR_MEM;
        goto cleanup;
    }

    if (isSimulated) {
        connHlos = MinkHub_startService_simulated(mgr, EXAMPLEHUB_QTVM_SOCKADDR_QRTR,
                                                    NULL, credHlos);
        connOemvm = MinkHub_startService_simulated(mgr, EXAMPLEHUB_QTVM_SOCKADDR_QRTR,
                                                     NULL, credOemvm);
    } else {
        connHlos = MinkHub_startService_QRTR(mgr, EXAMPLEHUB_QTVM_SOCKARRD_VSOCK,
                                               NULL, credHlos);
        connOemvm = MinkHub_startService_VSOCK(mgr, EXAMPLEHUB_QTVM_SOCKARRD_VSOCK,
                                                 NULL, credOemvm);
    }
    if (!connHlos) {
        LOG_ERR("failed to start QRTR service on addr: %s\n", EXAMPLEHUB_QTVM_SOCKADDR_QRTR);
        goto cleanup;
    }
    if (!connOemvm) {
        LOG_ERR("failed to start VSOCK service on addr: %s\n", EXAMPLEHUB_QTVM_SOCKARRD_VSOCK);
        goto cleanup;
    }

    connIntern = MinkHub_startService_UNIX(mgr, EXAMPLEHUB_QTVM_SOCKADDR_UNIX, EXAMPLEHUB_SOCKNAME,
                                           credQtvm, NULL, 0);
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

    Object_ASSIGN_NULL(credQtvm);
    Object_ASSIGN_NULL(credHlos);
    Object_ASSIGN_NULL(credOemvm);
    if (connIntern) {
        MinkIPC_release(connIntern);
    }
    if (connHlos) {
        MinkIPC_release(connHlos);
    }
    if (connOemvm) {
        MinkIPC_release(connOemvm);
    }
    if (mgr) {
        ServiceManager_delete(mgr);
    }

    return 0;
}
