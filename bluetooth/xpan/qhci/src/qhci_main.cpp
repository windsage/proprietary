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
#include "qhci_hm.h"
#include "xpan_utils.h"
#include <hidl/HidlSupport.h>
#include "hci_transport.h"
#include "qhci_ac_if.h"
#include "XpanAcQhciIf.h"
//#include "logger.h"


#ifdef LOG_TAG
#undef LOG_TAG
#endif

//using ::xpan::implementation::QHci;
using xpan::implementation::XpanQhciAcIf;
using xpan::ac::XpanAcQhciIf;
using xpan::implementation::QHciHm;

#define LOG_TAG "vendor.qti.qhci@1.0-xpan_qhci"
using android::hardware::bluetooth::V1_0::implementation::DataHandler;

namespace xpan {
namespace implementation {

std::shared_ptr<QHci> QHci::qhci_main_instance = nullptr;

QHciXmIntf qhci_xm_intf;

QHci::QHci() {
  ALOGD("%s", __func__);
  //logger_ = Logger::Get();
  XpanAcQhciIf *intf = XpanAcQhciIf::GetIf();
}

QHci::~QHci() {
  ALOGD("%s", __func__);
}

std::shared_ptr<QHci> QHci::Get() {
  if (!qhci_main_instance)
    qhci_main_instance.reset(new QHci());
  return qhci_main_instance;
}

void QHci::QHciMainThreadRoutine() {
  DataHandler *data_handler = DataHandler::Get();

  ALOGD("%s", __func__);
  if (data_handler) {
    //data_handler->QHciIntialized(true);
  } else {
    ALOGE("%s: DataHandler Not initialized", __func__);
    return;
  }

  if (std::atomic_exchange(&main_thread_running, true)) return;

  while (main_thread_running) {

    std::atomic_exchange(&is_main_thread_busy, false);
    std::unique_lock<std::mutex> qhci_lock(qhci_wq_mtx);

    qhci_wq_notifier.wait(qhci_lock);
    qhci_lock.unlock();
    std::atomic_exchange(&is_main_thread_busy, true);

    while(1) {
      qhci_wq_mtx.lock();

      if (qhci_workqueue.empty()) {
        qhci_wq_mtx.unlock();
        break;
      } else {
        qhci_msg_t *data = qhci_workqueue.front();
        qhci_workqueue.pop();
        qhci_wq_mtx.unlock();
        QHciMsgHandler(data);

      }
    }
  }

  ALOGI("%s: is stopped", __func__);

}

void QHci::curr_usr_handler (int) {
  bool status = true;

  ALOGI("%s: exit\n", __func__);
  pthread_exit(&status);
}

int QHci::Init() {
  qhci_main_thread = std::thread([this]() {
                      struct sigaction prev_sig, curr_sig;
                      memset(&curr_sig, 0, sizeof(curr_sig));
                      curr_sig.sa_handler = curr_usr_handler;
                      sigaction(SIGUSR1, &curr_sig, &prev_sig);
                      ALOGD("%s: Starting QHCI main thread", __func__);
                      QHciMainThreadRoutine();
                     });
  if (!qhci_main_thread.joinable()) return -1;

  DataHandler *data_handler = DataHandler::Get();

  if (data_handler) {
    data_handler->QHciInitialized(true);
  }

  char value_prop[PROPERTY_VALUE_MAX] = {'\0'};
  property_get("persist.vendor.service.bt.mtp_mora_testing", value_prop , "false");
  if (strcmp(value_prop, "true") == 0) {
    dbg_mtp_mora_prop = true;
    ALOGD("%s: dbg_mtp_mora_prop %d", __func__, dbg_mtp_mora_prop);
  }

  property_get("persist.vendor.service.bt.mtp_mtp_testing", value_prop , "false");
  if (strcmp(value_prop, "true") == 0) {
    dbg_mtp_mtp_prop = true;
    ALOGD("%s: dbg_mtp_mtp_prop %d", __func__, dbg_mtp_mtp_prop);
  }

  property_get("persist.vendor.service.bt.mtp_mora_nut_testing", value_prop , "false");
  if (strcmp(value_prop, "true") == 0) {
    dbg_mora_nut_prop = true;
    ALOGD("%s: dbg_mora_nut_prop %d", __func__, dbg_mora_nut_prop);
  }

  property_get("persist.vendor.service.bt.qhci_direc_ap", value_prop , "false");
  if (strcmp(value_prop, "true") == 0) {
    dbg_direct_ap_prop = true;
    ALOGD("%s: dbg_direct_ap_prop %d", __func__, dbg_direct_ap_prop);
  }

  qhci_set_host_bit = false;

  return 0;
}

int QHci::DeInit() {
  DataHandler *data_handler = DataHandler::Get();

  if (!std::atomic_exchange(&main_thread_running, false)) {
    ALOGW("%s: main thread already stopped", __func__);
  }
  ALOGI("%s clearing out pending message", __func__);
  /* Unqueue all the pending messages */
  std::unique_lock<std::mutex> qhci_lock(qhci_wq_mtx);
  while(!qhci_workqueue.empty()) {
    qhci_msg_t *msg = qhci_workqueue.front();
    qhci_workqueue.pop();
  }
  if (qhci_main_thread.joinable()) {
    ALOGD("%s: sending SIGUSR1 signal", __func__);
    pthread_kill(qhci_main_thread.native_handle(), SIGUSR1);
    ALOGD("%s: joining QHCI main thread", __func__);
    qhci_main_thread.join();
    ALOGI("%s: joined QHCI main thread", __func__);
  }

  qhci_set_host_bit = false;

  if (data_handler) {
    data_handler->QHciInitialized(false);
  }

  if (qhci_main_instance) {
    qhci_main_instance.reset();
    qhci_main_instance = NULL;
  }

  XpanQhciAcIf::GetIf()->DeInit();
  QHciHm::GetIf()->DeInit();

  return 0;
}

void QHci::PostMessage(qhci_msg_t * msg) {
  GetMainThreadState();
  if (!main_thread_running) {
    ALOGW("%s: Main worker thread is not ready to process this message: %d",
          __func__, msg->eventId);
    free(msg);
    return;
  }

  qhci_wq_mtx.lock();
  qhci_workqueue.push(msg);
  qhci_wq_mtx.unlock();
  /* Main thread anyways get the data from queue.
   * if it is idle due to no data notify it that
   * it has data to process in its queue.
   */
  if (!is_main_thread_busy) {
    std::unique_lock<std::mutex> qhci_lock(qhci_wq_mtx);
    qhci_wq_notifier.notify_all();
  }
}

void QHci::QHciProcessConnCmplEvt(qhci_msg_t *msg) {
  ALOGD("%s  ", __func__);
  qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(msg->ConnCmpl.handle);
  if (rem_info) {
    QHciSmExecute(rem_info, QHCI_CSM_LE_CONN_CMPL_EVT);
    ALOGD("%s  QHCI_STATE: %s", __func__, ConvertStatetoString(rem_info->tState));

    //TODO_WHC_New_SM
    QHciTransportSmExecute(rem_info, QHCI_CSM_LE_CONN_CMPL_EVT);
  } else {
    ALOGD("%s  Not a valid ConnCmplEvt", __func__);
  }
}

void QHci::QHciUnprepareAudioBearerRspfromXm(bdaddr_t bd_addr,
                                                          bool status) {
  ALOGD("%s  ", __func__);
  prep_bearer_active = false;
  QHciSendDisconnectCmplt(cig_params.cis_handles[0],
      QHCI_DISCONNECT_DUE_TO_LOCAL_HOST_TERM);
  QHciSendDisconnectCmplt(cig_params.cis_handles[1],
      QHCI_DISCONNECT_DUE_TO_LOCAL_HOST_TERM);
}

void QHci::GetMainThreadState(void)
{
  ALOGD("%s:  qhci_workqueue:%p qhci_wq_mtx :%p qhci_wq_notifier %p", __func__,
            &qhci_workqueue, &qhci_wq_mtx, &qhci_wq_notifier);
}

/******************************************************************
 *
 * Function       QHciParseUsecaseUpdateCmd
 *
 * Description    process the usecase update cmd and send it to the
 *                XM Manager
 *
 *
 * Arguments      msg- Process the message based on eventId
 *
 * return         none
 ******************************************************************/
void QHci::QHciParseUsecaseUpdateCmd(qhci_msg_t *msg) {

  UseCaseType usecase_update_to_xm = USECASE_XPAN_NONE;
  uint8_t usecase_update_to_stack = 0;
  uint8_t recv_context_type = msg->TxUsecaseRcvd.context_type;
  uint16_t acl_handle = msg->TxUsecaseRcvd.acl_handle;

  ALOGD("%s Acl Handle %d context_type %d ", __func__, acl_handle,
        recv_context_type);
  qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(acl_handle);

  if (IsQHciApTransportEnable(acl_handle)) {
    if (recv_context_type == GAME_USECASE) {
      usecase_update_to_stack = LE_UNICAST_GAMING_PROFILE;
      usecase_update_to_xm = USECASE_XPAN_NONE;
    } else if (recv_context_type == HD_USECASE) {
      usecase_update_to_xm = USECASE_XPAN_AP_LOSSLESS;
      usecase_update_to_stack = XPAN_WIFI_AP_HQ_PROFILE;
    } else if (recv_context_type == VOICE_USECASE) {
      usecase_update_to_xm = USECASE_XPAN_AP_VOICE;
      usecase_update_to_stack = XPAN_AP_VOICE_CALL_PROFILE;
    }
    ALOGW("%s In AP Mode usecase_update_to_stack %d usecase_update_to_xm %d",
           __func__, usecase_update_to_stack, usecase_update_to_xm);
  } else if (xpan_active_devices_.size() > 0) {
    bool xpan_transport_enable =
      isRemoteXpanTransportEnabled(acl_handle);
    if (xpan_transport_enable) {
      if (recv_context_type == GAME_USECASE) {
        usecase_update_to_stack = LE_UNICAST_GAMING_PROFILE;
        usecase_update_to_xm = USECASE_XPAN_NONE;
      } else if (recv_context_type == HD_USECASE) {
        usecase_update_to_stack = XPAN_WIFI_HQ_PROFILE;
        usecase_update_to_xm = USECASE_XPAN_LOSSLESS;
      }
      if (rem_info) {
        rem_info->is_xpan_usecase = true;
      }
      ALOGW("%s Transport_enabled: usecase_update_to_stack %d "
            " usecase_update_to_xm %d", __func__, usecase_update_to_stack,
            usecase_update_to_xm);
    } else {
      if (recv_context_type == GAME_USECASE) {
        usecase_update_to_stack = LE_UNICAST_GAMING_PROFILE;
        usecase_update_to_xm = USECASE_XPAN_NONE;
      } else if (recv_context_type == HD_USECASE) {
        usecase_update_to_stack = LE_UNICAST_HQ_PROFILE;
        usecase_update_to_xm = USECASE_XPAN_NONE;
      }
    }
    if (recv_context_type == VOICE_USECASE) {
      usecase_update_to_xm = USECASE_XPAN_LE_VOICE;
      usecase_update_to_stack = XPAN_LE_VOICE_CALL_PROFILE;
    }
  } else {
    if (recv_context_type == GAME_USECASE) {
      usecase_update_to_stack = LE_UNICAST_GAMING_PROFILE;
      usecase_update_to_xm = USECASE_XPAN_NONE;
    } else if (recv_context_type == HD_USECASE) {
      usecase_update_to_stack = LE_UNICAST_HQ_PROFILE;
      usecase_update_to_xm = USECASE_XPAN_NONE;
    } else if (recv_context_type == VOICE_USECASE) {
      usecase_update_to_xm = USECASE_XPAN_LE_VOICE;
      usecase_update_to_stack = XPAN_LE_VOICE_CALL_PROFILE;
    }
  }

  if (rem_info) {
    ALOGW("QHCI_STATE: %s QHCI_TRANSPORT_STATE %s ",
           ConvertMsgtoString(rem_info->state),
           ConvertStatetoString(rem_info->tState));
    if ((rem_info->state == QHCI_BT_OPEN_XPAN_CLOSE) &&
         (recv_context_type == HD_USECASE)) {
        usecase_update_to_stack = LE_UNICAST_HQ_PROFILE;
    } else if ((rem_info->state == QHCI_BT_CLOSE_XPAN_OPEN) &&
         (recv_context_type == HD_USECASE)) {
      usecase_update_to_stack = XPAN_WIFI_HQ_PROFILE;
    }
  }
  if (dbg_mtp_mtp_prop) {
    usecase_update_to_stack = XPAN_WIFI_HQ_PROFILE;
    usecase_update_to_xm = USECASE_XPAN_LOSSLESS;
  }
  if (dbg_mtp_lib_prop) {
    usecase_update_to_xm = USECASE_XPAN_AP_LOSSLESS;
    usecase_update_to_stack = XPAN_WIFI_AP_HQ_PROFILE;
  }
  ALOGW("%s usecase_update_to_stack %d usecase_update_to_xm %d", __func__,
         usecase_update_to_stack, usecase_update_to_xm);

  QHciSendUsecaseUpdateCfm(usecase_update_to_stack);
  QHciUseCaseUpdateEvt(usecase_update_to_stack);

  if (usecase_type_to_xm != usecase_update_to_xm) {
    usecase_type_to_xm = usecase_update_to_xm;
    //Send to XPAN Manager about usecase update
    qhci_xm_intf.UseCaseUpdateToXm(usecase_update_to_xm);
  }
  usecase_type_on_transport = usecase_type_to_xm;

}


/******************************************************************
 *
 * Function       QHciMsgHandler
 *
 * Description    prasing the message which was scheduled in qhci
 *                main thread
 *
 *
 * Arguments      msg- Process the message based on eventId
 *
 * return         none
 ******************************************************************/
void QHci::QHciMsgHandler(qhci_msg_t *msg) {
  QHciEventId eventId = msg->eventId;
  ALOGI("%s: Event %s ", __func__, ConvertIpcEventToString(eventId));

  switch (eventId) {
    case QHCI_LE_CONN_CMPL_EVT:
      {
        QHciProcessConnCmplEvt(msg);
        break;
      }
    case QHCI_ACL_DISCONNECT:
      {
        QHciProcessRxPktEvt(msg);
        break;
      }
    case QHCI_XM_PREPARE_REQ:
      {
        uint16_t handle = QHciBDAddrToHandleMap(msg->PreBearerReq.bd_addr);
        qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);
        ALOGD("%s: Event QHCI_XM_PREPARE_REQ handle %d", __func__, handle);
        if (rem_info) {
          ALOGD("%s: is_cis_established %d qhci_state == %s ", __func__,
                 rem_info->is_cis_established,
                 ConvertMsgtoString(rem_info->state));
          if (rem_info->state == QHCI_BT_CLOSE_XPAN_CLOSE) {
            QHciSmExecute(rem_info, QHCI_CSM_CIS_CLOSE_XPAN_TRANS_ENABLE_EVT);
          } else if (rem_info->state == QHCI_BT_OPEN_XPAN_CLOSE){
            QHciSmExecute(rem_info, QHCI_CSM_CIS_OPEN_XPAN_TRANS_ENABLE_EVT);
          } else {
            ALOGE("%s: Prepare transport Request received at wrong time ", __func__);
            qhci_xm_intf.PrepareAudioBearerRspToXm(msg->PreBearerReq.bd_addr,
                                                  XM_FAILED_WRONG_TRANSPORT_TYPE_REQUESTED);
          }
        }
        break;
      }
    case QHCI_XM_PREPARE_REQ_BT:
      {
        uint16_t handle = QHciBDAddrToHandleMap(msg->PreBearerReq.bd_addr);
        qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);
        if (rem_info) {
          if (IsQHciApTransportEnable(handle)) {
             ALOGD("%s: QHCI_XM_PREPARE_REQ_BT QHCI_STATE: %s ", __func__,
                    ConvertStatetoString(rem_info->tState));
             if (rem_info->tState == QHCI_BT_ENABLE_AP_ENABLE) {
                pending_bd_addr_cis = msg->PreBearerReq.bd_addr;
                QHciTransportSmExecute(rem_info, QHCI_CSM_PREPARE_BEARER_BT);
             } else {
                qhci_xm_intf.PrepareAudioBearerRspToXm(msg->PreBearerReq.bd_addr,
                                XM_FAILED_WRONG_TRANSPORT_TYPE_REQUESTED);
             }
          } else if (GetQHciState(rem_info) == QHCI_BT_CLOSE_XPAN_OPEN) {
            pending_bd_addr_cis = msg->PreBearerReq.bd_addr;
            ALOGI("%s: Event QHCI_CSM_PREPARE_BEARER_BT ", __func__);
            QHciSmExecute(rem_info, QHCI_CSM_PREPARE_BEARER_BT);
          } else if (GetQHciState(rem_info) == QHCI_BT_OPEN_XPAN_CLOSE) {
            if (rem_info->current_transport == BT_LE) {
              qhci_xm_intf.PrepareAudioBearerRspToXm(msg->PreBearerReq.bd_addr,
                                XM_FAILED_STATE_ALREADY_IN_REQUESTED_TRANSPORT);
            } else {
              ALOGE("%s: QHCI in wrong state with wrong transport ", __func__);
            }
          } else {
            ALOGE("%s: QHCI SM is in wrong state ", __func__);
          }
        } else {
          ALOGE("%s: QHCI BAD State for  QHCI_XM_PREPARE_REQ_BT ", __func__);
        }
        break;
      }
    case QHCI_XM_PREPARE_REQ_AP:
      {
        uint16_t handle = QHciBDAddrToHandleMap(msg->PreBearerReq.bd_addr);
        qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);
        ALOGD("%s: Event QHCI_XM_PREPARE_REQ_AP handle %d", __func__, handle);
        if (rem_info) {
          ALOGD("%s: is_cis_established %d QHCI_STATE: %s ", __func__,
                 rem_info->is_cis_established,
                 ConvertStatetoString(rem_info->tState));
          if (rem_info->tState == QHCI_BT_ENABLE) {
            QHciTransportSmExecute(rem_info, QHCI_CSM_PREPARE_BEARER_XPAN_AP_REQ);
          } else if (rem_info->tState == QHCI_BT_ENABLE_AP_ENABLE){
            QHciTransportSmExecute(rem_info, QHCI_CSM_PREPARE_BEARER_XPAN_AP_REQ);
          } else {
            ALOGE("%s: Prepare transport Request received in Wrong State ", __func__);
            qhci_xm_intf.PrepareAudioBearerRspToXm(msg->PreBearerReq.bd_addr,
                                                  XM_FAILED_WRONG_TRANSPORT_TYPE_REQUESTED);
          }
        }
        break;
      }

    case QHCI_XM_PREPARE_RSP:
      {
        uint16_t handle;
        if (!dbg_mtp_lib_prop) {
          handle = QHciBDAddrToHandleMap(msg->PreBearerRsp.bd_addr);
        } else {
          handle = QHciBDAddrToHandleMap(debug_lib_bd_addr);
        }
        ALOGD("%s: QHCI_XM_PREPARE_RSP handle %d", __func__, handle);
        if (handle != 0) {
          qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);
          if (rem_info) {
            if (msg->PreBearerRsp.status == QHCI_BT_SUCCESS) {
              prep_bearer_active = true;
              // sending cis established event to stack.
              if (!IsQHciApTransportEnable(handle)) {
                rem_info->state = QHCI_BT_CLOSE_XPAN_OPEN;
              }
              rem_info->cis_state = QHCI_CIS_OPEN;
              ALOGD("%s:QHCI_XM_PREPARE_RSP Success QHCI_STATE: %s usecase %d",
                    __func__, ConvertStatetoString(rem_info->tState),
                    usecase_type_on_transport);
              QHciSendCisEstablishedEvt(rem_info, QHCI_BT_SUCCESS, 0);
              if ((usecase_type_on_transport == USECASE_XPAN_AP_VOICE)
                  || (usecase_type_on_transport == USECASE_XPAN_LE_VOICE)) {
                ALOGD("%s: Delay for 100 msec for 2nd cis establish evt",
                    __func__);
                usleep(100000);
              }
              QHciSendCisEstablishedEvt(rem_info, QHCI_BT_SUCCESS, 1);
              if (dbg_mtp_mora_prop) {
                ALOGD("%s: DBG MORA PROP ", __func__);
                uint16_t dbg_handle = QHciBDAddrToHandleMap(curr_bd_addr);
                qhci_dev_cb_t *dbg_rem_info
                    = GetQHciRemoteDeviceInfo(dbg_handle);
                if (dbg_rem_info) {
                  dbg_rem_info->state = QHCI_BT_CLOSE_XPAN_OPEN;
                  rem_info->cis_state = QHCI_CIS_OPEN;
                } else {
                  ALOGE("%s: DBG MORA is not correctly connected ", __func__);
                }
              }
            } else {
              ALOGE("%s: QHCI_XM_PREPARE_RSP FAILED %d", __func__, handle);
              QHciSmExecute(rem_info, QHCI_CSM_XPAN_CONN_FAILED_EVT);
            }
          }
        }
        break;
      }
    case QHCI_PROCESS_RX_PKT_EVT:
      {
        QHciProcessRxPktEvt(msg);
        break;
      }
    case QHCI_STATE_CHANGE:
      {
        ALOGD("%s: QHCI_STATE_CHANGE ", __func__);
        uint16_t acl_handle
                  = QHciGetMappingAclHandle(msg->StateChangeEvt.cis_handle);
        qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(acl_handle);
        if (rem_info) {
            ALOGD("%s: QHCI_STATE_CHANGE %s", __func__,
                          ConvertMsgtoString(msg->StateChangeEvt.state));
            QHciSmExecute(rem_info,
                          (QHCI_CSM_EVENT)msg->StateChangeEvt.csm_event);
        }
        break;
      }
    case QHCI_USECASE_UPDATE_CFM:
      {
        QHciSendUsecaseUpdateCfm(3);
        break;
      }
    case QHCI_XM_UNPREPARE_REQ:
      {
        qhci_packetizer.ProcessMessage(msg);
        break;
      }
    case QHCI_XM_UNPREPARE_RSP:
      {
        usecase_type_to_xm = USECASE_XPAN_NONE;
        active_xpan_device = {};
        ALOGD("QHCI_XM_UNPREPARE_RSP Active Addr %s ",
                 ConvertRawBdaddress(active_xpan_device));
        uint16_t handle = QHciBDAddrToHandleMap(msg->UnPreBearerRsp.bd_addr);
        if (handle != 0) {
          qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);
          if (rem_info) {
            ALOGI("%s: QHCI_XM_UNPREPARE_RSP QHci state %s "
                   "qhci_block_cis_disconnect_evt %d ", __func__,
                   ConvertMsgtoString(rem_info->state),
                   rem_info->qhci_block_cis_disconnect_evt);
            if (!rem_info->qhci_block_cis_disconnect_evt) {
              QHciUnprepareAudioBearerRspfromXm(msg->UnPreBearerRsp.bd_addr,
                                               msg->UnPreBearerRsp.status);
            } else {
              rem_info->qhci_block_cis_disconnect_evt = false;
            }
          }
        }
        break;
      }
    case QHCI_TRANSPORT_ENABLE:
      {
        uint16_t handle = QHciBDAddrToHandleMap(msg->TransportEnabled.bd_addr);
        ALOGW("%s: QHCI_TRANSPORT_ENABLE handle %d Enable %d", __func__,
                  handle, msg->TransportEnabled.enable);
        if (handle != 0) {
          qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);
          if (rem_info) {
            if (msg->TransportEnabled.enable == QHCI_SAP_ENABLE) {
              rem_info->transport_enable = true;
              usecase_type_on_transport = USECASE_XPAN_LOSSLESS;
              ALOGW("%s: CIS_STATE %s and QHCI_STATE: %s", __func__,
                      ConvertCisStatetoString(rem_info->cis_state),
                      ConvertStatetoString(rem_info->tState));
              rem_info->tState = QHCI_P2P_ENABLE;
              if (GetQHciState(rem_info) != QHCI_BT_CLOSE_XPAN_CLOSE) {
                if (!rem_info->is_cis_established) {
                  QHciSmExecute(rem_info,
                                QHCI_CSM_CIS_CLOSE_XPAN_TRANS_ENABLE_EVT);
                } else {
                  ALOGE("%s: QHCI_TRANSPORT_ENABLE Wrong state to handle",
                    __func__);
                  //TODO Currently its below event not handling
                  //QHciSmExecute(rem_info, QHCI_CSM_CIS_OPEN_XPAN_TRANS_ENABLE_EVT);
                }
              }
            } else if (msg->TransportEnabled.reason ==
                TRANSPORT_DISABLE_DUE_TO_WIFI_SSR) {
              ALOGI("%s:QHCI_STATE: %s", __func__,
                  ConvertMsgtoString(rem_info->state));
              if ((rem_info->state == QHCI_BT_CLOSE_XPAN_OPEN) ||
                  (rem_info->state == QHCI_BT_OPEN_XPAN_OPEN) ||
                  (rem_info->state == QHCI_BT_OPEN_XPAN_CONNECTING)) {
                // WIFI SSR
                rem_info->is_wifi_ssr_enabled = true;
                //Disconnect the LE ACL Link
                QHciPrepareAndSendHciDisconnect(rem_info->handle);
                usecase_type_on_transport = USECASE_XPAN_NONE;
                rem_info->tState = QHCI_BT_ENABLE;
              }
            } else {
              ALOGE("%s: QHCI_TRANSPORT Disabled with reason %d",
                    __func__, msg->TransportEnabled.reason);
              rem_info->transport_enable = false;
	          usecase_type_on_transport = USECASE_XPAN_NONE;
              if ((rem_info->tState == QHCI_P2P_ENABLE) ||
                  (rem_info->tState == QHCI_P2P_ENABLE_BT_ENABLE)) {
                rem_info->tState = QHCI_BT_ENABLE;
              }
            }
          }
        }
        break;
      }
    case QHCI_UPDATE_TRANSPORT:
      {
        uint16_t handle = QHciBDAddrToHandleMap(curr_bd_addr);
        ALOGE("%s: QHCI_UPDATE_TRANSPORT handle %d", __func__, handle);
        if (handle != 0) {
          qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);
          if (msg->UpdateTransport.transport_type == XPAN_P2P) {
            rem_info->update_transport = XPAN_P2P;
            if ((GetQHciState(rem_info) == QHCI_BT_CLOSE_XPAN_CONNECTING)
               || (GetQHciState(rem_info) == QHCI_BT_OPEN_XPAN_CONNECTING)) {
              QHciSmExecute(rem_info, QHCI_CSM_UPDATE_TRANS_XPAN_EVT);
            } else {
              ALOGE("%s: Update transport received in wrong state ", __func__);
            }
          } else {
            if ((GetQHciState(rem_info) == QHCI_BT_OPEN_XPAN_OPEN)) {
              QHciSmExecute(rem_info, QHCI_CSM_USECASE_XPAN_TRANS_DISABLE_EVT);
            }
          }
        }
        break;
      }
    case QHCI_DELAY_REPORT_EVT:
      {
        ALOGE("%s: ++++ Sending Delay reporting ", __func__);
        QHciDelayReportingEvt(msg);
        break;
      }
    case QHCI_USECASE_UPDATE_CMD:
      {
        QHciParseUsecaseUpdateCmd(msg);
        break;
      }
    case QHCI_BEARER_SWITCH_IND:
      {
        uint16_t handle;
        if (!dbg_mtp_lib_prop) {
          handle = QHciBDAddrToHandleMap(msg->BearerSwitchInd.bd_addr);
        } else {
          handle = QHciBDAddrToHandleMap(debug_lib_bd_addr);
        }

        if (handle != 0) {
          qhci_dev_cb_t *rem_info = GetQHciRemoteDeviceInfo(handle);
          if (rem_info) {
            ALOGI("%s: QHCI_BEARER_SWITCH_IND QHCI_P2P_STATE %s", __func__,
                   ConvertMsgtoString(rem_info->state));
            ALOGI("%s: QHCI_BEARER_SWITCH_IND QHCI_STATE: %s", __func__,
                   ConvertStatetoString(rem_info->tState));
            ALOGI("%s: QHCI_BEARER_SWITCH_IND CIS_STATE %s", __func__,
                   ConvertCisStatetoString(rem_info->cis_state));
            if (msg->BearerSwitchInd.ind_status == XM_SUCCESS) {
              active_xpan_device = msg->BearerSwitchInd.bd_addr;
              ALOGW("QHCI_BEARER_SWITCH_IND Active Addr %s",
                 ConvertRawBdaddress(active_xpan_device));
              switch (rem_info->state) {
                case QHCI_BT_OPEN_XPAN_OPEN:
                {
                  QHciSmExecute(rem_info,
                                QHCI_CSM_USECASE_XPAN_TRANS_DISABLE_EVT);
                }
                  break;
                case QHCI_BT_OPEN_XPAN_CONNECTING:
                {
                  QHciSmExecute(rem_info,
                                QHCI_CSM_UPDATE_TRANS_XPAN_EVT);
                }
                  break;
                default:
                  ALOGI("%s: Its not valid BT<-> P2P in the"
                         "state %s", __func__,
                         ConvertMsgtoString(rem_info->state));
              }
              switch (rem_info->tState) {
                case QHCI_BT_ENABLE_AP_CONNECTING:
                case QHCI_BT_ENABLE_AP_ENABLE:
                {
                  if (msg->BearerSwitchInd.transport == XPAN_AP) {
                    if (msg->BearerSwitchInd.ind_status == XM_SUCCESS) {
                      QHciTransportSmExecute(rem_info,
                          QHCI_CSM_AP_BEARER_SWITCH_IND_EVT);
                    } else {
                      QHciTransportSmExecute(rem_info,
                          QHCI_CSM_AP_BEARER_SWITCH_IND_FAIL_EVT);
                    }
                  } else if (msg->BearerSwitchInd.transport == BT_LE) {
                    if (msg->BearerSwitchInd.ind_status == XM_SUCCESS) {
                      QHciTransportSmExecute(rem_info,
                          QHCI_CSM_BT_BEARER_SWITCH_IND_EVT);
                    } else {
                      QHciTransportSmExecute(rem_info,
                          QHCI_CSM_BT_BEARER_SWITCH_IND_FAIL_EVT);
                    }
                  }
                }
                  break;
                case QHCI_AP_ENABLE:
                {
                  QHciTransportSmExecute(rem_info,
                    QHCI_CSM_AP_BEARER_SWITCH_IND_EVT);
                }
                  break;
                default:
                  ALOGI("%s: Its not valid for BT <-> WHC in the"
                         "QHCI_STATE: %s", __func__,
                         ConvertStatetoString(rem_info->tState));
              }
            } else {
              ALOGE("%s: QHCI_BEARER_SWITCH_IND Failed and stay in current"
                   "transport", __func__, rem_info->current_transport);
              ALOGW("QHCI_BEARER_SWITCH_IND fail Active Addr %s ",
                      ConvertRawBdaddress(active_xpan_device));
              switch (rem_info->state) {
                case QHCI_BT_OPEN_XPAN_OPEN:
                {
                  rem_info->state = QHCI_BT_CLOSE_XPAN_OPEN;
                  rem_info->tState = QHCI_P2P_ENABLE;
                  rem_info->is_prepare_bearer_triggered = false;
                  qhci_bearer_switch_pending = false;
                  QHciSendDisconnectCisToSoc(rem_info);
                }
                  break;
                case QHCI_BT_OPEN_XPAN_CONNECTING:
                {
                  active_xpan_device = {};
                  qhci_bearer_switch_pending = false;
                  QHciSmExecute(rem_info,
                                QHCI_CSM_XPAN_CONN_FAILED_EVT);
                }
                  break;
                case QHCI_BT_CLOSE_XPAN_OPEN:
                {
                  active_xpan_device = {};
                  QHciSmExecute(rem_info,
                                QHCI_CSM_BEARER_SWITCH_FAILED);
                }
                  break;
                default:
                  active_xpan_device = {};
                  ALOGD("%s: Its not valid for QHCI_BEARER_SWITCH_IND in the"
                         "state %s", __func__,
                         ConvertMsgtoString(rem_info->state));
              }
            }
          }
        } else {
          ALOGE("%s: QHCI_BEARER_SWITCH_IND Wrong handle value", __func__);
        }
      }
      break;
    case QHCI_VENDOR_ENCODER_LIMIT_CMD:
      {
        uint8_t num_limits = msg->EncoderLimitCmd.num_limit;
        std::vector<uint8_t> encoder_data;
        for (uint8_t i = 0; i < num_limits*3; i++) {
          encoder_data.push_back(msg->EncoderLimitCmd.data[i]);
        }
        qhci_xm_intf.SendEncoderLimitToXm(num_limits,
                                          msg->EncoderLimitCmd.data);
        free(msg->EncoderLimitCmd.data);
        break;
      }
    case QHCI_LOCAL_VER_LE_FEATURES_TO_AC:
      {
        //AC Send the data to AC
        QHciSendLocalVerAndLeFeatToAC();
      }
      break;
    case QHCI_SEND_CREATE_CONNECT_TO_AC:
      {
        ALOGD("%s: QHCI_SEND_CREATE_CONNECT_TO_AC ", __func__);
        XpanQhciAcIf::GetIf()->CreateConnection(msg->LeConnToAc.bd_addr,
                                             msg->LeConnToAc.supervision_tout);
      }
      break;
    case QHCI_SEND_CANCEL_CONNECT_TO_AC:
      {
        ALOGD("%s: QHCI_SEND_CANCEL_CONNECT_TO_AC ", __func__);
        XpanQhciAcIf::
          GetIf()->CreateConnectionCancel(msg->CancelConnToAc.bd_addr);
      }
      break;
    case QHCI_SEND_READ_REMOTE_LE_FEATURES:
      {
        qhci_dev_cb_t* rem_info =
          GetQHciRemoteDeviceInfo(msg->GetLeFeatFromAc.handle);
        if (rem_info) {
          ALOGD("%s: QHCI_SEND_READ_REMOTE_LE_FEATURES handle %d",
                __func__, msg->GetLeFeatFromAc.handle);

          XpanQhciAcIf::GetIf()->GetRemoteLeFeatures(rem_info->rem_addr);
        } else {
          ALOGE("%s: QHCI_SEND_READ_REMOTE_LE_FEATURES Wrong handle value",
                 __func__);
        }
      }
      break;
    case QHCI_SEND_READ_REMOTE_VER_REQ:
      {
        qhci_dev_cb_t* rem_info =
          GetQHciRemoteDeviceInfo(msg->GetVersionFromAc.handle);
        if (rem_info) {
          ALOGD("%s: QHCI_SEND_READ_REMOTE_VER_REQ handle %d",
                __func__, msg->GetVersionFromAc.handle);

          XpanQhciAcIf::GetIf()->GetRemoteVersion(rem_info->rem_addr);
        } else {
          ALOGE("%s: QHCI_SEND_READ_REMOTE_VER_REQ Wrong handle value",
                 __func__);
        }
      }
      break;
    case QHCI_SEND_CONNECTION_CMPL_EVENT:
      {
      }
      break;
    case QHCI_RECV_REMOTE_SUPPORT_LE_FEAT_EVENT:
      {
        uint16_t handle = QHciBDAddrToHandleMap(msg->LeFeatResFromAC.bd_addr);
        ALOGD("%s: QHCI_RECV_REMOTE_VERSION_INFO_EVENT handle %d",
                __func__, handle);
        if (handle != 0) {
          QHciSendLeRemoteFeatureEvent(handle, msg->LeFeatResFromAC.status,
                                      msg->LeFeatResFromAC.featureMask);
        } else {
          ALOGD("%s: QHCI_RECV_REMOTE_VERSION_INFO_EVENT Wrong handle value",
                __func__);
        }
      }

      break;
    case QHCI_RECV_REMOTE_VERSION_INFO_EVENT:
      {
        uint16_t handle = QHciBDAddrToHandleMap(msg->RemoteVerFromAC.bd_addr);
        ALOGD("%s: QHCI_RECV_REMOTE_VERSION_INFO_EVENT handle %d",
                __func__, handle);
        if (handle != 0) {
          QHciSendRemoteVersionCmplEvt(handle, msg->RemoteVerFromAC.version,
                                      msg->RemoteVerFromAC.companyId,
                                      msg->RemoteVerFromAC.subversion);
        } else {
          ALOGD("%s: QHCI_RECV_REMOTE_VERSION_INFO_EVENT Wrong handle value",
                __func__);
        }
      }
      break;
    case QHCI_RECV_NOCP_FROM_AC:
      {
        uint16_t handle = QHciBDAddrToHandleMap(msg->NocpFromAc.bd_addr);
        ALOGD("%s: QHCI_RECV_NOCP_FROM_AC handle %d", __func__, handle);
        if (handle != 0) {
          QHciSendNocpEvent(handle, msg->NocpFromAc.noOfPacketsSent);
        } else {
          ALOGE("%s: QHCI_RECV_NOCP_FROM_AC Wrong handle value", __func__);
        }
      }
      break;
    case QHCI_RECV_CONN_CMPL_FROM_AC:
      {
        //create a handle
        //change the QHCI state AP_ENABLE
        //Send LE Connection cmpl to stack
        if (msg->ConnCmplFromAc.status == QHCI_BT_SUCCESS) {
          ALOGD ("%s qhci_curr_transport %s", __func__,
                  TransportTypeToString(qhci_curr_transport));
          //If No LE transport is present
          if (qhci_curr_transport != BT_LE) {
            //TODO call deviceHandleManagement
            uint16_t handle =
                QHciHm::GetIf()->xpanConnectionCmplt(msg->ConnCmplFromAc.bd_addr);
            qhci_dev_cb_t rem_dev_info;
            rem_dev_info.handle = handle;
            for (int i = 0; i < 6; i++) {
              rem_dev_info.rem_addr.b[i] = msg->ConnCmplFromAc.bd_addr.b[i];
            }
            //TODO Remove during end to end
            uint16_t cur_handle = QHciBDAddrToHandleMap(rem_dev_info.rem_addr);
            ALOGD ("%s Handle_bdaddr_map size %d handle %d", __func__,
                  handle_bdaddr_map_.size(), handle);

            if (cur_handle != 0) {
              ALOGW ("%s cur_handle %d is mapping bdaddr %s",__func__,
                      cur_handle, ConvertRawBdaddress(rem_dev_info.rem_addr));
              handle_bdaddr_map_.erase(cur_handle);
            }

            ALOGD ("%s bdaddr %s",__func__,
                  ConvertRawBdaddress(rem_dev_info.rem_addr));

            handle_bdaddr_map_[handle] = msg->ConnCmplFromAc.bd_addr;
            qhci_ac_active_handle = handle;
            xpan_active_devices_[handle] = rem_dev_info.rem_addr;

            rem_dev_info.tState = QHCI_AP_ENABLE;
            rem_dev_info.qhci_link_transport = XPAN_AP;
            qhci_curr_transport = XPAN_AP;
            qhci_xpan_dev_db.push_back(rem_dev_info);
            QHciLeConnCmplEvent(handle, msg->ConnCmplFromAc.bd_addr,
                                msg->ConnCmplFromAc.status);

            if (dbg_mtp_lib_prop) {
              qhci_xm_intf.RemoteSupportXpanToXm(rem_dev_info.rem_addr,
                                               true);
            }
            curr_active_bd_addr = rem_dev_info.rem_addr;
            qhci_ac_le_acl_prog = false;
          } else {
            //LE Link Already Present
            bdaddr_t bd_addr;
            uint16_t handle =
                QHciHm::GetIf()->xpanConnectionCmplt
                                (msg->ConnCmplFromAc.bd_addr);
            for (int i = 0; i < 6; i++) {
              bd_addr.b[i] = msg->ConnCmplFromAc.bd_addr.b[i];
            }
            uint16_t acl_handle =
                  QHciBDAddrToHandleMap(msg->ConnCmplFromAc.bd_addr);
            qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(acl_handle);

            if (rem_info) {
              rem_info->tState = QHCI_BT_ENABLE_AP_ENABLE;
            }
          }
        } else {
          ALOGW ("%s Connection failed", __func__);
          if (qhci_curr_transport == BT_LE) {
            uint16_t acl_handle =
                QHciBDAddrToHandleMap(msg->ConnCmplFromAc.bd_addr);
            qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(acl_handle);
            if (rem_info && (rem_info->tState == QHCI_BT_ENABLE_AP_CONNECTING ||
                rem_info->tState == QHCI_BT_ENABLE_AP_ENABLE)) {
              rem_info->tState = QHCI_BT_ENABLE;
            }
          } else {
            QHciLeConnCmplEvent(0, msg->ConnCmplFromAc.bd_addr,
                                msg->ConnCmplFromAc.status);
          }
        }
      }
      break;
    case QHCI_SEND_LE_ENCRYPT_CMD_TO_AC:
      {
        qhci_dev_cb_t* rem_info =
          GetQHciRemoteDeviceInfo(msg->LeEncrptCmdToAc.handle);
        if (rem_info) {
          ALOGD("%s: QHCI_SEND_LE_ENCRYPT_CMD_TO_AC handle %d",
                __func__, msg->LeEncrptCmdToAc.handle);
          std::vector<uint8_t> ltk(std::begin(msg->LeEncrptCmdToAc.ltk),
                                    std::end(msg->LeEncrptCmdToAc.ltk));
          XpanQhciAcIf::GetIf()->EnableEncrption(rem_info->rem_addr, ltk);
        } else {
          ALOGE("%s: QHCI_SEND_LE_ENCRYPT_CMD_TO_AC Wrong handle value",
                 __func__);
        }
      }
      break;
    case QHCI_RECV_ENCRYPT_CMPL_FROM_AC:
      {
        uint16_t handle = QHciBDAddrToHandleMap(msg->EncrResFromAc.bd_addr);
        ALOGD("%s: QHCI_RECV_ENCRYPT_CMPL_FROM_AC handle %d", __func__,
                handle);
        QHciSendEncryCmplEvent(handle, msg->EncrResFromAc.status,
                                msg->EncrResFromAc.encr_enable);
      }
      break;
    case QHCI_XPAN_BONDED_DEVICE_LIST:
      {
        qhci_xpan_bonded_list.num_devices = msg->BondedDevList.num_devices;
        for (int i =0; i < msg->BondedDevList.num_devices ; i++) {
          memcpy(&qhci_xpan_bonded_list.bonded_devices[i],
                  &msg->BondedDevList.bonded_devices[i], sizeof(bdaddr_t));
          ALOGD("%s: QHCI_XPAN_BONDED_DEVICE_LIST[%d]: %s", __func__, i,
                ConvertRawBdaddress(qhci_xpan_bonded_list.bonded_devices[i]));
        }
        free(msg->BondedDevList.bonded_devices);
      }
      break;
    case QHCI_SEND_DISCONNECT_TO_AC:
      {
        ALOGD("%s: QHCI_SEND_DISCONNECT_TO_AC ", __func__);
        XpanQhciAcIf::GetIf()->DisconnectConnection(msg->DisconnToAc.bd_addr);
      }
      break;
    case QHCI_RECV_DISCONNECTION_CMPL_EVENT:
      {
        uint16_t handle =
          QHciBDAddrToHandleMap(msg->DisconnCmplFromAc.bd_addr);
        ALOGD("%s: Event: %s handle %d", __func__,
               ConvertIpcEventToString(eventId), handle);

        QHciHm::GetIf()->xpanDisconnectionCmplt(msg->DisconnCmplFromAc.bd_addr);
        qhci_ac_active_handle = 0;

        qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);
        if (rem_info) {
          ALOGD("QHCI_STATE: %s", ConvertStatetoString(rem_info->tState));
          if ((rem_info->tState != QHCI_BT_ENABLE) &&
              (rem_info->tState != QHCI_BT_ENABLE_AP_CONNECTING) &&
              (rem_info->tState != QHCI_BT_ENABLE_AP_ENABLE)) {
            QHciSendDisconnectCmplt(handle, msg->DisconnCmplFromAc.status);
            rem_info->tState = QHCI_TRANSPORT_IDLE_STATE;
            curr_active_bd_addr = {};
            QHciLeConnCleanup(rem_info);
            xpan_active_devices_.erase(handle);
            handle_bdaddr_map_.erase(handle);
          }
          if (rem_info->tState == QHCI_BT_ENABLE_AP_ENABLE) {
            QHciHandleBtEnableApEnable(rem_info, QHCI_RECV_DISCONNECTION_CMPL_EVENT);
            rem_info->tState = QHCI_BT_ENABLE;
          }
        } else {
          ALOGE("%s QHCI is in BAD state for "
                "QHCI_RECV_DISCONNECTION_CMPL_EVENT", __func__);
        }
      }
      break;
    case QHCI_RECV_ACL_DATA:
      {
        ALOGD("%s: QHCI_RECV_ACL_DATA ", __func__);
        uint16_t handle =
          QHciBDAddrToHandleMap(msg->RxDataFromAc.bd_addr);
        if (handle != 0) {
          QHciSendAcRxDataToStack(handle, msg->RxDataFromAc.rx_data,
                                  msg->RxDataFromAc.len);
        } else {
          ALOGE("%s: QHCI_RECV_ACL_DATA Wrong handle value",
                 __func__);
        }
        free(msg->RxDataFromAc.rx_data);
      }
      break;
    case QHCI_RECV_CONNECT_LE_REQ_FROM_XM:
      {
        uint16_t handle = QHciBDAddrToHandleMap(msg->LeConnFromXm.bd_addr);
        if (handle != 0) {
          ALOGD("%s: QHCI_RECV_CONNECT_LE_REQ_FROM_XM handle %d", __func__,
               handle);
          qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);
          if (rem_info) {
            ALOGD("%s QHCI_RECV_CONNECT_LE_REQ_FROM_XM QHCI_STATE: %s", __func__,
                    ConvertStatetoString(rem_info->tState));
            if (rem_info->tState == QHCI_AP_ENABLE) {
              QHciHandleApEnable(rem_info, QHCI_CSM_BT_CONNECT_REQ_EVT);
            } else if (rem_info->tState == QHCI_BT_ENABLE_AP_ENABLE) {
              qhci_xm_intf.connectLELinkRspToXm(rem_info->rem_addr ,
                                                XM_SUCCESS);
            } else {
              qhci_xm_intf.connectLELinkRspToXm(msg->LeConnFromXm.bd_addr,
                                                XM_FAILED);
            }
          } else {
            ALOGE("%s: QHCI_RECV_CONNECT_LE_REQ_FROM_XM  is in bad state"
                  "for handle %d", __func__, handle);
            qhci_xm_intf.connectLELinkRspToXm(msg->LeConnFromXm.bd_addr,
                                              XM_FAILED);
          }
        } else {
          ALOGE("%s: QHCI_RECV_CONNECT_LE_REQ_FROM_XM  Wrong handle value",
                 __func__);
          qhci_xm_intf.connectLELinkRspToXm(msg->LeConnFromXm.bd_addr,
                                             XM_FAILED);
        }
      }
      break;
    case QHCI_L2CAP_PAUSE_UNPAUSE_REQ:
      {
        uint16_t handle =
                  QHciBDAddrToHandleMap(msg->L2capPauseUnPauseReq.bd_addr);
        qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);
        uint8_t pause = msg->L2capPauseUnPauseReq.pause;
        ALOGD("Transport Type Request: ",
              TransportTypeToString(msg->L2capPauseUnPauseReq.transportType));

        if (!pause) {
          if (rem_info) {
            rem_info->qhci_link_transport =
              msg->L2capPauseUnPauseReq.transportType;
          }
          qhci_curr_transport = msg->L2capPauseUnPauseReq.transportType;
        }
        XpanQhciAcIf::GetIf()->L2capPauseUnpauseRes(
                               msg->L2capPauseUnPauseReq.bd_addr,
                               pause,
                               0);

      }
      break;
    case QHCI_WIFI_SCAN_STARTED_IND:
      {
        uint16_t handle =
                QHciBDAddrToHandleMap(msg->WifiScanStartedInd.bd_addr);
        qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);

        if (rem_info) {
          ALOGD("%s QHCI_WIFI_SCAN_STARTED_IND QHCI_STATE: %s", __func__,
                    ConvertStatetoString(rem_info->tState));
          if ((rem_info->tState == QHCI_BT_ENABLE) ||
            (rem_info->tState == QHCI_BT_ENABLE_AP_CONNECTING) ||
            (rem_info->tState == QHCI_P2P_ENABLE)) {
            QHciXPanBearerTransitionCmdToSoc(QHCI_TRANSTITION_INIT, handle,
                                             QHCI_TRANSTITION_LE_AP);
            SET_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
          }
        }
      }
      break;
    default:
      ALOGD("%s: UnKnown Event ", __func__);
  }
  free(msg);

}

void QHci::PostQHciStateChange(uint16_t handle,
                                       QHCI_CSM_STATE state,
                                       QHCI_CSM_EVENT qhci_event) {
  ALOGD("%s: ", __func__);
  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH);
  msg->eventId = QHCI_STATE_CHANGE;
  msg->StateChangeEvt.eventId = QHCI_STATE_CHANGE;
  msg->StateChangeEvt.cis_handle = handle;
  msg->StateChangeEvt.state = (uint8_t) state;
  msg->StateChangeEvt.csm_event = (uint8_t) qhci_event;
  PostMessage(msg);

}

void QHci::QHciHandleIdleState(qhci_dev_cb_t *rem_info, uint8_t event) {
  //Handle the parameters in Idle state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  switch (event)
  {
    case QHCI_CSM_LE_CONN_CMPL_EVT:
      {
        rem_info->state = QHCI_BT_CLOSE_XPAN_CLOSE;
        rem_info->is_xpan_device = true;
        is_xpan_supported = true;
        if (dbg_direct_ap_prop)
          dbg_mtp_lib_prop = true;
        //rem_info->transport_enable = true;
        qhci_xm_intf.RemoteSupportXpanToXm(rem_info->rem_addr,
                                           rem_info->is_xpan_device);
      }
      break;
    default:
      ALOGE("%s: Invalid Event in this state", __func__);
  }
}

void QHci::QHciHandleTransportIdleState(qhci_dev_cb_t *rem_info,
                                                   uint8_t event) {
  //Handle the parameters in Idle state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  switch (event)
  {
    case QHCI_CSM_LE_CONN_CMPL_EVT:
      {
        rem_info->tState = QHCI_BT_ENABLE;
        rem_info->is_xpan_device = true;
        is_xpan_supported = true;
        if (dbg_direct_ap_prop)
          dbg_mtp_lib_prop = true;
        qhci_xm_intf.RemoteSupportXpanToXm(rem_info->rem_addr,
                                           rem_info->is_xpan_device);
      }
      break;
    default:
      ALOGE("%s: Invalid Event in this state", __func__);
  }
}

/******************************************************************
 *
 * Function       QHciHandleBtEnable
 *
 * Description    Handling the QHciHandleBtEnable
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleBtEnable(qhci_dev_cb_t *rem_info, uint8_t event) {
  //Handle the parameters in QHciHandleBtCloseXpanClose state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));
  ALOGD("%s:  CIS_STATE %s ", __func__,
            ConvertCisStatetoString(rem_info->cis_state));

  switch (event)
  {
    case QHCI_CSM_P2P_TRANSPORT_ENABLE_EVT:
      {
        if (rem_info->cis_state == QHCI_CIS_CLOSE) {
          rem_info->tState = QHCI_P2P_ENABLE;
        }
      }
      break;
    case QHCI_CSM_P2P_PREPARE_BEARER_EVT:
      {
        ALOGD("%s:  QHCI_CSM_P2P_PREPARE_BEARER_EVT ", __func__);
        if (rem_info->cis_state == QHCI_CIS_OPEN) {
          rem_info->tState = QHCI_P2P_ENABLE_BT_ENABLE;
          if (!dbg_mtp_mora_prop) {
            qhci_xm_intf.PrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                  XPAN_P2P);
          } else {
            qhci_xm_intf.PrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                  XPAN_P2P);
          }
        } else {
          ALOGE("%s: Invalid CIS State in this state", __func__);
        }
      }
      break;
    case QHCI_CSM_LE_CLOSE_EVT:
      {
        rem_info->tState = QHCI_TRANSPORT_IDLE_STATE;
        rem_info->current_transport = DEFAULT;
        qhci_curr_transport = DEFAULT;
        if ((rem_info->cis_state == QHCI_CIS_OPEN) ||
            (rem_info->cis_state == QHCI_CIS_CONNECTING)) {
            // check any things to make it clean
            rem_info->cis_state = QHCI_CIS_CLOSE;
        }
        QHciLeConnCleanup(rem_info);
      }
      break;
    case QHCI_CSM_PREPARE_BEARER_XPAN_AP_REQ:
      {
        // Respond to prepare Bearer Request to XPAN.
        rem_info->tState = QHCI_BT_ENABLE_AP_CONNECTING;
        qhci_bearer_switch_pending = true;
        rem_info->is_prepare_bearer_triggered = true;
        uint16_t handle = QHciBDAddrToHandleMap(rem_info->rem_addr);
        QHciXPanBearerTransitionCmdToSoc(QHCI_TRANSTITION_STARTED, handle,
                                       QHCI_TRANSTITION_LE_AP);
        SET_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
        qhci_xm_intf.PrepareAudioBearerRspToXm(rem_info->rem_addr, XM_SUCCESS);

      }
      break;

    default:
      ALOGE("%s: Invalid Event in this state", __func__);
  }
}

/******************************************************************
 *
 * Function       QHciHandleP2PEnable
 *
 * Description    Handling the QHciHandleP2PEnable
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleP2PEnable(qhci_dev_cb_t *rem_info, uint8_t event) {
  //Handle the parameters in QHciHandleBtCloseXpanClose state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  switch (event)
  {
    case QHCI_CSM_P2P_TRANSPORT_DISABLE_EVT:
      {
        ALOGD("%s:  CIS_STATE %d ", __func__, rem_info->cis_state);
        if (rem_info->cis_state == QHCI_CIS_CLOSE) {
          rem_info->tState = QHCI_BT_ENABLE;
        }
      }
      break;
    case QHCI_CSM_BT_PREPARE_BEARER_EVT:
      {
        ALOGD("%s:  QHCI_CSM_CIS_CLOSE_XPAN_TRANS_ENABLE_EVT ", __func__);
        if (rem_info->cis_state == QHCI_CIS_OPEN) {
          rem_info->tState = QHCI_P2P_ENABLE_BT_ENABLE;

          rem_info->is_create_cis_from_qhci_to_soc = true;
          qhci_bearer_switch_pending = true;
          //Send Create CIS to Soc
          ALOGE("%s:  QHCI_CSM_PREPARE_BEARER_BT qhci_bearer_switch_pending"
                  "%d", __func__, qhci_bearer_switch_pending);
          SendCreateCisToSoc(rem_info);
        } else {
          ALOGE("%s: Invalid CIS State in this state", __func__);
        }
      }
      break;
    case QHCI_CSM_LE_CLOSE_EVT:
      {
        rem_info->tState = QHCI_TRANSPORT_IDLE_STATE;
        rem_info->current_transport = DEFAULT;
        qhci_curr_transport = DEFAULT;
        if ((rem_info->cis_state == QHCI_CIS_OPEN) ||
            (rem_info->cis_state == QHCI_CIS_CONNECTING)) {
            rem_info->cis_state = QHCI_CIS_CLOSE;
          //Sending Unprepare Audio Bearer to XPAN Manager
          if (!dbg_mtp_mora_prop) {
            qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                     NONE);
          } else {
            qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                     NONE);
          }
        }
        QHciLeConnCleanup(rem_info);
      }
      break;
      case QHCI_CSM_CIS_DISCONNECT_EVT:
        {
          QHciSetCisState(rem_info, QHCI_CIS_CLOSE);
          //This Event will be triggered when the stream is stopped
          rem_info->tState = QHCI_P2P_ENABLE;
          //Sending Unprepare Audio Bearer to XPAN Manager
          if ((rem_info->cis_state == QHCI_CIS_OPEN) ||
            (rem_info->cis_state == QHCI_CIS_CONNECTING)) {
            if (!dbg_mtp_mora_prop) {
              qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                       NONE);
            } else {
              qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                       NONE);
            }
          }
        }
        break;
      case QHCI_CSM_P2P_PREPARE_BEARER_FAIL_EVT:
      {
        ALOGD("%s:  QHCI_CSM_P2P_PREPARE_BEARER_FAIL_EVT ", __func__);
        if ((rem_info->cis_state == QHCI_CIS_OPEN) ||
            (rem_info->cis_state == QHCI_CIS_CONNECTING)){
          rem_info->tState = QHCI_BT_ENABLE;

          rem_info->is_create_cis_from_qhci_to_soc = true;
          qhci_bearer_switch_pending = true;
          //Send Create CIS to Soc
          ALOGE("%s:  QHCI_CSM_PREPARE_BEARER_BT qhci_bearer_switch_pending"
                "%d", __func__, qhci_bearer_switch_pending);
          SendCreateCisToSoc(rem_info);
        } else {
          ALOGE("%s: Invalid CIS State in this state", __func__);
        }
      }
      break;
    default:
      ALOGE("%s: Invalid Event in this state", __func__);
  }
}

/******************************************************************
 *
 * Function       QHciHandleP2PEnableBtEnable
 *
 * Description    Handling the QHciHandleP2PEnableBtEnable
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleP2PEnableBtEnable(qhci_dev_cb_t *rem_info,
                                                  uint8_t event) {
  //Handle the parameters in QHciHandleBtCloseXpanClose state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  switch (event)
  {
    case QHCI_CSM_P2P_TRANSPORT_DISABLE_EVT:
      {
        ALOGD("%s:  CIS_STATE %d ", __func__, rem_info->cis_state);
        if (rem_info->cis_state == QHCI_CIS_CLOSE) {
          rem_info->tState = QHCI_BT_ENABLE;
        }
      }
      break;
    case QHCI_CSM_BT_PREPARE_BEARER_EVT:
      {
        ALOGD("%s:  QHCI_CSM_CIS_CLOSE_XPAN_TRANS_ENABLE_EVT ", __func__);
        if (rem_info->cis_state == QHCI_CIS_OPEN) {
          rem_info->tState = QHCI_P2P_ENABLE_BT_ENABLE;

          rem_info->is_create_cis_from_qhci_to_soc = true;
          qhci_bearer_switch_pending = true;
          //Send Create CIS to Soc
          ALOGE("%s:  QHCI_CSM_PREPARE_BEARER_BT qhci_bearer_switch_pending"
                  "%d", __func__, qhci_bearer_switch_pending);
          SendCreateCisToSoc(rem_info);
        } else {
          ALOGE("%s: Invalid CIS State in this state", __func__);
        }
      }
      break;
    case QHCI_CSM_LE_CLOSE_EVT:
      {
        rem_info->tState = QHCI_TRANSPORT_IDLE_STATE;
        if ((rem_info->cis_state == QHCI_CIS_OPEN) ||
            (rem_info->cis_state == QHCI_CIS_CONNECTING)) {
          //Sending Unprepare Audio Bearer to XPAN Manager
          if (!dbg_mtp_mora_prop) {
            qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                     NONE);
          } else {
            qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                     NONE);
          }
        }
        QHciLeConnCleanup(rem_info);
      }
      break;
      case QHCI_CSM_CIS_DISCONNECT_EVT:
        {
          QHciSetCisState(rem_info, QHCI_CIS_CLOSE);
          //This Event will be triggered when the stream is stopped
          rem_info->tState = QHCI_P2P_ENABLE;
          //Sending Unprepare Audio Bearer to XPAN Manager
          if ((rem_info->cis_state == QHCI_CIS_OPEN) ||
            (rem_info->cis_state == QHCI_CIS_CONNECTING)) {
            if (!dbg_mtp_mora_prop) {
              qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                       NONE);
            } else {
              qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                       NONE);
            }
          }
        }
        break;
      case QHCI_CSM_P2P_PREPARE_BEARER_FAIL_EVT:
      {
        ALOGD("%s:  QHCI_CSM_P2P_PREPARE_BEARER_FAIL_EVT ", __func__);
        if ((rem_info->cis_state == QHCI_CIS_OPEN) ||
            (rem_info->cis_state == QHCI_CIS_CONNECTING)){
          rem_info->tState = QHCI_BT_ENABLE;

          rem_info->is_create_cis_from_qhci_to_soc = true;
          qhci_bearer_switch_pending = true;
          //Send Create CIS to Soc
          ALOGE("%s:  QHCI_CSM_PREPARE_BEARER_BT qhci_bearer_switch_pending"
                "%d", __func__, qhci_bearer_switch_pending);
          SendCreateCisToSoc(rem_info);
        } else {
          ALOGE("%s: Invalid CIS State in this state", __func__);
        }
      }
    default:
      ALOGE("%s: Invalid Event in this state", __func__);
  }
}

/******************************************************************
 *
 * Function       QHciHandleApEnable
 *
 * Description    Handling the QHciHandleApEnable
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleApEnable(qhci_dev_cb_t *rem_info, uint8_t event) {
  //Handle the parameters in QHciHandleBtCloseXpanClose state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  if (rem_info) {
    ALOGD("%s:  CIS_STATE %d ", __func__, rem_info->cis_state);

    if (event == QHCI_CSM_AP_LE_DISCONNECT_EVT) {
      //moving to IDLE State
      rem_info->tState = QHCI_TRANSPORT_IDLE_STATE;
      rem_info->current_transport = DEFAULT;
      qhci_curr_transport = DEFAULT;
      return;
    }

    switch (event) {
      case QHCI_CSM_BT_CONNECT_REQ_EVT:
        {
          //Here stream present. Initiate BT Transport
          rem_info->tState = QHCI_AP_ENABLE_BT_CONNECTING;
          //initiate Create LE Connection to Soc
          QHciLeCreateConnectToSoc(rem_info->rem_addr);
        }
        break;
      case QHCI_CSM_PREPARE_BEARER_XPAN:
        {
          ALOGD("%s:   dbg_mtp_lib_prop %d ", __func__, dbg_mtp_lib_prop);
          if (!dbg_mtp_lib_prop) {
            active_xpan_device = {};
            qhci_xm_intf.PrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                    XPAN_AP);
          } else {
            qhci_xm_intf.PrepareAudioBearerReqToXm(curr_active_bd_addr,
                                                    XPAN_AP);
            debug_lib_bd_addr = rem_info->rem_addr;
          }
        }
        break;
    case QHCI_CSM_CIS_DISCONNECT_EVT:
      {
        QHciSetCisState(rem_info, QHCI_CIS_CLOSE);
        //This Event will be triggered when the stream is stopped
        //Sending Unprepare Audio Bearer to XPAN Manager
        if (!dbg_mtp_lib_prop) {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                   NONE);
        } else {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(curr_active_bd_addr,
                                                   NONE);
        }
        qhci_bearer_switch_pending = false;
      }
      break;
    case QHCI_CSM_LE_CONN_CMPL_EVT:
      {
        rem_info->tState = QHCI_BT_ENABLE_AP_ENABLE;
      }
      break;
      default:
        ALOGD("%s: Invalid Event in CIS OPEN/CIS CONNECTING State", __func__);
    }
  }

}

/******************************************************************
 *
 * Function       QHciHandleApEnableBtConnecting
 *
 * Description    Handling the QHciHandleApEnableBtConnecting
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleApEnableBtConnecting(qhci_dev_cb_t *rem_info,
                                                      uint8_t event) {
  //Handle the parameters in QHciHandleBtCloseXpanClose state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  if (rem_info) {
    ALOGD("%s:  CIS_STATE %d ", __func__, rem_info->cis_state);

    if (rem_info->cis_state == QHCI_CIS_OPEN) {
      switch(event) {
        case QHCI_CSM_AP_LE_DISCONNECT_EVT:
        {
          //move to the idle State
          rem_info->tState = QHCI_TRANSPORT_IDLE_STATE;
        }
        break;
        case QHCI_CSM_BT_BEARER_SWITCH_IND_FAIL_EVT:
        {
          // Go back to AP state
          rem_info->tState = QHCI_AP_ENABLE;
          if (rem_info->le_acl_connected) {
            uint16_t handle = QHciBDAddrToHandleMap(rem_info->rem_addr);
            ALOGD("%s:  Disconnect the LE ACL link with handle %d", __func__,
                   handle);
            if (handle != 0) {
              QHciPrepareAndSendHciDisconnect(handle);
            } else {
              ALOGD("%s:  QHCI is in bad state with handle %d", __func__,
                   handle);
            }
          }
        }
        break;
        case QHCI_CSM_BT_BEARER_SWITCH_IND_EVT:
        {
          //Go to BT state
          rem_info->tState = QHCI_BT_ENABLE;

          //Disconnect AP connection
          XpanQhciAcIf::GetIf()->DisconnectConnection(rem_info->rem_addr);
        }
        break;
        #if 0
        case QHCI_CSM_LE_CONN_CMPL_EVT:
        {
          //No change in state
          rem_info->tState = QHCI_AP_ENABLE_BT_CONNECTING;
          rem_info->le_acl_connected = true;
          //Trigger Create CIS
          SendCreateCisToSoc(rem_info);
        }
        break;
        #endif
        case QHCI_CSM_CIS_ESTABLISH_EVT:
        {
          RspStatus status = XM_SUCCESS;
          //Send PrepareAudioBearerResponse to XM
          qhci_xm_intf.PrepareAudioBearerRspToXm(rem_info->rem_addr, status);
          //Stay in current state
          rem_info->tState = QHCI_AP_ENABLE_BT_CONNECTING;
        }
        break;
        case QHCI_CSM_LE_CONN_CMPL_EVT:
        {
          rem_info->tState = QHCI_BT_ENABLE_AP_ENABLE;
          qhci_xm_intf.connectLELinkRspToXm(rem_info->rem_addr , XM_SUCCESS);
          //TODO Handle Failure scenario in separate event
        }
        break;
        default:
          ALOGE("%s: Invalid event for this CIS state", __func__);
      }
    } else if (rem_info->cis_state == QHCI_CIS_CLOSE) {
      switch(event) {
        case QHCI_CSM_LE_CONN_CMPL_EVT:
        {
          rem_info->tState = QHCI_BT_ENABLE_AP_ENABLE;
          qhci_xm_intf.connectLELinkRspToXm(rem_info->rem_addr , XM_SUCCESS);
          //TODO Handle Failure scenario in separate event
        }
        break;
        case QHCI_CSM_AP_LE_DISCONNECT_EVT:
        {
        }
        break;
        default:
          ALOGE("%s: Invalid event for this CIS state", __func__);
      }
    } else {
      ALOGE("%s: Invalid CIS state for this state", __func__);
    }
  }

}

/******************************************************************
 *
 * Function       QHciHandleBtEnableApConnecting
 *
 * Description    Handling the QHciHandleBtEnableApConnecting
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleBtEnableApConnecting(qhci_dev_cb_t *rem_info,
                                                      uint8_t event) {
  //Handle the parameters in QHciHandleBtCloseXpanClose state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  if (rem_info) {
    ALOGD("%s:  CIS_STATE %d ", __func__, rem_info->cis_state);

    if (rem_info->cis_state == QHCI_CIS_CLOSE) {
    }

    switch (event) {
      case QHCI_CSM_AP_BEARER_SWITCH_IND_FAIL_EVT:
      {
        rem_info->tState = QHCI_BT_ENABLE;
        qhci_bearer_switch_pending = false;
        rem_info->is_prepare_bearer_triggered = false;
        QHciXPanBearerTransitionCmdToSoc(QHCI_TRANSTITION_FAILED, rem_info->handle,
                                         QHCI_TRANSTITION_LE_AP);
        SET_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
      } break;
    }
  }

}

/******************************************************************
 *
 * Function       QHciHandleBtEnableApEnable
 *
 * Description    Handling the QHciHandleBtEnableApEnable
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleBtEnableApEnable(qhci_dev_cb_t *rem_info,
                                                      uint8_t event) {
  //Handle the parameters in QHciHandleBtCloseXpanClose state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  if (rem_info) {
    ALOGD("%s:  CIS_STATE %s ", __func__,
            ConvertCisStatetoString(rem_info->cis_state));

    switch(event) {
      case QHCI_CSM_PREPARE_BEARER_XPAN:
        {
          ALOGD("%s:   dbg_mtp_lib_prop %d ", __func__, dbg_mtp_lib_prop);
          if (!dbg_mtp_lib_prop) {
            active_xpan_device = {};
            qhci_xm_intf.PrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                    XPAN_AP);
          } else {
            qhci_xm_intf.PrepareAudioBearerReqToXm(curr_active_bd_addr,
                                                    XPAN_AP);
            debug_lib_bd_addr = rem_info->rem_addr;
          }
        }
        break;
      case QHCI_CSM_PREPARE_BEARER_XPAN_AP_REQ:
      {
        uint16_t handle = QHciBDAddrToHandleMap(rem_info->rem_addr);
        //Respond to prepare Bearer Request to XPAN.
        rem_info->tState = QHCI_BT_ENABLE_AP_ENABLE;
        qhci_bearer_switch_pending = true;
        rem_info->is_prepare_bearer_triggered = true;
        //QHciXPanBearerTransitionCmdToSoc(QHCI_TRANSTITION_STARTED, handle,
        //                                 QHCI_TRANSTITION_LE_AP);
        //SET_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
        qhci_xm_intf.PrepareAudioBearerRspToXm(rem_info->rem_addr, XM_SUCCESS);
      }
        break;
      case QHCI_CSM_LE_CLOSE_EVT:
      {
        rem_info->tState = QHCI_AP_ENABLE;
      }
        break;
      case QHCI_CSM_PREPARE_BEARER_BT:
      {
        uint16_t handle = QHciBDAddrToHandleMap(rem_info->rem_addr);
        //Respond to prepare Bearer Request to XPAN.
        rem_info->tState = QHCI_BT_ENABLE_AP_ENABLE;
        qhci_bearer_switch_pending = true;
        rem_info->is_prepare_bearer_triggered = true;
        SendCreateCisToSoc(rem_info);
      }
        break;
      case QHCI_CSM_AP_BEARER_SWITCH_IND_EVT:
      {
        qhci_bearer_switch_pending = false;
        rem_info->is_prepare_bearer_triggered = false;
        if (rem_info->cis_state == QHCI_CIS_OPEN) {
          uint16_t handle = QHciBDAddrToHandleMap(rem_info->rem_addr);
          QHciXPanBearerTransitionCmdToSoc(QHCI_TRANSTITION_COMPLETE, handle,
                                              QHCI_TRANSTITION_LE_AP);
          SET_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
            //Disconnect the CISs with SOC
          QHciSendDisconnectCisToSoc(rem_info);
        }
      }
        break;
      case QHCI_RECV_DISCONNECTION_CMPL_EVENT:
      {
          QHciXPanBearerTransitionCmdToSoc(QHCI_TRANSTITION_FAILED, rem_info->handle,
                                           QHCI_TRANSTITION_LE_AP);
          SET_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
      }
        break;
      case QHCI_CSM_AP_BEARER_SWITCH_IND_FAIL_EVT:
      {
        rem_info->tState = QHCI_BT_ENABLE;
        qhci_bearer_switch_pending = false;
        rem_info->is_prepare_bearer_triggered = false;
        QHciXPanBearerTransitionCmdToSoc(QHCI_TRANSTITION_FAILED, rem_info->handle,
                                         QHCI_TRANSTITION_LE_AP);
        SET_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
      }
        break;
      case QHCI_CSM_BT_BEARER_SWITCH_IND_EVT:
      {
        if (rem_info->cis_state == QHCI_CIS_OPEN) {
          qhci_bearer_switch_pending = false;
          rem_info->is_prepare_bearer_triggered = false;
          uint16_t handle = QHciBDAddrToHandleMap(rem_info->rem_addr);
          QHciXPanBearerTransitionCmdToSoc(QHCI_TRANSTITION_COMPLETE, handle,
                                               QHCI_TRANSTITION_AP_LE);
          SET_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
        }
      }
        break;
      case QHCI_CSM_BT_BEARER_SWITCH_IND_FAIL_EVT:
      {
        qhci_bearer_switch_pending = false;
        rem_info->is_prepare_bearer_triggered = false;
        uint16_t handle = QHciBDAddrToHandleMap(rem_info->rem_addr);
        QHciSendDisconnectCisToSoc(rem_info);
        QHciXPanBearerTransitionCmdToSoc(QHCI_TRANSTITION_FAILED, handle,
                                               QHCI_TRANSTITION_AP_LE);
        SET_BIT(qhci_hci_cmd_wait, BIT_VENDOR_XPAN_BEARER_CMD);
      }
        break;
      case QHCI_CSM_CIS_ESTABLISH_EVT:
      {
        RspStatus status = XM_SUCCESS;
        //Send PrepareAudioBearerResponse to XM
        qhci_xm_intf.PrepareAudioBearerRspToXm(rem_info->rem_addr, status);
        //Stay in current state
        rem_info->tState = QHCI_BT_ENABLE_AP_ENABLE;
      }
        break;
      case QHCI_CSM_LE_TX_DISCONN_EVT:
      {
        uint16_t le_handle =
          QHciHm::GetIf()->GetLeHandleFromStackHandle(rem_info->handle);
        ALOGE("%s: LE Handle %d", __func__, le_handle);
        if (le_handle != 0) {
          QHciPrepareAndSendHciDisconnect(le_handle);
          QHciSendDisconnectCmdStatus();
        }
        QHciSendDisconnectToAc(rem_info->handle);
      }
        break;
      case QHCI_CSM_CIS_DISCONNECT_EVT:
      {
        QHciSetCisState(rem_info, QHCI_CIS_CLOSE);
        //This Event will be triggered when the stream is stopped
        //Sending Unprepare Audio Bearer to XPAN Manager
        if (!dbg_mtp_lib_prop) {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                   NONE);
        } else {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(curr_active_bd_addr,
                                                   NONE);
        }
        qhci_bearer_switch_pending = false;

      }
        break;
      default:
        ALOGE("%s: Invalid event for this state", __func__);
    }
  }

}

/******************************************************************
 *
 * Function       QHciSetHostSupportedBit
 *
 * Description    Sending Set XPAN Host Supported Bit
 *
 * Arguments      none
 *
 * return         none
 ******************************************************************/
void QHci::QHciSetHostSupportedBit() {
  ALOGE("%s ", __func__);

  uint16_t length = QHCI_SET_HOST_SUPPORTED_BIT_COMMAND_LEN;

  uint8_t *data = (uint8_t *)malloc(length * sizeof(uint8_t));

  data[0] = 0x51;
  data[1] = 0xFC;
  data[2] = 3;
  data[3] = HCI_SUB_OPCODE_QLE_SET_HOST_FEATURE;
  //BIT Position
  data[4] = 0x3D;
  data[5] = 0x01;

  qhci_set_host_bit = true;
  DataHandler *data_handler = DataHandler::Get();

  if (data_handler) {
    if (data_handler->GetControllerRef() != nullptr) {
      data_handler->UpdateRingBufferFromQHci(HCI_PACKET_TYPE_COMMAND,
                              data,
                              QHCI_SET_HOST_SUPPORTED_BIT_COMMAND_LEN);
      ALOGE("%s Sending data to the soc", __func__);
      data_handler->GetControllerRef()->SendPacket(HCI_PACKET_TYPE_COMMAND,
                                                   data,
                                                   QHCI_SET_HOST_SUPPORTED_BIT_COMMAND_LEN);
      }
  }
  free((uint8_t *)data);
  return;
}


/******************************************************************
 *
 * Function       QHciLeConnCleanup
 *
 * Description    cleaning up the remote device data
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *
 * return         none
 ******************************************************************/
void QHci::QHciLeConnCleanup(qhci_dev_cb_t *rem_info) {
  QHciClearRemoteDeviceInfo(rem_info->handle);
}

/******************************************************************
 *
 * Function       QHciHandleBtCloseXpanClose
 *
 * Description    Handling the QHCI_BT_CLOSE_XPAN_CLOSE
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleBtCloseXpanClose(qhci_dev_cb_t *rem_info, uint8_t event) {
  //Handle the parameters in QHciHandleBtCloseXpanClose state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  switch (event)
  {
    case QHCI_CSM_CIS_OPEN_XPAN_TRANS_DISABLE_EVT:
      {      
        ALOGD("%s:  QHCI_CSM_CIS_OPEN_XPAN_TRANS_DISABLE_EVT ", __func__);
        rem_info->state = QHCI_BT_OPEN_XPAN_CLOSE;
        rem_info->tState = QHCI_BT_ENABLE;
        //Create CIS is triggered from the stack
        rem_info->is_create_cis_from_stack = false;
        qhci_xm_intf.AudioTransportUpdateToXm(rem_info->rem_addr, BT_LE);
        if (rem_info->is_create_cis_from_stack) {
          //Update the params if anything needed
        } else {
          ALOGE("%s:  Invalid call from QHCI module", __func__);
        }
      }
      break;
    case QHCI_CSM_CIS_CLOSE_XPAN_TRANS_ENABLE_EVT:
      {
        ALOGD("%s:  QHCI_CSM_CIS_CLOSE_XPAN_TRANS_ENABLE_EVT ", __func__);
        rem_info->state = QHCI_BT_CLOSE_XPAN_CONNECTING;
        //Sending prepare Bearer Request to XPAN Manager
        if (!dbg_mtp_mora_prop) {
          active_xpan_device = {};
          qhci_xm_intf.PrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                  XPAN_P2P);
        } else {
          qhci_xm_intf.PrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                  XPAN_P2P);
        }
      }
      break;
    case QHCI_CSM_LE_CLOSE_EVT:
      {
        rem_info->state = QHCI_IDLE_STATE;
        rem_info->tState = QHCI_TRANSPORT_IDLE_STATE;
        rem_info->current_transport = DEFAULT;
        qhci_curr_transport = DEFAULT;
        if ((rem_info->cis_state == QHCI_CIS_OPEN) ||
            (rem_info->cis_state == QHCI_CIS_CONNECTING)) {
            // check any things to make it clean
            rem_info->cis_state = QHCI_CIS_CLOSE;
        }
        QHciLeConnCleanup(rem_info);
      }
      break;
    default:
      ALOGE("%s: Invalid Event in this state", __func__);
  }

}

/******************************************************************
 *
 * Function       QHciHandleBtCloseXpanConnecting
 *
 * Description    Handling the QHCI_BT_CLOSE_XPAN_CONNECTING
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleBtCloseXpanConnecting(qhci_dev_cb_t *rem_info,
                                                       uint8_t event) {
  //Handle the parameters in QHciHandleBtCloseXpanConnecting state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  switch(event)
  {
    case QHCI_CSM_UPDATE_TRANS_XPAN_EVT:
      {
        rem_info->state = QHCI_BT_CLOSE_XPAN_OPEN;
        rem_info->current_transport = XPAN_P2P;

        //Sending Fake CIS Establishment to the Stack
        QHciSendCisEstablishedEvt(rem_info, QHCI_BT_SUCCESS, 0);
        QHciSendCisEstablishedEvt(rem_info, QHCI_BT_SUCCESS, 1);
      }
      break;
    case QHCI_CSM_LE_CLOSE_EVT:
      {
        rem_info->state = QHCI_IDLE_STATE;
        //Sending Unprepare Audio Bearer to XPAN Manager
        if (!dbg_mtp_mora_prop) {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                   NONE);
        } else {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                   NONE);
        }

        //clear any other pending things related to control blocks.
        //Ideally those will be over written
        QHciLeConnCleanup(rem_info);
        //Check with XM whether it need disconnect info or not?
      }
      break;
    case QHCI_CSM_XPAN_CONN_FAILED_EVT:
      {
        //TODO Remove below api call once testing with EB
        rem_info->state = QHCI_BT_OPEN_XPAN_CLOSE;
        rem_info->tState = QHCI_BT_ENABLE;
        qhci_xm_intf.AudioTransportUpdateToXm(rem_info->rem_addr, BT_LE);
        //Create CIS is triggered from the stack
        if (rem_info->is_create_cis_from_stack) {
          SET_BIT(qhci_hci_cmd_wait, BIT_LE_CREATE_CIS_QHCI_TO_SOC);
          SendCreateCisToSoc(rem_info);
          rem_info->is_create_cis_from_stack = false;
          qhci_bearer_switch_pending = false;
          QHciUseCaseUpdateEvt(LE_UNICAST_HQ_PROFILE);
        } else {
          ALOGE("%s:  Invalid call from QHCI moduele", __func__);
        }
      }
      break;
    default:
      ALOGE("%s:  Invalid Event  %d in this state", __func__, event);
  }
}

/******************************************************************
 *
 * Function       QHciHandleBtCloseXpanOpen
 *
 * Description    Handling the QHCI_BT_CLOSE_XPAN_OPEN
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleBtCloseXpanOpen(qhci_dev_cb_t *rem_info,
                                               uint8_t event) {
  //Handle the parameters in QHciHandleBtCloseXpanOpen state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  switch(event)
  {
    case QHCI_CSM_LE_CLOSE_EVT:
      {
        rem_info->state = QHCI_IDLE_STATE;
        //Sending Unprepare Audio Bearer to XPAN Manager
        if (!dbg_mtp_mora_prop) {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                   NONE);
        } else {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                   NONE);
        }

        //clear any other pending things related to control blocks.
        //Ideally those will be over written
        QHciLeConnCleanup(rem_info);
        //Check with XM whether it need disconnect info or not?
      }
      break;
    case QHCI_CSM_CIS_DISCONNECT_EVT:
      {
        QHciSetCisState(rem_info, QHCI_CIS_CLOSE);
        //This Event will be triggered when the stream is stopped
        rem_info->state = QHCI_BT_CLOSE_XPAN_CLOSE;
        //Sending Unprepare Audio Bearer to XPAN Manager
        if (!dbg_mtp_mora_prop) {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                   NONE);
        } else {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                   NONE);
        }
        qhci_bearer_switch_pending = false;
        //clear any other pending things related to control blocks.
        //Ideally those will be over written
      }
      break;
    case QHCI_CSM_PREPARE_BEARER_BT:
      {
        //This Event will be triggered when preparea bearer req from XM
        rem_info->state = QHCI_BT_OPEN_XPAN_OPEN;
        rem_info->is_create_cis_from_qhci_to_soc = true;
        qhci_bearer_switch_pending = true;
        //Send Create CIS to Soc
        ALOGE("%s:  QHCI_CSM_PREPARE_BEARER_BT qhci_bearer_switch_pending  %d",
                __func__, qhci_bearer_switch_pending);
        SendCreateCisToSoc(rem_info);
      }
      break;
    case QHCI_CSM_BEARER_SWITCH_FAILED:
      {
        rem_info->state = QHCI_BT_CLOSE_XPAN_CLOSE;
        QHciSendDisconnectCmplt(cig_params.cis_handles[0],
                                QHCI_DISCONNECT_DUE_TO_LOCAL_HOST_TERM);
        QHciSendDisconnectCmplt(cig_params.cis_handles[1],
                                QHCI_DISCONNECT_DUE_TO_LOCAL_HOST_TERM);
      }
      break;
    default:
      ALOGE("%s:  Invalid Event  %d in this state", __func__, event);
  }
}

/******************************************************************
 *
 * Function       QHciHandleBtOpenXpanClose
 *
 * Description    Handling the QHCI_BT_OPEN_XPAN_CLOSE
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleBtOpenXpanClose(qhci_dev_cb_t *rem_info,
                                               uint8_t event) {
  //Handle the parameters in QHciHandleBtOpenXpanClose state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  switch(event)
  {
    case QHCI_CSM_LE_CLOSE_EVT:
      {
        rem_info->state = QHCI_IDLE_STATE;

        qhci_xm_intf.AudioTransportUpdateToXm(rem_info->rem_addr, NONE);
        //clear any other pending things related to control blocks.
        //Ideally those will be over written
        QHciLeConnCleanup(rem_info);
      }
      break;
    case QHCI_CSM_CIS_DISCONNECT_TX_EVT:
      {
        //This Event will be triggered when the stream is stopped
        rem_info->state = QHCI_BT_CLOSE_XPAN_CLOSE;
        //No Need to update the XPAN Manager.
        //Here Transport is BT. Change current active_transport to NONE
        rem_info->current_transport = BT_LE;
        qhci_cis_disconnect_cmd_wait = true;
          qhci_bearer_switch_pending = false;
        QHciPrepareAndSendHciDisconnect(cig_params.cis_handles[0]);
        //QHciPrepareAndSendHciDisconnect(cig_params.cis_handles[1]);
        qhci_xm_intf.AudioTransportUpdateToXm(rem_info->rem_addr, NONE);
      }
      break;
    case QHCI_CSM_CIS_DISCONNECT_EVT:
      {
        QHciSetCisState(rem_info, QHCI_CIS_CLOSE);
        //This Event will be triggered when the stream is stopped
        rem_info->state = QHCI_BT_CLOSE_XPAN_CLOSE;
        qhci_bearer_switch_pending = false;
        //Here Transport is BT. Change current active_transport to NONE
        rem_info->current_transport = BT_LE;
        qhci_xm_intf.AudioTransportUpdateToXm(rem_info->rem_addr, NONE);
      }
      break;
    case QHCI_CSM_USECASE_XPAN_TRANS_ENABLE_EVT:
      {
        //Move the Bearer to XPAN.
        rem_info->state = QHCI_BT_OPEN_XPAN_CONNECTING;

        //Send the prepareBearer request to the XM
        rem_info->is_prepare_bearer_triggered = true;
        if (!dbg_mtp_mora_prop) {
          active_xpan_device = {};
          qhci_xm_intf.PrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                 rem_info->current_transport);
        } else {
          qhci_xm_intf.PrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                 rem_info->current_transport);
        }
      }
      break;
    case QHCI_CSM_CIS_OPEN_XPAN_TRANS_ENABLE_EVT:
      {
        // Respond to prepare Bearer Request to XPAN.
        rem_info->state = QHCI_BT_OPEN_XPAN_CONNECTING;
        qhci_bearer_switch_pending = true;
        rem_info->is_prepare_bearer_triggered = true;

        qhci_xm_intf.PrepareAudioBearerRspToXm(rem_info->rem_addr, XM_SUCCESS);
      }
      break;
    default:
      ALOGE("%s:  Invalid Event  %d in this state", __func__, event);
  }
}

/******************************************************************
 *
 * Function       QHciSendDisconnectCisToSoc
 *
 * Description    Sending HCI Disconnect to CIS handles to SCO
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *
 * return         none
 ******************************************************************/
void QHci::QHciSendDisconnectCisToSoc(qhci_dev_cb_t *rem_info) {
  ALOGD("%s:  ", __func__);

  if (rem_info) {
    rem_info->is_cis_disconnect_pending_from_soc =  true;
    //currently only one cis will created with this remote
    // need to enhance to csip devices also
    qhci_cis_disconnect_cmd_wait = true;
    QHciPrepareAndSendHciDisconnect(cig_params.cis_handles[0]);
    //QHciPrepareAndSendHciDisconnect(cig_params.cis_handles[1]);
  } else {
    ALOGE("%s:  QHCI Rem_info Database is Null", __func__);
  }

}

/******************************************************************
 *
 * Function       QHciHandleBtOpenXpanConnecting
 *
 * Description    Handling the QHCI_BT_OPEN_XPAN_CONNECTING
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleBtOpenXpanConnecting(qhci_dev_cb_t *rem_info,
                                                      uint8_t event) {
  //Handle the parameters in QHciHandleBtOpenXpanConnecting state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  switch(event) {
    case QHCI_CSM_LE_CLOSE_EVT:
      {
        rem_info->state = QHCI_IDLE_STATE;
        //Sending Unprepare Audio Bearer to XPAN Manager
        if (!dbg_mtp_mora_prop) {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                   NONE);
        } else {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                   NONE);
        }
        //clear any other pending things related to control blocks.
        //Ideally those will be over written
        QHciLeConnCleanup(rem_info);
        //Check with XM whether it need disconnect info or not?
      }
      break;
      case QHCI_CSM_CIS_DISCONNECT_TX_EVT:
      {
        QHciSetCisState(rem_info, QHCI_CIS_CLOSE);
        //This Event will be triggered when the stream is stopped
        rem_info->state = QHCI_BT_CLOSE_XPAN_CLOSE;
        rem_info->qhci_block_cis_disconnect_evt = true;
        //Sending Unprepare Audio Bearer to XPAN Manager
        if (!dbg_mtp_mora_prop) {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                   NONE);
        } else {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                   NONE);
        }

        //Here Transport is BT. Change current active_transport to NONE
        rem_info->current_transport = BT_LE;
        qhci_cis_disconnect_cmd_wait = true;
        qhci_bearer_switch_pending = false;
        QHciPrepareAndSendHciDisconnect(cig_params.cis_handles[0]);
        qhci_xm_intf.AudioTransportUpdateToXm(rem_info->rem_addr, NONE);
      }
      break;
    case QHCI_CSM_CIS_DISCONNECT_EVT:
      {
        //This Event will be triggered when the stream is stopped
        rem_info->state = QHCI_BT_CLOSE_XPAN_CLOSE;
        //Sending Unprepare Audio Bearer to XPAN Manager
        if (!dbg_mtp_mora_prop) {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                   NONE);
        } else {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                   NONE);
        }
        //Here Transport is BT. Change current active_transport to NONE
        rem_info->current_transport = BT_LE;
        qhci_cis_disconnect_cmd_wait = true;
        qhci_bearer_switch_pending = false;
        QHciPrepareAndSendHciDisconnect(cig_params.cis_handles[0]);
        qhci_xm_intf.AudioTransportUpdateToXm(rem_info->rem_addr, NONE);
      }
      break;
    case QHCI_CSM_XPAN_CONN_FAILED_EVT:
      {
        rem_info->state = QHCI_BT_OPEN_XPAN_CLOSE;
        rem_info->tState = QHCI_BT_ENABLE;
        rem_info->current_transport = BT_LE;
        rem_info->is_prepare_bearer_triggered = false;
      }
      break;
    case QHCI_CSM_UPDATE_TRANS_XPAN_EVT:
      {
        rem_info->state = QHCI_BT_CLOSE_XPAN_OPEN;
        rem_info->current_transport = XPAN_P2P;
        rem_info->is_prepare_bearer_triggered = false;
        rem_info->is_create_cis_from_qhci_to_soc = true;
        qhci_bearer_switch_pending = false;
        //disconnect the CIS handles with the Soc
        QHciSendDisconnectCisToSoc(rem_info);
      }
      break;
    default:
      ALOGE("%s:  Invalid Event  %d in this state", __func__, event);
  }
}

/******************************************************************
 *
 * Function       QHciHandleBtOpenXpanOpen
 *
 * Description    Handling the QHCI_BT_OPEN_XPAN_OPEN
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciHandleBtOpenXpanOpen(qhci_dev_cb_t *rem_info,
                                              uint8_t event) {
  //Handle the parameters in QHciHandleBtOpenXpanOpen state
  ALOGD("%s:  Event  %s", __func__, ConvertEventToString(event));

  switch(event)
  {
    case QHCI_CSM_LE_CLOSE_EVT:
      {
        rem_info->state = QHCI_IDLE_STATE;
        //Sending Unprepare Audio Bearer to XPAN Manager
        if (!dbg_mtp_mora_prop) {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                   NONE);
        } else {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                   NONE);
        }
        //clear any other pending things related to control blocks.
        //Ideally those will be over written
        QHciLeConnCleanup(rem_info);
        //Check with XM whether it need disconnect info or not?
      }
      break;
    case QHCI_CSM_CIS_DISCONNECT_EVT:
      {
        QHciSetCisState(rem_info, QHCI_CIS_CLOSE);

        //This Event will be triggered when the stream is stopped
        rem_info->state = QHCI_BT_CLOSE_XPAN_CLOSE;
        //Sending Unprepare Audio Bearer to XPAN Manager
        if (!dbg_mtp_mora_prop) {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(rem_info->rem_addr,
                                                   NONE);
        } else {
          qhci_xm_intf.UnPrepareAudioBearerReqToXm(dbg_curr_bd_addr[0],
                                                   NONE);
        }
        qhci_bearer_switch_pending = false;
        //Here Transport is BT. Change current active_transport to NONE
        rem_info->current_transport = BT_LE;
      }
      break;
    case QHCI_CSM_USECASE_XPAN_TRANS_DISABLE_EVT:
      {
        rem_info->state = QHCI_BT_OPEN_XPAN_CLOSE;
        rem_info->tState = QHCI_BT_ENABLE;
        rem_info->current_transport = BT_LE;
        rem_info->is_prepare_bearer_triggered = false;
        qhci_bearer_switch_pending = false;
        qhci_xm_intf.AudioTransportUpdateToXm(rem_info->rem_addr, BT_LE);
      }
      break;
    default:
      ALOGE("%s:  Invalid Event  %d in this state", __func__, event);
  }
}

/******************************************************************
 *
 * Function       QHciSmExecute
 *
 * Description    Execute the state machine based on the state
 *                for that remote device
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciSmExecute(qhci_dev_cb_t *rem_info, uint8_t event) {
  ALOGD("%s state %s Event %s ", __func__, ConvertMsgtoString(rem_info->state),
         ConvertEventToString(event));

  switch (rem_info->state) {
    case QHCI_IDLE_STATE:
        QHciHandleIdleState(rem_info, event);
        break;
      case QHCI_BT_CLOSE_XPAN_CLOSE:
        QHciHandleBtCloseXpanClose(rem_info, event);
        break;
      case QHCI_BT_CLOSE_XPAN_CONNECTING:
        QHciHandleBtCloseXpanConnecting(rem_info, event);
        break;
      case QHCI_BT_CLOSE_XPAN_OPEN:
        QHciHandleBtCloseXpanOpen(rem_info, event);
        break;
      case QHCI_BT_OPEN_XPAN_CLOSE:
        QHciHandleBtOpenXpanClose(rem_info, event);
        break;
      case QHCI_BT_OPEN_XPAN_CONNECTING:
        QHciHandleBtOpenXpanConnecting(rem_info, event);
        break;
      case QHCI_BT_OPEN_XPAN_OPEN:
        QHciHandleBtOpenXpanOpen(rem_info, event);
        break;
      default:
        ALOGD(" %s Not valid event = %d", event);
        break;
    }
}


void QHci::QHciCmdTimeOut(union sigval sig) {
  //Handle when command was timeout
}

void QHci::QHciSetCmdTimerState(QHciTimerState timer_state) {
  qhci_cmd_timer_.timer_state = timer_state;
}

QHciTimerState QHci::QHciGetCmdTimerState() {
  return qhci_cmd_timer_.timer_state;
}

void QHci::QHciStopCmdTimer() {
  struct itimerspec ts;
  QHciTimerState cmd_timer_state;

  cmd_timer_state = QHciGetCmdTimerState();

  if (cmd_timer_state == QHCI_TIMER_NOT_CREATED) {
      ALOGD("%s: CmdTimer already stopped", __func__);
      return;
  } else {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    if (timer_settime(qhci_cmd_timer_.timer_id, 0, &ts, 0) == -1) {
      ALOGE("%s:Failed to stop cmd thread timer", __func__);
    }
    timer_delete(qhci_cmd_timer_.timer_id);
    QHciSetCmdTimerState(QHCI_TIMER_NOT_CREATED);
    ALOGD("%s: CmdTimer Stopped", __func__);
    return;
  }
}

void QHci::QHciStartCommandTimer() {
  struct itimerspec ts;
  struct sigevent se;
  uint32_t timeout;
  int prop_val = 0;

  ALOGV("%s", __func__);
  if (qhci_cmd_timer_.timer_state == QHCI_TIMER_NOT_CREATED) {
    se.sigev_notify_function = (void (*)(union sigval))QHciCmdTimeOut;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &qhci_cmd_timer_.timer_id;
    se.sigev_notify_attributes = NULL;

    if (!timer_create(CLOCK_MONOTONIC, &se, &qhci_cmd_timer_.timer_id))
      QHciSetCmdTimerState(QHCI_TIMER_CREATED);
    else
      ALOGE("%s: Failed to create QHCICmdTimer", __func__);
  }

  if (QHciGetCmdTimerState() == QHCI_TIMER_CREATED) {

    //Currently timeout is 1 sec
    timeout = 1000;
    ts.it_value.tv_sec = (timeout / 1000);
    ts.it_value.tv_nsec = 1000000 * (timeout % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    if ((timer_settime(qhci_cmd_timer_.timer_id, 0, &ts, 0)) == -1) {
      ALOGE("%s: Failed to start Init timer", __func__);
      return;
    } else {
      QHciSetCmdTimerState(QHCI_TIMER_ACTIVE);
      ALOGD("%s: Command timer started", __func__);
    }
  }
}

/******************************************************************
 *
 * Function       PostLocalVersionFeatureToAc
 *
 * Description    Sending Local Version and Feature Request to AC
 *
 * Arguments      None
 *
 * return         None
 ******************************************************************/
void QHci::PostLocalVersionFeatureToAc() {
  ALOGD("%s: ", __func__);

  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH);
  msg->eventId = QHCI_LOCAL_VER_LE_FEATURES_TO_AC;
  msg->StateChangeEvt.eventId = QHCI_LOCAL_VER_LE_FEATURES_TO_AC;
  PostMessage(msg);
}

/******************************************************************
 *
 * Function       QHciSendLocalVerAndLeFeatToAC
 *
 * Description    Sending Local Version and Feature Request to AC
 *
 * Arguments      None
 *
 * return         None
 ******************************************************************/
void QHci::QHciSendLocalVerAndLeFeatToAC() {
  ALOGD("%s: ", __func__);
  XpanQhciAcIf::GetIf()->UpdateLocalLeFeatures(qhci_le_local_support_feature);
  XpanQhciAcIf::GetIf()->UpdateLocalVersion(qhci_local_version.version,
                                  qhci_local_version.companyId,
                                  qhci_local_version.subversion);

}

/******************************************************************
 *
 * Function       QHciPostMessageToAc
 *
 * Description    QHCI Posting Message to AC 
 *
 * Arguments      Opcode
 *
 * return         None
 ******************************************************************/
void QHci::QHciPostMessageToAc(const uint8_t* data, size_t length) {
  ALOGD("%s  ", __func__);

  uint16_t opcode = (data[1] << 8 | data[0]);
  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH);

  if (opcode == HCI_LE_ENABLE_ENCRYPTION_CMD) {
    msg->eventId = QHCI_SEND_LE_ENCRYPT_CMD_TO_AC;
    msg->LeEncrptCmdToAc.eventId = QHCI_SEND_LE_ENCRYPT_CMD_TO_AC;
    msg->LeEncrptCmdToAc.handle = (data[4] << 8 | data[3]);
    for (uint8_t i = 0; i < 16; i++) {
      msg->LeEncrptCmdToAc.ltk[i] = data[15 + i];
    }
    PostMessage(msg);
    return;
  }

  if (opcode == HCI_LE_READ_REMOTE_FEAT_CMD) {
    msg->eventId = QHCI_SEND_READ_REMOTE_LE_FEATURES;
    msg->GetLeFeatFromAc.eventId = QHCI_SEND_READ_REMOTE_LE_FEATURES;
    msg->GetLeFeatFromAc.handle = (data[4] << 8 | data[3]);
    PostMessage(msg);
    return;
  }

  if (opcode == HCI_READ_REMOTE_VERSION_CMD) {
    msg->eventId = QHCI_SEND_READ_REMOTE_VER_REQ;
    msg->GetLeFeatFromAc.eventId = QHCI_SEND_READ_REMOTE_VER_REQ;
    msg->GetLeFeatFromAc.handle = (data[4] << 8 | data[3]);
    PostMessage(msg);
    return;
  }
}

void QHci::QHciSendCreateConnectionToAc(bdaddr_t bd_addr,
                                            uint16_t supervision_tout) {
  ALOGE("%s  ", __func__);
  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH);

  msg->eventId = QHCI_SEND_CREATE_CONNECT_TO_AC;
  msg->LeConnToAc.eventId = QHCI_SEND_CREATE_CONNECT_TO_AC;
  msg->LeConnToAc.supervision_tout = supervision_tout;

  for (int i = 0; i < 6; i++)
    msg->LeConnToAc.bd_addr.b[i] = bd_addr.b[i];

  PostMessage(msg);

}

void QHci::QHciSendCancelConnectionToAc(bdaddr_t bd_addr) {
  ALOGE("%s  ", __func__);
  qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH);

  msg->eventId = QHCI_SEND_CANCEL_CONNECT_TO_AC;
  msg->CancelConnToAc.eventId = QHCI_SEND_CANCEL_CONNECT_TO_AC;


  for (int i = 0; i < 6; i++)
    msg->CancelConnToAc.bd_addr.b[i] = bd_addr.b[i];

  PostMessage(msg);

}


void QHci::QHciSendDisconnectToAc(uint16_t handle) {
  ALOGE("%s  ", __func__);

  qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);
  if (rem_info) {
    qhci_msg_t *msg = (qhci_msg_t *) malloc(QHCI_PKT_MESSAGE_LENGTH);

    msg->eventId = QHCI_SEND_DISCONNECT_TO_AC;
    msg->DisconnToAc.eventId = QHCI_SEND_DISCONNECT_TO_AC;

    for (int i = 0; i < 6; i++) {
      msg->DisconnToAc.bd_addr.b[i] = rem_info->rem_addr.b[i];
    }
    PostMessage(msg);
  } else {
    ALOGE("%s  Invalid Handle %d. QHCI is BAD State ", __func__, handle);
  }

}

UseCaseType QHci::getCurrentUsecaseType() {
  ALOGD("%s UsecaseType %d", __func__, usecase_type_on_transport);

  return usecase_type_on_transport;
}

void QHci::ProcessTxAclData(const uint8_t *data, size_t length) {
  ALOGE("%s  length %d", __func__, length);

  uint16_t handle = (data[1] << 8 | data [0]);

  qhci_dev_cb_t* rem_info = GetQHciRemoteDeviceInfo(handle);
  if (rem_info) {
    const std::vector<uint8_t> qhci_data(data + 4, data + length);

    ALOGE("%s  valid handle %d data %d %d %d %d", __func__, handle,
           qhci_data[0], qhci_data[1], qhci_data[2], qhci_data[3]);
    XpanQhciAcIf::GetIf()->SendAclData(rem_info->rem_addr, 1, qhci_data);
  } else {
    ALOGE("%s  Invalid Handle %d. QHCI is BAD State ", __func__, handle);
  }

}

/******************************************************************
 *
 * Function       QHciTransportSmExecute
 *
 * Description    Execute the state machine based on the state
 *                for that remote device
 *
 * Arguments      qhci_dev_cb_t -QHCI control block for that
 *                remote
 *                event - type of event which needs to handle in
 *                this state
 *
 * return         none
 ******************************************************************/
void QHci::QHciTransportSmExecute(qhci_dev_cb_t *rem_info,
                                           uint8_t event) {
  ALOGD("%s QHCI_STATE: %s Event %s ", __func__,
          ConvertStatetoString(rem_info->tState),
          ConvertEventToString(event));

  switch (rem_info->tState) {
    case QHCI_TRANSPORT_IDLE_STATE:
        QHciHandleTransportIdleState(rem_info, event);
        break;
      case QHCI_BT_ENABLE:
        QHciHandleBtEnable(rem_info, event);
        break;
      case QHCI_BT_ENABLE_AP_ENABLE:
        QHciHandleBtEnableApEnable(rem_info, event);
        break;
      case QHCI_P2P_ENABLE:
        QHciHandleP2PEnable(rem_info, event);
        break;
      case QHCI_P2P_ENABLE_BT_ENABLE:
        QHciHandleP2PEnableBtEnable(rem_info, event);
        break;
      case QHCI_BT_ENABLE_AP_CONNECTING:
        QHciHandleBtEnableApConnecting(rem_info, event);
        break;
      case QHCI_AP_ENABLE:
        QHciHandleApEnable(rem_info, event);
        break;
      case QHCI_AP_ENABLE_BT_CONNECTING:
        QHciHandleApEnableBtConnecting(rem_info, event);
        break;
      default:
        ALOGD(" %s Not valid event = %d", event);
        break;
    }
}

void QHci::QHciSetCisState(qhci_dev_cb_t *rem_info, QHCI_CIS_STATE state) {
  ALOGD("%s state %s", __func__, ConvertCisStatetoString(state));

  if (rem_info) {
    rem_info->cis_state = state;
  } else {
    ALOGE("%s rem_info is in bad state , FATAL ", __func__);
  }
}

} // namespace implementation
} // namespace xpan

