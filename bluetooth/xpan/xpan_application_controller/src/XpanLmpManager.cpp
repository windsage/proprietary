/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <log/log.h>
#include "xpan_ac_int.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.bluetooth.xpan_ac.XpanLmpManager"

/* Xpan LMP Manager Implementation.*/

namespace xpan {
namespace ac {

std::mutex XpanLmpManager::mLmpOpLock;

XpanLmpManager::XpanLmpManager() {
  ALOGD("%s", __func__);
  mSupervisionTimeout = 10000; // default 10 sec
  mPingTimeout = mSupervisionTimeout * 0.75;
}

XpanLmpManager::~XpanLmpManager() {
  mLmpCmdQue.clear();
  mCurrentLmpOp = -1;
  mSocketHdlr = NULL;

  if (mPingTimer) {
    stopTimer(mPingTimer);
    mPingTimer = NULL;
  }

  if (mSupervisionTimer) {
    stopTimer(mSupervisionTimer);
    mSupervisionTimer = NULL;
  }
  ALOGD("%s: Completed", __func__);
}

void XpanLmpManager::LmpExecute(int8_t opcode, std::vector<uint8_t> data) {
  LmpOp op{opcode, data};
  // fetch currently executing op before queing/scehduling next op
  uint8_t curExeOp = GetCurrentExeOp();

  if (!mLmpCmdQue.empty() && opcode != curExeOp) {
    ALOGD("%s: Lmp operation (%s) is currently executing. Queue (%s) operation", __func__,
          LmpOpString(curExeOp));
    LmpEnqueue(op);
    return;
  }

  if (mCurrentLmpOp != opcode) {
    mCurrentLmpOp = opcode;
    LmpEnqueue(op);
  }

  ALOGD("%s: Execute %s", __func__, LmpOpString(mCurrentLmpOp));
  // Send to SocketManager for sending data over TCP connection
  mSocketHdlr->SendData(XPAN_LMP_MSG, data);
}

void XpanLmpManager::LmpEnqueue(LmpOp op) {
  mLmpCmdQue.push_back(op);
}

void XpanLmpManager::LmpExecuteNextOp() {
  mLmpOpLock.lock();
  ALOGD("%s: operation %s completed. Remove it from pending queue.", __func__,
        LmpOpString(mCurrentLmpOp));
  if (!mLmpCmdQue.empty()) {
    mLmpCmdQue.pop_front();
    mCurrentLmpOp = -1; // resetting if there is no further queued op
  }

  if (mLmpCmdQue.empty()) {
    ALOGD("%s: No Lmp operation to execute ", __func__);
    mLmpOpLock.unlock();
    return;
  }

  LmpOp op = mLmpCmdQue.front();
  mCurrentLmpOp = op.opcode;
  ALOGD("%s: Execute %s", __func__, LmpOpString(mCurrentLmpOp));
  LmpExecute(op.opcode, op.data);
  mLmpOpLock.unlock();
}

int8_t XpanLmpManager::GetCurrentExeOp() {
  return mCurrentLmpOp;
}

uint8_t XpanLmpManager::GetCurrentPauseUnpauseAction() {
  return l2cPauseUnpauseVal;
}

void XpanLmpManager::SetL2capPauseUnpauseAction(uint8_t val) {
  l2cPauseUnpauseVal = val;
}

void XpanLmpManager::SetIpAddress(ipaddr_t ip) {
  mIpAddr = ip;
}

/* Utility API to check if LMP Opcode is valid */
bool IsValidLmpOpcode(uint8_t opcode) {
  return
    ((opcode >= XPAN_LMP_ACCEPTED && opcode <= XPAN_LMP_BEARER_SWITCH_REQ) ||
    (opcode == XPAN_LMP_HOST_CONNECTION_REQ) ||
    (opcode >= XPAN_LMP_BEARER_SWITCH_COMPLETE_IND && opcode <= XPAN_LMP_CANCEL_BEARER_SWITCH));
}

/* Parser API's for remote LMP response */
uint8_t XpanLmpManager::parseLmpConnectionReq(uint8_t *p, uint16_t len, ipaddr_t ip, int fd) {
  XpanLmpIncomingConnReqEvt *req =
      (XpanLmpIncomingConnReqEvt *)malloc(sizeof(XpanLmpIncomingConnReqEvt));

  req->event = XPAN_LMP_INC_CONNECTION_REQ_EVT;
  req->ip = ip;
  req->fd = fd;

  DATA_TO_UINT_8(req->link_type, p);
  DATA_TO_UINT8_ARRAY(req->remote_mdns_uuid.b, p, 16);
  DATA_TO_UINT8_ARRAY(req->cod, p, 3);

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return XPAN_CONNECTION_FAILED_TO_ESTABLISH;
  }

  /* Validate if the mdns uuid belongs to any bonded device.
   * If not, send reject and close fd
   */
  XpanDevice *dev = xac->GetDeviceByMdnsUuid(req->remote_mdns_uuid);
  if (!dev) {
    ALOGE("%s: Couldnt find device for mdns uuid %s", __func__,
          req->remote_mdns_uuid.toString().c_str());
    return XPAN_CONNECTION_FAILED_TO_ESTABLISH;
  }

  XpanAcStateMachine *psm = dev->GetStateMachine(ROLE_PRIMARY);
  if (!psm || psm->GetState() != XPAN_TCP_CONNECTING) {
    ALOGE("%s: Incoming LMP Connect request when acting primary"
          " current State: %d, Reject the request", __func__, (psm ? psm->GetState(): XPAN_IDLE));
    return XPAN_CONNECTION_ALREADY_EXISTS;
  }

  req->addr = dev->GetAddr();
  xac->PostMessage((xac_handler_msg_t *)req, MSG_PRIORITY_DEFAULT);

  return XPAN_AC_SUCCESS;
}

uint8_t XpanLmpManager::parseLmpLeFeatureReq(uint8_t *p, uint16_t len, ipaddr_t ip) {
  XpanLmpFeatureReqEvt *req =
      (XpanLmpFeatureReqEvt *)malloc(sizeof(XpanLmpFeatureReqEvt));

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return XPAN_CONN_DOESNT_EXIST;
  }

  XpanDevice *dev = xac->GetDeviceByIpAddr(ip);
  if (!dev) {
    ALOGE("%s: No device found for ip: %s", __func__, ip.toString().c_str());
    return XPAN_CONN_DOESNT_EXIST;
  }

  req->event = XPAN_LMP_LE_FEATURE_REQ_EVT;
  req->addr = dev->GetAddr();
  req->ip = ip;
  DATA_TO_UINT8_ARRAY(req->mdns_uuid.b, p, 16);
  DATA_TO_UINT_64(req->le_features, p);

  xac->PostMessage((xac_handler_msg_t *)req, MSG_PRIORITY_DEFAULT);
  return XPAN_AC_SUCCESS;
}

uint8_t XpanLmpManager::parseLmpLeFeatureRes(uint8_t *p, uint16_t len, ipaddr_t ip) {
  /*Schedule the next command if pending in LMP queue */
  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return XPAN_CONN_DOESNT_EXIST;
  }
  XpanLmpExecuteNextCmd *msg = (XpanLmpExecuteNextCmd *)malloc(sizeof(XpanLmpExecuteNextCmd));
  msg->event = XPAN_LMP_EXECUTE_NEXT_CMD;
  msg->ip = ip;
  xac->PostMessage((xac_handler_msg_t *)msg, MSG_PRIORITY_DEFAULT);

  /* Parse the response */
  XpanLmpFeatureResEvt *res =
      (XpanLmpFeatureResEvt *)malloc(sizeof(XpanLmpFeatureResEvt));

  XpanDevice *dev = xac->GetDeviceByIpAddr(ip);
  if (!dev) {
    ALOGE("%s: No device found for ip: %s", __func__, ip.toString().c_str());
    free(res);
    return XPAN_CONN_DOESNT_EXIST;
  }

  res->event = XPAN_LMP_LE_FEATURE_RES_EVT;
  res->addr = dev->GetAddr();
  res->ip = ip;

  DATA_TO_UINT8_ARRAY(res->mdns_uuid.b, p, 16);
  DATA_TO_UINT_64(res->le_features, p);

  xac->PostMessage((xac_handler_msg_t *)res, MSG_PRIORITY_DEFAULT);
  return XPAN_AC_SUCCESS;
}

uint8_t XpanLmpManager::parseLmpVersionReq(uint8_t *p, uint16_t len, ipaddr_t ip) {
  XpanVersionReqEvt *req =
      (XpanVersionReqEvt *)malloc(sizeof(XpanVersionReqEvt));

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return XPAN_CONN_DOESNT_EXIST;
  }

  XpanDevice *dev = xac->GetDeviceByIpAddr(ip);
  if (!dev) {
    ALOGE("%s: No device found for ip: %s", __func__, ip.toString().c_str());
    return XPAN_CONN_DOESNT_EXIST;
  }

  req->event = XPAN_LMP_VERSION_REQ_EVT;
  req->addr = dev->GetAddr();
  req->ip = ip;
  DATA_TO_UINT8_ARRAY(req->mdns_uuid.b, p, 16);
  DATA_TO_UINT_8(req->version, p);
  DATA_TO_UINT_16(req->company_id, p);
  DATA_TO_UINT_16(req->subversion, p);

  xac->PostMessage((xac_handler_msg_t *)req, MSG_PRIORITY_DEFAULT);
  return XPAN_AC_SUCCESS;
}

uint8_t XpanLmpManager::parseLmpVersionRes(uint8_t *p, uint16_t len, ipaddr_t ip) {
  /*Schedule the next command if pending in LMP queue */
  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return XPAN_CONN_DOESNT_EXIST;
  }
  XpanLmpExecuteNextCmd *msg = (XpanLmpExecuteNextCmd *)malloc(sizeof(XpanLmpExecuteNextCmd));
  msg->event = XPAN_LMP_EXECUTE_NEXT_CMD;
  msg->ip = ip;
  xac->PostMessage((xac_handler_msg_t *)msg, MSG_PRIORITY_DEFAULT);

  /* Parse the response */
  XpanDevice *dev = xac->GetDeviceByIpAddr(ip);
  if (!dev) {
    ALOGE("%s: No device found for ip: %s", __func__, ip.toString().c_str());
    return XPAN_CONN_DOESNT_EXIST;
  }

  XpanVersionResEvt *res =
      (XpanVersionResEvt *)malloc(sizeof(XpanVersionResEvt));

  res->event = XPAN_LMP_VERSION_RES_EVT;
  res->addr = dev->GetAddr();
  res->ip = ip;
  DATA_TO_UINT8_ARRAY(res->mdns_uuid.b, p, 16);
  DATA_TO_UINT_8(res->version, p);
  DATA_TO_UINT_16(res->company_id, p);
  DATA_TO_UINT_16(res->subversion, p);

  xac->PostMessage((xac_handler_msg_t *)res, MSG_PRIORITY_DEFAULT);
  return XPAN_AC_SUCCESS;
}

uint8_t XpanLmpManager::parseLmpRemotePingReq(uint8_t *p, uint16_t len, ipaddr_t ip) {
  XpanLmpRemotePingReqEvt *req =
      (XpanLmpRemotePingReqEvt *)malloc(sizeof(XpanLmpRemotePingReqEvt));

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return XPAN_CONN_DOESNT_EXIST;
  }

  req->event = XPAN_LMP_REMOTE_PING_REQ_EVT;
  req->ip = ip;

  xac->PostMessage((xac_handler_msg_t *)req, MSG_PRIORITY_DEFAULT);
  return XPAN_AC_SUCCESS;
}

uint8_t XpanLmpManager::parseLmpSwitchPrimaryReq(uint8_t *p, uint16_t len, ipaddr_t ip) {
  XpanPrimarySwitchEvt *req =
      (XpanPrimarySwitchEvt *)malloc(sizeof(XpanPrimarySwitchEvt));

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return XPAN_CONN_DOESNT_EXIST;
  }

  XpanDevice *dev = xac->GetDeviceByIpAddr(ip);
  if (!dev) {
    ALOGE("%s: No device found for ip: %s", __func__, ip.toString().c_str());
    return XPAN_CONN_DOESNT_EXIST;
  }

  req->event = XPAN_LMP_SWITCH_PRIMARY_EVT;
  req->addr = dev->GetAddr();
  req->ip = ip;
  DATA_TO_UINT_8(req->operation, p);

  xac->PostMessage((xac_handler_msg_t *)req, MSG_PRIORITY_DEFAULT);
  return XPAN_AC_SUCCESS;
}

uint8_t XpanLmpManager::HandleLmpAcceptReject(uint8_t *p, uint16_t len,
                                           ipaddr_t ip, uint8_t op) {
  uint8_t status = XPAN_AC_SUCCESS;
  uint16_t reason = 0;

  if (op == XPAN_LMP_NOT_ACCEPTED) {
    status = !XPAN_AC_SUCCESS;
    DATA_TO_UINT_16(reason, p);
  }

  ALOGD("%s: IP Address = %s", __func__, ip.toString().c_str());
  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return XPAN_CONN_DOESNT_EXIST;
  }

  mLmpOpLock.lock();
  /* Schedule the next command if pending in LMP queue */
  XpanLmpExecuteNextCmd *msg = (XpanLmpExecuteNextCmd *)malloc(sizeof(XpanLmpExecuteNextCmd));
  msg->event = XPAN_LMP_EXECUTE_NEXT_CMD;
  msg->ip = ip;
  xac->PostMessage((xac_handler_msg_t *)msg, MSG_PRIORITY_DEFAULT);

  XpanLmpManager *lmp = xac->GetLmpManager(ip);
  if (!lmp) {
    ALOGE("%s: Couldn't find associated lmp manager. Ignore this response.", __func__);
    mLmpOpLock.unlock();
    return XPAN_CONN_DOESNT_EXIST;
  }

  bdaddr_t addr{};
  XpanEarbudRole role = ROLE_INVALID;
  XpanDevice *dev = xac->GetDeviceByIpAddr(ip);
  if (dev) {
    addr = dev->GetAddr();
    role = dev->GetRoleByIpAddr(ip);
  }

  XpanLmpOperation pendingOp = (XpanLmpOperation)lmp->GetCurrentExeOp();
  ALOGD("%s: device(%s) %s response for operation = %s", __func__,
        addr.toString().c_str(), LmpOpString(op), LmpOpString(pendingOp));

  switch(pendingOp) {
    case XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ: {
         XpanL2capPauseUnpauseResEvt *res =
            (XpanL2capPauseUnpauseResEvt *)malloc(sizeof(XpanL2capPauseUnpauseResEvt));
         res->event = XPAN_LMP_L2CAP_PAUSE_UNPAUSE_RES_EVT;
         res->addr = addr;
         res->ip = ip;
         res->status = status;
         res->action = lmp->GetCurrentPauseUnpauseAction();
         xac->PostMessage((xac_handler_msg_t *)res, MSG_PRIORITY_DEFAULT);
         lmp->SetL2capPauseUnpauseAction(0xFF); // unsetting
        }
         break;

    case XPAN_LMP_BEARER_SWITCH_REQ: {
          XpanBearerSwitchResEvt *res =
              (XpanBearerSwitchResEvt *)malloc(sizeof(XpanBearerSwitchResEvt));
          res->event = XPAN_LMP_BEARER_SWITCH_RES_EVT;
          res->addr = addr;
          res->ip = ip;
          res->status = status;
          xac->PostMessage((xac_handler_msg_t *)res, MSG_PRIORITY_DEFAULT);
        }
        break;

    case XPAN_LMP_HOST_CONNECTION_REQ: {
          XpanLmpConnectionResEvt *res =
            (XpanLmpConnectionResEvt *)malloc(sizeof(XpanLmpConnectionResEvt));
          res->event = XPAN_LMP_CONNECTION_RES_EVT;
          res->addr = addr;
          res->ip = ip;
          res->role = role;
          res->status = status;
          xac->PostMessage((xac_handler_msg_t *)res, MSG_PRIORITY_DEFAULT);
        }
        break;

    case XPAN_LMP_PREPARE_BEARER_SWITCH_REQ: {
          XpanPrepareBearerResEvt *res =
              (XpanPrepareBearerResEvt *)malloc(sizeof(XpanPrepareBearerResEvt));
          res->event = XPAN_LMP_PREPARE_BEARER_RES_EVT;
          res->addr = addr;
          res->ip = ip;
          res->status = status;
          xac->PostMessage((xac_handler_msg_t *)res, MSG_PRIORITY_DEFAULT);
        }
        break;

    case XPAN_LMP_BURST_INTERVAL_REQ:
         break;

    default:
         ALOGE("%s: unexpected response. Operation: %d", __func__, pendingOp);
  }
  mLmpOpLock.unlock();
  return XPAN_AC_SUCCESS;
}

uint8_t XpanLmpManager::HandleReceivedL2capData(uint8_t *p, uint8_t llid, uint16_t len, ipaddr_t ip) {
  ALOGD("%s: llid = %d, len = %d ip=%s", __func__, llid, len, ip.toString().c_str());

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return XPAN_CONN_DOESNT_EXIST;
  }

  XpanDevice *dev = xac->GetDeviceByIpAddr(ip);
  if (!dev) {
    ALOGE("%s: No device for ip %s. Ignore this data", __func__, ip.toString().c_str());
    return XPAN_CONN_DOESNT_EXIST;
  }

  XpanAclDataRecvdEvt *evt = (XpanAclDataRecvdEvt *)malloc(sizeof(XpanAclDataRecvdEvt));
  evt->event = XPAN_LMP_ACL_DATA_EVT;
  evt->addr = dev->GetAddr();
  evt->ip = ip;
  evt->llid = llid;
  evt->len = len;
  evt->data = (uint8_t *)malloc(len);
  memcpy(evt->data, p, len);

  xac->PostMessage((xac_handler_msg_t *)evt, MSG_PRIORITY_DEFAULT);
  return XPAN_AC_SUCCESS;
}

void XpanLmpManager::setXpanSocketHandler(XpanSocketHandler *handler) {
  mSocketHdlr = handler;
}

void XpanLmpManager::setSupervisionTimeout(uint16_t supervision_timeout) {
  mSupervisionTimeout = supervision_timeout; // default 10 sec
  mPingTimeout = mSupervisionTimeout * 0.75;
  ALOGD("%s: Supervision timeout updated -> %d, Ping Timeout = %d",
        __func__, mSupervisionTimeout, mPingTimeout);
}

uint8_t XpanLmpManager::parseXpanEvent(uint8_t *p, uint16_t len, ipaddr_t ip, int fd) {
  uint8_t opcode = 0, llid = 0;
  uint16_t length = 0;

  // Reset Ping Timer
  ResetPingTimer(ip);

  int data_parsed = 0;

  while (data_parsed < len) {
    ALOGD("%s: Total data parsed = %d bytes, Remaining = %d bytes", __func__, data_parsed, (len - data_parsed));
    DATA_TO_UINT_8(llid, p);
    data_parsed++;

    DATA_TO_UINT_16(length, p);
    data_parsed += 2;

    ALOGD("%s: llid = %d, length = %d", __func__, llid, length);

    if (length == 0) {
      ALOGE("%s: Data with 0 length (llid: %d)", __func__, llid);
      return XPAN_LMP_INVALID_PARAMS;
    }

    if (llid == LE_L2CAP_CONT || llid == LE_L2CAP_START) {
      ALOGD("%s: LE L2CAP data received..", __func__);
      uint8_t ret = HandleReceivedL2capData(p, llid, length, ip);
      if (ret != XPAN_AC_SUCCESS) {
        return ret;
      }

      data_parsed += length;
      p += (uint8_t)length;
      continue;
    }

    DATA_TO_UINT_8(opcode, p);
    if (!IsValidLmpOpcode(opcode)) {
      ALOGE("%s: Invalid LMP operation %d", __func__, opcode);
      return XPAN_LMP_UNKNOWN_PDU;
    }

    ALOGD("%s: opcode = %s", __func__, xpan_lmpop_str((XpanLmpOperation)opcode));

    if ((data_parsed + length) > len) {
      ALOGE("%s: Invalid Command length for %s", __func__,
            xpan_lmpop_str((XpanLmpOperation)opcode));
      return XPAN_LMP_INVALID_PARAMS;
    }

    switch (opcode) {
      case XPAN_LMP_ACCEPTED:
      case XPAN_LMP_NOT_ACCEPTED:
           HandleLmpAcceptReject(p, length, ip, opcode);
           break;

      case XPAN_LMP_HOST_CONNECTION_REQ: {
           uint8_t ret = parseLmpConnectionReq(p, length, ip, fd);
           if (ret != XPAN_AC_SUCCESS) {
            return ret;
           }
          } break;

      case XPAN_LMP_LE_FEATURES_REQ: {
           uint8_t ret = parseLmpLeFeatureReq(p, length, ip);
           if (ret != XPAN_AC_SUCCESS) {
            return ret;
           }
          } break;

      case XPAN_LMP_LE_FEATURES_RSP: {
           uint8_t ret = parseLmpLeFeatureRes(p, length, ip);
           if (ret != XPAN_AC_SUCCESS) {
            return ret;
           }
          } break;

      case XPAN_LMP_VERSION_REQ: {
           uint8_t ret = parseLmpVersionReq(p, length, ip);
           if (ret != XPAN_AC_SUCCESS) {
            return ret;
           }
          } break;

      case XPAN_LMP_VERSION_RSP: {
           uint8_t ret = parseLmpVersionRes(p, length, ip);
           if (ret != XPAN_AC_SUCCESS) {
            return ret;
           }
          } break;

      case XPAN_LMP_PING_REQ: {
           uint8_t ret = parseLmpRemotePingReq(p, length, ip);
           if (ret != XPAN_AC_SUCCESS) {
            return ret;
           }
          } break;

      case XPAN_LMP_SWITCH_PRIMARY_REQ: {
           uint8_t ret = parseLmpSwitchPrimaryReq(p, length, ip);
           if (ret != XPAN_AC_SUCCESS) {
            return ret;
           }
          } break;

      case XPAN_LMP_PING_RSP:
           ALOGD("%s: Ping response. Timer reset done already", __func__);
           break;

      default:
        ALOGE("Unrecognized data. Ignore it.");
    }

    data_parsed += length;
    p += (uint8_t)(length - 1);
  }
  return XPAN_AC_SUCCESS;
}

void XpanLmpManager::StartSupervisionTimer(XpanLmpManager *lmp) {
  if (lmp->mPingTimer) {
    stopTimer(lmp->mPingTimer);
  }

  if (lmp->mSupervisionTimer) {
    stopTimer(lmp->mSupervisionTimer);
  }

  // start the Ping timer
  lmp->mPingTimer = new XpanAcTimer("PingTimer", HandleLmpPingTimeout, lmp);
  lmp->mPingTimer->StartTimer(lmp->mPingTimeout);
  ALOGD("%s: Timer started for Ping (%d ms)", __func__, lmp->mPingTimeout);

  /* Restart the link Supervision timer */
  lmp->mSupervisionTimer = new XpanAcTimer("SupervisionTimer", HandleSupervisionTimeout, lmp);
  lmp->mSupervisionTimer->StartTimer(lmp->mSupervisionTimeout);
  ALOGD("%s: Timer started for Supervision (%d ms)", __func__, lmp->mSupervisionTimeout);
}

void XpanLmpManager::ResetPingTimer(ipaddr_t ip) {
  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Unexpected Error. AC instance not found", __func__);
    return;
  }

  XpanLmpManager *lmp = xac->GetLmpManager(ip);
  if (!lmp) {
    ALOGE("%s: Lmp Manager not found for IP %s", __func__, ip.toString().c_str());
    return;
  }

  if (!lmp->mPingTimer) {
    ALOGE("%s: Ping Timer not started", __func__);
    return;
  }

  if (!lmp->mSupervisionTimer) {
    ALOGE("%s: Supervision Timer not started", __func__);
    return;
  }

  stopTimer(lmp->mPingTimer);
  lmp->mPingTimer = NULL;

  stopTimer(lmp->mSupervisionTimer);
  lmp->mSupervisionTimer = NULL;

  // Start the timers again
  StartSupervisionTimer(lmp);
}

void XpanLmpManager::HandleLmpPingTimeout(void *data) {
  XpanLmpManager *lmp = static_cast<XpanLmpManager *>(data);
  if (!lmp) {
    ALOGE("%s: Invalid Lmp Manager instance", __func__);
    return;
  }

  // send Lmp Ping request without queing
  lmp->SendLmpPingReq();
}

void XpanLmpManager::HandleSupervisionTimeout(void *data) {
  XpanLmpManager *lmp = static_cast<XpanLmpManager *>(data);
  if (!lmp) {
    ALOGE("%s: Invalid Lmp Manager instance", __func__);
    return;
  }

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return;
  }

  XpanDevice *dev = xac->GetDeviceByIpAddr(lmp->mIpAddr);
  if (!dev) {
    ALOGE("%s: No device found for ip: %s", __func__, lmp->mIpAddr.toString().c_str());
    return;
  }

  XmRemoteDisconnectedEvent *msg = (XmRemoteDisconnectedEvent *)malloc(sizeof(XmRemoteDisconnectedEvent));
  msg->event = XPAN_REMOTE_DISCONNECTED_EVT;
  msg->addr = dev->GetAddr();
  msg->role = dev->GetRoleByIpAddr(lmp->mIpAddr);

  xac->PostMessage((xac_handler_msg_t *)msg, MSG_PRIORITY_DEFAULT);
}

void XpanLmpManager::SendLmpConnectionReq(uint8_t link_type, mdns_uuid_t uuid,
                                          uint8_t *cod) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 21;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_HOST_CONNECTION_REQ));
  cmd_data.push_back(link_type);
  addUintArrToData(cmd_data, uuid.b, 16);
  addUintArrToData(cmd_data, cod, 3);

  LmpExecute(XPAN_LMP_HOST_CONNECTION_REQ, cmd_data);
}

void XpanLmpManager::SendLmpAccepted() {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 1;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_ACCEPTED));

  mSocketHdlr->SendData(XPAN_LMP_MSG, cmd_data);
}

void XpanLmpManager::SendLmpNotAccepted(uint16_t err_code) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 3;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_NOT_ACCEPTED));
  addUint16ToData(cmd_data, err_code);

  mSocketHdlr->SendData(XPAN_LMP_MSG, cmd_data);
}

void XpanLmpManager::SendLmpPrepareBearerReq(uint8_t bearer_from, uint8_t bearer_to) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 3;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_PREPARE_BEARER_SWITCH_REQ));
  cmd_data.push_back(bearer_from);
  cmd_data.push_back(bearer_to);

  LmpExecute(XPAN_LMP_PREPARE_BEARER_SWITCH_REQ, cmd_data);
}

void XpanLmpManager::SendLmpL2capPauseUnpauseReq(uint8_t pause, uint8_t transport_type) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 3;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ));
  cmd_data.push_back(pause);
  cmd_data.push_back(transport_type);

  l2cPauseUnpauseVal = pause;

  LmpExecute(XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ, cmd_data);
}

void XpanLmpManager::SendLmpBearerSwitchReq(uint8_t bearer_from, uint8_t bearer_to) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 3;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_BEARER_SWITCH_REQ));
  cmd_data.push_back(bearer_from);
  cmd_data.push_back(bearer_to);

  LmpExecute(XPAN_LMP_BEARER_SWITCH_REQ, cmd_data);
}

void XpanLmpManager::SendLmpBearerSwitchCmplInd(uint8_t bearer_from, uint8_t bearer_to) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 3;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_BEARER_SWITCH_COMPLETE_IND));
  cmd_data.push_back(bearer_from);
  cmd_data.push_back(bearer_to);

  mSocketHdlr->SendData(XPAN_LMP_MSG, cmd_data);
}

void XpanLmpManager::SendLmpCancelBearerSwitchInd(uint8_t bearer_from, uint8_t bearer_to,
                                                  uint16_t err_code) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 5;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_CANCEL_BEARER_SWITCH));
  cmd_data.push_back(bearer_from);
  cmd_data.push_back(bearer_to);
  addUint16ToData(cmd_data, err_code);

  mSocketHdlr->SendData(XPAN_LMP_MSG, cmd_data);
}

void XpanLmpManager::SendLmpPingReq() {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 1;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_PING_REQ));

  mSocketHdlr->SendData(XPAN_LMP_MSG, cmd_data);
}

void XpanLmpManager::SendLmpPingRes() {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 1;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_PING_RSP));

  mSocketHdlr->SendData(XPAN_LMP_MSG, cmd_data);
}

void XpanLmpManager::SendLmpLstoInd(uint16_t lsto) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 3;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_LSTO_IND));
  addUint16ToData(cmd_data, lsto);

  mSocketHdlr->SendData(XPAN_LMP_MSG, cmd_data);
}

void XpanLmpManager::SendLmpLeFeatureReq(mdns_uuid_t uuid, uint64_t le_features) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 25;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_LE_FEATURES_REQ));
  addUintArrToData(cmd_data, uuid.b, 16);
  addUint64ToData(cmd_data, le_features);

  LmpExecute(XPAN_LMP_LE_FEATURES_REQ, cmd_data);
}

void XpanLmpManager::SendLmpLeFeatureRes(mdns_uuid_t uuid, uint64_t le_features) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 25;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_LE_FEATURES_RSP));
  addUintArrToData(cmd_data, uuid.b, 16);
  addUint64ToData(cmd_data, le_features);

  mSocketHdlr->SendData(XPAN_LMP_MSG, cmd_data);
}

void XpanLmpManager::SendLmpVersionReq(mdns_uuid_t uuid, uint8_t version, uint16_t companyId,
                                       uint16_t subversion) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 22;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_VERSION_REQ));
  addUintArrToData(cmd_data, uuid.b, 16);
  cmd_data.push_back(version);
  addUint16ToData(cmd_data, companyId);
  addUint16ToData(cmd_data, subversion);

  LmpExecute(XPAN_LMP_VERSION_REQ, cmd_data);
}

void XpanLmpManager::SendLmpVersionRes(mdns_uuid_t uuid, uint8_t version, uint16_t companyId,
                                       uint16_t subversion) {
  std::vector<uint8_t> cmd_data;
  const uint16_t length = 22;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_VERSION_RSP));
  addUintArrToData(cmd_data, uuid.b, 16);
  cmd_data.push_back(version);
  addUint16ToData(cmd_data, companyId);
  addUint16ToData(cmd_data, subversion);

  mSocketHdlr->SendData(XPAN_LMP_MSG, cmd_data);
}

void XpanLmpManager::SendAclData(uint8_t llid, uint16_t len, uint8_t* data) {
  ALOGD("%s: llid = %d, len = %d", __func__, llid, len);
  std::vector<uint8_t> cmd_data;
  const uint16_t length = len;
  cmd_data.push_back((uint8_t)(0xFF & llid));
  addUint16ToData(cmd_data, length);
  cmd_data.insert(cmd_data.end(), &data[0], &data[len]);

  mSocketHdlr->SendData(llid, cmd_data);
}

const char* XpanLmpManager::LmpOpString(uint8_t op) {
  switch (op) {
    case XPAN_LMP_ACCEPTED:
      return "XPAN_LMP_ACCEPTED";
    case XPAN_LMP_NOT_ACCEPTED:
      return "XPAN_LMP_NOT_ACCEPTED";
    case XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ:
      return "XPAN_LMP_L2CAP_PAUSE_UNPAUSE_REQ";
    case XPAN_LMP_SWITCH_PRIMARY_REQ:
      return "XPAN_LMP_SWITCH_PRIMARY_REQ";
    case XPAN_LMP_BEARER_SWITCH_REQ:
      return "XPAN_LMP_BEARER_SWITCH_REQ";
    case XPAN_LMP_HOST_CONNECTION_REQ:
      return "XPAN_LMP_HOST_CONNECTION_REQ";
    case XPAN_LMP_BEARER_SWITCH_COMPLETE_IND:
      return "XPAN_LMP_BEARER_SWITCH_COMPLETE_IND";
    case XPAN_LMP_PREPARE_BEARER_SWITCH_REQ:
      return "XPAN_LMP_PREPARE_BEARER_SWITCH_REQ";
    case XPAN_LMP_PING_REQ:
      return "XPAN_LMP_PING_REQ";
    case XPAN_LMP_PING_RSP:
      return "XPAN_LMP_PING_RSP";
    case XPAN_LMP_BURST_INTERVAL_REQ:
      return "XPAN_LMP_BURST_INTERVAL_REQ";
    case XPAN_LMP_BURST_INTERVAL_IND:
      return "XPAN_LMP_BURST_INTERVAL_IND";
    case XPAN_LMP_LSTO_IND:
      return "XPAN_LMP_LSTO_IND";
    case XPAN_LMP_LE_FEATURES_REQ:
      return "XPAN_LMP_LE_FEATURES_REQ";
    case XPAN_LMP_LE_FEATURES_RSP:
      return "XPAN_LMP_LE_FEATURES_RSP";
    case XPAN_LMP_VERSION_REQ:
      return "XPAN_LMP_VERSION_REQ";
    case XPAN_LMP_VERSION_RSP:
      return "XPAN_LMP_VERSION_RSP";
    case XPAN_LMP_CANCEL_BEARER_SWITCH:
      return "XPAN_LMP_CANCEL_BEARER_SWITCH";
    default:
      return "Incorrect State";
  }
}

/* To translate uint16_t to byte stream */
void addUint16ToData(std::vector<uint8_t> &data, uint16_t val) {
  data.push_back((uint8_t)(0xFF & (val)));
  data.push_back((uint8_t)(0xFF & (val >> 8)));
}

/* To translate uint32_t to byte stream */
void addUint32ToData(std::vector<uint8_t> &data, uint32_t val) {
  data.push_back((uint8_t)(0xFF & (val)));
  data.push_back((uint8_t)(0xFF & (val >> 8)));
  data.push_back((uint8_t)(0xFF & (val >> 16)));
  data.push_back((uint8_t)(0xFF & (val >> 24)));
}

/* To translate uint64_t to byte stream */
void addUint64ToData(std::vector<uint8_t> &data, uint64_t val) {
  data.push_back((uint8_t)(0xFF & (val)));
  data.push_back((uint8_t)(0xFF & (val >> 8)));
  data.push_back((uint8_t)(0xFF & (val >> 16)));
  data.push_back((uint8_t)(0xFF & (val >> 24)));
  data.push_back((uint8_t)(0xFF & (val >> 32)));
  data.push_back((uint8_t)(0xFF & (val >> 40)));
  data.push_back((uint8_t)(0xFF & (val >> 48)));
  data.push_back((uint8_t)(0xFF & (val >> 56)));
}

void addUintArrToData(std::vector<uint8_t> &data, uint8_t* val, uint8_t len) {
  for (int i = 0; i < len; i++) {
    data.push_back((uint8_t)(0xFF & (val[i])));
  }
}

} // namespace ac
} // namespace xpan
