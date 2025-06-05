/******************************************************************************
  @file    service_sock.cpp
  @brief   LE performance HAL service

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#define LOG_TAG_HAL "PERF-HAL"
#include "PerfLog.h"
#include <sys/types.h>
#include <sys/stat.h>


#include "Perf_sock.h"
#define PERF_SERVICE_NAME "PERF-SERVICE"
#define SOCKET_NAME "/dev/socket/perfservice"
#define MAX_CONNECTIONS     10

void printMsg(mpctl_msg_t &pMsg) {
    QLOGV(LOG_TAG_HAL, "Msg Contains");
    QLOGV(LOG_TAG_HAL, "Handle: %" PRId32, pMsg.pl_handle);
    QLOGV(LOG_TAG_HAL, "req_type: %" PRIu8, pMsg.req_type);
    QLOGV(LOG_TAG_HAL, "pl_time: %" PRId32, pMsg.pl_time);

    QLOGV(LOG_TAG_HAL, "num Args: %" PRIu16, pMsg.data);
    for (uint16_t i = 0; i < pMsg.data; i++) {
        QLOGV(LOG_TAG_HAL, "pl_args[%" PRIu16 "]: %" PRId32,i, pMsg.pl_args[i]);
    }
    QLOGV(LOG_TAG_HAL, "num Args: %" PRId32, pMsg.numRArgs);
    for (int32_t i = 0; i < pMsg.numRArgs; i++) {
        QLOGV(LOG_TAG_HAL, "reservedArgs[%" PRId32 "]: %" PRId32,i, pMsg.reservedArgs[i]);
    }
    QLOGV(LOG_TAG_HAL, "client_pid: %" PRId32, pMsg.client_pid);
    QLOGV(LOG_TAG_HAL, "client_tid: %" PRId32, pMsg.client_tid);
    QLOGV(LOG_TAG_HAL, "Hint Id: %x, Hint Type: %" PRId32, pMsg.hint_id, pMsg.hint_type);
    QLOGV(LOG_TAG_HAL, "usrdata_str: %s", pMsg.usrdata_str);
    QLOGV(LOG_TAG_HAL, "propDefVal: %s", pMsg.propDefVal);
    QLOGV(LOG_TAG_HAL, "renewFlag: %" PRId8, pMsg.renewFlag);
    QLOGV(LOG_TAG_HAL, "offloadFlag: %" PRId8, pMsg.offloadFlag);
}
int32_t Perf::recvMsg(mpctl_msg_t &msg) {
    QLOGV(LOG_TAG_HAL, "waiting to recv msg %s",__func__);
    int32_t msgSize = sizeof(mpctl_msg_t);
    int32_t bytes_read = recv(mSocketHandler, &msg, msgSize, 0);
    QLOGL(LOG_TAG_HAL, QLOG_L2, "received msg %s",__func__);
    if (bytes_read != msgSize) {
        QLOGE(LOG_TAG_HAL, "Invalid msg received from client");
        return FAILED;
    }
    return SUCCESS;
}

int32_t Perf::sendMsg(void *msg, int32_t type) {
    int32_t msgSize = 0;
    int32_t rc = FAILED;
    switch (type) {
        case PERF_LOCK_TYPE: {
            msgSize = sizeof(client_msg_int_t);
            break;
        }
        case PERF_GET_PROP_TYPE: {
            client_msg_str_t *tmp = (client_msg_str_t*)msg;
            if (tmp == NULL) {
                return rc;
            }

            QLOGL(LOG_TAG_HAL, QLOG_L2, "value %s", tmp->value);
            msgSize = sizeof(client_msg_str_t);
            break;
        }
        case PERF_SYNC_REQ_TYPE: {
            msgSize = sizeof(client_msg_sync_req_t);
            break;
        }
        default: {
            QLOGE(LOG_TAG_HAL, "Invalid Msg type Received %d", type);
            return rc;
        }
    }

    rc = send(mSocketHandler, msg, msgSize, 0);
    QLOGV(LOG_TAG_HAL, "sent msg");
    return rc;
}

void Perf::closeSocket(int32_t socketHandler) {
    QLOGV(LOG_TAG_HAL, "Closing client handler");
    close(socketHandler);
    setSocketHandler(-1);
}


void Perf::setSocketHandler(int32_t socketHandler) {
    mSocketHandler = socketHandler;
}

int32_t Perf::getSocketHandler() {
    return mSocketHandler;
}

int32_t Perf::connectToSocket() {
    int32_t rc = FAILED;
    /* Attempt to open socket */
    char mode[] = "0666";
    int perm = 0;
    mListnerHandler = socket(AF_UNIX, SOCK_STREAM, 0);

    if (mListnerHandler < 0) {
        QLOGE(LOG_TAG_HAL, "Unable to open socket: %s\n", SOCKET_NAME);
        return FAILED;
    }
    mAddr.sun_family = AF_LOCAL;
    snprintf(mAddr.sun_path, UNIX_PATH_MAX, SOCKET_NAME);
    unlink(mAddr.sun_path);
    rc = bind(mListnerHandler, (struct sockaddr*)&mAddr, sizeof(struct sockaddr_un));
    if (rc < 0) {
        QLOGE(LOG_TAG_HAL, "bind() failed %s", SOCKET_NAME);
        return FAILED;
    }
    perm  = strtol(mode, 0, 8);
    if (chmod(SOCKET_NAME, perm) < 0 ) {
        QLOGE(LOG_TAG_HAL, "Failed to set file perm %s", mode);
    }

    /* Create listener for opened socket */
    if (listen(mListnerHandler, MAX_CONNECTIONS) < 0) {
        QLOGE(LOG_TAG_HAL, "listen error: %d", errno);
        QLOGE(LOG_TAG_HAL, "Unable to listen on handle %" PRId32 " for socket %s \n",  mListnerHandler, SOCKET_NAME);
        return FAILED;
    }
    return SUCCESS;
}

void Perf::callAPI(mpctl_msg_t &msg) {
    QLOGV(LOG_TAG_HAL, ": %" PRIu8,msg.req_type);
    printMsg(msg);
    switch (msg.req_type) {
        case PERF_LOCK_CMD            : {   perfLockCmd(msg);
                                            break;
                                        }
        case PERF_LOCK_RELEASE        : {
                                            perfLockRelease(msg);
                                            break;
                                        }
        case PERF_HINT                : {
                                            perfHint(msg);
                                            break;
                                        }
        case PERF_GET_PROP            : {
                                            perfGetProp(msg);
                                            break;
                                        }
        case PERF_LOCK_ACQUIRE        : {
                                            perfLockAcquire(msg);
                                            break;
                                        }
        case PERF_LOCK_ACQUIRE_RELEASE: {
                                            perfLockAcqAndRelease(msg);
                                            break;
                                        }
        case PERF_GET_FEEDBACK        : {
                                            perfGetFeedback(msg);
                                            break;
                                        }
        case PERF_EVENT               : {
                                            perfEvent(msg);
                                            break;
                                        }
        case PERF_HINT_ACQUIRE_RELEASE: {
                                            perfHintAcqRel(msg);
                                            break;
                                        }
        case PERF_HINT_RENEW          : {
                                            perfHintRenew(msg);
                                            break;
                                        }
        case PERF_SYNC_REQ            : {
                                            perfSyncRequest(msg);
                                            break;
                                        }
        default                       :
                                        QLOGE(LOG_TAG_HAL, "Invalid API Called %" PRId8, msg.req_type);
    }
    closeSocket(getSocketHandler());
}

void Perf::startListener() {
    mpctl_msg_t msg;
    while (!mSignalFlag) {
        memset(&msg, 0, sizeof(mpctl_msg_t));
        struct sockaddr addr;
        socklen_t alen = sizeof(addr);

        /* Check for new incoming connection */
        QLOGV(LOG_TAG_HAL, ": waiting to accept");
        int32_t handler = accept(mListnerHandler, (struct sockaddr *)&addr, &alen);
        QLOGL(LOG_TAG_HAL,  QLOG_L2, ": accepted");
        if (handler > 0) {

            setSocketHandler(handler);
            recvMsg(msg);
            callAPI(msg);
        }
        else {
            QLOGE(LOG_TAG_HAL, "Failed to accept incoming connection: %s", strerror(errno));
        }
    }
    QLOGL(LOG_TAG_HAL, QLOG_L2, "Stopping Listener Handler");
    close(mListnerHandler);
    mListnerHandler = -1;
}

void *Perf::socketMain(void *obj) {
    Perf *me = reinterpret_cast<Perf *>(obj);
    int32_t ret = me->connectToSocket();
    if (ret < 0) {
        QLOGE(LOG_TAG_HAL, "Failed to connect to socket");
        return NULL;
    }

    me->startListener();

    return NULL;
}

int32_t Perf::startService() {
    /* Connect to socket*/
    /* Start the listener thread to handling incoming connections */
    int32_t rc = FAILED;
    rc = pthread_create(&mPerfThread, NULL, Perf::socketMain, this);
    if (rc < 0) {
        QLOGE(LOG_TAG_HAL, "Failed to initialize listener thread");
        return rc;
    }
    rc = pthread_setname_np(mPerfThread, PERF_SERVICE_NAME);
    if (rc != SUCCESS) {
        QLOGE(LOG_TAG_HAL, "Failed to name Perf Service:  %s", PERF_SERVICE_NAME);
    }
    return SUCCESS;
}

void Perf::joinThread() {
    pthread_join(mPerfThread, NULL);
}

static void SignalHandler(int32_t sig) {
    /* Dummy signal handler */
    Perf &perf = Perf::getPerfInstance();
    perf.setSignal(true);
    perf.joinThread();
}

int main() {
    Perf &perf = Perf::getPerfInstance();
    int32_t rc =  perf.startService();
    QLOGV(LOG_TAG_HAL, "Starting socket");
    if (rc < 0) {
        QLOGE(LOG_TAG_HAL, "unable to start perf service");
        exit(EXIT_FAILURE);
    }

    sigset_t signal_mask;
    int32_t sig;

    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);

    /* Register the signal handler */
    sigfillset(&signal_mask);
    signal(SIGTERM, &SignalHandler);
    signal(SIGPIPE, SIG_IGN);
    while (1) {
        sigwait(&signal_mask, &sig);
        switch (sig) {
            case SIGTERM:
                QLOGV(LOG_TAG_HAL, "SIGTERM received, stopping service");
                exit(EXIT_SUCCESS);
                break;
        }
    }
    return SUCCESS;
}
