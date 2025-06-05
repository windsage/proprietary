/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <log/log.h>
#include "xpan_ac_int.h"
#include <cutils/properties.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.bluetooth.xpan_ac.XpanAcStateMachine"

using xpan::implementation::XpanQhciAcIf;
using xpan::implementation::XMXacIf;

/* Xpan Application Controller State Machine Implementation.*/
namespace xpan {
namespace ac {

XpanAcStateMachine::XpanAcStateMachine(XpanDevice *dev, bdaddr_t bd_addr, XpanEarbudRole role,
                                       TransportType transport) {
  mCurrentState = XPAN_IDLE;
  mPrevState = XPAN_IDLE;
  mEarbudRole = role;
  mBdAddr = bd_addr;
  mTransportFrom = transport;
  mSocketFd = -1;
  mDevice = dev;
  lmp = new XpanLmpManager();
  qhci = XpanQhciAcIf::GetIf();
  xm = XMXacIf::GetIf();
  mSocketHdlr = new XpanSocketHandler(mBdAddr, role);
  lmp->setXpanSocketHandler(mSocketHdlr);
  mBearerSwitchState = -1;

  if (mEarbudRole == ROLE_PRIMARY) {
    int slen = 12;
    const char *s = "[Primary-EB]";
    memcpy(R, s, slen);
  } else {
    int slen = 14;
    const char *s = "[Secondary-EB]";
    memcpy(R, s, slen);
  }

  char xpan_prop[PROPERTY_VALUE_MAX];
  isPingEnabled = false;
  property_get("persist.vendor.qcom.btadvaudio.target.support.xpan_ping", xpan_prop, "true");
  if (!strcmp(xpan_prop, "true")) {
    ALOGI("%s %s: LMP Ping enabled", R, __func__);
    isPingEnabled = true;
  } else {
    ALOGI("%s %s: LMP Ping not enabled", R, __func__);
  }
}

XpanAcStateMachine::~XpanAcStateMachine() {
  ALOGD("%s %s: BDADDR(%s), IP(%s) Role(%d)", R, __func__, mBdAddr.toString().c_str(),
       mIpAddr.toString().c_str(), mEarbudRole);

  if (mDnsQueryTimer) {
    ALOGD("%s %s: Stopping mDnsQueryTimer", R, __func__);
    stopTimer(mDnsQueryTimer);
    mDnsQueryTimer = NULL;
  }

  if (mFilteredScanTimer) {
    ALOGD("%s %s: Stopping mDnsQueryTimer", R, __func__);
    stopTimer(mFilteredScanTimer);
    mFilteredScanTimer = NULL;
  }

  if (mSocketHdlr) {
    ALOGD("%s %s: cleaning up Socket Handler", R, __func__);
    delete mSocketHdlr;
    mSocketHdlr = NULL;
  }

  if (lmp) {
    ALOGD("%s %s: cleaning up Socket Handler", R, __func__);
    delete lmp;
    lmp = NULL;
  }

  mDevice = NULL;
}

bool XpanAcStateMachine::GetRemoteApDetails() {
  ALOGD("%s", __func__);

  char ip_prop[PROPERTY_VALUE_MAX];
  property_get("persist.vendor.qcom.btadvaudio.target.support.xpan_remote_ip", ip_prop, "0.0.0.0");
  ipaddr_t remote_ip;
  ipStringToIpAddr(ip_prop, &remote_ip);

  if (remote_ip.isEmpty()) {
    ALOGE("%s: Invalid ip. Connection will timeout", __func__);
    return false;
  }

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (xac) {
    XmUpdateRemoteApParams *params =
      (XmUpdateRemoteApParams *) malloc (sizeof(XmUpdateRemoteApParams));
    memset(params, 0, sizeof(XmUpdateRemoteApParams));
    params->event = XM_UPDATE_REMOTE_AP_PARAMS_EVT;
    params->addr = mBdAddr;
    params->is_mdns_update = true;
    params->remote_tcp_port = 51236;
    params->num_of_earbuds = 1;

    for (int i = 0; i < params->num_of_earbuds; i++) {
      params->role[i] = ROLE_PRIMARY;
      params->ip[i] = remote_ip;
    }
    xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  }
  return true;
}

void XpanAcStateMachine::XpanAcSmExecuteEvent(xac_handler_msg_t* msg) {
  switch(mCurrentState) {
    case XPAN_IDLE:
      XpanIdleStateHandler(msg);
      break;

    case XPAN_TCP_CONNECTING:
      XpanTcpConnectingStateHandler(msg);
      break;

    case XPAN_TCP_CONNECTED:
      XpanTcpConnectedStateHandler(msg);
      break;

    case XPAN_LMP_CONNECTING:
      XpanLmpConnectingStateHandler(msg);
      break;

    case XPAN_LMP_CONNECTED:
      XpanLmpConnectedStateHandler(msg);
      break;

    case XPAN_BEARER_SWITCH_PENDING:
      XpanBearerSwitchPendingStateHandler(msg);
      break;

    case XPAN_AP_ACTIVE:
      XpanApActiveStateHandler(msg);
      break;

    case XPAN_DISCONNECTING:
      XpanDisconectingStateHandler(msg);
  }
}

void XpanAcStateMachine::XpanIdleStateHandler(xac_handler_msg_t* msg) {
  XacEvent evt = msg->event;
  ALOGD("%s State: XPAN_IDLE, ProcessEvent = %s", R, xpan_event_str(msg->event));

  switch(evt) {
    case QHCI_CREATE_CONNECTION_EVT:
         mTransitionType = XPAN_RECONNECTION_OUTGOING;
         if (mEarbudRole == ROLE_PRIMARY && !HandleInitReconnection()) {
           ALOGE("%s %s: Connection Initiation Failed", R, __func__);
           return;
         }
         setState(XPAN_TCP_CONNECTING, evt);
         break;

    case QHCI_CREATE_CONNECTION_CANCEL_EVT:
         if (qhci && mEarbudRole == ROLE_PRIMARY) {
           qhci->ConnectionCompleteRes(mBdAddr, UNKNOWN_CONNECTION_IDENTIFIER);
         }
         break;

    case XM_BEARER_PREFERENCE_EVT:
         if (msg->bearerPreference.bearer != XPAN_AP) {
           ALOGE("%s: Ignore Bearer Preference to %d in IDLE State",
                 __func__, msg->bearerPreference.bearer);
           return;
         }
         mTransitionType = XPAN_LE_TO_AP_IDLE;
         mBearerPrefTimer = new XpanAcTimer("BearerPreferenceTimer", BearerPreferenceTimeout, this);
         mBearerPrefTimer->StartTimer(XPAN_AC_BEARER_PREFERENCE_TIMER);
         setState(XPAN_TCP_CONNECTING, evt);
         /* Initiate TCP Connection */
         mSocketHdlr->InitTcpConnection(mSocketHdlr, mIpAddr, mTcpPort);
         break;

    case XM_PREPARE_BEARER_EVT:
         if (msg->prepareBearer.bearer != XPAN_AP) {
           ALOGE("%s: Ignore prepare bearer to %d in IDLE State",
                 __func__, msg->bearerPreference.bearer);
           return;
         }
         mTransitionType = XPAN_LE_TO_AP_STREAMING;
         setState(XPAN_TCP_CONNECTING, evt);
         /* Initiate TCP Connection */
         mSocketHdlr->InitTcpConnection(mSocketHdlr, mIpAddr, mTcpPort);
         break;

    case XPAN_TCP_CONNECTED_EVT:
        mTransitionType = XPAN_RECONNECTION_INCOMING;
        /* Check for Incoming connection */
        if (msg->tcpConnected.isIncoming) {
          mTransitionType = XPAN_RECONNECTION_INCOMING;
        }
        break;

    default:
        ALOGE("%s %s: Unanticipated Event %d", R, __func__, evt);
  }

}

void XpanAcStateMachine::XpanTcpConnectingStateHandler(xac_handler_msg_t* msg) {
  XacEvent evt = msg->event;
  ALOGD("%s State: XPAN_TCP_CONNECTING, ProcessEvent = %s", R, xpan_event_str(msg->event));

  switch(evt) {
    case QHCI_CREATE_CONNECTION_CANCEL_EVT: {
         /* Steps: (Handled in XPAN_DISCONNECTING)
           1. close fd for outgoing/incoming connection.
           2. Inform XP to stop MDNS query
           2. Callback to QHCI that connection is cancelled (XPAN_DISCONNECTING State).*/
         setState(XPAN_DISCONNECTING, evt);
         if (xm) xm->TriggerMdnsQuery(mBdAddr, false);
         XpanAcSmExecuteEvent(msg);
        } break;

    case XM_UPDATE_REMOTE_AP_PARAMS_EVT:
        /* Note: Received for Outgoing reconnection */
        /* Steps:
         * 1. Stop Mdns Timer
         * 2. Stop Listening if no pending connection
         * 3. Initiate Connection
         */
        if (mEarbudRole == ROLE_PRIMARY) {
          if (mDnsQueryTimer) {
            stopTimer(mDnsQueryTimer);
            mDnsQueryTimer = NULL;
          }
          /* TODO: Step 2: Stop Listening if no pending connection */
        }
        /* Applicable for both roles */
        HandleRemoteApDetailsUpdate(msg);
        break;

    case XM_MDNS_DISCOVERY_STATUS_EVT: {
         uint8_t status = msg->mdnsDiscoveryStatus.status;
         MDNS_OP state = msg->mdnsDiscoveryStatus.state;
         if ((state == MDNS_QUERY_START &&
               status == MDNS_DISCOVERY_FAILED_TO_START) ||
             (state == MDNS_QUERY_STOP)) {
           if (mDnsQueryTimer) {
             stopTimer(mDnsQueryTimer);
             mDnsQueryTimer = NULL;
           }
           if (XpanSocketHandler::IsListeningOnTcp()) {
             ALOGD("%s %s: Waiting for incoming connection now", R, __func__);
             mTransitionType = XPAN_RECONNECTION_INCOMING;
           }
         }
        } break;

    case XPAN_TCP_CONNECTED_EVT: {
         setState(XPAN_TCP_CONNECTED, evt);
         XpanTcpConnectedEvt *tcpConn = (XpanTcpConnectedEvt *)msg;
         mSocketFd = tcpConn->fd;
         if (!tcpConn->isIncoming) {
           /* Outgoing Connection */
           //if (tlsNotSupported) { // Init Lmp Host Connection
           mSocketHdlr->CloseListeningSocket(mBdAddr);
           XpanLmpOutgoingConnReq outLmpConnReq{XPAN_LMP_OUT_CONNECTION_REQ,
              mIpAddr, mTcpPort, mLocalUuid, BT_LE, {/* COD - Set in TCP Connected state */}};
           XpanAcSmExecuteEvent((xac_handler_msg_t *)&outLmpConnReq);
           /*else if (tlsSupported) {
             // Note: XpanLmpOutgoingConnReq to be initiated after TLS Established
                      event in TCP_CONNECTED State.

           }*/
         } else {
           /* Incoming Connection */
           /*
           if (tls_supported) {
             // Call TLS API to wait on Handshake completion
           } else {
             // Nothing to do: Wait for Incoming LMP Host Connection request
           }
           */
         }
        }
        break;

    case XPAN_TCP_CONNECT_FAILED: {
         if (mTransitionType == XPAN_RECONNECTION_OUTGOING) {
           if (XpanSocketHandler::IsListeningOnTcp() && mEarbudRole == ROLE_PRIMARY) {
             ALOGD("%s %s: Waiting for incoming connection now", R, __func__);
             mTransitionType = XPAN_RECONNECTION_INCOMING;
           } else {
             if (qhci && mEarbudRole == ROLE_PRIMARY) {
               qhci->ConnectionCompleteRes(mBdAddr, UNKNOWN_CONNECTION_IDENTIFIER);
             }
             setState(XPAN_IDLE, evt);
           }
         } else if (mTransitionType == XPAN_LE_TO_AP_IDLE && xm) {
           setState(XPAN_IDLE, evt);
           HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
         } else if (mTransitionType == XPAN_LE_TO_AP_STREAMING && xm) {
           setState(XPAN_IDLE, evt);
           if (mEarbudRole == ROLE_PRIMARY) {
              xm->PrepareAudioBearerRsp(mBdAddr, XM_AC_PREPARE_AUDIO_BEARER_FAILED);
           }
         }
       }
       break;

    case XPAN_BEARER_PREFERENCE_TIMEOUT: {
         setState(XPAN_DISCONNECTING, evt);
         XpanAcSmExecuteEvent(msg);
         // This event is received for LE->AP idle case timeout only in this state
         if (mTransitionType == XPAN_LE_TO_AP_IDLE && xm) {
           HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
         }
       }
       break;

    /* Received only for Incoming connect request since TCP Connected event is skipped
     * as IP address or bd address of remote cant be matched during incoming tcp connection
     * request phase */
    case XPAN_LMP_INC_CONNECTION_REQ_EVT:
        /* Set State to TCP_Connected first and Handle LMP Connect request */
        setState(XPAN_TCP_CONNECTED, evt);
        XpanAcSmExecuteEvent(msg);
        break;

    case XPAN_PRIMARY_DISCONNECTING_EVT:
    case XPAN_WIFI_AP_DISCONNECTED: {
        /* Set State to Disconnecting for Socket cleanups */
        setState(XPAN_DISCONNECTING, evt);
        XpanAcSmExecuteEvent(msg);
        if (mTransitionType == XPAN_LE_TO_AP_IDLE)
          HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
        else if (mTransitionType == XPAN_LE_TO_AP_STREAMING)
          xm->PrepareAudioBearerRsp(mBdAddr, XM_AC_PREPARE_AUDIO_BEARER_FAILED);
      } break;

    default:
        ALOGE("%s %s: Unanticipated Event %d", R, __func__, evt);
  }
}

void XpanAcStateMachine::XpanTcpConnectedStateHandler(xac_handler_msg_t* msg) {
  XacEvent evt = msg->event;
  ALOGD("%s State: XPAN_TCP_CONNECTED, ProcessEvent = %s", R, xpan_event_str(msg->event));
  mdns_uuid_t rem_uuid;

  switch(evt) {
    case QHCI_CREATE_CONNECTION_CANCEL_EVT:
         /* TODO: close fd for outgoing connection */
         /* Cancel Secondary connection if attempted */
         /* Callback to QHCI that connection is cancelled */
         setState(XPAN_DISCONNECTING, evt);
         XpanAcSmExecuteEvent(msg);
         break;

    case XPAN_LMP_INC_CONNECTION_REQ_EVT: {
         mTransitionType = XPAN_RECONNECTION_INCOMING;
         /* Incoming Connection */
         rem_uuid = msg->incLmpConnReq.remote_mdns_uuid;
         XpanDevice *dev = GetDeviceInstance(rem_uuid);
         if (!dev) {
           /* Wouldn't execute this condition as MDNS uuid is already matched in
            * XPan Lmp Manager */
           ALOGE("%s: MDNS UUID didnt match. Received(%s)", __func__,
                 rem_uuid.toString().c_str());
           lmp->SendLmpNotAccepted(XPAN_LMP_CONN_REJECTED_INVALID_DEVICE);
           setState(XPAN_DISCONNECTING, evt);
           XpanAcSmExecuteEvent(msg);
           return;
         }

         /* Update device psm ip address , psm , fd etc*/
         mIpAddr = msg->incLmpConnReq.ip;
         mSocketFd = msg->incLmpConnReq.fd;
         mSocketHdlr->SetSocketFd(mSocketFd);
         // Send acceptance to remote and callback to QHCI
         lmp->SetIpAddress(mIpAddr);
         lmp->SendLmpAccepted();
         setState(XPAN_LMP_CONNECTED, evt);
         if (mEarbudRole == ROLE_PRIMARY) {
           qhci->ConnectionCompleteRes(mBdAddr, XPAN_AC_SUCCESS);
         }
         HandleXpanLmpConnected();
        }
        break;

    case XPAN_LMP_OUT_CONNECTION_REQ: {
         setState(XPAN_LMP_CONNECTING, evt);
         /* Handset Class of Device*/
         uint8_t cod[3] = { 0x0C,  /* Minor Device class - Smartphone*/
                            0x42,  /* Major Device Class: Phone (cellular, cordless,
                                  payphone, modem,...)*/
                            0x5A}; /* Service class*/
         lmp->SendLmpConnectionReq(BT_LE, XpanApplicationController::GetMdnsUuid(), cod);
         }
         break;

    case XPAN_REMOTE_DETAILS_NOT_FOUND_EVT: {
        /* Note: Incoming connection form unbonded device */
         setState(XPAN_DISCONNECTING, evt);
         XpanAcSmExecuteEvent(msg);
        /* TODO: close fd and state machine */
        }
        break;

    case XPAN_TCP_DISCONNECTED_EVT:
        setState(XPAN_DISCONNECTING, evt);
        XpanAcSmExecuteEvent(msg);
        break;

    case XPAN_PRIMARY_DISCONNECTING_EVT:
    case XPAN_BEARER_PREFERENCE_TIMEOUT: {
        setState(XPAN_DISCONNECTING, evt);
        XpanAcSmExecuteEvent(msg);
        // This event is received for LE->AP idle case timeout only in this state
        if (mTransitionType == XPAN_LE_TO_AP_IDLE && xm) {
          HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
        }
       }
       break;

    default:
        ALOGE("%s %s: Unanticipated Event %d", R, __func__, evt);
  }
}

void XpanAcStateMachine::XpanLmpConnectingStateHandler(xac_handler_msg_t* msg) {
  XacEvent evt = msg->event;
  ALOGD("%s State: XPAN_LMP_CONNECTING, ProcessEvent = %s", R, xpan_event_str(msg->event));
  uint8_t status = XPAN_AC_SUCCESS;

  switch(evt) {
    case QHCI_CREATE_CONNECTION_CANCEL_EVT:
         /* TODO: close fd for outgoing connection */
         /* Cancel Secondary connection if attempted */
         /* Callback to QHCI that connection is cancelled */
         setState(XPAN_DISCONNECTING, evt);
         XpanAcSmExecuteEvent(msg);
         break;

    case XPAN_LMP_CONNECTION_RES_EVT:
         status = msg->lmpConnectRsp.status;

         /* Failure response */
         if (status != XPAN_AC_SUCCESS) {
           setState(XPAN_DISCONNECTING, evt);
           XpanAcSmExecuteEvent(msg);
           if (mEarbudRole == ROLE_PRIMARY) {
             if (mTransitionType == XPAN_LE_TO_AP_IDLE) {
              HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
             } else if (mTransitionType == XPAN_LE_TO_AP_STREAMING) {
              xm->PrepareAudioBearerRsp(mBdAddr, XM_AC_PREPARE_AUDIO_BEARER_FAILED);
             }
           }
         /* Success Response */
         } else {
           setState(XPAN_LMP_CONNECTED, evt);
           // Callback to QHCI and XM only from Primary SM
           if (mEarbudRole == ROLE_PRIMARY) {
             qhci->ConnectionCompleteRes(mBdAddr, XPAN_AC_SUCCESS);
           }
           HandleXpanLmpConnected();
         }
         break;

    case XPAN_TCP_DISCONNECTED_EVT:
        setState(XPAN_DISCONNECTING, evt);
        XpanAcSmExecuteEvent(msg);
        if (mTransitionType == XPAN_LE_TO_AP_IDLE)
          HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
        else if (mTransitionType == XPAN_LE_TO_AP_IDLE)
          xm->PrepareAudioBearerRsp(mBdAddr, XM_AC_PREPARE_AUDIO_BEARER_FAILED);
        break;

    case XPAN_PRIMARY_DISCONNECTING_EVT:
    case XPAN_BEARER_PREFERENCE_TIMEOUT: {
        setState(XPAN_DISCONNECTING, evt);
        XpanAcSmExecuteEvent(msg);
        // This event is received for LE->AP idle case timeout only in this state
        if (mTransitionType == XPAN_LE_TO_AP_IDLE && xm) {
          HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
        }
       }
       break;


    default:
        ALOGE("%s %s: Unanticipated Event %d", R, __func__, evt);
  }
}

void XpanAcStateMachine::XpanLmpConnectedStateHandler(xac_handler_msg_t* msg) {
  XacEvent evt = msg->event;
  ALOGD("%s State: XPAN_LMP_CONNECTED, ProcessEvent = %s", R, xpan_event_str(msg->event));

  switch(evt) {
    case QHCI_ENABLE_ENCRYPTION_EVT:
         /*TODO: TLS */
         break;

    case XM_INITIATE_LMP_BEARER_SWITCH_EVT:
         setState(XPAN_BEARER_SWITCH_PENDING, evt);
         // TODO: Initiate LMP procedure
         break;

    /* TODO: To be moved to TCP Connected State */
    case XPAN_TLS_ESTABLISHED_EVT:
         if (mTransitionType == XPAN_RECONNECTION_OUTGOING ||
             mTransitionType == XPAN_RECONNECTION_INCOMING) {
           // No need to perform bearer switch procedure
           setState(XPAN_AP_ACTIVE, evt);
         } else {
           if (mTransitionType == XPAN_LE_TO_AP_IDLE ||
               mTransitionType == XPAN_LE_TO_AP_STREAMING) {
             // below call gets triggered for both earbuds
             lmp->SendLmpPrepareBearerReq(LMP_TP_LE, LMP_TP_AP);
           }
         }
         break;

    case XPAN_LMP_REMOTE_PING_REQ_EVT:
        lmp->SendLmpPingRes();
        break;

    /* ACL data before transition comepleted (could be delayed due to congestion) */
    case XPAN_LMP_ACL_DATA_EVT: {
         std::vector<uint8_t> data(msg->data.data,
                                  (msg->data.data + msg->data.len));
         qhci->DataReceivedCb(mBdAddr, msg->data.llid, data);
        } break;

    case XPAN_REMOTE_DISCONNECTED_EVT:
    case XPAN_TCP_DISCONNECTED_EVT:
    case QHCI_DISCONNECT_CONNECTION_EVT:
        setState(XPAN_DISCONNECTING, evt);
        XpanAcSmExecuteEvent(msg);
        break;

    case XPAN_PRIMARY_DISCONNECTING_EVT:
    case XPAN_BEARER_PREFERENCE_TIMEOUT: {
        setState(XPAN_DISCONNECTING, evt);
        XpanAcSmExecuteEvent(msg);
        // This event is received for LE->AP idle case timeout only in this state
        if (mTransitionType == XPAN_LE_TO_AP_IDLE && xm) {
          HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
        }
       }
       break;

    default:
        ALOGE("%s %s: Unanticipated Event %d", R, __func__, evt);
  }
}

void XpanAcStateMachine::XpanBearerSwitchPendingStateHandler(xac_handler_msg_t* msg) {
  XacEvent evt = msg->event;
  ALOGD("%s State: XPAN_BEAARER_SWITCH_PENDING, ProcessEvent = %s", R, xpan_event_str(msg->event));

  uint8_t status;

  switch(evt) {
    case XPAN_LMP_PREPARE_BEARER_RES_EVT:
         HandleLmpPrepareBearerRspEvt(msg);
         break;

    case XM_INITIATE_LMP_BEARER_SWITCH_EVT: {
           mOperation = XPAN_BEARER_SWITCH;
           // Init L2CAP pause. Pause QHCI traffic first
           TransportType pauseTransport = XPAN_AP;
           if (mTransitionType == XPAN_LE_TO_AP_IDLE ||
               mTransitionType == XPAN_LE_TO_AP_STREAMING) {
             pauseTransport = BT_LE;
           }
           qhci->L2capPauseUnpauseReq(mBdAddr, PAUSE, pauseTransport);
         } break;

    case QHCI_L2CAP_PAUSE_UNPAUSE_RES_EVT:
         /* QHCI response for pause/unpause */
         HandleQhciPauseUnpauseEvt(msg);
         break;

    case XPAN_LMP_L2CAP_PAUSE_UNPAUSE_RES_EVT:
         HandleLmpPauseUnpauseRspEvt(msg);
         break;

    case XPAN_LMP_BEARER_SWITCH_RES_EVT:
         HandleLmpBearerSwitchRspEvt(msg);
         break;

    case XM_BEARER_SWITCH_IND_EVT:
         HandleXmBearerSwitchInd(msg);
         break;

          /* During AP-AP roaming only*/
    case XPAN_TCP_NEW_AP_TRANSPORT_STATUS:
         HandleNewTcpTransportReadyEvt(msg);
         break;

    case XPAN_LMP_REMOTE_PING_REQ_EVT:
        lmp->SendLmpPingRes();
        break;

    /* ACL data before l2cap pause-unpause was done */
    case XPAN_LMP_ACL_DATA_EVT: {
         std::vector<uint8_t> data(msg->data.data,
                                  (msg->data.data + msg->data.len));
         qhci->DataReceivedCb(mBdAddr, msg->data.llid, data);
        } break;

    case XPAN_REMOTE_DISCONNECTED_EVT:
    case XPAN_TCP_DISCONNECTED_EVT:
        if (mTransitionType == XPAN_LE_TO_AP_IDLE)
          HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
        else if (mTransitionType == XPAN_LE_TO_AP_IDLE)
          xm->PrepareAudioBearerRsp(mBdAddr, XM_AC_PREPARE_AUDIO_BEARER_FAILED);
        else if (mTransitionType == XPAN_AP_TO_LE_IDLE ||
                 mTransitionType == XPAN_AP_TO_LE_STREAMING)
          xm->PrepareAudioBearerRsp(mBdAddr, XM_SUCCESS);
    case QHCI_DISCONNECT_CONNECTION_EVT:
        setState(XPAN_DISCONNECTING, evt);
        XpanAcSmExecuteEvent(msg);
        break;

    case XPAN_PRIMARY_DISCONNECTING_EVT:
        if (mTransitionType == XPAN_LE_TO_AP_STREAMING) {
          setState(XPAN_DISCONNECTING, evt);
          XpanAcSmExecuteEvent(msg);
        }
        break;

    case QHCI_SEND_ACL_DATA_EVT:
         lmp->SendAclData(msg->aclDataParams.llid, msg->aclDataParams.len, msg->aclDataParams.data);
         break;

    case XPAN_BEARER_PREFERENCE_TIMEOUT: {
        if (mTransitionType == XPAN_LE_TO_AP_IDLE) {
          if (mBearerSwitchState == XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ ||
              mBearerSwitchState == XPAN_LMP_PREPARE_BEARER_SWITCH_REQ ||
              mBearerSwitchState == XPAN_LMP_BEARER_SWITCH_REQ) {
            lmp->SendLmpCancelBearerSwitchInd(LMP_TP_LE, LMP_TP_AP, 0xFF);
            XpanAcStateMachine *sm = mDevice->GetStateMachine(ROLE_SECONDARY);
            if (sm && mEarbudRole == ROLE_PRIMARY) {
              sm->SendLmpCancelBearerSwitchInd(LMP_TP_LE, LMP_TP_AP, 0xFF);
            }
          }
          setState(XPAN_DISCONNECTING, evt);
          XpanAcSmExecuteEvent(msg);
          HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
        } else if (mTransitionType == XPAN_AP_TO_LE_IDLE) {
          if (mBearerSwitchState == XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ ||
              mBearerSwitchState == XPAN_LMP_PREPARE_BEARER_SWITCH_REQ ||
              mBearerSwitchState == XPAN_LMP_BEARER_SWITCH_REQ) {
            lmp->SendLmpCancelBearerSwitchInd(LMP_TP_AP, LMP_TP_LE, 0xFF);
            XpanAcStateMachine *sm = mDevice->GetStateMachine(ROLE_SECONDARY);
            if (sm && mEarbudRole == ROLE_PRIMARY) {
              sm->SendLmpCancelBearerSwitchInd(LMP_TP_AP, LMP_TP_LE, 0xFF);
            }
          }
          setState(XPAN_AP_ACTIVE, evt);
          HandleBearerPreferenceCmpl(mBdAddr, BT_LE, XM_AC_BEARER_PREFERENCE_REJECTED);
        }
       }
       break;
    default:
        ALOGE("%s %s: Unanticipated Event %d", R, __func__, evt);
  }

}

void XpanAcStateMachine::XpanApActiveStateHandler(xac_handler_msg_t* msg) {
  XacEvent evt = msg->event;
  ALOGD("%s State: XPAN_AP_ACTIVE, ProcessEvent = %s", R, xpan_event_str(msg->event));

  uint8_t bearer_from, bearer_to;

  switch (evt) {
    case XM_PREPARE_BEARER_EVT:
         bearer_to = msg->prepareBearer.bearer;
         mOperation = XPAN_BEARER_SWITCH;
         if (bearer_to == XPAN_AP) {
           mTransitionType = XPAN_AP_TO_AP_ROAMING_STREAMING;
           setState(XPAN_BEARER_SWITCH_PENDING, evt);
           lmp->SendLmpPrepareBearerReq(LMP_TP_AP, LMP_TP_AP);
           newTcpTransportReady = false;
           prepBearerApRoamingDone = false;
           // Create TCP Connection secure connection
           mSocketHdlr->InitiateRoamingPrep(mSocketHdlr, mIpRoaming, mTcpPortRoaming);
         } else if (bearer_to == BT_LE) {
           mBearerSwitchState = XPAN_LMP_PREPARE_BEARER_SWITCH_REQ;
           mTransitionType = XPAN_AP_TO_LE_STREAMING;
           setState(XPAN_BEARER_SWITCH_PENDING, evt);
           lmp->SendLmpPrepareBearerReq(LMP_TP_AP, LMP_TP_LE);
         }
         break;

    case XM_BEARER_PREFERENCE_EVT:
         bearer_to = msg->prepareBearer.bearer;
         mOperation = XPAN_BEARER_SWITCH;
         if (bearer_to == XPAN_AP) {
           mTransitionType = XPAN_AP_TO_AP_ROAMING_IDLE;
           setState(XPAN_BEARER_SWITCH_PENDING, evt);
           lmp->SendLmpPrepareBearerReq(LMP_TP_AP, LMP_TP_AP);
           // Create TCP Connection secure connection
           mSocketHdlr->InitiateRoamingPrep(mSocketHdlr, mIpRoaming, mTcpPortRoaming);
         } else if (bearer_to == BT_LE) {
           mTransitionType = XPAN_AP_TO_LE_IDLE;
           mBearerPrefTimer = new XpanAcTimer("BearerPreferenceTimer",
                                               BearerPreferenceTimeout, this);
           mBearerPrefTimer->StartTimer(XPAN_AC_BEARER_PREFERENCE_TIMER);
           setState(XPAN_BEARER_SWITCH_PENDING, evt);
           lmp->SendLmpPrepareBearerReq(LMP_TP_AP, LMP_TP_LE);
         }
         break;

    case XM_INITIATE_LMP_BEARER_SWITCH_EVT:
        /* Received for Primary earbud only in this state */
        bearer_to = msg->initiateLmpBearerSwitch.bearer;
        if (bearer_to == XPAN_AP) {
          // Prepare audio bearer response (bearer preference wont come while IDLE->Streaming)
          xm->PrepareAudioBearerRsp(mBdAddr, XM_SUCCESS);
        }
        break;

    case QHCI_SEND_ACL_DATA_EVT:
         lmp->SendAclData(msg->aclDataParams.llid, msg->aclDataParams.len, msg->aclDataParams.data);
         break;

    case QHCI_GET_REMOTE_VERSION_EVT: {
         mdns_uuid_t uuid = XpanApplicationController::GetLocalMdnsUuid();
         LocalVersionInfo info = XpanApplicationController::GetLocalVersion();
         uint8_t version = info.version;
         uint16_t subversion = info.subversion;
         uint16_t companyId = info.companyId;
         lmp->SendLmpVersionReq(uuid, version, companyId, subversion);
         } break;

    case QHCI_GET_REMOTE_LE_FEATURES_EVT: {
          mdns_uuid_t uuid = XpanApplicationController::GetLocalMdnsUuid();;
          uint64_t le_feat = XpanApplicationController::GetLocalLeFeatures();
          lmp->SendLmpLeFeatureReq(uuid, le_feat);
         } break;

    case XPAN_LMP_VERSION_RES_EVT: {
          qhci->RemoteVersionRes(mBdAddr, msg->versionRsp.version,
              msg->versionRsp.company_id, msg->versionRsp.subversion);
         } break;

    case XPAN_LMP_LE_FEATURE_RES_EVT: {
          qhci->RemoteLeFeaturesRes(mBdAddr, XPAN_AC_SUCCESS, msg->featRsp.le_features);
         } break;

    case XPAN_LMP_SWITCH_PRIMARY_EVT:
         /* on current primary*/
         if (msg->primarySwitchReq.operation == PRIMARY_SWITCH_START) {
           mOperation = XPAN_ROLE_SWITCH;
           // Accept response sent once QHCI paused the traffic
         } else if (msg->primarySwitchReq.operation == PRIMARY_SWITCH_COMPLETE) {
           lmp->SendLmpAccepted();
           // Primary switch callback to XM
           //xm->RoleSwitchInd(mMacAddr);
         } else if (msg->primarySwitchReq.operation == PRIMARY_SWITCH_CANCEL) {
           lmp->SendLmpNotAccepted(0xFF);
         }
         break;

    case QHCI_L2CAP_PAUSE_UNPAUSE_RES_EVT:
         /* QHCI response for pause/unpause - for role switch in this state */
         HandleQhciPauseUnpauseEvt(msg);
         break;

    case XPAN_LMP_REMOTE_PING_REQ_EVT:
        lmp->SendLmpPingRes();
        break;

    case XPAN_PRIMARY_DISCONNECTING_EVT:
    case XPAN_REMOTE_DISCONNECTED_EVT:
    case XPAN_TCP_DISCONNECTED_EVT:
    case QHCI_DISCONNECT_CONNECTION_EVT:
        setState(XPAN_DISCONNECTING, evt);
        XpanAcSmExecuteEvent(msg);
        break;

    case XPAN_LMP_ACL_DATA_EVT: {
         if (mEarbudRole == ROLE_PRIMARY) {
           std::vector<uint8_t> data(msg->data.data,
                                    (msg->data.data + msg->data.len));
           qhci->DataReceivedCb(mBdAddr, msg->data.llid, data);
         } else {
           ALOGE("%s %s: ACL Data received on Secondary Earbud. Ignore", R, __func__);
         }
        } break;

    case XPAN_LMP_VERSION_REQ_EVT: {
          LocalVersionInfo info = XpanApplicationController::GetLocalVersion();
          mdns_uuid_t uuid = XpanApplicationController::GetLocalMdnsUuid();
          lmp->SendLmpVersionRes(uuid, info.version, info.companyId,
                                         info.subversion);
        } break;

    case XPAN_LMP_LE_FEATURE_REQ_EVT: {
          uint64_t le_feat = XpanApplicationController::GetLocalLeFeatures();
          mdns_uuid_t uuid = XpanApplicationController::GetLocalMdnsUuid();
          lmp->SendLmpLeFeatureRes(uuid, le_feat);
        } break;

    default:
        ALOGE("%s %s: Unanticipated Event %d", R, __func__, evt);
  }

}

void XpanAcStateMachine::XpanDisconectingStateHandler(xac_handler_msg_t* msg) {
  XacEvent evt = msg->event;
  ALOGD("%s State: XPAN_DISCONNECTING, ProcessEvent = %s", R, xpan_event_str(msg->event));
  mApDetailsOkToSend = false;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return;
  }

  switch (evt) {
    case XPAN_BEARER_PREFERENCE_TIMEOUT:
    case XPAN_WIFI_AP_DISCONNECTED:
    case XPAN_PRIMARY_DISCONNECTING_EVT:
    case QHCI_CREATE_CONNECTION_CANCEL_EVT:
        HandleSocketClosure();
        XpanSocketHandler::CloseListeningSocket(mBdAddr);
        setState(XPAN_IDLE, XPAN_TCP_DISCONNECTED_EVT);
        if (mDevice) {
          mDevice->HandleEarbudDisconnectedEvt(mEarbudRole);
        }
        if (mTransitionType == XPAN_LE_TO_AP_IDLE && mBearerPrefTimer) {
          stopTimer(mBearerPrefTimer);
          mBearerPrefTimer = NULL;
        }
        if (qhci && mEarbudRole == ROLE_PRIMARY) {
          qhci->ConnectionCompleteRes(mBdAddr, UNKNOWN_CONNECTION_IDENTIFIER);
        }
        Cleanup();
        UpdatePrimaryDisconnToSec();
        break;

    case XPAN_LMP_CONNECTION_RES_EVT:
        HandleSocketClosure();
        if (qhci && mEarbudRole == ROLE_PRIMARY &&
            mTransitionType == XPAN_RECONNECTION_OUTGOING) {
          qhci->ConnectionCompleteRes(mBdAddr, UNKNOWN_CONNECTION_IDENTIFIER);
        }
        setState(XPAN_IDLE, XPAN_TCP_DISCONNECTED_EVT);
        Cleanup();
        UpdatePrimaryDisconnToSec();
        break;

    case QHCI_DISCONNECT_CONNECTION_EVT:
        HandleSocketClosure();
        if (xm && mEarbudRole == ROLE_PRIMARY) xm->OnCurrentTransportUpdated(mBdAddr, NONE);
        setState(XPAN_IDLE, XPAN_TCP_DISCONNECTED_EVT);
        if (mDevice) {
          mDevice->HandleEarbudDisconnectedEvt(mEarbudRole);
        }
        if (qhci && mEarbudRole == ROLE_PRIMARY) {
          qhci->DisconnectionCompleteRes(mBdAddr, LOCAL_USER_TERMINATED_CONNECION);
        }
        Cleanup();
        UpdatePrimaryDisconnToSec();
        break;

    case XPAN_TCP_DISCONNECTED_EVT:
    case XPAN_REMOTE_DISCONNECTED_EVT:
        HandleSocketClosure();
        if (xm && mEarbudRole == ROLE_PRIMARY) xm->OnCurrentTransportUpdated(mBdAddr, NONE);
        if (mDevice) {
          mDevice->HandleEarbudDisconnectedEvt(mEarbudRole);
        }
        if (qhci && mEarbudRole == ROLE_PRIMARY &&
            mPrevState >= XPAN_LMP_CONNECTED) {
          qhci->DisconnectionCompleteRes(mBdAddr, XPAN_CONNECTION_TIMEOUT);
        }
        setState(XPAN_IDLE, XPAN_TCP_DISCONNECTED_EVT);
        Cleanup();
        UpdatePrimaryDisconnToSec();
        break;

    case XM_BEARER_SWITCH_IND_EVT:
    case XPAN_LMP_L2CAP_PAUSE_UNPAUSE_RES_EVT:
    case XPAN_LMP_PREPARE_BEARER_RES_EVT:
    case XPAN_LMP_BEARER_SWITCH_RES_EVT: {
        HandleSocketClosure();
        if (xm && mEarbudRole == ROLE_PRIMARY) xm->OnCurrentTransportUpdated(mBdAddr, NONE);
        setState(XPAN_IDLE, XPAN_TCP_DISCONNECTED_EVT);
        if (qhci && mEarbudRole == ROLE_PRIMARY) {
          qhci->DisconnectionCompleteRes(mBdAddr, XPAN_CONNECTION_TIMEOUT);
        }
        Cleanup();

        //  used for State machine object removal (this will be anyway ignored in IDLE state)
        XmRemoteDisconnectedEvent *msg = (XmRemoteDisconnectedEvent *)malloc(sizeof(XmRemoteDisconnectedEvent));
        msg->event = XPAN_REMOTE_DISCONNECTED_EVT;
        msg->addr = mBdAddr;
        msg->role = mEarbudRole;
        xac->PostMessage((xac_handler_msg_t *)msg, MSG_PRIORITY_DEFAULT);
        UpdatePrimaryDisconnToSec();
        } break;

    default:
      ALOGW("%s %s: Disconnection for Unknown event(%d)", R, __func__, (uint8_t)evt);
  }
}

XacSmState XpanAcStateMachine::GetState() {
  return mCurrentState;
}

void XpanAcStateMachine::setState(XacSmState newState, XacEvent event) {
  ALOGD("%s %s: State Transition %s -> %s, Event %s", R, __func__, xpan_state_str(mCurrentState),
        xpan_state_str(newState), xpan_event_str(event));
  mPrevState = (newState != XPAN_IDLE ? mCurrentState: XPAN_IDLE);
  mCurrentState = newState;
}

void XpanAcStateMachine::SetEarbudProperties(ipaddr_t ip, macaddr_t mac,
    uint32_t audio_loc, uint32_t tcp_port) {
  mIpAddr = ip;
  mMacAddr = mac;
  mAudioLoc = audio_loc;
  mTcpPort = tcp_port;
  lmp->SetIpAddress(mIpAddr);
}

void XpanAcStateMachine::SetRoamingDetails(ipaddr_t ip, macaddr_t mac,
                                               uint32_t tcp_port) {
  mIpRoaming = ip;
  mMacRoaming = mac;
  mTcpPortRoaming = tcp_port;
}

XpanLmpManager* XpanAcStateMachine::GetLmpManager() {
  return lmp;
}

void XpanAcStateMachine::UpdateEarbudRole(XpanEarbudRole role) {
  mEarbudRole = role;
}

void XpanAcStateMachine::SetTrigger(XpanSwitchTrigger trigger) {
  mTrigger = trigger;
}

void XpanAcStateMachine::HandleRemoteApDetailsUpdate(xac_handler_msg_t* msg) {
  /* Initiate TCP Connection irrespective of ROLE & trigger
   * and handle next steps after TCP_CONNECTED_EVT based on trigger */

  if (mCurrentState == XPAN_TCP_CONNECTING /*&&
      mTransitionType == XPAN_RECONNECTION_OUTGOING*/) {
    ALOGD("%s %s: Initiate Outgoing TCP Connection", R, __func__);
    mSocketHdlr->InitTcpConnection(mSocketHdlr, mIpAddr, mTcpPort);
  }
}

void XpanAcStateMachine::TriggerFilteredScan(void *data) {
  XpanAcStateMachine *sm = (XpanAcStateMachine *)data;

  if (!sm) {
    ALOGE("%s: Invalid state machine instance", __func__);
    return;
  }

  XMXacIf* xm = XMXacIf::GetIf();
  if (!xm) {
    ALOGE("%s: XM is not initialized", __func__);
    return;
  }

  // start filtered scan in XP for AP->LE transition
  xm->StartFilteredScan(sm->GetAddr());
}

void HandleMdnsQueryTimeout(void *data) {
  ALOGD("%s: Mdns Query Timeout. Stop MDNS Query and Wait for Incoming connection.", __func__);

  XpanAcStateMachine* sm = (XpanAcStateMachine *)data;
  if (!sm) {
    ALOGE("%s: Invalid AC Instance", __func__);
    return;
  }

  bdaddr_t addr = sm->GetAddr();
  ALOGD("%s: Device %s", __func__, ConvertRawBdaddress(addr));

  XMXacIf* xm = XMXacIf::GetIf();
  if (!xm) {
    ALOGE("%s: XM is not initialized", __func__);
    return;
  }

  // stop MDNS Query at Xpan Profile
  xm->TriggerMdnsQuery(addr, false);

  // Set now that the transition type is XPAN_RECONNECTION_INCOMING
  // i.e. Handset will wait for incoming connection
  if (XpanSocketHandler::IsListeningOnTcp()) {
    sm->SetTransitionType(XPAN_RECONNECTION_INCOMING);
  }
}

void XpanAcStateMachine::BearerPreferenceTimeout(void *data) {
  ALOGD("%s: Bearer Preference procedure timeout.", __func__);

  XpanAcStateMachine* sm = (XpanAcStateMachine *)data;
  if (!sm) {
    ALOGE("%s: Invalid AC Instance", __func__);
    return;
  }

  XpanBearerPrefTimeout *msg =
        (XpanBearerPrefTimeout *) malloc(sizeof(XpanBearerPrefTimeout));
  if (msg == NULL) {
    ALOGE("%s: failed to allocate memory", __func__);
    return;
  }

  msg->event = (XacEvent)XPAN_BEARER_PREFERENCE_TIMEOUT;
  msg->addr = sm->GetAddr();
  msg->role = sm->GetRole();

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    free(msg);
    return;
  }

  xac->PostMessage((xac_handler_msg_t *)msg, MSG_PRIORITY_DEFAULT);
}


void XpanAcStateMachine::SetTransitionType(XpanTransitionType transition) {
  mTransitionType = transition;
}

bool XpanAcStateMachine::HandleInitReconnection() {
  ALOGD("%s %s: %s", R, __func__, ConvertRawBdaddress(mBdAddr));
  bool queryTriggered = false, registrationDone = false, listening = false;

  /* Steps:
    1. Trigger MDNS Query.
       Start 30 sec timer.
    2. Start Listening on TCP port.
    3. Register MDNS Service.
 */

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s %s: Unexpected Error. AC instance not found", R, __func__);
    return false;
  }

  ipaddr_t local_ip = xac->GetLocalIpAddr();
  if (local_ip.isEmpty()) {
    ALOGE("%s %s: Local IP address is not set. Probably HS is not connected to any AP",
          R, __func__);
    /*Note: No response to QHCI to prevent create connection and failure response loop
     for backgound connection */
    //qhci->ConnectionCompleteRes(mBdAddr, XPAN_TCP_CONNECTION_FAILED);
    return false;
  }

  queryTriggered = GetRemoteApDetails();
  if (!queryTriggered) {
    queryTriggered = xm->TriggerMdnsQuery(mBdAddr, true);
    mDnsQueryTimer = new XpanAcTimer("MDnsQueryTimer", HandleMdnsQueryTimeout, this);
    mDnsQueryTimer->StartTimer(XPAN_AC_MDNS_QUERY_TIMEOUT);
  }

  listening = XpanSocketHandler::IsListeningOnTcp();
  if (!listening) {
    ALOGD("%s %s: Starting to Listen for incoming connection for HS IP (%s)",
          R, __func__, local_ip.toString().c_str());
    listening = XpanSocketHandler::CreateTcpSocketForIncomingConnection(local_ip, mBdAddr);
  }

  // Send Port details callback to Xpan Profile
  XMXacIf *xm = XMXacIf::GetIf();
  if (xm) {
    xm->HandSetPortNumberRsp(XpanSocketHandler::GetTcpListeningPort(),
                             XpanSocketHandler::GetUdpPort(),
                             XpanSocketHandler::GetUdpTsfPort());
  }

  if (listening) {
    ALOGD("%s %s: Register HS MDNS Service for remote to discover (listening = %d)",
          R, __func__, listening);
    registrationDone = xm->RegisterMdnsService(mBdAddr);
  }

  return (registrationDone || queryTriggered);
}

void XpanAcStateMachine::HandleXpanLmpConnected() {
  ALOGD("%s %s: TransitionType = %d", R, __func__, mTransitionType);

  switch (mTransitionType) {
    case XPAN_RECONNECTION_OUTGOING:
    case XPAN_RECONNECTION_INCOMING: {
          if (isPingEnabled) {
            XpanLmpManager::StartSupervisionTimer(lmp);
          }
          mApDetailsOkToSend = true;
          // Callback to Xpan Profile suggesting BT is connected over AP
          if (mEarbudRole == ROLE_PRIMARY) {
            xm->OnCurrentTransportUpdated(mBdAddr, XPAN_AP);
            SetFilteredScanTimer();
          }
          setState(XPAN_AP_ACTIVE, XPAN_LMP_CONNECTION_RES_EVT);
          if (mEarbudRole == ROLE_SECONDARY && qhci->isStreamingActive(mBdAddr)) {
            mDevice->UpdateEarbudConnectionStatusToCp(ROLE_SECONDARY, XPAN_EB_CONNECTED);
          }
        } break;

    case XPAN_LE_TO_AP_IDLE: {
          mBearerSwitchState = XPAN_LMP_PREPARE_BEARER_SWITCH_REQ;
          lmp->SendLmpPrepareBearerReq(LMP_TP_LE, LMP_TP_AP);
          setState(XPAN_BEARER_SWITCH_PENDING, XM_BEARER_PREFERENCE_EVT);
        } break;

    case XPAN_LE_TO_AP_STREAMING: {
          mBearerSwitchState = XPAN_LMP_PREPARE_BEARER_SWITCH_REQ;
          lmp->SendLmpPrepareBearerReq(LMP_TP_LE, LMP_TP_AP);
          setState(XPAN_BEARER_SWITCH_PENDING, XM_PREPARE_BEARER_EVT);
        } break;

    default:
        ALOGD("%s %s: LMP Connected in Unexpected Transition", R, __func__);
  }
}

void XpanAcStateMachine::HandleLmpPrepareBearerRspEvt(xac_handler_msg_t* msg) {
  uint8_t status = msg->prepareBearerRsp.status;
  XacEvent evt = msg->prepareBearerRsp.event;
  XpanAcStateMachine *ssm = NULL;

  ALOGD("%s %s: status = %d Transition type = %d", R, __func__, status, mTransitionType);

  /* Prepare Bearer failed case */
  if (status != XPAN_AC_SUCCESS) {
    switch (mTransitionType) {
      case XPAN_LE_TO_AP_IDLE:
      case XPAN_LE_TO_AP_STREAMING:
           /* Change state to XPAN_DISCONNECTING. Since remote rejected the procedure,
              entire procedure is needed to be restarted as needed */
           setState(XPAN_DISCONNECTING, evt);
           if (mEarbudRole == ROLE_PRIMARY) {
             XpanApplicationController *xac = XpanApplicationController::Get();
             XpanDevice* dev = xac ? xac->GetDevice(mBdAddr): NULL;
             if (dev) {
               ssm = dev->GetStateMachine(ROLE_SECONDARY);
               if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_LE,
                            LMP_TP_AP, evt);
             }
             if (mTransitionType == XPAN_LE_TO_AP_STREAMING)
               xm->PrepareAudioBearerRsp(mBdAddr, XM_AC_PREPARE_AUDIO_BEARER_FAILED);
             else {
              HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
             }
           }
           XpanAcSmExecuteEvent(msg);
           break;

      case XPAN_AP_TO_LE_IDLE:
      case XPAN_AP_TO_LE_STREAMING:
      case XPAN_AP_TO_AP_ROAMING_IDLE:
      case XPAN_AP_TO_AP_ROAMING_STREAMING:
           /* keep state as XPAN AP */
           setState(XPAN_AP_ACTIVE, evt);
           if (mEarbudRole == ROLE_PRIMARY) {
             XpanApplicationController *xac = XpanApplicationController::Get();
             XpanDevice* dev = xac ? xac->GetDevice(mBdAddr): NULL;
             if (dev) {
               ssm = dev->GetStateMachine(ROLE_SECONDARY);
             }
             if (mTransitionType != XPAN_AP_TO_AP_ROAMING_IDLE &&
                 mTransitionType != XPAN_AP_TO_AP_ROAMING_STREAMING) {
               if (ssm) ssm->SendLmpCancelBearerSwitchInd(
                             LMP_TP_AP, LMP_TP_LE, 0xFF);
             } else {
               if (ssm) ssm->SendLmpCancelBearerSwitchInd(
                             LMP_TP_AP, LMP_TP_AP, 0xFF);
             }
             if (mTransitionType == XPAN_AP_TO_LE_STREAMING ||
                 mTransitionType == XPAN_AP_TO_AP_ROAMING_STREAMING) {
               xm->PrepareAudioBearerRsp(mBdAddr, XM_AC_PREPARE_AUDIO_BEARER_FAILED);
             } else {
              HandleBearerPreferenceCmpl(mBdAddr, BT_LE, XM_AC_PREPARE_AUDIO_BEARER_FAILED);
             }
           }
           break;

      default:
        ALOGE("%s %s: Unexpected Transition", R, __func__);
        // disconnect connection
        setState(XPAN_DISCONNECTING, evt);
    }
  } else {
    switch (mTransitionType) {
      case XPAN_LE_TO_AP_IDLE:
           /* Proceed with L2cap Pause Unpause. Send Pause to QHCI first and wait for CB */
           if (mEarbudRole == ROLE_PRIMARY) {
             qhci->L2capPauseUnpauseReq(mBdAddr, PAUSE, BT_LE);
           } else if (mEarbudRole == ROLE_SECONDARY) {
             mApDetailsOkToSend = true;
             XpanAcStateMachine *psm = mDevice->GetStateMachine(ROLE_PRIMARY);
             if (psm && psm->GetState() == XPAN_AP_ACTIVE) {
                SendLmpBearerSwitchCompleteInd(LMP_TP_LE, LMP_TP_AP);
             }
           }
           break;

      case XPAN_LE_TO_AP_STREAMING:
           /* Send remote AP details to XM and wait for Initiation of LMP BS procedure.
              Note - Common step for both roles */
           mApDetailsOkToSend = true;
           mDevice->UpdateEarbudConnectionStatusToCp(mEarbudRole, XPAN_EB_CONNECTED);

           if (mEarbudRole == ROLE_PRIMARY) {
             XpanApplicationController *xac = XpanApplicationController::Get();
             if (xac) {
               XmInitiateLmpBearerSwitch *initEvent =
                   (XmInitiateLmpBearerSwitch *)malloc(sizeof(XmInitiateLmpBearerSwitch));
               initEvent->event = XM_INITIATE_LMP_BEARER_SWITCH_EVT;
               initEvent->addr = mBdAddr;
               initEvent->bearer = XPAN_AP;
               xac->PostMessage((xac_handler_msg_t *)initEvent, false);
             }
           }

           if (mEarbudRole == ROLE_SECONDARY) {
             XpanAcStateMachine *psm = mDevice->GetStateMachine(ROLE_PRIMARY);
             if (psm && psm->GetState() == XPAN_AP_ACTIVE) {
                SendLmpBearerSwitchCompleteInd(LMP_TP_LE, LMP_TP_AP);
             }
           }
           break;

      case XPAN_AP_TO_LE_IDLE:
      case XPAN_AP_TO_LE_STREAMING:
           /* Proceed with L2cap Pause Unpause, wait to QHCI callback.
              Note InitiateLmpBearerSwitch is not initiated in this scenario */
           if (mEarbudRole == ROLE_PRIMARY) {
             qhci->L2capPauseUnpauseReq(mBdAddr, PAUSE, XPAN_AP);
           }
           break;

      case XPAN_AP_TO_AP_ROAMING_IDLE:
      case XPAN_AP_TO_AP_ROAMING_STREAMING:
           prepBearerApRoamingDone = true;
           /* Proceed with L2cap Pause Unpause (idle transition) & wait to QHCI callback */
           if (newTcpTransportReady && mTransitionType == XPAN_AP_TO_AP_ROAMING_IDLE &&
               mEarbudRole == ROLE_PRIMARY) {
            qhci->L2capPauseUnpauseReq(mBdAddr, PAUSE, XPAN_AP);
           } else if (newTcpTransportReady && mTransitionType == XPAN_AP_TO_AP_ROAMING_STREAMING) {
            mApDetailsOkToSend = true;
            // callback to XM with remote AP details
            mDevice->UpdateEarbudConnectionStatusToCp(mEarbudRole, XPAN_EB_CONNECTED);
           }
           break;

      default:
        ALOGE("%s %s: Unexpected Transition", R, __func__);
        // disconnect connection
        setState(XPAN_DISCONNECTING, evt);
    }
  }
}

void XpanAcStateMachine::HandleQhciPauseUnpauseEvt(xac_handler_msg_t* msg) {
  uint8_t action = msg->l2capPauseUnpauseRes.action; // pause/unpause
  uint8_t status = msg->l2capPauseUnpauseRes.status; // accepted/rejected
  XpanAcStateMachine *ssm = NULL;

  ALOGD("%s %s: action = %d, status = %d mOp = %d", R, __func__, action, status, mOperation);

  if (mOperation == XPAN_ROLE_SWITCH) {
    HandleXpanPrimarySwitchReq(msg, action);
    return;
  }

  /* If this is for unpause callback for Bearer Switch Cancel procedure, ignore the callback */
  if (action == UNPAUSE && mOperation == XPAN_BEARER_SWITCH_CANCEL) {
    //nothing to do, Ignore
    mOperation = XPAN_OP_NONE;
    return;
  }

  if (status == XPAN_AC_SUCCESS) {
    switch (mTransitionType) {
      case XPAN_LE_TO_AP_IDLE:
      case XPAN_LE_TO_AP_STREAMING: {
             /* Send L2CAP Pause/Unpause */
             if (action == PAUSE) {
               /* mOperation is set here for LE->AP IDLE. For Streaming case, its already set
                 thus overriden */
               mOperation = XPAN_BEARER_SWITCH;
               mBearerSwitchState = XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ;
               lmp->SendLmpL2capPauseUnpauseReq(PAUSE, LMP_TP_LE);
             } else if (action == UNPAUSE) {
               // nothing to do
             }
           } break;

      case XPAN_AP_TO_LE_IDLE:
      case XPAN_AP_TO_LE_STREAMING:
      case XPAN_AP_TO_AP_ROAMING_IDLE:
      case XPAN_AP_TO_AP_ROAMING_STREAMING:
           /* Send L2CAP Pause/Unpause */
           if (action == PAUSE) {
             mBearerSwitchState = XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ;
             lmp->SendLmpL2capPauseUnpauseReq(PAUSE, LMP_TP_AP);
           } else if (action == UNPAUSE) {
             // nothing to do
           }
           break;

      default:
        ALOGE("%s %s: Unexpected Transition", R, __func__);
    }
  } else {
    /* QHCI rejected pause-unpause */
    switch (mTransitionType) {
      case XPAN_LE_TO_AP_IDLE:
      case XPAN_LE_TO_AP_STREAMING:
           /*TODO: check its bearer switch procedure before sending cancel ind */
           /* Cancel bearer switch and send indication */
           mOperation = XPAN_BEARER_SWITCH_CANCEL;
           lmp->SendLmpCancelBearerSwitchInd(LMP_TP_LE, LMP_TP_AP, 0xFF);
           if (mEarbudRole == ROLE_PRIMARY) {
             XpanApplicationController *xac = XpanApplicationController::Get();
             XpanDevice* dev = xac ? xac->GetDevice(mBdAddr): NULL;
             if (dev) {
               ssm = dev->GetStateMachine(ROLE_SECONDARY);
             }
             if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_LE, LMP_TP_AP, 0xFF);
           }
           if (action == UNPAUSE) {
             // not a desired scenario. QHCI never rejects
             qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, BT_LE);
           }
           break;

      case XPAN_AP_TO_LE_IDLE:
      case XPAN_AP_TO_LE_STREAMING:
           /*TODO: check its bearer switch procedure before sending cancel ind */
           lmp->SendLmpCancelBearerSwitchInd(LMP_TP_AP, LMP_TP_LE, 0xFF);
           if (mEarbudRole == ROLE_PRIMARY) {
             XpanApplicationController *xac = XpanApplicationController::Get();
             XpanDevice* dev = xac ? xac->GetDevice(mBdAddr): NULL;
             if (dev) {
               ssm = dev->GetStateMachine(ROLE_SECONDARY);
             }
             if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                        LMP_TP_LE, 0xFF);
           }
           if (action == UNPAUSE) {
             // not a desired scenario. QHCI never rejects
             qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, XPAN_AP);
           }
           break;

      case XPAN_AP_TO_AP_ROAMING_IDLE:
      case XPAN_AP_TO_AP_ROAMING_STREAMING:
           /*TODO: check its bearer switch procedure before sending cancel ind */
           /* Proceed with L2cap Pause Unpause, wait to QHCI callback */
           lmp->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                  LMP_TP_AP, 0xFF);
           if (mEarbudRole == ROLE_PRIMARY) {
             XpanApplicationController *xac = XpanApplicationController::Get();
             XpanDevice* dev = xac ? xac->GetDevice(mBdAddr): NULL;
             if (dev) {
               XpanAcStateMachine* ssm = dev->GetStateMachine(ROLE_SECONDARY);
             }
             if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                            LMP_TP_AP, 0xFF);
           }
           if (action == UNPAUSE) {
             // not a desired scenario. QHCI never rejects
             qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, XPAN_AP);
           }
           break;

      default:
        ALOGE("%s %s: Unexpected Transition", R, __func__);
        // disconnect connection
        //setState(XPAN_DISCONNECTING, evt);
    }
  }
}

void XpanAcStateMachine::HandleLmpPauseUnpauseRspEvt(xac_handler_msg_t* msg) {
  uint8_t action = msg->l2cPauseUnpause.action; // pause/unpause
  uint8_t status = msg->l2cPauseUnpause.status; // accepted/rejected
  XpanAcStateMachine *ssm = NULL;
  XpanApplicationController *xac = NULL;
  XpanDevice* dev = NULL;

  xac = XpanApplicationController::Get();
  dev = xac ? xac->GetDevice(mBdAddr): NULL;
  if (dev) {
   ssm = dev->GetStateMachine(ROLE_SECONDARY);
  }

  ALOGD("%s %s: action = %d, status = %d mOp = %d", R, __func__, action, status, mOperation);

  if (status == XPAN_AC_SUCCESS) {
    switch (mTransitionType) {
      case XPAN_LE_TO_AP_IDLE:
      case XPAN_LE_TO_AP_STREAMING:
           if (action == PAUSE) {
            mBearerSwitchState = XPAN_LMP_BEARER_SWITCH_REQ;
            lmp->SendLmpBearerSwitchReq(LMP_TP_LE, LMP_TP_AP);
           } else if (action == UNPAUSE && mOperation == XPAN_BEARER_SWITCH) {
             // Send Bearer Switch Complete Ind to Secondary
             qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, XPAN_AP);
             if (ssm)
               ssm->SendLmpBearerSwitchCompleteInd(LMP_TP_LE, LMP_TP_AP);
             /* Role check not required since LMP BS ops are done with primary */
             if (mTransitionType == XPAN_LE_TO_AP_IDLE) {
               mApDetailsOkToSend = true;
               setState(XPAN_AP_ACTIVE, XPAN_LMP_L2CAP_PAUSE_UNPAUSE_RES_EVT);
               HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_SUCCESS);
               xm->OnCurrentTransportUpdated(mBdAddr, XPAN_AP);
               XpanAcStateMachine::TriggerFilteredScan(this);
             } else if (mTransitionType == XPAN_LE_TO_AP_STREAMING) {
               setState(XPAN_AP_ACTIVE, XM_BEARER_SWITCH_IND_EVT);
             }
             mOperation = XPAN_OP_NONE;
             mBearerSwitchState = -1;
             if (isPingEnabled) {
               XpanLmpManager::StartSupervisionTimer(lmp);
             }
           } else if (action == UNPAUSE && mOperation == XPAN_BEARER_SWITCH_CANCEL) {
             // Send BS cancel indication to both earbuds
             lmp->SendLmpCancelBearerSwitchInd(LMP_TP_LE, LMP_TP_AP, 0xFF);
             if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_LE, LMP_TP_AP,
                  msg->l2cPauseUnpause.event);
             //qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, BT_LE);
             setState(XPAN_DISCONNECTING, msg->l2cPauseUnpause.event);
             XpanAcSmExecuteEvent(msg);
           }
           break;

      case XPAN_AP_TO_LE_IDLE:
      case XPAN_AP_TO_LE_STREAMING:
           if (action == PAUSE) {
            mBearerSwitchState = XPAN_LMP_BEARER_SWITCH_REQ;
            lmp->SendLmpBearerSwitchReq(LMP_TP_AP, LMP_TP_LE);
           } else if (action == UNPAUSE && mOperation == XPAN_BEARER_SWITCH) {
             // Send Bearer Switch Complete Ind to Secondary
             qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, BT_LE);
             if (ssm)
               ssm->SendLmpBearerSwitchCompleteInd(LMP_TP_AP, LMP_TP_LE);
             if (mTransitionType == XPAN_AP_TO_LE_IDLE) {
               HandleBearerPreferenceCmpl(mBdAddr, BT_LE, XM_SUCCESS);
               setState(XPAN_DISCONNECTING, XPAN_LMP_L2CAP_PAUSE_UNPAUSE_RES_EVT);
               XpanAcSmExecuteEvent(msg);
             } else if (mTransitionType == XPAN_AP_TO_LE_STREAMING) {
               setState(XPAN_DISCONNECTING, XPAN_LMP_L2CAP_PAUSE_UNPAUSE_RES_EVT);
               XpanAcSmExecuteEvent(msg);
             }
             mOperation = XPAN_OP_NONE;
             mBearerSwitchState = -1;
           } else if (action == UNPAUSE && mOperation == XPAN_BEARER_SWITCH_CANCEL) {
             // Send BS cancel indication to both earbuds
             lmp->SendLmpCancelBearerSwitchInd(LMP_TP_AP, LMP_TP_LE, 0xFF);
             if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                            LMP_TP_LE, 0xFF);
             qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, XPAN_AP);
           }
           break;

       case XPAN_AP_TO_AP_ROAMING_IDLE:
       case XPAN_AP_TO_AP_ROAMING_STREAMING:
           if (action == PAUSE) {
            mBearerSwitchState = XPAN_LMP_BEARER_SWITCH_REQ;
            lmp->SendLmpBearerSwitchReq(LMP_TP_AP, LMP_TP_AP);
           } else if (action == UNPAUSE && mOperation == XPAN_BEARER_SWITCH) {
             UpdateApTransport();
             qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, XPAN_AP);
             // Send Bearer Switch Complete Ind to Secondary
             if (ssm)
               ssm->SendLmpBearerSwitchCompleteInd(LMP_TP_AP, LMP_TP_AP);
             if (mTransitionType == XPAN_AP_TO_AP_ROAMING_IDLE) {
                setState(XPAN_AP_ACTIVE, XPAN_LMP_L2CAP_PAUSE_UNPAUSE_RES_EVT);
                HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_SUCCESS);
             } else if (mTransitionType == XPAN_AP_TO_AP_ROAMING_STREAMING) {
                setState(XPAN_AP_ACTIVE, XM_BEARER_SWITCH_IND_EVT);
                // prepare bearer switch rsp callback to XM sent already
             }
             mOperation = XPAN_OP_NONE;
             mBearerSwitchState = -1;
           } else if (action == UNPAUSE && mOperation == XPAN_BEARER_SWITCH_CANCEL) {
             // Send BS cancel indication to both earbuds
             lmp->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                        LMP_TP_AP, 0xFF);
             if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                        LMP_TP_AP, 0xFF);
             qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, XPAN_AP);
           }
           break;

      default:
        ALOGE("%s %s: Unexpected Transition", R, __func__);
    }
  } else {
    /* Remote rejected Lmp Pause-Unpause */
    mOperation = XPAN_BEARER_SWITCH_CANCEL;
    switch (mTransitionType) {
      case XPAN_LE_TO_AP_IDLE:
      case XPAN_LE_TO_AP_STREAMING:
           if (action == PAUSE) {
             qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, BT_LE);
             lmp->SendLmpCancelBearerSwitchInd(LMP_TP_LE,
                                               LMP_TP_AP, 0xFF);
             if (mEarbudRole == ROLE_PRIMARY) {
               if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_LE, LMP_TP_AP,
                            msg->l2cPauseUnpause.event);
             }
             setState(XPAN_DISCONNECTING, msg->l2cPauseUnpause.event);
             if (mTransitionType == XPAN_LE_TO_AP_IDLE) {
               HandleBearerPreferenceCmpl(mBdAddr, XPAN_AP, XM_AC_BEARER_PREFERENCE_REJECTED);
             } else {
               xm->PrepareAudioBearerRsp(mBdAddr, XM_AC_PREPARE_AUDIO_BEARER_FAILED);
             }
             XpanAcSmExecuteEvent(msg);
           } else if (action == UNPAUSE) {
             /* note: Error in unpause -> Disconnect */
             setState(XPAN_DISCONNECTING, msg->l2cPauseUnpause.event); // primary
             XmRemoteDisconnectedEvent s_eb_disconn{XPAN_REMOTE_DISCONNECTED_EVT,
                                                    mBdAddr, ROLE_SECONDARY};
             // disconnect secondary
             if (dev) dev->RemoteDisconnectedEvent((xac_handler_msg_t *)&s_eb_disconn);
           }
           break;

      case XPAN_AP_TO_LE_IDLE:
      case XPAN_AP_TO_LE_STREAMING:
      case XPAN_AP_TO_AP_ROAMING_IDLE:
      case XPAN_AP_TO_AP_ROAMING_STREAMING:
           xac = XpanApplicationController::Get();
           dev = xac ? xac->GetDevice(mBdAddr): NULL;
           if (dev) {
             ssm = dev->GetStateMachine(ROLE_SECONDARY);
           }
           if (action == PAUSE) {
             if (mEarbudRole == ROLE_PRIMARY) {
               qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, BT_LE);
               if (mTransitionType != XPAN_AP_TO_AP_ROAMING_IDLE &&
                   mTransitionType != XPAN_AP_TO_AP_ROAMING_STREAMING) {
                 lmp->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                                                   LMP_TP_LE, 0xFF);
                 if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                                                            LMP_TP_LE, 0xFF);
               } else {
                 lmp->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                          LMP_TP_AP, 0xFF);
                 if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                          LMP_TP_AP, 0xFF);
               }
             }
           } else if (action == UNPAUSE) {
             /* note: Error in unpause -> Disconnect */
             setState(XPAN_DISCONNECTING, msg->l2cPauseUnpause.event); // primary
             // disconnect secondary
             XmRemoteDisconnectedEvent s_eb_disconn{XPAN_REMOTE_DISCONNECTED_EVT,
                                                    mBdAddr, ROLE_SECONDARY};
             // secondary disconnect
             if (dev) dev->RemoteDisconnectedEvent((xac_handler_msg_t *)&s_eb_disconn);
           }
           break;

      default:
        ALOGE("%s %s: Unexpected Transition", R, __func__);
        // disconnect connection
        //setState(XPAN_DISCONNECTING, evt);
    }
  }
}

void XpanAcStateMachine::HandleLmpBearerSwitchRspEvt(xac_handler_msg_t* msg) {
  uint8_t status = msg->bearerSwitchRsp.status;

  if (status == XPAN_AC_SUCCESS) {
    switch (mTransitionType) {
      case XPAN_LE_TO_AP_IDLE:
      case XPAN_AP_TO_AP_ROAMING_IDLE:
           mBearerSwitchState = XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ;
           lmp->SendLmpL2capPauseUnpauseReq(UNPAUSE, LMP_TP_AP);
           break;

      case XPAN_AP_TO_LE_IDLE:
           mBearerSwitchState = XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ;
           lmp->SendLmpL2capPauseUnpauseReq(UNPAUSE, LMP_TP_LE);
           break;

      case XPAN_LE_TO_AP_STREAMING:
      case XPAN_AP_TO_AP_ROAMING_STREAMING:
      case XPAN_AP_TO_LE_STREAMING:
           // received only for primary so no role check required
           xm->PrepareAudioBearerRsp(mBdAddr, XM_SUCCESS);
           break;

      default:
        ALOGE("%s %s: Unexpected Transition", R, __func__);
    }
  } else {
    /* Remote rejected Bearer switch request
       Steps:
       1. Unpause previous transport (with QHCI and remote)
       2. Send Bearer Switch Cancel Indication to both buds
    */
    mOperation = XPAN_BEARER_SWITCH_CANCEL;
    switch (mTransitionType) {
      case XPAN_LE_TO_AP_IDLE:
      case XPAN_LE_TO_AP_STREAMING:
           qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, BT_LE);
           lmp->SendLmpL2capPauseUnpauseReq(UNPAUSE, LMP_TP_LE);
           break;

      case XPAN_AP_TO_LE_IDLE:
      case XPAN_AP_TO_LE_STREAMING:
      case XPAN_AP_TO_AP_ROAMING_IDLE:
      case XPAN_AP_TO_AP_ROAMING_STREAMING:
           qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, XPAN_AP);
           lmp->SendLmpL2capPauseUnpauseReq(UNPAUSE, LMP_TP_AP);
           break;

      default:
        ALOGE("%s %s: Unexpected Transition", R, __func__);
        // disconnect connection
        //setState(XPAN_DISCONNECTING, evt);
    }
  }
}

void XpanAcStateMachine::HandleXmBearerSwitchInd(xac_handler_msg_t* msg) {
  uint8_t status = msg->bearerSwitchInd.status;

  /* Get device & ssm instance*/
  XpanAcStateMachine *ssm = NULL;
  XpanApplicationController *xac = XpanApplicationController::Get();
  XpanDevice* dev = xac ? xac->GetDevice(mBdAddr): NULL;
  if (dev) {
   ssm = dev->GetStateMachine(ROLE_SECONDARY);
  }

  if (status == XPAN_AC_SUCCESS) {
    /* Steps:
       1. Unpause ACL traffic
       2. Send Bearer Switch Complete Ind to secondary after unapuse response
    */
    switch(mTransitionType) {
      case XPAN_LE_TO_AP_STREAMING:
           mBearerSwitchState = XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ;
           lmp->SendLmpL2capPauseUnpauseReq(UNPAUSE, LMP_TP_AP);
           XpanAcStateMachine::TriggerFilteredScan(this);
           break;

      case XPAN_AP_TO_LE_STREAMING:
           mBearerSwitchState = XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ;
           lmp->SendLmpL2capPauseUnpauseReq(UNPAUSE, LMP_TP_LE);
           //qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, BT_LE);
           break;

      case XPAN_AP_TO_AP_ROAMING_STREAMING:
           mBearerSwitchState = XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ;
           lmp->SendLmpL2capPauseUnpauseReq(UNPAUSE, LMP_TP_AP);
           //qhci->L2capPauseUnpauseReq(mBdAddr, UNPAUSE, XPAN_AP);
           break;

      default:
           ALOGD("%s %s: Bearer Switch indication for undesired transport %d",
                 R, __func__, mTransitionType);
    }
  } else {
    /* XM send Bearer switch failed indication */
    /* Determine at what point bearer switch failed and execute next steps
       accordingly.
     */
    /* Set that Bearer Switch Cancel procedure is initiated*/
     mOperation = XPAN_BEARER_SWITCH_CANCEL;

     uint8_t curLmpOperation = mBearerSwitchState;
     uint8_t pauseAction = 0/*TODO: lmp->getCurrentLmpOperation()*/;
     ALOGD("%s %s: bearer switch failed ind from XM while performing operation:%d",
           R, __func__, curLmpOperation);
     switch (curLmpOperation) {
       case XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ:
       case XPAN_LMP_BEARER_SWITCH_REQ:
            /* Unpause L2CAP traffic on previous transport and then bearer switch
               cancel indication to both is sent from Lmp pause/unpause handler */
            switch(mTransitionType) {
              case XPAN_LE_TO_AP_IDLE:
              case XPAN_LE_TO_AP_STREAMING:
                   setState(XPAN_DISCONNECTING, msg->event);
                   lmp->SendLmpL2capPauseUnpauseReq(UNPAUSE, LMP_TP_LE);
                   lmp->SendLmpCancelBearerSwitchInd(BT_LE, XPAN_AP, 0xFF);
                   break;

              case XPAN_AP_TO_LE_IDLE:
              case XPAN_AP_TO_LE_STREAMING:
              case XPAN_AP_TO_AP_ROAMING_IDLE:
              case XPAN_AP_TO_AP_ROAMING_STREAMING:
                  lmp->SendLmpL2capPauseUnpauseReq(UNPAUSE, LMP_TP_AP);
                  break;

              default:
                   ALOGE("%s %s: Undesired transition(%d). Ignore", R, __func__, mTransitionType);
            }
            break;

       case XPAN_LMP_PREPARE_BEARER_SWITCH_REQ:
            /* Just send bearer switch Cancel Indication as L2CAP Pause/Unpause
               is not even initiated */
            switch(mTransitionType) {
              case XPAN_LE_TO_AP_IDLE:
              case XPAN_LE_TO_AP_STREAMING:
                   lmp->SendLmpCancelBearerSwitchInd(LMP_TP_LE,
                          LMP_TP_AP, 0xFF);
                   if (ssm) ssm->SendLmpCancelBearerSwitchInd(LMP_TP_LE,
                          LMP_TP_AP, 0xFF);
                   setState(XPAN_DISCONNECTING, msg->event);
                   XpanAcSmExecuteEvent(msg);
                   break;

              case XPAN_AP_TO_LE_IDLE:
              case XPAN_AP_TO_LE_STREAMING:
                  lmp->SendLmpCancelBearerSwitchInd(
                      LMP_TP_AP, LMP_TP_LE, 0xFF);
                  if (ssm) ssm->SendLmpCancelBearerSwitchInd(
                          LMP_TP_AP, LMP_TP_LE, 0xFF);
                  setState(XPAN_AP_ACTIVE, msg->event);
                  break;

              case XPAN_AP_TO_AP_ROAMING_IDLE:
              case XPAN_AP_TO_AP_ROAMING_STREAMING:
                   lmp->SendLmpCancelBearerSwitchInd(
                        LMP_TP_AP, LMP_TP_AP, 0xFF);
                   if (ssm) ssm->SendLmpCancelBearerSwitchInd(
                      LMP_TP_AP, LMP_TP_AP, 0xFF);
                   setState(XPAN_AP_ACTIVE, msg->event);
                   break;
              default:
                   ALOGE("%s %s: Undesired transition(%d). Ignore", R, __func__, mTransitionType);
            }

       default:
            ALOGD("%s %s: undesired LMP operation. Nothing to be done.", R, __func__);
     }
  }
}

void XpanAcStateMachine::HandleNewTcpTransportReadyEvt(xac_handler_msg_t* msg) {
  uint8_t status = msg->newApTransportStatus.status;

  if (status == XPAN_AC_SUCCESS) {
    switch (mTransitionType) {
      case XPAN_AP_TO_AP_ROAMING_IDLE:
           newTcpTransportReady = true;
           /* Proceed with L2cap Pause Unpause (idle transition) & wait to QHCI callback */
           if (newTcpTransportReady && mTransitionType == XPAN_AP_TO_AP_ROAMING_IDLE) {
            qhci->L2capPauseUnpauseReq(mBdAddr, PAUSE, XPAN_AP);
           } else if (mTransitionType == XPAN_AP_TO_AP_ROAMING_STREAMING) {
             //TODO: callback to XM with remote AP details
           }
           break;

      case XPAN_AP_TO_AP_ROAMING_STREAMING:
           newTcpTransportReady = true;
           /* Proceed with L2cap Pause Unpause (idle transition) & wait to QHCI callback */
           if (prepBearerApRoamingDone && mTransitionType == XPAN_AP_TO_AP_ROAMING_IDLE) {
            qhci->L2capPauseUnpauseReq(mBdAddr, PAUSE, XPAN_AP);
           } else if (mTransitionType == XPAN_AP_TO_AP_ROAMING_STREAMING) {
             //TODO: callback to XM with remote AP details
           }
           break;

      default:
          ALOGE("%s %s: Unexpected transition (%d)", R, __func__, mTransitionType);
    }
  } else {
    switch (mTransitionType) {
      case XPAN_AP_TO_AP_ROAMING_IDLE:
           // TODO: Send Bearer preference failed rsp to XM
           lmp->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                                             LMP_TP_AP, 0xFF);
           // roll back to previous AP transport
           setState(XPAN_AP_ACTIVE, XPAN_TCP_NEW_AP_TRANSPORT_STATUS);
           break;

      case XPAN_AP_TO_AP_ROAMING_STREAMING:
           // TODO: Send prepare bearer failed rsp to XM
           // sent to both earbuds
           lmp->SendLmpCancelBearerSwitchInd(LMP_TP_AP,
                                             LMP_TP_AP, 0xFF);
           // roll back to previous AP transport
           setState(XPAN_AP_ACTIVE, XPAN_TCP_NEW_AP_TRANSPORT_STATUS);
           break;

      default:
          ALOGE("%s %s: Unexpected transition (%d)", R, __func__, mTransitionType);
    }
  }
}

void XpanAcStateMachine::UpdateApTransport() {
  /* Update AP details to new transport details */
  mIpAddr = mIpRoaming;
  mMacAddr = mMacRoaming;
  mTcpPort = mTcpPortRoaming;

  /* Unset Roaming details */
  mIpRoaming = {};
  mMacAddr = {};
  mTcpPort = 0;

  /* Update IP address in LMP Manager */
  lmp->SetIpAddress(mIpAddr);

  /* Set new transport in socket handler */
  mSocketHdlr->HandleRoamingCompletion();
}

void XpanAcStateMachine::HandleXpanPrimarySwitchReq(xac_handler_msg_t* msg, uint8_t action) {
  uint8_t status = msg->primarySwitchReq.operation;

  if (status == XPAN_AC_SUCCESS) {
    if (action == PAUSE) {
      lmp->SendLmpAccepted();
    }
  }
}

void XpanAcStateMachine::HandleSocketClosure() {
  if (!mSocketHdlr) {
    ALOGE("%s %s: Socket Handler already cleared", R, __func__);
    return;
  }

  // close the connection
  mSocketHdlr->CloseConnectionSocket();

  mSocketFd = -1;
}

XpanDevice* XpanAcStateMachine::GetDeviceInstance(mdns_uuid_t uuid) {
  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s %s: Xpan Application Controller is not initialized.", R, __func__);
    return NULL;
  }

  XpanDevice *dev = xac->GetDeviceByMdnsUuid(uuid);
  if (dev) {
    return dev;
  }
  return NULL;
}

int XpanAcStateMachine::GetSocketFd() {
  if (mSocketHdlr) {
    return mSocketHdlr->GetTcpFd();
  }
  return -1;
}

bdaddr_t XpanAcStateMachine::GetAddr() {
  return mBdAddr;
}

ipaddr_t XpanAcStateMachine::GetIpAddr() {
   return mIpAddr;
}

XpanEarbudRole XpanAcStateMachine::GetRole() {
  return mEarbudRole;
}

XpanTransitionType XpanAcStateMachine::GetCurrentTransition() {
  return mTransitionType;
}

TransportType XpanAcStateMachine::GetTransportFrom() {
  return mTransportFrom;
}

bool XpanAcStateMachine::IsRemoteApDetailsOkToSend() {
  return mApDetailsOkToSend;
}

void XpanAcStateMachine::SendLmpCancelBearerSwitchInd(uint8_t bearer_from, uint8_t bearer_to,
                                                      uint16_t err) {
  if (mCurrentState == XPAN_BEARER_SWITCH_PENDING) {
    ALOGD("%s %s: Secondary to send LMP Bearer Switch Cancel indication."
          ", bearer_from = %d, bearer_to = %d", R, __func__, bearer_from, bearer_to);
    lmp->SendLmpCancelBearerSwitchInd(bearer_from, bearer_to, err);
    if (bearer_to == LMP_TP_AP && bearer_from == LMP_TP_LE) {
      setState(XPAN_DISCONNECTING, (XacEvent)err);
      XpanBearerSwitchResEvt msg;
      msg.event = (XacEvent)XPAN_LMP_CANCEL_BEARER_SWITCH;
      XpanAcSmExecuteEvent((xac_handler_msg_t *)&msg);

      if (mEarbudRole == ROLE_SECONDARY && mTransitionType == XPAN_LE_TO_AP_IDLE) {
        if (mBearerPrefTimer) {
          stopTimer(mBearerPrefTimer);
          mBearerPrefTimer = NULL;
        }
      }

    } else if (bearer_to == LMP_TP_LE && bearer_from == LMP_TP_AP) {
      setState(XPAN_AP_ACTIVE, (XacEvent)err);

      if (mEarbudRole == ROLE_SECONDARY && mTransitionType == XPAN_AP_TO_LE_IDLE) {
        if (mBearerPrefTimer) {
          stopTimer(mBearerPrefTimer);
          mBearerPrefTimer = NULL;
        }
      }
    }
  }
}

void XpanAcStateMachine::SendLmpBearerSwitchCompleteInd(uint8_t bearer_from, uint8_t bearer_to) {
  if (mCurrentState == XPAN_BEARER_SWITCH_PENDING) {
    ALOGD("%s %s: Secondary to send LMP Bearer Switch complete indication."
          ", bearer_from = %d, bearer_to = %d", R, __func__, bearer_from, bearer_to);
    lmp->SendLmpBearerSwitchCmplInd(bearer_from, bearer_to);
    if (bearer_to == LMP_TP_AP) {
      setState(XPAN_AP_ACTIVE, XM_BEARER_SWITCH_IND_EVT);
      if (mEarbudRole == ROLE_SECONDARY && mTransitionType == XPAN_LE_TO_AP_IDLE) {
        stopTimer(mBearerPrefTimer);
        mBearerPrefTimer = NULL;
      }
      if (isPingEnabled && bearer_from == LMP_TP_LE) {
        XpanLmpManager::StartSupervisionTimer(lmp);
      }
    } else if (bearer_to == LMP_TP_LE && bearer_from == LMP_TP_AP) {
      setState(XPAN_DISCONNECTING, XM_BEARER_SWITCH_IND_EVT);
      XpanL2capPauseUnpauseResEvt l2cPauseUnpause = {.event = XPAN_LMP_L2CAP_PAUSE_UNPAUSE_RES_EVT};
      XpanAcSmExecuteEvent((xac_handler_msg_t *)&l2cPauseUnpause);
    }
  }
}

void XpanAcStateMachine::HandleBearerPreferenceCmpl(bdaddr_t addr,
    TransportType transport, RspStatus status) {
  if (mBearerPrefTimer) {
    //ALOGD("%s: Stopping the bearer prefernece timer", __func__);
    stopTimer(mBearerPrefTimer);
    mBearerPrefTimer = NULL;
  }

  xm = XMXacIf::GetIf();
  if (xm && mEarbudRole == ROLE_PRIMARY) {
    xm->BearerPreferenceRsp(addr, transport, status);
  }
}

void XpanAcStateMachine::UpdatePrimaryDisconnToSec() {
  if (mEarbudRole != ROLE_PRIMARY) {
    return;
  }

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return;
  }

  XpanPrimaryDiscEvt *msg =
      (XpanPrimaryDiscEvt *)malloc(sizeof(XpanPrimaryDiscEvt));
  msg->event = XPAN_PRIMARY_DISCONNECTING_EVT,
  msg->addr = mBdAddr;

  xac->PostMessage((xac_handler_msg_t *)msg, MSG_PRIORITY_DEFAULT);
}

void XpanAcStateMachine::SetFilteredScanTimer() {
    mFilteredScanTimer = new XpanAcTimer("MFilterScanTimer", TriggerFilteredScan, this);
    mFilteredScanTimer->StartTimer(XPAN_AC_FILTERED_SCAN_TRIGGER_TIMER);
}

void XpanAcStateMachine::Cleanup() {
  ALOGD("%s %s: BDADDR(%s), IP(%s) Role(%d)", R, __func__, mBdAddr.toString().c_str(),
         mIpAddr.toString().c_str(), mEarbudRole);

  if (mDnsQueryTimer) {
    ALOGD("%s %s: Stopping mDnsQueryTimer", R, __func__);
    stopTimer(mDnsQueryTimer);
    mDnsQueryTimer = NULL;
  }

  if (mFilteredScanTimer) {
    ALOGD("%s %s: Stopping mDnsQueryTimer", R, __func__);
    stopTimer(mFilteredScanTimer);
    mFilteredScanTimer = NULL;
  }


  if (mSocketHdlr) {
    ALOGD("%s %s: cleaning up Socket Handler", R, __func__);
    delete mSocketHdlr;
    mSocketHdlr = NULL;
  }

  if (lmp) {
    ALOGD("%s %s: cleaning up Lmp Manager", R, __func__);
    delete lmp;
    lmp = NULL;
  }
}

/* To stop and delete Timer */
void stopTimer(XpanAcTimer *timer) {
  ALOGD("%s: Stop and Delete Timer", __func__);
  timer->StopTimer();
  timer->DeleteTimer();
  delete(timer);
}

} // namespace ac
} // namespace xpan
