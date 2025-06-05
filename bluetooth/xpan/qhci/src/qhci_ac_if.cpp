/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <stdint.h>
#include <hidl/HidlSupport.h>
#include "hci_transport.h"
#include "qhci_main.h"
#include "qhci_packetizer.h"
#include "qhci_ac_if.h"
#include "xm_qhci_if.h"
#include "xpan_utils.h"
#include "XpanAcQhciIf.h"
#include <utils/Log.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.qhci@1.0-xpan_qhci_ac_intf"

using xpan::ac::XpanAcQhciIf;

namespace xpan {
namespace implementation {

std::shared_ptr<XpanQhciAcIf> XpanQhciAcIf::qhci_ac_instance = nullptr;

XpanQhciAcIf::XpanQhciAcIf() {
  ALOGD("%s", __func__);
}

XpanQhciAcIf::~XpanQhciAcIf() {
  ALOGD("%s", __func__);
}

std::shared_ptr<XpanQhciAcIf> XpanQhciAcIf::GetIf() {
  if (!qhci_ac_instance)
    qhci_ac_instance.reset(new XpanQhciAcIf());
  return qhci_ac_instance;
}

void XpanQhciAcIf::DeInit() {
  ALOGD("%s", __func__);

  if (qhci_ac_instance) {
    qhci_ac_instance.reset();
    qhci_ac_instance = NULL;
  }
}

/******************************************************************
 *
 * Function       CreateConnection
 *
 * Description    Initiating CreateConnection to AC module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *                uint16_t - superVisionTimeout
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::CreateConnection(bdaddr_t addr,
                                          uint16_t supervision_timeout) {
  ALOGD("%s addr %s", __func__, ConvertRawBdaddress(addr));

  XpanAcQhciIf::GetIf()->CreateConnection(addr, supervision_timeout);

  return true;
}

/******************************************************************
 *
 * Function       DisconnectConnection
 *
 * Description    Initiating Disconnect to AC module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::DisconnectConnection(bdaddr_t addr) {
  ALOGD("%s addr %s", __func__, ConvertRawBdaddress(addr));

  XpanAcQhciIf::GetIf()->DisconnectConnection(addr);

  return true;
}

/******************************************************************
 *
 * Function       GetRemoteVersion
 *
 * Description    Fetching Remote Version from AC module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::GetRemoteVersion  (bdaddr_t addr) {
  ALOGD("%s addr %s", __func__, ConvertRawBdaddress(addr));

  XpanAcQhciIf::GetIf()->GetRemoteVersion(addr);

  return true;
}

/******************************************************************
 *
 * Function       GetRemoteLeFeatures
 *
 * Description    Fetching Remote LE Features from AC module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::GetRemoteLeFeatures   (bdaddr_t addr) {
  ALOGD("%s addr %s", __func__, ConvertRawBdaddress(addr));

  XpanAcQhciIf::GetIf()->GetRemoteLeFeatures(addr);

  return true;
}

/******************************************************************
 *
 * Function       EnableEncrption
 *
 * Description    Trigger Start Enable to AC Module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *                LTK - std::vector<uint8_t>
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::EnableEncrption(bdaddr_t addr,
                                         const std::vector<uint8_t>& ltk) {
  ALOGD("%s addr %s", __func__, ConvertRawBdaddress(addr));

  //XpanAcQhciIf::GetIf()->EnableEncrption(addr, ltk);
  EncryptionChangedRes(addr, 0, 1);

  return true;
}

/******************************************************************
 *
 * Function       SendAclData
 *
 * Description    Send BTHost data to remote over TCP via AC Module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *                uint8_t - llid
 *                std::vector<uint8_t> - data
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::SendAclData(bdaddr_t addr, uint8_t llid,
                                    const std::vector<uint8_t>& data) {
  ALOGD("%s addr %s length %d", __func__, ConvertRawBdaddress(addr),
          data.size());

  XpanAcQhciIf::GetIf()->SendAclData(addr, llid, data);

  return true;
}

/******************************************************************
 *
 * Function       L2capPauseUnpauseRes
 *
 * Description    Send L2CAP Pause and Unpause response to AC Module
 *
 * Arguments      Remote bluetooth addres - bdaddr_t
 *                uint8_t - pause
 *                uint8_t - status
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::L2capPauseUnpauseRes(bdaddr_t addr, uint8_t pause,
                                                uint8_t status) {
  ALOGD("%s ", __func__);

  XpanAcQhciIf::GetIf()->L2capPauseUnpauseRes(addr, pause, status);

  return true;
}

/******************************************************************
 *
 * Function       UpdateLocalVersion
 *
 * Description    sending local version details to AC module
 *
 * Arguments      uint8_t - version
 *                uint16_t - companyId
 *                uint16_t - subversion
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::UpdateLocalVersion(uint8_t version, uint16_t companyId,
                                             uint16_t subversion) {
  ALOGD("%s ", __func__);

  XpanAcQhciIf::GetIf()->UpdateLocalVersion(version, companyId, subversion);

  return true;
}

/******************************************************************
 *
 * Function       UpdateLocalLeFeatures
 *
 * Description    sending local version details to AC module
 *
 * Arguments      uint64_t - le_features
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::UpdateLocalLeFeatures(uint64_t le_features) {
  ALOGD("%s 0x%llx", __func__, le_features);

  XpanAcQhciIf::GetIf()->UpdateLocalLeFeatures(le_features);

  return true;
}

/******************************************************************
 *
 * Function       L2capPauseUnpauseReq
 *
 * Description    Received L2CAP Pause and Unpause Req from
 *                AC Module
 *
 * Arguments      Remote bluetooth addres - bdaddr_t
 *                uint8_t - pause
 *                uint8_t - status
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::L2capPauseUnpauseReq(bdaddr_t bdAddr, uint8_t pause,
                                                TransportType transport) {
  ALOGD("%s Pause: %d , TransportType: %s", __func__, pause,
    TransportTypeToString(transport));

  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_AUDIO_BEARER_REQ_SIZE +
                                            sizeof(bdAddr));
  msg->eventId = QHCI_L2CAP_PAUSE_UNPAUSE_REQ;
  msg->L2capPauseUnPauseReq.eventId = QHCI_L2CAP_PAUSE_UNPAUSE_REQ;
  msg->L2capPauseUnPauseReq.pause = pause;
  msg->L2capPauseUnPauseReq.transportType = transport;

  for (int i = 0; i < 6; i++)
    msg->L2capPauseUnPauseReq.bd_addr.b[i] = bdAddr.b[i];

  QHci::Get()->PostMessage(msg);

  return true;
}

/******************************************************************
 *
 * Function       ConnectionCompleteRes
 *
 * Description    Receiving Connection complete from AC module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *                uint8_t - status
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::ConnectionCompleteRes(bdaddr_t bdAddr, uint8_t status) {
  ALOGD("%s ", __func__);
  
  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_AUDIO_BEARER_REQ_SIZE +
                                            sizeof(bdAddr));
  msg->eventId = QHCI_RECV_CONN_CMPL_FROM_AC;
  msg->ConnCmplFromAc.eventId = QHCI_RECV_CONN_CMPL_FROM_AC;
  msg->ConnCmplFromAc.status = status;

  for (int i = 0; i < 6; i++)
    msg->ConnCmplFromAc.bd_addr.b[i] = bdAddr.b[i];

  QHci::Get()->PostMessage(msg);

  return true;
}

/******************************************************************
 *
 * Function       DisconnectionCompleteRes
 *
 * Description    receiving  Disconnect Complete Response from
 *                AC module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *                uint8_t - status
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::DisconnectionCompleteRes (bdaddr_t bdAddr, uint8_t status) {
  ALOGD("%s ", __func__);

  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_AUDIO_BEARER_REQ_SIZE +
                                            sizeof(bdAddr));
  msg->eventId = QHCI_RECV_DISCONNECTION_CMPL_EVENT;
  msg->DisconnCmplFromAc.eventId = QHCI_RECV_DISCONNECTION_CMPL_EVENT;
  msg->DisconnCmplFromAc.status = status;

  for (int i = 0; i < 6; i++)
    msg->DisconnCmplFromAc.bd_addr.b[i] = bdAddr.b[i];

  QHci::Get()->PostMessage(msg);

  return true;
}

/******************************************************************
 *
 * Function       RemoteVersionRes
 *
 * Description    receiving remote verison from AC module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *                uint8_t - version
 *                uint16_t - companyId
 *                uint16_t - subversion
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::RemoteVersionRes(bdaddr_t bdAddr, uint8_t version,
                                          uint16_t companyId,
                                          uint16_t subversion) {
  ALOGD("%s version %d companyId %d subversion %d", __func__, version,
         companyId, subversion);

  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_AUDIO_BEARER_REQ_SIZE +
                                            sizeof(bdAddr));
  msg->eventId = QHCI_RECV_REMOTE_VERSION_INFO_EVENT;
  msg->RemoteVerFromAC.eventId = QHCI_RECV_REMOTE_VERSION_INFO_EVENT;
  msg->RemoteVerFromAC.version = version;
  msg->RemoteVerFromAC.companyId = companyId;
  msg->RemoteVerFromAC.subversion = subversion;

  for (int i = 0; i < 6; i++)
    msg->RemoteVerFromAC.bd_addr.b[i] = bdAddr.b[i];

  QHci::Get()->PostMessage(msg);

  return true;
}

/******************************************************************
 *
 * Function       RemoteLeFeaturesRes
 *
 * Description    receiving remote verison from AC module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *                uint8_t - version
 *                uint16_t - companyId
 *                uint16_t - subversion
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::RemoteLeFeaturesRes(bdaddr_t bdAddr, uint8_t status,
                                              uint64_t featureMask) {
  ALOGD("%s FeatureMask 0x%llx", __func__, featureMask);

  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_AUDIO_BEARER_REQ_SIZE +
                                            sizeof(bdAddr));
  msg->eventId = QHCI_RECV_REMOTE_SUPPORT_LE_FEAT_EVENT;
  msg->LeFeatResFromAC.eventId = QHCI_RECV_REMOTE_SUPPORT_LE_FEAT_EVENT;
  msg->LeFeatResFromAC.status = status;
  msg->LeFeatResFromAC.featureMask = featureMask;

  for (int i = 0; i < 6; i++)
    msg->LeFeatResFromAC.bd_addr.b[i] = bdAddr.b[i];

  QHci::Get()->PostMessage(msg);

  return true;
}

/******************************************************************
 *
 * Function       EncryptionChangedRes
 *
 * Description    Encryption Response from AC Module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *                uint8_t - status
 *                uint8_t - encryptionEnabled
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::EncryptionChangedRes(bdaddr_t bdAddr, uint8_t status,
                                                uint8_t encryptionEnabled) {
  ALOGD("%s ", __func__);

  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_AUDIO_BEARER_REQ_SIZE +
                                            sizeof(bdAddr));
  msg->eventId = QHCI_RECV_ENCRYPT_CMPL_FROM_AC;
  msg->EncrResFromAc.eventId = QHCI_RECV_ENCRYPT_CMPL_FROM_AC;
  msg->EncrResFromAc.status = status;
  msg->EncrResFromAc.encr_enable = encryptionEnabled;

  for (int i = 0; i < 6; i++)
    msg->EncrResFromAc.bd_addr.b[i] = bdAddr.b[i];

  QHci::Get()->PostMessage(msg);

  return true;
}

/******************************************************************
 *
 * Function       DataReceivedCb
 *
 * Description    Received data from remote over TCP via AC Module
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *                uint8_t - llid
 *                std::vector<uint8_t> - data
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::DataReceivedCb(bdaddr_t bd_addr, uint8_t llid,
                                    const std::vector<uint8_t>& data) {
  ALOGD("%s data Length %d", __func__, data.size());

  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_AUDIO_BEARER_REQ_SIZE +
                                            sizeof(bd_addr) + data.size());
  msg->eventId = QHCI_RECV_ACL_DATA;
  msg->RxDataFromAc.eventId = QHCI_RECV_ACL_DATA;
  msg->RxDataFromAc.rx_data = (uint8_t *)malloc((size_t)data.size());
  msg->RxDataFromAc.len = (uint16_t)data.size();
  memcpy(msg->RxDataFromAc.rx_data, data.data(), (size_t)data.size());

  for (int i = 0; i < 6; i++)
    msg->RxDataFromAc.bd_addr.b[i] = bd_addr.b[i];

  QHci::Get()->PostMessage(msg);

  return true;
}

/******************************************************************
 *
 * Function       NumberOfPacketsCompleted
 *
 * Description    NOCP callback
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *                uint8_t - noOfPackets Sent
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::NumberOfPacketsCompleted(bdaddr_t bdAddr,
                                                     uint8_t noOfPacketsSent) {
  ALOGD("%s ", __func__);

  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_AUDIO_BEARER_REQ_SIZE +
                                            sizeof(bdAddr));
  msg->eventId = QHCI_RECV_NOCP_FROM_AC;
  msg->NocpFromAc.eventId = QHCI_RECV_NOCP_FROM_AC;
  msg->NocpFromAc.noOfPacketsSent = noOfPacketsSent;

  for (int i = 0; i < 6; i++)
    msg->NocpFromAc.bd_addr.b[i] = bdAddr.b[i];

  QHci::Get()->PostMessage(msg);

  return true;
}

/******************************************************************
 *
 * Function       isStreamingActive
 *
 * Description    Is LE Audio streaming is active or not?
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::isStreamingActive(bdaddr_t bdAddr) {
  ALOGD("%s ", __func__);

  return QHci::Get()->isStreamingActive(bdAddr);
}

/******************************************************************
 *
 * Function       CreateConnectionCancel
 *
 * Description    Sending Cancel connect request to AC
 *
 * Arguments      bdaddr_t - Remote bluetooth address
 *
 * return         bool
 ******************************************************************/
bool XpanQhciAcIf::CreateConnectionCancel(bdaddr_t bdAddr) {
  ALOGD("%s ", __func__);

  return XpanAcQhciIf::GetIf()->CreateConnectionCancel(bdAddr);
}


}
}


