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
#include "qhci_hm.h"


#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.qhci@1.0-xpan_qhci_handler"

using android::hardware::bluetooth::V1_0::implementation::DataHandler;


namespace xpan {
namespace implementation {

void QHci::QHciSendDisconnectCmplt(uint16_t handle, uint8_t reason) {
  ALOGE("%s  ", __func__);

  uint8_t qhci_evt_pkt[6]
    = {0x5, 0x4, 0x0, 0x0, 0x0, 0x16};

  qhci_evt_pkt[3] = 0x00FF & handle;
  qhci_evt_pkt[4] = 0xFF00 & handle;
  
  qhci_evt_pkt[5] = reason;

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(6);
    memcpy(hidl_data->data(), qhci_evt_pkt, 6);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }
}

/******************************************************************
 *
 * Function       QHciSendNocpEvent
 *
 * Description    QHCI Sending NOCP event to Stack
 *
 * Arguments      Acl Handle
 *                No of packets sent
 *
 * return         None
 ******************************************************************/
void QHci::QHciSendNocpEvent(uint16_t handle,
                                    uint8_t no_of_pkts_sent) {
  ALOGE("%s  ", __func__);

  uint8_t qhci_evt_pkt[7]
    = {0x13, 0x05, 0x01, 0x00, 0x00, 0x01, 0x00};

    qhci_evt_pkt[3] = (handle & 0x00FF);
    qhci_evt_pkt[4] = (handle & 0xFF00) >> 8;
    qhci_evt_pkt[5] = no_of_pkts_sent;

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(7);
    memcpy(hidl_data->data(), qhci_evt_pkt, 7);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }
}

/******************************************************************
 *
 * Function       QHciSendCisEstablishedEvt
 *
 * Description    Sending Fake CIS Established Evt to Stack
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *
 * return         none
 ******************************************************************/
void QHci::QHciSendCisEstablishedEvt(qhci_dev_cb_t *rem_info,
                                               uint8_t status,
                                               uint8_t handle_num) {

  ALOGD("%s ", __func__);

  if (rem_info) {
    //Move this logic to other thread _if there any crashes
    uint16_t length = QHCI_CIS_ESTABLISHED_EVT_SIZE;
    uint8_t data[QHCI_CIS_ESTABLISHED_EVT_SIZE]
      = {0x3E, 0x1D, 0x19, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x10,
        0x27, 0x00, 0xAE, 0x4C, 0x01, 0xAE, 0x4C, 0x01, 0x10, 0x10,
        0x07, 0x02, 0x00, 0x04, 0x04, 0x9B, 0x00, 0x00, 0x00, 0x10,
        0x00};
    //CIG Sync Delay - 0x002710 -- 10000msec
    //Transport Latency - 0x014CAE
    //ISO interval - 0x0010
    //num of subevents - 7

    //Event Code
    data[0] = HCI_LE_EVT;
    //length 
    data[1] = length - 2;
    //SubEvent
    data[2] = HCI_LE_CIS_ESTABLISHED_EVT;
    //Status
    data[3] = status;
    //CIS Handle
    data[4] = cig_params.cis_handles[handle_num] & 0x00FF;
    data[5] = (cig_params.cis_handles[handle_num] & 0xFF00) >> 8;

    DataHandler *data_handler = DataHandler::Get();
    if (data_handler) {
#ifdef XPAN_SUPPORTED
      hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
      hidl_data->resize(QHCI_CIS_ESTABLISHED_EVT_SIZE);
      memcpy(hidl_data->data(), data, QHCI_CIS_ESTABLISHED_EVT_SIZE);

      data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
          hidl_data, false);
#endif
    }
  }

}


void QHci::QHciSendDisconnectCmdStatus() {
  ALOGE("%s  ", __func__);

  uint8_t qhci_evt_pkt[6]
    = {0x0f, 0x04, 0x00, 0x01, 0x06, 0x04};

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(6);
    memcpy(hidl_data->data(), qhci_evt_pkt, 6);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }
}


void QHci::QHciSendUsecaseUpdateCfm(uint8_t usecase) {
  ALOGE("%s  ", __func__);

  uint8_t vs_qhci_evt_pkt[VSC_QHCI_VENDOR_USECASE_EVT_LEN]
    = {0xe, 0x7, 0x1, 0x0a, 0xfc, 0x15, 0xFF, 0xFF, 0xFF};
  if ((usecase == (uint8_t) XPAN_WIFI_HQ_PROFILE)
    || (usecase == (uint8_t) XPAN_WIFI_GAMING_PROFILE)) {
    is_xpan_supported = true;
  } else {
    is_xpan_supported = false;
  }

  vs_qhci_evt_pkt[8] = usecase;
  
  ALOGD("%s %d DBGG usecase ", __func__, vs_qhci_evt_pkt[8]);
  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(VSC_QHCI_VENDOR_USECASE_EVT_LEN);
    memcpy(hidl_data->data(), vs_qhci_evt_pkt, VSC_QHCI_VENDOR_USECASE_EVT_LEN);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }
}

void QHci::QHciUseCaseUpdateEvt(uint8_t usecase) {
  ALOGE("%s: ++++ Updating usecase ", __func__);

  uint8_t vs_qhci_evt_pkt[QHCI_DELAY_REPORT_EVT_LEN]
    = {0xFF, 0x4, 0x12, 0xFF, 0xFF, 0xFF};

  vs_qhci_evt_pkt[5] = usecase;

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(6);
    memcpy(hidl_data->data(), vs_qhci_evt_pkt, 6);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }
}

void QHci::QHciDelayReportingEvt(qhci_msg_t *msg) {
  ALOGE("%s: ++++ Sending Delay reporting ", __func__);

  uint8_t vs_qhci_evt_pkt[QHCI_DELAY_REPORT_EVT_LEN]
    = {0xFF, 0x4, 0x12, 0xFF, 0xFF, 0xFF};

  vs_qhci_evt_pkt[3] = msg->DelayReport.delay_report & 0x00FF;
  vs_qhci_evt_pkt[4] = (msg->DelayReport.delay_report & 0xFF00) >> 8;

  ALOGE("%s: ++++ Sending Delay reporting value %d", __func__,
    ((vs_qhci_evt_pkt[4] << 8) | vs_qhci_evt_pkt[3]));
  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(6);
    memcpy(hidl_data->data(), vs_qhci_evt_pkt, 6);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }
}

void QHci::QHciSendEncryCmplEvent(uint16_t handle, uint8_t status,
                                      uint8_t encryptionEnabled) {

  ALOGE("%s  ", __func__);

  uint8_t qhci_evt_pkt[6]
    = {0x8, 0x4, 0x0, 0x0, 0x0, 0x01};

  qhci_evt_pkt[2] = status;

  qhci_evt_pkt[3] = 0x00FF & handle;
  qhci_evt_pkt[4] = 0xFF00 & handle;

  qhci_evt_pkt[5] = encryptionEnabled;

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(6);
    memcpy(hidl_data->data(), qhci_evt_pkt, 6);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }

}

/******************************************************************
 *
 * Function       QHciSendRemoteVersionCmplEvt
 *
 * Description    QHCI Sending remote version complete event to
 *                stack
 *
 * Arguments      Acl Handle
 *                version
 *                companyId
 *                subversion
 *
 * return         None
 ******************************************************************/
void QHci::QHciSendRemoteVersionCmplEvt(uint16_t handle,
                                                   uint8_t version,
                                                   uint16_t companyId,
                                                   uint16_t subversion) {
  ALOGE("%s  ", __func__);

  uint8_t qhci_evt_pkt[10]
    = {0xc, 0x08, 0x00, 0x00, 0x00, 0x0D, 0x1D, 0x00, 0xC3, 0x61};

  qhci_evt_pkt[3] = (handle & 0x00FF);
  qhci_evt_pkt[4] = (handle & 0xFF00) >> 8;
  qhci_evt_pkt[5] = version;
  qhci_evt_pkt[6] = companyId & 0x00FF;
  qhci_evt_pkt[7] = (companyId & 0xFF00) >> 8;
  qhci_evt_pkt[8] = subversion & 0x00FF;
  qhci_evt_pkt[9] = (subversion & 0xFF00) >> 8;

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(10);
    memcpy(hidl_data->data(), qhci_evt_pkt, 10);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }
}

void QHci::QHciSendLeRemoteFeatureEvent(uint16_t handle, uint8_t status,
                                                   uint64_t feature_mask) {

  ALOGE("%s  ", __func__);

  uint8_t qhci_evt_pkt[14]
    = {0x3e, 0x0c, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,0x00,0x00};

  qhci_evt_pkt[3] = status;

  qhci_evt_pkt[4] = (handle & 0x00FF);
  qhci_evt_pkt[5] = (handle & 0xFF00) >> 8;

  qhci_evt_pkt[6] = feature_mask & 0x00FF;
  qhci_evt_pkt[7] = (feature_mask >> 8) & 0x00FF;
  qhci_evt_pkt[8] = (feature_mask >> 16) & 0x00FF;
  qhci_evt_pkt[9] = (feature_mask >> 24) & 0x00FF;
  qhci_evt_pkt[10] = (feature_mask >> 32) & 0x00FF;
  qhci_evt_pkt[11] = (feature_mask >> 40) & 0x00FF;
  qhci_evt_pkt[12] = (feature_mask >> 48) & 0x00FF;
  qhci_evt_pkt[13] = (feature_mask >> 56) & 0x00FF;

  ALOGE("%s  0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", __func__,
            qhci_evt_pkt[6], qhci_evt_pkt[7], qhci_evt_pkt[8],
            qhci_evt_pkt[9], qhci_evt_pkt[10], qhci_evt_pkt[11],
            qhci_evt_pkt[12], qhci_evt_pkt[13]);

  DataHandler *data_handler = DataHandler::Get();

  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(14);
    memcpy(hidl_data->data(), qhci_evt_pkt, 14);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }
}

void QHci::QHciLeConnCmplEvent(uint16_t handle, bdaddr_t bd_addr,
                                       uint8_t status) {
  ALOGE("%s  ", __func__);
  uint8_t qhci_evt_pkt[33] =
    {0x3e, 0x1f, 0x0a, 0x00, 0x02, 0x00, 0x00, 0x00, 0x25, 0xeb, 0x00, 0x5b,
     0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xf4, 0x01, 0x00};

  qhci_evt_pkt[3] = status;
  qhci_evt_pkt[4] = handle & 0x00FF;
  qhci_evt_pkt[5] = (handle & 0xFF00) >> 8;
  qhci_evt_pkt[8] = bd_addr.b[0];
  qhci_evt_pkt[9] = bd_addr.b[1];
  qhci_evt_pkt[10] = bd_addr.b[2];
  qhci_evt_pkt[11] = bd_addr.b[3];
  qhci_evt_pkt[12] = bd_addr.b[4];
  qhci_evt_pkt[13] = bd_addr.b[5];

  DataHandler *data_handler = DataHandler::Get();

  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(33);
    memcpy(hidl_data->data(), qhci_evt_pkt, 33);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }
}

void QHci::QHciSendLeConnUpdateEvt(uint16_t handle,
                                            uint16_t conn_interval) {
  ALOGD("%s  handle %d conn_interval %d", __func__, handle, conn_interval);
  uint8_t qhci_evt_pkt[12]
    = {0x3e, 0x0A, 0x03, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xf4, 0x01};

  qhci_evt_pkt[4] = (handle & 0x00FF);
  qhci_evt_pkt[5] = (handle & 0xFF00) >> 8;
  qhci_evt_pkt[6] = (conn_interval & 0x00FF);
  qhci_evt_pkt[7] = (conn_interval & 0xFF00) >> 8;

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(12);
    memcpy(hidl_data->data(), qhci_evt_pkt, 12);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }

}

void QHci::QHciSendAcRxDataToStack(uint16_t handle,
                                         uint8_t* rx_data, uint16_t rx_len) {
  ALOGD("%s  handle %d length %d", __func__, handle, rx_len);

  ALOGD("%s data %d %d %d %d %d ", __func__, rx_data[0], rx_data[1],
         rx_data[2], rx_data[3], rx_data[4]);

  uint16_t length = rx_len + 4;

  uint8_t* qhci_evt_pkt;
  qhci_evt_pkt = (uint8_t *)malloc(length * sizeof(uint8_t));

  qhci_evt_pkt[0] = (handle & 0x00FF);
  qhci_evt_pkt[1] = 0x20;
  qhci_evt_pkt[2] = ((length - 4) & 0x00FF);
  qhci_evt_pkt[3] = ((length - 4) & 0xFF00) >> 8;

  for (int i = 0; i < rx_len; i++)
    qhci_evt_pkt[4+i] = rx_data[i];


  for (int i = 0; i < length; i++)
    ALOGD("%s RX data 0x%x ", __func__, qhci_evt_pkt[i]);

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(length);
    memcpy(hidl_data->data(), qhci_evt_pkt, length);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_ACL_DATA,
                                        hidl_data, false);
#endif
  }

  free(qhci_evt_pkt);
}

void QHci::QHciLeCreateConnectToSoc(bdaddr_t bd_addr) {
  ALOGI("%s  ", __func__);

  int length = QHCI_LE_CREATE_CONN_LEN;
  uint8_t * data = (uint8_t *)malloc (length);
  //opcode
  data[0] = 0x43;
  data[1] = 0x20;
  //length
  data[2] = 0x3A;
  //Initiater policy, use peer address and peer addr type
  data[3] = 0x0;
  //own address type
  data[4] = 0x1;
  //peer address type
  data[5] = 0x0;
  //Bd addr
  data[6] = bd_addr.b[0];
  data[7] = bd_addr.b[1];
  data[8] = bd_addr.b[2];
  data[9] = bd_addr.b[3];
  data[10] = bd_addr.b[4];
  data[11] = bd_addr.b[5];
  //initiating phys
  data[12] = 0x07;
  //1M phy
  data[13] = 0x60;
  data[14] = 0x00;
  data[15] = 0x30;
  data[16] = 0x00;
  data[17] = 0x18;
  data[18] = 0x00;
  data[19] = 0x28;
  data[20] = 0x00;
  data[21] = 0x00;
  data[22] = 0x00;
  data[23] = 0xF4;
  data[24] = 0x01;
  data[25] = 0x00;
  data[26] = 0x00;
  data[27] = 0x00;
  data[28] = 0x00;
  //2M phy
  data[29] = 0x60;
  data[30] = 0x00;
  data[31] = 0x30;
  data[32] = 0x00;
  data[33] = 0x18;
  data[34] = 0x00;
  data[35] = 0x28;
  data[36] = 0x00;
  data[37] = 0x00;
  data[38] = 0x00;
  data[39] = 0xF4;
  data[40] = 0x01;
  data[41] = 0x00;
  data[42] = 0x00;
  data[43] = 0x00;
  data[44] = 0x00;
  //coded phy
  data[45] = 0x60;
  data[46] = 0x00;
  data[47] = 0x30;
  data[48] = 0x00;
  data[49] = 0x18;
  data[50] = 0x00;
  data[51] = 0x28;
  data[52] = 0x00;
  data[53] = 0x00;
  data[54] = 0x00;
  data[55] = 0xF4;
  data[56] = 0x01;
  data[57] = 0x00;
  data[58] = 0x00;
  data[59] = 0x00;
  data[60] = 0x00;

  qhci_ap_soc_cmd_pending = true;

#ifdef XPAN_SUPPORTED
  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
    if (data_handler->GetControllerRef() != nullptr)
      SET_BIT(qhci_hci_cmd_wait, BIT_LE_CREATE_CONN_CMD_TO_SOC);
      data_handler->UpdateRingBufferFromQHci(HCI_PACKET_TYPE_COMMAND,
                                             data, length);
      ALOGD("%s Sending data to the soc", __func__);
      data_handler->GetControllerRef()->SendPacket(HCI_PACKET_TYPE_COMMAND,
                                                   data, length);
  }
#endif
  free((uint8_t *)data);

}

/******************************************************************
 *
 * Function       SendCreateCisToSoc
 *
 * Description    Sending Create CIS to SOC
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *
 * return         none
 ******************************************************************/
void QHci::SendCreateCisToSoc(qhci_dev_cb_t *rem_info) {

  ALOGD("%s ", __func__);
  uint16_t handle;

  if (rem_info) {
    handle = rem_info->handle;
    ALOGD("%s CIS Handles %d %d", __func__, cig_params.cis_handles[0],
           cig_params.cis_handles[1]);
    if (rem_info->tState == QHCI_BT_ENABLE_AP_ENABLE) {
      uint16_t le_handle =
        QHciHm::GetIf()->GetLeHandleFromStackHandle(handle);
      ALOGD("%s Handle %d and Le Handle %d", __func__, handle, le_handle);
      if ((le_handle != 0) && (le_handle!= handle)) {
        handle = le_handle;
      }
    }
  } else {
    ALOGE("%s Enter Valid Handle ", __func__);
    handle = 0;
  }

  //Move this logic to other thread _if there any crashes
  uint16_t length = 12;
  uint8_t *data = (uint8_t *)malloc(length);

  //Opcode
  data[0] = 0x64;
  data[1] = 0x20;

  //length
  data[2] = 9;

  //cis_count
  data[3] = 2;


  //Currently create cis going each handle separately
  data[4] = cig_params.cis_handles[0] & 0x00FF;
  data[5] = cig_params.cis_handles[0] & 0xFF00;

  //ACL Handle
  data[6] = handle & 0x00FF;
  data[7] = handle & 0xFF00;

  data[8] = cig_params.cis_handles[1] & 0x00FF;
  data[9] = cig_params.cis_handles[1] & 0xFF00;

  //ACL Handle
  data[10] = handle & 0x00FF;
  data[11] = handle & 0xFF00;
  SET_BIT(qhci_hci_cmd_wait, BIT_LE_CREATE_CIS_QHCI_TO_SOC);

#ifdef XPAN_SUPPORTED
  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
    if (data_handler->GetControllerRef() != nullptr)
      data_handler->UpdateRingBufferFromQHci(HCI_PACKET_TYPE_COMMAND,
                                             data, length);
      ALOGD("%s Sending data to the soc", __func__);
      data_handler->GetControllerRef()->SendPacket(HCI_PACKET_TYPE_COMMAND,
                                                   data, length);
  }
#endif
  free((uint8_t *)data);


}

/******************************************************************
 *
 * Function       QHciPrepareAndSendHciDisconnect
 *
 * Description    Sending HCI Disconnect  to SCO
 *
 * Arguments      acl handle
 *
 * return         none
 ******************************************************************/
void QHci::QHciPrepareAndSendHciDisconnect(uint16_t handle) {
  ALOGD("%s:  handle %d", __func__, handle);

  if (handle > 0) {
     uint16_t length = QHCI_DISCONNECT_COMMAND_LEN;

     uint8_t *data = (uint8_t *)malloc(length);

     data[0] = 0x06;
     data[1] = 0x04;
     data[2] = 0x03;
     data[3] = handle & 0x00FF;
     data[4] = (handle & 0xFF00) >> 8;
     data[5] = 0x13; //Remote user terminated connection status code

     is_cis_handle_disc_pending = true;
     SET_BIT(qhci_hci_cmd_wait, BIT_LE_DISCONNECT_QHCI_TO_SOC);
     DataHandler *data_handler = DataHandler::Get();
     if (data_handler) {
       if (data_handler->GetControllerRef() != nullptr)
         data_handler->UpdateRingBufferFromQHci(HCI_PACKET_TYPE_COMMAND,
                                                data, length);
         ALOGD("%s Sending data to the soc", __func__);
         data_handler->GetControllerRef()->SendPacket(HCI_PACKET_TYPE_COMMAND,
                                                      data, length);
     }
     free((uint8_t *)data);
  }
  return;
}

void QHci::QHciSendCmdStatusForCis() {
  ALOGD("%s ", __func__);

  uint8_t qhci_cmd_status[QHCI_CMD_STATUS_CIS_LEN]
      = {0x0f, 0x04, 0x00, 0x01, 0x64, 0x20};

  ALOGD("%s Sending Status", __func__);

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    ALOGD("%s Sending Cis Command status to stack ", __func__);
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(QHCI_CMD_STATUS_CIS_LEN);
    memcpy(hidl_data->data(), qhci_cmd_status, QHCI_CMD_STATUS_CIS_LEN);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  } else {
    ALOGE("%s Command Status couldnt send data_handler is null", __func__);
  }
}

void QHci:: QHciSendCmdStatusToStack(uint16_t opcode) {
  ALOGD("%s Sending Status opcode 0x%x", __func__, opcode);
  uint8_t qhci_cmd_status[6]
          = {0x0f, 0x04, 0x00, 0x01, 0x00, 0x00};

  qhci_cmd_status[4] = opcode & 0x00FF;
  qhci_cmd_status[5] = (opcode & 0xFF00) >> 8;

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    ALOGD("%s Sending to stack ", __func__);
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(6);
    memcpy(hidl_data->data(), qhci_cmd_status, 6);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  } else {
    ALOGE("%s Command Status couldnt send data_handler is null", __func__);
  }

}

void QHci::QHciSendVndrQllEvtMask() {
    ALOGD("%s  ", __func__);

    uint8_t qhci_evt_pkt[7]
      = {0xe, 0x5, 0x1, 0x51, 0xfc, 0x00, 0xf};

    DataHandler *data_handler = DataHandler::Get();
    if (data_handler) {
#ifdef XPAN_SUPPORTED
      hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
      hidl_data->resize(7);
      memcpy(hidl_data->data(), qhci_evt_pkt, 7);
      data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                          hidl_data, false);
#endif
    }

}

void QHci::QHciSendCmdCmpltForEncoderLimit() {
  ALOGD("%s ", __func__);

  uint8_t qhci_cmd_cmplt[QHCI_CMD_CMPLT_ENCODER_LIMIT]
      = {0x0e, 0x07, 0x01, 0x51, 0xfc, 0x00, 0x24, 0x00, 0x00};

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    ALOGD("%s Sending EncoderLimit Compt event to stack ", __func__);
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(QHCI_CMD_CMPLT_ENCODER_LIMIT);
    memcpy(hidl_data->data(), qhci_cmd_cmplt, QHCI_CMD_CMPLT_ENCODER_LIMIT);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  } else {
    ALOGE("%s Command Status couldnt send data_handler is null", __func__);
  }
}

/******************************************************************
 *
 * Function       QHciSendLeSetDataLengthStatus
 *
 * Description    QHCI Sending set Data Length event to Stack
 *
 * Arguments      Acl Handle
 *                No of packets sent
 *
 * return         None
 ******************************************************************/
void QHci::QHciSendLeSetDataLengthStatus(uint16_t handle) {
  ALOGE("%s  ", __func__);

  uint8_t qhci_evt_pkt[QHCI_LEN_SET_DATA_LENGTH_EVT] =
    {0x0E, 0x06, 0x01, 0x22, 0x20, QHCI_ERR_COMMAND_DISALLOWED, 0x00, 0x00};

    qhci_evt_pkt[6] = (handle & 0x00FF);
    qhci_evt_pkt[7] = (handle & 0xFF00) >> 8;

  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
#ifdef XPAN_SUPPORTED
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(QHCI_LEN_SET_DATA_LENGTH_EVT);
    memcpy(hidl_data->data(), qhci_evt_pkt, QHCI_LEN_SET_DATA_LENGTH_EVT);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  }
}

void QHci::SendRxPktToHost(qhci_msg_t *msg) {
  DataHandler *data_handler = DataHandler::Get();
  ALOGD("%s  ", __func__);
  if (data_handler) {
#if 0
    if (!ProcessRxLogger(msg)) {
      free(msg->RxEvtPkt.hidl_data);
      return;
    }
#endif
    hidl_vec<uint8_t> *hidl_data = new hidl_vec(*msg->RxEvtPkt.hidl_data);
#ifdef XPAN_SUPPORTED
    data_handler->OnPacketReadyFromQHci(msg->RxEvtPkt.type,
                                        hidl_data, false);
#endif
    free(msg->RxEvtPkt.hidl_data);
    ALOGD("%s After free", __func__);
  }
}

void QHci::QHciProcessQllReq(uint16_t handle) {
  ALOGD("%s ", __func__);

  uint8_t  pkt[QHCI_VS_QLL_CMD_REQ_LEN] = {0};
  //Opcode
  pkt[0] = 0x51;
  pkt[1] = 0xFC;

  pkt[2] = 3; //length
  pkt[3] = QHCI_VS_QBCE_READ_REMOTE_QLL_SUPPORTED_FEATURES;

  pkt[4] = (handle & 0x00FF);
  pkt[5] = (handle & 0xFF00);

  qhci_qll_req_sent = true;
  DataHandler *data_handler = DataHandler::Get();
  if (data_handler) {
    if (data_handler->GetControllerRef() != nullptr)
      ALOGD("%s  Sending data to the soc", __func__);
      data_handler->GetControllerRef()->SendPacket(HCI_PACKET_TYPE_COMMAND,
                                                 pkt, QHCI_VS_QLL_CMD_REQ_LEN);
  }

}

/******************************************************************
 *
 * Function       QHciXPanBearerTransitionCmdToSoc
 *
 * Description    Sending HCI Disconnect  to Soc
 *
 * Arguments      transition state
 *                handle
 *                transition type
 *
 * return         none
 ******************************************************************/
void QHci::QHciXPanBearerTransitionCmdToSoc(uint8_t transition_state,
                                            uint16_t handle,
                                            uint16_t transition_type) {
  ALOGD("%s:  handle %d", __func__, handle);

  if (handle > 0) {
     uint16_t length = QHCI_XPAN_BEARER_TRANSITION_CMD;

     uint8_t *data = (uint8_t *)malloc(length);

     data[0] = 0x00;
     data[1] = 0xFE;
     data[2] = 6;
     data[3] = 0x2;
     data[4] = handle & 0x00FF;
     data[5] = (handle & 0xFF00) >> 8;
     data[6] = transition_state;
     data[7] = 0x00; //Frequency band default is non 2.4gh
     data[8] = transition_type;

     DataHandler *data_handler = DataHandler::Get();
     if (data_handler) {
       if (data_handler->GetControllerRef() != nullptr)
         data_handler->UpdateRingBufferFromQHci(HCI_PACKET_TYPE_COMMAND,
                                                data, length);
         ALOGD("%s Sending data to the soc", __func__);
         data_handler->GetControllerRef()->SendPacket(HCI_PACKET_TYPE_COMMAND,
                                                      data, length);
     }
  	 free((uint8_t *)data);
  }
  return;
}

void QHci::QHciLeCancelCmdCmplEvt(uint8_t status) {
  ALOGD("%s ", __func__);

  uint8_t qhci_cmd_cmplt[QHCI_CMD_CMPLT_LE_CANCEL_CONN_LEN]
      = {0x0e, 0x04, 0x01, 0x0e, 0x20, 0x00};

  DataHandler *data_handler = DataHandler::Get();
  qhci_cmd_cmplt[5] = status;

  if (data_handler) {
#ifdef XPAN_SUPPORTED
    ALOGD("%s event to stack ", __func__);
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(QHCI_CMD_CMPLT_LE_CANCEL_CONN_LEN);
    memcpy(hidl_data->data(), qhci_cmd_cmplt,
      QHCI_CMD_CMPLT_LE_CANCEL_CONN_LEN);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  } else {
    ALOGE("%s Command Status couldnt send data_handler is null", __func__);
  }
}

void QHci::QHciSetupIsoDataCmdCmplEvt(uint8_t status, uint16_t handle) {
  ALOGD("%s handle %d ", __func__, handle);

  uint8_t qhci_cmd_cmplt[QHCI_CMD_CMPLT_SETUP_ISO_PATH_LEN]
      = {0x0e, 0x06, 0x01, 0x6e, 0x20, 0x00, 0x00, 0x00};

  DataHandler *data_handler = DataHandler::Get();
  qhci_cmd_cmplt[5] = status;
  qhci_cmd_cmplt[6] = (handle & 0x00FF);
  qhci_cmd_cmplt[7] = (handle & 0xFF00) >> 8;

  if (data_handler) {
#ifdef XPAN_SUPPORTED
    ALOGD("%s LE_SETUP_ISO_DATA_PATH event to stack ", __func__);
    hidl_vec<uint8_t> *hidl_data = new hidl_vec<uint8_t>;
    hidl_data->resize(QHCI_CMD_CMPLT_SETUP_ISO_PATH_LEN);
    memcpy(hidl_data->data(), qhci_cmd_cmplt,
      QHCI_CMD_CMPLT_SETUP_ISO_PATH_LEN);
    data_handler->OnPacketReadyFromQHci(HCI_PACKET_TYPE_EVENT,
                                        hidl_data, false);
#endif
  } else {
    ALOGE("%s Command Status couldnt send data_handler is null", __func__);
  }
}

}
}

