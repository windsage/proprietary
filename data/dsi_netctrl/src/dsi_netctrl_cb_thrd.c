/*!
  @file
  dsi_netctrl_cb_thrd.c

  @brief
  implements dsi_netctrl callback processing thread

*/

/*===========================================================================

  Copyright (c) 2010-2015, 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/10   js      created

===========================================================================*/
#include "dsi_netctrli.h"
#include "dsi_netctrl_platform.h"
#include "dsi_netctrl_cb_thrd.h"
#include "dsi_netctrl_mni_cb.h"
#include "ds_cmdq.h"
#include "qdi.h"
#include "dsi_netctrl_mni.h"

/*===========================================================================
                    LOCAL DATA STRUCTURES
===========================================================================*/

/* global queue to hold dsi_netctrl_cb commands */
ds_cmdq_info_t dsi_netctrl_cb_cmdq;

/* if we accumulate more than these many commands
 * ds_cmdq will print a warning msg */
#define DSI_NETCTRL_CB_MAX_CMDS 20

/*===========================================================================
  FUNCTION:  dsi_netctrl_cb_cmd_free
===========================================================================*/
/*!
    @brief
    releases memory for the dsi_netctrl_cb cmd

    @return
    none
*/
/*=========================================================================*/
void dsi_netctrl_cb_cmd_free (ds_cmd_t * cmd, void * data)
{
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;

  DSI_LOG_VERBOSE("dsi_netctrl_cb_cmd_free ENTRY");

  if (NULL == data ||
      NULL == cmd)
  {
    DSI_LOG_FATAL("*** memory corruption : rcvd invalid data ***");
    DSI_LOG_DEBUG("dsi_netctrl_cb_cmd_free EXIT");
    return;
  }

  cmd_buf = (dsi_netctrl_cb_cmd_t *)data;
  /* verify self-reference */
  if (cmd != &cmd_buf->cmd)
  {
    DSI_LOG_FATAL("*** memory corruption : rcvd invalid data ***");
    DSI_LOG_DEBUG("dsi_netctrl_cb_cmd_free EXIT");
    return;
  }

  /* release mem */
  dsi_free(cmd_buf);

  DSI_LOG_VERBOSE("dsi_netctrl_cb_cmd_free EXIT");
  return;
}

void dsiProcessNicmEvents(struct ds_cmd_s * cmd, void * data)
{
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;
  int dsi_iface_id = DSI_INVALID_IFACE;
  int rc = 0;
  dsiNicmNLEvent event = DSI_NICM_INVALID_EV;
  dsiNicmNLEventInfo *dsiNicmEventInfo = NULL;
  dsiNicmRestartState nicm_restart_state = DSI_NICM_POST_RESTART_STATE_INVALID;
  int dsi_event = DSI_EVT_INVALID;

  DSI_GLOBAL_LOCK;

  do {
    if (NULL == cmd || NULL == data)
    {
      DSI_LOG_FATAL("*** memory corruption : rcvd invalid data ***");
      break;
    }

    cmd_buf = (dsi_netctrl_cb_cmd_t *)data;
    /* verify self-refernce */
    if (&cmd_buf->cmd != cmd)
    {
      DSI_LOG_FATAL("*** memory corruption : rcvd invalid data ***");
      break;
    }

    dsiNicmEventInfo = (dsiNicmNLEventInfo *) malloc(sizeof(dsiNicmNLEventInfo));

    if(!dsiNicmEventInfo)
    {
      DSI_LOG_ERROR("%s(): failed to allocate memory", __func__);
      goto bail;
    }

    memset(dsiNicmEventInfo, 0, sizeof(dsiNicmNLEventInfo));

    if(DSI_SUCCESS != dsiTranslateNicmEvent
                      (
                        cmd_buf->cmd_data.data_union.nicm_cb_info.eventInfo,
                        dsiNicmEventInfo,
                        &dsi_event
                      )
      )
    {
      DSI_LOG_ERROR("%s(): Failed to translate event!", __func__);
      goto bail;
    }

    event = dsiNicmEventInfo->event;

    /* DSI_NICM_USER_CMD is sent when NICM is restarted. When we get this event we check
       the NICM state and set dsi_inited flag to false to disallow further DSI operations
       from this client. Once the restart is complete, we reset dsi_inited  back to TRUE */
    if (DSI_NICM_USER_CMD == dsiNicmEventInfo->event)
    {
      nicm_restart_state = dsiNicmEventInfo->cmd_data.nicm_restart_state;
      DSI_LOG_ERROR("DSI_NICM_USER_CMD received, nicm_state [%d]", nicm_restart_state);
      if (nicm_restart_state == DSI_NICM_POST_RESTART_STATE_INIT_DONE)
      {
        set_dsi_init_state(DSI_TRUE);
        DSI_LOG_DEBUG("DSI_NICM_POST_RESTART_STATE_INIT_DONE received, dsi inited");
      }
      else if (nicm_restart_state == DSI_NICM_POST_RESTART_STATE_INIT_START)
      {
        dsi_inited = DSI_FALSE;
        DSI_LOG_DEBUG("DSI_NICM_POST_RESTART_STATE_INIT_START received, dsi not inited");
      }

      goto bail; /* Nothing further to do till next event */
    }
    else if ((DSI_NICM_DOWN_EV == dsiNicmEventInfo->event)
               && (NICM_POST_RESTART_STATE_INIT_ONGOING
                     == dsiNicmEventInfo->cmd_data.nicm_restart_state))
    {
      /* Active call handles prior to NICM restarting will receive DSI_NICM_DOWN_EV
         to trigger cleanup from client side. During this time 'dsi_inited' will be set
         to FALSE so we need to force the cleanup to continue */
      DSI_LOG_DEBUG("NICM_POST_RESTART_STATE_INIT_ONGOING received");
      goto skip_inited_check;
    }

    /* ignore events in uninitialized state (except DSI_NICM_READY_RESP) */
    if (!dsi_inited && DSI_NICM_READY_RESP != event)
    {
      DSI_LOG_DEBUG("dsi_process_nicm_ev: ignoring event [%d] dsi_inited [%d]",
                    event, dsi_inited);
      goto bail;
    }

skip_inited_check:

#ifdef FEATURE_DATA_IWLAN
    if (dsi_config.oemprxy_enabled
          && (event == DSI_NICM_OEMPRXY_IFACE_UP
                || event == DSI_NICM_OEMPRXY_IFACE_DOWN))
    {
      /* If one of these event is sent and iwlan is not defined rc will remain as an error */
      rc = dsiNicmMapReverseLink(dsiNicmEventInfo->link, &dsi_iface_id);
      if (DSI_SUCCESS != rc)
      {
        DSI_LOG_DEBUG("dsi reverse map link failed with err [%d]", rc);
        break;
      }
    }
    else
#endif /* FEATURE_DATA_IWLAN */
    {
      /* map nicm link to dsi iface id */
      rc = dsiNicmMapLink(dsiNicmEventInfo->link, &dsi_iface_id);
      if (DSI_SUCCESS != rc)
      {
        DSI_LOG_DEBUG("dsiNicmMapLink failed with err [%d]", rc);
      }
    }

    //if oemproxy is TRUE no need to post the event
    if(event == DSI_EVT_OEMPRXY_IS_CONN)
    {
      dsi_oemproxy_common_ind(DSI_EVT_OEMPRXY_IS_CONN, dsiNicmEventInfo->link);
      goto bail;
    }
    else if(event == DSI_EVT_OEMPRXY_NO_NET)
    {
      dsi_oemproxy_common_ind(DSI_EVT_OEMPRXY_NO_NET, dsiNicmEventInfo->link);
      goto bail;
    }

    if(DSI_NICM_READY_RESP != event && DSI_NICM_USER_CMD != event)
    {
      /* no need to process further for DSI_NICM_READY_RESP. For the rest of other events,
          need to notify clients accordingly */

      /* cache info associated with this event */
      if (DSI_EVT_INVALID
            != (dsi_event = dsiNicmStoreEventData(dsi_iface_id, dsi_event, dsiNicmEventInfo)))
      {
        DSI_LOG_DEBUG("dsi_process_nicm_ev posting event [%d][%s]",
                      dsi_event, dsi_evt_str[dsi_event]);
        DSI_L2S_FORMAT_MSG(dsi_iface_id, "Posting %s", dsi_evt_str[dsi_event]);
        /* now post the dsi event to it's clients */
        dsiNicmPostEvent(dsi_iface_id, dsi_event, dsiNicmEventInfo);
      }
    }
  } while(0);

bail:
  if(dsiNicmEventInfo)
  {
    free(dsiNicmEventInfo);
  }

  if(cmd_buf && cmd_buf->cmd_data.data_union.nicm_cb_info.eventInfo)
  {
    free(cmd_buf->cmd_data.data_union.nicm_cb_info.eventInfo);
  }

  DSI_GLOBAL_UNLOCK;
}

/*===========================================================================
  FUNCTION:  dsi_netctrl_cb_cmd_exec
===========================================================================*/
/*!
    @brief
    This function is registered as executive function in each command that
    is posted to dsi_netctrl_cb global queue.
    When called, this function further calls appropriate functions based
    on the command types.

    @return
    none
*/
/*=========================================================================*/
void dsi_netctrl_cb_cmd_exec (struct ds_cmd_s * cmd, void * data)
{
  int ret = DSI_ERROR;
  int reti = DSI_SUCCESS;
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;

  DSI_LOG_VERBOSE("dsi_netctrl_cb_cmd_exec: ENTRY");

  do
  {
    if (NULL == cmd ||
        NULL == data)
    {
      DSI_LOG_FATAL("*** memory corruption : rcvd invalid data ***");
      break;
    }

    cmd_buf = (dsi_netctrl_cb_cmd_t *)data;
    /* verify self-refernce */
    if (&cmd_buf->cmd != cmd)
    {
      DSI_LOG_FATAL("*** memory corruption : rcvd invalid data ***");
      break;
    }

    reti = DSI_SUCCESS;
    switch(cmd_buf->cmd_data.type)
    {
    case DSI_NETCTRL_CB_CMD_QMI_WDS_IND:
      dsi_process_wds_ind
        (
          cmd_buf->cmd_data.data_union.wds_ind.wds_hndl,
          cmd_buf->cmd_data.data_union.wds_ind.qmi_service_id,
          cmd_buf->cmd_data.data_union.wds_ind.user_data,
          cmd_buf->cmd_data.data_union.wds_ind.ind_id,
          &cmd_buf->cmd_data.data_union.wds_ind.ind_data
          );
      break;
    case DSI_NETCTRL_CB_CMD_QMI_ASYNC_RSP:
      dsi_process_async_wds_rsp
        (
          cmd_buf->cmd_data.data_union.async_rsp.user_handle,
          cmd_buf->cmd_data.data_union.async_rsp.qmi_service_id,
          cmd_buf->cmd_data.data_union.async_rsp.sys_err_code,
          cmd_buf->cmd_data.data_union.async_rsp.qmi_err_code,
          cmd_buf->cmd_data.data_union.async_rsp.user_data,
          cmd_buf->cmd_data.data_union.async_rsp.rsp_id,
          &cmd_buf->cmd_data.data_union.async_rsp.rsp_data
        );
      break;
    case DSI_NETCTRL_CB_CMD_QMI_QOS_IND:
      dsi_process_qos_ind
        (
          cmd_buf->cmd_data.data_union.qos_ind.qos_hndl,
          cmd_buf->cmd_data.data_union.qos_ind.qmi_service_id,
          cmd_buf->cmd_data.data_union.qos_ind.user_data,
          cmd_buf->cmd_data.data_union.qos_ind.ind_id,
          &cmd_buf->cmd_data.data_union.qos_ind.ind_data
        );
      break;
    case DSI_NETCTRL_CB_CMD_QMI_SYS_IND:
      dsi_process_qmi_sys_ind
        (
          cmd_buf->cmd_data.data_union.qmi_sys_ind.event_id,
          cmd_buf->cmd_data.data_union.qmi_sys_ind.user_data
        );
      break;
    case DSI_NETCTRL_CB_CMD_QMI_WDS_EXT_IND:
      dsi_process_qmi_wds_extended_ind
        (
            &cmd_buf->cmd_data.data_union.wds_ext_ind
        );
      break;
    default:
      DSI_LOG_FATAL("*** memory corruption: rcvd invalid data ***");
      reti = DSI_ERROR;
      break;
    }
    if (DSI_ERROR == reti)
    {
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("dsi_netctrl_cb_cmd_exec: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("dsi_netctrl_cb_cmd_exec: EXIT with err");
  }

}

/*===========================================================================
  FUNCTION:  dsi_qmi_wds_cmd_cb
===========================================================================*/
/*!
    @brief
    callback function registered for asynchronous qmi wds commands
    currently used for
    start_nw_if
    stop_nw_if
    This function will post a command to dsi_netctrl_cb thread for
    later processing.

    @return
    none
*/
/*=========================================================================*/
void dsi_qmi_wds_cmd_cb
(
  int                           user_handle,     /* QDI client ID  */
  qmi_service_id_type           service_id,      /* QMI service ID         */
  int                           sys_err_code,    /* QMI Msg Lib error      */
  int                           qmi_err_code,    /* QMI error              */
  void                         *user_data,       /* Callback context       */
  int                           rsp_id,          /* QMI Msg Lib txn ID     */
  qdi_wds_async_rsp_data_type  *rsp_data         /* QMI Msg Lib txn data   */
)
{
  int ret = DSI_ERROR;
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;

  DSI_LOG_VERBOSE( ">>>dsi_qmi_wds_cmd_cb: ENTRY" );

  do
  {
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: user_handle=0x%x", user_handle);
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: service_id=%d", (int)service_id);
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: sys_err_code=%d", sys_err_code);
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: qmi_err_code=%d", qmi_err_code);
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: rsp_id=%d", rsp_id);
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: rsp_data=%p", rsp_data);

    if (NULL == rsp_data)
    {
      DSI_LOG_FATAL(">>>*** rcvd NULL rsp_data ***");
      break;
    }

    cmd_buf = (dsi_netctrl_cb_cmd_t *)dsi_malloc(sizeof(dsi_netctrl_cb_cmd_t));
    if (NULL == cmd_buf)
    {
      DSI_LOG_ERROR(">>>malloc failed for dsi_netctrl_cb_cmd_t");
      break;
    }

    /* set parameters in our internal structure  */
    cmd_buf->cmd_data.data_union.async_rsp.user_handle = user_handle;
    cmd_buf->cmd_data.data_union.async_rsp.qmi_service_id = service_id;
    cmd_buf->cmd_data.data_union.async_rsp.sys_err_code = sys_err_code;
    cmd_buf->cmd_data.data_union.async_rsp.qmi_err_code = qmi_err_code;
    cmd_buf->cmd_data.data_union.async_rsp.user_data = user_data;
    cmd_buf->cmd_data.data_union.async_rsp.rsp_id = rsp_id;
    /* there are no embedded pointers inside rsp_data structure, so
     * memcpy should be enough to copy everything */
    memcpy(&(cmd_buf->cmd_data.data_union.async_rsp.rsp_data),
           rsp_data,
           sizeof(cmd_buf->cmd_data.data_union.async_rsp.rsp_data));

    /* set broad category to discriminate data, at the end
       dsc_cmd_q is going to call the execute_f with data */
    cmd_buf->cmd_data.type = DSI_NETCTRL_CB_CMD_QMI_ASYNC_RSP;

    /* prepare ds_cmd_t required by ds_cmdq */
    cmd_buf->cmd.execute_f = dsi_netctrl_cb_cmd_exec;
    cmd_buf->cmd.free_f = dsi_netctrl_cb_cmd_free;
    /* self pointer. this will be freed later */
    cmd_buf->cmd.data = cmd_buf;

    /* post command to global dsi_netctrl_cb queue */
    DSI_LOG_VERBOSE(">>>posting cmd [%p] to dsi_netctrl_cb queue",
                  &cmd_buf->cmd);
    ds_cmdq_enq(&dsi_netctrl_cb_cmdq, &cmd_buf->cmd);

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE( ">>>dsi_qmi_wds_cmd_cb: EXIT with suc" );
  }
  else
  {
    DSI_LOG_VERBOSE( ">>>dsi_qmi_wds_cmd_cb: EXIT with err" );
  }

}

/*===========================================================================
  FUNCTION:  dsi_qmi_wds_ind_cb
===========================================================================*/
/*!
    @brief
    callback function registered for wds indications.
    This function will post a command to separate dsi_netctrl_cb thread
    for later processing.

    @return
    none
*/
/*=========================================================================*/
void dsi_qmi_wds_ind_cb
(
  qmi_client_type                   wds_hndl,
  qmi_service_id_type               sid,
  void                             *user_data,
  int                               ind_id,
  dsi_qmi_wds_indication_data_type *ind_data
)
{
  int ret = DSI_ERROR;
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;

  int count;
  int reti = DSI_SUCCESS;

  DSI_LOG_VERBOSE(">>>dsi_qmi_wds_ind_cb: ENTRY");

  do
  {
    if (NULL == ind_data)
    {
      DSI_LOG_FATAL("*** rcvd invalid ind_data ***");
      break;
    }

    cmd_buf = (dsi_netctrl_cb_cmd_t *)dsi_malloc(sizeof(dsi_netctrl_cb_cmd_t));
    if (NULL == cmd_buf)
    {
      DSI_LOG_FATAL("*** malloc failed for dsi_netctrl_cb cmd ***");
      break;
    }

    DSI_LOG_VERBOSE(">>>received wds_ind with wds_hndl [%p] "
                    "sid [%d] user_data [%p] ind_id [%d] ind_data [%p]",
                    wds_hndl, sid, user_data, ind_id, ind_data);
    /* set parameters in our internal structure  */
    cmd_buf->cmd_data.data_union.wds_ind.wds_hndl = wds_hndl;
    cmd_buf->cmd_data.data_union.wds_ind.qmi_service_id = sid;
    cmd_buf->cmd_data.data_union.wds_ind.user_data = user_data;
    cmd_buf->cmd_data.data_union.wds_ind.ind_id = ind_id;

    switch(ind_id)
    {
      case QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01:
        {
          DSI_LOG_VERBOSE("process QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01");

          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.embms_activate_status),
                 &(ind_data->embms_activate_status),
                 sizeof(ind_data->embms_activate_status));
        }
        break;

      case QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01:
        {
          DSI_LOG_VERBOSE("process QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01");

          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.embms_deactivate_status),
                 &(ind_data->embms_deactivate_status),
                 sizeof(ind_data->embms_deactivate_status));
        }
        break;

      case QMI_WDS_EMBMS_TMGI_ACT_DEACT_IND_V01:
        {
          DSI_LOG_VERBOSE("process QMI_WDS_EMBMS_TMGI_ACT_DEACT_IND_V01");

          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.embms_act_deact_status),
                 &(ind_data->embms_act_deact_status),
                 sizeof(ind_data->embms_act_deact_status));
        }
        break;

      case QMI_WDS_EMBMS_TMGI_LIST_IND_V01:
        {
          DSI_LOG_VERBOSE("process QMI_WDS_EMBMS_TMGI_LIST_IND_V01");

          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.embms_list),
                 &(ind_data->embms_list),
                 sizeof(ind_data->embms_list));
        }
        break;

      case QMI_WDS_EMBMS_CONTENT_DESC_CONTROL_IND_V01:
        {
          DSI_LOG_VERBOSE("process QMI_WDS_EMBMS_CONTENT_DESC_CONTROL_IND_V01");

          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.embms_content_desc_control),
                 &(ind_data->embms_content_desc_control),
                 sizeof(ind_data->embms_content_desc_control));
        }
        break;

      case QMI_WDS_EMBMS_SAI_LIST_IND_V01:
        {
          DSI_LOG_VERBOSE("process QMI_WDS_EMBMS_SAI_LIST_IND_V01");
          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.sai_list),
                 &(ind_data->sai_list),
                 sizeof((cmd_buf->cmd_data.data_union.wds_ind.ind_data.sai_list)));
        }
        break;

      case QMI_WDS_EMBMS_TUNEAWAY_INFO_IND_V01:
        {
          DSI_LOG_VERBOSE("process QMI_WDS_EMBMS_TUNEAWAY_INFO_IND_V01");
          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.tuneaway_info),
                 &(ind_data->tuneaway_info),
                 sizeof((cmd_buf->cmd_data.data_union.wds_ind.ind_data.tuneaway_info)));
        }
        break;

      default:
        {
          /* there are no embedded pointers inside ind_data structure, so
           * memcpy should be enough to copy everything */
          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data),
                 ind_data,
                 sizeof(cmd_buf->cmd_data.data_union.wds_ind.ind_data));
        }
        break;
    }

    /* set broad category to discriminate data, at the end
       dsc_cmd_q is going to call the execute_f with data */
    cmd_buf->cmd_data.type = DSI_NETCTRL_CB_CMD_QMI_WDS_IND;

    /* prepare ds_cmd_t required by ds_cmdq */
    cmd_buf->cmd.execute_f = dsi_netctrl_cb_cmd_exec;
    cmd_buf->cmd.free_f = dsi_netctrl_cb_cmd_free;
    /* self pointer. this will be freed later */
    cmd_buf->cmd.data = cmd_buf;

    /* post command to global dsi_netctrl_cb queue */
    DSI_LOG_VERBOSE(">>>posting cmd [%p] to dsi_netctrl_cb queue",
                  &cmd_buf->cmd);
    ds_cmdq_enq(&dsi_netctrl_cb_cmdq, &cmd_buf->cmd);

    if(DSI_SUCCESS == reti)
    {
      ret = DSI_SUCCESS;
    }
    else
    {
      break;
    }
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE(">>>dsi_qmi_wds_ind_cb: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE(">>>dsi_qmi_wds_ind_cb: EXIT with err");
  }

}

/*===========================================================================
  FUNCTION:  dsi_netctrl_cb_deinit
===========================================================================*/
/*!
    @brief
    This function must be called at init time

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
void dsi_netctrl_cb_deinit()
{
  DSI_LOG_DEBUG("dsi_netctrl_cb_deinit ENTRY");
  if (0 != ds_cmdq_deinit(&dsi_netctrl_cb_cmdq))
  {
    DSI_LOG_ERROR("*** could not deinit dsi_netctrl_cb_cmdq ***");
  }
  DSI_LOG_DEBUG("dsi_netctrl_cb_deinit EXIT");
}

/*===========================================================================
  FUNCTION:  dsi_netctrl_cb_init
===========================================================================*/
/*!
    @brief
    This function must be called at init time

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_netctrl_cb_init()
{
  int rc;
  int ret = DSI_SUCCESS;

  DSI_LOG_DEBUG("dsi_netctrl_cb_init ENTRY");
  /* init ds_cmdq queue */
  rc = ds_cmdq_init(&dsi_netctrl_cb_cmdq, DSI_NETCTRL_CB_MAX_CMDS);
  if (0 != rc)
  {
    DSI_LOG_FATAL("*** ds_cmdq_init failed with err [%d] ***", rc);
    ret = DSI_ERROR;
  }

  DSI_LOG_DEBUG("dsi_netctrl_cb_init EXIT");
  return ret;
}
