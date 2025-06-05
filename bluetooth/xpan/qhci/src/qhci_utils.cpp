/* 
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <signal.h>
#include "data_handler.h"
#include "qhci_main.h"
#include "qhci_packetizer.h"
#include "qhci_xm_if.h"
#include "xpan_utils.h"
#include <hidl/HidlSupport.h>
#include "hci_transport.h"
#include <utils/Log.h>


#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.qhci@1.0-xpan_qhci_utils"

namespace xpan {
namespace implementation {

char* QHci::ConvertIpcEventToString(QHciEventId eventId) {
  switch(eventId) {
    case QHCI_LE_CONN_CMPL_EVT:
      return "QHCI_LE_CONN_CMPL_EVT";
    case QHCI_ACL_DISCONNECT:
      return "QHCI_ACL_DISCONNECT";
    case QHCI_XM_PREPARE_REQ:
      return "QHCI_XM_PREPARE_REQ";
    case QHCI_XM_PREPARE_REQ_BT:
      return "QHCI_XM_PREPARE_REQ_BT";
    case QHCI_XM_PREPARE_REQ_AP:
      return "QHCI_XM_PREPARE_REQ_AP";
    case QHCI_XM_PREPARE_RSP:
      return "QHCI_XM_PREPARE_RSP";
    case QHCI_PROCESS_RX_PKT_EVT:
      return "QHCI_PROCESS_RX_PKT_EVT";
    case QHCI_STATE_CHANGE:
      return "QHCI_STATE_CHANGE";
    case QHCI_USECASE_UPDATE_CFM:
      return "QHCI_USECASE_UPDATE_CFM";
    case QHCI_XM_UNPREPARE_REQ:
      return "QHCI_XM_UNPREPARE_REQ";
    case QHCI_XM_UNPREPARE_RSP:
      return "QHCI_XM_UNPREPARE_RSP";
    case QHCI_TRANSPORT_ENABLE:
      return "QHCI_TRANSPORT_ENABLE";
    case QHCI_UPDATE_TRANSPORT:
      return "QHCI_UPDATE_TRANSPORT";
    case QHCI_DELAY_REPORT_EVT:
      return "QHCI_DELAY_REPORT_EVT";
    case QHCI_USECASE_UPDATE_CMD:
      return "QHCI_USECASE_UPDATE_CMD";
    case QHCI_BEARER_SWITCH_IND:
      return "QHCI_BEARER_SWITCH_IND";
    case QHCI_VENDOR_ENCODER_LIMIT_CMD:
      return "QHCI_VENDOR_ENCODER_LIMIT_CMD";
    case QHCI_LOCAL_VER_LE_FEATURES_TO_AC:
      return "QHCI_LOCAL_VER_LE_FEATURES_TO_AC";
    case QHCI_SEND_CREATE_CONNECT_TO_AC:
      return "QHCI_SEND_CREATE_CONNECT_TO_AC";
    case QHCI_SEND_CANCEL_CONNECT_TO_AC:
      return "QHCI_SEND_CANCEL_CONNECT_TO_AC";
    case QHCI_SEND_READ_REMOTE_LE_FEATURES:
      return "QHCI_SEND_READ_REMOTE_LE_FEATURES";
    case QHCI_SEND_READ_REMOTE_VER_REQ:
      return "QHCI_SEND_READ_REMOTE_VER_REQ";
    case QHCI_SEND_CONNECTION_CMPL_EVENT:
      return "QHCI_SEND_CONNECTION_CMPL_EVENT";
    case QHCI_RECV_REMOTE_SUPPORT_LE_FEAT_EVENT:
      return "QHCI_RECV_REMOTE_SUPPORT_LE_FEAT_EVENT";
    case QHCI_RECV_REMOTE_VERSION_INFO_EVENT:
      return "QHCI_RECV_REMOTE_VERSION_INFO_EVENT";
    case QHCI_RECV_NOCP_FROM_AC:
      return "QHCI_RECV_NOCP_FROM_AC";
    case QHCI_RECV_CONN_CMPL_FROM_AC:
      return "QHCI_RECV_CONN_CMPL_FROM_AC";
    case QHCI_SEND_LE_ENCRYPT_CMD_TO_AC:
      return "QHCI_SEND_LE_ENCRYPT_CMD_TO_AC";
    case QHCI_RECV_ENCRYPT_CMPL_FROM_AC:
      return "QHCI_RECV_ENCRYPT_CMPL_FROM_AC";
    case QHCI_XPAN_BONDED_DEVICE_LIST:
      return "QHCI_XPAN_BONDED_DEVICE_LIST";
    case QHCI_SEND_DISCONNECT_TO_AC:
      return "QHCI_SEND_DISCONNECT_TO_AC";
    case QHCI_RECV_DISCONNECTION_CMPL_EVENT:
      return "QHCI_RECV_DISCONNECTION_CMPL_EVENT";
    case QHCI_RECV_ACL_DATA:
      return "QHCI_RECV_ACL_DATA";
    case QHCI_RECV_CONNECT_LE_REQ_FROM_XM:
      return "QHCI_RECV_CONNECT_LE_REQ_FROM_XM";
    case QHCI_L2CAP_PAUSE_UNPAUSE_REQ:
      return "QHCI_L2CAP_PAUSE_UNPAUSE_REQ";
    case QHCI_WIFI_SCAN_STARTED_IND:
      return "QHCI_WIFI_SCAN_STARTED_IND";
    default:
      ALOGW("%s: Invalid IPC EVENT ID", __func__);
  }
  return NULL;
}


char* QHci::ConvertEventToString(uint8_t eventId) {
  switch(eventId) {
    case QHCI_CSM_LE_CONN_CMPL_EVT:
      return "QHCI_CSM_LE_CONN_CMPL_EVT";
      break;
    case QHCI_CSM_LE_CLOSE_EVT:
      return "QHCI_CSM_LE_CLOSE_EVT";
      break;
    case QHCI_CSM_CIS_OPEN_XPAN_TRANS_DISABLE_EVT:
      return "QHCI_CSM_CIS_OPEN_XPAN_TRANS_DISABLE_EVT";
      break;
    case QHCI_CSM_CIS_OPEN_XPAN_TRANS_ENABLE_EVT:
      return "QHCI_CSM_CIS_OPEN_XPAN_TRANS_ENABLE_EVT";
      break;
    case QHCI_CSM_CIS_CLOSE_XPAN_TRANS_ENABLE_EVT:
      return "QHCI_CSM_CIS_CLOSE_XPAN_TRANS_ENABLE_EVT";
      break;
    case QHCI_CSM_XPAN_CONN_FAILED_EVT:
      return "QHCI_CSM_XPAN_CONN_FAILED_EVT";
      break;
    case QHCI_CSM_CIS_DISCONNECT_EVT:
      return "QHCI_CSM_CIS_DISCONNECT_EVT";
      break;
    case QHCI_CSM_USECASE_XPAN_TRANS_ENABLE_EVT:
      return "QHCI_CSM_USECASE_XPAN_TRANS_ENABLE_EVT";
      break;
    case QHCI_CSM_USECASE_XPAN_TRANS_DISABLE_EVT:
      return "QHCI_CSM_USECASE_XPAN_TRANS_DISABLE_EVT";
      break;
    case QHCI_CSM_UPDATE_TRANS_XPAN_EVT:
      return "QHCI_CSM_UPDATE_TRANS_XPAN_EVT";
      break;
    case QHCI_CSM_PREPARE_BEARER_BT:
      return "QHCI_CSM_PREPARE_BEARER_BT";
      break;
    case QHCI_CSM_PREPARE_BEARER_XPAN:
      return "QHCI_CSM_PREPARE_BEARER_XPAN";
      break;
    case QHCI_CSM_CIS_DISCONNECT_TX_EVT:
      return "QHCI_CSM_CIS_DISCONNECT_TX_EVT";
      break;
    case QHCI_CSM_BEARER_SWITCH_FAILED:
      return "QHCI_CSM_BEARER_SWITCH_FAILED";
      break;
    case QHCI_CSM_BT_LE_CONNECT_EVT:
      return "QHCI_CSM_BT_LE_CONNECT_EVT";
      break;
    case QHCI_CSM_BT_LE_DISCONNECT_EVT:
      return "QHCI_CSM_BT_LE_DISCONNECT_EVT";
      break;
    case QHCI_CSM_P2P_PREPARE_BEARER_EVT:
      return "QHCI_CSM_P2P_PREPARE_BEARER_EVT";
      break;
    case QHCI_CSM_P2P_PREPARE_BEARER_FAIL_EVT:
      return "QHCI_CSM_P2P_PREPARE_BEARER_FAIL_EVT";
      break;
    case QHCI_CSM_BT_PREPARE_BEARER_EVT:
      return "QHCI_CSM_BT_PREPARE_BEARER_EVT";
      break;
    case QHCI_CSM_BT_PREPARE_BEARER_FAIL_EVT:
      return "QHCI_CSM_BT_PREPARE_BEARER_FAIL_EVT";
      break;
    case QHCI_CSM_P2P_TRANSPORT_ENABLE_EVT:
      return "QHCI_CSM_P2P_TRANSPORT_ENABLE_EVT";
      break;
    case QHCI_CSM_P2P_TRANSPORT_DISABLE_EVT:
      return "QHCI_CSM_P2P_TRANSPORT_DISABLE_EVT";
      break;
    case QHCI_CSM_BT_BEARER_SWITCH_IND_EVT:
      return "QHCI_CSM_BT_BEARER_SWITCH_IND_EVT";
      break;
    case QHCI_CSM_BT_BEARER_SWITCH_IND_FAIL_EVT:
      return "QHCI_CSM_BT_BEARER_SWITCH_IND_FAIL_EVT";
      break;
    case QHCI_CSM_P2P_BEARER_SWITCH_IND_EVT:
      return "QHCI_CSM_P2P_BEARER_SWITCH_IND_EVT";
      break;
    case QHCI_CSM_P2P_BEARER_SWITCH_IND_FAIL_EVT:
      return "QHCI_CSM_P2P_BEARER_SWITCH_IND_FAIL_EVT";
      break;
    case QHCI_CSM_AP_LE_CONNECT_EVT:
      return "QHCI_CSM_AP_LE_CONNECT_EVT";
      break;
    case QHCI_CSM_AP_LE_DISCONNECT_EVT:
      return "QHCI_CSM_AP_LE_DISCONNECT_EVT";
      break;
    case QHCI_CSM_BEARER_PREFERENCE_BT_EVT:
      return "QHCI_CSM_BEARER_PREFERENCE_BT_EVT";
      break;
    case QHCI_CSM_BEARER_PREFERENCE_AP_EVT:
      return "QHCI_CSM_BEARER_PREFERENCE_AP_EVT";
      break;
    case QHCI_CSM_AP_PREPARE_BEARER_EVT:
      return "QHCI_CSM_AP_PREPARE_BEARER_EVT";
      break;
    case QHCI_CSM_AP_PREPARE_BEARER_FAIL_EVT:
      return "QHCI_CSM_AP_PREPARE_BEARER_FAIL_EVT";
      break;
    case QHCI_CSM_BT_CONNECT_REQ_EVT:
      return "QHCI_CSM_BT_CONNECT_REQ_EVT";
      break;
    case QHCI_CSM_AP_BEARER_SWITCH_IND_EVT:
      return "QHCI_CSM_AP_BEARER_SWITCH_IND_EVT";
      break;
    case QHCI_CSM_AP_BEARER_SWITCH_IND_FAIL_EVT:
      return "QHCI_CSM_AP_BEARER_SWITCH_IND_FAIL_EVT";
      break;
    case QHCI_CSM_CIS_ESTABLISH_EVT:
      return "QHCI_CSM_CIS_ESTABLISH_EVT";
      break;
    case QHCI_CSM_PREPARE_BEARER_XPAN_AP_REQ:
      return "QHCI_CSM_PREPARE_BEARER_XPAN_AP_REQ";
      break;
    case QHCI_CSM_BEARER_SWITCH_IND_SUCCESS:
      return "QHCI_CSM_BEARER_SWITCH_IND_SUCCESS";
      break;
    case QHCI_CSM_LE_TX_DISCONN_EVT:
      return "QHCI_CSM_LE_TX_DISCONN_EVT";
      break;
    default:
      ALOGW("%s: Invalid EVENT ID", __func__);
  }
  return NULL;
}

char* QHci::ConvertMsgtoString(uint8_t state) {
  switch(state) {
    case QHCI_IDLE_STATE:
      return "QHCI_IDLE_STATE";
    case QHCI_BT_CLOSE_XPAN_CLOSE:
      return "QHCI_BT_CLOSE_XPAN_CLOSE";
    case QHCI_BT_CLOSE_XPAN_CONNECTING:
      return "QHCI_BT_CLOSE_XPAN_CONNECTING";
    case QHCI_BT_CLOSE_XPAN_OPEN:
      return "QHCI_BT_CLOSE_XPAN_OPEN";
    case QHCI_BT_OPEN_XPAN_CLOSE:
      return "QHCI_BT_OPEN_XPAN_CLOSE";
    case QHCI_BT_OPEN_XPAN_CONNECTING:
      return "QHCI_BT_OPEN_XPAN_CONNECTING";
    case QHCI_BT_OPEN_XPAN_OPEN:
      return "QHCI_BT_OPEN_XPAN_OPEN";
    /* TODO Fill Remaining */
    default:
      ALOGW("%s: Invalid State %d", __func__, state);
  }
  return NULL;
}

char* QHci::ConvertStatetoString(uint8_t state) {
  switch(state) {
    case QHCI_TRANSPORT_IDLE_STATE:
      return "QHCI_TRANSPORT_IDLE_STATE";
    case QHCI_BT_ENABLE:
      return "QHCI_BT_ENABLE";
    case QHCI_BT_ENABLE_AP_CONNECTING:
      return "QHCI_BT_ENABLE_AP_CONNECTING";
    case QHCI_BT_ENABLE_AP_ENABLE:
      return "QHCI_BT_ENABLE_AP_ENABLE";
    case QHCI_AP_ENABLE:
      return "QHCI_AP_ENABLE";
    case QHCI_AP_ENABLE_BT_CONNECTING:
      return "QHCI_AP_ENABLE_BT_CONNECTING";
    case QHCI_P2P_ENABLE:
      return "QHCI_P2P_ENABLE";
    case QHCI_P2P_ENABLE_BT_ENABLE:
      return "QHCI_P2P_ENABLE_BT_ENABLE";
    /* TODO Fill Remaining */
    default:
      ALOGW("%s: Invalid State %d", __func__, state);
  }
  return NULL;
}

char* QHci::ConvertCisStatetoString(QHCI_CIS_STATE state) {
  switch(state) {
    case QHCI_CIS_CLOSE:
      return "QHCI_CIS_CLOSE";
    case QHCI_CIS_CONNECTING:
      return "QHCI_CIS_CONNECTING";
    case QHCI_CIS_OPEN:
      return "QHCI_CIS_OPEN";
    case QHCI_CIS_DISCONNECTING:
      return "QHCI_CIS_DISCONNECTING";
    default:
      ALOGW("%s: Invalid State %d", __func__, state);
  }
  return NULL;
}

bool QHci::isXpanStreamActive() {
  bdaddr_t default_addr = {};

  for (int i = 0; i < 6; i++) {
    if (active_xpan_device.b[i] != default_addr.b[i]) return true;
  }

  return false;

}

/******************************************************************
 *
 * Function       getCigParams
 *
 * Description    returning current active CIG params
 *
 * Arguments      None
 *
 * return         qhci_cig_params_t
 ******************************************************************/
qhci_cig_params_t QHci::getCigParams() {
  return cig_params;
}

/******************************************************************
 *
 * Function       isXpanSupported
 *
 * Description    returning is current device support XPAN or not
 *
 * Arguments      None
 *
 * return         bool
 ******************************************************************/
bool QHci::isXpanSupported() {
  return is_xpan_supported;
}

/******************************************************************
 *
 * Function       isCisHandleDiscPending
 *
 * Description    returning is current cis handle disconnect pending
 *
 * Arguments      None
 *
 * return         bool
 ******************************************************************/
bool QHci::isCisHandleDiscPending() {
  return is_cis_handle_disc_pending;
}

/******************************************************************
 *
 * Function       setCisHandleDiscPending
 *
 * Description    Setting Disconnect Pending Value true
 *
 * Arguments      bool
 *
 * return         None
 ******************************************************************/
void QHci::setCisHandleDiscPending(bool value) {
   is_cis_handle_disc_pending = value;
}

/******************************************************************
 *
 * Function       IsQHciApTransportEnable
 *
 * Description    AP Transport is Active or Not
 *
 * Arguments      None
 *
 * return         None
 ******************************************************************/
bool QHci::IsQHciApTransportEnable(uint16_t handle) {
  //enhance it for multiple devices connected it over AC
  ALOGD("%s: handle %d ", __func__, handle);

  qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);

  if (rem_info) {
    ALOGD("%s: Rem Link Transport %s Curr_transport %s ", __func__,
      TransportTypeToString(rem_info->qhci_link_transport),
      TransportTypeToString(qhci_curr_transport));

    if (rem_info->qhci_link_transport == XPAN_AP)
      return true;

    if ((qhci_ac_active_handle == handle) &&
        (qhci_curr_transport == XPAN_AP)) {
      ALOGD("%s: IsQHciApTransportEnable True for handle %d ", __func__, handle);
      return true;
    }
  }

  return false;
}


/******************************************************************
 *
 * Function       IsQHciXpanSupportedAddress
 *
 * Description    Checking the bd_addr is in supported XPAN devices
 *                or not?
 *
 * Arguments      bd_addr
 *
 * return         true/false
 ******************************************************************/
bool QHci::isQHciXpanSupportedAddress(bdaddr_t bd_addr) {
  ALOGD("%s ", __func__);
#if 1
  if (dbg_direct_ap_prop) {
    if (dbg_mtp_lib_prop) {
      return true;
    } else {
      return false;
    }
  }
#endif
  for (int i = 0; i < qhci_xpan_bonded_list.num_devices; i++) {
    if (QHciCmpBDAddrs(bd_addr, qhci_xpan_bonded_list.bonded_devices[i]))
      return (dbg_direct_ap_prop? true: false);
  }
  return false;
}

void QHci::QHciClearRemoteDeviceInfo(uint16_t handle) {
  uint16_t xpan_dev_size = qhci_xpan_dev_db.size();
  ALOGD("%s Size %d", __func__, xpan_dev_size);

  for (int i = 0; i < xpan_dev_size; i++) {
    ALOGD("%s db_handle %d handle %d", __func__,
            qhci_xpan_dev_db[i].handle, handle);
    if (qhci_xpan_dev_db[i].handle == handle) {
      qhci_xpan_dev_db.erase(qhci_xpan_dev_db.begin() + i);
      return;
    }
  }
  qhci_bearer_switch_pending = false;
}

qhci_dev_cb_t* QHci::GetQHciRemoteDeviceInfo(uint16_t handle) {
  //Get Remote info from the active XPAN devices

  ALOGD("%s ", __func__);
  for (int i = 0; i < qhci_xpan_dev_db.size(); i++) {
    if (handle == qhci_xpan_dev_db[i].handle) {
      ALOGD("%s Remote device matched in database ", __func__);
      return &qhci_xpan_dev_db[i];
    }
  }
  return NULL;
}

/******************************************************************
 *
 * Function       IsQHciSupportVersion
 *
 * Description    Checking the Remote Version is QHCI Supported
                  version or not
 *
 *
 * Arguments      version- uint8_t
 *
 * return         true/false
 ******************************************************************/
bool QHci::IsQHciSupportVersion (uint8_t version) {
  ALOGD("%s version %d", __func__, version);

  if (version >= QHCI_BT_MIN_VERSION_SUPPORT) {
    return true;
  }

  return false;
}

/******************************************************************
 *
 * Function       IsQHciSupportLmpVersion
 *
 * Description    Checking the remote LMP Version is QHCI Supported
                  version or not
 *
 *
 * Arguments      lmp version- uint16_t
 *
 * return         true/false
 ******************************************************************/
bool QHci::IsQHciSupportLmpVersion (uint16_t subversion) {
  ALOGD("%s ", __func__);

  if (subversion >= QHCI_BT_MIN_LMP_VERSION_SUPPORT) {
    return true;
  }

  return true;
}

/******************************************************************
 *
 * Function       IsQHciSupportManuFacture
 *
 * Description    Checking the remote Manufacute is QHCI Supported
                  manufacture id or not
 *
 *
 * Arguments      manufacture_id- uint16_t
 *
 * return         true/false
 ******************************************************************/
bool QHci::IsQHciSupportManuFacture (uint16_t manufacture_id) {
  ALOGD("%s ", __func__);

  if (manufacture_id == QHCI_BT_MANUFACTURE_SUPPORT_ID) {
    return true;
  }

  return false;
}

/******************************************************************
 *
 * Function       IsQHciXpanSupportedDevice
 *
 * Description    Checking the handle is in supported XPAN devices
 *                or not?
 *
 *
 * Arguments      hidl_data- vector with uint8_t
 *
 * return         true/false
 ******************************************************************/
bool QHci::IsQHciXpanSupportedDevice(uint16_t handle) {
  ALOGD("%s ", __func__);

  std::map<uint16_t, bdaddr_t>::iterator it;
  it = xpan_active_devices_.find(handle);
  if (it != xpan_active_devices_.end()) {

    ALOGD("%s XPAN DEVICE IS SUPPORTED", __func__);
    return true;
  }
  return false;

}

/******************************************************************
 *
 * Function       QHciGetMappingAclHandle
 *
 * Description    Checking the handle from CIS Handle
 *
 *
 * Arguments      uint16_t
 *
 * return         uint16_t
 ******************************************************************/
uint16_t QHci::QHciGetMappingAclHandle(uint16_t cis_handle) {
  ALOGD("%s ", __func__);
  uint16_t acl_handle = 0;
  if (cis_acl_handle_map.find(cis_handle) != cis_acl_handle_map.end()) {
    acl_handle = cis_acl_handle_map[cis_handle];
    ALOGD("%s Cis Handle mappaed to acl_handle %d", __func__, acl_handle);
  }

  return acl_handle;
}

bool QHci::QHciCmpBDAddrs(bdaddr_t bd_addr1, bdaddr_t bd_addr2) {
  ALOGD("%s Address1 %s size %d", __func__,
         ConvertRawBdaddress(bd_addr1), sizeof(bd_addr1));
  ALOGD("%s Address2  %s size %d", __func__,
         ConvertRawBdaddress(bd_addr2), sizeof(bd_addr2));

  int val = memcmp(&bd_addr1, &bd_addr2, sizeof(bd_addr1));

  if (val != 0) {
    ALOGD("%s Address not equal", __func__);
    return false;
  }

  return true;
}

uint16_t QHci::QHciBDAddrToHandleMap(bdaddr_t bd_addr) {
  ALOGD("%s ", __func__);

  for (auto& it : handle_bdaddr_map_) {
      // If mapped value is K,
      // then print the key value
      if (QHciCmpBDAddrs(it.second, bd_addr)) {
          ALOGD("%s Addr %s Handle %d", __func__,
                 ConvertRawBdaddress(bd_addr), it.first);
          return (it.first);
      }
  }
  return 0;
}

bool QHci::isRemoteXpanTransportEnabled(uint16_t handle) {

  qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);

  if (rem_info && rem_info->transport_enable) {
    ALOGW("%s Transport_enabled: %d", __func__,
        rem_info->transport_enable);
    return true;
  }
  return false;
}

bool QHci::isXpanTransportEnabled()
{
  for (auto it = xpan_active_devices_.begin();
        it != xpan_active_devices_.end(); it++) {
    uint16_t handle = it->first;
    ALOGD("%s Acl Handle %d", __func__, handle);
    qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);

    if (rem_info && rem_info->transport_enable) {
        ALOGW("%s Transport_enabled: %d", __func__,
                rem_info->transport_enable);
        return true;
    }
  }
  return false;
}


QHCI_CSM_STATE QHci::GetQHciStateByHandle(uint16_t cis_handle) {

  uint16_t handle = QHciGetMappingAclHandle(cis_handle);
  qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);

  if (rem_info) {
    ALOGD("%s:  state  %s", __func__, ConvertMsgtoString(rem_info->state));
    return rem_info->state;
  }

  return QHCI_IDLE_STATE;
}


QHCI_CSM_STATE QHci::GetQHciState(qhci_dev_cb_t *rem_info) {

  if (rem_info) {
    ALOGD("%s:  state  %s", __func__, ConvertMsgtoString(rem_info->state));
    return rem_info->state;
  }

  return QHCI_IDLE_STATE;

}

QHCI_CIS_STATE QHci::QHciGetCisState(qhci_dev_cb_t *rem_info) {

  if (rem_info) {
    ALOGD("%s state %s", __func__, ConvertCisStatetoString(rem_info->cis_state));
    return(rem_info->cis_state);
  } else {
    ALOGE("%s rem_info is in bad state , FATAL ", __func__);
  }
  return QHCI_CIS_CLOSE;
}

bool QHci::isStreamingActive(bdaddr_t bd_addr) {

  uint16_t handle = QHciBDAddrToHandleMap(bd_addr);
  bool status = false;
    ALOGD("%s isStreamingActive Handle %d %s", __func__, handle,
        ConvertRawBdaddress(bd_addr));

  if (handle != 0) {
    qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);

    if (rem_info) {
        ALOGD("%s isStreamingActive %s", __func__,
        ConvertCisStatetoString(rem_info->cis_state));
      if (rem_info->cis_state == QHCI_CIS_OPEN) {
        status = true;
      }
    }
  }

  return status;
}

QHCI_CTSM_STATE QHci::GetQhciTransportState(uint16_t handle) {
  qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);

  if (rem_info) {
    ALOGD("%s QHCI_STATE %s", __func__,
        ConvertStatetoString(rem_info->tState));
    return rem_info->tState;
  }

  return QHCI_TRANSPORT_IDLE_STATE;
}

TransportType QHci::GetQHciActiveLinkTransport(uint16_t handle) {
  qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);

  if (rem_info) {
    ALOGD("%s QHCI Active Link Transport %s", __func__,
        TransportTypeToString(rem_info->qhci_link_transport));
    return rem_info->qhci_link_transport;
  }

  return DEFAULT;
}

}
}

