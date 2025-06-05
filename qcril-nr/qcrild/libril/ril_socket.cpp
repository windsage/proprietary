/*
* Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* Not a Contribution.
* Apache license notifications and license are retained
* for attribution purposes only.
*/
/* //device/libs/telephony/ril.cpp
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <Marshal.h>
#include <telephony/record_stream.h>
#include <libril/ril_ex.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>
#include <telephony/ril_call.h>
#include <ril_socket_api.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ril_event.h>
#include "ril_internal.h"
#include "WakeLock.h"
#include <inttypes.h>
#include <log/log.h>
#include <sys/stat.h>
#include <string.h>
#include "RilSocketClientController.h"

#undef TAG
#define TAG "RILQ"

#define MAX_COMMAND_BYTES (8 * 1024)
#define PHONE_PROCESS "radio"
#define RIL_SOCKET_PATH "/dev/socket/qcrild/rild"

static void listenCallback(int fd, short flags, void *param);
static int processCommandBuffer(void *buffer, size_t buflen, RIL_SOCKET_ID socket_id,
    int fd, bool& isDisconnectionRequest);
static void processCommandsCallback(int fd, short flags, void *param);
static void startListen(SocketListenParam* socket_listen_p, const std::string& socket_path);
static int blockingWrite(int fd, const void *buffer, size_t len);
static void printBuffer(const void* buffer, size_t bufferLength, std::string& s);
static pthread_mutex_t s_writeMutex = PTHREAD_MUTEX_INITIALIZER;
WakeLock socketWakeLock("SocketWakeLock");
static struct ril_event s_commands_event;
static struct ril_event s_listen_event;
int sendUnsolResponse(int unsolResponse, std::shared_ptr<Marshal> p);
int sendUnsolResponse(int socketFd, int unsolResponse, std::shared_ptr<Marshal> p);

namespace android {
    void releaseWakeLock();
}

static SocketListenParam s_ril_param_socket = {
                        RIL_SOCKET_1,             /* socket_id */
                        -1,                       /* fdListen */
                        -1,                       /* fdCommand */
                        PHONE_PROCESS,            /* processName */
                        &s_commands_event,        /* commands_event */
                        &s_listen_event,          /* listen_event */
                        processCommandsCallback,  /* processCommandsCallback */
                        NULL,                     /* p_rs */
                        RIL_TELEPHONY_SOCKET      /* type */
                        };


static int sendResponseRaw(int socketFd, const void *data, size_t dataSize);

static int sendResponseRaw(int socketFd, const void *data, size_t dataSize) {
    int fd = socketFd;
    int ret;
    pthread_mutex_t* writeMutexHook = &s_writeMutex;

    if (fd < 0) {
        return -1;
    }

    if (dataSize > MAX_COMMAND_BYTES) {
        QCRIL_LOG_ERROR("RIL: packet larger than %u (%u)",
                MAX_COMMAND_BYTES, (unsigned int )dataSize);
        return -1;
    }

    pthread_mutex_lock(writeMutexHook);

    uint32_t header = htonl(dataSize);
    socketWakeLock.acquire();
    ret = blockingWrite(fd, (void*) &header, sizeof(header));

    if (ret < 0) {
        pthread_mutex_unlock(writeMutexHook);
        return ret;
    }

    ret = blockingWrite(fd, data, dataSize);
    socketWakeLock.release();
    if (ret < 0) {
        pthread_mutex_unlock(writeMutexHook);
        return ret;
    }

    pthread_mutex_unlock(writeMutexHook);

    return 0;
}

void notifyClientServerIsReady(int fd) {
    static constexpr const int QcrilRilVersion = 15;
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p && p->write(QcrilRilVersion) == Marshal::Result::SUCCESS) {
        sendUnsolResponse(fd, RIL_UNSOL_RIL_CONNECTED, p);
    }
}

bool getClientCredentials(int client_fd, int &uid, int &pid) {
    bool status = false;
    int val = 1;
    struct ucred cred = {};

    do {
        if (client_fd == -1) {
            break;
        }

        socklen_t len = sizeof(struct ucred);
        if (getsockopt(client_fd, SOL_SOCKET, SO_PEERCRED, &cred, &len) < 0) {
            QCRIL_LOG_DEBUG("failed to get credentials: %s ", strerror(errno));
            break;
        }
        pid = cred.pid;
        uid = cred.uid;
        status = true;
    } while (0);

    return status;
}

static void listenCallback(int fd, short /*flags*/, void *param) {
    int ret;
    int fdCommand = -1;
    RecordStream *p_rs;

    SocketListenParam *p_info = (SocketListenParam*) param;

    struct sockaddr_un peeraddr;
    socklen_t socklen = sizeof(peeraddr);

    assert (p_info->fdCommand < 0);
    assert (fd == p_info->fdListen);

    fdCommand = accept(fd, (sockaddr*) &peeraddr, &socklen);

    if (fdCommand < 0 ) {
        QCRIL_LOG_ERROR("Error on accept() errno: %d", errno);
        /* start listening for new connections again */
        android::rilEventAddWakeup(p_info->listen_event);
        return;
    }

    ret = fcntl(fdCommand, F_SETFL, O_NONBLOCK);

    if (ret < 0) {
        QCRIL_LOG_ERROR("Error setting O_NONBLOCK errno: %d", errno);
    }

    QCRIL_LOG_DEBUG("libril: socket connection %d ", fdCommand);

    //receive client credentials
    //Commenting due to issue in SO_PEERCRED . Will be enabled when
    //enabling authentication in future
    int uid =-1, pid = -1 ;
    /*if (getClientCredentials(fd, uid, pid)) {
    //    QCRIL_LOG_DEBUG("Client UID %d , PID %d ",uid, pid);
    }*/

    //create Client Context
    RilSocketClientController::getInstance().createClientContext(fdCommand, uid, pid);


    //create a new socket param struct for each client
    ril_event *commands_event = new ril_event();
    ril_event *listen_event = new ril_event();
    SocketListenParam *sParam = new SocketListenParam{RIL_SOCKET_1, -1, -1, PHONE_PROCESS,
           commands_event, listen_event, processCommandsCallback, NULL, RIL_TELEPHONY_SOCKET};
    sParam->fdCommand = fdCommand;
    p_rs = record_stream_new(sParam->fdCommand, MAX_COMMAND_BYTES);
    sParam->p_rs = p_rs;

    ril_event_set(sParam->commands_event, sParam->fdCommand, 1,
                  sParam->processCommandsCallback, sParam);
    android::rilEventAddWakeup(sParam->commands_event);

    /*UNSOL_RIL_CONNECTED will be sent when client registers for indication
     * in dispatchRegisterIndicationsRequest()
    */
}

static int createServerSocket(const std::string& name) {
    QCRIL_LOG_DEBUG("Creating socket named %s", name.c_str());

    int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0) {
        QCRIL_LOG_DEBUG("Failed to create socket: %s", strerror(errno));
        return fd;
    }

    struct sockaddr_un socketAddress = {};
    socketAddress.sun_family = AF_LOCAL;
    if (strlcpy(socketAddress.sun_path, name.c_str(),
                sizeof(socketAddress.sun_path)) >= sizeof(socketAddress.sun_path))
    {
        return -1;
    }

    unlink(socketAddress.sun_path);
    if (bind(fd, reinterpret_cast<struct sockaddr*>(&socketAddress), sizeof(socketAddress)) < 0) {
        QCRIL_LOG_DEBUG("Failed to bind socket: %s", strerror(errno));
        close(fd);
        return -1;
    }
    int rc = chmod(name.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    int errc = errno;
    if (rc < 0) {
        QCRIL_LOG_DEBUG("Unable to set permissions on socket with chmod: '%s' (%d)", strerror_r(errc, nullptr, 0), errc);
    }

    return fd;
}

static void startListen(SocketListenParam* socket_listen_p,
        const std::string& socket_path) {
    int fdListen = -1;
    int ret;

    fdListen = createServerSocket(socket_path);
    if (fdListen < 0) {
        QCRIL_LOG_ERROR("Failed to create socket %s", socket_path.c_str());
        return;
    }

    QCRIL_LOG_DEBUG("Start to listen on socket %s.", socket_path.c_str());

    ret = listen(fdListen, 4);
    if (ret < 0) {
        QCRIL_LOG_ERROR("Failed to listen on control socket '%d': %s", fdListen, strerror(errno));
        return;
    }

    socket_listen_p->fdListen = fdListen;

    /* note: persistent so we can accept multiple connections */
    ril_event_set(socket_listen_p->listen_event, fdListen, true,
                  listenCallback, socket_listen_p);

    android::rilEventAddWakeup(socket_listen_p->listen_event);
}

int sendResponse(std::shared_ptr<SocketRequestContext> context, RIL_Errno errorCode, std::shared_ptr<Marshal> p) {
    if (context == nullptr) {
        return -1;
    }

    //get socket fd from SocketRequestContext
    int socketFd = context->getClientFd();
    QCRIL_LOG_DEBUG("sending response on socket %d ", socketFd);

    Marshal responseHeader;
    responseHeader.write(static_cast<uint32_t>(RESPONSE_SOLICITED));
    responseHeader.write(context->getRequestToken());
    responseHeader.write(static_cast<uint32_t>(errorCode));

    if (p != nullptr) {
        std::string rh = responseHeader.toString();
        rh.resize(responseHeader.dataSize());

        std::string rd = p->toString();
        rd.resize(p->dataSize());

        Marshal m;
        m.setData(rh + rd);
        return sendResponseRaw(socketFd, m.data(), m.dataSize());
    } else {
        return sendResponseRaw(socketFd, responseHeader.data(), responseHeader.dataSize());
    }
}

int sendUnsolResponse(int unsolResponse, std::shared_ptr<Marshal> p) {
    int ret = -1;

    //Get the list of registered clients for the unsol indication
    const ClientContextList clientList =
        RilSocketClientController::getInstance().getRegisteredClients(unsolResponse);
    if (clientList.empty()) {
        QCRIL_LOG_DEBUG("No registered client for the indication %d",unsolResponse);
        return -1;
    }

    for (const auto &iter : clientList) {
        const std::shared_ptr<ClientContext> &context = iter;
        if (context == nullptr) {
            continue;
        }

        if (!context->mIsActive) {
            continue;
        }
        int socketFd = context->mSocketFd;
        ret = sendUnsolResponse(socketFd, unsolResponse, p);
    }
   return ret;
}

/*
 * Version of sendUnsolResponse() API to send UNSOL indication
 * to a specific client.
*/
int sendUnsolResponse(int socketFd, int unsolResponse, std::shared_ptr<Marshal> p) {
    int ret = -1;
    QCRIL_LOG_DEBUG("sending indication to client %d ", socketFd);
    Marshal unsolResponseHeader;
    unsolResponseHeader.write(static_cast<uint32_t>(RESPONSE_UNSOLICITED));
    unsolResponseHeader.write(static_cast<int32_t>(unsolResponse));

    if (p != nullptr) {
        std::string urh = unsolResponseHeader.toString();
        urh.resize(unsolResponseHeader.dataSize());

        std::string urd = p->toString();
        urd.resize(p->dataSize());

        Marshal m;
        m.setData(urh + urd);
        ret = sendResponseRaw( socketFd, m.data(), m.dataSize());
    } else {
        ret = sendResponseRaw( socketFd, unsolResponseHeader.data(), unsolResponseHeader.dataSize());
    }
   return ret;
}

static int processCommandBuffer(void *buffer, size_t buflen, RIL_SOCKET_ID socket_id, int fd,
    bool& isDisconnectionRequest) {
    Marshal p;
    int32_t request;
    uint64_t token;

    std::string s;
    printBuffer(buffer, buflen, s);
    QCRIL_LOG_DEBUG("Buffer size: %zu and Contents %s", buflen, s.c_str());

    size_t requestParameterParcelOffset = sizeof(request) + sizeof(token);

    // Buffer is expected to contain at least 12 bytes. First 4 bytes contain
    // the request ID, and the following 8 bytes contain the token.
    if (buflen < requestParameterParcelOffset) {
        QCRIL_LOG_ERROR("Command buffer should contain at least %zu bytes.",
            requestParameterParcelOffset);
        return 0;
    }

    request = *(reinterpret_cast<int32_t*>(buffer));
    token = *(reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(buffer) + sizeof(request)));

    p.setData(std::string(reinterpret_cast<const char*>(buffer) + requestParameterParcelOffset,
                          buflen - requestParameterParcelOffset));

    QCRIL_LOG_DEBUG("client Fd: %d, Request: %d, Token: %d ", fd, request, token);
    // Received an Ack for the previous result sent to RIL.java,
    // so release wakelock and exit
    // TODO: See if this is really needed
    if (request == RIL_RESPONSE_ACKNOWLEDGEMENT) {
        android::releaseWakeLock();
        return 0;
    }

    std::shared_ptr<SocketRequestContext> context =
        std::make_shared<SocketRequestContext>(fd, static_cast<uint8_t>(socket_id), request, token);
    // TODO: Check if context was allocated

    switch (request) {
        case RIL_REQUEST_DIAL:
            ril::socket::api::dispatchDial(context, p);
            break;
        case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG:
            ril::socket::api::dispatchRilRequestGsmGetBroadcastSmsConfig(context, p);
            break;
        case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG:
            ril::socket::api::dispatchRilRequestGsmSetBroadcastSmsConfig(context, p);
            break;
        case RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION:
            ril::socket::api::dispatchRilRequestGsmSmsBroadcastActivation(context, p);
            break;
        case RIL_REQUEST_VOICE_REGISTRATION_STATE:
            ril::socket::api::dispatchGetVoiceRegState(context, p);
            break;
        case RIL_REQUEST_DATA_REGISTRATION_STATE:
            ril::socket::api::dispatchGetDataRegState(context, p);
            break;
        case RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER:
            ril::socket::api::dispatchSetIndicationFilter(context, p);
            break;
        case RIL_REQUEST_HANGUP:
            ril::socket::api::dispatchHangup(context, p);
            break;
        case RIL_REQUEST_SEND_USSD:
            ril::socket::api::dispatchSendUssd(context, p, false);
            break;
        case RIL_REQUEST_CANCEL_USSD:
            ril::socket::api::dispatchCancelUssd(context, p);
            break;
        case RIL_REQUEST_DTMF:
            ril::socket::api::dispatchDtmf(context, p);
            break;
        case RIL_REQUEST_DTMF_START:
            ril::socket::api::dispatchDtmfStart(context, p);
            break;
        case RIL_REQUEST_DTMF_STOP:
            ril::socket::api::dispatchDtmfStop(context, p);
            break;
        case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
            ril::socket::api::dispatchLastCallFailCause(context, p);
            break;
        case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
            ril::socket::api::dispatchSetNetworkType(context, p);
            break;
        case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
            ril::socket::api::dispatchGetNetworkType(context, p);
            break;
        case RIL_REQUEST_GET_CURRENT_CALLS:
            ril::socket::api::dispatchGetCurrentCalls(context, p);
            break;
        case RIL_REQUEST_ANSWER:
            ril::socket::api::dispatchAnswer(context, p);
            break;
        case RIL_REQUEST_CONFERENCE:
            ril::socket::api::dispatchConference(context, p);
            break;
        case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
            ril::socket::api::dispatchSwitchWaitingOrHoldingAndActive(context, p);
            break;
        case RIL_REQUEST_UDUB:
            ril::socket::api::dispatchUdub(context, p);
            break;
        case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
            ril::socket::api::dispatchHangupWaitingOrBackground(context, p);
            break;
        case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
            ril::socket::api::dispatchHangupForegroundResumeBackground(context, p);
            break;
        case RIL_REQUEST_SEPARATE_CONNECTION:
            ril::socket::api::dispatchSeparateConnection(context, p);
            break;
        case RIL_REQUEST_SHUTDOWN:
            ril::socket::api::dispatchShutDown(context, p);
            break;
        case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
            ril::socket::api::dispatchExitEmergencyCbMode(context, p);
            break;
        case RIL_REQUEST_SET_BAND_MODE:
            ril::socket::api::dispatchSetBandMode(context, p);
            break;
        case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE:
            ril::socket::api::dispatchQueryAvailableBandMode(context, p);
            break;
        case RIL_REQUEST_SIGNAL_STRENGTH:
            ril::socket::api::dispatchSignalStrength(context, p);
            break;
        case RIL_REQUEST_RADIO_POWER:
            ril::socket::api::dispatchRadioPower(context, p);
            break;
        case RIL_REQUEST_ALLOW_DATA:
            ril::socket::api::dispatchAllowData(context, p);
            break;
        case RIL_REQUEST_GET_UICC_ENABLEMENT_STATUS:
            ril::socket::api::dispatchGetEnableSimStatus(context, p);
            break;
        case RIL_REQUEST_ENABLE_UICC_APPLICATIONS:
            ril::socket::api::dispatchEnableSim(context, p);
            break;
        case RIL_REQUEST_SET_LOCATION_UPDATES:
            ril::socket::api::dispatchSetLocationUpdates(context, p);
            break;
        case RIL_REQUEST_GET_ACTIVITY_INFO:
            ril::socket::api::dispatchGetActivityInfo(context, p);
            break;
        case RIL_REQUEST_VOICE_RADIO_TECH:
            ril::socket::api::dispatchVoiceRadioTech(context, p);
            break;
        case RIL_REQUEST_DEVICE_IDENTITY:
            ril::socket::api::dispatchDeviceIdentity(context, p);
            break;
        case RIL_REQUEST_OPERATOR:
            ril::socket::api::dispatchOperatorName(context, p);
            break;
        case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE:
            ril::socket::api::dispatchQueryNetworkSelectionMode(context, p);
            break;
        case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC:
            ril::socket::api::dispatchSetNetworkSelectionAutomatic(context, p);
            break;
        case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL:
            ril::socket::api::dispatchSetNetworkSelectionManual(context, p);
            break;
        case RIL_REQUEST_BASEBAND_VERSION:
            ril::socket::api::dispatchBasebandVersion(context, p);
            break;
        case RIL_REQUEST_EXPLICIT_CALL_TRANSFER:
            ril::socket::api::dispatchExplicitCallTransfer(context, p);
            break;
        case RIL_REQUEST_SET_MUTE:
            ril::socket::api::dispatchSetMute(context, p);
            break;
        case RIL_REQUEST_GET_MUTE:
            ril::socket::api::dispatchGetMute(context, p);
            break;
        case RIL_REQUEST_SET_CLIR:
            ril::socket::api::dispatchSetClir(context, p);
            break;
        case RIL_REQUEST_GET_CLIR:
            ril::socket::api::dispatchGetClir(context, p);
            break;
        case RIL_IMS_REQUEST_SEND_RTT_MSG:
            ril::socket::api::dispatchImsSendRttMsg(context, p);
            break;
        case RIL_IMS_REQUEST_SEND_GEOLOCATION_INFO:
            ril::socket::api::dispatchImsSendGeolocationInfo(context, p);
            break;
        case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE:
            ril::socket::api::dispatchSetUnsolCellInfoListRate(context, p);
            break;
        case RIL_REQUEST_SETUP_DATA_CALL:
            ril::socket::api::dispatchSetupDataCall(context, p);
            break;
        case RIL_REQUEST_SETUP_DATA_CALL_URSP:
            ril::socket::api::dispatchSetupDataCall_1_6(context, p);
            break;
        case RIL_REQUEST_START_LCE:
            ril::socket::api::dispatchStartLce(context, p);
            break;
        case RIL_REQUEST_STOP_LCE:
            ril::socket::api::dispatchStopLce(context, p);
            break;
        case RIL_REQUEST_PULL_LCEDATA:
            ril::socket::api::dispatchPullLceData(context, p);
            break;
        case RIL_REQUEST_DEACTIVATE_DATA_CALL:
            ril::socket::api::dispatchDeactivateDataCall(context, p);
            break;
        case RIL_REQUEST_SET_DATA_PROFILE:
            ril::socket::api::dispatchSetDataProfile(context, p);
            break;
        case RIL_REQUEST_DATA_DUMP:
            ril::socket::api::dispatchCaptureRilDataDump(context,p);
            break;
        case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
            ril::socket::api::dispatchSetInitialAttachApn(context, p);
            break;
        case RIL_REQUEST_DATA_CALL_LIST:
            ril::socket::api::dispatchGetDataCallList(context, p);
            break;
            break;
        case RIL_REQUEST_SET_DATA_THROTTLING:
            ril::socket::api::dispatchSetDataThrottling(context, p);
            break;
        case RIL_REQUEST_GET_CELL_INFO_LIST:
            ril::socket::api::dispatchCellInfo(context, p);
            break;
        case RIL_REQUEST_SEND_SMS:
            ril::socket::api::dispatchSendSms(context, p);
            break;
        case RIL_REQUEST_SMS_ACKNOWLEDGE:
            ril::socket::api::dispatchSmsAck(context, p);
            break;
        case RIL_REQUEST_GET_SMSC_ADDRESS:
            ril::socket::api::dispatchGetSmscAddress(context, p);
            break;
        case RIL_REQUEST_SET_SMSC_ADDRESS:
            ril::socket::api::dispatchSetSmscAddress(context, p);
            break;
        case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION:
            ril::socket::api::dispatchEnterNetworkDepersonalization(context, p);
            break;
        case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
            ril::socket::api::dispatchStkSendEnvelopeCommand(context, p);
            break;
        case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM:
            ril::socket::api::dispatchStkHandleCallSetupRequestedFromSim(context, p);
            break;
        case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
            ril::socket::api::dispatchStkSendTerminalResponse(context, p);
            break;
        case RIL_REQUEST_UPDATE_ADN_RECORD:
            ril::socket::api::dispatchUpdateAdnRecord(context, p);
            break;
        case RIL_REQUEST_GET_ADN_RECORD:
            ril::socket::api::dispatchGetAdnRecord(context, p);
            break;
        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
            ril::socket::api::dispatchStkServiceIsRunning(context, p);
            break;
        case RIL_REQUEST_QUERY_TTY_MODE:
            ril::socket::api::dispatchGetTtyMode(context, p);
            break;
        case RIL_REQUEST_SET_TTY_MODE:
            ril::socket::api::dispatchSetTtyMode(context, p);
            break;
        case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE:
            ril::socket::api::dispatchGetVoicePrivacyMode(context, p);
            break;
        case RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE:
            ril::socket::api::dispatchSetVoicePrivacyMode(context, p);
            break;
        case RIL_REQUEST_CDMA_FLASH:
            ril::socket::api::dispatchCdmaFlash(context, p);
            break;
        case RIL_REQUEST_CDMA_BURST_DTMF:
            ril::socket::api::dispatchCdmaBurstDtmf(context, p);
            break;
        case RIL_REQUEST_IMS_REGISTRATION_STATE:
            ril::socket::api::dispatchGetImsRegistration(context, p);
            break;
        case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS:
            ril::socket::api::dispatchQueryAvailableNetworks(context, p);
            break;
        case RIL_REQUEST_START_NETWORK_SCAN:
            ril::socket::api::dispatchStartNetworkScanRequest(context, p);
            break;
        case RIL_REQUEST_STOP_NETWORK_SCAN:
            ril::socket::api::dispatchStopNetworkScanRequest(context, p);
            break;
        case RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS:
            ril::socket::api::dispatchGetSystemSelectionChannels(context, p);
            break;
        case RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS:
            ril::socket::api::dispatchSetSystemSelectionChannels(context, p);
            break;
        case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:
            ril::socket::api::dispatchQueryCallForwardStatus(context, p);
            break;
        case RIL_REQUEST_SET_CALL_FORWARD:
            ril::socket::api::dispatchSetCallForward(context, p);
            break;
        case RIL_REQUEST_QUERY_CALL_WAITING:
            ril::socket::api::dispatchQueryCallWaiting(context, p);
            break;
        case RIL_REQUEST_SET_CALL_WAITING:
            ril::socket::api::dispatchSetCallWaiting(context, p);
            break;
        case RIL_REQUEST_CHANGE_BARRING_PASSWORD:
            ril::socket::api::dispatchChangeBarringPassword(context, p);
            break;
        case RIL_REQUEST_QUERY_CLIP:
            ril::socket::api::dispatchQueryClip(context, p);
            break;
        case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION:
            ril::socket::api::dispatchSetSuppSvcNotification(context, p);
            break;
        case RIL_REQUEST_GET_IMSI:
            ril::socket::api::dispatchSimGetImsiReq(context, p);
            break;
        case RIL_REQUEST_OEM_HOOK_RAW:
            ril::socket::api::dispatchOemhookRaw(context, p);
            break;
        case RIL_REQUEST_SIM_IO:
            ril::socket::api::dispatchSimIOReq(context, p);
            break;
        case RIL_REQUEST_GET_SIM_STATUS:
            ril::socket::api::dispatchSimGetSimStatusReq(context, p);
            break;
        case RIL_REQUEST_SIM_OPEN_CHANNEL:
            ril::socket::api::dispatchSimOpenChannelReq(context, p);
            break;
        case RIL_REQUEST_SIM_CLOSE_CHANNEL:
            ril::socket::api::dispatchSimCloseChannelReq(context, p);
            break;
        case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC:
            ril::socket::api::dispatchSimTransmitApduBasicReq(context, p);
            break;
        case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL:
            ril::socket::api::dispatchSimTransmitApduChannelReq(context, p);
            break;
        case RIL_REQUEST_CHANGE_SIM_PIN:
            ril::socket::api::dispatchSimChangePinReq(context, p);
            break;
        case RIL_REQUEST_CHANGE_SIM_PIN2:
            ril::socket::api::dispatchSimChangePin2Req(context, p);
            break;
        case RIL_REQUEST_ENTER_SIM_PUK:
            ril::socket::api::dispatchSimEnterPukReq(context, p);
            break;
        case RIL_REQUEST_ENTER_SIM_PUK2:
            ril::socket::api::dispatchSimEnterPuk2Req(context, p);
            break;
        case RIL_REQUEST_ENTER_SIM_PIN:
            ril::socket::api::dispatchSimEnterPinReq(context, p);
            break;
        case RIL_REQUEST_ENTER_SIM_PIN2:
            ril::socket::api::dispatchSimEnterPin2Req(context, p);
            break;
        case RIL_REQUEST_QUERY_FACILITY_LOCK:
            ril::socket::api::dispatchSimQueryFacilityLockReq(context, p);
            break;
        case RIL_REQUEST_SET_FACILITY_LOCK:
            ril::socket::api::dispatchSimSetFacilityLockReq(context, p);
            break;
        case RIL_REQUEST_ISIM_AUTHENTICATION:
            ril::socket::api::dispatchSimIsimAuthenticationReq(context, p);
            break;
        case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS:
            // TODO - Find the right UIM module message
            ril::socket::api::dispatchSimStkSendEnvelopeWithStatusReq(context, p);
            break;
        case RIL_REQUEST_SIM_AUTHENTICATION:
            ril::socket::api::dispatchSimAuthenticationReq(context, p);
            break;
            // IMS Requests
        case RIL_IMS_REQUEST_GET_REGISTRATION_STATE:
            ril::socket::api::dispatchImsGetRegistrationState(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_SERVICE_STATUS:
            ril::socket::api::dispatchImsQueryServiceStatus(context, p);
            break;
        case RIL_IMS_REQUEST_REGISTRATION_CHANGE:
            ril::socket::api::dispatchImsRegistrationChange(context, p);
            break;
        case RIL_IMS_REQUEST_SET_SERVICE_STATUS:
            ril::socket::api::dispatchImsSetServiceStatus(context, p);
            break;
        case RIL_IMS_REQUEST_SUPP_SVC_STATUS:
            ril::socket::api::dispatchImsSuppSvcStatus(context, p);
            break;
        case RIL_IMS_REQUEST_GET_IMS_SUB_CONFIG:
            ril::socket::api::dispatchImsGetImsSubConfig(context, p);
            break;
        case RIL_REQUEST_SET_LINK_CAP_FILTER:
            ril::socket::api::dispatchSetLinkCapFilter(context, p);
            break;
        case RIL_REQUEST_SET_LINK_CAP_RPT_CRITERIA:
            ril::socket::api::dispatchSetLinkCapRptCriteria(context, p);
            break;
        case RIL_REQUEST_GET_IMEI:
          ril::socket::api::dispatchGetImei(context, p);
          break;
        case RIL_IMS_REQUEST_DIAL:
            ril::socket::api::dispatchImsDial(context, p);
            break;
        case RIL_IMS_REQUEST_ANSWER:
            ril::socket::api::dispatchImsAnswer(context, p);
            break;
        case RIL_IMS_REQUEST_HANGUP:
            ril::socket::api::dispatchImsHangup(context, p);
            break;
        case RIL_IMS_REQUEST_DTMF:
            ril::socket::api::dispatchImsDtmf(context, p);
            break;
        case RIL_IMS_REQUEST_DTMF_START:
            ril::socket::api::dispatchImsDtmfStart(context, p);
            break;
        case RIL_IMS_REQUEST_DTMF_STOP:
            ril::socket::api::dispatchImsDtmfStop(context, p);
            break;
        case RIL_IMS_REQUEST_MODIFY_CALL_INITIATE:
            ril::socket::api::dispatchImsModifyCallInitiate(context, p);
            break;
        case RIL_IMS_REQUEST_MODIFY_CALL_CONFIRM:
            ril::socket::api::dispatchImsModifyCallConfirm(context, p);
            break;
        case RIL_IMS_REQUEST_CANCEL_MODIFY_CALL:
            ril::socket::api::dispatchImsCancelModifyCall(context, p);
            break;
        case RIL_IMS_REQUEST_ADD_PARTICIPANT:
            ril::socket::api::dispatchImsAddParticipant(context, p);
            break;
        case RIL_IMS_REQUEST_SET_IMS_CONFIG:
            ril::socket::api::dispatchImsSetConfig(context, p);
            break;
        case RIL_IMS_REQUEST_GET_IMS_CONFIG:
            ril::socket::api::dispatchImsGetConfig(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_CALL_FORWARD_STATUS:
            ril::socket::api::dispatchImsQueryCallForwardStatus(context, p);
            break;
        case RIL_IMS_REQUEST_SET_CALL_FORWARD_STATUS:
            ril::socket::api::dispatchImsSetCallForwardStatus(context, p);
            break;
        case RIL_IMS_REQUEST_EXPLICIT_CALL_TRANSFER:
            ril::socket::api::dispatchImsExplicitCallTransfer(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_CLIP:
            ril::socket::api::dispatchImsQueryClip(context, p);
            break;
        case RIL_IMS_REQUEST_GET_CLIR:
            ril::socket::api::dispatchImsGetClir(context, p);
            break;
        case RIL_IMS_REQUEST_SET_CLIR:
            ril::socket::api::dispatchImsSetClir(context, p);
            break;
        case RIL_IMS_REQUEST_DEFLECT_CALL:
            ril::socket::api::dispatchImsDeflectCall(context, p);
            break;
        case RIL_IMS_REQUEST_SEND_UI_TTY_MODE:
            ril::socket::api::dispatchImsSendUiTtyMode(context, p);
            break;
        case RIL_IMS_REQUEST_SET_SUPP_SVC_NOTIFICATION:
            ril::socket::api::dispatchImsSetSuppSvcNotification(context, p);
            break;
        case RIL_IMS_REQUEST_CONFERENCE:
            ril::socket::api::dispatchImsRequestConference(context, p);
            break;
        case RIL_IMS_REQUEST_HOLD:
            ril::socket::api::dispatchImsRequestHold(context, p);
            break;
        case RIL_IMS_REQUEST_RESUME:
            ril::socket::api::dispatchImsRequestResume(context, p);
            break;
        case RIL_IMS_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
            ril::socket::api::dispatchImsExitEmergencyCallbackMode(context, p);
            break;
        case RIL_IMS_REQUEST_SEND_SMS:
            ril::socket::api::imsSendSms(context, p);
            break;
        case RIL_IMS_REQUEST_ACK_SMS:
            ril::socket::api::imsAckSms(context, p);
            break;
        case RIL_IMS_REQUEST_GET_SMS_FORMAT:
            ril::socket::api::imsGetSmsFormat(context, p);
            break;
        case RIL_IMS_REQUEST_SET_CALL_WAITING:
            ril::socket::api::imsSetCallWaiting(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_CALL_WAITING:
            ril::socket::api::imsQueryCallWaiting(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_MULTI_SIM_VOICE_CAPABILITY:
          ril::socket::api::dispatchImsQueryMultiSimVoiceCapability(context, p);
          break;
        case RIL_REQUEST_GET_RADIO_CAPABILITY:
            ril::socket::api::dispatchGetRadioCapability(context, p);
            break;
        case RIL_REQUEST_SET_RADIO_CAPABILITY:
            ril::socket::api::dispatchSetRadioCapability(context, p);
            break;
        case RIL_REQUEST_CDMA_SEND_SMS:
            ril::socket::api::dispatchSendCdmaSms(context, p);
            break;
        case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE:
            ril::socket::api::dispatchAckCdmaSms(context, p);
            break;
        case RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG:
            ril::socket::api::dispatchCdmaGetBroadcastSmsConfig(context, p);
            break;
        case RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG:
            ril::socket::api::dispatchCdmaSetBroadcastSmsConfig(context, p);
            break;
        case RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION:
            ril::socket::api::dispatchCdmaSmsBroadcastActivation(context, p);
            break;
        case RIL_REQUEST_SET_UNSOLICITED_DATA_RESPONSE_FILTER:
            ril::socket::api::dispatchRegisterDataUnsolIndication(context, p);
            break;
        case RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM:
            ril::socket::api::dispatchCdmaWriteSmsToRuim(context, p);
            break;
        case RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM:
            ril::socket::api::dispatchCdmaDeleteSmsOnRuim(context, p);
            break;
        case RIL_REQUEST_CDMA_SUBSCRIPTION:
            ril::socket::api::dispatchCdmaSubscription(context, p);
            break;
        case RIL_REQUEST_WRITE_SMS_TO_SIM:
            ril::socket::api::dispatchWriteSmsToSim(context, p);
            break;
        case RIL_REQUEST_DELETE_SMS_ON_SIM:
            ril::socket::api::dispatchDeleteSmsOnSim(context, p);
            break;
        case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS:
            ril::socket::api::reportSmsMemoryStatus(context, p);
            break;
        case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE:
            ril::socket::api::dispatchCdmaSetSubscriptionSource(context, p);
            break;
        case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE:
            ril::socket::api::dispatchCdmaGetSubscriptionSource(context, p);
            break;
        case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE:
            ril::socket::api::dispatchCdmaSetRoamingPreference(context, p);
            break;
        case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE:
            ril::socket::api::dispatchCdmaGetRoamingPreference(context, p);
            break;
        case RIL_REQUEST_GET_DATA_NR_ICON_TYPE:
            ril::socket::api::dispatchGetDataNrIconType(context, p);
            break;
        case RIL_REQUEST_SET_PREFERRED_DATA_MODEM:
            ril::socket::api::dispatchSetPreferredDataModem(context, p);
            break;
        case RIL_REQUEST_START_KEEPALIVE:
            ril::socket::api::dispatchStartKeepAlive(context, p);
            break;
        case RIL_REQUEST_STOP_KEEPALIVE:
            ril::socket::api::dispatchStopKeepAlive(context, p);
            break;
        case RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION:
            ril::socket::api::dispatchCarrierInfoImsiEncryption(context, p);
            break;
        case RIL_IMS_REQUEST_GET_COLR:
            ril::socket::api::dispatchImsGetColr(context, p);
            break;
        case RIL_IMS_REQUEST_SET_COLR:
            ril::socket::api::dispatchImsSetColr(context, p);
            break;
        case RIL_IMS_REQUEST_REGISTER_MULTI_IDENTITY_LINES:
            ril::socket::api::dispatchImsRegisterMultiIdentityLines(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_VIRTUAL_LINE_INFO:
            ril::socket::api::dispatchImsQueryVirtualLineInfo(context, p);
            break;
        case RIL_REQUEST_SIM_GET_ATR:
            ril::socket::api::dispatchSimGetAtr(context, p);
            break;
        case RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA:
            ril::socket::api::setSignalStrengthReportingCriteria(context, p);
            break;
        case RIL_REQUEST_DISCONNECT:
            ril::socket::api::handleClientDisconnectionRequest(context, p);
            isDisconnectionRequest = true;
            break;
        case RIL_IMS_REQUEST_SEND_USSD:
            ril::socket::api::dispatchSendUssd(context, p, true);
            break;
        case RIL_IMS_REQUEST_CANCEL_USSD:
             ril::socket::api::dispatchCancelUssd(context, p);
             break;
        case RIL_SEND_LPA_USER_REQUEST:
          ril::socket::api::sendlpauserrequest(context, p);
          break;
        case RIL_SEND_LPA_HTTP_REQUEST:
          ril::socket::api::sendlpahttprequest(context, p);
          break;
        case RIL_REQUEST_GET_USAGE_SETTING:
          ril::socket::api::dispatchGetUsageSetting(context, p);
          break;
        case RIL_REQUEST_SET_USAGE_SETTING:
          ril::socket::api::dispatchSetUsageSetting(context, p);
          break;
        case RIL_REQUEST_GET_SLICING_CONFIG:
          ril::socket::api::dispatchGetSlicingConfigRequest(context, p);
          break;
        case RIL_REQUEST_REGISTER_INDICATIONS:
          ril::socket::api::dispatchRegisterIndicationsRequest(context, p);
          break;
        case RIL_CALL_REQUEST_DEFLECT_CALL:
          ril::socket::api::voice::dispatchDeflectCall(context, p);
          break;
        case RIL_CALL_REQUEST_QUERY_CALL_FORWARD_STATUS:
          ril::socket::api::voice::dispatchQueryCallForwardStatus(context, p);
          break;
        case RIL_CALL_REQUEST_SET_CALL_FORWARD:
          ril::socket::api::voice::dispatchSetCallForward(context, p);
          break;
        case RIL_CALL_REQUEST_QUERY_CALL_WAITING:
          ril::socket::api::voice::dispatchQueryCallWaiting(context, p);
          break;
        case RIL_CALL_REQUEST_SET_CALL_WAITING:
          ril::socket::api::voice::dispatchSetCallWaiting(context, p);
          break;
        case RIL_CALL_REQUEST_CHANGE_BARRING_PASSWORD:
          ril::socket::api::voice::dispatchChangeBarringPassword(context, p);
          break;
        case RIL_CALL_REQUEST_LAST_CALL_FAIL_CAUSE:
          ril::socket::api::voice::dispatchLastCallFailCause(context, p);
          break;
        case RIL_CALL_REQUEST_SEND_USSD:
          ril::socket::api::voice::dispatchSendUssd(context, p);
          break;
        case RIL_CALL_REQUEST_CANCEL_USSD:
          ril::socket::api::voice::dispatchCancelUssd(context, p);
          break;
        case RIL_CALL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
          ril::socket::api::voice::dispatchExitEmergencyCallbackMode(context, p);
          break;
        case RIL_CALL_REQUEST_QUERY_CLIP:
          ril::socket::api::voice::dispatchQueryClip(context, p);
          break;
        case RIL_CALL_REQUEST_GET_CLIR:
          ril::socket::api::voice::dispatchGetClir(context, p);
          break;
        case RIL_CALL_REQUEST_SET_CLIR:
          ril::socket::api::voice::dispatchSetClir(context, p);
          break;
        case RIL_CALL_REQUEST_SET_SUPP_SVC_NOTIFICATION:
          ril::socket::api::voice::dispatchSetSuppSvcNotification(context, p);
          break;
        case RIL_CALL_REQUEST_SUPP_SVC_STATUS:
          ril::socket::api::voice::dispatchSuppSvcStatus(context, p);
          break;
        case RIL_CALL_REQUEST_SEPARATE_CONNECTION:
          ril::socket::api::voice::dispatchSeparateConnection(context, p);
          break;
        case RIL_CALL_REQUEST_QUERY_TTY_MODE:
          ril::socket::api::voice::dispatchGetTtyMode(context, p);
          break;
        case RIL_CALL_REQUEST_SET_TTY_MODE:
          ril::socket::api::voice::dispatchSetTtyMode(context, p);
          break;
        case RIL_CALL_REQUEST_HOLD:
          ril::socket::api::voice::dispatchHold(context, p);
          break;
        case RIL_CALL_REQUEST_RESUME:
          ril::socket::api::voice::dispatchResume(context, p);
          break;
        case RIL_CALL_REQUEST_GET_COLR:
          ril::socket::api::voice::dispatchGetColr(context, p);
          break;
        case RIL_CALL_REQUEST_SET_COLR:
          ril::socket::api::voice::dispatchSetColr(context, p);
          break;
        case RIL_CALL_REQUEST_ADD_PARTICIPANT:
          ril::socket::api::voice::dispatchAddParticipant(context, p);
          break;
        case RIL_CALL_REQUEST_MODIFY_CALL_INITIATE:
          ril::socket::api::voice::dispatchModifyCallInitiate(context, p);
          break;
        case RIL_CALL_REQUEST_MODIFY_CALL_CONFIRM:
          ril::socket::api::voice::dispatchModifyCallConfirm(context, p);
          break;
        case RIL_CALL_REQUEST_CANCEL_MODIFY_CALL:
          ril::socket::api::voice::dispatchCancelModifyCall(context, p);
          break;
        case RIL_CALL_REQUEST_SEND_UI_TTY_MODE:
          ril::socket::api::voice::dispatchSendUiTtyMode(context, p);
          break;
        case RIL_CALL_REQUEST_SEND_RTT_MSG:
          ril::socket::api::voice::dispatchSendRttMsg(context, p);
          break;
        case RIL_CALL_REQUEST_DTMF:
          ril::socket::api::voice::dispatchDtmf(context, p);
          break;
        case RIL_CALL_REQUEST_DTMF_START:
          ril::socket::api::voice::dispatchDtmfStart(context, p);
          break;
        case RIL_CALL_REQUEST_DTMF_STOP:
          ril::socket::api::voice::dispatchDtmfStop(context, p);
          break;
        case RIL_CALL_REQUEST_EXPLICIT_CALL_TRANSFER:
          ril::socket::api::voice::dispatchExplicitCallTransfer(context, p);
          break;
        case RIL_CALL_REQUEST_CONFERENCE:
          ril::socket::api::voice::dispatchConference(context, p);
          break;
        case RIL_CALL_REQUEST_DIAL:
          ril::socket::api::voice::dispatchDial(context, p);
          break;
        case RIL_CALL_REQUEST_ANSWER:
          ril::socket::api::voice::dispatchAnswer(context, p);
          break;
        case RIL_CALL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
          ril::socket::api::voice::dispatchSwitchWaitingOrHoldingAndActive(context, p);
          break;
        case RIL_CALL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
          ril::socket::api::voice::dispatchHangupWaitingOrBackground(context, p);
          break;
        case RIL_CALL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
          ril::socket::api::voice::dispatchHangupForegroundResumeBackground(context, p);
          break;
        case RIL_CALL_REQUEST_HANGUP:
          ril::socket::api::voice::dispatchHangup(context, p);
          break;
        case RIL_CALL_REQUEST_UDUB:
          ril::socket::api::voice::dispatchUdub(context, p);
          break;
        case RIL_CALL_REQUEST_GET_CURRENT_CALLS:
          ril::socket::api::voice::dispatchGetCurrentCalls(context, p);
          break;
        case RIL_REQUEST_GET_MODULE_LIST:
          ril::socket::api::dispatchGetModuleList(context, p);
          break;
        case RIL_REQUEST_GET_CONFIG_LIST:
          ril::socket::api::dispatchGetConfigList(context, p);
          break;
        case RIL_REQUEST_MOD_GET_CURRENT_CONFIG:
          ril::socket::api::dispatchGetCurrentConfig(context, p);
          break;
        case RIL_REQUEST_MOD_SET_CURRENT_CONFIG:
          ril::socket::api::dispatchSetCurrentConfig(context, p);
          break;
        case RIL_REQUEST_MOD_SET_CUSTOM_CFG_MODULES:
          ril::socket::api::dispatchSetCustomConfigModules(context, p);
          break;
        default:
          QCRIL_LOG_ERROR("Unsupported request (%d) token %" PRIx64 "x", request, token);
          RLOGE("Unsupported request (%d) token %" PRIx64 "x", request, token);
          sendResponse(context, RIL_E_REQUEST_NOT_SUPPORTED, nullptr);
          break;
    }

    return 0;
}

static int blockingWrite(int fd, const void *buffer, size_t len) {
    size_t writeOffset = 0;
    const uint8_t *toWrite;

    toWrite = (const uint8_t *)buffer;

    while (writeOffset < len) {
        ssize_t written;
        do {
            written = write (fd, toWrite + writeOffset,
                                len - writeOffset);
        } while (written < 0 && ((errno == EINTR) || (errno == EAGAIN)));

        if (written >= 0) {
            writeOffset += written;
        } else {   // written < 0
            QCRIL_LOG_ERROR("RIL Response: unexpected error on write errno:%d", errno);
            close(fd);
            return -1;
        }
    }

    QCRIL_LOG_DEBUG("RIL Response bytes written:%zu", writeOffset);

    return 0;
}

static void processCommandsCallback(int fd, short /*flags*/, void *param) {
    RecordStream* p_rs;
    void* p_record;
    size_t recordlen;
    int ret;
    bool isDisconnectionRequest = false;
    SocketListenParam* p_info = (SocketListenParam*) param;

    if (p_info == NULL) {
      RLOGE("param pointer is NULL");
      return;
    }

    assert(fd == p_info->fdCommand);

    p_rs = p_info->p_rs;

    for (;;) {
        /* loop until EAGAIN/EINTR, end of stream, or other error */
        ret = record_stream_get_next(p_rs, &p_record, &recordlen);

        if (ret == 0 && p_record == NULL) {
            /* end-of-stream */
            break;
        } else if (ret < 0) {
            break;
        } else if (ret == 0) {
            processCommandBuffer(p_record, recordlen, p_info->socket_id, fd,
                isDisconnectionRequest);
        }

        if (isDisconnectionRequest) {
            break;
        }
    }

    if (isDisconnectionRequest || ret == 0 || !(errno == EAGAIN || errno == EINTR)) {
        /* fatal error or end-of-stream or disconnection request from client */
        if (isDisconnectionRequest) {
            QCRIL_LOG_DEBUG("Client requested to disconnect. Closing command socket.");
        } else if (ret != 0) {
            QCRIL_LOG_ERROR("error on reading command socket errno: %s", strerror(errno));
        } else {
            QCRIL_LOG_ERROR("EOS. Closing command socket: %s", strerror(errno));
        }

        // TODO: Check if the client will be notified of the disconnection
        close(fd);
        p_info->fdCommand = -1;

        //clean client context
        RilSocketClientController::getInstance().ClearClientContext(fd);

        ril_event_del(p_info->commands_event);

        record_stream_free(p_rs);

        if (p_info) {
            delete p_info->commands_event;
            delete p_info->listen_event;
            delete p_info;
        }
    }
}

void printBuffer(const void* buffer, size_t bufferLength, std::string& s) {
    char hex[4];
    for (int i = 0; i < bufferLength; i++) {
        std::snprintf(hex, sizeof(hex), "%-3.2hhX", *(static_cast<const uint8_t*>(buffer) + i));
        s += hex;
    }
}

void socket_start(qcril_instance_id_e_type instance_id) {
    // start listening for connections from clients
    startListen(&s_ril_param_socket, std::string(RIL_SOCKET_PATH) +
            std::to_string(instance_id));
}
