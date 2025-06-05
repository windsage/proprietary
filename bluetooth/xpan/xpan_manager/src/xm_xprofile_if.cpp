/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <utils/Log.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include "xm_xprofile_if.h"

#include <android-base/logging.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cutils/properties.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.xpan@1.0-xmxprofileif"

using bluetooth::xpanprovider::XpanProviderIf;
tXPAN_MANAGER_API XpanProviderCb;
namespace xpan {
namespace implementation {
std::mutex profile_sync_mtk;
uint8_t twtconfig_prop_read = 0;
XMXprofileIf::XMXprofileIf()
{
}

XMXprofileIf::~XMXprofileIf()
{
}

/******************************************************************
 *
 * Function         RemoteSupportsXpan
 *
 * Description      This API is notify profile when a new remote
 *                  is connected.
 * Parameters:      bdaddr_t - Bluetooth address of the remote.
 *                  enable   - add a place holder always set to true.
 *
 * Return:          bool - return the status wether the message is
 *                  queued to processed by AIDL.
 ******************************************************************/
bool XMXprofileIf::RemoteSupportsXpan(bdaddr_t addr, bool enable)
{
  ALOGD("%s: bd addr :%s and status:%d", __func__,
        ConvertRawBdaddress(addr), enable);
  /* status wether the device supports xpan or not are for reserved
   * purpose. This parameters might be used in future implementation.
   */
 if(XpanProviderIf::GetIf())
   XpanProviderIf::GetIf()->XpanDeviceFoundCb(addr);
 else
   ALOGE("Discarding %s AIDL service is not up and running", __func__);
  return true;
}

/******************************************************************
 *
 * Function         TransportEnabled
 *
 * Description      This API is invoked by profile to notify XM the status
 *                  of SAP connetion
 *
 * Parameters:      bdaddr_t - Bluetooth address of the remote.
 *                  status   - holds the status of SAP connection.
 *
 * Return:          bool - return the status wether the message is
 *                  queued to process by QHCI.
 ******************************************************************/
void XMXprofileIf::TransportEnabled(bdaddr_t addr, TransportType type,
		                    bool status, uint8_t reason)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  ALOGI("%s for bd address %s of transport %s with status: %d and reason:%d",
	__func__, TransportTypeToString(type), ConvertRawBdaddress(addr),
	status, reason);
  msg->TransportEnabled.eventId = XP_XM_TRANSPORT_ENABLED;
  memcpy(&msg->TransportEnabled.bdaddr, &addr, sizeof(bdaddr_t));
  msg->TransportEnabled.type = type;
  msg->TransportEnabled.status = status;
  msg->TransportEnabled.reason = reason;
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::BondStateUpdate(bdaddr_t addr, uint8_t state)
{
  ALOGD("%s: for device %s: state  : %s", __func__,
        ConvertRawBdaddress(addr), BondStateToString((BondState)state));
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  msg->BondStateInd.eventId = XP_XAC_BONDED_STATE_UPDATE;
  msg->BondStateInd.state = (BondState)state;
  memcpy(&msg->BondStateInd.bdaddr, &addr, sizeof(bdaddr_t));
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::UpdateWifiScanStarted(bdaddr_t addr, uint8_t state)
{
  ALOGD("%s  %s  %d", __func__, ConvertRawBdaddress(addr), state);
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  if( msg == NULL)
  {
    ALOGE("%s: nullptr", __func__);
    return;
  }
  msg->XpWifiScanStarted.eventId = XP_WIFI_SCAN_STARTED;
  msg->XpWifiScanStarted.state = (BondState)state;
  memcpy(&msg->XpWifiScanStarted.bdaddr, &addr, sizeof(bdaddr_t));
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::UpdateLocalApDetails(tXPAN_Local_Params params)
{
/*  ALOGD("%s: BSSID %s MAC %s iptype %s IP addr %s MDSN UUID %s", __func__,
	ConvertRawMacaddress(params.bssid), ConvertRawMacaddress(params.randomMac),
	);
*/
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  msg->ApParams.eventId = XP_XAC_LOCAL_AP_DETAILS;
  memcpy(&msg->ApParams.bssid, &params.bssid, sizeof(macaddr_t));
  memcpy(&msg->ApParams.ap_mac_address, &params.randomMac, sizeof(macaddr_t));
  memcpy(&msg->ApParams.ipaddr, &params.ipAddr, sizeof(ipaddr_t));
  memcpy(&msg->ApParams.mdns_uuid, &params.mdnsUuid, sizeof(mdns_uuid_t));
  msg->ApParams.center_freq = params.freq;
  ALOGD("%s: mac %s", __func__, msg->ApParams.ap_mac_address.toString().c_str());
  ALOGD("%s: mac %s", __func__, msg->ApParams.bssid.toString().c_str());

  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::UpdateXpanBondedDevices(uint8_t numOfDevices, bdaddr_t *devices)
{
  ALOGI("%s with numOfDevices %d", __func__, numOfDevices);
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  bdaddr_t *bdaddr;

  msg->BondedDevies.eventId = XP_XM_BONDED_DEVICES_LIST;
  msg->BondedDevies.numOfDevices = numOfDevices;
  bdaddr = (bdaddr_t *)malloc(numOfDevices * sizeof(bdaddr_t));
  for (int i =0; i < numOfDevices; i++) {
    memcpy(&bdaddr[i], &devices[i], sizeof(bdaddr_t));
    ALOGI("%s: BondedDevies[%d]: %s", __func__, i, ConvertRawBdaddress(devices[i]));  
  }
  msg->BondedDevies.bdaddr = bdaddr;
  XpanManager::Get()->PostMessage(msg);
}

bool XMXprofileIf::WifiTransportPreferenceReq(uint8_t transport_type)
{
  ALOGI("%s: Transport type %s", __func__, TransportTypeToString((TransportType)transport_type));
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->WifiTransportPreferenceCb((uint8_t) transport_type);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);
  return true;
}

void XMXprofileIf::WifiTransportPreferenceRsp(uint8_t transport_type, uint8_t status)
{
  ALOGI("%s: transport_type %s with status %d", __func__,
        TransportTypeToString((TransportType)transport_type), status);

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->eventId = XP_WIFI_TRANSPORT_SWITCH_RSP;
  msg->WiFiTransportSwitch.transport_type = transport_type;
  msg->WiFiTransportSwitch.status = status;
  XpanManager::Get()->PostMessage(msg);
}
 
void XMXprofileIf::UpdateConnectedEbDetails(uint8_t set_id, macaddr_t left_eb, macaddr_t right_eb)
{
   ALOGI("%s: set_id %d left macaddr (%02X:%02X:%02X:%02X:%02X:%02X), \
          right macaddr (%02X:%02X:%02X:%02X:%02X:%02X)", __func__, set_id, 
          left_eb.b[0], left_eb.b[1], left_eb.b[2], left_eb.b[3], left_eb.b[4], left_eb.b[5], 
          right_eb.b[0], right_eb.b[1], right_eb.b[2],right_eb.b[3], right_eb.b[4], right_eb.b[5]);

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->eventId = XP_WIFI_CONNECTED_EB_DETAILS;
  msg->WiFiConnectedEbs.set_id = set_id;

  uint8_t right_rev_macaddr[6];
  uint8_t left_rev_macaddr[6];
  int mac_addr_len = sizeof(macaddr_t);
  for (int j = 0; j < mac_addr_len; j++) {
      left_rev_macaddr[j] = left_eb.b[mac_addr_len-j-1];
      right_rev_macaddr[j] = right_eb.b[mac_addr_len-j-1];
  }  
  for (int j =0; j < sizeof(macaddr_t); j++) {
        msg->WiFiConnectedEbs.left_eb.b[j] = left_rev_macaddr[j];
        msg->WiFiConnectedEbs.right_eb.b[j] = right_rev_macaddr[j];
  }
  XpanManager::Get()->PostMessage(msg);

   ALOGI("%s: after set_id %d left macaddr %s, \
          right macaddr %s ", __func__, set_id,
          msg->WiFiConnectedEbs.left_eb.toString().c_str(),
          msg->WiFiConnectedEbs.right_eb.toString().c_str());
}

void XMXprofileIf::SetApAvailableReq(bdaddr_t addr, uint32_t duration)
{
  ALOGI("%s: bdaddr %s and duration %d", __func__,
        ConvertRawBdaddress(addr), duration);

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->eventId = XP_WIFI_SET_AP_AVB_REQ;
  memcpy(&msg->SetApAvbReq.addr, &addr, sizeof(bdaddr_t));
  msg->SetApAvbReq.duration = duration;
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::CancelApAvailableReq(bdaddr_t addr)
{
  ALOGI("%s: bdaddr %s", __func__, ConvertRawBdaddress(addr));

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->eventId = XP_WIFI_CANCEL_AP_AVB;
  memcpy(&msg->SetApAvbReq.addr, &addr, sizeof(bdaddr_t));
  XpanManager::Get()->PostMessage(msg);
}

bool XMXprofileIf::XmXpPrepareAudioBearerReq(bdaddr_t addr, TransportType type)
{
  ALOGI("%s for bdaddr_t:%s with transport: %s", __func__,
        (ConvertRawBdaddress(addr)), (TransportTypeToString(type)));

  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->PrepareBearerCb(addr, (uint8_t) type);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;
}

void XMXprofileIf::XpXmPrepareAudioBearerRsp(bdaddr_t addr, uint8_t bearer,
                                             uint8_t status)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  
  ALOGD("%s", __func__);
  msg->AudioBearerRsp.eventId = XP_XM_PREPARE_AUDIO_BEARER_RSP;
  memcpy(&msg->AudioBearerRsp.bdaddr, &addr, sizeof(bdaddr_t));
  msg->AudioBearerRsp.status = (RspStatus)status;
  XpanManager::Get()->PostMessage(msg);
}

bool XMXprofileIf::XmXpBearerSwitchInd(bdaddr_t addr, TransportType type, uint8_t status)
{
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->BearerSwitchIndicationCb(addr, type, status);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;;
}

void XMXprofileIf::XpXmBearerSwitchInd(bdaddr_t addr, uint8_t bearer,
		                       uint8_t status)
{
 
  ALOGD("%s", __func__);
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  msg->XpBearerSwitchInd.eventId = XP_XM_AUDIO_BEARER_SWITCHED;
  memcpy(&msg->XpBearerSwitchInd.bdaddr, &addr, sizeof(bdaddr_t));
  msg->XpBearerSwitchInd.status = (RspStatus)status;
  XpanManager::Get()->PostMessage(msg);
}

/******************************************************************
 *
 * Function         UseCaseUpdate
 *
 * Description      This API is invoked by XM to notify on usecase.
 *
 * Parameters:      usecase - Usecase type
 *
 * Return:          bool - return the status whether the message is
 *                  queued to process by profile.
 ******************************************************************/

bool XMXprofileIf::UseCaseUpdate(UseCaseType usecase)
{
  ALOGI("%s usecase: %s", __func__, UseCaseToString(usecase));
  /* BD address is reserved for future use, for now sending
   * ACTIVE_BDADDR. Upper layers will apply usecase for either
   * active device or for next connected device.
   */
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->UsecaseUpdateCb(ACTIVE_BDADDR, usecase);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;;
}

void XMXprofileIf::EnableAcs(std::vector<uint32_t>freq_list)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  int i = 0,freq;
  int freq_list_size = freq_list.size();

  ALOGD("%s with freq_list_size: %d", __func__, freq_list_size);
  msg->Acslist.eventId = XP_XM_ENABLE_ACS;

  /* Create a separate chunk instead of relying on static memory locations */
  msg->Acslist.freq_list = (int *) malloc(sizeof(int) * freq_list_size);
  for (int freq : freq_list)
    msg->Acslist.freq_list[i++] = freq;

  msg->Acslist.freq_list_size = freq_list_size;

  XpanManager::Get()->PostMessage(msg);
}

bool XMXprofileIf::WiFiAcsResults(xm_ipc_msg_t *msg)
{
  ALOGD("%s: Rx Acs results freq: %d and status%d", __func__,
        msg->AcsResults.freq, msg->AcsResults.status);
  if(XpanProviderIf::GetIf())
     XpanProviderIf::GetIf()->AcsUpdateCb(msg->AcsResults.status,
		                       msg->AcsResults.freq);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);
  return true;
}

bool XMXprofileIf::WiFiSsrEventInd(xm_ipc_msg_t *msg)
{
  ALOGD("%s: SSR status: %d", __func__, msg->SsrEvent.status);
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->SsrWifiCb(msg->SsrEvent.status);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);
  return true;
}

bool XMXprofileIf::WifiTwtEvent(xm_ipc_msg_t *msg)
{

  uint8_t twt_event = msg->TwtEvent.twt_event;
  int32_t wake_dur_us = msg->TwtEvent.wake_dur_us;
  int32_t wake_int_us = msg->TwtEvent.wake_int_us;

  ALOGD("%s", __func__);
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->TwtSessionEstablishedCb(msg->TwtEvent.addr,
		                       wake_dur_us, wake_int_us, twt_event);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);
  return true;
}

bool XMXprofileIf::UpdateSapInterface(xm_ipc_msg_t *msg)
{
  uint8_t req_state = msg->CreateSapInfStatusParams.req_state;
  uint8_t status = msg->CreateSapInfStatusParams.status;
  ALOGD("%s: for state req :%s and status is %d", __func__,
	req_state ? "enable": "disable", status);
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->SapInterfaceCreatedCb(req_state, status);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);
  return true;
}

bool XMXprofileIf::WifiSapPowerSaveEvent(xm_ipc_msg_t *msg)
{
  uint8_t dialog_id = msg->SapPowerStateRsp.dialog_id;
  uint16_t power_save_bi_multiplier = msg->SapPowerStateRsp.power_save_bi_multiplier;
  uint64_t next_tsf = msg->SapPowerStateRsp.next_tsf;
  ALOGD("%s", __func__);

  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->SapLowPowerModeUpdateCb(dialog_id,
		                       power_save_bi_multiplier, next_tsf);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;
}

bool XMXprofileIf::BearerPreferenceRsp(bdaddr_t bdaddr, TransportType type,
		                       RspStatus status, uint8_t requestor)
{
  ALOGD("%s", __func__);
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->BearerPreferenceResCb(bdaddr, type, status, requestor);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;
}

bool XMXprofileIf::TriggerMdnsQuery(bdaddr_t bdaddr, bool status)
{
  ALOGD("%s: with for %s status %s", __func__, ConvertRawBdaddress(bdaddr),
        (status? "enable": "disable"));
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->MdnsSearchCb(bdaddr, (uint8_t)status);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;
}

bool XMXprofileIf::StartFilteredScan(bdaddr_t bdaddr)
{
  ALOGD("%s: with for %s", __func__, ConvertRawBdaddress(bdaddr));
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->FilteredScanCb(bdaddr, true);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;
}

bool XMXprofileIf::ConnectLeLinkRsp(bdaddr_t bdaddr, uint8_t status)
{
  ALOGD("%s for bd address %s with status %s", __func__,
        ConvertRawBdaddress(bdaddr), !status ? "connected": "not connected");
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->LeLinkEstablishedCb(bdaddr, status);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;
}

bool XMXprofileIf::OnCurrentTransportUpdated(bdaddr_t addr, TransportType type) {
  ALOGD("%s: for %s with type %s", __func__, ConvertRawBdaddress(addr),
        TransportTypeToString(type));

  if(XpanProviderIf::GetIf())
   XpanProviderIf::GetIf()->CurrentTransportCb(addr, type);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;
}

bool XMXprofileIf::RegisterMdnsService(bdaddr_t addr)
{
  ALOGD("%s: with for %s", __func__, ConvertRawBdaddress(addr));

  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->MdnsRegisterUnRegister(addr, true);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;
}

bool XMXprofileIf::HandSetPortNumberRsp(int tcp_port, int udp_port, int udp_tsf_port)
{
  ALOGD("%s: udp port %d tcp port %d udp_tsf_port %d:", __func__, udp_port, tcp_port,
        udp_tsf_port);

  if(XpanProviderIf::GetIf())
   XpanProviderIf::GetIf()->PortDetailsCb(tcp_port, udp_port, udp_tsf_port);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);

  return true;
}

bool XMXprofileIf::GetTwtPropStatus(void)
{
	return twtconfig_prop_read;
}

bool XMXprofileIf::WifiAPAvbRsp(bdaddr_t bdaddr, uint8_t  status)
{
  ALOGD("%s: bd addr :%s and status:%d", __func__,
        ConvertRawBdaddress(bdaddr), status);
  if(XpanProviderIf::GetIf())
   XpanProviderIf::GetIf()->WifiAPAvbRsp(bdaddr, status);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);
  return true;
}

bool XMXprofileIf::ChannelSwitchStarted(xm_ipc_msg_t *msg)
{
  ALOGD("%s", __func__);
  if(XpanProviderIf::GetIf())
     XpanProviderIf::GetIf()->SapChannelSwitchStarted(msg->ChannelSwitchStarted.freq,
             msg->ChannelSwitchStarted.tsf, msg->ChannelSwitchStarted.bw);
  else
    ALOGE("Discarding %s AIDL service is not up and running", __func__);
  return true;
}

void XMXprofileIf::ReUpdateTwtSessionParams(TwtParameters *TwtParams)
{
  uint32_t rightOffset = TwtParams->right_earbud_offset;
  uint8_t periodicity = TwtParams->vbc_si;
  uint8_t num_devices = TwtParams->num_devices;
  std::vector<tXPAN_Twt_Session_Params> twtparams;

  ALOGD("%s", __func__);
  if (num_devices) {
    for (XPANTwtSessionParams params: TwtParams->params) {
      tXPAN_Twt_Session_Params twt_params;
      twt_params.SI = params.interval;
      twt_params.SP = params.peroid;
      twt_params.location = params.location;
      for (int j =0; j < sizeof(macaddr_t); j++)
        twt_params.mac_addr.b[j] = params.mac_addr.b[j];

      twtparams.push_back(twt_params);
    }
  }

  UpdateTWTSessionParams(rightOffset, periodicity, num_devices, twtparams);
}

void XMXprofileIf::UpdateTWTSessionParams(uint32_t rightOffset, uint8_t periodicity,
                      uint8_t num_devices, std::vector<tXPAN_Twt_Session_Params> twt_params)
{
  std::unique_lock<std::mutex> lck(profile_sync_mtk);
  int i = 0;
  XPANTwtSessionParams *Twtparams;

  if (!twtconfig_prop_read) {
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("persist.vendor.service.twtconfig", value, "true");
    ALOGI("%s: persist.vendor.service.twtconfig:%s", __func__, value);
    twtconfig_prop_read++;
    if (strcmp(value, "false") == 0) {
      ALOGI("%s: disable to send TWT", __func__);
      twtconfig_prop_read++;
      return;
    }
  }

  if (twtconfig_prop_read == TWT_CONFIG_ENABLE) {
    ALOGD("%s with num_devices %d", __func__, num_devices);
    CHECK(num_devices == twt_params.size());
    xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

    msg->TwtParams.eventId = XP_XM_TWT_SESSION_EST;
    msg->TwtParams.num_devices = num_devices;
    Twtparams = (XPANTwtSessionParams *)
                        malloc(num_devices * sizeof(XPANTwtSessionParams));

    for (tXPAN_Twt_Session_Params params: twt_params) {
      for (int j =0; j < sizeof(macaddr_t); j++)
        Twtparams[i].mac_addr.b[j] = params.mac_addr.b[j];
      Twtparams[i].interval = params.SI;
      Twtparams[i].peroid = params.SP;
      Twtparams[i].location = params.location;
      ALOGD("%s: macaddr %s interval: %d peroid: %d location :%d", __func__,
            ConvertRawMacaddress(Twtparams[i].mac_addr),
      Twtparams[i].interval, Twtparams[i].peroid, Twtparams[i].location);
      i++;
    }
    msg->TwtParams.params = Twtparams;
    msg->TwtParams.right_earbud_offset = rightOffset;
    msg->TwtParams.vbc_si = periodicity;
    ALOGE("%s: XP_XM_TWT_SESSION_EST msg twt addr %p and locat twt %p", __func__, msg->TwtParams.params, Twtparams);
    XpanManager::Get()->PostMessage(msg);
  }
}

void XMXprofileIf::XpBearerPreferenceReq(bdaddr_t bdaddr, uint8_t transport,
		                         uint8_t requestor)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  ALOGI("%s for bdaddr_t:%s with transport: %d", __func__,
        ConvertRawBdaddress(bdaddr), transport);
  msg->BearerPreference.eventId = XP_XM_BEARER_PREFERENCE_REQ;
    memcpy(&msg->BearerPreference.bdaddr, &bdaddr, sizeof(bdaddr_t));
  msg->BearerPreference.transport = transport;
  msg->BearerPreference.requestor = requestor;
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::XpSapPowerSave(uint8_t dialogId, uint8_t mode)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  ALOGD("%s: with dialogId %d and mode :%s",
	__func__, dialogId, mode == 1 ? "enable": "disable");
  msg->SapPowerStateParams.eventId = XP_XM_SAP_POWER_STATE;
  msg->SapPowerStateParams.dialog_id = dialogId;
  msg->SapPowerStateParams.state = mode;
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::HostParameters(macaddr_t macaddr, uint16_t Ethertype)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  ALOGD("%s", __func__);
  msg->HostParams.eventId = XP_XM_HOST_PARAMETERS;
  memcpy(&msg->HostParams.macaddr, &macaddr, sizeof(macaddr_t));
  msg->HostParams.Ethertype = Ethertype;
  XpanManager::Get()->PostMessage(msg);

  /* Queue message to CP work queue */
  {
    xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

    msg->HostParams.eventId = XP_XM_HOST_PARAMETERS;
    memcpy(&msg->HostParams.macaddr, &macaddr, sizeof(macaddr_t));
    msg->HostParams.Ethertype = Ethertype;
    XpanManager::Get()->CacheCpMessage(msg);
  }
}

void XMXprofileIf::SapState(uint16_t state)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  ALOGD("%s: with state:%d", __func__, state);
  msg->SapState.eventId = XP_XM_SAP_CURRENT_STATE;
  msg->SapState.state = state;
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::CreateSapInterface(uint8_t state)
{
  ALOGD("%s: with state:%s", __func__, state? "enable": "disable");
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  msg->CreateSapInfParams.eventId = XP_XM_CREATE_SAP_INF;
  msg->CreateSapInfParams.state = state;
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::MdnsDiscoveryStatus(bdaddr_t bdaddr, uint8_t status, mdns_uuid_t uuid, uint8_t state)
{
  ALOGD("%s: with for %s status %s(%d) state %d", __func__, ConvertRawBdaddress(bdaddr),
        (status? "error": "success"), status, state);

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->MdnsRsp.eventId = XP_XAC_MDNS_RSP;
  memcpy(&msg->MdnsRsp.bdaddr, &bdaddr, sizeof(bdaddr_t));
  msg->MdnsRsp.status = status;
  msg->MdnsRsp.uuid = uuid;
  msg->MdnsRsp.state = state;
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::ConnectLeLinkReq(bdaddr_t bdaddr)
{
  ALOGD("%s for bd address %s", __func__, ConvertRawBdaddress(bdaddr));

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->ConnectToLeLinkReq.eventId = XP_QHCI_CONNECT_LINK_REQ;
  memcpy(&msg->ConnectToLeLinkReq.bdaddr, &bdaddr, sizeof(bdaddr_t));
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::UpdateRemoteApDetails(tXPAN_Remote_Params params)
{
  ALOGD("%s", __func__);

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  msg->EbParams.eventId = XP_XAC_REMOTE_AP_DETAILS;
  memcpy(&msg->EbParams.params.addr, &params.addr, sizeof(bdaddr_t));
  memcpy(&msg->EbParams.params.mac_bssid, &params.mac_bssid, sizeof(macaddr_t));
  msg->EbParams.params.mdns = params.mdns;
  memcpy(&msg->EbParams.params.mdnsUuid, &params.mdnsUuid, sizeof(mdns_uuid_t));
  msg->EbParams.params.portL2capTcp = params.portL2capTcp;
  msg->EbParams.params.portUdpAudio = params.portUdpAudio;
  msg->EbParams.params.portUdpReports = params.portUdpReports;
  msg->EbParams.params.numOfEbs = params.numOfEbs;

  for (int j = 0; j < params.numOfEbs; j++) {
    msg->EbParams.params.vectorEbParams[j]= params.vectorEbParams[j];
  }

  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::GetHandSetPortNumberReq(void)
{
  ALOGD("%s", __func__);

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->HandSetPortNumReq.eventId = XP_XAC_HANDSET_PORT_REQ;
  XpanManager::Get()->PostMessage(msg);
}

void XMXprofileIf::ProcessMessage(XmIpcEventId eventId, xm_ipc_msg_t *msg)
{
  ALOGD("%s %s", __func__, ConvertMsgtoString(eventId));
  switch (eventId) {
    case QHCI_XM_REMOTE_SUPPORT_XPAN: {
      QhciXmRemoteSupportXpan SupportsXpan = msg->SupportsXpan;
      RemoteSupportsXpan(SupportsXpan.bdaddr, SupportsXpan.is_supported);
      break;
    } case QHCI_XM_USECASE_UPDATE: {
      QhciXmUseCase UseCase = msg->UseCase;
      UseCaseUpdate(UseCase.usecase);
      break;
    } case QHCI_XM_PREPARE_AUDIO_BEARER_REQ: {
      QhciXmPrepareAudioBearerReq AudioBearerReq =  msg->AudioBearerReq;
      TransportType type = AudioBearerReq.type;
      bdaddr_t bdaddr = AudioBearerReq.bdaddr;
      XmXpPrepareAudioBearerReq(bdaddr, type); 
      break;
    } case WIFI_XM_ACS_RESULTS: {
      WiFiAcsResults(msg);
      break;
    } case WIFI_XM_SSR_EVENT: {
      WiFiSsrEventInd(msg);
      break;
    }  case WIFI_XM_CSA: {
      ChannelSwitchStarted(msg);
      break;
    } case QHCI_XM_UNPREPARE_AUDIO_BEARER_REQ: {
      QhciXmUnPrepareAudioBearerReq UnPrepareAudioBearerReq =  msg->UnPrepareAudioBearerReq;
      bdaddr_t bdaddr = UnPrepareAudioBearerReq.bdaddr;
      TransportType type = UnPrepareAudioBearerReq.type;
      XmXpPrepareAudioBearerReq(bdaddr, type); 
      break;
    } case WIFI_XM_TWT_EVENT: {
      WifiTwtEvent(msg);
      break;
    } case WIFI_XM_SAP_POWER_SAVE_STATE_RSP: {
      WifiSapPowerSaveEvent(msg);
      break;
    } case XM_XP_CREATE_SAP_INF_STATUS: {
      UpdateSapInterface(msg);
      break;
    } case CP_XM_BEARER_SWITCH_REQ: {
      TransportType type = msg->BearerSwitchReq.transport;
      XmXpPrepareAudioBearerReq(ACTIVE_BDADDR, type);
      break;
    } case XAC_XP_MDNS_REQ : {
      TriggerMdnsQuery(msg->MdnsReq.bdaddr, msg->MdnsReq.status);
      break;
    } case XAC_XP_START_FILTERED_SCAN : {
      StartFilteredScan(msg->FilterScanInd.bdaddr);
      break;
    } case QHCI_XP_CONNECT_LINK_RSP: {
      ConnectLeLinkRsp(msg->ConnectToLeLinkRsp.bdaddr,
		       msg->ConnectToLeLinkRsp.status);
      break;
    } case XP_XAC_HANDSET_PORT_RSP: {
      HandSetPortNumberRsp(msg->HandSetPortNumRsp.tcp_port,
		        msg->HandSetPortNumRsp.udp_port,
			msg->HandSetPortNumRsp.udp_tsf_port);
      break;
    } case XAC_XP_REGISTER_MDNS_SERVICE: {
      RegisterMdnsService(msg->MdnsReq.bdaddr);
      break;
    } case XAC_XP_CURRENT_TRANS_UPDATE: {
      OnCurrentTransportUpdated(msg->TransportUpdate.bdaddr,
		                msg->TransportUpdate.transport);
      break;
    } case WIFI_XP_TRANSPORT_SWITCH_REQ: {
      WifiAPAvbRsp(msg->SetApAvbReq.addr, msg->SetApAvbReq.status);
      break;
    } case WIFI_XP_SET_AP_AVB_RSP : {
      WifiAPAvbRsp(msg->SetApAvbRsp.addr, msg->SetApAvbRsp.status);
      break;
    } default: {
      ALOGI("%s: this :%08x ipc message is not handled", __func__, eventId);
    }
  }
}

void XMXprofileIf::Initialize(void)
{
  XpanProviderCb = {
    .update_bonded_dev = UpdateXpanBondedDevices,
    .update_transport = TransportEnabled,
    .enable_acs = EnableAcs,
    .update_bearer_switched = XpXmBearerSwitchInd,
    .update_twt_params = UpdateTWTSessionParams,
    .update_bearer_prepared = XpXmPrepareAudioBearerRsp,
    .update_low_power_mode = XpSapPowerSave,
    .update_host_params = HostParameters,
    .update_sap_state = SapState,
    .create_sap_interface = CreateSapInterface,
    .update_bearer_preference_req = XpBearerPreferenceReq,
    .update_bond_state = BondStateUpdate,
    .update_clear_to_send_req = NULL,
    .airplane_mode_state_changed = NULL,
    .connect_le_link = ConnectLeLinkReq,
    .update_ap_details_local = UpdateLocalApDetails,
    .update_ap_details_remote = UpdateRemoteApDetails,
    .get_port_details = GetHandSetPortNumberReq,
    .update_mdns_status = MdnsDiscoveryStatus,
    .update_connected_eb_details = UpdateConnectedEbDetails,
    .wifi_transport_preferece_res = WifiTransportPreferenceRsp,
    .set_apavailable_req = SetApAvailableReq,
    .update_wifi_scan_started = UpdateWifiScanStarted,
  };

  ALOGI("%s: registering with XpanProviderIf", __func__);
  if(XpanProviderIf::GetIf())
    XpanProviderIf::GetIf()->RegisterXpanManagerIf(&XpanProviderCb);
  else
    ALOGE("%s: Failed to register with AIDL service", __func__);
}

void XMXprofileIf::Deinitialize(void)
{
 ALOGI("%s: Deregistering with XpanProviderIf", __func__);
 if(XpanProviderIf::GetIf())
   XpanProviderIf::GetIf()->DeregisterXpanManagerIf();
 else
   ALOGE("%s: Failed to Deregister with AIDL service", __func__);
}

} //namespace implementation
} //namespace xpan
