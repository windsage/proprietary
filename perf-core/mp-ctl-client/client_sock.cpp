/******************************************************************************
  @file    client_sock.cpp
  @brief   LE performance PerfLock client library

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG_SOCK "CLIENT-SOCK"
#include "PerfLog.h"
#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <exception>
#include <cstring>
#include "PerfController.h"
#include "PerfThreadPool.h"
#include "Common.h"
#include <sys/prctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/un.h>
#include "PerfOffloadHelper.h"
#include "client.h"

#define FAILED -1
#define SUCCESS 0
#define THREADS_IN_POOL 4
#define DEFAULT_OFFLOAD_FLAG 0
#define SET_OFFLOAD_FLAG 1
#define MPCTL_SOCKET "/dev/socket/perfservice"

static double perf_hal_ver = 0.0;
static bool is_perf_hal_alive();
static int32_t perf_hint_private(int hint, const char *pkg, int duration, int type, int numArgs, int list[], bool offloadFlag = false, bool feedback = false);
static void perf_event_private(int eventId, const char *pkg,  int numArgs, int list[], bool offloadFlag = false);
static int32_t perf_hint_acq_rel_private(int handle, int hint, const char *pkg, int duration, int type, int numArgs, int list[], bool offloadFlag = false, bool renewFlag = false);

static int recvMsg(void *msg, int msgSize, int socketHandler);
static void printMsg(mpctl_msg_t pMsg);
static int sendMsg(void *msg, int msgSize, int socketHandler);
static void closeSocket(int socketHandler);
static int connectToPerf();

static int recvMsg(void *msg, int msgSize, int socketHandler) {
    QLOGV(LOG_TAG_SOCK, "Waiting To receive msg %s", __func__);
    int bytes_read = recv(socketHandler, msg, msgSize, 0);
    QLOGV(LOG_TAG_SOCK, "received msg %s", __func__);
    if (bytes_read != msgSize) {
        QLOGE(LOG_TAG_SOCK, "Invalid msg  received from server");
        return FAILED;
    }
    return SUCCESS;
}

static void printMsg(mpctl_msg_t pMsg) {
    QLOGD(LOG_TAG_SOCK, "==============================");
    QLOGD(LOG_TAG_SOCK, "Msg Contains");
    QLOGD(LOG_TAG_SOCK, "Handle: %d", pMsg.pl_handle);
    QLOGD(LOG_TAG_SOCK, "req_type: %d", pMsg.req_type);
    QLOGD(LOG_TAG_SOCK, "pl_time: %d", pMsg.pl_time);

    QLOGD(LOG_TAG_SOCK, "num Args: %d", pMsg.data);
    for (int32_t i = 0; i < pMsg.data; i++) {
        QLOGD(LOG_TAG_SOCK, "pl_args[%d]: %d",i, pMsg.pl_args[i]);
    }
    QLOGD(LOG_TAG_SOCK, "num Args: %d", pMsg.numRArgs);
    for (int32_t i = 0; i < pMsg.numRArgs; i++) {
        QLOGD(LOG_TAG_SOCK, "reservedArgs[%d]: %d",i, pMsg.reservedArgs[i]);
    }
    QLOGD(LOG_TAG_SOCK, "client_pid: %d", pMsg.client_pid);
    QLOGD(LOG_TAG_SOCK, "client_tid: %d", pMsg.client_tid);
    QLOGD(LOG_TAG_SOCK, "Hint Id: %x, Hint Type: %d", pMsg.hint_id, pMsg.hint_type);
    QLOGD(LOG_TAG_SOCK, "usrdata_str: %s", pMsg.usrdata_str);
    QLOGD(LOG_TAG_SOCK, "propDefVal: %s", pMsg.propDefVal);
    QLOGD(LOG_TAG_SOCK, "renewFlag: %d", pMsg.renewFlag);
    QLOGD(LOG_TAG_SOCK, "offloadFlag: %d", pMsg.offloadFlag);
    QLOGD(LOG_TAG_SOCK, "==============================");
}

static int sendMsg(void *msg, int msgSize, int socketHandler) {
    QLOGV(LOG_TAG_SOCK, "Sending msg %s", __func__);
    int rc = send(socketHandler, msg, msgSize, 0);
    QLOGV(LOG_TAG_SOCK, "Sending msg completed");
    return rc;
}

static void closeSocket(int socketHandler) {
    QLOGV(LOG_TAG_SOCK, "Closing client handler");
    close(socketHandler);
}

static int connectToPerf() {
    int client_comsoc = -1;
    struct sockaddr_un client_addr;
    int rc = FAILED;
    int len = 0;

    client_comsoc = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_comsoc < 0) {
        QLOGE(LOG_TAG_SOCK, "Failed to initialize control channel");
        return 0;
    }

    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    client_addr.sun_family = AF_UNIX;
    snprintf(client_addr.sun_path, UNIX_PATH_MAX, MPCTL_SOCKET);
    len = sizeof(struct sockaddr_un);
    rc = connect(client_comsoc, (struct sockaddr *) &client_addr, len);
    if (rc < 0) {
        QLOGE(LOG_TAG_SOCK, "Failed to connect to perfservice");
        return FAILED;
    }
    return client_comsoc;
}

int perf_lock_acq(int handle, int duration, int list[], int numArgs) {
    int rc = FAILED;
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    if (duration < 0 || numArgs <= 0 || numArgs > MAX_ARGS_PER_REQUEST) {
        QLOGE(LOG_TAG_SOCK, "Invalid Arguments passed to %s", __func__);
        return FAILED;
    }
    int socketHandler = connectToPerf();
    if (socketHandler < 0) {
        QLOGE(LOG_TAG_SOCK, "Error in connecting to Perf Service");
        return rc;
    }
    mpctl_msg_t msg;
    client_msg_int_t rMsg = {FAILED};
    try {
        int msgSize = sizeof(mpctl_msg_t);
        memset(&msg, 0, msgSize);
        msg.client_pid = getpid();
        msg.client_tid = gettid();
        msg.req_type = PERF_LOCK_ACQUIRE;
        msg.pl_handle = handle;
        msg.pl_time = duration;
        msg.data = numArgs;
        if (handle > 0) {
            msg.renewFlag = true;
        }
        memcpy(&(msg.pl_args), list, sizeof(int) * numArgs);
        if (socketHandler > 0) {
            rc = sendMsg(&msg, msgSize, socketHandler);
            if (rc == FAILED || rc == EMSGSIZE) {
                QLOGE(LOG_TAG_SOCK, "Error in communicating to Perf Service");
            } else {
                recvMsg(&rMsg, sizeof(client_msg_int_t), socketHandler);
            }
            closeSocket(socketHandler);
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }
    return rMsg.handle;
}

int perf_lock_rel(int handle) {
    QLOGV(LOG_TAG_SOCK, "inside perf_lock_rel of client");
    int rc = FAILED;
    if (handle == 0) {
        QLOGE(LOG_TAG_SOCK, "Invalid Arguments passed to %s", __func__);
        return FAILED;
    }

    int socketHandler = connectToPerf();
    if (socketHandler < 0) {
        QLOGE(LOG_TAG_SOCK, "Error in connecting to Perf Service");
        return FAILED;
    }

    try {
        mpctl_msg_t msg;
        int msgSize = sizeof(mpctl_msg_t);
        memset(&msg, 0, msgSize);
        msg.client_pid = getpid();
        msg.client_tid = gettid();
        msg.req_type = PERF_LOCK_RELEASE;
        msg.pl_handle = handle;
        if (socketHandler > 0) {
            rc = sendMsg(&msg, msgSize, socketHandler);
            if (rc == FAILED || rc == EMSGSIZE) {
                QLOGE(LOG_TAG_SOCK, "Error in communicating to Perf Service");
            }
            closeSocket(socketHandler);
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }
    return SUCCESS;
}

int perf_hint(int hint, const char *pkg, int duration, int type) {
    return perf_hint_private(hint, pkg, duration, type, 0, NULL);
}

int32_t perf_hint_private(int hint, const char *pkg, int duration, int type, int numArgs, int list[],bool offloadFlag, bool feedback) {
    int32_t rc = FAILED;
    if (feedback == false) {
        QLOGV(LOG_TAG_SOCK, "inside %s of client",__func__);
    }
    if (duration < 0 || numArgs < 0 || numArgs > MAX_RESERVE_ARGS_PER_REQUEST) {
        QLOGE(LOG_TAG_SOCK, "Invalid Arguments passed to %s", __func__);
        return rc;
    }

    int socketHandler = connectToPerf();
    if (socketHandler < 0) {
        QLOGE(LOG_TAG_SOCK, "Error in connecting to Perf Service");
        return rc;
    }

    mpctl_msg_t msg;
    client_msg_int_t rMsg = {FAILED};
    try {
        int msgSize = sizeof(mpctl_msg_t);
        memset(&msg, 0, msgSize);
        msg.client_pid = getpid();
        msg.client_tid = gettid();
        msg.hint_id = hint;
        msg.hint_type = type;
        msg.pl_time = duration;
        msg.numRArgs = numArgs;
        msg.offloadFlag = offloadFlag;
        msg.req_type = feedback ? PERF_GET_FEEDBACK: PERF_HINT;

        if (pkg != NULL) {
            memcpy(&(msg.usrdata_str), pkg, sizeof(char) * strlen(pkg));
        }
        if (msg.numRArgs > 0) {
            memcpy(&(msg.reservedArgs), list, sizeof(int) * msg.numRArgs);
        }
        if (socketHandler > 0) {
            rc = sendMsg(&msg, msgSize, socketHandler);
            if (rc == FAILED || rc == EMSGSIZE) {
                QLOGE(LOG_TAG_SOCK, "Error in communicating to Perf Service");
            } else {
                recvMsg(&rMsg, sizeof(client_msg_int_t), socketHandler);
            }
            closeSocket(socketHandler);
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }
    return rMsg.handle;
}

int perf_get_feedback(int req, const char *pkg) {

    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    return perf_hint_private(req, pkg, 0, 0, 0, NULL, false, true);
}

//This  API is getting deprecated...
PropVal perf_get_prop(const char *prop , const char *def_val) {
    int rc = FAILED;
    int len = 0;
    PropVal rMsg = {""};
    QLOGV(LOG_TAG_SOCK, "inside %s of client",__func__);

    if (!def_val || strlen(def_val) >= PROP_VAL_LENGTH) {
        QLOGE(LOG_TAG_SOCK, "Invalid default value passed in %s", __func__);
        return rMsg;
    }

    if (!prop || strlen(prop) >= MAX_MSG_APP_NAME_LEN) {
        QLOGE(LOG_TAG_SOCK, "Invalid prop passed in %s", __func__);
        return rMsg;
    }

    int socketHandler = connectToPerf();
    if (socketHandler < 0) {
        QLOGE(LOG_TAG_SOCK, "Error in connecting to Perf Service");
    } else {

        mpctl_msg_t msg;
        try {
            int msgSize = sizeof(mpctl_msg_t);
            memset(&msg, 0, msgSize);
            msg.client_pid = getpid();
            msg.client_tid = gettid();
            msg.req_type = PERF_GET_PROP;
            strlcpy(msg.usrdata_str, prop, MAX_MSG_APP_NAME_LEN);
            msg.usrdata_str[MAX_MSG_APP_NAME_LEN - 1] = '\0';
            strlcpy(msg.propDefVal, def_val, PROP_VAL_LENGTH);
            msg.propDefVal[PROP_VAL_LENGTH - 1] = '\0';
            if (socketHandler > 0) {
                rc = sendMsg(&msg, msgSize, socketHandler);
                if (rc == FAILED || rc == EMSGSIZE) {
                    QLOGE(LOG_TAG_SOCK, "Error in communicating to Perf Service");
                } else {
                    recvMsg(&rMsg, sizeof(PropVal), socketHandler);
                }
                closeSocket(socketHandler);
            }
        } catch (std::exception &e) {
            QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
        }
    }
    if (rc == FAILED) {
        if (def_val != NULL) {
            len = strlen(def_val);
            strlcpy(rMsg.value, def_val, len + 1);
            rMsg.value[PROP_VAL_LENGTH - 1] = '\0';
        }
    }
    return rMsg;
}
//This is the new perf get prop extn API which takes a buffer and its size for writing result values.
int perf_get_prop_extn(const char *prop , char *buffer, size_t buffer_size, const char *def_val) {


    int rc = FAILED;
    int len = 0;
    PropVal rMsg = {""};

    QLOGV(LOG_TAG_SOCK, "inside %s of client",__func__);
    if (buffer_size < PROP_VAL_LENGTH || buffer == NULL) {
        if (buffer != NULL) {
            QLOGE(LOG_TAG, "Invalid Buffer passed of size %d Expected: %d", buffer_size, PROP_VAL_LENGTH);
        } else {
            QLOGE(LOG_TAG, "Invalid empty buffer passed");
        }
        return rc;
    }

    if (!def_val || strlen(def_val) >= PROP_VAL_LENGTH) {
        QLOGE(LOG_TAG_SOCK, "Invalid default value passed in %s", __func__);
        return rc;
    }

    if (!prop || strlen(prop) >= MAX_MSG_APP_NAME_LEN) {
        QLOGE(LOG_TAG_SOCK, "Invalid prop passed in %s", __func__);
        return rc;
    }

    int socketHandler = connectToPerf();
    if (socketHandler < 0) {
        QLOGE(LOG_TAG_SOCK, "Error in connecting to Perf Service");
    } else {

        mpctl_msg_t msg;
        try {
            int msgSize = sizeof(mpctl_msg_t);
            memset(&msg, 0, msgSize);
            msg.client_pid = getpid();
            msg.client_tid = gettid();
            msg.req_type = PERF_GET_PROP;
            strlcpy(msg.usrdata_str, prop, MAX_MSG_APP_NAME_LEN);
            msg.usrdata_str[MAX_MSG_APP_NAME_LEN - 1] = '\0';
            strlcpy(msg.propDefVal, def_val, PROP_VAL_LENGTH);
            msg.propDefVal[PROP_VAL_LENGTH - 1] = '\0';
            if (socketHandler > 0) {
                rc = sendMsg(&msg, msgSize, socketHandler);
                if (rc == FAILED || rc == EMSGSIZE) {
                    QLOGE(LOG_TAG_SOCK, "Error in communicating to Perf Service");
                } else {
                    recvMsg(&rMsg, sizeof(PropVal), socketHandler);
                }
                closeSocket(socketHandler);
            }
        } catch (std::exception &e) {
            QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
        }
    }
    if (rc == FAILED) {
        if (def_val != NULL) {
            len = strlen(def_val);
            strlcpy(rMsg.value, def_val, len + 1);
            rMsg.value[PROP_VAL_LENGTH - 1] = '\0';
        }
    }
    strlcpy(buffer, rMsg.value, PROP_VAL_LENGTH);
    return rc;
}

int perf_lock_acq_rel(int handle, int duration, int list[], int numArgs, int reserveNumArgs) {

    int rc = FAILED;
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    if (duration < 0 || numArgs <= 0 || numArgs > MAX_ARGS_PER_REQUEST ||
            reserveNumArgs < 0 || reserveNumArgs > MAX_RESERVE_ARGS_PER_REQUEST ||
            (reserveNumArgs + numArgs) > (MAX_ARGS_PER_REQUEST + MAX_RESERVE_ARGS_PER_REQUEST) ||
            (reserveNumArgs + numArgs) <= 0) {
        return FAILED;
    }

    int socketHandler = connectToPerf();
    if (socketHandler < 0) {
        QLOGE(LOG_TAG_SOCK, "Error in connecting to Perf Service");
        return rc;
    }
    mpctl_msg_t msg;
    client_msg_int_t rMsg = {FAILED};
    try {
        int msgSize = sizeof(mpctl_msg_t);
        memset(&msg, 0, msgSize);
        msg.client_pid = getpid();
        msg.client_tid = gettid();
        msg.req_type = PERF_LOCK_ACQUIRE_RELEASE;
        msg.pl_handle = handle;
        msg.pl_time = duration;
        msg.data = numArgs;
        msg.numRArgs = reserveNumArgs;
        msg.renewFlag = false;
        memcpy(&(msg.pl_args), list, sizeof(int) * numArgs);
        if (msg.numRArgs > 0) {
            memcpy(&(msg.reservedArgs), list + msg.data, sizeof(int) * msg.numRArgs);
        }
        if (socketHandler > 0) {
            rc = sendMsg(&msg, msgSize, socketHandler);
            if (rc == FAILED || rc == EMSGSIZE) {
                QLOGE(LOG_TAG_SOCK, "Error in communicating to Perf Service");
            } else {
                recvMsg(&rMsg, sizeof(client_msg_int_t), socketHandler);
            }
            closeSocket(socketHandler);
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }
    return rMsg.handle;

}

int perf_get_feedback_extn(int req, const char *pkg, int numArgs, int list[]) {
    int rc = -1;
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    return perf_hint_private(req, pkg, 0, 0, numArgs, list, false, true);
}

void perf_event(int eventId, const char *pkg, int numArgs, int list[]) {
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    perf_event_private(eventId, pkg, numArgs, list);
}

void perf_event_private(int eventId, const char *pkg, int numArgs, int list[], bool offloadFlag) {
    int rc = FAILED;
    QLOGV(LOG_TAG_SOCK, "inside %s of client",__func__);
    if (numArgs < 0 || numArgs > MAX_RESERVE_ARGS_PER_REQUEST) {
        QLOGE(LOG_TAG_SOCK, "Invalid Arguments passed to %s", __func__);
        return;
    }

    int socketHandler = connectToPerf();
    if (socketHandler < 0) {
        QLOGE(LOG_TAG_SOCK, "Error in connecting to Perf Service");
        return;
    }

    mpctl_msg_t msg;
    try {
        int msgSize = sizeof(mpctl_msg_t);
        memset(&msg, 0, msgSize);
        msg.client_pid = getpid();
        msg.client_tid = gettid();
        msg.hint_id = eventId;
        msg.numRArgs = numArgs;
        msg.offloadFlag = offloadFlag;
        msg.req_type = PERF_EVENT;

        if (pkg != NULL) {
            strlcpy(msg.usrdata_str, pkg, MAX_MSG_APP_NAME_LEN);
        }
        if (msg.numRArgs > 0) {
            memcpy(&(msg.reservedArgs), list, sizeof(int) * msg.numRArgs);
        }
        if (socketHandler > 0) {
            rc = sendMsg(&msg, msgSize, socketHandler);
            if (rc == FAILED || rc == EMSGSIZE) {
                QLOGE(LOG_TAG_SOCK, "Error in communicating to Perf Service");
            }
            closeSocket(socketHandler);
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }

}

int perf_hint_acq_rel(int handle, int hint, const char *pkg, int duration, int type,
        int numArgs, int list[]) {
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    return perf_hint_acq_rel_private(handle, hint, pkg, duration, type, numArgs, list);
}

int32_t perf_hint_acq_rel_private(int handle, int hint, const char *pkg, int duration, int type,
        int numArgs, int list[], bool offloadFlag, bool renewFlag) {
    int32_t rc = FAILED;
    if (renewFlag == false) {
        QLOGV(LOG_TAG_SOCK, "inside %s of client",__func__);
    }
    if (duration < 0 || numArgs < 0 || numArgs > MAX_RESERVE_ARGS_PER_REQUEST) {
        QLOGE(LOG_TAG_SOCK, "Invalid Arguments passed to %s", __func__);
        return rc;
    }

    int socketHandler = connectToPerf();
    if (socketHandler < 0) {
        QLOGE(LOG_TAG_SOCK, "Error in connecting to Perf Service");
        return rc;
    }

    mpctl_msg_t msg;
    client_msg_int_t rMsg = {FAILED};
    try {
        int msgSize = sizeof(mpctl_msg_t);
        memset(&msg, 0, msgSize);
        msg.client_pid = getpid();
        msg.client_tid = gettid();
        msg.pl_handle = handle;
        msg.hint_id = hint;
        msg.hint_type = type;
        msg.pl_time = duration;
        msg.numRArgs = numArgs;
        msg.offloadFlag = offloadFlag;
        msg.renewFlag = renewFlag;
        msg.req_type = renewFlag ? PERF_HINT_RENEW:PERF_HINT_ACQUIRE_RELEASE;

        if (pkg != NULL) {
            memcpy(&(msg.usrdata_str), pkg, MAX_MSG_APP_NAME_LEN);
        }
        if (msg.numRArgs > 0) {
            memcpy(&(msg.reservedArgs), list, sizeof(int) * msg.numRArgs);
        }
        if (socketHandler > 0) {
            rc = sendMsg(&msg, msgSize, socketHandler);
            if (rc == FAILED || rc == EMSGSIZE) {
                QLOGE(LOG_TAG_SOCK, "Error in communicating to Perf Service");
            } else {
                recvMsg(&rMsg, sizeof(client_msg_int_t), socketHandler);
            }
            closeSocket(socketHandler);
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }
    return rMsg.handle;
}

int perf_hint_renew(int handle, int hint, const char *pkg, int duration, int type,
        int numArgs, int list[]) {
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);

  return perf_hint_acq_rel_private(handle, hint, pkg, duration, type, numArgs, list, false, true);
}

/*
below are thread based offload API's
*/
int perf_hint_offload(int hint, const char *pkg, int duration, int type, int listlen, int list[]) {
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    int rc = FAILED;
    char *pkg_t = NULL;
    int *list_t = NULL;

    if (listlen > MAX_RESERVE_ARGS_PER_REQUEST || listlen < 0) {
        QLOGE(LOG_TAG_SOCK, "Invalid number of arguments %d Max Accepted %d", listlen, MAX_RESERVE_ARGS_PER_REQUEST);
        return FAILED;
    }

    if (!is_perf_hal_alive()) {
        return rc;
    }

    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool(THREADS_IN_POOL);
    PerfOffloadHelper &poh = PerfOffloadHelper::getPerfOffloadHelper();
    int32_t offload_handle = poh.getNewOffloadHandle();
    if (offload_handle == FAILED) {
        QLOGE(LOG_TAG_SOCK, "Failed to create Offload Handle in %s", __func__);
        return rc;
    }

    if (pkg != NULL) {
        int len = strlen(pkg);
        pkg_t = (char*)calloc(len + 1,sizeof(char));
        if (pkg_t == NULL) {
            QLOGE(LOG_TAG_SOCK, "Memory alloc error in %s", __func__);
            return rc;
        }
        strlcpy(pkg_t, pkg, len + 1);
        pkg_t[len] = '\0';
    }

    if (listlen > 0) {
        list_t = (int*)calloc(listlen, sizeof(int));
        if (list_t == NULL) {
            QLOGE(LOG_TAG_SOCK, "Memory alloc error in %s", __func__);
            if (pkg_t != NULL) {
                free(pkg_t);
                pkg_t = NULL;
            }
            return rc;
        }
        memcpy(list_t, list, sizeof(int) * listlen);
    }

    try {
        rc = ptp.placeTask([=]() mutable {
                    int32_t handle = perf_hint_private(hint, pkg_t, duration, type, listlen, list_t, SET_OFFLOAD_FLAG);
                    PerfOffloadHelper &poh = PerfOffloadHelper::getPerfOffloadHelper();
                    int32_t res = poh.mapPerfHandle(offload_handle, handle);
                    if (res == RELEASE_REQ_STATE) {
                        perf_lock_rel(handle);
                        poh.releaseHandle(offload_handle);
                    }
                    if (pkg_t != NULL) {
                       free(pkg_t);
                       pkg_t = NULL;
                    }
                    if (list_t != NULL) {
                        free(list_t);
                        list_t = NULL;
                    }
                });
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG_SOCK, "Failed to Offload hint: 0x%X", hint);
        if (pkg_t != NULL) {
            free(pkg_t);
            pkg_t = NULL;
        }
        if (list_t != NULL) {
            free(list_t);
            list_t = NULL;
        }
    } else {
        rc = offload_handle;
    }
    return rc;
}

int perf_lock_rel_offload(int handle) {
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    int rc = FAILED;
    if (!is_perf_hal_alive()) {
        return rc;
    }
    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool(THREADS_IN_POOL);

    try {
        rc = ptp.placeTask([=]() {
                PerfOffloadHelper &poh = PerfOffloadHelper::getPerfOffloadHelper();
                int32_t releaseHandle = poh.getPerfHandle(handle);
                    if (releaseHandle > 0) {
                        int p_handle = perf_lock_rel(releaseHandle);
                        poh.releaseHandle(handle);
                    }
                });
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG_SOCK, "Failed to Offload release for %d", handle);
    }
    return rc;
}

int perf_hint_acq_rel_offload(int handle, int hint, const char *pkg, int duration, int type, int numArgs, int list[]) {
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    int rc = FAILED;
    char *pkg_t = NULL;
    int *list_t = NULL;

    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE(LOG_TAG_SOCK, "Invalid number of arguments %d Max Accepted %d", numArgs, MAX_RESERVE_ARGS_PER_REQUEST);
        return FAILED;
    }

    if (!is_perf_hal_alive()) {
        return rc;
    }

    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool(THREADS_IN_POOL);
    PerfOffloadHelper &poh = PerfOffloadHelper::getPerfOffloadHelper();
    int32_t offload_handle = poh.getNewOffloadHandle();
    if (offload_handle == FAILED) {
        QLOGE(LOG_TAG_SOCK, "Failed to create Offload Handle in %s", __func__);
        return rc;
    }

    if (pkg != NULL) {
        int len = strlen(pkg);
        pkg_t = (char*)calloc(len + 1,sizeof(char));
        if (pkg_t == NULL) {
            QLOGE(LOG_TAG_SOCK, "Memory alloc error in %s", __func__);
            return rc;
        }
        strlcpy(pkg_t, pkg, len + 1);
        pkg_t[len] = '\0';
    }
    if (numArgs > 0) {
        list_t = (int*)calloc(numArgs, sizeof(int));
        if (list_t == NULL) {
            QLOGE(LOG_TAG_SOCK, "Memory alloc error in %s", __func__);
            if (pkg_t != NULL) {
                free(pkg_t);
                pkg_t = NULL;
            }
            return rc;
        }
        memcpy(list_t, list, sizeof(int) * numArgs);
    }

    try {
        rc = ptp.placeTask([=]() mutable {
                PerfOffloadHelper &poh = PerfOffloadHelper::getPerfOffloadHelper();
                int32_t releaseHandle = poh.getPerfHandle(handle);
                QLOGV(LOG_TAG, "%s releasing %d mapped to %d",__func__,releaseHandle, handle);
                int32_t p_handle = perf_hint_acq_rel_private(releaseHandle, hint, pkg_t, duration, type, numArgs, list_t, SET_OFFLOAD_FLAG);
                int32_t res = poh.mapPerfHandle(offload_handle, p_handle);
                if (res == RELEASE_REQ_STATE) {
                    perf_lock_rel(p_handle);
                    poh.releaseHandle(offload_handle);
                }
                if (releaseHandle > 0) {
                    poh.releaseHandle(handle);
                }

                if (pkg_t != NULL) {
                    free(pkg_t);
                    pkg_t = NULL;
                }
                if (list_t != NULL) {
                    free(list_t);
                    list_t = NULL;
                }
        });
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG_SOCK, "Failed to Offload hint: 0x%X", hint);
        if (pkg_t != NULL) {
            free(pkg_t);
            pkg_t = NULL;
        }
        if (list_t != NULL) {
            free(list_t);
            list_t = NULL;
        }
    } else {
        rc = offload_handle;
    }
    return rc;
}

int perf_event_offload(int eventId, const char *pkg, int numArgs, int list[]) {
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    int rc = FAILED;
    char *pkg_t = NULL;
    int *list_t = NULL;

    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE(LOG_TAG_SOCK, "Invalid number of arguments %d", numArgs);
        return FAILED;
    }
    if (!is_perf_hal_alive()) {
        return rc;
    }

    if (pkg != NULL) {
        int len = strlen(pkg);
        pkg_t = (char*)calloc(len + 1,sizeof(char));
        if (pkg_t == NULL) {
            QLOGE(LOG_TAG_SOCK, "Memory alloc error in %s", __func__);
            return rc;
        }
        strlcpy(pkg_t, pkg, len + 1);
        pkg_t[len] = '\0';
    }
    if (numArgs > 0) {
        list_t = (int*)calloc(numArgs, sizeof(int));
        if (list_t == NULL) {
            QLOGE(LOG_TAG_SOCK, "Memory alloc error in %s", __func__);
            if (pkg_t != NULL) {
                free(pkg_t);
                pkg_t = NULL;
            }
            return rc;
        }
        memcpy(list_t, list, sizeof(int) * numArgs);
    }

    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool(THREADS_IN_POOL);
    try {
        rc = ptp.placeTask([=]() mutable {
                    perf_event_private(eventId, pkg_t, numArgs, list_t, SET_OFFLOAD_FLAG);
                    if (pkg_t != NULL) {
                        free(pkg_t);
                        pkg_t = NULL;
                    }
                    if (list_t != NULL) {
                        free(list_t);
                        list_t = NULL;
                    }
                });
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG_SOCK, "Failed to Offload event: 0x%X", eventId);
        if (pkg_t != NULL) {
            free(pkg_t);
            pkg_t = NULL;
        }
        if (list_t != NULL) {
            free(list_t);
            list_t = NULL;
        }
    }
    return rc;
}

static bool is_perf_hal_alive() {
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    return true;

}


double get_perf_hal_ver() {
    QLOGV(LOG_TAG_SOCK, "inside %s of client", __func__);
    perf_hal_ver = strtod(PACKAGE_VERSION, NULL);

    return perf_hal_ver;
}

void perf_lock_cmd(int cmd) {
    int rc = FAILED;
    int socketHandler = connectToPerf();
    if (socketHandler < 0) {
        QLOGE(LOG_TAG_SOCK, "Error in connecting to Perf Service");
        return;
    }

    mpctl_msg_t msg;
    try {
        int msgSize = sizeof(mpctl_msg_t);
        memset(&msg, 0, msgSize);
        msg.client_pid = getpid();
        msg.client_tid = gettid();
        msg.req_type = PERF_LOCK_CMD;
        msg.data = cmd;

        if (socketHandler > 0) {
            rc = sendMsg(&msg, msgSize, socketHandler);
            if (rc == FAILED || rc == EMSGSIZE) {
                QLOGE(LOG_TAG_SOCK, "Error in communicating to Perf Service");
            }
            closeSocket(socketHandler);
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
    }
}

/* this function returns a malloced string which must be freed by the caller */
const char* perf_sync_request(int cmd) {
    int rc = FAILED;
    int len = 0;
    char *out = NULL;
    client_msg_sync_req_t rMsg = {""};
    QLOGV(LOG_TAG_SOCK, "inside %s of client",__func__);

    int socketHandler = connectToPerf();
    if (socketHandler < 0) {
        QLOGE(LOG_TAG_SOCK, "Error in connecting to Perf Service");
    } else {

        mpctl_msg_t msg;
        try {
            int msgSize = sizeof(mpctl_msg_t);
            memset(&msg, 0, msgSize);
            msg.client_pid = getpid();
            msg.client_tid = gettid();
            msg.req_type = PERF_SYNC_REQ;
            msg.hint_id = cmd;
            if (socketHandler > 0) {
                rc = sendMsg(&msg, msgSize, socketHandler);
                if (rc == FAILED || rc == EMSGSIZE) {
                    QLOGE(LOG_TAG_SOCK, "Error in communicating to Perf Service");
                } else {
                    recvMsg(&rMsg, sizeof(client_msg_sync_req_t), socketHandler);
                }
                closeSocket(socketHandler);
            }
        } catch (std::exception &e) {
            QLOGE(LOG_TAG_SOCK, "Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE(LOG_TAG_SOCK, "Exception caught in %s", __func__);
        }
    }
    if (strlen(rMsg.value) > 0) {
        out = strdup(rMsg.value);
    }
    return out;
}

void getthread_name(char *buf,int buf_size) {
   int rc=-1;
   if(buf_size>=MAX_BUF_SIZE){
       rc=prctl(PR_GET_NAME, reinterpret_cast<unsigned long>(const_cast<char *>(buf)) , 0, 0,0);
   }
   if(rc!=0){
       QLOGE(LOG_TAG_SOCK, "Failed in getting thread name");
   }
}
