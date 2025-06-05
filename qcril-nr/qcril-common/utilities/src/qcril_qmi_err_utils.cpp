/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "utils_internal.h"
#include "qcril_qmi_err_utils.h"
#include "voice_service_v02.h"
#include "device_management_service_v01.h"

//===========================================================================
// qcril_qmi_util_convert_qmi_response_codes_to_ril_result
//===========================================================================
RIL_Errno qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
    qmi_client_error_type qmi_transport_error, qmi_response_type_v01* qmi_service_response) {
  return qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
      qmi_transport_error, qmi_service_response, QCRIL_QMI_ERR_CTX_NONE, NULL);
}
//===========================================================================
// qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex
//===========================================================================
RIL_Errno qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
    qmi_client_error_type qmi_transport_error, qmi_response_type_v01* qmi_service_response,
    qmi_ril_err_context_e_type context, void* any) {
  int res;

  int res_settled;

  voice_dial_call_resp_msg_v02* dial_call_resp_msg;
  voice_orig_ussd_resp_msg_v02* ussd_resp_msg;
  voice_manage_ip_calls_resp_msg_v02* mng_ip_call_rsp_msg;
  qmi_ril_err_ctx_ss_resp_data_type* ss_resp_info;

  switch (qmi_transport_error) {
    case QMI_NO_ERR:  // fallthrough
    case QMI_SERVICE_ERR:
      res_settled = FALSE;

      UTIL_LOG_INFO("ctx check %d", (int)context);
      switch (context) {
        case QCRIL_QMI_ERR_CTX_MNG_IP_TXN:
          mng_ip_call_rsp_msg = (voice_manage_ip_calls_resp_msg_v02*)any;
          UTIL_LOG_INFO("ctx mng_ip %p", mng_ip_call_rsp_msg);
          if (mng_ip_call_rsp_msg && mng_ip_call_rsp_msg->failure_cause_valid) {
            UTIL_LOG_INFO("ctx mng_ip failure_cause 0x%x", mng_ip_call_rsp_msg->failure_cause);
            switch (mng_ip_call_rsp_msg->failure_cause) {
              case QMI_FAILURE_CAUSE_HOLD_RESUME_FAILED_V02:
                res = RIL_E_GENERIC_FAILURE;  // RIL_E_HOLD_RESUME_FAILED;
                res_settled = TRUE;
                break;
              case QMI_FAILURE_CAUSE_HOLD_RESUME_CANCELED_V02:
                res = RIL_E_GENERIC_FAILURE;  // RIL_E_HOLD_RESUME_CANCELED;
                res_settled = TRUE;
                break;
              case QMI_FAILURE_CAUSE_REINVITE_COLLISION_V02:
                res = RIL_E_GENERIC_FAILURE;  // RIL_E_REINVITE_COLLISION;
                res_settled = TRUE;
                break;
              default:
                break;
            }
          }
          break;
        case QCRIL_QMI_ERR_CTX_DIAL_TXN:
          dial_call_resp_msg = (voice_dial_call_resp_msg_v02*)any;
          UTIL_LOG_INFO("ctx dial %p", dial_call_resp_msg);
          if (dial_call_resp_msg && dial_call_resp_msg->cc_result_type_valid) {
            UTIL_LOG_INFO("ctx dial cc result type %d", (int)dial_call_resp_msg->cc_result_type);
            switch (dial_call_resp_msg->cc_result_type) {
              case VOICE_CC_RESULT_TYPE_VOICE_V02:
                res = RIL_E_DIAL_MODIFIED_TO_DIAL;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_SUPS_V02:
                res = RIL_E_DIAL_MODIFIED_TO_SS;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_USSD_V02:
                res = RIL_E_DIAL_MODIFIED_TO_USSD;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_VT_V02:
                res = RIL_E_DIAL_MODIFIED_TO_DIAL_VIDEO;
                res_settled = TRUE;
                break;

              default:
                // skip
                break;
            }
          }
          break;

        case QCRIL_QMI_ERR_CTX_DIAL_VIDEO_TXN:
          dial_call_resp_msg = (voice_dial_call_resp_msg_v02*)any;
          UTIL_LOG_INFO("ctx dial video %p", dial_call_resp_msg);
          if (dial_call_resp_msg && dial_call_resp_msg->cc_result_type_valid) {
            UTIL_LOG_INFO("ctx dial cc result type %d", (int)dial_call_resp_msg->cc_result_type);
            switch (dial_call_resp_msg->cc_result_type) {
              case VOICE_CC_RESULT_TYPE_VOICE_V02:
                res = RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_SUPS_V02:
                res = RIL_E_DIAL_VIDEO_MODIFIED_TO_SS;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_USSD_V02:
                res = RIL_E_DIAL_VIDEO_MODIFIED_TO_USSD;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_VT_V02:
                res = RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
                res_settled = TRUE;
                break;

              default:
                // skip
                break;
            }
          }
          break;

        case QCRIL_QMI_ERR_CTX_SEND_SS_TXN:
          ss_resp_info = (qmi_ril_err_ctx_ss_resp_data_type*)any;
          if (ss_resp_info && ss_resp_info->cc_result_type_valid) {
            switch (*ss_resp_info->cc_result_type) {
              case VOICE_CC_RESULT_TYPE_VOICE_V02:
                res = RIL_E_SS_MODIFIED_TO_DIAL;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_SUPS_V02:
                res = RIL_E_SS_MODIFIED_TO_SS;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_USSD_V02:
                res = RIL_E_SS_MODIFIED_TO_USSD;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_VT_V02:
                res = RIL_E_SS_MODIFIED_TO_DIAL_VIDEO;
                res_settled = TRUE;
                break;

              default:
                // skip
                break;
            }
          }
          break;

        case QCRIL_QMI_ERR_CTX_SEND_USSD_TXN:
          ussd_resp_msg = (voice_orig_ussd_resp_msg_v02*)any;
          if (ussd_resp_msg && ussd_resp_msg->cc_result_type_valid) {
            switch (ussd_resp_msg->cc_result_type) {
              case VOICE_CC_RESULT_TYPE_VOICE_V02:
                res = RIL_E_USSD_MODIFIED_TO_DIAL;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_SUPS_V02:
                res = RIL_E_USSD_MODIFIED_TO_SS;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_USSD_V02:
                res = RIL_E_USSD_MODIFIED_TO_USSD;
                res_settled = TRUE;
                break;

              case VOICE_CC_RESULT_TYPE_VT_V02:
                res = RIL_E_USSD_MODIFIED_TO_DIAL_VIDEO;
                res_settled = TRUE;
                break;

              default:
                // skip
                break;
            }
          }
          break;
        case QCRIL_QMI_ERR_CTX_RADIO_POWER_TXN:
          if (qmi_service_response && qmi_service_response->result == QMI_RESULT_FAILURE_V01 &&
              qmi_service_response->error == QMI_ERR_DEVICE_NOT_READY_V01) {
            res = RIL_E_SUCCESS;
            res_settled = TRUE;
          } else {
            auto set_opmode_resp = static_cast<dms_set_operating_mode_resp_msg_v01*>(any);
            if (set_opmode_resp && set_opmode_resp->fail_reason_valid) {
              switch (set_opmode_resp->fail_reason) {
                case DMS_DEVICE_STATE_FAIL_RF_SW_V01:
                  res = RIL_E_RF_SOFTWARE_ISSUE;
                  break;
                case DMS_DEVICE_STATE_FAIL_RF_HW_V01:
                  res = RIL_E_RF_HARDWARE_ISSUE;
                  break;
                case DMS_DEVICE_STATE_FAIL_GPS_V01:
                  res = RIL_E_MODEM_ERR;
                  break;
                default:
                  res = RIL_E_GENERIC_FAILURE;
              }
              res_settled = TRUE;
            }
          }
          break;

        default:
          // skip
          break;
      }

      if (!res_settled) {
        if (NULL == qmi_service_response) {
          res = RIL_E_SUCCESS;
        } else {
          switch (qmi_service_response->result) {
            case QMI_RESULT_SUCCESS_V01:
              res = RIL_E_SUCCESS;
              break;

            case QMI_RESULT_FAILURE_V01:
              if (qmi_service_response) {
                switch (qmi_service_response->error) {
                  case QMI_ERR_NONE_V01:
                  case QMI_ERR_NO_EFFECT_V01:
                    res = RIL_E_SUCCESS;
                    break;

                  case QMI_ERR_NO_NETWORK_FOUND_V01:
                    switch (context) {
                      case QCRIL_QMI_ERR_CTX_CELL_LOCATION_TXN:
                        res = RIL_E_SUCCESS;
                        break;
                      default:
                        res = RIL_E_NO_NETWORK_FOUND;
                        break;
                    }
                    break;

                  case QMI_ERR_NO_MEMORY_V01:
                    res = RIL_E_NO_MEMORY;
                    break;

                  case QMI_ERR_INTERNAL_V01:
                    res = RIL_E_MODEM_ERR;
                    break;

                  case QMI_ERR_MALFORMED_MSG_V01:
                    res = RIL_E_INTERNAL_ERR;
                    break;

                  case QMI_ERR_NOT_PROVISIONED_V01:
                    res = RIL_E_NOT_PROVISIONED;
                    break;

                  case QMI_ERR_MISSING_ARG_V01:
                  case QMI_ERR_ARG_TOO_LONG_V01:  // fallthrough
                  case QMI_ERR_INVALID_ARG_V01:
                    res = RIL_E_INVALID_ARGUMENTS;
                    break;

                  case QMI_ERR_OP_DEVICE_UNSUPPORTED_V01:
                    res = RIL_E_REQUEST_NOT_SUPPORTED;
                    break;

                  case QMI_ERR_CLIENT_IDS_EXHAUSTED_V01:     // fallthrough
                  case QMI_ERR_UNABORTABLE_TRANSACTION_V01:  // fallthrough
                  case QMI_ERR_INVALID_CLIENT_ID_V01:        // fallthrough
                  case QMI_ERR_INVALID_HANDLE_V01:
                  case QMI_ERR_INVALID_PROFILE_V01:
                  case QMI_ERR_OUT_OF_CALL_V01:
                  case QMI_ERR_INVALID_TX_ID_V01:
                  case QMI_ERR_NO_FREE_PROFILE_V01:
                  case QMI_ERR_INVALID_PDP_TYPE_V01:
                  case QMI_ERR_INVALID_TECH_PREF_V01:
                  case QMI_ERR_CARD_CALL_CONTROL_FAILED_V01:
                  case QMI_ERR_ACK_NOT_SENT_V01:
                  case QMI_ERR_CALL_FAILED_V01:
                    res = RIL_E_MODEM_ERR;
                    break;

                  case QMI_ERR_ABORTED_V01:
                    res = RIL_E_CANCELLED;
                    break;

                  case QMI_ERR_FDN_RESTRICT_V01:
                    res = RIL_E_FDN_CHECK_FAILURE;
                    break;

                  case QMI_ERR_INFO_UNAVAILABLE_V01:
                    switch (context) {
                      case QCRIL_QMI_ERR_TOLERATE_NOT_FOUND:
                        res = RIL_E_SUCCESS;
                        break;

                      default:
                        res = RIL_E_NOT_PROVISIONED;
                        break;
                    }
                    break;

                  case QMI_ERR_DEVICE_NOT_READY_V01:
                  {
                    res = (context == QCRIL_QMI_ERR_TOLERATE_NOT_READY) ?
                            RIL_E_SUCCESS : RIL_E_INVALID_MODEM_STATE;
                    break;
                  }

                  case QMI_ERR_ENCODING_V01:
                    res = RIL_E_ENCODING_ERR;
                    break;

                  case QMI_ERR_SMSC_ADDR_V01:
                    res = RIL_E_INVALID_SMSC_ADDRESS;
                    break;

                  case QMI_ERR_DEVICE_STORAGE_FULL_V01:
                    res = RIL_E_NO_RESOURCES;
                    break;

                  case QMI_ERR_NETWORK_NOT_READY_V01:
                    res = RIL_E_NETWORK_NOT_READY;
                    break;

                  case QMI_ERR_INVALID_OPERATION_V01:
                  case QMI_ERR_INVALID_TRANSITION_V01:
                  case QMI_ERR_OP_NETWORK_UNSUPPORTED_V01:
                    res = RIL_E_OPERATION_NOT_ALLOWED;
                    break;

                  case QMI_ERR_NO_ENTRY_V01:
                    res = RIL_E_NO_SUCH_ENTRY;
                    break;

                  case QMI_ERR_DEVICE_IN_USE_V01:
                    res = RIL_E_DEVICE_IN_USE;
                    break;

                  case QMI_ERR_INCOMPATIBLE_STATE_V01:
                    res = RIL_E_INVALID_MODEM_STATE;
                    break;

                  case QMI_ERR_NO_RADIO_V01:
                    /* Nas and voice qmi documents have different perception
                       for this error QMI_ERR_NO_RADIO. Hence using MODEM_ERR
                       as a common ground */
                    res = RIL_E_MODEM_ERR;
                    break;

                  case QMI_ERR_NOT_SUPPORTED_V01:
                    /* This error is sent by the modem when the Request is
                       "currently" not supported. Using MODEM_ERR as this
                        pertains to modem not being able to perform the request
                        currently */
                    res = RIL_E_MODEM_ERR;
                    break;

                  case QMI_ERR_NETWORK_ABORTED_V01:
                    res = RIL_E_ABORTED;
                    break;

                  case QMI_ERR_NO_SUBSCRIPTION_V01:
                    res = RIL_E_NO_SUBSCRIPTION;
                    break;

                  case QMI_ERR_HARDWARE_RESTRICTED_V01:
                    res = RIL_E_SYSTEM_ERR;
                    break;

                  case QMI_ERR_INVALID_QMI_CMD_V01:
                    res = RIL_E_REQUEST_NOT_SUPPORTED;
                    break;

                  default:
                    res = RIL_E_MODEM_ERR;
                    break;
                }
              } else {
                res = RIL_E_SYSTEM_ERR;
              }
              break;

            default:
              res = RIL_E_SYSTEM_ERR;
              break;
          }
        }
      }
      break;

    case QMI_INTERNAL_ERR:
      res = RIL_E_SYSTEM_ERR;
      break;

    default:
      res = RIL_E_SYSTEM_ERR;
      break;
  }

  return (RIL_Errno)res;
}  // qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex
