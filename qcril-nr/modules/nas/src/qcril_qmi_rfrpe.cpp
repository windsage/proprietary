/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "framework/Log.h"
#include "qcril_qmi_err_utils.h"
#include "qcril_qmi_client.h"
#include "qcril_qmi_rfrpe.h"
#include "radio_frequency_radiated_performance_enhancement_v01.h"
#include "telephony/ril.h"

#include "modules/qmi/QmiUtils.h"

#include <algorithm>


//===========================================================================
// qcril_qmi_nas_get_rfm_scenario_req
//===========================================================================
void qcril_qmi_nas_get_rfm_scenario_req(std::shared_ptr<RilRequestGetRfmScenarioMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg) {
    std::shared_ptr<qcril::interfaces::GetRfmScenarioResult> payload;
    RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
    rfrpe_get_rfm_scenarios_resp_v01 qmi_response;

    memset(&qmi_response, 0, sizeof(qmi_response));

    qmi_client_error_type qmi_client_error = qmi_client_send_msg_sync_with_shm(
        qcril_qmi_client_get_user_handle(QCRIL_QMI_CLIENT_RFPE), QMI_RFRPE_GET_RFM_SCENARIO_REQ_V01,
        NULL, QMI_RIL_ZERO, (void*)&qmi_response, sizeof(qmi_response),
        QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);

    ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_client_error,
                                                                          &qmi_response.resp);
    QCRIL_LOG_INFO("ril_req_res=%d, RFPE scenario valid=%d, RFPE scenario len=%d", ril_req_res,
                   qmi_response.active_scenarios_valid, qmi_response.active_scenarios_len);
    if (ril_req_res == RIL_E_SUCCESS) {
      if (qmi_response.active_scenarios_valid) {
        for (size_t i = 0; i < qmi_response.active_scenarios_len; i++) {
          QCRIL_LOG_INFO("RFPE scenario %d is %d", i, qmi_response.active_scenarios[i]);
        }
        payload = std::make_shared<qcril::interfaces::GetRfmScenarioResult>();
        if (payload) {
          payload->response.active_scenarios_valid = qmi_response.active_scenarios_valid;
          payload->response.active_scenarios_len = qmi_response.active_scenarios_len;
          memcpy(payload->response.active_scenarios, qmi_response.active_scenarios,
                 std::min(sizeof(payload->response.active_scenarios),
                          sizeof(qmi_response.active_scenarios)));
        }
      }
    }
    auto respData = std::make_shared<QcRilRequestMessageCallbackPayload>(ril_req_res, payload);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
  }
  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_nas_get_provisioned_table_revision_req
//===========================================================================
void qcril_qmi_nas_get_provisioned_table_revision_req(
    std::shared_ptr<RilRequestGetProvTableRevMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg) {
    std::shared_ptr<qcril::interfaces::GetProvTableRevResult> payload;
    RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
    rfrpe_get_provisioned_table_revision_resp_v01 qmi_response;

    memset(&qmi_response, 0, sizeof(qmi_response));

    qmi_client_error_type qmi_client_error = qmi_client_send_msg_sync_with_shm(
        qcril_qmi_client_get_user_handle(QCRIL_QMI_CLIENT_RFPE),
        QMI_RFRPE_GET_PROVISIONED_TABLE_REVISION_REQ_V01, NULL, QMI_RIL_ZERO, (void*)&qmi_response,
        sizeof(qmi_response), QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);
    ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_client_error,
                                                                          &qmi_response.resp);
    QCRIL_LOG_INFO("ril_req_res=%d, provision table valid=%d, provision table revision = %d",
                   ril_req_res, qmi_response.provisioned_table_revision_valid,
                   qmi_response.provisioned_table_revision);
    QCRIL_LOG_INFO("provisioned_table_OEM_valid=%d, provisioned_table_OEM_len=%d",
                   qmi_response.provisioned_table_OEM_valid, qmi_response.provisioned_table_OEM_len);

    if (ril_req_res == RIL_E_SUCCESS) {
      payload = std::make_shared<qcril::interfaces::GetProvTableRevResult>();
      if (payload) {
        payload->response.provisioned_table_revision_valid =
            qmi_response.provisioned_table_revision_valid;
        payload->response.provisioned_table_revision = qmi_response.provisioned_table_revision;
        payload->response.provisioned_table_OEM_valid = qmi_response.provisioned_table_OEM_valid;
        payload->response.provisioned_table_OEM_len = qmi_response.provisioned_table_OEM_len;

        for (size_t i = 0; i < qmi_response.provisioned_table_OEM_len; i++) {
          QCRIL_LOG_INFO("Provisioned table (OEM) %d is %d", i,
                         qmi_response.provisioned_table_OEM[i]);
        }

        memcpy(payload->response.provisioned_table_OEM, qmi_response.provisioned_table_OEM,
               std::min(sizeof(payload->response.provisioned_table_OEM),
                        sizeof(qmi_response.provisioned_table_OEM)));
      }
    }
    auto respData = std::make_shared<QcRilRequestMessageCallbackPayload>(ril_req_res, payload);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
  }
  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_nas_set_rfm_scenario_req
//===========================================================================
void qcril_qmi_nas_set_rfm_scenario_req(std::shared_ptr<RilRequestSetRfmScenarioMessage> msg) {
  RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error;

  QCRIL_LOG_FUNC_ENTRY();
  if (msg) {
    const Qtuner_set_scenario_req_v01& ril_request = msg->getRequest();
    rfrpe_set_scenario_req_v01 qmi_request;
    rfrpe_set_scenario_resp_v01 qmi_response;

    memset(&qmi_request, 0, sizeof(qmi_request));
    memset(&qmi_response, 0, sizeof(qmi_response));

    QCRIL_LOG_INFO("Qtuner set rfpe scenarios len %d", ril_request.scenarios_len);

    qmi_request.scenarios_len = ril_request.scenarios_len;

    if (ril_request.scenarios_len <= Qtuner_CONCURRENT_SCENARIOS_MAX_V01) {
      for (int scenario_idx = 0; scenario_idx < (int)ril_request.scenarios_len; scenario_idx++) {
        QCRIL_LOG_INFO("Qtuner set rfpe scenario %d = %d", scenario_idx,
                       ril_request.scenarios[scenario_idx]);
        qmi_request.scenarios[scenario_idx] = ril_request.scenarios[scenario_idx];
      }

      qmi_client_error = qmi_client_send_msg_sync_with_shm(
          qcril_qmi_client_get_user_handle(QCRIL_QMI_CLIENT_RFPE),
          QMI_RFRPE_SET_RFM_SCENARIO_REQ_V01, &qmi_request, sizeof(qmi_request),
          (void*)&qmi_response, sizeof(qmi_response), QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);
      ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_client_error,
                                                                            &qmi_response.resp);
      QCRIL_LOG_INFO("Qtuner set rfpe qmi code=%d", ril_req_res);
    }
    auto respData = std::make_shared<QcRilRequestMessageCallbackPayload>(ril_req_res, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respData);
  }
  QCRIL_LOG_FUNC_RETURN();
}
