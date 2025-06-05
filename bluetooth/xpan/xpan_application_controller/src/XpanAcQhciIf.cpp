/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <log/log.h>
#include "XpanAcQhciIf.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.bluetooth.xpan_ac.XpanAcQhciIf"

namespace xpan {
namespace ac {

XpanAcQhciIf* XpanAcQhciIf::sInstance = 0;

XpanAcQhciIf::XpanAcQhciIf() {
  ALOGD("%s: XpanAcQhciIf instantiated.", __func__);
}

XpanAcQhciIf::~XpanAcQhciIf() {
  ALOGD("%s: XpanAcQhciIf deinitialized.", __func__);
}

XpanAcQhciIf* XpanAcQhciIf::GetIf() {
  if (sInstance == NULL) {
    sInstance = new XpanAcQhciIf();
  }
  return sInstance;
}

bool XpanAcQhciIf::Deinitialize() {
  if (sInstance) {
    delete sInstance;
    sInstance = NULL;
  }
  return true;
}

bool XpanAcQhciIf::CreateConnection (bdaddr_t addr, uint16_t supervision_timeout) {
  ALOGD("%s", __func__);

  QhciCreateConnection *params =
      (QhciCreateConnection *) malloc(sizeof(QhciCreateConnection));
  params->event = QHCI_CREATE_CONNECTION_EVT;
  params->addr = addr;
  params->supervision_timeout = supervision_timeout;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcQhciIf::CreateConnectionCancel (bdaddr_t addr) {
  ALOGD("%s", __func__);

  QhciCreateConnectionCancel *params =
      (QhciCreateConnectionCancel *) malloc(sizeof(QhciCreateConnectionCancel));
  params->event = QHCI_CREATE_CONNECTION_CANCEL_EVT;
  params->addr = addr;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcQhciIf::DisconnectConnection (bdaddr_t addr) {
  ALOGD("%s", __func__);

  QhciDisconnectConnection *params =
      (QhciDisconnectConnection *) malloc(sizeof(QhciDisconnectConnection));
  params->event = QHCI_DISCONNECT_CONNECTION_EVT;
  params->addr = addr;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcQhciIf::UpdateLocalVersion(uint8_t version, uint16_t companyId, uint16_t subversion) {
  ALOGD("%s", __func__);

  QhciSetLocalVersionEvt *params =
      (QhciSetLocalVersionEvt *)malloc(sizeof(QhciSetLocalVersionEvt));
  params->event = QHCI_SET_LOCAL_VERSION_EVT;
  params->version = version;
  params->companyId = companyId;
  params->subversion = subversion;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcQhciIf::UpdateLocalLeFeatures(uint64_t le_features) {
  ALOGD("%s", __func__);

  QhciSetLocalLeFeaturesEvt *params =
      (QhciSetLocalLeFeaturesEvt *)malloc(sizeof(QhciSetLocalLeFeaturesEvt));
  params->event = QHCI_SET_LOCAL_LE_FEATURES_EVT;
  params->le_features = le_features;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcQhciIf::GetRemoteVersion (bdaddr_t addr) {
  ALOGD("%s", __func__);

  QhciGetRemoteVersion *params =
      (QhciGetRemoteVersion *) malloc(sizeof(QhciGetRemoteVersion));
  params->event = QHCI_GET_REMOTE_VERSION_EVT;
  params->addr = addr;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcQhciIf::GetRemoteLeFeatures (bdaddr_t addr) {
  ALOGD("%s", __func__);

  QhciGetRemoteLeFeatures *params =
      (QhciGetRemoteLeFeatures *) malloc(sizeof(QhciGetRemoteLeFeatures));
  params->event = QHCI_GET_REMOTE_LE_FEATURES_EVT;
  params->addr = addr;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcQhciIf::EnableEncrption (bdaddr_t addr,
                                    const std::vector<uint8_t>& ltk) {
  ALOGD("%s", __func__);

  QhciEnableEncrption *params =
      (QhciEnableEncrption *) malloc(sizeof(QhciEnableEncrption));
  params->event = QHCI_ENABLE_ENCRYPTION_EVT;
  params->addr = addr;
  std::copy(ltk.begin(), ltk.end(), params->ltk);

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcQhciIf::SendAclData (bdaddr_t addr, uint8_t llid,
                                const std::vector<uint8_t>& data) {
  ALOGD("%s", __func__);

  QhciSendAclData *params =
      (QhciSendAclData *) malloc(sizeof(QhciSendAclData));
  params->event = QHCI_SEND_ACL_DATA_EVT;
  params->addr = addr;
  params->llid = llid;
  params->len = data.size();
  params->data= (uint8_t *)malloc(params->len);
  std::copy(data.begin(), data.end(), params->data);

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcQhciIf::L2capPauseUnpauseRes (bdaddr_t addr, uint8_t act, uint8_t status) {
  ALOGD("%s", __func__);

  QhciL2capPauseUnpauseRes *params =
      (QhciL2capPauseUnpauseRes *) malloc(sizeof(QhciL2capPauseUnpauseRes));
  params->event = QHCI_L2CAP_PAUSE_UNPAUSE_RES_EVT;
  params->addr = addr;
  params->action = act;
  params->status = status;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

} // namespace ac
} // namespace xpan
