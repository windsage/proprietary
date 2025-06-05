/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "framework/Log.h"
#include "qcril_qmi_err_utils.h"
#include "qcril_qmi_client.h"
#include "qcril_qmi_sar.h"
#include "specific_absorption_rate_v01.h"
#include "telephony/ril.h"

#include "modules/qmi/QmiUtils.h"

#include <algorithm>

static qmi_sar_rf_state_enum_type_v01 mapSarRfState(uint32_t rilState) {
  qmi_sar_rf_state_enum_type_v01 qmiState = QMI_SAR_RF_STATE_DEFAULT_V01;
  switch (rilState) {
    case 1:
      qmiState = QMI_SAR_RF_STATE_1_V01;
      break;
    case 2:
      qmiState = QMI_SAR_RF_STATE_2_V01;
      break;
    case 3:
      qmiState = QMI_SAR_RF_STATE_3_V01;
      break;
    case 4:
      qmiState = QMI_SAR_RF_STATE_4_V01;
      break;
    case 5:
      qmiState = QMI_SAR_RF_STATE_5_V01;
      break;
    case 6:
      qmiState = QMI_SAR_RF_STATE_6_V01;
      break;
    case 7:
      qmiState = QMI_SAR_RF_STATE_7_V01;
      break;
    case 8:
      qmiState = QMI_SAR_RF_STATE_8_V01;
      break;
    case 9:
      qmiState = QMI_SAR_RF_STATE_9_V01;
      break;
    case 10:
      qmiState = QMI_SAR_RF_STATE_10_V01;
      break;
    case 11:
      qmiState = QMI_SAR_RF_STATE_11_V01;
      break;
    case 12:
      qmiState = QMI_SAR_RF_STATE_12_V01;
      break;
    case 13:
      qmiState = QMI_SAR_RF_STATE_13_V01;
      break;
    case 14:
      qmiState = QMI_SAR_RF_STATE_14_V01;
      break;
    case 15:
      qmiState = QMI_SAR_RF_STATE_15_V01;
      break;
    case 16:
      qmiState = QMI_SAR_RF_STATE_16_V01;
      break;
    case 17:
      qmiState = QMI_SAR_RF_STATE_17_V01;
      break;
    case 18:
      qmiState = QMI_SAR_RF_STATE_18_V01;
      break;
    case 19:
      qmiState = QMI_SAR_RF_STATE_19_V01;
      break;
    case 20:
      qmiState = QMI_SAR_RF_STATE_20_V01;
      break;
    case 0:  // fallthrough
    default:
      qmiState = QMI_SAR_RF_STATE_DEFAULT_V01;
      break;
  }
  return qmiState;
}

//===========================================================================
// qcril_qmi_sar_set_max_transmit_power
//===========================================================================
void qcril_qmi_sar_set_max_transmit_power(std::shared_ptr<RilRequestSetTransmitPowerMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg) {
    sar_rf_set_state_req_msg_v01 qmi_request;
    sar_rf_set_state_resp_msg_v01 qmi_response;

    memset(&qmi_request, 0, sizeof(qmi_request));
    memset(&qmi_response, 0, sizeof(qmi_response));

    qmi_request.sar_rf_state = mapSarRfState(msg->getRfState());
    QCRIL_LOG_INFO(".. params rf state index %d / sar rf state %d",
                   (int)msg->getRfState(), (int)qmi_request.sar_rf_state);
    qmi_request.compatibility_key_valid = msg->hasCompatibilityKey();
    QCRIL_LOG_INFO(".. params compatibility_key_valid %d / qmi request compatibility_key_valid %d",
                   msg->hasCompatibilityKey(), (uint32_t)qmi_request.compatibility_key_valid);
    qmi_request.compatibility_key = msg->getCompatibilityKey();
    QCRIL_LOG_INFO(".. params compatibility_key %d / qmi request compatibility_key %d",
                   msg->getCompatibilityKey(), (uint32_t)qmi_request.compatibility_key);

    qmi_client_error_type qmi_client_error = qmi_client_send_msg_sync_with_shm(
        qcril_qmi_client_get_user_handle(QCRIL_QMI_CLIENT_RF_SAR), QMI_SAR_RF_SET_STATE_REQ_MSG_V01,
        (void*)&qmi_request, sizeof(qmi_request), (void*)&qmi_response, sizeof(qmi_response),
        QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);

    RIL_Errno ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
        qmi_client_error, &qmi_response.resp);
    QCRIL_LOG_INFO(
        "QMI response status - qmi_client_error: %d, qmi_serv_resp.result: %d, "
        "qmi_serv_resp.error: %d, ril_error: %d",
        qmi_client_error, qmi_response.resp.result, qmi_response.resp.error, ril_req_res);

    auto respData = std::make_shared<QcRilRequestMessageCallbackPayload>(ril_req_res, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
  }
  QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_sar_set_max_transmit_power

//===========================================================================
// qcril_qmi_sar_get_sar_rev_key
//===========================================================================
void qcril_qmi_sar_get_sar_rev_key(std::shared_ptr<RilRequestGetSarRevKeyMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg) {
    sar_rf_get_compatibility_key_resp_msg_v01 qmi_response;
    memset(&qmi_response, 0, sizeof(qmi_response));
    qmi_client_error_type qmi_client_error = qmi_client_send_msg_sync_with_shm(
        qcril_qmi_client_get_user_handle(QCRIL_QMI_CLIENT_RF_SAR),
        QMI_SAR_GET_COMPATIBILITY_KEY_REQ_MSG_V01, NULL, QMI_RIL_ZERO, (void*)&qmi_response,
        sizeof(qmi_response), QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);

    RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
    std::shared_ptr<qcril::interfaces::GetSarRevKeyResult> payload;
    QCRIL_LOG_DEBUG("qmi_client_error=%d", qmi_client_error);
    if (qmi_client_error == QMI_NO_ERR) {
      ril_req_res = RIL_E_SUCCESS;
      if (qmi_response.compatibility_key_valid) {
        payload = std::make_shared<qcril::interfaces::GetSarRevKeyResult>();
        if (payload) {
          payload->key = qmi_response.compatibility_key;
        }
      }
    }
    auto respData = std::make_shared<QcRilRequestMessageCallbackPayload>(ril_req_res, payload);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
  }
  QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_sar_get_sar_rev_key
