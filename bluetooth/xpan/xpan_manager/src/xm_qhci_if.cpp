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
#include "xm_qhci_if.h"
#include "qhci_xm_if.h"
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.xpan@1.0-xmqhciif"

namespace xpan {
namespace implementation {
QHciXmIntf qhci_xm_if;
XMQhciIf::XMQhciIf()
{
}

XMQhciIf::~XMQhciIf()
{
}

/******************************************************************
 *
 * Function         RemoteSupportsXpan
 *
 * Description      This API is invoked by QHCI when a new remote
 *                  is connected.
 * Parameters:      bdaddr_t - Bluetooth address of the remote.
 *                  enable   - add a place holder always set to true.
 *
 * Return:          bool - return the status wether the message is
 *                  queued to process by XM.
 ******************************************************************/
bool XMQhciIf::RemoteSupportsXpan(bdaddr_t addr, bool enable)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  ALOGI("%s with bd address %s and supported: %d", __func__,
        ConvertRawBdaddress(addr), enable);
  msg->SupportsXpan.eventId = QHCI_XM_REMOTE_SUPPORT_XPAN;
  memcpy(&msg->SupportsXpan.bdaddr, &addr, sizeof(bdaddr_t));
  msg->SupportsXpan.is_supported = enable;
  return XpanManager::Get()->PostMessage(msg);
}

/******************************************************************
 *
 * Function         PrepareAudioBearerReq
 *
 * Description      This API is invoked by QHCI/XM when Bearer has
 *                  to be prepared before streaming or during seamless
 *                  transition.
 *
 * Parameters:      bdaddr_t - Bluetooth address of the remote.
 *                  type     - Type of transport to be prepared.
 *                  dir      - API directions.
 *
 * Return:          bool - return the status wether the message is
 *                  queued to process by XM or QHCI.
 ******************************************************************/
bool XMQhciIf::PrepareAudioBearerReq(bdaddr_t addr, TransportType type,
		                     ApiDirection dir)
{
  ALOGI("%s for bdaddr_t:%s with transport: %s", __func__,
        (ConvertRawBdaddress(addr)), (TransportTypeToString(type)));
  if(dir == QHCI_To_XM) {
    xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

    msg->AudioBearerReq.eventId = QHCI_XM_PREPARE_AUDIO_BEARER_REQ;
    memcpy(&msg->AudioBearerReq.bdaddr, &addr, sizeof(bdaddr_t));
    msg->AudioBearerReq.type = type;
    return XpanManager::Get()->PostMessage(msg);
  } else {
    qhci_xm_if.PrepareAudioBearerReqFromXm(addr, type);
    return true;
  }
}

/******************************************************************
 *
 * Function         UseCaseUpdate
 *
 * Description      This API is invoked by QHCI to notify on usecase.
 *
 * Parameters:      usecase - Usecase type
 *
 * Return:          bool - return the status whether the message is
 *                  queued to process by XM.
 ******************************************************************/
bool XMQhciIf::UseCaseUpdate(UseCaseType usecase)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  ALOGI("%s usecase: %s", __func__, UseCaseToString(usecase));
  msg->UseCase.eventId = QHCI_XM_USECASE_UPDATE;
  msg->UseCase.usecase = usecase;
  return XpanManager::Get()->PostMessage(msg);
}

/******************************************************************
 *
 * Function         UnPrepareAudioBearerReq
 *
 * Description      This API is invoked by QHCI to unprepare bearer.
 *
 * Parameters:      bdaddr_t - Bluetooth address of the remote.
 *
 * Return:          bool - return the status wether the message is
 *                  queued to process by XM.
 ******************************************************************/
bool XMQhciIf::UnPrepareAudioBearerReq(bdaddr_t addr, TransportType type)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  ALOGI("%s for bdaddr_t:%s", __func__, ConvertRawBdaddress(addr));
  msg->UnPrepareAudioBearerReq.eventId = QHCI_XM_UNPREPARE_AUDIO_BEARER_REQ;
  memcpy(&msg->UnPrepareAudioBearerReq.bdaddr, &addr, sizeof(bdaddr_t));
  msg->UnPrepareAudioBearerReq.type = type;
  return XpanManager::Get()->PostMessage(msg);
}

/******************************************************************
 *
 * Function         AudioTransportUpdate
 *
 * Description      This API is invoked by QHCI when there is
 *                  state update i.e. audio is streaming on a new
 *                  transport
 *
 * Parameters:      bdaddr_t - Bluetooth address of the remote.
 *                  type - Transport on which audio is stream.
 *
 * Return:          bool - return the status wether the message is
 *                  queued to process by XM.
 ******************************************************************/
bool XMQhciIf::AudioTransportUpdate(bdaddr_t addr, TransportType type)
{
  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  ALOGI("%s for bdaddr_t:%s", __func__, ConvertRawBdaddress(addr));
  msg->UnPrepareAudioBearerReq.eventId = QHCI_XM_AUDIO_STREAM_UPDATE;
  memcpy(&msg->AudioUpdate.bdaddr, &addr, sizeof(bdaddr_t));
  msg->AudioUpdate.type = type;
  return XpanManager::Get()->PostMessage(msg);
}

/******************************************************************
 *
 * Function         TransportEnabled
 *
 * Description      This API is invoked by XM to notify QHCI the status
 *                  of SAP
 *
 * Parameters:      bdaddr_t - Bluetooth address of the remote.
 *                  status   - holds the status of SAP connection.
 *
 * Return:          bool - return the status wether the message is
 *                  queued to process by QHCI.
 ******************************************************************/

bool XMQhciIf::XpTransportEnabled(bdaddr_t addr, TransportType type,
		                  bool status, uint8_t reason)
{
  ALOGD("%s", __func__);
  qhci_xm_if.TransportEnabledFromXm(addr, type, status, reason);
  return true;
}

/******************************************************************
 *
 * Function         PrepareAudioBearerRsp
 *
 * Description      This API is invoked by XM or QHCI to notify the status
 *                  of Audio Bearer req.
 *
 * Parameters:      bdaddr_t - Bluetooth address of the remote.
 *                  status   - holds the status of SAP connection.
 *                  dir      - API directions.
 *
 * Return:          bool - return the status wether the message is
 *                  queued to process by QHCI/XM.
 ******************************************************************/
bool XMQhciIf::PrepareAudioBearerRsp(bdaddr_t addr, RspStatus status,
		                     ApiDirection dir)
{
  ALOGI("%s for bdaddr_t:%s with status :%d", __func__,
		  ConvertRawBdaddress(addr), status);
  if (dir == QHCI_To_XM) {
    xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

    msg->AudioBearerRsp.eventId = XM_QHCI_PREPARE_AUDIO_BEARER_RSP;
    memcpy(&msg->AudioBearerRsp.bdaddr, &addr, sizeof(bdaddr_t));
    msg->AudioBearerRsp.status = status;
    return XpanManager::Get()->PostMessage(msg);
  } else {
    qhci_xm_if.PrepareAudioBearerRspFromXm(addr, status);
    return true;
  }
}

bool XMQhciIf::EncoderLimitUpdate(uint8_t num_limit, uint8_t *data)
{
  int total_len = num_limit * XM_ENCODER_LIMIT_PARAMS_SIZE;
  ALOGD("%s: num_limit %d", __func__, num_limit);

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);

  msg->EncoderLimitParams.eventId = QHCI_CP_ENCODER_LIMIT_UPDATE;
  msg->EncoderLimitParams.num_limit = num_limit;
  msg->EncoderLimitParams.data = (uint8_t *) malloc(sizeof(uint8_t) * total_len);
  for (int i = 0; i < total_len; i++) {
    msg->EncoderLimitParams.data[i] = data[i];
  }
  return XpanManager::Get()->PostMessage(msg);
}

bool XMQhciIf::ConnectLeLinkRsp(bdaddr_t bdaddr, RspStatus status)
{
  ALOGD("%s for bd address %s with status %s", __func__,
        ConvertRawBdaddress(bdaddr), status ? "connected": "not connected");

  xm_ipc_msg_t *msg = (xm_ipc_msg_t *) malloc(XM_IPC_MSG_SIZE);
  msg->ConnectToLeLinkRsp.eventId = QHCI_XP_CONNECT_LINK_RSP;
  memcpy(&msg->ConnectToLeLinkRsp.bdaddr, &bdaddr, sizeof(bdaddr_t));
  msg->ConnectToLeLinkRsp.status = status;
  return XpanManager::Get()->PostMessage(msg);
}

bool XMQhciIf::BearerSwitchInd(bdaddr_t addr, TransportType type,
                                     uint8_t status)
{
  ALOGD("%s", __func__);
  qhci_xm_if.BearerSwitchInd(addr, type, (uint8_t)status);
  return true;
}

UseCaseType XMQhciIf::GetCurrentUsecase(void)
{
  ALOGD("%s", __func__);
  return qhci_xm_if.GetUsecaseTypeInfo();
}

/******************************************************************
 *
 * Function         UnPrepareAudioBearerRsp
 *
 * Description      This API is invoked by XM to notify QHCI the status
 *                  of audio bearer req.
 *
 * Parameters:      bdaddr_t - Bluetooth address of the remote.
 *                  status   - holds the status of SAP connection.
 *
 * Return:          bool - return the status wether the message is
 *                  queued to process by QHCI.
 ******************************************************************/
bool XMQhciIf::UnPrepareAudioBearerRsp(bdaddr_t addr, RspStatus status)
{
  ALOGD("%s", __func__);
  qhci_xm_if.UnPrepareAudioBearerRspFromXm(addr, status);
  return true;
}

bool XMQhciIf::DelayReporting(xm_ipc_msg_t * msg)
{
  ALOGD("%s", __func__);
  qhci_xm_if.DelayReporting(msg->DelayReporting.delay_reporting);
  return true;
}

bool XMQhciIf::TransportUpdate(xm_ipc_msg_t * msg)
{
  ALOGD("%s", __func__);
  qhci_xm_if.UpdateTransportFromXm(msg->TransportUpdate.transport);
  return true;
}

bool XMQhciIf::UpdateXpanBondedDevices(uint8_t numOfDevices, bdaddr_t *devices)
{
  ALOGD("%s", __func__);
  qhci_xm_if.UpdateXpanBondedDevices(numOfDevices, devices);
  return true;
}

bool XMQhciIf::ConnectLeLinkReq(bdaddr_t bdaddr)
{
  ALOGD("%s for bd address %s", __func__, ConvertRawBdaddress(bdaddr));
  qhci_xm_if.ConnectLeLinkReqFromXm(bdaddr);
  return true;
}

bool XMQhciIf::UpdateWifiScanStarted(bdaddr_t bdaddr, uint8_t state)
{
  ALOGD("%s  %s  %d ", __func__, ConvertRawBdaddress(bdaddr), state);
  qhci_xm_if.UpdateWifiScanStarted(bdaddr, state);
  return true;
}

void XMQhciIf::ProcessMessage(XmIpcEventId eventId, xm_ipc_msg_t *msg)
{
  switch (eventId) {
    case XP_XM_TRANSPORT_ENABLED: {
      XmXpTransportEnabled TransportEnabled = msg->TransportEnabled;
      XpTransportEnabled(TransportEnabled.bdaddr, TransportEnabled.type,
		         TransportEnabled.status, TransportEnabled.reason);
      break;
    } case XP_XM_BONDED_DEVICES_LIST: {
      UpdateXpanBondedDevices(msg->BondedDevies.numOfDevices,
		              msg->BondedDevies.bdaddr);
      break;
    } case XP_QHCI_CONNECT_LINK_REQ: {
      ConnectLeLinkReq(msg->ConnectToLeLinkReq.bdaddr);
      break;
    } case XP_WIFI_SCAN_STARTED: {
       UpdateWifiScanStarted(msg->XpWifiScanStarted.bdaddr, msg->XpWifiScanStarted.state);
       break;
      } default: {
      ALOGI("%s: this :%04x ipc message is not handled", __func__, eventId);
    }
  }
}
} // namespace implementation
} // namespace xpan
