/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#ifndef QHCI_MAIN_H
#define QHCI_MAIN_H

#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <string>
#include "qhci_packetizer.h"
#include <hidl/HidlSupport.h>
#include "hci_transport.h"
#include "qhci_xm_if.h"
#include "xpan_utils.h"


//#include "logger.h"

#define HCI_DATA_HANDLE_MASK 0x0FFF
#define HCI_SUCCESS 0x00

#define HCI_CHANGE_CONN_PACKET_TYPE 0x040F
#define HCI_AUTHENTICATION_REQUESTED 0x0411
#define HCI_SET_CONN_ENCRYPTION 0x0413
#define HCI_READ_RMT_FEATURES 0x041B
#define HCI_READ_RMT_EXT_FEATURES 0x041C
#define HCI_READ_RMT_VERSION_INFO 0x041D
#define HCI_READ_RMT_CLOCK_OFFSET 0x041F
#define HCI_SNIFF_MODE 0x0803
#define HCI_EXIT_SNIFF_MODE 0x0804
#define HCI_FLOW_SPECIFICATION 0x0810
#define HCI_SNIFF_SUB_RATE 0x0811
#define HCI_WRITE_POLICY_SETTINGS 0x080D
#define HCI_READ_ENCRYPTION_MODE 0x0C21
#define HCI_WRITE_LINK_SUPER_TOUT 0x0C37
#define HCI_READ_ENCR_KEY_SIZE 0x1408

#define QHCI_CREATE_CIS_CMD_SIZE 4
#define QHCI_CIS_ESTABLISHED_EVT_SIZE 31

#define HCI_LE_ADD_DEVICE_TO_WHITE_LIST 0x2011
#define HCI_LE_SET_CIG_PARAMETERS 0x2062
#define HCI_LE_SETUP_ISO_DATA_PATH 0x206E
#define HCI_LE_CREATE_CIS 0x2064
#define HCI_DISCONNECT 0x0406
#define HCI_LE_REMOVE_CIG 0x2065
#define HCI_VENDOR_USECASE_UPDATE 0xFC0A
#define HCI_LE_EXT_CREATE_CONN 0x2043
#define HCI_LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST 0x2012
#define HCI_LE_ENABLE_ENCRYPTION_CMD 0x2019

#define HCI_LE_CREATE_CANCEL_CONN_CMD 0x200E

#define HCI_LE_SET_DATA_LENGTH 0x2022

#define HCI_LE_READ_REMOTE_FEAT_CMD 0x2016
#define HCI_READ_REMOTE_VERSION_CMD 0x041D

#define HCI_LE_READ_LOCAL_SUPP_FEATURES 0x2003
#define HCI_READ_LOCAL_VERSION_INFORM 0x1001

#define HCI_LE_CONN_UPDATE_CMD 0x2013

#define HCI_VENDOR_ENCODER_LIMIT_CMD 0xFC51
#define HCI_SUB_OPCODE_ENCODER_LIMIT_CMD 0x24

#define HCI_SUB_OPCODE_QLE_SET_HOST_FEATURE 0x14
#define QHCI_SET_HOST_SUPPORTED_BIT_COMMAND_LEN 6

#define HCI_BLE_UPD_LL_CONN_PARAMS 0x2013
#define HCI_BLE_READ_REMOTE_FEAT 0x2016
#define HCI_BLE_START_ENC 0x2019
#define HCI_BLE_RC_PARAM_REQ_REPLY 0x2020
#define HCI_BLE_SET_DATA_LENGTH 0x2022
#define HCI_BLE_SET_PHY 0x2032
#define HCI_LE_SET_PERIODIC_ADVERTISING_SET_INFO_TRANSFER 0x205B
#define QHCI_DISCONNECT_COMMAND_LEN 6

#define VSC_QHCI_VENDOR_OPCODE 0x15
#define VSC_QHCI_VENDOR_USECASE_EVT_LEN 9
#define HCI_ENCRYPT_CHANGE_EVT_LEN 6

#define QHCI_DELAY_REPORT_EVT_LEN 6
#define QHCI_CMD_STATUS_CIS_LEN 6
#define QHCI_CMD_CMPLT_ENCODER_LIMIT 9
#define QHCI_CMD_CMPLT_LE_CANCEL_CONN_LEN 6
#define QHCI_CMD_CMPLT_SETUP_ISO_PATH_LEN 8

#define HCI_CONNECTION_COMP_EVT 0x03
#define HCI_DISCONNECT_CMPL_EVT 0x05
#define HCI_AUTHENTICATION_COMP_EVT 0x06
#define HCI_READ_RMT_FEATURES_COMP_EVT 0x0B
#define HCI_READ_RMT_VERSION_COMP_EVT 0x0C

#define HCI_LE_EVT 0x3E
#define HCI_VENDOR_EVT 0xFF
#define QHCI_VS_TIMESYNC_EVT 0xD1
#define QHCI_QBCE_SUBEVENT_CODE 0x51
#define READ_REMOTE_QLL_FEATURE_CMPL_EVT 0x06
#define HCI_ENCRYPT_CHANGE_EVT 0x08
#define HCI_READ_RMT_EXT_FEATURES_COMP_EVT 0x23
#define HCI_LINK_SUPER_TOUT_CHANGED_EVT 0x38
#define QHCI_COMMAND_COMPLETE_EVENT 0x0E
#define QHCI_COMMAND_STATUS 0x0F

#define HCI_LE_CONN_CMPL_EVT 0x0A
#define HCI_LE_EXT_ADV_EVT 0x0D
#define HCI_BLE_LL_CONN_PARAM_UPD_EVT 0x03
#define HCI_LE_READ_REMOTE_FEAT_CMPL_EVT 0x04
#define HCI_LE_RC_PARAM_REQ_EVT 0x06
#define HCI_LE_DATA_LENGTH_CHANGE_EVT 0x07
#define HCI_LE_READ_REMOTE_VERSION_EVT 0x0C
#define HCI_LE_PHY_UPDATE_COMPLETE_EVT 0x0c
#define HCI_NUM_COMPL_DATA_PKTS_EVT 0x13
#define HCI_MAX_SLOTS_CHANGE_EVT 0x1b
#define HCI_LE_CHANNEL_SELECTION_ALGORITHM 0x14
#define HCI_READ_CLOCK_OFF_COMP_EVT 0x1C
#define HCI_CONN_PKT_TYPE_CHANGE_EVT 0x1D
#define HCI_LE_CIS_ESTABLISHED_EVT 0x19

#define QHCI_AUDIO_BEARER_REQ_SIZE 100
#define QHCI_AUDIO_BEARER_RSP_SIZE 100
#define QHCI_USECASE_UPDATE_CONFIRM_SIZE 100

#define QHCI_BT_SUCCESS 0
#define QHCI_SAP_ENABLE 1

#define QHCI_BT_MIN_VERSION_SUPPORT 13
#define QHCI_BT_MIN_LMP_VERSION_SUPPORT 0x521D
#define QHCI_BT_MANUFACTURE_SUPPORT_ID 0x001D

#define QHCI_VS_QLL_CMD_REQ_LEN 6
#define QHCI_VS_QBCE_READ_REMOTE_QLL_SUPPORTED_FEATURES 0x3

#define QHCI_PKT_MESSAGE_LENGTH 200

//Status codes for DISCONNECTION Complete Event
#define QHCI_REMOTE_USER_TERMINATE_CONN 0x13
#define QHCI_DISCONNECT_DUE_TO_LOCAL_HOST_TERM 0x16
#define QHCI_DISCONNECT_DUE_TO_CONN_TOUT 0x08

//Reason codes for Transport Enable
#define TRANSPORT_DISABLE_DUE_TO_WIFI_SSR 0x09

#define QHCI_CIS_COUNT_ONE 1
#define QHCI_CIS_COUNT_TWO 2

//HCI Command Status codes
#define QHCI_ERR_COMMAND_DISALLOWED 0x0C
#define QHCI_SUCCESS 0x00
#define QHCI_UNKNOWN_CONNECTION_IDENTIFIER 0x02

//HCI Commands length
#define QHCI_LEN_SET_DATA_LENGTH_EVT 8

#define QHCI_LE_CREATE_CONN_LEN 61

#define QHCI_XPAN_BEARER_TRANSITION_CMD 9
#define QHCI_LE_ENHANCED_CREATE_CONN_CMD_LEN 61

using ::xpan::implementation::QHciPacketizer;

typedef enum {
  QHCI_IDLE_STATE = 0,
  QHCI_BT_CLOSE_XPAN_CLOSE,
  QHCI_BT_CLOSE_XPAN_CONNECTING,
  QHCI_BT_CLOSE_XPAN_OPEN,
  QHCI_BT_OPEN_XPAN_CLOSE,
  QHCI_BT_OPEN_XPAN_CONNECTING,
  QHCI_BT_OPEN_XPAN_OPEN,
} QHCI_CSM_STATE;

typedef enum {
  QHCI_TRANSPORT_IDLE_STATE = 0,
  QHCI_BT_ENABLE = 6,
  QHCI_P2P_ENABLE,
  QHCI_P2P_ENABLE_BT_ENABLE,
  QHCI_BT_ENABLE_AP_CONNECTING,
  QHCI_AP_ENABLE,
  QHCI_AP_ENABLE_BT_CONNECTING,
  QHCI_BT_ENABLE_AP_ENABLE,
} QHCI_CTSM_STATE;

typedef enum {
  QHCI_CIS_CLOSE = 0,
  QHCI_CIS_CONNECTING,
  QHCI_CIS_OPEN,
  QHCI_CIS_DISCONNECTING,
} QHCI_CIS_STATE;

typedef enum {
  QHCI_TRANSPORT_NONE,
  QHCI_BT_LE_TRANSPORT,
  QHCI_AP_TRANSPORT,
} QHCI_LINK_TRANSPORT;

#define QHCI_TRANSTITION_COMPLETE 0x00
#define QHCI_TRANSTITION_STARTED 0x01
#define QHCI_TRANSTITION_FAILED 0x02
#define QHCI_TRANSTITION_INIT 0x03

#define QHCI_TRANSTITION_LE_AP 0x00
#define QHCI_TRANSTITION_AP_LE 0x01

#define BIT_VENDOR_XPAN_BEARER_CMD 0x01
#define BIT_SEND_CIS_DISCONNECT_TO_SOC 0x02
#define BIT_LE_CREATE_CONN_CMD_TO_SOC 0x03
#define BIT_LE_CREATE_CIS_QHCI_TO_SOC 0x04
#define BIT_LE_DISCONNECT_QHCI_TO_SOC 0x05

typedef enum {
  QHCI_CSM_LE_CONN_CMPL_EVT,
  QHCI_CSM_LE_CLOSE_EVT,
  QHCI_CSM_CIS_OPEN_XPAN_TRANS_DISABLE_EVT,
  QHCI_CSM_CIS_OPEN_XPAN_TRANS_ENABLE_EVT,
  QHCI_CSM_CIS_CLOSE_XPAN_TRANS_ENABLE_EVT,
  QHCI_CSM_XPAN_CONN_FAILED_EVT,
  QHCI_CSM_CIS_DISCONNECT_TX_EVT,
  QHCI_CSM_CIS_DISCONNECT_EVT,
  QHCI_CSM_USECASE_XPAN_TRANS_ENABLE_EVT,
  QHCI_CSM_USECASE_XPAN_TRANS_DISABLE_EVT,
  QHCI_CSM_UPDATE_TRANS_XPAN_EVT,
  QHCI_CSM_PREPARE_BEARER_BT,
  QHCI_CSM_PREPARE_BEARER_XPAN,
  QHCI_CSM_BEARER_SWITCH_FAILED,
  QHCI_CSM_BT_LE_CONNECT_EVT,
  QHCI_CSM_BT_LE_DISCONNECT_EVT,
  QHCI_CSM_P2P_PREPARE_BEARER_EVT,
  QHCI_CSM_P2P_PREPARE_BEARER_FAIL_EVT,
  QHCI_CSM_BT_PREPARE_BEARER_EVT,
  QHCI_CSM_BT_PREPARE_BEARER_FAIL_EVT,
  QHCI_CSM_P2P_TRANSPORT_ENABLE_EVT,
  QHCI_CSM_P2P_TRANSPORT_DISABLE_EVT,
  QHCI_CSM_BT_BEARER_SWITCH_IND_EVT,
  QHCI_CSM_BT_BEARER_SWITCH_IND_FAIL_EVT,
  QHCI_CSM_P2P_BEARER_SWITCH_IND_EVT,
  QHCI_CSM_P2P_BEARER_SWITCH_IND_FAIL_EVT,
  QHCI_CSM_AP_LE_CONNECT_EVT,
  QHCI_CSM_AP_LE_DISCONNECT_EVT,
  QHCI_CSM_BEARER_PREFERENCE_BT_EVT,
  QHCI_CSM_BEARER_PREFERENCE_AP_EVT,
  QHCI_CSM_AP_PREPARE_BEARER_EVT,
  QHCI_CSM_AP_PREPARE_BEARER_FAIL_EVT,
  QHCI_CSM_BT_CONNECT_REQ_EVT,
  QHCI_CSM_AP_BEARER_SWITCH_IND_EVT,
  QHCI_CSM_AP_BEARER_SWITCH_IND_FAIL_EVT,
  QHCI_CSM_CIS_ESTABLISH_EVT,
  QHCI_CSM_PREPARE_BEARER_XPAN_AP_REQ,
  QHCI_CSM_BEARER_SWITCH_IND_SUCCESS,
  QHCI_CSM_LE_TX_DISCONN_EVT,
} QHCI_CSM_EVENT;

#define GAME_USECASE 0x08
#define HD_USECASE 0x04
#define VOICE_USECASE 0x02

#define LE_UNICAST_HQ_PROFILE 0
#define LE_BROADCAST_HQ_PROFILE 1
#define LE_UNICAST_GAMING_PROFILE 2
#define XPAN_WIFI_HQ_PROFILE 3
#define XPAN_WIFI_GAMING_PROFILE 4
#define XPAN_WIFI_AP_HQ_PROFILE 5
#define XPAN_WIFI_AP_GAMING_PROFILE 6
#define XPAN_LE_VOICE_CALL_PROFILE 7
#define XPAN_AP_VOICE_CALL_PROFILE 8


#define BT_LE_PUBLIC_ADDR_TYPE 0
#define BT_LE_RANDOM_ADDR_TYPE 1

#define MAX_NO_OF_BONDED_DEVICES 3

typedef struct
{
  uint8_t cis_count;
  uint8_t cig_id;
  uint16_t cis_handles[2] = {0};
} qhci_cig_params_t;

typedef struct
{
  uint8_t num_devices;
  bdaddr_t bonded_devices[MAX_NO_OF_BONDED_DEVICES];
} qhci_xpan_bonded_list_t;

typedef struct
{
  uint8_t cig_sync_delay[3]; //0x002fc2
  uint8_t cis_sync_delay[3]; //0x002fc2
  uint8_t trans_latency_m_s[3]; //0x014132
  uint8_t trans_latency_s_m[3]; //0x0056d2
  uint8_t phy_m_s;
  uint8_t phy_s_m;
  uint8_t num_sub_events;
  uint8_t burst_num_m_s;
  uint8_t burst_num_s_m;
  uint8_t flush_timeout_m_s;
  uint8_t flush_timeout_s_m;
  uint8_t max_pdu_size_m_s[2];
  uint8_t max_pdu_size_s_m[2];
  uint8_t iso_interval[2];
} qhci_cis_establish_evt_data_t;

typedef struct
{
  bool qll_send_from_host = false;
  bool is_xpan_device = false;
  QHCI_CSM_STATE state;
  uint16_t handle;
  uint16_t active_cis_handle;
  bdaddr_t rem_addr;
  uint8_t addr_type;
  qhci_cig_params_t cig_params;
  QHCI_CTSM_STATE tState;
  QHCI_CIS_STATE cis_state;

  /* false - if create cis triggered from QHCI
   * true -  if create cis triggered from Stack */
  bool is_create_cis_from_stack;

  bool is_cis_established;

  /* false - XPAN Disabled means BT Transport
   * true - XPAN Enabled*/
  bool transport_enable;
  /* 0 - Default BT Transport
   * 1 -  XPAN Transport */
  uint8_t update_transport;

  TransportType current_transport;

  /* false - is prepare bearer not triggered from QHCI
   * true -  prepare bearer request triggered from QHCI */
  bool is_prepare_bearer_triggered;

  /* false - is create_cis not triggered from QHCI to SOC
   * true -  create_cis triggered from QHCI tto SOC*/
  bool is_create_cis_from_qhci_to_soc;

  bool is_xpan_usecase;

  qhci_cis_establish_evt_data_t qhci_cis_establish_evt_data;

  bool is_cis_disconnect_pending_from_soc;

  bool is_wifi_ssr_enabled;
  bool qhci_block_cis_disconnect_evt;

  bool le_acl_connected;

  TransportType qhci_link_transport;

  uint64_t qhci_hci_cmd_wait;
} qhci_dev_cb_t;

namespace xpan {
namespace implementation {
using ::android::hardware::hidl_vec;


enum QHciTimerState {
  QHCI_TIMER_NOT_CREATED = 0x00,
  QHCI_TIMER_CREATED,
  QHCI_TIMER_ACTIVE,
  QHCI_TIMER_CLOSE,
};

typedef struct {
  QHciTimerState timer_state;
  timer_t timer_id;
} QHciCmdTimer;

typedef struct {
  uint8_t version;
  uint16_t companyId;
  uint16_t subversion;
} QHciLocalDevVer;

class QHci {
  public:
    QHci();
    ~QHci();
    int Init(void);
    static std::shared_ptr<QHci> Get(void);
    int DeInit(void);
    bool IsQhciRxPkt(const hidl_vec<uint8_t>*hidl_data);
    void ProcessRxPktEvent(HciPacketType type,
                                    const hidl_vec < uint8_t > * hidl_data);
    void ProcessTxPktCmd(const uint8_t *data, size_t length);
    uint8_t IsQhciTxPkt(const uint8_t *data, size_t length);
    void PostMessage(qhci_msg_t *msg);
    UseCaseType getCurrentUsecaseType();
    qhci_cig_params_t getCigParams();
    bool isXpanSupported();
    bool isCisHandleDiscPending();
    void setCisHandleDiscPending(bool value);
    bool IsQHciApTransportEnable(uint16_t handle);
    void ProcessTxAclData(const uint8_t* data, size_t length);
    bool isStreamingActive(bdaddr_t bd_addr);
    uint8_t* UpdateTxPktHandle(HciPacketType type, uint8_t *data, size_t length);
    void UpdateRxPktHandle(HciPacketType type, uint8_t *data);
    QHCI_CTSM_STATE GetQhciTransportState(uint16_t handle);
    TransportType GetQHciActiveLinkTransport(uint16_t handle);

  private:
    void QHciMainThreadRoutine();
    static void curr_usr_handler(int);
    void QHciMsgHandler(qhci_msg_t *msg);
    void SendRxPktToHost(qhci_msg_t *msg);
    void QHciHandleIdleState(qhci_dev_cb_t *rem_info, uint8_t event);
    void QHciHandleBtCloseXpanClose(qhci_dev_cb_t *rem_info, uint8_t event);
    void QHciHandleBtCloseXpanConnecting(qhci_dev_cb_t *rem_info,
                                                      uint8_t event);
    void QHciHandleBtCloseXpanOpen(qhci_dev_cb_t *rem_info,
                                              uint8_t event);
    void QHciHandleBtOpenXpanClose(qhci_dev_cb_t *rem_info,
                                             uint8_t event);
    void QHciHandleBtOpenXpanConnecting(qhci_dev_cb_t *rem_info,
                                                    uint8_t event);
    void QHciHandleBtOpenXpanOpen(qhci_dev_cb_t *rem_info, uint8_t event);

    void QHciSmExecute(qhci_dev_cb_t *rem_info, uint8_t event);
    qhci_dev_cb_t* GetQHciRemoteDeviceInfo(uint16_t handle);
    void SendCreateCisToSoc(qhci_dev_cb_t *rem_info);
    QHCI_CSM_STATE GetQHciState(qhci_dev_cb_t *rem_info);
    void QHciSendCisEstablishedEvt(qhci_dev_cb_t *rem_info, uint8_t status,
                                             uint8_t handle_num);
    void QHciLeConnCleanup(qhci_dev_cb_t *rem_info);
    bool IsQHciSupportLmpVersion (uint16_t subversion);
    bool IsQHciSupportVersion (uint8_t version);
    bool IsQHciSupportManuFacture (uint16_t man_facture_id);
    void QHciProcessRxPktEvt(qhci_msg_t *msg);
    void QHciParseUsecaseUpdateCmd(qhci_msg_t *msg);
    void QHciSetHostSupportedBit();
    void QHciStartCommandTimer();
    QHciTimerState QHciGetCmdTimerState();
    void QHciSetCmdTimerState(QHciTimerState timer_state);
    static void QHciCmdTimeOut(union sigval sig);
    void QHciStopCmdTimer();
    bool IsQHciXpanSupportedDevice(uint16_t handle);
    void QHciSendUsecaseUpdateCfm(uint8_t usecase);
    void QHciProcessQllReq(uint16_t handle);
    void QHciPrepareAndSendHciDisconnect(uint16_t handle);
    void QHciSendDisconnectCisToSoc(qhci_dev_cb_t *rem_info);
    uint16_t QHciGetMappingAclHandle(uint16_t cis_handle);
    void QHciDelayReportingEvt(qhci_msg_t *msg);
    void GetMainThreadState(void);
    bool QHciCmpBDAddrs(bdaddr_t bd_addr1, bdaddr_t bd_addr2);
    uint16_t QHciBDAddrToHandleMap(bdaddr_t bd_addr);
    void QHciSendCmdStatusForCis();
    void QHciProcessConnCmplEvt(qhci_msg_t *msg);
    void QHciSendDisconnectCmplt(uint16_t handle, uint8_t reason);
    void QHciSendDisconnectCmdStatus();
    void QHciUseCaseUpdateEvt(uint8_t usecase);
    void QHciSendVndrQllEvtMask();
    void QHciClearRemoteDeviceInfo(uint16_t handle);
    void QHciUnprepareAudioBearerRspfromXm(bdaddr_t bd_addr, bool status);
    char* ConvertEventToString(uint8_t eventId);
    bool isXpanTransportEnabled();
    QHCI_CSM_STATE GetQHciStateByHandle(uint16_t cis_handle);
    void PostQHciStateChange(uint16_t handle,
                             QHCI_CSM_STATE state,
                             QHCI_CSM_EVENT qhci_event);
    void PostLocalVersionFeatureToAc();
    void QHciSendCmdCmpltForEncoderLimit();
    bool isXpanStreamActive();
    bool isRemoteXpanTransportEnabled(uint16_t handle);
    void QHciHandleTransportIdleState(qhci_dev_cb_t *rem_info,
                                                 uint8_t event);
    void QHciHandleBtEnable(qhci_dev_cb_t *rem_info, uint8_t event);
    void QHciHandleP2PEnable(qhci_dev_cb_t *rem_info, uint8_t event);
    void QHciHandleP2PEnableBtEnable(qhci_dev_cb_t *rem_info,
                                                  uint8_t event);
    void QHciHandleApEnable(qhci_dev_cb_t *rem_info, uint8_t event);
    void QHciHandleApEnableBtConnecting(qhci_dev_cb_t *rem_info,
                                                    uint8_t event);
    void QHciHandleBtEnableApConnecting(qhci_dev_cb_t *rem_info,
                                                    uint8_t event);
    void QHciHandleBtEnableApEnable(qhci_dev_cb_t *rem_info,
                                                  uint8_t event);
    bool isQHciXpanSupportedAddress(bdaddr_t bd_addr);
    void QHciSendLocalVerAndLeFeatToAC();
    void QHciSendNocpEvent(uint16_t handle,
                                    uint8_t no_of_pkts_sent);
    void QHciSendLeSetDataLengthStatus(uint16_t handle);
    void QHciSendCmdStatusToStack(uint16_t opcode);
    void QHciSetupIsoDataCmdCmplEvt(uint8_t status, uint16_t handle);
    void QHciLeCancelCmdCmplEvt(uint8_t status);
    void QHciPostMessageToAc(const uint8_t* data, size_t length);
    void QHciSendEncryCmplEvent(uint16_t handle, uint8_t status,
                                      uint8_t encryptionEnabled);
    void QHciSendRemoteVersionCmplEvt(uint16_t handle,
                                                 uint8_t version,
                                                 uint16_t companyId,
                                                 uint16_t subversion);
    void QHciSendLeRemoteFeatureEvent(uint16_t handle, uint8_t status,
                                                 uint64_t feature_mask);
    void QHciSendCreateConnectionToAc(bdaddr_t bd_addr,
                                            uint16_t supervision_tout);
    void QHciLeConnCmplEvent(uint16_t handle, bdaddr_t bd_addr,
                                       uint8_t status);
    void QHciSendDisconnectToAc(uint16_t handle);
    void QHciSendLeConnUpdateEvt(uint16_t handle, uint16_t conn_interval);
    void QHciSendAcRxDataToStack(uint16_t handle, uint8_t* rx_data, uint16_t);
    void QHciLeCreateConnectToSoc(bdaddr_t bd_addr);
    void QHciTransportSmExecute(qhci_dev_cb_t *rem_info, uint8_t event);
    void QHciSendCancelConnectionToAc(bdaddr_t bd_addr);

    //API related to Setting/Getting the CIS state of remote
    void QHciSetCisState(qhci_dev_cb_t *rem_info, QHCI_CIS_STATE state);
    QHCI_CIS_STATE QHciGetCisState(qhci_dev_cb_t *rem_info);

    //APIs are related Parsing the LE commands in QHciTx
    uint8_t QHciParseCreateCisCmd(const uint8_t *data, size_t length);
    void QHciProcessCreateCisCmd(const uint8_t *data, size_t length);
    //void QHciLeExtendedCreateConnToSoc(bdaddr_t bd_addr);

    //utility Apis to printing the states/events
    char * ConvertMsgtoString(uint8_t);
    char* ConvertCisStatetoString(QHCI_CIS_STATE state);
    char* ConvertStatetoString(uint8_t state);
    char* ConvertIpcEventToString(QHciEventId eventId);

    void QHciXPanBearerTransitionCmdToSoc(uint8_t transition_state,
                                            uint16_t handle,
                                            uint16_t transition_type);

    std::atomic_bool main_thread_running;
    static std::shared_ptr<QHci> qhci_main_instance;
    //Logger *logger_;
    std::thread qhci_main_thread;
    QHciPacketizer qhci_packetizer;
    std::mutex qhci_wq_mtx;
    std::queue <qhci_msg_t *> qhci_workqueue;
    std::condition_variable qhci_wq_notifier;
    std::atomic_bool is_main_thread_busy{false};
    std::vector <qhci_dev_cb_t> qhci_xpan_dev_db;
    qhci_cig_params_t cig_params;
    bool qhci_wait_for_qll_event;
    bool qhci_set_host_bit;
    QHciCmdTimer qhci_cmd_timer_ = {QHCI_TIMER_NOT_CREATED, 0};
    std::map<uint16_t, bdaddr_t> xpan_active_devices_;
    std::map<uint16_t, bdaddr_t> handle_bdaddr_map_;
    bool qhci_wait_for_cmd_status_from_soc;
    bool is_xpan_supported;
    bool qhci_progress_cis_cmd;
    uint8_t qhci_remote_version_data[100];
    static std::mutex cmd_timer_mutex_;
    bool qhci_qll_req_sent;
    bool is_cis_handle_disc_pending;
    std::map<uint16_t, uint16_t> cis_acl_handle_map;
    std::vector <uint16_t> active_cis_handles;
    bool prep_bearer_active;
    bdaddr_t curr_bd_addr;
    bdaddr_t dbg_curr_bd_addr[2];
    bdaddr_t pending_bd_addr_cis;
    int dbg_cnt_bd_addr;
    qhci_dev_cb_t curr_active_xpan_dev;
    bool is_cis_conn_prog;
    bool encrypt_event_pending;
    bool dbg_mtp_mtp_prop;
    bool dbg_mtp_mora_prop;
    bool dbg_mora_nut_prop;
    UseCaseType usecase_type_to_xm;
    bool qhci_bearer_switch_pending;
    int pending_cis_est_evt;
    bdaddr_t active_xpan_device;
    UseCaseType usecase_type_on_transport;
    bool qhci_cis_disconnect_cmd_wait;
    bdaddr_t qhci_whitelist_address;
    bool qhci_ac_le_acl_prog;
    uint64_t qhci_le_local_support_feature;
    QHciLocalDevVer qhci_local_version;
    TransportType qhci_curr_transport;
    qhci_xpan_bonded_list_t qhci_xpan_bonded_list;
    uint16_t qhci_ac_active_handle;
    bdaddr_t curr_active_bd_addr;
    bdaddr_t debug_lib_bd_addr;
    bool dbg_mtp_lib_prop;
    bool qhci_ap_soc_cmd_pending;
    bool dbg_direct_ap_prop;
    bool qhci_ap_connect;
    uint64_t qhci_hci_cmd_wait;
};

} // namespace implementation
} // namespace xpan
#endif

