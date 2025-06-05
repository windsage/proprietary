/******************************************************************************
#  Copyright (c) 2013 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
  @file    ril_utf_oem_if.cpp
  @brief   ims API's for test case dev

  DESCRIPTION

  API's for modifying ims messages used in RIL UTF tests.
  ---------------------------------------------------------------------------
******************************************************************************/
#include "ril_utf_oem_if.h"
#ifdef QMI_RIL_HAL_UTF
#include "qcril_qmi_oem_reqlist.h"
#endif

//---------------------------------------------------------------------------------------
//==================================================
// OEM Interfaces
//==================================================

/*========================================================
  QCRIL_EVT_HOOK_UNSOL_WMS_READY
========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_unsol_wms_ready(void)
{
  payload = NULL;
  payload_len = 0;
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_WMS_READY;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_WMS_READY;
#endif
}

/*========================================================
  QCRIL_EVT_HOOK_UNSOL_CSG_ID_CHANGE_IND
========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_unsol_csg_id()
{
  payload = malloc(sizeof(uint32_t));
  payload_len = sizeof(uint32_t);
  memset(payload,0,payload_len);
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_CSG_ID_CHANGE_IND;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_CSG_ID_CHANGE_IND;
#endif
}

void ril_utf_oem_if::update_qcril_evt_hook_unsol_with_csg_id(uint32_t csg_id)
{
	uint32_t *ptr = (uint32_t*) payload;
	*ptr = csg_id;
}

/*========================================================
  QCRIL_EVT_HOOK_UNSOL_MAX_DATA_CHANGE_IND
========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_unsol_max_data_sub_change()
{
  payload = malloc(sizeof(uint8_t));
  payload_len = sizeof(uint8_t);
  memset(payload,0,payload_len);
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_MAX_DATA_CHANGE_IND;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_MAX_DATA_CHANGE_IND;
#endif
}

void ril_utf_oem_if::update_qcril_evt_hook_unsol_with_max_data_sub(uint8_t max_data_sub)
{
	uint8_t *ptr = (uint8_t*) payload;
	*ptr = max_data_sub;
}

/*========================================================
  QCRIL_EVT_HOOK_UNSOL_SS_ERROR_CODE
========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_unsol_ss_error_code(void)
{
  payload = NULL;
  payload_len = 0;
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_SS_ERROR_CODE;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_SS_ERROR_CODE;
#endif
}

/*
  QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01
*/
Qtuner_set_scenario_req_v01 *ril_utf_oem_if::update_default_qmi_qtuner_set_rfm_scenario_req(void)
{
  payload_len = sizeof(Qtuner_set_scenario_req_v01);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
  }
  payload_type = ril_utf_oem_request;
  service_id = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER;
  msg_id = QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01;
  return (Qtuner_set_scenario_req_v01 *)payload;
}

/*
  QMI_Qtuner_SET_RFM_SCENARIO_RESP_V01
*/
void ril_utf_oem_if::update_default_qmi_qtuner_set_rfm_scenario_resp(RIL_Errno err)
{
  payload_len = 0;
  payload = nullptr;
  payload_type = ril_utf_oem_response;
  service_id = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER;
  msg_id = QMI_Qtuner_SET_RFM_SCENARIO_RESP_V01;
  error = err;
}

/*
  QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01
*/
void ril_utf_oem_if::update_default_qmi_qtuner_get_rfm_scenario_req(void)
{
  payload_len = 0;
  payload = nullptr;
  payload_type = ril_utf_oem_request;
  service_id = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER;
  msg_id = QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01;
}

/*
  QMI_Qtuner_GET_RFM_SCENARIO_RESP_V01
*/
Qtuner_get_rfm_scenarios_resp_v01 *ril_utf_oem_if::update_default_qmi_qtuner_get_rfm_scenario_resp(RIL_Errno err)
{
  payload_len = sizeof(Qtuner_get_rfm_scenarios_resp_v01);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
  }
  payload_type = ril_utf_oem_response;
  service_id = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER;
  msg_id = QMI_Qtuner_GET_RFM_SCENARIO_RESP_V01;
  error = err;
  return (Qtuner_get_rfm_scenarios_resp_v01 *)payload;
}

/*
  QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01
*/
void ril_utf_oem_if::update_default_qmi_qtuner_get_provisioned_table_revision_req (void)
{
  payload_len = 0;
  payload = nullptr;
  payload_type = ril_utf_oem_request;
  service_id = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER;
  msg_id = QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01;
}

/*
  QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_RESP_V01
*/
Qtuner_get_provisioned_table_revision_resp_v01 *ril_utf_oem_if::update_default_qmi_qtuner_get_provisioned_table_revision_resp(RIL_Errno err)
{
  payload_len = sizeof(Qtuner_get_provisioned_table_revision_resp_v01);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
  }
  payload_type = ril_utf_oem_response;
  service_id = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER;
  msg_id = QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_RESP_V01;
  error = err;
  return (Qtuner_get_provisioned_table_revision_resp_v01 *)payload;
}

#ifndef UTF_TARGET_BUILD
/*========================================================
  QCRIL_EVT_HOOK_GET_CURRENT_SETUP_CALLS
=========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_get_current_setup_calls()
{
  payload = NULL;
  payload_len = 0;
  payload_type = ril_utf_oem_request;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_GET_CURRENT_SETUP_CALLS;
#else
  oem_command_id = QCRIL_EVT_HOOK_GET_CURRENT_SETUP_CALLS;
#endif
}

/*========================================================
  QCRIL_EVT_HOOK_GET_CURRENT_SETUP_CALLS (resp)
=========================================================*/
qcril_qmi_voice_setup_call_info *ril_utf_oem_if::update_default_qcril_evt_hook_get_current_setup_calls_resp(boolean call_present)
{
  if (FALSE == call_present)
  {
    payload = NULL;
    payload_len = 0;
  }
  else
  {
    payload = malloc(sizeof(qcril_qmi_voice_setup_call_info));
    payload_len = sizeof(qcril_qmi_voice_setup_call_info);
    memset(payload, 0, payload_len);
  }
  payload_type = ril_utf_oem_response;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_GET_CURRENT_SETUP_CALLS;
#else
  oem_command_id = QCRIL_EVT_HOOK_GET_CURRENT_SETUP_CALLS;
#endif
  qcril_qmi_voice_setup_call_info *dptr = (qcril_qmi_voice_setup_call_info *) payload;
  return dptr;
}
/*========================================================
  QCRIL_EVT_HOOK_REQUEST_SETUP_ANSWER
=========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_request_setup_answer(boolean rejection)
{
  payload = malloc(sizeof(qcril_qmi_voice_setup_answer_data_type));
  payload_len = sizeof(qcril_qmi_voice_setup_answer_data_type);
  memset(payload, 0, payload_len);
  payload_type = ril_utf_oem_request;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_REQUEST_SETUP_ANSWER;
#else
  oem_command_id = QCRIL_EVT_HOOK_REQUEST_SETUP_ANSWER;
#endif
  qcril_qmi_voice_setup_answer_data_type *ptr = (qcril_qmi_voice_setup_answer_data_type *) payload;
  ptr->rejection = rejection;
}
/*========================================================
  QCRIL_EVT_HOOK_REQUEST_SETUP_ANSWER (resp)
=========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_request_setup_answer_resp()
{
  payload = NULL;
  payload_len = 0;
  payload_type = ril_utf_oem_response;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_REQUEST_SETUP_ANSWER;
#else
  oem_command_id = QCRIL_EVT_HOOK_REQUEST_SETUP_ANSWER;
#endif
}

/*========================================================
  QCRIL_REQ_HOOK_REJECT_INCOMING_CALL_CAUSE_21
=========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_reject_incoming_call_cause_21()
{
  payload = malloc(sizeof(qcril_qmi_voice_setup_answer_data_type));
  payload_len = 0;
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_REJECT_INCOMING_CALL_CAUSE_21;
}
/*========================================================
  QCRIL_REQ_HOOK_REJECT_INCOMING_CALL_CAUSE_21 (resp)
=========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_reject_incoming_call_cause_21_resp()
{
  payload = NULL;
  payload_len = 0;
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_REJECT_INCOMING_CALL_CAUSE_21;
}

/*
 QCRIL_REQ_HOOK_SET_TRANSMIT_POWER
*/
SarRfState *ril_utf_oem_if::update_default_qcril_req_hook_set_transmit_power_req(bool includeKeyValFlag)
{
  payload_len = sizeof(SarRfState);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
  }
//  if (includeKeyValFlag) {
//    payload_len = sizeof(int) + sizeof(uint32_t);
//  }
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_SET_TRANSMIT_POWER;

  return (SarRfState *)payload;
}
/*
 QCRIL_REQ_HOOK_SET_TRANSMIT_POWER (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_set_transmit_power_resp()
{
  payload_len = 0;
  payload = NULL;
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_SET_TRANSMIT_POWER;
}

/*
 QCRIL_REQ_HOOK_GET_SAR_REV_KEY
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_sar_rev_key_req()
{
  payload_len = 0;
  payload = NULL;
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_GET_SAR_REV_KEY;
}
/*
 QCRIL_REQ_HOOK_GET_SAR_REV_KEY (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_sar_rev_key_resp(uint32_t key)
{
  payload_len = sizeof(uint32_t);
  payload = malloc(payload_len);
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_GET_SAR_REV_KEY;
  uint32_t *ptr = (uint32_t *)payload;
  *ptr = key;
}

/*
   QCRIL_REQ_HOOK_GET_CSG_ID
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_csg_id_req()
{
  payload = nullptr;
  payload_len = 0;
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_GET_CSG_ID;
}
/*
  QCRIL_REQ_HOOK_GET_CSG_ID (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_csg_id_resp(uint32_t csgId)
{
  payload_len = sizeof(uint32_t);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
    uint32_t *ptr = (uint32_t *)payload;
    *ptr = csgId;
  }
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_GET_CSG_ID;
}

/*
   QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN
*/
void ril_utf_oem_if::update_default_qcril_req_hook_csg_perform_nw_scan_req(const uint8_t *data, uint32_t dataLen)
{
  payload = nullptr;
  payload_len = 0;
  if (data && dataLen) {
    payload_len = dataLen;
    payload = malloc(payload_len);
    if (payload) {
      memcpy(payload, data, dataLen);
    }
  }
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN;
}
/*
   QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_csg_perform_nw_scan_resp(const uint8_t *data, uint32_t dataLen)
{
  payload = nullptr;
  payload_len = 0;
  if (data && dataLen) {
    payload_len = dataLen;
    payload = malloc(payload_len);
    if (payload) {
      memcpy(payload, data, dataLen);
    }
  }
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN;
}

/*
   QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF
*/
void ril_utf_oem_if::update_default_qcril_req_hook_csg_set_sys_sel_pref_req(const uint8_t *data, uint32_t dataLen)
{
  payload = nullptr;
  payload_len = 0;
  if (data && dataLen) {
    payload_len = dataLen;
    payload = malloc(payload_len);
    if (payload) {
      memcpy(payload, data, dataLen);
    }
  }
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF;
}
/*
   QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_csg_set_sys_sel_pref_resp()
{
  payload = nullptr;
  payload_len = 0;
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF;
}

/*
 QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_max_data_allowed_req()
{
  payload = nullptr;
  payload_len = 0;
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ;
}
/*
 QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_max_data_allowed_resp(uint8_t maxAllowed)
{
  payload_len = sizeof(uint8_t);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
    uint8_t *ptr = (uint8_t *)payload;
    *ptr = maxAllowed;
  }
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ;
}
/*
 QCRIL_REQ_HOOK_GET_ADN_RECORD
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_adn_record_req()
{
  payload = nullptr;
  payload_len = 0;
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_GET_ADN_RECORD;
}

void ril_utf_oem_if::update_default_qcril_req_hook_get_adn_record_resp(const uint8_t * data, uint32_t dataLen)
{
  payload = nullptr;
  payload_len = 0;
  if (data && dataLen) {
    payload_len = dataLen;
    payload = malloc(payload_len);
    assert(payload != nullptr);
    memset(payload, 0, payload_len);
    memcpy(payload, data, dataLen);
  }
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_GET_ADN_RECORD;
}
/*
 QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_preferred_network_band_pref_req(uint32_t ratBandType)
{
  payload_len = sizeof(uint32_t);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
    uint32_t *ptr = (uint32_t *)payload;
    *ptr = ratBandType;
  }
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF;
}
/*
 QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_preferred_network_band_pref_resp(uint32_t ratBandMap)
{
  payload_len = sizeof(uint32_t);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
    uint32_t *ptr = (uint32_t *)payload;
    *ptr = ratBandMap;
  }
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF;
}
/*
 QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF
*/
void ril_utf_oem_if::update_default_qcril_req_hook_set_preferred_network_band_pref_req(uint32_t bandPrefMap)
{
  payload_len = sizeof(uint32_t);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
    uint32_t *ptr = (uint32_t *)payload;
    *ptr = bandPrefMap;
  }
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF;
}
/*
 QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_set_preferred_network_band_pref_resp()
{
  payload_len = 0;
  payload = nullptr;
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF;
}
/*
 QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS
*/
void ril_utf_oem_if::update_default_qcril_req_hook_set_atel_ui_status_req(uint8_t isUiReady)
{
  payload_len = sizeof(uint8_t);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
    uint8_t *ptr = (uint8_t *)payload;
    *ptr = isUiReady;
  }
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS;
}
/*
 QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_set_atel_ui_status_resp()
{
  payload_len = 0;
  payload = nullptr;
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS;
}

/*
 QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_l_plus_l_feature_support_status_req()
{
  payload = nullptr;
  payload_len = 0;
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ;
}
/*
 QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_get_l_plus_l_feature_support_status_resp(uint8_t lPlusLStatus)
{
  payload_len = sizeof(uint8_t);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
    uint8_t *ptr = (uint8_t *)payload;
    *ptr = lPlusLStatus;
  }
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ;
}

/*
 QCRIL_REQ_HOOK_NV_READ
*/
void ril_utf_oem_if::update_default_qcril_req_hook_nv_read_req(uint32_t nv)
{
  payload_len = sizeof(uint32_t);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
    uint32_t *ptr = (uint32_t *)payload;
    *ptr = nv;
  }
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_NV_READ;
}

/*
 QCRIL_REQ_HOOK_NV_READ (resp)
*/
nv_item_type *ril_utf_oem_if::update_default_qcril_req_hook_nv_read_resp()
{
  payload_len = sizeof(nv_item_type);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
  }
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_NV_READ;

  nv_item_type *ptr = (nv_item_type *)payload;
  return ptr;
}

/*
 QCRIL_REQ_HOOK_NV_WRITE
*/
NvWriteReqType *ril_utf_oem_if::update_default_qcril_req_hook_nv_write_req(uint32_t nv, uint32_t nvItemSize)
{
  payload_len = sizeof(NvWriteReqType);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
  }
  payload_len = nvItemSize;
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_NV_WRITE;
  NvWriteReqType *ptr = (NvWriteReqType *)payload;
  return ptr;
}
NvWriteReqType *ril_utf_oem_if::update_default_qcril_req_hook_nv_write_req()
{
  payload_len = sizeof(NvWriteReqType);
  payload = malloc(payload_len);
  if (payload) {
    memset(payload, 0, payload_len);
  }
  payload_type = ril_utf_oem_request;
  oem_command_id = QCRIL_REQ_HOOK_NV_WRITE;
  NvWriteReqType *ptr = (NvWriteReqType *)payload;
  return ptr;
}
/*
 QCRIL_REQ_HOOK_NV_WRITE (resp)
*/
void ril_utf_oem_if::update_default_qcril_req_hook_nv_write_resp()
{
  payload_len = 0;
  payload = nullptr;
  payload_type = ril_utf_oem_response;
  oem_command_id = QCRIL_REQ_HOOK_NV_WRITE;
}

/*========================================================
  QCRIL_EVT_HOOK_SET_CDMA_SUB_SRC_WITH_SPC
========================================================*/
void ril_utf_oem_if::update_default_oem_request_cdma_set_subscription_source(oem_hook_cdma_subscription_source_e_type sub_source, char *spc, int spc_len)
{
  payload = (char *)malloc((spc_len+1)*sizeof(char));
  payload_len = (spc_len+1);
  memset(payload, 0, payload_len);
  payload_type = ril_utf_oem_request;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_SET_CDMA_SUB_SRC_WITH_SPC;
#else
  oem_command_id = QCRIL_EVT_HOOK_SET_CDMA_SUB_SRC_WITH_SPC;
#endif
  char *ptr = (char *)payload;
  ptr[0] = (oem_hook_cdma_subscription_source_e_type)sub_source;
  strncpy(&ptr[1],spc, spc_len);
}

/*========================================================
  QCRIL_EVT_HOOK_SET_CDMA_SUB_SRC_WITH_SPC_RESP
========================================================*/
void ril_utf_oem_if::update_default_oem_request_cdma_set_subscription_source_resp(boolean result)
{
  payload = NULL;
  payload_len = 0;

  payload = (boolean *)malloc(1 *sizeof(boolean));
  memset(payload, 0, sizeof(boolean));
  payload_len = sizeof(boolean);
  boolean *ptr = (boolean *)payload;
  *ptr = result;

  payload_type = ril_utf_oem_response;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_SET_CDMA_SUB_SRC_WITH_SPC;
#else
  oem_command_id = QCRIL_EVT_HOOK_SET_CDMA_SUB_SRC_WITH_SPC;
#endif
}

/*========================================================
  QCRIL_EVT_HOOK_SET_LTE_TUNE_AWAY
========================================================*/
void ril_utf_oem_if::update_default_oem_request_set_lte_tune_away_req(boolean lte_tune_away)
{
  payload = NULL;
  payload_len = 0;
  payload = (boolean *)malloc(1 *sizeof(boolean));
  memset(payload, 0, sizeof(boolean));
  payload_len = sizeof(boolean);
  payload_type = ril_utf_oem_request;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_SET_LTE_TUNE_AWAY;
#else
  oem_command_id = QCRIL_EVT_HOOK_SET_LTE_TUNE_AWAY;
#endif
  boolean *ptr = (boolean *)payload;
  *ptr = lte_tune_away;
}

/*========================================================
  QCRIL_EVT_HOOK_SET_LTE_TUNE_AWAY_RESP
========================================================*/
void ril_utf_oem_if::update_default_oem_request_set_lte_tune_away_resp()
{
  payload = NULL;
  payload_len = 0;

  payload_type = ril_utf_oem_response;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_SET_LTE_TUNE_AWAY;
#else
  oem_command_id = QCRIL_EVT_HOOK_SET_LTE_TUNE_AWAY;
#endif
}

/*========================================================
  QCRIL_EVT_HOOK_UNSOL_WWAN_AVAILABLE
========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_unsol_wlan_available(uint8_t is_wlan_status)
{
  payload = NULL;
  payload_len = 0;

  payload = (uint8_t *)malloc(1 *sizeof(uint8_t));
  memset(payload, 0, sizeof(uint8_t));
  payload_len = sizeof(uint8_t);
  uint8_t *ptr = (uint8_t *)payload;
  *ptr = is_wlan_status;

  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_WWAN_AVAILABLE;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_WWAN_AVAILABLE;
#endif
}

#ifdef RIL_UTF_L_MASTER
/*========================================================
  QCRIL_EVT_HOOK_SET_DATA_SUBSCRIPTION_REQ
========================================================*/
void ril_utf_oem_if::update_default_oem_request_set_data_subscription_req(int data_subsciption)
{
  payload = NULL;
  payload_len = 0;
  payload = (boolean *)malloc(1 *sizeof(int));
  memset(payload, 0, sizeof(int));
  payload_len = sizeof(int);
  payload_type = ril_utf_oem_request;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_SET_DATA_SUBSCRIPTION;
#else
  oem_command_id = QCRIL_EVT_HOOK_SET_DATA_SUBSCRIPTION;
#endif
  int *ptr = (int *)payload;
  *ptr = data_subsciption;
}

/*========================================================
  QCRIL_EVT_HOOK_SET_DATA_SUBSCRIPTION_RESP
========================================================*/
void ril_utf_oem_if::update_default_oem_request_set_data_subscription_resp()
{
  payload = NULL;
  payload_len = 0;

  payload_type = ril_utf_oem_response;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_SET_DATA_SUBSCRIPTION;
#else
  oem_command_id = QCRIL_EVT_HOOK_SET_DATA_SUBSCRIPTION;
#endif
}
#endif
/*========================================================
  QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY
========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_unsol_modem_capability(uint8_t *data, size_t len)
{
  payload = malloc (len);
  payload_len = len;
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_MODEM_CAPABILITY;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY;
#endif

  memcpy (payload, data, len);
}

/*========================================================
    QCRIL_REQ_HOOK_UNSOL_CDMA_BURST_DTMF
========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_cdma_burst_dtmf(char *digits, char *on_length, char *off_length)
{
  char data[QCRIL_QMI_VOICE_DTMF_FWD_BURST_PAYLOAD_LENGTH] = {0};
  if (on_length) {
    strlcpy(data, on_length, 4);
  }
  if (off_length) {
    strlcpy(data+4, off_length, 4);
  }
  if (digits) {
    strlcpy(data+8, digits, strlen(digits)+1);
  }
  int len = sizeof(data);
  payload = malloc (len);
  memcpy(payload, data, len);
  payload_len = len;
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_CDMA_BURST_DTMF;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_CDMA_BURST_DTMF;
#endif
}
/*========================================================
  QCRIL_REQ_HOOK_UNSOL_CDMA_CONT_DTMF_START
 ========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_cdma_cont_dtmf_start(char digit)
{
  int len = sizeof(digit);
  payload = malloc (len);
  memcpy(payload, &digit, len);
  payload_len = len;
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_CDMA_CONT_DTMF_START;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_CDMA_CONT_DTMF_START;
#endif
}

/*========================================================
    QCRIL_REQ_HOOK_UNSOL_CDMA_CONT_DTMF_STOP
========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_cdma_cont_dtmf_stop()
{
  payload = NULL;
  payload_len = 0;
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_CDMA_CONT_DTMF_STOP;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_CDMA_CONT_DTMF_STOP;
#endif
}
/*========================================================
  QCRIL_EVT_HOOK_UNSOL_SPEECH_CODEC_INFO
========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_unsol_speech_codec_info(int call_id, int codec, int mode)
{
  int data[3] = {0};
  int len = sizeof(data);
  payload = malloc (len);
  payload_len = len;
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_SPEECH_CODEC_INFO;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_SPEECH_CODEC_INFO;
#endif

  data[0] = call_id;
  data[1] = codec;
  data[2] = mode;
  memcpy(payload, data, len);
}
/*========================================================
  QCRIL_REQ_HOOK_UNSOL_NSS_RELEASE
  ========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_unsol_nss_release(uint8_t callId, voice_nss_release_enum_v02 nssRelease)
{
  char data[50] = {0};
  memcpy(data, &nssRelease, sizeof(nssRelease));
  memcpy((data + sizeof(nssRelease)), &callId, sizeof(callId));
  size_t len = sizeof(callId) + sizeof(nssRelease);
  payload = malloc (len);
  memcpy(payload, data, len);
  payload_len = len;
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_NSS_RELEASE;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_NSS_RELEASE;
#endif
}

/*========================================================
  QCRIL_REQ_HOOK_UNSOL_EXTENDED_DBM_INTL
  ========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_unsol_extended_dbm_intl(uint16_t mcc, uint8_t db_subtype, uint8_t chg_ind, uint8_t sub_unit, uint8_t unit)
{
  int data[QCRIL_QMI_VOICE_EXT_BRST_INTL_PAYLOAD_LENGTH];
  data[0] = mcc;
  data[1] = db_subtype;
  data[2] = chg_ind;
  data[3] = sub_unit;
  data[4] = unit;

  size_t len = sizeof(data);
  payload = malloc (sizeof(data));
  memcpy(payload, data, len);
  payload_len = len;
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_EXTENDED_DBM_INTL;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_EXTENDED_DBM_INTL;
#endif
}

/*========================================================
  QCRIL_REQ_HOOK_UNSOL_CARD_STATE_CHANGE_IND
========================================================*/
void ril_utf_oem_if::update_default_qcril_evt_hook_unsol_card_state_change_ind(void)
{
  payload = NULL;
  payload_len = 0;
  payload_type = ril_utf_oem_indication;
#ifdef QMI_RIL_HAL_UTF
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_CARD_STATE_CHANGE_IND;
#else
  oem_command_id = QCRIL_EVT_HOOK_UNSOL_CARD_STATE_CHANGE_IND;
#endif
}

/*========================================================
  QCRIL_REQ_HOOK_UNSOL_SIM_REFRESH
========================================================*/
RIL_Hook_SimRefreshResponse* ril_utf_oem_if::update_default_sim_refresh_unsol_event(void)
{
  payload = malloc(sizeof(RIL_Hook_SimRefreshResponse));
  payload_len = sizeof(RIL_Hook_SimRefreshResponse) ;
  memset(payload, 0x00, payload_len);
  payload_type = ril_utf_oem_indication;
  oem_command_id = QCRIL_REQ_HOOK_UNSOL_SIM_REFRESH;
  service_id = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_NONE;

  return (RIL_Hook_SimRefreshResponse*)payload;
}

#endif
