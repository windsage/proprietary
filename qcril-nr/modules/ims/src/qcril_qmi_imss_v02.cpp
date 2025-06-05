/******************************************************************************
#  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#  All rights reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_imss_v02.c
  @brief   qcril qmi - IMS Setting

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI IMS Setting.

******************************************************************************/
#define TAG "RILQ"

#include <framework/Log.h>
#include <cutils/properties.h>
#include <string.h>
#include "qcrili.h"
#include "qcril_qmi_client.h"
#include "qcril_qmi_imss.h"
#include "qcril_qmi_imss_qmi.h"
#include "qcril_qmi_imss_v02.h"
#include "ip_multimedia_subsystem_settings_v01.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_npb_utils.h"
#include "qcril_qmi_radio_config_imss.h"
#include <cutils/properties.h>
#include "modules/nas/NasSetVoiceDomainPreferenceRequest.h"

#include <interfaces/common.h>
#include <interfaces/ims/ims.h>
#include <interfaces/ims/QcRilUnsolImsRttMessage.h>
#include "ImsModule.h"
#include "qcril_qmi_ims_utils.h"
#include "qcril_qmi_err_utils.h"
#include "qcril_memory_management.h"

extern struct ims_cached_info_type qcril_qmi_ims_cached_info;
extern boolean feature_voice_dom_pref_on_toggle_ims_cap;
extern boolean feature_disabled_modem_req;

void qcril_qmi_imss_ims_service_enable_config_ind_hdlr(void *ind_data_ptr);

RIL_Errno qcril_qmi_imss_send_set_ims_service_enable_config_req
(
 const ims_settings_set_ims_service_enable_config_req_msg_v01 *qmi_req,
 uint32 user_data,
 qmiAsyncCbType resp_cb
);

void qcril_qmi_imss_set_ims_service_enable_config_resp_ims_reg_change_hdlr
(
 unsigned int                 msg_id,
 std::shared_ptr<void>        resp_c_struct,
 unsigned int                 resp_c_struct_len,
 void                        *resp_cb_data,
 qmi_client_error_type        transp_err
);

qcril_ims_setting_wfc_status_type qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02
(
  uint8_t wfc_status
);

qcril_ims_setting_wfc_preference_type qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02
(
  ims_settings_call_mode_option_enum_v01 wfc_preference
);

//===========================================================================
// qcril_qmi_imss_request_set_ims_registration_v02
//===========================================================================
void qcril_qmi_imss_request_set_ims_registration_v02(
    std::shared_ptr<QcRilRequestImsRegistrationChangeMessage> msg)
{
  qcril::interfaces::RegState imsRegState = qcril::interfaces::RegState::UNKNOWN;
  ims_settings_set_ims_service_enable_config_req_msg_v01 qmi_req;
  uint32 user_data;
  bool sendResponse = false;
  RIL_Errno errorCode = RIL_E_SUCCESS;
  uint16_t req_id = -1;
  bool doCleanup = false;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    imsRegState = msg->getRegState();
    //QCRIL_LOG_INFO("state: %d", imsRegState);

    if(feature_disabled_modem_req)
    {
      QCRIL_LOG_INFO("Modem IMS config is disabled. Respond to Telephony with success");
      errorCode = RIL_E_SUCCESS;
      sendResponse = true;
      break;
    }

    if(imsRegState == qcril::interfaces::RegState::UNKNOWN)
    {
      QCRIL_LOG_ERROR("Invalid parameters: state is not present; Send failure");
      errorCode = RIL_E_GENERIC_FAILURE;
      sendResponse = true;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));
    qmi_req.ims_service_enabled_valid = TRUE;
    qmi_req.ims_service_enabled =
        (imsRegState == qcril::interfaces::RegState::REGISTERED ? TRUE : FALSE);

    QCRIL_LOG_INFO("ims_service_enabled = %s", qmi_req.ims_service_enabled ? "TRUE" : "FALSE");
    auto pendingMsgStatus = getImsModule()->getPendingMessageList().insert(msg);
    if (pendingMsgStatus.second != true) {
      break;
    }
    req_id = pendingMsgStatus.first;
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    errorCode = qcril_qmi_imss_send_set_ims_service_enable_config_req(&qmi_req, user_data,
        qcril_qmi_imss_set_ims_service_enable_config_resp_ims_reg_change_hdlr);
    if (errorCode != RIL_E_SUCCESS)
    {
      sendResponse = true;
      doCleanup = true;
    }
  } while (FALSE);

  if (sendResponse)
  {
    auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(errorCode, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    if (doCleanup) {
      getImsModule()->getPendingMessageList().erase(req_id);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_set_ims_registration_v02

//===========================================================================
// qcril_qmi_imss_send_set_ims_service_enable_config_req
//===========================================================================
RIL_Errno qcril_qmi_imss_send_set_ims_service_enable_config_req
(
 const ims_settings_set_ims_service_enable_config_req_msg_v01 *qmi_req,
 uint32 user_data,
 qmiAsyncCbType resp_cb
)
{
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;
  RIL_Errno res = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();

  qmi_client_error = qmi_client_imss_send_async(
      QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01,
      (void *)qmi_req,
      sizeof(ims_settings_set_ims_service_enable_config_req_msg_v01),
      sizeof(ims_settings_set_ims_service_enable_config_rsp_msg_v01),
      resp_cb,
      (void*)(uintptr_t)user_data );

  QCRIL_LOG_INFO(".. qmi send async res %d", (int) qmi_client_error);

  if (qmi_client_error == QMI_NO_ERR)
  {
    res = RIL_E_SUCCESS;
  }

  QCRIL_LOG_FUNC_RETURN();

  return res;
} // qcril_qmi_imss_send_set_ims_service_enable_config_req

//===========================================================================
// qcril_qmi_imss_request_set_ims_srv_status_v02
//===========================================================================
void qcril_qmi_imss_request_set_ims_srv_status_v02(
    std::shared_ptr<QcRilRequestImsSetServiceStatusMessage> msg) {
  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  QCRIL_LOG_FUNC_ENTRY();
  if (msg != nullptr) {
    do {
      if (feature_disabled_modem_req) {
        QCRIL_LOG_INFO("Modem IMS config is disabled. Respond to Telephony with success");
        res = RIL_E_SUCCESS;
        break;
      }
      ims_settings_set_ims_service_enable_config_req_msg_v01 qmi_req = {};
      if (msg->hasVolteEnabled()) {
        qmi_req.volte_enabled_valid = TRUE;
        qmi_req.volte_enabled = msg->getVolteEnabled();
      }
      if (msg->hasVideoTelephonyEnabled()) {
        qmi_req.videotelephony_enabled_valid = TRUE;
        qmi_req.videotelephony_enabled = msg->getVideoTelephonyEnabled();
      }
      if (msg->hasWifiCallingEnabled()) {
        qmi_req.wifi_calling_enabled_valid = TRUE;
        qmi_req.wifi_calling_enabled = msg->getWifiCallingEnabled();
      }
      if (msg->hasUTEnabled()) {
        qmi_req.ut_enabled_valid = TRUE;
        qmi_req.ut_enabled = msg->getUTEnabled();
      }
      if (!qmi_req.volte_enabled_valid && !qmi_req.videotelephony_enabled_valid &&
          !qmi_req.wifi_calling_enabled_valid && !qmi_req.ut_enabled_valid) {
        QCRIL_LOG_INFO("Invalid parameters");
        res = RIL_E_INVALID_ARGUMENTS;
        break;
      }
      auto pendingMsgStatus = getImsModule()->getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true) {
        break;
      }
      uint32 user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID,
                                                 pendingMsgStatus.first);
      if (feature_voice_dom_pref_on_toggle_ims_cap && qmi_req.volte_enabled_valid &&
          qmi_req.volte_enabled) {
        // Make sure to send the response only when we set voice_domain_preference and volte_enabled.
        auto setVoiceDomainPrefMsg = std::make_shared<NasSetVoiceDomainPreferenceRequest>(
            NAS_VOICE_DOMAIN_PREF_PS_PREF_V01,
            [msg, qmi_req, user_data](std::shared_ptr<Message> solicitedMsg,
                                      Message::Callback::Status status,
                                      std::shared_ptr<RIL_Errno> result) -> void {
              RIL_Errno res = RIL_E_GENERIC_FAILURE;
              if (status == Message::Callback::Status::SUCCESS && result) {
                res = *result;
              }
              QCRIL_LOG_DEBUG("%s: result = %d", solicitedMsg->dump().c_str(), res);
              if (res == RIL_E_SUCCESS) {
                res = qcril_qmi_imss_send_set_ims_service_enable_config_req(
                    &qmi_req, user_data, qcril_qmi_imss_set_ims_service_enable_config_resp_hdlr);
              } else {
                msg->setPendingMessageState(PendingMessageState::COMPLETED_FAILURE);
              }
              if (res != RIL_E_SUCCESS) {
                msg->sendResponse(
                    msg, Message::Callback::Status::SUCCESS,
                    std::make_shared<QcRilRequestMessageCallbackPayload>(res, nullptr));
                getImsModule()->getPendingMessageList().erase(msg);
              }
            });
        if (setVoiceDomainPrefMsg) {
          setVoiceDomainPrefMsg->dispatch();
          res = RIL_E_SUCCESS;
        }
        break;
      }
      res = qcril_qmi_imss_send_set_ims_service_enable_config_req(
          &qmi_req, user_data, qcril_qmi_imss_set_ims_service_enable_config_resp_hdlr);
    } while (FALSE);
    if (feature_disabled_modem_req || RIL_E_SUCCESS != res) {
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
                        std::make_shared<QcRilRequestMessageCallbackPayload>(res, nullptr));
      getImsModule()->getPendingMessageList().erase(msg);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_imss_request_set_ims_srv_status_v02

//===========================================================================
// qcril_qmi_imss_request_set_ims_config_v02
//===========================================================================
void qcril_qmi_imss_request_set_ims_config_v02
(
  std::shared_ptr<QcRilRequestImsSetConfigMessage> msg
)
{
  uint32 user_data;
  qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;

  /* Radio config mapping related types.
   Needed for processing the request*/
  qcril_qmi_radio_config_params_type config_params;
  qcril_qmi_ims_config_item_value_type config_item_type;
  qcril_qmi_radio_config_handler_type* req_handler = NULL;
  qcril_qmi_ims_config_error_type radio_config_error =
          QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  /* To Convert the int value to bool  */
  bool bool_value;
  bool sendResponse = false;
  uint16_t req_id = 0;
  std::string strVal;
  uint32_t intVal = 0;

  QCRIL_LOG_FUNC_ENTRY();
  if (msg != nullptr) {
    do {
      if(feature_disabled_modem_req)
      {
        QCRIL_LOG_INFO("Modem IMS config is disabled. Respond to Telephony with success");
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
        auto config = std::make_shared<qcril::interfaces::ConfigInfo>();
        if (config) {
          config->setItem(msg->getConfigInfo().getItem());
        }
        auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(
                                RIL_E_SUCCESS, config);
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
        break;
      }

      auto pendingMsgStatus = getImsModule()->getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true)
      {
          sendResponse = true;
          break;
      }
      req_id = pendingMsgStatus.first;

      qcril::interfaces::ConfigInfo &configInfo = msg->getConfigInfo();
      if (!configInfo.hasItem()) {
        sendResponse = true;
        QCRIL_LOG_ERROR("Config Item is not present in the request, ..sending failure");
        break;
      }

      config_params.config_item =
        qcril_qmi_ims_map_ims_config_to_radio_config_item(configInfo.getItem());

      if(config_params.config_item == QCRIL_QMI_IMS_CONFIG_INVALID)
      {
        QCRIL_LOG_ERROR("Invalid config item: %d. Doesnt exist in radio config",
                         configInfo.getItem());
        sendResponse = true;
        break;
      }

      user_data = QCRIL_COMPOSE_USER_DATA( instance_id,
                                           QCRIL_DEFAULT_MODEM_ID,
                                           req_id );

      QCRIL_LOG_INFO("processing request - config item: %s",
                   qcril_qmi_radio_config_get_item_log_str(config_params.config_item));
      config_item_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);
      // If item value type is boolean and input params has int val
      // As per imsproto for now the items coming in Boolean are placed
      // in intValue as there is no corresponding API in ImsConfig.java
      if((config_item_type == QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN)
          && (configInfo.hasIntValue()))
      {
        QCRIL_LOG_INFO("Config item received is of boolean type in intValue, value: %d",
                        configInfo.getIntValue());
        bool_value = (configInfo.getIntValue() ? TRUE : FALSE );
        config_params.config_item_value_len = sizeof(bool_value);
        config_params.config_item_value = (void *)(&bool_value);
      }
      else if ((config_item_type == QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT)
                           && configInfo.hasIntValue()) {
        QCRIL_LOG_INFO("Config item received is an integer, value: %d",
                        configInfo.getIntValue());
        intVal = configInfo.getIntValue();
        config_params.config_item_value_len = sizeof(intVal);
        config_params.config_item_value = (void *)(&intVal);
      }
      else if((config_item_type == QCRIL_QMI_RADIO_CONFIG_ITEM_IS_STRING)
                          && (configInfo.hasStringValue())) {
        strVal = configInfo.getStringValue();
        QCRIL_LOG_INFO("Config item received is a string, value: %s", strVal.c_str());
        config_params.config_item_value_len = strlen(strVal.c_str());
        config_params.config_item_value = (void *) strVal.c_str();
        //TODO check string
      }
      else
      {
         QCRIL_LOG_ERROR("..invalid parameters for the config items value");
         radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
         sendResponse = true;
         break;
      }
      config_params.extra_data_len = sizeof(user_data);
      config_params.extra_data = (void *)(uintptr_t)user_data;
      config_params.config_item_value_type = config_item_type;
      /* Get the set request handler and call it with config params */
      req_handler = qcril_qmi_radio_config_find_set_config_req_handler(config_params.config_item);
      if(req_handler == NULL)
      {
        QCRIL_LOG_ERROR("NULL req handler for the item");
        sendResponse = true;
        break;
      }
      radio_config_error = (req_handler)(&config_params, req_id);
      QCRIL_LOG_INFO("Returned from req handler with radio_config_error: %d", radio_config_error);
    } while(FALSE);

    if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      QCRIL_LOG_ERROR("radio config error: %d..sending empty response", radio_config_error);
      qcril_qmi_imss_set_ims_config_log_and_send_response(req_id,
                                                    NULL,
                                                    radio_config_error,
                                                    QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_set_ims_config_v02

//===========================================================================
// qcril_qmi_imss_request_get_ims_config_v02
//===========================================================================
void qcril_qmi_imss_request_get_ims_config_v02
(
    std::shared_ptr<QcRilRequestImsGetConfigMessage> msg
)
{
  uint32 user_data;
  qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;

  /* Radio config mapping related types. */
  qcril_qmi_radio_config_params_type config_params;
  qcril_qmi_radio_config_handler_type* req_handler = NULL;
  qcril_qmi_ims_config_error_type radio_config_error =
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  uint16_t req_id = 0;

  QCRIL_LOG_FUNC_ENTRY();
  if (msg != nullptr) {
    do {
      auto pendingMsgStatus = getImsModule()->getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true)
      {
          break;
      }
      req_id = pendingMsgStatus.first;

      qcril::interfaces::ConfigInfo &configInfo = msg->getConfigInfo();
      if (!configInfo.hasItem()) {
        QCRIL_LOG_ERROR("Config Item is not present in the request, ..sending failure");
        break;
      }
      config_params.config_item =
        qcril_qmi_ims_map_ims_config_to_radio_config_item(configInfo.getItem());

      if(config_params.config_item == QCRIL_QMI_IMS_CONFIG_INVALID)
      {
        QCRIL_LOG_ERROR("Invalid config item: %d. Doesnt exist in radio config",
                         configInfo.getItem());
        break;
      }

      user_data = QCRIL_COMPOSE_USER_DATA( instance_id,
                                           QCRIL_DEFAULT_MODEM_ID,
                                           req_id );
      QCRIL_LOG_INFO("processing request - config item: %s",
                   qcril_qmi_radio_config_get_item_log_str(config_params.config_item));
      config_params.config_item_value_len = 0;
      config_params.config_item_value = NULL;
      config_params.extra_data_len = sizeof(user_data);
      config_params.extra_data = (void *)(uintptr_t)user_data;

      /* Get the get request handler and call it with config params */
      req_handler = qcril_qmi_radio_config_find_get_config_req_handler(config_params.config_item);
      if(req_handler == NULL)
      {
        QCRIL_LOG_ERROR("NULL req handler for the item");
        break;
      }

      radio_config_error = (req_handler)(&config_params, req_id);

      QCRIL_LOG_INFO("Returned from req handler with radio_config_error: %d",
                      radio_config_error);
    } while(FALSE);
    if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      QCRIL_LOG_ERROR("radio config error: %d..sending empty response", radio_config_error);
      qcril_qmi_imss_get_ims_config_log_and_send_response(req_id,
                                                      NULL,
                                                      radio_config_error,
                                                      QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} //qcril_qmi_imss_request_get_ims_config_v02

//===========================================================================
// qcril_qmi_imss_config_resp_cb
//===========================================================================
void qcril_qmi_imss_config_resp_cb_v02
(
   unsigned int                 msg_id,
   std::shared_ptr<void>        resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
)
{
 int config_item;

 uint32 user_data = (uint32)(uintptr_t) resp_cb_data;
 uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
 auto pendingMsg = getImsModule()->getPendingMessageList().find(req_id);

 qcril_qmi_radio_config_resp_data_type req_data;
 QCRIL_LOG_FUNC_ENTRY();
  do {
    if (pendingMsg == nullptr)
    {
      QCRIL_LOG_ERROR("pendingMsg is null");
      break;
    }
    if (transp_err != QMI_NO_ERR)
    {
      QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI", transp_err);
      break;
    }
    if (resp_c_struct_len == 0)
    {
      QCRIL_LOG_ERROR("response length is zero");
      break;
    }
    req_data.data_len = resp_c_struct_len;
    req_data.data = resp_c_struct.get();
    if (pendingMsg->get_message_id() ==
               QcRilRequestImsSetConfigMessage::get_class_message_id()) {
      auto msg(std::static_pointer_cast<QcRilRequestImsSetConfigMessage>(pendingMsg));
      qcril::interfaces::ConfigInfo &configInfo = msg->getConfigInfo();
      config_item =
        qcril_qmi_ims_map_ims_config_to_radio_config_item(configInfo.getItem());
      QCRIL_LOG_INFO("Calling config dispatch response helper, Config_item: %d", config_item);
      qcril_qmi_imss_dispatch_config_response_helper(&req_data, config_item, msg_id, req_id);
    }
    if (pendingMsg->get_message_id() ==
               QcRilRequestImsGetConfigMessage::get_class_message_id())
    {
      auto msg(std::static_pointer_cast<QcRilRequestImsGetConfigMessage>(pendingMsg));
      qcril::interfaces::ConfigInfo &configInfo = msg->getConfigInfo();
      config_item =
         qcril_qmi_ims_map_ims_config_to_radio_config_item(configInfo.getItem());
      QCRIL_LOG_INFO("Calling config dispatch response helper, Config_item: %d", config_item);
      qcril_qmi_imss_dispatch_config_response_helper(&req_data, config_item, msg_id, req_id);
    }
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN();

}

//===========================================================================
// qcril_qmi_imss_unsol_ind_cb_helper_v02
//===========================================================================
void qcril_qmi_imss_unsol_ind_cb_helper_v02
(
 unsigned int   msg_id,
 unsigned char *decoded_payload,
 uint32_t       decoded_payload_len
)
{

  QCRIL_LOG_FUNC_ENTRY();

  if (decoded_payload && decoded_payload_len)
  {
    switch(msg_id)
    {
      case QMI_IMS_SETTINGS_IMS_SERVICE_ENABLE_CONFIG_IND_V01:
        qcril_qmi_imss_ims_service_enable_config_ind_hdlr(decoded_payload);
        break;

      case QMI_IMS_SETTINGS_RTT_RCVD_DATA_IND_V01:
        qcril_qmi_imss_rtt_rcvd_data_ind_hdlr(decoded_payload);
        break;
      default:
        QCRIL_LOG_INFO("Unknown QMI IMSA indication %d", msg_id);
        break;
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_unsol_ind_cb_helper_v02

//===========================================================================
// qcril_qmi_imss_set_ims_service_enable_config_resp_hdlr
//===========================================================================
void qcril_qmi_imss_set_ims_service_enable_config_resp_hdlr
(
   unsigned int                 msg_id,
   std::shared_ptr<void>        resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
)
{
  (void)msg_id;
  (void)resp_c_struct_len;

  ims_settings_set_ims_service_enable_config_rsp_msg_v01 *resp;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;

  uint32 user_data = (uint32)(uintptr_t) resp_cb_data;
  uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
  auto pendingMsg = getImsModule()->getPendingMessageList().find(req_id);

  QCRIL_LOG_FUNC_ENTRY();

  do {
    if (pendingMsg == nullptr)
    {
      QCRIL_LOG_ERROR("pendingMsg is null");
      break;
    }
    if (transp_err != QMI_NO_ERR)
    {
      QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI", transp_err);
      break;
    }
    resp = (ims_settings_set_ims_service_enable_config_rsp_msg_v01*)(resp_c_struct.get());
    if (resp != nullptr)
    {
      ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &(resp->resp));
      QCRIL_LOG_ESSENTIAL("ril_err: %d, qmi res: %d", (int) ril_err, (int)resp->resp.error);
    } else {
      QCRIL_LOG_ERROR("resp is null");
      ril_err = RIL_E_GENERIC_FAILURE;
    }

    if (pendingMsg->get_message_id() ==
               QcRilRequestImsSetServiceStatusMessage::get_class_message_id()) {
      qcril_qmi_imss_handle_ims_set_service_status_event_resp(req_id, ril_err);
    }
  } while(FALSE);

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_set_ims_service_enable_config_resp_hdlr

//===========================================================================
// qcril_qmi_imss_set_ims_service_enable_config_resp_ims_reg_change_hdlr
//===========================================================================
void qcril_qmi_imss_set_ims_service_enable_config_resp_ims_reg_change_hdlr
(
 unsigned int                 msg_id,
 std::shared_ptr<void>        resp_c_struct,
 unsigned int                 resp_c_struct_len,
 void                        *resp_cb_data,
 qmi_client_error_type        transp_err
)
{
  (void)msg_id;
  (void)resp_c_struct_len;
  ims_settings_set_ims_service_enable_config_rsp_msg_v01 *resp;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32 user_data = (uint32)(uintptr_t) resp_cb_data;
  uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);

  QCRIL_LOG_FUNC_ENTRY();

  getImsModule()->getPendingMessageList().print();
  auto pendingMsg = getImsModule()->getPendingMessageList().extract(req_id);
  getImsModule()->getPendingMessageList().print();

  do {
    if (pendingMsg == nullptr)
    {
      QCRIL_LOG_ERROR("pendingMsg is null");
      break;
    }
    if (transp_err != QMI_NO_ERR)
    {
      QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI", transp_err);
      ril_err = RIL_E_GENERIC_FAILURE;
      break;
    }
    resp = (ims_settings_set_ims_service_enable_config_rsp_msg_v01*)(resp_c_struct.get());
    if (resp == nullptr)
    {
      QCRIL_LOG_ERROR("resp is null");
      ril_err = RIL_E_GENERIC_FAILURE;
      break;
    }
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &(resp->resp));
    QCRIL_LOG_ESSENTIAL("ril_err: %d, qmi res: %d", (int) ril_err, (int)resp->resp.error);
  } while (FALSE);

  if (pendingMsg != nullptr)
  {
    auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(ril_err, nullptr);
    auto msg(std::static_pointer_cast<QcRilRequestImsRegistrationChangeMessage>(pendingMsg));
    msg->sendResponse(pendingMsg, Message::Callback::Status::SUCCESS, respPayload);
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_set_ims_service_enable_config_resp_ims_reg_change_hdlr

//===========================================================================
// qcril_qmi_imss_client_ims_service_enable_config_ind_hdlr
//===========================================================================
void qcril_qmi_imss_ims_service_enable_config_ind_hdlr(void *ind_data_ptr)
{
  ims_settings_ims_service_enable_config_ind_msg_v01 *reg_ind_msg_ptr =
      (ims_settings_ims_service_enable_config_ind_msg_v01 *)ind_data_ptr;
  bool send_update_to_data = false;

  QCRIL_LOG_FUNC_ENTRY();

  if (NULL != reg_ind_msg_ptr)
  {
    qcril_qmi_imss_info_lock();

    if (reg_ind_msg_ptr->wifi_calling_enabled_valid)
    {
      qcril_qmi_ims_cached_info.wifi_calling_enabled_valid =
          reg_ind_msg_ptr->wifi_calling_enabled_valid;
      qcril_ims_setting_wfc_status_type old_value_status =
          qcril_qmi_ims_cached_info.wifi_calling_enabled;
      qcril_qmi_ims_cached_info.wifi_calling_enabled =
          qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02(reg_ind_msg_ptr->wifi_calling_enabled);

      if (old_value_status != qcril_qmi_ims_cached_info.wifi_calling_enabled)
      {
        send_update_to_data = true;
      }
    }
    if (reg_ind_msg_ptr->call_mode_preference_ext_valid)
    {
      qcril_qmi_ims_cached_info.call_mode_preference_valid =
          reg_ind_msg_ptr->call_mode_preference_ext_valid;
      qcril_ims_setting_wfc_preference_type old_value_pref =
          qcril_qmi_ims_cached_info.call_mode_preference;
      qcril_qmi_ims_cached_info.call_mode_preference =
          qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02(
              reg_ind_msg_ptr->call_mode_preference_ext);

      if (old_value_pref != qcril_qmi_ims_cached_info.call_mode_preference)
      {
        send_update_to_data = true;
      }
    }
    else if (reg_ind_msg_ptr->call_mode_preference_valid)
    {
      qcril_qmi_ims_cached_info.call_mode_preference_valid =
          reg_ind_msg_ptr->call_mode_preference_valid;
      qcril_ims_setting_wfc_preference_type old_value_pref =
          qcril_qmi_ims_cached_info.call_mode_preference;
      qcril_qmi_ims_cached_info.call_mode_preference =
          qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02(
              reg_ind_msg_ptr->call_mode_preference);

      if (old_value_pref != qcril_qmi_ims_cached_info.call_mode_preference)
      {
        send_update_to_data = true;
      }
    }
    if (reg_ind_msg_ptr->call_mode_roam_preference_ext_valid)
    {
      qcril_qmi_ims_cached_info.call_mode_roam_preference_valid =
          reg_ind_msg_ptr->call_mode_roam_preference_ext_valid;
      qcril_ims_setting_wfc_preference_type old_value_pref =
          qcril_qmi_ims_cached_info.call_mode_roam_preference;
      qcril_qmi_ims_cached_info.call_mode_roam_preference =
          qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02(
              reg_ind_msg_ptr->call_mode_roam_preference_ext);

      if (old_value_pref != qcril_qmi_ims_cached_info.call_mode_roam_preference)
      {
        send_update_to_data = true;
      }
    }
    else if (reg_ind_msg_ptr->call_mode_roam_preference_valid)
    {
      qcril_qmi_ims_cached_info.call_mode_roam_preference_valid =
          reg_ind_msg_ptr->call_mode_roam_preference_valid;
      qcril_ims_setting_wfc_preference_type old_value_pref =
          qcril_qmi_ims_cached_info.call_mode_roam_preference;
      qcril_qmi_ims_cached_info.call_mode_roam_preference =
          qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02(
              reg_ind_msg_ptr->call_mode_roam_preference);

      if (old_value_pref != qcril_qmi_ims_cached_info.call_mode_roam_preference)
      {
        send_update_to_data = true;
      }
    }
    qcril_qmi_ims_cached_info.service_mask_by_network_enabled_valid =
        reg_ind_msg_ptr->service_mask_by_network_enabled_valid;
    qcril_qmi_ims_cached_info.service_mask_by_network_enabled =
        reg_ind_msg_ptr->service_mask_by_network_enabled;

    QCRIL_LOG_INFO(".. wifi_call:(%d, %d), wifi_call_preference:(%d, %d), "
                   "wifi_call_preference roam:(%d, %d), "
                   "servce_mask_by_network_enabled:(%d, %d)",
                   qcril_qmi_ims_cached_info.wifi_calling_enabled_valid,
                   qcril_qmi_ims_cached_info.wifi_calling_enabled,
                   qcril_qmi_ims_cached_info.call_mode_preference_valid,
                   qcril_qmi_ims_cached_info.call_mode_preference,
                   qcril_qmi_ims_cached_info.call_mode_roam_preference_valid,
                   qcril_qmi_ims_cached_info.call_mode_roam_preference,
                   qcril_qmi_ims_cached_info.service_mask_by_network_enabled_valid,
                   qcril_qmi_ims_cached_info.service_mask_by_network_enabled);

    /* IMSA refactoring */
    qcril_qmi_imss_broadcast_wfc_settings(qcril_qmi_ims_cached_info);

    qcril_qmi_imss_info_unlock();

    if (send_update_to_data)
    {
      qcril_qmi_imss_update_wifi_pref_from_ind_to_qcril_data();
    }
  }
  else
  {
    QCRIL_LOG_ERROR("ind_data_ptr is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
//  qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02
//===========================================================================
qcril_ims_setting_wfc_status_type qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02
(
  uint8_t wfc_status
)
{
  qcril_ims_setting_wfc_status_type res;

  switch(wfc_status)
  {
    case FALSE:
      res = QCRIL_IMS_SETTING_WFC_OFF;
      break;

    case TRUE:
      res = QCRIL_IMS_SETTING_WFC_ON;
      break;

    default:
      res = QCRIL_IMS_SETTING_WFC_NOT_SUPPORTED;
  }
  return res;
}

//===========================================================================
//  qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02
//===========================================================================
qcril_ims_setting_wfc_preference_type qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02
(
  ims_settings_call_mode_option_enum_v01 wfc_preference
)
{
  qcril_ims_setting_wfc_preference_type res;

  switch(wfc_preference)
  {
    case IMS_SETTINGS_CALL_MODE_PREFERRED_WLAN_ONLY_V01:
      res = QCRIL_IMS_SETTING_WFC_WLAN_ONLY;
      break;

    case IMS_SETTINGS_CALL_MODE_PREFERRED_WIFI_V01:
      res = QCRIL_IMS_SETTING_WFC_WLAN_PREFERRED;
      break;

    case IMS_SETTINGS_CALL_MODE_PREFERRED_CELLULAR_V01:
      res = QCRIL_IMS_SETTING_WFC_CELLULAR_PREFERRED;
      break;

    case IMS_SETTINGS_CALL_MODE_PREFERRED_IMS_V01:
      res = QCRIL_IMS_SETTING_WFC_IMS_PREFERRED;
      break;

    default:
      res = QCRIL_IMS_SETTING_WFC_PREF_NONE;
  }
  return res;
}

//===========================================================================
//  qcril_qmi_imss_get_ims_service_enable_config
//===========================================================================
void qcril_qmi_imss_get_ims_service_enable_config()
{
   qmi_client_error_type qmi_client_error = QMI_NO_ERR;
   ims_settings_get_ims_service_enable_config_rsp_msg_v01
     qmi_ims_get_ims_service_enable_config_resp;

   QCRIL_LOG_FUNC_ENTRY();

   memset(&qmi_ims_get_ims_service_enable_config_resp, 0x0,
     sizeof(qmi_ims_get_ims_service_enable_config_resp));

   qmi_client_error = qmi_client_imss_send_sync(
     QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01,
     NULL,
     0,
     &qmi_ims_get_ims_service_enable_config_resp,
     sizeof(qmi_ims_get_ims_service_enable_config_resp) );
   QCRIL_LOG_INFO(".. qmi send sync res %d", (int) qmi_client_error );

   if (qmi_client_error == QMI_NO_ERR)
   {
      if( qmi_ims_get_ims_service_enable_config_resp.resp.result == QMI_RESULT_SUCCESS_V01 )
      {
        qcril_qmi_imss_store_get_ims_service_enable_resp(
          &qmi_ims_get_ims_service_enable_config_resp);
      }
   }

   qcril_qmi_imss_broadcast_wfc_settings(qcril_qmi_ims_cached_info);

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_get_ims_service_enable_config

//===========================================================================
//  qcril_qmi_imss_store_get_ims_service_enable_resp
//===========================================================================
void qcril_qmi_imss_store_get_ims_service_enable_resp
(
 ims_settings_get_ims_service_enable_config_rsp_msg_v01 *qmi_resp
)
{
  bool send_update_to_data = false;

  qcril_qmi_imss_info_lock();
  QCRIL_LOG_INFO(".. IMS has_state: %d, state: %d", qmi_resp->ims_service_enabled_valid,
                 qmi_resp->ims_service_enabled);
  qcril_qmi_ims_cached_info.wifi_calling_enabled_valid = qmi_resp->wifi_calling_enabled_valid;
  qcril_ims_setting_wfc_status_type old_value_status =
      qcril_qmi_ims_cached_info.wifi_calling_enabled;
  qcril_qmi_ims_cached_info.wifi_calling_enabled =
      qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02(qmi_resp->wifi_calling_enabled);
  if (old_value_status != qcril_qmi_ims_cached_info.wifi_calling_enabled)
  {
    send_update_to_data = true;
  }
  if (qmi_resp->call_mode_preference_ext_valid)
  {
    qcril_qmi_ims_cached_info.call_mode_preference_valid = qmi_resp->call_mode_preference_ext_valid;
    qcril_ims_setting_wfc_preference_type old_value_pref = qcril_qmi_ims_cached_info.call_mode_preference;
    qcril_qmi_ims_cached_info.call_mode_preference =
        qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02(qmi_resp->call_mode_preference_ext);
    if (old_value_pref != qcril_qmi_ims_cached_info.call_mode_preference)
    {
      send_update_to_data = true;
    }
  }
  else if (qmi_resp->call_mode_preference_valid)
  {
    qcril_qmi_ims_cached_info.call_mode_preference_valid = qmi_resp->call_mode_preference_valid;
    qcril_ims_setting_wfc_preference_type old_value_pref = qcril_qmi_ims_cached_info.call_mode_preference;
    qcril_qmi_ims_cached_info.call_mode_preference =
        qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02(qmi_resp->call_mode_preference);
    if (old_value_pref != qcril_qmi_ims_cached_info.call_mode_preference)
    {
      send_update_to_data = true;
    }
  }
  qcril_qmi_ims_cached_info.service_mask_by_network_enabled_valid =
      qmi_resp->service_mask_by_network_enabled_valid;
  qcril_qmi_ims_cached_info.service_mask_by_network_enabled =
      qmi_resp->service_mask_by_network_enabled;

  QCRIL_LOG_INFO(".. wifi_call:(%d, %d), wifi_call_preference:(%d, %d), "
                 "servce_mask_by_network_enabled:(%d, %d)",
                 qcril_qmi_ims_cached_info.wifi_calling_enabled_valid,
                 qcril_qmi_ims_cached_info.wifi_calling_enabled,
                 qcril_qmi_ims_cached_info.call_mode_preference_valid,
                 qcril_qmi_ims_cached_info.call_mode_preference,
                 qcril_qmi_ims_cached_info.service_mask_by_network_enabled_valid,
                 qcril_qmi_ims_cached_info.service_mask_by_network_enabled);
  qcril_qmi_imss_info_unlock();

  if (send_update_to_data)
  {
    qcril_qmi_imss_update_wifi_pref_from_ind_to_qcril_data();
  }
}

//===========================================================================
//  qcril_qmi_imss_get_service_mask_by_network_enabled
//===========================================================================
ims_settings_omadm_services_type_mask_v01 qcril_qmi_imss_get_service_mask_by_network_enabled()
{
  ims_settings_omadm_services_type_mask_v01 service_mask =
    IMS_SETTINGS_OMADM_SERVICES_BIT_MASK_NONE_V01;

  qcril_qmi_imss_info_lock();
  if(qcril_qmi_ims_cached_info.service_mask_by_network_enabled_valid)
  {
    service_mask = qcril_qmi_ims_cached_info.service_mask_by_network_enabled;
  }
  qcril_qmi_imss_info_unlock();

  return service_mask;
}

//===========================================================================
// qcril_qmi_imss_request_send_rtt_msg
//===========================================================================
void qcril_qmi_imss_request_send_rtt_msg
(
    std::shared_ptr<QcRilRequestImsSendRttMessage> msg
)
{
  ims_settings_send_rtt_req_msg_v01  qmi_req;
  uint32 user_data;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;
  RIL_Errno res = RIL_E_GENERIC_FAILURE;

  memset(&qmi_req, 0, sizeof(qmi_req));
  qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;
  uint16_t req_id = -1;
  bool doCleanup = false;

  QCRIL_LOG_FUNC_ENTRY();

  if (msg != nullptr) {
    do {
      string rttMessage = msg->getRttMessage();
      if (rttMessage.empty()) {
        QCRIL_LOG_ERROR("Invalid Arguments for RTT Message");
        break;
      }

      if (IMS_SETTINGS_RTT_DATA_LEN_MAX_V01 <= rttMessage.length())
      {
        QCRIL_LOG_ERROR("Invalid rtt string length %d", rttMessage.length());
        break;
      }

      qmi_req.rtt_data_len = rttMessage.length();

      strlcpy((char*)qmi_req.rtt_data,
              (char*)rttMessage.c_str(),
              IMS_SETTINGS_RTT_DATA_LEN_MAX_V01);
      QCRIL_LOG_DEBUG("Rtt Message : %s ",qmi_req.rtt_data);

      auto pendingMsgStatus = getImsModule()->getPendingMessageList().insert(msg);
      if (pendingMsgStatus.second != true)
      {
        break;
      }
      req_id = pendingMsgStatus.first;
      user_data = QCRIL_COMPOSE_USER_DATA(instance_id, QCRIL_DEFAULT_MODEM_ID, req_id);

      qmi_client_error = qmi_client_imss_send_async(QMI_IMS_SETTINGS_SEND_RTT_REQ_V01,
                                                    &qmi_req,
                                                    sizeof(ims_settings_send_rtt_req_msg_v01),
                                                    sizeof(ims_settings_send_rtt_resp_msg_v01),
                                                    qcril_qmi_imss_request_send_rtt_msg_resp_hdlr,
                                                    (void*)(uintptr_t)user_data );
      QCRIL_LOG_INFO(".. qmi send async res %d", (int) qmi_client_error );
      if (E_SUCCESS == qmi_client_error)
      {
        res = RIL_E_SUCCESS;
      } else {
        doCleanup = true;
      }
    } while(FALSE);
    if (RIL_E_SUCCESS != res)
    {
        auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(res, nullptr);
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
        if (doCleanup) {
            getImsModule()->getPendingMessageList().erase(req_id);
        }
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_imss_request_send_rtt_msg

//===========================================================================
// qcril_qmi_imss_request_send_rtt_msg_resp_hdlr
//===========================================================================
void qcril_qmi_imss_request_send_rtt_msg_resp_hdlr
(
   unsigned int                 msg_id,
   std::shared_ptr<void>        resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
)
{
  (void)msg_id;
  (void)resp_c_struct_len;
  ims_settings_send_rtt_resp_msg_v01 *resp = nullptr;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;

  uint32 user_data = (uint32)(uintptr_t) resp_cb_data;
  uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
  auto pendingMsg = getImsModule()->getPendingMessageList().extract(req_id);

  QCRIL_LOG_FUNC_ENTRY();

  do {
    if (pendingMsg == nullptr)
    {
      QCRIL_LOG_ERROR("pendingMsg is null");
      break;
    }
    if (transp_err != QMI_NO_ERR)
    {
      QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI", transp_err);
      break;
    }
    resp = (ims_settings_send_rtt_resp_msg_v01*)(resp_c_struct.get());
    if (resp == nullptr)
    {
      QCRIL_LOG_ERROR("resp is null");
      break;
    }
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR,
                                                                     &(resp->resp));
    QCRIL_LOG_ESSENTIAL("ril_err: %d, qmi res: %d", (int) ril_err, (int)resp->resp.error);
  } while(FALSE);

  if (pendingMsg) {
    auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(ril_err, nullptr);
    auto ril_msg = std::static_pointer_cast<QcRilRequestImsSendRttMessage>(pendingMsg);
    ril_msg->sendResponse(ril_msg, Message::Callback::Status::SUCCESS, respPayload);
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_send_rtt_msg_resp_hdlr

//===========================================================================
// qcril_qmi_imss_rtt_rcvd_data_ind_hdlr
//===========================================================================
void qcril_qmi_imss_rtt_rcvd_data_ind_hdlr(void *ind_data_ptr)
{
  ims_settings_rtt_ind_msg_v01 *rtt_message_incoming = (ims_settings_rtt_ind_msg_v01 *)ind_data_ptr;
  int rtt_msg_len;

  QCRIL_LOG_FUNC_ENTRY();

  if( rtt_message_incoming != NULL)
  {
    rtt_msg_len =  (int)rtt_message_incoming->rtt_data_len + 1;
    char *rtt_message = nullptr;
    rtt_message = (char *) qcril_malloc(sizeof(char) * rtt_msg_len);
    if (rtt_message) {
      strlcpy(rtt_message, (char*)rtt_message_incoming->rtt_data,
                rtt_msg_len);
      auto msg = std::make_shared<QcRilUnsolImsRttMessage>(rtt_message);
      if (msg != nullptr)
      {
        Dispatcher::getInstance().dispatchSync(msg);
      }
    } else {
      QCRIL_LOG_ERROR("memory failure");
    }
    if (rtt_message) {
        qcril_free(rtt_message);
    }
  }
  else
  {
    QCRIL_LOG_ERROR("ind_data_ptr is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_imss_set_ims_media_config_resp_hdlr
//===========================================================================
void qcril_qmi_imss_set_ims_media_config_resp_hdlr
(
 unsigned int                 msg_id,
 std::shared_ptr<void>        resp_c_struct,
 unsigned int                 resp_c_struct_len,
 void                        *resp_cb_data,
 qmi_client_error_type        transp_err
)
{
  (void)msg_id;
  (void)resp_c_struct_len;
  ims_settings_set_screen_resolution_config_rsp_msg_v01 *resp;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  uint32 user_data = (uint32)(uintptr_t) resp_cb_data;
  uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);

  QCRIL_LOG_FUNC_ENTRY();

  getImsModule()->getPendingMessageList().print();
  auto pendingMsg = getImsModule()->getPendingMessageList().extract(req_id);

  do {
    if (pendingMsg == nullptr)
    {
      QCRIL_LOG_ERROR("pendingMsg is null");
      break;
    }
    if (transp_err != QMI_NO_ERR)
    {
      QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI", transp_err);
      ril_err = RIL_E_GENERIC_FAILURE;
      break;
    }
    resp = (ims_settings_set_screen_resolution_config_rsp_msg_v01*)(resp_c_struct.get());
    if (resp == nullptr)
    {
      QCRIL_LOG_ERROR("resp is null");
      ril_err = RIL_E_GENERIC_FAILURE;
      break;
    }
    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &(resp->resp));
    QCRIL_LOG_ESSENTIAL("ril_err: %d, qmi res: %d", (int) ril_err, (int)resp->resp.error);
  } while (FALSE);

  if (pendingMsg != nullptr)
  {
    auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(ril_err, nullptr);
    auto msg(std::static_pointer_cast<QcRilRequestImsSetMediaInfoMessage>(pendingMsg));
    msg->sendResponse(pendingMsg, Message::Callback::Status::SUCCESS, respPayload);
  }
} // qcril_qmi_imss_set_ims_media_config_resp_hdlr

//===========================================================================
// qcril_qmi_imss_request_set_ims_media_config
//===========================================================================
void qcril_qmi_imss_request_set_ims_media_config
(
  std::shared_ptr<QcRilRequestImsSetMediaInfoMessage> msg
)
{
  ims_settings_set_screen_resolution_config_req_msg_v01 req_msg;
  uint32 user_data;
  RIL_Errno errorCode = RIL_E_SUCCESS;
  bool sendResponse = false;
  bool doCleanup = false;
  uint16_t req_id = -1;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    if(feature_disabled_modem_req)
    {
      QCRIL_LOG_INFO("Modem IMS config is disabled. Respond to Telephony with success");
      errorCode = RIL_E_SUCCESS;
      sendResponse = true;
      break;
    }

    memset(&req_msg, 0, sizeof(req_msg));
    if (msg->hasMaxAvcCodecWidth()) {
      req_msg.h264_supported_width_valid = true;
      req_msg.h264_supported_width = msg->getMaxAvcCodecWidth();
    }

    if (msg->hasMaxAvcCodecHeight()) {
      req_msg.h264_supported_height_valid = true;
      req_msg.h264_supported_height = msg->getMaxAvcCodecHeight();
    }

    if (msg->hasMaxHevcCodecWidth()) {
      req_msg.h265_supported_width_valid = true;
      req_msg.h265_supported_width = msg->getMaxHevcCodecWidth();
    }
    if (msg->hasMaxHevcCodecHeight()) {
      req_msg.h265_supported_height_valid = true;
      req_msg.h265_supported_height = msg->getMaxHevcCodecHeight();
    }
    if (msg->hasScreenWidth()) {
      req_msg.screen_width_valid = true;
      req_msg.screen_width = msg->getScreenWidth();
    }
    if (msg->hasScreenHeight()) {
      req_msg.screen_height_valid = true;
      req_msg.screen_height = msg->getScreenHeight();
    }

    auto pendingMsgStatus = getImsModule()->getPendingMessageList().insert(msg);
    if (pendingMsgStatus.second != true)
    {
        sendResponse = true;
        break;
    }
    req_id = pendingMsgStatus.first;
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID,
                                        QCRIL_DEFAULT_MODEM_ID, req_id);
    qmi_client_error_type qmi_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_SET_SCREEN_RESOLUTION_CONFIG_REQ_V01, &req_msg,
        sizeof(ims_settings_set_screen_resolution_config_req_msg_v01),
        sizeof(ims_settings_set_screen_resolution_config_rsp_msg_v01),
        qcril_qmi_imss_set_ims_media_config_resp_hdlr,
        (void *)(uintptr_t)user_data);
    errorCode = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error, NULL);
    QCRIL_LOG_INFO(".. transport error %d, ril_error %d", (int)qmi_error, (int)errorCode);
    if (errorCode != RIL_E_SUCCESS) {
      sendResponse = true;
      doCleanup = true;
    }
  } while(FALSE);

  if (sendResponse)
  {
    auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(errorCode, nullptr);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
    if (doCleanup) {
      getImsModule()->getPendingMessageList().erase(req_id);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_set_ims_media_config

//===========================================================================
// qcril_qmi_imss_map_multi_sim_voice_capability
//===========================================================================
qcril::interfaces::MultiSimVoiceCapability qcril_qmi_imss_map_multi_sim_voice_capability(
    ims_settings_voice_capability_on_dual_sims_enum_v01 in) {
  switch (in) {
    case IMS_SETTINGS_VOICE_CAPABILITY_NONE_MODE_V01:
      return qcril::interfaces::MultiSimVoiceCapability::NONE;
    case IMS_SETTINGS_VOICE_CAPABILITY_DSSS_MODE_V01:
      return qcril::interfaces::MultiSimVoiceCapability::DSSS;
    case IMS_SETTINGS_VOICE_CAPABILITY_DSDS_MODE_V01:
      return qcril::interfaces::MultiSimVoiceCapability::DSDS;
    case IMS_SETTINGS_VOICE_CAPABILITY_DSDA_MODE_V01:
      return qcril::interfaces::MultiSimVoiceCapability::DSDA;
    case IMS_SETTINGS_VOICE_CAPABILITY_PSEUDO_DSDA_MODE_V01:
      return qcril::interfaces::MultiSimVoiceCapability::PSEUDO_DSDA;
    default:
      QCRIL_LOG_INFO("Unknown QMI IMSA multi sim voice capability %d", in);
      return qcril::interfaces::MultiSimVoiceCapability::UNKNOWN;
  }
}  // qcril_qmi_imss_map_multi_sim_voice_capability

//===========================================================================
// qcril_qmi_imss_request_get_dual_sim_voice_capability_resp_hdlr
//===========================================================================
void qcril_qmi_imss_request_get_dual_sim_voice_capability_resp_hdlr(
    unsigned int /*msg_id*/, std::shared_ptr<void> resp_c_struct,
    unsigned int /*resp_c_struct_len*/, void* resp_cb_data, qmi_client_error_type transp_err) {
  ims_settings_get_dual_sim_voice_capability_rsp_msg_v01* resp = nullptr;
  uint32 user_data = (uint32)(uintptr_t)resp_cb_data;
  uint16_t req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA(user_data);
  auto pendingMsg = getImsModule()->getPendingMessageList().extract(req_id);
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  std::shared_ptr<qcril::interfaces::MultiSimVoiceCapabilityRespData> voiceCapability;

  QCRIL_LOG_FUNC_ENTRY();

  do {
    if (pendingMsg == nullptr) {
      QCRIL_LOG_ERROR("pendingMsg is null");
      break;
    }
    if (transp_err != QMI_NO_ERR) {
      QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI", transp_err);
      break;
    }
    resp = (ims_settings_get_dual_sim_voice_capability_rsp_msg_v01*)(resp_c_struct.get());
    if (resp == nullptr) {
      QCRIL_LOG_ERROR("resp is null");
      break;
    }

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &(resp->resp));

    if (ril_err == RIL_E_SUCCESS && !resp->voice_capability_resp_valid) {
      ril_err = RIL_E_GENERIC_FAILURE;
      QCRIL_LOG_ERROR("not a valid voice_capability_resp_valid");
    }

    QCRIL_LOG_ESSENTIAL("ril_err: %d, qmi res: %d", (int)ril_err, (int)resp->resp.error);
    if (ril_err != RIL_E_SUCCESS) {
      break;
    }

    qcril::interfaces::MultiSimVoiceCapability capability =
        qcril_qmi_imss_map_multi_sim_voice_capability(resp->voice_capability_resp);
    if (capability != qcril::interfaces::MultiSimVoiceCapability::UNKNOWN) {
      voiceCapability =
          std::make_shared<qcril::interfaces::MultiSimVoiceCapabilityRespData>(capability);
      if (voiceCapability == nullptr) {
        ril_err = RIL_E_NO_MEMORY;
      }
    }
  } while (FALSE);

  if (pendingMsg) {
    auto respPayload =
        std::make_shared<QcRilRequestMessageCallbackPayload>(ril_err, voiceCapability);
    auto ril_msg = std::static_pointer_cast<QcRilRequestImsQueryMultiSimVoiceCapability>(pendingMsg);
    ril_msg->sendResponse(ril_msg, Message::Callback::Status::SUCCESS, respPayload);
  }

  QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_imss_request_get_dual_sim_voice_capability_resp_hdlr

//===========================================================================
// qcril_qmi_imss_request_get_dual_sim_voice_capability
//===========================================================================
void qcril_qmi_imss_request_get_dual_sim_voice_capability(
    std::shared_ptr<QcRilRequestImsQueryMultiSimVoiceCapability> msg) {
  uint32 user_data;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;
  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  bool doCleanup = false;
  uint16_t req_id = -1;

  QCRIL_LOG_FUNC_ENTRY();

  if (msg != nullptr) {
    do {
      auto pendingMsgStatus = getImsModule()->getPendingMessageList().insert(msg);
      if (!pendingMsgStatus.second) {
        break;
      }
      req_id = pendingMsgStatus.first;
      user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

      qmi_client_error = qmi_client_imss_send_async(
          QMI_IMS_SETTINGS_GET_DUAL_SIM_VOICE_CAPABILITY_REQ_V01, NULL, 0,
          sizeof(ims_settings_get_dual_sim_voice_capability_rsp_msg_v01),
          qcril_qmi_imss_request_get_dual_sim_voice_capability_resp_hdlr,
          (void*)(uintptr_t)user_data);
      if (E_SUCCESS == qmi_client_error) {
        res = RIL_E_SUCCESS;
      } else {
        doCleanup = true;
      }
    } while (FALSE);

    if (RIL_E_SUCCESS != res) {
      auto respPayload = std::make_shared<QcRilRequestMessageCallbackPayload>(res, nullptr);
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, respPayload);
      if (doCleanup) {
        getImsModule()->getPendingMessageList().erase(req_id);
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_imss_request_get_dual_sim_voice_capability

//===========================================================================
// qcril_qmi_imss_request_exit_sms_callback_mode
//===========================================================================
void qcril_qmi_imss_request_exit_sms_callback_mode(
    std::shared_ptr<QcRilRequestImsExitSmsCallbackModeMessage> msg) {

  ims_settings_exit_scbm_mode_rsp_msg_v01 qmi_response;
  RIL_Errno   ril_req_res = RIL_E_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;
  QCRIL_LOG_FUNC_ENTRY();
  memset(&qmi_response, 0, sizeof(qmi_response));

  do {

    qmi_client_error = qmi_client_imss_send_sync(
        QMI_IMS_SETTINGS_EXIT_SCBM_MODE_REQ_V01, NULL, 0,
        (void*) &qmi_response,
        sizeof( qmi_response ));
    ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
        qmi_client_error, &qmi_response.resp );
  } while (FALSE);

  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
        std::make_shared<QcRilRequestMessageCallbackPayload>(ril_req_res, nullptr));
  QCRIL_LOG_INFO("completed with %d", (int) ril_req_res);

  QCRIL_LOG_FUNC_RETURN();

}  // qcril_qmi_imss_request_exit_sms_callback_mode

//===========================================================================
// qcril_qmi_imss_request_get_c_iwlan_config
//===========================================================================
void qcril_qmi_imss_request_get_c_iwlan_config(
    std::shared_ptr<QcRilRequestImsQueryCIWLANConfigSyncMessage> msg) {
  qcril::interfaces::ImsCiWlanConfig config {};
  RIL_Errno   ril_req_res = RIL_E_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;
  ims_settings_get_c_iwlan_status_rsp_msg_v01 qmi_resp {};

  QCRIL_LOG_FUNC_ENTRY();

  memset(&qmi_resp, 0x0, sizeof(qmi_resp));

  qmi_client_error = qmi_client_imss_send_sync(
    QMI_IMS_SETTINGS_GET_C_IWLAN_STATUS_REQ_V01,
    NULL,
    0,
    &qmi_resp,
    sizeof(qmi_resp));
  QCRIL_LOG_INFO(".. qmi send sync res %d", (int) qmi_client_error );

  ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
    qmi_client_error, &qmi_resp.resp );

  std::shared_ptr<qcril::interfaces::ImsCiWlanConfig> ciwlanConfig = nullptr;

  if (ril_req_res == RIL_E_SUCCESS) {
    config.c_iwlan_mode_in_home_valid = qmi_resp.c_iwlan_mode_in_home_valid;
    if (qmi_resp.c_iwlan_mode_in_home == IMS_SETTINGS_C_IWLAN_ONLY_MODE_V01) {
      config.c_iwlan_mode_in_home = qcril::interfaces::ImsCiWlanMode::C_IWLAN_ONLY;
    } else {
      config.c_iwlan_mode_in_home = qcril::interfaces::ImsCiWlanMode::C_IWLAN_PREFERRED;
    }

    config.c_iwlan_mode_in_roam_valid = qmi_resp.c_iwlan_mode_in_roam_valid;
    if (qmi_resp.c_iwlan_mode_in_roam == IMS_SETTINGS_C_IWLAN_ONLY_MODE_V01) {
      config.c_iwlan_mode_in_roam = qcril::interfaces::ImsCiWlanMode::C_IWLAN_ONLY;
    } else {
      config.c_iwlan_mode_in_roam = qcril::interfaces::ImsCiWlanMode::C_IWLAN_PREFERRED;
    }

    ciwlanConfig = std::make_shared<qcril::interfaces::ImsCiWlanConfig>(config);
    if (ciwlanConfig == nullptr) {
      QCRIL_LOG_INFO("C_IWLAN config null due to no memory");
      ril_req_res = RIL_E_NO_MEMORY;
    }
  }

  msg->sendResponse(msg, Message::Callback::Status::SUCCESS, ciwlanConfig);

  QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_imss_request_get_c_iwlan_config

//===========================================================================
// qcril_qmi_imss_request_ims_feature_supported
//===========================================================================
// There will be 3 cases within this function to determine if feature is supported.
//
// 1. RIL needs to query modem to determine if feature is supported by modem.
// 2. RIL needs to do some internal processing to determine if feature is supported by RIL.
// 3. RIL can just return true for feature supported since the supporting code exists in RIL.
//
// When adding support for a new feature, add a new switch case corresponding to the
// correct case above.
bool qcril_qmi_imss_request_ims_feature_supported(
    std::shared_ptr<QcRilRequestImsFeatureSupportedSyncMessage> msg) {

  switch(msg->getFeature()) {
    case qcril::interfaces::FeatureSupported::INTERNAL_AIDL_REORDERING:
      return true;
    /*
     * When RIL needs to query modem for feature supported, we can use
     * qcril_qmi_imss_query_modem_supported_features to query for IMS features.
     * This is a placeholder, and will be used when a future feature needs to
     * query modem.
     *
     * case qcril::interfaces::FeatureSupported::FEATURE_REQUIRING_MODEM_API :
     *   return qcril_qmi_imss_query_modem_supported_features(
     *     QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01, msg->getFeature());
     */
    default :
      return false;
  }
} // qcril_qmi_imss_request_ims_feature_supported
