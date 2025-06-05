/******************************************************************************
#  Copyright (c) 2013 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

  @file    ril_utf_oem_if.h
  @brief   oem API's for test case dev

  DESCRIPTION

  API's for modifying OEM messages used in RIL UTF tests.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef RIL_UTF_OEM_IF_H_
#define RIL_UTF_OEM_IF_H_

#include "ril_utf_if.h"
#include "qcril_qmi_voice.h"
#include "interfaces/uim/UimOemHook.h"
#include "interfaces/nv/nv_items.h"
#include "services/qtuner_v01.h"

typedef struct {
  int index;                                        /* Connection Index for use with, eg, AT+CHLD */
  int toa;                                          /* type of address, eg 145 = intl */
  char als;                                         /* ALS line indicator if available
                                                       (0 = line 1) */
  char isVoice;                                     /* nonzero if this is is a voice call */
  char number[QCRIL_QMI_VOICE_DIAL_NUMBER_MAX_LEN]; /* Remote party number */
  int numberPresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown 3=Payphone */
  char name[QCRIL_QMI_VOICE_DIAL_NUMBER_MAX_LEN]; /* Remote party name */
  int namePresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown 3=Payphone */
} qcril_qmi_voice_setup_call_info;

typedef struct {
  boolean rejection;
} qcril_qmi_voice_setup_answer_data_type;

typedef struct __attribute__((__packed__)) {
  uint32_t nv;
  uint32_t nvItemSize;
  nv_item_type nvItem;
} NvWriteReqType;

typedef struct {
  int rf_state_index;
  uint32_t compatibility_key;
  uint8_t compatibility_key_valid;
} SarRfState;


/* OEM HOOK DTMF forward burst payload length (72 bytes)
 * on_length   : 4 bytes
 * off_length  : 4 bytes
 * dtmf_digits: 64 bytes */
#define QCRIL_QMI_VOICE_DTMF_FWD_BURST_PAYLOAD_LENGTH 72

#define QCRIL_QMI_VOICE_EXT_BRST_INTL_PAYLOAD_LENGTH 5

//RIL interfaces extracted from ril_utf_if.h
typedef class ril_utf_oem_if : public ril_utf_node_base_t {
public:
//==================================================
// OEM Interfaces
//==================================================

////////////////// Presence ////////////////////////
  /*
    QCRIL_EVT_HOOK_UNSOL_WMS_READY
  */
  void update_default_qcril_evt_hook_unsol_wms_ready(void);

  /*
    QCRIL_EVT_HOOK_UNSOL_CSG_ID_CHANGE_IND
  */
  void update_default_qcril_evt_hook_unsol_csg_id();
  void update_qcril_evt_hook_unsol_with_csg_id(uint32_t);

  /*
    QCRIL_EVT_HOOK_UNSOL_MAX_DATA_CHANGE_IND
  */
  void update_default_qcril_evt_hook_unsol_max_data_sub_change();
  void update_qcril_evt_hook_unsol_with_max_data_sub(uint8_t max_data_sub);

  /*
    QCRIL_EVT_HOOK_UNSOL_SS_ERROR_CODE
  */
  void update_default_qcril_evt_hook_unsol_ss_error_code(void);

  /*
    QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01
  */
  Qtuner_set_scenario_req_v01 *update_default_qmi_qtuner_set_rfm_scenario_req(void);

  /*
    QMI_Qtuner_SET_RFM_SCENARIO_RESP_V01
  */
  void update_default_qmi_qtuner_set_rfm_scenario_resp(RIL_Errno err);

  /*
    QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01
  */
  void update_default_qmi_qtuner_get_rfm_scenario_req(void);

  /*
    QMI_Qtuner_GET_RFM_SCENARIO_RESP_V01
  */
  Qtuner_get_rfm_scenarios_resp_v01 *update_default_qmi_qtuner_get_rfm_scenario_resp(RIL_Errno err);

  /*
    QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01
  */
  void update_default_qmi_qtuner_get_provisioned_table_revision_req (void);

  /*
    QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_RESP_V01
  */
  Qtuner_get_provisioned_table_revision_resp_v01 *update_default_qmi_qtuner_get_provisioned_table_revision_resp(RIL_Errno err);

#ifndef UTF_TARGET_BUILD
  /*
    QCRIL_EVT_HOOK_GET_CURRENT_SETUP_CALLS
  */
  void update_default_qcril_evt_hook_get_current_setup_calls();
  /*
    QCRIL_EVT_HOOK_GET_CURRENT_SETUP_CALLS (resp)
  */
  qcril_qmi_voice_setup_call_info *update_default_qcril_evt_hook_get_current_setup_calls_resp (boolean call_present);
  /*
    QCRIL_EVT_HOOK_REQUEST_SETUP_ANSWER
  */
  void update_default_qcril_evt_hook_request_setup_answer(boolean rejection);
  /*
   QCRIL_EVT_HOOK_REQUEST_SETUP_ANSWER (resp)
  */
  void update_default_qcril_evt_hook_request_setup_answer_resp();
  /*
   QCRIL_REQ_HOOK_REJECT_INCOMING_CALL_CAUSE_21
  */
  void update_default_qcril_evt_hook_reject_incoming_call_cause_21();
  /*
   QCRIL_REQ_HOOK_REJECT_INCOMING_CALL_CAUSE_21 (resp)
  */
  void update_default_qcril_evt_hook_reject_incoming_call_cause_21_resp();
  /*
   QCRIL_REQ_HOOK_GET_CSG_ID
  */
  void update_default_qcril_req_hook_get_csg_id_req();
  /*
   QCRIL_REQ_HOOK_GET_CSG_ID (resp)
  */
  void update_default_qcril_req_hook_get_csg_id_resp(uint32_t csgId);
  /*
   QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN
  */
  void update_default_qcril_req_hook_csg_perform_nw_scan_req(const uint8_t *data, uint32_t dataLen);
  /*
   QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN (resp)
  */
  void update_default_qcril_req_hook_csg_perform_nw_scan_resp(const uint8_t *data, uint32_t dataLen);
  /*
   QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF
  */
  void update_default_qcril_req_hook_csg_set_sys_sel_pref_req(const uint8_t *data, uint32_t dataLen);
  /*
   QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF (resp)
  */
  void update_default_qcril_req_hook_csg_set_sys_sel_pref_resp();
  /*
   QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ
  */
  void update_default_qcril_req_hook_get_max_data_allowed_req();
  /*
   QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ (resp)
  */
  void update_default_qcril_req_hook_get_max_data_allowed_resp(uint8_t maxAllowed);
  /*
   QCRIL_REQ_HOOK_GET_ADN_RECORD
  */
  void update_default_qcril_req_hook_get_adn_record_req();
  /*
   QCRIL_REQ_HOOK_GET_ADN_RECORD (resp)
  */
  void update_default_qcril_req_hook_get_adn_record_resp(const uint8_t * data, uint32_t dataLen);
  /*
   QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF
  */
  void update_default_qcril_req_hook_get_preferred_network_band_pref_req(uint32_t ratBandType);
  /*
   QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF (resp)
  */
  void update_default_qcril_req_hook_get_preferred_network_band_pref_resp(uint32_t ratBandMap);
  /*
   QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF
  */
  void update_default_qcril_req_hook_set_preferred_network_band_pref_req(uint32_t bandPrefMap);
  /*
   QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF (resp)
  */
  void update_default_qcril_req_hook_set_preferred_network_band_pref_resp();
  /*
   QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS
  */
  void update_default_qcril_req_hook_set_atel_ui_status_req(uint8_t isUiReady);
  /*
   QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS (resp)
  */
  void update_default_qcril_req_hook_set_atel_ui_status_resp();
  /*
   QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ
  */
  void update_default_qcril_req_hook_get_l_plus_l_feature_support_status_req();
  /*
   QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ (resp)
  */
  void update_default_qcril_req_hook_get_l_plus_l_feature_support_status_resp(uint8_t lPlusLStatus);
  /*
   QCRIL_REQ_HOOK_NV_READ
  */
  void update_default_qcril_req_hook_nv_read_req(uint32_t nv);
  /*
   QCRIL_REQ_HOOK_NV_READ (resp)
  */
  nv_item_type *update_default_qcril_req_hook_nv_read_resp();
  /*
   QCRIL_REQ_HOOK_NV_WRITE
  */
  NvWriteReqType *update_default_qcril_req_hook_nv_write_req(uint32_t nv, uint32_t nvItemSize);
  NvWriteReqType *update_default_qcril_req_hook_nv_write_req();
  /*
   QCRIL_REQ_HOOK_NV_WRITE (resp)
  */
  void update_default_qcril_req_hook_nv_write_resp();
  /*
   QCRIL_REQ_HOOK_SET_TRANSMIT_POWER
  */
  SarRfState *update_default_qcril_req_hook_set_transmit_power_req(bool includeKeyValFlag = true);
  /*
   QCRIL_REQ_HOOK_SET_TRANSMIT_POWER (resp)
  */
  void update_default_qcril_req_hook_set_transmit_power_resp();
  /*
   QCRIL_REQ_HOOK_GET_SAR_REV_KEY
  */
  void update_default_qcril_req_hook_get_sar_rev_key_req();
  /*
   QCRIL_REQ_HOOK_GET_SAR_REV_KEY (resp)
  */
  void update_default_qcril_req_hook_get_sar_rev_key_resp(uint32_t key);

  /*========================================================
    QCRIL_REQ_HOOK_UNSOL_CDMA_BURST_DTMF
    ========================================================*/
  void update_default_qcril_evt_hook_cdma_burst_dtmf(char *digits, char *on_length, char *off_length);

  /*========================================================
    QCRIL_REQ_HOOK_UNSOL_CDMA_CONT_DTMF_START
    ========================================================*/
  void update_default_qcril_evt_hook_cdma_cont_dtmf_start(char digits);

  /*========================================================
    QCRIL_REQ_HOOK_UNSOL_CDMA_CONT_DTMF_STOP
    ========================================================*/
  void update_default_qcril_evt_hook_cdma_cont_dtmf_stop();

  /*========================================================
    QCRIL_REQ_HOOK_UNSOL_SPEECH_CODEC_INFO
    ========================================================*/
  void update_default_qcril_evt_hook_unsol_speech_codec_info(int call_id, int codec, int mode);

  /*========================================================
    QCRIL_REQ_HOOK_UNSOL_NSS_RELEASE
    ========================================================*/
  void update_default_qcril_evt_hook_unsol_nss_release(uint8_t callId, voice_nss_release_enum_v02 nssRelease);

  /*========================================================
    QCRIL_REQ_HOOK_UNSOL_EXTENDED_DBM_INTL
    ========================================================*/
  void update_default_qcril_evt_hook_unsol_extended_dbm_intl(uint16_t mcc, uint8_t db_subtype, uint8_t chg_ind, uint8_t sub_unit, uint8_t unit);
  /*
TODO:
   QCRIL_REQ_HOOK_UNSOL_SS_ERROR_CODE
  */


 /*========================================================
  QCRIL_EVT_HOOK_SET_CDMA_SUB_SRC_WITH_SPC
 ========================================================*/
  void update_default_oem_request_cdma_set_subscription_source(oem_hook_cdma_subscription_source_e_type sub_source, char *spc, int spc_len);
  void update_default_oem_request_cdma_set_subscription_source_resp(boolean result);

  /*========================================================
  QCRIL_EVT_HOOK_SET_LTE_TUNE_AWAY
 ========================================================*/
  void update_default_oem_request_set_lte_tune_away_req(boolean lte_tune_away);
  void update_default_oem_request_set_lte_tune_away_resp();

#endif

  /*
   QCRIL_EVT_HOOK_UNSOL_WWAN_AVAILABLE
  */
  void update_default_qcril_evt_hook_unsol_wlan_available(uint8_t is_wlan_status);

/*========================================================
  QCRIL_EVT_HOOK_SET_DATA_SUBSCRIPTION
========================================================*/
#ifdef RIL_UTF_L_MASTER
  void update_default_oem_request_set_data_subscription_req(int data_subsciption);
  void update_default_oem_request_set_data_subscription_resp();
#endif
/*
  QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY
*/
void update_default_qcril_evt_hook_unsol_modem_capability(uint8_t *data, size_t len);


void update_default_qcril_evt_hook_unsol_card_state_change_ind(void);

/*=======================================================
  SIM REFRESH OEM HOOK INDICATION
=======================================================*/
RIL_Hook_SimRefreshResponse* update_default_sim_refresh_unsol_event(void);
} OEMTestnode;

#endif /* RIL_UTF_OEM_IF_H_*/
