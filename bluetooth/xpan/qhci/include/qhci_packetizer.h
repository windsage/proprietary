/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#ifndef QHCI_PACKETIZER_H
#define QHCI_PACKETIZER_H

#include <stdint.h>
#include <hidl/HidlSupport.h>
#include "hci_transport.h"
#include "xpan_utils.h"
#include <thread>

using ::android::hardware::hidl_vec;

#define QHCI_COMMAND_PREAMBLE_SIZE 3 

//QBCE Vendor Specific Command Opcode
#define QHCI_VS_QBCE 0xFC51

//QLL SET HOST FEATURE SUBCOMMAND
#define QBCE_QHCI_QLE_SET_HOST_FEATURE 0x14

typedef struct {
  uint16_t qhci_event;
  uint16_t qhci_len;
  uint16_t qhci_offset;
  uint16_t qhci_layer_specific;
  uint8_t qhci_data[];
} QHCI_BT_HDR;


typedef enum {
  QHCI_MSG_BASE = 2000,
  QHCI_QLL_CMD_REQ,
  QHCI_QLL_EVT_RSP,
  QHCI_CIG_EVT_RCV,
  QHCI_CIS_CONN_CMD,
  QHCI_CIS_CONN_EVT,
  QHCI_LE_CONN_CMPL_EVT,
  QHCI_ACL_DISCONNECT,
  QHCI_XM_PREPARE_REQ,
  QHCI_XM_PREPARE_REQ_BT,
  QHCI_XM_PREPARE_RSP,
  QHCI_XM_UNPREPARE_REQ,
  QHCI_XM_UNPREPARE_RSP,
  QHCI_UPDATE_TRANSPORT,
  QHCI_TRANSPORT_ENABLE,
  //Move below event to other thread
  QHCI_PROCESS_RX_PKT_EVT,
  QHCI_USECASE_UPDATE_CFM,
  QHCI_USECASE_UPDATE_CMD,
  QHCI_PROCESS_TX_CMD_PKT,
  QHCI_DELAY_REPORT_EVT,
  QHCI_BEARER_SWITCH_IND,
  QHCI_STATE_CHANGE,
  QHCI_VENDOR_ENCODER_LIMIT_CMD,
  QHCI_LOCAL_VER_LE_FEATURES_TO_AC,
  QHCI_SEND_READ_REMOTE_VER_REQ,
  QHCI_SEND_READ_REMOTE_LE_FEATURES,
  QHCI_SEND_CREATE_CONNECT_TO_AC,
  QHCI_SEND_CONNECTION_CMPL_EVENT,
  QHCI_RECV_REMOTE_SUPPORT_LE_FEAT_EVENT,
  QHCI_RECV_REMOTE_VERSION_INFO_EVENT,
  QHCI_RECV_NOCP_FROM_AC,
  QHCI_RECV_CONN_CMPL_FROM_AC,
  QHCI_SEND_LE_ENCRYPT_CMD_TO_AC,
  QHCI_RECV_ENCRYPT_CMPL_FROM_AC,
  QHCI_XPAN_BONDED_DEVICE_LIST,
  QHCI_SEND_DISCONNECT_TO_AC,
  QHCI_RECV_DISCONNECTION_CMPL_EVENT,
  QHCI_RECV_ACL_DATA,
  QHCI_RECV_CONNECT_LE_REQ_FROM_XM,
  QHCI_SEND_CANCEL_CONNECT_TO_AC,
  QHCI_L2CAP_PAUSE_UNPAUSE_REQ,
  QHCI_XM_PREPARE_REQ_AP,
  QHCI_WIFI_SCAN_STARTED_IND,
} QHciEventId;

typedef struct {
  QHciEventId eventId;
  uint16_t handle;
} __attribute__((packed)) QHciQllCmd;

typedef struct {
  QHciEventId eventId;
  uint8_t len;
  uint8_t *data;
  bool from_stack;
} __attribute__((packed)) QHciQllEvt;

typedef struct {
  
}__attribute__((packed)) QHciCigEvt;

typedef struct {
  QHciEventId eventId;
  HciPacketType type;
  hidl_vec<uint8_t> *hidl_data;

  bool from_stack;
} __attribute__((packed)) QHciRxEvtPkt;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint16_t handle;
  uint8_t transport_type;
} __attribute__((packed)) QHciPrepareBearerReq;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint16_t handle;
  bool status;
} __attribute__((packed)) QHciPrepareBearerRsp;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint8_t transport_type;
} __attribute__((packed)) QHciUnPrepareBearerReq;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  bool status;
} __attribute__((packed)) QHciUnPrepareBearerRsp;

typedef struct {
  QHciEventId eventId;
  uint8_t usecase;
  bool status;
} __attribute__((packed)) QHciUsecaseUpdateCfm;

typedef struct {
  QHciEventId eventId;
  bool enable;
  uint8_t reason;
  bdaddr_t bd_addr;
  uint16_t handle;
} QHciTransportEnabled;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint16_t handle;
  uint8_t transport_type;
} __attribute__((packed)) QHciUpdateTransport;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  TransportType transport;
  uint8_t ind_status;
} __attribute__((packed)) QHciBearerSwitchInd;

typedef struct {
  QHciEventId eventId;
  uint8_t *data;
} __attribute__((packed)) QHciTxCmdPkt;

typedef struct {
  QHciEventId eventId;
  uint8_t opcode;
  uint8_t length;
  uint16_t acl_handle;
  UseCaseType context_type;
} QHciTxUsecaseRcvd;

typedef struct {
  QHciEventId eventId;
  uint32_t delay_report;
} QHciDelayReport;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint16_t handle;
} QHciConnCmpl;

typedef struct {
  QHciEventId eventId;
  uint16_t cis_handle;
  uint8_t state;
  uint8_t csm_event;
} QHciStateChange;

typedef struct {
  QHciEventId eventId;
  uint8_t num_limit;
  uint8_t *data;
} QHciEncoderLimit;

typedef struct {
  QHciEventId eventId;
} QHciLocalVerFeatureToAc;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint8_t noOfPacketsSent;
} QHciNocpFromAc;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint8_t status;
} QHciConnCmplFromAc;

typedef struct {
  QHciEventId eventId;
  uint16_t handle;
  uint8_t ltk[16];
} QHciLeEncrptCmdToAc;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint8_t status;
  uint8_t encr_enable;
} QHciEncrResFromAc;

typedef struct {
  QHciEventId eventId;
  uint16_t handle;
} QHciGetLeFeatFromAc;

typedef struct {
  QHciEventId eventId;
  uint16_t handle;
} QHciGetVersionFromAc;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint8_t status;
  uint64_t featureMask;
} QHciLeFeatResFromAC;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint8_t version;
  uint16_t companyId;
  uint16_t subversion;
} QHciRemoteVerFromAC;

typedef struct {
  QHciEventId eventId;
  uint8_t num_devices;
  bdaddr_t* bonded_devices;
} QHciBondedDevList;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint16_t supervision_tout;
} QHciLeConnToAc;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
} QHciDisconnToAc;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint8_t status;
} QHciDisconnCmplFromAc;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint8_t *rx_data;
  uint16_t len;
} QHciRxDataFromAc;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
} QHciLeConnFromXm;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
} QHciCancelConnToAc;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint8_t pause;
  TransportType transportType;
} QHciL2capPauseUnPauseReq;

typedef struct {
  QHciEventId eventId;
  bdaddr_t bd_addr;
  uint8_t state;
} QHciWifiScanStartedInd;


typedef union {
  QHciEventId eventId;
  QHciQllCmd QllCmd;
  QHciConnCmpl ConnCmpl;
  QHciQllEvt QllEvt;
  QHciRxEvtPkt RxEvtPkt;
  QHciTxCmdPkt TxCmdPkt;
  QHciTxUsecaseRcvd TxUsecaseRcvd;
  QHciPrepareBearerReq PreBearerReq;
  QHciPrepareBearerRsp PreBearerRsp;
  QHciUnPrepareBearerReq UnPreBearerReq;
  QHciUnPrepareBearerRsp UnPreBearerRsp;
  QHciUsecaseUpdateCfm UsecaseUpdateCfm;
  QHciTransportEnabled TransportEnabled;
  QHciUpdateTransport UpdateTransport;
  QHciDelayReport DelayReport;
  QHciBearerSwitchInd BearerSwitchInd;
  QHciStateChange StateChangeEvt;
  QHciEncoderLimit EncoderLimitCmd;
  QHciNocpFromAc NocpFromAc;
  QHciConnCmplFromAc ConnCmplFromAc;
  QHciLeEncrptCmdToAc LeEncrptCmdToAc;
  QHciEncrResFromAc EncrResFromAc;
  QHciGetLeFeatFromAc GetLeFeatFromAc;
  QHciGetVersionFromAc GetVersionFromAc;
  QHciLeFeatResFromAC LeFeatResFromAC;
  QHciRemoteVerFromAC RemoteVerFromAC;
  QHciBondedDevList BondedDevList;
  QHciLeConnToAc LeConnToAc;
  QHciDisconnToAc DisconnToAc;
  QHciDisconnCmplFromAc DisconnCmplFromAc;
  QHciRxDataFromAc RxDataFromAc;
  QHciLeConnFromXm LeConnFromXm;
  QHciCancelConnToAc CancelConnToAc;
  QHciL2capPauseUnPauseReq L2capPauseUnPauseReq;
  QHciWifiScanStartedInd WifiScanStartedInd;
} qhci_msg_t;

namespace xpan {
namespace implementation {

class QHciPacketizer
{
  public:
    QHciPacketizer();
    ~QHciPacketizer();
    void ProcessMessage(qhci_msg_t *msg);

  private:
    QHCI_BT_HDR* QHciMakeCommand(uint16_t opcode, size_t parameter_size,
                                      uint8_t** stream_out);
    QHCI_BT_HDR* QHciMakeQbceSetHostFeature(uint8_t bit_position,
                                            uint8_t bit_value);
    QHCI_BT_HDR* QHciMakePacket(size_t data_size);
};

} // namespace implementation
} // namespace xpan
#endif


