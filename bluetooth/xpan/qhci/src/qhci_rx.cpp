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

#define LOG_TAG "vendor.qti.qhci@1.0-xpan_qhci_rx"
using android::hardware::bluetooth::V1_0::implementation::DataHandler;
using xpan::implementation::QHciHm;

namespace xpan {
namespace implementation {

extern QHciXmIntf qhci_xm_intf;

/******************************************************************
 *
 * Function       IsQhciRxPkt
 *
 * Description    Checking the received HCI RX Event should be
 *                parsed by QHCI or not?
 *
 *
 * Arguments      hidl_data- vector with uint8_t
 *
 * return         true/false
 ******************************************************************/
bool QHci::IsQhciRxPkt (const hidl_vec <uint8_t> *hidl_data) {
  const uint8_t* data = hidl_data->data();

  if (data[0] == QHCI_COMMAND_STATUS) {
    uint16_t opcode = ((data[5] << 8) | (data[4]));
    if (opcode == HCI_DISCONNECT) {
      ALOGD("%s HCI_DISCONNECT Command Status %d", __func__, data[2]);
      if (qhci_cis_disconnect_cmd_wait) {
        return true;
      } else if (IS_BIT_SET(qhci_hci_cmd_wait, BIT_LE_DISCONNECT_QHCI_TO_SOC)) {
        ALOGI("%s HCI_DISCONNECT Command Status will skip stack", __func__);
        CLEAR_BIT(qhci_hci_cmd_wait, BIT_LE_DISCONNECT_QHCI_TO_SOC);
        return true;
      } else {
        return false;
      }
    }
    if (opcode == HCI_LE_CREATE_CIS) {
      if (IS_BIT_SET(qhci_hci_cmd_wait, BIT_LE_CREATE_CIS_QHCI_TO_SOC)) {
        ALOGI("%s HCI_LE_CREATE_CIS Command Status will skip stack", __func__);
        CLEAR_BIT(qhci_hci_cmd_wait, BIT_LE_CREATE_CIS_QHCI_TO_SOC);
        return true;
      } else {
        return false;
      }
    }
  }

  if (data[0] == QHCI_COMMAND_STATUS) {
    if (qhci_ap_soc_cmd_pending) {
      ALOGD("%s This command status will ignored because of MTP is already"
             "connected to AP -- Status %d", __func__, data[2]);
      return true;
    }
  }

  if (data[0] == QHCI_COMMAND_COMPLETE_EVENT) {
    uint16_t opcode = ((data[4] << 8) | (data[3]));
    if (opcode == 0xFC51) {
      if (data[6] == 0x0F) {
        if (!qhci_set_host_bit) {
          ALOGD("%s HCI_LE_VENDOR_SPECIFIC_EVENT Params ", __func__);
          return true;
        } else {
          return false;
        }
      }
    }
    if (opcode == 0xFC51) {
      if (data[6] == 0x14) {
        ALOGD("%s HCI_VS_QBCE_QLE_SET_HOST_FEATURE status %d", __func__, data[5]);
        QHciSendVndrQllEvtMask();
        return true;
      }
    }
    if (opcode == 0xFE00) {
      uint8_t sub_opcode = data[6];
      ALOGD("%s QHCI_COMMAND_COMPLETE_EVENT SubOpcode 0x%x", __func__,
             sub_opcode);
      if (sub_opcode == 0x2) {
        uint8_t status = data[5];
        uint16_t handle = ((data[8] << 8) | (data[7]));
        ALOGD("%s QHCI_COMMAND_COMPLETE_EVENT handle %d", __func__, handle);
        if(IS_BIT_SET(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD)) {
          qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);
          ALOGD("%s BIT_VENDOR_XPAN_BEARER_CMD 0x%x", __func__,
                 qhci_hci_cmd_wait);
          CLEAR_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
          if (IS_BIT_SET(qhci_hci_cmd_wait,
                BIT_SEND_CIS_DISCONNECT_TO_SOC)) {
            CLEAR_BIT(qhci_hci_cmd_wait, BIT_SEND_CIS_DISCONNECT_TO_SOC);
            if (rem_info)
              QHciSendDisconnectCisToSoc(rem_info);
          }
        }
        return true;
      }
    }
  }

  if (data[0] == QHCI_COMMAND_COMPLETE_EVENT) {
    if (qhci_wait_for_cmd_status_from_soc) {
      qhci_wait_for_cmd_status_from_soc = false;
      uint8_t length = data[1];
      uint8_t num_of_cmds = data[2];
      if (num_of_cmds == 1) {
        uint16_t opcode = ((data[4] << 8) | (data[3]));
        switch (opcode) {
          case HCI_LE_SET_CIG_PARAMETERS:
            {
              if (data[5] == QHCI_BT_SUCCESS) {
                ALOGD("%s HCI_LE_SET_CIG_PARAMETERS ", __func__);
                if (is_xpan_supported && isXpanTransportEnabled()) {
                  cig_params.cig_id = data[6];
                  cig_params.cis_count = data[7];
                   if (cig_params.cis_count == 1) {
                      cig_params.cis_handles[0] = ((data[9] << 8) | (data[8]));
                    } else {
                      cig_params.cis_handles[0] = ((data[9] << 8) | (data[8]));
                      cig_params.cis_handles[1] = ((data[11] << 8) | (data[10]));
                      ALOGD("%s HCI_LE_SET_CIG_PARAMETERS CIS HANDLES %d %d ",
                              __func__, cig_params.cis_handles[0],
                              cig_params.cis_handles[1]);
                   }
                }
              }
            }
            break;
          case HCI_LE_REMOVE_CIG:
            {
              if (data[5] == QHCI_BT_SUCCESS) {
                ALOGD("%s HCI_LE_REMOVE_CIG ", __func__);
                cig_params.cig_id = 0;
                cig_params.cis_handles[0] = 0;
                cig_params.cis_handles[1] = 0;
                cis_acl_handle_map.clear();
              }
            }
            break;
          case HCI_DISCONNECT:
            {
              if (prep_bearer_active) {
                prep_bearer_active = false;
                if (!dbg_mtp_mora_prop) {
                  qhci_xm_intf.UnPrepareAudioBearerReqToXm(curr_bd_addr, NONE);
                } else {
                  qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0], NONE);
                }
                return true;
              }
              if (is_cis_handle_disc_pending)
                return true;
            }
            break;
          case HCI_READ_LOCAL_VERSION_INFORM:
            {
              //Copy the feature request data locally
              if (data[5] == QHCI_BT_SUCCESS) {
                memcpy(&qhci_local_version, &data[6],
                        sizeof(uint64_t));
                qhci_local_version.version = data[6];
                qhci_local_version.companyId = (data[11] << 8 | data[10]);
                qhci_local_version.subversion = (data[13] << 8 | data[12]);
                ALOGD("%s HCI_READ_LOCAL_VERSION_INFORM are 0x%x 0x%x 0x%x",
                      __func__, qhci_local_version.version,
                      qhci_local_version.companyId,
                      qhci_local_version.subversion);
              } else {
                ALOGW("%s HCI_READ_LOCAL_VERSION_INFORM Failed ",__func__);
              }
              return false;
            }
            break;
          case HCI_LE_READ_LOCAL_SUPP_FEATURES:
            {
              //Copy the feature request data locally
              if (data[5] == QHCI_BT_SUCCESS) {
                memcpy(&qhci_le_local_support_feature, &data[6],
                        sizeof(uint64_t));
                ALOGD("%s HCI_LE_READ_LOCAL_SUPP_FEATURES are 0x%x ",
                      __func__, qhci_le_local_support_feature);
                //Call AC function Send remote support features and local version
                PostLocalVersionFeatureToAc();
              } else {
                ALOGW("%s HCI_LE_READ_LOCAL_SUPP_FEATURES Failed ",__func__);
              }
              return false;
            }
            break;
          default:
            ALOGD("%s Wrong QHCI_COMMAND_COMPLETE_EVENT", __func__);
        }
      }
    }
    return false;
  }

  if (data[0] == HCI_VENDOR_EVT) {
    if (data[2] == QHCI_QBCE_SUBEVENT_CODE) {
      if (data[3] == READ_REMOTE_QLL_FEATURE_CMPL_EVT) {
        uint16_t acl_handle = (data[6] << 8 | data[5]);
        uint8_t is_xpan_feature_set = data[14] & 0x20;
        ALOGD("%s QHCI_QBCE_SUBEVENT_CODE Acl handle %d  "
          "is_XPAN_feature_set %d ", __func__, acl_handle, is_xpan_feature_set);

        qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(acl_handle);
        if (encrypt_event_pending) {
          encrypt_event_pending = false;
          uint8_t evt_pkt[6] = {0x08, 0x04, 0x00, 0x02, 0x00, 0x01};
          DataHandler *data_handler = DataHandler::Get();
          if (data_handler) {
#ifdef XPAN_SUPPORTED
                hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
                hidl_data->resize(HCI_ENCRYPT_CHANGE_EVT_LEN);
                memcpy(hidl_data->data(), evt_pkt,
                  HCI_ENCRYPT_CHANGE_EVT_LEN);
                data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                                    hidl_data, false);
#endif
          }
        }
        if (is_xpan_feature_set) {

          rem_info->is_xpan_device = true;

          std::map<uint16_t, bdaddr_t>::iterator it;
          it = handle_bdaddr_map_.find(acl_handle);
          if (it != handle_bdaddr_map_.end()) {
            bdaddr_t bd_addr = it->second;
            curr_bd_addr = bd_addr;
            xpan_active_devices_[acl_handle] = it->second;
            qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(acl_handle);
            if (rem_info) {
              ALOGD("%s handle state is QHCI_BT_CLOSE_XPAN_CLOSE %d ", __func__,
                    acl_handle);
              rem_info->qhci_link_transport = BT_LE;
            }
            qhci_curr_transport = BT_LE;
            qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH);
            msg->eventId = QHCI_LE_CONN_CMPL_EVT;
            msg->ConnCmpl.eventId = QHCI_LE_CONN_CMPL_EVT;
            msg->ConnCmpl.bd_addr = bd_addr;
            msg->ConnCmpl.handle = acl_handle;
            PostMessage(msg);
            return false;
          }
        }
      }
    }
  }

  if (data[0] == HCI_ENCRYPT_CHANGE_EVT) {
    //if its xpan supported version
    return false;
  }


  if ((data[0] != HCI_LE_EVT) && (data[0] != HCI_DISCONNECT_CMPL_EVT) &&
    (data[0] != HCI_READ_RMT_VERSION_COMP_EVT)) {
    return false;
  }

  if (data[0] == HCI_READ_RMT_VERSION_COMP_EVT) {
    uint8_t event_status = data[2];

    if (event_status != QHCI_BT_SUCCESS) {
      return false;
    }
    uint8_t version = data[5];
    uint16_t manufacture_id = (data[7] << 8 | data[6]);
    uint16_t lmp_version = (data[9] << 8 | data[8]);
    uint16_t le_handle = (data[4] <<8 | data[3]);

    if ((IsQHciSupportVersion(version)) &&
       (IsQHciSupportManuFacture(manufacture_id)) &&
       (IsQHciSupportLmpVersion(lmp_version)) && !dbg_mora_nut_prop) {

       qhci_wait_for_qll_event = true;
       return false;

    } else {
      //Not Supported
      if (dbg_mtp_mora_prop || dbg_mtp_mtp_prop || dbg_mora_nut_prop) {
        std::map<uint16_t, bdaddr_t>::iterator it;
        it = handle_bdaddr_map_.find(le_handle);
        if (it != handle_bdaddr_map_.end()) {
          bdaddr_t bd_addr = it->second;
          //qhci_xm_intf.RemoteSupportXpanToXm(bd_addr, true);
          curr_bd_addr = bd_addr;
          dbg_curr_bd_addr[dbg_cnt_bd_addr] = bd_addr;
          dbg_cnt_bd_addr++;
          dbg_cnt_bd_addr = dbg_cnt_bd_addr%2;
          xpan_active_devices_[le_handle] = it->second;
          qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(le_handle);
          if (rem_info) {
            ALOGD("%s handle state is QHCI_BT_CLOSE_XPAN_CLOSE %d ", __func__,
                  le_handle);
            rem_info->qhci_link_transport = BT_LE;
          }
          qhci_curr_transport = BT_LE;
          qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH);
          msg->eventId = QHCI_LE_CONN_CMPL_EVT;
          msg->ConnCmpl.eventId = QHCI_LE_CONN_CMPL_EVT;
          msg->ConnCmpl.bd_addr = bd_addr;
          msg->ConnCmpl.handle = le_handle;
          PostMessage(msg);

          //QHciSmExecute(rem_info, QHCI_CSM_LE_CONN_CMPL_EVT);
          ALOGD("%s handle for remote version %d ", __func__, le_handle);
        }
      }
      return false;
    }

    return false;
  }

  if (data[0] == HCI_DISCONNECT_CMPL_EVT) {
    //parse the handle
    is_cis_conn_prog = false;
    uint16_t handle = ((data[4] << 8) | (data[3]));

    //check the handle is present in XPAN active device map
    std::map<uint16_t, bdaddr_t>::iterator it;
    it = xpan_active_devices_.find(handle);
    if (it != xpan_active_devices_.end()) {
      ALOGD("%s HCI_DISCONNECT_CMPL_EVT is for XPAN ACTIVE DEVICE", __func__);
          //check the handle is present in XPAN active device map
      qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);

      if (rem_info) {
        if (rem_info->is_wifi_ssr_enabled) {
          ALOGW("%s HCI_DISCONNECT_CMPL_EVT Due to WIFI SSR", __func__);
          return true;
        }
        if (!IsQHciApTransportEnable(handle)) {
          if ((rem_info->state == QHCI_BT_CLOSE_XPAN_OPEN) ||
              (rem_info->state == QHCI_BT_OPEN_XPAN_OPEN)) {
              QHciSendDisconnectCmplt(cig_params.cis_handles[0],
                              QHCI_REMOTE_USER_TERMINATE_CONN);
              QHciSendDisconnectCmplt(cig_params.cis_handles[1],
                              QHCI_REMOTE_USER_TERMINATE_CONN);
          }
          ALOGW("%s HCI_DISCONNECT_CMPL_EVT QHCI_STATE: %s %s", __func__,
              ConvertStatetoString(rem_info->tState),
              ConvertMsgtoString(rem_info->state));
          QHciSmExecute(rem_info, QHCI_CSM_LE_CLOSE_EVT);
          xpan_active_devices_.erase(handle);
          handle_bdaddr_map_.erase(handle);
        } else {
          ALOGW("%s HCI_DISCONNECT_CMPL_EVT QHCI_STATE: %s %s", __func__,
            ConvertStatetoString(rem_info->tState),
            ConvertMsgtoString(rem_info->state));
          if (rem_info->tState != QHCI_BT_ENABLE_AP_ENABLE) {
            QHciTransportSmExecute(rem_info, QHCI_CSM_LE_CLOSE_EVT);
            xpan_active_devices_.erase(handle);
            handle_bdaddr_map_.erase(handle);
          } else {
            QHciTransportSmExecute(rem_info, QHCI_CSM_LE_CLOSE_EVT);
            return true;
          }
        }
      }
      return false;
    }

     if ((cig_params.cis_handles[0] == handle) ||
            (cig_params.cis_handles[1] == handle)) {
      ALOGD("%s CIS HANDLE DISCONNECT_CMPL_EVT is for XPAN- handle %d",
              __func__, handle);
      QHCI_CSM_STATE qhci_state = GetQHciStateByHandle(handle);
      if ((qhci_state == QHCI_BT_CLOSE_XPAN_CLOSE) ||
          (qhci_state == QHCI_BT_OPEN_XPAN_CLOSE)) {
          if (qhci_state == QHCI_BT_OPEN_XPAN_CLOSE) {
            //Make sure QHCI state is went to BT_CLOSE_XPAN_CLOSE
            PostQHciStateChange(handle, QHCI_BT_OPEN_XPAN_CLOSE,
                                QHCI_CSM_CIS_DISCONNECT_EVT);
          }
          cis_acl_handle_map.erase(handle);
          return false;
      } else {
        return true;
      }
    } else {
      if (handle_bdaddr_map_.find(handle) != handle_bdaddr_map_.end()) {
        ALOGD("%s Clearning Handle_Bdaddr map", __func__);
        handle_bdaddr_map_.erase(handle);
      }
      QHciClearRemoteDeviceInfo(handle);
      ALOGD("%s HCI_DISCONNECT_CMPL_EVT Non XPAN ACTIVE DEVICE - handle %d",
        __func__, handle);
      return false;
    }
  }

  uint8_t le_evt_sub_code = data[2];

  if (le_evt_sub_code == HCI_LE_EXT_ADV_EVT) return false;

  bool status = false;
  switch (le_evt_sub_code) {
    case HCI_LE_CONN_CMPL_EVT:
    {
      bdaddr_t bd_addr;
      //parsing BD addr
      for (int i = 0; i < 6; i++) {
        bd_addr.b[i] = data[8+i];
      }
      uint16_t existing_handle = QHciBDAddrToHandleMap(bd_addr);
      ALOGD ("%s HCI_LE_CONN_CMPL_EVT existing_handle %d ", __func__,
              existing_handle);
      if (!qhci_ap_soc_cmd_pending && (existing_handle == 0)) {
        qhci_dev_cb_t rem_dev_info;
        //store the Handle
        rem_dev_info.handle = ((data[5] << 8) | (data[4]));
        //Initial state will be IDLE state
        rem_dev_info.state = QHCI_IDLE_STATE;
        rem_dev_info.tState = QHCI_TRANSPORT_IDLE_STATE;

        ALOGD ("%s Parsing LE CONN CMPL EVT New Handle %d", __func__,
                rem_dev_info.handle);

        if (rem_dev_info.handle == 0) {
          ALOGD ("%s HCI_LE_CONN_CMPL_EVT InValid Handle", __func__);
          return false;
        }
        //parsing BD addr type
        rem_dev_info.addr_type = data[7];

        //parsing BD addr
        for (int i = 0; i < 6; i++) {
          rem_dev_info.rem_addr.b[i] = data[8+i];
        }

        uint16_t cur_handle = QHciBDAddrToHandleMap(rem_dev_info.rem_addr);
        ALOGD ("%s Handle_bdaddr_map size %d", __func__,
                handle_bdaddr_map_.size());

        if (cur_handle != 0) {
          ALOGW ("%s cur_handle %d is mapping bdaddr %s",__func__, cur_handle,
                  ConvertRawBdaddress(rem_dev_info.rem_addr));
          handle_bdaddr_map_.erase(cur_handle);
        }

        handle_bdaddr_map_[rem_dev_info.handle] = rem_dev_info.rem_addr;

        //inserting remote device details into xpan device db
        qhci_xpan_dev_db.push_back(rem_dev_info);

        ALOGD ("%s bdaddr %s",__func__,
                ConvertRawBdaddress(rem_dev_info.rem_addr));

        /* return false so that it can go to the BT Stack and
           QHCI wont process the command */
        status = false;
      } else {
        bdaddr_t rem_addr;
        //parsing BD addr
        for (int i = 0; i < 6; i++) {
          rem_addr.b[i] = data[8+i];
        }
        uint16_t handle = QHciBDAddrToHandleMap(rem_addr);
        ALOGD ("%s qhci_ap_soc_cmd_pending  %d bdaddr %s Handle %d", __func__,
                qhci_ap_soc_cmd_pending, ConvertRawBdaddress(rem_addr),
                handle);
        qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);
        if (rem_info) {
          ALOGD ("%s QHCI_STATE: %s", __func__,
                  ConvertStatetoString(rem_info->tState));
          if (rem_info->tState == QHCI_AP_ENABLE_BT_CONNECTING) {
            QHciTransportSmExecute(rem_info, QHCI_CSM_LE_CONN_CMPL_EVT);
          } else if (rem_info->tState == QHCI_AP_ENABLE) {
            QHciTransportSmExecute(rem_info, QHCI_CSM_LE_CONN_CMPL_EVT);
          }
        }
        qhci_ap_soc_cmd_pending = false;
        return true;
      }
    }
      break;
    case HCI_LE_READ_REMOTE_FEAT_CMPL_EVT:
    {
      //Store the details for future purpose
      status = false;
    }
     break;
    case HCI_LE_CIS_ESTABLISHED_EVT:
    {
      // If Remote supports XPAN,
      // Process the CIS Established evt
      // Else -- if remote not supports XPAN
      ALOGD("%s HCI_LE_CIS_ESTABLISHED_EVT qhci_bearer_switch_pending %d",
      __func__, qhci_bearer_switch_pending);
      if (qhci_bearer_switch_pending) {
        pending_cis_est_evt++;
        status = true;
      } else {
        status = false;
      }
    }
      break;
    default:
      status = false;
  }

  // parse the Command complete for CREATE CIG and SET ISO path
  // Check the status
  // Update the status

  return status;
}

void QHci::QHciProcessRxPktEvt(qhci_msg_t *msg) {
  ALOGD("%s msg->RxEvtPkt.hidl_data->size() %d ", __func__, msg->RxEvtPkt.hidl_data->size());
  const uint8_t* data = msg->RxEvtPkt.hidl_data->data();

  if (data[0] == QHCI_COMMAND_STATUS) {
    uint16_t opcode = ((data[5] << 8) | (data[4]));
    if (opcode == HCI_DISCONNECT) {
      ALOGD("%s HCI_DISCONNECT Command Status", __func__);
      if (qhci_cis_disconnect_cmd_wait) {
        ALOGD("%s Sending 2nd CIS_DISCONNECT Cmd ", __func__);
        qhci_cis_disconnect_cmd_wait = false;
        QHciPrepareAndSendHciDisconnect(cig_params.cis_handles[1]);
      }
    }
  }

    if (data[0] == QHCI_COMMAND_COMPLETE_EVENT) {
    uint16_t opcode = ((data[4] << 8) | (data[3]));
    if (opcode == 0xFC51) {
      if (data[6] == 0x0F) {
        ALOGD("%s HCI_LE_VENDOR_SPECIFIC_EVENT Params ", __func__);
        QHciSetHostSupportedBit();
        return;
      }
      if (data[6] == 0x14) {
        ALOGD("%s HCI_VS_QBCE_QLE_SET_HOST_FEATURE status %d", __func__, data[5]);
        return;
      }
    }
  }

  if (data[0] == HCI_ENCRYPT_CHANGE_EVT) {
    //If its xpan supported version
    uint16_t handle = (data[4] << 8 | data[3]);
    ALOGD("%s HCI_ENCRYPT_CHANGE_EVT handle %d", __func__, handle);
    //Store the key in the map
    encrypt_event_pending = true;
  }
  if (data[0] == HCI_READ_RMT_VERSION_COMP_EVT) {
    //Add the blocket call for this
    if (qhci_wait_for_qll_event) {
      ALOGD("%s Sending QLL", __func__);
      memcpy(&qhci_remote_version_data, msg->RxEvtPkt.hidl_data->data(),
              msg->RxEvtPkt.hidl_data->size());
      for (int i = 0; i < msg->RxEvtPkt.hidl_data->size(); i++)
        ALOGD("%s 0x%2x", __func__, qhci_remote_version_data[i]);
      uint16_t handle = (data[4] << 8 | data[3]);
      //QHciProcessQllReq(handle);
      return;
    }
  }

  if (data[0] == HCI_DISCONNECT_CMPL_EVT) {
    ALOGD("%s HCI_DISCONNECT_CMPL_EVT ", __func__);
    //parse the handle
    uint16_t handle = ((data[4] << 8) | (data[3]));
    qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);

    if (rem_info && rem_info->is_wifi_ssr_enabled) {
      rem_info->is_wifi_ssr_enabled = false;

      ALOGD("%s HCI_DISCONNECT_CMPLT_EVT due to WIFI SSR ", __func__);
      //Send CIS Disconnect Event Handles
      QHciSendDisconnectCmplt(cig_params.cis_handles[0],
          QHCI_REMOTE_USER_TERMINATE_CONN);
      QHciSendDisconnectCmplt(cig_params.cis_handles[1],
          QHCI_REMOTE_USER_TERMINATE_CONN);

      //change the reason code to connection timeout
      QHciSendDisconnectCmplt(rem_info->handle,
        QHCI_DISCONNECT_DUE_TO_CONN_TOUT);

      QHciSmExecute(rem_info, QHCI_CSM_LE_CLOSE_EVT);
      xpan_active_devices_.erase(handle);
      handle_bdaddr_map_.erase(handle);
      //reset rem_info if needed
      QHciClearRemoteDeviceInfo(handle);
      return;
    }

    //This code will be trigger during seamless transtition between
    //from LE to P2P.
    if (is_cis_handle_disc_pending) {
      is_cis_handle_disc_pending = false;
      if (rem_info) {
        ALOGD("%s CIS HANDLE DISCONNECTED state %s", __func__,
              ConvertMsgtoString(rem_info->state));
        rem_info->is_cis_disconnect_pending_from_soc = false;
        return;
      }
    }

    //TODO check this handle map is present in cis_acl_handle_map
    // then check the cis count and clear the handles
    // cis_acl_handle_map.erase(handle);

    std::vector<uint16_t>::iterator it2;
    it2 = std::find(active_cis_handles.begin(), active_cis_handles.end(),
                   handle);
    if (it2 != active_cis_handles.end()) {
      ALOGD("%s CIS HANDLE DISCONNECT_CMPL_EVT is for XPAN ACTIVE DEVICE",
        __func__);
      active_cis_handles.erase(it2);
      QHciSmExecute(rem_info, QHCI_CSM_CIS_DISCONNECT_EVT);
    }
  }

  if (data[0] == HCI_LE_EVT) {
    if (data[2] == HCI_LE_CIS_ESTABLISHED_EVT) {
      ALOGD("%s HCI_LE_CIS_ESTABLISHED_EVT from SOC ", __func__);
      if (pending_cis_est_evt == 2) {
        pending_cis_est_evt = 0;
        if (data[3] == QHCI_BT_SUCCESS) {
          uint16_t acl_handle = QHciBDAddrToHandleMap(pending_bd_addr_cis);
          qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(acl_handle);
          if (rem_info) {
            if (IsQHciApTransportEnable(acl_handle)) {
              if (rem_info->tState == QHCI_BT_ENABLE_AP_ENABLE) {
                QHciXPanBearerTransitionCmdToSoc(QHCI_TRANSTITION_STARTED,
                                                acl_handle,
                                                QHCI_TRANSTITION_AP_LE);
                SET_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
                qhci_xm_intf.PrepareAudioBearerRspToXm(pending_bd_addr_cis,
                                                        XM_SUCCESS);
              }
            } else {
              ALOGD("%s HCI_LE_CIS_ESTABLISHED_EVT QHCI state %s ", __func__, 
                      ConvertMsgtoString(rem_info->state));
              rem_info->state = QHCI_BT_OPEN_XPAN_OPEN;
              rem_info->tState = QHCI_P2P_ENABLE_BT_ENABLE;
              qhci_xm_intf.PrepareAudioBearerRspToXm(pending_bd_addr_cis,
                                                     XM_SUCCESS);
            }
          }
        } else {
          qhci_xm_intf.PrepareAudioBearerRspToXm(pending_bd_addr_cis, XM_FAILED);
          qhci_bearer_switch_pending = false;
        }
      }
    }
  }
}

void QHci::ProcessRxPktEvent(HciPacketType type,
                                    const hidl_vec < uint8_t > * hidl_data_t) {
  ALOGD("%s ", __func__);

  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH
                    + hidl_data_t->size());
  msg->RxEvtPkt.hidl_data = new hidl_vec(*hidl_data_t);
  msg->RxEvtPkt.type = type;
  const uint8_t* data = hidl_data_t->data();
  const uint8_t* data2 = msg->RxEvtPkt.hidl_data->data();
  uint16_t len = hidl_data_t->size();
  uint16_t len2 = msg->RxEvtPkt.hidl_data->size();
  msg->eventId = QHCI_PROCESS_RX_PKT_EVT;
  msg->RxEvtPkt.eventId = QHCI_PROCESS_RX_PKT_EVT;

  PostMessage(msg);

  return;
}

/******************************************************************
 *
 * Function       UpdateRxPktHandle
 *
 * Description    Update the handle in RX pkt if handle map done
 *                in qhci
 *
 * Arguments      *data- contains the tx packet
 *                HciPacketType - packet type
 *
 * return         none
 ******************************************************************/
void QHci::UpdateRxPktHandle(HciPacketType packet_type, uint8_t *data) {

  uint16_t soc_handle = 0x00;
  uint8_t  handle_boundary_flag = 0x00;
  uint8_t  status = 0;
  uint16_t stack_handle = 0x00;
  uint16_t opcode = 0x00;

  if (packet_type == HCI_PACKET_TYPE_EVENT) {

    uint16_t event = data[0];

    switch(event) {
      case QHCI_COMMAND_COMPLETE_EVENT: {

        opcode = (data[4] << 8 | data[3]);

        if ((data[4] == 0xFC) ||
            (data[4] == 0x10) ||
            (data[4] == 0x14) ||
            (data[4] == 0x18))
          return;

        ALOGD("%s opcode 0x%04x", __func__, opcode);

        switch (opcode) {
          case HCI_BLE_SET_DATA_LENGTH:
          case HCI_BLE_RC_PARAM_REQ_REPLY:
          case HCI_LE_SET_PERIODIC_ADVERTISING_SET_INFO_TRANSFER: {

            soc_handle = (data[7] << 8 | data[6]);
            stack_handle = QHciHm::GetIf()->GetStackHandleFromLeHandle(soc_handle);

            if (stack_handle != soc_handle) {
              data[7] = 0xFF & stack_handle >> 8;
              data[6] = 0xFF & stack_handle;
            }
            break;
          }
          case HCI_WRITE_POLICY_SETTINGS:
          case HCI_WRITE_LINK_SUPER_TOUT: {

            soc_handle = (data[7] << 8 | data[6]);

            stack_handle = QHciHm::GetIf()->GetStackHandleFromBredrHandle(soc_handle);

            ALOGD("%s soc_handle 0x%04x, stack_handle 0x%04x", __func__,
                   soc_handle, stack_handle);

            if (stack_handle != soc_handle) {
              data[7] = 0xFF & stack_handle >> 8;
              data[6] = 0xFF & stack_handle;
            }
            break;
          }
          default: {
            break;
          }
        }
        break;
      }
      case HCI_CONNECTION_COMP_EVT: {

        soc_handle = (data[4] << 8 | data[3]);
        status = data[2];
        bdaddr_t bd_addr;

        //parsing BD addr
        for (int i = 0; i < 6; i++) {
          bd_addr.b[i] = data[5+i];
        }

        stack_handle = QHciHm::GetIf()->bredrConnectionCmplt(soc_handle,bd_addr, status);
        if (stack_handle != soc_handle) {
          data[4] = 0xFF & stack_handle >> 8;
          data[3] = 0xFF & stack_handle;
        }
        break;
      }
      case HCI_READ_CLOCK_OFF_COMP_EVT:
      case HCI_LINK_SUPER_TOUT_CHANGED_EVT:
      case HCI_READ_RMT_FEATURES_COMP_EVT:
      case HCI_READ_RMT_EXT_FEATURES_COMP_EVT:
      case HCI_CONN_PKT_TYPE_CHANGE_EVT:
      case HCI_AUTHENTICATION_COMP_EVT: {

        soc_handle = (data[4] << 8 | data[3]);

        stack_handle = QHciHm::GetIf()->GetStackHandleFromBredrHandle(soc_handle);

        ALOGD("%s soc_handle 0x%04x, stack_handle 0x%04x", __func__,
               soc_handle, stack_handle);

        if (stack_handle != soc_handle) {
          data[4] = 0xFF & stack_handle >> 8;
          data[3] = 0xFF & stack_handle;
        }
        break;
      }
      case HCI_MAX_SLOTS_CHANGE_EVT: {

        soc_handle = (data[3] << 8 | data[2]);

        stack_handle = QHciHm::GetIf()->GetStackHandleFromBredrHandle(soc_handle);

        ALOGD("%s soc_handle 0x%04x, stack_handle 0x%04x", __func__,
               soc_handle, stack_handle);

        if (stack_handle != soc_handle) {
          data[3] = 0xFF & stack_handle >> 8;
          data[2] = 0xFF & stack_handle;
        }
        break;
      }
      case HCI_READ_RMT_VERSION_COMP_EVT:
      case HCI_ENCRYPT_CHANGE_EVT:
      case HCI_NUM_COMPL_DATA_PKTS_EVT: {

        soc_handle = (data[4] << 8 | data[3]);

        stack_handle = QHciHm::GetIf()->GetStackHandleFromSocHandle(soc_handle);

        ALOGD("%s soc_handle 0x%04x, stack_handle 0x%04x", __func__,
               soc_handle, stack_handle);

        if (stack_handle != soc_handle) {
          data[4] = 0xFF & stack_handle >> 8;
          data[3] = 0xFF & stack_handle;
        }
        break;
      }
      case HCI_DISCONNECT_CMPL_EVT: {

        soc_handle = (data[4] << 8 | data[3]);

        stack_handle = QHciHm::GetIf()->bredrDisconnectionCmplt(soc_handle);

        if (stack_handle == 0x00) {
          stack_handle = QHciHm::GetIf()->leDisconnectionCmplt(soc_handle);
        }

        if (stack_handle == 0x00)
          stack_handle = soc_handle;

        ALOGD("%s soc_handle 0x%04x, stack_handle 0x%04x", __func__,
               soc_handle, stack_handle);

        if (stack_handle != soc_handle) {
          data[4] = 0xFF & stack_handle >> 8;
          data[3] = 0xFF & stack_handle;
        }
        break;
      }
      case HCI_VENDOR_EVT: {
        if (QHCI_VS_TIMESYNC_EVT == data[2]) {
          soc_handle = (data[4] << 8 | data[3]);
          stack_handle = QHciHm::GetIf()->GetStackHandleFromLeHandle(soc_handle);

          ALOGD("%s soc_handle 0x%04x, stack_handle 0x%04x", __func__,
                 soc_handle, stack_handle);

          if (stack_handle != soc_handle) {
            data[4] = 0xFF & stack_handle >> 8;
            data[3] = 0xFF & stack_handle;
          }
        }
        break;
      }
      case HCI_LE_EVT: {

        uint16_t sub_event = data[2];

        switch(sub_event) {

          case HCI_LE_CONN_CMPL_EVT:
          {

            status = data[3];
            soc_handle = (data[5] << 8 | data[4]);
            bdaddr_t bd_addr;

            //parsing BD addr
            for (int i = 0; i < 6; i++) {
              bd_addr.b[i] = data[8+i];
            }
            stack_handle = QHciHm::GetIf()->leConnectionCmplt(soc_handle,bd_addr, status);
            if (stack_handle != soc_handle) {
              data[5] = 0xFF & stack_handle >> 8;
              data[4] = 0xFF & stack_handle;
            }
            break;
          }
          case HCI_LE_CHANNEL_SELECTION_ALGORITHM:
          case HCI_LE_DATA_LENGTH_CHANGE_EVT:
          case HCI_LE_RC_PARAM_REQ_EVT: {

            soc_handle = (data[4] << 8 | data[3]);

            stack_handle = QHciHm::GetIf()->GetStackHandleFromLeHandle(soc_handle);

            if (stack_handle != soc_handle) {
              data[4] = 0xFF & stack_handle >> 8;
              data[3] = 0xFF & stack_handle;
            }
            break;
          }
          case HCI_LE_READ_REMOTE_FEAT_CMPL_EVT:
          case HCI_BLE_LL_CONN_PARAM_UPD_EVT:
          case HCI_LE_PHY_UPDATE_COMPLETE_EVT: {

            soc_handle = (data[5] << 8 | data[4]);
            stack_handle = QHciHm::GetIf()->GetStackHandleFromLeHandle(soc_handle);
            if (stack_handle != soc_handle) {
              data[5] = 0xFF & stack_handle >> 8;
              data[4] = 0xFF & stack_handle;
            }
            break;
          }
        }
      }
    }
  } else if (packet_type == HCI_PACKET_TYPE_ACL_DATA) {
    soc_handle = (data[1] << 8 | data[0]);

    if (soc_handle == 0x2edc)
      return;
    
    ALOGD("%s HCI_PACKET_TYPE_ACL_DATA soc_handle 0x%04x", __func__,
           soc_handle);

    handle_boundary_flag = data[1] & 0xF0;
    soc_handle = soc_handle & HCI_DATA_HANDLE_MASK;

    stack_handle = QHciHm::GetIf()->GetStackHandleFromSocHandle(soc_handle);

    ALOGD("%s HCI_PACKET_TYPE_ACL_DATA soc_handle 0x%04x, stack_handle 0x%04x", __func__,
           soc_handle, stack_handle);

    if (stack_handle != soc_handle) {
      data[1] = 0xFF & stack_handle >> 8;
      data[1] = data[1] | handle_boundary_flag;
      data[0] = 0xFF & stack_handle;
    }
  }
}

} // namespace implementation
} // namespace xpan
