/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <log/log.h>
#include "XpanAcXmIf.h"
#include "XpanAcQhciIf.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.bluetooth.xpan_ac.XpanAcXmIf"

namespace xpan {
namespace ac {

XpanAcXmIf* XpanAcXmIf::sInstance = NULL;

XpanAcXmIf::XpanAcXmIf() {
  ALOGD("%s: Instantiated.", __func__);

}

XpanAcXmIf::~XpanAcXmIf() {
  ALOGD("%s: Deintialized.", __func__);

}

XpanAcXmIf* XpanAcXmIf::GetIf() {
  if (!sInstance) {
    sInstance = new XpanAcXmIf();
  }
  return sInstance;
}

bool XpanAcXmIf::Initialize() {
  return XpanApplicationController::Initialize();
}

bool XpanAcXmIf::Deinitialize() {
  if (sInstance) {
    delete sInstance;
    sInstance = NULL;
    ALOGD("%s: XpanAcXmIf cleared", __func__);
  }

  XpanAcQhciIf::Deinitialize();

  return XpanApplicationController::Deinitialize();
}

bool XpanAcXmIf::PrepareBearer (bdaddr_t addr, TransportType bearer) {
  ALOGD("%s", __func__);

  XmPrepareBearer *params = (XmPrepareBearer *) malloc (sizeof(XmPrepareBearer));
  params->event = XM_PREPARE_BEARER_EVT;
  params->addr = addr;
  params->bearer = bearer;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcXmIf::BearerPreference (bdaddr_t addr, TransportType bearer) {
  ALOGD("%s", __func__);

  XmBearerPreference *params =
      (XmBearerPreference *) malloc (sizeof(XmBearerPreference));
  params->event = XM_BEARER_PREFERENCE_EVT;
  params->addr = addr;
  params->bearer = bearer;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcXmIf::UpdateRemoteApParams (tXPAN_Remote_Params ap_details) {
  ALOGD("%s", __func__);

  XmUpdateRemoteApParams *params =
      (XmUpdateRemoteApParams *) malloc (sizeof(XmUpdateRemoteApParams));
  params->event = XM_UPDATE_REMOTE_AP_PARAMS_EVT;
  params->addr = ap_details.addr;
  params->is_mdns_update = ap_details.mdns;
  params->mdns_uuid = ap_details.mdnsUuid;
  params->remote_tcp_port = ap_details.portL2capTcp;
  params->remote_udp_port = ap_details.portUdpAudio;
  params->remote_udp_tsf_port = ap_details.portUdpReports;
  params->num_of_earbuds = ap_details.numOfEbs;

  for (int i = 0; i < params->num_of_earbuds; i++) {
    params->role[i] = (XpanEarbudRole)ap_details.vectorEbParams[i].role;
    params->ip[i] = ap_details.vectorEbParams[i].ipAddr;
    params->mac[i] = ap_details.vectorEbParams[i].mac_addr;
    params->audio_loc[i] = ap_details.vectorEbParams[i].audioLocation;
    params->eb_bssid = ap_details.vectorEbParams[i].mac_bssid;
  }

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcXmIf::InitiateLmpBearerSwitch (bdaddr_t addr, TransportType bearer) {
  ALOGD("%s", __func__);

  XmInitiateLmpBearerSwitch *params =
      (XmInitiateLmpBearerSwitch *) malloc (sizeof(XmInitiateLmpBearerSwitch));
  params->event = XM_INITIATE_LMP_BEARER_SWITCH_EVT;
  params->addr = addr;
  params->bearer = bearer;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcXmIf::RemoteDisconnectedEvent (bdaddr_t addr, XpanEarbudRole role) {
  ALOGD("%s", __func__);

  XmRemoteDisconnectedEvent *params =
      (XmRemoteDisconnectedEvent *) malloc (sizeof(XmRemoteDisconnectedEvent));
  params->event = XM_BEARER_SWITCH_IND_EVT;
  params->addr = addr;
  params->role = role;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcXmIf::BearerSwitchInd (bdaddr_t addr, TransportType bearer, uint8_t status) {
  ALOGD("%s", __func__);

  XmBearerSwitchInd *params =
      (XmBearerSwitchInd *) malloc (sizeof(XmBearerSwitchInd));
  params->event = XM_BEARER_SWITCH_IND_EVT;
  params->addr = addr;
  params->bearer = bearer;
  params->status = status;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcXmIf::MdnsDiscoveryStatus (bdaddr_t addr, uint8_t status,
    mdns_uuid_t uuid, uint8_t state) {
  ALOGD("%s", __func__);

  XmMdnsDiscoveryStatus *params =
      (XmMdnsDiscoveryStatus *) malloc (sizeof(XmMdnsDiscoveryStatus));
  params->event = XM_MDNS_DISCOVERY_STATUS_EVT;
  params->addr = addr;
  params->status = status;
  params->uuid = uuid;
  params->state = (MDNS_OP)state;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcXmIf::UpdateBondState (bdaddr_t addr, BondState state) {
  ALOGD("%s", __func__);

  XmUpdateBondState *params =
      (XmUpdateBondState *) malloc (sizeof(XmUpdateBondState));
  params->event = XM_UPDATE_BOND_STATE_EVT;
  params->addr = addr;
  params->state = state;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcXmIf::UpdateLocalApDetails (macaddr_t mac, macaddr_t bssid,
                                       ipaddr_t ipAddr, mdns_uuid_t uuid, uint32_t freq) {
  ALOGD("%s", __func__);

  XmUpdateLocalApDetails *params =
      (XmUpdateLocalApDetails *) malloc (sizeof(XmUpdateLocalApDetails));
  params->event = XM_UPDATE_LOCAL_AP_DETAILS_EVT;
  params->local_mac_addr = mac;
  params->bssid = bssid;
  params->ipAddr = ipAddr;
  params->uuid = uuid;
  params->freq = freq;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcXmIf::GetLocalListeningPorts () {
  ALOGD("%s", __func__);

  XmGetLocalListeningPorts *params =
      (XmGetLocalListeningPorts *) malloc (sizeof(XmGetLocalListeningPorts));
  params->event = XM_GET_LOCAL_LISTENING_PORTS_EVT;

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  xac->PostMessage((xac_handler_msg_t *)params, MSG_PRIORITY_DEFAULT);
  return true;
}

bool XpanAcXmIf::UpdateBondedXpanDevices (uint8_t numOfDevices, bdaddr_t devices[]) {
  ALOGD("%s", __func__);

  XmUpdateBondedXpanDevices *params =
      (XmUpdateBondedXpanDevices *) malloc (sizeof(XmUpdateBondedXpanDevices));
  params->event = XM_UPDATE_BONDED_XPAN_DEVICES_EVT;
  params->numOfDevices = numOfDevices;
  params->devices = (bdaddr_t *)malloc(sizeof(bdaddr_t) * numOfDevices);
  for (int i = 0; i < numOfDevices; i++) {
    memcpy(&params->devices[i], &devices[i], sizeof(bdaddr_t));
  }

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