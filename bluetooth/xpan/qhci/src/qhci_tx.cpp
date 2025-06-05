/* 
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "data_handler.h"
#include "qhci_main.h"
#include "qhci_packetizer.h"
#include "qhci_xm_if.h"
#include "qhci_hm.h"
#include <hidl/HidlSupport.h>
#include "hci_transport.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.qhci@1.0-xpan_qhci_tx"
using android::hardware::bluetooth::V1_0::implementation::DataHandler;
using xpan::implementation::QHciHm;

namespace xpan {
namespace implementation {

/******************************************************************
 *
 * Function       QHciParseCreateCisCmd
 *
 * Description    prasing the hci cis cmd pkt and checks whether it
 *                QHci module interaction or not?
 *
 *
 * Arguments      *data- contains the tx packet
 *                length - size of the packet
 *
 * return         uint8_t
 ******************************************************************/
uint8_t QHci::QHciParseCreateCisCmd(const uint8_t *data, size_t length) {
  ALOGD("%s HCI_LE_CREATE_CIS ", __func__);

  uint8_t status = 0;

  if (is_cis_conn_prog) {
    ALOGD("%s ALready CIS IS GOING ON IGNORE HCI_LE_CREATE_CIS ", __func__);
    return 0;
  }

  uint16_t acl_handle = 0;
  acl_handle = (data[7] << 8 | data[6]);
  bool is_xpan_supported_dev = IsQHciXpanSupportedDevice(acl_handle);

  if (!is_xpan_supported_dev) {
    if (qhci_curr_transport == BT_LE) {
      uint16_t handle = QHciHm::GetIf()->GetStackHandleFromLeHandle(acl_handle);
      is_xpan_supported_dev = IsQHciXpanSupportedDevice(handle);
      ALOGD("%s LE Handle %d Stack Handle %d is_xpan_supported_dev %d", __func__,
          acl_handle, handle, is_xpan_supported_dev);
    }
  }

  if (is_xpan_supported_dev) {
    ALOGD("%s HCI_LE_CREATE_CIS  CIS Count %d ", __func__, data[3]);
    is_cis_conn_prog = true;
    // for one cis
    if (data[3] == QHCI_CIS_COUNT_ONE) {
      cig_params.cis_handles[0] = (data[5] << 8 | data[4]);
      status = 1;
      qhci_progress_cis_cmd = true;
    } else if (data[3] == QHCI_CIS_COUNT_TWO) {
      cig_params.cis_handles[0] = (data[5] << 8 | data[4]);
      cig_params.cis_handles[1] = (data[9] << 8 | data[8]);
      cig_params.cis_count = 2;
      ALOGD("%s HCI_LE_CREATE_CIS  ACL HANDLE %d CIS Handles %d %d",
              __func__, acl_handle, cig_params.cis_handles[0],
              cig_params.cis_handles[1]);
      status = 1;
      qhci_progress_cis_cmd = true;
    }
  } else {
    ALOGD("%s XPAN Not supported for ACL %d, so discard CIG Params",
          __func__, acl_handle);
    cig_params = {};
  }

  return status;
}

/******************************************************************
 *
 * Function       QHciProcessCreateCisCmd
 *
 * Description    prasing the hci cis cmd pkt and checks whether it
 *                QHci module interaction or not?
 *
 *
 * Arguments      *data- contains the tx packet
 *                length - size of the packet
 *
 * return         void
 ******************************************************************/
void QHci::QHciProcessCreateCisCmd(const uint8_t *data, size_t length) {
  ALOGD("%s ", __func__);

  if (qhci_progress_cis_cmd) {
    qhci_progress_cis_cmd = false;
    uint16_t acl_handle = 0;
    uint8_t cis_count = data[3];
    uint16_t cis_handle[2] = {0};

    if (cis_count == 1) {
      acl_handle = (data[7] << 8 | data[6]);
      cis_handle[0] = (data[5] << 8 | data[4]);
      cis_acl_handle_map.insert({cis_handle[0], acl_handle});
    } else {
      cis_handle[0] = (data[5] << 8 | data[4]);
      acl_handle = (data[7] << 8 | data[6]);
      cis_handle[1] = (data[9] << 8 | data[8]);
      cis_acl_handle_map.insert({cis_handle[0], acl_handle});
      cis_acl_handle_map.insert({cis_handle[1], acl_handle});
    }

    ALOGD("%s HCI_LE_CREATE_CIS CIS count %d ACL HANDLE %d", __func__, 
           cis_count, acl_handle);
  
    qhci_dev_cb_t *rem_qhci_dev = GetQHciRemoteDeviceInfo(acl_handle);
    if (rem_qhci_dev) {
      QHCI_CSM_STATE qhci_state = GetQHciState(rem_qhci_dev);
      ALOGD("%s HCI_LE_CREATE_CIS  GetQHciState %s transport_enable %d "
             "usecase %d", __func__, ConvertMsgtoString(qhci_state),
             rem_qhci_dev->transport_enable, usecase_type_to_xm);
      if (dbg_mtp_mora_prop) {
        bool dbg_st = true;
        for (int i = 0; i < 6; i++) {
          if (curr_bd_addr.b[i] != dbg_curr_bd_addr[1].b[i]) {
            dbg_st = false;
            break;
          }
        }
        if (dbg_st) {
          ALOGD("%s HCI_LE_CREATE_CIS ON Mora", __func__);
          rem_qhci_dev->transport_enable = true;
        }
      }

      QHciSetCisState(rem_qhci_dev, QHCI_CIS_CONNECTING);

      if (dbg_mtp_lib_prop) {
        rem_qhci_dev->tState = QHCI_AP_ENABLE;
        ALOGE("%s PROP QHCI_STATE: %s", __func__,
                  ConvertStatetoString(rem_qhci_dev->tState));
        rem_qhci_dev->active_cis_handle = (data[5] << 8 | data[4]);
        rem_qhci_dev->is_create_cis_from_stack = true;
        rem_qhci_dev->current_transport = XPAN_AP;
        QHciTransportSmExecute(rem_qhci_dev, QHCI_CSM_PREPARE_BEARER_XPAN);

        DataHandler *data_handler = DataHandler::Get();

        if (data_handler) {
          if (data_handler->GetControllerRef() != nullptr) {
            ALOGE("%s Sending cis req to the soc", __func__);
            data_handler->GetControllerRef()->SendPacket(HCI_PACKET_TYPE_COMMAND,
                                                 data, length);
          }
        }
      } else if (IsQHciApTransportEnable(acl_handle)) {
        ALOGE("%s QHCI_STATE: %s", __func__,
                  ConvertStatetoString(rem_qhci_dev->tState));
        rem_qhci_dev->active_cis_handle = (data[5] << 8 | data[4]);
        rem_qhci_dev->is_create_cis_from_stack = true;
        rem_qhci_dev->current_transport = XPAN_AP;
        QHciSendCmdStatusForCis();
        QHciTransportSmExecute(rem_qhci_dev, QHCI_CSM_PREPARE_BEARER_XPAN);
      } else if ((rem_qhci_dev->transport_enable) &&
        (usecase_type_to_xm == USECASE_XPAN_LOSSLESS)) {
        QHciSendCmdStatusForCis();
        rem_qhci_dev->active_cis_handle = (data[5] << 8 | data[4]);
        rem_qhci_dev->is_create_cis_from_stack = true;
        rem_qhci_dev->current_transport = XPAN_P2P;
        if (qhci_state == QHCI_BT_CLOSE_XPAN_CLOSE)
          QHciSmExecute(rem_qhci_dev, QHCI_CSM_CIS_CLOSE_XPAN_TRANS_ENABLE_EVT);
        else
          ALOGE("%s qhci_state wrong state, state %s", __func__,
                  ConvertMsgtoString(qhci_state));
      } else {
        rem_qhci_dev->active_cis_handle = (data[5] << 8 | data[4]);
        rem_qhci_dev->is_create_cis_from_stack = true;
        rem_qhci_dev->current_transport = BT_LE;
        DataHandler *data_handler = DataHandler::Get();
  
        if (data_handler) {
          if (data_handler->GetControllerRef() != nullptr) {
            ALOGE("%s Sending cis req to the soc", __func__);
            data_handler->GetControllerRef()->SendPacket(HCI_PACKET_TYPE_COMMAND,
                                                 data, length);
          }
        }
        if (qhci_state == QHCI_BT_CLOSE_XPAN_CLOSE)
          QHciSmExecute(rem_qhci_dev, QHCI_CSM_CIS_OPEN_XPAN_TRANS_DISABLE_EVT);
        else
          ALOGE("%s qhci_state wrong state, state %s", __func__,
                  ConvertMsgtoString(qhci_state));
      }
    } else {
  
      ALOGE("%s QHCI Is in BAD STATE. NO REMINFO for CIS", __func__);
      DataHandler *data_handler = DataHandler::Get();
  
      if (data_handler) {
        if (data_handler->GetControllerRef() != nullptr) {
          ALOGE("%s Sending cis req to the soc", __func__);
          data_handler->GetControllerRef()->SendPacket(HCI_PACKET_TYPE_COMMAND,
                                                 data, length);
        }
      }
    }
  }else {
    DataHandler *data_handler = DataHandler::Get();
  
    if (data_handler) {
      if (data_handler->GetControllerRef() != nullptr) {
        data_handler->UpdateRingBufferFromQHci(HCI_PACKET_TYPE_COMMAND,
                                               data, length);
        ALOGE("%s Sending data to the soc", __func__);
        data_handler->GetControllerRef()->SendPacket(HCI_PACKET_TYPE_COMMAND,
                                                 data, length);
      }
    }
  }

}

/******************************************************************
 *
 * Function       IsQhciTxPkt
 *
 * Description    prasing the hci TX pkt and checks whether it needs
 *                QHci module interaction or not?
 *
 *
 * Arguments      *data- contains the tx packet
 *                length - size of the packet
 *
 * return         uint8_t
 ******************************************************************/
uint8_t QHci::IsQhciTxPkt(const uint8_t *data, size_t length) {
  uint16_t opcode = (data[1] << 8 | data[0]);

  uint8_t status = 0;

  //TODO check the state is AP, then send Command Complete to the stack

  switch (opcode) {
    case HCI_VENDOR_USECASE_UPDATE:
      if (data[3] == VSC_QHCI_VENDOR_OPCODE) {
        status = 1;
      } else {
        status = 0;
      }
      break;
    case HCI_LE_SET_CIG_PARAMETERS:
      qhci_wait_for_cmd_status_from_soc = true;
      break;
    case HCI_LE_READ_LOCAL_SUPP_FEATURES:
      qhci_wait_for_cmd_status_from_soc = true;
      break;
    case HCI_READ_LOCAL_VERSION_INFORM:
      qhci_wait_for_cmd_status_from_soc = true;
      break;
    case HCI_LE_SETUP_ISO_DATA_PATH:
      {
        uint16_t cis_handle = (data[4] << 8 | data[3]);
        uint16_t acl_handle = QHciGetMappingAclHandle(cis_handle);
        ALOGD("%s HCI_LE_SETUP_ISO_DATA_PATH CIS Handle %d", __func__,
                  cis_handle, acl_handle);
        if (IsQHciApTransportEnable(acl_handle)) {
        // TODO This Needs to Enhance it during seamless AP -> LE
        // Need to cache the values and send it soc if any issue
        // seen during to streaming
          status = 1;
          QHciSetupIsoDataCmdCmplEvt(QHCI_SUCCESS, cis_handle);
        } else {
          status = 0;
        }
      }
      break;
    case HCI_LE_CREATE_CIS:
      {
        status = QHciParseCreateCisCmd(data, length);
      }
      break;
    case HCI_DISCONNECT:
      {
      uint16_t handle = (data[4] << 8 | data[3]);
      qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);

      ALOGD("%s HCI_DISCONNECT Handle %d", __func__, handle);

      if (rem_info) {
        ALOGD("%s HCI_DISCONNECT for rem handle %d and QHCI_STATE: %s",
               __func__, rem_info->handle, ConvertStatetoString(rem_info->tState));
        if (rem_info->tState == QHCI_BT_ENABLE_AP_ENABLE) {
          status = 2;
          QHciTransportSmExecute(rem_info, QHCI_CSM_LE_TX_DISCONN_EVT);
          return status;
        } else if (rem_info->tState == QHCI_AP_ENABLE) {
          status = 2;
          QHciSendDisconnectToAc(handle);
          QHciSendDisconnectCmdStatus();
          return status;
        }
      }

      if (handle == qhci_ac_active_handle) {
        ALOGD("%s HCI_DISCONNECT for WHC Handle %d", __func__, handle);
        status = 2;
        QHciSendDisconnectToAc(handle);
        QHciSendDisconnectCmdStatus();
        return status;
      }

      if (prep_bearer_active) {
         if ((cig_params.cis_handles[0] == handle) ||
              (cig_params.cis_handles[1] == handle)) {
            if (dbg_mtp_lib_prop) {
              uint16_t acl_handle = QHciGetMappingAclHandle(handle);
              qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(acl_handle);
              status = 0;
              ALOGD("%s CIS DISCONNECT for Hack code", __func__);
              if (rem_info)
                QHciTransportSmExecute(rem_info, QHCI_CSM_CIS_DISCONNECT_EVT);
            } else {
              status = 1;
              QHciSendDisconnectCmdStatus();
            }
         }
      } else {
        if ((cig_params.cis_handles[0] == handle) ||
            (cig_params.cis_handles[1] == handle)) {
          uint16_t acl_handle = QHciGetMappingAclHandle(handle);
          qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(acl_handle);

          if (rem_info) {
            ALOGD("%s HCI_DISCONNECT  for CIS_HANDLE: %d state %s", __func__,
                    handle, ConvertMsgtoString(rem_info->state));
            if ((rem_info->state == QHCI_BT_OPEN_XPAN_CONNECTING)
                || (rem_info->state == QHCI_BT_CLOSE_XPAN_OPEN)) {
              status = 1;
              QHciSendDisconnectCmdStatus();
              return status;
            } else {
              PostQHciStateChange(handle, rem_info->state,
                                QHCI_CSM_CIS_DISCONNECT_EVT);
            }

          }
        }
      }
        is_cis_conn_prog = false;
      }
      break;
    case HCI_LE_REMOVE_CIG:
      ALOGD("%s HCI_LE_REMOVE_CIG ", __func__);
      cis_acl_handle_map.clear();
      status = 0;
      break;
    case HCI_VENDOR_ENCODER_LIMIT_CMD:
      if (data[3] == HCI_SUB_OPCODE_ENCODER_LIMIT_CMD) {
        ALOGW("isXpanStreamActive %d ", isXpanStreamActive());
        if ((cig_params.cig_id == data[4]) && (isXpanStreamActive())) {
          uint16_t  handle = QHciBDAddrToHandleMap(active_xpan_device);
          qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);
          QHCI_CSM_STATE qhci_state = GetQHciState(rem_info);

          if (rem_info) {
            ALOGW(" HCI_VENDOR_ENCODER_LIMIT_CMD Qhci_state %s ",
                ConvertMsgtoString(qhci_state));
            if ((handle != 0) && ((qhci_state == QHCI_BT_CLOSE_XPAN_OPEN)
                  || (qhci_state == QHCI_BT_OPEN_XPAN_OPEN))) {
              status = 1;
            } else
              status = 0;
          } else
            status = 0;
        } else {
          status = 0;
        }
      } else {
        status = 0;
      }
      break;
    case HCI_LE_ADD_DEVICE_TO_WHITE_LIST:
      {
        //parsing BD addr
        for (int i = 0; i < 6; i++) {
          qhci_whitelist_address.b[i] = data[4+i];
        }
        ALOGD ("%s HCI_LE_ADD_DEVICE_TO_WHITE_LIST bdaddr %s",__func__,
                ConvertRawBdaddress(qhci_whitelist_address));
        status = 0;
      }
      break;
    case HCI_LE_EXT_CREATE_CONN:
      {
        char value_prop[PROPERTY_VALUE_MAX] = {'\0'};
        property_get("persist.vendor.service.bt.qhci_ap_connect", value_prop , "false");
        if (strcmp(value_prop, "true") == 0) {
          qhci_ap_connect = true;
        } else {
          qhci_ap_connect = false;
        }
        ALOGD("%s: qhci_ap_connect %d", __func__, qhci_ap_connect);
        ALOGD ("%s HCI_LE_EXT_CREATE_CONN ", __func__);
        //check the qhci_whitelist_address is in XPAN bonded List?
        //if (isQHciXpanSupportedAddress(qhci_whitelist_address)) {
        if (qhci_ap_connect ||
            isQHciXpanSupportedAddress(qhci_whitelist_address)) {
          ALOGD ("%s HCI_LE_EXT_CREATE_CONN triggered to both AC and SOC",
                  __func__);
          //TODO send the connect request to AC
          qhci_ac_le_acl_prog = true;
          //TODO Remove this during for LE to Ap transition testing
          status = 2;
          QHciSendCreateConnectionToAc(qhci_whitelist_address,
                                      (data[24] << 8 | data[23]));
          QHciSendCmdStatusToStack(opcode);
        } else {
          status = 0;
        }
      }
      break;
    case HCI_LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST:
      {
        ALOGD ("%s HCI_LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST ",__func__);
        //qhci_whitelist_address ={};
        //qhci_ac_le_acl_prog -false;
        status = 0;
      }
      break;
    case HCI_LE_READ_REMOTE_FEAT_CMD:
      {
        ALOGD ("%s HCI_LE_READ_REMOTE_FEAT_CMD ",__func__);

        uint16_t acl_handle = (data[4] << 8 | data[3]);
        if (IsQHciApTransportEnable(acl_handle)) {
          status = 1;
          QHciSendCmdStatusToStack(opcode);
        } else {
          status = 0;
        }
      }
      break;
    case HCI_READ_REMOTE_VERSION_CMD:
      {
        ALOGD ("%s HCI_READ_REMOTE_VERSION_CMD ",__func__);

        uint16_t acl_handle = (data[4] << 8 | data[3]);
        if (IsQHciApTransportEnable(acl_handle)) {
          status = 1;
          QHciSendCmdStatusToStack(opcode);
        } else {
          status = 0;
        }
      }
      break;
    case HCI_LE_ENABLE_ENCRYPTION_CMD:
      {
        ALOGD ("%s HCI_LE_ENABLE_ENCRYPTION_CMD ",__func__);

        uint16_t acl_handle = (data[4] << 8 | data[3]);
        if (IsQHciApTransportEnable(acl_handle)) {
          status = 1;
          QHciSendCmdStatusToStack(opcode);
        } else {
          status = 0;
        }
      }
      break;
    case HCI_LE_CONN_UPDATE_CMD:
      {
        ALOGD ("%s HCI_LE_CONN_UPDATE_CMD ",__func__);

        uint16_t acl_handle = (data[4] << 8 | data[3]);
        if (IsQHciApTransportEnable(acl_handle)) {
          status = 2;
          QHciSendCmdStatusToStack(opcode);
          uint16_t conn_interval = (data[6] << 8 | data[5]);
          QHciSendLeConnUpdateEvt(acl_handle, conn_interval);
        } else {
          status = 0;
        }
      }
      break;
    case HCI_LE_CREATE_CANCEL_CONN_CMD:
      {
        ALOGD ("%s HCI_LE_CREATE_CANCEL_CONN_CMD AC Connection Progrees %d",
                __func__, qhci_ac_le_acl_prog);
        if (qhci_ac_le_acl_prog) {
          qhci_ac_le_acl_prog = false;
          status = 2;
          QHciSendCancelConnectionToAc(qhci_whitelist_address);
          QHciLeCancelCmdCmplEvt(QHCI_SUCCESS);
        }
      }
      break;
    default:
      status = 0;
  }

  return status;

}

/******************************************************************
 *
 * Function       ProcessTxPktCmd
 *
 * Description    Processing the TX pkt which are needed in qhci
 *                main thread.
 *
 *
 * Arguments      *data- contains the tx packet
 *                length - size of the packet
 *
 * return         none
 ******************************************************************/
void QHci::ProcessTxPktCmd(const uint8_t *data, size_t length) {

  ALOGD("%s length %d", __func__, length);
  uint16_t opcode = (data[1] << 8 | data[0]);

  if (opcode == HCI_VENDOR_USECASE_UPDATE) {
    if (data[3] == VSC_QHCI_VENDOR_OPCODE) {
      ALOGD("%s Usecase Update length %d", __func__, length);
      qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH);
      msg->eventId = QHCI_USECASE_UPDATE_CMD;
      msg->TxUsecaseRcvd.eventId = QHCI_USECASE_UPDATE_CMD;
      msg->TxUsecaseRcvd.length = data[2];
      msg->TxUsecaseRcvd.opcode = data[3];
      msg->TxUsecaseRcvd.acl_handle = (data[5] << 8 | data[4]);
      msg->TxUsecaseRcvd.context_type = (UseCaseType) data[6];
      PostMessage(msg);
      return;
    }
  }

  if (opcode == HCI_VENDOR_ENCODER_LIMIT_CMD) {
    if (data[3] == HCI_SUB_OPCODE_ENCODER_LIMIT_CMD) {
      ALOGD("%s HCI_VENDOR_ENCODER_LIMIT_CMD %d CiG id = %d "
          "Cis Id = %d Num limit = %d", __func__, length, data[4], data[5],
              data[6]);
      qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH);
      msg->eventId = QHCI_VENDOR_ENCODER_LIMIT_CMD;
      msg->TxUsecaseRcvd.eventId = QHCI_VENDOR_ENCODER_LIMIT_CMD;

      msg->EncoderLimitCmd.num_limit = data[6];
      uint8_t num_limit = data[6];
      uint8_t *encoder_data = (uint8_t *) malloc (num_limit * 3);

      memcpy(encoder_data, &data[7], num_limit * 3);
      msg->EncoderLimitCmd.data = encoder_data;
      ALOGD("Encoder Data %d %d %d ", msg->EncoderLimitCmd.data[0],
          msg->EncoderLimitCmd.data[1], msg->EncoderLimitCmd.data[2]);
      PostMessage(msg);
      QHciSendCmdCmpltForEncoderLimit();
      return;
    }
  }

  if (opcode == HCI_DISCONNECT) {
    uint16_t handle = (data[4] << 8 | data[3]);
    ALOGD("%s HCI_DISCONNECT %d", __func__, handle);
    if ((cig_params.cis_handles[0] == handle) ||
         (cig_params.cis_handles[1] == handle)) {
      ALOGD("%s CIS_DISCONNECT %d", __func__, handle);

      uint16_t acl_handle = QHciGetMappingAclHandle(handle);
      qhci_dev_cb_t *rem_qhci_dev = GetQHciRemoteDeviceInfo(acl_handle);
      cig_params.cis_count--;
      if (rem_qhci_dev) {
        ALOGD("%s QHCI State %s QHCI Transport State %s", __func__,
          ConvertMsgtoString(rem_qhci_dev->state),
          ConvertStatetoString(rem_qhci_dev->tState));
        if (IsQHciApTransportEnable(acl_handle)) {
          cis_acl_handle_map.erase(handle);
        } else {
          if (rem_qhci_dev->state != QHCI_BT_OPEN_XPAN_CLOSE) {
            cis_acl_handle_map.erase(handle);
          }
        }
      }
      if (cig_params.cis_count <= 0) {
        if (rem_qhci_dev) {
          if(IsQHciApTransportEnable(acl_handle)) {
            QHciTransportSmExecute(rem_qhci_dev, QHCI_CSM_CIS_DISCONNECT_EVT);
          } else {
            if ((rem_qhci_dev->state == QHCI_BT_OPEN_XPAN_CLOSE) ||
                (rem_qhci_dev->state == QHCI_BT_OPEN_XPAN_CONNECTING))
              QHciSmExecute(rem_qhci_dev, QHCI_CSM_CIS_DISCONNECT_TX_EVT);
            else
              QHciSmExecute(rem_qhci_dev, QHCI_CSM_CIS_DISCONNECT_EVT);
          }
        } else {
          ALOGD("%s QHCI DB is bad state. Incorrect handle %d",
            __func__, acl_handle);
        }
      }
    }
  }

  if (opcode == HCI_LE_CREATE_CIS) {
    ALOGD("%s HCI_LE_CREATE_CIS %d", __func__, qhci_progress_cis_cmd);
    QHciProcessCreateCisCmd(data, length);
    return;
  }

  if (opcode == HCI_LE_ENABLE_ENCRYPTION_CMD) {
    ALOGD("%s HCI_LE_ENABLE_ENCRYPTION_CMD ", __func__);
    QHciPostMessageToAc(data, length);
    //delete data;
    return;
  }

  if (opcode == HCI_LE_READ_REMOTE_FEAT_CMD) {
    ALOGD("%s HCI_LE_READ_REMOTE_FEAT_CMD ", __func__);
    QHciPostMessageToAc(data, length);
    //delete data;
    return;
  }

  if (opcode == HCI_READ_REMOTE_VERSION_CMD) {
    ALOGD("%s HCI_READ_REMOTE_VERSION_CMD ", __func__);
    QHciPostMessageToAc(data, length);
    //delete data;
    return;
  }

}

/******************************************************************
 * Function       UpdateTxPktHandle
 *
 * Description    Update the handle in TX pkt if handle map done
 *                in qhci
 *
 * Arguments      *data- contains the tx packet
 *                HciPacketType - packet type
 *                length - length of the tx pkt
 *
 * return         none
 ******************************************************************/
uint8_t* QHci::UpdateTxPktHandle(HciPacketType packet_type,
                                 uint8_t *data, size_t length) {

  uint16_t handle = 0x00;
  uint16_t stack_handle = 0x00;
  uint8_t *data_new = NULL;

  if (packet_type == HCI_PACKET_TYPE_COMMAND) {

    uint16_t opcode = (data[1] << 8 | data[0]);

    switch(opcode) {
      case HCI_READ_RMT_CLOCK_OFFSET:
      case HCI_WRITE_POLICY_SETTINGS:
      case HCI_WRITE_LINK_SUPER_TOUT:
      case HCI_READ_RMT_FEATURES:
      case HCI_READ_RMT_EXT_FEATURES:
      case HCI_AUTHENTICATION_REQUESTED:
      case HCI_SET_CONN_ENCRYPTION:
      case HCI_READ_ENCRYPTION_MODE:
      case HCI_READ_ENCR_KEY_SIZE:
      case HCI_EXIT_SNIFF_MODE:
      case HCI_SNIFF_MODE:
      case HCI_FLOW_SPECIFICATION:
      case HCI_CHANGE_CONN_PACKET_TYPE:
      case HCI_SNIFF_SUB_RATE: {

        stack_handle = (data[4] << 8 | data[3]);

        ALOGD("%s opcode 0x%04x, stack_handle 0x%04x", __func__,
                opcode, stack_handle);

        handle = QHciHm::GetIf()->GetBredrHandleFromStackHandle(stack_handle);

        ALOGD("%s handle 0x%04x, stack_handle 0x%04x", __func__,
               handle, stack_handle);

        if (handle != stack_handle) {
          data_new = (uint8_t *)malloc(length * sizeof(uint8_t));
          memcpy(data_new, data, length * sizeof(uint8_t));
          data_new[3] = (uint8_t) 0xFF & handle;
          data_new[4] = (uint8_t) 0xFF & handle >> 8;
        }

        ALOGD("%s data[3] 0x%02x, data[4] 0x%02x", __func__,
               data[3], data[4]);

        break;
      }
      case HCI_READ_RMT_VERSION_INFO:
      case HCI_DISCONNECT: {

        stack_handle = (data[4] << 8 | data[3]);

        ALOGD("%s opcode 0x%04x, stack_handle 0x%04x", __func__,
               opcode, stack_handle);

        handle = QHciHm::GetIf()->GetSocHandleFromStackHandle(stack_handle);

        if (handle != stack_handle) {
          data_new = (uint8_t *)malloc(length * sizeof(uint8_t));
          memcpy(data_new, data, length * sizeof(uint8_t));
          data_new[4] = 0xFF & handle >> 8;
          data_new[3] = 0xFF & handle;
        }
        break;
      }
      case HCI_BLE_READ_REMOTE_FEAT:
      case HCI_BLE_START_ENC:
      case HCI_BLE_UPD_LL_CONN_PARAMS:
      case HCI_BLE_SET_PHY:
      case HCI_BLE_RC_PARAM_REQ_REPLY:
      case HCI_BLE_SET_DATA_LENGTH:
      case HCI_LE_SET_PERIODIC_ADVERTISING_SET_INFO_TRANSFER: {

        stack_handle = (data[4] << 8 | data[3]);

        handle = QHciHm::GetIf()->GetLeHandleFromStackHandle(stack_handle);

        ALOGD("%s opcode 0x%04x, stack_handle 0x%04x, le_handle 0x%04x", __func__,
              opcode, stack_handle, handle);

        if (handle != stack_handle) {
          data_new = (uint8_t *)malloc(length * sizeof(uint8_t));
          memcpy(data_new, data, length * sizeof(uint8_t));
          data_new[4] = 0xFF & handle >> 8;
          data_new[3] = 0xFF & handle;
        }
        break;
      }
      case HCI_LE_CREATE_CIS: {

        uint8_t CIS_COUNT = data[3];

        stack_handle = (data[7] << 8 | data[6]);
        if (IsQHciApTransportEnable(stack_handle)) {
          handle = QHciHm::GetIf()->GetXpanHandleFromStackHandle(stack_handle);
        } else {
          handle = QHciHm::GetIf()->GetLeHandleFromStackHandle(stack_handle);
        }

        if (handle != stack_handle) {
          data_new = (uint8_t *)malloc(length * sizeof(uint8_t));
          memcpy(data_new, data, length * sizeof(uint8_t));
          data_new[7] = 0xFF & handle >> 8;
          data_new[6] = 0xFF & handle;

          if (CIS_COUNT == 2) {
            data_new[11] = 0xFF & handle >> 8;
            data_new[10] = 0xFF & handle;
          }
        }
        break;
      }
    }
  } else if (packet_type == HCI_PACKET_TYPE_ACL_DATA) {

    stack_handle = (data[1] << 8 | data[0]);

    handle = QHciHm::GetIf()->GetSocHandleFromStackHandle(stack_handle);

    ALOGD("%s handle 0x%04x, stack_handle 0x%04x", __func__,
           handle, stack_handle);

    if (handle != stack_handle) {
      data_new = (uint8_t *)malloc(length * sizeof(uint8_t));
      memcpy(data_new, data, length * sizeof(uint8_t));
      data_new[1] = 0xFF & handle >> 8;
      data_new[0] = 0xFF & handle;
    }
  }

  return data_new;
}
} // namespace implementation
} // namespace xpan

