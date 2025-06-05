/******************************************************************************
#  Copyright (c) 2009-2013, 2017, 2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "qcril_nv.h"

#include "interfaces/nv/nv_items.h"
#include "interfaces/nv/RilRequestNvReadMessage.h"
#include "interfaces/nv/RilRequestNvWriteMessage.h"

#include "network_access_service_v01.h"
#include "voice_service_v02.h"
#include "qmi_client.h"

#include "qcril_memory_management.h"

#include "modules/nas/qcril_nas_legacy.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/VoiceModemEndPoint.h"
#include "modules/voice/VoiceGetConfigMessage.h"
#include "modules/voice/VoiceSetConfigMessage.h"
#include "qcril_qmi_err_utils.h"

#define TAG "RILQ"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DEFAULT_NAM_ID 0xFF

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  qcril_nv_get_3gpp2_subscription_info

===========================================================================*/
/*!
    @brief
    internel use only. Get 3gpp2 subscription info.

    @return
    error code.
    nas_get_3gpp2_subscription_info_resp_msg_v01 is returned back in get_config_resp_msg parameter
   if successful.
*/
/*=========================================================================*/
static RIL_Errno qcril_nv_get_3gpp2_subscription_info(
    nas_get_3gpp2_subscription_info_resp_msg_v01* get_config_resp_msg, int nam_id) {
  nas_get_3gpp2_subscription_info_req_msg_v01 get_config_req_msg;
  RIL_Errno ret = RIL_E_SUCCESS;

  /* To start with, set all the optional fields to be invalid */
  memset(&get_config_req_msg, 0, sizeof(get_config_req_msg));
  memset(get_config_resp_msg, 0, sizeof(*get_config_resp_msg));
  get_config_req_msg.nam_id = nam_id;

  if (QMI_NO_ERR != qmi_client_nas_send_sync(QMI_NAS_GET_3GPP2_SUBSCRIPTION_INFO_REQ_MSG_V01,
                                             &get_config_req_msg, sizeof(get_config_req_msg),
                                             get_config_resp_msg, sizeof(*get_config_resp_msg))) {
    QCRIL_LOG_INFO("qcril_qmi_client_send_msg_sync returned error: %d",
                   get_config_resp_msg->resp.error);
    ret = RIL_E_GENERIC_FAILURE;
  }
  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_nv_preset_min_based_info

===========================================================================*/
/*!
    @brief
    internel use only. set min_based_info to the current value.

    @return
    error code.
    current min value is set in min_based_info if the function call is successful.
*/
/*=========================================================================*/
static RIL_Errno qcril_nv_preset_min_based_info(nas_3gpp2_min_based_info_type_v01* min_based_info) {
  nas_get_3gpp2_subscription_info_resp_msg_v01 get_config_resp_msg;
  RIL_Errno ret = qcril_nv_get_3gpp2_subscription_info(&get_config_resp_msg, DEFAULT_NAM_ID);
  do {
    if (RIL_E_SUCCESS != ret) {
      break;
    }

    if (!get_config_resp_msg.min_based_info_valid) {
      QCRIL_LOG_INFO("min_based_info not available");
      ret = RIL_E_GENERIC_FAILURE;
    }

    memcpy(min_based_info, &get_config_resp_msg.min_based_info, sizeof(*min_based_info));
  } while (0);
  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_nv_preset_min_based_info

===========================================================================*/
/*!
    @brief
    internel use only. set true_imsi to the current value.

    @return
    error code.
    current true imsi value is set in true_imsi if the function call is successful.
*/
/*=========================================================================*/
static RIL_Errno qcril_nv_preset_true_imsi_info(nas_3gpp2_true_imsi_info_type_v01* true_imsi) {
  nas_get_3gpp2_subscription_info_resp_msg_v01 get_config_resp_msg;
  RIL_Errno ret = qcril_nv_get_3gpp2_subscription_info(&get_config_resp_msg, DEFAULT_NAM_ID);
  do {
    if (RIL_E_SUCCESS != ret) {
      break;
    }

    if (!get_config_resp_msg.true_imsi_valid) {
      QCRIL_LOG_INFO("true_imsi not available");
      ret = RIL_E_GENERIC_FAILURE;
    }

    memcpy(true_imsi, &get_config_resp_msg.true_imsi, sizeof(*true_imsi));
  } while (0);
  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_nv_preset_cdma_channel_info

===========================================================================*/
/*!
    @brief
    internel use only. set cdmach to the current value.

    @return
    error code.
    current cdma channel value is set in cdmach if the function call is successful.
*/
/*=========================================================================*/
static RIL_Errno qcril_nv_preset_cdma_channel_info(nas_cdma_channel_info_type_v01* cdmach) {
  nas_get_3gpp2_subscription_info_resp_msg_v01 get_config_resp_msg;
  RIL_Errno ret = qcril_nv_get_3gpp2_subscription_info(&get_config_resp_msg, DEFAULT_NAM_ID);
  do {
    if (RIL_E_SUCCESS != ret) {
      break;
    }

    if (!get_config_resp_msg.cdma_channel_info_valid) {
      QCRIL_LOG_INFO("cdma channel info not available");
      ret = RIL_E_GENERIC_FAILURE;
    }

    memcpy(cdmach, &get_config_resp_msg.cdma_channel_info, sizeof(*cdmach));
  } while (0);
  return ret;
}

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  qcril_nv_ascii_to_int

===========================================================================*/
/*!
    @brief
    Convert a non NULL terminated string to an integer

    @return
    the integer value of the string.
*/
/*=========================================================================*/
int qcril_nv_ascii_to_int(const char* str, int size) {
  int ret = 0;
  char* tmp = (char*)qcril_malloc(size + 1);
  if (tmp != NULL) {
    memcpy(tmp, str, size);
    tmp[size] = 0;
    ret = atoi(tmp);
    qcril_free(tmp);
  } else {
    ret = 0;
    QCRIL_LOG_FATAL("CHECK FAILED");
  }
  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_nv_int_to_ascii

===========================================================================*/
/*!
    @brief
    Convert an integer value to a non NULL terminated string

    @return
    None.
*/
/*=========================================================================*/
void qcril_nv_int_to_ascii(char* str, int size, int value) {
  int i;
  for (i = size - 1; i >= 0; i--) {
    str[i] = value % 10 + '0';
    value /= 10;
  }
}

/*=========================================================================

  FUNCTION:  qcril_nv_request_oem_hook_nv_read

===========================================================================*/
/*!
    @brief
    Reads the request NAM parameter from NV.

    @return
    If NV read is success then the corresponding NV item value is returned
    void
*/
/*=========================================================================*/
void qcril_nv_request_oem_hook_nv_read(std::shared_ptr<RilRequestNvReadMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (!msg) {
    return;
  }

  RIL_Errno errNo = RIL_E_GENERIC_FAILURE;
  nv_items_enum_type nv_item_id = msg->getNvItemId();
  QCRIL_LOG_INFO("NV: %s", getNvItemName(nv_item_id));
  auto nvReadResp = std::make_shared<qcril::interfaces::NvReadResult_t>();

  do {
    if (!nvReadResp) {
      errNo = RIL_E_NO_MEMORY;
      QCRIL_LOG_ERROR("Failed to create nvReadResp");
      break;
    }

    nvReadResp->nvItemId = nv_item_id;
    nv_item_type& nv_item = nvReadResp->nvItemValue;

    QCRIL_LOG_INFO("Received request for Reading nv_item_id = %lu", nv_item_id);

    if (!isValidNvItem(nv_item_id)) {
      QCRIL_LOG_INFO("Requested NV item not found = %lu", nv_item_id);
      errNo = RIL_E_INVALID_ARGUMENTS;
      break;
    }

    /* dispatch nv read request processing */
    switch (nv_item_id) {
      case NV_AUTO_ANSWER_I:
      case NV_PREF_VOICE_SO_I:
      case NV_ROAM_CNT_I:
      case NV_AIR_CNT_I: {
        voice_get_config_req_msg_v02 get_config_req_msg;

        /* To start with, set all the optional fields to be invalid */
        memset(&get_config_req_msg, 0, sizeof(get_config_req_msg));

        if (NV_AUTO_ANSWER_I == nv_item_id) {
          get_config_req_msg.auto_answer_valid = TRUE;
          get_config_req_msg.auto_answer = 1;
        } else if (NV_PREF_VOICE_SO_I == nv_item_id) {
          get_config_req_msg.pref_voice_so_valid = TRUE;
          get_config_req_msg.pref_voice_so = 1;
        } else if (NV_ROAM_CNT_I == nv_item_id) {
          get_config_req_msg.roam_timer_valid = TRUE;
          get_config_req_msg.roam_timer = 1;
        } else {
          /* case NV_AIR_CNT_I */
          get_config_req_msg.air_timer_valid = TRUE;
          get_config_req_msg.air_timer = 1;
        }

        std::shared_ptr<voice_get_config_resp_msg_v02> respPtr;
        auto msg = std::make_shared<VoiceGetConfigMessage>(get_config_req_msg);
        if (!msg){
          QCRIL_LOG_ERROR("Failed to create msg");
          errNo = RIL_E_GENERIC_FAILURE;
          break;
        }
        auto msgStatus = msg->dispatchSync(respPtr);
        QCRIL_LOG_INFO("Response %d", msgStatus);
        if (msgStatus != Message::Callback::Status::SUCCESS || !respPtr) {
          QCRIL_LOG_ERROR("FAILED");
          errNo = RIL_E_GENERIC_FAILURE;
          break;
        }
        voice_get_config_resp_msg_v02 &get_config_resp_msg = *respPtr;

        /* check response status */
        if (QMI_RESULT_SUCCESS_V01 != get_config_resp_msg.resp.result ||
            ((NV_AUTO_ANSWER_I == nv_item_id) && !(get_config_resp_msg.auto_answer_status_valid)) ||
            ((NV_PREF_VOICE_SO_I == nv_item_id) &&
             !(get_config_resp_msg.current_preferred_voice_so_valid)) ||
            ((NV_ROAM_CNT_I == nv_item_id) && !(get_config_resp_msg.roam_timer_count_valid)) ||
            ((NV_AIR_CNT_I == nv_item_id) && !(get_config_resp_msg.air_timer_count_valid))) {
          QCRIL_LOG_INFO("Response is invalid");
          errNo = RIL_E_GENERIC_FAILURE;
          break;
        }

        if (NV_AUTO_ANSWER_I == nv_item_id) {
          /* fill the data with response */
          nv_item.auto_answer.enable = get_config_resp_msg.auto_answer_status;
          /* TBD qmi voice does not support this field, we hard code 3 here as tmp workaround */
          nv_item.auto_answer.rings = 3;
          QCRIL_LOG_INFO("nv_item.auto_answer.enable: %d", nv_item.auto_answer.enable);
        } else if (NV_PREF_VOICE_SO_I == nv_item_id) {
          nv_item.pref_voice_so.nam = get_config_resp_msg.current_preferred_voice_so.nam_id;
          nv_item.pref_voice_so.evrc_capability_enabled =
              get_config_resp_msg.current_preferred_voice_so.evrc_capability;
          nv_item.pref_voice_so.home_page_voice_so =
              get_config_resp_msg.current_preferred_voice_so.home_page_voice_so;
          nv_item.pref_voice_so.home_orig_voice_so =
              get_config_resp_msg.current_preferred_voice_so.home_orig_voice_so;
          nv_item.pref_voice_so.roam_orig_voice_so =
              get_config_resp_msg.current_preferred_voice_so.roam_orig_voice_so;
          QCRIL_LOG_INFO("nv_item.pref_voice_so.nam: %d", nv_item.pref_voice_so.nam);
          QCRIL_LOG_INFO("nv_item.pref_voice_so.evrc_capability_enabled: %d",
                         nv_item.pref_voice_so.evrc_capability_enabled);
          QCRIL_LOG_INFO("nv_item.pref_voice_so.home_page_voice_so: %d",
                         nv_item.pref_voice_so.home_page_voice_so);
          QCRIL_LOG_INFO("nv_item.pref_voice_so.home_orig_voice_so: %d",
                         nv_item.pref_voice_so.home_orig_voice_so);
          QCRIL_LOG_INFO("nv_item.pref_voice_so.roam_orig_voice_so: %d",
                         nv_item.pref_voice_so.roam_orig_voice_so);
        } else if (NV_ROAM_CNT_I == nv_item_id) {
          nv_item.roam_cnt.nam = get_config_resp_msg.current_preferred_voice_so.nam_id;
          nv_item.roam_cnt.cnt = get_config_resp_msg.roam_timer_count.roam_timer;
          QCRIL_LOG_INFO("nv_item.roam_cnt.cnt: %d", nv_item.roam_cnt.cnt);
        } else {
          /* case NV_AIR_CNT_I */
          nv_item.air_cnt.nam = get_config_resp_msg.current_preferred_voice_so.nam_id;
          nv_item.air_cnt.cnt = get_config_resp_msg.air_timer_count.air_timer;
          QCRIL_LOG_INFO("nv_item.air_cnt.cnt: %d", nv_item.air_cnt.cnt);
        }
        errNo = RIL_E_SUCCESS;
      } break;
      case NV_MIN1_I:
      case NV_MIN2_I:
      case NV_IMSI_11_12_I:
      case NV_IMSI_MCC_I: /* NV_MIN1_I to NV_IMSI_MCC_I are retrieved from
                             nas_3gpp2_min_based_info_type_v01*/
      case NV_IMSI_T_MCC_I:
      case NV_IMSI_T_11_12_I:
      case NV_IMSI_T_S1_I:
      case NV_IMSI_T_S2_I:
      case NV_IMSI_T_ADDR_NUM_I: /* NV_IMSI_T_MCC_I to NV_IMSI_T_ADDR_NUM_I are retrieved from
                                    nas_3gpp2_true_imsi_info_type_v01*/
      case NV_PCDMACH_I:
      case NV_SCDMACH_I: /* NV_PCDMACH_I and NV_SCDMACH_I are retrieved from nas_cdma_channel_info_type_v01*/
      case NV_HOME_SID_NID_I: /* retrieved from nas_3gpp2_home_sid_nid_info_type_v01*/
      case NV_DIR_NUMBER_I:   /* retrieved from dir_num*/
      case NV_NAME_NAM_I:     /* retrieved from nam_name*/
      {
        nas_get_3gpp2_subscription_info_req_msg_v01 get_config_req_msg;
        nas_get_3gpp2_subscription_info_resp_msg_v01 get_config_resp_msg;

        /* To start with, set all the optional fields to be invalid */
        memset(&get_config_req_msg, 0, sizeof(get_config_req_msg));
        memset(&get_config_resp_msg, 0, sizeof(get_config_resp_msg));
        get_config_req_msg.nam_id = DEFAULT_NAM_ID;  // current NAM

        if (QMI_NO_ERR != qmi_client_nas_send_sync(QMI_NAS_GET_3GPP2_SUBSCRIPTION_INFO_REQ_MSG_V01,
                                                   &get_config_req_msg, sizeof(get_config_req_msg),
                                                   &get_config_resp_msg,
                                                   sizeof(get_config_resp_msg))) {
          QCRIL_LOG_INFO("qcril_qmi_client_send_msg_sync returned error: %d",
                         get_config_resp_msg.resp.error);
          errNo = RIL_E_GENERIC_FAILURE;
          break;
        }

        /* check response status */
        if (QMI_RESULT_SUCCESS_V01 != get_config_resp_msg.resp.result ||
            (((NV_MIN1_I == nv_item_id) || (NV_MIN2_I == nv_item_id) ||
              (NV_IMSI_11_12_I == nv_item_id) || (NV_IMSI_MCC_I == nv_item_id)) &&
             !(get_config_resp_msg.min_based_info_valid)) ||
            (((NV_IMSI_T_MCC_I == nv_item_id) || (NV_IMSI_T_11_12_I == nv_item_id) ||
              (NV_IMSI_T_S1_I == nv_item_id) || (NV_IMSI_T_S2_I == nv_item_id) ||
              (NV_IMSI_T_ADDR_NUM_I == nv_item_id)) &&
             !(get_config_resp_msg.true_imsi_valid)) ||
            (((NV_PCDMACH_I == nv_item_id) || (NV_SCDMACH_I == nv_item_id)) &&
             !(get_config_resp_msg.cdma_channel_info_valid)) ||
            ((NV_HOME_SID_NID_I == nv_item_id) && !(get_config_resp_msg.cdma_sys_id_valid)) ||
            ((NV_DIR_NUMBER_I == nv_item_id) && !(get_config_resp_msg.dir_num_valid)) ||
            ((NV_NAME_NAM_I == nv_item_id) && !(get_config_resp_msg.nam_name_valid))) {
          QCRIL_LOG_INFO("Response is invalid");
          errNo = RIL_E_GENERIC_FAILURE;
          break;
        }

        if (NV_MIN1_I == nv_item_id) {
          /* fill the data with response */
          nv_item.min1.nam = get_config_req_msg.nam_id;
          nv_item.min1.min1[1] = qcril_nv_ascii_to_int(get_config_resp_msg.min_based_info.imsi_m_s1,
                                                       NAS_IMSI_MIN1_LEN_V01);
          QCRIL_LOG_INFO("min1_val: %d", PII(nv_item.min1.min1[1]));
        } else if (NV_MIN2_I == nv_item_id) {
          /* fill the data with response */
          nv_item.min1.nam = get_config_req_msg.nam_id;
          nv_item.min2.min2[1] = qcril_nv_ascii_to_int(get_config_resp_msg.min_based_info.imsi_m_s2,
                                                       NAS_IMSI_MIN2_LEN_V01);
          QCRIL_LOG_INFO("min2_val: %d", PII(nv_item.min2.min2[1]));
        } else if (NV_IMSI_11_12_I == nv_item_id) {
          /* fill the data with response */
          nv_item.imsi_11_12.nam = get_config_req_msg.nam_id;
          nv_item.imsi_11_12.imsi_11_12 = qcril_nv_ascii_to_int(
              get_config_resp_msg.min_based_info.imsi_m_11_12, NAS_IMSI_11_12_LEN_V01);
          QCRIL_LOG_INFO("imsi_11_12 value: %d", PII(nv_item.imsi_11_12.imsi_11_12));
        } else if (NV_IMSI_MCC_I == nv_item_id) {
          /* fill the data with response */
          nv_item.imsi_mcc.nam = get_config_req_msg.nam_id;
          nv_item.imsi_mcc.imsi_mcc =
              qcril_nv_ascii_to_int(get_config_resp_msg.min_based_info.mcc_m, NAS_MCC_LEN_V01);
          QCRIL_LOG_INFO("imsi_mcc value: %d", PII(nv_item.imsi_mcc.imsi_mcc));
        } else if (NV_IMSI_T_MCC_I == nv_item_id) {
          /* fill the data with response */
          nv_item.imsi_t_mcc.nam = get_config_req_msg.nam_id;
          nv_item.imsi_t_mcc.imsi_mcc =
              qcril_nv_ascii_to_int(get_config_resp_msg.true_imsi.mcc_t, NAS_MCC_LEN_V01);
          QCRIL_LOG_INFO("imsi_t_mcc value: %d", PII(nv_item.imsi_t_mcc.imsi_mcc));
        } else if (NV_IMSI_T_11_12_I == nv_item_id) {
          /* fill the data with response */
          nv_item.imsi_t_11_12.nam = get_config_req_msg.nam_id;
          nv_item.imsi_t_11_12.imsi_11_12 = qcril_nv_ascii_to_int(
              get_config_resp_msg.true_imsi.imsi_t_11_12, NAS_IMSI_11_12_LEN_V01);
          QCRIL_LOG_INFO("imsi_t_11_12 value: %d", PII(nv_item.imsi_t_11_12.imsi_11_12));
        } else if (NV_IMSI_T_S1_I == nv_item_id) {
          /* fill the data with response */
          nv_item.imsi_t_s1.nam = get_config_req_msg.nam_id;
          nv_item.imsi_t_s1.min1[1] =
              qcril_nv_ascii_to_int(get_config_resp_msg.true_imsi.imsi_t_s1, NAS_IMSI_MIN1_LEN_V01);
          QCRIL_LOG_INFO("imsi_t_s1: %d", PII(nv_item.imsi_t_s1.min1[1]));
        } else if (NV_IMSI_T_S2_I == nv_item_id) {
          /* fill the data with response */
          nv_item.imsi_t_s2.nam = get_config_req_msg.nam_id;
          nv_item.imsi_t_s2.min2[1] =
              qcril_nv_ascii_to_int(get_config_resp_msg.true_imsi.imsi_t_s2, NAS_IMSI_MIN2_LEN_V01);
          QCRIL_LOG_INFO("imsi_t_s2: %d", PII(nv_item.imsi_t_s2.min2[1]));
        } else if (NV_IMSI_T_ADDR_NUM_I == nv_item_id) {
          /* fill the data with response */
          nv_item.imsi_t_addr_num.nam = get_config_req_msg.nam_id;
          nv_item.imsi_t_addr_num.num = get_config_resp_msg.true_imsi.imsi_t_addr_num;
          QCRIL_LOG_INFO("imsi_t_addr_num value: %d", nv_item.imsi_t_addr_num.num);
        } else if (NV_PCDMACH_I == nv_item_id) {
          nv_item.pcdmach.nam = get_config_req_msg.nam_id;
          nv_item.pcdmach.channel_a = get_config_resp_msg.cdma_channel_info.pri_ch_a;
          nv_item.pcdmach.channel_b = get_config_resp_msg.cdma_channel_info.pri_ch_b;
          QCRIL_LOG_INFO("pcdmach value: ch_a: %d, ch_b: %d", nv_item.pcdmach.channel_a,
                         nv_item.pcdmach.channel_b);
        } else if (NV_SCDMACH_I == nv_item_id) {
          nv_item.scdmach.nam = get_config_req_msg.nam_id;
          nv_item.scdmach.channel_a = get_config_resp_msg.cdma_channel_info.sec_ch_a;
          nv_item.scdmach.channel_b = get_config_resp_msg.cdma_channel_info.sec_ch_b;
          QCRIL_LOG_INFO("scdmach value: ch_a: %d, ch_b: %d", nv_item.scdmach.channel_a,
                         nv_item.scdmach.channel_b);
        } else if (NV_HOME_SID_NID_I == nv_item_id) {
          nv_item.home_sid_nid.nam = get_config_req_msg.nam_id;
          uint32_t i;
          for (i = 0; i < get_config_resp_msg.cdma_sys_id_len; i++) {
            nv_item.home_sid_nid.pair[i].sid = get_config_resp_msg.cdma_sys_id[i].sid;
            nv_item.home_sid_nid.pair[i].nid = get_config_resp_msg.cdma_sys_id[i].nid;
          }
        } else if (NV_DIR_NUMBER_I == nv_item_id) {
          nv_item.dir_number.nam = get_config_req_msg.nam_id;
          memcpy(nv_item.dir_number.dir_number, get_config_resp_msg.dir_num,
                 get_config_resp_msg.dir_num_len);
        } else {
          /* case NV_NAME_NAM_I */
          nv_item.name_nam.nam = get_config_req_msg.nam_id;
          memcpy(nv_item.name_nam.name, get_config_resp_msg.nam_name,
                 get_config_resp_msg.nam_name_len);
        }
        errNo = RIL_E_SUCCESS;
      } break;
      case NV_SID_NID_I: {
        nas_get_home_network_resp_msg_v01 get_config_resp_msg;

        /* To start with, set all the optional fields to be invalid */
        memset(&get_config_resp_msg, 0, sizeof(get_config_resp_msg));

        if (QMI_NO_ERR != qmi_client_nas_send_sync(QMI_NAS_GET_HOME_NETWORK_REQ_MSG_V01, NULL, 0,
                                                   &get_config_resp_msg,
                                                   sizeof(get_config_resp_msg))) {
          QCRIL_LOG_INFO("qcril_qmi_client_send_msg_sync returned error: %d",
                         get_config_resp_msg.resp.error);
          errNo = RIL_E_GENERIC_FAILURE;
          break;
        }

        /* check response status */
        if (QMI_RESULT_SUCCESS_V01 != get_config_resp_msg.resp.result ||
            !(get_config_resp_msg.home_system_id_valid)) {
          QCRIL_LOG_INFO("Response is invalid");
          errNo = RIL_E_GENERIC_FAILURE;
          break;
        }

        /* fill the data with response */
        nv_item.sid_nid.pair[1][0].sid = get_config_resp_msg.home_system_id.sid;
        nv_item.sid_nid.pair[1][0].nid = get_config_resp_msg.home_system_id.nid;
        QCRIL_LOG_INFO("sid val: %d; nid val: %d", nv_item.sid_nid.pair[1][0].sid,
                       nv_item.sid_nid.pair[1][0].nid);

        errNo = RIL_E_SUCCESS;
      } break;
      case NV_MOB_CAI_REV_I: {
        nas_get_mob_cai_rev_resp_v01 get_config_resp_msg;

        /* To start with, set all the optional fields to be invalid */
        memset(&get_config_resp_msg, 0, sizeof(get_config_resp_msg));

        if (QMI_NO_ERR != qmi_client_nas_send_sync(QMI_NAS_GET_MOB_CAI_REV_REQ_MSG_V01, NULL, 0,
                                                   &get_config_resp_msg,
                                                   sizeof(get_config_resp_msg))) {
          QCRIL_LOG_INFO("qcril_qmi_client_send_msg_sync returned error: %d",
                         get_config_resp_msg.resp.error);
          errNo = RIL_E_GENERIC_FAILURE;
          break;
        }

        /* check response status */
        if (QMI_RESULT_SUCCESS_V01 != get_config_resp_msg.resp.result ||
            !(get_config_resp_msg.cai_rev_valid)) {
          QCRIL_LOG_INFO("Response is invalid");
          errNo = RIL_E_GENERIC_FAILURE;
          break;
        }

        /* fill the data with response */
        nv_item.mob_cai_rev = get_config_resp_msg.cai_rev;
        QCRIL_LOG_INFO("nv_item.mob_cai_rev val: %d", nv_item.mob_cai_rev);
        errNo = RIL_E_SUCCESS;
      } break;
      default:
        QCRIL_LOG_INFO("Requested NV item not supported = %lu", nv_item_id);
        errNo = RIL_E_GENERIC_FAILURE;
        break;
    }
  } while (0);

  if (nvReadResp) {
    if (errNo == RIL_E_SUCCESS) {
      nvReadResp->nvItemValueSize = getNvItemSize(nv_item_id);
    }
    QCRIL_LOG_DEBUG("errNo = %d, nvItemId = %d, nvItemValueSize = %d", errNo, nvReadResp->nvItemId,
                    nvReadResp->nvItemValueSize);
  }
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
                    std::make_shared<QcRilRequestMessageCallbackPayload>(errNo, nvReadResp));
  QCRIL_LOG_FUNC_RETURN();
} /* qcril_nv_request_oem_hook_nv_read */

/*=========================================================================

  FUNCTION:  qcril_nv_request_oem_hook_nv_write

===========================================================================*/
/*!
    @brief
    Writes the requested NAM parameter to NV item after validating the data.

    @return
    Void
    NV Write status is returned back in nv_write_status parameter.
*/
/*=========================================================================*/
void qcril_nv_request_oem_hook_nv_write(std::shared_ptr<RilRequestNvWriteMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (!msg) {
    return;
  }
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;
  RIL_Errno ril_req_res = RIL_E_SUCCESS;

  do {
    nv_items_enum_type nv_item_id = msg->getNvItemId();
    nv_item_type nv_item = msg->getNvItem();

    QCRIL_LOG_INFO("NV: %s", getNvItemName(nv_item_id));

    switch (nv_item_id) {
      case NV_AUTO_ANSWER_I:
      case NV_PREF_VOICE_SO_I:
      case NV_ROAM_CNT_I:
      case NV_AIR_CNT_I: {
        voice_set_config_req_msg_v02 set_config_req_msg;

        /* To start with, set all the optional fields to be invalid */
        memset(&set_config_req_msg, 0, sizeof(set_config_req_msg));

        if (NV_AUTO_ANSWER_I == nv_item_id) {
          set_config_req_msg.auto_answer_valid = TRUE;
          set_config_req_msg.auto_answer = nv_item.auto_answer.enable;
          /* TBD nv_item.auto_answer.rings
           *     qmi voice does not support this field, we ignore this field for now
           */
        } else if (NV_PREF_VOICE_SO_I == nv_item_id) {
          /* NV_PREF_VOICE_SO_I case */
          set_config_req_msg.preferred_voice_so_valid = TRUE;
          set_config_req_msg.preferred_voice_so.nam_id = nv_item.pref_voice_so.nam;
          set_config_req_msg.preferred_voice_so.evrc_capability =
              nv_item.pref_voice_so.evrc_capability_enabled;
          set_config_req_msg.preferred_voice_so.home_page_voice_so =
              static_cast<voice_so_enum_v02>(nv_item.pref_voice_so.home_page_voice_so);
          set_config_req_msg.preferred_voice_so.home_orig_voice_so =
              static_cast<voice_so_enum_v02>(nv_item.pref_voice_so.home_orig_voice_so);
          set_config_req_msg.preferred_voice_so.roam_orig_voice_so =
              static_cast<voice_so_enum_v02>(nv_item.pref_voice_so.roam_orig_voice_so);
        } else if (NV_ROAM_CNT_I == nv_item_id) {
          set_config_req_msg.roam_timer_valid = TRUE;
          set_config_req_msg.roam_timer.roam_timer = nv_item.roam_cnt.cnt;
        } else {
          /* case NV_AIR_CNT_I */
          set_config_req_msg.air_timer_valid = TRUE;
          set_config_req_msg.air_timer.air_timer = nv_item.air_cnt.cnt;
        }

        std::shared_ptr<voice_set_config_resp_msg_v02> respPtr;
        auto msg = std::make_shared<VoiceSetConfigMessage>(set_config_req_msg);
        if (!msg){
          QCRIL_LOG_ERROR("Failed to create msg");
          ril_req_res = RIL_E_GENERIC_FAILURE;
          break;
        }
        auto msgStatus = msg->dispatchSync(respPtr);
        QCRIL_LOG_INFO("Response %d", msgStatus);
        if (msgStatus != Message::Callback::Status::SUCCESS || !respPtr) {
          QCRIL_LOG_ERROR("FAILED");
          ril_req_res = RIL_E_GENERIC_FAILURE;
          break;
        }
        voice_set_config_resp_msg_v02 &set_config_resp_msg = *respPtr;

        /* check response status */
        ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
            qmi_client_error, &set_config_resp_msg.resp);
        QCRIL_LOG_INFO("ril_req_res %d, qmi_client_error %d", (int)ril_req_res, qmi_client_error);
      } break;
      case NV_MIN1_I:
      case NV_MIN2_I:
      case NV_IMSI_11_12_I:
      case NV_IMSI_MCC_I: /* retrieved from nas_3gpp2_min_based_info_type_v01*/
      case NV_IMSI_T_MCC_I:
      case NV_IMSI_T_11_12_I:
      case NV_IMSI_T_S1_I:
      case NV_IMSI_T_S2_I:
      case NV_IMSI_T_ADDR_NUM_I: /* retrieved from nas_3gpp2_true_imsi_info_type_v01*/
      case NV_PCDMACH_I:
      case NV_SCDMACH_I:      /* retrieved from nas_cdma_channel_info_type_v01*/
      case NV_HOME_SID_NID_I: /* retrieved from NV_HOME_SID_NID_I*/
      case NV_DIR_NUMBER_I:   /* retrieved from NV_HOME_SID_NID_I*/
      case NV_NAME_NAM_I:     /* retrieved from nam_name*/
      {
        nas_set_3gpp2_subscription_info_req_msg_v01 set_config_req_msg;
        nas_set_3gpp2_subscription_info_resp_msg_v01 set_config_resp_msg;

        /* To start with, set all the optional fields to be invalid */
        memset(&set_config_req_msg, 0, sizeof(set_config_req_msg));
        memset(&set_config_resp_msg, 0, sizeof(set_config_resp_msg));

        set_config_req_msg.nam_id = DEFAULT_NAM_ID;

        if (NV_MIN1_I == nv_item_id) {
          /* first need to fill the whole min_based info struct*/
          RIL_Errno ret = qcril_nv_preset_min_based_info(&set_config_req_msg.min_based_info);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.min_based_info_valid = 1;

          /* fill min1 */
          qcril_nv_int_to_ascii(set_config_req_msg.min_based_info.imsi_m_s1, NAS_IMSI_MIN1_LEN_V01,
                                nv_item.min1.min1[1]);
        } else if (NV_MIN2_I == nv_item_id) {
          /* first need to fill the whole min_based info struct*/
          RIL_Errno ret = qcril_nv_preset_min_based_info(&set_config_req_msg.min_based_info);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.min_based_info_valid = 1;

          /* fill min2 */
          qcril_nv_int_to_ascii(set_config_req_msg.min_based_info.imsi_m_s2, NAS_IMSI_MIN2_LEN_V01,
                                nv_item.min2.min2[1]);
        } else if (NV_IMSI_11_12_I == nv_item_id) {
          /* first need to fill the whole min_based info struct*/
          RIL_Errno ret = qcril_nv_preset_min_based_info(&set_config_req_msg.min_based_info);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.min_based_info_valid = 1;

          /* fill imsi_11_22 */
          qcril_nv_int_to_ascii(set_config_req_msg.min_based_info.imsi_m_11_12,
                                NAS_IMSI_11_12_LEN_V01, nv_item.imsi_11_12.imsi_11_12);
        } else if (NV_IMSI_MCC_I == nv_item_id) {
          /* first need to fill the whole min_based info struct*/
          RIL_Errno ret = qcril_nv_preset_min_based_info(&set_config_req_msg.min_based_info);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.min_based_info_valid = 1;

          /* fill imsi_11_22 */
          qcril_nv_int_to_ascii(set_config_req_msg.min_based_info.mcc_m, NAS_MCC_LEN_V01,
                                nv_item.imsi_11_12.imsi_11_12);
        } else if (NV_IMSI_T_MCC_I == nv_item_id) {
          /* first need to fill the whole true_imsi struct*/
          RIL_Errno ret = qcril_nv_preset_true_imsi_info(&set_config_req_msg.true_imsi);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.true_imsi_valid = 1;

          /* fill imsi_11_22 */
          qcril_nv_int_to_ascii(set_config_req_msg.true_imsi.mcc_t, NAS_MCC_LEN_V01,
                                nv_item.imsi_t_mcc.imsi_mcc);
        } else if (NV_IMSI_T_11_12_I == nv_item_id) {
          /* first need to fill the whole true_imsi struct*/
          RIL_Errno ret = qcril_nv_preset_true_imsi_info(&set_config_req_msg.true_imsi);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.true_imsi_valid = 1;

          /* fill imsi_11_22 */
          qcril_nv_int_to_ascii(set_config_req_msg.true_imsi.imsi_t_11_12, NAS_IMSI_11_12_LEN_V01,
                                nv_item.imsi_t_11_12.imsi_11_12);
        } else if (NV_IMSI_T_S1_I == nv_item_id) {
          /* first need to fill the whole true_imsi struct*/
          RIL_Errno ret = qcril_nv_preset_true_imsi_info(&set_config_req_msg.true_imsi);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.true_imsi_valid = 1;

          /* fill imsi_t_s1 */
          qcril_nv_int_to_ascii(set_config_req_msg.true_imsi.imsi_t_s1, NAS_IMSI_MIN1_LEN_V01,
                                nv_item.imsi_t_s1.min1[1]);
        } else if (NV_IMSI_T_S2_I == nv_item_id) {
          /* first need to fill the whole true_imsi struct*/
          RIL_Errno ret = qcril_nv_preset_true_imsi_info(&set_config_req_msg.true_imsi);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.true_imsi_valid = 1;

          /* fill imsi_t_s2 */
          qcril_nv_int_to_ascii(set_config_req_msg.true_imsi.imsi_t_s2, NAS_IMSI_MIN2_LEN_V01,
                                nv_item.imsi_t_s2.min2[1]);
        } else if (NV_IMSI_T_ADDR_NUM_I == nv_item_id) {
          /* first need to fill the whole true_imsi struct*/
          RIL_Errno ret = qcril_nv_preset_true_imsi_info(&set_config_req_msg.true_imsi);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.true_imsi_valid = 1;

          /* fill imsi_t_addr_num */
          set_config_req_msg.true_imsi.imsi_t_addr_num = nv_item.imsi_t_addr_num.num;
        } else if (NV_PCDMACH_I == nv_item_id) {
          /* first need to fill the whole cdma_channel_info struct*/
          RIL_Errno ret = qcril_nv_preset_cdma_channel_info(&set_config_req_msg.cdma_channel_info);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.cdma_channel_info_valid = 1;

          set_config_req_msg.cdma_channel_info.pri_ch_a = nv_item.pcdmach.channel_a;
          set_config_req_msg.cdma_channel_info.pri_ch_b = nv_item.pcdmach.channel_b;
        } else if (NV_SCDMACH_I == nv_item_id) {
          /* first need to fill the whole cdma_channel_info struct*/
          RIL_Errno ret = qcril_nv_preset_cdma_channel_info(&set_config_req_msg.cdma_channel_info);
          if (RIL_E_SUCCESS != ret) {
            ril_req_res = ret;
            break;
          }
          set_config_req_msg.cdma_channel_info_valid = 1;

          set_config_req_msg.cdma_channel_info.sec_ch_a = nv_item.scdmach.channel_a;
          set_config_req_msg.cdma_channel_info.sec_ch_b = nv_item.scdmach.channel_b;
        } else if (NV_HOME_SID_NID_I == nv_item_id) {
          set_config_req_msg.cdma_sys_id_valid = 1;
          set_config_req_msg.cdma_sys_id_len = NV_MAX_HOME_SID_NID;
          uint32_t i;
          for (i = 0; i < set_config_req_msg.cdma_sys_id_len; i++) {
            set_config_req_msg.cdma_sys_id[i].sid = nv_item.home_sid_nid.pair[i].sid;
            set_config_req_msg.cdma_sys_id[i].nid = nv_item.home_sid_nid.pair[i].nid;
          }
        } else if (NV_DIR_NUMBER_I == nv_item_id) {
          set_config_req_msg.dir_num_valid = 1;
          set_config_req_msg.nam_id = nv_item.dir_number.nam;
          set_config_req_msg.dir_num_len = strlen((char*)nv_item.dir_number.dir_number);
          memcpy(set_config_req_msg.dir_num, nv_item.dir_number.dir_number,
                 set_config_req_msg.dir_num_len);
        } else {
          /* case NV_NAME_NAM_I */
          set_config_req_msg.nam_name_valid = 1;
          set_config_req_msg.nam_id = nv_item.name_nam.nam;
          set_config_req_msg.nam_name_len = strlen((char*)nv_item.name_nam.name);
          memcpy(set_config_req_msg.nam_name, nv_item.name_nam.name,
                 set_config_req_msg.nam_name_len);
        }

        if (msg->hasSpc()) {
          set_config_req_msg.spc_valid = TRUE;
          memcpy(set_config_req_msg.spc, msg->getSpc(), sizeof(set_config_req_msg.spc));
        }

        if (RIL_E_SUCCESS == ril_req_res) {
          qmi_client_error = qmi_client_nas_send_sync(
              QMI_NAS_SET_3GPP2_SUBSCRIPTION_INFO_REQ_MSG_V01, &set_config_req_msg,
              sizeof(set_config_req_msg), &set_config_resp_msg, sizeof(set_config_resp_msg));

          /* check response status */
          ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
              qmi_client_error, &set_config_resp_msg.resp);
          QCRIL_LOG_INFO("ril_req_res %d, qmi_client_error %d", (int)ril_req_res, qmi_client_error);
        }
      } break;
      default:
        QCRIL_LOG_INFO("Requested NV item not supported = %lu", nv_item_id);
        ril_req_res = RIL_E_GENERIC_FAILURE;
        break;
    }
  } while (0);
  QCRIL_LOG_DEBUG("ril_req_res = %d", ril_req_res);
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
                    std::make_shared<QcRilRequestMessageCallbackPayload>(ril_req_res, nullptr));
  QCRIL_LOG_FUNC_RETURN();
} /* qcril_nv_request_oem_hook_nv_write */
