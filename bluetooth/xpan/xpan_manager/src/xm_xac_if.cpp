/*
 *  Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <utils/Log.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include "xm_xac_if.h"
#include "XpanAcXmIf.h"
#include <stdlib.h>
#include <cstdio>
#include <iostream>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.xpan@1.0-xmxacif"

//XpanAcXmIf xac_xm_if;
using xpan::ac::XpanAcXmIf;
XMXacIf * XMXacIf :: instance_ = NULL;
namespace xpan {
namespace implementation {

XMXacIf::XMXacIf()
{
}

XMXacIf::~XMXacIf()
{

}

XMXacIf * XMXacIf :: GetIf()
{
  if (!instance_) {
    instance_ = new XMXacIf();
  }
  return instance_;
}

bool XMXacIf::Initialize(void)
{
  ALOGD("%s", __func__);
  return XpanAcXmIf::GetIf()->Initialize();
//  return true;
}

void XMXacIf::Deinitialize(void)
{
  ALOGD("%s", __func__);
  XpanAcXmIf::GetIf()->Deinitialize();
  if (instance_) {
    delete instance_;
    instance_ = NULL;
  }
}

bool XMXacIf::BondStateUpdate(bdaddr_t bdaddr, BondState state)
{
  ALOGD("%s: for device %s: state  : %s", __func__,
        ConvertRawBdaddress(bdaddr), BondStateToString(state));
  return XpanAcXmIf::GetIf()->UpdateBondState(bdaddr, state);
}

bool XMXacIf::UpdateXpanBondedDevices(uint8_t numOfDevices, bdaddr_t *devices)
{
  ALOGD("%s: numOfDevices %d", __func__, numOfDevices);
  return XpanAcXmIf::GetIf()->UpdateBondedXpanDevices(numOfDevices, devices);
}

bool XMXacIf::TriggerMdnsQuery(bdaddr_t addr, bool status)
{
  ALOGD("%s: with for %s status %s", __func__, ConvertRawBdaddress(addr),
        (status? "enable": "disable"));

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->MdnsReq.eventId = XAC_XP_MDNS_REQ;
  memcpy(&msg->MdnsReq.bdaddr, &addr, sizeof(bdaddr_t));
  msg->MdnsReq.status = status;
  return XpanManager::Get()->PostMessage(msg);
}

bool XMXacIf::OnCurrentTransportUpdated(bdaddr_t addr, TransportType type) {
  ALOGD("%s: for %s with type %s", __func__, ConvertRawBdaddress(addr),
        TransportTypeToString(type));

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->MdnsReq.eventId = XAC_XP_CURRENT_TRANS_UPDATE;
  memcpy(&msg->TransportUpdate.bdaddr, &addr, sizeof(bdaddr_t));
  msg->TransportUpdate.transport = type;
  return XpanManager::Get()->PostMessage(msg);
}

bool XMXacIf::RegisterMdnsService(bdaddr_t addr)
{
  ALOGD("%s: with for %s", __func__, ConvertRawBdaddress(addr));

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->MdnsReq.eventId = XAC_XP_REGISTER_MDNS_SERVICE;
  memcpy(&msg->MdnsReq.bdaddr, &addr, sizeof(bdaddr_t));
  return XpanManager::Get()->PostMessage(msg);
}

bool XMXacIf::StartFilteredScan(bdaddr_t addr)
{
  ALOGD("%s: with for %s", __func__, ConvertRawBdaddress(addr));

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->FilterScanInd.eventId = XAC_XP_START_FILTERED_SCAN;
  memcpy(&msg->FilterScanInd.bdaddr, &addr, sizeof(bdaddr_t));
  return XpanManager::Get()->PostMessage(msg);
}

bool XMXacIf::BurstIntervalReq(macaddr_t macaddr, uint16_t bi_media,
		               uint16_t bi_voice)
{
  ALOGD("%s: with for %s", __func__, ConvertRawMacaddress(macaddr));

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->BurstIntReq.eventId = XAC_CP_BURST_INT_REQ;
  memcpy(&msg->BurstIntReq.macaddr, &macaddr, sizeof(macaddr_t));
  msg->BurstIntReq.bi_media = bi_media;
  msg->BurstIntReq.bi_voice = bi_voice;
  return XpanManager::Get()->PostMessage(msg);
}

bool XMXacIf::UpdateHsApBssidChanged (macaddr_t connected_ap_bssid)
{
  return true;
}

bool XMXacIf::HandSetPortNumberRsp(int tcp_port, int udp_port, int udp_tsf_port)
{
  ALOGD("%s: udp port %d tcp port %d udp_tsf_port %d",
	__func__, udp_port, tcp_port, udp_tsf_port);

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  msg->HandSetPortNumRsp.eventId = XP_XAC_HANDSET_PORT_RSP;
  msg->HandSetPortNumRsp.tcp_port = tcp_port;
  msg->HandSetPortNumRsp.udp_port = udp_port;
  msg->HandSetPortNumRsp.udp_tsf_port = udp_tsf_port;
  return XpanManager::Get()->PostMessage(msg);
}

bool XMXacIf::PrepareAudioBearerRsp(bdaddr_t bdaddr, RspStatus status)
{
  ALOGD("%s: with for %s", __func__, ConvertRawBdaddress(bdaddr));

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->AudioBearerRsp.eventId = XAC_XM_PREPARE_AUDIO_BEARER_RSP;
  memcpy(&msg->AudioBearerRsp.bdaddr, &bdaddr, sizeof(bdaddr_t));
  msg->AudioBearerRsp.status = status;
  return XpanManager::Get()->PostMessage(msg);
}

bool XMXacIf::BearerPreferenceRsp (bdaddr_t bdaddr, TransportType type,
		                   RspStatus status)
{
  ALOGD("%s: with for %s", __func__, ConvertRawBdaddress(bdaddr));
  return true;
}

bool XMXacIf:: UpdateRemoteEbDetails(bool encryption, uint8_t *psk, uint8_t *identity,
                            macaddr_t hs_ap_bssid, macaddr_t hs_mac_addr,
                            ipaddr_t hs_ip_addr,
                            uint32_t center_freq,
                            uint16_t time_sync_tx_port,
                            uint16_t time_sync_rx_port,
                            uint16_t remote_udp_port,
                            uint16_t rx_udp_port,
			    std::vector<RemoteEbParams> EbParams)
{
  int num_devices;
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  msg->CpRemoteParamsInd.eventId = XAC_CP_REMOTE_PARAMS_IND;

  msg->CpRemoteParamsInd.encryption = (uint8_t) encryption;
  if (psk) {
    for (int i =0; i< 16; i++)
	    msg->CpRemoteParamsInd.psk[i] = psk[i];
  } else {
    for (int i =0; i< 16; i++)
	    msg->CpRemoteParamsInd.psk[i] = 0;
  }

  memset(msg->CpRemoteParamsInd.identity, 0,
         sizeof(msg->CpRemoteParamsInd.identity));
  if (identity) {
    for (int i = 0; i < 16; i++)
      msg->CpRemoteParamsInd.identity[i] = identity[i];
  }

  memcpy(&msg->CpRemoteParamsInd.hs_ap_bssid, &hs_ap_bssid, sizeof(macaddr_t));
  memcpy(&msg->CpRemoteParamsInd.hs_mac_addr, &hs_mac_addr, sizeof(macaddr_t));
  memcpy(&msg->CpRemoteParamsInd.hs_ip_addr, &hs_ip_addr, sizeof(ipaddr_t));

  msg->CpRemoteParamsInd.center_freq = center_freq;
  msg->CpRemoteParamsInd.time_sync_tx_port = time_sync_tx_port;
  msg->CpRemoteParamsInd.time_sync_rx_port = time_sync_rx_port;
  msg->CpRemoteParamsInd.remote_udp_port = remote_udp_port;
  msg->CpRemoteParamsInd.rx_udp_port = rx_udp_port;

  num_devices = EbParams.size();
  msg->CpRemoteParamsInd.num_devices = num_devices;

  //ALOGD("%s:  encryption: %d psk:%s", __func__, (uint8_t)encryption, psk);
  ALOGD("%s: hs_ap_bssid %s", __func__, ConvertRawMacaddress(hs_ap_bssid));
  ALOGD("%s: hs_mac_addr %s", __func__, ConvertRawMacaddress(hs_mac_addr));
  ALOGD("%s: hs_ip_addr %s", __func__, hs_ip_addr.toString().c_str());
  ALOGD("%s: center_freq %d", __func__, center_freq);
  ALOGD("%s: time_sync_tx_port %d time_sync_rx_port %d remote_udp_port %d rx_udp_port %d",
        __func__, time_sync_tx_port, time_sync_rx_port, remote_udp_port , rx_udp_port);

  ALOGD("%s: numOfDevices %d", __func__, num_devices);

  if (num_devices) {
    RemoteEbParams *Eb;
    int i = 0;
    Eb = (RemoteEbParams *)(malloc(sizeof(RemoteEbParams) * num_devices));
    for (RemoteEbParams eb_params: EbParams) {
      memcpy(&Eb[i].eb_ap_bssid, &eb_params.eb_ap_bssid, sizeof(macaddr_t));
      memcpy(&Eb[i].eb_mac_addr, &eb_params.eb_mac_addr, sizeof(macaddr_t));
      memcpy(&Eb[i].eb_ip_addr, &eb_params.eb_ip_addr, sizeof(ipaddr_t));

      Eb[i].eb_audio_loc = eb_params.eb_audio_loc;
      Eb[i].role = eb_params.role;
      ALOGD("%s: Eb.eb_ap_bssid %s", __func__, ConvertRawMacaddress(Eb[i].eb_ap_bssid));
      ALOGD("%s: Eb.eb_mac_addr %s", __func__, ConvertRawMacaddress(Eb[i].eb_mac_addr));
      ALOGD("%s: Eb.eb_ip_addr %s", __func__, Eb[i].eb_ip_addr.toString().c_str());
      ALOGD("%s: audioLocation %d role %d", __func__, Eb[i].eb_audio_loc, Eb[i].role);
      i++;
    }
    msg->CpRemoteParamsInd.EbParams= Eb;
  }
  return XpanManager::Get()->PostMessage(msg);
}

bool XMXacIf::BearerSwitchInd(bdaddr_t bdaddr, TransportType type, RspStatus status)
{
  ALOGD("%s: with for %s with status %d", __func__, ConvertRawBdaddress(bdaddr),
        TransportTypeToString(type), status);
  return XpanAcXmIf::GetIf()->BearerSwitchInd(bdaddr, type, status);
}

bool XMXacIf::InitiateLmpBearerSwitch(bdaddr_t bdaddr, TransportType type)
{
  ALOGD("%s: with for %s", __func__, ConvertRawBdaddress(bdaddr),
	TransportTypeToString(type));
  return XpanAcXmIf::GetIf()->InitiateLmpBearerSwitch(bdaddr, XPAN_AP);
}

bool XMXacIf::MdnsDiscoveryStatus(bdaddr_t bdaddr, uint8_t status, mdns_uuid_t uuid, uint8_t state)
{
  ALOGD("%s: with for %s status %s(%d) uuid %s state %d", __func__, ConvertRawBdaddress(bdaddr),
        (status? "error": "success"), status, uuid.toString().c_str(), state);
  return XpanAcXmIf::GetIf()->MdnsDiscoveryStatus(bdaddr, status, uuid, state);
}

bool XMXacIf::PrepareAudioBearerReq(bdaddr_t bdaddr, TransportType type)
{
  ALOGI("%s for bdaddr_t:%s with transport: %s", __func__,
        (ConvertRawBdaddress(bdaddr)), (TransportTypeToString(type)));

  return XpanAcXmIf::GetIf()->PrepareBearer(bdaddr, type);
}

bool XMXacIf::BearerPreferenceReq (bdaddr_t bdaddr, TransportType type)
{
  ALOGI("%s for bdaddr_t:%s with transport: %s", __func__,
        (ConvertRawBdaddress(bdaddr)), (TransportTypeToString(type)));

  return XpanAcXmIf::GetIf()->BearerPreference(bdaddr, type);
}

bool XMXacIf::UpdateRemoteApDetails(tXPAN_Remote_Params params)
{
  return XpanAcXmIf::GetIf()->UpdateRemoteApParams(params);
}

bool XMXacIf::GetHandSetPortNumberReq(void)
{
  ALOGI("%s", __func__);
  return XpanAcXmIf::GetIf()->GetLocalListeningPorts();
}

bool XMXacIf::UpdateLocalApDetails(macaddr_t mac, macaddr_t bssid,
		                     ipaddr_t ipAddr, mdns_uuid_t uuid, uint32_t freq)
{
  ALOGI("%s", __func__);
  return XpanAcXmIf::GetIf()->UpdateLocalApDetails(mac, bssid, ipAddr, uuid, freq);
}

void XMXacIf::ProcessMessage(XmIpcEventId eventId, xm_ipc_msg_t *msg)
{
  switch (eventId) {
    case XP_XM_BONDED_DEVICES_LIST: {
      UpdateXpanBondedDevices(msg->BondedDevies.numOfDevices,
		              msg->BondedDevies.bdaddr);
      break;
    } case XP_XAC_MDNS_RSP: {
      MdnsDiscoveryStatus(msg->MdnsRsp.bdaddr, msg->MdnsRsp.status, msg->MdnsRsp.uuid, msg->MdnsRsp.state);
      break;
    } case XP_XAC_REMOTE_AP_DETAILS: {
      UpdateRemoteApDetails(msg->EbParams.params);
      break;
    } case XP_XAC_HANDSET_PORT_REQ: {
      GetHandSetPortNumberReq();
      break;
    } case XP_XAC_BONDED_STATE_UPDATE : {
      BondStateUpdate(msg->BondStateInd.bdaddr, msg->BondStateInd.state);
      break;
    } case XP_XAC_LOCAL_AP_DETAILS: {
      ALOGD("%s msg->ApParams.ap_mac_address = %s ", __func__, msg->ApParams.ap_mac_address.toString().c_str());
      UpdateLocalApDetails(msg->ApParams.ap_mac_address, msg->ApParams.bssid,
          msg->ApParams.ipaddr, msg->ApParams.mdns_uuid, msg->ApParams.center_freq);
      break;
    } default: {
      ALOGI("%s: this :%04x ipc message is not handled", __func__, eventId);
    }
  }
}

} // namespace implementation
} // namespace xpan
