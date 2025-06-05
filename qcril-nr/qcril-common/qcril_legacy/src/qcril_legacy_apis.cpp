/******************************************************************************
#  Copyright (c) 2017, 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "cutils/properties.h"
#include "framework/legacy.h"
#include "framework/Log.h"
#include "PeripheralMgr.h"
#include "simple_list.h"
#include "qcril_legacy_apis.h"

#define QMI_RIL_SYS_PROP_NAME_BASEBAND "ro.baseband"
#define QMI_RIL_SYS_PROP_NAME_SGLTE_CSFB "persist.vendor.radio.sglte_csfb"
#define QMI_RIL_SYS_PROP_SGLTE_CSFB_LENGTH 4
#define QMI_RIL_SYS_PROP_RAT_OPTION "persist.vendor.radio.rat_on"
#define QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP "persist.vendor.radio.ims_retry_3gpp"
#define QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP2 "persist.vendor.radio.ims_retry_3gpp2"
#define QCRIL_FEATURE_KDDI_HOLD_ANSWER_ON "persist.vendor.radio.kddi_hold_answ_on"
#define QMI_RIL_SYS_PROP_NAME_OEM_IND_TO_BOTH "persist.vendor.radio.oem_ind_to_both"
#define QMI_RIL_SYS_PROP_NAME_OEM_SOCKET "persist.vendor.radio.oem_socket"
#define QCRIL_FEATURE_SAP_SILENT_PIN_VERIFY "persist.vendor.radio.sap_silent_pin"
#define QCRIL_PROCESS_BLOCK_ALLOW_DATA "persist.vendor.radio.block_allow_data"
#define QMI_RIL_SYS_PROP_NAME_SUPPRESS_REQ "persist.vendor.radio.suppress_req"
#define QMI_RIL_SYS_PROP_RIL_DATA_REG_STATE_CONDITIONAL_REPORT "persist.vendor.radio.data_con_rprt"

#define TAG "legacy_api"

static qcril_instance_id_e_type qmi_ril_process_instance_id = QCRIL_DEFAULT_INSTANCE_ID;
static uint32_t qmi_ril_sim_slot;  ///< sim slot associated w/this ril instance

typedef struct {
  int is_valid;
  pthread_t thread_id;
  char thread_name[QMI_RIL_THREAD_NAME_MAX_SIZE];
} qmi_ril_thread_name_info_type;

#ifndef QMI_RIL_UTF
static
#endif
    qmi_ril_thread_name_info_type qmi_ril_thread_name_info[QMI_RIL_THREAD_INFO_MAX_SIZE];

static qcril_qmi_ftr_multi_sim_info_type qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_MULTI_SIM_UNKNOWN;
static qcril_qmi_ftr_sglte_csfb_info_type qmi_ril_sglte_csfb_ftr_info =
    QMI_RIL_FTR_SGLTE_CSFB_UNKNOWN;
static qcril_qmi_ftr_baseband_info_type qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_UNKNOWN;
static qcril_qmi_ftr_baseband_info_type qmi_ril_sglte_ftr_info = QMI_RIL_FTR_BASEBAND_UNKNOWN;
static qcril_qmi_ftr_rat_enable_option qmi_ril_rat_enable_option = QMI_RIL_FTR_RAT_UNKNOWN;
static qcril_qmi_kddi_hold_answer_info_type qmi_ril_kddi_hold_answer =
    QMI_RIL_KDDI_HOLD_ANSWER_UNKNOWN;
static qcril_qmi_feature_type qmi_ril_oem_ind_to_both = QMI_RIL_FEATURE_UNKNOWN;
static qcril_qmi_feature_type qmi_ril_ftr_suppress_req = QMI_RIL_FEATURE_UNKNOWN;

/*===========================================================================

  FUNCTION:  qcril_get_baseband_name

===========================================================================*/
/*!
    @brief
    retrieve baseband info

    @return
    None
*/
/*=========================================================================*/
void qcril_get_baseband_name(char* prop_str) {
  const char* link_name = NULL;
  if (prop_str) {
    property_get(QMI_RIL_SYS_PROP_NAME_BASEBAND, prop_str, "");

    if (!strcmp("mdm", prop_str) || !strcmp("mdm2", prop_str)) {
      link_name = PeripheralMgr::getInstance().getEsocLinkName();

      if (link_name) {
        QCRIL_LOG_INFO("link name %s ", link_name);
        if (!strcmp("HSIC", link_name)) {
          strlcpy(prop_str, "mdm", PROPERTY_VALUE_MAX);
        } else {
          strlcpy(prop_str, "mdm2", PROPERTY_VALUE_MAX);
        }
      }
    }
  }

  return;
}

//===========================================================================
// qmi_ril_reset_multi_sim_ftr_info
//===========================================================================
void qmi_ril_reset_multi_sim_ftr_info() {
  qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_MULTI_SIM_UNKNOWN;
}  // qmi_ril_reset_multi_sim_ftr_info

//===========================================================================
// qmi_ril_is_feature_supported
//===========================================================================
int qmi_ril_is_feature_supported(int feature) {
  int res = FALSE;
  char prop_str[PROPERTY_VALUE_MAX];

  switch (feature) {
    case QMI_RIL_FEATURE_FUSION_CSFB:
    case QMI_RIL_FEATURE_APQ:
    case QMI_RIL_FEATURE_MSM:
    case QMI_RIL_FEATURE_SVLTE2:
    case QMI_RIL_FEATURE_SGLTE:
    case QMI_RIL_FEATURE_DSDA:
    case QMI_RIL_FEATURE_DSDA2:
      if (QMI_RIL_FTR_BASEBAND_UNKNOWN == qmi_ril_baseband_ftr_info) {
        *prop_str = 0;

        qcril_get_baseband_name(prop_str);

        if (strcmp(prop_str, "apq") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_APQ;
        } else if (strcmp(prop_str, "mdm") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_APQ;
        } else if (strcmp(prop_str, "mdm2") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_MDM2;
        } else if (strcmp(prop_str, "msm") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_MSM;
        } else if (strcmp(prop_str, "csfb") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_CSFB;
        } else if (strcmp(prop_str, "svlte2a") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_SVLTE2;
        } else if (strcmp(prop_str, "sglte2") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_SGLTE;
        } else if (strcmp(prop_str, "sglte") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_SGLTE;
        } else if (strcmp(prop_str, "dsda") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_DSDA;
        } else if (strcmp(prop_str, "dsda2") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_DSDA2;
        } else if (strcmp(prop_str, "auto") == 0) {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_APQ;
        } else {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_NONE;
        }
      }

      if (QMI_RIL_FEATURE_FUSION_CSFB == feature &&
          QMI_RIL_FTR_BASEBAND_CSFB == qmi_ril_baseband_ftr_info) {
        res = TRUE;
      } else if (QMI_RIL_FEATURE_APQ == feature &&
                 QMI_RIL_FTR_BASEBAND_APQ == qmi_ril_baseband_ftr_info) {
        res = TRUE;
      } else if (QMI_RIL_FEATURE_APQ == feature &&
                 QMI_RIL_FTR_BASEBAND_MDM2 == qmi_ril_baseband_ftr_info) {
        res = TRUE;
      } else if (QMI_RIL_FEATURE_MSM == feature &&
                 QMI_RIL_FTR_BASEBAND_MSM == qmi_ril_baseband_ftr_info) {
        res = TRUE;
      } else if (QMI_RIL_FEATURE_SVLTE2 == feature &&
                 QMI_RIL_FTR_BASEBAND_SVLTE2 == qmi_ril_baseband_ftr_info) {
        res = TRUE;
      } else if (QMI_RIL_FEATURE_SGLTE == feature &&
                 QMI_RIL_FTR_BASEBAND_SGLTE == qmi_ril_baseband_ftr_info) {
        res = TRUE;
      } else if (QMI_RIL_FEATURE_DSDA == feature &&
                 QMI_RIL_FTR_BASEBAND_DSDA == qmi_ril_baseband_ftr_info) {
        res = TRUE;
      } else if (QMI_RIL_FEATURE_DSDA2 == feature &&
                 QMI_RIL_FTR_BASEBAND_DSDA2 == qmi_ril_baseband_ftr_info) {
        res = TRUE;
      } else {
        res = FALSE;
      }
      break;

    case QMI_RIL_FEATURE_SGLTE2:
      if (QMI_RIL_FTR_BASEBAND_UNKNOWN == qmi_ril_sglte_ftr_info) {
        *prop_str = 0;
        property_get(QMI_RIL_SYS_PROP_NAME_BASEBAND, prop_str, "");
        if (strcmp(prop_str, "sglte2") == 0) {
          qmi_ril_sglte_ftr_info = QMI_RIL_FTR_BASEBAND_SGLTE2;
        }
      }

      switch (qmi_ril_sglte_ftr_info) {
        case QMI_RIL_FTR_BASEBAND_SGLTE2:
          res = TRUE;
          break;

        default:
          res = FALSE;
          break;
      }

      break;
    case QMI_RIL_FEATURE_SGLTE_CSFB:
      if (QMI_RIL_FTR_SGLTE_CSFB_UNKNOWN == qmi_ril_sglte_csfb_ftr_info) {
        *prop_str = 0;
        property_get(QMI_RIL_SYS_PROP_NAME_SGLTE_CSFB, prop_str, "");
        if (strncmp(prop_str, "true", QMI_RIL_SYS_PROP_SGLTE_CSFB_LENGTH) == 0) {
          qmi_ril_sglte_csfb_ftr_info = QMI_RIL_FTR_SGLTE_CSFB_YES;
        } else {
          qmi_ril_sglte_csfb_ftr_info = QMI_RIL_FTR_SGLTE_CSFB_NO;
        }
      }

      switch (qmi_ril_sglte_csfb_ftr_info) {
        case QMI_RIL_FTR_SGLTE_CSFB_YES:
          res = TRUE;
          break;

        default:
          res = FALSE;
          break;
      }

      break;

    case QMI_RIL_FEATURE_DSDS:
    case QMI_RIL_FEATURE_TSTS:
      if (QMI_RIL_FTR_MULTI_SIM_UNKNOWN == qmi_ril_multi_sim_ftr_info) {
        *prop_str = 0;
        property_get(QMI_RIL_SYS_PROP_NAME_MULTI_SIM, prop_str, "");
        if (strncmp(prop_str, "dsds", QMI_RIL_SYS_PROP_LENGTH_MULTI_SIM) == 0) {
          qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_DSDS_ENABLED;
        } else if (strncmp(prop_str, "tsts", QMI_RIL_SYS_PROP_LENGTH_MULTI_SIM) == 0) {
          qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_TSTS_ENABLED;
        } else if (strncmp(prop_str, "dsda", QMI_RIL_SYS_PROP_LENGTH_MULTI_SIM) == 0) {
          qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_DSDA_ENABLED;
        } else {
          qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_MULTI_SIM_DISABLED;
        }
      }

      if ((feature == QMI_RIL_FEATURE_DSDS) &&
          ((qmi_ril_multi_sim_ftr_info == QMI_RIL_FTR_DSDS_ENABLED) ||
           (qmi_ril_multi_sim_ftr_info == QMI_RIL_FTR_DSDA_ENABLED))) {
        // DSDS/DSDA is enabled
        res = TRUE;
      } else if ((feature == QMI_RIL_FEATURE_TSTS) &&
                 (qmi_ril_multi_sim_ftr_info == QMI_RIL_FTR_TSTS_ENABLED)) {
        // TSTS is enabled
        res = TRUE;
      } else {
        res = FALSE;
      }
      break;

    case QMI_RIL_FEATURE_LEGACY_RAT:
    case QMI_RIL_FEATURE_COMBINE_RAT:
      if (QMI_RIL_FTR_RAT_UNKNOWN == qmi_ril_rat_enable_option) {
        *prop_str = 0;
        property_get(QMI_RIL_SYS_PROP_RAT_OPTION, prop_str, "");
        if (!strncmp(prop_str, "legacy", 6)) {
          qmi_ril_rat_enable_option = QMI_RIL_FTR_RAT_LEGACY;
        } else if (!strncmp(prop_str, "combine", 7)) {
          qmi_ril_rat_enable_option = QMI_RIL_FTR_RAT_COMBINE;
        } else {
          qmi_ril_rat_enable_option = QMI_RIL_FTR_RAT_DISBLE;
        }
      }

      if ((feature == QMI_RIL_FEATURE_LEGACY_RAT) &&
          (qmi_ril_rat_enable_option == QMI_RIL_FTR_RAT_LEGACY)) {
        res = TRUE;
      } else if ((feature == QMI_RIL_FEATURE_COMBINE_RAT) &&
                 (qmi_ril_rat_enable_option == QMI_RIL_FTR_RAT_COMBINE)) {
        res = TRUE;
      } else {
        res = FALSE;
      }
      break;

    case QMI_RIL_FEATURE_KDDI_HOLD_ANSWER:
      res = FALSE;
      char prop[PROPERTY_VALUE_MAX];

      if (QMI_RIL_KDDI_HOLD_ANSWER_UNKNOWN == qmi_ril_kddi_hold_answer) {
        property_get(QCRIL_FEATURE_KDDI_HOLD_ANSWER_ON, prop, "");
        if ((strlen(prop) > 0) && atoi(prop)) {
          qmi_ril_kddi_hold_answer = QMI_RIL_KDDI_HOLD_ANSWER_ENABLED;
        } else {
          qmi_ril_kddi_hold_answer = QMI_RIL_KDDI_HOLD_ANSWER_DISABLED;
        }
      }

      switch (qmi_ril_kddi_hold_answer) {
        case QMI_RIL_KDDI_HOLD_ANSWER_ENABLED:
          res = TRUE;
          break;

        case QMI_RIL_KDDI_HOLD_ANSWER_DISABLED:
        default:
          res = FALSE;
          break;
      }
      break;

    case QMI_RIL_FEATURE_SHM:
#ifdef FEATURE_QCRIL_SHM
      res = TRUE;
#else
      res = FALSE;
#endif
      break;

    case QMI_RIL_FEATURE_IMS:
/* SMS over IMS is supported only if FEATURE_QCRIL_IMS is defined */
#ifdef FEATURE_QCRIL_IMS
      res = TRUE;
#else
      res = FALSE;
#endif
      break;

    case QMI_RIL_FEATURE_IMS_RETRY_3GPP:
      // feature is enabled (true) by default, eg. if property set to anything
      // other than "false" or "0", including if its missing (unset).
      *prop_str = 0;
      property_get(QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP, prop_str, "");
      res = ((strcmp(prop_str, "false") != 0) && (strcmp(prop_str, "0") != 0));
      QCRIL_LOG_DEBUG("property %s = \"%s\", %d", QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP, prop_str,
                      res);
      break;

    case QMI_RIL_FEATURE_IMS_RETRY_3GPP2:
      // feature is enabled (true) by default, eg. if property set to anything
      // other than "false" or "0", including if its missing (unset).
      *prop_str = 0;
      property_get(QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP2, prop_str, "");
      res = ((strcmp(prop_str, "false") != 0) && (strcmp(prop_str, "0") != 0));
      QCRIL_LOG_DEBUG("property %s = \"%s\", %d", QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP2,
                      prop_str, res);
      break;

    case QMI_RIL_FEATURE_PLAIN_ANDROID:
#if defined(QMI_RIL_UNDER_PLAIN_ANDROID)
      res = TRUE;
#else
      res = FALSE;
#endif
      break;

    case QMI_RIL_FEATURE_OEM_IND_TO_BOTH:
      // feature is enabled (true) by default, eg. if property set to anything
      // other than "false" or "0", including if its missing (unset).
      if (QMI_RIL_FEATURE_UNKNOWN == qmi_ril_oem_ind_to_both) {
        *prop_str = 0;
        property_get(QMI_RIL_SYS_PROP_NAME_OEM_IND_TO_BOTH, prop_str, "");
        if ((strcmp(prop_str, "false") != 0) && (strcmp(prop_str, "0") != 0)) {
          qmi_ril_oem_ind_to_both = QMI_RIL_FEATURE_ENABLED;
        } else {
          qmi_ril_oem_ind_to_both = QMI_RIL_FEATURE_DISABLED;
        }
      }

      if (QMI_RIL_FEATURE_ENABLED == qmi_ril_oem_ind_to_both) {
        res = TRUE;
      } else {
        res = FALSE;
      }
      break;

    case QMI_RIL_FEATURE_OEM_SOCKET:
      // feature is enabled (true) by default, eg. if property set to anything
      // other than "false" or "0", including if its missing (unset).
      *prop_str = 0;
      property_get(QMI_RIL_SYS_PROP_NAME_OEM_SOCKET, prop_str, "");
      res = ((strcmp(prop_str, "false") != 0) && (strcmp(prop_str, "0") != 0));
      break;

    case QMI_RIL_FEATURE_POSIX_CLOCKS:
      res = TRUE;
      break;

    case QMI_RIL_FEATURE_SAP_SILENT_PIN_VERIFY:
      /* feature is disabled by default, eg. if property set to anything
         other than "true" or "1", including if its missing (unset). */
      *prop_str = 0;
      property_get(QCRIL_FEATURE_SAP_SILENT_PIN_VERIFY, prop_str, "");
      res = ((strcmp(prop_str, "true") == 0) || (strcmp(prop_str, "1") == 0));
      break;

    case QMI_RIL_FEATURE_BLOCK_ALLOW_DATA:
      *prop_str = 0;
      property_get(QCRIL_PROCESS_BLOCK_ALLOW_DATA, prop_str, "");
      if ((strcmp(prop_str, "true") == 0) || (strcmp(prop_str, "1") == 0)) {
        res = TRUE;
      } else {
        res = FALSE;
      }
      break;

    case QMI_RIL_FEATURE_SUPPRESS_REQ:
      /* feature is disabled by default, eg. if property set to anything
         other than "true" or "1", including if its missing (unset). */
      if (QMI_RIL_FEATURE_UNKNOWN == qmi_ril_ftr_suppress_req) {
        *prop_str = 0;
        property_get(QMI_RIL_SYS_PROP_NAME_SUPPRESS_REQ, prop_str, "");
        if ((strcmp(prop_str, "true") == 0) || (strcmp(prop_str, "1") == 0)) {
          qmi_ril_ftr_suppress_req = QMI_RIL_FEATURE_ENABLED;
        } else {
          qmi_ril_ftr_suppress_req = QMI_RIL_FEATURE_DISABLED;
        }
      }

      switch (qmi_ril_ftr_suppress_req) {
        case QMI_RIL_FEATURE_ENABLED:
          res = TRUE;
          break;

        default:
          res = FALSE;
          break;
      }
      break;

    case QMI_RIL_FEATURE_RIL_DATA_REG_STATE_CONDITIONAL_REPORT:

      /* RIL to report DATA_REG_STATE not only looking for
      preffered technology reported through DSD. It should
      look for RAT on which IMS registerred*/
      *prop_str = 0;
      property_get(QMI_RIL_SYS_PROP_RIL_DATA_REG_STATE_CONDITIONAL_REPORT, prop_str, "");
      res = ((strcmp(prop_str, "true") == 0) || (strcmp(prop_str, "1") == 0));
      break;

    default:
      res = FALSE;
      break;
  }

  return res;
}  // qmi_ril_is_feature_supported

//===========================================================================
// qmi_ril_set_thread_name
//===========================================================================
void qmi_ril_set_thread_name(pthread_t thread_id, const char* thread_name) {
  pthread_setname_np(thread_id, thread_name);
  int iter_i = 0;

  for (iter_i = 0; iter_i < QMI_RIL_THREAD_INFO_MAX_SIZE; iter_i++) {
    if (FALSE == qmi_ril_thread_name_info[iter_i].is_valid) {
      qmi_ril_thread_name_info[iter_i].is_valid = TRUE;
      qmi_ril_thread_name_info[iter_i].thread_id = thread_id;
      strlcpy(qmi_ril_thread_name_info[iter_i].thread_name, thread_name,
              QMI_RIL_THREAD_NAME_MAX_SIZE);
      break;
    }
  }

}  // qmi_ril_set_thread_name

//===========================================================================
// qmi_ril_get_thread_name
//===========================================================================
int qmi_ril_get_thread_name(pthread_t thread_id, char* thread_name) {
#if !defined(__ANDROID__) && !defined(RIL_FOR_OPENWRT)
  pthread_getname_np(thread_id, thread_name, QMI_RIL_THREAD_NAME_MAX_SIZE);
  int res = TRUE;
#else
  int iter_i = 0, res = FALSE;

  for (iter_i = 0; iter_i < QMI_RIL_THREAD_INFO_MAX_SIZE; iter_i++) {
    if (TRUE == qmi_ril_thread_name_info[iter_i].is_valid &&
        thread_id == qmi_ril_thread_name_info[iter_i].thread_id) {
      strlcpy(thread_name, qmi_ril_thread_name_info[iter_i].thread_name,
              QMI_RIL_THREAD_NAME_MAX_SIZE);
      res = TRUE;
      break;
    }
  }
#endif

  return res;
}  // qmi_ril_get_thread_name

//===========================================================================
// qmi_ril_clear_thread_name
//===========================================================================
void qmi_ril_clear_thread_name(pthread_t thread_id) {
  int iter_i = 0;

  for (iter_i = 0; iter_i < QMI_RIL_THREAD_INFO_MAX_SIZE; iter_i++) {
    if (TRUE == qmi_ril_thread_name_info[iter_i].is_valid &&
        thread_id == qmi_ril_thread_name_info[iter_i].thread_id) {
      qmi_ril_thread_name_info[iter_i].is_valid = FALSE;
      break;
    }
  }
}  // qmi_ril_clear_thread_name

//===========================================================================
// qmi_ril_get_property_value_helper
//===========================================================================
void qmi_ril_get_property_value_helper(const char* property_name, char* property_value,
                                       const char* default_property_value) {
  int ret;
  if (property_name && property_value && default_property_value) {
    ret = property_get(property_name, property_value, default_property_value);
    QCRIL_LOG_DEBUG("ret value %d", ret);
  } else {
    QCRIL_LOG_ERROR("invalid property name/value/default value");
  }
}  // qmi_ril_get_property_value_helper

//===========================================================================
// qmi_ril_set_property_value_helper
//===========================================================================
RIL_Errno qmi_ril_set_property_value_helper(const char* property_name, const char* property_value) {
  RIL_Errno res;
  int ret;

  res = RIL_E_GENERIC_FAILURE;

  if (property_name && property_value) {
    if (strlen(property_value) <= PROPERTY_VALUE_MAX) {
      ret = property_set(property_name, property_value);

      if (ret != 0) {
        QCRIL_LOG_ERROR("failed to set %s to %s", property_name, property_value);
      } else {
        res = RIL_E_SUCCESS;
        QCRIL_LOG_INFO("%s set to %s successfully", property_name, property_value);
      }
    } else {
      QCRIL_LOG_ERROR("property value can not have more than %d characters", PROPERTY_VALUE_MAX);
    }
  } else {
    QCRIL_LOG_ERROR("invalid property name/value");
  }

  QCRIL_LOG_FUNC_RETURN("res: %d", (int)res);

  return res;
}  // qmi_ril_set_property_value_helper

//===========================================================================
// qmi_ril_get_property_value_from_string
//===========================================================================
void qmi_ril_get_property_value_from_string(const char* property_name, char* property_value,
                                            const char* default_property_value) {
  if (property_name && property_value) {
    qmi_ril_get_property_value_helper(property_name, property_value, default_property_value);

    QCRIL_LOG_INFO("retrieved %s from %s", property_value, property_name);
  } else {
    QCRIL_LOG_ERROR("invalid property name/value");
  }
}  // qmi_ril_get_property_value_from_string

//===========================================================================
// qmi_ril_get_property_value_from_integer
//===========================================================================
void qmi_ril_get_property_value_from_integer(const char* property_name, int* property_value,
                                             int default_property_value) {
  unsigned long res;
  char* end_ptr;
  char read_value[PROPERTY_VALUE_MAX];
  char temp_default_property_value[PROPERTY_VALUE_MAX];

  res = QMI_RIL_ZERO;
  end_ptr = NULL;
  memset(read_value, QMI_RIL_ZERO, sizeof(read_value));
  memset(temp_default_property_value, QMI_RIL_ZERO, sizeof(temp_default_property_value));

  if (property_name && property_value) {
    QCRIL_SNPRINTF(temp_default_property_value, sizeof(temp_default_property_value), "%d",
                   default_property_value);

    qmi_ril_get_property_value_helper(property_name, read_value, temp_default_property_value);

    if ((strlen(read_value) > 0)) {
      res = strtoul(read_value, &end_ptr, QMI_RIL_ZERO);

      if (((errno == ERANGE) && (res == ULONG_MAX)) || *end_ptr) {
        QCRIL_LOG_ERROR("failed to convert %s, read value %s", property_name, read_value);
        *(property_value) = QMI_RIL_ZERO;
      } else {
        *(property_value) = (int)res;
        QCRIL_LOG_INFO("retrieved %d from %s", *(property_value), property_name);
      }
    }
  } else {
    QCRIL_LOG_ERROR("invalid property name/value/default value");
  }
}  // qmi_ril_get_property_value_from_integer

//===========================================================================
// qmi_ril_get_property_value_from_boolean
//===========================================================================
void qmi_ril_get_property_value_from_boolean(const char* property_name, boolean* property_value,
                                             boolean default_property_value) {
  unsigned long res;
  char* end_ptr;
  char read_value[PROPERTY_VALUE_MAX];
  char temp_default_property_value[PROPERTY_VALUE_MAX];

#define QCRIL_TRUE_STR "true"

  res = QMI_RIL_ZERO;
  end_ptr = NULL;
  memset(read_value, QMI_RIL_ZERO, sizeof(read_value));
  memset(temp_default_property_value, QMI_RIL_ZERO, sizeof(temp_default_property_value));

  if (property_name && property_value) {
    QCRIL_SNPRINTF(temp_default_property_value, sizeof(temp_default_property_value), "%d",
                   !!default_property_value);

    qmi_ril_get_property_value_helper(property_name, read_value, temp_default_property_value);

    res = strtoul(read_value, &end_ptr, QMI_RIL_ZERO);

    if (end_ptr == read_value) {
      if (!strncmp(read_value, QCRIL_TRUE_STR, strlen(QCRIL_TRUE_STR))) {
        *(property_value) = TRUE;
      } else {
        *(property_value) = FALSE;
      }
    } else if (((errno == ERANGE) && (res == ULONG_MAX)) || *end_ptr) {
      QCRIL_LOG_ERROR("failed to convert %s, read value %s", property_name, read_value);
      *(property_value) = !!default_property_value;
    } else {
      *(property_value) = (boolean) !!res;
      QCRIL_LOG_INFO("retrieved %d from %s(%s)", *(property_value), property_name, read_value);
    }
  } else {
    QCRIL_LOG_ERROR("invalid property name/value/default value");
  }
}  // qmi_ril_get_property_value_from_boolean

//===========================================================================
// qmi_ril_set_property_value_to_string
//===========================================================================
RIL_Errno qmi_ril_set_property_value_to_string(const char* property_name,
                                               const char* property_value) {
  RIL_Errno res;

  res = RIL_E_GENERIC_FAILURE;

  if (property_name && property_value) {
    res = qmi_ril_set_property_value_helper(property_name, property_value);

  } else {
    QCRIL_LOG_ERROR("invalid property name/value");
  }

  return res;
}  // qmi_ril_set_property_value_to_string

//===========================================================================
// qmi_ril_set_property_value_to_integer
//===========================================================================
RIL_Errno qmi_ril_set_property_value_to_integer(const char* property_name, int property_value) {
  RIL_Errno res;
  char write_value[PROPERTY_VALUE_MAX];

  res = RIL_E_GENERIC_FAILURE;
  memset(write_value, QMI_RIL_ZERO, sizeof(write_value));

  if (property_name) {
    QCRIL_SNPRINTF(write_value, sizeof(write_value), "%d", (int)property_value);

    res = qmi_ril_set_property_value_helper(property_name, write_value);

  } else {
    QCRIL_LOG_ERROR("invalid property name");
  }

  return res;
}  // qmi_ril_set_property_value_to_integer

//===========================================================================
// qmi_ril_get_process_instance_id
//===========================================================================
qcril_instance_id_e_type qmi_ril_get_process_instance_id(void) {
  return qmi_ril_process_instance_id;  // this is always thread safe as may only be changed upon ril
                                       // library load
}  // qmi_ril_get_process_instance_id

void qmi_ril_set_process_instance_id(qcril_instance_id_e_type instance_id) {
  qmi_ril_process_instance_id = instance_id;
}

/*=========================================================================
  FUNCTION:  qcril_qmi_client_map_qmi_err_to_ril_err

===========================================================================*/
/*!
    @brief
    Map QMI error to RIL error.

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_client_map_qmi_err_to_ril_err(qmi_error_type_v01 qmi_err) {
  RIL_Errno ril_err;

  switch (qmi_err) {
    case QMI_ERR_NONE_V01:  // fallthrough
    case QMI_ERR_NO_EFFECT_V01:
      ril_err = RIL_E_SUCCESS;
      break;

    case QMI_ERR_FDN_RESTRICT_V01:
      ril_err = RIL_E_FDN_CHECK_FAILURE;
      break;

    case QMI_ERR_MALFORMED_MSG_V01:
      ril_err = RIL_E_INTERNAL_ERR;
      break;

    case QMI_ERR_INTERNAL_V01:
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_DEVICE_NOT_READY_V01:
      ril_err = RIL_E_INVALID_MODEM_STATE;
      break;

    case QMI_ERR_NO_MEMORY_V01:
      ril_err = RIL_E_NO_MEMORY;
      break;

    case QMI_ERR_ARG_TOO_LONG_V01:  // fallthrough
    case QMI_ERR_MISSING_ARG_V01:   // fallthrough
    case QMI_ERR_INVALID_ARG_V01:
      ril_err = RIL_E_INVALID_ARGUMENTS;
      break;

    case QMI_ERR_ENCODING_V01:
      ril_err = RIL_E_ENCODING_ERR;
      break;

    case QMI_ERR_SMSC_ADDR_V01:
      ril_err = RIL_E_INVALID_SMSC_ADDRESS;
      break;

    case QMI_ERR_DEVICE_STORAGE_FULL_V01:
      ril_err = RIL_E_NO_RESOURCES;
      break;

    case QMI_ERR_OP_DEVICE_UNSUPPORTED_V01:
      ril_err = RIL_E_REQUEST_NOT_SUPPORTED;
      break;

    case QMI_ERR_NETWORK_NOT_READY_V01:
      ril_err = RIL_E_NETWORK_NOT_READY;
      break;

    case QMI_ERR_NOT_PROVISIONED_V01:
      ril_err = RIL_E_NOT_PROVISIONED;
      break;

    case QMI_ERR_INVALID_OPERATION_V01:
    case QMI_ERR_INVALID_TRANSITION_V01:
    case QMI_ERR_OP_NETWORK_UNSUPPORTED_V01:
      ril_err = RIL_E_OPERATION_NOT_ALLOWED;
      break;

    case QMI_ERR_NO_ENTRY_V01:
      ril_err = RIL_E_NO_SUCH_ENTRY;
      break;

    case QMI_ERR_ACK_NOT_SENT_V01:
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_CARD_CALL_CONTROL_FAILED_V01:
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_NO_SUBSCRIPTION_V01:
      ril_err = RIL_E_NO_SUBSCRIPTION;
      break;

    case QMI_ERR_NO_NETWORK_FOUND_V01:
      ril_err = RIL_E_NO_NETWORK_FOUND;
      break;

    case QMI_ERR_DEVICE_IN_USE_V01:
      ril_err = RIL_E_DEVICE_IN_USE;
      break;

    case QMI_ERR_CALL_FAILED_V01:
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_INCOMPATIBLE_STATE_V01:
      ril_err = RIL_E_INVALID_MODEM_STATE;
      break;

    case QMI_ERR_ABORTED_V01:
      ril_err = RIL_E_CANCELLED;
      break;

    case QMI_ERR_NO_RADIO_V01:
      /* Nas and voice qmi documents have different perception
         for this error QMI_ERR_NO_RADIO. Hence using MODEM_ERR
         as a common ground */
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_NOT_SUPPORTED_V01:
      /* This error is sent by the modem when the Request is
         "currently" not supported. Using MODEM_ERR as this
          pertains to modem not being able to perform the request
          currently
          */
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_NETWORK_ABORTED_V01:
      ril_err = RIL_E_ABORTED;
      break;

    case QMI_ERR_INFO_UNAVAILABLE_V01:
      ril_err = RIL_E_NOT_PROVISIONED;
      break;

    case QMI_ERR_HARDWARE_RESTRICTED_V01:
      ril_err = RIL_E_SYSTEM_ERR;
      break;

    default:
      ril_err = RIL_E_MODEM_ERR;
      break;
  }
  return ril_err;
} /* qcril_qmi_client_map_qmi_err_to_ril_err */

//============================================================================
// FUNCTION: qmi_ril_retrieve_number_of_rilds
//
// DESCRIPTION:
// Returns the number of rilds supported on a target that supports mutiple rild scenario
//
// RETURN: number of rilds supported on a target that supports mutiple rild scenario
//============================================================================
int qmi_ril_retrieve_number_of_rilds() {
  int num_of_rilds = 1;
  if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_DSDS)) {
    num_of_rilds = 2;
  } else if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_TSTS)) {
    num_of_rilds = 3;
  }

  return num_of_rilds;
}  // qmi_ril_retrieve_number_of_rilds

/*===========================================================================

  FUNCTION:  qcril_log_ril_radio_state_to_str

===========================================================================*/
/*!
    @brief
    Convert Radio State to string

    @return
    Radio state in readable string format
*/
/*=========================================================================*/
const char* qcril_log_ril_radio_state_to_str(RIL_RadioState radio_state) {
  switch (radio_state) {
    case RADIO_STATE_OFF:
      return "Radio Off";
    case RADIO_STATE_UNAVAILABLE:
      return "Radio Unavailable";
    case RADIO_STATE_ON:
      return "Radio On";
    default:
      break;
  }
  return "<unspecified>";
}

//===========================================================================
// qmi_ril_is_multi_sim_feature_supported
//===========================================================================
int qmi_ril_is_multi_sim_feature_supported() {
  return (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_DSDS) ||
          qmi_ril_is_feature_supported(QMI_RIL_FEATURE_TSTS));
}  // qmi_ril_is_multi_sim_feature_supported

//=============================================================================
// FUNCTION: qmi_ril_get_sim_slot
//
// DESCRIPTION:
// returns the sim card slot index associated with current RIL instance
// Note: thread safe as long as its only set durring ril init
//
// RETURN: 0 | 1 - sim card slot index
//=============================================================================
uint32_t qmi_ril_get_sim_slot(void) {
  return qmi_ril_sim_slot;
}

void qmi_ril_set_sim_slot(uint32_t slot) {
  qmi_ril_sim_slot = slot;
}

void qmi_ril_reset_baseband_rat_option() {
  qmi_ril_rat_enable_option = QMI_RIL_FTR_RAT_UNKNOWN;
  qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_UNKNOWN;
}
