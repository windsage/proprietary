/**
* Copyright (c) 2020, 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "qcril_data.h"
#include <sys/socket.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include "DataModuleMutex.h"
#include "framework/Dispatcher.h"
#include "framework/legacy.h"
#include "UnSolMessages/SetPrefDataTechMessage.h"
#include "UnSolMessages/DsdSysStatusMessage.h"
#include "UnSolMessages/DataRegistrationStateMessage.h"
#include "UnSolMessages/DataSysStatusMessage.h"
#include "UnSolMessages/CallStatusMessage.h"
#include "UnSolMessages/NewDDSInfoMessage.h"
#include "UnSolMessages/DDSStatusFollowupMessage.h"

#ifndef RIL_FOR_LOW_RAM
#include "UnSolMessages/DataAllBearerTypeChangedMessage.h"
#else
#include "MessageCommon.h"
#endif
#include "UnSolMessages/LinkCapIndMessage.h"
#include "UnSolMessages/DataBearerTypeChangedMessage.h"
#define TAG "DataModule"

using namespace rildata;

qcril_data_ce_map qcril_data_ce_map_tbl[] =
{
/*{RIL code,                           QMI code,                           DSI type}*/
  /*3GPP defined error codes*/
  {PDP_FAIL_OPERATOR_BARRED,                          WDS_VCER_3GPP_OPERATOR_DETERMINED_BARRING_V01,               DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_NAS_SIGNALLING,                           WDS_VCER_3GPP_NAS_SIGNALLING_ERROR_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_LLC_SNDCP,                                WDS_VCER_3GPP_LLC_SNDCP_FAILURE_V01,                         DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_INSUFFICIENT_RESOURCES,                   WDS_VCER_3GPP_INSUFFICIENT_RESOURCES_V01,                    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MISSING_UKNOWN_APN,                       WDS_VCER_3GPP_UNKNOWN_APN_V01,                               DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE,                 WDS_VCER_3GPP_UNKNOWN_PDP_V01,                               DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_USER_AUTHENTICATION,                      WDS_VCER_3GPP_AUTH_FAILED_V01,                               DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ACTIVATION_REJECT_GGSN,                   WDS_VCER_3GPP_GGSN_REJECT_V01,                               DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ACTIVATION_REJECT_UNSPECIFIED,            WDS_VCER_3GPP_ACTIVATION_REJECT_V01,                         DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED,             WDS_VCER_3GPP_OPTION_NOT_SUPPORTED_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED,            WDS_VCER_3GPP_OPTION_UNSUBSCRIBED_V01,                       DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_SERVICE_OPTION_OUT_OF_ORDER,              WDS_VCER_3GPP_OPTION_TEMP_OOO_V01,                           DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_NSAPI_IN_USE,                             WDS_VCER_3GPP_NSAPI_ALREADY_USED_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_REGULAR_DEACTIVATION,                     WDS_VCER_3GPP_REGULAR_DEACTIVATION_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_QOS_NOT_ACCEPTED,                         WDS_VCER_3GPP_QOS_NOT_ACCEPTED_V01,                          DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_NETWORK_FAILURE,                          WDS_VCER_3GPP_NETWORK_FAILURE_V01,                           DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_UMTS_REACTIVATION_REQ,                    WDS_VCER_3GPP_UMTS_REACTIVATION_REQ_V01,                     DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_FEATURE_NOT_SUPP,                         WDS_VCER_3GPP_FEATURE_NOT_SUPP_V01,                          DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_TFT_SEMANTIC_ERROR,                       WDS_VCER_3GPP_TFT_SEMANTIC_ERROR_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_TFT_SYTAX_ERROR,                          WDS_VCER_3GPP_TFT_SYTAX_ERROR_V01,                           DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_UNKNOWN_PDP_CONTEXT,                      WDS_VCER_3GPP_UNKNOWN_PDP_CONTEXT_V01,                       DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_FILTER_SEMANTIC_ERROR,                    WDS_VCER_3GPP_FILTER_SEMANTIC_ERROR_V01,                     DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_FILTER_SYTAX_ERROR,                       WDS_VCER_3GPP_FILTER_SYTAX_ERROR_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_PDP_WITHOUT_ACTIVE_TFT,                   WDS_VCER_3GPP_PDP_WITHOUT_ACTIVE_TFT_V01,                    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ONLY_IPV4_ALLOWED,                        WDS_VCER_3GPP_IP_V4_ONLY_ALLOWED_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ONLY_IPV6_ALLOWED,                        WDS_VCER_3GPP_IP_V6_ONLY_ALLOWED_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED,               WDS_VCER_3GPP_SINGLE_ADDR_BEARER_ONLY_V01,                   DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ESM_INFO_NOT_RECEIVED,                    WDS_VCER_3GPP_ESM_INFO_NOT_RECEIVED_V01,                     DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_PDN_CONN_DOES_NOT_EXIST,                  WDS_VCER_3GPP_PDN_CONN_DOES_NOT_EXIST_V01,                   DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED,       WDS_VCER_3GPP_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED_V01,        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MAX_ACTIVE_PDP_CONTEXT_REACHED,           WDS_VCER_3GPP_MAX_ACTIVE_PDP_CONTEXT_REACHED_V01,            DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_UNSUPPORTED_APN_IN_CURRENT_PLMN,          WDS_VCER_3GPP_UNSUPPORTED_APN_IN_CURRENT_PLMN_V01,           DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_INVALID_TRANSACTION_ID,                   WDS_VCER_3GPP_INVALID_TRANSACTION_ID_V01,                    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MESSAGE_INCORRECT_SEMANTIC,               WDS_VCER_3GPP_MESSAGE_INCORRECT_SEMANTIC_V01,                DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_INVALID_MANDATORY_INFO,                   WDS_VCER_3GPP_INVALID_MANDATORY_INFO_V01,                    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MESSAGE_TYPE_UNSUPPORTED,                 WDS_VCER_3GPP_MESSAGE_TYPE_UNSUPPORTED_V01,                  DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MSG_TYPE_NONCOMPATIBLE_STATE,             WDS_VCER_3GPP_MSG_TYPE_NONCOMPATIBLE_STATE_V01,              DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_UNKNOWN_INFO_ELEMENT,                     WDS_VCER_3GPP_UNKNOWN_INFO_ELEMENT_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_CONDITIONAL_IE_ERROR,                     WDS_VCER_3GPP_CONDITIONAL_IE_ERROR_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE,      WDS_VCER_3GPP_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE_V01,       DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_PROTOCOL_ERRORS,                          WDS_VCER_3GPP_PROTOCOL_ERROR_V01,                            DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_APN_TYPE_CONFLICT,                        WDS_VCER_3GPP_APN_TYPE_CONFLICT_V01,                         DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_INVALID_PCSCF_ADDR,                       WDS_VCER_3GPP_INVALID_PCSCF_ADDR_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN,   WDS_VCER_3GPP_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN_V01,    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_EMM_ACCESS_BARRED,                        WDS_VCER_3GPP_EMM_ACCESS_BARRED_V01,                         DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_EMERGENCY_IFACE_ONLY,                     WDS_VCER_3GPP_EMERGENCY_IFACE_ONLY_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_IFACE_MISMATCH,                           WDS_VCER_3GPP_IFACE_MISMATCH_V01,                            DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_COMPANION_IFACE_IN_USE,                   WDS_VCER_3GPP_COMPANION_IFACE_IN_USE_V01,                    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_IP_ADDRESS_MISMATCH,                      WDS_VCER_3GPP_IP_ADDRESS_MISMATCH_V01,                       DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_IFACE_AND_POL_FAMILY_MISMATCH,            WDS_VCER_3GPP_IFACE_AND_POL_FAMILY_MISMATCH_V01,             DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_EMM_ACCESS_BARRED_INFINITE_RETRY,         WDS_VCER_3GPP_EMM_ACCESS_BARRED_INFINITE_RETRY_V01,          DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_AUTH_FAILURE_ON_EMERGENCY_CALL,           WDS_VCER_3GPP_AUTH_FAILURE_ON_EMERGENCY_CALL_V01,            DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  /*CM call end codes*/
  {PDP_FAIL_SIGNAL_LOST,                  WDS_VCER_CM_NO_SRV_V01,          DSI_CE_TYPE_CALL_MANAGER_DEFINED},
  {PDP_FAIL_SIGNAL_LOST,                  WDS_VCER_CM_FADE_V01,            DSI_CE_TYPE_CALL_MANAGER_DEFINED},
  {PDP_FAIL_PREF_RADIO_TECH_CHANGED,      WDS_VCER_CM_UE_RAT_CHANGE_V01,   DSI_CE_TYPE_CALL_MANAGER_DEFINED},
  {PDP_FAIL_PROTOCOL_ERRORS,              WDS_VCER_IPV6_ERR_HRPD_IPV6_DISABLED_V01, DSI_CE_TYPE_IPV6},
  {PDP_FAIL_USER_AUTHENTICATION,          WDS_VCER_PPP_CHAP_FAILURE_V01,            DSI_CE_TYPE_PPP},
  {PDP_FAIL_USER_AUTHENTICATION,          WDS_VCER_PPP_PAP_FAILURE_V01,             DSI_CE_TYPE_PPP},
  {PDP_FAIL_ONLY_IPV4_ALLOWED,            WDS_VCER_INTERNAL_ERR_PDN_IPV6_CALL_DISALLOWED_V01, DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_ONLY_IPV6_ALLOWED,            WDS_VCER_INTERNAL_ERR_PDN_IPV4_CALL_DISALLOWED_V01, DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_MISSING_UKNOWN_APN,           WDS_VCER_INTERNAL_APN_DISABLED_V01,                 DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_ONLY_IPV4_ALLOWED,            WDS_VCER_IPV6_DISABLED_V01,                         DSI_CE_TYPE_IPV6},
  {PDP_FAIL_INSUFFICIENT_RESOURCES,       WDS_VCER_INTERNAL_MAX_V4_CONNECTIONS_V01,           DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_INSUFFICIENT_RESOURCES,       WDS_VCER_INTERNAL_MAX_V6_CONNECTIONS_V01,           DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE,     WDS_VCER_INTERNAL_IP_VERSION_MISMATCH_V01,          DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED, WDS_VCER_INTERNAL_DUN_CALL_DISALLOWED_V01,          DSI_CE_TYPE_INTERNAL},
#if (QCRIL_RIL_VERSION >= 6)
  {PDP_FAIL_TETHERED_CALL_ACTIVE,      WDS_VCER_INTERNAL_APP_PREEMPTED_V01,   DSI_CE_TYPE_INTERNAL}
#else
  {PDP_FAIL_TETHERED_CALL_ON,          WDS_VCER_INTERNAL_APP_PREEMPTED_V01,   DSI_CE_TYPE_INTERNAL}
#endif /* (QCRIL_RIL_VERSION >= 6) */
};

/*=========================================================================
  FUNCTION:  qcril_data_set_pref_data_tech

===========================================================================*/
/*!
    @brief
    Set the preferred data technology used for data call setup.

    @return
    None
*/
/*=========================================================================*/
void qcril_data_set_pref_data_tech
(
  int instance_id,
  qcril_data_pref_data_tech_e_type pref_data_tech
)
{
   auto msg = std::make_shared<rildata::SetPrefDataTechMessage>
            (static_cast<qcril_instance_id_e_type>(instance_id), pref_data_tech);
   msg->broadcast();
}

/*=========================================================================
  FUNCTION:  qcril_data_set_dsd_sys_status

===========================================================================*/
/*!
    @brief
    Set the preferred data technology used for data call setup. This is
    called upon receipt of
    QMI_DSD_REPORT_SYSTEM_STATUS_IND_V01/QMI_DSD_GET_SYSTEM_STATUS_RESP_V01.

    @return
    None
*/
/*=========================================================================*/
void qcril_data_set_dsd_sys_status
(
  dsd_system_status_ind_msg_v01 *dsd_system_status
)
{
  if (dsd_system_status != nullptr && dsd_system_status->avail_sys_len >= 1)
  {
    //For Non-NR RAT, DsdSysStatusMessage will be used.
    if (dsd_system_status->avail_sys[0].rat_value != DSD_SYS_RAT_EX_3GPP_5G_V01)
    {
      auto msg = std::make_shared<rildata::DsdSysStatusMessage>
          (global_instance_id, *dsd_system_status);
      msg->broadcast();
    }
    else //For NR RAT, DataRegistrationStateMessage will be used.
    {
      //For NSA Mode
      if (dsd_system_status->avail_sys[0].so_mask & QMI_DSD_3GPP_SO_MASK_5G_NSA_V01)
      {
        if (dsd_system_status->avail_sys[0].so_mask & QMI_DSD_3GPP_SO_MASK_5G_MMWAVE_V01)
        {
          auto msg = std::make_shared<rildata::DataRegistrationStateMessage>
              (global_instance_id, NR_NSA_MMWV);
          msg->broadcast();
        }
        //NR_NSA is the default value for NSA Mode,
        //even if mask QMI_DSD_3GPP_SO_MASK_5G_SUB6_V01 is not set.
        else
        {
          auto msg = std::make_shared<rildata::DataRegistrationStateMessage>
              (global_instance_id, NR_NSA);
          msg->broadcast();
        }
      }
      //For SA Mode
      else if (dsd_system_status->avail_sys[0].so_mask & QMI_DSD_3GPP_SO_MASK_5G_SA_V01)
      {
        if (dsd_system_status->avail_sys[0].so_mask & QMI_DSD_3GPP_SO_MASK_5G_MMWAVE_V01)
        {
          auto msg = std::make_shared<rildata::DataRegistrationStateMessage>
              (global_instance_id, NR_SA_MMWV);
          msg->broadcast();
        }
        //NR_SA is the default value for SA Mode,
        //Even if mask QMI_DSD_3GPP_SO_MASK_5G_SUB6_V01 is not set.
        else
        {
          auto msg = std::make_shared<rildata::DataRegistrationStateMessage>
              (global_instance_id, NR_SA);
          msg->broadcast();
        }
      }
      else
      {
        QCRIL_LOG_ERROR("Invalid so_mask for 5G RAT %llu. so_mask should contain either"
                        "5G_NSA or 5G_SA", dsd_system_status->avail_sys[0].so_mask);
      }
    }
  }
  else
  {
    QCRIL_LOG_ERROR("dsd_system_status is NULL or no available system.");
  }
}

/*=========================================================================
  FUNCTION:  qcril_data_set_data_sys_status

===========================================================================*/
/*!
    @brief
    Set the preferred data technology used for data call setup. This is
    newer version of qcril_arb_set_pref_data_tech, we pass over all info
    returned from modem to upper layer

    @return
    None
*/
/*=========================================================================*/
void qcril_data_set_data_sys_status
(
  qcril_instance_id_e_type          instance_id,
  wds_data_system_status_type_v01 * data_sys_status
)
{
  if (data_sys_status != nullptr)
  {
    auto msg = std::make_shared<rildata::DataSysStatusMessage>
                           (instance_id, *data_sys_status);
    msg->broadcast();
  }
}

/*===========================================================================

  FUNCTION:  qcril_data_dds_status_followup

===========================================================================*/
/*!
    @brief
    Notify DDS switch followup status

    @return
    None
*/
/*=========================================================================*/
void qcril_data_dds_status_followup
(
  DDSStatusInfo * status
)
{
  if (status != nullptr)
  {
    auto msg = std::make_shared<rildata::DDSStatusFollowupMessage>
                                         (global_instance_id, *status);
    msg->broadcast();
  }
}

/*===========================================================================

  FUNCTION:  qcril_data_bearer_type_info_ind_update

===========================================================================*/
/*!
    @brief
    Update a bearer type info using qmi indication

    @return
    None
*/
/*=========================================================================*/
void qcril_data_bearer_type_info_ind_update
(
  int32_t cid,
  wds_data_bearer_type_info_v01 *bearer_type_info
)
{
  // Adding one bearer with BearerInfo_t
  BearerInfo_t b;
  b.bearerId = (int32_t)bearer_type_info->bearer_id;
  b.uplink = (RatType_t)bearer_type_info->ul_direction;
  b.downlink = (RatType_t)bearer_type_info->dl_direction;

  auto msg = std::make_shared<DataBearerTypeChangedMessage>(cid, b);
  msg->broadcast();
}

int qcril_data_get_ril_ce_code
(
  dsi_ce_reason_t * dsi_ce_reason,
  int * ril_ce_code
)
{
  unsigned int i=0;
  unsigned int max=sizeof(qcril_data_ce_map_tbl)/sizeof(qcril_data_ce_map);
  int ret = DSI_ERROR;

  do
  {
    /* the only real error condition for this function */
    if (NULL == dsi_ce_reason ||
        NULL == ril_ce_code )
    {
      QCRIL_LOG_ERROR("**programming err** NULL param rcvd");
      break;
    }

    /* we must return some cause code. If we can't find any,
     * we return UNSPECIFIED, but this function must return success
     * from this point onwards */
    ret = DSI_SUCCESS;

    /* check to see if it matches with cause codes as defined in
     * qcril_data_ce_map_tbl */
    for(i=0; i<max; i++)
    {
      if (qcril_data_ce_map_tbl[i].qmi_data_ce_code == dsi_ce_reason->reason_code &&
          qcril_data_ce_map_tbl[i].reason_category == dsi_ce_reason->reason_type
        )
      {
        *ril_ce_code = qcril_data_ce_map_tbl[i].ril_data_ce_code;
        break;
      } else if ( ( dsi_ce_reason->reason_type == DSI_CE_TYPE_CALL_MANAGER_DEFINED ) && ( ( dsi_ce_reason->reason_code == WDS_VCER_CM_DETACH_EPS_SERVICES_NOT_ALLOWED_V01 ) || ( dsi_ce_reason->reason_code == WDS_VCER_CM_NEW_GPRS_SERV_NOT_ALLOWED_V01 ) ) ) {
        *ril_ce_code = PDP_FAIL_ACTIVATION_NOT_ALLOWED;
        break;
      }
    }
    /* break out if we found matching cause code */
    if (i < max)
    {
      break;
    }

    /* if no match found in qcril_data_ce_map_tbl, handle passthrough
     * and unsupported cases */
    switch( dsi_ce_reason->reason_type )
    {
      case DSI_CE_TYPE_3GPP_SPEC_DEFINED:
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_6;
        break;

      case DSI_CE_TYPE_INTERNAL:
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_4;
        break;

      case DSI_CE_TYPE_MOBILE_IP:
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_3 ;
        break;

      case DSI_CE_TYPE_CALL_MANAGER_DEFINED:
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_5;
        break;

      case DSI_CE_TYPE_PPP:
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_7;
        break;

      case DSI_CE_TYPE_EHRPD:
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_8;
        break;

      default:
        /* if we reach here, return UNSPECIFIED cause code */
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_4;
        Log::getInstance().d("unspecified/invalid call end reason rcvd");
    }
    Log::getInstance().d("Ril CE code :" +  std::to_string(*ril_ce_code));
  } while (0);

  return ret;
}

#ifndef RIL_FOR_LOW_RAM
/*===========================================================================

  FUNCTION:  qcril_data_bearer_type_info_response_update

===========================================================================*/
/*!
    @brief
    Update bearer type info using qmi response

    @return
    None
*/
/*=========================================================================*/
void qcril_data_bearer_type_info_response_update
(
  int32_t cid,
  char * apnname,
  char * ifacename,
  wds_get_data_bearer_type_resp_msg_v01 *resp
)
{
  AllocatedBearer_t bearerInfo;
  bearerInfo.cid = cid;
  bearerInfo.apn.assign(apnname);
  bearerInfo.ifaceName.assign(ifacename);
  // Adding bearers with BearerInfo_t
  if(resp->bearer_type_info_valid) {
    for(uint32_t i=0;i<resp->bearer_type_info_len;i++) {
      BearerInfo_t b;
      b.bearerId = (int32_t)resp->bearer_type_info[i].bearer_id;
      b.uplink = (RatType_t)resp->bearer_type_info[i].ul_direction;
      b.downlink = (RatType_t)resp->bearer_type_info[i].dl_direction;
      bearerInfo.bearers.push_back(b);
    }
    auto msg = std::make_shared<DataAllBearerTypeChangedMessage>(bearerInfo);
    msg->broadcast();
  }
}
#endif

int qcril_data_get_ifindex(const char * dev, int * ifindex)
{
  struct ifreq ifr;
  int fd;

  if(NULL == ifindex || NULL == dev)
  {
    Log::getInstance().d("invalid input");
    return DSI_ERROR;
  }

  *ifindex = -1;
#ifndef QMI_RIL_UTF
  /* Open a temporary socket of datagram type to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    Log::getInstance().d("Open datagram socket failed");
    return DSI_ERROR;
  }

  /* Set device name in the ioctl req struct */
  memset(&ifr, 0x0, sizeof(ifr));
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));

  /* Issue ioctl on the device */
  if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
    Log::getInstance().d("Issue ioctl on the device failed, errno:" + std::to_string(errno));
    close(fd);
    return DSI_ERROR;
  }
  *ifindex = ifr.ifr_ifindex;

  /* Close temporary socket */
  close(fd);
#endif

  return DSI_SUCCESS;
}
