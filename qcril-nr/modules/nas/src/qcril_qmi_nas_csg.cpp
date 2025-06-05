/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/nas/NasModule.h"
#include "modules/nas/qcril_nas_legacy.h"
#include "modules/nas/qcril_qmi_nas.h"
#include "interfaces/nas/RilRequestCsgPerformNetworkScanMessage.h"
#include "interfaces/nas/RilRequestCsgSetSysSelPrefMessage.h"
#include "qcril_qmi_err_utils.h"

#define TAG "RILQ"

RIL_Errno qcril_qmi_nas_get_prev_reported_csg_id(uint32_t& csg_id);

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_get_csg_id
======================================================================================*/
RIL_Errno qcril_qmi_nas_get_csg_id(uint32_t& csg_id) {
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  QCRIL_LOG_FUNC_ENTRY();

  result = qcril_qmi_nas_get_prev_reported_csg_id(csg_id);

  QCRIL_LOG_INFO("csg_id - %d", csg_id);

  QCRIL_LOG_FUNC_RETURN();
  return result;
}

//===========================================================================
// qcril_qmi_nas_csg_perform_network_scan_command_cb
//===========================================================================
void qcril_qmi_nas_csg_perform_network_scan_command_cb(unsigned int /*msg_id*/,
                                                       std::shared_ptr<void> resp_c_struct,
                                                       unsigned int /*resp_c_struct_len*/,
                                                       void* resp_cb_data,
                                                       qmi_client_error_type transp_err) {
  QCRIL_LOG_FUNC_ENTRY();
  nas_perform_network_scan_resp_msg_v01* qmi_response =
      (nas_perform_network_scan_resp_msg_v01*)resp_c_struct.get();
  uint32 user_data = (uint32)(uintptr_t)resp_cb_data;
  uint16 req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
  auto pendingMsg = getNasModule().getPendingMessageList().extract(req_id);
  if (pendingMsg && qmi_response) {
    RIL_Errno rilErr =
        qcril_qmi_util_convert_qmi_response_codes_to_ril_result(transp_err, &qmi_response->resp);
    QCRIL_LOG_INFO(".. res %d, qmi trasp err %d, qmi det err %d", (int)rilErr, (int)transp_err,
                   (int)qmi_response->resp.error);
    std::shared_ptr<qcril::interfaces::CsgPerformNwScanResult_t> payload;
    if (rilErr == RIL_E_SUCCESS) {
      payload = std::make_shared<qcril::interfaces::CsgPerformNwScanResult_t>();
      if (payload) {
        payload->scan_result_valid = qmi_response->scan_result_valid;
        if (qmi_response->scan_result_valid == TRUE) {
          QCRIL_LOG_DEBUG("Scan result TLV filled from QMI-NAS = %d", qmi_response->scan_result);
          payload->scan_result = (uint32_t)qmi_response->scan_result;
        }

        payload->csg_info_valid = qmi_response->csg_info_valid;
        payload->csg_info_len = qmi_response->csg_info_len;
        if (qmi_response->csg_info_valid == TRUE && qmi_response->csg_info_len > 0) {
          for (uint32_t i = 0; i < qmi_response->csg_info_len; i++) {
            payload->csg_info[i].mcc = qmi_response->csg_info[i].mcc;
            payload->csg_info[i].mnc = qmi_response->csg_info[i].mnc;
            if (qmi_response->mnc_includes_pcs_digit_valid) {
              payload->csg_info[i].mnc_includes_pcs_digit =
                  qmi_response->mnc_includes_pcs_digit[i].mnc_includes_pcs_digit;
            } else {
              payload->csg_info[i].mnc_includes_pcs_digit = FALSE;
            }
            payload->csg_info[i].csg_list_cat = qmi_response->csg_info[i].csg_list_cat;
            payload->csg_info[i].csg_info.id = qmi_response->csg_info[i].csg_info.id;
            payload->csg_info[i].csg_info.name_len = qmi_response->csg_info[i].csg_info.name_len;
            /*CSG name  len*2-UCS2 bytes*/
            memcpy(payload->csg_info[i].csg_info.name, qmi_response->csg_info[i].csg_info.name,
                   payload->csg_info[i].csg_info.name_len * 2);
          }
        }

        payload->csg_sig_info_valid = qmi_response->csg_sig_info_valid;
        payload->csg_sig_info_len = qmi_response->csg_sig_info_len;
        if (qmi_response->csg_sig_info_valid == TRUE && qmi_response->csg_sig_info_len > 0) {
          for (uint32_t i = 0; i < qmi_response->csg_sig_info_len; i++) {
            payload->csg_sig_info[i].mcc = qmi_response->csg_sig_info[i].mcc;
            payload->csg_sig_info[i].mnc = qmi_response->csg_sig_info[i].mnc;
            if (qmi_response->mnc_includes_pcs_digit_valid) {
              payload->csg_sig_info[i].mnc_includes_pcs_digit =
                  qmi_response->mnc_includes_pcs_digit[i].mnc_includes_pcs_digit;
            } else {
              payload->csg_sig_info[i].mnc_includes_pcs_digit = FALSE;
            }

            payload->csg_sig_info[i].csg_id = qmi_response->csg_sig_info[i].csg_id;
            payload->csg_sig_info[i].signal_strength = qmi_response->csg_sig_info[i].signal_strength;
          }
        }
      }
    }

    auto respData = std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, payload);
    auto msg(std::static_pointer_cast<RilRequestCsgPerformNetworkScanMessage>(pendingMsg));
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
  }
  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_nas_csg_perform_network_scan_command_cb */

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_csg_perform_network_scan
===========================================================================*/
void qcril_qmi_nas_csg_perform_network_scan(
    std::shared_ptr<RilRequestCsgPerformNetworkScanMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg) {
    nas_perform_network_scan_req_msg_v01 qmi_request;
    memset(&qmi_request, 0, sizeof(qmi_request));
    if (msg->hasNetworkType()) {
      qmi_request.network_type_valid = TRUE;
      qmi_request.network_type = msg->getNetworkType();
    }
    if (msg->hasScanType()) {
      qmi_request.scan_type_valid = TRUE;
      qmi_request.scan_type = msg->getScanType();
    }

    RIL_Errno rilErr = RIL_E_SUCCESS;
    auto pendingMsgStatus = std::make_pair(0, false);
    do {
      pendingMsgStatus = getNasModule().getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
        rilErr = RIL_E_GENERIC_FAILURE;
        break;
      }
      uint16_t req_id = pendingMsgStatus.first;
      uint32_t user_data =
          QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);
      qmi_client_error_type qmi_client_error = qmi_client_nas_send_async(
          QMI_NAS_PERFORM_NETWORK_SCAN_REQ_MSG_V01, &qmi_request, sizeof(qmi_request),
          sizeof(nas_perform_network_scan_resp_msg_v01),
          qcril_qmi_nas_csg_perform_network_scan_command_cb, (void*)(uintptr_t)user_data);
      rilErr = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_client_error, NULL);
    } while (0);

    if (rilErr != RIL_E_SUCCESS) {
      if (pendingMsgStatus.second) {
        getNasModule().getPendingMessageList().erase(pendingMsgStatus.first);
      }
      auto respData = std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_nas_csg_perform_network_scan */

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_csg_set_system_selection_preference
===========================================================================*/
/*!
    @brief
    Manually select a CSG Network.

    @return
    If success then it indicates sys selection is set
*/
/*=========================================================================*/
void qcril_qmi_nas_csg_set_system_selection_preference(
    std::shared_ptr<RilRequestCsgSetSysSelPrefMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg) {
    RIL_Errno rilErr = RIL_E_SUCCESS;
    bool sendResp = false;
    do {
      nas_get_system_selection_preference_resp_msg_v01 qmi_response_get;
      memset(&qmi_response_get, 0, sizeof(qmi_response_get));
      qmi_client_error_type qmi_client_error =
          qmi_client_nas_send_sync(QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01, NULL,
                                   NAS_ZERO, (void*)&qmi_response_get, sizeof(qmi_response_get),
                                   ModemEndPoint::SYNC_REQ_UNRESTRICTED_TIMEOUT);

      if (qmi_response_get.csg_info_valid && msg->hasCsgInfo()) {
        nas_csg_nw_iden_type_v01 csg_info = msg->getCsgInfo();
        if ((qmi_response_get.csg_info.id == csg_info.id) &&
            (qmi_response_get.csg_info.mcc == csg_info.mcc) &&
            (qmi_response_get.csg_info.mnc == csg_info.mnc)) {
          QCRIL_LOG_DEBUG("Already camped on requested CSG ID");
          rilErr = RIL_E_SUCCESS;
          sendResp = true;
          break;
        }
      }
      nas_set_system_selection_preference_req_msg_v01 qmi_request;
      nas_set_system_selection_preference_resp_msg_v01 qmi_response;

      memset(&qmi_request, 0, sizeof(qmi_request));
      memset(&qmi_response, 0, sizeof(qmi_response));

      if (msg->hasCsgInfo()) {
        qmi_request.csg_info_valid = TRUE;
        qmi_request.csg_info = msg->getCsgInfo();
      } else {
        qmi_request.net_sel_pref_valid = TRUE;
        qmi_request.net_sel_pref.net_sel_pref = NAS_NET_SEL_PREF_AUTOMATIC_V01;
      }

      qmi_client_error = qmi_client_nas_send_sync(
          QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01, &qmi_request, sizeof(qmi_request),
          (void*)&qmi_response, sizeof(qmi_response), ModemEndPoint::SYNC_REQ_UNRESTRICTED_TIMEOUT);
      rilErr = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_client_error,
                                                                       &qmi_response.resp);

      if (rilErr == RIL_E_SUCCESS) {
        QCRIL_LOG_DEBUG("Waiting for System selection pref indication from QMI-NAS..");
        auto pendingMsgStatus = getNasModule().getPendingMessageList().insert(msg);
        if (pendingMsgStatus.second != true) {
          QCRIL_LOG_ERROR("getPendingMessageList().insert failed!!");
          rilErr = RIL_E_GENERIC_FAILURE;
          sendResp = true;
          break;
        }
      } else {
        QCRIL_LOG_ERROR("CSG Manual Selection error, QMI Result = %d,error=%d\n",
                        qmi_response.resp.result, qmi_response.resp.error);
        rilErr = RIL_E_GENERIC_FAILURE;
        sendResp = true;
        break;
      }
    } while (0);

    if (sendResp) {
      auto respData = std::make_shared<QcRilRequestMessageCallbackPayload>(rilErr, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_nas_csg_set_system_selection_preference */

//===========================================================================
// qcril_qmi_nas_csg_system_selection_preference_ind_handler
//===========================================================================
void qcril_qmi_nas_csg_system_selection_preference_ind_handler(
    nas_system_selection_preference_ind_msg_v01* qmi_ind) {
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = getNasModule().getPendingMessageList().find<RilRequestCsgSetSysSelPrefMessage>();
  if (msg && qmi_ind) {
    RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
    if (qmi_ind->csg_info_valid) {
      QCRIL_LOG_DEBUG("CSG Manual Selection Set Successfully: CSG_ID: %d, MCC: %03d, MNC: %03d\n",
                      qmi_ind->csg_info.id, qmi_ind->csg_info.mcc, qmi_ind->csg_info.mnc);
      ril_req_res = RIL_E_SUCCESS;
    } else if (qmi_ind->net_sel_pref_valid) {
      QCRIL_LOG_DEBUG("CSG Manual Selection Set Successfully: %d\n", qmi_ind->net_sel_pref);
      ril_req_res = RIL_E_SUCCESS;
    } else {
      QCRIL_LOG_ERROR("CSG Manual Selection Set Failed\n");
    }
    getNasModule().getPendingMessageList().erase(msg);
    auto respData = std::make_shared<QcRilRequestMessageCallbackPayload>(ril_req_res, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
  }
  QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_nas_csg_system_selection_preference_ind_handler
