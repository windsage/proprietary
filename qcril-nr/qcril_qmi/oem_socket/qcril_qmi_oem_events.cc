/******************************************************************************
#  Copyright (c) 2017,2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <map>
#include <framework/legacy.h>
#include <framework/Dispatcher.h>
#include <framework/qcril_event.h>
#include "qcril_qmi_oem_events.h"
#include "qcril_qmi_oem_reqlist.h"

#undef QCRIL_LEGACY_EVENT
#define QCRIL_LEGACY_EVENT QCRIL_DEFINE_LEGACY_EVENT
#include "qcril_qmi_oem_eventlist.h"

static std::map<int, qcril_evt_e_type> qcril_qmi_oem_hook_map;

void qcril_qmi_oem_events_init()
{
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_BASE]                                 = QCRIL_EVT_HOOK_BASE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_DATA_GO_DORMANT]                      = QCRIL_EVT_HOOK_DATA_GO_DORMANT;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_ME_DEPERSONALIZATION]                 = QCRIL_EVT_HOOK_ME_DEPERSONALIZATION;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_TUNE_AWAY]                        = QCRIL_EVT_HOOK_SET_TUNE_AWAY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_TUNE_AWAY]                        = QCRIL_EVT_HOOK_GET_TUNE_AWAY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_PAGING_PRIORITY]                  = QCRIL_EVT_HOOK_SET_PAGING_PRIORITY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_PAGING_PRIORITY]                  = QCRIL_EVT_HOOK_GET_PAGING_PRIORITY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_NEIGHBORING_CELLS_INFO]           = QCRIL_EVT_HOOK_GET_NEIGHBORING_CELLS_INFO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_CDMA_SUB_SRC_WITH_SPC]            = QCRIL_EVT_HOOK_SET_CDMA_SUB_SRC_WITH_SPC;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_DEFAULT_VOICE_SUB]                = QCRIL_EVT_HOOK_SET_DEFAULT_VOICE_SUB;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CDMA_AVOID_CUR_NWK]                   = QCRIL_EVT_HOOK_CDMA_AVOID_CUR_NWK;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CDMA_CLEAR_AVOIDANCE_LIST]            = QCRIL_EVT_HOOK_CDMA_CLEAR_AVOIDANCE_LIST;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CDMA_GET_AVOIDANCE_LIST]              = QCRIL_EVT_HOOK_CDMA_GET_AVOIDANCE_LIST;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_BUILTIN_PLMN_LIST]                = QCRIL_EVT_HOOK_SET_BUILTIN_PLMN_LIST;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_RAT_RAC]                          = QCRIL_EVT_HOOK_GET_RAT_RAC;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_ACQ_ORDER]      = QCRIL_EVT_HOOK_SET_PREFERRED_NETWORK_ACQ_ORDER;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_ACQ_ORDER]      = QCRIL_EVT_HOOK_GET_PREFERRED_NETWORK_ACQ_ORDER;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_MODEM_CAPABILITY]                 = QCRIL_EVT_HOOK_GET_MODEM_CAPABILITY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UPDATE_SUB_BINDING]                   = QCRIL_EVT_HOOK_UPDATE_SUB_BINDING;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_DATA_SUBSCRIPTION]                = QCRIL_EVT_HOOK_SET_DATA_SUBSCRIPTION;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_IS_DATA_ENABLED]                  = QCRIL_EVT_HOOK_SET_IS_DATA_ENABLED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_IS_DATA_ROAMING_ENABLED]          = QCRIL_EVT_HOOK_SET_IS_DATA_ROAMING_ENABLED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_APN_INFO]                         = QCRIL_EVT_HOOK_SET_APN_INFO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_SET_LTE_TUNE_AWAY]                    = QCRIL_EVT_HOOK_SET_LTE_TUNE_AWAY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_REQ_GENERIC]                          = QCRIL_EVT_HOOK_REQ_GENERIC;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_PROTOBUF_MSG]                         = QCRIL_EVT_HOOK_PROTOBUF_MSG;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_LTE_DIRECT_DISC_REQ]                  = QCRIL_EVT_HOOK_LTE_DIRECT_DISC_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_LTE_DIRECT_DISC]                = QCRIL_EVT_HOOK_UNSOL_LTE_DIRECT_DISC;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_CSG_GET_SYS_INFO]                     = QCRIL_EVT_HOOK_CSG_GET_SYS_INFO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_GET_DATA_CALL_PROFILE]                = QCRIL_EVT_HOOK_GET_DATA_CALL_PROFILE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CALL_EVT_PROGRESS_INFO_IND]     = QCRIL_EVT_HOOK_UNSOL_CALL_EVT_PROGRESS_INFO_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_NEIGHBOR_CELL_INFO_RCVD]              = QCRIL_EVT_HOOK_NEIGHBOR_CELL_INFO_RCVD;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_EUTRA_STATUS]                   = QCRIL_EVT_HOOK_UNSOL_EUTRA_STATUS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_LTE_COEX]                       = QCRIL_EVT_HOOK_UNSOL_LTE_COEX;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_WMS_READY]                      = QCRIL_EVT_HOOK_UNSOL_WMS_READY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_VOICE_SYSTEM_ID]                = QCRIL_EVT_HOOK_UNSOL_VOICE_SYSTEM_ID;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_ENGINEER_MODE]                  = QCRIL_EVT_HOOK_UNSOL_ENGINEER_MODE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PDC_CONFIG]                     = QCRIL_EVT_HOOK_UNSOL_PDC_CONFIG;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_AUDIO_STATE_CHANGED]            = QCRIL_EVT_HOOK_UNSOL_AUDIO_STATE_CHANGED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_SIM_REFRESH]                    = QCRIL_EVT_HOOK_UNSOL_SIM_REFRESH;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PDC_CLEAR_CONFIGS]              = QCRIL_EVT_HOOK_UNSOL_PDC_CLEAR_CONFIGS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_WWAN_AVAILABLE]                 = QCRIL_EVT_HOOK_UNSOL_WWAN_AVAILABLE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_MODEM_CAPABILITY]               = QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PDC_VALIDATE_CONFIGS]           = QCRIL_EVT_HOOK_UNSOL_PDC_VALIDATE_CONFIGS;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PDC_VALIDATE_DUMPED]            = QCRIL_EVT_HOOK_UNSOL_PDC_VALIDATE_DUMPED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PDC_LIST_CONFIG]                = QCRIL_EVT_HOOK_UNSOL_PDC_LIST_CONFIG;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CSG_SYS_INFO_IND]               = QCRIL_EVT_HOOK_UNSOL_CSG_SYS_INFO_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_CSG_NETWORK_REJ_IND]            = QCRIL_EVT_HOOK_UNSOL_CSG_NETWORK_REJ_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_RAT_RAC_CHANGE_IND]             = QCRIL_EVT_HOOK_UNSOL_RAT_RAC_CHANGE_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_PCSCF_ADDR]                     = QCRIL_EVT_HOOK_UNSOL_PCSCF_ADDR;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_OPERATOR_RESERVED_PCO]          = QCRIL_EVT_HOOK_UNSOL_OPERATOR_RESERVED_PCO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_GENERIC]                        = QCRIL_EVT_HOOK_UNSOL_GENERIC;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_UNSOL_OEM_SOCKET_CONNECTED]           = QCRIL_EVT_HOOK_UNSOL_OEM_SOCKET_CONNECTED;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_BASE]                              = QCRIL_EVT_HOOK_VT_BASE;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_DIAL_CALL]                         = QCRIL_EVT_HOOK_VT_DIAL_CALL;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_END_CALL]                          = QCRIL_EVT_HOOK_VT_END_CALL;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_ANSWER_CALL]                       = QCRIL_EVT_HOOK_VT_ANSWER_CALL;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_GET_CALL_INFO]                     = QCRIL_EVT_HOOK_VT_GET_CALL_INFO;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_VT_UNSOL_CALL_STATUS_IND]             = QCRIL_EVT_HOOK_VT_UNSOL_CALL_STATUS_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_ENABLER_STATE_REQ]                = QCRIL_EVT_HOOK_IMS_ENABLER_STATE_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_PUBLISH_REQ]                 = QCRIL_EVT_HOOK_IMS_SEND_PUBLISH_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_PUBLISH_XML_REQ]             = QCRIL_EVT_HOOK_IMS_SEND_PUBLISH_XML_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_UNPUBLISH_REQ]               = QCRIL_EVT_HOOK_IMS_SEND_UNPUBLISH_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_SUBSCRIBE_REQ]               = QCRIL_EVT_HOOK_IMS_SEND_SUBSCRIBE_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_SUBSCRIBE_XML_REQ]           = QCRIL_EVT_HOOK_IMS_SEND_SUBSCRIBE_XML_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SEND_UNSUBSCRIBE_REQ]             = QCRIL_EVT_HOOK_IMS_SEND_UNSUBSCRIBE_REQ;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SET_NOTIFY_FMT_REQ_V01]           = QCRIL_EVT_HOOK_IMS_SET_NOTIFY_FMT_REQ_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_GET_NOTIFY_FMT_REQ_V01]           = QCRIL_EVT_HOOK_IMS_GET_NOTIFY_FMT_REQ_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_SET_EVENT_REPORT_REQ_V01]         = QCRIL_EVT_HOOK_IMS_SET_EVENT_REPORT_REQ_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_GET_EVENT_REPORT_REQ_V01]         = QCRIL_EVT_HOOK_IMS_GET_EVENT_REPORT_REQ_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_PUBLISH_TRIGGER_IND_V01]          = QCRIL_EVT_HOOK_IMS_PUBLISH_TRIGGER_IND_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_NOTIFY_XML_IND_V01]               = QCRIL_EVT_HOOK_IMS_NOTIFY_XML_IND_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_NOTIFY_IND_V01]                   = QCRIL_EVT_HOOK_IMS_NOTIFY_IND_V01;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_IMS_ENABLER_STATUS_IND]               = QCRIL_EVT_HOOK_IMS_ENABLER_STATUS_IND;
    qcril_qmi_oem_hook_map[QCRIL_REQ_HOOK_MAX]                                  = QCRIL_EVT_HOOK_MAX;
}

qcril_evt_e_type qcril_qmi_oem_get_event(int oemhook_msg)
{
  std::map<int, qcril_evt_e_type>::iterator it;

  it = qcril_qmi_oem_hook_map.find(oemhook_msg);

  if (it != qcril_qmi_oem_hook_map.end())
      return it->second;
  else
      return QCRIL_EVT_HOOK_BASE;
}

int qcril_qmi_oem_get_oemhook_msg(qcril_evt_e_type event)
{
  std::map<int, qcril_evt_e_type>::iterator it;
  int request = QCRIL_REQ_HOOK_BASE;
  for (it=qcril_qmi_oem_hook_map.begin(); it!=qcril_qmi_oem_hook_map.end(); ++it)
  {
    if (it->second == event)
     {
         request = it->first;
         break;
     }
  }
  return request;
}
